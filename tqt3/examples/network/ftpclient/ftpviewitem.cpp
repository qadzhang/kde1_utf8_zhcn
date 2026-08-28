/****************************************************************************
**
** Copyright (C) 1992-2008 Trolltech ASA.  All rights reserved.
**
** This file is part of an example program for TQt.  This example
** program may be used, distributed and modified without limitation.
**
*****************************************************************************/

#include <ntqpixmap.h>

#include "ftpviewitem.h"


FtpViewItem::FtpViewItem( TQListView *parent, Type t, const TQString &name, const TQString &size, const TQString &lastModified )
    : TQListViewItem(parent,name,size,lastModified), type(t)
{
    // the pixmaps for folders and files are in an image collection
    if ( type == Directory )
	setPixmap( 0, TQPixmap::fromMimeSource( "folder.png" ) );
    else
	setPixmap( 0, TQPixmap::fromMimeSource( "file.png" ) );
}

int FtpViewItem::compare( TQListViewItem * i, int col, bool ascending ) const
{
    // The entry ".." is always the first one.
    if ( text(0) == ".." ) {
	if ( ascending )
	    return -1;
	else
	    return 1;
    }
    if ( i->text(0) == ".." ) {
	if ( ascending )
	    return 1;
	else
	    return -1;
    }

    // Directories are before files.
    if ( type != ((FtpViewItem*)i)->type ) {
	if ( type == Directory ) {
	    if ( ascending )
		return -1;
	    else
		return 1;
	} else {
	    if ( ascending )
		return 1;
	    else
		return -1;
	}
    }

    // Use default sorting otherwise.
    return TQListViewItem::compare( i, col, ascending );
}
