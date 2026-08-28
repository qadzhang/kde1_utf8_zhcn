/****************************************************************************
**
** Definition of TQUrlInfo class
**
** Created : 950429
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

#ifndef TQURLINFO_H
#define TQURLINFO_H

#ifndef QT_H
#include "ntqdatetime.h"
#include "ntqstring.h"
#if defined(QT_ABI_QT4)
#include "ntqiodevice.h"
#endif
#endif // QT_H

class TQUrlOperator;
class TQUrl;
class TQUrlInfoPrivate;

class TQ_EXPORT TQUrlInfo
{
public:
    enum PermissionSpec {
	ReadOwner = 00400, WriteOwner = 00200, ExeOwner = 00100,
	ReadGroup = 00040, WriteGroup = 00020, ExeGroup = 00010,
	ReadOther = 00004, WriteOther = 00002, ExeOther = 00001 };

    TQUrlInfo();
    TQUrlInfo( const TQUrlOperator &path, const TQString &file );
    TQUrlInfo( const TQUrlInfo &ui );
#if (TQT_VERSION-0 >= 0x040000)
#error "TQUrlInfo::TQUrlInfo() should accept TQIODevice::Offset instead of uint"
#elif defined(QT_ABI_QT4)
    TQUrlInfo( const TQString &name, int permissions, const TQString &owner,
	      const TQString &group, TQIODevice::Offset size, const TQDateTime &lastModified,
	      const TQDateTime &lastRead, bool isDir, bool isFile, bool isSymLink,
	      bool isWritable, bool isReadable, bool isExecutable );
    TQUrlInfo( const TQUrl &url, int permissions, const TQString &owner,
	      const TQString &group, TQIODevice::Offset size, const TQDateTime &lastModified,
	      const TQDateTime &lastRead, bool isDir, bool isFile, bool isSymLink,
	      bool isWritable, bool isReadable, bool isExecutable );
#else
    TQUrlInfo( const TQString &name, int permissions, const TQString &owner,
	      const TQString &group, uint size, const TQDateTime &lastModified,
	      const TQDateTime &lastRead, bool isDir, bool isFile, bool isSymLink,
	      bool isWritable, bool isReadable, bool isExecutable );
    TQUrlInfo( const TQUrl &url, int permissions, const TQString &owner,
	      const TQString &group, uint size, const TQDateTime &lastModified,
	      const TQDateTime &lastRead, bool isDir, bool isFile, bool isSymLink,
	      bool isWritable, bool isReadable, bool isExecutable );
#endif
    TQUrlInfo &operator=( const TQUrlInfo &ui );
    virtual ~TQUrlInfo();

    virtual void setName( const TQString &name );
    virtual void setDir( bool b );
    virtual void setFile( bool b );
    virtual void setSymLink( bool b );
    virtual void setOwner( const TQString &s );
    virtual void setGroup( const TQString &s );
#if (TQT_VERSION-0 >= 0x040000)
#error "TQUrlInfo::setSize() should accept TQIODevice::Offset instead of uint"
#elif defined(QT_ABI_QT4)
    virtual void setSize( TQIODevice::Offset size );
#else
    virtual void setSize( uint size );
#endif
    virtual void setWritable( bool b );
    virtual void setReadable( bool b );
    virtual void setPermissions( int p );
    virtual void setLastModified( const TQDateTime &dt );

    bool isValid() const;

    TQString name() const;
    int permissions() const;
    TQString owner() const;
    TQString group() const;
#if (TQT_VERSION-0 >= 0x040000)
#error "TQUrlInfo::size() should return TQIODevice::Offset instead of uint"
#elif defined(QT_ABI_QT4)
    TQIODevice::Offset size() const;
#else
    uint size() const;
#endif
    TQDateTime lastModified() const;
    TQDateTime lastRead() const;
    bool isDir() const;
    bool isFile() const;
    bool isSymLink() const;
    bool isWritable() const;
    bool isReadable() const;
    bool isExecutable() const;

    static bool greaterThan( const TQUrlInfo &i1, const TQUrlInfo &i2,
			     int sortBy );
    static bool lessThan( const TQUrlInfo &i1, const TQUrlInfo &i2,
			  int sortBy );
    static bool equal( const TQUrlInfo &i1, const TQUrlInfo &i2,
		       int sortBy );

    bool operator==( const TQUrlInfo &i ) const;
private:
    TQUrlInfoPrivate *d;

};

#endif
