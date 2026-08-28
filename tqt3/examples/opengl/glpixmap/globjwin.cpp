/****************************************************************************
**
** Copyright (C) 1992-2008 Trolltech ASA.  All rights reserved.
**
** This file is part of an example program for TQt.  This example
** program may be used, distributed and modified without limitation.
**
*****************************************************************************/

/****************************************************************************
**
** Implementation of GLObjectWindow widget class
**
****************************************************************************/


#include <ntqpushbutton.h>
#include <ntqslider.h>
#include <ntqlayout.h>
#include <ntqframe.h>
#include <ntqlabel.h>
#include <ntqmenubar.h>
#include <ntqpopupmenu.h>
#include <ntqapplication.h>
#include <ntqkeycode.h>
#include <ntqpixmap.h>
#include <ntqimage.h>
#include <ntqpainter.h>
#include "globjwin.h"
#include "glbox.h"


GLObjectWindow::GLObjectWindow( TQWidget* parent, const char* name )
    : TQWidget( parent, name )
{
    // Create a menu
    file = new TQPopupMenu( this );
    file->setCheckable( true );
    file->insertItem( "Grab Frame Buffer", this, TQ_SLOT(grabFrameBuffer()) );
    file->insertItem( "Render Pixmap", this, TQ_SLOT(makePixmap()) );
    file->insertItem( "Render Pixmap Hidden", this, TQ_SLOT(makePixmapHidden()) );
    file->insertSeparator();
    fixMenuItemId = file->insertItem( "Use Fixed Pixmap Size", this, 
				      TQ_SLOT(useFixedPixmapSize()) );
    file->insertSeparator();
    insertMenuItemId = file->insertItem( "Insert Pixmap Here", this, 
					 TQ_SLOT(makePixmapForMenu()) );
    file->insertSeparator();
    file->insertItem( "Exit",  tqApp, TQ_SLOT(quit()), CTRL+Key_Q );

    // Create a menu bar
    TQMenuBar *m = new TQMenuBar( this );
    m->setSeparator( TQMenuBar::InWindowsStyle );
    m->insertItem("&File", file );

    // Create nice frames to put around the OpenGL widgets
    TQFrame* f1 = new TQFrame( this, "frame1" );
    f1->setFrameStyle( TQFrame::Sunken | TQFrame::Panel );
    f1->setLineWidth( 2 );

    // Create an OpenGL widget
    c1 = new GLBox( f1, "glbox1");

    // Create a label that can display the pixmap
    lb = new TQLabel( this, "pixlabel" );
    lb->setFrameStyle( TQFrame::Sunken | TQFrame::Panel );
    lb->setLineWidth( 2 );
    lb->setAlignment( AlignCenter );
    lb->setMargin( 0 );
    lb->setIndent( 0 );

    // Create the three sliders; one for each rotation axis
    TQSlider* x = new TQSlider ( 0, 360, 60, 0, TQSlider::Vertical, this, "xsl" );
    x->setTickmarks( TQSlider::Left );
    connect( x, TQ_SIGNAL(valueChanged(int)), c1, TQ_SLOT(setXRotation(int)) );

    TQSlider* y = new TQSlider ( 0, 360, 60, 0, TQSlider::Vertical, this, "ysl" );
    y->setTickmarks( TQSlider::Left );
    connect( y, TQ_SIGNAL(valueChanged(int)), c1, TQ_SLOT(setYRotation(int)) );

    TQSlider* z = new TQSlider ( 0, 360, 60, 0, TQSlider::Vertical, this, "zsl" );
    z->setTickmarks( TQSlider::Left );
    connect( z, TQ_SIGNAL(valueChanged(int)), c1, TQ_SLOT(setZRotation(int)) );


    // Now that we have all the widgets, put them into a nice layout

    // Put the sliders on top of each other
    TQVBoxLayout* vlayout = new TQVBoxLayout( 20, "vlayout");
    vlayout->addWidget( x );
    vlayout->addWidget( y );
    vlayout->addWidget( z );

    // Put the GL widget inside the frame
    TQHBoxLayout* flayout1 = new TQHBoxLayout( f1, 2, 2, "flayout1");
    flayout1->addWidget( c1, 1 );

    // Top level layout, puts the sliders to the left of the frame/GL widget
    TQHBoxLayout* hlayout = new TQHBoxLayout( this, 20, 20, "hlayout");
    hlayout->setMenuBar( m );
    hlayout->addLayout( vlayout );
    hlayout->addWidget( f1, 1 );
    hlayout->addWidget( lb, 1 );

}



void GLObjectWindow::grabFrameBuffer()
{
    TQImage img = c1->grabFrameBuffer();

    // Convert image to pixmap so we can show it
    TQPixmap pm;
    pm.convertFromImage( img, AvoidDither );
    drawOnPixmap( &pm );
    lb->setPixmap( pm );
}



void GLObjectWindow::makePixmap()
{
    // Make a pixmap to to be rendered by the gl widget
    TQPixmap pm;

    // Render the pixmap, with either c1's size or the fixed size pmSz
    if ( pmSz.isValid() )
	pm = c1->renderPixmap( pmSz.width(), pmSz.height() );
    else 
	pm = c1->renderPixmap();

    if ( !pm.isNull() ) {
	// Present the pixmap to the user
	drawOnPixmap( &pm );
	lb->setPixmap( pm );
    }
    else {
	lb->setText( "Failed to render Pixmap." );
    }
}


void GLObjectWindow::makePixmapHidden()
{
    // Make a TQGLWidget to draw the pixmap. This widget will not be shown.
    GLBox* w = new GLBox( this, "temporary glwidget", c1 );

    bool success = false;
    TQPixmap pm;

    if ( w->isValid() ) {
	// Set the current rotation
	w->copyRotation( *c1 );

	// Determine wanted pixmap size
	TQSize sz = pmSz.isValid() ? pmSz : c1->size();

	// Make our hidden glwidget render the pixmap
	pm = w->renderPixmap( sz.width(), sz.height() );

	if ( !pm.isNull() )
	    success = true;
    }

    if ( success ) {
	// Present the pixmap to the user
	drawOnPixmap( &pm );
	lb->setPixmap( pm );
    }
    else {
	lb->setText( "Failed to render Pixmap." );
    }
    delete w;
}


void GLObjectWindow::drawOnPixmap( TQPixmap* pm )
{
    // Draw some text on the pixmap to differentiate it from the GL window

    if ( pm->isNull() ) {
	tqWarning("Cannot draw on null pixmap");
	return;
    }
    else {
	TQPainter p( pm );
       p.setFont( TQFont( "Helvetica", 18 ) );
	p.setPen( white );
	p.drawText( pm->rect(), AlignCenter, "This is a Pixmap" );
    }
}



void GLObjectWindow::useFixedPixmapSize()
{
    if ( pmSz.isValid() ) {
	pmSz = TQSize();
	file->setItemChecked( fixMenuItemId, false );
    }
    else {
	pmSz = TQSize( 200, 200 );
	file->setItemChecked( fixMenuItemId, true );
    }
}


void GLObjectWindow::makePixmapForMenu()
{
    TQPixmap pm = c1->renderPixmap( 32, 32 );
    if ( !pm.isNull() )
	file->changeItem( pm, "Insert Pixmap Here", insertMenuItemId );
}
