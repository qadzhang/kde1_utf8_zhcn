/****************************************************************************
**
** Definition of TQDir class
**
** Created : 950427
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

#ifndef TQDIR_H
#define TQDIR_H

#ifndef QT_H
#include "ntqglobal.h"
#include "ntqstrlist.h"
#include "ntqfileinfo.h"
#endif // QT_H


#ifndef TQT_NO_DIR
typedef TQPtrList<TQFileInfo> TQFileInfoList;
typedef TQPtrListIterator<TQFileInfo> TQFileInfoListIterator;
class TQStringList;
template <class T> class TQDeepCopy;


class TQ_EXPORT TQDir
{
public:
    enum FilterSpec { Dirs	    = 0x001,
		      Files	    = 0x002,
		      Drives	    = 0x004,
		      NoSymLinks    = 0x008,
		      All	    = 0x007,
		      TypeMask	    = 0x00F,

		      Readable	    = 0x010,
		      Writable	    = 0x020,
		      Executable    = 0x040,
		      RWEMask	    = 0x070,

		      Modified	    = 0x080,
		      Hidden	    = 0x100,
		      System	    = 0x200,
		      AccessMask    = 0x3F0,

		      DefaultFilter = -1 };

    enum SortSpec   { Name	    = 0x00,
		      Time	    = 0x01,
		      Size	    = 0x02,
		      Unsorted	    = 0x03,
		      SortByMask    = 0x03,

		      DirsFirst	    = 0x04,
		      Reversed	    = 0x08,
		      IgnoreCase    = 0x10,
                      LocaleAware   = 0x20, 
		      DefaultSort   = -1 };

    TQDir();
    TQDir( const TQString &path, const TQString &nameFilter = TQString::null,
	  int sortSpec = Name | IgnoreCase, int filterSpec = All );
    TQDir( const TQDir & );

    virtual ~TQDir();

    TQDir       &operator=( const TQDir & );
    TQDir       &operator=( const TQString &path );

    virtual void setPath( const TQString &path );
    virtual TQString path()		const;
    virtual TQString absPath()	const;
    virtual TQString canonicalPath()	const;

    virtual TQString dirName() const;
    virtual TQString filePath( const TQString &fileName,
			      bool acceptAbsPath = true ) const;
    virtual TQString absFilePath( const TQString &fileName,
				 bool acceptAbsPath = true ) const;

    static TQString convertSeparators( const TQString &pathName );

    virtual bool cd( const TQString &dirName, bool acceptAbsPath = true );
    virtual bool cdUp();

    TQString	nameFilter() const;
    virtual void setNameFilter( const TQString &nameFilter );
    FilterSpec filter() const;
    virtual void setFilter( int filterSpec );
    SortSpec sorting() const;
    virtual void setSorting( int sortSpec );

    bool	matchAllDirs() const;
    virtual void setMatchAllDirs( bool );

    uint count() const;
    TQString	operator[]( int ) const;

    virtual TQStrList encodedEntryList( int filterSpec = DefaultFilter,
				       int sortSpec   = DefaultSort  ) const;
    virtual TQStrList encodedEntryList( const TQString &nameFilter,
				       int filterSpec = DefaultFilter,
				       int sortSpec   = DefaultSort   ) const;
    virtual TQStringList entryList( int filterSpec = DefaultFilter,
				   int sortSpec   = DefaultSort  ) const;
    virtual TQStringList entryList( const TQString &nameFilter,
				   int filterSpec = DefaultFilter,
				   int sortSpec   = DefaultSort   ) const;

    virtual const TQFileInfoList *entryInfoList( int filterSpec = DefaultFilter,
						int sortSpec = DefaultSort ) const;
    virtual const TQFileInfoList *entryInfoList( const TQString &nameFilter,
						int filterSpec = DefaultFilter,
						int sortSpec = DefaultSort ) const;

    static const TQFileInfoList *drives();

    virtual bool mkdir( const TQString &dirName,
			bool acceptAbsPath = true ) const;
    virtual bool rmdir( const TQString &dirName,
			bool acceptAbsPath = true ) const;

    virtual bool isReadable() const;
    virtual bool exists()   const;
    virtual bool isRoot()   const;

    virtual bool isRelative() const;
    virtual void convertToAbs();

    virtual bool operator==( const TQDir & ) const;
    virtual bool operator!=( const TQDir & ) const;

    virtual bool remove( const TQString &fileName,
			 bool acceptAbsPath = true );
    virtual bool rename( const TQString &name, const TQString &newName,
			 bool acceptAbsPaths = true  );
    virtual bool exists( const TQString &name,
			 bool acceptAbsPath = true );

    static char separator();

    static bool setCurrent( const TQString &path );
    static TQDir current();
    static TQDir home();
    static TQDir root();
    static TQString currentDirPath();
    static TQString homeDirPath();
    static TQString rootDirPath();

    static bool match( const TQStringList &filters, const TQString &fileName );
    static bool match( const TQString &filter, const TQString &fileName );
    static TQString cleanDirPath( const TQString &dirPath );
    static bool isRelativePath( const TQString &path );
    void refresh() const;

private:
#ifdef Q_OS_MAC
    typedef struct FSSpec FSSpec;
    static FSSpec *make_spec(const TQString &);
#endif
    void init();
    virtual bool readDirEntries( const TQString &nameFilter,
				 int FilterSpec, int SortSpec  );

    static void slashify( TQString & );

    TQString	dPath;
    TQStringList   *fList;
    TQFileInfoList *fiList;
    TQString	nameFilt;
    FilterSpec	filtS;
    SortSpec	sortS;
    uint	dirty	: 1;
    uint	allDirs : 1;

    void detach();
    friend class TQDeepCopy< TQDir >;
};


inline TQString TQDir::path() const
{
    return dPath;
}

inline TQString TQDir::nameFilter() const
{
    return nameFilt;
}

inline TQDir::FilterSpec TQDir::filter() const
{
    return filtS;
}

inline TQDir::SortSpec TQDir::sorting() const
{
    return sortS;
}

inline bool TQDir::matchAllDirs() const
{
    return allDirs;
}

inline bool TQDir::operator!=( const TQDir &d ) const
{
    return !(*this == d);
}


struct TQDirSortItem {
    TQString filename_cache;
    TQFileInfo* item;
};

#endif // TQT_NO_DIR
#endif // TQDIR_H
