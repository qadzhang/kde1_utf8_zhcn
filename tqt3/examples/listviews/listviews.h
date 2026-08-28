/****************************************************************************
**
** Copyright (C) 1992-2008 Trolltech ASA.  All rights reserved.
**
** This file is part of an example program for TQt.  This example
** program may be used, distributed and modified without limitation.
**
*****************************************************************************/

#ifndef LISTVIEWS_H
#define LISTVIEWS_H

#include <ntqsplitter.h>
#include <ntqstring.h>
#include <ntqobject.h>
#include <ntqdatetime.h>
#include <ntqptrlist.h>
#include <ntqlistview.h>

class TQListView;
class TQLabel;
class TQPainter;
class TQColorGroup;
class TQObjectList;
class TQPopupMenu;

// -----------------------------------------------------------------

class MessageHeader
{
public:
    MessageHeader( const TQString &_sender, const TQString &_subject, const TQDateTime &_datetime )
	: msender( _sender ), msubject( _subject ), mdatetime( _datetime )
    {}

    MessageHeader( const MessageHeader &mh );
    MessageHeader &operator=( const MessageHeader &mh );

    TQString sender() { return msender; }
    TQString subject() { return msubject; }
    TQDateTime datetime() { return mdatetime; }

protected:
    TQString msender, msubject;
    TQDateTime mdatetime;

};

// -----------------------------------------------------------------

class Message
{
public:
    enum State { Read = 0,
		 Unread};

    Message( const MessageHeader &mh, const TQString &_body )
	: mheader( mh ), mbody( _body ), mstate( Unread )
    {}

    Message( const Message &m )
	: mheader( m.mheader ), mbody( m.mbody ), mstate( m.mstate )
    {}

    MessageHeader header() { return mheader; }
    TQString body() { return mbody; }

    void setState( const State &s ) { mstate = s; }
    State state() { return mstate; }

protected:
    MessageHeader mheader;
    TQString mbody;
    State mstate;

};

// -----------------------------------------------------------------

class Folder : public TQObject
{
    TQ_OBJECT

public:
    Folder( Folder *parent, const TQString &name );
    ~Folder()
    {}

    void addMessage( Message *m )
    { lstMessages.append( m ); }

    TQString folderName() { return fName; }

    Message *firstMessage() { return lstMessages.first(); }
    Message *nextMessage() { return lstMessages.next(); }

protected:
    TQString fName;
    TQPtrList<Message> lstMessages;

};

// -----------------------------------------------------------------

class FolderListItem : public TQListViewItem
{
public:
    FolderListItem( TQListView *parent, Folder *f );
    FolderListItem( FolderListItem *parent, Folder *f );

    void insertSubFolders( const TQObjectList *lst );

    Folder *folder() { return myFolder; }

protected:
    Folder *myFolder;

};

// -----------------------------------------------------------------

class MessageListItem : public TQListViewItem
{
public:
    MessageListItem( TQListView *parent, Message *m );

    virtual void paintCell( TQPainter *p, const TQColorGroup &cg,
			    int column, int width, int alignment );

    Message *message() { return myMessage; }

protected:
    Message *myMessage;

};

// -----------------------------------------------------------------

class ListViews : public TQSplitter
{
    TQ_OBJECT

public:
    ListViews( TQWidget *parent = 0, const char *name = 0 );
    ~ListViews()
    {}

protected:
    void initFolders();
    void initFolder( Folder *folder, unsigned int &count );
    void setupFolders();

    TQListView *messages, *folders;
    TQLabel *message;
    TQPopupMenu* menu;

    TQPtrList<Folder> lstFolders;

protected slots:
    void slotFolderChanged( TQListViewItem* );
    void slotMessageChanged();
    void slotRMB( TQListViewItem*, const TQPoint &, int );

};

#endif
