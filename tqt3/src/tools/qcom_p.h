/****************************************************************************
**
** ...
**
** Copyright (C) 2001-2008 Trolltech ASA.  All rights reserved.
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

#ifndef TQCOM_P_H
#define TQCOM_P_H

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
#include "ntqstringlist.h"
#include "ntquuid.h"
#endif // QT_H

#if __GNUC__ - 0 > 3
#pragma GCC system_header
#endif

#ifndef TQT_NO_COMPONENT

class TQObject;
struct TQUInterfaceDescription;
struct TQUObject;

#define TQRESULT		unsigned long
#define TQS_OK		(TQRESULT)0x00000000
#define TQS_FALSE	(TQRESULT)0x00000001

#define TQE_NOTIMPL      (TQRESULT)0x80000001
#define TQE_OUTOFMEMORY  (TQRESULT)0x80000002
#define TQE_INVALIDARG	(TQRESULT)0x80000003
#define TQE_NOINTERFACE	(TQRESULT)0x80000004
#define TQE_NOCOMPONENT	(TQRESULT)0x80000005


// {1D8518CD-E8F5-4366-99E8-879FD7E482DE}
#ifndef IID_QUnknown
#define IID_QUnknown TQUuid(0x1d8518cd, 0xe8f5, 0x4366, 0x99, 0xe8, 0x87, 0x9f, 0xd7, 0xe4, 0x82, 0xde)
#endif

struct TQ_EXPORT TQUnknownInterface
{
    virtual TQRESULT queryInterface( const TQUuid&, TQUnknownInterface** ) = 0;
    virtual ulong   addRef() = 0;
    virtual ulong   release() = 0;
};

// {FBAC965E-A441-413F-935E-CDF582573FAB}
#ifndef IID_QDispatch
#define IID_QDispatch TQUuid( 0xfbac965e, 0xa441, 0x413f, 0x93, 0x5e, 0xcd, 0xf5, 0x82, 0x57, 0x3f, 0xab)
#endif

// the dispatch interface that inherits the unknown interface.. It is
// used to explore interfaces during runtime and to do dynamic calls.
struct TQ_EXPORT TQDispatchInterface : public TQUnknownInterface
{
    // returns the interface description of this dispatch interface.
    virtual const TQUInterfaceDescription* interfaceDescription() const = 0;

    // returns the event description of this dispatch interface.
    virtual const TQUInterfaceDescription* eventsDescription() const = 0;

    // invokes method id with parameters V*. Returns some sort of
    // exception code.
    virtual TQRESULT invoke( int id, TQUObject* o ) = 0;

    // installs listener as event listener
    virtual void installListener( TQDispatchInterface* listener ) = 0;

    // remove listener as event listener
    virtual void removeListener( TQDispatchInterface* listener ) = 0;
};

template <class T>
class TQInterfacePtr
{
public:
    TQInterfacePtr():iface(0){}

    TQInterfacePtr( T* i) {
	if ( (iface = i) )
	    iface->addRef();
    }

    TQInterfacePtr(const TQInterfacePtr<T> &p) {
	if ( (iface = p.iface) )
	    iface->addRef();
    }

    ~TQInterfacePtr() {
	if ( iface )
	    iface->release();
    }

    TQInterfacePtr<T> &operator=(const TQInterfacePtr<T> &p) {
	if ( iface != p.iface ) {
	    if ( iface )
		iface->release();
	    if ( (iface = p.iface) )
		iface->addRef();
	}
	return *this;
    }

    TQInterfacePtr<T> &operator=(T* i) {
	if (iface != i ) {
	    if ( iface )
		iface->release();
	    if ( (iface = i) )
		iface->addRef();
	}
	return *this;
    }

    bool operator==( const TQInterfacePtr<T> &p ) const { return iface == p.iface; }

    bool operator!= ( const TQInterfacePtr<T>& p ) const {  return !( *this == p ); }

    bool isNull() const { return !iface; }

    T* operator->() const { return iface; }

    T& operator*() const { return *iface; }

    operator T*() const { return iface; }

    TQUnknownInterface** operator &() const {
	if( iface )
	    iface->release();
	return (TQUnknownInterface**)&iface;
    }

    T** operator &() {
	if ( iface )
	    iface->release();
	return &iface;
    }

private:
    T* iface;
};

// {10A1501B-4C5F-4914-95DD-C400486CF900}
#ifndef IID_QObject
#define IID_QObject TQUuid( 0x10a1501b, 0x4c5f, 0x4914, 0x95, 0xdd, 0xc4, 0x00, 0x48, 0x6c, 0xf9, 0x00)
#endif

struct TQ_EXPORT TQObjectInterface
{
    virtual TQObject*   qObject() = 0;
};

// {5F3968A5-F451-45b1-96FB-061AD98F926E}
#ifndef IID_QComponentInformation
#define IID_QComponentInformation TQUuid(0x5f3968a5, 0xf451, 0x45b1, 0x96, 0xfb, 0x6, 0x1a, 0xd9, 0x8f, 0x92, 0x6e)
#endif

struct TQ_EXPORT TQComponentInformationInterface : public TQUnknownInterface
{
    virtual TQString name() const = 0;
    virtual TQString description() const = 0;
    virtual TQString author() const = 0;
    virtual TQString version() const = 0;
};

// {6CAA771B-17BB-4988-9E78-BA5CDDAAC31E}
#ifndef IID_QComponentFactory
#define IID_QComponentFactory TQUuid( 0x6caa771b, 0x17bb, 0x4988, 0x9e, 0x78, 0xba, 0x5c, 0xdd, 0xaa, 0xc3, 0x1e)
#endif

struct TQ_EXPORT TQComponentFactoryInterface : public TQUnknownInterface
{
    virtual TQRESULT createInstance( const TQUuid &cid, const TQUuid &iid, TQUnknownInterface** instance, TQUnknownInterface *outer ) = 0;
};

// {D16111D4-E1E7-4C47-8599-24483DAE2E07}
#ifndef IID_QLibrary
#define IID_QLibrary TQUuid( 0xd16111d4, 0xe1e7, 0x4c47, 0x85, 0x99, 0x24, 0x48, 0x3d, 0xae, 0x2e, 0x07)
#endif

struct TQ_EXPORT TQLibraryInterface : public TQUnknownInterface
{
    virtual bool    init() = 0;
    virtual void    cleanup() = 0;
    virtual bool    canUnload() const = 0;
};

// {3F8FDC44-3015-4f3e-B6D6-E4AAAABDEAAD}
#ifndef IID_QFeatureList
#define IID_QFeatureList TQUuid(0x3f8fdc44, 0x3015, 0x4f3e, 0xb6, 0xd6, 0xe4, 0xaa, 0xaa, 0xbd, 0xea, 0xad)
#endif

struct TQ_EXPORT TQFeatureListInterface : public TQUnknownInterface
{
    virtual TQStringList	featureList() const = 0;
};

// {B5FEB5DE-E0CD-4E37-B0EB-8A812499A0C1}
#ifndef IID_QComponentRegistration
#define IID_QComponentRegistration TQUuid( 0xb5feb5de, 0xe0cd, 0x4e37, 0xb0, 0xeb, 0x8a, 0x81, 0x24, 0x99, 0xa0, 0xc1)
#endif

struct TQ_EXPORT TQComponentRegistrationInterface : public TQUnknownInterface
{
    virtual bool    registerComponents( const TQString &filepath ) const = 0;
    virtual bool    unregisterComponents() const = 0;
};

// internal class that wraps an initialized ulong
struct TQ_EXPORT TQtULong
{
    TQtULong() : ref( 0 ) { }
    operator unsigned long () const { return ref; }
    unsigned long& operator++() { return ++ref; }
    unsigned long operator++( int ) { return ref++; }
    unsigned long& operator--() { return --ref; }
    unsigned long operator--( int ) { return ref--; }

    unsigned long ref;
};
// default implementation of ref counting. A variable "ulong ref" has to be a member


#define TQ_REFCOUNT \
private:	   \
    TQtULong qtrefcount;   \
public:		   \
    ulong addRef() {return qtrefcount++;} \
    ulong release() {if(!--qtrefcount){delete this;return 0;}return qtrefcount;}

#ifndef TQ_EXPORT_COMPONENT
#if defined(TQT_THREAD_SUPPORT)
#define QT_THREADED_BUILD 1
#define Q_UCM_FLAGS_STRING "11"
#else
#define QT_THREADED_BUILD 0
#define Q_UCM_FLAGS_STRING "01"
#endif

#ifndef TQ_EXTERN_C
#ifdef __cplusplus
#define TQ_EXTERN_C    extern "C"
#else
#define TQ_EXTERN_C    extern
#endif
#endif

// this is duplicated at Q_PLUGIN_VERIFICATION_DATA in ntqgplugin.h
// NOTE: if you change pattern, you MUST change the pattern in
// qcomlibrary.cpp as well.  changing the pattern will break all
// backwards compatibility as well (no old plugins will be loaded).
#ifndef Q_UCM_VERIFICATION_DATA
#  define Q_UCM_VERIFICATION_DATA \
	static const char *qt_ucm_verification_data =			\
            "pattern=" "QT_UCM_VERIFICATION_DATA" "\n"			\
            "version=" TQT_VERSION_STR "\n"				\
            "flags=" Q_UCM_FLAGS_STRING "\n"				\
	    "buildkey=" TQT_BUILD_KEY "\0";
#endif // Q_UCM_VERIFICATION_DATA

// This macro expands to the default implementation of ucm_instantiate.
#ifndef Q_CREATE_INSTANCE
#    define Q_CREATE_INSTANCE( IMPLEMENTATION )		\
	IMPLEMENTATION *i = new IMPLEMENTATION;		\
	TQUnknownInterface* iface = 0; 			\
	i->queryInterface( IID_QUnknown, &iface );	\
	return iface;
#endif // Q_CREATE_INSTANCE

#    ifdef TQ_WS_WIN
#	    define TQ_EXPORT_COMPONENT() \
	        Q_UCM_VERIFICATION_DATA \
		TQ_EXTERN_C __declspec(dllexport) \
                const char *qt_ucm_query_verification_data() \
                { return qt_ucm_verification_data; } \
		TQ_EXTERN_C TQ_EXPORT __declspec(dllexport) TQUnknownInterface* ucm_instantiate()
#    else
#	define TQ_EXPORT_COMPONENT() \
	    Q_UCM_VERIFICATION_DATA \
	    TQ_EXTERN_C \
            const char *qt_ucm_query_verification_data() \
            { return qt_ucm_verification_data; } \
	    TQ_EXTERN_C TQ_EXPORT TQUnknownInterface* ucm_instantiate()
#    endif
#    define TQ_EXPORT_INTERFACE() TQ_EXPORT_COMPONENT()
#endif

#endif //TQT_NO_COMPONENT

#endif //TQCOM_P_H
