/****************************************************************************
**
** Copyright (C) 1992-2008 Trolltech ASA.  All rights reserved.
**
** This file is part of an example program for TQt.  This example
** program may be used, distributed and modified without limitation.
**
*****************************************************************************/

/****************************************************************************
** ui.h extension file, included from the uic-generated form implementation.
**
** If you wish to add, delete or rename functions or slots use
** TQt Designer which will update this file, preserving your code. Create an
** init() function in place of a constructor, and a destroy() function in
** place of a destructor.
*****************************************************************************/

void ArchiveDialog::init()
{
    connect(&articleSearcher, TQ_SIGNAL(done(bool)), this, TQ_SLOT(searchDone(bool)));
    connect(&articleFetcher, TQ_SIGNAL(done(bool)), this, TQ_SLOT(fetchDone(bool)));
    connect(myListView, TQ_SIGNAL(selectionChanged(TQListViewItem*)), this, TQ_SLOT(fetch(TQListViewItem*)));
    connect(myLineEdit, TQ_SIGNAL(returnPressed()), this, TQ_SLOT(search()));
    connect(myListView, TQ_SIGNAL(returnPressed(TQListViewItem*)), this, TQ_SLOT(fetch(TQListViewItem*)));
    connect(myPushButton, TQ_SIGNAL(clicked()), this, TQ_SLOT(close()));
}

void ArchiveDialog::fetch( TQListViewItem *it )
{
    TQUrl u(it->text(1));
    articleFetcher.setHost(u.host());
    articleFetcher.get(it->text(1));
}

void ArchiveDialog::fetchDone( bool error )
{
    if (error) {
	TQMessageBox::critical(this, "Error fetching",
			      "An error occurred when fetching this document: "
			      + articleFetcher.errorString(),	
			      TQMessageBox::Ok, TQMessageBox::NoButton);
    } else {
	myTextBrowser->setText(articleFetcher.readAll());
    }
}

void ArchiveDialog::search()
{
    if (articleSearcher.state() == TQHttp::HostLookup
	|| articleSearcher.state() == TQHttp::Connecting
	|| articleSearcher.state() == TQHttp::Sending
	|| articleSearcher.state() == TQHttp::Reading) {
	articleSearcher.abort();
    }
    
    if (myLineEdit->text() == "") {
	TQMessageBox::critical(this, "Empty query",
			      "Please type a search string.",
			      TQMessageBox::Ok, TQMessageBox::NoButton);
    } else {
	TQApplication::setOverrideCursor(TQCursor(TQt::WaitCursor));
    
	articleSearcher.setHost("lists.trolltech.com");
    
	TQHttpRequestHeader header("POST", "/qt-interest/search.php");
	header.setValue("Host", "lists.trolltech.com");
	header.setContentType("application/x-www-form-urlencoded");
    
	TQString encodedTopic = myLineEdit->text();
	TQUrl::encode(encodedTopic);
	TQString searchString = "qt-interest=on&search=" + encodedTopic;

	articleSearcher.request(header, searchString.utf8());
    }

}

void ArchiveDialog::searchDone( bool error )
{
    if (error) {
	TQMessageBox::critical(this, "Error searching",
			      "An error occurred when searching: "
			      + articleSearcher.errorString(),
			      TQMessageBox::Ok, TQMessageBox::NoButton);
    } else {
	TQString result(articleSearcher.readAll());
	
	TQRegExp rx("<a href=\"(http://lists\\.trolltech\\.com/qt-interest/.*)\">(.*)</a>");
	rx.setMinimal(true);
	int pos = 0;
	while (pos >= 0) {
	    pos = rx.search(result, pos);
	    if (pos > -1) {
		pos += rx.matchedLength();
		new TQListViewItem(myListView, rx.cap(2), rx.cap(1));
	    }
	}
    }
    
    TQApplication::restoreOverrideCursor();
}
