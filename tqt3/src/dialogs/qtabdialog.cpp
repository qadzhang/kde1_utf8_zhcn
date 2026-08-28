/****************************************************************************
**
** Implementation of TQTabDialog class
**
** Created : 960825
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

#include "ntqtabdialog.h"

#ifndef TQT_NO_TABDIALOG

#include "ntqobjectlist.h"
#include "ntqtabbar.h"
#include "ntqtabwidget.h"
#include "ntqpushbutton.h"
#include "ntqpainter.h"
#include "ntqpixmap.h"
#include "ntqapplication.h"
#include "ntqtabwidget.h"
#include "ntqwidgetstack.h"
#include "ntqlayout.h"

/*!
  \class TQTabDialog ntqtabdialog.h

  \brief The TQTabDialog class provides a stack of tabbed widgets.

  \ingroup dialogs
  \mainclass

  A tabbed dialog is one in which several "tab pages" are available.
  By clicking on a tab page's tab or by pressing the indicated
  Alt+\e{letter} key combination, the user can select which tab page
  they want to use.

  TQTabDialog provides a tab bar consisting of single row of tabs at
  the top; each tab has an associated widget which is that tab's
  tab page. In addition, TQTabDialog provides an OK button and the
  following optional buttons: Apply, Cancel, Defaults and Help.

  The normal way to use TQTabDialog is to do the following in the
  constructor:
  \list 1
  \i Create a TQTabDialog.
  \i Create a TQWidget for each of the pages in the tab dialog, insert
  children into it, set up geometry management for it, and use
  addTab() (or insertTab()) to set up a tab and keyboard accelerator
  for it.
  \i Set up the buttons for the tab dialog using setOkButton(),
  setApplyButton(), setDefaultsButton(), setCancelButton() and
  setHelpButton().
  \i Connect to the signals and slots.
  \endlist

  If you don't call addTab() the page you have created will not be
  visible. Don't confuse the object name you supply to the
  TQWidget constructor and the tab label you supply to addTab();
  addTab() takes user-visible name that appears on the widget's tab
  and may identify an accelerator, whereas the widget name is used
  primarily for debugging.

  Almost all applications have to connect the applyButtonPressed()
  signal to something. applyButtonPressed() is emitted when either OK
  or Apply is clicked, and your slot must copy the dialog's state into
  the application.

  There are also several other signals which may be useful:
  \list
  \i cancelButtonPressed() is emitted when the user clicks Cancel.
  \i defaultButtonPressed() is emitted when the user clicks Defaults;
  the slot it is connected to should reset the state of the dialog to
  the application defaults.
  \i helpButtonPressed() is emitted when the user clicks Help.
  \i aboutToShow() is emitted at the start of show(); if there is any
  chance that the state of the application may change between the
  creation of the tab dialog and the time show() is called, you must
  connect this signal to a slot that resets the state of the dialog.
  \i currentChanged() is emitted when the user selects a page.
  \endlist

  Each tab is either enabled or disabled at any given time (see
  setTabEnabled()). If a tab is enabled the tab text is drawn in
  black and the user can select that tab. If it is disabled the tab
  is drawn in a different way and the user cannot select that tab.
  Note that even if a tab is disabled, the page can still be visible;
  for example, if all of the tabs happen to be disabled.

  You can change a tab's label and iconset using changeTab(). A tab
  page can be removed with removePage() and shown with showPage(). The
  current page is given by currentPage().

  TQTabDialog does not support tabs on the sides or bottom, nor can
  you set or retrieve the visible page. If you need more functionality
  than TQTabDialog provides, consider creating a TQDialog and using a
  TQTabBar with TQTabWidgets.

  Most of the functionality in TQTabDialog is provided by a TQTabWidget.

  <img src=qtabdlg-m.png> <img src=qtabdlg-w.png>

  \sa TQDialog
*/

/*!
  \fn void TQTabDialog::selected( const TQString & );
  \obsolete

  This signal is emitted whenever a tab is selected (raised),
  including during the first show().

  \sa raise()
*/

/*! \fn void TQTabDialog::currentChanged( TQWidget* );

  This signal is emitted whenever the current page changes.

  \sa currentPage(), showPage(), tabLabel()
*/


// add comments about delete, ok and apply

class TQTabDialogPrivate
{
public:
    TQTabDialogPrivate();

    TQTabWidget* tw;

    TQPushButton * ok;
    TQPushButton * cb;
    TQPushButton * db;
    TQPushButton * hb;
    TQPushButton * ab;

    TQBoxLayout * tll;
};

TQTabDialogPrivate::TQTabDialogPrivate()
	: tw(0),
	  ok(0), cb(0), db(0), hb(0), ab(0),
	  tll(0)
{ }

/*!
  Constructs a TQTabDialog with only an OK button.
  The \a parent, \a name, \a modal and widget flag, \a f, arguments
  are passed on to the TQDialog constructor.
*/

TQTabDialog::TQTabDialog( TQWidget *parent, const char *name, bool modal,
			WFlags f )
    : TQDialog( parent, name, modal, f )
{
    d = new TQTabDialogPrivate;
    TQ_CHECK_PTR( d );

    d->tw = new TQTabWidget( this, "tab widget" );
    connect ( d->tw, TQ_SIGNAL ( selected(const TQString&) ), this, TQ_SIGNAL( selected(const TQString&) ) );
    connect ( d->tw, TQ_SIGNAL ( currentChanged(TQWidget*) ), this, TQ_SIGNAL( currentChanged(TQWidget*) ) );

    d->ok = new TQPushButton( this, "ok" );
    TQ_CHECK_PTR( d->ok );
    d->ok->setText( tr("OK") );
    d->ok->setDefault( true );
    connect( d->ok, TQ_SIGNAL(clicked()),
	     this, TQ_SIGNAL(applyButtonPressed()) );
    connect( d->ok, TQ_SIGNAL(clicked()),
	     this, TQ_SLOT(accept()) );
}


/*!
  Destroys the tab dialog.
*/

TQTabDialog::~TQTabDialog()
{
    delete d;
}


/*!
  Sets the font for the tabs to \a font.

  If the widget is visible, the display is updated with the new font
  immediately. There may be some geometry changes, depending on the
  size of the old and new fonts.
*/

void TQTabDialog::setFont( const TQFont & font )
{
    TQDialog::setFont( font );
    setSizes();
}


/*!
  \fn void TQTabDialog::applyButtonPressed();

  This signal is emitted when either the Apply or OK button is clicked.

  It should be connected to a slot (or several slots) that change the
  application's state according to the state of the dialog.

  \sa cancelButtonPressed() defaultButtonPressed() setApplyButton()
*/


/*!
  Returns true if the tab dialog has a Defaults button; otherwise
  returns false.

  \sa setDefaultButton() defaultButtonPressed() hasApplyButton()
  hasCancelButton()
*/

bool TQTabDialog::hasDefaultButton() const
{
     return d->db != 0;
}


/*!
  Returns true if the tab dialog has a Help button; otherwise returns
  false.

  \sa setHelpButton() helpButtonPressed() hasApplyButton()
  hasCancelButton()
*/

bool TQTabDialog::hasHelpButton() const
{
     return d->hb != 0;
}


/*!
  \fn void TQTabDialog::cancelButtonPressed();

  This signal is emitted when the Cancel button is clicked. It is
  automatically connected to TQDialog::reject(), which will hide the
  dialog.

  The Cancel button should not change the application's state at all,
  so you should generally not need to connect it to any slot.

  \sa applyButtonPressed() defaultButtonPressed() setCancelButton()
*/


/*!
  Returns true if the tab dialog has a Cancel button; otherwise
  returns false.

  \sa setCancelButton() cancelButtonPressed() hasApplyButton()
  hasDefaultButton()
*/

bool TQTabDialog::hasCancelButton() const
{
     return d->cb != 0;
}


/*!
  \fn void TQTabDialog::defaultButtonPressed();

  This signal is emitted when the Defaults button is pressed. It
  should reset the dialog (but not the application) to the "factory
  defaults".

  The application's state should not be changed until the user clicks
  Apply or OK.

  \sa applyButtonPressed() cancelButtonPressed() setDefaultButton()
*/


/*!
  \fn void TQTabDialog::helpButtonPressed();

  This signal is emitted when the Help button is pressed. It
  could be used to present information about how to use the dialog.

  \sa applyButtonPressed() cancelButtonPressed() setHelpButton()
*/


/*!
  Returns true if the tab dialog has an Apply button; otherwise
  returns false.

  \sa setApplyButton() applyButtonPressed() hasCancelButton()
  hasDefaultButton()
*/

bool TQTabDialog::hasApplyButton() const
{
    return d->ab != 0;
}


/*!
  Returns true if the tab dialog has an OK button; otherwise returns
  false.

  \sa setOkButton() hasApplyButton() hasCancelButton()
  hasDefaultButton()
*/

bool TQTabDialog::hasOkButton() const
{
    return d->ok != 0;
}


/*!
  \fn void TQTabDialog::aboutToShow()

  This signal is emitted by show() when it is time to set the state of
  the dialog's contents. The dialog should reflect the current state
  of the application when it appears; if there is any possibility that
  the state of the application may change between the time you call
  TQTabDialog::TQTabDialog() and TQTabDialog::show(), you should set the
  dialog's state in a slot and connect this signal to it.

  This applies mainly to TQTabDialog objects that are kept around
  hidden, rather than being created, shown, and deleted afterwards.

  \sa applyButtonPressed(), show(), cancelButtonPressed()
*/


/*!\reimp
*/
void TQTabDialog::show()
{
    //   Reimplemented in order to delay show()'ing of every page
    //   except the initially visible one, and in order to emit the
    //   aboutToShow() signal.
    if ( topLevelWidget() == this )
	d->tw->setFocus();
    emit aboutToShow();
    setSizes();
    setUpLayout();
    TQDialog::show();
}


/*!
  Ensures that tab page \a i is visible and appropriately sized.
*/

void TQTabDialog::showTab( int i )
{
    d->tw->showTab( i );
}


/*!
  Adds another tab and page to the tab view.

  The new page is \a child; the tab's label is \a label.
  Note the difference between the widget name (which you supply to
  widget constructors and to setTabEnabled(), for example) and the tab
  label. The name is internal to the program and invariant, whereas
  the label is shown on-screen and may vary according to language and
  other factors.

  If the tab's \a label contains an ampersand, the letter following
  the ampersand is used as an accelerator for the tab, e.g. if the
  label is "Bro&wse" then Alt+W becomes an accelerator which will
  move the focus to this tab.

  If you call addTab() after show() the screen will flicker and the
  user may be confused.

  \sa insertTab()
*/

void TQTabDialog::addTab( TQWidget * child, const TQString &label )
{
    d->tw->addTab( child, label );
}



/*! \overload

  This version of the function shows the \a iconset as well as the \a
  label on the tab of \a child.
*/
void TQTabDialog::addTab( TQWidget *child, const TQIconSet& iconset, const TQString &label)
{
    d->tw->addTab( child, iconset, label );
}

/*!
  \overload

  This is a lower-level method for adding tabs, similar to the other
  addTab() method. It is useful if you are using setTabBar() to set a
  TQTabBar subclass with an overridden TQTabBar::paint() function for a
  subclass of TQTab.

  The \a child is the widget to be placed on the new tab page. The \a
  tab is the tab to display on the tab page -- normally this shows a
  label or an icon that identifies the tab page.

*/
void TQTabDialog::addTab( TQWidget * child, TQTab* tab )
{
    d->tw->addTab( child, tab );
}

/*!
  Inserts another tab and page to the tab view.

  The new page is \a child; the tab's label is \a label.
  Note the difference between the widget name (which you supply to
  widget constructors and to setTabEnabled(), for example) and the tab
  label. The name is internal to the program and invariant, whereas
  the label is shown on-screen and may vary according to language and
  other factors.

  If the tab's \a label contains an ampersand, the letter following
  the ampersand is used as an accelerator for the tab, e.g. if the
  label is "Bro&wse" then Alt+W becomes an accelerator which will
  move the focus to this tab.

  If \a index is not specified, the tab is simply added. Otherwise
  it is inserted at the specified position.

  If you call insertTab() after show(), the screen will flicker and the
  user may be confused.

  \sa addTab()
*/

void TQTabDialog::insertTab( TQWidget * child, const TQString &label, int index )
{
    d->tw->insertTab( child, label, index );
}


/*! \overload

  This version of the function shows the \a iconset as well as the \a
  label on the tab of \a child.
 */
void TQTabDialog::insertTab( TQWidget *child, const TQIconSet& iconset, const TQString &label, int index)
{
    d->tw->insertTab( child, iconset, label, index );
}

/*!
    \overload

  This is a lower-level method for inserting tabs, similar to the other
  insertTab() method. It is useful if you are using setTabBar() to set a
  TQTabBar subclass with an overridden TQTabBar::paint() function for a
  subclass of TQTab.

  The \a child is the widget to be placed on the new tab page. The \a
  tab is the tab to display on the tab page -- normally this shows a
  label or an icon that identifies the tab page. The \a index is the
  position where this tab page should be inserted.

*/
void TQTabDialog::insertTab( TQWidget * child, TQTab* tab, int index )
{
    d->tw->insertTab( child, tab, index );
}

/*!
  Replaces the TQTabBar heading the dialog by the given tab bar, \a tb.
  Note that this must be called \e before any tabs have been added,
  or the behavior is undefined.
  \sa tabBar()
*/
void TQTabDialog::setTabBar( TQTabBar* tb )
{
    d->tw->setTabBar( tb );
    setUpLayout();
}

/*!
  Returns the currently set TQTabBar.
  \sa setTabBar()
*/
TQTabBar* TQTabDialog::tabBar() const
{
    return d->tw->tabBar();
}

/*!  Ensures that widget \a w is shown. This is mainly useful for accelerators.

  \warning If used carelessly, this function can easily surprise or
  confuse the user.

  \sa TQTabBar::setCurrentTab()
*/

void TQTabDialog::showPage( TQWidget * w )
{
    d->tw->showPage( w );
}


/*! \obsolete
  Returns true if the page with object name \a name is enabled and
  false if it is disabled.

  If \a name is 0 or not the name of any of the pages, isTabEnabled()
  returns false.

  \sa setTabEnabled(), TQWidget::isEnabled()
*/

bool TQTabDialog::isTabEnabled( const char* name ) const
{
    if ( !name )
	return false;
    TQObjectList * l
	= ((TQTabDialog *)this)->queryList( "TQWidget", name, false, true );
    if ( l && l->first() ) {
	TQWidget * w;
	while( l->current() ) {
	    while( l->current() && !l->current()->isWidgetType() )
		l->next();
	    w = (TQWidget *)(l->current());
	    if ( w ) {
		bool enabled = d->tw->isTabEnabled( w );
		delete l;
		return enabled;
	    }
	}
    }
    delete l;
    return false;
}


/*!\obsolete

  Finds the page with object name \a name, enables/disables it
  according to the value of \a enable and redraws the page's tab
  appropriately.

  TQTabDialog uses TQWidget::setEnabled() internally, rather than keeping a
  separate flag.

  Note that even a disabled tab/page may be visible. If the page is
  already visible TQTabDialog will not hide it; if all the pages
  are disabled TQTabDialog will show one of them.

  The object name is used (rather than the tab label) because the tab
  text may not be invariant in multi-language applications.

  \sa isTabEnabled(), TQWidget::setEnabled()
*/

void TQTabDialog::setTabEnabled( const char* name, bool enable )
{
    if ( !name )
	return;
    TQObjectList * l
	= ((TQTabDialog *)this)->queryList( "TQWidget", name, false, true );
    if ( l && l->first() ) {
	TQObjectListIt it(*l);
	TQObject *o;
	while( (o = it.current()) ) {
	    ++it;
	    if( o->isWidgetType() )
		d->tw->setTabEnabled( (TQWidget*)o, enable );
	}
    }
    delete l;
}


/* ### SHOULD THIS BE HERE?
  Adds an Apply button to the dialog. The button's text is set to \e
  text (and defaults to "Apply").

  The Apply button should apply the current settings in the dialog box
  to the application, while keeping the dialog visible.

  When Apply is clicked, the applyButtonPressed() signal is emitted.

  If \a text is a
  \link TQString::operator!() null string\endlink,
  no button is shown.

  \sa setCancelButton() setDefaultButton() applyButtonPressed()
*/


/*!
  Returns true if the page \a w is enabled; otherwise returns false.

  \sa setTabEnabled(), TQWidget::isEnabled()
*/

bool TQTabDialog::isTabEnabled( TQWidget* w ) const
{
    return d->tw->isTabEnabled( w );
}

/*!
  If \a enable is true the page \a w is enabled; otherwise \a w is
  disabled. The page's tab is redrawn appropriately.

  TQTabWidget uses TQWidget::setEnabled() internally, rather than keeping a
  separate flag.

  Note that even a disabled tab and tab page may be visible. If the
  page is already visible TQTabWidget will not hide it; if all the
  pages are disabled TQTabWidget will show one of them.

  \sa isTabEnabled(), TQWidget::setEnabled()
*/

void TQTabDialog::setTabEnabled( TQWidget* w, bool enable)
{
    d->tw->setTabEnabled( w, enable );
}


/*!
  Adds an Apply button to the dialog. The button's text is set to \a
  text.

  The Apply button should apply the current settings in the dialog box
  to the application while keeping the dialog visible.

  When Apply is clicked, the applyButtonPressed() signal is emitted.

  If \a text is a
  \link TQString::operator!() null string\endlink,
  no button is shown.

  \sa setCancelButton() setDefaultButton() applyButtonPressed()
*/
void TQTabDialog::setApplyButton( const TQString &text )
{
    if ( !text && d->ab ) {
	delete d->ab;
	d->ab = 0;
	setSizes();
    } else {
	if ( !d->ab ) {
	    d->ab = new TQPushButton( this, "apply settings" );
	    connect( d->ab, TQ_SIGNAL(clicked()),
		     this, TQ_SIGNAL(applyButtonPressed()) );
	    setUpLayout();
	}
	d->ab->setText( text );
	setSizes();
	//d->ab->show();
    }
}

/*!
    \overload

  Adds an Apply button to the dialog. The button's text is set to
  a localizable "Apply".
 */
void TQTabDialog::setApplyButton()
{
    setApplyButton( tr("Apply") );
}


/*!
  Adds a Help button to the dialog. The button's text is set to \a
  text.

  When Help is clicked, the helpButtonPressed() signal is emitted.

  If \a text is a
  \link TQString::operator!() null string\endlink,
  no button is shown.

  \sa setApplyButton() setCancelButton() helpButtonPressed()
*/

void TQTabDialog::setHelpButton( const TQString &text )
{
    if ( !text ) {
	delete d->hb;
	d->hb = 0;
	setSizes();
    } else {
	if ( !d->hb ) {
	    d->hb = new TQPushButton( this, "give help" );
	    connect( d->hb, TQ_SIGNAL(clicked()),
		     this, TQ_SIGNAL(helpButtonPressed()) );
	    setUpLayout();
	}
	d->hb->setText( text );
	setSizes();
	//d->hb->show();
    }
}


/*!
    \overload

  Adds a Help button to the dialog. The button's text is set to
  a localizable "Help".
 */
void TQTabDialog::setHelpButton()
{
    setHelpButton( tr("Help") );
}

/*!
  Adds a Defaults button to the dialog. The button's text is set to \a
  text.

  The Defaults button should set the dialog (but not the application)
  back to the application defaults.

  When Defaults is clicked, the defaultButtonPressed() signal is emitted.

  If \a text is a
  \link TQString::operator!() null string\endlink,
  no button is shown.

  \sa setApplyButton() setCancelButton() defaultButtonPressed()
*/

void TQTabDialog::setDefaultButton( const TQString &text )
{
    if ( !text ) {
	delete d->db;
	d->db = 0;
	setSizes();
    } else {
	if ( !d->db ) {
	    d->db = new TQPushButton( this, "back to default" );
	    connect( d->db, TQ_SIGNAL(clicked()),
		     this, TQ_SIGNAL(defaultButtonPressed()) );
	    setUpLayout();
	}
	d->db->setText( text );
	setSizes();
	//d->db->show();
    }
}


/*!
    \overload

  Adds a Defaults button to the dialog. The button's text is set to
  a localizable "Defaults".
 */
void TQTabDialog::setDefaultButton()
{
    setDefaultButton( tr("Defaults") );
}

/*!
  Adds a Cancel button to the dialog. The button's text is set to \a
  text.

  The cancel button should always return the application to the state
  it was in before the tab view popped up, or if the user has clicked
  Apply, back to the state immediately after the last Apply.

  When Cancel is clicked, the cancelButtonPressed() signal is emitted.
  The dialog is closed at the same time.

  If \a text is a
  \link TQString::operator!() null string\endlink,
  no button is shown.

  \sa setApplyButton() setDefaultButton() cancelButtonPressed()
*/

void TQTabDialog::setCancelButton( const TQString &text )
{
    if ( !text ) {
	delete d->cb;
	d->cb = 0;
	setSizes();
    } else {
	if ( !d->cb ) {
	    d->cb = new TQPushButton( this, "cancel dialog" );
	    connect( d->cb, TQ_SIGNAL(clicked()),
		     this, TQ_SIGNAL(cancelButtonPressed()) );
	    connect( d->cb, TQ_SIGNAL(clicked()),
		     this, TQ_SLOT(reject()) );
	    setUpLayout();
	}
	d->cb->setText( text );
	setSizes();
	//d->cb->show();
    }
}


/*!
    \overload

  Adds a Cancel button to the dialog. The button's text is set to
  a localizable "Cancel".
 */

void TQTabDialog::setCancelButton()
{
    setCancelButton( tr("Cancel") );
}


/*!  Sets up the layout manager for the tab dialog.

  \sa setSizes() setApplyButton() setCancelButton() setDefaultButton()
*/

void TQTabDialog::setUpLayout()
{
    // the next four are probably the same, really?
    const int topMargin = 6;
    const int leftMargin = 6;
    const int rightMargin = 6;
    const int bottomMargin = 6;
    const int betweenButtonsMargin = 7;
    const int aboveButtonsMargin = 8;

    delete d->tll;
    d->tll = new TQBoxLayout( this, TQBoxLayout::Down );

    // top margin
    d->tll->addSpacing( topMargin );

    TQBoxLayout * tmp = new TQHBoxLayout();
    d->tll->addLayout( tmp, 1 );
    tmp->addSpacing( leftMargin );
    tmp->addWidget( d->tw, 1);
    tmp->addSpacing( rightMargin + 2 );

    d->tll->addSpacing( aboveButtonsMargin + 2 );
    TQBoxLayout * buttonRow = new TQBoxLayout(TQBoxLayout::RightToLeft);
    d->tll->addLayout( buttonRow, 0 );
    d->tll->addSpacing( bottomMargin );

    buttonRow->addSpacing( rightMargin );
    if ( d->cb ) {
	buttonRow->addWidget( d->cb, 0 );
	buttonRow->addSpacing( betweenButtonsMargin );
	d->cb->raise();
    }

    if ( d->ab ) {
	buttonRow->addWidget( d->ab, 0 );
	buttonRow->addSpacing( betweenButtonsMargin );
	d->ab->raise();
    }

    if ( d->db ) {
	buttonRow->addWidget( d->db, 0 );
	buttonRow->addSpacing( betweenButtonsMargin );
	d->db->raise();
    }

    if ( d->hb ) {
	buttonRow->addWidget( d->hb, 0 );
	buttonRow->addSpacing( betweenButtonsMargin );
	d->hb->raise();
    }

    if ( d->ok ) {
	buttonRow->addWidget( d->ok, 0 );
	buttonRow->addSpacing( betweenButtonsMargin );
	d->ok->raise();
    }

    // add one custom widget here
    buttonRow->addStretch( 1 );
    // add another custom widget here

    d->tll->activate();
}


/*!  Sets up the minimum and maximum sizes for each child widget.

  \sa setUpLayout() setFont()
*/

void TQTabDialog::setSizes()
{
    // compute largest button size
    TQSize s( 0, 0 );
    int bw = s.width();
    int bh = s.height();

    if ( d->ok ) {
	s = d->ok->sizeHint();
	if ( s.width() > bw )
	    bw = s.width();
	if ( s.height() > bh )
	    bh = s.height();
    }

    if ( d->ab ) {
	s = d->ab->sizeHint();
	if ( s.width() > bw )
	    bw = s.width();
	if ( s.height() > bh )
	    bh = s.height();
    }

    if ( d->db ) {
	s = d->db->sizeHint();
	if ( s.width() > bw )
	    bw = s.width();
	if ( s.height() > bh )
	    bh = s.height();
    }

    if ( d->hb ) {
	s = d->hb->sizeHint();
	if ( s.width() > bw )
	    bw = s.width();
	if ( s.height() > bh )
	    bh = s.height();
    }

    if ( d->cb ) {
	s = d->cb->sizeHint();
	if ( s.width() > bw )
	    bw = s.width();
	if ( s.height() > bh )
	    bh = s.height();
    }

    // and set all the buttons to that size
    if ( d->ok )
	d->ok->setFixedSize( bw, bh );
    if ( d->ab )
	d->ab->setFixedSize( bw, bh );
    if ( d->db )
	d->db->setFixedSize( bw, bh );
    if ( d->hb )
	d->hb->setFixedSize( bw, bh );
    if ( d->cb )
	d->cb->setFixedSize( bw, bh );

    // fiddle the tab chain so the buttons are in their natural order
    TQWidget * w = d->ok;

    if ( d->hb ) {
	if ( w )
	    setTabOrder( w, d->hb );
	w = d->hb;
    }
    if ( d->db ) {
	if ( w )
	    setTabOrder( w, d->db );
	w = d->db;
    }
    if ( d->ab ) {
	if ( w )
	    setTabOrder( w, d->ab );
	w = d->ab;
    }
    if ( d->cb ) {
	if ( w )
	    setTabOrder( w, d->cb );
	w = d->cb;
    }
    setTabOrder( w, d->tw );
}

/*!\reimp
*/
void TQTabDialog::resizeEvent( TQResizeEvent * e )
{
    TQDialog::resizeEvent( e );
}


/*!\reimp
*/
void TQTabDialog::paintEvent( TQPaintEvent * )
{
}


/*!
  Adds an OK button to the dialog and sets the button's text to \a text.

  When the OK button is clicked, the applyButtonPressed() signal is emitted,
  and the current settings in the dialog box should be applied to
  the application. The dialog then closes.

  If \a text is a
  \link TQString::operator!() null string\endlink,
  no button is shown.

  \sa setCancelButton() setDefaultButton() applyButtonPressed()
*/

void TQTabDialog::setOkButton( const TQString &text )
{
    if ( !text ) {
	delete d->ok;
	d->ok = 0;
	setSizes();
    } else {
	if ( !d->ok ) {
	    d->ok = new TQPushButton( this, "ok" );
	    connect( d->ok, TQ_SIGNAL(clicked()),
		     this, TQ_SIGNAL(applyButtonPressed()) );
	    setUpLayout();
	}
	d->ok->setText( text );
	setSizes();
	//d->ok->show();
    }
}
/*!
  \overload

  Adds an OK button to the dialog. The button's text is set to
  a localizable "OK".
 */

void TQTabDialog::setOkButton()
{
    setOkButton( tr("OK") );
}


/*
    \overload
  Old version of setOkButton(), provided for backward compatibility.
*/
void TQTabDialog::setOKButton( const TQString &text )
{
    // Ugly workaround for original "OK" default argument
    TQString newText( text );
    if ( text.isNull() )
	newText = TQString::fromLatin1( "OK" );
    setOkButton( newText );
}


/*!  Returns the text in the tab for page \a w.
*/

TQString TQTabDialog::tabLabel( TQWidget * w )
{
    return d->tw->tabLabel( w );
}


/*!  \reimp
*/
void TQTabDialog::styleChange( TQStyle& s )
{
    TQDialog::styleChange( s );
    setSizes();
}


/*!  Returns a pointer to the page currently being displayed by the
tab dialog. The tab dialog does its best to make sure that this value
is never 0 (but if you try hard enough, it can be).
*/

TQWidget * TQTabDialog::currentPage() const
{
    return d->tw->currentPage();
}

/*!
 \overload
  Defines a new \a label for the tab of page \a w
 */
void TQTabDialog::changeTab( TQWidget *w, const TQString &label)
{
    d->tw->changeTab( w, label );
}

/*!
    Changes tab page \a w's iconset to \a iconset and label to \a label.

 */
void TQTabDialog::changeTab( TQWidget *w, const TQIconSet& iconset, const TQString &label)
{
    d->tw->changeTab( w, iconset, label );
}

/*! Removes page \a w from this stack of widgets. Does not
  delete \a w.
  \sa showPage(), TQTabWidget::removePage(), TQWidgetStack::removeWidget()
*/
void TQTabDialog::removePage( TQWidget * w )
{
    d->tw->removePage( w );
}

#endif
