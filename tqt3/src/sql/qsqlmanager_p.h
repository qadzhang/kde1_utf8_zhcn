/****************************************************************************
**
** Definition of TQSqlManager class
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

#ifndef TQSQLMANAGER_P_H
#define TQSQLMANAGER_P_H

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
#include "ntqglobal.h"
#include "ntqstring.h"
#include "ntqstringlist.h"
#include "ntqsql.h"
#include "ntqsqlerror.h"
#include "ntqsqlindex.h"
#include "ntqsqlcursor.h"
#endif // QT_H

#if !defined( TQT_MODULE_SQL ) || defined( QT_LICENSE_PROFESSIONAL )
#define TQM_EXPORT_SQL
#else
#define TQM_EXPORT_SQL TQ_EXPORT
#endif

#ifndef TQT_NO_SQL

class TQSqlCursor;
class TQSqlForm;
class TQSqlCursorManagerPrivate;

class TQM_EXPORT_SQL TQSqlCursorManager
{
public:
    TQSqlCursorManager();
    virtual ~TQSqlCursorManager();

    virtual void setSort( const TQSqlIndex& sort );
    virtual void setSort( const TQStringList& sort );
    TQStringList  sort() const;
    virtual void setFilter( const TQString& filter );
    TQString filter() const;
    virtual void setCursor( TQSqlCursor* cursor, bool autoDelete = false );
    TQSqlCursor* cursor() const;

    virtual void setAutoDelete( bool enable );
    bool autoDelete() const;

    virtual bool refresh();
    virtual bool findBuffer( const TQSqlIndex& idx, int atHint = 0 );

private:
    TQSqlCursorManagerPrivate* d;
};

#ifndef TQT_NO_SQL_FORM

class TQSqlFormManagerPrivate;

class TQM_EXPORT_SQL TQSqlFormManager
{
public:
    TQSqlFormManager();
    virtual ~TQSqlFormManager();

    virtual void setForm( TQSqlForm* form );
    TQSqlForm* form();
    virtual void setRecord( TQSqlRecord* record );
    TQSqlRecord* record();

    virtual void clearValues();
    virtual void readFields();
    virtual void writeFields();

private:
    TQSqlFormManagerPrivate* d;
};

#endif

class TQWidget;
class TQDataManagerPrivate;

class TQM_EXPORT_SQL TQDataManager
{
public:
    TQDataManager();
    virtual ~TQDataManager();

    virtual void setMode( TQSql::Op m );
    TQSql::Op mode() const;
    virtual void setAutoEdit( bool autoEdit );
    bool autoEdit() const;

    virtual void handleError( TQWidget* parent, const TQSqlError& error );
    virtual TQSql::Confirm confirmEdit( TQWidget* parent, TQSql::Op m );
    virtual TQSql::Confirm confirmCancel( TQWidget* parent, TQSql::Op m );

    virtual void setConfirmEdits( bool confirm );
    virtual void setConfirmInsert( bool confirm );
    virtual void setConfirmUpdate( bool confirm );
    virtual void setConfirmDelete( bool confirm );
    virtual void setConfirmCancels( bool confirm );

    bool confirmEdits() const;
    bool confirmInsert() const;
    bool confirmUpdate() const;
    bool confirmDelete() const;
    bool confirmCancels() const;

private:
    TQDataManagerPrivate* d;
};


#endif
#endif
