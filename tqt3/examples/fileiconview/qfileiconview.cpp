/****************************************************************************
**
** Copyright (C) 1992-2008 Trolltech ASA.  All rights reserved.
**
** This file is part of an example program for TQt.  This example
** program may be used, distributed and modified without limitation.
**
*****************************************************************************/

#include "qfileiconview.h"
#include <ntqpainter.h>
#include <ntqstringlist.h>
#include <ntqpixmap.h>
#include <ntqmime.h>
#include <ntqstrlist.h>
#include <ntqdragobject.h>
#include <ntqmessagebox.h>
#include <ntqevent.h>
#include <ntqpopupmenu.h>
#include <ntqcursor.h>
#include <ntqapplication.h>
#include <ntqwmatrix.h>

#include <stdlib.h>

static const char * file_icon[]={
    "32 32 17 1",
    "# c #000000",
    "a c #ffffff",
    "j c #808080",
    "n c #a0a0a4",
    "g c #c0c0c0",
    "m c #004000",
    "o c #000000",
    "l c #004040",
    "k c #404000",
    "i c #c0c000",
    "h c #ffff00",
    "b c #ffffc0",
    "e c #ff8000",
    "f c #c05800",
    "c c #ffa858",
    "d c #ffdca8",
    ". c None",
    "................................",
    "................................",
    "................................",
    "................................",
    ".............#....###...........",
    "...###......#a##.#aba##.........",
    "..#cdb#....#aaaa#aaaaaa##.......",
    "..#ecdb#..#aaaa#aaaaaaaba##.....",
    "..#fecdb##aaaa#aaaaaaaaaaab##...",
    "...#fecdb#aaa#aaaaaaabaabaaaa##.",
    "....#fecdb#a#baaaaa#baaaaaabaaa#",
    ".....#fecdb#aaaaab#a##baaaaaaa#.",
    ".....##fecdb#bbba#aaaa##baaab#..",
    "....#bb#fecdb#ba#aaaaaaa##aa#...",
    "...#bbbb#fecdb##aaabaaaaaa##....",
    "..#bbbb#b#fecdb#aaaaaaabaaaa##..",
    ".#bbbb#bbb#fecdg#aaaaaaaaaaaba#.",
    "#hhbb#bbbbb#fegg#iiaaaaaaaaaaaa#",
    "#jhhhklibbbk#ggj#aaiiaaaaaaaaa#j",
    ".#mjhhhkmikab####aaabiiaaaaaa#j.",
    "...##jhhhmaaibbaaiibaaaiiaab#n..",
    ".....##j#baaaiiabaaiibaabaa#n...",
    "......##baibaabiibaaaiiabb#j....",
    "......#bbbbiiaabbiiaaaaabon.....",
    ".....#bbbbbbbiiabbaiiaab#n......",
    ".....#jbbbbbbbbiibaabba#n.......",
    "......##jbbbbbbbbiiaabmj........",
    "........##jbbbbbbbbbb#j.........",
    "..........##nbbbbbbbmj..........",
    "............##jbbbb#j...........",
    "..............#mjj#n............",
    "................##n............."};

static const char * folder_icon[]={
    "32 32 11 1",
    "# c #000000",
    "b c #c0c000",
    "d c #585858",
    "a c #ffff00",
    "i c #400000",
    "h c #a0a0a4",
    "e c #000000",
    "c c #ffffff",
    "f c #303030",
    "g c #c0c0c0",
    ". c None",
    "...###..........................",
    "...#aa##........................",
    ".###baaa##......................",
    ".#cde#baaa##....................",
    ".#cccdeebaaa##..##f.............",
    ".#cccccdeebaaa##aaa##...........",
    ".#cccccccdeebaaaaaaaa##.........",
    ".#cccccccccdeebababaaa#.........",
    ".#cccccgcgghhebbbbbbbaa#........",
    ".#ccccccgcgggdebbbbbbba#........",
    ".#cccgcgcgcgghdeebiebbba#.......",
    ".#ccccgcggggggghdeddeeba#.......",
    ".#cgcgcgcggggggggghghdebb#......",
    ".#ccgcggggggggghghghghd#b#......",
    ".#cgcgcggggggggghghghhd#b#......",
    ".#gcggggggggghghghhhhhd#b#......",
    ".#cgcggggggggghghghhhhd#b#......",
    ".#ggggggggghghghhhhhhhdib#......",
    ".#gggggggggghghghhhhhhd#b#......",
    ".#hhggggghghghhhhhhhhhd#b#......",
    ".#ddhhgggghghghhhhhhhhd#b#......",
    "..##ddhhghghhhhhhhhhhhdeb#......",
    "....##ddhhhghhhhhhhhhhd#b#......",
    "......##ddhhhhhhhhhhhhd#b#......",
    "........##ddhhhhhhhhhhd#b#......",
    "..........##ddhhhhhhhhd#b#......",
    "............##ddhhhhhhd#b###....",
    "..............##ddhhhhd#b#####..",
    "................##ddhhd#b######.",
    "..................##dddeb#####..",
    "....................##d#b###....",
    "......................####......"};


static const char * link_icon[]={
    "32 32 12 1",
    "# c #000000",
    "h c #a0a0a4",
    "b c #c00000",
    "d c #585858",
    "i c #400000",
    "c c #ffffff",
    "e c #000000",
    "g c #c0c0c0",
    "a c #ff0000",
    "f c #303030",
    "n c white",
    ". c None",
    "...###..........................",
    "...#aa##........................",
    ".###baaa##......................",
    ".#cde#baaa##....................",
    ".#cccdeebaaa##..##f.............",
    ".#cccccdeebaaa##aaa##...........",
    ".#cccccccdeebaaaaaaaa##.........",
    ".#cccccccccdeebababaaa#.........",
    ".#cccccgcgghhebbbbbbbaa#........",
    ".#ccccccgcgggdebbbbbbba#........",
    ".#cccgcgcgcgghdeebiebbba#.......",
    ".#ccccgcggggggghdeddeeba#.......",
    ".#cgcgcgcggggggggghghdebb#......",
    ".#ccgcggggggggghghghghd#b#......",
    ".#cgcgcggggggggghghghhd#b#......",
    ".#gcggggggggghghghhhhhd#b#......",
    ".#cgcggggggggghghghhhhd#b#......",
    ".#ggggggggghghghhhhhhhdib#......",
    ".#gggggggggghghghhhhhhd#b#......",
    ".#hhggggghghghhhhhhhhhd#b#......",
    ".#ddhhgggghghghhhhhhhhd#b#......",
    "..##ddhhghghhhhhhhhhhhdeb#......",
    "############hhhhhhhhhhd#b#......",
    "#nnnnnnnnnn#hhhhhhhhhhd#b#......",
    "#nnnnnnnnnn#hhhhhhhhhhd#b#......",
    "#nn#nn#nnnn#ddhhhhhhhhd#b#......",
    "#nn##n##nnn###ddhhhhhhd#b###....",
    "#nnn#####nn#..##ddhhhhd#b#####..",
    "#nnnnn##nnn#....##ddhhd#b######.",
    "#nnnnn#nnnn#......##dddeb#####..",
    "#nnnnnnnnnn#........##d#b###....",
    "############..........####......"};

static const char * folder_locked_icon[]={
    "32 32 12 1",
    "# c #000000",
    "g c #808080",
    "h c #c0c0c0",
    "f c #c05800",
    "c c #ffffff",
    "d c #585858",
    "b c #ffa858",
    "a c #ffdca8",
    "e c #000000",
    "i c #a0a0a4",
    "j c #c0c0c0",
    ". c None",
    "...###..........................",
    "...#aa##........................",
    ".###baaa##......................",
    ".#cde#baaa##....................",
    ".#cccdeeba#######...............",
    ".#cccccde##fffff##..............",
    ".#cccccc##fffgggg#..............",
    ".#ccccccc#ffg####a##............",
    ".#ccccchc#ffg#eebbaa##..........",
    ".#ccccccc#ffg#ddeebbba##........",
    ".#ccchccc#ffg#ihddeebbba##......",
    ".#cccccaa#ffg#ihhhddeeba##......",
    ".#chchhbbaafg#ihhhihidebb#......",
    ".#cchccbbbbaa#ihhihihid#b#......",
    ".#chchhbb#bbbaaiihihiid#b#......",
    ".#hchhcbb#fbbbafhiiiiid#b#......",
    ".#chchhbb#ffgbbfihiiiid#b#......",
    ".#hhhhhbb#ffg#bfiiiiiid#b#......",
    ".#hhhhhbbaffg#bfiiiiiid#b#......",
    ".#iihhhjbbaab#bfiiiiiid#b#......",
    ".#ddiihhh#bbbabfiiiiiid#b#......",
    "..##ddiih#ffbbbfiiiiiid#b#......",
    "....##ddi#ffg#biiiiiiid#b#......",
    "......##d#ffg#iiiiiiiid#b#......",
    "........##ffg#iiiiiiiid#b#......",
    ".........#ffg#iiiiiiiid#b#......",
    ".........#ffg#ddiiiiiid#b###....",
    ".........##fg###ddiiiid#b#####..",
    "...........####.##ddiid#b######.",
    "..................##dddeb#####..",
    "....................##d#b###....",
    "......................####......"};

static TQPixmap *iconFolderLockedLarge = 0;
static TQPixmap *iconFolderLarge = 0;
static TQPixmap *iconFileLarge = 0;
static TQPixmap *iconLinkLarge = 0;
static TQPixmap *iconFolderLockedSmall = 0;
static TQPixmap *iconFolderSmall = 0;
static TQPixmap *iconFileSmall = 0;
static TQPixmap *iconLinkSmall = 0;

static void cleanup()
{
    delete iconFolderLockedLarge;
    iconFolderLockedLarge = 0;
    delete iconFolderLarge;
    iconFolderLarge = 0;
    delete iconFileLarge;
    iconFileLarge = 0;
    delete iconLinkLarge;
    iconLinkLarge = 0;
    delete iconFolderLockedSmall;
    iconFolderLockedSmall = 0;
    delete iconFolderSmall;
    iconFolderSmall = 0;
    delete iconFileSmall;
    iconFileSmall = 0;
    delete iconLinkSmall;
    iconLinkSmall = 0;
}

/*****************************************************************************
 *
 * Class TQtFileIconDrag
 *
 *****************************************************************************/

TQtFileIconDrag::TQtFileIconDrag( TQWidget * dragSource, const char* name )
    : TQIconDrag( dragSource, name )
{
}

const char* TQtFileIconDrag::format( int i ) const
{
    if ( i == 0 )
	return "application/x-qiconlist";
    else if ( i == 1 )
	return "text/uri-list";
    else
	return 0;
}

TQByteArray TQtFileIconDrag::encodedData( const char* mime ) const
{
    TQByteArray a;
    if ( TQString( mime ) == "application/x-qiconlist" ) {
	a = TQIconDrag::encodedData( mime );
    } else if ( TQString( mime ) == "text/uri-list" ) {
	TQString s = urls.join( "\r\n" );
	a.resize( s.length() );
	memcpy( a.data(), s.latin1(), s.length() );
    }
    return a;
}

bool TQtFileIconDrag::canDecode( TQMimeSource* e )
{
    return e->provides( "application/x-qiconlist" ) ||
	e->provides( "text/uri-list" );
}

void TQtFileIconDrag::append( const TQIconDragItem &item, const TQRect &pr,
			     const TQRect &tr, const TQString &url )
{
    TQIconDrag::append( item, pr, tr );
    TQString ourUrl = url;    
#ifdef TQ_WS_WIN
    if (ourUrl.length() > 2 && ourUrl[1] != ':') {
	TQDir dir(ourUrl);
	ourUrl = dir.absPath();
    }
#endif
    urls << TQUriDrag::localFileToUri(ourUrl);
}

/*****************************************************************************
 *
 * Class TQtFileIconViewItem
 *
 *****************************************************************************/

TQtFileIconViewItem::TQtFileIconViewItem( TQtFileIconView *parent, TQFileInfo *fi )
    : TQIconViewItem( parent, fi->fileName() ), itemFileName( fi->filePath() ),
      itemFileInfo( fi ), checkSetText( false )
{
    vm = TQtFileIconView::Large;

    if ( itemFileInfo->isDir() )
	itemType = Dir;
    else if ( itemFileInfo->isFile() )
	itemType = File;
    if ( itemFileInfo->isSymLink() )
	itemType = Link;

    viewModeChanged( ( (TQtFileIconView*)iconView() )->viewMode() );

    if ( itemFileInfo->fileName() == "." ||
	 itemFileInfo->fileName() == ".." )
	setRenameEnabled( false );

    checkSetText = true;

    TQObject::connect( &timer, TQ_SIGNAL( timeout() ),
		      iconView(), TQ_SLOT( openFolder() ) );
}

void TQtFileIconViewItem::paintItem( TQPainter *p, const TQColorGroup &cg )
{
    if ( itemFileInfo->isSymLink() ) {
	TQFont f( p->font() );
	f.setItalic( true );
	p->setFont( f );
    }

    TQIconViewItem::paintItem( p, cg );
}

void TQtFileIconViewItem::viewModeChanged( TQtFileIconView::ViewMode m )
{
    vm = m;
    setDropEnabled( itemType == Dir && TQDir( itemFileName ).isReadable() );
    calcRect();
}

TQPixmap *TQtFileIconViewItem::pixmap() const
{
    switch ( itemType ) {
    case Dir:
	{
	    if ( !TQDir( itemFileName ).isReadable() ) {
		if ( vm == TQtFileIconView::Small )
		    return iconFolderLockedSmall;
		else
		    return iconFolderLockedLarge;
	    } else {
		if ( vm == TQtFileIconView::Small )
		    return iconFolderSmall;
		else
		    return iconFolderLarge;
	    }
	}
    case Link:
	{
	    if ( vm == TQtFileIconView::Small )
		return iconLinkSmall;
	    else
		return iconLinkLarge;
	}
    default:
	{
	    if ( vm == TQtFileIconView::Small )
		return iconFileSmall;
	    else
		return iconFileLarge;
	}
    }
}

TQtFileIconViewItem::~TQtFileIconViewItem()
{
    delete itemFileInfo;
}

void TQtFileIconViewItem::setText( const TQString &text )
{
    if ( checkSetText ) {
	if ( text == "." || text == "." || text.isEmpty() )
	    return;
	TQDir dir( itemFileInfo->dir() );
	if ( dir.rename( itemFileInfo->fileName(), text ) ) {
	    itemFileName = itemFileInfo->dirPath( true ) + "/" + text;
	    delete itemFileInfo;
	    itemFileInfo = new TQFileInfo( itemFileName );
	    TQIconViewItem::setText( text );
	}
    } else {
	TQIconViewItem::setText( text );
    }
}

bool TQtFileIconViewItem::acceptDrop( const TQMimeSource *e ) const
{
    if ( type() == Dir && e->provides( "text/uri-list" ) &&
	 dropEnabled() )
	return true;

    return false;
}

void TQtFileIconViewItem::dropped( TQDropEvent *e, const TQValueList<TQIconDragItem> & )
{
    timer.stop();

    if ( !TQUriDrag::canDecode( e ) ) {
	e->ignore();
	return;
    }

    TQStringList lst;
    TQUriDrag::decodeLocalFiles( e, lst );

    TQString str;
    if ( e->action() == TQDropEvent::Copy )
	str = "Copy\n\n";
    else
	str = "Move\n\n";
    for ( uint i = 0; i < lst.count(); ++i )
	str += TQString( "   %1\n" ).arg( lst[i] );
    str += TQString( "\n"
	"To\n\n"
	"	%1" ).arg( filename() );

    TQMessageBox::information( iconView(), e->action() == TQDropEvent::Copy ? "Copy" : "Move" , str, "Not Implemented" );
    if ( e->action() == TQDropEvent::Move )
	TQMessageBox::information( iconView(), "Remove" , str, "Not Implemented" );
    e->acceptAction();
}

void TQtFileIconViewItem::dragEntered()
{
    if ( type() != Dir ||
	 type() == Dir && !TQDir( itemFileName ).isReadable() )
	return;

    ( (TQtFileIconView*)iconView() )->setOpenItem( this );
    timer.start( 1500 );
}

void TQtFileIconViewItem::dragLeft()
{
    if ( type() != Dir ||
	 type() == Dir && !TQDir( itemFileName ).isReadable() )
	return;

    timer.stop();
}

/*****************************************************************************
 *
 * Class TQtFileIconView
 *
 *****************************************************************************/

TQtFileIconView::TQtFileIconView( const TQString &dir, TQWidget *parent, const char *name )
    : TQIconView( parent, name ), viewDir( dir ), newFolderNum( 0 )
{
    if ( !iconFolderLockedLarge ) {
	tqAddPostRoutine( cleanup );
	TQWMatrix m;
	m.scale( 0.6, 0.6 );
	TQPixmap iconpix( folder_locked_icon );
	iconFolderLockedLarge = new TQPixmap( folder_locked_icon );
	iconpix = iconpix.xForm( m );
	iconFolderLockedSmall = new TQPixmap( iconpix );
	iconpix = TQPixmap( folder_icon );
	iconFolderLarge = new TQPixmap( folder_icon );
	iconpix = iconpix.xForm( m );
	iconFolderSmall = new TQPixmap( iconpix );
	iconpix = TQPixmap( file_icon );
	iconFileLarge = new TQPixmap( file_icon );
	iconpix = iconpix.xForm( m );
	iconFileSmall = new TQPixmap( iconpix );
	iconpix = TQPixmap( link_icon );
	iconLinkLarge = new TQPixmap( link_icon );
	iconpix = iconpix.xForm( m );
	iconLinkSmall = new TQPixmap( iconpix );
    }

    vm = Large;

    setGridX( 75 );
    setResizeMode( Adjust );
    setWordWrapIconText( false );

    connect( this, TQ_SIGNAL( doubleClicked( TQIconViewItem * ) ),
	     this, TQ_SLOT( itemDoubleClicked( TQIconViewItem * ) ) );
    connect( this, TQ_SIGNAL( returnPressed( TQIconViewItem * ) ),
	     this, TQ_SLOT( itemDoubleClicked( TQIconViewItem * ) ) );
    connect( this, TQ_SIGNAL( dropped( TQDropEvent *, const TQValueList<TQIconDragItem> & ) ),
	     this, TQ_SLOT( slotDropped( TQDropEvent *, const TQValueList<TQIconDragItem> & ) ) );
    connect( this, TQ_SIGNAL( contextMenuRequested( TQIconViewItem *, const TQPoint & ) ),
	     this, TQ_SLOT( slotRightPressed( TQIconViewItem * ) ) );

    setHScrollBarMode( AlwaysOff );
    setVScrollBarMode( Auto );

    setAutoArrange( true );
    setSorting( true );
    openItem = 0;
}

void TQtFileIconView::openFolder()
{
    if ( !openItem )
	return;
    if ( openItem->type() != TQtFileIconViewItem::Dir ||
	 openItem->type() == TQtFileIconViewItem::Dir &&
	 !TQDir( openItem->itemFileName ).isReadable() )
	return;

    openItem->timer.stop();
    setDirectory( openItem->itemFileName );
}

void TQtFileIconView::setDirectory( const TQString &dir )
{
    viewDir = TQDir( dir );
    readDir( viewDir );
}

void TQtFileIconView::setDirectory( const TQDir &dir )
{
    viewDir = dir;
    readDir( viewDir );
}

void TQtFileIconView::newDirectory()
{
    setAutoArrange( false );
    selectAll( false );
    if ( viewDir.mkdir( TQString( "New Folder %1" ).arg( ++newFolderNum ) ) ) {
	TQFileInfo *fi = new TQFileInfo( viewDir, TQString( "New Folder %1" ).arg( newFolderNum ) );
	TQtFileIconViewItem *item = new TQtFileIconViewItem( this, new TQFileInfo( *fi ) );
	item->setKey( TQString( "000000%1" ).arg( fi->fileName() ) );
	delete fi;
	repaintContents( contentsX(), contentsY(), contentsWidth(), contentsHeight(), false );
	ensureItemVisible( item );
	item->setSelected( true, true );
	setCurrentItem( item );
	repaintItem( item );
	tqApp->processEvents();
	item->rename();
    }
    setAutoArrange( true );
}

TQDir TQtFileIconView::currentDir()
{
    return viewDir;
}

static bool isRoot( const TQString &s )
{
#if defined(Q_OS_UNIX)
    if ( s == "/" )
	return true;
#elif defined(Q_OS_WIN32)
    TQString p = s;
    if ( p.length() == 3 &&
	 p.right( 2 ) == ":/" )
	return true;
    if ( p[ 0 ] == '/' && p[ 1 ] == '/' ) {
	int slashes = p.contains( '/' );
	if ( slashes <= 3 )
	    return true;
	if ( slashes == 4 && p[ (int)p.length() - 1 ] == '/' )
	    return true;
    }
#endif

    return false;
}

void TQtFileIconView::readDir( const TQDir &dir )
{
    if ( !dir.isReadable() )
	return;

    if ( isRoot( dir.absPath() ) )
	emit disableUp();
    else
	emit enableUp();

    clear();

    emit directoryChanged( dir.absPath() );

    const TQFileInfoList *filist = dir.entryInfoList( TQDir::DefaultFilter, TQDir::DirsFirst | TQDir::Name );

    emit startReadDir( filist->count() );

    TQFileInfoListIterator it( *filist );
    TQFileInfo *fi;
    bool allowRename = false, allowRenameSet = false;
    while ( ( fi = it.current() ) != 0 ) {
	++it;
	if ( fi && fi->fileName() == ".." && ( fi->dirPath() == "/" || fi->dirPath().isEmpty() ) )
	    continue;
	emit readNextDir();
	TQtFileIconViewItem *item = new TQtFileIconViewItem( this, new TQFileInfo( *fi ) );
	if ( fi->isDir() )
	    item->setKey( TQString( "000000%1" ).arg( fi->fileName() ) );
	else
	    item->setKey( fi->fileName() );
	if ( !allowRenameSet ) {
	    if ( !TQFileInfo( fi->absFilePath() ).isWritable() ||
		 item->text() == "." || item->text() == ".." )
		allowRename = false;
	    else
		allowRename = true;
	    if ( item->text() == "." || item->text() == ".." )
		allowRenameSet = false;
	    else
		allowRenameSet = true;
	}
	item->setRenameEnabled( allowRename );
    }

    if ( !TQFileInfo( dir.absPath() ).isWritable() )
	emit disableMkdir();
    else
	emit enableMkdir();

    emit readDirDone();
}

void TQtFileIconView::itemDoubleClicked( TQIconViewItem *i )
{
    TQtFileIconViewItem *item = ( TQtFileIconViewItem* )i;

    if ( item->type() == TQtFileIconViewItem::Dir ) {
	viewDir = TQDir( item->filename() );
	readDir( viewDir );
    } else if ( item->type() == TQtFileIconViewItem::Link &&
		TQFileInfo( TQFileInfo( item->filename() ).readLink() ).isDir() ) {
	viewDir = TQDir( TQFileInfo( item->filename() ).readLink() );
	readDir( viewDir );
    }
}

TQDragObject *TQtFileIconView::dragObject()
{
    if ( !currentItem() )
	return 0;

    TQPoint orig = viewportToContents( viewport()->mapFromGlobal( TQCursor::pos() ) );
    TQtFileIconDrag *drag = new TQtFileIconDrag( viewport() );
    drag->setPixmap( *currentItem()->pixmap(),
		     TQPoint( currentItem()->pixmapRect().width() / 2, currentItem()->pixmapRect().height() / 2 ) );
    for ( TQtFileIconViewItem *item = (TQtFileIconViewItem*)firstItem(); item;
	  item = (TQtFileIconViewItem*)item->nextItem() ) {
	if ( item->isSelected() ) {
	    TQIconDragItem id;
	    id.setData( TQCString( item->filename() ) );
	    drag->append( id,
			  TQRect( item->pixmapRect( false ).x() - orig.x(),
				 item->pixmapRect( false ).y() - orig.y(),
				 item->pixmapRect().width(), item->pixmapRect().height() ),
			  TQRect( item->textRect( false ).x() - orig.x(),
				 item->textRect( false ).y() - orig.y(),
				 item->textRect().width(), item->textRect().height() ),
			  TQString( item->filename() ) );
	}
    }

    return drag;
}

void TQtFileIconView::keyPressEvent( TQKeyEvent *e )
{
    if ( e->key() == Key_N &&
	 ( e->state() & ControlButton ) )
	newDirectory();
    else
	TQIconView::keyPressEvent( e );
}

void TQtFileIconView::slotDropped( TQDropEvent *e, const TQValueList<TQIconDragItem> & )
{
    if ( openItem )
	openItem->timer.stop();
    if ( !TQUriDrag::canDecode( e ) ) {
	e->ignore();
	return;
    }

    TQStringList lst;
    TQUriDrag::decodeLocalFiles( e, lst );

    TQString str;
    if ( e->action() == TQDropEvent::Copy )
	str = "Copy\n\n";
    else
	str = "Move\n\n";
    for ( uint i = 0; i < lst.count(); ++i )
	str += TQString( "   %1\n" ).arg( TQDir::convertSeparators(lst[i]) );
    str += TQString( "\n"
	"To\n\n"
	"	%1" ).arg( viewDir.absPath() );

    TQMessageBox::information( this, e->action() == TQDropEvent::Copy ? "Copy" : "Move" , str, "Not Implemented" );
    if ( e->action() == TQDropEvent::Move )
	TQMessageBox::information( this, "Remove" , TQDir::convertSeparators(lst.join("\n")), "Not Implemented" );
    e->acceptAction();
    openItem = 0;
}

void TQtFileIconView::viewLarge()
{
    setViewMode( Large );
}

void TQtFileIconView::viewSmall()
{
    setViewMode( Small );
}

void TQtFileIconView::viewBottom()
{
    setItemTextPos( Bottom );
}

void TQtFileIconView::viewRight()
{
    setItemTextPos( Right );
}

void TQtFileIconView::flowEast()
{
    setHScrollBarMode( AlwaysOff );
    setVScrollBarMode( Auto );
    setArrangement( LeftToRight );
}

void TQtFileIconView::flowSouth()
{
    setVScrollBarMode( AlwaysOff );
    setHScrollBarMode( Auto );
    setArrangement( TopToBottom );
}

void TQtFileIconView::sortAscending()
{
    sort( true );
}

void TQtFileIconView::sortDescending()
{
    sort( false );
}

void TQtFileIconView::itemTextTruncate()
{
    setWordWrapIconText( false );
}

void TQtFileIconView::itemTextWordWrap()
{
    setWordWrapIconText( true );
}

void TQtFileIconView::slotRightPressed( TQIconViewItem *item )
{
    if ( !item ) { // right pressed on viewport
	TQPopupMenu menu( this );

	menu.insertItem( "&Large view", this, TQ_SLOT( viewLarge() ) );
	menu.insertItem( "&Small view", this, TQ_SLOT( viewSmall() ) );
	menu.insertSeparator();
	menu.insertItem( "Text at the &bottom", this, TQ_SLOT( viewBottom() ) );
	menu.insertItem( "Text at the &right", this, TQ_SLOT( viewRight() ) );
	menu.insertSeparator();
	menu.insertItem( "Arrange l&eft to right", this, TQ_SLOT( flowEast() ) );
	menu.insertItem( "Arrange t&op to bottom", this, TQ_SLOT( flowSouth() ) );
	menu.insertSeparator();
	menu.insertItem( "&Truncate item text", this, TQ_SLOT( itemTextTruncate() ) );
	menu.insertItem( "&Wordwrap item text", this, TQ_SLOT( itemTextWordWrap() ) );
	menu.insertSeparator();
	menu.insertItem( "Arrange items in &grid", this, TQ_SLOT( arrangeItemsInGrid() ) );
	menu.insertSeparator();
	menu.insertItem( "Sort &ascending", this, TQ_SLOT( sortAscending() ) );
	menu.insertItem( "Sort &descending", this, TQ_SLOT( sortDescending() ) );

	menu.setMouseTracking( true );
	menu.exec( TQCursor::pos() );
    } else { // on item
	TQPopupMenu menu( this );

	int RENAME_ITEM = menu.insertItem( "Rename Item" );
	int REMOVE_ITEM = menu.insertItem( "Remove Item" );

	menu.setMouseTracking( true );
	int id = menu.exec( TQCursor::pos() );

	if ( id == -1 )
	    return;

	if ( id == RENAME_ITEM && item->renameEnabled() ) {
	    item->rename();
	} else if ( id == REMOVE_ITEM ) {
	    delete item;
	    TQMessageBox::information( this, "Not implemented!", "Deleting files not implemented yet,\n"
				      "The item has only been removed from the view! " );
	}
    }
}

void TQtFileIconView::setViewMode( ViewMode m )
{
    if ( m == vm )
	return;

    vm = m;
    TQtFileIconViewItem *item = (TQtFileIconViewItem*)firstItem();
    for ( ; item; item = (TQtFileIconViewItem*)item->nextItem() )
	item->viewModeChanged( vm );

    arrangeItemsInGrid();
}
