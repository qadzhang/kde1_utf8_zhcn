/**********************************************************************
**
** Implementation of TQGroupBox widget class
**
** Created : 950203
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

#include "ntqgroupbox.h"
#ifndef TQT_NO_GROUPBOX
#include "ntqlayout.h"
#include "ntqpainter.h"
#include "ntqbitmap.h"
#include "ntqaccel.h"
#include "ntqradiobutton.h"
#include "ntqfocusdata.h"
#include "ntqobjectlist.h"
#include "ntqdrawutil.h"
#include "ntqapplication.h"
#include "ntqstyle.h"
#include "ntqcheckbox.h"
#include "ntqbuttongroup.h"
#if defined(QT_ACCESSIBILITY_SUPPORT)
#include "ntqaccessible.h"
#endif

/*!
    \class TQGroupBox ntqgroupbox.h
    \brief The TQGroupBox widget provides a group box frame with a title.

    \ingroup organizers
    \ingroup geomanagement
    \ingroup appearance
    \mainclass

    A group box provides a frame, a title and a keyboard shortcut, and
    displays various other widgets inside itself. The title is on top,
    the keyboard shortcut moves keyboard focus to one of the group
    box's child widgets, and the child widgets are usually laid out
    horizontally (or vertically) inside the frame.

    The simplest way to use it is to create a group box with the
    desired number of columns (or rows) and orientation, and then just
    create widgets with the group box as parent.

    It is also possible to change the orientation() and number of
    columns() after construction, or to ignore all the automatic
    layout support and manage the layout yourself. You can add 'empty'
    spaces to the group box with addSpace().

    TQGroupBox also lets you set the title() (normally set in the
    constructor) and the title's alignment().

    You can change the spacing used by the group box with
    setInsideMargin() and setInsideSpacing(). To minimize space
    consumption, you can remove the right, left and bottom edges of
    the frame with setFlat().

    <img src=qgrpbox-w.png>

    \sa TQButtonGroup
*/

class TQCheckBox;

class TQGroupBoxPrivate
{
public:
    TQGroupBoxPrivate():
	spacer( 0 ),
	checkbox( 0 ) {}

    TQSpacerItem *spacer;
    TQCheckBox *checkbox;
};




/*!
    Constructs a group box widget with no title.

    The \a parent and \a name arguments are passed to the TQWidget
    constructor.

    This constructor does not do automatic layout.
*/

TQGroupBox::TQGroupBox( TQWidget *parent, const char *name )
    : TQFrame( parent, name )
{
    init();
}

/*!
    Constructs a group box with the title \a title.

    The \a parent and \a name arguments are passed to the TQWidget
    constructor.

    This constructor does not do automatic layout.
*/

TQGroupBox::TQGroupBox( const TQString &title, TQWidget *parent, const char *name )
    : TQFrame( parent, name )
{
    init();
    setTitle( title );
}

/*!
    Constructs a group box with no title. Child widgets will be
    arranged in \a strips rows or columns (depending on \a
    orientation).

    The \a parent and \a name arguments are passed to the TQWidget
    constructor.
*/

TQGroupBox::TQGroupBox( int strips, Orientation orientation,
		    TQWidget *parent, const char *name )
    : TQFrame( parent, name )
{
    init();
    setColumnLayout( strips, orientation );
}

/*!
    Constructs a group box titled \a title. Child widgets will be
    arranged in \a strips rows or columns (depending on \a
    orientation).

    The \a parent and \a name arguments are passed to the TQWidget
    constructor.
*/

TQGroupBox::TQGroupBox( int strips, Orientation orientation,
		    const TQString &title, TQWidget *parent,
		    const char *name )
    : TQFrame( parent, name )
{
    init();
    setTitle( title );
    setColumnLayout( strips, orientation );
}

/*!
    Destroys the group box.
*/
TQGroupBox::~TQGroupBox()
{
    delete d;
}

void TQGroupBox::init()
{
    align = AlignAuto;
    setFrameStyle( TQFrame::GroupBoxPanel | TQFrame::Sunken );
#ifndef TQT_NO_ACCEL
    accel = 0;
#endif
    vbox = 0;
    grid = 0;
    d = new TQGroupBoxPrivate();
    lenvisible = 0;
    nCols = nRows = 0;
    dir = Horizontal;
    marg = 11;
    spac = 5;
    bFlat = false;
}

void TQGroupBox::setTextSpacer()
{
    if ( !d->spacer )
	return;
    int h = 0;
    int w = 0;
    if ( isCheckable() || lenvisible ) {
	TQFontMetrics fm = fontMetrics();
	int fh = fm.height();
	if ( isCheckable() ) {
#ifndef TQT_NO_CHECKBOX
	    fh = d->checkbox->sizeHint().height() + 2;
	    w = d->checkbox->sizeHint().width() + 2*fm.width( "xx" );
#endif
	} else {
	    fh = fm.height();
	    w = fm.width( str, lenvisible ) + 2*fm.width( "xx" );
	}
	h = frameRect().y();
	if ( layout() ) {
	    int m = layout()->margin();
	    int sp = layout()->spacing();
	    // do we have a child layout?
	    for ( TQLayoutIterator it = layout()->iterator(); it.current(); ++it ) {
		if ( it.current()->layout() ) {
		    m += it.current()->layout()->margin();
		    sp = TQMAX( sp, it.current()->layout()->spacing() );
		    break;
		}
	    }
	    h = TQMAX( fh-m, h );
	    h += TQMAX( sp - (h+m - fh), 0 );
	}
    }
    d->spacer->changeSize( w, h, TQSizePolicy::Minimum, TQSizePolicy::Fixed );
}


void TQGroupBox::setTitle( const TQString &title )
{
    if ( str == title )				// no change
	return;
    str = title;
#ifndef TQT_NO_ACCEL
    if ( accel )
	delete accel;
    accel = 0;
    int s = TQAccel::shortcutKey( title );
    if ( s ) {
	accel = new TQAccel( this, "automatic focus-change accelerator" );
	accel->connectItem( accel->insertItem( s, 0 ),
			    this, TQ_SLOT(fixFocus()) );
    }
#endif
#ifndef TQT_NO_CHECKBOX
    if ( d->checkbox ) {
	d->checkbox->setText( str );
	updateCheckBoxGeometry();
    }
#endif
    calculateFrame();
    setTextSpacer();

    update();
    updateGeometry();
#if defined(QT_ACCESSIBILITY_SUPPORT)
    TQAccessible::updateAccessibility( this, 0, TQAccessible::NameChanged );
#endif
}

/*!
    \property TQGroupBox::title
    \brief the group box title text.

    The group box title text will have a focus-change keyboard
    accelerator if the title contains \&, followed by a letter.

    \code
	g->setTitle( "&User information" );
    \endcode
    This produces "<u>U</u>ser information"; Alt+U moves the keyboard
    focus to the group box.

    There is no default title text.
*/

/*!
    \property TQGroupBox::alignment
    \brief the alignment of the group box title.

    The title is always placed on the upper frame line. The horizontal
    alignment can be specified by the alignment parameter.

    The alignment is one of the following flags:
    \list
    \i \c AlignAuto aligns the title according to the language,
    usually to the left.
    \i \c AlignLeft aligns the title text to the left.
    \i \c AlignRight aligns the title text to the right.
    \i \c AlignHCenter aligns the title text centered.
    \endlist

    The default alignment is \c AlignAuto.

    \sa TQt::AlignmentFlags
*/

void TQGroupBox::setAlignment( int alignment )
{
    align = alignment;
#ifndef TQT_NO_CHECKBOX
    updateCheckBoxGeometry();
#endif
    update();
}

/*! \reimp
*/
void TQGroupBox::resizeEvent( TQResizeEvent *e )
{
    TQFrame::resizeEvent(e);
#ifndef TQT_NO_CHECKBOX
    if ( align & AlignRight || align & AlignCenter ||
	 ( TQApplication::reverseLayout() && !(align & AlignLeft) ) )
	updateCheckBoxGeometry();
#endif
    calculateFrame();
}

/*! \reimp

  \internal
  overrides TQFrame::paintEvent
*/

void TQGroupBox::paintEvent( TQPaintEvent *event )
{
    TQPainter paint( this );

    if ( lenvisible && !isCheckable() ) {	// draw title
	TQFontMetrics fm = paint.fontMetrics();
	int h = fm.height();
	int tw = fm.width( str, lenvisible ) + fm.width(TQChar(' '));
	int x;
	int marg = bFlat ? 0 : 8;
	if ( align & AlignHCenter )		// center alignment
	    x = frameRect().width()/2 - tw/2;
	else if ( align & AlignRight )	// right alignment
	    x = frameRect().width() - tw - marg;
	else if ( align & AlignLeft )		 // left alignment
	    x = marg;
	else { // auto align
	    if( TQApplication::reverseLayout() )
		x = frameRect().width() - tw - marg;
	    else
		x = marg;
	}
	TQRect r( x, 0, tw, h );
	int va = style().styleHint(TQStyle::SH_GroupBox_TextLabelVerticalAlignment, this);
	if(va & AlignTop)
	    r.moveBy(0, fm.descent());
	TQColor pen( (TQRgb) style().styleHint(TQStyle::SH_GroupBox_TextLabelColor, this )  );
	if ((!style().styleHint(TQStyle::SH_UnderlineAccelerator, this)) || ((style().styleHint(TQStyle::SH_HideUnderlineAcceleratorWhenAltUp, this)) && (!style().acceleratorsShown()))) {
	    va |= NoAccel;
	}
	style().drawItem( &paint, r, ShowPrefix | AlignHCenter | va, colorGroup(),
			  isEnabled(), 0, str, -1, ownPalette() ? 0 : &pen );
	paint.setClipRegion( event->region().subtract( r ) ); // clip everything but title
#ifndef TQT_NO_CHECKBOX
    } else if ( d->checkbox ) {
	TQRect cbClip = d->checkbox->geometry();
	TQFontMetrics fm = paint.fontMetrics();
	cbClip.setX( cbClip.x() - fm.width(TQChar(' ')) );
	cbClip.setWidth( cbClip.width() + fm.width(TQChar(' ')) );
	paint.setClipRegion( event->region().subtract( cbClip ) );
#endif
    }
    if ( bFlat ) {
	    TQRect fr = frameRect();
	    TQPoint p1( fr.x(), fr.y() + 1 );
            TQPoint p2( fr.x() + fr.width(), p1.y() );
	    // ### This should probably be a style primitive.
            qDrawShadeLine( &paint, p1, p2, colorGroup(), true,
                            lineWidth(), midLineWidth() );
    } else {
	drawFrame(&paint);
    }
    drawContents( &paint );			// draw the contents
}


/*!
    Adds an empty cell at the next free position. If \a size is
    greater than 0, the empty cell takes \a size to be its fixed width
    (if orientation() is \c Horizontal) or height (if orientation() is
    \c Vertical).

    Use this method to separate the widgets in the group box or to
    skip the next free cell. For performance reasons, call this method
    after calling setColumnLayout() or by changing the \l
    TQGroupBox::columns or \l TQGroupBox::orientation properties. It is
    generally a good idea to call these methods first (if needed at
    all), and insert the widgets and spaces afterwards.
*/
void TQGroupBox::addSpace( int size )
{
    TQApplication::sendPostedEvents( this, TQEvent::ChildInserted );

    if ( nCols <= 0 || nRows <= 0 )
	return;

    if ( row >= nRows || col >= nCols )
	grid->expand( row+1, col+1 );

    if ( size > 0 ) {
	TQSpacerItem *spacer
	    = new TQSpacerItem( ( dir == Horizontal ) ? 0 : size,
			       ( dir == Vertical ) ? 0 : size,
			       TQSizePolicy::Fixed, TQSizePolicy::Fixed );
	grid->addItem( spacer, row, col );
    }

    skip();
}

/*!
    \property TQGroupBox::columns
    \brief the number of columns or rows (depending on \l TQGroupBox::orientation) in the group box

    Usually it is not a good idea to set this property because it is
    slow (it does a complete layout). It is best to set the number
    of columns directly in the constructor.
*/
int TQGroupBox::columns() const
{
    if ( dir == Horizontal )
	return nCols;
    return nRows;
}

void TQGroupBox::setColumns( int c )
{
    setColumnLayout( c, dir );
}

/*!
    Returns the width of the empty space between the items in the
    group and the frame of the group.

    Only applies if the group box has a defined orientation.

    The default is usually 11, by may vary depending on the platform
    and style.

    \sa setInsideMargin(), orientation
*/
int TQGroupBox::insideMargin() const
{
    return marg;
}

/*!
    Returns the width of the empty space between each of the items
    in the group.

    Only applies if the group box has a defined orientation.

    The default is usually 5, by may vary depending on the platform
    and style.

    \sa setInsideSpacing(), orientation
*/
int TQGroupBox::insideSpacing() const
{
    return spac;
}

/*!
    Sets the the width of the inside margin to \a m pixels.

    \sa insideMargin()
*/
void TQGroupBox::setInsideMargin( int m )
{
    marg = m;
    setColumnLayout( columns(), dir );
}

/*!
    Sets the width of the empty space between each of the items in
    the group to \a s pixels.

    \sa insideSpacing()
*/
void TQGroupBox::setInsideSpacing( int s )
{
    spac = s;
    setColumnLayout( columns(), dir );
}

/*!
    \property TQGroupBox::orientation
    \brief the group box's orientation

    A horizontal group box arranges it's children in columns, while a
    vertical group box arranges them in rows.

    Usually it is not a good idea to set this property because it is
    slow (it does a complete layout). It is better to set the
    orientation directly in the constructor.
*/
void TQGroupBox::setOrientation( TQt::Orientation o )
{
    setColumnLayout( columns(), o );
}

/*!
    Changes the layout of the group box. This function is only useful
    in combination with the default constructor that does not take any
    layout information. This function will put all existing children
    in the new layout. It is not good TQt programming style to call
    this function after children have been inserted. Sets the number
    of columns or rows to be \a strips, depending on \a direction.

    \sa orientation columns
*/
void TQGroupBox::setColumnLayout(int strips, Orientation direction)
{
    if ( layout() )
	delete layout();

    vbox = 0;
    grid = 0;

    if ( strips < 0 ) // if 0, we create the vbox but not the grid. See below.
	return;

    vbox = new TQVBoxLayout( this, marg, 0 );

    d->spacer = new TQSpacerItem( 0, 0, TQSizePolicy::Minimum,
				 TQSizePolicy::Fixed );

    setTextSpacer();
    vbox->addItem( d->spacer );

    nCols = 0;
    nRows = 0;
    dir = direction;

    // Send all child events and ignore them. Otherwise we will end up
    // with doubled insertion. This won't do anything because nCols ==
    // nRows == 0.
    TQApplication::sendPostedEvents( this, TQEvent::ChildInserted );

    // if 0 or smaller , create a vbox-layout but no grid. This allows
    // the designer to handle its own grid layout in a group box.
    if ( strips <= 0 )
	return;

    dir = direction;
    if ( dir == Horizontal ) {
	nCols = strips;
	nRows = 1;
    } else {
	nCols = 1;
	nRows = strips;
    }
    grid = new TQGridLayout( nRows, nCols, spac );
    row = col = 0;
    grid->setAlignment( AlignTop );
    vbox->addLayout( grid );

    // Add all children
    if ( children() ) {
	TQObjectListIt it( *children() );
	TQWidget *w;
	while( (w=(TQWidget *)it.current()) != 0 ) {
	    ++it;
	    if ( w->isWidgetType()
#ifndef TQT_NO_CHECKBOX
		 && w != d->checkbox
#endif
		 )
		insertWid( w );
	}
    }
}


/*! \reimp  */
bool TQGroupBox::event( TQEvent * e )
{
    if ( e->type() == TQEvent::LayoutHint && layout() )
	setTextSpacer();
    return TQFrame::event( e );
}

/*!\reimp */
void TQGroupBox::childEvent( TQChildEvent *c )
{
    if ( !c->inserted() || !c->child()->isWidgetType() )
	return;
    TQWidget *w = (TQWidget*)c->child();
#ifndef TQT_NO_CHECKBOX
    if ( d->checkbox ) {
	if ( w == d->checkbox )
	    return;
	if ( d->checkbox->isChecked() ) {
	    if ( !w->testWState( WState_ForceDisabled ) )
		w->setEnabled( true );
	} else {
	    if ( w->isEnabled() ) {
		w->setEnabled( false );
		((TQGroupBox*)w)->clearWState( WState_ForceDisabled );
	    }
	}
    }
#endif
    if ( !grid )
	return;
    insertWid( w );
}

void TQGroupBox::insertWid( TQWidget* w )
{
    if ( row >= nRows || col >= nCols )
	grid->expand( row+1, col+1 );
    grid->addWidget( w, row, col );
    skip();
    TQApplication::postEvent( this, new TQEvent( TQEvent::LayoutHint ) );
}


void TQGroupBox::skip()
{
    // Same as TQGrid::skip()
    if ( dir == Horizontal ) {
	if ( col+1 < nCols ) {
	    col++;
	} else {
	    col = 0;
	    row++;
	}
    } else { //Vertical
	if ( row+1 < nRows ) {
	    row++;
	} else {
	    row = 0;
	    col++;
	}
    }
}


/*!
    \internal

    This private slot finds a widget in this group box that can accept
    focus, and gives the focus to that widget.
*/

void TQGroupBox::fixFocus()
{
    TQFocusData * fd = focusData();
    TQWidget * orig = fd->home();
    TQWidget * best = 0;
    TQWidget * candidate = 0;
    TQWidget * w = orig;
    do {
	TQWidget * p = w;
	while( p && p != this && !p->isTopLevel() )
	    p = p->parentWidget();
	if ( p == this && ( w->focusPolicy() & TabFocus ) == TabFocus
	     && w->isVisibleTo(this) ) {
	    if ( w->hasFocus()
#ifndef TQT_NO_RADIOBUTTON
		 || ( !best && ::tqt_cast<TQRadioButton*>(w)
		 && ((TQRadioButton*)w)->isChecked() )
#endif
		    )
		// we prefer a checked radio button or a widget that
		// already has focus, if there is one
		best = w;
	    else if ( !candidate )
		// but we'll accept anything that takes focus
		candidate = w;
	}
	w = fd->next();
    } while( w != orig );
    if ( best )
	best->setFocus();
    else if ( candidate )
	candidate->setFocus();
}


/*
    Sets the right frame rect depending on the title. Also calculates
    the visible part of the title.
*/
void TQGroupBox::calculateFrame()
{
    lenvisible = str.length();

    if ( lenvisible && !isCheckable() ) { // do we have a label?
	TQFontMetrics fm = fontMetrics();
	while ( lenvisible ) {
	    int tw = fm.width( str, lenvisible ) + 4*fm.width(TQChar(' '));
	    if ( tw < width() )
		break;
	    lenvisible--;
	}
	if ( lenvisible ) { // but do we also have a visible label?
	    TQRect r = rect();
	    int va = style().styleHint(TQStyle::SH_GroupBox_TextLabelVerticalAlignment, this);
	    if(va & AlignVCenter)
		r.setTop( fm.height()/2 );				// frame rect should be
	    else if(va & AlignTop)
		r.setTop(fm.ascent());
	    setFrameRect( r );			//   smaller than client rect
	    return;
	}
    } else if ( isCheckable() ) {
#ifndef TQT_NO_CHECKBOX
	TQRect r = rect();
	int va = style().styleHint(TQStyle::SH_GroupBox_TextLabelVerticalAlignment, this);
	if( va & AlignVCenter )
	    r.setTop( d->checkbox->rect().height()/2 );
	else if( va & AlignTop )
	    r.setTop( fontMetrics().ascent() );
	setFrameRect( r );
	return;
#endif
    }

    // no visible label
    setFrameRect( TQRect(0,0,0,0) );		//  then use client rect
}



/*! \reimp
 */
void TQGroupBox::focusInEvent( TQFocusEvent * )
{ // note no call to super
    fixFocus();
}


/*!\reimp
 */
void TQGroupBox::fontChange( const TQFont & oldFont )
{
    TQWidget::fontChange( oldFont );
#ifndef TQT_NO_CHECKBOX
    updateCheckBoxGeometry();
#endif
    calculateFrame();
    setTextSpacer();
}

/*!
  \reimp
*/

TQSize TQGroupBox::sizeHint() const
{
    TQFontMetrics fm( font() );
    int tw, th;
    if ( isCheckable() ) {
#ifndef TQT_NO_CHECKBOX
	tw = d->checkbox->sizeHint().width() + 2*fm.width( "xx" );
	th = d->checkbox->sizeHint().height() + fm.width( TQChar(' ') );
#endif
    } else {
	tw = fm.width( title() ) + 2 * fm.width( "xx" );
	th = fm.height() + fm.width( TQChar(' ') );
    }

    TQSize s;
    if ( layout() ) {
	s = TQFrame::sizeHint();
	return s.expandedTo( TQSize( tw, 0 ) );
    } else {
	TQRect r = childrenRect();
	TQSize s( 100, 50 );
	s = s.expandedTo( TQSize( tw, th ) );
	if ( r.isNull() )
	    return s;

	return s.expandedTo( TQSize( r.width() + 2 * r.x(), r.height()+ 2 * r.y() ) );
    }
}

/*!
    \property TQGroupBox::flat
    \brief whether the group box is painted flat or has a frame

    By default a group box has a surrounding frame, with the title
    being placed on the upper frame line. In flat mode the right, left
    and bottom frame lines are omitted, and only the thin line at the
    top is drawn.

    \sa title
*/
bool TQGroupBox::isFlat() const
{
    return bFlat;
}

void TQGroupBox::setFlat( bool b )
{
    if ( (bool)bFlat == b )
	return;
    bFlat = b;
    update();
}


/*!
    \property TQGroupBox::checkable
    \brief Whether the group box has a checkbox in its title.

    If this property is true, the group box has a checkbox. If the
    checkbox is checked (which is the default), the group box's
    children are enabled.

    setCheckable() controls whether or not the group box has a
    checkbox, and isCheckable() controls whether the checkbox is
    checked or not.
*/
#ifndef TQT_NO_CHECKBOX
void TQGroupBox::setCheckable( bool b )
{
    if ( (d->checkbox != 0) == b )
	return;

    if ( b ) {
	if ( !d->checkbox ) {
	    d->checkbox = new TQCheckBox( title(), this, "qt_groupbox_checkbox" );
            if (TQButtonGroup *meAsButtonGroup = ::tqt_cast<TQButtonGroup*>(this))
                meAsButtonGroup->remove(d->checkbox);
	    setChecked( true );
	    setChildrenEnabled( true );
	    connect( d->checkbox, TQ_SIGNAL( toggled(bool) ),
		     this, TQ_SLOT( setChildrenEnabled(bool) ) );
	    connect( d->checkbox, TQ_SIGNAL( toggled(bool) ),
		     this, TQ_SIGNAL( toggled(bool) ) );
	    updateCheckBoxGeometry();
	}
	d->checkbox->show();
    } else {
	setChildrenEnabled( true );
	delete d->checkbox;
	d->checkbox = 0;
    }
    calculateFrame();
    setTextSpacer();
    update();
}
#endif //TQT_NO_CHECKBOX

bool TQGroupBox::isCheckable() const
{
#ifndef TQT_NO_CHECKBOX
    return ( d->checkbox != 0 );
#else
    return false;
#endif
}


bool TQGroupBox::isChecked() const
{
#ifndef TQT_NO_CHECKBOX
    return d->checkbox && d->checkbox->isChecked();
#else
    return false;
#endif
}


/*!
    \fn void TQGroupBox::toggled( bool on )

    If the group box has a check box (see \l isCheckable()) this signal
    is emitted when the check box is toggled. \a on is true if the check
    box is checked; otherwise it is false.
*/

/*!
    \property TQGroupBox::checked
    \brief Whether the group box's checkbox is checked.

    If the group box has a check box (see \l isCheckable()), and the
    check box is checked (see \l isChecked()), the group box's children
    are enabled. If the checkbox is unchecked the children are
    disabled.
*/
#ifndef TQT_NO_CHECKBOX
void TQGroupBox::setChecked( bool b )
{
    if ( d->checkbox )
	d->checkbox->setChecked( b );
}
#endif

/*
  sets all children of the group box except the qt_groupbox_checkbox
  to either disabled/enabled
*/
void TQGroupBox::setChildrenEnabled( bool b )
{
    if ( !children() )
	return;
    TQObjectListIt it( *children() );
    TQObject *o;
    while( (o = it.current()) ) {
	++it;
	if ( o->isWidgetType()
#ifndef TQT_NO_CHECKBOX
	     && o != d->checkbox
#endif
	     ) {
	    TQWidget *w = (TQWidget*)o;
	    if ( b ) {
		if ( !w->testWState( WState_ForceDisabled ) )
		    w->setEnabled( true );
	    } else {
		if ( w->isEnabled() ) {
		    w->setEnabled( false );
		    ((TQGroupBox*)w)->clearWState( WState_ForceDisabled );
		}
	    }
	}
    }
}

/*! \reimp */
void TQGroupBox::setEnabled(bool on)
{
    TQFrame::setEnabled(on);
    if ( !d->checkbox || !on )
	return;

#ifndef TQT_NO_CHECKBOX
    // we are being enabled - disable children
    if ( !d->checkbox->isChecked() )
	setChildrenEnabled( false );
#endif
}

/*
  recalculates and sets the checkbox setGeometry
*/
#ifndef TQT_NO_CHECKBOX
void TQGroupBox::updateCheckBoxGeometry()
{
    if ( d->checkbox ) {
	TQSize cbSize = d->checkbox->sizeHint();
	TQRect cbRect( 0, 0, cbSize.width(), cbSize.height() );

	int marg = bFlat ? 2 : 8;
	marg += fontMetrics().width( TQChar(' ') );

	if ( align & AlignHCenter ) {
	    cbRect.moveCenter( frameRect().center() );
	    cbRect.moveTop( 0 );
	} else if ( align & AlignRight ) {
	    cbRect.moveRight( frameRect().right() - marg );
	} else if ( align & AlignLeft ) {
	    cbRect.moveLeft( frameRect().left() + marg );
	} else { // auto align
	    if( TQApplication::reverseLayout() )
		cbRect.moveRight( frameRect().right() - marg );
	    else
		cbRect.moveLeft( frameRect().left() + marg );
	}

	d->checkbox->setGeometry( cbRect );
    }
}
#endif //TQT_NO_CHECKBOX


#endif //TQT_NO_GROUPBOX
