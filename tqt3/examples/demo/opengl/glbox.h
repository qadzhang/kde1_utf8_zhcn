/****************************************************************************
**
** Copyright (C) 1992-2008 Trolltech ASA.  All rights reserved.
**
** This file is part of an example program for TQt.  This example
** program may be used, distributed and modified without limitation.
**
*****************************************************************************/

/****************************************************************************
**
** This is a simple TQGLWidget displaying an openGL wireframe box
**
****************************************************************************/

#ifndef GLBOX_H
#define GLBOX_H

#include "glcontrolwidget.h"

class GLBox : public GLControlWidget
{
    TQ_OBJECT

public:

    GLBox( TQWidget* parent = 0, const char* name = 0, WFlags f = 0 );
    ~GLBox();

protected:

    void		initializeGL();
    void		paintGL();
    void		resizeGL( int w, int h );

    virtual GLuint 	makeObject();

private:
    GLuint object;
};


#endif // GLBOX_H
