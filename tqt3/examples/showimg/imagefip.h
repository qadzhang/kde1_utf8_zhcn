/****************************************************************************
**
** Copyright (C) 1992-2008 Trolltech ASA.  All rights reserved.
**
** This file is part of an example program for TQt.  This example
** program may be used, distributed and modified without limitation.
**
*****************************************************************************/

#ifndef IMAGEFIP_H
#define IMAGEFIP_H

#include <ntqfiledialog.h>
#include <ntqstrlist.h>
#include <ntqpixmap.h>

class ImageIconProvider : public TQFileIconProvider
{
    TQ_OBJECT
    TQStrList fmts;
    TQPixmap imagepm;

public:
    ImageIconProvider( TQWidget *parent=0, const char *name=0 );
    ~ImageIconProvider();

    const TQPixmap * pixmap( const TQFileInfo &fi );
};


#endif // IMAGEFIP_H
