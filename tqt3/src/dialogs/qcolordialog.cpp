/****************************************************************************
**
** Implementation of TQColorDialog class
**
** Created : 990222
**
** Copyright (C) 1999-2008 Trolltech ASA.  All rights reserved.
**
** This file is part of the dialogs module of the TQt GUI Toolkit.
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

#include "ntqcolordialog.h"

#ifndef TQT_NO_COLORDIALOG

#include "ntqpainter.h"
#include "ntqlayout.h"
#include "ntqlabel.h"
#include "ntqpushbutton.h"
#include "ntqlineedit.h"
#include "ntqimage.h"
#include "ntqpixmap.h"
#include "ntqdrawutil.h"
#include "ntqvalidator.h"
#include "ntqdragobject.h"
#include "ntqgridview.h"
#include "ntqapplication.h"
#include "ntqstyle.h"
#include "ntqsettings.h"
#include "ntqpopupmenu.h"

#ifdef TQ_WS_MAC
TQRgb macGetRgba( TQRgb initial, bool *ok, TQWidget *parent, const char* name );
TQColor macGetColor( const TQColor& initial, TQWidget *parent, const char *name );
#endif

#ifdef TQ_WS_X11
#include "private/tqttdeintegration_x11_p.h"
#endif

//////////// TQWellArray BEGIN

struct TQWellArrayData;

class TQWellArray : public TQGridView
{
    TQ_OBJECT
    TQ_PROPERTY( int selectedColumn READ selectedColumn )
    TQ_PROPERTY( int selectedRow READ selectedRow )

public:
    TQWellArray( TQWidget* parent=0, const char* name=0, bool popup = false );

    ~TQWellArray() {}
    TQString cellContent( int row, int col ) const;
    // ### Paul !!! virtual void setCellContent( int row, int col, const TQString &);

    int selectedColumn() const { return selCol; }
    int selectedRow() const { return selRow; }

    virtual void setCurrent( int row, int col );
    virtual void setSelected( int row, int col );

    TQSize sizeHint() const;

    virtual void setCellBrush( int row, int col, const TQBrush & );
    TQBrush cellBrush( int row, int col );

signals:
    void selected( int row, int col );

protected:
    void dimensionChange( int oldRows, int oldCols );

    virtual void paintCell( TQPainter *, int row, int col );
    virtual void paintCellContents( TQPainter *, int row, int col, const TQRect& );

    void mousePressEvent( TQMouseEvent* );
    void mouseReleaseEvent( TQMouseEvent* );
    void mouseMoveEvent( TQMouseEvent* );
    void keyPressEvent( TQKeyEvent* );
    void focusInEvent( TQFocusEvent* );
    void focusOutEvent( TQFocusEvent* );

private:
    int curRow;
    int curCol;
    int selRow;
    int selCol;
    bool smallStyle;
    TQWellArrayData *d;

private:	// Disabled copy constructor and operator=
#if defined(TQ_DISABLE_COPY)
    TQWellArray( const TQWellArray & );
    TQWellArray& operator=( const TQWellArray & );
#endif
};



// non-interface ...



struct TQWellArrayData {
    TQBrush *brush;
};

/*!
    \internal
    \class TQWellArray qwellarray_p.h
    \brief The TQWellArray class provides a well array.


    \ingroup advanced
*/

TQWellArray::TQWellArray( TQWidget *parent, const char * name, bool popup )
    : TQGridView( parent, name,
		 (popup ? (WStyle_Customize|WStyle_Tool|WStyle_NoBorder) : 0 ) )
{
    d = 0;
    setFocusPolicy( StrongFocus );
    setVScrollBarMode(AlwaysOff);
    setHScrollBarMode(AlwaysOff);
    viewport()->setBackgroundMode( PaletteBackground );
    setNumCols( 7 );
    setNumRows( 7 );
    setCellWidth( 24 );
    setCellHeight( 21 );
    smallStyle = popup;

    if ( popup ) {
	setCellWidth( 18 );
	setCellHeight( 18 );
	setFrameStyle(TQFrame::StyledPanel | TQFrame::Raised);
	setMargin( 1 );
	setLineWidth( 2 );
    } else {
	setFrameStyle( TQFrame::NoFrame );
    }
    curCol = 0;
    curRow = 0;
    selCol = -1;
    selRow = -1;

    if ( smallStyle )
	setMouseTracking( true );
}


TQSize TQWellArray::sizeHint() const
{
    constPolish();
    TQSize s = gridSize().boundedTo( TQSize(640, 480 ) );
    return TQSize( s.width() + 2*frameWidth(), s.height() + 2*frameWidth() );
}


void TQWellArray::paintCell( TQPainter* p, int row, int col )
{
    int w = cellWidth();			// width of cell in pixels
    int h = cellHeight();			// height of cell in pixels
    int b = 1;

    if ( !smallStyle )
	b = 3;

    const TQColorGroup & g = colorGroup();
    p->setPen( TQPen( black, 0, SolidLine ) );
    if ( !smallStyle && row ==selRow && col == selCol &&
	 style().styleHint(TQStyle::SH_GUIStyle) != MotifStyle) {
	int n = 2;
	p->drawRect( n, n, w-2*n, h-2*n );
    }

    style().drawPrimitive(TQStyle::PE_Panel, p, TQRect(b, b, w-2*b, h-2*b), g,
			  TQStyle::Style_Enabled | TQStyle::Style_Sunken);

    int t = 0;
    if (style().styleHint(TQStyle::SH_GUIStyle) == MotifStyle)
	t = ( row == selRow && col == selCol ) ? 2 : 0;
    b += 2 + t;

    if ( (row == curRow) && (col == curCol) ) {
	if ( smallStyle ) {
	    p->setPen ( white );
	    p->drawRect( 1, 1, w-2, h-2 );
	    p->setPen ( black );
	    p->drawRect( 0, 0, w, h );
	    p->drawRect( 2, 2, w-4, h-4 );
	    b = 3;
	} else if ( hasFocus() ) {
	    style().drawPrimitive(TQStyle::PE_FocusRect, p, TQRect(0, 0, w, h), g);
	}
    }
    paintCellContents( p, row, col, TQRect(b, b, w - 2*b, h - 2*b) );
}

/*!
  Reimplement this function to change the contents of the well array.
 */
void TQWellArray::paintCellContents( TQPainter *p, int row, int col, const TQRect &r )
{

    if ( d ) {
	p->fillRect( r, d->brush[row*numCols()+col] );
    } else {
	p->fillRect( r, white );
	p->setPen( black );
	p->drawLine( r.topLeft(), r.bottomRight() );
	p->drawLine( r.topRight(), r.bottomLeft() );
    }
}


/*\reimp
*/
void TQWellArray::mousePressEvent( TQMouseEvent* e )
{
    // The current cell marker is set to the cell the mouse is pressed
    // in.
    TQPoint pos = e->pos();
    setCurrent( rowAt( pos.y() ), columnAt( pos.x() ) );
}

/*\reimp
*/
void TQWellArray::mouseReleaseEvent( TQMouseEvent* )
{
    // The current cell marker is set to the cell the mouse is clicked
    // in.
    setSelected( curRow, curCol );
}


/*\reimp
*/
void TQWellArray::mouseMoveEvent( TQMouseEvent* e )
{
    //   The current cell marker is set to the cell the mouse is
    //   clicked in.
    if ( smallStyle ) {
	TQPoint pos = e->pos();
	setCurrent( rowAt( pos.y() ), columnAt( pos.x() ) );
    }
}

/*
  Sets the cell currently having the focus. This is not necessarily
  the same as the currently selected cell.
*/

void TQWellArray::setCurrent( int row, int col )
{

    if ( (curRow == row) && (curCol == col) )
	return;

    if ( row < 0 || col < 0 )
	row = col = -1;

    int oldRow = curRow;
    int oldCol = curCol;

    curRow = row;
    curCol = col;

    updateCell( oldRow, oldCol );
    updateCell( curRow, curCol );
}


/*!
  Sets the currently selected cell to \a row, \a col.  If \a row or \a
  col are less than zero, the current cell is unselected.

  Does not set the position of the focus indicator.
*/

void TQWellArray::setSelected( int row, int col )
{
    if ( (selRow == row) && (selCol == col) )
	return;

    int oldRow = selRow;
    int oldCol = selCol;

    if ( row < 0 || col < 0 )
	row = col = -1;

    selCol = col;
    selRow = row;

    updateCell( oldRow, oldCol );
    updateCell( selRow, selCol );
    if ( row >= 0 )
	emit selected( row, col );

    if ( isVisible() && ::tqt_cast<TQPopupMenu*>(parentWidget()) )
	parentWidget()->close();
}



/*!\reimp
*/
void TQWellArray::focusInEvent( TQFocusEvent* )
{
    updateCell( curRow, curCol );
}


/*!
  Sets the size of the well array to be \a rows cells by \a cols.
  Resets any brush information set by setCellBrush().
 */
void TQWellArray::dimensionChange( int, int )
{
    if ( d ) {
	if ( d->brush )
	    delete[] d->brush;
	delete d;
	d = 0;
    }
}

void TQWellArray::setCellBrush( int row, int col, const TQBrush &b )
{
    if ( !d ) {
	d = new TQWellArrayData;
	int i = numRows()*numCols();
	d->brush = new TQBrush[i];
    }
    if ( row >= 0 && row < numRows() && col >= 0 && col < numCols() )
	d->brush[row*numCols()+col] = b;
#ifdef QT_CHECK_RANGE
    else
	tqWarning( "TQWellArray::setCellBrush( %d, %d ) out of range", row, col );
#endif
}



/*!
  Returns the brush set for the cell at \a row, \a col. If no brush is set,
  \c NoBrush is returned.
*/

TQBrush TQWellArray::cellBrush( int row, int col )
{
    if ( d && row >= 0 && row < numRows() && col >= 0 && col < numCols() )
	return d->brush[row*numCols()+col];
    return NoBrush;
}



/*!\reimp
*/

void TQWellArray::focusOutEvent( TQFocusEvent* )
{
    updateCell( curRow, curCol );
}

/*\reimp
*/
void TQWellArray::keyPressEvent( TQKeyEvent* e )
{
    switch( e->key() ) {			// Look at the key code
    case Key_Left:				// If 'left arrow'-key,
	if( curCol > 0 )			// and cr't not in leftmost col
	    setCurrent( curRow, curCol - 1);	// set cr't to next left column
	break;
    case Key_Right:				// Correspondingly...
	if( curCol < numCols()-1 )
	    setCurrent( curRow, curCol + 1);
	break;
    case Key_Up:
	if( curRow > 0 )
	    setCurrent( curRow - 1, curCol);
	else if ( smallStyle )
	    focusNextPrevChild( false );
	break;
    case Key_Down:
	if( curRow < numRows()-1 )
	    setCurrent( curRow + 1, curCol);
	else if ( smallStyle )
	    focusNextPrevChild( true );
	break;
    case Key_Space:
    case Key_Return:
    case Key_Enter:
	setSelected( curRow, curCol );
	break;
    default:				// If not an interesting key,
	e->ignore();			// we don't accept the event
	return;
    }

}

//////////// TQWellArray END

static bool initrgb = false;
static TQRgb stdrgb[6*8];
static TQRgb cusrgb[2*8];
static bool customSet = false;


static void initRGB()
{
    if ( initrgb )
	return;
    initrgb = true;
    int i = 0;
    for ( int g = 0; g < 4; g++ )
	for ( int r = 0;  r < 4; r++ )
	    for ( int b = 0; b < 3; b++ )
		stdrgb[i++] = tqRgb( r*255/3, g*255/3, b*255/2 );

    for ( i = 0; i < 2*8; i++ )
	cusrgb[i] = tqRgb(0xff,0xff,0xff);
}

/*!
    Returns the number of custom colors supported by TQColorDialog. All
    color dialogs share the same custom colors.
*/
int TQColorDialog::customCount()
{
    return 2*8;
}

/*!
    Returns custom color number \a i as a TQRgb.
*/
TQRgb TQColorDialog::customColor( int i )
{
    initRGB();
    if ( i < 0 || i >= customCount() ) {
#ifdef QT_CHECK_RANGE
	tqWarning( "TQColorDialog::customColor() index %d out of range", i );
#endif
	i = 0;
    }
    return cusrgb[i];
}

/*!
    Sets custom color number \a i to the TQRgb value \a c.
*/
void TQColorDialog::setCustomColor( int i, TQRgb c )
{
    initRGB();
    if ( i < 0 || i >= customCount() ) {
#ifdef QT_CHECK_RANGE
	tqWarning( "TQColorDialog::setCustomColor() index %d out of range", i );
#endif
	return;
    }
    customSet = true;
    cusrgb[i] = c;
}

/*!
    Sets standard color number \a i to the TQRgb value \a c.
*/

void TQColorDialog::setStandardColor( int i, TQRgb c )
{
    initRGB();
    if ( i < 0 || i >= 6*8 ) {
#ifdef QT_CHECK_RANGE
	tqWarning( "TQColorDialog::setStandardColor() index %d out of range", i );
#endif
	return;
    }
    stdrgb[i] = c;
}

static inline void rgb2hsv( TQRgb rgb, int&h, int&s, int&v )
{
    TQColor c;
    c.setRgb( rgb );
    c.getHsv(h,s,v);
}

class TQColorWell : public TQWellArray
{
public:
    TQColorWell( TQWidget *parent, int r, int c, TQRgb *vals )
	:TQWellArray( parent, "" ), values( vals ), mousePressed( false ), oldCurrent( -1, -1 )
    { setNumRows(r), setNumCols(c); setSizePolicy( TQSizePolicy( TQSizePolicy::Minimum, TQSizePolicy::Minimum) ); }

protected:
    void paintCellContents( TQPainter *, int row, int col, const TQRect& );
    void mousePressEvent( TQMouseEvent *e );
    void mouseMoveEvent( TQMouseEvent *e );
    void mouseReleaseEvent( TQMouseEvent *e );
#ifndef TQT_NO_DRAGANDDROP
    void dragEnterEvent( TQDragEnterEvent *e );
    void dragLeaveEvent( TQDragLeaveEvent *e );
    void dragMoveEvent( TQDragMoveEvent *e );
    void dropEvent( TQDropEvent *e );
#endif

private:
    TQRgb *values;
    bool mousePressed;
    TQPoint pressPos;
    TQPoint oldCurrent;

};

void TQColorWell::paintCellContents( TQPainter *p, int row, int col, const TQRect &r )
{
    int i = row + col*numRows();
    p->fillRect( r, TQColor( values[i] ) );
}

void TQColorWell::mousePressEvent( TQMouseEvent *e )
{
    oldCurrent = TQPoint( selectedRow(), selectedColumn() );
    TQWellArray::mousePressEvent( e );
    mousePressed = true;
    pressPos = e->pos();
}

void TQColorWell::mouseMoveEvent( TQMouseEvent *e )
{
    TQWellArray::mouseMoveEvent( e );
#ifndef TQT_NO_DRAGANDDROP
    if ( !mousePressed )
	return;
    if ( ( pressPos - e->pos() ).manhattanLength() > TQApplication::startDragDistance() ) {
	setCurrent( oldCurrent.x(), oldCurrent.y() );
	int i = rowAt(pressPos.y()) + columnAt(pressPos.x()) * numRows();
	TQColor col( values[ i ] );
	TQColorDrag *drg = new TQColorDrag( col, this );
	TQPixmap pix( cellWidth(), cellHeight() );
	pix.fill( col );
	TQPainter p( &pix );
	p.drawRect( 0, 0, pix.width(), pix.height() );
	p.end();
	drg->setPixmap( pix );
	mousePressed = false;
	drg->dragCopy();
    }
#endif
}

#ifndef TQT_NO_DRAGANDDROP
void TQColorWell::dragEnterEvent( TQDragEnterEvent *e )
{
    setFocus();
    if ( TQColorDrag::canDecode( e ) )
	e->accept();
    else
	e->ignore();
}

void TQColorWell::dragLeaveEvent( TQDragLeaveEvent * )
{
    if ( hasFocus() )
	parentWidget()->setFocus();
}

void TQColorWell::dragMoveEvent( TQDragMoveEvent *e )
{
    if ( TQColorDrag::canDecode( e ) ) {
	setCurrent( rowAt( e->pos().y() ), columnAt( e->pos().x() ) );
	e->accept();
    } else
	e->ignore();
}

void TQColorWell::dropEvent( TQDropEvent *e )
{
    if ( TQColorDrag::canDecode( e ) ) {
	int i = rowAt( e->pos().y() ) + columnAt( e->pos().x() ) * numRows();
	TQColor col;
	TQColorDrag::decode( e, col );
	values[ i ] = col.rgb();
	repaintContents( false );
	e->accept();
    } else {
	e->ignore();
    }
}

#endif // TQT_NO_DRAGANDDROP

void TQColorWell::mouseReleaseEvent( TQMouseEvent *e )
{
    if ( !mousePressed )
	return;
    TQWellArray::mouseReleaseEvent( e );
    mousePressed = false;
}

class TQColorPicker : public TQFrame
{
    TQ_OBJECT
public:
    TQColorPicker(TQWidget* parent=0, const char* name=0);
    ~TQColorPicker();

public slots:
    void setCol( int h, int s );

signals:
    void newCol( int h, int s );

protected:
    TQSize sizeHint() const;
    void drawContents(TQPainter* p);
    void mouseMoveEvent( TQMouseEvent * );
    void mousePressEvent( TQMouseEvent * );

private:
    int hue;
    int sat;

    TQPoint colPt();
    int huePt( const TQPoint &pt );
    int satPt( const TQPoint &pt );
    void setCol( const TQPoint &pt );

    TQPixmap *pix;
};

static int pWidth = 200;
static int pHeight = 200;

class TQColorLuminancePicker : public TQWidget
{
    TQ_OBJECT
public:
    TQColorLuminancePicker(TQWidget* parent=0, const char* name=0);
    ~TQColorLuminancePicker();

public slots:
    void setCol( int h, int s, int v );
    void setCol( int h, int s );

signals:
    void newHsv( int h, int s, int v );

protected:
    void paintEvent( TQPaintEvent*);
    void mouseMoveEvent( TQMouseEvent * );
    void mousePressEvent( TQMouseEvent * );

private:
    enum { foff = 3, coff = 4 }; //frame and contents offset
    int val;
    int hue;
    int sat;

    int y2val( int y );
    int val2y( int val );
    void setVal( int v );

    TQPixmap *pix;
};


int TQColorLuminancePicker::y2val( int y )
{
    int d = height() - 2*coff - 1;
    return 255 - (y - coff)*255/d;
}

int TQColorLuminancePicker::val2y( int v )
{
    int d = height() - 2*coff - 1;
    return coff + (255-v)*d/255;
}

TQColorLuminancePicker::TQColorLuminancePicker(TQWidget* parent,
						  const char* name)
    :TQWidget( parent, name )
{
    hue = 100; val = 100; sat = 100;
    pix = 0;
    //    setBackgroundMode( NoBackground );
}

TQColorLuminancePicker::~TQColorLuminancePicker()
{
    delete pix;
}

void TQColorLuminancePicker::mouseMoveEvent( TQMouseEvent *m )
{
    setVal( y2val(m->y()) );
}
void TQColorLuminancePicker::mousePressEvent( TQMouseEvent *m )
{
    setVal( y2val(m->y()) );
}

void TQColorLuminancePicker::setVal( int v )
{
    if ( val == v )
	return;
    val = TQMAX( 0, TQMIN(v,255));
    delete pix; pix=0;
    repaint( false ); //###
    emit newHsv( hue, sat, val );
}

//receives from a hue,sat chooser and relays.
void TQColorLuminancePicker::setCol( int h, int s )
{
    setCol( h, s, val );
    emit newHsv( h, s, val );
}

void TQColorLuminancePicker::paintEvent( TQPaintEvent * )
{
    int w = width() - 5;

    TQRect r( 0, foff, w, height() - 2*foff );
    int wi = r.width() - 2;
    int hi = r.height() - 2;
    if ( !pix || pix->height() != hi || pix->width() != wi ) {
	delete pix;
	TQImage img( wi, hi, 32 );
	int y;
	for ( y = 0; y < hi; y++ ) {
	    TQColor c( hue, sat, y2val(y+coff), TQColor::Hsv );
	    TQRgb r = c.rgb();
	    int x;
	    for ( x = 0; x < wi; x++ )
		img.setPixel( x, y, r );
	}
	pix = new TQPixmap;
	pix->convertFromImage(img);
    }
    TQPainter p(this);
    p.drawPixmap( 1, coff, *pix );
    const TQColorGroup &g = colorGroup();
    qDrawShadePanel( &p, r, g, true );
    p.setPen( g.foreground() );
    p.setBrush( g.foreground() );
    TQPointArray a;
    int y = val2y(val);
    a.setPoints( 3, w, y, w+5, y+5, w+5, y-5 );
    erase( w, 0, 5, height() );
    p.drawPolygon( a );
}

void TQColorLuminancePicker::setCol( int h, int s , int v )
{
    val = v;
    hue = h;
    sat = s;
    delete pix; pix=0;
    repaint( false );//####
}

TQPoint TQColorPicker::colPt()
{ return TQPoint( (360-hue)*(pWidth-1)/360, (255-sat)*(pHeight-1)/255 ); }
int TQColorPicker::huePt( const TQPoint &pt )
{ return 360 - pt.x()*360/(pWidth-1); }
int TQColorPicker::satPt( const TQPoint &pt )
{ return 255 - pt.y()*255/(pHeight-1) ; }
void TQColorPicker::setCol( const TQPoint &pt )
{ setCol( huePt(pt), satPt(pt) ); }

TQColorPicker::TQColorPicker(TQWidget* parent, const char* name )
    : TQFrame( parent, name )
{
    hue = 0; sat = 0;
    setCol( 150, 255 );

    TQImage img( pWidth, pHeight, 32 );
    int x,y;
    for ( y = 0; y < pHeight; y++ )
	for ( x = 0; x < pWidth; x++ ) {
	    TQPoint p( x, y );
	    img.setPixel( x, y, TQColor(huePt(p), satPt(p),
				       200, TQColor::Hsv).rgb() );
	}
    pix = new TQPixmap;
    pix->convertFromImage(img);
    setBackgroundMode( NoBackground );
    setSizePolicy( TQSizePolicy( TQSizePolicy::Fixed, TQSizePolicy::Fixed )  );
}

TQColorPicker::~TQColorPicker()
{
    delete pix;
}

TQSize TQColorPicker::sizeHint() const
{
    return TQSize( pWidth + 2*frameWidth(), pHeight + 2*frameWidth() );
}

void TQColorPicker::setCol( int h, int s )
{
    int nhue = TQMIN( TQMAX(0,h), 360 );
    int nsat = TQMIN( TQMAX(0,s), 255);
    if ( nhue == hue && nsat == sat )
	return;
    TQRect r( colPt(), TQSize(20,20) );
    hue = nhue; sat = nsat;
    r = r.unite( TQRect( colPt(), TQSize(20,20) ) );
    r.moveBy( contentsRect().x()-9, contentsRect().y()-9 );
    //    update( r );
    repaint( r, false );
}

void TQColorPicker::mouseMoveEvent( TQMouseEvent *m )
{
    TQPoint p = m->pos() - contentsRect().topLeft();
    setCol( p );
    emit newCol( hue, sat );
}

void TQColorPicker::mousePressEvent( TQMouseEvent *m )
{
    TQPoint p = m->pos() - contentsRect().topLeft();
    setCol( p );
    emit newCol( hue, sat );
}

void TQColorPicker::drawContents(TQPainter* p)
{
    TQRect r = contentsRect();

    p->drawPixmap( r.topLeft(), *pix );
    TQPoint pt = colPt() + r.topLeft();
    p->setPen( TQPen(black) );

    p->fillRect( pt.x()-9, pt.y(), 20, 2, black );
    p->fillRect( pt.x(), pt.y()-9, 2, 20, black );

}

class TQColorShowLabel;



class TQColIntValidator: public TQIntValidator
{
public:
    TQColIntValidator( int bottom, int top,
		   TQWidget * parent, const char *name = 0 )
	:TQIntValidator( bottom, top, parent, name ) {}

    TQValidator::State validate( TQString &, int & ) const;
};

TQValidator::State TQColIntValidator::validate( TQString &s, int &pos ) const
{
    State state = TQIntValidator::validate(s,pos);
    if ( state == Valid ) {
	long int val = s.toLong();
	// This is not a general solution, assumes that top() > 0 and
	// bottom >= 0
	if ( val < 0 ) {
	    s = "0";
	    pos = 1;
	} else if ( val > top() ) {
	    s.setNum( top() );
	    pos = s.length();
	}
    }
    return state;
}



class TQColNumLineEdit : public TQLineEdit
{
public:
    TQColNumLineEdit( TQWidget *parent, const char* name=0 )
	: TQLineEdit( parent, name ) { setMaxLength( 3 );}
    TQSize sizeHint() const {
	return TQSize( fontMetrics().width( "999" ) + 2 * ( margin() + frameWidth() ),
		      TQLineEdit::sizeHint().height() ); }
    void setNum( int i ) {
	TQString s;
	s.setNum(i);
	bool block = signalsBlocked();
	blockSignals(true);
	setText( s );
	blockSignals(block);
    }
    int val() const { return text().toInt(); }
};


class TQColorShower : public TQWidget
{
    TQ_OBJECT
public:
    TQColorShower( TQWidget *parent, const char *name=0 );

    //things that don't emit signals
    void setHsv( int h, int s, int v );

    int currentAlpha() const { return alphaEd->val(); }
    void setCurrentAlpha( int a ) { alphaEd->setNum( a ); }
    void showAlpha( bool b );


    TQRgb currentColor() const { return curCol; }

public slots:
    void setRgb( TQRgb rgb );

signals:
    void newCol( TQRgb rgb );
private slots:
    void rgbEd();
    void hsvEd();
private:
    void showCurrentColor();
    int hue, sat, val;
    TQRgb curCol;
    TQColNumLineEdit *hEd;
    TQColNumLineEdit *sEd;
    TQColNumLineEdit *vEd;
    TQColNumLineEdit *rEd;
    TQColNumLineEdit *gEd;
    TQColNumLineEdit *bEd;
    TQColNumLineEdit *alphaEd;
    TQLabel *alphaLab;
    TQColorShowLabel *lab;
    bool rgbOriginal;
};

class TQColorShowLabel : public TQFrame
{
    TQ_OBJECT

public:
    TQColorShowLabel( TQWidget *parent ) : TQFrame( parent, "qt_colorshow_lbl" ) {
	setFrameStyle( TQFrame::Panel|TQFrame::Sunken );
	setBackgroundMode( PaletteBackground );
	setAcceptDrops( true );
	mousePressed = false;
    }
    void setColor( TQColor c ) { col = c; }

signals:
    void colorDropped( TQRgb );

protected:
    void drawContents( TQPainter *p );
    void mousePressEvent( TQMouseEvent *e );
    void mouseMoveEvent( TQMouseEvent *e );
    void mouseReleaseEvent( TQMouseEvent *e );
#ifndef TQT_NO_DRAGANDDROP
    void dragEnterEvent( TQDragEnterEvent *e );
    void dragLeaveEvent( TQDragLeaveEvent *e );
    void dropEvent( TQDropEvent *e );
#endif

private:
    TQColor col;
    bool mousePressed;
    TQPoint pressPos;

};

void TQColorShowLabel::drawContents( TQPainter *p )
{
    p->fillRect( contentsRect(), col );
}

void TQColorShower::showAlpha( bool b )
{
    if ( b ) {
	alphaLab->show();
	alphaEd->show();
    } else {
	alphaLab->hide();
	alphaEd->hide();
    }
}

void TQColorShowLabel::mousePressEvent( TQMouseEvent *e )
{
    mousePressed = true;
    pressPos = e->pos();
}

void TQColorShowLabel::mouseMoveEvent( TQMouseEvent *e )
{
#ifndef TQT_NO_DRAGANDDROP
    if ( !mousePressed )
	return;
    if ( ( pressPos - e->pos() ).manhattanLength() > TQApplication::startDragDistance() ) {
	TQColorDrag *drg = new TQColorDrag( col, this );
	TQPixmap pix( 30, 20 );
	pix.fill( col );
	TQPainter p( &pix );
	p.drawRect( 0, 0, pix.width(), pix.height() );
	p.end();
	drg->setPixmap( pix );
	mousePressed = false;
	drg->dragCopy();
    }
#endif
}

#ifndef TQT_NO_DRAGANDDROP
void TQColorShowLabel::dragEnterEvent( TQDragEnterEvent *e )
{
    if ( TQColorDrag::canDecode( e ) )
	e->accept();
    else
	e->ignore();
}

void TQColorShowLabel::dragLeaveEvent( TQDragLeaveEvent * )
{
}

void TQColorShowLabel::dropEvent( TQDropEvent *e )
{
    if ( TQColorDrag::canDecode( e ) ) {
	TQColorDrag::decode( e, col );
	repaint( false );
	emit colorDropped( col.rgb() );
	e->accept();
    } else {
	e->ignore();
    }
}
#endif // TQT_NO_DRAGANDDROP

void TQColorShowLabel::mouseReleaseEvent( TQMouseEvent * )
{
    if ( !mousePressed )
	return;
    mousePressed = false;
}

TQColorShower::TQColorShower( TQWidget *parent, const char *name )
    :TQWidget( parent, name)
{
    curCol = tqRgb( -1, -1, -1 );
    TQColIntValidator *val256 = new TQColIntValidator( 0, 255, this );
    TQColIntValidator *val360 = new TQColIntValidator( 0, 360, this );

    TQGridLayout *gl = new TQGridLayout( this, 1, 1, 6 );
    lab = new TQColorShowLabel( this );
    lab->setMinimumWidth( 60 ); //###
    gl->addMultiCellWidget(lab, 0,-1,0,0);
    connect( lab, TQ_SIGNAL( colorDropped(TQRgb) ),
	     this, TQ_SIGNAL( newCol(TQRgb) ) );
    connect( lab, TQ_SIGNAL( colorDropped(TQRgb) ),
	     this, TQ_SLOT( setRgb(TQRgb) ) );

    hEd = new TQColNumLineEdit( this, "qt_hue_edit" );
    hEd->setValidator( val360 );
    TQLabel *l = new TQLabel( hEd, TQColorDialog::tr("Hu&e:"), this, "qt_hue_lbl" );
    l->setAlignment( AlignRight|AlignVCenter );
    gl->addWidget( l, 0, 1 );
    gl->addWidget( hEd, 0, 2 );

    sEd = new TQColNumLineEdit( this, "qt_sat_edit" );
    sEd->setValidator( val256 );
    l = new TQLabel( sEd, TQColorDialog::tr("&Sat:"), this, "qt_sat_lbl" );
    l->setAlignment( AlignRight|AlignVCenter );
    gl->addWidget( l, 1, 1 );
    gl->addWidget( sEd, 1, 2 );

    vEd = new TQColNumLineEdit( this, "qt_val_edit" );
    vEd->setValidator( val256 );
    l = new TQLabel( vEd, TQColorDialog::tr("&Val:"), this, "qt_val_lbl" );
    l->setAlignment( AlignRight|AlignVCenter );
    gl->addWidget( l, 2, 1 );
    gl->addWidget( vEd, 2, 2 );

    rEd = new TQColNumLineEdit( this, "qt_red_edit" );
    rEd->setValidator( val256 );
    l = new TQLabel( rEd, TQColorDialog::tr("&Red:"), this, "qt_red_lbl" );
    l->setAlignment( AlignRight|AlignVCenter );
    gl->addWidget( l, 0, 3 );
    gl->addWidget( rEd, 0, 4 );

    gEd = new TQColNumLineEdit( this, "qt_grn_edit" );
    gEd->setValidator( val256 );
    l = new TQLabel( gEd, TQColorDialog::tr("&Green:"), this, "qt_grn_lbl" );
    l->setAlignment( AlignRight|AlignVCenter );
    gl->addWidget( l, 1, 3 );
    gl->addWidget( gEd, 1, 4 );

    bEd = new TQColNumLineEdit( this, "qt_blue_edit" );
    bEd->setValidator( val256 );
    l = new TQLabel( bEd, TQColorDialog::tr("Bl&ue:"), this, "qt_blue_lbl" );
    l->setAlignment( AlignRight|AlignVCenter );
    gl->addWidget( l, 2, 3 );
    gl->addWidget( bEd, 2, 4 );

    alphaEd = new TQColNumLineEdit( this, "qt_aplha_edit" );
    alphaEd->setValidator( val256 );
    alphaLab = new TQLabel( alphaEd, TQColorDialog::tr("A&lpha channel:"), this, "qt_alpha_lbl" );
    alphaLab->setAlignment( AlignRight|AlignVCenter );
    gl->addMultiCellWidget( alphaLab, 3, 3, 1, 3 );
    gl->addWidget( alphaEd, 3, 4 );
    alphaEd->hide();
    alphaLab->hide();

    connect( hEd, TQ_SIGNAL(textChanged(const TQString&)), this, TQ_SLOT(hsvEd()) );
    connect( sEd, TQ_SIGNAL(textChanged(const TQString&)), this, TQ_SLOT(hsvEd()) );
    connect( vEd, TQ_SIGNAL(textChanged(const TQString&)), this, TQ_SLOT(hsvEd()) );

    connect( rEd, TQ_SIGNAL(textChanged(const TQString&)), this, TQ_SLOT(rgbEd()) );
    connect( gEd, TQ_SIGNAL(textChanged(const TQString&)), this, TQ_SLOT(rgbEd()) );
    connect( bEd, TQ_SIGNAL(textChanged(const TQString&)), this, TQ_SLOT(rgbEd()) );
    connect( alphaEd, TQ_SIGNAL(textChanged(const TQString&)), this, TQ_SLOT(rgbEd()) );
}

void TQColorShower::showCurrentColor()
{
    lab->setColor( currentColor() );
    lab->repaint(false); //###
}

void TQColorShower::rgbEd()
{
    rgbOriginal = true;
    if ( alphaEd->isVisible() )
	curCol = tqRgba( rEd->val(), gEd->val(), bEd->val(), currentAlpha() );
    else
	curCol = tqRgb( rEd->val(), gEd->val(), bEd->val() );

    rgb2hsv(currentColor(), hue, sat, val );

    hEd->setNum( hue );
    sEd->setNum( sat );
    vEd->setNum( val );

    showCurrentColor();
    emit newCol( currentColor() );
}

void TQColorShower::hsvEd()
{
    rgbOriginal = false;
    hue = hEd->val();
    sat = sEd->val();
    val = vEd->val();

    curCol = TQColor( hue, sat, val, TQColor::Hsv ).rgb();

    rEd->setNum( tqRed(currentColor()) );
    gEd->setNum( tqGreen(currentColor()) );
    bEd->setNum( tqBlue(currentColor()) );

    showCurrentColor();
    emit newCol( currentColor() );
}

void TQColorShower::setRgb( TQRgb rgb )
{
    rgbOriginal = true;
    curCol = rgb;

    rgb2hsv( currentColor(), hue, sat, val );

    hEd->setNum( hue );
    sEd->setNum( sat );
    vEd->setNum( val );

    rEd->setNum( tqRed(currentColor()) );
    gEd->setNum( tqGreen(currentColor()) );
    bEd->setNum( tqBlue(currentColor()) );

    showCurrentColor();
}

void TQColorShower::setHsv( int h, int s, int v )
{
    rgbOriginal = false;
    hue = h; val = v; sat = s; //Range check###
    curCol = TQColor( hue, sat, val, TQColor::Hsv ).rgb();

    hEd->setNum( hue );
    sEd->setNum( sat );
    vEd->setNum( val );

    rEd->setNum( tqRed(currentColor()) );
    gEd->setNum( tqGreen(currentColor()) );
    bEd->setNum( tqBlue(currentColor()) );

    showCurrentColor();
}

class TQColorDialogPrivate : public TQObject
{
TQ_OBJECT
public:
    TQColorDialogPrivate( TQColorDialog *p );
    TQRgb currentColor() const { return cs->currentColor(); }
    void setCurrentColor( TQRgb rgb );

    int currentAlpha() const { return cs->currentAlpha(); }
    void setCurrentAlpha( int a ) { cs->setCurrentAlpha( a ); }
    void showAlpha( bool b ) { cs->showAlpha( b ); }

public slots:
    void addCustom();

    void newHsv( int h, int s, int v );
    void newColorTypedIn( TQRgb rgb );
    void newCustom( int, int );
    void newStandard( int, int );
public:
    TQWellArray *custom;
    TQWellArray *standard;

    TQColorPicker *cp;
    TQColorLuminancePicker *lp;
    TQColorShower *cs;
    int nextCust;
    bool compact;
};

//sets all widgets to display h,s,v
void TQColorDialogPrivate::newHsv( int h, int s, int v )
{
    cs->setHsv( h, s, v );
    cp->setCol( h, s );
    lp->setCol( h, s, v );
}

//sets all widgets to display rgb
void TQColorDialogPrivate::setCurrentColor( TQRgb rgb )
{
    cs->setRgb( rgb );
    newColorTypedIn( rgb );
}

//sets all widgets exept cs to display rgb
void TQColorDialogPrivate::newColorTypedIn( TQRgb rgb )
{
    int h, s, v;
    rgb2hsv(rgb, h, s, v );
    cp->setCol( h, s );
    lp->setCol( h, s, v);
}

void TQColorDialogPrivate::newCustom( int r, int c )
{
    int i = r+2*c;
    setCurrentColor( cusrgb[i] );
    nextCust = i;
    if (standard)
        standard->setSelected(-1,-1);
}

void TQColorDialogPrivate::newStandard( int r, int c )
{
    setCurrentColor( stdrgb[r+c*6] );
    if (custom)
        custom->setSelected(-1,-1);
}

TQColorDialogPrivate::TQColorDialogPrivate( TQColorDialog *dialog ) :
    TQObject(dialog)
{
    compact = false;
    // small displays (e.g. PDAs cannot fit the full color dialog,
    // so just use the color picker.
    if ( tqApp->desktop()->width() < 480 || tqApp->desktop()->height() < 350 )
	compact = true;

    nextCust = 0;
    const int lumSpace = 3;
    int border = 12;
    if ( compact )
	border = 6;
    TQHBoxLayout *topLay = new TQHBoxLayout( dialog, border, 6 );
    TQVBoxLayout *leftLay = 0;

    if ( !compact )
	leftLay = new TQVBoxLayout( topLay );

    initRGB();

    if ( !compact ) {
	standard = new TQColorWell( dialog, 6, 8, stdrgb );
	standard->setCellWidth( 28 );
	standard->setCellHeight( 24 );
	TQLabel * lab = new TQLabel( standard,
				TQColorDialog::tr( "&Basic colors"), dialog, "qt_basiccolors_lbl" );
	connect( standard, TQ_SIGNAL(selected(int,int)), TQ_SLOT(newStandard(int,int)));
	leftLay->addWidget( lab );
	leftLay->addWidget( standard );


	leftLay->addStretch();

	custom = new TQColorWell( dialog, 2, 8, cusrgb );
	custom->setCellWidth( 28 );
	custom->setCellHeight( 24 );
	custom->setAcceptDrops( true );

	connect( custom, TQ_SIGNAL(selected(int,int)), TQ_SLOT(newCustom(int,int)));
	lab = new TQLabel( custom, TQColorDialog::tr( "&Custom colors") , dialog, "qt_custcolors_lbl" );
	leftLay->addWidget( lab );
	leftLay->addWidget( custom );

	TQPushButton *custbut =
	    new TQPushButton( TQColorDialog::tr("&Define Custom Colors >>"),
						dialog, "qt_def_custcolors_lbl" );
	custbut->setEnabled( false );
	leftLay->addWidget( custbut );
    } else {
	// better color picker size for small displays
	pWidth = 150;
	pHeight = 100;

        custom = 0;
        standard = 0;
    }

    TQVBoxLayout *rightLay = new TQVBoxLayout( topLay );

    TQHBoxLayout *pickLay = new TQHBoxLayout( rightLay );


    TQVBoxLayout *cLay = new TQVBoxLayout( pickLay );
    cp = new TQColorPicker( dialog, "qt_colorpicker" );
    cp->setFrameStyle( TQFrame::Panel + TQFrame::Sunken );
    cLay->addSpacing( lumSpace );
    cLay->addWidget( cp );
    cLay->addSpacing( lumSpace );

    lp = new TQColorLuminancePicker( dialog, "qt_luminance_picker" );
    lp->setFixedWidth( 20 ); //###
    pickLay->addWidget( lp );

    connect( cp, TQ_SIGNAL(newCol(int,int)), lp, TQ_SLOT(setCol(int,int)) );
    connect( lp, TQ_SIGNAL(newHsv(int,int,int)), this, TQ_SLOT(newHsv(int,int,int)) );

    rightLay->addStretch();

    cs = new TQColorShower( dialog, "qt_colorshower" );
    connect( cs, TQ_SIGNAL(newCol(TQRgb)), this, TQ_SLOT(newColorTypedIn(TQRgb)));
    rightLay->addWidget( cs );

    TQHBoxLayout *buttons;
    if ( compact )
	buttons = new TQHBoxLayout( rightLay );
    else
	buttons = new TQHBoxLayout( leftLay );

    TQPushButton *ok, *cancel;
    ok = new TQPushButton( TQColorDialog::tr("OK"), dialog, "qt_ok_btn" );
    connect( ok, TQ_SIGNAL(clicked()), dialog, TQ_SLOT(accept()) );
    ok->setDefault(true);
    cancel = new TQPushButton( TQColorDialog::tr("Cancel"), dialog, "qt_cancel_btn" );
    connect( cancel, TQ_SIGNAL(clicked()), dialog, TQ_SLOT(reject()) );
    buttons->addWidget( ok );
    buttons->addWidget( cancel );
    buttons->addStretch();

    if ( !compact ) {
	TQPushButton *addCusBt = new TQPushButton(
					TQColorDialog::tr("&Add to Custom Colors"),
						 dialog, "qt_add_btn" );
	rightLay->addWidget( addCusBt );
	connect( addCusBt, TQ_SIGNAL(clicked()), this, TQ_SLOT(addCustom()) );
    }
}

void TQColorDialogPrivate::addCustom()
{
    cusrgb[nextCust] = cs->currentColor();
    if (custom)
        custom->repaintContents( false );
    nextCust = (nextCust+1) % 16;
}


/*!
    \class TQColorDialog ntqcolordialog.h
    \brief The TQColorDialog class provides a dialog widget for specifying colors.
    \mainclass
    \ingroup dialogs
    \ingroup graphics

    The color dialog's function is to allow users to choose colors.
    For example, you might use this in a drawing program to allow the
    user to set the brush color.

    The static functions provide modal color dialogs.
    \omit
    If you require a modeless dialog, use the TQColorDialog constructor.
    \endomit

    The static getColor() function shows the dialog and allows the
    user to specify a color. The getRgba() function does the same but
    also allows the user to specify a color with an alpha channel
    (transparency) value.

    The user can store customCount() different custom colors. The
    custom colors are shared by all color dialogs, and remembered
    during the execution of the program. Use setCustomColor() to set
    the custom colors, and use customColor() to get them.

    \img qcolordlg-w.png
*/

/*!
    Constructs a default color dialog with parent \a parent and called
    \a name. If \a modal is true the dialog will be modal. Use
    setColor() to set an initial value.

    \sa getColor()
*/

TQColorDialog::TQColorDialog(TQWidget* parent, const char* name, bool modal) :
    TQDialog(parent, name, modal, (  WType_Dialog | WStyle_Customize | WStyle_Title |
                                    WStyle_DialogBorder | WStyle_SysMenu ) )
{
    setSizeGripEnabled( false );
    d = new TQColorDialogPrivate( this );

#ifndef TQT_NO_SETTINGS
    if ( !customSet ) {
	TQSettings settings;
	settings.insertSearchPath( TQSettings::Windows, "/Trolltech" );
	for ( int i = 0; i < 2*8; ++i ) {
	    bool ok = false;
	    TQRgb rgb = (TQRgb)settings.readNumEntry( "/TQt/customColors/" + TQString::number( i ), 0, &ok );
	    if ( ok )
		cusrgb[i] = rgb;
	}
    }
#endif
}

/*!
    Pops up a modal color dialog, lets the user choose a color, and
    returns that color. The color is initially set to \a initial. The
    dialog is a child of \a parent and is called \a name. It returns
    an invalid (see TQColor::isValid()) color if the user cancels the
    dialog. All colors allocated by the dialog will be deallocated
    before this function returns.
*/

TQColor TQColorDialog::getColor( const TQColor& initial, TQWidget *parent,
			       const char *name )
{
#if defined(TQ_WS_X11)
    if( TQTDEIntegration::enabled())
        return TQTDEIntegration::getColor( initial, parent, name );
#elif defined(TQ_WS_MAC)
    return macGetColor(initial, parent, name);
#endif

    int allocContext = TQColor::enterAllocContext();
    TQColorDialog *dlg = new TQColorDialog( parent, name, true );  //modal
#ifndef TQT_NO_WIDGET_TOPEXTRA
    dlg->setCaption( TQColorDialog::tr( "Select color" ) );
#endif
    dlg->setColor( initial );
    dlg->selectColor( initial );
    int resultCode = dlg->exec();
    TQColor::leaveAllocContext();
    TQColor result;
    if ( resultCode == TQDialog::Accepted )
	result = dlg->color();
    TQColor::destroyAllocContext(allocContext);
    delete dlg;
    return result;
}


/*!
    Pops up a modal color dialog to allow the user to choose a color
    and an alpha channel (transparency) value. The color+alpha is
    initially set to \a initial. The dialog is a child of \a parent
    and called \a name.

    If \a ok is non-null, \e *\a ok is set to true if the user clicked
    OK, and to false if the user clicked Cancel.

    If the user clicks Cancel, the \a initial value is returned.
*/

TQRgb TQColorDialog::getRgba( TQRgb initial, bool *ok,
			    TQWidget *parent, const char* name )
{
#if defined(TQ_WS_MAC)
    if( TQTDEIntegration::enabled()) {
        TQColor color = TQTDEIntegration::getColor( TQColor( initial ), parent, name );
        if( ok )
            *ok = color.isValid();
        return color.rgba();
    }
#elif defined(TQ_WS_MAC)
    return macGetRgba(initial, ok, parent, name);
#endif

    int allocContext = TQColor::enterAllocContext();
    TQColorDialog *dlg = new TQColorDialog( parent, name, true );  //modal

    TQ_CHECK_PTR( dlg );
#ifndef TQT_NO_WIDGET_TOPEXTRA
    dlg->setCaption( TQColorDialog::tr( "Select color" ) );
#endif
    dlg->setColor( initial );
    dlg->selectColor( initial );
    dlg->setSelectedAlpha( tqAlpha(initial) );
    int resultCode = dlg->exec();
    TQColor::leaveAllocContext();
    TQRgb result = initial;
    if ( resultCode == TQDialog::Accepted ) {
	TQRgb c = dlg->color().rgb();
	int alpha = dlg->selectedAlpha();
	result = tqRgba( tqRed(c), tqGreen(c), tqBlue(c), alpha );
    }
    if ( ok )
	*ok = resultCode == TQDialog::Accepted;

    TQColor::destroyAllocContext(allocContext);
    delete dlg;
    return result;
}





/*!
    Returns the color currently selected in the dialog.

    \sa setColor()
*/

TQColor TQColorDialog::color() const
{
    return TQColor(d->currentColor());
}


/*!
    Destroys the dialog and frees any memory it allocated.
*/

TQColorDialog::~TQColorDialog()
{
#ifndef TQT_NO_SETTINGS
    if ( !customSet ) {
	TQSettings settings;
	settings.insertSearchPath( TQSettings::Windows, "/Trolltech" );
	for ( int i = 0; i < 2*8; ++i )
	    settings.writeEntry( "/TQt/customColors/" + TQString::number( i ), (int)cusrgb[i] );
    }
#endif
}


/*!
    Sets the color shown in the dialog to \a c.

    \sa color()
*/

void TQColorDialog::setColor( const TQColor& c )
{
    d->setCurrentColor( c.rgb() );
}




/*!
    Sets the initial alpha channel value to \a a, and shows the alpha
    channel entry box.
*/

void TQColorDialog::setSelectedAlpha( int a )
{
    d->showAlpha( true );
    d->setCurrentAlpha( a );
}


/*!
    Returns the value selected for the alpha channel.
*/

int TQColorDialog::selectedAlpha() const
{
    return d->currentAlpha();
}

/*!
    Sets focus to the corresponding button, if any.
*/
bool TQColorDialog::selectColor( const TQColor& col )
{
    TQRgb color = col.rgb();
    int i = 0, j = 0;
    // Check standard colors
    if (d->standard) {
        for ( i = 0; i < 6; i++ ) {
            for ( j = 0; j < 8; j++ ) {
                if ( color == stdrgb[i + j*6] ) {
                    d->newStandard( i, j );
                    d->standard->setCurrent( i, j );
                    d->standard->setSelected( i, j );
                    d->standard->setFocus();
                    return true;
                }
            }
        }
    }
    // Check custom colors
    if (d->custom) {
        for ( i = 0; i < 2; i++ ) {
            for ( j = 0; j < 8; j++ ) {
                if ( color == cusrgb[i + j*2] ) {
                    d->newCustom( i, j );
                    d->custom->setCurrent( i, j );
                    d->custom->setSelected( i, j );
                    d->custom->setFocus();
                    return true;
                }
            }
        }
    }
    return false;
}

#include "qcolordialog.moc"

#endif
