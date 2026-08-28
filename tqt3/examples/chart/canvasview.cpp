#include "canvasview.h"
#include "chartform.h"

#include <ntqcursor.h>
#include <ntqpoint.h>
#include <ntqpopupmenu.h>
#include <ntqstatusbar.h>


void CanvasView::contentsContextMenuEvent( TQContextMenuEvent * )
{
    ((ChartForm*)parent())->optionsMenu->exec( TQCursor::pos() );
}


void CanvasView::viewportResizeEvent( TQResizeEvent *e )
{
    canvas()->resize( e->size().width(), e->size().height() );
    ((ChartForm*)parent())->drawElements();
}


void CanvasView::contentsMousePressEvent( TQMouseEvent *e )
{
    TQCanvasItemList list = canvas()->collisions( e->pos() );
    for ( TQCanvasItemList::iterator it = list.begin(); it != list.end(); ++it )
	if ( (*it)->rtti() == CanvasText::CANVAS_TEXT ) {
	    m_movingItem = *it;
	    m_pos = e->pos();
	    return;
	}
    m_movingItem = 0;
}


void CanvasView::contentsMouseMoveEvent( TQMouseEvent *e )
{
    if ( m_movingItem ) {
	TQPoint offset = e->pos() - m_pos;
	m_movingItem->moveBy( offset.x(), offset.y() );
	m_pos = e->pos();
	ChartForm *form = (ChartForm*)parent();
	form->setChanged( true );
	int chartType = form->chartType();
	CanvasText *item = (CanvasText*)m_movingItem;
	int i = item->index();

	(*m_elements)[i].setProX( chartType, item->x() / canvas()->width() );
	(*m_elements)[i].setProY( chartType, item->y() / canvas()->height() );

	canvas()->update();
    }
}


