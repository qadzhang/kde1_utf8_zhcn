/****************************************************************************
**
** Definition of TQSqlIndex class
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

#ifndef TQSQLINDEX_H
#define TQSQLINDEX_H

#ifndef QT_H
#include "ntqstring.h"
#include "ntqstringlist.h"
#include "ntqsqlfield.h"
#include "ntqsqlrecord.h"
#endif // QT_H

#if !defined( TQT_MODULE_SQL ) || defined( QT_LICENSE_PROFESSIONAL )
#define TQM_EXPORT_SQL
#define TQM_TEMPLATE_EXTERN_SQL
#else
#define TQM_EXPORT_SQL TQ_EXPORT
#define TQM_TEMPLATE_EXTERN_SQL Q_TEMPLATE_EXTERN
#endif

#ifndef TQT_NO_SQL

class TQSqlCursor;

class TQM_EXPORT_SQL TQSqlIndex : public TQSqlRecord
{
public:
    TQSqlIndex( const TQString& cursorName = TQString::null, const TQString& name = TQString::null );
    TQSqlIndex( const TQSqlIndex& other );
    ~TQSqlIndex();
    TQSqlIndex&       operator=( const TQSqlIndex& other );
    virtual void     setCursorName( const TQString& cursorName );
    TQString          cursorName() const { return cursor; }
    virtual void     setName( const TQString& name );
    TQString          name() const { return nm; }

    void             append( const TQSqlField& field );
    virtual void     append( const TQSqlField& field, bool desc );

    bool             isDescending( int i ) const;
    virtual void     setDescending( int i, bool desc );

    TQString          toString( const TQString& prefix = TQString::null,
			       const TQString& sep = ",",
			       bool verbose = true ) const;
    TQStringList      toStringList( const TQString& prefix = TQString::null,
				   bool verbose = true ) const;

    static TQSqlIndex fromStringList( const TQStringList& l, const TQSqlCursor* cursor );

private:
    TQString          createField( int i, const TQString& prefix, bool verbose ) const;
    TQString          cursor;
    TQString          nm;
    TQValueList<bool> sorts;
};

#define Q_DEFINED_QSQLINDEX
#include "ntqwinexport.h"
#endif	// TQT_NO_SQL
#endif
