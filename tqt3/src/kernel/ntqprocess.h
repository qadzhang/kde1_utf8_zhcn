/****************************************************************************
**
** Implementation of TQProcess class
**
** Created : 20000905
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

#ifndef TQPROCESS_H
#define TQPROCESS_H

#ifndef QT_H
#include "ntqobject.h"
#include "ntqstringlist.h"
#include "ntqdir.h"
#endif // QT_H

#ifndef TQT_NO_PROCESS

class TQProcessPrivate;
class TQMembuf;


class TQ_EXPORT TQProcess : public TQObject
{
    TQ_OBJECT
public:
    TQProcess( TQObject *parent=0, const char *name=0 );
    TQProcess( const TQString& arg0, TQObject *parent=0, const char *name=0 );
    TQProcess( const TQStringList& args, TQObject *parent=0, const char *name=0 );
    ~TQProcess();

    // set and get the arguments and working directory
    TQStringList arguments() const;
    void clearArguments();
    virtual void setArguments( const TQStringList& args );
    virtual void addArgument( const TQString& arg );
#ifndef TQT_NO_DIR
    TQDir workingDirectory() const;
    virtual void setWorkingDirectory( const TQDir& dir );
#endif

    // set and get the comms wanted
    enum Communication { Stdin=0x01, Stdout=0x02, Stderr=0x04, DupStderr=0x08 };
    void setCommunication( int c );
    int communication() const;

    // start the execution
    virtual bool start( TQStringList *env=0 );
    virtual bool launch( const TQString& buf, TQStringList *env=0  );
    virtual bool launch( const TQByteArray& buf, TQStringList *env=0  );

    // inquire the status
    bool isRunning() const;
    bool normalExit() const;
    int exitStatus() const;

    // reading
    virtual TQByteArray readStdout();
    virtual TQByteArray readStderr();
    bool canReadLineStdout() const;
    bool canReadLineStderr() const;
    virtual TQString readLineStdout();
    virtual TQString readLineStderr();

    // get platform dependent process information
#if defined(Q_OS_WIN32)
    typedef void* PID;
#else
    typedef TQ_LONG PID;
#endif
    PID processIdentifier();

    void flushStdin();

signals:
    void readyReadStdout();
    void readyReadStderr();
    void processExited();
    void wroteToStdin();
    void launchFinished();

public slots:
    // end the execution
    void tryTerminate() const;
    void kill() const;

    // input
    virtual void writeToStdin( const TQByteArray& buf );
    virtual void writeToStdin( const TQString& buf );
    virtual void closeStdin();

protected: // ### or private?
    void connectNotify( const char * signal );
    void disconnectNotify( const char * signal );
private:
    void setIoRedirection( bool value );
    void setNotifyOnExit( bool value );
    void setWroteStdinConnected( bool value );

    void init();
    void reset();
#if defined(Q_OS_WIN32)
    uint readStddev( HANDLE dev, char *buf, uint bytes );
#endif
    TQMembuf* membufStdout();
    TQMembuf* membufStderr();

private slots:
    void socketRead( int fd );
    void socketWrite( int fd );
    void timeout();
    void closeStdinLaunch();

private:
    TQProcessPrivate *d;
#ifndef TQT_NO_DIR
    TQDir        workingDir;
#endif
    TQStringList _arguments;

    int  exitStat; // exit status
    bool exitNormal; // normal exit?
    bool ioRedirection; // automatically set be (dis)connectNotify
    bool notifyOnExit; // automatically set be (dis)connectNotify
    bool wroteToStdinConnected; // automatically set be (dis)connectNotify

    bool readStdoutCalled;
    bool readStderrCalled;
    int comms;

    friend class TQProcessPrivate;
#if defined(Q_OS_UNIX)
    friend class TQProcessManager;
    friend class TQProc;
#endif

#if defined(TQ_DISABLE_COPY) // Disabled copy constructor and operator=
    TQProcess( const TQProcess & );
    TQProcess &operator=( const TQProcess & );
#endif
};

#endif // TQT_NO_PROCESS

#endif // TQPROCESS_H
