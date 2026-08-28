#include <ntqapplication.h>
#include "chartform.h"


int main( int argc, char *argv[] )
{
    TQApplication app( argc, argv );

    TQString filename;
    if ( app.argc() > 1 ) {
	filename = app.argv()[1];
	if ( !filename.endsWith( ".cht" ) )
	    filename = TQString::null;
    }

    ChartForm *cf = new ChartForm( filename );
    app.setMainWidget( cf );
    cf->show();

    return app.exec();
}
