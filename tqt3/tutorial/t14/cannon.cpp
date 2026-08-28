/****************************************************************
**
** Implementation CannonField class, TQt tutorial 14
**
****************************************************************/

#include "cannon.h"
#include <ntqtimer.h>
#include <ntqpainter.h>
#include <ntqpixmap.h>
#include <ntqdatetime.h>

#include <math.h>
#include <stdlib.h>


CannonField::CannonField( TQWidget *parent, const char *name )
        : TQWidget( parent, name )
{
    ang = 45;
    f = 0;
    timerCount = 0;
    autoShootTimer = new TQTimer( this, "movement handler" );
    connect( autoShootTimer, TQ_SIGNAL(timeout()),
	     this, TQ_SLOT(moveShot()) );
    shoot_ang = 0;
    shoot_f = 0;
    target = TQPoint( 0, 0 );
    gameEnded = false;
    barrelPressed = false;
    setPalette( TQPalette( TQColor( 250, 250, 200) ) );
    newTarget();
}


void CannonField::setAngle( int degrees )
{
    if ( degrees < 5 )
	degrees = 5;
    if ( degrees > 70 )
	degrees = 70;
    if ( ang == degrees )
	return;
    ang = degrees;
    repaint( cannonRect(), false );
    emit angleChanged( ang );
}


void CannonField::setForce( int newton )
{
    if ( newton < 0 )
	newton = 0;
    if ( f == newton )
	return;
    f = newton;
    emit forceChanged( f );
}


void CannonField::shoot()
{
    if ( isShooting() )
	return;
    timerCount = 0;
    shoot_ang = ang;
    shoot_f = f;
    autoShootTimer->start( 50 );
    emit canShoot( false );
}


void  CannonField::newTarget()
{
    static bool first_time = true;
    if ( first_time ) {
	first_time = false;
	TQTime midnight( 0, 0, 0 );
	srand( midnight.secsTo(TQTime::currentTime()) );
    }
    TQRegion r( targetRect() );
    target = TQPoint( 200 + rand() % 190,
		     10  + rand() % 255 );
    repaint( r.unite( targetRect() ) );
}

void CannonField::setGameOver()
{
    if ( gameEnded )
	return;
    if ( isShooting() )
	autoShootTimer->stop();
    gameEnded = true;
    repaint();
}

void CannonField::restartGame()
{
    if ( isShooting() )
	autoShootTimer->stop();
    gameEnded = false;
    repaint();
    emit canShoot( true );
}

void CannonField::moveShot()
{
    TQRegion r( shotRect() );
    timerCount++;

    TQRect shotR = shotRect();

    if ( shotR.intersects( targetRect() ) ) {
	autoShootTimer->stop();
	emit hit();
	emit canShoot( true );
    } else if ( shotR.x() > width() || shotR.y() > height() ||
		shotR.intersects(barrierRect()) ) {
	autoShootTimer->stop();
	emit missed();
	emit canShoot( true );
    } else {
	r = r.unite( TQRegion( shotR ) );
    }

    repaint( r );
}


void CannonField::mousePressEvent( TQMouseEvent *e )
{
    if ( e->button() != LeftButton )
	return;
    if ( barrelHit( e->pos() ) )
	barrelPressed = true;
}


void CannonField::mouseMoveEvent( TQMouseEvent *e )
{
    if ( !barrelPressed )
	return;
    TQPoint pnt = e->pos();
    if ( pnt.x() <= 0 )
	pnt.setX( 1 );
    if ( pnt.y() >= height() )
	pnt.setY( height() - 1 );
    double rad = atan(((double)rect().bottom()-pnt.y())/pnt.x());
    setAngle( tqRound ( rad*180/3.14159265 ) );
}


void CannonField::mouseReleaseEvent( TQMouseEvent *e )
{
    if ( e->button() == LeftButton )
	barrelPressed = false;
}


void CannonField::paintEvent( TQPaintEvent *e )
{
    TQRect updateR = e->rect();
    TQPainter p( this );

    if ( gameEnded ) {
	p.setPen( black );
	p.setFont( TQFont( "Courier", 48, TQFont::Bold ) );
	p.drawText( rect(), AlignCenter, "Game Over" );
    }
    if ( updateR.intersects( cannonRect() ) )
	paintCannon( &p );
    if ( updateR.intersects( barrierRect() ) )
	paintBarrier( &p );
    if ( isShooting() && updateR.intersects( shotRect() ) )
	paintShot( &p );
    if ( !gameEnded && updateR.intersects( targetRect() ) )
	paintTarget( &p );
}

void CannonField::paintShot( TQPainter *p )
{
    p->setBrush( black );
    p->setPen( NoPen );
    p->drawRect( shotRect() );
}


void CannonField::paintTarget( TQPainter *p )
{
    p->setBrush( red );
    p->setPen( black );
    p->drawRect( targetRect() );
}

void CannonField::paintBarrier( TQPainter *p )
{
    p->setBrush( yellow );
    p->setPen( black );
    p->drawRect( barrierRect() );
}

const TQRect barrelRect(33, -4, 15, 8);

void CannonField::paintCannon( TQPainter *p )
{
    TQRect cr = cannonRect();
    TQPixmap pix( cr.size() );
    pix.fill( this, cr.topLeft() );

    TQPainter tmp( &pix );
    tmp.setBrush( blue );
    tmp.setPen( NoPen );

    tmp.translate( 0, pix.height() - 1 );
    tmp.drawPie( TQRect( -35,-35, 70, 70 ), 0, 90*16 );
    tmp.rotate( -ang );
    tmp.drawRect( barrelRect );
    tmp.end();

    p->drawPixmap( cr.topLeft(), pix );
}


TQRect CannonField::cannonRect() const
{
    TQRect r( 0, 0, 50, 50 );
    r.moveBottomLeft( rect().bottomLeft() );
    return r;
}


TQRect CannonField::shotRect() const
{
    const double gravity = 4;

    double time      = timerCount / 4.0;
    double velocity  = shoot_f;
    double radians   = shoot_ang*3.14159265/180;

    double velx      = velocity*cos( radians );
    double vely      = velocity*sin( radians );
    double x0        = ( barrelRect.right()  + 5 )*cos(radians);
    double y0        = ( barrelRect.right()  + 5 )*sin(radians);
    double x         = x0 + velx*time;
    double y         = y0 + vely*time - 0.5*gravity*time*time;

    TQRect r = TQRect( 0, 0, 6, 6 );
    r.moveCenter( TQPoint( tqRound(x), height() - 1 - tqRound(y) ) );
    return r;
}


TQRect CannonField::targetRect() const
{
    TQRect r( 0, 0, 20, 10 );
    r.moveCenter( TQPoint(target.x(),height() - 1 - target.y()) );
    return r;
}


TQRect CannonField::barrierRect() const
{
    return TQRect( 145, height() - 100, 15, 100 );
}


bool CannonField::barrelHit( const TQPoint &p ) const
{
    TQWMatrix mtx;
    mtx.translate( 0, height() - 1 );
    mtx.rotate( -ang );
    mtx = mtx.invert();
    return barrelRect.contains( mtx.map(p) );
}


bool CannonField::isShooting() const
{
    return autoShootTimer->isActive();
}


TQSize CannonField::sizeHint() const
{
    return TQSize( 400, 300 );
}


TQSizePolicy CannonField::sizePolicy() const
{
    return TQSizePolicy( TQSizePolicy::Expanding, TQSizePolicy::Expanding );
}
