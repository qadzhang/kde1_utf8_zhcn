/****************************************************************************
**
** Copyright (C) 1992-2008 Trolltech ASA.  All rights reserved.
**
** This file is part of an example program for TQt.  This example
** program may be used, distributed and modified without limitation.
**
*****************************************************************************/

#ifndef NNTP_H
#define NNTP_H

#include <ntqsocket.h>
#include <ntqnetworkprotocol.h>

class Nntp : public TQNetworkProtocol
{
    TQ_OBJECT

public:
    Nntp();
    virtual ~Nntp();
    virtual int supportedOperations() const;

protected:
    virtual void operationListChildren( TQNetworkOperation *op );
    virtual void operationGet( TQNetworkOperation *op );

    TQSocket *commandSocket;
    bool connectionReady;
    bool readGroups;
    bool readArticle;

private:
    bool checkConnection( TQNetworkOperation *op );
    void close();
    void parseGroups();
    void parseArticle();

protected slots:
    void hostFound();
    void connected();
    void closed();
    void readyRead();
    void error( int );

};

#endif
