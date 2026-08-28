/****************************************************************************
**
** Definition of the TQDockWindow class
**
** Created : 001010
**
** Copyright (C) 1992-2008 Trolltech ASA.  All rights reserved.
**
** This file is part of the workspace module of the TQt GUI Toolkit.
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

#ifndef TQDOCKWINDOW_H
#define TQDOCKWINDOW_H

#ifndef QT_H
#include "ntqframe.h"
#endif // QT_H

#ifndef TQT_NO_MAINWINDOW

class TQDockWindowHandle;
class TQDockWindowTitleBar;
class TQPainter;
class TQDockWindowResizeHandle;
class TQBoxLayout;
class TQHBoxLayout;
class TQVBoxLayout;
class TQDockArea;
class TQWidgetResizeHandler;
class TQMainWindow;
class TQDockAreaLayout;
class TQDockWindowPrivate;
class TQToolBar;
class TQWindowsXPStyle;

class TQ_EXPORT TQDockWindow : public TQFrame
{
    TQ_OBJECT
    TQ_ENUMS( CloseMode Place )
    TQ_PROPERTY( int closeMode READ closeMode  WRITE setCloseMode ) //### this shouldn't be of type int?!
    TQ_PROPERTY( bool resizeEnabled READ isResizeEnabled  WRITE setResizeEnabled )
    TQ_PROPERTY( bool movingEnabled READ isMovingEnabled  WRITE setMovingEnabled )
    TQ_PROPERTY( bool horizontallyStretchable READ isHorizontallyStretchable  WRITE setHorizontallyStretchable )
    TQ_PROPERTY( bool verticallyStretchable READ isVerticallyStretchable  WRITE setVerticallyStretchable )
    TQ_PROPERTY( bool stretchable READ isStretchable )
    TQ_PROPERTY( bool newLine READ newLine  WRITE setNewLine )
    TQ_PROPERTY( bool opaqueMoving READ opaqueMoving  WRITE setOpaqueMoving )
    TQ_PROPERTY( int offset READ offset  WRITE setOffset )
    TQ_PROPERTY( Place place READ place )

    friend class TQDockWindowHandle;
    friend class TQDockWindowTitleBar;
    friend class TQDockArea;
    friend class TQDockAreaLayout;
    friend class TQMainWindow;
    friend class TQCEMainWindow;
    friend class TQToolBar;
    friend class TQWindowsXPStyle;

public:
    enum Place { InDock, OutsideDock };
    enum CloseMode { Never = 0, Docked = 1, Undocked = 2, Always = Docked | Undocked };

    TQDockWindow( Place p = InDock, TQWidget* parent=0, const char* name=0, WFlags f = 0 );
    TQDockWindow( TQWidget* parent, const char* name=0, WFlags f = 0 );
    ~TQDockWindow();

    virtual void setWidget( TQWidget *w );
    TQWidget *widget() const;

    Place place() const { return curPlace; }

    TQDockArea *area() const;

    virtual void setCloseMode( int m );
    bool isCloseEnabled() const;
    int closeMode() const;

    virtual void setResizeEnabled( bool b );
    virtual void setMovingEnabled( bool b );
    bool isResizeEnabled() const;
    bool isMovingEnabled() const;

    virtual void setHorizontallyStretchable( bool b );
    virtual void setVerticallyStretchable( bool b );
    bool isHorizontallyStretchable() const;
    bool isVerticallyStretchable() const;
    void setHorizontalStretchable( bool b ) { setHorizontallyStretchable( b ); }
    void setVerticalStretchable( bool b ) { setVerticallyStretchable( b ); }
    bool isHorizontalStretchable() const { return isHorizontallyStretchable(); }
    bool isVerticalStretchable() const { return isVerticallyStretchable(); }
    bool isStretchable() const;

    virtual void setOffset( int o );
    int offset() const;

    virtual void setFixedExtentWidth( int w );
    virtual void setFixedExtentHeight( int h );
    TQSize fixedExtent() const;

    virtual void setNewLine( bool b );
    bool newLine() const;

    TQt::Orientation orientation() const;

    TQSize sizeHint() const;
    TQSize minimumSize() const;
    TQSize minimumSizeHint() const;

    TQBoxLayout *boxLayout();

    virtual void setOpaqueMoving( bool b );
    bool opaqueMoving() const;

    bool eventFilter( TQObject *o, TQEvent *e );

#ifdef TQT_NO_WIDGET_TOPEXTRA
    TQString caption() const;
#endif

signals:
    void orientationChanged( Orientation o );
    void placeChanged( TQDockWindow::Place p );
    void visibilityChanged( bool );

public slots:
    virtual void undock( TQWidget *w );
    virtual void undock() { undock( 0 ); }
    virtual void dock();
    virtual void setOrientation( Orientation o );
    void setCaption( const TQString &s );

protected:
    void resizeEvent( TQResizeEvent *e );
    void showEvent( TQShowEvent *e );
    void hideEvent( TQHideEvent *e );
    void contextMenuEvent( TQContextMenuEvent *e );

    void drawFrame( TQPainter * );
    void drawContents( TQPainter * );

    bool event( TQEvent *e );

private slots:
    void toggleVisible() { if ( !isVisible() ) show(); else hide(); }

private:
    TQDockWindow( Place p, TQWidget* parent, const char* name, WFlags f, bool toolbar );

    void handleMove( const TQPoint &pos, const TQPoint &gp, bool drawRect );
    void updateGui();
    void updateSplitterVisibility( bool visible );

    void startRectDraw( const TQPoint &so, bool drawRect );
    void endRectDraw( bool drawRect );
    void updatePosition( const TQPoint &globalPos  );
    TQWidget *areaAt( const TQPoint &gp );
    void removeFromDock( bool fixNewLines = true );
    void swapRect( TQRect &r, TQt::Orientation o, const TQPoint &offset, TQDockArea *area );
    void init();

private:
    TQDockWindowHandle *horHandle, *verHandle;
    TQDockWindowTitleBar *titleBar;
    TQWidget *wid;
    TQPainter *unclippedPainter;
    TQDockArea *dockArea, *tmpDockArea;
    TQRect currRect;
    Place curPlace;
    Place state;
    bool resizeEnabled : 1;
    bool moveEnabled : 1;
    bool nl : 1;
    bool opaque : 1;
    bool isToolbar : 1;
    bool stretchable[ 3 ];
    Orientation startOrientation;
    int cMode;
    TQPoint startOffset;
    int offs;
    TQSize fExtent;
    TQDockWindowResizeHandle *hHandleTop, *hHandleBottom, *vHandleLeft, *vHandleRight;
    TQVBoxLayout *hbox;
    TQHBoxLayout *vbox;
    TQBoxLayout *childBox;
    void *dockWindowData;
    TQPoint lastPos;
    TQSize lastSize;
    TQWidgetResizeHandler *widgetResizeHandler;
    TQDockWindowPrivate *d;

private:	// Disabled copy constructor and operator=
#if defined(TQ_DISABLE_COPY)
    TQDockWindow( const TQDockWindow & );
    TQDockWindow& operator=( const TQDockWindow & );
#endif
};

inline TQDockArea *TQDockWindow::area() const
{
    return dockArea;
}

#define Q_DEFINED_QDOCKWINDOW
#include "ntqwinexport.h"
#endif

#endif // TQDOCKWINDOW_H
