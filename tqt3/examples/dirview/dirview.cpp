/****************************************************************************
**
** Copyright (C) 1992-2008 Trolltech ASA.  All rights reserved.
**
** This file is part of an example program for TQt.  This example
** program may be used, distributed and modified without limitation.
**
*****************************************************************************/

#include "dirview.h"

#include <ntqdir.h>
#include <ntqfile.h>
#include <ntqfileinfo.h>
#include <ntqpixmap.h>
#include <ntqevent.h>
#include <ntqpoint.h>
#include <ntqmessagebox.h>
#include <ntqdragobject.h>
#include <ntqmime.h>
#include <ntqstrlist.h>
#include <ntqstringlist.h>
#include <ntqapplication.h>
#include <ntqheader.h>

static const char* folder_closed_xpm[]={
    "16 16 9 1",
    "g c #808080",
    "b c #c0c000",
    "e c #c0c0c0",
    "# c #000000",
    "c c #ffff00",
    ". c None",
    "a c #585858",
    "f c #a0a0a4",
    "d c #ffffff",
    "..###...........",
    ".#abc##.........",
    ".#daabc#####....",
    ".#ddeaabbccc#...",
    ".#dedeeabbbba...",
    ".#edeeeeaaaab#..",
    ".#deeeeeeefe#ba.",
    ".#eeeeeeefef#ba.",
    ".#eeeeeefeff#ba.",
    ".#eeeeefefff#ba.",
    ".##geefeffff#ba.",
    "...##gefffff#ba.",
    ".....##fffff#ba.",
    ".......##fff#b##",
    ".........##f#b##",
    "...........####."};

static const char* folder_open_xpm[]={
    "16 16 11 1",
    "# c #000000",
    "g c #c0c0c0",
    "e c #303030",
    "a c #ffa858",
    "b c #808080",
    "d c #a0a0a4",
    "f c #585858",
    "c c #ffdca8",
    "h c #dcdcdc",
    "i c #ffffff",
    ". c None",
    "....###.........",
    "....#ab##.......",
    "....#acab####...",
    "###.#acccccca#..",
    "#ddefaaaccccca#.",
    "#bdddbaaaacccab#",
    ".eddddbbaaaacab#",
    ".#bddggdbbaaaab#",
    "..edgdggggbbaab#",
    "..#bgggghghdaab#",
    "...ebhggghicfab#",
    "....#edhhiiidab#",
    "......#egiiicfb#",
    "........#egiibb#",
    "..........#egib#",
    "............#ee#"};

static const char * folder_locked[]={
    "16 16 10 1",
    "h c #808080",
    "b c #ffa858",
    "f c #c0c0c0",
    "e c #c05800",
    "# c #000000",
    "c c #ffdca8",
    ". c None",
    "a c #585858",
    "g c #a0a0a4",
    "d c #ffffff",
    "..#a#...........",
    ".#abc####.......",
    ".#daa#eee#......",
    ".#ddf#e##b#.....",
    ".#dfd#e#bcb##...",
    ".#fdccc#daaab#..",
    ".#dfbbbccgfg#ba.",
    ".#ffb#ebbfgg#ba.",
    ".#ffbbe#bggg#ba.",
    ".#fffbbebggg#ba.",
    ".##hf#ebbggg#ba.",
    "...###e#gggg#ba.",
    ".....#e#gggg#ba.",
    "......###ggg#b##",
    ".........##g#b##",
    "...........####."};

static const char * pix_file []={
    "16 16 7 1",
    "# c #000000",
    "b c #ffffff",
    "e c #000000",
    "d c #404000",
    "c c #c0c000",
    "a c #ffffc0",
    ". c None",
    "................",
    ".........#......",
    "......#.#a##....",
    ".....#b#bbba##..",
    "....#b#bbbabbb#.",
    "...#b#bba##bb#..",
    "..#b#abb#bb##...",
    ".#a#aab#bbbab##.",
    "#a#aaa#bcbbbbbb#",
    "#ccdc#bcbbcbbb#.",
    ".##c#bcbbcabb#..",
    "...#acbacbbbe...",
    "..#aaaacaba#....",
    "...##aaaaa#.....",
    ".....##aa#......",
    ".......##......."};

TQPixmap *folderLocked = 0;
TQPixmap *folderClosed = 0;
TQPixmap *folderOpen = 0;
TQPixmap *fileNormal = 0;

/*****************************************************************************
 *
 * Class Directory
 *
 *****************************************************************************/

Directory::Directory( Directory * parent, const TQString& filename )
    : TQListViewItem( parent ), f(filename),
      showDirsOnly( parent->showDirsOnly ),
      pix( 0 )
{
    p = parent;
    readable = TQDir( fullName() ).isReadable();

    if ( !readable )
	setPixmap( folderLocked );
    else
	setPixmap( folderClosed );
}


Directory::Directory( TQListView * parent, const TQString& filename )
    : TQListViewItem( parent ), f(filename),
      showDirsOnly( ( (DirectoryView*)parent )->showDirsOnly() ),
      pix( 0 )
{
    p = 0;
    readable = TQDir( fullName() ).isReadable();
}


void Directory::setPixmap( TQPixmap *px )
{
    pix = px;
    setup();
    widthChanged( 0 );
    invalidateHeight();
    repaint();
}


const TQPixmap *Directory::pixmap( int i ) const
{
    if ( i )
	return 0;
    return pix;
}

void Directory::setOpen( bool o )
{
    if ( o )
	setPixmap( folderOpen );
    else
	setPixmap( folderClosed );

    if ( o && !childCount() ) {
	TQString s( fullName() );
	TQDir thisDir( s );
	if ( !thisDir.isReadable() ) {
	    readable = false;
	    setExpandable( false );
	    return;
	}

	listView()->setUpdatesEnabled( false );
	const TQFileInfoList * files = thisDir.entryInfoList();
	if ( files ) {
	    TQFileInfoListIterator it( *files );
	    TQFileInfo * fi;
	    while( (fi=it.current()) != 0 ) {
		++it;
		if ( fi->fileName() == "." || fi->fileName() == ".." )
		    ; // nothing
		else if ( fi->isSymLink() && !showDirsOnly ) {
		    FileItem *item = new FileItem( this, fi->fileName(),
						     "Symbolic Link" );
		    item->setPixmap( fileNormal );
		}
		else if ( fi->isDir() )
		    (void)new Directory( this, fi->fileName() );
		else if ( !showDirsOnly ) {
		    FileItem *item
			= new FileItem( this, fi->fileName(),
					     fi->isFile()?"File":"Special" );
		    item->setPixmap( fileNormal );
		}
	    }
	}
	listView()->setUpdatesEnabled( true );
    }
    TQListViewItem::setOpen( o );
}


void Directory::setup()
{
    setExpandable( true );
    TQListViewItem::setup();
}


TQString Directory::fullName()
{
    TQString s;
    if ( p ) {
	s = p->fullName();
	s.append( f.name() );
	s.append( "/" );
    } else {
	s = f.name();
    }
    return s;
}


TQString Directory::text( int column ) const
{
    if ( column == 0 )
	return f.name();
    else if ( readable )
	return "Directory";
    else
	return "Unreadable Directory";
}

/*****************************************************************************
 *
 * Class DirectoryView
 *
 *****************************************************************************/

DirectoryView::DirectoryView( TQWidget *parent, const char *name, bool sdo )
    : TQListView( parent, name ), dirsOnly( sdo ), oldCurrent( 0 ),
      dropItem( 0 ), mousePressed( false )
{
    autoopen_timer = new TQTimer( this );
    if ( !folderLocked ) {
	folderLocked = new TQPixmap( folder_locked );
	folderClosed = new TQPixmap( folder_closed_xpm );
	folderOpen = new TQPixmap( folder_open_xpm );
	fileNormal = new TQPixmap( pix_file );
    }

    connect( this, TQ_SIGNAL( doubleClicked( TQListViewItem * ) ),
	     this, TQ_SLOT( slotFolderSelected( TQListViewItem * ) ) );
    connect( this, TQ_SIGNAL( returnPressed( TQListViewItem * ) ),
	     this, TQ_SLOT( slotFolderSelected( TQListViewItem * ) ) );

    setAcceptDrops( true );
    viewport()->setAcceptDrops( true );

    connect( autoopen_timer, TQ_SIGNAL( timeout() ),
	     this, TQ_SLOT( openFolder() ) );
}

void DirectoryView::slotFolderSelected( TQListViewItem *i )
{
    if ( !i || !showDirsOnly() )
	return;

    Directory *dir = (Directory*)i;
    emit folderSelected( dir->fullName() );
}

void DirectoryView::openFolder()
{
    autoopen_timer->stop();
    if ( dropItem && !dropItem->isOpen() ) {
	dropItem->setOpen( true );
	dropItem->repaint();
    }
}

static const int autoopenTime = 750;


void DirectoryView::contentsDragEnterEvent( TQDragEnterEvent *e )
{
    if ( !TQUriDrag::canDecode(e) ) {
	e->ignore();
	return;
    }

    oldCurrent = currentItem();

    TQListViewItem *i = itemAt( contentsToViewport(e->pos()) );
    if ( i ) {
	dropItem = i;
	autoopen_timer->start( autoopenTime );
    }
}


void DirectoryView::contentsDragMoveEvent( TQDragMoveEvent *e )
{
    if ( !TQUriDrag::canDecode(e) ) {
	e->ignore();
	return;
    }

    TQPoint vp = contentsToViewport( ( (TQDragMoveEvent*)e )->pos() );
    TQListViewItem *i = itemAt( vp );
    if ( i ) {
	setSelected( i, true );
	e->accept();
	if ( i != dropItem ) {
	    autoopen_timer->stop();
	    dropItem = i;
	    autoopen_timer->start( autoopenTime );
	}
	switch ( e->action() ) {
	case TQDropEvent::Copy:
	    break;
	case TQDropEvent::Move:
	    e->acceptAction();
	    break;
	case TQDropEvent::Link:
	    e->acceptAction();
	    break;
	default:
	    ;
	}
    } else {
	e->ignore();
	autoopen_timer->stop();
	dropItem = 0;
    }
}

void DirectoryView::contentsDragLeaveEvent( TQDragLeaveEvent * )
{
    autoopen_timer->stop();
    dropItem = 0;

    setCurrentItem( oldCurrent );
    setSelected( oldCurrent, true );
}

void DirectoryView::contentsDropEvent( TQDropEvent *e )
{
    autoopen_timer->stop();

    if ( !TQUriDrag::canDecode(e) ) {
	e->ignore();
	return;
    }

    TQListViewItem *item = itemAt( contentsToViewport(e->pos()) );
    if ( item ) {

	TQStrList lst;

	TQUriDrag::decode( e, lst );

	TQString str;

	switch ( e->action() ) {
	    case TQDropEvent::Copy:
	    str = "Copy";
	    break;
	    case TQDropEvent::Move:
	    str = "Move";
	    e->acceptAction();
	    break;
	    case TQDropEvent::Link:
	    str = "Link";
	    e->acceptAction();
	    break;
	    default:
	    str = "Unknown";
	}

	str += "\n\n";

	e->accept();

	for ( uint i = 0; i < lst.count(); ++i ) {
	    TQString filename = TQDir::convertSeparators(TQUriDrag::uriToLocalFile(lst.at(i)));
	    str += filename + "\n";
	}
	str += TQString( "\nTo\n\n   %1" )
	    .arg( TQDir::convertSeparators(fullPath(item)) );
	
	TQMessageBox::information( this, "Drop target", str, "Not implemented" );
    } else
	e->ignore();

}


TQString DirectoryView::fullPath(TQListViewItem* item)
{
    TQString fullpath = item->text(0);
    while ( (item=item->parent()) ) {
	if ( item->parent() )
	    fullpath = item->text(0) + "/" + fullpath;
	else
	    fullpath = item->text(0) + fullpath;
    }
#ifdef TQ_WS_WIN
	if (fullpath.length() > 2 && fullpath[1] != ':') {
		TQDir dir(fullpath);
		fullpath = dir.currentDirPath().left(2) + fullpath;
	}
#endif
	
    return fullpath;
}

void DirectoryView::contentsMousePressEvent( TQMouseEvent* e )
{
    TQListView::contentsMousePressEvent(e);
    TQPoint p( contentsToViewport( e->pos() ) );
    TQListViewItem *i = itemAt( p );
    if ( i ) {
	// if the user clicked into the root decoration of the item, don't try to start a drag!
	if ( p.x() > header()->cellPos( header()->mapToActual( 0 ) ) +
	     treeStepSize() * ( i->depth() + ( rootIsDecorated() ? 1 : 0) ) + itemMargin() ||
	     p.x() < header()->cellPos( header()->mapToActual( 0 ) ) ) {
	    presspos = e->pos();
	    mousePressed = true;
	}
    }
}

void DirectoryView::contentsMouseMoveEvent( TQMouseEvent* e )
{
    if ( mousePressed && ( presspos - e->pos() ).manhattanLength() > TQApplication::startDragDistance() ) {
	mousePressed = false;
	TQListViewItem *item = itemAt( contentsToViewport(presspos) );
	if ( item ) {
	    TQString source = fullPath(item);
	    if ( TQFile::exists(source) ) {
		TQUriDrag* ud = new TQUriDrag(viewport());
		ud->setFileNames( source );
		if ( ud->drag() )
		    TQMessageBox::information( this, "Drag source",
		    TQString("Delete ") + TQDir::convertSeparators(source), "Not implemented" );
	    }
	}
    }
}

void DirectoryView::contentsMouseReleaseEvent( TQMouseEvent * )
{
    mousePressed = false;
}

void DirectoryView::setDir( const TQString &s )
{
    TQListViewItemIterator it( this );
    ++it;
    for ( ; it.current(); ++it ) {
	it.current()->setOpen( false );
    }

    TQStringList lst( TQStringList::split( "/", s ) );
    TQListViewItem *item = firstChild();
    TQStringList::Iterator it2 = lst.begin();
    for ( ; it2 != lst.end(); ++it2 ) {
	while ( item ) {
	    if ( item->text( 0 ) == *it2 ) {
		item->setOpen( true );
		break;
	    }
	    item = item->itemBelow();
	}
    }

    if ( item )
	setCurrentItem( item );
}

void FileItem::setPixmap( TQPixmap *p )
{
    pix = p;
    setup();
    widthChanged( 0 );
    invalidateHeight();
    repaint();
}


const TQPixmap *FileItem::pixmap( int i ) const
{
    if ( i )
	return 0;
    return pix;
}
