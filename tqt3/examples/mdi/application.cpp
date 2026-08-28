/****************************************************************************
**
** Copyright (C) 1992-2008 Trolltech ASA.  All rights reserved.
**
** This file is part of an example program for TQt.  This example
** program may be used, distributed and modified without limitation.
**
*****************************************************************************/

#include "application.h"
#include <ntqworkspace.h>
#include <ntqimage.h>
#include <ntqpixmap.h>
#include <ntqtoolbar.h>
#include <ntqtoolbutton.h>
#include <ntqpopupmenu.h>
#include <ntqmenubar.h>
#include <ntqmovie.h>
#include <ntqfile.h>
#include <ntqfiledialog.h>
#include <ntqlabel.h>
#include <ntqstatusbar.h>
#include <ntqmessagebox.h>
#include <ntqprinter.h>
#include <ntqapplication.h>
#include <ntqpushbutton.h>
#include <ntqaccel.h>
#include <ntqtextstream.h>
#include <ntqtextedit.h>
#include <ntqpainter.h>
#include <ntqpaintdevicemetrics.h>
#include <ntqwhatsthis.h>
#include <ntqobjectlist.h>
#include <ntqvbox.h>
#include <ntqsimplerichtext.h>

#include "filesave.xpm"
#include "fileopen.xpm"
#include "fileprint.xpm"


const char * fileOpenText = "Click this button to open a <em>new file</em>. <br><br>"
"You can also select the <b>Open command</b> from the File menu.";
const char * fileSaveText = "Click this button to save the file you are "
"editing.  You will be prompted for a file name.\n\n"
"You can also select the Save command from the File menu.\n\n"
"Note that implementing this function is left as an exercise for the reader.";
const char * filePrintText = "Click this button to print the file you "
"are editing.\n\n"
"You can also select the Print command from the File menu.";

ApplicationWindow::ApplicationWindow()
    : TQMainWindow( 0, "example application main window", WDestructiveClose )
{
    int id;

    TQPixmap openIcon, saveIcon;

    fileTools = new TQToolBar( this, "file operations" );
    addToolBar( fileTools, tr( "File Operations" ), DockTop, true );

    openIcon = TQPixmap( fileopen );
    TQToolButton * fileOpen
	= new TQToolButton( openIcon, "Open File", TQString::null,
			   this, TQ_SLOT(load()), fileTools, "open file" );

    saveIcon = TQPixmap( filesave );
    TQToolButton * fileSave
	= new TQToolButton( saveIcon, "Save File", TQString::null,
			   this, TQ_SLOT(save()), fileTools, "save file" );

#ifndef TQT_NO_PRINTER
    printer = new TQPrinter( TQPrinter::HighResolution );
    TQPixmap printIcon;

    printIcon = TQPixmap( fileprint );
    TQToolButton * filePrint
	= new TQToolButton( printIcon, "Print File", TQString::null,
			   this, TQ_SLOT(print()), fileTools, "print file" );
    TQWhatsThis::add( filePrint, filePrintText );
#endif

    (void)TQWhatsThis::whatsThisButton( fileTools );

    TQWhatsThis::add( fileOpen, fileOpenText );
    TQWhatsThis::add( fileSave, fileSaveText );

    TQPopupMenu * file = new TQPopupMenu( this );
    menuBar()->insertItem( "&File", file );

    file->insertItem( "&New", this, TQ_SLOT(newDoc()), CTRL+Key_N );

    id = file->insertItem( openIcon, "&Open...",
			   this, TQ_SLOT(load()), CTRL+Key_O );
    file->setWhatsThis( id, fileOpenText );

    id = file->insertItem( saveIcon, "&Save",
			   this, TQ_SLOT(save()), CTRL+Key_S );
    file->setWhatsThis( id, fileSaveText );
    id = file->insertItem( "Save &As...", this, TQ_SLOT(saveAs()) );
    file->setWhatsThis( id, fileSaveText );
#ifndef TQT_NO_PRINTER
    file->insertSeparator();
    id = file->insertItem( printIcon, "&Print...",
			   this, TQ_SLOT(print()), CTRL+Key_P );
    file->setWhatsThis( id, filePrintText );
#endif
    file->insertSeparator();
    file->insertItem( "&Close", this, TQ_SLOT(closeWindow()), CTRL+Key_W );
    file->insertItem( "&Quit", tqApp, TQ_SLOT( closeAllWindows() ), CTRL+Key_Q );

    windowsMenu = new TQPopupMenu( this );
    windowsMenu->setCheckable( true );
    connect( windowsMenu, TQ_SIGNAL( aboutToShow() ),
	     this, TQ_SLOT( windowsMenuAboutToShow() ) );
    menuBar()->insertItem( "&Windows", windowsMenu );

    menuBar()->insertSeparator();
    TQPopupMenu * help = new TQPopupMenu( this );
    menuBar()->insertItem( "&Help", help );

    help->insertItem( "&About", this, TQ_SLOT(about()), Key_F1);
    help->insertItem( "About &TQt", this, TQ_SLOT(aboutTQt()));
    help->insertSeparator();
    help->insertItem( "What's &This", this, TQ_SLOT(whatsThis()), SHIFT+Key_F1);

    TQVBox* vb = new TQVBox( this );
    vb->setFrameStyle( TQFrame::StyledPanel | TQFrame::Sunken );
    ws = new TQWorkspace( vb );
    ws->setScrollBarsEnabled( true );
    setCentralWidget( vb );

    statusBar()->message( "Ready", 2000 );
}


ApplicationWindow::~ApplicationWindow()
{
#ifndef TQT_NO_PRINTER
    delete printer;
#endif
}



MDIWindow* ApplicationWindow::newDoc()
{
    MDIWindow* w = new MDIWindow( ws, 0, WDestructiveClose );
    connect( w, TQ_SIGNAL( message(const TQString&, int) ), statusBar(), TQ_SLOT( message(const TQString&, int )) );
    w->setCaption("unnamed document");
    w->setIcon( TQPixmap("document.xpm") );
    // show the very first window in maximized mode
    if ( ws->windowList().isEmpty() )
	w->showMaximized();
    else
	w->show();
    return w;
}

void ApplicationWindow::load()
{
    TQString fn = TQFileDialog::getOpenFileName( TQString::null, TQString::null, this );
    if ( !fn.isEmpty() ) {
	MDIWindow* w = newDoc();
	w->load( fn );
    }  else {
	statusBar()->message( "Loading aborted", 2000 );
    }
}

void ApplicationWindow::save()
{
    MDIWindow* m = (MDIWindow*)ws->activeWindow();
    if ( m )
	m->save();
}


void ApplicationWindow::saveAs()
{
    MDIWindow* m = (MDIWindow*)ws->activeWindow();
    if ( m )
	m->saveAs();
}


void ApplicationWindow::print()
{
#ifndef TQT_NO_PRINTER
    MDIWindow* m = (MDIWindow*)ws->activeWindow();
    if ( m )
	m->print( printer );
#endif
}


void ApplicationWindow::closeWindow()
{
    MDIWindow* m = (MDIWindow*)ws->activeWindow();
    if ( m )
	m->close();
}

void ApplicationWindow::about()
{
    TQMessageBox::about( this, "TQt Application Example",
			"This example demonstrates simple use of\n "
			"TQt's Multiple Document Interface (MDI).");
}


void ApplicationWindow::aboutTQt()
{
    TQMessageBox::aboutTQt( this, "TQt Application Example" );
}


void ApplicationWindow::windowsMenuAboutToShow()
{
    windowsMenu->clear();
    int cascadeId = windowsMenu->insertItem("&Cascade", ws, TQ_SLOT(cascade() ) );
    int tileId = windowsMenu->insertItem("&Tile", ws, TQ_SLOT(tile() ) );
    int horTileId = windowsMenu->insertItem("Tile &Horizontally", this, TQ_SLOT(tileHorizontal() ) );
    if ( ws->windowList().isEmpty() ) {
	windowsMenu->setItemEnabled( cascadeId, false );
	windowsMenu->setItemEnabled( tileId, false );
	windowsMenu->setItemEnabled( horTileId, false );
    }
    windowsMenu->insertSeparator();
    TQWidgetList windows = ws->windowList();
    for ( int i = 0; i < int(windows.count()); ++i ) {
	int id = windowsMenu->insertItem(windows.at(i)->caption(),
					 this, TQ_SLOT( windowsMenuActivated( int ) ) );
	windowsMenu->setItemParameter( id, i );
	windowsMenu->setItemChecked( id, ws->activeWindow() == windows.at(i) );
    }
}

void ApplicationWindow::windowsMenuActivated( int id )
{
    TQWidget* w = ws->windowList().at( id );
    if ( w )
	w->showNormal();
    w->setFocus();
}

void ApplicationWindow::tileHorizontal()
{
    // primitive horizontal tiling
    TQWidgetList windows = ws->windowList();
    if ( !windows.count() )
	return;

    if (ws->activeWindow())
        ws->activeWindow()->showNormal();

    int heightForEach = ws->height() / windows.count();
    int y = 0;
    for ( int i = 0; i < int(windows.count()); ++i ) {
	TQWidget *window = windows.at(i);
	int preferredHeight = window->minimumHeight()+window->parentWidget()->baseSize().height();
	int actHeight = TQMAX(heightForEach, preferredHeight);

	window->parentWidget()->setGeometry( 0, y, ws->width(), actHeight );
	y += actHeight;
    }
}

void ApplicationWindow::closeEvent( TQCloseEvent *e )
{
    TQWidgetList windows = ws->windowList();
    if ( windows.count() ) {
	for ( int i = 0; i < int(windows.count()); ++i ) {
	    TQWidget *window = windows.at( i );
	    if ( !window->close() ) {
		e->ignore();
		return;
	    }
	}
    }

    TQMainWindow::closeEvent( e );
}

MDIWindow::MDIWindow( TQWidget* parent, const char* name, int wflags )
    : TQMainWindow( parent, name, wflags )
{
    mmovie = 0;
    medit = new TQTextEdit( this );
    setFocusProxy( medit );
    setCentralWidget( medit );
}

MDIWindow::~MDIWindow()
{
    delete mmovie;
}

void MDIWindow::closeEvent( TQCloseEvent *e )
{
    if ( medit->isModified() ) {
	switch( TQMessageBox::warning( this, "Save Changes",
	    tr("Save changes to %1?").arg( caption() ),
	    tr("Yes"), tr("No"), tr("Cancel") ) ) {
	case 0:
	    {
		save();
		if ( !filename.isEmpty() )
		    e->accept();
		else
		    e->ignore();
	    }
	    break;
	case 1:
	    e->accept();
	    break;
	default:
	    e->ignore();
	    break;
	}
    } else {
	e->accept();
    }
}

void MDIWindow::load( const TQString& fn )
{
    filename  = fn;
    TQFile f( filename );
    if ( !f.open( IO_ReadOnly ) )
	return;

    if(fn.contains(".gif")) {
	TQWidget * tmp=new TQWidget(this);
	setFocusProxy(tmp);
	setCentralWidget(tmp);
	medit->hide();
	delete medit;
	TQMovie * qm=new TQMovie(fn);
#ifdef TQ_WS_QWS // temporary speed-test hack
	qm->setDisplayWidget(tmp);
#endif
	tmp->setBackgroundMode(TQWidget::NoBackground);
	tmp->show();
	mmovie=qm;
    } else {
	mmovie = 0;

	TQTextStream t(&f);
	TQString s = t.read();
	medit->setText( s );
	f.close();


    }
    setCaption( filename );
    emit message( TQString("Loaded document %1").arg(filename), 2000 );
}

void MDIWindow::save()
{
    if ( filename.isEmpty() ) {
        saveAs();
        return;
    }

    TQString text = medit->text();
    TQFile f( filename );
    if ( !f.open( IO_WriteOnly ) ) {
        emit message( TQString("Could not write to %1").arg(filename),
		      2000 );
        return;
    }

    TQTextStream t( &f );
    t << text;
    f.close();

    setCaption( filename );

    emit message( TQString( "File %1 saved" ).arg( filename ), 2000 );
}

void MDIWindow::saveAs()
{
    TQString fn = TQFileDialog::getSaveFileName( filename, TQString::null, this );
    if ( !fn.isEmpty() ) {
        filename = fn;
        save();
    } else {
        emit message( "Saving aborted", 2000 );
    }
}

void MDIWindow::print( TQPrinter* printer)
{
#ifndef TQT_NO_PRINTER
    int pageNo = 1;

    if ( printer->setup(this) ) {		// printer dialog
        printer->setFullPage( true );
	emit message( "Printing...", 0 );
	TQPainter p;
	if ( !p.begin( printer ) )
	    return;				// paint on printer
	TQPaintDeviceMetrics metrics( p.device() );
	int dpiy = metrics.logicalDpiY();
	int margin = (int) ( (2/2.54)*dpiy ); // 2 cm margins
	TQRect view( margin, margin, metrics.width() - 2*margin, metrics.height() - 2*margin );
	TQSimpleRichText richText( TQStyleSheet::convertFromPlainText(medit->text()),
				  TQFont(),
				  medit->context(),
				  medit->styleSheet(),
				  medit->mimeSourceFactory(),
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
	    TQString msg( "Printing (page " );
	    msg += TQString::number( ++pageNo );
	    msg += ")...";
	    emit message( msg, 0 );
	    printer->newPage();
	    page++;
	} while (true);
    }
#endif
}
