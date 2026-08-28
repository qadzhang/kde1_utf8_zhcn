/****************************************************************************
**
** Copyright (C) 1992-2008 Trolltech ASA.  All rights reserved.
**
** This file is part of an example program for TQt.  This example
** program may be used, distributed and modified without limitation.
**
*****************************************************************************/

#include <ntqapplication.h>

#include "server.h"

int main( int argc, char** argv )
{
    TQApplication app( argc, argv );
    TQ_UINT16 port = ( argc > 1 ) ? TQString( argv[ 1 ] ).toInt() : infoPort;
    ServerInfo info( port, 0, "server info" );
    app.setMainWidget( &info );
    info.show();
    return app.exec();
}
