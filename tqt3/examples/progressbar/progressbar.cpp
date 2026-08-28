/****************************************************************************
**
** Copyright (C) 1992-2008 Trolltech ASA.  All rights reserved.
**
** This file is part of an example program for TQt.  This example
** program may be used, distributed and modified without limitation.
**
*****************************************************************************/

#include "progressbar.h"

#include <ntqradiobutton.h>
#include <ntqpushbutton.h>
#include <ntqprogressbar.h>
#include <ntqlayout.h>

#include <ntqmotifstyle.h>

/*
 * Constructor
 *
 * Creates child widgets of the ProgressBar widget
 */

ProgressBar::ProgressBar( TQWidget *parent, const char *name )
    : TQButtonGroup( 0, Horizontal, "Progress Bar", parent, name ), timer()
{
    setMargin( 10 );

    TQGridLayout* toplayout = new TQGridLayout( layout(), 2, 2, 5);

    setRadioButtonExclusive( true );

    // insert three radiobuttons which the user can use
    // to set the speed of the progress and two pushbuttons
    // to start/pause/continue and reset the progress
    slow = new TQRadioButton( "S&low", this );
    normal = new TQRadioButton( "&Normal", this );
    fast = new TQRadioButton( "&Fast", this );
    TQVBoxLayout* vb1 = new TQVBoxLayout;
    toplayout->addLayout( vb1, 0, 0 );
    vb1->addWidget( slow );
    vb1->addWidget( normal );
    vb1->addWidget( fast );

    // two push buttons, one for start, for for reset.
    start = new TQPushButton( "&Start", this );
    reset = new TQPushButton( "&Reset", this );
    TQVBoxLayout* vb2 = new TQVBoxLayout;
    toplayout->addLayout( vb2, 0, 1 );
    vb2->addWidget( start );
    vb2->addWidget( reset );

    // Create the progressbar
    progress = new TQProgressBar( 100, this );
    //    progress->setStyle( new TQMotifStyle() );
    toplayout->addMultiCellWidget( progress, 1, 1, 0, 1 );

    // connect the clicked() SIGNALs of the pushbuttons to SLOTs
    connect( start, TQ_SIGNAL( clicked() ), this, TQ_SLOT( slotStart() ) );
    connect( reset, TQ_SIGNAL( clicked() ), this, TQ_SLOT( slotReset() ) );

    // connect the timeout() TQ_SIGNAL of the progress-timer to a TQ_SLOT
    connect( &timer, TQ_SIGNAL( timeout() ), this, TQ_SLOT( slotTimeout() ) );

    // Let's start with normal speed...
    normal->setChecked( true );


    // some contraints
    start->setFixedWidth( 80 );
    setMinimumWidth( 300 );
}

/*
 * TQ_SLOT slotStart
 *
 * This TQ_SLOT is called if the user clicks start/pause/continue
 * button
 */

void ProgressBar::slotStart()
{
    // If the progress bar is at the beginning...
    if ( progress->progress() == -1 ) {
        // ...set according to the checked speed-radiobutton
        // the number of steps which are needed to complete the process
        if ( slow->isChecked() )
            progress->setTotalSteps( 10000 );
        else if ( normal->isChecked() )
            progress->setTotalSteps( 1000 );
        else
            progress->setTotalSteps( 50 );

        // disable the speed-radiobuttons
        slow->setEnabled( false );
        normal->setEnabled( false );
        fast->setEnabled( false );
    }

    // If the progress is not running...
    if ( !timer.isActive() ) {
        // ...start the timer (and so the progress) with a interval of 1 ms...
        timer.start( 1 );
        // ...and rename the start/pause/continue button to Pause
        start->setText( "&Pause" );
    } else { // if the prgress is running...
        // ...stop the timer (and so the prgress)...
        timer.stop();
        // ...and rename the start/pause/continue button to Continue
        start->setText( "&Continue" );
    }
}

/*
 * TQ_SLOT slotReset
 *
 * This TQ_SLOT is called when the user clicks the reset button
 */

void ProgressBar::slotReset()
{
    // stop the timer and progress
    timer.stop();

    // rename the start/pause/continue button to Start...
    start->setText( "&Start" );
    // ...and enable this button
    start->setEnabled( true );

    // enable the speed-radiobuttons
    slow->setEnabled( true );
    normal->setEnabled( true );
    fast->setEnabled( true );

    // reset the progressbar
    progress->reset();
}

/*
 * TQ_SLOT slotTimeout
 *
 * This TQ_SLOT is called each ms when the timer is
 * active (== progress is running)
 */

void ProgressBar::slotTimeout()
{
    int p = progress->progress();

#if 1
    // If the progress is complete...
    if ( p == progress->totalSteps() )  {
        // ...rename the start/pause/continue button to Start...
        start->setText( "&Start" );
        // ...and disable it...
        start->setEnabled( false );
        // ...and return
        return;
    }
#endif

    // If the process is not complete increase it
    progress->setProgress( ++p );
}
