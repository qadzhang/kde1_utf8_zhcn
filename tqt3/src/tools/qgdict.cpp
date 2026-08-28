/****************************************************************************
**
** Implementation of TQGDict and TQGDictIterator classes
**
** Created : 920529
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

#include "ntqgdict.h"
#include "ntqptrlist.h"
#include "ntqstring.h"
#include "ntqdatastream.h"
#include <ctype.h>

/*!
  \class TQGDict
  \reentrant
  \ingroup collection
  \brief The TQGDict class is an internal class for implementing TQDict template classes.

  \internal

  TQGDict is a strictly internal class that acts as a base class for the
  \link collection.html collection classes\endlink TQDict and TQIntDict.

  TQGDict has some virtual functions that can be reimplemented to customize
  the subclasses.
  \list
  \i read() reads a collection/dictionary item from a TQDataStream.
  \i write() writes a collection/dictionary item to a TQDataStream.
  \endlist
  Normally, you do not have to reimplement any of these functions.
*/

static const int op_find = 0;
static const int op_insert = 1;
static const int op_replace = 2;


class TQGDItList : public TQPtrList<TQGDictIterator>
{
public:
    TQGDItList() : TQPtrList<TQGDictIterator>() {}
    TQGDItList( const TQGDItList &list ) : TQPtrList<TQGDictIterator>(list) {}
    ~TQGDItList() { clear(); }
    TQGDItList &operator=(const TQGDItList &list)
	{ return (TQGDItList&)TQPtrList<TQGDictIterator>::operator=(list); }
};


/*****************************************************************************
  Default implementation of special and virtual functions
 *****************************************************************************/

/*!
  Returns the hash key for \a key, when key is a string.
*/

int TQGDict::hashKeyString( const TQString &key )
{
#if defined(QT_CHECK_NULL)
    if ( key.isNull() )
	tqWarning( "TQGDict::hashKeyString: Invalid null key" );
#endif
    int i;
    uint h=0;
    uint g;
    const TQChar *p = key.unicode();
    if ( cases ) {				// case sensitive
	for ( i=0; i<(int)key.length(); i++ ) {
	    h = (h<<4) + p[i].cell();
	    if ( (g = h & 0xf0000000) )
		h ^= g >> 24;
	    h &= ~g;
	}
    } else {					// case insensitive
	for ( i=0; i<(int)key.length(); i++ ) {
	    h = (h<<4) + p[i].lower().cell();
	    if ( (g = h & 0xf0000000) )
		h ^= g >> 24;
	    h &= ~g;
	}
    }
    int index = h;
    if ( index < 0 )				// adjust index to table size
	index = -index;
    return index;
}

/*!
  Returns the hash key for \a key, which is a C string.
*/

int TQGDict::hashKeyAscii( const char *key )
{
#if defined(QT_CHECK_NULL)
    if ( key == 0 )
	tqWarning( "TQGDict::hashAsciiKey: Invalid null key" );
#endif
    const char *k = key;
    uint h=0;
    uint g;
    if ( cases ) {				// case sensitive
	while ( *k ) {
	    h = (h<<4) + *k++;
	    if ( (g = h & 0xf0000000) )
		h ^= g >> 24;
	    h &= ~g;
	}
    } else {					// case insensitive
	while ( *k ) {
	    h = (h<<4) + tolower((uchar) *k);
	    if ( (g = h & 0xf0000000) )
		h ^= g >> 24;
	    h &= ~g;
	    k++;
	}
    }
    int index = h;
    if ( index < 0 )				// adjust index to table size
	index = -index;
    return index;
}

#ifndef TQT_NO_DATASTREAM

/*!
    \overload
  Reads a collection/dictionary item from the stream \a s and returns a
  reference to the stream.

  The default implementation sets \a item to 0.

  \sa write()
*/

TQDataStream& TQGDict::read( TQDataStream &s, TQPtrCollection::Item &item )
{
    item = 0;
    return s;
}

/*!
    \overload
  Writes a collection/dictionary item to the stream \a s and returns a
  reference to the stream.

  \sa read()
*/

TQDataStream& TQGDict::write( TQDataStream &s, TQPtrCollection::Item ) const
{
    return s;
}
#endif //TQT_NO_DATASTREAM

/*****************************************************************************
  TQGDict member functions
 *****************************************************************************/

/*!
  Constructs a dictionary.

  \a len is the initial size of the dictionary.
  The key type is \a kt which may be \c StringKey, \c AsciiKey,
  \c IntKey or \c PtrKey. The case-sensitivity of lookups is set with
  \a caseSensitive. Keys are copied if \a copyKeys is true.
*/

TQGDict::TQGDict( uint len, KeyType kt, bool caseSensitive, bool copyKeys )
{
    init( len, kt, caseSensitive, copyKeys );
}


void TQGDict::init( uint len, KeyType kt, bool caseSensitive, bool copyKeys )
{
    vlen = len ? len : 17;
    vec = new TQBaseBucket *[ vlen ];

    TQ_CHECK_PTR( vec );
    memset( (char*)vec, 0, vlen*sizeof(TQBaseBucket*) );
    numItems  = 0;
    iterators = 0;
    // The caseSensitive and copyKey options don't make sense for
    // all dict types.
    switch ( (keytype = (uint)kt) ) {
	case StringKey:
	    cases = caseSensitive;
	    copyk = false;
	    break;
	case AsciiKey:
	    cases = caseSensitive;
	    copyk = copyKeys;
	    break;
	default:
	    cases = false;
	    copyk = false;
	    break;
    }
}


/*!
  Constructs a copy of \a dict.
*/

TQGDict::TQGDict( const TQGDict & dict )
    : TQPtrCollection( dict )
{
    init( dict.vlen, (KeyType)dict.keytype, dict.cases, dict.copyk );
    TQGDictIterator it( dict );
    while ( it.get() ) {			// copy from other dict
	switch ( keytype ) {
	    case StringKey:
		look_string( it.getKeyString(), it.get(), op_insert );
		break;
	    case AsciiKey:
		look_ascii( it.getKeyAscii(), it.get(), op_insert );
		break;
	    case IntKey:
		look_int( it.getKeyInt(), it.get(), op_insert );
		break;
	    case PtrKey:
		look_ptr( it.getKeyPtr(), it.get(), op_insert );
		break;
	}
	++it;
    }
}


/*!
  Removes all items from the dictionary and destroys it.
*/

TQGDict::~TQGDict()
{
    clear();					// delete everything
    delete [] vec;
    if ( !iterators )				// no iterators for this dict
	return;
    TQGDictIterator *i = iterators->first();
    while ( i ) {				// notify all iterators that
	i->dict = 0;				// this dict is deleted
	i = iterators->next();
    }
    delete iterators;
}


/*!
  Assigns \a dict to this dictionary.
*/

TQGDict &TQGDict::operator=( const TQGDict &dict )
{
    if ( &dict == this )
	return *this;
    clear();
    TQGDictIterator it( dict );
    while ( it.get() ) {			// copy from other dict
	switch ( keytype ) {
	    case StringKey:
		look_string( it.getKeyString(), it.get(), op_insert );
		break;
	    case AsciiKey:
		look_ascii( it.getKeyAscii(), it.get(), op_insert );
		break;
	    case IntKey:
		look_int( it.getKeyInt(), it.get(), op_insert );
		break;
	    case PtrKey:
		look_ptr( it.getKeyPtr(), it.get(), op_insert );
		break;
	}
	++it;
    }
    return *this;
}

/*!
  \fn uint TQGDict::count() const

  Returns the number of items in the dictionary.
*/

/*!
  \fn uint TQGDict::size() const

  Returns the size of the hash array.
*/

/*!
  The do-it-all function; \a op is one of op_find, op_insert, op_replace.
  The key is \a key and the item is \a d.
*/

TQPtrCollection::Item TQGDict::look_string( const TQString &key, TQPtrCollection::Item d,
				       int op )
{
    TQStringBucket *n = 0;
    int	index = hashKeyString(key) % vlen;
    if ( op == op_find ) {			// find
	if ( cases ) {
	    n = (TQStringBucket*)vec[index];
	    while( n != 0 ) {
		if ( key == n->getKey() )
		    return n->getData();	// item found
		n = (TQStringBucket*)n->getNext();
	    }
	} else {
	    TQString k = key.lower();
	    n = (TQStringBucket*)vec[index];
	    while( n != 0 ) {
		if ( k == n->getKey().lower() )
		    return n->getData();	// item found
		n = (TQStringBucket*)n->getNext();
	    }
	}
	return 0;				// not found
    }
    if ( op == op_replace ) {			// replace
	if ( vec[index] != 0 )			// maybe something there
	    remove_string( key );
    }
    // op_insert or op_replace
    n = new TQStringBucket(key,newItem(d),vec[index]);
    TQ_CHECK_PTR( n );
#if defined(QT_CHECK_NULL)
    if ( n->getData() == 0 )
	tqWarning( "TQDict: Cannot insert null item" );
#endif
    vec[index] = n;
    numItems++;
    return n->getData();
}

TQPtrCollection::Item TQGDict::look_ascii( const char *key, TQPtrCollection::Item d, int op )
{
    TQAsciiBucket *n;
    int	index = hashKeyAscii(key) % vlen;
    if ( op == op_find ) {			// find
	if ( cases ) {
	    for ( n=(TQAsciiBucket*)vec[index]; n;
		  n=(TQAsciiBucket*)n->getNext() ) {
		if ( qstrcmp(n->getKey(),key) == 0 )
		    return n->getData();	// item found
	    }
	} else {
	    for ( n=(TQAsciiBucket*)vec[index]; n;
		  n=(TQAsciiBucket*)n->getNext() ) {
		if ( tqstricmp(n->getKey(),key) == 0 )
		    return n->getData();	// item found
	    }
	}
	return 0;				// not found
    }
    if ( op == op_replace ) {			// replace
	if ( vec[index] != 0 )			// maybe something there
	    remove_ascii( key );
    }
    // op_insert or op_replace
    n = new TQAsciiBucket(copyk ? tqstrdup(key) : key,newItem(d),vec[index]);
    TQ_CHECK_PTR( n );
#if defined(QT_CHECK_NULL)
    if ( n->getData() == 0 )
	tqWarning( "TQAsciiDict: Cannot insert null item" );
#endif
    vec[index] = n;
    numItems++;
    return n->getData();
}

TQPtrCollection::Item TQGDict::look_int( long key, TQPtrCollection::Item d, int op )
{
    TQIntBucket *n;
    int index = (int)((ulong)key % vlen);	// simple hash
    if ( op == op_find ) {			// find
	for ( n=(TQIntBucket*)vec[index]; n;
	      n=(TQIntBucket*)n->getNext() ) {
	    if ( n->getKey() == key )
		return n->getData();		// item found
	}
	return 0;				// not found
    }
    if ( op == op_replace ) {			// replace
	if ( vec[index] != 0 )			// maybe something there
	    remove_int( key );
    }
    // op_insert or op_replace
    n = new TQIntBucket(key,newItem(d),vec[index]);
    TQ_CHECK_PTR( n );
#if defined(QT_CHECK_NULL)
    if ( n->getData() == 0 )
	tqWarning( "TQIntDict: Cannot insert null item" );
#endif
    vec[index] = n;
    numItems++;
    return n->getData();
}

TQPtrCollection::Item TQGDict::look_ptr( void *key, TQPtrCollection::Item d, int op )
{
    TQPtrBucket *n;
    int index = (int)((ulong)key % vlen);	// simple hash
    if ( op == op_find ) {			// find
	for ( n=(TQPtrBucket*)vec[index]; n;
	      n=(TQPtrBucket*)n->getNext() ) {
	    if ( n->getKey() == key )
		return n->getData();		// item found
	}
	return 0;				// not found
    }
    if ( op == op_replace ) {			// replace
	if ( vec[index] != 0 )			// maybe something there
	    remove_ptr( key );
    }
    // op_insert or op_replace
    n = new TQPtrBucket(key,newItem(d),vec[index]);
    TQ_CHECK_PTR( n );
#if defined(QT_CHECK_NULL)
    if ( n->getData() == 0 )
	tqWarning( "TQPtrDict: Cannot insert null item" );
#endif
    vec[index] = n;
    numItems++;
    return n->getData();
}


/*!
  Changes the size of the hashtable to \a newsize.
  The contents of the dictionary are preserved,
  but all iterators on the dictionary become invalid.
*/
void TQGDict::resize( uint newsize )
{
    // Save old information
    TQBaseBucket **old_vec = vec;
    uint old_vlen  = vlen;
    bool old_copyk = copyk;

    vec = new TQBaseBucket *[vlen = newsize];
    TQ_CHECK_PTR( vec );
    memset( (char*)vec, 0, vlen*sizeof(TQBaseBucket*) );
    numItems = 0;
    copyk = false;

    // Reinsert every item from vec, deleting vec as we go
    for ( uint index = 0; index < old_vlen; index++ ) {
	switch ( keytype ) {
	    case StringKey:
		{
		    TQStringBucket *n=(TQStringBucket *)old_vec[index];
		    while ( n ) {
			look_string( n->getKey(), n->getData(), op_insert );
			TQStringBucket *t=(TQStringBucket *)n->getNext();
			delete n;
			n = t;
		    }
		}
		break;
	    case AsciiKey:
		{
		    TQAsciiBucket *n=(TQAsciiBucket *)old_vec[index];
		    while ( n ) {
			look_ascii( n->getKey(), n->getData(), op_insert );
			TQAsciiBucket *t=(TQAsciiBucket *)n->getNext();
			delete n;
			n = t;
		    }
		}
		break;
	    case IntKey:
		{
		    TQIntBucket *n=(TQIntBucket *)old_vec[index];
		    while ( n ) {
			look_int( n->getKey(), n->getData(), op_insert );
			TQIntBucket *t=(TQIntBucket *)n->getNext();
			delete n;
			n = t;
		    }
		}
		break;
	    case PtrKey:
		{
		    TQPtrBucket *n=(TQPtrBucket *)old_vec[index];
		    while ( n ) {
			look_ptr( n->getKey(), n->getData(), op_insert );
			TQPtrBucket *t=(TQPtrBucket *)n->getNext();
			delete n;
			n = t;
		    }
		}
		break;
	}
    }
    delete [] old_vec;

    // Restore state
    copyk = old_copyk;

    // Invalidate all iterators, since order is lost
    if ( iterators && iterators->count() ) {
	TQGDictIterator *i = iterators->first();
	while ( i ) {
	    i->toFirst();
	    i = iterators->next();
	}
    }
}

/*!
  Unlinks the bucket with the specified key (and specified data pointer,
  if it is set).
*/

void TQGDict::unlink_common( int index, TQBaseBucket *node, TQBaseBucket *prev )
{
    if ( iterators && iterators->count() ) {	// update iterators
	TQGDictIterator *i = iterators->first();
	while ( i ) {				// invalidate all iterators
	    if ( i->curNode == node )		// referring to pending node
		i->operator++();
	    i = iterators->next();
	}
    }
    if ( prev )					// unlink node
	prev->setNext( node->getNext() );
    else
	vec[index] = node->getNext();
    numItems--;
}

TQStringBucket *TQGDict::unlink_string( const TQString &key, TQPtrCollection::Item d )
{
    if ( numItems == 0 )			// nothing in dictionary
	return 0;
    TQStringBucket *n;
    TQStringBucket *prev = 0;
    int index = hashKeyString(key) % vlen;
    if ( cases ) {
	for ( n=(TQStringBucket*)vec[index]; n;
	      n=(TQStringBucket*)n->getNext() ) {
	    bool found = (key == n->getKey());
	    if ( found && d )
		found = (n->getData() == d);
	    if ( found ) {
		unlink_common(index,n,prev);
		return n;
	    }
	    prev = n;
	}
    } else {
	TQString k = key.lower();
	for ( n=(TQStringBucket*)vec[index]; n;
	      n=(TQStringBucket*)n->getNext() ) {
	    bool found = (k == n->getKey().lower());
	    if ( found && d )
		found = (n->getData() == d);
	    if ( found ) {
		unlink_common(index,n,prev);
		return n;
	    }
	    prev = n;
	}
    }
    return 0;
}

TQAsciiBucket *TQGDict::unlink_ascii( const char *key, TQPtrCollection::Item d )
{
    if ( numItems == 0 )			// nothing in dictionary
	return 0;
    TQAsciiBucket *n;
    TQAsciiBucket *prev = 0;
    int index = hashKeyAscii(key) % vlen;
    for ( n=(TQAsciiBucket *)vec[index]; n; n=(TQAsciiBucket *)n->getNext() ) {
	bool found = (cases ? qstrcmp(n->getKey(),key)
		       : tqstricmp(n->getKey(),key)) == 0;
	if ( found && d )
	    found = (n->getData() == d);
	if ( found ) {
	    unlink_common(index,n,prev);
	    return n;
	}
	prev = n;
    }
    return 0;
}

TQIntBucket *TQGDict::unlink_int( long key, TQPtrCollection::Item d )
{
    if ( numItems == 0 )			// nothing in dictionary
	return 0;
    TQIntBucket *n;
    TQIntBucket *prev = 0;
    int index = (int)((ulong)key % vlen);
    for ( n=(TQIntBucket *)vec[index]; n; n=(TQIntBucket *)n->getNext() ) {
	bool found = (n->getKey() == key);
	if ( found && d )
	    found = (n->getData() == d);
	if ( found ) {
	    unlink_common(index,n,prev);
	    return n;
	}
	prev = n;
    }
    return 0;
}

TQPtrBucket *TQGDict::unlink_ptr( void *key, TQPtrCollection::Item d )
{
    if ( numItems == 0 )			// nothing in dictionary
	return 0;
    TQPtrBucket *n;
    TQPtrBucket *prev = 0;
    int index = (int)((ulong)key % vlen);
    for ( n=(TQPtrBucket *)vec[index]; n; n=(TQPtrBucket *)n->getNext() ) {
	bool found = (n->getKey() == key);
	if ( found && d )
	    found = (n->getData() == d);
	if ( found ) {
	    unlink_common(index,n,prev);
	    return n;
	}
	prev = n;
    }
    return 0;
}


/*!
  Removes the item with the specified \a key.  If \a item is not null,
  the remove will match the \a item as well (used to remove an
  item when several items have the same key).
*/

bool TQGDict::remove_string( const TQString &key, TQPtrCollection::Item item )
{
    TQStringBucket *n = unlink_string( key, item );
    if ( n ) {
	deleteItem( n->getData() );
	delete n;
	return true;
    } else {
	return false;
    }
}

bool TQGDict::remove_ascii( const char *key, TQPtrCollection::Item item )
{
    TQAsciiBucket *n = unlink_ascii( key, item );
    if ( n ) {
	if ( copyk )
	    delete [] (char *)n->getKey();
	deleteItem( n->getData() );
	delete n;
    }
    return n != 0;
}

bool TQGDict::remove_int( long key, TQPtrCollection::Item item )
{
    TQIntBucket *n = unlink_int( key, item );
    if ( n ) {
	deleteItem( n->getData() );
	delete n;
    }
    return n != 0;
}

bool TQGDict::remove_ptr( void *key, TQPtrCollection::Item item )
{
    TQPtrBucket *n = unlink_ptr( key, item );
    if ( n ) {
	deleteItem( n->getData() );
	delete n;
    }
    return n != 0;
}

TQPtrCollection::Item TQGDict::take_string( const TQString &key )
{
    TQStringBucket *n = unlink_string( key );
    Item d;
    if ( n ) {
	d = n->getData();
	delete n;
    } else {
	d = 0;
    }
    return d;
}

TQPtrCollection::Item TQGDict::take_ascii( const char *key )
{
    TQAsciiBucket *n = unlink_ascii( key );
    Item d;
    if ( n ) {
	if ( copyk )
	    delete [] (char *)n->getKey();
	d = n->getData();
	delete n;
    } else {
	d = 0;
    }
    return d;
}

TQPtrCollection::Item TQGDict::take_int( long key )
{
    TQIntBucket *n = unlink_int( key );
    Item d;
    if ( n ) {
	d = n->getData();
	delete n;
    } else {
	d = 0;
    }
    return d;
}

TQPtrCollection::Item TQGDict::take_ptr( void *key )
{
    TQPtrBucket *n = unlink_ptr( key );
    Item d;
    if ( n ) {
	d = n->getData();
	delete n;
    } else {
	d = 0;
    }
    return d;
}

/*!
  Removes all items from the dictionary.
*/
void TQGDict::clear()
{
    if ( !numItems )
	return;
    numItems = 0;				// disable remove() function
    for ( uint j=0; j<vlen; j++ ) {		// destroy hash table
	if ( vec[j] ) {
	    switch ( keytype ) {
		case StringKey:
		    {
			TQStringBucket *n=(TQStringBucket *)vec[j];
			while ( n ) {
			    TQStringBucket *next = (TQStringBucket*)n->getNext();
			    deleteItem( n->getData() );
			    delete n;
			    n = next;
			}
		    }
		    break;
		case AsciiKey:
		    {
			TQAsciiBucket *n=(TQAsciiBucket *)vec[j];
			while ( n ) {
			    TQAsciiBucket *next = (TQAsciiBucket*)n->getNext();
			    if ( copyk )
				delete [] (char *)n->getKey();
			    deleteItem( n->getData() );
			    delete n;
			    n = next;
			}
		    }
		    break;
		case IntKey:
		    {
			TQIntBucket *n=(TQIntBucket *)vec[j];
			while ( n ) {
			    TQIntBucket *next = (TQIntBucket*)n->getNext();
			    deleteItem( n->getData() );
			    delete n;
			    n = next;
			}
		    }
		    break;
		case PtrKey:
		    {
			TQPtrBucket *n=(TQPtrBucket *)vec[j];
			while ( n ) {
			    TQPtrBucket *next = (TQPtrBucket*)n->getNext();
			    deleteItem( n->getData() );
			    delete n;
			    n = next;
			}
		    }
		    break;
	    }
	    vec[j] = 0;				// detach list of buckets
	}
    }
    if ( iterators && iterators->count() ) {	// invalidate all iterators
	TQGDictIterator *i = iterators->first();
	while ( i ) {
	    i->curNode = 0;
	    i = iterators->next();
	}
    }
}

/*!
  Outputs debug statistics.
*/
void TQGDict::statistics() const
{
#if defined(QT_DEBUG)
    TQString line;
    line.fill( '-', 60 );
    double real, ideal;
    tqDebug( "%s", line.ascii() );
    tqDebug( "DICTIONARY STATISTICS:" );
    if ( count() == 0 ) {
	tqDebug( "Empty!" );
	tqDebug( "%s", line.ascii() );
	return;
    }
    real = 0.0;
    ideal = (float)count()/(2.0*size())*(count()+2.0*size()-1);
    uint i = 0;
    while ( i<size() ) {
	TQBaseBucket *n = vec[i];
	int b = 0;
	while ( n ) {				// count number of buckets
	    b++;
	    n = n->getNext();
	}
	real = real + (double)b * ((double)b+1.0)/2.0;
	char buf[80], *pbuf;
	if ( b > 78 )
	    b = 78;
	pbuf = buf;
	while ( b-- )
	    *pbuf++ = '*';
	*pbuf = '\0';
	tqDebug( "%s", buf );
	i++;
    }
    tqDebug( "Array size = %d", size() );
    tqDebug( "# items    = %d", count() );
    tqDebug( "Real dist  = %g", real );
    tqDebug( "Rand dist  = %g", ideal );
    tqDebug( "Real/Rand  = %g", real/ideal );
    tqDebug( "%s", line.ascii() );
#endif // QT_DEBUG
}


/*****************************************************************************
  TQGDict stream functions
 *****************************************************************************/
#ifndef TQT_NO_DATASTREAM
TQDataStream &operator>>( TQDataStream &s, TQGDict &dict )
{
    return dict.read( s );
}

TQDataStream &operator<<( TQDataStream &s, const TQGDict &dict )
{
    return dict.write( s );
}

#if defined(Q_CC_DEC) && defined(__alpha) && (__DECCXX_VER-0 >= 50190001)
#pragma message disable narrowptr
#endif

/*!
  Reads a dictionary from the stream \a s.
*/

TQDataStream &TQGDict::read( TQDataStream &s )
{
    uint num;
    s >> num;					// read number of items
    clear();					// clear dict
    while ( num-- ) {				// read all items
	Item d;
	switch ( keytype ) {
	    case StringKey:
		{
		    TQString k;
		    s >> k;
		    read( s, d );
		    look_string( k, d, op_insert );
		}
		break;
	    case AsciiKey:
		{
		    char *k;
		    s >> k;
		    read( s, d );
		    look_ascii( k, d, op_insert );
		    if ( copyk )
			delete [] k;
		}
		break;
	    case IntKey:
		{
		    TQ_UINT32 k;
		    s >> k;
		    read( s, d );
		    look_int( k, d, op_insert );
		}
		break;
	    case PtrKey:
		{
		    TQ_UINT32 k;
		    s >> k;
		    read( s, d );
		    // Pointer real values are not serialized, so it doesn't make sense
		    // to insert null values back into the dictionary by calling 'look_ptr' here
		}
		break;
	}
    }
    return s;
}

/*!
  Writes the dictionary to the stream \a s.
*/

TQDataStream& TQGDict::write( TQDataStream &s ) const
{
    s << count();				// write number of items
    uint i = 0;
    while ( i<size() ) {
	TQBaseBucket *n = vec[i];
	while ( n ) {				// write all buckets
	    switch ( keytype ) {
		case StringKey:
		    s << ((TQStringBucket*)n)->getKey();
		    break;
		case AsciiKey:
		    s << ((TQAsciiBucket*)n)->getKey();
		    break;
		case IntKey:
		    s << (TQ_UINT32)((TQIntBucket*)n)->getKey();
		    break;
		case PtrKey:
		    s << (TQ_UINT32)0; // ### it doesn't make sense to serialize a pointer's value
		    break;
	    }
	    write( s, n->getData() );		// write data
	    n = n->getNext();
	}
	i++;
    }
    return s;
}
#endif //TQT_NO_DATASTREAM

/*****************************************************************************
  TQGDictIterator member functions
 *****************************************************************************/

/*!
  \class TQGDictIterator ntqgdict.h
  \reentrant
  \ingroup collection
  \brief The TQGDictIterator class is an internal class for implementing TQDictIterator and TQIntDictIterator.

  \internal

  TQGDictIterator is a strictly internal class that does the heavy work for
  TQDictIterator and TQIntDictIterator.
*/

/*!
  Constructs an iterator that operates on the dictionary \a d.
*/

TQGDictIterator::TQGDictIterator( const TQGDict &d )
{
    dict = (TQGDict *)&d;			// get reference to dict
    toFirst();					// set to first noe
    if ( !dict->iterators ) {
	dict->iterators = new TQGDItList;	// create iterator list
	TQ_CHECK_PTR( dict->iterators );
    }
    dict->iterators->append( this );		// attach iterator to dict
}

/*!
  Constructs a copy of the iterator \a it.
*/

TQGDictIterator::TQGDictIterator( const TQGDictIterator &it )
{
    dict = it.dict;
    curNode = it.curNode;
    curIndex = it.curIndex;
    if ( dict )
	dict->iterators->append( this );	// attach iterator to dict
}

/*!
  Assigns a copy of the iterator \a it and returns a reference to this
  iterator.
*/

TQGDictIterator &TQGDictIterator::operator=( const TQGDictIterator &it )
{
    if ( dict )					// detach from old dict
	dict->iterators->removeRef( this );
    dict = it.dict;
    curNode = it.curNode;
    curIndex = it.curIndex;
    if ( dict )
	dict->iterators->append( this );	// attach to new list
    return *this;
}

/*!
  Destroys the iterator.
*/

TQGDictIterator::~TQGDictIterator()
{
    if ( dict )					// detach iterator from dict
	dict->iterators->removeRef( this );
}


/*!
  Sets the iterator to point to the first item in the dictionary.
*/

TQPtrCollection::Item TQGDictIterator::toFirst()
{
    if ( !dict ) {
#if defined(QT_CHECK_NULL)
	tqWarning( "TQGDictIterator::toFirst: Dictionary has been deleted" );
#endif
	return 0;
    }
    if ( dict->count() == 0 ) {			// empty dictionary
	curNode = 0;
	return 0;
    }
    uint i = 0;
    TQBaseBucket **v = dict->vec;
    while ( !(*v++) )
	i++;
    curNode = dict->vec[i];
    curIndex = i;
    return curNode->getData();
}


/*!
  Moves to the next item (postfix).
*/

TQPtrCollection::Item TQGDictIterator::operator()()
{
    if ( !dict ) {
#if defined(QT_CHECK_NULL)
	tqWarning( "TQGDictIterator::operator(): Dictionary has been deleted" );
#endif
	return 0;
    }
    if ( !curNode )
	return 0;
    TQPtrCollection::Item d = curNode->getData();
    this->operator++();
    return d;
}

/*!
  Moves to the next item (prefix).
*/

TQPtrCollection::Item TQGDictIterator::operator++()
{
    if ( !dict ) {
#if defined(QT_CHECK_NULL)
	tqWarning( "TQGDictIterator::operator++: Dictionary has been deleted" );
#endif
	return 0;
    }
    if ( !curNode )
	return 0;
    curNode = curNode->getNext();
    if ( !curNode ) {				// no next bucket
	uint i = curIndex + 1;		// look from next vec element
	TQBaseBucket **v = &dict->vec[i];
	while ( i < dict->size() && !(*v++) )
	    i++;
	if ( i == dict->size() ) {		// nothing found
	    curNode = 0;
	    return 0;
	}
	curNode = dict->vec[i];
	curIndex = i;
    }
    return curNode->getData();
}

/*!
  Moves \a jumps positions forward.
*/

TQPtrCollection::Item TQGDictIterator::operator+=( uint jumps )
{
    while ( curNode && jumps-- )
	operator++();
    return curNode ? curNode->getData() : 0;
}
