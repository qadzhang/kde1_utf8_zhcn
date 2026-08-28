/****************************************************************************
**
** Definition of TQUuid class
**
** Created : 010523
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

#ifndef TQUUID_H
#define TQUUID_H

#ifndef QT_H
#include "ntqstring.h"
#endif // QT_H

#include <string.h>

#if defined(Q_OS_WIN32)
#ifndef GUID_DEFINED
#define GUID_DEFINED
typedef struct _GUID
{
    ulong   Data1;
    ushort  Data2;
    ushort  Data3;
    uchar   Data4[ 8 ];
} GUID, *REFGUID, *LPGUID;
#endif
#endif


struct TQ_EXPORT TQUuid
{
    enum Variant {
	VarUnknown	=-1,
	NCS		= 0, // 0 - -
	DCE		= 2, // 1 0 -
	Microsoft	= 6, // 1 1 0
	Reserved	= 7  // 1 1 1
    };

    enum Version {
	VerUnknown	=-1,
	Time		= 1, // 0 0 0 1
	EmbeddedPOSIX	= 2, // 0 0 1 0
	Name		= 3, // 0 0 1 1
	Random		= 4  // 0 1 0 0
    };

    TQUuid()
    {
	memset( this, 0, sizeof(TQUuid) );
    }
    TQUuid( uint l, ushort w1, ushort w2, uchar b1, uchar b2, uchar b3, uchar b4, uchar b5, uchar b6, uchar b7, uchar b8 )
    {
	data1 = l;
	data2 = w1;
	data3 = w2;
	data4[0] = b1;
	data4[1] = b2;
	data4[2] = b3;
	data4[3] = b4;
	data4[4] = b5;
	data4[5] = b6;
	data4[6] = b7;
	data4[7] = b8;
    }
    TQUuid( const TQUuid &uuid )
    {
	memcpy( this, &uuid, sizeof(TQUuid) );
    }
#ifndef TQT_NO_QUUID_STRING
    TQUuid( const TQString & );
    TQUuid( const char * );
    TQString toString() const;
    operator TQString() const { return toString(); }
#endif
    bool isNull() const;

    TQUuid &operator=(const TQUuid &orig )
    {
	memcpy( this, &orig, sizeof(TQUuid) );
	return *this;
    }

    bool operator==(const TQUuid &orig ) const
    {
	uint i;
	if ( data1 != orig.data1 || data2 != orig.data2 || 
	     data3 != orig.data3 )
	    return false;

	for( i = 0; i < 8; i++ )
	    if ( data4[i] != orig.data4[i] )
		return false;
	
	return true;
    }

    bool operator!=(const TQUuid &orig ) const
    {
	return !( *this == orig );
    }

    bool operator<(const TQUuid &other ) const;
    bool operator>(const TQUuid &other ) const;

#if defined(Q_OS_WIN32)
    // On Windows we have a type GUID that is used by the platform API, so we
    // provide convenience operators to cast from and to this type.
    TQUuid( const GUID &guid )
    {
	memcpy( this, &guid, sizeof(GUID) );
    }

    TQUuid &operator=(const GUID &orig )
    {
	memcpy( this, &orig, sizeof(TQUuid) );
	return *this;
    }

    operator GUID() const
    {
	GUID guid = { data1, data2, data3, { data4[0], data4[1], data4[2], data4[3], data4[4], data4[5], data4[6], data4[7] } };
	return guid;
    }

    bool operator==( const GUID &guid ) const
    {
	uint i;
	if ( data1 != guid.Data1 || data2 != guid.Data2 || 
	     data3 != guid.Data3 )
	    return false;

	for( i = 0; i < 8; i++ )
	    if ( data4[i] != guid.Data4[i] )
		return false;
	
	return true;
    }

    bool operator!=( const GUID &guid ) const
    {
	return !( *this == guid );
    }
#endif
    static TQUuid createUuid();
    TQUuid::Variant variant() const;
    TQUuid::Version version() const;

    uint    data1;
    ushort  data2;
    ushort  data3;
    uchar   data4[ 8 ];
};

#ifndef TQT_NO_DATASTREAM
TQ_EXPORT TQDataStream &operator<<( TQDataStream &, const TQUuid & );
TQ_EXPORT TQDataStream &operator>>( TQDataStream &, TQUuid & );
#endif

#endif //TQUUID_H
