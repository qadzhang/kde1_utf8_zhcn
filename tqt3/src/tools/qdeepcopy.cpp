/****************************************************************************
**
** Implementation of TQDeepCopy class
**
** Created : 20020613
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

#include "ntqdeepcopy.h"

/*!
    \class TQDeepCopy ntqdeepcopy.h
    \brief The TQDeepCopy class is a template class which ensures that
    implicitly shared and explicitly shared classes reference unique
    data.

    \reentrant

    \ingroup tools
    \ingroup shared

    Normally, shared copies reference the same data to optimize memory
    use and for maximum speed. In the example below, \c s1, \c s2, \c
    s3, \c s4 and \c s5 share data.

    \code
    // all 5 strings share the same data
    TQString s1 = "abcd";
    TQString s2 = s1;
    TQString s3 = s2;
    TQString s4 = s3;
    TQString s5 = s2;
    \endcode

    TQDeepCopy can be used several ways to ensure that an object
    references unique, unshared data. In the example below, \c s1, \c
    s2 and \c s5 share data, while neither \c s3 nor \c s4 share data.
    \code
    // s1, s2 and s5 share the same data, neither s3 nor s4 are shared
    TQString s1 = "abcd";
    TQString s2 = s1;
    TQDeepCopy<TQString> s3 = s2;  // s3 is a deep copy of s2
    TQString s4 = s3;             // s4 is a deep copy of s3
    TQString s5 = s2;
    \endcode

    In the example below, \c s1, \c s2 and \c s5 share data, and \c s3
    and \c s4 share data.
    \code
    // s1, s2 and s5 share the same data, s3 and s4 share the same data
    TQString s1 = "abcd";
    TQString s2 = s1;
    TQString s3 = TQDeepCopy<TQString>( s2 );  // s3 is a deep copy of s2
    TQString s4 = s3;                        // s4 is a shallow copy of s3
    TQString s5 = s2;
    \endcode

    TQDeepCopy can also provide safety in multithreaded applications
    that use shared classes. In the example below, the variable \c
    global_string is used safely since the data contained in \c
    global_string is always a deep copy. This ensures that all threads
    get a unique copy of the data, and that any assignments to \c
    global_string will result in a deep copy.

    \code
    TQDeepCopy<TQString> global_string;  // global string data
    TQMutex global_mutex;               // mutex to protext global_string

    ...

    void setGlobalString( const TQString &str )
    {
	global_mutex.lock();
	global_string = str;           // global_string is a deep copy of str
	global_mutex.unlock();
    }

    ...

    void MyThread::run()
    {
	global_mutex.lock();
	TQString str = global_string;          // str is a deep copy of global_string
	global_mutex.unlock();

	// process the string data
	...

	// update global_string
	setGlobalString( str );
    }
    \endcode

    \warning It is the application developer's responsibility to
    protect the object shared across multiple threads.

    The examples above use TQString, which is an implicitly shared
    class. The behavior of TQDeepCopy is the same when using explicitly
    shared classes like TQByteArray.

    Currently, TQDeepCopy works with the following classes:
    \list
    \i TQMemArray (including subclasses like TQByteArray and TQCString)
    \i TQMap
    \i TQString
    \i TQValueList (including subclasses like TQStringList and TQValueStack)
    \i TQValueVector
    \endlist

    \sa \link threads.html Thread Support in TQt \endlink
*/

/*!
    \fn TQDeepCopy::TQDeepCopy()

    Constructs an empty instance of type \e T.
*/

/*!
    \fn TQDeepCopy::TQDeepCopy( const T &t )

    Constructs a deep copy of \a t.
*/

/*!
    \fn TQDeepCopy<T>& TQDeepCopy::operator=( const T &t )

    Assigns a deep copy of \a t.
*/

/*!
    \fn TQDeepCopy::operator T ()

    Returns a deep copy of the encapsulated data.
*/

