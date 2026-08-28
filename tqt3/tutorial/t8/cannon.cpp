/****************************************************************
**
** Implementation CannonField class, TQt tutorial 8
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
    TQString s = "Angle = " + TQString::number( ang );
    TQPainter p( this );
    p.drawText( 200, 200, s );
}


TQSizePolicy CannonField::sizePolicy() const
{
    return TQSizePolicy( TQSizePolicy::Expanding, TQSizePolicy::Expanding );
}

