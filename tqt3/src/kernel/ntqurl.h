/****************************************************************************
**
** Definition of TQUrl class
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

#ifndef TQURL_H
#define TQURL_H

#ifndef QT_H
#include "ntqstring.h"
#endif // QT_H

#ifndef TQT_NO_URL

class TQUrlPrivate;

class TQ_EXPORT TQUrl
{
public:
    TQUrl();
    TQUrl( const TQString& url );
    TQUrl( const TQUrl& url );
    TQUrl( const TQUrl& url, const TQString& relUrl, bool checkSlash = false );
    virtual ~TQUrl();

    TQString protocol() const;
    virtual void setProtocol( const TQString& protocol );

    TQString user() const;
    virtual void setUser( const TQString& user );
    bool hasUser() const;

    TQString password() const;
    virtual void setPassword( const TQString& pass );
    bool hasPassword() const;

    TQString host() const;
    virtual void setHost( const TQString& user );
    bool hasHost() const;

    int port() const;
    virtual void setPort( int port );
    bool hasPort() const;

    TQString path( bool correct = true ) const;
    virtual void setPath( const TQString& path );
    bool hasPath() const;

    virtual void setEncodedPathAndQuery( const TQString& enc );
    TQString encodedPathAndQuery();

    virtual void setQuery( const TQString& txt );
    TQString query() const;

    TQString ref() const;
    virtual void setRef( const TQString& txt );
    bool hasRef() const;

    bool isValid() const;
    bool isLocalFile() const;

    virtual void addPath( const TQString& path );
    virtual void setFileName( const TQString& txt );

    TQString fileName() const;
    TQString dirPath() const;

    TQUrl& operator=( const TQUrl& url );
    TQUrl& operator=( const TQString& url );

    bool operator==( const TQUrl& url ) const;
    bool operator==( const TQString& url ) const;

    static void decode( TQString& url );
    static void encode( TQString& url );

    operator TQString() const;
    virtual TQString toString( bool encodedPath = false, bool forcePrependProtocol = true ) const;

    virtual bool cdUp();

    static bool isRelativeUrl( const TQString &url );

protected:
    virtual void reset();
    virtual bool parse( const TQString& url );

private:
    TQUrlPrivate *d;

};

#endif //TQT_NO_URL

#endif
