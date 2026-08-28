/****************************************************************************
**
** Implementation of the TQSqlExtension class
**
** Created : 2002-06-03
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

#include "qsqlextension_p.h"

#ifndef TQT_NO_SQL
TQSqlExtension::TQSqlExtension()
    : bindm( BindByPosition ), bindCount( 0 )
{
}

TQSqlExtension::~TQSqlExtension()
{
}

bool TQSqlExtension::prepare( const TQString& /*query*/ )
{
    return false;
}

bool TQSqlExtension::exec()
{
    return false;
}

void TQSqlExtension::bindValue( const TQString& placeholder, const TQVariant& val, TQSql::ParameterType tp )
{
    bindm = BindByName;
    // if the index has already been set when doing emulated named
    // bindings - don't reset it
    if ( index.contains( (int)values.count() ) ) {
	index[ (int)values.count() ] = placeholder;
    }
    values[ placeholder ] = TQSqlParam( val, tp );
}

void TQSqlExtension::bindValue( int pos, const TQVariant& val, TQSql::ParameterType tp )
{
    bindm = BindByPosition;
    index[ pos ] = TQString::number( pos );
    TQString nm = TQString::number( pos );
    values[ nm ] = TQSqlParam( val, tp );
}

void TQSqlExtension::addBindValue( const TQVariant& val, TQSql::ParameterType tp )
{
    bindm = BindByPosition;
    bindValue( bindCount++, val, tp );
}

void TQSqlExtension::clearValues()
{
    values.clear();
    bindCount = 0;
}

void TQSqlExtension::resetBindCount()
{
    bindCount = 0;
}

void TQSqlExtension::clearIndex()
{
    index.clear();
    holders.clear();
}

void TQSqlExtension::clear()
{
    clearValues();
    clearIndex();
}

TQVariant TQSqlExtension::parameterValue( const TQString& holder )
{
    return values[ holder ].value;
}

TQVariant TQSqlExtension::parameterValue( int pos )
{
    return values[ index[ pos ] ].value;
}

TQVariant TQSqlExtension::boundValue( const TQString& holder ) const
{
    return values[ holder ].value;
}

TQVariant TQSqlExtension::boundValue( int pos ) const
{
    return values[ index[ pos ] ].value;
}

TQStringVariantMap TQSqlExtension::boundValues() const
{
    TQMap<TQString, TQSqlParam>::ConstIterator it;
    TQStringVariantMap m;
    if ( bindm == BindByName ) {
	for ( it = values.begin(); it != values.end(); ++it )
	    m.insert( it.key(), it.data().value );
    } else {
	TQString key, tmp, fmt;
	fmt.sprintf( "%%0%dd", TQString::number( values.count()-1 ).length() );
	for ( it = values.begin(); it != values.end(); ++it ) {
	    tmp.sprintf( fmt.ascii(), it.key().toInt() );
	    m.insert( tmp, it.data().value );
	}
    }
    return m;
}

TQSqlExtension::BindMethod TQSqlExtension::bindMethod()
{
    return bindm;
}

TQSqlDriverExtension::TQSqlDriverExtension()
{
}

TQSqlDriverExtension::~TQSqlDriverExtension()
{
}

TQSqlOpenExtension::TQSqlOpenExtension()
{
}

TQSqlOpenExtension::~TQSqlOpenExtension()
{
}
#endif
