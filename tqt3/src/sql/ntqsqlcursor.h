/****************************************************************************
**
** Definition of TQSqlCursor class
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

#ifndef TQSQLCURSOR_H
#define TQSQLCURSOR_H

#ifndef QT_H
#include "ntqsqlrecord.h"
#include "ntqstringlist.h"
#include "ntqsqlquery.h"
#include "ntqsqlindex.h"
#endif // QT_H

#if !defined( TQT_MODULE_SQL ) || defined( QT_LICENSE_PROFESSIONAL )
#define TQM_EXPORT_SQL
#else
#define TQM_EXPORT_SQL TQ_EXPORT
#endif

#ifndef TQT_NO_SQL

class TQSqlDatabase;
class TQSqlCursorPrivate;

class TQM_EXPORT_SQL TQSqlCursor : public TQSqlRecord, public TQSqlQuery
{
public:
    TQSqlCursor( const TQString & name = TQString::null, bool autopopulate = true, TQSqlDatabase* db = 0 );
    TQSqlCursor( const TQSqlCursor & other );
    TQSqlCursor& operator=( const TQSqlCursor& other );
    ~TQSqlCursor();

    enum Mode {
	ReadOnly = 0,
	Insert = 1,
	Update = 2,
	Delete = 4,
	Writable = 7
    };

    TQVariant		value( int i ) const;
    TQVariant		value( const TQString& name ) const;
    void         	setValue( int i, const TQVariant& val );
    void         	setValue( const TQString& name, const TQVariant& val );
    virtual TQSqlIndex	primaryIndex( bool prime = true ) const;
    virtual TQSqlIndex	index( const TQStringList& fieldNames ) const;
    TQSqlIndex		index( const TQString& fieldName ) const;
    TQSqlIndex		index( const char* fieldName ) const;
    virtual void	setPrimaryIndex( const TQSqlIndex& idx );

    virtual void	append( const TQSqlFieldInfo& fieldInfo );
    virtual void	insert( int pos, const TQSqlFieldInfo& fieldInfo );
    void		remove( int pos );
    void		clear();
    void		setGenerated( const TQString& name, bool generated );
    void		setGenerated( int i, bool generated );

    virtual TQSqlRecord*	editBuffer( bool copy = false );
    virtual TQSqlRecord*	primeInsert();
    virtual TQSqlRecord*	primeUpdate();
    virtual TQSqlRecord*	primeDelete();
    virtual int		insert( bool invalidate = true );
    virtual int		update( bool invalidate = true );
    virtual int		del( bool invalidate = true );

    virtual void	setMode( int flags );
    int			mode() const;
    virtual void	setCalculated( const TQString& name, bool calculated );
    bool		isCalculated( const TQString& name ) const;
    virtual void	setTrimmed( const TQString& name, bool trim );
    bool		isTrimmed( const TQString& name ) const;

    bool		isReadOnly() const;
    bool		canInsert() const;
    bool		canUpdate() const;
    bool		canDelete() const;

    bool		select();
    bool		select( const TQSqlIndex& sort );
    bool		select( const TQSqlIndex & filter, const TQSqlIndex & sort );
    virtual bool	select( const TQString & filter, const TQSqlIndex & sort = TQSqlIndex() );

    virtual void	setSort( const TQSqlIndex& sort );
    TQSqlIndex		sort() const;
    virtual void	setFilter( const TQString& filter );
    TQString		filter() const;
    virtual void	setName( const TQString& name, bool autopopulate = true );
    TQString		name() const;
    TQString		toString( const TQString& prefix = TQString::null,
				const TQString& sep = "," ) const;
    bool 		isNull( int i ) const;
    bool 		isNull( const TQString& name ) const;
    
protected:
    void		afterSeek();
    bool		exec( const TQString & sql );

    virtual TQVariant	calculateField( const TQString& name );
    virtual int		update( const TQString & filter, bool invalidate = true );
    virtual int		del( const TQString & filter, bool invalidate = true );

    virtual TQString	toString( const TQString& prefix, TQSqlField* field, const TQString& fieldSep ) const;
    virtual TQString	toString( TQSqlRecord* rec, const TQString& prefix, const TQString& fieldSep,
				const TQString& sep ) const;
    virtual TQString	toString( const TQSqlIndex& i, TQSqlRecord* rec, const TQString& prefix,
				const TQString& fieldSep, const TQString& sep ) const;

private:
    void		sync();
    int			apply( const TQString& q, bool invalidate );
    int			applyPrepared( const TQString& q, bool invalidate );
    TQSqlRecord&		operator=( const TQSqlRecord & list );
    void 		append( const TQSqlField& field );
    void 		insert( int pos, const TQSqlField& field );

    TQSqlCursorPrivate*	d;
};




#endif	// TQT_NO_SQL
#endif
