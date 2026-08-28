#include <ntqapplication.h>

#include "startup.h"

int main( int argc, char **argv )
{
    TQApplication a( argc, argv );
    StartUp s;
    TQObject::connect( &a, TQ_SIGNAL(lastWindowClosed()), &a, TQ_SLOT(quit()) );
    return a.exec();
}
