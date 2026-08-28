/****************************************************************************
**
** Definition of the TQUcom interfaces
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

#ifndef TQUCOM_P_H
#define TQUCOM_P_H

//
//  W A R N I N G
//  -------------
//
// This file is not part of the TQt API.  It exists for the convenience
// of a number of TQt sources files.  This header file may change from
// version to version without notice, or even be removed.
//
// We mean it.
//
//

#ifndef QT_H
#include "ntqstring.h"
#include "ntquuid.h"
#endif // QT_H

#ifdef check
#undef check
#endif

#if __GNUC__ - 0 > 3
#pragma GCC system_header
#endif

struct TQUObject;
struct TQUInterfaceDescription;
struct TQUnknownInterface;
struct TQDispatchInterface;


struct TQ_EXPORT TQUBuffer
{
    virtual long read( char *data, ulong maxlen ) = 0;
    virtual long write( const char *data, ulong len ) = 0;
};


// A type for a TQUObject
struct TQ_EXPORT TQUType
{
    virtual const TQUuid *uuid() const = 0;
    virtual const char *desc() const = 0;


    virtual bool canConvertFrom( TQUObject *, TQUType * ) = 0;
    // virtual private, only called by canConvertFrom
    virtual bool canConvertTo( TQUObject *, TQUType * ) = 0;


    virtual bool convertFrom( TQUObject *, TQUType * ) = 0;
    // virtual private, only called by convertFrom
    virtual bool convertTo( TQUObject *, TQUType * ) = 0;

    virtual void clear( TQUObject * ) = 0;

    virtual int serializeTo( TQUObject *, TQUBuffer * ) = 0;
    virtual int serializeFrom( TQUObject *, TQUBuffer * ) = 0;

    static bool isEqual( const TQUType *t1, const TQUType *t2 );
    static bool check( TQUObject* o, TQUType* t );
};


// {DE56510E-4E9F-4b76-A3C2-D1E2EF42F1AC}
extern TQ_EXPORT const TQUuid TID_QUType_Null;
struct TQ_EXPORT TQUType_Null : public TQUType
{
    const TQUuid *uuid() const;
    const char *desc() const;

    bool canConvertFrom( TQUObject *, TQUType * );
    bool canConvertTo( TQUObject *, TQUType * );
    bool convertFrom( TQUObject *, TQUType * );
    bool convertTo( TQUObject *, TQUType * );
    void clear( TQUObject * );
    int serializeTo( TQUObject *, TQUBuffer * );
    int serializeFrom( TQUObject *, TQUBuffer * );
};
extern TQ_EXPORT TQUType_Null static_QUType_Null;


// The magic TQUObject
struct TQ_EXPORT TQUObject
{
public: // scary MSVC bug makes this necessary
    TQUObject() : type( &static_QUType_Null ), isLastObject(false) {}
    ~TQUObject() { type->clear( this ); }

    TQUType *type;

    // the unavoidable union
    union
    {
	bool b;

	char c;
	short s;
	int i;
	long l;

	unsigned char uc;
	unsigned short us;
	unsigned int ui;
	unsigned long ul;

	float f;
	double d;

	char byte[16];

	struct {
	   char* data;
	   unsigned long size;
	} bytearray;

	void* ptr;

	struct {
	    void *ptr;
	    bool owner;
	} voidstar;

	struct {
	    char *ptr;
	    bool owner;
	} charstar;

	struct {
	    char *ptr;
	    bool owner;
	} utf8;

	struct {
	    char *ptr;
	    bool owner;
	} local8bit;

	TQUnknownInterface* iface;
	TQDispatchInterface* idisp;

    } payload;

    TQUObject* deepCopy(TQUObject*);
    bool isLastObject;
};


// A parameter description describes one method parameters. A
// parameter has a name, a type and a flag describing whether it's an
// in parameter, an out parameter, or both ways
struct TQ_EXPORT TQUParameter
{
    const char* name;
    TQUType *type;
    const void* typeExtra; //Usually 0, UEnum* for TQUType_enum, const char* for TQUType_ptr, int* for TQUType_varptr
    enum { In = 1, Out = 2, InOut = In | Out };
    int inOut;
};

// A method description describes one method. A method has a name and
// an array of parameters.
struct TQ_EXPORT TQUMethod
{
    const char* name;
    int count;
    const TQUParameter* parameters;
};

// A Property description. Not used yet in the example.
struct TQ_EXPORT TQUProperty
{
    const char* name;
    TQUType* type;
    const void* typeExtra; //type dependend. Usually 0, but UEnum for TQUTypeenum or const char* for TQUTypeptr

    int set; // -1 undefined
    int get; // -1 undefined

    int designable; // -1 false, -2 true, else method
    int stored; // -1 false, -2 true, else method
};

// An interface description describes one interface, that is all its
// methods and properties.
struct TQ_EXPORT TQUInterfaceDescription
{
    int methodCount;
    const TQUMethod* methods;
    int propertyCount;
    const TQUProperty* properties;
};


// A component description describe one component, that is its name,
// vendor, release, info, its component uuid and all its interface
// uuids.
struct TQ_EXPORT TQUComponentDescription
{
    const char* name;
    const char* vendor;
    const char* release;
    const char* info;
    TQUuid cid;
    int count;
    const TQUuid* interfaces;
};


// A component server description describe one component server, that
// is its name, vendor, release, info and the descriptions of all
// components it can instantiate.
struct TQ_EXPORT TQUComponentServerDescription
{
    const char* name;
    const char* vendor;
    const char* release;
    const char* info;
    int count;
    const TQUComponentDescription* components;
};



struct TQ_EXPORT TQUEnumItem 				// - a name/value pair
{
    const char *key;
    int value;
};

struct TQ_EXPORT TQUEnum
{
    const char *name;			// - enumerator name
    unsigned int count;			// - number of values
    const TQUEnumItem *items;		// - the name/value pairs
    bool set;				// whether enum has to be treated as a set
};

inline bool TQUType::isEqual( const TQUType *t1, const TQUType *t2 ) {
    return t1 == t2 || t1->uuid() == t2->uuid() ||
	*(t1->uuid()) == *(t2->uuid());
}

inline bool TQUType::check( TQUObject* o, TQUType* t ) {
    return isEqual( o->type, t ) || t->convertFrom( o, o->type );
}



// {7EE17B08-5419-47e2-9776-8EEA112DCAEC}
extern TQ_EXPORT const TQUuid TID_QUType_enum;
struct TQ_EXPORT TQUType_enum : public TQUType
{
    const TQUuid *uuid() const;
    const char *desc() const;

    void set( TQUObject *, int );
    int &get( TQUObject * o ) { return o->payload.i; }
    bool canConvertFrom( TQUObject *, TQUType * );
    bool canConvertTo( TQUObject *, TQUType * );
    bool convertFrom( TQUObject *, TQUType * );
    bool convertTo( TQUObject *, TQUType * );
    void clear( TQUObject * ) {}
    int serializeTo( TQUObject *, TQUBuffer * );
    int serializeFrom( TQUObject *, TQUBuffer * );
};
extern TQ_EXPORT TQUType_enum static_QUType_enum;


// {8AC26448-5AB4-49eb-968C-8F30AB13D732}
extern TQ_EXPORT const TQUuid TID_QUType_ptr;
struct TQ_EXPORT TQUType_ptr : public TQUType
{
    const TQUuid *uuid() const;
    const char *desc() const;

    void set( TQUObject *, const void* );
    void* &get( TQUObject * o ) { return o->payload.ptr; }
    bool canConvertFrom( TQUObject *, TQUType * );
    bool canConvertTo( TQUObject *, TQUType * );
    bool convertFrom( TQUObject *, TQUType * );
    bool convertTo( TQUObject *, TQUType * );
    void clear( TQUObject * ) {}
    int serializeTo( TQUObject *, TQUBuffer * );
    int serializeFrom( TQUObject *, TQUBuffer * );
};
extern TQ_EXPORT TQUType_ptr static_QUType_ptr;

// {97A2594D-6496-4402-A11E-55AEF2D4D25C}
extern TQ_EXPORT const TQUuid TID_QUType_iface;
struct TQ_EXPORT TQUType_iface : public TQUType
{
    const TQUuid *uuid() const;
    const char *desc() const;

    void set( TQUObject *, TQUnknownInterface* );
    TQUnknownInterface* &get( TQUObject *o ){ return o->payload.iface; }
    bool canConvertFrom( TQUObject *, TQUType * );
    bool canConvertTo( TQUObject *, TQUType * );
    bool convertFrom( TQUObject *, TQUType * );
    bool convertTo( TQUObject *, TQUType * );
    void clear( TQUObject * ) {}
    int serializeTo( TQUObject *, TQUBuffer * );
    int serializeFrom( TQUObject *, TQUBuffer * );
};
extern TQ_EXPORT TQUType_iface static_QUType_iface;

// {2F358164-E28F-4bf4-9FA9-4E0CDCABA50B}
extern TQ_EXPORT const TQUuid TID_QUType_idisp;
struct TQ_EXPORT TQUType_idisp : public TQUType
{
    const TQUuid *uuid() const;
    const char *desc() const;

    void set( TQUObject *, TQDispatchInterface* );
    TQDispatchInterface* &get( TQUObject *o ){ return o->payload.idisp; }
    bool canConvertFrom( TQUObject *, TQUType * );
    bool canConvertTo( TQUObject *, TQUType * );
    bool convertFrom( TQUObject *, TQUType * );
    bool convertTo( TQUObject *, TQUType * );
    void clear( TQUObject * ) {}
    int serializeTo( TQUObject *, TQUBuffer * );
    int serializeFrom( TQUObject *, TQUBuffer * );
};
extern TQ_EXPORT TQUType_idisp static_QUType_idisp;

// {CA42115D-13D0-456c-82B5-FC10187F313E}
extern TQ_EXPORT const TQUuid TID_QUType_bool;
struct TQ_EXPORT TQUType_bool : public TQUType
{
    const TQUuid *uuid() const;
    const char *desc() const;

    void set( TQUObject *, bool );
    bool &get( TQUObject *o ) { return o->payload.b; }
    bool canConvertFrom( TQUObject *, TQUType * );
    bool canConvertTo( TQUObject *, TQUType * );
    bool convertFrom( TQUObject *, TQUType * );
    bool convertTo( TQUObject *, TQUType * );
    void clear( TQUObject * ) {}
    int serializeTo( TQUObject *, TQUBuffer * );
    int serializeFrom( TQUObject *, TQUBuffer * );
};
extern TQ_EXPORT TQUType_bool static_QUType_bool;

// {53C1F3BE-73C3-4c7d-9E05-CCF09EB676B5}
extern TQ_EXPORT const TQUuid TID_QUType_int;
struct TQ_EXPORT TQUType_int : public TQUType
{
    const TQUuid *uuid() const;
    const char *desc() const;

    void set( TQUObject *, int );
    int &get( TQUObject *o ) { return o->payload.i; }
    bool canConvertFrom( TQUObject *, TQUType * );
    bool canConvertTo( TQUObject *, TQUType * );
    bool convertFrom( TQUObject *, TQUType * );
    bool convertTo( TQUObject *, TQUType * );
    void clear( TQUObject * ) {}
    int serializeTo( TQUObject *, TQUBuffer * );
    int serializeFrom( TQUObject *, TQUBuffer * );
};
extern TQ_EXPORT TQUType_int static_QUType_int;


// {2D0974E5-0BA6-4ec2-8837-C198972CB48C}
extern TQ_EXPORT const TQUuid TID_QUType_double;
struct TQ_EXPORT TQUType_double : public TQUType
{
    const TQUuid *uuid() const;
    const char *desc() const;

    void set( TQUObject *, double );
    double &get( TQUObject *o ) { return o->payload.d; }
    bool canConvertFrom( TQUObject *, TQUType * );
    bool canConvertTo( TQUObject *, TQUType * );
    bool convertFrom( TQUObject *, TQUType * );
    bool convertTo( TQUObject *, TQUType * );
    void clear( TQUObject * ) {}
    int serializeTo( TQUObject *, TQUBuffer * );
    int serializeFrom( TQUObject *, TQUBuffer * );
};
extern TQ_EXPORT TQUType_double static_QUType_double;


// {EFCDD1D4-77A3-4b8e-8D46-DC14B8D393E9}
extern TQ_EXPORT const TQUuid TID_QUType_charstar;
struct TQ_EXPORT TQUType_charstar : public TQUType
{
    const TQUuid *uuid() const;
    const char *desc() const;

    void set( TQUObject *, const char*, bool take = false );
    char* get( TQUObject *o ){ return o->payload.charstar.ptr; }
    bool canConvertFrom( TQUObject *, TQUType * );
    bool canConvertTo( TQUObject *, TQUType * );
    bool convertFrom( TQUObject *, TQUType * );
    bool convertTo( TQUObject *, TQUType * );
    void clear( TQUObject * );
    int serializeTo( TQUObject *, TQUBuffer * );
    int serializeFrom( TQUObject *, TQUBuffer * );

};
extern TQ_EXPORT TQUType_charstar static_QUType_charstar;

// {44C2A547-01E7-4e56-8559-35AF9D2F42B7}
extern const TQUuid TID_QUType_TQString;

struct TQ_EXPORT TQUType_TQString : public TQUType
{
    const TQUuid *uuid() const;
    const char *desc() const;

    void set( TQUObject *, const TQString & );
    TQString &get( TQUObject * o ) { return *(TQString*)o->payload.ptr; }

    bool canConvertFrom( TQUObject *, TQUType * );
    bool canConvertTo( TQUObject *, TQUType * );
    bool convertFrom( TQUObject *, TQUType * );
    bool convertTo( TQUObject *, TQUType * );
    void clear( TQUObject * );
    int serializeTo( TQUObject *, TQUBuffer * );
    int serializeFrom( TQUObject *, TQUBuffer * );

};
extern TQ_EXPORT TQUType_TQString static_QUType_TQString;


#endif // TQUCOM_P_H
