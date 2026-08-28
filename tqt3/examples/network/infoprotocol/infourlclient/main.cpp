/****************************************************************************
**
** Copyright (C) 1992-2008 Trolltech ASA.  All rights reserved.
**
** This file is part of an example program for TQt.  This example
** program may be used, distributed and modified without limitation.
**
*****************************************************************************/

#include <ntqapplication.h>

#include "qip.h"
#include "client.h"

int main( int argc, char** argv )
{
    TQApplication app( argc, argv );
    TQNetworkProtocol::registerNetworkProtocol( "qip", new TQNetworkProtocolFactory< Qip > );
    ClientInfo info;
    app.setMainWidget( &info );
    info.show();
    return app.exec();
}
