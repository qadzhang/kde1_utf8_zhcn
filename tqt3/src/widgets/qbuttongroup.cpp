/****************************************************************************
**
** Implementation of TQButtonGroup class
**
** Created : 950130
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

#include "ntqbuttongroup.h"
#ifndef TQT_NO_BUTTONGROUP
#include "ntqbutton.h"
#include "ntqptrlist.h"
#include "ntqapplication.h"
#include "ntqradiobutton.h"



/*!
    \class TQButtonGroup ntqbuttongroup.h
    \brief The TQButtonGroup widget organizes TQButton widgets in a group.

    \ingroup organizers
    \ingroup geomanagement
    \ingroup appearance
    \mainclass

    A button group widget makes it easier to deal with groups of
    buttons. Each button in a button group has a unique identifier.
    The button group emits a clicked() signal with this identifier
    when a button in the group is clicked. This makes a button group
    particularly useful when you have several similar buttons and want
    to connect all their clicked() signals to a single slot.

    An \link setExclusive() exclusive\endlink button group switches
    off all toggle buttons except the one that was clicked. A button
    group is, by default, non-exclusive. Note that all radio buttons
    that are inserted into a button group are mutually exclusive even
    if the button group is non-exclusive. (See
    setRadioButtonExclusive().)

    There are two ways of using a button group:
    \list
    \i The button group is the parent widget of a number of buttons,
    i.e. the button group is the parent argument in the button
    constructor. The buttons are assigned identifiers 0, 1, 2, etc.,
    in the order they are created. A TQButtonGroup can display a frame
    and a title because it inherits TQGroupBox.
    \i The button group is an invisible widget and the contained
    buttons have some other parent widget. In this usage, each button
    must be manually inserted, using insert(), into the button group
    and given an identifier.
    \endlist

    A button can be removed from the group with remove(). A pointer to
    a button with a given id can be obtained using find(). The id of a
    button is available using id(). A button can be set \e on with
    setButton(). The number of buttons in the group is returned by
    count().

    <img src=qbttngrp-m.png> <img src=qbttngrp-w.png>

    \sa TQPushButton, TQCheckBox, TQRadioButton
*/

/*!
    \property TQButtonGroup::exclusive
    \brief whether the button group is exclusive

    If this property is true, then the buttons in the group are
    toggled, and to untoggle a button you must click on another button
    in the group. The default value is false.
*/

/*!
    \property TQButtonGroup::radioButtonExclusive
    \brief whether the radio buttons in the group are exclusive

    If this property is true (the default), the \link TQRadioButton
    radiobuttons\endlink in the group are treated exclusively.
*/

struct TQButtonItem
{
    TQButton *button;
    int	     id;
};


class TQButtonList: public TQPtrList<TQButtonItem>
{
public:
    TQButtonList() {}
   ~TQButtonList() {}
};


typedef TQPtrListIterator<TQButtonItem> TQButtonListIt;


/*!
    Constructs a button group with no title.

    The \a parent and \a name arguments are passed to the TQWidget
    constructor.
*/

TQButtonGroup::TQButtonGroup( TQWidget *parent, const char *name )
    : TQGroupBox( parent, name )
{
    init();
}

/*!
    Constructs a button group with the title \a title.

    The \a parent and \a name arguments are passed to the TQWidget
    constructor.
*/

TQButtonGroup::TQButtonGroup( const TQString &title, TQWidget *parent,
			    const char *name )
    : TQGroupBox( title, parent, name )
{
    init();
}

/*!
    Constructs a button group with no title. Child widgets will be
    arranged in \a strips rows or columns (depending on \a
    orientation).

    The \a parent and \a name arguments are passed to the TQWidget
    constructor.
*/

TQButtonGroup::TQButtonGroup( int strips, Orientation orientation,
			    TQWidget *parent, const char *name )
    : TQGroupBox( strips, orientation, parent, name )
{
    init();
}

/*!
    Constructs a button group with title \a title. Child widgets will
    be arranged in \a strips rows or columns (depending on \a
    orientation).

    The \a parent and \a name arguments are passed to the TQWidget
    constructor.
*/

TQButtonGroup::TQButtonGroup( int strips, Orientation orientation,
			    const TQString &title, TQWidget *parent,
			    const char *name )
    : TQGroupBox( strips, orientation, title, parent, name )
{
    init();
}

/*!
    Initializes the button group.
*/

void TQButtonGroup::init()
{
    buttons = new TQButtonList;
    TQ_CHECK_PTR( buttons );
    buttons->setAutoDelete( true );
    excl_grp = false;
    radio_excl = true;
}

/*! \reimp */

TQButtonGroup::~TQButtonGroup()
{
    TQButtonList * tmp = buttons;
    TQButtonItem *bi = tmp->first();
    buttons = 0;
    while( bi ) {
	bi->button->setGroup(0);
	bi = tmp->next();
    }
    delete tmp;
}

bool TQButtonGroup::isExclusive() const
{
    return excl_grp;
}

void TQButtonGroup::setExclusive( bool enable )
{
    excl_grp = enable;
}


/*!
    Inserts the \a button with the identifier \a id into the button
    group. Returns the button identifier.

    Buttons are normally inserted into a button group automatically by
    passing the button group as the parent when the button is
    constructed. So it is not necessary to manually insert buttons
    that have this button group as their parent widget. An exception
    is when you want custom identifiers instead of the default 0, 1,
    2, etc., or if you want the buttons to have some other parent.

    The button is assigned the identifier \a id or an automatically
    generated identifier. It works as follows: If \a id >= 0, this
    identifier is assigned. If \a id == -1 (default), the identifier
    is equal to the number of buttons in the group. If \a id is any
    other negative integer, for instance -2, a unique identifier
    (negative integer \<= -2) is generated. No button has an id of -1.

    \sa find(), remove(), setExclusive()
*/

int TQButtonGroup::insert( TQButton *button, int id )
{
    if ( button->group() )
	button->group()->remove( button );

    static int seq_no = -2;
    TQButtonItem *bi = new TQButtonItem;
    TQ_CHECK_PTR( bi );

    if ( id < -1 )
	bi->id = seq_no--;
    else if ( id == -1 )
	bi->id = buttons->count();
    else
	bi->id = id;

    bi->button = button;
    button->setGroup(this);
    buttons->append( bi );

    connect( button, TQ_SIGNAL(pressed()) , TQ_SLOT(buttonPressed()) );
    connect( button, TQ_SIGNAL(released()), TQ_SLOT(buttonReleased()) );
    connect( button, TQ_SIGNAL(clicked()) , TQ_SLOT(buttonClicked()) );
    connect( button, TQ_SIGNAL(toggled(bool)) , TQ_SLOT(buttonToggled(bool)) );

    if ( button->isToggleButton() && !button->isOn() &&
	 selected() && (selected()->focusPolicy() & TabFocus) != 0 )
	button->setFocusPolicy( (FocusPolicy)(button->focusPolicy() &
					      ~TabFocus) );

    return bi->id;
}

/*!
    Returns the number of buttons in the group.
*/
int TQButtonGroup::count() const
{
    return buttons->count();
}

/*!
    Removes the \a button from the button group.

    \sa insert()
*/

void TQButtonGroup::remove( TQButton *button )
{
    if ( !buttons )
	return;

    TQButtonListIt it( *buttons );
    TQButtonItem *i;
    while ( (i=it.current()) != 0 ) {
	++it;
	if ( i->button == button ) {
	    buttons->remove( i );
	    button->setGroup(0);
	    button->disconnect( this );
	    return;
	}
    }
}


/*!
    Returns the button with the specified identifier \a id, or 0 if
    the button was not found.
*/

TQButton *TQButtonGroup::find( int id ) const
{
    // introduce a TQButtonListIt if calling anything
    for ( TQButtonItem *i=buttons->first(); i; i=buttons->next() )
	if ( i->id == id )
	    return i->button;
    return 0;
}


/*!
    \fn void TQButtonGroup::pressed( int id )

    This signal is emitted when a button in the group is \link
    TQButton::pressed() pressed\endlink. The \a id argument is the
    button's identifier.

    \sa insert()
*/

/*!
    \fn void TQButtonGroup::released( int id )

    This signal is emitted when a button in the group is \link
    TQButton::released() released\endlink. The \a id argument is the
    button's identifier.

    \sa insert()
*/

/*!
    \fn void TQButtonGroup::clicked( int id )

    This signal is emitted when a button in the group is \link
    TQButton::clicked() clicked\endlink. The \a id argument is the
    button's identifier.

    \sa insert()
*/


/*!
  \internal
  This slot is activated when one of the buttons in the group emits the
  TQButton::pressed() signal.
*/

void TQButtonGroup::buttonPressed()
{
    // introduce a TQButtonListIt if calling anything
    int id = -1;
    TQButton *bt = (TQButton *)sender();		// object that sent the signal
    for ( TQButtonItem *i=buttons->first(); i; i=buttons->next() )
	if ( bt == i->button ) {		// button was clicked
	    id = i->id;
	    break;
	}
    if ( id != -1 )
	emit pressed( id );
}

/*!
  \internal
  This slot is activated when one of the buttons in the group emits the
  TQButton::released() signal.
*/

void TQButtonGroup::buttonReleased()
{
    // introduce a TQButtonListIt if calling anything
    int id = -1;
    TQButton *bt = (TQButton *)sender();		// object that sent the signal
    for ( TQButtonItem *i=buttons->first(); i; i=buttons->next() )
	if ( bt == i->button ) {		// button was clicked
	    id = i->id;
	    break;
	}
    if ( id != -1 )
	emit released( id );
}

/*!
  \internal
  This slot is activated when one of the buttons in the group emits the
  TQButton::clicked() signal.
*/

void TQButtonGroup::buttonClicked()
{
    // introduce a TQButtonListIt if calling anything
    int id = -1;
    TQButton *bt = ::tqt_cast<TQButton*>(sender());		// object that sent the signal
#if defined(QT_CHECK_NULL)
    Q_ASSERT( bt );
#endif
    for ( TQButtonItem *i=buttons->first(); i; i=buttons->next() ) {
	if ( bt == i->button ) {			// button was clicked
	    id = i->id;
	    break;
	}
    }
    if ( id != -1 )
	emit clicked( id );
}


/*!
  \internal
  This slot is activated when one of the buttons in the group emits the
  TQButton::toggled() signal.
*/

void TQButtonGroup::buttonToggled( bool on )
{
    // introduce a TQButtonListIt if calling anything
    if ( !on || ( !excl_grp && !radio_excl ) )
	return;
    TQButton *bt = ::tqt_cast<TQButton*>(sender());		// object that sent the signal
#if defined(QT_CHECK_NULL)
    Q_ASSERT( bt );
    Q_ASSERT( bt->isToggleButton() );
#endif

    if ( !excl_grp && !::tqt_cast<TQRadioButton*>(bt) )
	return;
    TQButtonItem * i = buttons->first();
    bool hasTabFocus = false;

    while( i != 0 && hasTabFocus == false ) {
	if ( ( excl_grp || ::tqt_cast<TQRadioButton*>(i->button) ) &&
	     (i->button->focusPolicy() & TabFocus) )
	    hasTabFocus = true;
	i = buttons->next();
    }

    i = buttons->first();
    while( i ) {
	if ( bt != i->button &&
	     i->button->isToggleButton() &&
	     i->button->isOn() &&
	     ( excl_grp || ::tqt_cast<TQRadioButton*>(i->button) ) )
	    i->button->setOn( false );
	if ( ( excl_grp || ::tqt_cast<TQRadioButton*>(i->button) ) &&
	     i->button->isToggleButton() &&
	     hasTabFocus )
	    i->button->setFocusPolicy( (FocusPolicy)(i->button->focusPolicy() &
						     ~TabFocus) );
	i = buttons->next();
    }

    if ( hasTabFocus )
	bt->setFocusPolicy( (FocusPolicy)(bt->focusPolicy() | TabFocus) );
}



void TQButtonGroup::setButton( int id )
{
    TQButton * b = find( id );
    if ( b )
	b->setOn( true );
}

void TQButtonGroup::setRadioButtonExclusive( bool on)
{
    radio_excl = on;
}


/*!
    Moves the keyboard focus according to \a key, and if appropriate
    checks the new focus item.

    This function does nothing unless the keyboard focus points to one
    of the button group members and \a key is one of \c Key_Up, \c
    Key_Down, \c Key_Left and \c Key_Right.
*/

void TQButtonGroup::moveFocus( int key )
{
    TQWidget * f = tqApp->focusWidget();

    TQButtonItem * i;
    i = buttons->first();
    while( i && i->button != f )
	i = buttons->next();

    if ( !i || !i->button )
	return;

    TQWidget * candidate = 0;
    int bestScore = -1;

    TQPoint goal( f->mapToGlobal( f->geometry().center() ) );

    i = buttons->first();
    while( i && i->button ) {
	if ( i->button != f &&
	     i->button->isEnabled() ) {
	    TQPoint p(i->button->mapToGlobal(i->button->geometry().center()));
	    int score = (p.y() - goal.y())*(p.y() - goal.y()) +
			(p.x() - goal.x())*(p.x() - goal.x());
	    bool betterScore = score < bestScore || !candidate;
	    switch( key ) {
	    case Key_Up:
		if ( p.y() < goal.y() && betterScore ) {
		    if ( TQABS( p.x() - goal.x() ) < TQABS( p.y() - goal.y() ) ) {
			candidate = i->button;
			bestScore = score;
		    } else if ( i->button->x() == f->x() ) {
			candidate = i->button;
			bestScore = score/2;
		    }
		}
		break;
	    case Key_Down:
		if ( p.y() > goal.y() && betterScore ) {
		    if ( TQABS( p.x() - goal.x() ) < TQABS( p.y() - goal.y() ) ) {
			candidate = i->button;
			bestScore = score;
		    } else if ( i->button->x() == f->x() ) {
			candidate = i->button;
			bestScore = score/2;
		    }
		}
		break;
	    case Key_Left:
		if ( p.x() < goal.x() && betterScore ) {
		    if ( TQABS( p.y() - goal.y() ) < TQABS( p.x() - goal.x() ) ) {
			candidate = i->button;
			bestScore = score;
		    } else if ( i->button->y() == f->y() ) {
			candidate = i->button;
			bestScore = score/2;
		    }
		}
		break;
	    case Key_Right:
		if ( p.x() > goal.x() && betterScore ) {
		    if ( TQABS( p.y() - goal.y() ) < TQABS( p.x() - goal.x() ) ) {
			candidate = i->button;
			bestScore = score;
		    } else if ( i->button->y() == f->y() ) {
			candidate = i->button;
			bestScore = score/2;
		    }
		}
		break;
	    }
	}
	i = buttons->next();
    }

    TQButton *buttoncand = ::tqt_cast<TQButton*>(candidate);
    if ( buttoncand && ::tqt_cast<TQButton*>(f) &&
	 ((TQButton*)f)->isOn() &&
	 buttoncand->isToggleButton() &&
	 ( isExclusive() || ( ::tqt_cast<TQRadioButton*>(f) &&
			      ::tqt_cast<TQRadioButton*>(candidate)))) {
	if ( f->focusPolicy() & TabFocus ) {
	    f->setFocusPolicy( (FocusPolicy)(f->focusPolicy() & ~TabFocus) );
	    candidate->setFocusPolicy( (FocusPolicy)(candidate->focusPolicy()|
						     TabFocus) );
	}
	buttoncand->setOn( true );
	buttoncand->animateClick();
	buttoncand->animateTimeout(); // ### crude l&f hack
    }

    if ( candidate ) {
	if (key == Key_Up || key == Key_Left)
	    TQFocusEvent::setReason(TQFocusEvent::Backtab);
	else
	    TQFocusEvent::setReason(TQFocusEvent::Tab);
	candidate->setFocus();
	TQFocusEvent::resetReason();
    }
}


/*!
    Returns the selected toggle button if exactly one is selected;
    otherwise returns 0.

    \sa selectedId()
*/

TQButton * TQButtonGroup::selected() const
{
    if ( !buttons )
	return 0;
    TQButtonListIt it( *buttons );
    TQButtonItem *i;
    TQButton *candidate = 0;

    while ( (i = it.current()) != 0 ) {
	++it;
	if ( i->button && i->button->isToggleButton() && i->button->isOn() ) {
	    if ( candidate != 0 )
		return 0;
	    candidate = i->button;
	}
    }
    return candidate;
}

/*!
    \property TQButtonGroup::selectedId
    \brief the selected toggle button

    The toggle button is specified as an ID.

    If no toggle button is selected, this property holds -1.

    If setButton() is called on an exclusive group, the button with
    the given id will be set to on and all the others will be set to
    off.

    \sa selected()
*/

int TQButtonGroup::selectedId() const
{
    return id( selected() );
}


/*!
    Returns the id of \a button, or -1 if \a button is not a member of
    this group.

    \sa selectedId();
*/

int TQButtonGroup::id( TQButton * button ) const
{
    TQButtonItem *i = buttons->first();
    while ( i && i->button != button )
	i = buttons->next();
    return i ? i->id : -1;
}


/*!
    \reimp
*/
bool TQButtonGroup::event( TQEvent * e )
{
    if ( e->type() == TQEvent::ChildInserted ) {
	TQChildEvent * ce = (TQChildEvent *) e;
	if ( radio_excl && ::tqt_cast<TQRadioButton*>(ce->child()) ) {
	    TQButton * button = (TQButton *) ce->child();
	    if ( button->isToggleButton() && !button->isOn() &&
		 selected() && (selected()->focusPolicy() & TabFocus) != 0 )
		button->setFocusPolicy( (FocusPolicy)(button->focusPolicy() &
					      ~TabFocus) );
	}
    }
    return TQGroupBox::event( e );
}
#endif
