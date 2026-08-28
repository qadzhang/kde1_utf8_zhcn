/**********************************************************************
** Copyright (C) 2000-2008 Trolltech ASA.  All rights reserved.
**
** This file is part of TQt Assistant.
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

#include "settingsdialogimpl.h"
#include "docuparser.h"
#include "config.h"

#include <ntqapplication.h>
#include <ntqpushbutton.h>
#include <ntqcheckbox.h>
#include <ntqcolordialog.h>
#include <ntqdir.h>
#include <ntqfiledialog.h>
#include <ntqfileinfo.h>
#include <ntqlineedit.h>
#include <ntqlistbox.h>
#include <ntqlistview.h>
#include <ntqmessagebox.h>
#include <ntqptrstack.h>
#include <ntqsettings.h>
#include <ntqtimer.h>
#include <ntqtoolbutton.h>
#include <ntqtabwidget.h>
#include <ntqmap.h>


SettingsDialog::SettingsDialog( TQWidget *parent, const char* name )
    : SettingsDialogBase( parent, name )
{
    init();
}

void SettingsDialog::init()
{
    Config *config = Config::configuration();

    browserApp->setText( config->webBrowser() );
    homePage->setText( config->homePage() );
    pdfApp->setText( config->pdfReader() );
}

void SettingsDialog::selectColor()
{
    TQColor c = TQColorDialog::getColor( colorButton->paletteBackgroundColor(), this );
    colorButton->setPaletteBackgroundColor( c );
}

void SettingsDialog::browseWebApp()
{
    setFile( browserApp, tr( "TQt Assistant - Set Web Browser" ) );
}

void SettingsDialog::browsePDFApplication()
{
    setFile( pdfApp, tr( "TQt Assistant - Set PDF Browser" ) );
}

void SettingsDialog::browseHomepage()
{
    setFile( homePage, tr( "TQt Assistant - Set Homepage" ) );
}

void SettingsDialog::setFile( TQLineEdit *le, const TQString &caption )
{
    TQFileDialog *fd = new TQFileDialog( this );
    fd->setCaption( caption );
    fd->setMode( TQFileDialog::AnyFile );
    fd->setDir( TQDir::homeDirPath() );

    if ( fd->exec() == TQDialog::Accepted ) {
	if ( !fd->selectedFile().isEmpty() )
	   le->setText( fd->selectedFile() );
    }
}

void SettingsDialog::accept()
{
    Config *config = Config::configuration();

    config->setWebBrowser( browserApp->text() );
    config->setHomePage( homePage->text() );
    config->setPdfReader( pdfApp->text() );

    hide();
    done( Accepted );
}

void SettingsDialog::reject()
{
    init();
    done( Rejected );
}
