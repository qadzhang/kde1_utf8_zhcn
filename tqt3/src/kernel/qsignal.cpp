/****************************************************************************
**
** Implementation of TQSignal class
**
** Created : 941201
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

#include "ntqsignal.h"
#include "ntqmetaobject.h"
#include "ntqguardedptr.h"

/*!
    \class TQSignal ntqsignal.h
    \brief The TQSignal class can be used to send signals for classes
    that don't inherit TQObject.

    \ingroup io
    \ingroup misc

    If you want to send signals from a class that does not inherit
    TQObject, you can create an internal TQSignal object to emit the
    signal. You must also provide a function that connects the signal
    to an outside object slot.  This is how we have implemented
    signals in the TQMenuData class, which is not a TQObject.

    In general, we recommend inheriting TQObject instead. TQObject
    provides much more functionality.

    You can set a single TQVariant parameter for the signal with
    setValue().

    Note that TQObject is a \e private base class of TQSignal, i.e. you
    cannot call any TQObject member functions from a TQSignal object.

    Example:
    \code
	#include <ntqsignal.h>

	class MyClass
	{
	public:
	    MyClass();
	    ~MyClass();

	    void doSomething();

	    void connect( TQObject *receiver, const char *member );

	private:
	    TQSignal *sig;
	};

	MyClass::MyClass()
	{
	    sig = new TQSignal;
	}

	MyClass::~MyClass()
	{
	    delete sig;
	}

	void MyClass::doSomething()
	{
	    // ... does something
	    sig->activate(); // emits the signal
	}

	void MyClass::connect( TQObject *receiver, const char *member )
	{
	    sig->connect( receiver, member );
	}
    \endcode
*/

/*!
    Constructs a signal object called \a name, with the parent object
    \a parent. These arguments are passed directly to TQObject.
*/

TQSignal::TQSignal( TQObject *parent, const char *name )
    : TQObject( parent, name )
{
    isSignal = true;
#ifndef TQT_NO_VARIANT
    val = 0;
#endif
}

/*!
    Destroys the signal. All connections are removed, as is the case
    with all TQObjects.
*/
TQSignal::~TQSignal()
{
}
#ifndef TQT_NO_VARIANT
// Returns true if it matches ".+(.*int.*"
static inline bool intSignature( const char *member )
{
    TQCString s( member );
    int p = s.find( '(' );
    return p > 0 && p < s.findRev( "int" );
}
#endif
/*!
    Connects the signal to \a member in object \a receiver.

    \sa disconnect(), TQObject::connect()
*/

bool TQSignal::connect( const TQObject *receiver, const char *member )
{
#ifndef TQT_NO_VARIANT
    if ( intSignature( member ) )
#endif
	return TQObject::connect( (TQObject *)this, TQ_SIGNAL(intSignal(int)), receiver, member );
#ifndef TQT_NO_VARIANT
    return TQObject::connect( (TQObject *)this, TQ_SIGNAL(signal(const TQVariant&)),
			     receiver, member );
#endif
}

/*!
    Disonnects the signal from \a member in object \a receiver.

    \sa connect(), TQObject::disconnect()
*/

bool TQSignal::disconnect( const TQObject *receiver, const char *member )
{
    if (!member)
	return TQObject::disconnect( (TQObject *)this, 0, receiver, member);
#ifndef TQT_NO_VARIANT
    if ( intSignature( member ) )
#endif
	return TQObject::disconnect( (TQObject *)this, TQ_SIGNAL(intSignal(int)), receiver, member );
#ifndef TQT_NO_VARIANT
    return TQObject::disconnect( (TQObject *)this, TQ_SIGNAL(signal(const TQVariant&)),
				receiver, member );
#endif
}


/*!
  \fn bool TQSignal::isBlocked() const
  \obsolete
  Returns true if the signal is blocked, or false if it is not blocked.

  The signal is not blocked by default.

  \sa block(), TQObject::signalsBlocked()
*/

/*!
  \fn void TQSignal::block( bool b )
  \obsolete
  Blocks the signal if \a b is true, or unblocks the signal if \a b is false.

  An activated signal disappears into hyperspace if it is blocked.

  \sa isBlocked(), activate(), TQObject::blockSignals()
*/


/*!
    \fn void TQSignal::activate()

    Emits the signal. If the platform supports TQVariant and a
    parameter has been set with setValue(), this value is passed in
    the signal.
*/
void  TQSignal::activate()
{
#ifndef TQT_NO_VARIANT
    /* Create this TQGuardedPtr on this, if we get destroyed after the intSignal (but before the variant signal)
       we cannot just emit the signal (because val has been destroyed already) */
    TQGuardedPtr<TQSignal> me = this;
    if( me ) 
	emit intSignal( val.toInt() );
    if( me ) 
	emit signal( val );
#else
    emit intSignal(0);
#endif
}

#ifndef TQT_NO_VARIANT
/*!
    Sets the signal's parameter to \a value
*/
void TQSignal::setValue( const TQVariant &value )
{
    val = value;
}

/*!
    Returns the signal's parameter
*/
TQVariant TQSignal::value() const
{
    return val;
}
/*! \fn void TQSignal::signal( const TQVariant & )
    \internal
*/
/*! \fn void TQSignal::intSignal( int )
    \internal
*/

#ifndef TQT_NO_COMPAT
/*! \obsolete */
void TQSignal::setParameter( int value )
{
    val = value;
}

/*! \obsolete */
int TQSignal::parameter() const
{
    return val.toInt();
}
#endif
#endif //TQT_NO_VARIANT
