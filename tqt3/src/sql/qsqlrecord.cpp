/****************************************************************************
**
** Implementation of TQSqlRecord class
**
** Created : 2000-11-03
**
** Copyright (C) 2005-2008 Trolltech ASA.  All rights reserved.
**
** This file is part of the sql module of the TQt GUI Toolkit.
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

#include "ntqsqlrecord.h"

#ifndef TQT_NO_SQL

#include "ntqregexp.h"
#include "ntqvaluevector.h"
#include "ntqshared.h"
#include "ntqnamespace.h"

class TQSqlRecordPrivate
{
public:
    class info {
    public:
	info() : nogen(false){}
	~info() {}
	info( const info& other )
	    : field( other.field ), nogen( other.nogen )
	{
	}
	info& operator=(const info& other)
	{
	    field = other.field;
	    nogen = other.nogen;
	    return *this;
	}
	bool isValid() const
	{
	    return !field.name().isNull();
	}
	TQSqlField field;
	bool    nogen;
    };

    TQSqlRecordPrivate(): cnt(0) 
    {
    }
    TQSqlRecordPrivate( const TQSqlRecordPrivate& other )
    {
	*this = other;
    }
    ~TQSqlRecordPrivate() {};
    TQSqlRecordPrivate& operator=( const TQSqlRecordPrivate& other )
    {
	fi = other.fi;
	cnt = other.cnt;
	return *this;
    }
    void append( const TQSqlField& field )
    {
	info i;
	i.field = field;
	fi.append( i );
	cnt++;
    }
    void insert( int pos, const TQSqlField& field )
    {
	info i;
	i.field = field;
	if ( pos == (int)fi.size() )
	    append( field );
	if ( pos > (int)fi.size() ) {
	    fi.resize( pos + 1 );
	    cnt++;
	}
	fi[ pos ] = i;
    }
    void remove( int i )
    {
	info inf;
	if ( i >= (int)fi.count() )
	    return;
	if ( fi[ i ].isValid() )
	    cnt--;
	fi[ i ] = inf;
	// clean up some memory
	while ( fi.count() && !fi.back().isValid() )
	    fi.pop_back();
    }
    void clear()
    {
	fi.clear();
	cnt = 0;
    }
    bool isEmpty()
    {
	return cnt == 0;
    }
    info* fieldInfo( int i )
    {
	if ( i < (int)fi.count() )
	    return &fi[i];
	return 0;
    }
    uint count() const
    {
	return cnt;
    }
    bool contains( int i ) const
    {
	return i >= 0 && i < (int)fi.count() && fi[ i ].isValid();
    }
private:
    TQValueVector< info > fi;
    uint		 cnt;
};

TQSqlRecordShared::~TQSqlRecordShared()
{
    if ( d )
	delete d;
}

/*!
    \class TQSqlRecord ntqsqlfield.h
    \brief The TQSqlRecord class encapsulates a database record, i.e. a
    set of database fields.

    \ingroup database
    \module sql

    The TQSqlRecord class encapsulates the functionality and
    characteristics of a database record (usually a table or view within
    the database). TQSqlRecords support adding and removing fields as
    well as setting and retrieving field values.

    TQSqlRecord is implicitly shared. This means you can make copies of
    the record in time O(1). If multiple TQSqlRecord instances share
    the same data and one is modifying the record's data then this
    modifying instance makes a copy and modifies its private copy -
    thus it does not affect other instances.

    \sa TQSqlRecordInfo
*/


/*!
    Constructs an empty record.
*/

TQSqlRecord::TQSqlRecord()
{
    sh = new TQSqlRecordShared( new TQSqlRecordPrivate() );
}

/*!
    Constructs a copy of \a other.
*/

TQSqlRecord::TQSqlRecord( const TQSqlRecord& other )
    : sh( other.sh )
{
    sh->ref();
}

/*!
    Sets the record equal to \a other.
*/

TQSqlRecord& TQSqlRecord::operator=( const TQSqlRecord& other )
{
    other.sh->ref();
    deref();
    sh = other.sh;
    return *this;
}

/*! \internal
*/

void TQSqlRecord::deref()
{
    if ( sh->deref() ) {
	delete sh;
	sh = 0;
    }
}

/*! \internal
*/

bool TQSqlRecord::checkDetach()
{
    if ( sh->count > 1 ) {
	sh->deref();
	sh = new TQSqlRecordShared( new TQSqlRecordPrivate( *sh->d ) );
	return true;
    }
    return false;
}

/*!
    Destroys the object and frees any allocated resources.
*/

TQSqlRecord::~TQSqlRecord()
{
    deref();
}

/*!
    Returns the value of the field located at position \a i in the
    record. If field \a i does not exist the resultant behaviour is
    undefined.

    This function should be used with \l{TQSqlQuery}s. When working
    with a TQSqlCursor the \link TQSqlCursor::value() value(const
    TQString&)\endlink overload which uses field names is more
    appropriate.
*/

TQVariant TQSqlRecord::value( int i ) const
{
    const TQSqlField * f = field(i);

    if( f )
	return f->value();
    return TQVariant();
}

/*!
    \overload

    Returns the value of the field called \a name in the record. If
    field \a name does not exist the resultant behaviour is undefined.
*/

TQVariant  TQSqlRecord::value( const TQString& name ) const
{
    const TQSqlField * f = field( name );

    if( f )
	return f->value();
    return TQVariant();
}

/*!
    Returns the name of the field at position \a i. If the field does
    not exist, TQString::null is returned.
*/

TQString TQSqlRecord::fieldName( int i ) const
{
    const TQSqlField* f = field( i );
    if ( f )
	return f->name();
    return TQString::null;
}

/*!
    Returns the position of the field called \a name within the
    record, or -1 if it cannot be found. Field names are not
    case-sensitive. If more than one field matches, the first one is
    returned.
*/

int TQSqlRecord::position( const TQString& name ) const
{
    for ( uint i = 0; i < count(); ++i ) {
	if ( fieldName(i).upper() == name.upper() )
	    return i;
    }
#ifdef QT_CHECK_RANGE
    tqWarning( "TQSqlRecord::position: unable to find field %s", name.latin1() );
#endif
    return -1;
}

/*!
    Returns the field at position \a i within the record, or 0 if it
    cannot be found.
*/

TQSqlField* TQSqlRecord::field( int i )
{
    checkDetach();
    if ( !sh->d->contains( i ) ) {
#ifdef QT_CHECK_RANGE
	tqWarning( "TQSqlRecord::field: index out of range: %d", i );
#endif
	return 0;
    }
    return &sh->d->fieldInfo( i )->field;
}

/*!
    \overload

    Returns the field called \a name within the record, or 0 if it
    cannot be found. Field names are not case-sensitive.
*/

TQSqlField* TQSqlRecord::field( const TQString& name )
{
    checkDetach();
    if ( !sh->d->contains( position( name ) ) )
	return 0;
    return &sh->d->fieldInfo( position( name ) )->field;
}


/*!
    \overload
*/

const TQSqlField* TQSqlRecord::field( int i ) const
{
    if ( !sh->d->contains( i ) ) {
#ifdef QT_CHECK_RANGE
	tqWarning( "TQSqlRecord::field: index out of range: %d", i  );
#endif // QT_CHECK_RANGE
	return 0;
    }
    return &sh->d->fieldInfo( i )->field;
}

/*!
    \overload

    Returns the field called \a name within the record, or 0 if it
    cannot be found. Field names are not case-sensitive.
*/

const TQSqlField* TQSqlRecord::field( const TQString& name ) const
{
    if( !sh->d->contains( position( name ) ) )
	return 0;
    return &sh->d->fieldInfo( position( name ) )->field;
}

/*!
    Append a copy of field \a field to the end of the record.
*/

void TQSqlRecord::append( const TQSqlField& field )
{
    checkDetach();
    sh->d->append( field );
}

/*!
    Insert a copy of \a field at position \a pos. If a field already
    exists at \a pos, it is removed.
*/

void TQSqlRecord::insert( int pos, const TQSqlField& field ) // ### 4.0: rename to ::replace
{
    checkDetach();
    sh->d->insert( pos, field );
}

/*!
    Removes the field at \a pos. If \a pos does not exist, nothing
    happens.
*/

void TQSqlRecord::remove( int pos )
{
    checkDetach();
    sh->d->remove( pos );
}

/*!
    Removes all the record's fields.

    \sa clearValues()
*/

void TQSqlRecord::clear()
{
    checkDetach();
    sh->d->clear();
}

/*!
    Returns true if there are no fields in the record; otherwise
    returns false.
*/

bool TQSqlRecord::isEmpty() const
{
    return sh->d->isEmpty();
}


/*!
    Returns true if there is a field in the record called \a name;
    otherwise returns false.
*/

bool TQSqlRecord::contains( const TQString& name ) const
{
    for ( uint i = 0; i < count(); ++i ) {
	if ( fieldName(i).upper() == name.upper() )
	    return true;
    }
    return false;
}

/*!
    Clears the value of all fields in the record. If \a nullify is
    true, (the default is false), each field is set to NULL.
*/

void TQSqlRecord::clearValues( bool nullify )
{
    checkDetach();
    int cnt = (int)count();
    int i;
    for ( i = 0; i < cnt; ++i ) {
	field( i )->clear( nullify );
    }
}

/*!
    Sets the generated flag for the field called \a name to \a
    generated. If the field does not exist, nothing happens. Only
    fields that have \a generated set to true are included in the SQL
    that is generated, e.g. by TQSqlCursor.

    \sa isGenerated()
*/

void TQSqlRecord::setGenerated( const TQString& name, bool generated )
{
    setGenerated( position( name ), generated );
}

/*!
    \overload

    Sets the generated flag for the field \a i to \a generated.

    \sa isGenerated()
*/

void TQSqlRecord::setGenerated( int i, bool generated )
{
    checkDetach();
    if ( !field( i ) )
	return;
    sh->d->fieldInfo( i )->nogen = !generated;
}

/*!
    \internal
    ### Remove in 4.0
*/
bool TQSqlRecord::isNull( int i )
{
    checkDetach();
    TQSqlField* f = field( i );
    if ( f ) {
	return f->isNull();
    }
    return true;
}

/*! 
    \internal
    ### Remove in 4.0
*/
bool TQSqlRecord::isNull( const TQString& name )
{
    return isNull( position( name ) );
}

/*!
    \overload

    Returns true if the field \a i is NULL or if there is no field at
    position \a i; otherwise returns false.

    \sa fieldName()
*/
bool TQSqlRecord::isNull( int i ) const
{
    const TQSqlField* f = field( i );
    if ( f ) {
	return f->isNull();
    }
    return true;
}

/*!
    Returns true if the field called \a name is NULL or if there is no
    field called \a name; otherwise returns false.

    \sa position()
*/
bool TQSqlRecord::isNull( const TQString& name ) const
{
    return isNull( position( name ) );
}

/*!
    Sets the value of field \a i to NULL. If the field does not exist,
    nothing happens.
*/
void TQSqlRecord::setNull( int i )
{
    checkDetach();
    TQSqlField* f = field( i );
    if ( f ) {
	f->setNull();
    }
}

/*!
    \overload

    Sets the value of the field called \a name to NULL. If the field
    does not exist, nothing happens.
*/
void TQSqlRecord::setNull( const TQString& name )
{
    setNull( position( name ) );
}


/*!
    Returns true if the record has a field called \a name and this
    field is to be generated (the default); otherwise returns false.

    \sa setGenerated()
*/
bool TQSqlRecord::isGenerated( const TQString& name ) const
{
    return isGenerated( position( name ) );
}

/*!
    \overload

    Returns true if the record has a field at position \a i and this
    field is to be generated (the default); otherwise returns false.

    \sa setGenerated()
*/
bool TQSqlRecord::isGenerated( int i ) const
{
    if ( !field( i ) )
	return false;
    return !sh->d->fieldInfo( i )->nogen;
}


/*!
    Returns a list of all the record's field names as a string
    separated by \a sep.

    Note that fields which are not generated are \e not included (see
    \l{isGenerated()}). The returned string is suitable, for example, for
    generating SQL SELECT statements. If a \a prefix is specified,
    e.g. a table name, all fields are prefixed in the form:

    "\a{prefix}.\<fieldname\>"
*/

TQString TQSqlRecord::toString( const TQString& prefix, const TQString& sep ) const
{
    TQString pflist;
    bool comma = false;
    for ( uint i = 0; i < count(); ++i ){
	if ( isGenerated( field(i)->name() ) ) {
	    if( comma )
		pflist += sep + " ";
	    pflist += createField( i, prefix );
	    comma = true;
	}
    }
    return pflist;
}

/*!
    Returns a list of all the record's field names, each having the
    prefix \a prefix.

    Note that fields which have generated set to false are \e not
    included. (See \l{isGenerated()}). If \a prefix is supplied, e.g.
    a table name, all fields are prefixed in the form:

    "\a{prefix}.\<fieldname\>"
*/

TQStringList TQSqlRecord::toStringList( const TQString& prefix ) const
{
    TQStringList s;
    for ( uint i = 0; i < count(); ++i ) {
	if ( isGenerated( field(i)->name() ) )
	    s += createField( i, prefix );
    }
    return s;
}

/*! \internal
*/

TQString TQSqlRecord::createField( int i, const TQString& prefix ) const
{
    TQString f;
    if ( !prefix.isEmpty() )
	f = prefix + ".";
    f += field( i )->name();
    return f;
}

/*!
    Returns the number of fields in the record.
*/

uint TQSqlRecord::count() const
{
    return sh->d->count();
}

/*!
    Sets the value of the field at position \a i to \a val. If the
    field does not exist, nothing happens.
*/

void TQSqlRecord::setValue( int i, const TQVariant& val )
{
    checkDetach();
    TQSqlField* f = field( i );
    if ( f ) {
	f->setValue( val );
    }
}


/*!
    \overload

    Sets the value of the field called \a name to \a val. If the field
    does not exist, nothing happens.
*/

void TQSqlRecord::setValue( const TQString& name, const TQVariant& val )
{
    setValue( position( name ), val );
}


/******************************************/
/*******     TQSqlRecordInfo Impl     ******/
/******************************************/

/*!
    \class TQSqlRecordInfo ntqsqlrecord.h
    \brief The TQSqlRecordInfo class encapsulates a set of database field meta data.

    \ingroup database
    \module sql

    This class is a TQValueList that holds a set of database field meta
    data. Use contains() to see if a given field name exists in the
    record, and use find() to get a TQSqlFieldInfo record for a named
    field.

    \sa TQValueList, TQSqlFieldInfo
*/


/*!
    Constructs a TQSqlRecordInfo object based on the fields in the
    TQSqlRecord \a other.
*/
TQSqlRecordInfo::TQSqlRecordInfo( const TQSqlRecord& other )
{
    for ( uint i = 0; i < other.count(); ++i ) {
	push_back( TQSqlFieldInfo( *(other.field( i )), other.isGenerated( i ) ) );
    }
}

/*!
    Returns the number of times a field called \a fieldName occurs in
    the record. Returns 0 if no field by that name could be found.
*/
TQSqlRecordInfo::size_type TQSqlRecordInfo::contains( const TQString& fieldName ) const
{
   size_type i = 0;
   TQString fName = fieldName.upper();
   for( const_iterator it = begin(); it != end(); ++it ) {
	if ( (*it).name().upper() == fName ) {
	    ++i;
	}
    }
    return i;
}

/*!
    Returns a TQSqlFieldInfo object for the first field in the record
    which has the field name \a fieldName. If no matching field is
    found then an empty TQSqlFieldInfo object is returned.
*/
TQSqlFieldInfo TQSqlRecordInfo::find( const TQString& fieldName ) const
{
   TQString fName = fieldName.upper();
   for( const_iterator it = begin(); it != end(); ++it ) {
	if ( (*it).name().upper() == fName ) {
	    return *it;
	}
    }
    return TQSqlFieldInfo();
}

/*!
    Returns an empty TQSqlRecord based on the field information
    in this TQSqlRecordInfo.
*/
TQSqlRecord TQSqlRecordInfo::toRecord() const
{
   TQSqlRecord buf;
   for( const_iterator it = begin(); it != end(); ++it ) {
	buf.append( (*it).toField() );
   }
   return buf;
}

/*!
    \fn TQSqlRecordInfo::TQSqlRecordInfo()

    Constructs an empty record info object
*/

/*!
    \fn TQSqlRecordInfo::TQSqlRecordInfo( const TQSqlFieldInfoList& other )

    Constructs a copy of \a other.
*/

#endif
