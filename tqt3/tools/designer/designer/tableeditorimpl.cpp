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

#include "tableeditorimpl.h"
#ifndef TQT_NO_TABLE
#include <ntqtable.h>
#endif
#include "formwindow.h"
#include <ntqlabel.h>
#include <ntqcombobox.h>
#include <ntqheader.h>
#include <ntqlistbox.h>
#include <ntqlineedit.h>
#include <ntqpushbutton.h>
#include "pixmapchooser.h"
#include "command.h"
#include <ntqvaluelist.h>
#include <ntqtabwidget.h>
#include <ntqdatatable.h>
#include "project.h"
#include "metadatabase.h"
#include "mainwindow.h"

TableEditor::TableEditor( TQWidget* parent,  TQWidget *editWidget, FormWindow *fw, const char* name, bool modal, WFlags fl )
    : TableEditorBase( parent, name, modal, fl ),
#ifndef TQT_NO_TABLE
    editTable( (TQTable*)editWidget ),
#endif
    formWindow( fw )
{
    connect( buttonHelp, TQ_SIGNAL( clicked() ), MainWindow::self, TQ_SLOT( showDialogHelp() ) );
#ifndef TQT_NO_TABLE
    labelColumnPixmap->setText( "" );
    labelRowPixmap->setText( "" );

#ifndef TQT_NO_SQL
    if ( !::tqt_cast<TQDataTable*>(editTable) ) 
#endif
    {
	labelFields->hide();
	comboFields->hide();
	labelTable->hide();
	labelTableValue->hide();
    }
#ifndef TQT_NO_SQL
    if ( ::tqt_cast<TQDataTable*>(editTable) ) {
	// ## why does this behave weird?
	//	TabWidget->removePage( rows_tab );
	//	rows_tab->hide();
	// ## do this in the meantime...
	TabWidget->setTabEnabled( rows_tab, false );
    }

    if ( formWindow->project() && ::tqt_cast<TQDataTable*>(editTable) ) {
	TQStringList lst = MetaDataBase::fakeProperty( editTable, "database" ).toStringList();
	if ( lst.count() == 2 && !lst[ 0 ].isEmpty() && !lst[ 1 ].isEmpty() ) {
	    TQStringList fields;
	    fields << "<no field>";
	    fields += formWindow->project()->databaseFieldList( lst[ 0 ], lst[ 1 ] );
	    comboFields->insertStringList( fields );
	}
	if ( !lst[ 1 ].isEmpty() )
	    labelTableValue->setText( lst[ 1 ] );
    }
#endif

    readFromTable();
#endif
}

TableEditor::~TableEditor()
{
}

void TableEditor::columnDownClicked()
{
    if ( listColumns->currentItem() == -1 ||
	 listColumns->currentItem() == (int)listColumns->count() - 1 ||
	 listColumns->count() < 2 )
	return;
    saveFieldMap();
    int index = listColumns->currentItem() + 1;
    TQListBoxItem *i = listColumns->item( listColumns->currentItem() );
    listColumns->takeItem( i );
    listColumns->insertItem( i, index );
    listColumns->setCurrentItem( i );
    readColumns();
    restoreFieldMap();
    currentColumnChanged( i );
}

void TableEditor::columnTextChanged( const TQString &s )
{
    if ( listColumns->currentItem() == -1 )
	return;
    listColumns->blockSignals( true );
    listColumns->changeItem( s, listColumns->currentItem() );
    listColumns->blockSignals( false );
#ifndef TQT_NO_TABLE
    if ( table->horizontalHeader()->iconSet( listColumns->currentItem() ) )
	table->horizontalHeader()->setLabel( listColumns->currentItem(),
					     *table->horizontalHeader()->iconSet( listColumns->currentItem() ), s );
    else
	table->horizontalHeader()->setLabel( listColumns->currentItem(), s );
#endif
}

void TableEditor::columnUpClicked()
{
    if ( listColumns->currentItem() <= 0 ||
	 listColumns->count() < 2 )
	return;
    saveFieldMap();
    int index = listColumns->currentItem() - 1;
    TQListBoxItem *i = listColumns->item( listColumns->currentItem() );
    listColumns->takeItem( i );
    listColumns->insertItem( i, index );
    listColumns->setCurrentItem( i );
    readColumns();
    restoreFieldMap();
    currentColumnChanged( i );
}

void TableEditor::currentColumnChanged( TQListBoxItem *i )
{
    if ( !i )
	return;
    editColumnText->blockSignals( true );
    editColumnText->setText( i->text() );
    if ( i->pixmap() )
	labelColumnPixmap->setPixmap( *i->pixmap() );
    else
	labelColumnPixmap->setText( "" );
    editColumnText->blockSignals( false );

#ifndef TQT_NO_SQL
    if ( ::tqt_cast<TQDataTable*>(editTable) ) {
	TQString s = *fieldMap.find( listColumns->index( i ) );
	if ( s.isEmpty() )
	    comboFields->setCurrentItem( 0 );
	else if ( comboFields->listBox()->findItem( s ) )
	    comboFields->setCurrentItem( comboFields->listBox()->index( comboFields->listBox()->findItem( s ) ) );
	else
	    comboFields->lineEdit()->setText( s );
    }
#endif
}

void TableEditor::currentFieldChanged( const TQString &s )
{
    if ( listColumns->currentItem() == -1 )
	return;
    fieldMap.remove( listColumns->currentItem() );
    fieldMap.insert( listColumns->currentItem(), s );
    editColumnText->blockSignals( true ); //## necessary
    TQString newColText = s.mid(0,1).upper() + s.mid(1);
    editColumnText->setText( newColText );
    columnTextChanged( newColText );
    editColumnText->blockSignals( false );
}

void TableEditor::currentRowChanged( TQListBoxItem *i )
{
    if ( !i )
	return;
    editRowText->blockSignals( true );
    editRowText->setText( i->text() );
    if ( i->pixmap() )
	labelRowPixmap->setPixmap( *i->pixmap() );
    else
	labelRowPixmap->setText( "" );
    editRowText->blockSignals( false );
}

void TableEditor::deleteColumnClicked()
{
    if ( listColumns->currentItem() == -1 )
	return;
#ifndef TQT_NO_TABLE
    table->setNumCols( table->numCols() - 1 );
    delete listColumns->item( listColumns->currentItem() );
    readColumns();
    if ( listColumns->firstItem() ) {
	listColumns->setCurrentItem( listColumns->firstItem() );
	listColumns->setSelected( listColumns->firstItem(), true );
    }
#endif
}

void TableEditor::deleteRowClicked()
{
    if ( listRows->currentItem() == -1 )
	return;
#ifndef TQT_NO_TABLE
    table->setNumRows( table->numRows() - 1 );
    delete listRows->item( listRows->currentItem() );
    readRows();
    if ( listRows->firstItem() ) {
	listRows->setCurrentItem( listRows->firstItem() );
	listRows->setSelected( listRows->firstItem(), true );
    }
#endif
}

void TableEditor::newColumnClicked()
{
#ifndef TQT_NO_TABLE
    table->setNumCols( table->numCols() + 1 );
    TQMap<TQString, bool> m;
    for ( int i = 0; i < table->numCols() - 1; ++i )
	m.insert( table->horizontalHeader()->label( i ), true );
    int n = table->numCols() - 1;
    TQString t = TQString::number( n );
    while ( m.find( t ) != m.end() )
	t = TQString::number( ++n );
    table->horizontalHeader()->setLabel( table->numCols() - 1, t );
    listColumns->insertItem( t );
    TQListBoxItem *item = listColumns->item( listColumns->count() - 1 );
    listColumns->setCurrentItem( item );
    listColumns->setSelected( item, true );
#ifndef TQT_NO_SQL
    if ( ::tqt_cast<TQDataTable*>(editTable) ) {
	comboFields->setFocus();
    } else
#endif
    {
	editColumnText->setFocus();
	editColumnText->selectAll();
    }
#endif
}

void TableEditor::newRowClicked()
{
#ifndef TQT_NO_TABLE
    table->setNumRows( table->numRows() + 1 );
    TQMap<TQString, bool> m;
    for ( int i = 0; i < table->numRows() - 1; ++i )
	m.insert( table->verticalHeader()->label( i ), true );
    int n = table->numRows() - 1;
    TQString t = TQString::number( n );
    while ( m.find( t ) != m.end() )
	t = TQString::number( ++n );
    table->verticalHeader()->setLabel( table->numRows() - 1, t );
    listRows->insertItem( t );
    TQListBoxItem *item = listRows->item( listRows->count() - 1 );
    listRows->setCurrentItem( item );
    listRows->setSelected( item, true );
#endif
}

void TableEditor::okClicked()
{
    applyClicked();
    accept();
}

void TableEditor::rowDownClicked()
{
    if ( listRows->currentItem() == -1 ||
	 listRows->currentItem() == (int)listRows->count() - 1 ||
	 listRows->count() < 2 )
	return;
    int index = listRows->currentItem() + 1;
    TQListBoxItem *i = listRows->item( listRows->currentItem() );
    listRows->takeItem( i );
    listRows->insertItem( i, index );
    listRows->setCurrentItem( i );
    readRows();
}

void TableEditor::rowTextChanged( const TQString &s )
{
    if ( listRows->currentItem() == -1 )
	return;
#ifndef TQT_NO_TABLE
    listRows->blockSignals(true);
    listRows->changeItem( s, listRows->currentItem() );
    listRows->blockSignals(false);
    if ( table->verticalHeader()->iconSet( listRows->currentItem() ) )
	table->verticalHeader()->setLabel( listRows->currentItem(),
					     *table->verticalHeader()->iconSet( listRows->currentItem() ), s );
    else
	table->verticalHeader()->setLabel( listRows->currentItem(), s );
#endif
}

void TableEditor::rowUpClicked()
{
    if ( listRows->currentItem() <= 0 ||
	 listRows->count() < 2 )
	return;
    int index = listRows->currentItem() - 1;
    TQListBoxItem *i = listRows->item( listRows->currentItem() );
    listRows->takeItem( i );
    listRows->insertItem( i, index );
    listRows->setCurrentItem( i );
    readRows();
}

void TableEditor::applyClicked()
{
    TQValueList<PopulateTableCommand::Row> rows;
    TQValueList<PopulateTableCommand::Column> cols;

    int i = 0;
#ifndef TQT_NO_TABLE
    for ( i = 0; i < table->horizontalHeader()->count(); ++i ) {
	PopulateTableCommand::Column col;
	col.text = table->horizontalHeader()->label( i );
	if ( table->horizontalHeader()->iconSet( i ) )
	    col.pix = table->horizontalHeader()->iconSet( i )->pixmap();
	col.field = *fieldMap.find( i );
	cols.append( col );
    }
    for ( i = 0; i < table->verticalHeader()->count(); ++i ) {
	PopulateTableCommand::Row row;
	row.text = table->verticalHeader()->label( i );
	if ( table->verticalHeader()->iconSet( i ) )
	    row.pix = table->verticalHeader()->iconSet( i )->pixmap();
	rows.append( row );
    }
    PopulateTableCommand *cmd = new PopulateTableCommand( tr( "Edit the Rows and Columns of '%1' " ).arg( editTable->name() ),
							  formWindow, editTable, rows, cols );
    cmd->execute();
    formWindow->commandHistory()->addCommand( cmd );
#endif
}

void TableEditor::chooseRowPixmapClicked()
{
    if ( listRows->currentItem() == -1 )
	return;
    TQPixmap pix;
    if ( listRows->item( listRows->currentItem() )->pixmap() )
	pix = qChoosePixmap( this, formWindow, *listRows->item( listRows->currentItem() )->pixmap() );
    else
	pix = qChoosePixmap( this, formWindow, TQPixmap() );

    if ( pix.isNull() )
	return;

#ifndef TQT_NO_TABLE
    table->verticalHeader()->setLabel( listRows->currentItem(), pix, table->verticalHeader()->label( listRows->currentItem() ) );
    listRows->changeItem( pix, listRows->currentText(), listRows->currentItem() );
#endif
}

void TableEditor::deleteRowPixmapClicked()
{
    if ( listRows->currentItem() == -1 )
	return;
#ifndef TQT_NO_TABLE
    table->verticalHeader()->setLabel( listRows->currentItem(), TQPixmap(), table->verticalHeader()->label( listRows->currentItem() ) );
    listRows->changeItem( listRows->currentText(), listRows->currentItem() );
#endif
}

void TableEditor::chooseColPixmapClicked()
{
    if ( listColumns->currentItem() == -1 )
	return;
    TQPixmap pix;
    if ( listColumns->item( listColumns->currentItem() )->pixmap() )
	pix = qChoosePixmap( this, formWindow, *listColumns->item( listColumns->currentItem() )->pixmap() );
    else
	pix = qChoosePixmap( this, formWindow, TQPixmap() );

    if ( pix.isNull() )
	return;
#ifndef TQT_NO_TABLE
    table->horizontalHeader()->setLabel( listColumns->currentItem(), pix, table->horizontalHeader()->label( listColumns->currentItem() ) );
    listColumns->changeItem( pix, listColumns->currentText(), listColumns->currentItem() );
#endif
}

void TableEditor::deleteColPixmapClicked()
{
    if ( listColumns->currentItem() == -1 )
	return;
#ifndef TQT_NO_TABLE
    table->horizontalHeader()->setLabel( listColumns->currentItem(), TQPixmap(), table->horizontalHeader()->label( listColumns->currentItem() ) );
    listColumns->changeItem( listColumns->currentText(), listColumns->currentItem() );
#endif
}

void TableEditor::readFromTable()
{
#ifndef TQT_NO_TABLE
    TQHeader *cols = editTable->horizontalHeader();
    table->setNumCols( cols->count() );
    TQMap<TQString, TQString> columnFields = MetaDataBase::columnFields( editTable );
    for ( int i = 0; i < cols->count(); ++i ) {
	if ( editTable->horizontalHeader()->iconSet( i ) ) {
	    table->horizontalHeader()->setLabel( i, *editTable->horizontalHeader()->iconSet( i ),
						 editTable->horizontalHeader()->label( i ) );
	    listColumns->insertItem( editTable->horizontalHeader()->iconSet( i )->pixmap(),
				     editTable->horizontalHeader()->label( i ) );
	} else {
	    table->horizontalHeader()->setLabel( i, editTable->horizontalHeader()->label( i ) );
	    listColumns->insertItem( editTable->horizontalHeader()->label( i ) );
	}
	TQString cf = *columnFields.find( editTable->horizontalHeader()->label( i ) );
	fieldMap.insert( i, cf );
    }

    if ( listColumns->firstItem() ) {
	listColumns->setCurrentItem( listColumns->firstItem() );
	listColumns->setSelected( listColumns->firstItem(), true );
    }

    TQHeader *rows = editTable->verticalHeader();
    table->setNumRows( rows->count() );
    for ( int j = 0; j < rows->count(); ++j ) {
	if ( editTable->verticalHeader()->iconSet( j ) ) {
	    table->verticalHeader()->setLabel( j, *editTable->verticalHeader()->iconSet( j ),
					       editTable->verticalHeader()->label( j ) );
	    listRows->insertItem( editTable->verticalHeader()->iconSet( j )->pixmap(),
				  editTable->verticalHeader()->label( j ) );
	} else {
	    table->verticalHeader()->setLabel( j, editTable->verticalHeader()->label( j ) );
	    listRows->insertItem( editTable->verticalHeader()->label( j ) );
	}
    }

    if ( listRows->firstItem() ) {
	listRows->setCurrentItem( listRows->firstItem() );
	listRows->setSelected( listRows->firstItem(), true );
    }
#endif
}

void TableEditor::readColumns()
{
    int j = 0;
#ifndef TQT_NO_TABLE
    for ( TQListBoxItem *i = listColumns->firstItem(); i; i = i->next(), ++j ) {
	if ( i->pixmap() )
	    table->horizontalHeader()->setLabel( j, *i->pixmap(), i->text() );
	else
	    table->horizontalHeader()->setLabel( j, i->text() );
    }
#endif
}

void TableEditor::readRows()
{
    int j = 0;
#ifndef TQT_NO_TABLE
    for ( TQListBoxItem *i = listRows->firstItem(); i; i = i->next(), ++j ) {
	if ( i->pixmap() )
	    table->verticalHeader()->setLabel( j, *i->pixmap(), i->text() );
	else
	    table->verticalHeader()->setLabel( j, i->text() );
    }
#endif
}

void TableEditor::saveFieldMap()
{
    tmpFieldMap.clear();
    for ( TQMap<int, TQString>::Iterator it = fieldMap.begin(); it != fieldMap.end(); ++it )
	tmpFieldMap.insert( listColumns->item( it.key() ), *it );
}

void TableEditor::restoreFieldMap()
{
    fieldMap.clear();
    for ( TQMap<TQListBoxItem*, TQString>::Iterator it = tmpFieldMap.begin(); it != tmpFieldMap.end(); ++it )
	fieldMap.insert( listColumns->index( it.key() ), *it );
}
