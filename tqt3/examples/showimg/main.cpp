/****************************************************************************
**
** Copyright (C) 1992-2008 Trolltech ASA.  All rights reserved.
**
** This file is part of an example program for TQt.  This example
** program may be used, distributed and modified without limitation.
**
*****************************************************************************/

#include "showimg.h"
#include "imagefip.h"
#include <ntqapplication.h>
#include <ntqimage.h>

int main( int argc, char **argv )
{
    if ( argc > 1 && TQString(argv[1]) == "-m" ) {
	TQApplication::setColorSpec( TQApplication::ManyColor );
	argc--;
	argv++;
    } 
    else if ( argc > 1 && TQString(argv[1]) == "-n" ) {
	TQApplication::setColorSpec( TQApplication::NormalColor );
	argc--;
	argv++;
    } 
    else {
	TQApplication::setColorSpec( TQApplication::CustomColor );
    }

    TQApplication a( argc, argv );

    ImageIconProvider iip;
    TQFileDialog::setIconProvider( &iip );

    if ( argc <= 1 ) {
	// Create a window which looks after its own existence.
	ImageViewer *w =
	    new ImageViewer(0, "new window", TQt::WDestructiveClose | TQt::WResizeNoErase );
	w->setCaption("TQt Example - Image Viewer");
	w->show();
    } else {
	for ( int i=1; i<argc; i++ ) {
	    // Create a window which looks after its own existence.
	    ImageViewer *w =
		new ImageViewer(0, argv[i], TQt::WDestructiveClose | TQt::WResizeNoErase );
	    w->setCaption("TQt Example - Image Viewer");
	    w->loadImage( argv[i] );
	    w->show();
	}
    }

    TQObject::connect(tqApp, TQ_SIGNAL(lastWindowClosed()), tqApp, TQ_SLOT(quit()));

    return a.exec();
}
