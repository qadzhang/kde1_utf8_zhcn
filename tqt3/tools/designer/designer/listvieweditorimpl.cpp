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

#include "formwindow.h"
#include "mainwindow.h"
#include "listvieweditorimpl.h"
#include "pixmapchooser.h"
#include "command.h"
#include "listviewdnd.h"
#include "listboxdnd.h"
#include "listboxrename.h"

#include <ntqlistview.h>
#include <ntqheader.h>
#include <ntqlistbox.h>
#include <ntqlineedit.h>
#include <ntqcheckbox.h>
#include <ntqlabel.h>
#include <ntqtabwidget.h>
#include <ntqspinbox.h>
#include <ntqpushbutton.h>
#include <ntqptrstack.h>

ListViewEditor::ListViewEditor( TQWidget *parent, TQListView *lv, FormWindow *fw )
    : ListViewEditorBase( parent, 0, true ), listview( lv ), formwindow( fw )
{
    connect( helpButton, TQ_SIGNAL( clicked() ), MainWindow::self, TQ_SLOT( showDialogHelp() ) );
    itemText->setEnabled( false );
    itemChoosePixmap->setEnabled( false );
    itemDeletePixmap->setEnabled( false );
    itemColumn->setEnabled( false );

    setupColumns();
    PopulateListViewCommand::transferItems( listview, itemsPreview );
    setupItems();

    itemsPreview->setShowSortIndicator( listview->showSortIndicator() );
    itemsPreview->setAllColumnsShowFocus( listview->allColumnsShowFocus() );
    itemsPreview->setRootIsDecorated( listview->rootIsDecorated() );

    if ( itemsPreview->firstChild() ) {
	itemsPreview->setCurrentItem( itemsPreview->firstChild() );
	itemsPreview->setSelected( itemsPreview->firstChild(), true );
    }

    // Clamp on drag and drop to TQListView
    ListViewDnd *itemsDnd = new ListViewDnd( itemsPreview );
    itemsDnd->setDragMode( ListViewDnd::Internal | ListViewDnd::Move );
    TQObject::connect( itemsDnd, TQ_SIGNAL( dropped( TQListViewItem * ) ),
		      itemsDnd, TQ_SLOT( confirmDrop( TQListViewItem * ) ) );

    // Enable rename for all TQListViewItems
    TQListViewItemIterator it = ((TQListView *)itemsPreview)->firstChild();
    for ( ; *it; it++ )
	(*it)->setRenameEnabled( 0, true );

    // Connect listview signal to signal-relay
    TQObject::connect( itemsPreview,
		      TQ_SIGNAL( itemRenamed( TQListViewItem*, int, const TQString & ) ),
		      this,
		      TQ_SLOT( emitItemRenamed(TQListViewItem*, int, const TQString&) ) );

    // Connect signal-relay to TQLineEdit "itemText"
    TQObjectList *l = parent->queryList( "TQLineEdit", "itemText" );
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

    // Clamp on drag and drop to TQListBox
    ListBoxDnd *columnsDnd = new ListBoxDnd( colPreview );
    columnsDnd->setDragMode( ListBoxDnd::Internal | ListBoxDnd::Move );
    TQObject::connect( columnsDnd, TQ_SIGNAL( dropped( TQListBoxItem * ) ),
		      columnsDnd, TQ_SLOT( confirmDrop( TQListBoxItem * ) ) );

    // Clamp on rename to TQListBox
    ListBoxRename *columnsRename = new ListBoxRename( colPreview );
    TQObject::connect( columnsRename,
		      TQ_SIGNAL( itemTextChanged( const TQString & ) ),
		      this,
		      TQ_SLOT( columnTextChanged( const TQString & ) ) );

    // Find TQLineEdit "colText" and connect
    l = parent->queryList( "TQLineEdit", "colText" );
    TQObjectListIt columnsLineEditIt( *l );
    while ( (obj = columnsLineEditIt.current()) != 0 ) {
        ++columnsLineEditIt;
	TQObject::connect( columnsRename,
			  TQ_SIGNAL( itemTextChanged( const TQString & ) ),
			  obj,
			  TQ_SLOT( setText( const TQString & ) ) );
    }
    delete l;
}

void ListViewEditor::applyClicked()
{
    setupItems();
    PopulateListViewCommand *cmd = new PopulateListViewCommand( tr( "Edit the Items and Columns of '%1'" ).arg( listview->name() ),
								formwindow, listview, itemsPreview );
    cmd->execute();
    formwindow->commandHistory()->addCommand( cmd );
}

void ListViewEditor::okClicked()
{
    applyClicked();
    accept();
}

void ListViewEditor::columnClickable( bool b )
{
    Column *c = findColumn( colPreview->item( colPreview->currentItem() ) );
    if ( !c )
	return;
    c->clickable = b;
}

void ListViewEditor::columnDownClicked()
{
    if ( colPreview->currentItem() == -1 ||
	 colPreview->currentItem() > (int)colPreview->count() - 2 )
	return;

    colPreview->clearSelection();
    TQListBoxItem *i = colPreview->item( colPreview->currentItem() );
    TQListBoxItem *below = i->next();

    colPreview->takeItem( i );
    colPreview->insertItem( i, below );

    colPreview->setCurrentItem( i );
    colPreview->setSelected( i, true );
}

void ListViewEditor::columnPixmapChosen()
{
    Column *c = findColumn( colPreview->item( colPreview->currentItem() ) );
    if ( !c )
	return;

    TQPixmap pix;
    if ( colPixmap->pixmap() )
	pix = qChoosePixmap( this, formwindow, *colPixmap->pixmap() );
    else
	pix = qChoosePixmap( this, formwindow, TQPixmap() );

    if ( pix.isNull() )
	return;

    c->pixmap = pix;
    colPreview->blockSignals( true );
    if ( !c->pixmap.isNull() )
	colPreview->changeItem( c->pixmap, c->text, colPreview->index( c->item ) );
    else
	colPreview->changeItem( c->text, colPreview->index( c->item ) );
    c->item = colPreview->item( colPreview->currentItem() );
    colPixmap->setPixmap( c->pixmap );
    colPreview->blockSignals( false );
    colDeletePixmap->setEnabled( true );
}

void ListViewEditor::columnPixmapDeleted()
{
    Column *c = findColumn( colPreview->item( colPreview->currentItem() ) );
    if ( !c )
	return;

    c->pixmap = TQPixmap();
    colPreview->blockSignals( true );
    if ( !c->pixmap.isNull() )
	colPreview->changeItem( c->pixmap, c->text, colPreview->index( c->item ) );
    else
	colPreview->changeItem( c->text, colPreview->index( c->item ) );
    c->item = colPreview->item( colPreview->currentItem() );
    colPixmap->setText( "" );
    colPreview->blockSignals( false );
    colDeletePixmap->setEnabled( false );
}

void ListViewEditor::columnResizable( bool b )
{
    Column *c = findColumn( colPreview->item( colPreview->currentItem() ) );
    if ( !c )
	return;
    c->resizable = b;
}

void ListViewEditor::columnTextChanged( const TQString &txt )
{
    Column *c = findColumn( colPreview->item( colPreview->currentItem() ) );
    if ( !c )
	return;

    c->text = txt;
    colPreview->blockSignals( true );
    if ( !c->pixmap.isNull() )
	colPreview->changeItem( c->pixmap, c->text, colPreview->index( c->item ) );
    else
	colPreview->changeItem( c->text, colPreview->index( c->item ) );
    c->item = colPreview->item( colPreview->currentItem() );
    colPreview->blockSignals( false );
}

void ListViewEditor::columnUpClicked()
{
    if ( colPreview->currentItem() <= 0 )
	return;

    colPreview->clearSelection();
    TQListBoxItem *i = colPreview->item( colPreview->currentItem() );
    TQListBoxItem *above = i->prev();

    colPreview->takeItem( above );
    colPreview->insertItem( above, i );

    colPreview->setCurrentItem( i );
    colPreview->setSelected( i, true );
}

void ListViewEditor::currentColumnChanged( TQListBoxItem *i )
{
    Column *c = findColumn( i );
    if ( !i || !c ) {
	colText->setEnabled( false );
	colPixmap->setEnabled( false );
	colDeletePixmap->setEnabled( false );
	colText->blockSignals( true );
	colText->setText( "" );
	colText->blockSignals( false );
	colClickable->setEnabled( false );
	colResizable->setEnabled( false );
	return;
    }

    colText->setEnabled( true );
    colPixmap->setEnabled( true );
    colDeletePixmap->setEnabled( i->pixmap() && !i->pixmap()->isNull() );
    colClickable->setEnabled( true );
    colResizable->setEnabled( true );

    colText->blockSignals( true );
    colText->setText( c->text );
    colText->blockSignals( false );
    if ( !c->pixmap.isNull() )
	colPixmap->setPixmap( c->pixmap );
    else
	colPixmap->setText( "" );
    colClickable->setChecked( c->clickable );
    colResizable->setChecked( c->resizable );
}

void ListViewEditor::newColumnClicked()
{
    Column col;
    col.text = tr( "New Column" );
    col.pixmap = TQPixmap();
    col.clickable = true;
    col.resizable = true;
    if ( !col.pixmap.isNull() )
	col.item = new TQListBoxPixmap( colPreview, col.pixmap, col.text );
    else
	col.item = new TQListBoxText( colPreview, col.text );
    columns.append( col );
    colPreview->setCurrentItem( col.item );
    colPreview->setSelected( col.item, true );
}

void ListViewEditor::deleteColumnClicked()
{
    TQListBoxItem *i = colPreview->item( colPreview->currentItem() );
    if ( !i )
	return;

    for ( TQValueList<Column>::Iterator it = columns.begin(); it != columns.end(); ++it ) {
	if ( ( *it ).item == i ) {
	    delete (*it).item;
	    columns.remove( it );
	    break;
	}
    }

    if ( colPreview->currentItem() != -1 )
	colPreview->setSelected( colPreview->currentItem(), true );
}

void ListViewEditor::currentItemChanged( TQListViewItem *i )
{
    if ( !i ) {
	itemText->setEnabled( false );
	itemChoosePixmap->setEnabled( false );
	itemDeletePixmap->setEnabled( false );
	itemColumn->setEnabled( false );
	return;
    }

    itemText->setEnabled( true );
    itemChoosePixmap->setEnabled( true );
    itemDeletePixmap->setEnabled( i->pixmap( itemColumn->value() ) &&
				  !i->pixmap( itemColumn->value() )->isNull() );
    itemColumn->setEnabled( true );

    displayItem( i, itemColumn->value() );
}

void ListViewEditor::displayItem( TQListViewItem *i, int col )
{
    itemText->blockSignals( true );
    itemText->setText( i->text( col ) );
    itemText->blockSignals( false );

    itemPixmap->blockSignals( true );
    if ( i->pixmap( col ) )
	itemPixmap->setPixmap( *i->pixmap( col ) );
    else
	itemPixmap->setText( "" );
    itemPixmap->blockSignals( false );
}

void ListViewEditor::itemColChanged( int col )
{
    TQListViewItem *i = itemsPreview->currentItem();
    if ( !i )
	return;

    displayItem( i, col );
    itemDeletePixmap->setEnabled( i->pixmap( col ) && !i->pixmap( col )->isNull() );
}

void ListViewEditor::itemDeleteClicked()
{
    TQListViewItem *i = itemsPreview->currentItem();
    if ( !i )
	return;

    delete i;
    if ( itemsPreview->firstChild() ) {
	itemsPreview->setCurrentItem( itemsPreview->firstChild() );
	itemsPreview->setSelected( itemsPreview->firstChild(), true );
    }
}

void ListViewEditor::itemDownClicked()
{
    TQListViewItem *i = itemsPreview->currentItem();
    if ( !i )
	return;

    TQListViewItemIterator it( i );
    TQListViewItem *parent = i->parent();
    it++;
    while ( it.current() ) {
	if ( it.current()->parent() == parent )
	    break;
	it++;
    }

    if ( !it.current() )
	return;
    TQListViewItem *other = it.current();

    i->moveItem( other );
}

void ListViewEditor::itemNewClicked()
{
    TQListViewItem *item = new TQListViewItem( itemsPreview );
    item->setText( 0, "Item" );
    item->setRenameEnabled( 0, true );
    itemsPreview->setCurrentItem( item );
    itemsPreview->setSelected( item, true );
    itemText->setFocus();
    itemText->selectAll();
}

void ListViewEditor::itemNewSubClicked()
{
    TQListViewItem *parent = itemsPreview->currentItem();
    TQListViewItem *item = 0;
    if ( parent ) {
	item = new TQListViewItem( parent );
	parent->setOpen( true );
    } else {
	item = new TQListViewItem( itemsPreview );
    }
    item->setText( 0, "Subitem" );
    item->setRenameEnabled( 0, true );
    itemsPreview->setCurrentItem( item );
    itemsPreview->setSelected( item, true );
}

void ListViewEditor::itemPixmapChoosen()
{
    TQListViewItem *i = itemsPreview->currentItem();
    if ( !i )
	return;

    TQPixmap pix;
    if ( itemPixmap->pixmap() )
	pix = qChoosePixmap( this, formwindow, *itemPixmap->pixmap() );
    else
	pix = qChoosePixmap( this, formwindow, TQPixmap() );

    if ( pix.isNull() )
	return;

    i->setPixmap( itemColumn->value(), TQPixmap( pix ) );
    itemPixmap->setPixmap( pix );
    itemDeletePixmap->setEnabled( true );
}

void ListViewEditor::itemPixmapDeleted()
{
    TQListViewItem *i = itemsPreview->currentItem();
    if ( !i )
	return;

    i->setPixmap( itemColumn->value(), TQPixmap() );
    itemPixmap->setText( "" );
    itemDeletePixmap->setEnabled( false );
}

void ListViewEditor::itemTextChanged( const TQString &txt )
{
    TQListViewItem *i = itemsPreview->currentItem();
    if ( !i )
	return;
    i->setText( itemColumn->value(), txt );
}

void ListViewEditor::itemUpClicked()
{
    TQListViewItem *i = itemsPreview->currentItem();
    if ( !i )
	return;

    TQListViewItemIterator it( i );
    TQListViewItem *parent = i->parent();
    --it;
    while ( it.current() ) {
	if ( it.current()->parent() == parent )
	    break;
	--it;
    }

    if ( !it.current() )
	return;
    TQListViewItem *other = it.current();

    other->moveItem( i );
}

void ListViewEditor::itemRightClicked()
{
    TQListViewItem *i = itemsPreview->currentItem();
    if ( !i )
	return;

    TQListViewItemIterator it( i );
    TQListViewItem *parent = i->parent();
    parent = parent ? parent->firstChild() : itemsPreview->firstChild();
    if ( !parent )
	return;
    it++;
    while ( it.current() ) {
	if ( it.current()->parent() == parent )
	    break;
	it++;
    }

    if ( !it.current() )
	return;
    TQListViewItem *other = it.current();

    for ( int c = 0; c < itemsPreview->columns(); ++c ) {
	TQString s = i->text( c );
	i->setText( c, other->text( c ) );
	other->setText( c, s );
	TQPixmap pix;
	if ( i->pixmap( c ) )
	    pix = *i->pixmap( c );
	if ( other->pixmap( c ) )
	    i->setPixmap( c, *other->pixmap( c ) );
	else
	    i->setPixmap( c, TQPixmap() );
	other->setPixmap( c, pix );
    }

    itemsPreview->setCurrentItem( other );
    itemsPreview->setSelected( other, true );
}

void ListViewEditor::itemLeftClicked()
{
    TQListViewItem *i = itemsPreview->currentItem();
    if ( !i )
	return;

    TQListViewItemIterator it( i );
    TQListViewItem *parent = i->parent();
    if ( !parent )
	return;
    parent = parent->parent();
    --it;
    while ( it.current() ) {
	if ( it.current()->parent() == parent )
	    break;
	--it;
    }

    if ( !it.current() )
	return;
    TQListViewItem *other = it.current();

    for ( int c = 0; c < itemsPreview->columns(); ++c ) {
	TQString s = i->text( c );
	i->setText( c, other->text( c ) );
	other->setText( c, s );
	TQPixmap pix;
	if ( i->pixmap( c ) )
	    pix = *i->pixmap( c );
	if ( other->pixmap( c ) )
	    i->setPixmap( c, *other->pixmap( c ) );
	else
	    i->setPixmap( c, TQPixmap() );
	other->setPixmap( c, pix );
    }

    itemsPreview->setCurrentItem( other );
    itemsPreview->setSelected( other, true );
}

void ListViewEditor::setupColumns()
{
    TQHeader *h = listview->header();
    for ( int i = 0; i < (int)h->count(); ++i ) {
	Column col;
	col.text = h->label( i );
	col.pixmap = TQPixmap();
	if ( h->iconSet( i ) )
	    col.pixmap = h->iconSet( i )->pixmap();
	col.clickable = h->isClickEnabled( i );
	col.resizable = h->isResizeEnabled( i );
	if ( !col.pixmap.isNull() )
	    col.item = new TQListBoxPixmap( colPreview, col.pixmap, col.text );
	else
	    col.item = new TQListBoxText( colPreview, col.text );
	columns.append( col );
    }

    colText->setEnabled( false );
    colPixmap->setEnabled( false );
    colClickable->setEnabled( false );
    colResizable->setEnabled( false );

    if ( colPreview->firstItem() )
	colPreview->setCurrentItem( colPreview->firstItem() );
    numColumns = colPreview->count();
}

void ListViewEditor::setupItems()
{
    itemColumn->setMinValue( 0 );
    itemColumn->setMaxValue( TQMAX( numColumns - 1, 0 ) );
    int i = 0;
    TQHeader *header = itemsPreview->header();
    for ( TQListBoxItem *item = colPreview->firstItem(); item; item = item->next() ) {
	Column *col = findColumn( item );
	if ( !col )
	    continue;
	if ( i >= itemsPreview->columns() )
	    itemsPreview->addColumn( col->text );
	header->setLabel( i, col->pixmap, col->text );
	header->setResizeEnabled( col->resizable, i );
	header->setClickEnabled( col->clickable, i );
	++i;
    }
    while ( itemsPreview->columns() > i )
	itemsPreview->removeColumn( i );

    itemColumn->setValue( TQMIN( numColumns - 1, itemColumn->value() ) );
}

ListViewEditor::Column *ListViewEditor::findColumn( TQListBoxItem *i )
{
    if ( !i )
	return 0;

    for ( TQValueList<Column>::Iterator it = columns.begin(); it != columns.end(); ++it ) {
	if ( ( *it ).item == i )
	    return &( *it );
    }

    return 0;
}

void ListViewEditor::initTabPage( const TQString &page )
{
    numColumns = colPreview->count();
    if ( page == tr( "&Items" ) ) {
	setupItems();
	if ( numColumns == 0 ) {
	    itemNew->setEnabled( false );
	    itemNewSub->setEnabled( false );
	    itemText->setEnabled( false );
	    itemChoosePixmap->setEnabled( false );
	    itemDeletePixmap->setEnabled( false );
	    itemColumn->setEnabled( false );
	} else {
	    itemNew->setEnabled( true );
	    itemNewSub->setEnabled( true );
	}
    }
}

void ListViewEditor::emitItemRenamed( TQListViewItem *, int, const TQString & text )
{
    emit itemRenamed( text ); // Relay signal ( to TQLineEdit )
}
