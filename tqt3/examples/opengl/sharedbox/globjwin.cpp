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
#include "glbox.h"


GLObjectWindow::GLObjectWindow( TQWidget* parent, const char* name )
    : TQWidget( parent, name )
{
    // Create a menu
    TQPopupMenu *file = new TQPopupMenu( this );
    file->insertItem( "Delete Left TQGLWidget", this, 
		      TQ_SLOT(deleteFirstWidget()) );
    file->insertItem( "Exit",  tqApp, TQ_SLOT(quit()), CTRL+Key_Q );

    // Create a menu bar
    TQMenuBar *m = new TQMenuBar( this );
    m->setSeparator( TQMenuBar::InWindowsStyle );
    m->insertItem("&File", file );

    // Create nice frames to put around the OpenGL widgets
    TQFrame* f1 = new TQFrame( this, "frame1" );
    f1->setFrameStyle( TQFrame::Sunken | TQFrame::Panel );
    f1->setLineWidth( 2 );
    TQFrame* f2 = new TQFrame( this, "frame2" );
    f2->setFrameStyle( TQFrame::Sunken | TQFrame::Panel );
    f2->setLineWidth( 2 );

    // Create an OpenGL widget
    c1 = new GLBox( f1, "glbox1" );
    
    // Create another OpenGL widget that shares display lists with the first
    c2 = new GLBox( f2, "glbox2", c1 );

    // Create the three sliders; one for each rotation axis
    // Make them spin the boxes, but not in synch
    TQSlider* x = new TQSlider ( 0, 360, 60, 0, TQSlider::Vertical, this, "xsl" );
    x->setTickmarks( TQSlider::Left );
    connect( x, TQ_SIGNAL(valueChanged(int)), c1, TQ_SLOT(setXRotation(int)) );
    connect( x, TQ_SIGNAL(valueChanged(int)), c2, TQ_SLOT(setZRotation(int)) );

    TQSlider* y = new TQSlider ( 0, 360, 60, 0, TQSlider::Vertical, this, "ysl" );
    y->setTickmarks( TQSlider::Left );
    connect( y, TQ_SIGNAL(valueChanged(int)), c1, TQ_SLOT(setYRotation(int)) );
    connect( y, TQ_SIGNAL(valueChanged(int)), c2, TQ_SLOT(setXRotation(int)) );

    TQSlider* z = new TQSlider ( 0, 360, 60, 0, TQSlider::Vertical, this, "zsl" );
    z->setTickmarks( TQSlider::Left );
    connect( z, TQ_SIGNAL(valueChanged(int)), c1, TQ_SLOT(setZRotation(int)) );
    connect( z, TQ_SIGNAL(valueChanged(int)), c2, TQ_SLOT(setYRotation(int)) );


    // Now that we have all the widgets, put them into a nice layout

    // Put the sliders on top of each other
    TQVBoxLayout* vlayout = new TQVBoxLayout( 20, "vlayout");
    vlayout->addWidget( x );
    vlayout->addWidget( y );
    vlayout->addWidget( z );

    // Put the GL widgets inside the frames
    TQHBoxLayout* flayout1 = new TQHBoxLayout( f1, 2, 2, "flayout1");
    flayout1->addWidget( c1, 1 );
    TQHBoxLayout* flayout2 = new TQHBoxLayout( f2, 2, 2, "flayout2");
    flayout2->addWidget( c2, 1 );

    // Top level layout, puts the sliders to the left of the frame/GL widget
    TQHBoxLayout* hlayout = new TQHBoxLayout( this, 20, 20, "hlayout");
    hlayout->setMenuBar( m );
    hlayout->addLayout( vlayout );
    hlayout->addWidget( f1, 1 );
    hlayout->addWidget( f2, 1 );

}


void GLObjectWindow::deleteFirstWidget()
{
    // Delete only c1; c2 will keep working and use the shared display list
    if ( c1 ) {
	delete c1;
	c1 = 0;
    }
}
