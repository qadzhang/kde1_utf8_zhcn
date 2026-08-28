/****************************************************************************
**
** Copyright (C) 1992-2008 Trolltech ASA.  All rights reserved.
**
** This file is part of an example program for TQt.  This example
** program may be used, distributed and modified without limitation.
**
*****************************************************************************/

#include <ntqapplication.h>

#include "mainwindow.h"

int main( int argc, char ** argv )
{
    TQApplication a( argc, argv );

    ABMainWindow *mw = new ABMainWindow();
    mw->setCaption( "TQt Example - Addressbook" );
    a.setMainWidget( mw );
    mw->show();

    a.connect( &a, TQ_SIGNAL( lastWindowClosed() ), &a, TQ_SLOT( quit() ) );
    int result = a.exec();
    delete mw;
    return result;
}
