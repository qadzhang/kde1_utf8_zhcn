/****************************************************************************
**
** Definition of TQSqlField class
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

#ifndef TQSQLFIELD_H
#define TQSQLFIELD_H

#ifndef QT_H
#include "ntqstring.h"
#include "ntqvariant.h"
#endif // QT_H

#if !defined( TQT_MODULE_SQL ) || defined( QT_LICENSE_PROFESSIONAL )
#define TQM_EXPORT_SQL
#else
#define TQM_EXPORT_SQL TQ_EXPORT
#endif

#ifndef TQT_NO_SQL

class TQSqlFieldPrivate
{
public:
    TQVariant::Type type;
};

class TQM_EXPORT_SQL TQSqlField
{
public:
    TQSqlField( const TQString& fieldName = TQString::null, TQVariant::Type type = TQVariant::Invalid );
    TQSqlField( const TQSqlField& other );
    TQSqlField& operator=( const TQSqlField& other );
    bool operator==(const TQSqlField& other) const;
    virtual ~TQSqlField();

    virtual void	setValue( const TQVariant& value );
    virtual TQVariant	value() const;
    virtual void	setName( const TQString& name );
    TQString		name() const;
    virtual void	setNull();
    bool		isNull() const;
    virtual void	setReadOnly( bool readOnly );
    bool		isReadOnly() const;
    void		clear( bool nullify = true );
    TQVariant::Type	type() const;

private:
    TQString       nm;
    TQVariant      val;
    uint          ro: 1;
    uint          nul: 1;
    TQSqlFieldPrivate* d;
};

inline TQVariant TQSqlField::value() const
{ return val; }

inline TQString TQSqlField::name() const
{ return nm; }

inline bool TQSqlField::isNull() const
{ return nul; }

inline bool TQSqlField::isReadOnly() const
{ return ro; }

inline TQVariant::Type TQSqlField::type() const
{ return d->type; }


/******************************************/
/*******     TQSqlFieldInfo Class     ******/
/******************************************/

struct TQSqlFieldInfoPrivate;

class TQM_EXPORT_SQL TQSqlFieldInfo
{
public:
    TQSqlFieldInfo( const TQString& name = TQString::null,
		   TQVariant::Type typ = TQVariant::Invalid,
		   int required = -1,
		   int len = -1,
		   int prec = -1,
		   const TQVariant& defValue = TQVariant(),
		   int sqlType = 0,
		   bool generated = true,
		   bool trim = false,
		   bool calculated = false );
    TQSqlFieldInfo( const TQSqlFieldInfo & other );
    TQSqlFieldInfo( const TQSqlField & other, bool generated = true );
    virtual ~TQSqlFieldInfo();
    TQSqlFieldInfo& operator=( const TQSqlFieldInfo& other );
    bool operator==( const TQSqlFieldInfo& f ) const;

    TQSqlField		toField() const;
    int			isRequired() const;
    TQVariant::Type	type() const;
    int			length() const;
    int			precision() const;
    TQVariant		defaultValue() const;
    TQString		name() const;
    int			typeID() const;
    bool		isGenerated() const;
    bool		isTrim() const;
    bool		isCalculated() const;

    virtual void	setTrim( bool trim );
    virtual void	setGenerated( bool gen );
    virtual void	setCalculated( bool calc );

private:
    TQSqlFieldInfoPrivate* d;
};


#endif	// TQT_NO_SQL
#endif
