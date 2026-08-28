/****************************************************************************
**
** Definition of TQSqlError class
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

#ifndef TQSQLERROR_H
#define TQSQLERROR_H

#ifndef QT_H
#include "ntqstring.h"
#endif // QT_H

#if !defined( TQT_MODULE_SQL ) || defined( QT_LICENSE_PROFESSIONAL )
#define TQM_EXPORT_SQL
#else
#define TQM_EXPORT_SQL TQ_EXPORT
#endif

#ifndef TQT_NO_SQL

class TQM_EXPORT_SQL TQSqlError
{
public:
    enum Type {
	None,
	Connection,
	Statement,
	Transaction,
	Unknown
    };
    TQSqlError(  const TQString& driverText = TQString::null,
		const TQString& databaseText = TQString::null,
		int type = TQSqlError::None,
		int number = -1 );
    TQSqlError( const TQSqlError& other );
    TQSqlError& operator=( const TQSqlError& other );
    virtual ~TQSqlError();

    TQString	driverText() const;
    virtual void setDriverText( const TQString& driverText );
    TQString	databaseText() const;
    virtual void setDatabaseText( const TQString& databaseText );
    int		type() const;
    virtual void setType( int type );
    int		number() const;
    virtual void setNumber( int number );
    TQString 	text() const;
    void 	showMessage( const TQString& msg = TQString::null ) const;
    
private:
    TQString	driverError;
    TQString	databaseError;
    int		errorType;
    int	errorNumber;
};

#endif // TQT_NO_SQL
#endif
