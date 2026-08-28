/****************************************************************************
**
** Copyright (C) 1992-2008 Trolltech ASA.  All rights reserved.
**
** This file is part of an example program for TQt.  This example
** program may be used, distributed and modified without limitation.
**
*****************************************************************************/

#include "rangecontrols.h"

#include <ntqhbox.h>
#include <ntqlcdnumber.h>
#include <ntqspinbox.h>
#include <ntqlabel.h>
#include <ntqstring.h>
#include <ntqslider.h>
#include <ntqcheckbox.h>

#include <limits.h>

RangeControls::RangeControls( TQWidget *parent, const char *name )
    : TQVBox( parent, name )
{
    TQHBox *row1 = new TQHBox( this );

    TQVBox *cell2 = new TQVBox( row1 );
    cell2->setMargin( 10 );
    cell2->setFrameStyle( TQFrame::WinPanel | TQFrame::Sunken );

    (void)new TQWidget( cell2 );

    TQLabel *label1 = new TQLabel( TQString( "Enter a value between\n%1 and %2:" ).arg( -INT_MAX ).arg( INT_MAX ), cell2 );
    label1->setMaximumHeight( label1->sizeHint().height() );
    TQSpinBox *sb1 = new TQSpinBox( -INT_MAX, INT_MAX, 1, cell2 );
    sb1->setValue( 0 );

    TQLabel *label2 = new TQLabel( "Enter a zoom value:", cell2 );
    label2->setMaximumHeight( label2->sizeHint().height() );
    TQSpinBox *sb2 = new TQSpinBox( 0, 1000, 10, cell2 );
    sb2->setSuffix( " %" );
    sb2->setSpecialValueText( "Automatic" );

    TQLabel *label3 = new TQLabel( "Enter a price:", cell2 );
    label3->setMaximumHeight( label3->sizeHint().height() );
    TQSpinBox *sb3 = new TQSpinBox( 0, INT_MAX, 1, cell2 );
    sb3->setPrefix( "$" );
    sb3->setValue( 355 );

    (void)new TQWidget( cell2 );

    TQHBox *row2 = new TQHBox( this );

    TQVBox *cell3 = new TQVBox( row2 );
    cell3->setMargin( 10 );
    cell3->setFrameStyle( TQFrame::WinPanel | TQFrame::Sunken );
    TQSlider *hslider = new TQSlider( 0, 64, 1, 33, TQt::Horizontal, cell3, "horizontal_s" );
    TQLCDNumber *lcd2 = new TQLCDNumber( 2, cell3 );
    lcd2->display( 33 );
    lcd2->setSegmentStyle( TQLCDNumber::Filled );
    connect( hslider, TQ_SIGNAL( valueChanged( int ) ), lcd2, TQ_SLOT( display( int ) ) );

    TQHBox *cell4 = new TQHBox( row2 );
    cell4->setFrameStyle( TQFrame::WinPanel | TQFrame::Sunken );
    cell4->setMargin( 10 );
    TQSlider *vslider = new TQSlider( 0, 64, 1, 8, TQt::Vertical, cell4 );
    TQLCDNumber *lcd3 = new TQLCDNumber( 3, cell4 );
    lcd3->display( 8 );
    connect( vslider, TQ_SIGNAL( valueChanged( int ) ), lcd3, TQ_SLOT( display( int ) ) );
}
