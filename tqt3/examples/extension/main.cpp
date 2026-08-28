#include <ntqapplication.h>
#include "mainform.h"

int main( int argc, char ** argv )
{
    TQApplication a( argc, argv );
    MainForm *w = new MainForm;
    w->show();
    a.connect( &a, TQ_SIGNAL( lastWindowClosed() ), w, TQ_SLOT( quit() ) );
    return a.exec();
}
