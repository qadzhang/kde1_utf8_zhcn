/****************************************************************************
**
** Copyright (C) 1992-2008 Trolltech ASA.  All rights reserved.
**
** This file is part of an example program for TQt.  This example
** program may be used, distributed and modified without limitation.
**
*****************************************************************************/

#include <ntqapplication.h>
#include "application.h"

int main( int argc, char ** argv ) {
    TQApplication a( argc, argv );
    ApplicationWindow * mw = new ApplicationWindow();
    a.setMainWidget(mw);
    mw->setCaption( "TQt Example - Multiple Documents Interface (MDI)" );
    mw->show();
    a.connect( &a, TQ_SIGNAL(lastWindowClosed()), &a, TQ_SLOT(quit()) );
    int res = a.exec();
    return res;
}
