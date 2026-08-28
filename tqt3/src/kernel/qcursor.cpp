/****************************************************************************
**
** Implementation of TQCursor class
**
** Created : 940220
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

#include "ntqcursor.h"

#ifndef TQT_NO_CURSOR

#include "ntqbitmap.h"
#include "ntqimage.h"
#include "ntqdatastream.h"


/*!
    \class TQCursor ntqcursor.h

    \brief The TQCursor class provides a mouse cursor with an arbitrary
    shape.

    \ingroup appearance
    \ingroup shared

    This class is mainly used to create mouse cursors that are
    associated with particular widgets and to get and set the position
    of the mouse cursor.

    TQt has a number of standard cursor shapes, but you can also make
    custom cursor shapes based on a TQBitmap, a mask and a hotspot.

    To associate a cursor with a widget, use TQWidget::setCursor(). To
    associate a cursor with all widgets (normally for a short period
    of time), use TQApplication::setOverrideCursor().

    To set a cursor shape use TQCursor::setShape() or use the TQCursor
    constructor which takes the shape as argument, or you can use one
    of the predefined cursors defined in the \l CursorShape enum.

    If you want to create a cursor with your own bitmap, either use
    the TQCursor constructor which takes a bitmap and a mask or the
    constructor which takes a pixmap as arguments.

    To set or get the position of the mouse cursor use the static
    methods TQCursor::pos() and TQCursor::setPos().

    \img cursors.png Cursor Shapes

    \sa TQWidget \link guibooks.html#fowler GUI Design Handbook:
    Cursors\endlink

    On X11, TQt supports the \link
    http://www.xfree86.org/4.3.0/Xcursor.3.html Xcursor\endlink
    library, which allows for full color icon themes. The table below
    shows the cursor name used for each TQt::CursorShape value. If a
    cursor cannot be found using the name shown below, a standard X11
    cursor will be used instead. Note: X11 does not provide
    appropriate cursors for all possible TQt::CursorShape values. It
    is possible that some cursors will be taken from the Xcursor
    theme, while others will use an internal bitmap cursor.

    \table
    \header \i TQt::CursorShape Values   \i Cursor Names
    \row \i TQt::ArrowCursor             \i left_ptr
    \row \i TQt::UpArrowCursor           \i up_arrow
    \row \i TQt::CrossCursor             \i cross
    \row \i TQt::WaitCursor              \i wait
    \row \i TQt::BusyCursor		\i left_ptr_watch
    \row \i TQt::IbeamCursor             \i ibeam
    \row \i TQt::SizeVerCursor           \i size_ver
    \row \i TQt::SizeHorCursor           \i size_hor
    \row \i TQt::SizeBDiagCursor         \i size_bdiag
    \row \i TQt::SizeFDiagCursor         \i size_fdiag
    \row \i TQt::SizeAllCursor           \i size_all
    \row \i TQt::SplitVCursor            \i split_v
    \row \i TQt::SplitHCursor            \i split_h
    \row \i TQt::PointingHandCursor      \i pointing_hand
    \row \i TQt::ForbiddenCursor         \i forbidden
    \row \i TQt::WhatsThisCursor         \i whats_this
    \endtable
*/

/*!
    \enum TQt::CursorShape

    This enum type defines the various cursors that can be used.

    \value ArrowCursor  standard arrow cursor
    \value UpArrowCursor  upwards arrow
    \value CrossCursor  crosshair
    \value WaitCursor  hourglass/watch
    \value BusyCursor  standard arrow with hourglass/watch
    \value IbeamCursor  ibeam/text entry
    \value SizeVerCursor  vertical resize
    \value SizeHorCursor  horizontal resize
    \value SizeFDiagCursor  diagonal resize (\)
    \value SizeBDiagCursor  diagonal resize (/)
    \value SizeAllCursor  all directions resize
    \value BlankCursor  blank/invisible cursor
    \value SplitVCursor  vertical splitting
    \value SplitHCursor  horizontal splitting
    \value PointingHandCursor  a pointing hand
    \value ForbiddenCursor  a slashed circle
    \value WhatsThisCursor  an arrow with a question mark
    \value BitmapCursor

    ArrowCursor is the default for widgets in a normal state.

    \img cursors.png Cursor Shapes
*/

/*****************************************************************************
  TQCursor stream functions
 *****************************************************************************/

#ifndef TQT_NO_DATASTREAM


/*!
    \relates TQCursor
    Writes the cursor \a c to the stream \a s.

    \sa \link datastreamformat.html Format of the TQDataStream operators \endlink
*/

TQDataStream &operator<<( TQDataStream &s, const TQCursor &c )
{
    s << (TQ_INT16)c.shape();			// write shape id to stream
    if ( c.shape() == TQt::BitmapCursor ) {		// bitmap cursor
#if !defined(TQT_NO_IMAGEIO)
	s << *c.bitmap() << *c.mask();
	s << c.hotSpot();
#else
	tqWarning("No Image Cursor I/O");
#endif
    }
    return s;
}

/*!
    \relates TQCursor
    Reads a cursor from the stream \a s and sets \a c to the read data.

    \sa \link datastreamformat.html Format of the TQDataStream operators \endlink
*/

TQDataStream &operator>>( TQDataStream &s, TQCursor &c )
{
    TQ_INT16 shape;
    s >> shape;					// read shape id from stream
    if ( shape == TQt::BitmapCursor ) {		// read bitmap cursor
#if !defined(TQT_NO_IMAGEIO)
	TQBitmap bm, bmm;
	TQPoint	hot;
	s >> bm >> bmm >> hot;
	c = TQCursor( bm, bmm, hot.x(), hot.y() );
#else
	tqWarning("No Image Cursor I/O");
#endif
    } else {
	c.setShape( (int)shape );		// create cursor with shape
    }
    return s;
}
#endif // TQT_NO_DATASTREAM


/*!
    Constructs a custom pixmap cursor.

    \a pixmap is the image. It is usual to give it a mask (set using
    TQPixmap::setMask()). \a hotX and \a hotY define the cursor's hot
    spot.

    If \a hotX is negative, it is set to the \c{pixmap().width()/2}.
    If \a hotY is negative, it is set to the \c{pixmap().height()/2}.

    Valid cursor sizes depend on the display hardware (or the
    underlying window system). We recommend using 32x32 cursors,
    because this size is supported on all platforms. Some platforms
    also support 16x16, 48x48 and 64x64 cursors.

    Currently, only black-and-white pixmaps can be used.

    \sa TQPixmap::TQPixmap(), TQPixmap::setMask()
*/

TQCursor::TQCursor( const TQPixmap &pixmap, int hotX, int hotY )
{
    TQImage img = pixmap.convertToImage().
		    convertDepth( 8, TQt::ThresholdDither|TQt::AvoidDither );
    TQBitmap bm;
    bm.convertFromImage( img, TQt::ThresholdDither|TQt::AvoidDither );
    TQBitmap bmm;
    if ( bm.mask() ) {
	bmm = *bm.mask();
	TQBitmap nullBm;
	bm.setMask( nullBm );
    }
    else if ( pixmap.mask() ) {
	TQImage mimg = pixmap.mask()->convertToImage().
	              convertDepth( 8, TQt::ThresholdDither|TQt::AvoidDither );
	bmm.convertFromImage( mimg, TQt::ThresholdDither|TQt::AvoidDither );
    }
    else {
	bmm.resize( bm.size() );
	bmm.fill( TQt::color1 );
    }

    setBitmap(bm,bmm,hotX,hotY);
}



/*!
    Constructs a custom bitmap cursor.

    \a bitmap and
    \a mask make up the bitmap.
    \a hotX and
    \a hotY define the cursor's hot spot.

    If \a hotX is negative, it is set to the \c{bitmap().width()/2}.
    If \a hotY is negative, it is set to the \c{bitmap().height()/2}.

    The cursor \a bitmap (B) and \a mask (M) bits are combined like this:
    \list
    \i B=1 and M=1 gives black.
    \i B=0 and M=1 gives white.
    \i B=0 and M=0 gives transparent.
    \i B=1 and M=0 gives an undefined result.
    \endlist

    Use the global TQt color \c color0 to draw 0-pixels and \c color1 to
    draw 1-pixels in the bitmaps.

    Valid cursor sizes depend on the display hardware (or the
    underlying window system). We recommend using 32x32 cursors,
    because this size is supported on all platforms. Some platforms
    also support 16x16, 48x48 and 64x64 cursors.

    \sa TQBitmap::TQBitmap(), TQBitmap::setMask()
*/

TQCursor::TQCursor( const TQBitmap &bitmap, const TQBitmap &mask,
		  int hotX, int hotY )
{
    setBitmap(bitmap,mask,hotX,hotY);
}

#endif // TQT_NO_CURSOR


