/****************************************************************************
**
** Definition of TQSqlResult class
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

#ifndef TQSQLRESULT_H
#define TQSQLRESULT_H

#ifndef QT_H
#include "ntqstring.h"
#include "ntqvariant.h"
#include "ntqsqlerror.h"
#include "ntqsqlfield.h"
#include "ntqsql.h"
#endif // QT_H

#if !defined( TQT_MODULE_SQL ) || defined( QT_LICENSE_PROFESSIONAL )
#define TQM_EXPORT_SQL
#else
#define TQM_EXPORT_SQL TQ_EXPORT
#endif

#ifndef TQT_NO_SQL

class TQSqlDriver;
class TQSql;
class TQSqlResultPrivate;
class TQSqlExtension;

class TQM_EXPORT_SQL TQSqlResult
{
friend class TQSqlQuery;
friend class TQSqlResultShared;
public:
    virtual ~TQSqlResult();
    
    // BCI HACK - remove in 4.0
    void 	    setExtension( TQSqlExtension * ext );
    TQSqlExtension * extension();

protected:
    TQSqlResult(const TQSqlDriver * db );
    int		    at() const;
    TQString         lastQuery() const;
    TQSqlError       lastError() const;
    bool            isValid() const;
    bool            isActive() const;
    bool            isSelect() const;
    bool            isForwardOnly() const;
    const TQSqlDriver* driver() const;
    virtual void    setAt( int at );
    virtual void    setActive( bool a );
    virtual void    setLastError( const TQSqlError& e );
    virtual void    setQuery( const TQString& query );
    virtual void    setSelect( bool s );
    virtual void    setForwardOnly( bool forward );

    virtual TQVariant data( int i ) = 0;
    virtual bool    isNull( int i ) = 0;
    virtual bool    reset ( const TQString& sqlquery ) = 0;
    virtual bool    fetch( int i ) = 0;
    virtual bool    fetchNext();
    virtual bool    fetchPrev();
    virtual bool    fetchFirst() = 0;
    virtual bool    fetchLast() = 0;
    virtual int     size() = 0;
    virtual int     numRowsAffected() = 0;
private:
    TQSqlResultPrivate* d;
    bool forwardOnly;

private:	// Disabled copy constructor and operator=
#if defined(TQ_DISABLE_COPY)
    TQSqlResult( const TQSqlResult & );
    TQSqlResult &operator=( const TQSqlResult & );
#endif
};

#endif	// TQT_NO_SQL
#endif
