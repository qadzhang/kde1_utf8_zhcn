#include <ntqapplication.h>
#include "distributor.h"

int main( int argc, char ** argv )
{
    TQApplication a( argc, argv );
    Distributor w;
    w.show();
    a.connect( &a, TQ_SIGNAL( lastWindowClosed() ), &a, TQ_SLOT( quit() ) );
    return a.exec();
}
