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
** ui.h extension file, included from the uic-generated form implementation.
**
** The init() function is used in place of a constructor.
** The destroy() function is used in place of a destructor.
** The slots uploadFile(), downloadFile(), removeFile() and connectToHost() are
** connected with the resp. actions of the GUI.
**
*****************************************************************************/

#include <ntqftp.h>
#include <ntqlineedit.h>
#include <ntqspinbox.h>
#include <ntqstatusbar.h>
#include <ntqmessagebox.h>
#include <ntqfiledialog.h>
#include <ntqprogressdialog.h>
#include <ntqapplication.h>

#include "connectdialog.h"
#include "ftpviewitem.h"

void FtpMainWindow::init()
{
    stateFtp = new TQLabel( tr("Unconnected"), statusBar() );
    statusBar()->addWidget( stateFtp, 0, true );

    ftp = new TQFtp( this );
    connect( ftp, TQ_SIGNAL(commandStarted(int)),
	    TQ_SLOT(ftp_commandStarted()) );
    connect( ftp, TQ_SIGNAL(commandFinished(int,bool)),
	    TQ_SLOT(ftp_commandFinished()) );
    connect( ftp, TQ_SIGNAL(done(bool)),
	    TQ_SLOT(ftp_done(bool)) );
    connect( ftp, TQ_SIGNAL(stateChanged(int)),
	    TQ_SLOT(ftp_stateChanged(int)) );
    connect( ftp, TQ_SIGNAL(listInfo(const TQUrlInfo &)),
	    TQ_SLOT(ftp_listInfo(const TQUrlInfo &)) );
    connect( ftp, TQ_SIGNAL(rawCommandReply(int, const TQString &)),
	    TQ_SLOT(ftp_rawCommandReply(int, const TQString &)) );
}

void FtpMainWindow::destroy()
{
    if ( ftp->state() != TQFtp::Unconnected )
	ftp->close();
}

void FtpMainWindow::uploadFile()
{
    TQString fileName = TQFileDialog::getOpenFileName(
	    TQString::null,
	    TQString::null,
	    this,
	    "upload file dialog",
	    tr("Choose a file to upload") );
    if ( fileName.isNull() )
	return;

    TQFile *file = new TQFile( fileName );
    if ( !file->open( IO_ReadOnly ) ) {
	TQMessageBox::critical( this, tr("Upload error"),
		tr("Can't open file '%1' for reading.").arg(fileName) );
	delete file;
	return;
    }

    TQProgressDialog progress(
	    tr("Uploading file..."),
	    tr("Cancel"),
	    0,
	    this,
	    "upload progress dialog",
	    true );
    connect( ftp, TQ_SIGNAL(dataTransferProgress(int,int)),
	    &progress, TQ_SLOT(setProgress(int,int)) );
    connect( ftp, TQ_SIGNAL(commandFinished(int,bool)),
	    &progress, TQ_SLOT(reset()) );
    connect( &progress, TQ_SIGNAL(cancelled()),
	    ftp, TQ_SLOT(abort()) );

    TQFileInfo fi( fileName );
    ftp->put( file, fi.fileName() );
    progress.exec(); // ### takes a lot of time!!!

    ftp->list();
}

void FtpMainWindow::downloadFile()
{
    FtpViewItem *item = (FtpViewItem*)remoteView->selectedItem();
    if ( !item || item->isDir() )
	return;

    TQString fileName = TQFileDialog::getSaveFileName(
	    item->text(0),
	    TQString::null,
	    this,
	    "download file dialog",
	    tr("Save downloaded file as") );
    if ( fileName.isNull() )
	return;

    // create file on the heap because it has to be valid throughout the whole
    // asynchronous download operation
    TQFile *file = new TQFile( fileName );
    if ( !file->open( IO_WriteOnly ) ) {
	TQMessageBox::critical( this, tr("Download error"),
		tr("Can't open file '%1' for writing.").arg(fileName) );
	delete file;
	return;
    }

    TQProgressDialog progress(
	    tr("Downloading file..."),
	    tr("Cancel"),
	    0,
	    this,
	    "download progress dialog",
	    true );
    connect( ftp, TQ_SIGNAL(dataTransferProgress(int,int)),
	    &progress, TQ_SLOT(setProgress(int,int)) );
    connect( ftp, TQ_SIGNAL(commandFinished(int,bool)),
	    &progress, TQ_SLOT(reset()) );
    connect( &progress, TQ_SIGNAL(cancelled()),
	    ftp, TQ_SLOT(abort()) );

    ftp->get( item->text(0), file );
    progress.exec(); // ### takes a lot of time!!!
}

void FtpMainWindow::removeFile()
{
    FtpViewItem *item = (FtpViewItem*)remoteView->selectedItem();
    if ( !item || item->isDir() )
	return;

    ftp->remove( item->text(0) );
    ftp->list();
}

void FtpMainWindow::connectToHost()
{
    ConnectDialog connectDialog;
    if ( connectDialog.exec() == TQDialog::Rejected )
	return;

    remotePath->clear();
    remoteView->clear();

    if ( ftp->state() != TQFtp::Unconnected )
	ftp->close();

    ftp->connectToHost( connectDialog.host->text(), connectDialog.port->value() );
    ftp->login( connectDialog.username->text(), connectDialog.password->text() );
    ftp->rawCommand( "PWD" );
    ftp->list();
}

// This slot is connected to the TQComboBox::activated() signal of the
// remotePath.
void FtpMainWindow::changePath( const TQString &newPath )
{
    ftp->cd( newPath );
    ftp->rawCommand( "PWD" );
    ftp->list();
}

// This slot is connected to the TQListView::doubleClicked() and
// TQListView::returnPressed() signals of the remoteView.
void FtpMainWindow::changePathOrDownload( TQListViewItem *item )
{
    if ( ((FtpViewItem*)item)->isDir() )
	changePath( item->text(0) );
    else
	downloadFile();
}

/****************************************************************************
**
** Slots connected to signals of the TQFtp class
**
*****************************************************************************/

void FtpMainWindow::ftp_commandStarted()
{
    TQApplication::setOverrideCursor( TQCursor(TQt::WaitCursor) );
    if ( ftp->currentCommand() == TQFtp::List ) {
	remoteView->clear();
	if ( currentFtpDir != "/" )
	    new FtpViewItem( remoteView, FtpViewItem::Directory, "..", "", "" );
    }
}

void FtpMainWindow::ftp_commandFinished()
{
    TQApplication::restoreOverrideCursor();
    delete ftp->currentDevice();
}

void FtpMainWindow::ftp_done( bool error )
{
    if ( error ) {
	TQMessageBox::critical( this, tr("FTP Error"), ftp->errorString() );

	// If we are connected, but not logged in, it is not meaningful to stay
	// connected to the server since the error is a really fatal one (login
	// failed).
	if ( ftp->state() == TQFtp::Connected )
	    ftp->close();
    }
}

void FtpMainWindow::ftp_stateChanged( int state )
{
    switch ( (TQFtp::State)state ) {
	case TQFtp::Unconnected:
	    stateFtp->setText( tr("Unconnected") );
	    break;
	case TQFtp::HostLookup:
	    stateFtp->setText( tr("Host lookup") );
	    break;
	case TQFtp::Connecting:
	    stateFtp->setText( tr("Connecting") );
	    break;
	case TQFtp::Connected:
	    stateFtp->setText( tr("Connected") );
	    break;
	case TQFtp::LoggedIn:
	    stateFtp->setText( tr("Logged in") );
	    break;
	case TQFtp::Closing:
	    stateFtp->setText( tr("Closing") );
	    break;
    }
}

void FtpMainWindow::ftp_listInfo( const TQUrlInfo &i )
{
    FtpViewItem::Type type;
    if ( i.isDir() )
	type = FtpViewItem::Directory;
    else
	type = FtpViewItem::File;

    new FtpViewItem( remoteView, type,
	    i.name(), TQString::number(i.size()), i.lastModified().toString() );
}

void FtpMainWindow::ftp_rawCommandReply( int code, const TQString &text )
{
    if ( code == 257 ) {
	currentFtpDir = text.section( '"', 1, 1 );

	for ( int i = 0; i<remotePath->count(); i++ ) {
	    // make sure that we don't insert duplicates
	    if ( remotePath->text( i ) == currentFtpDir )
		remotePath->removeItem( i );
	}
	remotePath->insertItem( currentFtpDir, 0 );
	remotePath->setCurrentItem( 0 );
    }
}
