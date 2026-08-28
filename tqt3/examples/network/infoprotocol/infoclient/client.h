/****************************************************************************
**
** Copyright (C) 1992-2008 Trolltech ASA.  All rights reserved.
**
** This file is part of an example program for TQt.  This example
** program may be used, distributed and modified without limitation.
**
*****************************************************************************/

#ifndef CLIENT_H
#define CLIENT_H

#include "clientbase.h"

class TQSocket;
class TQTextEdit;
class TQLineEdit;
class TQListBox;
class TQLabel;

static const TQ_UINT16 infoPort = 42417;

class ClientInfo : public ClientInfoBase
{
    TQ_OBJECT
public:
    ClientInfo( TQWidget *parent = 0, const char *name = 0 );

private:
    enum Operation { List, Get };

private slots:
    void connectToServer();
    void selectItem( const TQString& item );
    void stepBack();
    void sendToServer( Operation op, const TQString& location );
    void socketConnected();
    void socketReadyRead();
    void socketConnectionClosed();
    void socketError( int code );

private:
    TQSocket *socket;
};

#endif // CLIENT_H

