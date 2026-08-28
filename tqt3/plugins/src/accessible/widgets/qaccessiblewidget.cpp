#include "qaccessiblewidget.h"

#include <ntqapplication.h>
#include <ntqstyle.h>
#include <ntqobjectlist.h>
#include <ntqpushbutton.h>
#include <ntqslider.h>
#include <ntqdial.h>
#include <ntqspinbox.h>
#include <ntqscrollbar.h>
#include <ntqslider.h>
#include <ntqlineedit.h>
#include <ntqlabel.h>
#include <ntqlcdnumber.h>
#include <ntqprogressbar.h>
#include <ntqgroupbox.h>
#include <ntqtoolbutton.h>
#include <ntqwhatsthis.h>
#include <ntqtooltip.h>
#include <ntqscrollview.h>
#include <ntqheader.h>
#include <ntqtabbar.h>
#include <ntqcombobox.h>
#include <ntqrangecontrol.h>
#include <ntqlistbox.h>
#include <ntqlistview.h>
#include <ntqiconview.h>
#include <ntqtextedit.h>
#include <ntqwidgetstack.h>
#include <private/qtitlebar_p.h>


TQString buddyString( TQWidget *widget )
{
    TQWidget *parent = widget->parentWidget();
    TQObjectList *ol = parent->queryList( "TQLabel", 0, false, false );
    if ( !ol || !ol->count() ) {
	delete ol;
	return TQString::null;
    }

    TQString str;

    TQObjectListIt it(*ol);
    while ( it.current() ) {
	TQLabel *label = (TQLabel*)it.current();
	++it;
	if ( label->buddy() == widget ) {
	    str = label->text();
	    break;
	}
    }
    delete ol;
    if ( !!str )
	return str;

    if ( parent->inherits( "TQGroupBox" ) )
	return ((TQGroupBox*)parent)->title();

    return TQString::null;
}

TQString stripAmp( const TQString &text )
{
    if ( text.isEmpty() )
	return text;

    TQString n = text;
    for ( uint i = 0; i < n.length(); i++ ) {
	if ( n[(int)i] == '&' )
	    n.remove( i, 1 );
    }
    return n;
}

TQString hotKey( const TQString &text )
{
    if ( text.isEmpty() )
	return text;

    TQString n = text;
    int fa = 0;
    bool ac = false;
    while ( ( fa = n.find( "&", fa ) ) != -1 ) {
	if ( n.at(fa+1) != '&' ) {
	    ac = true;
	    break;
	}
    }
    if ( fa != -1 && ac )
	return TQString( n.at(fa + 1) );

    return TQString::null;
}

/*!
  \class TQAccessibleWidget qaccessiblewidget.h
  \brief The TQAccessibleWidget class implements the TQAccessibleInterface for TQWidgets.
*/

ulong TQAccessibleWidget::objects = 0;

/*!
  Creates a TQAccessibleWidget object for \a o.
  \a role, \a name, \a description, \a value, \a help, \a defAction,
  \a accelerator and \a state are optional parameters for static values
  of the object's property.
*/
TQAccessibleWidget::TQAccessibleWidget( TQObject *o, Role role, TQString name,
    TQString description, TQString value, TQString help, TQString defAction, TQString accelerator, State state )
    : TQAccessibleObject( o ), role_(role), state_(state), name_(name),
      description_(description),value_(value),help_(help),
      defAction_(defAction), accelerator_(accelerator)
{
    objects++;
}

TQAccessibleWidget::~TQAccessibleWidget()
{
    objects--;
}

/*! Returns the widget. */
TQWidget *TQAccessibleWidget::widget() const
{
    Q_ASSERT(object()->isWidgetType());
    if ( !object()->isWidgetType() )
	return 0;
    return (TQWidget*)object();
}

/*! \reimp */
int TQAccessibleWidget::controlAt( int x, int y ) const
{
    TQWidget *w = widget();
    TQPoint gp = w->mapToGlobal( TQPoint( 0, 0 ) );
    if ( !TQRect( gp.x(), gp.y(), w->width(), w->height() ).contains( x, y ) )
	return -1;

    TQPoint rp = w->mapFromGlobal( TQPoint( x, y ) );

    TQObjectList *list = w->queryList( "TQWidget", 0, false, false );

    if ( !list || list->isEmpty() )
	return 0;

    TQObjectListIt it( *list );
    TQWidget *child = 0;
    int index = 1;
    while ( ( child = (TQWidget*)it.current() ) ) {
	if ( !child->isTopLevel() && !child->isHidden() && child->geometry().contains( rp ) ) {
	    delete list;
	    return index;
	}
	++it;
	++index;
    }
    delete list;
    return 0;
}

/*! \reimp */
TQRect	TQAccessibleWidget::rect( int control ) const
{
#if defined(QT_DEBUG)
    if ( control )
	tqWarning( "TQAccessibleWidget::rect: This implementation does not support subelements! (ID %d unknown for %s)", control, widget()->className() );
#else
    Q_UNUSED(control)
#endif
    TQWidget *w = widget();
    TQPoint wpos = w->mapToGlobal( TQPoint( 0, 0 ) );

    return TQRect( wpos.x(), wpos.y(), w->width(), w->height() );
}

/*! \reimp */
int TQAccessibleWidget::navigate( NavDirection dir, int startControl ) const
{
#if defined(QT_DEBUG)
    if ( startControl )
	tqWarning( "TQAccessibleWidget::navigate: This implementation does not support subelements! (ID %d unknown for %s)", startControl, widget()->className() );
#else
    Q_UNUSED(startControl);
#endif
    TQWidget *w = widget();
    switch ( dir ) {
    case NavFirstChild:
	{
	    TQObjectList *list = w->queryList( "TQWidget", 0, false, false );
	    bool has = !list->isEmpty();
	    delete list;
	    return has ? 1 : -1;
	}
    case NavLastChild:
	{
	    TQObjectList *list = w->queryList( "TQWidget", 0, false, false );
	    bool has = !list->isEmpty();
	    delete list;
	    return has ? childCount() : -1;
	}
    case NavNext:
    case NavPrevious:
	{
	    TQWidget *parent = w->parentWidget();
	    TQObjectList *sl = parent ? parent->queryList( "TQWidget", 0, false, false ) : 0;
	    if ( !sl )
		return -1;
	    TQObject *sib;
	    TQObjectListIt it( *sl );
	    int index;
	    if ( dir == NavNext ) {
		index = 1;
		while ( ( sib = it.current() ) ) {
		    ++it;
		    ++index;
		    if ( sib == w )
			break;
		}
	    } else {
		it.toLast();
		index = sl->count();
		while ( ( sib = it.current() ) ) {
		    --it;
		    --index;
		    if ( sib == w )
			break;
		}
	    }
	    sib = it.current();
	    delete sl;
	    if ( sib )
		return index;
	    return -1;
	}
	break;
    case NavFocusChild:
	{
	    if ( w->hasFocus() )
		return 0;

	    TQWidget *w2 = w->focusWidget();
	    if ( !w2 )
		return -1;

	    TQObjectList *list = w->queryList( "TQWidget", 0, false, false );
	    int index = list->findRef( w2 );
	    delete list;
	    return ( index != -1 ) ? index+1 : -1;
	}
    default:
	tqWarning( "TQAccessibleWidget::navigate: unhandled request" );
	break;
    };
    return -1;
}

/*! \reimp */
int TQAccessibleWidget::childCount() const
{
    TQObjectList *cl = widget()->queryList( "TQWidget", 0, false, false );
    if ( !cl )
	return 0;

    int count = cl->count();
    delete cl;
    return count;
}

/*! \reimp */
TQRESULT TQAccessibleWidget::queryChild( int control, TQAccessibleInterface **iface ) const
{
    *iface = 0;
    TQObjectList *cl = widget()->queryList( "TQWidget", 0, false, false );
    if ( !cl )
	return TQS_FALSE;

    TQObject *o = 0;
    if ( cl->count() >= (uint)control )
	o = cl->at( control-1 );
    delete cl;

    if ( !o )
	return TQS_FALSE;

    return TQAccessible::queryAccessibleInterface( o, iface );
}

/*! \reimp */
TQRESULT TQAccessibleWidget::queryParent( TQAccessibleInterface **iface ) const
{
    return TQAccessible::queryAccessibleInterface( widget()->parentWidget(), iface );
}

/*! \reimp */
bool TQAccessibleWidget::doDefaultAction( int control )
{
#if defined(QT_DEBUG)
    if ( control )
	tqWarning( "TQAccessibleWidget::doDefaultAction: This implementation does not support subelements! (ID %d unknown for %s)", control, widget()->className() );
#else
    Q_UNUSED(control)
#endif
    return false;
}

/*! \reimp */
TQString TQAccessibleWidget::text( Text t, int control ) const
{
    switch ( t ) {
    case DefaultAction:
	return defAction_;
    case Description:
	if ( !control && description_.isNull() ) {
	    TQString desc = TQToolTip::textFor( widget() );
	    return desc;
	}
	return description_;
    case Help:
	if ( !control && help_.isNull() ) {
	    TQString help = TQWhatsThis::textFor( widget() );
	    return help;
	}
	return help_;
    case Accelerator:
	return accelerator_;
    case Name:
	{
	    if ( !control && name_.isNull() && widget()->isTopLevel() )
		return widget()->caption();
	    return name_;
	}
    case Value:
	return value_;
    default:
	break;
    }
    return TQString::null;
}

/*! \reimp */
void TQAccessibleWidget::setText( Text t, int /*control*/, const TQString &text )
{
    switch ( t ) {
    case DefaultAction:
	defAction_ = text;
	break;
    case Description:
	description_ = text;
	break;
    case Help:
	help_ = text;
	break;
    case Accelerator:
	accelerator_ = text;
	break;
    case Name:
	name_ = text;
	break;
    case Value:
	value_ = text;
	break;
    default:
	break;
    }
}

/*! \reimp */
TQAccessible::Role TQAccessibleWidget::role( int control ) const
{
    if ( !control )
	return role_;
    return NoRole;
}

/*! \reimp */
TQAccessible::State TQAccessibleWidget::state( int control ) const
{
    if ( control )
	return Normal;

    if ( state_ != Normal )
	return state_;

    int state = Normal;

    TQWidget *w = widget();
    if ( w->isHidden() )
	state |= Invisible;
    if ( w->focusPolicy() != TQWidget::NoFocus && w->isActiveWindow() )
	state |= Focusable;
    if ( w->hasFocus() )
	state |= Focused;
    if ( !w->isEnabled() )
	state |= Unavailable;
    if ( w->isTopLevel() ) {
	state |= Moveable;
	if ( w->minimumSize() != w->maximumSize() )
	    state |= Sizeable;
    }

    return (State)state;
}

/*! \reimp */
bool TQAccessibleWidget::setFocus( int control )
{
#if defined(QT_DEBUG)
    if ( control )
	tqWarning( "TQAccessibleWidget::setFocus: This implementation does not support subelements! (ID %d unknown for %s)", control, widget()->className() );
#else
    Q_UNUSED(control)
#endif
    if ( widget()->focusPolicy() != TQWidget::NoFocus ) {
	widget()->setFocus();
	return true;
    }
    return false;
}

/*! \reimp */
bool TQAccessibleWidget::setSelected( int, bool, bool )
{
#if defined(QT_DEBUG)
    tqWarning( "TQAccessibleWidget::setSelected: This function not supported for simple widgets." );
#endif
    return false;
}

/*! \reimp */
void TQAccessibleWidget::clearSelection()
{
#if defined(QT_DEBUG)
    tqWarning( "TQAccessibleWidget::clearSelection: This function not supported for simple widgets." );
#endif
}

/*! \reimp */
TQMemArray<int> TQAccessibleWidget::selection() const
{
    return TQMemArray<int>();
}

/*!
  \class TQAccessibleWidgetStack ntqaccessible.h
  \brief The TQAccessibleWidgetStack class implements the TQAccessibleInterface for widget stacks.
*/

/*!
  Creates a TQAccessibleWidgetStack object for \a o.
*/
TQAccessibleWidgetStack::TQAccessibleWidgetStack( TQObject *o )
: TQAccessibleWidget( o )
{
    Q_ASSERT( o->inherits("TQWidgetStack") );
}

/*! Returns the widget stack. */
TQWidgetStack *TQAccessibleWidgetStack::widgetStack() const
{
    return (TQWidgetStack*)object();
}

/*! \reimp */
int TQAccessibleWidgetStack::controlAt( int, int ) const
{
    return widgetStack()->id( widgetStack()->visibleWidget() ) + 1;
}

/*! \reimp */
TQRESULT TQAccessibleWidgetStack::queryChild( int control, TQAccessibleInterface **iface ) const
{
    if ( !control ) {
	*iface = (TQAccessibleInterface*)this;
	return TQS_OK;
    }

    TQWidget *widget = widgetStack()->widget( control - 1 );
    if ( !widget )
	return TQAccessibleWidget::queryChild( control, iface );
    return TQAccessible::queryAccessibleInterface( widgetStack()->widget( control - 1 ), iface );
}


/*!
  \class TQAccessibleButton ntqaccessible.h
  \brief The TQAccessibleButton class implements the TQAccessibleInterface for button type widgets.
*/

/*!
  Creates a TQAccessibleButton object for \a o.
  \a role, \a description and \a help are propagated to the TQAccessibleWidget constructor.
*/
TQAccessibleButton::TQAccessibleButton( TQObject *o, Role role, TQString description,
				     TQString /* help */ )
: TQAccessibleWidget( o, role, TQString::null, description, TQString::null,
		    TQString::null, TQString::null, TQString::null )
{
    Q_ASSERT(o->inherits("TQButton"));
}

/*! \reimp */
bool	TQAccessibleButton::doDefaultAction( int control )
{
    if ( !widget()->isEnabled() )
	return false;

    Role r = role(control);
    if ( r == PushButton || r ==  CheckBox || r == RadioButton ) {
	((TQButton*)object())->animateClick();
    } else if ( object()->inherits("TQToolButton") ) {
	TQToolButton *tb = (TQToolButton*)object();
	tb->openPopup();
    }

    return true;
}

/*! \reimp */
TQString TQAccessibleButton::text( Text t, int control ) const
{
    TQString tx = TQAccessibleWidget::text( t, control );
    if ( !!tx )
	return tx;

    switch ( t ) {
    case DefaultAction:
	switch( role(control) ) {
	case PushButton:
	    return TQButton::tr("Press");
	case CheckBox:
	    if ( state(control) & Checked )
		return TQButton::tr("UnCheck");
	    return TQButton::tr("Check");
	case RadioButton:
	    return TQButton::tr("Check");
	default:
	    return TQButton::tr("Press");
	}
    case Accelerator:
	tx = hotKey( ((TQButton*)widget())->text() );
	if ( !!tx ) {
	    tx = "Alt + "+tx;
	} else {
	    tx = hotKey( buddyString( widget() ) );
	    if ( !!tx )
		tx = "Alt + "+tx;
	}
	return tx;
    case Name:
	tx = ((TQButton*)widget())->text();
	if ( tx.isEmpty() && widget()->inherits("TQToolButton") )
	    tx = ((TQToolButton*)widget())->textLabel();
	if ( tx.isEmpty() )
	    tx = buddyString( widget() );

	return stripAmp( tx );
    default:
	break;
    }
    return tx;
}

/*! \reimp */
TQAccessible::State TQAccessibleButton::state( int control ) const
{
    int state = TQAccessibleWidget::state( control );

    TQButton *b = (TQButton*)widget();
    if ( b->state() == TQButton::On )
	state |= Checked;
    else  if ( b->state() == TQButton::NoChange )
	    state |= Mixed;
    if ( b->isDown() )
	state |= Pressed;
    if ( b->inherits( "TQPushButton" ) ) {
	TQPushButton *pb = (TQPushButton*)b;
	if ( pb->isDefault() )
	    state |= Default;
    }

    return (State)state;
}

/*!
  \class TQAccessibleRangeControl qaccessiblewidget.h
  \brief The TQAccessibleRangeControl class implements the TQAccessibleInterface for range controls.
*/

/*!
  Constructs a TQAccessibleRangeControl object for \a o.
  \a role, \a name, \a description, \a help, \a defAction and \a accelerator
  are propagated to the TQAccessibleWidget constructor.
*/
TQAccessibleRangeControl::TQAccessibleRangeControl( TQObject *o, Role role, TQString name,
						 TQString description, TQString help, TQString defAction, TQString accelerator )
: TQAccessibleWidget( o, role, name, description, TQString::null, help, defAction, accelerator )
{
}

/*! \reimp */
TQString TQAccessibleRangeControl::text( Text t, int control ) const
{
    TQString tx = TQAccessibleWidget::text( t, control );
    if ( !!tx )
	return stripAmp(tx);

    switch ( t ) {
    case Name:
	return stripAmp( buddyString( widget() ) );
    case Accelerator:
	tx = hotKey( buddyString( widget() ) );
	if ( !!tx )
	    return "Alt + "+tx;
	break;
    case Value:
	if ( widget()->inherits( "TQSlider" ) ) {
	    TQSlider *s = (TQSlider*)widget();
	    return TQString::number( s->value() );
	} else if ( widget()->inherits( "TQDial" ) ) {
	    TQDial *d = (TQDial*)widget();
	    return TQString::number( d->value() );
	} else if ( widget()->inherits( "TQSpinBox" ) ) {
	    TQSpinBox *s = (TQSpinBox*)widget();
	    return s->text();
	} else if ( widget()->inherits( "TQScrollBar" ) ) {
	    TQScrollBar *s = (TQScrollBar*)widget();
	    return TQString::number( s->value() );
	} else if ( widget()->inherits( "TQProgressBar" ) ) {
	    TQProgressBar *p = (TQProgressBar*)widget();
	    return TQString::number( p->progress() );
	}
    default:
	break;
    }
    return tx;
}


/*!
  \class TQAccessibleSpinWidget qaccessiblewidget.h
  \brief The TQAccessibleSpinWidget class implements the TQAccessibleInterface for up/down widgets.
*/

/*!
  Constructs a TQAccessibleSpinWidget object for \a o.
*/
TQAccessibleSpinWidget::TQAccessibleSpinWidget( TQObject *o )
: TQAccessibleRangeControl( o, SpinBox )
{
}

/*! \reimp */
int TQAccessibleSpinWidget::controlAt( int x, int y ) const
{
    TQPoint tl = widget()->mapFromGlobal( TQPoint( x, y ) );
    if ( ((TQSpinWidget*)widget())->upRect().contains( tl ) )
	return 1;
    else if ( ((TQSpinWidget*)widget())->downRect().contains( tl ) )
	return 2;

    return -1;
}

/*! \reimp */
TQRect TQAccessibleSpinWidget::rect( int control ) const
{
    TQRect rect;
    switch( control ) {
    case 1:
	rect = ((TQSpinWidget*)widget())->upRect();
	break;
    case 2:
	rect = ((TQSpinWidget*)widget())->downRect();
	break;
    default:
	rect = widget()->rect();
    }
    TQPoint tl = widget()->mapToGlobal( TQPoint( 0, 0 ) );
    return TQRect( tl.x() + rect.x(), tl.y() + rect.y(), rect.width(), rect.height() );
}

/*! \reimp */
int TQAccessibleSpinWidget::navigate( NavDirection direction, int startControl ) const
{
    if ( direction != NavFirstChild && direction != NavLastChild && direction != NavFocusChild && !startControl )
	return TQAccessibleWidget::navigate( direction, startControl );

    switch ( direction ) {
    case NavFirstChild:
	return 1;
    case NavLastChild:
	return 2;
    case NavNext:
    case NavDown:
	startControl += 1;
	if ( startControl > 2 )
	    return -1;
	return startControl;
    case NavPrevious:
    case NavUp:
	startControl -= 1;
	if ( startControl < 1 )
	    return -1;
	return startControl;
    default:
	break;
    }

    return -1;
}

/*! \reimp */
int TQAccessibleSpinWidget::childCount() const
{
    return 2;
}

/*! \reimp */
TQRESULT TQAccessibleSpinWidget::queryChild( int /*control*/, TQAccessibleInterface **iface ) const
{
    *iface = 0;
    return TQS_FALSE;
}

/*! \reimp */
TQString TQAccessibleSpinWidget::text( Text t, int control ) const
{
    switch ( t ) {
    case Name:
	switch ( control ) {
	case 1:
	    return TQSpinWidget::tr("More");
	case 2:
	    return TQSpinWidget::tr("Less");
	default:
	    break;
	}
	break;
    case DefaultAction:
	switch( control ) {
	case 1:
	case 2:
	    return TQSpinWidget::tr("Press");
	default:
	    break;
	}
	break;
    default:
	break;
    }
    return TQAccessibleRangeControl::text( t, control );
}

/*! \reimp */
TQAccessible::Role TQAccessibleSpinWidget::role( int control ) const
{
    switch( control ) {
    case 1:
	return PushButton;
    case 2:
	return PushButton;
    default:
	break;
    }
    return TQAccessibleRangeControl::role( control );
}

/*! \reimp */
TQAccessible::State TQAccessibleSpinWidget::state( int control ) const
{
    int state = TQAccessibleRangeControl::state( control );
    switch( control ) {
    case 1:
	if ( !((TQSpinWidget*)widget())->isUpEnabled() )
	    state |= Unavailable;
	return (State)state;
    case 2:
	if ( !((TQSpinWidget*)widget())->isDownEnabled() )
	    state |= Unavailable;
	return (State)state;
    default:
	break;
    }
    return TQAccessibleRangeControl::state( control );
}

/*! \reimp */
bool TQAccessibleSpinWidget::doDefaultAction( int control )
{
    switch( control ) {
    case 1:
	if ( !((TQSpinWidget*)widget())->isUpEnabled() )
	    return false;
	((TQSpinWidget*)widget())->stepUp();
	return true;
    case 2:
	if ( !((TQSpinWidget*)widget())->isDownEnabled() )
	    return false;
	((TQSpinWidget*)widget())->stepDown();
	return true;
    default:
	break;
    }
    return TQAccessibleRangeControl::doDefaultAction( control );
}

/*!
  \class TQAccessibleScrollBar qaccessiblewidget.h
  \brief The TQAccessibleScrollBar class implements the TQAccessibleInterface for scroll bars.
*/

/*!
  Constructs a TQAccessibleScrollBar object for \a o.
  \a name, \a description, \a help, \a defAction and \a accelerator
  are propagated to the TQAccessibleRangeControl constructor.
*/
TQAccessibleScrollBar::TQAccessibleScrollBar( TQObject *o, TQString name,
    TQString description, TQString help, TQString defAction, TQString accelerator )
: TQAccessibleRangeControl( o, ScrollBar, name, description, help, defAction, accelerator )
{
    Q_ASSERT( o->inherits("TQScrollBar" ) );
}

/*! Returns the scroll bar. */
TQScrollBar *TQAccessibleScrollBar::scrollBar() const
{
    return (TQScrollBar*)widget();
}

/*! \reimp */
int TQAccessibleScrollBar::controlAt( int x, int y ) const
{
    for ( int i = 1; i <= childCount(); i++ ) {
	if ( rect(i).contains( x,y ) )
	    return i;
    }
    return 0;
}

/*! \reimp */
TQRect TQAccessibleScrollBar::rect( int control ) const
{
    TQRect rect;
    TQRect srect = scrollBar()->sliderRect();
    int sz = scrollBar()->style().pixelMetric( TQStyle::PM_ScrollBarExtent, scrollBar() );
    switch ( control ) {
    case 1:
	if ( scrollBar()->orientation() == Vertical )
	    rect = TQRect( 0, 0, sz, sz );
	else
	    rect = TQRect( 0, 0, sz, sz );
	break;
    case 2:
	if ( scrollBar()->orientation() == Vertical )
	    rect = TQRect( 0, sz, sz, srect.y() - sz );
	else
	    rect = TQRect( sz, 0, srect.x() - sz, sz );
	break;
    case 3:
	rect = srect;
	break;
    case 4:
	if ( scrollBar()->orientation() == Vertical )
	    rect = TQRect( 0, srect.bottom(), sz, scrollBar()->rect().height() - srect.bottom() - sz );
	else
	    rect = TQRect( srect.right(), 0, scrollBar()->rect().width() - srect.right() - sz, sz ) ;
	break;
    case 5:
	if ( scrollBar()->orientation() == Vertical )
	    rect = TQRect( 0, scrollBar()->rect().height() - sz, sz, sz );
	else
	    rect = TQRect( scrollBar()->rect().width() - sz, 0, sz, sz );
	break;
    default:
	return TQAccessibleRangeControl::rect( control );
    }

    TQPoint tp = scrollBar()->mapToGlobal( TQPoint( 0,0 ) );
    return TQRect( tp.x() + rect.x(), tp.y() + rect.y(), rect.width(), rect.height() );
}

/*! \reimp */
int TQAccessibleScrollBar::navigate( NavDirection direction, int startControl ) const
{
    if ( direction != NavFirstChild && direction != NavLastChild && direction != NavFocusChild && !startControl )
	return TQAccessibleRangeControl::navigate( direction, startControl );

    switch ( direction ) {
    case NavFirstChild:
	return 1;
    case NavLastChild:
	return 5;
    case NavNext:
	return startControl == childCount() ? -1 : startControl + 1;
    case NavDown:
	if ( scrollBar()->orientation() == Horizontal )
	    break;
	return startControl == childCount() ? -1 : startControl + 1;
    case NavRight:
	if ( scrollBar()->orientation() == Vertical )
	    break;
	return startControl == childCount() ? -1 : startControl + 1;
    case NavPrevious:
	return startControl == 1 ? -1 : startControl - 1;
    case NavUp:
	if ( scrollBar()->orientation() == Horizontal )
	    break;
	return startControl == 1 ? -1 : startControl - 1;
    case NavLeft:
	if ( scrollBar()->orientation() == Vertical )
	    break;
	return startControl == 1 ? -1 : startControl - 1;
    default:
	break;
    }

    return -1;
}

/*! \reimp */
int TQAccessibleScrollBar::childCount() const
{
    return 5;
}

/*! \reimp */
TQRESULT	TQAccessibleScrollBar::queryChild( int /*control*/, TQAccessibleInterface **iface ) const
{
    *iface = 0;
    return TQS_FALSE;
}

/*! \reimp */
TQString	TQAccessibleScrollBar::text( Text t, int control ) const
{
    switch ( t ) {
    case Value:
	if ( control && control != 3 )
	    return TQString::null;
	break;
    case Name:
	switch ( control ) {
	case 1:
	    return TQScrollBar::tr("Line up");
	case 2:
	    return TQScrollBar::tr("Page up");
	case 3:
	    return TQScrollBar::tr("Position");
	case 4:
	    return TQScrollBar::tr("Page down");
	case 5:
	    return TQScrollBar::tr("Line down");
	}
	break;
    case DefaultAction:
	if ( control != 3 )
	    return TQScrollBar::tr("Press");
	break;
    default:
	break;

    }
    return TQAccessibleRangeControl::text( t, control );
}

/*! \reimp */
TQAccessible::Role TQAccessibleScrollBar::role( int control ) const
{
    switch ( control ) {
    case 1:
    case 2:
	return PushButton;
    case 3:
	return Indicator;
    case 4:
    case 5:
	return PushButton;
    default:
	return ScrollBar;
    }
}

/*! \reimp */
bool TQAccessibleScrollBar::doDefaultAction( int control )
{
    switch ( control ) {
    case 1:
	scrollBar()->subtractLine();
	return true;
    case 2:
	scrollBar()->subtractPage();
	return true;
    case 4:
	scrollBar()->addPage();
	return true;
    case 5:
	scrollBar()->addLine();
	return true;
    default:
	return false;
    }
}

/*!
  \class TQAccessibleSlider qaccessiblewidget.h
  \brief The TQAccessibleScrollBar class implements the TQAccessibleInterface for sliders.
*/

/*!
  Constructs a TQAccessibleScrollBar object for \a o.
  \a name, \a description, \a help, \a defAction and \a accelerator
  are propagated to the TQAccessibleRangeControl constructor.
*/
TQAccessibleSlider::TQAccessibleSlider( TQObject *o, TQString name,
    TQString description, TQString help, TQString defAction, TQString accelerator )
: TQAccessibleRangeControl( o, ScrollBar, name, description, help, defAction, accelerator )
{
    Q_ASSERT( o->inherits("TQSlider" ) );
}

/*! Returns the slider. */
TQSlider *TQAccessibleSlider::slider() const
{
    return (TQSlider*)widget();
}

/*! \reimp */
int TQAccessibleSlider::controlAt( int x, int y ) const
{
    for ( int i = 1; i <= childCount(); i++ ) {
	if ( rect(i).contains( x,y ) )
	    return i;
    }
    return 0;
}

/*! \reimp */
TQRect TQAccessibleSlider::rect( int control ) const
{
    TQRect rect;
    TQRect srect = slider()->sliderRect();
    switch ( control ) {
    case 1:
	if ( slider()->orientation() == Vertical )
	    rect = TQRect( 0, 0, slider()->width(), srect.y() );
	else
	    rect = TQRect( 0, 0, srect.x(), slider()->height() );
	break;
    case 2:
	rect = srect;
	break;
    case 3:
	if ( slider()->orientation() == Vertical )
	    rect = TQRect( 0, srect.y() + srect.height(), slider()->width(), slider()->height()- srect.y() - srect.height() );
	else
	    rect = TQRect( srect.x() + srect.width(), 0, slider()->width() - srect.x() - srect.width(), slider()->height() );
	break;
    default:
	return TQAccessibleRangeControl::rect( control );
    }

    TQPoint tp = slider()->mapToGlobal( TQPoint( 0,0 ) );
    return TQRect( tp.x() + rect.x(), tp.y() + rect.y(), rect.width(), rect.height() );
}

/*! \reimp */
int TQAccessibleSlider::navigate( NavDirection direction, int startControl ) const
{
    if ( direction != NavFirstChild && direction != NavLastChild && direction != NavFocusChild && !startControl )
	return TQAccessibleRangeControl::navigate( direction, startControl );

    switch ( direction ) {
    case NavFirstChild:
	return 1;
    case NavLastChild:
	return childCount();
    case NavNext:
	return startControl == childCount() ? -1 : startControl + 1;
    case NavDown:
	if ( slider()->orientation() == Horizontal )
	    break;
	return startControl == childCount() ? -1 : startControl + 1;
    case NavRight:
	if ( slider()->orientation() == Vertical )
	    break;
	return startControl == childCount() ? -1 : startControl + 1;
    case NavPrevious:
	return startControl == 1 ? -1 : startControl - 1;
    case NavUp:
	if ( slider()->orientation() == Horizontal )
	    break;
	return startControl == 1 ? -1 : startControl - 1;
    case NavLeft:
	if ( slider()->orientation() == Vertical )
	    break;
	return startControl == 1 ? -1 : startControl - 1;
    default:
	break;
    }

    return -1;
}

/*! \reimp */
int TQAccessibleSlider::childCount() const
{
    return 3;
}

/*! \reimp */
TQRESULT	TQAccessibleSlider::queryChild( int /*control*/, TQAccessibleInterface **iface ) const
{
    *iface = 0;
    return TQS_FALSE;
}

/*! \reimp */
TQString	TQAccessibleSlider::text( Text t, int control ) const
{
    switch ( t ) {
    case Value:
	if ( control && control != 2 )
	    return TQString::null;
	break;
    case Name:
	switch ( control ) {
	case 1:
	    return TQSlider::tr("Page up");
	case 2:
	    return TQSlider::tr("Position");
	case 3:
	    return TQSlider::tr("Page down");
	}
	break;
    case DefaultAction:
	if ( control != 2 )
	    return TQSlider::tr("Press");
	break;
    default:
	break;
    }
    return TQAccessibleRangeControl::text( t, control );
}

/*! \reimp */
TQAccessible::Role TQAccessibleSlider::role( int control ) const
{
    switch ( control ) {
    case 1:
	return PushButton;
    case 2:
	return Indicator;
    case 3:
	return PushButton;
    default:
	return Slider;
    }
}

/*! \reimp */
bool TQAccessibleSlider::doDefaultAction( int control )
{
    switch ( control ) {
    case 1:
	slider()->subtractLine();
	return true;
    case 3:
	slider()->addLine();
	return true;
    default:
	return false;
    }
}


/*!
  \class TQAccessibleText qaccessiblewidget.h
  \brief The TQAccessibleText class implements the TQAccessibleInterface for widgets with editable text.
*/

/*!
  Constructs a TQAccessibleText object for \a o.
  \a role, \a name, \a description, \a help, \a defAction and \a accelerator
  are propagated to the TQAccessibleWidget constructor.
*/
TQAccessibleText::TQAccessibleText( TQObject *o, Role role, TQString name, TQString description, TQString help, TQString defAction, TQString accelerator )
: TQAccessibleWidget( o, role, name, description, TQString::null, help, defAction, accelerator )
{
}

/*! \reimp */
TQString TQAccessibleText::text( Text t, int control ) const
{
    TQString str = TQAccessibleWidget::text( t, control );
    if ( !!str )
	return str;
    switch ( t ) {
    case Name:
	return stripAmp( buddyString( widget() ) );
    case Accelerator:
	str = hotKey( buddyString( widget() ) );
	if ( !!str )
	    return "Alt + "+str;
	break;
    case Value:
	if ( widget()->inherits( "TQLineEdit" ) )
	    return ((TQLineEdit*)widget())->text();
	break;
    default:
	break;
    }
    return str;
}

/*! \reimp */
void TQAccessibleText::setText(Text t, int control, const TQString &text)
{
    if (t != Value || !widget()->inherits("TQLineEdit") || control) {
        TQAccessibleWidget::setText(t, control, text);
        return;
    }
    ((TQLineEdit*)widget())->setText(text);
}

/*! \reimp */
TQAccessible::State TQAccessibleText::state( int control ) const
{
    int state = TQAccessibleWidget::state( control );

    if ( widget()->inherits( "TQLineEdit" ) ) {
	TQLineEdit *l = (TQLineEdit*)widget();
	if ( l->isReadOnly() )
	    state |= ReadOnly;
	if ( l->echoMode() == TQLineEdit::Password )
	    state |= Protected;
	state |= Selectable;
	if ( l->hasSelectedText() )
	    state |= Selected;
    }

    return (State)state;
}

/*!
  \class TQAccessibleDisplay qaccessiblewidget.h
  \brief The TQAccessibleDisplay class implements the TQAccessibleInterface for widgets that display static information.
*/

/*!
  Constructs a TQAccessibleDisplay object for \a o.
  \a role, \a description, \a value, \a help, \a defAction and \a accelerator
  are propagated to the TQAccessibleWidget constructor.
*/
TQAccessibleDisplay::TQAccessibleDisplay( TQObject *o, Role role, TQString description, TQString value, TQString help, TQString defAction, TQString accelerator )
: TQAccessibleWidget( o, role, TQString::null, description, value, help, defAction, accelerator )
{
}

/*! \reimp */
TQAccessible::Role TQAccessibleDisplay::role( int control ) const
{
    if ( widget()->inherits( "TQLabel" ) ) {
	TQLabel *l = (TQLabel*)widget();
	if ( l->pixmap() || l->picture() )
	    return Graphic;
#ifndef TQT_NO_PICTURE
	if ( l->picture() )
	    return Graphic;
#endif
#ifndef TQT_NO_MOVIE
	if ( l->movie() )
	    return Animation;
#endif
    }
    return TQAccessibleWidget::role( control );
}

/*! \reimp */
TQString TQAccessibleDisplay::text( Text t, int control ) const
{
    TQString str = TQAccessibleWidget::text( t, control );
    if ( !!str )
	return str;

    switch ( t ) {
    case Name:
	if ( widget()->inherits( "TQLabel" ) ) {
	    return stripAmp( ((TQLabel*)widget())->text() );
	} else if ( widget()->inherits( "TQLCDNumber" ) ) {
	    TQLCDNumber *l = (TQLCDNumber*)widget();
	    if ( l->numDigits() )
		return TQString::number( l->value() );
	    return TQString::number( l->intValue() );
	} else if ( widget()->inherits( "TQGroupBox" ) ) {
	    return stripAmp( ((TQGroupBox*)widget())->title() );
	}
	break;
    default:
	break;
    }
    return str;
}


/*!
  \class TQAccessibleHeader qaccessiblewidget.h
  \brief The TQAccessibleHeader class implements the TQAccessibleInterface for header widgets.
*/

/*!
  Constructs a TQAccessibleHeader object for \a o.
  \a role, \a description, \a value, \a help, \a defAction and \a accelerator
  are propagated to the TQAccessibleWidget constructor.
*/
TQAccessibleHeader::TQAccessibleHeader( TQObject *o, TQString description,
    TQString value, TQString help, TQString defAction, TQString accelerator )
    : TQAccessibleWidget( o, NoRole, description, value, help, defAction, accelerator )
{
    Q_ASSERT(widget()->inherits("TQHeader"));
}

/*! Returns the TQHeader. */
TQHeader *TQAccessibleHeader::header() const
{
    return (TQHeader *)widget();
}

/*! \reimp */
int TQAccessibleHeader::controlAt( int x, int y ) const
{
    TQPoint point = header()->mapFromGlobal( TQPoint( x, y ) );
    for ( int i = 0; i < header()->count(); i++ ) {
	if ( header()->sectionRect( i ).contains( point ) )
	    return i+1;
    }
    return -1;
}

/*! \reimp */
TQRect TQAccessibleHeader::rect( int control ) const
{
    TQPoint zero = header()->mapToGlobal( TQPoint ( 0,0 ) );
    TQRect sect = header()->sectionRect( control - 1 );
    return TQRect( sect.x() + zero.x(), sect.y() + zero.y(), sect.width(), sect.height() );
}

/*! \reimp */
int TQAccessibleHeader::navigate( NavDirection direction, int startControl ) const
{
    if ( direction != NavFirstChild && direction != NavLastChild && direction != NavFocusChild && !startControl )
	return TQAccessibleWidget::navigate( direction, startControl );

    int count = header()->count();
    switch ( direction ) {
    case NavFirstChild:
	return 1;
    case NavLastChild:
	return count;
    case NavNext:
	return startControl + 1 > count ? -1 : startControl + 1;
    case NavPrevious:
	return startControl - 1 < 1 ? -1 : startControl - 1;
    case NavUp:
	if ( header()->orientation() == Vertical )
	    return startControl - 1 < 1 ? -1 : startControl - 1;
	return -1;
    case NavDown:
	if ( header()->orientation() == Vertical )
	    return startControl + 1 > count ? -1 : startControl + 1;
	break;
    case NavLeft:
	if ( header()->orientation() == Horizontal )
	    return startControl - 1 < 1 ? -1 : startControl - 1;
	break;
    case NavRight:
	if ( header()->orientation() == Horizontal )
	    return startControl + 1 > count ? -1 : startControl + 1;
	break;
    default:
	break;
    }
    return -1;
}

/*! \reimp */
int TQAccessibleHeader::childCount() const
{
    return header()->count();
}

/*! \reimp */
TQRESULT TQAccessibleHeader::queryChild( int /*control*/, TQAccessibleInterface **iface ) const
{
    *iface = 0;
    return TQS_FALSE;
}

/*! \reimp */
TQString TQAccessibleHeader::text( Text t, int control ) const
{
    TQString str = TQAccessibleWidget::text( t, control );
    if ( !!str )
	return str;

    switch ( t ) {
    case Name:
	return header()->label( control - 1 );
    default:
	break;
    }
    return str;
}

/*! \reimp */
TQAccessible::Role TQAccessibleHeader::role( int /*control*/ ) const
{
    if ( header()->orientation() == TQt::Horizontal )
	return ColumnHeader;
    else
	return RowHeader;
}

/*! \reimp */
TQAccessible::State TQAccessibleHeader::state( int control ) const
{
    return TQAccessibleWidget::state( control );
}


/*!
  \class TQAccessibleTabBar qaccessiblewidget.h
  \brief The TQAccessibleTabBar class implements the TQAccessibleInterface for tab bars.
*/

/*!
  Constructs a TQAccessibleTabBar object for \a o.
  \a role, \a description, \a value, \a help, \a defAction and \a accelerator
  are propagated to the TQAccessibleWidget constructor.
*/
TQAccessibleTabBar::TQAccessibleTabBar( TQObject *o, TQString description,
    TQString value, TQString help, TQString defAction, TQString accelerator )
    : TQAccessibleWidget( o, NoRole, description, value, help, defAction, accelerator )
{
    Q_ASSERT(widget()->inherits("TQTabBar"));
}

/*! Returns the TQHeader. */
TQTabBar *TQAccessibleTabBar::tabBar() const
{
    return (TQTabBar*)widget();
}

/*! \reimp */
int TQAccessibleTabBar::controlAt( int x, int y ) const
{
    int wc = TQAccessibleWidget::controlAt( x, y );
    if ( wc )
	return wc + tabBar()->count();

    TQPoint tp = tabBar()->mapFromGlobal( TQPoint( x,y ) );
    TQTab *tab = tabBar()->selectTab( tp );
    return tabBar()->indexOf( tab->identifier() ) + 1;
}

/*! \reimp */
TQRect TQAccessibleTabBar::rect( int control ) const
{
    if ( !control )
	return TQAccessibleWidget::rect( 0 );
    if ( control > tabBar()->count() ) {
	TQAccessibleInterface *iface;
	TQAccessibleWidget::queryChild( control - tabBar()->count(), &iface );
	if ( !iface )
	    return TQRect();
	return iface->rect( 0 );
    }

    TQTab *tab = tabBar()->tabAt( control - 1 );

    TQPoint tp = tabBar()->mapToGlobal( TQPoint( 0,0 ) );
    TQRect rec = tab->rect();
    return TQRect( tp.x() + rec.x(), tp.y() + rec.y(), rec.width(), rec.height() );
}

/*! \reimp */
TQRESULT	TQAccessibleTabBar::queryChild( int /*control*/, TQAccessibleInterface **iface ) const
{
    *iface = 0;
    return TQS_FALSE;
}

/*! \reimp */
int TQAccessibleTabBar::navigate( NavDirection direction, int startControl ) const
{
    if ( direction != NavFirstChild && direction != NavLastChild && direction != NavFocusChild && !startControl )
	return TQAccessibleWidget::navigate( direction, startControl );

    switch ( direction ) {
    case NavFirstChild:
	return 1;
	break;
    case NavLastChild:
	return childCount();
	break;
    case NavNext:
    case NavRight:
	return startControl + 1 > childCount() ? -1 : startControl + 1;
    case NavPrevious:
    case NavLeft:
	return startControl -1 < 1 ? -1 : startControl - 1;
    default:
	break;
    }
    return -1;
}

/*! \reimp */
int TQAccessibleTabBar::childCount() const
{
    int wc = TQAccessibleWidget::childCount();
    wc += tabBar()->count();
    return wc;
}

/*! \reimp */
TQString TQAccessibleTabBar::text( Text t, int control ) const
{
    TQString str = TQAccessibleWidget::text( t, control );
    if ( !!str )
	return str;

    if ( !control )
	return TQAccessibleWidget::text( t, control );
    if ( control > tabBar()->count() ) {
	TQAccessibleInterface *iface;
	TQAccessibleWidget::queryChild( control - tabBar()->count(), &iface );
	if ( !iface )
	    return TQAccessibleWidget::text( t, 0 );
	return iface->text( t, 0 );
    }

    TQTab *tab = tabBar()->tabAt( control - 1 );
    if ( !tab )
	return TQAccessibleWidget::text( t, 0 );

    switch ( t ) {
    case Name:
	return stripAmp( tab->text() );
    case DefaultAction:
	return TQTabBar::tr( "Switch" );
    default:
	break;
    }
    return str;
}

/*! \reimp */
TQAccessible::Role TQAccessibleTabBar::role( int control ) const
{
    if ( !control )
	return PageTabList;
    if ( control > tabBar()->count() ) {
	TQAccessibleInterface *iface;
	TQAccessibleWidget::queryChild( control - tabBar()->count(), &iface );
	if ( !iface )
	    return TQAccessibleWidget::role( 0 );
	return iface->role( 0 );
    }

    return PageTab;
}

/*! \reimp */
TQAccessible::State TQAccessibleTabBar::state( int control ) const
{
    int st = TQAccessibleWidget::state( 0 );

    if ( !control )
	return (State)st;
    if ( control > tabBar()->count() ) {
	TQAccessibleInterface *iface;
	TQAccessibleWidget::queryChild( control - tabBar()->count(), &iface );
	if ( !iface )
	    return (State)st;
	return iface->state( 0 );
    }

    TQTab *tab = tabBar()->tabAt( control - 1 );
    if ( !tab )
	return (State)st;

    if ( !tab->isEnabled() )
	st |= Unavailable;
    else
	st |= Selectable;

    if ( tabBar()->currentTab() == tab->identifier() )
	st |= Selected;

    return (State)st;
}

/*! \reimp */
bool TQAccessibleTabBar::doDefaultAction( int control )
{
    if ( !control )
	return false;
    if ( control > tabBar()->count() ) {
	TQAccessibleInterface *iface;
	TQAccessibleWidget::queryChild( control - tabBar()->count(), &iface );
	if ( !iface )
	    return false;
	return iface->doDefaultAction( 0 );
    }

    TQTab *tab = tabBar()->tabAt( control - 1 );
    if ( !tab || !tab->isEnabled() )
	return false;
    tabBar()->setCurrentTab( tab );
    return true;
}

/*! \reimp */
bool TQAccessibleTabBar::setSelected( int control, bool on, bool extend )
{
    if ( !control || !on || extend || control > tabBar()->count() )
	return false;

    TQTab *tab = tabBar()->tabAt( control - 1 );
    if ( !tab || !tab->isEnabled() )
	return false;
    tabBar()->setCurrentTab( tab );
    return true;
}

/*! \reimp */
void TQAccessibleTabBar::clearSelection()
{
}

/*! \reimp */
TQMemArray<int> TQAccessibleTabBar::selection() const
{
    TQMemArray<int> array( 1 );
    array.at(0) = tabBar()->indexOf( tabBar()->currentTab() ) + 1;

    return array;
}

/*!
  \class TQAccessibleComboBox qaccessiblewidget.h
  \brief The TQAccessibleComboBox class implements the TQAccessibleInterface for editable and read-only combo boxes.
*/


/*!
  Constructs a TQAccessibleComboBox object for \a o.
*/
TQAccessibleComboBox::TQAccessibleComboBox( TQObject *o )
: TQAccessibleWidget( o, ComboBox )
{
    Q_ASSERT(o->inherits("TQComboBox"));
}

/*!
  Returns the combo box.
*/
TQComboBox *TQAccessibleComboBox::comboBox() const
{
    return (TQComboBox*)object();
}

/*! \reimp */
int TQAccessibleComboBox::controlAt( int x, int y ) const
{
    for ( int i = childCount(); i >= 0; --i ) {
	if ( rect( i ).contains( x, y ) )
	    return i;
    }
    return -1;
}

/*! \reimp */
TQRect TQAccessibleComboBox::rect( int control ) const
{
    TQPoint tp;
    TQRect r;

    switch( control ) {
    case 1:
	if ( comboBox()->editable() ) {
	    tp = comboBox()->lineEdit()->mapToGlobal( TQPoint( 0,0 ) );
	    r = comboBox()->lineEdit()->rect();
	} else  {
	    tp = comboBox()->mapToGlobal( TQPoint( 0,0 ) );
	    r = comboBox()->style().querySubControlMetrics( TQStyle::CC_ComboBox, comboBox(), TQStyle::SC_ComboBoxEditField );
	}
	break;
    case 2:
	tp = comboBox()->mapToGlobal( TQPoint( 0,0 ) );
	r = comboBox()->style().querySubControlMetrics( TQStyle::CC_ComboBox, comboBox(), TQStyle::SC_ComboBoxArrow );
	break;
    default:
	return TQAccessibleWidget::rect( control );
    }
    return TQRect( tp.x() + r.x(), tp.y() + r.y(), r.width(), r.height() );
}

/*! \reimp */
int TQAccessibleComboBox::navigate( NavDirection direction, int startControl ) const
{
    if ( direction != NavFirstChild && direction != NavLastChild && direction != NavFocusChild && !startControl )
	return TQAccessibleWidget::navigate( direction, startControl );

    switch ( direction ) {
    case NavFirstChild:
	return 1;
	break;
    case NavLastChild:
	return childCount();
	break;
    case NavNext:
    case NavRight:
	return startControl + 1 > childCount() ? -1 : startControl + 1;
    case NavPrevious:
    case NavLeft:
	return startControl -1 < 1 ? -1 : startControl - 1;
    default:
	break;
    }
    return -1;
}

/*! \reimp */
int TQAccessibleComboBox::childCount() const
{
    return 2;
}

/*! \reimp */
TQRESULT	TQAccessibleComboBox::queryChild( int /*control*/, TQAccessibleInterface **iface ) const
{
    *iface = 0;
    return TQS_FALSE;
}

/*! \reimp */
TQString TQAccessibleComboBox::text( Text t, int control ) const
{
    TQString str;

    switch ( t ) {
    case Name:
	if ( control < 2 )
	    return stripAmp( buddyString( comboBox() ) );
	return TQComboBox::tr("Open");
    case Accelerator:
	if ( control < 2 ) {
	    str = hotKey( buddyString( comboBox() ) );
	    if ( !!str )
		return "Alt + " + str;
	    return str;
	}
	return TQComboBox::tr("Alt + Down Arrow" );
    case Value:
	if ( control < 2 ) {
	    if ( comboBox()->editable() )
		return comboBox()->lineEdit()->text();
	    return comboBox()->currentText();
	}
	break;
    case DefaultAction:
	if ( control == 2 )
	    return TQComboBox::tr("Open");
	break;
    default:
	str = TQAccessibleWidget::text( t, 0 );
	break;
    }
    return str;
}

/*! \reimp */
TQAccessible::Role TQAccessibleComboBox::role( int control ) const
{
    switch ( control ) {
    case 0:
	return ComboBox;
    case 1:
	if ( comboBox()->editable() )
	    return EditableText;
	return StaticText;
    case 2:
	return PushButton;
    default:
	return List;
    }
}

/*! \reimp */
TQAccessible::State TQAccessibleComboBox::state( int /*control*/ ) const
{
    return TQAccessibleWidget::state( 0 );
}

/*! \reimp */
bool TQAccessibleComboBox::doDefaultAction( int control )
{
    if ( control != 2 )
	return false;
    comboBox()->popup();
    return true;
}

/*!
  \class TQAccessibleTitleBar qaccessiblewidget.h
  \brief The TQAccessibleTitleBar class implements the TQAccessibleInterface for title bars.
*/

/*!
  Constructs a TQAccessibleComboBox object for \a o.
*/
TQAccessibleTitleBar::TQAccessibleTitleBar( TQObject *o )
: TQAccessibleWidget( o, ComboBox )
{
    Q_ASSERT(o->inherits("TQTitleBar"));
}

/*!
  Returns the title bar.
*/
TQTitleBar *TQAccessibleTitleBar::titleBar() const
{
    return (TQTitleBar*)object();
}

/*! \reimp */
int TQAccessibleTitleBar::controlAt( int x, int y ) const
{
    int ctrl = titleBar()->style().querySubControl( TQStyle::CC_TitleBar, titleBar(), titleBar()->mapFromGlobal( TQPoint( x,y ) ) );

    switch ( ctrl )
    {
    case TQStyle::SC_TitleBarSysMenu:
	return 1;
    case TQStyle::SC_TitleBarLabel:
	return 2;
    case TQStyle::SC_TitleBarMinButton:
	return 3;
    case TQStyle::SC_TitleBarMaxButton:
	return 4;
    case TQStyle::SC_TitleBarCloseButton:
	return 5;
    default:
	break;
    }
    return 0;
}

/*! \reimp */
TQRect TQAccessibleTitleBar::rect( int control ) const
{
    if ( !control )
	return TQAccessibleWidget::rect( control );

    TQRect r;
    switch ( control ) {
    case 1:
	r = titleBar()->style().querySubControlMetrics( TQStyle::CC_TitleBar, titleBar(), TQStyle::SC_TitleBarSysMenu );
	break;
    case 2:
	r = titleBar()->style().querySubControlMetrics( TQStyle::CC_TitleBar, titleBar(), TQStyle::SC_TitleBarLabel );
	break;
    case 3:
	r = titleBar()->style().querySubControlMetrics( TQStyle::CC_TitleBar, titleBar(), TQStyle::SC_TitleBarMinButton );
	break;
    case 4:
	r = titleBar()->style().querySubControlMetrics( TQStyle::CC_TitleBar, titleBar(), TQStyle::SC_TitleBarMaxButton );
	break;
    case 5:
	r = titleBar()->style().querySubControlMetrics( TQStyle::CC_TitleBar, titleBar(), TQStyle::SC_TitleBarCloseButton );
	break;
    default:
	break;
    }

    TQPoint tp = titleBar()->mapToGlobal( TQPoint( 0,0 ) );
    return TQRect( tp.x() + r.x(), tp.y() + r.y(), r.width(), r.height() );
}

/*! \reimp */
int TQAccessibleTitleBar::navigate( NavDirection direction, int startControl ) const
{
    if ( direction != NavFirstChild && direction != NavLastChild && direction != NavFocusChild && !startControl )
	return TQAccessibleWidget::navigate( direction, startControl );

    switch ( direction ) {
    case NavFirstChild:
	return 1;
	break;
    case NavLastChild:
	return childCount();
	break;
    case NavNext:
    case NavRight:
	return startControl + 1 > childCount() ? -1 : startControl + 1;
    case NavPrevious:
    case NavLeft:
	return startControl -1 < 1 ? -1 : startControl - 1;
    default:
	break;
    }
    return -1;
}

/*! \reimp */
int TQAccessibleTitleBar::childCount() const
{
    return 5;
}

/*! \reimp */
TQRESULT TQAccessibleTitleBar::queryChild( int /*control*/, TQAccessibleInterface **iface ) const
{
    *iface = 0;
    return TQS_FALSE;
}

/*! \reimp */
TQString TQAccessibleTitleBar::text( Text t, int control ) const
{
    TQString str = TQAccessibleWidget::text( t, control );
    if ( !!str )
	return str;

    switch ( t ) {
    case Name:
	switch ( control ) {
	case 1:
	    return TQTitleBar::tr("System");
	case 3:
	    if ( titleBar()->window()->isMinimized() )
		return TQTitleBar::tr("Restore up");
	    return TQTitleBar::tr("Minimize");
	case 4:
	    if ( titleBar()->window()->isMaximized() )
		return TQTitleBar::tr("Restore down");
	    return TQTitleBar::tr("Maximize");
	case 5:
	    return TQTitleBar::tr("Close");
	default:
	    break;
	}
	break;
    case Value:
	if ( !control || control == 2 )
	    return titleBar()->window()->caption();
	break;
    case DefaultAction:
	if ( control > 2 )
	    return TQTitleBar::tr("Press");
	break;
    case Description:
	switch ( control ) {
	case 1:
	    return TQTitleBar::tr("Contains commands to manipulate the window");
	case 3:
	    if ( titleBar()->window()->isMinimized() )
		return TQTitleBar::tr("Puts a minimized back to normal");
	    return TQTitleBar::tr("Moves the window out of the way");
	case 4:
	    if ( titleBar()->window()->isMaximized() )
		return TQTitleBar::tr("Puts a maximized window back to normal");
	    return TQTitleBar::tr("Makes the window full screen");
	case 5:
	    return TQTitleBar::tr("Closes the window");
	default:
	    return TQTitleBar::tr("Displays the name of the window and contains controls to manipulate it");
	}
    default:
	break;
    }
    return str;
}

/*! \reimp */
TQAccessible::Role TQAccessibleTitleBar::role( int control ) const
{
    switch ( control )
    {
    case 1:
    case 3:
    case 4:
    case 5:
	return PushButton;
    default:
	return TitleBar;
    }
}

/*! \reimp */
TQAccessible::State TQAccessibleTitleBar::state( int control ) const
{
    return TQAccessibleWidget::state( control );
}

/*! \reimp */
bool TQAccessibleTitleBar::doDefaultAction( int control )
{
    switch ( control ) {
    case 3:
	if ( titleBar()->window()->isMinimized() )
	    titleBar()->window()->showNormal();
	else
	    titleBar()->window()->showMinimized();
	return true;
    case 4:
	if ( titleBar()->window()->isMaximized() )
	    titleBar()->window()->showNormal();
	else
	    titleBar()->window()->showMaximized();
	return true;
    case 5:
	titleBar()->window()->close();
	return true;
    default:
	break;
    }
    return false;
}


/*!
  \class TQAccessibleViewport qaccessiblewidget.h
  \brief The TQAccessibleViewport class hides the viewport of scrollviews for accessibility.
  \internal
*/

TQAccessibleViewport::TQAccessibleViewport( TQObject *o, TQObject *sv )
    : TQAccessibleWidget( o )
{
    Q_ASSERT( sv->inherits("TQScrollView") );
    scrollview = (TQScrollView*)sv;
}

TQAccessibleScrollView *TQAccessibleViewport::scrollView() const
{
    TQAccessibleInterface *iface = 0;
    queryAccessibleInterface( scrollview, &iface );
    Q_ASSERT(iface);
    return (TQAccessibleScrollView *)iface;
}

int TQAccessibleViewport::controlAt( int x, int y ) const
{
    int control = TQAccessibleWidget::controlAt( x, y );
    if ( control > 0 )
	return control;

    TQPoint p = widget()->mapFromGlobal( TQPoint( x,y ) );
    return scrollView()->itemAt( p.x(), p.y() );
}

TQRect TQAccessibleViewport::rect( int control ) const
{
    if ( !control )
	return TQAccessibleWidget::rect( control );
    TQRect rect = scrollView()->itemRect( control );
    TQPoint tl = widget()->mapToGlobal( TQPoint( 0,0 ) );
    return TQRect( tl.x() + rect.x(), tl.y() + rect.y(), rect.width(), rect.height() );
}

int TQAccessibleViewport::navigate( NavDirection direction, int startControl ) const
{
    if ( direction != NavFirstChild && direction != NavLastChild && direction != NavFocusChild && !startControl )
	return TQAccessibleWidget::navigate( direction, startControl );

    // ### call itemUp/Down etc. here
    const int items = scrollView()->itemCount();
    switch( direction ) {
    case NavFirstChild:
	return 1;
    case NavLastChild:
	return items;
    case NavNext:
    case NavDown:
	return startControl + 1 > items ? -1 : startControl + 1;
    case NavPrevious:
    case NavUp:
	return startControl - 1 < 1 ? -1 : startControl - 1;
    default:
	break;
    }

    return -1;
}

int TQAccessibleViewport::childCount() const
{
    int widgets = TQAccessibleWidget::childCount();
    return widgets ? widgets : scrollView()->itemCount();
}

TQString TQAccessibleViewport::text( Text t, int control ) const
{
    return scrollView()->text( t, control );
}

bool TQAccessibleViewport::doDefaultAction( int control )
{
    return scrollView()->doDefaultAction( control );
}

TQAccessible::Role TQAccessibleViewport::role( int control ) const
{
    return scrollView()->role( control );
}

TQAccessible::State TQAccessibleViewport::state( int control ) const
{
    return scrollView()->state( control );
}

bool TQAccessibleViewport::setFocus( int control )
{
    return scrollView()->setFocus( control );
}

bool TQAccessibleViewport::setSelected( int control, bool on, bool extend )
{
    return scrollView()->setSelected( control, on, extend );
}

void TQAccessibleViewport::clearSelection()
{
    scrollView()->clearSelection();
}

TQMemArray<int> TQAccessibleViewport::selection() const
{
    return scrollView()->selection();
}

/*!
  \class TQAccessibleScrollView qaccessiblewidget.h
  \brief The TQAccessibleScrollView class implements the TQAccessibleInterface for scrolled widgets.
*/

/*!
  Constructs a TQAccessibleScrollView object for \a o.
  \a role, \a description, \a value, \a help, \a defAction and \a accelerator
  are propagated to the TQAccessibleWidget constructor.
*/
TQAccessibleScrollView::TQAccessibleScrollView( TQObject *o, Role role, TQString name,
    TQString description, TQString value, TQString help, TQString defAction, TQString accelerator )
    : TQAccessibleWidget( o, role, name, description, value, help, defAction, accelerator )
{
}

/*! \reimp */
TQString TQAccessibleScrollView::text( Text t, int control ) const
{
    TQString str = TQAccessibleWidget::text( t, control );
    if ( !!str )
	return str;
    switch ( t ) {
    case Name:
	return buddyString( widget() );
    default:
	break;
    }

    return str;
}

/*!
  Returns the ID of the item at viewport position \a x, \a y.
*/
int TQAccessibleScrollView::itemAt( int /*x*/, int /*y*/ ) const
{
    return 0;
}

/*!
  Returns the location of the item with ID \a item in viewport coordinates.
*/
TQRect TQAccessibleScrollView::itemRect( int /*item*/ ) const
{
    return TQRect();
}

/*!
  Returns the number of items.
*/
int TQAccessibleScrollView::itemCount() const
{
    return 0;
}

/*!
  \class TQAccessibleListBox qaccessiblewidget.h
  \brief The TQAccessibleListBox class implements the TQAccessibleInterface for list boxes.
*/

/*!
  Constructs a TQAccessibleListBox object for \a o.
*/
TQAccessibleListBox::TQAccessibleListBox( TQObject *o )
    : TQAccessibleScrollView( o, List )
{
    Q_ASSERT(widget()->inherits("TQListBox"));
}

/*! Returns the list box. */
TQListBox *TQAccessibleListBox::listBox() const
{
    return (TQListBox*)widget();
}

/*! \reimp */
int TQAccessibleListBox::itemAt( int x, int y ) const
{
    TQListBoxItem *item = listBox()->itemAt( TQPoint( x, y ) );
    return listBox()->index( item ) + 1;
}

/*! \reimp */
TQRect TQAccessibleListBox::itemRect( int item ) const
{
    return listBox()->itemRect( listBox()->item( item-1 ) );
}

/*! \reimp */
int TQAccessibleListBox::itemCount() const
{
    return listBox()->count();
}

/*! \reimp */
TQString TQAccessibleListBox::text( Text t, int control ) const
{
    if ( !control || t != Name )
	return TQAccessibleScrollView::text( t, control );

    TQListBoxItem *item = listBox()->item( control - 1 );
    if ( item )
	return item->text();
    return TQString::null;
}

/*! \reimp */
TQAccessible::Role TQAccessibleListBox::role( int control ) const
{
    if ( !control )
	return TQAccessibleScrollView::role( control );
    return ListItem;
}

/*! \reimp */
TQAccessible::State TQAccessibleListBox::state( int control ) const
{
    int state = TQAccessibleScrollView::state( control );
    TQListBoxItem *item;
    if ( !control || !( item = listBox()->item( control - 1 ) ) )
	return (State)state;

    if ( item->isSelectable() ) {
	if ( listBox()->selectionMode() == TQListBox::Multi )
	    state |= MultiSelectable;
	else if ( listBox()->selectionMode() == TQListBox::Extended )
	    state |= ExtSelectable;
	else if ( listBox()->selectionMode() == TQListBox::Single )
	    state |= Selectable;
	if ( item->isSelected() )
	    state |= Selected;
    }
    if ( listBox()->focusPolicy() != TQWidget::NoFocus ) {
	state |= Focusable;
	if ( item->isCurrent() )
	    state |= Focused;
    }
    if ( !listBox()->itemVisible( item ) )
	state |= Invisible;

    return (State)state;
}

/*! \reimp */
bool TQAccessibleListBox::setFocus( int control )
{
    bool res = TQAccessibleScrollView::setFocus( 0 );
    if ( !control || !res )
	return res;

    TQListBoxItem *item = listBox()->item( control -1 );
    if ( !item )
	return false;
    listBox()->setCurrentItem( item );
    return true;
}

/*! \reimp */
bool TQAccessibleListBox::setSelected( int control, bool on, bool extend )
{
    if ( !control || ( extend &&
	listBox()->selectionMode() != TQListBox::Extended &&
	listBox()->selectionMode() != TQListBox::Multi ) )
	return false;

    TQListBoxItem *item = listBox()->item( control -1 );
    if ( !item )
	return false;
    if ( !extend ) {
	listBox()->setSelected( item, on );
    } else {
	int current = listBox()->currentItem();
	bool down = control > current;
	for ( int i = current; i != control;) {
	    down ? i++ : i--;
	    listBox()->setSelected( i, on );
	}

    }
    return true;
}

/*! \reimp */
void TQAccessibleListBox::clearSelection()
{
    listBox()->clearSelection();
}

/*! \reimp */
TQMemArray<int> TQAccessibleListBox::selection() const
{
    TQMemArray<int> array;
    uint size = 0;
    const uint c = listBox()->count();
    array.resize( c );
    for ( uint i = 0; i < c; ++i ) {
	if ( listBox()->isSelected( i ) ) {
	    ++size;
	    array[ (int)size-1 ] = i+1;
	}
    }
    array.resize( size );
    return array;
}

/*!
  \class TQAccessibleListView qaccessiblewidget.h
  \brief The TQAccessibleListView class implements the TQAccessibleInterface for list views.
*/

static TQListViewItem *findLVItem( TQListView* listView, int control )
{
    int id = 1;
    TQListViewItemIterator it( listView );
    TQListViewItem *item = it.current();
    while ( item && id < control ) {
	++it;
	++id;
	item = it.current();
    }
    return item;
}

/*!
  Constructs a TQAccessibleListView object for \a o.
*/
TQAccessibleListView::TQAccessibleListView( TQObject *o )
    : TQAccessibleScrollView( o, Outline )
{
}

/*! Returns the list view. */
TQListView *TQAccessibleListView::listView() const
{
    Q_ASSERT(widget()->inherits("TQListView"));
    return (TQListView*)widget();
}

/*! \reimp */
int TQAccessibleListView::itemAt( int x, int y ) const
{
    TQListViewItem *item = listView()->itemAt( TQPoint( x, y ) );
    if ( !item )
	return 0;

    TQListViewItemIterator it( listView() );
    int c = 1;
    while ( it.current() ) {
	if ( it.current() == item )
	    return c;
	++c;
	++it;
    }
    return 0;
}

/*! \reimp */
TQRect TQAccessibleListView::itemRect( int control ) const
{
    TQListViewItem *item = findLVItem( listView(), control );
    if ( !item )
	return TQRect();
    return listView()->itemRect( item );
}

/*! \reimp */
int TQAccessibleListView::itemCount() const
{
    TQListViewItemIterator it( listView() );
    int c = 0;
    while ( it.current() ) {
	++c;
	++it;
    }

    return c;
}

/*! \reimp */
TQString TQAccessibleListView::text( Text t, int control ) const
{
    if ( !control || t != Name )
	return TQAccessibleScrollView::text( t, control );

    TQListViewItem *item = findLVItem( listView(), control );
    if ( !item )
	return TQString::null;
    return item->text( 0 );
}

/*! \reimp */
TQAccessible::Role TQAccessibleListView::role( int control ) const
{
    if ( !control )
	return TQAccessibleScrollView::role( control );
    return OutlineItem;
}

/*! \reimp */
TQAccessible::State TQAccessibleListView::state( int control ) const
{
    int state = TQAccessibleScrollView::state( control );
    TQListViewItem *item;
    if ( !control || !( item = findLVItem( listView(), control ) ) )
	return (State)state;

    if ( item->isSelectable() ) {
	if ( listView()->selectionMode() == TQListView::Multi )
	    state |= MultiSelectable;
	else if ( listView()->selectionMode() == TQListView::Extended )
	    state |= ExtSelectable;
	else if ( listView()->selectionMode() == TQListView::Single )
	    state |= Selectable;
	if ( item->isSelected() )
	    state |= Selected;
    }
    if ( listView()->focusPolicy() != TQWidget::NoFocus ) {
	state |= Focusable;
	if ( item == listView()->currentItem() )
	    state |= Focused;
    }
    if ( item->childCount() ) {
	if ( item->isOpen() )
	    state |= Expanded;
	else
	    state |= Collapsed;
    }
    if ( !listView()->itemRect( item ).isValid() )
	state |= Invisible;

    if ( item->rtti() == TQCheckListItem::RTTI ) {
	if ( ((TQCheckListItem*)item)->isOn() )
	    state|=Checked;
    }
    return (State)state;
}

/*! \reimp
TQAccessibleInterface *TQAccessibleListView::focusChild( int *control ) const
{
    TQListViewItem *item = listView()->currentItem();
    if ( !item )
	return 0;

    TQListViewItemIterator it( listView() );
    int c = 1;
    while ( it.current() ) {
	if ( it.current() == item ) {
	    *control = c;
	    return (TQAccessibleInterface*)this;
	}
	++c;
	++it;
    }
    return 0;
}
*/
/*! \reimp */
bool TQAccessibleListView::setFocus( int control )
{
    bool res = TQAccessibleScrollView::setFocus( 0 );
    if ( !control || !res )
	return res;

    TQListViewItem *item = findLVItem( listView(), control );
    if ( !item )
	return false;
    listView()->setCurrentItem( item );
    return true;
}

/*! \reimp */
bool TQAccessibleListView::setSelected( int control, bool on, bool extend )
{
    if ( !control || ( extend &&
	listView()->selectionMode() != TQListView::Extended &&
	listView()->selectionMode() != TQListView::Multi ) )
	return false;

    TQListViewItem *item = findLVItem( listView(), control );
    if ( !item )
	return false;
    if ( !extend ) {
	listView()->setSelected( item, on );
    } else {
	TQListViewItem *current = listView()->currentItem();
	if ( !current )
	    return false;
	bool down = item->itemPos() > current->itemPos();
	TQListViewItemIterator it( current );
	while ( it.current() ) {
	    listView()->setSelected( it.current(), on );
	    if ( it.current() == item )
		break;
	    if ( down )
		++it;
	    else
		--it;
	}
    }
    return true;
}

/*! \reimp */
void TQAccessibleListView::clearSelection()
{
    listView()->clearSelection();
}

/*! \reimp */
TQMemArray<int> TQAccessibleListView::selection() const
{
    TQMemArray<int> array;
    uint size = 0;
    int id = 1;
    array.resize( size );
    TQListViewItemIterator it( listView() );
    while ( it.current() ) {
	if ( it.current()->isSelected() ) {
	    ++size;
	    array.resize( size );
	    array[ (int)size-1 ] = id;
	}
	++it;
	++id;
    }
    return array;
}

#ifndef TQT_NO_ICONVIEW
/*!
  \class TQAccessibleIconView qaccessiblewidget.h
  \brief The TQAccessibleIconView class implements the TQAccessibleInterface for icon views.
*/

static TQIconViewItem *findIVItem( TQIconView *iconView, int control )
{
    int id = 1;
    TQIconViewItem *item = iconView->firstItem();
    while ( item && id < control ) {
	item = item->nextItem();
	++id;
    }

    return item;
}

/*!
  Constructs a TQAccessibleIconView object for \a o.
*/
TQAccessibleIconView::TQAccessibleIconView( TQObject *o )
    : TQAccessibleScrollView( o, Outline )
{
    Q_ASSERT(widget()->inherits("TQIconView"));
}

/*! Returns the icon view. */
TQIconView *TQAccessibleIconView::iconView() const
{
    return (TQIconView*)widget();
}

/*! \reimp */
int TQAccessibleIconView::itemAt( int x, int y ) const
{
    TQIconViewItem *item = iconView()->findItem( TQPoint( x, y ) );
    return iconView()->index( item ) + 1;
}

/*! \reimp */
TQRect TQAccessibleIconView::itemRect( int control ) const
{
    TQIconViewItem *item = findIVItem( iconView(), control );

    if ( !item )
	return TQRect();
    return item->rect();
}

/*! \reimp */
int TQAccessibleIconView::itemCount() const
{
    return iconView()->count();
}

/*! \reimp */
TQString TQAccessibleIconView::text( Text t, int control ) const
{
    if ( !control || t != Name )
	return TQAccessibleScrollView::text( t, control );

    TQIconViewItem *item = findIVItem( iconView(), control );
    if ( !item )
	return TQString::null;
    return item->text();
}

/*! \reimp */
TQAccessible::Role TQAccessibleIconView::role( int control ) const
{
    if ( !control )
	return TQAccessibleScrollView::role( control );
    return OutlineItem;
}

/*! \reimp */
TQAccessible::State TQAccessibleIconView::state( int control ) const
{
    int state = TQAccessibleScrollView::state( control );
    TQIconViewItem *item;
    if ( !control || !( item = findIVItem( iconView(), control ) ) )
	return (State)state;

    if ( item->isSelectable() ) {
	if ( iconView()->selectionMode() == TQIconView::Multi )
	    state |= MultiSelectable;
	else if ( iconView()->selectionMode() == TQIconView::Extended )
	    state |= ExtSelectable;
	else if ( iconView()->selectionMode() == TQIconView::Single )
	    state |= Selectable;
	if ( item->isSelected() )
	    state |= Selected;
    }
    if ( iconView()->itemsMovable() )
	state |= Moveable;
    if ( iconView()->focusPolicy() != TQWidget::NoFocus ) {
	state |= Focusable;
	if ( item == iconView()->currentItem() )
	    state |= Focused;
    }

    return (State)state;
}

/*! \reimp
TQAccessibleInterface *TQAccessibleIconView::focusChild( int *control ) const
{
    TQIconViewItem *item = iconView()->currentItem();
    if ( !item )
	return 0;

    *control = iconView()->index( item );
    return (TQAccessibleInterface*)this;
}
*/
/*! \reimp */
bool TQAccessibleIconView::setFocus( int control )
{
    bool res = TQAccessibleScrollView::setFocus( 0 );
    if ( !control || !res )
	return res;

    TQIconViewItem *item = findIVItem( iconView(), control );
    if ( !item )
	return false;
    iconView()->setCurrentItem( item );
    return true;
}

/*! \reimp */
bool TQAccessibleIconView::setSelected( int control, bool on, bool extend  )
{
    if ( !control || ( extend &&
	iconView()->selectionMode() != TQIconView::Extended &&
	iconView()->selectionMode() != TQIconView::Multi ) )
	return false;

    TQIconViewItem *item = findIVItem( iconView(), control );
    if ( !item )
	return false;
    if ( !extend ) {
	iconView()->setSelected( item, on, true );
    } else {
	TQIconViewItem *current = iconView()->currentItem();
	if ( !current )
	    return false;
	bool down = false;
	TQIconViewItem *temp = current;
	while ( ( temp = temp->nextItem() ) ) {
	    if ( temp == item ) {
		down = true;
		break;
	    }
	}
	temp = current;
	if ( down ) {
	    while ( ( temp = temp->nextItem() ) ) {
		iconView()->setSelected( temp, on, true );
		if ( temp == item )
		    break;
	    }
	} else {
	    while ( ( temp = temp->prevItem() ) ) {
		iconView()->setSelected( temp, on, true );
		if ( temp == item )
		    break;
	    }
	}
    }
    return true;
}

/*! \reimp */
void TQAccessibleIconView::clearSelection()
{
    iconView()->clearSelection();
}

/*! \reimp */
TQMemArray<int> TQAccessibleIconView::selection() const
{
    TQMemArray<int> array;
    uint size = 0;
    int id = 1;
    array.resize( iconView()->count() );
    TQIconViewItem *item = iconView()->firstItem();
    while ( item ) {
	if ( item->isSelected() ) {
	    ++size;
	    array[ (int)size-1 ] = id;
	}
	item = item->nextItem();
	++id;
    }
    array.resize( size );
    return array;
}
#endif


/*!
  \class TQAccessibleTextEdit qaccessiblewidget.h
  \brief The TQAccessibleTextEdit class implements the TQAccessibleInterface for richtext editors.
*/

/*!
  Constructs a TQAccessibleTextEdit object for \a o.
*/
TQAccessibleTextEdit::TQAccessibleTextEdit( TQObject *o )
: TQAccessibleScrollView( o, Pane )
{
    Q_ASSERT(widget()->inherits("TQTextEdit"));
}

/*! Returns the text edit. */
TQTextEdit *TQAccessibleTextEdit::textEdit() const
{

    return (TQTextEdit*)widget();
}

/*! \reimp */
int TQAccessibleTextEdit::itemAt( int x, int y ) const
{
    int p;
    TQPoint cp = textEdit()->viewportToContents( TQPoint( x,y ) );
    textEdit()->charAt( cp , &p );
    return p + 1;
}

/*! \reimp */
TQRect TQAccessibleTextEdit::itemRect( int item ) const
{
    TQRect rect = textEdit()->paragraphRect( item - 1 );
    if ( !rect.isValid() )
	return TQRect();
    TQPoint ntl = textEdit()->contentsToViewport( TQPoint( rect.x(), rect.y() ) );
    return TQRect( ntl.x(), ntl.y(), rect.width(), rect.height() );
}

/*! \reimp */
int TQAccessibleTextEdit::itemCount() const
{
    return textEdit()->paragraphs();
}

/*! \reimp */
TQString TQAccessibleTextEdit::text( Text t, int control ) const
{
    if (t == Name) {
        if (control)
            return textEdit()->text(control - 1);
        else
            return stripAmp(buddyString(widget()));
    } else if (t == Value) {
        if (control)
            return textEdit()->text(control - 1);
        else
            return textEdit()->text();
    }

    return TQAccessibleScrollView::text( t, control );
}

/*! \reimp */
void TQAccessibleTextEdit::setText(Text t, int control, const TQString &text)
{
    if (control || t != Value) {
        TQAccessibleScrollView::setText(t, control, text);
        return;
    }
    textEdit()->setText(text);
}

/*! \reimp */
TQAccessible::Role TQAccessibleTextEdit::role( int control ) const
{
    if ( control )
	return EditableText;
    return TQAccessibleScrollView::role( control );
}
