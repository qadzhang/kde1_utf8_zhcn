#include <ntqapplication.h>
#include "helpdemo.h"

int main( int argc, char ** argv )
{
    TQApplication a( argc, argv );
    HelpDemo help;
    help.show();
    a.connect( &a, TQ_SIGNAL( lastWindowClosed() ), &a, TQ_SLOT( quit() ) );
    return a.exec();
}
