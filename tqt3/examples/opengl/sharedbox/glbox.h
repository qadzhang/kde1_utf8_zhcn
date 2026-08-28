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
** This is a simple TQGLWidget displaying a box
**
****************************************************************************/

#ifndef GLBOX_H
#define GLBOX_H

#include <ntqgl.h>


class GLBox : public TQGLWidget
{
    TQ_OBJECT

public:

    GLBox( TQWidget* parent, const char* name, const TQGLWidget* shareWidget=0 );
    ~GLBox();

public slots:

    void		setXRotation( int degrees );
    void		setYRotation( int degrees );
    void		setZRotation( int degrees );

protected:

    void		initializeGL();
    void		paintGL();
    void		resizeGL( int w, int h );

    virtual GLuint 	makeObject();

private:

    GLuint		object;
    GLuint		localDisplayList;

    static GLuint	sharedDisplayList;
    static int		sharedListUsers;

    GLfloat xRot, yRot, zRot, scale;

};


#endif // GLBOX_H
