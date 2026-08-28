/****************************************************************************
**
** Copyright (C) 1992-2008 Trolltech ASA.  All rights reserved.
**
** This file is part of an example program for TQt.  This example
** program may be used, distributed and modified without limitation.
**
*****************************************************************************/

#include <ntqapplication.h>
#include <ntqnetwork.h>

#include "nntp.h"
#include "view.h"

int main( int argc, char **argv )
{
    TQApplication a( argc, argv );

    tqInitNetworkProtocols();
    TQNetworkProtocol::registerNetworkProtocol( "nntp", new TQNetworkProtocolFactory<Nntp> );
    
    View v;
    v.resize( 600, 600 );
    v.show();
    a.setMainWidget( &v );
    
    return a.exec();
}
