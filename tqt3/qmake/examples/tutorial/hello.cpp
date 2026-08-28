#include "hello.h"

MyPushButton::MyPushButton( const TQString& text ) 
    : TQPushButton( text, 0, "mypushbutton" )
{
    tqDebug( "My PushButton has been constructed" );
}
