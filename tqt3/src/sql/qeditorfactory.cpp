/****************************************************************************
**
** Implementation of TQEditorFactory class
**
** Created : 2000-11-17
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

#include "ntqcleanuphandler.h"
#include "ntqlabel.h"
#include "ntqlineedit.h"
#include "ntqspinbox.h"
#include "ntqcombobox.h"

#include "ntqeditorfactory.h"
#include "ntqdatetimeedit.h"

#ifndef TQT_NO_SQL_EDIT_WIDGETS

/*!
    \class TQEditorFactory ntqeditorfactory.h
    \brief The TQEditorFactory class is used to create editor widgets
    for TQVariant data types.

    \ingroup database
    \module sql

    Each editor factory provides the createEditor() function which
    given a TQVariant will create and return a TQWidget that can edit
    that TQVariant. For example if you have a TQVariant::String type, a
    TQLineEdit would be the default editor returned, whereas a
    TQVariant::Int's default editor would be a TQSpinBox.

    If you want to create different editors for fields with the same
    data type, subclass TQEditorFactory and reimplement the
    createEditor() function.
*/

/*!
    Constructs an editor factory with parent \a parent, called \a name.
*/

TQEditorFactory::TQEditorFactory ( TQObject * parent, const char * name )
    : TQObject( parent, name )
{

}

/*!
    Destroys the object and frees any allocated resources.
*/

TQEditorFactory::~TQEditorFactory()
{

}

static TQEditorFactory * defaultfactory = 0;
static TQCleanupHandler< TQEditorFactory > q_cleanup_editor_factory;

/*!
    Returns an instance of a default editor factory.
*/

TQEditorFactory * TQEditorFactory::defaultFactory()
{
    if( defaultfactory == 0 ){
	defaultfactory = new TQEditorFactory();
	q_cleanup_editor_factory.add( &defaultfactory );
    }

    return defaultfactory;
}

/*!
    Replaces the default editor factory with \a factory.
    \e{TQEditorFactory takes ownership of factory, and destroys it
    when it is no longer needed.}
*/

void TQEditorFactory::installDefaultFactory( TQEditorFactory * factory )
{
    if( factory == 0 || factory == defaultfactory ) return;

    if( defaultfactory != 0 ){
	q_cleanup_editor_factory.remove( &defaultfactory );
	delete defaultfactory;
    }
    defaultfactory = factory;
    q_cleanup_editor_factory.add( &defaultfactory );
}

/*!
    Creates and returns the appropriate editor for the TQVariant \a v.
    If the TQVariant is invalid, 0 is returned. The \a parent is passed
    to the appropriate editor's constructor.
*/

TQWidget * TQEditorFactory::createEditor( TQWidget * parent, const TQVariant & v )
{
    TQWidget * w = 0;
    switch( v.type() ){
	case TQVariant::Invalid:
	    w = 0;
	    break;
	case TQVariant::Bool:
	    w = new TQComboBox( parent, "qt_editor_bool" );
	    ((TQComboBox *) w)->insertItem( "False" );
	    ((TQComboBox *) w)->insertItem( "True" );
	    break;
	case TQVariant::UInt:
	    w = new TQSpinBox( 0, 999999, 1, parent, "qt_editor_spinbox" );
	    break;
	case TQVariant::Int:
	    w = new TQSpinBox( -999999, 999999, 1, parent, "qt_editor_int" );
	    break;
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
