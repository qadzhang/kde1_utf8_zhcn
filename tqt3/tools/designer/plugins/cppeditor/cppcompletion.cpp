/**********************************************************************
**
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

#include "cppcompletion.h"
#include <ntqobject.h>
#include <ntqmetaobject.h>
#include <ntqobjectlist.h>
#include <ntqregexp.h>

CppEditorCompletion::CppEditorCompletion( Editor *e )
    : EditorCompletion( e )
{
}

bool CppEditorCompletion::doObjectCompletion( const TQString &objName )
{
    if ( !ths )
	return false;
    TQString object( objName );
    int i = -1;
    if ( ( i = object.findRev( "->" ) ) != -1 )
	object = object.mid( i + 2 );
    if ( ( i = object.findRev( "." ) ) != -1 )
	object = object.mid( i + 1 );
    object = object.simplifyWhiteSpace();
    TQObject *obj = 0;
    if ( ths->name() == object || object == "this" ) {
	obj = ths;
    } else {
	obj = ths->child( object );
    }

    if ( !obj )
	return false;

    TQValueList<CompletionEntry> lst;

    if ( obj->children() ) {
	for ( TQObjectListIt cit( *obj->children() ); cit.current(); ++cit ) {
	    TQString s( cit.current()->name() );
	    if ( s.find( " " ) == -1 && s.find( "qt_" ) == -1 && s.find( "unnamed" ) == -1 ) {
		CompletionEntry c;
		c.type = "variable";
		c.text = s;
		c.prefix = "";
		lst << c;
	    }
	}
    }

    TQStrList props = obj->metaObject()->propertyNames( true );
    for ( TQPtrListIterator<char> pit( props ); pit.current(); ++pit ) {
	TQString f( pit.current() );
	TQChar c = f[ 0 ];
	f.remove( (uint)0, 1 );
	f.prepend( c.upper() );
	f.prepend( "set" );

	CompletionEntry ce;
	ce.type = "property";
	ce.text = f;
	ce.postfix = "()";

	if ( lst.find( ce ) == lst.end() )
	    lst << ce;
    }

    TQStrList slts = obj->metaObject()->slotNames( true );
    for ( TQPtrListIterator<char> sit( slts ); sit.current(); ++sit ) {
	TQString f( sit.current() );
	f = f.left( f.find( "(" ) );
	CompletionEntry c;
	c.type = "slot";
	c.text = f;
	c.postfix = "()";
	if ( lst.find( c ) == lst.end() )
	    lst << c;
    }

    if ( lst.isEmpty() )
	return false;

    showCompletion( lst );
    return true;
}

TQValueList<TQStringList> CppEditorCompletion::functionParameters( const TQString &expr, TQChar &separator,
						     TQString &prefix, TQString &postfix )
{
    Q_UNUSED( prefix );
    Q_UNUSED( postfix );
    separator = ',';
    if ( !ths )
	return TQValueList<TQStringList>();
    TQString func;
    TQString objName;
    int i = -1;

    i = expr.findRev( "->" );
    if ( i == -1 )
	i = expr.findRev( "." );
    else
	++i;
    if ( i == -1 ) {
	i = expr.findRev( " " );

	if ( i == -1 )
	    i = expr.findRev( "\t" );
	else
	    objName = ths->name();

	if ( i == -1 && expr[ 0 ] != ' ' && expr[ 0 ] != '\t' )
	    objName = ths->name();
    }

    if ( !objName.isEmpty() ) {
	func = expr.mid( i + 1 );
	func = func.simplifyWhiteSpace();
    } else {
	func = expr.mid( i + 1 );
	func = func.simplifyWhiteSpace();
	TQString ex( expr );
	ex.remove( i, 0xFFFFFF );
	if ( ex[ (int)ex.length() - 1 ] == '-' )
	    ex.remove( ex.length() - 1, 1 );
	int j = -1;
	j = ex.findRev( "->" );
	if ( j == -1 )
	    j = ex.findRev( "." );
	else
	    ++j;
	if ( j == -1 ) {
	    j = ex.findRev( " " );

	    if ( j == -1 )
		j = ex.findRev( "\t" );
	    else
		objName = ths->name();

	    if ( j == -1 )
		objName = ths->name();
	}
	objName = ex.mid( j + 1 );
	objName = objName.simplifyWhiteSpace();
    }

    TQObject *obj = 0;
    if ( ths->name() == objName || objName == "this" ) {
	obj = ths;
    } else {
	obj = ths->child( objName );
    }

    if ( !obj )
	return TQValueList<TQStringList>();

    TQStrList slts = obj->metaObject()->slotNames( true );
    for ( TQPtrListIterator<char> sit( slts ); sit.current(); ++sit ) {
	TQString f( sit.current() );
	f = f.left( f.find( "(" ) );
	if ( f == func ) {
	    f = TQString( sit.current() );
	    f.remove( (uint)0, f.find( "(" ) + 1 );
	    f = f.left( f.find( ")" ) );
	    TQStringList lst = TQStringList::split( ',', f );
	    if ( !lst.isEmpty() ) {
		TQValueList<TQStringList> l;
		l << lst;
		return l;
	    }
	}
    }

    const TQMetaProperty *prop =
	obj->metaObject()->
	property( obj->metaObject()->findProperty( func[ 3 ].lower() + func.mid( 4 ), true ), true );
    if ( prop ) {
	TQValueList<TQStringList> l;
	l << TQStringList( prop->type() );
	return l;
    }

    return TQValueList<TQStringList>();
}

void CppEditorCompletion::setContext( TQObject *this_ )
{
    ths = this_;
}
