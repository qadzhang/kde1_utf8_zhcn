/****************************************************************************
**
** Definition of TQScrollView class
**
** Created : 970523
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
#ifndef TQSCROLLVIEW_H
#define TQSCROLLVIEW_H

#ifndef QT_H
#include "ntqframe.h"
#include "ntqscrollbar.h"
#endif // QT_H

#ifndef TQT_NO_SCROLLVIEW

class TQScrollViewData;

class TQ_EXPORT TQScrollView : public TQFrame
{
    TQ_OBJECT
    TQ_ENUMS( ResizePolicy ScrollBarMode )
    TQ_PROPERTY( ResizePolicy resizePolicy READ resizePolicy WRITE setResizePolicy )
    TQ_PROPERTY( ScrollBarMode vScrollBarMode READ vScrollBarMode WRITE setVScrollBarMode )
    TQ_PROPERTY( ScrollBarMode hScrollBarMode READ hScrollBarMode WRITE setHScrollBarMode )
    TQ_PROPERTY( int visibleWidth READ visibleWidth )
    TQ_PROPERTY( int visibleHeight READ visibleHeight )
    TQ_PROPERTY( int contentsWidth READ contentsWidth )
    TQ_PROPERTY( int contentsHeight READ contentsHeight )
    TQ_PROPERTY( int contentsX READ contentsX )
    TQ_PROPERTY( int contentsY READ contentsY )
#ifndef TQT_NO_DRAGANDDROP
    TQ_PROPERTY( bool dragAutoScroll READ dragAutoScroll WRITE setDragAutoScroll )
#endif

public:
    TQScrollView(TQWidget* parent=0, const char* name=0, WFlags f=0);
    ~TQScrollView();

    enum ResizePolicy { Default, Manual, AutoOne, AutoOneFit };
    virtual void setResizePolicy( ResizePolicy );
    ResizePolicy resizePolicy() const;

    void styleChange( TQStyle & );
    void removeChild(TQWidget* child);
    virtual void addChild( TQWidget* child, int x=0, int y=0 );
    virtual void moveChild( TQWidget* child, int x, int y );
    int childX(TQWidget* child);
    int childY(TQWidget* child);
    bool childIsVisible(TQWidget* child) { return child->isVisible(); } // obsolete functions
    void showChild(TQWidget* child, bool yes=true) {
	if ( yes )
	    child->show();
	else
	    child->hide();
    }

    enum ScrollBarMode { Auto, AlwaysOff, AlwaysOn };

    ScrollBarMode vScrollBarMode() const;
    virtual void  setVScrollBarMode( ScrollBarMode );

    ScrollBarMode hScrollBarMode() const;
    virtual void  setHScrollBarMode( ScrollBarMode );

    TQWidget*     cornerWidget() const;
    virtual void setCornerWidget(TQWidget*);

    // ### 4.0: Consider providing a factory function for scrollbars
    //          (e.g. make the two following functions virtual)
    TQScrollBar*  horizontalScrollBar() const;
    TQScrollBar*  verticalScrollBar() const;
    TQWidget*	 viewport() const;
    TQWidget*	 clipper() const;

    int		visibleWidth() const;
    int		visibleHeight() const;

    int		contentsWidth() const;
    int		contentsHeight() const;
    int		contentsX() const;
    int		contentsY() const;

    void	resize( int w, int h );
    void	resize( const TQSize& );
    void	show();

    void	updateContents( int x, int y, int w, int h );
    void	updateContents( const TQRect& r );
    void 	updateContents();
    void	repaintContents( int x, int y, int w, int h, bool erase=true );
    void	repaintContents( const TQRect& r, bool erase=true );
    void 	repaintContents( bool erase=true );
    void	contentsToViewport( int x, int y, int& vx, int& vy ) const;
    void	viewportToContents( int vx, int vy, int& x, int& y ) const;
    TQPoint	contentsToViewport( const TQPoint& ) const;
    TQPoint	viewportToContents( const TQPoint& ) const;
    void	enableClipper( bool y );

    void	setStaticBackground( bool y );
    bool	hasStaticBackground() const;

    TQSize	viewportSize( int, int ) const;
    TQSize	sizeHint() const;
    TQSize	minimumSizeHint() const;

    void	removeChild(TQObject* child);

    bool	isHorizontalSliderPressed();
    bool	isVerticalSliderPressed();

#ifndef TQT_NO_DRAGANDDROP
    virtual void setDragAutoScroll( bool b );
    bool	 dragAutoScroll() const;
#endif

signals:
    void	contentsMoving(int x, int y);
    void	horizontalSliderPressed();
    void	horizontalSliderReleased();
    void	verticalSliderPressed();
    void	verticalSliderReleased();

public slots:
    virtual void resizeContents( int w, int h );
    void	scrollBy( int dx, int dy );
    virtual void setContentsPos( int x, int y );
    void	ensureVisible(int x, int y);
    void	ensureVisible(int x, int y, int xmargin, int ymargin);
    void	center(int x, int y);
    void	center(int x, int y, float xmargin, float ymargin);

    void	updateScrollBars(); // ### virtual in 4.0
    void	setEnabled( bool enable );

protected:
    virtual void drawContents(TQPainter*, int cx, int cy, int cw, int ch);
    virtual void drawContentsOffset(TQPainter*, int ox, int oy,
		    int cx, int cy, int cw, int ch);


    virtual void contentsMousePressEvent( TQMouseEvent* );
    virtual void contentsMouseReleaseEvent( TQMouseEvent* );
    virtual void contentsMouseDoubleClickEvent( TQMouseEvent* );
    virtual void contentsMouseMoveEvent( TQMouseEvent* );
#ifndef TQT_NO_DRAGANDDROP
    virtual void contentsDragEnterEvent( TQDragEnterEvent * );
    virtual void contentsDragMoveEvent( TQDragMoveEvent * );
    virtual void contentsDragLeaveEvent( TQDragLeaveEvent * );
    virtual void contentsDropEvent( TQDropEvent * );
#endif
#ifndef TQT_NO_WHEELEVENT
    virtual void contentsWheelEvent( TQWheelEvent * );
#endif
    virtual void contentsContextMenuEvent( TQContextMenuEvent * );


    virtual void viewportPaintEvent( TQPaintEvent* );
    virtual void viewportResizeEvent( TQResizeEvent* );
    virtual void viewportMousePressEvent( TQMouseEvent* );
    virtual void viewportMouseReleaseEvent( TQMouseEvent* );
    virtual void viewportMouseDoubleClickEvent( TQMouseEvent* );
    virtual void viewportMouseMoveEvent( TQMouseEvent* );
#ifndef TQT_NO_DRAGANDDROP
    virtual void viewportDragEnterEvent( TQDragEnterEvent * );
    virtual void viewportDragMoveEvent( TQDragMoveEvent * );
    virtual void viewportDragLeaveEvent( TQDragLeaveEvent * );
    virtual void viewportDropEvent( TQDropEvent * );
#endif
#ifndef TQT_NO_WHEELEVENT
    virtual void viewportWheelEvent( TQWheelEvent * );
#endif
    virtual void viewportContextMenuEvent( TQContextMenuEvent * );

    void	frameChanged();

    virtual void setMargins(int left, int top, int right, int bottom);
    int leftMargin() const;
    int topMargin() const;
    int rightMargin() const;
    int bottomMargin() const;

    bool focusNextPrevChild( bool next );

    virtual void setHBarGeometry(TQScrollBar& hbar, int x, int y, int w, int h);
    virtual void setVBarGeometry(TQScrollBar& vbar, int x, int y, int w, int h);

    void resizeEvent(TQResizeEvent*);
    void  mousePressEvent( TQMouseEvent * );
    void  mouseReleaseEvent( TQMouseEvent * );
    void  mouseDoubleClickEvent( TQMouseEvent * );
    void  mouseMoveEvent( TQMouseEvent * );
#ifndef TQT_NO_WHEELEVENT
    void  wheelEvent( TQWheelEvent * );
#endif
    void contextMenuEvent( TQContextMenuEvent * );
    bool eventFilter( TQObject *, TQEvent *e );

    void setCachedSizeHint( const TQSize &sh ) const;
    TQSize cachedSizeHint() const;
    void fontChange( const TQFont & );

private:
    void drawContents( TQPainter* );
    void moveContents(int x, int y);

    TQScrollViewData* d;

private slots:
    void hslide(int);
    void vslide(int);
    void hbarIsPressed();
    void hbarIsReleased();
    void vbarIsPressed();
    void vbarIsReleased();
#ifndef TQT_NO_DRAGANDDROP
    void doDragAutoScroll();
    void startDragAutoScroll();
    void stopDragAutoScroll();
#endif

private: // Disabled copy constructor and operator=
#if defined(TQ_DISABLE_COPY)
    TQScrollView( const TQScrollView & );
    TQScrollView &operator=( const TQScrollView & );
#endif
    void changeFrameRect(const TQRect&);

public:
    void disableSizeHintCaching();

};

#endif // TQT_NO_SCROLLVIEW

#endif // TQSCROLLVIEW_H
