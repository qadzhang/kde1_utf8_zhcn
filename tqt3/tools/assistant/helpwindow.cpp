/**********************************************************************
** Copyright (C) 2000-2008 Trolltech ASA.  All rights reserved.
**
** This file is part of the TQt Assistant.
**
** This file may be used under the terms of the GNU General
** Public License versions 2.0 or 3.0 as published by the Free
** Software Foundation and appearing in the files LICENSE.GPL2
** and LICENSE.GPL3 included in the packaging of this file.
** Alternatively you may (at your option) use any later version
** of the GNU General Public License if such license has been
** publicly approved by Trolltech ASA (or its successors, if any)
** and the KDE Free TQt Foundation.
**
** Please review the following information to ensure GNU General
** Public Licensing requirements will be met:
** http://trolltech.com/products/qt/licenses/licensing/opensource/.
** If you are unsure which license is appropriate for your use, please
** review the following information:
** http://trolltech.com/products/qt/licenses/licensing/licensingoverview
** or contact the sales department at sales@trolltech.com.
**
** Licensees holding valid TQt Commercial licenses may use this file in
** accordance with the TQt Commercial License Agreement provided with
** the Software.
**
** This file is provided "AS IS" with NO WARRANTY OF ANY KIND,
** INCLUDING THE WARRANTIES OF DESIGN, MERCHANTABILITY AND FITNESS FOR
** A PARTICULAR PURPOSE. Trolltech reserves all rights not granted
** herein.
**
**********************************************************************/

#include "helpwindow.h"
#include "mainwindow.h"
#include "tabbedbrowser.h"
#include "helpdialogimpl.h"
#include "config.h"

#include <ntqapplication.h>
#include <ntqclipboard.h>
#include <ntqurl.h>
#include <ntqmessagebox.h>
#include <ntqdir.h>
#include <ntqfile.h>
#include <ntqprocess.h>
#include <ntqpopupmenu.h>
#include <ntqaction.h>
#include <ntqfileinfo.h>
#include <ntqevent.h>
#include <ntqtextstream.h>
#include <ntqtextcodec.h>

#if defined(Q_OS_WIN32)
#include <windows.h>
#endif

HelpWindow::HelpWindow( MainWindow *w, TQWidget *parent, const char *name )
    : TQTextBrowser( parent, name ), mw( w ), blockScroll( false ),
      shiftPressed( false ), newWindow( false )
{
    connect(this, TQ_SIGNAL(forwardAvailable(bool)), this, TQ_SLOT(updateForward(bool)));
    connect(this, TQ_SIGNAL(backwardAvailable(bool)), this, TQ_SLOT(updateBackward(bool)));
}

void HelpWindow::setSource( const TQString &name )
{
    if ( name.isEmpty() )
	return;

    if (newWindow || shiftPressed) {
	removeSelection();
	mw->saveSettings();
	mw->saveToolbarSettings();
	MainWindow *nmw = new MainWindow;

	TQFileInfo currentInfo( source() );
	TQFileInfo linkInfo( name );
	TQString target = name;
	if( linkInfo.isRelative() )
	    target = currentInfo.dirPath( true ) + "/" + name;

	nmw->setup();	
	nmw->move( mw->geometry().topLeft() );
	if ( mw->isMaximized() )
	    nmw->showMaximized();
	else
	    nmw->show();
    nmw->showLink( target );
    return;
    }

    if ( name.left( 7 ) == "http://" || name.left( 6 ) == "ftp://" ) {
	TQString webbrowser = Config::configuration()->webBrowser();
	if ( webbrowser.isEmpty() ) {
#if defined(Q_OS_WIN32)
	    QT_WA( {
		ShellExecute( winId(), 0, (TCHAR*)name.ucs2(), 0, 0, SW_SHOWNORMAL );
	    } , {
		ShellExecuteA( winId(), 0, name.local8Bit(), 0, 0, SW_SHOWNORMAL );
	    } );
#elif defined(Q_OS_MACX)
	    webbrowser = "/usr/bin/open";
#else
	    int result = TQMessageBox::information( mw, tr( "Help" ),
			 tr( "Currently no Web browser is selected.\nPlease use the settings dialog to specify one!\n" ),
			 "Open", "Cancel" );
	    if ( result == 0 ) {
		emit chooseWebBrowser();
		webbrowser = Config::configuration()->webBrowser();
	    }
#endif
	    if ( webbrowser.isEmpty() )
		return;
	}
	TQProcess *proc = new TQProcess(this);

	proc->addArgument( webbrowser );
        TQObject::connect(proc, TQ_SIGNAL(processExited()), proc, TQ_SLOT(deleteLater()));
	proc->addArgument( name );
	proc->start();
	return;
    }

    if ( name.right( 3 ) == "pdf" ) {
	TQString pdfbrowser = Config::configuration()->pdfReader();
	if ( pdfbrowser.isEmpty() ) {
#if defined(Q_OS_MACX)
	    pdfbrowser = "/usr/bin/open";
#else
	    TQMessageBox::information( mw,
				      tr( "Help" ),
				      tr( "No PDF Viewer has been specified\n"
					  "Please use the settings dialog to specify one!\n" ) );
	    return;
#endif
	}
	TQFileInfo info( pdfbrowser );
	if( !info.exists() ) {
	    TQMessageBox::information( mw,
				      tr( "Help" ),
				      tr( "TQt Assistant is unable to start the PDF Viewer\n\n"
					  "%1\n\n"
					  "Please make sure that the executable exists and is located at\n"
					  "the specified location." ).arg( pdfbrowser ) );
	    return;
	}
	TQProcess *proc = new TQProcess(this);
        TQObject::connect(proc, TQ_SIGNAL(processExited()), proc, TQ_SLOT(deleteLater()));
	proc->addArgument( pdfbrowser );
	proc->addArgument( name );
	proc->start();

	return;
    }

    TQUrl u( context(), name );
    if ( !u.isLocalFile() ) {
	TQMessageBox::information( mw, tr( "Help" ), tr( "Can't load and display non-local file\n"
		    "%1" ).arg( name ) );
	return;
    }

    setText("<body bgcolor=\"" + paletteBackgroundColor().name() + "\">");
    TQTextBrowser::setSource( name );
}


void HelpWindow::openLinkInNewWindow()
{
    if ( lastAnchor.isEmpty() )
	return;
    newWindow = true;
    setSource(lastAnchor);
    newWindow = false;
}

void HelpWindow::openLinkInNewWindow( const TQString &link )
{
    lastAnchor = link;
    openLinkInNewWindow();
}

void HelpWindow::openLinkInNewPage()
{
    if( lastAnchor.isEmpty() )
	return;
    mw->browsers()->newTab( lastAnchor );
    lastAnchor = TQString::null;
}

void HelpWindow::openLinkInNewPage( const TQString &link )
{
    lastAnchor = link;
    openLinkInNewPage();
}

TQPopupMenu *HelpWindow::createPopupMenu( const TQPoint& pos )
{
    TQPopupMenu *m = new TQPopupMenu(0);
    lastAnchor = anchorAt( pos );
    if ( !lastAnchor.isEmpty() ) {
	if ( lastAnchor.at( 0 ) == '#' ) {
	    TQString src = source();
	    int hsh = src.find( '#' );
	    lastAnchor = ( hsh>=0 ? src.left( hsh ) : src ) + lastAnchor;
	}
	m->insertItem( tr("Open Link in New Window\tShift+LMB"),
		       this, TQ_SLOT( openLinkInNewWindow() ) );
	m->insertItem( tr("Open Link in New Tab"),
		       this, TQ_SLOT( openLinkInNewPage() ) );
    }
    mw->actionNewWindow->addTo( m );
    mw->actionOpenPage->addTo( m );
    mw->actionClosePage->addTo( m );
    m->insertSeparator();
    mw->actionGoPrevious->addTo( m );
    mw->actionGoNext->addTo( m );
    mw->actionGoHome->addTo( m );
    m->insertSeparator();
    mw->actionZoomIn->addTo( m );
    mw->actionZoomOut->addTo( m );
    m->insertSeparator();
    mw->actionEditCopy->addTo( m );
    mw->actionEditFind->addTo( m );
    return m;
}

void HelpWindow::blockScrolling( bool b )
{
    blockScroll = b;
}

void HelpWindow::ensureCursorVisible()
{
    if ( !blockScroll )
	TQTextBrowser::ensureCursorVisible();
}

void HelpWindow::contentsMousePressEvent(TQMouseEvent *e)
{
    shiftPressed = ( e->state() & ShiftButton );
    TQTextBrowser::contentsMousePressEvent(e);
}

void HelpWindow::keyPressEvent(TQKeyEvent *e)
{
    shiftPressed = ( e->state() & ShiftButton );
    TQTextBrowser::keyPressEvent(e);
}

void HelpWindow::copy()
{
    if (textFormat() == PlainText) {
	TQTextEdit::copy();
    } else {
	TextFormat oldTf = textFormat();
	setTextFormat(PlainText);
	TQString selectText = selectedText();
	selectText.replace("<br>", "\n");
	selectText.replace("\xa0", " ");
	selectText.replace("&gt;", ">");
	selectText.replace("&lt;", "<");
	selectText.replace("&amp;", "&");

	TQClipboard *cb = TQApplication::clipboard();
	if (cb->supportsSelection())
	    cb->setText(selectText, TQClipboard::Selection);
	cb->setText(selectText, TQClipboard::Clipboard);
	setTextFormat(oldTf);
    }
}

void HelpWindow::updateForward(bool fwd)
{
    fwdAvail = fwd;
}

void HelpWindow::updateBackward(bool back)
{
    backAvail = back;
}
