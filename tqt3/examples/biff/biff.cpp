/****************************************************************************
**
** Copyright (C) 1992-2008 Trolltech ASA.  All rights reserved.
**
** This file is part of an example program for TQt.  This example
** program may be used, distributed and modified without limitation.
**
*****************************************************************************/

#include "biff.h"
#include <ntqstring.h>
#include <ntqfileinfo.h>
#include <ntqpainter.h>

#include <unistd.h>
#include <stdlib.h>

#include "bmp.cpp"


Biff::Biff( TQWidget *parent, const char *name )
    : TQWidget( parent, name, WShowModal | WType_Dialog )
{
    TQFileInfo fi = TQString(getenv( "MAIL" ));
    if ( !fi.exists() ) {
	TQString s( "/var/spool/mail/" );
	s += getlogin();
	fi.setFile( s );
    }

    if ( fi.exists() ) {
	mailbox = fi.absFilePath();
	startTimer( 1000 );
    }

    setMinimumSize( 48, 48 );
    setMaximumSize( 48, 48 );
    resize( 48, 48 );

    hasNewMail.loadFromData( hasmail_bmp_data, hasmail_bmp_len );
    noNewMail.loadFromData( nomail_bmp_data, nomail_bmp_len );

    gotMail = false;
    lastModified = fi.lastModified();
}


void Biff::timerEvent( TQTimerEvent * )
{
    TQFileInfo fi( mailbox );
    bool newState = ( fi.lastModified() != lastModified &&
		      fi.lastModified() > fi.lastRead() );
    if ( newState != gotMail ) {
	if ( gotMail )
	    lastModified = fi.lastModified();
	gotMail = newState;
	repaint( false );
    }
}
    

void Biff::paintEvent( TQPaintEvent * )
{
    if ( gotMail )
	bitBlt( this, 0, 0, &hasNewMail );
    else
	bitBlt( this, 0, 0, &noNewMail );
}


void Biff::mousePressEvent( TQMouseEvent * )
{
    TQFileInfo fi( mailbox );
    lastModified = fi.lastModified();
}
