/****************************************************************************
**
** Declaration of TQEventLoop class
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

#ifndef TQEVENTLOOP_H
#define TQEVENTLOOP_H

#ifndef QT_H
#include "ntqobject.h"
#include "ntqsocketnotifier.h"
#endif // QT_H

#ifdef Q_QDOC
#else // Q_QDOC
#if defined(QT_USE_GLIBMAINLOOP)
#include <glib.h>
#endif // QT_USE_GLIBMAINLOOP
#endif // Q_QDOC

class TQEventLoopPrivate;
class TQSocketNotifier;
class TQTimer;
#ifdef TQ_WS_MAC
struct timeval; //stdc struct
struct TimerInfo; //internal structure (qeventloop_mac.cpp)
#endif

#if defined(TQT_THREAD_SUPPORT)
class TQMutex;
#endif // TQT_THREAD_SUPPORT


class TQ_EXPORT TQEventLoop : public TQObject
{
    TQ_OBJECT

public:
    TQEventLoop( TQObject *parent = 0, const char *name = 0 );
    ~TQEventLoop();

    enum ProcessEvents {
	AllEvents              = 0x00,
	ExcludeUserInput       = 0x01,
	ExcludeSocketNotifiers = 0x02,
	WaitForMore            = 0x04,
	ExcludeTimers          = 0x08
    };
    typedef uint ProcessEventsFlags;

    void processEvents( ProcessEventsFlags flags, int maxtime );
    virtual bool processEvents( ProcessEventsFlags flags );

    virtual bool hasPendingEvents() const;

    virtual void registerSocketNotifier( TQSocketNotifier * );
    virtual void unregisterSocketNotifier( TQSocketNotifier * );
    void setSocketNotifierPending( TQSocketNotifier * );
    int activateSocketNotifiers();

    int activateTimers();
    int timeToWait() const;

    virtual int exec();
    virtual void exit( int retcode = 0 );

    virtual int enterLoop();
    virtual void exitLoop();
    virtual int loopLevel() const;

    virtual void wakeUp();

    void setSingleToolkitEventHandling(bool enabled);

signals:
    void awake();
    void aboutToBlock();

private:
#if defined(TQ_WS_MAC)
    friend TQMAC_PASCAL void tqt_mac_select_timer_callbk(EventLoopTimerRef, void *);
    int macHandleSelect(timeval *);
    void macHandleTimer(TimerInfo *);
#endif // TQ_WS_MAC

    // internal initialization/cleanup - implemented in various platform specific files
    void init();
    void cleanup();
    virtual void appStartingUp();
    virtual void appClosingDown();

    // data for the default implementation - other implementations should not
    // use/need this data
    TQEventLoopPrivate *d;

    friend class TQApplication;

#ifdef Q_QDOC
#else // Q_QDOC
#if defined(QT_USE_GLIBMAINLOOP)

    // glib main loop support
    /* internal: used to fit glib-main-loop gsource concept */
public:
    bool gsourcePrepare(GSource *gs, int * timeout);
    bool gsourceCheck(GSource * gs);
    bool gsourceDispatch(GSource * gs);

    bool processX11Events();

    // end glib main loop support
	
#endif //QT_USE_GLIBMAINLOOP
#endif // Q_QDOC
};

#endif // TQEVENTLOOP_H
