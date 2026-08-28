#include <ntqapplication.h>
#include <ntqpushbutton.h>
#include <ntqlabel.h>
#include "myobject.h"
#include "mydialog.h"

int main(int argc, char **argv)
{
    TQApplication app(argc, argv);
    
    MyObject obj;
    MyDialog dia;
    app.setMainWidget( &dia );
    dia.connect( dia.aButton, TQ_SIGNAL(clicked()), TQ_SLOT(close()) );
    dia.show();

    return app.exec();
}
