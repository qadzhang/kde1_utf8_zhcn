/****************************************************************************
**
** Definition of TQVariant class
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

#ifndef TQVARIANT_H
#define TQVARIANT_H

#ifndef QT_H
#include "ntqmap.h"
#include "ntqstring.h"
#include "ntqstringlist.h"
#include "ntqvaluelist.h"
#endif // QT_H

#ifndef TQT_NO_VARIANT
class TQString;
class TQCString;
class TQFont;
class TQPixmap;
class TQBrush;
class TQRect;
class TQPoint;
class TQImage;
class TQSize;
class TQColor;
class TQPalette;
class TQColorGroup;
class TQIconSet;
class TQDataStream;
class TQPointArray;
class TQRegion;
class TQBitmap;
class TQCursor;
class TQStringList;
class TQSizePolicy;
class TQDate;
class TQTime;
class TQDateTime;
class TQBitArray;
class TQKeySequence;
class TQPen;
// Some headers rejected after TQVariant declaration for GCC 2.7.* compatibility
class TQVariant;
#ifndef TQT_NO_TEMPLATE_VARIANT
template <class T> class TQValueList;
template <class T> class TQValueListConstIterator;
template <class T> class TQValueListNode;
template <class Key, class T> class TQMap;
template <class Key, class T> class TQMapConstIterator;

typedef TQMap<TQString, TQVariant> TQStringVariantMap;
typedef TQMapIterator<TQString, TQVariant> TQStringVariantMapIterator;
typedef TQMapConstIterator<TQString, TQVariant> TQStringVariantMapConstIterator;
#endif

class TQ_EXPORT TQVariant
{
public:
    enum Type {
	Invalid,
	Map,
	List,
	String,
	StringList,
	Font,
	Pixmap,
	Brush,
	Rect,
	Size,
	Color,
	Palette,
	ColorGroup,
	IconSet,
	Point,
	Image,
	Int,
	UInt,
	Bool,
	Double,
	CString,
	PointArray,
	Region,
	Bitmap,
	Cursor,
	SizePolicy,
	Date,
	Time,
	DateTime,
	ByteArray,
	BitArray,
	KeySequence,
	Pen,
	LongLong,
	ULongLong
    };

    TQVariant();
    ~TQVariant();
    TQVariant( const TQVariant& );
#ifndef TQT_NO_DATASTREAM
    TQVariant( TQDataStream& s );
#endif
    TQVariant( const TQString& );
    TQVariant( const TQCString& );
    TQVariant( const char* );
#ifndef TQT_NO_STRINGLIST
    TQVariant( const TQStringList& );
#endif
    TQVariant( const TQFont& );
    TQVariant( const TQPixmap& );
    TQVariant( const TQImage& );
    TQVariant( const TQBrush& );
    TQVariant( const TQPoint& );
    TQVariant( const TQRect& );
    TQVariant( const TQSize& );
    TQVariant( const TQColor& );
    TQVariant( const TQPalette& );
    TQVariant( const TQColorGroup& );
    TQVariant( const TQIconSet& );
    TQVariant( const TQPointArray& );
    TQVariant( const TQRegion& );
    TQVariant( const TQBitmap& );
    TQVariant( const TQCursor& );
    TQVariant( const TQDate& );
    TQVariant( const TQTime& );
    TQVariant( const TQDateTime& );
    TQVariant( const TQByteArray& );
    TQVariant( const TQBitArray& );
#ifndef TQT_NO_ACCEL
    TQVariant( const TQKeySequence& );
#endif
    TQVariant( const TQPen& );
#ifndef TQT_NO_TEMPLATE_VARIANT
    TQVariant( const TQValueList<TQVariant>& );
    TQVariant( const TQMap<TQString,TQVariant>& );
#endif
    TQVariant( int );
    TQVariant( uint );
    TQVariant( TQ_LLONG );
    TQVariant( TQ_ULLONG );
    TQVariant( bool );
    TQVariant( double );
    TQVariant( TQSizePolicy );

    TQVariant& operator= ( const TQVariant& );
    bool operator==( const TQVariant& ) const;
    bool operator!=( const TQVariant& ) const;

    Type type() const;
    const char* typeName() const;

    bool canCast( Type ) const;
    bool cast( Type );

    bool isValid() const;
    bool isNull() const;

    void clear();

    const TQString toString() const;
    const TQCString toCString() const;
#ifndef TQT_NO_STRINGLIST
    const TQStringList toStringList() const;
#endif
    const TQFont toFont() const;
    const TQPixmap toPixmap() const;
    const TQImage toImage() const;
    const TQBrush toBrush() const;
    const TQPoint toPoint() const;
    const TQRect toRect() const;
    const TQSize toSize() const;
    const TQColor toColor() const;
    const TQPalette toPalette() const;
    const TQColorGroup toColorGroup() const;
    const TQIconSet toIconSet() const;
    const TQPointArray toPointArray() const;
    const TQBitmap toBitmap() const;
    const TQRegion toRegion() const;
    const TQCursor toCursor() const;
    const TQDate toDate() const;
    const TQTime toTime() const;
    const TQDateTime toDateTime() const;
    const TQByteArray toByteArray() const;
    const TQBitArray toBitArray() const;
#ifndef TQT_NO_ACCEL
    const TQKeySequence toKeySequence() const;
#endif
    const TQPen toPen() const;
    int toInt( bool * ok=0 ) const;
    uint toUInt( bool * ok=0 ) const;
    TQ_LLONG toLongLong( bool * ok=0 ) const;
    TQ_ULLONG toULongLong( bool * ok=0 ) const;
    bool toBool() const;
    double toDouble( bool * ok=0 ) const;
#ifndef TQT_NO_TEMPLATE_VARIANT
    const TQValueList<TQVariant> toList() const;
    const TQMap<TQString,TQVariant> toMap() const;
#endif
    TQSizePolicy toSizePolicy() const;

#ifndef TQT_NO_TEMPLATE_VARIANT
    TQValueListConstIterator<TQString> stringListBegin() const;
    TQValueListConstIterator<TQString> stringListEnd() const;
    TQValueListConstIterator<TQVariant> listBegin() const;
    TQValueListConstIterator<TQVariant> listEnd() const;
    TQMapConstIterator<TQString,TQVariant> mapBegin() const;
    TQMapConstIterator<TQString,TQVariant> mapEnd() const;
    TQMapConstIterator<TQString,TQVariant> mapFind( const TQString& ) const;
#endif
    TQString& asString();
    TQCString& asCString();
#ifndef TQT_NO_STRINGLIST
    TQStringList& asStringList();
#endif
    TQFont& asFont();
    TQPixmap& asPixmap();
    TQImage& asImage();
    TQBrush& asBrush();
    TQPoint& asPoint();
    TQRect& asRect();
    TQSize& asSize();
    TQColor& asColor();
    TQPalette& asPalette();
    TQColorGroup& asColorGroup();
    TQIconSet& asIconSet();
    TQPointArray& asPointArray();
    TQBitmap& asBitmap();
    TQRegion& asRegion();
    TQCursor& asCursor();
    TQDate& asDate();
    TQTime& asTime();
    TQDateTime& asDateTime();
    TQByteArray& asByteArray();
    TQBitArray& asBitArray();
#ifndef TQT_NO_ACCEL
    TQKeySequence& asKeySequence();
#endif
    TQPen& asPen();
    int& asInt();
    uint& asUInt();
    TQ_LLONG& asLongLong();
    TQ_ULLONG& asULongLong();
    bool& asBool();
    double& asDouble();
#ifndef TQT_NO_TEMPLATE_VARIANT
    TQValueList<TQVariant>& asList();
    TQMap<TQString,TQVariant>& asMap();
#endif
    TQSizePolicy& asSizePolicy();

#ifndef TQT_NO_DATASTREAM
    void load( TQDataStream& );
    void save( TQDataStream& ) const;
#endif
    static const char* typeToName( Type typ );
    static Type nameToType( const char* name );

private:
    void detach();

    class Private : public TQShared
    {
    public:
	Private();
	Private( Private* );
	~Private();

	void clear();

	Type typ;
	union
	{
	    uint u;
	    int i;
	    TQ_LLONG ll;
	    TQ_ULLONG ull;
	    bool b;
	    double d;
	    void *ptr;
	} value;
	uint is_null : 1; // ## 4.0 merge with typ
    };

    Private* d;

public:
    void* rawAccess( void* ptr = 0, Type typ = Invalid, bool deepCopy = false );
};

inline TQVariant::Type TQVariant::type() const
{
    return d->typ;
}

inline bool TQVariant::isValid() const
{
    return (d->typ != Invalid);
}

#ifndef TQT_NO_TEMPLATE_VARIANT
inline TQValueListConstIterator<TQString> TQVariant::stringListBegin() const
{
    if ( d->typ != StringList )
	return TQValueListConstIterator<TQString>();
    return ((const TQStringList*)d->value.ptr)->begin();
}

inline TQValueListConstIterator<TQString> TQVariant::stringListEnd() const
{
    if ( d->typ != StringList )
	return TQValueListConstIterator<TQString>();
    return ((const TQStringList*)d->value.ptr)->end();
}

inline TQValueListConstIterator<TQVariant> TQVariant::listBegin() const
{
    if ( d->typ != List )
	return TQValueListConstIterator<TQVariant>();
    return ((const TQValueList<TQVariant>*)d->value.ptr)->begin();
}

inline TQValueListConstIterator<TQVariant> TQVariant::listEnd() const
{
    if ( d->typ != List )
	return TQValueListConstIterator<TQVariant>();
    return ((const TQValueList<TQVariant>*)d->value.ptr)->end();
}

inline TQMapConstIterator<TQString,TQVariant> TQVariant::mapBegin() const
{
    if ( d->typ != Map )
	return TQMapConstIterator<TQString,TQVariant>();
    return ((const TQMap<TQString,TQVariant>*)d->value.ptr)->begin();
}

inline TQMapConstIterator<TQString,TQVariant> TQVariant::mapEnd() const
{
    if ( d->typ != Map )
	return TQMapConstIterator<TQString,TQVariant>();
    return ((const TQMap<TQString,TQVariant>*)d->value.ptr)->end();
}

inline TQMapConstIterator<TQString,TQVariant> TQVariant::mapFind( const TQString& key ) const
{
    if ( d->typ != Map )
	return TQMapConstIterator<TQString,TQVariant>();
    return ((const TQMap<TQString,TQVariant>*)d->value.ptr)->find( key );
}
#endif

#ifndef TQT_NO_DATASTREAM
TQ_EXPORT TQDataStream& operator>> ( TQDataStream& s, TQVariant& p );
TQ_EXPORT TQDataStream& operator<< ( TQDataStream& s, const TQVariant& p );
TQ_EXPORT TQDataStream& operator>> ( TQDataStream& s, TQVariant::Type& p );
TQ_EXPORT TQDataStream& operator<< ( TQDataStream& s, const TQVariant::Type p );
#endif

#endif //TQT_NO_VARIANT
#endif // TQVARIANT_H
