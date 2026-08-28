/****************************************************************************
**
** Macros and definitions related to TQObject
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

#ifndef TQOBJECTDEFS_H
#define TQOBJECTDEFS_H

#ifndef QT_H
#include "ntqglobal.h"
#endif // QT_H


#ifndef TQT_NO_TRANSLATION
# ifndef TQT_NO_TEXTCODEC
// full set of tr functions
#  define TQT_TR_FUNCTIONS \
    static TQString tr( const char *, const char * = 0 ); \
    static TQString trUtf8( const char *, const char * = 0 );
# else
// no TQTextCodec, no utf8
#  define TQT_TR_FUNCTIONS \
    static TQString tr( const char *, const char * = 0 );
# endif
#else
// inherit the ones from TQObject
# define TQT_TR_FUNCTIONS
#endif

#ifndef TQT_NO_PROPERTIES
# define QT_PROP_FUNCTIONS \
    virtual bool tqt_property( int id, int f, TQVariant* v); \
    static bool tqt_static_property( TQObject* , int, int, TQVariant* );
#else
# define QT_PROP_FUNCTIONS
#endif

// The following macros are our "extensions" to C++
// They are used, strictly speaking, only by the moc.
struct TQUObject;

#ifdef QT_MOC_CPP
#define slots			    slots
#define signals		    signals
#define TQ_CLASSINFO( name, value ) TQ_CLASSINFO( name, value )
#define TQ_PROPERTY( text )	    TQ_PROPERTY( text )
#define TQ_OVERRIDE( text )	    TQ_OVERRIDE( text )
#define TQ_ENUMS( x )		    TQ_ENUMS( x )
#define TQ_SETS( x )		    TQ_SETS( x )
 /* tmake ignore TQ_OBJECT */
#define TQ_OBJECT		    TQ_OBJECT
 /* tmake ignore TQ_OBJECT */
#define TQ_OBJECT_FAKE		    TQ_OBJECT_FAKE

#else
#define slots					// slots: in class
#define signals protected			// signals: in class
#ifndef TQT_NO_EMIT
#define emit					// emit signal
#endif
#define TQ_CLASSINFO( name, value )		// class info
#define TQ_PROPERTY( text )			// property
#define TQ_OVERRIDE( text )			// override property
#define TQ_ENUMS( x )
#define TQ_SETS( x )

/* tmake ignore TQ_OBJECT */
#define TQ_OBJECT							\
public:									\
    virtual TQMetaObject *metaObject() const { 				\
         return staticMetaObject();					\
    }									\
    virtual const char *className() const;				\
    virtual void* tqt_cast( const char* ); 				\
    virtual bool tqt_invoke( int, TQUObject* ); 				\
    virtual bool tqt_emit( int, TQUObject* ); 				\
    QT_PROP_FUNCTIONS							\
    static TQMetaObject* staticMetaObject();				\
    TQObject* qObject() { return (TQObject*)this; } 			\
    TQT_TR_FUNCTIONS							\
private:								\
    static TQMetaObject *metaObj;

/* tmake ignore TQ_OBJECT */
#define TQ_OBJECT_FAKE TQ_OBJECT

#endif

// macro for naming members
#ifdef TQ_METHOD
#undef TQ_METHOD
#endif
#ifdef TQ_SLOT
#undef TQ_SLOT
#endif
#ifdef TQ_SIGNAL
#undef TQ_SIGNAL
#endif

#if defined(_OLD_CPP_)
#define TQ_METHOD(a)	"0""a"
#define TQ_SLOT(a)		"1""a"
#define TQ_SIGNAL(a)	"2""a"
#else
#define TQ_METHOD(a)	"0"#a
#define TQ_SLOT(a)		"1"#a
#define TQ_SIGNAL(a)	"2"#a
#endif

#define TQ_METHOD_CODE	0			// member type codes
#define TQ_SLOT_CODE	1
#define TQ_SIGNAL_CODE	2

class TQObject;
class TQMetaObject;
class TQSignal;
class TQConnection;
class TQEvent;
class TQMutex;
struct TQMetaData;
class TQConnectionList;
class TQConnectionListIt;
class TQSignalVec;
class TQObjectList;
class TQObjectListIt;
class TQMemberDict;

extern TQMutex *tqt_sharedMetaObjectMutex;

TQ_EXPORT void *tqt_find_obj_child( TQObject *, const char *, const char * );
#define Q_CHILD(parent,type,name) \
	((type*)tqt_find_obj_child(parent,#type,name))

TQ_EXPORT void *tqt_inheritedBy( TQMetaObject *super, const TQObject *cls );

template <typename T>
T tqt_cast(const TQObject *object)
{ return (T)tqt_inheritedBy( ((T)0)->staticMetaObject(), object ); }
#endif // TQOBJECTDEFS_H
