/****************************************************************************
**
** Definition of TQSessionManager class
**
** Created : 990510
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

#ifndef TQSESSIONMANAGER_H
#define TQSESSIONMANAGER_H

#ifndef QT_H
#include "ntqobject.h"
#include "ntqwindowdefs.h"
#include "ntqstring.h"
#include "ntqstringlist.h"
#endif // QT_H
#ifndef TQT_NO_SESSIONMANAGER

class TQSessionManagerData;

class TQ_EXPORT  TQSessionManager : public TQObject
{
    TQ_OBJECT
    TQSessionManager( TQApplication *app, TQString &id, TQString &key );
    ~TQSessionManager();
public:
    TQString sessionId() const;
    TQString sessionKey() const;
#if defined(TQ_WS_X11) || defined(TQ_WS_MAC)
    void* handle() const;
#endif

    bool allowsInteraction();
    bool allowsErrorInteraction();
    void release();

    void cancel();

    enum RestartHint {
	RestartIfRunning,
	RestartAnyway,
	RestartImmediately,
	RestartNever
    };
    void setRestartHint( RestartHint );
    RestartHint restartHint() const;

    void setRestartCommand( const TQStringList& );
    TQStringList restartCommand() const;
    void setDiscardCommand( const TQStringList& );
    TQStringList discardCommand() const;

    void setManagerProperty( const TQString& name, const TQString& value );
    void setManagerProperty( const TQString& name, const TQStringList& value );

    bool isPhase2() const;
    void requestPhase2();

private:
    friend class TQApplication;
    friend class TQBaseApplication;
    TQSessionManagerData* d;
};

#endif // TQT_NO_SESSIONMANAGER
#endif // TQSESSIONMANAGER_H
