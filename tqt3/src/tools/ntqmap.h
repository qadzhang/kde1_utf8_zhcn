/****************************************************************************
**
** Definition of TQMap class
**
** Created : 990406
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

#ifndef TQMAP_H
#define TQMAP_H

#ifndef QT_H
#include "ntqglobal.h"
#include "ntqshared.h"
#include "ntqdatastream.h"
#include "ntqpair.h"
#include "ntqvaluelist.h"
#endif // QT_H

#ifndef TQT_NO_STL
#include <iterator>
#include <map>
#include <cstddef>
#endif

//#define QT_CHECK_MAP_RANGE

struct TQ_EXPORT TQMapNodeBase
{
    enum Color { Red, Black };

    TQMapNodeBase* left;
    TQMapNodeBase* right;
    TQMapNodeBase* parent;

    Color color;

    TQMapNodeBase* minimum() {
	TQMapNodeBase* x = this;
	while ( x->left )
	    x = x->left;
	return x;
    }

    TQMapNodeBase* maximum() {
	TQMapNodeBase* x = this;
	while ( x->right )
	    x = x->right;
	return x;
    }
};


template <class K, class T>
struct TQMapNode : public TQMapNodeBase
{
    TQMapNode( const K& _key, const T& _data ) { data = _data; key = _key; }
    TQMapNode( const K& _key )	   { key = _key; }
    TQMapNode( const TQMapNode<K,T>& _n ) { key = _n.key; data = _n.data; }
    TQMapNode() { }
    T data;
    K key;
};


template<class K, class T>
class TQMapIterator
{
 public:
    /**
     * Typedefs
     */
    typedef TQMapNode< K, T >* NodePtr;
#ifndef TQT_NO_STL
    typedef std::bidirectional_iterator_tag  iterator_category;
#endif
    typedef T          value_type;
#ifndef TQT_NO_STL
    typedef ptrdiff_t  difference_type;
#else
    typedef int difference_type;
#endif
    typedef T*         pointer;
    typedef T&         reference;

    /**
     * Variables
     */
    TQMapNode<K,T>* node;

    /**
     * Functions
     */
    TQMapIterator() : node( 0 ) {}
    TQMapIterator( TQMapNode<K,T>* p ) : node( p ) {}
    TQMapIterator( const TQMapIterator<K,T>& it ) : node( it.node ) {}

    bool operator==( const TQMapIterator<K,T>& it ) const { return node == it.node; }
    bool operator!=( const TQMapIterator<K,T>& it ) const { return node != it.node; }
    T& operator*() { return node->data; }
    const T& operator*() const { return node->data; }
    // UDT for T = x*
    // T* operator->() const { return &node->data; }

    const K& key() const { return node->key; }
    T& data() { return node->data; }
    const T& data() const { return node->data; }

private:
    int inc();
    int dec();

public:
    TQMapIterator<K,T>& operator++() {
	inc();
	return *this;
    }

    TQMapIterator<K,T> operator++(int) {
	TQMapIterator<K,T> tmp = *this;
	inc();
	return tmp;
    }

    TQMapIterator<K,T>& operator--() {
	dec();
	return *this;
    }

    TQMapIterator<K,T> operator--(int) {
	TQMapIterator<K,T> tmp = *this;
	dec();
	return tmp;
    }
};

template <class K, class T>
int TQMapIterator<K,T>::inc()
{
    TQMapNodeBase* tmp = node;
    if ( tmp->right ) {
	tmp = tmp->right;
	while ( tmp->left )
	    tmp = tmp->left;
    } else {
	TQMapNodeBase* y = tmp->parent;
	while (tmp == y->right) {
	    tmp = y;
	    y = y->parent;
	}
	if (tmp->right != y)
	    tmp = y;
    }
    node = (NodePtr)tmp;
    return 0;
}

template <class K, class T>
int TQMapIterator<K,T>::dec()
{
    TQMapNodeBase* tmp = node;
    if (tmp->color == TQMapNodeBase::Red &&
	tmp->parent->parent == tmp ) {
	tmp = tmp->right;
    } else if (tmp->left != 0) {
	TQMapNodeBase* y = tmp->left;
	while ( y->right )
	    y = y->right;
	tmp = y;
    } else {
	TQMapNodeBase* y = tmp->parent;
	while (tmp == y->left) {
	    tmp = y;
	    y = y->parent;
	}
	tmp = y;
    }
    node = (NodePtr)tmp;
    return 0;
}

template<class K, class T>
class TQMapConstIterator
{
 public:
    /**
     * Typedefs
     */
    typedef TQMapNode< K, T >* NodePtr;
#ifndef TQT_NO_STL
    typedef std::bidirectional_iterator_tag  iterator_category;
#endif
    typedef T          value_type;
#ifndef TQT_NO_STL
    typedef ptrdiff_t  difference_type;
#else
    typedef int difference_type;
#endif
    typedef const T*   pointer;
    typedef const T&   reference;


    /**
     * Variables
     */
    TQMapNode<K,T>* node;

    /**
     * Functions
     */
    TQMapConstIterator() : node( 0 ) {}
    TQMapConstIterator( TQMapNode<K,T>* p ) : node( p ) {}
    TQMapConstIterator( const TQMapConstIterator<K,T>& it ) : node( it.node ) {}
    TQMapConstIterator( const TQMapIterator<K,T>& it ) : node( it.node ) {}

    bool operator==( const TQMapConstIterator<K,T>& it ) const { return node == it.node; }
    bool operator!=( const TQMapConstIterator<K,T>& it ) const { return node != it.node; }
    const T& operator*()  const { return node->data; }
    // UDT for T = x*
    // const T* operator->() const { return &node->data; }

    const K& key() const { return node->key; }
    const T& data() const { return node->data; }

private:
    int inc();
    int dec();

public:
    TQMapConstIterator<K,T>& operator++() {
	inc();
	return *this;
    }

    TQMapConstIterator<K,T> operator++(int) {
	TQMapConstIterator<K,T> tmp = *this;
	inc();
	return tmp;
    }

    TQMapConstIterator<K,T>& operator--() {
	dec();
	return *this;
    }

    TQMapConstIterator<K,T> operator--(int) {
	TQMapConstIterator<K,T> tmp = *this;
	dec();
	return tmp;
    }
};

template <class K, class T>
int TQMapConstIterator<K,T>::inc()
{
    TQMapNodeBase* tmp = node;
    if ( tmp->right ) {
	tmp = tmp->right;
	while ( tmp->left )
	    tmp = tmp->left;
    } else {
	TQMapNodeBase* y = tmp->parent;
	while (tmp == y->right) {
	    tmp = y;
	    y = y->parent;
	}
	if (tmp->right != y)
	    tmp = y;
    }
    node = (NodePtr)tmp;
    return 0;
}

template <class K, class T>
int TQMapConstIterator<K,T>::dec()
{
    TQMapNodeBase* tmp = node;
    if (tmp->color == TQMapNodeBase::Red &&
	tmp->parent->parent == tmp ) {
	tmp = tmp->right;
    } else if (tmp->left != 0) {
	TQMapNodeBase* y = tmp->left;
	while ( y->right )
	    y = y->right;
	tmp = y;
    } else {
	TQMapNodeBase* y = tmp->parent;
	while (tmp == y->left) {
	    tmp = y;
	    y = y->parent;
	}
	tmp = y;
    }
    node = (NodePtr)tmp;
    return 0;
}

// ### 4.0: rename to something without Private in it. Not really internal.
class TQ_EXPORT TQMapPrivateBase : public TQShared
{
public:
    TQMapPrivateBase() {
	node_count = 0;
    }
    TQMapPrivateBase( const TQMapPrivateBase* _map) {
	node_count = _map->node_count;
    }

    /**
     * Implementations of basic tree algorithms
     */
    void rotateLeft( TQMapNodeBase* x, TQMapNodeBase*& root);
    void rotateRight( TQMapNodeBase* x, TQMapNodeBase*& root );
    void rebalance( TQMapNodeBase* x, TQMapNodeBase*& root );
    TQMapNodeBase* removeAndRebalance( TQMapNodeBase* z, TQMapNodeBase*& root,
				      TQMapNodeBase*& leftmost,
				      TQMapNodeBase*& rightmost );

    /**
     * Variables
     */
    int node_count;
};


template <class Key, class T>
class TQMapPrivate : public TQMapPrivateBase
{
public:
    /**
     * Typedefs
     */
    typedef TQMapIterator< Key, T > Iterator;
    typedef TQMapConstIterator< Key, T > ConstIterator;
    typedef TQMapNode< Key, T > Node;
    typedef TQMapNode< Key, T >* NodePtr;

    /**
     * Functions
     */
    TQMapPrivate();
    TQMapPrivate( const TQMapPrivate< Key, T >* _map );
    ~TQMapPrivate() { clear(); delete header; }

    NodePtr copy( NodePtr p );
    void clear();
    void clear( NodePtr p );

    Iterator begin()	{ return Iterator( (NodePtr)(header->left ) ); }
    Iterator end()	{ return Iterator( header ); }
    ConstIterator begin() const { return ConstIterator( (NodePtr)(header->left ) ); }
    ConstIterator end() const { return ConstIterator( header ); }

    ConstIterator find(const Key& k) const;

    void remove( Iterator it ) {
	NodePtr del = (NodePtr) removeAndRebalance( it.node, header->parent, header->left, header->right );
	delete del;
	--node_count;
    }

#ifdef QT_QMAP_DEBUG
    void inorder( TQMapNodeBase* x = 0, int level = 0 ){
	if ( !x )
	    x = header->parent;
	if ( x->left )
	    inorder( x->left, level + 1 );
    //cout << level << " Key=" << key(x) << " Value=" << ((NodePtr)x)->data << endl;
	if ( x->right )
	    inorder( x->right, level + 1 );
    }
#endif

#if 0
    Iterator insertMulti(const Key& v){
	TQMapNodeBase* y = header;
	TQMapNodeBase* x = header->parent;
	while (x != 0){
	    y = x;
	    x = ( v < key(x) ) ? x->left : x->right;
	}
	return insert(x, y, v);
    }
#endif

    Iterator insertSingle( const Key& k );
    Iterator insert( TQMapNodeBase* x, TQMapNodeBase* y, const Key& k );

protected:
    /**
     * Helpers
     */
    const Key& key( TQMapNodeBase* b ) const { return ((NodePtr)b)->key; }

    /**
     * Variables
     */
    NodePtr header;
};


template <class Key, class T>
TQMapPrivate<Key,T>::TQMapPrivate() {
    header = new Node;
    header->color = TQMapNodeBase::Red; // Mark the header
    header->parent = 0;
    header->left = header->right = header;
}
template <class Key, class T>
TQMapPrivate<Key,T>::TQMapPrivate( const TQMapPrivate< Key, T >* _map ) : TQMapPrivateBase( _map ) {
    header = new Node;
    header->color = TQMapNodeBase::Red; // Mark the header
    if ( _map->header->parent == 0 ) {
	header->parent = 0;
	header->left = header->right = header;
    } else {
	header->parent = copy( (NodePtr)(_map->header->parent) );
	header->parent->parent = header;
	header->left = header->parent->minimum();
	header->right = header->parent->maximum();
    }
}

template <class Key, class T>
TQ_TYPENAME TQMapPrivate<Key,T>::NodePtr TQMapPrivate<Key,T>::copy( TQ_TYPENAME TQMapPrivate<Key,T>::NodePtr p )
{
    if ( !p )
	return 0;
    NodePtr n = new Node( *p );
    n->color = p->color;
    if ( p->left ) {
	n->left = copy( (NodePtr)(p->left) );
	n->left->parent = n;
    } else {
	n->left = 0;
    }
    if ( p->right ) {
	n->right = copy( (NodePtr)(p->right) );
	n->right->parent = n;
    } else {
	n->right = 0;
    }
    return n;
}

template <class Key, class T>
void TQMapPrivate<Key,T>::clear()
{
    clear( (NodePtr)(header->parent) );
    header->color = TQMapNodeBase::Red;
    header->parent = 0;
    header->left = header->right = header;
    node_count = 0;
}

template <class Key, class T>
void TQMapPrivate<Key,T>::clear( TQ_TYPENAME TQMapPrivate<Key,T>::NodePtr p )
{
    while ( p != 0 ) {
	clear( (NodePtr)p->right );
	NodePtr y = (NodePtr)p->left;
	delete p;
	p = y;
    }
}

template <class Key, class T>
TQ_TYPENAME TQMapPrivate<Key,T>::ConstIterator TQMapPrivate<Key,T>::find(const Key& k) const
{
    TQMapNodeBase* y = header;        // Last node
    TQMapNodeBase* x = header->parent; // Root node.

    while ( x != 0 ) {
	// If as k <= key(x) go left
	if ( !( key(x) < k ) ) {
	    y = x;
	    x = x->left;
	} else {
	    x = x->right;
	}
    }

    // Was k bigger/smaller then the biggest/smallest
    // element of the tree ? Return end()
    if ( y == header || k < key(y) )
	return ConstIterator( header );
    return ConstIterator( (NodePtr)y );
}

template <class Key, class T>
TQ_TYPENAME TQMapPrivate<Key,T>::Iterator TQMapPrivate<Key,T>::insertSingle( const Key& k )
{
    // Search correct position in the tree
    TQMapNodeBase* y = header;
    TQMapNodeBase* x = header->parent;
    bool result = true;
    while ( x != 0 ) {
	result = ( k < key(x) );
	y = x;
	x = result ? x->left : x->right;
    }
    // Get iterator on the last not empty one
    Iterator j( (NodePtr)y );
    if ( result ) {
	// Smaller then the leftmost one ?
	if ( j == begin() ) {
	    return insert(x, y, k );
	} else {
	    // Perhaps daddy is the right one ?
	    --j;
	}
    }
    // Really bigger ?
    if ( (j.node->key) < k )
	return insert(x, y, k );
    // We are going to replace a node
    return j;
}


template <class Key, class T>
TQ_TYPENAME TQMapPrivate<Key,T>::Iterator TQMapPrivate<Key,T>::insert( TQMapNodeBase* x, TQMapNodeBase* y, const Key& k )
{
    NodePtr z = new Node( k );
    if (y == header || x != 0 || k < key(y) ) {
	y->left = z;                // also makes leftmost = z when y == header
	if ( y == header ) {
	    header->parent = z;
	    header->right = z;
	} else if ( y == header->left )
	    header->left = z;           // maintain leftmost pointing to min node
    } else {
	y->right = z;
	if ( y == header->right )
	    header->right = z;          // maintain rightmost pointing to max node
    }
    z->parent = y;
    z->left = 0;
    z->right = 0;
    rebalance( z, header->parent );
    ++node_count;
    return Iterator(z);
}


#ifdef QT_CHECK_RANGE
# if !defined( TQT_NO_DEBUG ) && defined( QT_CHECK_MAP_RANGE )
#  define TQT_CHECK_INVALID_MAP_ELEMENT if ( empty() ) tqWarning( "TQMap: Warning invalid element" )
#  define TQT_CHECK_INVALID_MAP_ELEMENT_FATAL Q_ASSERT( !empty() );
# else
#  define TQT_CHECK_INVALID_MAP_ELEMENT
#  define TQT_CHECK_INVALID_MAP_ELEMENT_FATAL
# endif
#else
# define TQT_CHECK_INVALID_MAP_ELEMENT
# define TQT_CHECK_INVALID_MAP_ELEMENT_FATAL
#endif

template <class T> class TQDeepCopy;

template<class Key, class T>
class TQMap
{
public:
    /**
     * Typedefs
     */
    typedef Key key_type;
    typedef T mapped_type;
    typedef TQPair<const key_type, mapped_type> value_type;
    typedef value_type* pointer;
    typedef const value_type* const_pointer;
    typedef value_type& reference;
    typedef const value_type& const_reference;
#ifndef TQT_NO_STL
    typedef ptrdiff_t  difference_type;
#else
    typedef int difference_type;
#endif
    typedef size_t      size_type;
    typedef TQMapIterator<Key,T> iterator;
    typedef TQMapConstIterator<Key,T> const_iterator;
    typedef TQPair<iterator,bool> insert_pair;

    typedef TQMapIterator< Key, T > Iterator;
    typedef TQMapConstIterator< Key, T > ConstIterator;
    typedef T ValueType;
    typedef TQMapPrivate< Key, T > Priv;

    /**
     * API
     */
    TQMap()
    {
	sh = new TQMapPrivate< Key, T >;
    }
    TQMap( const TQMap<Key,T>& m )
    {
	sh = m.sh; sh->ref();
    }

#ifndef TQT_NO_STL
    TQMap( const std::map<Key,T>& m )
    {
	sh = new TQMapPrivate<Key,T>;
	TQ_TYPENAME std::map<Key,T>::const_iterator it = m.begin();
	for ( ; it != m.end(); ++it ) {
	    value_type p( (*it).first, (*it).second );
	    insert( p );
	}
    }
#endif
    ~TQMap()
    {
	if ( sh ) {
	    if ( sh->deref() ) {
		delete sh;
		sh = 0L;
	    }
	}
    }
    TQMap<Key,T>& operator= ( const TQMap<Key,T>& m );
#ifndef TQT_NO_STL
    TQMap<Key,T>& operator= ( const std::map<Key,T>& m )
    {
	clear();
	TQ_TYPENAME std::map<Key,T>::const_iterator it = m.begin();
	for ( ; it != m.end(); ++it ) {
	    value_type p( (*it).first, (*it).second );
	    insert( p );
	}
	return *this;
    }
#endif

    iterator begin() { detach(); return sh->begin(); }
    iterator end() { detach(); return sh->end(); }
    const_iterator begin() const { return ((const Priv*)sh)->begin(); }
    const_iterator end() const { return ((const Priv*)sh)->end(); }
    const_iterator constBegin() const { return begin(); }
    const_iterator constEnd() const { return end(); }

    iterator replace( const Key& k, const T& v )
    {
	remove( k );
	return insert( k, v );
    }

    size_type size() const
    {
	return sh->node_count;
    }
    bool empty() const
    {
	return sh->node_count == 0;
    }
    TQPair<iterator,bool> insert( const value_type& x );

    void erase( iterator it )
    {
	detach();
	sh->remove( it );
    }
    void erase( const key_type& k );
    size_type count( const key_type& k ) const;
    T& operator[] ( const Key& k );
    void clear();

    iterator find ( const Key& k )
    {
	detach();
	return iterator( sh->find( k ).node );
    }
    const_iterator find ( const Key& k ) const {	return sh->find( k ); }

    const T& operator[] ( const Key& k ) const
	{ TQT_CHECK_INVALID_MAP_ELEMENT; return sh->find( k ).data(); }
    bool contains ( const Key& k ) const
	{ return find( k ) != end(); }
	//{ return sh->find( k ) != ((const Priv*)sh)->end(); }

    size_type count() const { return sh->node_count; }

    TQValueList<Key> keys() const {
	TQValueList<Key> r;
	for (const_iterator i=begin(); i!=end(); ++i)
	    r.append(i.key());
	return r;
    }

    TQValueList<T> values() const {
	TQValueList<T> r;
	for (const_iterator i=begin(); i!=end(); ++i)
	    r.append(*i);
	return r;
    }

    bool isEmpty() const { return sh->node_count == 0; }

    iterator insert( const Key& key, const T& value, bool overwrite = true );
    void remove( iterator it ) { detach(); sh->remove( it ); }
    void remove( const Key& k );

protected:
    /**
     * Helpers
     */
    void detach() {  if ( sh->count > 1 ) detachInternal(); }

    Priv* sh;
private:
    void detachInternal();

    friend class TQDeepCopy< TQMap<Key,T> >;
};

template<class Key, class T>
TQMap<Key,T>& TQMap<Key,T>::operator= ( const TQMap<Key,T>& m )
{
    m.sh->ref();
    if ( sh->deref() )
	delete sh;
    sh = m.sh;
    return *this;
}

template<class Key, class T>
TQ_TYPENAME TQMap<Key,T>::insert_pair TQMap<Key,T>::insert( const TQ_TYPENAME TQMap<Key,T>::value_type& x )
{
    detach();
    size_type n = size();
    iterator it = sh->insertSingle( x.first );
    bool inserted = false;
    if ( n < size() ) {
	inserted = true;
	it.data() = x.second;
    }
    return TQPair<iterator,bool>( it, inserted );
}

template<class Key, class T>
void TQMap<Key,T>::erase( const Key& k )
{
    detach();
    iterator it( sh->find( k ).node );
    if ( it != end() )
	sh->remove( it );
}

template<class Key, class T>
TQ_TYPENAME TQMap<Key,T>::size_type TQMap<Key,T>::count( const Key& k ) const
{
    const_iterator it( sh->find( k ).node );
    if ( it != end() ) {
	size_type c = 0;
	while ( it != end() ) {
	    ++it;
	    ++c;
	}
	return c;
    }
    return 0;
}

template<class Key, class T>
T& TQMap<Key,T>::operator[] ( const Key& k )
{
    detach();
    TQMapNode<Key,T>* p = sh->find( k ).node;
    if ( p != sh->end().node )
	return p->data;
    return insert( k, T() ).data();
}

template<class Key, class T>
void TQMap<Key,T>::clear()
{
    if ( sh->count == 1 )
	sh->clear();
    else {
	sh->deref();
	sh = new TQMapPrivate<Key,T>;
    }
}

template<class Key, class T>
TQ_TYPENAME TQMap<Key,T>::iterator TQMap<Key,T>::insert( const Key& key, const T& value, bool overwrite )
{
    detach();
    size_type n = size();
    iterator it = sh->insertSingle( key );
    if ( overwrite || n < size() )
	it.data() = value;
    return it;
}

template<class Key, class T>
void TQMap<Key,T>::remove( const Key& k )
{
    detach();
    iterator it( sh->find( k ).node );
    if ( it != end() )
	sh->remove( it );
}

template<class Key, class T>
void TQMap<Key,T>::detachInternal()
{
    sh->deref(); sh = new TQMapPrivate<Key,T>( sh );
}


#ifndef TQT_NO_DATASTREAM
template<class Key, class T>
TQDataStream& operator>>( TQDataStream& s, TQMap<Key,T>& m ) {
    m.clear();
    TQ_UINT32 c;
    s >> c;
    for( TQ_UINT32 i = 0; i < c; ++i ) {
	Key k; T t;
	s >> k >> t;
	m.insert( k, t );
	if ( s.atEnd() )
	    break;
    }
    return s;
}


template<class Key, class T>
TQDataStream& operator<<( TQDataStream& s, const TQMap<Key,T>& m ) {
    s << (TQ_UINT32)m.size();
    TQMapConstIterator<Key,T> it = m.begin();
    for( ; it != m.end(); ++it )
	s << it.key() << it.data();
    return s;
}
#endif

#define Q_DEFINED_QMAP
#include "ntqwinexport.h"
#endif // TQMAP_H
