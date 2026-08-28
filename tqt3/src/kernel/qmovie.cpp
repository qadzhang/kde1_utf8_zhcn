/****************************************************************************
**
** Implementation of movie classes
**
** Created : 970617
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

// #define QT_SAVE_MOVIE_HACK

#include "ntqtimer.h"
#include "ntqpainter.h"
#include "ntqptrlist.h"
#include "ntqbitmap.h"
#include "ntqmovie.h"
#include "ntqfile.h"
#include "ntqbuffer.h"
#include "ntqobject.h"
#include "ntqpixmapcache.h"

#ifndef TQT_NO_MOVIE

#ifdef TQ_WS_QWS
#include "qgfx_qws.h"
#endif

#include "ntqasyncio.h"
#include "ntqasyncimageio.h"

#include <stdlib.h>

/*!
    \class TQMovie ntqmovie.h
    \brief The TQMovie class provides incremental loading of animations or images, signalling as it progresses.

    \ingroup images
    \ingroup graphics
    \ingroup multimedia
    \mainclass

    The simplest way to display a TQMovie is to use a TQLabel and
    TQLabel::setMovie().

    A TQMovie provides a TQPixmap as the framePixmap(); connections can
    be made via connectResize() and connectUpdate() to receive
    notification of size and pixmap changes. All decoding is driven
    by the normal event-processing mechanisms.

    The movie begins playing as soon as the TQMovie is created
    (actually, once control returns to the event loop). When the last
    frame in the movie has been played, it may loop back to the start
    if such looping is defined in the input source.

    TQMovie objects are explicitly shared. This means that a TQMovie
    copied from another TQMovie will be displaying the same frame at
    all times. If one shared movie pauses, all pause. To make \e
    independent movies, they must be constructed separately.

    The set of data formats supported by TQMovie is determined by the
    decoder factories that have been installed; the format of the
    input is determined as the input is decoded.

    The supported formats are MNG (if TQt is configured with MNG
    support enabled) and GIF (if TQt is configured with GIF support
    enabled, see ntqgif.h).

    If TQt is configured to support GIF reading, we are required to
    state that "The Graphics Interchange Format(c) is the Copyright
    property of CompuServe Incorporated. GIF(sm) is a Service Mark
    property of CompuServe Incorporated.

    \warning If you are in a country that recognizes software patents
    and in which Unisys holds a patent on LZW compression and/or
    decompression and you want to use GIF, Unisys may require you to
    license that technology. Such countries include Canada, Japan,
    the USA, France, Germany, Italy and the UK.

    GIF support may be removed completely in a future version of TQt.
    We recommend using the MNG or PNG format.

    \img qmovie.png TQMovie

    \sa TQLabel::setMovie()
*/

/*!
    \enum TQMovie::Status

    \value SourceEmpty
    \value UnrecognizedFormat
    \value Paused
    \value EndOfFrame
    \value EndOfLoop
    \value EndOfMovie
    \value SpeedChanged
*/

class TQMoviePrivate : public TQObject, public TQShared,
		      private TQDataSink, private TQImageConsumer
{
    TQ_OBJECT

public: // for TQMovie

    // Creates a null Private
    TQMoviePrivate();

    // NOTE:  The ownership of the TQDataSource is transferred to the Private
    TQMoviePrivate(TQDataSource* src, TQMovie* movie, int bufsize);

    virtual ~TQMoviePrivate();

    bool isNull() const;

    // Initialize, possibly to the null state
    void init(bool fully);
    void flushBuffer();
    void updatePixmapFromImage();
    void updatePixmapFromImage(const TQPoint& off, const TQRect& area);
    void showChanges();

    // This as TQImageConsumer
    void changed(const TQRect& rect);
    void end();
    void preFrameDone(); //util func
    void frameDone();
    void frameDone(const TQPoint&, const TQRect& rect);
    void restartTimer();
    void setLooping(int l);
    void setFramePeriod(int milliseconds);
    void setSize(int w, int h);

    // This as TQDataSink
    int readyToReceive();
    void receive(const uchar* b, int bytecount);
    void eof();
    void pause();

signals:
    void sizeChanged(const TQSize&);
    void areaChanged(const TQRect&);
    void dataStatus(int);

public slots:
    void refresh();

public:
    TQMovie *that;
    TQWidget * display_widget;

    TQImageDecoder *decoder;

    // Cyclic buffer
    int buf_size;
    uchar *buffer;
    int buf_r, buf_w, buf_usage;

    int framenumber;
    int frameperiod;
    int speed;
    TQTimer *frametimer;
    int lasttimerinterval;
    int loop;
    bool movie_ended;
    bool dirty_cache;
    bool waitingForFrameTick;
    int stepping;
    TQRect changed_area;
    TQRect valid_area;
    TQDataPump *pump;
    TQDataSource *source;
    TQPixmap mypixmap;
    TQBitmap mymask;
    TQColor bg;

    int error;
    bool empty;

#ifdef QT_SAVE_MOVIE_HACK
  bool save_image;
  int image_number;
#endif
};


TQMoviePrivate::TQMoviePrivate()
{
    dirty_cache = false;
    buffer = 0;
    pump = 0;
    source = 0;
    decoder = 0;
    display_widget=0;
    buf_size = 0;
    init(false);
}

// NOTE:  The ownership of the TQDataSource is transferred to the Private
TQMoviePrivate::TQMoviePrivate(TQDataSource* src, TQMovie* movie, int bufsize) :
    that(movie),
    buf_size(bufsize)
{
    frametimer = new TQTimer(this);
    pump = src ? new TQDataPump(src, this) : 0;
    TQObject::connect(frametimer, TQ_SIGNAL(timeout()), this, TQ_SLOT(refresh()));
    dirty_cache = false;
    source = src;
    buffer = 0;
    decoder = 0;
    speed = 100;
    display_widget=0;
    init(true);
}

TQMoviePrivate::~TQMoviePrivate()
{
    if ( buffer )				// Avoid purify complaint
	delete [] buffer;
    delete pump;
    delete decoder;
    delete source;

    // Too bad.. but better be safe than sorry
    if ( dirty_cache )
        TQPixmapCache::clear();
}

bool TQMoviePrivate::isNull() const
{
    return !buf_size;
}

// Initialize. Only actually allocate any space if \a fully is true,
// otherwise, just enough to be a valid null Private.
void TQMoviePrivate::init(bool fully)
{
#ifdef QT_SAVE_MOVIE_HACK
    save_image = true;
    image_number = 0;
#endif

    buf_usage = buf_r = buf_w = 0;
    if ( buffer )				// Avoid purify complaint
	delete [] buffer;
    buffer = fully ? new uchar[buf_size] : 0;
    if ( buffer )
	memset( buffer, 0, buf_size );

    delete decoder;
    decoder = fully ? new TQImageDecoder(this) : 0;

#ifdef AVOID_OPEN_FDS
    if ( source && !source->isOpen() )
	source->open(IO_ReadOnly);
#endif

    waitingForFrameTick = false;
    stepping = -1;
    framenumber = 0;
    frameperiod = -1;
    if (fully) frametimer->stop();
    lasttimerinterval = -1;
    changed_area.setRect(0,0,-1,-1);
    valid_area = changed_area;
    loop = -1;
    movie_ended = false;
    error = 0;
    empty = true;
}

void TQMoviePrivate::flushBuffer()
{
    int used;
    while (buf_usage && !waitingForFrameTick && stepping != 0 && !error) {
	used = decoder->decode(buffer + buf_r, TQMIN(buf_usage, buf_size - buf_r));
	if (used <= 0) {
	    if ( used < 0 ) {
		error = 1;
		emit dataStatus(TQMovie::UnrecognizedFormat);
	    }
	    break;
	}
	buf_r = (buf_r + used) % buf_size;
	buf_usage -= used;
    }

    // Some formats, like MNG, can make stuff happen without any extra data.
    // Only do this if the movie hasn't ended, however or we'll never get the end of loop signal.
    if (!movie_ended) {
	used = decoder->decode(buffer + buf_r, 0);
	if (used <= 0) {
	    if ( used < 0 ) {
		error = 1;
		emit dataStatus(TQMovie::UnrecognizedFormat);
	    }
	}
    }

    if (error)
	frametimer->stop();
    maybeReady();
}

void TQMoviePrivate::updatePixmapFromImage()
{
    if (changed_area.isEmpty()) return;
    updatePixmapFromImage(TQPoint(0,0),changed_area);
}

void TQMoviePrivate::updatePixmapFromImage(const TQPoint& off,
						const TQRect& area)
{
    // Create temporary TQImage to hold the part we want
    const TQImage& gimg = decoder->image();
    TQImage img = gimg.copy(area);

#ifdef QT_SAVE_MOVIE_HACK
    if ( save_image ) {
	TQString name;
	name.sprintf("movie%i.ppm",image_number++);
	gimg.save( name, "PPM" );
    }
#endif

    // Resize to size of image
    if (mypixmap.width() != gimg.width() || mypixmap.height() != gimg.height())
	mypixmap.resize(gimg.width(), gimg.height());

    // Convert to pixmap and paste that onto myself
    TQPixmap lines;

#ifndef TQT_NO_SPRINTF
    if (!(frameperiod < 0 && loop == -1)) {
        // its an animation, lets see if we converted
        // this frame already.
        TQString key;
        key.sprintf( "%08lx:%04d", ( long )this, framenumber );
        if ( !TQPixmapCache::find( key, lines ) ) {
            lines.convertFromImage(img, TQt::ColorOnly);
            TQPixmapCache::insert( key, lines );
            dirty_cache = true;
        }
    } else
#endif
    {
        lines.convertFromImage(img, TQt::ColorOnly);
    }

    if (bg.isValid()) {
	TQPainter p;
	p.begin(&mypixmap);
	p.fillRect(area, bg);
	p.drawPixmap(area, lines);
	p.end();
    } else {
	if (gimg.hasAlphaBuffer()) {
	    // Resize to size of image
	    if (mymask.isNull()) {
		mymask.resize(gimg.width(), gimg.height());
		mymask.fill( TQt::color1 );
	    }
	}
	mypixmap.setMask(TQBitmap()); // Remove reference to my mask
	copyBlt( &mypixmap, area.left(), area.top(),
		 &lines, off.x(), off.y(), area.width(), area.height() );
    }

#ifdef TQ_WS_QWS
    if(display_widget) {
	TQGfx * mygfx=display_widget->graphicsContext();
	if(mygfx) {
	    double xscale,yscale;
	    xscale=display_widget->width();
	    yscale=display_widget->height();
	    xscale=xscale/((double)mypixmap.width());
	    yscale=yscale/((double)mypixmap.height());
	    double xh,yh;
	    xh=xscale*((double)area.left());
	    yh=yscale*((double)area.top());
	    mygfx->setSource(&mypixmap);
	    mygfx->setAlphaType(TQGfx::IgnoreAlpha);
	    mygfx->stretchBlt(0,0,display_widget->width(),
			      display_widget->height(),mypixmap.width(),
			      mypixmap.height());
	    delete mygfx;
	}
    }
#endif
}

void TQMoviePrivate::showChanges()
{
    if (changed_area.isValid()) {
	updatePixmapFromImage();

	valid_area = valid_area.unite(changed_area);
	emit areaChanged(changed_area);

	changed_area.setWidth(-1); // make empty
    }
}

// Private as TQImageConsumer
void TQMoviePrivate::changed(const TQRect& rect)
{
    if (!frametimer->isActive())
	frametimer->start(0);
    changed_area = changed_area.unite(rect);
}

void TQMoviePrivate::end()
{
    movie_ended = true;
}

void TQMoviePrivate::preFrameDone()
{
    if (stepping > 0) {
	stepping--;
	if (!stepping) {
	    frametimer->stop();
	    emit dataStatus( TQMovie::Paused );
	}
    } else {
	waitingForFrameTick = true;
	restartTimer();
    }
}
void TQMoviePrivate::frameDone()
{
    preFrameDone();
    showChanges();
    emit dataStatus(TQMovie::EndOfFrame);
    framenumber++;
}
void TQMoviePrivate::frameDone(const TQPoint& p,
				const TQRect& rect)
{
    preFrameDone();
    const TQImage& gimg = decoder->image();
    TQPoint point = p - gimg.offset();
    if (framenumber==0)
	emit sizeChanged(gimg.size());
    valid_area = valid_area.unite(TQRect(point,rect.size()));
    updatePixmapFromImage(point,rect);
    emit areaChanged(TQRect(point,rect.size()));
    emit dataStatus(TQMovie::EndOfFrame);
    framenumber++;
}

void TQMoviePrivate::restartTimer()
{
    if (speed > 0) {
	int i = frameperiod >= 0 ? frameperiod * 100/speed : 0;
	if ( i != lasttimerinterval || !frametimer->isActive() ) {
	    lasttimerinterval = i;
	    frametimer->start( i );
	}
    } else {
	frametimer->stop();
    }
}

void TQMoviePrivate::setLooping(int nloops)
{
    if (loop == -1) { // Only if we don't already know how many loops!
	if (source && source->rewindable()) {
	    source->enableRewind(true);
	    loop = nloops;
	} else {
	    // Cannot loop from this source
	    loop = -2;
	}
    }
}

void TQMoviePrivate::setFramePeriod(int milliseconds)
{
    // Animation:  only show complete frame
    frameperiod = milliseconds;
    if (stepping<0 && frameperiod >= 0) restartTimer();
}

void TQMoviePrivate::setSize(int w, int h)
{
    if (mypixmap.width() != w || mypixmap.height() != h) {
	mypixmap.resize(w, h);
	emit sizeChanged(TQSize(w, h));
    }
}


// Private as TQDataSink

int TQMoviePrivate::readyToReceive()
{
    // Could pre-fill buffer, but more efficient to just leave the
    // data back at the source.
    return (waitingForFrameTick || !stepping || buf_usage || error)
	? 0 : buf_size;
}

void TQMoviePrivate::receive(const uchar* b, int bytecount)
{
    if ( bytecount ) empty = false;

    while (bytecount && !waitingForFrameTick && stepping != 0) {
	int used = decoder->decode(b, bytecount);
	if (used<=0) {
	    if ( used < 0 ) {
		error = 1;
		emit dataStatus(TQMovie::UnrecognizedFormat);
	    }
	    break;
	}
	b+=used;
	bytecount-=used;
    }

    // Append unused to buffer
    while (bytecount--) {
	buffer[buf_w] = *b++;
	buf_w = (buf_w+1)%buf_size;
	buf_usage++;
    }
}

void TQMoviePrivate::eof()
{
    if ( !movie_ended )
	return;

    if ( empty )
	emit dataStatus(TQMovie::SourceEmpty);

#ifdef QT_SAVE_MOVIE_HACK
    save_image = false;
#endif

    emit dataStatus(TQMovie::EndOfLoop);

    if (loop >= 0) {
	if (loop) {
	    loop--;
	    if (!loop) return;
	}
	delete decoder;
	decoder = new TQImageDecoder(this);
	source->rewind();
	framenumber = 0;
	movie_ended = false;
    } else {
	delete decoder;
	decoder = 0;
	if ( buffer )				// Avoid purify complaint
	    delete [] buffer;
	buffer = 0;
	emit dataStatus(TQMovie::EndOfMovie);
#ifdef AVOID_OPEN_FDS
	if ( source )
	    source->close();
#endif
    }
}

void TQMoviePrivate::pause()
{
    if ( stepping ) {
	stepping = 0;
	frametimer->stop();
	emit dataStatus( TQMovie::Paused );
    }
}

void TQMoviePrivate::refresh()
{
    if (!decoder) {
	frametimer->stop();
	return;
    }

    if (frameperiod < 0 && loop == -1) {
	// Only show changes if probably not an animation
	showChanges();
    }

    if (!buf_usage) {
	frametimer->stop();
    }

    waitingForFrameTick = false;
    flushBuffer();
}

///////////////// End of Private /////////////////





/*!
    Constructs a null TQMovie. The only interesting thing to do with
    such a movie is to assign another movie to it.

    \sa isNull()
*/
TQMovie::TQMovie()
{
    d = new TQMoviePrivate();
}

/*!
    Constructs a TQMovie with an external data source. You should later
    call pushData() to send incoming animation data to the movie.

    The \a bufsize argument sets the maximum amount of data the movie
    will transfer from the data source per event loop. The lower this
    value, the better interleaved the movie playback will be with
    other event processing, but the slower the overall processing will
    be.

    \sa pushData()
*/
TQMovie::TQMovie(int bufsize)
{
    d = new TQMoviePrivate(0, this, bufsize);
}

/*!
    Returns the maximum amount of data that can currently be pushed
    into the movie by a call to pushData(). This is affected by the
    initial buffer size, but varies as the movie plays and data is
    consumed.
*/
int TQMovie::pushSpace() const
{
    return d->readyToReceive();
}

/*!
    Pushes \a length bytes from \a data into the movie. \a length must
    be no more than the amount returned by pushSpace() since the
    previous call to pushData().
*/
void TQMovie::pushData(const uchar* data, int length)
{
    d->receive(data,length);
}

#ifdef TQ_WS_QWS // ##### Temporary performance experiment
/*!
    \internal
*/
void TQMovie::setDisplayWidget(TQWidget * w)
{
    d->display_widget=w;
}
#endif

/*!
    Constructs a TQMovie that reads an image sequence from the given
    data source, \a src. The source must be allocated dynamically,
    because TQMovie will take ownership of it and will destroy it when
    the movie is destroyed. The movie starts playing as soon as event
    processing continues.

    The \a bufsize argument sets the maximum amount of data the movie
    will transfer from the data source per event loop. The lower this
    value, the better interleaved the movie playback will be with
    other event processing, but the slower the overall processing will
    be.
*/
TQMovie::TQMovie(TQDataSource* src, int bufsize)
{
    d = new TQMoviePrivate(src, this, bufsize);
}

/*!
    Constructs a TQMovie that reads an image sequence from the file, \a
    fileName.

    The \a bufsize argument sets the maximum amount of data the movie
    will transfer from the data source per event loop. The lower this
    value, the better interleaved the movie playback will be with
    other event processing, but the slower the overall processing will
    be.
*/
TQMovie::TQMovie(const TQString &fileName, int bufsize)
{
    TQFile* file = new TQFile(fileName);
    if ( !fileName.isEmpty() )
 	file->open(IO_ReadOnly);
    d = new TQMoviePrivate(new TQIODeviceSource(file, bufsize), this, bufsize);
}

/*!
    Constructs a TQMovie that reads an image sequence from the byte
    array, \a data.

    The \a bufsize argument sets the maximum amount of data the movie
    will transfer from the data source per event loop. The lower this
    value, the better interleaved the movie playback will be with
    other event processing, but the slower the overall processing will
    be.
*/
TQMovie::TQMovie(TQByteArray data, int bufsize)
{
    TQBuffer* buffer = new TQBuffer(data);
    buffer->open(IO_ReadOnly);
    d = new TQMoviePrivate(new TQIODeviceSource(buffer, bufsize), this, bufsize);
}

/*!
    Constructs a movie that uses the same data as movie \a movie.
    TQMovies use explicit sharing, so operations on the copy will
    affect both.
*/
TQMovie::TQMovie(const TQMovie& movie)
{
    d = movie.d;
    d->ref();
}

/*!
    Destroys the TQMovie. If this is the last reference to the data of
    the movie, the data is deallocated.
*/
TQMovie::~TQMovie()
{
    if (d->deref()) delete d;
}

/*!
    Returns true if the movie is null; otherwise returns false.
*/
bool TQMovie::isNull() const
{
    return d->isNull();
}

/*!
    Makes this movie use the same data as movie \a movie. TQMovies use
    explicit sharing.
*/
TQMovie& TQMovie::operator=(const TQMovie& movie)
{
    movie.d->ref();
    if (d->deref()) delete d;
    d = movie.d;
    return *this;
}


/*!
    Sets the background color of the pixmap to \a c. If the background
    color isValid(), the pixmap will never have a mask because the
    background color will be used in transparent regions of the image.

    \sa backgroundColor()
*/
void TQMovie::setBackgroundColor(const TQColor& c)
{
    d->bg = c;
}

/*!
    Returns the background color of the movie set by
    setBackgroundColor().
*/
const TQColor& TQMovie::backgroundColor() const
{
    return d->bg;
}

/*!
    Returns the area of the pixmap for which pixels have been
    generated.
*/
const TQRect& TQMovie::getValidRect() const
{
    return d->valid_area;
}

/*!
    Returns the current frame of the movie, as a TQPixmap. It is not
    generally useful to keep a copy of this pixmap. It is better to
    keep a copy of the TQMovie and get the framePixmap() only when
    needed for drawing.

    \sa frameImage()
*/
const TQPixmap& TQMovie::framePixmap() const
{
    return d->mypixmap;
}

/*!
    Returns the current frame of the movie, as a TQImage. It is not
    generally useful to keep a copy of this image. Also note that you
    must not call this function if the movie is finished(), since by
    then the image will not be available.

    \sa framePixmap()
*/
const TQImage& TQMovie::frameImage() const
{
    return d->decoder->image();
}

/*!
    Returns the number of steps remaining after a call to step(). If
    the movie is paused, steps() returns 0. If it's running normally
    or is finished, steps() returns a negative number.
*/
int TQMovie::steps() const
{
    return d->stepping;
}

/*!
    Returns the number of times EndOfFrame has been emitted since the
    start of the current loop of the movie. Thus, before any
    EndOfFrame has been emitted the value will be 0; within slots
    processing the first signal, frameNumber() will be 1, and so on.
*/
int TQMovie::frameNumber() const { return d->framenumber; }

/*!
    Returns true if the image is paused; otherwise returns false.
*/
bool TQMovie::paused() const
{
    return d->stepping == 0;
}

/*!
    Returns true if the image is no longer playing: this happens when
    all loops of all frames are complete; otherwise returns false.
*/
bool TQMovie::finished() const
{
    return !d->decoder;
}

/*!
    Returns true if the image is not single-stepping, not paused, and
    not finished; otherwise returns false.
*/
bool TQMovie::running() const
{
    return d->stepping<0 && d->decoder;
}

/*!
    Pauses the progress of the animation.

    \sa unpause()
*/
void TQMovie::pause()
{
    d->pause();
}

/*!
    Unpauses the progress of the animation.

    \sa pause()
*/
void TQMovie::unpause()
{
    if ( d->stepping >= 0 )	{
	if (d->isNull())
	    return;
	d->stepping = -1;
	d->restartTimer();
    }
}

/*!
    \overload

    Steps forward, showing \a steps frames, and then pauses.
*/
void TQMovie::step(int steps)
{
    if (d->isNull())
	return;
    d->stepping = steps;
    d->frametimer->start(0);
    d->waitingForFrameTick = false; // Full speed ahead!
}

/*!
    Steps forward 1 frame and then pauses.
*/
void TQMovie::step()
{
    step(1);
}

/*!
    Rewinds the movie to the beginning. If the movie has not been
    paused, it begins playing again.
*/
void TQMovie::restart()
{
    if (d->isNull())
	return;
    if (d->source->rewindable()) {
	d->source->enableRewind(true);
	d->source->rewind();
	int s = d->stepping;
	d->init(true);
	if ( s>0 )
	    step(s);
	else if ( s==0 )
	    pause();
    }
}

/*!
    Returns the movie's play speed as a percentage. The default is 100
    percent.

    \sa setSpeed()
*/
int TQMovie::speed() const
{
    return d->speed;
}

/*!
    Sets the movie's play speed as a percentage, to \a percent. This
    is a percentage of the speed dictated by the input data format.
    The default is 100 percent.
*/
void TQMovie::setSpeed(int percent)
{
    int oldspeed = d->speed;
    if ( oldspeed != percent && percent >= 0 ) {
	d->speed = percent;
	// Restart timer only if really needed
	if (d->stepping < 0) {
	    if ( !percent || !oldspeed    // To or from zero
		 || oldspeed*4 / percent > 4   // More than 20% slower
		 || percent*4 / oldspeed > 4   // More than 20% faster
		 )
		d->restartTimer();
	}
    }
}

/*!
    Connects the \a{receiver}'s \a member of type \c{void member(const
    TQSize&)} so that it is signalled when the movie changes size.

    Note that due to the explicit sharing of TQMovie objects, these
    connections persist until they are explicitly disconnected with
    disconnectResize() or until \e every shared copy of the movie is
    deleted.
*/
void TQMovie::connectResize(TQObject* receiver, const char *member)
{
    TQObject::connect(d, TQ_SIGNAL(sizeChanged(const TQSize&)), receiver, member);
}

/*!
    Disconnects the \a{receiver}'s \a member (or all members if \a
    member is zero) that were previously connected by connectResize().
*/
void TQMovie::disconnectResize(TQObject* receiver, const char *member)
{
    TQObject::disconnect(d, TQ_SIGNAL(sizeChanged(const TQSize&)), receiver, member);
}

/*!
    Connects the \a{receiver}'s \a member of type \c{void member(const
    TQRect&)} so that it is signalled when an area of the framePixmap()
    has changed since the previous frame.

    Note that due to the explicit sharing of TQMovie objects, these
    connections persist until they are explicitly disconnected with
    disconnectUpdate() or until \e every shared copy of the movie is
    deleted.
*/
void TQMovie::connectUpdate(TQObject* receiver, const char *member)
{
    TQObject::connect(d, TQ_SIGNAL(areaChanged(const TQRect&)), receiver, member);
}

/*!
    Disconnects the \a{receiver}'s \a member (or all members if \q
    member is zero) that were previously connected by connectUpdate().
*/
void TQMovie::disconnectUpdate(TQObject* receiver, const char *member)
{
    TQObject::disconnect(d, TQ_SIGNAL(areaChanged(const TQRect&)), receiver, member);
}

/*!
    Connects the \a{receiver}'s \a member, of type \c{void
    member(int)} so that it is signalled when the movie changes
    status. The status codes are negative for errors and positive for
    information.

    \table
    \header \i Status Code \i Meaning
    \row \i TQMovie::SourceEmpty
	 \i signalled if the input cannot be read.
    \row \i TQMovie::UnrecognizedFormat
	 \i signalled if the input data is unrecognized.
    \row \i TQMovie::Paused
	  \i signalled when the movie is paused by a call to paused()
	  or by after \link step() stepping \endlink pauses.
    \row \i TQMovie::EndOfFrame
	 \i signalled at end-of-frame after any update and Paused signals.
    \row \i TQMovie::EndOfLoop
	 \i signalled at end-of-loop, after any update signals,
	 EndOfFrame - but before EndOfMovie.
    \row \i TQMovie::EndOfMovie
	 \i signalled when the movie completes and is not about to loop.
    \endtable

    More status messages may be added in the future, so a general test
    for errors would test for negative.

    Note that due to the explicit sharing of TQMovie objects, these
    connections persist until they are explicitly disconnected with
    disconnectStatus() or until \e every shared copy of the movie is
    deleted.
*/
void TQMovie::connectStatus(TQObject* receiver, const char *member)
{
    TQObject::connect(d, TQ_SIGNAL(dataStatus(int)), receiver, member);
}

/*!
    Disconnects the \a{receiver}'s \a member (or all members if \a
    member is zero) that were previously connected by connectStatus().
*/
void TQMovie::disconnectStatus(TQObject* receiver, const char *member)
{
    TQObject::disconnect(d, TQ_SIGNAL(dataStatus(int)), receiver, member);
}


#include "qmovie.moc"

#endif	// TQT_NO_MOVIE
