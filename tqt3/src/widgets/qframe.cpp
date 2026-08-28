/****************************************************************************
**
** Implementation of TQFrame widget class
**
** Created : 950201
**
** Copyright (C) 1992-2008 Trolltech ASA.  All rights reserved.
**
** This file is part of the widgets module of the TQt GUI Toolkit.
**
** This file may be used under the terms of the GNU General
** Public License versions 2.0 or 3.0 as published by the Free
** Software Foundation and appearing in the files LICENSE.GPL2
** and LICENSE.GPL3 included in the packaging of this file.
** Alternatively you may (at your option) use any later version
** of the GNU General Public License if such license has been
** publicly approved by Trolltech ASA (or its successors, if any)
** and the KDE Free TQt Foundation.
**
** Please review the following information to ensure GNU General
** Public Licensing requirements will be met:
** http://trolltech.com/products/qt/licenses/licensing/opensource/.
** If you are unsure which license is appropriate for your use, please
** review the following information:
** http://trolltech.com/products/qt/licenses/licensing/licensingoverview
** or contact the sales department at sales@trolltech.com.
**
** This file may be used under the terms of the Q Public License as
** defined by Trolltech ASA and appearing in the file LICENSE.TQPL
** included in the packaging of this file.  Licensees holding valid TQt
** Commercial licenses may use this file in accordance with the TQt
** Commercial License Agreement provided with the Software.
**
** This file is provided "AS IS" with NO WARRANTY OF ANY KIND,
** INCLUDING THE WARRANTIES OF DESIGN, MERCHANTABILITY AND FITNESS FOR
** A PARTICULAR PURPOSE. Trolltech reserves all rights not granted
** herein.
**
**********************************************************************/

#include "ntqframe.h"
#ifndef TQT_NO_FRAME
#include "ntqpainter.h"
#include "ntqdrawutil.h"
#include "ntqframe.h"
#include "ntqbitmap.h"
#include "ntqstyle.h"

/*!
    \class TQFrame
    \brief The TQFrame class is the base class of widgets that can have a frame.

    \ingroup abstractwidgets

    It draws a frame and calls a virtual function, drawContents(), to
    fill in the frame. This function is reimplemented by subclasses.
    There are also two other less useful functions: drawFrame() and
    frameChanged().

    TQPopupMenu uses this to "raise" the menu above the surrounding
    screen. TQProgressBar has a "sunken" look. TQLabel has a flat look.
    The frames of widgets like these can be changed.

    \code
    TQLabel label(...);
    label.setFrameStyle( TQFrame::Panel | TQFrame::Raised );
    label.setLineWidth( 2 );

    TQProgressBar pbar(...);
    label.setFrameStyle( TQFrame::NoFrame );
    \endcode

    The TQFrame class can also be used directly for creating simple
    frames without any contents, although usually you would use a
    TQHBox or TQVBox because they automatically lay out the widgets you
    put inside the frame.

    A frame widget has four attributes: frameStyle(), lineWidth(),
    midLineWidth(), and margin().

    The frame style is specified by a \link TQFrame::Shape frame
    shape\endlink and a \link TQFrame::Shadow shadow style\endlink. The
    frame shapes are \c NoFrame, \c Box, \c Panel, \c StyledPanel, \c
    PopupPanel, \c WinPanel, \c ToolBarPanel, \c MenuBarPanel, \c
    HLine and \c VLine; the shadow styles are \c Plain, \c Raised and
    \c Sunken.

    The line width is the width of the frame border.

    The mid-line width specifies the width of an extra line in the
    middle of the frame, which uses a third color to obtain a special
    3D effect. Notice that a mid-line is only drawn for \c Box, \c
    HLine and \c VLine frames that are raised or sunken.

    The margin is the gap between the frame and the contents of the
    frame.

    \target picture
    This table shows the most useful combinations of styles and widths
    (and some rather useless ones):

    \img frames.png Table of frame styles
*/


/*!
    \enum TQFrame::Shape

    This enum type defines the shapes of a TQFrame's frame.

    \value NoFrame  TQFrame draws nothing
    \value Box  TQFrame draws a box around its contents
    \value Panel  TQFrame draws a panel to make the contents appear
    raised or sunken
    \value StyledPanel  draws a rectangular panel with a look that
    depends on the current GUI style. It can be raised or sunken.
    \value HLine  TQFrame draws a horizontal line that frames nothing
    (useful as separator)
    \value VLine  TQFrame draws a vertical line that frames nothing
    (useful as separator)
    \value GroupBoxPanel draws a rectangular panel
    \value WinPanel draws a rectangular panel that can be raised or
    sunken like those in Windows 95. Specifying this shape sets
    the line width to 2 pixels. WinPanel is provided for compatibility.
    For GUI style independence we recommend using StyledPanel instead.
    \value ToolBarPanel
    \value MenuBarPanel
    \value PopupPanel
    \value LineEditPanel is used to draw a frame suitable for line edits. The
    look depends upon the current GUI style.
    \value TabWidgetPanel is used to draw a frame suitable for tab widgets. The
    look depends upon the current GUI style.
    \value MShape internal mask

    When it does not call TQStyle, Shape interacts with TQFrame::Shadow,
    the lineWidth() and the midLineWidth() to create the total result.
    See the \link #picture picture of the frames\endlink in the class
    description.

    \sa TQFrame::Shadow TQFrame::style() TQStyle::drawPrimitive()
*/


/*!
    \enum TQFrame::Shadow

    This enum type defines the 3D effect used for TQFrame's frame.

    \value Plain  the frame and contents appear level with the
    surroundings; draws using the palette foreground color (without
    any 3D effect)
    \value Raised the frame and contents appear raised; draws a 3D
    raised line using the light and dark colors of the current color
    group
    \value Sunken the frame and contents appear sunken; draws a 3D
    sunken line using the light and dark colors of the current color
    group
    \value MShadow internal; mask for the shadow

    Shadow interacts with TQFrame::Shape, the lineWidth() and the
    midLineWidth(). See the \link #picture picture of the frames\endlink
    in the class description.

    \sa TQFrame::Shape lineWidth() midLineWidth()
*/


/*!
    Constructs a frame widget with frame style \c NoFrame and a
    1-pixel frame width.

    The \a parent, \a name and \a f arguments are passed to the
    TQWidget constructor.
*/

TQFrame::TQFrame( TQWidget *parent, const char *name, WFlags f )
    : TQWidget( parent, name, f )
{
    frect  = TQRect( 0, 0, 0, 0 );
    fstyle = NoFrame | Plain;
    lwidth = 1;
    mwidth = 0;
    mlwidth = 0;
    updateFrameWidth();
}

static const int wpwidth = 2; // WinPanel lwidth

/*!
    \fn int TQFrame::frameStyle() const

    Returns the frame style.

    The default value is TQFrame::NoFrame.

    \sa setFrameStyle(), frameShape(), frameShadow()
*/

/*!
    \property TQFrame::frameShape
    \brief the frame shape value from the frame style

    \sa frameStyle(), frameShadow()
*/

/*!
    \property TQFrame::frameShadow
    \brief the frame shadow value from the frame style

    \sa frameStyle(), frameShape()
*/

/*!
    Sets the frame style to \a style.

    The \a style is the bitwise OR between a frame shape and a frame
    shadow style. See the \link #picture illustration\endlink in the
    class documentation.

    The frame shapes are given in \l{TQFrame::Shape} and the shadow
    styles in \l{TQFrame::Shadow}.

    If a mid-line width greater than 0 is specified, an additional
    line is drawn for \c Raised or \c Sunken \c Box, \c HLine, and \c
    VLine frames. The mid-color of the current color group is used for
    drawing middle lines.

    \sa \link #picture Illustration\endlink, frameStyle(),
    colorGroup(), TQColorGroup
*/

void TQFrame::setFrameStyle( int style )
{
    if ( !testWState( WState_OwnSizePolicy ) ) {
	switch ( style & MShape ) {
	case HLine:
	    setSizePolicy( TQSizePolicy::Minimum, TQSizePolicy::Fixed );
	    break;
	case VLine:
	    setSizePolicy( TQSizePolicy::Fixed, TQSizePolicy::Minimum );
	    break;
	default:
	    if ( (fstyle & MShape) == HLine || (fstyle & MShape) == VLine)
		setSizePolicy( TQSizePolicy::Preferred, TQSizePolicy::Preferred );
	}
	clearWState( WState_OwnSizePolicy );
    }
    fstyle = (short)style;
    updateFrameWidth( true );
}

/*!
    \property TQFrame::lineWidth
    \brief the line width

    Note that the \e total line width for \c HLine and \c VLine is
    given by frameWidth(), not lineWidth().

    The default value is 1.

    \sa midLineWidth(), frameWidth()
*/

void TQFrame::setLineWidth( int w )
{
    lwidth = (short)w;
    updateFrameWidth();
}

/*!
    \property TQFrame::midLineWidth
    \brief the width of the mid-line

    The default value is 0.

    \sa lineWidth(), frameWidth()
*/

void TQFrame::setMidLineWidth( int w )
{
    mlwidth = (short)w;
    updateFrameWidth();
}



/*!
    \property TQFrame::margin
    \brief the width of the margin

    The margin is the distance between the innermost pixel of the
    frame and the outermost pixel of contentsRect(). It is included in
    frameWidth().

    The margin is filled according to backgroundMode().

    The default value is 0.

    \sa setMargin(), lineWidth(), frameWidth()
*/

void TQFrame::setMargin( int w )
{
    mwidth = (short)w;
    updateFrameWidth();
}


/*!
  \internal
  Updated the fwidth parameter.
*/

void TQFrame::updateFrameWidth( bool resetLineMetrics )
{
    int frameType  = fstyle & MShape;
    int frameStyle = fstyle & MShadow;

    if ( resetLineMetrics ) {
	switch ( frameType ) {
	case MenuBarPanel:
	    mwidth = 0;
	    lwidth = style().pixelMetric( TQStyle::PM_MenuBarFrameWidth, this );
	    break;
	case ToolBarPanel:
	    mwidth = 0;
	    lwidth = style().pixelMetric( TQStyle::PM_DockWindowFrameWidth, this );
	    break;
	case LineEditPanel:
	case TabWidgetPanel:
	case PopupPanel:
	    mwidth = 0;
	    lwidth = style().pixelMetric( TQStyle::PM_DefaultFrameWidth, this );
	    break;
	}
    }

    fwidth = -1;

    switch ( frameType ) {

    case NoFrame:
	fwidth = 0;
	break;

    case Box:
	switch ( frameStyle ) {
	case Plain:
	    fwidth = lwidth;
	    break;
	case Raised:
	case Sunken:
	    fwidth = (short)(lwidth*2 + midLineWidth() );
	    break;
	}
	break;


    case LineEditPanel:
    case TabWidgetPanel:
    case PopupPanel:
    case GroupBoxPanel:
    case Panel:
    case StyledPanel:
	switch ( frameStyle ) {
	case Plain:
	case Raised:
	case Sunken:
	    fwidth = lwidth;
	    break;
	}
	break;

    case WinPanel:
	switch ( frameStyle ) {
	case Plain:
	case Raised:
	case Sunken:
	    fwidth =  wpwidth; //WinPanel does not use lwidth!
	    break;
	}
	break;
    case MenuBarPanel:
	fwidth = lwidth;
	break;
    case ToolBarPanel:
	fwidth = lwidth;
	break;
    case HLine:
    case VLine:
	switch ( frameStyle ) {
	case Plain:
	    fwidth = lwidth;
	    break;
	case Raised:
	case Sunken:
	    fwidth = (short)(lwidth*2 + midLineWidth());
	    break;
	}
	break;
    }

    if ( fwidth == -1 )				// invalid style
	fwidth = 0;

    fwidth += margin();

    frameChanged();
}


/*!
    \property TQFrame::frameWidth
    \brief the width of the frame that is drawn.

    Note that the frame width depends on the \link
    TQFrame::setFrameStyle() frame style \endlink, not only the line
    width and the mid-line width. For example, the style \c NoFrame
    always has a frame width of 0, whereas the style \c Panel has a
    frame width equivalent to the line width. The frame width also
    includes the margin.

    \sa lineWidth(), midLineWidth(), frameStyle(), margin()
*/

/*!
    \property TQFrame::frameRect
    \brief the frame rectangle

    The frame rectangle is the rectangle the frame is drawn in. By
    default, this is the entire widget. Setting this property does \e
    not cause a widget update.

    If this property is set to a null rectangle (for example
    \c{TQRect(0, 0, 0, 0)}), then the frame rectangle is equivalent to
    the \link TQWidget::rect() widget rectangle\endlink.

    \sa contentsRect()
*/

TQRect TQFrame::frameRect() const
{
    if ( frect.isNull() )
        return rect();
    else
        return frect;
}

void TQFrame::setFrameRect( const TQRect &r )
{
    frect = r.isValid() ? r : rect();
}


/*!
    \property TQFrame::contentsRect
    \brief the rectangle inside the frame

    \sa frameRect(), drawContents()
*/

TQRect TQFrame::contentsRect() const
{
    TQRect r = frameRect();
    int   w = frameWidth();                     // total width
    int frameType  = fstyle & MShape;
    if (frameType == PopupPanel) {
	int vExtra = style().pixelMetric(TQStyle::PM_PopupMenuFrameVerticalExtra, this);
	int hExtra = style().pixelMetric(TQStyle::PM_PopupMenuFrameHorizontalExtra, this);
	r.setRect( r.x()+w+hExtra, r.y()+w+vExtra, r.width()-w*2-hExtra*2, r.height()-w*2-vExtra*2 );
    } else {
	r.setRect( r.x()+w, r.y()+w, r.width()-w*2, r.height()-w*2 );
    }
    return r;
}

/*!\reimp
*/
TQSize TQFrame::sizeHint() const
{
    //   Returns a size hint for the frame - for HLine and VLine
    //   shapes, this is stretchable one way and 3 pixels wide the
    //   other.  For other shapes, TQWidget::sizeHint() is used.
    switch (fstyle & MShape) {
    case HLine:
        return TQSize(-1,3);
    case VLine:
        return TQSize(3,-1);
    default:
        return TQWidget::sizeHint();
    }
}

/*!
    Processes the paint event \a event.

    Paints the frame and the contents.

    Opens the painter on the frame and calls drawFrame(), then
    drawContents().
*/

void TQFrame::paintEvent( TQPaintEvent *event )
{
    const int m = margin();
    if ( m && testWFlags( WNoAutoErase ) ) {
	TQRect r = contentsRect();
	r.addCoords( -m, -m, m, m );
	erase( event->region().intersect( TQRegion( r ) - contentsRect() ) );
    }

    TQPainter paint( this );

    if ( !contentsRect().contains( event->rect() ) ) {
        paint.save();
	paint.setClipRegion( event->region().intersect(frameRect()) );
        drawFrame( &paint );
        paint.restore();
    }
    if ( event->rect().intersects( contentsRect() ) &&
         (fstyle & MShape) != HLine && (fstyle & MShape) != VLine ) {
        paint.setClipRegion( event->region().intersect( contentsRect() ) );
        drawContents( &paint );
    }
}


/*!
    Processes the resize event \a e.

    Adjusts the frame rectangle for the resized widget. The frame
    rectangle is elastic, and the surrounding area is static.

    The resulting frame rectangle may be null or invalid. You can use
    setMinimumSize() to avoid those possibilities.

    Nothing is done if the frame rectangle is a \link TQRect::isNull()
    null rectangle\endlink already.
*/

void TQFrame::resizeEvent( TQResizeEvent *e )
{
    if ( !frect.isNull() ) {
        TQRect r( frect.x(), frect.y(),
                 width()  - (e->oldSize().width()  - frect.width()),
                 height() - (e->oldSize().height() - frect.height()) );
        setFrameRect( r );
    }
    TQWidget::resizeEvent( e );
}


/*!
    Draws the frame using the painter \a p and the current frame
    attributes and color group. The rectangle inside the frame is not
    affected.

    This function is virtual, but in general you do not need to
    reimplement it. If you do, note that the TQPainter is already open
    and must remain open.

    \sa frameRect(), contentsRect(), drawContents(), frameStyle(), setPalette()
*/

void TQFrame::drawFrame( TQPainter *p )
{
    TQPoint      p1, p2;
    TQRect       r     = frameRect();
    int         type  = fstyle & MShape;
    int         cstyle = fstyle & MShadow;
#ifdef TQT_NO_DRAWUTIL
    p->setPen( black ); // ####
    p->drawRect( r ); //### a bit too simple
#else
    const TQColorGroup & g = colorGroup();

#ifndef TQT_NO_STYLE
    TQStyleOption opt(lineWidth(),midLineWidth());

    TQStyle::SFlags flags = TQStyle::Style_Default;
    if (isEnabled())
	flags |= TQStyle::Style_Enabled;
    if (cstyle == Sunken)
	flags |= TQStyle::Style_Sunken;
    else if (cstyle == Raised)
	flags |= TQStyle::Style_Raised;
    if (hasFocus())
	flags |= TQStyle::Style_HasFocus;
    if (hasMouse())
	flags |= TQStyle::Style_MouseOver;
#endif // TQT_NO_STYLE

    switch ( type ) {

    case Box:
        if ( cstyle == Plain )
            qDrawPlainRect( p, r, g.foreground(), lwidth );
        else
            qDrawShadeRect( p, r, g, cstyle == Sunken, lwidth,
                            midLineWidth() );
        break;

    case LineEditPanel:
	style().drawPrimitive( TQStyle::PE_PanelLineEdit, p, r, g, flags, opt );
	break;

    case GroupBoxPanel:
	style().drawPrimitive( TQStyle::PE_PanelGroupBox, p, r, g, flags, opt );
	break;

    case TabWidgetPanel:
	style().drawPrimitive( TQStyle::PE_PanelTabWidget, p, r, g, flags, opt );
	break;

    case MenuBarPanel:
#ifndef TQT_NO_STYLE
	style().drawPrimitive(TQStyle::PE_PanelMenuBar, p, r, g, flags, opt);
	break;
#endif // fall through to Panel if TQT_NO_STYLE

    case ToolBarPanel:
#ifndef TQT_NO_STYLE
	style().drawPrimitive( TQStyle::PE_PanelDockWindow, p, rect(), g, flags, opt);
        break;
#endif // fall through to Panel if TQT_NO_STYLE

    case StyledPanel:
#ifndef TQT_NO_STYLE
        if ( cstyle == Plain )
            qDrawPlainRect( p, r, g.foreground(), lwidth );
        else
	    style().drawPrimitive(TQStyle::PE_Panel, p, r, g, flags, opt);
        break;
#endif // fall through to Panel if TQT_NO_STYLE

    case PopupPanel:
#ifndef TQT_NO_STYLE
    {
	int vextra = style().pixelMetric(TQStyle::PM_PopupMenuFrameVerticalExtra, this),
	    hextra = style().pixelMetric(TQStyle::PM_PopupMenuFrameHorizontalExtra, this);
	if(vextra > 0 || hextra > 0) {
	    TQRect fr = frameRect();
	    int   fw = frameWidth();
	    if(vextra > 0) {
		style().drawControl(TQStyle::CE_PopupMenuVerticalExtra, p, this,
				    TQRect(fr.x() + fw, fr.y() + fw, fr.width() - (fw*2), vextra),
				    g, flags, opt);
		style().drawControl(TQStyle::CE_PopupMenuVerticalExtra, p, this,
				    TQRect(fr.x() + fw, fr.bottom() - fw - vextra, fr.width() - (fw*2), vextra),
				    g, flags, opt);
	    }
	    if(hextra > 0) {
		style().drawControl(TQStyle::CE_PopupMenuHorizontalExtra, p, this,
				    TQRect(fr.x() + fw, fr.y() + fw + vextra, hextra, fr.height() - (fw*2) - vextra),
				    g, flags, opt);
		style().drawControl(TQStyle::CE_PopupMenuHorizontalExtra, p, this,
				    TQRect(fr.right() - fw - hextra, fr.y() + fw + vextra, hextra, fr.height() - (fw*2) - vextra),
				    g, flags, opt);
	    }
	}

        if ( cstyle == Plain )
            qDrawPlainRect( p, r, g.foreground(), lwidth );
        else
	    style().drawPrimitive(TQStyle::PE_PanelPopup, p, r, g, flags, opt);
        break;
    }
#endif // fall through to Panel if TQT_NO_STYLE

    case Panel:
        if ( cstyle == Plain )
            qDrawPlainRect( p, r, g.foreground(), lwidth );
        else
            qDrawShadePanel( p, r, g, cstyle == Sunken, lwidth );
        break;

    case WinPanel:
        if ( cstyle == Plain )
            qDrawPlainRect( p, r, g.foreground(), wpwidth );
        else
            qDrawWinPanel( p, r, g, cstyle == Sunken );
        break;
    case HLine:
    case VLine:
        if ( type == HLine ) {
            p1 = TQPoint( r.x(), r.height()/2 );
            p2 = TQPoint( r.x()+r.width(), p1.y() );
        }
        else {
            p1 = TQPoint( r.x()+r.width()/2, 0 );
            p2 = TQPoint( p1.x(), r.height() );
        }
        if ( cstyle == Plain ) {
            TQPen oldPen = p->pen();
            p->setPen( TQPen(g.foreground(),lwidth) );
            p->drawLine( p1, p2 );
            p->setPen( oldPen );
        }
        else
            qDrawShadeLine( p, p1, p2, g, cstyle == Sunken,
                            lwidth, midLineWidth() );
        break;
    }
#endif // TQT_NO_DRAWUTIL
}


/*!
    Virtual function that draws the contents of the frame.

    The TQPainter is already open when you get it, and you must leave
    it open. Painter \link TQPainter::setWorldMatrix()
    transformations\endlink are switched off on entry. If you
    transform the painter, remember to take the frame into account and
    \link TQPainter::resetXForm() reset transformation\endlink before
    returning.

    This function is reimplemented by subclasses that draw something
    inside the frame. It should only draw inside contentsRect(). The
    default function does nothing.

    \sa contentsRect(), TQPainter::setClipRect()
*/

void TQFrame::drawContents( TQPainter * )
{
}


/*!
    Virtual function that is called when the frame style, line width
    or mid-line width changes.

    This function can be reimplemented by subclasses that need to know
    when the frame attributes change.

    The default implementation calls update().
*/

void TQFrame::frameChanged()
{
    update();
    updateGeometry();
}

/*!\reimp
 */
void  TQFrame::styleChange( TQStyle& old )
{
    updateFrameWidth( true );
    TQWidget::styleChange( old );
}

#endif //TQT_NO_FRAME
