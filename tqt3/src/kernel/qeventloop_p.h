/****************************************************************************
**
** Definition of TQEventLoop class
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
** Licensees holding valid TQt Commercial licenses may use this file in
** accordance with the TQt Commercial License Agreement provided with
** the Software.
**
** This file is provided "AS IS" with NO WARRANTY OF ANY KIND,
** INCLUDING THE WARRANTIES OF DESIGN, MERCHANTABILITY AND FITNESS FOR
** A PARTICULAR PURPOSE. Trolltech reserves all rights not granted
** herein.
**
**********************************************************************/

#ifndef TQEVENTLOOP_P_H
#define TQEVENTLOOP_P_H

//
//  W A R N I N G
//  -------------
//
// This file is not part of the TQt API.  This header file may
// change from version to version without notice, or even be
// removed.
//
// We mean it.
//
//

#if defined(QT_USE_GLIBMAINLOOP)
#include "qeventloop_glib_p.h"  // use a different header file!!!
#else

#ifndef QT_H
#include "qplatformdefs.h"
#endif // QT_H

#include "ntqwindowdefs.h"

class TQSocketNotifier;
#ifdef Q_OS_MAC
class TQMacSockNotPrivate;
#endif

#if defined(Q_OS_UNIX) || defined (TQ_WS_WIN)
#include "ntqptrlist.h"
#endif // Q_OS_UNIX || TQ_WS_WIN

#if defined(Q_OS_UNIX)
struct TQSockNot
{
    TQSocketNotifier *obj;
    int fd;
    fd_set *queue;
};

class TQSockNotType
{
public:
    TQSockNotType();
    ~TQSockNotType();

    TQPtrList<TQSockNot> *list;
    fd_set select_fds;
    fd_set enabled_fds;
    fd_set pending_fds;

};
#endif // Q_OS_UNIX

#if defined(TQ_WS_WIN)
struct TQSockNot {
    TQSocketNotifier *obj;
    int fd;
};
#endif // TQ_WS_WIN

class TQEventLoopPrivate
{
public:
    TQEventLoopPrivate()
    {
	reset();
    }

    void reset() {
	looplevel = 0;
	quitcode = 0;
	quitnow = false;
	exitloop = false;
	shortcut = false;
    }

    int looplevel;
    int quitcode;
    unsigned int quitnow  : 1;
    unsigned int exitloop : 1;
    unsigned int shortcut : 1;

#if defined(TQ_WS_MAC)
    uchar        next_select_timer;
    EventLoopTimerRef select_timer;
#endif

#if defined(TQ_WS_X11)
    int xfd;
#endif // TQ_WS_X11

#if defined(Q_OS_UNIX)
    int thread_pipe[2];

    // pending socket notifiers list
    TQPtrList<TQSockNot> sn_pending_list;
    // highest fd for all socket notifiers
    int sn_highest;
    // 3 socket notifier types - read, write and exception
    TQSockNotType sn_vec[3];
#endif

#ifdef TQ_WS_WIN
    // pending socket notifiers list
    TQPtrList<TQSockNot> sn_pending_list;
#endif // TQ_WS_WIN

};

#endif // !QT_USE_GLIBMAINLOOP

#endif // TQEVENTLOOP_P_H
