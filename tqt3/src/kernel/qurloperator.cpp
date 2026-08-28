/****************************************************************************
**
** Implementation of TQUrlOperator class
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

#include "ntqurloperator.h"

#ifndef TQT_NO_NETWORKPROTOCOL

#include "ntqurlinfo.h"
#include "ntqnetworkprotocol.h"
#include "ntqmap.h"
#include "ntqdir.h"
#include "ntqptrdict.h"
#include "ntqguardedptr.h"

//#define TQURLOPERATOR_DEBUG

class TQUrlOperatorPrivate
{
public:
    TQUrlOperatorPrivate()
    {
	oldOps.setAutoDelete( false );
	networkProtocol = 0;
	nameFilter = "*";
	currPut = 0;
    }

    ~TQUrlOperatorPrivate()
    {
	delete networkProtocol;
	while ( oldOps.first() ) {
	    oldOps.first()->free();
	    oldOps.removeFirst();
	}
    }

    TQMap<TQString, TQUrlInfo> entryMap;
    TQNetworkProtocol *networkProtocol;
    TQString nameFilter;
    TQDir dir;

    // maps needed for copy/move operations
    TQPtrDict<TQNetworkOperation> getOpPutOpMap;
    TQPtrDict<TQNetworkProtocol> getOpPutProtMap;
    TQPtrDict<TQNetworkProtocol> getOpGetProtMap;
    TQPtrDict<TQNetworkOperation> getOpRemoveOpMap;
    TQGuardedPtr<TQNetworkProtocol> currPut;
    TQStringList waitingCopies;
    TQString waitingCopiesDest;
    bool waitingCopiesMove;
    TQPtrList< TQNetworkOperation > oldOps;
};

/*!
    \class TQUrlOperator ntqurloperator.h

    \brief The TQUrlOperator class provides common operations on URLs.
\if defined(commercial)
    It is part of the <a href="commercialeditions.html">TQt Enterprise Edition</a>.
\endif

    \ingroup io
    \ingroup misc
    \mainclass

    \module network

    This class operates on hierarchical structures (such as
    filesystems) using URLs. Its API facilitates all the common
    operations:
    \table
    \header \i Operation	\i Function
    \row \i List files		\i \l listChildren()
    \row \i Make a directory	\i \l mkdir()
    \row \i Remove a file	\i \l remove()
    \row \i Rename a file	\i \l rename()
    \row \i Get a file		\i \l get()
    \row \i Put a file		\i \l put()
    \row \i Copy a file		\i \l copy()
    \endtable

    You can obtain additional information about the URL with isDir()
    and info(). If a directory is to be traversed using
    listChildren(), a name filter can be set with setNameFilter().

    A TQUrlOperator can be used like this, for example to download a
    file (and assuming that the FTP protocol is \link
    tqInitNetworkProtocols() registered\endlink):
    \code
    TQUrlOperator *op = new TQUrlOperator();
    op->copy( TQString("ftp://ftp.trolltech.com/qt/source/qt-2.1.0.tar.gz"),
	     "file:/tmp" );
    \endcode

    If you want to be notified about success/failure, progress, etc.,
    you can connect to TQUrlOperator's signals, e.g. to start(),
    newChildren(), createdDirectory(), removed(), data(),
    dataTransferProgress(), startedNextCopy(),
    connectionStateChanged(), finished(), etc. A network operation can
    be stopped with stop().

    The class uses the functionality of registered network protocols
    to perform these operations. Depending of the protocol of the URL,
    it uses an appropriate network protocol class for the operations.
    Each of the operation functions of TQUrlOperator creates a
    TQNetworkOperation object that describes the operation and puts it
    into the operation queue for the network protocol used. If no
    suitable protocol could be found (because no implementation of the
    necessary network protocol is registered), the URL operator emits
    errors. Not every protocol supports every operation, but error
    handling deals with this problem.

    To register the available network protocols, use the
    tqInitNetworkProtocols() function. The protocols currently
    supported are:
    \list
    \i \link TQFtp FTP\endlink,
    \i \link TQHttp HTTP\endlink,
    \i \link TQLocalFs local file system\endlink.
    \endlist

    For more information about the TQt Network Architecture see the
    \link network.html TQt Network Documentation\endlink.

    \sa TQNetworkProtocol, TQNetworkOperation
*/

/*!
    \fn void TQUrlOperator::newChildren( const TQValueList<TQUrlInfo> &i, TQNetworkOperation *op )

    This signal is emitted after listChildren() was called and new
    children (i.e. files) have been read from a list of files. \a i
    holds the information about the new files. \a op is a pointer
    to the operation object which contains all the information about
    the operation, including the state.

    \sa TQNetworkOperation, TQNetworkProtocol
*/


/*!
    \fn void TQUrlOperator::finished( TQNetworkOperation *op )

    This signal is emitted when an operation of some sort finishes,
    whether with success or failure. \a op is a pointer to the
    operation object, which contains all the information, including
    the state, of the operation which has been finished. Check the
    state and error code of the operation object to see whether or not
    the operation was successful.

    \sa TQNetworkOperation, TQNetworkProtocol
*/

/*!
    \fn void TQUrlOperator::start( TQNetworkOperation *op )

    Some operations (such as listChildren()) emit this signal when
    they start processing the operation. \a op is a pointer to the
    operation object which contains all the information about the
    operation, including the state.

    \sa TQNetworkOperation, TQNetworkProtocol
*/

/*!
    \fn void TQUrlOperator::createdDirectory( const TQUrlInfo &i, TQNetworkOperation *op )

    This signal is emitted when mkdir() succeeds and the directory has
    been created. \a i holds the information about the new directory.

    \a op is a pointer to the operation object, which contains all the
    information about the operation, including the state.
    \c op->arg(0) holds the new directory's name.

    \sa TQNetworkOperation, TQNetworkProtocol
*/

/*!
    \fn void TQUrlOperator::removed( TQNetworkOperation *op )

    This signal is emitted when remove() has been succesful and the
    file has been removed.

    \a op is a pointer to the operation object which contains all the
    information about the operation, including the state.
    \c op->arg(0) holds the name of the file that was removed.

    \sa TQNetworkOperation, TQNetworkProtocol
*/

/*!
    \fn void TQUrlOperator::itemChanged( TQNetworkOperation *op )

    This signal is emitted whenever a file which is a child of the URL
    has been changed, for example by successfully calling rename().
    \a op is a pointer to the operation object which contains all the
    information about the operation, including the state.
    \c op->arg(0) holds the original file name and \c op->arg(1) holds
    the new file name (if it was changed).

    \sa TQNetworkOperation, TQNetworkProtocol
*/

/*!
    \fn void TQUrlOperator::data( const TQByteArray &data, TQNetworkOperation *op )

    This signal is emitted when new \a data has been received after calling
    get() or put().
    \a op is a pointer to the operation object which contains all
    the information about the operation, including the state.
    \c op->arg(0) holds the name of the file whose data is retrieved
    and op->rawArg(1) holds the (raw) data.

    \sa TQNetworkOperation, TQNetworkProtocol
*/

/*!
    \fn void TQUrlOperator::dataTransferProgress( int bytesDone, int bytesTotal, TQNetworkOperation *op )

    This signal is emitted during data transfer (using put() or
    get()). \a bytesDone specifies how many bytes of \a bytesTotal have
    been transferred. More information about the operation is stored in
    \a op, a pointer to the network operation that is processed.
    \a bytesTotal may be -1, which means that the total number of bytes
    is not known.

    \sa TQNetworkOperation, TQNetworkProtocol
*/

/*!
    \fn void TQUrlOperator::startedNextCopy( const TQPtrList<TQNetworkOperation> &lst )

    This signal is emitted if copy() starts a new copy operation. \a
    lst contains all TQNetworkOperations related to this copy
    operation.

    \sa copy()
*/

/*!
    \fn void TQUrlOperator::connectionStateChanged( int state, const TQString &data )

    This signal is emitted whenever the URL operator's connection
    state changes. \a state describes the new state, which is a
    \l{TQNetworkProtocol::ConnectionState} value.

    \a data is a string that describes the change of the connection.
    This can be used to display a message to the user.
*/

/*!
    Constructs a TQUrlOperator with an empty (i.e. invalid) URL.
*/

TQUrlOperator::TQUrlOperator()
    : TQUrl()
{
#ifdef TQURLOPERATOR_DEBUG
    tqDebug( "TQUrlOperator: cstr 1" );
#endif
    d = new TQUrlOperatorPrivate;
}

/*!
    Constructs a TQUrlOperator using \a url and parses this string.

    If you pass strings like "/home/qt" the "file" protocol is
    assumed.
*/

TQUrlOperator::TQUrlOperator( const TQString &url )
    : TQUrl( url )
{
#ifdef TQURLOPERATOR_DEBUG
    tqDebug( "TQUrlOperator: cstr 2" );
#endif
    d = new TQUrlOperatorPrivate;
    getNetworkProtocol();
}

/*!
    Constructs a copy of \a url.
*/

TQUrlOperator::TQUrlOperator( const TQUrlOperator& url )
    : TQObject(), TQUrl( url )
{
#ifdef TQURLOPERATOR_DEBUG
    tqDebug( "TQUrlOperator: cstr 3" );
#endif
    d = new TQUrlOperatorPrivate;
    *d = *url.d;

    d->networkProtocol = 0;
    getNetworkProtocol();
    d->nameFilter = "*";
    d->currPut = 0;
}

/*!
    Constructs a TQUrlOperator. The URL on which this TQUrlOperator
    operates is constructed out of the arguments \a url, \a relUrl and
    \a checkSlash: see the corresponding TQUrl constructor for an
    explanation of these arguments.
*/

TQUrlOperator::TQUrlOperator( const TQUrlOperator& url, const TQString& relUrl, bool checkSlash )
    : TQUrl( url, relUrl, checkSlash )
{
#ifdef TQURLOPERATOR_DEBUG
    tqDebug( "TQUrlOperator: cstr 4" );
#endif
    d = new TQUrlOperatorPrivate;
    if ( relUrl == "." )
	*d = *url.d;

    d->networkProtocol = 0;
    getNetworkProtocol();
    d->currPut = 0;
}

/*!
    Destructor.
*/

TQUrlOperator::~TQUrlOperator()
{
#ifdef TQURLOPERATOR_DEBUG
    tqDebug( "TQUrlOperator: dstr" );
#endif
    delete d;
}

/*!
    This private function is used by the simple operation functions,
    i.e. listChildren(), mkdir(), remove(), rename(), get() and put(),
    to really start the operation. \a op is a pointer to the network
    operation that should be started. Returns \a op on success;
    otherwise returns 0.
*/
const TQNetworkOperation *TQUrlOperator::startOperation( TQNetworkOperation *op )
{
    if ( !d->networkProtocol )
        getNetworkProtocol();
    
    if ( d->networkProtocol && (d->networkProtocol->supportedOperations()&op->operation()) ) {
	d->networkProtocol->addOperation( op );
	if ( op->operation() == TQNetworkProtocol::OpListChildren )
	    clearEntries();
	return op;
    }

    // error
    TQString msg;
    if ( !d->networkProtocol ) {
	msg = tr( "The protocol `%1' is not supported" ).arg( protocol() );
    } else {
	switch ( op->operation() ) {
	case TQNetworkProtocol::OpListChildren:
	    msg = tr( "The protocol `%1' does not support listing directories" ).arg( protocol() );
	    break;
	case TQNetworkProtocol::OpMkDir:
	    msg = tr( "The protocol `%1' does not support creating new directories" ).arg( protocol() );
	    break;
	case TQNetworkProtocol::OpRemove:
	    msg = tr( "The protocol `%1' does not support removing files or directories" ).arg( protocol() );
	    break;
	case TQNetworkProtocol::OpRename:
	    msg = tr( "The protocol `%1' does not support renaming files or directories" ).arg( protocol() );
	    break;
	case TQNetworkProtocol::OpGet:
	    msg = tr( "The protocol `%1' does not support getting files" ).arg( protocol() );
	    break;
	case TQNetworkProtocol::OpPut:
	    msg = tr( "The protocol `%1' does not support putting files" ).arg( protocol() );
	    break;
	default:
	    // this should never happen
	    break;
	}
    }
    op->setState( TQNetworkProtocol::StFailed );
    op->setProtocolDetail( msg );
    op->setErrorCode( (int)TQNetworkProtocol::ErrUnsupported );
    emit finished( op );
    deleteOperation( op );
    return 0;
}

/*!
    Starts listing the children of this URL (e.g. the files in the
    directory). The start() signal is emitted before the first entry
    is listed and finished() is emitted after the last one. The
    newChildren() signal is emitted for each list of new entries. If
    an error occurs, the signal finished() is emitted, so be sure to
    check the state of the network operation pointer.

    Because the operation may not be executed immediately, a pointer
    to the TQNetworkOperation object created by this function is
    returned. This object contains all the data about the operation
    and is used to refer to this operation later (e.g. in the signals
    that are emitted by the TQUrlOperator). The return value can also
    be 0 if the operation object couldn't be created.

    The path of this TQUrlOperator must to point to a directory
    (because the children of this directory will be listed), not to a
    file.
*/

const TQNetworkOperation *TQUrlOperator::listChildren()
{
    if ( !checkValid() )
	return 0;

    TQNetworkOperation *res = new TQNetworkOperation( TQNetworkProtocol::OpListChildren, TQString::null, TQString::null, TQString::null );
    return startOperation( res );
}

/*!
    Tries to create a directory (child) with the name \a dirname. If
    it is successful, a newChildren() signal with the new child is
    emitted, and the createdDirectory() signal with the information
    about the new child is also emitted. The finished() signal (with
    success or failure) is emitted after the operation has been
    processed, so check the state of the network operation object to
    see whether or not the operation was successful.

    Because the operation will not be executed immediately, a pointer
    to the TQNetworkOperation object created by this function is
    returned. This object contains all the data about the operation
    and is used to refer to this operation later (e.g. in the signals
    that are emitted by the TQUrlOperator). The return value can also
    be 0 if the operation object couldn't be created.

    The path of this TQUrlOperator must to point to a directory (not a
    file) because the new directory will be created in this path.
*/

const TQNetworkOperation *TQUrlOperator::mkdir( const TQString &dirname )
{
    if ( !checkValid() )
	return 0;

    TQNetworkOperation *res = new TQNetworkOperation( TQNetworkProtocol::OpMkDir, dirname, TQString::null, TQString::null );
    return startOperation( res );
}

/*!
    Tries to remove the file (child) \a filename. If it succeeds the
    removed() signal is emitted. finished() (with success or failure)
    is also emitted after the operation has been processed, so check
    the state of the network operation object to see whether or not
    the operation was successful.

    Because the operation will not be executed immediately, a pointer
    to the TQNetworkOperation object created by this function is
    returned. This object contains all the data about the operation
    and is used to refer to this operation later (e.g. in the signals
    that are emitted by the TQUrlOperator). The return value can also
    be 0 if the operation object couldn't be created.

    The path of this TQUrlOperator must point to a directory; because
    if \a filename is relative, it will try to remove it in this
    directory.
*/

const TQNetworkOperation *TQUrlOperator::remove( const TQString &filename )
{
    if ( !checkValid() )
	return 0;

    TQNetworkOperation *res = new TQNetworkOperation( TQNetworkProtocol::OpRemove, filename, TQString::null, TQString::null );
    return startOperation( res );
}

/*!
    Tries to rename the file (child) called \a oldname to \a newname.
    If it succeeds, the itemChanged() signal is emitted. finished()
    (with success or failure) is also emitted after the operation has
    been processed, so check the state of the network operation object
    to see whether or not the operation was successful.

    Because the operation may not be executed immediately, a pointer
    to the TQNetworkOperation object created by this function is
    returned. This object contains all the data about the operation
    and is used to refer to this operation later (e.g. in the signals
    that are emitted by the TQUrlOperator). The return value can also
    be 0 if the operation object couldn't be created.

    This path of this TQUrlOperator must to point to a directory
    because \a oldname and \a newname are handled relative to this
    directory.
*/

const TQNetworkOperation *TQUrlOperator::rename( const TQString &oldname, const TQString &newname )
{
    if ( !checkValid() )
	return 0;

    TQNetworkOperation *res = new TQNetworkOperation( TQNetworkProtocol::OpRename, oldname, newname, TQString::null );
    return startOperation( res );
}

/*!
    Copies the file \a from to \a to. If \a move is true, the file is
    moved (copied and removed). \a from must point to a file and \a to
    must point to a directory (into which \a from is copied) unless \a
    toPath is set to false. If \a toPath is set to false then the \a
    to variable is assumed to be the absolute file path (destination
    file path + file name). The copying is done using the get() and
    put() operations. If you want to be notified about the progress of
    the operation, connect to the dataTransferProgress() signal. Bear
    in mind that the get() and put() operations emit this signal
    through the TQUrlOperator. The number of transferred bytes and the
    total bytes that you receive as arguments in this signal do not
    relate to the the whole copy operation; they relate first to the
    get() and then to the put() operation. Always check what type of
    operation the signal comes from; this is given in the signal's
    last argument.

    At the end, finished() (with success or failure) is emitted, so
    check the state of the network operation object to see whether or
    not the operation was successful.

    Because a move or copy operation consists of multiple operations
    (get(), put() and maybe remove()), this function doesn't return a
    single TQNetworkOperation, but rather a list of them. They are in
    the order: get(), put() and (if applicable) remove().

    \sa get(), put()
*/

TQPtrList<TQNetworkOperation> TQUrlOperator::copy( const TQString &from, const TQString &to, bool move, bool toPath )
{
#ifdef TQURLOPERATOR_DEBUG
    tqDebug( "TQUrlOperator: copy %s %s %d", from.latin1(), to.latin1(), move );
#endif

    TQPtrList<TQNetworkOperation> ops;
    ops.setAutoDelete( false );

    TQUrlOperator *uFrom = new TQUrlOperator( *this, from );
    TQUrlOperator *uTo = new TQUrlOperator( to );

    // prepare some string for later usage
    TQString frm = *uFrom;
    TQString file = uFrom->fileName();

     if (frm == to + file)
         return ops;
    
    file.prepend( "/" );

    // uFrom and uTo are deleted when the TQNetworkProtocol deletes itself via
    // autodelete
    uFrom->getNetworkProtocol();
    uTo->getNetworkProtocol();
    TQNetworkProtocol *gProt = uFrom->d->networkProtocol;
    TQNetworkProtocol *pProt = uTo->d->networkProtocol;

    uFrom->setPath( uFrom->dirPath() );

    if ( gProt && (gProt->supportedOperations()&TQNetworkProtocol::OpGet) &&
	 pProt && (pProt->supportedOperations()&TQNetworkProtocol::OpPut) ) {

	connect( gProt, TQ_SIGNAL( data(const TQByteArray&,TQNetworkOperation*) ),
		 this, TQ_SLOT( copyGotData(const TQByteArray&,TQNetworkOperation*) ) );
	connect( gProt, TQ_SIGNAL( dataTransferProgress(int,int,TQNetworkOperation*) ),
		 this, TQ_SIGNAL( dataTransferProgress(int,int,TQNetworkOperation*) ) );
	connect( gProt, TQ_SIGNAL( finished(TQNetworkOperation*) ),
		 this, TQ_SLOT( continueCopy(TQNetworkOperation*) ) );
	connect( gProt, TQ_SIGNAL( finished(TQNetworkOperation*) ),
		 this, TQ_SIGNAL( finished(TQNetworkOperation*) ) );
	connect( gProt, TQ_SIGNAL( connectionStateChanged(int,const TQString&) ),
		 this, TQ_SIGNAL( connectionStateChanged(int,const TQString&) ) );

	connect( pProt, TQ_SIGNAL( dataTransferProgress(int,int,TQNetworkOperation*) ),
		 this, TQ_SIGNAL( dataTransferProgress(int,int,TQNetworkOperation*) ) );
	connect( pProt, TQ_SIGNAL( finished(TQNetworkOperation*) ),
		 this, TQ_SIGNAL( finished(TQNetworkOperation*) ) );
	connect( pProt, TQ_SIGNAL( finished(TQNetworkOperation*) ),
		 this, TQ_SLOT( finishedCopy() ) );

	TQNetworkOperation *opGet = new TQNetworkOperation( TQNetworkProtocol::OpGet, frm, TQString::null, TQString::null );
	ops.append( opGet );
	gProt->addOperation( opGet );


	TQString toFile = to + file;
	if (!toPath)
	    toFile = to;

	TQNetworkOperation *opPut = new TQNetworkOperation( TQNetworkProtocol::OpPut, toFile, TQString::null, TQString::null );
	ops.append( opPut );

	d->getOpPutProtMap.insert( (void*)opGet, pProt );
	d->getOpGetProtMap.insert( (void*)opGet, gProt );
	d->getOpPutOpMap.insert( (void*)opGet, opPut );

	if ( move && (gProt->supportedOperations()&TQNetworkProtocol::OpRemove) ) {
	    gProt->setAutoDelete( false );

	    TQNetworkOperation *opRm = new TQNetworkOperation( TQNetworkProtocol::OpRemove, frm, TQString::null, TQString::null );
	    ops.append( opRm );
	    d->getOpRemoveOpMap.insert( (void*)opGet, opRm );
	} else {
	    gProt->setAutoDelete( true );
	}
#ifdef TQURLOPERATOR_DEBUG
	tqDebug( "TQUrlOperator: copy operation should start now..." );
#endif
	return ops;
    } else {
	TQString msg;
	if ( !gProt ) {
	    msg = tr( "The protocol `%1' is not supported" ).arg( uFrom->protocol() );
	} else if ( gProt->supportedOperations() & TQNetworkProtocol::OpGet ) {
	    msg = tr( "The protocol `%1' does not support copying or moving files or directories" ).arg( uFrom->protocol() );
	} else if ( !pProt ) {
	    msg = tr( "The protocol `%1' is not supported" ).arg( uTo->protocol() );
	} else {
	    msg = tr( "The protocol `%1' does not support copying or moving files or directories" ).arg( uTo->protocol() );
	}
	delete uFrom;
	delete uTo;
	TQNetworkOperation *res = new TQNetworkOperation( TQNetworkProtocol::OpGet, frm, to, TQString::null );
	res->setState( TQNetworkProtocol::StFailed );
	res->setProtocolDetail( msg );
	res->setErrorCode( (int)TQNetworkProtocol::ErrUnsupported );
	emit finished( res );
	deleteOperation( res );
    }

    return ops;
}

/*!
    \overload

    Copies the \a files to the directory \a dest. If \a move is true
    the files are moved, not copied. \a dest must point to a
    directory.

    This function calls copy() for each entry in \a files in turn. You
    don't get a result from this function; each time a new copy
    begins, startedNextCopy() is emitted, with a list of
    TQNetworkOperations that describe the new copy operation.
*/

void TQUrlOperator::copy( const TQStringList &files, const TQString &dest,
			 bool move )
{
    d->waitingCopies = files;
    d->waitingCopiesDest = dest;
    d->waitingCopiesMove = move;

    finishedCopy();
}

/*!
    Returns true if the URL is a directory; otherwise returns false.
    This may not always work correctly, if the protocol of the URL is
    something other than file (local filesystem). If you pass a bool
    pointer as the \a ok argument, \a *ok is set to true if the result
    of this function is known to be correct, and to false otherwise.
*/

bool TQUrlOperator::isDir( bool *ok )
{
    if ( ok )
	*ok = true;
    if ( isLocalFile() ) {
	if ( TQFileInfo( path() ).isDir() )
	    return true;
	else
	    return false;
    }

    if ( d->entryMap.contains( "." ) ) {
	return d->entryMap[ "." ].isDir();
    }
    // #### can assume that we are a directory?
    if ( ok )
	*ok = false;
    return true;
}

/*!
    Tells the network protocol to get data from \a location or, if
    this is TQString::null, to get data from the location to which this
    URL points (see TQUrl::fileName() and TQUrl::encodedPathAndQuery()).
    What happens then depends on the network protocol. The data()
    signal is emitted when data comes in. Because it's unlikely that
    all data will come in at once, it is common for multiple data()
    signals to be emitted. The dataTransferProgress() signal is
    emitted while processing the operation. At the end, finished()
    (with success or failure) is emitted, so check the state of the
    network operation object to see whether or not the operation was
    successful.

    If \a location is TQString::null, the path of this TQUrlOperator
    should point to a file when you use this operation. If \a location
    is not empty, it can be a relative URL (a child of the path to
    which the TQUrlOperator points) or an absolute URL.

    For example, to get a web page you might do something like this:

    \code
    TQUrlOperator op( "http://www.whatever.org/cgi-bin/search.pl?cmd=Hello" );
    op.get();
    \endcode

    For most other operations, the path of the TQUrlOperator must point
    to a directory. If you want to download a file you could do the
    following:

    \code
    TQUrlOperator op( "ftp://ftp.whatever.org/pub" );
    // do some other stuff like op.listChildren() or op.mkdir( "new_dir" )
    op.get( "a_file.txt" );
    \endcode

    This will get the data of ftp://ftp.whatever.org/pub/a_file.txt.

    \e Never do anything like this:
    \code
    TQUrlOperator op( "http://www.whatever.org/cgi-bin" );
    op.get( "search.pl?cmd=Hello" ); // WRONG!
    \endcode

    If \a location is not empty and relative it must not contain any
    queries or references, just the name of a child. So if you need to
    specify a query or reference, do it as shown in the first example
    or specify the full URL (such as
    http://www.whatever.org/cgi-bin/search.pl?cmd=Hello) as \a location.

    \sa copy()
*/

const TQNetworkOperation *TQUrlOperator::get( const TQString &location )
{
    TQUrl u( *this );
    if ( !location.isEmpty() )
	u = TQUrl( *this, location );

    if ( !u.isValid() )
	return 0;

    if ( !d->networkProtocol ) {
	setProtocol( u.protocol() );
	getNetworkProtocol();
    }

    TQNetworkOperation *res = new TQNetworkOperation( TQNetworkProtocol::OpGet, u, TQString::null, TQString::null );
    return startOperation( res );
}

/*!
    This function tells the network protocol to put \a data in \a
    location. If \a location is empty (TQString::null), it puts the \a
    data in the location to which the URL points. What happens depends
    on the network protocol. Depending on the network protocol, some
    data might come back after putting data, in which case the data()
    signal is emitted. The dataTransferProgress() signal is emitted
    during processing of the operation. At the end, finished() (with
    success or failure) is emitted, so check the state of the network
    operation object to see whether or not the operation was
    successful.

    If \a location is TQString::null, the path of this TQUrlOperator
    should point to a file when you use this operation. If \a location
    is not empty, it can be a relative (a child of the path to which
    the TQUrlOperator points) or an absolute URL.

    For putting some data to a file you can do the following:

    \code
    TQUrlOperator op( "ftp://ftp.whatever.com/home/me/filename.dat" );
    op.put( data );
    \endcode

    For most other operations, the path of the TQUrlOperator must point
    to a directory. If you want to upload data to a file you could do
    the following:

    \code
    TQUrlOperator op( "ftp://ftp.whatever.com/home/me" );
    // do some other stuff like op.listChildren() or op.mkdir( "new_dir" )
    op.put( data, "filename.dat" );
    \endcode

    This will upload the data to ftp://ftp.whatever.com/home/me/filename.dat.

    \sa copy()
*/

const TQNetworkOperation *TQUrlOperator::put( const TQByteArray &data, const TQString &location )
{
    TQUrl u( *this );
    if ( !location.isEmpty() )
	u = TQUrl( *this, location );

    if ( !u.isValid() )
	return 0;

    if ( !d->networkProtocol ) {
	setProtocol( u.protocol() );
	getNetworkProtocol();
    }

    TQNetworkOperation *res = new TQNetworkOperation( TQNetworkProtocol::OpPut, u, TQString::null, TQString::null );
    res->setRawArg( 1, data );
    return startOperation( res );
}

/*!
    Sets the name filter of the URL to \a nameFilter.

    \sa TQDir::setNameFilter()
*/

void TQUrlOperator::setNameFilter( const TQString &nameFilter )
{
    d->nameFilter = nameFilter;
}

/*!
    Returns the name filter of the URL.

    \sa TQUrlOperator::setNameFilter() TQDir::nameFilter()
*/

TQString TQUrlOperator::nameFilter() const
{
    return d->nameFilter;
}

/*!
    Clears the cache of children.
*/

void TQUrlOperator::clearEntries()
{
    d->entryMap.clear();
}

/*!
    Adds an entry to the cache of children.
*/

void TQUrlOperator::addEntry( const TQValueList<TQUrlInfo> &i )
{
    TQValueList<TQUrlInfo>::ConstIterator it = i.begin();
    for ( ; it != i.end(); ++it )
	d->entryMap[ ( *it ).name().stripWhiteSpace() ] = *it;
}

/*!
    Returns the URL information for the child \a entry, or returns an
    empty TQUrlInfo object if there is no information available about
    \a entry. Information about \a entry is only available after a successfully
    finished listChildren() operation.
*/

TQUrlInfo TQUrlOperator::info( const TQString &entry ) const
{
    if ( d->entryMap.contains( entry.stripWhiteSpace() ) ) {
	return d->entryMap[ entry.stripWhiteSpace() ];
    } else if ( entry == "." || entry == ".." ) {
	 // return a faked TQUrlInfo
	 TQUrlInfo inf;
	 inf.setName( entry );
	 inf.setDir( true );
	 inf.setFile( false );
	 inf.setSymLink( false );
	 inf.setOwner( tr( "(unknown)" ) );
	 inf.setGroup( tr( "(unknown)" ) );
	 inf.setSize( 0 );
	 inf.setWritable( false );
	 inf.setReadable( true );
	 return inf;
    }
    return TQUrlInfo();
}

/*!
    Finds a network protocol for the URL and deletes the old network protocol.
*/

void TQUrlOperator::getNetworkProtocol()
{
    delete d->networkProtocol;
    TQNetworkProtocol *p = TQNetworkProtocol::getNetworkProtocol( protocol() );
    if ( !p ) {
	d->networkProtocol = 0;
	return;
    }

    d->networkProtocol = (TQNetworkProtocol *)p;
    d->networkProtocol->setUrl( this );
    connect( d->networkProtocol, TQ_SIGNAL( itemChanged(TQNetworkOperation*) ),
	     this, TQ_SLOT( slotItemChanged(TQNetworkOperation*) ) );
}

/*!
    Deletes the currently used network protocol.
*/

void TQUrlOperator::deleteNetworkProtocol()
{
    if (d->networkProtocol) {
        d->networkProtocol->deleteLater();
        d->networkProtocol = 0;
    }
}

/*!
    \reimp
*/

void TQUrlOperator::setPath( const TQString& path )
{
    TQUrl::setPath( path );
    if ( d->networkProtocol )
	d->networkProtocol->setUrl( this );
}

/*!
    \reimp
*/

void TQUrlOperator::reset()
{
    TQUrl::reset();
    deleteNetworkProtocol();
    d->nameFilter = "*";
}

/*!
    \reimp
*/

bool TQUrlOperator::parse( const TQString &url )
{
    bool b = TQUrl::parse( url );
    if ( !b ) {
	return b;
    }

    getNetworkProtocol();

    return b;
}

/*!
    \reimp
*/

TQUrlOperator& TQUrlOperator::operator=( const TQUrlOperator &url )
{
    deleteNetworkProtocol();
    TQUrl::operator=( url );

    TQPtrDict<TQNetworkOperation> getOpPutOpMap = d->getOpPutOpMap;
    TQPtrDict<TQNetworkProtocol> getOpPutProtMap = d->getOpPutProtMap;
    TQPtrDict<TQNetworkProtocol> getOpGetProtMap = d->getOpGetProtMap;
    TQPtrDict<TQNetworkOperation> getOpRemoveOpMap = d->getOpRemoveOpMap;

    *d = *url.d;

    d->oldOps.setAutoDelete( false );
    d->getOpPutOpMap = getOpPutOpMap;
    d->getOpPutProtMap = getOpPutProtMap;
    d->getOpGetProtMap = getOpGetProtMap;
    d->getOpRemoveOpMap = getOpRemoveOpMap;

    d->networkProtocol = 0;
    getNetworkProtocol();
    return *this;
}

/*!
    \reimp
*/

TQUrlOperator& TQUrlOperator::operator=( const TQString &url )
{
    deleteNetworkProtocol();
    TQUrl::operator=( url );
    d->oldOps.setAutoDelete( false );
    getNetworkProtocol();
    return *this;
}

/*!
    \reimp
*/

bool TQUrlOperator::cdUp()
{
    bool b = TQUrl::cdUp();
    if ( d->networkProtocol )
	d->networkProtocol->setUrl( this );
    return b;
}

/*!
    \reimp
*/

bool TQUrlOperator::checkValid()
{
    // ######
    if ( !isValid() ) {
	//emit error( ErrValid, tr( "The entered URL is not valid!" ) );
	return false;
    } else
	return true;
}


/*!
    \internal
*/

void TQUrlOperator::copyGotData( const TQByteArray &data_, TQNetworkOperation *op )
{
#ifdef TQURLOPERATOR_DEBUG
    tqDebug( "TQUrlOperator: copyGotData: %d new bytes", data_.size() );
#endif
    TQNetworkOperation *put = d->getOpPutOpMap[ (void*)op ];
    if ( put ) {
	TQByteArray &s = put->raw( 1 );
	int size = s.size();
	s.resize( size + data_.size() );
	memcpy( s.data() + size, data_.data(), data_.size() );
    }
    emit data( data_, op );
}

/*!
    \internal
*/

void TQUrlOperator::continueCopy( TQNetworkOperation *op )
{
    if ( op->operation() != TQNetworkProtocol::OpGet )
	return;
    if ( op->state()!=TQNetworkProtocol::StDone &&  op->state()!=TQNetworkProtocol::StFailed ) {
	return;
    }

#ifdef TQURLOPERATOR_DEBUG
    if ( op->state() != TQNetworkProtocol::StFailed ) {
	tqDebug( "TQUrlOperator: continue copy (get finished, put will start)" );
    }
#endif

    TQNetworkOperation *put = d->getOpPutOpMap[ (void*)op ];
    TQNetworkProtocol *gProt = d->getOpGetProtMap[ (void*)op ];
    TQNetworkProtocol *pProt = d->getOpPutProtMap[ (void*)op ];
    TQNetworkOperation *rm = d->getOpRemoveOpMap[ (void*)op ];
    d->getOpPutOpMap.take( op );
    d->getOpGetProtMap.take( op );
    d->getOpPutProtMap.take( op );
    d->getOpRemoveOpMap.take( op );
    if ( pProt )
	pProt->setAutoDelete( true );
    if ( put && pProt ) {
	if ( op->state() != TQNetworkProtocol::StFailed ) {
	    pProt->addOperation( put );
	    d->currPut = pProt;
	} else {
	    deleteOperation( put );
	}
    }
    if ( gProt ) {
	gProt->setAutoDelete( true );
    }
    if ( rm && gProt ) {
	if ( op->state() != TQNetworkProtocol::StFailed ) {
	    gProt->addOperation( rm );
	} else {
	    deleteOperation( rm );
	}
    }
    disconnect( gProt, TQ_SIGNAL( data(const TQByteArray&,TQNetworkOperation*) ),
		this, TQ_SLOT( copyGotData(const TQByteArray&,TQNetworkOperation*) ) );
    disconnect( gProt, TQ_SIGNAL( finished(TQNetworkOperation*) ),
		this, TQ_SLOT( continueCopy(TQNetworkOperation*) ) );
}

/*!
    \internal
*/

void TQUrlOperator::finishedCopy()
{
#ifdef TQURLOPERATOR_DEBUG
    tqDebug( "TQUrlOperator: finished copy (finished putting)" );
#endif

    if ( d->waitingCopies.isEmpty() )
	return;

    TQString cp = d->waitingCopies.first();
    d->waitingCopies.remove( cp );
    TQPtrList<TQNetworkOperation> lst = copy( cp, d->waitingCopiesDest, d->waitingCopiesMove );
    emit startedNextCopy( lst );
}

/*!
    Stops the current network operation and removes all this
    TQUrlOperator's waiting network operations.
*/

void TQUrlOperator::stop()
{
    d->getOpPutOpMap.clear();
    d->getOpRemoveOpMap.clear();
    d->getOpGetProtMap.setAutoDelete( true );
    d->getOpPutProtMap.setAutoDelete( true );
    TQPtrDictIterator<TQNetworkProtocol> it( d->getOpPutProtMap );
    for ( ; it.current(); ++it )
	it.current()->stop();
    d->getOpPutProtMap.clear();
    it = TQPtrDictIterator<TQNetworkProtocol>( d->getOpGetProtMap );
    for ( ; it.current(); ++it )
	it.current()->stop();
    d->getOpGetProtMap.clear();
    if ( d->currPut ) {
	d->currPut->stop();
	delete (TQNetworkProtocol *) d->currPut;
	d->currPut = 0;
    }
    d->waitingCopies.clear();
    if ( d->networkProtocol )
	d->networkProtocol->stop();
    getNetworkProtocol();
}

/*!
    \internal
*/

void TQUrlOperator::deleteOperation( TQNetworkOperation *op )
{
    if ( op )
	d->oldOps.append( op );
}

/*!
    \internal
    updates the entryMap after a network operation finished
*/

void TQUrlOperator::slotItemChanged( TQNetworkOperation *op )
{
    if ( !op )
	return;

    switch ( op->operation() ) {
    case TQNetworkProtocol::OpRename :
    {
	if ( op->arg( 0 ) == op->arg( 1 ) )
	    return;

	TQMap<TQString, TQUrlInfo>::iterator mi = d->entryMap.find( op->arg( 0 ) );
	if ( mi != d->entryMap.end() ) {
	    mi.data().setName( op->arg( 1 ) );
	    d->entryMap[ op->arg( 1 ) ] = mi.data();
	    d->entryMap.erase( mi );
	}
	break;
    }
    case TQNetworkProtocol::OpRemove :
    {
	TQMap<TQString, TQUrlInfo>::iterator mi = d->entryMap.find( op->arg( 0 ) );
	if ( mi != d->entryMap.end() )
	    d->entryMap.erase( mi );
	break;
    }
    default:
	break;
    }
}


#endif // TQT_NO_NETWORKPROTOCOL
