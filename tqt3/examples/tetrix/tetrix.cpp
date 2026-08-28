/****************************************************************************
**
** Copyright (C) 1992-2008 Trolltech ASA.  All rights reserved.
**
** This file is part of an example program for TQt.  This example
** program may be used, distributed and modified without limitation.
**
*****************************************************************************/

#include "qtetrix.h"
#include "qdragapp.h"
#include "ntqfont.h"

int main( int argc, char **argv )
{
    TQApplication::setColorSpec( TQApplication::CustomColor );
    TQDragApplication a(argc,argv);
    TQTetrix *tetrix = new TQTetrix;
    tetrix->setCaption("Tetrix");
    a.setMainWidget(tetrix);
    tetrix->setCaption("TQt Example - Tetrix");
    tetrix->show();
    return a.exec();
}
