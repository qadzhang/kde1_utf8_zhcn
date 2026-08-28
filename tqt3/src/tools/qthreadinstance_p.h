/****************************************************************************
**
** ...
**
** Copyright (C) 2005-2008 Trolltech ASA.  All rights reserved.
**
** This file is part of the tools module of the TQt GUI Toolkit.
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

#ifndef TQTHREAD_P_H
#define TQTHREAD_P_H

//
//  W A R N I N G
//  -------------
//
// This file is not part of the TQt API.  It exists for the convenience
// of TQThread and TQThreadStorage. This header file may change from
// version to version without notice, or even be removed.
//
// We mean it.
//
//

#ifdef TQT_THREAD_SUPPORT

#ifndef QT_H
#include "ntqmutex.h"
#include "ntqwindowdefs.h"
#endif // QT_H

#ifdef Q_OS_UNIX
#include <pthread.h>
#endif

class TQThread;
class TQEventLoop;

class TQThreadInstance {
public:
    static void setCurrentThread(TQThread *thread);
    static TQThreadInstance *current();

    void init(unsigned int stackSize);
    void deinit();

    TQMutex *mutex() const;
    void terminate();

    unsigned int stacksize;
    void *args[2];
    void **thread_storage;
    bool finished : 1;
    bool running  : 1;
    bool orphan   : 1;

#ifdef Q_OS_UNIX
    pthread_cond_t thread_done;
    pthread_t thread_id;

    static void *start( void * );
    static void finish( void * );
#endif // Q_OS_UNIX

#ifdef Q_OS_WIN32
    TQt::HANDLE handle;
    unsigned int thread_id;
    int waiters;

    static unsigned int __stdcall start( void * );
    static void finish( TQThreadInstance * );
#endif // Q_OS_WIN32

    static void finishGuiThread( TQThreadInstance *d );

    TQEventLoop* eventLoop;
    int cleanupType;
    bool disableThreadPostedEvents : 1;
};

#endif // TQT_THREAD_SUPPORT
#endif // TQTHREAD_P_H
