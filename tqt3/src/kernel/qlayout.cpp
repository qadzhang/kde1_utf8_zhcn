/****************************************************************************
**
** Implementation of layout classes
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

#include "ntqlayout.h"

#ifndef TQT_NO_LAYOUT

#include "ntqapplication.h"
#include "ntqwidget.h"
#include "ntqptrlist.h"
#include "ntqsizepolicy.h"

#include "qlayoutengine_p.h"

/*
  Three internal classes related to TQGridLayout: (1) TQGridBox is a
  TQLayoutItem with (row, column) information; (2) TQGridMultiBox is a
  TQGridBox with (torow, tocolumn) information; (3) TQGridLayoutData is
  the internal representation of a TQGridLayout.
*/

class TQGridBox
{
public:
    TQGridBox( TQLayoutItem *lit ) { item_ = lit; }

    TQGridBox( TQWidget *wid ) { item_ = new TQWidgetItem( wid ); }
    TQGridBox( int w, int h, TQSizePolicy::SizeType hData = TQSizePolicy::Minimum,
	      TQSizePolicy::SizeType vData = TQSizePolicy::Minimum )
    { item_ = new TQSpacerItem( w, h, hData, vData ); }
    ~TQGridBox() { delete item_; }

    TQSize sizeHint() const { return item_->sizeHint(); }
    TQSize minimumSize() const { return item_->minimumSize(); }
    TQSize maximumSize() const { return item_->maximumSize(); }
    TQSizePolicy::ExpandData expanding() const { return item_->expanding(); }
    bool isEmpty() const { return item_->isEmpty(); }

    bool hasHeightForWidth() const { return item_->hasHeightForWidth(); }
    int heightForWidth( int w ) const { return item_->heightForWidth(w); }

    void setAlignment( int a ) { item_->setAlignment( a ); }
    void setGeometry( const TQRect &r ) { item_->setGeometry( r ); }
    int alignment() const { return item_->alignment(); }
    TQLayoutItem *item() { return item_; }
    TQLayoutItem *takeItem() { TQLayoutItem *i = item_; item_ = 0; return i; }

    int hStretch() { return item_->widget() ?
			 item_->widget()->sizePolicy().horStretch() : 0; }
    int vStretch() { return item_->widget() ?
			 item_->widget()->sizePolicy().verStretch() : 0; }

private:
    friend class TQGridLayoutData;
    friend class TQGridLayoutDataIterator;

    TQLayoutItem *item_;
    int row, col;
};

class TQGridMultiBox
{
public:
    TQGridMultiBox( TQGridBox *box, int toRow, int toCol )
	: box_( box ), torow( toRow ), tocol( toCol ) { }
    ~TQGridMultiBox() { delete box_; }
    TQGridBox *box() { return box_; }
    TQLayoutItem *takeItem() { return box_->takeItem(); }

private:
    friend class TQGridLayoutData;
    friend class TQGridLayoutDataIterator;

    TQGridBox *box_;
    int torow, tocol;
};

class TQGridLayoutData
{
public:
    TQGridLayoutData();
    TQGridLayoutData( int nRows, int nCols );
    ~TQGridLayoutData();

    void add( TQGridBox*, int row, int col );
    void add( TQGridBox*, int row1, int row2, int col1, int col2 );
    TQSize sizeHint( int ) const;
    TQSize minimumSize( int ) const;
    TQSize maximumSize( int ) const;

    TQSizePolicy::ExpandData expanding( int spacing );

    void distribute( TQRect, int );
    inline int numRows() const { return rr; }
    inline int numCols() const { return cc; }
    inline void expand( int rows, int cols )
    { setSize( TQMAX(rows, rr), TQMAX(cols, cc) ); }
    inline void setRowStretch( int r, int s )
    { expand( r + 1, 0 ); rStretch[r] = s; setDirty(); }
    inline void setColStretch( int c, int s )
    { expand( 0, c + 1 ); cStretch[c] = s; setDirty(); }
    inline int rowStretch( int r ) const { return rStretch[r]; }
    inline int colStretch( int c ) const { return cStretch[c]; }
    inline void setRowSpacing( int r, int s )
    { expand( r + 1, 0 ); rSpacing[r] = s; setDirty(); }
    inline void setColSpacing( int c, int s )
    { expand( 0, c + 1 ); cSpacing[c] = s; setDirty(); }
    inline int rowSpacing( int r ) const { return rSpacing[r]; }
    inline int colSpacing( int c ) const { return cSpacing[c]; }

    inline void setReversed( bool r, bool c ) { hReversed = c; vReversed = r; }
    inline bool horReversed() const { return hReversed; }
    inline bool verReversed() const { return vReversed; }
    inline void setDirty() { needRecalc = true; hfw_width = -1; }
    inline bool isDirty() const { return needRecalc; }
    bool hasHeightForWidth( int space );
    int heightForWidth( int, int, int );
    int minimumHeightForWidth( int, int, int );

    bool findWidget( TQWidget* w, int *row, int *col );

    inline void getNextPos( int &row, int &col ) { row = nextR; col = nextC; }
    inline uint count() const
    { return things.count() + ( multi ? multi->count() : 0 ); }
    TQRect cellGeometry( int row, int col ) const;

private:
    void setNextPosAfter( int r, int c );
    void recalcHFW( int w, int s );
    void addHfwData ( TQGridBox *box, int width );
    void init();
    TQSize findSize( TQCOORD TQLayoutStruct::*, int ) const;
    void addData( TQGridBox *b, bool r = true, bool c = true );
    void setSize( int rows, int cols );
    void setupLayoutData( int space );
    void setupHfwLayoutData( int space );

    int rr;
    int cc;
    TQMemArray<TQLayoutStruct> rowData;
    TQMemArray<TQLayoutStruct> colData;
    TQMemArray<TQLayoutStruct> *hfwData;
    TQMemArray<int> rStretch;
    TQMemArray<int> cStretch;
    TQMemArray<int> rSpacing;
    TQMemArray<int> cSpacing;
    TQPtrList<TQGridBox> things;
    TQPtrList<TQGridMultiBox> *multi;

    int hfw_width;
    int hfw_height;
    int hfw_minheight;
    int nextR;
    int nextC;

    uint hReversed	: 1;
    uint vReversed	: 1;
    uint needRecalc	: 1;
    uint has_hfw	: 1;
    uint addVertical	: 1;

    friend class TQGridLayoutDataIterator;
};

TQGridLayoutData::TQGridLayoutData()
{
    init();
}

TQGridLayoutData::TQGridLayoutData( int nRows, int nCols )
    : rowData( 0 ), colData( 0 )
{
    init();
    if ( nRows  < 0 ) {
	nRows = 1;
	addVertical = false;
    }
    if ( nCols  < 0 ) {
	nCols = 1;
	addVertical = true;
    }
    setSize( nRows, nCols );
}

TQGridLayoutData::~TQGridLayoutData()
{
    // must be cleared while the data is still in a stable state
    things.clear();

    delete multi;
    delete hfwData;
}

void TQGridLayoutData::init()
{
    addVertical = false;
    setDirty();
    multi = 0;
    rr = cc = 0;
    nextR = nextC = 0;
    hfwData = 0;
    things.setAutoDelete( true );
    hReversed = false;
    vReversed = false;
}

bool TQGridLayoutData::hasHeightForWidth( int spacing )
{
    setupLayoutData( spacing );
    return has_hfw;
}

/*
  Assumes that setupLayoutData() has been called, and that
  qGeomCalc() has filled in colData with appropriate values.
*/
void TQGridLayoutData::recalcHFW( int w, int spacing )
{
    /*
      Go through all children, using colData and heightForWidth()
      and put the results in hfw_rowData.
    */
    if ( !hfwData )
	hfwData = new TQMemArray<TQLayoutStruct>( rr );
    setupHfwLayoutData( spacing );
    TQMemArray<TQLayoutStruct> &rData = *hfwData;

    int h = 0;
    int mh = 0;
    int n = 0;
    for ( int r = 0; r < rr; r++ ) {
	h += rData[r].sizeHint;
	mh += rData[r].minimumSize;
	if ( !rData[r].empty )
	    n++;
    }
    if ( n ) {
	h += ( n - 1 ) * spacing;
	mh += ( n - 1 ) * spacing;
    }

    hfw_width = w;
    hfw_height = TQMIN( TQLAYOUTSIZE_MAX, h );
    hfw_minheight = TQMIN( TQLAYOUTSIZE_MAX, mh );
}

int TQGridLayoutData::heightForWidth( int w, int margin, int spacing )
{
    setupLayoutData( spacing );
    if ( !has_hfw )
	return -1;
    if ( w + 2*margin != hfw_width ) {
	qGeomCalc( colData, 0, cc, 0, w+2*margin, spacing );
	recalcHFW( w+2*margin, spacing );
    }
    return hfw_height + 2*margin;
}

int TQGridLayoutData::minimumHeightForWidth( int w, int margin, int spacing )
{
    (void) heightForWidth( w, margin, spacing );
    return has_hfw ? (hfw_minheight + 2*margin) : -1;
}

bool TQGridLayoutData::findWidget( TQWidget* w, int *row, int *col )
{
    TQPtrListIterator<TQGridBox> it( things );
    TQGridBox * box;
    while ( (box = it.current()) != 0 ) {
	++it;
	if ( box->item()->widget() == w ) {
	    if ( row )
		*row = box->row;
	    if ( col )
		*col = box->col;
	    return true;
	}
    }
    if ( multi ) {
	TQPtrListIterator<TQGridMultiBox> it( *multi );
	TQGridMultiBox * mbox;
	while ( (mbox = it.current()) != 0 ) {
	    ++it;
	    box = mbox->box();
	    if ( box->item()->widget() == w ) {
		if ( row )
		    *row = box->row;
		if ( col )
		    *col = box->col;
		return true;
	    }

	}
    }
    return false;
}

TQSize TQGridLayoutData::findSize( TQCOORD TQLayoutStruct::*size, int spacer ) const
{
    TQGridLayoutData *that = (TQGridLayoutData *)this;
    that->setupLayoutData( spacer );

    int w = 0;
    int h = 0;
    int n = 0;
    for ( int r = 0; r < rr; r++ ) {
	h = h + rowData[r].*size;
	if ( !rowData[r].empty )
	    n++;
    }
    if ( n )
	h += ( n - 1 ) * spacer;
    n = 0;
    for ( int c = 0; c < cc; c++ ) {
	w = w + colData[c].*size;
	if ( !colData[c].empty )
	    n++;
    }
    if ( n )
	w += ( n - 1 ) * spacer;
    w = TQMIN( TQLAYOUTSIZE_MAX, w );
    h = TQMIN( TQLAYOUTSIZE_MAX, h );

    return TQSize( w, h );
}

TQSizePolicy::ExpandData TQGridLayoutData::expanding( int spacing )
{
    setupLayoutData( spacing );
    int ret = 0;

    for ( int r = 0; r < rr; r++ ) {
	if ( rowData[r].expansive ) {
	    ret |= (int) TQSizePolicy::Vertically;
	    break;
	}
    }
    for ( int c = 0; c < cc; c++ ) {
	if ( colData[c].expansive ) {
	    ret |= (int) TQSizePolicy::Horizontally;
	    break;
	}
    }
    return (TQSizePolicy::ExpandData) ret;
}

TQSize TQGridLayoutData::sizeHint( int spacer ) const
{
    return findSize( &TQLayoutStruct::sizeHint, spacer );
}

TQSize TQGridLayoutData::maximumSize( int spacer ) const
{
    return findSize( &TQLayoutStruct::maximumSize, spacer );
}

TQSize TQGridLayoutData::minimumSize( int spacer ) const
{
    return findSize( &TQLayoutStruct::minimumSize, spacer );
}

void TQGridLayoutData::setSize( int r, int c )
{
    if ( (int)rowData.size() < r ) {
	int newR = TQMAX( r, rr * 2 );
	rowData.resize( newR );
	rStretch.resize( newR );
	rSpacing.resize( newR );
	for ( int i = rr; i < newR; i++ ) {
	    rowData[i].init();
	    rStretch[i] = 0;
	    rSpacing[i] = 0;
	}
    }
    if ( (int)colData.size() < c ) {
	int newC = TQMAX( c, cc * 2 );
	colData.resize( newC );
	cStretch.resize( newC );
	cSpacing.resize( newC );
	for ( int i = cc; i < newC; i++ ) {
	    colData[i].init();
	    cStretch[i] = 0;
	    cSpacing[i] = 0;
	}
    }

    if ( hfwData && (int)hfwData->size() < r ) {
	delete hfwData;
	hfwData = 0;
	hfw_width = -1;
    }
    rr = r;
    cc = c;
}

void TQGridLayoutData::setNextPosAfter( int row, int col )
{
    if ( addVertical ) {
	if ( (col > nextC) || ((col == nextC) && (row >= nextR)) ) {
	    nextR = row + 1;
	    nextC = col;
	    if ( nextR >= rr ) {
		nextR = 0;
		nextC++;
	    }
	}
    } else {
	if ( (row > nextR) || ((row == nextR) && (col >= nextC)) ) {
	    nextR = row;
	    nextC = col + 1;
	    if ( nextC >= cc ) {
		nextC = 0;
		nextR++;
	    }
	}
    }
}

void TQGridLayoutData::add( TQGridBox *box, int row, int col )
{
    expand( row+1, col+1 );
    box->row = row;
    box->col = col;
    things.append( box );
    setDirty();
    setNextPosAfter( row, col );
}

void TQGridLayoutData::add( TQGridBox *box, int row1, int row2, int col1,
			   int col2  )
{
#ifdef QT_CHECK_RANGE
    if ( row2 >= 0 && row2 < row1 )
	tqWarning( "TQGridLayout: Multi-cell fromRow greater than toRow" );
    if ( col2 >= 0 && col2 < col1 )
	tqWarning( "TQGridLayout: Multi-cell fromCol greater than toCol" );
#endif
    if ( row1 == row2 && col1 == col2 ) {
	add( box, row1, col1 );
	return;
    }
    expand( row2+1, col2+1 );
    box->row = row1;
    box->col = col1;
    TQGridMultiBox *mbox = new TQGridMultiBox( box, row2, col2 );
    if ( !multi ) {
	multi = new TQPtrList<TQGridMultiBox>;
	multi->setAutoDelete( true );
    }
    multi->append( mbox );
    setDirty();
    if ( col2 < 0 )
	col2 = cc - 1;

    setNextPosAfter( row2, col2 );
}

void TQGridLayoutData::addData( TQGridBox *box, bool r, bool c )
{
    TQSize hint = box->sizeHint();
    TQSize minS = box->minimumSize();
    TQSize maxS = box->maximumSize();

    if ( c ) {
	if ( !cStretch[box->col] )
	    colData[box->col].stretch = TQMAX( colData[box->col].stretch,
					      box->hStretch() );
	colData[box->col].sizeHint = TQMAX( hint.width(),
					   colData[box->col].sizeHint );
	colData[box->col].minimumSize = TQMAX( minS.width(),
					      colData[box->col].minimumSize );

	qMaxExpCalc( colData[box->col].maximumSize, colData[box->col].expansive,
		     maxS.width(),
		     box->expanding() & TQSizePolicy::Horizontally );
    }
    if ( r ) {
	if ( !rStretch[box->row] )
	    rowData[box->row].stretch = TQMAX( rowData[box->row].stretch,
					      box->vStretch() );
	rowData[box->row].sizeHint = TQMAX( hint.height(),
					   rowData[box->row].sizeHint );
	rowData[box->row].minimumSize = TQMAX( minS.height(),
					      rowData[box->row].minimumSize );

	qMaxExpCalc( rowData[box->row].maximumSize, rowData[box->row].expansive,
		     maxS.height(),
		     box->expanding() & TQSizePolicy::Vertically );
    }
    if ( box->isEmpty() ) {
	if ( box->item()->widget() != 0 ) {
	    /*
	      Hidden widgets should behave exactly the same as if
	      there were no widgets at all in the cell. We could have
	      TQWidgetItem::maximumSize() to return
	      TQSize(TQLAYOUTSIZE_MAX, TQLAYOUTSIZE_MAX). However, that
	      value is not suitable for TQBoxLayouts. So let's fix it
	      here.
	    */
	    if ( c )
		colData[box->col].maximumSize = TQLAYOUTSIZE_MAX;
	    if ( r )
		rowData[box->row].maximumSize = TQLAYOUTSIZE_MAX;
	}
    } else {
	// Empty boxes (i.e. spacers) do not get borders. This is
	// hacky, but compatible.
	if ( c )
	    colData[box->col].empty = false;
	if ( r )
	    rowData[box->row].empty = false;
    }
}

static void distributeMultiBox( TQMemArray<TQLayoutStruct> &chain, int spacing,
				int start, int end,
				int minSize, int sizeHint,
				TQMemArray<int> &stretchArray, int stretch )
{
    int i;
    int w = 0;
    int wh = 0;
    int max = 0;
    for ( i = start; i <= end; i++ ) {
	w += chain[i].minimumSize;
	wh += chain[i].sizeHint;
	max += chain[i].maximumSize;
	chain[i].empty = false;
	if ( stretchArray[i] == 0 )
	    chain[i].stretch = TQMAX(chain[i].stretch,stretch);
    }
    w += spacing * ( end - start );
    wh += spacing * ( end - start );
    max += spacing * ( end - start );

    if ( max < minSize ) { // implies w < minSize
	/*
	  We must increase the maximum size of at least one of the
	  items. qGeomCalc() will put the extra space in between the
	  items. We must recover that extra space and put it
	  somewhere. It does not really matter where, since the user
	  can always specify stretch factors and avoid this code.
	*/
	qGeomCalc( chain, start, end - start + 1, 0, minSize, spacing );
	int pos = 0;
	for ( i = start; i <= end; i++ ) {
	    int nextPos = ( i == end ) ? minSize - 1 : chain[i + 1].pos;
	    int realSize = nextPos - pos;
	    if ( i != end )
		realSize -= spacing;
	    if ( chain[i].minimumSize < realSize )
		chain[i].minimumSize = realSize;
	    if ( chain[i].maximumSize < chain[i].minimumSize )
		chain[i].maximumSize = chain[i].minimumSize;
	    pos = nextPos;
	}
    } else if ( w < minSize ) {
	qGeomCalc( chain, start, end - start + 1, 0, minSize, spacing );
	for ( i = start; i <= end; i++ ) {
	    if ( chain[i].minimumSize < chain[i].size )
		chain[i].minimumSize = chain[i].size;
	}
    }

    if ( wh < sizeHint ) {
	qGeomCalc( chain, start, end - start + 1, 0, sizeHint, spacing );
	for ( i = start; i <= end; i++ ) {
	    if ( chain[i].sizeHint < chain[i].size )
		chain[i].sizeHint = chain[i].size;
	}
    }
}

//#define QT_LAYOUT_DISABLE_CACHING

void TQGridLayoutData::setupLayoutData( int spacing )
{
#ifndef QT_LAYOUT_DISABLE_CACHING
    if ( !needRecalc )
	return;
#endif
    has_hfw = false;
    int i;

    for ( i = 0; i < rr; i++ )
	rowData[i].init( rStretch[i], rSpacing[i] );
    for ( i = 0; i < cc; i++ )
	colData[i].init( cStretch[i], cSpacing[i] );

    TQPtrListIterator<TQGridBox> it( things );
    TQGridBox * box;
    while ( (box = it.current()) != 0 ) {
	++it;
	addData( box );
	has_hfw = has_hfw || box->item()->hasHeightForWidth();
    }

    if ( multi ) {
	TQPtrListIterator<TQGridMultiBox> it( *multi );
	TQGridMultiBox * mbox;
	while ( (mbox = it.current()) != 0 ) {
	    ++it;
	    TQGridBox *box = mbox->box();
	    int r1 = box->row;
	    int c1 = box->col;
	    int r2 = mbox->torow;
	    int c2 = mbox->tocol;
	    if ( r2 < 0 )
		r2 = rr - 1;
	    if ( c2 < 0 )
		c2 = cc - 1;

	    TQSize hint = box->sizeHint();
	    TQSize min = box->minimumSize();
	    if ( box->hasHeightForWidth() )
		has_hfw = true;

	    if ( r1 == r2 ) {
		addData( box, true, false );
	    } else {
		distributeMultiBox( rowData, spacing, r1, r2,
				    min.height(), hint.height(),
				    rStretch, box->vStretch() );
	    }
	    if ( c1 == c2 ) {
		addData( box, false, true );
	    } else {
		distributeMultiBox( colData, spacing, c1, c2,
				    min.width(), hint.width(),
				    cStretch, box->hStretch() );
	    }
	}
    }
    for ( i = 0; i < rr; i++ )
	rowData[i].expansive = rowData[i].expansive || rowData[i].stretch > 0;
    for ( i = 0; i < cc; i++ )
	colData[i].expansive = colData[i].expansive || colData[i].stretch > 0;

    needRecalc = false;
}

void TQGridLayoutData::addHfwData( TQGridBox *box, int width )
{
    TQMemArray<TQLayoutStruct> &rData = *hfwData;
    if ( box->hasHeightForWidth() ) {
	int hint = box->heightForWidth( width );
	rData[box->row].sizeHint = TQMAX( hint, rData[box->row].sizeHint );
	rData[box->row].minimumSize = TQMAX( hint, rData[box->row].minimumSize );
    } else {
	TQSize hint = box->sizeHint();
	TQSize minS = box->minimumSize();
	rData[box->row].sizeHint = TQMAX( hint.height(),
					 rData[box->row].sizeHint );
	rData[box->row].minimumSize = TQMAX( minS.height(),
					    rData[box->row].minimumSize );
    }
}

/*
  Similar to setupLayoutData(), but uses heightForWidth(colData)
  instead of sizeHint(). Assumes that setupLayoutData() and
  qGeomCalc(colData) has been called.
*/
void TQGridLayoutData::setupHfwLayoutData( int spacing )
{
    TQMemArray<TQLayoutStruct> &rData = *hfwData;
    int i;
    for ( i = 0; i < rr; i++ ) {
	rData[i] = rowData[i];
	rData[i].minimumSize = rData[i].sizeHint = 0;
    }
    TQPtrListIterator<TQGridBox> it( things );
    TQGridBox * box;
    while ( (box=it.current()) != 0 ) {
	++it;
	addHfwData( box, colData[box->col].size );
    }
    if ( multi ) {
	TQPtrListIterator<TQGridMultiBox> it( *multi );
	TQGridMultiBox * mbox;
	while ( (mbox=it.current()) != 0 ) {
	    ++it;
	    TQGridBox *box = mbox->box();
	    int r1 = box->row;
	    int c1 = box->col;
	    int r2 = mbox->torow;
	    int c2 = mbox->tocol;
	    if ( r2 < 0 )
		r2 = rr-1;
	    if ( c2 < 0 )
		c2 = cc-1;
	    int w = colData[c2].pos + colData[c2].size - colData[c1].pos;
	    if ( r1 == r2 ) {
		addHfwData( box, w );
	    } else {
		TQSize hint = box->sizeHint();
		TQSize min = box->minimumSize();
		if ( box->hasHeightForWidth() ) {
		    int hfwh = box->heightForWidth( w );
		    if ( hfwh > hint.height() )
			hint.setHeight( hfwh );
		    if ( hfwh > min.height() )
			min.setHeight( hfwh );
		}
		distributeMultiBox( rData, spacing, r1, r2,
				    min.height(), hint.height(),
				    rStretch, box->vStretch() );
	    }
	}
    }
    for ( i = 0; i < rr; i++ )
	rData[i].expansive = rData[i].expansive || rData[i].stretch > 0;
}

void TQGridLayoutData::distribute( TQRect r, int spacing )
{
    bool visualHReversed = hReversed;
    if ( TQApplication::reverseLayout() )
	visualHReversed = !visualHReversed;

    setupLayoutData( spacing );

    qGeomCalc( colData, 0, cc, r.x(), r.width(), spacing );
    TQMemArray<TQLayoutStruct> *rDataPtr;
    if ( has_hfw ) {
	recalcHFW( r.width(), spacing );
	qGeomCalc( *hfwData, 0, rr, r.y(), r.height(), spacing );
	rDataPtr = hfwData;
    } else {
	qGeomCalc( rowData, 0, rr, r.y(), r.height(), spacing );
	rDataPtr = &rowData;
    }
    TQMemArray<TQLayoutStruct> &rData = *rDataPtr;

    TQPtrListIterator<TQGridBox> it( things );
    TQGridBox * box;
    while ( (box=it.current()) != 0 ) {
	++it;
	int x = colData[box->col].pos;
	int y = rData[box->row].pos;
	int w = colData[box->col].size;
	int h = rData[box->row].size;
	if ( visualHReversed )
	    x = r.left() + r.right() - x - w + 1;
	if ( vReversed )
	    y = r.top() + r.bottom() - y - h + 1;
	box->setGeometry( TQRect( x, y, w, h ) );
    }
    if ( multi ) {
	TQPtrListIterator<TQGridMultiBox> it( *multi );
	TQGridMultiBox * mbox;
	while ( (mbox=it.current()) != 0 ) {
	    ++it;
	    TQGridBox *box = mbox->box();
	    int r2 = mbox->torow;
	    int c2 = mbox->tocol;
	    if ( r2 < 0 )
		r2 = rr-1;
	    if ( c2 < 0 )
		c2 = cc-1;

	    int x = colData[box->col].pos;
	    int y = rData[box->row].pos;
	    int x2p = colData[c2].pos + colData[c2].size; // x2+1
	    int y2p = rData[r2].pos + rData[r2].size;    // y2+1
	    int w = x2p - x;
	    int h = y2p - y;
	    // this code is copied from above:
	    if ( visualHReversed )
		x = r.left() + r.right() - x - w + 1;
	    if ( vReversed )
		y = r.top() + r.bottom() - y - h + 1;
	    box->setGeometry( TQRect( x, y, w, h ) );
	}
    }
}

TQRect TQGridLayoutData::cellGeometry( int row, int col ) const
{
    if ( row < 0 || row >= rr || col < 0 || col >= cc )
	return TQRect();

    const TQMemArray<TQLayoutStruct> *rDataPtr;
    if ( has_hfw )
	rDataPtr = hfwData;
    else
	rDataPtr = &rowData;
    return TQRect( colData[col].pos, (*rDataPtr)[row].pos,
		  colData[col].size, (*rDataPtr)[row].size );
}

class TQGridLayoutDataIterator : public TQGLayoutIterator
{
public:
    TQGridLayoutDataIterator( TQGridLayoutData *d );
    uint count() const { return data->count(); }
    TQLayoutItem *current() {
	if ( multi ) {
	    if ( !data->multi || idx >= (int)data->multi->count() )
		return 0;
	    return data->multi->at( idx )->box()->item();
	} else {
	    if ( idx >= (int)data->things.count() )
		return 0;
	    return data->things.at( idx )->item();
	}
    }
    void toFirst() {
	multi = data->things.isEmpty();
	idx = 0;
    }
    TQLayoutItem *next() {
	idx++;
	if ( !multi && idx >= (int)data->things.count() ) {
	    multi = true;
	    idx = 0;
	}
	return current();
    }
    TQLayoutItem *takeCurrent() {
	TQLayoutItem *item = 0;
	if ( multi ) {
	     if ( !data->multi || idx >= (int)data->multi->count() )
		return 0;
	    TQGridMultiBox *b = data->multi->take( idx );
	    item = b->takeItem();
	    delete b;
	} else {
	    if ( idx >= (int)data->things.count() )
		return 0;
	    TQGridBox *b = data->things.take( idx );
	    item = b->takeItem();
	    delete b;
	}
	return item;
    }

private:
    TQGridLayoutData *data;
    bool multi;
    int idx;
};

inline TQGridLayoutDataIterator::TQGridLayoutDataIterator( TQGridLayoutData *d )
    : data( d )
{
    toFirst();
}

/*!
    \class TQGridLayout

    \brief The TQGridLayout class lays out widgets in a grid.

    \ingroup geomanagement
    \ingroup appearance
    \mainclass

    TQGridLayout takes the space made available to it (by its parent
    layout or by the mainWidget()), divides it up into rows and
    columns, and puts each widget it manages into the correct cell.

    Columns and rows behave identically; we will discuss columns, but
    there are equivalent functions for rows.

    Each column has a minimum width and a stretch factor. The minimum
    width is the greatest of that set using addColSpacing() and the
    minimum width of each widget in that column. The stretch factor is
    set using setColStretch() and determines how much of the available
    space the column will get over and above its necessary minimum.

    Normally, each managed widget or layout is put into a cell of its
    own using addWidget(), addLayout() or by the \link
    TQLayout::setAutoAdd() auto-add facility\endlink. It is also
    possible for a widget to occupy multiple cells using
    addMultiCellWidget(). If you do this, TQGridLayout will guess how
    to distribute the size over the columns/rows (based on the stretch
    factors).

    To remove a widget from a layout, call remove(). Calling
    TQWidget::hide() on a widget also effectively removes the widget
    from the layout until TQWidget::show() is called.

    This illustration shows a fragment of a dialog with a five-column,
    three-row grid (the grid is shown overlaid in magenta):

    \img gridlayout.png

    Columns 0, 2 and 4 in this dialog fragment are made up of a
    TQLabel, a TQLineEdit, and a TQListBox. Columns 1 and 3 are
    placeholders made with addColSpacing(). Row 0 consists of three
    TQLabel objects, row 1 of three TQLineEdit objects and row 2 of
    three TQListBox objects. We used placeholder columns (1 and 3) to
    get the right amount of space between the columns.

    Note that the columns and rows are not equally wide or tall. If
    you want two columns to have the same width, you must set their
    minimum widths and stretch factors to be the same yourself. You do
    this using addColSpacing() and setColStretch().

    If the TQGridLayout is not the top-level layout (i.e. does not
    manage all of the widget's area and children), you must add it to
    its parent layout when you create it, but before you do anything
    with it. The normal way to add a layout is by calling
    parentLayout-\>addLayout().

    Once you have added your layout you can start putting widgets and
    other layouts into the cells of your grid layout using
    addWidget(), addLayout() and addMultiCellWidget().

    TQGridLayout also includes two margin widths: the border and the
    spacing. The border is the width of the reserved space along each
    of the TQGridLayout's four sides. The spacing is the width of the
    automatically allocated spacing between neighboring boxes.

    Both the border and the spacing are parameters of the constructor
    and default to 0.

    \sa TQGrid, \link layout.html Layout Overview \endlink
*/

/*!
    \enum TQGridLayout::Corner

    This enum identifies which corner is the origin (0, 0) of the
    layout.

    \value TopLeft the top-left corner
    \value TopRight the top-right corner
    \value BottomLeft the bottom-left corner
    \value BottomRight the bottom-right corner
*/

/*!
    Constructs a new TQGridLayout with \a nRows rows, \a nCols columns
    and parent widget, \a  parent. \a parent may not be 0. The grid
    layout is called \a name.

    \a margin is the number of pixels between the edge of the widget
    and its managed children. \a space is the default number of pixels
    between cells. If \a space is -1, the value of \a margin is used.
*/
TQGridLayout::TQGridLayout( TQWidget *parent, int nRows, int nCols, int margin,
			  int space, const char *name )
    : TQLayout( parent, margin, space, name )
{
    init( nRows, nCols );
}

/*!
    Constructs a new grid that is placed inside \a parentLayout with
    \a nRows rows and \a nCols columns. If \a spacing is -1, this
    TQGridLayout inherits its parent's spacing(); otherwise \a spacing
    is used. The grid layout is called \a name.

    This grid is placed according to \a parentLayout's default
    placement rules.
*/
TQGridLayout::TQGridLayout( TQLayout *parentLayout, int nRows, int nCols,
			  int spacing, const char *name )
    : TQLayout( parentLayout, spacing, name )
{
    init( nRows, nCols );
}

/*!
    Constructs a new grid with \a nRows rows and \a nCols columns. If
    \a spacing is -1, this TQGridLayout inherits its parent's
    spacing(); otherwise \a spacing is used. The grid layout is called
    \a name.

    You must insert this grid into another layout. You can insert
    widgets and layouts into this layout at any time, but laying out
    will not be performed before this is inserted into another layout.
*/
TQGridLayout::TQGridLayout( int nRows, int nCols,
			  int spacing, const char *name )
     : TQLayout( spacing, name )
{
    init( nRows, nCols );
}

/*!
    Destroys the grid layout. Geometry management is terminated if
    this is a top-level grid.

    The layout's widgets aren't destroyed.
*/
TQGridLayout::~TQGridLayout()
{
    delete data;
}

/*!
    Returns the number of rows in this grid.
*/
int TQGridLayout::numRows() const
{
    return data->numRows();
}

/*!
    Returns the number of columns in this grid.
*/
int TQGridLayout::numCols() const
{
    return data->numCols();
}

/*!
    Returns the preferred size of this grid.
*/
TQSize TQGridLayout::sizeHint() const
{
    return data->sizeHint( spacing() ) + TQSize( 2 * margin(), 2 * margin() );
}

/*!
    Returns the minimum size needed by this grid.
*/
TQSize TQGridLayout::minimumSize() const
{
    return data->minimumSize( spacing() ) + TQSize( 2 * margin(), 2 * margin() );
}

/*!
    Returns the maximum size needed by this grid.
*/
TQSize TQGridLayout::maximumSize() const
{
    TQSize s = data->maximumSize( spacing() ) +
	      TQSize( 2 * margin(), 2 * margin() );
    s = s.boundedTo( TQSize(TQLAYOUTSIZE_MAX, TQLAYOUTSIZE_MAX) );
    if ( alignment() & TQt::AlignHorizontal_Mask )
	s.setWidth( TQLAYOUTSIZE_MAX );
    if ( alignment() & TQt::AlignVertical_Mask )
	s.setHeight( TQLAYOUTSIZE_MAX );
    return s;
}

/*!
    Returns true if this layout's preferred height depends on its
    width; otherwise returns false.
*/
bool TQGridLayout::hasHeightForWidth() const
{
    return ((TQGridLayout*)this)->data->hasHeightForWidth( spacing() );
}

/*!
    Returns the layout's preferred height when it is \a w pixels wide.
*/
int TQGridLayout::heightForWidth( int w ) const
{
    TQGridLayout *that = (TQGridLayout*)this;
    return that->data->heightForWidth( w, margin(), spacing() );
}

/*! \internal */
int TQGridLayout::minimumHeightForWidth( int w ) const
{
    TQGridLayout *that = (TQGridLayout*)this;
    return that->data->minimumHeightForWidth( w, margin(), spacing() );
}

/*!
    Searches for widget \a w in this layout (not including child
    layouts). If \a w is found, it sets \c \a row and \c \a col to
    the row and column and returns true; otherwise returns false.

    Note: if a widget spans multiple rows/columns, the top-left cell
    is returned.
*/
bool TQGridLayout::findWidget( TQWidget* w, int *row, int *col )
{
    return data->findWidget( w, row, col );
}

/*!
    Resizes managed widgets within the rectangle \a r.
*/
void TQGridLayout::setGeometry( const TQRect &r )
{
    if ( data->isDirty() || r != geometry() ) {
	TQLayout::setGeometry( r );
	TQRect cr = alignment() ? alignmentRect( r ) : r;
	TQRect s( cr.x() + margin(), cr.y() + margin(),
		 cr.width() - 2 * margin(), cr.height() - 2 * margin() );
	data->distribute( s, spacing() );
    }
}

/*!
    Returns the geometry of the cell with row \a row and column \a col
    in the grid. Returns an invalid rectangle if \a row or \a col is
    outside the grid.

    \warning in the current version of TQt this function does not
    return valid results until setGeometry() has been called, i.e.
    after the mainWidget() is visible.
*/
TQRect TQGridLayout::cellGeometry( int row, int col ) const
{
    return data->cellGeometry( row, col );
}

/*!
    Expands this grid so that it will have \a nRows rows and \a nCols
    columns. Will not shrink the grid. You should not need to call
    this function because TQGridLayout expands automatically as new
    items are inserted.
*/
void TQGridLayout::expand( int nRows, int nCols )
{
    data->expand( nRows, nCols );
}

/*!
    Sets up the grid.
*/
void TQGridLayout::init( int nRows, int nCols )
{
    setSupportsMargin( true );
    data = new TQGridLayoutData( nRows, nCols );
}

/*!
    \overload

    Adds \a item to the next free position of this layout.
*/
void TQGridLayout::addItem( TQLayoutItem *item )
{
    int r, c;
    data->getNextPos( r, c );
    add( item, r, c );
}

/*!
    Adds \a item at position \a row, \a col. The layout takes
    ownership of the \a item.
*/
void TQGridLayout::addItem( TQLayoutItem *item, int row, int col )
{
    add( item, row, col );
}

/*!
    Adds \a item at position \a row, \a col. The layout takes
    ownership of the \a item.
*/
void TQGridLayout::add( TQLayoutItem *item, int row, int col )
{
    TQGridBox *box = new TQGridBox( item );
    data->add( box, row, col );
}

/*!
    Adds the \a item to the cell grid, spanning multiple rows/columns.

    The cell will span from \a fromRow, \a fromCol to \a toRow, \a
    toCol. Alignment is specified by \a alignment, which is a bitwise
    OR of \l TQt::AlignmentFlags values. The default alignment is 0,
    which means that the widget fills the entire cell.
*/
void TQGridLayout::addMultiCell( TQLayoutItem *item, int fromRow, int toRow,
				int fromCol, int toCol, int alignment )
{
    TQGridBox *b = new TQGridBox( item );
    b->setAlignment( alignment );
    data->add( b, fromRow, toRow, fromCol, toCol );
}

/*
  Returns true if the widget \a w can be added to the layout \a l;
  otherwise returns false.
*/
static bool checkWidget( TQLayout *l, TQWidget *w )
{
    if ( !w ) {
#if defined(QT_CHECK_STATE)
	tqWarning( "TQLayout: Cannot add null widget to %s/%s", l->className(),
		  l->name() );
#endif
	return false;
    }
    if ( w->parentWidget() != l->mainWidget() && l->mainWidget() ) {
#if defined(QT_CHECK_STATE)
	if ( w->parentWidget() )
	    tqWarning( "TQLayout: Adding %s/%s (child of %s/%s) to layout for "
		      "%s/%s", w->className(), w->name(),
		      w->parentWidget()->className(), w->parentWidget()->name(),
		      l->mainWidget()->className(), l->mainWidget()->name() );
	else
	    tqWarning( "TQLayout: Adding %s/%s (top-level widget) to layout for"
		      " %s/%s", w->className(), w->name(),
		      l->mainWidget()->className(), l->mainWidget()->name() );
#endif
	return false;
    }
    return true;
}

/*!
    Adds the widget \a w to the cell grid at \a row, \a col. The
    top-left position is (0, 0) by default.

    Alignment is specified by \a alignment, which is a bitwise OR of
    \l TQt::AlignmentFlags values. The default alignment is 0, which
    means that the widget fills the entire cell.

    \list
    \i You should not call this if you have enabled the
    \link TQLayout::setAutoAdd() auto-add facility of the layout\endlink.

    \i From TQt 3.0, the \a alignment parameter is interpreted more
    aggressively than in previous versions of TQt. A non-default
    alignment now indicates that the widget should not grow to fill
    the available space, but should be sized according to sizeHint().
    \endlist

    \sa addMultiCellWidget()
*/
void TQGridLayout::addWidget( TQWidget *w, int row, int col, int alignment )
{
    if ( !checkWidget( this, w ) )
	return;
    if ( row < 0 || col < 0 ) {
#if defined(QT_CHECK_STATE)
	tqWarning( "TQGridLayout: Cannot add %s/%s to %s/%s at row %d col %d",
		  w->className(), w->name(), className(), name(), row, col );
#endif
	return;
    }
    TQWidgetItem *b = new TQWidgetItem( w );
    b->setAlignment( alignment );
    add( b, row, col );
}

/*!
    Adds the widget \a w to the cell grid, spanning multiple
    rows/columns. The cell will span from \a fromRow, \a fromCol to \a
    toRow, \a toCol.

    Alignment is specified by \a alignment, which is a bitwise OR of
    \l TQt::AlignmentFlags values. The default alignment is 0, which
    means that the widget fills the entire cell.

    A non-zero alignment indicates that the widget should not grow to
    fill the available space but should be sized according to
    sizeHint().

    \sa addWidget()
*/
void TQGridLayout::addMultiCellWidget( TQWidget *w, int fromRow, int toRow,
				      int fromCol, int toCol, int alignment )
{
    TQGridBox *b = new TQGridBox( w );
    b->setAlignment( alignment );
    data->add( b, fromRow, toRow, fromCol, toCol );
}

/*!
    Places the \a layout at position (\a row, \a col) in the grid. The
    top-left position is (0, 0).

    \a layout becomes a child of the grid layout.

    When a layout is constructed with another layout as its parent,
    you don't need to call addLayout(); the child layout is
    automatically added to the parent layout as it is constructed.

    \sa addMultiCellLayout()
*/
void TQGridLayout::addLayout( TQLayout *layout, int row, int col )
{
    addChildLayout( layout );
    add( layout, row, col );
}

/*!
    Adds the layout \a layout to the cell grid, spanning multiple
    rows/columns. The cell will span from \a fromRow, \a fromCol to \a
    toRow, \a toCol.

    Alignment is specified by \a alignment, which is a bitwise OR of
    \l TQt::AlignmentFlags values. The default alignment is 0, which
    means that the widget fills the entire cell.

    A non-zero alignment indicates that the layout should not grow to
    fill the available space but should be sized according to
    sizeHint().

    \a layout becomes a child of the grid layout.

    \sa addLayout()
*/
void TQGridLayout::addMultiCellLayout( TQLayout *layout, int fromRow, int toRow,
				      int fromCol, int toCol, int alignment )
{
    addChildLayout( layout );
    TQGridBox *b = new TQGridBox( layout );
    b->setAlignment( alignment );
    data->add( b, fromRow, toRow, fromCol, toCol );
}

/*!
    Sets the stretch factor of row \a row to \a stretch. The first row
    is number 0.

    The stretch factor is relative to the other rows in this grid.
    Rows with a higher stretch factor take more of the available
    space.

    The default stretch factor is 0. If the stretch factor is 0 and no
    other row in this table can grow at all, the row may still grow.

    \sa rowStretch(), setRowSpacing(), setColStretch()
*/
void TQGridLayout::setRowStretch( int row, int stretch )
{
    data->setRowStretch( row, stretch );
}

/*!
    Returns the stretch factor for row \a row.

    \sa setRowStretch()
*/
int TQGridLayout::rowStretch( int row ) const
{
    return data->rowStretch( row );
}

/*!
    Returns the stretch factor for column \a col.

    \sa setColStretch()
*/
int TQGridLayout::colStretch( int col ) const
{
    return data->colStretch( col );
}

/*!
    Sets the stretch factor of column \a col to \a stretch. The first
    column is number 0.

    The stretch factor is relative to the other columns in this grid.
    Columns with a higher stretch factor take more of the available
    space.

    The default stretch factor is 0. If the stretch factor is 0 and no
    other column in this table can grow at all, the column may still
    grow.

    \sa colStretch(), addColSpacing(), setRowStretch()
*/
void TQGridLayout::setColStretch( int col, int stretch )
{
    data->setColStretch( col, stretch );
}

#if TQT_VERSION >= 0x040000
#error "Make add{Row,Col}Spacing() inline TQT_NO_COMPAT functions defined in terms of set{Row,Col}Spacing()"
#endif

/*!
    \obsolete

    Sets the minimum height of row \a row to \a minsize pixels.

    Use setRowSpacing() instead.
*/
void TQGridLayout::addRowSpacing( int row, int minsize )
{
    TQLayoutItem *b = new TQSpacerItem( 0, minsize );
    add( b, row, 0 );
}

/*!
    \obsolete

    Sets the minimum width of column \a col to \a minsize pixels.

    Use setColSpacing() instead.
*/
void TQGridLayout::addColSpacing( int col, int minsize )
{
    TQLayoutItem *b = new TQSpacerItem( minsize, 0 );
    add( b, 0, col );
}

/*!
    Sets the minimum height of row \a row to \a minSize pixels.

    \sa rowSpacing(), setColSpacing()
*/
void TQGridLayout::setRowSpacing( int row, int minSize )
{
    data->setRowSpacing( row, minSize );
}

/*!
    Returns the row spacing for row \a row.

    \sa setRowSpacing()
*/
int TQGridLayout::rowSpacing( int row ) const
{
    return data->rowSpacing( row );
}

/*!
    Sets the minimum width of column \a col to \a minSize pixels.

    \sa colSpacing(), setRowSpacing()
*/
void TQGridLayout::setColSpacing( int col, int minSize )
{
    data->setColSpacing( col, minSize );
}

/*!
    Returns the column spacing for column \a col.

    \sa setColSpacing()
*/
int TQGridLayout::colSpacing( int col ) const
{
    return data->colSpacing( col );
}

/*!
    Returns whether this layout can make use of more space than
    sizeHint(). A value of \c Vertical or \c Horizontal means that it wants
    to grow in only one dimension, whereas \c BothDirections means that
    it wants to grow in both dimensions.
*/
TQSizePolicy::ExpandData TQGridLayout::expanding() const
{
    return data->expanding( spacing() );
}

/*!
    Sets the grid's origin corner, i.e. position (0, 0), to \a c.
*/
void TQGridLayout::setOrigin( Corner c )
{
    data->setReversed( c == BottomLeft || c == BottomRight,
		       c == TopRight || c == BottomRight );
}

/*!
    Returns the corner that's used for the grid's origin, i.e. for
    position (0, 0).
*/
TQGridLayout::Corner TQGridLayout::origin() const
{
    if ( data->horReversed() ) {
	return data->verReversed() ? BottomRight : TopRight;
    } else {
	return data->verReversed() ? BottomLeft : TopLeft;
    }
}

/*!
    Resets cached information.
*/
void TQGridLayout::invalidate()
{
    TQLayout::invalidate();
    TQLayout::setGeometry( TQRect() ); // for binary compatibility (?)
    data->setDirty();
}

/*! \reimp */
TQLayoutIterator TQGridLayout::iterator()
{
    return TQLayoutIterator( new TQGridLayoutDataIterator(data) );
}

struct TQBoxLayoutItem
{
    TQBoxLayoutItem( TQLayoutItem *it, int stretch_ = 0 )
	: item( it ), stretch( stretch_ ), magic( false ) { }
    ~TQBoxLayoutItem() { delete item; }

    int hfw( int w ) {
	if ( item->hasHeightForWidth() ) {
	    return item->heightForWidth( w );
	} else {
	    return item->sizeHint().height();
	}
    }
    int mhfw( int w ) {
	if ( item->hasHeightForWidth() ) {
	    return item->heightForWidth( w );
	} else {
	    return item->minimumSize().height();
	}
    }
    int hStretch() {
	if ( stretch == 0 && item->widget() ) {
	    return item->widget()->sizePolicy().horStretch();
	} else {
	    return stretch;
	}
    }
    int vStretch() {
	if ( stretch == 0 && item->widget() ) {
	    return item->widget()->sizePolicy().verStretch();
	} else {
	    return stretch;
	}
    }

    TQLayoutItem *item;
    int stretch;
    bool magic;
};

class TQBoxLayoutData
{
public:
    TQBoxLayoutData() : geomArray( 0 ), hfwWidth( -1 ), dirty( true )
    { list.setAutoDelete( true ); }

    ~TQBoxLayoutData() { delete geomArray; }
    void setDirty() {
	delete geomArray;
	geomArray = 0;
	hfwWidth = -1;
	hfwHeight = -1;
	dirty = true;
    }

    TQPtrList<TQBoxLayoutItem> list;
    TQMemArray<TQLayoutStruct> *geomArray;
    int hfwWidth;
    int hfwHeight;
    int hfwMinHeight;
    TQSize sizeHint;
    TQSize minSize;
    TQSize maxSize;
    TQSizePolicy::ExpandData expanding;
    uint hasHfw : 1;
    uint dirty : 1;
};

class TQBoxLayoutIterator : public TQGLayoutIterator
{
public:
    TQBoxLayoutIterator( TQBoxLayoutData *d ) : data( d ), idx( 0 ) {}
    TQLayoutItem *current() {
	if ( idx >= int(data->list.count()) )
	    return 0;
	return data->list.at(idx)->item;
    }
    TQLayoutItem *next() {
	idx++;
	return current();
    }
    TQLayoutItem *takeCurrent() {
	TQLayoutItem *item = 0;

	TQBoxLayoutItem *b = data->list.take( idx );
	if ( b ) {
	    item = b->item;
	    b->item = 0;
	    delete b;
	}
	data->setDirty();
	return item;
    }

private:
    TQBoxLayoutData *data;
    int idx;
};

/*!
    \class TQBoxLayout

    \brief The TQBoxLayout class lines up child widgets horizontally or
    vertically.

    \ingroup geomanagement
    \ingroup appearance

    TQBoxLayout takes the space it gets (from its parent layout or from
    the mainWidget()), divides it up into a row of boxes, and makes
    each managed widget fill one box.

    \img qhbox-m.png Horizontal box with five child widgets

    If the TQBoxLayout's orientation is \c Horizontal the boxes are
    placed in a row, with suitable sizes. Each widget (or other box)
    will get at least its minimum size and at most its maximum size.
    Any excess space is shared according to the stretch factors (more
    about that below).

    \img qvbox-m.png Vertical box with five child widgets

    If the TQBoxLayout's orientation is \c Vertical, the boxes are
    placed in a column, again with suitable sizes.

    The easiest way to create a TQBoxLayout is to use one of the
    convenience classes, e.g. TQHBoxLayout (for \c Horizontal boxes) or
    TQVBoxLayout (for \c Vertical boxes). You can also use the
    TQBoxLayout constructor directly, specifying its direction as \c
    LeftToRight, \c Down, \c RightToLeft or \c Up.

    If the TQBoxLayout is not the top-level layout (i.e. it is not
    managing all of the widget's area and children), you must add it
    to its parent layout before you can do anything with it. The
    normal way to add a layout is by calling
    parentLayout-\>addLayout().

    Once you have done this, you can add boxes to the TQBoxLayout using
    one of four functions:

    \list
    \i addWidget() to add a widget to the TQBoxLayout and set the
    widget's stretch factor. (The stretch factor is along the row of
    boxes.)

    \i addSpacing() to create an empty box; this is one of the
    functions you use to create nice and spacious dialogs. See below
    for ways to set margins.

    \i addStretch() to create an empty, stretchable box.

    \i addLayout() to add a box containing another TQLayout to the row
    and set that layout's stretch factor.
    \endlist

    Use insertWidget(), insertSpacing(), insertStretch() or
    insertLayout() to insert a box at a specified position in the
    layout.

    TQBoxLayout also includes two margin widths:

    \list
    \i setMargin() sets the width of the outer border. This is the width
       of the reserved space along each of the TQBoxLayout's four sides.
    \i setSpacing() sets the width between neighboring boxes. (You
       can use addSpacing() to get more space at a particular spot.)
    \endlist

    The margin defaults to 0. The spacing defaults to the same as the
    margin width for a top-level layout, or to the same as the parent
    layout. Both are parameters to the constructor.

    To remove a widget from a layout, call remove(). Calling
    TQWidget::hide() on a widget also effectively removes the widget
    from the layout until TQWidget::show() is called.

    You will almost always want to use TQVBoxLayout and TQHBoxLayout
    rather than TQBoxLayout because of their convenient constructors.

    \sa TQGrid \link layout.html Layout Overview \endlink
*/

/*!
    \enum TQBoxLayout::Direction

    This type is used to determine the direction of a box layout.

    \value LeftToRight  Horizontal, from left to right
    \value RightToLeft  Horizontal, from right to left
    \value TopToBottom  Vertical, from top to bottom
    \value Down  The same as \c TopToBottom
    \value BottomToTop  Vertical, from bottom to top
    \value Up  The same as \c BottomToTop
*/

static inline bool horz( TQBoxLayout::Direction dir )
{
    return dir == TQBoxLayout::RightToLeft || dir == TQBoxLayout::LeftToRight;
}

/*!
    Constructs a new TQBoxLayout with direction \a d and main widget \a
    parent. \a parent may not be 0.

    The \a margin is the number of pixels between the edge of the
    widget and its managed children. The \a spacing is the default
    number of pixels between neighboring children. If \a spacing is -1
    the value of \a margin is used for \a spacing.

    \a name is the internal object name.

    \sa direction()
*/
TQBoxLayout::TQBoxLayout( TQWidget *parent, Direction d,
			int margin, int spacing, const char *name )
    : TQLayout( parent, margin, spacing, name )
{
    data = new TQBoxLayoutData;
    dir = d;
    setSupportsMargin( true );
}

/*!
    Constructs a new TQBoxLayout called \a name, with direction \a d,
    and inserts it into \a parentLayout.

    The \a spacing is the default number of pixels between neighboring
    children. If \a spacing is -1, the layout will inherit its
    parent's spacing().
*/
TQBoxLayout::TQBoxLayout( TQLayout *parentLayout, Direction d, int spacing,
			const char *name )
    : TQLayout( parentLayout, spacing, name )
{
    data = new TQBoxLayoutData;
    dir = d;
    setSupportsMargin( true );
}

/*!
    Constructs a new TQBoxLayout called \a name, with direction \a d.

    If \a spacing is -1, the layout will inherit its parent's
    spacing(); otherwise \a spacing is used.

    You must insert this box into another layout.
*/
TQBoxLayout::TQBoxLayout( Direction d, int spacing, const char *name )
    : TQLayout( spacing, name )
{
    data = new TQBoxLayoutData;
    dir = d;
    setSupportsMargin( true );
}

/*!
    Destroys this box layout.

    The layout's widgets aren't destroyed.
*/
TQBoxLayout::~TQBoxLayout()
{
    delete data;
}

/*!
    Returns the preferred size of this box layout.
*/
TQSize TQBoxLayout::sizeHint() const
{
    if ( data->dirty ) {
	TQBoxLayout *that = (TQBoxLayout*)this;
	that->setupGeom();
    }
    return data->sizeHint + TQSize( 2 * margin(), 2 * margin() );
}

/*!
    Returns the minimum size needed by this box layout.
*/
TQSize TQBoxLayout::minimumSize() const
{
    if ( data->dirty ) {
	TQBoxLayout *that = (TQBoxLayout*)this;
	that->setupGeom();
    }
    return data->minSize + TQSize( 2 * margin(), 2 * margin() );
}

/*!
    Returns the maximum size needed by this box layout.
*/
TQSize TQBoxLayout::maximumSize() const
{
    if ( data->dirty ) {
	TQBoxLayout *that = (TQBoxLayout*)this;
	that->setupGeom();
    }
    TQSize s = ( data->maxSize + TQSize(2 * margin(), 2 * margin()) )
	      .boundedTo(TQSize(TQLAYOUTSIZE_MAX, TQLAYOUTSIZE_MAX));
    if ( alignment() & TQt::AlignHorizontal_Mask )
	s.setWidth( TQLAYOUTSIZE_MAX );
    if ( alignment() & TQt::AlignVertical_Mask )
	s.setHeight( TQLAYOUTSIZE_MAX );
    return s;
}

/*!
  Returns true if this layout's preferred height depends on its width;
  otherwise returns false.
*/
bool TQBoxLayout::hasHeightForWidth() const
{
    if ( data->dirty ) {
	TQBoxLayout *that = (TQBoxLayout*)this;
	that->setupGeom();
    }
    return data->hasHfw;
}

/*!
    Returns the layout's preferred height when it is \a w pixels wide.
*/
int TQBoxLayout::heightForWidth( int w ) const
{
    if ( !hasHeightForWidth() )
	return -1;
    w -= 2 * margin();
    if ( w != data->hfwWidth ) {
	TQBoxLayout *that = (TQBoxLayout*)this;
	that->calcHfw( w );
    }
    return data->hfwHeight + 2 * margin();
}

/*! \internal */
int TQBoxLayout::minimumHeightForWidth( int w ) const
{
    (void) heightForWidth( w );
    return data->hasHfw ? (data->hfwMinHeight + 2 * margin() ) : -1;
}

/*!
    Resets cached information.
*/
void TQBoxLayout::invalidate()
{
    TQLayout::invalidate();
    data->setDirty();
}

/*!
    \reimp
*/
TQLayoutIterator TQBoxLayout::iterator()
{
    return TQLayoutIterator( new TQBoxLayoutIterator(data) );
}

/*!
    Returns whether this layout can make use of more space than
    sizeHint(). A value of \c Vertical or \c Horizontal means that it wants
    to grow in only one dimension, whereas \c BothDirections means that
    it wants to grow in both dimensions.
*/
TQSizePolicy::ExpandData TQBoxLayout::expanding() const
{
    if ( data->dirty ) {
	TQBoxLayout *that = (TQBoxLayout*)this;
	that->setupGeom();
    }
    return data->expanding;
}

/*!
    Resizes managed widgets within the rectangle \a r.
*/
void TQBoxLayout::setGeometry( const TQRect &r )
{
    if ( !data->geomArray || r != geometry() ) {
	TQLayout::setGeometry( r );
	if ( !data->geomArray )
	    setupGeom();
	TQRect cr = alignment() ? alignmentRect( r ) : r;
	TQRect s( cr.x() + margin(), cr.y() + margin(),
		 cr.width() - 2 * margin(), cr.height() - 2 * margin() );

	TQMemArray<TQLayoutStruct> a = *data->geomArray;
	int pos = horz( dir ) ? s.x() : s.y();
	int space = horz( dir ) ? s.width() : s.height();
	int n = a.count();
	if ( data->hasHfw && !horz(dir) ) {
	    for ( int i = 0; i < n; i++ ) {
		TQBoxLayoutItem *box = data->list.at( i );
		if ( box->item->hasHeightForWidth() )
		    a[i].sizeHint = a[i].minimumSize =
				    box->item->heightForWidth( s.width() );
	    }
	}

	Direction visualDir = dir;
	if ( TQApplication::reverseLayout() ) {
	    if ( dir == LeftToRight )
		visualDir = RightToLeft;
	    else if ( dir == RightToLeft )
		visualDir = LeftToRight;
	}

	qGeomCalc( a, 0, n, pos, space, spacing() );
	for ( int i = 0; i < n; i++ ) {
	    TQBoxLayoutItem *box = data->list.at( i );

	    switch ( visualDir ) {
	    case LeftToRight:
		box->item->setGeometry( TQRect(a[i].pos, s.y(),
					      a[i].size, s.height()) );
		break;
	    case RightToLeft:
		box->item->setGeometry( TQRect(s.left() + s.right()
					      - a[i].pos - a[i].size + 1, s.y(),
					      a[i].size, s.height()) );
		break;
	    case TopToBottom:
		box->item->setGeometry( TQRect(s.x(), a[i].pos,
					      s.width(), a[i].size) );
		break;
	    case BottomToTop:
		box->item->setGeometry( TQRect(s.x(), s.top() + s.bottom()
					      - a[i].pos - a[i].size + 1,
					      s.width(), a[i].size) );
	    }
	}
    }
}

/*!
    Adds \a item to the end of this box layout.
*/
void TQBoxLayout::addItem( TQLayoutItem *item )
{
    TQBoxLayoutItem *it = new TQBoxLayoutItem( item );
    data->list.append( it );
    invalidate();
}

/*!
    Inserts \a item into this box layout at position \a index. If \a
    index is negative, the item is added at the end.

    \warning Does not call TQLayout::insertChildLayout() if \a item is
    a TQLayout.

    \sa addItem(), findWidget()
*/
void TQBoxLayout::insertItem( int index, TQLayoutItem *item )
{
    if ( index < 0 )				// append
	index = data->list.count();

    TQBoxLayoutItem *it = new TQBoxLayoutItem( item );
    data->list.insert( index, it );
    invalidate();
}

/*!
    Inserts a non-stretchable space at position \a index, with size \a
    size. If \a index is negative the space is added at the end.

    The box layout has default margin and spacing. This function adds
    additional space.

    \sa insertStretch()
*/
void TQBoxLayout::insertSpacing( int index, int size )
{
    if ( index < 0 )				// append
	index = data->list.count();

    // hack in TQGridLayoutData: spacers do not get insideSpacing
    TQLayoutItem *b;
    if ( horz( dir ) )
	b = new TQSpacerItem( size, 0, TQSizePolicy::Fixed,
			     TQSizePolicy::Minimum );
    else
	b = new TQSpacerItem( 0, size, TQSizePolicy::Minimum,
			     TQSizePolicy::Fixed );

    TQBoxLayoutItem *it = new TQBoxLayoutItem( b );
    it->magic = true;
    data->list.insert( index, it );
    invalidate();
}

/*!
    Inserts a stretchable space at position \a index, with zero
    minimum size and stretch factor \a stretch. If \a index is
    negative the space is added at the end.

    \sa insertSpacing()
*/
void TQBoxLayout::insertStretch( int index, int stretch )
{
    if ( index < 0 )				// append
	index = data->list.count();

    // hack in TQGridLayoutData: spacers do not get insideSpacing
    TQLayoutItem *b;
    if ( horz( dir ) )
	b = new TQSpacerItem( 0, 0, TQSizePolicy::Expanding,
			     TQSizePolicy::Minimum );
    else
	b = new TQSpacerItem( 0, 0,  TQSizePolicy::Minimum,
			     TQSizePolicy::Expanding );

    TQBoxLayoutItem *it = new TQBoxLayoutItem( b, stretch );
    it->magic = true;
    data->list.insert( index, it );
    invalidate();
}

/*!
    Inserts \a layout at position \a index, with stretch factor \a
    stretch. If \a index is negative, the layout is added at the end.

    \a layout becomes a child of the box layout.

    \sa setAutoAdd(), insertWidget(), insertSpacing()
*/
void TQBoxLayout::insertLayout( int index, TQLayout *layout, int stretch )
{
    if ( index < 0 )				// append
	index = data->list.count();

    addChildLayout( layout );
    TQBoxLayoutItem *it = new TQBoxLayoutItem( layout, stretch );
    data->list.insert( index, it );
    invalidate();
}

/*!
    Inserts \a widget at position \a index, with stretch factor \a
    stretch and alignment \a alignment. If \a index is negative, the
    widget is added at the end.

    The stretch factor applies only in the \link direction() direction
    \endlink of the TQBoxLayout, and is relative to the other boxes and
    widgets in this TQBoxLayout. Widgets and boxes with higher stretch
    factors grow more.

    If the stretch factor is 0 and nothing else in the TQBoxLayout has
    a stretch factor greater than zero, the space is distributed
    according to the TQWidget:sizePolicy() of each widget that's
    involved.

    Alignment is specified by \a alignment, which is a bitwise OR of
    \l TQt::AlignmentFlags values. The default alignment is 0, which
    means that the widget fills the entire cell.

    From TQt 3.0, the \a alignment parameter is interpreted more
    aggressively than in previous versions of TQt. A non-default
    alignment now indicates that the widget should not grow to fill
    the available space, but should be sized according to sizeHint().

    \sa setAutoAdd(), insertLayout(), insertSpacing()
*/
void TQBoxLayout::insertWidget( int index, TQWidget *widget, int stretch,
			       int alignment )
{
    if ( !checkWidget(this, widget) )
	 return;

    if ( index < 0 )				// append
	index = data->list.count();

    TQWidgetItem *b = new TQWidgetItem( widget );
    b->setAlignment( alignment );
    TQBoxLayoutItem *it = new TQBoxLayoutItem( b, stretch );
    data->list.insert( index, it );
    invalidate();
}

/*!
    Adds a non-stretchable space with size \a size to the end of this
    box layout. TQBoxLayout provides default margin and spacing. This
    function adds additional space.

    \sa insertSpacing(), addStretch()
*/
void TQBoxLayout::addSpacing( int size )
{
    insertSpacing( -1, size );
}

/*!
    Adds a stretchable space with zero minimum size and stretch factor
    \a stretch to the end of this box layout.

    \sa addSpacing()
*/
void TQBoxLayout::addStretch( int stretch )
{
    insertStretch( -1, stretch );
}

/*!
    Adds \a widget to the end of this box layout, with a stretch
    factor of \a stretch and alignment \a alignment.

    The stretch factor applies only in the \link direction() direction
    \endlink of the TQBoxLayout, and is relative to the other boxes and
    widgets in this TQBoxLayout. Widgets and boxes with higher stretch
    factors grow more.

    If the stretch factor is 0 and nothing else in the TQBoxLayout has
    a stretch factor greater than zero, the space is distributed
    according to the TQWidget:sizePolicy() of each widget that's
    involved.

    Alignment is specified by \a alignment which is a bitwise OR of \l
    TQt::AlignmentFlags values. The default alignment is 0, which means
    that the widget fills the entire cell.

    From TQt 3.0, the \a alignment parameter is interpreted more
    aggressively than in previous versions of TQt. A non-default
    alignment now indicates that the widget should not grow to fill
    the available space, but should be sized according to sizeHint().

    \sa insertWidget(), setAutoAdd(), addLayout(), addSpacing()
*/
void TQBoxLayout::addWidget( TQWidget *widget, int stretch,
			    int alignment )
{
    insertWidget( -1, widget, stretch, alignment );
}

/*!
    Adds \a layout to the end of the box, with serial stretch factor
    \a stretch.

    When a layout is constructed with another layout as its parent,
    you don't need to call addLayout(); the child layout is
    automatically added to the parent layout as it is constructed.

    \sa insertLayout(), setAutoAdd(), addWidget(), addSpacing()
*/
void TQBoxLayout::addLayout( TQLayout *layout, int stretch )
{
    insertLayout( -1, layout, stretch );
}

/*!
    Limits the perpendicular dimension of the box (e.g. height if the
    box is LeftToRight) to a minimum of \a size. Other constraints may
    increase the limit.
*/
void TQBoxLayout::addStrut( int size )
{
    TQLayoutItem *b;
    if ( horz( dir ) )
	b = new TQSpacerItem( 0, size, TQSizePolicy::Fixed,
			     TQSizePolicy::Minimum );
    else
	b = new TQSpacerItem( size, 0, TQSizePolicy::Minimum,
			     TQSizePolicy::Fixed );

    TQBoxLayoutItem *it = new TQBoxLayoutItem( b );
    it->magic = true;
    data->list.append( it );
    invalidate();
}

/*!
    Searches for widget \a w in this layout (not including child
    layouts).

    Returns the index of \a w, or -1 if \a w is not found.
*/
int TQBoxLayout::findWidget( TQWidget* w )
{
    const int n = data->list.count();
    for ( int i = 0; i < n; i++ ) {
	if ( data->list.at(i)->item->widget() == w )
	    return i;
    }
    return -1;
}

/*!
    Sets the stretch factor for widget \a w to \a stretch and returns
    true if \a w is found in this layout (not including child
    layouts); otherwise returns false.
*/
bool TQBoxLayout::setStretchFactor( TQWidget *w, int stretch )
{
    TQPtrListIterator<TQBoxLayoutItem> it( data->list );
    TQBoxLayoutItem *box;
    while ( (box=it.current()) != 0 ) {
	++it;
	if ( box->item->widget() == w ) {
	    box->stretch = stretch;
	    invalidate();
	    return true;
	}
    }
    return false;
}

/*!
  \overload

  Sets the stretch factor for the layout \a l to \a stretch and
  returns true if \a l is found in this layout (not including child
  layouts); otherwise returns false.
*/
bool TQBoxLayout::setStretchFactor( TQLayout *l, int stretch )
{
    TQPtrListIterator<TQBoxLayoutItem> it( data->list );
    TQBoxLayoutItem *box;
    while ( (box=it.current()) != 0 ) {
	++it;
	if ( box->item->layout() == l ) {
	    box->stretch = stretch;
	    invalidate();
	    return true;
	}
    }
    return false;
}

/*!
    Sets the direction of this layout to \a direction.
*/
void TQBoxLayout::setDirection( Direction direction )
{
    if ( dir == direction )
	return;
    if ( horz(dir) != horz(direction) ) {
	//swap around the spacers (the "magic" bits)
	//#### a bit yucky, knows too much.
	//#### probably best to add access functions to spacerItem
	//#### or even a TQSpacerItem::flip()
	TQPtrListIterator<TQBoxLayoutItem> it( data->list );
	TQBoxLayoutItem *box;
	while ( (box=it.current()) != 0 ) {
	    ++it;
	    if ( box->magic ) {
		TQSpacerItem *sp = box->item->spacerItem();
		if ( sp ) {
		    if ( sp->expanding() == TQSizePolicy::NoDirection ) {
			//spacing or strut
			TQSize s = sp->sizeHint();
			sp->changeSize( s.height(), s.width(),
			    horz(direction) ? TQSizePolicy::Fixed:TQSizePolicy::Minimum,
			    horz(direction) ? TQSizePolicy::Minimum:TQSizePolicy::Fixed );

		    } else {
			//stretch
			if ( horz(direction) )
			    sp->changeSize( 0, 0, TQSizePolicy::Expanding,
					    TQSizePolicy::Minimum );
			else
			    sp->changeSize( 0, 0, TQSizePolicy::Minimum,
					    TQSizePolicy::Expanding );
		    }
		}
	    }
	}
    }
    dir = direction;
    invalidate();
    if ( mainWidget() ) {
	TQEvent *lh = new TQEvent( TQEvent::LayoutHint );
	TQApplication::postEvent( mainWidget(), lh );
    }

}

/*
    Initializes the data structure needed by qGeomCalc and
    recalculates max/min and size hint.
*/
void TQBoxLayout::setupGeom()
{
    if ( !data->dirty )
	return;

    int maxw = horz( dir ) ? 0 : TQLAYOUTSIZE_MAX;
    int maxh = horz( dir ) ? TQLAYOUTSIZE_MAX : 0;
    int minw = 0;
    int minh = 0;
    int hintw = 0;
    int hinth = 0;

    bool horexp = false;
    bool verexp = false;

    data->hasHfw = false;

    delete data->geomArray;
    int n = data->list.count();
    data->geomArray = new TQMemArray<TQLayoutStruct>( n );
    TQMemArray<TQLayoutStruct>& a = *data->geomArray;

    bool first = true;
    for ( int i = 0; i < n; i++ ) {
	TQBoxLayoutItem *box = data->list.at( i );
	TQSize max = box->item->maximumSize();
	TQSize min = box->item->minimumSize();
	TQSize hint = box->item->sizeHint();
	TQSizePolicy::ExpandData exp = box->item->expanding();
	bool empty = box->item->isEmpty();
	// space before non-empties, except the first:
	int space = ( empty || first ) ? 0 : spacing();
	bool ignore = empty && box->item->widget(); // ignore hidden widgets

	if ( horz(dir) ) {
	    bool expand = exp & TQSizePolicy::Horizontally || box->stretch > 0;
	    horexp = horexp || expand;
	    maxw += max.width() + space;
	    minw += min.width() + space;
	    hintw += hint.width() + space;
	    if ( !ignore )
		qMaxExpCalc( maxh, verexp,
			     max.height(), exp & TQSizePolicy::Vertically );
	    minh = TQMAX( minh, min.height() );
	    hinth = TQMAX( hinth, hint.height() );

	    a[i].sizeHint = hint.width();
	    a[i].maximumSize = max.width();
	    a[i].minimumSize = min.width();
	    a[i].expansive = expand;
	    a[i].stretch = box->stretch ? box->stretch : box->hStretch();
	} else {
	    bool expand = ( exp & TQSizePolicy::Vertically || box->stretch > 0 );
	    verexp = verexp || expand;
	    maxh += max.height() + space;
	    minh += min.height() + space;
	    hinth += hint.height() + space;
	    if ( !ignore )
		qMaxExpCalc( maxw, horexp,
			     max.width(), exp & TQSizePolicy::Horizontally );
	    minw = TQMAX( minw, min.width() );
	    hintw = TQMAX( hintw, hint.width() );

	    a[i].sizeHint = hint.height();
	    a[i].maximumSize = max.height();
	    a[i].minimumSize = min.height();
	    a[i].expansive = expand;
	    a[i].stretch = box->stretch ? box->stretch : box->vStretch();
	}

	a[i].empty = empty;
	data->hasHfw = data->hasHfw || box->item->hasHeightForWidth();
	first = first && empty;
    }

    data->minSize = TQSize( minw, minh );
    data->maxSize = TQSize( maxw, maxh ).expandedTo( data->minSize );

    data->expanding = (TQSizePolicy::ExpandData)
		       ( (horexp ? TQSizePolicy::Horizontally : 0)
			 | (verexp ? TQSizePolicy::Vertically : 0) );

    data->sizeHint = TQSize( hintw, hinth )
		     .expandedTo( data->minSize )
		     .boundedTo( data->maxSize );

    data->dirty = false;
}

/*
  Calculates and stores the preferred height given the width \a w.
*/
void TQBoxLayout::calcHfw( int w )
{
    int h = 0;
    int mh = 0;

    if ( horz(dir) ) {
	TQMemArray<TQLayoutStruct> &a = *data->geomArray;
	int n = a.count();
	qGeomCalc( a, 0, n, 0, w, spacing() );
	for ( int i = 0; i < n; i++ ) {
	    TQBoxLayoutItem *box = data->list.at(i);
	    h = TQMAX( h, box->hfw(a[i].size) );
	    mh = TQMAX( mh, box->mhfw(a[i].size) );
	}
    } else {
	TQPtrListIterator<TQBoxLayoutItem> it( data->list );
	TQBoxLayoutItem *box;
	bool first = true;
	while ( (box = it.current()) != 0 ) {
	    ++it;
	    bool empty = box->item->isEmpty();
	    h += box->hfw( w );
	    mh += box->mhfw( w );
	    if ( !first && !empty ) {
		h += spacing();
		mh += spacing();
	    }
	    first = first && empty;
	}
    }
    data->hfwWidth = w;
    data->hfwHeight = h;
    data->hfwMinHeight = mh;
}

/*!
    \fn TQBoxLayout::Direction TQBoxLayout::direction() const

    Returns the direction of the box. addWidget() and addSpacing()
    work in this direction; the stretch stretches in this direction.

    \sa TQBoxLayout::Direction addWidget() addSpacing()
*/

/*!
    \class TQHBoxLayout
    \brief The TQHBoxLayout class lines up widgets horizontally.

    \ingroup geomanagement
    \ingroup appearance
    \mainclass

    This class is used to construct horizontal box layout objects. See
    \l TQBoxLayout for more details.

    The simplest use of the class is like this:
    \code
	TQBoxLayout * l = new TQHBoxLayout( widget );
	l->setAutoAdd( true );
	new TQSomeWidget( widget );
	new TQSomeOtherWidget( widget );
	new TQAnotherWidget( widget );
    \endcode

    or like this:
    \code
	TQBoxLayout * l = new TQHBoxLayout( widget );
	l->addWidget( existingChildOfWidget );
	l->addWidget( anotherChildOfWidget );
    \endcode

    \img qhboxlayout.png TQHBox

    \sa TQVBoxLayout TQGridLayout
	\link layout.html the Layout overview \endlink
*/

/*!
    Constructs a new top-level horizontal box called \a name, with
    parent \a parent.

    The \a margin is the number of pixels between the edge of the
    widget and its managed children. The \a spacing is the default
    number of pixels between neighboring children. If \a spacing is -1
    the value of \a margin is used for \a spacing.
*/
TQHBoxLayout::TQHBoxLayout( TQWidget *parent, int margin,
			  int spacing, const char *name )
    : TQBoxLayout( parent, LeftToRight, margin, spacing, name )
{
}

/*!
    Constructs a new horizontal box called name \a name and adds it to
    \a parentLayout.

    The \a spacing is the default number of pixels between neighboring
    children. If \a spacing is -1, this TQHBoxLayout will inherit its
    parent's spacing().
*/
TQHBoxLayout::TQHBoxLayout( TQLayout *parentLayout, int spacing,
			  const char *name )
    : TQBoxLayout( parentLayout, LeftToRight, spacing, name )
{
}

/*!
    Constructs a new horizontal box called name \a name. You must add
    it to another layout.

    The \a spacing is the default number of pixels between neighboring
    children. If \a spacing is -1, this TQHBoxLayout will inherit its
    parent's spacing().
*/
TQHBoxLayout::TQHBoxLayout( int spacing, const char *name )
    : TQBoxLayout( LeftToRight, spacing, name )
{
}

/*!
    Destroys this box layout.

    The layout's widgets aren't destroyed.
*/
TQHBoxLayout::~TQHBoxLayout()
{
}

/*!
    \class TQVBoxLayout

    \brief The TQVBoxLayout class lines up widgets vertically.

    \ingroup geomanagement
    \ingroup appearance
    \mainclass

    This class is used to construct vertical box layout objects. See
    TQBoxLayout for more details.

    The simplest use of the class is like this:
    \code
	TQBoxLayout * l = new TQVBoxLayout( widget );
	l->addWidget( aWidget );
	l->addWidget( anotherWidget );
    \endcode

    \img qvboxlayout.png TQVBox

    \sa TQHBoxLayout TQGridLayout \link layout.html the Layout overview \endlink
*/

/*!
    Constructs a new top-level vertical box called \a name, with
    parent \a parent.

    The \a margin is the number of pixels between the edge of the
    widget and its managed children. The \a spacing is the default
    number of pixels between neighboring children. If \a spacing is -1
    the value of \a margin is used for \a spacing.
*/
TQVBoxLayout::TQVBoxLayout( TQWidget *parent, int margin, int spacing,
			  const char *name )
    : TQBoxLayout( parent, TopToBottom, margin, spacing, name )
{

}

/*!
    Constructs a new vertical box called name \a name and adds it to
    \a parentLayout.

    The \a spacing is the default number of pixels between neighboring
    children. If \a spacing is -1, this TQVBoxLayout will inherit its
    parent's spacing().
*/
TQVBoxLayout::TQVBoxLayout( TQLayout *parentLayout, int spacing,
			  const char *name )
    : TQBoxLayout( parentLayout, TopToBottom, spacing, name )
{
}

/*!
    Constructs a new vertical box called name \a name. You must add
    it to another layout.

    The \a spacing is the default number of pixels between neighboring
    children. If \a spacing is -1, this TQVBoxLayout will inherit its
    parent's spacing().
*/
TQVBoxLayout::TQVBoxLayout( int spacing, const char *name )
    : TQBoxLayout( TopToBottom, spacing, name )
{
}

/*!
    Destroys this box layout.

    The layout's widgets aren't destroyed.
*/
TQVBoxLayout::~TQVBoxLayout()
{
}

TQBoxLayout *TQBoxLayout::createTmpCopy()
{
    TQBoxLayout *bl = new TQBoxLayout( direction() );
    delete bl->data;
    bl->data = data;
    return bl;
}

#endif // TQT_NO_LAYOUT
