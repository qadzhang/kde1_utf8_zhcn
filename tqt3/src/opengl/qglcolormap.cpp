/****************************************************************************
**
** Implementation of TQGLColormap class
**
** Created : 20010326
**
** Copyright (C) 1992-2008 Trolltech ASA.  All rights reserved.
**
** This file is part of the opengl module of the TQt GUI Toolkit.
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

/*!
    \class TQGLColormap ntqglcolormap.h
    \brief The TQGLColormap class is used for installing custom colormaps into
    TQGLWidgets.
\if defined(commercial)
    It is part of the <a href="commercialeditions.html">TQt Enterprise Edition</a>.
\endif

    \module OpenGL
    \ingroup graphics
    \ingroup images

    TQGLColormap provides a platform independent way of specifying and
    installing indexed colormaps into TQGLWidgets. TQGLColormap is
    especially useful when using the \link opengl.html OpenGL\endlink
    color-index mode.

    Under X11 you must use an X server that supports either a \c
    PseudoColor or \c DirectColor visual class. If your X server
    currently only provides a \c GrayScale, \c TrueColor, \c
    StaticColor or \c StaticGray visual, you will not be able to
    allocate colorcells for writing. If this is the case, try setting
    your X server to 8 bit mode. It should then provide you with at
    least a \c PseudoColor visual. Note that you may experience
    colormap flashing if your X server is running in 8 bit mode.

    Under Windows the size of the colormap is always set to 256
    colors. Note that under Windows you can also install colormaps
    in child widgets.

    This class uses explicit sharing (see \link shclass.html Shared
    Classes\endlink).

    Example of use:
    \code
    #include <ntqapplication.h>
    #include <ntqglcolormap.h>

    int main()
    {
	TQApplication a( argc, argv );

	MySuperGLWidget widget( 0 ); // A TQGLWidget in color-index mode
	TQGLColormap colormap;

	// This will fill the colormap with colors ranging from
	// black to white.
	for ( int i = 0; i < colormap.size(); i++ )
	    colormap.setEntry( i, tqRgb( i, i, i ) );

	widget.setColormap( colormap );
	widget.show();
	return a.exec();
    }
    \endcode

    \sa TQGLWidget::setColormap(), TQGLWidget::colormap()
*/

#include "ntqglcolormap.h"
#include "ntqmemarray.h"


/*!
    Construct a TQGLColormap.
*/
TQGLColormap::TQGLColormap()
{
    d = 0;
}


/*!
    Construct a shallow copy of \a map.
*/
TQGLColormap::TQGLColormap( const TQGLColormap & map )
{
    d = map.d;
    if ( d )
	d->ref();
}

/*!
    Dereferences the TQGLColormap and deletes it if this was the last
    reference to it.
*/
TQGLColormap::~TQGLColormap()
{
    if ( d && d->deref() ) {
	delete d;
	d = 0;
    }
}

/*!
    Assign a shallow copy of \a map to this TQGLColormap.
*/
TQGLColormap & TQGLColormap::operator=( const TQGLColormap & map )
{
    if ( map.d != 0 )
	map.d->ref();

    if ( d && d->deref() )
	delete d;
    d = map.d;

    return *this;
}

/*!
    Detaches this TQGLColormap from the shared block.
*/
void TQGLColormap::detach()
{
    if ( d && d->count != 1 ) {
	// ### What about the actual colormap handle?
	Private * newd = new Private();
	newd->cells = d->cells;
	newd->cells.detach();
	if ( d->deref() )
	    delete d;
	d = newd;
    }
}

/*!
    Set cell at index \a idx in the colormap to color \a color.
*/
void TQGLColormap::setEntry( int idx, TQRgb color )
{
    if ( !d )
	d = new Private();

#if defined(QT_CHECK_RANGE)
    if ( idx < 0 || idx > (int) d->cells.size() ) {
	tqWarning( "TQGLColormap::setRgb: Index out of range." );
	return;
    }
#endif
    d->cells[ idx ] = color;
}

/*!
    Set an array of cells in this colormap. \a count is the number of
    colors that should be set, \a colors is the array of colors, and
    \a base is the starting index.
*/
void TQGLColormap::setEntries( int count, const TQRgb * colors, int base )
{
    if ( !d )
	d = new Private();

    if ( !colors || base < 0 || base >= (int) d->cells.size() )
	return;

    for( int i = base; i < base + count; i++ ) {
	if ( i < (int) d->cells.size() )
	    setEntry( i, colors[i] );
	else
	    break;
    }
}

/*!
    Returns the TQRgb value in the colorcell with index \a idx.
*/
TQRgb TQGLColormap::entryRgb( int idx ) const
{
    if ( !d || idx < 0 || idx > (int) d->cells.size() )
	return 0;
    else
	return d->cells[ idx ];
}

/*!
    \overload

    Set the cell with index \a idx in the colormap to color \a color.
*/
void TQGLColormap::setEntry( int idx, const TQColor & color )
{
    setEntry( idx, color.rgb() );
}

/*!
    Returns the TQRgb value in the colorcell with index \a idx.
*/
TQColor TQGLColormap::entryColor( int idx ) const
{
    if ( !d || idx < 0 || idx > (int) d->cells.size() )
	return TQColor();
    else
	return TQColor( d->cells[ idx ] );
}

/*!
    Returns true if the colormap is empty; otherwise returns false. A
    colormap with no color values set is considered to be empty.
*/
bool TQGLColormap::isEmpty() const
{
    return (d == 0) || (d->cells.size() == 0)  || (d->cmapHandle == 0);
}


/*!
    Returns the number of colorcells in the colormap.
*/
int TQGLColormap::size() const
{
    return d != 0 ? d->cells.size() : 0;
}

/*!
    Returns the index of the color \a color. If \a color is not in the
    map, -1 is returned.
*/
int TQGLColormap::find( TQRgb color ) const
{
    if ( d )
	return d->cells.find( color );
    return -1;
}

/*!
    Returns the index of the color that is the closest match to color
    \a color.
*/
int TQGLColormap::findNearest( TQRgb color ) const
{
    int idx = find( color );
    if ( idx >= 0 )
	return idx;
    int mapSize = size();
    int mindist = 200000;
    int r = tqRed( color );
    int g = tqGreen( color );
    int b = tqBlue( color );
    int rx, gx, bx, dist;
    for ( int i=0; i < mapSize; i++ ) {
	TQRgb ci = d->cells[i];
	rx = r - tqRed( ci );
	gx = g - tqGreen( ci );
	bx = b - tqBlue( ci );
	dist = rx*rx + gx*gx + bx*bx;	// calculate distance
	if ( dist < mindist ) {		// minimal?
	    mindist = dist;
	    idx = i;
	}
    }
    return idx;
}
