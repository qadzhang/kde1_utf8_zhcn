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

#include "editfunctionsimpl.h"
#include "formwindow.h"
#include "metadatabase.h"
#include "asciivalidator.h"
#include "mainwindow.h"
#include "hierarchyview.h"
#include "project.h"

#include <ntqlistview.h>
#include <ntqpushbutton.h>
#include <ntqlineedit.h>
#include <ntqcombobox.h>
#include <ntqstrlist.h>
#include <ntqmessagebox.h>
#include <ntqlayout.h>
#include <ntqlabel.h>
#include <ntqgroupbox.h>
#include <ntqheader.h>
#include <ntqcheckbox.h>

EditFunctions::EditFunctions( TQWidget *parent, FormWindow *fw, bool justSlots )
    : EditFunctionsBase( parent, 0, true ), formWindow( fw )
{
    connect( helpButton, TQ_SIGNAL( clicked() ), MainWindow::self, TQ_SLOT( showDialogHelp() ) );

    id = 0;
    functList.clear();

    TQValueList<MetaDataBase::Function> functionList = MetaDataBase::functionList( fw );
    for ( TQValueList<MetaDataBase::Function>::Iterator it = functionList.begin(); it != functionList.end(); ++it ) {
	TQListViewItem *i = new TQListViewItem( functionListView );

	i->setPixmap( 0, TQPixmap::fromMimeSource( "designer_editslots.png" ) );
	i->setText( 0, (*it).function );
	i->setText( 1, (*it).returnType );
	i->setText( 2, (*it).specifier );
	i->setText( 3, (*it).access  );
	i->setText( 4, (*it).type );

	FunctItem fui;
	fui.id = id;
	fui.oldName = (*it).function;
	fui.newName = fui.oldName;
	fui.oldRetTyp = (*it).returnType;
	fui.retTyp = fui.oldRetTyp;
	fui.oldSpec = (*it).specifier;
	fui.spec = fui.oldSpec;
	fui.oldAccess = (*it).access;
	fui.access = fui.oldAccess;
	fui.oldType = (*it).type;
	fui.type = fui.oldType;
	functList.append( fui );

	functionIds.insert( i, id );
	id++;

	if ( (*it).type == "slot" ) {
	    if ( MetaDataBase::isSlotUsed( formWindow, MetaDataBase::normalizeFunction( (*it).function ).latin1() ) )
		i->setText( 5, tr( "Yes" ) );
	    else
		i->setText( 5, tr( "No" ) );
	} else {
	    i->setText( 5, "---" );
	}
    }

    boxProperties->setEnabled( false );
    functionName->setValidator( new AsciiValidator( true, functionName ) );

    if ( functionListView->firstChild() )
	functionListView->setCurrentItem( functionListView->firstChild() );

    showOnlySlots->setChecked( justSlots );
    lastType = "function";

    // Enable rename for all TQListViewItems
    TQListViewItemIterator lvit = functionListView->firstChild();
    for ( ; *lvit; lvit++ )
	(*lvit)->setRenameEnabled( 0, true );
    
    // Connect listview signal to signal-relay
    TQObject::connect( functionListView,
		      TQ_SIGNAL( itemRenamed( TQListViewItem*, int, const TQString & ) ),
		      this,
		      TQ_SLOT( emitItemRenamed(TQListViewItem*, int, const TQString&) ) );

    // Connect signal-relay to TQLineEdit "functionName"
    TQObjectList *l = parent->queryList( "TQLineEdit", "functionName" );
    TQObject *obj;
    TQObjectListIt itemsLineEditIt( *l );
    while ( (obj = itemsLineEditIt.current()) != 0 ) {
        ++itemsLineEditIt;
	TQObject::connect( this,
			  TQ_SIGNAL( itemRenamed( const TQString & ) ),
			  obj,
			  TQ_SLOT( setText( const TQString & ) ) );
    }
    delete l;
}

void EditFunctions::okClicked()
{
    TQValueList<MetaDataBase::Function> functionList = MetaDataBase::functionList( formWindow );
    TQString n = tr( "Add/Remove functions of '%1'" ).arg( formWindow->name() );
    TQPtrList<Command> commands;
    TQValueList<MetaDataBase::Function>::Iterator fit;
    if ( !functionList.isEmpty() ) {
	for ( fit = functionList.begin(); fit != functionList.end(); ++fit ) {
	    bool functionFound = false;
	    TQValueList<FunctItem>::Iterator it = functList.begin();
	    for ( ; it != functList.end(); ++it ) {
		if ( MetaDataBase::normalizeFunction( (*it).oldName ) ==
		     MetaDataBase::normalizeFunction( (*fit).function ) ) {
		    functionFound = true;
		    break;
		}
	    }
	    if ( !functionFound )
		commands.append( new RemoveFunctionCommand( tr( "Remove function" ),
							    formWindow, (*fit).function, (*fit).specifier,
							    (*fit).access,
							    (*fit).type,
							    formWindow->project()->language(),
							    (*fit).returnType ) );
	}
    }

    bool invalidFunctions = false;
    TQValueList<FunctItem> invalidItems;

    if ( !functList.isEmpty() ) {
	TQStrList lst;
	TQValueList<FunctItem>::Iterator it = functList.begin();
	for ( ; it != functList.end(); ++it ) {
	    MetaDataBase::Function function;
	    function.function = (*it).newName;
	    function.returnType = (*it).retTyp;
	    function.specifier = (*it).spec;
	    function.access = (*it).access;
	    function.type = (*it).type;
	    function.language = formWindow->project()->language();
	    if ( function.returnType.isEmpty() )
		function.returnType = "void";
	    TQString s = function.function;
	    s = s.simplifyWhiteSpace();
	    bool startNum = s[ 0 ] >= '0' && s[ 0 ] <= '9';
	    bool noParens = s.contains( '(' ) != 1 || s.contains( ')' ) != 1;
	    bool illegalSpace = s.find( ' ' ) != -1 && s.find( ' ' ) < s.find( '(' );

	    if ( startNum || noParens || illegalSpace || lst.find( function.function ) != -1 ) {
		invalidFunctions = true;
		invalidItems.append( (*it) );
		continue;
	    }
	    bool functionFound = false;
	    for ( fit = functionList.begin(); fit != functionList.end(); ++fit ) {
		if ( MetaDataBase::normalizeFunction( (*fit).function ) ==
		     MetaDataBase::normalizeFunction( (*it).oldName ) ) {
		    functionFound = true;
		    break;
		}
	    }
	    if ( !functionFound )
		commands.append( new AddFunctionCommand( tr( "Add function" ),
							formWindow, function.function, function.specifier,
							function.access,
							function.type, formWindow->project()->language(),
							function.returnType ) );
	    if ( MetaDataBase::normalizeFunction( (*it).newName ) != MetaDataBase::normalizeFunction( (*it).oldName ) ||
		 (*it).spec != (*it).oldSpec || (*it).access != (*it).oldAccess || (*it).type != (*it).oldType ||
		 (*it).retTyp != (*it).oldRetTyp ) {
		TQString normalizedOldName = MetaDataBase::normalizeFunction( (*it).oldName );
		if ((*it).oldName.endsWith("const")) // make sure we get the 'const' when we remove the old name
		    normalizedOldName += " const";
		commands.append( new ChangeFunctionAttribCommand( tr( "Change function attributes" ),
								  formWindow, function, normalizedOldName,
								  (*it).oldSpec, (*it).oldAccess, (*it).oldType,
								  formWindow->project()->language(), (*it).oldRetTyp ) );
	    }
	    lst.append( function.function );
	}
    }

    if ( invalidFunctions ) {
	if ( TQMessageBox::information( this, tr( "Edit Functions" ),
				       tr( "Some syntactically incorrect functions have been defined.\n"
				       "Remove these functions?" ), tr( "&Yes" ), tr( "&No" ) ) == 0 ) {
	    TQValueList<FunctItem>::Iterator it = functList.begin();
	    while ( it != functList.end() ) {
		bool found = false;
		TQValueList<FunctItem>::Iterator vit = invalidItems.begin();
		for ( ; vit != invalidItems.end(); ++vit ) {
		    if ( (*vit).newName == (*it).newName ) {
			invalidItems.remove( vit );
			found = true;
			break;
		    }
		}
		if ( found ) {
		    int delId = (*it).id;
		    it = functList.remove( it );
		    TQMap<TQListViewItem*, int>::Iterator fit = functionIds.begin();
		    while ( fit != functionIds.end() ) {
			if ( *fit == delId ) {
			    TQListViewItem *litem = fit.key();
			    functionIds.remove( fit );
			    delete litem;
			    if ( functionListView->currentItem() )
				functionListView->setSelected( functionListView->currentItem(), true );
			    currentItemChanged( functionListView->currentItem() );
			    break;
			}
			++fit;
		    }
		}
		else
		    ++it;
	    }
	    if ( functionListView->firstChild() ) {
		functionListView->setCurrentItem( functionListView->firstChild() );
		functionListView->setSelected( functionListView->firstChild(), true );
	    }
	}
	formWindow->mainWindow()->objectHierarchy()->updateFormDefinitionView();
	return;
    }

    if ( !commands.isEmpty() ) {
	MacroCommand *cmd = new MacroCommand( n, formWindow, commands );
	formWindow->commandHistory()->addCommand( cmd );
	cmd->execute();
    }

    formWindow->mainWindow()->objectHierarchy()->updateFormDefinitionView();
    accept();
}

void EditFunctions::functionAdd( const TQString &access, const TQString &type )
{
    TQListViewItem *i = new TQListViewItem( functionListView );
    i->setPixmap( 0, TQPixmap::fromMimeSource( "designer_editslots.png" ) );
    i->setRenameEnabled( 0, true );
    i->setText( 1, "void" );
    i->setText( 2, "virtual" );

    if ( access.isEmpty() )
	i->setText( 3, "public" );
    else
	i->setText( 3, access );

    if( type.isEmpty() ) {
	if ( showOnlySlots->isChecked() )
	    i->setText( 4, "slot" );
	else {
	    i->setText( 4, lastType );
	}
    } else {
	i->setText( 4, type );
    }

    if ( i->text( 4 ) == "slot" ) {
	i->setText( 0, "newSlot()" );
	if ( MetaDataBase::isSlotUsed( formWindow, "newSlot()" ) )
	    i->setText( 5, tr( "Yes" ) );
	else
	    i->setText( 5, tr( "No" ) );
    } else {
	i->setText( 0, "newFunction()" );
	i->setText( 5, "---" );
    }
    
    functionListView->setCurrentItem( i );
    functionListView->setSelected( i, true );
    functionListView->ensureItemVisible( i );
    functionName->setFocus();
    functionName->selectAll();

    FunctItem fui;
    fui.id = id;
    fui.oldName = i->text( 0 );
    fui.newName = fui.oldName;
    fui.oldRetTyp = i->text( 1 );
    fui.retTyp = fui.oldRetTyp;
    fui.oldSpec = i->text ( 2 );
    fui.spec = fui.oldSpec;
    fui.oldAccess = i->text( 3 );
    fui.access = fui.oldAccess;
    fui.oldType = i->text( 4 );
    fui.type = fui.oldType;
    lastType = fui.oldType;
    functList.append( fui );
    functionIds.insert( i, id );
    id++;
}

void EditFunctions::functionRemove()
{
    if ( !functionListView->currentItem() )
	return;

    functionListView->blockSignals( true );
    removedFunctions << MetaDataBase::normalizeFunction( functionListView->currentItem()->text( 0 ) );
    int delId = functionIds[ functionListView->currentItem() ];
    TQValueList<FunctItem>::Iterator it = functList.begin();
    while ( it != functList.end() ) {
	if ( (*it).id == delId ) {
	    functList.remove( it );
	    break;
	}
	++it;
    }
    functionIds.remove( functionListView->currentItem() );
    delete functionListView->currentItem();
    if ( functionListView->currentItem() )
	functionListView->setSelected( functionListView->currentItem(), true );
    functionListView->blockSignals( false );
    currentItemChanged( functionListView->currentItem() );
}

void EditFunctions::currentItemChanged( TQListViewItem *i )
{
    functionName->blockSignals( true );
    functionName->setText( "" );
    functionAccess->setCurrentItem( 0 );
    functionName->blockSignals( false );

    if ( !i ) {
	boxProperties->setEnabled( false );
	return;
    }

    functionName->blockSignals( true );
    functionName->setText( i->text( 0 ) );
    editType->setText( i->text( 1 ) );
    TQString specifier = i->text( 2 );
    TQString access = i->text( 3 );
    TQString type = i->text( 4 );
    if ( specifier == "pure virtual" )
	functionSpecifier->setCurrentItem( 2 );
    else if ( specifier == "non virtual" )
	functionSpecifier->setCurrentItem( 0 );
    else if ( specifier == "virtual" )
	functionSpecifier->setCurrentItem( 1 );
    else
	functionSpecifier->setCurrentItem( 3 );
    if ( access == "private" )
	functionAccess->setCurrentItem( 2 );
    else if ( access == "protected" )
	functionAccess->setCurrentItem( 1 );
    else
	functionAccess->setCurrentItem( 0 );
    if ( type == "slot" )
	functionType->setCurrentItem( 0 );
    else
	functionType->setCurrentItem( 1 );

    functionName->blockSignals( false );
    boxProperties->setEnabled( true );
}

void EditFunctions::currentTextChanged( const TQString &txt )
{
    if ( !functionListView->currentItem() )
	return;

    changeItem( functionListView->currentItem(), Name, txt );
    functionListView->currentItem()->setText( 0, txt );

    if ( functionListView->currentItem()->text( 4 ) == "slot" ) {
	if ( MetaDataBase::isSlotUsed( formWindow, MetaDataBase::normalizeFunction( txt.latin1() ).latin1() ) )
	    functionListView->currentItem()->setText( 5, tr( "Yes" ) );
	else
	    functionListView->currentItem()->setText( 5, tr( "No" ) );
    } else {
	functionListView->currentItem()->setText( 5, "---" );
    }
}

void EditFunctions::currentSpecifierChanged( const TQString& s )
{
    if ( !functionListView->currentItem() )
	return;

    changeItem( functionListView->currentItem(), Specifier, s );
    functionListView->currentItem()->setText( 2, s );
}

void EditFunctions::currentAccessChanged( const TQString& a )
{
    if ( !functionListView->currentItem() )
	return;
    changeItem( functionListView->currentItem(), Access, a );
    functionListView->currentItem()->setText( 3, a );
}


void EditFunctions::currentReturnTypeChanged( const TQString &type )
{
    if ( !functionListView->currentItem() )
	return;
    changeItem( functionListView->currentItem(), ReturnType, type );
    functionListView->currentItem()->setText( 1, type );
}

void EditFunctions::currentTypeChanged( const TQString &type )
{
    if ( !functionListView->currentItem() )
	return;
    changeItem( functionListView->currentItem(), Type,  type );
    lastType = type;
    functionListView->currentItem()->setText( 4, type );
    if ( type == "slot" ) {
	if ( MetaDataBase::isSlotUsed( formWindow,
		MetaDataBase::normalizeFunction( functionListView->currentItem()->text( 0 ).latin1() ).latin1() ) )
	    functionListView->currentItem()->setText( 5, tr( "Yes" ) );
	else
	    functionListView->currentItem()->setText( 5, tr( "No" ) );
    } else {
	functionListView->currentItem()->setText( 5, "---" );
    }
}

void EditFunctions::changeItem( TQListViewItem *item, Attribute a, const TQString &nV )
{
    int itemId;
    TQMap<TQListViewItem*, int>::Iterator fit = functionIds.find( item );
    if ( fit != functionIds.end() )
	itemId = *fit;
    else
	return;

    TQValueList<FunctItem>::Iterator it = functList.begin();
    for ( ; it != functList.end(); ++it ) {
	if ( (*it).id == itemId ) {
	    switch( a ) {
		case Name:
		    (*it).newName = nV;
		    break;
		case Specifier:
		    (*it).spec = nV;
		    break;
		case Access:
		    (*it).access = nV;
		    break;
		case ReturnType:
		    (*it).retTyp = nV;
		    break;
		case Type:
		    (*it).type = nV;
		    break;
	    }
	}
    }
}

void EditFunctions::setCurrentFunction( const TQString &function )
{
    TQListViewItemIterator it( functionListView );
    while ( it.current() ) {
	if ( MetaDataBase::normalizeFunction( it.current()->text( 0 ) ) == function ) {
	    functionListView->setCurrentItem( it.current() );
	    functionListView->setSelected( it.current(), true );
	    currentItemChanged( it.current() );
	    return;
	}
	++it;
    }
}

void EditFunctions::displaySlots( bool justSlots )
{
    functionIds.clear();
    functionListView->clear();
    for ( TQValueList<FunctItem>::Iterator it = functList.begin(); it != functList.end(); ++it ) {
	if ( (*it).type == "function" && justSlots )
	    continue;
	TQListViewItem *i = new TQListViewItem( functionListView );
	functionIds.insert( i, (*it).id );
	i->setPixmap( 0, TQPixmap::fromMimeSource( "designer_editslots.png" ) );
	i->setText( 0, (*it).newName );
	i->setText( 1, (*it).retTyp );
	i->setText( 2, (*it).spec );
	i->setText( 3, (*it).access  );
	i->setText( 4, (*it).type );

	if ( (*it).type == "slot" ) {
	    if ( MetaDataBase::isSlotUsed( formWindow, MetaDataBase::normalizeFunction( (*it).newName ).latin1() ) )
		i->setText( 5, tr( "Yes" ) );
	    else
		i->setText( 5, tr( "No" ) );
	} else {
	    i->setText( 5, "---" );
	}
    }

    if ( functionListView->firstChild() )
	functionListView->setSelected( functionListView->firstChild(), true );
}

void EditFunctions::emitItemRenamed( TQListViewItem *, int, const TQString & text )
{
    emit itemRenamed( text ); // Relay signal ( to TQLineEdit )
}
