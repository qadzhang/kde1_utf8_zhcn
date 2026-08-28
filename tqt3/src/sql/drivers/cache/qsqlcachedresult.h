/****************************************************************************
**
** Definition of shared TQt SQL module classes
**
** Copyright (C) 1992-2008 Trolltech ASA.  All rights reserved.
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

#ifndef TQSQLCACHEDRESULT_H
#define TQSQLCACHEDRESULT_H

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

#include <ntqglobal.h>
#include <ntqvariant.h>
#include <ntqptrvector.h>
#include <ntqvaluevector.h>
#include <ntqsqlresult.h>

#if !defined( TQT_MODULE_SQL ) || defined( QT_LICENSE_PROFESSIONAL )
#define TQM_EXPORT_SQL
#else
#define TQM_EXPORT_SQL TQ_EXPORT
#endif

#ifndef TQT_NO_SQL

class TQtSqlCachedResultPrivate;

class TQM_EXPORT_SQL TQtSqlCachedResult: public TQSqlResult
{
public:
    virtual ~TQtSqlCachedResult();

    typedef TQValueVector<TQVariant> RowCache;
    typedef TQPtrVector<RowCache> RowsetCache;

protected:    
    TQtSqlCachedResult(const TQSqlDriver * db);

    void init(int colCount);
    void cleanup();
    bool cacheNext();

    virtual bool gotoNext(RowCache* row) = 0;

    TQVariant data(int i);
    bool isNull(int i);
    bool fetch(int i);
    bool fetchNext();
    bool fetchPrev();
    bool fetchFirst();
    bool fetchLast();

    int colCount() const;

private:
    TQtSqlCachedResultPrivate *d;
};


#endif

#endif
