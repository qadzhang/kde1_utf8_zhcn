/****************************************************************************
**
** Definition of TQLock class. This manages interprocess locking
**
** Created : 20000406
**
** Copyright (C) 2000-2008 Trolltech ASA.  All rights reserved.
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

#include "qlock_p.h"

#ifndef TQT_NO_QWS_MULTIPROCESS

#include <unistd.h>
#include <sys/types.h>
#if defined(Q_OS_MACX)
#define Q_NO_SEMAPHORE
#include <sys/stat.h>
#include <sys/file.h>
#else
#define _WANT_SEMUN
#include <sys/sem.h>
#if defined(__GNU_LIBRARY__) && !defined(_SEM_SEMUN_UNDEFINED) \
    || defined(Q_OS_FREEBSD) || defined(Q_OS_OPENBSD) || defined(Q_OS_NETBSD) || defined(Q_OS_BSDI)
/* union semun is defined by including <sys/sem.h> */
#else
/* according to X/OPEN we have to define it ourselves */
union semun {
    int val;                    /* value for SETVAL */
    struct semid_ds *buf;       /* buffer for IPC_STAT, IPC_SET */
    unsigned short *array;      /* array for GETALL, SETALL */
};
#endif
#endif
#include <sys/ipc.h>
#include <string.h>
#include <errno.h>

#define MAX_LOCKS   200	    // maximum simultaneous read locks

class TQLockData
{
public:
#ifdef Q_NO_SEMAPHORE
    TQCString file;
#endif
    int id;
    int count;
    bool owned;
};

#endif

/*!
    \class TQLock qlock_p.h
    \brief The TQLock class is a wrapper for a System V shared semaphore.

    \ingroup qws
    \ingroup io

    \internal

    It is used by TQt/Embedded for synchronizing access to the graphics
    card and shared memory region between processes.
*/

/*!
    \enum TQLock::Type

    \value Read
    \value Write
*/

/*!
    \fn TQLock::TQLock( const TQString &filename, char id, bool create )

    Creates a lock. \a filename is the file path of the Unix-domain
    socket the TQt/Embedded client is using. \a id is the name of the
    particular lock to be created on that socket. If \a create is true
    the lock is to be created (as the TQt/Embedded server does); if \a
    create is false the lock should exist already (as the TQt/Embedded
    client expects).
*/

TQLock::TQLock( const TQString &filename, char id, bool create )
{
#ifndef TQT_NO_QWS_MULTIPROCESS
    data = new TQLockData;
    data->count = 0;
#ifdef Q_NO_SEMAPHORE
    data->file = TQString(filename+id).local8Bit();
    for(int x = 0; x < 2; x++) {
	data->id = open(data->file, O_RDWR | (x ? O_CREAT : 0), S_IRWXU);
	if(data->id != -1 || !create) {
	    data->owned = x;
	    break;
	}
    }
#else
    key_t semkey = ftok(filename, id);
    data->id = semget(semkey,0,0);
    data->owned = create;
    if ( create ) {
	semun arg; arg.val = 0;
	if ( data->id != -1 )
	    semctl(data->id,0,IPC_RMID,arg);
	data->id = semget(semkey,1,IPC_CREAT|0600);
	arg.val = MAX_LOCKS;
	semctl(data->id,0,SETVAL,arg);
    }
#endif
    if ( data->id == -1 ) {
	tqWarning( "Cannot %s semaphore %s \'%c\'",
	    create ? "create" : "get", filename.latin1(), id );
	tqDebug("Error %d %s\n",errno,strerror(errno));
    }
#endif
}

/*!
    \fn TQLock::~TQLock()

    Destroys a lock
*/

TQLock::~TQLock()
{
#ifndef TQT_NO_QWS_MULTIPROCESS
    if ( locked() )
	unlock();
#ifdef Q_NO_SEMAPHORE
    if(isValid()) {
	close(data->id);
	if( data->owned )
	    unlink( data->file );
    }
#else
    if(data->owned) {
	semun arg; arg.val = 0;
	semctl( data->id, 0, IPC_RMID, arg );
    }
#endif
    delete data;
#endif
}

/*!
    \fn bool TQLock::isValid() const

    Returns true if the lock constructor was succesful; returns false if
    the lock could not be created or was not available to connect to.
*/

bool TQLock::isValid() const
{
#ifndef TQT_NO_QWS_MULTIPROCESS
    return (data->id != -1);
#else
    return true;
#endif
}

/*!
    Locks the semaphore with a lock of type \a t. Locks can either be
    \c Read or \c Write. If a lock is \c Read, attempts by other
    processes to obtain \c Read locks will succeed, and \c Write
    attempts will block until the lock is unlocked. If locked as \c
    Write, all attempts to lock by other processes will block until
    the lock is unlocked. Locks are stacked: i.e. a given TQLock can be
    locked multiple times by the same process without blocking, and
    will only be unlocked after a corresponding number of unlock()
    calls.
*/

void TQLock::lock( Type t )
{
#ifndef TQT_NO_QWS_MULTIPROCESS
    if ( !data->count ) {
#ifdef Q_NO_SEMAPHORE
	int op = LOCK_SH;
	if(t == Write)
	    op = LOCK_EX;
	for( int rv=1; rv; ) {
	    rv = flock(data->id, op);
	    if (rv == -1 && errno != EINTR)
		tqDebug("Semop lock failure %s",strerror(errno));
	}
#else
	sembuf sops;
	sops.sem_num = 0;
	sops.sem_flg = SEM_UNDO;

	if ( t == Write ) {
	    sops.sem_op = -MAX_LOCKS;
	    type = Write;
	} else {
	    sops.sem_op = -1;
	    type = Read;
	}

	int rv;
	do {
	    rv = semop(data->id,&sops,1);
	    if (rv == -1 && errno != EINTR)
		tqDebug("Semop lock failure %s",strerror(errno));
	} while ( rv == -1 && errno == EINTR );
#endif
    }
    data->count++;
#endif
}

/*!
    \fn void TQLock::unlock()

    Unlocks the semaphore. If other processes were blocking waiting to
    lock() the semaphore, one of them will wake up and succeed in
    lock()ing.
*/

void TQLock::unlock()
{
#ifndef TQT_NO_QWS_MULTIPROCESS
    if( data->count ) {
	data->count--;
	if( !data->count ) {
#ifdef Q_NO_SEMAPHORE
	    for( int rv=1; rv; ) {
		rv = flock(data->id, LOCK_UN);
		if (rv == -1 && errno != EINTR)
		    tqDebug("Semop lock failure %s",strerror(errno));
	    }
#else
	    sembuf sops;
	    sops.sem_num = 0;
	    sops.sem_op = 1;
	    sops.sem_flg = SEM_UNDO;
	    if ( type == Write )
		sops.sem_op = MAX_LOCKS;

	    int rv;
	    do {
		rv = semop(data->id,&sops,1);
		if (rv == -1 && errno != EINTR)
		    tqDebug("Semop unlock failure %s",strerror(errno));
	    } while ( rv == -1 && errno == EINTR );
#endif
	}
    } else {
	tqDebug("Unlock without corresponding lock");
    }
#endif
}

/*!
    \fn bool TQLock::locked() const

    Returns true if the lock is currently held by the current process;
    otherwise returns false.
*/

bool TQLock::locked() const
{
#ifndef TQT_NO_QWS_MULTIPROCESS
    return (data->count > 0);
#else
    return false;
#endif
}
