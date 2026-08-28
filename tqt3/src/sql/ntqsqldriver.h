/****************************************************************************
**
** Definition of TQSqlDriver class
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

#ifndef TQSQLDRIVER_H
#define TQSQLDRIVER_H

#ifndef QT_H
#include "ntqobject.h"
#include "ntqptrdict.h"
#include "ntqstring.h"
#include "ntqsqlerror.h"
#include "ntqsqlquery.h"
#include "ntqsqlfield.h"
#include "ntqsqlindex.h"
#include "ntqstringlist.h"
#include "ntqmap.h"
#endif // QT_H

#if !defined( TQT_MODULE_SQL ) || defined( QT_LICENSE_PROFESSIONAL )
#define TQM_EXPORT_SQL
#else
#define TQM_EXPORT_SQL TQ_EXPORT
#endif

#ifndef TQT_NO_SQL

class TQSqlDriverExtension;

class TQSqlDatabase;

class TQM_EXPORT_SQL TQSqlDriver : public TQObject
{
    friend class TQSqlDatabase;
    TQ_OBJECT
public:
    enum DriverFeature { Transactions, QuerySize, BLOB, Unicode, PreparedQueries,
			 NamedPlaceholders, PositionalPlaceholders };

    TQSqlDriver( TQObject * parent=0, const char * name=0 );
    ~TQSqlDriver();
    bool			isOpen() const;
    bool			isOpenError() const;

    virtual bool		beginTransaction();
    virtual bool		commitTransaction();
    virtual bool		rollbackTransaction();
    virtual TQStringList		tables( const TQString& tableType ) const;
    virtual TQSqlIndex		primaryIndex( const TQString& tableName ) const;
    virtual TQSqlRecord		record( const TQString& tableName ) const;
    virtual TQSqlRecord		record( const TQSqlQuery& query ) const;
    virtual TQSqlRecordInfo	recordInfo( const TQString& tablename ) const;
    virtual TQSqlRecordInfo	recordInfo( const TQSqlQuery& query ) const;
    virtual TQString		nullText() const;
    virtual TQString		formatValue( const TQSqlField* field, bool trimStrings = false ) const;
    TQSqlError			lastError() const;

    virtual bool		hasFeature( DriverFeature f ) const = 0;
    virtual bool		open( const TQString & db,
				      const TQString & user = TQString::null,
				      const TQString & password = TQString::null,
				      const TQString & host = TQString::null,
				      int port = -1 ) = 0;
    virtual void		close() = 0;
    virtual bool		ping() = 0;
    virtual TQSqlQuery		createQuery() const = 0;

    // ### remove for 4.0
    bool			open( const TQString& db,
				      const TQString& user,
				      const TQString& password,
				      const TQString& host,
				      int port,
				      const TQString& connOpts );
protected:
    virtual void		setOpen( bool o );
    virtual void		setOpenError( bool e );
    virtual void		setLastError( const TQSqlError& e );
private:
    // ### This class needs a d-pointer in 4.0.
    int		          dbState;
    TQSqlError	          error;
#if defined(TQ_DISABLE_COPY)
    TQSqlDriver( const TQSqlDriver & );
    TQSqlDriver &operator=( const TQSqlDriver & );
#endif
};

#endif	// TQT_NO_SQL
#endif
