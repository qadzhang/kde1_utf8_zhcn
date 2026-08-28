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

#include "pixmapcollection.h"
#include "project.h"
#include "mainwindow.h"
#include <ntqmime.h>
#include <ntqdir.h>
#include <ntqfileinfo.h>
#include <ntqtextstream.h>
#include <ntqimage.h>

PixmapCollection::PixmapCollection( Project *pro )
    : project( pro )
{
    iface = new DesignerPixmapCollectionImpl( this );
    mimeSourceFactory = new TQMimeSourceFactory();
}

PixmapCollection::~PixmapCollection()
{
    delete mimeSourceFactory;
    delete iface;
}

bool PixmapCollection::addPixmap( const Pixmap &pix, bool force )
{
    Pixmap pixmap = pix;
    savePixmap( pixmap );

    if ( !force ) {
	for ( TQValueList<Pixmap>::Iterator it = pixList.begin(); it != pixList.end(); ++it ) {
	    if ( (*it).name == pixmap.name )
		return false;
	}
    }

    pixList.append( pixmap );
    mimeSourceFactory->setPixmap( pixmap.name, pixmap.pix );
    project->setModified( true );
    return true;
}

void PixmapCollection::removePixmap( const TQString &name )
{
    for ( TQValueList<Pixmap>::Iterator it = pixList.begin(); it != pixList.end(); ++it ) {
	if ( (*it).name == name ) {
	    pixList.remove( it );
	    break;
	}
    }
    project->setModified( true );
}

TQValueList<PixmapCollection::Pixmap> PixmapCollection::pixmaps() const
{
    return pixList;
}

TQString PixmapCollection::unifyName( const TQString &n )
{
    TQString name = n;
    bool restart = false;
    int added = 1;

    for ( TQValueList<Pixmap>::Iterator it = pixList.begin(); it != pixList.end(); ++it ) {
	if ( restart )
	    it = pixList.begin();
	restart = false;
	if ( name == (*it).name ) {
	    name = n;
	    name += "_" + TQString::number( added );
	    ++added;
	    restart = true;
	}
    }

    return name;
}

void PixmapCollection::setActive( bool b )
{
    if ( b )
	TQMimeSourceFactory::defaultFactory()->addFactory( mimeSourceFactory );
    else
	TQMimeSourceFactory::defaultFactory()->removeFactory( mimeSourceFactory );
}

TQPixmap PixmapCollection::pixmap( const TQString &name )
{
    for ( TQValueList<Pixmap>::Iterator it = pixList.begin(); it != pixList.end(); ++it ) {
	if ( (*it).name == name )
	    return (*it).pix;
    }
    return TQPixmap();
}

void PixmapCollection::savePixmap( Pixmap &pix )
{
    if ( pix.absname == imageDir() + "/" + pix.name )
	return; // no need to save, it is already there
    TQString rel = project->makeRelative( pix.absname );
    if ( rel[0] == '/' || ( rel[1] == ':' && rel[2] == '/' ) ) { // only save if file is outside the project
	mkdir();
	pix.name = unifyName( TQFileInfo( pix.absname ).baseName() ) + ".png";
	pix.absname = imageDir() + "/" + pix.name;
	pix.pix.save( pix.absname, "PNG" );
    } else if ( rel.isEmpty() ) {
	mkdir();
	pix.name = unifyName( pix.name );
	pix.absname = imageDir() + "/" + pix.name;
	pix.pix.save( pix.absname, "PNG" );
    }
}

TQString PixmapCollection::imageDir() const
{
    return TQFileInfo( project->fileName() ).dirPath( true ) + "/images";
}

void PixmapCollection::mkdir()
{
    TQString f = project->fileName();
    TQDir d( TQFileInfo( f ).dirPath( true ) );
    d.mkdir( "images" );
}

void PixmapCollection::load( const TQString& filename )
{
    if ( filename.isEmpty() )
	return;
    TQString absfile;
    if ( filename[0] == '/' )
	absfile = filename;
    else
	absfile = TQFileInfo( project->fileName() ).dirPath( true ) + "/" + filename;

    TQPixmap pm( absfile );
    if ( pm.isNull() )
	return;

    Pixmap pix;
    pix.name = TQFileInfo( absfile ).fileName();
    pix.absname = absfile;
    pix.pix = pm;
    addPixmap( pix, true );
}

DesignerPixmapCollection *PixmapCollection::iFace()
{
    return iface;
}

bool PixmapCollection::isEmpty() const
{
    return pixList.isEmpty();
}
