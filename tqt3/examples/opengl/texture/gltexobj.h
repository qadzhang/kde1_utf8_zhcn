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

#include <ntqgl.h>


class GLTexobj : public TQGLWidget
{
    TQ_OBJECT

public:

    GLTexobj( TQWidget* parent, const char* name );
    ~GLTexobj();

public slots:

    void		setXRotation( int degrees );
    void		setYRotation( int degrees );
    void		setZRotation( int degrees );
    void		toggleAnimation();

protected:

    void		initializeGL();
    void		paintGL();
    void		resizeGL( int w, int h );

    virtual GLuint 	makeObject( const TQImage& tex1, const TQImage& tex2 );

private:
    bool animation;
    GLuint object;
    GLfloat xRot, yRot, zRot, scale;
    TQTimer* timer;
};


#endif // GLTEXOBJ_H
