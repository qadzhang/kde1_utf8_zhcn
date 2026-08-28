/****************************************************************
**
** Implementation of ArrowPad class, translation tutorial 2
**
****************************************************************/

#include "arrowpad.h"

#include <ntqpushbutton.h>

ArrowPad::ArrowPad( TQWidget *parent, const char *name )
    : TQGrid( 3, Horizontal, parent, name )
{
    setMargin( 10 );
    setSpacing( 10 );

    skip();
    (void) new TQPushButton( tr("&Up"), this );
    skip();
    (void) new TQPushButton( tr("&Left"), this );
    skip();
    (void) new TQPushButton( tr("&Right"), this );
    skip();
    (void) new TQPushButton( tr("&Down"), this );
    skip();
}

void ArrowPad::skip()
{
    (void) new TQWidget( this );
}
