/****************************************************************************
**
** Definition of the TQDockArea class
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

#ifndef TQDOCKAREA_H
#define TQDOCKAREA_H

#ifndef QT_H
#include "ntqwidget.h"
#include "ntqptrlist.h"
#include "ntqdockwindow.h"
#include "ntqlayout.h"
#include "ntqvaluelist.h"
#include "ntqguardedptr.h"
#include "ntqtextstream.h"
#endif // QT_H

#ifndef TQT_NO_MAINWINDOW

class TQSplitter;
class TQBoxLayout;
class TQDockAreaLayout;
class TQMouseEvent;
class TQDockWindowResizeHandle;
class TQDockAreaPrivate;

class TQ_EXPORT TQDockAreaLayout : public TQLayout
{
    TQ_OBJECT
    friend class TQDockArea;

public:
    TQDockAreaLayout( TQWidget* parent, TQt::Orientation o, TQPtrList<TQDockWindow> *wl, int space = -1, int margin = -1, const char *name = 0 )
	: TQLayout( parent, space, margin, name ), orient( o ), dockWindows( wl ), parentWidget( parent ) { init(); }
    ~TQDockAreaLayout() {}

    void addItem( TQLayoutItem * ) {}
    bool hasHeightForWidth() const;
    int heightForWidth( int ) const;
    int widthForHeight( int ) const;
    TQSize sizeHint() const;
    TQSize minimumSize() const;
    TQLayoutIterator iterator();
    TQSizePolicy::ExpandData expanding() const { return TQSizePolicy::NoDirection; }
    void invalidate();
    TQt::Orientation orientation() const { return orient; }
    TQValueList<TQRect> lineList() const { return lines; }
    TQPtrList<TQDockWindow> lineStarts() const { return ls; }

protected:
    void setGeometry( const TQRect& );

private:
    void init();
    int layoutItems( const TQRect&, bool testonly = false );
    TQt::Orientation orient;
    bool dirty;
    int cached_width, cached_height;
    int cached_hfw, cached_wfh;
    TQPtrList<TQDockWindow> *dockWindows;
    TQWidget *parentWidget;
    TQValueList<TQRect> lines;
    TQPtrList<TQDockWindow> ls;
#if defined(TQ_DISABLE_COPY) // Disabled copy constructor and operator=
    TQDockAreaLayout( const TQDockAreaLayout & );
    TQDockAreaLayout &operator=( const TQDockAreaLayout & );
#endif
};

class TQ_EXPORT TQDockArea : public TQWidget
{
    TQ_OBJECT
    TQ_ENUMS( HandlePosition )
    TQ_PROPERTY( Orientation orientation READ orientation )
    TQ_PROPERTY( int count READ count )
    TQ_PROPERTY( bool empty READ isEmpty )
    TQ_PROPERTY( HandlePosition handlePosition READ handlePosition )

    friend class TQDockWindow;
    friend class TQDockWindowResizeHandle;
    friend class TQDockAreaLayout;

public:
    enum HandlePosition { Normal, Reverse };

    TQDockArea( Orientation o, HandlePosition h = Normal, TQWidget* parent=0, const char* name=0 );
    ~TQDockArea();

    void moveDockWindow( TQDockWindow *w, const TQPoint &globalPos, const TQRect &rect, bool swap );
    void removeDockWindow( TQDockWindow *w, bool makeFloating, bool swap, bool fixNewLines = true );
    void moveDockWindow( TQDockWindow *w, int index = -1 );
    bool hasDockWindow( TQDockWindow *w, int *index = 0 );

    void invalidNextOffset( TQDockWindow *dw );

    Orientation orientation() const { return orient; }
    HandlePosition handlePosition() const { return hPos; }

    bool eventFilter( TQObject *, TQEvent * );
    bool isEmpty() const;
    int count() const;
    TQPtrList<TQDockWindow> dockWindowList() const;

    bool isDockWindowAccepted( TQDockWindow *dw );
    void setAcceptDockWindow( TQDockWindow *dw, bool accept );

public slots:
    void lineUp( bool keepNewLines );

private:
    struct DockWindowData
    {
	int index;
	int offset;
	int line;
	TQSize fixedExtent;
	TQGuardedPtr<TQDockArea> area;
    };

    int findDockWindow( TQDockWindow *w );
    int lineOf( int index );
    DockWindowData *dockWindowData( TQDockWindow *w );
    void dockWindow( TQDockWindow *dockWindow, DockWindowData *data );
    void updateLayout();
    void invalidateFixedSizes();
    int maxSpace( int hint, TQDockWindow *dw );
    void setFixedExtent( int d, TQDockWindow *dw );
    bool isLastDockWindow( TQDockWindow *dw );

private:
    Orientation orient;
    TQPtrList<TQDockWindow> *dockWindows;
    TQDockAreaLayout *layout;
    HandlePosition hPos;
    TQPtrList<TQDockWindow> forbiddenWidgets;
    TQDockAreaPrivate *d;

private:	// Disabled copy constructor and operator=
#if defined(TQ_DISABLE_COPY)
    TQDockArea( const TQDockArea & );
    TQDockArea& operator=( const TQDockArea & );
#endif

};

#ifndef TQT_NO_TEXTSTREAM
TQ_EXPORT TQTextStream &operator<<( TQTextStream &, const TQDockArea & );
TQ_EXPORT TQTextStream &operator>>( TQTextStream &, TQDockArea & );
#endif

#define Q_DEFINED_QDOCKAREA
#include "ntqwinexport.h"
#endif

#endif //TQT_NO_MAINWINDOW
