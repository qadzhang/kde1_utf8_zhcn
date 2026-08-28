/****************************************************************
**
** Implementation of LCDRange class, TQt tutorial 8
**
****************************************************************/

#include "lcdrange.h"

#include <ntqslider.h>
#include <ntqlcdnumber.h>

LCDRange::LCDRange( TQWidget *parent, const char *name )
        : TQVBox( parent, name )
{
    TQLCDNumber *lcd  = new TQLCDNumber( 2, this, "lcd"  );
    slider = new TQSlider( Horizontal, this, "slider" );
    slider->setRange( 0, 99 );
    slider->setValue( 0 );
    connect( slider, TQ_SIGNAL(valueChanged(int)),
	     lcd, TQ_SLOT(display(int)) );
    connect( slider, TQ_SIGNAL(valueChanged(int)),
	     TQ_SIGNAL(valueChanged(int)) );

    setFocusProxy( slider );
}

int LCDRange::value() const
{
    return slider->value();
}

void LCDRange::setValue( int value )
{
    slider->setValue( value );
}

void LCDRange::setRange( int minVal, int maxVal )
{
    if ( minVal < 0 || maxVal > 99 || minVal > maxVal ) {
      tqWarning( "LCDRange::setRange(%d,%d)\n"
	       "\tRange must be 0..99\n"
	       "\tand minVal must not be greater than maxVal",
	       minVal, maxVal );
      return;
    }
    slider->setRange( minVal, maxVal );
}
