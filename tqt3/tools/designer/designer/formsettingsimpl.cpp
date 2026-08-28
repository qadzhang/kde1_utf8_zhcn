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

#include "formsettingsimpl.h"
#include "formwindow.h"
#include "metadatabase.h"
#include "command.h"
#include "asciivalidator.h"
#include "mainwindow.h"
#include "project.h"

#include <ntqmultilineedit.h>
#include <ntqpushbutton.h>
#include <ntqcombobox.h>
#include <ntqradiobutton.h>
#include <ntqlineedit.h>
#include <ntqspinbox.h>
#include <ntqcheckbox.h>

FormSettings::FormSettings( TQWidget *parent, FormWindow *fw )
    : FormSettingsBase( parent, 0, true ), formwindow( fw )
{
    connect( buttonHelp, TQ_SIGNAL( clicked() ), MainWindow::self, TQ_SLOT( showDialogHelp() ) );
    MetaDataBase::MetaInfo info = MetaDataBase::metaInfo( fw );
    if ( info.classNameChanged && !info.className.isEmpty() )
	editClassName->setText( info.className );
    else
	editClassName->setText( fw->name() );
    editComment->setText( info.comment );
    editAuthor->setText( info.author );

    editClassName->setValidator( new AsciiValidator( TQString( ":" ), editClassName ) );
    editPixmapFunction->setValidator( new AsciiValidator( TQString( ":" ), editPixmapFunction ) );

    if ( formwindow->savePixmapInline() )
	radioPixmapInline->setChecked( true );
    else if ( formwindow->savePixmapInProject() )
	radioProjectImageFile->setChecked( true );
    else
	radioPixmapFunction->setChecked( true );
    editPixmapFunction->setText( formwindow->pixmapLoaderFunction() );
    radioProjectImageFile->setEnabled( !fw->project()->isDummy() );
    spinSpacing->setValue( formwindow->layoutDefaultSpacing() );
    spinMargin->setValue( formwindow->layoutDefaultMargin() );
    editSpacingFunction->setValidator( new AsciiValidator( TQString( ":" ), editSpacingFunction ) );
    editMarginFunction->setValidator( new AsciiValidator( TQString( ":" ), editMarginFunction ) ); 
    checkLayoutFunctions->setChecked( formwindow->hasLayoutFunctions() );
    editSpacingFunction->setText( formwindow->spacingFunction() );
    editMarginFunction->setText( formwindow->marginFunction() );
}

void FormSettings::okClicked()
{
    MetaDataBase::MetaInfo info;
    info.className = editClassName->text();
    info.classNameChanged = info.className != TQString( formwindow->name() );
    info.comment = editComment->text();
    info.author = editAuthor->text();
    MetaDataBase::setMetaInfo( formwindow, info );

    formwindow->commandHistory()->setModified( true );

    if ( formwindow->savePixmapInline() ) {
	MetaDataBase::clearPixmapArguments( formwindow );
	MetaDataBase::clearPixmapKeys( formwindow );
    } else if ( formwindow->savePixmapInProject() ) {
	MetaDataBase::clearPixmapArguments( formwindow );
    } else {
	MetaDataBase::clearPixmapKeys( formwindow );
    }

    if ( radioPixmapInline->isChecked() ) {
	formwindow->setSavePixmapInline( true );
	formwindow->setSavePixmapInProject( false );
    } else if ( radioProjectImageFile->isChecked() ){
	formwindow->setSavePixmapInline( false );
	formwindow->setSavePixmapInProject( true );
    } else {
	formwindow->setSavePixmapInline( false );
	formwindow->setSavePixmapInProject( false );
    }
    
    if ( checkLayoutFunctions->isChecked() )
	formwindow->hasLayoutFunctions( true );
    else
	formwindow->hasLayoutFunctions( false );

    formwindow->setPixmapLoaderFunction( editPixmapFunction->text() );
    formwindow->setLayoutDefaultSpacing( spinSpacing->value() );
    formwindow->setSpacingFunction( editSpacingFunction->text() );
    formwindow->setLayoutDefaultMargin( spinMargin->value() );
    formwindow->setMarginFunction( editMarginFunction->text() );

    accept();
}
