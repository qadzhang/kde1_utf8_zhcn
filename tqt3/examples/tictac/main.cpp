/****************************************************************************
**
** Copyright (C) 1992-2008 Trolltech ASA.  All rights reserved.
**
** This file is part of an example program for TQt.  This example
** program may be used, distributed and modified without limitation.
**
*****************************************************************************/

#include <ntqapplication.h>
#include <stdlib.h>
#include "tictac.h"


int main( int argc, char **argv )
{
    TQApplication a( argc, argv );
    int n = 3;
    if ( argc == 2 )				// get board size n
        n = atoi(argv[1]);
    if ( n < 3 || n > 10 ) {			// out of range
        tqWarning( "%s: Board size must be from 3x3 to 10x10", argv[0] );
        return 1;
    }
    TicTacToe ttt( n );				// create game
    a.setMainWidget( &ttt );
    ttt.setCaption("TQt Example - TicTac");
    ttt.show();					// show widget
    return a.exec();				// go
}
