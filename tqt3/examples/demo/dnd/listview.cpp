#include <ntqdragobject.h>
#include <ntqapplication.h>
#include "listview.h"
#include "dnd.h"

ListView::ListView( TQWidget* parent, const char* name )
    : TQListView( parent, name )
{
    setAcceptDrops( true );
    setSorting( -1, false );
    dragging = false;
}

ListView::~ListView()
{

}

void ListView::dragEnterEvent( TQDragEnterEvent *e )
{
    if ( e->provides( "text/dragdemotag" ) )
	e->accept();
}

void ListView::dropEvent( TQDropEvent *e )
{
    if ( !e->provides( "text/dragdemotag" ) )
         return;

    TQString tag;

    if ( TQTextDrag::decode( e, tag ) ) {
        IconItem item = ((DnDDemo*) parentWidget())->findItem( tag );
        TQListViewItem *after = itemAt( viewport()->mapFromParent( e->pos() ) );
        ListViewItem *litem = new ListViewItem( this, after, item.name(), tag );
        litem->setPixmap( 0, *item.pixmap() );
    }
}

void ListView::contentsMousePressEvent( TQMouseEvent *e )
{
    TQListView::contentsMousePressEvent( e );
    dragging = true;
    pressPos = e->pos();
}

void ListView::contentsMouseMoveEvent( TQMouseEvent *e )
{
    TQListView::contentsMouseMoveEvent( e );

    if ( ! dragging ) return;

    if ( !currentItem() ) return;

    if ( ( pressPos - e->pos() ).manhattanLength() > TQApplication::startDragDistance() ) {
        TQTextDrag *drg = new TQTextDrag( ((ListViewItem*)currentItem())->tag(), this );

	const TQPixmap *p = ((ListViewItem*)currentItem())->pixmap( 0 );
	if (p)
	    drg->setPixmap(*p);
        drg->setSubtype( "dragdemotag" );
        drg->dragCopy();
        dragging = false;
    }
}

void ListView::contentsMouseReleaseEvent( TQMouseEvent *e )
{
    TQListView::contentsMouseReleaseEvent( e );
    dragging = false;
}

