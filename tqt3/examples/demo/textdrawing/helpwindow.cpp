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
      pathCombo( 0 ), selectedURL()
{
    readHistory();
    readBookmarks();

    browser = new TQTextBrowser( this );
    browser->mimeSourceFactory()->setFilePath( _path );
    browser->setFrameStyle( TQFrame::Panel | TQFrame::Sunken );
    connect( browser, TQ_SIGNAL( textChanged() ),
	     this, TQ_SLOT( textChanged() ) );

    setCentralWidget( browser );

    if ( !home_.isEmpty() )
	browser->setSource( home_ );

    connect( browser, TQ_SIGNAL( highlighted( const TQString&) ),
	     statusBar(), TQ_SLOT( message( const TQString&)) );

    resize( 640,700 );

    TQPopupMenu* file = new TQPopupMenu( this );
    file->insertItem( tr("&New Window"), this, TQ_SLOT( newWindow() ), ALT | Key_N );
    file->insertItem( tr("&Open File"), this, TQ_SLOT( openFile() ), ALT | Key_O );
    file->insertItem( tr("&Print"), this, TQ_SLOT( print() ), ALT | Key_P );

    // The same three icons are used twice each.
    TQIconSet icon_back( TQPixmap("textdrawing/previous.png") );
    TQIconSet icon_forward( TQPixmap("textdrawing/next.png") );
    TQIconSet icon_home( TQPixmap("textdrawing/home.png") );

    TQPopupMenu* go = new TQPopupMenu( this );
    backwardId = go->insertItem( icon_back,
				 tr("&Backward"), browser, TQ_SLOT( backward() ),
				 ALT | Key_Left );
    forwardId = go->insertItem( icon_forward,
				tr("&Forward"), browser, TQ_SLOT( forward() ),
				ALT | Key_Right );
    go->insertItem( icon_home, tr("&Home"), browser, TQ_SLOT( home() ) );

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


void HelpWindow::textChanged()
{
    if ( browser->documentTitle().isNull() )
	setCaption( browser->context() );
    else
	setCaption( browser->documentTitle() ) ;

    selectedURL = caption();
    if ( !selectedURL.isEmpty() && pathCombo ) {
	bool exists = false;
	int i;
	for ( i = 0; i < pathCombo->count(); ++i ) {
	    if ( pathCombo->text( i ) == selectedURL ) {
		exists = true;
		break;
	    }
	}
	if ( !exists ) {
	    pathCombo->insertItem( selectedURL, 0 );
	    pathCombo->setCurrentItem( 0 );
	    mHistory[ hist->insertItem( selectedURL ) ] = selectedURL;
	} else
	    pathCombo->setCurrentItem( i );
	selectedURL = TQString::null;
    }
}

HelpWindow::~HelpWindow()
{
    history.clear();
    TQMap<int, TQString>::Iterator it = mHistory.begin();
    for ( ; it != mHistory.end(); ++it )
	history.append( *it );

    TQFile f( TQDir::currentDirPath() + "/.history" );
    f.open( IO_WriteOnly );
    TQDataStream s( &f );
    s << history;
    f.close();

    bookmarks.clear();
    TQMap<int, TQString>::Iterator it2 = mBookmarks.begin();
    for ( ; it2 != mBookmarks.end(); ++it2 )
	bookmarks.append( *it2 );

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
			"<p>It's just about 100 lines of C++ code, so don't expect too much :-)</p>"
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
    TQPrinter printer;
    printer.setFullPage(true);
    if ( printer.setup() ) {
	TQPainter p( &printer );
	TQPaintDeviceMetrics metrics(p.device());
	int dpix = metrics.logicalDpiX();
	int dpiy = metrics.logicalDpiY();
	const int margin = 72; // pt
	TQRect body(margin*dpix/72, margin*dpiy/72,
		   metrics.width()-margin*dpix/72*2,
		   metrics.height()-margin*dpiy/72*2 );
	TQFont font("times", 10);
	TQStringList filePaths = browser->mimeSourceFactory()->filePath();
	TQString file;
	TQStringList::Iterator it = filePaths.begin();
	for ( ; it != filePaths.end(); ++it ) {
	    file = TQUrl( *it, TQUrl( browser->source() ).path() ).path();
	    if ( TQFile::exists( file ) )
		break;
	    else
		file = TQString::null;
	}
	if ( file.isEmpty() )
	    return;
	TQFile f( file );
	if ( !f.open( IO_ReadOnly ) )
	    return;
	TQTextStream ts( &f );
	TQSimpleRichText richText( ts.read(), font, browser->context(), browser->styleSheet(),
				  browser->mimeSourceFactory(), body.height() );
	richText.setWidth( &p, body.width() );
	TQRect view( body );
	int page = 1;
	do {
	    richText.draw( &p, body.left(), body.top(), view, colorGroup() );
	    view.moveBy( 0, body.height() );
	    p.translate( 0 , -body.height() );
	    p.setFont( font );
	    p.drawText( view.right() - p.fontMetrics().width( TQString::number(page) ),
			view.bottom() + p.fontMetrics().ascent() + 5, TQString::number(page) );
	    if ( view.top()  >= richText.height() )
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
    TQMap<int, TQString>::Iterator it = mHistory.begin();
    bool exists = false;
    for ( ; it != mHistory.end(); ++it ) {
	if ( *it == _path ) {
	    exists = true;
	    break;
	}
    }
    if ( !exists )
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
    mBookmarks[ bookm->insertItem( caption() ) ] = caption();
}
