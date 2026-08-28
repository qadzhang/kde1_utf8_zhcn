/****************************************************************************
**
** Definition of TQSplitter class
**
**  Created : 980105
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

#ifndef TQSPLITTER_H
#define TQSPLITTER_H

#ifndef QT_H
#include "ntqframe.h"
#include "ntqvaluelist.h"
#endif // QT_H

#ifndef TQT_NO_SPLITTER

#define QT_QSPLITTER_DEFAULT 2

class TQSplitterHandle;
class TQSplitterPrivate;
class TQSplitterLayoutStruct;
class TQTextStream;

class TQSplitterPrivate
{
public:
    TQSplitterPrivate()
	: opaque( false ), firstShow( true ), childrenCollapsible( true ),
	  handleWidth( 0 ) { }

    TQPtrList<TQSplitterLayoutStruct> list;
    bool opaque;
    bool firstShow;
    bool childrenCollapsible;
    int handleWidth;
};

class TQ_EXPORT TQSplitterLayoutStruct : public TQt
{
public:
    TQCOORD sizer;
    uint isHandle;
    uint collapsible;
    uint resizeMode;
    TQWidget *wid;

    TQSplitterLayoutStruct()
	: sizer( -1 ), collapsible( QT_QSPLITTER_DEFAULT ) { }
    TQCOORD getSizer( Orientation orient );
};

class TQ_EXPORT TQSplitter : public TQFrame
{
    TQ_OBJECT
    TQ_PROPERTY( Orientation orientation READ orientation WRITE setOrientation )
    TQ_PROPERTY( bool opaqueResize READ opaqueResize WRITE setOpaqueResize )
    TQ_PROPERTY( int handleWidth READ handleWidth WRITE setHandleWidth )
    TQ_PROPERTY( bool childrenCollapsible READ childrenCollapsible WRITE setChildrenCollapsible )

public:
    // ### TQt 4.0: remove Auto from public API
    enum ResizeMode { Stretch, KeepSize, FollowSizeHint, Auto };

    TQSplitter( TQWidget* parent = 0, const char* name = 0 );
    TQSplitter( Orientation, TQWidget* parent = 0, const char* name = 0 );
    ~TQSplitter();

    virtual void setOrientation( Orientation );
    Orientation orientation() const { return orient; }

    // ### TQt 4.0: make setChildrenCollapsible() and setCollapsible() virtual

    void setChildrenCollapsible( bool );
    bool childrenCollapsible() const;

    void setCollapsible( TQWidget *w, bool );
    virtual void setResizeMode( TQWidget *w, ResizeMode );
    virtual void setOpaqueResize( bool = true );
    bool opaqueResize() const;

    void moveToFirst( TQWidget * );
    void moveToLast( TQWidget * );

    void refresh() { recalc( true ); }
    TQSize sizeHint() const;
    TQSize minimumSizeHint() const;

    TQValueList<int> sizes() const;
    void setSizes( TQValueList<int> );

    int handleWidth() const;
    void setHandleWidth( int );

protected:
    void childEvent( TQChildEvent * );

    bool event( TQEvent * );
    void resizeEvent( TQResizeEvent * );

    int idAfter( TQWidget* ) const;

    void moveSplitter( TQCOORD pos, int id );
    virtual void drawSplitter( TQPainter*, TQCOORD x, TQCOORD y,
			       TQCOORD w, TQCOORD h );
    void styleChange( TQStyle& );
    int adjustPos( int, int );
    virtual void setRubberband( int );
    void getRange( int id, int *, int * );

public: //private:
    enum { DefaultResizeMode = 3 };

    void init();
    void recalc( bool update = false );
    void doResize();
    void storeSizes();
    void getRange( int id, int *, int *, int *, int * );
    void addContribution( int, int *, int *, bool );
    int adjustPos( int, int, int *, int *, int *, int * );
    bool collapsible( TQSplitterLayoutStruct * );
    void processChildEvents();
    TQSplitterLayoutStruct *findWidget( TQWidget * );
    TQSplitterLayoutStruct *addWidget( TQWidget *, bool prepend = false );
    void recalcId();
    void doMove( bool backwards, int pos, int id, int delta, bool upLeft,
		 bool mayCollapse );
    void setGeo( TQWidget *w, int pos, int size, bool splitterMoved );
    int findWidgetJustBeforeOrJustAfter( int id, int delta, int &collapsibleSize );
    void updateHandles();

    inline TQCOORD pick( const TQPoint &p ) const
    { return orient == Horizontal ? p.x() : p.y(); }
    inline TQCOORD pick( const TQSize &s ) const
    { return orient == Horizontal ? s.width() : s.height(); }

    inline TQCOORD trans( const TQPoint &p ) const
    { return orient == Vertical ? p.x() : p.y(); }
    inline TQCOORD trans( const TQSize &s ) const
    { return orient == Vertical ? s.width() : s.height(); }

    TQSplitterPrivate *d;

    Orientation orient;
    friend class TQSplitterHandle;
    friend class TQTextStream;

private:
#if defined(TQ_DISABLE_COPY)
    TQSplitter( const TQSplitter & );
    TQSplitter& operator=( const TQSplitter & );
#endif
};

#ifndef TQT_NO_TEXTSTREAM
TQ_EXPORT TQTextStream& operator<<( TQTextStream&, const TQSplitter& );
TQ_EXPORT TQTextStream& operator>>( TQTextStream&, TQSplitter& );
#endif

class TQ_EXPORT TQSplitterHandle : public TQWidget
{
    TQ_OBJECT
public:
    TQSplitterHandle( Orientation o,
		     TQSplitter *parent, const char* name=0 );
    void setOrientation( Orientation o );
    Orientation orientation() const { return orient; }

    bool opaque() const { return s->opaqueResize(); }

    TQSize sizeHint() const;

    int id() const { return myId; } // d->list.at(id())->wid == this
    void setId( int i ) { myId = i; }

protected:
    void paintEvent( TQPaintEvent * );
    void mouseMoveEvent( TQMouseEvent * );
    void mousePressEvent( TQMouseEvent * );
    void mouseReleaseEvent( TQMouseEvent * );

public: // private:
    Orientation orient;
    bool opaq;
    int myId;

    TQSplitter *s;
};

#endif // TQT_NO_SPLITTER

#endif // TQSPLITTER_H
