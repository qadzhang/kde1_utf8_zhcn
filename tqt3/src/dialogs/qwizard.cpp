/****************************************************************************
**
** Implementation of TQWizard class.
**
** Created : 990124
**
** Copyright (C) 1999-2008 Trolltech ASA.  All rights reserved.
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

#include "ntqwizard.h"

#ifndef TQT_NO_WIZARD

#include "ntqlayout.h"
#include "ntqpushbutton.h"
#include "ntqcursor.h"
#include "ntqlabel.h"
#include "ntqwidgetstack.h"
#include "ntqapplication.h"
#include "ntqptrlist.h"
#include "ntqpainter.h"
#include "ntqaccel.h"

/*! \file wizard/wizard.cpp */
/*! \file wizard/wizard.h */

/*!
    \class TQWizard ntqwizard.h
    \brief The TQWizard class provides a framework for wizard dialogs.

    \ingroup abstractwidgets
    \ingroup organizers
    \ingroup dialogs
    \mainclass

    A wizard is a special type of input dialog that consists of a
    sequence of dialog pages. A wizard's purpose is to walk the user
    through a process step by step. Wizards are useful for complex or
    infrequently occurring tasks that people may find difficult to
    learn or do.

    TQWizard provides page titles and displays Next, Back, Finish,
    Cancel, and Help push buttons, as appropriate to the current
    position in the page sequence. These buttons can be
    enabled/disabled using setBackEnabled(), setNextEnabled(),
    setFinishEnabled() and setHelpEnabled().

    Create and populate dialog pages that inherit from TQWidget and add
    them to the wizard using addPage(). Use insertPage() to add a
    dialog page at a certain position in the page sequence. Use
    removePage() to remove a page from the page sequence.

    Use currentPage() to retrieve a pointer to the currently displayed
    page. page() returns a pointer to the page at a certain position
    in the page sequence.

    Use pageCount() to retrieve the total number of pages in the page
    sequence. indexOf() will return the index of a page in the page
    sequence.

    TQWizard provides functionality to mark pages as appropriate (or
    not) in the current context with setAppropriate(). The idea is
    that a page may be irrelevant and should be skipped depending on
    the data entered by the user on a preceding page.

    It is generally considered good design to provide a greater number
    of simple pages with fewer choices rather than a smaller number of
    complex pages.

    Example code is available here: \l wizard/wizard.cpp \l wizard/wizard.h

    \img qwizard.png A TQWizard page
    \caption A TQWizard page

*/


class TQWizardPrivate
{
public:
    struct Page {
	Page( TQWidget * widget, const TQString & title ):
	    w( widget ), t( title ),
	    backEnabled( true ), nextEnabled( true ), finishEnabled( false ),
	    helpEnabled( true ),
	    appropriate( true )
	{}
	TQWidget * w;
	TQString t;
	bool backEnabled;
	bool nextEnabled;
	bool finishEnabled;
	bool helpEnabled;
	bool appropriate;
    };

    TQVBoxLayout * v;
    Page * current;
    TQWidgetStack * ws;
    TQPtrList<Page> pages;
    TQLabel * title;
    TQPushButton * backButton;
    TQPushButton * nextButton;
    TQPushButton * finishButton;
    TQPushButton * cancelButton;
    TQPushButton * helpButton;

    TQFrame * hbar1, * hbar2;

#ifndef TQT_NO_ACCEL
    TQAccel * accel;
    int backAccel;
    int nextAccel;
#endif

    Page * page( const TQWidget * w )
    {
	if ( !w )
	    return 0;
	int i = pages.count();
	while( --i >= 0 && pages.at( i ) && pages.at( i )->w != w ) { }
	return i >= 0 ? pages.at( i ) : 0;
    }

};


/*!
    Constructs an empty wizard dialog. The \a parent, \a name, \a
    modal and \a f arguments are passed to the TQDialog constructor.
*/

TQWizard::TQWizard( TQWidget *parent, const char *name, bool modal,
		  WFlags f )
    : TQDialog( parent, name, modal, f )
{
    d = new TQWizardPrivate();
    d->current = 0; // not quite true, but...
    d->ws = new TQWidgetStack( this, "qt_widgetstack" );
    d->pages.setAutoDelete( true );
    d->title = new TQLabel( this, "title label" );

    // create in nice tab order
    d->nextButton = new TQPushButton( this, "next" );
    d->finishButton = new TQPushButton( this, "finish" );
    d->helpButton = new TQPushButton( this, "help" );
    d->backButton = new TQPushButton( this, "back" );
    d->cancelButton = new TQPushButton( this, "cancel" );

    d->ws->installEventFilter( this );

    d->v = 0;
    d->hbar1 = 0;
    d->hbar2 = 0;

    d->cancelButton->setText( tr( "&Cancel" ) );
    d->backButton->setText( tr( "< &Back" ) );
    d->nextButton->setText( tr( "&Next >" ) );
    d->finishButton->setText( tr( "&Finish" ) );
    d->helpButton->setText( tr( "&Help" ) );

    d->nextButton->setDefault( true );

    connect( d->backButton, TQ_SIGNAL(clicked()),
	     this, TQ_SLOT(back()) );
    connect( d->nextButton, TQ_SIGNAL(clicked()),
	     this, TQ_SLOT(next()) );
    connect( d->finishButton, TQ_SIGNAL(clicked()),
	     this, TQ_SLOT(accept()) );
    connect( d->cancelButton, TQ_SIGNAL(clicked()),
	     this, TQ_SLOT(reject()) );
    connect( d->helpButton, TQ_SIGNAL(clicked()),
	     this, TQ_SLOT(help()) );

#ifndef TQT_NO_ACCEL
    d->accel = new TQAccel( this, "arrow-key accel" );
    d->backAccel = d->accel->insertItem( TQt::ALT + TQt::Key_Left );
    d->accel->connectItem( d->backAccel, this, TQ_SLOT(back()) );
    d->nextAccel = d->accel->insertItem( TQt::ALT + TQt::Key_Right );
    d->accel->connectItem( d->nextAccel, this, TQ_SLOT(next()) );
#endif
}


/*!
    Destroys the object and frees any allocated resources, including
    all pages and controllers.
*/

TQWizard::~TQWizard()
{
    delete d;
}


/*!
    \reimp
*/

void TQWizard::show()
{
    if ( !d->current ) {
	// No page yet
	if ( pageCount() > 0 )
	    showPage( d->pages.at( 0 )->w );
	else
	    showPage( 0 );
    }

    TQDialog::show();
}


/*!
    \reimp
*/

void TQWizard::setFont( const TQFont & font )
{
    TQApplication::postEvent( this, new TQEvent( TQEvent::LayoutHint ) );
    TQDialog::setFont( font );
}


/*!
    Adds \a page to the end of the page sequence, with the title, \a
    title.
*/

void TQWizard::addPage( TQWidget * page, const TQString & title )
{
    if ( !page )
	return;
    if ( d->page( page ) ) {
#if defined(QT_CHECK_STATE)
	tqWarning( "TQWizard::addPage(): already added %s/%s to %s/%s",
		  page->className(), page->name(),
		  className(), name() );
#endif
	return;
    }
    int i = d->pages.count();

    if( i > 0 )
	d->pages.at( i - 1 )->nextEnabled = true;

    TQWizardPrivate::Page * p = new TQWizardPrivate::Page( page, title );
    p->backEnabled = ( i > 0 );
    d->ws->addWidget( page, i );
    d->pages.append( p );
}

/*!
    Inserts \a page at position \a index into the page sequence, with
    title \a title. If \a index is -1, the page will be appended to
    the end of the wizard's page sequence.
*/

void TQWizard::insertPage( TQWidget * page, const TQString & title, int index )
{
    if ( !page )
	return;
    if ( d->page( page ) ) {
#if defined(QT_CHECK_STATE)
	tqWarning( "TQWizard::insertPage(): already added %s/%s to %s/%s",
		  page->className(), page->name(),
		  className(), name() );
#endif
	return;
    }

    if ( index < 0  || index > (int)d->pages.count() )
	index = d->pages.count();

    if( index > 0 && ( index == (int)d->pages.count() ) )
	d->pages.at( index - 1 )->nextEnabled = true;

    TQWizardPrivate::Page * p = new TQWizardPrivate::Page( page, title );
    p->backEnabled = ( index > 0 );
    p->nextEnabled = ( index < (int)d->pages.count() );

    d->ws->addWidget( page, index );
    d->pages.insert( index, p );
}

/*!
    \fn void TQWizard::selected(const TQString&)

    This signal is emitted when the current page changes. The
    parameter contains the title of the selected page.
*/


/*!
    Makes \a page the current page and emits the selected() signal.

    This virtual function is called whenever a different page is to
    be shown, including the first time the TQWizard is shown.
    By reimplementing it (and calling TQWizard::showPage()),
    you can prepare each page prior to it being shown.
*/

void TQWizard::showPage( TQWidget * page )
{
    TQWizardPrivate::Page * p = d->page( page );
    if ( p ) {
	int i;
	for( i = 0; i < (int)d->pages.count() && d->pages.at( i ) != p; i++ );
	bool notFirst( false );

	if( i ) {
	    i--;
	    while( ( i >= 0 ) && !notFirst ) {
		notFirst |= appropriate( d->pages.at( i )->w );
		i--;
	    }
	}
	setBackEnabled( notFirst );
	setNextEnabled( true );
	d->ws->raiseWidget( page );
	d->current = p;
    }

    layOut();
    updateButtons();
    emit selected( p ? p->t : TQString::null );
}


/*!
    Returns the number of pages in the wizard.
*/

int TQWizard::pageCount() const
{
    return d->pages.count();
}

/*!
    Returns the position of page \a page. If the page is not part of
    the wizard -1 is returned.
*/

int TQWizard::indexOf( TQWidget* page ) const
{
    TQWizardPrivate::Page * p = d->page( page );
    if ( !p ) return -1;

    return d->pages.find( p );
}

/*!
    Called when the user clicks the Back button; this function shows
    the preceding relevant page in the sequence.

    \sa appropriate()
*/
void TQWizard::back()
{
    int i = 0;

    while( i < (int)d->pages.count() && d->pages.at( i ) &&
	   d->current && d->pages.at( i )->w != d->current->w )
	i++;

    i--;
    while( i >= 0 && ( !d->pages.at( i ) || !appropriate( d->pages.at( i )->w ) ) )
	i--;

    if( i >= 0 )
       if( d->pages.at( i ) )
	    showPage( d->pages.at( i )->w );
}


/*!
    Called when the user clicks the Next button, this function shows
    the next relevant page in the sequence.

    \sa appropriate()
*/
void TQWizard::next()
{
    int i = 0;
    while( i < (int)d->pages.count() && d->pages.at( i ) &&
	   d->current && d->pages.at( i )->w != d->current->w )
	i++;
    i++;
    while( i <= (int)d->pages.count()-1 &&
	   ( !d->pages.at( i ) || !appropriate( d->pages.at( i )->w ) ) )
	i++;
    // if we fell of the end of the world, step back
    while ( i > 0 && (i >= (int)d->pages.count() || !d->pages.at( i ) ) )
	i--;
    if ( d->pages.at( i ) )
	showPage( d->pages.at( i )->w );
}


/*!
    \fn void TQWizard::helpClicked()

    This signal is emitted when the user clicks on the Help button.
*/

/*!
    Called when the user clicks the Help button, this function emits
    the helpClicked() signal.
*/

void TQWizard::help()
{
    TQWidget * page = d->ws->visibleWidget();
    if ( !page )
	return;

#if 0
    TQWizardPage *wpage = ::tqt_cast<TQWizardPage*>(page);
    if ( wpage )
	emit wpage->helpClicked();
#endif
    emit helpClicked();
}


void TQWizard::setBackEnabled( bool enable )
{
    d->backButton->setEnabled( enable );
#ifndef TQT_NO_ACCEL
    d->accel->setItemEnabled( d->backAccel, enable );
#endif
}


void TQWizard::setNextEnabled( bool enable )
{
    d->nextButton->setEnabled( enable );
#ifndef TQT_NO_ACCEL
    d->accel->setItemEnabled( d->nextAccel, enable );
#endif
}


void TQWizard::setHelpEnabled( bool enable )
{
    d->helpButton->setEnabled( enable );
}


/*!
  \fn void TQWizard::setFinish( TQWidget *, bool )
  \obsolete

  Use setFinishEnabled instead
*/

/*!
    If \a enable is true, page \a page has a Back button; otherwise \a
    page has no Back button. By default all pages have this button.
*/
void TQWizard::setBackEnabled( TQWidget * page, bool enable )
{
    TQWizardPrivate::Page * p = d->page( page );
    if ( !p )
	return;

    p->backEnabled = enable;
    updateButtons();
}


/*!
    If \a enable is true, page \a page has a Next button; otherwise
    the Next button on \a page is disabled. By default all pages have
    this button.
*/

void TQWizard::setNextEnabled( TQWidget * page, bool enable )
{
    TQWizardPrivate::Page * p = d->page( page );
    if ( !p )
	return;

    p->nextEnabled = enable;
    updateButtons();
}


/*!
    If \a enable is true, page \a page has a Finish button; otherwise
    \a page has no Finish button. By default \e no page has this
    button.
*/
void TQWizard::setFinishEnabled( TQWidget * page, bool enable )
{
    TQWizardPrivate::Page * p = d->page( page );
    if ( !p )
	return;

    p->finishEnabled = enable;
    updateButtons();
}


/*!
    If \a enable is true, page \a page has a Help button; otherwise \a
    page has no Help button. By default all pages have this button.
*/
void TQWizard::setHelpEnabled( TQWidget * page, bool enable )
{
    TQWizardPrivate::Page * p = d->page( page );
    if ( !p )
	return;

    p->helpEnabled = enable;
    updateButtons();
}


/*!
    Called when the Next button is clicked; this virtual function
    returns true if \a page is relevant for display in the current
    context; otherwise it is ignored by TQWizard and returns false. The
    default implementation returns the value set using
    setAppropriate(). The ultimate default is true.

    \warning The last page of the wizard will be displayed if no page
    is relevant in the current context.
*/

bool TQWizard::appropriate( TQWidget * page ) const
{
    TQWizardPrivate::Page * p = d->page( page );
    return p ? p->appropriate : true;
}


/*!
    If \a appropriate is true then page \a page is considered relevant
    in the current context and should be displayed in the page
    sequence; otherwise \a page should not be displayed in the page
    sequence.

    \sa appropriate()
*/
void TQWizard::setAppropriate( TQWidget * page, bool appropriate )
{
    TQWizardPrivate::Page * p = d->page( page );
    if ( p )
	p->appropriate = appropriate;
}


void TQWizard::updateButtons()
{
    if ( !d->current )
	return;

    int i;
    for( i = 0; i < (int)d->pages.count() && d->pages.at( i ) != d->current; i++ );
    bool notFirst( false );
    if( i ) {
	i--;
	while( ( i >= 0 ) && !notFirst ) {
	    notFirst |= appropriate( d->pages.at( i )->w );
	    i--;
	}
    }
    setBackEnabled( d->current->backEnabled && notFirst );
    setNextEnabled( d->current->nextEnabled );
    d->finishButton->setEnabled( d->current->finishEnabled );
    d->helpButton->setEnabled( d->current->helpEnabled );

    if ( ( d->current->finishEnabled && !d->finishButton->isVisible() ) ||
	 ( d->current->backEnabled && !d->backButton->isVisible() ) ||
	 ( d->current->nextEnabled && !d->nextButton->isVisible() ) ||
	 ( d->current->helpEnabled && !d->helpButton->isVisible() ) )
	layOut();
}


/*!
    Returns a pointer to the current page in the sequence. Although
    the wizard does its best to make sure that this value is never 0,
    it can be if you try hard enough.
*/

TQWidget * TQWizard::currentPage() const
{
    return d->ws->visibleWidget();
}


/*!
    Returns the title of page \a page.
*/

TQString TQWizard::title( TQWidget * page ) const
{
    TQWizardPrivate::Page * p = d->page( page );
    return p ? p->t : TQString::null;
}

/*!
    Sets the title for page \a page to \a title.
*/

void TQWizard::setTitle( TQWidget *page, const TQString &title )
{
    TQWizardPrivate::Page * p = d->page( page );
    if ( p )
	p->t = title;
    if ( page == currentPage() )
	d->title->setText( title );
}

/*!
    \property TQWizard::titleFont
    \brief the font used for page titles

    The default is TQApplication::font().
*/
TQFont TQWizard::titleFont() const
{
    return d->title->font();
}

void TQWizard::setTitleFont( const TQFont & font )
{
    d->title->setFont( font );
}


/*!
    Returns a pointer to the dialog's Back button

    By default, this button is connected to the back() slot, which is
    virtual so you can reimplement it in a TQWizard subclass. Use
    setBackEnabled() to enable/disable this button.
*/
TQPushButton * TQWizard::backButton() const
{
    return d->backButton;
}


/*!
    Returns a pointer to the dialog's Next button

    By default, this button is connected to the next() slot, which is
    virtual so you can reimplement it in a TQWizard subclass. Use
    setNextEnabled() to enable/disable this button.
*/
TQPushButton * TQWizard::nextButton() const
{
    return d->nextButton;
}


/*!
    Returns a pointer to the dialog's Finish button

    By default, this button is connected to the TQDialog::accept()
    slot, which is virtual so you can reimplement it in a TQWizard
    subclass. Use setFinishEnabled() to enable/disable this button.
*/
TQPushButton * TQWizard::finishButton() const
{
    return d->finishButton;
}


/*!
    Returns a pointer to the dialog's Cancel button

    By default, this button is connected to the TQDialog::reject()
    slot, which is virtual so you can reimplement it in a TQWizard
    subclass.
*/
TQPushButton * TQWizard::cancelButton() const
{
    return d->cancelButton;
}


/*!
    Returns a pointer to the dialog's Help button

    By default, this button is connected to the help() slot, which is
    virtual so you can reimplement it in a TQWizard subclass. Use
    setHelpEnabled() to enable/disable this button.
*/
TQPushButton * TQWizard::helpButton() const
{
    return d->helpButton;
}


/*!
    This virtual function is responsible for adding the buttons below
    the bottom divider.

    \a layout is the horizontal layout of the entire wizard.
*/

void TQWizard::layOutButtonRow( TQHBoxLayout * layout )
{
    bool hasHelp = false;
    bool hasEarlyFinish = false;

    int i = d->pages.count() - 2;
    while ( !hasEarlyFinish && i >= 0 ) {
	if ( d->pages.at( i ) && d->pages.at( i )->finishEnabled )
	    hasEarlyFinish = true;
	i--;
    }
    i = 0;
    while ( !hasHelp && i < (int)d->pages.count() ) {
	if ( d->pages.at( i ) && d->pages.at( i )->helpEnabled )
	    hasHelp = true;
	i++;
    }

    TQBoxLayout * h = new TQBoxLayout( TQBoxLayout::LeftToRight );
    layout->addLayout( h );

    if ( hasHelp )
	h->addWidget( d->helpButton );
    else
	d->helpButton->hide();

    h->addStretch( 42 );

    h->addWidget( d->backButton );

    h->addSpacing( 6 );

    if (d->current == d->pages.at( d->pages.count()-1 ))
        hasEarlyFinish = false;

    if ( hasEarlyFinish ) {
	d->nextButton->show();
	d->finishButton->show();
	h->addWidget( d->nextButton );
	h->addSpacing( 12 );
	h->addWidget( d->finishButton );
    } else if ( d->pages.count() == 0 ||
		d->current->finishEnabled ||
		d->current == d->pages.at( d->pages.count()-1 ) ) {
	d->nextButton->hide();
	d->finishButton->show();
	h->addWidget( d->finishButton );
    } else {
	d->nextButton->show();
	d->finishButton->hide();
	h->addWidget( d->nextButton );
    }

    // if last page is disabled - show finished btn. at lastpage-1
    i = d->pages.count()-1;
    if ( i >= 0 && !appropriate( d->pages.at( i )->w ) &&
	 d->current == d->pages.at( d->pages.count()-2 ) ) {
	d->nextButton->hide();
	d->finishButton->show();
	h->addWidget( d->finishButton );
    }

    h->addSpacing( 12 );
    h->addWidget( d->cancelButton );
}


/*!
    This virtual function is responsible for laying out the title row.

    \a layout is the horizontal layout for the wizard, and \a
    title is the title for this page. This function is called every
    time \a title changes.
*/

void TQWizard::layOutTitleRow( TQHBoxLayout * layout, const TQString & title )
{
    d->title->setText( title );
    layout->addWidget( d->title, 10 );
}


/*

*/

void TQWizard::layOut()
{
    delete d->v;
    d->v = new TQVBoxLayout( this, 6, 0, "top-level layout" );

    TQHBoxLayout * l;
    l = new TQHBoxLayout( 6 );
    d->v->addLayout( l, 0 );
    layOutTitleRow( l, d->current ? d->current->t : TQString::null );

    if ( ! d->hbar1 ) {
	d->hbar1 = new TQFrame( this, "<hr>", 0 );
	d->hbar1->setFrameStyle( TQFrame::Sunken + TQFrame::HLine );
	d->hbar1->setFixedHeight( 12 );
    }

    d->v->addWidget( d->hbar1 );

    d->v->addWidget( d->ws, 10 );

    if ( ! d->hbar2 ) {
	d->hbar2 = new TQFrame( this, "<hr>", 0 );
	d->hbar2->setFrameStyle( TQFrame::Sunken + TQFrame::HLine );
	d->hbar2->setFixedHeight( 12 );
    }
    d->v->addWidget( d->hbar2 );

    l = new TQHBoxLayout( 6 );
    d->v->addLayout( l );
    layOutButtonRow( l );
    d->v->activate();
}


/*!
    \reimp
*/

bool TQWizard::eventFilter( TQObject * o, TQEvent * e )
{
    if ( o == d->ws && e && e->type() == TQEvent::ChildRemoved ) {
	TQChildEvent * c = (TQChildEvent*)e;
	if ( c->child() && c->child()->isWidgetType() )
	    removePage( (TQWidget *)c->child() );
    }
    return TQDialog::eventFilter( o, e );
}


/*!
    Removes \a page from the page sequence but does not delete the
    page. If \a page is currently being displayed, TQWizard will
    display the page that precedes it, or the first page if this was
    the first page.
*/

void TQWizard::removePage( TQWidget * page )
{
    if ( !page )
	return;

    int i = d->pages.count();
    TQWidget* cp = currentPage();
    while( --i >= 0 && d->pages.at( i ) && d->pages.at( i )->w != page ) { }
    if ( i < 0 )
	return;
    TQWizardPrivate::Page * p = d->pages.at( i );
    d->pages.removeRef( p );
    d->ws->removeWidget( page );

    if( cp == page ) {
	i--;
	if( i < 0 )
	    i = 0;
	if ( pageCount() > 0 )
	    showPage( TQWizard::page( i ) );
    }
}


/*!
    Returns a pointer to the page at position \a index in the
    sequence, or 0 if \a index is out of range. The first page has
    index 0.
*/

TQWidget* TQWizard::page( int index ) const
{
    if ( index >= pageCount() || index < 0 )
      return 0;

    return d->pages.at( index )->w;
}

#endif // TQT_NO_WIZARD
