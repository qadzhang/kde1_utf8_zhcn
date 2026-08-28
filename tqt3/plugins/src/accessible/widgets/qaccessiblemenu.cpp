#include "qaccessiblemenu.h"

#include <ntqpopupmenu.h>
#include <ntqmenubar.h>
#include <ntqstyle.h>

class MyPopupMenu : public TQPopupMenu
{
    friend class TQAccessiblePopup;
};

TQAccessiblePopup::TQAccessiblePopup( TQObject *o )
: TQAccessibleWidget( o )
{
    Q_ASSERT( o->inherits("TQPopupMenu") );
}

TQPopupMenu *TQAccessiblePopup::popupMenu() const
{
    return (TQPopupMenu*)object();
}

int TQAccessiblePopup::childCount() const
{
    return popupMenu()->count();
}

TQRESULT TQAccessiblePopup::queryChild( int /*control*/, TQAccessibleInterface **iface ) const
{
    *iface = 0;
    return TQS_FALSE;
}

TQRect TQAccessiblePopup::rect( int control ) const
{
    if ( !control )
	return TQAccessibleWidget::rect( control );

    TQRect r = popupMenu()->itemGeometry( control - 1 );
    TQPoint tlp = popupMenu()->mapToGlobal( TQPoint( 0,0 ) );

    return TQRect( tlp.x() + r.x(), tlp.y() + r.y(), r.width(), r.height() );
}

int TQAccessiblePopup::controlAt( int x, int y ) const
{
    TQPoint p = popupMenu()->mapFromGlobal( TQPoint( x,y ) );
    MyPopupMenu *mp = (MyPopupMenu*)popupMenu();
    return mp->itemAtPos( p, false ) + 1;
}

int TQAccessiblePopup::navigate( NavDirection direction, int startControl ) const
{
    if ( direction != NavFirstChild && direction != NavLastChild && direction != NavFocusChild && !startControl )
	return TQAccessibleWidget::navigate( direction, startControl );

    switch ( direction ) {
    case NavFirstChild:
	return 1;
    case NavLastChild:
	return childCount();
    case NavNext:
    case NavDown:
	return (startControl + 1) > childCount() ? -1 : startControl + 1;
    case NavPrevious:
    case NavUp:
	return (startControl - 1) < 1 ? -1 : startControl - 1;
    default:
	break;
    }
    return -1;
}

TQString TQAccessiblePopup::text( Text t, int control ) const
{
    TQString tx = TQAccessibleWidget::text( t, control );
    if ( !!tx )
	return tx;

    int id;
    TQMenuItem *item = 0;
    if ( control ) {
	id = popupMenu()->idAt( control - 1 );
	item = popupMenu()->findItem( id );
    }

    switch ( t ) {
    case Name:
	if ( !control )
	    return popupMenu()->caption();
	return stripAmp( popupMenu()->text( id ) );
    case Help:
	return popupMenu()->whatsThis( id );
    case Accelerator:
	return hotKey( popupMenu()->text( id ) );
    case DefaultAction:
	if ( !item || item->isSeparator() || !item->isEnabled() )
	    break;
	if ( item->popup() )
	    return TQPopupMenu::tr("Open");
	return TQPopupMenu::tr("Execute");
    default:
	break;
    }
    return tx;
}

TQAccessible::Role TQAccessiblePopup::role( int control ) const
{
    if ( !control )
	return PopupMenu;
    
    TQMenuItem *item = popupMenu()->findItem( popupMenu()->idAt( control -1 ) );
    if ( item && item->isSeparator() )
	return Separator;
    return MenuItem;
}

TQAccessible::State TQAccessiblePopup::state( int control ) const
{
    int s = TQAccessibleWidget::state( control );
    if ( !control )
	return (State)s;

    int id = popupMenu()->idAt( control -1 );
    TQMenuItem *item = popupMenu()->findItem( id );
    if ( !item )
	return (State)s;

    if ( popupMenu()->style().styleHint( TQStyle::SH_PopupMenu_MouseTracking ) )
	s |= HotTracked;
    if ( item->isSeparator() || !item->isEnabled() )
	s |= Unavailable;
    if ( popupMenu()->isCheckable() && item->isChecked() )
	s |= Checked;
    if ( popupMenu()->isItemActive( id ) )
	s |= Focused;

    return (State)s;
}

bool TQAccessiblePopup::doDefaultAction( int control )
{
    if ( !control )
	return false;

    int id = popupMenu()->idAt( control -1 );
    TQMenuItem *item = popupMenu()->findItem( id );
    if ( !item || !item->isEnabled() )
	return false;

    popupMenu()->activateItemAt( control - 1);
    return true;
}

bool TQAccessiblePopup::setFocus( int control )
{
    if ( !control )
	return false;

    int id = popupMenu()->idAt( control -1 );
    TQMenuItem *item = popupMenu()->findItem( id );
    if ( !item || !item->isEnabled() )
	return false;

    popupMenu()->setActiveItem( control - 1 );
    return true;
}


class MyMenuBar : public TQMenuBar
{
    friend class TQAccessibleMenuBar;
};

TQAccessibleMenuBar::TQAccessibleMenuBar( TQObject *o )
: TQAccessibleWidget( o )
{
    Q_ASSERT( o->inherits( "TQMenuBar" ) );
}

TQMenuBar *TQAccessibleMenuBar::menuBar() const
{
    return (TQMenuBar*)object();
}

int TQAccessibleMenuBar::childCount() const
{
    return menuBar()->count();
}

TQRESULT TQAccessibleMenuBar::queryChild( int /*control*/, TQAccessibleInterface **iface ) const
{
    *iface = 0;
    return TQS_FALSE;
}

TQRect TQAccessibleMenuBar::rect( int control ) const
{
    if ( !control )
	return TQAccessibleWidget::rect( control );

    MyMenuBar *mb = (MyMenuBar*)menuBar();
    TQRect r = mb->itemRect( control - 1 );
    TQPoint tlp = mb->mapToGlobal( TQPoint( 0,0 ) );

    return TQRect( tlp.x() + r.x(), tlp.y() + r.y(), r.width(), r.height() );
}

int TQAccessibleMenuBar::controlAt( int x, int y ) const
{
    MyMenuBar *mb = (MyMenuBar*)menuBar();
    TQPoint p = mb->mapFromGlobal( TQPoint( x,y ) );
    return mb->itemAtPos( p ) + 1;
}

int TQAccessibleMenuBar::navigate( NavDirection direction, int startControl ) const
{
    if ( direction != NavFirstChild && direction != NavLastChild && direction != NavFocusChild && !startControl )
	return TQAccessibleWidget::navigate( direction, startControl );

    switch ( direction ) {
    case NavFirstChild:
	return 1;
    case NavLastChild:
	return childCount();
    case NavNext:
    case NavRight:
	return (startControl + 1) > childCount() ? -1 : startControl + 1;
    case NavPrevious:
    case NavLeft:
	return (startControl - 1) < 1 ? -1 : startControl - 1;
    default:
	break;
    }

    return -1;
}

TQString TQAccessibleMenuBar::text( Text t, int control ) const
{
    TQString tx = TQAccessibleWidget::text( t, control );
    if ( !!tx )
	return tx;
    if ( !control )
	return tx;

    int id = menuBar()->idAt( control - 1 );
    switch ( t ) {
    case Name:
	return stripAmp( menuBar()->text( id ) );
    case Accelerator:
	tx = hotKey( menuBar()->text( id ) );
	if ( !!tx )
	    return "Alt + "+tx;
	break;
    case DefaultAction:
	return TQMenuBar::tr("Open");
    default:
	break;
    }

    return tx;
}

TQAccessible::Role TQAccessibleMenuBar::role( int control ) const
{
    if ( !control )
	return MenuBar;

    TQMenuItem *item = menuBar()->findItem( menuBar()->idAt( control -1 ) );
    if ( item && item->isSeparator() )
	return Separator;
    return MenuItem;
}

TQAccessible::State TQAccessibleMenuBar::state( int control ) const
{
    int s = TQAccessibleWidget::state( control );
    if ( !control )
	return (State)s;

    int id = menuBar()->idAt( control -1 );
    TQMenuItem *item = menuBar()->findItem( id );
    if ( !item )
	return (State)s;

    if ( menuBar()->style().styleHint( TQStyle::SH_PopupMenu_MouseTracking ) )
	s |= HotTracked;
    if ( item->isSeparator() || !item->isEnabled() )
	s |= Unavailable;
    if ( menuBar()->isItemActive( id ) )
	s |= Focused;

    return (State)s;
}

bool TQAccessibleMenuBar::doDefaultAction( int control )
{
    if ( !control )
	return false;

    int id = menuBar()->idAt( control -1 );
    TQMenuItem *item = menuBar()->findItem( id );
    if ( !item || !item->isEnabled() )
	return false;

    menuBar()->activateItemAt( control - 1);
    return true;
}

bool TQAccessibleMenuBar::setFocus( int control )
{
    if ( !control )
	return false;

    int id = menuBar()->idAt( control -1 );
    TQMenuItem *item = menuBar()->findItem( id );
    if ( !item || !item->isEnabled() )
	return false;

    return true;
}
