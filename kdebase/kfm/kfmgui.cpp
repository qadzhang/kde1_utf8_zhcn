#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <pwd.h>
#include <stddef.h>
#include <sys/wait.h>				// [KDE1 Revival 2026] 双内核子进程回收
#include <errno.h>				// [2026-08-31] EINTR 重试判定
#include <signal.h>				// [2026-08-31] SIGKILL 兜底

#include <qpopmenu.h>
#include <qcursor.h>
#include <qpainter.h>
#include <qapp.h>
#include <qkeycode.h>
// [KDE1 Revival 2026] XEmbed 原型（XCreateSimpleWindow 等）经 Qt 头内已含的
// Xlib 声明直接使用，无需也不得在此显式 include X11/Xlib.h（宏序冲突）
#include <qpushbt.h>
#include <qdir.h>
#include <qstrlist.h>
#include <qregexp.h>
#include <qmsgbox.h>
#include <qtooltip.h>
#include <qclipbrd.h>

#include <kconfig.h>
#include <kapp.h>
#include <kmsgbox.h>
#include <kstdaccel.h>
#include <kcursor.h>
#include <kprocess.h>

#include "kfmpaths.h"
#include "kfmgui.h"
#include "kfmdlg.h"
#include "kfmprops.h"
#include "kbutton.h"
#include "root.h"
#include "kbind.h"
#include "config-kfm.h"
#include "utils.h"
#include "kfmw.h"
#include "kcookiejar.h"

#include <klocale.h>
#include <kstring.h>
#include <kwm.h> // for sendKWMCommand. David.

#define FIRSTFILEMENUITEM 100

bool KfmGui::sumode = false;
bool KfmGui::rooticons = true;

char* kfm_getrev();

char* kfm_getrev()
{
    static char *revision = "$Revision: 1.167.2.21 $";
    static char *rev;
    static char *p;

    static bool flag = false;
    if ( flag )
	return rev;
    flag = true;

    char *dummy = strdup( revision );
    if ((p = strchr(dummy, ':'))) {
	rev = p + 2;
	p = strchr(rev, '$');
	*--p = 0;
    } else
	rev = "???";
    return rev;
}

KBookmarkManager *KfmGui::bookmarkManager;
QList<KfmGui> *KfmGui::windowList;
QList<QPixmap> *KfmGui::animatedLogo;

KfmGui::KfmGui( QWidget *, const char *name, const char * _url)
    : KTopLevelWidget( name )
{
    GUI_ready = false; //sven: flag to know do we fire up or what.
  
    toolbarButtons = 0L;
    toolbarURL = 0L;
    findDialog = 0;

    bTreeViewInitialized = false; // this is alway true (sven: moved it here)
    
    // Timer used for animated logo
    animatedLogoTimer = new QTimer( this );
    animatedLogoCounter = 0;
    connect( animatedLogoTimer, SIGNAL( timeout() ),
	     this, SLOT( slotAnimatedLogoTimeout() ) );
    
    /* read the settings. We have some defaults, if the 
       settings are not defined or if there are unknown
       words in it. */

    kfmgui_width = KFMGUI_WIDTH;
    kfmgui_height = KFMGUI_HEIGHT;

    bTreeView = false;
    viewMode = ICON_VIEW;
    showDot = false;
    visualSchnauzer = false;
    bViewHTML = true;
    // [KDE1 Revival 2026] 新内核初始状态（见 kfmgui.h）
    webEnginePid = -1;
    webEngineIn = -1;
    webEngineSocket = 0;
    webEngineOn = false;

    showToolbar = true;
    toolbarPos = KToolBar::Top;

    showLocationBar = true;
    locationBarPos = KToolBar::Top;

    showMenubar = true;
    menubarPos = KMenuBar::Top;

    showStatusbar = true;
    hasLocal = false;

    
    KConfig *config = kapp->getConfig();
    config->setGroup( "Settings" );
    readProperties( config );

    // Sven: We leave this out, since it's in other group.
    config->setGroup( "Cache" );
    bool on;
    on=config->readBoolEntry( "CacheEnabled", true );
    HTMLCache::enableCache(on);
    on=config->readBoolEntry( "SaveCacheEnabled", true );
    HTMLCache::enableSaveCache(on);

    initGUI();
    
    GUI_ready = true; //sven: finished constructor;
    
    windowList->setAutoDelete( false );
    windowList->append( this );

    if ( _url )
      view->openURL( _url ); // sven: local props get read here!!!
    this->resize(kfmgui_width,kfmgui_height);
}

KfmGui* KfmGui::findWindow( const char *_url )
{
    KfmGui *w;
    for ( w = windowList->first(); w != 0L; w = windowList->next() )
	if ( strcmp( _url, w->getURL() ) == 0 )
	    return w;
    
    return 0L;
}

void KfmGui::initGUI()
{
    setMinimumSize( 100, 100 );

    initPanner();
    initTreeView();
    initStatusBar();
    initView();
    initMenu();
    initToolBar();
    QGridLayout *gl = new QGridLayout( pannerChild0, 1, 1 );
    gl->addWidget( treeView, 0, 0 );
    gl = new QGridLayout( pannerChild1, 1, 1 );
    gl->addWidget( view, 0, 0 );
}

void KfmGui::initPanner()
{
    if ( bTreeView )
	panner = new KPanner( this, "_panner", KPanner::O_VERTICAL, 30 );
    else
	panner = new KPanner( this, "_panner", KPanner::O_VERTICAL, 0 );
    
    pannerChild0 = panner->child0();
    pannerChild1 = panner->child1();    
    connect( panner, SIGNAL( positionChanged() ), this, SLOT( slotPannerChanged() ) );

    setView( panner );

    // [KDE1 Revival 2026] 双内核自动激活：KFM_WEBENGINE=1 时启动即切 WebKit
    // 新内核（500ms 后窗口就绪再切，避免构造期窗口尺寸未定）
    if ( getenv( "KFM_WEBENGINE" ) )
	QTimer::singleShot( 500, this, SLOT(slotWebEngine()) );
    panner->show();
}

void KfmGui::initTreeView()
{
    treeView = new KFMDirTree( pannerChild0, this );

    connect( treeView, SIGNAL( urlSelected( const char *, int ) ),
	     this, SLOT( slotTreeUrlSelected( const char *, int ) ) );

    if ( bTreeView )
    {
	bTreeViewInitialized = true;
	treeView->fill();  
    }

    treeView->show();
}

void KfmGui::initStatusBar()
{
    statusBar = new KStatusBar( this );
 
    statusBar->insertItem( (char*)klocale->translate("KFM"), 1 );
    
    //statusBar->show();
    setStatusBar( statusBar );
    if ( !showStatusbar )
	statusBar->enable( KStatusBar::Hide );
}

void KfmGui::initMenu()
{
    KStdAccel stdAccel;
    menuNew = new KNewMenu();

    mfile = new QPopupMenu;
    CHECK_PTR( mfile );
  
    mfile->clear();
    mfile->insertSeparator();
    { int k1id = mfile->insertItem( klocale->translate("New &Window"), this, SLOT(slotNewWindow()) ); mfile->setAccel( stdAccel.openNew(), k1id ); }  /* TQt3 迁移 */
    mfile->insertSeparator();
    mfile->insertItem( klocale->translate("&Run..."), 
		       this, SLOT(slotRun()) );
    { int k1id = mfile->insertItem( klocale->translate("Open &Terminal"), this, SLOT(slotTerminal()) ); mfile->setAccel( CTRL+Key_T, k1id ); }  /* TQt3 迁移 */
    mfile->insertSeparator();
    { int k1id = mfile->insertItem( klocale->translate("&Open Location..."), this, SLOT(slotOpenLocation()) ); mfile->setAccel( stdAccel.open(), k1id ); }  /* TQt3 迁移 */
    { int k1id = mfile->insertItem( klocale->translate("&Find"), this, SLOT(slotToolFind()) ); mfile->setAccel( stdAccel.find(), k1id ); }  /* TQt3 迁移 */
    mfile->insertSeparator();
    { int k1id = mfile->insertItem( klocale->translate("&Print..."), this, SLOT(slotPrint()) ); mfile->setAccel( stdAccel.print(), k1id ); }  /* TQt3 迁移 */
    mfile->insertSeparator();        
    { int k1id = mfile->insertItem( klocale->translate("&Close"), this, SLOT(slotClose()) ); mfile->setAccel( stdAccel.close(), k1id ); }  /* TQt3 迁移 */
// This was meant for testing only. (hoelzer)
//    file->insertItem( klocale->translate("&Quit..."),  
//		      this, SLOT(slotQuit()), stdAccel.quit() );

    connect( mfile, SIGNAL(aboutToShow()), this, SLOT(slotFile()) );

    /*CT QPopupMenu *  -- make it visible outside this method */
    edit = new QPopupMenu;
    CHECK_PTR( edit );
    edit->insertItem( klocale->translate("&Copy"), this, SLOT(slotCopy()) );  /* TQt3 迁移：整 accel 参删 */
    edit->insertItem( klocale->translate("&Paste"), this, 
		      SLOT(slotPaste()), stdAccel.paste() );
    { int k1id = edit->insertItem( klocale->translate("&Move to Trash"), this, SLOT(slotTrash()) ); edit->setAccel( stdAccel.cut(), k1id ); }  /* TQt3 迁移 */
    edit->insertItem( klocale->translate("&Delete"), this, 
		      SLOT(slotDelete()), CTRL+Key_Delete);
    // This can't be Key_Delete only, it breaks deleting in LineEdits

    edit->insertSeparator();
    edit->insertItem( klocale->translate("&Select"), this, 
		      SLOT(slotSelect()), CTRL+Key_S );
    edit->insertItem( klocale->translate("Select &all"), this,
                      SLOT(slotSelectAll()), CTRL+Key_A );
    edit->insertSeparator();
    edit->insertItem( klocale->translate("&Find in page..."), this, 
		      SLOT(slotFind()), Key_F2 );
    edit->insertItem( klocale->translate("Find &next"), this, 
		      SLOT(slotFindNext()), Key_F3 );
    edit->insertSeparator();
    edit->insertItem( klocale->translate( "Show History" ), this, 
                      SLOT( slotShowHistory() ) );
    edit->insertItem( klocale->translate( "Show Cache" ), this, 
                      SLOT( slotShowCache() ) );
    edit->insertItem( klocale->translate("Mime &Types"), this, 
		      SLOT(slotEditMimeTypes()) );
    edit->insertItem( klocale->translate("App&lications"), this, 
		      SLOT(slotEditApplications()) );

    if ( sumode )
    {
	edit->insertSeparator();
	edit->insertItem( klocale->translate("Global Mime Types"), this, 
		      SLOT(slotEditSUMimeTypes()) );
	edit->insertItem( klocale->translate("Global Applications"), this, 
		      SLOT(slotEditSUApplications()) );
    }
    connect ( edit, SIGNAL (aboutToShow()), this, SLOT (slotUpdateEditMenu()));

    mview = new QPopupMenu;
    CHECK_PTR( mview );
    mview->setCheckable(true);
    mview->insertItem( klocale->translate("Show Hi&dden Files"),
		       this, SLOT(slotShowDot()) );
    mview->insertItem( klocale->translate("Show Tr&ee"), 
		       this, SLOT(slotShowTreeView()) );
    mview->insertItem( klocale->translate("Sho&w Thumbnails"),
		       this, SLOT(slotShowSchnauzer()) );
    mview->insertItem( klocale->translate("&HTML View"),
		       this, SLOT(slotViewHTML()) );
    mview->insertSeparator();
    mview->insertItem( klocale->translate("&Icon View"),
		       this, SLOT(slotIconView()) );
    mview->insertItem( klocale->translate("&Text View"),
		       this, SLOT(slotTextView()) );
    mview->insertItem( klocale->translate("&Long View"),
		       this, SLOT(slotLongView()) );
    mview->insertItem( klocale->translate("&Short View"),
		       this, SLOT(slotShortView()) );
    /* mview->insertSeparator();
    mview->insertItem( klocale->translate("Split &window"),
		       this, SLOT(slotSplitWindow()) ); */
    mview->insertSeparator();
    mview->insertItem( klocale->translate("Rel&oad Tree"),
		       this, SLOT(slotReloadTree()) );
    mview->insertItem( klocale->translate("&Reload Document"),
		       view, SLOT(slotReload()), Key_F5 );
    mview->insertItem( klocale->translate("Rescan &Bindings"),
		       this, SLOT(slotRescanBindings()) );
    mview->insertSeparator();
    mview->insertItem( klocale->translate("&View Frame Source"),
		       this, SLOT(slotViewFrameSource()) );
    mview->insertItem( klocale->translate("View Docu&ment Source"),
		       this, SLOT(slotViewDocumentSource()) );

    // charsets
    mcharset = new QPopupMenu();

    QStrList charsets = kapp->getCharsets()->available();
    mcharset->setCheckable(true);
    charsets.insert( 0, klocale->translate("Auto") );
    for(charsets.first(); charsets.current(); charsets.next())
	mcharset->insertItem( charsets.current() );
    mcharset->setItemChecked( mcharset->idAt( 0 ), true );

    connect( mcharset, SIGNAL( activated( int ) ), 
	     this, SLOT( slotSetCharset( int ) ) );
    mview->insertSeparator();
    mview->insertItem( klocale->translate("Document Encodin&g"), mcharset );

    // [KDE1 Revival 2026] 双内核：View 菜单末尾追加（不影响 handleViewMenu
    // 依赖的 idAt(0..17) 位置索引）——WebKit 新内核开关
    mview->insertSeparator();
    int webItemId = mview->insertItem(
        klocale->translate("Browse with &WebKit Engine"), this,
        SLOT(slotWebEngine()), CTRL+SHIFT+Key_W );
    mview->setItemChecked( webItemId, false );

    
    mview->setItemChecked( mview->idAt( 0 ), showDot );
    mview->setItemChecked( mview->idAt( 1 ), bTreeView );
    mview->setItemChecked( mview->idAt( 2 ), visualSchnauzer );
    mview->setItemChecked( mview->idAt( 3 ), bViewHTML );

    mgo = new QPopupMenu;
    CHECK_PTR( mgo );
    slotUpdateHistoryMenu(); // we have to create it on the first time, for the
    // key bindings to work. It does the connect, so we don't it here. David.
    
    mcache = new QPopupMenu;
    CHECK_PTR( mcache );
    mcache->insertItem( klocale->translate( "Clear Cache" ),
			this, SLOT( slotClearCache() ) );
    mcache->insertSeparator();
    mcache->insertItem( klocale->translate( "Always look in cache" ),
			this, SLOT( slotCacheOn() ) );
    mcache->insertItem( klocale->translate( "Never look in cache" ),
			this, SLOT( slotCacheOff() ) );
    mcache->insertItem( klocale->translate( "Always save cache" ),
			this, SLOT( slotSaveCacheOn() ) );
    mcache->insertItem( klocale->translate( "Never save cache" ),
			this, SLOT( slotSaveCacheOff() ) );
    
    mcache->setItemChecked( mcache->idAt( 2 ), HTMLCache::isEnabled());
    mcache->setItemChecked( mcache->idAt( 3 ), !HTMLCache::isEnabled());
    mcache->setItemChecked( mcache->idAt( 4 ), HTMLCache::isSaveEnabled() );
    mcache->setItemChecked( mcache->idAt( 5 ), !HTMLCache::isSaveEnabled() );

    moptions = new QPopupMenu;
    CHECK_PTR( moptions );
    moptions->setCheckable(true);
    moptions->insertItem( klocale->translate("Show &Menubar"),
			  this, SLOT(slotShowMenubar()) );
    moptions->insertItem( klocale->translate("Show &Statusbar"),
			  this, SLOT(slotShowStatusbar()) );
    moptions->insertItem( klocale->translate("Show &Toolbar"),
			  this, SLOT(slotShowToolbar()) );
    moptions->insertItem( klocale->translate("Show &Location bar"),
			  this, SLOT(slotShowLocationBar()) );
    moptions->insertSeparator();
    moptions->insertItem( klocale->translate("Sa&ve Settings"),
                          this, SLOT(slotSaveSettings()) );
    moptions->insertItem( klocale->translate("Save Settings for this &URL"),
                          view, SLOT(slotSaveLocalProperties()));
    if (pkfm->isURLPropesEnabled())
      moptions->setItemEnabled(moptions->idAt( 6 ), true);
    else
      moptions->setItemEnabled(moptions->idAt( 6 ), false);
    moptions->insertSeparator();

    moptions->insertItem( klocale->translate("&Cache"), mcache );
    moptions->insertItem( klocale->translate("&Configure File Manager..."),
                          this, SLOT(slotConfigureFileManager()));
    moptions->insertItem( klocale->translate("Configure &Browser..."),
                          this, SLOT(slotConfigureBrowser()));
    
    moptions->setItemChecked( moptions->idAt( 0 ), showMenubar );
    moptions->setItemChecked( moptions->idAt( 1 ), showStatusbar );
    moptions->setItemChecked( moptions->idAt( 2 ), showToolbar );
    moptions->setItemChecked( moptions->idAt( 3 ), showLocationBar );

    switch (viewMode) 
      {
      case ICON_VIEW:
	mview->setItemChecked( mview->idAt( 5 ), true );
	break;
      case TEXT_VIEW:
	mview->setItemChecked( mview->idAt( 6 ), true );
	break;
      case LONG_VIEW:
	mview->setItemChecked( mview->idAt( 7 ), true );
	break;
      case SHORT_VIEW:
	mview->setItemChecked( mview->idAt( 8 ), true );
	break;
      }

    QPopupMenu *nav = new QPopupMenu;
    CHECK_PTR( nav );

    bookmarkMenu = new QPopupMenu;
    CHECK_PTR( bookmarkMenu );
    connect( bookmarkManager, SIGNAL( changed() ), 
	     this, SLOT( slotBookmarksChanged() ) );
    slotBookmarksChanged();
    

    QString about_text;
    about_text.sprintf("KFM Ver. %s\n\n%s", kfm_getrev(),
klocale->translate("Author: Torben Weis\nweis@kde.org\n\nHTML widget by Martin Jones\nmjones@kde.org\n\nProxy Manager by Lars Hoss\nLars.Hoss@munich.netsurf.de\n\nCurrent maintainer: David Faure\nfaure@kde.org") );
    // added, but apart in order no to break existing translations for the
    // first part, and because sprintf cores dump if >256 !
    about_text += i18n("\nPlease submit bugs and wishes as explained on http://bugs.kde.org");

    QPopupMenu *help = kapp->getHelpMenu(false, about_text);
    CHECK_PTR( help );

    menu = new KMenuBar( this );
    if ( sumode )
    {
      menu->setBackgroundColor( red );
      connect (menu, SIGNAL(moved(menuPosition)), this, SLOT(slotPaintRed()));
    }
    
    CHECK_PTR( menu );
    menu->insertItem( klocale->translate("&File"), mfile );
    menu->insertItem( klocale->translate("&Edit"), edit );
    menu->insertItem( klocale->translate("&View"), mview );
    menu->insertItem( klocale->translate("&Go"), mgo );
    menu->insertItem( klocale->translate("&Bookmarks"), bookmarkMenu );
    menu->insertItem( klocale->translate("&Options"), moptions );
    menu->insertSeparator();
    menu->insertItem( klocale->translate("&Help"), help );
    //menu->show();
    
    setMenu( menu );
}

void KfmGui::slotPaintRed()
{
  menu->setBackgroundColor( red );
}

void KfmGui::enableToolbarButton( int id, bool enable )
{
    if ( toolbarButtons == 0L )
	return;
    
    toolbarButtons->setItemEnabled( id, enable );
    // apply this to the related menu item, for Up, Back and Forward
    if (id <= 2) mgo->setItemEnabled(mgo->idAt( id ), enable );

}

void KfmGui::initToolBar()
{
    QString file, path;
    QPixmap pixmap;
    toolbarButtons = new KToolBar(this, "kfmwin-toolbar");
    path = kapp->kde_toolbardir() + "/";

    pixmap.load(path + "up.xpm");
    toolbarButtons->insertButton(pixmap, 0, SIGNAL( clicked() ), view, 
			  SLOT( slotUp() ), false, 
			  klocale->translate("Up"));
    
    pixmap.load(path + "back.xpm");
    toolbarButtons->insertButton(pixmap, 1, SIGNAL( clicked() ), view, 
			  SLOT( slotBack() ), false, 
			  klocale->translate("Back"));
    
    pixmap.load(path + "forward.xpm");
    toolbarButtons->insertButton(pixmap, 2, SIGNAL( clicked() ), view, 
			  SLOT( slotForward() ), false, 
			  klocale->translate("Forward"));
    
    pixmap.load(path + "home.xpm");
    toolbarButtons->insertButton(pixmap, 3, SIGNAL( clicked() ), this, 
			  SLOT( slotHome() ), true, 
			  klocale->translate("Home") );
    
    toolbarButtons->insertSeparator();
    
    pixmap.load(path + "reload.xpm");
    toolbarButtons->insertButton(pixmap, 4, SIGNAL( clicked() ), view, 
			  SLOT( slotReload() ), true, 
			  klocale->translate("Reload") );

    toolbarButtons->insertSeparator();
    
    pixmap.load(path + "editcopy.xpm");
    toolbarButtons->insertButton(pixmap, 5, SIGNAL( clicked() ), this, 
			  SLOT( slotCopy() ), true, 
			  klocale->translate("Copy") );
    
    pixmap.load(path + "editpaste.xpm");
    toolbarButtons->insertButton(pixmap, 6, SIGNAL( clicked() ), this, 
			  SLOT( slotPaste() ), true, 
			  klocale->translate("Paste") );
    
    toolbarButtons->insertSeparator();
    
    pixmap.load(path + "help.xpm");
    toolbarButtons->insertButton(pixmap, 7, SIGNAL( clicked() ), kapp, 
			  SLOT( appHelpActivated() ), true, 
			  klocale->translate("Help"));
    
    toolbarButtons->insertSeparator();
    
    pixmap.load(path + "stop.xpm");
    toolbarButtons->insertButton(pixmap, 8, SIGNAL( clicked() ), this, 
			  SLOT( slotStop() ), false, 
			  klocale->translate("Stop"));

    path = kapp->kde_datadir() + "/kfm/pics/";

    pixmap.load( path + "/kde1.xpm" );
    
    toolbarButtons->insertButton(pixmap, 9, SIGNAL( clicked() ), this, 
			  SLOT( slotNewWindow() ), false );
    toolbarButtons->setItemEnabled( 9, true );
    toolbarButtons->alignItemRight( 9, true );
    
    // Load animated logo
    if ( animatedLogo->count() == 0 )
    {
	animatedLogo->setAutoDelete( true );
	for ( int i = 1; i <= 9; i++ )
	{
	    QString n;
	    n.sprintf( "/kde%i.xpm", i );
	    QPixmap *p = new QPixmap();
	    p->load( path + n );
	    if ( p->isNull() )
	    {
		QString e;
		e << klocale->translate( "Could not load icon\n" ) << n.data();
		QMessageBox::warning( this, klocale->translate( "KFM Error" ), e.data() );
	    }
	    animatedLogo->append( p );
	}
    }
	    
    addToolBar( toolbarButtons );
    toolbarButtons->setBarPos( toolbarPos );
    //toolbarButtons->show();                
    if ( !showToolbar )
	toolbarButtons->enable( KToolBar::Hide );

    toolbarURL = new KToolBar(this, "URL History");
    QLabel *locationLabel = new QLabel(i18n("Location:"), toolbarURL,
				       "locationLabel");
    locationLabel->adjustSize();
    toolbarURL->insertWidget(0, locationLabel->width(), locationLabel);
    toolbarURL->insertLined( "", TOOLBAR_URL_ID,
			  SIGNAL( returnPressed() ), this, SLOT( slotURLEntered() ) );
    KLined *lined = toolbarURL->getLined (TOOLBAR_URL_ID); //Sven
    completion = new KURLCompletion();
    connect ( lined, SIGNAL (completion()),
	      completion, SLOT (make_completion()));
    connect ( lined, SIGNAL (rotation()),
	      completion, SLOT (make_rotation()));
    connect ( lined, SIGNAL (textChanged(const char *)),
	      completion, SLOT (edited(const char *)));
    connect ( completion, SIGNAL (setText (const char *)),
	      lined, SLOT (setText (const char *)));
    addToolBar( toolbarURL );
    toolbarURL->setFullWidth( TRUE );
    toolbarURL->setItemAutoSized( TOOLBAR_URL_ID, TRUE );
    toolbarURL->setBarPos( locationBarPos );
    //toolbarURL->show();                
    if ( !showLocationBar )
	toolbarURL->enable( KToolBar::Hide );
    enableToolbarButton(0,false);
    enableToolbarButton(1,false);
    enableToolbarButton(2,false);
}

void KfmGui::initView()
{
    view = new KfmView( this, pannerChild1, "" );
    CHECK_PTR( view );
    
    connect( view, SIGNAL( setTitle(const char *)), this, SLOT( slotTitle(const char *)) );
    connect( view, SIGNAL( historyUpdate( bool, bool ) ), this, SLOT( slotUpdateHistory( bool, bool ) ) );
    connect( view, SIGNAL( newURL( const char * ) ), this, SLOT( slotNewURL( const char * ) ) );
    connect( view, SIGNAL( textSelected( KHTMLView *, bool) ), this, SLOT( slotTextSelected( KHTMLView *, bool) ) );
    connect( view, SIGNAL( documentStarted( KHTMLView * ) ), 
	     this, SLOT( slotAddWaitingWidget( KHTMLView * ) ) );
    connect( view, SIGNAL( documentDone( KHTMLView * ) ), 
	     this, SLOT( slotRemoveWaitingWidget( KHTMLView * ) ) );
    view->show();
    view->setFocus();
}

void KfmGui::closeEvent( QCloseEvent *e )
{
    e->accept();

    delete this;
}

void KfmGui::updateView()
{
    view->slotUpdateView();
    //  update tree view   Sep 5 rjakob
    if (bTreeView && pkfm->isTreeViewFollowMode())
        treeView->slotshowDirectory(toolbarURL->getLinedText(TOOLBAR_URL_ID));
}


/*****************************************************************************
  [KDE1 Revival 2026] kfm 双内核（路线乙目标 3）
  ┌─ What : 「Browse with WebKit Engine」开关——开启时在 view 区域上叠
  │         XEmbed socket 并 fork kde1-webview（WebKit2GTK 承载现代网页）；
  │         关闭时结束子进程并还原 1999 原引擎视图
  │  Why  : 原 KHTML 引擎只认 HTML 3.2，现代站点全部无法渲染；双内核共存
  │         使 kfm 既能看现代网页又保留历史原貌（AGENTS.md §1 目标 3）
  │  Who  : View 菜单触发；KfmView::openURL 检测 webEngineOn 后把 URL
  │         经 webEngineNavigate 转发给子进程（stdin 管道）
  │  When : 会话期内随时切换；kfm 退出时析构清理
  │  How  : 伪代码：
  │         开启：view->winId() 内按 view 几何创建 socket 子窗口 →
  │               pipe + fork + exec kde1-webview(socket, 当前URL) → 显示
  │         关闭：关闭管道（子进程 stdin EOF 自动退出）→ waitpid →
  │               XDestroyWindow(socket) → view->repaint
  *****************************************************************************/
void KfmGui::slotWebEngine()
{
    if ( !webEngineOn ) {
        // 开启：在 view 之上叠 XEmbed socket（与 view 同几何）
        Window parent = view->winId();
        XWindowAttributes wa;
        if ( !XGetWindowAttributes( qt_xdisplay(), parent, &wa ) ) {
            return;
        }
        webEngineSocket = (unsigned long)XCreateSimpleWindow(
            qt_xdisplay(), parent, 0, 0,
            (unsigned)wa.width, (unsigned)wa.height, 0, 0, 0xffffff );
        if ( !webEngineSocket )          // socket 创建失败即中止
            return;
        XMapWindow( qt_xdisplay(), (Window)webEngineSocket );
        XFlush( qt_xdisplay() );
        fprintf(stderr, "[kde1-revival] webview socket=%lu 尺寸 %dx%d\n",
                webEngineSocket, wa.width, wa.height);

        // fork 子进程：stdin 用管道，stdout/stderr 继承
        int fds[2];
        if ( pipe( fds ) != 0 ) {
            /* [2026-08-31] pipe 失败回滚已建 socket（原实现泄漏窗口且
               webEngineOn 未置位，下次 toggle 再叠一个） */
            XDestroyWindow( qt_xdisplay(), (Window)webEngineSocket );
            webEngineSocket = 0;
            return;
        }
        webEnginePid = fork();
        if ( webEnginePid == 0 ) {
            close( fds[1] );
            dup2( fds[0], STDIN_FILENO );
            char xidbuf[32], *args[4];
            snprintf( xidbuf, sizeof(xidbuf), "%lu", webEngineSocket );
            args[0] = (char*)"kde1-webview";
            args[1] = xidbuf;
            const char *initurl = view->getURL();
            /* [2026-08-31] 空视图守卫：getURL 可能为 0/空串，传空参数
               会使子进程 argc 回落 example.com 之外再被 execvp 当非法参数 */
            args[2] = (char*)( initurl && *initurl ? initurl
                                                   : "about:blank" );
            args[3] = 0;
            execvp( "kde1-webview", args );
            _exit(127);			// exec 失败
        }
        if ( webEnginePid < 0 ) {
            /* [2026-08-31] fork 失败同样回滚（原实现照常置 webEngineOn） */
            close( fds[0] ); close( fds[1] );
            XDestroyWindow( qt_xdisplay(), (Window)webEngineSocket );
            webEngineSocket = 0;
            webEnginePid = -1;
            return;
        }
        close( fds[0] );
        webEngineIn = fds[1];
        webEngineOn = true;
        view->repaint();
    } else {
        /* 关闭：管道 EOF 令子进程自行退出。退出后的收尸交由全局 SIGCHLD
         * 处理器（main.cpp sig_handler 的 waitpid(-1, WNOHANG) 循环）——
         * [2026-08-31] 移除 UI 线程内阻塞 waitpid：它与 SIGCHLD 抢收子进程
         * （多数时候返回 ECHILD），且 webview 卡死时 kfm 界面会跟着冻结 */
        if ( webEngineIn >= 0 ) { close( webEngineIn ); webEngineIn = -1; }
        webEnginePid = -1;
        if ( webEngineSocket ) {
            XDestroyWindow( qt_xdisplay(), (Window)webEngineSocket );
            webEngineSocket = 0;
        }
        webEngineOn = false;
        view->repaint();
    }
    // 同步菜单勾选态
    mview->setItemChecked( mview->idAt( mview->count() - 1 ), webEngineOn );
}

// 新内核激活时由 KfmView::openURL 调用：URL 写入子进程 stdin
void KfmGui::webEngineNavigate( const char *url )
{
    if ( !webEngineOn || webEngineIn < 0 || !url || !*url )
        return;
    char buf[2048];
    int n = snprintf( buf, sizeof(buf), "URL %s\n", url );
    if ( n <= 0 )
        return;
    if ( n >= (int)sizeof(buf) )
        n = (int)sizeof(buf) - 1;		// snprintf 截断防护
    /* [2026-08-31] 循环写全：管道缓冲不足时的短写会让 webview 收到半行命令 */
    const char *p = buf;
    while ( n > 0 ) {
        ssize_t w = write( webEngineIn, p, (size_t)n );
        if ( w <= 0 ) {
            if ( w < 0 && errno == EINTR )
                continue;
            break;			// 管道破裂：子进程已退出，静默放弃
        }
        p += w; n -= (int)w;
    }
}

bool KfmGui::webEngineActive()
{
    return webEngineOn;
}

//CT 16Dec1998 -- handle the View menu according with the type of view
void KfmGui::handleViewMenu(bool _bHtmlMode) 
{
  bHtmlMode = _bHtmlMode;
  edit->setItemEnabled(edit->idAt( 2 ), !bHtmlMode);
  edit->setItemEnabled(edit->idAt( 3 ), !bHtmlMode);

  mview->setItemEnabled(mview->idAt( 0 ), !bHtmlMode );
  mview->setItemEnabled(mview->idAt( 2 ), !bHtmlMode );
  mview->setItemEnabled(mview->idAt( 5 ), !bHtmlMode );
  mview->setItemEnabled(mview->idAt( 6 ), !bHtmlMode );
  mview->setItemEnabled(mview->idAt( 7 ), !bHtmlMode );
  mview->setItemEnabled(mview->idAt( 8 ), !bHtmlMode );
  mview->setItemEnabled(mview->idAt( 10 ), !bHtmlMode ); // reload tree
  mview->setItemEnabled(mview->idAt( 14 ), bHtmlMode && view->isFrameSet()); // frame source
  mview->setItemEnabled(mview->idAt( 15 ), bHtmlMode ); // document source
  mview->setItemEnabled(mview->idAt( 17 ), bHtmlMode ); // document encoding
}
//CT

void KfmGui::slotReloadTree()
{
    if ( bTreeView )
	treeView->update();
}

void KfmGui::slotShowCache()
{
  HTMLCache::save();
  
  QString s = KFMPaths::CachePath().data();
  s += "/index.html";
  QString tmp( "file:" );
  tmp += s;
  view->openURL( tmp );
}

void KfmGui::slotShowHistory()
{
  QString s = KFMPaths::CachePath().data();
  s += "/history.html";
  Kfm::saveHTMLHistory( s );

  QString tmp( "file:" );
  tmp += s;
  view->openURL( tmp );
}

void KfmGui::slotClearCache()
{
  HTMLCache::clear();
  // Send notifications to windows displaying 
  // the cache directory
  KURL k (KFMPaths::CachePath().data());
  KIOServer::sendNotify(k.url());
  // or the 'contents of the cache' page
  QString s = k.url()+"/index.html";
  KIOServer::sendNotify(s);
}

void KfmGui::slotCacheOn()
{
    mcache->setItemChecked( mcache->idAt( 2 ), true );
    mcache->setItemChecked( mcache->idAt( 3 ), false );

    HTMLCache::enableCache( true );
}

void KfmGui::slotCacheOff()
{
    mcache->setItemChecked( mcache->idAt( 2 ), false );
    mcache->setItemChecked( mcache->idAt( 3 ), true );

    HTMLCache::enableCache( false );
}

void KfmGui::slotSaveCacheOn()
{
    mcache->setItemChecked( mcache->idAt( 4 ), true );
    mcache->setItemChecked( mcache->idAt( 5 ), false );

    HTMLCache::enableSaveCache( true );
}

void KfmGui::slotSaveCacheOff()
{
    mcache->setItemChecked( mcache->idAt( 4 ), false );
    mcache->setItemChecked( mcache->idAt( 5 ), true );

    HTMLCache::enableSaveCache( false );
}

void KfmGui::slotURLEntered()
{
   openFilteredURL( toolbarURL->getLinedText( TOOLBAR_URL_ID ) );
}

void KfmGui::openFilteredURL ( const char * _url )
{
    if ( view->getActiveView() )
    {
        QString url = _url;
        // strip any leading and trailing spaces.
        url = url.stripWhiteSpace();
        // Exit if the user did not enter a URL
        if ( url.isEmpty() )
	        return;
        // If url starts with // and contains no '~',
        // clean it up so kfm can behave much like
        // a normal shell would. (Dawit A.)
        if ( url.find("//") == 0 && url.find ('~') == -1 )
           url = QDir:: cleanDirPath (url.data());
        // Strip off "file:/" in order to properly
        // expand local URLs when necessary.
        if ( url.find ("file:") == 0 )
        {
           url.remove ( 0, 5 );
           // Home directory preceeded by "file:/" or "file://"
           if ( url.find("/~") == 0 || url.find("//~") == 0 )
              url.remove( 0, url.find('~') );
           else
              url = QDir:: cleanDirPath (url.data());
        }
        // Home directory?
        if ( url.find ( QRegExp ( "^~.*" ) ) == 0 )
        {
            int length = url.length();
            int index = url.find ( "/" );
            if ( length == 1 || index == 1 )
                url.replace ( 0, 1, QDir::homeDirPath().data() );
            else
            {
              struct passwd *dir = ((index == -1) ? getpwnam(url.mid(1,length).data()) : getpwnam( url.mid(1,index-1).data()));
               if ( !dir ) return; // unknown user
              (index == -1) ? url.replace (0, length,  dir->pw_dir) : url.replace (0, index, dir->pw_dir);
            }
        }
        // An Easter egg ? Let's have some fun first !!
	    else if ( url.lower() == "about:kde" )
        {
    		url = getenv( "KDEURL" );
		    if ( url.isEmpty() )
			    /* [2026-08-31] 默认主页 http→https：明文 http 会被现代站点
			       重定向，旧内核不跟随跳转即白屏；https 由新内核可开 */
			    url = "https://www.kde.org";
        }
        // No protocol, but begins with www? (sven)
    	else if ( url.find( "www.", 0, false ) == 0 )
            url.prepend("http://");  // Is this necessary since we default to "http://" below ?? (Dawit A.)

    	// No protocol, but begins with "ftp."?  (sven)
	    else if ( url.find( "ftp.", 0, false ) == 0 )
            url.prepend("ftp://");

        // Valid URL?
        // Do nothing if url, for our purposes here, is VALID.
        // This check is case in-sensitive . (Dawit A.)
        else if ( url.find ( QRegExp ("[a-zA-Z0-9\\+\\.\\-]+:/?/?.*") ) == 0 ||
                  url.find ( QRegExp ("^//*.*") ) == 0 ) ;
        /*
           When all else fails, check if the user entered a URL that
           exists under the current directory. If it does not, attach
           "http://" as default protocol. This enables us to provide
           short URL support to users, Ex. "linux.org" (Dawit A)
        */
        else
        {
            KURL path ( getURL() );
            QDir::setCurrent ( path.directory()  );
            QFileInfo f ( QDir::currentDirPath().append( "/" ).append( url ).data() );
            if ( f.exists() )
                url = f.filePath();
            else if ( url[ url.length()-1 ] != ':' && url[0] != '/' &&
                      url.find ('.') > 0 && url.find(' ') == -1 )
                url = url.prepend ("http://");
        }
    	KURL u( url.data() );
    	if ( u.isMalformed() )
	    {
	        QString tmp;
    	    tmp << klocale->translate("Malformed URL\n") << toolbarURL->getLinedText( TOOLBAR_URL_ID );
	        QMessageBox::critical( (QWidget*)0L, klocale->translate( "KFM Error" ), tmp );
            return;
        }
        view->openURL( u.url().data() );
        view->getActiveView()->setFocus();
        //  update tree view Sep 5 rjakob
        if ( u.protocol() == "file" )
            if (bTreeView && pkfm->isTreeViewFollowMode())
                treeView->slotshowDirectory(url.data()+5);
    }
}

void KfmGui::setToolbarURL( const char *_url )
{
    KURL u(_url);
    u.setPassword(""); // hide password
    QString url(u.url());
    toolbarURL->setLinedText( TOOLBAR_URL_ID, url.data() );
    //  update tree view Sep 5 rjakob
    if (u.isLocalFile())
      if (bTreeView && pkfm->isTreeViewFollowMode())
         treeView->slotshowDirectory(url.data()+5);

}

void KfmGui::slotNewURL( const char *_url )
{
    setToolbarURL( _url );

    if ( historyList.find( _url ) == -1 )
	return;
    
    historyList.insert( 0, _url );
    /* toolbarURL->clearCombo( TOOLBAR_URL_ID );
    toolbarURL->insertComboList( TOOLBAR_URL_ID, &historyList, 0 ); */
}

void KfmGui::slotRescanBindings()
{
    KMimeType::clearAll();
    KMimeType::init();
    if ( KRootWidget::getKRootWidget() )
	KRootWidget::getKRootWidget()->update();

    KfmGui *win;
    for ( win = windowList->first(); win != 0L; win = windowList->next() )
	win->updateView();

    KNewMenu::fillTemplates(); // re-read the Templates
    KWM::sendKWMCommand("krootwm:refreshNew"); // and ask krootwm to do so too
}

void KfmGui::slotRun()
{
    if ( view->getActiveView() )
	view->getActiveView()->slotRun();
}

void KfmGui::slotTerminal()
{
    if ( view->getActiveView() )
	view->getActiveView()->slotTerminal();
}

void KfmGui::slotOpenURL( const char *_url )
{
    view->openURL( _url );
}

void KfmGui::slotEditSUMimeTypes()
{
    QString tmp = KApplication::kde_mimedir();
    view->openURL( tmp );
}

void KfmGui::slotEditSUApplications()
{
    QString tmp = KApplication::kde_appsdir();
    view->openURL( tmp );
}

void KfmGui::slotEditMimeTypes()
{
    QString tmp = KApplication::localkdedir();
    tmp += "/share/mimelnk";
    view->openURL( tmp );
}

void KfmGui::slotEditApplications()
{
    QString tmp = KApplication::localkdedir();
    tmp += "/share/applnk";
    view->openURL( tmp );
}

void KfmGui::slotPrint()
{
    if ( view->getActiveView() )
	view->getActiveView()->print();
}   

void KfmGui::slotPannerChanged()
{
    if ( panner->getSeparator() == 0 )
    {
      mview->setItemChecked( mview->idAt( 1 ), false );
      bTreeView = false;
    }
    else
    {
        if ( !bTreeViewInitialized )
        {
            bTreeViewInitialized = TRUE;
            treeView->fill();
        }
        if (!bTreeView && pkfm->isTreeViewFollowMode()) // only if tree view wasn't shown
            treeView->slotshowDirectory(toolbarURL->getLinedText(TOOLBAR_URL_ID));

        mview->setItemChecked( mview->idAt( 1 ), true );
        bTreeView = true;
    }
    
    resizeEvent( 0L );
}

void KfmGui::slotSplitWindow()
{
    view->getActiveView()->splitWindow();
}

void KfmGui::slotViewHTML( )
{
    bViewHTML = !mview->isItemChecked( mview->idAt(3) );
    mview->setItemChecked( mview->idAt( 3 ), bViewHTML);
    bViewHTMLLocal = bViewHTML; //force local mode too (sven)
    view->slotReload();
    // slotUpdateView isn't enough when switching off "HTML View"
}

void KfmGui::slotIconView()
{
    viewMode = ICON_VIEW;
    mview->setItemChecked( mview->idAt( 5 ), true );
    mview->setItemChecked( mview->idAt( 6 ), false );
    mview->setItemChecked( mview->idAt( 7 ), false );
    mview->setItemChecked( mview->idAt( 8 ), false );
    viewModeLocal = viewMode; //force local mode too (sven)
    view->slotUpdateView( false );
}

void KfmGui::slotLongView()
{
    viewMode = LONG_VIEW;
    mview->setItemChecked( mview->idAt( 5 ), false );
    mview->setItemChecked( mview->idAt( 6 ), false);
    mview->setItemChecked( mview->idAt( 7 ), true );
    mview->setItemChecked( mview->idAt( 8 ), false );
    viewModeLocal = viewMode; //force local mode too (sven)
    view->slotUpdateView( false );
}

void KfmGui::slotTextView()
{
    viewMode = TEXT_VIEW;
    mview->setItemChecked( mview->idAt( 5 ), false );
    mview->setItemChecked( mview->idAt( 6 ), true );
    mview->setItemChecked( mview->idAt( 7 ), false );
    mview->setItemChecked( mview->idAt( 8 ), false );
    viewModeLocal = viewMode; //force local mode too (sven)
    view->slotUpdateView( false );
}

void KfmGui::slotShortView()
{
    viewMode = SHORT_VIEW;
    mview->setItemChecked( mview->idAt( 5 ), false );
    mview->setItemChecked( mview->idAt( 6 ), false );
    mview->setItemChecked( mview->idAt( 7 ), false );
    mview->setItemChecked( mview->idAt( 8 ), true );
    viewModeLocal = viewMode; //force local mode too (sven)
    view->slotUpdateView( false );
}

void KfmGui::slotSelect()
{
    DlgLineEntry l( klocale->translate("Select files:"), "", this );
    if ( l.exec() )
    {
	QString pattern = l.getText();
	if ( pattern.length() == 0 )
	    return;

	QRegExp re( pattern, true, true );
	
	view->getActiveView()->select( re, true );
    }
}

void KfmGui::slotSelectAll()
{
    // if a QLineEdit is focused, the user really intends to go to
    // the beginning of the line when they press CTRL-A (a la sh, emacs).  
    if (focusWidget()->inherits("QLineEdit"))
    {
        KfmView::clipboard->clear();
        KfmView::clipboard->append(((QLineEdit*)focusWidget())->text());
        // Simulate a key event to the widget focused
        // We use Key_Home because Ctrl+A calls this function !
        QKeyEvent e( Event_KeyPress, Key_Home, 0, 0);
        QApplication::sendEvent( focusWidget(), &e);
    }
    else
        view->getActiveView()->select( 0L, true );
}

void KfmGui::slotFind()
{
    // if we haven't already created a find dialog then do it now
    if ( !findDialog )
    {
	findDialog = new KFindTextDialog();
	connect( findDialog, SIGNAL( find( const QRegExp & ) ),
		SLOT( slotFindNext( const QRegExp & ) ) );
    }

    // reset the find iterator
    view->getActiveView()->findTextBegin();

    findDialog->show();
}

void KfmGui::slotFindNext()
{
    if ( findDialog && !findDialog->regExp().isEmpty() )
    {
	// We have a find dialog, so use the reg exp it maintains.
	slotFindNext( findDialog->regExp() );
    }
    else
    {
	// no find has been attempted yet - open the find dialog.
	slotFind();
    }
}

void KfmGui::slotFindNext( const QRegExp &regExp )
{
    if ( !view->getActiveView()->findTextNext( regExp ) )
    {
	// We've reached the end of the document.
	// Can either stop searching and close the find dialog,
	// or start again from the top.
	if ( KMsgBox::yesNo( this, i18n( "Find Complete" ),
	    i18n( "Continue search from the top of the page?" ),
	    KMsgBox::DB_SECOND | KMsgBox::QUESTION ) == 1 )
	{
	    view->getActiveView()->findTextBegin();
	    slotFindNext( regExp );
	}
	else
	{
	    view->getActiveView()->findTextEnd();
	    findDialog->hide();
	}
    }
}

void KfmGui::slotBookmarksChanged()
{
    bookmarkMenu->clear();
    bookmarkMenu->disconnect( this );
    bookmarkMenu->insertItem( klocale->translate("&Edit Bookmarks..."), 
			      this, SLOT(slotEditBookmarks()) );

    fillBookmarkMenu( bookmarkManager->root(), bookmarkMenu );
    bookmarkMenu->setAccel( CTRL + Key_B, bookmarkManager->root()->id() );
    // set Ctrl+Key_B for toplevel "add bookmark" menu entry.
}

void KfmGui::fillBookmarkMenu( KBookmark *parent, QPopupMenu *menu )
{
    KBookmark *bm;
    
    menu->insertItem( i18n("&Add Bookmark"), parent->id() );
    menu->insertSeparator();

    connect( menu, SIGNAL( activated( int ) ),
	     SLOT( slotBookmarkSelected( int ) ) );
    
    for ( bm = parent->children()->first(); bm != NULL;
	  bm = parent->children()->next() )
    {
	if ( bm->type() == KBookmark::URL )
	{
	    menu->insertItem( *(bm->miniPixmap()),
                              stringSqueeze(bm->text(), 50), bm->id() );
	}
	else
	{	    
	    QPopupMenu *subMenu = new QPopupMenu;
	    menu->insertItem( *(bm->miniPixmap()),
                              stringSqueeze(bm->text(), 50), subMenu );
	    fillBookmarkMenu( bm, subMenu );
	}
    }
}

void KfmGui::slotFile()
{
    if( mfile->findItem( FIRSTFILEMENUITEM ) )
        mfile->removeItem( FIRSTFILEMENUITEM );  

    QString url = "file:";
    url.append(KFMPaths::TrashPath());
    if( view->getURL() == url ) 
        mfile->insertItem( klocale->getAlias(ID_STRING_TRASH), 
		       view, SLOT( slotEmptyTrashBin() ), 0, FIRSTFILEMENUITEM, 0 );
    else 
    {
      mfile->insertItem( klocale->translate("&New"), menuNew, FIRSTFILEMENUITEM, 0 );
      // The menu applies to the current directory
      menuNew->setPopupFiles( view->getURL() );
    }
}

void KfmGui::slotStop()
{
    view->slotStop();
}

void KfmGui::slotUpdateEditMenu ()
{
    // Get current selection (either URLs or text in a web page)
    QStrList ulist;
    QString txt;
    view->getActiveView()->getSelected( ulist );
    view->getActiveView()->getSelectedText ( txt );
    bool enable;

    edit->setItemEnabled ( edit->idAt (0), false );
    edit->setItemEnabled ( edit->idAt (2), false );
    edit->setItemEnabled ( edit->idAt (3), false );
    
    //tqDebug(focusWidget()->className());
    // If user has selected text in a web page,
    // or if a qlineedit has focus, enable only the Copy method.
    if ( !txt.isEmpty() || 
         (focusWidget()->inherits("QLineEdit") &&
          ((QLineEdit*)focusWidget())->text()[0] != '\0') )
    {
        edit->setItemEnabled ( edit->idAt (0), true ); // copy
        edit->setItemEnabled ( edit->idAt (2), false ); // trash
        edit->setItemEnabled ( edit->idAt (3), false ); // delete
    }

    // no 'else' here, because if URLs are selected, txt is not empty
    // If user has selected URLs
    if ( ulist.count() != 0 )
    {
        // Copy Item : if read possible
        enable = KIOServer::supports ( ulist, KIO_Read );
        edit->setItemEnabled ( edit->idAt (0), enable );
        // MoveToTrash Item - 
        // Not Enabled if current directory is the Trash folder itself, 
        // if the selected item is the Trash, (bugs if trash + other folder!)
        // or if KIO_Move is not supported.
        KURL u ( view->getActiveView()->getURL() );
        enable = ( KIOServer::supports ( ulist, KIO_Move ) &&
            !KIOServer::isTrash ( u.directory() ) &&
            !KIOServer::isTrash ( ulist ) );
        edit->setItemEnabled ( edit->idAt (2), enable );
        // Delete Item : if delete possible
        enable = KIOServer::supports ( ulist, KIO_Delete );
        edit->setItemEnabled ( edit->idAt (3), enable );
    }

    // Update for the paste menu based on the state of the clipboard.
    enable = ( KfmView::clipboard->count() != 0 );
    edit->setItemEnabled ( edit->idAt (1), enable );
} 

void KfmGui::slotUpdateHistoryMenu( )
{
    // Store current state of Up, Back & Forward menu items
    static bool itemsEnabled[3];
    for (int i=0; i<3; i++) itemsEnabled[i] = mgo->isItemEnabled( mgo->idAt( i ) );

    mgo->clear();
    mgo->disconnect( this );
    connect( mgo, SIGNAL(aboutToShow()), this, SLOT(slotUpdateHistoryMenu()) );

    // The go menu, with items in the same order as the toolbar.
    { int k1id = mgo->insertItem( klocale->translate( "&Up" ), this, SLOT(slotUp()) ); mgo->setAccel( ALT+Key_Up, k1id ); }  /* TQt3 迁移 */
    { int k1id = mgo->insertItem( klocale->translate( "&Back" ), this, SLOT(slotBack()) ); mgo->setAccel( ALT+Key_Left, k1id ); }  /* TQt3 迁移 */
    { int k1id = mgo->insertItem( klocale->translate( "&Forward" ), this, SLOT(slotForward()) ); mgo->setAccel( ALT+Key_Right, k1id ); }  /* TQt3 迁移 */
    { int k1id = mgo->insertItem( klocale->translate( "&Home" ), this, SLOT(slotHome()) ); mgo->setAccel( ALT+Key_Home, k1id ); }  /* TQt3 迁移 */
    mgo->insertSeparator();

    connect( mgo, SIGNAL( activated( int ) ),
	     SLOT( slotGoHistory( int ) ) );

    QStrList * hlist = Kfm::history();
    char * s;
    int id;
    for (id = 0, s = hlist->last(); (id<10) && (s != 0L); id++, s = hlist->prev()) {
        KURL u(s);
        u.setPassword(""); // hide password
        QString url(u.url());
        KURL::decodeURL(url); // we don't want encoded URLs in the menu
        mgo->insertItem ( stringSqueeze(url,100), id );
    }
    // Enable or disable Up, Back & Forward menu items
    for (int i=0; i<3; i++) mgo->setItemEnabled( mgo->idAt( i ), itemsEnabled[i] );
}

void KfmGui::slotUpdateHistory( bool _back, bool _forward )
{
    enableToolbarButton( 1, _back );
    enableToolbarButton( 2, _forward );
}

void KfmGui::slotHome()
{  
    QString url = "file:";
    url += QDir::homeDirPath().data();
    
    view->openURL( url.data() );
}

void KfmGui::addBookmark( const char *_title, const char *_url )
{
    QString p = kapp->localkdedir().data();
    QString bmFile = p + "/share/apps/kfm/bookmarks";

    KBookmark *root = bookmarkManager->root();
   
    (void)new KBookmark( bookmarkManager, root, _title, _url );
}

void KfmGui::slotEditBookmarks()
{
  QString p = kapp->localkdedir();
  p += "/share/apps/kfm/bookmarks";

  KfmGui *m = new KfmGui( 0L, 0L, p );
  m->show();
}

void KfmGui::slotBookmarkSelected( int _id )
{
    KBookmark *bm = bookmarkManager->findBookmark( _id );
    
    if ( bm )
    {
      if ( bm->type() == KBookmark::Folder )
      {
	(void)new KBookmark( bookmarkManager, bm, title.data(), view->getURL() );
	return;
      }

      KURL u( bm->url() );
      if ( u.isMalformed() )
      {
	tqWarning(QString(klocale->translate("ERROR: Malformed URL"))+" : %s",u.path());
	return;
      }
	
      view->openURL( bm->url() );
    }
    // else
    //  tqWarning("Internal: Could not find bookmark id\n");
    // Commented out, because this happens anytime one hits 'Edit bookmarks'
    // or 'Add bookmarks'. This is normal (menu is connected, and menu item
    // as well). David.
}

void KfmGui::slotToolFind( )
{
  KShellProcess proc;
  proc << "kfind1";
  
  QString strURL( getURL() );
  KURL::decodeURL( strURL );
  KURL url( strURL );

  if( url.isLocalFile() )
    proc << url.directory();

  proc.start(KShellProcess::DontCare);
}

void KfmGui::slotNewWindow( )
{
    KfmGui *f;

    if ( view->getURL() )
	f = new KfmGui( 0L, 0L, view->getURL() );
    else
    {
	QString url;
	url.sprintf( "file:%s", QDir::homeDirPath().data());
	f = new KfmGui( 0L, 0L, url );
    }
    
    f->show();
}

void KfmGui::slotShowMenubar()
{
    showMenubar = !showMenubar;
    moptions->setItemChecked( moptions->idAt( 0 ), showMenubar );
    if (showMenubar)
	menu->show();
    else
	menu->hide();
    updateRects();
    // TODO: does not work yet
    // sven: what doesn't work? Ah, how do I show my menubar back?
}

void KfmGui::slotShowToolbar()
{
    showToolbar = !showToolbar;
    moptions->setItemChecked( moptions->idAt( 2 ), showToolbar );
    if ( !showToolbar )
	toolbarButtons->enable( KToolBar::Hide );
    else
	toolbarButtons->enable( KToolBar::Show );
    resizeEvent( 0L ); //sven: why this?
}

void KfmGui::slotShowLocationBar()
{
    showLocationBar = !showLocationBar;
    moptions->setItemChecked( moptions->idAt( 3 ), showLocationBar );
    if ( !showLocationBar )
	toolbarURL->enable( KToolBar::Hide );
    else
	toolbarURL->enable( KToolBar::Show );
    resizeEvent( 0L ); //sven: why this?
}

void KfmGui::slotShowStatusbar()
{
    showStatusbar = !showStatusbar;
    moptions->setItemChecked( moptions->idAt( 1 ), showStatusbar );
    if ( !showStatusbar )
	statusBar->enable( KStatusBar::Hide );
    else
	statusBar->enable( KStatusBar::Show );
    resizeEvent( 0L ); //sven: why this?
}

void KfmGui::slotShowDot()
{
    showDot = !mview->isItemChecked( mview->idAt( 0 ));
    mview->setItemChecked( mview->idAt( 0 ), showDot );
    showDotLocal = showDot; //force local mode too. (sven)
    view->slotUpdateView();
    if ( bTreeView )
	treeView->update();
}

void KfmGui::slotShowSchnauzer()
{
    visualSchnauzer = !mview->isItemChecked( mview->idAt( 2 ));
    mview->setItemChecked( mview->idAt( 2 ), visualSchnauzer );
    visualSchnauzerLocal = visualSchnauzer; //force local mode too. (sven)
    view->slotUpdateView();
}

void KfmGui::slotShowTreeView()
{
    panner->setSeparator( !bTreeView ? 30 : 0 );
    // all is done by slotPannerChanged when called by setSeparator
}

void KfmGui::slotOpenLocation( )
{
    QString url = "";
    if ( view->getActiveView()->getURL() )
       url = view->getActiveView()->getURL();
    
    DlgLineEntry l( i18n("Open Location:"), url.data(), this, true );
    int x = l.exec();
    if ( x )
    {
        openFilteredURL ( l.getText() );
    }
}

//Called by the Quit menu item, when un-commented (for testing purposes).
void KfmGui::slotQuit()
{
    if ( QMessageBox::warning( 0, klocale->translate("KFM Confirm"), 
			      klocale->translate("Do you really want to quit?\nImportant desktop functionality will be lost"),
			      klocale->translate("Yes"),
			      klocale->translate("No") ) )
	return;
    
    // Session management
    pkfm->slotSave();
    pkfm->slotShutDown();
    
    QString file = kapp->localkdedir();
    file += "/share/apps/kfm/pid";
    file += displayName();
    unlink( file.data() );

    // Clean up IO stuff
    KIOJob::deleteAllJobs();
    delete ( KIOServer::getKIOServer() );

    // Delete root widget instance. David.
    if (KRootWidget::getKRootWidget()) delete KRootWidget::getKRootWidget();

    // saveSettings();

    exit(0);
}

void KfmGui::slotClose()
{
    close();
}

void KfmGui::slotTreeUrlSelected( const char *_url , int _button )
{
    if ( _button == LeftButton )
    {
	slotOpenURL( _url );
	return;
    }
    
    KfmGui *f = new KfmGui( 0L, 0L, _url );
    f->show();
}

void KfmGui::slotTitle( const char *_title )
{
    // This title is *encoded* ! The fix should go to khtml[w], but it's frozen 
    // right now. David.
    title = _title; // keeps a copy of the title. Isn't used at all, AFAIK. David.
    KURL::decodeURL(title);
    setCaption( title.data() );
}

void KfmGui::slotSetStatusBar( const char *_url )
{
    statusBar->changeItem( (char*)_url, 1 );
}

void KfmGui::slotCopy()
{
    // We have to deal with two clipboards : 
    // the system one (QClipboard) and the kfm one (KfmView::clipboard)
    if (focusWidget()->inherits("QLineEdit"))
    {
        KfmView::clipboard->clear();
        const char * txt = ((QLineEdit*)focusWidget())->text();
        KfmView::clipboard->append(txt); // into kfm's clipboard (might be a URL)
        ((QLineEdit*)focusWidget())->copy(); // copy the text
    }
    else
    {
        view->getActiveView()->slotCopy(); // copy the URLs
        QApplication::clipboard()->setText(KfmView::clipboard->first()); //
        // first URL in the system clipboard
    }
}

void KfmGui::slotPaste()
{
    if (focusWidget()->inherits("QLineEdit"))
        ((QLineEdit*)focusWidget())->paste();
    else 
        view->getActiveView()->slotPaste();
}

void KfmGui::slotDelete()
{
    view->getActiveView()->slotDelete();
}

void KfmGui::slotTrash()
{
    view->getActiveView()->slotTrash();
}

const char* KfmGui::getURL() 
{
    return view->getURL();
}

void KfmGui::slotAnimatedLogoTimeout()
{
    animatedLogoCounter++;
    if ( animatedLogoCounter == animatedLogo->count() )
	animatedLogoCounter = 0;
    toolbarButtons->setButtonPixmap( 9, *( animatedLogo->at( animatedLogoCounter ) ) );
}

void KfmGui::slotAddWaitingWidget( KHTMLView *_w )
{
    //tqDebug( "Adding waiting: %p, %d", _w, waitingWidgetList.count() );
    if ( waitingWidgetList.findRef( _w ) != -1 )
	return;
    waitingWidgetList.append( _w );
    if ( !animatedLogoTimer->isActive() )
    {
	animatedLogoTimer->start( 50 );
	toolbarButtons->setItemEnabled( 8, true );
    }
}

void KfmGui::slotRemoveWaitingWidget( KHTMLView *_w )
{
    if ( toolbarButtons == 0L ) // we might be in the destructor
	return;

    waitingWidgetList.removeRef( _w );
    
    if ( waitingWidgetList.count() == 0 )
    {
	animatedLogoTimer->stop();
	toolbarButtons->setButtonPixmap( 9, *( animatedLogo->at( 0 ) ) );
	toolbarButtons->setItemEnabled( 8, false );
	slotSetStatusBar( klocale->translate("Document: Done") );
    }

    //tqDebug( "Removed waiting: %p, %d", _w, waitingWidgetList.count() );
}

void KfmGui::slotTextSelected( KHTMLView *v, bool s )
{
    if ( s )
    {
       QString sel;

       v->getSelectedText( sel );
       QClipboard *cb = KApplication::clipboard();
       cb->setText( sel );
    }
}

void KfmGui::slotSaveSettings()
{
  KConfig *config = kapp->getConfig();

  config->setGroup( "Cache" );
  config->writeEntry( "CacheEnabled", HTMLCache::isEnabled() );
  config->writeEntry( "SaveCacheEnabled", HTMLCache::isSaveEnabled() );

  config->setGroup( "Settings" );
  saveProperties (config); //this will sync on end.
}


void KfmGui::slotConfigureFileManager()
{
   if (fork() == 0)
   {
       // execute 'kcmkfm' with file manager options pages
      execl(kapp->kde_bindir()+"/kcmkfm","kcmkfm","font","color","misc",0);
      tqWarning("Error launching kcmkfm !");
      exit(1); 
   }
}

void KfmGui::slotConfigureBrowser()
{
   if (fork() == 0)
   {
       // execute 'kcmkfm' with browser options pages
      execl(kapp->kde_bindir()+"/kcmkfm","kcmkfm","proxy","http","useragent","cookies",0);
      tqWarning("Error launching kcmkfm !");
      exit(1); 
   }
}

/** Static method, to read configuration and apply it to all kfmguis */
void KfmGui::slotConfigure()
{
    KConfig *config = kapp->getConfig();
    config->reparseConfiguration();
    // -- Group KFM HTML Defaults -- (Colors, Fonts & HTTP tabs)
    KfmGui *w;
    for ( w = windowList->first(); w != 0L; w = windowList->next() )
    {
        if (w->view->getKHTMLWidget())
        {
            w->view->setHTMLWidgetOptions(); // reads and applies config
            w->view->slotUpdateView();
        }

        if(w->treeView){
            config->setGroup( "KFM HTML Defaults" );	
            QColor bgColor = config->readColorEntry( "BgColor", &HTML_DEFAULT_BG_COLOR );
            QColor linkColor = config->readColorEntry( "LinkColor", &HTML_DEFAULT_LNK_COLOR );
            
            w->treeView->setColors(bgColor,linkColor);
            w->treeView->repaint();
        }
    }
    
    // -- Group KFM Root Icons -- (Desktop Icons tab)
    if ( KRootWidget::getKRootWidget() ){
        KRootWidget::getKRootWidget()->configure(config); // read and apply configuration
    }

    // -- Group KFM Misc Defaults -- (Misc tab)
    config->setGroup("KFM Misc Defaults");
    bool urlprops = config->readBoolEntry( "EnablePerURLProps", false );
    pkfm->setURLProps( urlprops );
    bool followmode = config->readBoolEntry( "TreeFollowsView", false );
    pkfm->setTreeViewFollowMode( followmode );

    for ( w = windowList->first(); w != 0L; w = windowList->next() ) {
        w->moptions->setItemEnabled(w->moptions->idAt( 6 ), urlprops);
    }

    // -- Group Browser Settings/UserAgent -- (UserAgent tab)
    // Nothing to do. Next jobs launched will read the setting and use it.

    // -- Group Browser Settings/Proxy -- (Proxy tab)
    // Nothing to do. Next jobs launched will read the setting and use it.

    // -- Group Browser Settings/HTTP -- (Cookies)
    config->setGroup( "Browser Settings/HTTP" );
    bool cookiesEnabled = config->readBoolEntry( "Cookies", true );

    if (cookiejar)
    {
        QString cookieFile = kapp->localkdedir().data();
        cookieFile += "/share/apps/kfm/cookies";
        cookiejar->saveCookies( cookieFile.data() );
        delete cookiejar;
        cookiejar = 0;
    }

    if (cookiesEnabled)
    {
        cookiejar = new KCookieJar();
        cookiejar->loadConfig(config);

	QString cookieFile = kapp->localkdedir().data();
        cookieFile += "/share/apps/kfm/cookies";
        cookiejar->loadCookies( cookieFile.data() );
    }
}

void KfmGui::slotViewFrameSource()
{
    if ( !view->getActiveView() )
	return;
    QString url = view->getActiveView()->getJobURL();
    if (url.isEmpty())
        url = view->getActiveView()->getURL();
    showSource ( url.data() );
}

void KfmGui::slotViewDocumentSource()
{
    QString url = view->getJobURL();
    if (url.isEmpty())
        url = view->getURL();
    showSource ( url.data() );
}

void KfmGui::showSource ( const char * url )
{
    KURL u(url);
    u.setReference(""); // remove reference part.
    QString tmp = view->getHTMLCache()->isCached( u.url() );
    // if cache is enabled and it contains the requested URL
    // use it.  ( Dawit A. )
    if ( view->getHTMLCache()->isEnabled() && tmp != 0L )
        u = QString("file:") + tmp;

    KConfig *config = KApplication::getKApplication()->getConfig();
    config->setGroup( "KFM Misc Defaults" );
    QString term = config->readEntry( "Editor", DEFAULT_EDITOR );

    QString cmd;
    cmd << term << " \"";
    if (u.isLocalFile())
      // remove protocol part, just in case the application doesn't support URLs
      cmd << u.path();
    else
      cmd << u.url();
    cmd << "\"";
    KMimeBind::runCmd( cmd );
}

void KfmGui::slotSetCharset( int id )
{
    uint i;
    printf("chose charset %d,%s\n",id,mcharset->text(id));

    if( id == 0)
	view->setOverrideCharset();
    else
	view->setOverrideCharset(mcharset->text(id));
    // uncheck all items but the current one
    for(i = 0; i< mcharset->count(); i++)
    {
	if((int)i == id)
	    mcharset->setItemChecked(i, true);
	else
	    mcharset->setItemChecked(i, false);
    }
    updateView();
}

void KfmGui::setCharset(const char *_c){

   view->setCharset(_c);
}


void KfmGui::slotUp( )
{
    view->getActiveView()->slotUp();
}

void KfmGui::slotBack( )
{
    view->getActiveView()->slotBack();
}

void KfmGui::slotForward( )
{
    view->getActiveView()->slotForward();
}

void KfmGui::slotGoHistory( int id )
{  
    if (id>=0) {
      QStrList * hlist = Kfm::history();
      // The items are in reverse order, from count()-1 to count()-11
      // so we have to do count()-1-id
      int listId = hlist->count()-1-id;
      if (listId>=0) {
        view->openURL(hlist->at(listId));
      }
    }
}

//-------------------------------------------------------------------------
// session management readProperties - cannot be used for other stuff (sven)
// Because they apply settings, so they cannot be called from constructor.
// Rearranging constructor would be too heavy.

void KfmGui::readProperties(int number)
{
  KTopLevelWidget::readPropertiesInternal(kapp->getConfig(), number);

  //adjusting internal state of the *bars to match the actual state
  if( ( menu->isVisible() && !showMenubar )
      || ( !menu->isVisible() && showMenubar ) )
    slotShowMenubar();

  if( ( toolbarButtons->isVisible() && !showToolbar )
      || ( !toolbarButtons->isVisible() && showToolbar ) )
    slotShowToolbar();

  if( ( toolbarURL->isVisible() && !showLocationBar )
      || ( !toolbarURL->isVisible() && showLocationBar ) )
    slotShowLocationBar();

  if( ( statusBar->isVisible() && !showStatusbar )
      || ( !statusBar->isVisible() && showStatusbar ) )
    slotShowStatusbar();
}


// ReadPropertiesInternal calls this function
void KfmGui::readProperties( KConfig* config )
{

  // GUI_ready == true -> constructor reads global defaults
  // GUI_ready == false -> view reads URL proerties
  
  
    QString entry;

    entry = config->readEntry("ViewMode", "unset");
    if ( entry == "IconView")
      viewModeLocal = ICON_VIEW;
    else if (entry == "LongView")
      viewModeLocal = LONG_VIEW;
    else if (entry == "TextView")
      viewModeLocal = TEXT_VIEW;
    else if (entry == "ShortView")
      viewModeLocal = SHORT_VIEW;
      viewModeLocal = viewMode;
    
    entry = config->readEntry("ShowDotFiles","unset");
    if (entry == "yes")
      showDotLocal = true;
    else if (entry == "no")
      showDotLocal = false;
      showDotLocal = showDot;
    
    entry = config->readEntry("VisualSchnauzer", "unset");
    if (entry == "Off")
      visualSchnauzerLocal = false;
    else if (entry == "On")
      visualSchnauzerLocal = true;
      visualSchnauzerLocal = visualSchnauzer;
    
    entry = config->readEntry("HTMLView", "unset");
    if (entry == "yes")
      bViewHTMLLocal = true;
    else  if (entry == "no")
      bViewHTMLLocal = false;
      bViewHTMLLocal = bViewHTML;
    
    // Now set the changes!!

    if (GUI_ready) //if these are local changes...
    {
        hasLocal = true;
      
        mview->setItemChecked( mview->idAt( 3 ), bViewHTMLLocal);
        mview->setItemChecked( mview->idAt( 2 ), visualSchnauzerLocal );
        mview->setItemChecked( mview->idAt( 0 ), showDotLocal );
        if  (  bViewHTML != bViewHTMLLocal
            || visualSchnauzer != visualSchnauzerLocal
            || showDot != showDotLocal )
            view->slotReload();
        
        mview->setItemChecked( mview->idAt( 5 ), false);
        mview->setItemChecked( mview->idAt( 6 ), false);
        mview->setItemChecked( mview->idAt( 7 ), false);
        mview->setItemChecked( mview->idAt( 8 ), false);

        switch (viewModeLocal)
        {
          case ICON_VIEW:
            mview->setItemChecked( mview->idAt( 5 ), true );
            break;

          case LONG_VIEW:
            mview->setItemChecked( mview->idAt( 7 ), true );
            break;

          case SHORT_VIEW:
            mview->setItemChecked( mview->idAt( 8 ), true );
            break;

          case TEXT_VIEW:
            mview->setItemChecked( mview->idAt( 6 ), true );
            break;
	}
    }
    else // else if constructor reads globals
    {
      bViewHTML = bViewHTMLLocal;
      visualSchnauzer = visualSchnauzerLocal;
      showDot = showDotLocal;
      viewMode = viewModeLocal;
    }
        
    // The rest should be read only if we open new window

    if (!isVisible())
    {

      kfmgui_width = config->readNumEntry("kfmgui_width",  kfmgui_width);
      kfmgui_height = config->readNumEntry("kfmgui_height",kfmgui_height);

      entry = config->readEntry("TreeView", "unset");
      if (entry == "Off")
        bTreeView = false;
      else if (entry == "On")
        bTreeView = true;
    
      entry = config->readEntry("Toolbar", "unset");
      showToolbar = true;
      if ( entry == "top" )
        toolbarPos = KToolBar::Top;
      else if ( entry == "left" )
        toolbarPos = KToolBar::Left;
      else if ( entry == "right" )
        toolbarPos = KToolBar::Right;
      else if ( entry == "bottom" )
        toolbarPos = KToolBar::Bottom;
      else if ( entry == "floating" )
        toolbarPos = KToolBar::Floating;
      else if ( entry == "flat" )
        toolbarPos = KToolBar::Flat;
      else if (entry == "hide"){
        showToolbar = false;
        toolbarPos = KToolBar::Top;
      }

      entry = config->readEntry("LocationBar", "unset");
      showLocationBar = true;
      if ( entry == "top" )
        locationBarPos = KToolBar::Top;
      else if ( entry == "bottom" )
        locationBarPos = KToolBar::Bottom;
      else if ( entry == "floating" )
        locationBarPos = KToolBar::Floating;
      else if ( entry == "flat" )
        locationBarPos = KToolBar::Flat;
      else if (entry == "hide"){
        showLocationBar = false;
        locationBarPos = KToolBar::Top;
      }

      entry = config->readEntry("Menubar", "unset");
      showMenubar = true;
      if ( entry == "top" )
        menubarPos = KMenuBar::Top;
      else if ( entry == "bottom" )
        menubarPos = KMenuBar::Bottom;
      else if ( entry == "floating" )
        menubarPos = KMenuBar::Floating;
      else if ( entry == "flat" )
        menubarPos = KMenuBar::Flat;
      else if (entry == "hide") {
        showMenubar = false;
        menubarPos = KMenuBar::Top;
      }

      entry = config->readEntry("Statusbar", "unset");
      showStatusbar = true;
      if ( entry == "show" )
        showStatusbar = true;
      else if (entry == "hide")
        showStatusbar = false;

      if (GUI_ready)
      {
        bTreeView =!bTreeView;
        slotShowTreeView();

        showMenubar = !showMenubar;
        menu->setMenuBarPos(menubarPos);
        slotShowMenubar ();

        showToolbar = !showToolbar;
        toolbarButtons->setBarPos(toolbarPos);
        slotShowToolbar();

        showLocationBar = !showLocationBar;
        toolbarURL->setBarPos(locationBarPos);
        slotShowLocationBar ();

        showStatusbar = !showStatusbar;
        slotShowStatusbar();
      }
    }
}
//--------------------------------------------------------------------------
// Theese are SM saveProperties used for normal and local properties as well

void KfmGui::saveProperties(int number)
{
  KTopLevelWidget::savePropertiesInternal(kapp->getConfig(), number);
} 


void KfmGui::saveProperties( KConfig* config )
{
  QString entry;

  config->writeEntry("kfmgui_width",this->width());
  config->writeEntry("kfmgui_height",this->height());

  
  if ( bTreeView == false)
    entry = "Off";
  else
    entry = "On";
  
  config->writeEntry("TreeView", entry);
  
  switch (getViewMode())
    {
    case ICON_VIEW:
      entry = "IconView";
      break;
    case LONG_VIEW:
      entry = "LongView";
      break;
    case TEXT_VIEW:
      entry = "TextView";
      break;
    case SHORT_VIEW:
      entry = "ShortView";
      break;
    }

  config->writeEntry("ViewMode", entry);

  if (isShowDot() == true)
    entry = "yes";
  else
    entry = "no";
  
  config->writeEntry("ShowDotFiles", entry);

  if (isVisualSchnauzer() == false)
    entry = "Off";
  else
    entry = "On";
  
  config->writeEntry("VisualSchnauzer", entry);

  if (isViewHTML() == true)
    entry = "yes";
  else
    entry = "no";
  
  config->writeEntry("HTMLView", entry);

  if ( !showToolbar )
      config->writeEntry( "Toolbar", "hide" );
  else if ( toolbarButtons->barPos() == KToolBar::Top )
      config->writeEntry( "Toolbar", "top" );
  else if ( toolbarButtons->barPos() == KToolBar::Bottom )
      config->writeEntry( "Toolbar", "bottom" );
  else if ( toolbarButtons->barPos() == KToolBar::Left )
      config->writeEntry( "Toolbar", "left" );
  else if ( toolbarButtons->barPos() == KToolBar::Right )
      config->writeEntry( "Toolbar", "right" );
  else if ( toolbarButtons->barPos() == KToolBar::Floating )
      config->writeEntry( "Toolbar", "floating" );
  else if ( toolbarButtons->barPos() == KToolBar::Flat )
      config->writeEntry( "Toolbar", "flat" );

  if ( !showLocationBar )
      config->writeEntry( "LocationBar", "hide" );
  else if ( toolbarURL->barPos() == KToolBar::Top )
      config->writeEntry( "LocationBar", "top" );
  else if ( toolbarURL->barPos() == KToolBar::Bottom )
      config->writeEntry( "LocationBar", "bottom" );
  else if ( toolbarURL->barPos() == KToolBar::Floating )
      config->writeEntry( "LocationBar", "floating" );
  else if ( toolbarURL->barPos() == KToolBar::Flat )
      config->writeEntry( "LocationBar", "flat" );

  if ( !showStatusbar )
      config->writeEntry( "Statusbar", "hide" );
  else
      config->writeEntry( "Statusbar", "show" );

  if ( !showMenubar )
      config->writeEntry( "Menubar", "hide" );
  else if ( menu->menuBarPos() == KMenuBar::Top )
      config->writeEntry( "Menubar", "top" );
  else if ( menu->menuBarPos() == KMenuBar::Bottom )
      config->writeEntry( "Menubar", "bottom" );
  else if ( menu->menuBarPos() == KMenuBar::Floating )
      config->writeEntry( "Menubar", "floating" );
  else if ( menu->menuBarPos() == KMenuBar::Flat )
      config->writeEntry( "Menubar", "flat" );
  config->sync();
}

//--------------------------------------------------------------------------
void KfmGui::writeProperties(KConfig *cfg)
{
  //Sven - mostly because saveProperties is protected :-(
  // And I have to set Group!
  cfg->setGroup("URL properties");
  saveProperties(cfg); // will sync on end
}

void KfmGui::loadProperties(KConfig *cfg)
{
  cfg->setGroup("URL properties");
  if (cfg->hasKey("ViewMode"))     //do we really have our props there?
    readProperties(cfg);           // Yeah, read them.
  else
    setHasLocal (false);           // No, we don't
}

void KfmGui::setHasLocal (bool aintGotNoProps)
{
  hasLocal = aintGotNoProps;

  // This works only for dirs without .directory, and for urls
  // that are neither bookmarked nor have kdelnk to them on desktop

  if (!hasLocal) // reset checkboxes to global state
  {
    if (bViewHTML != bViewHTMLLocal)
      mview->setItemChecked( mview->idAt( 3 ), bViewHTML);

    if (visualSchnauzer != visualSchnauzerLocal)
      mview->setItemChecked( mview->idAt( 2 ), visualSchnauzer);

    if (showDot != showDotLocal)
      mview->setItemChecked( mview->idAt( 0 ), showDot);

    if (viewMode != viewModeLocal)
    {
      mview->setItemChecked( mview->idAt( 5 ), false);
      mview->setItemChecked( mview->idAt( 6 ), false);
      mview->setItemChecked( mview->idAt( 7 ), false);
      mview->setItemChecked( mview->idAt( 8 ), false);

      switch (viewMode)
      {
	case ICON_VIEW:
	  mview->setItemChecked( mview->idAt( 5 ), true );
	  break;

	case LONG_VIEW:
	  mview->setItemChecked( mview->idAt( 7 ), true );
	  break;

	case SHORT_VIEW:
	  mview->setItemChecked( mview->idAt( 8 ), true );
	  break;

	case TEXT_VIEW:
	  mview->setItemChecked( mview->idAt( 6 ), true );
	  break;
      }
    }
  }
}

KfmGui::~KfmGui()
{
    // [KDE1 Revival 2026] 双内核：退出前结束新内核子进程（复用关闭逻辑）
    if ( webEngineOn ) {
        if ( webEngineIn >= 0 ) { close( webEngineIn ); webEngineIn = -1; }
        /* [2026-08-31] 有界等待替代无限阻塞：EOF 后 webview 自行退出，此处
           至多等 2 秒（100x20ms 循环），超时 SIGKILL 兜底——原 waitpid(0)
           无限期阻塞会在 webview 卡死时连 kfm 退出一起冻住 */
        if ( webEnginePid > 0 ) {
            for ( int i = 0; i < 100; i++ ) {
                int st;
                if ( waitpid( webEnginePid, &st, WNOHANG ) == webEnginePid )
                    goto reaped;
                usleep( 20000 );
            }
            kill( webEnginePid, SIGKILL );
            int st;
            while ( waitpid( webEnginePid, &st, 0 ) < 0 && errno == EINTR )
                ;
        }
      reaped:
        webEnginePid = -1;
        if ( webEngineSocket ) {
            XDestroyWindow( qt_xdisplay(), (Window)webEngineSocket );
            webEngineSocket = 0;
        }
        webEngineOn = false;
    }
    if ( animatedLogoTimer )
    {
	animatedLogoTimer->stop();
	delete animatedLogoTimer;
    }
    
    if ( toolbarButtons )
    {
	delete toolbarButtons;
	toolbarButtons = 0L;
    }
    
    if ( toolbarURL )
	delete toolbarURL;
    if (menu)
       delete menu;

    if ( findDialog )
        delete findDialog;
 
    delete view;
    delete completion;
    windowList->remove( this );
    delete menuNew;

    // Last window and in window-only-mode ?
    if ( windowList->count() == 0 && !rooticons )
    {
	// remove pid file
	QString file = kapp->localkdedir();
	file += "/share/apps/kfm/pid";
	file += displayName();
	unlink( file.data() );
	// quit
	exit(0);
    }
}

#include "kfmgui.moc"



