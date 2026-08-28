/****************************************************************************
**
** Copyright (C) 1992-2008 Trolltech ASA.  All rights reserved.
**
** This file is part of an example program for TQt.  This example
** program may be used, distributed and modified without limitation.
**
*****************************************************************************/

#ifndef GLINFO_H
#define GLINFO_H

#include <ntqstring.h>

class GLInfo
{
public:
    GLInfo();
    TQString info();//extensions();
/*     TQString configs(); */

protected:
    TQString infotext;
/*     TQString config; */
};
#endif
