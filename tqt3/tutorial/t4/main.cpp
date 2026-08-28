/****************************************************************
**
** TQt tutorial 4
**
****************************************************************/

#include <ntqapplication.h>
#include <ntqpushbutton.h>
#include <ntqfont.h>


class MyWidget : public TQWidget
{
public:
    MyWidget( TQWidget *parent=0, const char *name=0 );
};


MyWidget::MyWidget( TQWidget *parent, const char *name )
        : TQWidget( parent, name )
{
    setMinimumSize( 200, 120 );
    setMaximumSize( 200, 120 );

    TQPushButton *quit = new TQPushButton( "Quit", this, "quit" );
    quit->setGeometry( 62, 40, 75, 30 );
    quit->setFont( TQFont( "Times", 18, TQFont::Bold ) );

    connect( quit, TQ_SIGNAL(clicked()), tqApp, TQ_SLOT(quit()) );
}


int main( int argc, char **argv )
{
    TQApplication a( argc, argv );

    MyWidget w;
    w.setGeometry( 100, 100, 200, 120 );
    a.setMainWidget( &w );
    w.show();
    return a.exec();
}
