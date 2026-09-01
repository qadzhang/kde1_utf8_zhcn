// kpanel. Part of the KDE project.
//
// Copyright (C) 1996,97 Matthias Ettrich
//

//   Modified for the KDE1 Revival Project, 2026
//   Maintainer: <维护者姓名> <邮箱>
//   Modifications written with GLM-5.3 (Z.ai)
//   [2026-08-29] 试验过 show() 前登记 KWM_DOCKWINDOW 求贴边，因主窗口整体
//   消失已回退；面板贴边问题已定位待续（详见 show() 处标注）

#include "kpanel.h"
#include "snitray.h"
#include <qapp.h>
#include <kwmmapp.h>
#include <qdir.h>
#include <qmsgbox.h>
#include <qfile.h>
#include <qfileinfo.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "kpanel_version.h"
#include <X11/keysym.h>

#include <dirent.h>
#include <sys/stat.h>

kPanel *the_panel;
SNITray *the_snitray; /* [KDE1 Revival 2026] SNI 托盘 host */
int o_argc;
char ** o_argv;

void restart_the_panel(){
  QApplication::exit();
  execvp(o_argv[0],  o_argv);
  exit(1);
}

#include "kpanel.moc"


class MyApp:public KWMModuleApplication {
public:
  MyApp( int &argc, char **argv, const QString& rAppName );
  virtual bool x11EventFilter( XEvent * );
};

MyApp::MyApp(int &argc, char **argv , const QString& rAppName):
  KWMModuleApplication(argc, argv, rAppName){
}

bool MyApp::x11EventFilter( XEvent * ev){
  /* [KDE1 Revival 2026] XEmbed 系统托盘停靠申请转发：
     _NET_SYSTEM_TRAY_OPCODE data.l[1]==0 即 SYSTEM_TRAY_REQUEST_DOCK */
  if (ev->type == ClientMessage &&
      ev->xclient.message_type == the_panel->tray_opcode_atom &&
      ev->xclient.data.l[1] == 0){
    the_panel->embedTrayClient((Window) ev->xclient.data.l[2]);
    return True;
  }

  /* [KDE1 Revival 2026] SNI 托盘图标点击 → 转发 SNI 交互方法：
     左键 Activate、右键 ContextMenu、其余 SecondaryActivate。
     这些 24x24 窗口是 kpanel 经 XCreateSimpleWindow 自建的（非 TQt 窗口），
     事件吞掉（return True）防止进入 Qt 事件循环后无人认领。 */
  if (ev->type == ButtonPress && the_snitray){
    SNIClient *sni = the_snitray->clientByWindow(ev->xbutton.window);
    if (sni){
      the_snitray->sendClick(sni, ev->xbutton.button,
                             ev->xbutton.x_root, ev->xbutton.y_root);
      return True;
    }
  }

  /* [KDE1 Revival 2026] dock 图标右键（kpanel 对 dock 窗口 select 了
     ButtonPress 副本事件）：转投 Qt 事件循环内弹出"退出应用"菜单。
     X 原始事件里不能直接弹 Qt 菜单（嵌套循环风险），singleShot(0) 转移。
     [KDE1 Revival 2026] 改为 Shift+右键触发——普通右键按现代系统托盘
     语义留给嵌入应用自己的菜单（kpanel 吞掉普通右键会让 fcitx5 等
     XEmbed 托盘永远打不开右键菜单）。 */
  if (ev->type == ButtonPress &&
      ev->xbutton.button == 3 /* RightButton */ &&
      (ev->xbutton.state & ShiftMask) &&
      the_panel->dockWindowAt(QPoint(ev->xbutton.x_root, ev->xbutton.y_root)) != None) {
    the_panel->pendingDockContextWindow =
      the_panel->dockWindowAt(QPoint(ev->xbutton.x_root, ev->xbutton.y_root));
    XAllowEvents(qt_xdisplay(), AsyncPointer, CurrentTime);
    QTimer::singleShot(0, the_panel, SLOT(dockAppContextMenuSlot()) );
    return True; /* 该事件不再往下传 */
  }

  if (ev->xany.window != None &&
      the_panel->parentOfSwallowed(ev->xany.window)){
    if (ev->type == ButtonPressMask){
      XAllowEvents(qt_xdisplay(), SyncPointer, CurrentTime);
      the_panel->parentOfSwallowed(ev->xany.window)->grabMouse();
    }
    ev->xany.window = the_panel->parentOfSwallowed(ev->xany.window)->winId();
  }

  return KWMModuleApplication::x11EventFilter(ev);
}

void testDir( const char *_name )
{
  DIR *dp;
  QString c = KApplication::localkdedir();
  c += _name;
  dp = opendir( c.data() );
  if ( dp == NULL )
    ::mkdir( c.data(), S_IRWXU );
  else
    closedir( dp );
}

void copyFiles( QString source, QString dest )
{
  char data[1024];
  QFile in(source);
  QFile out(dest);
  if( !in.open(IO_ReadOnly) || !out.open(IO_WriteOnly) )
    return;
  int len;
  while( (len = in.readBlock(data, 1024)) > 0 ) {
    if( out.writeBlock(data,len) < len ) {
      len = -1;
      break;
    }
  }
  out.close();
  in.close();
  if( len == -1 ) { // abort and remove destination file
    QFileInfo fi(dest);
    fi.dir().remove(dest);
  }
}

int main( int argc, char ** argv ){

  o_argc = argc;
  o_argv = new char*[o_argc + 2];
  int v;

  for (v=0; v<o_argc; v++) o_argv[v] = argv[v];
  o_argv[v] = 0;

  MyApp myapp( argc, argv, "kpanel" );
  bool use_kwm = true;

  for (v=1; v<argc; v++){
    if (QString("-version")==argv[v]){
      printf(KPANEL_VERSION);
      printf("\n");
      printf("Copyright (C) 1997 Matthias Ettrich (ettrich@kde.org)\n\n");
      exit(0);
    }
    if (QString("-no-KDE-compliant-window-manager")==argv[v]){
      use_kwm = false;
      break;
    }

    printf("Desktop Panel for the KDE Desktop Environment\n");
    printf("Permitted arguments:\n");
    printf("-help : displays this message\n");
    printf("-version : displays the version number\n");
    printf("-no-KDE-compliant-window-manager : force startup without \n");
    printf("           initializing the module communication \n");
    printf("And all KApplication and Qt-toolkit arguments.\n");
    exit(0);
  }

  if (use_kwm){
    if (!KWM::isKWMInitialized()){
      printf("kpanel: waiting for windowmanager\n");
      while (!KWM::isKWMInitialized()) sleep(1);
      sleep(1);
      printf("kpanel: ok, commencing initialization\n");
    }
  }

  // create $HOME/.kde/share/apps/kpanel/applnk
  testDir( "" );
  testDir( "/share" );
  testDir( "/share/config" );
  testDir( "/share/apps" );
  testDir( "/share/apps/kpanel" );
  testDir( "/share/apps/kpanel/applnk" );
  // create $HOME/.kde/share/applnk
  testDir( "/share/applnk" );
  // create default $HOME/.kde/share/applnk/.directory file if there is none
  QString src_path = KApplication::kde_datadir();
  src_path += "/kpanel/default/personal_directory";
  QString dest_path = KApplication::localkdedir();
  dest_path += "/share/applnk/.directory";
  QFileInfo fi(dest_path);
  if( !fi.exists() ) {
    copyFiles(src_path, dest_path);
  }

  the_panel = new kPanel(&myapp);
  the_panel->setupSystemTray(); /* [KDE1 Revival 2026] XEmbed 托盘 manager 上线 */
  /* [KDE1 Revival 2026] SNI（DBus StatusNotifier）host：fcitx5 5.x 等
     现代程序的默认托盘路径 */
  the_snitray = new SNITray(the_panel->dockAreaWidget());
  if (the_snitray->init())
      QObject::connect(the_snitray, SIGNAL(clientsChanged()),
                       the_panel, SLOT(dockClientsChanged()));
  the_panel->connect(&myapp, SIGNAL(init()),
		     SLOT(kwmInit()));
  the_panel->connect(&myapp, SIGNAL(windowAdd(Window)),
		     SLOT(windowAdd(Window)));
  the_panel->connect(&myapp, SIGNAL(dialogWindowAdd(Window)),
		     SLOT(dialogWindowAdd(Window)));
  the_panel->connect(&myapp, SIGNAL(windowRemove(Window)),
		     SLOT(windowRemove(Window)));
  the_panel->connect(&myapp, SIGNAL(windowChange(Window)),
		     SLOT(windowChange(Window)));
  the_panel->connect(&myapp, SIGNAL(windowActivate(Window)),
		     SLOT(windowActivate(Window)));
  the_panel->connect(&myapp, SIGNAL(windowIconChanged(Window)),
		     SLOT(windowIconChanged(Window)));
  the_panel->connect(&myapp, SIGNAL(windowRaise(Window)),
		     SLOT(windowRaise(Window)));
  the_panel->connect(&myapp, SIGNAL(desktopChange(int)),
		     SLOT(kwmDesktopChange(int)));
  the_panel->connect(&myapp, SIGNAL(desktopNameChange(int, QString)),
		     SLOT(kwmDesktopNameChange(int, QString)));
  the_panel->connect(&myapp, SIGNAL(desktopNumberChange(int)),
		     SLOT(kwmDesktopNumberChange(int)));
  the_panel->connect(&myapp, SIGNAL(commandReceived(QString)),
		     SLOT(kwmCommandReceived(QString)));
  the_panel->connect(&myapp, SIGNAL(dockWindowAdd(Window)),
		     SLOT(dockWindowAdd(Window)));
  the_panel->connect(&myapp, SIGNAL(dockWindowRemove(Window)),
		     SLOT(dockWindowRemove(Window)));

//   the_panel->connect(&myapp, SIGNAL(playSound(QString)),
// 		     SLOT(playSound(QString)));

  the_panel->connect(&myapp, SIGNAL(kdisplayPaletteChanged()),
		     SLOT(kdisplayPaletteChanged()));

  the_panel->connect( &myapp, SIGNAL( kdisplayStyleChanged() ), SLOT( restart() ) );


  myapp.setMainWidget(the_panel);

  // connect to kwm as docking module
  myapp.connectToKWM(true);
  // [KDE1 Revival 2026] 曾试验 show() 前登记 KWM_DOCKWINDOW 以求 kwm 免管理、
  // 面板贴边——但主窗口随之整体消失（机理已明：KWM_DOCKWINDOW 是"面板
  // 停靠图标"协议，kwm 收到后经 dockWindowAdd 信号把窗口嵌进 kpanel 自家
  // 停靠区，面板窗口因此"消失"），已回退。
  // [2026-09-01] 改用 KDE1 原生的 KWM_WIN_DECORATION=noDecoration 客户自
  // 声明（kwm 的 manage() 两条路径——启动期初始扫描与 MapRequest——都会
  // 读取并优先尊重客户声明）：面板/任务栏不再被加框，配合 kwm 侧的
  // "贴边窗口豁免钳制"，面板稳定贴底不再被智能摆位推离屏幕边缘。
  // winId() 强制先创建 X 窗口，确保属性先于任何映射/扫描存在。
  KWM::setDecoration( the_panel->winId(), KWM::noDecoration );
  the_panel->launchSwallowedApplications();
  the_panel->show();
  the_panel->raise();
  myapp.syncX();
  myapp.processEvents();
  the_panel->parseMenus();
  XSelectInput(qt_xdisplay(), qt_xrootwin(),
	       KeyPressMask);
  while (1)
    myapp.exec();
  return 0;
}
