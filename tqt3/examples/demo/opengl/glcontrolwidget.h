#ifndef GLCONTROLWIDGET_H
#define GLCONTROLWIDGET_H

#include <ntqgl.h>

class GLControlWidget : public TQGLWidget
{
    TQ_OBJECT

public:
    GLControlWidget( TQWidget *parent, const char *name = 0, TQGLWidget *share = 0, WFlags f = 0 );

    virtual void	transform();

public slots:
    void		setXRotation( double degrees );
    void		setYRotation( double degrees );
    void		setZRotation( double degrees );

    void		setScale( double s );

    void		setXTrans( double x );
    void		setYTrans( double y );
    void		setZTrans( double z );

    virtual void	setRotationImpulse( double x, double y, double z );
    virtual void	setTranslationImpulse( double x, double y, double z );
    void		drawText();

protected:
    void		setAnimationDelay( int ms );
    void		mousePressEvent( TQMouseEvent *e );
    void		mouseReleaseEvent( TQMouseEvent *e );
    void		mouseMoveEvent( TQMouseEvent * );
    void		mouseDoubleClickEvent( TQMouseEvent * );
    void		wheelEvent( TQWheelEvent * );

    void		showEvent( TQShowEvent * );
    void		hideEvent( TQHideEvent * );

    GLfloat xRot, yRot, zRot;
    GLfloat xTrans, yTrans, zTrans;
    GLfloat scale;
    bool animation;

protected slots:
    virtual void	animate();

private:
    bool wasAnimated;
    TQPoint oldPos;
    TQTimer* timer;
    int delay;
};

#endif
