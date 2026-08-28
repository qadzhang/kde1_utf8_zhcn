/****************************************************************************
**
** Copyright (C) 1992-2008 Trolltech ASA.  All rights reserved.
**
** This file is part of an example program for TQt.  This example
** program may be used, distributed and modified without limitation.
**
*****************************************************************************/

#include "qwerty.h"
#include <ntqapplication.h>
#include <ntqfile.h>
#include <ntqfiledialog.h>
#include <ntqpopupmenu.h>
#include <ntqtextstream.h>
#include <ntqpainter.h>
#include <ntqmessagebox.h>
#include <ntqpaintdevicemetrics.h>
#include <ntqptrlist.h>
#include <ntqfontdialog.h>

#include <ntqtextcodec.h>

const bool no_writing = false;

static TQPtrList<TQTextCodec> *codecList = 0;

enum { Uni = 0, MBug = 1, Lat1 = 2, Local = 3, Guess = 4, Codec = 5 };


Editor::Editor( TQWidget * parent , const char * name )
    : TQWidget( parent, name, WDestructiveClose )
{
    m = new TQMenuBar( this, "menu" );

    TQPopupMenu * file = new TQPopupMenu();
    TQ_CHECK_PTR( file );
    m->insertItem( "&File", file );

    file->insertItem( "&New",   this, TQ_SLOT(newDoc()),   ALT+Key_N );
    file->insertItem( "&Open...",  this, TQ_SLOT(load()),     ALT+Key_O );
    file->insertItem( "&Save...",  this, TQ_SLOT(save()),     ALT+Key_S );
    file->insertSeparator();
    open_as = new TQPopupMenu();
    file->insertItem( "Open &As",  open_as );
    save_as = new TQPopupMenu();
    file->insertItem( "Sa&ve As",  save_as );
    file->insertItem( "Add &Encoding", this, TQ_SLOT(addEncoding()) );    
#ifndef TQT_NO_PRINTER
    file->insertSeparator();
    file->insertItem( "&Print...", this, TQ_SLOT(print()),    ALT+Key_P );
#endif
    file->insertSeparator();
    file->insertItem( "&Close", this, TQ_SLOT(close()),ALT+Key_W );
    file->insertItem( "&Quit",  tqApp, TQ_SLOT(closeAllWindows()),     ALT+Key_Q );

    connect( save_as, TQ_SIGNAL(activated(int)), this, TQ_SLOT(saveAsEncoding(int)) );
    connect( open_as, TQ_SIGNAL(activated(int)), this, TQ_SLOT(openAsEncoding(int)) );
    rebuildCodecList();

    TQPopupMenu * edit = new TQPopupMenu();
    TQ_CHECK_PTR( edit );
    m->insertItem( "&Edit", edit );

    edit->insertItem( "To &Uppercase",   this, TQ_SLOT(toUpper()),   ALT+Key_U );
    edit->insertItem( "To &Lowercase",   this, TQ_SLOT(toLower()),   ALT+Key_L );
#ifndef TQT_NO_FONTDIALOG
    edit->insertSeparator();
    edit->insertItem( "&Select Font" ,	 this, TQ_SLOT(font()),     ALT+Key_T );
#endif
    changed = false;
    e = new TQMultiLineEdit( this, "editor" );
    connect( e, TQ_SIGNAL( textChanged() ), this, TQ_SLOT( textChanged() ) );

    // We use Unifont - if you have it installed you'll see all
    // Unicode character glyphs.
    //
    // Unifont only comes in one pixel size, so we cannot let
    // it change pixel size as the display DPI changes.
    //
    TQFont unifont("unifont",16,50); unifont.setPixelSize(16);
    e->setFont( unifont );

    e->setFocus();
}

Editor::~Editor()
{
}

void Editor::font()
{
#ifndef TQT_NO_FONTDIALOG
    bool ok;
    TQFont f = TQFontDialog::getFont( &ok, e->font() );
    if ( ok ) {
        e->setFont( f );
    }
#endif
}



void Editor::rebuildCodecList()
{
    delete codecList;
    codecList = new TQPtrList<TQTextCodec>;
    TQTextCodec *codec;
    int i;
    for (i = 0; (codec = TQTextCodec::codecForIndex(i)); i++)
	codecList->append( codec );
    int n = codecList->count();
    for (int pm=0; pm<2; pm++) {
	TQPopupMenu* menu = pm ? open_as : save_as;
	menu->clear();
	TQString local = "Local (";
	local += TQTextCodec::codecForLocale()->name();
	local += ")";
	menu->insertItem( local, Local );
	menu->insertItem( "Unicode", Uni );
	menu->insertItem( "Latin1", Lat1 );
	menu->insertItem( "Microsoft Unicode", MBug );
	if ( pm )
	    menu->insertItem( "[guess]", Guess );
	for ( i = 0; i < n; i++ )
	    menu->insertItem( codecList->at(i)->name(), Codec + i );
    }
}

void Editor::newDoc()
{
    Editor *ed = new Editor;
    if ( tqApp->desktop()->size().width() < 450
	 || tqApp->desktop()->size().height() < 450 ) {
	ed->showMaximized();
    } else {
	ed->resize( 400, 400 );
	ed->show();
    }
}


void Editor::load()
{
#ifndef TQT_NO_FILEDIALOG
    TQString fn = TQFileDialog::getOpenFileName( TQString::null, TQString::null, this );
    if ( !fn.isEmpty() )
	load( fn, -1 );
#endif
}

void Editor::load( const TQString& fileName, int code )
{
    TQFile f( fileName );
    if ( !f.open( IO_ReadOnly ) )
	return;

    e->setAutoUpdate( false );

    TQTextStream t(&f);
    if ( code >= Codec )
	t.setCodec( codecList->at(code-Codec) );
    else if ( code == Uni )
	t.setEncoding( TQTextStream::Unicode );
    else if ( code == MBug )
	t.setEncoding( TQTextStream::UnicodeReverse );
    else if ( code == Lat1 )
	t.setEncoding( TQTextStream::Latin1 );
    else if ( code == Guess ) {
	TQFile f(fileName);
	f.open(IO_ReadOnly);
	char buffer[256];
	int l = 256;
	l=f.readBlock(buffer,l);
	TQTextCodec* codec = TQTextCodec::codecForContent(buffer, l);
	if ( codec ) {
	    TQMessageBox::information(this,"Encoding",TQString("Codec: ")+codec->name());
	    t.setCodec( codec );
	}
    }
    e->setText( t.read() );
    f.close();

    e->setAutoUpdate( true );
    e->repaint();
    setCaption( fileName );

    changed = false;
}

void Editor::openAsEncoding( int code )
{
#ifndef TQT_NO_FILEDIALOG
    //storing filename (proper save) is left as an exercise...
    TQString fn = TQFileDialog::getOpenFileName( TQString::null, TQString::null, this );
    if ( !fn.isEmpty() )
	(void) load( fn, code );
#endif
}

bool Editor::save()
{
#ifndef TQT_NO_FILEDIALOG
    //storing filename (proper save) is left as an exercise...
    TQString fn = TQFileDialog::getSaveFileName( TQString::null, TQString::null, this );
    if ( !fn.isEmpty() )
	return saveAs( fn );
    return false;
#endif
}

void Editor::saveAsEncoding( int code )
{
#ifndef TQT_NO_FILEDIALOG
    //storing filename (proper save) is left as an exercise...
    TQString fn = TQFileDialog::getSaveFileName( TQString::null, TQString::null, this );
    if ( !fn.isEmpty() )
	(void) saveAs( fn, code );
#endif
}

void Editor::addEncoding()
{
#ifndef TQT_NO_FILEDIALOG
    TQString fn = TQFileDialog::getOpenFileName( TQString::null, "*.map", this );
    if ( !fn.isEmpty() ) {
	TQFile f(fn);
	if (f.open(IO_ReadOnly)) {
	    if (TQTextCodec::loadCharmap(&f)) {
		rebuildCodecList();
	    } else {
		TQMessageBox::warning(0,"Charmap error",
		    "The file did not contain a valid charmap.\n\n"
		    "A charmap file should look like this:\n"
		       "  <code_set_name> thename\n"
		       "  <escape_char> /\n"
		       "  % alias thealias\n"
		       "  CHARMAP\n"
		       "  <tokenname> /x12 <U3456>\n"
		       "  <tokenname> /xAB/x12 <U0023>\n"
		       "  ...\n"
		       "  END CHARMAP\n"
		);
	    }
	}
    }
#endif
}


bool Editor::saveAs( const TQString& fileName, int code )
{
    TQFile f( fileName );
    if ( no_writing || !f.open( IO_WriteOnly ) ) {
	TQMessageBox::warning(this,"I/O Error",
		    TQString("The file could not be opened.\n\n")
			+fileName);
	return false;
    }
    TQTextStream t(&f);
    if ( code >= Codec )
	t.setCodec( codecList->at(code-Codec) );
    else if ( code == Uni )
	t.setEncoding( TQTextStream::Unicode );
    else if ( code == MBug )
	t.setEncoding( TQTextStream::UnicodeReverse );
    else if ( code == Lat1 )
	t.setEncoding( TQTextStream::Latin1 );
    t << e->text();
    f.close();
    setCaption( fileName );
    changed = false;
    return true;
}

void Editor::print()
{
#ifndef TQT_NO_PRINTER
    if ( printer.setup(this) ) {		// opens printer dialog
	printer.setFullPage(true);		// we'll set our own margins
	TQPainter p;
	p.begin( &printer );			// paint on printer
	p.setFont( e->font() );
	TQFontMetrics fm = p.fontMetrics();
	TQPaintDeviceMetrics metrics( &printer ); // need width/height
						 // of printer surface
	const int MARGIN = metrics.logicalDpiX() / 2; // half-inch margin
	int yPos        = MARGIN;		// y position for each line

	for( int i = 0 ; i < e->numLines() ; i++ ) {
	    if ( printer.aborted() )
		break;
	    if ( yPos + fm.lineSpacing() > metrics.height() - MARGIN ) {
		// no more room on this page
		if ( !printer.newPage() )          // start new page
		    break;                           // some error
		yPos = MARGIN;			 // back to top of page
	    }
	    p.drawText( MARGIN, yPos, metrics.width() - 2*MARGIN,
			fm.lineSpacing(), ExpandTabs, e->textLine( i ) );
	    yPos += fm.lineSpacing();
	}
	p.end();				// send job to printer
    }
#endif
}

void Editor::resizeEvent( TQResizeEvent * )
{
    if ( e && m )
	e->setGeometry( 0, m->height(), width(), height() - m->height() );
}

void Editor::closeEvent( TQCloseEvent *event )
{
    event->accept();

    if ( changed ) { // the text has been changed
	switch ( TQMessageBox::warning( this, "Qwerty",
					"Save changes to Document?",
					tr("&Yes"),
					tr("&No"),
					tr("Cancel"),
					0, 2) ) {
	case 0: // yes
	    if ( save() )
		event->accept();
	    else
		event->ignore();
	    break;
	case 1: // no
	    event->accept();
	    break;
	default: // cancel
	    event->ignore();
	    break;
	}
    }
}

void Editor::toUpper()
{
    e->setText(e->text().upper());
}

void Editor::toLower()
{
    e->setText(e->text().lower());
}

void Editor::textChanged()
{
    changed = true;
}
