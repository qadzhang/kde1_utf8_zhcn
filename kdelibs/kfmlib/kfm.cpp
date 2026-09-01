/* This file is part of the KDE libraries
    Copyright (C) 1997 Torben Weis (weis@kde.org)

    This library is free software; you can redistribute it and/or
    modify it under the terms of the GNU Library General Public
    License as published by the Free Software Foundation; either
    version 2 of the License, or (at your option) any later version.

    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    Library General Public License for more details.

    You should have received a copy of the GNU Library General Public License
    along with this library; see the file COPYING.LIB.  If not, write to
    the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
    Boston, MA 02111-1307, USA.
*/
#include <stdlib.h>
#include <stdio.h>
#include <signal.h>
#include <unistd.h>

#include <qdir.h>
#include <qstring.h>
#include <qmessagebox.h>
#include <qapplication.h>

#include "kfm.h"
#include <kapp.h>
#include <kstring.h>

QString displayName()
{
  // note: We can not rely on DISPLAY. If KDE is started by
  // KDM, DISPLAY will be something like ":0", but this is
  // not unique if we start KDE several times in a network

  QString d = QString(getenv("DISPLAY"));

  int i = d.find( ':' );
  if ( i != -1 )
    d[i] = '_';
  if (i==0)
    {
      // we are running local, so add the hostname
      char name[25];

      if (gethostname(name, 25) == 0)
	d = name + d;
    }

  if ( d.find( '.' ) == -1 )
    d += ".0";

  return d;
}

// ┌─ [KDE1 Revival 2026] kfm 自愈辅助（供 KFM::init 的非阻塞重启使用）
// │  What : spawnKfmDaemon 按 $KDEDIR/bin 解析并后台拉起 kfm -d；
// │        waitForKfmDaemon 以 100ms 步长轮询 pid 文件至多 3 秒
// │  Why  : 见 KFM::init 头注释——kfm 掉线后桌面菜单/图标全失效，
// │        需要无 UI 冻结的自动恢复；不用 fork+execl 是为避开库内 fork
// │        与宿主应用 SIGCHLD 回收逻辑的竞态（shell 后台化即 DontCare）
// │  How  : system("/…/kfm -d &") 立即返回；轮询读 pid 文件并 kill(pid,0)
// │        确认存活后才放行 init 重试
// └───────────────────────────────────────────────────────────────────
static void spawnKfmDaemon()
{
    QString cmd = "kfm -d &";	// 退回 PATH（startkde 包装环境 PATH 已含）
    const char *kdedir = getenv( "KDEDIR" );
    if ( kdedir != 0L && kdedir[0] != 0 )
    {
	QString abs = QString( kdedir ) + "/bin/kfm";
	if ( access( abs.data(), X_OK ) == 0 )
	    cmd = abs + " -d &";
    }
    if (!KFM::isSilent()) tqWarning("KFM: 未运行，正在自动重启 kfm 守护进程");
    system( cmd.data() );
}

static void waitForKfmDaemon( const QString &pidFile )
{
    for ( int i = 0; i < 30; i++ )
    {
	FILE *f = fopen( pidFile.data(), "rb" );
	if ( f != 0L )
	{
	    char buf[64];
	    buf[0] = 0;
	    char *p = fgets( buf, 63, f );
	    fclose( f );
	    int pid = p ? atoi( buf ) : 0;
	    if ( pid > 0 && kill( pid, 0 ) == 0 )
		return;		// kfm 已就绪
	}
	usleep( 100 * 1000 );	// 100ms
    }
    if (!KFM::isSilent()) tqWarning("KFM: 等待 kfm 守护进程就绪超时（3 秒）");
}

KFM::KFM()
{
    flag = 0;
    ok = FALSE;
    ipc = 0L;
    allowRestart = FALSE;
    modal_hack_widget = 0;

    init();
}

KFM::~KFM()
{
    if ( ipc )
	delete ipc;
}              


bool KFM::download(const QString & src, QString & target){
  KURL u (src);
  if (u.isLocalFile() && !u.hasSubProtocol()){
    // file protocol. We do not need the network
    target = u.path();
    return true;
  }
  KFM* kfm = new KFM;
  bool result = kfm->downloadInternal(src,target);
  delete kfm;
  return result;
}

QStrList* KFM::tmpfiles = 0;

void KFM::removeTempFile(const QString & name){
  if (!tmpfiles)
    return;
  if (tmpfiles->contains(name)){
    unlink(name);
    tmpfiles->remove(name);
  }
}

bool KFM::downloadInternal(const QString & src, QString & target){
  if (target.isEmpty()){
    target = tmpnam(0);
    if (!tmpfiles)
      tmpfiles = new QStrList;
    tmpfiles->append(qstrdup(target.data()));
  }
  download_state = true; // success

  /* this is a bit tricky. We use a faked modal dialog to be able to
     process the download syncronious. For the user it will look
     (almost) as if the kfm-dialog is the modal dialog of your
     application. After show() we will also enter a local event loop
     within Qt. The modal_hack_widget will be hidden and destroyed in
     the finish slot. This will implictly exit the local event loop
     in addition (Matthias) 
  */
  modal_hack_widget = new QWidget(0,0,WType_Modal);
  modal_hack_widget->setGeometry(-10,-10,2,2);
  copy(src, target);
  modal_hack_widget->show();
  qApp->enter_loop();
  return download_state; 
}


void KFM::init()
{
    QString file = KApplication::localkdedir() + "/share/apps/kfm/pid";
    file += displayName();

    // ┌─ [KDE1 Revival 2026] kfm 掉线自愈（非阻塞重启）+ 错误非模态化
    // │  What : kfm 未运行/pid 失效时自动拉起 kfm -d 并短轮询等待，
    // │        替换 1999 年 allowRestart 门控 + system("kfm -d &")+sleep(10)
    // │        的阻塞重启；magic 文件异常改 stderr 告警（原为模态弹窗）
    // │  Why  : kfm 一旦崩溃/退出，桌面所有 K 菜单启动、桌面图标刷新全部
    // │        静默失效（用户只能注销重登）。原实现 sleep(10) 冻结调用方
    // │        UI，且 allowRestart 从未被任何调用方置位——自愈路径实际是
    // │        死代码；模态弹窗则会把 kpanel 冻在嵌套事件循环里
    // │  Who  : kpanel（K 菜单/面板按钮）、krootwm（桌面图标双击）、
    // │        任何经 KFM 类发起 IPC 的调用方
    // │  When : 每个 KFM 对象构造（init）且 kfm 不在运行时，至多自愈一次
    // │  Where: kdelibs/kfmlib/kfm.cpp；拉起路径按 $KDEDIR/bin 解析绝对
    // │        路径（与 kcmdisplay 的 KProcess 拉起修复同思路）
    // │  How  : 伪代码——
    // │        1. pid 文件缺失或 pid 不存活 且 本对象未自愈过：
    // │             fork+exec kfm -d（不阻塞 shell，DontCare 语义）
    // │             → 最多轮询 3 秒等 pid 文件出现且 pid 存活
    // │             → flag=1 防递归，重走 init 一次
    // │        2. magic 文件缺失/损坏：tqWarning 到 stderr 后返回
    // │           （不再 QMessageBox——调用方在点击路径上不能被模态框冻住）
    // └───────────────────────────────────────────────────────────────────
    // Try to open the pid file
    FILE *f = fopen( file.data(), "rb" );
    if ( f == 0L )
    {
	// Did we already try to start a new kfm ?
	if ( flag == 0 )
	{
	    // Try to start a new kfm (self-heal, non-blocking)
	    flag = 1;
	    spawnKfmDaemon();
	    waitForKfmDaemon( file );
	    init();
	    return;
	}

	if (!silent) tqWarning("ERROR: KFM is not running");
	return;
    }

    // Read the PID
    char buffer[ 1024 ];
    buffer[0] = 0;
    fgets( buffer, 1023, f );
    int pid = atoi( buffer );
    if ( pid <= 0 )
    {
	if (!silent) tqWarning("ERROR: Invalid PID");
	fclose( f );
	return;
    }

    // Is the PID ok ?
    if ( kill( pid, 0 ) != 0 )
    {
	// Did we already try to start a new kfm ?
	if ( flag == 0 )
	{
	    flag = 1;
	    // Try to start a new kfm (self-heal, non-blocking)
	    fclose( f );
	    spawnKfmDaemon();
	    waitForKfmDaemon( file );
	    init();
	    return;
	}

	if (!silent) tqWarning("ERROR: KFM crashed");
	fclose( f );
	return;
    }

    // Read the socket's name
    buffer[0] = 0;
    fscanf(f, "%s", buffer); 
    fclose( f );
    char * slot = strdup( buffer );
    if ( slot == (void *) 0 )
    {
	if (!silent) tqWarning("ERROR: Invalid Slot");
	return;
    }
    
    // Connect to KFM
    ipc = new KfmIpc( slot );
    free(slot);

    connect( ipc, SIGNAL( finished() ), this, SLOT( slotFinished() ) );
    connect( ipc, SIGNAL( error( int, const char* ) ),
	     this, SLOT( slotError( int, const char* ) ) );
    connect( ipc, SIGNAL( dirEntry( const char*, const char*, const char*, const char*, const char*, int ) ),
	     this, SLOT( slotDirEntry( const char*, const char*, const char*, const char*, const char*, int ) ) );

    // Read the password
    QString fn = KApplication::localkdedir() + "/share/apps/kfm/magic";
    f = fopen( fn.data(), "rb" );
    if ( f == 0L )
    {
	// [KDE1 Revival 2026] 模态弹窗改 stderr 告警：本函数运行在 kpanel
	// 点击路径上，模态框会把面板冻在嵌套事件循环里（且无助于恢复）
	if (!silent) tqWarning("KFM Error: You dont have the file %s\n"
			       "Could not do Authorization", fn.data());
	return;
    }
    char *p = fgets( buffer, 1023, f );
    fclose( f );
    if ( p == 0L )
    {
	if (!silent) tqWarning("KFM Error: The file %s is corrupted\n"
			       "Could not do Authorization", fn.data());
	return;
    }

    ipc->auth( buffer );
    
    ok = TRUE;
}

void KFM::refreshDesktop()
{
    if ( !test() )
	return;
    
    ipc->refreshDesktop();
}

void KFM::sortDesktop()
{
    if ( !test() )
	return;
    
    ipc->sortDesktop();
}

void KFM::configure()
{
  if ( !test() )
    return;
  ipc->configure();
}

void KFM::openURL()
{
    if ( !test() )
	return;
    
    ipc->openURL( "" );
}

void KFM::openURL( const char *_url )
{
    if ( !test() )
	return;
    
    ipc->openURL( _url );
}

void KFM::list( const char *_url )
{
    if ( !test() )
	return;
    
    ipc->list( _url );
}

void KFM::refreshDirectory( const char *_url )
{
    if ( !test() )
	return;
    
    ipc->refreshDirectory( _url );
}

void KFM::openProperties( const char *_url )
{
    if ( !test() )
	return;
    
    ipc->openProperties( _url );
}

void KFM::exec( const char *_url, const char *_binding )
{
    if ( !test() ) {
	return;
    }
    ipc->exec( _url, _binding );
}

void KFM::copy( const char *_src, const char *_dest )
{
    if ( !test() )
	return;
    
    ipc->copy( _src, _dest );
}

void KFM::move( const char *_src, const char *_dest )
{
    if ( !test() )
	return;
    
    ipc->move( _src, _dest );
}

void KFM::copyClient( const char *_src, const char *_dest )
{
    if ( !test() )
	return;
    
    ipc->copyClient( _src, _dest );
}

void KFM::moveClient( const char *_src, const char *_dest )
{
    if ( !test() )
	return;
    
    ipc->moveClient( _src, _dest );
}

void KFM::selectRootIcons( int _x, int _y, int _w, int _h, bool _add )
{
    //tqWarning( "KFM call: selectRootIcons");
    if ( !test() )
	return;
    //tqWarning( "KFM doing call");
    
    ipc->selectRootIcons( _x, _y, _w, _h, _add );
}

void KFM::slotFinished()
{
  if (modal_hack_widget){
    modal_hack_widget->close(true);
    modal_hack_widget = 0;
    qApp->exit_loop();
  }
  emit finished();
}

bool KFM::test()
{
    if ( ( ipc == 0L || !ipc->isConnected() ) && allowRestart )
    {
	tqWarning( "*********** KFM crashed **************" );
	if ( ipc )
	    delete ipc;
	
	ipc = 0L;
	flag = 0;
	ok = FALSE;

	tqWarning( "KFM recovery" );
	init();
	tqWarning( "KFM recovery done" );
    }

    if ( ipc == 0L )
	tqWarning( "KFM NOT READY");
    
    return ( ipc==0L?false:true );
}

void KFM::allowKFMRestart( bool _allow )
{
    allowRestart = _allow;
}

bool KFM::isKFMRunning()
{
    if ( ipc == 0L ) return FALSE;
    if ( ipc->isConnected() )
	return TRUE;
    return FALSE;
}

void KFM::slotError( int _kerror, const char *_text )
{
  download_state = false;
  emit error( _kerror, _text );
}

void KFM::slotDirEntry(const char* _name, const char* _access, const char* _owner,
		  const char* _group, const char* _date, int _size)
{
  entry.name = _name;
  entry.access = _access;
  entry.owner = _owner;
  entry.group = _group;
  entry.date = _date;
  entry.size = _size;
  emit dirEntry( entry );
}

//static
void KFM::setSilent(bool _silent) { silent = _silent; }
bool KFM::silent = false;

DlgLocation::DlgLocation( const char *_text, const char* _value, QWidget *parent )
        : QDialog( parent, 0L, TRUE )
{

    QLabel *label = new QLabel( _text , this );
    label->adjustSize(); // depends on the text length
    label->move(10,10);

    edit = new QLineEdit( this, 0L );
    connect( edit, SIGNAL(returnPressed()), SLOT(accept()) );

    ok = new QPushButton( i18n("OK"), this );
    connect( ok, SIGNAL(clicked()), SLOT(accept()) );
    ok->adjustSize();

    cancel = new QPushButton( i18n("Cancel"), this );
    connect( cancel, SIGNAL(clicked()), SLOT(reject()) );
    cancel->adjustSize();

    edit->setText( _value );
    edit->setFocus();

    setMinimumSize ( 200, 40+edit->height()+10+ok->height()+10 );
    setGeometry( x(), y(), label->width()+20, 110 );
}

void DlgLocation::resizeEvent(QResizeEvent *e)
{
    QDialog::resizeEvent(e);
    int w = rect().width();
    int h = rect().height();
    edit->setGeometry (10, 40, w-20, 20);
    ok->move( 10, h-10-ok->height() );
    cancel->move( w-10-cancel->width(), h-10-cancel->height() );
}

#include "kfm.moc"
