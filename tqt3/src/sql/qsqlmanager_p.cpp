/****************************************************************************
**
** Implementation of sql manager classes
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

#include "qsqlmanager_p.h"

#ifndef TQT_NO_SQL

#include "ntqapplication.h"
#include "ntqwidget.h"
#include "ntqsqlcursor.h"
#include "ntqsqlform.h"
#include "ntqsqldriver.h"
#include "ntqstring.h"
#include "ntqmessagebox.h"
#include "ntqbitarray.h"

//#define QT_DEBUG_DATAMANAGER

class TQSqlCursorManagerPrivate
{
public:
    TQSqlCursorManagerPrivate()
	: cur( 0 ), autoDelete( false )
    {}

    TQString ftr;
    TQStringList srt;
    TQSqlCursor* cur;
    bool autoDelete;
};

/*!
  \class TQSqlCursorManager qsqlmanager_p.h
  \brief The TQSqlCursorManager class manages a database cursor.

  \module sql

  \internal

  This class provides common cursor management functionality.  This
  includes saving and applying sorts and filters, refreshing (i.e.,
  re-selecting) the cursor and searching for records within the
  cursor.

*/

/*!  \internal

  Constructs a cursor manager.

*/

TQSqlCursorManager::TQSqlCursorManager()
{
    d = new TQSqlCursorManagerPrivate;
}


/*! \internal

  Destroys the object and frees any allocated resources.

*/

TQSqlCursorManager::~TQSqlCursorManager()
{
    if ( d->autoDelete )
	delete d->cur;
    delete d;
}

/*! \internal

  Sets the manager's sort to the index \a sort.  To apply the new
  sort, use refresh().

 */

void TQSqlCursorManager::setSort( const TQSqlIndex& sort )
{
    setSort( sort.toStringList() );
}

/*! \internal

  Sets the manager's sort to the stringlist \a sort.  To apply the
  new sort, use refresh().

 */

void TQSqlCursorManager::setSort( const TQStringList& sort )
{
    d->srt = sort;
}

/*! \internal

  Returns the current sort, or an empty stringlist if there is none.

*/

TQStringList  TQSqlCursorManager::sort() const
{
    return d->srt;
}

/*! \internal

  Sets the manager's filter to the string \a filter.  To apply the
  new filter, use refresh().

*/

void TQSqlCursorManager::setFilter( const TQString& filter )
{
    d->ftr = filter;
}

/*! \internal

  Returns the current filter, or an empty string if there is none.

*/

TQString TQSqlCursorManager::filter() const
{
    return d->ftr;
}

/*! \internal

  Sets auto-delete to \a enable.  If true, the default cursor will
  be deleted when necessary.

  \sa autoDelete()
*/

void TQSqlCursorManager::setAutoDelete( bool enable )
{
    d->autoDelete = enable;
}


/*! \internal

  Returns true if auto-deletion is enabled, otherwise false.

  \sa setAutoDelete()

*/

bool TQSqlCursorManager::autoDelete() const
{
    return d->autoDelete;
}

/*! \internal

  Sets the default cursor used by the manager to \a cursor.  If \a
  autoDelete is true (the default is false), the manager takes
  ownership of the \a cursor pointer, which will be deleted when the
  manager is destroyed, or when setCursor() is called again. To
  activate the \a cursor use refresh().

  \sa cursor()

*/

void TQSqlCursorManager::setCursor( TQSqlCursor* cursor, bool autoDelete )
{
    if ( d->autoDelete )
	delete d->cur;
    d->cur = cursor;
    d->autoDelete = autoDelete;
}

/*! \internal

  Returns a pointer to the default cursor used for navigation, or 0
  if there is no default cursor.

  \sa setCursor()

*/

TQSqlCursor* TQSqlCursorManager::cursor() const
{
    return d->cur;
}


/*! \internal

  Refreshes the manager using the default cursor.  The manager's
  filter and sort are applied.  Returns true on success, false if an
  error occurred or there is no current cursor.

  \sa setFilter() setSort()

*/

bool TQSqlCursorManager::refresh()
{
    TQSqlCursor* cur = cursor();
    if ( !cur )
	return false;
    TQString currentFilter = d->ftr;
    TQStringList currentSort = d->srt;
    TQSqlIndex newSort = TQSqlIndex::fromStringList( currentSort, cur );
    return cur->select( currentFilter, newSort );
}

/* \internal

   Returns true if the \a buf field values that correspond to \a idx
   match the field values in \a cur that correspond to \a idx.
*/

static bool index_matches( const TQSqlCursor* cur, const TQSqlRecord* buf,
			   const TQSqlIndex& idx )
{
    bool indexEquals = false;
    for ( uint i = 0; i < idx.count(); ++i ) {
	const TQString fn( idx.field(i)->name() );
	if ( cur->value( fn ) == buf->value( fn ) )
	    indexEquals = true;
	else {
	    indexEquals = false;
	    break;
	}
    }
    return indexEquals;
}

/*
  Return less than, equal to or greater than 0 if buf1 is less than,
  equal to or greater than buf2 according to fields described in idx.
  (### Currently only uses first field.)
*/

static int compare_recs( const TQSqlRecord* buf1, const TQSqlRecord* buf2,
			 const TQSqlIndex& idx )
{
    int cmp = 0;

    int i = 0;
    const TQString fn( idx.field(i)->name() );
    const TQSqlField* f1 = buf1->field( fn );

    if ( f1 ) {
	switch ( f1->type() ) { // ### more types?
	case TQVariant::String:
	case TQVariant::CString:
	    cmp = f1->value().toString().simplifyWhiteSpace().compare(
		          buf2->value(fn).toString().simplifyWhiteSpace() );
	    break;
	default:
	    if ( f1->value().toDouble() < buf2->value( fn ).toDouble() )
		cmp = -1;
	    else if ( f1->value().toDouble() > buf2->value( fn ).toDouble() )
		cmp = 1;
	}
    }

    if ( idx.isDescending(i) )
	cmp = -cmp;
    return cmp;
}

#ifdef QT_DEBUG_DATAMANAGER
static void debug_datamanager_buffer( const TQString& msg, TQSqlRecord* cursor )
{
    tqDebug("+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++");
    tqDebug( "%s", msg.latin1() );
    for ( uint j = 0; j < cursor->count(); ++j ) {
	tqDebug( "%s", (cursor->field(j)->name() + " type:"
		       + TQString(cursor->field(j)->value().typeName())
		       + " value:" + cursor->field(j)->value().toString())
		       .latin1() );
    }
}
#endif


/*! \internal

  Relocates the default cursor to the record matching the cursor's
edit buffer.  Only the field names specified by \a idx are used to
determine an exact match of the cursor to the edit buffer. However,
other fields in the edit buffer are also used during the search,
therefore all fields in the edit buffer should be primed with desired
values for the record being sought.  This function is typically used
to relocate a cursor to the correct position after an insert or
update.  For example:

\code
    TQSqlCursor* myCursor = myManager.cursor();
    ...
    TQSqlRecord* buf = myCursor->primeUpdate();
    buf->setValue( "name", "Ola" );
    buf->setValue( "city", "Oslo" );
    ...
    myCursor->update();  // update current record
    myCursor->select();  // refresh the cursor
    myManager.findBuffer( myCursor->primaryIndex() ); // go to the updated record
\endcode

*/

//## possibly add sizeHint parameter
bool TQSqlCursorManager::findBuffer( const TQSqlIndex& idx, int atHint )
{
#ifdef QT_DEBUG_DATAMANAGER
    tqDebug("TQSqlCursorManager::findBuffer:");
#endif
    TQSqlCursor* cur = cursor();
    if ( !cur )
	return false;
    if ( !cur->isActive() )
	return false;
    if ( !idx.count() ) {
	if ( cur->at() == TQSql::BeforeFirst )
	    cur->next();
	return false;
    }
    TQSqlRecord* buf = cur->editBuffer();
    bool indexEquals = false;
#ifdef QT_DEBUG_DATAMANAGER
    tqDebug(" Checking hint...");
#endif
    /* check the hint */
    if ( cur->seek( atHint ) )
	indexEquals = index_matches( cur, buf, idx );

    if ( !indexEquals ) {
#ifdef QT_DEBUG_DATAMANAGER
	tqDebug(" Checking current page...");
#endif
	/* check current page */
	int pageSize = 20;
	int startIdx = TQMAX( atHint - pageSize, 0 );
	int endIdx = atHint + pageSize;
	for ( int j = startIdx; j <= endIdx; ++j ) {
	    if ( cur->seek( j ) ) {
		indexEquals = index_matches( cur, buf, idx );
		if ( indexEquals )
		    break;
	    }
	}
    }

    if ( !indexEquals && cur->driver()->hasFeature( TQSqlDriver::QuerySize )
	 && cur->sort().count() ) {
#ifdef QT_DEBUG_DATAMANAGER
	tqDebug(" Using binary search...");
#endif
	// binary search based on record buffer and current sort fields
	int lo = 0;
	int hi = cur->size();
	int mid;
	if ( compare_recs( buf, cur, cur->sort() ) >= 0 )
	    lo = cur->at();
	while ( lo != hi ) {
	    mid = lo + (hi - lo) / 2;
	    if ( !cur->seek( mid ) )
		break;
	    if ( index_matches( cur, buf, idx ) ) {
		indexEquals = true;
		break;
	    }
	    int c = compare_recs( buf, cur, cur->sort() );
	    if ( c < 0 ) {
		hi = mid;
	    } else if ( c == 0 ) {
		// found it, but there may be duplicates
		int at = mid;
		do {
		    mid--;
		    if ( !cur->seek( mid ) )
			break;
		    if ( index_matches( cur, buf, idx ) ) {
			indexEquals = true;
			break;
		    }
		} while ( compare_recs( buf, cur, cur->sort() ) == 0 );

		if ( !indexEquals ) {
		    mid = at;
		    do {
			mid++;
			if ( !cur->seek( mid ) )
			    break;
			if ( index_matches( cur, buf, idx ) ) {
			    indexEquals = true;
			    break;
			}
		    } while ( compare_recs( buf, cur, cur->sort() ) == 0 );
		}
		break;
	    } else if ( c > 0 ) {
		lo = mid + 1;
	    }
	}
    }

    if ( !indexEquals ) {
#ifdef QT_DEBUG_DATAMANAGER
	tqDebug(" Using brute search...");
#endif
#ifndef TQT_NO_CURSOR
	TQApplication::setOverrideCursor( TQt::waitCursor );
#endif
	/* give up, use brute force */
	int startIdx = 0;
	if ( cur->at() != startIdx ) {
	    cur->seek( startIdx );
	}
	for ( ;; ) {
	    indexEquals = false;
	    indexEquals = index_matches( cur, buf, idx );
	    if ( indexEquals )
		break;
	    if ( !cur->next() )
		break;
	}
#ifndef TQT_NO_CURSOR
	TQApplication::restoreOverrideCursor();
#endif
    }
#ifdef QT_DEBUG_DATAMANAGER
	tqDebug(" Done, result:" + TQString::number( indexEquals ) );
#endif
    return indexEquals;
}

#ifndef TQT_NO_SQL_FORM

class TQSqlFormManagerPrivate
{
public:
    TQSqlFormManagerPrivate() : frm(0), rcd(0) {}
    TQSqlForm* frm;
    TQSqlRecord* rcd;
};


/*! \internal

  Creates a form manager.

*/

TQSqlFormManager::TQSqlFormManager()
{
    d = new TQSqlFormManagerPrivate();
}

/*! \internal

  Destroys the object and frees any allocated resources.

*/

TQSqlFormManager::~TQSqlFormManager()
{
    delete d;
}

/*!  \internal

  Clears the default form values.  If there is no default form,
  nothing happens,

*/

void TQSqlFormManager::clearValues()
{
    if ( form() )
	form()->clearValues();
}

/*! \internal

  Sets the form used by the form manager to \a form.  If a record has
  already been assigned to the form manager, that record is also used by
  the \a form to display data.

  \sa form()

*/

void TQSqlFormManager::setForm( TQSqlForm* form )
{
    d->frm = form;
    if ( d->rcd && d->frm )
	d->frm->setRecord( d->rcd );
}


/*! \internal

  Returns the default form used by the form manager, or 0 if there is
  none.

  \sa setForm()

*/

TQSqlForm* TQSqlFormManager::form()
{
    return d->frm;
}


/*! \internal

  Sets the record used by the form manager to \a record.  If a form has
  already been assigned to the form manager, \a record is also used by
  the default form to display data.

  \sa record()

*/

void TQSqlFormManager::setRecord( TQSqlRecord* record )
{
    d->rcd = record;
    if ( d->frm ) {
	d->frm->setRecord( d->rcd );
    }
}


/*! \internal

  Returns the default record used by the form manager, or 0 if there is
  none.

  \sa setRecord()
*/

TQSqlRecord* TQSqlFormManager::record()
{
    return d->rcd;
}


/*! \internal

  Causes the default form to read its fields .  If there is no
  default form, nothing happens.

  \sa setForm()

*/

void TQSqlFormManager::readFields()
{
    if ( d->frm ) {
	d->frm->readFields();
    }
}

/*! \internal

  Causes the default form to write its fields .  If there is no
  default form, nothing happens.

  \sa setForm()

*/

void TQSqlFormManager::writeFields()
{
    if ( d->frm ) {
	d->frm->writeFields();
    }
}

#endif // TQT_NO_SQL_FORM

class TQDataManagerPrivate
{
public:
    TQDataManagerPrivate()
	: mode( TQSql::None ), autoEd( true ), confEdits( 3 ),
	  confCancs( false ) {}
    TQSql::Op mode;
    bool autoEd;
    TQBitArray confEdits;
    bool confCancs;

};

/*!
  \class TQDataManager qsqlmanager_p.h
    \ingroup database

  \brief The TQDataManager class is an internal class for implementing
  the data-aware widgets.

  \internal

  TQDataManager is a strictly internal class that acts as a base class
  for other data-aware widgets.

*/


/*!  \internal

  Constructs an empty data handler.

*/

TQDataManager::TQDataManager()
{
    d = new TQDataManagerPrivate();
}


/*! \internal

  Destroys the object and frees any allocated resources.

*/

TQDataManager::~TQDataManager()
{
    delete d;
}


/*!  \internal

  Virtual function which is called when an error has occurred The
  default implementation displays a warning message to the user with
  information about the error.

*/
void TQDataManager::handleError( TQWidget* parent, const TQSqlError& e )
{
#ifndef TQT_NO_MESSAGEBOX
    if (e.driverText().isEmpty() && e.databaseText().isEmpty()) {
	TQMessageBox::warning ( parent, "Warning", "An error occurred while accessing the database");
    } else {
	TQMessageBox::warning ( parent, "Warning", e.driverText() + "\n" + e.databaseText(),
			   0, 0 );
    }
#endif // TQT_NO_MESSAGEBOX
}


/*! \internal

  Sets the internal mode to \a m.

*/

void TQDataManager::setMode( TQSql::Op m )
{
    d->mode = m;
}


/*! \internal

  Returns the current mode.

*/

TQSql::Op TQDataManager::mode() const
{
    return d->mode;
}


/*! \internal

  Sets the auto-edit mode to \a auto.

*/

void TQDataManager::setAutoEdit( bool autoEdit )
{
    d->autoEd = autoEdit;
}



/*! \internal

  Returns true if auto-edit mode is enabled; otherwise returns false.

*/

bool TQDataManager::autoEdit() const
{
    return d->autoEd;
}

/*! \internal

  If \a confirm is true, all edit operations (inserts, updates and
  deletes) will be confirmed by the user.  If \a confirm is false (the
  default), all edits are posted to the database immediately.

*/
void TQDataManager::setConfirmEdits( bool confirm )
{
    d->confEdits.fill( confirm );
}

/*! \internal

  If \a confirm is true, all inserts will be confirmed by the user.
  If \a confirm is false (the default), all edits are posted to the
  database immediately.

*/

void TQDataManager::setConfirmInsert( bool confirm )
{
    d->confEdits[ TQSql::Insert ] = confirm;
}

/*! \internal

  If \a confirm is true, all updates will be confirmed by the user.
  If \a confirm is false (the default), all edits are posted to the
  database immediately.

*/

void TQDataManager::setConfirmUpdate( bool confirm )
{
    d->confEdits[ TQSql::Update ] = confirm;
}

/*! \internal

  If \a confirm is true, all deletes will be confirmed by the user.
  If \a confirm is false (the default), all edits are posted to the
  database immediately.

*/

void TQDataManager::setConfirmDelete( bool confirm )
{
    d->confEdits[ TQSql::Delete ] = confirm;
}

/*! \internal

  Returns true if the table confirms all edit operations (inserts,
  updates and deletes), otherwise returns false.
*/

bool TQDataManager::confirmEdits() const
{
    return ( confirmInsert() && confirmUpdate() && confirmDelete() );
}

/*! \internal

  Returns true if the table confirms inserts, otherwise returns
  false.
*/

bool TQDataManager::confirmInsert() const
{
    return d->confEdits[ TQSql::Insert ];
}

/*! \internal

  Returns true if the table confirms updates, otherwise returns
  false.
*/

bool TQDataManager::confirmUpdate() const
{
    return d->confEdits[ TQSql::Update ];
}

/*! \internal

  Returns true if the table confirms deletes, otherwise returns
  false.
*/

bool TQDataManager::confirmDelete() const
{
    return d->confEdits[ TQSql::Delete ];
}

/*! \internal

  If \a confirm is true, all cancels will be confirmed by the user
  through a message box.  If \a confirm is false (the default), all
  cancels occur immediately.
*/

void TQDataManager::setConfirmCancels( bool confirm )
{
    d->confCancs = confirm;
}

/*! \internal

  Returns true if the table confirms cancels, otherwise returns false.
*/

bool TQDataManager::confirmCancels() const
{
    return d->confCancs;
}

/*!  \internal

  Virtual function which returns a confirmation for an edit of mode \a
  m.  Derived classes can reimplement this function and provide their
  own confirmation dialog.  The default implementation uses a message
  box which prompts the user to confirm the edit action.  The dialog
  is centered over \a parent.

*/

TQSql::Confirm TQDataManager::confirmEdit( TQWidget* parent, TQSql::Op m )
{
    int ans = 2;
    if ( m == TQSql::Delete ) {
#ifndef TQT_NO_MESSAGEBOX
	ans = TQMessageBox::information( parent,
					tqApp->translate( "TQSql", "Delete" ),
					tqApp->translate( "TQSql", "Delete this record?" ),
					tqApp->translate( "TQSql", "Yes" ),
					tqApp->translate( "TQSql", "No" ),
					TQString::null, 0, 1 );
#else
	ans = TQSql::No;
#endif // TQT_NO_MESSAGEBOX
    } else if ( m != TQSql::None ) {
	TQString caption;
	if ( m == TQSql::Insert ) {
	    caption = tqApp->translate( "TQSql", "Insert" );
	} else { // TQSql::Update
	    caption = tqApp->translate( "TQSql", "Update" );
	}
#ifndef TQT_NO_MESSAGEBOX
	ans = TQMessageBox::information( parent, caption,
					tqApp->translate( "TQSql", "Save edits?" ),
					tqApp->translate( "TQSql", "Yes" ),
					tqApp->translate( "TQSql", "No" ),
					tqApp->translate( "TQSql", "Cancel" ),
					0, 2 );
#else
	ans = TQSql::No;
#endif // TQT_NO_MESSAGEBOX
    }

    switch ( ans ) {
    case 0:
	return TQSql::Yes;
    case 1:
	return TQSql::No;
    default:
	return TQSql::Cancel;
    }
}

/*!  \internal

  Virtual function which returns a confirmation for cancelling an edit
  mode \a m.  Derived classes can reimplement this function and
  provide their own confirmation dialog.  The default implementation
  uses a message box which prompts the user to confirm the edit
  action. The dialog is centered over \a parent.


*/

TQSql::Confirm TQDataManager::confirmCancel( TQWidget* parent, TQSql::Op )
{
#ifndef TQT_NO_MESSAGEBOX
    switch ( TQMessageBox::information( parent,
				       tqApp->translate( "TQSql", "Confirm" ),
				       tqApp->translate( "TQSql", "Cancel your edits?" ),
				       tqApp->translate( "TQSql", "Yes" ),
				       tqApp->translate( "TQSql", "No" ),
				       TQString::null, 0, 1 ) ) {
    case 0:
	return TQSql::Yes;
    case 1:
	return TQSql::No;
    default:
	return TQSql::Cancel;
    }
#else
    return TQSql::Yes;
#endif // TQT_NO_MESSAGEBOX
}

#endif
