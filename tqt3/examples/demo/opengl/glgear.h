#ifndef GLGEAR_H
#define GLGEAR_H

#include "glcontrolwidget.h"

class GLGear : public GLControlWidget
{
    TQ_OBJECT

public:
    GLGear( TQWidget *parent = 0, const char *name = 0, WFlags f = 0 );

protected:
    void draw();
    void animate();
    void initializeGL();
    void resizeGL( int, int );
    void paintGL();
    
private:
    GLfloat view_rotx, view_roty, view_rotz;
    GLint gear1, gear2, gear3;
    GLfloat angle;
};

#endif // GLGEAR_H
