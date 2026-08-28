/****************************************************************
**
** Translation tutorial 3
**
****************************************************************/

#include "mainwindow.h"

#include <ntqapplication.h>
#include <ntqstring.h>
#include <ntqtextcodec.h>
#include <ntqtranslator.h>

int main( int argc, char **argv )
{
    TQApplication app( argc, argv );

    TQTranslator translator( 0 );
    translator.load( TQString("tt3_") + TQTextCodec::locale(), "." );
    app.installTranslator( &translator );

    MainWindow *mw = new MainWindow;
    app.setMainWidget( mw );
    mw->show();
    return app.exec();
}
