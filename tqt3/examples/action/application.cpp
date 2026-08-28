/****************************************************************************
**
** Copyright (C) 1992-2008 Trolltech ASA.  All rights reserved.
**
** This file is part of an example program for TQt.  This example
** program may be used, distributed and modified without limitation.
**
*****************************************************************************/

#include "application.h"

#include <ntqimage.h>
#include <ntqpixmap.h>
#include <ntqtoolbar.h>
#include <ntqtoolbutton.h>
#include <ntqpopupmenu.h>
#include <ntqmenubar.h>
#include <ntqtextedit.h>
#include <ntqfile.h>
#include <ntqfiledialog.h>
#include <ntqstatusbar.h>
#include <ntqmessagebox.h>
#include <ntqprinter.h>
#include <ntqapplication.h>
#include <ntqaccel.h>
#include <ntqtextstream.h>
#include <ntqpainter.h>
#include <ntqpaintdevicemetrics.h>
#include <ntqwhatsthis.h>
#include <ntqaction.h>
#include <ntqsimplerichtext.h>

#include "filesave.xpm"
#include "fileopen.xpm"
#include "fileprint.xpm"


ApplicationWindow::ApplicationWindow()
    : TQMainWindow( 0, "example application main window", WDestructiveClose )
{
    printer = new TQPrinter( TQPrinter::HighResolution );

    TQAction * fileNewAction;
    TQAction * fileOpenAction;
    TQAction * fileSaveAction, * fileSaveAsAction, * filePrintAction;
    TQAction * fileCloseAction, * fileQuitAction;

    fileNewAction = new TQAction( "&New", CTRL+Key_N, this, "new" );
    connect( fileNewAction, TQ_SIGNAL( activated() ) , this,
             TQ_SLOT( newDoc() ) );

    fileOpenAction = new TQAction( TQPixmap( fileopen ), "&Open...",
                                  CTRL+Key_O, this, "open" );
    connect( fileOpenAction, TQ_SIGNAL( activated() ) , this, TQ_SLOT( choose() ) );

    const char * fileOpenText = "<p><img source=\"fileopen\"> "
                     "Click this button to open a <em>new file</em>. <br>"
                     "You can also select the <b>Open</b> command "
                     "from the <b>File</b> menu.</p>";
    TQMimeSourceFactory::defaultFactory()->setPixmap( "document-open",
                          fileOpenAction->iconSet().pixmap() );
    fileOpenAction->setWhatsThis( fileOpenText );

    fileSaveAction = new TQAction( TQPixmap( filesave ),
                                  "&Save", CTRL+Key_S, this, "save" );
    connect( fileSaveAction, TQ_SIGNAL( activated() ) , this, TQ_SLOT( save() ) );

    const char * fileSaveText = "<p>Click this button to save the file you "
                     "are editing. You will be prompted for a file name.\n"
                     "You can also select the <b>Save</b> command "
                     "from the <b>File</b> menu.</p>";
    fileSaveAction->setWhatsThis( fileSaveText );

    fileSaveAsAction = new TQAction( "Save File As", "Save &As...", 0,  this,
                                    "save as" );
    connect( fileSaveAsAction, TQ_SIGNAL( activated() ) , this,
             TQ_SLOT( saveAs() ) );
    fileSaveAsAction->setWhatsThis( fileSaveText );

    filePrintAction = new TQAction( "Print File", TQPixmap( fileprint ),
                                   "&Print...", CTRL+Key_P, this, "print" );
    connect( filePrintAction, TQ_SIGNAL( activated() ) , this,
             TQ_SLOT( print() ) );

    const char * filePrintText = "Click this button to print the file you "
                     "are editing.\n You can also select the Print "
                     "command from the File menu.";
    filePrintAction->setWhatsThis( filePrintText );

    fileCloseAction = new TQAction( "Close", "&Close", CTRL+Key_W, this,
                                   "close" );
    connect( fileCloseAction, TQ_SIGNAL( activated() ) , this,
             TQ_SLOT( close() ) );

    fileQuitAction = new TQAction( "Quit", "&Quit", CTRL+Key_Q, this,
                                  "quit" );
    connect( fileQuitAction, TQ_SIGNAL( activated() ) , tqApp,
             TQ_SLOT( closeAllWindows() ) );

    // populate a tool bar with some actions

    TQToolBar * fileTools = new TQToolBar( this, "file operations" );
    fileTools->setLabel( "File Operations" );
    fileOpenAction->addTo( fileTools );
    fileSaveAction->addTo( fileTools );
    filePrintAction->addTo( fileTools );
    (void)TQWhatsThis::whatsThisButton( fileTools );


    // populate a menu with all actions

    TQPopupMenu * file = new TQPopupMenu( this );
    menuBar()->insertItem( "&File", file );
    fileNewAction->addTo( file );
    fileOpenAction->addTo( file );
    fileSaveAction->addTo( file );
    fileSaveAsAction->addTo( file );
    file->insertSeparator();
    filePrintAction->addTo( file );
    file->insertSeparator();
    fileCloseAction->addTo( file );
    fileQuitAction->addTo( file );


    menuBar()->insertSeparator();

    // add a help menu

    TQPopupMenu * help = new TQPopupMenu( this );
    menuBar()->insertItem( "&Help", help );
    help->insertItem( "&About", this, TQ_SLOT(about()), Key_F1 );
    help->insertItem( "About &TQt", this, TQ_SLOT(aboutTQt()) );
    help->insertSeparator();
    help->insertItem( "What's &This", this, TQ_SLOT(whatsThis()),
                      SHIFT+Key_F1 );


    // create and define the central widget

    e = new TQTextEdit( this, "editor" );
    e->setFocus();
    setCentralWidget( e );
    statusBar()->message( "Ready", 2000 );

    resize( 450, 600 );
}


ApplicationWindow::~ApplicationWindow()
{
    delete printer;
}



void ApplicationWindow::newDoc()
{
    ApplicationWindow *ed = new ApplicationWindow;
    ed->show();
}

void ApplicationWindow::choose()
{
    TQString fn = TQFileDialog::getOpenFileName( TQString::null, TQString::null,
					       this);
    if ( !fn.isEmpty() )
	load( fn );
    else
	statusBar()->message( "Loading aborted", 2000 );
}


void ApplicationWindow::load( const TQString &fileName )
{
    TQFile f( fileName );
    if ( !f.open( IO_ReadOnly ) )
	return;

    TQTextStream ts( &f );
    e->setText( ts.read() );
    e->setModified( false );
    setCaption( fileName );
    statusBar()->message( "Loaded document " + fileName, 2000 );
}


void ApplicationWindow::save()
{
    if ( filename.isEmpty() ) {
	saveAs();
	return;
    }

    TQString text = e->text();
    TQFile f( filename );
    if ( !f.open( IO_WriteOnly ) ) {
	statusBar()->message( TQString("Could not write to %1").arg(filename),
			      2000 );
	return;
    }

    TQTextStream t( &f );
    t << text;
    f.close();

    e->setModified( false );

    setCaption( filename );

    statusBar()->message( TQString( "File %1 saved" ).arg( filename ), 2000 );
}


void ApplicationWindow::saveAs()
{
    TQString fn = TQFileDialog::getSaveFileName( TQString::null, TQString::null,
					       this );
    if ( !fn.isEmpty() ) {
	filename = fn;
	save();
    } else {
	statusBar()->message( "Saving aborted", 2000 );
    }
}


void ApplicationWindow::print()
{
    printer->setFullPage( true );
    if ( printer->setup(this) ) {		// printer dialog
	statusBar()->message( "Printing..." );
	TQPainter p;
	if( !p.begin( printer ) ) {               // paint on printer
	    statusBar()->message( "Printing aborted", 2000 );
	    return;
	}

	TQPaintDeviceMetrics metrics( p.device() );
	int dpiy = metrics.logicalDpiY();
	int margin = (int) ( (2/2.54)*dpiy ); // 2 cm margins
	TQRect view( margin, margin, metrics.width() - 2*margin, metrics.height() - 2*margin );
	TQSimpleRichText richText( TQStyleSheet::convertFromPlainText(e->text()),
				  TQFont(),
				  e->context(),
				  e->styleSheet(),
				  e->mimeSourceFactory(),
				  view.height() );
	richText.setWidth( &p, view.width() );
	int page = 1;
	do {
	    richText.draw( &p, margin, margin, view, colorGroup() );
	    view.moveBy( 0, view.height() );
	    p.translate( 0 , -view.height() );
	    p.drawText( view.right() - p.fontMetrics().width( TQString::number( page ) ),
			view.bottom() + p.fontMetrics().ascent() + 5, TQString::number( page ) );
	    if ( view.top() - margin >= richText.height() )
		break;
	    printer->newPage();
	    page++;
	} while (true);

	statusBar()->message( "Printing completed", 2000 );
    } else {
	statusBar()->message( "Printing aborted", 2000 );
    }
}

void ApplicationWindow::closeEvent( TQCloseEvent* ce )
{
    if ( !e->isModified() ) {
	ce->accept();
	return;
    }

    switch( TQMessageBox::information( this, "TQt Application Example",
				      "The document has been changed since "
				      "the last save.",
				      "Save Now", "Cancel", "Leave Anyway",
				      0, 1 ) ) {
    case 0:
	save();
	ce->accept();
	break;
    case 1:
    default: // just for sanity
	ce->ignore();
	break;
    case 2:
	ce->accept();
	break;
    }
}


void ApplicationWindow::about()
{
    TQMessageBox::about( this, "TQt Application Example",
			"This example demonstrates simple use of "
			"TQMainWindow,\nTQMenuBar and TQToolBar.");
}


void ApplicationWindow::aboutTQt()
{
    TQMessageBox::aboutTQt( this, "TQt Application Example" );
}
