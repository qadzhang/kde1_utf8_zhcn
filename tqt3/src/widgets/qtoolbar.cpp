/****************************************************************************
**
** Implementation of TQToolBar class
**
** Created : 980315
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

#include "ntqtoolbar.h"
#ifndef TQT_NO_TOOLBAR

#include "ntqmainwindow.h"
#include "ntqtooltip.h"
#include "ntqcursor.h"
#include "ntqlayout.h"
#include "ntqframe.h"
#include "ntqobjectlist.h"
#include "ntqpainter.h"
#include "ntqdrawutil.h"
#include "ntqtoolbutton.h"
#include "ntqpopupmenu.h"
#include "ntqcombobox.h"
#include "ntqtimer.h"
#include "ntqwidgetlist.h"
#include "ntqstyle.h"

static const char * const arrow_v_xpm[] = {
    "7 9 3 1",
    "	    c None",
    ".	    c #000000",
    "+	    c none",
    ".+++++.",
    "..+++..",
    "+..+..+",
    "++...++",
    ".++.++.",
    "..+++..",
    "+..+..+",
    "++...++",
    "+++.+++"};

static const char * const arrow_h_xpm[] = {
    "9 7 3 1",
    "	    c None",
    ".	    c #000000",
    "+	    c none",
    "..++..+++",
    "+..++..++",
    "++..++..+",
    "+++..++..",
    "++..++..+",
    "+..++..++",
    "..++..+++"};

class TQToolBarExtensionWidget;

class TQToolBarPrivate
{
public:
    TQToolBarPrivate() : moving( false ) {
    }

    bool moving;
    TQToolBarExtensionWidget *extension;
    TQPopupMenu *extensionPopup;
};


class TQToolBarSeparator : public TQWidget
{
    TQ_OBJECT
public:
    TQToolBarSeparator( Orientation, TQToolBar *parent, const char* name=0 );

    TQSize sizeHint() const;
    Orientation orientation() const { return orient; }
public slots:
    void setOrientation( Orientation );
protected:
    void styleChange( TQStyle& );
    void paintEvent( TQPaintEvent * );

private:
    Orientation orient;
};

class TQToolBarExtensionWidget : public TQWidget
{
    TQ_OBJECT

public:
    TQToolBarExtensionWidget( TQWidget *w );
    void setOrientation( Orientation o );
    TQToolButton *button() const { return tb; }

protected:
    void resizeEvent( TQResizeEvent *e ) {
	TQWidget::resizeEvent( e );
	layOut();
    }

private:
    void layOut();
    TQToolButton *tb;
    Orientation orient;

};

TQToolBarExtensionWidget::TQToolBarExtensionWidget( TQWidget *w )
    : TQWidget( w, "qt_dockwidget_internal" )
{
    tb = new TQToolButton( this, "qt_toolbar_ext_button" );
    tb->setAutoRaise( true );
    setOrientation( Horizontal );
}

void TQToolBarExtensionWidget::setOrientation( Orientation o )
{
    orient = o;
    if ( orient == Horizontal )
	tb->setPixmap( TQPixmap( (const char **)arrow_h_xpm ) );
    else
	tb->setPixmap( TQPixmap( (const char **)arrow_v_xpm ) );
    layOut();
}

void TQToolBarExtensionWidget::layOut()
{
    tb->setGeometry( 2, 2, width() - 4, height() - 4 );
}

TQToolBarSeparator::TQToolBarSeparator(Orientation o , TQToolBar *parent,
				     const char* name )
    : TQWidget( parent, name )
{
    connect( parent, TQ_SIGNAL(orientationChanged(Orientation)),
	     this, TQ_SLOT(setOrientation(Orientation)) );
    setOrientation( o );
    setBackgroundMode( parent->backgroundMode() );
    setBackgroundOrigin( ParentOrigin );
    setSizePolicy( TQSizePolicy( TQSizePolicy::Minimum, TQSizePolicy::Minimum ) );
}



void TQToolBarSeparator::setOrientation( Orientation o )
{
    orient = o;
}

void TQToolBarSeparator::styleChange( TQStyle& )
{
    setOrientation( orient );
}

TQSize TQToolBarSeparator::sizeHint() const
{
    int extent = style().pixelMetric( TQStyle::PM_DockWindowSeparatorExtent,
				      this );
    if ( orient == Horizontal )
	return TQSize( extent, 0 );
    else
	return TQSize( 0, extent );
}

void TQToolBarSeparator::paintEvent( TQPaintEvent * )
{
    TQPainter p( this );
    TQStyle::SFlags flags = TQStyle::Style_Default;

    if ( orientation() == Horizontal )
	flags |= TQStyle::Style_Horizontal;

    style().drawPrimitive( TQStyle::PE_DockWindowSeparator, &p, rect(),
			   colorGroup(), flags );
}

#include "qtoolbar.moc"


/*!
    \class TQToolBar ntqtoolbar.h
    \brief The TQToolBar class provides a movable panel containing
    widgets such as tool buttons.

    \ingroup application
    \mainclass

    A toolbar is a panel that contains a set of controls, usually
    represented by small icons. It's purpose is to provide quick
    access to frequently used commands or options. Within a
    TQMainWindow the user can drag toolbars within and between the
    \link TQDockArea dock areas\endlink. Toolbars can also be dragged
    out of any dock area to float freely as top-level windows.

    TQToolBar is a specialization of TQDockWindow, and so provides all
    the functionality of a TQDockWindow.

    To use TQToolBar you simply create a TQToolBar as a child of a
    TQMainWindow, create a number of TQToolButton widgets (or other
    widgets) in left to right (or top to bottom) order and call
    addSeparator() when you want a separator. When a toolbar is
    floated the caption used is the label given in the constructor
    call. This can be changed with setLabel().

    \quotefile action/application.cpp
    \skipto new TQToolBar
    \printuntil fileSaveAction

    This extract from the \l application/application.cpp example shows
    the creation of a new toolbar as a child of a TQMainWindow and
    adding two TQActions.

    You may use most widgets within a toolbar, with TQToolButton and
    TQComboBox being the most common.

    If you create a new widget on an already visible TQToolBar, this
    widget will automatically become visible without needing a show()
    call. (This differs from every other TQt widget container. We
    recommend calling show() anyway since we hope to fix this anomaly
    in a future release.)

    TQToolBars, like TQDockWindows, are located in \l{TQDockArea}s or
    float as top-level windows. TQMainWindow provides four TQDockAreas
    (top, left, right and bottom). When you create a new toolbar (as
    in the example above) as a child of a TQMainWindow the toolbar will
    be added to the top dock area. You can move it to another dock
    area (or float it) by calling TQMainWindow::moveDockWindow(). TQDock
    areas lay out their windows in \link ntqdockarea.html#lines
    Lines\endlink.

    If the main window is resized so that the area occupied by the
    toolbar is too small to show all its widgets a little arrow button
    (which looks like a right-pointing chevron, '&#187;') will appear
    at the right or bottom of the toolbar depending on its
    orientation. Clicking this button pops up a menu that shows the
    'overflowing' items. TQToolButtons are represented in the menu using
    their textLabel property, other TQButton subclasses are represented
    using their text property, and TQComboBoxes are represented as submenus,
    with the caption text being used in the submenu item.

    Usually a toolbar will get precisely the space it needs. However,
    with setHorizontalStretchable(), setVerticalStretchable() or
    setStretchableWidget() you can tell the main window to expand the
    toolbar to fill all available space in the specified orientation.

    The toolbar arranges its buttons either horizontally or vertically
    (see orientation() for details). Generally, TQDockArea will set the
    orientation correctly for you, but you can set it yourself with
    setOrientation() and track any changes by connecting to the
    orientationChanged() signal.

    You can use the clear() method to remove all items from a toolbar.

    \img qdockwindow.png Toolbar (dock window)
    \caption A floating TQToolbar (dock window)

    \sa TQToolButton TQMainWindow \link http://www.iarchitect.com/visual.htm Parts of Isys on Visual Design\endlink \link guibooks.html#fowler GUI Design Handbook: Tool Bar\endlink.
*/

/*!
    \fn TQToolBar::TQToolBar( const TQString &label,
	      TQMainWindow *, ToolBarDock = Top,
	      bool newLine = false, const char * name = 0 );
    \obsolete
*/

/*!
    Constructs an empty toolbar.

    The toolbar is called \a name and is a child of \a parent and is
    managed by \a parent. It is initially located in dock area \a dock
    and is labeled \a label. If \a newLine is true the toolbar will be
    placed on a new line in the dock area.
*/

TQToolBar::TQToolBar( const TQString &label,
		    TQMainWindow * parent, TQMainWindow::ToolBarDock dock,
		    bool newLine, const char * name )
    : TQDockWindow( InDock, parent, name, 0, true )
{
    mw = parent;
    init();

    if ( parent )
	parent->addToolBar( this, label, dock, newLine );
}


/*!
    Constructs an empty horizontal toolbar.

    The toolbar is called \a name and is a child of \a parent and is
    managed by \a mainWindow. The \a label and \a newLine parameters
    are passed straight to TQMainWindow::addDockWindow(). \a name and
    the widget flags \a f are passed on to the TQDockWindow constructor.

    Use this constructor if you want to create torn-off (undocked,
    floating) toolbars or toolbars in the \link TQStatusBar status
    bar\endlink.
*/

TQToolBar::TQToolBar( const TQString &label, TQMainWindow * mainWindow,
		    TQWidget * parent, bool newLine, const char * name,
		    WFlags f )
    : TQDockWindow( InDock, parent, name, f, true )
{
    mw = mainWindow;
    init();

    clearWFlags( WType_Dialog | WStyle_Customize | WStyle_NoBorder );
    reparent( parent, TQPoint( 0, 0 ), false );

    if ( mainWindow )
	mainWindow->addToolBar( this, label, TQMainWindow::DockUnmanaged, newLine );
}


/*!
    \overload

    Constructs an empty toolbar called \a name, with parent \a parent,
    in its \a parent's top dock area, without any label and without
    requiring a newline.
*/

TQToolBar::TQToolBar( TQMainWindow * parent, const char * name )
    : TQDockWindow( InDock, parent, name, 0, true )
{
    mw = parent;
    init();

    if ( parent )
	parent->addToolBar( this, TQString::null, TQMainWindow::DockTop );
}

/*!
    \internal

  Common initialization code. Requires that \c mw and \c o are set.
  Does not call TQMainWindow::addDockWindow().
*/
void TQToolBar::init()
{
    d = new TQToolBarPrivate;
    d->extension = 0;
    d->extensionPopup = 0;
    sw = 0;

    setBackgroundMode( PaletteButton);
    setFocusPolicy( NoFocus );
    setFrameStyle( TQFrame::ToolBarPanel | TQFrame::Raised);
    boxLayout()->setSpacing(style().pixelMetric(TQStyle::PM_ToolBarItemSpacing));
}

/*!
    \reimp
*/

TQToolBar::~TQToolBar()
{
    delete d;
    d = 0;
}

/*!
    \reimp
*/

void TQToolBar::setOrientation( Orientation o )
{
    TQDockWindow::setOrientation( o );
    if (d->extension)
	d->extension->setOrientation( o );
    TQObjectList *childs = queryList( "TQToolBarSeparator" );
    if ( childs ) {
        TQObject *ob = 0;
	for ( ob = childs->first(); ob; ob = childs->next() ) {
	    TQToolBarSeparator* w = (TQToolBarSeparator*)ob;
	    w->setOrientation( o );
        }
    }
    delete childs;
}

/*!
    Adds a separator to the right/bottom of the toolbar.
*/

void TQToolBar::addSeparator()
{
    (void) new TQToolBarSeparator( orientation(), this, "toolbar separator" );
}

/*!
    \reimp
*/

void TQToolBar::styleChange( TQStyle& )
{
    TQObjectList *childs = queryList( "TQWidget" );
    if ( childs ) {
        TQObject *ob = 0;
	for ( ob = childs->first(); ob; ob = childs->next() ) {
	    TQWidget *w = (TQWidget*)ob;
            if ( ::tqt_cast<TQToolButton*>(w) || ::tqt_cast<TQToolBarSeparator*>(w) )
                w->setStyle( &style() );
        }
    }
    delete childs;
    boxLayout()->setSpacing(style().pixelMetric(TQStyle::PM_ToolBarItemSpacing));
}

/*!
    \reimp.
*/

void TQToolBar::show()
{
    TQDockWindow::show();
    if ( mw )
	mw->triggerLayout( false );
    checkForExtension( size() );
}


/*!
    \reimp
*/

void TQToolBar::hide()
{
    TQDockWindow::hide();
    if ( mw )
	mw->triggerLayout( false );
}

/*!
    Returns a pointer to the TQMainWindow which manages this toolbar.
*/

TQMainWindow * TQToolBar::mainWindow() const
{
    return mw;
}


/*!
    Sets the widget \a w to be expanded if this toolbar is requested
    to stretch.

    The request to stretch might occur because TQMainWindow
    right-justifies the dock area the toolbar is in, or because this
    toolbar's isVerticalStretchable() or isHorizontalStretchable() is
    set to true.

    If you call this function and the toolbar is not yet stretchable,
    setStretchable() is called.

    \sa TQMainWindow::setRightJustification(), setVerticalStretchable(),
    setHorizontalStretchable()
*/

void TQToolBar::setStretchableWidget( TQWidget * w )
{
    sw = w;
    boxLayout()->setStretchFactor( w, 1 );

    if ( !isHorizontalStretchable() && !isVerticalStretchable() ) {
	if ( orientation() == Horizontal )
	    setHorizontalStretchable( true );
	else
	    setVerticalStretchable( true );
    }
}


/*!
    \reimp
*/

bool TQToolBar::event( TQEvent * e )
{
    bool r =  TQDockWindow::event( e );
    // After the event filters have dealt with it, do our stuff.
    if ( e->type() == TQEvent::ChildInserted ) {
	TQObject * child = ((TQChildEvent*)e)->child();
	if ( child && child->isWidgetType() && !((TQWidget*)child)->isTopLevel()
	     && child->parent() == this
	     && qstrcmp("qt_dockwidget_internal", child->name()) != 0 ) {
	    boxLayout()->addWidget( (TQWidget*)child );
	    if ( isVisible() ) {
		if ( ((TQWidget*)child)->testWState( WState_CreatedHidden ) )
		    ((TQWidget*)child)->show();
		checkForExtension( size() );
	    }
	}
	if ( child && child->isWidgetType() && ((TQWidget*)child) == sw )
	    boxLayout()->setStretchFactor( (TQWidget*)child, 1 );
    } else if ( e->type() == TQEvent::Show ) {
	layout()->activate();
    } else if ( e->type() == TQEvent::LayoutHint && place() == OutsideDock ) {
	adjustSize();
    }
    return r;
}


/*!
    \property TQToolBar::label
    \brief the toolbar's label.

    If the toolbar is floated the label becomes the toolbar window's
    caption. There is no default label text.
*/

void TQToolBar::setLabel( const TQString & label )
{
    l = label;
    setCaption( l );
}

TQString TQToolBar::label() const
{
    return l;
}


/*!
    Deletes all the toolbar's child widgets.
*/

void TQToolBar::clear()
{
    if ( !children() )
	return;
    TQObjectListIt it( *children() );
    TQObject * obj;
    while( (obj=it.current()) != 0 ) {
	++it;
	if ( obj->isWidgetType() &&
	     qstrcmp( "qt_dockwidget_internal", obj->name() ) != 0 )
	    delete obj;
    }
}

/*!
    \reimp
*/

TQSize TQToolBar::minimumSize() const
{
    if ( orientation() == Horizontal )
	return TQSize( 0, TQDockWindow::minimumSize().height() );
    return TQSize( TQDockWindow::minimumSize().width(), 0 );
}

/*!
    \reimp
*/

TQSize TQToolBar::minimumSizeHint() const
{
    if ( orientation() == Horizontal )
	return TQSize( 0, TQDockWindow::minimumSizeHint().height() );
    return TQSize( TQDockWindow::minimumSizeHint().width(), 0 );
}

void TQToolBar::createPopup()
{
    if (!d->extensionPopup) {
	d->extensionPopup = new TQPopupMenu( this, "qt_dockwidget_internal" );
	connect( d->extensionPopup, TQ_SIGNAL( aboutToShow() ), this, TQ_SLOT( createPopup() ) );
    }

    if (!d->extension) {
	d->extension = new TQToolBarExtensionWidget( this );
	d->extension->setOrientation(orientation());
	d->extension->button()->setPopup( d->extensionPopup );
	d->extension->button()->setPopupDelay( -1 );
    }

    d->extensionPopup->clear();
    // clear doesn't delete submenus, so do this explicitly
    TQObjectList *childlist = d->extensionPopup->queryList( "TQPopupMenu", 0, false, true );
    childlist->setAutoDelete(true);
    delete childlist;

    childlist = queryList( "TQWidget", 0, false, true );
    TQObjectListIt it( *childlist );
    bool hide = false;
    bool doHide = false;
    int id;
    while ( it.current() ) {
        int j = 2;
        if ( !it.current()->isWidgetType() || it.current() == d->extension->button() ||
	    qstrcmp( "qt_dockwidget_internal", it.current()->name() ) == 0 ) {
	    ++it;
	    continue;
	}
	TQWidget *w = (TQWidget*)it.current();
#ifndef TQT_NO_COMBOBOX
	if ( ::tqt_cast<TQComboBox*>(w) )
	    j = 1;
#endif
	hide = false;
	TQPoint p = w->parentWidget()->mapTo( this, w->geometry().bottomRight() );
	if ( orientation() == Horizontal ) {
	    if ( p.x() > ( doHide ? width() - d->extension->width() / j : width() ) )
	        hide = true;
	} else {
	    if ( p.y() > ( doHide ? height()- d->extension->height() / j : height() ) )
	        hide = true;
	}
	if ( hide && w->isVisible() ) {
	    doHide = true;
	    if ( ::tqt_cast<TQToolButton*>(w) ) {
		TQToolButton *b = (TQToolButton*)w;
	        TQString s = b->textLabel();
	        if ( s.isEmpty() )
		    s = b->text();
	        if ( b->popup() && b->popupDelay() <= 0 )
		    id = d->extensionPopup->insertItem( b->iconSet(), s, b->popup() );
		else
		    id = d->extensionPopup->insertItem( b->iconSet(), s, b, TQ_SLOT( emulateClick() ) ) ;
	        if ( b->isToggleButton() )
		    d->extensionPopup->setItemChecked( id, b->isOn() );
		if ( !b->isEnabled() )
		    d->extensionPopup->setItemEnabled( id, false );
	    } else if ( ::tqt_cast<TQButton*>(w) ) {
		TQButton *b = (TQButton*)w;
		TQString s = b->text();
		if ( s.isEmpty() )
		    s = "";
		if ( b->pixmap() )
		    id = d->extensionPopup->insertItem( *b->pixmap(), s, b, TQ_SLOT( emulateClick() ) );
		else
		    id = d->extensionPopup->insertItem( s, b, TQ_SLOT( emulateClick() ) );
		if ( b->isToggleButton() )
		    d->extensionPopup->setItemChecked( id, b->isOn() );
		if ( !b->isEnabled() )
		    d->extensionPopup->setItemEnabled( id, false );
#ifndef TQT_NO_COMBOBOX
	    } else if ( ::tqt_cast<TQComboBox*>(w) ) {
		TQComboBox *c = (TQComboBox*)w;
		if ( c->count() != 0 ) {
#ifndef TQT_NO_WIDGET_TOPEXTRA
		    TQString s = c->caption();
#else
		    TQString s;
#endif
		    if ( s.isEmpty() )
		        s = c->currentText();
		    uint maxItems = 0;
		    TQPopupMenu *cp = new TQPopupMenu(d->extensionPopup);
                    cp->setEnabled(c->isEnabled());
		    d->extensionPopup->insertItem( s, cp );
		    connect( cp, TQ_SIGNAL( activated(int) ), c, TQ_SLOT( internalActivate(int) ) );
		    for ( int i = 0; i < c->count(); ++i ) {
		        TQString tmp = c->text( i );
			cp->insertItem( tmp, i );
		        if ( c->currentText() == tmp )
			    cp->setItemChecked( i, true );
			if ( !maxItems ) {
			    if ( cp->count() == 10 ) {
			        int h = cp->sizeHint().height();
			        maxItems = TQApplication::desktop()->height() * 10 / h;
			    }
			} else if ( cp->count() >= maxItems - 1 ) {
			    TQPopupMenu* sp = new TQPopupMenu(d->extensionPopup);
			    cp->insertItem( tr( "More..." ), sp );
			    cp = sp;
			    connect( cp, TQ_SIGNAL( activated(int) ), c, TQ_SLOT( internalActivate(int) ) );
			}
		    }
		}
#endif //TQT_NO_COMBOBOX
	    }
	}
        ++it;
    }
    delete childlist;
}


/*!
    \reimp
*/

void TQToolBar::resizeEvent( TQResizeEvent *e )
{
    checkForExtension( e->size() );
}

void TQToolBar::checkForExtension( const TQSize &sz )
{
    if (!isVisible())
	return;

    bool tooSmall;
    if ( orientation() == Horizontal )
	tooSmall = sz.width() < sizeHint().width();
    else
	tooSmall = sz.height() < sizeHint().height();

    if ( tooSmall ) {
	createPopup();
	if ( d->extensionPopup->count() ) {
	    if ( orientation() == Horizontal )
	        d->extension->setGeometry( width() - 20, 1, 20, height() - 2 );
	    else
	        d->extension->setGeometry( 1, height() - 20, width() - 2, 20 );
	    d->extension->show();
	    d->extension->raise();
	} else {
	    delete d->extension;
	    d->extension = 0;
	    delete d->extensionPopup;
	    d->extensionPopup = 0;
	}
    } else {
	delete d->extension;
	d->extension = 0;
	delete d->extensionPopup;
	d->extensionPopup = 0;
    }
}


/*!
    \reimp
*/

void TQToolBar::setMinimumSize( int, int )
{
}

/* from chaunsee:

1.  Tool Bars should contain only high-frequency functions.  Avoid putting
things like About and Exit on a tool bar unless they are frequent functions.

2.  All tool bar buttons must have some keyboard access method (it can be a
menu or shortcut key or a function in a dialog box that can be accessed
through the keyboard).

3.  Make tool bar functions as efficient as possible (the common example is to
Print in Microsoft applications, it doesn't bring up the Print dialog box, it
prints immediately to the default printer).

4.  Avoid turning tool bars into graphical menu bars.  To me, a tool bar should
be efficient. Once you make almost all the items in a tool bar into graphical
pull-down menus, you start to lose efficiency.

5.  Make sure that adjacent icons are distinctive. There are some tool bars
where you see a group of 4-5 icons that represent related functions, but they
are so similar that you can't differentiate among them.	 These tool bars are
often a poor attempt at a "common visual language".

6.  Use any de facto standard icons of your platform (for windows use the
cut, copy and paste icons provided in dev kits rather than designing your
own).

7.  Avoid putting a highly destructive tool bar button (delete database) by a
safe, high-frequency button (Find) -- this will yield 1-0ff errors).

8.  Tooltips in many Microsoft products simply reiterate the menu text even
when that is not explanatory.  Consider making your tooltips slightly more
verbose and explanatory than the corresponding menu item.

9.  Keep the tool bar as stable as possible when you click on different
objects. Consider disabling tool bar buttons if they are used in most, but not
all contexts.

10.  If you have multiple tool bars (like the Microsoft MMC snap-ins have),
put the most stable tool bar to at the left with less stable ones to the
right. This arrangement (stable to less stable) makes the tool bar somewhat
more predictable.

11.  Keep a single tool bar to fewer than 20 items divided into 4-7 groups of
items.
*/
#endif
