/****************************************************************************
**
** Definition of TQSqlRecord class
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

#ifndef TQSQLRECORD_H
#define TQSQLRECORD_H

#ifndef QT_H
#include "ntqstring.h"
#include "ntqstringlist.h"
#include "ntqvariant.h"
#include "ntqsqlfield.h"
#endif // QT_H

#ifndef TQT_NO_SQL

class TQSqlRecordPrivate;

class TQ_EXPORT TQSqlRecordShared : public TQShared
{
public:
    TQSqlRecordShared( TQSqlRecordPrivate* sqlRecordPrivate )
    : d( sqlRecordPrivate )
    {}
    virtual ~TQSqlRecordShared();
    TQSqlRecordPrivate* d;
};

class TQ_EXPORT TQSqlRecord
{
public:
    TQSqlRecord();
    TQSqlRecord( const TQSqlRecord& other );
    TQSqlRecord& operator=( const TQSqlRecord& other );
    virtual ~TQSqlRecord();
    virtual TQVariant     value( int i ) const;
    virtual TQVariant     value( const TQString& name ) const;
    virtual void         setValue( int i, const TQVariant& val );
    virtual void         setValue( const TQString& name, const TQVariant& val );
    bool                 isGenerated( int i ) const;
    bool                 isGenerated( const TQString& name ) const;
    virtual void         setGenerated( const TQString& name, bool generated );
    virtual void         setGenerated( int i, bool generated );
    virtual void         setNull( int i );
    virtual void         setNull( const TQString& name );
    bool                 isNull( int i ); // remove in 4.0
    bool                 isNull( const TQString& name ); // remove in 4.0
    bool                 isNull( int i ) const;
    bool                 isNull( const TQString& name ) const;

    int                  position( const TQString& name ) const;
    TQString              fieldName( int i ) const;
    TQSqlField*           field( int i );
    TQSqlField*           field( const TQString& name );
    const TQSqlField*     field( int i ) const;
    const TQSqlField*     field( const TQString& name ) const;

    virtual void         append( const TQSqlField& field );
    virtual void         insert( int pos, const TQSqlField& field );
    virtual void         remove( int pos );

    bool                 isEmpty() const;
    bool                 contains( const TQString& name ) const;
    virtual void         clear();
    virtual void         clearValues( bool nullify = false );
    uint                 count() const;
    virtual TQString      toString( const TQString& prefix = TQString::null,
				   const TQString& sep = "," ) const;
    virtual TQStringList  toStringList( const TQString& prefix = TQString::null ) const;

private:
    TQString              createField( int i, const TQString& prefix ) const;
    void                 deref();
    bool                 checkDetach();
    TQSqlRecordShared*    sh;
};

/******************************************/
/*******     TQSqlRecordInfo Class    ******/
/******************************************/

#if defined(Q_TEMPLATEDLL)
// MOC_SKIP_BEGIN
Q_TEMPLATE_EXTERN template class TQ_EXPORT TQValueList<TQSqlFieldInfo>;
// MOC_SKIP_END
#endif

typedef TQValueList<TQSqlFieldInfo> TQSqlFieldInfoList;

class TQ_EXPORT TQSqlRecordInfo: public TQSqlFieldInfoList
{
public:
    TQSqlRecordInfo(): TQSqlFieldInfoList() {}
    TQSqlRecordInfo( const TQSqlFieldInfoList& other ): TQSqlFieldInfoList( other ) {}
    TQSqlRecordInfo( const TQSqlRecord& other );

    size_type contains( const TQString& fieldName ) const;
    TQSqlFieldInfo find( const TQString& fieldName ) const;
    TQSqlRecord toRecord() const;

};


#endif	// TQT_NO_SQL
#endif
