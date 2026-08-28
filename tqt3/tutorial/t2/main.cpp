/****************************************************************
**
** TQt tutorial 2
**
****************************************************************/

#include <ntqapplication.h>
#include <ntqpushbutton.h>
#include <ntqfont.h>


int main( int argc, char **argv )
{
    TQApplication a( argc, argv );

    TQPushButton quit( "Quit", 0 );
    quit.resize( 75, 30 );
    quit.setFont( TQFont( "Times", 18, TQFont::Bold ) );

    TQObject::connect( &quit, TQ_SIGNAL(clicked()), &a, TQ_SLOT(quit()) );

    a.setMainWidget( &quit );
    quit.show();
    return a.exec();
}
