/****************************************************************************
**
** Copyright (C) 1992-2008 Trolltech ASA.  All rights reserved.
**
** This file is part of an example program for TQt.  This example
** program may be used, distributed and modified without limitation.
**
*****************************************************************************/

#include "mainwindow.h"
#include "qfileiconview.h"
#include "../dirview/dirview.h"

#include <ntqsplitter.h>
#include <ntqprogressbar.h>
#include <ntqlabel.h>
#include <ntqstatusbar.h>
#include <ntqtoolbar.h>
#include <ntqcombobox.h>
#include <ntqpixmap.h>
#include <ntqtoolbutton.h>
#include <ntqdir.h>
#include <ntqfileinfo.h>

static const char* cdtoparent_xpm[]={
    "15 13 3 1",
    ". c None",
    "* c #000000",
    "a c #ffff99",
    "..*****........",
    ".*aaaaa*.......",
    "***************",
    "*aaaaaaaaaaaaa*",
    "*aaaa*aaaaaaaa*",
    "*aaa***aaaaaaa*",
    "*aa*****aaaaaa*",
    "*aaaa*aaaaaaaa*",
    "*aaaa*aaaaaaaa*",
    "*aaaa******aaa*",
    "*aaaaaaaaaaaaa*",
    "*aaaaaaaaaaaaa*",
    "***************"};

static const char* newfolder_xpm[] = {
    "15 14 4 1",
    " 	c None",
    ".	c #000000",
    "+	c #FFFF00",
    "@	c #FFFFFF",
    "          .    ",
    "               ",
    "          .    ",
    "       .     . ",
    "  ....  . . .  ",
    " .+@+@.  . .   ",
    "..........  . .",
    ".@+@+@+@+@..   ",
    ".+@+@+@+@+. .  ",
    ".@+@+@+@+@.  . ",
    ".+@+@+@+@+.    ",
    ".@+@+@+@+@.    ",
    ".+@+@+@+@+.    ",
    "...........    "};

FileMainWindow::FileMainWindow()
    : TQMainWindow()
{
    setup();
}

void FileMainWindow::show()
{
    TQMainWindow::show();
}

void FileMainWindow::setup()
{
    TQSplitter *splitter = new TQSplitter( this );

    dirlist = new DirectoryView( splitter, "dirlist", true );
    dirlist->addColumn( "Name" );
    dirlist->addColumn( "Type" );
    Directory *root = new Directory( dirlist, "/" );
    root->setOpen( true );
    splitter->setResizeMode( dirlist, TQSplitter::KeepSize );

    fileview = new TQtFileIconView( "/", splitter );
    fileview->setSelectionMode( TQIconView::Extended );

    setCentralWidget( splitter );

    TQToolBar *toolbar = new TQToolBar( this, "toolbar" );
    setRightJustification( true );

    (void)new TQLabel( tr( " Path: " ), toolbar );

    pathCombo = new TQComboBox( true, toolbar );
    pathCombo->setAutoCompletion( true );
    toolbar->setStretchableWidget( pathCombo );
    connect( pathCombo, TQ_SIGNAL( activated( const TQString & ) ),
	     this, TQ_SLOT ( changePath( const TQString & ) ) );

    toolbar->addSeparator();

    TQPixmap pix;

    pix = TQPixmap( cdtoparent_xpm );
    upButton = new TQToolButton( pix, "One directory up", TQString::null,
				this, TQ_SLOT( cdUp() ), toolbar, "cd up" );

    pix = TQPixmap( newfolder_xpm );
    mkdirButton = new TQToolButton( pix, "New Folder", TQString::null,
				   this, TQ_SLOT( newFolder() ), toolbar, "new folder" );

    connect( dirlist, TQ_SIGNAL( folderSelected( const TQString & ) ),
	     fileview, TQ_SLOT ( setDirectory( const TQString & ) ) );
    connect( fileview, TQ_SIGNAL( directoryChanged( const TQString & ) ),
	     this, TQ_SLOT( directoryChanged( const TQString & ) ) );
    connect( fileview, TQ_SIGNAL( startReadDir( int ) ),
	     this, TQ_SLOT( slotStartReadDir( int ) ) );
    connect( fileview, TQ_SIGNAL( readNextDir() ),
	     this, TQ_SLOT( slotReadNextDir() ) );
    connect( fileview, TQ_SIGNAL( readDirDone() ),
	     this, TQ_SLOT( slotReadDirDone() ) );

    setDockEnabled( DockLeft, false );
    setDockEnabled( DockRight, false );

    label = new TQLabel( statusBar() );
    statusBar()->addWidget( label, 2, true );
    progress = new TQProgressBar( statusBar() );
    statusBar()->addWidget( progress, 1, true );

    connect( fileview, TQ_SIGNAL( enableUp() ),
	     this, TQ_SLOT( enableUp() ) );
    connect( fileview, TQ_SIGNAL( disableUp() ),
	     this, TQ_SLOT( disableUp() ) );
    connect( fileview, TQ_SIGNAL( enableMkdir() ),
	     this, TQ_SLOT( enableMkdir() ) );
    connect( fileview, TQ_SIGNAL( disableMkdir() ),
	     this, TQ_SLOT( disableMkdir() ) );
}

void FileMainWindow::setPathCombo()
{
    TQString dir = caption();
    int i = 0;
    bool found = false;
    for ( i = 0; i < pathCombo->count(); ++i ) {
	if ( pathCombo->text( i ) == dir) {
	    found = true;
	    break;
	}
    }

    if ( found )
	pathCombo->setCurrentItem( i );
    else {
	pathCombo->insertItem( dir );
	pathCombo->setCurrentItem( pathCombo->count() - 1 );
    }

}

void FileMainWindow::directoryChanged( const TQString &dir )
{
    setCaption( dir );
    setPathCombo();
}

void FileMainWindow::slotStartReadDir( int dirs )
{
    label->setText( tr( " Reading Directory..." ) );
    progress->reset();
    progress->setTotalSteps( dirs );
}

void FileMainWindow::slotReadNextDir()
{
    int p = progress->progress();
    progress->setProgress( ++p );
}

void FileMainWindow::slotReadDirDone()
{
    label->setText( tr( " Reading Directory Done." ) );
    progress->setProgress( progress->totalSteps() );
}

void FileMainWindow::cdUp()
{
    TQDir dir = fileview->currentDir();
    dir.cd( ".." );
    fileview->setDirectory( dir );
}

void FileMainWindow::newFolder()
{
    fileview->newDirectory();
}

void FileMainWindow::changePath( const TQString &path )
{
    if ( TQFileInfo( path ).exists() )
	fileview->setDirectory( path );
    else
	setPathCombo();
}

void FileMainWindow::enableUp()
{
    upButton->setEnabled( true );
}

void FileMainWindow::disableUp()
{
    upButton->setEnabled( false );
}

void FileMainWindow::enableMkdir()
{
    mkdirButton->setEnabled( true );
}

void FileMainWindow::disableMkdir()
{
    mkdirButton->setEnabled( false );
}
