/**********************************************************************
** Copyright (C) 2000-2008 Trolltech ASA.  All rights reserved.
**
** This file is part of TQt Linguist.
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

/*  TRANSLATOR FindDialog

    Choose Edit|Find from the menu bar or press Ctrl+F to pop up the 
    Find dialog
*/

#include "finddialog.h"

#include <ntqcheckbox.h>
#include <ntqlabel.h>
#include <ntqlayout.h>
#include <ntqlineedit.h>
#include <ntqpushbutton.h>
#include <ntqwhatsthis.h>

FindDialog::FindDialog( bool replace, TQWidget *parent, const char *name,
			bool modal )
    : TQDialog( parent, name, modal )
{
    sourceText = 0;
    
    led = new TQLineEdit( this, "find line edit" );
    TQLabel *findWhat = new TQLabel( led, tr("Fi&nd what:"), this, "find what" );
    TQLabel *replaceWith = 0;
    TQPushButton *findNxt = new TQPushButton( tr("&Find Next"), this,
					    "find next" );
    findNxt->setDefault( true );
    connect( findNxt, TQ_SIGNAL(clicked()), this, TQ_SLOT(emitFindNext()) );
    TQPushButton *cancel = new TQPushButton( tr("Cancel"), this, "cancel find" );
    connect( cancel, TQ_SIGNAL(clicked()), this, TQ_SLOT(reject()) );

    TQVBoxLayout *bl = new TQVBoxLayout( 6, "find button layout" );
    bl->addWidget( findNxt );

    matchCase = new TQCheckBox( tr("&Match case"), this, "find match case" );
    matchCase->setChecked( false );

    if ( replace ) {
	TQWhatsThis::add( this, tr("This window allows you to search and replace"
				  " some text in the translations.") );

	red = new TQLineEdit( this, "replace line edit" );
	replaceWith = new TQLabel( red, tr("Replace &with:"), this,
				  "replace with" );
	setTabOrder( led, red );

	TQPushButton *replace = new TQPushButton( tr("&Replace"), this,
						"replace" );
	connect( replace, TQ_SIGNAL(clicked()), this, TQ_SLOT(emitReplace()) );
	TQPushButton *replaceAll = new TQPushButton( tr("Replace &All"), this,
						   "replace all" );
	connect( replaceAll, TQ_SIGNAL(clicked()), this, TQ_SLOT(emitReplaceAll()) );
	setTabOrder( findNxt, replace );
	setTabOrder( replace, replaceAll );
	setTabOrder( replaceAll, cancel );

	TQWhatsThis::add( replace, tr("Click here to replace the next occurrence"
				     " of the text you typed in.") );
	TQWhatsThis::add( replaceAll, tr("Click here to replace all occurrences"
					" of the text you typed in.") );

	bl->addWidget( replace );
	bl->addWidget( replaceAll );

	TQGridLayout *gl = new TQGridLayout( this, 4, 3, 11, 11,
					   "find outer layout" );
	gl->addWidget( findWhat, 0, 0 );
	gl->addWidget( led, 0, 1 );
	gl->addWidget( replaceWith, 1, 0 );
	gl->addWidget( red, 1, 1 );
	gl->addMultiCellWidget( matchCase, 2, 2, 0, 1 );
	gl->addMultiCell( bl, 0, 3, 3, 3 );
    } else {
	TQWhatsThis::add( this, tr("This window allows you to search for some"
				  " text in the translation source file.") );

	sourceText = new TQCheckBox( tr("&Source texts"), this,
				    "find in source texts" );
	sourceText->setChecked( true );
	translations = new TQCheckBox( tr("&Translations"), this,
				      "find in translations" );
	translations->setChecked( true );
	comments = new TQCheckBox( tr("&Comments"), this, "find in comments" );
	comments->setChecked( true );

	TQWhatsThis::add( sourceText, tr("Source texts are searched when"
					" checked.") );
	TQWhatsThis::add( translations, tr("Translations are searched when"
					  " checked.") );
	TQWhatsThis::add( comments, tr("Comments and contexts are searched when"
				      " checked.") );

	TQVBoxLayout *cl = new TQVBoxLayout( 6, "find checkbox layout" );

	TQGridLayout *gl = new TQGridLayout( this, 3, 4, 11, 11,
					   "find outer layout" );
	gl->addWidget( findWhat, 0, 0 );
	gl->addMultiCellWidget( led, 0, 0, 1, 2 );
	gl->addWidget( matchCase, 1, 2 );
	gl->addMultiCell( bl, 0, 2, 3, 3 );
	gl->addMultiCell( cl, 1, 2, 0, 1 );
	gl->setColStretch( 0, 0 );
	gl->addColSpacing( 1, 40 );
	gl->setColStretch( 2, 1 );
	gl->setColStretch( 3, 0 );

	cl->addWidget( sourceText );
	cl->addWidget( translations );
	cl->addWidget( comments );
	cl->addStretch( 1 );
    }

    TQWhatsThis::add( led, tr("Type in the text to search for.") );

    TQWhatsThis::add( matchCase, tr("Texts such as 'TeX' and 'tex' are"
				   " considered as different when checked.") );
    TQWhatsThis::add( findNxt, tr("Click here to find the next occurrence of the"
				 " text you typed in.") );
    TQWhatsThis::add( cancel, tr("Click here to close this window.") );

    bl->addWidget( cancel );
    bl->addStretch( 1 );

    resize( 400, 1 );
    setMaximumHeight( height() );

    led->setFocus();
}

void FindDialog::emitFindNext()
{
    int where;
    if ( sourceText != 0 )
	where = ( sourceText->isChecked() ? SourceText : 0 ) |
		( translations->isChecked() ? Translations : 0 ) |
		( comments->isChecked() ? Comments : 0 );
    else
	where = Translations;
    emit findNext( led->text(), where, matchCase->isChecked() );
}

void FindDialog::emitReplace()
{
    emit replace( led->text(), red->text(), matchCase->isChecked(), false );
}

void FindDialog::emitReplaceAll()
{
    emit replace( led->text(), red->text(), matchCase->isChecked(), true );
}
