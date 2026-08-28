/****************************************************************************
**
** Implementation of a nice qInstallMsgHandler() handler
**
** Created : 000527, after Kalle Dalheimer's birthday
**
** Copyright (C) 1992-2008 Trolltech ASA.  All rights reserved.
**
** This file is part of the dialogs module of the TQt GUI Toolkit.
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
** This file may be used under the terms of the Q Public License as
** defined by Trolltech ASA and appearing in the file LICENSE.TQPL
** included in the packaging of this file.  Licensees holding valid TQt
** Commercial licenses may use this file in accordance with the TQt
** Commercial License Agreement provided with the Software.
**
** This file is provided "AS IS" with NO WARRANTY OF ANY KIND,
** INCLUDING THE WARRANTIES OF DESIGN, MERCHANTABILITY AND FITNESS FOR
** A PARTICULAR PURPOSE. Trolltech reserves all rights not granted
** herein.
**
**********************************************************************/

#include "ntqerrormessage.h"

#ifndef TQT_NO_ERRORMESSAGE

#include "ntqapplication.h"
#include "ntqcheckbox.h"
#include "ntqdict.h"
#include "ntqlabel.h"
#include "ntqlayout.h"
#include "ntqmessagebox.h"
#include "ntqpushbutton.h"
#include "ntqstringlist.h"
#include "ntqstylesheet.h"
#include "ntqtextview.h"

#include <stdio.h>
#include <stdlib.h>

class TQErrorMessageTextView : public TQTextView
{
public:
    TQErrorMessageTextView( TQWidget *parent, const char *name )
	: TQTextView( parent, name ) { }

    virtual TQSize minimumSizeHint() const;
    virtual TQSize sizeHint() const;
};

TQSize TQErrorMessageTextView::minimumSizeHint() const
{
    return TQSize( 50, 50 );
}

TQSize TQErrorMessageTextView::sizeHint() const
{
    return TQSize( 250, 75 );
}

/*! \class TQErrorMessage

  \brief The TQErrorMessage class provides an error message display dialog.

  \ingroup dialogs
  \ingroup misc

This is basically a TQLabel and a "show this message again" checkbox which
remembers what not to show.

There are two ways to use this class:
\list 1
\i For production applications. In this context the class can be used to
display messages which you don't need the user to see more than once. To use
TQErrorMessage like this, you create the dialog in the usual way and call the
message() slot, or connect signals to it.

\i For developers. In this context the static qtHandler() installs
a message handler using qInstallMsgHandler() and creates a TQErrorMessage
that displays tqDebug(), tqWarning() and tqFatal() messages.
\endlist

In both cases TQErrorMessage will queue pending messages, and display
them (or not) in order, as soon as the user presses Enter or clicks OK
after seeing each message.

\img qerrormessage.png

\sa TQMessageBox TQStatusBar::message()
*/

static TQErrorMessage * qtMessageHandler = 0;

static void deleteStaticcTQErrorMessage() // post-routine
{
    if ( qtMessageHandler ) {
	delete qtMessageHandler;
	qtMessageHandler = 0;
    }
}

static bool metFatal = false;

void jump( TQtMsgType t, const char * m )
{
    if ( !qtMessageHandler )
	return;

    TQString rich;

    switch ( t ) {
    case TQtDebugMsg:
    default:
	rich = TQErrorMessage::tr( "Debug Message:" );
	break;
    case TQtWarningMsg:
	rich = TQErrorMessage::tr( "Warning:" );
	break;
    case TQtFatalMsg:
	rich = TQErrorMessage::tr( "Fatal Error:" );
    }
    rich = TQString( "<p><b>%1</b></p>" ).arg( rich );
    rich += TQStyleSheet::convertFromPlainText( m,
		TQStyleSheetItem::WhiteSpaceNormal );

    // ### work around text engine quirk
    if ( rich.endsWith("</p>") )
	rich.truncate( rich.length() - 4 );

    if ( !metFatal ) {
	qtMessageHandler->message( rich );
	metFatal = ( t == TQtFatalMsg );
    }
}


/*!  Constructs and installs an error handler window.
    The parent \a parent and name \a name are passed on to the TQDialog
    constructor.
*/

TQErrorMessage::TQErrorMessage( TQWidget * parent, const char * name )
    : TQDialog( parent, name )
{
    TQGridLayout * grid = new TQGridLayout( this, 3, 2, 11, 6 );
    icon = new TQLabel( this, "qt_icon_lbl" );
#ifndef TQT_NO_MESSAGEBOX
    icon->setPixmap( TQMessageBox::standardIcon(TQMessageBox::Information) );
#endif
    grid->addWidget( icon, 0, 0, AlignTop );
    errors = new TQErrorMessageTextView( this, "errors" );
    grid->addWidget( errors, 0, 1 );
    again = new TQCheckBox( tr( "&Show this message again" ), this, "again" );
    again->setChecked( true );
    grid->addWidget( again, 1, 1, AlignTop + AlignAuto );
    ok = new TQPushButton( tr( "&OK" ), this, "ok" );
    connect( ok, TQ_SIGNAL(clicked()), this, TQ_SLOT(accept()) );
    ok->setFocus();
    grid->addMultiCellWidget( ok, 2, 2, 0, 1, AlignCenter );
    grid->setColStretch( 1, 42 );
    grid->setRowStretch( 0, 42 );
    pending = new TQStringList;
    doNotShow = new TQDict<int>;
}


/*! Destroys the object and frees any allocated resources.  Notably,
the list of "do not show again" messages is deleted. */

TQErrorMessage::~TQErrorMessage()
{
    if ( this == qtMessageHandler ) {
	qtMessageHandler = 0;
	TQtMsgHandler tmp = qInstallMsgHandler( 0 );
	// in case someone else has later stuck in another...
	if ( tmp != jump )
	    qInstallMsgHandler( tmp );
    }

    delete pending;
    delete doNotShow;
}


/*! \reimp */

void TQErrorMessage::done( int a )
{
    int dummy = 0;
    if ( !again->isChecked() )
	doNotShow->insert( errors->text(), &dummy );
    if ( !nextPending() ) {
	TQDialog::done( a );
	if ( this == qtMessageHandler && metFatal )
	    exit( 1 );
    }
}


/*!  Returns a pointer to a TQErrorMessage object that outputs the
default TQt messages.  This function creates such an object, if there
isn't one already.
*/

TQErrorMessage * TQErrorMessage::qtHandler()
{
    if ( !qtMessageHandler ) {
	qtMessageHandler = new TQErrorMessage( 0, "automatic message handler" );
	tqAddPostRoutine( deleteStaticcTQErrorMessage ); // clean up
#ifndef TQT_NO_WIDGET_TOPEXTRA
	if ( tqApp->mainWidget() )
	    qtMessageHandler->setCaption( tqApp->mainWidget()->caption() );
#endif
	qInstallMsgHandler( jump );
    }
    return qtMessageHandler;
}


/*! \internal */

bool TQErrorMessage::nextPending()
{
    while ( !pending->isEmpty() ) {
	TQString p = *pending->begin();
	pending->remove( pending->begin() );
	if ( !p.isEmpty() && !doNotShow->find( p ) ) {
	    errors->setText( p );
	    return true;
	}
    }
    return false;
}


/*! Shows message \a m and returns immediately.  If the user has requested
  that \a m not be shown, this function does nothing.

  Normally, \a m is shown at once, but if there are pending messages,
  \a m is queued for later display.
*/

void TQErrorMessage::message( const TQString & m )
{
    if ( doNotShow->find( m ) )
	return;
    pending->append( m );
    if ( !isVisible() && nextPending() )
	show();
}

#endif // TQT_NO_ERRORMESSAGE
