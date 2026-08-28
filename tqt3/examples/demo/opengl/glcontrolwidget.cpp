#include "glcontrolwidget.h"

#include <ntqcursor.h>
#include <ntqtimer.h>

#include <math.h>

GLControlWidget::GLControlWidget( TQWidget *parent, const char *name, TQGLWidget *share, WFlags f )
: TQGLWidget( parent, name, share, f ),
  xRot(0),yRot(0),zRot(0),xTrans(0),yTrans(0),zTrans(-10.0),scale(5.0), animation(true), wasAnimated(false), delay( 50 )
{
    setCursor( pointingHandCursor  );
    timer = new TQTimer( this );
    connect( timer, TQ_SIGNAL(timeout()), TQ_SLOT(animate()) );
    timer->start( delay );
}

void GLControlWidget::transform()
{
    glTranslatef( xTrans, yTrans, zTrans );
    glScalef( scale, scale, scale );

    glRotatef( xRot, 1.0, 0.0, 0.0 );
    glRotatef( yRot, 0.0, 1.0, 0.0 );
    glRotatef( zRot, 0.0, 0.0, 1.0 );
}

void GLControlWidget::drawText()
{
    glPushAttrib( GL_LIGHTING_BIT | GL_TEXTURE_BIT );
    glDisable( GL_LIGHTING );
    glDisable( GL_TEXTURE_2D );
    qglColor( white );
    TQString str( "Rendering text in OpenGL is easy with TQt" );
    TQFontMetrics fm( font() );
    renderText( (width() - fm.width( str )) / 2, 15, str );
    TQFont f( "courier", 8 );
    TQFontMetrics fmc( f );
    qglColor( TQColor("skyblue") );
    int x, y, z;
    x = (xRot >= 0) ? (int) xRot % 360 : 359 - (TQABS((int) xRot) % 360);
    y = (yRot >= 0) ? (int) yRot % 360 : 359 - (TQABS((int) yRot) % 360);
    z = (zRot >= 0) ? (int) zRot % 360 : 359 - (TQABS((int) zRot) % 360);
    str.sprintf( "Rot X: %03d - Rot Y: %03d - Rot Z: %03d", x, y, z );
    renderText( (width() - fmc.width( str )) / 2, height() - 15, str, f );
    glPopAttrib();
}

/*!
  Set the rotation angle of the object to \e degrees around the X axis.
*/
void GLControlWidget::setXRotation( double degrees )
{
    xRot = (GLfloat)fmod(degrees, 360.0);
    updateGL();
}

/*!
  Set the rotation angle of the object to \e degrees around the Y axis.
*/
void GLControlWidget::setYRotation( double degrees )
{
    yRot = (GLfloat)fmod(degrees, 360.0);
    updateGL();
}


/*!
  Set the rotation angle of the object to \e degrees around the Z axis.
*/
void GLControlWidget::setZRotation( double degrees )
{
    zRot = (GLfloat)fmod(degrees, 360.0);
    updateGL();
}

void GLControlWidget::setScale( double s )
{
    scale = s;
    updateGL();
}

void GLControlWidget::setXTrans( double x )
{
    xTrans = x;
    updateGL();
}

void GLControlWidget::setYTrans( double y )
{
    yTrans = y;
    updateGL();
}

void GLControlWidget::setZTrans( double z )
{
    zTrans = z;
    updateGL();
}

void GLControlWidget::setRotationImpulse( double x, double y, double z )
{
    setXRotation( xRot + 180*x );
    setYRotation( yRot + 180*y );
    setZRotation( zRot - 180*z );
}

void GLControlWidget::setTranslationImpulse( double x, double y, double z )
{
    setXTrans( xTrans + 2*x );
    setYTrans( yTrans - 2*y );
    setZTrans( zTrans + 2*z );
}

void GLControlWidget::mousePressEvent( TQMouseEvent *e )
{
    e->accept();
    oldPos = e->pos();
}

void GLControlWidget::mouseReleaseEvent( TQMouseEvent *e )
{
    e->accept();
    oldPos = e->pos();
}

void GLControlWidget::mouseMoveEvent( TQMouseEvent *e )
{
    e->accept();
    double dx = e->x() - oldPos.x();
    double dy = e->y() - oldPos.y();

    oldPos = e->pos();

    double rx = dx / width();
    double ry = dy / height();

    if ( e->state() == LeftButton )
	setRotationImpulse( ry, rx, 0 );
    else if ( e->state() == RightButton )
	setRotationImpulse( ry, 0, rx );
    else if ( e->state() == MidButton )
	setTranslationImpulse( rx, ry, 0 );
    else if ( e->state() == ( LeftButton | RightButton ) )
	setTranslationImpulse( rx, 0, ry );
}

void GLControlWidget::wheelEvent( TQWheelEvent *e )
{
    e->accept();
    if ( scale <= ( (double)e->delta() / 1000 ) )
	return;
    setScale( scale - ( (double)e->delta() / 1000 ));
}

void GLControlWidget::mouseDoubleClickEvent( TQMouseEvent * )
{
    if ( delay <= 0 )
	return;

    animation = !animation;
    if ( animation )
	timer->start( delay );
    else
	timer->stop();
}

void GLControlWidget::showEvent( TQShowEvent *e )
{
    if ( wasAnimated && !timer->isActive() )
	timer->start( delay );

    TQGLWidget::showEvent( e );
}

void GLControlWidget::hideEvent( TQHideEvent *e )
{
    wasAnimated = timer->isActive();
    timer->stop();
    TQGLWidget::hideEvent( e );
}

void GLControlWidget::animate()
{
}

void GLControlWidget::setAnimationDelay( int ms )
{
    timer->stop();
    delay = ms;
    if ( animation ) {
	wasAnimated = true;
	timer->start( delay );
    }
}
