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

#include <ntqurloperator.h>

#include "clientbase.h"


class ClientInfo : public ClientInfoBase
{
    TQ_OBJECT

public:
    ClientInfo( TQWidget *parent = 0, const char *name = 0 );

private slots:
    void downloadFile();
    void newData( const TQByteArray &ba );

private:
    TQUrlOperator op;
    TQString getOpenFileName();
};

#endif // CLIENT_H

