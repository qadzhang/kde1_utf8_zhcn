/****************************************************************************
**
** Copyright (C) 1992-2008 Trolltech ASA.  All rights reserved.
**
** This file is part of an example program for TQt.  This example
** program may be used, distributed and modified without limitation.
**
*****************************************************************************/

#include "view.h"

#include <ntqlabel.h>
#include <ntqpushbutton.h>
#include <ntqmultilineedit.h>
#include <ntqfiledialog.h>

View::View()
    : TQVBox()
{
    // setup the GUI
    setSpacing( 5 );
    setMargin( 5 );

    TQLabel *l = new TQLabel( this );
    l->setAlignment( TQt::WordBreak ),
    l->setText( tr( "The button below opens the TQFileDialog and you "
		    "can choose a file then which is downloaded and "
		    "opened below then. You can use for that the <b>local "
		    "filesystem</b> using the file protocol, you can download "
		    "files from an <b>FTP</b> server using the ftp protocol and "
		    "you can download and open <b>USENET</b> articles using the "
		    "demo implementation of the nntp protocol of this "
		    "example (<i>This implementation of the nntp protocol is a very "
		    "basic and incomplete one, so you need to connect to a news server "
		    "which allows reading without authentification</i>)\n"
		    "To open a file from the local filesystem, enter in the "
		    "path combobox of the file dialog a url starting with file "
		    "(like <u>file:/usr/bin</u>), to download something from an FTP "
		    "server, use something like <u>ftp://ftp.trolltech.com</u> as url, and "
		    "for downloading a news article start with an url like "
		    "<u>nntp://news.tu-graz.ac.at</u> " ) );
    TQPushButton *b = new TQPushButton( tr( "Open a file..." ), this );
    connect( b, TQ_SIGNAL( clicked() ),
	     this, TQ_SLOT( downloadFile() ) );

    fileView = new TQMultiLineEdit( this );
    fileView->setReadOnly( true );

    // if new data comes in, display it
    connect( &op, TQ_SIGNAL( data( const TQByteArray &, TQNetworkOperation * ) ),
	     this, TQ_SLOT( newData( const TQByteArray & ) ) );
}

void View::downloadFile()
{
    // TQString file = TQFileDialog::getOpenFileName();
    // under Windows you must not use the native file dialog
    TQString file = getOpenFileName();
    if ( !file.isEmpty() ) {
	// clear the view
	fileView->clear();

	// download the data
	op = file;
	op.get();
    }
}

TQString View::getOpenFileName()
{
    static TQString workingDirectory = TQDir::currentDirPath();

    TQFileDialog *dlg = new TQFileDialog( workingDirectory,
	    TQString::null, 0, 0, true );
    dlg->setCaption( TQFileDialog::tr( "Open" ) );
    dlg->setMode( TQFileDialog::ExistingFile );
    TQString result;
    if ( dlg->exec() == TQDialog::Accepted ) {
	result = dlg->selectedFile();
	workingDirectory = dlg->url();
    }
    delete dlg;
    return result;
}

void View::newData( const TQByteArray &ba )
{
    // append new data
    fileView->append( ba );
}
