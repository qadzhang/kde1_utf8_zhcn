/****************************************************************************
**
** Drop site example implementation
**
** Created : 979899
**
** Copyright (C) 1997-2008 Trolltech ASA.  All rights reserved.
**
** This file is part of an example program for TQt.  This example
** program may be used, distributed and modified without limitation.
**
*****************************************************************************/

#include "dropsite.h"
#include "secret.h"
#include <ntqevent.h>
#include <ntqpixmap.h>
#include <ntqdragobject.h>
#include <ntqimage.h>
#include <ntqdir.h>


DropSite::DropSite( TQWidget * parent, const char * name )
    : TQLabel( parent, name )
{
    setAcceptDrops(true);
}


DropSite::~DropSite()
{
    // nothing necessary
}


void DropSite::dragMoveEvent( TQDragMoveEvent *e )
{
    // Check if you want the drag at e->pos()...
    // Give the user some feedback - only copy is possible
    e->acceptAction( e->action() == TQDropEvent::Copy );
}


void DropSite::dragEnterEvent( TQDragEnterEvent *e )
{
    // Check if you want the drag...
    if ( SecretDrag::canDecode( e )
      || TQTextDrag::canDecode( e )
      || TQImageDrag::canDecode( e )
      || TQUriDrag::canDecode( e ) )
    {
	e->accept();
    }


    // Give the user some feedback...
    TQString t;
    const char *f;
    for( int i=0; (f=e->format( i )); i++ ) {
	if ( *(f) ) {
	    if ( !t.isEmpty() )
		t += "\n";
	    t += f;
	}
    }
    emit message( t );
    setBackgroundColor(white);
}

void DropSite::dragLeaveEvent( TQDragLeaveEvent * )
{
    // Give the user some feedback...
    emit message("");
    setBackgroundColor(lightGray);
}


void DropSite::dropEvent( TQDropEvent * e )
{
    setBackgroundColor(lightGray);

    // Try to decode to the data you understand...
    TQStrList strings;
    if ( TQUriDrag::decode( e, strings ) ) {
	TQString m("Full URLs:\n");
	for (const char* u=strings.first(); u; u=strings.next())
	    m = m + "   " + u + '\n';
	TQStringList files;
	if ( TQUriDrag::decodeLocalFiles( e, files ) ) {
	    m += "Files:\n";
	    for (TQStringList::Iterator i=files.begin(); i!=files.end(); ++i)
		m = m + "   " + TQDir::convertSeparators(*i) + '\n';
	}
	setText( m );
	setMinimumSize( minimumSize().expandedTo( sizeHint() ) );
	return;
    }

    TQString str;
    if ( TQTextDrag::decode( e, str ) ) {
	setText( str );
	setMinimumSize( minimumSize().expandedTo( sizeHint() ) );
	return;
    }

    TQPixmap pm;
    if ( TQImageDrag::decode( e, pm ) ) {
	setPixmap( pm );
	setMinimumSize( minimumSize().expandedTo( sizeHint() ) );
	return;
    }

    if ( SecretDrag::decode( e, str ) ) {
	setText( str );
	setMinimumSize( minimumSize().expandedTo( sizeHint() ) );
	return;
    }
}

DragMoviePlayer::DragMoviePlayer( TQDragObject* p ) :
    TQObject(p),
    dobj(p),
    movie("trolltech.gif" )
{
    movie.connectUpdate(this,TQ_SLOT(updatePixmap(const TQRect&)));
}

void DragMoviePlayer::updatePixmap( const TQRect& )
{
    dobj->setPixmap(movie.framePixmap());
}

void DropSite::mousePressEvent( TQMouseEvent * /*e*/ )
{
    TQDragObject *drobj;
    if ( pixmap() ) {
	drobj = new TQImageDrag( pixmap()->convertToImage(), this );
#if 1
	TQPixmap pm;
	pm.convertFromImage(pixmap()->convertToImage().smoothScale(
	    pixmap()->width()/3,pixmap()->height()/3));
	drobj->setPixmap(pm,TQPoint(-5,-7));
#else
	// Try it.
	(void)new DragMoviePlayer(drobj);
#endif
    } else {
	drobj = new TQTextDrag( text(), this );
    }
    drobj->dragCopy();
}


void DropSite::backgroundColorChange( const TQColor & )
{
    // Reduce flicker by using repaint() rather than update()
    repaint();
}
