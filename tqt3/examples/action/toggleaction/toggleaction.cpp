#include <ntqapplication.h>
#include <ntqmainwindow.h>
#include <ntqtoolbar.h>
#include <ntqaction.h>

#include "labelonoff.xpm"

int main( int argc, char **argv )
{
    TQApplication app( argc, argv );
    TQMainWindow * window = new TQMainWindow;
    window->setCaption("TQt Example - Toggleaction");
    TQToolBar * toolbar = new TQToolBar( window );

    TQAction * labelonoffaction = new TQAction( window, "labelonoff" );
    labelonoffaction->setToggleAction( true );

    labelonoffaction->setText( "labels on/off" );
    labelonoffaction->setAccel( TQt::ALT+TQt::Key_L );
    labelonoffaction->setIconSet( (TQPixmap) labelonoff_xpm );

    TQObject::connect( labelonoffaction, TQ_SIGNAL( toggled( bool ) ),
                      window, TQ_SLOT( setUsesTextLabel( bool ) ) );

    labelonoffaction->addTo( toolbar );

    app.setMainWidget( window );
    window->show();
    return app.exec();
}

