/****************************************************************
**
** TQt tutorial 10
**
****************************************************************/

#include <ntqapplication.h>
#include <ntqpushbutton.h>
#include <ntqlcdnumber.h>
#include <ntqfont.h>
#include <ntqlayout.h>

#include "lcdrange.h"
#include "cannon.h"


class MyWidget: public TQWidget
{
public:
    MyWidget( TQWidget *parent=0, const char *name=0 );
};


MyWidget::MyWidget( TQWidget *parent, const char *name )
        : TQWidget( parent, name )
{
    TQPushButton *quit = new TQPushButton( "&Quit", this, "quit" );
    quit->setFont( TQFont( "Times", 18, TQFont::Bold ) );

    connect( quit, TQ_SIGNAL(clicked()), tqApp, TQ_SLOT(quit()) );

    LCDRange *angle = new LCDRange( this, "angle" );
    angle->setRange( 5, 70 );

    LCDRange *force  = new LCDRange( this, "force" );
    force->setRange( 10, 50 );

    CannonField *cannonField = new CannonField( this, "cannonField" );

    connect( angle, TQ_SIGNAL(valueChanged(int)),
	     cannonField, TQ_SLOT(setAngle(int)) );
    connect( cannonField, TQ_SIGNAL(angleChanged(int)),
	     angle, TQ_SLOT(setValue(int)) );

    connect( force, TQ_SIGNAL(valueChanged(int)),
	     cannonField, TQ_SLOT(setForce(int)) );
    connect( cannonField, TQ_SIGNAL(forceChanged(int)),
	     force, TQ_SLOT(setValue(int)) );

    TQGridLayout *grid = new TQGridLayout( this, 2, 2, 10 );
    grid->addWidget( quit, 0, 0 );
    grid->addWidget( cannonField, 1, 1 );
    grid->setColStretch( 1, 10 );

    TQVBoxLayout *leftBox = new TQVBoxLayout;
    grid->addLayout( leftBox, 1, 0 );
    leftBox->addWidget( angle );
    leftBox->addWidget( force );

    angle->setValue( 60 );
    force->setValue( 25 );
    angle->setFocus();
}

int main( int argc, char **argv )
{
    TQApplication::setColorSpec( TQApplication::CustomColor );
    TQApplication a( argc, argv );

    MyWidget w;
    w.setGeometry( 100, 100, 500, 355 );
    a.setMainWidget( &w );
    w.show();
    return a.exec();
}
