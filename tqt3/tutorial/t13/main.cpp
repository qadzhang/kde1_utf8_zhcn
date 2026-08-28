/****************************************************************
**
** TQt tutorial 13
**
****************************************************************/

#include <ntqapplication.h>

#include "gamebrd.h"


int main( int argc, char **argv )
{
    TQApplication::setColorSpec( TQApplication::CustomColor );
    TQApplication a( argc, argv );

    GameBoard gb;
    gb.setGeometry( 100, 100, 500, 355 );
    a.setMainWidget( &gb );
    gb.show();
    return a.exec();
}
