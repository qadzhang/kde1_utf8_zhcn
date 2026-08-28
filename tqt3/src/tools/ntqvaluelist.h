/****************************************************************************
**
** Definition of TQValueList class
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

#ifndef TQVALUELIST_H
#define TQVALUELIST_H

#ifndef QT_H
#include "ntqtl.h"
#include "ntqshared.h"
#include "ntqdatastream.h"
#endif // QT_H

#ifndef TQT_NO_STL
#include <iterator>
#include <list>
#include <cstddef>
#endif

//#define QT_CHECK_VALUELIST_RANGE

template <class T>
class TQValueListNode
{
public:
    TQValueListNode( const T& t ) : data( t ) { }
    TQValueListNode() { }
#if defined(Q_TEMPLATEDLL)
    // Workaround MS bug in memory de/allocation in DLL vs. EXE
    virtual ~TQValueListNode() { }
#endif

    TQValueListNode<T>* next;
    TQValueListNode<T>* prev;
    T data;
};

template<class T>
class TQValueListIterator
{
 public:
    /**
     * Typedefs
     */
    typedef TQValueListNode<T>* NodePtr;
#ifndef TQT_NO_STL
    typedef std::bidirectional_iterator_tag  iterator_category;
#endif
    typedef T        value_type;
    typedef size_t size_type;
#ifndef TQT_NO_STL
    typedef ptrdiff_t  difference_type;
#else
    typedef int difference_type;
#endif
    typedef T*   pointer;
    typedef T& reference;

    /**
     * Variables
     */
    NodePtr node;

    /**
     * Functions
     */
    TQValueListIterator() : node( 0 ) {}
    TQValueListIterator( NodePtr p ) : node( p ) {}
    TQValueListIterator( const TQValueListIterator<T>& it ) : node( it.node ) {}

    bool operator==( const TQValueListIterator<T>& it ) const { return node == it.node; }
    bool operator!=( const TQValueListIterator<T>& it ) const { return node != it.node; }
    const T& operator*() const { return node->data; }
    T& operator*() { return node->data; }
    const T* operator->() const { return &node->data; }
    T* operator->() { return &node->data; }

    TQValueListIterator<T>& operator++() {
	node = node->next;
	return *this;
    }

    TQValueListIterator<T> operator++(int) {
	TQValueListIterator<T> tmp = *this;
	node = node->next;
	return tmp;
    }

    TQValueListIterator<T>& operator--() {
	node = node->prev;
	return *this;
    }

    TQValueListIterator<T> operator--(int) {
	TQValueListIterator<T> tmp = *this;
	node = node->prev;
	return tmp;
    }

    TQValueListIterator<T>& operator+=( int j ) {
	while ( j-- )
	    node = node->next;
	return *this;
    }

    TQValueListIterator<T>& operator-=( int j ) {
	while ( j-- )
	    node = node->prev;
	return *this;
    }

};

template<class T>
class TQValueListConstIterator
{
 public:
    /**
     * Typedefs
     */
    typedef TQValueListNode<T>* NodePtr;
#ifndef TQT_NO_STL
    typedef std::bidirectional_iterator_tag  iterator_category;
#endif
    typedef T        value_type;
    typedef size_t size_type;
#ifndef TQT_NO_STL
    typedef ptrdiff_t  difference_type;
#else
    typedef int difference_type;
#endif
    typedef const T*   pointer;
    typedef const T& reference;

    /**
     * Variables
     */
    NodePtr node;

    /**
     * Functions
     */
    TQValueListConstIterator() : node( 0 ) {}
    TQValueListConstIterator( NodePtr p ) : node( p ) {}
    TQValueListConstIterator( const TQValueListConstIterator<T>& it ) : node( it.node ) {}
    TQValueListConstIterator( const TQValueListIterator<T>& it ) : node( it.node ) {}

    bool operator==( const TQValueListConstIterator<T>& it ) const { return node == it.node; }
    bool operator!=( const TQValueListConstIterator<T>& it ) const { return node != it.node; }
    const T& operator*() const { return node->data; }
    // UDT for T = x*
    // const T* operator->() const { return &node->data; }

    TQValueListConstIterator<T>& operator++() {
	node = node->next;
	return *this;
    }

    TQValueListConstIterator<T> operator++(int) {
	TQValueListConstIterator<T> tmp = *this;
	node = node->next;
	return tmp;
    }

    TQValueListConstIterator<T>& operator--() {
	node = node->prev;
	return *this;
    }

    TQValueListConstIterator<T> operator--(int) {
	TQValueListConstIterator<T> tmp = *this;
	node = node->prev;
	return tmp;
    }
};

template <class T>
class TQValueListPrivate : public TQShared
{
public:
    /**
     * Typedefs
     */
    typedef TQValueListIterator<T> Iterator;
    typedef TQValueListConstIterator<T> ConstIterator;
    typedef TQValueListNode<T> Node;
    typedef TQValueListNode<T>* NodePtr;
    typedef size_t size_type;

    /**
     * Functions
     */
    TQValueListPrivate();
    TQValueListPrivate( const TQValueListPrivate<T>& _p );

#if defined(Q_TEMPLATEDLL)
    // Workaround MS bug in memory de/allocation in DLL vs. EXE
    virtual
#endif
    ~TQValueListPrivate();

    Iterator insert( Iterator it, const T& x );
    Iterator remove( Iterator it );
    NodePtr find( NodePtr start, const T& x ) const;
    int findIndex( NodePtr start, const T& x ) const;
    uint contains( const T& x ) const;
    bool containsYesNo( const T& x ) const;
    uint remove( const T& x );
    NodePtr at( size_type i ) const;
    void clear();

    NodePtr node;
    size_type nodes;
};

template <class T>
TQValueListPrivate<T>::TQValueListPrivate()
{
    node = new Node(); node->next = node->prev = node; nodes = 0;
}

template <class T>
TQValueListPrivate<T>::TQValueListPrivate( const TQValueListPrivate<T>& _p )
    : TQShared()
{
    node = new Node(); node->next = node->prev = node; nodes = 0;
    Iterator b( _p.node->next );
    Iterator e( _p.node );
    Iterator i( node );
    while( b != e )
	insert( i, *b++ );
}

template <class T>
TQValueListPrivate<T>::~TQValueListPrivate() {
    NodePtr p = node->next;
    while( p != node ) {
	NodePtr x = p->next;
	delete p;
	p = x;
    }
    delete node;
}

template <class T>
TQ_TYPENAME TQValueListPrivate<T>::Iterator TQValueListPrivate<T>::insert( TQ_TYPENAME TQValueListPrivate<T>::Iterator it, const T& x )
{
    NodePtr p = new Node( x );
    p->next = it.node;
    p->prev = it.node->prev;
    it.node->prev->next = p;
    it.node->prev = p;
    nodes++;
    return p;
}

template <class T>
TQ_TYPENAME TQValueListPrivate<T>::Iterator TQValueListPrivate<T>::remove( TQ_TYPENAME TQValueListPrivate<T>::Iterator it )
{
    Q_ASSERT ( it.node != node );
    NodePtr next = it.node->next;
    NodePtr prev = it.node->prev;
    prev->next = next;
    next->prev = prev;
    delete it.node;
    nodes--;
    return Iterator( next );
}

template <class T>
TQ_TYPENAME TQValueListPrivate<T>::NodePtr TQValueListPrivate<T>::find( TQ_TYPENAME TQValueListPrivate<T>::NodePtr start, const T& x ) const
{
    ConstIterator first( start );
    ConstIterator last( node );
    while( first != last) {
	if ( *first == x )
	    return first.node;
	++first;
    }
    return last.node;
}

template <class T>
int TQValueListPrivate<T>::findIndex( TQ_TYPENAME TQValueListPrivate<T>::NodePtr start, const T& x ) const
{
    ConstIterator first( start );
    ConstIterator last( node );
    int pos = 0;
    while( first != last) {
	if ( *first == x )
	    return pos;
	++first;
	++pos;
    }
    return -1;
}

template <class T>
uint TQValueListPrivate<T>::contains( const T& x ) const
{
    uint result = 0;
    Iterator first = Iterator( node->next );
    Iterator last = Iterator( node );
    while( first != last) {
	if ( *first == x )
	    ++result;
	++first;
    }
    return result;
}

template <class T>
bool TQValueListPrivate<T>::containsYesNo( const T& x ) const
{
    Iterator first = Iterator( node->next );
    Iterator last = Iterator( node );
    while( first != last) {
	if ( *first == x )
	    return true;
	++first;
    }
    return false;
}

template <class T>
uint TQValueListPrivate<T>::remove( const T& _x )
{
    const T x = _x;
    uint result = 0;
    Iterator first = Iterator( node->next );
    Iterator last = Iterator( node );
    while( first != last) {
	if ( *first == x ) {
	    first = remove( first );
	    ++result;
	} else
	    ++first;
    }
    return result;
}

template <class T>
TQ_TYPENAME TQValueListPrivate<T>::NodePtr TQValueListPrivate<T>::at( size_type i ) const
{
    Q_ASSERT( i <= nodes );
    NodePtr p = node->next;
    for( size_type x = 0; x < i; ++x )
	p = p->next;
    return p;
}

template <class T>
void TQValueListPrivate<T>::clear()
{
    nodes = 0;
    NodePtr p = node->next;
    while( p != node ) {
	NodePtr next = p->next;
	delete p;
	p = next;
    }
    node->next = node->prev = node;
}

#ifdef QT_CHECK_RANGE
# if !defined( TQT_NO_DEBUG ) && defined( QT_CHECK_VALUELIST_RANGE )
#  define QT_CHECK_INVALID_LIST_ELEMENT if ( empty() ) tqWarning( "TQValueList: Warning invalid element" )
#  define QT_CHECK_INVALID_LIST_ELEMENT_FATAL Q_ASSERT( !empty() );
# else
#  define QT_CHECK_INVALID_LIST_ELEMENT
#  define QT_CHECK_INVALID_LIST_ELEMENT_FATAL
# endif
#else
# define QT_CHECK_INVALID_LIST_ELEMENT
# define QT_CHECK_INVALID_LIST_ELEMENT_FATAL
#endif

template <class T> class TQDeepCopy;

template <class T>
class TQValueList
{
public:
    /**
     * Typedefs
     */
    typedef TQValueListIterator<T> iterator;
    typedef TQValueListConstIterator<T> const_iterator;
    typedef T value_type;
    typedef value_type* pointer;
    typedef const value_type* const_pointer;
    typedef value_type& reference;
    typedef const value_type& const_reference;
    typedef size_t size_type;
#ifndef TQT_NO_STL
    typedef ptrdiff_t  difference_type;
#else
    typedef int difference_type;
#endif

    /**
     * API
     */
    TQValueList() { sh = new TQValueListPrivate<T>; }
    TQValueList( const TQValueList<T>& l ) { sh = l.sh; sh->ref(); }
#ifndef TQT_NO_STL
    TQValueList( const std::list<T>& l )
    {
	sh = new TQValueListPrivate<T>;
	tqCopy( l.begin(), l.end(), std::back_inserter( *this ) );
    }
#endif
    ~TQValueList()
    {
        if (sh->deref())
	    delete sh;
    }

    TQValueList<T>& operator= ( const TQValueList<T>& l )
    {
        if (this == &l || sh == l.sh)
            return *this;  // Do nothing is self-assigning
	l.sh->ref();
        if (sh->deref())
	    delete sh;
	sh = l.sh;
	return *this;
    }
    
#ifndef TQT_NO_STL
    TQValueList<T>& operator= ( const std::list<T>& l )
    {
	detach();
	tqCopy( l.begin(), l.end(), std::back_inserter( *this ) );
	return *this;
    }
    
    bool operator== ( const std::list<T>& l ) const
    {
	if ( size() != l.size() )
	    return false;
	const_iterator it2 = begin();
#if !defined(Q_CC_MIPS)
	typename
#endif
	std::list<T>::const_iterator it = l.begin();
	for ( ; it2 != end(); ++it2, ++it )
	if ( !((*it2) == (*it)) )
	    return false;
	return true;
    }
#endif
    bool operator== ( const TQValueList<T>& l ) const;
    bool operator!= ( const TQValueList<T>& l ) const { return !( *this == l ); }
    iterator begin() { detach(); return iterator( sh->node->next ); }
    const_iterator begin() const { return const_iterator( sh->node->next ); }
    const_iterator constBegin() const { return const_iterator( sh->node->next ); }
    iterator end() { detach(); return iterator( sh->node ); }
    const_iterator end() const { return const_iterator( sh->node ); }
    const_iterator constEnd() const { return const_iterator( sh->node ); }
    iterator insert( iterator it, const T& x ) { detach(); return sh->insert( it, x ); }
    uint remove( const T& x ) { detach(); return sh->remove( x ); }
    void clear();

    // ### 4.0: move out of class
    TQValueList<T>& operator<< ( const T& x )
    {
	append( x );
	return *this;
    }

    size_type size() const { return sh->nodes; }
    bool empty() const { return sh->nodes == 0; }
    void push_front( const T& x ) { detach(); sh->insert( begin(), x ); }
    void push_back( const T& x ) { detach(); sh->insert( end(), x ); }
    iterator erase( iterator pos ) { detach(); return sh->remove( pos ); }
    iterator erase( iterator first, iterator last );
    reference front() { QT_CHECK_INVALID_LIST_ELEMENT_FATAL; return *begin(); }
    const_reference front() const { QT_CHECK_INVALID_LIST_ELEMENT_FATAL; return *begin(); }
    reference back() { QT_CHECK_INVALID_LIST_ELEMENT_FATAL; return *(--end()); }
    const_reference back() const { QT_CHECK_INVALID_LIST_ELEMENT_FATAL; return *(--end()); }
    void pop_front() { QT_CHECK_INVALID_LIST_ELEMENT; erase( begin() ); }
    void pop_back() {
	QT_CHECK_INVALID_LIST_ELEMENT;
	iterator tmp = end();
	erase( --tmp );
    }
    void insert( iterator pos, size_type n, const T& x );
    // Some compilers (incl. vc++) would instantiate this function even if
    // it is not used; this would constrain TQValueList to classes that provide
    // an operator<
    /*
    void sort()
    {
	qHeapSort( *this );
    }
    */

    TQValueList<T> operator+ ( const TQValueList<T>& l ) const;
    TQValueList<T>& operator+= ( const TQValueList<T>& l );

    iterator fromLast() { detach(); return iterator( sh->node->prev ); }
    const_iterator fromLast() const { return const_iterator( sh->node->prev ); }

    bool isEmpty() const { return ( sh->nodes == 0 ); }

    iterator append( const T& x ) { detach(); return sh->insert( end(), x ); }
    iterator prepend( const T& x ) { detach(); return sh->insert( begin(), x ); }

    iterator remove( iterator it ) { detach(); return sh->remove( it ); }

    T& first() { QT_CHECK_INVALID_LIST_ELEMENT; detach(); return sh->node->next->data; }
    const T& first() const { QT_CHECK_INVALID_LIST_ELEMENT; return sh->node->next->data; }
    T& last() { QT_CHECK_INVALID_LIST_ELEMENT; detach(); return sh->node->prev->data; }
    const T& last() const { QT_CHECK_INVALID_LIST_ELEMENT; return sh->node->prev->data; }

    T& operator[] ( size_type i ) { QT_CHECK_INVALID_LIST_ELEMENT; detach(); return sh->at(i)->data; }
    const T& operator[] ( size_type i ) const { QT_CHECK_INVALID_LIST_ELEMENT; return sh->at(i)->data; }
    iterator at( size_type i ) { QT_CHECK_INVALID_LIST_ELEMENT; detach(); return iterator( sh->at(i) ); }
    const_iterator at( size_type i ) const { QT_CHECK_INVALID_LIST_ELEMENT; return const_iterator( sh->at(i) ); }
    iterator find ( const T& x ) { detach(); return iterator( sh->find( sh->node->next, x) ); }
    const_iterator find ( const T& x ) const { return const_iterator( sh->find( sh->node->next, x) ); }
    iterator find ( iterator it, const T& x ) { detach(); return iterator( sh->find( it.node, x ) ); }
    const_iterator find ( const_iterator it, const T& x ) const { return const_iterator( sh->find( it.node, x ) ); }
    int findIndex( const T& x ) const { return sh->findIndex( sh->node->next, x) ; }
    size_type contains( const T& x ) const { return sh->contains( x ); }
    bool containsYesNo( const T& x ) const { return sh->containsYesNo( x ); }

    size_type count() const { return sh->nodes; }

    TQValueList<T>& operator+= ( const T& x )
    {
	append( x );
	return *this;
    }
    typedef TQValueListIterator<T> Iterator;
    typedef TQValueListConstIterator<T> ConstIterator;
    typedef T ValueType;

protected:
    /**
     * Helpers
     */
    void detach()
    { 
        if (sh->count > 1) 
        {
            sh->deref();
            sh = new TQValueListPrivate<T>(*sh);        
        }
    }

    /**
     * Variables
     */
    TQValueListPrivate<T>* sh;

private:
    friend class TQDeepCopy< TQValueList<T> >;
};

template <class T>
bool TQValueList<T>::operator== ( const TQValueList<T>& l ) const
{
    if ( size() != l.size() )
	return false;
    const_iterator it2 = begin();
    const_iterator it = l.begin();
    for( ; it != l.end(); ++it, ++it2 )
	if ( !( *it == *it2 ) )
	    return false;
    return true;
}

template <class T>
void TQValueList<T>::clear()
{
    if ( sh->count == 1 ) sh->clear(); else { sh->deref(); sh = new TQValueListPrivate<T>(); }
}

template <class T>
TQ_TYPENAME TQValueList<T>::iterator TQValueList<T>::erase( TQ_TYPENAME TQValueList<T>::iterator first, TQ_TYPENAME TQValueList<T>::iterator last )
{
    while ( first != last )
	erase( first++ );
    return last;
}


template <class T>
void TQValueList<T>::insert( TQ_TYPENAME TQValueList<T>::iterator pos, size_type n, const T& x )
{
    for ( ; n > 0; --n )
	insert( pos, x );
}

template <class T>
TQValueList<T> TQValueList<T>::operator+ ( const TQValueList<T>& l ) const
{
    TQValueList<T> l2( *this );
    for( const_iterator it = l.begin(); it != l.end(); ++it )
	l2.append( *it );
    return l2;
}

template <class T>
TQValueList<T>& TQValueList<T>::operator+= ( const TQValueList<T>& l )
{
    TQValueList<T> copy = l;
    for( const_iterator it = copy.begin(); it != copy.end(); ++it )
	append( *it );
    return *this;
}

#ifndef TQT_NO_DATASTREAM
template <class T>
TQDataStream& operator>>( TQDataStream& s, TQValueList<T>& l )
{
    l.clear();
    TQ_UINT32 c;
    s >> c;
    for( TQ_UINT32 i = 0; i < c && !s.atEnd(); ++i )
    {
	T t;
	s >> t;
	l.append( t );
    }
    return s;
}

template <class T>
TQDataStream& operator<<( TQDataStream& s, const TQValueList<T>& l )
{
    s << (TQ_UINT32)l.size();
    TQValueListConstIterator<T> it = l.begin();
    for( ; it != l.end(); ++it )
	s << *it;
    return s;
}
#endif // TQT_NO_DATASTREAM

#define Q_DEFINED_QVALUELIST
#define Q_DEFINED_QVALUELIST
#include "ntqwinexport.h"
#endif // TQVALUELIST_H
