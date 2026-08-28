#include <ntqapplication.h>
#include "metric.h"

int main( int argc, char ** argv )
{
    TQApplication a( argc, argv );
    ConversionForm *w = new ConversionForm;
    w->show();
    a.setMainWidget( w );
    a.connect( &a, TQ_SIGNAL( lastWindowClosed() ), &a, TQ_SLOT( quit() ) );
    return a.exec();
}
