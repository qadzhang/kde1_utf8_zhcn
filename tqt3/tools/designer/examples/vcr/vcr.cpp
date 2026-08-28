#include "vcr.h"
#include <ntqpushbutton.h>
#include <ntqlayout.h>

static const char * rewind_xpm[] = {
"16 16 3 1",
" 	c None",
".	c #FFFFFF",
"+	c #000000",
"................",
".++..........++.",
".++........++++.",
".++......++++++.",
".++....++++++++.",
".++..++++++++++.",
".++++++++++++++.",
".++++++++++++++.",
".++++++++++++++.",
".++++++++++++++.",
".++..++++++++++.",
".++....++++++++.",
".++......++++++.",
".++........++++.",
".++.........+++.",
"................"};

static const char * play_xpm[] = {
"16 16 3 1",
" 	c None",
".	c #FFFFFF",
"+	c #000000",
"................",
".++.............",
".++++...........",
".++++++.........",
".++++++++.......",
".++++++++++.....",
".++++++++++++...",
".+++++++++++++..",
".+++++++++++++..",
".++++++++++++...",
".++++++++++.....",
".++++++++.......",
".++++++.........",
".++++...........",
".+++............",
"................"};

static const char * next_xpm[] = {
"16 16 3 1",
" 	c None",
".	c #FFFFFF",
"+	c #000000",
"................",
".++.....+.......",
".+++....++......",
".++++...+++.....",
".+++++..++++....",
".++++++.+++++...",
".+++++++++++++..",
".++++++++++++++.",
".++++++++++++++.",
".+++++++++++++..",
".++++++.+++++...",
".+++++..++++....",
".++++...+++.....",
".+++....++......",
".++.....+.......",
"................"};

static const char * stop_xpm[] = {
"16 16 3 1",
" 	c None",
".	c #FFFFFF",
"+	c #000000",
"................",
".++++++++++++++.",
".++++++++++++++.",
".++++++++++++++.",
".+++........+++.",
".+++........+++.",
".+++........+++.",
".+++........+++.",
".+++........+++.",
".+++........+++.",
".+++........+++.",
".+++........+++.",
".++++++++++++++.",
".++++++++++++++.",
".++++++++++++++.",
"................"};


Vcr::Vcr( TQWidget *parent, const char *name )
    : TQWidget( parent, name )
{
    TQHBoxLayout *layout = new TQHBoxLayout( this );
    layout->setMargin( 0 );

    TQPushButton *rewind = new TQPushButton( TQPixmap( rewind_xpm ), 0, this, "vcr_rewind" );
    layout->addWidget( rewind );
    connect( rewind, TQ_SIGNAL(clicked()), TQ_SIGNAL(rewind()) ); 

    TQPushButton *play = new TQPushButton( TQPixmap( play_xpm ), 0, this, "vcr_play" );
    layout->addWidget( play );
    connect( play, TQ_SIGNAL(clicked()), TQ_SIGNAL(play()) ); 

    TQPushButton *next = new TQPushButton( TQPixmap( next_xpm ), 0, this, "vcr_next" );
    layout->addWidget( next );
    connect( next, TQ_SIGNAL(clicked()), TQ_SIGNAL(next()) ); 

    TQPushButton *stop = new TQPushButton( TQPixmap( stop_xpm ), 0, this, "vcr_stop" );
    layout->addWidget( stop );
    connect( stop, TQ_SIGNAL(clicked()), TQ_SIGNAL(stop()) ); 
}


