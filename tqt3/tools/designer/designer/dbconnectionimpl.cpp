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

#include "dbconnectionimpl.h"
#include "dbconnection.h"
#include "project.h"
#include "asciivalidator.h"
#include <ntqgroupbox.h>
#include <ntqcombobox.h>
#include <ntqspinbox.h>
#include <ntqlineedit.h>
#include <ntqlayout.h>
#include <ntqsqldatabase.h>

DatabaseConnectionEditor::DatabaseConnectionEditor( DatabaseConnection* connection, TQWidget* parent,
						    const char* name, bool modal, WFlags fl )
    : DatabaseConnectionEditorBase( parent, name, modal, fl ), conn( connection )
{
    connectionWidget = new DatabaseConnectionWidget( grp );
    grpLayout->addWidget( connectionWidget, 0, 0 );
    init();
}

DatabaseConnectionEditor::~DatabaseConnectionEditor()
{
}

void DatabaseConnectionEditor::accept()
{
#ifndef TQT_NO_SQL
    conn->setUsername( connectionWidget->editUsername->text() );
    conn->setPassword( connectionWidget->editPassword->text() );
    conn->setHostname( connectionWidget->editHostname->text() );
#endif
    DatabaseConnectionEditorBase::accept();
}

void DatabaseConnectionEditor::init()
{
    connectionWidget->editName->setEnabled( false );
    connectionWidget->editName->setValidator( new AsciiValidator( connectionWidget->editName ) );
    connectionWidget->editName->setText( conn->name() );
    connectionWidget->comboDriver->setEnabled( false );
    connectionWidget->comboDriver->lineEdit()->setText( conn->driver() );
    connectionWidget->editDatabase->setEnabled( false );
    connectionWidget->editDatabase->setText( conn->database() );
    connectionWidget->editUsername->setEnabled( true );
    connectionWidget->editUsername->setText( conn->username() );
    connectionWidget->editPassword->setEnabled( true );
    connectionWidget->editPassword->setText( "" );
    connectionWidget->editHostname->setEnabled( true );
    connectionWidget->editHostname->setText( conn->hostname() );
    connectionWidget->editPort->setEnabled( true );
    connectionWidget->editPort->setValue( conn->port() );
    connectionWidget->editUsername->setFocus();
    connectionWidget->editUsername->selectAll();
}
