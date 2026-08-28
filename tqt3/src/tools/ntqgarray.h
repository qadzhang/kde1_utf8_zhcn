/****************************************************************************
**
** Definition of TQGArray class
**
** Created : 930906
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

#ifndef TQGARRAY_H
#define TQGARRAY_H

#ifndef QT_H
#include "ntqshared.h"
#endif // QT_H


class TQ_EXPORT TQGArray					// generic array
{
friend class TQBuffer;
public:
    // do not use this, even though this is public
    // ### make protected or private in TQt 4.0 beta?
    struct array_data : public TQShared {	// shared array
	array_data():data(0),len(0)
#ifdef QT_QGARRAY_SPEED_OPTIM
		    ,maxl(0)
#endif
	    {}
	char *data;				// actual array data
	uint  len;
#ifdef QT_QGARRAY_SPEED_OPTIM
	uint maxl;
#endif
    };
    TQGArray();
    enum Optimization { MemOptim, SpeedOptim };
protected:
    TQGArray( int, int );			// dummy; does not alloc
    TQGArray( int size );			// allocate 'size' bytes
    TQGArray( const TQGArray &a );		// shallow copy
    virtual ~TQGArray();

    TQGArray    &operator=( const TQGArray &a ) { return assign( a ); }

    virtual void detach()	{ duplicate(*this); }

    // ### TQt 4.0: maybe provide two versions of data(), at(), etc.
    char       *data()	 const	{ return shd->data; }
    uint	nrefs()	 const	{ return shd->count; }
    uint	size()	 const	{ return shd->len; }
    bool	isEqual( const TQGArray &a ) const;

    bool	resize( uint newsize, Optimization optim );
    bool	resize( uint newsize );

    bool	fill( const char *d, int len, uint sz );

    TQGArray    &assign( const TQGArray &a );
    TQGArray    &assign( const char *d, uint len );
    TQGArray    &duplicate( const TQGArray &a );
    TQGArray    &duplicate( const char *d, uint len );
    void	store( const char *d, uint len );

    array_data *sharedBlock()	const		{ return shd; }
    void	setSharedBlock( array_data *p ) { shd=(array_data*)p; }

    TQGArray    &setRawData( const char *d, uint len );
    void	resetRawData( const char *d, uint len );

    int		find( const char *d, uint index, uint sz ) const;
    int		contains( const char *d, uint sz ) const;

    void	sort( uint sz );
    int		bsearch( const char *d, uint sz ) const;

    char       *at( uint index ) const;

    bool	setExpand( uint index, const char *d, uint sz );

protected:
    virtual array_data *newData();
    virtual void deleteData( array_data *p );

private:
    static void msg_index( uint );
    array_data *shd;
};


inline char *TQGArray::at( uint index ) const
{
#if defined(QT_CHECK_RANGE)
    if ( index >= size() ) {
	msg_index( index );
	index = 0;
    }
#endif
    return &shd->data[index];
}


#endif // TQGARRAY_H
