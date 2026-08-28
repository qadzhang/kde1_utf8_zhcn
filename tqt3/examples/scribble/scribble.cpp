/****************************************************************************
**
** Copyright (C) 1992-2008 Trolltech ASA.  All rights reserved.
**
** This file is part of an example program for TQt.  This example
** program may be used, distributed and modified without limitation.
**
*****************************************************************************/

#include "scribble.h"

#include <ntqapplication.h>
#include <ntqevent.h>
#include <ntqpainter.h>
#include <ntqtoolbar.h>
#include <ntqtoolbutton.h>
#include <ntqspinbox.h>
#include <ntqtooltip.h>
#include <ntqrect.h>
#include <ntqpoint.h>
#include <ntqcolordialog.h>
#include <ntqfiledialog.h>
#include <ntqcursor.h>
#include <ntqimage.h>
#include <ntqstrlist.h>
#include <ntqpopupmenu.h>
#include <ntqintdict.h>

const bool no_writing = false;

Canvas::Canvas( TQWidget *parent, const char *name )
    : TQWidget( parent, name, WStaticContents ), pen( TQt::red, 3 ), polyline(3),
      mousePressed( false ), buffer( width(), height() )
{

    if ((tqApp->argc() > 0) && !buffer.load(tqApp->argv()[1]))
	buffer.fill( colorGroup().base() );
    setBackgroundMode( TQWidget::PaletteBase );
#ifndef TQT_NO_CURSOR
    setCursor( TQt::crossCursor );
#endif
}

void Canvas::save( const TQString &filename, const TQString &format )
{
    if ( !no_writing )
	buffer.save( filename, format.upper() );
}

void Canvas::clearScreen()
{
    buffer.fill( colorGroup().base() );
    repaint( false );
}

void Canvas::mousePressEvent( TQMouseEvent *e )
{
    mousePressed = true;
    polyline[2] = polyline[1] = polyline[0] = e->pos();
}

void Canvas::mouseReleaseEvent( TQMouseEvent * )
{
    mousePressed = false;
}

void Canvas::mouseMoveEvent( TQMouseEvent *e )
{
    if ( mousePressed ) {
	TQPainter painter;
	painter.begin( &buffer );
	painter.setPen( pen );
	polyline[2] = polyline[1];
	polyline[1] = polyline[0];
	polyline[0] = e->pos();
	painter.drawPolyline( polyline );
	painter.end();

	TQRect r = polyline.boundingRect();
	r = r.normalize();
	r.setLeft( r.left() - penWidth() );
	r.setTop( r.top() - penWidth() );
	r.setRight( r.right() + penWidth() );
	r.setBottom( r.bottom() + penWidth() );

	bitBlt( this, r.x(), r.y(), &buffer, r.x(), r.y(), r.width(), r.height() );
    }
}

void Canvas::resizeEvent( TQResizeEvent *e )
{
    TQWidget::resizeEvent( e );

    int w = width() > buffer.width() ?
	    width() : buffer.width();
    int h = height() > buffer.height() ?
	    height() : buffer.height();

    TQPixmap tmp( buffer );
    buffer.resize( w, h );
    buffer.fill( colorGroup().base() );
    bitBlt( &buffer, 0, 0, &tmp, 0, 0, tmp.width(), tmp.height() );
}

void Canvas::paintEvent( TQPaintEvent *e )
{
    TQWidget::paintEvent( e );

    TQMemArray<TQRect> rects = e->region().rects();
    for ( uint i = 0; i < rects.count(); i++ ) {
	TQRect r = rects[(int)i];
	bitBlt( this, r.x(), r.y(), &buffer, r.x(), r.y(), r.width(), r.height() );
    }
}

//------------------------------------------------------

Scribble::Scribble( TQWidget *parent, const char *name )
    : TQMainWindow( parent, name )
{
    canvas = new Canvas( this );
    setCentralWidget( canvas );

    TQToolBar *tools = new TQToolBar( this );

    bSave = new TQToolButton( TQPixmap(), "Save", "Save as PNG image", this, TQ_SLOT( slotSave() ), tools );
    bSave->setText( "Save as..." );

    tools->addSeparator();

    bPColor = new TQToolButton( TQPixmap(), "Choose Pen Color", "Choose Pen Color", this, TQ_SLOT( slotColor() ), tools );
    bPColor->setText( "Choose Pen Color..." );

    tools->addSeparator();

    bPWidth = new TQSpinBox( 1, 20, 1, tools );
    TQToolTip::add( bPWidth, "Choose Pen Width" );
    connect( bPWidth, TQ_SIGNAL( valueChanged( int ) ), this, TQ_SLOT( slotWidth( int ) ) );
    bPWidth->setValue( 3 );

    tools->addSeparator();

    bClear = new TQToolButton( TQPixmap(), "Clear Screen", "Clear Screen", this, TQ_SLOT( slotClear() ), tools );
    bClear->setText( "Clear Screen" );
}

void Scribble::slotSave()
{
    TQPopupMenu *menu = new TQPopupMenu( 0 );
    TQIntDict<TQString> formats;
    formats.setAutoDelete( true );

    for ( unsigned int i = 0; i < TQImageIO::outputFormats().count(); i++ ) {
	TQString str = TQString( TQImageIO::outputFormats().at( i ) );
	formats.insert( menu->insertItem( TQString( "%1..." ).arg( str ) ), new TQString( str ) );
    }

    menu->setMouseTracking( true );
    int id = menu->exec( bSave->mapToGlobal( TQPoint( 0, bSave->height() + 1 ) ) );

    if ( id != -1 ) {
	TQString format = *formats[ id ];

	TQString filename = TQFileDialog::getSaveFileName( TQString::null, TQString( "*.%1" ).arg( format.lower() ), this );
	if ( !filename.isEmpty() )
	    canvas->save( filename, format );
    }

    delete menu;
}

void Scribble::slotColor()
{
    TQColor c = TQColorDialog::getColor( canvas->penColor(), this );
    if ( c.isValid() )
	canvas->setPenColor( c );
}

void Scribble::slotWidth( int w )
{
    canvas->setPenWidth( w );
}

void Scribble::slotClear()
{
    canvas->clearScreen();
}
