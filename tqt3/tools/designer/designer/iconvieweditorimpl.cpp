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

#include "iconvieweditorimpl.h"
#include "formwindow.h"
#include "mainwindow.h"
#include "pixmapchooser.h"

#include <ntqlineedit.h>
#include <ntqlabel.h>
#include <ntqiconview.h>
#include <ntqpushbutton.h>
#include <ntqcheckbox.h>

IconViewEditor::IconViewEditor( TQWidget *parent, TQWidget *editWidget, FormWindow *fw )
    : IconViewEditorBase( parent, 0, true ), formwindow( fw )
{
    connect( buttonHelp, TQ_SIGNAL( clicked() ), MainWindow::self, TQ_SLOT( showDialogHelp() ) );
    iconview = (TQIconView*)editWidget;

    itemText->setText( "" );
    itemText->setEnabled( false );
    itemPixmap->setText( "" );
    itemChoosePixmap->setEnabled( false );
    itemDeletePixmap->setEnabled( false );

    TQIconViewItem *i = 0;
    for ( i = iconview->firstItem(); i; i = i->nextItem() ) {
	(void)new TQIconViewItem( preview, i->text(), *i->pixmap()  );
    }

    if ( preview->firstItem() )
	preview->setCurrentItem( preview->firstItem() );
}

void IconViewEditor::insertNewItem()
{
    TQIconViewItem *i = new TQIconViewItem( preview, tr( "New Item" ) );
    preview->setCurrentItem( i );
    preview->setSelected( i, true );
    itemText->setFocus();
    itemText->selectAll();
}

void IconViewEditor::deleteCurrentItem()
{
    delete preview->currentItem();
    if ( preview->currentItem() )
	preview->setSelected( preview->currentItem(), true );
}

void IconViewEditor::currentItemChanged( TQIconViewItem *i )
{
    itemText->blockSignals( true );
    itemText->setText( "" );
    itemPixmap->setText( "" );
    itemText->blockSignals( false );

    if ( !i ) {
	itemText->setEnabled( false );
	itemChoosePixmap->setEnabled( false );
	return;
    }

    itemText->blockSignals( true );
    itemText->setEnabled( true );
    itemChoosePixmap->setEnabled( true );
    itemDeletePixmap->setEnabled( i->pixmap() && !i->pixmap()->isNull() );

    itemText->setText( i->text() );
    if ( i->pixmap() )
	itemPixmap->setPixmap( *i->pixmap() );
    itemText->blockSignals( false );
}

void IconViewEditor::currentTextChanged( const TQString &txt )
{
    if ( !preview->currentItem() )
	return;

    preview->currentItem()->setText( txt );
}

void IconViewEditor::okClicked()
{
    applyClicked();
    accept();
}

void IconViewEditor::cancelClicked()
{
    reject();
}

void IconViewEditor::applyClicked()
{
    TQIconViewItem *i = 0;
    TQValueList<PopulateIconViewCommand::Item> items;
    for ( i = preview->firstItem(); i; i = i->nextItem() ) {
	PopulateIconViewCommand::Item item;
	if ( i->pixmap() )
	    item.pix = *i->pixmap();
	item.text = i->text();
	items.append( item );
    }

    PopulateIconViewCommand *cmd = new PopulateIconViewCommand( tr( "Edit the Items of '%1'" ).arg( iconview->name() ),
								formwindow, iconview, items );
    cmd->execute();
    formwindow->commandHistory()->addCommand( cmd );
}

void IconViewEditor::choosePixmap()
{
    if ( !preview->currentItem() )
	return;

    TQPixmap pix;
    if ( preview->currentItem()->pixmap() )
    	pix = qChoosePixmap( this, formwindow, *preview->currentItem()->pixmap() );
    else
    	pix = qChoosePixmap( this, formwindow, TQPixmap() );

    if ( pix.isNull() )
	return;

    preview->currentItem()->setPixmap( pix );
    itemPixmap->setPixmap( pix );
    itemDeletePixmap->setEnabled( true );
}

void IconViewEditor::deletePixmap()
{
    if ( !preview->currentItem() )
	return;

    preview->currentItem()->setPixmap( TQPixmap() );
    itemPixmap->setText( "" );
    itemDeletePixmap->setEnabled( false );
}
