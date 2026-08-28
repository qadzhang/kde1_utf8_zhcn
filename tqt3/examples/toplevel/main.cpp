#include <ntqapplication.h>
#include "options.h"

int main( int argc, char ** argv )
{
    TQApplication a( argc, argv );
    OptionsDialog dlg;
    return dlg.exec();
}
