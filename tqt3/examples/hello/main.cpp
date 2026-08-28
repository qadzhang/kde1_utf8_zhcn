/****************************************************************************
**
** Copyright (C) 1992-2008 Trolltech ASA.  All rights reserved.
**
** This file is part of an example program for TQt.  This example
** program may be used, distributed and modified without limitation.
**
*****************************************************************************/

#include "hello.h"
#include <ntqapplication.h>


/*
  The program starts here. It parses the command line and builds a message
  string to be displayed by the Hello widget.
*/

int main( int argc, char **argv )
{
    TQApplication a(argc,argv);
    TQString s;
    for ( int i=1; i<argc; i++ ) {
	s += argv[i];
	if ( i<argc-1 )
	    s += " ";
    }
    if ( s.isEmpty() )
	s = "Hello, World";
    Hello h( s );
#ifndef TQT_NO_WIDGET_TOPEXTRA	// for TQt/Embedded minimal build
    h.setCaption( "TQt says hello" );
#endif
    TQObject::connect( &h, TQ_SIGNAL(clicked()), &a, TQ_SLOT(quit()) );
    h.setFont( TQFont("times",32,TQFont::Bold) );		// default font
    h.setBackgroundColor( TQt::white );			// default bg color
    a.setMainWidget( &h );
    h.show();
    return a.exec();
}
