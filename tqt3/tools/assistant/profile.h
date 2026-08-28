/**********************************************************************
** Copyright (C) 2000-2008 Trolltech ASA.  All rights reserved.
**
** This file is part of the TQt Assistant.
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
** Licensees holding valid TQt Commercial licenses may use this file in
** accordance with the TQt Commercial License Agreement provided with
** the Software.
**
** This file is provided "AS IS" with NO WARRANTY OF ANY KIND,
** INCLUDING THE WARRANTIES OF DESIGN, MERCHANTABILITY AND FITNESS FOR
** A PARTICULAR PURPOSE. Trolltech reserves all rights not granted
** herein.
**
**********************************************************************/

#ifndef PROFILE_H
#define PROFILE_H

#include <ntqfileinfo.h>
#include <ntqstring.h>
#include <ntqstringlist.h>
#include <ntqmap.h>

class DocuParser;

class Profile
{
public:
    enum ProfileType { DefaultProfile, UserProfile };
    Profile();

    inline bool isValid() const;

    inline void addDCF( const TQString &docfile );
    inline void addDCFIcon( const TQString title, const TQString &icon );
    inline void addDCFIndexPage( const TQString title, const TQString &indexPage );
    inline void addDCFImageDir( const TQString title, const TQString &imgDir );
    inline void addDCFTitle( const TQString &dcf, const TQString &title );
    inline void addProperty( const TQString &name, const TQString &value );
    inline bool hasDocFile( const TQString &docFile );
    void removeDocFileEntry( const TQString &title );

    inline ProfileType profileType() const { return type; }
    inline void setProfileType( ProfileType t ) { type = t; }

    inline DocuParser *docuParser() const { return dparser; }
    inline void setDocuParser( DocuParser *dp ) { dparser = dp; }

    static Profile* createDefaultProfile( const TQString &docPath = TQString::null );
    static TQString makeRelativePath( const TQString &base, const TQString &path );

    int valid:1;
    ProfileType type;
    DocuParser *dparser;
    TQMap<TQString,TQString> props;
    TQMap<TQString,TQString> icons;
    TQMap<TQString,TQString> indexPages;
    TQMap<TQString,TQString> imageDirs;
    TQMap<TQString,TQString> dcfTitles;
    TQStringList docs;
};


inline bool Profile::isValid() const
{
    return valid;
}

inline void Profile::addDCFTitle(const TQString &dcf, const TQString &title)
{
    TQString absdcf = TQFileInfo(dcf).absFilePath();
    dcfTitles[title] = absdcf;
    if (docs.contains(absdcf) == 0)
	docs << absdcf;
}

inline void Profile::addDCF( const TQString &docfile )
{
    if( !docs.contains( docfile ) == 0 )
	docs << docfile;
}

inline void Profile::addDCFIcon( const TQString docfile,
				     const TQString &icon )
{
    icons[docfile] = icon;
}

inline void Profile::addDCFIndexPage( const TQString title,
				      const TQString &indexPage )
{
    indexPages[title] = indexPage;
}

inline void Profile::addDCFImageDir( const TQString docfile,
				     const TQString &imgDir )
{
    imageDirs[docfile] = imgDir;
}

inline void Profile::addProperty( const TQString &name,
				  const TQString &value )
{
    props[name] = value;
}

inline bool Profile::hasDocFile( const TQString &name )
{
    return docs.contains( name ) > 0;
}

#endif
