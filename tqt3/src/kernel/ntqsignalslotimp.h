/****************************************************************************
**
** Definition of signal/slot collections etc.
**
** Created : 980821
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

#ifndef TQSIGNALSLOTIMP_H
#define TQSIGNALSLOTIMP_H

#ifndef QT_H
#include "ntqconnection.h"
#include "ntqptrlist.h"
#include "ntqptrvector.h"
#endif // QT_H

class TQ_EXPORT TQConnectionList : public TQPtrList<TQConnection>
{
public:
    TQConnectionList() : TQPtrList<TQConnection>() {}
    TQConnectionList( const TQConnectionList &list ) : TQPtrList<TQConnection>(list) {}
   ~TQConnectionList() { clear(); }
    TQConnectionList &operator=(const TQConnectionList &list)
	{ return (TQConnectionList&)TQPtrList<TQConnection>::operator=(list); }
};

class TQ_EXPORT TQConnectionListIt : public TQPtrListIterator<TQConnection>
{
public:
    TQConnectionListIt( const TQConnectionList &l ) : TQPtrListIterator<TQConnection>(l) {}
    TQConnectionListIt &operator=(const TQConnectionListIt &i)
	{ return (TQConnectionListIt&)TQPtrListIterator<TQConnection>::operator=(i); }
};

#if defined(Q_TEMPLATEDLL) && defined(Q_CC_INTEL)
// MOC_SKIP_BEGIN
Q_TEMPLATE_EXTERN template class TQ_EXPORT TQPtrVector<TQConnectionList>;
#define TQ_EXPORTED_QPTRVECTORCONNECTTIONLIST_TEMPLATES
// MOC_SKIP_END
#endif

class TQ_EXPORT TQSignalVec : public TQPtrVector<TQConnectionList>
{
public:
    TQSignalVec(int size=17 )
	: TQPtrVector<TQConnectionList>(size) {}
    TQSignalVec( const TQSignalVec &dict )
	: TQPtrVector<TQConnectionList>(dict) {}
   ~TQSignalVec() { clear(); }
    TQSignalVec &operator=(const TQSignalVec &dict)
	{ return (TQSignalVec&)TQPtrVector<TQConnectionList>::operator=(dict); }
    TQConnectionList* at( uint index ) const  {
	return index >= size()? 0 : TQPtrVector<TQConnectionList>::at(index);
    }
    bool  insert( uint index, const TQConnectionList* d ) {
	if (index >= size() )
	    resize( 2*index + 1);
	return TQPtrVector<TQConnectionList>::insert(index, d);
    }
};

#define Q_DEFINED_QCONNECTION_LIST
#include "ntqwinexport.h"
#endif // TQSIGNALSLOTIMP_H
