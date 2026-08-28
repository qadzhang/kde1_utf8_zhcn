/****************************************************************************
**
** Custom MIME type implementation example
**
** Created : 979899
**
** Copyright (C) 1997-2008 Trolltech ASA.  All rights reserved.
**
** This file is part of an example program for TQt.  This example
** program may be used, distributed and modified without limitation.
**
*****************************************************************************/

#include "secret.h"
#include <ntqevent.h>


//create the object withe the secret byte
SecretDrag::SecretDrag( uchar secret, TQWidget * parent, const char * name )
    : TQStoredDrag( "secret/magic", parent, name )
{
    TQByteArray data(1);
    data[0]= secret;
    setEncodedData( data );
}


bool SecretDrag::canDecode( TQDragMoveEvent* e )
{
    return e->provides( "secret/magic" );
}

//decode it into a string
bool SecretDrag::decode( TQDropEvent* e, TQString& str )
{
    TQByteArray payload = e->data( "secret/magic" );
    if ( payload.size() ) {
	e->accept();
	TQString msg;
	msg.sprintf("The secret number is %d", payload[0] );
	str = msg;
	return true;
    }
    return false;
}


SecretSource::SecretSource( int secret, TQWidget *parent, const char * name )
    : TQLabel( "Secret", parent, name )
{
    setBackgroundColor( blue.light() );
    setFrameStyle( Box | Sunken );
    setMinimumHeight( sizeHint().height()*2 );
    setAlignment( AlignCenter );
    mySecret = secret;
}

SecretSource::~SecretSource()
{
}

/* XPM */
static const char * picture_xpm[] = {
"16 16 3 1",
" 	c None",
".	c #000000",
"X	c #FFFF00",
"     .....      ",
"   ..XXXXX..    ",
"  .XXXXXXXXX.   ",
" .XXXXXXXXXXX.  ",
" .XX..XXX..XX.  ",
".XXXXXXXXXXXXX. ",
".XX...XXX...XX. ",
".XXX..XXX..XXX. ",
".XXXXXXXXXXXXX. ",
".XXXXXX.XXXXXX. ",
" .XX.XX.XX.XX.  ",
" .XXX..X..XXX.  ",
"  .XXXXXXXXX.   ",
"   ..XXXXX..    ",
"     .....      ",
"                "};

void SecretSource::mousePressEvent( TQMouseEvent * /*e*/ )
{
    SecretDrag *sd = new SecretDrag( mySecret, this );
    sd->setPixmap(TQPixmap(picture_xpm),TQPoint(8,8));
    sd->dragCopy();
    mySecret++;
}
