/****************************************************************************
**
** Definition of TQStrVec and TQStrIVec classes
**
** Created : 931203
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

#ifndef TQSTRVEC_H
#define TQSTRVEC_H

#ifndef QT_H
#include "ntqstring.h"
#include "ntqptrvector.h"
#include "ntqdatastream.h"
#endif // QT_H

class TQ_EXPORT TQStrVec : public TQPtrVector<char>
{
public:
    TQStrVec()  { dc = true; }
    TQStrVec( uint size, bool deepc = true ) : TQPtrVector<char>(size) {dc=deepc;}
   ~TQStrVec()  { clear(); }
private:
    Item	 newItem( Item d )	{ return dc ? tqstrdup( (const char*)d ) : d; }
    void deleteItem( Item d )	{ if ( dc ) delete[] (char*)d; }
    int	 compareItems( Item s1, Item s2 )
				{ return qstrcmp((const char*)s1,
						(const char*)s2); }
#ifndef TQT_NO_DATASTREAM
    TQDataStream &read( TQDataStream &s, Item &d )
				{ s >> (char *&)d; return s; }
    TQDataStream &write( TQDataStream &s, Item d ) const
				{ return s << (const char*)d; }
#endif
    bool dc;
};


class TQ_EXPORT TQStrIVec : public TQStrVec	// case insensitive string vec
{
public:
    TQStrIVec() {}
    TQStrIVec( uint size, bool dc = true ) : TQStrVec( size, dc ) {}
   ~TQStrIVec() { clear(); }
private:
    int	 compareItems( Item s1, Item s2 )
				{ return tqstricmp((const char*)s1,
						 (const char*)s2); }
};


#endif // TQSTRVEC_H
