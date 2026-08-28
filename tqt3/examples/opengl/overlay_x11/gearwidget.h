/****************************************************************************
**
** Definition of a simple TQt OpenGL widget
**
** Copyright (C) 1999-2008 Trolltech ASA.  All rights reserved.
**
** This file is part of an example program for TQt.  This example
** program may be used, distributed and modified without limitation.
**
*****************************************************************************/

#ifndef GEAR_H
#define GEAR_H

#include <ntqgl.h>

class GearWidget : public TQGLWidget
{
public:
    GearWidget( TQWidget *parent=0, const char *name=0 );

protected:
    void initializeGL();
    void resizeGL( int, int );
    void paintGL();
};


#endif
