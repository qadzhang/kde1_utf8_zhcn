/****************************************************************************
**
** Definition of TQSound class and TQAuServer internal class
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
#ifndef TQSOUND_H
#define TQSOUND_H

#ifndef QT_H
#include "ntqobject.h"
#endif // QT_H

#ifndef TQT_NO_SOUND

class TQSoundData;

class TQ_EXPORT TQSound : public TQObject {
    TQ_OBJECT
public:
    static bool isAvailable();
    static void play(const TQString& filename);

    TQSound(const TQString& filename, TQObject* parent=0, const char* name=0);
    ~TQSound();

    /* Coming soon...
	?
    TQSound(int hertz, Type type=Mono);
    int play(const ushort* data, int samples);
    bool full();
    signal void notFull();
	?
    */

#ifndef TQT_NO_COMPAT
    static bool available() { return isAvailable(); }
#endif

    int loops() const;
    int loopsRemaining() const;
    void setLoops(int);
    TQString fileName() const;

    bool isFinished() const;

public slots:
    void play();
    void stop();

private:
    TQSoundData* d;
    friend class TQAuServer;
};


/*
  TQAuServer is an INTERNAL class.  If you wish to provide support for
  additional audio servers, you can make a subclass of TQAuServer to do
  so, HOWEVER, your class may need to be re-engineered to some degree
  with each new TQt release, including minor releases.

  TQAuBucket is whatever you want.
*/

class TQ_EXPORT TQAuBucket {
public:
    virtual ~TQAuBucket();
};

class TQ_EXPORT TQAuServer : public TQObject {
    TQ_OBJECT

public:
    TQAuServer(TQObject* parent, const char* name);
    ~TQAuServer();

    virtual void init(TQSound*);
    virtual void play(const TQString& filename);
    virtual void play(TQSound*)=0;
    virtual void stop(TQSound*)=0;
    virtual bool okay()=0;

protected:
    void setBucket(TQSound*, TQAuBucket*);
    TQAuBucket* bucket(TQSound*);
    int decLoop(TQSound*);
};

#endif // TQT_NO_SOUND

#endif
