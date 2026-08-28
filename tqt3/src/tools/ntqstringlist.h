/****************************************************************************
**
** Definition of TQStringList class
**
** Created : 990406
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

#ifndef TQSTRINGLIST_H
#define TQSTRINGLIST_H

#ifndef QT_H
#include "ntqvaluelist.h"
#include "ntqstring.h"
#include "ntqstrlist.h"
#endif // QT_H

#ifndef TQT_NO_STRINGLIST

class TQRegExp;
template <class T> class TQDeepCopy;

#if defined(Q_TEMPLATEDLL)
// MOC_SKIP_BEGIN
//Q_TEMPLATE_EXTERN template class TQ_EXPORT TQValueList<TQString>; 
// MOC_SKIP_END
#endif

class TQ_EXPORT TQStringList : public TQValueList<TQString>
{
public:
    TQStringList() { }
    TQStringList( const TQStringList& l ) : TQValueList<TQString>(l) { }
    TQStringList( const TQValueList<TQString>& l ) : TQValueList<TQString>(l) { }
    TQStringList( const TQString& i ) { append(i); }
#ifndef TQT_NO_CAST_ASCII
    TQStringList( const char* i ) { append(i); }
#endif

    static TQStringList fromStrList(const TQStrList&);

    void sort();

    static TQStringList split( const TQString &sep, const TQString &str, bool allowEmptyEntries = false );
    static TQStringList split( const TQChar &sep, const TQString &str, bool allowEmptyEntries = false );
#ifndef TQT_NO_REGEXP
    static TQStringList split( const TQRegExp &sep, const TQString &str, bool allowEmptyEntries = false );
#endif
    TQString join( const TQString &sep ) const;

    TQStringList grep( const TQString &str, bool cs = true ) const;
#ifndef TQT_NO_REGEXP
    TQStringList grep( const TQRegExp &expr ) const;
#endif

    TQStringList& gres( const TQString &before, const TQString &after,
		       bool cs = true );
#ifndef TQT_NO_REGEXP_CAPTURE
    TQStringList& gres( const TQRegExp &expr, const TQString &after );
#endif

protected:
    void detach() { TQValueList<TQString>::detach(); }
    friend class TQDeepCopy< TQStringList >;
};

#ifndef TQT_NO_DATASTREAM
class TQDataStream;
extern TQ_EXPORT TQDataStream &operator>>( TQDataStream &, TQStringList& );
extern TQ_EXPORT TQDataStream &operator<<( TQDataStream &, const TQStringList& );
#endif

#endif // TQT_NO_STRINGLIST
#endif // TQSTRINGLIST_H
