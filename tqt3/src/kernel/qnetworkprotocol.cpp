/****************************************************************************
**
** Implementation of TQNetworkProtocol class
**
** Created : 950429
**
** Copyright (C) 1992-2008 Trolltech ASA.  All rights reserved.
**
** This file is part of the kernel module of the TQt GUI Toolkit.
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
** This file may be used under the terms of the Q Public License as
** defined by Trolltech ASA and appearing in the file LICENSE.TQPL
** included in the packaging of this file.  Licensees holding valid TQt
** Commercial licenses may use this file in accordance with the TQt
** Commercial License Agreement provided with the Software.
**
** This file is provided "AS IS" with NO WARRANTY OF ANY KIND,
** INCLUDING THE WARRANTIES OF DESIGN, MERCHANTABILITY AND FITNESS FOR
** A PARTICULAR PURPOSE. Trolltech reserves all rights not granted
** herein.
**
**********************************************************************/

#include "ntqnetworkprotocol.h"

#ifndef TQT_NO_NETWORKPROTOCOL

#include "ntqlocalfs.h"
#include "ntqurloperator.h"
#include "ntqtimer.h"
#include "ntqmap.h"
#include "ntqptrqueue.h"

//#define TQNETWORKPROTOCOL_DEBUG
#define NETWORK_OP_DELAY 1000

extern TQ_EXPORT TQNetworkProtocolDict *tqNetworkProtocolRegister;

TQNetworkProtocolDict *tqNetworkProtocolRegister = 0;

class TQNetworkProtocolPrivate
{
public:
    TQNetworkProtocolPrivate( TQNetworkProtocol *p )
    {
	url = 0;
	opInProgress = 0;
	opStartTimer = new TQTimer( p );
	removeTimer = new TQTimer( p );
	operationQueue.setAutoDelete( false );
	autoDelete = false;
	removeInterval = 10000;
	oldOps.setAutoDelete( false );
    }

    ~TQNetworkProtocolPrivate()
    {
	removeTimer->stop();
	if ( opInProgress ) {
	    if ( opInProgress == operationQueue.head() )
		operationQueue.dequeue();
	    opInProgress->free();
	}
	while ( operationQueue.head() ) {
	    operationQueue.head()->free();
	    operationQueue.dequeue();
	}
	while ( oldOps.first() ) {
	    oldOps.first()->free();
	    oldOps.removeFirst();
	}
	delete opStartTimer;
    }

    TQUrlOperator *url;
    TQPtrQueue< TQNetworkOperation > operationQueue;
    TQNetworkOperation *opInProgress;
    TQTimer *opStartTimer, *removeTimer;
    int removeInterval;
    bool autoDelete;
    TQPtrList< TQNetworkOperation > oldOps;
};

/*!
    \class TQNetworkProtocol ntqnetworkprotocol.h
    \brief The TQNetworkProtocol class provides a common API for network protocols.
\if defined(commercial)
    It is part of the <a href="commercialeditions.html">TQt Enterprise Edition</a>.
\endif

    \module network
    \ingroup io
    \module network
    \mainclass

    This is a base class which should be used for network protocols
    implementations that can then be used in TQt (e.g. in the file
    dialog) together with the TQUrlOperator.

    The easiest way to implement a new network protocol is to
    reimplement the operation*() methods, e.g. operationGet(), etc.
    Only the supported operations should be reimplemented. To specify
    which operations are supported, also reimplement
    supportedOperations() and return an int that is OR'd together
    using the supported operations from the \l
    TQNetworkProtocol::Operation enum.

    When you implement a network protocol this way, it is important to
    emit the correct signals. Also, always emit the finished() signal
    when an operation is done (on success \e and on failure). TQt
    relies on correctly emitted finished() signals.

    For a detailed description of the TQt Network Architecture and how
    to implement and use network protocols in TQt, see the \link
    network.html TQt Network Documentation\endlink.
*/

/*!
    \fn void TQNetworkProtocol::newChildren( const TQValueList<TQUrlInfo> &i, TQNetworkOperation *op )

    This signal is emitted after listChildren() was called and new
    children (files) have been read from the list of files. \a i holds
    the information about the new children. \a op is the pointer to
    the operation object which contains all the information about the
    operation, including the state, etc.

    When a protocol emits this signal, TQNetworkProtocol is smart
    enough to let the TQUrlOperator, which is used by the network
    protocol, emit its corresponding signal.

    When implementing your own network protocol and reading children,
    you usually don't read one child at once, but rather a list of
    them. That's why this signal takes a list of TQUrlInfo objects. If
    you prefer to read just one child at a time you can use the
    convenience signal newChild(), which takes a single TQUrlInfo
    object.
*/

/*!
    \fn void TQNetworkProtocol::newChild( const TQUrlInfo &i, TQNetworkOperation *op )

    This signal is emitted if a new child (file) has been read.
    TQNetworkProtocol automatically connects it to a slot which creates
    a list of TQUrlInfo objects (with just one TQUrlInfo \a i) and emits
    the newChildren() signal with this list. \a op is the pointer to
    the operation object which contains all the information about the
    operation that has finished, including the state, etc.

    This is just a convenience signal useful for implementing your own
    network protocol. In all other cases connect to the newChildren()
    signal with its list of TQUrlInfo objects.
*/

/*!
    \fn void TQNetworkProtocol::finished( TQNetworkOperation *op )

    This signal is emitted when an operation finishes. This signal is
    always emitted, for both success and failure. \a op is the pointer
    to the operation object which contains all the information about
    the operation, including the state, etc. Check the state and error
    code of the operation object to determine whether or not the
    operation was successful.

    When a protocol emits this signal, TQNetworkProtocol is smart
    enough to let the TQUrlOperator, which is used by the network
    protocol, emit its corresponding signal.
*/

/*!
    \fn void TQNetworkProtocol::start( TQNetworkOperation *op )

    Some operations (such as listChildren()) emit this signal when
    they start processing the operation. \a op is the pointer to the
    operation object which contains all the information about the
    operation, including the state, etc.

    When a protocol emits this signal, TQNetworkProtocol is smart
    enough to let the TQUrlOperator, which is used by the network
    protocol, emit its corresponding signal.
*/

/*!
    \fn void TQNetworkProtocol::createdDirectory( const TQUrlInfo &i, TQNetworkOperation *op )

    This signal is emitted when mkdir() has been succesful and the
    directory has been created. \a i holds the information about the
    new directory. \a op is the pointer to the operation object which
    contains all the information about the operation, including the
    state, etc. Using op->arg( 0 ), you can get the file name of the
    new directory.

    When a protocol emits this signal, TQNetworkProtocol is smart
    enough to let the TQUrlOperator, which is used by the network
    protocol, emit its corresponding signal.
*/

/*!
    \fn void TQNetworkProtocol::removed( TQNetworkOperation *op )

    This signal is emitted when remove() has been succesful and the
    file has been removed. \a op holds the file name of the removed
    file in the first argument, accessible with op->arg( 0 ). \a op is
    the pointer to the operation object which contains all the
    information about the operation, including the state, etc.

    When a protocol emits this signal, TQNetworkProtocol is smart
    enough to let the TQUrlOperator, which is used by the network
    protocol, emit its corresponding signal.
*/

/*!
    \fn void TQNetworkProtocol::itemChanged( TQNetworkOperation *op )

    This signal is emitted whenever a file which is a child of this
    URL has been changed, e.g. by successfully calling rename(). \a op
    holds the original and the new file names in the first and second
    arguments, accessible with op->arg( 0 ) and op->arg( 1 )
    respectively. \a op is the pointer to the operation object which
    contains all the information about the operation, including the
    state, etc.

    When a protocol emits this signal, TQNetworkProtocol is smart
    enough to let the TQUrlOperator, which is used by the network
    protocol, emit its corresponding signal.
*/

/*!
    \fn void TQNetworkProtocol::data( const TQByteArray &data,
    TQNetworkOperation *op )

    This signal is emitted when new \a data has been received after
    calling get() or put(). \a op holds the name of the file from
    which data is retrieved or uploaded in its first argument, and the
    (raw) data in its second argument. You can get them with
    op->arg( 0 ) and op->rawArg( 1 ). \a op is the pointer to the
    operation object, which contains all the information about the
    operation, including the state, etc.

    When a protocol emits this signal, TQNetworkProtocol is smart
    enough to let the TQUrlOperator (which is used by the network
    protocol) emit its corresponding signal.
*/

/*!
    \fn void TQNetworkProtocol::dataTransferProgress( int bytesDone, int bytesTotal, TQNetworkOperation *op )

    This signal is emitted during the transfer of data (using put() or
    get()). \a bytesDone is how many bytes of \a bytesTotal have been
    transferred. \a bytesTotal may be -1, which means that the total
    number of bytes is not known. \a op is the pointer to the
    operation object which contains all the information about the
    operation, including the state, etc.

    When a protocol emits this signal, TQNetworkProtocol is smart
    enough to let the TQUrlOperator, which is used by the network
    protocol, emit its corresponding signal.
*/

/*!
    \fn void TQNetworkProtocol::connectionStateChanged( int state, const TQString &data )

    This signal is emitted whenever the state of the connection of the
    network protocol is changed. \a state describes the new state,
    which is one of, \c ConHostFound, \c ConConnected or \c ConClosed.
    \a data is a message text.
*/

/*!
    \enum TQNetworkProtocol::State

    This enum contains the state that a TQNetworkOperation can have.

    \value StWaiting  The operation is in the TQNetworkProtocol's queue
    waiting to be prcessed.

    \value StInProgress  The operation is being processed.

    \value StDone  The operation has been processed succesfully.

    \value StFailed  The operation has been processed but an error occurred.

    \value StStopped  The operation has been processed but has been
    stopped before it finished, and is waiting to be processed.

*/

/*!
    \enum TQNetworkProtocol::Operation

    This enum lists the possible operations that a network protocol
    can support. supportedOperations() returns an int of these that is
    OR'd together. Also, the type() of a TQNetworkOperation is always
    one of these values.

    \value OpListChildren  List the children of a URL, e.g. of a directory.
    \value OpMkDir  Create a directory.
    \value OpRemove  Remove a child (e.g. a file).
    \value OpRename  Rename a child (e.g. a file).
    \value OpGet  Get data from a location.
    \value OpPut  Put data to a location.
*/

/*!
    \enum TQNetworkProtocol::ConnectionState

    When the connection state of a network protocol changes it emits
    the signal connectionStateChanged(). The first argument is one of
    the following values:

    \value ConHostFound  Host has been found.
    \value ConConnected  Connection to the host has been established.
    \value ConClosed  Connection has been closed.
*/

/*!
    \enum TQNetworkProtocol::Error

    When an operation fails (finishes unsuccessfully), the
    TQNetworkOperation of the operation returns an error code which has
    one of the following values:

    \value NoError  No error occurred.

    \value ErrValid  The URL you are operating on is not valid.

    \value ErrUnknownProtocol  There is no protocol implementation
    available for the protocol of the URL you are operating on (e.g.
    if the protocol is http and no http implementation has been
    registered).

    \value ErrUnsupported  The operation is not supported by the
    protocol.

    \value ErrParse  The URL could not be parsed correctly.

    \value ErrLoginIncorrect  You needed to login but the username
    or password is wrong.

    \value ErrHostNotFound  The specified host (in the URL) couldn't
    be found.

    \value ErrListChildren  An error occurred while listing the
    children (files).

    \value ErrMkDir  An error occurred when creating a directory.

    \value ErrRemove  An error occurred when removing a child (file).

    \value ErrRename   An error occurred when renaming a child (file).

    \value ErrGet  An error occurred while getting (retrieving) data.

    \value ErrPut  An error occurred while putting (uploading) data.

    \value ErrFileNotExisting  A file which is needed by the operation
    doesn't exist.

    \value ErrPermissionDenied  Permission for doing the operation has
    been denied.

    You should also use these error codes when implementing custom
    network protocols. If this is not possible, you can define your own
    error codes by using integer values that don't conflict with any
    of these values.
*/

TQNetworkProtocolFactoryBase::TQNetworkProtocolFactoryBase() {
	//
}

TQNetworkProtocolFactoryBase::~TQNetworkProtocolFactoryBase() {
	//
}

/*!
    Constructor of the network protocol base class. Does some
    initialization and connecting of signals and slots.
*/

TQNetworkProtocol::TQNetworkProtocol()
    : TQObject()
{
    d = new TQNetworkProtocolPrivate( this );

    connect( d->opStartTimer, TQ_SIGNAL( timeout() ),
	     this, TQ_SLOT( startOps() ) );
    connect( d->removeTimer, TQ_SIGNAL( timeout() ),
	     this, TQ_SLOT( removeMe() ) );

    if ( url() ) {
	connect( this, TQ_SIGNAL( data(const TQByteArray&,TQNetworkOperation*) ),
		 url(), TQ_SIGNAL( data(const TQByteArray&,TQNetworkOperation*) ) );
	connect( this, TQ_SIGNAL( finished(TQNetworkOperation*) ),
		 url(), TQ_SIGNAL( finished(TQNetworkOperation*) ) );
	connect( this, TQ_SIGNAL( start(TQNetworkOperation*) ),
		 url(), TQ_SIGNAL( start(TQNetworkOperation*) ) );
	connect( this, TQ_SIGNAL( newChildren(const TQValueList<TQUrlInfo>&,TQNetworkOperation*) ),
		 url(), TQ_SIGNAL( newChildren(const TQValueList<TQUrlInfo>&,TQNetworkOperation*) ) );
	connect( this, TQ_SIGNAL( newChildren(const TQValueList<TQUrlInfo>&,TQNetworkOperation*) ),
		 url(), TQ_SLOT( addEntry(const TQValueList<TQUrlInfo>&) ) );
	connect( this, TQ_SIGNAL( createdDirectory(const TQUrlInfo&,TQNetworkOperation*) ),
		 url(), TQ_SIGNAL( createdDirectory(const TQUrlInfo&,TQNetworkOperation*) ) );
	connect( this, TQ_SIGNAL( removed(TQNetworkOperation*) ),
		 url(), TQ_SIGNAL( removed(TQNetworkOperation*) ) );
	connect( this, TQ_SIGNAL( itemChanged(TQNetworkOperation*) ),
		 url(), TQ_SIGNAL( itemChanged(TQNetworkOperation*) ) );
	connect( this, TQ_SIGNAL( dataTransferProgress(int,int,TQNetworkOperation*) ),
		 url(), TQ_SIGNAL( dataTransferProgress(int,int,TQNetworkOperation*) ) );
	connect( this, TQ_SIGNAL( connectionStateChanged(int,const TQString&) ),
		 url(), TQ_SIGNAL( connectionStateChanged(int,const TQString&) ) );
    }

    connect( this, TQ_SIGNAL( finished(TQNetworkOperation*) ),
	     this, TQ_SLOT( processNextOperation(TQNetworkOperation*) ) );
    connect( this, TQ_SIGNAL( newChild(const TQUrlInfo&,TQNetworkOperation*) ),
	     this, TQ_SLOT( emitNewChildren(const TQUrlInfo&,TQNetworkOperation*) ) );

}

/*!
    Destructor.
*/

TQNetworkProtocol::~TQNetworkProtocol()
{
    delete d;
}

/*!
    Sets the TQUrlOperator, on which the protocol works, to \a u.

    \sa TQUrlOperator
*/

void TQNetworkProtocol::setUrl( TQUrlOperator *u )
{
    if ( url() ) {
	disconnect( this, TQ_SIGNAL( data(const TQByteArray&,TQNetworkOperation*) ),
		    url(), TQ_SIGNAL( data(const TQByteArray&,TQNetworkOperation*) ) );
	disconnect( this, TQ_SIGNAL( finished(TQNetworkOperation*) ),
		    url(), TQ_SIGNAL( finished(TQNetworkOperation*) ) );
	disconnect( this, TQ_SIGNAL( start(TQNetworkOperation*) ),
		    url(), TQ_SIGNAL( start(TQNetworkOperation*) ) );
	disconnect( this, TQ_SIGNAL( newChildren(const TQValueList<TQUrlInfo>&,TQNetworkOperation*) ),
		    url(), TQ_SIGNAL( newChildren(const TQValueList<TQUrlInfo>&,TQNetworkOperation*) ) );
	disconnect( this, TQ_SIGNAL( newChildren(const TQValueList<TQUrlInfo>&,TQNetworkOperation*) ),
		    url(), TQ_SLOT( addEntry(const TQValueList<TQUrlInfo>&) ) );
	disconnect( this, TQ_SIGNAL( createdDirectory(const TQUrlInfo&,TQNetworkOperation*) ),
		    url(), TQ_SIGNAL( createdDirectory(const TQUrlInfo&,TQNetworkOperation*) ) );
	disconnect( this, TQ_SIGNAL( removed(TQNetworkOperation*) ),
		    url(), TQ_SIGNAL( removed(TQNetworkOperation*) ) );
	disconnect( this, TQ_SIGNAL( itemChanged(TQNetworkOperation*) ),
		    url(), TQ_SIGNAL( itemChanged(TQNetworkOperation*) ) );
	disconnect( this, TQ_SIGNAL( dataTransferProgress(int,int,TQNetworkOperation*) ),
		    url(), TQ_SIGNAL( dataTransferProgress(int,int,TQNetworkOperation*) ) );
	disconnect( this, TQ_SIGNAL( connectionStateChanged(int,const TQString&) ),
		    url(), TQ_SIGNAL( connectionStateChanged(int,const TQString&) ) );
    }


    // ### if autoDelete is true, we should delete the TQUrlOperator (something
    // like below; but that is not possible since it would delete this, too).
    //if ( d->autoDelete && (d->url!=u) ) {
    //    delete d->url; // destructor deletes the network protocol
    //}
    d->url = u;

    if ( url() ) {
	connect( this, TQ_SIGNAL( data(const TQByteArray&,TQNetworkOperation*) ),
		 url(), TQ_SIGNAL( data(const TQByteArray&,TQNetworkOperation*) ) );
	connect( this, TQ_SIGNAL( finished(TQNetworkOperation*) ),
		 url(), TQ_SIGNAL( finished(TQNetworkOperation*) ) );
	connect( this, TQ_SIGNAL( start(TQNetworkOperation*) ),
		 url(), TQ_SIGNAL( start(TQNetworkOperation*) ) );
	connect( this, TQ_SIGNAL( newChildren(const TQValueList<TQUrlInfo>&,TQNetworkOperation*) ),
		 url(), TQ_SIGNAL( newChildren(const TQValueList<TQUrlInfo>&,TQNetworkOperation*) ) );
	connect( this, TQ_SIGNAL( newChildren(const TQValueList<TQUrlInfo>&,TQNetworkOperation*) ),
		 url(), TQ_SLOT( addEntry(const TQValueList<TQUrlInfo>&) ) );
	connect( this, TQ_SIGNAL( createdDirectory(const TQUrlInfo&,TQNetworkOperation*) ),
		 url(), TQ_SIGNAL( createdDirectory(const TQUrlInfo&,TQNetworkOperation*) ) );
	connect( this, TQ_SIGNAL( removed(TQNetworkOperation*) ),
		 url(), TQ_SIGNAL( removed(TQNetworkOperation*) ) );
	connect( this, TQ_SIGNAL( itemChanged(TQNetworkOperation*) ),
		 url(), TQ_SIGNAL( itemChanged(TQNetworkOperation*) ) );
	connect( this, TQ_SIGNAL( dataTransferProgress(int,int,TQNetworkOperation*) ),
		 url(), TQ_SIGNAL( dataTransferProgress(int,int,TQNetworkOperation*) ) );
	connect( this, TQ_SIGNAL( connectionStateChanged(int,const TQString&) ),
		 url(), TQ_SIGNAL( connectionStateChanged(int,const TQString&) ) );
    }

    if ( !d->opInProgress && !d->operationQueue.isEmpty() )
	d->opStartTimer->start( 0, true );
}

/*!
    For processing operations the network protocol base class calls
    this method quite often. This should be reimplemented by new
    network protocols. It should return true if the connection is OK
    (open); otherwise it should return false. If the connection is not
    open the protocol should open it.

    If the connection can't be opened (e.g. because you already tried
    but the host couldn't be found), set the state of \a op to
    TQNetworkProtocol::StFailed and emit the finished() signal with
    this TQNetworkOperation as argument.

    \a op is the operation that needs an open connection.
*/

bool TQNetworkProtocol::checkConnection( TQNetworkOperation * )
{
    return true;
}

/*!
    Returns an int that is OR'd together using the enum values of
    \l{TQNetworkProtocol::Operation}, which describes which operations
    are supported by the network protocol. Should be reimplemented by
    new network protocols.
*/

int TQNetworkProtocol::supportedOperations() const
{
    return 0;
}

/*!
    Adds the operation \a op to the operation queue. The operation
    will be processed as soon as possible. This method returns
    immediately.
*/

void TQNetworkProtocol::addOperation( TQNetworkOperation *op )
{
#ifdef TQNETWORKPROTOCOL_DEBUG
    tqDebug( "TQNetworkOperation: addOperation: %p %d", op, op->operation() );
#endif
    d->operationQueue.enqueue( op );
    if ( !d->opInProgress )
	d->opStartTimer->start( 0, true );
}

/*!
    Static method to register a network protocol for TQt. For example,
    if you have an implementation of NNTP (called Nntp) which is
    derived from TQNetworkProtocol, call:
    \code
    TQNetworkProtocol::registerNetworkProtocol( "nntp", new TQNetworkProtocolFactory<Nntp> );
    \endcode
    after which your implementation is registered for future nntp
    operations.

    The name of the protocol is given in \a protocol and a pointer to
    the protocol factory is given in \a protocolFactory.
*/

void TQNetworkProtocol::registerNetworkProtocol( const TQString &protocol,
						TQNetworkProtocolFactoryBase *protocolFactory )
{
    if ( !tqNetworkProtocolRegister ) {
	tqNetworkProtocolRegister = new TQNetworkProtocolDict;
	TQNetworkProtocol::registerNetworkProtocol( "file", new TQNetworkProtocolFactory< TQLocalFs > );
    }

    tqNetworkProtocolRegister->insert( protocol, protocolFactory );
}

/*!
    Static method to get a new instance of the network protocol \a
    protocol. For example, if you need to do some FTP operations, do
    the following:
    \code
    TQFtp *ftp = TQNetworkProtocol::getNetworkProtocol( "ftp" );
    \endcode
    This returns a pointer to a new instance of an ftp implementation
    or null if no protocol for ftp was registered. The ownership of
    the pointer is transferred to you, so you must delete it if you
    don't need it anymore.

    Normally you should not work directly with network protocols, so
    you will not need to call this method yourself. Instead, use
    TQUrlOperator, which makes working with network protocols much more
    convenient.

    \sa TQUrlOperator
*/

TQNetworkProtocol *TQNetworkProtocol::getNetworkProtocol( const TQString &protocol )
{
    if ( !tqNetworkProtocolRegister ) {
	tqNetworkProtocolRegister = new TQNetworkProtocolDict;
	TQNetworkProtocol::registerNetworkProtocol( "file", new TQNetworkProtocolFactory< TQLocalFs > );
    }

    if ( protocol.isNull() )
	return 0;

    TQNetworkProtocolFactoryBase *factory = tqNetworkProtocolRegister->find( protocol );
    if ( factory )
	return factory->createObject();

    return 0;
}

/*!
    Returns true if the only protocol registered is for working on the
    local filesystem; returns false if other network protocols are
    also registered.
*/

bool TQNetworkProtocol::hasOnlyLocalFileSystem()
{
    if ( !tqNetworkProtocolRegister )
	return false;

    TQDictIterator< TQNetworkProtocolFactoryBase > it( *tqNetworkProtocolRegister );
    for ( ; it.current(); ++it )
	if ( it.currentKey() != "file" )
	    return false;
    return true;
}

/*!
  \internal
  Starts processing network operations.
*/

void TQNetworkProtocol::startOps()
{
#ifdef TQNETWORKPROTOCOL_DEBUG
    tqDebug( "TQNetworkOperation: start processing operations" );
#endif
    processNextOperation( 0 );
}

/*!
  \internal
  Processes the operation \a op. It calls the
  corresponding operation[something]( TQNetworkOperation * )
  methods.
*/

void TQNetworkProtocol::processOperation( TQNetworkOperation *op )
{
    if ( !op )
	return;

    switch ( op->operation() ) {
    case OpListChildren:
	operationListChildren( op );
	break;
    case OpMkDir:
	operationMkDir( op );
	break;
    case OpRemove:
	operationRemove( op );
	break;
    case OpRename:
	operationRename( op );
	break;
    case OpGet:
	operationGet( op );
	break;
    case OpPut:
	operationPut( op );
	break;
    }
}

/*!
    When implementing a new network protocol, this method should be
    reimplemented if the protocol supports listing children (files);
    this method should then process this TQNetworkOperation.

    When you reimplement this method it's very important that you emit
    the correct signals at the correct time (especially the finished()
    signal after processing an operation). Take a look at the \link
    network.html TQt Network Documentation\endlink which describes in
    detail how to reimplement this method. You may also want to look
    at the example implementation in
    examples/network/networkprotocol/nntp.cpp.

    \a op is the pointer to the operation object which contains all
    the information on the operation that has finished, including the
    state, etc.
*/

void TQNetworkProtocol::operationListChildren( TQNetworkOperation * )
{
}

/*!
    When implementing a new network protocol, this method should be
    reimplemented if the protocol supports making directories; this
    method should then process this TQNetworkOperation.

    When you reimplement this method it's very important that you emit
    the correct signals at the correct time (especially the finished()
    signal after processing an operation). Take a look at the \link
    network.html TQt Network Documentation\endlink which describes in
    detail how to reimplement this method. You may also want to look
    at the example implementation in
    examples/network/networkprotocol/nntp.cpp.

    \a op is the pointer to the operation object which contains all
    the information on the operation that has finished, including the
    state, etc.
*/

void TQNetworkProtocol::operationMkDir( TQNetworkOperation * )
{
}

/*!
    When implementing a new network protocol, this method should be
    reimplemented if the protocol supports removing children (files);
    this method should then process this TQNetworkOperation.

    When you reimplement this method it's very important that you emit
    the correct signals at the correct time (especially the finished()
    signal after processing an operation). Take a look at the \link
    network.html TQt Network Documentation\endlink which is describes
    in detail how to reimplement this method. You may also want to
    look at the example implementation in
    examples/network/networkprotocol/nntp.cpp.

    \a op is the pointer to the operation object which contains all
    the information on the operation that has finished, including the
    state, etc.
*/

void TQNetworkProtocol::operationRemove( TQNetworkOperation * )
{
}

/*!
    When implementing a new newtork protocol, this method should be
    reimplemented if the protocol supports renaming children (files);
    this method should then process this TQNetworkOperation.

    When you reimplement this method it's very important that you emit
    the correct signals at the correct time (especially the finished()
    signal after processing an operation). Take a look at the \link
    network.html TQt Network Documentation\endlink which describes in
    detail how to reimplement this method. You may also want to look
    at the example implementation in
    examples/network/networkprotocol/nntp.cpp.

    \a op is the pointer to the operation object which contains all
    the information on the operation that has finished, including the
    state, etc.
*/

void TQNetworkProtocol::operationRename( TQNetworkOperation * )
{
}

/*!
    When implementing a new network protocol, this method should be
    reimplemented if the protocol supports getting data; this method
    should then process the TQNetworkOperation.

    When you reimplement this method it's very important that you emit
    the correct signals at the correct time (especially the finished()
    signal after processing an operation). Take a look at the \link
    network.html TQt Network Documentation\endlink which describes in
    detail how to reimplement this method. You may also want to look
    at the example implementation in
    examples/network/networkprotocol/nntp.cpp.

    \a op is the pointer to the operation object which contains all
    the information on the operation that has finished, including the
    state, etc.
*/

void TQNetworkProtocol::operationGet( TQNetworkOperation * )
{
}

/*!
    When implementing a new network protocol, this method should be
    reimplemented if the protocol supports putting (uploading) data;
    this method should then process the TQNetworkOperation.

    When you reimplement this method it's very important that you emit
    the correct signals at the correct time (especially the finished()
    signal after processing an operation). Take a look at the \link
    network.html TQt Network Documentation\endlink which describes in
    detail how to reimplement this method. You may also want to look
    at the example implementation in
    examples/network/networkprotocol/nntp.cpp.

    \a op is the pointer to the operation object which contains all
    the information on the operation that has finished, including the
    state, etc.
*/

void TQNetworkProtocol::operationPut( TQNetworkOperation * )
{
}

/*! \internal
*/

void TQNetworkProtocol::operationPutChunk( TQNetworkOperation * )
{
}

/*!
  \internal
  Handles operations. Deletes the previous operation object and
  tries to process the next operation. It also checks the connection state
  and only processes the next operation, if the connection of the protocol
  is open. Otherwise it waits until the protocol opens the connection.
*/

void TQNetworkProtocol::processNextOperation( TQNetworkOperation *old )
{
#ifdef TQNETWORKPROTOCOL_DEBUG
    tqDebug( "TQNetworkOperation: process next operation, old: %p", old );
#endif
    d->removeTimer->stop();

    if ( old )
	d->oldOps.append( old );
    if ( d->opInProgress && d->opInProgress!=old )
	d->oldOps.append( d->opInProgress );

    if ( d->operationQueue.isEmpty() ) {
	d->opInProgress = 0;
	if ( d->autoDelete )
	    d->removeTimer->start( d->removeInterval, true );
	return;
    }

    TQNetworkOperation *op = d->operationQueue.head();

    d->opInProgress = op;

    if ( !checkConnection( op ) ) {
	if ( op->state() != TQNetworkProtocol::StFailed ) {
	    d->opStartTimer->start( 0, true );
	} else {
	    d->operationQueue.dequeue();
	    clearOperationQueue();
	    emit finished( op );
	}

	return;
    }

    d->opInProgress = op;
    d->operationQueue.dequeue();
    processOperation( op );
}

/*!
    Returns the TQUrlOperator on which the protocol works.
*/

TQUrlOperator *TQNetworkProtocol::url() const
{
    return d->url;
}

/*!
    Returns the operation, which is being processed, or 0 of no
    operation is being processed at the moment.
*/

TQNetworkOperation *TQNetworkProtocol::operationInProgress() const
{
    return d->opInProgress;
}

/*!
    Clears the operation queue.
*/

void TQNetworkProtocol::clearOperationQueue()
{
    d->operationQueue.dequeue();
    d->operationQueue.setAutoDelete( true );
    d->operationQueue.clear();
}

/*!
    Stops the current operation that is being processed and clears all
    waiting operations.
*/

void TQNetworkProtocol::stop()
{
    TQNetworkOperation *op = d->opInProgress;
    clearOperationQueue();
    if ( op ) {
	op->setState( StStopped );
	op->setProtocolDetail( tr( "Operation stopped by the user" ) );
	emit finished( op );
	setUrl( 0 );
	op->free();
    }
}

/*!
    Because it's sometimes hard to take care of removing network
    protocol instances, TQNetworkProtocol provides an auto-delete
    mechanism. If you set \a b to true, the network protocol instance
    is removed after it has been inactive for \a i milliseconds (i.e.
    \a i milliseconds after the last operation has been processed).
    If you set \a b to false the auto-delete mechanism is switched
    off.

    If you switch on auto-delete, the TQNetworkProtocol also deletes
    its TQUrlOperator.
*/

void TQNetworkProtocol::setAutoDelete( bool b, int i )
{
    d->autoDelete = b;
    d->removeInterval = i;
}

/*!
    Returns true if auto-deleting is enabled; otherwise returns false.

    \sa TQNetworkProtocol::setAutoDelete()
*/

bool TQNetworkProtocol::autoDelete() const
{
    return d->autoDelete;
}

/*!
  \internal
*/

void TQNetworkProtocol::removeMe()
{
    if ( d->autoDelete ) {
#ifdef TQNETWORKPROTOCOL_DEBUG
	tqDebug( "TQNetworkOperation:  autodelete of TQNetworkProtocol %p", this );
#endif
	delete d->url; // destructor deletes the network protocol
    }
}

void TQNetworkProtocol::emitNewChildren( const TQUrlInfo &i, TQNetworkOperation *op )
{
    TQValueList<TQUrlInfo> lst;
    lst << i;
    emit newChildren( lst, op );
}

class TQNetworkOperationPrivate
{
public:
    TQNetworkProtocol::Operation operation;
    TQNetworkProtocol::State state;
    TQMap<int, TQString> args;
    TQMap<int, TQByteArray> rawArgs;
    TQString protocolDetail;
    int errorCode;
    TQTimer *deleteTimer;
};

/*!
    \class TQNetworkOperation

    \brief The TQNetworkOperation class provides common operations for network protocols.
\if defined(commercial)
    It is part of the <a href="commercialeditions.html">TQt Enterprise Edition</a>.
\endif

    \module network
    \ingroup io

    An object is created to describe the operation and the current
    state for each operation that a network protocol should process.

    For a detailed description of the TQt Network Architecture and how
    to implement and use network protocols in TQt, see the \link
    network.html TQt Network Documentation\endlink.

    \sa TQNetworkProtocol
*/

/*!
    Constructs a network operation object. \a operation is the type of
    the operation, and \a arg0, \a arg1 and \a arg2 are the first
    three arguments of the operation. The state is initialized to
    TQNetworkProtocol::StWaiting.

    \sa TQNetworkProtocol::Operation TQNetworkProtocol::State
*/

TQNetworkOperation::TQNetworkOperation( TQNetworkProtocol::Operation operation,
				      const TQString &arg0, const TQString &arg1,
				      const TQString &arg2 )
{
    d = new TQNetworkOperationPrivate;
    d->deleteTimer = new TQTimer( this );
    connect( d->deleteTimer, TQ_SIGNAL( timeout() ),
	     this, TQ_SLOT( deleteMe() ) );
    d->operation = operation;
    d->state = TQNetworkProtocol::StWaiting;
    d->args[ 0 ] = arg0;
    d->args[ 1 ] = arg1;
    d->args[ 2 ] = arg2;
    d->rawArgs[ 0 ] = TQByteArray( 0 );
    d->rawArgs[ 1 ] = TQByteArray( 0 );
    d->rawArgs[ 2 ] = TQByteArray( 0 );
    d->protocolDetail = TQString::null;
    d->errorCode = (int)TQNetworkProtocol::NoError;
}

/*!
    Constructs a network operation object. \a operation is the type of
    the operation, and \a arg0, \a arg1 and \a arg2 are the first
    three raw data arguments of the operation. The state is
    initialized to TQNetworkProtocol::StWaiting.

    \sa TQNetworkProtocol::Operation TQNetworkProtocol::State
*/

TQNetworkOperation::TQNetworkOperation( TQNetworkProtocol::Operation operation,
				      const TQByteArray &arg0, const TQByteArray &arg1,
				      const TQByteArray &arg2 )
{
    d = new TQNetworkOperationPrivate;
    d->deleteTimer = new TQTimer( this );
    connect( d->deleteTimer, TQ_SIGNAL( timeout() ),
	     this, TQ_SLOT( deleteMe() ) );
    d->operation = operation;
    d->state = TQNetworkProtocol::StWaiting;
    d->args[ 0 ] = TQString::null;
    d->args[ 1 ] = TQString::null;
    d->args[ 2 ] = TQString::null;
    d->rawArgs[ 0 ] = arg0;
    d->rawArgs[ 1 ] = arg1;
    d->rawArgs[ 2 ] = arg2;
    d->protocolDetail = TQString::null;
    d->errorCode = (int)TQNetworkProtocol::NoError;
}

/*!
    Destructor.
*/

TQNetworkOperation::~TQNetworkOperation()
{
    delete d;
}

/*!
    Sets the \a state of the operation object. This should be done by
    the network protocol during processing; at the end it should be
    set to TQNetworkProtocol::StDone or TQNetworkProtocol::StFailed,
    depending on success or failure.

    \sa TQNetworkProtocol::State
*/

void TQNetworkOperation::setState( TQNetworkProtocol::State state )
{
    if ( d->deleteTimer->isActive() ) {
	d->deleteTimer->stop();
	d->deleteTimer->start( NETWORK_OP_DELAY );
    }
    d->state = state;
}

/*!
    If the operation failed, the error message can be specified as \a
    detail.
*/

void TQNetworkOperation::setProtocolDetail( const TQString &detail )
{
    if ( d->deleteTimer->isActive() ) {
	d->deleteTimer->stop();
	d->deleteTimer->start( NETWORK_OP_DELAY );
    }
    d->protocolDetail = detail;
}

/*!
    Sets the error code to \a ec.

    If the operation failed, the protocol should set an error code to
    describe the error in more detail. If possible, one of the error
    codes defined in TQNetworkProtocol should be used.

    \sa setProtocolDetail() TQNetworkProtocol::Error
*/

void TQNetworkOperation::setErrorCode( int ec )
{
    if ( d->deleteTimer->isActive() ) {
	d->deleteTimer->stop();
	d->deleteTimer->start( NETWORK_OP_DELAY );
    }
    d->errorCode = ec;
}

/*!
    Sets the network operation's \a{num}-th argument to \a arg.
*/

void TQNetworkOperation::setArg( int num, const TQString &arg )
{
    if ( d->deleteTimer->isActive() ) {
	d->deleteTimer->stop();
	d->deleteTimer->start( NETWORK_OP_DELAY );
    }
    d->args[ num ] = arg;
}

/*!
    Sets the network operation's \a{num}-th raw data argument to \a arg.
*/

void TQNetworkOperation::setRawArg( int num, const TQByteArray &arg )
{
    if ( d->deleteTimer->isActive() ) {
	d->deleteTimer->stop();
	d->deleteTimer->start( NETWORK_OP_DELAY );
    }
    d->rawArgs[ num ] = arg;
}

/*!
    Returns the type of the operation.
*/

TQNetworkProtocol::Operation TQNetworkOperation::operation() const
{
    if ( d->deleteTimer->isActive() ) {
	d->deleteTimer->stop();
	d->deleteTimer->start( NETWORK_OP_DELAY );
    }
    return d->operation;
}

/*!
    Returns the state of the operation. You can determine whether an
    operation is still waiting to be processed, is being processed,
    has been processed successfully, or failed.
*/

TQNetworkProtocol::State TQNetworkOperation::state() const
{
    if ( d->deleteTimer->isActive() ) {
	d->deleteTimer->stop();
	d->deleteTimer->start( NETWORK_OP_DELAY );
    }
    return d->state;
}

/*!
    Returns the operation's \a{num}-th argument. If this argument was
    not already set, an empty string is returned.
*/

TQString TQNetworkOperation::arg( int num ) const
{
    if ( d->deleteTimer->isActive() ) {
	d->deleteTimer->stop();
	d->deleteTimer->start( NETWORK_OP_DELAY );
    }
    return d->args[ num ];
}

/*!
    Returns the operation's \a{num}-th raw data argument. If this
    argument was not already set, an empty bytearray is returned.
*/

TQByteArray TQNetworkOperation::rawArg( int num ) const
{
    if ( d->deleteTimer->isActive() ) {
	d->deleteTimer->stop();
	d->deleteTimer->start( NETWORK_OP_DELAY );
    }
    return d->rawArgs[ num ];
}

/*!
    Returns a detailed error message for the last error. This must
    have been set using setProtocolDetail().
*/

TQString TQNetworkOperation::protocolDetail() const
{
    if ( d->deleteTimer->isActive() ) {
	d->deleteTimer->stop();
	d->deleteTimer->start( NETWORK_OP_DELAY );
    }
    return d->protocolDetail;
}

/*!
    Returns the error code for the last error that occurred.
*/

int TQNetworkOperation::errorCode() const
{
    if ( d->deleteTimer->isActive() ) {
	d->deleteTimer->stop();
	d->deleteTimer->start( NETWORK_OP_DELAY );
    }
    return d->errorCode;
}

/*!
  \internal
*/

TQByteArray& TQNetworkOperation::raw( int num ) const
{
    if ( d->deleteTimer->isActive() ) {
	d->deleteTimer->stop();
	d->deleteTimer->start( NETWORK_OP_DELAY );
    }
    return d->rawArgs[ num ];
}

/*!
    Sets this object to delete itself when it hasn't been used for one
    second.

    Because TQNetworkOperation pointers are passed around a lot the
    TQNetworkProtocol generally does not have enough knowledge to
    delete these at the correct time. If a TQNetworkProtocol doesn't
    need an operation any more it will call this function instead.

    Note: you should never need to call the method yourself.
*/

void TQNetworkOperation::free()
{
    d->deleteTimer->start( NETWORK_OP_DELAY );
}

/*!
  \internal
  Internal slot for auto-deletion.
*/

void TQNetworkOperation::deleteMe()
{
    delete this;
}

#endif
