/****************************************************************************
**
** Definition of TQPtrStack pointer based template class
**
** Created : 920917
**
** Copyright (C) 1992-2008 Trolltech ASA.  All rights reserved.
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

#ifndef TQPTRSTACK_H
#define TQPTRSTACK_H

#ifndef QT_H
#include "ntqglist.h"
#endif // QT_H

template<class type>
class TQPtrStack : protected TQGList
{
public:
    TQPtrStack()				{ }
    TQPtrStack( const TQPtrStack<type> &s ) : TQGList( s ) { }
    ~TQPtrStack()			{ clear(); }
    TQPtrStack<type> &operator=(const TQPtrStack<type> &s)
			{ return (TQPtrStack<type>&)TQGList::operator=(s); }
    bool  autoDelete() const		{ return TQPtrCollection::autoDelete(); }
    void  setAutoDelete( bool del )	{ TQPtrCollection::setAutoDelete(del); }
    uint  count()   const		{ return TQGList::count(); }
    bool  isEmpty() const		{ return TQGList::count() == 0; }
    void  push( const type *d )		{ TQGList::insertAt(0,Item(d)); }
    type *pop()				{ return (type *)TQGList::takeFirst(); }
    bool  remove()			{ return TQGList::removeFirst(); }
    void  clear()			{ TQGList::clear(); }
    type *top()	    const		{ return (type *)TQGList::cfirst(); }
	  operator type *() const	{ return (type *)TQGList::cfirst(); }
    type *current() const		{ return (type *)TQGList::cfirst(); }

#ifdef Q_QDOC
protected:
    virtual TQDataStream& read( TQDataStream&, TQPtrCollection::Item& );
    virtual TQDataStream& write( TQDataStream&, TQPtrCollection::Item ) const;
#endif

private:
    void  deleteItem( Item d );
};

template<> inline void TQPtrStack<void>::deleteItem( TQPtrCollection::Item )
{
}

template<class type> inline void TQPtrStack<type>::deleteItem( TQPtrCollection::Item d )
{
    if ( del_item ) delete (type *)d;
}

#endif // TQPTRSTACK_H
