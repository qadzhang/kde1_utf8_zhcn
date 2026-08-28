/****************************************************************************
**
** Copyright (C) 1992-2008 Trolltech ASA.  All rights reserved.
**
** This file is part of an example program for TQt.  This example
** program may be used, distributed and modified without limitation.
**
*****************************************************************************/

#include <ntqapplication.h>
#include <ntqtextedit.h>
#include <ntqpushbutton.h>
#include <ntqfiledialog.h>

#include "qip.h"
#include "client.h"




ClientInfo::ClientInfo( TQWidget *parent, const char *name ) :
    ClientInfoBase( parent, name )
{
    connect( btnOpen, TQ_SIGNAL(clicked()), TQ_SLOT(downloadFile()) );
    connect( btnQuit, TQ_SIGNAL(clicked()), tqApp, TQ_SLOT(quit()) );
    connect( &op, TQ_SIGNAL( data( const TQByteArray &, TQNetworkOperation * ) ),
	     this, TQ_SLOT( newData( const TQByteArray & ) ) );
}

void ClientInfo::downloadFile()
{
    // under Windows you must not use the native file dialog
    TQString file = getOpenFileName();
    if ( !file.isEmpty() ) {
	infoText->clear();
	// download the data
	op = file;
	op.get();
    }
}

TQString ClientInfo::getOpenFileName()
{
    static TQString workingDirectory( "qip://localhost/" );

    TQFileDialog dlg( workingDirectory, TQString::null, 0, 0, true );
    dlg.setCaption( TQFileDialog::tr( "Open" ) );
    dlg.setMode( TQFileDialog::ExistingFile );
    TQString result;
    if ( dlg.exec() == TQDialog::Accepted ) {
	result = dlg.selectedFile();
	workingDirectory = dlg.url();
    }
    return result;
}

void ClientInfo::newData( const TQByteArray &ba )
{
    infoText->append( TQString::fromUtf8( ba ) );
}
