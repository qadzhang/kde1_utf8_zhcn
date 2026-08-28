/****************************************************************************
**
** Implementation of some internal classes
**
** Created : 010427
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

#include "private/qinternal_p.h"
#include "ntqwidget.h"
#include "ntqpixmap.h"
#include "ntqpainter.h"
#include "ntqcleanuphandler.h"

// Modern systems (year 2011) have very large screens in excess of 1000 pixels wide
// Some may even have screens in excess of 4000 pixels wide(!)
// Prevent drawing artifacts on such screens
#define USE_HUGE_QT_SHARED_DOUBLE_BUFFERS 1

static TQPixmap* qdb_shared_pixmap = 0;
static TQPixmap *qdb_force_pixmap = 0;
static TQSharedDoubleBuffer* qdb_owner = 0;

TQCleanupHandler<TQPixmap> qdb_pixmap_cleanup;

#ifdef TQ_WS_MACX
bool TQSharedDoubleBuffer::dblbufr = false;
#else
bool TQSharedDoubleBuffer::dblbufr = true;
#endif


/*
  hardLimitWidth/Height: if >= 0, the maximum number of pixels that
  get double buffered.

  sharedLimitWidth/Height: if >= 0, the maximum number of pixels the
  shared double buffer can keep.

  For x with sharedLimitSize < x <= hardLimitSize, temporary buffers
  are constructed.
 */
static const int hardLimitWidth = -1;
static const int hardLimitHeight = -1;
#if defined( TQ_WS_QWS ) || defined( TQ_WS_MAC9 )
// Small in TQt/Embedded / Mac9 - 5K on 32bpp
static const int sharedLimitWidth = 64;
static const int sharedLimitHeight = 20;
#else
#ifdef USE_HUGE_QT_SHARED_DOUBLE_BUFFERS
// 24M on 32bpp
static const int sharedLimitWidth = 6400;
static const int sharedLimitHeight = 1000;
#else
// 240K on 32bpp
static const int sharedLimitWidth = 640;
static const int sharedLimitHeight = 100;
#endif
#endif

// *******************************************************************
// TQSharedDoubleBufferCleaner declaration and implementation
// *******************************************************************

/* \internal
   This class is responsible for cleaning up the pixmaps created by the
   TQSharedDoubleBuffer class.  When TQSharedDoubleBuffer creates a
   pixmap larger than the shared limits, this class deletes it after a
   specified amount of time.

   When the large pixmap is created/used, you must call start(). If the
   large pixmap is ever deleted, you must call stop().  The start()
   method always restarts the timer, so if the large pixmap is
   constantly in use, the timer will never fire, and the pixmap will
   not be constantly created and destroyed.
*/

static const int shared_double_buffer_cleanup_timeout = 30000; // 30 seconds

// declaration

class TQSharedDoubleBufferCleaner : public TQObject
{
public:
    TQSharedDoubleBufferCleaner( void );

    void start( void );
    void stop( void );

    void doCleanup( void );

    bool event( TQEvent *e );

private:
    int timer_id;
};

// implementation

/* \internal
   Creates a TQSharedDoubleBufferCleaner object. The timer is not
   started when creating the object.
*/
TQSharedDoubleBufferCleaner::TQSharedDoubleBufferCleaner( void )
    : TQObject( 0, "internal shared double buffer cleanup object" ),
      timer_id( -1 )
{
}

/* \internal
   Starts the cleanup timer.  Any previously running timer is stopped.
*/
void TQSharedDoubleBufferCleaner::start( void )
{
    stop();
    timer_id = startTimer( shared_double_buffer_cleanup_timeout );
}

/* \internal
   Stops the cleanup timer, if it is running.
*/
void TQSharedDoubleBufferCleaner::stop( void )
{
    if ( timer_id != -1 )
	killTimer( timer_id );
    timer_id = -1;
}

/* \internal
 */
void TQSharedDoubleBufferCleaner::doCleanup( void )
{
    qdb_pixmap_cleanup.remove( &qdb_force_pixmap );
    delete qdb_force_pixmap;
    qdb_force_pixmap = 0;
}

/* \internal
   Event handler reimplementation.  Calls doCleanup() when the timer
   fires.
*/
bool TQSharedDoubleBufferCleaner::event( TQEvent *e )
{
    if ( e->type() != TQEvent::Timer )
	return false;

    TQTimerEvent *event = (TQTimerEvent *) e;
    if ( event->timerId() == timer_id ) {
	doCleanup();
	stop();
    }
#ifdef QT_CHECK_STATE
    else {
	tqWarning( "TQSharedDoubleBufferCleaner::event: invalid timer event received." );
	return false;
    }
#endif // QT_CHECK_STATE

    return true;
}

// static instance
static TQSharedDoubleBufferCleaner *static_cleaner = 0;
TQSingleCleanupHandler<TQSharedDoubleBufferCleaner> cleanup_static_cleaner;

inline static TQSharedDoubleBufferCleaner *staticCleaner()
{
    if ( ! static_cleaner ) {
	static_cleaner = new TQSharedDoubleBufferCleaner();
	cleanup_static_cleaner.set( &static_cleaner );
    }
    return static_cleaner;
}


// *******************************************************************
// TQSharedDoubleBuffer implementation
// *******************************************************************

/* \internal
   \enum DoubleBufferFlags

   \value InitBG initialize the background of the double buffer.

   \value Force disable shared buffer size limits.

   \value Default InitBG and Force are used by default.
*/

/* \internal
   \enum DoubleBufferState

   \value Active indicates that the buffer may be used.

   \value BufferActive indicates that painting with painter() will be
   double buffered.

   \value ExternalPainter indicates that painter() will return a
   painter that was not created by TQSharedDoubleBuffer.
*/

/* \internal
   \class TQSharedDoubleBuffer

   This class provides a single, reusable double buffer.  This class
   is used internally by TQt widgets that need double buffering, which
   prevents each individual widget form creating a double buffering
   pixmap.

   Using a single pixmap double buffer and sharing it across all
   widgets is nicer on window system resources.
*/

/* \internal
   Creates a TQSharedDoubleBuffer with flags \f.

   \sa DoubleBufferFlags
*/
TQSharedDoubleBuffer::TQSharedDoubleBuffer( DBFlags f )
    : wid( 0 ), rx( 0 ), ry( 0 ), rw( 0 ), rh( 0 ), flags( f ), state( 0 ),
      p( 0 ), external_p( 0 ), pix( 0 )
{
}

/* \internal
   Creates a TQSharedDoubleBuffer with flags \f. The \a widget, \a x,
   \a y, \a w and \a h arguments are passed to begin().

   \sa DoubleBufferFlags begin()
*/
TQSharedDoubleBuffer::TQSharedDoubleBuffer( TQWidget* widget,
					  int x, int y, int w, int h,
					  DBFlags f )
    : wid( 0 ), rx( 0 ), ry( 0 ), rw( 0 ), rh( 0 ), flags( f ), state( 0 ),
      p( 0 ), external_p( 0 ), pix( 0 )
{
    begin( widget, x, y, w, h );
}

/* \internal
   Creates a TQSharedDoubleBuffer with flags \f. The \a painter, \a x,
   \a y, \a w and \a h arguments are passed to begin().

   \sa DoubleBufferFlags begin()
*/
TQSharedDoubleBuffer::TQSharedDoubleBuffer( TQPainter* painter,
					  int x, int y, int w, int h,
					  DBFlags f)
    : wid( 0 ), rx( 0 ), ry( 0 ), rw( 0 ), rh( 0 ), flags( f ), state( 0 ),
      p( 0 ), external_p( 0 ), pix( 0 )
{
    begin( painter, x, y, w, h );
}

/* \internal
   Creates a TQSharedDoubleBuffer with flags \f. The \a widget and
   \a r arguments are passed to begin().

   \sa DoubleBufferFlags begin()
*/
TQSharedDoubleBuffer::TQSharedDoubleBuffer( TQWidget *widget, const TQRect &r, DBFlags f )
    : wid( 0 ), rx( 0 ), ry( 0 ), rw( 0 ), rh( 0 ), flags( f ), state( 0 ),
      p( 0 ), external_p( 0 ), pix( 0 )
{
    begin( widget, r );
}

/* \internal
   Creates a TQSharedDoubleBuffer with flags \f. The \a painter and
   \a r arguments are passed to begin().

   \sa DoubleBufferFlags begin()
*/
TQSharedDoubleBuffer::TQSharedDoubleBuffer( TQPainter *painter, const TQRect &r, DBFlags f )
    : wid( 0 ), rx( 0 ), ry( 0 ), rw( 0 ), rh( 0 ), flags( f ), state( 0 ),
      p( 0 ), external_p( 0 ), pix( 0 )
{
    begin( painter, r );
}

/* \internal
   Destructs the TQSharedDoubleBuffer and calls end() if the buffer is
   active.

   \sa isActive() end()
*/
TQSharedDoubleBuffer::~TQSharedDoubleBuffer()
{
    if ( isActive() )
        end();
}

/* \internal
   Starts double buffered painting in the area specified by \a x,
   \a y, \a w and \a h on \a painter.  Painting should be done using the
   TQPainter returned by TQSharedDoubleBuffer::painter().

   The double buffered area will be updated when calling end().

   \sa painter() isActive() end()
*/
bool TQSharedDoubleBuffer::begin( TQPainter* painter, int x, int y, int w, int h )
{
    if ( isActive() ) {
#if defined(QT_CHECK_STATE)
        tqWarning( "TQSharedDoubleBuffer::begin: Buffer is already active."
                  "\n\tYou must end() the buffer before a second begin()" );
#endif // QT_CHECK_STATE
        return false;
    }

    external_p = painter;

    if ( painter->device()->devType() == TQInternal::Widget )
	return begin( (TQWidget *) painter->device(), x, y, w, h );

    state = Active;

    rx = x;
    ry = y;
    rw = w;
    rh = h;

    if ( ( pix = getPixmap() ) ) {
#ifdef TQ_WS_X11
	if ( painter->device()->x11Screen() != pix->x11Screen() )
	    pix->x11SetScreen( painter->device()->x11Screen() );
	TQPixmap::x11SetDefaultScreen( pix->x11Screen() );
#endif // TQ_WS_X11

	state |= BufferActive;
	p = new TQPainter( pix );
	if ( p->isActive() ) {
	    p->setPen( external_p->pen() );
	    p->setBackgroundColor( external_p->backgroundColor() );
	    p->setFont( external_p->font() );
	}
    } else {
	state |= ExternalPainter;
	p = external_p;
    }

    return true;
}

/* \internal


   Starts double buffered painting in the area specified by \a x,
   \a y, \a w and \a h on \a widget.  Painting should be done using the
   TQPainter returned by TQSharedDoubleBuffer::painter().

   The double buffered area will be updated when calling end().

   \sa painter() isActive() end()
*/
bool TQSharedDoubleBuffer::begin( TQWidget* widget, int x, int y, int w, int h )
{
    if ( isActive() ) {
#if defined(QT_CHECK_STATE)
        tqWarning( "TQSharedDoubleBuffer::begin: Buffer is already active."
                  "\n\tYou must end() the buffer before a second begin()" );
#endif // QT_CHECK_STATE
        return false;
    }

    state = Active;

    wid = widget;
    rx = x;
    ry = y;
    rw = w <= 0 ? wid->width() : w;
    rh = h <= 0 ? wid->height() : h;

    if ( ( pix = getPixmap() ) ) {
#ifdef TQ_WS_X11
	if ( wid->x11Screen() != pix->x11Screen() )
	    pix->x11SetScreen( wid->x11Screen() );
	TQPixmap::x11SetDefaultScreen( pix->x11Screen() );
#endif // TQ_WS_X11

	state |= BufferActive;
	if ( flags & InitBG ) {
	    pix->fill( wid, rx, ry );
	}
	p = new TQPainter( pix, wid );
	// newly created painters should be translated to the origin
	// of the widget, so that paint methods can draw onto the double
	// buffered painter in widget coordinates.
	p->setBrushOrigin( -rx, -ry );
	p->translate( -rx, -ry );
    } else {
	if ( external_p ) {
	    state |= ExternalPainter;
	    p = external_p;
	} else {
	    p = new TQPainter( wid );
	}

	if ( flags & InitBG ) {
	    wid->erase( rx, ry, rw, rh );
	}
    }
    return true;
}

/* \internal
   Ends double buffered painting.  The contents of the shared double
   buffer pixmap are drawn onto the destination by calling flush(),
   and ownership of the shared double buffer pixmap is released.

   \sa begin() flush()
*/
bool TQSharedDoubleBuffer::end()
{
    if ( ! isActive() ) {
#if defined(QT_CHECK_STATE)
	tqWarning( "TQSharedDoubleBuffer::end: Buffer is not active."
		  "\n\tYou must call begin() before calling end()." );
#endif // QT_CHECK_STATE
	return false;
    }

    if ( ! ( state & ExternalPainter ) ) {
	p->end();
	delete p;
    }

    flush();

    if ( pix ) {
	releasePixmap();
    }

    wid = 0;
    rx = ry = rw = rh = 0;
    // do not reset flags!
    state = 0;

    p = external_p = 0;
    pix = 0;

    return true;
}

/* \internal
   Paints the contents of the shared double buffer pixmap onto the
   destination.  The destination is determined from the arguments
   based to begin().

   Note: You should not need to call this function, since it is called
   from end().

   \sa begin() end()
*/
void TQSharedDoubleBuffer::flush()
{
    if ( ! isActive() || ! ( state & BufferActive ) )
	return;

    if ( external_p )
	external_p->drawPixmap( rx, ry, *pix, 0, 0, rw, rh );
    else if ( wid && wid->isVisible() )
	bitBlt( wid, rx, ry, pix, 0, 0, rw, rh );
}

/* \internal
   Acquire ownership of the shared double buffer pixmap, subject to the
   following conditions:

   \list 1
   \i double buffering is enabled globally.
   \i the shared double buffer pixmap is not in use.
   \i the size specified in begin() is valid, and within limits.
   \endlist

   If all of these conditions are met, then this TQSharedDoubleBuffer
   object becomes the owner of the shared double buffer pixmap.  The
   shared double buffer pixmap is resize if necessary, and this
   function returns a pointer to the pixmap.  Ownership must later be
   relinquished by calling releasePixmap().

   If none of the above conditions are met, this function returns
   zero.

   \sa releasePixmap()
*/
TQPixmap *TQSharedDoubleBuffer::getPixmap()
{
    if ( isDisabled() ) {
	// double buffering disabled globally
	return 0;
    }

    if ( qdb_owner ) {
	// shared pixmap already in use
	return 0;
    }

    if ( rw <= 0 || rh <= 0 ||
	 ( hardLimitWidth > 0 && rw >= hardLimitWidth ) ||
	 ( hardLimitHeight > 0 && rh >= hardLimitHeight ) ) {
	// invalid size, or hard limit reached
	return 0;
    }

    if ( rw >= sharedLimitWidth || rh >= sharedLimitHeight ) {
	if ( flags & Force ) {
#ifdef USE_HUGE_QT_SHARED_DOUBLE_BUFFERS
	    rw = TQMIN(rw, 16000);
	    rh = TQMIN(rh, 16000);
#else
	    rw = TQMIN(rw, 8000);
	    rh = TQMIN(rh, 8000);
#endif
	    // need to create a big pixmap and start the cleaner
	    if ( ! qdb_force_pixmap ) {
		qdb_force_pixmap = new TQPixmap( rw, rh );
		qdb_pixmap_cleanup.add( &qdb_force_pixmap );
	    } else if ( qdb_force_pixmap->width () < rw ||
			qdb_force_pixmap->height() < rh ) {
		qdb_force_pixmap->resize( rw, rh );
	    }
	    qdb_owner = this;
	    staticCleaner()->start();
	    return qdb_force_pixmap;
	}

	// size is outside shared limit
	return 0;
    }

    if ( ! qdb_shared_pixmap ) {
	qdb_shared_pixmap = new TQPixmap( rw, rh );
	qdb_pixmap_cleanup.add( &qdb_shared_pixmap );
    } else if ( qdb_shared_pixmap->width() < rw ||
		qdb_shared_pixmap->height() < rh ) {
	qdb_shared_pixmap->resize( rw, rh );
    }
    qdb_owner = this;
    return qdb_shared_pixmap;
}

/* \internal
   Releases ownership of the shared double buffer pixmap.

   \sa getPixmap()
*/
void TQSharedDoubleBuffer::releasePixmap()
{
    if ( qdb_owner != this ) {
	// sanity check

#ifdef QT_CHECK_STATE
	tqWarning( "TQSharedDoubleBuffer::releasePixmap: internal error."
		  "\n\t%p does not own shared pixmap, %p does.",
		  (void*)this, (void*)qdb_owner );
#endif // QT_CHECK_STATE

	return;
    }

    qdb_owner = 0;
}

/* \internal
   \fn bool TQSharedDoubleBuffer::isDisabled()

   Returns true if double buffering is disabled globally, false otherwise.
*/

/* \internal
   \fn void TQSharedDoubleBuffer::setDisabled( bool off )

   Disables global double buffering \a off is true, otherwise global
   double buffering is enabled.
*/

/* \internal
   Deletes the shared double buffer pixmap.  You should not need to
   call this function, since it is called from the TQApplication
   destructor.
*/
void TQSharedDoubleBuffer::cleanup()
{
    qdb_pixmap_cleanup.remove( &qdb_shared_pixmap );
    qdb_pixmap_cleanup.remove( &qdb_force_pixmap );
    delete qdb_shared_pixmap;
    delete qdb_force_pixmap;
    qdb_shared_pixmap = 0;
    qdb_force_pixmap = 0;
    qdb_owner = 0;
}

/* \internal
   \fn bool TQSharedDoubleBuffer::begin( TQWidget *widget, const TQRect &r )
   \overload
*/

/* \internal
   \fn bool TQSharedDoubleBuffer::begin( TQPainter *painter, const TQRect &r )
   \overload
*/

/* \internal
   \fn TQPainter *TQSharedDoubleBuffer::painter() const

   Returns the active painter on the double buffered area,
   or zero if double buffered painting is not active.
*/

/* \internal
   \fn bool TQSharedDoubleBuffer::isActive() const

   Returns true if double buffered painting is active, false otherwise.
*/

/* \internal
   \fn bool TQSharedDoubleBuffer::isBuffered() const

   Returns true if painting is double buffered, false otherwise.
*/


// *******************************************************************
// TQMembuf declaration and implementation
// *******************************************************************

/*  \internal
    This class implements an efficient buffering of data that is often used by
    asynchronous IO classes like TQSocket, TQHttp and TQProcess.
*/

TQMembuf::TQMembuf() : _size(0), _index(0)
{
    buf = new TQPtrList<TQByteArray>;
    buf->setAutoDelete( true );
}

TQMembuf::~TQMembuf()
{
    delete buf;
}

/*! \internal
    This function consumes \a nbytes bytes of data from the
    buffer and copies it into \a sink. If \a sink is a 0 pointer
    the data goes into the nirvana.
*/
bool TQMembuf::consumeBytes( TQ_ULONG nbytes, char *sink )
{
    if ( nbytes <= 0 || nbytes > _size )
	return false;
    _size -= nbytes;
    for ( ;; ) {
	TQByteArray *a = buf->first();
	if ( _index + nbytes >= a->size() ) {
	    // Here we skip the whole byte array and get the next later
	    int len = a->size() - _index;
	    if ( sink ) {
		memcpy( sink, a->data()+_index, len );
		sink += len;
	    }
	    nbytes -= len;
	    buf->remove();
	    _index = 0;
	    if ( nbytes == 0 )
		break;
	} else {
	    // Here we skip only a part of the first byte array
	    if ( sink )
		memcpy( sink, a->data()+_index, nbytes );
	    _index += nbytes;
	    break;
	}
    }
    return true;
}

/*! \internal
    Scans for any occurrence of '\n' in the buffer. If \a store
    is not 0 the text up to the first '\n' (or terminating 0) is
    written to \a store, and a terminating 0 is appended to \a store
    if necessary. Returns true if a '\n' was found; otherwise returns
    false.
*/
bool TQMembuf::scanNewline( TQByteArray *store )
{
    if ( _size == 0 )
	return false;
    int i = 0; // index into 'store'
    TQByteArray *a = 0;
    char *p;
    int n;
    for ( ;; ) {
	if ( !a ) {
	    a = buf->first();
	    if ( !a || a->size() == 0 )
		return false;
	    p = a->data() + _index;
	    n = a->size() - _index;
	} else {
	    a = buf->next();
	    if ( !a || a->size() == 0 )
		return false;
	    p = a->data();
	    n = a->size();
	}
	if ( store ) {
	    while ( n-- > 0 ) {
		*(store->data()+i) = *p;
		if ( ++i == (int)store->size() )
		    store->resize( store->size() < 256
				   ? 1024 : store->size()*4 );
		switch ( *p ) {
		    case '\0':
			store->resize( i );
			return false;
		    case '\n':
			*(store->data()+i) = '\0';
			store->resize( i );
			return true;
		}
		p++;
	    }
	} else {
	    while ( n-- > 0 ) {
		switch ( *p++ ) {
		    case '\0':
			return false;
		    case '\n':
			return true;
		}
	    }
	}
    }
}

int TQMembuf::ungetch( int ch )
{
    if ( buf->isEmpty() || _index==0 ) {
	// we need a new TQByteArray
	TQByteArray *ba = new TQByteArray( 1 );
	buf->insert( 0, ba );
	_size++;
	ba->at( 0 ) = ch;
    } else {
	// we can reuse a place in the buffer
	TQByteArray *ba = buf->first();
	_index--;
	_size++;
	ba->at( _index ) = ch;
    }
    return ch;
}
