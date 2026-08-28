/****************************************************************************
**
** Definition of TQTable widget class
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

#ifndef TQTABLE_H
#define TQTABLE_H

#ifndef QT_H
#include "ntqscrollview.h"
#include "ntqpixmap.h"
#include "ntqptrvector.h"
#include "ntqheader.h"
#include "ntqmemarray.h"
#include "ntqptrlist.h"
#include "ntqguardedptr.h"
#include "ntqshared.h"
#include "ntqintdict.h"
#include "ntqstringlist.h"
#endif // QT_H


#ifndef TQT_NO_TABLE

#if !defined( TQT_MODULE_TABLE ) || defined( QT_LICENSE_PROFESSIONAL ) || defined( QT_INTERNAL_TABLE )
#define TQM_EXPORT_TABLE
#ifndef TQM_TEMPLATE_EXTERN_TABLE
#   define TQM_TEMPLATE_EXTERN_TABLE
#endif
#else
#define TQM_EXPORT_TABLE TQ_EXPORT
#define TQM_TEMPLATE_EXTERN_TABLE Q_TEMPLATE_EXTERN
#endif

class TQTableHeader;
class TQValidator;
class TQTable;
class TQPaintEvent;
class TQTimer;
class TQResizeEvent;
class TQComboBox;
class TQCheckBox;
class TQDragObject;

struct TQTablePrivate;
struct TQTableHeaderPrivate;


class TQM_EXPORT_TABLE TQTableSelection
{
public:
    TQTableSelection();
    TQTableSelection( int start_row, int start_col, int end_row, int end_col );
    void init( int row, int col );
    void expandTo( int row, int col );
    bool operator==( const TQTableSelection &s ) const;
    bool operator!=( const TQTableSelection &s ) const { return !(operator==(s)); }

    int topRow() const { return tRow; }
    int bottomRow() const { return bRow; }
    int leftCol() const { return lCol; }
    int rightCol() const { return rCol; }
    int anchorRow() const { return aRow; }
    int anchorCol() const { return aCol; }
    int numRows() const;
    int numCols() const;

    bool isActive() const { return active; }
    bool isEmpty() const { return numRows() == 0; }

private:
    uint active : 1;
    uint inited : 1;
    int tRow, lCol, bRow, rCol;
    int aRow, aCol;
};

#define Q_DEFINED_QTABLE_SELECTION
#include "ntqwinexport.h"

class TQM_EXPORT_TABLE TQTableItem : public TQt
{
    friend class TQTable;

public:
    enum EditType { Never, OnTyping, WhenCurrent, Always };

    TQTableItem( TQTable *table, EditType et );
    TQTableItem( TQTable *table, EditType et, const TQString &text );
    TQTableItem( TQTable *table, EditType et, const TQString &text,
                const TQPixmap &p );
    virtual ~TQTableItem();

    virtual TQPixmap pixmap() const;
    virtual TQString text() const;
    virtual void setPixmap( const TQPixmap &p );
    virtual void setText( const TQString &t );
    TQTable *table() const { return t; }

    virtual int alignment() const;
    virtual void setWordWrap( bool b );
    bool wordWrap() const;

    EditType editType() const;
    virtual TQWidget *createEditor() const;
    virtual void setContentFromEditor( TQWidget *w );
    virtual void setReplaceable( bool );
    bool isReplaceable() const;

    virtual TQString key() const;
    virtual TQSize sizeHint() const;

    virtual void setSpan( int rs, int cs );
    int rowSpan() const;
    int colSpan() const;

    virtual void setRow( int r );
    virtual void setCol( int c );
    int row() const;
    int col() const;

    virtual void paint( TQPainter *p, const TQColorGroup &cg,
                        const TQRect &cr, bool selected );

    void updateEditor( int oldRow, int oldCol );

    virtual void setEnabled( bool b );
    bool isEnabled() const;

    virtual int rtti() const;
    static int RTTI;

private:
    TQString txt;
    TQPixmap pix;
    TQTable *t;
    EditType edType;
    uint wordwrap : 1;
    uint tcha : 1;
    uint enabled : 1;
    int rw, cl;
    int rowspan, colspan;
#if (TQT_VERSION >= 0x040000)
#error "Add a setAlignment() function in 4.0 (but no d pointer)"
#endif
};

#define Q_DEFINED_QTABLE_ITEM
#include "ntqwinexport.h"

class TQM_EXPORT_TABLE TQComboTableItem : public TQTableItem
{
public:
    TQComboTableItem( TQTable *table, const TQStringList &list, bool editable = false );
    ~TQComboTableItem();
    virtual TQWidget *createEditor() const;
    virtual void setContentFromEditor( TQWidget *w );
    virtual void paint( TQPainter *p, const TQColorGroup &cg,
                        const TQRect &cr, bool selected );
    virtual void setCurrentItem( int i );
    virtual void setCurrentItem( const TQString &i );
    int currentItem() const;
    TQString currentText() const;
    int count() const;
#if !defined(Q_NO_USING_KEYWORD)
    using TQTableItem::text;
#endif
    TQString text( int i ) const;
    virtual void setEditable( bool b );
    bool isEditable() const;
    virtual void setStringList( const TQStringList &l );

    int rtti() const;
    static int RTTI;

    TQSize sizeHint() const;

private:
    TQComboBox *cb;
    TQStringList entries;
    int current;
    bool edit;
    static TQComboBox *fakeCombo;
    static TQWidget *fakeComboWidget;
    static int fakeRef;

};

class TQM_EXPORT_TABLE TQCheckTableItem : public TQTableItem
{
public:
    TQCheckTableItem( TQTable *table, const TQString &txt );
    void setText( const TQString &t );
    virtual TQWidget *createEditor() const;
    virtual void setContentFromEditor( TQWidget *w );
    virtual void paint( TQPainter *p, const TQColorGroup &cg,
                        const TQRect &cr, bool selected );
    virtual void setChecked( bool b );
    bool isChecked() const;

    int rtti() const;
    static int RTTI;

    TQSize sizeHint() const;

private:
    TQCheckBox *cb;
    bool checked;

};

class TQM_EXPORT_TABLE TQTable : public TQScrollView
{
    TQ_OBJECT
    TQ_ENUMS( SelectionMode FocusStyle )
    TQ_PROPERTY( int numRows READ numRows WRITE setNumRows )
    TQ_PROPERTY( int numCols READ numCols WRITE setNumCols )
    TQ_PROPERTY( bool showGrid READ showGrid WRITE setShowGrid )
    TQ_PROPERTY( bool rowMovingEnabled READ rowMovingEnabled WRITE setRowMovingEnabled )
    TQ_PROPERTY( bool columnMovingEnabled READ columnMovingEnabled WRITE setColumnMovingEnabled )
    TQ_PROPERTY( bool readOnly READ isReadOnly WRITE setReadOnly )
    TQ_PROPERTY( bool sorting READ sorting WRITE setSorting )
    TQ_PROPERTY( SelectionMode selectionMode READ selectionMode WRITE setSelectionMode )
    TQ_PROPERTY( FocusStyle focusStyle READ focusStyle WRITE setFocusStyle )
    TQ_PROPERTY( int numSelections READ numSelections )

    friend class TQTableHeader;
    friend class TQComboTableItem;
    friend class TQCheckTableItem;
    friend class TQTableItem;

public:
    TQTable( TQWidget* parent=0, const char* name=0 );
    TQTable( int numRows, int numCols,
            TQWidget* parent=0, const char* name=0 );
    ~TQTable();

    TQHeader *horizontalHeader() const;
    TQHeader *verticalHeader() const;

    enum SelectionMode { Single, Multi, SingleRow, MultiRow, NoSelection };
    virtual void setSelectionMode( SelectionMode mode );
    SelectionMode selectionMode() const;

    virtual void setItem( int row, int col, TQTableItem *item );
    virtual void setText( int row, int col, const TQString &text );
    virtual void setPixmap( int row, int col, const TQPixmap &pix );
    virtual TQTableItem *item( int row, int col ) const;
    virtual TQString text( int row, int col ) const;
    virtual TQPixmap pixmap( int row, int col ) const;
    virtual void clearCell( int row, int col );

    virtual TQRect cellGeometry( int row, int col ) const;
    virtual int columnWidth( int col ) const;
    virtual int rowHeight( int row ) const;
    virtual int columnPos( int col ) const;
    virtual int rowPos( int row ) const;
    virtual int columnAt( int x ) const;
    virtual int rowAt( int y ) const;

    virtual int numRows() const;
    virtual int numCols() const;

    void updateCell( int row, int col );

    bool eventFilter( TQObject * o, TQEvent * );

    int currentRow() const { return curRow; }
    int currentColumn() const { return curCol; }
    void ensureCellVisible( int row, int col );

    bool isSelected( int row, int col ) const;
    bool isRowSelected( int row, bool full = false ) const;
    bool isColumnSelected( int col, bool full = false ) const;
    int numSelections() const;
    TQTableSelection selection( int num ) const;
    virtual int addSelection( const TQTableSelection &s );
    virtual void removeSelection( const TQTableSelection &s );
    virtual void removeSelection( int num );
    virtual int currentSelection() const;

    void selectCells( int start_row, int start_col, int end_row, int end_col );
    void selectRow( int row );
    void selectColumn( int col );

    bool showGrid() const;

    bool columnMovingEnabled() const;
    bool rowMovingEnabled() const;

    virtual void sortColumn( int col, bool ascending = true,
                             bool wholeRows = false );
    bool sorting() const;

    virtual void takeItem( TQTableItem *i );

    virtual void setCellWidget( int row, int col, TQWidget *e );
    virtual TQWidget *cellWidget( int row, int col ) const;
    virtual void clearCellWidget( int row, int col );

    virtual TQRect cellRect( int row, int col ) const;

    virtual void paintCell( TQPainter *p, int row, int col,
                            const TQRect &cr, bool selected );
    virtual void paintCell( TQPainter *p, int row, int col,
                            const TQRect &cr, bool selected, const TQColorGroup &cg );
    virtual void paintFocus( TQPainter *p, const TQRect &r );
    TQSize sizeHint() const;

    bool isReadOnly() const;
    bool isRowReadOnly( int row ) const;
    bool isColumnReadOnly( int col ) const;

    void setEnabled( bool b );

    void repaintSelections();

    enum FocusStyle { FollowStyle, SpreadSheet };
    virtual void setFocusStyle( FocusStyle fs );
    FocusStyle focusStyle() const;

    void updateHeaderStates();

public slots:
    virtual void setNumRows( int r );
    virtual void setNumCols( int r );
    virtual void setShowGrid( bool b );
    virtual void hideRow( int row );
    virtual void hideColumn( int col );
    virtual void showRow( int row );
    virtual void showColumn( int col );
    bool isRowHidden( int row ) const;
    bool isColumnHidden( int col ) const;

    virtual void setColumnWidth( int col, int w );
    virtual void setRowHeight( int row, int h );

    virtual void adjustColumn( int col );
    virtual void adjustRow( int row );

    virtual void setColumnStretchable( int col, bool stretch );
    virtual void setRowStretchable( int row, bool stretch );
    bool isColumnStretchable( int col ) const;
    bool isRowStretchable( int row ) const;
    virtual void setSorting( bool b );
    virtual void swapRows( int row1, int row2, bool swapHeader = false );
    virtual void swapColumns( int col1, int col2, bool swapHeader = false );
    virtual void swapCells( int row1, int col1, int row2, int col2 );

    virtual void setLeftMargin( int m );
    virtual void setTopMargin( int m );
    virtual void setCurrentCell( int row, int col );
    void clearSelection( bool repaint = true );
    virtual void setColumnMovingEnabled( bool b );
    virtual void setRowMovingEnabled( bool b );

    virtual void setReadOnly( bool b );
    virtual void setRowReadOnly( int row, bool ro );
    virtual void setColumnReadOnly( int col, bool ro );

    virtual void setDragEnabled( bool b );
    bool dragEnabled() const;

    virtual void insertRows( int row, int count = 1 );
    virtual void insertColumns( int col, int count = 1 );
    virtual void removeRow( int row );
    virtual void removeRows( const TQMemArray<int> &rows );
    virtual void removeColumn( int col );
    virtual void removeColumns( const TQMemArray<int> &cols );

    virtual void editCell( int row, int col, bool replace = false );

    void setRowLabels( const TQStringList &labels );
    void setColumnLabels( const TQStringList &labels );

protected:
    enum EditMode { NotEditing, Editing, Replacing };
    void drawContents( TQPainter *p, int cx, int cy, int cw, int ch );
    void contentsMousePressEvent( TQMouseEvent* );
    void contentsMouseMoveEvent( TQMouseEvent* );
    void contentsMouseDoubleClickEvent( TQMouseEvent* );
    void contentsMouseReleaseEvent( TQMouseEvent* );
    void contentsContextMenuEvent( TQContextMenuEvent * e );
    void keyPressEvent( TQKeyEvent* );
    void focusInEvent( TQFocusEvent* );
    void focusOutEvent( TQFocusEvent* );
    void viewportResizeEvent( TQResizeEvent * );
    void showEvent( TQShowEvent *e );
    void paintEvent( TQPaintEvent *e );
    void setEditMode( EditMode mode, int row, int col );
#ifndef TQT_NO_DRAGANDDROP
    virtual void contentsDragEnterEvent( TQDragEnterEvent *e );
    virtual void contentsDragMoveEvent( TQDragMoveEvent *e );
    virtual void contentsDragLeaveEvent( TQDragLeaveEvent *e );
    virtual void contentsDropEvent( TQDropEvent *e );
    virtual TQDragObject *dragObject();
    virtual void startDrag();
#endif

    virtual void paintEmptyArea( TQPainter *p, int cx, int cy, int cw, int ch );
    virtual void activateNextCell();
    virtual TQWidget *createEditor( int row, int col, bool initFromCell ) const;
    virtual void setCellContentFromEditor( int row, int col );
    virtual TQWidget *beginEdit( int row, int col, bool replace );
    virtual void endEdit( int row, int col, bool accept, bool replace );

    virtual void resizeData( int len );
    virtual void insertWidget( int row, int col, TQWidget *w );
    int indexOf( int row, int col ) const;

    void windowActivationChange( bool );
    bool isEditing() const;
    EditMode editMode() const;
    int currEditRow() const;
    int currEditCol() const;

protected slots:
    virtual void columnWidthChanged( int col );
    virtual void rowHeightChanged( int row );
    virtual void columnIndexChanged( int section, int fromIndex, int toIndex );
    virtual void rowIndexChanged( int section, int fromIndex, int toIndex );
    virtual void columnClicked( int col );

signals:
    void currentChanged( int row, int col );
    void clicked( int row, int col, int button, const TQPoint &mousePos );
    void doubleClicked( int row, int col, int button, const TQPoint &mousePos );
    void pressed( int row, int col, int button, const TQPoint &mousePos );
    void selectionChanged();
    void valueChanged( int row, int col );
    void contextMenuRequested( int row, int col, const TQPoint &pos );
#ifndef TQT_NO_DRAGANDDROP
    void dropped( TQDropEvent *e );
#endif

private slots:
    void doAutoScroll();
    void doValueChanged();
    void updateGeometriesSlot();

private:
    void contentsMousePressEventEx( TQMouseEvent* );
    void drawContents( TQPainter* );
    void updateGeometries();
    void repaintSelections( TQTableSelection *oldSelection,
                            TQTableSelection *newSelection,
                            bool updateVertical = true,
                            bool updateHorizontal = true );
    TQRect rangeGeometry( int topRow, int leftCol,
                         int bottomRow, int rightCol, bool &optimize );
    void fixRow( int &row, int y );
    void fixCol( int &col, int x );

    void init( int numRows, int numCols );
    TQSize tableSize() const;
    void repaintCell( int row, int col );
    void contentsToViewport2( int x, int y, int& vx, int& vy );
    TQPoint contentsToViewport2( const TQPoint &p );
    void viewportToContents2( int vx, int vy, int& x, int& y );
    TQPoint viewportToContents2( const TQPoint &p );

    void updateRowWidgets( int row );
    void updateColWidgets( int col );
    bool isSelected( int row, int col, bool includeCurrent ) const;
    void setCurrentCell( int row, int col, bool updateSelections, bool ensureVisible = false );
    void fixCell( int &row, int &col, int key );
    void delayedUpdateGeometries();
    struct TableWidget
    {
	TableWidget( TQWidget *w, int r, int c ) : wid( w ), row( r ), col ( c ) {}
	TQWidget *wid;
	int row, col;
    };
    void saveContents( TQPtrVector<TQTableItem> &tmp,
		       TQPtrVector<TableWidget> &tmp2 );
    void updateHeaderAndResizeContents( TQTableHeader *header,
					int num, int colRow,
					int width, bool &updateBefore );
    void restoreContents( TQPtrVector<TQTableItem> &tmp,
			  TQPtrVector<TableWidget> &tmp2 );
    void finishContentsResze( bool updateBefore );

private:
    TQPtrVector<TQTableItem> contents;
    TQPtrVector<TQWidget> widgets;
    int curRow;
    int curCol;
    TQTableHeader *leftHeader, *topHeader;
    EditMode edMode;
    int editCol, editRow;
    TQPtrList<TQTableSelection> selections;
    TQTableSelection *currentSel;
    TQTimer *autoScrollTimer;
    int lastSortCol;
    bool sGrid : 1;
    bool mRows : 1;
    bool mCols : 1;
    bool asc : 1;
    bool doSort : 1;
    bool unused : 1;
    bool readOnly : 1;
    bool shouldClearSelection : 1;
    bool dEnabled : 1;
    bool context_menu : 1;
    bool drawActiveSelection : 1;
    bool was_visible : 1;
    SelectionMode selMode;
    int pressedRow, pressedCol;
    TQTablePrivate *d;
    TQIntDict<int> roRows;
    TQIntDict<int> roCols;
    int startDragRow;
    int startDragCol;
    TQPoint dragStartPos;
    int oldCurrentRow, oldCurrentCol;
    TQWidget *unused_topLeftCorner; //### remove in 4.0
    FocusStyle focusStl;
    TQSize unused_cachedSizeHint; // ### remove in 4.0

#if defined(TQ_DISABLE_COPY)
    TQTable( const TQTable & );
    TQTable &operator=( const TQTable & );
#endif
};

#define Q_DEFINED_QTABLE
#include "ntqwinexport.h"
#endif // TQT_NO_TABLE
#endif // TABLE_H
