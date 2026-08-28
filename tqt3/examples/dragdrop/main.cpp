/****************************************************************************
**
** Ritual main() for TQt applications
**
** Copyright (C) 1996-2008 Trolltech ASA.  All rights reserved.
**
** This file is part of an example program for TQt.  This example
** program may be used, distributed and modified without limitation.
**
*****************************************************************************/

#include <ntqapplication.h>
#include "dropsite.h"
#include "secret.h"
#include <ntqlayout.h>
#include <ntqcombobox.h>
#include <ntqlabel.h>
#include <ntqpixmap.h>

static void addStuff( TQWidget * parent, bool image, bool secret = false )
{
    TQVBoxLayout * tll = new TQVBoxLayout( parent, 10 );
    DropSite * d = new DropSite( parent );
    d->setFrameStyle( TQFrame::Sunken + TQFrame::WinPanel );
    tll->addWidget( d );
    if ( image ) {
	TQPixmap stuff;
	if ( !stuff.load( "trolltech.bmp" ) ) {
	    stuff = TQPixmap(20,20);
	    stuff.fill(TQt::green);
	}
	d->setPixmap( stuff );
    } else {
	d->setText("Drag and Drop");
    }
    d->setFont(TQFont("Helvetica",18));
    if ( secret ) {
	SecretSource *s = new SecretSource( 42, parent );
	tll->addWidget( s );
    }

    TQLabel * format = new TQLabel( "\n\n\n\nNone\n\n\n\n", parent );
    tll->addWidget( format );
    tll->activate();
    parent->resize( parent->sizeHint() );

    TQObject::connect( d, TQ_SIGNAL(message(const TQString&)),
		      format, TQ_SLOT(setText(const TQString&)) );
}


int main( int argc, char ** argv )
{
    TQApplication a( argc, argv );

    TQWidget mw;
    addStuff( &mw, true );
    mw.setCaption( "TQt Example - Drag and Drop" );
    mw.show();

    TQWidget mw2;
    addStuff( &mw2, false );
    mw2.setCaption( "TQt Example - Drag and Drop" );
    mw2.show();

    TQWidget mw3;
    addStuff( &mw3, true, true );
    mw3.setCaption( "TQt Example - Drag and Drop" );
    mw3.show();

    TQObject::connect(tqApp,TQ_SIGNAL(lastWindowClosed()),tqApp,TQ_SLOT(quit()));
    return a.exec();
}
