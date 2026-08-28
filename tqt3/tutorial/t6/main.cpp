/****************************************************************
**
** TQt tutorial 6
**
****************************************************************/

#include <ntqapplication.h>
#include <ntqpushbutton.h>
#include <ntqslider.h>
#include <ntqlcdnumber.h>
#include <ntqfont.h>
#include <ntqvbox.h>
#include <ntqgrid.h>

class LCDRange : public TQVBox
{
public:
    LCDRange( TQWidget *parent=0, const char *name=0 );
};

LCDRange::LCDRange( TQWidget *parent, const char *name )
        : TQVBox( parent, name )
{
    TQLCDNumber *lcd  = new TQLCDNumber( 2, this, "lcd"  );
    TQSlider * slider = new TQSlider( Horizontal, this, "slider" );
    slider->setRange( 0, 99 );
    slider->setValue( 0 );
    connect( slider, TQ_SIGNAL(valueChanged(int)), lcd, TQ_SLOT(display(int)) );
}

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

    for( int r = 0 ; r < 4 ; r++ )
	for( int c = 0 ; c < 4 ; c++ )
	    (void)new LCDRange( grid );
}

int main( int argc, char **argv )
{
    TQApplication a( argc, argv );

    MyWidget w;
    a.setMainWidget( &w );
    w.show();
    return a.exec();
}
