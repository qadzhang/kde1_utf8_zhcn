/****************************************************************************
**
** Definition of TQGDict and TQGDictIterator classes
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

#ifndef TQGDICT_H
#define TQGDICT_H

#ifndef QT_H
#include "ntqptrcollection.h"
#include "ntqstring.h"
#endif // QT_H

class TQGDictIterator;
class TQGDItList;


class TQBaseBucket				// internal dict node
{
public:
    TQPtrCollection::Item	 getData()			{ return data; }
    TQPtrCollection::Item	 setData( TQPtrCollection::Item d ) { return data = d; }
    TQBaseBucket		*getNext()			{ return next; }
    void		 setNext( TQBaseBucket *n)	{ next = n; }
protected:
    TQBaseBucket( TQPtrCollection::Item d, TQBaseBucket *n ) : data(d), next(n) {}
    TQPtrCollection::Item	 data;
    TQBaseBucket		*next;
};

class TQStringBucket : public TQBaseBucket
{
public:
    TQStringBucket( const TQString &k, TQPtrCollection::Item d, TQBaseBucket *n )
	: TQBaseBucket(d,n), key(k)		{}
    const TQString  &getKey() const		{ return key; }
private:
    TQString	    key;
};

class TQAsciiBucket : public TQBaseBucket
{
public:
    TQAsciiBucket( const char *k, TQPtrCollection::Item d, TQBaseBucket *n )
	: TQBaseBucket(d,n), key(k) {}
    const char *getKey() const { return key; }
private:
    const char *key;
};

class TQIntBucket : public TQBaseBucket
{
public:
    TQIntBucket( long k, TQPtrCollection::Item d, TQBaseBucket *n )
	: TQBaseBucket(d,n), key(k) {}
    long  getKey() const { return key; }
private:
    long  key;
};

class TQPtrBucket : public TQBaseBucket
{
public:
    TQPtrBucket( void *k, TQPtrCollection::Item d, TQBaseBucket *n )
	: TQBaseBucket(d,n), key(k) {}
    void *getKey() const { return key; }
private:
    void *key;
};


class TQ_EXPORT TQGDict : public TQPtrCollection	// generic dictionary class
{
public:
    uint	count() const	{ return numItems; }
    uint	size()	const	{ return vlen; }
    TQPtrCollection::Item look_string( const TQString& key, TQPtrCollection::Item,
				   int );
    TQPtrCollection::Item look_ascii( const char *key, TQPtrCollection::Item, int );
    TQPtrCollection::Item look_int( long key, TQPtrCollection::Item, int );
    TQPtrCollection::Item look_ptr( void *key, TQPtrCollection::Item, int );
#ifndef TQT_NO_DATASTREAM
    TQDataStream &read( TQDataStream & );
    TQDataStream &write( TQDataStream & ) const;
#endif
protected:
    enum KeyType { StringKey, AsciiKey, IntKey, PtrKey };

    TQGDict( uint len, KeyType kt, bool cs, bool ck );
    TQGDict( const TQGDict & );
   ~TQGDict();

    TQGDict     &operator=( const TQGDict & );

    bool	remove_string( const TQString &key, TQPtrCollection::Item item=0 );
    bool	remove_ascii( const char *key, TQPtrCollection::Item item=0 );
    bool	remove_int( long key, TQPtrCollection::Item item=0 );
    bool	remove_ptr( void *key, TQPtrCollection::Item item=0 );
    TQPtrCollection::Item take_string( const TQString &key );
    TQPtrCollection::Item take_ascii( const char *key );
    TQPtrCollection::Item take_int( long key );
    TQPtrCollection::Item take_ptr( void *key );

    void	clear();
    void	resize( uint );

    int		hashKeyString( const TQString & );
    int		hashKeyAscii( const char * );

    void	statistics() const;

#ifndef TQT_NO_DATASTREAM
    virtual TQDataStream &read( TQDataStream &, TQPtrCollection::Item & );
    virtual TQDataStream &write( TQDataStream &, TQPtrCollection::Item ) const;
#endif
private:
    TQBaseBucket **vec;
    uint	vlen;
    uint	numItems;
    uint	keytype	: 2;
    uint	cases	: 1;
    uint	copyk	: 1;
    TQGDItList  *iterators;
    void	   unlink_common( int, TQBaseBucket *, TQBaseBucket * );
    TQStringBucket *unlink_string( const TQString &,
				  TQPtrCollection::Item item = 0 );
    TQAsciiBucket  *unlink_ascii( const char *, TQPtrCollection::Item item = 0 );
    TQIntBucket    *unlink_int( long, TQPtrCollection::Item item = 0 );
    TQPtrBucket    *unlink_ptr( void *, TQPtrCollection::Item item = 0 );
    void	init( uint, KeyType, bool, bool );
    friend class TQGDictIterator;
};


class TQ_EXPORT TQGDictIterator			// generic dictionary iterator
{
friend class TQGDict;
public:
    TQGDictIterator( const TQGDict & );
    TQGDictIterator( const TQGDictIterator & );
    TQGDictIterator &operator=( const TQGDictIterator & );
   ~TQGDictIterator();

    TQPtrCollection::Item toFirst();

    TQPtrCollection::Item get()	     const;
    TQString	      getKeyString() const;
    const char	     *getKeyAscii()  const;
    long	      getKeyInt()    const;
    void	     *getKeyPtr()    const;

    TQPtrCollection::Item operator()();
    TQPtrCollection::Item operator++();
    TQPtrCollection::Item operator+=(uint);

protected:
    TQGDict	     *dict;

private:
    TQBaseBucket      *curNode;
    uint	      curIndex;
};

inline TQPtrCollection::Item TQGDictIterator::get() const
{
    return curNode ? curNode->getData() : 0;
}

inline TQString TQGDictIterator::getKeyString() const
{
    return curNode ? ((TQStringBucket*)curNode)->getKey() : TQString::null;
}

inline const char *TQGDictIterator::getKeyAscii() const
{
    return curNode ? ((TQAsciiBucket*)curNode)->getKey() : 0;
}

inline long TQGDictIterator::getKeyInt() const
{
    return curNode ? ((TQIntBucket*)curNode)->getKey() : 0;
}

inline void *TQGDictIterator::getKeyPtr() const
{
    return curNode ? ((TQPtrBucket*)curNode)->getKey() : 0;
}


#endif // TQGDICT_H
