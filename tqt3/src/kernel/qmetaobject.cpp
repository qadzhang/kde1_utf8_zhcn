/****************************************************************************
**
** Implementation of TQMetaObject class
**
** Created : 930419
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

#include "ntqmetaobject.h"
#include "ntqasciidict.h"

#ifdef TQT_THREAD_SUPPORT
#include <private/qmutexpool_p.h>
#endif // TQT_THREAD_SUPPORT

/*!
  \class TQMetaData ntqmetaobject.h
  \reentrant

  \brief The TQMetaData class provides information about a member function that is known to the meta object system.

  \internal

  The struct consists of three members, \e name, \e method  and \e access:

  \code
    const char *name;				// - member name
    const TQUMethod* method;			// - detailed method description
    enum Access { Private, Protected, Public };
    Access access;				// - access permission
  \endcode
 */

/*!
  \class TQClassInfo ntqmetaobject.h

  \brief The TQClassInfo class provides a struct that stores some basic information about a single class.

  \internal

  The class information is a simple \e name - \e value pair:

  \code
    const char* name;
    const char* value;
  \endcode

 */


/*!
    \class TQMetaObject ntqmetaobject.h
    \brief The TQMetaObject class contains meta information about TQt objects.

    \ingroup objectmodel

    The Meta Object System in TQt is responsible for the signals and
    slots inter-object communication mechanism, runtime type
    information and the property system. All meta information in TQt is
    kept in a single instance of TQMetaObject per class.

    This class is not normally required for application programming.
    But if you write meta applications, such as scripting engines or
    GUI builders, you might find these functions useful:
    \list
    \i className() to get the name of a class.
    \i superClassName() to get the name of the superclass.
    \i inherits(), the function called by TQObject::inherits().
    \i superClass() to access the superclass's meta object.
    \i numSlots(), numSignals(), slotNames(), and  signalNames() to get
	information about a class's signals and slots.
    \i property() and propertyNames() to obtain information about a
	class's properties.
    \endlist

    Classes may have a list of name-value pairs of class information.
    The number of pairs is returned by numClassInfo(), and values are
    returned by classInfo().

    \sa \link moc.html moc (Meta Object Compiler)\endlink

*/


/*****************************************************************************
  The private object.
 *****************************************************************************/

// extra flags from moc.y
enum Flags  {
    Invalid		= 0x00000000,
    Readable		= 0x00000001,
    Writable		= 0x00000002,
    EnumOrSet		= 0x00000004,
    UnresolvedEnum	= 0x00000008,
    StdSet		= 0x00000100,
    Override		= 0x00000200,
    NotDesignable	= 0x00001000,
    DesignableOverride  = 0x00002000,
    NotScriptable	= 0x00004000,
    ScriptableOverride  = 0x00008000,
    NotStored 		= 0x00010000,
    StoredOverride 	= 0x00020000
};

static TQAsciiDict<void> *tqt_metaobjects = 0;
static int tqt_metaobjects_count = 0;

class TQMetaObjectPrivate
{
public:
    TQMetaObjectPrivate() :
#ifndef TQT_NO_PROPERTIES
	enumData(0), numEnumData(0),
	propData(0),numPropData(0),
	tqt_static_property(0),
#endif
	classInfo(0), numClassInfo(0) {}
#ifndef TQT_NO_PROPERTIES
    const TQMetaEnum *enumData;
    int numEnumData;
    const TQMetaProperty *propData;
    int numPropData;
    bool (*tqt_static_property)(TQObject*, int, int, TQVariant*);
#endif
    const TQClassInfo *classInfo;
    int numClassInfo;
};


/*****************************************************************************
  Internal dictionary for fast access to class members
 *****************************************************************************/

typedef const TQMetaData TQConstMetaData;

class TQ_EXPORT TQMemberDict : public TQAsciiDict<TQConstMetaData>
{
public:
    TQMemberDict( int size = 17, bool cs = true, bool ck = true ) :
	TQAsciiDict<TQConstMetaData>(size,cs,ck) {}
    TQMemberDict( const TQMemberDict &dict ) : TQAsciiDict<TQConstMetaData>(dict) {}
    ~TQMemberDict() { clear(); }
    TQMemberDict &operator=(const TQMemberDict &dict)
    { return (TQMemberDict&)TQAsciiDict<TQConstMetaData>::operator=(dict); }
};


/*
  Calculate optimal dictionary size for n entries using prime numbers,
  and assuming there are no more than 40 entries.
*/

static int optDictSize( int n )
{
    if ( n < 6 )
	n = 5;
    else if ( n < 10 )
	n = 11;
    else if ( n < 14 )
	n = 17;
    else
	n = 23;
    return n;
}


/*****************************************************************************
  TQMetaObject member functions
 *****************************************************************************/

/*!\internal
 */
TQMetaObject::TQMetaObject( const char *const class_name, TQMetaObject *super_class,
			  const TQMetaData *const slot_data, int n_slots,
			  const TQMetaData *const signal_data, int n_signals,
#ifndef TQT_NO_PROPERTIES
			  const TQMetaProperty *const prop_data, int n_props,
			  const TQMetaEnum *const enum_data, int n_enums,
#endif
			  const TQClassInfo *const class_info, int n_info )
{
    classname = class_name;			// set meta data
    superclass = super_class;
    superclassname = superclass ? superclass->className() : 0;
    slotDict = init( slotData = slot_data, n_slots );
    signalDict = init( signalData = signal_data, n_signals );

    d = new TQMetaObjectPrivate;
    reserved = 0;

#ifndef TQT_NO_PROPERTIES
    d->propData = prop_data;
    d->numPropData = n_props;
    d->enumData = enum_data;
    d->numEnumData = n_enums;
#endif
    d->classInfo = class_info;
    d->numClassInfo = n_info;

    signaloffset = superclass ? ( superclass->signalOffset() + superclass->numSignals() ) : 0;
    slotoffset = superclass ? ( superclass->slotOffset() + superclass->numSlots() ) : 0;
#ifndef TQT_NO_PROPERTIES
    propertyoffset = superclass ? ( superclass->propertyOffset() + superclass->numProperties() ) : 0;
#endif
}

#ifndef TQT_NO_PROPERTIES
/*!\internal
 */
TQMetaObject::TQMetaObject( const char *const class_name, TQMetaObject *super_class,
			  const TQMetaData *const slot_data, int n_slots,
			  const TQMetaData *const signal_data, int n_signals,
			  const TQMetaProperty *const prop_data, int n_props,
			  const TQMetaEnum *const enum_data, int n_enums,
			  bool (*tqt_static_property)(TQObject*, int, int, TQVariant*),
			  const TQClassInfo *const class_info, int n_info )
{
    classname = class_name;			// set meta data
    superclass = super_class;
    superclassname = superclass ? superclass->className() : 0;
    slotDict = init( slotData = slot_data, n_slots );
    signalDict = init( signalData = signal_data, n_signals );

    d = new TQMetaObjectPrivate;
    reserved = 0;

    d->propData = prop_data;
    d->numPropData = n_props;
    d->enumData = enum_data;
    d->numEnumData = n_enums;
    d->tqt_static_property = tqt_static_property;
    d->classInfo = class_info;
    d->numClassInfo = n_info;

    signaloffset = superclass ? ( superclass->signalOffset() + superclass->numSignals() ) : 0;
    slotoffset = superclass ? ( superclass->slotOffset() + superclass->numSlots() ) : 0;
    propertyoffset = superclass ? ( superclass->propertyOffset() + superclass->numProperties() ) : 0;
}
#endif

/*!\internal
 */
TQMetaObject::~TQMetaObject()
{
    delete slotDict;				// delete dicts
    delete signalDict;
    delete d;
#ifdef TQT_THREAD_SUPPORT
    TQMutexLocker( tqt_global_mutexpool ?
		   tqt_global_mutexpool->get( &tqt_metaobjects ) : 0 );
#endif // TQT_THREAD_SUPPORT
    if ( tqt_metaobjects ) {
	tqt_metaobjects->remove( classname );
	if ( tqt_metaobjects->isEmpty() ) {
	    delete tqt_metaobjects;
	    tqt_metaobjects = 0;
	}
    }

    // delete reserved;				// Unused void*
}


/*!
    \fn const char *TQMetaObject::className() const

    Returns the class name.

    \sa TQObject::className(), superClassName()
*/

/*!
    \fn const char *TQMetaObject::superClassName() const

    Returns the class name of the superclass or 0 if there is no
    superclass in the TQObject hierachy.

    \sa className()
*/

/*!
    \fn TQMetaObject *TQMetaObject::superClass() const

    Returns the meta object of the super class or 0 if there is no
    such object.
*/

/*!
    Returns the number of slots for this class.

    If \a super is true, inherited slots are included.

    \sa slotNames()
*/
int TQMetaObject::numSlots( bool super ) const	// number of slots
{
    int n = slotDict ? slotDict->count() : 0;
    if ( !super || !superclass )
	return n;
    return n + superclass->numSlots( super );
}

/*!
    Returns the number of signals for this class.

    If \a super is true, inherited signals are included.

    \sa signalNames()
*/
int TQMetaObject::numSignals( bool super ) const	// number of signals
{
    int n = signalDict ? signalDict->count() : 0;
    if ( !super || !superclass )
	return n;
    return n + superclass->numSignals( super );
}


/*!  \internal

  Returns the meta data of the slot with the name \a n or 0 if no
  such slot exists.

  If  \a super is true, inherited slots are included.
 */
const TQMetaData* TQMetaObject::slot( int index, bool super ) const
{
    int idx = index - ( super ? slotOffset() : 0 );
    if ( slotDict && idx >= 0 && idx < (int) slotDict->count() ) {
	return slotData + idx;
    }
    if ( !super || !superclass )
	return 0;
    return superclass->slot( index, super );
}

/*!  \internal

  Returns the meta data of the signal with the name \a n or 0 if no
  such signal exists.

  If  \a super is true, inherited signals are included.
 */
const TQMetaData* TQMetaObject::signal( int index, bool super ) const
{
    int idx = index - ( super ? signalOffset() : 0 );
    if ( signalDict && idx >= 0 && idx < (int) signalDict->count() ) {
	return signalData + idx;
    }
    if ( !super || !superclass )
	return 0;
    return superclass->signal( index, super );
}


/*!
  \fn int TQMetaObject::signalOffset() const

  \internal

  Returns the signal offset for this metaobject.

*/

/*!
  \fn int TQMetaObject::propertyOffset() const

  \internal

  Returns the property offset for this metaobject.

*/

/*! \internal
  Returns the index of the signal with name \n or -1 if no such signal exists.

  If  \a super is true, inherited signals are included.
*/
int TQMetaObject::findSignal( const char* n, bool super ) const
{
    const TQMetaObject *mo = this;
    int offset = -1;

    do {
	const TQMetaData *md = mo->signalDict ? mo->signalDict->find( n ) : 0;
	if ( md ) {
#if defined(QT_CHECK_RANGE)
	    if ( offset != -1 ) {
		tqWarning( "TQMetaObject::findSignal:%s: Conflict with %s::%s",
			  className(), mo->className(), n );
		return offset;
	    }
#endif
	    offset = mo->signalOffset() + ( md - mo->signalData );
#if !defined(QT_CHECK_RANGE)
	    return offset;
#endif
	}
    } while ( super && (mo = mo->superclass) );

    return offset;
}

/*!
  \fn int TQMetaObject::slotOffset() const

  \internal

  Returns the slot offset for this metaobject.

*/

/*! \internal
  Returns the index of the slot with name \n or -1 if no such slot exists.

  If  \a super is true, inherited slots are included.
 */
int TQMetaObject::findSlot( const char* n, bool super ) const
{
    const TQMetaData *md = slotDict ? slotDict->find( n ) : 0;
    if ( md )
	return slotOffset() + ( md - slotData );
    if ( !super || !superclass)
	return -1;
    return superclass->findSlot( n, super );
}

/*!\internal
 */
TQMetaObject *TQMetaObject::new_metaobject( const char *classname,
					  TQMetaObject *superclassobject,
					  const TQMetaData * const slot_data, int n_slots,
					  const TQMetaData * const signal_data, int n_signals,
#ifndef TQT_NO_PROPERTIES
					  const TQMetaProperty * const prop_data, int n_props,
					  const TQMetaEnum * const enum_data, int n_enums,
#endif
					  const TQClassInfo * const class_info, int n_info )
{
    return new TQMetaObject( classname, superclassobject, slot_data, n_slots,
			    signal_data, n_signals,
#ifndef TQT_NO_PROPERTIES
			    prop_data, n_props,
			    enum_data, n_enums,
#endif
			    class_info, n_info );
}

#ifndef TQT_NO_PROPERTIES
/*!\internal
 */
TQMetaObject *TQMetaObject::new_metaobject( const char *classname,
					  TQMetaObject *superclassobject,
					  const TQMetaData * const slot_data, int n_slots,
					  const TQMetaData * const signal_data, int n_signals,
					  const TQMetaProperty * const prop_data, int n_props,
					  const TQMetaEnum * const enum_data, int n_enums,
					  bool (*tqt_static_property)(TQObject*, int, int, TQVariant*),
					  const TQClassInfo * const class_info, int n_info )
{
    return new TQMetaObject( classname, superclassobject, slot_data, n_slots,
			    signal_data, n_signals,
			    prop_data, n_props,
			    enum_data, n_enums,
			    tqt_static_property,
			    class_info, n_info );
}
#endif

/*!\internal
 */
TQMemberDict *TQMetaObject::init( const TQMetaData * data, int n )
{
    if ( n == 0 )				// nothing, then make no dict
	return 0;
    TQMemberDict *dict = new TQMemberDict( optDictSize(n), true, false );
    TQ_CHECK_PTR( dict );
    while ( n-- ) {				// put all members into dict
	dict->insert( data->name, data );
	data++;
    }
    return dict;
}

/*!
    Returns the number of items of class information available for
    this class.

    If \a super is true, inherited class information is included.
*/
int TQMetaObject::numClassInfo( bool super ) const
{
    return d->numClassInfo + ((super && superclass)?superclass->numClassInfo(super):0);
}

/*!
    Returns the class information with index \a index or 0 if no such
    information exists.

    If \a super is true,  inherited class information is included.
*/
const TQClassInfo* TQMetaObject::classInfo( int index, bool super ) const
{
    if ( index < 0 )
	return 0;
    if ( index < d->numClassInfo )
	return &(d->classInfo[ index ]);
    if ( !super || !superclass )
	return 0;
    return superclass->classInfo( index - d->numClassInfo, super );
}

/*!
    \overload
    Returns the class information with name \a name or 0 if no such
    information exists.

    If \a super is true, inherited class information is included.
*/
const char* TQMetaObject::classInfo( const char* name, bool super ) const
{
    for( int i = 0; i < d->numClassInfo; ++i ) {
	if ( qstrcmp( d->classInfo[i].name, name ) == 0 )
	    return d->classInfo[i].value;
    }
    if ( !super || !superclass )
	return 0;
    return superclass->classInfo( name, super );
}

#ifndef TQT_NO_PROPERTIES

/*!
    Returns the number of properties for this class.

    If \a super is true, inherited properties are included.

    \sa propertyNames()
 */
int TQMetaObject::numProperties( bool super ) const	// number of signals
{
    int n = d->numPropData;
    if ( !super || !superclass )
	return n;
    return n + superclass->numProperties( super );
}

/*!
    Returns the property meta data for the property at index \a index
    or 0 if no such property exists.

    If \a super is true, inherited properties are included.

    \sa propertyNames()
 */
const TQMetaProperty* TQMetaObject::property( int index, bool super ) const
{
    int idx = index - ( super ? propertyOffset() : 0 );
    if ( d->propData && idx >= 0 && idx < (int)d->numPropData )
	return d->propData + idx;
    if ( !super || !superclass )
	return 0;
    return superclass->property( index, super );
}


/*!
    Returns the index for the property with name \a name or -1 if no
    such property exists.

    If \a super is true, inherited properties are included.

    \sa property(), propertyNames()
*/

int TQMetaObject::findProperty( const char *name, bool super ) const
{
    for( int i = 0; i < d->numPropData; ++i ) {
	if ( d->propData[i].isValid() && qstrcmp( d->propData[i].name(), name ) == 0 ) {
	    return ( super ? propertyOffset() : 0 ) + i;
	}
    }
    if ( !super || !superclass )
	return -1;
    return superclass->findProperty( name, super );
}

/*! \internal

  Returns the index for the property \a prop
  or -1 if the property can not be found.

  If \a super is true, inherited properties are included.

  \sa property(), propertyNames()
*/

int TQMetaObject::indexOfProperty( const TQMetaProperty* prop, bool super ) const
{
    if ( *prop->meta == this )
	return ( super ? propertyOffset() : 0 ) + ( prop - d->propData);
    if ( !super || !superclass )
	return -1;
    return superclass->indexOfProperty( prop, super );
}

/*!\internal

  Returns the parent property of property \a p or 0, if the property
  cannot be resolved.

  \a p has to be contained in this meta object
*/

const TQMetaProperty* TQMetaObject::resolveProperty( const TQMetaProperty* p ) const
{
    if ( !superclass )
	return 0;
    return superclass->property( superclass->findProperty( p->n, true ), true );
}

/*!\internal

  \overload

  The version of resolveProperty that is used by moc generated code
*/

int TQMetaObject::resolveProperty( int index ) const
{
    if ( !superclass )
	return -1;
    const TQMetaProperty* p = d->propData + ( index - propertyOffset() );
    return superclass->findProperty( p->n, true );
}


/*!
    Returns a list with the names of all this class's properties.

    If \a super is true, inherited properties are included.

    \sa property()
*/
TQStrList TQMetaObject::propertyNames( bool super ) const
{
    TQStrList l( false );

    if ( superclass && super ) {
	TQStrList sl = superclass->propertyNames( super );
	for ( TQStrListIterator slit( sl ); slit.current(); ++slit )
	    l.append( slit.current() );
    }

    for( int i = 0; i < d->numPropData; ++i ) {
	if ( d->propData[i].isValid() )
	    l.append( d->propData[i].name() );
    }

    return l;
}

/*!
    Returns a list with the names of all this class's signals.

    If \a super is true, inherited signals are included.
*/
TQStrList TQMetaObject::signalNames( bool super ) const
{
    TQStrList l( false );
    int n = numSignals( super );
    for( int i = 0; i < n; ++i ) {
	l.append( signal(i, super)->name );
    }
    return l;
}

/*!
    Returns a list with the names of all this class's slots.

    If \a super is true, inherited slots are included.

    \sa numSlots()
*/
TQStrList TQMetaObject::slotNames( bool super ) const
{
    TQStrList l( false );
    int n = numSlots( super );
    for( int i = 0; i < n; ++i )
	l.append( slot( i, super)->name );
    return l;
}

/*!\internal

 */

int TQMetaObject::numEnumerators( bool super ) const
{
    int n = 0;
    if ( superclass && super )
	n += superclass->numEnumerators( super );
    return n + d->numEnumData;
}

/*!\internal

 */
TQStrList TQMetaObject::enumeratorNames( bool super ) const
{
    TQStrList l( false );

    if ( superclass && super ) {
	TQStrList sl = superclass->enumeratorNames( super );
	for ( TQStrListIterator slit( sl ); slit.current(); ++slit )
	    l.append( slit.current() );
    }

    for( int i = 0; i < d->numEnumData; ++i ) {
	if ( d->enumData[i].items )
	    l.append( d->enumData[i].name );
    }

    return l;
}

/*!\internal
 */
const TQMetaEnum* TQMetaObject::enumerator( const char* name, bool super ) const
{
    for( int i = 0; i < d->numEnumData; ++i )
	if ( qstrcmp( d->enumData[i].name, name ) == 0 )
	    return &(d->enumData[i]);
    if ( !super || !superclass )
	return 0;
    return superclass->enumerator( name, super );
}

#endif // TQT_NO_PROPERTIES


/*!
    Returns true if this class inherits \a clname within the meta
    object inheritance chain; otherwise returns false.

    (A class is considered to inherit itself.)
*/
bool TQMetaObject::inherits( const char* clname ) const
{
    const TQMetaObject *meta = this;
    while ( meta ) {
	if ( qstrcmp(clname, meta->className()) == 0 )
	    return true;
	meta = meta->superclass;
    }
    return false;
}

/*! \internal */

TQMetaObject *TQMetaObject::metaObject( const char *class_name )
{
    if ( !tqt_metaobjects )
	return 0;
#ifdef TQT_THREAD_SUPPORT
    TQMutexLocker( tqt_global_mutexpool ?
		   tqt_global_mutexpool->get( &tqt_metaobjects ) : 0 );
#endif // TQT_THREAD_SUPPORT
    TQtStaticMetaObjectFunction func = (TQtStaticMetaObjectFunction)tqt_metaobjects->find( class_name );
    if ( func )
	return func();
    return 0;
}

/*! \internal */
bool TQMetaObject::hasMetaObject( const char *class_name )
{
    if ( !tqt_metaobjects )
	return false;
#ifdef TQT_THREAD_SUPPORT
    TQMutexLocker( tqt_global_mutexpool ?
		   tqt_global_mutexpool->get( &tqt_metaobjects ) : 0 );
#endif // TQT_THREAD_SUPPORT
    return !!tqt_metaobjects->find( class_name );
}

#ifndef TQT_NO_PROPERTIES
/*! \internal

### this functions will go away. It exists purely for the sake of meta
### object code generated with TQt 3.1.0
*/
bool TQMetaObject::tqt_static_property( TQObject* o, int id, int f, TQVariant* v)
{
    if ( d->tqt_static_property )
	return d->tqt_static_property( o, id, f, v );
    else if ( o ) // compatibility
	return o->tqt_property( id, f, v );
    else if ( superclass )
	return superclass->tqt_static_property( o, id, f, v );
    switch ( f ) {
    case 3: case 4: case 5:
	return true;
    default:
	return false;
    }
}


/*!
    \class TQMetaProperty ntqmetaobject.h

    \brief The TQMetaProperty class stores meta data about a property.

    \ingroup objectmodel

    Property meta data includes type(), name(), and whether a property
    is writable(), designable() and stored().

    The functions isSetType(), isEnumType() and enumKeys() provide
    further information about a property's type. The conversion
    functions keyToValue(), valueToKey(), keysToValue() and
    valueToKeys() allow conversion between the integer representation
    of an enumeration or set value and its literal representation.

    Actual property values are set and received through TQObject's set
    and get functions. See TQObject::setProperty() and
    TQObject::property() for details.

    You receive meta property data through an object's meta object.
    See TQMetaObject::property() and TQMetaObject::propertyNames() for
    details.
*/

/*!
    Returns the possible enumeration keys if this property is an
    enumeration type (or a set type).

    \sa isEnumType()
*/
TQStrList TQMetaProperty::enumKeys() const
{
    TQStrList l( false );
    const TQMetaEnum* ed = enumData;
    if ( !enumData && meta )
	ed = (*meta)->enumerator( t, true );
    if ( !ed )
	return l;
    if ( ed != 0 ) {
	for( uint i = 0; i < ed->count; ++i ) {
	    uint j = 0;
	    while ( j < i &&
		    ed->items[j].value != ed->items[i].value )
		++j;
	    if ( i == j )
		l.append( ed->items[i].key );
	}
    }
    return l;
}

/*!
    Converts the enumeration key \a key to its integer value.

    For set types, use keysToValue().

    \sa valueToKey(), isSetType(), keysToValue()
*/
int TQMetaProperty::keyToValue( const char* key ) const
{
    const TQMetaEnum* ed = enumData;
    if ( !enumData && meta )
	ed = (*meta)->enumerator( t, true );
    if ( !ed )
	return -1;
    for ( uint i = 0; i < ed->count; ++i ) {
	if ( !qstrcmp( key, ed->items[i].key) )
	    return ed->items[i].value;
    }
    return -1;
}

/*!
    Converts the enumeration value \a value to its literal key.

    For set types, use valueToKeys().

    \sa valueToKey(), isSetType(), valueToKeys()
*/
const char* TQMetaProperty::valueToKey( int value ) const
{
    const TQMetaEnum* ed = enumData;
    if ( !enumData && meta )
	ed = (*meta)->enumerator( t, true );
    if ( !ed )
	return 0;
    for ( uint i = 0; i < ed->count; ++i ) {
	if ( value == ed->items[i].value )
	    return ed->items[i].key ;
    }
    return 0;
}

/*!
    Converts the list of keys \a keys to their combined (OR-ed)
    integer value.

    \sa isSetType(), valueToKey(), keysToValue()
*/
int TQMetaProperty::keysToValue( const TQStrList& keys ) const
{
    const TQMetaEnum* ed = enumData;
    if ( !enumData && meta )
	ed = (*meta)->enumerator( t, true );
    if ( !ed )
	return -1;
    int value = 0;
    for ( TQStrListIterator it( keys ); it.current(); ++it ) {
	uint i;
	for( i = ed->count; i > 0; --i ) {
	    if ( !qstrcmp( it.current(), ed->items[i-1].key) ) {
		value |= ed->items[i-1].value;
		break;
	    }
	}
	if ( i == 0 )
	    value |= -1;
    }
    return value;
}

/*!
    Converts the set value \a value to a list of keys.

    \sa isSetType(), valueToKey(), valueToKeys()
*/
TQStrList TQMetaProperty::valueToKeys( int value ) const
{
    TQStrList keys;
    const TQMetaEnum* ed = enumData;
    if ( !enumData && meta )
	ed = (*meta)->enumerator( t, true );
    if ( !ed )
	return keys;

    int v = value;
    for( uint i = ed->count; i > 0; --i ) {
	int k = ed->items[i-1].value;
	if ( ( k != 0 && (v & k) == k  ) ||  ( k == value) )  {
	    v = v & ~k;
	    keys.append( ed->items[i-1].key );
	}
    }
    return keys;
}

bool TQMetaProperty::writable() const
{
    if ( !testFlags( Override ) || testFlags( Writable ) )
	return testFlags( Writable );
    const TQMetaObject* mo = (*meta);
    const TQMetaProperty* parent = mo->resolveProperty( this );
    return parent ? parent->writable() : false;
}

/*!\internal
 */
bool TQMetaProperty::stdSet() const
{
    if ( !testFlags( Override ) || testFlags( Writable ) )
	return testFlags( StdSet );
    const TQMetaObject* mo = (*meta);
    const TQMetaProperty* parent = mo->resolveProperty( this );
    return parent ? parent->stdSet() : false;
}

/*!\internal
 */
int TQMetaProperty::id() const
{
    return _id < 0 ? (*meta)->indexOfProperty( this, true ) : _id;
}

/*! \internal
*/
void TQMetaProperty::clear()
{
    t = n = 0;
    meta = 0;
    enumData = 0;
    _id = -1;
    flags = 0;
}

bool TQMetaProperty::isValid() const
{
    if ( testFlags( UnresolvedEnum ) ) {
	if ( !enumData && (!meta || !(*meta)->enumerator( t, true ) ) )
	    return false;
    }
    if ( !testFlags( Override ) || testFlags( Readable ) )
	return testFlags( Readable );
    const TQMetaObject* mo = (*meta);
    const TQMetaProperty* parent = mo->resolveProperty( this );
    return parent ? parent->isValid() : false;
}

bool TQMetaProperty::isSetType() const
{
    const TQMetaEnum* ed = enumData;
    if ( !enumData && meta )
	ed = (*meta)->enumerator( t, true );
    return ( ed != 0 && ed->set );
}

bool TQMetaProperty::isEnumType() const
{
    return testFlags( EnumOrSet );
}



/*!
    \fn const char* TQMetaProperty::type() const

    Returns the type of the property.
*/

/*!
    \fn const char* TQMetaProperty::name() const

    Returns the name of the property.
*/

/*!
    \fn bool TQMetaProperty::writable() const

    Returns true if the property is writable; otherwise returns false.

*/

/*! \fn bool TQMetaProperty::isValid() const

  \internal

  Returns whether the property is valid.
*/

/*!
    \fn bool TQMetaProperty::isEnumType() const

    Returns true if the property's type is an enumeration value;
    otherwise returns false.

    \sa isSetType(), enumKeys()
*/

/*!
    \fn bool TQMetaProperty::isSetType() const

    Returns true if the property's type is an enumeration value that
    is used as set, i.e. if the enumeration values can be OR-ed
    together; otherwise returns false. A set type is implicitly also
    an enum type.

    \sa isEnumType(), enumKeys()
*/


/*!  Returns true if the property is designable for object \a o;
    otherwise returns false.

    If no object \a o is given, the function returns a static
    approximation.
 */
bool TQMetaProperty::designable( TQObject* o ) const
{
    if ( !isValid() || !writable() )
	return false;
    if ( o ) {
	int idx = _id >= 0 ? _id : (*meta)->indexOfProperty( this, true );
	return idx >= 0 && o->tqt_property( idx, 3, 0 );
    }
    if ( testFlags( DesignableOverride ) ) {
	const TQMetaObject* mo = (*meta);
	const TQMetaProperty* parent = mo->resolveProperty( this );
	return parent ? parent->designable() : false;
    }
    return !testFlags( NotDesignable );
}

/*!
    Returns true if the property is scriptable for object \a o;
    otherwise returns false.

    If no object \a o is given, the function returns a static
    approximation.
 */
bool TQMetaProperty::scriptable( TQObject* o ) const
{
    if ( o ) {
	int idx = _id >= 0 ? _id : (*meta)->indexOfProperty( this, true );
	return idx >= 0 && o->tqt_property( idx, 4, 0 );
    }
    if ( testFlags( ScriptableOverride ) ) {
	const TQMetaObject* mo = (*meta);
	const TQMetaProperty* parent = mo->resolveProperty( this );
	return parent ? parent->scriptable() : false;
    }
    return !testFlags( NotScriptable );
}

/*!
    Returns true if the property shall be stored for object \a o;
    otherwise returns false.

    If no object \a o is given, the function returns a static
    approximation.
 */
bool TQMetaProperty::stored( TQObject* o ) const
{
    if ( !isValid() || !writable() )
	return false;
    if ( o ) {
	int idx = _id >= 0 ? _id : (*meta)->indexOfProperty( this, true );
	return idx >= 0 && o->tqt_property( idx, 5, 0 );
    }
    if ( testFlags( StoredOverride ) ) {
	const TQMetaObject* mo = (*meta);
	const TQMetaProperty* parent = mo->resolveProperty( this );
	return parent ? parent->stored() : false;
    }
    return !testFlags( NotStored );
}


/*!
    Tries to reset the property for object \a o with a reset method.
    On success, returns true; otherwise returns false.

    Reset methods are optional, usually only a few properties support
    them.
*/
bool TQMetaProperty::reset( TQObject* o ) const
{
    if ( !o )
	return false;
    int idx = _id >= 0 ? _id : (*meta)->indexOfProperty( this, true );
    if ( idx < 0 )
	return 0;
    return o->tqt_property( idx, 2, 0 );
}


/*! \enum TQMetaProperty::Flags

  \internal
*/

#endif // TQT_NO_PROPERTIES

/*
 * TQMetaObjectCleanUp is used as static global object in the moc-generated cpp
 * files and deletes the TQMetaObject provided with setMetaObject. It sets the
 * TQObject reference to the metaObj to NULL when it is destroyed.
 */
TQMetaObjectCleanUp::TQMetaObjectCleanUp( const char *mo_name, TQtStaticMetaObjectFunction func )
    : metaObject( 0 )
{
#ifdef TQT_THREAD_SUPPORT
    TQMutexLocker( tqt_global_mutexpool ?
		   tqt_global_mutexpool->get( &tqt_metaobjects ) : 0 );
#endif // TQT_THREAD_SUPPORT
    if ( !tqt_metaobjects )
	tqt_metaobjects = new TQAsciiDict<void>( 257 );
    tqt_metaobjects->insert( mo_name, (void*)func );

    tqt_metaobjects_count++;
}

TQMetaObjectCleanUp::TQMetaObjectCleanUp()
    : metaObject( 0 )
{
}

/*!    \fn bool TQMetaProperty::testFlags( uint f ) const
    \internal
*/

TQMetaObjectCleanUp::~TQMetaObjectCleanUp()
{
#ifdef TQT_THREAD_SUPPORT
    TQMutexLocker( tqt_global_mutexpool ?
		  tqt_global_mutexpool->get( &tqt_metaobjects ) : 0 );
#endif // TQT_THREAD_SUPPORT
    if ( !--tqt_metaobjects_count ) {
	delete tqt_metaobjects;
	tqt_metaobjects = 0;
    }
    if ( metaObject ) {
	delete *metaObject;
	*metaObject = 0;
	metaObject = 0;
    }
}

void TQMetaObjectCleanUp::setMetaObject( TQMetaObject *&mo )
{
#if defined(QT_CHECK_RANGE)
    if (metaObject && metaObject != &mo)
	tqWarning( "TQMetaObjectCleanUp::setMetaObject: Double use of TQMetaObjectCleanUp!" );
#endif
    metaObject = &mo;
}
