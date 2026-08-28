/****************************************************************************
**
** Drop site example implementation
**
** Created : 979899
**
** Copyright (C) 1997-2008 Trolltech ASA.  All rights reserved.
**
** This file is part of an example program for TQt.  This example
** program may be used, distributed and modified without limitation.
**
*****************************************************************************/

#ifndef DROPSITE_H
#define DROPSITE_H

#include <ntqlabel.h>
#include <ntqmovie.h>
#include "ntqdropsite.h"

class TQDragObject;

class DropSite: public TQLabel
{
    TQ_OBJECT
public:
    DropSite( TQWidget * parent = 0, const char * name = 0 );
    ~DropSite();

signals:
    void message( const TQString& );

protected:
    void dragEnterEvent( TQDragEnterEvent * );
    void dragMoveEvent( TQDragMoveEvent * );
    void dragLeaveEvent( TQDragLeaveEvent * );
    void dropEvent( TQDropEvent * );
    void backgroundColorChange( const TQColor& );

    // this is a normal even
    void mousePressEvent( TQMouseEvent * );
};

class DragMoviePlayer : public TQObject {
    TQ_OBJECT
    TQDragObject* dobj;
    TQMovie movie;
public:
    DragMoviePlayer(TQDragObject*);
private slots:
    void updatePixmap( const TQRect& );
};


#endif
