/****************************************************************************
**
** Copyright (C) 1992-2008 Trolltech ASA.  All rights reserved.
**
** This file is part of an example program for TQt.  This example
** program may be used, distributed and modified without limitation.
**
*****************************************************************************/

#include "archivedialog.h"
#include <ntqapplication.h>

int main(int argc, char **argv)
{
    TQApplication a( argc, argv );
    ArchiveDialog ad;
    ad.show();

    TQObject::connect( &a, TQ_SIGNAL(lastWindowClosed()),
		      &a, TQ_SLOT(quit()) );
    
    return a.exec();
}
