/****************************************************************************
**
** Definition of TQSignal class
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

#ifndef TQSIGNAL_H
#define TQSIGNAL_H

#ifndef QT_H
#include "ntqvariant.h"
#include "ntqobject.h"
#endif // QT_H


class TQ_EXPORT TQSignal : public TQObject
{
    TQ_OBJECT

public:
    TQSignal( TQObject *parent=0, const char *name=0 );
    ~TQSignal();

    bool	connect( const TQObject *receiver, const char *member );
    bool	disconnect( const TQObject *receiver, const char *member=0 );

    void	activate();

#ifndef TQT_NO_COMPAT
    bool	isBlocked()	 const		{ return TQObject::signalsBlocked(); }
    void	block( bool b )		{ TQObject::blockSignals( b ); }
#ifndef TQT_NO_VARIANT
    void	setParameter( int value );
    int		parameter() const;
#endif
#endif

#ifndef TQT_NO_VARIANT
    void	setValue( const TQVariant &value );
    TQVariant	value() const;
#endif
signals:
#ifndef TQT_NO_VARIANT
    void signal( const TQVariant& );
#endif
    void intSignal( int );

private:
#ifndef TQT_NO_VARIANT
    TQVariant val;
#endif
private:	// Disabled copy constructor and operator=
#if defined(TQ_DISABLE_COPY)
    TQSignal( const TQSignal & );
    TQSignal &operator=( const TQSignal & );
#endif
};


#endif // TQSIGNAL_H
