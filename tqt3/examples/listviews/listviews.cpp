/****************************************************************************
**
** Copyright (C) 1992-2008 Trolltech ASA.  All rights reserved.
**
** This file is part of an example program for TQt.  This example
** program may be used, distributed and modified without limitation.
**
*****************************************************************************/

#include "listviews.h"

#include <ntqlabel.h>
#include <ntqpainter.h>
#include <ntqpalette.h>
#include <ntqobjectlist.h>
#include <ntqpopupmenu.h>
#include <ntqheader.h>
#include <ntqregexp.h>

// -----------------------------------------------------------------

MessageHeader::MessageHeader( const MessageHeader &mh )
{
    msender = mh.msender;
    msubject = mh.msubject;
    mdatetime = mh.mdatetime;
}

MessageHeader &MessageHeader::operator=( const MessageHeader &mh )
{
    msender = mh.msender;
    msubject = mh.msubject;
    mdatetime = mh.mdatetime;

    return *this;
}

// -----------------------------------------------------------------

Folder::Folder( Folder *parent, const TQString &name )
    : TQObject( parent, name ), fName( name )
{
    lstMessages.setAutoDelete( true );
}

// -----------------------------------------------------------------

FolderListItem::FolderListItem( TQListView *parent, Folder *f )
    : TQListViewItem( parent )
{
    myFolder = f;
    setText( 0, f->folderName() );

    if ( myFolder->children() )
	insertSubFolders( myFolder->children() );
}

FolderListItem::FolderListItem( FolderListItem *parent, Folder *f )
    : TQListViewItem( parent )
{
    myFolder = f;

    setText( 0, f->folderName() );

    if ( myFolder->children() )
	insertSubFolders( myFolder->children() );
}

void FolderListItem::insertSubFolders( const TQObjectList *lst )
{
    Folder *f;
    for ( f = ( Folder* )( ( TQObjectList* )lst )->first(); f; f = ( Folder* )( ( TQObjectList* )lst )->next() )
	(void)new FolderListItem( this, f );
}

// -----------------------------------------------------------------

MessageListItem::MessageListItem( TQListView *parent, Message *m )
    : TQListViewItem( parent )
{
    myMessage = m;
    setText( 0, myMessage->header().sender() );
    setText( 1, myMessage->header().subject() );
    setText( 2, myMessage->header().datetime().toString() );
}

void MessageListItem::paintCell( TQPainter *p, const TQColorGroup &cg,
				 int column, int width, int alignment )
{
    TQColorGroup _cg( cg );
    TQColor c = _cg.text();

    if ( myMessage->state() == Message::Unread )
	_cg.setColor( TQColorGroup::Text, TQt::red );

    TQListViewItem::paintCell( p, _cg, column, width, alignment );

    _cg.setColor( TQColorGroup::Text, c );
}

// -----------------------------------------------------------------

ListViews::ListViews( TQWidget *parent, const char *name )
    : TQSplitter( TQt::Horizontal, parent, name )
{
    lstFolders.setAutoDelete( true );

    folders = new TQListView( this );
    folders->header()->setClickEnabled( false );
    folders->addColumn( "Folder" );

    initFolders();
    setupFolders();

    folders->setRootIsDecorated( true );
    setResizeMode( folders, TQSplitter::KeepSize );

    TQSplitter *vsplitter = new TQSplitter( TQt::Vertical, this );

    messages = new TQListView( vsplitter );
    messages->addColumn( "Sender" );
    messages->addColumn( "Subject" );
    messages->addColumn( "Date" );
    messages->setColumnAlignment( 1, TQt::AlignRight );
    messages->setAllColumnsShowFocus( true );
    messages->setShowSortIndicator( true );
    menu = new TQPopupMenu( messages );
    for( int i = 1; i <= 10; i++ )
	menu->insertItem( TQString( "Context Item %1" ).arg( i ) );
    connect(messages, TQ_SIGNAL( contextMenuRequested( TQListViewItem *, const TQPoint& , int ) ),
	    this, TQ_SLOT( slotRMB( TQListViewItem *, const TQPoint &, int ) ) );
    vsplitter->setResizeMode( messages, TQSplitter::KeepSize );

    message = new TQLabel( vsplitter );
    message->setAlignment( TQt::AlignTop );
    message->setBackgroundMode( PaletteBase );

    connect( folders, TQ_SIGNAL( selectionChanged( TQListViewItem* ) ),
	     this, TQ_SLOT( slotFolderChanged( TQListViewItem* ) ) );
    connect( messages, TQ_SIGNAL( selectionChanged() ),
	     this, TQ_SLOT( slotMessageChanged() ) );
    connect( messages, TQ_SIGNAL( currentChanged( TQListViewItem * ) ),
	     this, TQ_SLOT( slotMessageChanged() ) );

    messages->setSelectionMode( TQListView::Extended );
    // some preparations
    folders->firstChild()->setOpen( true );
    folders->firstChild()->firstChild()->setOpen( true );
    folders->setCurrentItem( folders->firstChild()->firstChild()->firstChild() );
    folders->setSelected( folders->firstChild()->firstChild()->firstChild(), true );

    messages->setSelected( messages->firstChild(), true );
    messages->setCurrentItem( messages->firstChild() );
    message->setMargin( 5 );

    TQValueList<int> lst;
    lst.append( 170 );
    setSizes( lst );
}

void ListViews::initFolders()
{
    unsigned int mcount = 1;

    for ( unsigned int i = 1; i < 20; i++ ) {
	TQString str;
	str = TQString( "Folder %1" ).arg( i );
	Folder *f = new Folder( 0, str );
	for ( unsigned int j = 1; j < 5; j++ ) {
	    TQString str2;
	    str2 = TQString( "Sub Folder %1" ).arg( j );
	    Folder *f2 = new Folder( f, str2 );
	    for ( unsigned int k = 1; k < 3; k++ ) {
		TQString str3;
		str3 = TQString( "Sub Sub Folder %1" ).arg( k );
		Folder *f3 = new Folder( f2, str3 );
		initFolder( f3, mcount );
	    }
	}
	lstFolders.append( f );
    }
}

void ListViews::initFolder( Folder *folder, unsigned int &count )
{
    for ( unsigned int i = 0; i < 15; i++, count++ ) {
	TQString str;
	str = TQString( "Message %1  " ).arg( count );
	TQDateTime dt = TQDateTime::currentDateTime();
	dt = dt.addSecs( 60 * count );
	MessageHeader mh( "Trolltech <info@trolltech.com>  ", str, dt );

	TQString body;
	body = TQString( "This is the message number %1 of this application, \n"
			"which shows how to use TQListViews, TQListViewItems, \n"
			"TQSplitters and so on. The code should show how easy\n"
			"this can be done in TQt." ).arg( count );
	Message *msg = new Message( mh, body );
	folder->addMessage( msg );
    }
}

void ListViews::setupFolders()
{
    folders->clear();

    for ( Folder* f = lstFolders.first(); f; f = lstFolders.next() )
	(void)new FolderListItem( folders, f );
}

void ListViews::slotRMB( TQListViewItem* Item, const TQPoint & point, int )
{
    if( Item )
	menu->popup( point );
}


void ListViews::slotFolderChanged( TQListViewItem *i )
{
    if ( !i )
	return;
    messages->clear();
    message->setText( "" );

    FolderListItem *item = ( FolderListItem* )i;

    for ( Message* msg = item->folder()->firstMessage(); msg;
	  msg = item->folder()->nextMessage() )
	(void)new MessageListItem( messages, msg );
}

void ListViews::slotMessageChanged()
{
    TQListViewItem *i = messages->currentItem();
    if ( !i )
	return;

    if ( !i->isSelected() ) {
	message->setText( "" );
	return;
    }

    MessageListItem *item = ( MessageListItem* )i;
    Message *msg = item->message();

    TQString text;
    TQString tmp = msg->header().sender();
    tmp = tmp.replace( "<", "&lt;" );
    tmp = tmp.replace( ">", "&gt;" );
    text = TQString( "<b><i>From:</i></b> <a href=\"mailto:info@trolltech.com\">%1</a><br>"
		    "<b><i>Subject:</i></b> <big><big><b>%2</b></big></big><br>"
		    "<b><i>Date:</i></b> %3<br><br>"
		    "%4" ).
	   arg( tmp ).arg( msg->header().subject() ).
	   arg( msg->header().datetime().toString() ).arg( msg->body() );

    message->setText( text );

    msg->setState( Message::Read );
}
