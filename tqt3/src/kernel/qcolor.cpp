/****************************************************************************
**
** Implementation of TQColor class
**
** Created : 940112
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

#include "ntqcolor.h"
#include "ntqnamespace.h"
#include "ntqdatastream.h"

#include <stdio.h>


/*!
    \class TQColor ntqcolor.h
    \brief The TQColor class provides colors based on RGB or HSV values.

    \ingroup images
    \ingroup graphics
    \ingroup appearance

    A color is normally specified in terms of RGB (red, green and blue)
    components, but it is also possible to specify HSV (hue, saturation
    and value) or set a color name (the names are copied from from the
    X11 color database).

    In addition to the RGB value, a TQColor also has a pixel value and a
    validity. The pixel value is used by the underlying window system
    to refer to a color. It can be thought of as an index into the
    display hardware's color table.

    The validity (isValid()) indicates whether the color is legal at
    all. For example, a RGB color with RGB values out of range is
    illegal. For performance reasons, TQColor mostly disregards illegal
    colors. The result of using an invalid color is unspecified and
    will usually be surprising.

    There are 19 predefined TQColor objects: \c white, \c black, \c
    red, \c darkRed, \c green, \c darkGreen, \c blue, \c darkBlue, \c
    cyan, \c darkCyan, \c magenta, \c darkMagenta, \c yellow, \c
    darkYellow, \c gray, \c darkGray, \c lightGray, \c color0 and \c
    color1, accessible as members of the TQt namespace (ie. \c TQt::red).

    \img qt-colors.png TQt Colors

    The colors \c color0 (zero pixel value) and \c color1 (non-zero
    pixel value) are special colors for drawing in \link TQBitmap
    bitmaps\endlink. Painting with \c color0 sets the bitmap bits to 0
    (transparent, i.e. background), and painting with \c color1 sets the
    bits to 1 (opaque, i.e. foreground).

    The TQColor class has an efficient, dynamic color allocation
    strategy. A color is normally allocated the first time it is used
    (lazy allocation), that is, whenever the pixel() function is called.
    The following steps are taken to allocate a color. If, at any point,
    a suitable color is found then the appropriate pixel value is
    returned and the subsequent steps are not taken:

    \list 1
    \i Is the pixel value valid? If it is, just return it; otherwise,
    allocate a pixel value.
    \i Check an internal hash table to see if we allocated an equal RGB
    value earlier. If we did, set the corresponding pixel value for the
    color and return it.
    \i Try to allocate the RGB value. If we succeed, we get a pixel value
    that we save in the internal table with the RGB value.
    Return the pixel value.
    \i The color could not be allocated. Find the closest matching
    color, save it in the internal table, and return it.
    \endlist

    A color can be set by passing setNamedColor() an RGB string like
    "#112233", or a color name, e.g. "blue". The names are taken from
    X11's rgb.txt database but can also be used under Windows. To get
    a lighter or darker color use light() and dark() respectively.
    Colors can also be set using setRgb() and setHsv(). The color
    components can be accessed in one go with rgb() and hsv(), or
    individually with red(), green() and blue().

    Use maxColors() and numBitPlanes() to determine the maximum number
    of colors and the number of bit planes supported by the underlying
    window system,

    If you need to allocate many colors temporarily, for example in an
    image viewer application, enterAllocContext(), leaveAllocContext() and
    destroyAllocContext() will prove useful.

    \section1 HSV Colors

    Because many people don't know the HSV color model very well, we'll
    cover it briefly here.

    The RGB model is hardware-oriented. Its representation is close to
    what most monitors show. In contrast, HSV represents color in a way
    more suited to the human perception of color. For example, the
    relationships "stronger than", "darker than" and "the opposite of"
    are easily expressed in HSV but are much harder to express in RGB.

    HSV, like RGB, has three components:

    \list

    \i H, for hue, is either 0-359 if the color is chromatic (not
    gray), or meaningless if it is gray. It represents degrees on the
    color wheel familiar to most people. Red is 0 (degrees), green is
    120 and blue is 240.

    \i S, for saturation, is 0-255, and the bigger it is, the
    stronger the color is. Grayish colors have saturation near 0; very
    strong colors have saturation near 255.

    \i V, for value, is 0-255 and represents lightness or brightness
    of the color. 0 is black; 255 is as far from black as possible.

    \endlist

    Here are some examples: Pure red is H=0, S=255, V=255. A dark red,
    moving slightly towards the magenta, could be H=350 (equivalent to
    -10), S=255, V=180. A grayish light red could have H about 0 (say
    350-359 or 0-10), S about 50-100, and S=255.

    TQt returns a hue value of -1 for achromatic colors. If you pass a
    too-big hue value, TQt forces it into range. Hue 360 or 720 is
    treated as 0; hue 540 is treated as 180.

    \sa TQPalette, TQColorGroup, TQApplication::setColorSpec(),
    \link http://www.poynton.com/ColorFAQ.html Color FAQ\endlink
*/

/*****************************************************************************
  Global colors
 *****************************************************************************/

#if defined(TQ_WS_WIN)
#define COLOR0_PIX 0x00ffffff
#define COLOR1_PIX 0
#else
#define COLOR0_PIX 0
#define COLOR1_PIX 1
#endif

#if (defined(Q_CC_GNU) && defined(Q_OS_WIN))
// workaround - bug in mingw
static TQColor stdcol[19] = {
    TQColor( 255, 255, 255 ),
    TQColor(   0,   0,   0 ),
    TQColor(   0,   0,   0 ),
    TQColor( 255, 255, 255 ),
    TQColor( 128, 128, 128 ),
    TQColor( 160, 160, 164 ),
    TQColor( 192, 192, 192 ),
    TQColor( 255,   0,   0 ),
    TQColor(   0, 255,   0 ),
    TQColor(   0,   0, 255 ),
    TQColor(   0, 255, 255 ),
    TQColor( 255,   0, 255 ),
    TQColor( 255, 255,   0 ),
    TQColor( 128,   0,   0 ),
    TQColor(   0, 128,   0 ),
    TQColor(   0,   0, 128 ),
    TQColor(   0, 128, 128 ),
    TQColor( 128,   0, 128 ),
    TQColor( 128, 128,   0 ) };
#else
    static TQColor stdcol[19];
#endif

const TQColor & TQt::color0 = stdcol[0];
const TQColor & TQt::color1  = stdcol[1];
const TQColor & TQt::black  = stdcol[2];
const TQColor & TQt::white = stdcol[3];
const TQColor & TQt::darkGray = stdcol[4];
const TQColor & TQt::gray = stdcol[5];
const TQColor & TQt::lightGray = stdcol[6];
const TQColor & TQt::red = stdcol[7];
const TQColor & TQt::green = stdcol[8];
const TQColor & TQt::blue = stdcol[9];
const TQColor & TQt::cyan = stdcol[10];
const TQColor & TQt::magenta = stdcol[11];
const TQColor & TQt::yellow = stdcol[12];
const TQColor & TQt::darkRed = stdcol[13];
const TQColor & TQt::darkGreen = stdcol[14];
const TQColor & TQt::darkBlue = stdcol[15];
const TQColor & TQt::darkCyan = stdcol[16];
const TQColor & TQt::darkMagenta = stdcol[17];
const TQColor & TQt::darkYellow = stdcol[18];


/*****************************************************************************
  TQColor member functions
 *****************************************************************************/

bool TQColor::color_init   = false;		// color system not initialized
bool TQColor::globals_init = false;		// global color not initialized
TQColor::ColorModel TQColor::colormodel = d32;


TQColor* TQColor::globalColors()
{
    return stdcol;
}


/*!
    Initializes the global colors. This function is called if a global
    color variable is initialized before the constructors for our
    global color objects are executed. Without this mechanism,
    assigning a color might assign an uninitialized value.

    Example:
    \code
	TQColor myColor = red;              // will initialize red etc.

	int main( int argc, char **argc )
	{
	}
    \endcode
*/

void TQColor::initGlobalColors()
{
    globals_init = true;

    #ifdef TQ_WS_X11
    // HACK: we need a way to recognize color0 and color1 uniquely, so
    // that we can use color0 and color1 with fixed pixel values on
    // all screens
    stdcol[ 0].d.argb = tqRgba(255, 255, 255, 1);
    stdcol[ 1].d.argb = tqRgba(  0,   0,   0, 1);
    #else
    stdcol[ 0].d.argb = tqRgb(255,255,255);
    stdcol[ 1].d.argb = 0;
    #endif // TQ_WS_X11
    stdcol[ 0].setPixel( COLOR0_PIX );
    stdcol[ 1].setPixel( COLOR1_PIX );

    // From the "The Palette Manager: How and Why" by Ron Gery, March 23,
    // 1992, archived on MSDN:
    // 	The Windows system palette is broken up into two sections,
    // 	one with fixed colors and one with colors that can be changed
    // 	by applications. The system palette predefines 20 entries;
    // 	these colors are known as the static or reserved colors and
    // 	consist of the 16 colors found in the Windows version 3.0 VGA
    // 	driver and 4 additional colors chosen for their visual appeal.
    // 	The DEFAULT_PALETTE stock object is, as the name implies, the
    // 	default palette selected into a device context (DC) and consists
    // 	of these static colors. Applications can set the remaining 236
    // 	colors using the Palette Manager.
    // The 20 reserved entries have indices in [0,9] and [246,255]. We
    // reuse 17 of them.
    stdcol[ 2].setRgb(   0,   0,   0 );   // index 0     black
    stdcol[ 3].setRgb( 255, 255, 255 );   // index 255   white
    stdcol[ 4].setRgb( 128, 128, 128 );   // index 248   medium gray
    stdcol[ 5].setRgb( 160, 160, 164 );   // index 247   light gray
    stdcol[ 6].setRgb( 192, 192, 192 );   // index 7     light gray
    stdcol[ 7].setRgb( 255,   0,   0 );   // index 249   red
    stdcol[ 8].setRgb(   0, 255,   0 );   // index 250   green
    stdcol[ 9].setRgb(   0,   0, 255 );   // index 252   blue
    stdcol[10].setRgb(   0, 255, 255 );   // index 254   cyan
    stdcol[11].setRgb( 255,   0, 255 );   // index 253   magenta
    stdcol[12].setRgb( 255, 255,   0 );   // index 251   yellow
    stdcol[13].setRgb( 128,   0,   0 );   // index 1     dark red
    stdcol[14].setRgb(   0, 128,   0 );   // index 2     dark green
    stdcol[15].setRgb(   0,   0, 128 );   // index 4     dark blue
    stdcol[16].setRgb(   0, 128, 128 );   // index 6     dark cyan
    stdcol[17].setRgb( 128,   0, 128 );   // index 5     dark magenta
    stdcol[18].setRgb( 128, 128,   0 );   // index 3     dark yellow
}

/*!
    \enum TQColor::Spec

    The type of color specified, either RGB or HSV, e.g. in the
    \c{TQColor::TQColor( x, y, z, colorSpec)} constructor.

    \value Rgb
    \value Hsv
*/


/*!
    \fn TQColor::TQColor()

    Constructs an invalid color with the RGB value (0, 0, 0). An
    invalid color is a color that is not properly set up for the
    underlying window system.

    The alpha value of an invalid color is unspecified.

    \sa isValid()
*/


/*!
    \fn TQColor::TQColor( int r, int g, int b )

    Constructs a color with the RGB value \a r, \a g, \a b, in the
    same way as setRgb().

    The color is left invalid if any or the arguments are illegal.

    \sa setRgb()
*/


/*!
    Constructs a color with the RGB value \a rgb and a custom pixel
    value \a pixel.

    If \a pixel == 0xffffffff (the default), then the color uses the
    RGB value in a standard way. If \a pixel is something else, then
    the pixel value is set directly to \a pixel, skipping the normal
    allocation procedure.
*/

TQColor::TQColor( TQRgb rgb, uint pixel )
{
    if ( pixel == 0xffffffff ) {
	setRgb( rgb );
    } else {
	d.argb = rgb;
	setPixel( pixel );
    }
}

void TQColor::setPixel( uint pixel )
{
    switch ( colormodel ) {
    case d8:
	d.d8.direct = true;
	d.d8.invalid = false;
	d.d8.dirty = false;
	d.d8.pix = pixel;
	break;
    case d32:
	d.d32.pix = pixel;
	break;
    }
}


/*!
    Constructs a color with the RGB or HSV value \a x, \a y, \a z.

    The arguments are an RGB value if \a colorSpec is TQColor::Rgb. \a
    x (red), \a y (green), and \a z (blue). All of them must be in the
    range 0-255.

    The arguments are an HSV value if \a colorSpec is TQColor::Hsv. \a
    x (hue) must be -1 for achromatic colors and 0-359 for chromatic
    colors; \a y (saturation) and \a z (value) must both be in the
    range 0-255.

    \sa setRgb(), setHsv()
*/

TQColor::TQColor( int x, int y, int z, Spec colorSpec )
{
    d.d32.argb = Invalid;
    d.d32.pix = Dirt;
    if ( colorSpec == Hsv )
	setHsv( x, y, z );
    else
	setRgb( x, y, z );
}


/*!
    Constructs a named color in the same way as setNamedColor() using
    name \a name.

    The color is left invalid if \a name cannot be parsed.

    \sa setNamedColor()
*/

TQColor::TQColor( const TQString& name )
{
    setNamedColor( name );
}


/*!
    Constructs a named color in the same way as setNamedColor() using
    name \a name.

    The color is left invalid if \a name cannot be parsed.

    \sa setNamedColor()
*/

TQColor::TQColor( const char *name )
{
    setNamedColor( TQString(name) );
}



/*!
    Constructs a color that is a copy of \a c.
*/

TQColor::TQColor( const TQColor &c )
{
    if ( !globals_init )
	initGlobalColors();
    d.argb = c.d.argb;
    d.d32.pix = c.d.d32.pix;
}


/*!
    Assigns a copy of the color \a c and returns a reference to this
    color.
*/

TQColor &TQColor::operator=( const TQColor &c )
{
    if ( !globals_init )
	initGlobalColors();
    d.argb = c.d.argb;
    d.d32.pix = c.d.d32.pix;
    return *this;
}


/*!
    \fn bool TQColor::isValid() const

    Returns false if the color is invalid, i.e. it was constructed using the
    default constructor; otherwise returns true.
*/

/*!
    \internal
*/
bool TQColor::isDirty() const
{
    if ( colormodel == d8 ) {
	return d.d8.dirty;
    } else {
	return d.d32.probablyDirty();
    }
}

/*!
    Returns the name of the color in the format "#RRGGBB", i.e. a "#"
    character followed by three two-digit hexadecimal numbers.

    \sa setNamedColor()
*/

TQString TQColor::name() const
{
#ifndef TQT_NO_SPRINTF
    TQString s;
    s.sprintf( "#%02x%02x%02x", red(), green(), blue() );
    return s;
#else
    char s[20];
    sprintf( s, "#%02x%02x%02x", red(), green(), blue() );
    return TQString(s);
#endif
}

static int hex2int( TQChar hexchar )
{
    int v;
    if ( hexchar.isDigit() )
	v = hexchar.digitValue();
    else if ( hexchar >= 'A' && hexchar <= 'F' )
	v = hexchar.cell() - 'A' + 10;
    else if ( hexchar >= 'a' && hexchar <= 'f' )
	v = hexchar.cell() - 'a' + 10;
    else
	v = -1;
    return v;
}


/*!
    Sets the RGB value to \a name, which may be in one of these
    formats:
    \list
    \i #RGB (each of R, G and B is a single hex digit)
    \i #RRGGBB
    \i #RRRGGGBBB
    \i #RRRRGGGGBBBB
    \i A name from the X color database (rgb.txt) (e.g.
    "steelblue" or "gainsboro"). These color names also work
    under Windows.
    \endlist

    The color is invalid if \a name cannot be parsed.
*/

void TQColor::setNamedColor( const TQString &name )
{
    if ( name.isEmpty() ) {
	d.argb = 0;
	if ( colormodel == d8 ) {
	    d.d8.invalid = true;
	} else {
	    d.d32.argb = Invalid;
	}
    } else if ( name[0] == '#' ) {
	const TQChar *p = name.unicode()+1;
	int len = name.length()-1;
	int r, g, b;
	if ( len == 12 ) {
	    r = (hex2int(p[0]) << 4) + hex2int(p[1]);
	    g = (hex2int(p[4]) << 4) + hex2int(p[5]);
	    b = (hex2int(p[8]) << 4) + hex2int(p[9]);
	} else if ( len == 9 ) {
	    r = (hex2int(p[0]) << 4) + hex2int(p[1]);
	    g = (hex2int(p[3]) << 4) + hex2int(p[4]);
	    b = (hex2int(p[6]) << 4) + hex2int(p[7]);
	} else if ( len == 6 ) {
	    r = (hex2int(p[0]) << 4) + hex2int(p[1]);
	    g = (hex2int(p[2]) << 4) + hex2int(p[3]);
	    b = (hex2int(p[4]) << 4) + hex2int(p[5]);
	} else if ( len == 3 ) {
	    r = (hex2int(p[0]) << 4) + hex2int(p[0]);
	    g = (hex2int(p[1]) << 4) + hex2int(p[1]);
	    b = (hex2int(p[2]) << 4) + hex2int(p[2]);
	} else {
	    r = g = b = -1;
	}
	if ( (uint)r > 255 || (uint)g > 255 || (uint)b > 255 ) {
	    d.d32.argb = Invalid;
	    d.d32.pix = Dirt;
#if defined(QT_CHECK_RANGE)
	    tqWarning( "TQColor::setNamedColor: could not parse color '%s'",
		      name.local8Bit().data() );
#endif
	} else {
	    setRgb( r, g, b );
	}
    } else {
	setSystemNamedColor( name );
    }
}


#undef max
#undef min

/*!
  \fn void TQColor::getHsv( int &h, int &s, int &v ) const
  \obsolete
*/

/*!  \fn void TQColor::getHsv( int *h, int *s, int *v ) const

    Returns the current RGB value as HSV. The contents of the \a h, \a
    s and \a v pointers are set to the HSV values. If any of the three
    pointers are null, the function does nothing.

    The hue (which \a h points to) is set to -1 if the color is
    achromatic.

    \warning Colors are stored internally as RGB values, so getHSv()
    may return slightly different values to those set by setHsv().

    \sa setHsv(), rgb()
*/

/*! \obsolete Use getHsv() instead.
 */
void TQColor::hsv( int *h, int *s, int *v ) const
{
    if ( !h || !s || !v )
	return;
    int r = tqRed(d.argb);
    int g = tqGreen(d.argb);
    int b = tqBlue(d.argb);
    uint max = r;				// maximum RGB component
    int whatmax = 0;				// r=>0, g=>1, b=>2
    if ( (uint)g > max ) {
	max = g;
	whatmax = 1;
    }
    if ( (uint)b > max ) {
	max = b;
	whatmax = 2;
    }
    uint min = r;				// find minimum value
    if ( (uint)g < min ) min = g;
    if ( (uint)b < min ) min = b;
    int delta = max-min;
    *v = max;					// calc value
    *s = max ? (510*delta+max)/(2*max) : 0;
    if ( *s == 0 ) {
	*h = -1;				// undefined hue
    } else {
	switch ( whatmax ) {
	    case 0:				// red is max component
		if ( g >= b )
		    *h = (120*(g-b)+delta)/(2*delta);
		else
		    *h = (120*(g-b+delta)+delta)/(2*delta) + 300;
		break;
	    case 1:				// green is max component
		if ( b > r )
		    *h = 120 + (120*(b-r)+delta)/(2*delta);
		else
		    *h = 60 + (120*(b-r+delta)+delta)/(2*delta);
		break;
	    case 2:				// blue is max component
		if ( r > g )
		    *h = 240 + (120*(r-g)+delta)/(2*delta);
		else
		    *h = 180 + (120*(r-g+delta)+delta)/(2*delta);
		break;
	}
    }
}


/*!
    Sets a HSV color value. \a h is the hue, \a s is the saturation
    and \a v is the value of the HSV color.

    If \a s or \a v are not in the range 0-255, or \a h is < -1, the
    color is not changed.

    \warning Colors are stored internally as RGB values, so getHSv()
    may return slightly different values to those set by setHsv().

    \sa hsv(), setRgb()
*/

void TQColor::setHsv( int h, int s, int v )
{
    if ( h < -1 || (uint)s > 255 || (uint)v > 255 ) {
#if defined(QT_CHECK_RANGE)
	tqWarning( "TQColor::setHsv: HSV parameters out of range" );
#endif
	return;
    }
    int r=v, g=v, b=v;
    if ( s == 0 || h == -1 ) {			// achromatic case
	// Ignore
    } else {					// chromatic case
	if ( (uint)h >= 360 )
	    h %= 360;
	uint f = h%60;
	h /= 60;
	uint p = (uint)(2*v*(255-s)+255)/510;
	uint q, t;
	if ( h&1 ) {
	    q = (uint)(2*v*(15300-s*f)+15300)/30600;
	    switch( h ) {
		case 1: r=(int)q; g=(int)v, b=(int)p; break;
		case 3: r=(int)p; g=(int)q, b=(int)v; break;
		case 5: r=(int)v; g=(int)p, b=(int)q; break;
	    }
	} else {
	    t = (uint)(2*v*(15300-(s*(60-f)))+15300)/30600;
	    switch( h ) {
		case 0: r=(int)v; g=(int)t, b=(int)p; break;
		case 2: r=(int)p; g=(int)v, b=(int)t; break;
		case 4: r=(int)t; g=(int)p, b=(int)v; break;
	    }
	}
    }
    setRgb( r, g, b );
}


/*!
    \fn TQRgb TQColor::rgb() const

    Returns the RGB value.

    The return type \e TQRgb is equivalent to \c unsigned \c int.

    For an invalid color, the alpha value of the returned color is
    unspecified.

    \sa setRgb(), hsv(), tqRed(), tqBlue(), tqGreen(), isValid()
*/

/*! \fn void TQColor::getRgb( int *r, int *g, int *b ) const

    Sets the contents pointed to by \a r, \a g and \a b to the red,
    green and blue components of the RGB value respectively. The value
    range for a component is 0..255.

    \sa rgb(), setRgb(), getHsv()
*/

/*! \obsolete Use getRgb() instead */
void TQColor::rgb( int *r, int *g, int *b ) const
{
    *r = tqRed(d.argb);
    *g = tqGreen(d.argb);
    *b = tqBlue(d.argb);
}


/*!
    Sets the RGB value to \a r, \a g, \a b. The arguments, \a r, \a g
    and \a b must all be in the range 0..255. If any of them are
    outside the legal range, the color is not changed.

    \sa rgb(), setHsv()
*/

void TQColor::setRgb( int r, int g, int b )
{
    if ( (uint)r > 255 || (uint)g > 255 || (uint)b > 255 ) {
#if defined(QT_CHECK_RANGE)
	tqWarning( "TQColor::setRgb: RGB parameter(s) out of range" );
#endif
	return;
    }
    d.argb = tqRgb( r, g, b );
    if ( colormodel == d8 ) {
	d.d8.invalid = false;
	d.d8.direct = false;
	d.d8.dirty = true;
    } else {
	d.d32.pix = Dirt;
    }
}


/*!
    \overload
    Sets the RGB value to \a rgb.

    The type \e TQRgb is equivalent to \c unsigned \c int.

    \sa rgb(), setHsv()
*/

void TQColor::setRgb( TQRgb rgb )
{
    d.argb = rgb;
    if ( colormodel == d8 ) {
	d.d8.invalid = false;
	d.d8.direct = false;
	d.d8.dirty = true;
    } else {
	d.d32.pix = Dirt;
    }
}

/*!
    \fn int TQColor::red() const

    Returns the R (red) component of the RGB value.
*/


/*!
    \fn int TQColor::green() const

    Returns the G (green) component of the RGB value.
*/

/*!
    \fn int TQColor::blue() const

    Returns the B (blue) component of the RGB value.
*/


/*!
    Returns a lighter (or darker) color, but does not change this
    object.

    Returns a lighter color if \a factor is greater than 100. Setting
    \a factor to 150 returns a color that is 50% brighter.

    Returns a darker color if \a factor is less than 100. We recommend
    using dark() for this purpose. If \a factor is 0 or negative, the
    return value is unspecified.

    (This function converts the current RGB color to HSV, multiplies V
    by \a factor, and converts the result back to RGB.)

    \sa dark()
*/

TQColor TQColor::light( int factor ) const
{
    if ( factor <= 0 )				// invalid lightness factor
	return *this;
    else if ( factor < 100 )			// makes color darker
	return dark( 10000/factor );

    int h, s, v;
    hsv( &h, &s, &v );
    v = (factor*v)/100;
    if ( v > 255 ) {				// overflow
	s -= v-255;				// adjust saturation
	if ( s < 0 )
	    s = 0;
	v = 255;
    }
    TQColor c;
    c.setHsv( h, s, v );
    return c;
}


/*!
    Returns a darker (or lighter) color, but does not change this
    object.

    Returns a darker color if \a factor is greater than 100. Setting
    \a factor to 300 returns a color that has one-third the
    brightness.

    Returns a lighter color if \a factor is less than 100. We
    recommend using lighter() for this purpose. If \a factor is 0 or
    negative, the return value is unspecified.

    (This function converts the current RGB color to HSV, divides V by
    \a factor and converts back to RGB.)

    \sa light()
*/

TQColor TQColor::dark( int factor ) const
{
    if ( factor <= 0 )				// invalid darkness factor
	return *this;
    else if ( factor < 100 )			// makes color lighter
	return light( 10000/factor );
    int h, s, v;
    hsv( &h, &s, &v );
    v = (v*100)/factor;
    TQColor c;
    c.setHsv( h, s, v );
    return c;
}


/*!
    \fn bool TQColor::operator==( const TQColor &c ) const

    Returns true if this color has the same RGB value as \a c;
    otherwise returns false.
*/

/*!
    \fn bool TQColor::operator!=( const TQColor &c ) const
    Returns true if this color has a different RGB value from \a c;
    otherwise returns false.
*/

/*!
    Returns the pixel value.

    This value is used by the underlying window system to refer to a
    color. It can be thought of as an index into the display
    hardware's color table, but the value is an arbitrary 32-bit
    value.

    \sa alloc()
*/
uint TQColor::pixel() const
{
    if ( isDirty() )
	return ((TQColor*)this)->alloc();
    else if ( colormodel == d8 )
#ifdef TQ_WS_WIN
	// since d.d8.pix is uchar we have to use the PALETTEINDEX
	// macro to get the respective palette entry index.
	return (0x01000000 | (int)(short)(d.d8.pix));
#else
	return d.d8.pix;
#endif
    else
	return d.d32.pix;
}

/*!
    \fn TQStringList TQColor::colorNames()
    Returns a TQStringList containing the color names TQt knows about.
*/

/*****************************************************************************
  TQColor stream functions
 *****************************************************************************/
#ifndef TQT_NO_DATASTREAM
/*!
    \relates TQColor
    Writes a color object, \a c to the stream, \a s.

    \sa \link datastreamformat.html Format of the TQDataStream operators \endlink
*/

TQDataStream &operator<<( TQDataStream &s, const TQColor &c )
{
    TQ_UINT32 p = (TQ_UINT32)c.rgb();
    if ( s.version() == 1 )			// Swap red and blue
	p = ((p << 16) & 0xff0000) | ((p >> 16) & 0xff) | (p & 0xff00ff00);
    return s << p;
}

/*!
    \relates TQColor
    Reads a color object, \a c, from the stream, \a s.

    \sa \link datastreamformat.html Format of the TQDataStream operators \endlink
*/

TQDataStream &operator>>( TQDataStream &s, TQColor &c )
{
    TQ_UINT32 p;
    s >> p;
    if ( s.version() == 1 )			// Swap red and blue
	p = ((p << 16) & 0xff0000) | ((p >> 16) & 0xff) | (p & 0xff00ff00);
    c.setRgb( p );
    return s;
}
#endif

/*****************************************************************************
  TQColor global functions (documentation only)
 *****************************************************************************/

/*!
    \fn int tqRed( TQRgb rgb )
    \relates TQColor

    Returns the red component of the RGB triplet \a rgb.
    \sa tqRgb(), TQColor::red()
*/

/*!
    \fn int tqGreen( TQRgb rgb )
    \relates TQColor

    Returns the green component of the RGB triplet \a rgb.
    \sa tqRgb(), TQColor::green()
*/

/*!
    \fn int tqBlue( TQRgb rgb )
    \relates TQColor

    Returns the blue component of the RGB triplet \a rgb.
    \sa tqRgb(), TQColor::blue()
*/

/*!
    \fn int tqAlpha( TQRgb rgba )
    \relates TQColor

    Returns the alpha component of the RGBA quadruplet \a rgba.
    */

/*!
    \fn TQRgb tqRgb( int r, int g, int b )
    \relates TQColor

    Returns the RGB triplet \a (r,g,b).

    The return type TQRgb is equivalent to \c unsigned \c int.

    \sa tqRgba(), tqRed(), tqGreen(), tqBlue()
*/

/*!
    \fn TQRgb tqRgba( int r, int g, int b, int a )
    \relates TQColor

    Returns the RGBA quadruplet \a (r,g,b,a).

    The return type TQRgba is equivalent to \c unsigned \c int.

    \sa tqRgb(), tqRed(), tqGreen(), tqBlue()
*/

/*!
    \fn int tqGray( int r, int g, int b )
    \relates TQColor

    Returns a gray value 0..255 from the (\a r, \a g, \a b) triplet.

    The gray value is calculated using the formula (r*11 + g*16 +
    b*5)/32.
*/

/*!
    \overload int tqGray( tqRgb rgb )
    \relates TQColor

    Returns a gray value 0..255 from the given \a rgb colour.
*/

