/****************************************************************************
**
** Implementation of TQTab and TQTabBar classes
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

#include "ntqtabbar.h"
#ifndef TQT_NO_TABBAR
#include "ntqaccel.h"
#include "ntqbitmap.h"
#include "ntqtoolbutton.h"
#include "ntqtooltip.h"
#include "ntqapplication.h"
#include "ntqstyle.h"
#include "ntqpainter.h"
#include "ntqiconset.h"
#include "ntqcursor.h"
#include "../kernel/qinternal_p.h"
#if defined(QT_ACCESSIBILITY_SUPPORT)
#include "ntqaccessible.h"
#endif

#ifdef TQ_WS_MACX
#include <qmacstyle_mac.h>
#endif


/*!
    \class TQTab ntqtabbar.h
    \brief The TQTab class provides the structures in a TQTabBar.

    \ingroup advanced

    This class is used for custom TQTabBar tab headings.

    \sa TQTabBar
*/


/*!
    Constructs an empty tab. All fields are set to empty.
*/

TQTab::TQTab()
    : enabled( true ),
      id ( 0 ),
      iconset( 0 ),
      tb( 0 )
{
}

/*!
    Constructs a tab with the text \a text.
*/

TQTab::TQTab( const TQString &text )
    : label( text ),
      enabled( true ),
      id( 0 ),
      iconset( 0 ),
      tb( 0 )
{
}

/*!
    Constructs a tab with an \a icon and the text, \a text.
*/

TQTab::TQTab( const TQIconSet& icon, const TQString& text )
    : label( text ),
      enabled( true ),
      id( 0 ),
      iconset( new TQIconSet(icon) ),
      tb( 0 )
{
}

/*!
    \fn TQString TQTab::text() const

    Returns the text of the TQTab label.
*/

/*!
    \fn TQIconSet TQTab::iconSet() const

    Return the TQIconSet of the TQTab.
*/

/*!
    \fn void TQTab::setRect( const TQRect &rect )

    Set the TQTab TQRect to \a rect.
*/

/*!
    \fn TQRect TQTab::rect() const

    Return the TQRect for the TQTab.
*/

/*!
    \fn void TQTab::setEnabled( bool enable )

    If \a enable is true enable the TQTab, otherwise disable it.
*/

/*!
    \fn bool TQTab::isEnabled() const

    Returns true if the TQTab is enabled; otherwise returns false.
*/

/*!
    \fn void TQTab::setIdentifier( int i )

    Set the identifier for the TQTab to \a i. Each TQTab's identifier
    within a TQTabBar must be unique.
*/

/*!
    \fn int TQTab::identifier() const

    Return the TQTab's identifier.
*/



/*!
    Destroys the tab and frees up all allocated resources.
*/

TQTab::~TQTab()
{
    delete iconset;
    tb = 0;
}

/*!
    \class TQTabBar ntqtabbar.h
    \brief The TQTabBar class provides a tab bar, e.g. for use in tabbed dialogs.

    \ingroup advanced

    TQTabBar is straightforward to use; it draws the tabs using one of
    the predefined \link TQTabBar::Shape shapes\endlink, and emits a
    signal when a tab is selected. It can be subclassed to tailor the
    look and feel. TQt also provides a ready-made \l{TQTabWidget} and a
    \l{TQTabDialog}.

    The choice of tab shape is a matter of taste, although tab dialogs
    (for preferences and similar) invariably use \c RoundedAbove;
    nobody uses \c TriangularAbove. Tab controls in windows other than
    dialogs almost always use either \c RoundedBelow or \c
    TriangularBelow. Many spreadsheets and other tab controls in which
    all the pages are essentially similar use \c TriangularBelow,
    whereas \c RoundedBelow is used mostly when the pages are
    different (e.g. a multi-page tool palette).

    The most important part of TQTabBar's API is the selected() signal.
    This is emitted whenever the selected page changes (even at
    startup, when the selected page changes from 'none'). There is
    also a slot, setCurrentTab(), which can be used to select a page
    programmatically.

    TQTabBar creates automatic accelerator keys in the manner of
    TQButton; e.g. if a tab's label is "\&Graphics", Alt+G becomes an
    accelerator key for switching to that tab.

    The following virtual functions may need to be reimplemented:
    \list
    \i paint() paints a single tab. paintEvent() calls paint() for
    each tab so that any overlap will look right.
    \i addTab() creates a new tab and adds it to the bar.
    \i selectTab() decides which tab, if any, the user selects with the mouse.
    \endlist

    The index of the current tab is returned by currentTab(). The tab
    with a particular index is returned by tabAt(), the tab with a
    particular id is returned by tab(). The index of a tab is returned
    by indexOf(). The current tab can be set by index or tab pointer
    using one of the setCurrentTab() functions.

    <img src=qtabbar-m.png> <img src=qtabbar-w.png>
*/

/*!
    \enum TQTabBar::Shape

    This enum type lists the built-in shapes supported by TQTabBar:

    \value RoundedAbove  the normal rounded look above the pages

    \value RoundedBelow  the normal rounded look below the pages

    \value TriangularAbove  triangular tabs above the pages (very
    unusual; included for completeness)

    \value TriangularBelow  triangular tabs similar to those used in
    the Excel spreadsheet, for example
*/

class TQTabBarToolTip;

struct TQTabPrivate {
    int id;
    int focus;
#ifndef TQT_NO_ACCEL
    TQAccel * a;
#endif
    TQTab *pressed;
    TQTabBar::Shape s;
    TQToolButton* rightB;
    TQToolButton* leftB;
    int btnWidth;
    bool  scrolls;
    TQTabBarToolTip * toolTips;
};

#ifndef TQT_NO_TOOLTIP
/* \internal
*/
class TQTabBarToolTip : public TQToolTip
{
public:
    TQTabBarToolTip( TQWidget * parent )
	: TQToolTip( parent ) {}
    virtual ~TQTabBarToolTip() {}

    void add( TQTab * tab, const TQString & tip )
    {
	tabTips.replace( tab, tip );
    }

    void remove( TQTab * tab )
    {
	tabTips.erase( tab );
    }

    TQString tipForTab( TQTab * tab ) const
    {
	TQMapConstIterator<TQTab *, TQString> it;
	it = tabTips.find( tab );
	if ( it != tabTips.end() )
	    return it.data();
	else
	    return TQString();
    }

protected:
    void maybeTip( const TQPoint & p )
    {
	TQTabBar * tb = (TQTabBar *) parentWidget();
	if ( !tb )
	    return;

	// check if the scroll buttons in the tab bar are visible -
	// don't display any tips if the pointer is over one of them
	TQRect rectL, rectR;
	rectL.setRect( tb->d->leftB->x(), tb->d->leftB->y(),
		       tb->d->leftB->width(), tb->d->leftB->height() );
	rectR.setRect( tb->d->rightB->x(), tb->d->rightB->y(),
		       tb->d->rightB->width(), tb->d->rightB->height() );
	if ( tb->d->scrolls && (rectL.contains( p ) || rectR.contains( p )) )
	     return;

#ifndef TQT_NO_TOOLTIP
	// find and show the tool tip for the tab under the point p
	TQMapIterator<TQTab *, TQString> it;
	for ( it = tabTips.begin(); it != tabTips.end(); ++it ) {
	    if ( it.key()->rect().contains( p ) )
		tip( it.key()->rect(), it.data() );
	}
#endif
    }

private:
    TQMap<TQTab *, TQString> tabTips;
};
#endif

/*!
    \fn void TQTabBar::selected( int id )

    TQTabBar emits this signal whenever any tab is selected, whether by
    the program or by the user. The argument \a id is the id of the
    tab as returned by addTab().

    show() is guaranteed to emit this signal; you can display your
    page in a slot connected to this signal.
*/

/*!
    \fn void TQTabBar::layoutChanged()

    TQTabBar emits the signal whenever the layout of the tab bar has
    been recalculated, for example when the contents of a tab change.
*/

/*!
    Constructs a new, empty tab bar; the \a parent and \a name
    arguments are passed on to the TQWidget constructor.
*/

TQTabBar::TQTabBar( TQWidget * parent, const char *name )
    : TQWidget( parent, name, WNoAutoErase | WNoMousePropagation  ), l(NULL), hoverTab( 0 )
{
    d = new TQTabPrivate;
    d->pressed = 0;
    d->id = 0;
    d->focus = 0;
    d->toolTips = 0;
#ifndef TQT_NO_ACCEL
    d->a = new TQAccel( this, "tab accelerators" );
    connect( d->a, TQ_SIGNAL(activated(int)), this, TQ_SLOT(setCurrentTab(int)) );
    connect( d->a, TQ_SIGNAL(activatedAmbiguously(int)), this, TQ_SLOT(setCurrentTab(int)) );
#endif
    d->s = RoundedAbove;
    d->scrolls = false;
    d->leftB = new TQToolButton( LeftArrow, this, "qt_left_btn" );
    connect( d->leftB, TQ_SIGNAL( clicked() ), this, TQ_SLOT( scrollTabs() ) );
    d->leftB->hide();
    d->rightB = new TQToolButton( RightArrow, this, "qt_right_btn" );
    connect( d->rightB, TQ_SIGNAL( clicked() ), this, TQ_SLOT( scrollTabs() ) );
    d->rightB->hide();
    d->btnWidth = style().pixelMetric(TQStyle::PM_TabBarScrollButtonWidth, this);
    l = new TQPtrList<TQTab>;
    lstatic = new TQPtrList<TQTab>;
    lstatic->setAutoDelete( true );
    setFocusPolicy( TabFocus );
    setSizePolicy( TQSizePolicy( TQSizePolicy::Preferred, TQSizePolicy::Fixed ) );
}


/*!
    Destroys the tab control, freeing memory used.
*/

TQTabBar::~TQTabBar()
{
#ifndef TQT_NO_TOOLTIP
    if ( d->toolTips )
	delete d->toolTips;
#endif
    delete d;
    d = 0;
    delete l;
    l = 0;
    delete lstatic;
    lstatic = 0;
}


/*!
    Adds the tab, \a newTab, to the tab control.

    Sets \a newTab's id to a new id and places the tab just to the
    right of the existing tabs. If the tab's label contains an
    ampersand, the letter following the ampersand is used as an
    accelerator for the tab, e.g. if the label is "Bro\&wse" then
    Alt+W becomes an accelerator which will move the focus to this
    tab. Returns the id.

    \sa insertTab()
*/

int TQTabBar::addTab( TQTab * newTab )
{
    return insertTab( newTab );
}


/*!
    Inserts the tab, \a newTab, into the tab control.

    If \a index is not specified, the tab is simply appended.
    Otherwise it's inserted at the specified position.

    Sets \a newTab's id to a new id. If the tab's label contains an
    ampersand, the letter following the ampersand is used as an
    accelerator for the tab, e.g. if the label is "Bro\&wse" then
    Alt+W becomes an accelerator which will move the focus to this
    tab. Returns the id.

    \sa addTab()
*/

int TQTabBar::insertTab( TQTab * newTab, int index )
{
    newTab->id = d->id++;
    if ( !tab( d->focus ) )
	d->focus = newTab->id;

    newTab->setTabBar( this );
    l->insert( 0, newTab );
    if ( index < 0 || index > int(lstatic->count()) )
	lstatic->append( newTab );
    else
	lstatic->insert( index, newTab );

    layoutTabs();
    updateArrowButtons();
    makeVisible( tab( currentTab() ) );

#ifndef TQT_NO_ACCEL
    int p = TQAccel::shortcutKey( newTab->label );
    if ( p )
	d->a->insertItem( p, newTab->id );
#endif

    return newTab->id;
}


/*!
    Removes tab \a t from the tab control, and deletes the tab.
*/
void TQTabBar::removeTab( TQTab * t )
{
    //#### accelerator labels??
#ifndef TQT_NO_TOOLTIP
    if ( d->toolTips )
	d->toolTips->remove( t );
#endif
#ifndef TQT_NO_ACCEL
    if ( d->a )
	d->a->removeItem( t->id );
#endif
    bool updateFocus = t->id == d->focus;
    // remove the TabBar Reference
    if(d->pressed == t)
	d->pressed = 0;
    t->setTabBar( 0 );
    l->remove( t );
    lstatic->remove( t );
    layoutTabs();
    updateArrowButtons();
    makeVisible( tab( currentTab() ) );
    if ( updateFocus )
	d->focus = currentTab();
    update();
}


/*!
    Enables tab \a id if \a enabled is true or disables it if \a
    enabled is false. If \a id is currently selected,
    setTabEnabled(false) makes another tab selected.

    setTabEnabled() updates the display if this causes a change in \a
    id's status.

    \sa update(), isTabEnabled()
*/

void TQTabBar::setTabEnabled( int id, bool enabled )
{
    TQTab * t;
    for( t = l->first(); t; t = l->next() ) {
	if ( t && t->id == id ) {
	    if ( t->enabled != enabled ) {
		t->enabled = enabled;
#ifndef TQT_NO_ACCEL
		d->a->setItemEnabled( t->id, enabled );
#endif
		TQRect r( t->r );
		if ( !enabled && id == currentTab() ) {
		    TQPoint p1( t->r.center() ), p2;
		    int m = 2147483647;
		    int distance;
		    // look for the closest enabled tab - measure the
		    // distance between the centers of the two tabs
		    for( TQTab * n = l->first(); n; n = l->next() ) {
			if ( n->enabled ) {
			    p2 = n->r.center();
			    distance = (p2.x() - p1.x())*(p2.x() - p1.x()) +
				       (p2.y() - p1.y())*(p2.y() - p1.y());
			    if ( distance < m ) {
				t = n;
				m = distance;
			    }
			}
		    }
		    if ( t->enabled ) {
			r = r.unite( t->r );
			l->append( l->take( l->findRef( t ) ) );
			emit selected( t->id );
		    }
		}
		repaint( r, false );
	    }
	    return;
	}
    }
}


/*!
    Returns true if the tab with id \a id exists and is enabled;
    otherwise returns false.

    \sa setTabEnabled()
*/

bool TQTabBar::isTabEnabled( int id ) const
{
    TQTab * t = tab( id );
    if ( t )
	return t->enabled;
    return false;
}



/*!
    \reimp
*/
TQSize TQTabBar::sizeHint() const
{
    TQSize sz(0, 0);
    if ( TQTab * t = l->first() ) {
	TQRect r( t->r );
	while ( (t = l->next()) != 0 )
	    r = r.unite( t->r );
	sz = r.size();
    }
    return sz.expandedTo(TQApplication::globalStrut());
}

/*!
    \reimp
*/

TQSize TQTabBar::minimumSizeHint() const
{
    if(style().styleHint( TQStyle::SH_TabBar_PreferNoArrows, this ))
	return sizeHint();
    return TQSize( d->rightB->sizeHint().width() * 2 + 75, sizeHint().height() );
}

/*!
    Paints the tab \a t using painter \a p. If and only if \a selected
    is true, \a t is drawn currently selected.

    This virtual function may be reimplemented to change the look of
    TQTabBar. If you decide to reimplement it, you may also need to
    reimplement sizeHint().
*/

void TQTabBar::paint( TQPainter * p, TQTab * t, bool selected ) const
{
	TQStyle::SFlags flags = TQStyle::Style_Default;
	
	if (isEnabled() && t->isEnabled()) {
		flags |= TQStyle::Style_Enabled;
	}
	if (topLevelWidget() == tqApp->activeWindow()) {
		flags |= TQStyle::Style_Active;
	}
	if ( selected ) {
		flags |= TQStyle::Style_Selected;
	}
	else if (t == d->pressed) {
		flags |= TQStyle::Style_Sunken;
	}

	//selection flags
	if (t->rect().contains(mapFromGlobal(TQCursor::pos()))) {
		flags |= TQStyle::Style_MouseOver;
	}
	style().drawControl( TQStyle::CE_TabBarTab, p, this, t->rect(), colorGroup(), flags, TQStyleOption(t, hoverTab) );

	TQRect r( t->r );
	p->setFont( font() );

	int iw = 0;
	int ih = 0;
	if ( t->iconset != 0 ) {
		iw = t->iconset->pixmap( TQIconSet::Small, TQIconSet::Normal ).width() + 4;
		ih = t->iconset->pixmap( TQIconSet::Small, TQIconSet::Normal ).height();
	}
	TQFontMetrics fm = p->fontMetrics();
	int fw = fm.width( t->label );
	fw -= t->label.contains('&') * fm.width('&');
	fw += t->label.contains("&&") * fm.width('&');
	int w = iw + fw + 4;
	int h = TQMAX(fm.height() + 4, ih );
	int offset = 3;
#ifdef TQ_WS_MAC
	if (::tqt_cast<TQMacStyle *>(&style())) {
		offset = 0;
	}
#endif
	paintLabel( p, TQRect( r.left() + (r.width()-w)/2 - offset, r.top() + (r.height()-h)/2, w, h ), t, t->id == keyboardFocusTab() );
}

/*!
    Paints the label of tab \a t centered in rectangle \a br using
    painter \a p. A focus indication is drawn if \a has_focus is true.
*/

void TQTabBar::paintLabel( TQPainter* p, const TQRect& br,
			  TQTab* t, bool has_focus ) const
{
	TQRect r = br;
	bool selected = currentTab() == t->id;
	if ( t->iconset) {
		// the tab has an iconset, draw it in the right mode
		TQIconSet::Mode mode = (t->enabled && isEnabled())
		? TQIconSet::Normal : TQIconSet::Disabled;
		if ( mode == TQIconSet::Normal && has_focus )
		mode = TQIconSet::Active;
		TQPixmap pixmap = t->iconset->pixmap( TQIconSet::Small, mode );
		int pixw = pixmap.width();
		int pixh = pixmap.height();
		r.setLeft( r.left() + pixw + 4 );
		r.setRight( r.right() + 2 );
	
		int xoff = 0, yoff = 0;
		if(!selected) {
			xoff = style().pixelMetric(TQStyle::PM_TabBarTabShiftHorizontal, this);
			yoff = style().pixelMetric(TQStyle::PM_TabBarTabShiftVertical, this);
		}
		p->drawPixmap( br.left() + 2 + xoff, br.center().y()-pixh/2 + yoff, pixmap );
	}

	TQStyle::SFlags flags = TQStyle::Style_Default;
	
	if (isEnabled() && t->isEnabled()) {
		flags |= TQStyle::Style_Enabled;
	}
	if (has_focus) {
		flags |= TQStyle::Style_HasFocus;
	}
	if ( selected ) {
		flags |= TQStyle::Style_Selected;
	}
	else if(t == d->pressed) {
		flags |= TQStyle::Style_Sunken;
	}
	if(t->rect().contains(mapFromGlobal(TQCursor::pos()))) {
		flags |= TQStyle::Style_MouseOver;
	}
	style().drawControl( TQStyle::CE_TabBarLabel, p, this, r,
				t->isEnabled() ? colorGroup(): palette().disabled(),
				flags, TQStyleOption(t, hoverTab) );
}


/*!
    Repaints the tab row. All the painting is done by paint();
    paintEvent() only decides which tabs need painting and in what
    order. The event is passed in \a e.

    \sa paint()
*/

void TQTabBar::paintEvent( TQPaintEvent * e )
{
    if ( e->rect().isNull() )
	return;

    TQSharedDoubleBuffer buffer( this, e->rect() );

    TQTab * t;
    t = l->first();
    do {
	TQTab * n = l->next();
	if ( t && t->r.intersects( e->rect() ) ) {
	    paint( buffer.painter(), t, n == 0 );
	}
	t = n;
    } while ( t != 0 );

    if ( d->scrolls && lstatic->first()->r.left() < 0 ) {
	TQPointArray a;
	int h = height();
	if ( d->s == RoundedAbove ) {
	    buffer.painter()->fillRect( 0, 3, 4, h-5,
			colorGroup().brush( TQColorGroup::Background ) );
	    a.setPoints( 5,  0,2,  3,h/4, 0,h/2, 3,3*h/4, 0,h );
	} else if ( d->s == RoundedBelow ) {
	    buffer.painter()->fillRect( 0, 2, 4, h-5,
			colorGroup().brush( TQColorGroup::Background ) );
	    a.setPoints( 5,  0,0,  3,h/4, 0,h/2, 3,3*h/4, 0,h-3 );
	}

	if ( !a.isEmpty() ) {
	    buffer.painter()->setPen( colorGroup().light() );
	    buffer.painter()->drawPolyline( a );
	    a.translate( 1, 0 );
	    buffer.painter()->setPen( colorGroup().midlight() );
	    buffer.painter()->drawPolyline( a );
	}
    }
}


/*!
    This virtual function is called by the mouse event handlers to
    determine which tab is pressed. The default implementation returns
    a pointer to the tab whose bounding rectangle contains \a p, if
    exactly one tab's bounding rectangle contains \a p. Otherwise it
    returns 0.

    \sa mousePressEvent() mouseReleaseEvent()
*/

TQTab * TQTabBar::selectTab( const TQPoint & p ) const
{
    TQTab * selected = 0;
    bool moreThanOne = false;

    TQPtrListIterator<TQTab> i( *l );
    while( i.current() ) {
	TQTab * t = i.current();
	++i;

	if ( t && t->r.contains( p ) ) {
	    if ( selected )
		moreThanOne = true;
	    else
		selected = t;
	}
    }

    return moreThanOne ? 0 : selected;
}


/*!
    \reimp
*/
void TQTabBar::mousePressEvent( TQMouseEvent * e )
{
    if ( e->button() != LeftButton ) {
	e->ignore();
	return;
    }
    TQTab *t = selectTab( e->pos() );
    if ( t && t->enabled ) {
	d->pressed = t;
	if(e->type() == style().styleHint( TQStyle::SH_TabBar_SelectMouseType, this ))
	    setCurrentTab( t );
	else
	    repaint(t->rect(), false);
    }
}


/*!
    \reimp
*/

void TQTabBar::mouseMoveEvent ( TQMouseEvent *e )
{
	TQTab *t = selectTab( e->pos() );

	// Repaint hover indicator(s)
	// Also, avoid unnecessary repaints which otherwise would occour on every MouseMove event causing high cpu load
	bool forceRepaint = true;
	if (hoverTab == t) {
		forceRepaint = false;
	}
	hoverTab = t;
	if (forceRepaint) {
		repaint(false);
	}

	if ( e->state() != LeftButton ) {
		e->ignore();
		return;
	}

	if(style().styleHint( TQStyle::SH_TabBar_SelectMouseType, this ) == TQEvent::MouseButtonRelease) {
		if(t != d->pressed) {
			if (d->pressed) {
				if (!forceRepaint) {
					repaint(d->pressed->rect(), false);
				}
			}
			if ((d->pressed = t)) {
				if (!forceRepaint) {
					repaint(t->rect(), false);
				}
			}
		}
	}
}

/*!
    \reimp
*/

void TQTabBar::mouseReleaseEvent( TQMouseEvent *e )
{
	if (e->button() != LeftButton) {
		e->ignore();
	}
	
	if (d->pressed) {
		TQTab *t = selectTab( e->pos() ) == d->pressed ? d->pressed : 0;
		d->pressed = 0;
		if(t && t->enabled && e->type() == style().styleHint( TQStyle::SH_TabBar_SelectMouseType, this )) {
			setCurrentTab( t );
		}
	}
}

void TQTabBar::enterEvent( TQEvent * )
{
	hoverTab = 0;
}

void TQTabBar::leaveEvent( TQEvent * )
{
	hoverTab = 0;
}

/*!
    \reimp
*/
void TQTabBar::show()
{
    //  ensures that one tab is selected.
    TQTab * t = l->last();
    TQWidget::show();

    if ( t )
	emit selected( t->id );
}

/*!
    \property TQTabBar::currentTab
    \brief the id of the tab bar's visible tab

    If no tab page is currently visible, the property's value is -1.
    Even if the property's value is not -1, you cannot assume that the
    user can see the relevant page, or that the tab is enabled. When
    you need to display something the value of this property
    represents the best page to display.

    When this property is set to \e id, it will raise the tab with the
    id \e id and emit the selected() signal.

    \sa selected() isTabEnabled()
*/

int TQTabBar::currentTab() const
{
    const TQTab * t = 0;
    if (l) {
        t = l->getLast();
    }

    return t ? t->id : -1;
}

void TQTabBar::setCurrentTab( int id )
{
    setCurrentTab( tab( id ) );
}


/*!
    \overload

    Raises \a tab and emits the selected() signal unless the tab was
    already current.

    \sa currentTab() selected()
*/

void TQTabBar::setCurrentTab( TQTab * tab )
{
    if ( tab && l ) {
	if ( l->last() == tab )
	    return;

	TQRect r = l->last()->r;
	if ( l->findRef( tab ) >= 0 )
	    l->append( l->take() );

	d->focus = tab->id;

	setMicroFocusHint( tab->rect().x(), tab->rect().y(), tab->rect().width(), tab->rect().height(), false );

	if ( tab->r.intersects( r ) ) {
	    repaint( r.unite( tab->r ), false );
	} else {
#ifdef TQ_WS_MACX
            update();
#else
	    repaint( r, false );
	    repaint( tab->r, false );
#endif
	}
	makeVisible( tab );
	emit selected( tab->id );

#ifdef QT_ACCESSIBILITY_SUPPORT
	TQAccessible::updateAccessibility( this, indexOf(tab->id)+1, TQAccessible::Focus );
#endif
    }
}

/*!
    \property TQTabBar::keyboardFocusTab
    \brief the id of the tab that has the keyboard focus

    This property contains the id of the tab that has the keyboard
    focus or -1 if the tab bar does not have the keyboard focus.
*/

int TQTabBar::keyboardFocusTab() const
{
    return hasFocus() ? d->focus : -1;
}


/*!
    \reimp
*/
void TQTabBar::keyPressEvent( TQKeyEvent * e )
{
    //   The right and left arrow keys move a selector, the spacebar
    //   makes the tab with the selector active.  All other keys are
    //   ignored.

    int old = d->focus;

    bool reverse = TQApplication::reverseLayout();
    if ( ( !reverse && e->key() == Key_Left ) || ( reverse && e->key() == Key_Right ) ) {
	// left - skip past any disabled ones
	if ( d->focus >= 0 ) {
	    TQTab * t = lstatic->last();
	    while ( t && t->id != d->focus )
		t = lstatic->prev();
	    do {
		t = lstatic->prev();
	    } while ( t && !t->enabled);
	    if (t)
		d->focus = t->id;
	}
	if ( d->focus < 0 )
	    d->focus = old;
    } else if ( ( !reverse && e->key() == Key_Right ) || ( reverse && e->key() == Key_Left ) ) {
	TQTab * t = lstatic->first();
	while ( t && t->id != d->focus )
	    t = lstatic->next();
	do {
	    t = lstatic->next();
	} while ( t && !t->enabled);

	if (t)
	    d->focus = t->id;
	if ( d->focus >= d->id )
	    d->focus = old;
    } else {
	// other keys - ignore
	e->ignore();
	return;
    }

    // if the focus moved, repaint and signal
    if ( old != d->focus ) {
	setCurrentTab( d->focus );
    }
}


/*!
    Returns the tab with id \a id or 0 if there is no such tab.

    \sa count()
*/

TQTab * TQTabBar::tab( int id ) const
{
    if (l) {
	TQTab * t;
	for( t = l->first(); t; t = l->next() )
	    if ( t && t->id == id )
		return t;
    }
    return 0;
}


/*!
    Returns the tab at position \a index.

    \sa indexOf()
*/

TQTab * TQTabBar::tabAt( int index ) const
{
    TQTab * t;
    TQPtrList<TQTab> internalList = *lstatic;
    t = internalList.at( index );
    return t;
}


/*!
    Returns the position index of the tab with id \a id or -1 if no
    tab has this \a id.

    \sa tabAt()
*/
int TQTabBar::indexOf( int id ) const
{
    TQTab * t;
    TQPtrList<TQTab> internalList = *lstatic;
    int idx = 0;
    for( t = internalList.first(); t; t = internalList.next() ) {
	if ( t && t->id == id )
	    return idx;
	idx++;
    }
    return -1;
}


/*!
    \property TQTabBar::count
    \brief the number of tabs in the tab bar

    \sa tab()
*/
int TQTabBar::count() const
{
    if (l) {
	return l->count();
    }
    return 0;
}


/*!
    The list of TQTab objects in the tab bar.

    This list is unlikely to be in the order that the TQTab elements
    appear visually. One way of iterating over the tabs is like this:
    \code
    for ( uint i = 0; i < myTabBar->count(); ++i ) {
	nextTab = myTabBar->tabAt( i );
	// do something with nextTab
    }
    \endcode
*/
TQPtrList<TQTab> * TQTabBar::tabList()
{
    return l;
}


/*!
    \property TQTabBar::shape
    \brief the shape of the tabs in the tab bar

    The value of this property is one of the following: \c
    RoundedAbove (default), \c RoundedBelow, \c TriangularAbove or \c
    TriangularBelow.

    \sa Shape
*/
TQTabBar::Shape TQTabBar::shape() const
{
    return d ? d->s : RoundedAbove;
}

void TQTabBar::setShape( Shape s )
{
    if ( !d || d->s == s )
	return;
    //######### must recalculate heights
    d->s = s;
    update();
}

/*!
    Lays out all existing tabs according to their label and their
    iconset.
 */
void TQTabBar::layoutTabs()
{
    if ( lstatic->isEmpty() )
	return;

    TQSize oldSh(0, 0);
    if ( TQTab * t = l->first() ) {
	TQRect r( t->r );
	while ( (t = l->next()) != 0 )
	    r = r.unite( t->r );
	oldSh = r.size();
    }

    d->btnWidth = style().pixelMetric(TQStyle::PM_TabBarScrollButtonWidth, this);
    int hframe, vframe, overlap;
    hframe  = style().pixelMetric( TQStyle::PM_TabBarTabHSpace, this );
    vframe  = style().pixelMetric( TQStyle::PM_TabBarTabVSpace, this );
    overlap = style().pixelMetric( TQStyle::PM_TabBarTabOverlap, this );

    TQFontMetrics fm = fontMetrics();
    TQRect r;
    TQTab *t;
    bool reverse = TQApplication::reverseLayout();
    if ( reverse )
	t = lstatic->last();
    else
	t = lstatic->first();
    int x = 0;
    int offset = (t && d->scrolls) ? t->r.x() : 0;
    while ( t ) {
	int lw = fm.width( t->label );
	lw -= t->label.contains('&') * fm.width('&');
	lw += t->label.contains("&&") * fm.width('&');
	int iw = 0;
	int ih = 0;
	if ( t->iconset != 0 ) {
	    iw = t->iconset->pixmap( TQIconSet::Small, TQIconSet::Normal ).width() + 4;
	    ih = t->iconset->pixmap( TQIconSet::Small, TQIconSet::Normal ).height();
	}
	int h = TQMAX( fm.height(), ih );
	h = TQMAX( h, TQApplication::globalStrut().height() );

	h += vframe;
	t->r = TQRect(TQPoint(x, 0), style().sizeFromContents(TQStyle::CT_TabBarTab, this,
	             TQSize( TQMAX( lw + hframe + iw, TQApplication::globalStrut().width() ), h ),
		     TQStyleOption(t, hoverTab) ));
	x += t->r.width() - overlap;
	r = r.unite( t->r );
	if ( reverse )
	    t = lstatic->prev();
	else
	    t = lstatic->next();
    }
    x += overlap;
    int w = (d->scrolls) ? d->leftB->x() : width();
    if (x + offset < w)
	offset = w - x;
    if (offset > 0)
	offset = 0;

    for ( t = lstatic->first(); t; t = lstatic->next() ) {
	t->r.moveBy( offset, 0 );
	t->r.setHeight( r.height() );
    }

    if ( sizeHint() != oldSh )
	updateGeometry();

    emit layoutChanged();
}

/*!
    \reimp
*/

bool TQTabBar::event( TQEvent *e )
{
    if ( e->type() == TQEvent::LanguageChange ) {
	layoutTabs();
	updateArrowButtons();
	makeVisible( tab( currentTab() ));
    }

    return TQWidget::event( e );
}

/*!
  \reimp
*/

void TQTabBar::styleChange( TQStyle& old )
{
    layoutTabs();
    updateArrowButtons();
    TQWidget::styleChange( old );
}

/*!
    \reimp
*/
void TQTabBar::focusInEvent( TQFocusEvent * )
{
    TQTab *t = tab( d->focus );
    if ( t )
	repaint( t->r, false );
}

/*!
    \reimp
*/
void TQTabBar::focusOutEvent( TQFocusEvent * )
{
    TQTab *t = tab( d->focus );
    if ( t )
	repaint( t->r, false );
}

/*!
    \reimp
*/
void TQTabBar::resizeEvent( TQResizeEvent * )
{
    const int arrowWidth = TQMAX( d->btnWidth, TQApplication::globalStrut().width() );;
    d->rightB->setGeometry( width() - arrowWidth, 0, arrowWidth, height() );
    d->leftB->setGeometry( width() - 2*arrowWidth, 0, arrowWidth, height() );
    layoutTabs();
    updateArrowButtons();
    makeVisible( tab( currentTab() ));
}

void TQTabBar::scrollTabs()
{
    TQTab* left = 0;
    TQTab* right = 0;
    for ( TQTab* t = lstatic->first(); t; t = lstatic->next() ) {
	if ( t->r.left() < 0 && t->r.right() > 0 )
	    left = t;
	if ( t->r.left() < d->leftB->x()+2 )
	    right = t;
    }

    if ( sender() == d->leftB )
	makeVisible( left );
    else  if ( sender() == d->rightB )
	makeVisible( right );
}

void TQTabBar::makeVisible( TQTab* tab  )
{
    bool tooFarLeft = ( tab && tab->r.left() < 0 );
    bool tooFarRight = ( tab && tab->r.right() >= d->leftB->x() );

    if ( !d->scrolls || ( !tooFarLeft && ! tooFarRight ) )
	return;

    bool bs = signalsBlocked();
    blockSignals(true);
    layoutTabs();
    blockSignals(bs);

    int offset = 0;

    if ( tooFarLeft ) {
	offset = tab->r.left();
	if (tab != lstatic->first())
	    offset -= 8;
    } else if ( tooFarRight ) {
	offset = tab->r.right() - d->leftB->x() + 1;
    }

    for ( TQTab* t = lstatic->first(); t; t = lstatic->next() )
	t->r.moveBy( -offset, 0 );

    d->leftB->setEnabled( lstatic->first()->r.left() < 0);
    d->rightB->setEnabled( lstatic->last()->r.right() >= d->leftB->x() );

    // Make sure disabled buttons pop up again
    if ( !d->leftB->isEnabled() && d->leftB->isDown() )
	d->leftB->setDown( false );
    if ( !d->rightB->isEnabled() && d->rightB->isDown() )
	d->rightB->setDown( false );

    update();
    emit layoutChanged();
}

void TQTabBar::updateArrowButtons()
{
    if (lstatic->isEmpty()) {
	d->scrolls = false;
    } else {
	d->scrolls = (lstatic->last()->r.right() - lstatic->first()->r.left() > width());
    }
    if ( d->scrolls ) {
	const int arrowWidth = TQMAX( d->btnWidth, TQApplication::globalStrut().width() );
	if ( TQApplication::reverseLayout() ) {
	    d->rightB->setGeometry( arrowWidth, 0, arrowWidth, height() );
	    d->leftB->setGeometry( 0, 0, arrowWidth, height() );
	} else {
	    d->rightB->setGeometry( width() - arrowWidth, 0, arrowWidth, height() );
	    d->leftB->setGeometry( width() - 2*arrowWidth, 0, arrowWidth, height() );
	}

	d->leftB->setEnabled( lstatic->first()->r.left() < 0);
	d->rightB->setEnabled( lstatic->last()->r.right() >= d->leftB->x() );
	d->leftB->show();
	d->rightB->show();
    } else {
	d->leftB->hide();
	d->rightB->hide();
	layoutTabs();
    }
}

/*!
    Removes the tool tip for the tab at index position \a index.
*/
void TQTabBar::removeToolTip( int index )
{
#ifndef TQT_NO_TOOLTIP
    TQTab * tab = tabAt( index );
    if ( !tab || !d->toolTips )
	return;
    d->toolTips->remove( tab );
#endif
}

/*!
    Sets the tool tip for the tab at index position \a index to \a
    tip.
*/
void TQTabBar::setToolTip( int index, const TQString & tip )
{
#ifndef TQT_NO_TOOLTIP
    TQTab * tab = tabAt( index );
    if ( !tab )
	return;
    if ( d->toolTips == 0 )
	d->toolTips = new TQTabBarToolTip( this );
    d->toolTips->add( tab, tip );
#endif
}

/*!
    Returns the tool tip for the tab at index position \a index.
*/
TQString TQTabBar::toolTip( int index ) const
{
#ifndef TQT_NO_TOOLTIP
    if ( d->toolTips )
	return d->toolTips->tipForTab( tabAt( index ) );
    else
#endif
	return TQString();
}

/*!
    Sets the text of the tab to \a text.
*/
void TQTab::setText( const TQString& text )
{
    label = text;
    if ( tb ) {
#ifndef TQT_NO_ACCEL
	tb->d->a->removeItem( id );
	int p = TQAccel::shortcutKey( text );
	if ( p )
	    tb->d->a->insertItem( p, id );
#endif
	tb->layoutTabs();
	tb->repaint(false);

#if defined(QT_ACCESSIBILITY_SUPPORT)
	TQAccessible::updateAccessibility( tb, tb->indexOf(id)+1, TQAccessible::NameChanged );
#endif
    }
}

/*!
    Sets the tab's iconset to \a icon
*/
void TQTab::setIconSet( const TQIconSet &icon )
{
    iconset = new TQIconSet( icon );
}

// this allows us to handle accelerators that are in a TQTabBar.
void TQTab::setTabBar( TQTabBar *newTb )
{
    tb = newTb;
}

/*!
    \internal
*/
void TQTabBar::fontChange( const TQFont & oldFont )
{
    layoutTabs();
    TQWidget::fontChange( oldFont );
}

/*!
    Returns the tab currently under the mouse pointer, or NULL if no tab is under the cursor
*/
TQTab *TQTabBar::mouseHoverTab() const
{
    return hoverTab;
}

#endif
