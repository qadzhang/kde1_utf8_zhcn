/****************************************************************************
**
** Custom MIME type implementation example
**
** Created : 979899
**
** Copyright (C) 1997-2008 Trolltech ASA.  All rights reserved.
**
** This file is part of an example program for TQt.  This example
** program may be used, distributed and modified without limitation.
**
*****************************************************************************/

#ifndef SECRETDRAG_H
#define SECRETDRAG_H

#include <ntqdragobject.h>
#include <ntqlabel.h>

class SecretDrag: public TQStoredDrag {
public:
    SecretDrag( uchar, TQWidget * parent = 0, const char * name = 0 );
    ~SecretDrag() {};

    static bool canDecode( TQDragMoveEvent* e );
    static bool decode( TQDropEvent* e, TQString& s );
};


class SecretSource: public TQLabel
{
public:
    SecretSource( int secret, TQWidget *parent = 0, const char * name = 0 );
    ~SecretSource();

protected:
    void mousePressEvent( TQMouseEvent * );
private:
    int mySecret;
};

#endif
