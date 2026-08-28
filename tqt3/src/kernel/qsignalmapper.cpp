/****************************************************************************
**
** Implementation of TQSignalMapper class
**
** Created : 980503
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

#include "ntqsignalmapper.h"
#ifndef TQT_NO_SIGNALMAPPER
#include "ntqptrdict.h"

struct TQSignalMapperRec {
    TQSignalMapperRec()
    {
	has_int = 0;
	str_id = TQString::null;
    }

    uint has_int:1;

    int int_id;
    TQString str_id;
    // extendable to other types of identification
};

class TQSignalMapperData {
public:
    TQSignalMapperData()
    {
	dict.setAutoDelete( true );
    }

    TQPtrDict<TQSignalMapperRec> dict;
};

/*!
    \class TQSignalMapper ntqsignalmapper.h
    \brief The TQSignalMapper class bundles signals from identifiable senders.

    \ingroup io

    This class collects a set of parameterless signals, and re-emits
    them with integer or string parameters corresponding to the object
    that sent the signal.
*/

/*!
    Constructs a TQSignalMapper called \a name, with parent \a parent.
    Like all TQObjects, it will be deleted when the parent is deleted.
*/
TQSignalMapper::TQSignalMapper( TQObject* parent, const char* name ) :
    TQObject( parent, name )
{
    d = new TQSignalMapperData;
}

/*!
    Destroys the TQSignalMapper.
*/
TQSignalMapper::~TQSignalMapper()
{
    delete d;
}

/*!
    Adds a mapping so that when map() is signaled from the given \a
    sender, the signal mapped(\a identifier) is emitted.

    There may be at most one integer identifier for each object.
*/
void TQSignalMapper::setMapping( const TQObject* sender, int identifier )
{
    TQSignalMapperRec* rec = getRec(sender);
    rec->int_id = identifier;
    rec->has_int = 1;
}

/*!
    \overload

    Adds a mapping so that when map() is signaled from the given \a
    sender, the signal mapper(\a identifier) is emitted.

    There may be at most one string identifier for each object, and it
    may not be empty.
*/
void TQSignalMapper::setMapping( const TQObject* sender, const TQString &identifier )
{
    TQSignalMapperRec* rec = getRec(sender);
    rec->str_id = identifier;
}

/*!
    Removes all mappings for \a sender. This is done automatically
    when mapped objects are destroyed.
*/
void TQSignalMapper::removeMappings( const TQObject* sender )
{
    d->dict.remove((void*)sender);
}

void TQSignalMapper::removeMapping()
{
    removeMappings(sender());
}

/*!
    This slot emits signals based on which object sends signals to it.
*/
void TQSignalMapper::map()
{
    const TQObject* s = sender();
    TQSignalMapperRec* rec = d->dict.find( (void*)s );
    if ( rec ) {
	if ( rec->has_int )
	    emit mapped( rec->int_id );
	if ( !rec->str_id.isEmpty() )
	    emit mapped( rec->str_id );
    }
}

TQSignalMapperRec* TQSignalMapper::getRec( const TQObject* sender )
{
    TQSignalMapperRec* rec = d->dict.find( (void*)sender );
    if (!rec) {
	rec = new TQSignalMapperRec;
	d->dict.insert( (void*)sender, rec );
	connect( sender, TQ_SIGNAL(destroyed()), this, TQ_SLOT(removeMapping()) );
    }
    return rec;
}

/*!
    \fn void TQSignalMapper::mapped(int)

    This signal is emitted when map() is signaled from an object that
    has an integer mapping set.

    \sa setMapping()
*/

/*!
    \overload void TQSignalMapper::mapped(const TQString&)

    This signal is emitted when map() is signaled from an object that
    has a string mapping set.

    \sa setMapping()
*/
#endif //TQT_NO_SIGNALMAPPER
