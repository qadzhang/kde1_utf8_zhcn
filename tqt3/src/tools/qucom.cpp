/****************************************************************************
**
** Implementation of the TQUcom classes
**
** Created : 990101
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

#include "private/qucom_p.h"
#include "private/qucomextra_p.h"

#include "ntqvariant.h"

#include <new>

// Standard types

// {DE56510E-4E9F-4b76-A3C2-D1E2EF42F1AC}
const TQUuid TID_QUType_Null( 0xde56510e, 0x4e9f, 0x4b76, 0xa3, 0xc2, 0xd1, 0xe2, 0xef, 0x42, 0xf1, 0xac );
const TQUuid *TQUType_Null::uuid() const { return &TID_QUType_Null; }
const char *TQUType_Null::desc() const { return "null"; }
bool TQUType_Null::canConvertFrom( TQUObject *, TQUType * ) { return false; }
bool TQUType_Null::canConvertTo( TQUObject *, TQUType * ) { return false; }
bool TQUType_Null::convertFrom( TQUObject *, TQUType * ) { return false; }
bool TQUType_Null::convertTo( TQUObject *, TQUType * ) { return false; }
void TQUType_Null::clear( TQUObject *) {}
int TQUType_Null::serializeTo( TQUObject *, TQUBuffer * ) { return 0; }
int TQUType_Null::serializeFrom( TQUObject *, TQUBuffer * ) { return 0; }
TQUType_Null static_QUType_Null;


// {7EE17B08-5419-47e2-9776-8EEA112DCAEC}
const TQUuid TID_QUType_enum( 0x7ee17b08, 0x5419, 0x47e2, 0x97, 0x76, 0x8e, 0xea, 0x11, 0x2d, 0xca, 0xec );
TQUType_enum static_QUType_enum;
const TQUuid *TQUType_enum::uuid() const { return &TID_QUType_enum; }
const char *TQUType_enum::desc() const { return "enum"; }
void TQUType_enum::set( TQUObject *o, int v )
{
    o->payload.i = v;
    o->type = this;
}

bool TQUType_enum::canConvertFrom( TQUObject *o, TQUType *t )
{
    if ( isEqual( t, &static_QUType_int ) ) // ## todo unsigned int?
	return true;

    return t->canConvertTo( o, this );
}

bool TQUType_enum::canConvertTo( TQUObject * /*o*/, TQUType *t )
{
    return isEqual( t, &static_QUType_int );
}

bool TQUType_enum::convertFrom( TQUObject *o, TQUType *t )
{
    if ( isEqual( t, &static_QUType_int ) ) // ## todo unsigned int?
	;
    else
	return t->convertTo( o, this );

    o->type = this;
    return true;
}

bool TQUType_enum::convertTo( TQUObject *o, TQUType *t )
{
    if ( isEqual( t,  &static_QUType_int ) ) {
	o->type = &static_QUType_int;
	return true;
    }
    return false;
}

int TQUType_enum::serializeTo( TQUObject *, TQUBuffer * )
{
    return 0;
}

int TQUType_enum::serializeFrom( TQUObject *, TQUBuffer * )
{
    return 0;
}

// {8AC26448-5AB4-49eb-968C-8F30AB13D732}
const TQUuid TID_QUType_ptr( 0x8ac26448, 0x5ab4, 0x49eb, 0x96, 0x8c, 0x8f, 0x30, 0xab, 0x13, 0xd7, 0x32 );
TQUType_ptr static_QUType_ptr;
const TQUuid *TQUType_ptr::uuid() const  { return &TID_QUType_ptr; }
const char *TQUType_ptr::desc() const { return "ptr"; }

void TQUType_ptr::set( TQUObject *o, const void* v )
{
    o->payload.ptr = (void*) v;
    o->type = this;
}

bool TQUType_ptr::canConvertFrom( TQUObject *o, TQUType *t )
{
    return t->canConvertTo( o, this );
}

bool TQUType_ptr::canConvertTo( TQUObject *, TQUType * )
{
    return false;
}

bool TQUType_ptr::convertFrom( TQUObject *o, TQUType *t )
{
    return t->convertTo( o, this );
}

bool TQUType_ptr::convertTo( TQUObject *, TQUType * )
{
    return false;
}

int TQUType_ptr::serializeTo( TQUObject *, TQUBuffer * )
{
    return 0;
}

int TQUType_ptr::serializeFrom( TQUObject *, TQUBuffer * )
{
    return 0;
}

// {97A2594D-6496-4402-A11E-55AEF2D4D25C}
const TQUuid TID_QUType_iface( 0x97a2594d, 0x6496, 0x4402, 0xa1, 0x1e, 0x55, 0xae, 0xf2, 0xd4, 0xd2, 0x5c );
TQUType_iface static_QUType_iface;
const TQUuid *TQUType_iface::uuid() const  { return &TID_QUType_iface; }
const char *TQUType_iface::desc() const { return "UnknownInterface"; }

void TQUType_iface::set( TQUObject *o, TQUnknownInterface* iface )
{
    o->payload.iface = iface;
    o->type = this;
}

bool TQUType_iface::canConvertFrom( TQUObject *o, TQUType *t )
{
    return t->canConvertTo( o, this );
}

bool TQUType_iface::canConvertTo( TQUObject *, TQUType * )
{
    return false;
}

bool TQUType_iface::convertFrom( TQUObject *o, TQUType *t )
{
    return t->convertTo( o, this );
}

bool TQUType_iface::convertTo( TQUObject *, TQUType * )
{
    return false;
}

int TQUType_iface::serializeTo( TQUObject *, TQUBuffer * )
{
    return 0;
}

int TQUType_iface::serializeFrom( TQUObject *, TQUBuffer * )
{
    return 0;
}

// {2F358164-E28F-4bf4-9FA9-4E0CDCABA50B}
const TQUuid TID_QUType_idisp( 0x2f358164, 0xe28f, 0x4bf4, 0x9f, 0xa9, 0x4e, 0xc, 0xdc, 0xab, 0xa5, 0xb );
TQUType_idisp static_QUType_idisp;
const TQUuid *TQUType_idisp::uuid() const  { return &TID_QUType_idisp; }
const char *TQUType_idisp::desc() const { return "DispatchInterface"; }

void TQUType_idisp::set( TQUObject *o, TQDispatchInterface* idisp )
{
    o->payload.idisp = idisp;
    o->type = this;
}

bool TQUType_idisp::canConvertFrom( TQUObject *o, TQUType *t )
{
    return t->canConvertTo( o, this );
}

bool TQUType_idisp::canConvertTo( TQUObject * /*o*/, TQUType *t )
{
    return isEqual( t, &static_QUType_iface );
}

bool TQUType_idisp::convertFrom( TQUObject *o, TQUType *t )
{
    return t->convertTo( o, this );
}

bool TQUType_idisp::convertTo( TQUObject *o, TQUType *t )
{
#ifndef TQT_NO_COMPONENT
    if ( isEqual( t, &static_QUType_iface ) ) {
	o->payload.iface = (TQUnknownInterface*)o->payload.idisp;
	o->type = &static_QUType_iface;
	return true;
    }
#endif
    return false;
}

int TQUType_idisp::serializeTo( TQUObject *, TQUBuffer * )
{
    return 0;
}

int TQUType_idisp::serializeFrom( TQUObject *, TQUBuffer * )
{
    return 0;
}

// {CA42115D-13D0-456c-82B5-FC10187F313E}
const TQUuid TID_QUType_bool( 0xca42115d, 0x13d0, 0x456c, 0x82, 0xb5, 0xfc, 0x10, 0x18, 0x7f, 0x31, 0x3e );
TQUType_bool static_QUType_bool;
const TQUuid *TQUType_bool::uuid() const  { return &TID_QUType_bool; }
const char *TQUType_bool::desc() const { return "bool"; }

void TQUType_bool::set( TQUObject *o, bool v )
{
    o->payload.b = v;
    o->type = this;
}

bool TQUType_bool::canConvertFrom( TQUObject *o, TQUType *t )
{
    return t->canConvertTo( o, this );
}

bool TQUType_bool::canConvertTo( TQUObject *, TQUType * )
{
    return false;
}

bool TQUType_bool::convertFrom( TQUObject *o, TQUType *t )
{
    return t->convertTo( o, this );
}

bool TQUType_bool::convertTo( TQUObject *, TQUType * )
{
    return false;
}

int TQUType_bool::serializeTo( TQUObject *, TQUBuffer * )
{
    return 0;
}

int TQUType_bool::serializeFrom( TQUObject *, TQUBuffer * )
{
    return 0;
}

// {53C1F3BE-73C3-4c7d-9E05-CCF09EB676B5}
const TQUuid TID_QUType_int( 0x53c1f3be, 0x73c3, 0x4c7d, 0x9e, 0x5, 0xcc, 0xf0, 0x9e, 0xb6, 0x76, 0xb5 );
TQUType_int static_QUType_int;
const TQUuid *TQUType_int::uuid() const  { return &TID_QUType_int; }
const char *TQUType_int::desc() const { return "int"; }

void TQUType_int::set( TQUObject *o, int v )
{
    o->payload.i = v;
    o->type = this;
}

bool TQUType_int::canConvertFrom( TQUObject *o, TQUType *t )
{
    if ( isEqual( t, &static_QUType_double ) )
	return true;

    return t->canConvertTo( o, this );
}

bool TQUType_int::canConvertTo( TQUObject * /*o*/, TQUType *t )
{
    return isEqual( t,  &static_QUType_double );
}

bool TQUType_int::convertFrom( TQUObject *o, TQUType *t )
{
    if ( isEqual( t, &static_QUType_double ) )
	o->payload.i = (long)o->payload.d;
    else
	return t->convertTo( o, this );

    o->type = this;
    return true;
}

bool TQUType_int::convertTo( TQUObject *o, TQUType *t )
{
    if ( isEqual( t,  &static_QUType_double ) ) {
	o->payload.d = (double)o->payload.i;
	o->type = &static_QUType_double;
    } else
	return false;
    return true;
}

int TQUType_int::serializeTo( TQUObject *, TQUBuffer * )
{
    return 0;
}

int TQUType_int::serializeFrom( TQUObject *, TQUBuffer * )
{
    return 0;
}

// {2D0974E5-0BA6-4ec2-8837-C198972CB48C}
const TQUuid TID_QUType_double( 0x2d0974e5, 0xba6, 0x4ec2, 0x88, 0x37, 0xc1, 0x98, 0x97, 0x2c, 0xb4, 0x8c );
TQUType_double static_QUType_double;
const TQUuid *TQUType_double::uuid() const { return &TID_QUType_double; }
const char *TQUType_double::desc() const {return "double"; }

void TQUType_double::set( TQUObject *o, double v )
{
    o->payload.d = v;
    o->type = this;
}

bool TQUType_double::canConvertFrom( TQUObject *o, TQUType *t )
{
    if ( isEqual( t, &static_QUType_int ) )
	return true;

    return t->canConvertTo( o, this );
}

bool TQUType_double::canConvertTo( TQUObject * /*o*/, TQUType *t )
{
    return isEqual( t,  &static_QUType_int );
}

bool TQUType_double::convertFrom( TQUObject *o, TQUType *t )
{
    if ( isEqual( t, &static_QUType_int ) )
	o->payload.d = (double)o->payload.i;
     else
	return t->convertTo( o, this );

    o->type = this;
    return true;
}

bool TQUType_double::convertTo( TQUObject *o, TQUType *t )
{
    if ( isEqual( t,  &static_QUType_int ) ) {
	o->payload.i = (int) o->payload.d;
	o->type = &static_QUType_int;
    } else if ( isEqual( t,  &static_QUType_double ) ) {
	o->payload.d = (double) o->payload.f;
	o->type = &static_QUType_double;
    } else
	return false;
    return true;
}

int TQUType_double::serializeTo( TQUObject *, TQUBuffer * )
{
    return 0;
}

int TQUType_double::serializeFrom( TQUObject *, TQUBuffer * )
{
    return 0;
}

// {EFCDD1D4-77A3-4b8e-8D46-DC14B8D393E9}
const TQUuid TID_QUType_charstar( 0xefcdd1d4, 0x77a3, 0x4b8e, 0x8d, 0x46, 0xdc, 0x14, 0xb8, 0xd3, 0x93, 0xe9 );
TQUType_charstar static_QUType_charstar;
const TQUuid *TQUType_charstar::uuid() const { return &TID_QUType_charstar; }
const char *TQUType_charstar::desc() const { return "char*"; }

void TQUType_charstar::set( TQUObject *o, const char* v, bool take )
{
    if ( take ) {
	if ( v ) {
	    o->payload.charstar.ptr = new char[ strlen(v) + 1 ];
	    strcpy( o->payload.charstar.ptr, v );
	} else {
	    o->payload.charstar.ptr = 0;
	}
	o->payload.charstar.owner = true;
    } else {
	o->payload.charstar.ptr = (char*) v;
	o->payload.charstar.owner = false;
    }
    o->type = this;
}

bool TQUType_charstar::canConvertFrom( TQUObject *o, TQUType *t )
{
    return t->canConvertTo( o, this );
}

bool TQUType_charstar::canConvertTo( TQUObject *, TQUType * )
{
    return false;
}

bool TQUType_charstar::convertFrom( TQUObject *o, TQUType *t )
{
    return t->convertTo( o, this );
}

bool TQUType_charstar::convertTo( TQUObject *, TQUType * )
{
    return false;
}

void TQUType_charstar::clear( TQUObject *o )
{
    if ( o->payload.charstar.owner )
	delete [] o->payload.charstar.ptr;
    o->payload.charstar.ptr = 0;
}

int TQUType_charstar::serializeTo( TQUObject *, TQUBuffer * )
{
    return 0;
}

int TQUType_charstar::serializeFrom( TQUObject *, TQUBuffer * )
{
    return 0;
}


// TQt specific types

// {44C2A547-01E7-4e56-8559-35AF9D2F42B7}
const TQUuid TID_QUType_TQString( 0x44c2a547, 0x1e7, 0x4e56, 0x85, 0x59, 0x35, 0xaf, 0x9d, 0x2f, 0x42, 0xb7 );
TQUType_TQString static_QUType_TQString;
const TQUuid *TQUType_TQString::uuid() const { return &TID_QUType_TQString; }
const char *TQUType_TQString::desc() const { return "TQString"; }

void TQUType_TQString::set( TQUObject *o, const TQString& v )
{
    o->payload.ptr = new TQString( v );
    o->type = this;
}

bool TQUType_TQString::canConvertFrom( TQUObject *o, TQUType *t )
{
    if ( isEqual( t, &static_QUType_charstar ) ||
	 isEqual( t, &static_QUType_double ) ||
	 isEqual( t, &static_QUType_int ) )
	return true;

    return t->canConvertTo( o, this );
}

bool TQUType_TQString::canConvertTo( TQUObject * /*o*/, TQUType *t )
{
    return isEqual( t, &static_QUType_charstar ) ||
	isEqual( t,  &static_QUType_int ) ||
	isEqual( t,  &static_QUType_double );
}

bool TQUType_TQString::convertFrom( TQUObject *o, TQUType *t )
{
    TQString *str = 0;
    if ( isEqual( t, &static_QUType_charstar ) )
	str = new TQString( o->payload.charstar.ptr );
    else if ( isEqual( t, &static_QUType_double ) )
	str = new TQString( TQString::number( o->payload.d ) );
    else if ( isEqual( t, &static_QUType_int ) )
	str = new TQString( TQString::number( o->payload.i ) );
    else
	return t->convertTo( o, this );

    o->type->clear( o );
    o->payload.ptr = str;
    o->type = this;
    return true;
}

bool TQUType_TQString::convertTo( TQUObject *o, TQUType *t )
{
    TQString *str = (TQString *)o->payload.ptr;
    if ( isEqual( t, &static_QUType_charstar ) ) {
	o->payload.charstar.ptr = tqstrdup( str->local8Bit().data() );
	o->payload.charstar.owner = true;
	o->type = &static_QUType_charstar;
    } else if ( isEqual( t,  &static_QUType_int ) ) {
	o->payload.l = str->toLong();
	o->type = &static_QUType_int;
    } else if ( isEqual( t,  &static_QUType_double ) ) {
	o->payload.d = str->toDouble();
	o->type = &static_QUType_double;
    } else {
        return false;
    }
    delete str;
    return true;
}

int TQUType_TQString::serializeTo( TQUObject *, TQUBuffer * )
{
    return 0;
}

int TQUType_TQString::serializeFrom( TQUObject *, TQUBuffer * )
{
    return 0;
}

void TQUType_TQString::clear( TQUObject *o )
{
    delete (TQString*)o->payload.ptr;
    o->payload.ptr = 0;
}

TQUObject* TQUObject::deepCopy(TQUObject* newLocation) {
    TQUObject* ret;
    if (newLocation) {
        ret = new(newLocation) TQUObject(*this);
    }
    else {
        ret = new TQUObject(*this);
    }
    // Any type that has a clear() method must be copied here!
    if (*(type->uuid()) == TID_QUType_charstar) {
        static_QUType_charstar.set( ret, (const char *)static_QUType_charstar.get(this), true );
    }
    if (*(type->uuid()) == TID_QUType_TQString) {
        static_QUType_TQString.set( ret, (TQString)static_QUType_TQString.get(this) );
    }
    if (*(type->uuid()) == TID_QUType_TQVariant) {
        static_QUType_TQVariant.set( ret, (TQVariant)static_QUType_TQVariant.get(this) );
    }
    return ret;
}
