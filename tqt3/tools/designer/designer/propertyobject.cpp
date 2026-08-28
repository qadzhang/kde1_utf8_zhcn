/**********************************************************************
** Copyright (C) 2005-2008 Trolltech ASA.  All rights reserved.
**
** This file is part of TQt Designer.
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
** Licensees holding valid TQt Commercial licenses may use this file in
** accordance with the TQt Commercial License Agreement provided with
** the Software.
**
** This file is provided "AS IS" with NO WARRANTY OF ANY KIND,
** INCLUDING THE WARRANTIES OF DESIGN, MERCHANTABILITY AND FITNESS FOR
** A PARTICULAR PURPOSE. Trolltech reserves all rights not granted
** herein.
**
**********************************************************************/

#include "propertyobject.h"
#include "metadatabase.h"
#include <ntqptrvector.h>
#include <ntqptrlist.h>
#include <ntqmetaobject.h>
#include <ntqvariant.h>

PropertyObject::PropertyObject( const TQWidgetList &objs )
    : TQObject(), objects( objs ), mobj( 0 )
{
    TQPtrVector<TQPtrList<TQMetaObject> > v;
    v.resize( objects.count() );
    v.setAutoDelete( true );

    for ( TQObject *o = objects.first(); o; o = objects.next() ) {
	const TQMetaObject *m = o->metaObject();
	TQPtrList<TQMetaObject> *mol = new TQPtrList<TQMetaObject>;
	while ( m ) {
	    mol->insert( 0, m );
	    m = m->superClass();
	}
	v.insert( v.count(), mol );
    }

    int numObjects = objects.count();
    int minDepth = v[0]->count();
    int depth = minDepth;

    for ( int i = 0; i < numObjects; ++i ) {
	depth = (int)v[i]->count();
	if ( depth < minDepth )
	    minDepth = depth;
    }

    const TQMetaObject *m = v[0]->at( --minDepth );
    
    for ( int j = 0; j < numObjects; ++j ) {
	if ( v[j]->at( minDepth ) != m ) {
	    m = v[0]->at( --minDepth );
	    j = 0;
	}
    }

    mobj = m;
    
    Q_ASSERT( mobj );
}

bool PropertyObject::setProperty( const char *name, const TQVariant& value )
{
    for ( TQObject *o = objects.first(); o; o = objects.next() )
	o->setProperty( name, value );

    return true;
}

TQVariant PropertyObject::property( const char *name ) const
{
    return ( (PropertyObject*)this )->objects.first()->property( name );
}

void PropertyObject::mdPropertyChanged( const TQString &property, bool changed )
{
    for ( TQObject *o = objects.first(); o; o = objects.next() )
	MetaDataBase::setPropertyChanged( o, property, changed );
}

bool PropertyObject::mdIsPropertyChanged( const TQString &property )
{
    for ( TQObject *o = objects.first(); o; o = objects.next() ) {
	if ( MetaDataBase::isPropertyChanged( o, property ) )
	    return true;
    }
    return false;
}

void PropertyObject::mdSetPropertyComment( const TQString &property, const TQString &comment )
{
    for ( TQObject *o = objects.first(); o; o = objects.next() )
	MetaDataBase::setPropertyComment( o, property, comment );
}

TQString PropertyObject::mdPropertyComment( const TQString &property )
{
    return MetaDataBase::propertyComment( objects.first(), property );
}

void PropertyObject::mdSetFakeProperty( const TQString &property, const TQVariant &value )
{
    for ( TQObject *o = objects.first(); o; o = objects.next() )
	MetaDataBase::setFakeProperty( o, property, value );
}

TQVariant PropertyObject::mdFakeProperty( const TQString &property )
{
    return MetaDataBase::fakeProperty( objects.first(), property );
}

void PropertyObject::mdSetCursor( const TQCursor &c )
{
    for ( TQObject *o = objects.first(); o; o = objects.next() ) {
	if ( o->isWidgetType() )
	    MetaDataBase::setCursor( (TQWidget*)o, c );
    }
}

TQCursor PropertyObject::mdCursor()
{
    return MetaDataBase::cursor( objects.first() );
}

void PropertyObject::mdSetPixmapKey( int pixmap, const TQString &arg )
{
    for ( TQObject *o = objects.first(); o; o = objects.next() )
	MetaDataBase::setPixmapKey( o, pixmap, arg );
}

TQString PropertyObject::mdPixmapKey( int pixmap )
{
    return MetaDataBase::pixmapKey( objects.first(), pixmap );
}

void PropertyObject::mdSetExportMacro( const TQString &macro )
{
    for ( TQObject *o = objects.first(); o; o = objects.next() )
	MetaDataBase::setExportMacro( o, macro );
}

TQString PropertyObject::mdExportMacro()
{
    return MetaDataBase::exportMacro( objects.first() );
}
