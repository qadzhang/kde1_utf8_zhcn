/****************************************************************************
**
** Copyright (C) 1992-2008 Trolltech ASA.  All rights reserved.
**
** This file is part of an example program for TQt.  This example
** program may be used, distributed and modified without limitation.
**
*****************************************************************************/

#include "helpwindow.h"
#include <ntqstatusbar.h>
#include <ntqpixmap.h>
#include <ntqpopupmenu.h>
#include <ntqmenubar.h>
#include <ntqtoolbar.h>
#include <ntqtoolbutton.h>
#include <ntqiconset.h>
#include <ntqfile.h>
#include <ntqtextstream.h>
#include <ntqstylesheet.h>
#include <ntqmessagebox.h>
#include <ntqfiledialog.h>
#include <ntqapplication.h>
#include <ntqcombobox.h>
#include <ntqevent.h>
#include <ntqlineedit.h>
#include <ntqobjectlist.h>
#include <ntqfileinfo.h>
#include <ntqfile.h>
#include <ntqdatastream.h>
#include <ntqprinter.h>
#include <ntqsimplerichtext.h>
#include <ntqpainter.h>
#include <ntqpaintdevicemetrics.h>

#include <ctype.h>

HelpWindow::HelpWindow( const TQString& home_, const TQString& _path,
			TQWidget* parent, const char *name )
    : TQMainWindow( parent, name, WDestructiveClose ),
      pathCombo( 0 )
{
    readHistory();
    readBookmarks();

    browser = new TQTextBrowser( this );

    browser->mimeSourceFactory()->setFilePath( _path );
    browser->setFrameStyle( TQFrame::Panel | TQFrame::Sunken );
    connect( browser, TQ_SIGNAL( sourceChanged(const TQString& ) ),
	     this, TQ_SLOT( sourceChanged( const TQString&) ) );

    setCentralWidget( browser );

    if ( !home_.isEmpty() )
	browser->setSource( home_ );

    connect( browser, TQ_SIGNAL( highlighted( const TQString&) ),
	     statusBar(), TQ_SLOT( message( const TQString&)) );

    resize( 640,700 );

    TQPopupMenu* file = new TQPopupMenu( this );
    file->insertItem( tr("&New Window"), this, TQ_SLOT( newWindow() ), CTRL+Key_N );
    file->insertItem( tr("&Open File"), this, TQ_SLOT( openFile() ), CTRL+Key_O );
    file->insertItem( tr("&Print"), this, TQ_SLOT( print() ), CTRL+Key_P );
    file->insertSeparator();
    file->insertItem( tr("&Close"), this, TQ_SLOT( close() ), CTRL+Key_Q );
    file->insertItem( tr("E&xit"), tqApp, TQ_SLOT( closeAllWindows() ), CTRL+Key_X );

    // The same three icons are used twice each.
    TQIconSet icon_back( TQPixmap("back.xpm") );
    TQIconSet icon_forward( TQPixmap("forward.xpm") );
    TQIconSet icon_home( TQPixmap("home.xpm") );

    TQPopupMenu* go = new TQPopupMenu( this );
    backwardId = go->insertItem( icon_back,
				 tr("&Backward"), browser, TQ_SLOT( backward() ),
				 CTRL+Key_Left );
    forwardId = go->insertItem( icon_forward,
				tr("&Forward"), browser, TQ_SLOT( forward() ),
				CTRL+Key_Right );
    go->insertItem( icon_home, tr("&Home"), browser, TQ_SLOT( home() ) );

    TQPopupMenu* help = new TQPopupMenu( this );
    help->insertItem( tr("&About"), this, TQ_SLOT( about() ) );
    help->insertItem( tr("About &TQt"), this, TQ_SLOT( aboutTQt() ) );

    hist = new TQPopupMenu( this );
    TQStringList::Iterator it = history.begin();
    for ( ; it != history.end(); ++it )
	mHistory[ hist->insertItem( *it ) ] = *it;
    connect( hist, TQ_SIGNAL( activated( int ) ),
	     this, TQ_SLOT( histChosen( int ) ) );

    bookm = new TQPopupMenu( this );
    bookm->insertItem( tr( "Add Bookmark" ), this, TQ_SLOT( addBookmark() ) );
    bookm->insertSeparator();

    TQStringList::Iterator it2 = bookmarks.begin();
    for ( ; it2 != bookmarks.end(); ++it2 )
	mBookmarks[ bookm->insertItem( *it2 ) ] = *it2;
    connect( bookm, TQ_SIGNAL( activated( int ) ),
	     this, TQ_SLOT( bookmChosen( int ) ) );

    menuBar()->insertItem( tr("&File"), file );
    menuBar()->insertItem( tr("&Go"), go );
    menuBar()->insertItem( tr( "History" ), hist );
    menuBar()->insertItem( tr( "Bookmarks" ), bookm );
    menuBar()->insertSeparator();
    menuBar()->insertItem( tr("&Help"), help );

    menuBar()->setItemEnabled( forwardId, false);
    menuBar()->setItemEnabled( backwardId, false);
    connect( browser, TQ_SIGNAL( backwardAvailable( bool ) ),
	     this, TQ_SLOT( setBackwardAvailable( bool ) ) );
    connect( browser, TQ_SIGNAL( forwardAvailable( bool ) ),
	     this, TQ_SLOT( setForwardAvailable( bool ) ) );


    TQToolBar* toolbar = new TQToolBar( this );
    addToolBar( toolbar, "Toolbar");
    TQToolButton* button;

    button = new TQToolButton( icon_back, tr("Backward"), "", browser, TQ_SLOT(backward()), toolbar );
    connect( browser, TQ_SIGNAL( backwardAvailable(bool) ), button, TQ_SLOT( setEnabled(bool) ) );
    button->setEnabled( false );
    button = new TQToolButton( icon_forward, tr("Forward"), "", browser, TQ_SLOT(forward()), toolbar );
    connect( browser, TQ_SIGNAL( forwardAvailable(bool) ), button, TQ_SLOT( setEnabled(bool) ) );
    button->setEnabled( false );
    button = new TQToolButton( icon_home, tr("Home"), "", browser, TQ_SLOT(home()), toolbar );

    toolbar->addSeparator();

    pathCombo = new TQComboBox( true, toolbar );
    connect( pathCombo, TQ_SIGNAL( activated( const TQString & ) ),
	     this, TQ_SLOT( pathSelected( const TQString & ) ) );
    toolbar->setStretchableWidget( pathCombo );
    setRightJustification( true );
    setDockEnabled( DockLeft, false );
    setDockEnabled( DockRight, false );

    pathCombo->insertItem( home_ );
    browser->setFocus();

}


void HelpWindow::setBackwardAvailable( bool b)
{
    menuBar()->setItemEnabled( backwardId, b);
}

void HelpWindow::setForwardAvailable( bool b)
{
    menuBar()->setItemEnabled( forwardId, b);
}


void HelpWindow::sourceChanged( const TQString& url )
{
    if ( browser->documentTitle().isNull() )
	setCaption( "TQt Example - Helpviewer - " + url );
    else
	setCaption( "TQt Example - Helpviewer - " + browser->documentTitle() ) ;

    if ( !url.isEmpty() && pathCombo ) {
	bool exists = false;
	int i;
	for ( i = 0; i < pathCombo->count(); ++i ) {
	    if ( pathCombo->text( i ) == url ) {
		exists = true;
		break;
	    }
	}
	if ( !exists ) {
	    pathCombo->insertItem( url, 0 );
	    pathCombo->setCurrentItem( 0 );
	    mHistory[ hist->insertItem( url ) ] = url;
	} else
	    pathCombo->setCurrentItem( i );
    }
}

HelpWindow::~HelpWindow()
{
    history =  mHistory.values();

    TQFile f( TQDir::currentDirPath() + "/.history" );
    f.open( IO_WriteOnly );
    TQDataStream s( &f );
    s << history;
    f.close();

    bookmarks = mBookmarks.values();

    TQFile f2( TQDir::currentDirPath() + "/.bookmarks" );
    f2.open( IO_WriteOnly );
    TQDataStream s2( &f2 );
    s2 << bookmarks;
    f2.close();
}

void HelpWindow::about()
{
    TQMessageBox::about( this, "HelpViewer Example",
			"<p>This example implements a simple HTML help viewer "
			"using TQt's rich text capabilities</p>"
			"<p>It's just about 400 lines of C++ code, so don't expect too much :-)</p>"
			);
}


void HelpWindow::aboutTQt()
{
    TQMessageBox::aboutTQt( this, "TQBrowser" );
}

void HelpWindow::openFile()
{
#ifndef TQT_NO_FILEDIALOG
    TQString fn = TQFileDialog::getOpenFileName( TQString::null, TQString::null, this );
    if ( !fn.isEmpty() )
	browser->setSource( fn );
#endif
}

void HelpWindow::newWindow()
{
    ( new HelpWindow(browser->source(), "qbrowser") )->show();
}

void HelpWindow::print()
{
#ifndef TQT_NO_PRINTER
    TQPrinter printer( TQPrinter::HighResolution );
    printer.setFullPage(true);
    if ( printer.setup( this ) ) {
	TQPainter p( &printer );
	if( !p.isActive() ) // starting printing failed
	    return;
	TQPaintDeviceMetrics metrics(p.device());
	int dpiy = metrics.logicalDpiY();
	int margin = (int) ( (2/2.54)*dpiy ); // 2 cm margins
	TQRect view( margin, margin, metrics.width() - 2*margin, metrics.height() - 2*margin );
	TQSimpleRichText richText( browser->text(),
				  TQFont(),
				  browser->context(),
				  browser->styleSheet(),
				  browser->mimeSourceFactory(),
				  view.height() );
	richText.setWidth( &p, view.width() );
	int page = 1;
	do {
	    richText.draw( &p, margin, margin, view, colorGroup() );
	    view.moveBy( 0, view.height() );
	    p.translate( 0 , -view.height() );
	    p.drawText( view.right() - p.fontMetrics().width( TQString::number(page) ),
			view.bottom() + p.fontMetrics().ascent() + 5, TQString::number(page) );
	    if ( view.top() - margin >= richText.height() )
		break;
	    printer.newPage();
	    page++;
	} while (true);
    }
#endif
}

void HelpWindow::pathSelected( const TQString &_path )
{
    browser->setSource( _path );
    if ( mHistory.values().contains(_path) )
	mHistory[ hist->insertItem( _path ) ] = _path;
}

void HelpWindow::readHistory()
{
    if ( TQFile::exists( TQDir::currentDirPath() + "/.history" ) ) {
	TQFile f( TQDir::currentDirPath() + "/.history" );
	f.open( IO_ReadOnly );
	TQDataStream s( &f );
	s >> history;
	f.close();
	while ( history.count() > 20 )
	    history.remove( history.begin() );
    }
}

void HelpWindow::readBookmarks()
{
    if ( TQFile::exists( TQDir::currentDirPath() + "/.bookmarks" ) ) {
	TQFile f( TQDir::currentDirPath() + "/.bookmarks" );
	f.open( IO_ReadOnly );
	TQDataStream s( &f );
	s >> bookmarks;
	f.close();
    }
}

void HelpWindow::histChosen( int i )
{
    if ( mHistory.contains( i ) )
	browser->setSource( mHistory[ i ] );
}

void HelpWindow::bookmChosen( int i )
{
    if ( mBookmarks.contains( i ) )
	browser->setSource( mBookmarks[ i ] );
}

void HelpWindow::addBookmark()
{
    mBookmarks[ bookm->insertItem( caption() ) ] = browser->context();
}
