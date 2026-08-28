/****************************************************************************
**
** Definition of TQHttp and related classes.
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

#ifndef TQHTTP_H
#define TQHTTP_H

#ifndef QT_H
#include "ntqobject.h"
#include "ntqnetworkprotocol.h"
#include "ntqstringlist.h"
#endif // QT_H

#if !defined( TQT_MODULE_NETWORK ) || defined( QT_LICENSE_PROFESSIONAL ) || defined( QT_INTERNAL_NETWORK )
#define TQM_EXPORT_HTTP
#define TQM_TEMPLATE_EXTERN_HTTP
#else
#define TQM_EXPORT_HTTP TQ_EXPORT
#define TQM_TEMPLATE_EXTERN_HTTP Q_TEMPLATE_EXTERN
#endif

#ifndef TQT_NO_NETWORKPROTOCOL_HTTP

class TQSocket;
class TQTimerEvent;
class TQTextStream;
class TQIODevice;

class TQHttpPrivate;
class TQHttpRequest;

class TQM_EXPORT_HTTP TQHttpHeader
{
public:
    TQHttpHeader();
    TQHttpHeader( const TQHttpHeader& header );
    TQHttpHeader( const TQString& str );
    virtual ~TQHttpHeader();

    TQHttpHeader& operator=( const TQHttpHeader& h );

    TQString value( const TQString& key ) const;
    void setValue( const TQString& key, const TQString& value );
    void removeValue( const TQString& key );

    TQStringList keys() const;
    bool hasKey( const TQString& key ) const;

    bool hasContentLength() const;
    uint contentLength() const;
    void setContentLength( int len );

    bool hasContentType() const;
    TQString contentType() const;
    void setContentType( const TQString& type );

    virtual TQString toString() const;
    bool isValid() const;

    virtual int majorVersion() const = 0;
    virtual int minorVersion() const = 0;

protected:
    virtual bool parseLine( const TQString& line, int number );
    bool parse( const TQString& str );
    void setValid( bool );

private:
    TQMap<TQString,TQString> values;
    bool valid;
};

class TQM_EXPORT_HTTP TQHttpResponseHeader : public TQHttpHeader
{
private:
    TQHttpResponseHeader( int code, const TQString& text = TQString::null, int majorVer = 1, int minorVer = 1 );
    TQHttpResponseHeader( const TQString& str );

    void setStatusLine( int code, const TQString& text = TQString::null, int majorVer = 1, int minorVer = 1 );

public:
    TQHttpResponseHeader();
    TQHttpResponseHeader( const TQHttpResponseHeader& header );

    int statusCode() const;
    TQString reasonPhrase() const;

    int majorVersion() const;
    int minorVersion() const;

    TQString toString() const;

protected:
    bool parseLine( const TQString& line, int number );

private:
    int statCode;
    TQString reasonPhr;
    int majVer;
    int minVer;

    friend class TQHttp;
};

class TQM_EXPORT_HTTP TQHttpRequestHeader : public TQHttpHeader
{
public:
    TQHttpRequestHeader();
    TQHttpRequestHeader( const TQString& method, const TQString& path, int majorVer = 1, int minorVer = 1 );
    TQHttpRequestHeader( const TQHttpRequestHeader& header );
    TQHttpRequestHeader( const TQString& str );

    void setRequest( const TQString& method, const TQString& path, int majorVer = 1, int minorVer = 1 );

    TQString method() const;
    TQString path() const;

    int majorVersion() const;
    int minorVersion() const;

    TQString toString() const;

protected:
    bool parseLine( const TQString& line, int number );

private:
    TQString m;
    TQString p;
    int majVer;
    int minVer;
};

class TQM_EXPORT_HTTP TQHttp : public TQNetworkProtocol
{
    TQ_OBJECT

public:
    TQHttp();
    TQHttp( TQObject* parent, const char* name = 0 ); // ### TQt 4.0: make parent=0 and get rid of the TQHttp() constructor
    TQHttp( const TQString &hostname, TQ_UINT16 port=80, TQObject* parent=0, const char* name = 0 );
    virtual ~TQHttp();

    int supportedOperations() const;

    enum State { Unconnected, HostLookup, Connecting, Sending, Reading, Connected, Closing };
    enum Error {
	NoError,
	UnknownError,
	HostNotFound,
	ConnectionRefused,
	UnexpectedClose,
	InvalidResponseHeader,
	WrongContentLength,
	Aborted
    };

    int setHost(const TQString &hostname, TQ_UINT16 port=80 );

    int get( const TQString& path, TQIODevice* to=0 );
    int post( const TQString& path, TQIODevice* data, TQIODevice* to=0  );
    int post( const TQString& path, const TQByteArray& data, TQIODevice* to=0 );
    int head( const TQString& path );
    int request( const TQHttpRequestHeader &header, TQIODevice *device=0, TQIODevice *to=0 );
    int request( const TQHttpRequestHeader &header, const TQByteArray &data, TQIODevice *to=0 );

    int closeConnection();

    TQ_ULONG bytesAvailable() const;
    TQ_LONG readBlock( char *data, TQ_ULONG maxlen );
    TQByteArray readAll();

    int currentId() const;
    TQIODevice* currentSourceDevice() const;
    TQIODevice* currentDestinationDevice() const;
    TQHttpRequestHeader currentRequest() const;
    bool hasPendingRequests() const;
    void clearPendingRequests();

    State state() const;

    Error error() const;
    TQString errorString() const;

public slots:
    void abort();

signals:
    void stateChanged( int );
    void responseHeaderReceived( const TQHttpResponseHeader& resp );
    void readyRead( const TQHttpResponseHeader& resp );
    void dataSendProgress( int, int );
    void dataReadProgress( int, int );

    void requestStarted( int );
    void requestFinished( int, bool );
    void done( bool );

protected:
    void operationGet( TQNetworkOperation *op );
    void operationPut( TQNetworkOperation *op );

    void timerEvent( TQTimerEvent * );

private slots:
    void clientReply( const TQHttpResponseHeader &rep );
    void clientDone( bool );
    void clientStateChanged( int );

    void startNextRequest();
    void slotReadyRead();
    void slotConnected();
    void slotError( int );
    void slotClosed();
    void slotBytesWritten( int );

private:
    TQHttpPrivate *d;
    void *unused; // ### TQt 4.0: remove this (in for binary compatibility)
    int bytesRead;

    int addRequest( TQHttpRequest * );
    void sendRequest();
    void finishedWithSuccess();
    void finishedWithError( const TQString& detail, int errorCode );

    void killIdleTimer();

    void init();
    void setState( int );
    void close();

    friend class TQHttpNormalRequest;
    friend class TQHttpSetHostRequest;
    friend class TQHttpCloseRequest;
    friend class TQHttpPGHRequest;
};

#define Q_DEFINED_QHTTP
#include "ntqwinexport.h"
#endif
#endif
