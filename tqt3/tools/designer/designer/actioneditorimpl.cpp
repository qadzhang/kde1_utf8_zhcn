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

#include "actioneditorimpl.h"
#include "formwindow.h"
#include "metadatabase.h"
#include "actionlistview.h"
#include "connectiondialog.h"
#include "mainwindow.h"
#include "hierarchyview.h"
#include "formfile.h"

#include <ntqaction.h>
#include <ntqlineedit.h>
#include <ntqlabel.h>
#include <ntqtoolbutton.h>
#include <ntqlistview.h>
#include <ntqcheckbox.h>
#include <ntqpushbutton.h>
#include <ntqpopupmenu.h>
#include <ntqobjectlist.h>

ActionEditor::ActionEditor( TQWidget* parent,  const char* name, WFlags fl )
    : ActionEditorBase( parent, name, fl ), currentAction( 0 ), formWindow( 0 ),
    explicitlyClosed(false)
{
    listActions->addColumn( tr( "Actions" ) );
    setEnabled( false );
    buttonConnect->setEnabled( false );

    TQPopupMenu *popup = new TQPopupMenu( this );
    popup->insertItem( tr( "New &Action" ), this, TQ_SLOT( newAction() ) );
    popup->insertItem( tr( "New Action &Group" ), this, TQ_SLOT( newActionGroup() ) );
    popup->insertItem( tr( "New &Dropdown Action Group" ), this, TQ_SLOT( newDropDownActionGroup() ) );
    buttonNewAction->setPopup( popup );
    buttonNewAction->setPopupDelay( 0 );

    connect( listActions, TQ_SIGNAL( insertAction() ), this, TQ_SLOT( newAction() ) );
    connect( listActions, TQ_SIGNAL( insertActionGroup() ), this, TQ_SLOT( newActionGroup() ) );
    connect( listActions, TQ_SIGNAL( insertDropDownActionGroup() ), this, TQ_SLOT( newDropDownActionGroup() ) );
    connect( listActions, TQ_SIGNAL( deleteAction() ), this, TQ_SLOT( deleteAction() ) );
    connect( listActions, TQ_SIGNAL( connectAction() ), this, TQ_SLOT( connectionsClicked() ) );
}

void ActionEditor::closeEvent( TQCloseEvent *e )
{
    emit hidden();
    e->accept();
}

void ActionEditor::currentActionChanged( TQListViewItem *i )
{
    buttonConnect->setEnabled( i != 0 );
    if ( !i )
	return;
    currentAction = ( (ActionItem*)i )->action();
    if ( !currentAction )
	currentAction = ( (ActionItem*)i )->actionGroup();
    if ( formWindow && currentAction )
	formWindow->setActiveObject( currentAction );
    MainWindow::self->objectHierarchy()->hierarchyList()->setCurrent( currentAction );
}

void ActionEditor::setCurrentAction( TQAction *a )
{
    TQListViewItemIterator it( listActions );
    while ( it.current() ) {
	if ( ( (ActionItem*)it.current() )->action() == a || ( (ActionItem*)it.current() )->actionGroup() == a ) {
	    listActions->setCurrentItem( it.current() );
	    listActions->ensureItemVisible( it.current() );
	    break;
	}
	++it;
    }
}

TQAction *ActionEditor::newActionEx()
{
    ActionItem *i = new ActionItem( listActions, (bool)false );
    TQAction *a = i->action();
    TQObject::connect( a, TQ_SIGNAL( destroyed( TQObject * ) ),
		      this, TQ_SLOT( removeConnections( TQObject* ) ) );
    MetaDataBase::addEntry( i->action() );
    TQString n = "Action";
    formWindow->unify( i->action(), n, true );
    i->setText( 0, n );
    i->action()->setName( n );
    i->action()->setText( i->action()->name() );
    MetaDataBase::setPropertyChanged( i->action(), "text", true );
    MetaDataBase::setPropertyChanged( i->action(), "name", true );
    formWindow->actionList().append( i->action() );
    if ( formWindow->formFile() )
	formWindow->formFile()->setModified( true );
    return i->action();
}

void ActionEditor::deleteAction()
{
    if ( !currentAction )
	return;

    TQListViewItemIterator it( listActions );
    ActionItem *ai = 0;
    while ( it.current() ) {
	ai = (ActionItem*)it.current();
	if ( ai->action() == currentAction || ai->actionGroup() == currentAction ) {
	    emit removing( currentAction );
	    formWindow->actionList().removeRef( currentAction );
	    delete currentAction;
	    currentAction = 0;
	    delete it.current();
	    break;
	}
	++it;
    }
    if ( formWindow ) {
	formWindow->setActiveObject( formWindow->mainContainer() );
	if ( formWindow->formFile() )
	    formWindow->formFile()->setModified( true );
    }
}

void ActionEditor::newAction()
{
    ActionItem *actionParent = (ActionItem*)listActions->selectedItem();
    if ( actionParent ) {
	if ( !::tqt_cast<TQActionGroup*>(actionParent->actionGroup()) )
	    actionParent = (ActionItem*)actionParent->parent();
    }

    ActionItem *i = 0;
    if ( actionParent )
	i = new ActionItem( actionParent );
    else
	i = new ActionItem( listActions, (bool)false );
    TQAction *a = i->action();
    TQObject::connect( a, TQ_SIGNAL( destroyed( TQObject * ) ),
		      this, TQ_SLOT( removeConnections( TQObject* ) ) );
    MetaDataBase::addEntry( i->action() );
    TQString n = "Action";
    formWindow->unify( i->action(), n, true );
    i->setText( 0, n );
    i->action()->setName( n );
    i->action()->setText( i->action()->name() );
    if ( actionParent && actionParent->actionGroup() &&
	 actionParent->actionGroup()->usesDropDown() ) {
	i->action()->setToggleAction( true );
	MetaDataBase::setPropertyChanged( i->action(), "toggleAction", true );
    }
    MetaDataBase::setPropertyChanged( i->action(), "text", true );
    MetaDataBase::setPropertyChanged( i->action(), "name", true );
    listActions->setCurrentItem( i );
    if ( !actionParent )
	formWindow->actionList().append( i->action() );
    if ( formWindow->formFile() )
	formWindow->formFile()->setModified( true );
}

void ActionEditor::newActionGroup()
{
    ActionItem *actionParent = (ActionItem*)listActions->selectedItem();
    if ( actionParent ) {
	if ( !::tqt_cast<TQActionGroup*>(actionParent->actionGroup()) )
	    actionParent = (ActionItem*)actionParent->parent();
    }

    ActionItem *i = 0;
    if ( actionParent )
	i = new ActionItem( actionParent, true );
    else
	i = new ActionItem( listActions, true );
    TQAction *ag = i->actionGroup();
    TQObject::connect( ag, TQ_SIGNAL( destroyed( TQObject * ) ),
		      this, TQ_SLOT( removeConnections( TQObject* ) ) );
    MetaDataBase::addEntry( i->actionGroup() );
    MetaDataBase::setPropertyChanged( i->actionGroup(), "usesDropDown", true );
    TQString n = "ActionGroup";
    formWindow->unify( i->action(), n, true );
    i->setText( 0, n );
    i->actionGroup()->setName( n );
    i->actionGroup()->setText( i->actionGroup()->name() );
    MetaDataBase::setPropertyChanged( i->actionGroup(), "text", true );
    MetaDataBase::setPropertyChanged( i->actionGroup(), "name", true );
    listActions->setCurrentItem( i );
    i->setOpen( true );
    if ( !actionParent )
	formWindow->actionList().append( i->actionGroup() );
    if ( formWindow->formFile() )
	formWindow->formFile()->setModified( true );
}

void ActionEditor::newDropDownActionGroup()
{
    newActionGroup();
    ( (ActionItem*)listActions->currentItem() )->actionGroup()->setUsesDropDown( true );
}

void ActionEditor::setFormWindow( FormWindow *fw )
{
    listActions->clear();
    formWindow = fw;
    if ( !formWindow ||
	 !::tqt_cast<TQMainWindow*>(formWindow->mainContainer()) ) {
	setEnabled( false );
    } else {
	setEnabled( true );
	for ( TQAction *a = formWindow->actionList().first(); a; a = formWindow->actionList().next() ) {
	    ActionItem *i = 0;
	    if ( ::tqt_cast<TQAction*>(a->parent()) )
		continue;
	    i = new ActionItem( listActions, a );
	    i->setText( 0, a->name() );
	    i->setPixmap( 0, a->iconSet().pixmap() );
	    // make sure we don't duplicate the connection
 	    TQObject::disconnect( a, TQ_SIGNAL( destroyed( TQObject * ) ),
 				 this, TQ_SLOT( removeConnections( TQObject * ) ) );
	    TQObject::connect( a, TQ_SIGNAL( destroyed( TQObject * ) ),
			      this, TQ_SLOT( removeConnections( TQObject* ) ) );
	    if ( ::tqt_cast<TQActionGroup*>(a) ) {
		insertChildActions( i );
	    }
	}
	if ( listActions->firstChild() ) {
	    listActions->setCurrentItem( listActions->firstChild() );
	    listActions->setSelected( listActions->firstChild(), true );
	}
    }
}

void ActionEditor::insertChildActions( ActionItem *i )
{
    if ( !i->actionGroup() || !i->actionGroup()->children() )
	return;
    TQObjectListIt it( *i->actionGroup()->children() );
    while ( it.current() ) {
	TQObject *o = it.current();
	++it;
	if ( !::tqt_cast<TQAction*>(o) )
	    continue;
	TQAction *a = (TQAction*)o;
	ActionItem *i2 = new ActionItem( (TQListViewItem*)i, a );
	i->setOpen( true );
	i2->setText( 0, a->name() );
	i2->setPixmap( 0, a->iconSet().pixmap() );
	// make sure we don't duplicate the connection
 	TQObject::disconnect( o, TQ_SIGNAL( destroyed( TQObject * ) ),
 			     this, TQ_SLOT( removeConnections( TQObject * ) ) );
 	TQObject::connect( o, TQ_SIGNAL( destroyed( TQObject * ) ),
 			  this, TQ_SLOT( removeConnections( TQObject * ) ) );
	if ( ::tqt_cast<TQActionGroup*>(a) )
	    insertChildActions( i2 );
    }
}

void ActionEditor::updateActionName( TQAction *a )
{
    TQListViewItemIterator it( listActions );
    while ( it.current() ) {
	if ( ( (ActionItem*)it.current() )->action() == a )
	    ( (ActionItem*)it.current() )->setText( 0, a->name() );
	else if ( ( (ActionItem*)it.current() )->actionGroup() == a )
	    ( (ActionItem*)it.current() )->setText( 0, a->name() );
	++it;
    }
}

void ActionEditor::updateActionIcon( TQAction *a )
{
    TQListViewItemIterator it( listActions );
    while ( it.current() ) {
	if ( ( (ActionItem*)it.current() )->action() == a )
	    ( (ActionItem*)it.current() )->setPixmap( 0, a->iconSet().pixmap() );
	else if ( ( (ActionItem*)it.current() )->actionGroup() == a )
	    ( (ActionItem*)it.current() )->setPixmap( 0, a->iconSet().pixmap() );
	++it;
    }
}

void ActionEditor::connectionsClicked()
{
    ConnectionDialog dlg( formWindow->mainWindow() );
    dlg.setDefault( currentAction, formWindow );
    dlg.addConnection();
    dlg.exec();
}

void ActionEditor::removeConnections( TQObject *o )
{
    TQValueList<MetaDataBase::Connection> conns =
	MetaDataBase::connections( formWindow, o );
    for ( TQValueList<MetaDataBase::Connection>::Iterator it2 = conns.begin();
	  it2 != conns.end(); ++it2 )
	MetaDataBase::removeConnection( formWindow, (*it2).sender, (*it2).signal,
					(*it2).receiver, (*it2).slot );
}
