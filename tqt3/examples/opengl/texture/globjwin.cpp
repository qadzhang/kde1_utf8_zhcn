/****************************************************************************
**
** Copyright (C) 1992-2008 Trolltech ASA.  All rights reserved.
**
** This file is part of an example program for TQt.  This example
** program may be used, distributed and modified without limitation.
**
*****************************************************************************/

#include <ntqpushbutton.h>
#include <ntqslider.h>
#include <ntqlayout.h>
#include <ntqframe.h>
#include <ntqmenubar.h>
#include <ntqpopupmenu.h>
#include <ntqapplication.h>
#include <ntqkeycode.h>
#include "globjwin.h"
#include "gltexobj.h"


GLObjectWindow::GLObjectWindow( TQWidget* parent, const char* name )
    : TQWidget( parent, name )
{

    // Create nice frames to put around the OpenGL widgets
    TQFrame* f1 = new TQFrame( this, "frame1" );
    f1->setFrameStyle( TQFrame::Sunken | TQFrame::Panel );
    f1->setLineWidth( 2 );

    // Create an OpenGL widget
    GLTexobj* c = new GLTexobj( f1, "glbox1");

    // Create a menu
    TQPopupMenu *file = new TQPopupMenu( this );
    file->insertItem( "Toggle Animation", c, TQ_SLOT(toggleAnimation()),
		      CTRL+Key_A );
    file->insertSeparator();
    file->insertItem( "Exit",  tqApp, TQ_SLOT(quit()), CTRL+Key_Q );
    
    // Create a menu bar
    TQMenuBar *m = new TQMenuBar( this );
    m->setSeparator( TQMenuBar::InWindowsStyle );
    m->insertItem("&File", file );

    // Create the three sliders; one for each rotation axis
    TQSlider* x = new TQSlider ( 0, 360, 60, 0, TQSlider::Vertical, this, "xsl" );
    x->setTickmarks( TQSlider::Left );
    connect( x, TQ_SIGNAL(valueChanged(int)), c, TQ_SLOT(setXRotation(int)) );

    TQSlider* y = new TQSlider ( 0, 360, 60, 0, TQSlider::Vertical, this, "ysl" );
    y->setTickmarks( TQSlider::Left );
    connect( y, TQ_SIGNAL(valueChanged(int)), c, TQ_SLOT(setYRotation(int)) );

    TQSlider* z = new TQSlider ( 0, 360, 60, 0, TQSlider::Vertical, this, "zsl" );
    z->setTickmarks( TQSlider::Left );
    connect( z, TQ_SIGNAL(valueChanged(int)), c, TQ_SLOT(setZRotation(int)) );


    // Now that we have all the widgets, put them into a nice layout

    // Put the sliders on top of each other
    TQVBoxLayout* vlayout = new TQVBoxLayout( 20, "vlayout");
    vlayout->addWidget( x );
    vlayout->addWidget( y );
    vlayout->addWidget( z );

    // Put the GL widget inside the frame
    TQHBoxLayout* flayout1 = new TQHBoxLayout( f1, 2, 2, "flayout1");
    flayout1->addWidget( c, 1 );

    // Top level layout, puts the sliders to the left of the frame/GL widget
    TQHBoxLayout* hlayout = new TQHBoxLayout( this, 20, 20, "hlayout");
    hlayout->setMenuBar( m );
    hlayout->addLayout( vlayout );
    hlayout->addWidget( f1, 1 );

}
