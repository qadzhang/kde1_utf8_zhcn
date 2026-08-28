/****************************************************************************
**
** Implementation of TQSqlForm class
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

#include "ntqsqlform.h"

#ifndef TQT_NO_SQL_FORM

#include "ntqsqlfield.h"
#include "ntqsqlpropertymap.h"
#include "ntqsqlrecord.h"
#include "ntqstringlist.h"
#include "ntqwidget.h"
#include "ntqdict.h"

class TQSqlFormPrivate
{
public:
    TQSqlFormPrivate() : propertyMap( 0 ), buf( 0 ), dirty( false ) {}
    ~TQSqlFormPrivate() { if ( propertyMap ) delete propertyMap; }
    TQStringList fld;
    TQDict<TQWidget> wgt;
    TQMap< TQWidget *, TQSqlField * > map;
    TQSqlPropertyMap * propertyMap;
    TQSqlRecord* buf;
    bool dirty;
};

/*!
    \class TQSqlForm
    \brief The TQSqlForm class creates and manages data entry forms
    tied to SQL databases.

    \ingroup database
    \mainclass
    \module sql

    Typical use of a TQSqlForm consists of the following steps:
    \list
    \i Create the widgets you want to appear in the form.
    \i Create a cursor and navigate to the record to be edited.
    \i Create the TQSqlForm.
    \i Set the form's record buffer to the cursor's update buffer.
    \i Insert each widget and the field it is to edit into the form.
    \i Use readFields() to update the editor widgets with values from
    the database's fields.
    \i Display the form and let the user edit values etc.
    \i Use writeFields() to update the database's field values with
    the values in the editor widgets.
    \endlist

    Note that a TQSqlForm does not access the database directly, but
    most often via TQSqlFields which are part of a TQSqlCursor. A
    TQSqlCursor::insert(), TQSqlCursor::update() or TQSqlCursor::del()
    call is needed to actually write values to the database.

    Some sample code to initialize a form successfully:

    \code
    TQLineEdit  myEditor( this );
    TQSqlForm   myForm( this );
    TQSqlCursor myCursor( "mytable" );

    // Execute a query to make the cursor valid
    myCursor.select();
    // Move the cursor to a valid record (the first record)
    myCursor.next();
    // Set the form's record pointer to the cursor's edit buffer (which
    // contains the current record's values)
    myForm.setRecord( myCursor.primeUpdate() );

    // Insert a field into the form that uses myEditor to edit the
    // field 'somefield' in 'mytable'
    myForm.insert( &myEditor, "somefield" );

    // Update myEditor with the value from the mapped database field
    myForm.readFields();
    ...
    // Let the user edit the form
    ...
    // Update the database
    myForm.writeFields();  // Update the cursor's edit buffer from the form
    myCursor.update();	// Update the database from the cursor's buffer
    \endcode

    If you want to use custom editors for displaying and editing data
    fields, you must install a custom TQSqlPropertyMap. The form
    uses this object to get or set the value of a widget.

    Note that \link designer-manual.book TQt Designer\endlink provides
    a visual means of creating data-aware forms.

    \sa installPropertyMap(), TQSqlPropertyMap
*/


/*!
    Constructs a TQSqlForm with parent \a parent and called \a name.
*/
TQSqlForm::TQSqlForm( TQObject * parent, const char * name )
    : TQObject( parent, name )
{
    d = new TQSqlFormPrivate();
}

/*!
    Destroys the object and frees any allocated resources.
*/
TQSqlForm::~TQSqlForm()
{
    delete d;
}

/*!
    Installs a custom TQSqlPropertyMap. This is useful if you plan to
    create your own custom editor widgets.

    TQSqlForm takes ownership of \a pmap, so \a pmap is deleted when
    TQSqlForm goes out of scope.

    \sa TQDataTable::installEditorFactory()
*/
void TQSqlForm::installPropertyMap( TQSqlPropertyMap * pmap )
{
    if( d->propertyMap )
	delete d->propertyMap;
    d->propertyMap = pmap;
}

/*!
    Sets \a buf as the record buffer for the form. To force the
    display of the data from \a buf, use readFields().

    \sa readFields() writeFields()
*/

void TQSqlForm::setRecord( TQSqlRecord* buf )
{
    d->dirty = true;
    d->buf = buf;
}

/*!
    Inserts a \a widget, and the name of the \a field it is to be
    mapped to, into the form. To actually associate inserted widgets
    with an edit buffer, use setRecord().

    \sa setRecord()
*/

void TQSqlForm::insert( TQWidget * widget, const TQString& field )
{
    d->dirty = true;
    d->wgt.insert( field, widget );
    d->fld += field;
}

/*!
    \overload

    Removes \a field from the form.
*/

void TQSqlForm::remove( const TQString& field )
{
    d->dirty = true;
    if ( d->fld.find( field ) != d->fld.end() )
	d->fld.remove( d->fld.find( field ) );
    d->wgt.remove( field );
}

/*!
    \overload

    Inserts a \a widget, and the \a field it is to be mapped to, into
    the form.
*/

void TQSqlForm::insert( TQWidget * widget, TQSqlField * field )
{
    d->map[widget] = field;
}

/*!
    Removes a \a widget, and hence the field it's mapped to, from the
    form.
*/

void TQSqlForm::remove( TQWidget * widget )
{
    d->map.remove( widget );
}

/*!
    Clears the values in all the widgets, and the fields they are
    mapped to, in the form. If \a nullify is true (the default is
    false), each field is also set to NULL.
*/
void TQSqlForm::clearValues( bool nullify )
{
    TQMap< TQWidget *, TQSqlField * >::Iterator it;
    for( it = d->map.begin(); it != d->map.end(); ++it ){
	TQSqlField* f = (*it);
	if ( f )
	    f->clear( nullify );
    }
    readFields();
}

/*!
    Removes every widget, and the fields they're mapped to, from the form.
*/
void TQSqlForm::clear()
{
    d->dirty = true;
    d->fld.clear();
    clearMap();
}

/*!
    Returns the number of widgets in the form.
*/
uint TQSqlForm::count() const
{
    return (uint)d->map.count();
}

/*!
    Returns the \a{i}-th widget in the form. Useful for traversing
    the widgets in the form.
*/
TQWidget * TQSqlForm::widget( uint i ) const
{
    TQMap< TQWidget *, TQSqlField * >::ConstIterator it;
    uint cnt = 0;

    if( i > d->map.count() ) return 0;
    for( it = d->map.begin(); it != d->map.end(); ++it ){
	if( cnt++ == i )
	    return it.key();
    }
    return 0;
}

/*!
    Returns the widget that field \a field is mapped to.
*/
TQWidget * TQSqlForm::fieldToWidget( TQSqlField * field ) const
{
    TQMap< TQWidget *, TQSqlField * >::ConstIterator it;
    for( it = d->map.begin(); it != d->map.end(); ++it ){
	if( *it == field )
	    return it.key();
    }
    return 0;
}

/*!
    Returns the SQL field that widget \a widget is mapped to.
*/
TQSqlField * TQSqlForm::widgetToField( TQWidget * widget ) const
{
    if( d->map.contains( widget ) )
	return d->map[widget];
    else
	return 0;
}

/*!
    Updates the widgets in the form with current values from the SQL
    fields they are mapped to.
*/
void TQSqlForm::readFields()
{
    sync();
    TQSqlField * f;
    TQMap< TQWidget *, TQSqlField * >::Iterator it;
    TQSqlPropertyMap * pmap = (d->propertyMap == 0) ?
			     TQSqlPropertyMap::defaultMap() : d->propertyMap;
    for(it = d->map.begin() ; it != d->map.end(); ++it ){
	f = widgetToField( it.key() );
	if( !f )
	    continue;
	pmap->setProperty( it.key(), f->value() );
    }
}

/*!
    Updates the SQL fields with values from the widgets they are
    mapped to. To actually update the database with the contents of
    the record buffer, use TQSqlCursor::insert(), TQSqlCursor::update()
    or TQSqlCursor::del() as appropriate.
*/
void TQSqlForm::writeFields()
{
    sync();
    TQSqlField * f;
    TQMap< TQWidget *, TQSqlField * >::Iterator it;
    TQSqlPropertyMap * pmap = (d->propertyMap == 0) ?
			     TQSqlPropertyMap::defaultMap() : d->propertyMap;

    for(it = d->map.begin() ; it != d->map.end(); ++it ){
	f = widgetToField( it.key() );
	if( !f )
	    continue;
	f->setValue( pmap->property( it.key() ) );
    }
}

/*!
    Updates the widget \a widget with the value from the SQL field it
    is mapped to. Nothing happens if no SQL field is mapped to the \a
    widget.
*/
void TQSqlForm::readField( TQWidget * widget )
{
    sync();
    TQSqlField * field = 0;
    TQSqlPropertyMap * pmap = (d->propertyMap == 0) ?
			     TQSqlPropertyMap::defaultMap() : d->propertyMap;
    field = widgetToField( widget );
    if( field )
	pmap->setProperty( widget, field->value() );
}

/*!
    Updates the SQL field with the value from the \a widget it is
    mapped to. Nothing happens if no SQL field is mapped to the \a
    widget.
*/
void TQSqlForm::writeField( TQWidget * widget )
{
    sync();
    TQSqlField * field = 0;
    TQSqlPropertyMap * pmap = (d->propertyMap == 0) ?
			     TQSqlPropertyMap::defaultMap() : d->propertyMap;
    field = widgetToField( widget );
    if( field )
	field->setValue( pmap->property( widget ) );
}

/*! \internal
*/

void TQSqlForm::sync()
{
    if ( d->dirty ) {
	clearMap();
	if ( d->buf ) {
	    for ( uint i = 0; i < d->fld.count(); ++i )
		insert( d->wgt[ d->fld[ i ] ], d->buf->field( d->fld[ i ] ) );
	}
    }
    d->dirty = false;
}

/*! \internal

  Clears the internal map of widget/field associations
*/

void TQSqlForm::clearMap()
{
    d->map.clear();
}

#endif // TQT_NO_SQL
