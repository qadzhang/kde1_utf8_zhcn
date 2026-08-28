/****************************************************************************
**
** Copyright (C) 1992-2008 Trolltech ASA.  All rights reserved.
**
** This file is part of an example program for TQt.  This example
** program may be used, distributed and modified without limitation.
**
*****************************************************************************/

#include <ntqapplication.h>
#include <ntqassistantclient.h>
#include <ntqfiledialog.h>
#include <ntqmenubar.h>
#include <ntqpopupmenu.h>
#include <ntqstatusbar.h>
#include <ntqtable.h>
#include <ntqtoolbar.h>
#include <ntqtoolbutton.h>
#include <ntqtooltip.h>

#include "mainwindow.h"
#include "tooltip.h"
#include "whatsthis.h"

MainWindow::MainWindow()
{
    statusBar();
    assistant = new TQAssistantClient( TQDir("../../bin").absPath(), this );

    TQTable* table = new TQTable( 2, 3, this );
    setCentralWidget( table );

    // populate table
    TQStringList comboEntries;
    comboEntries << "one" << "two" << "three" << "four";
    TQComboTableItem* comboItem1 = new TQComboTableItem( table, comboEntries );
    TQComboTableItem* comboItem2 = new TQComboTableItem( table, comboEntries );
    TQCheckTableItem* checkItem1 = new TQCheckTableItem( table, "Check me" );
    TQCheckTableItem* checkItem2 = new TQCheckTableItem( table, "Check me" );

    table->setItem( 0, 0, comboItem1 );
    table->setItem( 1, 0, comboItem2 );

    table->setItem( 1, 1, checkItem1  );
    table->setItem( 0, 1, checkItem2 );

    table->setText( 1, 2, "Text" );

    table->horizontalHeader()->setLabel( 0, " Combos" );
    table->horizontalHeader()->setLabel( 1, "Checkboxes" );
    table->verticalHeader()->setLabel( 0, "1" );
    table->verticalHeader()->setLabel( 1, "2" );


    // populate menubar
    TQPopupMenu* fileMenu = new TQPopupMenu( this );
    TQPopupMenu* helpMenu = new TQPopupMenu( this );

    menuBar()->insertItem( "&File", fileMenu );
    menuBar()->insertItem( "&Help", helpMenu );

    int fileId = fileMenu->insertItem( "E&xit", this, TQ_SLOT(close()) );

    int helpId = helpMenu->insertItem( "Open Assistant", this, TQ_SLOT(assistantSlot()) );

    // populate toolbar
    TQToolBar* toolbar = new TQToolBar( this );
    TQToolButton* assistantButton = new TQToolButton( toolbar );
    assistantButton->setIconSet( TQPixmap("appicon.png") );
    TQWhatsThis::whatsThisButton( toolbar );

    //create tooltipgroup
    TQToolTipGroup * tipGroup = new TQToolTipGroup( this );
    connect( tipGroup, TQ_SIGNAL(showTip(const TQString&)), statusBar(),
	TQ_SLOT(message(const TQString&)) );
    connect( tipGroup, TQ_SIGNAL(removeTip()), statusBar(), TQ_SLOT(clear()) );

    // set up tooltips
    TQToolTip::add( assistantButton, tr ("Open Assistant"), tipGroup, "Opens TQt Assistant" );

    horizontalTip = new HeaderToolTip( table->horizontalHeader(), tipGroup );
    verticalTip = new HeaderToolTip( table->verticalHeader(), tipGroup );

    cellTip = new TableToolTip( table, tipGroup );

    // set up whats this
    TQWhatsThis::add ( assistantButton, "This is a toolbutton which opens Assistant" );

    HeaderWhatsThis *horizontalWhatsThis = new HeaderWhatsThis( table->horizontalHeader() );
    HeaderWhatsThis *verticalWhatsThis = new HeaderWhatsThis( table->verticalHeader() );

    TableWhatsThis *cellWhatsThis = new TableWhatsThis( table );

    fileMenu->setWhatsThis( fileId, "Click here to exit the application" );
    helpMenu->setWhatsThis( helpId, "Click here to open Assistant" );

    // connections
    connect( assistantButton, TQ_SIGNAL(clicked()), this, TQ_SLOT(assistantSlot()) );
    connect( horizontalWhatsThis, TQ_SIGNAL(linkClicked(const TQString&)), assistant,
	TQ_SLOT(showPage(const TQString&)) );
    connect( verticalWhatsThis, TQ_SIGNAL(linkClicked(const TQString&)), assistant,
	TQ_SLOT(showPage(const TQString&)) );
    connect( cellWhatsThis, TQ_SIGNAL(linkClicked(const TQString&)), assistant,
	TQ_SLOT(showPage(const TQString&)) );
}

MainWindow::~MainWindow()
{
    delete horizontalTip;
    delete verticalTip;
    delete cellTip;
}

void MainWindow::assistantSlot()
{
    TQString docsPath = TQDir("../../doc").absPath();
    assistant->showPage( TQString("%1/html/ntqassistantclient.html").arg(docsPath) );
}
