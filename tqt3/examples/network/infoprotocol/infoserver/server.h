/****************************************************************************
**
** Copyright (C) 1992-2008 Trolltech ASA.  All rights reserved.
**
** This file is part of an example program for TQt.  This example
** program may be used, distributed and modified without limitation.
**
*****************************************************************************/

#ifndef SERVER_H
#define SERVER_H

#include <ntqsocket.h>
#include <ntqserversocket.h>

#include "infodata.h"
#include "serverbase.h"

static const TQ_UINT16 infoPort = 42417;


/*
  The ServerInfo class provides a small GUI for the server. It also creates the
  SimpleServer and as a result the server.
*/
class ServerInfo : public ServerInfoBase
{
    TQ_OBJECT
public:
    ServerInfo( TQ_UINT16 port = infoPort, TQWidget *parent = 0, const char *name = 0 );

private slots:
    void newConnect();
};


class SimpleServer : public TQServerSocket
{
    TQ_OBJECT
public:
    SimpleServer( TQ_UINT16 port = infoPort, TQObject* parent = 0, const char *name = 0 );
    void newConnection( int socket );

signals:
    void newConnect();

private:
    InfoData info;
};


class ClientSocket : public TQSocket
{
    TQ_OBJECT
public:
    ClientSocket( int sock, InfoData *i, TQObject *parent = 0, const char *name = 0 );

private slots:
    void readClient();
    void connectionClosed();

private:
    TQString processCommand( const TQString& command );
    InfoData *info;
};


#endif //SERVER_H

