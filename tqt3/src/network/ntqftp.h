/****************************************************************************
**
** Definition of TQFtp class.
**
** Created : 970521
**
** Copyright (C) 1997-2008 Trolltech ASA.  All rights reserved.
**
** This file is part of the network module of the TQt GUI Toolkit.
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

#ifndef TQFTP_H
#define TQFTP_H

#ifndef QT_H
#include "ntqstring.h" // char*->TQString conversion
#include "ntqurlinfo.h"
#include "ntqnetworkprotocol.h"
#endif // QT_H

#if !defined( TQT_MODULE_NETWORK ) || defined( QT_LICENSE_PROFESSIONAL ) || defined( QT_INTERNAL_NETWORK )
#define TQM_EXPORT_FTP
#else
#define TQM_EXPORT_FTP TQ_EXPORT
#endif

#ifndef TQT_NO_NETWORKPROTOCOL_FTP


class TQSocket;
class TQFtpCommand;

class TQM_EXPORT_FTP TQFtp : public TQNetworkProtocol
{
    TQ_OBJECT

public:
    TQFtp(); // ### TQt 4.0: get rid of this overload
    TQFtp( TQObject *parent, const char *name=0 );
    virtual ~TQFtp();

    int supportedOperations() const;

    // non-TQNetworkProtocol functions:
    enum State {
	Unconnected,
	HostLookup,
	Connecting,
	Connected,
	LoggedIn,
	Closing
    };
    enum Error {
	NoError,
	UnknownError,
	HostNotFound,
	ConnectionRefused,
	NotConnected
    };
    enum Command {
	None,
	ConnectToHost,
	Login,
	Close,
	List,
	Cd,
	Get,
	Put,
	Remove,
	Mkdir,
	Rmdir,
	Rename,
	RawCommand
    };

    int connectToHost( const TQString &host, TQ_UINT16 port=21 );
    int login( const TQString &user=TQString::null, const TQString &password=TQString::null );
    int close();
    int list( const TQString &dir=TQString::null );
    int cd( const TQString &dir );
    int get( const TQString &file, TQIODevice *dev=0 );
    int put( const TQByteArray &data, const TQString &file );
    int put( TQIODevice *dev, const TQString &file );
    int remove( const TQString &file );
    int mkdir( const TQString &dir );
    int rmdir( const TQString &dir );
    int rename( const TQString &oldname, const TQString &newname );

    int rawCommand( const TQString &command );

    TQ_ULONG bytesAvailable() const;
    TQ_LONG readBlock( char *data, TQ_ULONG maxlen );
    TQByteArray readAll();

    int currentId() const;
    TQIODevice* currentDevice() const;
    Command currentCommand() const;
    bool hasPendingCommands() const;
    void clearPendingCommands();

    State state() const;

    Error error() const;
    TQString errorString() const;

public slots:
    void abort();

signals:
    void stateChanged( int );
    void listInfo( const TQUrlInfo& );
    void readyRead();
    void dataTransferProgress( int, int );
    void rawCommandReply( int, const TQString& );

    void commandStarted( int );
    void commandFinished( int, bool );
    void done( bool );

protected:
    void parseDir( const TQString &buffer, TQUrlInfo &info ); // ### TQt 4.0: delete this? (not public API)
    void operationListChildren( TQNetworkOperation *op );
    void operationMkDir( TQNetworkOperation *op );
    void operationRemove( TQNetworkOperation *op );
    void operationRename( TQNetworkOperation *op );
    void operationGet( TQNetworkOperation *op );
    void operationPut( TQNetworkOperation *op );

    // ### TQt 4.0: delete these
    // unused variables:
    TQSocket *commandSocket, *dataSocket;
    bool connectionReady, passiveMode;
    int getTotalSize, getDoneSize;
    bool startGetOnFail;
    int putToWrite, putWritten;
    bool errorInListChildren;

private:
    void init();
    int addCommand( TQFtpCommand * );

    bool checkConnection( TQNetworkOperation *op );

private slots:
    void startNextCommand();
    void piFinished( const TQString& );
    void piError( int, const TQString& );
    void piConnectState( int );
    void piFtpReply( int, const TQString& );

private slots:
    void npListInfo( const TQUrlInfo & );
    void npDone( bool );
    void npStateChanged( int );
    void npDataTransferProgress( int, int );
    void npReadyRead();

protected slots:
    // ### TQt 4.0: delete these
    void hostFound();
    void connected();
    void closed();
    void dataHostFound();
    void dataConnected();
    void dataClosed();
    void dataReadyRead();
    void dataBytesWritten( int nbytes );
    void error( int );
};

#endif // TQT_NO_NETWORKPROTOCOL_FTP

#endif // TQFTP_H
