/****************************************************************
**
** Implementation CannonField class, TQt tutorial 10
**
****************************************************************/

#include "cannon.h"
#include <ntqpainter.h>
#include <ntqpixmap.h>


CannonField::CannonField( TQWidget *parent, const char *name )
        : TQWidget( parent, name )
{
    ang = 45;
    f = 0;
    setPalette( TQPalette( TQColor( 250, 250, 200) ) );
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


void CannonField::paintEvent( TQPaintEvent *e )
{
    if ( !e->rect().intersects( cannonRect() ) )
	return;

    TQRect cr = cannonRect();
    TQPixmap pix( cr.size() );
    pix.fill( this, cr.topLeft() );

    TQPainter p( &pix );
    p.setBrush( blue );
    p.setPen( NoPen );
    p.translate( 0, pix.height() - 1 );
    p.drawPie( TQRect( -35,-35, 70, 70 ), 0, 90*16 );
    p.rotate( -ang );
    p.drawRect( TQRect(33, -4, 15, 8) );
    p.end();

    p.begin( this );
    p.drawPixmap( cr.topLeft(), pix );
}


TQRect CannonField::cannonRect() const
{
    TQRect r( 0, 0, 50, 50 );
    r.moveBottomLeft( rect().bottomLeft() );
    return r;
}


TQSizePolicy CannonField::sizePolicy() const
{
    return TQSizePolicy( TQSizePolicy::Expanding, TQSizePolicy::Expanding );
}
