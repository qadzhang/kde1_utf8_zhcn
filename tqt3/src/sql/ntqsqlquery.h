/****************************************************************************
**
** Definition of TQSqlQuery class
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

#ifndef TQSQLQUERY_H
#define TQSQLQUERY_H

#ifndef QT_H
#include "ntqobject.h"
#include "ntqstring.h"
#include "ntqvariant.h"
#include "ntqvaluelist.h"
#include "ntqsqlerror.h"
#include "ntqsqlfield.h"
#include "ntqsql.h"
#endif // QT_H

#ifndef TQT_NO_SQL

class TQSqlDriver;
class TQSqlResult;
class TQSqlDatabase;

class TQ_EXPORT TQSqlResultShared : public TQObject, public TQShared
{
    TQ_OBJECT
public:
    TQSqlResultShared( TQSqlResult* result );
    virtual ~TQSqlResultShared();
    TQSqlResult* sqlResult;
    TQString executedQuery;
private slots:
    void slotResultDestroyed();
};

class TQ_EXPORT TQSqlQuery
{
public:
    TQSqlQuery( TQSqlResult * r );
    TQSqlQuery( const TQString& query = TQString::null, TQSqlDatabase* db = 0 );
    explicit TQSqlQuery( TQSqlDatabase* db );
    TQSqlQuery( const TQSqlQuery& other );
    TQSqlQuery& operator=( const TQSqlQuery& other );
    virtual ~TQSqlQuery();

    bool                isValid() const;
    bool                isActive() const;
    bool	        isNull( int field ) const;
    int                 at() const;
    TQString             lastQuery() const;
    int                 numRowsAffected() const;
    TQSqlError	        lastError() const;
    bool                isSelect() const;
    int                 size() const;
    const TQSqlDriver*   driver() const;
    const TQSqlResult*   result() const;
    bool		isForwardOnly() const;
    void		setForwardOnly( bool forward );

    virtual bool	exec ( const TQString& query );
    virtual TQVariant    value( int i ) const;

    virtual bool	seek( int i, bool relative = false );
    virtual bool        next();
    virtual bool        prev();
    virtual bool        first();
    virtual bool        last();

    // prepared query support
    bool		exec();
    bool		prepare( const TQString& query );
    void		bindValue( const TQString& placeholder, const TQVariant& val );
    void		bindValue( int pos, const TQVariant& val );
    void		addBindValue( const TQVariant& val );
    // remove these overloads in 4.0
    void		bindValue( const TQString& placeholder, const TQVariant& val, TQSql::ParameterType type );
    void		bindValue( int pos, const TQVariant& val, TQSql::ParameterType type );
    void		addBindValue( const TQVariant& val, TQSql::ParameterType type );
    TQVariant		boundValue( const TQString& placeholder ) const;
    TQVariant		boundValue( int pos ) const;
    TQStringVariantMap boundValues() const;
    TQString             executedQuery() const;
    
protected:
    virtual void        beforeSeek();
    virtual void        afterSeek();

private:
    void 		init( const TQString& query, TQSqlDatabase* db );
    void                deref();
    bool                checkDetach();
    TQSqlResultShared*   d;
};


#endif // TQT_NO_SQL
#endif
