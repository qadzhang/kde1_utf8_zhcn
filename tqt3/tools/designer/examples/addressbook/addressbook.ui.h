/****************************************************************************
** ui.h extension file, included from the uic-generated form implementation.
**
** If you wish to add, delete or rename slots use TQt Designer which will
** update this file, preserving your code. Create an init() slot in place of
** a constructor, and a destroy() slot in place of a destructor.
*****************************************************************************/

#include "addressdetails.h"
#include "search.h"
#include <ntqfile.h>
#include <ntqtextstream.h>
#include <ntqfiledialog.h>
#include <ntqapplication.h>

void AddressBook::fileNew()
{
    AddressDetails dia( this, 0, true );
    connect( &dia, TQ_SIGNAL( newAddress( const TQString &,
				       const TQString &,
				       const TQString &,
				       const TQString &,
				       const TQString &,
				       const TQString &,
				       const TQString & ) ),
	     this, TQ_SLOT( insertAddress( const TQString &,
					const TQString &,
					const TQString &,
					const TQString &,
					const TQString &,
					const TQString &,
					const TQString & ) ) );
    dia.exec();
}

void AddressBook::insertAddress( const TQString &firstName,
				 const TQString &lastName, 
				 const TQString &phoneNumber,
				 const TQString &street,
				 const TQString &city,
				 const TQString &country,
				 const TQString &zipCode )
{
    TQListViewItem *i = new TQListViewItem( addressView );
    i->setText( 0, firstName );
    i->setText( 1, lastName );
    i->setText( 2, phoneNumber );
    i->setText( 3, street );
    i->setText( 4, city );
    i->setText( 5, country );
    i->setText( 6, zipCode );
}

void AddressBook::deleteAddress()
{
    TQListViewItemIterator it( addressView );
    while ( it.current() ) {
	TQListViewItem *i = it.current();
	++it;
	if ( i->isSelected() )
	    delete i;
    }
}

void AddressBook::fileOpen()
{
    addressView->clear();
    TQString fn = TQFileDialog::getOpenFileName();
    if ( fn.isEmpty() )
	return;
    currentFileName = fn;
    TQFile f( currentFileName );
    if ( !f.open( IO_ReadOnly ) )
	return;
    TQTextStream ts( &f );
    
    while ( !ts.eof() ) {
	TQListViewItem *item = new TQListViewItem( addressView );
	for ( int i = 0; i < 7; ++i )
	    item->setText( i, ts.readLine() );
    }
	    
}

void AddressBook::fileSave()
{
    if ( currentFileName.isEmpty() )
	fileSaveAs();
    TQFile f( currentFileName );
    if ( !f.open( IO_WriteOnly ) )
	return;
    TQTextStream ts( &f );
    
    TQListViewItemIterator it( addressView );
    while ( it.current() ) {
	for ( int i = 0; i < 7; ++i )
	    ts << it.current()->text( i ) << endl;
	++it;
    }
    
    f.close();
}

void AddressBook::fileSaveAs()
{
    TQString fn = TQFileDialog::getSaveFileName();
    if ( fn.isEmpty() )
	return;
    currentFileName = fn;
    fileSave();
}

void AddressBook::fileExit()
{
    tqApp->exit();
}

void AddressBook::editFind()
{
    SearchDialog dia( this, 0, true );
    connect( &dia, TQ_SIGNAL( searchAddress( const TQString & ) ),
	     this, TQ_SLOT( searchAddress( const TQString & ) ) );
    dia.exec();
}

void AddressBook::searchAddress( const TQString &expr )
{
    addressView->clearSelection();
    TQListViewItemIterator it( addressView );
    while ( it.current() ) {
	for ( int i = 0; i < 7; ++i ) {
	    if ( it.current()->text( i ).find( expr ) != -1 )
		addressView->setSelected( it.current(), true );
	}
	++it;
    }  
}
