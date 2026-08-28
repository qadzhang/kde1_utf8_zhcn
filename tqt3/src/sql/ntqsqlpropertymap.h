/****************************************************************************
**
** Definition of TQSqlPropertyMap class
**
** Created : 2000-11-20
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

#ifndef TQSQLPROPERTYMAP_H
#define TQSQLPROPERTYMAP_H

#ifndef QT_H
#include "ntqvariant.h"
#include "ntqstring.h"
#endif // QT_H

#ifndef TQT_NO_SQL_FORM

class TQWidget;
class TQSqlPropertyMapPrivate;

class TQ_EXPORT TQSqlPropertyMap {
public:
    TQSqlPropertyMap();
    virtual ~TQSqlPropertyMap();

    TQVariant      property( TQWidget * widget );
    virtual void  setProperty( TQWidget * widget, const TQVariant & value );

    void insert( const TQString & classname, const TQString & property );
    void remove( const TQString & classname );

    static TQSqlPropertyMap * defaultMap();
    static void installDefaultMap( TQSqlPropertyMap * map );

private:	// Disabled copy constructor and operator=
#if defined(TQ_DISABLE_COPY)
    TQSqlPropertyMap( const TQSqlPropertyMap & );
    TQSqlPropertyMap &operator=( const TQSqlPropertyMap & );
#endif
    TQSqlPropertyMapPrivate* d;

};

#endif // TQT_NO_SQL_FORM
#endif // TQSQLPROPERTYMAP_H
