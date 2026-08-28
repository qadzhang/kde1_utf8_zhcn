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
** This is a TQGLWidget displaying a group of teapots and a rubber-band
** in an overlay plane
**
****************************************************************************/

#ifndef GLBOX_H
#define GLBOX_H

#include <ntqgl.h>


class GLTeapots : public TQGLWidget
{
    TQ_OBJECT

public:

    GLTeapots( TQWidget* parent, const char* name );
    ~GLTeapots();

protected:

    void		initializeGL();
    void		paintGL();
    void		resizeGL( int w, int h );

    void		initializeOverlayGL();
    void		paintOverlayGL();
    void		resizeOverlayGL( int w, int h );

    void		mousePressEvent( TQMouseEvent* e );
    void		mouseMoveEvent( TQMouseEvent* e );
    void		mouseReleaseEvent( TQMouseEvent* e );

    void		renderTeapot( GLfloat x, GLfloat y, GLfloat ambr,
				      GLfloat ambg, GLfloat ambb, GLfloat difr,
				      GLfloat difg, GLfloat difb, 
				      GLfloat specr, GLfloat specg, 
				      GLfloat specb, GLfloat shine );

    void		teapot();

private:
    GLuint teapotList;
    TQPoint rubberP1;
    TQPoint rubberP2;
    bool rubberOn;
};


#endif // GLBOX_H
