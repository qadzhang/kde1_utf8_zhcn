/****************************************************************************
**
** Definition of TQFileInfo class
**
** Created : 950628
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

#ifndef TQFILEINFO_H
#define TQFILEINFO_H

#ifndef QT_H
#include "ntqfile.h"
#include "ntqdatetime.h"
#endif // QT_H


class TQDir;
struct TQFileInfoCache;
template <class T> class TQDeepCopy;


class TQ_EXPORT TQFileInfo
{
public:
    enum PermissionSpec {
	ReadOwner = 04000, WriteOwner = 02000, ExeOwner = 01000,
	ReadUser  = 00400, WriteUser  = 00200, ExeUser  = 00100,
	ReadGroup = 00040, WriteGroup = 00020, ExeGroup = 00010,
	ReadOther = 00004, WriteOther = 00002, ExeOther = 00001 };

    TQFileInfo();
    TQFileInfo( const TQString &file );
    TQFileInfo( const TQFile & );
#ifndef TQT_NO_DIR
    TQFileInfo( const TQDir &, const TQString &fileName );
#endif
    TQFileInfo( const TQFileInfo & );
   ~TQFileInfo();

    TQFileInfo  &operator=( const TQFileInfo & );

    void	setFile( const TQString &file );
    void	setFile( const TQFile & );
#ifndef TQT_NO_DIR
    void	setFile( const TQDir &, const TQString &fileName );
#endif
    bool	exists()	const;
    void	refresh()	const;
    bool	caching()	const;
    void	setCaching( bool );

    TQString	filePath()	const;
    TQString	fileName()	const;
#ifndef TQT_NO_DIR //###
    TQString	absFilePath()	const;
#endif
    TQString	baseName( bool complete = false ) const;
    TQString	extension( bool complete = true ) const;

#ifndef TQT_NO_DIR //###
    TQString	dirPath( bool absPath = false ) const;
#endif
#ifndef TQT_NO_DIR
    TQDir	dir( bool absPath = false )	const;
#endif
    bool	isReadable()	const;
    bool	isWritable()	const;
    bool	isExecutable()	const;
    bool 	isHidden()      const;

#ifndef TQT_NO_DIR //###
    bool	isRelative()	const;
    bool	convertToAbs();
#endif

    bool	isFile()	const;
    bool	isDir()		const;
    bool	isSymLink()	const;

    TQString	readLink()	const;

    TQString	owner()		const;
    uint	ownerId()	const;
    TQString	group()		const;
    uint	groupId()	const;

    bool	permission( int permissionSpec ) const;

#if (TQT_VERSION-0 >= 0x040000)
#error "TQFileInfo::size() should return TQIODevice::Offset instead of uint"
#elif defined(QT_ABI_QT4)
    TQIODevice::Offset size()	const;
#else
    uint	size()		const;
#endif

    TQDateTime	created()	const;
    TQDateTime	lastModified()	const;
    TQDateTime	lastRead()	const;

private:
    void	doStat() const;
    static void slashify( TQString & );
    static void makeAbs( TQString & );

    TQString	fn;
    TQFileInfoCache *fic;
    bool	cache;
#if defined(Q_OS_UNIX)
    bool        symLink;
#endif

    void detach();
    friend class TQDeepCopy< TQFileInfo >;
};


inline bool TQFileInfo::caching() const
{
    return cache;
}


#endif // TQFILEINFO_H
