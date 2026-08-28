/****************************************************************************
**
** Definition of TQRegExp class
**
** Created : 950126
**
** Copyright (C) 1992-2008 Trolltech ASA.  All rights reserved.
**
** This file is part of the tools module of the TQt GUI Toolkit.
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

#ifndef TQREGEXP_H
#define TQREGEXP_H

#ifndef QT_H
#include "ntqstringlist.h"
#endif // QT_H

#ifndef TQT_NO_REGEXP
class TQRegExpEngine;
struct TQRegExpPrivate;

class TQ_EXPORT TQRegExp
{
public:
    enum CaretMode { CaretAtZero, CaretAtOffset, CaretWontMatch };

    TQRegExp();
    TQRegExp( const TQString& pattern, bool caseSensitive = true,
	     bool wildcard = false );
    TQRegExp( const TQRegExp& rx );
    ~TQRegExp();
    TQRegExp& operator=( const TQRegExp& rx );

    bool operator==( const TQRegExp& rx ) const;
    bool operator!=( const TQRegExp& rx ) const { return !operator==( rx ); }

    bool isEmpty() const;
    bool isValid() const;
    TQString pattern() const;
    void setPattern( const TQString& pattern );
    bool caseSensitive() const;
    void setCaseSensitive( bool sensitive );
#ifndef TQT_NO_REGEXP_WILDCARD
    bool wildcard() const;
    void setWildcard( bool wildcard );
#endif
    bool minimal() const;
    void setMinimal( bool minimal );

    bool exactMatch( const TQString& str ) const;
#ifndef TQT_NO_COMPAT
    int match( const TQString& str, int index = 0, int *len = 0,
	       bool indexIsStart = true ) const;
#endif

#if defined(Q_QDOC)
    int search( const TQString& str, int offset = 0,
		CaretMode caretMode = CaretAtZero ) const;
    int searchRev( const TQString& str, int offset = -1,
		   CaretMode caretMode = CaretAtZero ) const;
#else
    // ### TQt 4.0: reduce these four to two functions
    int search( const TQString& str, int offset = 0 ) const;
    int search( const TQString& str, int offset, CaretMode caretMode ) const;
    int searchRev( const TQString& str, int offset = -1 ) const;
    int searchRev( const TQString& str, int offset, CaretMode caretMode ) const;
#endif
    int matchedLength() const;
#ifndef TQT_NO_REGEXP_CAPTURE
    int numCaptures() const;
    TQStringList capturedTexts();
    TQString cap( int nth = 0 );
    int pos( int nth = 0 );
    TQString errorString();
#endif

    static TQString escape( const TQString& str );

private:
    void prepareEngine() const;
    void prepareEngineForMatch( const TQString& str ) const;
    void invalidateEngine();

    static int caretIndex( int offset, CaretMode caretMode );

    TQRegExpEngine *eng;
    TQRegExpPrivate *priv;
};
#endif // TQT_NO_REGEXP
#endif // TQREGEXP_H
