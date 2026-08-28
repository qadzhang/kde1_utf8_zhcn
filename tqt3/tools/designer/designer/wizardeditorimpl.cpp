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

#include "wizardeditorimpl.h"
#include "formwindow.h"
#include "mainwindow.h"
#include "command.h"
#include "listboxdnd.h"
#include "listboxrename.h"

#include <ntqwizard.h>
#include <ntqlistbox.h>
#include <ntqpushbutton.h>
#include <ntqinputdialog.h>

WizardEditor::WizardEditor( TQWidget *parent, TQWizard *w, FormWindow *fw )
    : WizardEditorBase( parent, 0 ), formwindow( fw ), wizard( w ), draggedItem( 0 )
{
    connect( buttonHelp, TQ_SIGNAL( clicked() ), MainWindow::self, TQ_SLOT( showDialogHelp() ) );
    fillListBox();

    // Add drag and drop
    ListBoxDnd *listBoxDnd = new ListBoxDnd( listBox );
    listBoxDnd->setDragMode( ListBoxDnd::Internal | ListBoxDnd::Move );
    TQObject::connect( listBoxDnd, TQ_SIGNAL( dropped( TQListBoxItem * ) ),
		      listBoxDnd, TQ_SLOT( confirmDrop( TQListBoxItem * ) ) );

    TQObject::connect( listBoxDnd, TQ_SIGNAL( dragged( TQListBoxItem * ) ),
		      this, TQ_SLOT( itemDragged( TQListBoxItem * ) ) );
    TQObject::connect( listBoxDnd, TQ_SIGNAL( dropped( TQListBoxItem * ) ),
		      this, TQ_SLOT( itemDropped( TQListBoxItem * ) ) );

    // Add in-place rename
    new ListBoxRename( listBox );
}

WizardEditor::~WizardEditor()
{
    commands.setAutoDelete( true );
}

void WizardEditor::okClicked()
{
    applyClicked();
    accept();
}

void WizardEditor::cancelClicked()
{
    reject();
}

void WizardEditor::applyClicked()
{
    if ( commands.isEmpty() ) return;

    // schedule macro command
    MacroCommand* cmd = new MacroCommand( tr( "Edit Wizard Pages" ), formwindow, commands );
    formwindow->commandHistory()->addCommand( cmd );
    cmd->execute();

    // clear command list
    commands.clear();

    // fix wizard buttons
    for ( int i = 0; i < wizard->pageCount(); i++ ) {

	TQWidget * page = wizard->page( i );
	if ( i == 0 ) { // first page

	    wizard->setBackEnabled( page, false );
	    wizard->setNextEnabled( page, true );
	}
	else if ( i == wizard->pageCount() - 1 ) { // last page

	    wizard->setBackEnabled( page, true );
	    wizard->setNextEnabled( page, false );
	}
	else {

	    wizard->setBackEnabled( page, true );
	    wizard->setNextEnabled( page, true );
	}
	wizard->setFinishEnabled( page, false );
    }

    // update listbox
    int index = listBox->currentItem();
    fillListBox();
    listBox->setCurrentItem( index );

    // show current page
    wizard->showPage( wizard->page( 0 ) );
}

void WizardEditor::helpClicked()
{

}

void WizardEditor::addClicked()
{
    int index = listBox->currentItem() + 1;
    // update listbox
    listBox->insertItem( "Page", index );

    // schedule add command
    AddWizardPageCommand *cmd = new AddWizardPageCommand( tr( "Add Page to %1" ).arg( wizard->name() ),
							  formwindow, wizard, "Page", index, false);
    commands.append( cmd );

    // update buttons
    updateButtons();
}

void WizardEditor::removeClicked()
{
    if ( listBox->count() < 2 ) return;

    int index = listBox->currentItem();

    // update listbox
    listBox->removeItem( index );

    // schedule remove command
    DeleteWizardPageCommand *cmd = new DeleteWizardPageCommand( tr( "Delete Page %1 of %2" )
								.arg( listBox->text( index ) ).arg( wizard->name() ),
								formwindow, wizard, index, false );
    commands.append( cmd );

    // update buttons
    updateButtons();
}

void WizardEditor::upClicked()
{
    int index1 = listBox->currentItem();
    int index2 = index1 - 1;

    // swap listbox items
    TQString item1 = listBox->text( index1 );
    listBox->removeItem( index1 );
    listBox->insertItem( item1, index2 );
    listBox->setCurrentItem( index2 );

    // schedule swap command
    SwapWizardPagesCommand *cmd = new SwapWizardPagesCommand( tr( "Swap pages %1 and %2 of %1" ).arg( index1 ).arg( index2 )
							     .arg( wizard->name() ), formwindow, wizard, index1, index2);
    commands.append( cmd );

    // update buttons
    updateButtons();
}

void WizardEditor::downClicked()
{
    int index1 = listBox->currentItem();
    int index2 = index1 + 1;

    // swap listbox items
    TQString item1 = listBox->text( index1 );
    listBox->removeItem( index1 );
    listBox->insertItem( item1, index2 );
    listBox->setCurrentItem( index2 );

    // schedule swap command
    SwapWizardPagesCommand *cmd = new SwapWizardPagesCommand( tr( "Swap pages %1 and %2 of %1" ).arg( index1 ).arg( index2 ).arg( wizard->name() ), formwindow, wizard, index2, index1);
    commands.append( cmd );

    // update buttons
    updateButtons();
}

void WizardEditor::fillListBox()
{
    listBox->clear();

    if ( !wizard ) return;
    for ( int i = 0; i < wizard->pageCount(); i++ )
	listBox->insertItem( wizard->title( wizard->page( i ) ) );

    updateButtons();
}

void WizardEditor::itemHighlighted( int )
{
    updateButtons();
}

void WizardEditor::itemSelected( int index )
{
    if ( index < 0 ) return;
    // Called when TQt::Key_Enter was pressed.
    // ListBoxRename has renamed the list item, so we only need to rename the page to the same name.
    TQString pn( tr( "Rename page %1 of %2" ).arg( wizard->title( wizard->page( index ) ) ).arg( wizard->name() ) );
	RenameWizardPageCommand *cmd = new RenameWizardPageCommand( pn, formwindow, wizard, index, listBox->text( index ) );
	commands.append( cmd );
}

void WizardEditor::updateButtons()
{
    int index = listBox->currentItem();

    buttonUp->setEnabled( index > 0 );
    buttonDown->setEnabled( index < (int)listBox->count() - 1 );
    buttonRemove->setEnabled( index >= 0 );

    if ( listBox->count() < 2 )
	buttonRemove->setEnabled( false );
}

void WizardEditor::itemDragged( TQListBoxItem * i )
{
    // Store item index
    draggedItem = listBox->index( i );
}

void WizardEditor::itemDropped( TQListBoxItem * i )
{
    if ( draggedItem < 0 ) return;
    // The reorder the pages acording to the listBox list of items
    // Assumes that only one item has been moved.
    int droppedItem = listBox->index( i );

    //tqDebug( "Moving page %d -> %d", draggedItem, droppedItem );
    MoveWizardPageCommand *cmd = new MoveWizardPageCommand( tr( "Move page %1 to %2 in %3" ).arg( draggedItem ).arg( droppedItem ).arg( wizard->name() ), formwindow, wizard, draggedItem, droppedItem );
    commands.append( cmd );
}
