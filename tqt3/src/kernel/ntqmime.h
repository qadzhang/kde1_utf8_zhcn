/****************************************************************************
**
** Definition of mime classes
**
** Created : 981204
**
** Copyright (C) 1998-2008 Trolltech ASA.  All rights reserved.
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

#ifndef TQMIME_H
#define TQMIME_H

#ifndef QT_H
#include "ntqwindowdefs.h"
#include "ntqmap.h"
#endif // QT_H

#ifndef TQT_NO_MIME

class TQImageDrag;
class TQTextDrag;

class TQ_EXPORT TQMimeSource
{
    friend class TQClipboardData;

public:
    TQMimeSource();
    virtual ~TQMimeSource();
    virtual const char* format( int n = 0 ) const = 0;
    virtual bool provides( const char* ) const;
    virtual TQByteArray encodedData( const char* ) const = 0;
    int serialNumber() const;

private:
    int ser_no;
    enum { NoCache, Text, Graphics } cacheType;
    union
    {
	struct
	{
	    TQString *str;
	    TQCString *subtype;
	} txt;
	struct
	{
	    TQImage *img;
	    TQPixmap *pix;
	} gfx;
    } cache;
    void clearCache();

    // friends for caching
    friend class TQImageDrag;
    friend class TQTextDrag;

};

inline int TQMimeSource::serialNumber() const
{ return ser_no; }

class TQStringList;
class TQMimeSourceFactoryData;

class TQ_EXPORT TQMimeSourceFactory {
public:
    TQMimeSourceFactory();
    virtual ~TQMimeSourceFactory();

    static TQMimeSourceFactory* defaultFactory();
    static void setDefaultFactory( TQMimeSourceFactory* );
    static TQMimeSourceFactory* takeDefaultFactory();
    static void addFactory( TQMimeSourceFactory *f );
    static void removeFactory( TQMimeSourceFactory *f );

    virtual const TQMimeSource* data(const TQString& abs_name) const;
    virtual TQString makeAbsolute(const TQString& abs_or_rel_name, const TQString& context) const;
    const TQMimeSource* data(const TQString& abs_or_rel_name, const TQString& context) const;

    virtual void setText( const TQString& abs_name, const TQString& text );
    virtual void setImage( const TQString& abs_name, const TQImage& im );
    virtual void setPixmap( const TQString& abs_name, const TQPixmap& pm );
    virtual void setData( const TQString& abs_name, TQMimeSource* data );
    virtual void setFilePath( const TQStringList& );
    virtual TQStringList filePath() const;
    void addFilePath( const TQString& );
    virtual void setExtensionType( const TQString& ext, const char* mimetype );

private:
    TQMimeSource *dataInternal(const TQString& abs_name, const TQMap<TQString, TQString> &extensions ) const;
    TQMimeSourceFactoryData* d;
};

#if defined(TQ_WS_WIN)

#ifndef QT_H
#include "ntqptrlist.h" // down here for GCC 2.7.* compatibility
#endif // QT_H

/*
  Encapsulation of conversion between MIME and Windows CLIPFORMAT.
  Not need on X11, as the underlying protocol uses the MIME standard
  directly.
*/

class TQ_EXPORT TQWindowsMime {
public:
    TQWindowsMime();
    virtual ~TQWindowsMime();

    static void initialize();

    static TQPtrList<TQWindowsMime> all();
    static TQWindowsMime* convertor( const char* mime, int cf );
    static const char* cfToMime(int cf);

    static int registerMimeType(const char *mime);

    virtual const char* convertorName()=0;
    virtual int countCf()=0;
    virtual int cf(int index)=0;
    virtual bool canConvert( const char* mime, int cf )=0;
    virtual const char* mimeFor(int cf)=0;
    virtual int cfFor(const char* )=0;
    virtual TQByteArray convertToMime( TQByteArray data, const char* mime, int cf )=0;
    virtual TQByteArray convertFromMime( TQByteArray data, const char* mime, int cf )=0;
};

#endif
#if defined(TQ_WS_MAC)

#ifndef QT_H
#include "ntqptrlist.h" // down here for GCC 2.7.* compatibility
#endif // QT_H

/*
  Encapsulation of conversion between MIME and Mac flavor.
  Not need on X11, as the underlying protocol uses the MIME standard
  directly.
*/

class TQ_EXPORT TQMacMime {
    char type;
public:
    enum TQMacMimeType { MIME_DND=0x01, MIME_CLIP=0x02, MIME_QT_CONVERTOR=0x04, MIME_ALL=MIME_DND|MIME_CLIP };
    TQMacMime(char);
    virtual ~TQMacMime();

    static void initialize();

    static TQPtrList<TQMacMime> all(TQMacMimeType);
    static TQMacMime* convertor(TQMacMimeType, const char* mime, int flav);
    static const char* flavorToMime(TQMacMimeType, int flav);

    virtual const char* convertorName()=0;
    virtual int countFlavors()=0;
    virtual int flavor(int index)=0;
    virtual bool canConvert(const char* mime, int flav)=0;
    virtual const char* mimeFor(int flav)=0;
    virtual int flavorFor(const char*)=0;
    virtual TQByteArray convertToMime(TQValueList<TQByteArray> data, const char* mime, int flav)=0;
    virtual TQValueList<TQByteArray> convertFromMime(TQByteArray data, const char* mime, int flav)=0;
};

#endif // TQ_WS_MAC

#endif // TQT_NO_MIME

#endif // TQMIME_H
