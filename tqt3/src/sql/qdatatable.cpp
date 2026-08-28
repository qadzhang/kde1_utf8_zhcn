/****************************************************************************
**
** Implementation of TQDataTable class
**
** Created : 2000-11-03
**
** Copyright (C) 2005-2008 Trolltech ASA.  All rights reserved.
**
** This file is part of the sql module of the TQt GUI Toolkit.
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

#include "ntqdatatable.h"

#ifndef TQT_NO_SQL_VIEW_WIDGETS

#include "ntqsqldriver.h"
#include "ntqsqleditorfactory.h"
#include "ntqsqlpropertymap.h"
#include "ntqapplication.h"
#include "ntqlayout.h"
#include "ntqpainter.h"
#include "ntqpopupmenu.h"
#include "ntqvaluelist.h"
#include "qsqlmanager_p.h"
#include "ntqdatetime.h"
#include "ntqcursor.h"
#include "ntqtimer.h"

//#define QT_DEBUG_DATATABLE

class TQDataTablePrivate
{
public:
    TQDataTablePrivate()
	: nullTxtChanged( false ),
	  haveAllRows( false ),
	  continuousEdit( false ),
	  editorFactory( 0 ),
	  propertyMap( 0 ),
	  editRow( -1 ),
	  editCol( -1 ),
	  insertRowLast( -1 ),
	  insertPreRows( -1 ),
	  editBuffer( 0 ),
	  cancelMode( false ),
	  cancelInsert( false ),
	  cancelUpdate( false )
    {}
    ~TQDataTablePrivate() { if ( propertyMap ) delete propertyMap; }

    TQString nullTxt;
    bool nullTxtChanged;
    typedef TQValueList< uint > ColIndex;
    ColIndex colIndex;
    bool haveAllRows;
    bool continuousEdit;
    TQSqlEditorFactory* editorFactory;
    TQSqlPropertyMap* propertyMap;
    TQString trueTxt;
    TQt::DateFormat datefmt;
    TQString falseTxt;
    int editRow;
    int editCol;
    int insertRowLast;
    TQString insertHeaderLabelLast;
    int insertPreRows;
    TQSqlRecord* editBuffer;
    bool cancelMode;
    bool cancelInsert;
    bool cancelUpdate;
    int lastAt;
    TQString ftr;
    TQStringList srt;
    TQStringList fld;
    TQStringList fldLabel;
    TQValueList<int> fldWidth;
    TQValueList<TQIconSet> fldIcon;
    TQValueList<bool> fldHidden;
    TQSqlCursorManager cur;
    TQDataManager dat;
};

#ifdef QT_DEBUG_DATATABLE
void qt_debug_buffer( const TQString& msg, TQSqlRecord* cursor )
{
    tqDebug("+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++");
    tqDebug(msg);
    for ( uint j = 0; j < cursor->count(); ++j ) {
	tqDebug(cursor->field(j)->name() + " type:" + TQString(cursor->field(j)->value().typeName()) + " value:" + cursor->field(j)->value().toString() );
    }
}
#endif

/*!
    \enum TQDataTable::Refresh

    This enum describes the refresh options.

    \value RefreshData  refresh the data, i.e. read it from the database
    \value RefreshColumns  refresh the list of fields, e.g. the column headings
    \value RefreshAll  refresh both the data and the list of fields
*/


/*!
    \class TQDataTable ntqdatatable.h
    \brief The TQDataTable class provides a flexible SQL table widget that supports browsing and editing.

    \ingroup database
    \mainclass
    \module sql

    TQDataTable supports various functions for presenting and editing
    SQL data from a \l TQSqlCursor in a table.

    If you want a to present your data in a form use TQDataBrowser, or
    for read-only forms, TQDataView.

    When displaying data, TQDataTable only retrieves data for visible
    rows. If the driver supports the 'query size' property the
    TQDataTable will have the correct number of rows and the vertical
    scrollbar will accurately reflect the number of rows displayed in
    proportion to the number of rows in the dataset. If the driver
    does not support the 'query size' property, rows are dynamically
    fetched from the database on an as-needed basis with the scrollbar
    becoming more accurate as the user scrolls down through the
    records. This allows extremely large queries to be displayed as
    quickly as possible, with minimum memory usage.

    TQDataTable inherits TQTable's API and extends it with functions to
    sort and filter the data and sort columns. See setSqlCursor(),
    setFilter(), setSort(), setSorting(), sortColumn() and refresh().

    When displaying editable cursors, cell editing will be enabled.
    (For more information on editable cursors, see \l TQSqlCursor).
    TQDataTable can be used to modify existing data and to add new
    records. When a user makes changes to a field in the table, the
    cursor's edit buffer is used. The table will not send changes in
    the edit buffer to the database until the user moves to a
    different record in the grid or presses Enter. Cell editing is
    initiated by pressing F2 (or right clicking and then clicking the
    appropriate popup menu item) and canceled by pressing Esc. If
    there is a problem updating or adding data, errors are handled
    automatically (see handleError() to change this behavior). Note
    that if autoEdit() is false navigating to another record will
    cancel the insert or update.

    The user can be asked to confirm all edits with setConfirmEdits().
    For more precise control use setConfirmInsert(),
    setConfirmUpdate(), setConfirmDelete() and setConfirmCancels().
    Use setAutoEdit() to control the behaviour of the table when the
    user edits a record and then navigates. (Note that setAutoDelete()
    is unrelated; it is used to set whether the TQSqlCursor is deleted
    when the table is deleted.)

    Since the data table can perform edits, it must be able to
    uniquely identify every record so that edits are correctly
    applied. Because of this the underlying cursor must have a valid
    primary index to ensure that a unique record is inserted, updated
    or deleted within the database otherwise the database may be
    changed to an inconsistent state.

    TQDataTable creates editors using the default \l TQSqlEditorFactory.
    Different editor factories can be used by calling
    installEditorFactory(). A property map is used to map between the
    cell's value and the editor. You can use your own property map
    with installPropertyMap().

    The contents of a cell is available as a TQString with text() or as
    a TQVariant with value(). The current record is returned by
    currentRecord(). Use the find() function to search for a string in
    the table.

    Editing actions can be applied programatically. For example, the
    insertCurrent() function reads the fields from the current record
    into the cursor and performs the insert. The updateCurrent() and
    deleteCurrent() functions perform similarly to update and delete
    the current record respectively.

    Columns in the table can be created automatically based on the
    cursor (see setSqlCursor()). Columns can be manipulated manually
    using addColumn(), removeColumn() and setColumn().

    The table automatically copies many of the properties of the
    cursor to format the display of data within cells (alignment,
    visibility, etc.). The cursor can be changed with setSqlCursor().
    The filter (see setFilter()) and sort defined within the table are
    used instead of the filter and sort set on the cursor. For sorting
    options see setSort(), sortColumn(), sortAscending() and
    sortDescending(). Note that sorting operations will not behave as
    expected if you are using a TQSqlSelectCursor because it uses
    user-defined SQL queries to obtain data.

    The text used to represent NULL, true and false values can be
    changed with setNullText(), setTrueText() and setFalseText()
    respectively. You can change the appearance of cells by
    reimplementing paintField().

    Whenever a new row is selected in the table the currentChanged()
    signal is emitted. The primeInsert() signal is emitted when an
    insert is initiated. The primeUpdate() and primeDelete() signals
    are emitted when update and deletion are initiated respectively.
    Just before the database is updated a signal is emitted;
    beforeInsert(), beforeUpdate() or beforeDelete() as appropriate.

*/

/*!
    Constructs a data table which is a child of \a parent, called
    name \a name.
*/

TQDataTable::TQDataTable ( TQWidget * parent, const char * name )
    : TQTable( parent, name )
{
    init();
}

/*!
    Constructs a data table which is a child of \a parent, called name
    \a name using the cursor \a cursor.

    If \a autoPopulate is true (the default is false), columns are
    automatically created based upon the fields in the \a cursor
    record. Note that \a autoPopulate only governs the creation of
    columns; to load the cursor's data into the table use refresh().

    If the \a cursor is read-only, the table also becomes read-only.
    In addition, the table adopts the cursor's driver's definition for
    representing NULL values as strings.
*/

TQDataTable::TQDataTable ( TQSqlCursor* cursor, bool autoPopulate, TQWidget * parent, const char * name )
    : TQTable( parent, name )
{
    init();
    setSqlCursor( cursor, autoPopulate );
}

/*! \internal
*/


void TQDataTable::init()
{
    d = new TQDataTablePrivate();
    setAutoEdit( true );
    setSelectionMode( SingleRow );
    setFocusStyle( FollowStyle );
    d->trueTxt = tr( "True" );
    d->falseTxt = tr( "False" );
    d->datefmt = TQt::LocalDate;
    reset();
    connect( this, TQ_SIGNAL( selectionChanged() ),
	     TQ_SLOT( updateCurrentSelection()));
}

/*!
    Destroys the object and frees any allocated resources.
*/

TQDataTable::~TQDataTable()
{
    delete d;
}


/*!
    Adds the next column to be displayed using the field \a fieldName,
    column label \a label, width \a width and iconset \a iconset.

    If \a label is specified, it is used as the column's header label,
    otherwise the field's display label is used when setSqlCursor() is
    called. The \a iconset is used to set the icon used by the column
    header; by default there is no icon.

    \sa setSqlCursor() refresh()
*/

void TQDataTable::addColumn( const TQString& fieldName,
			    const TQString& label,
			    int width,
			    const TQIconSet& iconset )
{
    d->fld += fieldName;
    d->fldLabel += label;
    d->fldIcon += iconset;
    d->fldWidth += width;
    d->fldHidden += false;
}

/*!
    Sets the \a col column to display using the field \a fieldName,
    column label \a label, width \a width and iconset \a iconset.

    If \a label is specified, it is used as the column's header label,
    otherwise the field's display label is used when setSqlCursor() is
    called. The \a iconset is used to set the icon used by the column
    header; by default there is no icon.

    \sa setSqlCursor() refresh()
*/

void TQDataTable::setColumn( uint col, const TQString& fieldName,
			    const TQString& label,
			    int width,
			    const TQIconSet& iconset )
{
    d->fld[col]= fieldName;
    d->fldLabel[col] = label;
    d->fldIcon[col] = iconset;
    d->fldWidth[col] = width;
    d->fldHidden[col] = false;
}

/*!
    Removes column \a col from the list of columns to be displayed. If
    \a col does not exist, nothing happens.

    \sa TQSqlField
*/

void TQDataTable::removeColumn( uint col )
{
    if ( d->fld.at( col ) != d->fld.end() ) {
	d->fld.remove( d->fld.at( col ) );
	d->fldLabel.remove( d->fldLabel.at( col ) );
	d->fldIcon.remove( d->fldIcon.at( col ) );
	d->fldWidth.remove( d->fldWidth.at( col ) );
	d->fldHidden.remove( d->fldHidden.at( col ) );
    }
}

/*!
    Sets the column \a col to the width \a w. Note that unlike TQTable
    the TQDataTable is not immediately redrawn, you must call
    refresh(TQDataTable::RefreshColumns)
    yourself.

    \sa refresh()
*/
void TQDataTable::setColumnWidth( int col, int w )
{
    if ( d->fldWidth.at( col ) != d->fldWidth.end() ) {
	d->fldWidth[col] = w;
    }
}

/*!
    Resizes column \a col so that the column width is wide enough to
    display the widest item the column contains (including the column
    label). If the table's TQSqlCursor is not currently active, the
    cursor will be refreshed before the column width is calculated. Be
    aware that this function may be slow on tables that contain large
    result sets.
*/
void TQDataTable::adjustColumn( int col )
{
    TQSqlCursor * cur = sqlCursor();
    if ( !cur || cur->count() <= (uint)col )
	return;
    if ( !cur->isActive() ) {
	d->cur.refresh();
    }
    int oldRow = currentRow();
    int w = fontMetrics().width( horizontalHeader()->label( col ) + "W" );
    cur->seek( TQSql::BeforeFirst );
    while ( cur->next() ) {
	w = TQMAX( w, fontMetrics().width( fieldToString( cur->field( indexOf( col ) ) ) ) + 10 );
    }
    setColumnWidth( col, w );
    cur->seek( oldRow );
    refresh( RefreshColumns );
}

/*! \reimp
*/
void TQDataTable::setColumnStretchable( int col, bool s )
{
    if ( numCols() == 0 ) {
	refresh( RefreshColumns );
    }
    if ( numCols() > col ) {
	TQTable::setColumnStretchable( col, s );
    }
}

TQString TQDataTable::filter() const
{
    return d->cur.filter();
}

/*!
    \property TQDataTable::filter
    \brief the data filter for the data table

    The filter applies to the data shown in the table. To view data
    with a new filter, use refresh(). A filter string is an SQL WHERE
    clause without the WHERE keyword.

    There is no default filter.

    \sa sort()

*/

void TQDataTable::setFilter( const TQString& filter )
{
    d->cur.setFilter( filter );
}


/*!
    \property TQDataTable::sort
    \brief the data table's sort

    The table's sort affects the order in which data records are
    displayed in the table. To apply a sort, use refresh().

    When examining the sort property, a string list is returned with
    each item having the form 'fieldname order' (e.g., 'id ASC',
    'surname DESC').

    There is no default sort.

    Note that if you want to iterate over the sort list, you should
    iterate over a copy, e.g.
    \code
    TQStringList list = myDataTable.sort();
    TQStringList::Iterator it = list.begin();
    while( it != list.end() ) {
	myProcessing( *it );
	++it;
    }
    \endcode

    \sa filter() refresh()
*/

void TQDataTable::setSort( const TQStringList& sort )
{
    d->cur.setSort( sort );
}

/*!
    \overload

    Sets the sort to be applied to the displayed data to \a sort. If
    there is no current cursor, nothing happens. A TQSqlIndex contains
    field names and their ordering (ASC or DESC); these are used to
    compose the ORDER BY clause.

    \sa sort()
*/

void TQDataTable::setSort( const TQSqlIndex& sort )
{
    d->cur.setSort( sort );
}

TQStringList TQDataTable::sort() const
{
    return d->cur.sort();
}

/*!
    Returns the cursor used by the data table.
*/

TQSqlCursor* TQDataTable::sqlCursor() const
{
    return d->cur.cursor();
}

void TQDataTable::setConfirmEdits( bool confirm )
{
    d->dat.setConfirmEdits( confirm );
}

void TQDataTable::setConfirmInsert( bool confirm )
{
    d->dat.setConfirmInsert( confirm );
}

void TQDataTable::setConfirmUpdate( bool confirm )
{
    d->dat.setConfirmUpdate( confirm );
}

void TQDataTable::setConfirmDelete( bool confirm )
{
    d->dat.setConfirmDelete( confirm );
}

/*!
    \property TQDataTable::confirmEdits
    \brief whether the data table confirms edit operations

    If the confirmEdits property is true, the data table confirms all
    edit operations (inserts, updates and deletes). Finer control of
    edit confirmation can be achieved using \l confirmCancels, \l
    confirmInsert, \l confirmUpdate and \l confirmDelete.

    \sa confirmCancels() confirmInsert() confirmUpdate() confirmDelete()
*/

bool TQDataTable::confirmEdits() const
{
    return ( d->dat.confirmEdits() );
}

/*!
    \property TQDataTable::confirmInsert
    \brief whether the data table confirms insert operations

    If the confirmInsert property is true, all insertions must be
    confirmed by the user through a message box (this behaviour can be
    changed by overriding the confirmEdit() function), otherwise all
    insert operations occur immediately.

    \sa confirmCancels() confirmEdits() confirmUpdate() confirmDelete()
*/

bool TQDataTable::confirmInsert() const
{
    return ( d->dat.confirmInsert() );
}

/*!
    \property TQDataTable::confirmUpdate
    \brief whether the data table confirms update operations

    If the confirmUpdate property is true, all updates must be
    confirmed by the user through a message box (this behaviour can be
    changed by overriding the confirmEdit() function), otherwise all
    update operations occur immediately.

    \sa confirmCancels() confirmEdits() confirmInsert() confirmDelete()
*/

bool TQDataTable::confirmUpdate() const
{
    return ( d->dat.confirmUpdate() );
}

/*!
    \property TQDataTable::confirmDelete
    \brief whether the data table confirms delete operations

    If the confirmDelete property is true, all deletions must be
    confirmed by the user through a message box (this behaviour can be
    changed by overriding the confirmEdit() function), otherwise all
    delete operations occur immediately.

    \sa confirmCancels() confirmEdits() confirmUpdate() confirmInsert()
*/

bool TQDataTable::confirmDelete() const
{
    return ( d->dat.confirmDelete() );
}

/*!
    \property TQDataTable::confirmCancels
    \brief whether the data table confirms cancel operations

    If the confirmCancel property is true, all cancels must be
    confirmed by the user through a message box (this behavior can be
    changed by overriding the confirmCancel() function), otherwise all
    cancels occur immediately. The default is false.

    \sa confirmEdits() confirmCancel()
*/

void TQDataTable::setConfirmCancels( bool confirm )
{
    d->dat.setConfirmCancels( confirm );
}

bool TQDataTable::confirmCancels() const
{
    return d->dat.confirmCancels();
}

/*!
    \reimp

    For an editable table, creates an editor suitable for the field in
    column \a col. The editor is created using the default editor
    factory, unless a different editor factory was installed with
    installEditorFactory(). The editor is primed with the value of the
    field in \a col using a property map. The property map used is the
    default property map, unless a new property map was installed with
    installPropertMap(). If \a initFromCell is true then the editor is
    primed with the value in the TQDataTable cell.
*/

TQWidget * TQDataTable::createEditor( int , int col, bool initFromCell ) const
{
    if ( d->dat.mode() == TQSql::None )
	return 0;

    TQSqlEditorFactory * f = (d->editorFactory == 0) ?
		     TQSqlEditorFactory::defaultFactory() : d->editorFactory;

    TQSqlPropertyMap * m = (d->propertyMap == 0) ?
			  TQSqlPropertyMap::defaultMap() : d->propertyMap;

    TQWidget * w = 0;
    if( initFromCell && d->editBuffer ){
	w = f->createEditor( viewport(), d->editBuffer->field( indexOf( col ) ) );
	if ( w )
	    m->setProperty( w, d->editBuffer->value( indexOf( col ) ) );
    }
    return w;
}

/*! \reimp */
bool TQDataTable::eventFilter( TQObject *o, TQEvent *e )
{
    if ( d->cancelMode )
	return true;

    int r = currentRow();
    int c = currentColumn();

    if ( d->dat.mode() != TQSql::None ) {
	r = d->editRow;
	c = d->editCol;
    }

    d->cancelInsert = false;
    d->cancelUpdate = false;
    switch ( e->type() ) {
    case TQEvent::KeyPress: {
	int conf = TQSql::Yes;
	TQKeyEvent *ke = (TQKeyEvent*)e;
	if ( ( ke->key() == Key_Tab || ke->key() == TQt::Key_BackTab )
	    && ke->state() & TQt::ControlButton )
	    return false;

	if ( ke->key() == Key_Escape && d->dat.mode() == TQSql::Insert ){
	    if ( confirmCancels() && !d->cancelMode ) {
		d->cancelMode = true;
		conf = confirmCancel( TQSql::Insert );
		d->cancelMode = false;
	    }
	    if ( conf == TQSql::Yes ) {
		d->cancelInsert = true;
	    } else {
		TQWidget *editorWidget = cellWidget( r, c );
		if ( editorWidget ) {
		    editorWidget->setActiveWindow();
		    editorWidget->setFocus();
		}
		return true;
	    }
	}
	if ( ke->key() == Key_Escape && d->dat.mode() == TQSql::Update ) {
	    if ( confirmCancels() && !d->cancelMode ) {
		d->cancelMode = true;
		conf = confirmCancel( TQSql::Update );
		d->cancelMode = false;
	    }
	    if ( conf == TQSql::Yes ){
		d->cancelUpdate = true;
	    } else {
		TQWidget *editorWidget = cellWidget( r, c );
		if ( editorWidget ) {
		    editorWidget->setActiveWindow();
		    editorWidget->setFocus();
		}
		return true;
	    }
	}
	if ( ke->key() == Key_Insert && d->dat.mode() == TQSql::None ) {
	    beginInsert();
	    return true;
	}
	if ( ke->key() == Key_Delete && d->dat.mode() == TQSql::None ) {
	    deleteCurrent();
	    return true;
	}
	if ( d->dat.mode() != TQSql::None ) {
	    if ( (ke->key() == Key_Tab) && (c < numCols() - 1) && (!isColumnReadOnly( c+1 ) || d->dat.mode() == TQSql::Insert) )
		d->continuousEdit = true;
	    else if ( (ke->key() == Key_BackTab) && (c > 0) && (!isColumnReadOnly( c-1 ) || d->dat.mode() == TQSql::Insert) )
		d->continuousEdit = true;
	    else
		d->continuousEdit = false;
	}
	TQSqlCursor * sql = sqlCursor();
	if ( sql && sql->driver() &&
	     !sql->driver()->hasFeature( TQSqlDriver::QuerySize ) &&
	     ke->key() == Key_End && d->dat.mode() == TQSql::None ) {
#ifndef TQT_NO_CURSOR
	    TQApplication::setOverrideCursor( TQt::WaitCursor );
#endif
	    int i = sql->at();
	    if ( i < 0 ) {
		i = 0;
		sql->seek(0);
	    }
	    while ( sql->next() )
		i++;
	    setNumRows( i+1 );
	    setCurrentCell( i+1, currentColumn() );
#ifndef TQT_NO_CURSOR
	    TQApplication::restoreOverrideCursor();
#endif
	    return true;
	}
	break;
    }
    case TQEvent::FocusOut: {
	TQWidget *editorWidget = cellWidget( r, c );
	repaintCell( currentRow(), currentColumn() );
	if ( !d->cancelMode && editorWidget && o == editorWidget &&
	     ( d->dat.mode() == TQSql::Insert) && !d->continuousEdit) {
	    setCurrentCell( r, c );
	    d->cancelInsert = true;
	}
	d->continuousEdit = false;
	break;
    }
    case TQEvent::FocusIn:
	repaintCell( currentRow(), currentColumn() );
	break;
    default:
	break;
    }
    return TQTable::eventFilter( o, e );
}

/*! \reimp */
void TQDataTable::resizeEvent ( TQResizeEvent * e )
{
    if ( sqlCursor() &&
	 sqlCursor()->driver() &&
	 !sqlCursor()->driver()->hasFeature( TQSqlDriver::QuerySize ) )
	loadNextPage();
    TQTable::resizeEvent( e );
}

/*! \reimp */
void TQDataTable::contentsContextMenuEvent( TQContextMenuEvent* e )
{
    TQTable::contentsContextMenuEvent( e );
    if ( isEditing() && d->dat.mode() != TQSql::None )
	endEdit( d->editRow, d->editCol, autoEdit(), false );
    if ( !sqlCursor() )
	return;
    if ( d->dat.mode() == TQSql::None ) {
	if ( isReadOnly() )
	    return;
	enum {
	    IdInsert,
	    IdUpdate,
	    IdDelete
	};
	TQGuardedPtr<TQPopupMenu> popup = new TQPopupMenu( this, "qt_datatable_menu" );
	int id[ 3 ];
	id[ IdInsert ] = popup->insertItem( tr( "Insert" ) );
	id[ IdUpdate ] = popup->insertItem( tr( "Update" ) );
	id[ IdDelete ] = popup->insertItem( tr( "Delete" ) );
	bool enableInsert = sqlCursor()->canInsert();
	popup->setItemEnabled( id[ IdInsert ], enableInsert );
	bool enableUpdate = currentRow() > -1 && sqlCursor()->canUpdate() && !isColumnReadOnly( currentColumn() );
	popup->setItemEnabled( id[ IdUpdate ], enableUpdate );
	bool enableDelete = currentRow() > -1 && sqlCursor()->canDelete();
	popup->setItemEnabled( id[ IdDelete ], enableDelete );
	int r = popup->exec( e->globalPos() );
	delete (TQPopupMenu*) popup;
	if ( r == id[ IdInsert ] )
	    beginInsert();
	else if ( r == id[ IdUpdate ] ) {
	    if ( beginEdit( currentRow(), currentColumn(), false ) )
		setEditMode( Editing, currentRow(), currentColumn() );
	    else
		endUpdate();
	}
	else if ( r == id[ IdDelete ] )
	    deleteCurrent();
	e->accept();
    }
}

/*! \reimp */
void TQDataTable::contentsMousePressEvent( TQMouseEvent* e )
{
    TQTable::contentsMousePressEvent( e );
}

/*! \reimp */
TQWidget* TQDataTable::beginEdit ( int row, int col, bool replace )
{
    d->editRow = -1;
    d->editCol = -1;
    if ( !sqlCursor() )
	return 0;
    if ( d->dat.mode() == TQSql::Insert && !sqlCursor()->canInsert() )
	return 0;
    if ( d->dat.mode() == TQSql::Update && !sqlCursor()->canUpdate() )
	return 0;
    d->editRow = row;
    d->editCol = col;
    if ( d->continuousEdit ) {
	// see comment in beginInsert()
	bool fakeReadOnly = isColumnReadOnly( col );
	setColumnReadOnly( col, false );
	TQWidget* w = TQTable::beginEdit( row, col, replace );
	setColumnReadOnly( col, fakeReadOnly );
	return w;
    }
    if ( d->dat.mode() == TQSql::None && sqlCursor()->canUpdate() && sqlCursor()->primaryIndex().count() > 0 )
	return beginUpdate( row, col, replace );
    return 0;
}

/*! \reimp */
void TQDataTable::endEdit( int row, int col, bool, bool )
{
    bool accept = autoEdit() && !d->cancelInsert && !d->cancelUpdate;

    TQWidget *editor = cellWidget( row, col );
    if ( !editor )
	return;
    if ( d->cancelMode )
	return;
    if ( d->dat.mode() != TQSql::None && d->editBuffer ) {
	TQSqlPropertyMap * m = (d->propertyMap == 0) ?
			      TQSqlPropertyMap::defaultMap() : d->propertyMap;
	d->editBuffer->setValue( indexOf( col ),  m->property( editor ) );
	clearCellWidget( row, col );
	if ( !d->continuousEdit ) {
	    switch ( d->dat.mode() ) {
	    case TQSql::Insert:
		if ( accept )
		    TQTimer::singleShot( 0, this, TQ_SLOT( doInsertCurrent() ) );
		else
		    endInsert();
		break;
	    case TQSql::Update:
		if ( accept )
		    TQTimer::singleShot( 0, this, TQ_SLOT( doUpdateCurrent() ) );
		else
		    endUpdate();
		break;
	    default:
		break;
	    }
	}
    } else {
	setEditMode( NotEditing, -1, -1 );
    }
    if ( d->dat.mode() == TQSql::None )
	viewport()->setFocus();
    updateCell( row, col );
    emit valueChanged( row, col );
}

/*! \internal */
void TQDataTable::doInsertCurrent()
{
    insertCurrent();
}

/*! \internal */
void TQDataTable::doUpdateCurrent()
{
    updateCurrent();
    if ( d->dat.mode() == TQSql::None ) {
	viewport()->setFocus();
    }
}

/*! \reimp */
void TQDataTable::activateNextCell()
{
//     if ( d->dat.mode() == TQSql::None )
//	TQTable::activateNextCell();
}

/*! \internal
*/

void TQDataTable::endInsert()
{
    if ( d->dat.mode() != TQSql::Insert )
	return;
    d->dat.setMode( TQSql::None );
    d->editBuffer = 0;
    verticalHeader()->setLabel( d->editRow, TQString::number( d->editRow +1 ) );
    d->editRow = -1;
    d->editCol = -1;
    d->insertRowLast = -1;
    d->insertHeaderLabelLast = TQString::null;
    setEditMode( NotEditing, -1, -1 );
    setNumRows( d->insertPreRows );
    d->insertPreRows = -1;
    viewport()->setFocus();
}

/*! \internal
*/

void TQDataTable::endUpdate()
{
    d->dat.setMode( TQSql::None );
    d->editBuffer = 0;
    updateRow( d->editRow );
    d->editRow = -1;
    d->editCol = -1;
    setEditMode( NotEditing, -1, -1 );
}

/*!
    Protected virtual function called when editing is about to begin
    on a new record. If the table is read-only, or if there's no
    cursor or the cursor does not allow inserts, nothing happens.

    Editing takes place using the cursor's edit buffer(see
    TQSqlCursor::editBuffer()).

    When editing begins, a new row is created in the table marked with
    an asterisk '*' in the row's vertical header column, i.e. at the
    left of the row.
*/

bool TQDataTable::beginInsert()
{
    if ( !sqlCursor() || isReadOnly() || !numCols() )
	return false;
    if ( !sqlCursor()->canInsert() )
	return false;
    int i = 0;
    int row = currentRow();

    d->insertPreRows = numRows();
    if ( row < 0 || numRows() < 1 )
	row = 0;
    setNumRows( d->insertPreRows + 1 );
    setCurrentCell( row, 0 );
    d->editBuffer = sqlCursor()->primeInsert();
    emit primeInsert( d->editBuffer );
    d->dat.setMode( TQSql::Insert );
    int lastRow = row;
    int lastY = contentsY() + visibleHeight();
    for ( i = row; i < numRows() ; ++i ) {
	TQRect cg = cellGeometry( i, 0 );
	if ( (cg.y()+cg.height()) > lastY ) {
	    lastRow = i;
	    break;
	}
    }
    if ( lastRow == row && ( numRows()-1 > row ) )
	lastRow = numRows() - 1;
    d->insertRowLast = lastRow;
    d->insertHeaderLabelLast = verticalHeader()->label( d->insertRowLast );
    verticalHeader()->setLabel( row, "*" );
    d->editRow = row;
    // in the db world it's common to allow inserting new records
    // into a table that has read-only columns - temporarily
    // switch off read-only mode for such columns
    bool fakeReadOnly = isColumnReadOnly( 0 );
    setColumnReadOnly( 0, false );
    if ( TQTable::beginEdit( row, 0, false ) )
	setEditMode( Editing, row, 0 );
    setColumnReadOnly( 0, fakeReadOnly );
    return true;
}

/*!
    Protected virtual function called when editing is about to begin
    on an existing row. If the table is read-only, or if there's no
    cursor, nothing happens.

    Editing takes place using the cursor's edit buffer (see
    TQSqlCursor::editBuffer()).

    \a row and \a col refer to the row and column in the TQDataTable.

    (\a replace is provided for reimplementors and reflects the API of
    TQTable::beginEdit().)
*/

TQWidget* TQDataTable::beginUpdate ( int row, int col, bool replace )
{
    if ( !sqlCursor() || isReadOnly() || isColumnReadOnly( col ) )
	return 0;
    setCurrentCell( row, col );
    d->dat.setMode( TQSql::Update );
    if ( sqlCursor()->seek( row ) ) {
	d->editBuffer = sqlCursor()->primeUpdate();
	sqlCursor()->seek( currentRow() );
	emit primeUpdate( d->editBuffer );
	return TQTable::beginEdit( row, col, replace );
    }
    return 0;
}

/*!
    For an editable table, issues an insert on the current cursor
    using the values in the cursor's edit buffer. If there is no
    current cursor or there is no current "insert" row, nothing
    happens. If confirmEdits() or confirmInsert() is true,
    confirmEdit() is called to confirm the insert. Returns true if the
    insert succeeded; otherwise returns false.

    The underlying cursor must have a valid primary index to ensure
    that a unique record is inserted within the database otherwise the
    database may be changed to an inconsistent state.
*/

bool TQDataTable::insertCurrent()
{
    if ( d->dat.mode() != TQSql::Insert || ! numCols() )
	return false;
    if ( !sqlCursor()->canInsert() ) {
#ifdef QT_CHECK_RANGE
	tqWarning("TQDataTable::insertCurrent: insert not allowed for %s",
		 sqlCursor()->name().latin1() );
#endif
	endInsert();
	return false;
    }
    int b = 0;
    int conf = TQSql::Yes;
    if ( confirmEdits() || confirmInsert() )
	conf = confirmEdit( TQSql::Insert );
    switch ( conf ) {
    case TQSql::Yes: {
#ifndef TQT_NO_CURSOR
	TQApplication::setOverrideCursor( TQt::waitCursor );
#endif
	emit beforeInsert( d->editBuffer );
	b = sqlCursor()->insert();
#ifndef TQT_NO_CURSOR
	TQApplication::restoreOverrideCursor();
#endif
	if ( ( !b && !sqlCursor()->isActive() ) || !sqlCursor()->isActive() ) {
	    handleError( sqlCursor()->lastError() );
	    endInsert(); // cancel the insert if anything goes wrong
	    refresh();
	} else {
	    endInsert();
	    refresh();
	    TQSqlIndex idx = sqlCursor()->primaryIndex();
	    findBuffer( idx, d->lastAt );
	    repaintContents( contentsX(), contentsY(), visibleWidth(), visibleHeight(), false );
	    emit cursorChanged( TQSql::Insert );
	}
	break;
    }
    case TQSql::No:
	endInsert();
	break;
    case TQSql::Cancel:
	if ( TQTable::beginEdit( currentRow(), currentColumn(), false ) )
	    setEditMode( Editing, currentRow(), currentColumn() );
	break;
    }
    return ( b > 0 );
}

/*! \internal

  Updates the row \a row.
*/

void TQDataTable::updateRow( int row )
{
    for ( int i = 0; i < numCols(); ++i )
	updateCell( row, i );
}

/*!
    For an editable table, issues an update using the cursor's edit
    buffer. If there is no current cursor or there is no current
    selection, nothing happens. If confirmEdits() or confirmUpdate()
    is true, confirmEdit() is called to confirm the update. Returns
    true if the update succeeded; otherwise returns false.

    The underlying cursor must have a valid primary index to ensure
    that a unique record is updated within the database otherwise the
    database may be changed to an inconsistent state.
*/

bool TQDataTable::updateCurrent()
{
    if ( d->dat.mode() != TQSql::Update )
	return false;
    if ( sqlCursor()->primaryIndex().count() == 0 ) {
#ifdef QT_CHECK_RANGE
	tqWarning("TQDataTable::updateCurrent: no primary index for %s",
		 sqlCursor()->name().latin1() );
#endif
	endUpdate();
	return false;
    }
    if ( !sqlCursor()->canUpdate() ) {
#ifdef QT_CHECK_RANGE
	tqWarning("TQDataTable::updateCurrent: updates not allowed for %s",
		 sqlCursor()->name().latin1() );
#endif
	endUpdate();
	return false;
    }
    int b = 0;
    int conf = TQSql::Yes;
    if ( confirmEdits() || confirmUpdate() )
	conf = confirmEdit( TQSql::Update );
    switch ( conf ) {
    case TQSql::Yes: {
#ifndef TQT_NO_CURSOR
	TQApplication::setOverrideCursor( TQt::waitCursor );
#endif
	emit beforeUpdate( d->editBuffer );
	b = sqlCursor()->update();
#ifndef TQT_NO_CURSOR
	TQApplication::restoreOverrideCursor();
#endif
	if ( ( !b && !sqlCursor()->isActive() ) || !sqlCursor()->isActive() ) {
	    handleError( sqlCursor()->lastError() );
	    endUpdate();
	    refresh();
	    setCurrentCell( d->editRow, d->editCol );
	    if ( TQTable::beginEdit( d->editRow, d->editCol, false ) )
		setEditMode( Editing, d->editRow, d->editCol );
	} else {
	    emit cursorChanged( TQSql::Update );
	    refresh();
	    endUpdate();
	}
	break;
    }
    case TQSql::No:
	endUpdate();
	setEditMode( NotEditing, -1, -1 );
	break;
    case TQSql::Cancel:
	setCurrentCell( d->editRow, d->editCol );
	if ( TQTable::beginEdit( d->editRow, d->editCol, false ) )
	    setEditMode( Editing, d->editRow, d->editCol );
	break;
    }
    return ( b > 0 );
}

/*!
    For an editable table, issues a delete on the current cursor's
    primary index using the values of the currently selected row. If
    there is no current cursor or there is no current selection,
    nothing happens. If confirmEdits() or confirmDelete() is true,
    confirmEdit() is called to confirm the delete. Returns true if the
    delete succeeded; otherwise false.

    The underlying cursor must have a valid primary index to ensure
    that a unique record is deleted within the database otherwise the
    database may be changed to an inconsistent state.
*/

bool TQDataTable::deleteCurrent()
{
    if ( !sqlCursor() || isReadOnly() )
	return false;
    if ( sqlCursor()->primaryIndex().count() == 0 ) {
#ifdef QT_CHECK_RANGE
	tqWarning("TQDataTable::deleteCurrent: no primary index %s",
		 sqlCursor()->name().latin1() );
#endif
	return false;
    }
    if ( !sqlCursor()->canDelete() )
	return false;

    int b = 0;
    int conf = TQSql::Yes;
    if ( confirmEdits() || confirmDelete() )
	conf = confirmEdit( TQSql::Delete );

    // Have to have this here - the confirmEdit() might pop up a
    // dialog that causes a repaint which the cursor to the
    // record it has to repaint.
    if ( !sqlCursor()->seek( currentRow() ) )
	return false;
    switch ( conf ) {
	case TQSql::Yes:{
#ifndef TQT_NO_CURSOR
	    TQApplication::setOverrideCursor( TQt::waitCursor );
#endif
	    sqlCursor()->primeDelete();
	    emit primeDelete( sqlCursor()->editBuffer() );
	    emit beforeDelete( sqlCursor()->editBuffer() );
	    b = sqlCursor()->del();
#ifndef TQT_NO_CURSOR
	    TQApplication::restoreOverrideCursor();
#endif
	    if ( !b )
		handleError( sqlCursor()->lastError() );
	    refresh();
	    emit cursorChanged( TQSql::Delete );
	    setCurrentCell( currentRow(), currentColumn() );
	    repaintContents( contentsX(), contentsY(), visibleWidth(), visibleHeight(), false );
	    verticalHeader()->repaint(); // get rid of trailing garbage
	}
	break;
    case TQSql::No:
	setEditMode( NotEditing, -1, -1 );
	break;
    }
    return ( b > 0 );
}

/*!
    Protected virtual function which returns a confirmation for an
    edit of mode \a m. Derived classes can reimplement this function
    to provide their own confirmation dialog. The default
    implementation uses a message box which prompts the user to
    confirm the edit action.
*/

TQSql::Confirm TQDataTable::confirmEdit( TQSql::Op m )
{
    return d->dat.confirmEdit( this, m );
}

/*!
    Protected virtual function which returns a confirmation for
    cancelling an edit mode of \a m. Derived classes can reimplement
    this function to provide their own cancel dialog. The default
    implementation uses a message box which prompts the user to
    confirm the cancel.
*/

TQSql::Confirm  TQDataTable::confirmCancel( TQSql::Op m )
{
    return d->dat.confirmCancel( this, m );
}


/*!
    Searches the current cursor for a cell containing the string \a
    str starting at the current cell and working forwards (or
    backwards if \a backwards is true). If the string is found, the
    cell containing the string is set as the current cell. If \a
    caseSensitive is false the case of \a str will be ignored.

    The search will wrap, i.e. if the first (or if backwards is true,
    last) cell is reached without finding \a str the search will
    continue until it reaches the starting cell. If \a str is not
    found the search will fail and the current cell will remain
    unchanged.
*/
void TQDataTable::find( const TQString & str, bool caseSensitive, bool backwards )
{
    if ( !sqlCursor() )
	return;

    TQSqlCursor * r = sqlCursor();
    TQString tmp, text;
    uint  row = currentRow(), startRow = row,
	  col = backwards ? currentColumn() - 1 : currentColumn() + 1;
    bool  wrap = true, found = false;

    if( str.isEmpty() || str.isNull() )
	return;

    if( !caseSensitive )
	tmp = str.lower();
    else
	tmp = str;

#ifndef TQT_NO_CURSOR
    TQApplication::setOverrideCursor( TQt::waitCursor );
#endif
    while( wrap ){
	while( !found && r->seek( row ) ){
	    for( int i = col; backwards ? (i >= 0) : (i < (int) numCols());
		 backwards ? i-- : i++ )
	    {
		text = r->value( indexOf( i ) ).toString();
		if( !caseSensitive ){
		    text = text.lower();
		}
		if( text.contains( tmp ) ){
		    setCurrentCell( row, i );
		    col = i;
		    found = true;
		}
	    }
	    if( !backwards ){
		col = 0;
		row++;
	    } else {
		col = numCols() - 1;
		row--;
	    }
	}
	if( !backwards ){
	    if( startRow != 0 ){
		startRow = 0;
	    } else {
		wrap = false;
	    }
	    r->first();
	    row = 0;
	} else {
	    if( startRow != (uint) (numRows() - 1) ){
		startRow = numRows() - 1;
	    } else {
		wrap = false;
	    }
	    r->last();
	    row = numRows() - 1;
	}
    }
#ifndef TQT_NO_CURSOR
    TQApplication::restoreOverrideCursor();
#endif
}


/*!
    Resets the table so that it displays no data.

    \sa setSqlCursor()
*/

void TQDataTable::reset()
{
    clearCellWidget( currentRow(), currentColumn() );
    switch ( d->dat.mode() ) {
    case TQSql::Insert:
	endInsert();
	break;
    case TQSql::Update:
	endUpdate();
	break;
    default:
	break;
    }
    ensureVisible( 0, 0 );
    verticalScrollBar()->setValue(0);
    setNumRows(0);

    d->haveAllRows = false;
    d->continuousEdit = false;
    d->dat.setMode( TQSql::None );
    d->editRow = -1;
    d->editCol = -1;
    d->insertRowLast = -1;
    d->insertHeaderLabelLast = TQString::null;
    d->cancelMode = false;
    d->lastAt = -1;
    d->fld.clear();
    d->fldLabel.clear();
    d->fldWidth.clear();
    d->fldIcon.clear();
    d->fldHidden.clear();
    if ( sorting() )
	horizontalHeader()->setSortIndicator( -1 );
}

/*!
    Returns the index of the field within the current SQL query that
    is displayed in column \a i.
*/

int TQDataTable::indexOf( uint i ) const
{
    TQDataTablePrivate::ColIndex::ConstIterator it = d->colIndex.at( i );
    if ( it != d->colIndex.end() )
	return *it;
    return -1;
}

/*!
    Returns true if the table will automatically delete the cursor
    specified by setSqlCursor(); otherwise returns false.
*/

bool TQDataTable::autoDelete() const
{
    return d->cur.autoDelete();
}

/*!
    Sets the cursor auto-delete flag to \a enable. If \a enable is
    true, the table will automatically delete the cursor specified by
    setSqlCursor(). If \a enable is false (the default), the cursor
    will not be deleted.
*/

void TQDataTable::setAutoDelete( bool enable )
{
    d->cur.setAutoDelete( enable );
}

/*!
    \property TQDataTable::autoEdit
    \brief whether the data table automatically applies edits

    The default value for this property is true. When the user begins
    an insert or update in the table there are two possible outcomes
    when they navigate to another record:

    \list 1
    \i the insert or update is is performed -- this occurs if autoEdit is true
    \i the insert or update is abandoned -- this occurs if autoEdit is false
    \endlist
*/

void TQDataTable::setAutoEdit( bool autoEdit )
{
    d->dat.setAutoEdit( autoEdit );
}

bool TQDataTable::autoEdit() const
{
    return d->dat.autoEdit();
}

/*!
    \property TQDataTable::nullText
    \brief the text used to represent NULL values

    The nullText property will be used to represent NULL values in the
    table. The default value is provided by the cursor's driver.
*/

void TQDataTable::setNullText( const TQString& nullText )
{
    d->nullTxt = nullText;
    d->nullTxtChanged = true;
}

TQString TQDataTable::nullText() const
{
    return d->nullTxt;
}

/*!
    \property TQDataTable::trueText
    \brief the text used to represent true values

    The trueText property will be used to represent NULL values in the
    table. The default value is "True".
*/

void TQDataTable::setTrueText( const TQString& trueText )
{
    d->trueTxt = trueText;
}

TQString TQDataTable::trueText() const
{
    return d->trueTxt;
}

/*!
    \property TQDataTable::falseText
    \brief the text used to represent false values

    The falseText property will be used to represent NULL values in
    the table. The default value is "False".
*/

void TQDataTable::setFalseText( const TQString& falseText )
{
    d->falseTxt = falseText;
}

TQString TQDataTable::falseText() const
{
    return d->falseTxt;
}

/*!
    \property TQDataTable::dateFormat
    \brief the format used for displaying date/time values

    The dateFormat property is used for displaying date/time values in
    the table. The default value is \c TQt::LocalDate.
*/

void TQDataTable::setDateFormat( const DateFormat f )
{
    d->datefmt = f;
}

TQt::DateFormat TQDataTable::dateFormat() const
{
    return d->datefmt;
}

/*!
    \property TQDataTable::numRows

    \brief the number of rows in the table
*/

int TQDataTable::numRows() const
{
    return TQTable::numRows();
}

/*!
    \reimp

    The number of rows in the table will be determined by the cursor
    (see setSqlCursor()), so normally this function should never be
    called. It is included for completeness.
*/

void TQDataTable::setNumRows ( int r )
{
    TQTable::setNumRows( r );
}

/*!
    \reimp

    The number of columns in the table will be determined
    automatically (see addColumn()), so normally this function should
    never be called. It is included for completeness.
*/

void TQDataTable::setNumCols ( int r )
{
    TQTable::setNumCols( r );
}

/*!
    \property TQDataTable::numCols

    \brief the number of columns in the table
*/

int TQDataTable::numCols() const
{
    return TQTable::numCols();
}

/*!
    Returns the text in cell \a row, \a col, or an empty string if the
    cell is empty. If the cell's value is NULL then nullText() will be
    returned. If the cell does not exist then TQString::null is
    returned.
*/

TQString TQDataTable::text ( int row, int col ) const
{
    if ( !sqlCursor() )
	return TQString::null;

    TQString s;
    if ( sqlCursor()->seek( row ) )
	s = sqlCursor()->value( indexOf( col ) ).toString();
    sqlCursor()->seek( currentRow() );
    return s;
}

/*!
    Returns the value in cell \a row, \a col, or an invalid value if
    the cell does not exist or has no value.
*/

TQVariant TQDataTable::value ( int row, int col ) const
{
    if ( !sqlCursor() )
	return TQVariant();

    TQVariant v;
    if ( sqlCursor()->seek( row ) )
	v = sqlCursor()->value( indexOf( col ) );
    sqlCursor()->seek( currentRow() );
    return v;
}

/*!  \internal
  Used to update the table when the size of the result set cannot be
  determined - divide the result set into pages and load the pages as
  the user moves around in the table.
*/
void TQDataTable::loadNextPage()
{
    if ( d->haveAllRows )
	return;
    if ( !sqlCursor() )
	return;
    int pageSize = 0;
    int lookAhead = 0;
    if ( height() ) {
	pageSize = (int)( height() * 2 / 20 );
	lookAhead = pageSize / 2;
    }
    int startIdx = verticalScrollBar()->value() / 20;
    int endIdx = startIdx + pageSize + lookAhead;
    if ( endIdx < numRows() || endIdx < 0 )
	return;

    // check for empty result set
    if ( sqlCursor()->at() == TQSql::BeforeFirst && !sqlCursor()->next() ) {
	d->haveAllRows = true;
	return;
    }

    while ( endIdx > 0 && !sqlCursor()->seek( endIdx ) )
	endIdx--;
    if ( endIdx != ( startIdx + pageSize + lookAhead ) )
	d->haveAllRows = true;
    // small hack to prevent TQTable from moving the view when a row
    // is selected and the contents is resized
    SelectionMode m = selectionMode();
    clearSelection();
    setSelectionMode( NoSelection );
    setNumRows( endIdx + 1 );
    sqlCursor()->seek( currentRow() );
    setSelectionMode( m );
}

/*! \internal */
void TQDataTable::sliderPressed()
{
    disconnect( verticalScrollBar(), TQ_SIGNAL( valueChanged(int) ),
		this, TQ_SLOT( loadNextPage() ) );
}

/*! \internal */
void TQDataTable::sliderReleased()
{
    loadNextPage();
    connect( verticalScrollBar(), TQ_SIGNAL( valueChanged(int) ),
	     this, TQ_SLOT( loadNextPage() ) );
}

/*!
    Sorts column \a col in ascending order if \a ascending is true
    (the default); otherwise sorts in descending order.

    The \a wholeRows parameter is ignored; TQDataTable always sorts
    whole rows by the specified column.
*/

void TQDataTable::sortColumn ( int col, bool ascending,
			      bool  )
{
    if ( sorting() ) {
	if ( isEditing() && d->dat.mode() != TQSql::None )
	    endEdit( d->editRow, d->editCol, autoEdit(), false );
	if ( !sqlCursor() )
	    return;
	TQSqlIndex lastSort = sqlCursor()->sort();
	TQSqlIndex newSort( lastSort.cursorName(), "newSort" );
	TQSqlField *field = sqlCursor()->field( indexOf( col ) );
	if ( field )
	    newSort.append( *field );
	newSort.setDescending( 0, !ascending );
	horizontalHeader()->setSortIndicator( col, ascending );
	setSort( newSort );
	refresh();
    }
}

/*! \reimp */
void TQDataTable::columnClicked ( int col )
{
    if ( sorting() ) {
	if ( !sqlCursor() )
	    return;
	TQSqlIndex lastSort = sqlCursor()->sort();
	bool asc = true;
	if ( lastSort.count() && lastSort.field( 0 )->name() == sqlCursor()->field( indexOf( col ) )->name() )
	    asc = lastSort.isDescending( 0 );
	sortColumn( col, asc );
	emit currentChanged( sqlCursor() );
    }
}

/*!
    \reimp

    Repaints the cell at \a row, \a col.
*/
void TQDataTable::repaintCell( int row, int col )
{
    TQRect cg = cellGeometry( row, col );
    TQRect re( TQPoint( cg.x() - 2, cg.y() - 2 ),
	      TQSize( cg.width() + 4, cg.height() + 4 ) );
    repaintContents( re, false );
}

/*!
    \reimp

    This function renders the cell at \a row, \a col with the value of
    the corresponding cursor field on the painter \a p. Depending on
    the table's current edit mode, paintField() is called for the
    appropriate cursor field. \a cr describes the cell coordinates in
    the content coordinate system. If \a selected is true the cell has
    been selected and would normally be rendered differently than an
    unselected cell.

    \sa TQSql::isNull()
*/

void TQDataTable::paintCell( TQPainter * p, int row, int col, const TQRect & cr,
			  bool selected, const TQColorGroup &cg )
{
    TQTable::paintCell( p, row, col, cr, selected, cg );  // empty cell

    if ( !sqlCursor() )
	return;

    p->setPen( selected ? cg.highlightedText() : cg.text() );
    if ( d->dat.mode() != TQSql::None ) {
	if ( row == d->editRow && d->editBuffer ) {
	    paintField( p, d->editBuffer->field( indexOf( col ) ), cr,
			selected );
	} else if ( row > d->editRow && d->dat.mode() == TQSql::Insert ) {
	    if ( sqlCursor()->seek( row - 1 ) )
		paintField( p, sqlCursor()->field( indexOf( col ) ), cr,
			    selected );
	} else {
	    if ( sqlCursor()->seek( row ) )
		paintField( p, sqlCursor()->field( indexOf( col ) ), cr,
			    selected );
	}
    } else {
	if ( sqlCursor()->seek( row ) )
		paintField( p, sqlCursor()->field( indexOf( col ) ), cr, selected );

    }
}


/*!
    Paints the \a field on the painter \a p. The painter has already
    been translated to the appropriate cell's origin where the \a
    field is to be rendered. \a cr describes the cell coordinates in
    the content coordinate system. The \a selected parameter is
    ignored.

    If you want to draw custom field content you must reimplement
    paintField() to do the custom drawing. The default implementation
    renders the \a field value as text. If the field is NULL,
    nullText() is displayed in the cell. If the field is Boolean,
    trueText() or falseText() is displayed as appropriate.
*/

void TQDataTable::paintField( TQPainter * p, const TQSqlField* field,
			    const TQRect & cr, bool )
{
    if ( !field )
	return;
    p->drawText( 2,2, cr.width()-4, cr.height()-4, fieldAlignment( field ), fieldToString( field ) );
}

/*!
    Returns the alignment for \a field.
*/

int TQDataTable::fieldAlignment( const TQSqlField* /*field*/ )
{
    return TQt::AlignLeft | TQt::AlignVCenter; //## Reggie: add alignment to TQTable
}


/*!
    If the cursor's \a sql driver supports query sizes, the number of
    rows in the table is set to the size of the query. Otherwise, the
    table dynamically resizes itself as it is scrolled. If \a sql is
    not active, it is made active by issuing a select() on the cursor
    using the \a sql cursor's current filter and current sort.
*/

void TQDataTable::setSize( TQSqlCursor* sql )
{
    // ### what are the connect/disconnect calls doing here!? move to refresh()
    if ( sql->driver() && sql->driver()->hasFeature( TQSqlDriver::QuerySize ) ) {
	setVScrollBarMode( Auto );
 	disconnect( verticalScrollBar(), TQ_SIGNAL( sliderPressed() ),
		    this, TQ_SLOT( sliderPressed() ) );
 	disconnect( verticalScrollBar(), TQ_SIGNAL( sliderReleased() ),
		    this, TQ_SLOT( sliderReleased() ) );
	disconnect( verticalScrollBar(), TQ_SIGNAL( valueChanged(int) ),
		    this, TQ_SLOT( loadNextPage() ) );
	if ( numRows() != sql->size() )
	    setNumRows( sql->size() );
    } else {
	setVScrollBarMode( AlwaysOn );
 	connect( verticalScrollBar(), TQ_SIGNAL( sliderPressed() ),
 		 this, TQ_SLOT( sliderPressed() ) );
 	connect( verticalScrollBar(), TQ_SIGNAL( sliderReleased() ),
 		 this, TQ_SLOT( sliderReleased() ) );
	connect( verticalScrollBar(), TQ_SIGNAL( valueChanged(int) ),
		 this, TQ_SLOT( loadNextPage() ) );
	setNumRows(0);
	loadNextPage();
    }
}

/*!
    Sets \a cursor as the data source for the table. To force the
    display of the data from \a cursor, use refresh(). If \a
    autoPopulate is true, columns are automatically created based upon
    the fields in the \a cursor record. If \a autoDelete is true (the
    default is false), the table will take ownership of the \a cursor
    and delete it when appropriate. If the \a cursor is read-only, the
    table becomes read-only. The table adopts the cursor's driver's
    definition for representing NULL values as strings.

    \sa refresh() setReadOnly() setAutoDelete() TQSqlDriver::nullText()
*/

void TQDataTable::setSqlCursor( TQSqlCursor* cursor, bool autoPopulate, bool autoDelete )
{
    setUpdatesEnabled( false );
    d->cur.setCursor( 0 );
    if ( cursor ) {
	d->cur.setCursor( cursor, autoDelete );
	if ( autoPopulate ) {
	    d->fld.clear();
	    d->fldLabel.clear();
	    d->fldWidth.clear();
	    d->fldIcon.clear();
	    d->fldHidden.clear();
	    for ( uint i = 0; i < sqlCursor()->count(); ++i ) {
		addColumn( sqlCursor()->field( i )->name(), sqlCursor()->field( i )->name() );
		setColumnReadOnly( i, sqlCursor()->field( i )->isReadOnly() );
	    }
	}
	setReadOnly( sqlCursor()->isReadOnly() );
	if ( sqlCursor()->driver() && !d->nullTxtChanged )
	    setNullText(sqlCursor()->driver()->nullText() );
	setAutoDelete( autoDelete );
    } else {
	setNumRows( 0 );
	setNumCols( 0 );
    }
    setUpdatesEnabled( true );
}


/*!
    Protected virtual function which is called when an error \a e has
    occurred on the current cursor(). The default implementation
    displays a warning message to the user with information about the
    error.
*/
void TQDataTable::handleError( const TQSqlError& e )
{
    d->dat.handleError( this, e );
}

/*! \reimp
  */

void TQDataTable::keyPressEvent( TQKeyEvent* e )
{
    switch( e->key() ) {
    case Key_Left:
    case Key_Right:
    case Key_Up:
    case Key_Down:
    case Key_Prior:
    case Key_Next:
    case Key_Home:
    case Key_End:
    case Key_F2:
    case Key_Enter: case Key_Return:
    case Key_Tab: case Key_BackTab:
	TQTable::keyPressEvent( e );
    default:
	return;
    }
}

/*!  \reimp
*/

void TQDataTable::resizeData ( int )
{

}

/*!  \reimp
*/

TQTableItem * TQDataTable::item ( int, int ) const
{
    return 0;
}

/*!  \reimp
*/

void TQDataTable::setItem ( int , int , TQTableItem * )
{

}

/*!  \reimp
*/

void TQDataTable::clearCell ( int , int )
{

}

/*!  \reimp
*/

void TQDataTable::setPixmap ( int , int , const TQPixmap &  )
{

}

/*! \reimp */
void TQDataTable::takeItem ( TQTableItem * )
{

}

/*!
    Installs a new SQL editor factory \a f. This enables the user to
    create and instantiate their own editors for use in cell editing.
    Note that TQDataTable takes ownership of this pointer, and will
    delete it when it is no longer needed or when
    installEditorFactory() is called again.

    \sa TQSqlEditorFactory
*/

void TQDataTable::installEditorFactory( TQSqlEditorFactory * f )
{
    if( f ) {
	delete d->editorFactory;
	d->editorFactory = f;
    }
}

/*!
    Installs a new property map \a m. This enables the user to create
    and instantiate their own property maps for use in cell editing.
    Note that TQDataTable takes ownership of this pointer, and will
    delete it when it is no longer needed or when installPropertMap()
    is called again.

    \sa TQSqlPropertyMap
*/

void TQDataTable::installPropertyMap( TQSqlPropertyMap* m )
{
    if ( m ) {
	delete d->propertyMap;
	d->propertyMap = m;
    }
}

/*!  \internal

  Sets the current selection to \a row, \a col.
*/

void TQDataTable::setCurrentSelection( int row, int )
{
    if ( !sqlCursor() )
	return;
    if ( row == d->lastAt )
	return;
    if ( !sqlCursor()->seek( row ) )
	return;
    d->lastAt = row;
    emit currentChanged( sqlCursor() );
}

void TQDataTable::updateCurrentSelection()
{
    setCurrentSelection( currentRow(), -1 );
}

/*!
    Returns the currently selected record, or 0 if there is no current
    selection. The table owns the pointer, so do \e not delete it or
    otherwise modify it or the cursor it points to.
*/

TQSqlRecord* TQDataTable::currentRecord() const
{
    if ( !sqlCursor() || currentRow() < 0 )
	return 0;
    if ( !sqlCursor()->seek( currentRow() ) )
	return 0;
    return sqlCursor();
}

/*!
    Sorts column \a col in ascending order.

    \sa setSorting()
*/

void TQDataTable::sortAscending( int col )
{
    sortColumn( col, true );
}

/*!
    Sorts column \a col in descending order.

    \sa setSorting()
*/

void TQDataTable::sortDescending( int col )
{
    sortColumn( col, false );
}

/*!
    \overload void TQDataTable::refresh( Refresh mode )

    Refreshes the table. If there is no currently defined cursor (see
    setSqlCursor()), nothing happens. The \a mode parameter determines
    which type of refresh will take place.

    \sa Refresh setSqlCursor() addColumn()
*/

void TQDataTable::refresh( TQDataTable::Refresh mode )
{
    TQSqlCursor* cur = sqlCursor();
    if ( !cur )
	return;
    bool refreshData = ( (mode & RefreshData) == RefreshData );
    bool refreshCol = ( (mode & RefreshColumns) == RefreshColumns );
    if ( ( (mode & RefreshAll) == RefreshAll ) ) {
	refreshData = true;
	refreshCol = true;
    }
    if ( !refreshCol && d->fld.count() && numCols() == 0 )
	refreshCol = true;
    viewport()->setUpdatesEnabled( false );
    d->haveAllRows = false;
    if ( refreshData ) {
	if ( !d->cur.refresh() && d->cur.cursor() ) {
	    handleError( d->cur.cursor()->lastError() );
	}
	d->lastAt = -1;
    }
    if ( refreshCol ) {
	setNumCols( 0 );
	d->colIndex.clear();
	if ( d->fld.count() ) {
	    TQSqlField* field = 0;
	    int i;
	    int fpos = -1;
	    for ( i = 0; i < (int)d->fld.count(); ++i ) {
		if ( cur->field( i ) && cur->field( i )->name() == d->fld[ i ] )
		    // if there is a field with the desired name on the desired position
		    // then we take that
		    fpos = i;
		else
		    // otherwise we take the first field that matches the desired name
		    fpos = cur->position( d->fld[ i ] );
		field = cur->field( fpos );
		if ( field && ( cur->isGenerated( fpos ) ||
				cur->isCalculated( field->name() ) ) )
		{
		    setNumCols( numCols() + 1 );
		    d->colIndex.append( fpos );
		    setColumnReadOnly( numCols()-1, field->isReadOnly() || isColumnReadOnly( numCols()-1 ) );
		    horizontalHeader()->setLabel( numCols()-1, d->fldIcon[ i ], d->fldLabel[ i ] );
		    if ( d->fldHidden[ i ] ) {
			TQTable::showColumn( i ); // ugly but necessary
			TQTable::hideColumn( i );
		    } else {
			TQTable::showColumn( i );
		    }
		    if ( d->fldWidth[ i ] > -1 )
			TQTable::setColumnWidth( i, d->fldWidth[i] );
		}
	    }
	}
    }
    viewport()->setUpdatesEnabled( true );
    viewport()->repaint( false );
    horizontalHeader()->repaint();
    verticalHeader()->repaint();
    setSize( cur );
    // keep others aware
    if ( d->lastAt == -1 )
 	setCurrentSelection( -1, -1 );
    else if ( d->lastAt != currentRow() )
	setCurrentSelection( currentRow(), currentColumn() );
    if ( cur->isValid() )
	emit currentChanged( sqlCursor() );
}

/*!
    Refreshes the table. The cursor is refreshed using the current
    filter, the current sort, and the currently defined columns.
    Equivalent to calling refresh( TQDataTable::RefreshData ).
*/

void TQDataTable::refresh()
{
    refresh( RefreshData );
}

/*!
    \reimp

    Selects the record in the table using the current cursor edit
    buffer and the fields specified by the index \a idx. If \a atHint
    is specified, it will be used as a hint about where to begin
    searching.
*/

bool TQDataTable::findBuffer( const TQSqlIndex& idx, int atHint )
{
    TQSqlCursor* cur = sqlCursor();
    if ( !cur )
	return false;
    bool found = d->cur.findBuffer( idx, atHint );
    if ( found )
	setCurrentCell( cur->at(), currentColumn() );
    return found;
}

/*! \internal
    Returns the string representation of a database field.
*/
TQString TQDataTable::fieldToString( const TQSqlField * field )
{
    TQString text;
    if ( field->isNull() ) {
	text = nullText();
    } else {
	TQVariant val = field->value();
	switch ( val.type() ) {
	    case TQVariant::Bool:
		text = val.toBool() ? d->trueTxt : d->falseTxt;
		break;
	    case TQVariant::Date:
		text = val.toDate().toString( d->datefmt );
		break;
	    case TQVariant::Time:
		text = val.toTime().toString( d->datefmt );
		break;
	    case TQVariant::DateTime:
		text = val.toDateTime().toString( d->datefmt );
		break;
	    default:
		text = val.toString();
		break;
	}
    }
    return text;
}

/*!
    \reimp
*/

void TQDataTable::swapColumns( int col1, int col2, bool )
{
    TQString fld = d->fld[ col1 ];
    TQString fldLabel = d->fldLabel[ col1 ];
    TQIconSet fldIcon = d->fldIcon[ col1 ];
    int fldWidth = d->fldWidth[ col1 ];

    d->fld[ col1 ] = d->fld[ col2 ];
    d->fldLabel[ col1 ] = d->fldLabel[ col2 ];
    d->fldIcon[ col1 ] = d->fldIcon[ col2 ];
    d->fldWidth[ col1 ] = d->fldWidth[ col2 ];

    d->fld[ col2 ] = fld;
    d->fldLabel[ col2 ] = fldLabel;
    d->fldIcon[ col2 ] = fldIcon;
    d->fldWidth[ col2 ] = fldWidth;

    int colIndex = d->colIndex[ col1 ];
    d->colIndex[ col1 ] = d->colIndex[ col2 ];
    d->colIndex[ col2 ] = colIndex;
}

/*!
    \reimp
*/

void TQDataTable::drawContents( TQPainter * p, int cx, int cy, int cw, int ch )
{
    TQTable::drawContents( p, cx, cy, cw, ch );
    if ( sqlCursor() && currentRow() >= 0 )
	sqlCursor()->seek( currentRow() );
}

/*!
    \reimp
*/

void TQDataTable::hideColumn( int col )
{
    d->fldHidden[col] = true;
    refresh( RefreshColumns );
}

/*!
    \reimp
*/

void TQDataTable::showColumn( int col )
{
    d->fldHidden[col] = false;
    refresh( RefreshColumns );
}

/*!
    \fn void TQDataTable::currentChanged( TQSqlRecord* record )

    This signal is emitted whenever a new row is selected in the
    table. The \a record parameter points to the contents of the newly
    selected record.
*/

/*!
    \fn void TQDataTable::primeInsert( TQSqlRecord* buf )

    This signal is emitted after the cursor is primed for insert by
    the table, when an insert action is beginning on the table. The \a
    buf parameter points to the edit buffer being inserted. Connect to
    this signal in order to, for example, prime the record buffer with
    default data values.
*/

/*!
    \fn void TQDataTable::primeUpdate( TQSqlRecord* buf )

    This signal is emitted after the cursor is primed for update by
    the table, when an update action is beginning on the table. The \a
    buf parameter points to the edit buffer being updated. Connect to
    this signal in order to, for example, provide some visual feedback
    that the user is in 'edit mode'.
*/

/*!
    \fn void TQDataTable::primeDelete( TQSqlRecord* buf )

    This signal is emitted after the cursor is primed for delete by
    the table, when a delete action is beginning on the table. The \a
    buf parameter points to the edit buffer being deleted. Connect to
    this signal in order to, for example, record auditing information
    on deletions.
*/

/*!
    \fn void TQDataTable::beforeInsert( TQSqlRecord* buf )

    This signal is emitted just before the cursor's edit buffer is
    inserted into the database. The \a buf parameter points to the
    edit buffer being inserted. Connect to this signal to, for
    example, populate a key field with a unique sequence number.
*/

/*!
    \fn void TQDataTable::beforeUpdate( TQSqlRecord* buf )

    This signal is emitted just before the cursor's edit buffer is
    updated in the database. The \a buf parameter points to the edit
    buffer being updated. Connect to this signal when you want to
    transform the user's data behind-the-scenes.
*/

/*!
    \fn void TQDataTable::beforeDelete( TQSqlRecord* buf )

    This signal is emitted just before the currently selected record
    is deleted from the database. The \a buf parameter points to the
    edit buffer being deleted. Connect to this signal to, for example,
    copy some of the fields for later use.
*/

/*!
    \fn void TQDataTable::cursorChanged( TQSql::Op mode )

    This signal is emitted whenever the cursor record was changed due
    to an edit. The \a mode parameter is the type of edit that just
    took place.
*/

#endif
