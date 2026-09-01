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
#include <ntqwmatrix.h> /* [KDE1 Revival 2026] 分辨率变化过渡帧的 xForm 拉伸 */
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
    startTimer( 200 ); /* [2026-09-01] 200ms 快轮询：分辨率变化即时感知（XGetGeometry 一次往返开销可忽略） */
  }

  // [KDE1 Revival 2026] 全屏壁纸画布在首次渲染后创建（见 applyDesktop
  // 之后的创建块）——必须先渲染出壁纸 pixmap、再建画布并映射，映射前
  // X 背景属性即已携带壁纸，服务器首次填充即为壁纸（映射后 Qt 层设
  // 背景不会刷新 X 属性，曾致整屏灰底）。

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

  /* [KDE1 Revival 2026] 创建全屏壁纸画布（5W1H 见 kbgndwm.h 声明处）。
   * How: ① 首次渲染已完成，取渲染结果 → ② 顶层窗建窗并设 Qt/X 双层
   *      背景（映射前 X 背景属性即携带壁纸，服务器首次填充即为壁纸）
   *      → ③ override_redirect=1（kwm 初始扫描与 MapRequest 均不管理）
   *      → ④ 事件掩码仅 Exposure|StructureNotify（点击穿透到根窗，
   *      krootwm 桌面菜单不受影响）→ ⑤ 映射后 XLowerWindow 压底。 */
  if ( !canvas ) {
    const QPixmap *wp0 = QApplication::desktop()->backgroundPixmap();
    canvas = new QWidget( 0, "kbgndwm_canvas",
			  WStyle_Customize | WStyle_NoBorder );
    canvas->setGeometry( 0, 0, last_root_w, last_root_h );
    if ( wp0 && !wp0->isNull() )
      canvas->setBackgroundPixmap( *wp0 );
    canvas->winId(); /* 强制先建 X 窗，属性须在映射前就位 */
    XSetWindowAttributes wa;
    wa.override_redirect = True;
    XChangeWindowAttributes( qt_xdisplay(), canvas->winId(),
			     CWOverrideRedirect, &wa );
    XSelectInput( qt_xdisplay(), canvas->winId(),
		  ExposureMask | StructureNotifyMask );
    canvas->show();
    XSelectInput( qt_xdisplay(), canvas->winId(),
		  ExposureMask | StructureNotifyMask ); /* show 后再钉一次 */
    if ( wp0 && !wp0->isNull() )
      /* 已映射状态再钉一次 X 层背景，保证服务器填充与壁纸一致 */
      XSetWindowBackgroundPixmap( qt_xdisplay(), canvas->winId(),
				  wp0->handle() );
    XLowerWindow( qt_xdisplay(), canvas->winId() );
    XFlush( qt_xdisplay() );
  }

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

  bool resized = ( (int) rw != last_root_w || (int) rh != last_root_h );
  if ( resized ) {
    fprintf( stderr, "kbgndwm: 屏幕分辨率已变更为 %dx%d，壁纸按新尺寸重渲染\n",
	     (int) rw, (int) rh );

    /* [2026-09-01] 过渡帧：先把旧壁纸立即拉伸到新尺寸设上（QWMatrix
     * xForm 毫秒级），桌面在精渲染完成前也保持视觉连续——否则
     * 5120x2880 级壁纸的精渲染耗时秒级，期间桌面黑/错位（真机
     * "背景刷新有问题"观感的主要来源） */
    const QPixmap *prev = QApplication::desktop()->backgroundPixmap();
    if ( canvas && prev && !prev->isNull() && prev->width() > 1 ) {
      QWMatrix m;
      m.scale( (double) rw / prev->width(), (double) rh / prev->height() );
      QPixmap stretched = prev->xForm( m );
      canvas->setGeometry( 0, 0, (int) rw, (int) rh );
      canvas->setBackgroundPixmap( stretched );
      XSetWindowBackgroundPixmap( qt_xdisplay(), canvas->winId(),
				  stretched.handle() );
      XClearArea( qt_xdisplay(), qt_xrootwin(), 0, 0, 0, 0, True );
      XFlush( qt_xdisplay() );
    }

    last_root_w = (int) rw;
    last_root_h = (int) rh;
    QPixmapCache::clear();
    applyDesktop( current );
  }

  /* [KDE1 Revival 2026] 画布每个 tick 都同步：KBackground::apply 对
   * 图片壁纸走 startTimer(0) 延迟渲染，构造器里同步时背景尚未就绪
   * （实测首启灰屏）——tick 粒度 2 秒补齐足够；已映射窗口必须手动
   * XSetWindowBackgroundPixmap 才会刷新 X 层背景属性。 */
  if ( canvas ) {
    const QPixmap *wp = QApplication::desktop()->backgroundPixmap();
    if ( wp && !wp->isNull() ) {
      if ( wp->width() != canvas->width() || wp->height() != canvas->height() )
        canvas->setGeometry( 0, 0, wp->width(), wp->height() );
      canvas->setBackgroundPixmap( *wp );
      XSetWindowBackgroundPixmap( qt_xdisplay(), canvas->winId(),
                                  wp->handle() );
      canvas->update();
      XLowerWindow( qt_xdisplay(), canvas->winId() );
    }
  }

  if ( resized ) {
    XClearArea( qt_xdisplay(), qt_xrootwin(),
                0, 0, 0, 0,  /* 0 宽高 = 整窗 */
                True );
    XFlush( qt_xdisplay() );
  }
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
