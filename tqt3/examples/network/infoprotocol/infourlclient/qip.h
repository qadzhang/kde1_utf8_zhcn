/****************************************************************************
**
** Copyright (C) 1992-2008 Trolltech ASA.  All rights reserved.
**
** This file is part of an example program for TQt.  This example
** program may be used, distributed and modified without limitation.
**
*****************************************************************************/

#ifndef TQIP_H
#define TQIP_H

#include <ntqnetworkprotocol.h>

class TQSocket;

static const TQ_UINT16 infoPort = 42417;


class Qip : public TQNetworkProtocol
{
    TQ_OBJECT

public:
    Qip();
    virtual int supportedOperations() const;

protected:
    virtual void operationListChildren( TQNetworkOperation *op ); 
    virtual void operationGet( TQNetworkOperation *op );
    virtual bool checkConnection( TQNetworkOperation *op );

private slots:
    void socketConnected();
    void socketReadyRead();
    void socketConnectionClosed();
    void socketError( int code );

private:
    TQSocket *socket;
    enum State { Start, List, Data } state;
    void error( int code, const TQString& msg );
};



#endif // TQIP_H

