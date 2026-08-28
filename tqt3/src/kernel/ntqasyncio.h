/****************************************************************************
**
** Definition of asynchronous I/O classes
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

#ifndef TQASYNCIO_H
#define TQASYNCIO_H

#ifndef QT_H
#include "ntqobject.h"
#include "ntqsignal.h"
#include "ntqtimer.h"
#endif // QT_H

#ifndef TQT_NO_ASYNC_IO

class TQIODevice;

class TQ_EXPORT TQAsyncIO {
public:
    virtual ~TQAsyncIO();
    void connect(TQObject*, const char *member);

protected:
    void ready();

private:
    TQSignal signal;
};

class TQ_EXPORT TQDataSink : public TQAsyncIO {
public:
    // Call this to know how much I can take.
    virtual int readyToReceive()=0;
    virtual void receive(const uchar*, int count)=0;
    virtual void eof()=0;
    void maybeReady();
};

class TQ_EXPORT TQDataSource : public TQAsyncIO {
public:
    virtual int readyToSend()=0; // returns -1 when never any more ready
    virtual void sendTo(TQDataSink*, int count)=0;
    void maybeReady();

    virtual bool rewindable() const;
    virtual void enableRewind(bool);
    virtual void rewind();
};

class TQ_EXPORT TQIODeviceSource : public TQDataSource {
    const int buf_size;
    uchar *buffer;
    TQIODevice* iod;
    bool rew;

public:
    TQIODeviceSource(TQIODevice*, int bufsize=4096);
   ~TQIODeviceSource();

    int readyToSend();
    void sendTo(TQDataSink* sink, int n);
    bool rewindable() const;
    void enableRewind(bool on);
    void rewind();
};

class TQ_EXPORT TQDataPump : public TQObject {
    TQ_OBJECT
    int interval;
    TQTimer timer;
    TQDataSource* source;
    TQDataSink* sink;

public:
    TQDataPump(TQDataSource*, TQDataSink*);

private slots:
    void kickStart();
    void tryToPump();
};

#endif	// TQT_NO_ASYNC_IO

#endif
