/****************************************************************************
**
** Definition of TQThread class
**
** Created : 931107
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

#ifndef TQTHREAD_H
#define TQTHREAD_H

#if defined(TQT_THREAD_SUPPORT)

#ifndef QT_H
#include "ntqwindowdefs.h"
#ifndef TQT_NO_COMPAT
#include "ntqmutex.h"
#include "ntqsemaphore.h"
#include "ntqwaitcondition.h"
#endif // TQT_NO_COMPAT
#endif // QT_H

#include <limits.h>

class TQThreadInstance;

class TQ_EXPORT TQThread : public TQt
{
public:
    static TQt::HANDLE currentThread();

#ifndef TQT_NO_COMPAT
    static void postEvent( TQObject *,TQEvent * );
#endif

    static void initialize();
    static void cleanup();

    static void exit();

#ifdef Q_QDOC
    TQThread( unsigned int stackSize = 0 );
#else
    TQThread( unsigned int stackSize );
    TQThread();
#endif

    virtual ~TQThread();

    // default argument causes thread to block indefinately
    bool wait( unsigned long time = ULONG_MAX );

    enum Priority {
	IdlePriority,

	LowestPriority,
	LowPriority,
	NormalPriority,
	HighPriority,
	HighestPriority,

	TimeCriticalPriority,

	InheritPriority
    };

#ifdef Q_QDOC
    void start( Priority = InheritPriority );
#else
    void start( Priority );
    void start();
#endif

    void terminate();

    bool finished() const;
    bool running() const;

    enum CleanupType {
	CleanupNone,
	CleanupMergeObjects
    };

    CleanupType cleanupType() const;
    void setCleanupType(CleanupType);

    bool threadPostedEventsDisabled() const;
    void setThreadPostedEventsDisabled(bool);

protected:
    virtual void run() = 0;

    static void sleep( unsigned long );
    static void msleep( unsigned long );
    static void usleep( unsigned long );

private:
    TQThreadInstance * d;
    friend class TQThreadInstance;
    friend class TQThreadStorageData;
    friend class TQCoreApplicationThread;
    friend class TQApplication;
    friend class TQEventLoop;

#if defined(TQ_DISABLE_COPY)
    TQThread( const TQThread & );
    TQThread &operator=( const TQThread & );
#endif // TQ_DISABLE_COPY

public:
    static TQThread* currentThreadObject();
};

class TQ_EXPORT TQEventLoopThread : public TQThread
{
    public:
        TQEventLoopThread();
        ~TQEventLoopThread();
        virtual void run();
};

#endif // TQT_THREAD_SUPPORT

#endif // TQTHREAD_H
