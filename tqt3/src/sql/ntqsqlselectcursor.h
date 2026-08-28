/****************************************************************************
**
** Definition of TQSqlSelectCursor class
**
** Created : 2002-11-13
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

#ifndef TQSQLSELECTCURSOR_H
#define TQSQLSELECTCURSOR_H

#ifndef QT_H
#include "ntqsqlcursor.h"
#endif // QT_H

#if !defined( TQT_MODULE_SQL ) || defined( QT_LICENSE_PROFESSIONAL )
#define TQM_EXPORT_SQL
#else
#define TQM_EXPORT_SQL TQ_EXPORT
#endif

#ifndef TQT_NO_SQL

class TQSqlSelectCursorPrivate;

class TQM_EXPORT_SQL TQSqlSelectCursor : public TQSqlCursor
{
public:
    TQSqlSelectCursor( const TQString& query = TQString::null, TQSqlDatabase* db = 0 );
    TQSqlSelectCursor( const TQSqlSelectCursor& other );
    ~TQSqlSelectCursor();
    bool exec( const TQString& query );
    bool select() { return TQSqlCursor::select(); }
    
protected:
    TQSqlIndex primaryIndex( bool = true ) const { return TQSqlIndex(); }
    TQSqlIndex index( const TQStringList& ) const { return TQSqlIndex(); }
    TQSqlIndex index( const TQString& ) const { return TQSqlIndex(); }
    TQSqlIndex index( const char* ) const { return TQSqlIndex(); }
    void setPrimaryIndex( const TQSqlIndex& ) {}
    void append( const TQSqlFieldInfo& ) {}
    void insert( int, const TQSqlFieldInfo& ) {}
    void remove( int ) {}
    void clear() {}
    void setGenerated( const TQString&, bool ) {}
    void setGenerated( int, bool ) {}
    TQSqlRecord*	editBuffer( bool = false ) { return 0; }
    TQSqlRecord*	primeInsert() { return 0; }
    TQSqlRecord*	primeUpdate() { return 0; }
    TQSqlRecord*	primeDelete() { return 0; }
    int	insert( bool = true ) { return 0; }
    int	update( bool = true ) { return 0; }
    int	del( bool = true ) { return 0; }
    void setMode( int ) {}

    void setSort( const TQSqlIndex& ) {}
    TQSqlIndex sort() const { return TQSqlIndex(); }
    void setFilter( const TQString& ) {}
    TQString filter() const { return TQString::null; }
    void setName( const TQString&, bool = true ) {}
    TQString name() const { return TQString::null; }
    TQString toString( const TQString& = TQString::null, const TQString& = "," ) const { return TQString::null; }
    bool select( const TQString &, const TQSqlIndex& = TQSqlIndex() );

private:
    void populateCursor();
    
    TQSqlSelectCursorPrivate * d;
};

#endif // TQT_NO_SQL
#endif // TQSQLSELECTCURSOR_H
