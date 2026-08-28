/****************************************************************
**
** Implementation CannonField class, TQt tutorial 9
**
****************************************************************/

#include "cannon.h"
#include <ntqpainter.h>


CannonField::CannonField( TQWidget *parent, const char *name )
        : TQWidget( parent, name )
{
    ang = 45;
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
    repaint();
    emit angleChanged( ang );
}


void CannonField::paintEvent( TQPaintEvent * )
{
    TQPainter p( this );

    p.setBrush( blue );
    p.setPen( NoPen );

    p.translate( 0, rect().bottom() );
    p.drawPie( TQRect(-35, -35, 70, 70), 0, 90*16 );
    p.rotate( -ang );
    p.drawRect( TQRect(33, -4, 15, 8) );
}


TQSizePolicy CannonField::sizePolicy() const
{
    return TQSizePolicy( TQSizePolicy::Expanding, TQSizePolicy::Expanding );
}
