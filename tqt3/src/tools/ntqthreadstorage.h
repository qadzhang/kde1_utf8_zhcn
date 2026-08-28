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

#ifndef TQTHREADSTORAGE_H
#define TQTHREADSTORAGE_H

#ifdef TQT_THREAD_SUPPORT

#ifndef QT_H
#include "ntqglobal.h"
#endif // QT_H

class TQ_EXPORT TQThreadStorageData
{
public:
    TQThreadStorageData( void (*func)(void *) );
    ~TQThreadStorageData();

    void** get() const;
    void** set( void* p );

    static void finish( void** );
    int id;
};


template <class T>
class TQThreadStorage
{
private:
    TQThreadStorageData d;

#if defined(TQ_DISABLE_COPY)
    // disable copy constructor and operator=
    TQThreadStorage( const TQThreadStorage & );
    TQThreadStorage &operator=( const TQThreadStorage & );
#endif // TQ_DISABLE_COPY

    static void deleteData( void *x ) { delete (T)x; }

public:
    inline TQThreadStorage() : d( deleteData ) {  }
    inline ~TQThreadStorage() { }

    inline bool hasLocalData() const
    { return d.get() != 0; }

    inline T& localData()
    { void **v = d.get(); if ( !v ) v = d.set( 0 ); return *(T*)v; }

    inline T localData() const
    { void **v = d.get(); return ( v ? *(T*)v : 0 ); }

    inline void setLocalData( T t )
    { (void) d.set( t ); }
};

#endif // TQT_THREAD_SUPPORT

#endif // TQTHREADSTORAGE_H
