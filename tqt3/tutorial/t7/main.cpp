/****************************************************************
**
** TQt tutorial 7
**
****************************************************************/

#include <ntqapplication.h>
#include <ntqpushbutton.h>
#include <ntqlcdnumber.h>
#include <ntqfont.h>
#include <ntqvbox.h>
#include <ntqgrid.h>

#include "lcdrange.h"


class MyWidget : public TQVBox
{
public:
    MyWidget( TQWidget *parent=0, const char *name=0 );
};


MyWidget::MyWidget( TQWidget *parent, const char *name )
        : TQVBox( parent, name )
{
    TQPushButton *quit = new TQPushButton( "Quit", this, "quit" );
    quit->setFont( TQFont( "Times", 18, TQFont::Bold ) );

    connect( quit, TQ_SIGNAL(clicked()), tqApp, TQ_SLOT(quit()) );

    TQGrid *grid = new TQGrid( 4, this );

    LCDRange *previous = 0;
    for( int r = 0 ; r < 4 ; r++ ) {
	for( int c = 0 ; c < 4 ; c++ ) {
	    LCDRange* lr = new LCDRange( grid );
	    if ( previous )
		connect( lr, TQ_SIGNAL(valueChanged(int)),
			 previous, TQ_SLOT(setValue(int)) );
	    previous = lr;
	}
    }
}


int main( int argc, char **argv )
{
    TQApplication a( argc, argv );

    MyWidget w;
    a.setMainWidget( &w );
    w.show();
    return a.exec();
}
