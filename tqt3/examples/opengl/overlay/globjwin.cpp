/****************************************************************************
**
** Copyright (C) 1992-2008 Trolltech ASA.  All rights reserved.
**
** This file is part of an example program for TQt.  This example
** program may be used, distributed and modified without limitation.
**
*****************************************************************************/

#include <ntqlayout.h>
#include <ntqframe.h>
#include <ntqmenubar.h>
#include <ntqpopupmenu.h>
#include <ntqapplication.h>
#include <ntqmessagebox.h>
#include "globjwin.h"
#include "glteapots.h"


GLObjectWindow::GLObjectWindow( TQWidget* parent, const char* name )
    : TQWidget( parent, name )
{
    // Create a menu
    TQPopupMenu *file = new TQPopupMenu( this );
    file->insertItem( "Exit",  tqApp, TQ_SLOT(quit()), CTRL+Key_Q );

    // Create a menu bar
    TQMenuBar *m = new TQMenuBar( this );
    m->setSeparator( TQMenuBar::InWindowsStyle );
    m->insertItem("&File", file );

    // Create a nice frame to put around the OpenGL widget
    TQFrame* f = new TQFrame( this, "frame" );
    f->setFrameStyle( TQFrame::Sunken | TQFrame::Panel );
    f->setLineWidth( 2 );

    // Create our OpenGL widget.
    GLTeapots* c = new GLTeapots( f, "glteapots" );

    // Check if we obtained an overlay
    if ( !c->format().hasOverlay() ) {
	TQMessageBox::warning( 0, tqApp->argv()[0], 
			      "Failed to get an OpenGL overlay",
			      "OK" );
    }

    // Now that we have all the widgets, put them into a nice layout

    // Put the GL widget inside the frame
    TQHBoxLayout* flayout = new TQHBoxLayout( f, 2, 2, "flayout");
    flayout->addWidget( c, 1 );

    // Top level layout
    TQVBoxLayout* hlayout = new TQVBoxLayout( this, 20, 20, "hlayout");
    hlayout->setMenuBar( m );
    hlayout->addWidget( f, 1 );
}
