/**********************************************************************
** Copyright (C) 2005-2008 Trolltech ASA.  All rights reserved.
**
** This file is part of TQt Designer.
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

#ifndef PIXMAPCOLLECTION_H
#define PIXMAPCOLLECTION_H

#include <ntqstring.h>
#include <ntqpixmap.h>
#include <ntqvaluelist.h>
#include "designerappiface.h"

class TQMimeSourceFactory;
class Project;

class PixmapCollection
{
public:
    struct Pixmap
    {
	TQPixmap pix;
	TQString name;
	TQString absname;
    };

    PixmapCollection( Project *pro );
    ~PixmapCollection();

    bool addPixmap( const Pixmap &pix, bool force = true );
    void removePixmap( const TQString &name );
    TQPixmap pixmap( const TQString &name );

    TQValueList<Pixmap> pixmaps() const;
    bool isEmpty() const;

    void setActive( bool b );

    void load( const TQString& filename );

    DesignerPixmapCollection *iFace();

private:
    TQString unifyName( const TQString &n );
    void savePixmap( Pixmap &pix );

    TQString imageDir() const;
    void mkdir();

private:
    TQValueList<Pixmap> pixList;
    TQMimeSourceFactory *mimeSourceFactory;
    Project *project;
    DesignerPixmapCollectionImpl *iface;

};

#endif
