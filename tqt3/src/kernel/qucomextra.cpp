/****************************************************************************
**
** Implementation of extra TQUcom classes
**
** Created : 990101
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

#include "qucomextra_p.h"
#include <ntqvariant.h>


#ifndef TQT_NO_VARIANT
// 6dc75d58-a1d9-4417-b591-d45c63a3a4ea
const TQUuid TID_QUType_TQVariant( 0x6dc75d58, 0xa1d9, 0x4417, 0xb5, 0x91, 0xd4, 0x5c, 0x63, 0xa3, 0xa4, 0xea );
TQUType_TQVariant static_QUType_TQVariant;

const TQUuid *TQUType_TQVariant::uuid() const { return &TID_QUType_TQVariant; }
const char *TQUType_TQVariant::desc() const { return "TQVariant"; }

void TQUType_TQVariant::set( TQUObject *o, const TQVariant& v )
{
    o->payload.ptr = new TQVariant( v );
    o->type = this;
}

TQVariant &TQUType_TQVariant::get( TQUObject * o )
{
    return *(TQVariant*)o->payload.ptr;
}

bool TQUType_TQVariant::canConvertFrom( TQUObject *o, TQUType *t )
{
    if ( isEqual( o->type, &static_QUType_TQString )
      || isEqual( o->type, &static_QUType_int )
      || isEqual( o->type, &static_QUType_bool )
      || isEqual( o->type, &static_QUType_double )
      || isEqual( o->type, &static_QUType_charstar ) )
	return true;
    return t->canConvertTo( o, this );
}

bool TQUType_TQVariant::canConvertTo( TQUObject * /*o*/, TQUType * /*t*/ )
{
    return false;
}

bool TQUType_TQVariant::convertFrom( TQUObject *o, TQUType *t )
{
    TQVariant *var = 0;
    if ( isEqual( o->type, &static_QUType_TQString ) )
	var = new TQVariant( static_QUType_TQString.get( o ) );
    else if ( isEqual( o->type, &static_QUType_int ) )
	var = new TQVariant( static_QUType_int.get( o ) );
    else if ( isEqual( o->type, &static_QUType_bool ) )
	var = new TQVariant( static_QUType_bool.get( o ) );
    else if ( isEqual( o->type, &static_QUType_double ) )
	var = new TQVariant( static_QUType_double.get( o ) );
    else if ( isEqual( o->type, &static_QUType_charstar ) )
	var = new TQVariant( static_QUType_charstar.get( o ) );
    else
	return t->convertTo( o, this );

    o->type->clear( o );
    o->payload.ptr = var;
    o->type = this;
    return true;
}

bool TQUType_TQVariant::convertTo( TQUObject * /*o*/, TQUType * /*t*/ )
{
    return false;
}

void TQUType_TQVariant::clear( TQUObject *o )
{
    delete (TQVariant*)o->payload.ptr;
    o->payload.ptr = 0;
}

int TQUType_TQVariant::serializeTo( TQUObject *, TQUBuffer * )
{
    return 0;
}

int TQUType_TQVariant::serializeFrom( TQUObject *, TQUBuffer * )
{
    return 0;
}


#endif

const TQUuid TID_QUType_varptr( 0x8d48b3a8, 0xbd7f, 0x11d5, 0x8d, 0x74, 0x00, 0xc0, 0xf0, 0x3b, 0xc0, 0xf3 );
TQUType_varptr static_QUType_varptr;
const TQUuid *TQUType_varptr::uuid() const  { return &TID_QUType_varptr; }
const char *TQUType_varptr::desc() const { return "varptr"; }

void TQUType_varptr::set( TQUObject *o, const void* v )
{
    o->payload.ptr = (void*) v;
    o->type = this;
}

bool TQUType_varptr::canConvertFrom( TQUObject *o, TQUType *t )
{
     if ( isEqual( t, &static_QUType_ptr ) )
	return true;
   return t->canConvertTo( o, this );
}

bool TQUType_varptr::canConvertTo( TQUObject *, TQUType * t)
{
    return isEqual( t, &static_QUType_ptr );
}

bool TQUType_varptr::convertFrom( TQUObject *o, TQUType *t )
{
     if ( isEqual( t, &static_QUType_ptr ) )
	;
    else
	return t->convertTo( o, this );

    o->type = this;
    return true;
}

bool TQUType_varptr::convertTo( TQUObject *o, TQUType * t)
{
    if ( isEqual( t,  &static_QUType_ptr ) ) {
	o->type = &static_QUType_ptr;
	return true;
    }
    return false;
}

int TQUType_varptr::serializeTo( TQUObject *, TQUBuffer * )
{
    return 0;
}

int TQUType_varptr::serializeFrom( TQUObject *, TQUBuffer * )
{
    return 0;
}
