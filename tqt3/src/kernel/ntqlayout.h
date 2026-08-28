/****************************************************************************
**
** Definition of layout classes
**
** Created : 960416
**
** Copyright (C) 1992-2008 Trolltech ASA.  All rights reserved.
**
** This file is part of the kernel module of the TQt GUI Toolkit.
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

#ifndef TQLAYOUT_H
#define TQLAYOUT_H

#ifndef QT_H
#include "ntqobject.h"
#include "ntqsizepolicy.h"
#include "ntqwidget.h"
#endif // QT_H

#include <limits.h>

#ifndef TQT_NO_LAYOUT

#if 0
TQ_OBJECT
#endif

static const int TQLAYOUTSIZE_MAX = INT_MAX/256/16;

class TQGridLayoutBox;
class TQGridLayoutData;
class TQLayout;
class TQLayoutItem;
struct TQLayoutData;
class TQMenuBar;
class TQSpacerItem;
class TQWidget;

class TQ_EXPORT TQGLayoutIterator : public TQShared
{
public:
    virtual ~TQGLayoutIterator();
    virtual TQLayoutItem *next() = 0;
    virtual TQLayoutItem *current() = 0;
    virtual TQLayoutItem *takeCurrent() = 0;
};

class TQ_EXPORT TQLayoutIterator
{
public:
    TQLayoutIterator( TQGLayoutIterator *i ) : it( i ) { }
    TQLayoutIterator( const TQLayoutIterator &i ) : it( i.it ) {
	if ( it )
	    it->ref();
    }
    ~TQLayoutIterator() { if ( it && it->deref() ) delete it; }
    TQLayoutIterator &operator=( const TQLayoutIterator &i ) {
	if ( i.it )
	    i.it->ref();
	if ( it && it->deref() )
	    delete it;
	it = i.it;
	return *this;
    }
    TQLayoutItem *operator++() { return it ? it->next() : 0; }
    TQLayoutItem *current() { return it ? it->current() : 0; }
    TQLayoutItem *takeCurrent() { return it ? it->takeCurrent() : 0; }
    void deleteCurrent();

private:
    TQGLayoutIterator *it;
};

class TQ_EXPORT TQLayoutItem
{
public:
    TQLayoutItem( int alignment = 0 ) : align( alignment ) { }
    virtual ~TQLayoutItem();
    virtual TQSize sizeHint() const = 0;
    virtual TQSize minimumSize() const = 0;
    virtual TQSize maximumSize() const = 0;
    virtual TQSizePolicy::ExpandData expanding() const = 0;
    virtual void setGeometry( const TQRect& ) = 0;
    virtual TQRect geometry() const = 0;
    virtual bool isEmpty() const = 0;
    virtual bool hasHeightForWidth() const;
    virtual int heightForWidth( int ) const;
    // ### add minimumHeightForWidth( int ) in TQt 4.0
    virtual void invalidate();

    virtual TQWidget *widget();
    virtual TQLayoutIterator iterator();
    virtual TQLayout *layout();
    virtual TQSpacerItem *spacerItem();

    int alignment() const { return align; }
    virtual void setAlignment( int a );

protected:
    int align;
};

class TQ_EXPORT TQSpacerItem : public TQLayoutItem
{
public:
    TQSpacerItem( int w, int h,
		 TQSizePolicy::SizeType hData = TQSizePolicy::Minimum,
		 TQSizePolicy::SizeType vData = TQSizePolicy::Minimum )
	: width( w ), height( h ), sizeP( hData, vData ) { }
    void changeSize( int w, int h,
		     TQSizePolicy::SizeType hData = TQSizePolicy::Minimum,
		     TQSizePolicy::SizeType vData = TQSizePolicy::Minimum );
    TQSize sizeHint() const;
    TQSize minimumSize() const;
    TQSize maximumSize() const;
    TQSizePolicy::ExpandData expanding() const;
    bool isEmpty() const;
    void setGeometry( const TQRect& );
    TQRect geometry() const;
    TQSpacerItem *spacerItem();

private:
    int width;
    int height;
    TQSizePolicy sizeP;
    TQRect rect;
};

class TQ_EXPORT TQWidgetItem : public TQLayoutItem
{
public:
    TQWidgetItem( TQWidget *w ) : wid( w ) { }
    TQSize sizeHint() const;
    TQSize minimumSize() const;
    TQSize maximumSize() const;
    TQSizePolicy::ExpandData expanding() const;
    bool isEmpty() const;
    void setGeometry( const TQRect& );
    TQRect geometry() const;
    virtual TQWidget *widget();

    bool hasHeightForWidth() const;
    int heightForWidth( int ) const;

private:
    TQWidget *wid;
};

class TQ_EXPORT TQLayout : public TQObject, public TQLayoutItem
{
    TQ_OBJECT
    TQ_ENUMS( ResizeMode )
    TQ_PROPERTY( int margin READ margin WRITE setMargin )
    TQ_PROPERTY( int spacing READ spacing WRITE setSpacing )
    TQ_PROPERTY( ResizeMode resizeMode READ resizeMode WRITE setResizeMode )

public:
    // ### TQt 4.0: put 'Auto' first in enum
    enum ResizeMode { FreeResize, Minimum, Fixed, Auto };

    TQLayout( TQWidget *parent, int margin = 0, int spacing = -1,
	     const char *name = 0 );
    TQLayout( TQLayout *parentLayout, int spacing = -1, const char *name = 0 );
    TQLayout( int spacing = -1, const char *name = 0 );
    ~TQLayout();

    int margin() const { return outsideBorder; }
    int spacing() const { return insideSpacing; }

    virtual void setMargin( int );
    virtual void setSpacing( int );

    int defaultBorder() const { return insideSpacing; }
    void freeze( int w, int h );
    void freeze() { setResizeMode( Fixed ); }

    void setResizeMode( ResizeMode );
    ResizeMode resizeMode() const;

#ifndef TQT_NO_MENUBAR
    virtual void setMenuBar( TQMenuBar *w );
    TQMenuBar *menuBar() const { return menubar; }
#endif

    TQWidget *mainWidget();
    bool isTopLevel() const { return topLevel; }

    virtual void setAutoAdd( bool );
    bool autoAdd() const { return autoNewChild; }

    void invalidate();
    TQRect geometry() const;
    bool activate();

    void add( TQWidget *w ) { addItem( new TQWidgetItem(w) ); }
    virtual void addItem( TQLayoutItem * ) = 0;

    void remove( TQWidget *w );
    void removeItem( TQLayoutItem * );

    TQSizePolicy::ExpandData expanding() const;
    TQSize minimumSize() const;
    TQSize maximumSize() const;
    void setGeometry( const TQRect& ) = 0;
    TQLayoutIterator iterator() = 0;
    bool isEmpty() const;

    int totalHeightForWidth( int w ) const;
    TQSize totalMinimumSize() const;
    TQSize totalMaximumSize() const;
    TQSize totalSizeHint() const;
    TQLayout *layout();

    bool supportsMargin() const { return marginImpl; }

    void setEnabled( bool );
    bool isEnabled() const;

protected:
    bool eventFilter( TQObject *, TQEvent * );
    void childEvent( TQChildEvent *e );
    void addChildLayout( TQLayout *l );
    void deleteAllItems();

    void setSupportsMargin( bool );
    TQRect alignmentRect( const TQRect& ) const;

private:
    void setWidgetLayout( TQWidget *, TQLayout * );
    void init();
    int insideSpacing;
    int outsideBorder;
    uint topLevel : 1;
    uint enabled : 1;
    uint autoNewChild : 1;
    uint frozen : 1;
    uint activated : 1;
    uint marginImpl : 1;
    uint autoMinimum : 1;
    uint autoResizeMode : 1;
    TQRect rect;
    TQLayoutData *extraData;
#ifndef TQT_NO_MENUBAR
    TQMenuBar *menubar;
#endif

private:
#if defined(TQ_DISABLE_COPY)
    TQLayout( const TQLayout & );
    TQLayout &operator=( const TQLayout & );
#endif

    static void propagateSpacing( TQLayout *layout );
};

inline void TQLayoutIterator::deleteCurrent()
{
    delete takeCurrent();
}

class TQ_EXPORT TQGridLayout : public TQLayout
{
    TQ_OBJECT
public:
    TQGridLayout( TQWidget *parent, int nRows = 1, int nCols = 1, int border = 0,
		 int spacing = -1, const char *name = 0 );
    TQGridLayout( int nRows = 1, int nCols = 1, int spacing = -1,
		 const char *name = 0 );
    TQGridLayout( TQLayout *parentLayout, int nRows = 1, int nCols = 1,
		 int spacing = -1, const char *name = 0 );
    ~TQGridLayout();

    TQSize sizeHint() const;
    TQSize minimumSize() const;
    TQSize maximumSize() const;

    // ### remove 'virtual' in 4.0 (or add 'virtual' to set{Row,Col}Spacing())
    virtual void setRowStretch( int row, int stretch );
    virtual void setColStretch( int col, int stretch );
    int rowStretch( int row ) const;
    int colStretch( int col ) const;

    void setRowSpacing( int row, int minSize );
    void setColSpacing( int col, int minSize );
    int rowSpacing( int row ) const;
    int colSpacing( int col ) const;

    int numRows() const;
    int numCols() const;
    TQRect cellGeometry( int row, int col ) const;

    bool hasHeightForWidth() const;
    int heightForWidth( int ) const;
    int minimumHeightForWidth( int ) const;

    TQSizePolicy::ExpandData expanding() const;
    void invalidate();

    void addItem( TQLayoutItem * );
    void addItem( TQLayoutItem *item, int row, int col );
    void addMultiCell( TQLayoutItem *, int fromRow, int toRow,
			       int fromCol, int toCol, int align = 0 );

    void addWidget( TQWidget *, int row, int col, int align = 0 );
    void addMultiCellWidget( TQWidget *, int fromRow, int toRow,
			     int fromCol, int toCol, int align = 0 );
    void addLayout( TQLayout *layout, int row, int col);
    void addMultiCellLayout( TQLayout *layout, int fromRow, int toRow,
			     int fromCol, int toCol, int align = 0 );
    void addRowSpacing( int row, int minsize );
    void addColSpacing( int col, int minsize );

    void expand( int rows, int cols );

    enum Corner { TopLeft, TopRight, BottomLeft, BottomRight };
    void setOrigin( Corner );
    Corner origin() const;
    TQLayoutIterator iterator();
    void setGeometry( const TQRect& );

protected:
    bool findWidget( TQWidget* w, int *r, int *c );
    void add( TQLayoutItem*, int row, int col );

private:
#if defined(TQ_DISABLE_COPY)
    TQGridLayout( const TQGridLayout & );
    TQGridLayout &operator=( const TQGridLayout & );
#endif

    void init( int rows, int cols );
    TQGridLayoutData *data;
};

class TQBoxLayoutData;
class TQDockWindow;

class TQ_EXPORT TQBoxLayout : public TQLayout
{
    TQ_OBJECT
public:
    enum Direction { LeftToRight, RightToLeft, TopToBottom, BottomToTop,
		     Down = TopToBottom, Up = BottomToTop };

    TQBoxLayout( TQWidget *parent, Direction, int border = 0, int spacing = -1,
		const char *name = 0 );
    TQBoxLayout( TQLayout *parentLayout, Direction, int spacing = -1,
		const char *name = 0 );
    TQBoxLayout( Direction, int spacing = -1, const char *name = 0 );
    ~TQBoxLayout();

    void addItem( TQLayoutItem * );

    Direction direction() const { return dir; }
    void setDirection( Direction );

    void addSpacing( int size );
    void addStretch( int stretch = 0 );
    void addWidget( TQWidget *, int stretch = 0, int alignment = 0 );
    void addLayout( TQLayout *layout, int stretch = 0 );
    void addStrut( int );

    void insertSpacing( int index, int size );
    void insertStretch( int index, int stretch = 0 );
    void insertWidget( int index, TQWidget *widget, int stretch = 0,
		       int alignment = 0 );
    void insertLayout( int index, TQLayout *layout, int stretch = 0 );

    bool setStretchFactor( TQWidget*, int stretch );
    bool setStretchFactor( TQLayout *l, int stretch );

    TQSize sizeHint() const;
    TQSize minimumSize() const;
    TQSize maximumSize() const;

    bool hasHeightForWidth() const;
    int heightForWidth( int ) const;
    int minimumHeightForWidth( int ) const;

    TQSizePolicy::ExpandData expanding() const;
    void invalidate();
    TQLayoutIterator iterator();
    void setGeometry( const TQRect& );

    int findWidget( TQWidget* w );

protected:
    void insertItem( int index, TQLayoutItem * );

private:
    friend class TQDockWindow;
#if defined(TQ_DISABLE_COPY)
    TQBoxLayout( const TQBoxLayout & );
    TQBoxLayout &operator=( const TQBoxLayout & );
#endif

    void setupGeom();
    void calcHfw( int );
    TQBoxLayoutData *data;
    Direction dir;
    TQBoxLayout *createTmpCopy();
};

class TQ_EXPORT TQHBoxLayout : public TQBoxLayout
{
    TQ_OBJECT
public:
    TQHBoxLayout( TQWidget *parent, int border = 0,
		 int spacing = -1, const char *name = 0 );
    TQHBoxLayout( TQLayout *parentLayout,
		 int spacing = -1, const char *name = 0 );
    TQHBoxLayout( int spacing = -1, const char *name = 0 );

    ~TQHBoxLayout();

private:	// Disabled copy constructor and operator=
#if defined(TQ_DISABLE_COPY)
    TQHBoxLayout( const TQHBoxLayout & );
    TQHBoxLayout &operator=( const TQHBoxLayout & );
#endif
};

class TQ_EXPORT TQVBoxLayout : public TQBoxLayout
{
    TQ_OBJECT
public:
    TQVBoxLayout( TQWidget *parent, int border = 0,
		 int spacing = -1, const char *name = 0 );
    TQVBoxLayout( TQLayout *parentLayout,
		 int spacing = -1, const char *name = 0 );
    TQVBoxLayout( int spacing = -1, const char *name = 0 );

    ~TQVBoxLayout();

private:	// Disabled copy constructor and operator=
#if defined(TQ_DISABLE_COPY)
    TQVBoxLayout( const TQVBoxLayout & );
    TQVBoxLayout &operator=( const TQVBoxLayout & );
#endif
};

#endif // TQT_NO_LAYOUT
#endif // TQLAYOUT_H
