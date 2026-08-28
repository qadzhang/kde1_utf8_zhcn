/****************************************************************************
**
** Implementation of TQAction class
**
** Copyright (C) 2000-2008 Trolltech ASA.  All rights reserved.
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

#include "ntqaction.h"

#ifndef TQT_NO_ACTION

#include "ntqtoolbar.h"
#include "ntqptrlist.h"
#include "ntqpopupmenu.h"
#include "ntqaccel.h"
#include "ntqtoolbutton.h"
#include "ntqcombobox.h"
#include "ntqtooltip.h"
#include "ntqwhatsthis.h"
#include "ntqstatusbar.h"
#include "ntqobjectlist.h"


/*!
    \class TQAction ntqaction.h
    \brief The TQAction class provides an abstract user interface
    action that can appear both in menus and tool bars.

    \ingroup basic
    \ingroup application
    \mainclass

    In GUI applications many commands can be invoked via a menu
    option, a toolbar button and a keyboard accelerator. Since the
    same action must be performed regardless of how the action was
    invoked, and since the menu and toolbar should be kept in sync, it
    is useful to represent a command as an \e action. An action can be
    added to a menu and a toolbar and will automatically keep them in
    sync. For example, if the user presses a Bold toolbar button the
    Bold menu item will automatically be checked.

    A TQAction may contain an icon, a menu text, an accelerator, a
    status text, a whats this text and a tool tip. Most of these can
    be set in the constructor. They can also be set independently with
    setIconSet(), setText(), setMenuText(), setToolTip(),
    setStatusTip(), setWhatsThis() and setAccel().

    An action may be a toggle action e.g. a Bold toolbar button, or a
    command action, e.g. 'Open File' to invoke an open file dialog.
    Toggle actions emit the toggled() signal when their state changes.
    Both command and toggle actions emit the activated() signal when
    they are invoked. Use setToggleAction() to set an action's toggled
    status. To see if an action is a toggle action use
    isToggleAction(). A toggle action may be "on", isOn() returns
    true, or "off", isOn() returns false.

    Actions are added to widgets (menus or toolbars) using addTo(),
    and removed using removeFrom().

    Once a TQAction has been created it should be added to the relevant
    menu and toolbar and then connected to the slot which will perform
    the action. For example:

    \quotefile action/application.cpp
    \skipto TQPixmap( fileopen
    \printuntil connect

    We create a "File Save" action with a menu text of "&Save" and
    \e{Ctrl+S} as the keyboard accelerator. We connect the
    fileSaveAction's activated() signal to our own save() slot. Note
    that at this point there is no menu or toolbar action, we'll add
    them next:

    \skipto new TQToolBar
    \printline
    \skipto fileSaveAction->addTo
    \printline
    \skipto new TQPopupMenu
    \printuntil insertItem
    \skipto fileSaveAction->addTo
    \printline

    We create a toolbar and add our fileSaveAction to it. Similarly we
    create a menu, add a top-level menu item, and add our
    fileSaveAction.

    We recommend that actions are created as children of the window
    that they are used in. In most cases actions will be children of
    the application's main window.

    To prevent recursion, don't create an action as a child of a
    widget that the action is later added to.
*/

class TQActionPrivate
{
public:
    TQActionPrivate(TQAction *act);
    ~TQActionPrivate();
    TQIconSet *iconset;
    TQString text;
    TQString menutext;
    TQString tooltip;
    TQString statustip;
    TQString whatsthis;
#ifndef TQT_NO_ACCEL
    TQKeySequence key;
    TQAccel* accel;
    int accelid;
#endif
    uint enabled : 1;
    uint visible : 1;
    uint toggleaction : 1;
    uint on : 1;
    uint forceDisabled : 1;
    uint forceInvisible : 1;
#ifndef TQT_NO_TOOLTIP
    TQToolTipGroup tipGroup;
#endif
    TQActionGroupPrivate* d_group;
    TQAction *action;

    struct MenuItem {
	MenuItem():popup(0),id(0){}
	TQPopupMenu* popup;
	int id;
    };
    // ComboItem is only necessary for actions that are
    // in dropdown/exclusive actiongroups. The actiongroup
    // will clean this up
    struct ComboItem {
	ComboItem():combo(0), id(0) {}
	TQComboBox *combo;
	int id;
    };
    TQPtrList<MenuItem> menuitems;
    TQPtrList<TQToolButton> toolbuttons;
    TQPtrList<ComboItem> comboitems;

    enum Update { Icons = 1, Visibility = 2, State = 4, EverythingElse = 8 };
    void update( uint upd = EverythingElse );

    TQString menuText() const;
    TQString toolTip() const;
    TQString statusTip() const;
};

TQActionPrivate::TQActionPrivate(TQAction *act)
    : iconset( 0 ),
#ifndef TQT_NO_ACCEL
      key( 0 ), accel( 0 ), accelid( 0 ),
#endif
      enabled( true ), visible( true ), toggleaction( false ), on( false ),
      forceDisabled( false ), forceInvisible( false ),
#ifndef TQT_NO_TOOLTIP
      tipGroup( 0 ),
#endif
      d_group( 0 ), action(act)
{
    menuitems.setAutoDelete( true );
    comboitems.setAutoDelete( true );
#ifndef TQT_NO_TOOLTIP
    tipGroup.setDelay( false );
#endif
}

TQActionPrivate::~TQActionPrivate()
{
    TQPtrListIterator<TQToolButton> ittb( toolbuttons );
    TQToolButton *tb;

    while ( ( tb = ittb.current() ) ) {
	++ittb;
	delete tb;
    }

    TQPtrListIterator<TQActionPrivate::MenuItem> itmi( menuitems);
    TQActionPrivate::MenuItem* mi;
    while ( ( mi = itmi.current() ) ) {
	++itmi;
	TQPopupMenu* menu = mi->popup;
	if ( menu->findItem( mi->id ) )
	    menu->removeItem( mi->id );
    }

    TQPtrListIterator<TQActionPrivate::ComboItem> itci(comboitems);
    TQActionPrivate::ComboItem* ci;
    while ( ( ci = itci.current() ) ) {
	++itci;
	TQComboBox* combo = ci->combo;
	combo->clear();
	TQActionGroup *group = ::tqt_cast<TQActionGroup*>(action->parent());
	TQObjectList *siblings = group ? group->queryList("TQAction") : 0;
	if (siblings) {
	    TQObjectListIt it(*siblings);
	    while (it.current()) {
		TQAction *sib = ::tqt_cast<TQAction*>(it.current());
		++it;
		sib->removeFrom(combo);
	    }
	    it = TQObjectListIt(*siblings);
	    while (it.current()) {
		TQAction *sib = ::tqt_cast<TQAction*>(it.current());
		++it;
		if (sib == action)
		    continue;
		sib->addTo(combo);
	    }
	}
	delete siblings;
    }

#ifndef TQT_NO_ACCEL
    delete accel;
#endif
    delete iconset;
}

class TQActionGroupPrivate
{
public:
    uint exclusive: 1;
    uint dropdown: 1;
    TQPtrList<TQAction> actions;
    TQAction* selected;
    TQAction* separatorAction;

    struct MenuItem {
	MenuItem():popup(0),id(0){}
	TQPopupMenu* popup;
	int id;
    };

    TQPtrList<TQComboBox> comboboxes;
    TQPtrList<TQToolButton> menubuttons;
    TQPtrList<MenuItem> menuitems;
    TQPtrList<TQPopupMenu> popupmenus;

    void update( const TQActionGroup * );
};

void TQActionPrivate::update( uint upd )
{
    for ( TQPtrListIterator<MenuItem> it( menuitems); it.current(); ++it ) {
	MenuItem* mi = it.current();
	TQString t = menuText();
#ifndef TQT_NO_ACCEL
	if ( key )
	    t += '\t' + TQAccel::keyToString( key );
#endif
	if ( upd & State ) {
	    mi->popup->setItemEnabled( mi->id, enabled );
	    if ( toggleaction )
		mi->popup->setItemChecked( mi->id, on );
	}
	if ( upd & Visibility )
	    mi->popup->setItemVisible( mi->id, visible );

	if ( upd & Icons ) {
	    if ( iconset )
		mi->popup->changeItem( mi->id, *iconset, t );
	    else
		mi->popup->changeItem( mi->id, TQIconSet(), t );
	}
	if ( upd & EverythingElse ) {
	    mi->popup->changeItem( mi->id, t );
	    if ( !whatsthis.isEmpty() )
		mi->popup->setWhatsThis( mi->id, whatsthis );
	    if ( toggleaction ) {
		mi->popup->setCheckable( true );
		mi->popup->setItemChecked( mi->id, on );
	    }
	}
    }
    for ( TQPtrListIterator<TQToolButton> it2(toolbuttons); it2.current(); ++it2 ) {
	TQToolButton* btn = it2.current();
	if ( upd & State ) {
	    btn->setEnabled( enabled );
	    if ( toggleaction )
		btn->setOn( on );
	}
	if ( upd & Visibility )
	    visible ? btn->show() : btn->hide();
	if ( upd & Icons ) {
	    if ( iconset )
		btn->setIconSet( *iconset );
	    else
		btn->setIconSet( TQIconSet() );
	}
	if ( upd & EverythingElse ) {
	    btn->setToggleButton( toggleaction );
	    if ( !text.isEmpty() )
		btn->setTextLabel( text, false );
#ifndef TQT_NO_TOOLTIP
	    TQToolTip::remove( btn );
	    TQToolTip::add( btn, toolTip(), &tipGroup, statusTip() );
#endif
#ifndef TQT_NO_WHATSTHIS
	    TQWhatsThis::remove( btn );
	    if ( !whatsthis.isEmpty() )
		TQWhatsThis::add( btn, whatsthis );
#endif
	}
    }
#ifndef TQT_NO_ACCEL
    if ( accel ) {
	accel->setEnabled( enabled && visible );
	if ( !whatsthis.isEmpty() )
	    accel->setWhatsThis( accelid, whatsthis );
    }
#endif
    // Only used by actiongroup
    for ( TQPtrListIterator<ComboItem> it3( comboitems ); it3.current(); ++it3 ) {
	ComboItem *ci = it3.current();
	if ( !ci->combo )
	    return;
	if ( iconset )
	    ci->combo->changeItem( iconset->pixmap(), text, ci->id );
	else
	    ci->combo->changeItem( text, ci->id );
    }
}

TQString TQActionPrivate::menuText() const
{
    if ( menutext.isNull() ) {
	TQString t(text);
	t.replace('&', "&&");
	return t;
    }
    return menutext;
}

TQString TQActionPrivate::toolTip() const
{
    if ( tooltip.isNull() ) {
#ifndef TQT_NO_ACCEL
	if ( accel )
	    return text + " (" + TQAccel::keyToString( accel->key( accelid )) + ")";
#endif
	return text;
    }
    return tooltip;
}

TQString TQActionPrivate::statusTip() const
{
    if ( statustip.isNull() )
	return toolTip();
    return statustip;
}

/*
  internal: guesses a descriptive text from a menu text
 */
static TQString qt_stripMenuText( TQString s )
{
    s.remove( TQString::fromLatin1("...") );
    s.remove( TQChar('&' ) );
    return s.stripWhiteSpace();
}

/*!
    Constructs an action called \a name with parent \a parent.

    If \a parent is a TQActionGroup, the new action inserts itself into
    \a parent.

    For accelerators and status tips to work, \a parent must either be
    a widget, or an action group whose parent is a widget.

    \warning To prevent recursion, don't create an action as a child
    of a widget that the action is later added to.
*/
TQAction::TQAction( TQObject* parent, const char* name )
    : TQObject( parent, name )
{
    d = new TQActionPrivate(this);
    init();
}

/*! \obsolete
    Constructs an action called \a name with parent \a parent.

    If \a toggle is true the action will be a toggle action, otherwise
    it will be a command action.

    If \a parent is a TQActionGroup, the new action inserts itself into
    \a parent.

    For accelerators and status tips to work, \a parent must either be
    a widget, or an action group whose parent is a widget.
*/
TQAction::TQAction( TQObject* parent, const char* name, bool toggle )
    : TQObject( parent, name )
{
    d = new TQActionPrivate(this);
    d->toggleaction = toggle;
    init();
}


#ifndef TQT_NO_ACCEL

/*!
    This constructor creates an action with the following properties:
    the icon or iconset \a icon, the menu text \a menuText and
    keyboard accelerator \a accel. It is a child of \a parent and
    called \a name.

    If \a parent is a TQActionGroup, the action automatically becomes
    a member of it.

    For accelerators and status tips to work, \a parent must either be
    a widget, or an action group whose parent is a widget.

    The action uses a stripped version of \a menuText (e.g. "\&Menu
    Option..." becomes "Menu Option") as descriptive text for
    toolbuttons. You can override this by setting a specific
    description with setText(). The same text and \a accel will be
    used for tool tips and status tips unless you provide text for
    these using setToolTip() and setStatusTip().

    Call setToggleAction(true) to make the action a toggle action.

    \warning To prevent recursion, don't create an action as a child
    of a widget that the action is later added to.
*/
TQAction::TQAction( const TQIconSet& icon, const TQString& menuText, TQKeySequence accel,
		  TQObject* parent, const char* name )
    : TQObject( parent, name )
{
    d = new TQActionPrivate(this);
    if ( !icon.isNull() )
	setIconSet( icon );
    d->text = qt_stripMenuText( menuText );
    d->menutext = menuText;
    setAccel( accel );
    init();
}

/*!
    This constructor results in an icon-less action with the the menu
    text \a menuText and keyboard accelerator \a accel. It is a child
    of \a parent and called \a name.

    If  \a parent is a TQActionGroup, the action automatically becomes
    a member of it.

    For accelerators and status tips to work, \a parent must either be
    a widget, or an action group whose parent is a widget.

    The action uses a stripped version of \a menuText (e.g. "\&Menu
    Option..." becomes "Menu Option") as descriptive text for
    toolbuttons. You can override this by setting a specific
    description with setText(). The same text and \a accel will be
    used for tool tips and status tips unless you provide text for
    these using setToolTip() and setStatusTip().

    Call setToggleAction(true) to make the action a toggle action.

    \warning To prevent recursion, don't create an action as a child
    of a widget that the action is later added to.
*/
TQAction::TQAction( const TQString& menuText, TQKeySequence accel,
		  TQObject* parent, const char* name )
    : TQObject( parent, name )
{
    d = new TQActionPrivate(this);
    d->text = qt_stripMenuText( menuText );
    d->menutext = menuText;
    setAccel( accel );
    init();
}

/*! \obsolete
    This constructor creates an action with the following properties:
    the description \a text, the icon or iconset \a icon, the menu
    text \a menuText and keyboard accelerator \a accel. It is a child
    of \a parent and called \a name. If \a toggle is true the action
    will be a toggle action, otherwise it will be a command action.

    If  \a parent is a TQActionGroup, the action automatically becomes
    a member of it.

    For accelerators and status tips to work, \a parent must either be
    a widget, or an action group whose parent is a widget.

    The \a text and \a accel will be used for tool tips and status
    tips unless you provide specific text for these using setToolTip()
    and setStatusTip().
*/
TQAction::TQAction( const TQString& text, const TQIconSet& icon, const TQString& menuText, TQKeySequence accel, TQObject* parent, const char* name, bool toggle )
    : TQObject( parent, name )
{
    d = new TQActionPrivate(this);
    d->toggleaction = toggle;
    if ( !icon.isNull() )
	setIconSet( icon );

    d->text = text;
    d->menutext = menuText;
    setAccel( accel );
    init();
}

/*! \obsolete
    This constructor results in an icon-less action with the
    description \a text, the menu text \a menuText and the keyboard
    accelerator \a accel. Its parent is \a parent and it is called \a
    name. If \a toggle is true the action will be a toggle action,
    otherwise it will be a command action.

    The action automatically becomes a member of \a parent if \a
    parent is a TQActionGroup.

    For accelerators and status tips to work, \a parent must either be
    a widget, or an action group whose parent is a widget.

    The \a text and \a accel will be used for tool tips and status
    tips unless you provide specific text for these using setToolTip()
    and setStatusTip().
*/
TQAction::TQAction( const TQString& text, const TQString& menuText, TQKeySequence accel, TQObject* parent, const char* name, bool toggle )
    : TQObject( parent, name )
{
    d = new TQActionPrivate(this);
    d->toggleaction = toggle;
    d->text = text;
    d->menutext = menuText;
    setAccel( accel );
    init();
}
#endif

/*!
  \internal
*/
void TQAction::init()
{
    if ( ::tqt_cast<TQActionGroup*>(parent()) )
	((TQActionGroup*) parent())->add( this );		// insert into action group
}

/*!
    Destroys the object and frees allocated resources.
*/

TQAction::~TQAction()
{
    delete d;
}

/*!
    \property TQAction::iconSet
    \brief  the action's icon

    The icon is used as the tool button icon and in the menu to the
    left of the menu text. There is no default icon.

    If a null icon (TQIconSet::isNull() is passed into this function,
    the icon of the action is cleared.

    (See the action/toggleaction/toggleaction.cpp example.)

*/
void TQAction::setIconSet( const TQIconSet& icon )
{
    TQIconSet *i = d->iconset;
    if ( !icon.isNull() )
	d->iconset = new TQIconSet( icon );
    else
	d->iconset = 0;
    delete i;
    d->update( TQActionPrivate::Icons );
}

TQIconSet TQAction::iconSet() const
{
    if ( d->iconset )
	return *d->iconset;
    return TQIconSet();
}

/*!
    \property TQAction::text
    \brief the action's descriptive text

    If \l TQMainWindow::usesTextLabel is true, the text appears as a
    label in the relevant tool button. It also serves as the default
    text in menus and tool tips if these have not been specifically
    defined. There is no default text.

    \sa setMenuText() setToolTip() setStatusTip()
*/
void TQAction::setText( const TQString& text )
{
    d->text = text;
    d->update();
}

TQString TQAction::text() const
{
    return d->text;
}


/*!
    \property TQAction::menuText
    \brief the action's menu text

    If the action is added to a menu the menu option will consist of
    the icon (if there is one), the menu text and the accelerator (if
    there is one). If the menu text is not explicitly set in the
    constructor or by using setMenuText() the action's description
    text will be used as the menu text. There is no default menu text.

    \sa text
*/
void TQAction::setMenuText( const TQString& text )
{
    if ( d->menutext == text )
	return;

    d->menutext = text;
    d->update();
}

TQString TQAction::menuText() const
{
    return d->menuText();
}

/*!
    \property TQAction::toolTip
    \brief the action's tool tip

    This text is used for the tool tip. If no status tip has been set
    the tool tip will be used for the status tip.

    If no tool tip is specified the action's text is used, and if that
    hasn't been specified the description text is used as the tool tip
    text.

    There is no default tool tip text.

    \sa setStatusTip() setAccel()
*/
void TQAction::setToolTip( const TQString& tip )
{
    if ( d->tooltip == tip )
	return;

    d->tooltip = tip;
    d->update();
}

TQString TQAction::toolTip() const
{
    return d->toolTip();
}

/*!
    \property TQAction::statusTip
    \brief the action's status tip

    The statusTip is displayed on all status bars that this action's
    toplevel parent widget provides.

    If no status tip is defined, the action uses the tool tip text.

    There is no default statusTip text.

    \sa setStatusTip() setToolTip()
*/
//#### Please reimp for TQActionGroup!
//#### For consistency reasons even action groups should show
//#### status tips (as they already do with tool tips)
//#### Please change TQActionGroup class doc appropriately after
//#### reimplementation.
void TQAction::setStatusTip( const TQString& tip )
{
    if ( d->statustip == tip )
	return;

    d->statustip = tip;
    d->update();
}

TQString TQAction::statusTip() const
{
    return d->statusTip();
}

/*!
    \property TQAction::whatsThis
    \brief the action's "What's This?" help text

    The whats this text is used to provide a brief description of the
    action. The text may contain rich text (HTML-like tags -- see
    TQStyleSheet for the list of supported tags). There is no default
    "What's This" text.

    \sa TQWhatsThis
*/
void TQAction::setWhatsThis( const TQString& whatsThis )
{
    if ( d->whatsthis == whatsThis )
	return;
    d->whatsthis = whatsThis;
    d->update();
}

TQString TQAction::whatsThis() const
{
    return d->whatsthis;
}


#ifndef TQT_NO_ACCEL
/*!
    \property TQAction::accel
    \brief the action's accelerator key

    The keycodes can be found in \l TQt::Key and \l TQt::Modifier. There
    is no default accelerator key.
*/
//#### Please reimp for TQActionGroup!
//#### For consistency reasons even TQActionGroups should respond to
//#### their accelerators and e.g. open the relevant submenu.
//#### Please change appropriate TQActionGroup class doc after
//#### reimplementation.
void TQAction::setAccel( const TQKeySequence& key )
{
    if ( d->key == key )
	return;

    d->key = key;
    delete d->accel;
    d->accel = 0;

    if ( !(int)key ) {
	d->update();
	return;
    }

    TQObject* p = parent();
    while ( p && !p->isWidgetType() ) {
	p = p->parent();
    }
    if ( p ) {
	d->accel = new TQAccel( (TQWidget*)p, this, "qt_action_accel" );
	d->accelid = d->accel->insertItem( d->key );
	d->accel->connectItem( d->accelid, this, TQ_SLOT( internalActivation() ) );
    }
#if defined(QT_CHECK_STATE)
    else
	tqWarning( "TQAction::setAccel() (%s) requires widget in parent chain", name() );
#endif
    d->update();
}


TQKeySequence TQAction::accel() const
{
    return d->key;
}
#endif


/*!
    \property TQAction::toggleAction
    \brief whether the action is a toggle action

    A toggle action is one which has an on/off state. For example a
    Bold toolbar button is either on or off. An action which is not a
    toggle action is a command action; a command action is simply
    executed, e.g. file save. This property's default is false.

    In some situations, the state of one toggle action should depend
    on the state of others. For example, "Left Align", "Center" and
    "Right Align" toggle actions are mutually exclusive. To achieve
    exclusive toggling, add the relevant toggle actions to a
    TQActionGroup with the \l TQActionGroup::exclusive property set to
    true.
*/
void TQAction::setToggleAction( bool enable )
{
    if ( enable == (bool)d->toggleaction )
	return;

    if ( !enable )
	d->on = false;

    d->toggleaction = enable;
    d->update();
}

bool TQAction::isToggleAction() const
{
    return d->toggleaction;
}

/*!
    Activates the action and executes all connected slots.
    This only works for actions that are not toggle action.

    \sa toggle()
*/
void TQAction::activate()
{
    if ( isToggleAction() ) {
#if defined(QT_CHECK_STATE)
	tqWarning( "TQAction::%s() (%s) Toggle actions "
		  "can not be activated", "activate", name() );
#endif
	return;
    }
    emit activated();
}

/*!
    Toggles the state of a toggle action.

    \sa on, activate(), toggled(), isToggleAction()
*/
void TQAction::toggle()
{
    if ( !isToggleAction() ) {
#if defined(QT_CHECK_STATE)
	tqWarning( "TQAction::%s() (%s) Only toggle actions "
		  "can be switched", "toggle", name() );
#endif
	return;
    }
    setOn( !isOn() );
}

/*!
    \property TQAction::on
    \brief whether a toggle action is on

    This property is always on (true) for command actions and
    \l{TQActionGroup}s; setOn() has no effect on them. For action's
    where isToggleAction() is true, this property's default value is
    off (false).

    \sa toggleAction
*/
void TQAction::setOn( bool enable )
{
    if ( !isToggleAction() ) {
#if defined(QT_CHECK_STATE)
	if ( enable )
	    tqWarning( "TQAction::%s() (%s) Only toggle actions "
		      "can be switched", "setOn", name() );
#endif
	return;
    }
    if ( enable == (bool)d->on )
	return;
    d->on = enable;
    d->update( TQActionPrivate::State );
    emit toggled( enable );
}

bool TQAction::isOn() const
{
    return d->on;
}

/*!
    \property TQAction::enabled
    \brief whether the action is enabled

    Disabled actions can't be chosen by the user. They don't disappear
    from the menu/tool bar but are displayed in a way which indicates
    that they are unavailable, e.g. they might be displayed grayed
    out.

    What's this? help on disabled actions is still available provided
    the \l TQAction::whatsThis property is set.
*/
void TQAction::setEnabled( bool enable )
{
    d->forceDisabled = !enable;

    if ( (bool)d->enabled == enable )
	return;

    d->enabled = enable;
    d->update( TQActionPrivate::State );
}

bool TQAction::isEnabled() const
{
    return d->enabled;
}

/*!
    Disables the action if \a disable is true; otherwise
    enables the action.

    See the \l enabled documentation for more information.
*/
void TQAction::setDisabled( bool disable )
{
    setEnabled( !disable );
}

/*!
    \property TQAction::visible
    \brief whether the action can be seen (e.g. in menus and toolbars)

    If \e visible is true the action can be seen (e.g. in menus and
    toolbars) and chosen by the user; if \e visible is false the
    action cannot be seen or chosen by the user.

    Actions which are not visible are \e not grayed out; they do not
    appear at all.
*/
void TQAction::setVisible( bool visible )
{
    d->forceInvisible = !visible;

    if ( (bool)d->visible == visible )
	return;
    d->visible = visible;
    d->update( TQActionPrivate::Visibility );
#if (TQT_VERSION-0 >= 0x040000)
#error "TQAction::setVisible function wants to be virtual. Also add virtual change() function"
#endif
    if ( d->d_group ) //### this function wants to be virtual in 4.0
	d->d_group->update( (TQActionGroup*) this );
}

/*
    Returns true if the action is visible (e.g. in menus and
    toolbars); otherwise returns false.
*/
bool TQAction::isVisible() const
{
    return d->visible;
}

/*! \internal
*/
void TQAction::internalActivation()
{
    if ( isToggleAction() )
	setOn( !isOn() );
    emit activated();
}

/*! \internal
*/
void TQAction::toolButtonToggled( bool on )
{
    if ( !isToggleAction() )
	return;
    setOn( on );
}

/*!
    Adds this action to widget \a w.

    Currently actions may be added to TQToolBar and TQPopupMenu widgets.

    An action added to a tool bar is automatically displayed as a tool
    button; an action added to a pop up menu appears as a menu option.

    addTo() returns true if the action was added successfully and
    false otherwise. (If \a w is not a TQToolBar or TQPopupMenu the
    action will not be added and false will be returned.)

    \sa removeFrom()
*/
bool TQAction::addTo( TQWidget* w )
{
#ifndef TQT_NO_TOOLBAR
    if ( ::tqt_cast<TQToolBar*>(w) ) {
	if ( !qstrcmp( name(), "qt_separator_action" ) ) {
	    ((TQToolBar*)w)->addSeparator();
	} else {
	    TQCString bname = name() + TQCString( "_action_button" );
	    TQToolButton* btn = new TQToolButton( (TQToolBar*) w, bname );
	    addedTo( btn, w );
	    btn->setToggleButton( d->toggleaction );
	    d->toolbuttons.append( btn );
	    if ( d->iconset )
		btn->setIconSet( *d->iconset );
	    d->update( TQActionPrivate::State | TQActionPrivate::Visibility | TQActionPrivate::EverythingElse ) ;
	    connect( btn, TQ_SIGNAL( clicked() ), this, TQ_SIGNAL( activated() ) );
	    connect( btn, TQ_SIGNAL( toggled(bool) ), this, TQ_SLOT( toolButtonToggled(bool) ) );
	    connect( btn, TQ_SIGNAL( destroyed() ), this, TQ_SLOT( objectDestroyed() ) );
#ifndef TQT_NO_TOOLTIP
	    connect( &(d->tipGroup), TQ_SIGNAL(showTip(const TQString&)), this, TQ_SLOT(showStatusText(const TQString&)) );
	    connect( &(d->tipGroup), TQ_SIGNAL(removeTip()), this, TQ_SLOT(clearStatusText()) );
#endif
	}
    } else
#endif
    if ( ::tqt_cast<TQPopupMenu*>(w) ) {
	TQActionPrivate::MenuItem* mi = new TQActionPrivate::MenuItem;
	mi->popup = (TQPopupMenu*) w;
	TQIconSet* diconset = d->iconset;
	if ( !qstrcmp( name(), "qt_separator_action" ) )
	    mi->id = ((TQPopupMenu*)w)->insertSeparator();
	else if ( diconset )
	    mi->id = mi->popup->insertItem( *diconset, TQString::fromLatin1("") );
	else
	    mi->id = mi->popup->insertItem( TQString::fromLatin1("") );
	addedTo( mi->popup->indexOf( mi->id ), mi->popup );
	mi->popup->connectItem( mi->id, this, TQ_SLOT(internalActivation()) );
	d->menuitems.append( mi );
	d->update( TQActionPrivate::State | TQActionPrivate::Visibility | TQActionPrivate::EverythingElse ) ;
	w->topLevelWidget()->className();
	connect( mi->popup, TQ_SIGNAL(highlighted(int)), this, TQ_SLOT(menuStatusText(int)) );
	connect( mi->popup, TQ_SIGNAL(aboutToHide()), this, TQ_SLOT(clearStatusText()) );
	connect( mi->popup, TQ_SIGNAL( destroyed() ), this, TQ_SLOT( objectDestroyed() ) );
    // Makes only sense when called by TQActionGroup::addTo
    } else if ( ::tqt_cast<TQComboBox*>(w) ) {
	TQActionPrivate::ComboItem *ci = new TQActionPrivate::ComboItem;
	ci->combo = (TQComboBox*)w;
	connect( ci->combo, TQ_SIGNAL( destroyed() ), this, TQ_SLOT( objectDestroyed() ) );
	ci->id = ci->combo->count();
	if ( qstrcmp( name(), "qt_separator_action" ) ) {
	    if ( d->iconset )
		ci->combo->insertItem( d->iconset->pixmap(), text() );
	    else
		ci->combo->insertItem( text() );
	} else {
	    ci->id = -1;
	}
	d->comboitems.append( ci );

	d->update( TQActionPrivate::State | TQActionPrivate::EverythingElse );
    } else {
	tqWarning( "TQAction::addTo(), unknown object" );
	return false;
    }
    return true;
}

/*!
    This function is called from the addTo() function when it has
    created a widget (\a actionWidget) for the action in the \a
    container.
*/

void TQAction::addedTo( TQWidget *actionWidget, TQWidget *container )
{
    Q_UNUSED( actionWidget );
    Q_UNUSED( container );
}

/*!
    \overload

    This function is called from the addTo() function when it has
    created a menu item at the index position \a index in the popup
    menu \a menu.
*/

void TQAction::addedTo( int index, TQPopupMenu *menu )
{
    Q_UNUSED( index );
    Q_UNUSED( menu );
}

/*!
    Sets the status message to \a text
*/
void TQAction::showStatusText( const TQString& text )
{
#ifndef TQT_NO_STATUSBAR
    // find out whether we are clearing the status bar by the popup that actually set the text
    static TQPopupMenu *lastmenu = 0;
    TQObject *s = (TQObject*)sender();
    if ( s ) {
	TQPopupMenu *menu = (TQPopupMenu*)s->tqt_cast( "TQPopupMenu" );
	if ( menu && !!text )
	    lastmenu = menu;
	else if ( menu && text.isEmpty() ) {
	    if ( lastmenu && menu != lastmenu )
		return;
	    lastmenu = 0;
	}
    }

    TQObject* par = parent();
    TQObject* lpar = 0;
    TQStatusBar *bar = 0;
    while ( par && !bar ) {
	lpar = par;
	bar = (TQStatusBar*)par->child( 0, "TQStatusBar", false );
	par = par->parent();
    }
    if ( !bar && lpar ) {
	TQObjectList *l = lpar->queryList( "TQStatusBar" );
	if ( !l )
	    return;
	// #### hopefully the last one is the one of the mainwindow...
	bar = (TQStatusBar*)l->last();
	delete l;
    }
    if ( bar ) {
	if ( text.isEmpty() )
	    bar->clear();
	else
	    bar->message( text );
    }
#endif
}

/*!
    Sets the status message to the menu item's status text, or to the
    tooltip, if there is no status text.
*/
void TQAction::menuStatusText( int id )
{
    static int lastId = 0;
    TQString text;
    TQPtrListIterator<TQActionPrivate::MenuItem> it( d->menuitems);
    TQActionPrivate::MenuItem* mi;
    while ( ( mi = it.current() ) ) {
	++it;
	if ( mi->id == id ) {
	    text = statusTip();
	    break;
	}
    }

    if ( !text.isEmpty() )
	showStatusText( text );
    else if ( id != lastId )
	clearStatusText();
    lastId = id;
}

/*!
    Clears the status text.
*/
void TQAction::clearStatusText()
{
    if (!statusTip().isEmpty())
	showStatusText( TQString::null );
}

/*!
    Removes the action from widget \a w.

    Returns true if the action was removed successfully; otherwise
    returns false.

    \sa addTo()
*/
bool TQAction::removeFrom( TQWidget* w )
{
#ifndef TQT_NO_TOOLBAR
    if ( ::tqt_cast<TQToolBar*>(w) ) {
	TQPtrListIterator<TQToolButton> it( d->toolbuttons);
	TQToolButton* btn;
	while ( ( btn = it.current() ) ) {
	    ++it;
	    if ( btn->parentWidget() == w ) {
		d->toolbuttons.removeRef( btn );
		disconnect( btn, TQ_SIGNAL( destroyed() ), this, TQ_SLOT( objectDestroyed() ) );
		delete btn;
		// no need to disconnect from statusbar
	    }
	}
    } else
#endif
    if ( ::tqt_cast<TQPopupMenu*>(w) ) {
	TQPtrListIterator<TQActionPrivate::MenuItem> it( d->menuitems);
	TQActionPrivate::MenuItem* mi;
	while ( ( mi = it.current() ) ) {
	    ++it;
	    if ( mi->popup == w ) {
		disconnect( mi->popup, TQ_SIGNAL(highlighted(int)), this, TQ_SLOT(menuStatusText(int)) );
		disconnect( mi->popup, TQ_SIGNAL(aboutToHide()), this, TQ_SLOT(clearStatusText()) );
		disconnect( mi->popup, TQ_SIGNAL( destroyed() ), this, TQ_SLOT( objectDestroyed() ) );
		mi->popup->removeItem( mi->id );
		d->menuitems.removeRef( mi );
	    }
	}
    } else if ( ::tqt_cast<TQComboBox*>(w) ) {
	TQPtrListIterator<TQActionPrivate::ComboItem> it( d->comboitems );
	TQActionPrivate::ComboItem *ci;
	while ( ( ci = it.current() ) ) {
	    ++it;
	    if ( ci->combo == w ) {
		disconnect( ci->combo, TQ_SIGNAL(destroyed()), this, TQ_SLOT(objectDestroyed()) );
		d->comboitems.removeRef( ci );
	    }
	}
    } else {
	tqWarning( "TQAction::removeFrom(), unknown object" );
	return false;
    }
    return true;
}

/*!
  \internal
*/
void TQAction::objectDestroyed()
{
    const TQObject* obj = sender();
    TQPtrListIterator<TQActionPrivate::MenuItem> it( d->menuitems );
    TQActionPrivate::MenuItem* mi;
    while ( ( mi = it.current() ) ) {
	++it;
	if ( mi->popup == obj )
	    d->menuitems.removeRef( mi );
    }
    TQActionPrivate::ComboItem *ci;
    TQPtrListIterator<TQActionPrivate::ComboItem> it2( d->comboitems );
    while ( ( ci = it2.current() ) ) {
	++it2;
	if ( ci->combo == obj )
	    d->comboitems.removeRef( ci );
    }
    d->toolbuttons.removeRef( (TQToolButton*) obj );
}

/*!
    \fn void TQAction::activated()

    This signal is emitted when an action is activated by the user,
    e.g. when the user clicks a menu option or a toolbar button or
    presses an action's accelerator key combination.

    Connect to this signal for command actions. Connect to the
    toggled() signal for toggle actions.
*/

/*!
    \fn void TQAction::toggled(bool on)

    This signal is emitted when a toggle action changes state; command
    actions and \l{TQActionGroup}s don't emit toggled().

    The \a on argument denotes the new state: If \a on is true the
    toggle action is switched on, and if \a on is false the toggle
    action is switched off.

    To trigger a user command depending on whether a toggle action has
    been switched on or off connect it to a slot that takes a bool to
    indicate the state, e.g.

    \quotefile action/toggleaction/toggleaction.cpp
    \skipto TQMainWindow * window
    \printline TQMainWindow * window
    \skipto labelonoffaction
    \printline labelonoffaction
    \skipto connect
    \printuntil setUsesTextLabel

    \sa activated() setToggleAction() setOn()
*/

void TQActionGroupPrivate::update( const TQActionGroup* that )
{
    for ( TQPtrListIterator<TQAction> it( actions ); it.current(); ++it ) {
	if ( that->isEnabled() && !it.current()->d->forceDisabled ) {
	    it.current()->setEnabled( true );
	} else if ( !that->isEnabled() && it.current()->isEnabled() ) {
	    it.current()->setEnabled( false );
	    it.current()->d->forceDisabled = false;
	}
	if ( that->isVisible() && !it.current()->d->forceInvisible ) {
	    it.current()->setVisible( true );
	} else if ( !that->isVisible() && it.current()->isVisible() ) {
	    it.current()->setVisible( false );
	    it.current()->d->forceInvisible = false;
	}
    }
    for ( TQPtrListIterator<TQComboBox> cb( comboboxes ); cb.current(); ++cb ) {
	TQComboBox *combobox = cb.current();
	combobox->setEnabled( that->isEnabled() );
	combobox->setShown( that->isVisible() );

#ifndef TQT_NO_TOOLTIP
	TQToolTip::remove( combobox );
	if ( !!that->toolTip() )
	    TQToolTip::add( combobox, that->toolTip() );
#endif
#ifndef TQT_NO_WHATSTHIS
	TQWhatsThis::remove( combobox );
	if ( !!that->whatsThis() )
	    TQWhatsThis::add( combobox, that->whatsThis() );
#endif

    }
    for ( TQPtrListIterator<TQToolButton> mb( menubuttons ); mb.current(); ++mb ) {
	TQToolButton *button = mb.current();
	button->setEnabled( that->isEnabled() );
	button->setShown( that->isVisible() );

	if ( !that->text().isNull() )
	    button->setTextLabel( that->text() );
	if ( !that->iconSet().isNull() )
	    button->setIconSet( that->iconSet() );

#ifndef TQT_NO_TOOLTIP
	TQToolTip::remove( mb.current() );
	if ( !!that->toolTip() )
	    TQToolTip::add( button, that->toolTip() );
#endif
#ifndef TQT_NO_WHATSTHIS
	TQWhatsThis::remove( button );
	if ( !!that->whatsThis() )
	    TQWhatsThis::add( button, that->whatsThis() );
#endif
    }
    for ( TQPtrListIterator<TQActionGroupPrivate::MenuItem> pu( menuitems ); pu.current(); ++pu ) {
	TQWidget* parent = pu.current()->popup->parentWidget();
	if ( ::tqt_cast<TQPopupMenu*>(parent) ) {
	    TQPopupMenu* ppopup = (TQPopupMenu*)parent;
	    ppopup->setItemEnabled( pu.current()->id, that->isEnabled() );
	    ppopup->setItemVisible( pu.current()->id, that->isVisible() );
	} else {
	    pu.current()->popup->setEnabled( that->isEnabled() );
	}
    }
    for ( TQPtrListIterator<TQPopupMenu> pm( popupmenus ); pm.current(); ++pm ) {
	TQPopupMenu *popup = pm.current();
	TQPopupMenu *parent = ::tqt_cast<TQPopupMenu*>(popup->parentWidget());
	if ( !parent )
	    continue;

	int index;
	parent->findPopup( popup, &index );
	int id = parent->idAt( index );
	if ( !that->iconSet().isNull() )
	    parent->changeItem( id, that->iconSet(), that->menuText() );
	else
	    parent->changeItem( id, that->menuText() );
	parent->setItemEnabled( id, that->isEnabled() );
#ifndef TQT_NO_ACCEL
	parent->setAccel( that->accel(), id );
#endif
    }
}

/*!
    \class TQActionGroup ntqaction.h
    \brief The TQActionGroup class groups actions together.

    \ingroup basic
    \ingroup application

    In some situations it is useful to group actions together. For
    example, if you have a left justify action, a right justify action
    and a center action, only one of these actions should be active at
    any one time, and one simple way of achieving this is to group the
    actions together in an action group.

    An action group can also be added to a menu or a toolbar as a
    single unit, with all the actions within the action group
    appearing as separate menu options and toolbar buttons.

    Here's an example from examples/textedit:
    \quotefile textedit/textedit.cpp
    \skipto TQActionGroup
    \printuntil connect

    Here we create a new action group. Since the action group is exclusive
    by default, only one of the actions in the group is ever active at any
    one time. We then connect the group's selected() signal to our
    textAlign() slot.

    \printuntil actionAlignLeft->setToggleAction

    We create a left align action, add it to the toolbar and the menu
    and make it a toggle action. We create center and right align
    actions in exactly the same way.

    \omit
    A TQActionGroup emits an activated() signal when one of its actions
    is activated.
    \endomit
    The actions in an action group emit their activated() (and for
    toggle actions, toggled()) signals as usual.

    The setExclusive() function is used to ensure that only one action
    is active at any one time: it should be used with actions which
    have their \c toggleAction set to true.

    Action group actions appear as individual menu options and toolbar
    buttons. For exclusive action groups use setUsesDropDown() to
    display the actions in a subwidget of any widget the action group
    is added to. For example, the actions would appear in a combobox
    in a toolbar or as a submenu in a menu.

    Actions can be added to an action group using add(), but normally
    they are added by creating the action with the action group as
    parent. Actions can have separators dividing them using
    addSeparator(). Action groups are added to widgets with addTo().
*/

/*!
    Constructs an action group called \a name, with parent \a parent.

    The action group is exclusive by default. Call setExclusive(false) to make
    the action group non-exclusive.
*/
TQActionGroup::TQActionGroup( TQObject* parent, const char* name )
    : TQAction( parent, name )
{
    d = new TQActionGroupPrivate;
    d->exclusive = true;
    d->dropdown = false;
    d->selected = 0;
    d->separatorAction = 0;
    TQAction::d->d_group = d;

    connect( this, TQ_SIGNAL(selected(TQAction*)), TQ_SLOT(internalToggle(TQAction*)) );
}

/*!
    Constructs an action group called \a name, with parent \a parent.

    If \a exclusive is true only one toggle action in the group will
    ever be active.

    \sa exclusive
*/
TQActionGroup::TQActionGroup( TQObject* parent, const char* name, bool exclusive )
    : TQAction( parent, name )
{
    d = new TQActionGroupPrivate;
    d->exclusive = exclusive;
    d->dropdown = false;
    d->selected = 0;
    d->separatorAction = 0;
    TQAction::d->d_group = d;

    connect( this, TQ_SIGNAL(selected(TQAction*)), TQ_SLOT(internalToggle(TQAction*)) );
}

/*!
    Destroys the object and frees allocated resources.
*/

TQActionGroup::~TQActionGroup()
{
    TQPtrListIterator<TQActionGroupPrivate::MenuItem> mit( d->menuitems );
    while ( mit.current() ) {
	TQActionGroupPrivate::MenuItem *mi = mit.current();
	++mit;
	if ( mi->popup )
	    mi->popup->disconnect( TQ_SIGNAL(destroyed()), this, TQ_SLOT(objectDestroyed()) );
    }

    TQPtrListIterator<TQComboBox> cbit( d->comboboxes );
    while ( cbit.current() ) {
	TQComboBox *cb = cbit.current();
	++cbit;
	cb->disconnect(  TQ_SIGNAL(destroyed()), this, TQ_SLOT(objectDestroyed()) );
    }
    TQPtrListIterator<TQToolButton> mbit( d->menubuttons );
    while ( mbit.current() ) {
	TQToolButton *mb = mbit.current();
	++mbit;
	mb->disconnect(  TQ_SIGNAL(destroyed()), this, TQ_SLOT(objectDestroyed()) );
    }
    TQPtrListIterator<TQPopupMenu> pmit( d->popupmenus );
    while ( pmit.current() ) {
	TQPopupMenu *pm = pmit.current();
	++pmit;
	pm->disconnect(  TQ_SIGNAL(destroyed()), this, TQ_SLOT(objectDestroyed()) );
    }

    delete d->separatorAction;
    d->menubuttons.setAutoDelete( true );
    d->comboboxes.setAutoDelete( true );
    d->menuitems.setAutoDelete( true );
    d->popupmenus.setAutoDelete( true );
    delete d;
}

/*!
    \property TQActionGroup::exclusive
    \brief whether the action group does exclusive toggling

    If exclusive is true only one toggle action in the action group
    can ever be active at any one time. If the user chooses another
    toggle action in the group the one they chose becomes active and
    the one that was active becomes inactive.

    \sa TQAction::toggleAction
*/
void TQActionGroup::setExclusive( bool enable )
{
    d->exclusive = enable;
}

bool TQActionGroup::isExclusive() const
{
    return d->exclusive;
}

/*!
    \property TQActionGroup::usesDropDown
    \brief whether the group's actions are displayed in a subwidget of
    the widgets the action group is added to

    Exclusive action groups added to a toolbar display their actions
    in a combobox with the action's \l TQAction::text and \l
    TQAction::iconSet properties shown. Non-exclusive groups are
    represented by a tool button showing their \l TQAction::iconSet and
    -- depending on \l TQMainWindow::usesTextLabel() -- text()
    property.

    In a popup menu the member actions are displayed in a submenu.

    Changing usesDropDown only affects \e subsequent calls to addTo().

    Note that setting this property for actions in a combobox causes
    calls to their \link TQAction::setVisible()\endlink,
    \link TQAction::setEnabled()\endlink, and
    \link TQAction::setDisabled()\endlink functions to have no effect.

    This property's default is false.

*/
void TQActionGroup::setUsesDropDown( bool enable )
{
    d->dropdown = enable;
}

bool TQActionGroup::usesDropDown() const
{
    return d->dropdown;
}

/*!
    Adds action \a action to this group.

    Normally an action is added to a group by creating it with the
    group as parent, so this function is not usually used.

    \sa addTo()
*/
void TQActionGroup::add( TQAction* action )
{
    if ( d->actions.containsRef( action ) )
	return;

    d->actions.append( action );

    if ( action->whatsThis().isNull() )
	action->setWhatsThis( whatsThis() );
    if ( action->toolTip().isNull() )
	action->setToolTip( toolTip() );

    if (!action->d->forceDisabled)
	action->d->enabled = isEnabled();
    if (!action->d->forceInvisible)
	action->d->visible = isVisible();

    connect( action, TQ_SIGNAL( destroyed() ), this, TQ_SLOT( childDestroyed() ) );
    connect( action, TQ_SIGNAL( activated() ), this, TQ_SIGNAL( activated() ) );
    connect( action, TQ_SIGNAL( toggled(bool) ), this, TQ_SLOT( childToggled(bool) ) );

    for ( TQPtrListIterator<TQComboBox> cb( d->comboboxes ); cb.current(); ++cb ) {
	action->addTo( cb.current() );
    }
    for ( TQPtrListIterator<TQToolButton> mb( d->menubuttons ); mb.current(); ++mb ) {
	TQPopupMenu* popup = mb.current()->popup();
	if ( !popup )
	    continue;
	action->addTo( popup );
    }
    for ( TQPtrListIterator<TQActionGroupPrivate::MenuItem> mi( d->menuitems ); mi.current(); ++mi ) {
	TQPopupMenu* popup = mi.current()->popup;
	if ( !popup )
	    continue;
	action->addTo( popup );
    }
}

/*!
    Adds a separator to the group.
*/
void TQActionGroup::addSeparator()
{
    if ( !d->separatorAction )
	d->separatorAction = new TQAction( 0, "qt_separator_action" );
    d->actions.append( d->separatorAction );
}


/*! \fn void TQActionGroup::insert( TQAction* a )

  \obsolete

  Use add() instead, or better still create the action with the action
  group as its parent.
 */

/*!
    Adds this action group to the widget \a w.

    If isExclusive() is false or usesDropDown() is false, the actions within
    the group are added to the widget individually. For example, if the widget
    is a menu, the actions will appear as individual menu options, and
    if the widget is a toolbar, the actions will appear as toolbar buttons.

    If both isExclusive() and usesDropDown() are true, the actions
    are presented either in a combobox (if \a w is a toolbar) or in a
    submenu (if \a w is a menu).

    All actions should be added to the action group \e before the
    action group is added to the widget. If actions are added to the
    action group \e after the action group has been added to the
    widget these later actions will \e not appear.

    \sa setExclusive() setUsesDropDown() removeFrom()
*/
bool TQActionGroup::addTo( TQWidget* w )
{
#ifndef TQT_NO_TOOLBAR
    if ( ::tqt_cast<TQToolBar*>(w) ) {
	if ( d->dropdown ) {
	    if ( !d->exclusive ) {
		TQPtrListIterator<TQAction> it( d->actions);
		if ( !it.current() )
		    return true;

		TQAction *defAction = it.current();

		TQToolButton* btn = new TQToolButton( (TQToolBar*) w, "qt_actiongroup_btn" );
		addedTo( btn, w );
		connect( btn, TQ_SIGNAL(destroyed()), TQ_SLOT(objectDestroyed()) );
		d->menubuttons.append( btn );

		if ( !iconSet().isNull() )
		    btn->setIconSet( iconSet() );
		else if ( !defAction->iconSet().isNull() )
		    btn->setIconSet( defAction->iconSet() );
		if ( !!text() )
		    btn->setTextLabel( text() );
		else if ( !!defAction->text() )
		    btn->setTextLabel( defAction->text() );
#ifndef TQT_NO_TOOLTIP
		if ( !!toolTip() )
		    TQToolTip::add( btn, toolTip() );
		else if ( !!defAction->toolTip() )
		    TQToolTip::add( btn, defAction->toolTip() );
#endif
#ifndef TQT_NO_WHATSTHIS
		if ( !!whatsThis() )
		    TQWhatsThis::add( btn, whatsThis() );
		else if ( !!defAction->whatsThis() )
		    TQWhatsThis::add( btn, defAction->whatsThis() );
#endif

		connect( btn, TQ_SIGNAL( clicked() ), defAction, TQ_SIGNAL( activated() ) );
		connect( btn, TQ_SIGNAL( toggled(bool) ), defAction, TQ_SLOT( toolButtonToggled(bool) ) );
		connect( btn, TQ_SIGNAL( destroyed() ), defAction, TQ_SLOT( objectDestroyed() ) );

		TQPopupMenu *menu = new TQPopupMenu( btn, "qt_actiongroup_menu" );
		btn->setPopupDelay( 0 );
		btn->setPopup( menu );

		while( it.current() ) {
		    it.current()->addTo( menu );
		    ++it;
		}
		d->update( this );
		return true;
	    } else {
		TQComboBox *box = new TQComboBox( false, w, "qt_actiongroup_combo" );
		addedTo( box, w );
		connect( box, TQ_SIGNAL(destroyed()), TQ_SLOT(objectDestroyed()) );
		d->comboboxes.append( box );
#ifndef TQT_NO_TOOLTIP
		if ( !!toolTip() )
		    TQToolTip::add( box, toolTip() );
#endif
#ifndef TQT_NO_WHATSTHIS
		if ( !!whatsThis() )
		    TQWhatsThis::add( box, whatsThis() );
#endif

		int onIndex = 0;
		bool foundOn = false;
		for ( TQPtrListIterator<TQAction> it( d->actions); it.current(); ++it ) {
		    TQAction *action = it.current();
		    if ( !foundOn )
			foundOn = action->isOn();
		    if ( qstrcmp( action->name(), "qt_separator_action" ) && !foundOn )
			onIndex++;
		    action->addTo( box );
		}
		if ( foundOn )
		    box->setCurrentItem( onIndex );
		connect( box, TQ_SIGNAL(activated(int)), this, TQ_SLOT( internalComboBoxActivated(int)) );
		connect( box, TQ_SIGNAL(highlighted(int)), this, TQ_SLOT( internalComboBoxHighlighted(int)) );
		d->update( this );
		return true;
	    }
	}
    } else
#endif
    if ( ::tqt_cast<TQPopupMenu*>(w) ) {
	TQPopupMenu *popup;
	if ( d->dropdown ) {
	    TQPopupMenu *menu = (TQPopupMenu*)w;
	    popup = new TQPopupMenu( w, "qt_actiongroup_menu" );
	    d->popupmenus.append( popup );
	    connect( popup, TQ_SIGNAL(destroyed()), TQ_SLOT(objectDestroyed()) );

	    int id;
	    if ( !iconSet().isNull() ) {
		if ( menuText().isEmpty() )
		    id = menu->insertItem( iconSet(), text(), popup );
		else
		    id = menu->insertItem( iconSet(), menuText(), popup );
	    } else {
		if ( menuText().isEmpty() )
		    id = menu->insertItem( text(), popup );
		else
		    id = menu->insertItem( menuText(), popup );
	    }

	    addedTo( menu->indexOf( id ), menu );

	    TQActionGroupPrivate::MenuItem *item = new TQActionGroupPrivate::MenuItem;
	    item->id = id;
	    item->popup = popup;
	    d->menuitems.append( item );
	} else {
	    popup = (TQPopupMenu*)w;
	}
	for ( TQPtrListIterator<TQAction> it( d->actions); it.current(); ++it ) {
	    // #### do an addedTo( index, popup, action), need to find out index
	    it.current()->addTo( popup );
	}
	return true;
    }

    for ( TQPtrListIterator<TQAction> it( d->actions); it.current(); ++it ) {
	// #### do an addedTo( index, popup, action), need to find out index
	it.current()->addTo( w );
    }

    return true;
}

/*! \reimp
*/
bool TQActionGroup::removeFrom( TQWidget* w )
{
    for ( TQPtrListIterator<TQAction> it( d->actions); it.current(); ++it ) {
	it.current()->removeFrom( w );
    }

#ifndef TQT_NO_TOOLBAR
    if ( ::tqt_cast<TQToolBar*>(w) ) {
	TQPtrListIterator<TQComboBox> cb( d->comboboxes );
	while( cb.current() ) {
	    TQComboBox *box = cb.current();
	    ++cb;
	    if ( box->parentWidget() == w )
		delete box;
	}
	TQPtrListIterator<TQToolButton> mb( d->menubuttons );
	while( mb.current() ) {
	    TQToolButton *btn = mb.current();
	    ++mb;
	    if ( btn->parentWidget() == w )
		delete btn;
	}
    } else
#endif
    if ( ::tqt_cast<TQPopupMenu*>(w) ) {
	TQPtrListIterator<TQActionGroupPrivate::MenuItem> pu( d->menuitems );
	while ( pu.current() ) {
	    TQActionGroupPrivate::MenuItem *mi = pu.current();
	    ++pu;
	    if ( d->dropdown && mi->popup )
		( (TQPopupMenu*)w )->removeItem( mi->id );
	    delete mi->popup;
	}
    }

    return true;
}

/*! \internal
*/
void TQActionGroup::childToggled( bool b )
{
    if ( !isExclusive() )
	return;
    TQAction* s = (TQAction*) sender();
    if ( b ) {
	if ( s != d->selected ) {
	    d->selected = s;
	    for ( TQPtrListIterator<TQAction> it( d->actions); it.current(); ++it ) {
		if ( it.current()->isToggleAction() && it.current() != s )
		    it.current()->setOn( false );
	    }
	    emit activated();
	    emit selected( s );
	} else if ( !s->isToggleAction() ) {
	    emit activated();
	}
    } else {
	if ( s == d->selected ) {
	    // at least one has to be selected
	    s->setOn( true );
	}
    }
}

/*! \internal
*/
void TQActionGroup::childDestroyed()
{
    d->actions.removeRef( (TQAction*) sender() );
    if ( d->selected == sender() )
	d->selected = 0;
}

/*! \reimp
*/
void TQActionGroup::setEnabled( bool enable )
{
    if ( enable == isEnabled() )
	return;

    TQAction::setEnabled( enable );
    d->update( this );
}

/*! \reimp
*/
void TQActionGroup::setToggleAction( bool toggle )
{
    for ( TQPtrListIterator<TQAction> it( d->actions); it.current(); ++it )
	it.current()->setToggleAction( toggle );

    TQAction::setToggleAction( true );
    d->update( this );
}

/*! \reimp
*/
void TQActionGroup::setOn( bool on )
{
    for ( TQPtrListIterator<TQAction> it( d->actions); it.current(); ++it ) {
	TQAction *act = it.current();
	if ( act->isToggleAction() )
	    act->setOn( on );
    }

    TQAction::setOn( on );
    d->update( this );
}

/*! \reimp
*/
void TQActionGroup::setIconSet( const TQIconSet& icon )
{
    TQAction::setIconSet( icon );
    d->update( this );
}

/*! \reimp
*/
void TQActionGroup::setText( const TQString& txt )
{
    if ( txt == text() )
	return;

    TQAction::setText( txt );
    d->update( this );
}

/*! \reimp
*/
void TQActionGroup::setMenuText( const TQString& text )
{
    if ( text == menuText() )
	return;

    TQAction::setMenuText( text );
    d->update( this );
}

/*! \reimp
*/
void TQActionGroup::setToolTip( const TQString& text )
{
    if ( text == toolTip() )
	return;
    for ( TQPtrListIterator<TQAction> it( d->actions); it.current(); ++it ) {
	if ( it.current()->toolTip().isNull() )
	    it.current()->setToolTip( text );
    }
    TQAction::setToolTip( text );
    d->update( this );
}

/*! \reimp
*/
void TQActionGroup::setWhatsThis( const TQString& text )
{
    if ( text == whatsThis() )
	return;
    for ( TQPtrListIterator<TQAction> it( d->actions); it.current(); ++it ) {
	if ( it.current()->whatsThis().isNull() )
	    it.current()->setWhatsThis( text );
    }
    TQAction::setWhatsThis( text );
    d->update( this );
}

/*! \reimp
*/
void TQActionGroup::childEvent( TQChildEvent *e )
{
    if ( !e->removed() )
	return;

    TQAction *action = ::tqt_cast<TQAction*>(e->child());
    if ( !action )
	return;

    for ( TQPtrListIterator<TQComboBox> cb( d->comboboxes ); cb.current(); ++cb ) {
	for ( int i = 0; i < cb.current()->count(); i++ ) {
	    if ( cb.current()->text( i ) == action->text() ) {
		cb.current()->removeItem( i );
		break;
	    }
	}
    }
    for ( TQPtrListIterator<TQToolButton> mb( d->menubuttons ); mb.current(); ++mb ) {
	TQPopupMenu* popup = mb.current()->popup();
	if ( !popup )
	    continue;
	action->removeFrom( popup );
    }
    for ( TQPtrListIterator<TQActionGroupPrivate::MenuItem> mi( d->menuitems ); mi.current(); ++mi ) {
	TQPopupMenu* popup = mi.current()->popup;
	if ( !popup )
	    continue;
	action->removeFrom( popup );
    }
}

/*!
    \fn void TQActionGroup::selected( TQAction* )

    This signal is emitted from exclusive groups when toggle actions
    change state.

    The argument is the action whose state changed to "on".

    \sa setExclusive(), isOn() TQAction::toggled()
*/

/*! \internal
*/
void TQActionGroup::internalComboBoxActivated( int index )
{
    TQAction *a = 0;
    for ( int i = 0; i <= index && i < (int)d->actions.count(); ++i ) {
	a = d->actions.at( i );
	if ( a && !qstrcmp( a->name(), "qt_separator_action" ) )
	    index++;
    }
    a = d->actions.at( index );
    if ( a ) {
	if ( a != d->selected ) {
	    d->selected = a;
	    for ( TQPtrListIterator<TQAction> it( d->actions); it.current(); ++it ) {
		if ( it.current()->isToggleAction() && it.current() != a )
		    it.current()->setOn( false );
	    }
	    if ( a->isToggleAction() )
		a->setOn( true );

	    emit activated();
	    if ( a->isToggleAction() )
		emit selected( d->selected );
	    emit ((TQActionGroup*)a)->activated();
	} else if ( !a->isToggleAction() ) {
	    emit activated();
	    emit ((TQActionGroup*)a)->activated();
	}
	a->clearStatusText();
    }
}

/*! \internal
*/
void TQActionGroup::internalComboBoxHighlighted( int index )
{
    TQAction *a = 0;
    for ( int i = 0; i <= index && i < (int)d->actions.count(); ++i ) {
	a = d->actions.at( i );
	if ( a && !qstrcmp( a->name(), "qt_separator_action" ) )
	    index++;
    }
    a = d->actions.at( index );
    if ( a )
	a->showStatusText(a->statusTip());
    else
	clearStatusText();
}

/*! \internal
*/
void TQActionGroup::internalToggle( TQAction *a )
{
    int index = d->actions.find( a );
    if ( index == -1 )
	return;

    int lastItem = index;
    for ( int i = 0; i < lastItem; i++ ) {
	TQAction *action = d->actions.at( i );
	if ( !qstrcmp( action->name(), "qt_separator_action" ) )
	    index--;
    }

    for ( TQPtrListIterator<TQComboBox> it( d->comboboxes); it.current(); ++it )
	    it.current()->setCurrentItem( index );
}

/*! \internal
*/
void TQActionGroup::objectDestroyed()
{
    const TQObject* obj = sender();
    d->menubuttons.removeRef( (TQToolButton*)obj );
    for ( TQPtrListIterator<TQActionGroupPrivate::MenuItem> mi( d->menuitems ); mi.current(); ++mi ) {
	if ( mi.current()->popup == obj ) {
	    d->menuitems.removeRef( mi.current() );
	    break;
	}
    }
    d->popupmenus.removeRef( (TQPopupMenu*)obj );
    d->comboboxes.removeRef( (TQComboBox*)obj );
}

/*!
    \internal
    
    This function is called from the addTo() function when it has
    created a widget (\a actionWidget) for the child action \a a in
    the \a container.
*/

void TQActionGroup::addedTo( TQWidget *actionWidget, TQWidget *container, TQAction *a )
{
    Q_UNUSED( actionWidget );
    Q_UNUSED( container );
    Q_UNUSED( a );
}

/*!
    \overload
    \internal
    
    This function is called from the addTo() function when it has
    created a menu item for the child action at the index position \a
    index in the popup menu \a menu.
*/

void TQActionGroup::addedTo( int index, TQPopupMenu *menu, TQAction *a )
{
    Q_UNUSED( index );
    Q_UNUSED( menu );
    Q_UNUSED( a );
}

/*!
    \reimp
    \overload

    This function is called from the addTo() function when it has
    created a widget (\a actionWidget) in the \a container.
*/

void TQActionGroup::addedTo( TQWidget *actionWidget, TQWidget *container )
{
    Q_UNUSED( actionWidget );
    Q_UNUSED( container );
}

/*!
    \reimp
    \overload

    This function is called from the addTo() function when it has
    created a menu item at the index position \a index in the popup
    menu \a menu.
*/

void TQActionGroup::addedTo( int index, TQPopupMenu *menu )
{
    Q_UNUSED( index );
    Q_UNUSED( menu );
}

#endif
