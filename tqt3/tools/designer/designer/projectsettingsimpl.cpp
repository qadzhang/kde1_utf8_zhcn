/**********************************************************************
** Copyright (C) 2000-2008 Trolltech ASA.  All rights reserved.
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

#include "projectsettingsimpl.h"
#include "project.h"
#include "formwindow.h"
#include "metadatabase.h"
#include "mainwindow.h"
#include "asciivalidator.h"
#include "mainwindow.h"
#include "sourcefile.h"
#include "workspace.h"

#include <ntqlineedit.h>
#include <ntqtextedit.h>
#include <ntqcheckbox.h>
#include <ntqfiledialog.h>
#include <ntqcombobox.h>
#include <ntqobjectlist.h>
#include <ntqheader.h>
#include <ntqpushbutton.h>
#include <ntqlabel.h>

/*
 *  Constructs a ProjectSettings which is a child of 'parent', with the
 *  name 'name' and widget flags set to 'f'
 *
 *  The dialog will by default be modeless, unless you set 'modal' to
 *  true to construct a modal dialog.
 */
ProjectSettings::ProjectSettings( Project *pro, TQWidget* parent,  const char* name, bool modal, WFlags fl )
    : ProjectSettingsBase( parent, name, modal, fl ), project( pro )
{
    connect( buttonHelp, TQ_SIGNAL( clicked() ), MainWindow::self, TQ_SLOT( showDialogHelp() ) );

    editProjectFile->setFocus();

    if ( project->isDummy() ) {
	editProjectFile->setEnabled( false );
	editProjectFile->setText( project->projectName() );
    } else {
	if ( project->fileName().isEmpty() || project->fileName() == ".pro" ) {
	    editProjectFile->setText( tr( "unnamed.pro" ) );
	    editProjectFile->selectAll();
	} else {
	    editProjectFile->setText( project->fileName() );
	}
    }

    editDatabaseFile->setText( project->databaseDescription() );

    comboLanguage->insertStringList( MetaDataBase::languages() );
    for ( int j = 0; j < (int)comboLanguage->count(); ++j ) {
	if ( project->language() == comboLanguage->text( j ) ) {
	    comboLanguage->setCurrentItem( j );
	    break;
	}
    }
}

/*
 *  Destroys the object and frees any allocated resources
 */
ProjectSettings::~ProjectSettings()
{
}

void ProjectSettings::chooseDatabaseFile()
{
    TQString fn = TQFileDialog::getSaveFileName(editDatabaseFile->text(), tr("Database Files (*.db);;All Files (*)"), 
					      this);
    if ( fn.isEmpty() )
	return;
    editDatabaseFile->setText( fn );
}

void ProjectSettings::chooseProjectFile()
{
    TQString fn = TQFileDialog::getSaveFileName(editProjectFile->text(), tr("Project Files (*.pro);;All Files (*)"), 
					      this);
    if ( fn.isEmpty() )
	return;
    editProjectFile->setText( fn );
}

void ProjectSettings::helpClicked()
{
}

void ProjectSettings::okClicked()
{
    // ### check for validity
    project->setFileName( editProjectFile->text(), false );
    project->setDatabaseDescription( editDatabaseFile->text() );
    project->setLanguage( comboLanguage->text( comboLanguage->currentItem() ) );
    project->setModified( true );
    accept();
}

void ProjectSettings::languageChanged( const TQString & )
{
}
