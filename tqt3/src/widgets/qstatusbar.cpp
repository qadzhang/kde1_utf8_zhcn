/****************************************************************************
**
** Implementation of TQStatusBar class
**
** Created : 980119
**
** Copyright (C) 1992-2008 Trolltech ASA.  All rights reserved.
**
** This file is part of the widgets module of the TQt GUI Toolkit.
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

#include "ntqstatusbar.h"
#ifndef TQT_NO_STATUSBAR

#include "ntqptrlist.h"
#include "ntqlayout.h"
#include "ntqpainter.h"
#include "ntqtimer.h"
#include "ntqdrawutil.h"
#include "ntqstyle.h"
#include "ntqsizegrip.h"

/*!
    \class TQStatusBar ntqstatusbar.h
    \brief The TQStatusBar class provides a horizontal bar suitable for
    presenting status information.

    \ingroup application
    \ingroup helpsystem
    \mainclass

    Each status indicator falls into one of three categories:

    \list
    \i \e Temporary - briefly occupies most of the status bar. Used
	to explain tool tip texts or menu entries, for example.
    \i \e Normal - occupies part of the status bar and may be hidden
	by temporary messages. Used to display the page and line
	number in a word processor, for example.
    \i \e Permanent - is never hidden. Used for important mode
	indications, for example, some applications put a Caps Lock
	indicator in the status bar.
    \endlist

    TQStatusBar lets you display all three types of indicators.

    To display a \e temporary message, call message() (perhaps by
    connecting a suitable signal to it). To remove a temporary
    message, call clear(). There are two variants of message(): one
    that displays the message until the next clear() or message() and
    one that has a time limit:

    \code
	connect( loader, TQ_SIGNAL(progressMessage(const TQString&)),
		 statusBar(), TQ_SLOT(message(const TQString&)) );

	statusBar()->message("Loading...");  // Initial message
	loader.loadStuff();                  // Emits progress messages
	statusBar()->message("Done.", 2000); // Final message for 2 seconds
    \endcode

    \e Normal and \e Permanent messages are displayed by creating a
    small widget and then adding it to the status bar with
    addWidget(). Widgets like TQLabel, TQProgressBar or even TQToolButton
    are useful for adding to status bars. removeWidget() is used to
    remove widgets.

    \code
	statusBar()->addWidget(new MyReadWriteIndication(statusBar()));
    \endcode

    By default TQStatusBar provides a TQSizeGrip in the lower-right
    corner. You can disable it with setSizeGripEnabled(false);

    <img src=qstatusbar-m.png> <img src=qstatusbar-w.png>

    \sa TQToolBar TQMainWindow TQLabel
    \link guibooks.html#fowler GUI Design Handbook: Status Bar.\endlink
*/


class TQStatusBarPrivate
{
public:
    TQStatusBarPrivate() {}

    struct SBItem {
	SBItem( TQWidget* widget, int stretch, bool permanent )
	    : s( stretch ), w( widget ), p( permanent ) {}
	int s;
	TQWidget * w;
	bool p;
    };

    TQPtrList<SBItem> items;
    TQString tempItem;

    TQBoxLayout * box;
    TQTimer * timer;

#ifndef TQT_NO_SIZEGRIP
    TQSizeGrip * resizer;
#endif

    int savedStrut;
};


/*!
    Constructs a status bar called \a name with parent \a parent and
    with a size grip.

    \sa setSizeGripEnabled()
*/
TQStatusBar::TQStatusBar( TQWidget * parent, const char *name )
    : TQWidget( parent, name )
{
    d = new TQStatusBarPrivate;
    d->items.setAutoDelete( true );
    d->box = 0;
    d->timer = 0;

#ifndef TQT_NO_SIZEGRIP
    d->resizer = 0;
    setSizeGripEnabled(true); // causes reformat()
#else
    reformat();
#endif
}


/*!
    Destroys the status bar and frees any allocated resources and
    child widgets.
*/
TQStatusBar::~TQStatusBar()
{
    delete d;
    d = 0;
}


/*!
    Adds \a widget to this status bar. \a widget is reparented if it
    isn't already a child of the TQStatusBar.

    \a widget is permanently visible if \a permanent is true and may
    be obscured by temporary messages if \a permanent is false. The
    default is false.

    If \a permanent is true, \a widget is located at the far right of
    the status bar. If \a permanent is false (the default), \a widget
    is located just to the left of the first permanent widget.

    \a stretch is used to compute a suitable size for \a widget as the
    status bar grows and shrinks. The default of 0 uses a minimum of
    space.

    This function may cause some flicker.

    \sa removeWidget()
*/

void TQStatusBar::addWidget( TQWidget * widget, int stretch, bool permanent )
{
    if ( !widget ) {
#if defined(QT_CHECK_NULL)
	tqWarning( "TQStatusBar::addWidget(): Cannot add null widget" );
#endif
	return;
    }

    if ( widget->parentWidget() != this )
	widget->reparent( this, TQPoint(0, 0), true );

    TQStatusBarPrivate::SBItem* item
	= new TQStatusBarPrivate::SBItem( widget, stretch, permanent );

    d->items.last();
    while( !permanent && d->items.current() && d->items.current()->p )
	d->items.prev();

    d->items.insert( d->items.at() >= 0 ? d->items.at()+1 : 0, item );

    if ( !d->tempItem.isEmpty() && !permanent )
	widget->hide();

    reformat();
}


/*!
    Removes \a widget from the status bar.

    This function may cause some flicker.

    Note that \a widget is not deleted.

    \sa addWidget()
*/

void TQStatusBar::removeWidget( TQWidget* widget )
{
    if ( !widget )
	return;
    bool found = false;
    TQStatusBarPrivate::SBItem* item = d->items.first();
    while ( item && !found ) {
	if ( item->w == widget ) {
	    d->items.remove();
	    found = true;
	}
	item = d->items.next();
    }

    if ( found )
	reformat();
#if defined(QT_DEBUG)
    else
	tqDebug( "TQStatusBar::removeWidget(): Widget not found." );
#endif
}

/*!
    \property TQStatusBar::sizeGripEnabled
    \brief whether the TQSizeGrip in the bottom right of the status bar is enabled

    Enables or disables the TQSizeGrip in the bottom right of the
    status bar. By default, the size grip is enabled.
*/

bool TQStatusBar::isSizeGripEnabled() const
{
#ifdef TQT_NO_SIZEGRIP
    return false;
#else
    return !!d->resizer;
#endif
}

void TQStatusBar::setSizeGripEnabled(bool enabled)
{
#ifndef TQT_NO_SIZEGRIP
    if ( !enabled != !d->resizer ) {
	if ( enabled ) {
	    d->resizer = new TQSizeGrip( this, "TQStatusBar::resizer" );
	} else {
	    delete d->resizer;
	    d->resizer = 0;
	}
	reformat();
	if ( d->resizer && isVisible() )
	    d->resizer->show();
    }
#endif
}


/*!
    Changes the status bar's appearance to account for item changes.
    Special subclasses may need this, but geometry management will
    usually take care of any necessary rearrangements.
*/
void TQStatusBar::reformat()
{
    if ( d->box )
	delete d->box;

    TQBoxLayout *vbox;
    if ( isSizeGripEnabled() ) {
	d->box = new TQHBoxLayout( this );
	vbox = new TQVBoxLayout( d->box );
    } else {
	vbox = d->box = new TQVBoxLayout( this );
    }
    vbox->addSpacing( 3 );
    TQBoxLayout* l = new TQHBoxLayout( vbox );
    l->addSpacing( 3 );
    l->setSpacing( 4 );

    int maxH = fontMetrics().height();

    TQStatusBarPrivate::SBItem* item = d->items.first();
    while ( item && !item->p ) {
	l->addWidget( item->w, item->s );
	int itemH = TQMIN(item->w->sizeHint().height(),
			 item->w->maximumHeight());
	maxH = TQMAX( maxH, itemH );
	item = d->items.next();
    }

    l->addStretch( 0 );

    while ( item ) {
	l->addWidget( item->w, item->s );
	int itemH = TQMIN(item->w->sizeHint().height(),
			 item->w->maximumHeight());
	maxH = TQMAX( maxH, itemH );
	item = d->items.next();
    }
    l->addSpacing( 4 );
#ifndef TQT_NO_SIZEGRIP
    if ( d->resizer ) {
	maxH = TQMAX( maxH, d->resizer->sizeHint().height() );
	d->box->addSpacing( 1 );
	d->box->addWidget( d->resizer, 0, AlignBottom );
    }
#endif
    l->addStrut( maxH );
    d->savedStrut = maxH;
    vbox->addSpacing( 2 );
    d->box->activate();
    repaint();
}




/*!
    Hides the normal status indicators and displays \a message until
    clear() or another message() is called.

    \sa clear()
*/
void TQStatusBar::message( const TQString &message )
{
    if ( d->tempItem == message )
	return;
    d->tempItem = message;
    if ( d->timer ) {
	delete d->timer;
	d->timer = 0;
    }
    hideOrShow();
}


/*!
    \overload

    Hides the normal status indications and displays \a message for \a
    ms milli-seconds or until clear() or another message() is called,
    whichever occurs first.
*/
void TQStatusBar::message( const TQString &message, int ms )
{
    d->tempItem = message;

    if ( !d->timer ) {
	d->timer = new TQTimer( this );
	connect( d->timer, TQ_SIGNAL(timeout()), this, TQ_SLOT(clear()) );
    }
    if ( ms > 0 ) {
	d->timer->start( ms );
    } else if ( d->timer ) {
	delete d->timer;
	d->timer = 0;
    }

    hideOrShow();
}


/*!
    Removes any temporary message being shown.

    \sa message()
*/

void TQStatusBar::clear()
{
    if ( d->tempItem.isEmpty() )
	return;
    if ( d->timer ) {
	delete d->timer;
	d->timer = 0;
    }
    d->tempItem = TQString::null;
    hideOrShow();
}

/*!
    \fn TQStatusBar::messageChanged( const TQString &message )

    This signal is emitted when the temporary status messages
    changes. \a message is the new temporary message, and is a
    null-string when the message has been removed.

    \sa message(), clear()
*/

/*!
    Ensures that the right widgets are visible. Used by message() and
    clear().
*/
void TQStatusBar::hideOrShow()
{
    bool haveMessage = !d->tempItem.isEmpty();

    TQStatusBarPrivate::SBItem* item = d->items.first();

    while( item && !item->p ) {
	if ( haveMessage )
	    item->w->hide();
	else
	    item->w->show();
	item = d->items.next();
    }

    emit messageChanged( d->tempItem );
    repaint();
}


/*!
    Shows the temporary message, if appropriate.
*/
void TQStatusBar::paintEvent( TQPaintEvent * )
{
    bool haveMessage = !d->tempItem.isEmpty();

    TQPainter p( this );
    TQStatusBarPrivate::SBItem* item = d->items.first();

#ifndef TQT_NO_SIZEGRIP
    int psx = ( d->resizer && d->resizer->isVisible() ) ? d->resizer->x() : width()-12;
#else
    int psx = width() - 12;
#endif

    while ( item ) {
	if ( !haveMessage || item->p )
	    if ( item->w->isVisible() ) {
		if ( item->p && item->w->x()-1 < psx )
		    psx = item->w->x()-1;
		style().drawPrimitive( TQStyle::PE_StatusBarSection, &p,
				       TQRect(item->w->x() - 1, item->w->y() - 1,
					     item->w->width()+2, item->w->height()+2),
				       colorGroup(), TQStyle::Style_Default,
				       TQStyleOption(item->w) );
	    }
	item = d->items.next();
    }
    if ( haveMessage ) {
	p.setPen( colorGroup().foreground() );
	p.drawText( 6, 0, psx, height(), AlignVCenter | SingleLine, d->tempItem );
    }
}

/*!
    \reimp
*/
void TQStatusBar::resizeEvent( TQResizeEvent * e )
{
    TQWidget::resizeEvent( e );
}

/*!
    \reimp
*/

bool TQStatusBar::event( TQEvent *e )
{
    if ( e->type() == TQEvent::LayoutHint ) {
	// Calculate new strut height and call reformat() if it has changed
	int maxH = fontMetrics().height();

	TQStatusBarPrivate::SBItem* item = d->items.first();
	while ( item ) {
	    int itemH = TQMIN(item->w->sizeHint().height(),
			    item->w->maximumHeight());
	    maxH = TQMAX( maxH, itemH );
	    item = d->items.next();
	}

#ifndef TQT_NO_SIZEGRIP
	if ( d->resizer )
	    maxH = TQMAX( maxH, d->resizer->sizeHint().height() );
#endif

	if ( maxH != d->savedStrut )
	    reformat();
	else
	    update();
    }
    if ( e->type() == TQEvent::ChildRemoved ) {
	TQStatusBarPrivate::SBItem* item = d->items.first();
	while ( item ) {
	    if ( item->w == ( (TQChildEvent*)e )->child() )
		d->items.removeRef( item );
	    item = d->items.next();
	}
    }
    return TQWidget::event( e );
}

#endif
