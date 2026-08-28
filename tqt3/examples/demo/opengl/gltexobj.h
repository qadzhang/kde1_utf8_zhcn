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

#ifndef GLTEXOBJ_H
#define GLTEXOBJ_H

#include "glcontrolwidget.h"

class GLTexobj : public GLControlWidget
{
    TQ_OBJECT

public:
    GLTexobj( TQWidget* parent, const char* name = 0, WFlags f = 0 );
    ~GLTexobj();

protected:
    void		animate();
    void		initializeGL();
    void		paintGL();
    void		resizeGL( int w, int h );

    virtual GLuint 	makeObject( const TQImage& tex1, const TQImage& tex2 );

    void		setRotationImpulse( double x, double y, double z );

private:
    GLuint object;
    double impX, impY, impZ;
};

#endif // GLTEXOBJ_H
