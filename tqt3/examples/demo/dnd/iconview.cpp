#include <ntqdragobject.h>

#include "dnd.h"
#include "iconview.h"


IconView::IconView( TQWidget* parent, const char* name )
    : TQIconView( parent, name )
{
    connect( this, TQ_SIGNAL(dropped(TQDropEvent*, const TQValueList<TQIconDragItem>&)),
             TQ_SLOT(slotNewItem(TQDropEvent*, const TQValueList<TQIconDragItem>&)));
}

IconView::~IconView()
{

}


TQDragObject *IconView::dragObject()
{
    if ( !currentItem() ) return 0;

    TQTextDrag * drg = new TQTextDrag( ((IconViewItem*)currentItem())->tag(), this );
    drg->setSubtype("dragdemotag");
    drg->setPixmap( *currentItem()->pixmap() );

    return drg;
}

void IconView::slotNewItem( TQDropEvent *e, const TQValueList<TQIconDragItem>& )
{
    TQString tag;
    if ( !e->provides( "text/dragdemotag" ) ) return;

    if ( TQTextDrag::decode( e, tag ) ) {
        IconItem item = ((DnDDemo*) parentWidget())->findItem( tag );
        IconViewItem *iitem = new IconViewItem( this, item.name(), *item.pixmap(), tag );
        iitem->setRenameEnabled( true );
    }
    e->acceptAction();
}
