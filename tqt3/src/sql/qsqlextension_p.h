/****************************************************************************
**
** Definition of the TQSqlExtension class
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

#ifndef TQSQLEXTENSION_P_H
#define TQSQLEXTENSION_P_H

//
//  W A R N I N G
//  -------------
//
// This file is not part of the TQt API.  It exists for the convenience
// of other TQt classes.  This header file may change from version to
// version without notice, or even be removed.
//
// We mean it.
//
//

#ifndef QT_H
#include "ntqmap.h"
#include "ntqvaluevector.h"
#include "ntqstring.h"
#include "ntqvariant.h"
#include "ntqsql.h"
#endif // QT_H

#ifndef TQT_NO_SQL

#if !defined( TQT_MODULE_SQL ) || defined( QT_LICENSE_PROFESSIONAL )
#define TQM_EXPORT_SQL
#define TQM_TEMPLATE_EXTERN_SQL
#else
#define TQM_EXPORT_SQL TQ_EXPORT
#define TQM_TEMPLATE_EXTERN_SQL Q_TEMPLATE_EXTERN
#endif

struct TQSqlParam {
    TQSqlParam( const TQVariant& v = TQVariant(), TQSql::ParameterType t = TQSql::In ): value( v ), typ( t ) {}
    TQVariant value;
    TQSql::ParameterType typ;
};

struct Holder {
    Holder( const TQString& hldr = TQString::null, int pos = -1 ): holderName( hldr ), holderPos( pos ) {}
    bool operator==( const Holder& h ) const { return h.holderPos == holderPos && h.holderName == holderName; }
    bool operator!=( const Holder& h ) const { return h.holderPos != holderPos || h.holderName != holderName; }
    TQString holderName;
    int	    holderPos;
};

#define Q_DEFINED_QSQLEXTENSION
#include "ntqwinexport.h"

class TQM_EXPORT_SQL TQSqlExtension {
public:
    TQSqlExtension();
    virtual ~TQSqlExtension();
    virtual bool prepare( const TQString& query );
    virtual bool exec();
    virtual void bindValue( const TQString& holder, const TQVariant& value, TQSql::ParameterType = TQSql::In );
    virtual void bindValue( int pos, const TQVariant& value, TQSql::ParameterType = TQSql::In );
    virtual void addBindValue( const TQVariant& value, TQSql::ParameterType = TQSql::In );
    virtual TQVariant parameterValue( const TQString& holder );
    virtual TQVariant parameterValue( int pos );
    TQVariant boundValue( const TQString& holder ) const;
    TQVariant boundValue( int pos ) const;
    TQStringVariantMap boundValues() const;
    void clear();
    void clearValues();
    void clearIndex();
    void resetBindCount();

    enum BindMethod { BindByPosition, BindByName };
    BindMethod bindMethod(); // ### 4.0: make this const
    BindMethod bindm;
    int bindCount;

    TQMap<int, TQString> index;
    typedef TQMap<TQString, TQSqlParam> ValueMap;
    ValueMap values;

    // convenience container for TQSqlQuery
    // to map holders <-> positions
    typedef TQValueVector<Holder> HolderVector;
    HolderVector holders;
};

class TQM_EXPORT_SQL TQSqlDriverExtension
{
public:
    TQSqlDriverExtension();
    virtual ~TQSqlDriverExtension();
    virtual bool isOpen() const = 0;
};

class TQM_EXPORT_SQL TQSqlOpenExtension
{
public:
    TQSqlOpenExtension();
    virtual ~TQSqlOpenExtension();
    virtual bool open( const TQString& db,
		       const TQString& user,
		       const TQString& password,
		       const TQString& host,
		       int port,
		       const TQString& connOpts ) = 0;
};
#endif

#endif
