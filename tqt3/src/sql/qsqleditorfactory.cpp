/****************************************************************************
**
** Implementation of TQSqlEditorFactory class
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

#include "ntqsqleditorfactory.h"

#ifndef TQT_NO_SQL_EDIT_WIDGETS

#include "ntqsqlfield.h"
#include "ntqcleanuphandler.h"
#include "ntqlabel.h"
#include "ntqlineedit.h"
#include "ntqspinbox.h"
#include "ntqcombobox.h"
#include "ntqdatetimeedit.h"

/*!
    \class TQSqlEditorFactory ntqsqleditorfactory.h
    \brief The TQSqlEditorFactory class is used to create the editors
    used by TQDataTable and TQSqlForm.

    \ingroup database
    \module sql

    TQSqlEditorFactory is used by TQDataTable and TQSqlForm to
    automatically create appropriate editors for a given TQSqlField.
    For example if the field is a TQVariant::String a TQLineEdit would
    be the default editor, whereas a TQVariant::Int's default editor
    would be a TQSpinBox.

    If you want to create different editors for fields with the same
    data type, subclass TQSqlEditorFactory and reimplement the
    createEditor() function.

    \sa TQDataTable, TQSqlForm
*/


/*!
    Constructs a SQL editor factory with parent \a parent, called \a
    name.
*/

TQSqlEditorFactory::TQSqlEditorFactory ( TQObject * parent, const char * name )
    : TQEditorFactory( parent, name )
{

}

/*!
    Destroys the object and frees any allocated resources.
*/

TQSqlEditorFactory::~TQSqlEditorFactory()
{

}

static TQSqlEditorFactory * defaultfactory = 0;
static TQCleanupHandler< TQSqlEditorFactory > qsql_cleanup_editor_factory;

/*!
    Returns an instance of a default editor factory.
*/

TQSqlEditorFactory * TQSqlEditorFactory::defaultFactory()
{
    if( defaultfactory == 0 ){
	defaultfactory = new TQSqlEditorFactory();
	qsql_cleanup_editor_factory.add( &defaultfactory );
    }

    return defaultfactory;
}

/*!
    Replaces the default editor factory with \a factory. All
    TQDataTable and TQSqlForm instantiations will use this new factory
    for creating field editors. \e{TQSqlEditorFactory takes ownership
    of \a factory, and destroys it when it is no longer needed.}
*/

void TQSqlEditorFactory::installDefaultFactory( TQSqlEditorFactory * factory )
{
    if( factory == 0 ) return;

    if( defaultfactory != 0 ){
	qsql_cleanup_editor_factory.remove( &defaultfactory );
	delete defaultfactory;
    }
    defaultfactory = factory;
    qsql_cleanup_editor_factory.add( &defaultfactory );
}

/*!
    Creates and returns the appropriate editor widget for the TQVariant
    \a variant.

    The widget that is returned has the parent \a parent (which may be
    zero). If \a variant is invalid, 0 is returned.
*/

TQWidget * TQSqlEditorFactory::createEditor( TQWidget * parent,
					   const TQVariant & variant )
{
    return TQEditorFactory::createEditor( parent, variant );
}

/*!
    \overload

    Creates and returns the appropriate editor for the TQSqlField \a
    field.
*/

TQWidget * TQSqlEditorFactory::createEditor( TQWidget * parent,
					   const TQSqlField * field )
{    
    if ( !field ) {
	return 0;
    }
    
    TQWidget * w = 0;
    switch( field->type() ){
	case TQVariant::Invalid:
	    w = 0;
	    break;
	case TQVariant::Bool:
	    w = new TQComboBox( parent, "qt_editor_bool" );
	    ((TQComboBox *) w)->insertItem( "False" );
	    ((TQComboBox *) w)->insertItem( "True" );
	    break;
	case TQVariant::UInt:
	    w = new TQSpinBox( 0, 2147483647, 1, parent, "qt_editor_spinbox" );
	    break;
	case TQVariant::Int:
	    w = new TQSpinBox( -2147483647, 2147483647, 1, parent, "qt_editor_int" );
	    break;
	case TQVariant::LongLong:
	case TQVariant::ULongLong:
	case TQVariant::String:
	case TQVariant::CString:
	case TQVariant::Double:
	    w = new TQLineEdit( parent, "qt_editor_double" );
	    ((TQLineEdit*)w)->setFrame( false );
	    break;
	case TQVariant::Date:
	    w = new TQDateEdit( parent, "qt_editor_date" );
	    break;
	case TQVariant::Time:
	    w = new TQTimeEdit( parent, "qt_editor_time" );
	    break;
	case TQVariant::DateTime:
	    w = new TQDateTimeEdit( parent, "qt_editor_datetime" );
	    break;
#ifndef TQT_NO_LABEL
	case TQVariant::Pixmap:
	    w = new TQLabel( parent, "qt_editor_pixmap" );
	    break;
#endif
	case TQVariant::Palette:
	case TQVariant::ColorGroup:
	case TQVariant::Color:
	case TQVariant::Font:
	case TQVariant::Brush:
	case TQVariant::Bitmap:
	case TQVariant::Cursor:
	case TQVariant::Map:
	case TQVariant::StringList:
	case TQVariant::Rect:
	case TQVariant::Size:
	case TQVariant::IconSet:
	case TQVariant::Point:
	case TQVariant::PointArray:
	case TQVariant::Region:
	case TQVariant::SizePolicy:
	case TQVariant::ByteArray:
	default:
	    w = new TQWidget( parent, "qt_editor_default" );
	    break;
    }
    return w;
}

#endif // TQT_NO_SQL
