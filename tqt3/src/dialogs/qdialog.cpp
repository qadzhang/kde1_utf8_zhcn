/****************************************************************************
**
** Implementation of TQDialog class
**
** Created : 950502
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

#include "ntqdialog.h"

#ifndef TQT_NO_DIALOG

#include "ntqpushbutton.h"
#include "ntqfocusdata.h"
#include "ntqapplication.h"
#include "ntqobjectlist.h"
#include "ntqwidgetlist.h"
#include "ntqlayout.h"
#include "ntqsizegrip.h"
#include "ntqwhatsthis.h"
#include "ntqpopupmenu.h"
#include "ntqcursor.h"
#if defined(QT_ACCESSIBILITY_SUPPORT)
#include "ntqaccessible.h"
#endif
#if defined( Q_OS_TEMP )
#include "qt_windows.h"
#endif

/*!
    \class TQDialog
    \brief The TQDialog class is the base class of dialog windows.

    \ingroup dialogs
    \ingroup abstractwidgets
    \mainclass

    A dialog window is a top-level window mostly used for short-term
    tasks and brief communications with the user. TQDialogs may be
    modal or modeless. TQDialogs support \link #extensibility
    extensibility\endlink and can provide a \link #return return
    value\endlink. They can have \link #default default
    buttons\endlink. TQDialogs can also have a TQSizeGrip in their
    lower-right corner, using setSizeGripEnabled().

    Note that TQDialog uses the parent widget slightly differently from
    other classes in TQt. A dialog is always a top-level widget, but if
    it has a parent, its default location is centered on top of the
    parent's top-level widget (if it is not top-level itself). It will
    also share the parent's taskbar entry.

    \target modal
    \section1 Modal Dialogs

    A <b>modal</b> dialog is a dialog that blocks input to other
    visible windows in the same application. Users must finish
    interacting with the dialog and close it before they can access
    any other window in the application. Dialogs that are used to
    request a file name from the user or that are used to set
    application preferences are usually modal.

    The most common way to display a modal dialog is to call its
    exec() function. When the user closes the dialog, exec() will
    provide a useful \link #return return value\endlink. Typically we
    connect a default button, e.g. "OK", to the accept() slot and a
    "Cancel" button to the reject() slot, to get the dialog to close
    and return the appropriate value. Alternatively you can connect to
    the done() slot, passing it \c Accepted or \c Rejected.

    An alternative is to call setModal(true), then show(). Unlike
    exec(), show() returns control to the caller immediately. Calling
    setModal(true) is especially useful for progress dialogs, where
    the user must have the ability to interact with the dialog, e.g.
    to cancel a long running operation. If you use show() and
    setModal(true) together you must call
    TQApplication::processEvents() periodically during processing to
    enable the user to interact with the dialog. (See \l
    TQProgressDialog.)

    \target modeless
    \section1 Modeless Dialogs

    A <b>modeless</b> dialog is a dialog that operates
    independently of other windows in the same application. Find and
    replace dialogs in word-processors are often modeless to allow the
    user to interact with both the application's main window and with
    the dialog.

    Modeless dialogs are displayed using show(), which returns control
    to the caller immediately.

    \target default
    \section1 Default button

    A dialog's \e default button is the button that's pressed when the
    user presses Enter (Return). This button is used to signify that
    the user accepts the dialog's settings and wants to close the
    dialog. Use TQPushButton::setDefault(), TQPushButton::isDefault()
    and TQPushButton::autoDefault() to set and control the dialog's
    default button.

    \target escapekey
    \section1 Escape Key

    If the user presses the Esc key in a dialog, TQDialog::reject()
    will be called. This will cause the window to close: the \link
    TQCloseEvent closeEvent \endlink cannot be \link
    TQCloseEvent::ignore() ignored \endlink.

    \target extensibility
    \section1 Extensibility

    Extensibility is the ability to show the dialog in two ways: a
    partial dialog that shows the most commonly used options, and a
    full dialog that shows all the options. Typically an extensible
    dialog will initially appear as a partial dialog, but with a
    "More" toggle button. If the user presses the "More" button down,
    the full dialog will appear. The extension widget will be resized
    to its sizeHint(). If orientation is \c Horizontal the extension
    widget's height() will be expanded to the height() of the dialog.
    If the orientation is \c Vertical the extension widget's width()
    will be expanded to the width() of the dialog. Extensibility is
    controlled with setExtension(), setOrientation() and
    showExtension().

    \target return
    \section1 Return value (modal dialogs)

    Modal dialogs are often used in situations where a return value is
    required, e.g. to indicate whether the user pressed "OK" or
    "Cancel". A dialog can be closed by calling the accept() or the
    reject() slots, and exec() will return \c Accepted or \c Rejected
    as appropriate. The exec() call returns the result of the dialog.
    The result is also available from result() if the dialog has not
    been destroyed. If the \c WDestructiveClose flag is set, the
    dialog is deleted after exec() returns.

    \target examples
    \section1 Examples

    A modal dialog.

    \quotefile network/networkprotocol/view.cpp
    \skipto TQFileDialog *dlg
    \printuntil return

    A modeless dialog. After the show() call, control returns to the main
    event loop.
    \quotefile life/main.cpp
    \skipto argv
    \printuntil TQApplication
    \skipto scale
    \printline
    \skipto LifeDialog
    \printuntil show
    \skipto exec
    \printuntil }

    \sa TQTabDialog TQWidget TQProgressDialog
    \link guibooks.html#fowler GUI Design Handbook: Dialogs, Standard\endlink
*/

/*! \enum TQDialog::DialogCode

    The value returned by a modal dialog.

    \value Accepted
    \value Rejected

*/

/*!
  \property TQDialog::sizeGripEnabled
  \brief whether the size grip is enabled

  A TQSizeGrip is placed in the bottom right corner of the dialog when this
  property is enabled. By default, the size grip is disabled.
*/

class TQDialogPrivate : public TQt
{
public:

    TQDialogPrivate()
	: mainDef(0), orientation(Horizontal),extension(0), doShowExtension(false)
#ifndef TQT_NO_SIZEGRIP
	,resizer(0)
#endif
	{
    }

    TQPushButton* mainDef;
    Orientation orientation;
    TQWidget* extension;
    bool doShowExtension;
    TQSize size, min, max;
#ifndef TQT_NO_SIZEGRIP
    TQSizeGrip* resizer;
#endif
    TQPoint lastRMBPress;
    TQPoint relPos; // relative position to the main window
};

/*!
  Constructs a dialog called \a name, with parent \a parent.

  A dialog is always a top-level widget, but if it has a parent, its
  default location is centered on top of the parent. It will also
  share the parent's taskbar entry.

  The widget flags \a f are passed on to the TQWidget constructor.
  If, for example, you don't want a What's This button in the titlebar
  of the dialog, pass WStyle_Customize | WStyle_NormalBorder |
  WStyle_Title | WStyle_SysMenu in \a f.

  \warning In TQt 3.2, the \a modal flag is obsolete. There is now a
  setModal() function that can be used for obtaining a modal behavior
  when calling show(). This is rarely needed, because modal dialogs
  are usually invoked using exec(), which ignores the \a modal flag.

  \sa TQWidget::setWFlags() TQt::WidgetFlags
*/

TQDialog::TQDialog( TQWidget *parent, const char *name, bool modal, WFlags f )
    : TQWidget( parent, name,
	       (modal ? (f|WShowModal) : f) | WType_Dialog ),
      rescode(0), did_move(0), has_relpos(0), did_resize(0), in_loop(0)
{
    d = new TQDialogPrivate;
}

/*!
  Destroys the TQDialog, deleting all its children.
*/

TQDialog::~TQDialog()
{
    // Need to hide() here, as our (to-be) overridden hide()
    // will not be called in ~TQWidget.
    hide();
    delete d;
}

/*!
  \internal
  This function is called by the push button \a pushButton when it
  becomes the default button. If \a pushButton is 0, the dialogs
  default default button becomes the default button. This is what a
  push button calls when it loses focus.
*/

void TQDialog::setDefault( TQPushButton *pushButton )
{
#ifndef TQT_NO_PUSHBUTTON
    TQObjectList *list = queryList( "TQPushButton" );
    Q_ASSERT(list);
    TQObjectListIt it( *list );
    TQPushButton *pb;
    bool hasMain = false;
    while ( (pb = (TQPushButton*)it.current()) ) {
	++it;
	if ( pb->topLevelWidget() != this )
	    continue;
	if ( pb == d->mainDef )
	    hasMain = true;
	if ( pb != pushButton )
	    pb->setDefault( false );
    }
    if (!pushButton && hasMain)
	d->mainDef->setDefault( true );
    if (!hasMain)
	d->mainDef = pushButton;
    delete list;
#endif
}

/*!
  \internal
  This function sets the default default pushbutton to \a pushButton.
  This function is called by TQPushButton::setDefault().
*/
void TQDialog::setMainDefault( TQPushButton *pushButton )
{
#ifndef TQT_NO_PUSHBUTTON
    d->mainDef = 0;
    setDefault(pushButton);
#endif
}

/*!
  \internal
  Hides the default button indicator. Called when non auto-default
  push button get focus.
 */
void TQDialog::hideDefault()
{
#ifndef TQT_NO_PUSHBUTTON
    TQObjectList *list = queryList( "TQPushButton" );
    TQObjectListIt it( *list );
    TQPushButton *pb;
    while ( (pb = (TQPushButton*)it.current()) ) {
	++it;
	pb->setDefault( false );
    }
    delete list;
#endif
}

#ifdef Q_OS_TEMP
/*!
  \internal
  Hides special buttons which are rather shown in the titlebar
  on WinCE, to conserve screen space.
*/
# include "ntqmessagebox.h"
extern const char * mb_texts[]; // Defined in qmessagebox.cpp
void TQDialog::hideSpecial()
{
    // "OK"     buttons are hidden, and (Ok) shown on titlebar
    // "Cancel" buttons are hidden, and (X)  shown on titlebar
    // "Help"   buttons are hidden, and (?)  shown on titlebar
    bool showOK = false,
	 showX  = false,
	 showQ  = false;
    TQObjectList *list = queryList( "TQPushButton" );
    TQObjectListIt it( *list );
    TQPushButton *pb;
    while ( (pb = (TQPushButton*)it.current()) ) {
	if ( !showOK &&
	     pb->text() == tqApp->translate( "TQMessageBox", mb_texts[TQMessageBox::Ok] ) ) {
	    pb->hide();
	    showOK = true;
	} else if ( !showX &&
		    pb->text() == tqApp->translate( "TQMessageBox", mb_texts[TQMessageBox::Cancel] ) ) {
	    pb->hide();
	    showX = true;
	} else if ( !showQ &&
		    pb->text() == tqApp->tr("Help") ) {
	    pb->hide();
	    showQ = true;
	}
        ++it;
    }
    delete list;
    if ( showOK || showQ ) {
	DWORD ext = GetWindowLong( winId(), GWL_EXSTYLE );
	ext |= showOK ? WS_EX_CAPTIONOKBTN : 0;
	ext |= showQ  ? WS_EX_CONTEXTHELP: 0;
	SetWindowLong( winId(), GWL_EXSTYLE, ext );
    }
    if ( !showX ) {
	DWORD ext = GetWindowLong( winId(), GWL_STYLE );
	ext &= ~WS_SYSMENU;
	SetWindowLong( winId(), GWL_STYLE, ext );
    }
}
#endif

/*!
  \fn int TQDialog::result() const

  Returns the modal dialog's result code, \c Accepted or \c Rejected.

  Do not call this function if the dialog was constructed with the \c
  WDestructiveClose flag.
*/

/*!
  \fn void TQDialog::setResult( int i )

  Sets the modal dialog's result code to \a i.
*/


/*!
    Shows the dialog as a \link #modal modal \endlink dialog,
    blocking until the user closes it. The function returns a \l
    DialogCode result.

    Users cannot interact with any other window in the same
    application until they close the dialog.

  \sa show(), result()
*/

int TQDialog::exec()
{
    if ( in_loop ) {
	tqWarning( "TQDialog::exec: Recursive call detected" );
	return -1;
    }

    bool destructiveClose = testWFlags( WDestructiveClose );
    clearWFlags( WDestructiveClose );

    bool wasShowModal = testWFlags( WShowModal );
    setWFlags( WShowModal );
    setResult( 0 );

    show();

    in_loop = true;
    tqApp->enter_loop();

    if ( !wasShowModal )
	clearWFlags( WShowModal );

    int res = result();

    if ( destructiveClose )
	delete this;

    return res;
}


/*! Closes the dialog and sets its result code to \a r. If this dialog
  is shown with exec(), done() causes the local event loop to finish,
  and exec() to return \a r.

  As with TQWidget::close(), done() deletes the dialog if the \c
  WDestructiveClose flag is set. If the dialog is the application's
  main widget, the application terminates. If the dialog is the
  last window closed, the TQApplication::lastWindowClosed() signal is
  emitted.

  \sa accept(), reject(), TQApplication::mainWidget(), TQApplication::quit()
*/

void TQDialog::done( int r )
{
    hide();
    setResult( r );

    // emulate TQWidget::close()
    bool isMain = tqApp->mainWidget() == this;
    bool checkLastWindowClosed = isTopLevel() && !isPopup();
    if ( checkLastWindowClosed
	 && tqApp->receivers(TQ_SIGNAL(lastWindowClosed())) ) {
	/* if there is no non-withdrawn top level window left (except
	   the desktop, popups, or dialogs with parents), we emit the
	   lastWindowClosed signal */
	TQWidgetList *list   = tqApp->topLevelWidgets();
	TQWidget     *widget = list->first();
	while ( widget ) {
	    if ( !widget->isHidden()
		 && !widget->isDesktop()
		 && !widget->isPopup()
		 && (!widget->isDialog() || !widget->parentWidget()))
		break;
	    widget = list->next();
	}
	delete list;
	if ( widget == 0 )
	    emit tqApp->lastWindowClosed();
    }
    if ( isMain )
	tqApp->quit();
    else if ( testWFlags(WDestructiveClose) ) {
	clearWFlags(WDestructiveClose);
	deleteLater();
    }
}

/*!
  Hides the modal dialog and sets the result code to \c Accepted.

  \sa reject() done()
*/

void TQDialog::accept()
{
    done( Accepted );
}

/*!
  Hides the modal dialog and sets the result code to \c Rejected.

  \sa accept() done()
*/

void TQDialog::reject()
{
    done( Rejected );
}

/*! \reimp */
bool TQDialog::eventFilter( TQObject *o, TQEvent *e )
{
    return TQWidget::eventFilter( o, e );
}

/*****************************************************************************
  Event handlers
 *****************************************************************************/

/*! \reimp */
void TQDialog::contextMenuEvent( TQContextMenuEvent *e )
{
#if !defined(TQT_NO_WHATSTHIS) && !defined(TQT_NO_POPUPMENU)
    TQWidget* w = childAt( e->pos(), true );
    if ( !w )
	return;
    TQString s;
    while ( s.isEmpty() && w ) {
	s = TQWhatsThis::textFor( w, e->pos(), false );
	if ( s.isEmpty() )
	    w = w->parentWidget(true);
    }
    if ( !s.isEmpty() ) {
	TQPopupMenu p(0,"qt_whats_this_menu");
	p.insertItem( tr("What's This?"), 42 );
	if ( p.exec( e->globalPos() ) >= 42 )
	    TQWhatsThis::display( s, w->mapToGlobal( w->rect().center() ), w );
    }
#endif
}

/*! \reimp */
void TQDialog::keyPressEvent( TQKeyEvent *e )
{
    //   Calls reject() if Escape is pressed. Simulates a button
    //   click for the default button if Enter is pressed. Move focus
    //   for the arrow keys. Ignore the rest.
#ifdef Q_OS_MAC
    if(e->state() == ControlButton && e->key() == Key_Period) {
	reject();
    } else
#endif
    if ( e->state() == 0 || ( e->state() & Keypad && e->key() == Key_Enter ) ) {
	switch ( e->key() ) {
	case Key_Enter:
	case Key_Return: {
#ifndef TQT_NO_PUSHBUTTON
	    TQObjectList *list = queryList( "TQPushButton" );
	    TQObjectListIt it( *list );
	    TQPushButton *pb;
	    while ( (pb = (TQPushButton*)it.current()) ) {
		if ( pb->isDefault() && pb->isVisible() ) {
		    delete list;
		    if ( pb->isEnabled() ) {
			emit pb->clicked();
		    }
		    return;
		}
		++it;
	    }
	    delete list;
#endif
	}
	break;
	case Key_Escape:
	    reject();
	    break;
	case Key_Up:
	case Key_Left:
	    if ( focusWidget() &&
		 ( focusWidget()->focusPolicy() == TQWidget::StrongFocus ||
		   focusWidget()->focusPolicy() == TQWidget::WheelFocus ) ) {
		e->ignore();
		break;
	    }
	    // call ours, since c++ blocks us from calling the one
	    // belonging to focusWidget().
	    TQFocusEvent::setReason(TQFocusEvent::Backtab);
	    focusNextPrevChild( false );
	    TQFocusEvent::resetReason();
	    break;
	case Key_Down:
	case Key_Right:
	    if ( focusWidget() &&
		 ( focusWidget()->focusPolicy() == TQWidget::StrongFocus ||
		   focusWidget()->focusPolicy() == TQWidget::WheelFocus ) ) {
		e->ignore();
		break;
	    }
	    TQFocusEvent::setReason(TQFocusEvent::Tab);
	    focusNextPrevChild( true );
	    TQFocusEvent::resetReason();
	    break;
	default:
	    e->ignore();
	    return;
	}
    } else {
	e->ignore();
    }
}

/*! \reimp */
void TQDialog::closeEvent( TQCloseEvent *e )
{
#ifndef TQT_NO_WHATSTHIS
    if ( isModal() && TQWhatsThis::inWhatsThisMode() )
	TQWhatsThis::leaveWhatsThisMode();
#endif
    if ( isShown() )
	reject();
    if ( isHidden() )
	e->accept();
}

#ifdef Q_OS_TEMP
/*! \internal
    \reimp
*/
bool TQDialog::event( TQEvent *e )
{
    switch ( e->type() ) {
    case TQEvent::OkRequest:
    case TQEvent::HelpRequest:
	{
	    TQString bName =
		(e->type() == TQEvent::OkRequest)
		? tqApp->translate( "TQMessageBox", mb_texts[TQMessageBox::Ok] )
		: tqApp->tr( "Help" );

	    TQObjectList *list = queryList( "TQPushButton" );
	    TQObjectListIt it( *list );
	    TQPushButton *pb;
	    while ( (pb = (TQPushButton*)it.current()) ) {
		if ( pb->text() == bName ) {
		    delete list;
		    if ( pb->isEnabled() )
			emit pb->clicked();
		    return pb->isEnabled();
		}
		++it;
	    }
	    delete list;
	}
    }
    return TQWidget::event( e );
}
#endif


/*****************************************************************************
  Geometry management.
 *****************************************************************************/

#if defined(TQ_WS_X11)
extern "C" { int XSetTransientForHint( Display *, unsigned long, unsigned long ); }
#include <private/qt_x11_p.h>
#undef FocusIn
// defined in qapplication_x11.cpp
extern Atom tqt_net_wm_full_placement;
extern bool tqt_net_supports(Atom atom);
#endif // TQ_WS_X11

/*!
    Shows the dialog as a \link #modeless modeless \endlink dialog.
    Control returns immediately to the calling code.

    The dialog will be modal or modeless according to the value
    of the \l modal property.

    \sa exec(), modal
*/

void TQDialog::show()
{
    if ( testWState(WState_Visible) )
	return;

    uint state = windowState();

    if ( !did_resize )
	adjustSize();
    if( !tqt_net_supports( tqt_net_wm_full_placement )) {
	if ( has_relpos && !did_move ) {
	    adjustPositionInternal( parentWidget(), true );
	} else if ( !did_move ) {
	    adjustPositionInternal( parentWidget() );
	}
    }

    if (windowState() != state)
	setWindowState(state);

#if defined(TQ_WS_X11)
    if (!parentWidget() && testWFlags(WShowModal)
	&& tqApp->mainWidget() && tqApp->mainWidget()->isVisible()
	&& !tqApp->mainWidget()->isMinimized()) {
	// make sure the transient for hint is set properly for modal dialogs
        x11SetWindowTransient( tqApp->mainWidget());
    }
#endif // TQ_WS_X11

#ifdef Q_OS_TEMP
    hideSpecial();
#endif

    TQWidget::show();
    showExtension( d->doShowExtension );
#ifndef TQT_NO_PUSHBUTTON
    TQWidget *fw = focusWidget();
    TQFocusData *fd = focusData();

    /*
      The following block is to handle a special case, and does not
      really follow propper logic in concern of autoDefault and TAB
      order. However, it's here to ease usage for the users. If a
      dialog has a default TQPushButton, and first widget in the TAB
      order also is a TQPushButton, then we give focus to the main
      default TQPushButton. This simplifies code for the developers,
      and actually catches most cases... If not, then they simply
      have to use [widget*]->setFocus() themselves...
    */
    if ( !fw || fw->focusPolicy() == NoFocus ) {
	fd->home(); // Skip main form
	TQWidget *first = fd->next(); // Get first main widget
	if ( d->mainDef &&
	     first != d->mainDef &&
	     ::tqt_cast<TQPushButton*>(first) )
	    d->mainDef->setFocus();
    }

    if ( !d->mainDef && isTopLevel() ) {
	if ( !fw || fw->focusPolicy() == NoFocus ) {
	    focusNextPrevChild( true );
	    fw = focusWidget();
	}
	if ( fw ) {
	    fd = focusData();
	    TQWidget *home = fd->home();
	    TQWidget *candidate = home;
	    Q_ASSERT( candidate == fw );
	    do {
		TQPushButton *pb = ::tqt_cast<TQPushButton*>(candidate);
		if ( pb && pb->autoDefault() ) {
		    pb->setDefault( true );
		    break;
		}
		candidate = fd->next();
	    } while ( candidate != home );
	}
    }
    if ( fw ) {
	TQFocusEvent e( TQEvent::FocusIn );
	TQFocusEvent::setReason( TQFocusEvent::Tab );
	TQApplication::sendEvent( fw, &e );
	TQFocusEvent::resetReason();
    }

#endif
#if defined(QT_ACCESSIBILITY_SUPPORT)
    TQAccessible::updateAccessibility( this, 0, TQAccessible::DialogStart );
#endif
}

/*! \internal */
void TQDialog::adjustPosition( TQWidget* w)
{
    adjustPositionInternal( w );
}


void TQDialog::adjustPositionInternal( TQWidget*w, bool useRelPos)
{
    /* need to make sure these events are already sent to be sure
       our information below is correct --sam */
    TQApplication::sendPostedEvents( this, TQEvent::LayoutHint );
    TQApplication::sendPostedEvents( this, TQEvent::Resize );

    // processing the events might call polish(), which is a nice place
    // to restore geometries, so return if the dialog has been positioned
    if ( did_move || has_relpos )
	return;

    TQPoint p( 0, 0 );
    int extraw = 0, extrah = 0, scrn = 0;
    if ( w )
	w = w->topLevelWidget();
    TQRect desk;
    if ( w ) {
	// Use mapToGlobal rather than geometry() in case w might
	// be embedded in another application
	scrn = TQApplication::desktop()->screenNumber( w->mapToGlobal( TQPoint(0,0) ) );
    } else if ( TQApplication::desktop()->isVirtualDesktop() ) {
	scrn = TQApplication::desktop()->screenNumber( TQCursor::pos() );
    } else {
	scrn = TQApplication::desktop()->screenNumber( this );
    }
    desk = TQApplication::desktop()->availableGeometry( scrn );

    TQWidgetList  *list = TQApplication::topLevelWidgets();
    TQWidgetListIt it( *list );
    while ( (extraw == 0 || extrah == 0) &&
	    it.current() != 0 ) {
	int framew, frameh;
	TQWidget * current = it.current();
	++it;
	if ( ! current->isVisible() )
	    continue;

	framew = current->geometry().x() - current->x();
	frameh = current->geometry().y() - current->y();

	extraw = TQMAX( extraw, framew );
	extrah = TQMAX( extrah, frameh );
    }
    delete list;

    // sanity check for decoration frames. With embedding, we
    // might get extraordinary values
    if ( extraw == 0 || extrah == 0 || extraw >= 10 || extrah >= 40 ) {
	extrah = 40;
	extraw = 10;
    }

    if ( useRelPos && w ) {
	p = w->pos() + d->relPos;
    } else {
#ifndef Q_OS_TEMP
	if ( w ) {
	    // Use mapToGlobal rather than geometry() in case w might
	    // be embedded in another application
	    TQPoint pp = w->mapToGlobal( TQPoint(0,0) );
	    p = TQPoint( pp.x() + w->width()/2,
			pp.y() + w->height()/ 2 );
	} else {
	    // p = middle of the desktop
	    p = TQPoint( desk.x() + desk.width()/2, desk.y() + desk.height()/2 );
	}
#else
	p = TQPoint( desk.x() + desk.width()/2, desk.y() + desk.height()/2 );
#endif

	// p = origin of this
	p = TQPoint( p.x()-width()/2 - extraw,
		    p.y()-height()/2 - extrah );
    }


    if ( p.x() + extraw + width() > desk.x() + desk.width() )
	p.setX( desk.x() + desk.width() - width() - extraw );
    if ( p.x() < desk.x() )
	p.setX( desk.x() );

    if ( p.y() + extrah + height() > desk.y() + desk.height() )
	p.setY( desk.y() + desk.height() - height() - extrah );
    if ( p.y() < desk.y() )
	p.setY( desk.y() );

    move( p );
    did_move = !useRelPos;
}


/*! \reimp */
void TQDialog::hide()
{
    if ( isHidden() )
	return;

#if defined(QT_ACCESSIBILITY_SUPPORT)
    if ( isVisible() )
	TQAccessible::updateAccessibility( this, 0, TQAccessible::DialogEnd );
#endif

    if ( parentWidget() && !did_move ) {
	d->relPos = pos() - parentWidget()->topLevelWidget()->pos();
	has_relpos = 1;
    }

    // Reimplemented to exit a modal when the dialog is hidden.
    TQWidget::hide();
    if ( in_loop ) {
	in_loop = false;
	tqApp->exit_loop();
    }
}


/*****************************************************************************
  Detects any widget geometry changes done by the user.
 *****************************************************************************/

/*! \reimp */

void TQDialog::move( int x, int y )
{
    did_move = true;
    TQWidget::move( x, y );
}

/*! \reimp */

void TQDialog::move( const TQPoint &p )
{
    did_move = true;
    TQWidget::move( p );
}

/*! \reimp */

void TQDialog::resize( int w, int h )
{
    did_resize = true;
    TQWidget::resize( w, h );
}

/*! \reimp */

void TQDialog::resize( const TQSize &s )
{
    did_resize = true;
    TQWidget::resize( s );
}

/*! \reimp */

void TQDialog::setGeometry( int x, int y, int w, int h )
{
    did_move   = true;
    did_resize = true;
    TQWidget::setGeometry( x, y, w, h );
}

/*! \reimp */

void TQDialog::setGeometry( const TQRect &r )
{
    did_move   = true;
    did_resize = true;
    TQWidget::setGeometry( r );
}


/*!
    If \a orientation is \c Horizontal, the extension will be displayed
    to the right of the dialog's main area. If \a orientation is \c
    Vertical, the extension will be displayed below the dialog's main
    area.

  \sa orientation(), setExtension()
*/
void TQDialog::setOrientation( Orientation orientation )
{
    d->orientation = orientation;
}

/*!
  Returns the dialog's extension orientation.

  \sa setOrientation()
*/
TQt::Orientation TQDialog::orientation() const
{
    return d->orientation;
}

/*!
    Sets the widget, \a extension, to be the dialog's extension,
    deleting any previous extension. The dialog takes ownership of the
    extension. Note that if 0 is passed any existing extension will be
    deleted.

  This function must only be called while the dialog is hidden.

  \sa showExtension(), setOrientation(), extension()
 */
void TQDialog::setExtension( TQWidget* extension )
{
    delete d->extension;
    d->extension = extension;

    if ( !extension )
	return;

    if ( extension->parentWidget() != this )
	extension->reparent( this, TQPoint(0,0) );
    extension->hide();
}

/*!
  Returns the dialog's extension or 0 if no extension has been
  defined.

  \sa setExtension()
 */
TQWidget* TQDialog::extension() const
{
    return d->extension;
}


/*!
  If \a showIt is true, the dialog's extension is shown; otherwise the
  extension is hidden.

  This slot is usually connected to the \l TQButton::toggled() signal
  of a TQPushButton.

  A dialog with a visible extension is not resizeable.

  \sa show(), setExtension(), setOrientation()
 */
void TQDialog::showExtension( bool showIt )
{
    d->doShowExtension = showIt;
    if ( !d->extension )
	return;
    if ( !testWState(WState_Visible) )
	return;
    if ( d->extension->isVisible() == showIt )
	return;

    if ( showIt ) {
	d->size = size();
	d->min = minimumSize();
	d->max = maximumSize();
#ifndef TQT_NO_LAYOUT
	if ( layout() )
	    layout()->setEnabled( false );
#endif
	TQSize s( d->extension->sizeHint()
		 .expandedTo( d->extension->minimumSize() )
		 .boundedTo( d->extension->maximumSize() ) );
	if ( d->orientation == Horizontal ) {
	    int h = TQMAX( height(), s.height() );
	    d->extension->setGeometry( width(), 0, s.width(), h );
	    setFixedSize( width() + s.width(), h );
	} else {
	    int w = TQMAX( width(), s.width() );
	    d->extension->setGeometry( 0, height(), w, s.height() );
	    setFixedSize( w, height() + s.height() );
	}
	d->extension->show();
    } else {
	d->extension->hide();
	// workaround for CDE window manager that won't shrink with (-1,-1)
	setMinimumSize( d->min.expandedTo( TQSize( 1, 1 ) ) );
	setMaximumSize( d->max );
	resize( d->size );
#ifndef TQT_NO_LAYOUT
	if ( layout() )
	    layout()->setEnabled( true );
#endif
    }
}


/*! \reimp */
TQSize TQDialog::sizeHint() const
{
    if ( d->extension ) {
	if ( d->orientation == Horizontal )
	    return TQSize( TQWidget::sizeHint().width(),
			TQMAX( TQWidget::sizeHint().height(),d->extension->sizeHint().height() ) );
	else
	    return TQSize( TQMAX( TQWidget::sizeHint().width(), d->extension->sizeHint().width() ),
			TQWidget::sizeHint().height() );
	}

    return TQWidget::sizeHint();
}


/*! \reimp */
TQSize TQDialog::minimumSizeHint() const
{
    if ( d->extension ) {
	if (d->orientation == Horizontal )
	    return TQSize( TQWidget::minimumSizeHint().width(),
			TQMAX( TQWidget::minimumSizeHint().height(), d->extension->minimumSizeHint().height() ) );
	else
	    return TQSize( TQMAX( TQWidget::minimumSizeHint().width(), d->extension->minimumSizeHint().width() ),
			TQWidget::minimumSizeHint().height() );
	}

    return TQWidget::minimumSizeHint();
}

/*! \property TQDialog::modal
    \brief whether show() should pop up the dialog as modal or modeless

    By default, this property is false and show() pops up the dialog as
    modeless.

    exec() ignores the value of this property and always pops up the
    dialog as modal.

    \sa show(), exec()
*/

void TQDialog::setModal( bool modal )
{
    if ( modal )
	setWFlags( WShowModal );
    else
	clearWFlags( WShowModal );
}

bool TQDialog::isModal() const
{
    return testWFlags( WShowModal ) != 0;
}

bool TQDialog::isSizeGripEnabled() const
{
#ifndef TQT_NO_SIZEGRIP
    return !!d->resizer;
#else
    return false;
#endif
}


void TQDialog::setSizeGripEnabled(bool enabled)
{
#ifndef TQT_NO_SIZEGRIP
    if ( !enabled != !d->resizer ) {
	if ( enabled ) {
	    d->resizer = new TQSizeGrip( this, "TQDialog::resizer" );
	    // adjustSize() processes all events, which is suboptimal
	    d->resizer->resize( d->resizer->sizeHint() );
	    if ( TQApplication::reverseLayout() )
		d->resizer->move( rect().bottomLeft() -d->resizer->rect().bottomLeft() );
	    else
		d->resizer->move( rect().bottomRight() -d->resizer->rect().bottomRight() );
	    d->resizer->raise();
	    d->resizer->show();
	} else {
	    delete d->resizer;
	    d->resizer = 0;
	}
    }
#endif //TQT_NO_SIZEGRIP
}



/*! \reimp */
void TQDialog::resizeEvent( TQResizeEvent * )
{
#ifndef TQT_NO_SIZEGRIP
    if ( d->resizer ) {
	if ( TQApplication::reverseLayout() )
	    d->resizer->move( rect().bottomLeft() -d->resizer->rect().bottomLeft() );
	else
	    d->resizer->move( rect().bottomRight() -d->resizer->rect().bottomRight() );
    }
#endif
}

#endif // TQT_NO_DIALOG
