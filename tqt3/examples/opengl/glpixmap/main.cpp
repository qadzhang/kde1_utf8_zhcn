/****************************************************************************
**
** Copyright (C) 1992-2008 Trolltech ASA.  All rights reserved.
**
** This file is part of an example program for TQt.  This example
** program may be used, distributed and modified without limitation.
**
*****************************************************************************/
//
// TQt OpenGL example: Shared Box
//
// A small example showing how to use OpenGL display list sharing
// 
// File: main.cpp
//
// The main() function 
// 

#include "globjwin.h"
#include <ntqapplication.h>
#include <ntqgl.h>

/*
  The main program is here. 
*/

int main( int argc, char **argv )
{
    TQApplication::setColorSpec( TQApplication::CustomColor );
    TQApplication a(argc,argv);			

    if ( !TQGLFormat::hasOpenGL() ) {
	tqWarning( "This system has no OpenGL support. Exiting." );
	return -1;
    }

    GLObjectWindow w;
    w.resize( 550, 350 );
    a.setMainWidget( &w );
    w.show();
    return a.exec();
}
