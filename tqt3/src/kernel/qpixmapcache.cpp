/****************************************************************************
**
** Implementation of TQPixmapCache class
**
** Created : 950504
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

#include "ntqpixmapcache.h"
#include "ntqcache.h"
#include "ntqobject.h"
#include "ntqcleanuphandler.h"


// REVISED: paul
/*!
    \class TQPixmapCache ntqpixmapcache.h

    \brief The TQPixmapCache class provides an application-global cache for
    pixmaps.

    \ingroup environment
    \ingroup graphics
    \ingroup images

    This class is a tool for optimized drawing with TQPixmap. You can
    use it to store temporary pixmaps that are expensive to generate
    without using more storage space than cacheLimit(). Use insert()
    to insert pixmaps, find() to find them and clear() to empty the
    cache.

    For example, TQRadioButton has a non-trivial visual representation
    so we don't want to regenerate a pixmap whenever a radio button is
    displayed or changes state. In the function
    TQRadioButton::drawButton(), we do not draw the radio button
    directly. Instead, we first check the global pixmap cache for a
    pixmap with the key "$qt_radio_nnn_", where \c nnn is a numerical
    value that specifies the the radio button state. If a pixmap is
    found, we bitBlt() it onto the widget and return. Otherwise, we
    create a new pixmap, draw the radio button in the pixmap, and
    finally insert the pixmap in the global pixmap cache, using the
    key above. The bitBlt() is ten times faster than drawing the
    radio button. All radio buttons in the program share the cached
    pixmap since TQPixmapCache is application-global.

    TQPixmapCache contains no member data, only static functions to
    access the global pixmap cache. It creates an internal TQCache for
    caching the pixmaps.

    The cache associates a pixmap with a string (key). If two pixmaps
    are inserted into the cache using equal keys, then the last pixmap
    will hide the first pixmap. The TQDict and TQCache classes do
    exactly the same.

    The cache becomes full when the total size of all pixmaps in the
    cache exceeds cacheLimit(). The initial cache limit is 1024 KByte
    (1 MByte); it is changed with setCacheLimit(). A pixmap takes
    roughly width*height*depth/8 bytes of memory.

    See the \l TQCache documentation for more details about the cache
    mechanism.
*/


static const int cache_size = 149;		// size of internal hash array
#ifdef TQ_WS_MAC9
static int cache_limit	  = 256;		// 256 KB cache limit
#else
static int cache_limit	  = 1024;		// 1024 KB cache limit
#endif

class TQPMCache: public TQObject, public TQCache<TQPixmap>
{
public:
    TQPMCache():
	TQObject( 0, "global pixmap cache" ),
	TQCache<TQPixmap>( cache_limit * 1024, cache_size ),
	id( 0 ), ps( 0 ), t( false )
	{
	    setAutoDelete( true );
	}
   ~TQPMCache() {}
    void timerEvent( TQTimerEvent * );
    bool insert( const TQString& k, const TQPixmap *d, int c, int p = 0 );
private:
    int id;
    int ps;
    bool t;
};


/*
  This is supposed to cut the cache size down by about 80-90% in a
  minute once the application becomes idle, to let any inserted pixmap
  remain in the cache for some time before it becomes a candidate for
  cleaning-up, and to not cut down the size of the cache while the
  cache is in active use.

  When the last pixmap has been deleted from the cache, kill the
  timer so TQt won't keep the CPU from going into sleep mode.
*/

void TQPMCache::timerEvent( TQTimerEvent * )
{
    int mc = maxCost();
    bool nt = totalCost() == ps;
    setMaxCost( nt ? totalCost() * 3 / 4 : totalCost() -1 );
    setMaxCost( mc );
    ps = totalCost();

    if ( !count() ) {
	killTimer( id );
	id = 0;
    } else if ( nt != t ) {
	killTimer( id );
	id = startTimer( nt ? 10000 : 30000 );
	t = nt;
    }
}

bool TQPMCache::insert( const TQString& k, const TQPixmap *d, int c, int p )
{
    bool r = TQCache<TQPixmap>::insert( k, d, c, p );
    if ( r && !id ) {
	id = startTimer( 30000 );
	t = false;
    }
    return r;
}

static TQPMCache *pm_cache = 0;			// global pixmap cache

static TQSingleCleanupHandler<TQPMCache> qpm_cleanup_cache;

/*!
    Returns the pixmap associated with the \a key in the cache, or
    null if there is no such pixmap.

    \warning If valid, you should copy the pixmap immediately (this is
    fast). Subsequent insertions into the cache could cause the
    pointer to become invalid. For this reason, we recommend you use
    find(const TQString&, TQPixmap&) instead.

    Example:
    \code
	TQPixmap* pp;
	TQPixmap p;
	if ( (pp=TQPixmapCache::find("my_big_image", pm)) ) {
	    p = *pp;
	} else {
	    p.load("bigimage.png");
	    TQPixmapCache::insert("my_big_image", new TQPixmap(p));
	}
	painter->drawPixmap(0, 0, p);
    \endcode
*/

TQPixmap *TQPixmapCache::find( const TQString &key )
{
    return pm_cache ? pm_cache->find(key) : 0;
}


/*!
    \overload

    Looks for a cached pixmap associated with the \a key in the cache.
    If a pixmap is found, the function sets \a pm to that pixmap and
    returns true; otherwise leaves \a pm alone and returns false.

    Example:
    \code
	TQPixmap p;
	if ( !TQPixmapCache::find("my_big_image", pm) ) {
	    pm.load("bigimage.png");
	    TQPixmapCache::insert("my_big_image", pm);
	}
	painter->drawPixmap(0, 0, p);
    \endcode
*/

bool TQPixmapCache::find( const TQString &key, TQPixmap& pm )
{
    TQPixmap* p = pm_cache ? pm_cache->find(key) : 0;
    if ( p ) pm = *p;
    return !!p;
}


/*!
  \obsolete
  Inserts the pixmap \a pm associated with \a key into the cache.
  Returns true if successful, or false if the pixmap is too big for the cache.

  <strong>
    Note: \a pm must be allocated on the heap (using \c new).

    If this function returns false, you must delete \a pm yourself.

    If this function returns true, do not use \a pm afterwards or
    keep references to it because any other insertions into the cache,
    whether from anywhere in the application or within TQt itself, could cause
    the pixmap to be discarded from the cache and the pointer to
    become invalid.

    Due to these dangers, we strongly recommend that you use
    insert(const TQString&, const TQPixmap&) instead.
  </strong>
*/

bool TQPixmapCache::insert( const TQString &key, TQPixmap *pm )
{
    if ( !pm_cache ) {				// create pixmap cache
	pm_cache = new TQPMCache;
	TQ_CHECK_PTR( pm_cache );
	qpm_cleanup_cache.set( &pm_cache );
    }
    return pm_cache->insert( key, pm, pm->width()*pm->height()*pm->depth()/8 );
}

/*!
    Inserts a copy of the pixmap \a pm associated with the \a key into
    the cache.

    All pixmaps inserted by the TQt library have a key starting with
    "$qt", so your own pixmap keys should never begin "$qt".

    When a pixmap is inserted and the cache is about to exceed its
    limit, it removes pixmaps until there is enough room for the
    pixmap to be inserted.

    The oldest pixmaps (least recently accessed in the cache) are
    deleted when more space is needed.

    \sa setCacheLimit().
*/

bool TQPixmapCache::insert( const TQString &key, const TQPixmap& pm )
{
    if ( !pm_cache ) {				// create pixmap cache
	pm_cache = new TQPMCache;
	TQ_CHECK_PTR( pm_cache );
	qpm_cleanup_cache.set( &pm_cache );
    }
    TQPixmap *p = new TQPixmap(pm);
    bool rt = pm_cache->insert( key, p, p->width()*p->height()*p->depth()/8 );
    if ( !rt )
	delete p;

    return rt;
}

/*!
    Returns the cache limit (in kilobytes).

    The default setting is 1024 kilobytes.

    \sa setCacheLimit().
*/

int TQPixmapCache::cacheLimit()
{
    return cache_limit;
}

/*!
    Sets the cache limit to \a n kilobytes.

    The default setting is 1024 kilobytes.

    \sa cacheLimit()
*/

void TQPixmapCache::setCacheLimit( int n )
{
#ifdef TQ_WS_MAC9
    if(n > 256)
        tqWarning("TQPixmapCache::setCacheLimit: Setting cache limits high is harmfull to mac9's health");
#endif
    cache_limit = n;
    if ( pm_cache )
	pm_cache->setMaxCost( 1024*cache_limit );
}


/*!
  Removes the pixmap associated with \a key from the cache.
*/
void TQPixmapCache::remove( const TQString &key )
{
    if ( pm_cache )
	pm_cache->remove( key );
}


/*!
    Removes all pixmaps from the cache.
*/

void TQPixmapCache::clear()
{
    if ( pm_cache )
	pm_cache->clear();
}
