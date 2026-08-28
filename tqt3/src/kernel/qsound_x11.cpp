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

#include "ntqptrdict.h"
#include "ntqsocketnotifier.h"
#include "ntqapplication.h"


#ifdef QT_NAS_SUPPORT

#include <audio/audiolib.h>
#include <audio/soundlib.h>

static AuServer *nas=0;

static AuBool eventPred(AuServer *, AuEvent *e, AuPointer p)
{
    if (e && (e->type == AuEventTypeElementNotify)) {
	if (e->auelementnotify.flow == *((AuFlowID *)p))
	    return true;
    }
    return false;
}

class TQAuBucketNAS : public TQAuBucket {
public:
    TQAuBucketNAS(AuBucketID b, AuFlowID f = 0) : id(b), flow(f), stopped(true), numplaying(0) { }
    ~TQAuBucketNAS()
    {
	if ( nas ) {
	    AuSync(nas, false);
	    AuDestroyBucket(nas, id, NULL);

	    AuEvent ev;
	    while (AuScanEvents(nas, AuEventsQueuedAfterFlush, true, eventPred, &flow, &ev))
		;
        }
    }

    AuBucketID id;
    AuFlowID flow;
    bool     stopped;
    int      numplaying;
};

class TQAuServerNAS : public TQAuServer {
    TQ_OBJECT

    TQSocketNotifier* sn;

public:
    TQAuServerNAS(TQObject* parent);
    ~TQAuServerNAS();

    void init(TQSound*);
    void play(const TQString& filename);
    void play(TQSound*);
    void stop(TQSound*);
    bool okay();
    void setDone(TQSound*);

public slots:
    void dataReceived();
    void soundDestroyed(TQObject *o);

private:
    TQAuBucketNAS* bucket(TQSound* s)
    {
	return (TQAuBucketNAS*)TQAuServer::bucket(s);
    }
};

TQAuServerNAS::TQAuServerNAS(TQObject* parent) :
    TQAuServer(parent,"Network Audio System")
{
    nas = AuOpenServer(NULL, 0, NULL, 0, NULL, NULL);
    if (nas) {
	AuSetCloseDownMode(nas, AuCloseDownDestroy, NULL);
	// Ask TQt for async messages...
	sn=new TQSocketNotifier(AuServerConnectionNumber(nas),
		TQSocketNotifier::Read);
	TQObject::connect(sn, TQ_SIGNAL(activated(int)),
		this, TQ_SLOT(dataReceived()));
    } else {
	sn = 0;
    }
}

TQAuServerNAS::~TQAuServerNAS()
{
    if ( nas )
	AuCloseServer( nas );
    delete sn;
    nas = 0;
}

static TQPtrDict<void> *inprogress=0;

void TQAuServerNAS::soundDestroyed(TQObject *o)
{
    if (inprogress) {
	TQSound *so = static_cast<TQSound *>(o);
	while (inprogress->remove(so))
	    ; // Loop while remove returns true
    }
}

void TQAuServerNAS::play(const TQString& filename)
{
    if (nas) {
	int iv=100;
	AuFixedPoint volume=AuFixedPointFromFraction(iv,100);
	AuSoundPlayFromFile(nas, filename, AuNone, volume, NULL, NULL, NULL, NULL, NULL, NULL);
	AuFlush(nas);
	dataReceived();
	AuFlush(nas);
	tqApp->flushX();
    }
}

static void callback( AuServer*, AuEventHandlerRec*, AuEvent* e, AuPointer p)
{
    if ( inprogress->find(p) && e ) {
	if (e->type==AuEventTypeElementNotify &&
		    e->auelementnotify.kind==AuElementNotifyKindState) {
	    if ( e->auelementnotify.cur_state == AuStateStop )
                ((TQAuServerNAS*)inprogress->find(p))->setDone((TQSound*)p);
	}
    }
}

void TQAuServerNAS::setDone(TQSound* s)
{
    if (nas) {
        decLoop(s);
        if (s->loopsRemaining() && !bucket(s)->stopped) {
	    bucket(s)->stopped = true;
            play(s);
        } else {
	    if (--(bucket(s)->numplaying) == 0)
		bucket(s)->stopped = true;
            inprogress->remove(s);
        }
    }
}

void TQAuServerNAS::play(TQSound* s)
{
    if (nas) {
	++(bucket(s)->numplaying);
	if (!bucket(s)->stopped) {
	    stop(s);
	}

	bucket(s)->stopped = false;
	if ( !inprogress )
	    inprogress = new TQPtrDict<void>;
	inprogress->insert(s,(void*)this);
	int iv=100;
	AuFixedPoint volume=AuFixedPointFromFraction(iv,100);
        TQAuBucketNAS *b = bucket(s);
        AuSoundPlayFromBucket(nas, b->id, AuNone, volume,
                              callback, s, 0, &b->flow, NULL, NULL, NULL);
	AuFlush(nas);
	dataReceived();
	AuFlush(nas);
	tqApp->flushX();
    }
}

void TQAuServerNAS::stop(TQSound* s)
{
    if (nas && !bucket(s)->stopped) {
	bucket(s)->stopped = true;
        AuStopFlow(nas, bucket(s)->flow, NULL);
        AuFlush(nas);
	dataReceived();
	AuFlush(nas);
	tqApp->flushX();
    }
}

void TQAuServerNAS::init(TQSound* s)
{
    connect(s, TQ_SIGNAL(destroyed(TQObject *)),
	    this, TQ_SLOT(soundDestroyed(TQObject *)));

    if ( nas ) {
        AuBucketID b_id =
            AuSoundCreateBucketFromFile(nas, s->fileName(),
                                        0 /*AuAccessAllMasks*/, NULL, NULL);
	setBucket(s, new TQAuBucketNAS(b_id));
    }
}

bool TQAuServerNAS::okay()
{
    return !!nas;
}

void TQAuServerNAS::dataReceived()
{
    AuHandleEvents(nas);
}

#include "qsound_x11.moc"

#endif


class TQAuServerNull : public TQAuServer {
public:
    TQAuServerNull(TQObject* parent);

    void play(const TQString&) { }
    void play(TQSound*s) { while(decLoop(s) > 0) /* nothing */ ; }
    void stop(TQSound*) { }
    bool okay() { return false; }
};

TQAuServerNull::TQAuServerNull(TQObject* parent) :
    TQAuServer(parent,"Null Audio Server")
{
}


TQAuServer* tqt_new_audio_server()
{
#ifdef QT_NAS_SUPPORT
    TQAuServer* s=new TQAuServerNAS(tqApp);
    if (s->okay()) {
	return s;
    } else {
	delete s;
    }
#endif
    return new TQAuServerNull(tqApp);
}

#endif // TQT_NO_SOUND
