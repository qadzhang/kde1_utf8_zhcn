/****************************************************************************
**
** Definition of TQObjectList
**
** Created : 940807
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

#ifndef TQOBJECTLIST_H
#define TQOBJECTLIST_H

#ifndef QT_H
#include "ntqobject.h"
#include "ntqptrlist.h"
#endif // QT_H


#if defined(Q_TEMPLATEDLL)
//Q_TEMPLATE_EXTERN template class TQ_EXPORT TQPtrList<TQObject>;
//Q_TEMPLATE_EXTERN template class TQ_EXPORT TQPtrListIterator<TQObject>;
#endif


class TQ_EXPORT TQObjectList : public TQPtrList<TQObject>
{
public:
    TQObjectList() : TQPtrList<TQObject>() {}
    TQObjectList( const TQObjectList &list ) : TQPtrList<TQObject>(list) {}
   ~TQObjectList() { clear(); }
    TQObjectList &operator=(const TQObjectList &list)
	{ return (TQObjectList&)TQPtrList<TQObject>::operator=(list); }
};

class TQ_EXPORT TQObjectListIterator : public TQPtrListIterator<TQObject>
{
public:
    TQObjectListIterator( const TQObjectList &l )
	: TQPtrListIterator<TQObject>( l ) { }
    TQObjectListIterator &operator=( const TQObjectListIterator &i )
	{ return (TQObjectListIterator&)
		 TQPtrListIterator<TQObject>::operator=( i ); }
};

#if (TQT_VERSION-0 >= 0x040000)
#if defined(Q_CC_GNU)
#warning "remove the TQObjectListIt class"
#warning "remove the typedef too, maybe"
#endif
typedef TQObjectListIterator TQObjectListIt;
#else
class TQ_EXPORT TQObjectListIt : public TQPtrListIterator<TQObject>
{
public:
    TQObjectListIt( const TQObjectList &l ) : TQPtrListIterator<TQObject>(l) {}
    TQObjectListIt &operator=(const TQObjectListIt &i)
	{ return (TQObjectListIt&)TQPtrListIterator<TQObject>::operator=(i); }
};
#endif

#endif // TQOBJECTLIST_H
