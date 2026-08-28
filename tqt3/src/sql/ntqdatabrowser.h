/****************************************************************************
**
** Definition of TQDataBrowser class
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

#ifndef TQDATABROWSER_H
#define TQDATABROWSER_H

#ifndef QT_H
#include "ntqwidget.h"
#include "ntqstring.h"
#include "ntqstringlist.h"
#include "ntqsql.h"
#include "ntqsqlindex.h"
#include "ntqsqlcursor.h"
#include "ntqsqlerror.h"
#endif // QT_H

#ifndef TQT_NO_SQL_VIEW_WIDGETS

class TQSqlForm;
class TQDataBrowserPrivate;

class TQ_EXPORT TQDataBrowser : public TQWidget
{
    TQ_OBJECT
    TQ_PROPERTY( bool boundaryChecking READ boundaryChecking WRITE setBoundaryChecking )
    TQ_PROPERTY( TQString filter READ filter WRITE setFilter )
    TQ_PROPERTY( TQStringList sort READ sort WRITE setSort )
    TQ_PROPERTY( bool confirmEdits READ confirmEdits WRITE setConfirmEdits )
    TQ_PROPERTY( bool confirmInsert READ confirmInsert WRITE setConfirmInsert )
    TQ_PROPERTY( bool confirmUpdate READ confirmUpdate WRITE setConfirmUpdate )
    TQ_PROPERTY( bool confirmDelete READ confirmDelete WRITE setConfirmDelete )
    TQ_PROPERTY( bool confirmCancels READ confirmCancels WRITE setConfirmCancels )
    TQ_PROPERTY( bool readOnly READ isReadOnly WRITE setReadOnly )
    TQ_PROPERTY( bool autoEdit READ autoEdit WRITE setAutoEdit )

public:
    TQDataBrowser( TQWidget* parent=0, const char* name=0, WFlags fl = 0 );
    ~TQDataBrowser();

    enum Boundary {
	Unknown,
	None,
	BeforeBeginning,
	Beginning,
	End,
	AfterEnd
    };

    Boundary boundary();
    void setBoundaryChecking( bool active );
    bool boundaryChecking() const;

    void setSort( const TQSqlIndex& sort );
    void setSort( const TQStringList& sort );
    TQStringList  sort() const;
    void setFilter( const TQString& filter );
    TQString filter() const;
    virtual void setSqlCursor( TQSqlCursor* cursor, bool autoDelete = false );
    TQSqlCursor* sqlCursor() const;
    virtual void setForm( TQSqlForm* form );
    TQSqlForm* form();

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

    virtual void setReadOnly( bool active );
    bool isReadOnly() const;
    virtual void setAutoEdit( bool autoEdit );
    bool autoEdit() const;

    virtual bool seek( int i, bool relative = false );

signals:
    void firstRecordAvailable( bool available );
    void lastRecordAvailable( bool available );
    void nextRecordAvailable( bool available );
    void prevRecordAvailable( bool available );

    void currentChanged( const TQSqlRecord* record );
    void primeInsert( TQSqlRecord* buf );
    void primeUpdate( TQSqlRecord* buf );
    void primeDelete( TQSqlRecord* buf );
    void beforeInsert( TQSqlRecord* buf );
    void beforeUpdate( TQSqlRecord* buf );
    void beforeDelete( TQSqlRecord* buf );
    void cursorChanged( TQSqlCursor::Mode mode );

public slots:
    virtual void refresh();

    virtual void insert();
    virtual void update();
    virtual void del();

    virtual void first();
    virtual void last();
    virtual void next();
    virtual void prev();

    virtual void readFields();
    virtual void writeFields();
    virtual void clearValues();

    void updateBoundary();

protected:
    virtual bool insertCurrent();
    virtual bool updateCurrent();
    virtual bool deleteCurrent();
    virtual bool currentEdited();

    virtual TQSql::Confirm confirmEdit( TQSql::Op m );
    virtual TQSql::Confirm confirmCancel( TQSql::Op m );

    virtual void handleError( const TQSqlError& error );

private:
    typedef bool (TQSqlCursor::*Nav)();
    bool preNav();
    void postNav( bool primeUpd );
    void nav( Nav nav );
    TQDataBrowserPrivate* d;

#if defined(TQ_DISABLE_COPY) // Disabled copy constructor and operator=
    TQDataBrowser( const TQDataBrowser & );
    TQDataBrowser &operator=( const TQDataBrowser & );
#endif
};


#endif
#endif
