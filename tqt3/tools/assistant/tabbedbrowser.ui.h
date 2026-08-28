/****************************************************************************
** ui.h extension file, included from the uic-generated form implementation.
**
** If you wish to add, delete or rename functions or slots use
** TQt Designer which will update this file, preserving your code. Create an
** init() function in place of a constructor, and a destroy() function in
** place of a destructor.
*****************************************************************************/

#include <ntqfileinfo.h>
#include <ntqtoolbutton.h>
#include <ntqpixmap.h>
#include <ntqiconset.h>
#include <ntqstyle.h>
#include <ntqtimer.h>

#include "config.h"

static TQString reduceLabelLength( const TQString &s )
{
    uint maxLength = 16;
    TQString str = s;
    if ( str.length() < maxLength )
	return str;
    str = str.left( maxLength - 3 );
    str += "...";
    return str;
}

void TabbedBrowser::forward()
{
    currentBrowser()->forward();
}

void TabbedBrowser::backward()
{
    currentBrowser()->backward();
}

void TabbedBrowser::setSource( const TQString &ref )
{
    HelpWindow * win = currentBrowser();
    win->setSource(ref);
}

void TabbedBrowser::reload()
{
    currentBrowser()->reload();
}

void TabbedBrowser::home()
{
    currentBrowser()->home();
}

HelpWindow * TabbedBrowser::currentBrowser()
{
    return (HelpWindow*) tab->currentPage();
}

void TabbedBrowser::nextTab()
{
    if( tab->currentPageIndex()<=tab->count()-1 )
	tab->setCurrentPage( tab->currentPageIndex()+1 );
}

void TabbedBrowser::previousTab()
{
    int idx = tab->currentPageIndex()-1;
    if( idx>=0 )
	tab->setCurrentPage( idx );
}

HelpWindow *TabbedBrowser::createHelpWindow(const TQString &title)
{
    MainWindow *mainWin = mainWindow();
    HelpWindow *win = new HelpWindow( mainWin, this, "qt_assistant_helpwin" );
    win->setFont( browserFont() );
    win->setPalette( palette() );
    win->setLinkUnderline( tabLinkUnderline );
    win->setStyleSheet( tabStyleSheet );
    win->setMimeSourceFactory( mimeSourceFactory );
    tab->addTab(win, reduceLabelLength(title));
    connect( win, TQ_SIGNAL( highlighted( const TQString & ) ),
	     (const TQObject*) (mainWin->statusBar()), TQ_SLOT( message( const TQString & ) ) );
    connect( win, TQ_SIGNAL( chooseWebBrowser() ), mainWin, TQ_SLOT( showWebBrowserSettings() ) );
    connect( win, TQ_SIGNAL( backwardAvailable(bool) ),
	     mainWin, TQ_SLOT( backwardAvailable(bool) ) );
    connect( win, TQ_SIGNAL( forwardAvailable(bool) ),
	     mainWin, TQ_SLOT( forwardAvailable(bool) ) );
    connect( win, TQ_SIGNAL( sourceChanged(const TQString &) ), this, TQ_SLOT( sourceChanged() ));

    tab->cornerWidget( TQt::TopRight )->setEnabled( tab->count() > 1 );
    return win;
}

HelpWindow *TabbedBrowser::newBackgroundTab( const TQString &url )
{
    HelpWindow *win = createHelpWindow(url);
    return win;
}

void TabbedBrowser::newTab( const TQString &lnk )
{
    TQString link( lnk );
    if( link.isNull() ) {
	HelpWindow *w = currentBrowser();
	if( w )
	    link = w->source();
    }
    HelpWindow *win = createHelpWindow(link);
    tab->showPage( win );
    if( !link.isNull() ) {
 	win->setSource( link );
    }
}

void TabbedBrowser::zoomIn()
{
    currentBrowser()->zoomIn();
}

void TabbedBrowser::zoomOut()
{
    currentBrowser()->zoomOut();
}

void TabbedBrowser::init()
{
    tabLinkUnderline = false;
    tabStyleSheet = new TQStyleSheet( TQStyleSheet::defaultSheet() );
    lastCurrentTab = 0;
    while( tab->count() )
	tab->removePage( tab->page(0) );

    mimeSourceFactory = new TQMimeSourceFactory();
    mimeSourceFactory->setExtensionType("html","text/html;charset=UTF-8");
    mimeSourceFactory->setExtensionType("htm","text/html;charset=UTF-8");
    mimeSourceFactory->setExtensionType("png", "image/png" );
    mimeSourceFactory->setExtensionType("jpg", "image/jpeg" );
    mimeSourceFactory->setExtensionType("jpeg", "image/jpeg" );
    setMimePath( Config::configuration()->mimePaths() );

    connect( tab, TQ_SIGNAL( currentChanged( TQWidget* ) ),
	     this, TQ_SLOT( transferFocus() ) );
    connect( tab, TQ_SIGNAL( currentChanged( TQWidget* ) ),
        mainWindow(), TQ_SLOT( browserTabChanged() ) );

    TQTabBar *tabBar = (TQTabBar*)tab->child( 0, "TQTabBar", false );
    int m = ( tabBar ? style().pixelMetric( TQStyle::PM_TabBarTabVSpace, (TQWidget*)tabBar )
	      + style().pixelMetric( TQStyle::PM_TabBarBaseHeight, (TQWidget*)tabBar ) : 0 );
    int s = tab->height() - m;

    // workaround for sgi style
    TQPalette pal = palette();
    pal.setColor(TQPalette::Active, TQColorGroup::Button, pal.active().background());
    pal.setColor(TQPalette::Disabled, TQColorGroup::Button, pal.disabled().background());
    pal.setColor(TQPalette::Inactive, TQColorGroup::Button, pal.inactive().background());

    TQToolButton *newTabButton = new TQToolButton( this );
    newTabButton->setPalette(pal);
    tab->setCornerWidget( newTabButton, TQt::TopLeft );
    newTabButton->setCursor( arrowCursor );
    newTabButton->setAutoRaise( true );
    newTabButton->setPixmap( TQPixmap::fromMimeSource( "addtab.png" ) );
    newTabButton->setFixedSize( s, s );
    TQObject::connect( newTabButton, TQ_SIGNAL( clicked() ), this, TQ_SLOT( newTab() ) );
    TQToolTip::add( newTabButton, tr( "Add page" ) );

    TQToolButton *closeTabButton = new TQToolButton( this );
    closeTabButton->setPalette(pal);
    tab->setCornerWidget( closeTabButton, TQt::TopRight );
    closeTabButton->setCursor( arrowCursor );
    closeTabButton->setAutoRaise( true );
    TQIconSet is( TQPixmap::fromMimeSource( "closetab.png") );
    TQPixmap disabledPix = TQPixmap::fromMimeSource( "d_closetab.png" );
    is.setPixmap( disabledPix, TQIconSet::Small, TQIconSet::Disabled );
    closeTabButton->setIconSet( is );
    closeTabButton->setFixedSize( s, s );
    TQObject::connect( closeTabButton, TQ_SIGNAL( clicked() ), this, TQ_SLOT( closeTab() ) );
    TQToolTip::add( closeTabButton, tr( "Close page" ) );
    closeTabButton->setEnabled( false );
}

void TabbedBrowser::setMimePath( TQStringList lst )
{
    mimeSourceFactory->setFilePath( lst );
}

void TabbedBrowser::setMimeExtension( const TQString &ext )
{
    mimeSourceFactory->setExtensionType( "html", ext );
    mimeSourceFactory->setExtensionType( "htm", ext );
}

void TabbedBrowser::updateTitle( const TQString &title )
{
    tab->changeTab( currentBrowser(), title );
}

MainWindow* TabbedBrowser::mainWindow()
{
    return (MainWindow*) parent();
}

void TabbedBrowser::newTab()
{
    newTab( TQString::null );
}

void TabbedBrowser::transferFocus()
{
    if( currentBrowser() ) {
	currentBrowser()->setFocus();
    }
    mainWindow()->setCaption(Config::configuration()->title()
			     + " - " + currentBrowser()->documentTitle());
}

void TabbedBrowser::initHelpWindow( HelpWindow * /*win*/ )
{
}

void TabbedBrowser::setup()
{
    Config *config = Config::configuration();

    TQFont fnt( font() );
    TQFontInfo fntInfo( fnt );
    fnt.setFamily( config->fontFamily() );
    fnt.setPointSize( config->fontSize() );
    setBrowserFont( fnt );

    TQPalette pal = palette();
    TQColor lc( config->linkColor() );
    pal.setColor( TQPalette::Active, TQColorGroup::Link, lc );
    pal.setColor( TQPalette::Inactive, TQColorGroup::Link, lc );
    pal.setColor( TQPalette::Disabled, TQColorGroup::Link, lc );
    setPalette( pal );

    tabLinkUnderline = config->isLinkUnderline();

    TQString family = config->fontFixedFamily();
    tabStyleSheet->item( "pre" )->setFontFamily( family );
    tabStyleSheet->item( "code" )->setFontFamily( family );
    tabStyleSheet->item( "tt" )->setFontFamily( family );

    newTab( TQString::null );
}

void TabbedBrowser::setLinkUnderline( bool uline )
{
    if( uline==tabLinkUnderline )
	return;
    tabLinkUnderline = uline;
    int cnt = tab->count();
    for( int i=0; i<cnt; i++ )
	( (TQTextBrowser*) tab->page( i ) )->setLinkUnderline( tabLinkUnderline );
}

TQFont TabbedBrowser::browserFont()
{
    return tabFont;
}

void TabbedBrowser::setBrowserFont( const TQFont &fnt )
{
    if( tabFont == fnt )
	return;
    tabFont = fnt;
    int cnt = tab->count();
    for( int i=0; i<cnt; i++ )
	( (TQTextBrowser*) tab->page( i ) )->setFont( fnt );
}

void TabbedBrowser::setPalette( const TQPalette &pal )
{
    if( palette()==pal )
	return;
    TQWidget::setPalette( pal );
    int cnt = tab->count();
    for( int i=0; i<cnt; i++ )
	( (TQTextBrowser*) tab->page( i ) )->setPalette( pal );
}

TQStyleSheet* TabbedBrowser::styleSheet()
{
    return tabStyleSheet;
}

bool TabbedBrowser::linkUnderline()
{
    return tabLinkUnderline;
}

void TabbedBrowser::copy()
{
    currentBrowser()->copy();
}

void TabbedBrowser::closeTab()
{
    if( tab->count()==1 )
	return;
    HelpWindow *win = currentBrowser();
    tab->removePage( win );
    TQTimer::singleShot(0, win, TQ_SLOT(deleteLater()));
    tab->cornerWidget( TQt::TopRight )->setEnabled( tab->count() > 1 );
}

TQStringList TabbedBrowser::sources()
{
    TQStringList lst;
    int cnt = tab->count();
    for( int i=0; i<cnt; i++ ) {
	lst.append( ( (TQTextBrowser*) tab->page(i) )->source() );
    }
    return lst;
}

TQPtrList<HelpWindow> TabbedBrowser::browsers() const
{
    TQPtrList<HelpWindow> list;
    for (int i=0; i<tab->count(); ++i) {
	Q_ASSERT(::tqt_cast<HelpWindow*>(tab->page(i)));
	list.append(::tqt_cast<HelpWindow*>(tab->page(i)));
    }
    return list;
}

void TabbedBrowser::sourceChanged()
{
    HelpWindow *win = ::tqt_cast<HelpWindow *>( TQObject::sender() );
    Q_ASSERT( win );
    TQString docTitle(win->documentTitle());
    if (docTitle.isEmpty())
	docTitle = "...";
    setTitle(win, docTitle);
}

void TabbedBrowser::setTitle(HelpWindow *win, const TQString &title)
{
    tab->setTabLabel(win, reduceLabelLength(title));
    if (win == currentBrowser())
	mainWindow()->setCaption(Config::configuration()->title() + " - " + title);
}

