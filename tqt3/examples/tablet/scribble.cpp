/****************************************************************************
**
** Copyright (C) 1992-2008 Trolltech ASA.  All rights reserved.
**
** This file is part of an example program for TQt.  This example
** program may be used, distributed and modified without limitation.
**
*****************************************************************************/

#include "canvas.h"
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

/*

Scribble::Scribble( TQWidget *parent, const char *name )
    : TQMainWindow( parent, name )
{
    canvas = new Canvas( this );
    setCentralWidget( canvas );

    TQToolBar *tools = new TQToolBar( this );

    bSave = new TQPushButton( "Save as...", tools );

    tools->addSeparator();

    bPColor = new TQPushButton( "Choose Pen Color...", tools );
    //    bPColor->setText( "Choose Pen Color..." );

    tools->addSeparator();

    bPWidth = new TQSpinBox( 1, 20, 1, tools );
    TQToolTip::add( bPWidth, "Choose Pen Width" );
    connect( bPWidth, TQ_SIGNAL( valueChanged( int ) ), this, TQ_SLOT( slotWidth( int ) ) );
    bPWidth->setValue( 3 );

    tools->addSeparator();

    bClear = new TQPushButton( "Clear Screen", tools );
    TQObject::connect( bSave, TQ_SIGNAL( clicked() ), this, TQ_SLOT( slotSave() ) );
    TQObject::connect( bPColor, TQ_SIGNAL( clicked() ), this, TQ_SLOT( slotColor() ) );
    TQObject::connect( bClear, TQ_SIGNAL( clicked() ), this, TQ_SLOT( slotClear() ) );
		
}

  */
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
