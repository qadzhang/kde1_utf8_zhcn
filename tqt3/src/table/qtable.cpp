/****************************************************************************
**
** Implementation of TQTable widget class
**
** Created : 000607
**
** Copyright (C) 1992-2008 Trolltech ASA.  All rights reserved.
**
** This file is part of the table module of the TQt GUI Toolkit.
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

#include "ntqglobal.h"
#include "ntqtable.h"

#ifndef TQT_NO_TABLE

#include <ntqpainter.h>
#include <ntqlineedit.h>
#include <ntqcursor.h>
#include <ntqapplication.h>
#include <ntqtimer.h>
#include <ntqobjectlist.h>
#include <ntqiconset.h>
#include <ntqcombobox.h>
#include <ntqcheckbox.h>
#include <ntqdragobject.h>
#include <ntqevent.h>
#include <ntqlistbox.h>
#include <ntqstyle.h>
#include <ntqdatatable.h>
#include <ntqvalidator.h>

#include <stdlib.h>
#include <limits.h>

static bool qt_update_cell_widget = true;
static bool qt_table_clipper_enabled = true;
#ifndef QT_INTERNAL_TABLE
TQ_EXPORT
#endif
void tqt_set_table_clipper_enabled( bool enabled )
{
    qt_table_clipper_enabled = enabled;
}

class TQM_EXPORT_TABLE TQTableHeader : public TQHeader
{
    friend class TQTable;
    TQ_OBJECT

public:
    enum SectionState {
	Normal,
	Bold,
	Selected
    };

    TQTableHeader( int, TQTable *t, TQWidget* parent=0, const char* name=0 );
    ~TQTableHeader() {};
    void addLabel( const TQString &s, int size );
    void setLabel( int section, const TQString & s, int size = -1 );
    void setLabel( int section, const TQIconSet & iconset, const TQString & s,
		   int size = -1 );

    void setLabels(const TQStringList & labels);

    void removeLabel( int section );

    void setSectionState( int s, SectionState state );
    void setSectionStateToAll( SectionState state );
    SectionState sectionState( int s ) const;

    int sectionSize( int section ) const;
    int sectionPos( int section ) const;
    int sectionAt( int section ) const;

    void setSectionStretchable( int s, bool b );
    bool isSectionStretchable( int s ) const;

    void updateCache();

signals:
    void sectionSizeChanged( int s );

protected:
    void paintEvent( TQPaintEvent *e );
    void paintSection( TQPainter *p, int index, const TQRect& fr );
    void mousePressEvent( TQMouseEvent *e );
    void mouseMoveEvent( TQMouseEvent *e );
    void mouseReleaseEvent( TQMouseEvent *e );
    void mouseDoubleClickEvent( TQMouseEvent *e );
    void resizeEvent( TQResizeEvent *e );

private slots:
    void doAutoScroll();
    void sectionWidthChanged( int col, int os, int ns );
    void indexChanged( int sec, int oldIdx, int newIdx );
    void updateStretches();
    void updateWidgetStretches();

private:
    void updateSelections();
    void saveStates();
    void setCaching( bool b );
    void swapSections( int oldIdx, int newIdx, bool swapTable = true );
    bool doSelection( TQMouseEvent *e );
    void sectionLabelChanged( int section );
    void resizeArrays( int n );

private:
    TQMemArray<int> states, oldStates;
    TQMemArray<bool> stretchable;
    TQMemArray<int> sectionSizes, sectionPoses;
    bool mousePressed;
    int pressPos, startPos, endPos;
    TQTable *table;
    TQTimer *autoScrollTimer;
    TQWidget *line1, *line2;
    bool caching;
    int resizedSection;
    bool isResizing;
    int numStretches;
    TQTimer *stretchTimer, *widgetStretchTimer;
    TQTableHeaderPrivate *d;

};

#ifdef _WS_QWS_
# define NO_LINE_WIDGET
#endif



struct TQTablePrivate
{
    TQTablePrivate() : hasRowSpan( false ), hasColSpan( false ),
		      inMenuMode( false ), redirectMouseEvent( false )
    {
	hiddenRows.setAutoDelete( true );
	hiddenCols.setAutoDelete( true );
    }
    uint hasRowSpan : 1;
    uint hasColSpan : 1;
    uint inMenuMode : 1;
    uint redirectMouseEvent : 1;
    TQIntDict<int> hiddenRows, hiddenCols;
    TQTimer *geomTimer;
    int lastVisRow;
    int lastVisCol;
};

struct TQTableHeaderPrivate
{
#ifdef NO_LINE_WIDGET
    int oldLinePos;
#endif
};

static bool isRowSelection( TQTable::SelectionMode selMode )
{
    return selMode == TQTable::SingleRow || selMode == TQTable::MultiRow;
}

/*!
    \class TQTableSelection
    \brief The TQTableSelection class provides access to a selected area in a
    TQTable.
\if defined(commercial)
    It is part of the <a href="commercialeditions.html">TQt Enterprise Edition</a>.
\endif

    \ingroup advanced
    \module table

    The selection is a rectangular set of cells in a TQTable. One of
    the rectangle's cells is called the anchor cell; this is the cell
    that was selected first. The init() function sets the anchor and
    the selection rectangle to exactly this cell; the expandTo()
    function expands the selection rectangle to include additional
    cells.

    There are various access functions to find out about the area:
    anchorRow() and anchorCol() return the anchor's position;
    leftCol(), rightCol(), topRow() and bottomRow() return the
    rectangle's four edges. All four are part of the selection.

    A newly created TQTableSelection is inactive -- isActive() returns
    false. You must use init() and expandTo() to activate it.

    \sa TQTable TQTable::addSelection() TQTable::selection()
    TQTable::selectCells() TQTable::selectRow() TQTable::selectColumn()
*/

/*!
    Creates an inactive selection. Use init() and expandTo() to
    activate it.
*/

TQTableSelection::TQTableSelection()
    : active( false ), inited( false ), tRow( -1 ), lCol( -1 ),
      bRow( -1 ), rCol( -1 ), aRow( -1 ), aCol( -1 )
{
}

/*!
    Creates an active selection, starting at \a start_row and \a
    start_col, ending at \a end_row and \a end_col.
*/

TQTableSelection::TQTableSelection( int start_row, int start_col, int end_row, int end_col )
    : active( false ), inited( false ), tRow( -1 ), lCol( -1 ),
      bRow( -1 ), rCol( -1 ), aRow( -1 ), aCol( -1 )
{
    init( start_row, start_col );
    expandTo( end_row, end_col );
}

/*!
    Sets the selection anchor to cell \a row, \a col and the selection
    to only contain this cell. The selection is not active until
    expandTo() is called.

    To extend the selection to include additional cells, call
    expandTo().

    \sa isActive()
*/

void TQTableSelection::init( int row, int col )
{
    aCol = lCol = rCol = col;
    aRow = tRow = bRow = row;
    active = false;
    inited = true;
}

/*!
    Expands the selection to include cell \a row, \a col. The new
    selection rectangle is the bounding rectangle of \a row, \a col
    and the previous selection rectangle. After calling this function
    the selection is active.

    If you haven't called init(), this function does nothing.

    \sa init() isActive()
*/

void TQTableSelection::expandTo( int row, int col )
{
    if ( !inited )
	return;
    active = true;

    if ( row < aRow ) {
	tRow = row;
	bRow = aRow;
    } else {
	tRow = aRow;
	bRow = row;
    }

    if ( col < aCol ) {
	lCol = col;
	rCol = aCol;
    } else {
	lCol = aCol;
	rCol = col;
    }
}

/*!
    Returns true if \a s includes the same cells as the selection;
    otherwise returns false.
*/

bool TQTableSelection::operator==( const TQTableSelection &s ) const
{
    return ( s.active == active &&
	     s.tRow == tRow && s.bRow == bRow &&
	     s.lCol == lCol && s.rCol == rCol );
}

/*!
    \fn bool TQTableSelection::operator!=( const TQTableSelection &s ) const

    Returns true if \a s does not include the same cells as the
    selection; otherwise returns false.
*/


/*!
    \fn int TQTableSelection::topRow() const

    Returns the top row of the selection.

    \sa bottomRow() leftCol() rightCol()
*/

/*!
    \fn int TQTableSelection::bottomRow() const

    Returns the bottom row of the selection.

    \sa topRow() leftCol() rightCol()
*/

/*!
    \fn int TQTableSelection::leftCol() const

    Returns the left column of the selection.

    \sa topRow() bottomRow() rightCol()
*/

/*!
    \fn int TQTableSelection::rightCol() const

    Returns the right column of the selection.

    \sa topRow() bottomRow() leftCol()
*/

/*!
    \fn int TQTableSelection::anchorRow() const

    Returns the anchor row of the selection.

    \sa anchorCol() expandTo()
*/

/*!
    \fn int TQTableSelection::anchorCol() const

    Returns the anchor column of the selection.

    \sa anchorRow() expandTo()
*/

/*!
    \fn int TQTableSelection::numRows() const

    Returns the number of rows in the selection.

    \sa numCols()
*/
int TQTableSelection::numRows() const
{
    return ( tRow < 0 ) ? 0 : bRow - tRow + 1;
}

/*!
    Returns the number of columns in the selection.

    \sa numRows()
*/
int TQTableSelection::numCols() const
{
    return ( lCol < 0 ) ? 0 : rCol - lCol + 1;
}

/*!
    \fn bool TQTableSelection::isActive() const

    Returns whether the selection is active or not. A selection is
    active after init() \e and expandTo() have been called.
*/

/*!
    \fn bool TQTableSelection::isEmpty() const

    Returns whether the selection is empty or not.

    \sa numRows(), numCols()
*/

/*!
    \class TQTableItem
    \brief The TQTableItem class provides the cell content for TQTable cells.
\if defined(commercial)
    It is part of the <a href="commercialeditions.html">TQt Enterprise Edition</a>.
\endif

    \ingroup advanced
    \module table

    For many applications TQTableItems are ideal for presenting and
    editing the contents of TQTable cells. In situations where you need
    to create very large tables you may prefer an alternative approach
    to using TQTableItems: see the notes on large tables.

    A TQTableItem contains a cell's data, by default, a string and a
    pixmap. The table item also holds the cell's display size and how
    the data should be aligned. The table item specifies the cell's
    \l EditType and the editor used for in-place editing (by default a
    TQLineEdit). If you want checkboxes use \l{TQCheckTableItem}, and if
    you want comboboxes use \l{TQComboTableItem}. The \l EditType (set
    in the constructor) determines whether the cell's contents may be
    edited.

    If a pixmap is specified it is displayed to the left of any text.
    You can change the text or pixmap with setText() and setPixmap()
    respectively. For text you can use setWordWrap().

    When sorting table items the key() function is used; by default
    this returns the table item's text(). Reimplement key() to
    customize how your table items will sort.

    Table items are inserted into a table using TQTable::setItem(). If
    you insert an item into a cell that already contains a table item
    the original item will be deleted.

    Example:
    \code
    for ( int row = 0; row < table->numRows(); row++ ) {
	for ( int col = 0; col < table->numCols(); col++ ) {
	    table->setItem( row, col,
		new TQTableItem( table, TQTableItem::WhenCurrent, TQString::number( row * col ) ) );
	}
    }
    \endcode

    You can move a table item from one cell to another, in the same or
    a different table, using TQTable::takeItem() and TQTable::setItem()
    but see also TQTable::swapCells().

    Table items can be deleted with delete in the standard way; the
    table and cell will be updated accordingly.

    Note, that if you have a table item that is not currently in a table
    then anything you do to that item other than insert it into a table
    will result in undefined behaviour.

    Reimplement createEditor() and setContentFromEditor() if you want
    to use your own widget instead of a TQLineEdit for editing cell
    contents. Reimplement paint() if you want to display custom
    content.

    It is important to ensure that your custom widget can accept the
    keyboard focus, so that the user can use the tab key to navigate the
    table as normal. Therefore, if the widget returned by createEditor()
    does not itself accept the keyboard focus, it is necessary to
    nominate a child widget to do so on its behalf. For example, a
    TQHBox with two child TQLineEdit widgets may use one of them to
    accept the keyboard focus:

    \code
    TQWidget* MyTableItem::createEditor() const
    {
        TQHBox* hbox = new TQHBox( table()->viewport() );
        hbox->setFocusProxy(new TQLineEdit( hbox ));
        new TQLineEdit( hbox );
        return hbox;
    }
    \endcode

    By default, table items may be replaced by new TQTableItems
    during the lifetime of a TQTable. Therefore, if you create your
    own subclass of TQTableItem, and you want to ensure that
    this does not happen, you must call setReplaceable(false)
    in the constructor of your subclass.

    \img qtableitems.png Table Items

    \sa TQCheckTableItem TQComboTableItem

*/

/*!
    \fn TQTable *TQTableItem::table() const

    Returns the TQTable the table item belongs to.

    \sa TQTable::setItem() TQTableItem()
*/

/*!
    \enum TQTableItem::EditType

    \target wheneditable
    This enum is used to define whether a cell is editable or
    read-only (in conjunction with other settings), and how the cell
    should be displayed.

    \value Always
    The cell always \e looks editable.

    Using this EditType ensures that the editor created with
    createEditor() (by default a TQLineEdit) is always visible. This
    has implications for the alignment of the content: the default
    editor aligns everything (even numbers) to the left whilst
    numerical values in the cell are by default aligned to the right.

    If a cell with the edit type \c Always looks misaligned you could
    reimplement createEditor() for these items.

    \value WhenCurrent
    The cell \e looks editable only when it has keyboard focus (see
    TQTable::setCurrentCell()).

    \value OnTyping
    The cell \e looks editable only when the user types in it or
    double-clicks it. It resembles the \c WhenCurrent functionality
    but is, perhaps, nicer.

    The \c OnTyping edit type is the default when TQTableItem objects
    are created by the convenience functions TQTable::setText() and
    TQTable::setPixmap().

    \value Never  The cell is not editable.

    The cell is actually editable only if TQTable::isRowReadOnly() is
    false for its row, TQTable::isColumnReadOnly() is false for its
    column, and TQTable::isReadOnly() is false.

    TQComboTableItems have an isEditable() property. This property is
    used to indicate whether the user may enter their own text or are
    restricted to choosing one of the choices in the list.
    TQComboTableItems may be interacted with only if they are editable
    in accordance with their EditType as described above.

*/

/*!
    Creates a table item that is a child of table \a table with no
    text. The item has the \l EditType \a et.

    The table item will use a TQLineEdit for its editor, will not
    word-wrap and will occupy a single cell. Insert the table item
    into a table with TQTable::setItem().

    The table takes ownership of the table item, so a table item
    should not be inserted into more than one table at a time.
*/

TQTableItem::TQTableItem( TQTable *table, EditType et )
    : txt(), pix(), t( table ), edType( et ), wordwrap( false ),
      tcha( true ), rw( -1 ), cl( -1 ), rowspan( 1 ), colspan( 1 )
{
    enabled = true;
}

/*!
    Creates a table item that is a child of table \a table with text
    \a text. The item has the \l EditType \a et.

    The table item will use a TQLineEdit for its editor, will not
    word-wrap and will occupy a single cell. Insert the table item
    into a table with TQTable::setItem().

    The table takes ownership of the table item, so a table item
    should not be inserted into more than one table at a time.
*/

TQTableItem::TQTableItem( TQTable *table, EditType et, const TQString &text )
    : txt( text ), pix(), t( table ), edType( et ), wordwrap( false ),
      tcha( true ), rw( -1 ), cl( -1 ), rowspan( 1 ), colspan( 1 )
{
    enabled = true;
}

/*!
    Creates a table item that is a child of table \a table with text
    \a text and pixmap \a p. The item has the \l EditType \a et.

    The table item will display the pixmap to the left of the text. It
    will use a TQLineEdit for editing the text, will not word-wrap and
    will occupy a single cell. Insert the table item into a table with
    TQTable::setItem().

    The table takes ownership of the table item, so a table item
    should not be inserted in more than one table at a time.
*/

TQTableItem::TQTableItem( TQTable *table, EditType et,
			const TQString &text, const TQPixmap &p )
    : txt( text ), pix( p ), t( table ), edType( et ), wordwrap( false ),
      tcha( true ), rw( -1 ), cl( -1 ), rowspan( 1 ), colspan( 1 )
{
    enabled = true;
}

/*!
    The destructor deletes this item and frees all allocated
    resources.

    If the table item is in a table (i.e. was inserted with
    setItem()), it will be removed from the table and the cell it
    occupied.
*/

TQTableItem::~TQTableItem()
{
    if ( table() )
	table()->takeItem( this );
}

int TQTableItem::RTTI = 0;

/*!
    Returns the Run Time Type Identification value for this table item
    which for TQTableItems is 0.

    When you create subclasses based on TQTableItem make sure that each
    subclass returns a unique rtti() value. It is advisable to use
    values greater than 1000, preferably large random numbers, to
    allow for extensions to this class.

    \sa TQCheckTableItem::rtti() TQComboTableItem::rtti()
*/

int TQTableItem::rtti() const
{
    return RTTI;
}

/*!
    Returns the table item's pixmap or a null pixmap if no pixmap has
    been set.

    \sa setPixmap() text()
*/

TQPixmap TQTableItem::pixmap() const
{
    return pix;
}


/*!
    Returns the text of the table item or TQString::null if there is no
    text.

    To ensure that the current value of the editor is returned,
    setContentFromEditor() is called:
    \list 1
    \i if the editMode() is \c Always, or
    \i if editMode() is \e not \c Always but the editor of the cell is
    active and the editor is not a TQLineEdit.
    \endlist

    This means that text() returns the original text value of the item
    if the editor is a line edit, until the user commits an edit (e.g.
    by pressing Enter or Tab) in which case the new text is returned.
    For other editors (e.g. a combobox) setContentFromEditor() is
    always called so the currently display value is the one returned.

    \sa setText() pixmap()
*/

TQString TQTableItem::text() const
{
    TQWidget *w = table()->cellWidget( rw, cl );
    if ( w && ( edType == Always ||
		rtti() == TQComboTableItem::RTTI ||
		rtti() == TQCheckTableItem::RTTI ) )
	( (TQTableItem*)this )->setContentFromEditor( w );
    return txt;
}

/*!
    Sets pixmap \a p to be this item's pixmap.

    Note that setPixmap() does not update the cell the table item
    belongs to. Use TQTable::updateCell() to repaint the cell's
    contents.

    For \l{TQComboTableItem}s and \l{TQCheckTableItem}s this function
    has no visible effect.

    \sa TQTable::setPixmap() pixmap() setText()
*/

void TQTableItem::setPixmap( const TQPixmap &p )
{
    pix = p;
}

/*!
    Changes the table item's text to \a str.

    Note that setText() does not update the cell the table item
    belongs to. Use TQTable::updateCell() to repaint the cell's
    contents.

    \sa TQTable::setText() text() setPixmap() TQTable::updateCell()
*/

void TQTableItem::setText( const TQString &str )
{
    txt = str;
}

/*!
    This virtual function is used to paint the contents of an item
    using the painter \a p in the rectangular area \a cr using the
    color group \a cg.

    If \a selected is true the cell is displayed in a way that
    indicates that it is highlighted.

    You don't usually need to use this function but if you want to
    draw custom content in a cell you will need to reimplement it.

    The painter passed to this function is translated so that 0, 0
    is the top-left corner of the item that is being painted.

    Note that the painter is not clipped by default in order to get
    maximum efficiency. If you want clipping, use

    \code
    p->setClipRect( table()->cellRect(row, col), TQPainter::ClipPainter );
    //... your drawing code
    p->setClipping( false );
    \endcode

*/

void TQTableItem::paint( TQPainter *p, const TQColorGroup &cg,
			const TQRect &cr, bool selected )
{
    p->fillRect( 0, 0, cr.width(), cr.height(),
		 selected ? cg.brush( TQColorGroup::Highlight )
			  : cg.brush( TQColorGroup::Base ) );

    int w = cr.width();
    int h = cr.height();

    int x = 0;
    if ( !pix.isNull() ) {
	p->drawPixmap( 0, ( cr.height() - pix.height() ) / 2, pix );
	x = pix.width() + 2;
    }

    if ( selected )
	p->setPen( cg.highlightedText() );
    else
	p->setPen( cg.text() );
    p->drawText( x + 2, 0, w - x - 4, h,
		 wordwrap ? (alignment() | WordBreak) : alignment(), text() );
}

/*!
This virtual function creates an editor which the user can
interact with to edit the cell's contents. The default
implementation creates a TQLineEdit.

If the function returns 0, the cell is read-only.

The returned widget should preferably be invisible, ideally with
TQTable::viewport() as parent.

If you reimplement this function you'll almost certainly need to
reimplement setContentFromEditor(), and may need to reimplement
sizeHint().

\quotefile table/statistics/statistics.cpp
\skipto createEditor
\printto }

\sa TQTable::createEditor() setContentFromEditor() TQTable::viewport() setReplaceable()
*/

TQWidget *TQTableItem::createEditor() const
{
    TQLineEdit *e = new TQLineEdit( table()->viewport(), "qt_tableeditor" );
    e->setFrame( false );
    e->setText( text() );
    return e;
}

/*!
Whenever the content of a cell has been edited by the editor \a w,
TQTable calls this virtual function to copy the new values into the
TQTableItem.

If you reimplement createEditor() and return something that is not
a TQLineEdit you will need to reimplement this function.

\quotefile table/statistics/statistics.cpp
\skipto setContentFromEditor
\printto }

\sa TQTable::setCellContentFromEditor()
*/

void TQTableItem::setContentFromEditor( TQWidget *w )
{
    TQLineEdit *le = ::tqt_cast<TQLineEdit*>(w);
    if ( le ) {
	TQString input = le->text();
	if ( le->validator() )
	    le->validator()->fixup( input );
	setText( input );
    }
}

/*!
    The alignment function returns how the text contents of the cell
    are aligned when drawn. The default implementation aligns numbers
    to the right and any other text to the left.

    \sa TQt::AlignmentFlags
*/

// ed: For consistency reasons a setAlignment() should be provided
// as well.

int TQTableItem::alignment() const
{
    bool num;
    bool ok1 = false, ok2 = false;
    (void)text().toInt( &ok1 );
    if ( !ok1 )
	(void)text().toDouble( &ok2 ); // ### should be .-aligned
    num = ok1 || ok2;

    return ( num ? AlignRight : AlignLeft ) | AlignVCenter;
}

/*!
    If \a b is true, the cell's text will be wrapped over multiple
    lines, when necessary, to fit the width of the cell; otherwise the
    text will be written as a single line.

    \sa wordWrap() TQTable::adjustColumn() TQTable::setColumnStretchable()
*/

void TQTableItem::setWordWrap( bool b )
{
    wordwrap = b;
}

/*!
    Returns true if word wrap is enabled for the cell; otherwise
    returns false.

    \sa setWordWrap()
*/

bool TQTableItem::wordWrap() const
{
    return wordwrap;
}

/*! \internal */

void TQTableItem::updateEditor( int oldRow, int oldCol )
{
    if ( edType != Always )
	return;
    if ( oldRow != -1 && oldCol != -1 )
	table()->clearCellWidget( oldRow, oldCol );
    if ( rw != -1 && cl != -1 )
	table()->setCellWidget( rw, cl, createEditor() );
}

/*!
    Returns the table item's edit type.

    This is set when the table item is constructed.

    \sa EditType TQTableItem()
*/

TQTableItem::EditType TQTableItem::editType() const
{
    return edType;
}

/*!
    If \a b is true it is acceptable to replace the contents of the
    cell with the contents of another TQTableItem. If \a b is false the
    contents of the cell may not be replaced by the contents of
    another table item. Table items that span more than one cell may
    not have their contents replaced by another table item.

    (This differs from \l EditType because EditType is concerned with
    whether the \e user is able to change the contents of a cell.)

    \sa isReplaceable()
*/

void TQTableItem::setReplaceable( bool b )
{
    tcha = b;
}

/*!
    This function returns whether the contents of the cell may be
    replaced with the contents of another table item. Regardless of
    this setting, table items that span more than one cell may not
    have their contents replaced by another table item.

    (This differs from \l EditType because EditType is concerned with
    whether the \e user is able to change the contents of a cell.)

    \sa setReplaceable() EditType
*/

bool TQTableItem::isReplaceable() const
{
    if ( rowspan > 1 || colspan > 1 )
	return false;
    return tcha;
}

/*!
    This virtual function returns the key that should be used for
    sorting. The default implementation returns the text() of the
    relevant item.

    \sa TQTable::setSorting()
*/

TQString TQTableItem::key() const
{
    return text();
}

/*!
    This virtual function returns the size a cell needs to show its
    entire content.

    If you subclass TQTableItem you will often need to reimplement this
    function.
*/

TQSize TQTableItem::sizeHint() const
{
    TQSize strutSize = TQApplication::globalStrut();
    if ( edType == Always && table()->cellWidget( rw, cl ) )
	return table()->cellWidget( rw, cl )->sizeHint().expandedTo( strutSize );

    TQSize s;
    int x = 0;
    if ( !pix.isNull() ) {
	s = pix.size();
	s.setWidth( s.width() + 2 );
	x = pix.width() + 2;
    }

    TQString t = text();
    if ( !wordwrap && t.find( '\n' ) == -1 )
	return TQSize( s.width() + table()->fontMetrics().width( text() ) + 10,
		      TQMAX( s.height(), table()->fontMetrics().height() ) ).expandedTo( strutSize );

    TQRect r = table()->fontMetrics().boundingRect( x + 2, 0, table()->columnWidth( col() ) - x - 4, 0,
						   wordwrap ? (alignment() | WordBreak) : alignment(),
						   text() );
    r.setWidth( TQMAX( r.width() + 10, table()->columnWidth( col() ) ) );
    return TQSize( r.width(), TQMAX( s.height(), r.height() ) ).expandedTo( strutSize );
}

/*!
    Changes the extent of the TQTableItem so that it spans multiple
    cells covering \a rs rows and \a cs columns. The top left cell is
    the original cell.

    \warning This function only works if the item has already been
    inserted into the table using e.g. TQTable::setItem(). This
    function also checks to make sure if \a rs and \a cs are within
    the bounds of the table and returns without changing the span if
    they are not. In addition swapping, inserting or removing rows and
    columns that cross TQTableItems spanning more than one cell is not
    supported.

    \sa rowSpan() colSpan()
*/

void TQTableItem::setSpan( int rs, int cs )
{
    if ( rs == rowspan && cs == colspan )
	return;

    if ( !table()->d->hasRowSpan )
	table()->d->hasRowSpan = rs > 1;
    if ( !table()->d->hasColSpan )
	table()->d->hasColSpan = cs > 1;
    // return if we are thinking too big...
    if ( rw + rs > table()->numRows() )
	return;

    if ( cl + cs > table()->numCols() )
	return;

    if ( rw == -1 || cl == -1 )
	return;

    int rrow = rw;
    int rcol = cl;
    if ( rowspan > 1 || colspan > 1 ) {
	TQTable* t = table();
	t->takeItem( this );
	t->setItem( rrow, rcol, this );
    }

    rowspan = rs;
    colspan = cs;

    for ( int r = 0; r < rowspan; ++r ) {
	for ( int c = 0; c < colspan; ++c ) {
	    if ( r == 0 && c == 0 )
		continue;
	    qt_update_cell_widget = false;
	    table()->setItem( r + rw, c + cl, this );
	    qt_update_cell_widget = true;
	    rw = rrow;
	    cl = rcol;
	}
    }

    table()->updateCell( rw, cl );
    TQWidget *w = table()->cellWidget( rw, cl );
    if ( w )
	w->resize( table()->cellGeometry( rw, cl ).size() );
}

/*!
    Returns the row span of the table item, usually 1.

    \sa setSpan() colSpan()
*/

int TQTableItem::rowSpan() const
{
    return rowspan;
}

/*!
    Returns the column span of the table item, usually 1.

    \sa setSpan() rowSpan()
*/

int TQTableItem::colSpan() const
{
    return colspan;
}

/*!
    Sets row \a r as the table item's row. Usually you do not need to
    call this function.

    If the cell spans multiple rows, this function sets the top row
    and retains the height of the multi-cell table item.

    \sa row() setCol() rowSpan()
*/

void TQTableItem::setRow( int r )
{
    rw = r;
}

/*!
    Sets column \a c as the table item's column. Usually you will not
    need to call this function.

    If the cell spans multiple columns, this function sets the
    left-most column and retains the width of the multi-cell table
    item.

    \sa col() setRow() colSpan()
*/

void TQTableItem::setCol( int c )
{
    cl = c;
}

/*!
    Returns the row where the table item is located. If the cell spans
    multiple rows, this function returns the top-most row.

    \sa col() setRow()
*/

int TQTableItem::row() const
{
    return rw;
}

/*!
    Returns the column where the table item is located. If the cell
    spans multiple columns, this function returns the left-most
    column.

    \sa row() setCol()
*/

int TQTableItem::col() const
{
    return cl;
}

/*!
    If \a b is true, the table item is enabled; if \a b is false the
    table item is disabled.

    A disabled item doesn't respond to user interaction.

    \sa isEnabled()
*/

void TQTableItem::setEnabled( bool b )
{
    if ( b == (bool)enabled )
	return;
    enabled = b;
    table()->updateCell( row(), col() );
}

/*!
    Returns true if the table item is enabled; otherwise returns false.

    \sa setEnabled()
*/

bool TQTableItem::isEnabled() const
{
    return (bool)enabled;
}

/*!
    \class TQComboTableItem
    \brief The TQComboTableItem class provides a means of using
    comboboxes in TQTables.
\if defined(commercial)
    It is part of the <a href="commercialeditions.html">TQt Enterprise Edition</a>.
\endif

    \ingroup advanced
    \module table

    A TQComboTableItem is a table item which looks and behaves like a
    combobox. The advantage of using TQComboTableItems rather than real
    comboboxes is that a TQComboTableItem uses far less resources than
    real comboboxes in \l{TQTable}s. When the cell has the focus it
    displays a real combobox which the user can interact with. When
    the cell does not have the focus the cell \e looks like a
    combobox. Only text items (i.e. no pixmaps) may be used in
    TQComboTableItems.

    TQComboTableItem items have the edit type \c WhenCurrent (see
    \l{EditType}). The TQComboTableItem's list of items is provided by
    a TQStringList passed to the constructor.

    The list of items may be changed using setStringList(). The
    current item can be set with setCurrentItem() and retrieved with
    currentItem(). The text of the current item can be obtained with
    currentText(), and the text of a particular item can be retrieved
    with text().

    If isEditable() is true the TQComboTableItem will permit the user
    to either choose an existing list item, or create a new list item
    by entering their own text; otherwise the user may only choose one
    of the existing list items.

    To populate a table cell with a TQComboTableItem use
    TQTable::setItem().

    TQComboTableItems may be deleted with TQTable::clearCell().

    TQComboTableItems can be distinguished from \l{TQTableItem}s and
    \l{TQCheckTableItem}s using their Run Time Type Identification
    number (see rtti()).

    \img qtableitems.png Table Items

    \sa TQCheckTableItem TQTableItem TQComboBox
*/

TQComboBox *TQComboTableItem::fakeCombo = 0;
TQWidget *TQComboTableItem::fakeComboWidget = 0;
int TQComboTableItem::fakeRef = 0;

/*!
    Creates a combo table item for the table \a table. The combobox's
    list of items is passed in the \a list argument. If \a editable is
    true the user may type in new list items; if \a editable is false
    the user may only select from the list of items provided.

    By default TQComboTableItems cannot be replaced by other table
    items since isReplaceable() returns false by default.

    \sa TQTable::clearCell() EditType
*/

TQComboTableItem::TQComboTableItem( TQTable *table, const TQStringList &list, bool editable )
    : TQTableItem( table, WhenCurrent, "" ), entries( list ), current( 0 ), edit( editable )
{
    setReplaceable( false );
    if ( !TQComboTableItem::fakeCombo ) {
	TQComboTableItem::fakeComboWidget = new TQWidget( 0, 0 );
	TQComboTableItem::fakeCombo = new TQComboBox( false, TQComboTableItem::fakeComboWidget, 0 );
	TQComboTableItem::fakeCombo->hide();
    }
    ++TQComboTableItem::fakeRef;
    if ( entries.count() )
	setText( *entries.at( current ) );
}

/*!
    TQComboTableItem destructor.
*/
TQComboTableItem::~TQComboTableItem()
{
    if (--TQComboTableItem::fakeRef <= 0) {
	delete TQComboTableItem::fakeComboWidget;
	TQComboTableItem::fakeComboWidget = 0;
	TQComboTableItem::fakeCombo = 0;
    }
}

/*!
    Sets the list items of this TQComboTableItem to the strings in the
    string list \a l.
*/

void TQComboTableItem::setStringList( const TQStringList &l )
{
    entries = l;
    current = 0;
    if ( entries.count() )
	setText( *entries.at( current ) );
    if ( table()->cellWidget( row(), col() ) ) {
	cb->clear();
	cb->insertStringList( entries );
    }
    table()->updateCell( row(), col() );
}

/*! \reimp */

TQWidget *TQComboTableItem::createEditor() const
{
    // create an editor - a combobox in our case
    ( (TQComboTableItem*)this )->cb = new TQComboBox( edit, table()->viewport(), "qt_editor_cb" );
    cb->insertStringList( entries );
    cb->setCurrentItem( current );
    TQObject::connect( cb, TQ_SIGNAL( activated(int) ), table(), TQ_SLOT( doValueChanged() ) );
    return cb;
}

/*! \reimp */

void TQComboTableItem::setContentFromEditor( TQWidget *w )
{
    TQComboBox *cb = ::tqt_cast<TQComboBox*>(w);
    if ( cb ) {
	entries.clear();
	for ( int i = 0; i < cb->count(); ++i )
	    entries << cb->text( i );
	current = cb->currentItem();
	setText( *entries.at( current ) );
    }
}

/*! \reimp */

void TQComboTableItem::paint( TQPainter *p, const TQColorGroup &cg,
			   const TQRect &cr, bool selected )
{
    fakeCombo->resize( cr.width(), cr.height() );

    TQColorGroup c( cg );
    if ( selected ) {
	c.setBrush( TQColorGroup::Base, cg.brush( TQColorGroup::Highlight ) );
	c.setColor( TQColorGroup::Text, cg.highlightedText() );
    }

    TQStyle::SFlags flags = TQStyle::Style_Default;
    if(isEnabled() && table()->isEnabled())
	flags |= TQStyle::Style_Enabled;
    table()->style().drawComplexControl( TQStyle::CC_ComboBox, p, fakeCombo, fakeCombo->rect(), c, flags );

    p->save();
    TQRect textR = table()->style().querySubControlMetrics(TQStyle::CC_ComboBox, fakeCombo,
							 TQStyle::SC_ComboBoxEditField);
    int align = alignment(); // alignment() changes entries
    p->drawText( textR, wordWrap() ? ( align | WordBreak ) : align, *entries.at( current ) );
    p->restore();
}

/*!
    Sets the list item \a i to be the combo table item's current list
    item.

    \sa currentItem()
*/

void TQComboTableItem::setCurrentItem( int i )
{
    TQWidget *w = table()->cellWidget( row(), col() );
    TQComboBox *cb = ::tqt_cast<TQComboBox*>(w);
    if ( cb ) {
	cb->setCurrentItem( i );
	current = i;
	setText( cb->currentText() );
    } else {
	current = i;
	setText( *entries.at( i ) );
	table()->updateCell( row(), col() );
    }
}

/*!
    \overload

    Sets the list item whose text is \a s to be the combo table item's
    current list item. Does nothing if no list item has the text \a s.

    \sa currentItem()
*/

void TQComboTableItem::setCurrentItem( const TQString &s )
{
    int i = entries.findIndex( s );
    if ( i != -1 )
	setCurrentItem( i );
}

/*!
    Returns the index of the combo table item's current list item.

    \sa setCurrentItem()
*/

int TQComboTableItem::currentItem() const
{
    TQWidget *w = table()->cellWidget( row(), col() );
    TQComboBox *cb = ::tqt_cast<TQComboBox*>(w);
    if ( cb )
	return cb->currentItem();
    return current;
}

/*!
    Returns the text of the combo table item's current list item.

    \sa currentItem() text()
*/

TQString TQComboTableItem::currentText() const
{
    TQWidget *w = table()->cellWidget( row(), col() );
    TQComboBox *cb = ::tqt_cast<TQComboBox*>(w);
    if ( cb )
	return cb->currentText();
    return *entries.at( current );
}

/*!
    Returns the total number of list items in the combo table item.
*/

int TQComboTableItem::count() const
{
    TQWidget *w = table()->cellWidget( row(), col() );
    TQComboBox *cb = ::tqt_cast<TQComboBox*>(w);
    if ( cb )
	return cb->count();
    return (int)entries.count();    //### size_t/int cast
}

/*!
    Returns the text of the combo's list item at index \a i.

    \sa currentText()
*/

TQString TQComboTableItem::text( int i ) const
{
    TQWidget *w = table()->cellWidget( row(), col() );
    TQComboBox *cb = ::tqt_cast<TQComboBox*>(w);
    if ( cb )
	return cb->text( i );
    return *entries.at( i );
}

/*!
    If \a b is true the combo table item can be edited, i.e. the user
    may enter a new text item themselves. If \a b is false the user may
    may only choose one of the existing items.

    \sa isEditable()
*/

void TQComboTableItem::setEditable( bool b )
{
    edit = b;
}

/*!
    Returns true if the user can add their own list items to the
    combobox's list of items; otherwise returns false.

    \sa setEditable()
*/

bool TQComboTableItem::isEditable() const
{
    return edit;
}

int TQComboTableItem::RTTI = 1;

/*!
    \fn int TQComboTableItem::rtti() const

    Returns 1.

    Make your derived classes return their own values for rtti()to
    distinguish between different table item subclasses. You should
    use values greater than 1000, preferably a large random number, to
    allow for extensions to this class.


    \sa TQTableItem::rtti()
*/

int TQComboTableItem::rtti() const
{
    return RTTI;
}

/*! \reimp */

TQSize TQComboTableItem::sizeHint() const
{
    fakeCombo->insertItem( currentText() );
    fakeCombo->setCurrentItem( fakeCombo->count() - 1 );
    TQSize sh = fakeCombo->sizeHint();
    fakeCombo->removeItem( fakeCombo->count() - 1 );
    return sh.expandedTo( TQApplication::globalStrut() );
}

/*!
    \class TQCheckTableItem
    \brief The TQCheckTableItem class provides checkboxes in TQTables.
\if defined(commercial)
    It is part of the <a href="commercialeditions.html">TQt Enterprise Edition</a>.
\endif

    \ingroup advanced
    \module table

    A TQCheckTableItem is a table item which looks and behaves like a
    checkbox. The advantage of using TQCheckTableItems rather than real
    checkboxes is that a TQCheckTableItem uses far less resources than
    a real checkbox would in a \l{TQTable}. When the cell has the focus
    it displays a real checkbox which the user can interact with. When
    the cell does not have the focus the cell \e looks like a
    checkbox. Pixmaps may not be used in TQCheckTableItems.

    TQCheckTableItem items have the edit type \c WhenCurrent (see
    \l{EditType}).

    To change the checkbox's label use setText(). The checkbox can be
    checked and unchecked with setChecked() and its state retrieved
    using isChecked().

    To populate a table cell with a TQCheckTableItem use
    TQTable::setItem().

    TQCheckTableItems can be distinguished from \l{TQTableItem}s and
    \l{TQComboTableItem}s using their Run Time Type Identification
    (rtti) value.

    \img qtableitems.png Table Items

    \sa rtti() EditType TQComboTableItem TQTableItem TQCheckBox
*/

/*!
    Creates a TQCheckTableItem with an \l{EditType} of \c WhenCurrent
    as a child of \a table. The checkbox is initially unchecked and
    its label is set to the string \a txt.
*/

TQCheckTableItem::TQCheckTableItem( TQTable *table, const TQString &txt )
    : TQTableItem( table, WhenCurrent, txt ), checked( false )
{
}

/*! \reimp */

void TQCheckTableItem::setText( const TQString &t )
{
    TQTableItem::setText( t );
    TQWidget *w = table()->cellWidget( row(), col() );
    TQCheckBox *cb = ::tqt_cast<TQCheckBox*>(w);
    if ( cb )
	cb->setText( t );
}


/*! \reimp */

TQWidget *TQCheckTableItem::createEditor() const
{
    // create an editor - a combobox in our case
    ( (TQCheckTableItem*)this )->cb = new TQCheckBox( table()->viewport(), "qt_editor_checkbox" );
    cb->setChecked( checked );
    cb->setText( text() );
    cb->setBackgroundColor( table()->viewport()->backgroundColor() );
    TQObject::connect( cb, TQ_SIGNAL( toggled(bool) ), table(), TQ_SLOT( doValueChanged() ) );
    return cb;
}

/*! \reimp */

void TQCheckTableItem::setContentFromEditor( TQWidget *w )
{
    TQCheckBox *cb = ::tqt_cast<TQCheckBox*>(w);
    if ( cb )
	checked = cb->isChecked();
}

/*! \reimp */

void TQCheckTableItem::paint( TQPainter *p, const TQColorGroup &cg,
				const TQRect &cr, bool selected )
{
    p->fillRect( 0, 0, cr.width(), cr.height(),
		 selected ? cg.brush( TQColorGroup::Highlight )
			  : cg.brush( TQColorGroup::Base ) );

    int w = cr.width();
    int h = cr.height();
    TQSize sz = TQSize( table()->style().pixelMetric( TQStyle::PM_IndicatorWidth ),
		      table()->style().pixelMetric( TQStyle::PM_IndicatorHeight ) );
    TQColorGroup c( cg );
    c.setBrush( TQColorGroup::Background, c.brush( TQColorGroup::Base ) );
    TQStyle::SFlags flags = TQStyle::Style_Default;
    if(isEnabled())
	flags |= TQStyle::Style_Enabled;
    if ( checked )
	flags |= TQStyle::Style_On;
    else
	flags |= TQStyle::Style_Off;
    if ( isEnabled() && table()->isEnabled() )
	flags |= TQStyle::Style_Enabled;

    table()->style().drawPrimitive( TQStyle::PE_Indicator, p,
				    TQRect( 0, ( cr.height() - sz.height() ) / 2, sz.width(), sz.height() ), c, flags );
    int x = sz.width() + 6;
    w = w - x;
    if ( selected )
	p->setPen( cg.highlightedText() );
    else
	p->setPen( cg.text() );
    p->drawText( x, 0, w, h, wordWrap() ? ( alignment() | WordBreak ) : alignment(), text() );
}

/*!
    If \a b is true the checkbox is checked; if \a b is false the
    checkbox is unchecked.

    \sa isChecked()
*/

void TQCheckTableItem::setChecked( bool b )
{
    checked = b;
    table()->updateCell( row(), col() );
    TQWidget *w = table()->cellWidget( row(), col() );
    TQCheckBox *cb = ::tqt_cast<TQCheckBox*>(w);
    if ( cb )
	cb->setChecked( b );
}

/*!
    Returns true if the checkbox table item is checked; otherwise
    returns false.

    \sa setChecked()
*/

bool TQCheckTableItem::isChecked() const
{
    // #### why was this next line here. It must not be here, as
    // #### people want to call isChecked() from within paintCell()
    // #### and end up in an infinite loop that way
    // table()->updateCell( row(), col() );
    TQWidget *w = table()->cellWidget( row(), col() );
    TQCheckBox *cb = ::tqt_cast<TQCheckBox*>(w);
    if ( cb )
	return cb->isChecked();
    return checked;
}

int TQCheckTableItem::RTTI = 2;

/*!
    \fn int TQCheckTableItem::rtti() const

    Returns 2.

    Make your derived classes return their own values for rtti()to
    distinguish between different table item subclasses. You should
    use values greater than 1000, preferably a large random number, to
    allow for extensions to this class.

    \sa TQTableItem::rtti()
*/

int TQCheckTableItem::rtti() const
{
    return RTTI;
}

/*! \reimp */

TQSize TQCheckTableItem::sizeHint() const
{
    TQSize sz = TQSize( table()->style().pixelMetric( TQStyle::PM_IndicatorWidth ),
		      table()->style().pixelMetric( TQStyle::PM_IndicatorHeight ) );
    sz.setWidth( sz.width() + 6 );
    TQSize sh( TQTableItem::sizeHint() );
    return TQSize( sh.width() + sz.width(), TQMAX( sh.height(), sz.height() ) ).
	expandedTo( TQApplication::globalStrut() );
}

/*! \file table/small-table-demo/main.cpp */
/*! \file table/bigtable/main.cpp */
/*! \file table/statistics/statistics.cpp */

/*!
    \class TQTable
    \brief The TQTable class provides a flexible editable table widget.
\if defined(commercial)
    It is part of the <a href="commercialeditions.html">TQt Enterprise Edition</a>.
\endif

    \mainclass
    \ingroup advanced
    \module table

    TQTable is easy to use, although it does have a large API because
    of the comprehensive functionality that it provides. TQTable
    includes functions for manipulating \link #headers
    headers\endlink, \link #columnsrows rows and columns\endlink,
    \link #cells cells\endlink and \link #selections
    selections\endlink. TQTable also provides in-place editing and
    \link dnd.html drag and drop\endlink, as well as a useful set of
    \link #signals signals\endlink. TQTable efficiently supports very
    large tables, for example, tables one million by one million cells
    are perfectly possible. TQTable is economical with memory, using
    none for unused cells.

    \code
    TQTable *table = new TQTable( 100, 250, this );
    table->setPixmap( 3, 2, pix );
    table->setText( 3, 2, "A pixmap" );
    \endcode

    The first line constructs the table specifying its size in rows
    and columns. We then insert a pixmap and some text into the \e
    same \link #cells cell\endlink, with the pixmap appearing to the
    left of the text. TQTable cells can be populated with
    \l{TQTableItem}s, \l{TQComboTableItem}s or by \l{TQCheckTableItem}s.
    By default a vertical header appears at the left of the table
    showing row numbers and a horizontal header appears at the top of
    the table showing column numbers. (The numbers displayed start at
    1, although row and column numbers within TQTable begin at 0.)

    If you want to use mouse tracking call setMouseTracking( true ) on
    the \e viewport; (see \link ntqscrollview.html#allviews
    TQScrollView\endlink).

    \img qtableitems.png Table Items

    \target headers
    \section1 Headers

    TQTable supports a header column, e.g. to display row numbers, and
    a header row, e.g to display column titles. To set row or column
    labels use TQHeader::setLabel() on the pointers returned by
    verticalHeader() and horizontalHeader() respectively. The vertical
    header is displayed within the table's left margin whose width is
    set with setLeftMargin(). The horizontal header is displayed
    within the table's top margin whose height is set with
    setTopMargin(). The table's grid can be switched off with
    setShowGrid(). If you want to hide a horizontal header call
    hide(), and call setTopMargin( 0 ) so that the area the header
    would have occupied is reduced to zero size.

    Header labels are indexed via their section numbers. Note that the
    default behavior of TQHeader regarding section numbers is overriden
    for TQTable. See the explanation below in the Rows and Columns
    section in the discussion of moving columns and rows.

    \target columnsrows
    \section1 Rows and Columns

    Row and column sizes are set with setRowHeight() and
    setColumnWidth(). If you want a row high enough to show the
    tallest item in its entirety, use adjustRow(). Similarly, to make
    a column wide enough to show the widest item use adjustColumn().
    If you want the row height and column width to adjust
    automatically as the height and width of the table changes use
    setRowStretchable() and setColumnStretchable().

    Rows and columns can be hidden and shown with hideRow(),
    hideColumn(), showRow() and showColumn(). New rows and columns are
    inserted using insertRows() and insertColumns(). Additional rows
    and columns are added at the  bottom (rows) or right (columns) if
    you set setNumRows() or setNumCols() to be larger than numRows()
    or numCols(). Existing rows and columns are removed with
    removeRow() and removeColumn(). Multiple rows and columns can be
    removed with removeRows() and removeColumns().

    Rows and columns can be set to be moveable using
    rowMovingEnabled() and columnMovingEnabled(). The user can drag
    them to reorder them holding down the Ctrl key and dragging the
    mouse. For performance reasons, the default behavior of TQHeader
    section numbers is overridden by TQTable. Currently in TQTable, when
    a row or column is dragged and reordered, the section number is
    also changed to its new position. Therefore, there is no
    difference between the section and the index fields in TQHeader.
    The TQTable TQHeader classes do not provide a mechanism for indexing
    independently of the user interface ordering.

    The table can be sorted using sortColumn(). Users can sort a
    column by clicking its header if setSorting() is set to true. Rows
    can be swapped with swapRows(), columns with swapColumns() and
    cells with swapCells().

    For editable tables (see setReadOnly()) you can set the read-only
    property of individual rows and columns with setRowReadOnly() and
    setColumnReadOnly(). (Whether a cell is editable or read-only
    depends on these settings and the cell's \link
    qtableitem.html#wheneditable TQTableItem::EditType\endlink.)

    The row and column which have the focus are returned by
    currentRow() and currentColumn() respectively.

    Although many TQTable functions operate in terms of rows and
    columns the indexOf() function returns a single integer
    identifying a particular cell.

    \target cells
    \section1 Cells

    All of a TQTable's cells are empty when the table is constructed.

    There are two approaches to populating the table's cells. The
    first and simplest approach is to use TQTableItems or TQTableItem
    subclasses. The second approach doesn't use TQTableItems at all
    which is useful for very large sparse tables but requires you to
    reimplement a number of functions. We'll look at each approach in
    turn.

    To put a string in a cell use setText(). This function will create
    a new TQTableItem for the cell if one doesn't already exist, and
    displays the text in it. By default the table item's widget will
    be a TQLineEdit. A pixmap may be put in a cell with setPixmap(),
    which also creates a table item if required. A cell may contain \e
    both a pixmap and text; the pixmap is displayed to the left of the
    text. Another approach is to construct a TQTableItem or TQTableItem
    subclass, set its properties, then insert it into a cell with
    setItem().

    If you want cells which contain comboboxes use the TQComboTableItem
    class. Similarly if you require cells containing checkboxes use
    the TQCheckTableItem class. These table items look and behave just
    like the combobox or checkbox widgets but consume far less memory.

    \quotefile table/small-table-demo/main.cpp
    \skipto int j
    \printuntil TQCheckTableItem
    In the example above we create a column of TQCheckTableItems and
    insert them into the table using setItem().

    TQTable takes ownership of its TQTableItems and will delete them
    when the table itself is destroyed. You can take ownership of a
    table item using takeItem() which you use to move a cell's
    contents from one cell to another, either within the same table,
    or from one table to another. (See also, swapCells()).

    In-place editing of the text in TQTableItems, and the values in
    TQComboTableItems and TQCheckTableItems works automatically. Cells
    may be editable or read-only, see \link
    qtableitem.html#wheneditable TQTableItem::EditType\endlink. If you
    want fine control over editing see beginEdit() and endEdit().

    The contents of a cell can be retrieved as a TQTableItem using
    item(), or as a string with text() or as a pixmap (if there is
    one) with pixmap(). A cell's bounding rectangle is given by
    cellGeometry(). Use updateCell() to repaint a cell, for example to
    clear away a cell's visual representation after it has been
    deleted with clearCell(). The table can be forced to scroll to
    show a particular cell with ensureCellVisible(). The isSelected()
    function indicates if a cell is selected.

    It is possible to use your own widget as a cell's widget using
    setCellWidget(), but subclassing TQTableItem might be a simpler
    approach. The cell's widget (if there is one) can be removed with
    clearCellWidget().

    \keyword notes on large tables
    \target bigtables
    \section2 Large tables

    For large, sparse, tables using TQTableItems or other widgets is
    inefficient. The solution is to \e draw the cell as it should
    appear and to create and destroy cell editors on demand.

    This approach requires that you reimplement various functions.
    Reimplement paintCell() to display your data, and createEditor()
    and setCellContentFromEditor() to support in-place editing. It
    is very important to reimplement resizeData() to have no
    functionality, to prevent TQTable from attempting to create a huge
    array. You will also need to reimplement item(), setItem(),
    takeItem(), clearCell(), and insertWidget(), cellWidget() and
    clearCellWidget(). In almost every circumstance (for sorting,
    removing and inserting columns and rows, etc.), you also need
    to reimplement swapRows(), swapCells() and swapColumns(), including
    header handling.

    If you represent active cells with a dictionary of TQTableItems and
    TQWidgets, i.e. only store references to cells that are actually
    used, many of the functions can be implemented with a single line
    of code. (See the \l table/bigtable/main.cpp example.)

    For more information on cells see the TQTableItem documenation.

    \target selections
    \section1 Selections

    TQTable's support single selection, multi-selection (multiple
    cells) or no selection. The selection mode is set with
    setSelectionMode(). Use isSelected() to determine if a particular
    cell is selected, and isRowSelected() and isColumnSelected() to
    see if a row or column is selected.

    TQTable's support many simultaneous selections. You can
    programmatically select cells with addSelection(). The number of
    selections is given by numSelections(). The current selection is
    returned by currentSelection(). You can remove a selection with
    removeSelection() and remove all selections with
    clearSelection(). Selections are TQTableSelection objects.

    To easily add a new selection use selectCells(), selectRow() or
    selectColumn().

    Alternatively, use addSelection() to add new selections using
    TQTableSelection objects. The advantage of using TQTableSelection
    objects is that you can call TQTableSelection::expandTo() to resize
    the selection and can query and compare them.

    The number of selections is given by numSelections(). The current
    selection is returned by currentSelection(). You can remove a
    selection with removeSelection() and remove all selections with
    clearSelection().

    \target signals
    \section1 Signals

    When the user clicks a cell the currentChanged() signal is
    emitted. You can also connect to the lower level clicked(),
    doubleClicked() and pressed() signals. If the user changes the
    selection the selectionChanged() signal is emitted; similarly if
    the user changes a cell's value the valueChanged() signal is
    emitted. If the user right-clicks (or presses the appropriate
    platform-specific key sequence) the contextMenuRequested() signal
    is emitted. If the user drops a drag and drop object the dropped()
    signal is emitted with the drop event.
*/

/*!
    \fn void TQTable::currentChanged( int row, int col )

    This signal is emitted when the current cell has changed to \a
    row, \a col.
*/

/*!
    \fn void TQTable::valueChanged( int row, int col )

    This signal is emitted when the user changed the value in the cell
    at \a row, \a col.
*/

/*!
    \fn int TQTable::currentRow() const

    Returns the current row.

    \sa currentColumn()
*/

/*!
    \fn int TQTable::currentColumn() const

    Returns the current column.

    \sa currentRow()
*/

/*!
    \enum TQTable::EditMode

    \value NotEditing  No cell is currently being edited.

    \value Editing  A cell is currently being edited. The editor was
    initialised with the cell's contents.

    \value Replacing  A cell is currently being edited. The editor was
    not initialised with the cell's contents.
*/

/*!
    \enum TQTable::SelectionMode

    \value NoSelection No cell can be selected by the user.

    \value Single The user may only select a single range of cells.

    \value Multi The user may select multiple ranges of cells.

    \value SingleRow The user may select one row at once.

    \value MultiRow The user may select multiple rows.
*/

/*!
    \enum TQTable::FocusStyle

    Specifies how the current cell (focus cell) is drawn.

    \value FollowStyle The current cell is drawn according to the
    current style and the cell's background is also drawn selected, if
    the current cell is within a selection

    \value SpreadSheet The current cell is drawn as in a spreadsheet.
    This means, it is signified by a black rectangle around the cell,
    and the background of the current cell is always drawn with the
    widget's base color - even when selected.

*/

/*!
    \fn void TQTable::clicked( int row, int col, int button, const TQPoint &mousePos )

    This signal is emitted when mouse button \a button is clicked. The
    cell where the event took place is at \a row, \a col, and the
    mouse's position is in \a mousePos.

    \sa TQt::ButtonState
*/

/*!
    \fn void TQTable::doubleClicked( int row, int col, int button, const TQPoint &mousePos )

    This signal is emitted when mouse button \a button is
    double-clicked. The cell where the event took place is at \a row,
    \a col, and the mouse's position is in \a mousePos.

    \sa TQt::ButtonState
*/

/*!
    \fn void TQTable::pressed( int row, int col, int button, const TQPoint &mousePos )

    This signal is emitted when mouse button \a button is pressed. The
    cell where the event took place is at \a row, \a col, and the
    mouse's position is in \a mousePos.

    \sa TQt::ButtonState
*/

/*!
    \fn void TQTable::selectionChanged()

    This signal is emitted whenever a selection changes.

    \sa TQTableSelection
*/

/*!
    \fn void TQTable::contextMenuRequested( int row, int col, const TQPoint & pos )

    This signal is emitted when the user invokes a context menu with
    the right mouse button (or with a system-specific keypress). The
    cell where the event took place is at \a row, \a col. \a pos is
    the position where the context menu will appear in the global
    coordinate system. This signal is always emitted, even if the
    contents of the cell are disabled.
*/

/*!
    Creates an empty table object called \a name as a child of \a
    parent.

    Call setNumRows() and setNumCols() to set the table size before
    populating the table if you're using TQTableItems.

    \sa TQWidget::clearWFlags() TQt::WidgetFlags
*/

TQTable::TQTable( TQWidget *parent, const char *name )
    : TQScrollView( parent, name, WNoAutoErase | WStaticContents ),
      leftHeader( 0 ), topHeader( 0 ),
      currentSel( 0 ), lastSortCol( -1 ), sGrid( true ), mRows( false ), mCols( false ),
      asc( true ), doSort( true ), readOnly( false )
{
    init( 0, 0 );
}

/*!
    Constructs an empty table called \a name with \a numRows rows and
    \a numCols columns. The table is a child of \a parent.

    If you're using \l{TQTableItem}s to populate the table's cells, you
    can create TQTableItem, TQComboTableItem and TQCheckTableItem items
    and insert them into the table using setItem(). (See the notes on
    large tables for an alternative to using TQTableItems.)

    \sa TQWidget::clearWFlags() TQt::WidgetFlags
*/

TQTable::TQTable( int numRows, int numCols, TQWidget *parent, const char *name )
    : TQScrollView( parent, name, WNoAutoErase | WStaticContents ),
      leftHeader( 0 ), topHeader( 0 ),
      currentSel( 0 ), lastSortCol( -1 ), sGrid( true ), mRows( false ), mCols( false ),
      asc( true ), doSort( true ), readOnly( false )
{
    init( numRows, numCols );
}

/*! \internal
*/

void TQTable::init( int rows, int cols )
{
#ifndef TQT_NO_DRAGANDDROP
    setDragAutoScroll( false );
#endif
    d = new TQTablePrivate;
    d->geomTimer = new TQTimer( this );
    d->lastVisCol = 0;
    d->lastVisRow = 0;
    connect( d->geomTimer, TQ_SIGNAL( timeout() ), this, TQ_SLOT( updateGeometriesSlot() ) );
    shouldClearSelection = false;
    dEnabled = false;
    roRows.setAutoDelete( true );
    roCols.setAutoDelete( true );
    setSorting( false );

    unused = true; // It's unused, ain't it? :)

    selMode = Multi;

    contents.setAutoDelete( true );
    widgets.setAutoDelete( true );

    // Enable clipper and set background mode
    enableClipper( qt_table_clipper_enabled );

    viewport()->setFocusProxy( this );
    viewport()->setFocusPolicy( WheelFocus );

    viewport()->setBackgroundMode( PaletteBase );
    setBackgroundMode( PaletteBackground, PaletteBase );
    setResizePolicy( Manual );
    selections.setAutoDelete( true );

    // Create headers
    leftHeader = new TQTableHeader( rows, this, this, "left table header" );
    leftHeader->setOrientation( Vertical );
    leftHeader->setTracking( true );
    leftHeader->setMovingEnabled( true );
    topHeader = new TQTableHeader( cols, this, this, "right table header" );
    topHeader->setOrientation( Horizontal );
    topHeader->setTracking( true );
    topHeader->setMovingEnabled( true );
    if ( TQApplication::reverseLayout() )
	setMargins( 0, fontMetrics().height() + 4, 30, 0 );
    else
	setMargins( 30, fontMetrics().height() + 4, 0, 0 );

    topHeader->setUpdatesEnabled( false );
    leftHeader->setUpdatesEnabled( false );
    // Initialize headers
    int i = 0;
    for ( i = 0; i < numCols(); ++i )
	topHeader->resizeSection( i, TQMAX( 100, TQApplication::globalStrut().height() ) );
    for ( i = 0; i < numRows(); ++i )
	leftHeader->resizeSection( i, TQMAX( 20, TQApplication::globalStrut().width() ) );
    topHeader->setUpdatesEnabled( true );
    leftHeader->setUpdatesEnabled( true );

    // Prepare for contents
    contents.setAutoDelete( false );

    // Connect header, table and scrollbars
    connect( horizontalScrollBar(), TQ_SIGNAL( valueChanged(int) ),
	     topHeader, TQ_SLOT( setOffset(int) ) );
    connect( verticalScrollBar(), TQ_SIGNAL( valueChanged(int) ),
	     leftHeader, TQ_SLOT( setOffset(int) ) );
    connect( topHeader, TQ_SIGNAL( sectionSizeChanged(int) ),
	     this, TQ_SLOT( columnWidthChanged(int) ) );
    connect( topHeader, TQ_SIGNAL( indexChange(int,int,int) ),
	     this, TQ_SLOT( columnIndexChanged(int,int,int) ) );
    connect( topHeader, TQ_SIGNAL( sectionClicked(int) ),
	     this, TQ_SLOT( columnClicked(int) ) );
    connect( leftHeader, TQ_SIGNAL( sectionSizeChanged(int) ),
	     this, TQ_SLOT( rowHeightChanged(int) ) );
    connect( leftHeader, TQ_SIGNAL( indexChange(int,int,int) ),
	     this, TQ_SLOT( rowIndexChanged(int,int,int) ) );

    // Initialize variables
    autoScrollTimer = new TQTimer( this );
    connect( autoScrollTimer, TQ_SIGNAL( timeout() ),
	     this, TQ_SLOT( doAutoScroll() ) );
    curRow = curCol = 0;
    topHeader->setSectionState( curCol, TQTableHeader::Bold );
    leftHeader->setSectionState( curRow, TQTableHeader::Bold );
    edMode = NotEditing;
    editRow = editCol = -1;

    drawActiveSelection = true;

    installEventFilter( this );

    focusStl = SpreadSheet;

    was_visible = false;

    // initial size
    resize( 640, 480 );
}

/*!
    Releases all the resources used by the TQTable object,
    including all \l{TQTableItem}s and their widgets.
*/

TQTable::~TQTable()
{
    setUpdatesEnabled( false );
    contents.setAutoDelete( true );
    contents.clear();
    widgets.clear();

    delete d;
}

void TQTable::setReadOnly( bool b )
{
    readOnly = b;

    TQTableItem *i = item(curRow, curCol);
    if (readOnly && isEditing()) {
	endEdit(editRow, editCol, true, false);
    } else if (!readOnly && i && (i->editType() == TQTableItem::WhenCurrent
				  || i->editType() == TQTableItem::Always)) {
	editCell(curRow, curCol);
    }
}

/*!
    If \a ro is true, row \a row is set to be read-only; otherwise the
    row is set to be editable.

    Whether a cell in this row is editable or read-only depends on the
    cell's EditType, and this setting:
    see \link qtableitem.html#wheneditable TQTableItem::EditType\endlink.

    \sa isRowReadOnly() setColumnReadOnly() setReadOnly()
*/

void TQTable::setRowReadOnly( int row, bool ro )
{
    if ( ro )
	roRows.replace( row, new int( 0 ) );
    else
	roRows.remove( row );

    if (curRow == row) {
	TQTableItem *i = item(curRow, curCol);
	if (ro && isEditing()) {
	    endEdit(editRow, editCol, true, false);
	} else if (!ro && i && (i->editType() == TQTableItem::WhenCurrent
				      || i->editType() == TQTableItem::Always)) {
	    editCell(curRow, curCol);
	}
    }
}

/*!
    If \a ro is true, column \a col is set to be read-only; otherwise
    the column is set to be editable.

    Whether a cell in this column is editable or read-only depends on
    the cell's EditType, and this setting:
    see \link qtableitem.html#wheneditable TQTableItem::EditType\endlink.

    \sa isColumnReadOnly() setRowReadOnly() setReadOnly()

*/

void TQTable::setColumnReadOnly( int col, bool ro )
{
    if ( ro )
	roCols.replace( col, new int( 0 ) );
    else
	roCols.remove( col );

    if (curCol == col) {
	TQTableItem *i = item(curRow, curCol);
	if (ro && isEditing()) {
	    endEdit(editRow, editCol, true, false);
	} else if (!ro && i && (i->editType() == TQTableItem::WhenCurrent
				      || i->editType() == TQTableItem::Always)) {
	    editCell(curRow, curCol);
	}
    }
}

/*!
    \property TQTable::readOnly
    \brief whether the table is read-only

    Whether a cell in the table is editable or read-only depends on
    the cell's \link TQTableItem::EditType EditType\endlink, and this setting:
    see \link qtableitem.html#wheneditable
    TQTableItem::EditType\endlink.

    \sa TQWidget::enabled setColumnReadOnly() setRowReadOnly()
*/

bool TQTable::isReadOnly() const
{
    return readOnly;
}

/*!
    Returns true if row \a row is read-only; otherwise returns false.

    Whether a cell in this row is editable or read-only depends on the
    cell's \link TQTableItem::EditType EditType\endlink, and this
    setting: see \link qtableitem.html#wheneditable
    TQTableItem::EditType\endlink.

    \sa setRowReadOnly() isColumnReadOnly()
*/

bool TQTable::isRowReadOnly( int row ) const
{
    return (roRows.find( row ) != 0);
}

/*!
    Returns true if column \a col is read-only; otherwise returns
    false.

    Whether a cell in this column is editable or read-only depends on
    the cell's EditType, and this setting: see \link
    qtableitem.html#wheneditable TQTableItem::EditType\endlink.

    \sa setColumnReadOnly() isRowReadOnly()
*/

bool TQTable::isColumnReadOnly( int col ) const
{
    return (roCols.find( col ) != 0);
}

void TQTable::setSelectionMode( SelectionMode mode )
{
    if ( mode == selMode )
	return;
    selMode = mode;
    clearSelection();
    if ( isRowSelection( selMode ) && numRows() > 0 && numCols() > 0 ) {
	currentSel = new TQTableSelection();
	selections.append( currentSel );
	currentSel->init( curRow, 0 );
	currentSel->expandTo( curRow, numCols() - 1 );
	repaintSelections( 0, currentSel );
    }
}

/*!
    \property TQTable::selectionMode
    \brief the current selection mode

    The default mode is \c Multi which allows the user to select
    multiple ranges of cells.

    \sa SelectionMode setSelectionMode()
*/

TQTable::SelectionMode TQTable::selectionMode() const
{
    return selMode;
}

/*!
    \property TQTable::focusStyle
    \brief how the current (focus) cell is drawn

    The default style is \c SpreadSheet.

    \sa TQTable::FocusStyle
*/

void TQTable::setFocusStyle( FocusStyle fs )
{
    focusStl = fs;
    updateCell( curRow, curCol );
}

TQTable::FocusStyle TQTable::focusStyle() const
{
    return focusStl;
}

/*!
    This functions updates all the header states to be in sync with
    the current selections. This should be called after
    programatically changing, adding or removing selections, so that
    the headers are updated.
*/

void TQTable::updateHeaderStates()
{
    horizontalHeader()->setUpdatesEnabled( false );
    verticalHeader()->setUpdatesEnabled( false );

    ( (TQTableHeader*)verticalHeader() )->setSectionStateToAll( TQTableHeader::Normal );
    ( (TQTableHeader*)horizontalHeader() )->setSectionStateToAll( TQTableHeader::Normal );

    TQPtrListIterator<TQTableSelection> it( selections );
    TQTableSelection *s;
    while ( ( s = it.current() ) != 0 ) {
	++it;
	if ( s->isActive() ) {
	    if ( s->leftCol() == 0 &&
		 s->rightCol() == numCols() - 1 ) {
		for ( int i = 0; i < s->bottomRow() - s->topRow() + 1; ++i )
		    leftHeader->setSectionState( s->topRow() + i, TQTableHeader::Selected );
	    }
	    if ( s->topRow() == 0 &&
		 s->bottomRow() == numRows() - 1 ) {
		for ( int i = 0; i < s->rightCol() - s->leftCol() + 1; ++i )
		    topHeader->setSectionState( s->leftCol() + i, TQTableHeader::Selected );
	    }
	}
    }

    horizontalHeader()->setUpdatesEnabled( true );
    verticalHeader()->setUpdatesEnabled( true );
    horizontalHeader()->repaint( false );
    verticalHeader()->repaint( false );
}

/*!
    Returns the table's top TQHeader.

    This header contains the column labels.

    To modify a column label use TQHeader::setLabel(), e.g.
    \quotefile table/statistics/statistics.cpp
    \skipto horizontalHeader
    \printline

    \sa verticalHeader() setTopMargin() TQHeader
*/

TQHeader *TQTable::horizontalHeader() const
{
    return (TQHeader*)topHeader;
}

/*!
    Returns the table's vertical TQHeader.

    This header contains the row labels.

    \sa horizontalHeader() setLeftMargin() TQHeader
*/

TQHeader *TQTable::verticalHeader() const
{
    return (TQHeader*)leftHeader;
}

void TQTable::setShowGrid( bool b )
{
    if ( sGrid == b )
	return;
    sGrid = b;
    updateContents();
}

/*!
    \property TQTable::showGrid
    \brief whether the table's grid is displayed

    The grid is shown by default.
*/

bool TQTable::showGrid() const
{
    return sGrid;
}

/*!
    \property TQTable::columnMovingEnabled
    \brief whether columns can be moved by the user

    The default is false. Columns are moved by dragging whilst holding
    down the Ctrl key.

    \warning If TQTable is used to move header sections as a result of user
    interaction, the mapping between header indexes and section exposed by
    TQHeader will not reflect the order of the headers in the table; i.e.,
    TQTable does not call TQHeader::moveSection() to move sections but handles
    move operations internally.

    \sa rowMovingEnabled
*/

void TQTable::setColumnMovingEnabled( bool b )
{
    mCols = b;
}

bool TQTable::columnMovingEnabled() const
{
    return mCols;
}

/*!
    \property TQTable::rowMovingEnabled
    \brief whether rows can be moved by the user

    The default is false. Rows are moved by dragging whilst holding
    down the Ctrl key.

    \warning If TQTable is used to move header sections as a result of user
    interaction, the mapping between header indexes and section exposed by
    TQHeader will not reflect the order of the headers in the table; i.e.,
    TQTable does not call TQHeader::moveSection() to move sections but handles
    move operations internally.

    \sa columnMovingEnabled
*/

void TQTable::setRowMovingEnabled( bool b )
{
    mRows = b;
}

bool TQTable::rowMovingEnabled() const
{
    return mRows;
}

/*!
    This is called when TQTable's internal array needs to be resized to
    \a len elements.

    If you don't use TQTableItems you should reimplement this as an
    empty method to avoid wasting memory. See the notes on large
    tables for further details.
*/

void TQTable::resizeData( int len )
{
    contents.resize( len );
    widgets.resize( len );
}

/*!
    Swaps the data in \a row1 and \a row2.

    This function is used to swap the positions of two rows. It is
    called when the user changes the order of rows (see
    setRowMovingEnabled()), and when rows are sorted.

    If you don't use \l{TQTableItem}s and want your users to be able to
    swap rows, e.g. for sorting, you will need to reimplement this
    function. (See the notes on large tables.)

    If \a swapHeader is true, the rows' header contents is also
    swapped.

    This function will not update the TQTable, you will have to do
    this manually, e.g. by calling updateContents().

    \sa swapColumns() swapCells()
*/

void TQTable::swapRows( int row1, int row2, bool swapHeader )
{
    if ( swapHeader )
	leftHeader->swapSections( row1, row2, false );

    TQPtrVector<TQTableItem> tmpContents;
    tmpContents.resize( numCols() );
    TQPtrVector<TQWidget> tmpWidgets;
    tmpWidgets.resize( numCols() );
    int i;

    contents.setAutoDelete( false );
    widgets.setAutoDelete( false );
    for ( i = 0; i < numCols(); ++i ) {
	TQTableItem *i1, *i2;
	i1 = item( row1, i );
	i2 = item( row2, i );
	if ( i1 || i2 ) {
	    tmpContents.insert( i, i1 );
	    contents.remove( indexOf( row1, i ) );
	    contents.insert( indexOf( row1, i ), i2 );
	    contents.remove( indexOf( row2, i ) );
	    contents.insert( indexOf( row2, i ), tmpContents[ i ] );
	    if ( contents[ indexOf( row1, i ) ] )
		contents[ indexOf( row1, i ) ]->setRow( row1 );
	    if ( contents[ indexOf( row2, i ) ] )
		contents[ indexOf( row2, i ) ]->setRow( row2 );
	}

	TQWidget *w1, *w2;
	w1 = cellWidget( row1, i );
	w2 = cellWidget( row2, i );
	if ( w1 || w2 ) {
	    tmpWidgets.insert( i, w1 );
	    widgets.remove( indexOf( row1, i ) );
	    widgets.insert( indexOf( row1, i ), w2 );
	    widgets.remove( indexOf( row2, i ) );
	    widgets.insert( indexOf( row2, i ), tmpWidgets[ i ] );
	}
    }
    contents.setAutoDelete( false );
    widgets.setAutoDelete( true );

    updateRowWidgets( row1 );
    updateRowWidgets( row2 );
    if ( curRow == row1 )
	curRow = row2;
    else if ( curRow == row2 )
	curRow = row1;
    if ( editRow == row1 )
	editRow = row2;
    else if ( editRow == row2 )
	editRow = row1;
}

/*!
    Sets the left margin to be \a m pixels wide.

    The verticalHeader(), which displays row labels, occupies this
    margin.

    In an Arabic or Hebrew localization, the verticalHeader() will
    appear on the right side of the table, and this call will set the
    right margin.

    \sa leftMargin() setTopMargin() verticalHeader()
*/

void TQTable::setLeftMargin( int m )
{
    if ( TQApplication::reverseLayout() )
	setMargins( leftMargin(), topMargin(), m, bottomMargin() );
    else
	setMargins( m, topMargin(), rightMargin(), bottomMargin() );
    updateGeometries();
}

/*!
    Sets the top margin to be \a m pixels high.

    The horizontalHeader(), which displays column labels, occupies
    this margin.

    \sa topMargin() setLeftMargin()
*/

void TQTable::setTopMargin( int m )
{
    setMargins( leftMargin(), m, rightMargin(), bottomMargin() );
    updateGeometries();
}

/*!
    Swaps the data in \a col1 with \a col2.

    This function is used to swap the positions of two columns. It is
    called when the user changes the order of columns (see
    setColumnMovingEnabled(), and when columns are sorted.

    If you don't use \l{TQTableItem}s and want your users to be able to
    swap columns you will need to reimplement this function. (See the
    notes on large tables.)

    If \a swapHeader is true, the columns' header contents is also
    swapped.

    \sa swapCells()
*/

void TQTable::swapColumns( int col1, int col2, bool swapHeader )
{
    if ( swapHeader )
	topHeader->swapSections( col1, col2, false );

    TQPtrVector<TQTableItem> tmpContents;
    tmpContents.resize( numRows() );
    TQPtrVector<TQWidget> tmpWidgets;
    tmpWidgets.resize( numRows() );
    int i;

    contents.setAutoDelete( false );
    widgets.setAutoDelete( false );
    for ( i = 0; i < numRows(); ++i ) {
	TQTableItem *i1, *i2;
	i1 = item( i, col1 );
	i2 = item( i, col2 );
	if ( i1 || i2 ) {
	    tmpContents.insert( i, i1 );
	    contents.remove( indexOf( i, col1 ) );
	    contents.insert( indexOf( i, col1 ), i2 );
	    contents.remove( indexOf( i, col2 ) );
	    contents.insert( indexOf( i, col2 ), tmpContents[ i ] );
	    if ( contents[ indexOf( i, col1 ) ] )
		contents[ indexOf( i, col1 ) ]->setCol( col1 );
	    if ( contents[ indexOf( i, col2 ) ] )
		contents[ indexOf( i, col2 ) ]->setCol( col2 );
	}

	TQWidget *w1, *w2;
	w1 = cellWidget( i, col1 );
	w2 = cellWidget( i, col2 );
	if ( w1 || w2 ) {
	    tmpWidgets.insert( i, w1 );
	    widgets.remove( indexOf( i, col1 ) );
	    widgets.insert( indexOf( i, col1 ), w2 );
	    widgets.remove( indexOf( i, col2 ) );
	    widgets.insert( indexOf( i, col2 ), tmpWidgets[ i ] );
	}
    }
    contents.setAutoDelete( false );
    widgets.setAutoDelete( true );

    columnWidthChanged( col1 );
    columnWidthChanged( col2 );
    if ( curCol == col1 )
	curCol = col2;
    else if ( curCol == col2 )
	curCol = col1;
    if ( editCol == col1 )
	editCol = col2;
    else if ( editCol == col2 )
	editCol = col1;
}

/*!
    Swaps the contents of the cell at \a row1, \a col1 with the
    contents of the cell at \a row2, \a col2.

    This function is also called when the table is sorted.

    If you don't use \l{TQTableItem}s and want your users to be able to
    swap cells, you will need to reimplement this function. (See the
    notes on large tables.)

    \sa swapColumns() swapRows()
*/

void TQTable::swapCells( int row1, int col1, int row2, int col2 )
{
    contents.setAutoDelete( false );
    widgets.setAutoDelete( false );
    TQTableItem *i1, *i2;
    i1 = item( row1, col1 );
    i2 = item( row2, col2 );
    if ( i1 || i2 ) {
	TQTableItem *tmp = i1;
	contents.remove( indexOf( row1, col1 ) );
	contents.insert( indexOf( row1, col1 ), i2 );
	contents.remove( indexOf( row2, col2 ) );
	contents.insert( indexOf( row2, col2 ), tmp );
	if ( contents[ indexOf( row1, col1 ) ] ) {
	    contents[ indexOf( row1, col1 ) ]->setRow( row1 );
	    contents[ indexOf( row1, col1 ) ]->setCol( col1 );
	}
	if ( contents[ indexOf( row2, col2 ) ] ) {
	    contents[ indexOf( row2, col2 ) ]->setRow( row2 );
	    contents[ indexOf( row2, col2 ) ]->setCol( col2 );
	}
    }

    TQWidget *w1, *w2;
    w1 = cellWidget( row1, col1 );
    w2 = cellWidget( row2, col2 );
    if ( w1 || w2 ) {
	TQWidget *tmp = w1;
	widgets.remove( indexOf( row1, col1 ) );
	widgets.insert( indexOf( row1, col1 ), w2 );
	widgets.remove( indexOf( row2, col2 ) );
	widgets.insert( indexOf( row2, col2 ), tmp );
    }

    updateRowWidgets( row1 );
    updateRowWidgets( row2 );
    updateColWidgets( col1 );
    updateColWidgets( col2 );
    contents.setAutoDelete( false );
    widgets.setAutoDelete( true );
}

static bool is_child_of( TQWidget *child, TQWidget *parent )
{
    while ( child ) {
	if ( child == parent )
	    return true;
	child = child->parentWidget();
    }
    return false;
}

/*!
    Draws the table contents on the painter \a p. This function is
    optimized so that it only draws the cells inside the \a cw pixels
    wide and \a ch pixels high clipping rectangle at position \a cx,
    \a cy.

    Additionally, drawContents() highlights the current cell.
*/

void TQTable::drawContents( TQPainter *p, int cx, int cy, int cw, int ch )
{
    int colfirst = columnAt( cx );
    int collast = columnAt( cx + cw );
    int rowfirst = rowAt( cy );
    int rowlast = rowAt( cy + ch );

    if ( rowfirst == -1 || colfirst == -1 ) {
	paintEmptyArea( p, cx, cy, cw, ch );
	return;
    }

    drawActiveSelection = hasFocus() || viewport()->hasFocus() || d->inMenuMode
			|| is_child_of( tqApp->focusWidget(), viewport() )
			|| !style().styleHint( TQStyle::SH_ItemView_ChangeHighlightOnFocus, this );
    if ( rowlast == -1 )
	rowlast = numRows() - 1;
    if ( collast == -1 )
	collast = numCols() - 1;

    bool currentInSelection = false;

    TQPtrListIterator<TQTableSelection> it( selections );
    TQTableSelection *s;
    while ( ( s = it.current() ) != 0 ) {
	++it;
	if ( s->isActive() &&
	     curRow >= s->topRow() &&
	     curRow <= s->bottomRow() &&
	     curCol >= s->leftCol() &&
	     curCol <= s->rightCol() ) {
	    currentInSelection = s->topRow() != curRow || s->bottomRow() != curRow || s->leftCol() != curCol || s->rightCol() != curCol;
	    break;
	}
    }

    // Go through the rows
    for ( int r = rowfirst; r <= rowlast; ++r ) {
	// get row position and height
	int rowp = rowPos( r );
	int rowh = rowHeight( r );

	// Go through the columns in row r
	// if we know from where to where, go through [colfirst, collast],
	// else go through all of them
	for ( int c = colfirst; c <= collast; ++c ) {
	    // get position and width of column c
	    int colp, colw;
	    colp = columnPos( c );
	    colw = columnWidth( c );
	    int oldrp = rowp;
	    int oldrh = rowh;

	    TQTableItem *itm = item( r, c );
	    if ( itm &&
		 ( itm->colSpan() > 1 || itm->rowSpan() > 1 ) ) {
		bool goon = (r == itm->row() && c == itm->col()) ||
			(r == rowfirst && c == itm->col()) ||
			(r == itm->row() && c == colfirst);
		if ( !goon )
		    continue;
		rowp = rowPos( itm->row() );
		rowh = 0;
		int i;
		for ( i = 0; i < itm->rowSpan(); ++i )
		    rowh += rowHeight( i + itm->row() );
		colp = columnPos( itm->col() );
		colw = 0;
		for ( i = 0; i < itm->colSpan(); ++i )
		    colw += columnWidth( i + itm->col() );
	    }

	    // Translate painter and draw the cell
	    p->translate( colp, rowp );
	    bool selected = isSelected( r, c );
	    if ( focusStl != FollowStyle && selected && !currentInSelection &&
		 r == curRow && c == curCol  )
		selected = false;
	    paintCell( p, r, c, TQRect( colp, rowp, colw, rowh ), selected );
	    p->translate( -colp, -rowp );

	    rowp = oldrp;
	    rowh = oldrh;

	    TQWidget *w = cellWidget( r, c );
	    TQRect cg( cellGeometry( r, c ) );
	    if ( w && w->geometry() != TQRect( contentsToViewport( cg.topLeft() ), cg.size() - TQSize( 1, 1 ) ) ) {
		moveChild( w, colp, rowp );
		w->resize( cg.size() - TQSize( 1, 1 ) );
	    }
	}
    }
    d->lastVisCol = collast;
    d->lastVisRow = rowlast;

    // draw indication of current cell
    TQRect focusRect = cellGeometry( curRow, curCol );
    p->translate( focusRect.x(), focusRect.y() );
    paintFocus( p, focusRect );
    p->translate( -focusRect.x(), -focusRect.y() );

    // Paint empty rects
    paintEmptyArea( p, cx, cy, cw, ch );

    drawActiveSelection = true;
}

/*!
    \reimp

    (Implemented to get rid of a compiler warning.)
*/

void TQTable::drawContents( TQPainter * )
{
}

/*!
    Returns the geometry of cell \a row, \a col in the cell's
    coordinate system. This is a convenience function useful in
    paintCell(). It is equivalent to TQRect( TQPoint(0,0), cellGeometry(
    row, col).size() );

    \sa cellGeometry()
*/

TQRect TQTable::cellRect( int row, int col ) const
{
    return TQRect( TQPoint(0,0), cellGeometry( row, col ).size() );
}

/*!
    \overload

    Use the other paintCell() function. This function is only included
    for backwards compatibilty.
*/

void TQTable::paintCell( TQPainter* p, int row, int col,
			const TQRect &cr, bool selected )
{
    if ( cr.width() == 0 || cr.height() == 0 )
	return;
#if defined(TQ_WS_WIN)
    const TQColorGroup &cg = ( !drawActiveSelection && style().styleHint( TQStyle::SH_ItemView_ChangeHighlightOnFocus ) ? palette().inactive() : colorGroup() );
#else
    const TQColorGroup &cg = colorGroup();
#endif

    TQTableItem *itm = item( row, col );
    TQColorGroup cg2( cg );
    if ( itm && !itm->isEnabled() )
	cg2 = palette().disabled();

    paintCell( p, row, col, cr, selected, cg2 );
}

/*!
    Paints the cell at \a row, \a col on the painter \a p. The painter
    has already been translated to the cell's origin. \a cr describes
    the cell coordinates in the content coordinate system.

    If \a selected is true the cell is highlighted.

    \a cg is the colorgroup which should be used to draw the cell
    content.

    If you want to draw custom cell content, for example right-aligned
    text, you must either reimplement paintCell(), or subclass
    TQTableItem and reimplement TQTableItem::paint() to do the custom
    drawing.

    If you're using a TQTableItem subclass, for example, to store a
    data structure, then reimplementing TQTableItem::paint() may be the
    best approach. For data you want to draw immediately, e.g. data
    retrieved from a database, it is probably best to reimplement
    paintCell(). Note that if you reimplement paintCell(), i.e. don't
    use \l{TQTableItem}s, you must reimplement other functions: see the
    notes on large tables.

    Note that the painter is not clipped by default in order to get
    maximum efficiency. If you want clipping, use code like this:

    \code
    p->setClipRect( cellRect(row, col), TQPainter::CoordPainter );
    //... your drawing code
    p->setClipping( false );
    \endcode
*/

void TQTable::paintCell( TQPainter *p, int row, int col,
			const TQRect &cr, bool selected, const TQColorGroup &cg )
{
    if ( focusStl == SpreadSheet && selected &&
	 row == curRow &&
	 col == curCol && ( hasFocus() || viewport()->hasFocus() ) )
	selected = false;

    int w = cr.width();
    int h = cr.height();
    int x2 = w - 1;
    int y2 = h - 1;


    TQTableItem *itm = item( row, col );
    if ( itm ) {
	p->save();
	itm->paint( p, cg, cr, selected );
	p->restore();
    } else {
	p->fillRect( 0, 0, w, h, selected ? cg.brush( TQColorGroup::Highlight ) : cg.brush( TQColorGroup::Base ) );
    }

    if ( sGrid ) {
	// Draw our lines
        TQPen pen( p->pen() );
        int gridColor = style().styleHint( TQStyle::SH_Table_GridLineColor, this );
        if (gridColor != -1) {
            const TQPalette &pal = palette();
            if (cg != colorGroup()
                && cg != pal.disabled()
                && cg != pal.inactive())
	        p->setPen(cg.mid());
            else
                p->setPen((TQRgb)gridColor);
        } else {
            p->setPen(cg.mid());
        }
        p->drawLine( x2, 0, x2, y2 );
	p->drawLine( 0, y2, x2, y2 );
	p->setPen( pen );
    }
}

/*!
    Draws the focus rectangle of the current cell (see currentRow(),
    currentColumn()).

    The painter \a p is already translated to the cell's origin, while
    \a cr specifies the cell's geometry in content coordinates.
*/

void TQTable::paintFocus( TQPainter *p, const TQRect &cr )
{
    if ( !hasFocus() && !viewport()->hasFocus() )
	return;
    TQRect focusRect( 0, 0, cr.width(), cr.height() );
    if ( focusStyle() == SpreadSheet ) {
	p->setPen( TQPen( black, 1 ) );
	p->setBrush( NoBrush );
	p->drawRect( focusRect.x(), focusRect.y(), focusRect.width() - 1, focusRect.height() - 1 );
	p->drawRect( focusRect.x() - 1, focusRect.y() - 1, focusRect.width() + 1, focusRect.height() + 1 );
    } else {
	TQColor c = isSelected( curRow, curCol, false ) ?
			     colorGroup().highlight() : colorGroup().base();
	style().drawPrimitive( TQStyle::PE_FocusRect, p, focusRect, colorGroup(),
			       ( isSelected( curRow, curCol, false ) ?
				 TQStyle::Style_FocusAtBorder :
				 TQStyle::Style_Default ),
			         TQStyleOption(c) );
    }
}

/*!
    This function fills the \a cw pixels wide and \a ch pixels high
    rectangle starting at position \a cx, \a cy with the background
    color using the painter \a p.

    paintEmptyArea() is invoked by drawContents() to erase or fill
    unused areas.
*/

void TQTable::paintEmptyArea( TQPainter *p, int cx, int cy, int cw, int ch )
{
    // Regions work with shorts, so avoid an overflow and adjust the
    // table size to the visible size
    TQSize ts( tableSize() );
    ts.setWidth( TQMIN( ts.width(), visibleWidth() ) );
    ts.setHeight( TQMIN( ts.height(), visibleHeight() ) );

    // Region of the rect we should draw, calculated in viewport
    // coordinates, as a region can't handle bigger coordinates
    contentsToViewport2( cx, cy, cx, cy );
    TQRegion reg( TQRect( cx, cy, cw, ch ) );

    // Subtract the table from it
    reg = reg.subtract( TQRect( TQPoint( 0, 0 ), ts ) );

    // And draw the rectangles (transformed inc contents coordinates as needed)
    TQMemArray<TQRect> r = reg.rects();
    for ( int i = 0; i < (int)r.count(); ++i )
	p->fillRect( TQRect(viewportToContents2(r[i].topLeft()),r[i].size()), viewport()->backgroundBrush() );
}

/*!
    Returns the TQTableItem representing the contents of the cell at \a
    row, \a col.

    If \a row or \a col are out of range or no content has been set
    for this cell, item() returns 0.

    If you don't use \l{TQTableItem}s you may need to reimplement this
    function: see the notes on large tables.

    \sa setItem()
*/

TQTableItem *TQTable::item( int row, int col ) const
{
    if ( row < 0 || col < 0 || row > numRows() - 1 ||
	 col > numCols() - 1 || row * col >= (int)contents.size() )
	return 0;

    return contents[ indexOf( row, col ) ];	// contents array lookup
}

/*!
    Inserts the table item \a item into the table at row \a row,
    column \a col, and repaints the cell. If a table item already
    exists in this cell it is deleted and replaced with \a item. The
    table takes ownership of the table item.

    If you don't use \l{TQTableItem}s you may need to reimplement this
    function: see the notes on large tables.

    \sa item() takeItem()
*/

void TQTable::setItem( int row, int col, TQTableItem *item )
{
    if ( !item )
	return;

    if ( (int)contents.size() != numRows() * numCols() )
	resizeData( numRows() * numCols() );

    int orow = item->row();
    int ocol = item->col();
    clearCell( row, col );

    contents.insert( indexOf( row, col ), item );
    item->setRow( row );
    item->setCol( col );
    item->t = this;
    updateCell( row, col );
    if ( qt_update_cell_widget )
	item->updateEditor( orow, ocol );

    if ( row == curRow && col == curCol && item->editType() == TQTableItem::WhenCurrent ) {
	if ( beginEdit( row, col, false ) )
	    setEditMode( Editing, row, col );
    }
}

/*!
    Removes the TQTableItem at \a row, \a col.

    If you don't use \l{TQTableItem}s you may need to reimplement this
    function: see the notes on large tables.
*/

void TQTable::clearCell( int row, int col )
{
    if ( (int)contents.size() != numRows() * numCols() )
	resizeData( numRows() * numCols() );
    clearCellWidget( row, col );
    contents.setAutoDelete( true );
    contents.remove( indexOf( row, col ) );
    contents.setAutoDelete( false );
}

/*!
    Sets the text in the cell at \a row, \a col to \a text.

    If the cell does not contain a table item a TQTableItem is created
    with an \link TQTableItem::EditType EditType\endlink of \c OnTyping,
    otherwise the existing table item's text (if any) is replaced with
    \a text.

    \sa text() setPixmap() setItem() TQTableItem::setText()
*/

void TQTable::setText( int row, int col, const TQString &text )
{
    TQTableItem *itm = item( row, col );
    if ( itm ) {
	itm->setText( text );
	itm->updateEditor( row, col );
	updateCell( row, col );
    } else {
	TQTableItem *i = new TQTableItem( this, TQTableItem::OnTyping,
					text, TQPixmap() );
	setItem( row, col, i );
    }
}

/*!
    Sets the pixmap in the cell at \a row, \a col to \a pix.

    If the cell does not contain a table item a TQTableItem is created
    with an \link TQTableItem::EditType EditType\endlink of \c OnTyping,
    otherwise the existing table item's pixmap (if any) is replaced
    with \a pix.

    Note that \l{TQComboTableItem}s and \l{TQCheckTableItem}s don't show
    pixmaps.

    \sa pixmap() setText() setItem() TQTableItem::setPixmap()
*/

void TQTable::setPixmap( int row, int col, const TQPixmap &pix )
{
    TQTableItem *itm = item( row, col );
    if ( itm ) {
	itm->setPixmap( pix );
	updateCell( row, col );
    } else {
	TQTableItem *i = new TQTableItem( this, TQTableItem::OnTyping,
					TQString::null, pix );
	setItem( row, col, i );
    }
}

/*!
    Returns the text in the cell at \a row, \a col, or TQString::null
    if the relevant item does not exist or has no text.

    \sa setText() setPixmap()
*/

TQString TQTable::text( int row, int col ) const
{
    TQTableItem *itm = item( row, col );
    if ( itm )
	return itm->text();
    return TQString::null;
}

/*!
    Returns the pixmap set for the cell at \a row, \a col, or a
    null-pixmap if the cell contains no pixmap.

    \sa setPixmap()
*/

TQPixmap TQTable::pixmap( int row, int col ) const
{
    TQTableItem *itm = item( row, col );
    if ( itm )
	return itm->pixmap();
    return TQPixmap();
}

/*!
    Moves the focus to the cell at \a row, \a col.

    \sa currentRow() currentColumn()
*/

void TQTable::setCurrentCell( int row, int col )
{
    setCurrentCell( row, col, true, true );
}

// need to use a define, as leftMargin() is protected
#define VERTICALMARGIN \
( TQApplication::reverseLayout() ? \
       rightMargin() \
       : \
       leftMargin() \
)

/*! \internal */

void TQTable::setCurrentCell( int row, int col, bool updateSelections, bool ensureVisible )
{
    TQTableItem *oldItem = item( curRow, curCol );

    if ( row > numRows() - 1 )
	row = numRows() - 1;
    if ( col > numCols() - 1 )
	col = numCols() - 1;

    if ( curRow == row && curCol == col )
	return;


    TQTableItem *itm = oldItem;
    if ( itm && itm->editType() != TQTableItem::Always && itm->editType() != TQTableItem::Never )
	endEdit( itm->row(), itm->col(), true, false );
    int oldRow = curRow;
    int oldCol = curCol;
    curRow = row;
    curCol = col;
    repaintCell( oldRow, oldCol );
    repaintCell( curRow, curCol );
    if ( ensureVisible )
        ensureCellVisible( curRow, curCol );
    emit currentChanged( row, col );

    if ( oldCol != curCol ) {
	if ( !isColumnSelected( oldCol ) )
	    topHeader->setSectionState( oldCol, TQTableHeader::Normal );
	else if ( isRowSelection( selectionMode() ) )
	    topHeader->setSectionState( oldCol, TQTableHeader::Selected );
	topHeader->setSectionState( curCol, isColumnSelected( curCol, true ) ?
				    TQTableHeader::Selected : TQTableHeader::Bold );
    }

    if ( oldRow != curRow ) {
	if ( !isRowSelected( oldRow ) )
	    leftHeader->setSectionState( oldRow, TQTableHeader::Normal );
	leftHeader->setSectionState( curRow, isRowSelected( curRow, true ) ?
				     TQTableHeader::Selected : TQTableHeader::Bold );
    }

    itm = item( curRow, curCol );

    TQPoint cellPos( columnPos( curCol ) + leftMargin() - contentsX(),
		    rowPos( curRow ) + topMargin() - contentsY() );
    setMicroFocusHint( cellPos.x(), cellPos.y(), columnWidth( curCol ),
		       rowHeight( curRow ), ( itm && itm->editType() != TQTableItem::Never ) );

    if ( cellWidget( oldRow, oldCol ) &&
	 cellWidget( oldRow, oldCol )->hasFocus() )
	viewport()->setFocus();

    if ( itm && itm->editType() == TQTableItem::WhenCurrent ) {
	if ( beginEdit( itm->row(), itm->col(), false ) )
	    setEditMode( Editing, itm->row(), itm->col() );
    } else if ( itm && itm->editType() == TQTableItem::Always ) {
	if ( cellWidget( itm->row(), itm->col() ) )
	    cellWidget( itm->row(), itm->col() )->setFocus();
    }

    if ( updateSelections && isRowSelection( selectionMode() ) &&
	 !isSelected( curRow, curCol, false ) ) {
	if ( selectionMode() == TQTable::SingleRow )
	    clearSelection();
	currentSel = new TQTableSelection();
	selections.append( currentSel );
	currentSel->init( curRow, 0 );
	currentSel->expandTo( curRow, numCols() - 1 );
	repaintSelections( 0, currentSel );
    }
}

/*!
    Scrolls the table until the cell at \a row, \a col becomes
    visible.
*/

void TQTable::ensureCellVisible( int row, int col )
{
    if ( !isUpdatesEnabled() || !viewport()->isUpdatesEnabled() )
	return;
    int cw = columnWidth( col );
    int rh = rowHeight( row );
    if ( cw < visibleWidth() )
	ensureVisible( columnPos( col ) + cw / 2, rowPos( row ) + rh / 2, cw / 2, rh / 2 );
    else
	ensureVisible( columnPos( col ), rowPos( row ) + rh / 2, 0, rh / 2 );
}

/*!
    Returns true if the cell at \a row, \a col is selected; otherwise
    returns false.

    \sa isRowSelected() isColumnSelected()
*/

bool TQTable::isSelected( int row, int col ) const
{
    return isSelected( row, col, true );
}

/*! \internal */

bool TQTable::isSelected( int row, int col, bool includeCurrent ) const
{
    TQPtrListIterator<TQTableSelection> it( selections );
    TQTableSelection *s;
    while ( ( s = it.current() ) != 0 ) {
	++it;
	if ( s->isActive() &&
	     row >= s->topRow() &&
	     row <= s->bottomRow() &&
	     col >= s->leftCol() &&
	     col <= s->rightCol() )
	    return true;
	if ( includeCurrent && row == currentRow() && col == currentColumn() )
	    return true;
    }
    return false;
}

/*!
    Returns true if row \a row is selected; otherwise returns false.

    If \a full is false (the default), 'row is selected' means that at
    least one cell in the row is selected. If \a full is true, then 'row
    is selected' means every cell in the row is selected.

    \sa isColumnSelected() isSelected()
*/

bool TQTable::isRowSelected( int row, bool full ) const
{
    if ( !full ) {
	TQPtrListIterator<TQTableSelection> it( selections );
	TQTableSelection *s;
	while ( ( s = it.current() ) != 0 ) {
	    ++it;
	    if ( s->isActive() &&
		 row >= s->topRow() &&
		 row <= s->bottomRow() )
	    return true;
	if ( row == currentRow() )
	    return true;
	}
    } else {
	TQPtrListIterator<TQTableSelection> it( selections );
	TQTableSelection *s;
	while ( ( s = it.current() ) != 0 ) {
	    ++it;
	    if ( s->isActive() &&
		 row >= s->topRow() &&
		 row <= s->bottomRow() &&
		 s->leftCol() == 0 &&
		 s->rightCol() == numCols() - 1 )
		return true;
	}
    }
    return false;
}

/*!
    Returns true if column \a col is selected; otherwise returns false.

    If \a full is false (the default), 'column is selected' means that
    at least one cell in the column is selected. If \a full is true,
    then 'column is selected' means every cell in the column is
    selected.

    \sa isRowSelected() isSelected()
*/

bool TQTable::isColumnSelected( int col, bool full ) const
{
    if ( !full ) {
	TQPtrListIterator<TQTableSelection> it( selections );
	TQTableSelection *s;
	while ( ( s = it.current() ) != 0 ) {
	    ++it;
	    if ( s->isActive() &&
		 col >= s->leftCol() &&
		 col <= s->rightCol() )
	    return true;
	if ( col == currentColumn() )
	    return true;
	}
    } else {
	TQPtrListIterator<TQTableSelection> it( selections );
	TQTableSelection *s;
	while ( ( s = it.current() ) != 0 ) {
	    ++it;
	    if ( s->isActive() &&
		 col >= s->leftCol() &&
		 col <= s->rightCol() &&
		 s->topRow() == 0 &&
		 s->bottomRow() == numRows() - 1 )
		return true;
	}
    }
    return false;
}

/*!
    \property TQTable::numSelections
    \brief The number of selections.

    \sa currentSelection()
*/

int TQTable::numSelections() const
{
    return selections.count();
}

/*!
    Returns selection number \a num, or an inactive TQTableSelection if \a
    num is out of range (see TQTableSelection::isActive()).
*/

TQTableSelection TQTable::selection( int num ) const
{
    if ( num < 0 || num >= (int)selections.count() )
	return TQTableSelection();

    TQTableSelection *s = ( (TQTable*)this )->selections.at( num );
    return *s;
}

/*!
    Adds a selection described by \a s to the table and returns its
    number or -1 if the selection is invalid.

    Remember to call TQTableSelection::init() and
    TQTableSelection::expandTo() to make the selection valid (see also
    TQTableSelection::isActive(), or use the
    TQTableSelection(int,int,int,int) constructor).

    \sa numSelections() removeSelection() clearSelection()
*/

int TQTable::addSelection( const TQTableSelection &s )
{
    if ( !s.isActive() )
	return -1;

    const int maxr = numRows()-1;
    const int maxc = numCols()-1;
    TQTableSelection *sel = new TQTableSelection( TQMIN(s.anchorRow(), maxr), TQMIN(s.anchorCol(), maxc),
				    TQMIN(s.bottomRow(), maxr), TQMIN(s.rightCol(), maxc) );

    selections.append( sel );

    repaintSelections( 0, sel, true, true );

    emit selectionChanged();

    return selections.count() - 1;
}

/*!
    If the table has a selection, \a s, this selection is removed from
    the table.

    \sa addSelection() numSelections()
*/

void TQTable::removeSelection( const TQTableSelection &s )
{
    selections.setAutoDelete( false );
    for ( TQTableSelection *sel = selections.first(); sel; sel = selections.next() ) {
	if ( s == *sel ) {
	    selections.removeRef( sel );
	    repaintSelections( sel, 0, true, true );
	    if ( sel == currentSel )
		currentSel = 0;
	    delete sel;
	}
    }
    selections.setAutoDelete( true );
    emit selectionChanged();
}

/*!
    \overload

    Removes selection number \a num from the table.

    \sa numSelections() addSelection() clearSelection()
*/

void TQTable::removeSelection( int num )
{
    if ( num < 0 || num >= (int)selections.count() )
	return;

    TQTableSelection *s = selections.at( num );
    if ( s == currentSel )
	currentSel = 0;
    selections.removeRef( s );
    repaintContents( false );
}

/*!
    Returns the number of the current selection or -1 if there is no
    current selection.

    \sa numSelections()
*/

int TQTable::currentSelection() const
{
    if ( !currentSel )
	return -1;
    return ( (TQTable*)this )->selections.findRef( currentSel );
}

/*! Selects the range starting at \a start_row and \a start_col and
  ending at \a end_row and \a end_col.

  \sa TQTableSelection
*/

void TQTable::selectCells( int start_row, int start_col, int end_row, int end_col )
{
    const int maxr = numRows()-1;
    const int maxc = numCols()-1;

    start_row = TQMIN( maxr, TQMAX( 0, start_row ) );
    start_col = TQMIN( maxc, TQMAX( 0, start_col ) );
    end_row = TQMIN( maxr, end_row );
    end_col = TQMIN( maxc, end_col );
    TQTableSelection sel( start_row, start_col, end_row, end_col );
    addSelection( sel );
}

/*! Selects the row \a row.

  \sa TQTableSelection
*/

// ### Make this virtual in 4.0 and remove hack for TQDataTable
void TQTable::selectRow( int row )
{
    row = TQMIN(numRows()-1, row);
    if ( row < 0 )
	return;
    bool isDataTable = false;
#ifndef TQT_NO_SQL
    isDataTable = ::tqt_cast<TQDataTable*>(this) != 0;
#endif
    if ( isDataTable || selectionMode() == SingleRow ) {
	setCurrentCell( row, currentColumn() );
    } else {
	TQTableSelection sel( row, 0, row, numCols() - 1 );
	addSelection( sel );
    }
}

/*! Selects the column \a col.

  \sa TQTableSelection
*/

// ### Make this virtual in 4.0
void TQTable::selectColumn( int col )
{
    col = TQMIN(numCols()-1, col);
    TQTableSelection sel( 0, col, numRows() - 1, col );
    addSelection( sel );
}

/*! \reimp
*/
void TQTable::contentsMousePressEvent( TQMouseEvent* e )
{
    contentsMousePressEventEx( e );
}

void TQTable::contentsMousePressEventEx( TQMouseEvent* e )
{
    shouldClearSelection = false;
    if ( isEditing() ) {
	if ( !cellGeometry( editRow, editCol ).contains( e->pos() ) ) {
	    endEdit( editRow, editCol, true, edMode != Editing );
	} else {
	    e->ignore();
	    return;
	}
    }

    d->redirectMouseEvent = false;

    int tmpRow = rowAt( e->pos().y() );
    int tmpCol = columnAt( e->pos().x() );
    pressedRow = tmpRow;
    pressedCol = tmpCol;
    fixRow( tmpRow, e->pos().y() );
    fixCol( tmpCol, e->pos().x() );
    startDragCol = -1;
    startDragRow = -1;

    if ( isSelected( tmpRow, tmpCol ) ) {
	startDragCol = tmpCol;
	startDragRow = tmpRow;
	dragStartPos = e->pos();
    }

    TQTableItem *itm = item( pressedRow, pressedCol );
    if ( itm && !itm->isEnabled() ) {
	emit pressed( tmpRow, tmpCol, e->button(), e->pos() );
	return;
    }

    if ( ( e->state() & ShiftButton ) == ShiftButton ) {
  	int oldRow = curRow;
  	int oldCol = curCol;
	setCurrentCell( tmpRow, tmpCol, selMode == SingleRow, true );
	if ( selMode != NoSelection && selMode != SingleRow ) {
	    if ( !currentSel ) {
		currentSel = new TQTableSelection();
		selections.append( currentSel );
		if ( !isRowSelection( selectionMode() ) )
		    currentSel->init( oldRow, oldCol );
		else
		    currentSel->init( oldRow, 0 );
	    }
	    TQTableSelection oldSelection = *currentSel;
	    if ( !isRowSelection( selectionMode() ) )
		currentSel->expandTo( tmpRow, tmpCol );
	    else
		currentSel->expandTo( tmpRow, numCols() - 1 );
	    repaintSelections( &oldSelection, currentSel );
	    emit selectionChanged();
	}
    } else if ( ( e->state() & ControlButton ) == ControlButton ) {
	setCurrentCell( tmpRow, tmpCol, false, true );
	if ( selMode != NoSelection ) {
	    if ( selMode == Single || ( selMode == SingleRow && !isSelected( tmpRow, tmpCol, false ) ) )
		clearSelection();
	    if ( !(selMode == SingleRow && isSelected( tmpRow, tmpCol, false )) ) {
		currentSel = new TQTableSelection();
		selections.append( currentSel );
		if ( !isRowSelection( selectionMode() ) ) {
		    currentSel->init( tmpRow, tmpCol );
		    currentSel->expandTo( tmpRow, tmpCol );
		} else {
		    currentSel->init( tmpRow, 0 );
		    currentSel->expandTo( tmpRow, numCols() - 1 );
		    repaintSelections( 0, currentSel );
		}
		emit selectionChanged();
	    }
	}
    } else {
	setCurrentCell( tmpRow, tmpCol, false, true );
	TQTableItem *itm = item( tmpRow, tmpCol );
	if ( itm && itm->editType() == TQTableItem::WhenCurrent ) {
	    TQWidget *w = cellWidget( tmpRow, tmpCol );
	    if ( ::tqt_cast<TQComboBox*>(w) || ::tqt_cast<TQButton*>(w) ) {
		TQMouseEvent ev( e->type(), w->mapFromGlobal( e->globalPos() ),
				e->globalPos(), e->button(), e->state() );
		TQApplication::sendPostedEvents( w, 0 );
		TQApplication::sendEvent( w, &ev );
		d->redirectMouseEvent = true;
	    }
	}
	if ( isSelected( tmpRow, tmpCol, false ) ) {
	    shouldClearSelection = true;
	} else {
	    bool b = signalsBlocked();
	    if ( selMode != NoSelection )
		blockSignals( true );
	    clearSelection();
	    blockSignals( b );
	    if ( selMode != NoSelection ) {
		currentSel = new TQTableSelection();
		selections.append( currentSel );
		if ( !isRowSelection( selectionMode() ) ) {
		    currentSel->init( tmpRow, tmpCol );
		    currentSel->expandTo( tmpRow, tmpCol );
		} else {
		    currentSel->init( tmpRow, 0 );
		    currentSel->expandTo( tmpRow, numCols() - 1 );
		    repaintSelections( 0, currentSel );
		}
		emit selectionChanged();
	    }
	}
    }

    emit pressed( tmpRow, tmpCol, e->button(), e->pos() );
}

/*! \reimp
*/

void TQTable::contentsMouseDoubleClickEvent( TQMouseEvent *e )
{
    if ( e->button() != LeftButton )
	return;
    if ( !isRowSelection( selectionMode() ) )
	clearSelection();
    int tmpRow = rowAt( e->pos().y() );
    int tmpCol = columnAt( e->pos().x() );
    TQTableItem *itm = item( tmpRow, tmpCol );
    if ( itm && !itm->isEnabled() )
	return;
    if ( tmpRow != -1 && tmpCol != -1 ) {
	if ( beginEdit( tmpRow, tmpCol, false ) )
	    setEditMode( Editing, tmpRow, tmpCol );
    }

    emit doubleClicked( tmpRow, tmpCol, e->button(), e->pos() );
}

/*!
    Sets the current edit mode to \a mode, the current edit row to \a
    row and the current edit column to \a col.

    \sa EditMode
*/

void TQTable::setEditMode( EditMode mode, int row, int col )
{
    edMode = mode;
    editRow = row;
    editCol = col;
}


/*! \reimp
*/

void TQTable::contentsMouseMoveEvent( TQMouseEvent *e )
{
    if ( (e->state() & MouseButtonMask) == NoButton )
	return;
    int tmpRow = rowAt( e->pos().y() );
    int tmpCol = columnAt( e->pos().x() );
    fixRow( tmpRow, e->pos().y() );
    fixCol( tmpCol, e->pos().x() );

#ifndef TQT_NO_DRAGANDDROP
    if ( dragEnabled() && startDragRow != -1 && startDragCol != -1 ) {
	if (TQPoint(dragStartPos - e->pos()).manhattanLength() > TQApplication::startDragDistance())
	    startDrag();
	return;
    }
#endif
    if ( selectionMode() == MultiRow && ( e->state() & ControlButton ) == ControlButton )
	shouldClearSelection = false;

    if ( shouldClearSelection ) {
	clearSelection();
	if ( selMode != NoSelection ) {
	    currentSel = new TQTableSelection();
	    selections.append( currentSel );
	    if ( !isRowSelection( selectionMode() ) )
		currentSel->init( tmpRow, tmpCol );
	    else
		currentSel->init( tmpRow, 0 );
	    emit selectionChanged();
	}
	shouldClearSelection = false;
    }

    TQPoint pos = mapFromGlobal( e->globalPos() );
    pos -= TQPoint( leftHeader->width(), topHeader->height() );
    autoScrollTimer->stop();
    doAutoScroll();
    if ( pos.x() < 0 || pos.x() > visibleWidth() || pos.y() < 0 || pos.y() > visibleHeight() )
	autoScrollTimer->start( 100, true );
}

/*! \internal
 */

void TQTable::doValueChanged()
{
    emit valueChanged( editRow, editCol );
}

/*! \internal
*/

void TQTable::doAutoScroll()
{
    TQPoint pos = TQCursor::pos();
    pos = mapFromGlobal( pos );
    pos -= TQPoint( leftHeader->width(), topHeader->height() );

    int tmpRow = curRow;
    int tmpCol = curCol;
    if ( pos.y() < 0 )
	tmpRow--;
    else if ( pos.y() > visibleHeight() )
	tmpRow++;
    if ( pos.x() < 0 )
	tmpCol--;
    else if ( pos.x() > visibleWidth() )
	tmpCol++;

    pos += TQPoint( contentsX(), contentsY() );
    if ( tmpRow == curRow )
	tmpRow = rowAt( pos.y() );
    if ( tmpCol == curCol )
	tmpCol = columnAt( pos.x() );
    pos -= TQPoint( contentsX(), contentsY() );

    fixRow( tmpRow, pos.y() );
    fixCol( tmpCol, pos.x() );

    if ( tmpRow < 0 || tmpRow > numRows() - 1 )
	tmpRow = currentRow();
    if ( tmpCol < 0 || tmpCol > numCols() - 1 )
	tmpCol = currentColumn();

    ensureCellVisible( tmpRow, tmpCol );

    if ( currentSel && selMode != NoSelection ) {
	TQTableSelection oldSelection = *currentSel;
	bool useOld = true;
	if ( selMode != SingleRow ) {
	    if ( !isRowSelection( selectionMode() ) ) {
		currentSel->expandTo( tmpRow, tmpCol );
	    } else {
		currentSel->expandTo( tmpRow, numCols() - 1 );
	    }
	} else {
	    bool currentInSelection = tmpRow == curRow && isSelected( tmpRow, tmpCol );
	    if ( !currentInSelection ) {
		useOld = false;
		clearSelection();
		currentSel = new TQTableSelection();
		selections.append( currentSel );
		currentSel->init( tmpRow, 0 );
		currentSel->expandTo( tmpRow, numCols() - 1 );
		repaintSelections( 0, currentSel );
	    } else {
		currentSel->expandTo( tmpRow, numCols() - 1 );
	    }
	}
	setCurrentCell( tmpRow, tmpCol, false, true );
	repaintSelections( useOld ? &oldSelection : 0, currentSel );
	if ( currentSel && oldSelection != *currentSel )
	    emit selectionChanged();
    } else {
	setCurrentCell( tmpRow, tmpCol, false, true );
    }

    if ( pos.x() < 0 || pos.x() > visibleWidth() || pos.y() < 0 || pos.y() > visibleHeight() )
	autoScrollTimer->start( 100, true );
}

/*! \reimp
*/

void TQTable::contentsMouseReleaseEvent( TQMouseEvent *e )
{
    if ( pressedRow == curRow && pressedCol == curCol )
	emit clicked( curRow, curCol, e->button(), e->pos() );

    if ( e->button() != LeftButton )
	return;
    if ( shouldClearSelection ) {
	int tmpRow = rowAt( e->pos().y() );
	int tmpCol = columnAt( e->pos().x() );
	fixRow( tmpRow, e->pos().y() );
	fixCol( tmpCol, e->pos().x() );
	clearSelection();
	if ( selMode != NoSelection ) {
	    currentSel = new TQTableSelection();
	    selections.append( currentSel );
	    if ( !isRowSelection( selectionMode() ) ) {
		currentSel->init( tmpRow, tmpCol );
	    } else {
		currentSel->init( tmpRow, 0 );
		currentSel->expandTo( tmpRow, numCols() - 1 );
		repaintSelections( 0, currentSel );
	    }
	    emit selectionChanged();
	}
	shouldClearSelection = false;
    }
    autoScrollTimer->stop();

    if ( d->redirectMouseEvent && pressedRow == curRow && pressedCol == curCol &&
	 item( pressedRow, pressedCol ) && item( pressedRow, pressedCol )->editType() ==
	 TQTableItem::WhenCurrent ) {
	TQWidget *w = cellWidget( pressedRow, pressedCol );
	if ( w ) {
	    TQMouseEvent ev( e->type(), w->mapFromGlobal( e->globalPos() ),
			    e->globalPos(), e->button(), e->state() );
	    TQApplication::sendPostedEvents( w, 0 );
	    TQApplication::sendEvent( w, &ev );
	}
    }
}

/*!
  \reimp
*/

void TQTable::contentsContextMenuEvent( TQContextMenuEvent *e )
{
    if ( !receivers( TQ_SIGNAL(contextMenuRequested(int,int,const TQPoint&)) ) ) {
	e->ignore();
	return;
    }
    if ( e->reason() == TQContextMenuEvent::Keyboard ) {
	TQRect r = cellGeometry( curRow, curCol );
	emit contextMenuRequested( curRow, curCol, viewport()->mapToGlobal( contentsToViewport( r.center() ) ) );
    } else {
	int tmpRow = rowAt( e->pos().y() );
	int tmpCol = columnAt( e->pos().x() );
	emit contextMenuRequested( tmpRow, tmpCol, e->globalPos() );
    }
}


/*! \reimp
*/

bool TQTable::eventFilter( TQObject *o, TQEvent *e )
{
    TQWidget *editorWidget = cellWidget( editRow, editCol );
    switch ( e->type() ) {
    case TQEvent::KeyPress: {
	TQTableItem *itm = item( curRow, curCol );
          int editRow = this->editRow;
          int editCol = this->editCol;
          if ((d->hasRowSpan || d->hasColSpan) && !editorWidget) {
              if (TQTableItem *eitm = item(editRow, editCol)) {
                  editRow = eitm->row();
                  editCol = eitm->col();
                  editorWidget = cellWidget(editRow, editCol);
              }
          }
	if ( isEditing() && editorWidget && o == editorWidget ) {
	    itm = item( editRow, editCol );
	    TQKeyEvent *ke = (TQKeyEvent*)e;
	    if ( ke->key() == Key_Escape ) {
		if ( !itm || itm->editType() == TQTableItem::OnTyping )
		    endEdit( editRow, editCol, false, edMode != Editing );
		return true;
	    }

	    if ( ( ke->state() == NoButton || ke->state() == Keypad )
		&& ( ke->key() == Key_Return || ke->key() == Key_Enter ) ) {
		if ( !itm || itm->editType() == TQTableItem::OnTyping )
		    endEdit( editRow, editCol, true, edMode != Editing );
		activateNextCell();
		return true;
	    }

	    if ( ke->key() == Key_Tab || ke->key() == Key_BackTab ) {
		if ( ke->state() & TQt::ControlButton )
		    return false;
		if ( !itm || itm->editType() == TQTableItem::OnTyping )
		    endEdit( editRow, editCol, true, edMode != Editing );
		if ( (ke->key() == Key_Tab) && !(ke->state() & ShiftButton) ) {
		    if ( currentColumn() >= numCols() - 1 )
			return true;
		    int cc  = TQMIN( numCols() - 1, currentColumn() + 1 );
		    while ( cc < numCols() ) {
			TQTableItem *i = item( currentRow(), cc );
			if ( !d->hiddenCols.find( cc ) && !isColumnReadOnly( cc ) && (!i || i->isEnabled()) )
			    break;
			++cc;
		    }
		    setCurrentCell( currentRow(), cc );
		} else { // Key_BackTab
		    if ( currentColumn() == 0 )
			return true;
		    int cc  = TQMAX( 0, currentColumn() - 1 );
		    while ( cc >= 0 ) {
			TQTableItem *i = item( currentRow(), cc );
			if ( !d->hiddenCols.find( cc ) && !isColumnReadOnly( cc ) && (!i || i->isEnabled()) )
			    break;
			--cc;
		    }
		    setCurrentCell( currentRow(), cc );
		}
		itm = item( curRow, curCol );
		if ( beginEdit( curRow, curCol, false ) )
		    setEditMode( Editing, curRow, curCol );
		return true;
	    }

	    if ( ( edMode == Replacing ||
		   ( itm && itm->editType() == TQTableItem::WhenCurrent ) ) &&
		 ( ke->key() == Key_Up || ke->key() == Key_Prior ||
		   ke->key() == Key_Home || ke->key() == Key_Down ||
		   ke->key() == Key_Next || ke->key() == Key_End ||
		   ke->key() == Key_Left || ke->key() == Key_Right ) ) {
		if ( !itm || itm->editType() == TQTableItem::OnTyping ) {
		    endEdit( editRow, editCol, true, edMode != Editing );
		}
		keyPressEvent( ke );
		return true;
	    }
	} else {
	    TQObjectList *l = viewport()->queryList( "TQWidget" );
	    if ( l && l->find( o ) != -1 ) {
                delete l;
		TQKeyEvent *ke = (TQKeyEvent*)e;
		if ( ( ke->state() & ControlButton ) == ControlButton ||
		     ( ke->key() != Key_Left && ke->key() != Key_Right &&
		       ke->key() != Key_Up && ke->key() != Key_Down &&
		       ke->key() != Key_Prior && ke->key() != Key_Next &&
		       ke->key() != Key_Home && ke->key() != Key_End ) )
		    return false;
		keyPressEvent( (TQKeyEvent*)e );
		return true;
	    }
	    delete l;
	}

	} break;
    case TQEvent::FocusOut:
	if ( isEditing() && editorWidget && o == editorWidget && ( (TQFocusEvent*)e )->reason() != TQFocusEvent::Popup ) {
	    TQTableItem *itm = item( editRow, editCol );
	    if ( !itm || itm->editType() == TQTableItem::OnTyping ) {
		endEdit( editRow, editCol, true, edMode != Editing );
		return true;
	    }
	}
	break;
#ifndef TQT_NO_WHEELEVENT
    case TQEvent::Wheel:
	if ( o == this || o == viewport() ) {
	    TQWheelEvent* we = (TQWheelEvent*)e;
	    scrollBy( 0, -we->delta() );
	    we->accept();
	    return true;
	}
#endif
    default:
	break;
    }

    return TQScrollView::eventFilter( o, e );
}

void TQTable::fixCell( int &row, int &col, int key )
{
    if ( rowHeight( row ) > 0 && columnWidth( col ) > 0 )
	return;
    if ( rowHeight( row ) <= 0 ) {
	if ( key == Key_Down ||
	     key == Key_Next ||
	     key == Key_End ) {
	    while ( row < numRows() && rowHeight( row ) <= 0 )
		row++;
	    if ( rowHeight( row ) <= 0 )
		row = curRow;
	} else if ( key == Key_Up ||
		    key == Key_Prior ||
		    key == Key_Home ) {
	    while ( row >= 0 && rowHeight( row ) <= 0 )
		row--;
	    if ( rowHeight( row ) <= 0 )
		row = curRow;
	}
    } else if ( columnWidth( col ) <= 0 ) {
	if ( key == Key_Left ) {
	    while ( col >= 0 && columnWidth( col ) <= 0 )
		col--;
	    if ( columnWidth( col ) <= 0 )
		col = curCol;
	} else if ( key == Key_Right ) {
	    while ( col < numCols() && columnWidth( col ) <= 0 )
		col++;
	    if ( columnWidth( col ) <= 0 )
		col = curCol;
	}
    }
}

/*! \reimp
*/

void TQTable::keyPressEvent( TQKeyEvent* e )
{
    if ( isEditing() && item( editRow, editCol ) &&
	 item( editRow, editCol )->editType() == TQTableItem::OnTyping )
	return;

    int tmpRow = curRow;
    int tmpCol = curCol;
    int oldRow = tmpRow;
    int oldCol = tmpCol;

    bool navigationKey = false;
    int r;
    switch ( e->key() ) {
    case Key_Left:
	tmpCol = TQMAX( 0, tmpCol - 1 );
	navigationKey = true;
	break;
    case Key_Right:
	tmpCol = TQMIN( numCols() - 1, tmpCol + 1 );
	navigationKey = true;
	break;
    case Key_Up:
	tmpRow = TQMAX( 0, tmpRow - 1 );
	navigationKey = true;
	break;
    case Key_Down:
	tmpRow = TQMIN( numRows() - 1, tmpRow + 1 );
	navigationKey = true;
	break;
    case Key_Prior:
	r = TQMAX( 0, rowAt( rowPos( tmpRow ) - visibleHeight() ) );
	if ( r < tmpRow || tmpRow < 0 )
	    tmpRow = r;
	navigationKey = true;
	break;
    case Key_Next:
	r = TQMIN( numRows() - 1, rowAt( rowPos( tmpRow ) + visibleHeight() ) );
	if ( r > tmpRow )
	    tmpRow = r;
	else
	    tmpRow = numRows() - 1;
	navigationKey = true;
	break;
    case Key_Home:
	tmpRow = 0;
	navigationKey = true;
	break;
    case Key_End:
	tmpRow = numRows() - 1;
	navigationKey = true;
	break;
    case Key_F2:
	if ( beginEdit( tmpRow, tmpCol, false ) )
	    setEditMode( Editing, tmpRow, tmpCol );
	break;
    case Key_Enter: case Key_Return:
	activateNextCell();
	return;
    case Key_Tab: case Key_BackTab:
	if ( (e->key() == Key_Tab) && !(e->state() & ShiftButton) ) {
	    if ( currentColumn() >= numCols() - 1 )
		return;
	    int cc  = TQMIN( numCols() - 1, currentColumn() + 1 );
	    while ( cc < numCols() ) {
		TQTableItem *i = item( currentRow(), cc );
		if ( !d->hiddenCols.find( cc ) && !isColumnReadOnly( cc ) && (!i || i->isEnabled()) )
		    break;
		++cc;
	    }
	    setCurrentCell( currentRow(), cc );
	} else { // Key_BackTab
	    if ( currentColumn() == 0 )
		return;
	    int cc  = TQMAX( 0, currentColumn() - 1 );
	    while ( cc >= 0 ) {
		TQTableItem *i = item( currentRow(), cc );
		if ( !d->hiddenCols.find( cc ) && !isColumnReadOnly( cc ) && (!i || i->isEnabled()) )
		    break;
		--cc;
	    }
	    setCurrentCell( currentRow(), cc );
	}
	return;
    case Key_Escape:
	e->ignore();
	return;
    default: // ... or start in-place editing
	if ( e->text()[ 0 ].isPrint() ) {
	    TQTableItem *itm = item( tmpRow, tmpCol );
	    if ( !itm || itm->editType() == TQTableItem::OnTyping ) {
		TQWidget *w = beginEdit( tmpRow, tmpCol,
					itm ? itm->isReplaceable() : true );
		if ( w ) {
		    setEditMode( ( !itm || ( itm && itm->isReplaceable() )
				   ? Replacing : Editing ), tmpRow, tmpCol );
		    TQApplication::sendEvent( w, e );
		    return;
		}
	    }
	}
	e->ignore();
	return;
    }

    if ( navigationKey ) {
	fixCell( tmpRow, tmpCol, e->key() );
	if ( ( e->state() & ShiftButton ) == ShiftButton &&
	     selMode != NoSelection && selMode != SingleRow ) {
	    bool justCreated = false;
	    setCurrentCell( tmpRow, tmpCol, false, true );
	    if ( !currentSel ) {
		justCreated = true;
		currentSel = new TQTableSelection();
		selections.append( currentSel );
		if ( !isRowSelection( selectionMode() ) )
		    currentSel->init( oldRow, oldCol );
		else
		    currentSel->init( oldRow < 0 ? 0 : oldRow, 0 );
	    }
	    TQTableSelection oldSelection = *currentSel;
	    if ( !isRowSelection( selectionMode() ) )
		currentSel->expandTo( tmpRow, tmpCol );
	    else
		currentSel->expandTo( tmpRow, numCols() - 1 );
	    repaintSelections( justCreated ? 0 : &oldSelection, currentSel );
	    emit selectionChanged();
	} else {
	    setCurrentCell( tmpRow, tmpCol, false, true );
	    if ( !isRowSelection( selectionMode() ) ) {
		clearSelection();
	    } else {
		bool currentInSelection = tmpRow == oldRow && isSelected( tmpRow, tmpCol, false );
		if ( !currentInSelection ) {
		    bool hasOldSel = false;
		    TQTableSelection oldSelection;
		    if ( selectionMode() == MultiRow ) {
			bool b = signalsBlocked();
			blockSignals( true );
			clearSelection();
			blockSignals( b );
		    } else {
			if ( currentSel ) {
			    oldSelection = *currentSel;
			    hasOldSel = true;
			    selections.removeRef( currentSel );
			    leftHeader->setSectionState( oldSelection.topRow(), TQTableHeader::Normal );
			}
		    }
		    currentSel = new TQTableSelection();
		    selections.append( currentSel );
		    currentSel->init( tmpRow, 0 );
		    currentSel->expandTo( tmpRow, numCols() - 1 );
		    repaintSelections( hasOldSel ? &oldSelection : 0, currentSel, !hasOldSel );
		    emit selectionChanged();
		}
	    }
	}
    } else {
	setCurrentCell( tmpRow, tmpCol, false, true );
    }
}

/*! \reimp
*/

void TQTable::focusInEvent( TQFocusEvent* )
{
    d->inMenuMode = false;
    TQWidget *editorWidget = cellWidget( editRow, editCol );
    updateCell( curRow, curCol );
    if ( style().styleHint( TQStyle::SH_ItemView_ChangeHighlightOnFocus, this ) )
	repaintSelections();
    if ( isEditing() && editorWidget )
	editorWidget->setFocus();

    TQPoint cellPos( columnPos( curCol ) + leftMargin() - contentsX(), rowPos( curRow ) + topMargin() - contentsY() );
    TQTableItem *itm = item( curRow, curCol );
    setMicroFocusHint( cellPos.x(), cellPos.y(), columnWidth( curCol ), rowHeight( curRow ), ( itm && itm->editType() != TQTableItem::Never ) );
}


/*! \reimp
*/

void TQTable::focusOutEvent( TQFocusEvent* )
{
    updateCell( curRow, curCol );
    if (style().styleHint( TQStyle::SH_ItemView_ChangeHighlightOnFocus, this )) {
	d->inMenuMode =
	    TQFocusEvent::reason() == TQFocusEvent::Popup ||
	    (tqApp->focusWidget() && tqApp->focusWidget()->inherits("TQMenuBar"));
	if ( !d->inMenuMode )
	    repaintSelections();
    }
}

/*! \reimp
*/

TQSize TQTable::sizeHint() const
{
    if ( cachedSizeHint().isValid() )
	return cachedSizeHint();

    constPolish();

    TQSize s = tableSize();
    TQSize sh;
    if ( s.width() < 500 && s.height() < 500 ) {
	sh = TQSize( tableSize().width() + VERTICALMARGIN + 5,
		    tableSize().height() + topMargin() + 5 );
    } else {
    	sh = TQScrollView::sizeHint();
    	if ( !topHeader->isHidden() )
		sh.setHeight( sh.height() + topHeader->height() );
    	if ( !leftHeader->isHidden() )
		sh.setWidth( sh.width() + leftHeader->width() );
    }
    setCachedSizeHint( sh );
    return sh;
}

/*! \reimp
*/

void TQTable::viewportResizeEvent( TQResizeEvent *e )
{
    TQScrollView::viewportResizeEvent( e );
    updateGeometries();
}

/*! \reimp
*/

void TQTable::showEvent( TQShowEvent *e )
{
    TQScrollView::showEvent( e );
    TQRect r( cellGeometry( numRows() - 1, numCols() - 1 ) );
    resizeContents( r.right() + 1, r.bottom() + 1 );
    updateGeometries();
}

/*! \reimp
*/

void TQTable::paintEvent( TQPaintEvent *e )
{
    TQRect topLeftCorner = TQStyle::visualRect( TQRect(frameWidth(), frameWidth(), VERTICALMARGIN, topMargin() ), rect() );
    erase( topLeftCorner ); // erase instead of widget on top
    TQScrollView::paintEvent( e );

#ifdef Q_OS_TEMP
    TQPainter p( this );
    p.drawLine( topLeftCorner.bottomLeft(), topLeftCorner.bottomRight() );
    p.drawLine( topLeftCorner.bottomRight(), topLeftCorner.topRight() );
#endif
}

static bool inUpdateCell = false;

/*!
    Repaints the cell at \a row, \a col.
*/

void TQTable::updateCell( int row, int col )
{
    if ( inUpdateCell || row < 0 || col < 0 )
	return;
    inUpdateCell = true;
    TQRect cg = cellGeometry( row, col );
    TQRect r( contentsToViewport( TQPoint( cg.x() - 2, cg.y() - 2 ) ),
	     TQSize( cg.width() + 4, cg.height() + 4 ) );
    if (viewport()->rect().intersects(r))
        TQApplication::postEvent( viewport(), new TQPaintEvent( r, false ) );
    inUpdateCell = false;
}

void TQTable::repaintCell( int row, int col )
{
    if ( row == -1 || col == -1 )
	return;
    TQRect cg = cellGeometry( row, col );
    TQRect r( TQPoint( cg.x() - 2, cg.y() - 2 ),
	     TQSize( cg.width() + 4, cg.height() + 4 ) );

    TQRect v = viewport()->rect();
    v.moveBy(contentsX(), contentsY());
    if (v.intersects(r))
        repaintContents( r, false );
}

void TQTable::contentsToViewport2( int x, int y, int& vx, int& vy )
{
    const TQPoint v = contentsToViewport2( TQPoint( x, y ) );
    vx = v.x();
    vy = v.y();
}

TQPoint TQTable::contentsToViewport2( const TQPoint &p )
{
    return TQPoint( p.x() - contentsX(),
		   p.y() - contentsY() );
}

TQPoint TQTable::viewportToContents2( const TQPoint& vp )
{
    return TQPoint( vp.x() + contentsX(),
		   vp.y() + contentsY() );
}

void TQTable::viewportToContents2( int vx, int vy, int& x, int& y )
{
    const TQPoint c = viewportToContents2( TQPoint( vx, vy ) );
    x = c.x();
    y = c.y();
}

/*!
    This function should be called whenever the column width of \a col
    has been changed. It updates the geometry of any affected columns
    and repaints the table to reflect the changes it has made.
*/

void TQTable::columnWidthChanged( int col )
{
    int p = columnPos( col );
    if ( d->hasColSpan )
	p = contentsX();
    updateContents( p, contentsY(), contentsWidth(), visibleHeight() );
    TQSize s( tableSize() );
    int w = contentsWidth();
    resizeContents( s.width(), s.height() );
    if ( contentsWidth() < w )
	repaintContents( s.width(), contentsY(),
			 w - s.width() + 1, visibleHeight(), true );
    else
	repaintContents( w, contentsY(),
			 s.width() - w + 1, visibleHeight(), false );

    // update widgets that are affected by this change
    if ( widgets.size() ) {
        int last = isHidden() ? numCols() - 1 : d->lastVisCol;
	for ( int c = col; c <= last; ++c )
	    updateColWidgets( c );
    }
    delayedUpdateGeometries();
}

/*!
    This function should be called whenever the row height of \a row
    has been changed. It updates the geometry of any affected rows and
    repaints the table to reflect the changes it has made.
*/

void TQTable::rowHeightChanged( int row )
{
    int p = rowPos( row );
    if ( d->hasRowSpan )
	p = contentsY();
    updateContents( contentsX(), p, visibleWidth(), contentsHeight() );
    TQSize s( tableSize() );
    int h = contentsHeight();
    resizeContents( s.width(), s.height() );
    if ( contentsHeight() < h ) {
	repaintContents( contentsX(), contentsHeight(),
			 visibleWidth(), h - s.height() + 1, true );
    } else {
	repaintContents( contentsX(), h,
			 visibleWidth(), s.height() - h + 1, false );
    }

    // update widgets that are affected by this change
    if ( widgets.size() ) {
	d->lastVisRow = rowAt( contentsY() + visibleHeight() + ( s.height() - h + 1 ) );
        int last = isHidden() ? numRows() - 1 : d->lastVisRow;
	for ( int r = row; r <= last; ++r )
	    updateRowWidgets( r );
    }
    delayedUpdateGeometries();
}

/*! \internal */

void TQTable::updateRowWidgets( int row )
{
    for ( int i = 0; i < numCols(); ++i ) {
	TQWidget *w = cellWidget( row, i );
	if ( !w )
	    continue;
	moveChild( w, columnPos( i ), rowPos( row ) );
	w->resize( columnWidth( i ) - 1, rowHeight( row ) - 1 );
    }
}

/*! \internal */

void TQTable::updateColWidgets( int col )
{
    for ( int i = 0; i < numRows(); ++i ) {
	TQWidget *w = cellWidget( i, col );
	if ( !w )
	    continue;
	moveChild( w, columnPos( col ), rowPos( i ) );
	w->resize( columnWidth( col ) - 1, rowHeight( i ) - 1 );
    }
}

/*!
    This function is called when column order is to be changed, i.e.
    when the user moved the column header \a section from \a fromIndex
    to \a toIndex.

    If you want to change the column order programmatically, call
    swapRows() or swapColumns();

    \sa TQHeader::indexChange() rowIndexChanged()
*/

void TQTable::columnIndexChanged( int, int fromIndex, int toIndex )
{
    if ( doSort && lastSortCol == fromIndex && topHeader )
	topHeader->setSortIndicator( toIndex, topHeader->sortIndicatorOrder() );
    repaintContents( contentsX(), contentsY(),
		     visibleWidth(), visibleHeight(), false );
}

/*!
    This function is called when the order of the rows is to be
    changed, i.e. the user moved the row header section \a section
    from \a fromIndex to \a toIndex.

    If you want to change the order programmatically, call swapRows()
    or swapColumns();

    \sa TQHeader::indexChange() columnIndexChanged()
*/

void TQTable::rowIndexChanged( int, int, int )
{
    repaintContents( contentsX(), contentsY(),
		     visibleWidth(), visibleHeight(), false );
}

/*!
    This function is called when the column \a col has been clicked.
    The default implementation sorts this column if sorting() is true.
*/

void TQTable::columnClicked( int col )
{
    if ( !sorting() )
	return;

    if ( col == lastSortCol ) {
	asc = !asc;
    } else {
	lastSortCol = col;
	asc = true;
    }
    sortColumn( lastSortCol, asc );
}

/*!
    \property TQTable::sorting
    \brief whether a click on the header of a column sorts that column

    \sa sortColumn()
*/

void TQTable::setSorting( bool b )
{
    doSort = b;
    if ( topHeader )
 	topHeader->setSortIndicator( b ? lastSortCol : -1 );
}

bool TQTable::sorting() const
{
    return doSort;
}

static bool inUpdateGeometries = false;

void TQTable::delayedUpdateGeometries()
{
    d->geomTimer->start( 0, true );
}

void TQTable::updateGeometriesSlot()
{
    updateGeometries();
}

/*!
    This function updates the geometries of the left and top header.
    You do not normally need to call this function.
*/

void TQTable::updateGeometries()
{
    if ( inUpdateGeometries )
	return;
    inUpdateGeometries = true;
    TQSize ts = tableSize();
    if ( topHeader->offset() &&
	 ts.width() < topHeader->offset() + topHeader->width() )
	horizontalScrollBar()->setValue( ts.width() - topHeader->width() );
    if ( leftHeader->offset() &&
	 ts.height() < leftHeader->offset() + leftHeader->height() )
	verticalScrollBar()->setValue( ts.height() - leftHeader->height() );

    leftHeader->setGeometry( TQStyle::visualRect( TQRect( frameWidth(), topMargin() + frameWidth(),
			     VERTICALMARGIN, visibleHeight() ), rect() ) );
    topHeader->setGeometry( TQStyle::visualRect( TQRect(VERTICALMARGIN + frameWidth(), frameWidth(),
						      visibleWidth(), topMargin() ), rect() ) );
    horizontalScrollBar()->raise();
    verticalScrollBar()->raise();
    topHeader->updateStretches();
    leftHeader->updateStretches();
    inUpdateGeometries = false;
}

/*!
    Returns the width of column \a col.

    \sa setColumnWidth() rowHeight()
*/

int TQTable::columnWidth( int col ) const
{
    return topHeader->sectionSize( col );
}

/*!
    Returns the height of row \a row.

    \sa setRowHeight() columnWidth()
*/

int TQTable::rowHeight( int row ) const
{
    return leftHeader->sectionSize( row );
}

/*!
    Returns the x-coordinate of the column \a col in content
    coordinates.

    \sa columnAt() rowPos()
*/

int TQTable::columnPos( int col ) const
{
    return topHeader->sectionPos( col );
}

/*!
    Returns the y-coordinate of the row \a row in content coordinates.

    \sa rowAt() columnPos()
*/

int TQTable::rowPos( int row ) const
{
    return leftHeader->sectionPos( row );
}

/*!
    Returns the number of the column at position \a x. \a x must be
    given in content coordinates.

    \sa columnPos() rowAt()
*/

int TQTable::columnAt( int x ) const
{
    return topHeader->sectionAt( x );
}

/*!
    Returns the number of the row at position \a y. \a y must be given
    in content coordinates.

    \sa rowPos() columnAt()
*/

int TQTable::rowAt( int y ) const
{
    return leftHeader->sectionAt( y );
}

/*!
    Returns the bounding rectangle of the cell at \a row, \a col in
    content coordinates.
*/

TQRect TQTable::cellGeometry( int row, int col ) const
{
    TQTableItem *itm = item( row, col );

    if ( !itm || ( itm->rowSpan() == 1 && itm->colSpan() == 1 ) )
	return TQRect( columnPos( col ), rowPos( row ),
		      columnWidth( col ), rowHeight( row ) );

    while ( row != itm->row() )
	row--;
    while ( col != itm->col() )
	col--;

    TQRect rect( columnPos( col ), rowPos( row ),
		columnWidth( col ), rowHeight( row ) );

    for ( int r = 1; r < itm->rowSpan(); ++r )
	rect.setHeight( rect.height() + rowHeight( r + row ) );

    for ( int c = 1; c < itm->colSpan(); ++c )
	rect.setWidth( rect.width() + columnWidth( c + col ) );

    return rect;
}

/*!
    Returns the size of the table.

    This is the same as the coordinates of the bottom-right edge of
    the last table cell.
*/

TQSize TQTable::tableSize() const
{
    return TQSize( columnPos( numCols() - 1 ) + columnWidth( numCols() - 1 ),
		  rowPos( numRows() - 1 ) + rowHeight( numRows() - 1 ) );
}

/*!
    \property TQTable::numRows
    \brief The number of rows in the table

    \sa numCols
*/

int TQTable::numRows() const
{
    return leftHeader->count();
}

/*!
    \property TQTable::numCols
    \brief The number of columns in the table

    \sa numRows
*/

int TQTable::numCols() const
{
    return topHeader->count();
}

void TQTable::saveContents( TQPtrVector<TQTableItem> &tmp,
			   TQPtrVector<TQTable::TableWidget> &tmp2)
{
    int nCols = numCols();
    if ( editRow != -1 && editCol != -1 )
	endEdit( editRow, editCol, false, edMode != Editing );
    tmp.resize( contents.size() );
    tmp2.resize( widgets.size() );
    int i;
    for ( i = 0; i < (int)tmp.size(); ++i ) {
	TQTableItem *item = contents[ i ];
	if ( item && ( item->row() * nCols) + item->col() == i )
	    tmp.insert( i, item );
	else
	    tmp.insert( i, 0 );
    }
    for ( i = 0; i < (int)tmp2.size(); ++i ) {
	TQWidget *w = widgets[ i ];
	if ( w )
	    tmp2.insert( i, new TableWidget( w, i / nCols, i % nCols ) );
	else
	    tmp2.insert( i, 0 );
    }
}

void TQTable::updateHeaderAndResizeContents( TQTableHeader *header,
					    int num, int rowCol,
					    int width, bool &updateBefore )
{
    updateBefore = rowCol < num;
    if ( rowCol > num ) {
	header->TQHeader::resizeArrays( rowCol );
	header->TQTableHeader::resizeArrays( rowCol );
	int old = num;
	clearSelection( false );
	int i = 0;
	for ( i = old; i < rowCol; ++i )
	    header->addLabel( TQString::null, width );
    } else {
	clearSelection( false );
	if ( header == leftHeader ) {
	    while ( numRows() > rowCol )
		header->removeLabel( numRows() - 1 );
	} else {
	    while ( numCols() > rowCol )
		header->removeLabel( numCols() - 1 );
	}
    }

    contents.setAutoDelete( false );
    contents.clear();
    contents.setAutoDelete( true );
    widgets.setAutoDelete( false );
    widgets.clear();
    widgets.setAutoDelete( true );
    resizeData( numRows() * numCols() );

    // keep numStretches in sync
    int n = 0;
    for ( uint i = 0; i < header->stretchable.size(); i++ )
	n += ( header->stretchable.at(i) & 1 ); // avoid cmp
     header->numStretches = n;
}

void TQTable::restoreContents( TQPtrVector<TQTableItem> &tmp,
			      TQPtrVector<TQTable::TableWidget> &tmp2 )
{
    int i;
    int nCols = numCols();
    for ( i = 0; i < (int)tmp.size(); ++i ) {
	TQTableItem *it = tmp[ i ];
	if ( it ) {
	    int idx = ( it->row() * nCols ) + it->col();
	    if ( (uint)idx < contents.size() &&
		 it->row() == idx /  nCols && it->col() == idx % nCols ) {
		contents.insert( idx, it );
		if ( it->rowSpan() > 1 || it->colSpan() > 1 ) {
		    int ridx, iidx;
		    for ( int irow = 0; irow < it->rowSpan(); irow++ ) {
			ridx = idx + irow * nCols;
			for ( int icol = 0; icol < it->colSpan(); icol++ ) {
			    iidx = ridx + icol;
			    if ( idx != iidx && (uint)iidx < contents.size() )
				contents.insert( iidx, it );
			}
		    }

		}
	    } else {
		delete it;
	    }
	}
    }
    for ( i = 0; i < (int)tmp2.size(); ++i ) {
	TableWidget *w = tmp2[ i ];
	if ( w ) {
	    int idx = ( w->row * nCols ) + w->col;
	    if ( (uint)idx < widgets.size() &&
		 w->row == idx / nCols && w->col == idx % nCols )
		widgets.insert( idx, w->wid );
	    else
		delete w->wid;
	    delete w;
	}
    }
}

void TQTable::finishContentsResze( bool updateBefore )
{
    TQRect r( cellGeometry( numRows() - 1, numCols() - 1 ) );
    resizeContents( r.right() + 1, r.bottom() + 1 );
    updateGeometries();
    if ( updateBefore )
	repaintContents( contentsX(), contentsY(),
			 visibleWidth(), visibleHeight(), true );
    else
	repaintContents( contentsX(), contentsY(),
			 visibleWidth(), visibleHeight(), false );

    if ( isRowSelection( selectionMode() ) ) {
	int r = curRow;
	curRow = -1;
	setCurrentCell( r, curCol );
    }
}

void TQTable::setNumRows( int r )
{
    if ( r < 0 )
	return;

    if (r < numRows()) {
	// Removed rows are no longer hidden, and should thus be removed from "hiddenRows"
	for (int rr = numRows()-1; rr >= r; --rr) {
	    if (d->hiddenRows.find(rr))
		d->hiddenRows.remove(rr);
	}
    }

    fontChange(font()); // invalidate the sizeHintCache

    TQPtrVector<TQTableItem> tmp;
    TQPtrVector<TableWidget> tmp2;
    saveContents( tmp, tmp2 );

    bool isUpdatesEnabled = leftHeader->isUpdatesEnabled();
    leftHeader->setUpdatesEnabled( false );

    bool updateBefore;
    updateHeaderAndResizeContents( leftHeader, numRows(), r, 20, updateBefore );

    int w = fontMetrics().width( TQString::number( r ) + "W" );
    if ( VERTICALMARGIN > 0 && w > VERTICALMARGIN )
	setLeftMargin( w );

    restoreContents( tmp, tmp2 );

    leftHeader->calculatePositions();
    finishContentsResze( updateBefore );
    leftHeader->setUpdatesEnabled( isUpdatesEnabled );
    if ( isUpdatesEnabled )
	leftHeader->update();
    leftHeader->updateCache();
    if ( curRow >= numRows() ) {
	curRow = numRows() - 1;
	if ( curRow < 0 )
	    curCol = -1;
	else
	    repaintCell( curRow, curCol );
    }

    if ( curRow > numRows() )
	curRow = numRows();
}

void TQTable::setNumCols( int c )
{
    if ( c < 0 )
	return;

    if (c < numCols()) {
	// Removed columns are no longer hidden, and should thus be removed from "hiddenCols"
	for (int cc = numCols()-1; cc >= c; --cc) {
	    if (d->hiddenCols.find(cc))
		d->hiddenCols.remove(cc);
	}
    }

    fontChange(font()); // invalidate the sizeHintCache

    TQPtrVector<TQTableItem> tmp;
    TQPtrVector<TableWidget> tmp2;
    saveContents( tmp, tmp2 );

    bool isUpdatesEnabled = topHeader->isUpdatesEnabled();
    topHeader->setUpdatesEnabled( false );

    bool updateBefore;
    updateHeaderAndResizeContents( topHeader, numCols(), c, 100, updateBefore );

    restoreContents( tmp, tmp2 );

    topHeader->calculatePositions();
    finishContentsResze( updateBefore );
    topHeader->setUpdatesEnabled( isUpdatesEnabled );
    if ( isUpdatesEnabled )
	topHeader->update();
    topHeader->updateCache();
    if ( curCol >= numCols() ) {
	curCol = numCols() - 1;
	if ( curCol < 0 )
	    curRow = -1;
	else
	    repaintCell( curRow, curCol );
    }
}

/*! Sets the section labels of the verticalHeader() to \a labels */

void TQTable::setRowLabels( const TQStringList &labels )
{
    leftHeader->setLabels(labels);
}

/*! Sets the section labels of the horizontalHeader() to \a labels */

void TQTable::setColumnLabels( const TQStringList &labels )
{
   topHeader->setLabels(labels);
}

/*!
    This function returns the widget which should be used as an editor
    for the contents of the cell at \a row, \a col.

    If \a initFromCell is true, the editor is used to edit the current
    contents of the cell (so the editor widget should be initialized
    with this content). If \a initFromCell is false, the content of
    the cell is replaced with the new content which the user entered
    into the widget created by this function.

    The default functionality is as follows: if \a initFromCell is
    true or the cell has a TQTableItem and the table item's
    TQTableItem::isReplaceable() is false then the cell is asked to
    create an appropriate editor (using TQTableItem::createEditor()).
    Otherwise a TQLineEdit is used as the editor.

    If you want to create your own editor for certain cells, implement
    a custom TQTableItem subclass and reimplement
    TQTableItem::createEditor().

    If you are not using \l{TQTableItem}s and you don't want to use a
    TQLineEdit as the default editor, subclass TQTable and reimplement
    this function with code like this:
    \code
    TQTableItem *i = item( row, col );
    if ( initFromCell || ( i && !i->isReplaceable() ) )
	// If we had a TQTableItem ask the base class to create the editor
	return TQTable::createEditor( row, col, initFromCell );
    else
	return ...(create your own editor)
    \endcode
    Ownership of the editor widget is transferred to the caller.

    If you reimplement this function return 0 for read-only cells. You
    will need to reimplement setCellContentFromEditor() to retrieve
    the data the user entered.

    \sa TQTableItem::createEditor()
*/

TQWidget *TQTable::createEditor( int row, int col, bool initFromCell ) const
{
    if ( isReadOnly() || isRowReadOnly( row ) || isColumnReadOnly( col ) )
	return 0;

    TQWidget *e = 0;

    // the current item in the cell should be edited if possible
    TQTableItem *i = item( row, col );
    if ( initFromCell || ( i && !i->isReplaceable() ) ) {
	if ( i ) {
	    if ( i->editType() == TQTableItem::Never )
		return 0;

	    e = i->createEditor();
	    if ( !e )
		return 0;
	}
    }

    // no contents in the cell yet, so open the default editor
    if ( !e ) {
	e = new TQLineEdit( viewport(), "qt_lineeditor" );
	( (TQLineEdit*)e )->setFrame( false );
    }

    return e;
}

/*!
    This function is called to start in-place editing of the cell at
    \a row, \a col. Editing is achieved by creating an editor
    (createEditor() is called) and setting the cell's editor with
    setCellWidget() to the newly created editor. (After editing is
    complete endEdit() will be called to replace the cell's content
    with the editor's content.) If \a replace is true the editor will
    start empty; otherwise it will be initialized with the cell's
    content (if any), i.e. the user will be modifying the original
    cell content.

    \sa endEdit()
*/

TQWidget *TQTable::beginEdit( int row, int col, bool replace )
{
    if ( isReadOnly() || isRowReadOnly( row ) || isColumnReadOnly( col ) )
	return 0;
    if ( row < 0 || row >= numRows() || col < 0 || col >= numCols() )
        return 0;

    TQTableItem *itm = item( row, col );
    if ( itm && !itm->isEnabled() )
	return 0;
    if ( cellWidget( row, col ) )
	return 0;
    ensureCellVisible( row, col );
    TQWidget *e = createEditor( row, col, !replace );
    if ( !e )
	return 0;
    setCellWidget( row, col, e );
    e->setActiveWindow();
    e->setFocus();
    updateCell( row, col );
    return e;
}

/*!
    This function is called when in-place editing of the cell at \a
    row, \a col is requested to stop.

    If the cell is not being edited or \a accept is false the function
    returns and the cell's contents are left unchanged.

    If \a accept is true the content of the editor must be transferred
    to the relevant cell. If \a replace is true the current content of
    this cell should be replaced by the content of the editor (this
    means removing the current TQTableItem of the cell and creating a
    new one for the cell). Otherwise (if possible) the content of the
    editor should just be set to the existing TQTableItem of this cell.

    setCellContentFromEditor() is called to replace the contents of
    the cell with the contents of the cell's editor.

    Finally clearCellWidget() is called to remove the editor widget.

    \sa setCellContentFromEditor(), beginEdit()
*/

void TQTable::endEdit( int row, int col, bool accept, bool replace )
{
    TQWidget *editor = cellWidget( row, col );
    if ( !editor )
	return;

    if ( !accept ) {
	if ( row == editRow && col == editCol )
	    setEditMode( NotEditing, -1, -1 );
	clearCellWidget( row, col );
	updateCell( row, col );
	viewport()->setFocus();
	updateCell( row, col );
	return;
    }

    TQTableItem *i = item( row, col );
    TQString oldContent;
    if ( i )
	oldContent = i->text();

    if ( !i || replace ) {
	setCellContentFromEditor( row, col );
	i = item( row, col );
    } else {
	i->setContentFromEditor( editor );
    }

    if ( row == editRow && col == editCol )
	setEditMode( NotEditing, -1, -1 );

    viewport()->setFocus();
    updateCell( row, col );

    if (!i || (oldContent != i->text()))
	emit valueChanged( row, col );

    clearCellWidget( row, col );
}

/*!
    This function is called to replace the contents of the cell at \a
    row, \a col with the contents of the cell's editor.

    If there already exists a TQTableItem for the cell,
    it calls TQTableItem::setContentFromEditor() on this TQTableItem.

    If, for example, you want to create different \l{TQTableItem}s
    depending on the contents of the editor, you might reimplement
    this function.

    If you want to work without \l{TQTableItem}s, you will need to
    reimplement this function to save the data the user entered into
    your data structure. (See the notes on large tables.)

    \sa TQTableItem::setContentFromEditor() createEditor()
*/

void TQTable::setCellContentFromEditor( int row, int col )
{
    TQWidget *editor = cellWidget( row, col );
    if ( !editor )
	return;

    TQTableItem *i = item( row, col );
    if ( i ) {
	i->setContentFromEditor( editor );
    } else {
	TQLineEdit *le = ::tqt_cast<TQLineEdit*>(editor);
	if ( le )
	    setText( row, col, le->text() );
    }
}

/*!
    Returns true if the \l EditMode is \c Editing or \c Replacing;
    otherwise (i.e. the \l EditMode is \c NotEditing) returns false.

    \sa TQTable::EditMode
*/

bool TQTable::isEditing() const
{
    return edMode != NotEditing;
}

/*!
    Returns the current edit mode

    \sa TQTable::EditMode
*/

TQTable::EditMode TQTable::editMode() const
{
    return edMode;
}

/*!
    Returns the current edited row
*/

int TQTable::currEditRow() const
{
    return editRow;
}

/*!
    Returns the current edited column
*/

int TQTable::currEditCol() const
{
    return editCol;
}

/*!
    Returns a single integer which identifies a particular \a row and \a
    col by mapping the 2D table to a 1D array.

    This is useful, for example, if you have a sparse table and want to
    use a TQIntDict to map integers to the cells that are used.
*/

int TQTable::indexOf( int row, int col ) const
{
    return ( row * numCols() ) + col;
}

/*! \internal
*/

void TQTable::repaintSelections( TQTableSelection *oldSelection,
				TQTableSelection *newSelection,
				bool updateVertical, bool updateHorizontal )
{
    if ( !oldSelection && !newSelection )
	return;
    if ( oldSelection && newSelection && *oldSelection == *newSelection )
	return;
    if ( oldSelection && !oldSelection->isActive() )
 	oldSelection = 0;

    bool optimizeOld = false;
    bool optimizeNew = false;

    TQRect old;
    if ( oldSelection )
	old = rangeGeometry( oldSelection->topRow(),
			     oldSelection->leftCol(),
			     oldSelection->bottomRow(),
			     oldSelection->rightCol(),
			     optimizeOld );
    else
	old = TQRect( 0, 0, 0, 0 );

    TQRect cur;
    if ( newSelection )
	cur = rangeGeometry( newSelection->topRow(),
			     newSelection->leftCol(),
			     newSelection->bottomRow(),
			     newSelection->rightCol(),
			     optimizeNew );
    else
	cur = TQRect( 0, 0, 0, 0 );
    int i;

    if ( !optimizeOld || !optimizeNew ||
	 old.width() > SHRT_MAX || old.height() > SHRT_MAX ||
	 cur.width() > SHRT_MAX || cur.height() > SHRT_MAX ) {
	TQRect rr = cur.unite( old );
	repaintContents( rr, false );
    } else {
	old = TQRect( contentsToViewport2( old.topLeft() ), old.size() );
	cur = TQRect( contentsToViewport2( cur.topLeft() ), cur.size() );
	TQRegion r1( old );
	TQRegion r2( cur );
 	TQRegion r3 = r1.subtract( r2 );
 	TQRegion r4 = r2.subtract( r1 );

	for ( i = 0; i < (int)r3.rects().count(); ++i ) {
	    TQRect r( r3.rects()[ i ] );
	    r = TQRect( viewportToContents2( r.topLeft() ), r.size() );
	    repaintContents( r, false );
	}
	for ( i = 0; i < (int)r4.rects().count(); ++i ) {
	    TQRect r( r4.rects()[ i ] );
	    r = TQRect( viewportToContents2( r.topLeft() ), r.size() );
	    repaintContents( r, false );
	}
    }

    int top, left, bottom, right;
    {
        int oldTopRow = oldSelection ? oldSelection->topRow() : numRows() - 1;
        int newTopRow = newSelection ? newSelection->topRow() : numRows() - 1;
        top = TQMIN(oldTopRow, newTopRow);
    }

    {
        int oldLeftCol = oldSelection ? oldSelection->leftCol() : numCols() - 1;
        int newLeftCol = newSelection ? newSelection->leftCol() : numCols() - 1;
        left = TQMIN(oldLeftCol, newLeftCol);
    }

    {
        int oldBottomRow = oldSelection ? oldSelection->bottomRow() : 0;
        int newBottomRow = newSelection ? newSelection->bottomRow() : 0;
        bottom = TQMAX(oldBottomRow, newBottomRow);
    }

    {
        int oldRightCol = oldSelection ? oldSelection->rightCol() : 0;
        int newRightCol = newSelection ? newSelection->rightCol() : 0;
        right = TQMAX(oldRightCol, newRightCol);
    }

    if ( updateHorizontal && numCols() > 0 && left >= 0 && !isRowSelection( selectionMode() ) ) {
	int *s = &topHeader->states.data()[left];
	for ( i = left; i <= right; ++i ) {
	    if ( !isColumnSelected( i ) )
		*s = TQTableHeader::Normal;
	    else if ( isColumnSelected( i, true ) )
		*s = TQTableHeader::Selected;
	    else
		*s = TQTableHeader::Bold;
	    ++s;
	}
	topHeader->repaint( false );
    }

    if ( updateVertical && numRows() > 0 && top >= 0 ) {
	int *s = &leftHeader->states.data()[top];
	for ( i = top; i <= bottom; ++i ) {
	    if ( !isRowSelected( i ) )
		*s = TQTableHeader::Normal;
	    else if ( isRowSelected( i, true ) )
		*s = TQTableHeader::Selected;
	    else
		*s = TQTableHeader::Bold;
	    ++s;
	}
	leftHeader->repaint( false );
    }
}

/*!
    Repaints all selections
*/

void TQTable::repaintSelections()
{
    if ( selections.isEmpty() )
	return;

    TQRect r;
    for ( TQTableSelection *s = selections.first(); s; s = selections.next() ) {
	bool b;
	r = r.unite( rangeGeometry( s->topRow(),
				    s->leftCol(),
				    s->bottomRow(),
				    s->rightCol(), b ) );
    }

    repaintContents( r, false );
}

/*!
    Clears all selections and repaints the appropriate regions if \a
    repaint is true.

    \sa removeSelection()
*/

void TQTable::clearSelection( bool repaint )
{
    if ( selections.isEmpty() )
	return;
    bool needRepaint = !selections.isEmpty();

    TQRect r;
    for ( TQTableSelection *s = selections.first(); s; s = selections.next() ) {
	bool b;
	r = r.unite( rangeGeometry( s->topRow(),
				    s->leftCol(),
				    s->bottomRow(),
				    s->rightCol(), b ) );
    }

    currentSel = 0;
    selections.clear();

    if ( needRepaint && repaint )
	repaintContents( r, false );

    leftHeader->setSectionStateToAll( TQTableHeader::Normal );
    leftHeader->repaint( false );
    if ( !isRowSelection( selectionMode() ) ) {
	topHeader->setSectionStateToAll( TQTableHeader::Normal );
	topHeader->repaint( false );
    }
    topHeader->setSectionState( curCol, TQTableHeader::Bold );
    leftHeader->setSectionState( curRow, TQTableHeader::Bold );
    emit selectionChanged();
}

/*! \internal
*/

TQRect TQTable::rangeGeometry( int topRow, int leftCol,
			     int bottomRow, int rightCol, bool &optimize )
{
    topRow = TQMAX( topRow, rowAt( contentsY() ) );
    leftCol = TQMAX( leftCol, columnAt( contentsX() ) );
    int ra = rowAt( contentsY() + visibleHeight() );
    if ( ra != -1 )
	bottomRow = TQMIN( bottomRow, ra );
    int ca = columnAt( contentsX() + visibleWidth() );
    if ( ca != -1 )
	rightCol = TQMIN( rightCol, ca );
    optimize = true;
    TQRect rect;
    for ( int r = topRow; r <= bottomRow; ++r ) {
	for ( int c = leftCol; c <= rightCol; ++c ) {
	    rect = rect.unite( cellGeometry( r, c ) );
	    TQTableItem *i = item( r, c );
	    if ( i && ( i->rowSpan() > 1 || i->colSpan() > 1 ) )
		optimize = false;
	}
    }
    return rect;
}

/*!
    This function is called to activate the next cell if in-place
    editing was finished by pressing the Enter key.

    The default behaviour is to move from top to bottom, i.e. move to
    the cell beneath the cell being edited. Reimplement this function
    if you want different behaviour, e.g. moving from left to right.
*/

void TQTable::activateNextCell()
{
    int firstRow = 0;
    while ( d->hiddenRows.find( firstRow ) )
	firstRow++;
    int firstCol = 0;
    while ( d->hiddenCols.find( firstCol ) )
	firstCol++;
    int nextRow = curRow;
    int nextCol = curCol;
    while ( d->hiddenRows.find( ++nextRow ) );
    if ( nextRow >= numRows() ) {
	nextRow = firstRow;
	while ( d->hiddenCols.find( ++nextCol ) );
	if ( nextCol >= numCols() )
	    nextCol = firstCol;
    }

    if ( !currentSel || !currentSel->isActive() ||
	 ( currentSel->leftCol() == currentSel->rightCol() &&
	   currentSel->topRow() == currentSel->bottomRow() ) ) {
	clearSelection();
	setCurrentCell( nextRow, nextCol );
    } else {
	if ( curRow < currentSel->bottomRow() )
	    setCurrentCell( nextRow, curCol );
	else if ( curCol < currentSel->rightCol() )
	    setCurrentCell( currentSel->topRow(), nextCol );
	else
	    setCurrentCell( currentSel->topRow(), currentSel->leftCol() );
    }

}

/*! \internal
*/

void TQTable::fixRow( int &row, int y )
{
    if ( row == -1 ) {
	if ( y < 0 )
	    row = 0;
	else
	    row = numRows() - 1;
    }
}

/*! \internal
*/

void TQTable::fixCol( int &col, int x )
{
    if ( col == -1 ) {
	if ( x < 0 )
	    col = 0;
	else
	    col = numCols() - 1;
    }
}

struct SortableTableItem
{
    TQTableItem *item;
};

#if defined(Q_C_CALLBACKS)
extern "C" {
#endif

#ifdef Q_OS_TEMP
static int _cdecl cmpTableItems( const void *n1, const void *n2 )
#else
static int cmpTableItems( const void *n1, const void *n2 )
#endif
{
    if ( !n1 || !n2 )
	return 0;

    SortableTableItem *i1 = (SortableTableItem *)n1;
    SortableTableItem *i2 = (SortableTableItem *)n2;

    return i1->item->key().localeAwareCompare( i2->item->key() );
}

#if defined(Q_C_CALLBACKS)
}
#endif

/*!
    Sorts column \a col. If \a ascending is true the sort is in
    ascending order, otherwise the sort is in descending order.

    If \a wholeRows is true, entire rows are sorted using swapRows();
    otherwise only cells in the column are sorted using swapCells().

    Note that if you are not using TQTableItems you will need to
    reimplement swapRows() and swapCells(). (See the notes on large
    tables.)

    \sa swapRows()
*/

void TQTable::sortColumn( int col, bool ascending, bool wholeRows )
{
    int filledRows = 0, i;
    for ( i = 0; i < numRows(); ++i ) {
	TQTableItem *itm = item( i, col );
	if ( itm )
	    filledRows++;
    }

    if ( !filledRows )
	return;

    SortableTableItem *items = new SortableTableItem[ filledRows ];
    int j = 0;
    for ( i = 0; i < numRows(); ++i ) {
	TQTableItem *itm = item( i, col );
	if ( !itm )
	    continue;
	items[ j++ ].item = itm;
    }

    qsort( items, filledRows, sizeof( SortableTableItem ), cmpTableItems );

    bool updatesEnabled = isUpdatesEnabled();
    setUpdatesEnabled( false );
    for ( i = 0; i < numRows(); ++i ) {
	if ( i < filledRows ) {
	    if ( ascending ) {
		if ( items[ i ].item->row() == i )
		    continue;
		if ( wholeRows )
		    swapRows( items[ i ].item->row(), i );
		else
		    swapCells( items[ i ].item->row(), col, i, col );
	    } else {
		if ( items[ i ].item->row() == filledRows - i - 1 )
		    continue;
		if ( wholeRows )
		    swapRows( items[ i ].item->row(), filledRows - i - 1 );
		else
		    swapCells( items[ i ].item->row(), col,
			       filledRows - i - 1, col );
	    }
	}
    }
    setUpdatesEnabled( updatesEnabled );
    if ( topHeader )
 	topHeader->setSortIndicator( col, ascending ? TQt::Ascending : TQt::Descending );

    if ( !wholeRows )
	repaintContents( columnPos( col ), contentsY(),
			 columnWidth( col ), visibleHeight(), false );
    else
	repaintContents( contentsX(), contentsY(),
			 visibleWidth(), visibleHeight(), false );

    delete [] items;
}

/*!
    Hides row \a row.

    \sa showRow() hideColumn()
*/

void TQTable::hideRow( int row )
{
    if ( d->hiddenRows.find( row ) )
	return;
    d->hiddenRows.replace( row, new int( leftHeader->sectionSize( row ) ) );
    leftHeader->resizeSection( row, 0 );
    leftHeader->setResizeEnabled( false, row );
    if ( isRowStretchable(row) )
	leftHeader->numStretches--;
    rowHeightChanged( row );
    if ( curRow == row ) {
	int r = curRow;
	int c = curCol;
	int k = ( r >= numRows() - 1 ? Key_Up : Key_Down );
	fixCell( r, c, k );
	if ( numRows() > 0 )
	    setCurrentCell( r, c );
    }
}

/*!
    Hides column \a col.

    \sa showColumn() hideRow()
*/

void TQTable::hideColumn( int col )
{
    if ( !numCols() || d->hiddenCols.find( col ) )
	return;
    d->hiddenCols.replace( col, new int( topHeader->sectionSize( col ) ) );
    topHeader->resizeSection( col, 0 );
    topHeader->setResizeEnabled( false, col );
    if ( isColumnStretchable(col) )
	topHeader->numStretches--;
    columnWidthChanged( col );
    if ( curCol == col ) {
	int r = curRow;
	int c = curCol;
	int k = ( c >= numCols() - 1 ? Key_Left : Key_Right );
	fixCell( r, c, k );
	if ( numCols() > 0 )
	    setCurrentCell( r, c );
    }
}

/*!
    Shows row \a row.

    \sa hideRow() showColumn()
*/

void TQTable::showRow( int row )
{
    int *h = d->hiddenRows.find( row );
    if ( h ) {
	int rh = *h;
	d->hiddenRows.remove( row );
	setRowHeight( row, rh );
	if ( isRowStretchable(row) )
	    leftHeader->numStretches++;
    } else if ( rowHeight( row ) == 0 ) {
	setRowHeight( row, 20 );
    }
    leftHeader->setResizeEnabled( true, row );
}

/*!
    Shows column \a col.

    \sa hideColumn() showRow()
*/

void TQTable::showColumn( int col )
{
    int *w = d->hiddenCols.find( col );
    if ( w ) {
	int cw = *w;
	d->hiddenCols.remove( col );
	setColumnWidth( col, cw );
	if ( isColumnStretchable( col ) )
	    topHeader->numStretches++;
    } else if ( columnWidth( col ) == 0 ) {
	setColumnWidth( col, 20 );
    }
    topHeader->setResizeEnabled( true, col );
}

/*!
    Returns true if row \a row is hidden; otherwise returns
    false.

    \sa hideRow(), isColumnHidden()
*/
bool TQTable::isRowHidden( int row ) const
{
    return d->hiddenRows.find( row );
}

/*!
    Returns true if column \a col is hidden; otherwise returns
    false.

    \sa hideColumn(), isRowHidden()
*/
bool TQTable::isColumnHidden( int col ) const
{
    return d->hiddenCols.find( col );
}

/*!
    Resizes column \a col to be \a w pixels wide.

    \sa columnWidth() setRowHeight()
*/

void TQTable::setColumnWidth( int col, int w )
{
    int *ow = d->hiddenCols.find( col );
    if ( ow ) {
	d->hiddenCols.replace( col, new int( w ) );
    } else {
	topHeader->resizeSection( col, w );
	columnWidthChanged( col );
    }
}

/*!
    Resizes row \a row to be \a h pixels high.

    \sa rowHeight() setColumnWidth()
*/

void TQTable::setRowHeight( int row, int h )
{
    int *oh = d->hiddenRows.find( row );
    if ( oh ) {
	d->hiddenRows.replace( row, new int( h ) );
    } else {
	leftHeader->resizeSection( row, h );
	rowHeightChanged( row );
    }
}

/*!
    Resizes column \a col so that the column width is wide enough to
    display the widest item the column contains.

    \sa adjustRow()
*/

void TQTable::adjustColumn( int col )
{
    int w;
    if ( currentColumn() == col ) {
        TQFont f = font();
        f.setBold(true);
        w = topHeader->sectionSizeHint( col, TQFontMetrics(f) ).width();
    } else {
        w = topHeader->sectionSizeHint( col, fontMetrics() ).width();
    }
    if ( topHeader->iconSet( col ) )
	w += topHeader->iconSet( col )->pixmap().width();
    w = TQMAX( w, 20 );
    for ( int i = 0; i < numRows(); ++i ) {
	TQTableItem *itm = item( i, col );
	if ( !itm ) {
	    TQWidget *widget = cellWidget( i, col );
	    if ( widget )
		w = TQMAX( w, widget->sizeHint().width() );
	} else {
	    if ( itm->colSpan() > 1 )
		w = TQMAX( w, itm->sizeHint().width() / itm->colSpan() );
	    else
		w = TQMAX( w, itm->sizeHint().width() );
	}
    }
    w = TQMAX( w, TQApplication::globalStrut().width() );
    setColumnWidth( col, w );
}

/*!
    Resizes row \a row so that the row height is tall enough to
    display the tallest item the row contains.

    \sa adjustColumn()
*/

void TQTable::adjustRow( int row )
{
    int h = 20;
    h = TQMAX( h, leftHeader->sectionSizeHint( row, leftHeader->fontMetrics() ).height() );
    if ( leftHeader->iconSet( row ) )
	h = TQMAX( h, leftHeader->iconSet( row )->pixmap().height() );
    for ( int i = 0; i < numCols(); ++i ) {
	TQTableItem *itm = item( row, i );
	if ( !itm ) {
	    TQWidget *widget = cellWidget( row, i );
	    if ( widget )
		h = TQMAX( h, widget->sizeHint().height() );
	} else {
	    if ( itm->rowSpan() > 1 )
		h = TQMAX( h, itm->sizeHint().height() / itm->rowSpan() );
	    else
		h = TQMAX( h, itm->sizeHint().height() );
	}
    }
    h = TQMAX( h, TQApplication::globalStrut().height() );
    setRowHeight( row, h );
}

/*!
    If \a stretch is true, column \a col is set to be stretchable;
    otherwise column \a col is set to be unstretchable.

    If the table widget's width decreases or increases stretchable
    columns will grow narrower or wider to fit the space available as
    completely as possible. The user cannot manually resize stretchable
    columns.

    \sa isColumnStretchable() setRowStretchable() adjustColumn()
*/

void TQTable::setColumnStretchable( int col, bool stretch )
{
    topHeader->setSectionStretchable( col, stretch );

    if ( stretch && d->hiddenCols.find(col) )
	topHeader->numStretches--;
}

/*!
    If \a stretch is true, row \a row is set to be stretchable;
    otherwise row \a row is set to be unstretchable.

    If the table widget's height decreases or increases stretchable
    rows will grow shorter or taller to fit the space available as
    completely as possible. The user cannot manually resize
    stretchable rows.

    \sa isRowStretchable() setColumnStretchable()
*/

void TQTable::setRowStretchable( int row, bool stretch )
{
    leftHeader->setSectionStretchable( row, stretch );

    if ( stretch && d->hiddenRows.find(row) )
	leftHeader->numStretches--;
}

/*!
    Returns true if column \a col is stretchable; otherwise returns
    false.

    \sa setColumnStretchable() isRowStretchable()
*/

bool TQTable::isColumnStretchable( int col ) const
{
    return topHeader->isSectionStretchable( col );
}

/*!
    Returns true if row \a row is stretchable; otherwise returns
    false.

    \sa setRowStretchable() isColumnStretchable()
*/

bool TQTable::isRowStretchable( int row ) const
{
    return leftHeader->isSectionStretchable( row );
}

/*!
    Takes the table item \a i out of the table. This function does \e
    not delete the table item. You must either delete the table item
    yourself or put it into a table (using setItem()) which will then
    take ownership of it.

    Use this function if you want to move an item from one cell in a
    table to another, or to move an item from one table to another,
    reinserting the item with setItem().

    If you want to exchange two cells use swapCells().
*/

void TQTable::takeItem( TQTableItem *i )
{
    if ( !i )
	return;
    TQRect rect = cellGeometry( i->row(), i->col() );
    contents.setAutoDelete( false );
    int bottom = i->row() + i->rowSpan();
    if ( bottom > numRows() )
	bottom = numRows();
    int right = i->col() + i->colSpan();
    if ( right > numCols() )
	right = numCols();
    for ( int r = i->row(); r < bottom; ++r ) {
	for ( int c = i->col(); c < right; ++c )
	    contents.remove( indexOf( r, c ) );
    }
    contents.setAutoDelete( true );
    repaintContents( rect, false );
    int orow = i->row();
    int ocol = i->col();
    i->setRow( -1 );
    i->setCol( -1 );
    i->updateEditor( orow, ocol );
    i->t = 0;
}

/*!
    Sets the widget \a e to the cell at \a row, \a col and takes care of
    placing and resizing the widget when the cell geometry changes.

    By default widgets are inserted into a vector with numRows() *
    numCols() elements. In very large tables you will probably want to
    store the widgets in a data structure that consumes less memory (see
    the notes on large tables). To support the use of your own data
    structure this function calls insertWidget() to add the widget to
    the internal data structure. To use your own data structure
    reimplement insertWidget(), cellWidget() and clearCellWidget().

    Cell widgets are created dynamically with the \c new operator. The
    cell widgets are destroyed automatically once the table is
    destroyed; the table takes ownership of the widget when using
    setCellWidget.

*/

void TQTable::setCellWidget( int row, int col, TQWidget *e )
{
    if ( !e || row >= numRows() || col >= numCols() )
	return;

    TQWidget *w = cellWidget( row, col );
    if ( w && row == editRow && col == editCol )
	endEdit( editRow, editCol, false, edMode != Editing );

    e->installEventFilter( this );
    clearCellWidget( row, col );
    if ( e->parent() != viewport() )
	e->reparent( viewport(), TQPoint( 0,0 ) );
    TQTableItem *itm = item(row, col);
    if (itm && itm->row() >= 0 && itm->col() >= 0) { // get the correct row and col if the item is spanning
        row = itm->row();
        col = itm->col();
    }
    insertWidget( row, col, e );
    TQRect cr = cellGeometry( row, col );
    e->resize( cr.size() );
    moveChild( e, cr.x(), cr.y() );
    e->show();
}

/*!
    Inserts widget \a w at \a row, \a col into the internal
    data structure. See the documentation of setCellWidget() for
    further details.

    If you don't use \l{TQTableItem}s you may need to reimplement this
    function: see the notes on large tables.
*/

void TQTable::insertWidget( int row, int col, TQWidget *w )
{
    if ( row < 0 || col < 0 || row > numRows() - 1 || col > numCols() - 1 )
	return;

    if ( (int)widgets.size() != numRows() * numCols() )
	widgets.resize( numRows() * numCols() );

    widgets.insert( indexOf( row, col ), w );
}

/*!
    Returns the widget that has been set for the cell at \a row, \a
    col, or 0 if no widget has been set.

    If you don't use \l{TQTableItem}s you may need to reimplement this
    function: see the notes on large tables.

    \sa clearCellWidget() setCellWidget()
*/

TQWidget *TQTable::cellWidget( int row, int col ) const
{
    if ( row < 0 || col < 0 || row > numRows() - 1 || col > numCols() - 1 )
	return 0;

    if ( (int)widgets.size() != numRows() * numCols() )
	( (TQTable*)this )->widgets.resize( numRows() * numCols() );

    return widgets[ indexOf( row, col ) ];
}

/*!
    Removes the widget (if there is one) set for the cell at \a row,
    \a col.

    If you don't use \l{TQTableItem}s you may need to reimplement this
    function: see the notes on large tables.

    This function deletes the widget at \a row, \a col. Note that the
    widget is not deleted immediately; instead TQObject::deleteLater()
    is called on the widget to avoid problems with timing issues.

    \sa cellWidget() setCellWidget()
*/

void TQTable::clearCellWidget( int row, int col )
{
    if ( row < 0 || col < 0 || row > numRows() - 1 || col > numCols() - 1 )
	return;

    if ( (int)widgets.size() != numRows() * numCols() )
	widgets.resize( numRows() * numCols() );

    TQWidget *w = cellWidget( row, col );
    if ( w ) {
	w->removeEventFilter( this );
	w->deleteLater();
    }
    widgets.setAutoDelete( false );
    widgets.remove( indexOf( row, col ) );
    widgets.setAutoDelete( true );
}

/*!
    \fn void TQTable::dropped ( TQDropEvent * e )

    This signal is emitted when a drop event occurred on the table.

    \a e contains information about the drop.
*/

/*!
    If \a b is true, the table starts a drag (see dragObject()) when
    the user presses and moves the mouse on a selected cell.
*/

void TQTable::setDragEnabled( bool b )
{
    dEnabled = b;
}

/*!
    If this function returns true, the table supports dragging.

    \sa setDragEnabled();
*/

bool TQTable::dragEnabled() const
{
    return dEnabled;
}

/*!
    Inserts \a count empty rows at row \a row. Also clears the selection(s).

    \sa insertColumns() removeRow()
*/

void TQTable::insertRows( int row, int count )
{
    // special case, so a call like insertRow( currentRow(), 1 ) also
    // works, when we have 0 rows and currentRow() is -1
    if ( row == -1 && curRow == -1 )
	row = 0;
    if ( row < 0 || count <= 0 )
	return;

    if ( curRow >= row && curRow < row + count )
	curRow = row + count;

    --row;
    if ( row >= numRows() )
	return;

    bool updatesEnabled = isUpdatesEnabled();
    setUpdatesEnabled( false );
    bool leftHeaderUpdatesEnabled = leftHeader->isUpdatesEnabled();
    leftHeader->setUpdatesEnabled( false );
    int oldLeftMargin = leftMargin();

    setNumRows( numRows() + count );

    for ( int i = numRows() - count - 1; i > row; --i )
	leftHeader->swapSections( i, i + count );

    leftHeader->setUpdatesEnabled( leftHeaderUpdatesEnabled );
    setUpdatesEnabled( updatesEnabled );

    int cr = TQMAX( 0, currentRow() );
    int cc = TQMAX( 0, currentColumn() );
    if ( curRow > row )
	curRow -= count; // this is where curRow was
    setCurrentCell( cr, cc, true, false ); // without ensureCellVisible

    // Repaint the header
    if ( leftHeaderUpdatesEnabled ) {
	int y = rowPos( row ) - contentsY();
	if ( leftMargin() != oldLeftMargin || d->hasRowSpan )
	    y = 0; // full repaint
	TQRect rect( 0, y, leftHeader->width(), contentsHeight() );
	leftHeader->update( rect );
    }

    if ( updatesEnabled ) {
	int p = rowPos( row );
	if ( d->hasRowSpan )
	    p = contentsY();
	updateContents( contentsX(), p, visibleWidth(), contentsHeight() + 1 );
    }
}

/*!
    Inserts \a count empty columns at column \a col.  Also clears the selection(s).

    \sa insertRows() removeColumn()
*/

void TQTable::insertColumns( int col, int count )
{
    // see comment in insertRows()
    if ( col == -1 && curCol == -1 )
	col = 0;
    if ( col < 0 || count <= 0 )
	return;

    if ( curCol >= col && curCol < col + count )
	curCol = col + count;

    --col;
    if ( col >= numCols() )
	return;

    bool updatesEnabled = isUpdatesEnabled();
    setUpdatesEnabled( false );
    bool topHeaderUpdatesEnabled = topHeader->isUpdatesEnabled();
    topHeader->setUpdatesEnabled( false );
    int oldTopMargin = topMargin();

    setNumCols( numCols() + count );

    for ( int i = numCols() - count - 1; i > col; --i )
	topHeader->swapSections( i, i + count );

    topHeader->setUpdatesEnabled( topHeaderUpdatesEnabled );
    setUpdatesEnabled( updatesEnabled );

    int cr = TQMAX( 0, currentRow() );
    int cc = TQMAX( 0, currentColumn() );
    if ( curCol > col )
	curCol -= count; // this is where curCol was
    setCurrentCell( cr, cc, true, false ); // without ensureCellVisible

    // Repaint the header
    if ( topHeaderUpdatesEnabled ) {
	int x = columnPos( col ) - contentsX();
	if ( topMargin() != oldTopMargin || d->hasColSpan )
	    x = 0; // full repaint
	TQRect rect( x, 0, contentsWidth(), topHeader->height() );
	topHeader->update( rect );
    }

    if ( updatesEnabled ) {
	int p = columnPos( col );
	if ( d->hasColSpan )
	    p = contentsX();
	updateContents( p, contentsY(), contentsWidth() + 1, visibleHeight() );
    }
}

/*!
    Removes row \a row, and deletes all its cells including any table
    items and widgets the cells may contain. Also clears the selection(s).

    \sa hideRow() insertRows() removeColumn() removeRows()
*/

void TQTable::removeRow( int row )
{
    if ( row < 0 || row >= numRows() )
	return;
    if ( row < numRows() - 1 ) {
	if (d->hiddenRows.find(row))
	    d->hiddenRows.remove(row);

	for ( int i = row; i < numRows() - 1; ++i )
	    ( (TQTableHeader*)verticalHeader() )->swapSections( i, i + 1 );
    }
    setNumRows( numRows() - 1 );
}

/*!
    Removes the rows listed in the array \a rows, and deletes all their
    cells including any table items and widgets the cells may contain.

    The array passed in must only contain valid rows (in the range
    from 0 to numRows() - 1) with no duplicates, and must be sorted in
    ascending order. Also clears the selection(s).

    \sa removeRow() insertRows() removeColumns()
*/

void TQTable::removeRows( const TQMemArray<int> &rows )
{
    if ( rows.count() == 0 )
	return;
    int i;
    for ( i = 0; i < (int)rows.count() - 1; ++i ) {
	for ( int j = rows[i] - i; j < rows[i + 1] - i - 1; j++ ) {
	    ( (TQTableHeader*)verticalHeader() )->swapSections( j, j + i + 1 );
	}
    }

    for ( int j = rows[i] - i; j < numRows() - (int)rows.size(); j++)
	( (TQTableHeader*)verticalHeader() )->swapSections( j, j + rows.count() );

    setNumRows( numRows() - rows.count() );
}

/*!
    Removes column \a col, and deletes all its cells including any
    table items and widgets the cells may contain. Also clears the
    selection(s).

    \sa removeColumns() hideColumn() insertColumns() removeRow()
*/

void TQTable::removeColumn( int col )
{
    if ( col < 0 || col >= numCols() )
	return;
    if ( col < numCols() - 1 ) {
	if (d->hiddenCols.find(col))
	    d->hiddenCols.remove(col);

	for ( int i = col; i < numCols() - 1; ++i )
	    ( (TQTableHeader*)horizontalHeader() )->swapSections( i, i + 1 );
    }
    setNumCols( numCols() - 1 );
}

/*!
    Removes the columns listed in the array \a cols, and deletes all
    their cells including any table items and widgets the cells may
    contain.

    The array passed in must only contain valid columns (in the range
    from 0 to numCols() - 1) with no duplicates, and must be sorted in
    ascending order. Also clears the selection(s).

   \sa removeColumn() insertColumns() removeRows()
*/

void TQTable::removeColumns( const TQMemArray<int> &cols )
{
    if ( cols.count() == 0 )
	return;
    int i;
    for ( i = 0; i < (int)cols.count() - 1; ++i ) {
	for ( int j = cols[i] - i; j < cols[i + 1] - i - 1; j++ ) {
	    ( (TQTableHeader*)horizontalHeader() )->swapSections( j, j + i + 1 );
	}
    }

    for ( int j = cols[i] - i; j < numCols() - (int)cols.size(); j++)
	( (TQTableHeader*)horizontalHeader() )->swapSections( j, j + cols.count() );

    setNumCols( numCols() - cols.count() );
}

/*!
    Starts editing the cell at \a row, \a col.

    If \a replace is true the content of this cell will be replaced by
    the content of the editor when editing is finished, i.e. the user
    will be entering new data; otherwise the current content of the
    cell (if any) will be modified in the editor.

    \sa beginEdit()
*/

void TQTable::editCell( int row, int col, bool replace )
{
    if ( row < 0 || col < 0 || row > numRows() - 1 || col > numCols() - 1 )
	return;

    if ( beginEdit( row, col, replace ) ) {
	edMode = Editing;
	editRow = row;
	editCol = col;
    }
}

#ifndef TQT_NO_DRAGANDDROP

/*!
    This event handler is called whenever a TQTable object receives a
    \l TQDragEnterEvent \a e, i.e. when the user pressed the mouse
    button to drag something.

    The focus is moved to the cell where the TQDragEnterEvent occurred.
*/

void TQTable::contentsDragEnterEvent( TQDragEnterEvent *e )
{
    oldCurrentRow = curRow;
    oldCurrentCol = curCol;
    int tmpRow = rowAt( e->pos().y() );
    int tmpCol = columnAt( e->pos().x() );
    fixRow( tmpRow, e->pos().y() );
    fixCol( tmpCol, e->pos().x() );
    if (e->source() != (TQObject*)cellWidget( currentRow(), currentColumn() ) )
	setCurrentCell( tmpRow, tmpCol, false, true );
    e->accept();
}

/*!
    This event handler is called whenever a TQTable object receives a
    \l TQDragMoveEvent \a e, i.e. when the user actually drags the
    mouse.

    The focus is moved to the cell where the TQDragMoveEvent occurred.
*/

void TQTable::contentsDragMoveEvent( TQDragMoveEvent *e )
{
    int tmpRow = rowAt( e->pos().y() );
    int tmpCol = columnAt( e->pos().x() );
    fixRow( tmpRow, e->pos().y() );
    fixCol( tmpCol, e->pos().x() );
    if (e->source() != (TQObject*)cellWidget( currentRow(), currentColumn() ) )
	setCurrentCell( tmpRow, tmpCol, false, true );
    e->accept();
}

/*!
    This event handler is called when a drag activity leaves \e this
    TQTable object with event \a e.
*/

void TQTable::contentsDragLeaveEvent( TQDragLeaveEvent * )
{
    setCurrentCell( oldCurrentRow, oldCurrentCol, false, true );
}

/*!
    This event handler is called when the user ends a drag and drop by
    dropping something onto \e this TQTable and thus triggers the drop
    event, \a e.
*/

void TQTable::contentsDropEvent( TQDropEvent *e )
{
    setCurrentCell( oldCurrentRow, oldCurrentCol, false, true );
    emit dropped( e );
}

/*!
    If the user presses the mouse on a selected cell, starts moving
    (i.e. dragging), and dragEnabled() is true, this function is
    called to obtain a drag object. A drag using this object begins
    immediately unless dragObject() returns 0.

    By default this function returns 0. You might reimplement it and
    create a TQDragObject depending on the selected items.

    \sa dropped()
*/

TQDragObject *TQTable::dragObject()
{
    return 0;
}

/*!
    Starts a drag.

    Usually you don't need to call or reimplement this function yourself.

    \sa dragObject();
*/

void TQTable::startDrag()
{
    if ( startDragRow == -1 || startDragCol == -1 )
	return;

    startDragRow = startDragCol = -1;

    TQDragObject *drag = dragObject();
    if ( !drag )
	return;

    drag->drag();
}

#endif

/*! \reimp */
void TQTable::windowActivationChange( bool oldActive )
{
    if ( oldActive && autoScrollTimer )
	autoScrollTimer->stop();

    if ( !isVisible() )
	return;

    if ( palette().active() != palette().inactive() )
	updateContents();
}

/*! \reimp */
void TQTable::setEnabled( bool b )
{
    if ( !b ) {
	// editor will lose focus, causing a crash deep in setEnabled(),
	// so we'll end the edit early.
	endEdit( editRow, editCol, true, edMode != Editing );
    }
    TQScrollView::setEnabled(b);
}


/*
    \class TQTableHeader
    \brief The TQTableHeader class allows for creation and manipulation
    of table headers.
\if defined(commercial)
    It is part of the <a href="commercialeditions.html">TQt Enterprise Edition</a>.
\endif

    \ingroup advanced
    \module table


   TQTable uses this subclass of TQHeader for its headers. TQTable has a
   horizontalHeader() for displaying column labels, and a
   verticalHeader() for displaying row labels.

*/

/*
    \enum TQTableHeader::SectionState

    This enum type denotes the state of the header's text

    \value Normal the default
    \value Bold
    \value Selected  typically represented by showing the section "sunken"
    or "pressed in"
*/

/*!
    Creates a new table header called \a name with \a i sections. It
    is a child of widget \a parent and attached to table \a t.
*/

TQTableHeader::TQTableHeader( int i, TQTable *t,
			    TQWidget *parent, const char *name )
    : TQHeader( i, parent, name ), mousePressed(false), startPos(-1),
      table( t ), caching( false ), resizedSection(-1),
      numStretches( 0 )
{
    setIsATableHeader( true );
    d = 0;
    states.resize( i );
    stretchable.resize( i );
    states.fill( Normal, -1 );
    stretchable.fill( false, -1 );
    autoScrollTimer = new TQTimer( this );
    connect( autoScrollTimer, TQ_SIGNAL( timeout() ),
	     this, TQ_SLOT( doAutoScroll() ) );
#ifndef NO_LINE_WIDGET
    line1 = new TQWidget( table->viewport(), "qt_line1" );
    line1->hide();
    line1->setBackgroundMode( PaletteText );
    table->addChild( line1 );
    line2 = new TQWidget( table->viewport(), "qt_line2" );
    line2->hide();
    line2->setBackgroundMode( PaletteText );
    table->addChild( line2 );
#else
    d = new TQTableHeaderPrivate;
    d->oldLinePos = -1; //outside, in contents coords
#endif
    connect( this, TQ_SIGNAL( sizeChange(int,int,int) ),
	     this, TQ_SLOT( sectionWidthChanged(int,int,int) ) );
    connect( this, TQ_SIGNAL( indexChange(int,int,int) ),
	     this, TQ_SLOT( indexChanged(int,int,int) ) );

    stretchTimer = new TQTimer( this );
    widgetStretchTimer = new TQTimer( this );
    connect( stretchTimer, TQ_SIGNAL( timeout() ),
	     this, TQ_SLOT( updateStretches() ) );
    connect( widgetStretchTimer, TQ_SIGNAL( timeout() ),
	     this, TQ_SLOT( updateWidgetStretches() ) );
    startPos = -1;
}

/*!
    Adds a new section, \a size pixels wide (or high for vertical
    headers) with the label \a s. If \a size is negative the section's
    size is calculated based on the width (or height) of the label's
    text.
*/

void TQTableHeader::addLabel( const TQString &s , int size )
{
    TQHeader::addLabel( s, size );
    if ( count() > (int)states.size() ) {
	int s = states.size();
	states.resize( count() );
	stretchable.resize( count() );
	for ( ; s < count(); ++s ) {
	    states[ s ] = Normal;
	    stretchable[ s ] = false;
	}
    }
}

void TQTableHeader::removeLabel( int section )
{
    TQHeader::removeLabel( section );
    if ( section == (int)states.size() - 1 ) {
	states.resize( states.size() - 1 );
	stretchable.resize( stretchable.size() - 1 );
    }
}

void TQTableHeader::resizeArrays( int n )
{
    int old = states.size();
    states.resize( n );
    stretchable.resize( n );
    if ( n > old ) {
	for ( int i = old; i < n; ++i ) {
	    stretchable[ i ] = false;
	    states[ i ] = Normal;
	}
    }
}

void TQTableHeader::setLabel( int section, const TQString & s, int size )
{
    TQHeader::setLabel( section, s, size );
    sectionLabelChanged( section );
}

void TQTableHeader::setLabel( int section, const TQIconSet & iconset,
			     const TQString & s, int size )
{
    TQHeader::setLabel( section, iconset, s, size );
    sectionLabelChanged( section );
}

/*!
    Sets the SectionState of section \a s to \a astate.

    \sa sectionState()
*/

void TQTableHeader::setSectionState( int s, SectionState astate )
{
    if ( s < 0 || s >= (int)states.count() )
	return;
    if ( states.data()[ s ] == astate )
	return;
    if ( isRowSelection( table->selectionMode() ) && orientation() == Horizontal )
	return;

    states.data()[ s ] = astate;
    if ( isUpdatesEnabled() ) {
	if ( orientation() == Horizontal )
	    repaint( sectionPos( s ) - offset(), 0, sectionSize( s ), height(), false );
	else
	    repaint( 0, sectionPos( s ) - offset(), width(), sectionSize( s ), false );
    }
}

void TQTableHeader::setSectionStateToAll( SectionState state )
{
    if ( isRowSelection( table->selectionMode() ) && orientation() == Horizontal )
	return;

    int *d = (int *) states.data();
    int n = count();

    while (n >= 4) {
	d[0] = state;
	d[1] = state;
	d[2] = state;
	d[3] = state;
	d += 4;
	n -= 4;
    }

    if (n > 0) {
	d[0] = state;
	if (n > 1) {
	    d[1] = state;
	    if (n > 2) {
		d[2] = state;
	    }
	}
    }
}

/*!
    Returns the SectionState of section \a s.

    \sa setSectionState()
*/

TQTableHeader::SectionState TQTableHeader::sectionState( int s ) const
{
    return (s < 0 || s >= (int)states.count() ? Normal : (TQTableHeader::SectionState)states[s]);
}

/*! \reimp
*/

void TQTableHeader::paintEvent( TQPaintEvent *e )
{
    TQPainter p( this );
    p.setPen( colorGroup().buttonText() );
    int pos = orientation() == Horizontal
		     ? e->rect().left()
		     : e->rect().top();
    int id = mapToIndex( sectionAt( pos + offset() ) );
    if ( id < 0 ) {
	if ( pos > 0 )
	    return;
	else
	    id = 0;
    }

    TQRegion reg = e->region();
    for ( int i = id; i < count(); i++ ) {
	TQRect r = sRect( i );
	reg -= r;
	p.save();
	if ( !( orientation() == Horizontal && isRowSelection( table->selectionMode() ) ) &&
	     ( sectionState( i ) == Bold || sectionState( i ) == Selected ) ) {
	    TQFont f( font() );
	    f.setBold( true );
	    p.setFont( f );
	}
	paintSection( &p, i, r );
	p.restore();
	if ( ( orientation() == Horizontal && r. right() >= e->rect().right() ) ||
	     ( orientation() == Vertical && r. bottom() >= e->rect().bottom() ) )
	    return;
    }
    if ( !reg.isEmpty() )
	erase( reg );
}

/*!
    \reimp

    Paints the header section with index \a index into the rectangular
    region \a fr on the painter \a p.
*/

void TQTableHeader::paintSection( TQPainter *p, int index, const TQRect& fr )
{
    int section = mapToSection( index );
    if ( section < 0 || cellSize( section ) <= 0 )
	return;

   if ( sectionState( index ) != Selected ||
	 ( orientation() == Horizontal && isRowSelection( table->selectionMode() ) ) ) {
	TQHeader::paintSection( p, index, fr );
   } else {
       TQStyle::SFlags flags = TQStyle::Style_Off | ( orient == Horizontal ? TQStyle::Style_Horizontal : 0 );
       if(isEnabled())
	   flags |= TQStyle::Style_Enabled;
       if(isClickEnabled()) {
	   if(sectionState(index) == Selected) {
	       flags |= TQStyle::Style_Down;
	       if(!mousePressed)
		   flags |= TQStyle::Style_Sunken;
	   }
       }
       if(!(flags & TQStyle::Style_Down))
	   flags |= TQStyle::Style_Raised;
       style().drawPrimitive( TQStyle::PE_HeaderSection, p, TQRect(fr.x(), fr.y(), fr.width(), fr.height()),
			      colorGroup(), flags );
       paintSectionLabel( p, index, fr );
   }
}

static int real_pos( const TQPoint &p, TQt::Orientation o )
{
    if ( o == TQt::Horizontal )
	return p.x();
    return p.y();
}

/*! \reimp
*/

void TQTableHeader::mousePressEvent( TQMouseEvent *e )
{
    if ( e->button() != LeftButton )
	return;
    TQHeader::mousePressEvent( e );
    mousePressed = true;
    pressPos = real_pos( e->pos(), orientation() );
    if ( !table->currentSel || ( e->state() & ShiftButton ) != ShiftButton )
	startPos = -1;
    setCaching( true );
    resizedSection = -1;
#ifdef TQT_NO_CURSOR
    isResizing = false;
#else
    isResizing = cursor().shape() != ArrowCursor;
    if ( !isResizing && sectionAt( pressPos ) != -1 )
	doSelection( e );
#endif
}

/*! \reimp
*/

void TQTableHeader::mouseMoveEvent( TQMouseEvent *e )
{
    if ( (e->state() & MouseButtonMask) != LeftButton // Using LeftButton simulates old behavior.
#ifndef TQT_NO_CURSOR
         || cursor().shape() != ArrowCursor
#endif
         || ( ( e->state() & ControlButton ) == ControlButton &&
              ( orientation() == Horizontal
	     ? table->columnMovingEnabled() : table->rowMovingEnabled() ) ) ) {
	TQHeader::mouseMoveEvent( e );
	return;
    }

    if ( !doSelection( e ) )
	TQHeader::mouseMoveEvent( e );
}

bool TQTableHeader::doSelection( TQMouseEvent *e )
{
    int p = real_pos( e->pos(), orientation() ) + offset();

    if ( isRowSelection( table->selectionMode() ) ) {
	if ( orientation() == Horizontal )
	    return true;
	if ( table->selectionMode() == TQTable::SingleRow ) {
	    int secAt = sectionAt( p );
	    if ( secAt == -1 )
		return true;
	    table->setCurrentCell( secAt, table->currentColumn() );
	    return true;
	}
    }

    if ( startPos == -1 ) {
 	int secAt = sectionAt( p );
	if ( ( ( e->state() & ControlButton ) != ControlButton &&
	     ( e->state() & ShiftButton ) != ShiftButton ) ||
	     table->selectionMode() == TQTable::Single ||
	     table->selectionMode() == TQTable::SingleRow ) {
	    startPos = p;
	    bool b = table->signalsBlocked();
	    table->blockSignals( true );
	    table->clearSelection();
	    table->blockSignals( b );
	}
	saveStates();

	if ( table->selectionMode() != TQTable::NoSelection ) {
	    startPos = p;
	    TQTableSelection *oldSelection = table->currentSel;

	    if ( orientation() == Vertical ) {
		if ( !table->isRowSelected( secAt, true ) ) {
		    table->currentSel = new TQTableSelection();
		    table->selections.append( table->currentSel );
		    table->currentSel->init( secAt, 0 );
		    table->currentSel->expandTo( secAt, table->numCols() - 1 );
                    emit table->selectionChanged();
		}
		table->setCurrentCell( secAt, 0 );
	    } else { // orientation == Horizontal
		if ( !table->isColumnSelected( secAt, true ) ) {
		    table->currentSel = new TQTableSelection();
		    table->selections.append( table->currentSel );
		    table->currentSel->init( 0, secAt );
		    table->currentSel->expandTo( table->numRows() - 1, secAt );
                    emit table->selectionChanged();
		}
		table->setCurrentCell( 0, secAt );
	    }

            if ( ( orientation() == Horizontal && table->isColumnSelected(secAt) ) ||
                 ( orientation() == Vertical && table->isRowSelected(secAt) ) ) {
                setSectionState( secAt, Selected );
            }

 	    table->repaintSelections( oldSelection, table->currentSel,
 				      orientation() == Horizontal,
 				      orientation() == Vertical );
	    if ( sectionAt( p ) != -1 )
 		endPos = p;

 	    return true;
	}
    }

    if ( sectionAt( p ) != -1 )
	endPos = p;
    if ( startPos != -1 ) {
	updateSelections();
	p -= offset();
	if ( orientation() == Horizontal && ( p < 0 || p > width() ) ) {
	    doAutoScroll();
	    autoScrollTimer->start( 100, true );
	} else if ( orientation() == Vertical && ( p < 0 || p > height() ) ) {
	    doAutoScroll();
	    autoScrollTimer->start( 100, true );
	}
	return true;
    }
    return table->selectionMode() == TQTable::NoSelection;
}

static inline bool mayOverwriteMargin( int before, int after )
{
    /*
      0 is the only user value that we always respect. We also never
      shrink a margin, in case the user wanted it that way.
    */
    return before != 0 && before < after;
}

void TQTableHeader::sectionLabelChanged( int section )
{
    emit sectionSizeChanged( section );

    // this does not really belong here
    if ( orientation() == Horizontal ) {
	int h = sizeHint().height();
	if ( h != height() && mayOverwriteMargin(table->topMargin(), h) )
	    table->setTopMargin( h );
    } else {
	int w = sizeHint().width();
	if ( w != width() && mayOverwriteMargin( ( TQApplication::reverseLayout() ? table->rightMargin() : table->leftMargin() ), w) )
	    table->setLeftMargin( w );
    }
}

/*! \reimp */
void TQTableHeader::mouseReleaseEvent( TQMouseEvent *e )
{
    if ( e->button() != LeftButton )
	return;
    autoScrollTimer->stop();
    mousePressed = false;
    setCaching( false );
    TQHeader::mouseReleaseEvent( e );
#ifndef NO_LINE_WIDGET
    line1->hide();
    line2->hide();
#else
    if ( d->oldLinePos >= 0 )
	if ( orientation() == Horizontal )
	    table->updateContents( d->oldLinePos, table->contentsY(),
				   1, table->visibleHeight() );
	else
	    table->updateContents(  table->contentsX(), d->oldLinePos,
				    table->visibleWidth(), 1 );
    d->oldLinePos = -1;
#endif
    if ( resizedSection != -1 ) {
	emit sectionSizeChanged( resizedSection );
	updateStretches();
    }

    //Make sure all newly selected sections are painted one last time
    TQRect selectedRects;
    for ( int i = 0; i < count(); i++ ) {
	if(sectionState( i ) == Selected)
	    selectedRects |= sRect( i );
    }
    if(!selectedRects.isNull())
	repaint(selectedRects);
}

/*! \reimp
*/

void TQTableHeader::mouseDoubleClickEvent( TQMouseEvent *e )
{
    if ( e->button() != LeftButton )
	return;
    if ( isResizing ) {
	int p = real_pos( e->pos(), orientation() ) + offset();
	int section = sectionAt( p );
	if ( section == -1 )
	    return;
	section--;
	if ( p >= sectionPos( count() - 1 ) + sectionSize( count() - 1 ) )
	    ++section;
	while ( sectionSize( section ) == 0 )
	    section--;
	if ( section < 0 )
	    return;
	int oldSize = sectionSize( section );
	if ( orientation() == Horizontal ) {
	    table->adjustColumn( section );
	    int newSize = sectionSize( section );
	    if ( oldSize != newSize )
		emit sizeChange( section, oldSize, newSize );
	    for ( int i = 0; i < table->numCols(); ++i ) {
		if ( table->isColumnSelected( i ) && sectionSize( i ) != 0 )
		    table->adjustColumn( i );
	    }
	} else {
	    table->adjustRow( section );
	    int newSize = sectionSize( section );
	    if ( oldSize != newSize )
		emit sizeChange( section, oldSize, newSize );
	    for ( int i = 0; i < table->numRows(); ++i ) {
		if ( table->isRowSelected( i )  && sectionSize( i ) != 0 )
		    table->adjustRow( i );
	    }
	}
    }
}

/*! \reimp
*/

void TQTableHeader::resizeEvent( TQResizeEvent *e )
{
    stretchTimer->stop();
    widgetStretchTimer->stop();
    TQHeader::resizeEvent( e );
    if ( numStretches == 0 )
	return;
    stretchTimer->start( 0, true );
}

void TQTableHeader::updateStretches()
{
    if ( numStretches == 0 )
	return;

    int dim = orientation() == Horizontal ? width() : height();
    if ( sectionPos(count() - 1) + sectionSize(count() - 1) == dim )
	return;
    int i;
    int pd = dim - ( sectionPos(count() - 1)
		     + sectionSize(count() - 1) );
    bool block = signalsBlocked();
    blockSignals( true );
    for ( i = 0; i < (int)stretchable.count(); ++i ) {
	if ( !stretchable[i] ||
	     ( stretchable[i] && table->d->hiddenCols[i] ) )
	    continue;
	pd += sectionSize( i );
    }
    pd /= numStretches;
    for ( i = 0; i < (int)stretchable.count(); ++i ) {
	if ( !stretchable[i] ||
	     ( stretchable[i] && table->d->hiddenCols[i] ) )
	    continue;
	if ( i == (int)stretchable.count() - 1 &&
	     sectionPos( i ) + pd < dim )
	    pd = dim - sectionPos( i );
	resizeSection( i, TQMAX( 20, pd ) );
    }
    blockSignals( block );
    table->repaintContents( false );
    widgetStretchTimer->start( 100, true );
}

void TQTableHeader::updateWidgetStretches()
{
    TQSize s = table->tableSize();
    table->resizeContents( s.width(), s.height() );
    for ( int i = 0; i < table->numCols(); ++i )
	table->updateColWidgets( i );
}

void TQTableHeader::updateSelections()
{
    if ( table->selectionMode() == TQTable::NoSelection ||
	 (isRowSelection( table->selectionMode() ) && orientation() != Vertical  ) )
	return;
    int a = sectionAt( startPos );
    int b = sectionAt( endPos );
    int start = TQMIN( a, b );
    int end = TQMAX( a, b );
    int *s = states.data();
    for ( int i = 0; i < count(); ++i ) {
	if ( i < start || i > end )
	    *s = oldStates.data()[ i ];
	else
	    *s = Selected;
	++s;
    }
    repaint( false );

    if (table->currentSel) {
	TQTableSelection oldSelection = *table->currentSel;
	if ( orientation() == Vertical )
	    table->currentSel->expandTo( b, table->horizontalHeader()->count() - 1 );
	else
	    table->currentSel->expandTo( table->verticalHeader()->count() - 1, b );
	table->repaintSelections( &oldSelection, table->currentSel,
				  orientation() == Horizontal,
				  orientation() == Vertical );
    }
    emit table->selectionChanged();
}

void TQTableHeader::saveStates()
{
    oldStates.resize( count() );
    int *s = states.data();
    int *s2 = oldStates.data();
    for ( int i = 0; i < count(); ++i ) {
	*s2 = *s;
	++s2;
	++s;
    }
}

void TQTableHeader::doAutoScroll()
{
    TQPoint pos = mapFromGlobal( TQCursor::pos() );
    int p = real_pos( pos, orientation() ) + offset();
    if ( sectionAt( p ) != -1 )
	endPos = p;
    if ( orientation() == Horizontal )
	table->ensureVisible( endPos, table->contentsY() );
    else
	table->ensureVisible( table->contentsX(), endPos );
    updateSelections();
    autoScrollTimer->start( 100, true );
}

void TQTableHeader::sectionWidthChanged( int col, int, int )
{
    resizedSection = col;
    if ( orientation() == Horizontal ) {
#ifndef NO_LINE_WIDGET
	table->moveChild( line1, TQHeader::sectionPos( col ) - 1,
			  table->contentsY() );
	line1->resize( 1, table->visibleHeight() );
	line1->show();
	line1->raise();
	table->moveChild( line2,
			  TQHeader::sectionPos( col ) + TQHeader::sectionSize( col ) - 1,
			  table->contentsY() );
	line2->resize( 1, table->visibleHeight() );
	line2->show();
	line2->raise();
#else
	TQPainter p( table->viewport() );
	int lx = TQHeader::sectionPos( col ) + TQHeader::sectionSize( col ) - 1;
	int ly = table->contentsY();

	if ( lx != d->oldLinePos ) {
	    TQPoint pt = table->contentsToViewport( TQPoint( lx, ly ) );
	    p.drawLine( pt.x(), pt.y()+1,
			pt.x(), pt.y()+ table->visibleHeight() );
	    if ( d->oldLinePos >= 0 )
		table->repaintContents( d->oldLinePos, table->contentsY(),
				       1, table->visibleHeight() );

	    d->oldLinePos = lx;
	}
#endif
    } else {
#ifndef NO_LINE_WIDGET
	table->moveChild( line1, table->contentsX(),
			  TQHeader::sectionPos( col ) - 1 );
	line1->resize( table->visibleWidth(), 1 );
	line1->show();
	line1->raise();
	table->moveChild( line2, table->contentsX(),
			  TQHeader::sectionPos( col ) + TQHeader::sectionSize( col ) - 1 );
	line2->resize( table->visibleWidth(), 1 );
	line2->show();
	line2->raise();

#else
	TQPainter p( table->viewport() );
	int lx = table->contentsX();
	int ly = TQHeader::sectionPos( col ) + TQHeader::sectionSize( col ) - 1;

	if ( ly != d->oldLinePos ) {
	    TQPoint pt = table->contentsToViewport( TQPoint( lx, ly ) );
	    p.drawLine( pt.x()+1, pt.y(),
			pt.x() + table->visibleWidth(), pt.y() );
	    if ( d->oldLinePos >= 0 )
		table->repaintContents(  table->contentsX(), d->oldLinePos,
					table->visibleWidth(), 1 );
	    d->oldLinePos = ly;
	}

#endif
    }
}

/*!
    \reimp

    Returns the size of section \a section in pixels or -1 if \a
    section is out of range.
*/

int TQTableHeader::sectionSize( int section ) const
{
    if ( count() <= 0 || section < 0 || section >= count() )
	return -1;
    if ( caching && section < (int)sectionSizes.count() )
	 return sectionSizes[ section ];
    return TQHeader::sectionSize( section );
}

/*!
    \reimp

    Returns the start position of section \a section in pixels or -1
    if \a section is out of range.

    \sa sectionAt()
*/

int TQTableHeader::sectionPos( int section ) const
{
    if ( count() <= 0 || section < 0 || section >= count() )
	return -1;
    if ( caching && section < (int)sectionPoses.count() )
	return sectionPoses[ section ];
    return TQHeader::sectionPos( section );
}

/*!
    \reimp

    Returns the number of the section at index position \a pos or -1
    if there is no section at the position given.

    \sa sectionPos()
*/

int TQTableHeader::sectionAt( int pos ) const
{
    if ( !caching || sectionSizes.count() <= 0 || sectionPoses.count() <= 0 )
	return TQHeader::sectionAt( pos );
    if ( count() <= 0 || pos > sectionPoses[ count() - 1 ] + sectionSizes[ count() - 1 ] )
	return -1;
    int l = 0;
    int r = count() - 1;
    int i = ( (l+r+1) / 2 );
    while ( r - l ) {
	if ( sectionPoses[i] > pos )
	    r = i -1;
	else
	    l = i;
	i = ( (l+r+1) / 2 );
    }
    if ( sectionPoses[i] <= pos &&
	 pos <= sectionPoses[i] + sectionSizes[ mapToSection( i ) ] )
	return mapToSection( i );
    return -1;
}

void TQTableHeader::updateCache()
{
    sectionPoses.resize( count() );
    sectionSizes.resize( count() );
    if ( !caching )
	return;
    for ( int i = 0; i < count(); ++i ) {
	sectionSizes[ i ] = TQHeader::sectionSize( i );
	sectionPoses[ i ] = TQHeader::sectionPos( i );
    }
}

void TQTableHeader::setCaching( bool b )
{
    if ( caching == b )
	return;
    caching = b;
    sectionPoses.resize( count() );
    sectionSizes.resize( count() );
    if ( b ) {
	for ( int i = 0; i < count(); ++i ) {
	    sectionSizes[ i ] = TQHeader::sectionSize( i );
	    sectionPoses[ i ] = TQHeader::sectionPos( i );
	}
    }
}

/*!
    If \a b is true, section \a s is stretchable; otherwise the
    section is not stretchable.

    \sa isSectionStretchable()
*/

void TQTableHeader::setSectionStretchable( int s, bool b )
{
    if ( stretchable[ s ] == b )
	return;
    stretchable[ s ] = b;
    if ( b )
	numStretches++;
    else
	numStretches--;
}

/*!
    Returns true if section \a s is stretcheable; otherwise returns
    false.

    \sa setSectionStretchable()
*/

bool TQTableHeader::isSectionStretchable( int s ) const
{
    return stretchable[ s ];
}

void TQTableHeader::swapSections( int oldIdx, int newIdx, bool swapTable )
{
    extern bool tqt_qheader_label_return_null_strings; // qheader.cpp
    tqt_qheader_label_return_null_strings = true;

    TQIconSet oldIconSet, newIconSet;
    if ( iconSet( oldIdx ) )
	oldIconSet = *iconSet( oldIdx );
    if ( iconSet( newIdx ) )
	newIconSet = *iconSet( newIdx );
    TQString oldLabel = label( oldIdx );
    TQString newLabel = label( newIdx );
    bool sectionsHasContent = !(oldIconSet.isNull() && newIconSet.isNull()
                            && oldLabel.isNull() && newLabel.isNull());
    if (sectionsHasContent) {
        setLabel( oldIdx, newIconSet, newLabel );
        setLabel( newIdx, oldIconSet, oldLabel );
    }

    tqt_qheader_label_return_null_strings = false;

    int w1 = sectionSize( oldIdx );
    int w2 = sectionSize( newIdx );
    if ( w1 != w2 ) {
        resizeSection( oldIdx, w2 );
        resizeSection( newIdx, w1 );
    }

    if ( !swapTable )
        return;
    if ( orientation() == Horizontal )
        table->swapColumns( oldIdx, newIdx );
    else
        table->swapRows( oldIdx, newIdx );
}

void TQTableHeader::indexChanged( int sec, int oldIdx, int newIdx )
{
    newIdx = mapToIndex( sec );
    if ( oldIdx > newIdx )
	moveSection( sec, oldIdx + 1 );
    else
	moveSection( sec, oldIdx );

    if ( oldIdx < newIdx ) {
	while ( oldIdx < newIdx ) {
	    swapSections( oldIdx, oldIdx + 1 );
	    oldIdx++;
	}
    } else {
	while ( oldIdx > newIdx ) {
	    swapSections( oldIdx - 1, oldIdx );
	    oldIdx--;
	}
    }

    table->repaintContents( table->contentsX(), table->contentsY(),
			    table->visibleWidth(), table->visibleHeight() );
}

void TQTableHeader::setLabels(const TQStringList & labels)
{
    int i = 0;
    bool updates = isUpdatesEnabled();
    const int c = TQMIN(count(), (int)labels.count());
    setUpdatesEnabled(false);
    for ( TQStringList::ConstIterator it = labels.begin(); i < c; ++i, ++it ) {
	if (i == c - 1) {
	    setUpdatesEnabled(updates);
	    setLabel( i, *it );
	} else {
	    TQHeader::setLabel( i, *it );
	    emit sectionSizeChanged( i );
	}
    }
}

#include "qtable.moc"

#endif // TQT_NO_TABLE
