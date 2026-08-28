/****************************************************************************
**
** Implementation of TQVariant class
**
** Created : 990414
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

#include <float.h>

#include "ntqvariant.h"
#ifndef TQT_NO_VARIANT
#include "ntqstring.h"
#include "ntqcstring.h"
#include "ntqfont.h"
#include "ntqpixmap.h"
#include "ntqimage.h"
#include "ntqbrush.h"
#include "ntqpoint.h"
#include "ntqrect.h"
#include "ntqsize.h"
#include "ntqcolor.h"
#include "ntqpalette.h"
#include "ntqiconset.h"
#include "ntqdatastream.h"
#include "ntqregion.h"
#include "ntqpointarray.h"
#include "ntqbitmap.h"
#include "ntqcursor.h"
#include "ntqdatetime.h"
#include "ntqsizepolicy.h"
#include "ntqshared.h"
#include "ntqbitarray.h"
#include "ntqkeysequence.h"
#include "ntqpen.h"

#ifndef DBL_DIG
#define DBL_DIG 10
#endif //DBL_DIG

// Uncomment to test for memory leaks or to run qt/test/qvariant/main.cpp
// #define TQVARIANT_DEBUG


static bool isNumeric(TQVariant::Type type)
{
    return (type == TQVariant::Int || type == TQVariant::UInt
	    || type == TQVariant::Double || type == TQVariant::LongLong
	    || type == TQVariant::ULongLong || type == TQVariant::Bool);
}


#ifdef TQVARIANT_DEBUG
int qv_count = 0;
int get_qv_count() { return qv_count; }
#endif

TQVariant::Private::Private()
{
#ifdef TQVARIANT_DEBUG
    qv_count++;
#endif
    typ = TQVariant::Invalid;
    is_null = true;
}

TQVariant::Private::Private( Private* d )
{
#ifdef TQVARIANT_DEBUG
    qv_count++;
#endif

    switch( d->typ )
	{
	case TQVariant::Invalid:
	    break;
	case TQVariant::Bitmap:
	    value.ptr = new TQBitmap( *((TQBitmap*)d->value.ptr) );
	    break;
	case TQVariant::Region:
	    value.ptr = new TQRegion( *((TQRegion*)d->value.ptr) );
	    // ## Force a detach
	    // ((TQRegion*)value.ptr)->translate( 0, 0 );
	    break;
	case TQVariant::PointArray:
	    // TQPointArray is explicit shared
	    value.ptr = new TQPointArray( *((TQPointArray*)d->value.ptr) );
	    break;
	case TQVariant::String:
	    value.ptr = new TQString( *((TQString*)d->value.ptr) );
	    break;
	case TQVariant::CString:
	    // TQCString is explicit shared
	    value.ptr = new TQCString( *((TQCString*)d->value.ptr) );
	    break;
#ifndef TQT_NO_STRINGLIST
	case TQVariant::StringList:
	    value.ptr = new TQStringList( *((TQStringList*)d->value.ptr) );
	    break;
#endif //TQT_NO_STRINGLIST
	case TQVariant::Font:
	    value.ptr = new TQFont( *((TQFont*)d->value.ptr) );
	    break;
	case TQVariant::Pixmap:
	    value.ptr = new TQPixmap( *((TQPixmap*)d->value.ptr) );
	    break;
	case TQVariant::Image:
	    // TQImage is explicit shared
	    value.ptr = new TQImage( *((TQImage*)d->value.ptr) );
	    break;
	case TQVariant::Brush:
	    value.ptr = new TQBrush( *((TQBrush*)d->value.ptr) );
	    // ## Force a detach
	    // ((TQBrush*)value.ptr)->setColor( ((TQBrush*)value.ptr)->color() );
	    break;
	case TQVariant::Point:
	    value.ptr = new TQPoint( *((TQPoint*)d->value.ptr) );
	    break;
	case TQVariant::Rect:
	    value.ptr = new TQRect( *((TQRect*)d->value.ptr) );
	    break;
	case TQVariant::Size:
	    value.ptr = new TQSize( *((TQSize*)d->value.ptr) );
	    break;
	case TQVariant::Color:
	    value.ptr = new TQColor( *((TQColor*)d->value.ptr) );
	    break;
#ifndef TQT_NO_PALETTE
	case TQVariant::Palette:
	    value.ptr = new TQPalette( *((TQPalette*)d->value.ptr) );
	    break;
	case TQVariant::ColorGroup:
	    value.ptr = new TQColorGroup( *((TQColorGroup*)d->value.ptr) );
	    break;
#endif
#ifndef TQT_NO_ICONSET
	case TQVariant::IconSet:
	    value.ptr = new TQIconSet( *((TQIconSet*)d->value.ptr) );
	    break;
#endif
#ifndef TQT_NO_TEMPLATE_VARIANT
	case TQVariant::Map:
	    value.ptr = new TQMap<TQString,TQVariant>( *((TQMap<TQString,TQVariant>*)d->value.ptr) );
	    break;
	case TQVariant::List:
	    value.ptr = new TQValueList<TQVariant>( *((TQValueList<TQVariant>*)d->value.ptr) );
	    break;
#endif
	case TQVariant::Date:
	    value.ptr = new TQDate( *((TQDate*)d->value.ptr) );
	    break;
	case TQVariant::Time:
	    value.ptr = new TQTime( *((TQTime*)d->value.ptr) );
	    break;
	case TQVariant::DateTime:
	    value.ptr = new TQDateTime( *((TQDateTime*)d->value.ptr) );
	    break;
	case TQVariant::ByteArray:
	    value.ptr = new TQByteArray( *((TQByteArray*)d->value.ptr) );
	    break;
	case TQVariant::BitArray:
	    value.ptr = new TQBitArray( *((TQBitArray*)d->value.ptr) );
	    break;
#ifndef TQT_NO_ACCEL
	case TQVariant::KeySequence:
	    value.ptr = new TQKeySequence( *((TQKeySequence*)d->value.ptr) );
	    break;
#endif
	case TQVariant::Pen:
	    value.ptr = new TQPen( *((TQPen*)d->value.ptr) );
	    break;
	case TQVariant::Int:
	    value.i = d->value.i;
	    break;
	case TQVariant::UInt:
	    value.u = d->value.u;
	    break;
	case TQVariant::LongLong:
	    value.ll = d->value.ll;
	    break;
	case TQVariant::ULongLong:
	    value.ull = d->value.ull;
	    break;
	case TQVariant::Bool:
	    value.b = d->value.b;
	    break;
	case TQVariant::Double:
	    value.d = d->value.d;
	    break;
	case TQVariant::SizePolicy:
	    value.ptr = new TQSizePolicy( *((TQSizePolicy*)d->value.ptr) );
	    break;
	case TQVariant::Cursor:
	    value.ptr = new TQCursor( *((TQCursor*)d->value.ptr) );
	    break;
	default:
	    Q_ASSERT( 0 );
	}

    typ = d->typ;
    is_null = d->is_null;
}

TQVariant::Private::~Private()
{
#ifdef TQVARIANT_DEBUG
    qv_count--;
#endif
    clear();
}

void TQVariant::Private::clear()
{
    switch( typ )
	{
	case TQVariant::Bitmap:
	    delete (TQBitmap*)value.ptr;
	    break;
	case TQVariant::Cursor:
	    delete (TQCursor*)value.ptr;
	    break;
	case TQVariant::Region:
	    delete (TQRegion*)value.ptr;
	    break;
	case TQVariant::PointArray:
	    delete (TQPointArray*)value.ptr;
	    break;
	case TQVariant::String:
	    delete (TQString*)value.ptr;
	    break;
	case TQVariant::CString:
	    delete (TQCString*)value.ptr;
	    break;
#ifndef TQT_NO_STRINGLIST
	case TQVariant::StringList:
	    delete (TQStringList*)value.ptr;
	    break;
#endif //TQT_NO_STRINGLIST
	case TQVariant::Font:
	    delete (TQFont*)value.ptr;
	    break;
	case TQVariant::Pixmap:
	    delete (TQPixmap*)value.ptr;
	    break;
	case TQVariant::Image:
	    delete (TQImage*)value.ptr;
	    break;
	case TQVariant::Brush:
	    delete (TQBrush*)value.ptr;
	    break;
	case TQVariant::Point:
	    delete (TQPoint*)value.ptr;
	    break;
	case TQVariant::Rect:
	    delete (TQRect*)value.ptr;
	    break;
	case TQVariant::Size:
	    delete (TQSize*)value.ptr;
	    break;
	case TQVariant::Color:
	    delete (TQColor*)value.ptr;
	    break;
#ifndef TQT_NO_PALETTE
	case TQVariant::Palette:
	    delete (TQPalette*)value.ptr;
	    break;
	case TQVariant::ColorGroup:
	    delete (TQColorGroup*)value.ptr;
	    break;
#endif
#ifndef TQT_NO_ICONSET
	case TQVariant::IconSet:
	    delete (TQIconSet*)value.ptr;
	    break;
#endif
#ifndef TQT_NO_TEMPLATE_VARIANT
	case TQVariant::Map:
	    delete (TQMap<TQString,TQVariant>*)value.ptr;
	    break;
	case TQVariant::List:
	    delete (TQValueList<TQVariant>*)value.ptr;
	    break;
#endif
	case TQVariant::SizePolicy:
	    delete (TQSizePolicy*)value.ptr;
	    break;
	case TQVariant::Date:
	    delete (TQDate*)value.ptr;
	    break;
	case TQVariant::Time:
	    delete (TQTime*)value.ptr;
	    break;
	case TQVariant::DateTime:
	    delete (TQDateTime*)value.ptr;
	    break;
	case TQVariant::ByteArray:
	    delete (TQByteArray*)value.ptr;
	    break;
	case TQVariant::BitArray:
	    delete (TQBitArray*)value.ptr;
	    break;
#ifndef TQT_NO_ACCEL
	case TQVariant::KeySequence:
	    delete (TQKeySequence*)value.ptr;
	    break;
#endif
	case TQVariant::Pen:
	    delete (TQPen*)value.ptr;
	    break;
	case TQVariant::Invalid:
	case TQVariant::Int:
	case TQVariant::UInt:
	case TQVariant::LongLong:
	case TQVariant::ULongLong:
	case TQVariant::Bool:
	case TQVariant::Double:
	    break;
	}

    typ = TQVariant::Invalid;
    is_null = true;
}

/*!
    \class TQVariant ntqvariant.h
    \brief The TQVariant class acts like a union for the most common TQt data types.

    \ingroup objectmodel
    \ingroup misc
    \mainclass

    Because C++ forbids unions from including types that have
    non-default constructors or destructors, most interesting TQt
    classes cannot be used in unions. Without TQVariant, this would be
    a problem for TQObject::property() and for database work, etc.

    A TQVariant object holds a single value of a single type() at a
    time. (Some type()s are multi-valued, for example a string list.)
    You can find out what type, T, the variant holds, convert it to a
    different type using one of the asT() functions, e.g. asSize(),
    get its value using one of the toT() functions, e.g. toSize(), and
    check whether the type can be converted to a particular type using
    canCast().

    The methods named toT() (for any supported T, see the \c Type
    documentation for a list) are const. If you ask for the stored
    type, they return a copy of the stored object. If you ask for a
    type that can be generated from the stored type, toT() copies and
    converts and leaves the object itself unchanged. If you ask for a
    type that cannot be generated from the stored type, the result
    depends on the type (see the function documentation for details).

    Note that three data types supported by TQVariant are explicitly
    shared, namely TQImage, TQPointArray, and TQCString, and in these
    cases the toT() methods return a shallow copy. In almost all cases
    you must make a deep copy of the returned values before modifying
    them.

    The asT() functions are not const. They do conversion like the
    toT() methods, set the variant to hold the converted value, and
    return a reference to the new contents of the variant.

    Here is some example code to demonstrate the use of TQVariant:

    \code
    TQDataStream out(...);
    TQVariant v(123);          // The variant now contains an int
    int x = v.toInt();        // x = 123
    out << v;                 // Writes a type tag and an int to out
    v = TQVariant("hello");    // The variant now contains a TQCString
    v = TQVariant(tr("hello"));// The variant now contains a TQString
    int y = v.toInt();        // y = 0 since v cannot be converted to an int
    TQString s = v.toString(); // s = tr("hello")  (see TQObject::tr())
    out << v;                 // Writes a type tag and a TQString to out
    ...
    TQDataStream in(...);      // (opening the previously written stream)
    in >> v;                  // Reads an Int variant
    int z = v.toInt();        // z = 123
    tqDebug("Type is %s",      // prints "Type is int"
	    v.typeName());
    v.asInt() += 100;	      // The variant now hold the value 223.
    v = TQVariant( TQStringList() );
    v.asStringList().append( "Hello" );
    \endcode

    You can even store TQValueList<TQVariant>s and
    TQMap<TQString,TQVariant>s in a variant, so you can easily construct
    arbitrarily complex data structures of arbitrary types. This is
    very powerful and versatile, but may prove less memory and speed
    efficient than storing specific types in standard data structures.

    TQVariant also supports the notion of NULL values, where you have a
    defined type with no value set.
    \code
    TQVariant x, y( TQString() ), z( TQString("") );
    x.asInt();
    // x.isNull() == true, y.isNull() == true, z.isNull() == false
    \endcode

    See the \link collection.html Collection Classes\endlink.
*/

/*!
    \enum TQVariant::Type

    This enum type defines the types of variable that a TQVariant can
    contain.

    \value Invalid  no type
    \value BitArray  a TQBitArray
    \value ByteArray  a TQByteArray
    \value Bitmap  a TQBitmap
    \value Bool  a bool
    \value Brush  a TQBrush
    \value Color  a TQColor
    \value ColorGroup  a TQColorGroup
    \value Cursor  a TQCursor
    \value Date  a TQDate
    \value DateTime  a TQDateTime
    \value Double  a double
    \value Font  a TQFont
    \value IconSet  a TQIconSet
    \value Image  a TQImage
    \value Int  an int
    \value KeySequence  a TQKeySequence
    \value List  a TQValueList<TQVariant>
    \value LongLong a long long
    \value ULongLong an unsigned long long
    \value Map  a TQMap<TQString,TQVariant>
    \value Palette  a TQPalette
    \value Pen  a TQPen
    \value Pixmap  a TQPixmap
    \value Point  a TQPoint
    \value PointArray  a TQPointArray
    \value Rect  a TQRect
    \value Region  a TQRegion
    \value Size  a TQSize
    \value SizePolicy  a TQSizePolicy
    \value String  a TQString
    \value CString  a TQCString
    \value StringList  a TQStringList
    \value Time  a TQTime
    \value UInt  an unsigned int

    Note that TQt's definition of bool depends on the compiler.
    \c ntqglobal.h has the system-dependent definition of bool.
*/

/*!
    Constructs an invalid variant.
*/
TQVariant::TQVariant()
{
    d = new Private;
}

/*!
    Destroys the TQVariant and the contained object.

    Note that subclasses that reimplement clear() should reimplement
    the destructor to call clear(). This destructor calls clear(), but
    because it is the destructor, TQVariant::clear() is called rather
    than a subclass's clear().
*/
TQVariant::~TQVariant()
{
    if ( d->deref() )
	delete d;
}

/*!
    Constructs a copy of the variant, \a p, passed as the argument to
    this constructor. Usually this is a deep copy, but a shallow copy
    is made if the stored data type is explicitly shared, as e.g.
    TQImage is.
*/
TQVariant::TQVariant( const TQVariant& p )
{
    p.d->ref();
    d = p.d;
}

#ifndef TQT_NO_DATASTREAM
/*!
    Reads the variant from the data stream, \a s.
*/
TQVariant::TQVariant( TQDataStream& s )
{
    d = new Private;
    s >> *this;
}
#endif //TQT_NO_DATASTREAM

/*!
    Constructs a new variant with a string value, \a val.
*/
TQVariant::TQVariant( const TQString& val )
{
    d = new Private;
    d->typ = String;
    d->value.ptr = new TQString( val );
}

/*!
    Constructs a new variant with a C-string value, \a val.

    If you want to modify the TQCString after you've passed it to this
    constructor, we recommend passing a deep copy (see
    TQCString::copy()).
*/
TQVariant::TQVariant( const TQCString& val )
{
    d = new Private;
    d->typ = CString;
    d->value.ptr = new TQCString( val );
}

/*!
    Constructs a new variant with a C-string value of \a val if \a val
    is non-null. The variant creates a deep copy of \a val.

    If \a val is null, the resulting variant has type Invalid.
*/
TQVariant::TQVariant( const char* val )
{
    d = new Private;
    if ( val == 0 )
	return;
    d->typ = CString;
    d->value.ptr = new TQCString( val );
}

#ifndef TQT_NO_STRINGLIST
/*!
    Constructs a new variant with a string list value, \a val.
*/
TQVariant::TQVariant( const TQStringList& val )
{
    d = new Private;
    d->typ = StringList;
    d->value.ptr = new TQStringList( val );
    d->is_null = false;
}
#endif // TQT_NO_STRINGLIST

#ifndef TQT_NO_TEMPLATE_VARIANT
/*!
    Constructs a new variant with a map of TQVariants, \a val.
*/
TQVariant::TQVariant( const TQMap<TQString,TQVariant>& val )
{
    d = new Private;
    d->typ = Map;
    d->value.ptr = new TQMap<TQString,TQVariant>( val );
    d->is_null = false;
}
#endif
/*!
    Constructs a new variant with a font value, \a val.
*/
TQVariant::TQVariant( const TQFont& val )
{
    d = new Private;
    d->typ = Font;
    d->value.ptr = new TQFont( val );
    d->is_null = false;
}

/*!
    Constructs a new variant with a pixmap value, \a val.
*/
TQVariant::TQVariant( const TQPixmap& val )
{
    d = new Private;
    d->typ = Pixmap;
    d->value.ptr = new TQPixmap( val );
}


/*!
    Constructs a new variant with an image value, \a val.

    Because TQImage is explicitly shared, you may need to pass a deep
    copy to the variant using TQImage::copy(), e.g. if you intend
    changing the image you've passed later on.
*/
TQVariant::TQVariant( const TQImage& val )
{
    d = new Private;
    d->typ = Image;
    d->value.ptr = new TQImage( val );
}

/*!
    Constructs a new variant with a brush value, \a val.
*/
TQVariant::TQVariant( const TQBrush& val )
{
    d = new Private;
    d->typ = Brush;
    d->value.ptr = new TQBrush( val );
    d->is_null = false;
}

/*!
    Constructs a new variant with a point value, \a val.
*/
TQVariant::TQVariant( const TQPoint& val )
{
    d = new Private;
    d->typ = Point;
    d->value.ptr = new TQPoint( val );
}

/*!
    Constructs a new variant with a rect value, \a val.
*/
TQVariant::TQVariant( const TQRect& val )
{
    d = new Private;
    d->typ = Rect;
    d->value.ptr = new TQRect( val );
}

/*!
    Constructs a new variant with a size value, \a val.
*/
TQVariant::TQVariant( const TQSize& val )
{
    d = new Private;
    d->typ = Size;
    d->value.ptr = new TQSize( val );
}

/*!
    Constructs a new variant with a color value, \a val.
*/
TQVariant::TQVariant( const TQColor& val )
{
    d = new Private;
    d->typ = Color;
    d->value.ptr = new TQColor( val );
    d->is_null = false;
}

#ifndef TQT_NO_PALETTE
/*!
    Constructs a new variant with a color palette value, \a val.
*/
TQVariant::TQVariant( const TQPalette& val )
{
    d = new Private;
    d->typ = Palette;
    d->value.ptr = new TQPalette( val );
    d->is_null = false;
}

/*!
    Constructs a new variant with a color group value, \a val.
*/
TQVariant::TQVariant( const TQColorGroup& val )
{
    d = new Private;
    d->typ = ColorGroup;
    d->value.ptr = new TQColorGroup( val );
    d->is_null = false;
}
#endif //TQT_NO_PALETTE
#ifndef TQT_NO_ICONSET
/*!
    Constructs a new variant with an icon set value, \a val.
*/
TQVariant::TQVariant( const TQIconSet& val )
{
    d = new Private;
    d->typ = IconSet;
    d->value.ptr = new TQIconSet( val );
}
#endif //TQT_NO_ICONSET
/*!
    Constructs a new variant with a region value, \a val.
*/
TQVariant::TQVariant( const TQRegion& val )
{
    d = new Private;
    d->typ = Region;
    // ## Force a detach
    d->value.ptr = new TQRegion( val );
    ((TQRegion*)d->value.ptr)->translate( 0, 0 );
}

/*!
    Constructs a new variant with a bitmap value, \a val.
*/
TQVariant::TQVariant( const TQBitmap& val )
{
    d = new Private;
    d->typ = Bitmap;
    d->value.ptr = new TQBitmap( val );
}

/*!
    Constructs a new variant with a cursor value, \a val.
*/
TQVariant::TQVariant( const TQCursor& val )
{
    d = new Private;
    d->typ = Cursor;
    d->value.ptr = new TQCursor( val );
    d->is_null = false;
}

/*!
    Constructs a new variant with a point array value, \a val.

    Because TQPointArray is explicitly shared, you may need to pass a
    deep copy to the variant using TQPointArray::copy(), e.g. if you
    intend changing the point array you've passed later on.
*/
TQVariant::TQVariant( const TQPointArray& val )
{
    d = new Private;
    d->typ = PointArray;
    d->value.ptr = new TQPointArray( val );
}

/*!
    Constructs a new variant with a date value, \a val.
*/
TQVariant::TQVariant( const TQDate& val )
{
    d = new Private;
    d->typ = Date;
    d->value.ptr = new TQDate( val );
}

/*!
    Constructs a new variant with a time value, \a val.
*/
TQVariant::TQVariant( const TQTime& val )
{
    d = new Private;
    d->typ = Time;
    d->value.ptr = new TQTime( val );
}

/*!
    Constructs a new variant with a date/time value, \a val.
*/
TQVariant::TQVariant( const TQDateTime& val )
{
    d = new Private;
    d->typ = DateTime;
    d->value.ptr = new TQDateTime( val );
}

/*!
    Constructs a new variant with a bytearray value, \a val.
*/
TQVariant::TQVariant( const TQByteArray& val )
{
    d = new Private;
    d->typ = ByteArray;
    d->value.ptr = new TQByteArray( val );
}

/*!
    Constructs a new variant with a bitarray value, \a val.
*/
TQVariant::TQVariant( const TQBitArray& val )
{
    d = new Private;
    d->typ = BitArray;
    d->value.ptr = new TQBitArray( val );
}

#ifndef TQT_NO_ACCEL

/*!
    Constructs a new variant with a key sequence value, \a val.
*/
TQVariant::TQVariant( const TQKeySequence& val )
{
    d = new Private;
    d->typ = KeySequence;
    d->value.ptr = new TQKeySequence( val );
    d->is_null = false;
}

#endif

/*!
    Constructs a new variant with a pen value, \a val.
*/
TQVariant::TQVariant( const TQPen& val )
{
    d = new Private;
    d->typ = Pen;
    d->value.ptr = new TQPen( val );
}

/*!
    Constructs a new variant with an integer value, \a val.
*/
TQVariant::TQVariant( int val )
{
    d = new Private;
    d->typ = Int;
    d->value.i = val;
    d->is_null = false;
}

/*!
    Constructs a new variant with an unsigned integer value, \a val.
*/
TQVariant::TQVariant( uint val )
{
    d = new Private;
    d->typ = UInt;
    d->value.u = val;
    d->is_null = false;
}

/*!
    Constructs a new variant with a long long integer value, \a val.
*/
TQVariant::TQVariant( TQ_LLONG val )
{
    d = new Private;
    d->typ = LongLong;
    d->value.ll = val;
    d->is_null = false;
}

/*!
    Constructs a new variant with an unsigned long long integer value, \a val.
*/

TQVariant::TQVariant( TQ_ULLONG val )
{
    d = new Private;
    d->typ = ULongLong;
    d->value.ull = val;
    d->is_null = false;
}

/*!
    Constructs a new variant with a boolean value, \a val.
*/
TQVariant::TQVariant( bool val )
{
    d = new Private;
    d->typ = Bool;
    d->value.b = val;
    d->is_null = false;
}


/*!
    Constructs a new variant with a floating point value, \a val.
*/
TQVariant::TQVariant( double val )
{
    d = new Private;
    d->typ = Double;
    d->value.d = val;
    d->is_null = false;
}

#ifndef TQT_NO_TEMPLATE_VARIANT
/*!
    Constructs a new variant with a list value, \a val.
*/
TQVariant::TQVariant( const TQValueList<TQVariant>& val )
{
    d = new Private;
    d->typ = List;
    d->value.ptr = new TQValueList<TQVariant>( val );
    d->is_null = false;
}
#endif

/*!
    Constructs a new variant with a size policy value, \a val.
*/
TQVariant::TQVariant( TQSizePolicy val )
{
    d = new Private;
    d->typ = SizePolicy;
    d->value.ptr = new TQSizePolicy( val );
    d->is_null = false;
}

/*!
    Assigns the value of the variant \a variant to this variant.

    This is a deep copy of the variant, but note that if the variant
    holds an explicitly shared type such as TQImage, a shallow copy is
    performed.
*/
TQVariant& TQVariant::operator= ( const TQVariant& variant )
{
    TQVariant& other = (TQVariant&)variant;

    other.d->ref();
    if ( d->deref() )
	delete d;

    d = other.d;

    return *this;
}

/*!
    \internal
*/
void TQVariant::detach()
{
    if ( d->count == 1 )
	return;

    d->deref();
    d = new Private( d );
}

/*!
    Returns the name of the type stored in the variant. The returned
    strings describe the C++ datatype used to store the data: for
    example, "TQFont", "TQString", or "TQValueList<TQVariant>". An Invalid
    variant returns 0.
*/
const char* TQVariant::typeName() const
{
    return typeToName( (Type) d->typ );
}

/*!
    Convert this variant to type Invalid and free up any resources
    used.
*/
void TQVariant::clear()
{
    if ( d->count > 1 )
    {
	d->deref();
	d = new Private;
	return;
    }

    d->clear();
}

/* Attention!

   For dependency reasons, this table is duplicated in moc.y. If you
   change one, change both.

   (Search for the word 'Attention' in moc.y.)
*/
static const int ntypes = 35;
static const char* const type_map[ntypes] =
{
    0,
    "TQMap<TQString,TQVariant>",
    "TQValueList<TQVariant>",
    "TQString",
    "TQStringList",
    "TQFont",
    "TQPixmap",
    "TQBrush",
    "TQRect",
    "TQSize",
    "TQColor",
    "TQPalette",
    "TQColorGroup",
    "TQIconSet",
    "TQPoint",
    "TQImage",
    "int",
    "uint",
    "bool",
    "double",
    "TQCString",
    "TQPointArray",
    "TQRegion",
    "TQBitmap",
    "TQCursor",
    "TQSizePolicy",
    "TQDate",
    "TQTime",
    "TQDateTime",
    "TQByteArray",
    "TQBitArray",
    "TQKeySequence",
    "TQPen",
    "TQ_LLONG",
    "TQ_ULLONG"
};


/*!
    Converts the enum representation of the storage type, \a typ, to
    its string representation.
*/
const char* TQVariant::typeToName( Type typ )
{
    if ( typ >= ntypes )
	return 0;
    return type_map[typ];
}


/*!
    Converts the string representation of the storage type given in \a
    name, to its enum representation.

    If the string representation cannot be converted to any enum
    representation, the variant is set to \c Invalid.
*/
TQVariant::Type TQVariant::nameToType( const char* name )
{
    for ( int i = 0; i < ntypes; i++ ) {
	if ( !qstrcmp( type_map[i], name ) )
	    return (Type) i;
    }
    return Invalid;
}

#ifndef TQT_NO_DATASTREAM
/*!
    Internal function for loading a variant from stream \a s. Use the
    stream operators instead.

    \internal
*/
void TQVariant::load( TQDataStream& s )
{
    clear();
    TQ_UINT32 u;
    s >> u;
    Type t = (Type)u;

    switch( t ) {
    case Invalid:
	{
	    // Since we wrote something, we should read something
	    TQString x;
	    s >> x;
	    d->typ = t;
	    d->is_null = true;
	}
	break;
#ifndef TQT_NO_TEMPLATE_VARIANT
    case Map:
	{
	    TQMap<TQString,TQVariant>* x = new TQMap<TQString,TQVariant>;
	    s >> *x;
	    d->value.ptr = x;
	    d->is_null = false;
	}
	break;
    case List:
	{
	    TQValueList<TQVariant>* x = new TQValueList<TQVariant>;
	    s >> *x;
	    d->value.ptr = x;
	    d->is_null = false;
	}
	break;
#endif
    case Cursor:
	{
#ifndef TQT_NO_CURSOR
	    TQCursor* x = new TQCursor;
	    s >> *x;
	    d->value.ptr = x;
	    d->is_null = false;
#endif
	}
	break;
    case Bitmap:
	{
	    TQBitmap* x = new TQBitmap;
#ifndef TQT_NO_IMAGEIO
	    s >> *x;
#endif
	    d->value.ptr = x;
	}
	break;
    case Region:
	{
	    TQRegion* x = new TQRegion;
	    s >> *x;
	    d->value.ptr = x;
	}
	break;
    case PointArray:
	{
	    TQPointArray* x = new TQPointArray;
	    s >> *x;
	    d->value.ptr = x;
	}
	break;
    case String:
	{
	    TQString* x = new TQString;
	    s >> *x;
	    d->value.ptr = x;
	}
	break;
    case CString:
	{
	    TQCString* x = new TQCString;
	    s >> *x;
	    d->value.ptr = x;
	}
	break;
#ifndef TQT_NO_STRINGLIST
    case StringList:
	{
	    TQStringList* x = new TQStringList;
	    s >> *x;
	    d->value.ptr = x;
	    d->is_null = false;
	}
	break;
#endif // TQT_NO_STRINGLIST
    case Font:
	{
	    TQFont* x = new TQFont;
	    s >> *x;
	    d->value.ptr = x;
	    d->is_null = false;
	}
	break;
    case Pixmap:
	{
	    TQPixmap* x = new TQPixmap;
#ifndef TQT_NO_IMAGEIO
	    s >> *x;
#endif
	    d->value.ptr = x;
	}
	break;
    case Image:
	{
	    TQImage* x = new TQImage;
#ifndef TQT_NO_IMAGEIO
	    s >> *x;
#endif
	    d->value.ptr = x;
	}
	break;
    case Brush:
	{
	    TQBrush* x = new TQBrush;
	    s >> *x;
	    d->value.ptr = x;
	    d->is_null = false;
	}
	break;
    case Rect:
	{
	    TQRect* x = new TQRect;
	    s >> *x;
	    d->value.ptr = x;
	}
	break;
    case Point:
	{
	    TQPoint* x = new TQPoint;
	    s >> *x;
	    d->value.ptr = x;
	}
	break;
    case Size:
	{
	    TQSize* x = new TQSize;
	    s >> *x;
	    d->value.ptr = x;
	}
	break;
    case Color:
	{
	    TQColor* x = new TQColor;
	    s >> *x;
	    d->value.ptr = x;
	    d->is_null = false;
	}
	break;
#ifndef TQT_NO_PALETTE
    case Palette:
	{
	    TQPalette* x = new TQPalette;
	    s >> *x;
	    d->value.ptr = x;
	    d->is_null = false;
	}
	break;
    case ColorGroup:
	{
	    TQColorGroup* x = new TQColorGroup;
	    s >> *x;
	    d->value.ptr = x;
	    d->is_null = false;
	}
	break;
#endif
#ifndef TQT_NO_ICONSET
    case IconSet:
	{
	    TQPixmap x;
	    s >> x;
	    d->value.ptr = new TQIconSet( x );
	}
	break;
#endif
    case Int:
	{
	    int x;
	    s >> x;
	    d->value.i = x;
	    d->is_null = false;
	}
	break;
    case UInt:
	{
	    uint x;
	    s >> x;
	    d->value.u = x;
	    d->is_null = false;
	}
	break;
    case LongLong:
	{
	    TQ_LLONG x;
	    s >> x;
	    d->value.ll = x;
	}
	break;
    case ULongLong:
	{
	    TQ_ULLONG x;
	    s >> x;
	    d->value.ull = x;
	}
	break;
    case Bool:
	{
	    TQ_INT8 x;
	    s >> x;
	    d->value.b = x;
	    d->is_null = false;
	}
	break;
    case Double:
	{
	    double x;
	    s >> x;
	    d->value.d = x;
	    d->is_null = false;
	}
	break;
    case SizePolicy:
	{
	    int h,v;
	    TQ_INT8 hfw;
	    s >> h >> v >> hfw;
	    d->value.ptr = new TQSizePolicy( (TQSizePolicy::SizeType)h,
					    (TQSizePolicy::SizeType)v,
					    (bool) hfw);
	    d->is_null = false;
	}
	break;
    case Date:
	{
	    TQDate* x = new TQDate;
	    s >> *x;
	    d->value.ptr = x;
	}
	break;
    case Time:
	{
	    TQTime* x = new TQTime;
	    s >> *x;
	    d->value.ptr = x;
	}
	break;
    case DateTime:
	{
	    TQDateTime* x = new TQDateTime;
	    s >> *x;
	    d->value.ptr = x;
	}
	break;
    case ByteArray:
	{
	    TQByteArray* x = new TQByteArray;
	    s >> *x;
	    d->value.ptr = x;
	}
	break;
    case BitArray:
	{
	    TQBitArray* x = new TQBitArray;
	    s >> *x;
	    d->value.ptr = x;
	}
	break;
#ifndef TQT_NO_ACCEL
    case KeySequence:
	{
	    TQKeySequence* x = new TQKeySequence;
	    s >> *x;
	    d->value.ptr = x;
	    d->is_null = false;
	}
	break;
#endif // TQT_NO_ACCEL
    case Pen:
	{
	    TQPen* x = new TQPen;
	    s >> *x;
	    d->value.ptr = x;
	    d->is_null = false;
	}
	break;
    }
    d->typ = t;
}

/*!
    Internal function for saving a variant to the stream \a s. Use the
    stream operators instead.

    \internal
*/
void TQVariant::save( TQDataStream& s ) const
{
    s << (TQ_UINT32)type();

    switch( d->typ ) {
    case Cursor:
	s << *((TQCursor*)d->value.ptr);
	break;
    case Bitmap:
#ifndef TQT_NO_IMAGEIO
	s << *((TQBitmap*)d->value.ptr);
#endif
	break;
    case PointArray:
	s << *((TQPointArray*)d->value.ptr);
	break;
    case Region:
	s << *((TQRegion*)d->value.ptr);
	break;
#ifndef TQT_NO_TEMPLATE_VARIANT
    case List:
	s << *((TQValueList<TQVariant>*)d->value.ptr);
	break;
    case Map:
	s << *((TQMap<TQString,TQVariant>*)d->value.ptr);
	break;
#endif
    case String:
	s << *((TQString*)d->value.ptr);
	break;
    case CString:
	s << *((TQCString*)d->value.ptr);
	break;
#ifndef TQT_NO_STRINGLIST
    case StringList:
	s << *((TQStringList*)d->value.ptr);
	break;
#endif
    case Font:
	s << *((TQFont*)d->value.ptr);
	break;
    case Pixmap:
#ifndef TQT_NO_IMAGEIO
	s << *((TQPixmap*)d->value.ptr);
#endif
	break;
    case Image:
#ifndef TQT_NO_IMAGEIO
	s << *((TQImage*)d->value.ptr);
#endif
	break;
    case Brush:
	s << *((TQBrush*)d->value.ptr);
	break;
    case Point:
	s << *((TQPoint*)d->value.ptr);
	break;
    case Rect:
	s << *((TQRect*)d->value.ptr);
	break;
    case Size:
	s << *((TQSize*)d->value.ptr);
	break;
    case Color:
	s << *((TQColor*)d->value.ptr);
	break;
#ifndef TQT_NO_PALETTE
    case Palette:
	s << *((TQPalette*)d->value.ptr);
	break;
    case ColorGroup:
	s << *((TQColorGroup*)d->value.ptr);
	break;
#endif
#ifndef TQT_NO_ICONSET
    case IconSet:
	//### add stream operator to iconset
	s << ((TQIconSet*)d->value.ptr)->pixmap();
	break;
#endif
    case Int:
	s << d->value.i;
	break;
    case UInt:
	s << d->value.u;
	break;
    case LongLong:
	s << d->value.ll;
	break;
    case ULongLong:
	s << d->value.ull;
	break;
    case Bool:
	s << (TQ_INT8)d->value.b;
	break;
    case Double:
	s << d->value.d;
	break;
    case SizePolicy:
	{
	    TQSizePolicy p = toSizePolicy();
	    s << (int) p.horData() << (int) p.verData()
	      << (TQ_INT8) p.hasHeightForWidth();
	}
	break;
    case Date:
	s << *((TQDate*)d->value.ptr);
	break;
    case Time:
	s << *((TQTime*)d->value.ptr);
	break;
    case DateTime:
	s << *((TQDateTime*)d->value.ptr);
	break;
    case ByteArray:
	s << *((TQByteArray*)d->value.ptr);
	break;
    case BitArray:
	s << *((TQBitArray*)d->value.ptr);
	break;
#ifndef TQT_NO_ACCEL
    case KeySequence:
	s << *((TQKeySequence*)d->value.ptr);
	break;
#endif
    case Pen:
	s << *((TQPen*)d->value.ptr);
	break;
    case Invalid:
	s << TQString(); // ### looks wrong.
	break;
    }
}

/*!
    Reads a variant \a p from the stream \a s.

    \sa \link datastreamformat.html Format of the TQDataStream
    operators \endlink
*/
TQDataStream& operator>> ( TQDataStream& s, TQVariant& p )
{
    p.load( s );
    return s;
}

/*!
    Writes a variant \a p to the stream \a s.

    \sa \link datastreamformat.html Format of the TQDataStream
    operators \endlink
*/
TQDataStream& operator<< ( TQDataStream& s, const TQVariant& p )
{
    p.save( s );
    return s;
}

/*!
    Reads a variant type \a p in enum representation from the stream \a s.
*/
TQDataStream& operator>> ( TQDataStream& s, TQVariant::Type& p )
{
    TQ_UINT32 u;
    s >> u;
    p = (TQVariant::Type) u;

    return s;
}

/*!
    Writes a variant type \a p to the stream \a s.
*/
TQDataStream& operator<< ( TQDataStream& s, const TQVariant::Type p )
{
    s << (TQ_UINT32)p;

    return s;
}

#endif //TQT_NO_DATASTREAM

/*!
    \fn Type TQVariant::type() const

    Returns the storage type of the value stored in the variant.
    Usually it's best to test with canCast() whether the variant can
    deliver the data type you are interested in.
*/

/*!
    \fn bool TQVariant::isValid() const

    Returns true if the storage type of this variant is not
    TQVariant::Invalid; otherwise returns false.
*/

/*!
    \fn TQValueListConstIterator<TQString> TQVariant::stringListBegin() const
    \obsolete

    Returns an iterator to the first string in the list if the
    variant's type is StringList; otherwise returns a null iterator.
*/

/*!
    \fn TQValueListConstIterator<TQString> TQVariant::stringListEnd() const
    \obsolete

    Returns the end iterator for the list if the variant's type is
    StringList; otherwise returns a null iterator.
*/

/*!
    \fn TQValueListConstIterator<TQVariant> TQVariant::listBegin() const
    \obsolete

    Returns an iterator to the first item in the list if the variant's
    type is appropriate; otherwise returns a null iterator.
*/

/*!
    \fn TQValueListConstIterator<TQVariant> TQVariant::listEnd() const
    \obsolete

    Returns the end iterator for the list if the variant's type is
    appropriate; otherwise returns a null iterator.
*/

/*!
    \fn TQMapConstIterator<TQString, TQVariant> TQVariant::mapBegin() const
    \obsolete

    Returns an iterator to the first item in the map, if the variant's
    type is appropriate; otherwise returns a null iterator.
*/

/*!
    \fn TQMapConstIterator<TQString, TQVariant> TQVariant::mapEnd() const
    \obsolete

    Returns the end iterator for the map, if the variant's type is
    appropriate; otherwise returns a null iterator.
*/

/*!
    \fn TQMapConstIterator<TQString, TQVariant> TQVariant::mapFind( const TQString& key ) const
    \obsolete

    Returns an iterator to the item in the map with \a key as key, if
    the variant's type is appropriate and \a key is a valid key;
    otherwise returns a null iterator.
*/

/*!
    Returns the variant as a TQString if the variant can be cast to
    String, otherwise returns TQString::null.

    \sa asString(), canCast()
*/
const TQString TQVariant::toString() const
{
    switch( d->typ ) {
    case CString:
	return TQString::fromLatin1( toCString() );
    case Int:
	return TQString::number( toInt() );
    case UInt:
	return TQString::number( toUInt() );
    case LongLong:
	return TQString::number( toLongLong() );
    case ULongLong:
	return TQString::number( toULongLong() );
    case Double:
	return TQString::number( toDouble(), 'g', DBL_DIG );
#if !defined(TQT_NO_SPRINTF) && !defined(TQT_NO_DATESTRING)
    case Date:
	return toDate().toString( TQt::ISODate );
    case Time:
	return toTime().toString( TQt::ISODate );
    case DateTime:
	return toDateTime().toString( TQt::ISODate );
#endif
    case Bool:
	return toInt() ? "true" : "false";
#ifndef TQT_NO_ACCEL
    case KeySequence:
	return (TQString) *( (TQKeySequence*)d->value.ptr );
#endif
    case ByteArray:
	return TQString( *((TQByteArray*)d->value.ptr) );
    case Font:
	return toFont().toString();
    case Color:
	return toColor().name();
    case String:
	return *((TQString*)d->value.ptr);
    default:
	return TQString::null;
    }
}
/*!
    Returns the variant as a TQCString if the variant can be cast to a
    CString; otherwise returns 0.

    \sa asCString(), canCast()
*/
const TQCString TQVariant::toCString() const
{
    switch( d->typ ) {
    case CString: return *((TQCString*)d->value.ptr);
    case String: return ((TQString*)d->value.ptr)->latin1();
    default: {
	if (!canCast(String))
	    return 0;
	TQString c = toString();
	return TQCString(c.latin1());
    }
    }
}


#ifndef TQT_NO_STRINGLIST
/*!
    Returns the variant as a TQStringList if the variant has type()
    StringList or List of a type that can be converted to TQString;
    otherwise returns an empty list.

    Note that if you want to iterate over the list, you should iterate
    over a copy, e.g.
    \code
    TQStringList list = myVariant.toStringList();
    TQStringList::Iterator it = list.begin();
    while( it != list.end() ) {
	myProcessing( *it );
	++it;
    }
    \endcode

    \sa asStringList()
*/
const TQStringList TQVariant::toStringList() const
{
    switch ( d->typ ) {
    case StringList:
	return *((TQStringList*)d->value.ptr);
#ifndef TQT_NO_TEMPLATE_VARIANT
    case List:
	{
	    TQStringList lst;
	    TQValueList<TQVariant>::ConstIterator it = listBegin();
	    TQValueList<TQVariant>::ConstIterator end = listEnd();
	    while( it != end ) {
		TQString tmp = (*it).toString();
		++it;
		lst.append( tmp );
	    }
	    return lst;
	}
#endif
    default:
	return TQStringList();
    }
}
#endif //TQT_NO_STRINGLIST

#ifndef TQT_NO_TEMPLATE_VARIANT
/*!
    Returns the variant as a TQMap<TQString,TQVariant> if the variant has
    type() Map; otherwise returns an empty map.

    Note that if you want to iterate over the map, you should iterate
    over a copy, e.g.
    \code
    TQMap<TQString, TQVariant> map = myVariant.toMap();
    TQMap<TQString, TQVariant>::Iterator it = map.begin();
    while( it != map.end() ) {
	myProcessing( *it );
	++it;
    }
    \endcode

    \sa asMap()
*/
const TQMap<TQString, TQVariant> TQVariant::toMap() const
{
    if ( d->typ != Map )
	return TQMap<TQString,TQVariant>();

    return *((TQMap<TQString,TQVariant>*)d->value.ptr);
}
#endif
/*!
    Returns the variant as a TQFont if the variant can be cast to Font;
    otherwise returns the application's default font.

  \sa asFont(), canCast()
*/
const TQFont TQVariant::toFont() const
{
    switch ( d->typ ) {
    case CString:
    case ByteArray:
    case String:
	{
	    TQFont fnt;
	    fnt.fromString( toString() );
	    return fnt;
	}
    case Font:
	return *((TQFont*)d->value.ptr);
    default:
	return TQFont();
    }
}

/*!
    Returns the variant as a TQPixmap if the variant has type() Pixmap;
    otherwise returns a null pixmap.

    \sa asPixmap()
*/
const TQPixmap TQVariant::toPixmap() const
{
    if ( d->typ != Pixmap )
	return TQPixmap();

    return *((TQPixmap*)d->value.ptr);
}

/*!
    Returns the variant as a TQImage if the variant has type() Image;
    otherwise returns a null image.

    \sa asImage()
*/
const TQImage TQVariant::toImage() const
{
    if ( d->typ != Image )
	return TQImage();

    return *((TQImage*)d->value.ptr);
}

/*!
    Returns the variant as a TQBrush if the variant has type() Brush;
    otherwise returns a default brush (with all black colors).

    \sa asBrush()
*/
const TQBrush TQVariant::toBrush() const
{
    if( d->typ != Brush )
	return TQBrush();

    return *((TQBrush*)d->value.ptr);
}

/*!
    Returns the variant as a TQPoint if the variant has type() Point;
    otherwise returns a point (0, 0).

    \sa asPoint()
*/
const TQPoint TQVariant::toPoint() const
{
    if ( d->typ != Point )
	return TQPoint();

    return *((TQPoint*)d->value.ptr);
}

/*!
    Returns the variant as a TQRect if the variant has type() Rect;
    otherwise returns an empty rectangle.

    \sa asRect()
*/
const TQRect TQVariant::toRect() const
{
    if ( d->typ != Rect )
	return TQRect();

    return *((TQRect*)d->value.ptr);
}

/*!
    Returns the variant as a TQSize if the variant has type() Size;
    otherwise returns an invalid size.

    \sa asSize()
*/
const TQSize TQVariant::toSize() const
{
    if ( d->typ != Size )
	return TQSize();

    return *((TQSize*)d->value.ptr);
}

/*!
    Returns the variant as a TQColor if the variant can be cast to Color;
    otherwise returns an invalid color.

    \sa asColor(), canCast()
*/
const TQColor TQVariant::toColor() const
{
    switch ( d->typ ) {
    case ByteArray:
    case CString:
    case String:
    {
	TQColor col;
	col.setNamedColor( toString() );
	return col;
    }
    case Color:
	return *((TQColor*)d->value.ptr);
    default:
	return TQColor();
    }
}
#ifndef TQT_NO_PALETTE
/*!
    Returns the variant as a TQPalette if the variant has type()
    Palette; otherwise returns a completely black palette.

    \sa asPalette()
*/
const TQPalette TQVariant::toPalette() const
{
    if ( d->typ != Palette )
	return TQPalette();

    return *((TQPalette*)d->value.ptr);
}

/*!
    Returns the variant as a TQColorGroup if the variant has type()
    ColorGroup; otherwise returns a completely black color group.

    \sa asColorGroup()
*/
const TQColorGroup TQVariant::toColorGroup() const
{
    if ( d->typ != ColorGroup )
	return TQColorGroup();

    return *((TQColorGroup*)d->value.ptr);
}
#endif //TQT_NO_PALETTE
#ifndef TQT_NO_ICONSET
/*!
    Returns the variant as a TQIconSet if the variant has type()
    IconSet; otherwise returns an icon set of null pixmaps.

    \sa asIconSet()
*/
const TQIconSet TQVariant::toIconSet() const
{
    if ( d->typ != IconSet )
	return TQIconSet();

    return *((TQIconSet*)d->value.ptr);
}
#endif //TQT_NO_ICONSET
/*!
    Returns the variant as a TQPointArray if the variant has type()
    PointArray; otherwise returns an empty TQPointArray.

    \sa asPointArray()
*/
const TQPointArray TQVariant::toPointArray() const
{
    if ( d->typ != PointArray )
	return TQPointArray();

    return *((TQPointArray*)d->value.ptr);
}

/*!
    Returns the variant as a TQBitmap if the variant has type() Bitmap;
    otherwise returns a null TQBitmap.

    \sa asBitmap()
*/
const TQBitmap TQVariant::toBitmap() const
{
    if ( d->typ != Bitmap )
	return TQBitmap();

    return *((TQBitmap*)d->value.ptr);
}

/*!
    Returns the variant as a TQRegion if the variant has type() Region;
    otherwise returns an empty TQRegion.

    \sa asRegion()
*/
const TQRegion TQVariant::toRegion() const
{
    if ( d->typ != Region )
	return TQRegion();

    return *((TQRegion*)d->value.ptr);
}

/*!
    Returns the variant as a TQCursor if the variant has type() Cursor;
    otherwise returns the default arrow cursor.

    \sa asCursor()
*/
const TQCursor TQVariant::toCursor() const
{
#ifndef TQT_NO_CURSOR
    if ( d->typ != Cursor )
	return TQCursor();
#endif

    return *((TQCursor*)d->value.ptr);
}

/*!
    Returns the variant as a TQDate if the variant can be cast to Date;
    otherwise returns an invalid date.

    Note that if the type() is String, CString or ByteArray an invalid
    date will be returned if the string cannot be parsed as a
    TQt::ISODate format date.

    \sa asDate(), canCast()
*/
const TQDate TQVariant::toDate() const
{
    switch ( d->typ ) {
    case Date:
	return *((TQDate*)d->value.ptr);
    case DateTime:
	return ((TQDateTime*)d->value.ptr)->date();
#ifndef TQT_NO_DATESTRING
    case String:
	return TQDate::fromString( *((TQString*)d->value.ptr), TQt::ISODate );
    case CString:
    case ByteArray:
	return TQDate::fromString(toString(), TQt::ISODate);
#endif
    default:
	return TQDate();
    }
}

/*!
    Returns the variant as a TQTime if the variant can be cast to Time;
    otherwise returns an invalid date.

    Note that if the type() is String, CString or ByteArray an invalid
    time will be returned if the string cannot be parsed as a
    TQt::ISODate format time.

    \sa asTime()
*/
const TQTime TQVariant::toTime() const
{
    switch ( d->typ ) {
    case Time:
	return *((TQTime*)d->value.ptr);
    case DateTime:
	return ((TQDateTime*)d->value.ptr)->time();
#ifndef TQT_NO_DATESTRING
    case String:
	return TQTime::fromString( *((TQString*)d->value.ptr), TQt::ISODate );
    case CString:
    case ByteArray:
	return TQTime::fromString(toString(), TQt::ISODate);
#endif
    default:
	return TQTime();
    }
}

/*!
    Returns the variant as a TQDateTime if the variant can be cast to
    DateTime; otherwise returns an invalid TQDateTime.

    Note that if the type() is String, CString or ByteArray an invalid
    TQDateTime will be returned if the string cannot be parsed as a
    TQt::ISODate format date/time.

    \sa asDateTime()
*/
const TQDateTime TQVariant::toDateTime() const
{
    switch ( d->typ ) {
    case DateTime:
	return *((TQDateTime*)d->value.ptr);
#ifndef TQT_NO_DATESTRING
    case String:
	return TQDateTime::fromString( *((TQString*)d->value.ptr), TQt::ISODate );
    case CString:
    case ByteArray:
	return TQDateTime::fromString(toString(), TQt::ISODate);
#endif
    case Date:
	return TQDateTime( *((TQDate*)d->value.ptr) );
    default:
	return TQDateTime();
    }
}

/*!
    Returns the variant as a TQByteArray if the variant can be cast to
    a ByteArray; otherwise returns an empty bytearray.

    \sa asByteArray(), canCast()
*/
const TQByteArray TQVariant::toByteArray() const
{
    switch(d->typ) {
    case ByteArray: return *((TQByteArray*)d->value.ptr);
    case CString: return *((TQByteArray*)d->value.ptr);
    default: {
	TQByteArray ret;
	if (canCast(String)) {
	    TQString c = toString();
	    ret.duplicate(c.latin1(), c.length());
	}
	return ret;
    }
    }
}

/*!
    Returns the variant as a TQBitArray if the variant has type()
    BitArray; otherwise returns an empty bitarray.

    \sa asBitArray()
*/
const TQBitArray TQVariant::toBitArray() const
{
    if ( d->typ == BitArray )
	return *((TQBitArray*)d->value.ptr);
    return TQBitArray();
}

#ifndef TQT_NO_ACCEL

/*!
    Returns the variant as a TQKeySequence if the variant can be cast
    to a KeySequence; otherwise returns an empty key sequence.

    \sa asKeySequence(), canCast()
*/
const TQKeySequence TQVariant::toKeySequence() const
{
    switch ( d->typ ) {
    case KeySequence:
	return *((TQKeySequence*)d->value.ptr);
    case String:
    case ByteArray:
    case CString:
	return TQKeySequence( toString() );
    case Int:
    case UInt:
    case Double:
    case ULongLong:
    case LongLong:
	return TQKeySequence( toInt() );
    default:
	return TQKeySequence();
    }
}

#endif // TQT_NO_ACCEL

/*!
    Returns the variant as a TQPen if the variant has type()
    Pen; otherwise returns an empty TQPen.

    \sa asPen()
*/
const TQPen TQVariant::toPen() const
{
    if ( d->typ != Pen )
	return TQPen();

    return *((TQPen*)d->value.ptr);
}

/*!
    Returns the variant as an int if the variant can be cast to Int;
    otherwise returns 0.

    If \a ok is non-null: \a *ok is set to true if the value could be
    converted to an int; otherwise \a *ok is set to false.

    \sa asInt(), canCast()
*/
int TQVariant::toInt( bool * ok ) const
{
    if ( ok )
	*ok = canCast( Int );

    switch ( d->typ ) {
    case String:
	return ((TQString*)d->value.ptr)->toInt( ok );
    case CString:
    case ByteArray:
	return ((TQCString*)d->value.ptr)->toInt( ok );
    case Int:
	return d->value.i;
    case UInt:
	return (int)d->value.u;
    case LongLong:
        return (int)d->value.ll;
    case ULongLong:
        return (int)d->value.ull;
    case Double:
	return (int)d->value.d;
    case Bool:
	return (int)d->value.b;
#ifndef TQT_NO_ACCEL
    case KeySequence:
	return (int) *( (TQKeySequence*)d->value.ptr );
#endif
    default:
	return 0;
    }
}

/*!
    Returns the variant as an unsigned int if the variant can be cast
    to UInt; otherwise returns 0.

    If \a ok is non-null: \a *ok is set to true if the value could be
    converted to an unsigned int; otherwise \a *ok is set to false.

    \sa asUInt(), canCast()
*/
uint TQVariant::toUInt( bool * ok ) const
{
    if ( ok )
	*ok = canCast( UInt );

    switch( d->typ ) {
    case String:
	return ((TQString*)d->value.ptr)->toUInt( ok );
    case CString:
    case ByteArray:
	return ((TQCString*)d->value.ptr)->toUInt( ok );
    case Int:
	return (uint)d->value.i;
    case UInt:
	return d->value.u;
    case LongLong:
        return (uint)d->value.ll;
    case ULongLong:
        return (uint)d->value.ull;
    case Double:
	return (uint)d->value.d;
    case Bool:
	return (uint)d->value.b;
    default:
	return 0;
    }
}

/*!
    Returns the variant as a long long int if the variant can be cast
    to LongLong; otherwise returns 0.

    If \a ok is non-null: \a *ok is set to true if the value could be
    converted to an int; otherwise \a *ok is set to false.

    \sa asLongLong(), canCast()
*/
TQ_LLONG TQVariant::toLongLong( bool * ok ) const
{
    if ( ok )
	*ok = canCast( LongLong );

    switch ( d->typ ) {
    case String:
	return ((TQString*)d->value.ptr)->toLongLong( ok );
    case CString:
    case ByteArray:
	return TQString(*(TQCString*)d->value.ptr).toLongLong(ok);
    case Int:
	return (TQ_LLONG)d->value.i;
    case UInt:
	return (TQ_LLONG)d->value.u;
    case LongLong:
	return d->value.ll;
    case ULongLong:
	return (TQ_LLONG)d->value.ull;
    case Double:
	return (TQ_LLONG)d->value.d;
    case Bool:
	return (TQ_LLONG)d->value.b;
    default:
	return 0;
    }
}

/*!
    Returns the variant as as an unsigned long long int if the variant
    can be cast to ULongLong; otherwise returns 0.

    If \a ok is non-null: \a *ok is set to true if the value could be
    converted to an int; otherwise \a *ok is set to false.

    \sa asULongLong(), canCast()
*/
TQ_ULLONG TQVariant::toULongLong( bool * ok ) const
{
    if ( ok )
	*ok = canCast( ULongLong );

    switch ( d->typ ) {
    case Int:
	return (TQ_ULLONG)d->value.i;
    case UInt:
	return (TQ_ULLONG)d->value.u;
    case LongLong:
	return (TQ_ULLONG)d->value.ll;
    case ULongLong:
	return d->value.ull;
    case Double:
	return (TQ_ULLONG)d->value.d;
    case Bool:
	return (TQ_ULLONG)d->value.b;
    case String:
	return ((TQString*)d->value.ptr)->toULongLong( ok );
    case CString:
    case ByteArray:
	return TQString(*(TQCString*)d->value.ptr).toULongLong(ok);
    default:
	return 0;
    }
}

/*!
    Returns the variant as a bool if the variant can be cast to Bool;
    otherWise returns false.

    Returns true if the variant has a numeric type and its value is
    non-zero, or if the variant has type String, ByteArray or CString
    and its lower-case content is not empty, "0" or "false"; otherwise
    returns false.

    \sa asBool(), canCast()
*/
bool TQVariant::toBool() const
{
    switch( d->typ ) {
    case Bool:
	return d->value.b;
    case Double:
	return d->value.d != 0.0;
    case Int:
	return d->value.i != 0;
    case UInt:
	return d->value.u != 0;
    case LongLong:
	return d->value.ll != 0;
    case ULongLong:
	return d->value.ull != 0;
    case String:
    case CString:
    case ByteArray:
	{
	    TQString str = toString().lower();
	    return !(str == "0" || str == "false" || str.isEmpty() );
	}
    default:
	return false;
    }
}

/*!
    Returns the variant as a double if the variant can be cast to
    Double; otherwise returns 0.0.

    If \a ok is non-null: \a *ok is set to true if the value could be
    converted to a double; otherwise \a *ok is set to false.

    \sa asDouble(), canCast()
*/
double TQVariant::toDouble( bool * ok ) const
{
    if ( ok )
	*ok = canCast( Double );

    switch ( d->typ ) {
    case String:
	return ((TQString*)d->value.ptr)->toDouble( ok );
    case CString:
    case ByteArray:
	return ((TQCString*)d->value.ptr)->toDouble( ok );
    case Double:
	return d->value.d;
    case Int:
	return (double)d->value.i;
    case Bool:
	return (double)d->value.b;
    case UInt:
	return (double)d->value.u;
    case LongLong:
	return (double)d->value.ll;
    case ULongLong:
#if defined(Q_CC_MSVC) && !defined(Q_CC_MSVC_NET)
	return (double)(TQ_LLONG)d->value.ull;
#else
	return (double)d->value.ull;
#endif
    default:
	return 0.0;
    }
}

#ifndef TQT_NO_TEMPLATE_VARIANT
/*!
    Returns the variant as a TQValueList<TQVariant> if the variant has
    type() List or StringList; otherwise returns an empty list.

    Note that if you want to iterate over the list, you should iterate
    over a copy, e.g.
    \code
    TQValueList<TQVariant> list = myVariant.toList();
    TQValueList<TQVariant>::Iterator it = list.begin();
    while( it != list.end() ) {
	myProcessing( *it );
	++it;
    }
    \endcode

    \sa asList()
*/
const TQValueList<TQVariant> TQVariant::toList() const
{
    if ( d->typ == List )
	return *((TQValueList<TQVariant>*)d->value.ptr);
#ifndef TQT_NO_STRINGLIST
    if ( d->typ == StringList ) {
	TQValueList<TQVariant> lst;
	TQStringList::ConstIterator it = stringListBegin();
	TQStringList::ConstIterator end = stringListEnd();
	for( ; it != end; ++it )
	    lst.append( TQVariant( *it ) );
	return lst;
    }
#endif //TQT_NO_STRINGLIST
    return TQValueList<TQVariant>();
}
#endif

/*!
    Returns the variant as a TQSizePolicy if the variant has type()
    SizePolicy; otherwise returns an undefined (but legal) size
    policy.
*/

TQSizePolicy TQVariant::toSizePolicy() const
{
    if ( d->typ == SizePolicy )
	return *((TQSizePolicy*)d->value.ptr);

    return TQSizePolicy();
}


#define Q_VARIANT_AS( f ) TQ##f& TQVariant::as##f() \
{ \
    bool b = isNull(); \
    if ( d->typ != f ) \
	*this = TQVariant( to##f() ); \
    else \
	detach(); \
    d->is_null = b; \
    return *((TQ##f*)d->value.ptr); \
}

Q_VARIANT_AS(String)
Q_VARIANT_AS(CString)
#ifndef TQT_NO_STRINGLIST
Q_VARIANT_AS(StringList)
#endif
Q_VARIANT_AS(Font)
Q_VARIANT_AS(Pixmap)
Q_VARIANT_AS(Image)
Q_VARIANT_AS(Brush)
Q_VARIANT_AS(Point)
Q_VARIANT_AS(Rect)
Q_VARIANT_AS(Size)
Q_VARIANT_AS(Color)
#ifndef TQT_NO_PALETTE
Q_VARIANT_AS(Palette)
Q_VARIANT_AS(ColorGroup)
#endif
#ifndef TQT_NO_ICONSET
Q_VARIANT_AS(IconSet)
#endif
Q_VARIANT_AS(PointArray)
Q_VARIANT_AS(Bitmap)
Q_VARIANT_AS(Region)
Q_VARIANT_AS(Cursor)
Q_VARIANT_AS(SizePolicy)
Q_VARIANT_AS(Date)
Q_VARIANT_AS(Time)
Q_VARIANT_AS(DateTime)
Q_VARIANT_AS(ByteArray)
Q_VARIANT_AS(BitArray)
#ifndef TQT_NO_ACCEL
Q_VARIANT_AS(KeySequence)
#endif
Q_VARIANT_AS(Pen)

/*!
    \fn TQString& TQVariant::asString()

    Tries to convert the variant to hold a string value. If that is
    not possible the variant is set to an empty string.

    Returns a reference to the stored string.

    \sa toString()
*/

/*!
    \fn TQCString& TQVariant::asCString()

    Tries to convert the variant to hold a string value. If that is
    not possible the variant is set to an empty string.

    Returns a reference to the stored string.

    \sa toCString()
*/

/*!
    \fn TQStringList& TQVariant::asStringList()

    Tries to convert the variant to hold a TQStringList value. If that
    is not possible the variant is set to an empty string list.

    Returns a reference to the stored string list.

    Note that if you want to iterate over the list, you should
    iterate over a copy, e.g.
    \code
    TQStringList list = myVariant.asStringList();
    TQStringList::Iterator it = list.begin();
    while( it != list.end() ) {
	myProcessing( *it );
	++it;
    }
    \endcode

    \sa toStringList()
*/

/*!
    \fn TQFont& TQVariant::asFont()

    Tries to convert the variant to hold a TQFont. If that is not
    possible the variant is set to the application's default font.

    Returns a reference to the stored font.

    \sa toFont()
*/

/*!
    \fn TQPixmap& TQVariant::asPixmap()

    Tries to convert the variant to hold a pixmap value. If that is
    not possible the variant is set to a null pixmap.

    Returns a reference to the stored pixmap.

    \sa toPixmap()
*/

/*!
    \fn TQImage& TQVariant::asImage()

    Tries to convert the variant to hold an image value. If that is
    not possible the variant is set to a null image.

    Returns a reference to the stored image.

    \sa toImage()
*/

/*!
    \fn TQBrush& TQVariant::asBrush()

    Tries to convert the variant to hold a brush value. If that is not
    possible the variant is set to a default black brush.

    Returns a reference to the stored brush.

    \sa toBrush()
*/

/*!
    \fn TQPoint& TQVariant::asPoint()

    Tries to convert the variant to hold a point value. If that is not
    possible the variant is set to a (0, 0) point.

    Returns a reference to the stored point.

    \sa toPoint()
*/

/*!
    \fn TQRect& TQVariant::asRect()

    Tries to convert the variant to hold a rectangle value. If that is
    not possible the variant is set to an empty rectangle.

    Returns a reference to the stored rectangle.

    \sa toRect()
*/

/*!
    \fn TQSize& TQVariant::asSize()

    Tries to convert the variant to hold a TQSize value. If that is not
    possible the variant is set to an invalid size.

    Returns a reference to the stored size.

    \sa toSize() TQSize::isValid()
*/

/*!
    \fn TQSizePolicy& TQVariant::asSizePolicy()

    Tries to convert the variant to hold a TQSizePolicy value. If that
    fails, the variant is set to an arbitrary (valid) size policy.
*/


/*!
    \fn TQColor& TQVariant::asColor()

    Tries to convert the variant to hold a TQColor value. If that is
    not possible the variant is set to an invalid color.

    Returns a reference to the stored color.

    \sa toColor() TQColor::isValid()
*/

/*!
    \fn TQPalette& TQVariant::asPalette()

    Tries to convert the variant to hold a TQPalette value. If that is
    not possible the variant is set to a palette of black colors.

    Returns a reference to the stored palette.

    \sa toString()
*/

/*!
    \fn TQColorGroup& TQVariant::asColorGroup()

    Tries to convert the variant to hold a TQColorGroup value. If that
    is not possible the variant is set to a color group of all black
    colors.

    Returns a reference to the stored color group.

    \sa toColorGroup()
*/

/*!
    \fn TQIconSet& TQVariant::asIconSet()

    Tries to convert the variant to hold a TQIconSet value. If that is
    not possible the variant is set to an empty iconset.

    Returns a reference to the stored iconset.

    \sa toIconSet()
*/

/*!
    \fn TQPointArray& TQVariant::asPointArray()

    Tries to convert the variant to hold a TQPointArray value. If that
    is not possible the variant is set to an empty point array.

    Returns a reference to the stored point array.

    \sa toPointArray()
*/

/*!
    \fn TQBitmap& TQVariant::asBitmap()

    Tries to convert the variant to hold a bitmap value. If that is
    not possible the variant is set to a null bitmap.

    Returns a reference to the stored bitmap.

    \sa toBitmap()
*/

/*!
    \fn TQRegion& TQVariant::asRegion()

    Tries to convert the variant to hold a TQRegion value. If that is
    not possible the variant is set to a null region.

    Returns a reference to the stored region.

    \sa toRegion()
*/

/*!
    \fn TQCursor& TQVariant::asCursor()

    Tries to convert the variant to hold a TQCursor value. If that is
    not possible the variant is set to a default arrow cursor.

    Returns a reference to the stored cursor.

    \sa toCursor()
*/

/*!
    \fn TQDate& TQVariant::asDate()

    Tries to convert the variant to hold a TQDate value. If that is not
    possible then the variant is set to an invalid date.

    Returns a reference to the stored date.

    \sa toDate()
*/

/*!
    \fn TQTime& TQVariant::asTime()

    Tries to convert the variant to hold a TQTime value. If that is not
    possible then the variant is set to an invalid time.

    Returns a reference to the stored time.

    \sa toTime()
*/

/*!
    \fn TQDateTime& TQVariant::asDateTime()

    Tries to convert the variant to hold a TQDateTime value. If that is
    not possible then the variant is set to an invalid date/time.

    Returns a reference to the stored date/time.

    \sa toDateTime()
*/

/*!
    \fn TQByteArray& TQVariant::asByteArray()

    Tries to convert the variant to hold a TQByteArray value. If that
    is not possible then the variant is set to an empty bytearray.

    Returns a reference to the stored bytearray.

    \sa toByteArray()
*/

/*!
    \fn TQBitArray& TQVariant::asBitArray()

    Tries to convert the variant to hold a TQBitArray value. If that is
    not possible then the variant is set to an empty bitarray.

    Returns a reference to the stored bitarray.

    \sa toBitArray()
*/

/*!
    \fn TQKeySequence& TQVariant::asKeySequence()

    Tries to convert the variant to hold a TQKeySequence value. If that
    is not possible then the variant is set to an empty key sequence.

    Returns a reference to the stored key sequence.

    \sa toKeySequence()
*/

/*! \fn TQPen& TQVariant::asPen()

  Tries to convert the variant to hold a TQPen value. If that
  is not possible then the variant is set to an empty pen.

  Returns a reference to the stored pen.

  \sa toPen()
*/

/*!
    Returns the variant's value as int reference.
*/
int& TQVariant::asInt()
{
    detach();
    if ( d->typ != Int ) {
	int i = toInt();
	bool b = isNull();
	d->clear();
 	d->value.i = i;
	d->typ = Int;
	d->is_null = b;
    }
    return d->value.i;
}

/*!
    Returns the variant's value as unsigned int reference.
*/
uint& TQVariant::asUInt()
{
    detach();
    if ( d->typ != UInt ) {
	uint u = toUInt();
	bool b = isNull();
	d->clear();
	d->value.u = u;
	d->typ = UInt;
	d->is_null = b;
    }
    return d->value.u;
}

/*!
    Returns the variant's value as long long reference.
*/
TQ_LLONG& TQVariant::asLongLong()
{
    detach();
    if ( d->typ != LongLong ) {
	TQ_LLONG ll = toLongLong();
	bool b = isNull();
	d->clear();
 	d->value.ll = ll;
	d->typ = LongLong;
	d->is_null = b;
    }
    return d->value.ll;
}

/*!
    Returns the variant's value as unsigned long long reference.
*/
TQ_ULLONG& TQVariant::asULongLong()
{
    detach();
    if ( d->typ != ULongLong ) {
	TQ_ULLONG ull = toULongLong();
	bool b = isNull();
	d->clear();
 	d->value.ull = ull;
	d->typ = ULongLong;
	d->is_null = b;
    }
    return d->value.ull;
}

/*!
    Returns the variant's value as bool reference.
*/
bool& TQVariant::asBool()
{
    detach();
    if ( d->typ != Bool ) {
	bool b = toBool();
	bool nb = isNull();
	d->clear();
	d->value.b = b;
	d->typ = Bool;
	d->is_null = nb;
    }
    return d->value.b;
}

/*!
    Returns the variant's value as double reference.
*/
double& TQVariant::asDouble()
{
    detach();
    if ( d->typ != Double ) {
	double dbl = toDouble();
	bool b = isNull();
	d->clear();
	d->value.d = dbl;
	d->typ = Double;
	d->is_null = b;
    }
    return d->value.d;
}

#ifndef TQT_NO_TEMPLATE_VARIANT
/*!
    Returns the variant's value as variant list reference.

    Note that if you want to iterate over the list, you should iterate
    over a copy, e.g.
    \code
    TQValueList<TQVariant> list = myVariant.asList();
    TQValueList<TQVariant>::Iterator it = list.begin();
    while( it != list.end() ) {
	myProcessing( *it );
	++it;
    }
    \endcode
*/
TQValueList<TQVariant>& TQVariant::asList()
{
    bool b = isNull();
    if ( d->typ != List )
	*this = TQVariant( toList() );
    else
	detach();
    d->is_null = b;
    return *((TQValueList<TQVariant>*)d->value.ptr);
}

/*!
    Returns the variant's value as variant map reference.

    Note that if you want to iterate over the map, you should iterate
    over a copy, e.g.
    \code
    TQMap<TQString, TQVariant> map = myVariant.asMap();
    TQMap<TQString, TQVariant>::Iterator it = map.begin();
    while( it != map.end() ) {
	myProcessing( *it );
	++it;
    }
    \endcode
*/
TQMap<TQString, TQVariant>& TQVariant::asMap()
{
    bool b = isNull();
    if ( d->typ != Map )
	*this = TQVariant( toMap() );
    else
	detach();
    d->is_null = b;
    return *((TQMap<TQString,TQVariant>*)d->value.ptr);
}
#endif

/*!
    Returns true if the variant's type can be cast to the requested
    type, \a t. Such casting is done automatically when calling the
    toInt(), toBool(), ... or asInt(), asBool(), ... methods.

    The following casts are done automatically:
    \table
    \header \i Type \i Automatically Cast To
    \row \i Bool \i Double, Int, UInt, LongLong, ULongLong, String, CString, ByteArray
    \row \i Color \i String. CString. ByteArray
    \row \i Date \i String, CString, ByteArray, DateTime
    \row \i DateTime \i String, CString, ByteArray, Date, Time
    \row \i Double \i String, CString, ByteArray, Int, Bool, UInt, LongLong, ULongLong
    \row \i Font \i String, CString, ByteArray
    \row \i Int \i String, CString, ByteArray, Double, Bool, UInt, LongLong, ULongLong, KeySequence
    \row \i LongLong \i String, CString, ByteArray, Double, Bool, UInt, LongLong, ULongLong, KeySequence
    \row \i ULongLong \i String, CString, ByteArray, Double, Bool, UInt, LongLong, ULongLong, KeySequence
    \row \i List \i StringList (if the list contains only strings or
    something that can be cast to a string)
    \row \i String \i CString, ByteArray, CString, Int, UInt, Bool, Double, Date,
    Time, DateTime, KeySequence, Font, Color
    \row \i CString \i String, ByteArray, Int, UInt, Bool, Double, Date, ULongLong, LongLong
    \row \i ByteArray \i String, CString, Int, UInt, Bool, Double, Date, ULongLong, LongLong
    \row \i StringList \i List
    \row \i Time \i String
    \row \i Int \i String, CString, ByteArray, Double, Bool, UInt, LongLong, ULongLong, KeySequence
    \row \i KeySequence \i String, CString, ByteArray, Int, UInt, LongLong, ULongLong
    \endtable
*/
bool TQVariant::canCast( Type t ) const
{
    if ( Type( d->typ ) == t )
	return true;

    switch ( t ) {
    case Bool:
    case Double:
	if (d->typ == KeySequence)
	    break;
    case Int:
    case UInt:
    case LongLong:
    case ULongLong:
	switch(d->typ) {
	case Bool:
	case ByteArray:
	case CString:
	case Double:
	case Int:
	case KeySequence:
	case LongLong:
	case String:
	case UInt:
	case ULongLong:
	    return true;
	default: break;
	}
	break;

    case CString:
    case ByteArray:
    case String:
	switch(d->typ) {
	case Bool:
	case ByteArray:
	case CString:
	case Color:
	case Date:
	case DateTime:
	case Double:
	case Font:
	case Int:
	case KeySequence:
	case LongLong:
	case String:
	case Time:
	case UInt:
	case ULongLong:
	    return true;
	default: break;
	}
	break;

    case Time:
	if (d->typ == Date)
	    break;
    case Date:
    case DateTime:
	switch(d->typ) {
	case ByteArray:
	case CString:
	case Date:
	case DateTime:
	case String:
	    return true;
	default: break;
	}
	break;

    case KeySequence:
	switch(d->typ) {
	case ByteArray:
	case CString:
	case Int:
	case UInt:
	case LongLong:
	case ULongLong:
	case Double:
	case String:
	    return true;
	default: break;
	}
	break;

    case Font:
    case Color:
	switch(d->typ) {
	case ByteArray:
	case CString:
	case String:
	    return true;
	default: break;
	}
	break;

#ifndef TQT_NO_STRINGLIST
    case List:
	return d->typ == StringList;
#endif
#ifndef TQT_NO_TEMPLATE_VARIANT
    case StringList:
	if ( d->typ == List ) {
	    TQValueList<TQVariant>::ConstIterator it = listBegin();
	    TQValueList<TQVariant>::ConstIterator end = listEnd();
	    for( ; it != end; ++it ) {
		if ( !(*it).canCast( String ) )
		    return false;
	    }
	    return true;
	}
#endif
    case Invalid:
    case Map:
    case Pixmap:
    case Brush:
    case Rect:
    case Size:
    case Palette:
    case ColorGroup:
    case IconSet:
    case Point:
    case Image:
    case PointArray:
    case Region:
    case Bitmap:
    case Cursor:
    case SizePolicy:
    case BitArray:
    case Pen:
	break;
    }
    return false;
}

/*!
    Casts the variant to the requested type. If the cast cannot be
    done, the variant is set to the default value of the requested
    type (e.g. an empty string if the requested type \a t is
    TQVariant::String, an empty point array if the requested type \a t
    is TQVariant::PointArray, etc). Returns true if the current type of
    the variant was successfully cast; otherwise returns false.

    \sa canCast()
*/

bool TQVariant::cast( Type t )
{
    switch ( t ) {
#ifndef TQT_NO_TEMPLATE_VARIANT
    case TQVariant::Map:
	asMap();
	break;
    case TQVariant::List:
	asList();
	break;
#endif
    case TQVariant::String:
	asString();
	break;
#ifndef TQT_NO_STRINGLIST
    case TQVariant::StringList:
	asStringList();
	break;
#endif
    case TQVariant::Font:
	asFont();
	break;
    case TQVariant::Pixmap:
	asPixmap();
	break;
    case TQVariant::Brush:
	asBrush();
	break;
    case TQVariant::Rect:
	asRect();
	break;
    case TQVariant::Size:
	asSize();
	break;
    case TQVariant::Color:
	asColor();
	break;
#ifndef TQT_NO_PALETTE
    case TQVariant::Palette:
	asPalette();
	break;
    case TQVariant::ColorGroup:
	asColorGroup();
	break;
#endif
#ifndef TQT_NO_ICONSET
    case TQVariant::IconSet:
	asIconSet();
	break;
#endif
    case TQVariant::Point:
	asPoint();
	break;
    case TQVariant::Image:
	asImage();
	break;
    case TQVariant::Int:
	asInt();
	break;
    case TQVariant::UInt:
	asUInt();
	break;
    case TQVariant::Bool:
	asBool();
	break;
    case TQVariant::Double:
	asDouble();
	break;
    case TQVariant::CString:
	asCString();
	break;
    case TQVariant::PointArray:
	asPointArray();
	break;
    case TQVariant::Region:
	asRegion();
	break;
    case TQVariant::Bitmap:
	asBitmap();
	break;
    case TQVariant::Cursor:
	asCursor();
	break;
    case TQVariant::SizePolicy:
	asSizePolicy();
	break;
    case TQVariant::Date:
	asDate();
	break;
    case TQVariant::Time:
	asTime();
	break;
    case TQVariant::DateTime:
	asDateTime();
	break;
    case TQVariant::ByteArray:
	asByteArray();
	break;
    case TQVariant::BitArray:
	asBitArray();
	break;
#ifndef TQT_NO_ACCEL
    case TQVariant::KeySequence:
	asKeySequence();
	break;
#endif
    case TQVariant::Pen:
	asPen();
	break;
    case TQVariant::LongLong:
	asLongLong();
	break;
    case TQVariant::ULongLong:
	asULongLong();
	break;
    default:
    case TQVariant::Invalid:
	(*this) = TQVariant();
    }
    return canCast( t );
}

/*!
    Compares this TQVariant with \a v and returns true if they are
    equal; otherwise returns false.
*/

bool TQVariant::operator==( const TQVariant &v ) const
{
    if (isNumeric(v.type()) && canCast(v.type())) {
	bool ok;
	switch(v.type()) {
	case Bool:
	    return toBool() == v.toBool();
	case Int:
	{
	    int val = toInt(&ok);
	    return (ok && val == v.toInt());
	}
	case UInt:
	{
	    uint val = toUInt(&ok);
	    return (ok && val == v.toUInt());
	}

	case Double:
	{
	    double val = toDouble(&ok);
	    return (ok && val == v.toDouble());
	}

	case LongLong:
	{
	    TQ_LLONG val = toLongLong(&ok);
	    return (ok && val == v.toLongLong());
	}

	case ULongLong:
	{
	    TQ_ULLONG val = toULongLong(&ok);
	    return (ok && val == v.toULongLong());
	}

	default:
	    Q_ASSERT(false);
	}
    }

    if (!v.canCast(d->typ)) {
	return false;
    }

    switch( d->typ ) {
    case Cursor:
#ifndef TQT_NO_CURSOR
	return v.toCursor().shape() == toCursor().shape();
#endif
    case Bitmap:
	return v.toBitmap().serialNumber() == toBitmap().serialNumber();
    case PointArray:
	return v.toPointArray() == toPointArray();
    case Region:
	return v.toRegion() == toRegion();
#ifndef TQT_NO_TEMPLATE_VARIANT
    case List:
	return v.toList() == toList();
    case Map: {
	if ( v.toMap().count() != toMap().count() )
	    return false;
	TQMap<TQString, TQVariant>::ConstIterator it = v.toMap().begin();
	TQMap<TQString, TQVariant>::ConstIterator it2 = toMap().begin();
	while ( it != v.toMap().end() ) {
	    if ( *it != *it2 )
		return false;
	    ++it;
	    ++it2;
	}
	return true;
    }
#endif
    case String:
	return v.toString() == toString();
    case CString:
	return v.toCString() == toCString();
#ifndef TQT_NO_STRINGLIST
    case StringList:
	return v.toStringList() == toStringList();
#endif
    case Font:
	return v.toFont() == toFont();
    case Pixmap:
	return v.toPixmap().serialNumber() == toPixmap().serialNumber();
    case Image:
	return v.toImage() == toImage();
    case Brush:
	return v.toBrush() == toBrush();
    case Point:
	return v.toPoint() == toPoint();
    case Rect:
	return v.toRect() == toRect();
    case Size:
	return v.toSize() == toSize();
    case Color:
	return v.toColor() == toColor();
#ifndef TQT_NO_PALETTE
    case Palette:
	return v.toPalette() == toPalette();
    case ColorGroup:
	return v.toColorGroup() == toColorGroup();
#endif
#ifndef TQT_NO_ICONSET
    case IconSet:
	return v.toIconSet().pixmap().serialNumber()
	    == toIconSet().pixmap().serialNumber();
#endif
    case Int:
	return v.toInt() == toInt();
    case UInt:
	return v.toUInt() == toUInt();
    case LongLong:
	return v.toLongLong() == toLongLong();
    case ULongLong:
	return v.toULongLong() == toULongLong();
    case Bool:
	return v.toBool() == toBool();
    case Double:
	return v.toDouble() == toDouble();
    case SizePolicy:
	return v.toSizePolicy() == toSizePolicy();
    case Date:
	return v.toDate() == toDate();
    case Time:
	return v.toTime() == toTime();
    case DateTime:
	return v.toDateTime() == toDateTime();
    case ByteArray:
	return v.toByteArray() == toByteArray();
    case BitArray:
	return v.toBitArray() == toBitArray();
#ifndef TQT_NO_ACCEL
    case KeySequence:
	return v.toKeySequence() == toKeySequence();
#endif
    case Pen:
	return v.toPen() == toPen();
    case Invalid:
	break;
    }
    return false;
}

/*!
    Compares this TQVariant with \a v and returns true if they are not
    equal; otherwise returns false.
*/

bool TQVariant::operator!=( const TQVariant &v ) const
{
    return !( v == *this );
}


/*! \internal

  Reads or sets the variant type and ptr
 */
void* TQVariant::rawAccess( void* ptr, Type typ, bool deepCopy )
{
    if ( ptr ) {
	clear();
	d->typ = typ;
	d->value.ptr = ptr;
	d->is_null = false;
	if ( deepCopy ) {
	    TQVariant::Private* p = new Private( d );
	    d->typ = Invalid;
	    delete d;
	    d = p;
	}
    }

    if ( !deepCopy )
	return d->value.ptr;
    TQVariant::Private* p = new Private( d );
    void *ret = (void*)p->value.ptr;
    p->typ = Invalid;
    delete p;
    return ret;
}

/*!
  Returns true if this is a NULL variant, false otherwise.
*/
bool TQVariant::isNull() const
{
    switch( d->typ )
	{
	case TQVariant::Bitmap:
	    return ((TQBitmap*) d->value.ptr)->isNull();
	case TQVariant::Region:
	    return ((TQRegion*) d->value.ptr)->isNull();
	case TQVariant::PointArray:
	    return ((TQPointArray*) d->value.ptr)->isNull();
	case TQVariant::String:
	    return ((TQString*) d->value.ptr)->isNull();
	case TQVariant::CString:
	    return ((TQCString*) d->value.ptr)->isNull();
	case TQVariant::Pixmap:
	    return ((TQPixmap*) d->value.ptr)->isNull();
	case TQVariant::Image:
	    return ((TQImage*) d->value.ptr)->isNull();
	case TQVariant::Point:
	    return ((TQPoint*) d->value.ptr)->isNull();
	case TQVariant::Rect:
	    return ((TQRect*) d->value.ptr)->isNull();
	case TQVariant::Size:
	    return ((TQSize*) d->value.ptr)->isNull();
#ifndef TQT_NO_ICONSET
	case TQVariant::IconSet:
	    return ((TQIconSet*) d->value.ptr)->isNull();
#endif
	case TQVariant::Date:
	    return ((TQDate*) d->value.ptr)->isNull();
	case TQVariant::Time:
	    return ((TQTime*) d->value.ptr)->isNull();
	case TQVariant::DateTime:
	    return ((TQDateTime*) d->value.ptr)->isNull();
	case TQVariant::ByteArray:
	    return ((TQByteArray*) d->value.ptr)->isNull();
	case TQVariant::BitArray:
	    return ((TQBitArray*) d->value.ptr)->isNull();
	case TQVariant::Cursor:
#ifndef TQT_NO_STRINGLIST
	case TQVariant::StringList:
#endif //TQT_NO_STRINGLIST
	case TQVariant::Font:
	case TQVariant::Brush:
	case TQVariant::Color:
#ifndef TQT_NO_PALETTE
	case TQVariant::Palette:
	case TQVariant::ColorGroup:
#endif
#ifndef TQT_NO_TEMPLATE_VARIANT
	case TQVariant::Map:
	case TQVariant::List:
#endif
	case TQVariant::SizePolicy:
#ifndef TQT_NO_ACCEL
	case TQVariant::KeySequence:
#endif
	case TQVariant::Pen:
	case TQVariant::Invalid:
	case TQVariant::Int:
	case TQVariant::UInt:
	case TQVariant::LongLong:
	case TQVariant::ULongLong:
	case TQVariant::Bool:
	case TQVariant::Double:
	    break;
	}
    return d->is_null;
}
#endif //TQT_NO_VARIANT
