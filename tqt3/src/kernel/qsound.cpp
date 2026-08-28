/****************************************************************************
**
** Implementation of TQSound class and TQAuServer internal class
**
** Created : 000117
**
** Copyright (C) 1999-2008 Trolltech ASA.  All rights reserved.
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

#include "ntqsound.h"

#ifndef TQT_NO_SOUND

#include "ntqptrlist.h"

static TQPtrList<TQAuServer> *servers=0;

TQAuServer::TQAuServer(TQObject* parent, const char* name) :
    TQObject(parent,name)
{
    if ( !servers ) {
	servers = new TQPtrList<TQAuServer>;
	// ### add cleanup
    }
    servers->prepend(this);
}

TQAuServer::~TQAuServer()
{
    servers->remove(this);
    if ( servers->count() == 0 ) {
	delete servers;
	servers = 0;
    }
}

void TQAuServer::play(const TQString& filename)
{
    TQSound s(filename);
    play(&s);
}

extern TQAuServer* tqt_new_audio_server();

static TQAuServer& server()
{
    if (!servers) tqt_new_audio_server();
    return *servers->first();
}

class TQSoundData {
public:
    TQSoundData(const TQString& fname) :
	filename(fname), bucket(0), looprem(0), looptotal(1)
    {
    }

    ~TQSoundData()
    {
	delete bucket;
    }

    TQString filename;
    TQAuBucket* bucket;
    int looprem;
    int looptotal;
};

/*!
    \class TQSound ntqsound.h
    \brief The TQSound class provides access to the platform audio facilities.

    \ingroup multimedia
    \mainclass

    TQt provides the most commonly required audio operation in GUI
    applications: asynchronously playing a sound file. This is most
    easily accomplished with a single call:
    \code
	TQSound::play("mysounds/bells.wav");
    \endcode

    A second API is provided in which a TQSound object is created from
    a sound file and is played later:
    \code
	TQSound bells("mysounds/bells.wav");

	bells.play();
    \endcode

    Sounds played using the second model may use more memory but play
    more immediately than sounds played using the first model,
    depending on the underlying platform audio facilities.

    On Microsoft Windows the underlying multimedia system is used;
    only WAVE format sound files are supported.

    On X11 the \link ftp://ftp.x.org/contrib/audio/nas/ Network Audio
    System\endlink is used if available, otherwise all operations work
    silently. NAS supports WAVE and AU files.

    On Macintosh, ironically, we use QT (\link
    http://quicktime.apple.com QuickTime\endlink) for sound, this
    means all QuickTime formats are supported by TQt/Mac.

    On TQt/Embedded, a built-in mixing sound server is used, which
    accesses \c /dev/dsp directly. Only the WAVE format is supported.

    The availability of sound can be tested with
    TQSound::isAvailable().
*/

/*!
    \fn static bool TQSound::available()

    Returns true if sound support is available; otherwise returns false.
*/

/*!
    Plays the sound in a file called \a filename.
*/
void TQSound::play(const TQString& filename)
{
    server().play(filename);
}

/*!
    Constructs a TQSound that can quickly play the sound in a file
    named \a filename.

    This may use more memory than the static \c play function.

    The \a parent and \a name arguments (default 0) are passed on to
    the TQObject constructor.
*/
TQSound::TQSound(const TQString& filename, TQObject* parent, const char* name) :
    TQObject(parent,name),
    d(new TQSoundData(filename))
{
    server().init(this);
}

/*!
    Destroys the sound object. If the sound is not finished playing stop() is called on it.

    \sa stop() isFinished()
*/
TQSound::~TQSound()
{
    if ( !isFinished() )
	stop();
    delete d;
}

/*!
    Returns true if the sound has finished playing; otherwise returns false.

    \warning On Windows this function always returns true for unlooped sounds.
*/
bool TQSound::isFinished() const
{
    return d->looprem == 0;
}

/*!
    \overload

    Starts the sound playing. The function returns immediately.
    Depending on the platform audio facilities, other sounds may stop
    or may be mixed with the new sound.

    The sound can be played again at any time, possibly mixing or
    replacing previous plays of the sound.
*/
void TQSound::play()
{
    d->looprem = d->looptotal;
    server().play(this);
}

/*!
    Returns the number of times the sound will play.
*/
int TQSound::loops() const
{
    return d->looptotal;
}

/*!
    Returns the number of times the sound will loop. This value
    decreases each time the sound loops.
*/
int TQSound::loopsRemaining() const
{
    return d->looprem;
}

/*!
    Sets the sound to repeat \a l times when it is played. Passing the
    value -1 will cause the sound to loop indefinitely.

    \sa loops()
*/
void TQSound::setLoops(int l)
{
    d->looptotal = l;
}

/*!
    Returns the filename associated with the sound.
*/
TQString TQSound::fileName() const
{
    return d->filename;
}

/*!
    Stops the sound playing.

    On Windows the current loop will finish if a sound is played
    in a loop.

    \sa play()
*/
void TQSound::stop()
{
    server().stop(this);
}


/*!
    Returns true if sound facilities exist on the platform; otherwise
    returns false. An application may choose either to notify the user
    if sound is crucial to the application or to operate silently
    without bothering the user.

    If no sound is available, all TQSound operations work silently and
    quickly.
*/
bool TQSound::isAvailable()
{
    return server().okay();
}

/*!
    Sets the internal bucket record of sound \a s to \a b, deleting
    any previous setting.
*/
void TQAuServer::setBucket(TQSound* s, TQAuBucket* b)
{
    delete s->d->bucket;
    s->d->bucket = b;
}

/*!
    Returns the internal bucket record of sound \a s.
*/
TQAuBucket* TQAuServer::bucket(TQSound* s)
{
    return s->d->bucket;
}

/*!
    Decrements the TQSound::loopRemaining() value for sound \a s,
    returning the result.
*/
int TQAuServer::decLoop(TQSound* s)
{
    if ( s->d->looprem > 0 )
	--s->d->looprem;
    return s->d->looprem;
}

/*!
    Initializes the sound. The default implementation does nothing.
*/
void TQAuServer::init(TQSound*)
{
}

TQAuBucket::~TQAuBucket()
{
}

#endif // TQT_NO_SOUND
