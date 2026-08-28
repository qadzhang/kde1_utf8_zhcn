/****************************************************************************
**
** Definition of TQNetworkProtocol class
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

#ifndef TQNETWORKPROTOCOL_H
#define TQNETWORKPROTOCOL_H

#ifndef QT_H
#include "ntqurlinfo.h"
#include "ntqstring.h"
#include "ntqdict.h"
#include "ntqobject.h"
#endif // QT_H

#ifndef TQT_NO_NETWORKPROTOCOL

#if __GNUC__ - 0 > 3
#pragma GCC system_header
#endif

class TQNetworkProtocol;
class TQNetworkOperation;
class TQTimer;
class TQUrlOperator;
class TQNetworkProtocolPrivate;
template <class T> class TQValueList;

class TQ_EXPORT TQNetworkProtocolFactoryBase
{
public:
   TQNetworkProtocolFactoryBase();
   virtual ~TQNetworkProtocolFactoryBase();

public:
   virtual TQNetworkProtocol *createObject() = 0;

};

template< class T >
class TQNetworkProtocolFactory : public TQNetworkProtocolFactoryBase
{
public:
    TQNetworkProtocol *createObject() {
	return new T;
    }

};

typedef TQDict< TQNetworkProtocolFactoryBase > TQNetworkProtocolDict;

class TQ_EXPORT TQNetworkProtocol : public TQObject
{
    TQ_OBJECT

public:
    enum State {
	StWaiting = 0,
	StInProgress,
	StDone,
	StFailed,
	StStopped
    };

    enum Operation {
	OpListChildren = 1,
	OpMkDir = 2,
	OpMkdir = OpMkDir, // ### remove in 4.0
	OpRemove = 4,
	OpRename = 8,
	OpGet = 32,
	OpPut = 64
    };

    enum ConnectionState {
	ConHostFound,
	ConConnected,
	ConClosed
    };

    enum Error {
	// no error
	NoError = 0,
	// general errors
	ErrValid,
	ErrUnknownProtocol,
	ErrUnsupported,
	ErrParse,
	// errors on connect
	ErrLoginIncorrect,
	ErrHostNotFound,
	// protocol errors
	ErrListChildren,
	ErrListChlidren = ErrListChildren, // ### remove in 4.0
	ErrMkDir,
	ErrMkdir = ErrMkDir, // ### remove in 4.0
	ErrRemove,
	ErrRename,
	ErrGet,
	ErrPut,
	ErrFileNotExisting,
	ErrPermissionDenied
    };

    TQNetworkProtocol();
    virtual ~TQNetworkProtocol();

    virtual void setUrl( TQUrlOperator *u );

    virtual void setAutoDelete( bool b, int i = 10000 );
    bool autoDelete() const;

    static void registerNetworkProtocol( const TQString &protocol,
					 TQNetworkProtocolFactoryBase *protocolFactory );
    static TQNetworkProtocol *getNetworkProtocol( const TQString &protocol );
    static bool hasOnlyLocalFileSystem();

    virtual int supportedOperations() const;
    virtual void addOperation( TQNetworkOperation *op );

    TQUrlOperator *url() const;
    TQNetworkOperation *operationInProgress() const;
    virtual void clearOperationQueue();
    virtual void stop();

signals:
    void data( const TQByteArray &, TQNetworkOperation *res );
    void connectionStateChanged( int state, const TQString &data );
    void finished( TQNetworkOperation *res );
    void start( TQNetworkOperation *res );
    void newChildren( const TQValueList<TQUrlInfo> &, TQNetworkOperation *res );
    void newChild( const TQUrlInfo &, TQNetworkOperation *res );
    void createdDirectory( const TQUrlInfo &, TQNetworkOperation *res );
    void removed( TQNetworkOperation *res );
    void itemChanged( TQNetworkOperation *res );
    void dataTransferProgress( int bytesDone, int bytesTotal, TQNetworkOperation *res );

protected:
    virtual void processOperation( TQNetworkOperation *op );
    virtual void operationListChildren( TQNetworkOperation *op );
    virtual void operationMkDir( TQNetworkOperation *op );
    virtual void operationRemove( TQNetworkOperation *op );
    virtual void operationRename( TQNetworkOperation *op );
    virtual void operationGet( TQNetworkOperation *op );
    virtual void operationPut( TQNetworkOperation *op );
    virtual void operationPutChunk( TQNetworkOperation *op );
    virtual bool checkConnection( TQNetworkOperation *op );

private:
    TQNetworkProtocolPrivate *d;

private slots:
    void processNextOperation( TQNetworkOperation *old );
    void startOps();
    void emitNewChildren( const TQUrlInfo &i, TQNetworkOperation *op );

    void removeMe();

private:	// Disabled copy constructor and operator=
#if defined(TQ_DISABLE_COPY)
    TQNetworkProtocol( const TQNetworkProtocol & );
    TQNetworkProtocol &operator=( const TQNetworkProtocol & );
#endif
};

class TQNetworkOperationPrivate;

class TQ_EXPORT TQNetworkOperation : public TQObject
{
    TQ_OBJECT
    friend class TQUrlOperator;

public:
    TQNetworkOperation( TQNetworkProtocol::Operation operation,
		    const TQString &arg0, const TQString &arg1,
		    const TQString &arg2 );
    TQNetworkOperation( TQNetworkProtocol::Operation operation,
		    const TQByteArray &arg0, const TQByteArray &arg1,
		    const TQByteArray &arg2 );
    ~TQNetworkOperation();

    void setState( TQNetworkProtocol::State state );
    void setProtocolDetail( const TQString &detail );
    void setErrorCode( int ec );
    void setArg( int num, const TQString &arg );
    void setRawArg( int num, const TQByteArray &arg );

    TQNetworkProtocol::Operation operation() const;
    TQNetworkProtocol::State state() const;
    TQString arg( int num ) const;
    TQByteArray rawArg( int num ) const;
    TQString protocolDetail() const;
    int errorCode() const;

    void free();

private slots:
    void deleteMe();

private:
    TQByteArray &raw( int num ) const;
    TQNetworkOperationPrivate *d;

private:	// Disabled copy constructor and operator=
#if defined(TQ_DISABLE_COPY)
    TQNetworkOperation( const TQNetworkOperation & );
    TQNetworkOperation &operator=( const TQNetworkOperation & );
#endif
};

#endif // TQT_NO_NETWORKPROTOCOL

#endif // TQNETWORKPROTOCOL_H
