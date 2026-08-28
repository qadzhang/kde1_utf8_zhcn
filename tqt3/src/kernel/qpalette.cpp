/****************************************************************************
**
** Implementation of TQColorGroup and TQPalette classes
**
** Created : 950323
**
** Copyright (C) 1992-2008 Trolltech ASA.  All rights reserved.
**
** This file is part of the kernel module of the TQt GUI Toolkit.
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

#include "ntqpalette.h"

#ifndef TQT_NO_PALETTE
#include "ntqdatastream.h"
#include "ntqcleanuphandler.h"

/*****************************************************************************
  TQColorGroup member functions
 *****************************************************************************/

/*!
    \class TQColorGroup ntqpalette.h
    \brief The TQColorGroup class contains a group of widget colors.

    \ingroup appearance
    \ingroup graphics
    \ingroup images

    A color group contains a group of colors used by widgets for
    drawing themselves. We recommend that widgets use color group
    roles such as "foreground" and "base" rather than literal colors
    like "red" or "turquoise". The color roles are enumerated and
    defined in the \l ColorRole documentation.

    The most common use of TQColorGroup is like this:

    \code
	TQPainter p;
	...
	p.setPen( colorGroup().foreground() );
	p.drawLine( ... )
    \endcode

    It is also possible to modify color groups or create new color
    groups from scratch.

    The color group class can be created using three different
    constructors or by modifying one supplied by TQt. The default
    constructor creates an all-black color group, which can then be
    modified using set functions; there's also a constructor for
    specifying all the color group colors. And there is also a copy
    constructor.

    We strongly recommend using a system-supplied color group and
    modifying that as necessary.

    You modify a color group by calling the access functions
    setColor() and setBrush(), depending on whether you want a pure
    color or a pixmap pattern.

    There are also corresponding color() and brush() getters, and a
    commonly used convenience function to get each ColorRole:
    background(), foreground(), base(), etc.

    \sa TQColor TQPalette TQWidget::colorGroup()
*/


/*!
    \enum TQColorGroup::ColorRole

    The ColorRole enum defines the different symbolic color roles used
    in current GUIs.

    The central roles are:

    \value Background  general background color.

    \value Foreground  general foreground color.

    \value Base  used as background color for text entry widgets, for example;
    usually white or another light color.

    \value Text  the foreground color used with \c Base. Usually this
    is the same as the \c Foreground, in which case it must provide good
    contrast with \c Background and \c Base.

    \value Button  general button background color in which buttons need a
    background different from \c Background, as in the Macintosh style.

    \value ButtonText  a foreground color used with the \c Button color.

    There are some color roles used mostly for 3D bevel and shadow
    effects:

    \value Light  lighter than \c Button color.

    \value Midlight  between \c Button and \c Light.

    \value Dark  darker than \c Button.

    \value Mid  between \c Button and \c Dark.

    \value Shadow  a very dark color.
    By default, the shadow color is \c TQt::black.

    All of these are normally derived from \c Background and used in
    ways that depend on that relationship. For example, buttons depend
    on it to make the bevels look attractive, and Motif scroll bars
    depend on \c Mid to be slightly different from \c Background.

    Selected (marked) items have two roles:

    \value Highlight   a color to indicate a selected item or the
    current item. By default, the highlight color is \c TQt::darkBlue.

    \value HighlightedText  a text color that contrasts with \c Highlight.
    By default, the highlighted text color is \c TQt::white.

    Finally, there is a special role for text that needs to be
    drawn where \c Text or \c Foreground would give poor contrast,
    such as on pressed push buttons:

    \value BrightText a text color that is very different from \c
    Foreground and contrasts well with e.g. \c Dark.

    \value Link a text color used for unvisited hyperlinks.
    By default, the link color is \c TQt::blue.

    \value LinkVisited a text color used for already visited hyperlinks.
    By default, the linkvisited color is \c TQt::magenta.

    \value NColorRoles Internal.

    Note that text colors can be used for things other than just
    words; text colors are \e usually used for text, but it's quite
    common to use the text color roles for lines, icons, etc.

    This image shows most of the color roles in use:
    \img palette.png Color Roles
*/


class TQColorGroupPrivate : public TQShared
{
public:
    TQBrush br[TQColorGroup::NColorRoles];
    TQColorGroupPrivate* detach() {
	if ( count > 1 ) {
	    deref();
	    TQColorGroupPrivate* d = new TQColorGroupPrivate;
	    for (int i=0; i<TQColorGroup::NColorRoles; i++)
		d->br[i] = br[i];
	    return d;
	}
	return this;
    }
};

/*!
    Constructs a color group with all colors set to black.
*/

TQColorGroup::TQColorGroup()
{
    static TQColorGroupPrivate* defColorGroupData = 0;
    if ( !defColorGroupData ) {
	static TQSharedCleanupHandler<TQColorGroupPrivate> defColorGroupCleanup;
	defColorGroupData = new TQColorGroupPrivate;
	defColorGroupCleanup.set( &defColorGroupData );
    }
    d = defColorGroupData;
    br = d->br;
    d->ref();
}

/*!
    Constructs a color group that is an independent copy of \a other.
*/
TQColorGroup::TQColorGroup( const TQColorGroup& other )
{
    d = other.d;
    d->ref();
    br = d->br;
}

/*!
    Copies the colors of \a other to this color group.
*/
TQColorGroup& TQColorGroup::operator =(const TQColorGroup& other)
{
    if ( d != other.d ) {
	if ( d->deref() )
	    delete d;
	d = other.d;
	br = d->br;
	d->ref();
    }
    return *this;
}

static TQColor tqt_mix_colors( TQColor a, TQColor b)
{
    return TQColor( (a.red() + b.red()) / 2, (a.green() + b.green()) / 2, (a.blue() + b.blue()) / 2 );
}


/*!
    Constructs a color group. You can pass either brushes, pixmaps or
    plain colors for \a foreground, \a button, \a light, \a dark, \a
    mid, \a text, \a bright_text, \a base and \a background.

    \sa TQBrush
*/
 TQColorGroup::TQColorGroup( const TQBrush &foreground, const TQBrush &button,
			   const TQBrush &light, const TQBrush &dark,
			   const TQBrush &mid, const TQBrush &text,
			   const TQBrush &bright_text, const TQBrush &base,
			   const TQBrush &background)
{
    d = new TQColorGroupPrivate;
    br = d->br;
    br[Foreground]      = foreground;
    br[Button]		= button;
    br[Light]		= light;
    br[Dark]		= dark;
    br[Mid]		= mid;
    br[Text]		= text;
    br[BrightText]	= bright_text;
    br[ButtonText]	= text;
    br[Base]		= base;
    br[Background]	= background;
    br[Midlight]	= tqt_mix_colors( br[Button].color(), br[Light].color() );
    br[Shadow]          = TQt::black;
    br[Highlight]       = TQt::darkBlue;
    br[HighlightedText] = TQt::white;
    br[Link]            = TQt::blue;
    br[LinkVisited]     = TQt::magenta;
}


/*!\obsolete

  Constructs a color group with the specified colors. The button
  color will be set to the background color.
*/

TQColorGroup::TQColorGroup( const TQColor &foreground, const TQColor &background,
			  const TQColor &light, const TQColor &dark,
			  const TQColor &mid,
			  const TQColor &text, const TQColor &base )
{
    d = new TQColorGroupPrivate;
    br = d->br;
    br[Foreground]      = TQBrush(foreground);
    br[Button]          = TQBrush(background);
    br[Light]           = TQBrush(light);
    br[Dark]            = TQBrush(dark);
    br[Mid]             = TQBrush(mid);
    br[Text]            = TQBrush(text);
    br[BrightText]      = br[Light];
    br[ButtonText]      = br[Text];
    br[Base]            = TQBrush(base);
    br[Background]      = TQBrush(background);
    br[Midlight]	= tqt_mix_colors( br[Button].color(), br[Light].color() );
    br[Shadow]          = TQt::black;
    br[Highlight]       = TQt::darkBlue;
    br[HighlightedText] = TQt::white;
    br[Link]            = TQt::blue;
    br[LinkVisited]     = TQt::magenta;
}

/*!
    Destroys the color group.
*/

TQColorGroup::~TQColorGroup()
{
    if ( d->deref() )
	delete d;
}

/*!
    Returns the color that has been set for color role \a r.

    \sa brush() ColorRole
 */
const TQColor &TQColorGroup::color( ColorRole r ) const
{
    return br[r].color();
}

/*!
    Returns the brush that has been set for color role \a r.

    \sa color() setBrush() ColorRole
*/
const TQBrush &TQColorGroup::brush( ColorRole r ) const
{
    return br[r];
}

/*!
    Sets the brush used for color role \a r to a solid color \a c.

    \sa brush() setColor() ColorRole
*/
void TQColorGroup::setColor( ColorRole r, const TQColor &c )
{
    setBrush( r, TQBrush(c) );
}

/*!
    Sets the brush used for color role \a r to \a b.

    \sa brush() setColor() ColorRole
*/
void TQColorGroup::setBrush( ColorRole r, const TQBrush &b )
{
    d = d->detach();
    br = d->br;
    br[r] = b;
}


/*!
    \fn const TQColor & TQColorGroup::foreground() const

    Returns the foreground color of the color group.

    \sa ColorRole
*/

/*!
    \fn const TQColor & TQColorGroup::button() const

    Returns the button color of the color group.

    \sa ColorRole
*/

/*!
    \fn const TQColor & TQColorGroup::light() const

    Returns the light color of the color group.

    \sa ColorRole
*/

/*!
    \fn const TQColor& TQColorGroup::midlight() const

    Returns the midlight color of the color group.

    \sa ColorRole
*/

/*!
    \fn const TQColor & TQColorGroup::dark() const

    Returns the dark color of the color group.

    \sa ColorRole
*/

/*!
    \fn const TQColor & TQColorGroup::mid() const

    Returns the mid color of the color group.

    \sa ColorRole
*/

/*!
    \fn const TQColor & TQColorGroup::text() const

    Returns the text foreground color of the color group.

    \sa ColorRole
*/

/*!
    \fn const TQColor & TQColorGroup::brightText() const

    Returns the bright text foreground color of the color group.

    \sa ColorRole
*/

/*!
    \fn const TQColor & TQColorGroup::buttonText() const

    Returns the button text foreground color of the color group.

    \sa ColorRole
*/

/*!
    \fn const TQColor & TQColorGroup::base() const

    Returns the base color of the color group.

    \sa ColorRole
*/

/*!
    \fn const TQColor & TQColorGroup::background() const

    Returns the background color of the color group.

    \sa ColorRole
*/

/*!
    \fn const TQColor & TQColorGroup::shadow() const

    Returns the shadow color of the color group.

    \sa ColorRole
*/

/*!
    \fn const TQColor & TQColorGroup::highlight() const

    Returns the highlight color of the color group.

    \sa ColorRole
*/

/*!
    \fn const TQColor & TQColorGroup::highlightedText() const

    Returns the highlighted text color of the color group.

    \sa ColorRole
*/

/*!
    \fn const TQColor & TQColorGroup::link() const

    Returns the unvisited link text color of the color group.

    \sa ColorRole
*/

/*!
    \fn const TQColor & TQColorGroup::linkVisited() const

    Returns the visited link text color of the color group.

    \sa ColorRole
*/

/*!
    \fn bool TQColorGroup::operator!=( const TQColorGroup &g ) const

    Returns true if this color group is different from \a g; otherwise
    returns  false.

    \sa operator!=()
*/

/*!
    Returns true if this color group is equal to \a g; otherwise
    returns false.

    \sa operator==()
*/

bool TQColorGroup::operator==( const TQColorGroup &g ) const
{
    if ( d == g.d )
	return true;
    for( int r = 0 ; r < NColorRoles ; r++ )
	if ( br[r] != g.br[r] )
	    return false;
    return true;
}


/*****************************************************************************
  TQPalette member functions
 *****************************************************************************/

/*!
    \class TQPalette ntqpalette.h

    \brief The TQPalette class contains color groups for each widget state.

    \ingroup appearance
    \ingroup shared
    \ingroup graphics
    \ingroup images
    \mainclass

    A palette consists of three color groups: \e active, \e disabled,
    and \e inactive. All widgets contain a palette, and all widgets in
    TQt use their palette to draw themselves. This makes the user
    interface easily configurable and easier to keep consistent.

    If you create a new widget we strongly recommend that you use the
    colors in the palette rather than hard-coding specific colors.

    The color groups:
    \list
    \i The active() group is used for the window that has keyboard focus.
    \i The inactive() group is used for other windows.
    \i The disabled() group is used for widgets (not windows) that are
    disabled for some reason.
    \endlist

    Both active and inactive windows can contain disabled widgets.
    (Disabled widgets are often called \e inaccessible or \e{grayed
    out}.)

    In Motif style, active() and inactive() look the same. In Windows
    2000 style and Macintosh Platinum style, the two styles look
    slightly different.

    There are setActive(), setInactive(), and setDisabled() functions
    to modify the palette. (TQt also supports a normal() group; this is
    an obsolete alias for active(), supported for backwards
    compatibility.)

    Colors and brushes can be set for particular roles in any of a
    palette's color groups with setColor() and setBrush().

    You can copy a palette using the copy constructor and test to see
    if two palettes are \e identical using isCopyOf().

    \sa TQApplication::setPalette(), TQWidget::setPalette(), TQColorGroup, TQColor
*/

/*!
    \enum TQPalette::ColorGroup

    \value Disabled
    \value Active
    \value Inactive
    \value NColorGroups
    \value Normal synonym for Active
*/

/*!
    \obsolete

    \fn const TQColorGroup &TQPalette::normal() const

    Returns the active color group. Use active() instead.

    \sa setActive() active()
*/

/*!
    \obsolete

    \fn void TQPalette::setNormal( const TQColorGroup & cg )

    Sets the active color group to \a cg. Use setActive() instead.

    \sa setActive() active()
*/


static int palette_count = 1;

/*!
    Constructs a palette that consists of color groups with only black
    colors.
*/

TQPalette::TQPalette()
{
    static TQPalData *defPalData = 0;
    if ( !defPalData ) {                // create common palette data
	defPalData = new TQPalData;      //   for the default palette
	static TQSharedCleanupHandler<TQPalData> defPalCleanup;
	defPalCleanup.set( &defPalData );
	defPalData->ser_no = palette_count++;
    }
    data = defPalData;
    data->ref();
}

/*!\obsolete
  Constructs a palette from the \a button color. The other colors are
  automatically calculated, based on this color. Background will be
  the button color as well.
*/

TQPalette::TQPalette( const TQColor &button )
{
    data = new TQPalData;
    TQ_CHECK_PTR( data );
    data->ser_no = palette_count++;
    TQColor bg = button, btn = button, fg, base, disfg;
    int h, s, v;
    bg.hsv( &h, &s, &v );
    if ( v > 128 ) {				// light background
	fg   = TQt::black;
	base = TQt::white;
	disfg = TQt::darkGray;
    } else {					// dark background
	fg   = TQt::white;
	base = TQt::black;
	disfg = TQt::darkGray;
    }
    data->active   = TQColorGroup( fg, btn, btn.light(150), btn.dark(),
				  btn.dark(150), fg, TQt::white, base, bg );
    data->disabled = TQColorGroup( disfg, btn, btn.light(150), btn.dark(),
				  btn.dark(150), disfg, TQt::white, base, bg );
    data->inactive = data->active;
}

/*!
    Constructs a palette from a \a button color and a \a background.
    The other colors are automatically calculated, based on these
    colors.
*/

TQPalette::TQPalette( const TQColor &button, const TQColor &background )
{
    data = new TQPalData;
    TQ_CHECK_PTR( data );
    data->ser_no = palette_count++;
    TQColor bg = background, btn = button, fg, base, disfg;
    int h, s, v;
    bg.hsv( &h, &s, &v );
    if ( v > 128 ) {				// light background
	fg   = TQt::black;
	base = TQt::white;
	disfg = TQt::darkGray;
    } else {					// dark background
	fg   = TQt::white;
	base = TQt::black;
	disfg = TQt::darkGray;
    }
    data->active   = TQColorGroup( fg, btn, btn.light(150), btn.dark(),
				  btn.dark(150), fg, TQt::white, base, bg );
    data->disabled = TQColorGroup( disfg, btn, btn.light(150), btn.dark(),
				  btn.dark(150), disfg, TQt::white, base, bg );
    data->inactive = data->active;
}

/*!
    Constructs a palette that consists of the three color groups \a
    active, \a disabled and \a inactive. See the \link #details
    Detailed Description\endlink for definitions of the color groups
    and \l TQColorGroup::ColorRole for definitions of each color role
    in the three groups.

    \sa TQColorGroup TQColorGroup::ColorRole TQPalette
*/

TQPalette::TQPalette( const TQColorGroup &active, const TQColorGroup &disabled,
		    const TQColorGroup &inactive )
{
    data = new TQPalData;
    TQ_CHECK_PTR( data );
    data->ser_no = palette_count++;
    data->active = active;
    data->disabled = disabled;
    data->inactive = inactive;
}

/*!
    Constructs a copy of \a p.

    This constructor is fast (it uses copy-on-write).
*/

TQPalette::TQPalette( const TQPalette &p )
{
    data = p.data;
    data->ref();
}

/*!
    Destroys the palette.
*/

TQPalette::~TQPalette()
{
    if ( data->deref() )
	delete data;
}

/*!
    Assigns \a p to this palette and returns a reference to this
    palette.

    This is fast (it uses copy-on-write).

    \sa copy()
*/

TQPalette &TQPalette::operator=( const TQPalette &p )
{
    p.data->ref();
    if ( data->deref() )
	delete data;
    data = p.data;
    return *this;
}


/*!
    Returns the color in color group \a gr, used for color role \a r.

    \sa brush() setColor() TQColorGroup::ColorRole
*/
const TQColor &TQPalette::color( ColorGroup gr, TQColorGroup::ColorRole r ) const
{
    return directBrush( gr, r ).color();
}

/*!
    Returns the brush in color group \a gr, used for color role \a r.

    \sa color() setBrush() TQColorGroup::ColorRole
*/
const TQBrush &TQPalette::brush( ColorGroup gr, TQColorGroup::ColorRole r ) const
{
    return directBrush( gr, r );
}

/*!
    Sets the brush in color group \a gr, used for color role \a r, to
    the solid color \a c.

    \sa setBrush() color() TQColorGroup::ColorRole
*/
void TQPalette::setColor( ColorGroup gr, TQColorGroup::ColorRole r,
			 const TQColor &c)
{
    setBrush( gr, r, TQBrush(c) );
}

/*!
    Sets the brush in color group \a gr, used for color role \a r, to
    \a b.

    \sa brush() setColor() TQColorGroup::ColorRole
*/
void TQPalette::setBrush( ColorGroup gr, TQColorGroup::ColorRole r,
			 const TQBrush &b)
{
    detach();
    data->ser_no = palette_count++;
    directSetBrush( gr, r, b);
}

/*!
    \overload

    Sets the brush color used for color role \a r to color \a c in all
    three color groups.

    \sa color() setBrush() TQColorGroup::ColorRole
*/
void TQPalette::setColor( TQColorGroup::ColorRole r, const TQColor &c )
{
    setBrush( r, TQBrush(c) );
}

/*!
    \overload

    Sets the brush in for color role \a r in all three color groups to
    \a b.

    \sa brush() setColor() TQColorGroup::ColorRole active() inactive() disabled()
*/
void TQPalette::setBrush( TQColorGroup::ColorRole r, const TQBrush &b )
{
    detach();
    data->ser_no = palette_count++;
    directSetBrush( Active, r, b );
    directSetBrush( Disabled, r, b );
    directSetBrush( Inactive, r, b );
}


/*!
    Returns a deep copy of this palette.

    \warning This is slower than the copy constructor and assignment
    operator and offers no benefits.
*/

TQPalette TQPalette::copy() const
{
    TQPalette p( data->active, data->disabled, data->inactive );
    return p;
}


/*!
    Detaches this palette from any other TQPalette objects with which
    it might implicitly share TQColorGroup objects. In essence, does
    the copying part of copy-on-write.

    Calling this should generally not be necessary; TQPalette calls it
    itself when necessary.
*/

void TQPalette::detach()
{
    if ( data->count != 1 )
	*this = copy();
}

/*!
    \fn const TQColorGroup & TQPalette::disabled() const

    Returns the disabled color group of this palette.

    \sa TQColorGroup, setDisabled(), active(), inactive()
*/

/*!
    Sets the \c Disabled color group to \a g.

    \sa disabled() setActive() setInactive()
*/

void TQPalette::setDisabled( const TQColorGroup &g )
{
    detach();
    data->ser_no = palette_count++;
    data->disabled = g;
}

/*!
    \fn const TQColorGroup & TQPalette::active() const

    Returns the active color group of this palette.

    \sa TQColorGroup, setActive(), inactive(), disabled()
*/

/*!
    Sets the \c Active color group to \a g.

    \sa active() setDisabled() setInactive() TQColorGroup
*/

void TQPalette::setActive( const TQColorGroup &g )
{
    detach();
    data->ser_no = palette_count++;
    data->active = g;
}

/*!
    \fn const TQColorGroup & TQPalette::inactive() const

    Returns the inactive color group of this palette.

    \sa TQColorGroup,  setInactive(), active(), disabled()
*/

/*!
    Sets the \c Inactive color group to \a g.

    \sa active() setDisabled() setActive() TQColorGroup
*/

void TQPalette::setInactive( const TQColorGroup &g )
{
    detach();
    data->ser_no = palette_count++;
    data->inactive = g;
}


/*!
    \fn bool TQPalette::operator!=( const TQPalette &p ) const

    Returns true (slowly) if this palette is different from \a p;
    otherwise returns false (usually quickly).
*/

/*!
    Returns true (usually quickly) if this palette is equal to \a p;
    otherwise returns false (slowly).
*/

bool TQPalette::operator==( const TQPalette &p ) const
{
    return data->active == p.data->active &&
	   data->disabled == p.data->disabled &&
	   data->inactive == p.data->inactive;
}


/*!
    \fn int TQPalette::serialNumber() const

    Returns a number that uniquely identifies this TQPalette object.
    The serial number is intended for caching. Its value may not be
    used for anything other than equality testing.

    Note that TQPalette uses copy-on-write, and the serial number
    changes during the lazy copy operation (detach()), not during a
    shallow copy (copy constructor or assignment).

    \sa TQPixmap TQPixmapCache TQCache
*/


/*****************************************************************************
  TQColorGroup/TQPalette stream functions
 *****************************************************************************/

#ifndef TQT_NO_DATASTREAM
/*!
    \relates TQColorGroup

    Writes color group, \a g to the stream \a s.

    \sa \link datastreamformat.html Format of the TQDataStream operators \endlink
*/

TQDataStream &operator<<( TQDataStream &s, const TQColorGroup &g )
{
    if ( s.version() == 1 ) {
	// TQt 1.x
	s << g.foreground()
	  << g.background()
	  << g.light()
	  << g.dark()
	  << g.mid()
	  << g.text()
	  << g.base();
    } else {
	int max = TQColorGroup::NColorRoles;
	if ( s.version() <= 3) // TQt 2.x
	    max = 14;

	for( int r = 0 ; r < max ; r++ )
	    s << g.brush( (TQColorGroup::ColorRole)r);
    }
    return s;
}

/*!
    \related TQColorGroup

    Reads a color group from the stream.

    \sa \link datastreamformat.html Format of the TQDataStream operators \endlink
*/

TQDataStream &operator>>( TQDataStream &s, TQColorGroup &g )
{
    if ( s.version() == 1 ) {
	// TQt 1.x
	TQColor fg, bg, light, dark, mid, text, base;
	s >> fg >> bg >> light >> dark >> mid >> text >> base;
	TQPalette p( bg );
	TQColorGroup n( p.active() );
	n.setColor( TQColorGroup::Foreground, fg );
	n.setColor( TQColorGroup::Light, light );
	n.setColor( TQColorGroup::Dark, dark );
	n.setColor( TQColorGroup::Mid, mid );
	n.setColor( TQColorGroup::Text, text );
	n.setColor( TQColorGroup::Base, base );
	g = n;
    } else {
	int max = TQColorGroup::NColorRoles;
	if (s.version() <= 3) // TQt 2.x
	    max = 14;

	TQBrush tmp;
	for( int r = 0 ; r < max; r++ ) {
	    s >> tmp;
	    g.setBrush( (TQColorGroup::ColorRole)r, tmp);
	}
    }
    return s;
}


/*!
    \relates TQPalette

    Writes the palette, \a p to the stream \a s and returns a
    reference to the stream.

    \sa \link datastreamformat.html Format of the TQDataStream operators \endlink
*/

TQDataStream &operator<<( TQDataStream &s, const TQPalette &p )
{
    return s << p.active()
	     << p.disabled()
	     << p.inactive();
}


static void readV1ColorGroup( TQDataStream &s, TQColorGroup &g,
		       TQPalette::ColorGroup r )
{
    TQColor fg, bg, light, dark, mid, text, base;
    s >> fg >> bg >> light >> dark >> mid >> text >> base;
    TQPalette p( bg );
    TQColorGroup n;
    switch ( r ) {
	case TQPalette::Disabled:
	    n = p.disabled();
	    break;
	case TQPalette::Inactive:
	    n = p.inactive();
	    break;
	default:
	    n = p.active();
	    break;
    }
    n.setColor( TQColorGroup::Foreground, fg );
    n.setColor( TQColorGroup::Light, light );
    n.setColor( TQColorGroup::Dark, dark );
    n.setColor( TQColorGroup::Mid, mid );
    n.setColor( TQColorGroup::Text, text );
    n.setColor( TQColorGroup::Base, base );
    g = n;
}


/*!
    \relates TQPalette

    Reads a palette from the stream, \a s into the palette \a p, and
    returns a reference to the stream.

    \sa \link datastreamformat.html Format of the TQDataStream operators \endlink
*/

TQDataStream &operator>>( TQDataStream &s, TQPalette &p )
{
    TQColorGroup active, disabled, inactive;
    if ( s.version() == 1 ) {
	readV1ColorGroup( s, active, TQPalette::Active );
	readV1ColorGroup( s, disabled, TQPalette::Disabled );
	readV1ColorGroup( s, inactive, TQPalette::Inactive );
    } else {
	s >> active >> disabled >> inactive;
    }
    TQPalette newpal( active, disabled, inactive );
    p = newpal;
    return s;
}
#endif //TQT_NO_DATASTREAM

/*!
    Returns true if this palette and \a p are copies of each other,
    i.e. one of them was created as a copy of the other and neither
    was subsequently modified; otherwise returns false. This is much
    stricter than equality.

    \sa operator=() operator==()
*/

bool TQPalette::isCopyOf( const TQPalette & p )
{
    return data && data == p.data;
}

const TQBrush &TQPalette::directBrush( ColorGroup gr, TQColorGroup::ColorRole r ) const
{
    if ( (uint)gr > (uint)TQPalette::NColorGroups ) {
#if defined(QT_CHECK_RANGE)
	tqWarning( "TQPalette::directBrush: colorGroup(%i) out of range", gr );
#endif
	return data->active.br[TQColorGroup::Foreground];
    }
    if ( (uint)r >= (uint)TQColorGroup::NColorRoles ) {
#if defined(QT_CHECK_RANGE)
	tqWarning( "TQPalette::directBrush: colorRole(%i) out of range", r );
#endif
	return data->active.br[TQColorGroup::Foreground];
    }
    switch( gr ) {
    case Active:
	return data->active.br[r];
	//break;
    case Disabled:
	return data->disabled.br[r];
	//break;
    case Inactive:
	return data->inactive.br[r];
	//break;
    default:
	break;
    }
#if defined(QT_CHECK_RANGE)
    tqWarning( "TQPalette::directBrush: colorGroup(%i) internal error", gr );
#endif
    return data->active.br[TQColorGroup::Foreground]; // Satisfy compiler
}

void TQPalette::directSetBrush( ColorGroup gr, TQColorGroup::ColorRole r, const TQBrush& b)
{
    if ( (uint)gr > (uint)TQPalette::NColorGroups ) {
#if defined(QT_CHECK_RANGE)
	tqWarning( "TQPalette::directBrush: colorGroup(%i) out of range", gr );
#endif
	return;
    }
    if ( (uint)r >= (uint)TQColorGroup::NColorRoles ) {
#if defined(QT_CHECK_RANGE)
	tqWarning( "TQPalette::directBrush: colorRole(%i) out of range", r );
#endif
	return;
    }
    switch( gr ) {
    case Active:
	data->active.setBrush(r,b);
	break;
    case Disabled:
	data->disabled.setBrush(r,b);
	break;
    case Inactive:
	data->inactive.setBrush(r,b);
	break;
    default:
#if defined(QT_CHECK_RANGE)
	tqWarning( "TQPalette::directBrush: colorGroup(%i) internal error", gr );
#endif
	break;
    }
}


/*!\internal*/
TQColorGroup::ColorRole TQPalette::foregroundRoleFromMode( TQt::BackgroundMode mode )
{
    switch (mode) {
    case TQt::PaletteButton:
	return TQColorGroup::ButtonText;
    case TQt::PaletteBase:
	return TQColorGroup::Text;
    case TQt::PaletteDark:
    case TQt::PaletteShadow:
	return TQColorGroup::Light;
    case TQt::PaletteHighlight:
	return TQColorGroup::HighlightedText;
    case TQt::PaletteBackground:
    default:
	return TQColorGroup::Foreground;
    }
}

/*!\internal*/
TQColorGroup::ColorRole TQPalette::backgroundRoleFromMode( TQt::BackgroundMode mode)
{
    switch (mode) {
    case TQt::PaletteForeground:
	return TQColorGroup::Foreground;
    case TQt::PaletteButton:
	return TQColorGroup::Button;
    case TQt::PaletteLight:
	return TQColorGroup::Light;
    case TQt::PaletteMidlight:
	return TQColorGroup::Midlight;
    case TQt::PaletteDark:
	return TQColorGroup::Dark;
    case TQt::PaletteMid:
	return TQColorGroup::Mid;
    case TQt::PaletteText:
	return TQColorGroup::Text;
    case TQt::PaletteBrightText:
	return TQColorGroup::BrightText;
    case TQt::PaletteButtonText:
	return TQColorGroup::ButtonText;
    case TQt::PaletteBase:
	return TQColorGroup::Base;
    case TQt::PaletteShadow:
	return TQColorGroup::Shadow;
    case TQt::PaletteHighlight:
	return TQColorGroup::Highlight;
    case TQt::PaletteHighlightedText:
	return TQColorGroup::HighlightedText;
    case TQt::PaletteLink:
	return TQColorGroup::Link;
    case TQt::PaletteLinkVisited:
	return TQColorGroup::LinkVisited;
    case TQt::PaletteBackground:
    default:
	return TQColorGroup::Background;
    }
}

#endif // TQT_NO_PALETTE
