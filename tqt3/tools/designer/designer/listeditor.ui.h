/****************************************************************************
** ui.h extension file, included from the uic-generated form implementation.
**
** If you wish to add, delete or rename slots use TQt Designer which will
** update this file, preserving your code. Create an init() slot in place of
** a constructor, and a destroy() slot in place of a destructor.
*****************************************************************************/

#include <ntqheader.h>
#include <ntqapplication.h>

void ListEditor::init()
{
    listview->header()->hide();
    listview->setSorting( -1 );
    listview->setDefaultRenameAction( TQListView::Accept );
}

void ListEditor::setList( const TQStringList &l )
{
    TQListViewItem *i = 0;
    for ( TQStringList::ConstIterator it = l.begin(); it != l.end(); ++it ) {
	i = new TQListViewItem( listview, i );
	i->setText( 0, *it );
	i->setRenameEnabled( 0, true );
    }
}

void ListEditor::addItem()
{
    TQListViewItem *i = new TQListViewItem( listview, listview->lastItem() );
    i->setRenameEnabled( 0, true );
    tqApp->processEvents();
    i->startRename( 0 );
}

void ListEditor::renamed( TQListViewItem *i )
{
    if ( i && i->text( 0 ).isEmpty() )
	i->startRename( 0 );
}

void ListEditor::removeItems()
{
    TQListViewItemIterator it( listview );
    TQListViewItem *i = 0;
    while ( ( i = it.current() ) ) {
	++it;
	if ( i->isSelected() )
	    delete i;
    }
}

TQStringList ListEditor::items()
{
    TQStringList l;
    TQListViewItemIterator it( listview );
    TQListViewItem *i = 0;
    while ( ( i = it.current() ) ) {
	++it;
	if ( !i->text( 0 ).isEmpty() )
	    l << i->text( 0 );
    }
    return l;
}

void ListEditor::renameItem()
{
    TQListViewItem *i = listview->currentItem();
    if ( i )
	i->startRename( 0 );
}
