/****************************************************************************
**
** Implementation of TQUrlInfo class
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

#include "ntqurlinfo.h"

#ifndef TQT_NO_NETWORKPROTOCOL

#include "ntqurloperator.h"
#include "ntqdir.h"
#include <limits.h>

class TQUrlInfoPrivate
{
public:
    TQUrlInfoPrivate() :
	permissions(0),
	size(0),
	isDir(false),
	isFile(true),
	isSymLink(false),
	isWritable(true),
	isReadable(true),
	isExecutable(false)
    {}

    TQString name;
    int permissions;
    TQString owner;
    TQString group;
#if defined(QT_LARGEFILE_SUPPORT)
    TQIODevice::Offset size;
#else
    uint size;
#endif
    TQDateTime lastModified;
    TQDateTime lastRead;
    bool isDir;
    bool isFile;
    bool isSymLink;
    bool isWritable;
    bool isReadable;
    bool isExecutable;
};


/*!
    \class TQUrlInfo ntqurlinfo.h
    \brief The TQUrlInfo class stores information about URLs.

    \ingroup io
    \ingroup misc

    This class is just a container for storing information about URLs,
    which is why all information must be passed in the constructor.

    Unless you're reimplementing a network protocol you're unlikely to
    create TQUrlInfo objects yourself, but you may receive TQUrlInfo
    objects from functions, e.g. TQUrlOperator::info().

    The information that can be retrieved includes name(),
    permissions(), owner(), group(), size(), lastModified(),
    lastRead(), isDir(), isFile(), isSymLink(), isWritable(),
    isReadable() and isExecutable().
*/

/*!
    \enum TQUrlInfo::PermissionSpec

    This enum is used by the permissions() function to report the
    permissions of a file.

    \value ReadOwner The file is readable by the owner of the file.
    \value WriteOwner The file is writable by the owner of the file.
    \value ExeOwner The file is executable by the owner of the file.
    \value ReadGroup The file is readable by the group.
    \value WriteGroup The file is writable by the group.
    \value ExeGroup The file is executable by the group.
    \value ReadOther The file is readable by anyone.
    \value WriteOther The file is writable by anyone.
    \value ExeOther The file is executable by anyone.
*/

/*!
    Constructs an invalid TQUrlInfo object with default values.

    \sa isValid()
*/

TQUrlInfo::TQUrlInfo()
{
    d = 0;
}

/*!
    Constructs a TQUrlInfo object with information about the file \a
    file in the \a path. It tries to find the information about the \a
    file in the TQUrlOperator \a path.

    If the information is not found, this constructor creates an
    invalid TQUrlInfo, i.e. isValid() returns false. You should always
    check if the URL info is valid before relying on the return values
    of any getter functions.

    If \a file is empty, it defaults to the TQUrlOperator \a path, i.e.
    to the directory.

    \sa isValid() TQUrlOperator::info()
*/

TQUrlInfo::TQUrlInfo( const TQUrlOperator &path, const TQString &file )
{
    TQString file_ = file;
    if ( file_.isEmpty() )
	file_ = ".";

    TQUrlInfo inf = path.info( file_ );
    if ( inf.d ) {
	d = new TQUrlInfoPrivate;
	*d = *inf.d;
    } else {
	d = 0;
    }
}

/*!
    Copy constructor, copies \a ui to this URL info object.
*/

TQUrlInfo::TQUrlInfo( const TQUrlInfo &ui )
{
    if ( ui.d ) {
	d = new TQUrlInfoPrivate;
	*d = *ui.d;
    } else {
	d = 0;
    }
}

/*!
    Constructs a TQUrlInfo object by specifying all the URL's
    information.

    The information that is passed is the \a name, file \a
    permissions, \a owner and \a group and the file's \a size. Also
    passed is the \a lastModified date/time and the \a lastRead
    date/time. Flags are also passed, specifically, \a isDir, \a
    isFile, \a isSymLink, \a isWritable, \a isReadable and \a
    isExecutable.
*/

#if defined(QT_ABI_QT4)
TQUrlInfo::TQUrlInfo( const TQString &name, int permissions, const TQString &owner,
		    const TQString &group, TQIODevice::Offset size, const TQDateTime &lastModified,
		    const TQDateTime &lastRead, bool isDir, bool isFile, bool isSymLink,
		    bool isWritable, bool isReadable, bool isExecutable )
#else
TQUrlInfo::TQUrlInfo( const TQString &name, int permissions, const TQString &owner,
		    const TQString &group, uint size, const TQDateTime &lastModified,
		    const TQDateTime &lastRead, bool isDir, bool isFile, bool isSymLink,
		    bool isWritable, bool isReadable, bool isExecutable )
#endif
{
    d = new TQUrlInfoPrivate;
    d->name = name;
    d->permissions = permissions;
    d->owner = owner;
    d->group = group;
    d->size = size;
    d->lastModified = lastModified;
    d->lastRead = lastRead;
    d->isDir = isDir;
    d->isFile = isFile;
    d->isSymLink = isSymLink;
    d->isWritable = isWritable;
    d->isReadable = isReadable;
    d->isExecutable = isExecutable;
}


/*!
    Constructs a TQUrlInfo object by specifying all the URL's
    information.

    The information that is passed is the \a url, file \a
    permissions, \a owner and \a group and the file's \a size. Also
    passed is the \a lastModified date/time and the \a lastRead
    date/time. Flags are also passed, specifically, \a isDir, \a
    isFile, \a isSymLink, \a isWritable, \a isReadable and \a
    isExecutable.
*/

#if defined(QT_ABI_QT4)
TQUrlInfo::TQUrlInfo( const TQUrl &url, int permissions, const TQString &owner,
		    const TQString &group, TQIODevice::Offset size, const TQDateTime &lastModified,
		    const TQDateTime &lastRead, bool isDir, bool isFile, bool isSymLink,
		    bool isWritable, bool isReadable, bool isExecutable )
#else
TQUrlInfo::TQUrlInfo( const TQUrl &url, int permissions, const TQString &owner,
		    const TQString &group, uint size, const TQDateTime &lastModified,
		    const TQDateTime &lastRead, bool isDir, bool isFile, bool isSymLink,
		    bool isWritable, bool isReadable, bool isExecutable )
#endif
{
    d = new TQUrlInfoPrivate;
    d->name = TQFileInfo( url.path() ).fileName();
    d->permissions = permissions;
    d->owner = owner;
    d->group = group;
    d->size = size;
    d->lastModified = lastModified;
    d->lastRead = lastRead;
    d->isDir = isDir;
    d->isFile = isFile;
    d->isSymLink = isSymLink;
    d->isWritable = isWritable;
    d->isReadable = isReadable;
    d->isExecutable = isExecutable;
}


/*!
    Sets the name of the URL to \a name. The name is the full text,
    for example, "http://doc.trolltech.com/ntqurlinfo.html".

    If you call this function for an invalid URL info, this function
    turns it into a valid one.

    \sa isValid()
*/

void TQUrlInfo::setName( const TQString &name )
{
    if ( !d )
	d = new TQUrlInfoPrivate;
    d->name = name;
}


/*!
    If \a b is true then the URL is set to be a directory; if \b is
    false then the URL is set not to be a directory (which normally
    means it is a file). (Note that a URL can refer to both a file and
    a directory even though most file systems do not support this.)

    If you call this function for an invalid URL info, this function
    turns it into a valid one.

    \sa isValid()
*/

void TQUrlInfo::setDir( bool b )
{
    if ( !d )
	d = new TQUrlInfoPrivate;
    d->isDir = b;
}


/*!
    If \a b is true then the URL is set to be a file; if \b is false
    then the URL is set not to be a file (which normally means it is a
    directory). (Note that a URL can refer to both a file and a
    directory even though most file systems do not support this.)

    If you call this function for an invalid URL info, this function
    turns it into a valid one.

    \sa isValid()
*/

void TQUrlInfo::setFile( bool b )
{
    if ( !d )
	d = new TQUrlInfoPrivate;
    d->isFile = b;
}


/*!
    Specifies that the URL refers to a symbolic link if \a b is true
    and that it does not if \a b is false.

    If you call this function for an invalid URL info, this function
    turns it into a valid one.

    \sa isValid()
*/

void TQUrlInfo::setSymLink( bool b )
{
    if ( !d )
	d = new TQUrlInfoPrivate;
    d->isSymLink = b;
}


/*!
    Specifies that the URL is writable if \a b is true and not
    writable if \a b is false.

    If you call this function for an invalid URL info, this function
    turns it into a valid one.

    \sa isValid()
*/

void TQUrlInfo::setWritable( bool b )
{
    if ( !d )
	d = new TQUrlInfoPrivate;
    d->isWritable = b;
}


/*!
    Specifies that the URL is readable if \a b is true and not
    readable if \a b is false.

    If you call this function for an invalid URL info, this function
    turns it into a valid one.

    \sa isValid()
*/

void TQUrlInfo::setReadable( bool b )
{
    if ( !d )
	d = new TQUrlInfoPrivate;
    d->isReadable = b;
}

/*!
    Specifies that the owner of the URL is called \a s.

    If you call this function for an invalid URL info, this function
    turns it into a valid one.

    \sa isValid()
*/

void TQUrlInfo::setOwner( const TQString &s )
{
    if ( !d )
	d = new TQUrlInfoPrivate;
    d->owner = s;
}

/*!
    Specifies that the owning group of the URL is called \a s.

    If you call this function for an invalid URL info, this function
    turns it into a valid one.

    \sa isValid()
*/

void TQUrlInfo::setGroup( const TQString &s )
{
    if ( !d )
	d = new TQUrlInfoPrivate;
    d->group = s;
}

/*!
    Specifies the \a size of the URL.

    If you call this function for an invalid URL info, this function
    turns it into a valid one.

    \sa isValid()
*/

#if defined(QT_ABI_QT4)
void TQUrlInfo::setSize( TQIODevice::Offset size )
#else
void TQUrlInfo::setSize( uint size )
#endif
{
    if ( !d )
	d = new TQUrlInfoPrivate;
    d->size = size;
}


// ### reggie - what's the permission type? As in Unix?

/*!
    Specifies that the URL has access permisions, \a p.

    If you call this function for an invalid URL info, this function
    turns it into a valid one.

    \sa isValid()
*/

void TQUrlInfo::setPermissions( int p )
{
    if ( !d )
	d = new TQUrlInfoPrivate;
    d->permissions = p;
}

/*!
    Specifies that the object the URL refers to was last modified at
    \a dt.

    If you call this function for an invalid URL info, this function
    turns it into a valid one.

    \sa isValid()
*/

void TQUrlInfo::setLastModified( const TQDateTime &dt )
{
    if ( !d )
	d = new TQUrlInfoPrivate;
    d->lastModified = dt;
}

/*!
    Destroys the URL info object.

    The TQUrlOperator object to which this URL referred (if any) is not
    affected.
*/

TQUrlInfo::~TQUrlInfo()
{
    delete d;
}

/*!
    Assigns the values of \a ui to this TQUrlInfo object.
*/

TQUrlInfo &TQUrlInfo::operator=( const TQUrlInfo &ui )
{
    if ( ui.d ) {
	if ( !d )
	    d= new TQUrlInfoPrivate;
	*d = *ui.d;
    } else {
	delete d;
	d = 0;
    }
    return *this;
}

/*!
    Returns the file name of the URL.

    \sa isValid()
*/

TQString TQUrlInfo::name() const
{
    if ( !d )
	return TQString::null;
    return d->name;
}

/*!
    Returns the permissions of the URL. You can use the \c PermissionSpec flags
    to test for certain permissions.

    \sa isValid()
*/

int TQUrlInfo::permissions() const
{
    if ( !d )
	return 0;
    return d->permissions;
}

/*!
    Returns the owner of the URL.

    \sa isValid()
*/

TQString TQUrlInfo::owner() const
{
    if ( !d )
	return TQString::null;
    return d->owner;
}

/*!
    Returns the group of the URL.

    \sa isValid()
*/

TQString TQUrlInfo::group() const
{
    if ( !d )
	return TQString::null;
    return d->group;
}

/*!
    Returns the size of the URL.

    \sa isValid()
*/

#if defined(QT_ABI_QT4)
TQIODevice::Offset TQUrlInfo::size() const
#else
uint TQUrlInfo::size() const
#endif
{
    if ( !d )
	return 0;
#if defined(QT_LARGEFILE_SUPPORT) && !defined(QT_ABI_QT4)
    return d->size > UINT_MAX ? UINT_MAX : (uint)d->size;
#else
    return d->size;
#endif
}

/*!
    Returns the last modification date of the URL.

    \sa isValid()
*/

TQDateTime TQUrlInfo::lastModified() const
{
    if ( !d )
	return TQDateTime();
    return d->lastModified;
}

/*!
    Returns the date when the URL was last read.

    \sa isValid()
*/

TQDateTime TQUrlInfo::lastRead() const
{
    if ( !d )
	return TQDateTime();
    return d->lastRead;
}

/*!
    Returns true if the URL is a directory; otherwise returns false.

    \sa isValid()
*/

bool TQUrlInfo::isDir() const
{
    if ( !d )
	return false;
    return d->isDir;
}

/*!
    Returns true if the URL is a file; otherwise returns false.

    \sa isValid()
*/

bool TQUrlInfo::isFile() const
{
    if ( !d )
	return false;
    return d->isFile;
}

/*!
    Returns true if the URL is a symbolic link; otherwise returns false.

    \sa isValid()
*/

bool TQUrlInfo::isSymLink() const
{
    if ( !d )
	return false;
    return d->isSymLink;
}

/*!
    Returns true if the URL is writable; otherwise returns false.

    \sa isValid()
*/

bool TQUrlInfo::isWritable() const
{
    if ( !d )
	return false;
    return d->isWritable;
}

/*!
    Returns true if the URL is readable; otherwise returns false.

    \sa isValid()
*/

bool TQUrlInfo::isReadable() const
{
    if ( !d )
	return false;
    return d->isReadable;
}

/*!
    Returns true if the URL is executable; otherwise returns false.

    \sa isValid()
*/

bool TQUrlInfo::isExecutable() const
{
    if ( !d )
	return false;
    return d->isExecutable;
}

/*!
    Returns true if \a i1 is greater than \a i2; otherwise returns
    false. The objects are compared by the value, which is specified
    by \a sortBy. This must be one of TQDir::Name, TQDir::Time or
    TQDir::Size.
*/

bool TQUrlInfo::greaterThan( const TQUrlInfo &i1, const TQUrlInfo &i2,
			    int sortBy )
{
    switch ( sortBy ) {
    case TQDir::Name:
	return i1.name() > i2.name();
    case TQDir::Time:
	return i1.lastModified() > i2.lastModified();
    case TQDir::Size:
	return i1.size() > i2.size();
    default:
	return false;
    }
}

/*!
    Returns true if \a i1 is less than \a i2; otherwise returns false.
    The objects are compared by the value, which is specified by \a
    sortBy. This must be one of TQDir::Name, TQDir::Time or TQDir::Size.
*/

bool TQUrlInfo::lessThan( const TQUrlInfo &i1, const TQUrlInfo &i2,
			 int sortBy )
{
    return !greaterThan( i1, i2, sortBy );
}

/*!
    Returns true if \a i1 equals to \a i2; otherwise returns false.
    The objects are compared by the value, which is specified by \a
    sortBy. This must be one of TQDir::Name, TQDir::Time or TQDir::Size.
*/

bool TQUrlInfo::equal( const TQUrlInfo &i1, const TQUrlInfo &i2,
		      int sortBy )
{
    switch ( sortBy ) {
    case TQDir::Name:
	return i1.name() == i2.name();
    case TQDir::Time:
	return i1.lastModified() == i2.lastModified();
    case TQDir::Size:
	return i1.size() == i2.size();
    default:
	return false;
    }
}

/*!
    Compares this TQUrlInfo with \a i and returns true if they are
    equal; otherwise returns false.
*/

bool TQUrlInfo::operator==( const TQUrlInfo &i ) const
{
    if ( !d )
	return i.d == 0;
    if ( !i.d )
	return false;

    return ( d->name == i.d->name &&
	     d->permissions == i.d->permissions &&
	     d->owner == i.d->owner &&
	     d->group == i.d->group &&
	     d->size == i.d->size &&
	     d->lastModified == i.d->lastModified &&
	     d->lastRead == i.d->lastRead &&
	     d->isDir == i.d->isDir &&
	     d->isFile == i.d->isFile &&
	     d->isSymLink == i.d->isSymLink &&
	     d->isWritable == i.d->isWritable &&
	     d->isReadable == i.d->isReadable &&
	     d->isExecutable == i.d->isExecutable );
}

/*!
    Returns true if the URL info is valid; otherwise returns false.
    Valid means that the TQUrlInfo contains real information. For
    example, a call to TQUrlOperator::info() might return a an invalid
    TQUrlInfo, if no information about the requested entry is
    available.

    You should always check if the URL info is valid before relying on
    the values.
*/
bool TQUrlInfo::isValid() const
{
    return d != 0;
}

#endif // TQT_NO_NETWORKPROTOCOL
