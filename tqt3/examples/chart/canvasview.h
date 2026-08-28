#ifndef CANVASVIEW_H
#define CANVASVIEW_H

#include "element.h"
#include "canvastext.h"

#include <ntqcanvas.h>


class TQPoint;


class CanvasView : public TQCanvasView
{
    TQ_OBJECT
public:
    CanvasView( TQCanvas *canvas, ElementVector *elements,
		TQWidget* parent = 0, const char* name = "canvas view",
		WFlags f = 0 )
	: TQCanvasView( canvas, parent, name, f ), m_movingItem(0),
	  m_elements( elements ) {}

protected:
    void viewportResizeEvent( TQResizeEvent *e );
    void contentsMousePressEvent( TQMouseEvent *e );
    void contentsMouseMoveEvent( TQMouseEvent *e );
    void contentsContextMenuEvent( TQContextMenuEvent *e );

private:
    TQCanvasItem *m_movingItem;
    TQPoint m_pos;
    ElementVector *m_elements;
};


#endif
