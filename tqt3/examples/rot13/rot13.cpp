/****************************************************************************
**
** Copyright (C) 1992-2008 Trolltech ASA.  All rights reserved.
**
** This file is part of an example program for TQt.  This example
** program may be used, distributed and modified without limitation.
**
*****************************************************************************/

#include "rot13.h"

#include <ntqmultilineedit.h>
#include <ntqpushbutton.h>
#include <ntqapplication.h>
#include <ntqlayout.h>

Rot13::Rot13()
{
    left = new TQMultiLineEdit( this, "left" );
    right = new TQMultiLineEdit( this, "right" );
    connect( left, TQ_SIGNAL(textChanged()), this, TQ_SLOT(changeRight()) );
    connect( right, TQ_SIGNAL(textChanged()), this, TQ_SLOT(changeLeft()) );

    TQPushButton * quit = new TQPushButton( "&Quit", this );
    quit->setFocusPolicy( NoFocus );
    connect( quit, TQ_SIGNAL(clicked()), tqApp, TQ_SLOT(quit()) );

    TQGridLayout * l = new TQGridLayout( this, 2, 2, 5 );
    l->addWidget( left, 0, 0 );
    l->addWidget( right, 0, 1 );
    l->addWidget( quit, 1, 1, AlignRight );

    left->setFocus();
}


void Rot13::changeLeft()
{
    left->blockSignals( true );
    left->setText( rot13( right->text() ) );
    left->blockSignals( false );
}


void Rot13::changeRight()
{
    right->blockSignals( true );
    right->setText( rot13( left->text() ) );
    right->blockSignals( false );
}


TQString Rot13::rot13( const TQString & input ) const
{
    TQString r = input;
    int i = r.length();
    while( i-- ) {
	if ( r[i] >= TQChar('A') && r[i] <= TQChar('M') ||
	     r[i] >= TQChar('a') && r[i] <= TQChar('m') )
	    r[i] = (char)((int)TQChar(r[i]) + 13);
	else if  ( r[i] >= TQChar('N') && r[i] <= TQChar('Z') ||
		   r[i] >= TQChar('n') && r[i] <= TQChar('z') )
	    r[i] = (char)((int)TQChar(r[i]) - 13);
    }
    return r;
}


int main( int argc, char ** argv )
{
    TQApplication a( argc, argv );
    Rot13 r;
    r.resize( 400, 400 );
    a.setMainWidget( &r );
    r.setCaption("TQt Example - ROT13");
    r.show();
    return a.exec();
}
