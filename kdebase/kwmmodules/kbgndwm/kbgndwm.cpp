/*
 * kbgndwm.cpp. Part of the KDE project.
 *
 * Copyright (C) 1997 Martin Jones
 *               1998 Matej Koss
 *
 */

//----------------------------------------------------------------------------

#include <qpmcache.h>

#include <kwm.h>
#include <kprocess.h>
#include <kstring.h>
#include <qapp.h> /* [KDE1 Revival 2026] qt_xdisplay/qt_xrootwin（分辨率监视用） */
#include "kbgndwm.h"
#include "config-kbgndwm.h"

#include "kbgndwm.moc"

//----------------------------------------------------------------------------

KBGndManager::KBGndManager( KWMModuleApplication * )
  : QWidget(0, "", 0)
{
  QPixmapCache::clear();

  docked = false;

  this->setGeometry( -100,-100, 10, 10);

  readSettings();

  // [KDE1 Revival 2026] 分辨率热变更监视起表：以 X 级真值（XGetGeometry）
  // 初始化基准几何，之后每 2 秒 timerEvent 比对（机制与边界见 kbgndwm.h
  // 的 5W1H 注释）。放在 readSettings 之后、applyDesktop 之前，首次
  // 渲染即用真值尺寸
  {
    Window root_ret; int x_ret, y_ret;
    unsigned int rw, rh, border, depth;
    if ( XGetGeometry( qt_xdisplay(), qt_xrootwin(), &root_ret,
		       &x_ret, &y_ret, &rw, &rh, &border, &depth ) ) {
      last_root_w = (int) rw;
      last_root_h = (int) rh;
    } else {
      last_root_w = QApplication::desktop()->width();
      last_root_h = QApplication::desktop()->height();
    }
    startTimer( 2000 );
  }

  desktops = new KBackground [ MAX_DESKTOPS ];

  for ( int i = 0; i < MAX_DESKTOPS; i++ )
    desktops[i].readSettings( i, oneDesktopMode, desktop );

  if ( oneDesktopMode )
    current = desktop;
  else
    current = KWM::currentDesktop() - 1;

  // popup menu for right mouse button
  popup_m = new QPopupMenu();
  CHECK_PTR( popup_m );
  popup_m->setCheckable( TRUE );

  popup_m->insertItem(i18n("Display Settings"), this, SLOT(displaySettings()));
  popup_m->insertItem(i18n("Background Settings"), this, SLOT(settings()));
  popup_m->insertSeparator();
  o_id = popup_m->insertItem(i18n("Common Background"), this, SLOT(toggleOneDesktop()));
  popup_m->insertSeparator();
  popup_m->insertItem(i18n("Undock"), this, SLOT(setUndock()));

  popup_m->setItemChecked( o_id, oneDesktopMode );

  // popup menu for display modes
  modePopup = new QPopupMenu();
  CHECK_PTR( modePopup );

  modePopup->insertItem(i18n("Tiled") );
  modePopup->insertItem(i18n("Mirrored") );
  modePopup->insertItem(i18n("CenterTiled") );
  modePopup->insertItem(i18n("Centred") );
  modePopup->insertItem(i18n("CentredBrick") );
  modePopup->insertItem(i18n("CentredWarp") );
  modePopup->insertItem(i18n("CentredMaxpect") );
  modePopup->insertItem(i18n("SymmetricalTiled") );
  modePopup->insertItem(i18n("SymmetricalMirrored") );
  modePopup->insertItem(i18n("Scaled") );
  connect( modePopup, SIGNAL( activated( int ) ),
	   this, SLOT( slotModeSelected( int ) ) );

  // setup icon
  QString pixdir = KApplication::kde_icondir();
  pixmap = TQPixmap( (pixdir + "/logo.xpm").ascii() );  /* TQt3 迁移 */

  KDNDDropZone *myDropZone = new KDNDDropZone(this, DndURL);
  connect( myDropZone, SIGNAL( dropAction( KDNDDropZone *) ),
	   this, SLOT( slotDropEvent( KDNDDropZone *) ) );

  applyDesktop( current );

  QString command;
  if ( oneDesktopMode )
    command = "kbgwm_change";
  else
    command << "kbgwm_change_" << current;
  KWM::sendKWMCommand( command.data() );

}


void KBGndManager::setUndock()
{
  KConfig config2(KApplication::kde_configdir() + "/kcmdisplayrc",
		  KApplication::localconfigdir() + "/kcmdisplayrc");

  undock(); // first undock to set docked variable
  config2.setGroup( "Desktop Common" );
  config2.writeEntry( "Docking", docked );
}


void KBGndManager::slotDropEvent( KDNDDropZone *zone)
{
  QStrList & list = zone->getURLList();
  wallpaper = list.first();

  int x = this->x();
  int y = this->y();

  modePopup->popup(QPoint(x, y));
  modePopup->exec();
}


void KBGndManager::slotModeSelected( int mode )
{
  if ( ! wallpaper.isEmpty() )
    desktops[current].setImmediately( wallpaper, mode + 1 );
}


void KBGndManager::desktopChange( int d )
{
  if ( oneDesktopMode )
    {
      //tqDebug( "One desktop mode");
      return;
    }

  d = KWM::currentDesktop() - 1;

  //tqDebug( "Changing to desktop %d", d+1 );

  if ( desktops[current].getName() == desktops[d].getName() )
    {
      //tqDebug( "Desktops identical" );
      current = d;
      return;
    }

  if ( current != d )
    cacheDesktop();

  applyDesktop( d );

  current = d;
}



void KBGndManager::commandReceived( QString com )
{
  if ( com == "kbgwm_reconfigure" )
    {
      //tqDebug( "Got background reload event" );

      QString oldName = desktops[current].getName();

      KConfig config2(KApplication::kde_configdir() + "/kcmdisplayrc",
		      KApplication::localconfigdir() + "/kcmdisplayrc");

      config2.setGroup( "Desktop Common" );
      oneDesktopMode = config2.readBoolEntry( "OneDesktopMode", DEFAULT_ENABLE_COMMON_BGND );
      popup_m->setItemChecked( o_id, oneDesktopMode );
 
      desktop = config2.readNumEntry( "DeskNum", DEFAULT_DESKTOP );
      if ( config2.readBoolEntry( "Docking", DEFAULT_ENABLE_DOCKING ) )
        dock(); 
      else
	undock();

      for ( int i = 0; i < MAX_DESKTOPS; i++ )
	desktops[i].readSettings( i, oneDesktopMode, desktop );

      if ( oneDesktopMode )
	current = desktop;
      else
	current = KWM::currentDesktop() - 1;

      applyDesktop( current );

      QString command;
      if ( oneDesktopMode )
	command = "kbgwm_change";
      else
	command << "kbgwm_change_" << current;
      KWM::sendKWMCommand( command.data() );
    }
}



void KBGndManager::applyDesktop( int d )
{
  desktops[d].apply();
}



/* [KDE1 Revival 2026] 分辨率热变更监视（5W1H 见 kbgndwm.h 声明处）
 * How  : ① XGetGeometry 直查根窗真值（TQt3 -no-xrandr 下 desktop()
 *          缓存永不过期，不可作依据）；② 与上次记录相同 → 返回；
 *        ③ 不同 → 记录新值 → QPixmapCache::clear()（缓存里全是旧尺寸
 *          的渲染结果，必须全部作废）→ applyDesktop(current) 按新
 *          尺寸重渲染并重设根窗背景 → ④ XClearArea 全屏清填：
 *          服务器立即用新背景 pixmap 重铺整个根窗，残留的旧壁纸
 *          "背影"当场消失，无需等下一次暴露 */
void KBGndManager::timerEvent( QTimerEvent * )
{
  Window root_ret;
  int x_ret, y_ret;
  unsigned int rw, rh, border, depth;
  if ( !XGetGeometry( qt_xdisplay(), qt_xrootwin(), &root_ret,
		      &x_ret, &y_ret, &rw, &rh, &border, &depth ) )
    return;
  if ( (int) rw == last_root_w && (int) rh == last_root_h )
    return;

  fprintf( stderr, "kbgndwm: 屏幕分辨率已变更为 %dx%d，壁纸按新尺寸重渲染\n",
	   (int) rw, (int) rh );

  last_root_w = (int) rw;
  last_root_h = (int) rh;

  QPixmapCache::clear();
  applyDesktop( current );
  XClearArea( qt_xdisplay(), qt_xrootwin(),
	      0, 0, 0, 0,  /* 0 宽高 = 整窗 */
	      True );
  XFlush( qt_xdisplay() );
}



void KBGndManager::cacheDesktop()
{
  // cache current desktop
  if ( desktops[current].hasPixmap() )
    {
      desktops[current].cancel();

      if ( qApp->desktop()->backgroundPixmap() == 0L )
	return;

      if ( !desktops[current].isApplied() )
	return;

      if ( !QPixmapCache::find( desktops[current].getName() ) )
	{
	  QPixmap *pm = new QPixmap;
	  *pm = *qApp->desktop()->backgroundPixmap();
	  if ( !QPixmapCache::insert( desktops[current].getName(), pm ) )
	    delete pm;
	}
    }
}


void KBGndManager::readSettings()
{

  KConfig config2(KApplication::kde_configdir() + "/kcmdisplayrc",
		  KApplication::localconfigdir() + "/kcmdisplayrc");

  config2.setGroup( "Desktop Common" );
  oneDesktopMode = config2.readBoolEntry( "OneDesktopMode", DEFAULT_ENABLE_COMMON_BGND );
  desktop = config2.readNumEntry( "DeskNum", DEFAULT_DESKTOP );

  if ( config2.readBoolEntry( "Docking", DEFAULT_ENABLE_DOCKING ) )
    dock();

  int cache = config2.readNumEntry( "CacheSize", DEFAULT_CACHE_SIZE );

  if ( cache < 128 )
    cache = 128;
  if ( cache > 10240 )
    cache = 10240;

  QPixmapCache::setCacheLimit( cache );

}


void KBGndManager::timeclick()
{
  desktops[current].doRandomize();
}


void KBGndManager::toggleOneDesktop()
{
  oneDesktopMode = !oneDesktopMode;
  desktop = KWM::currentDesktop() - 1;

  KConfig config2(KApplication::kde_configdir() + "/kcmdisplayrc",
		  KApplication::localconfigdir() + "/kcmdisplayrc");

  config2.setGroup( "Desktop Common" );
  config2.writeEntry( "OneDesktopMode", oneDesktopMode );
  config2.writeEntry( "DeskNum", desktop );
  config2.sync();

  popup_m->setItemChecked( o_id, oneDesktopMode );

  for ( int i = 0; i < MAX_DESKTOPS; i++ )
    desktops[i].setOneDesktop( oneDesktopMode, desktop );

  if ( oneDesktopMode )
    current = desktop;
  else
    current = KWM::currentDesktop() - 1;

  applyDesktop( current );

  KWM::sendKWMCommand( "kbgwm_change" );
}


void KBGndManager::paintEvent (QPaintEvent *e)
{
  (void) e;

  bitBlt(this, 0, 0, &pixmap);
}


void KBGndManager::mousePressEvent(QMouseEvent *e)
{
  if ( e->button() == LeftButton )
    timeclick();
  else if ( e->button() == RightButton ) {
    // open popup menu on right mouse button
    int x = e->x() + this->x();
    int y = e->y() + this->y();

    popup_m->popup(QPoint(x, y));
    popup_m->exec();
  }
}

void KBGndManager::displaySettings()
{
  KShellProcess proc;
  proc << "kcmdisplay";
  proc.start(KShellProcess::DontCare);
}


void KBGndManager::settings()
{
  KShellProcess proc;
  proc << "kcmdisplay background";
  proc.start(KShellProcess::DontCare);
}


void KBGndManager::dock()
{
  if (!docked) {

    // prepare panel to accept this widget
    KWM::setDockWindow (this->winId());

    // that's all the space there is
    this->setFixedSize(24, 24);

    // finally dock the widget
    this->show();

    docked = true;
  }
}

void KBGndManager::undock()
{
  if (docked) {

    // the widget's window has to be destroyed in order
    // to undock from the panel. Simply using hide() is
    // not enough (seems to be necessary though).

    this->hide();

    this->destroy(true, true);

    // recreate window for further dockings
    this->create(0, true, false);

    docked = false;
  }
}
