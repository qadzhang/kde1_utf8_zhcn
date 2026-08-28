#include <ntqapplication.h>
#include "filechooser.h"

int main( int argc, char ** argv )
{
    TQApplication a( argc, argv );
    FileChooser *fc = new FileChooser;
    fc->show();
    return a.exec();
}
