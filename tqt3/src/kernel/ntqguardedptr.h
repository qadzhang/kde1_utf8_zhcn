/****************************************************************************
**
** Definition of TQGuardedPtr class
**
** Created : 990929
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

#ifndef TQGUARDEDPTR_H
#define TQGUARDEDPTR_H

#ifndef QT_H
#include "ntqobject.h"
#endif // QT_H

// ### 4.0: rename to something without Private in it. Not really internal.
class TQ_EXPORT TQGuardedPtrPrivate : public TQObject, public TQShared
{
    TQ_OBJECT
public:
    TQGuardedPtrPrivate( TQObject* );
    ~TQGuardedPtrPrivate();

    TQObject* object() const;
    void reconnect( TQObject* );

private slots:
    void objectDestroyed();

private:
    TQObject* obj;
#if defined(TQ_DISABLE_COPY) // Disabled copy constructor and operator=
    TQGuardedPtrPrivate( const TQGuardedPtrPrivate & );
    TQGuardedPtrPrivate &operator=( const TQGuardedPtrPrivate & );
#endif
};

template <class T>
class TQGuardedPtr
{
public:
    TQGuardedPtr() : priv( new TQGuardedPtrPrivate( 0 ) ) {}

    TQGuardedPtr( T* o) {
	priv = new TQGuardedPtrPrivate( (TQObject*)o );
    }

    TQGuardedPtr(const TQGuardedPtr<T> &p) {
	priv = p.priv;
	ref();
    }

    ~TQGuardedPtr() { deref(); }

    TQGuardedPtr<T> &operator=(const TQGuardedPtr<T> &p) {
	if ( priv != p.priv ) {
	    deref();
	    priv = p.priv;
	    ref();
	}
	return *this;
    }

    TQGuardedPtr<T> &operator=(T* o) {
	if ( priv && priv->count == 1 ) {
	    priv->reconnect( (TQObject*)o );
	} else {
	    deref();
	    priv = new TQGuardedPtrPrivate( (TQObject*)o );
	}
	return *this;
    }

    bool operator==( const TQGuardedPtr<T> &p ) const {
	return (T*)(*this) == (T*) p;
    }

    bool operator!= ( const TQGuardedPtr<T>& p ) const {
	return !( *this == p );
    }

    bool isNull() const { return !priv || !priv->object(); }

    T* operator->() const { return (T*)(priv?priv->object():0); }

    T& operator*() const { return *((T*)(priv?priv->object():0)); }

    operator T*() const { return (T*)(priv?priv->object():0); }

private:

    void ref() { if (priv) priv->ref(); }

    void deref() {
	if ( priv && priv->deref() )
	    delete priv;
    }

    TQGuardedPtrPrivate* priv;
};




inline TQObject* TQGuardedPtrPrivate::object() const
{
    return obj;
}

#define Q_DEFINED_QGUARDEDPTR
#include "ntqwinexport.h"
#endif
