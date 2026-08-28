/****************************************************************************
**
** Definition of the TQFontDatabase class
**
** Created : 981126
**
** Copyright (C) 1999-2008 Trolltech ASA.  All rights reserved.
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

#ifndef TQFONTDATABASE_H
#define TQFONTDATABASE_H

#ifndef QT_H
#include "ntqwindowdefs.h"
#include "ntqstring.h"
#include "ntqstringlist.h"
#include "ntqfont.h"
#include "ntqvaluelist.h"
#endif // QT_H


#ifndef TQT_NO_FONTDATABASE

class TQFontStylePrivate; /* Don't touch! */
struct TQtFontStyle;
struct TQtFontFamily;
struct TQtFontFoundry;
struct TQFontDef;
class TQFontEngine;
#ifdef TQ_WS_QWS
class TQDiskFont;
#endif

class TQFontDatabasePrivate;

class TQ_EXPORT TQFontDatabase
{
public:
    static TQValueList<int> standardSizes();

    TQFontDatabase();

    TQStringList families() const;
    TQStringList families( TQFont::Script ) const;
    TQStringList styles( const TQString & ) const;
    TQValueList<int> pointSizes( const TQString &, const TQString & = TQString::null);
    TQValueList<int> smoothSizes( const TQString &, const TQString &);
    TQString styleString( const TQFont &);

    TQFont font( const TQString &, const TQString &, int);

    bool isBitmapScalable( const TQString &, const TQString & = TQString::null) const;
    bool isSmoothlyScalable( const TQString &, const TQString & = TQString::null) const;
    bool isScalable( const TQString &, const TQString & = TQString::null) const;
    bool isFixedPitch( const TQString &, const TQString & = TQString::null) const;

    bool italic( const TQString &, const TQString &) const;
    bool bold( const TQString &, const TQString &) const;
    int weight( const TQString &, const TQString &) const;

    static TQString scriptName(TQFont::Script);
    static TQString scriptSample(TQFont::Script);

#ifdef TQ_WS_QWS
    static void qwsAddDiskFont( TQDiskFont *qdf );
#endif

    // For source compatibility with < 3.0
#ifndef TQT_NO_COMPAT

    TQStringList families(bool) const;
    TQStringList styles( const TQString &, const TQString & ) const;
    TQValueList<int> pointSizes( const TQString &, const TQString &, const TQString & );
    TQValueList<int> smoothSizes( const TQString &, const TQString &, const TQString & );

    TQFont font( const TQString &, const TQString &, int, const TQString &);

    bool isBitmapScalable( const TQString &, const TQString &, const TQString & ) const;
    bool isSmoothlyScalable( const TQString &, const TQString &, const TQString & ) const;
    bool isScalable( const TQString &, const TQString &, const TQString & ) const;
    bool isFixedPitch( const TQString &, const TQString &, const TQString & ) const;

    bool italic( const TQString &, const TQString &, const TQString & ) const;
    bool bold( const TQString &, const TQString &, const TQString & ) const;
    int weight( const TQString &, const TQString &, const TQString & ) const;

#endif // TQT_NO_COMPAT

private:
#if defined(TQ_WS_X11) || defined(TQ_WS_WIN)
    static TQFontEngine *findFont( TQFont::Script script, const TQFontPrivate *fp,
				  const TQFontDef &request, int force_encoding_id = -1 );
#endif // TQ_WS_X11

    static void createDatabase();

    static void parseFontName(const TQString &name, TQString &foundry, TQString &family);

    friend struct TQFontDef;
    friend class TQFontPrivate;
    friend class TQFontDialog;
    friend class TQFontEngineLatinXLFD;

    TQFontDatabasePrivate *d;
};


#ifndef TQT_NO_COMPAT

inline TQStringList TQFontDatabase::families( bool ) const
{
    return families();
}

inline TQStringList TQFontDatabase::styles( const TQString &family,
					  const TQString & ) const
{
    return styles(family);
}

inline TQValueList<int> TQFontDatabase::pointSizes( const TQString &family,
						  const TQString &style ,
						  const TQString & )
{
    return pointSizes(family, style);
}

inline TQValueList<int> TQFontDatabase::smoothSizes( const TQString &family,
						   const TQString &style,
						   const TQString & )
{
    return smoothSizes(family, style);
}

inline TQFont TQFontDatabase::font( const TQString &familyName,
				  const TQString &style,
				  int pointSize,
				  const TQString &)
{
    return font(familyName, style, pointSize);
}

inline bool TQFontDatabase::isBitmapScalable( const TQString &family,
					     const TQString &style,
					     const TQString & ) const
{
    return isBitmapScalable(family, style);
}

inline bool TQFontDatabase::isSmoothlyScalable( const TQString &family,
					       const TQString &style,
					       const TQString & ) const
{
    return isSmoothlyScalable(family, style);
}

inline bool TQFontDatabase::isScalable( const TQString &family,
				       const TQString &style,
				       const TQString & ) const
{
    return isScalable(family, style);
}

inline bool TQFontDatabase::isFixedPitch( const TQString &family,
					 const TQString &style,
					 const TQString & ) const
{
    return isFixedPitch(family, style);
}

inline bool TQFontDatabase::italic( const TQString &family,
				   const TQString &style,
				   const TQString & ) const
{
    return italic(family, style);
}

inline bool TQFontDatabase::bold( const TQString &family,
				 const TQString &style,
				 const TQString & ) const
{
    return bold(family, style);
}

inline int TQFontDatabase::weight( const TQString &family,
				  const TQString &style,
				  const TQString & ) const
{
    return weight(family, style);
}

#endif // TQT_NO_COMPAT

#endif // TQT_NO_FONTDATABASE

#endif // TQFONTDATABASE_H
