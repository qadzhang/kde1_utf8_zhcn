/****************************************************************************
**
** Definition of TQSqlPropertyMap class
**
** Created : 2000-11-20
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

#include "ntqsqlpropertymap.h"

#ifndef TQT_NO_SQL_FORM

#include "ntqwidget.h"
#include "ntqcleanuphandler.h"
#include "ntqmetaobject.h"
#include "ntqmap.h"

class TQSqlPropertyMapPrivate
{
public:
    TQSqlPropertyMapPrivate() {}
    TQMap< TQString, TQString > propertyMap;
};

/*!
    \class TQSqlPropertyMap ntqsqlpropertymap.h
    \brief The TQSqlPropertyMap class is used to map widgets to SQL fields.

    \ingroup database
    \module sql

    The SQL module uses TQt \link properties.html object
    properties\endlink to insert and extract values from editor
    widgets.

    This class is used to map editors to SQL fields. This works by
    associating SQL editor class names to the properties used to
    insert and extract values to/from the editor.

    For example, a TQLineEdit can be used to edit text strings and
    other data types in TQDataTables or TQSqlForms. Several properties
    are defined in TQLineEdit, but only the \e text property is used to
    insert and extract text from a TQLineEdit. Both TQDataTable and
    TQSqlForm use the global TQSqlPropertyMap for inserting and
    extracting values to and from an editor widget. The global
    property map defines several common widgets and properties that
    are suitable for many applications. You can add and remove widget
    properties to suit your specific needs.

    If you want to use custom editors with your TQDataTable or
    TQSqlForm, you must install your own TQSqlPropertyMap for that table
    or form. Example:

    \code
    TQSqlPropertyMap *myMap  = new TQSqlPropertyMap();
    TQSqlForm        *myForm = new TQSqlForm( this );
    MyEditor myEditor( this );

    // Set the TQSqlForm's record buffer to the update buffer of
    // a pre-existing TQSqlCursor called 'cur'.
    myForm->setRecord( cur->primeUpdate() );

    // Install the customized map
    myMap->insert( "MyEditor", "content" );
    myForm->installPropertyMap( myMap ); // myForm now owns myMap
    ...
    // Insert a field into the form that uses a myEditor to edit the
    // field 'somefield'
    myForm->insert( &myEditor, "somefield" );

    // Update myEditor with the value from the mapped database field
    myForm->readFields();
    ...
    // Let the user edit the form
    ...
    // Update the database fields with the values in the form
    myForm->writeFields();
    ...
    \endcode

    You can also replace the global TQSqlPropertyMap that is used by
    default. (Bear in mind that TQSqlPropertyMap takes ownership of the
    new default map.)

    \code
    TQSqlPropertyMap *myMap = new TQSqlPropertyMap;

    myMap->insert( "MyEditor", "content" );
    TQSqlPropertyMap::installDefaultMap( myMap );
    ...
    \endcode

    \sa TQDataTable, TQSqlForm, TQSqlEditorFactory
*/

/*!

Constructs a TQSqlPropertyMap.

The default property mappings used by TQt widgets are:
\table
\header \i Widgets \i Property
\row \i \l TQCheckBox,
	\l TQRadioButton
     \i checked
\row \i \l TQComboBox,
	\l TQListBox
     \i currentItem
\row \i \l TQDateEdit
     \i date
\row \i \l TQDateTimeEdit
     \i dateTime
\row \i \l TQTextBrowser
     \i source
\row \i \l TQButton,
	\l TQDial,
	\l TQLabel,
	\l TQLineEdit,
	\l TQMultiLineEdit,
	\l TQPushButton,
	\l TQTextEdit,
     \i text
\row \i \l TQTimeEdit
     \i time
\row \i \l TQLCDNumber,
	\l TQScrollBar
	\l TQSlider,
	\l TQSpinBox
     \i value
\endtable
*/

TQSqlPropertyMap::TQSqlPropertyMap()
{
    d = new TQSqlPropertyMapPrivate();
    const struct MapData {
	const char *classname;
	const char *property;
    } mapData[] = {
	{ "TQButton", 		"text" },
	{ "TQCheckBox", 		"checked" },
	{ "TQRadioButton",	"checked" },
	{ "TQComboBox", 		"currentItem" },
	{ "TQDateEdit", 		"date" },
	{ "TQDateTimeEdit",	"dateTime" },
	{ "TQDial", 		"value" },
	{ "TQLabel", 		"text" },
	{ "TQLCDNumber",		"value" },
	{ "TQLineEdit",		"text" },
	{ "TQListBox",		"currentItem" },
	{ "TQMultiLineEdit",	"text" },
	{ "TQPushButton",	"text" },
	{ "TQScrollBar",		"value" },
	{ "TQSlider",		"value" },
	{ "TQSpinBox",		"value" },
	{ "TQTextBrowser",	"source" },
	{ "TQTextEdit",		"text" },
	{ "TQTextView",		"text" },
	{ "TQTimeEdit",		"time" }
    };

    const MapData *m = mapData;
    for ( uint i = 0; i < sizeof(mapData)/sizeof(MapData); i++, m++ )
	d->propertyMap.insert( m->classname, m->property );
}

/*!
    Destroys the TQSqlPropertyMap.

    Note that if the TQSqlPropertyMap is installed with
    installPropertyMap() the object it was installed into, e.g. the
    TQSqlForm, takes ownership and will delete the TQSqlPropertyMap when
    necessary.
*/
TQSqlPropertyMap::~TQSqlPropertyMap()
{
    delete d;
}

/*!
    Returns the mapped property of \a widget as a TQVariant.
*/
TQVariant TQSqlPropertyMap::property( TQWidget * widget )
{
    if( !widget ) return TQVariant();
    const TQMetaObject* mo = widget->metaObject();
    while ( mo && !d->propertyMap.contains( TQString( mo->className() ) ) )
	mo = mo->superClass();

    if ( !mo ) {
#ifdef QT_CHECK_RANGE
	tqWarning("TQSqlPropertyMap::property: %s does not exist", widget->metaObject()->className() );
#endif
	return TQVariant();
    }
    return widget->property( d->propertyMap[ mo->className() ] );
}

/*!
    Sets the property of \a widget to \a value.
*/
void TQSqlPropertyMap::setProperty( TQWidget * widget, const TQVariant & value )
{
    if( !widget ) return;

    TQMetaObject* mo = widget->metaObject();
    while ( mo && !d->propertyMap.contains( TQString( mo->className() ) ) )
	mo = mo->superClass();
    if ( !mo ) {
#ifdef QT_CHECK_RANGE
	tqWarning("TQSqlPropertyMap::setProperty: %s not handled by TQSqlPropertyMap", widget->metaObject()->className() );
#endif
	return;
    }

    widget->setProperty( d->propertyMap[ mo->className() ], value );
}

/*!
  Insert a new classname/property pair, which is used for custom SQL
  field editors. There \e must be a \c TQ_PROPERTY clause in the \a
  classname class declaration for the \a property.
*/
void TQSqlPropertyMap::insert( const TQString & classname,
			      const TQString & property )
{
    d->propertyMap[ classname ] = property;
}

/*!
    Removes \a classname from the map.
*/
void TQSqlPropertyMap::remove( const TQString & classname )
{
    d->propertyMap.remove( classname );
}

static TQSqlPropertyMap * defaultmap = 0;
static TQCleanupHandler< TQSqlPropertyMap > qsql_cleanup_property_map;

/*!
    Returns the application global TQSqlPropertyMap.
*/
TQSqlPropertyMap * TQSqlPropertyMap::defaultMap()
{
    if( defaultmap == 0 ){
	defaultmap = new TQSqlPropertyMap();
	qsql_cleanup_property_map.add( &defaultmap );
    }
    return defaultmap;
}

/*!
    Replaces the global default property map with \a map. All
    TQDataTable and TQSqlForm instantiations will use this new map for
    inserting and extracting values to and from editors.
    \e{TQSqlPropertyMap takes ownership of \a map, and destroys it
    when it is no longer needed.}
*/
void TQSqlPropertyMap::installDefaultMap( TQSqlPropertyMap * map )
{
    if( map == 0 ) return;

    if( defaultmap != 0 ){
	qsql_cleanup_property_map.remove( &defaultmap );
	delete defaultmap;
    }
    defaultmap = map;
    qsql_cleanup_property_map.add( &defaultmap );
}

#endif // TQT_NO_SQL_FORM
