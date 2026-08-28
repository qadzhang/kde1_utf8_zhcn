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

#include "mainwindow.h"
#include "widgetaction.h"
#include "listviewdnd.h"
#include <ntqlistview.h>

void ConfigToolboxDialog::init()
{
    listViewTools->setSorting( -1 );
    listViewCommon->setSorting( -1 );

    ListViewDnd *toolsDnd = new ListViewDnd( listViewTools );
    toolsDnd->setDragMode( ListViewDnd::External | ListViewDnd::NullDrop | ListViewDnd::Flat );

    ListViewDnd *commonDnd = new ListViewDnd( listViewCommon );
    commonDnd->setDragMode( ListViewDnd::Both | ListViewDnd::Move | ListViewDnd::Flat );

    TQObject::connect( toolsDnd, TQ_SIGNAL( dropped( TQListViewItem * ) ),
			commonDnd, TQ_SLOT( confirmDrop( TQListViewItem * ) ) );
    TQObject::connect( commonDnd, TQ_SIGNAL( dropped( TQListViewItem * ) ),
			commonDnd, TQ_SLOT( confirmDrop( TQListViewItem * ) ) );

    TQDict<TQListViewItem> groups;
    TQAction *a;
    for ( a = MainWindow::self->toolActions.last();
	  a;
	  a = MainWindow::self->toolActions.prev() ) {
	TQString grp = ( (WidgetAction*)a )->group();
	TQListViewItem *parent = groups.find( grp );
	if ( !parent ) {
	    parent = new TQListViewItem( listViewTools );
	    parent->setText( 0, grp );
	    parent->setOpen( true );
	    groups.insert( grp, parent );
	}
	TQListViewItem *i = new TQListViewItem( parent );
	i->setText( 0, a->text() );
	i->setPixmap( 0, a->iconSet().pixmap() );
    }
    for ( a = MainWindow::self->commonWidgetsPage.last(); a;
    a = MainWindow::self->commonWidgetsPage.prev() ) {
	TQListViewItem *i = new TQListViewItem( listViewCommon );
	i->setText( 0, a->text() );
	i->setPixmap( 0, a->iconSet().pixmap() );
    }

}


void ConfigToolboxDialog::addTool()
{
    TQListView *src = listViewTools;

    bool addKids = false;
    TQListViewItem *nextSibling = 0;
    TQListViewItem *nextParent = 0;
    TQListViewItemIterator it = src->firstChild();
    for ( ; *it; it++ ) {
	// Hit the nextSibling, turn of child processing
	if ( (*it) == nextSibling )
	    addKids = false;

	if ( (*it)->isSelected() ) {
	    if ( (*it)->childCount() == 0 ) {
		// Selected, no children
		TQListViewItem *i = new TQListViewItem( listViewCommon, listViewCommon->lastItem() );
		i->setText( 0, (*it)->text(0) );
		i->setPixmap( 0, *((*it)->pixmap(0)) );
		listViewCommon->setCurrentItem( i );
		listViewCommon->ensureItemVisible( i );
	    } else if ( !addKids ) {
		// Children processing not set, so set it
		// Also find the item were we shall quit
		// processing children...if any such item
		addKids = true;
		nextSibling = (*it)->nextSibling();
		nextParent = (*it)->parent();
		while ( nextParent && !nextSibling ) {
		    nextSibling = nextParent->nextSibling();
		    nextParent = nextParent->parent();
		}
	    }
	} else if ( ((*it)->childCount() == 0) && addKids ) {
	    // Leaf node, and we _do_ process children
	    TQListViewItem *i = new TQListViewItem( listViewCommon, listViewCommon->lastItem() );
	    i->setText( 0, (*it)->text(0) );
	    i->setPixmap( 0, *((*it)->pixmap(0)) );
	    listViewCommon->setCurrentItem( i );
	    listViewCommon->ensureItemVisible( i );
	}
    }
}


void ConfigToolboxDialog::removeTool()
{
    TQListViewItemIterator it = listViewCommon->firstChild();
    while ( *it ) {
	if ( (*it)->isSelected() )
	    delete (*it);
	else
	    it++;
    }
}


void ConfigToolboxDialog::moveToolUp()
{
    TQListViewItem *next = 0;
    TQListViewItem *item = listViewCommon->firstChild();
    for ( int i = 0; i < listViewCommon->childCount(); ++i ) {
	next = item->itemBelow();
	if ( item->isSelected() && (i > 0) && !item->itemAbove()->isSelected() )
	    item->itemAbove()->moveItem( item );
	item = next;
    }
}


void ConfigToolboxDialog::moveToolDown()
{
    int count = listViewCommon->childCount();
    TQListViewItem *next = 0;
    TQListViewItem *item = listViewCommon->lastItem();
    for ( int i = 0; i < count; ++i ) {
	next = item->itemAbove();
	if ( item->isSelected() && (i > 0) && !item->itemBelow()->isSelected() )
	    item->moveItem( item->itemBelow() );
	item = next;
    }

 //   TQListViewItem *item = listViewCommon->firstChild();
 //   for ( int i = 0; i < listViewCommon->childCount(); ++i ) {
	//if ( item == listViewCommon->currentItem() ) {
	//    item->moveItem( item->itemBelow() );
	//    currentCommonToolChanged( item );
	//    break;
	//}
	//item = item->itemBelow();
 //   }
}


void ConfigToolboxDialog::currentToolChanged( TQListViewItem *i )
{
    bool canAdd = false;
    TQListViewItemIterator it = listViewTools->firstChild();
    for ( ; *it; it++ ) {
	if ( (*it)->isSelected() ) {
	    canAdd = true;
	    break;
	}
    }
    buttonAdd->setEnabled( canAdd || ( i && i->isSelected() ) );
}


void ConfigToolboxDialog::currentCommonToolChanged( TQListViewItem *i )
{
    buttonUp->setEnabled( (bool) (i && i->itemAbove()) );
    buttonDown->setEnabled( (bool) (i && i->itemBelow()) );

    bool canRemove = false;
    TQListViewItemIterator it = listViewCommon->firstChild();
    for ( ; *it; it++ ) {
	if ( (*it)->isSelected() ) {
	    canRemove = true;
	    break;
	}
    }
    buttonRemove->setEnabled( canRemove || ( i && i->isSelected() ) );
}


void ConfigToolboxDialog::ok()
{
    MainWindow::self->commonWidgetsPage.clear();
    TQListViewItem *item = listViewCommon->firstChild();
    for ( int j = 0; j < listViewCommon->childCount(); item = item->itemBelow(), ++j ) {
        TQAction *a = 0;
	for ( a = MainWindow::self->toolActions.last();
	    a;
	    a = MainWindow::self->toolActions.prev() ) {
	    if ( a->text() == item->text( 0 ) )
		break;
	}
	if ( a )
	    MainWindow::self->commonWidgetsPage.insert( j, a );
    }
}
