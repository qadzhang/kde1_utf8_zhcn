/****************************************************************************
**
** Copyright (C) 1992-2008 Trolltech ASA.  All rights reserved.
**
** This file is part of an example program for TQt.  This example
** program may be used, distributed and modified without limitation.
**
*****************************************************************************/

#include "mainwindow.h"
#include "centralwidget.h"

#include <ntqtoolbar.h>
#include <ntqtoolbutton.h>
#include <ntqpopupmenu.h>
#include <ntqmenubar.h>
#include <ntqstatusbar.h>
#include <ntqapplication.h>
#include <ntqfiledialog.h>

ABMainWindow::ABMainWindow()
    : TQMainWindow( 0, "example addressbook application" ),
      filename( TQString::null )
{
    setupMenuBar();
    setupFileTools();
    setupStatusBar();
    setupCentralWidget();
}


ABMainWindow::~ABMainWindow()
{
}

void ABMainWindow::setupMenuBar()
{
    TQPopupMenu *file = new TQPopupMenu( this );
    menuBar()->insertItem( "&File", file );

    file->insertItem( "New", this, TQ_SLOT( fileNew() ), CTRL + Key_N );
    file->insertItem( TQPixmap( "fileopen.xpm" ), "Open", this, TQ_SLOT( fileOpen() ), CTRL + Key_O );
    file->insertSeparator();
    file->insertItem( TQPixmap( "filesave.xpm" ), "Save", this, TQ_SLOT( fileSave() ), CTRL + Key_S );
    file->insertItem( "Save As...", this, TQ_SLOT( fileSaveAs() ) );
    file->insertSeparator();
    file->insertItem( TQPixmap( "fileprint.xpm" ), "Print...", this, TQ_SLOT( filePrint() ), CTRL + Key_P );
    file->insertSeparator();
    file->insertItem( "Close", this, TQ_SLOT( closeWindow() ), CTRL + Key_W );
    file->insertItem( "Quit", tqApp, TQ_SLOT( quit() ), CTRL + Key_Q );
}

void ABMainWindow::setupFileTools()
{
    //fileTools = new TQToolBar( this, "file operations" );
}

void ABMainWindow::setupStatusBar()
{
    //statusBar()->message( "Ready", 2000 );
}

void ABMainWindow::setupCentralWidget()
{
    view = new ABCentralWidget( this );
    setCentralWidget( view );
}

void ABMainWindow::closeWindow()
{
    close();
}

void ABMainWindow::fileNew()
{
}

void ABMainWindow::fileOpen()
{
    TQString fn = TQFileDialog::getOpenFileName( TQString::null, TQString::null, this );
    if ( !fn.isEmpty() ) {
        filename = fn;
        view->load( filename );
    }
}

void ABMainWindow::fileSave()
{
    if ( filename.isEmpty() ) {
        fileSaveAs();
        return;
    }

    view->save( filename );
}

void ABMainWindow::fileSaveAs()
{
    TQString fn = TQFileDialog::getSaveFileName( TQString::null, TQString::null, this );
    if ( !fn.isEmpty() ) {
        filename = fn;
        fileSave();
    }
}

void ABMainWindow::filePrint()
{
}
