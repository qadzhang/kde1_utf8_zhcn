#include <ntqapplication.h>
#include "addressbook.h"

int main( int argc, char ** argv )
{
    TQApplication a( argc, argv );
    AddressBook *w = new AddressBook;
    w->show();
    a.connect( &a, TQ_SIGNAL( lastWindowClosed() ), &a, TQ_SLOT( quit() ) );
    return a.exec();
}
