/****************************************************************************
** ui.h extension file, included from the uic-generated form implementation.
**
** If you wish to add, delete or rename slots use TQt Designer which will
** update this file, preserving your code. Create an init() slot in place of
** a constructor, and a destroy() slot in place of a destructor.
*****************************************************************************/
#include <ntqsqldriver.h>
#include <ntqmessagebox.h>
#include <ntqsqldatabase.h>
#include <ntqlineedit.h>
#include <ntqcombobox.h>
#include <ntqspinbox.h>
#include <ntqsqlerror.h>
#include <ntqsqlcursor.h>
#include <ntqsqlselectcursor.h>
#include <ntqdatatable.h>
#include "connect.h"

static void showError( const TQSqlError& err, TQWidget* parent = 0 )
{
   TQString errStr ( "The database reported an error\n" );
    if ( !err.databaseText().isEmpty() )
	errStr += err.databaseText();
    if ( !err.driverText().isEmpty() )
	errStr += err.driverText();
    TQMessageBox::warning( parent, "Error", errStr );
}

ConnectDialog* conDiag = 0;

void SqlEx::init()
{
    hsplit->setResizeMode( lv, TQSplitter::KeepSize );
    vsplit->setResizeMode( gb, TQSplitter::KeepSize );
    submitBtn->setEnabled( false );
    conDiag = new ConnectDialog( this, "Connection Dialog", true );    
}

void SqlEx::dbConnect()
{ 
    if ( conDiag->exec() != TQDialog::Accepted )
	return;
    if ( dt->sqlCursor() ) {
	dt->setSqlCursor( 0 );
    }
    // close old connection (if any)
    if ( TQSqlDatabase::contains( "SqlEx" ) ) {
	TQSqlDatabase* oldDb = TQSqlDatabase::database( "SqlEx" );
	oldDb->close();
	TQSqlDatabase::removeDatabase( "SqlEx" );
    }
    // open the new connection
    TQSqlDatabase* db = TQSqlDatabase::addDatabase( conDiag->comboDriver->currentText(), "SqlEx" );
    if ( !db ) {
	TQMessageBox::warning( this, "Error", "Could not open database" );	
	return;
    }
    db->setHostName( conDiag->editHostname->text() );
    db->setDatabaseName( conDiag->editDatabase->text() );
    db->setPort( conDiag->portSpinBox->value() );
    if ( !db->open( conDiag->editUsername->text(), conDiag->editPassword->text() ) ) {
	showError( db->lastError(), this );
	return;
    }
    lbl->setText( "Double-Click on a table-name to view the contents" );
    lv->clear();
    
    TQStringList tables = db->tables();
    for ( TQStringList::Iterator it = tables.begin(); it != tables.end(); ++it ) {
	TQListViewItem* lvi = new TQListViewItem( lv, *it );
	TQSqlRecordInfo ri = db->recordInfo ( *it );
	for ( TQSqlRecordInfo::Iterator it = ri.begin(); it != ri.end(); ++it ) {
	    TQString req;
	    if ( (*it).isRequired() > 0 ) {
		req = "Yes";
	    } else if ( (*it).isRequired() == 0 ) {
		req = "No";
	    } else {
		req = "?";
	    }
	    TQListViewItem* fi = new TQListViewItem( lvi, (*it).name(),  + TQVariant::typeToName( (*it).type() ), req );
	    lvi->insertItem( fi );
	}
	lv->insertItem( lvi );	
    }
    submitBtn->setEnabled( true );
}

void SqlEx::execQuery()
{
    // use a custom cursor to populate the data table
    TQSqlSelectCursor* cursor = new TQSqlSelectCursor( te->text(), TQSqlDatabase::database( "SqlEx", true ) );
    if ( cursor->isSelect() ) {
	dt->setSqlCursor( cursor, true, true );
	dt->setSort( TQStringList() );
	dt->refresh( TQDataTable::RefreshAll );
	TQString txt( "Query OK" );
	if ( cursor->size() >= 0 )
	    txt += ", returned rows: " + TQString::number( cursor->size() );
	lbl->setText( txt );
    } else {
	// an error occured if the cursor is not active
	if ( !cursor->isActive() ) {
	    showError( cursor->lastError(), this );
	} else {
	    lbl->setText( TQString("Query OK, affected rows: %1").arg( cursor->numRowsAffected() ) );
	}
    }
}

void SqlEx::showTable( TQListViewItem * item )
{
    // get the table name
    TQListViewItem* i = item->parent();
    if ( !i ) {
	i = item;
    }

    // populate the data table
    TQSqlCursor* cursor = new TQSqlCursor( i->text( 0 ), true, TQSqlDatabase::database( "SqlEx", true ) );
    dt->setSqlCursor( cursor, true, true );
    dt->setSort( cursor->primaryIndex() );
    dt->refresh( TQDataTable::RefreshAll );
    lbl->setText( "Displaying table " + i->text( 0 ) );
}
