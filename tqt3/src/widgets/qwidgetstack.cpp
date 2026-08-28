/****************************************************************************
**
** Implementation of TQWidgetStack class
**
** Created : 980128
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

#include "ntqwidgetstack.h"
#include "ntqlayout.h"
#include "../kernel/qlayoutengine_p.h"
#ifndef TQT_NO_WIDGETSTACK

#include "ntqobjectlist.h"
#include "ntqfocusdata.h"
#include "ntqbutton.h"
#include "ntqbuttongroup.h"

#include "ntqapplication.h"

class TQWidgetStackPrivate {
public:
    class Invisible: public TQWidget
    {
    public:
	Invisible( TQWidgetStack * parent ): TQWidget( parent, "qt_invisible_widgetstack" )
	{
	    setBackgroundMode( NoBackground );
	}
	const char * className() const
	{
	    return "TQWidgetStackPrivate::Invisible";
	}
    };
};


#if (TQT_VERSION-0 >= 0x040000)
#if defined(Q_CC_GNU)
#warning "Remove TQWidgetStackEventFilter"
#endif
#endif
class TQWidgetStackEventFilter : public TQObject
{
    /* For binary compatibility, since we cannot implement virtual
       functions and rely on them being called. This is what we should
       have

	bool TQWidgetStack::event( TQEvent* e )
	{
	    if ( e->type() == TQEvent::LayoutHint )
		updateGeometry(); // propgate layout hints to parent
	    return TQFrame::event( e );
	}
    */
public:

    TQWidgetStackEventFilter( TQObject *parent = 0, const char * name = 0 )
	: TQObject( parent, name ) {}
    bool eventFilter( TQObject *o, TQEvent * e ) {
	if ( e->type() == TQEvent::LayoutHint && o->isWidgetType() )
	    ((TQWidget*)o)->updateGeometry();
	return false;
    }
};


/*!
    \class TQWidgetStack
    \brief The TQWidgetStack class provides a stack of widgets of which
    only the top widget is user-visible.

    \ingroup organizers
    \mainclass

    The application programmer can move any widget to the top of the
    stack at any time using raiseWidget(), and add or remove widgets
    using addWidget() and removeWidget(). It is not sufficient to pass
    the widget stack as parent to a widget which should be inserted into
    the widgetstack.

    visibleWidget() is the \e get equivalent of raiseWidget(); it
    returns a pointer to the widget that is currently at the top of
    the stack.

    TQWidgetStack also provides the ability to manipulate widgets
    through application-specified integer IDs. You can also translate
    from widget pointers to IDs using id() and from IDs to widget
    pointers using widget(). These numeric IDs are unique (per
    TQWidgetStack, not globally), but TQWidgetStack does not attach any
    additional meaning to them.

    The default widget stack is frameless, but you can use the usual
    TQFrame functions (such as setFrameStyle()) to add a frame.

    TQWidgetStack provides a signal, aboutToShow(), which is emitted
    just before a managed widget is shown.

    \sa TQTabDialog TQTabBar TQFrame
*/


/*!
    Constructs an empty widget stack.

    The \a parent and \a name arguments are passed to the TQFrame
    constructor.
*/

TQWidgetStack::TQWidgetStack( TQWidget * parent, const char *name )
    : TQFrame( parent, name )
{
    init();
}

/*!
  Constructs an empty widget stack.

  The \a parent, \a name and \a f arguments are passed to the TQFrame
  constructor.
*/
TQWidgetStack::TQWidgetStack( TQWidget * parent, const char *name, WFlags f )
    : TQFrame( parent, name, f ) //## merge constructors in 4.0
{
    init();
}

void TQWidgetStack::init()
{
   d = 0;
   TQWidgetStackEventFilter *ef = new TQWidgetStackEventFilter( this );
   installEventFilter( ef );
   dict = new TQIntDict<TQWidget>;
   focusWidgets = 0;
   topWidget = 0;
   invisible = new TQWidgetStackPrivate::Invisible( this );
   invisible->hide();
}


/*!
    Destroys the object and frees any allocated resources.
*/

TQWidgetStack::~TQWidgetStack()
{
    delete focusWidgets;
    delete d;
    delete dict;
}


/*!
    Adds widget \a w to this stack of widgets, with ID \a id.

    If you pass an id \>= 0 this ID is used. If you pass an \a id of
    -1 (the default), the widgets will be numbered automatically. If
    you pass -2 a unique negative integer will be generated. No widget
    has an ID of -1. Returns the ID or -1 on failure (e.g. \a w is 0).

    If you pass an id that is already used, then a unique negative
    integer will be generated to prevent two widgets having the same
    id.

    If \a w already exists in the stack the widget will be removed first.

    If \a w is not a child of this TQWidgetStack moves it using
    reparent().
*/

int TQWidgetStack::addWidget( TQWidget * w, int id )
{
    static int nseq_no = -2;
    static int pseq_no = 0;

    if ( !w || w == invisible )
	return -1;

    // prevent duplicates
    removeWidget( w );

    if ( id >= 0 && dict->find( id ) )
	id = -2;
    if ( id < -1 )
	id = nseq_no--;
    else if ( id == -1 )
	id = pseq_no++;
    else
	pseq_no = TQMAX(pseq_no, id + 1);
	// use id >= 0 as-is

    dict->insert( id, w );

    // preserve existing focus
    TQWidget * f = w->focusWidget();
    while( f && f != w )
	f = f->parentWidget();
    if ( f ) {
	if ( !focusWidgets )
	    focusWidgets = new TQPtrDict<TQWidget>( 17 );
	focusWidgets->replace( w, w->focusWidget() );
    }

    w->hide();
    if ( w->parent() != this )
	w->reparent( this, contentsRect().topLeft(), false );
    w->setGeometry( contentsRect() );
    updateGeometry();
    return id;
}


/*!
    Removes widget \a w from this stack of widgets. Does not delete \a
    w. If \a w is the currently visible widget, no other widget is
    substituted.

    \sa visibleWidget() raiseWidget()
*/

void TQWidgetStack::removeWidget( TQWidget * w )
{
    if ( !w )
	return;
    int i = id( w );
    if ( i != -1 )
	dict->take( i );

    if ( w == topWidget )
	topWidget = 0;
    if ( dict->isEmpty() )
	invisible->hide(); // let background shine through again
    updateGeometry();
}


/*!
    Raises the widget with ID \a id to the top of the widget stack.

    \sa visibleWidget()
*/

void TQWidgetStack::raiseWidget( int id )
{
    if ( id == -1 )
	return;
    TQWidget * w = dict->find( id );
    if ( w )
	raiseWidget( w );
}

static bool isChildOf( TQWidget* child, TQWidget *parent )
{
    const TQObjectList *list = parent->children();
    if ( !child || !list )
	return false;
    TQObjectListIt it(*list);
    TQObject *obj;
    while ( (obj = it.current()) ) {
	++it;
	if ( !obj->isWidgetType() || ((TQWidget *)obj)->isTopLevel() )
	    continue;
	TQWidget *widget = (TQWidget *)obj;
	if ( widget == child || isChildOf( child, widget ) )
	    return true;
    }
    return false;
}

/*!
    \overload

    Raises widget \a w to the top of the widget stack.
*/

void TQWidgetStack::raiseWidget( TQWidget *w )
{
    if ( !w || w == invisible || w->parent() != this || w == topWidget )
	return;

    if ( id(w) == -1 )
	addWidget( w );
    if ( !isVisible() ) {
	topWidget = w;
	return;
    }

    if (w->maximumSize().width() < invisible->width()
        || w->maximumSize().height() < invisible->height())
        invisible->setBackgroundMode(backgroundMode());
    else if (invisible->backgroundMode() != NoBackground)
        invisible->setBackgroundMode(NoBackground);

    if ( invisible->isHidden() ) {
	invisible->setGeometry( contentsRect() );
	invisible->lower();
	invisible->show();
	TQApplication::sendPostedEvents( invisible, TQEvent::ShowWindowRequest );
    }

    // try to move focus onto the incoming widget if focus
    // was somewhere on the outgoing widget.
    if ( topWidget ) {
	TQWidget * fw = focusWidget();
	TQWidget* p = fw;
	while ( p && p != topWidget )
	    p = p->parentWidget();
	if ( p == topWidget ) { // focus was on old page
	    if ( !focusWidgets )
		focusWidgets = new TQPtrDict<TQWidget>( 17 );
	    focusWidgets->replace( topWidget, fw );
	    fw->clearFocus();
	    // look for the best focus widget we can find
	    // best == what we had (which may be deleted)
	    fw = focusWidgets->take( w );
	    if ( isChildOf( fw, w ) ) {
		fw->setFocus();
	    } else {
		// second best == first child widget in the focus chain
		TQFocusData *f = focusData();
		TQWidget* home = f->home();
		TQWidget *i = home;
		do {
		    if ( ( ( i->focusPolicy() & TabFocus ) == TabFocus )
			 && !i->focusProxy() && i->isVisibleTo(w) && i->isEnabled() ) {
			p = i;
			while ( p && p != w )
			    p = p->parentWidget();
			if ( p == w ) {
			    i->setFocus();
			    break;
			}
		    }
		    i = f->next();
		} while( i != home );
	    }
	}
    }

    if ( isVisible() ) {
	emit aboutToShow( w );
	int i = id( w );
	if ( i != -1 )
	    emit aboutToShow( i );
    }

    topWidget = w;

    const TQObjectList * c = children();
    TQObjectListIt it( *c );
    TQObject * o;

    while( (o=it.current()) != 0 ) {
	++it;
	if ( o->isWidgetType() && o != w && o != invisible )
	    ((TQWidget *)o)->hide();
    }

    w->setGeometry( invisible->geometry() );
    w->show();
}

/*!
    \reimp
*/

void TQWidgetStack::frameChanged()
{
    TQFrame::frameChanged();
    setChildGeometries();
}


/*!
    \reimp
*/

void TQWidgetStack::setFrameRect( const TQRect & r )
{
    TQFrame::setFrameRect( r );
    setChildGeometries();
}


/*!
    Fixes up the children's geometries.
*/

void TQWidgetStack::setChildGeometries()
{
    invisible->setGeometry( contentsRect() );
    if ( topWidget )
	topWidget->setGeometry( invisible->geometry() );
}


/*!
    \reimp
*/
void TQWidgetStack::show()
{
    //  Reimplemented in order to set the children's geometries
    //  appropriately and to pick the first widget as topWidget if no
    //  topwidget was defined
    if ( !isVisible() && children() ) {
	const TQObjectList * c = children();
	TQObjectListIt it( *c );
	TQObject * o;

	while( (o=it.current()) != 0 ) {
	    ++it;
	    if ( o->isWidgetType() ) {
		if ( !topWidget && o != invisible )
		    topWidget = (TQWidget*)o;
		if ( o == topWidget )
		    ((TQWidget *)o)->show();
		else
		    ((TQWidget *)o)->hide();
	    }
	}
	setChildGeometries();
    }
    TQFrame::show();
}


/*!
    Returns the widget with ID \a id. Returns 0 if this widget stack
    does not manage a widget with ID \a id.

    \sa id() addWidget()
*/

TQWidget * TQWidgetStack::widget( int id ) const
{
    return id != -1 ? dict->find( id ) : 0;
}


/*!
    Returns the ID of the \a widget. Returns -1 if \a widget is 0 or
    is not being managed by this widget stack.

    \sa widget() addWidget()
*/

int TQWidgetStack::id( TQWidget * widget ) const
{
    if ( !widget )
	return -1;

    TQIntDictIterator<TQWidget> it( *dict );
    while ( it.current() && it.current() != widget )
	++it;
    return it.current() == widget ? it.currentKey() : -1;
}


/*!
    Returns the currently visible widget (the one at the top of the
    stack), or 0 if nothing is currently being shown.

    \sa aboutToShow() id() raiseWidget()
*/

TQWidget * TQWidgetStack::visibleWidget() const
{
    return topWidget;
}


/*!
    \fn void TQWidgetStack::aboutToShow( int )

    This signal is emitted just before a managed widget is shown if
    that managed widget has an ID != -1. The argument is the numeric
    ID of the widget.

    If you call visibleWidget() in a slot connected to aboutToShow(),
    the widget it returns is the one that is currently visible, not
    the one that is about to be shown.
*/


/*!
    \fn void TQWidgetStack::aboutToShow( TQWidget * )

    \overload

    This signal is emitted just before a managed widget is shown. The
    argument is a pointer to the widget.

    If you call visibleWidget() in a slot connected to aboutToShow(),
    the widget returned is the one that is currently visible, not the
    one that is about to be shown.
*/


/*!
    \reimp
*/

void TQWidgetStack::resizeEvent( TQResizeEvent * e )
{
    TQFrame::resizeEvent( e );
    setChildGeometries();
}


/*!
    \reimp
*/

TQSize TQWidgetStack::sizeHint() const
{
    constPolish();

    TQSize size( 0, 0 );

    TQIntDictIterator<TQWidget> it( *dict );
    TQWidget *w;

    while ( (w = it.current()) != 0 ) {
	++it;
	TQSize sh = w->sizeHint();
	if ( w->sizePolicy().horData() == TQSizePolicy::Ignored )
	    sh.rwidth() = 0;
	if ( w->sizePolicy().verData() == TQSizePolicy::Ignored )
	    sh.rheight() = 0;
#ifndef TQT_NO_LAYOUT
	size = size.expandedTo( sh ).expandedTo( qSmartMinSize(w) );
#endif
    }
    if ( size.isNull() )
	size = TQSize( 128, 64 );
    size += TQSize( 2*frameWidth(), 2*frameWidth() );
    return size;
}


/*!
    \reimp
*/
TQSize TQWidgetStack::minimumSizeHint() const
{
    constPolish();

    TQSize size( 0, 0 );

    TQIntDictIterator<TQWidget> it( *dict );
    TQWidget *w;

    while ( (w = it.current()) != 0 ) {
	++it;
	TQSize sh = w->minimumSizeHint();
	if ( w->sizePolicy().horData() == TQSizePolicy::Ignored )
	    sh.rwidth() = 0;
	if ( w->sizePolicy().verData() == TQSizePolicy::Ignored )
	    sh.rheight() = 0;
#ifndef TQT_NO_LAYOUT
	size = size.expandedTo( sh ).expandedTo( w->minimumSize() );
#endif
    }
    if ( size.isNull() )
	size = TQSize( 64, 32 );
    size += TQSize( 2*frameWidth(), 2*frameWidth() );
    return size;
}

/*!
    \reimp
*/
void TQWidgetStack::childEvent( TQChildEvent * e)
{
    if ( e->child()->isWidgetType() && e->removed() )
	removeWidget( (TQWidget*) e->child() );
}
#endif
