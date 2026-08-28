/**********************************************************************
** Copyright (C) 2005-2008 Trolltech ASA.  All rights reserved.
**
** This file is part of TQt Designer.
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
** Licensees holding valid TQt Commercial licenses may use this file in
** accordance with the TQt Commercial License Agreement provided with
** the Software.
**
** This file is provided "AS IS" with NO WARRANTY OF ANY KIND,
** INCLUDING THE WARRANTIES OF DESIGN, MERCHANTABILITY AND FITNESS FOR
** A PARTICULAR PURPOSE. Trolltech reserves all rights not granted
** herein.
**
**********************************************************************/

#if defined(DESIGNER)
#include "database.h"
#else
#include "database2.h"
#endif

#ifndef TQT_NO_SQL

#if defined(DESIGNER)
#include "formwindow.h"
#include "mainwindow.h"
#endif

#include <ntqsqldatabase.h>
#include <ntqsqlform.h>
#include <ntqsqlcursor.h>
#include <ntqsqlrecord.h>

DatabaseSupport::DatabaseSupport()
{
    con = 0;
    frm = 0;
    parent = 0;
}

void DatabaseSupport::initPreview( const TQString &connection, const TQString &table, TQObject *o,
				   const TQMap<TQString, TQString> &databaseControls )
{
    tbl = table;
    dbControls = databaseControls;
    parent = o;

    if ( connection != "(default)" )
	con = TQSqlDatabase::database( connection );
    else
	con = TQSqlDatabase::database();
    frm = new TQSqlForm( o, table );
    for ( TQMap<TQString, TQString>::Iterator it = dbControls.begin(); it != dbControls.end(); ++it ) {
	TQObject *chld = parent->child( it.key(), "TQWidget" );
	if ( !chld )
	    continue;
	frm->insert( (TQWidget*)chld, *it );
    }
}

TQDesignerDataBrowser::TQDesignerDataBrowser( TQWidget *parent, const char *name )
    : TQDataBrowser( parent, name )
{
}

bool TQDesignerDataBrowser::event( TQEvent* e )
{
    bool b = TQDataBrowser::event( e );
#if defined(DESIGNER)
    if ( MainWindow::self->isPreviewing() ) {
#endif
	if ( e->type() == TQEvent::Show ) {
	    if ( con ) {
		TQSqlCursor* cursor = new TQSqlCursor( tbl, true, con );
		setSqlCursor( cursor, true );
		setForm( frm );
		refresh();
		first();
	    }
	    return true;
	}
#if defined(DESIGNER)
    }
#endif
    return b;
}

TQDesignerDataView::TQDesignerDataView( TQWidget *parent, const char *name )
    : TQDataView( parent, name )
{
}

bool TQDesignerDataView::event( TQEvent* e )
{
    bool b = TQDataView::event( e );
#if defined(DESIGNER)
    if ( MainWindow::self->isPreviewing() ) {
#endif
	if ( e->type() == TQEvent::Show ) {
	    setForm( frm );
	    readFields();
	    return true;
	}
#if defined(DESIGNER)
    }
#endif
    return b;
}


#endif
