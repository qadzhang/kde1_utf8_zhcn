/****************************************************************************
**
** Copyright (C) 1992-2008 Trolltech ASA.  All rights reserved.
**
** This file is part of an example program for TQt.  This example
** program may be used, distributed and modified without limitation.
**
*****************************************************************************/

#include "../dirview/dirview.h"
#include "ntqdir.h"

#include <ntqapplication.h>
#include <ntqtextview.h>
#include <ntqfileinfo.h>
#include <ntqfile.h>
#include <ntqtextstream.h>
#include <ntqhbox.h>
#include <ntqspinbox.h>
#include <ntqlabel.h>
#include <ntqmultilineedit.h>
#include <ntqheader.h>
#include <ntqevent.h>
#include <ntqpainter.h>
#include <ntqpopupmenu.h>
#include <ntqpushbutton.h>
#include <ntqtoolbutton.h>
#include <ntqfile.h>
#include <ntqtextstream.h>
#include <ntqtooltip.h>

#include <stdlib.h>

/* XPM */
static const char *bookmarks[]={
    "22 14 8 1",
    "# c #000080",
    "a c #585858",
    "b c #000000",
    "c c #ffffff",
    "d c #ffffff",
    "e c #ffffff",
    "f c #000000",
    ". c None",
    "...bb.................",
    "..bacb....bbb.........",
    "..badcb.bbccbab.......",
    "..bacccbadccbab.......",
    "..baecdbcccdbab.......",
    "..bacccbacccbab.......",
    "..badcdbcecdfab.......",
    "..bacecbacccbab.......",
    "..baccdbcccdbab.......",
    "...badcbacdbbab.......",
    "....bacbcbbccab.......",
    ".....babbaaaaab.......",
    ".....bbabbbbbbb.......",
    "......bb.............."
};

/* XPM */
static const char *home[]={
    "16 15 4 1",
    "# c #000000",
    "a c #ffffff",
    "b c #c0c0c0",
    ". c None",
    ".......##.......",
    "..#...####......",
    "..#..#aabb#.....",
    "..#.#aaaabb#....",
    "..##aaaaaabb#...",
    "..#aaaaaaaabb#..",
    ".#aaaaaaaaabbb#.",
    "###aaaaaaaabb###",
    "..#aaaaaaaabb#..",
    "..#aaa###aabb#..",
    "..#aaa#.#aabb#..",
    "..#aaa#.#aabb#..",
    "..#aaa#.#aabb#..",
    "..#aaa#.#aabb#..",
    "..#####.######.."
};

// ****************************************************************************************************

PixmapView::PixmapView( TQWidget *parent )
    : TQScrollView( parent )
{
    viewport()->setBackgroundMode( PaletteBase );
}

void PixmapView::setPixmap( const TQPixmap &pix )
{
    pixmap = pix;
    resizeContents( pixmap.size().width(), pixmap.size().height() );
    viewport()->repaint( false );
}

void PixmapView::drawContents( TQPainter *p, int cx, int cy, int cw, int ch )
{
    p->fillRect( cx, cy, cw, ch, colorGroup().brush( TQColorGroup::Base ) );
    p->drawPixmap( 0, 0, pixmap );
}

// ****************************************************************************************************

Preview::Preview( TQWidget *parent )
    : TQWidgetStack( parent )
{
    normalText = new TQMultiLineEdit( this );
    normalText->setReadOnly( true );
    html = new TQTextView( this );
    pixmap = new PixmapView( this );
    raiseWidget( normalText );
}

void Preview::showPreview( const TQUrl &u, int size )
{
    if ( u.isLocalFile() ) {
	TQString path = u.path();
	TQFileInfo fi( path );
	if ( fi.isFile() && (int)fi.size() > size * 1000 ) {
	    normalText->setText( tr( "The File\n%1\nis too large, so I don't show it!" ).arg( path ) );
	    raiseWidget( normalText );
	    return;
	}
	
	TQPixmap pix( path );
	if ( pix.isNull() ) {
	    if ( fi.isFile() ) {
		TQFile f( path );
		if ( f.open( IO_ReadOnly ) ) {
		    TQTextStream ts( &f );
		    TQString text = ts.read();
		    f.close();
		    if ( fi.extension().lower().contains( "htm" ) ) {
			TQString url = html->mimeSourceFactory()->makeAbsolute( path, html->context() );
			html->setText( text, url ); 	
			raiseWidget( html );
			return;
		    } else {
			normalText->setText( text ); 	
			raiseWidget( normalText );
			return;
		    }
		}
	    }
	    normalText->setText( TQString::null );
	    raiseWidget( normalText );
	} else {
	    pixmap->setPixmap( pix );
	    raiseWidget( pixmap );
	}
    } else {
	normalText->setText( "I only show local files!" );
	raiseWidget( normalText );
    }
}

// ****************************************************************************************************

PreviewWidget::PreviewWidget( TQWidget *parent )
    : TQVBox( parent ), TQFilePreview()
{
    setSpacing( 5 );
    setMargin( 5 );
    TQHBox *row = new TQHBox( this );
    row->setSpacing( 5 );
    (void)new TQLabel( tr( "Only show files smaller than: " ), row );
    sizeSpinBox = new TQSpinBox( 1, 10000, 1, row );
    sizeSpinBox->setSuffix( " KB" );
    sizeSpinBox->setValue( 64 );
    row->setFixedHeight( 10 + sizeSpinBox->sizeHint().height() );
    preview = new Preview( this );
}

void PreviewWidget::previewUrl( const TQUrl &u )
{
    preview->showPreview( u, sizeSpinBox->value() );
}

// ****************************************************************************************************

CustomFileDialog::CustomFileDialog()
    :  TQFileDialog( 0, 0, true )
{
    setDir( "/" );

    dirView = new DirectoryView( this, 0, true );
    dirView->addColumn( "" );
    dirView->header()->hide();
    ::Directory *root = new ::Directory( dirView, "/" );
    root->setOpen( true );
    dirView->setFixedWidth( 150 );

    addLeftWidget( dirView );

    TQPushButton *p = new TQPushButton( this );
    p->setPixmap( TQPixmap( bookmarks ) );
    TQToolTip::add( p, tr( "Bookmarks" ) );

    bookmarkMenu = new TQPopupMenu( this );
    connect( bookmarkMenu, TQ_SIGNAL( activated( int ) ),
	     this, TQ_SLOT( bookmarkChosen( int ) ) );
    addId = bookmarkMenu->insertItem( tr( "Add bookmark" ) );
    bookmarkMenu->insertSeparator();

    TQFile f( ".bookmarks" );
    if ( f.open( IO_ReadOnly ) ) {
	TQDataStream ds( &f );
	ds >> bookmarkList;
	f.close();
	
	TQStringList::Iterator it = bookmarkList.begin();
	for ( ; it != bookmarkList.end(); ++it ) {
	    bookmarkMenu->insertItem( *it );
	}
    }
	
    p->setPopup( bookmarkMenu );

    addToolButton( p, true );

    connect( dirView, TQ_SIGNAL( folderSelected( const TQString & ) ),
	     this, TQ_SLOT( setDir2( const TQString & ) ) );
    connect( this, TQ_SIGNAL( dirEntered( const TQString & ) ),
	     dirView, TQ_SLOT( setDir( const TQString & ) ) );

    TQToolButton *b = new TQToolButton( this );
    TQToolTip::add( b, tr( "Go Home!" ) );
    b->setPixmap( TQPixmap( home ) );
    connect( b, TQ_SIGNAL( clicked() ),
	     this, TQ_SLOT( goHome() ) );

    addToolButton( b );

    resize( width() + width() / 3, height() );
}

CustomFileDialog::~CustomFileDialog()
{
    if ( !bookmarkList.isEmpty() ) {
	TQFile f( ".bookmarks" );
	if ( f.open( IO_WriteOnly ) ) {
	    TQDataStream ds( &f );
	    ds << bookmarkList;
	    f.close();
	}
    }
}

void CustomFileDialog::setDir2( const TQString &s )
{
    blockSignals( true );
    setDir( s );
    blockSignals( false );
}

void CustomFileDialog::showEvent( TQShowEvent *e )
{
    TQFileDialog::showEvent( e );
    dirView->setDir( dirPath() );
}

void CustomFileDialog::bookmarkChosen( int i )
{
    if ( i == addId ) {
	bookmarkList << dirPath();
	bookmarkMenu->insertItem( dirPath() );
    } else {
	setDir( bookmarkMenu->text( i ) );
    }
}

void CustomFileDialog::goHome()
{
    if ( getenv( "HOME" ) )
	setDir( getenv( "HOME" ) );
    else
	setDir( "/" );
}

// ****************************************************************************************************

int main( int argc, char ** argv )
{
    TQFileDialog::Mode mode = TQFileDialog::ExistingFile;
    TQString start;
    TQString filter;
    TQString caption;
    bool preview = false;
    bool custom = false;
    TQApplication a( argc, argv );
    for (int i=1; i<argc; i++) {
	TQString arg = argv[i];
	if ( arg == "-any" )
	    mode = TQFileDialog::AnyFile;
	else if ( arg == "-dir" )
	    mode = TQFileDialog::Directory;
	else if ( arg == "-default" )
	    start = argv[++i];
	else if ( arg == "-filter" )
	    filter = argv[++i];
	else if ( arg == "-preview" )
	    preview = true;
	else if ( arg == "-custom" )
	    custom = true;
	else if ( arg[0] == '-' ) {
	    tqDebug("Usage: qdir [-any | -dir | -custom] [-preview] [-default f] {-filter f} [caption ...]\n"
		   "      -any         Get any filename, need not exist.\n"
		   "      -dir         Return a directory rather than a file.\n"
		   "      -custom      Opens a customized TQFileDialog with \n"
		   "                   dir browser, bookmark menu, etc.\n"
		   "      -preview     Show a preview widget.\n"
		   "      -default f   Start from directory/file f.\n"
		   "      -filter f    eg. '*.gif' '*.bmp'\n"
		   "      caption ...  Caption for dialog.\n"
		   );
	    return 1;
	} else {
	    if ( !caption.isNull() )
		caption += ' ';
	    caption += arg;
	}
    }

    if ( !start )
	start = TQDir::currentDirPath();

    if ( !caption )
	caption = mode == TQFileDialog::Directory
		    ? "Choose directory..." : "Choose file...";

    if ( !custom ) {
	TQFileDialog fd( TQString::null, filter, 0, 0, true );
	fd.setMode( mode );
	if ( preview ) {
	    fd.setContentsPreviewEnabled( true );
	    PreviewWidget *pw = new PreviewWidget( &fd );
	    fd.setContentsPreview( pw, pw );
	    fd.setViewMode( TQFileDialog::List );
	    fd.setPreviewMode( TQFileDialog::Contents );
	}
	fd.setCaption( caption );
	fd.setSelection( start );
	if ( fd.exec() == TQDialog::Accepted ) {
	    TQString result = fd.selectedFile();
	    printf("%s\n", (const char*)result);
	    return 0;
	} else {
	    return 1;
	}
    } else {
	CustomFileDialog fd;
	fd.exec();
	return 1;
    }
}
