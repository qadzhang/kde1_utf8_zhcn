/****************************************************************************
**
** Implementation of TQLocalFs class
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

#include "ntqlocalfs.h"

#ifndef TQT_NO_NETWORKPROTOCOL

#include "ntqfileinfo.h"
#include "ntqfile.h"
#include "ntqurlinfo.h"
#include "ntqapplication.h"
#include "ntqurloperator.h"
#include "ntqguardedptr.h"

//#define TQLOCALFS_DEBUG


/*!
    \class TQLocalFs ntqlocalfs.h
    \brief The TQLocalFs class is an implementation of a
    TQNetworkProtocol that works on the local file system.
\if defined(commercial)
    It is part of the <a href="commercialeditions.html">TQt Enterprise Edition</a>.
\endif

    \module network

    \ingroup io

    This class is derived from TQNetworkProtocol. TQLocalFs is not
    normally used directly, but rather through a TQUrlOperator, for
    example:
    \code
    TQUrlOperator op( "file:///tmp" );
    op.listChildren(); // Asks the server to provide a directory listing
    \endcode

    This code will only work if the TQLocalFs class is registered; to
    register the class, you must call tqInitNetworkProtocols() before
    using a TQUrlOperator with TQLocalFs.

    If you really need to use TQLocalFs directly, don't forget
    to set its TQUrlOperator with setUrl().

    \sa \link network.html TQt Network Documentation \endlink TQNetworkProtocol, TQUrlOperator
*/

/*!
    Constructor.
*/

TQLocalFs::TQLocalFs()
    : TQNetworkProtocol()
{
}

static int convertPermissions(TQFileInfo *fi)
{
    int p = 0;
    if ( fi->permission( TQFileInfo::ReadOwner ) )
	p |= TQUrlInfo::ReadOwner;
    if ( fi->permission( TQFileInfo::WriteOwner ) )
	p |= TQUrlInfo::WriteOwner;
    if ( fi->permission( TQFileInfo::ExeOwner ) )
	p |= TQUrlInfo::ExeOwner;
    if ( fi->permission( TQFileInfo::ReadGroup ) )
	p |= TQUrlInfo::ReadGroup;
    if ( fi->permission( TQFileInfo::WriteGroup ) )
	p |= TQUrlInfo::WriteGroup;
    if ( fi->permission( TQFileInfo::ExeGroup ) )
	p |= TQUrlInfo::ExeGroup;
    if ( fi->permission( TQFileInfo::ReadOther ) )
	p |= TQUrlInfo::ReadOther;
    if ( fi->permission( TQFileInfo::WriteOther ) )
	p |= TQUrlInfo::WriteOther;
    if ( fi->permission( TQFileInfo::ExeOther ) )
	p |= TQUrlInfo::ExeOther;
    return p;
}

/*!
    \reimp
*/

void TQLocalFs::operationListChildren( TQNetworkOperation *op )
{
#ifdef TQLOCALFS_DEBUG
    tqDebug( "TQLocalFs: operationListChildren" );
#endif
    op->setState( StInProgress );

    dir = TQDir( url()->path() );
    dir.setNameFilter( url()->nameFilter() );
    dir.setMatchAllDirs( true );
    if ( !dir.isReadable() ) {
	TQString msg = tr( "Could not read directory\n%1" ).arg( url()->path() );
	op->setState( StFailed );
	op->setProtocolDetail( msg );
	op->setErrorCode( (int)ErrListChildren );
	emit finished( op );
	return;
    }

    const TQFileInfoList *filist = dir.entryInfoList( TQDir::All | TQDir::Hidden | TQDir::System );
    if ( !filist ) {
	TQString msg = tr( "Could not read directory\n%1" ).arg( url()->path() );
	op->setState( StFailed );
	op->setProtocolDetail( msg );
	op->setErrorCode( (int)ErrListChildren );
	emit finished( op );
	return;
    }

    emit start( op );

    TQFileInfoListIterator it( *filist );
    TQFileInfo *fi;
    TQValueList<TQUrlInfo> infos;
    while ( ( fi = it.current() ) != 0 ) {
	++it;
	infos << TQUrlInfo( fi->fileName(), convertPermissions(fi), fi->owner(), fi->group(),
			   fi->size(), fi->lastModified(), fi->lastRead(), fi->isDir(), fi->isFile(),
			   fi->isSymLink(), fi->isWritable(), fi->isReadable(), fi->isExecutable() );
    }
    emit newChildren( infos, op );
    op->setState( StDone );
    emit finished( op );
}

/*!
    \reimp
*/

void TQLocalFs::operationMkDir( TQNetworkOperation *op )
{
#ifdef TQLOCALFS_DEBUG
    tqDebug( "TQLocalFs: operationMkDir" );
#endif
    op->setState( StInProgress );
    TQString dirname = op->arg( 0 );

    dir = TQDir( url()->path() );
    if ( dir.mkdir( dirname ) ) {
	TQFileInfo fi( dir, dirname );
	TQUrlInfo inf( fi.fileName(), convertPermissions(&fi), fi.owner(), fi.group(),
		      fi.size(), fi.lastModified(), fi.lastRead(), fi.isDir(), fi.isFile(),
		      fi.isSymLink(), fi.isWritable(), fi.isReadable(), fi.isExecutable() );
	emit newChild( inf, op );
	op->setState( StDone );
	emit createdDirectory( inf, op );
	emit finished( op );
    } else {
	TQString msg = tr( "Could not create directory\n%1" ).arg( dirname );
	op->setState( StFailed );
	op->setProtocolDetail( msg );
	op->setErrorCode( (int)ErrMkDir );
	emit finished( op );
    }
}

/*!
    \reimp
*/

void TQLocalFs::operationRemove( TQNetworkOperation *op )
{
#ifdef TQLOCALFS_DEBUG
    tqDebug( "TQLocalFs: operationRemove" );
#endif
    op->setState( StInProgress );
    TQString name = TQUrl( op->arg( 0 ) ).path();
    bool deleted = false;

    dir = TQDir( url()->path() );

    TQFileInfo fi( dir, name );
    if ( fi.isDir() ) {
	if ( dir.rmdir( name ) )
	    deleted = true;
    }

    if ( deleted || dir.remove( name ) ) {
	op->setState( StDone );
	emit removed( op );
	emit finished( op );
    } else {
	TQString msg = tr( "Could not remove file or directory\n%1" ).arg( name );
	op->setState( StFailed );
	op->setProtocolDetail( msg );
	op->setErrorCode( (int)ErrRemove );
	emit finished( op );
    }
}

/*!
    \reimp
*/

void TQLocalFs::operationRename( TQNetworkOperation *op )
{
#ifdef TQLOCALFS_DEBUG
    tqDebug( "TQLocalFs: operationRename" );
#endif
    op->setState( StInProgress );
    TQString oldname = op->arg( 0 );
    TQString newname = op->arg( 1 );

    dir = TQDir( url()->path() );
    if ( dir.rename( oldname, newname ) ) {
	op->setState( StDone );
	emit itemChanged( op );
	emit finished( op );
    } else {
	TQString msg = tr( "Could not rename\n%1\nto\n%2" ).arg( oldname ).arg( newname );
	op->setState( StFailed );
	op->setProtocolDetail( msg );
	op->setErrorCode( (int)ErrRename );
	emit finished( op );
    }
}

/*!
    \reimp
*/

void TQLocalFs::operationGet( TQNetworkOperation *op )
{
#ifdef TQLOCALFS_DEBUG
    tqDebug( "TQLocalFs: operationGet" );
#endif
    op->setState( StInProgress );
    TQString from = TQUrl( op->arg( 0 ) ).path();

    TQFile f( from );
    if ( !f.open( IO_ReadOnly ) ) {
#ifdef TQLOCALFS_DEBUG
	tqDebug( "TQLocalFs: could not open %s", from.latin1() );
#endif
	TQString msg = tr( "Could not open\n%1" ).arg( from );
	op->setState( StFailed );
	op->setProtocolDetail( msg );
	op->setErrorCode( (int)ErrGet );
	emit finished( op );
	return;
    }

    TQByteArray s;
    emit dataTransferProgress( 0, f.size(), op );
    if ( f.size() != 0 ) {
	int blockSize = calcBlockSize( f.size() );
	if ( (int)f.size() < blockSize ) {
	    s.resize( f.size() );
	    f.readBlock( s.data(), f.size() );
	    emit data( s, op );
	    emit dataTransferProgress( f.size(), f.size(), op );
#ifdef TQLOCALFS_DEBUG
	    tqDebug( "TQLocalFs: got all %d bytes at once", f.size() );
#endif
	} else {
	    s.resize( blockSize );
	    int remaining = f.size();
	    TQGuardedPtr<TQObject> that = this;
	    while ( that && remaining > 0 ) {
		if ( operationInProgress() != op )
		    return;
		if ( remaining >= blockSize ) {
		    f.readBlock( s.data(), blockSize );
		    emit data( s, op );
		    emit dataTransferProgress( f.size() - remaining, f.size(), op );
		    remaining -= blockSize;
		} else {
		    s.resize( remaining );
		    f.readBlock( s.data(), remaining );
		    emit data( s, op );
		    emit dataTransferProgress( f.size() - remaining, f.size(), op );
		    remaining -= remaining;
		}
                tqApp->processEvents();
	    }
	    if (!that)
		return;
#ifdef TQLOCALFS_DEBUG
	    tqDebug( "TQLocalFs: got all %d bytes step by step", f.size() );
#endif
	    emit dataTransferProgress( f.size(), f.size(), op );
	}
    }
    op->setState( StDone );
    f.close();
    emit finished( op );
}

/*!
    \reimp
*/

void TQLocalFs::operationPut( TQNetworkOperation *op )
{
#ifdef TQLOCALFS_DEBUG
    tqDebug( "TQLocalFs: operationPut" );
#endif
    op->setState( StInProgress );
    TQString to = TQUrl( op->arg( 0 ) ).path();

    TQFile f( to );
    if ( !f.open( IO_WriteOnly ) ) {
	TQString msg = tr( "Could not write\n%1" ).arg( to );
	op->setState( StFailed );
	op->setProtocolDetail( msg );
	op->setErrorCode( (int)ErrPut );
	emit finished( op );
	return;
    }

    TQByteArray ba( op->rawArg( 1 ) );
    emit dataTransferProgress( 0, ba.size(), op );
    int blockSize = calcBlockSize( ba.size() );
    if ( (int)ba.size() < blockSize ) {
	f.writeBlock( ba.data(), ba.size() );
	emit dataTransferProgress( ba.size(), ba.size(), op );
    } else {
	int i = 0;
	while ( i + blockSize < (int)ba.size() - 1 ) {
	    if ( operationInProgress() != op )
		return;
	    f.writeBlock( &ba.data()[ i ], blockSize );
	    f.flush();
	    emit dataTransferProgress( i + blockSize, ba.size(), op );
	    i += blockSize;
	    TQGuardedPtr<TQObject> that = this;
            tqApp->processEvents();
            if (!that)
                return;
	}
	if ( i < (int)ba.size() - 1 )
	    f.writeBlock( &ba.data()[ i ], ba.size() - i );
	emit dataTransferProgress( ba.size(), ba.size(), op );
    }
    op->setState( StDone );
    f.close();
    emit finished( op );
}

/*!
    \reimp
*/

int TQLocalFs::supportedOperations() const
{
    return OpListChildren | OpMkDir | OpRemove | OpRename | OpGet | OpPut;
}

/*!
    \internal
*/

int TQLocalFs::calcBlockSize( int totalSize ) const
{
    if ( totalSize == 0 )
	return 1024;
    int s = totalSize / 100;
    // we want a block size between 1KB and 1MB
    if ( s < 1024 )
	s = 1024;
    if ( s > 1048576 )
	s = 1048576;
    return s;
}

#endif // TQT_NO_NETWORKPROTOCOL
