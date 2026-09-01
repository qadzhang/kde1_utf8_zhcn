/*
 * bg.cpp.  Part of the KDE Project.
 *
 * Copyright (C) 1997 Martin Jones
 *               1998 Matej Koss
 *
 * $Id: bg.cpp,v 1.33.2.9 1999/09/04 15:04:00 dmuell Exp $
 *
 *
 */

//----------------------------------------------------------------------------

#include <stdlib.h>

#include <X11/Xatom.h>
/* [KDE1 Revival 2026] kbg_root_size 用 XGetGeometry；Xlib 头必须在 TQt
 * 头之后包含——Xlib 的 Unsorted 等宏会破坏 TQDir 的枚举名 */

#include <qimage.h>
#include <qfile.h>
#include <qpainter.h>
#include <qstring.h>
#include <qpmcache.h>
#include <qdir.h>

#include <kapp.h>
#include <kpixmap.h>
#include <kstring.h>
#include <kwm.h>
#include <kurl.h>
#include <kprocess.h>

#include <X11/Xlib.h> /* [KDE1 Revival 2026] kbg_root_size 用 XGetGeometry；
 Xlib 头须在 TQt 头之后——其 Unsorted 等宏会破坏 TQDir 枚举名 */

#include "bg.h"
#include "bg.moc"
#include "config-kbgndwm.h"

//----------------------------------------------------------------------------


KBackground::KBackground()
{
  wpMode = Tiled;
  gfMode = Flat;
  orMode = Portrait;

  bgPixmap = 0;
  applied = false;

  hasPm = false;
  bUseWallpaper = false;

  timerRandom = 0;
  randomDesk = 0;
  desk = 0;

  useDir = false;
}


KBackground::~KBackground()
{
}



void KBackground::setImmediately( const char *_wallpaper, int mode )
{
  hasPm = false;
  color1 = QColor(black);
  gfMode=Flat;
  orMode=Portrait;
  wpMode = mode;
  bUseWallpaper = true;

  KURL url = _wallpaper;
  wallpaper = url.path();

  hasPm = true;
  apply();

  QString command;
  command << "kbgwm_change_" << desk;
  KWM::sendKWMCommand( command.data() );
}


/*
 * Read the settings from desktoXprc
 */
void KBackground::readSettings( int num, bool one, int onedesk )
{
  hasPm = false;

  desk = num;
  oneDesktopMode = one;
  oneDesk = onedesk;

  QString tmpf;
  ksprintf( &tmpf, "/desktop%drc", num);

  KConfig config(KApplication::kde_configdir() + tmpf,
		  KApplication::localconfigdir() + tmpf);

  config.setGroup( "Common" );
  randomMode = config.readBoolEntry( "RandomMode", DEFAULT_ENABLE_RANDOM_MODE );

  if ( randomMode ) {
    if ( !timerRandom ) {
      timerRandom = new QTimer( this );
      connect(timerRandom, SIGNAL(timeout()), SLOT(randomize()));
    }
    timerRandom->stop();
    timerRandom->start( config.readNumEntry( "Timer",
                                            DEFAULT_RANDOM_TIMER ) * 1000 );
    int count = config.readNumEntry( "Count", DEFAULT_RANDOM_COUNT );
    bool inorder = config.readBoolEntry( "InOrder", DEFAULT_RANDOM_IN_ORDER);
    useDir = config.readBoolEntry( "UseDir", DEFAULT_RANDOM_USE_DIR );

    if ( useDir ) {
      QString tmpd = config.readEntry( "Directory", KApplication::kde_wallpaperdir());
      QDir d( tmpd, "*", QDir::Name, QDir::Readable | QDir::Files );

      // TQt3 迁移：entryList 值语义——成员列表承接
      k1entrylist.clear();
      { QStringList k1sl = d.entryList();
        for (unsigned k1i = 0; k1i < k1sl.count(); ++k1i) k1entrylist.append( k1sl[k1i] ); }
      QStrList *list = &k1entrylist;

      count = list->count();
      if ( inorder ) {
        randomDesk = config.readNumEntry( "Item", DEFAULT_DESKTOP );
	randomDesk++;
	if ( randomDesk >= count ) randomDesk = DEFAULT_DESKTOP;
      } else if ( count > 0 ) {
        randomDesk = random() % count;
      }

      color1 = QColor(DEFAULT_COLOR_1);
      gfMode = DEFAULT_COLOR_MODE;
      orMode = DEFAULT_ORIENTATION_MODE;
      wpMode = DEFAULT_WALLPAPER_MODE;
      bUseWallpaper = true;

      wallpaper = d.absPath() + "/" + list->at( randomDesk );
      name.sprintf( "%s_%d_%d_%d#%02x%02x%02x#%02x%02x%02x#", wallpaper.data(),
                    wpMode, gfMode, orMode, color1.red(), color1.green(),
		    color1.blue(), color2.red(), color2.green(), color2.blue());

      hasPm = true;

      // this is mainly for kpager, so that we can at anytime find out how desktop
      //          really looks
      config.writeEntry( "Item", randomDesk );
      config.sync();

      return;
    }
    else if ( inorder ) {
      randomDesk = config.readNumEntry( "Item", DEFAULT_DESKTOP );
      randomDesk++;
      if ( randomDesk >= count ) randomDesk = DEFAULT_DESKTOP;
    }
    else if ( count > 0 ) {
      randomDesk = random() % count;
    }
    else {
      randomDesk = DEFAULT_DESKTOP;
    }
  }
  else {
    if(timerRandom) {
      delete timerRandom;
      timerRandom = 0;
    }
    randomDesk = DEFAULT_DESKTOP;
  }

  // this is mainly for kpager, so that we can at anytime find out how desktop
  //          really looks
  config.writeEntry( "Item", randomDesk );
  config.sync();

  ksprintf( &tmpf, "Desktop%d", randomDesk);
  config.setGroup( tmpf );

  QString str;

  str = config.readEntry( "Color1", DEFAULT_COLOR_1 );
  color1.setNamedColor( str );

  str = config.readEntry( "Color2", DEFAULT_COLOR_2 );
  color2.setNamedColor( str );

  gfMode = DEFAULT_COLOR_MODE;
  str = config.readEntry( "ColorMode", "unset" );
  if ( str == "Gradient" ) {
      gfMode = Gradient;
      hasPm = true;
  }
  else if (str == "Pattern") {
    gfMode = Pattern;
    QStrList strl;
    config.readListEntry("Pattern", strl);
    uint size = strl.count();
    if (size > 8) size = 8;
    uint i = 0;
    for (i = 0; i < 8; i++)
      pattern[i] = (i < size) ? QString(strl.at(i)).toUInt() : 255;
  }

  orMode = DEFAULT_ORIENTATION_MODE;
  str = config.readEntry( "OrientationMode", "unset" );
  if ( str == "Landscape" )
    orMode = Landscape;

  wpMode = DEFAULT_WALLPAPER_MODE;
  str = config.readEntry( "WallpaperMode", "unset" );
  if ( str == "Mirrored" )
    wpMode = Mirrored;
  else if ( str == "CenterTiled" )
    wpMode = CenterTiled;
  else if ( str == "Centred" )
    wpMode = Centred;
  else if ( str == "CentredBrick" )
    wpMode = CentredBrick;
  else if ( str == "CentredWarp" )
    wpMode = CentredWarp;
  else if ( str == "CentredMaxpect" )
    wpMode = CentredMaxpect;
  else if ( str == "SymmetricalTiled" )
    wpMode = SymmetricalTiled;
  else if ( str == "SymmetricalMirrored" )
    wpMode = SymmetricalMirrored;
  else if ( str == "Scaled" )
    wpMode = Scaled;

  wallpaper = DEFAULT_WALLPAPER;
  bUseWallpaper = config.readBoolEntry( "UseWallpaper", DEFAULT_USE_WALLPAPER );
  if ( bUseWallpaper )
    wallpaper = config.readEntry( "Wallpaper", DEFAULT_WALLPAPER );

  name.sprintf( "%s_%d_%d_%d#%02x%02x%02x#%02x%02x%02x#", wallpaper.data(),
		wpMode, gfMode, orMode, color1.red(), color1.green(),
		color1.blue(), color2.red(), color2.green(), color2.blue());

  QString tmp;
  for (int i = 0; i < 8; i++) {
    tmp.sprintf("%02x", pattern[i]);
    name += tmp;
  }

  hasPm = true;

}


void KBackground::randomize()
{
    doRandomize( TRUE );
}


void KBackground::doRandomize(bool fromTimer)
{
  if ( randomMode ) {
    readSettings( desk, oneDesktopMode, oneDesk );

    QString command;
    if ( oneDesktopMode )
      command = "kbgwm_change";
    else
      command << "kbgwm_change_" << desk;
    
    KWM::sendKWMCommand( command.data() );

    if ( ( oneDesktopMode && desk == oneDesk ) ||
	 ( !oneDesktopMode && desk == ( KWM::currentDesktop() - 1 ) ) )
      apply();

    return;
  }
  if (!fromTimer) {
      KShellProcess proc;
      proc << "kcmdisplay";
      proc.start(KShellProcess::DontCare);
  }
}

/* [KDE1 Revival 2026] 屏幕真实尺寸（壁纸"背影"残留根治的基石）
 * ┌─ What : 经 XGetGeometry 直查根窗当前几何
 * │  Why  : TQt3 以 -no-xrandr 构建，QApplication::desktop() 的宽高是
 * │        构造时刻的一次性缓存（qdesktopwidget_x11.cpp 的 d->rects），
 * │        热改分辨率后永不过期——壁纸按旧尺寸渲染，根窗背景随
 * │        弹出菜单关闭等暴露重填时错位平铺，形成桌面"背影"
 * │        （用户 2026-09-01 截图实测：1280 壁纸平铺出 1920 桌面）。
 * │  Who  : KBackground::apply() 全部渲染尺寸；kbgndwm.cpp 的
 * │        分辨率监视定时器同源
 * │  When : 每次 apply 渲染前调用
 * │  Where: bg.cpp（文件局部）
 * │  How  : XGetGeometry( qt_xdisplay(), qt_xrootwin() )；失败才回退
 * │        desktop() 缓存
 * └────────────────────────────────────────────────────────────── */
/* [KDE1 Revival 2026] KDE3/TDE kdesktop 配方的桌面画布（"兼容层"）
 * ┌─ What : 全部壁纸上屏点统一经此助手：根窗背景照旧设置（kfm 透明等
 * │        消费方仍读根窗），同时把渲染结果设为画布的 erase-pixmap
 * │        并立即 repaint——画面实际由 kbgndwm 的全屏画布窗承载
 * │  Why  : 参照 KDE3 kdesktop bgmanager.cpp 的 setPixmap()：
 * │            m_pDesktop->setErasePixmap(*ep); m_pDesktop->repaint();
 * │        服务端根窗背景填充在 vmware 类驱动/DM 残留状态下不可靠
 * │        （用户实测刷新桌面黑屏、菜单关闭残留；真机 Xorg 复现），
 * │        kdesktop 自 KDE2 起改用自有桌面窗 + erase-pixmap + 显式
 * │        重绘承载画面。erase-pixmap 与 repaint 的组合保证映射/
 * │        暴露时由本进程在场控制的重绘立即生效，与驱动实现解耦。
 * │  Who  : KBackground::apply() 各分支与延迟渲染 timerEvent
 * │  When : 每次渲染结果产生时（含 startTimer(0) 延迟路径——这是
 * │        此前画布同步悬垂崩溃的根源：轮询式读取桌面背景在 pixmap
 * │        替换瞬间踩已释放对象；改为渲染落点直接推送，零竞态）
 * │  Where: bg.cpp 文件局部
 * │  How  : setErasePixmap(pm) + repaint()；首次上屏时映射画布并
 * │        XLowerWindow 压底（画布属性 override_redirect/输入透明
 * │        在 kbgndwm.cpp 创建时已就位）
 * └────────────────────────────────────────────────────────────── */
static TQWidget *kbg_canvas_owner = 0;

void KBackground::setOwnerCanvas( TQWidget *c )
{
  kbg_canvas_owner = c;
}

static void kbg_apply_wallpaper( const TQPixmap &pm )
{
  qApp->desktop()->setBackgroundPixmap( pm );
  if ( kbg_canvas_owner ) {
    if ( !kbg_canvas_owner->isVisible() ) {
      kbg_canvas_owner->show();
      XLowerWindow( qt_xdisplay(), kbg_canvas_owner->winId() );
      XFlush( qt_xdisplay() );
    }
    kbg_canvas_owner->setErasePixmap( pm );
    kbg_canvas_owner->repaint();
  }
}

static void kbg_apply_wallpaper_color( const TQColor &c )
{
  qApp->desktop()->setBackgroundColor( c );
  if ( kbg_canvas_owner ) {
    if ( !kbg_canvas_owner->isVisible() ) {
      kbg_canvas_owner->show();
      XLowerWindow( qt_xdisplay(), kbg_canvas_owner->winId() );
      XFlush( qt_xdisplay() );
    }
    kbg_canvas_owner->setBackgroundColor( c );
    kbg_canvas_owner->repaint();
  }
}

static void kbg_root_size( int *w, int *h )
{
    Window root_ret;
    int x_ret, y_ret;
    unsigned int rw, rh, border, depth;
    if ( XGetGeometry( qt_xdisplay(), qt_xrootwin(), &root_ret,
		       &x_ret, &y_ret, &rw, &rh, &border, &depth ) ) {
	*w = (int) rw;
	*h = (int) rh;
    } else {
	*w = QApplication::desktop()->width();
	*h = QApplication::desktop()->height();
    }
}

QPixmap *KBackground::loadWallpaper()
{
  if( !bUseWallpaper ) return 0;

  QString filename;

  if ( wallpaper[0] != '/' )
    {
      filename = KApplication::kde_wallpaperdir() + "/";
      filename += wallpaper;
    }
  else
    filename = wallpaper;

  KPixmap *wpPixmap = new KPixmap;

  if ( wpPixmap->load( filename, 0, KPixmap::LowColor ) == FALSE )
    {
      delete wpPixmap;
      wpPixmap = 0;
    }

  return wpPixmap;
}


void KBackground::apply()
{
  applied = false;

  // the background pixmap is cached?
  bgPixmap = QPixmapCache::find( name );
  if ( bgPixmap )
    {
      //      tqDebug( "Desktop background found in cache" );
      kbg_apply_wallpaper( *bgPixmap );
      setPixmapProperty( bgPixmap );
      bgPixmap = 0;
      applied = true;
      return;
    }

  QPixmap *wpPixmap = loadWallpaper();

  uint w=0, h=0;

  if (wpPixmap) {
    /* [KDE1 Revival 2026] 渲染尺寸取屏幕真值（kbg_root_size），弃用
       desktop() 过期缓存——否则热改分辨率后壁纸按旧尺寸渲染，
       根窗暴露重填错位成"背影" */
    int render_w = 0, render_h = 0;
    kbg_root_size( &render_w, &render_h );
    w = (uint) render_w;
    h = (uint) render_h;

    bgPixmap = new QPixmap;
  }

  if ( !wpPixmap || (wpMode == Centred) || (wpMode == CentredBrick) ||
       (wpMode == CentredWarp) || ( wpMode == CentredMaxpect) ) {
    if (bgPixmap)
      bgPixmap->resize(w, h);

    KPixmap pmDesktop;
    switch (gfMode) {

    case Gradient:
      {
	int numColors = 4;
	if ( QColor::numBitPlanes() > 8 )
	  numColors = 16;

	if ( orMode == Portrait ) {

		  pmDesktop.resize( 20, (int) h ); /* [KDE1 Revival 2026] 真值 h */
		  pmDesktop.gradientFill( color2, color1, true, numColors );

		} else {
		  pmDesktop.resize( (int) w, 20 ); /* [KDE1 Revival 2026] 真值 w */
		  pmDesktop.gradientFill( color2, color1, false, numColors );
		}

	delete bgPixmap;
	bgPixmap = new QPixmap();
		
	if (! wpPixmap ) {

	  kbg_apply_wallpaper(pmDesktop);
          setPixmapProperty( &pmDesktop );
	  *bgPixmap = pmDesktop;

	} else {
	  bgPixmap->resize(w, h);
		
	  if ( orMode == Portrait ) {
	    for (uint pw = 0; pw <= w; pw += pmDesktop.width())
	      bitBlt( bgPixmap, pw, 0, &pmDesktop, 0, 0,
		      pmDesktop.width(), h);
	  } else {
	    for (uint ph = 0; ph <= h; ph += pmDesktop.height()) {
	      //	      tqDebug("land %d",ph);
	      bitBlt( bgPixmap, 0, ph, &pmDesktop, 0, 0,
		      w, pmDesktop.height());
	    }
	  }
	}

	// background switch is deferred in case the user switches
	// again while the background is loading
	startTimer( 0 );
      }
      break;
	
    case Flat:
      if (wpPixmap ) {
	delete bgPixmap;
	bgPixmap = new QPixmap(w, h);
	bgPixmap->fill( color1 );
      } else {
	kbg_apply_wallpaper_color( color1 );
	applied = true;
      }
      break;
	
    case Pattern:
      {
	QPixmap tile(8, 8);
	tile.fill(color2);
	QPainter pt;
	pt.begin(&tile);
	pt.setBackgroundColor( color2 );
	pt.setPen( color1 );
		
	for (int y = 0; y < 8; y++) {
	  uint v = pattern[y];
	  for (int x = 0; x < 8; x++) {
	    if ( v & 1 )
	      pt.drawPoint(7 - x, y);
	    v /= 2;
	  }
	}
	pt.end();

	delete bgPixmap;
	bgPixmap = new QPixmap();

	if (! wpPixmap ) {
	  kbg_apply_wallpaper(tile);
          setPixmapProperty( &pmDesktop );
	  *bgPixmap = tile;
	  applied = true;
	} else {
	  bgPixmap->resize(w, h);
	  uint sx, sy = 0;
	  while (sy < h) {
	    sx = 0;
	    while (sx < w) {
	      bitBlt( bgPixmap, sx, sy, &tile, 0, 0, 8, 8);
	      sx += 8;
	    }
	    sy += 8;
	  }
	}
	break;
      }
    }
	
  }

  if ( wpPixmap )
    {
	
      if ( ( wpPixmap->width() > (int)w || wpPixmap->height() > (int)h ||
	   wpMode == CentredMaxpect ) &&
	   wpMode != Scaled ) {
	// shrink if image is bigger than desktop or CentredMaxpect
	float sc;
	float S = (float)h / (float)w ;
	float I = (float)wpPixmap->height() / (float)wpPixmap->width() ;
	
	if (S < I)
	  sc= (float)h / (float)wpPixmap->height();
	else
	  sc= (float)w / (float)wpPixmap->width();
	
	QWMatrix scaleMat;
	scaleMat.scale(sc,sc);

	QPixmap tmp2 = wpPixmap->xForm( scaleMat );
	wpPixmap->resize( tmp2.width(), tmp2.height() );
	bitBlt( wpPixmap, 0, 0, &tmp2 );
      }

      switch ( wpMode )
	{

	case Tiled:
	  {
	    bgPixmap->resize( wpPixmap->width(), wpPixmap->height() );
	    bgPixmap->fill( color1 );
	    bitBlt( bgPixmap, 0, 0, wpPixmap );
	  }
	  break;

	case Mirrored:
	  {
	    int w = wpPixmap->width();
	    int h = wpPixmap->height();

	    bgPixmap->resize( w * 2, h * 2);

	    /* quadrant 2 */
 	    bitBlt( bgPixmap, 0, 0, wpPixmap );
	
	    /* quadrant 1 */
	    QWMatrix S(-1.0F, 0.0F, 0.0F, 1.0F, 0.0F, 0.0F);
	    QPixmap newp = wpPixmap->xForm( S );
	    bitBlt( wpPixmap, 0, 0, &newp );
	    bitBlt( bgPixmap, w, 0, wpPixmap );

	    /* quadrant 4 */
	    S.setMatrix(1.0F, 0.0F, 0.0F, -1.0F, 0.0F, 0.0F);
	    newp = wpPixmap->xForm( S );
	    bitBlt( wpPixmap, 0, 0, &newp );
	    bitBlt( bgPixmap, w, h, wpPixmap );

	    /* quadrant 3 */
	    S.setMatrix(-1.0F, 0.0F, 0.0F, 1.0F, 0.0F, 0.0F);
	    newp = wpPixmap->xForm( S );
	    bitBlt( wpPixmap, 0, 0, &newp );
	    bitBlt( bgPixmap, 0, h, wpPixmap );

	  }
	  break;

	case SymmetricalTiled:
	case SymmetricalMirrored:
	  {
	    int fliph = 0;
	    int flipv = 0;
	    uint w0 = wpPixmap->width();
	    uint h0 = wpPixmap->height();

	    bgPixmap->resize(w, h);

	    if (w == w0) {
	      /* horizontal center line */
	      int y, ay;

	      y = h0 - ((h/2)%h0); /* Starting point in picture to copy */
	      ay = 0;    /* Vertical anchor point */
	      while (ay < (int)h) {
		bitBlt( bgPixmap, 0, ay, wpPixmap, 0, y );
		ay += h0 - y;
		y = 0;
		if ( wpMode == SymmetricalMirrored ) {
		  QWMatrix S(1.0F, 0.0F, 0.0F, -1.0F, 0.0F, 0.0F);
		  QPixmap newp = wpPixmap->xForm( S );
		  bitBlt( wpPixmap, 0, 0, &newp );
		  flipv = !flipv;
		}
	      }
	    }
	    else if (h == h0) {
	      /* vertical centerline */
	      int x, ax;

	      x = w0 - ((w/2)%w0); /* Starting point in picture to copy */
	      ax = 0;    /* Horizontal anchor point */
	      while (ax < (int)w) {
		bitBlt( bgPixmap, ax, 0, wpPixmap, x, 0 );
		ax += w0 - x;
		x = 0;
		if ( wpMode == SymmetricalMirrored ) {
		  QWMatrix S(-1.0F, 0.0F, 0.0F, 1.0F, 0.0F, 0.0F);
		  QPixmap newp = wpPixmap->xForm( S );
		  bitBlt( wpPixmap, 0, 0, &newp );
		  fliph = !fliph;
		}
	      }
	    }
	    else {
	      /* vertical and horizontal centerlines */
	      int x,y, ax,ay;

	      y = h0 - ((h/2)%h0); /* Starting point in picture to copy */
	      ay = 0;    /* Vertical anchor point */

	      while (ay < (int)h) {
		x = w0 - ((w/2)%w0);/* Starting point in picture to cpy */
		ax = 0;    /* Horizontal anchor point */
		while (ax < (int)w) {
		  bitBlt( bgPixmap, ax, ay, wpPixmap, x, y );
		  if ( wpMode == SymmetricalMirrored ) {
		    QWMatrix S(-1.0F, 0.0F, 0.0F, 1.0F, 0.0F, 0.0F);
		    QPixmap newp = wpPixmap->xForm( S );
		    bitBlt( wpPixmap, 0, 0, &newp );
		    fliph = !fliph;
		  }
		  ax += w0 - x;
		  x = 0;
		}
		if ( wpMode == SymmetricalMirrored ) {
		  QWMatrix S(1.0F, 0.0F, 0.0F, -1.0F, 0.0F, 0.0F);
		  QPixmap newp = wpPixmap->xForm( S );
		  bitBlt( wpPixmap, 0, 0, &newp );
		  flipv = !flipv;
		  if (fliph) {   /* leftmost image is always non-hflipped */
		    S.setMatrix(-1.0F, 0.0F, 0.0F, 1.0F, 0.0F, 0.0F);
		    newp = wpPixmap->xForm( S );
		    bitBlt( wpPixmap, 0, 0, &newp );
		    fliph = !fliph;
		  }
		}
		ay += h0 - y;
		y = 0;
	      }
	    }
	  }
	  break;
	
	case CenterTiled:
	  {
	    int i, j, x, y, w0, h0, ax, ay, w1, h1, offx, offy;

	    bgPixmap->resize(w, h);

	    w0 = wpPixmap->width();  h0 = wpPixmap->height();

	    /* compute anchor pt (top-left coords of top-left-most pic) */
	    ax = (w-w0)/2;  ay = (h-h0)/2;
	    while (ax>0) ax = ax - w0;
	    while (ay>0) ay = ay - h0;

	    for (i=ay; i < (int)h; i+=h0) {
	      for (j=ax; j < (int)w; j+=w0) {
		/* if image goes off tmpPix, only draw subimage */
	
		x = j;  y = i;  w1 = w0;  h1 = h0;  offx = offy = 0;
		if (x<0)           { offx = -x;  w1 -= offx;  x = 0; }
		if (x+w1>w0) { w1 = (w0-x); }

		if (y<0)           { offy = -y;  h1 -= offy;  y = 0; }
		if (y+h1>h0)    { h1 = (h0-y); }
	
		bitBlt( bgPixmap, x, y, wpPixmap, offx, offy );
	      }
	    }

	  }
	  break;
		
	case Centred:
	case CentredMaxpect:
	  {
	    bitBlt( bgPixmap, ( w - wpPixmap->width() ) / 2,
		    ( h - wpPixmap->height() ) / 2, wpPixmap, 0, 0,
		    wpPixmap->width(), wpPixmap->height() );
	  }
	  break;
		
	case Scaled:
	  {
	    float sx = (float)w / wpPixmap->width();
	    float sy = (float)h / wpPixmap->height();
			
	    bgPixmap->resize( w, h );
	    bgPixmap->fill( color1 );
			
	    QWMatrix matrix;
	    matrix.scale( sx, sy );
	    QPixmap newp = wpPixmap->xForm( matrix );
	    bitBlt( bgPixmap, 0, 0, &newp );
	  }
	  break;

	case CentredBrick:
	  {
	    int i, j, k;

	    QPainter paint( bgPixmap );
	    paint.setPen( white );
	    for ( i=k=0; i < (int)w; i+=20,k++ ) {
	      paint.drawLine( 0, i, w, i );
	      for (j=(k&1) * 20 + 10; j< (int)w; j+=40)
		paint.drawLine( j, i, j, i+20 );
	    }

	    bitBlt( bgPixmap, ( w - wpPixmap->width() ) / 2,
		    ( h - wpPixmap->height() ) / 2, wpPixmap, 0, 0,
		    wpPixmap->width(), wpPixmap->height() );
	  }
	  break;

	case CentredWarp:
	  {
	    int i;

	    QPainter paint( bgPixmap );
	    paint.setPen( white );
	    for ( i=0; i < (int)w; i+=8 )
	      paint.drawLine( i, 0, w - i, h );
	    for ( i=0; i < (int)h; i+=8 )
	      paint.drawLine( 0, i, w, h - i );

	    bitBlt( bgPixmap, ( w - wpPixmap->width() ) / 2,
		    ( h - wpPixmap->height() ) / 2, wpPixmap, 0, 0,
		    wpPixmap->width(), wpPixmap->height() );
	  }
	  break;
	}

      delete wpPixmap;
      wpPixmap = 0;
	
      // background switch is deferred in case the user switches
      // again while the background is loading
      startTimer( 0 );
    }
}

void KBackground::cancel()
{
  killTimers();
  if ( bgPixmap )
    delete bgPixmap;
}

void KBackground::timerEvent( QTimerEvent * )
{
  killTimers();

  if ( !bgPixmap )
    return;

  kbg_apply_wallpaper( *bgPixmap );
  setPixmapProperty( bgPixmap );
  delete bgPixmap;
  bgPixmap = 0;
  applied = true;
}

void KBackground::setPixmapProperty( QPixmap *qP )
{
    Display *Xdisplay = kapp->getDisplay();
    Window Xroot = kapp->desktop()->handle();
    Pixmap p = qP->handle();
    Atom prop_root = XInternAtom(Xdisplay, "_XROOTPMAP_ID", False);
    Atom prop_esetroot = XInternAtom(Xdisplay, "ESETROOT_PMAP_ID", False);

    if(prop_root == None || prop_esetroot == None) return;

    XChangeProperty(Xdisplay, Xroot, prop_root, XA_PIXMAP, 32, PropModeReplace,
                    (unsigned char *) &p, 1);
    XChangeProperty(Xdisplay, Xroot, prop_esetroot, XA_PIXMAP, 32,
                    PropModeReplace, (unsigned char *) &p, 1);
}


//----------------------------------------------------------------------------

