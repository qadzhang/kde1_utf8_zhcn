#ifndef CANVASTEXT_H
#define CANVASTEXT_H

#include <ntqcanvas.h>

class TQFont;


class CanvasText : public TQCanvasText
{
public:
    enum { CANVAS_TEXT = 1100 };

    CanvasText( int index, TQCanvas *canvas )
	: TQCanvasText( canvas ), m_index( index ) {}
    CanvasText( int index, const TQString& text, TQCanvas *canvas )
	: TQCanvasText( text, canvas ), m_index( index ) {}
    CanvasText( int index, const TQString& text, TQFont font, TQCanvas *canvas )
	: TQCanvasText( text, font, canvas ), m_index( index ) {}

    int index() const { return m_index; }
    void setIndex( int index ) { m_index = index; }

    int rtti() const { return CANVAS_TEXT; }

private:
    int m_index;
};

#endif

