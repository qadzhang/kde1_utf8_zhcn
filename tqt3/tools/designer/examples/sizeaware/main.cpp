#include <ntqapplication.h>
#include "sizeaware.h"

int main( int argc, char ** argv )
{
    TQApplication app( argc, argv );
    SizeAware *sizeaware = new SizeAware( 0, 0, true );
    sizeaware->exec();
    sizeaware->destroy();
    return 0; 
}
