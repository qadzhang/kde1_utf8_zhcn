/****************************************************************************
**
** Implementation of the TQComponentFactory class
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

#include "qcomponentfactory_p.h"

#ifndef TQT_NO_COMPONENT
#include "ntqsettings.h"
#include <private/qcomlibrary_p.h>
#include "ntqdir.h"
#include "ntqapplication.h"

/*!
  \class TQComponentFactory qcomponentfactory.h
  \brief The TQComponentFactory class provides static functions to create and register components.

    \internal

  The static convenience functions can be used both by applications to instantiate components,
  and by component servers to register components.

  The createInstance() function provides a pointer to an interface implemented in a specific
  component if the component requested has been installed properly and implements the interface.

  Use registerServer() to load a component server and register its components, and unregisterServer()
  to unregister the components. The component exported by the component server has to implement the
  TQComponentRegistrationInterface.

  The static functions registerComponent() and unregisterComponent() register and unregister a single
  component in the system component registry, and should be used when implementing the
  \link TQComponentRegistrationInterface::registerComponents() registerCompontents() \endlink and
  \link TQComponentRegistrationInterface::unregisterComponents() unregisterCompontents() \endlink functions
  in the TQComponentRegistrationInterface.

  A component is registered using a UUID, but can additionally be registered with a name, version and
  description. A component registered with a name and a version can be instantiated by client applications
  using the name and specific version number, or the highest available version number for that component by
  just using the name. A component that is registered calling

  \code
  TQComponentFactory::registerComponent( TQUuid(...), filename, "MyProgram.Component", 1 );
  \endcode

  can be instantiated calling either:

  \code
  TQComponentFactory::createInstance( TQUuid(...), IID_XYZ, (TQUnknownInterface**)&iface );
  \endcode
  or
  \code
  TQComponentFactory::createInstance( "MyProgram.Component", IID_XYZ, (TQUnknownInterface**)&iface );
  \endcode
  or
  \code
  TQComponentFactory::createInstance( "MyProgram.Component.1", IID_XYZ, (TQUnknownInterface**)&iface );
  \endcode

  The first and the last way will always instantiate exactly the component registered above, while
  the second call might also return a later version of the same component. This allows smoother upgrading
  of components, and is easier to use in application source code, but should only be used when new versions
  of the component are guaranteed to work with the application.

  The component name can be anything, but should be unique on the system the component is being
  installed on. A common naming convention for components is \e application.component.

  \sa TQComponentRegistrationInterface TQComponentFactoryInterface
*/


static TQPtrList<TQComLibrary> *libraries = 0;

static void cleanup()
{
    delete libraries;
    libraries = 0;
}

static TQPtrList<TQComLibrary> *liblist()
{
    if ( !libraries ) {
	libraries = new TQPtrList<TQComLibrary>();
	libraries->setAutoDelete( true );
	tqAddPostRoutine( cleanup );
    }
    return libraries;
}

/*!
  Searches for the component identifier \a cid in the system component registry,
  loads the corresponding component server and queries for the interface \a iid.
  \a iface is set to the resulting interface pointer. \a cid can either be the
  UUID or the name of the component.

  The parameter \a outer is a pointer to the outer interface used
  for containment and aggregation and is propagated to the \link
  TQComponentFactoryInterface::createInstance() createInstance() \endlink
  implementation of the TQComponentFactoryInterface in the component server if
  provided.

  The function returns TQS_OK if the interface was successfully instantiated, TQE_NOINTERFACE if
  the component does not provide an interface \a iid, or TQE_NOCOMPONENT if there was
  an error loading the component.

  Example:
  \code
  TQInterfacePtr<MyInterface> iface;
  if ( TQComponentFactory::createInstance( IID_MyInterface, CID_MyComponent, (TQUnknownInterface**)&iface ) == TQS_OK )
      iface->doSomething();
      ...
  }
  \endcode
*/
TQRESULT TQComponentFactory::createInstance( const TQString &cid, const TQUuid &iid, TQUnknownInterface** iface, TQUnknownInterface *outer )
{
    TQSettings settings;
    settings.insertSearchPath( TQSettings::Windows, "/Classes" );
    bool ok = false;
    TQString cidStr = cid;
    TQRESULT res = TQE_NOCOMPONENT;

    TQUuid uuid( cidStr ); // try to parse, and resolve CLSID if necessary
    if ( uuid.isNull() ) {
	uuid = settings.readEntry( "/" + cid + "/CLSID/Default", TQString::null, &ok );
	cidStr = uuid.toString().upper();
    }

    if ( cidStr.isEmpty() )
	return res;

    TQString file = settings.readEntry( "/CLSID/" + cidStr + "/InprocServer32/Default", TQString::null, &ok );
    if ( !ok )
	return res;

    TQComLibrary *library = new TQComLibrary( file );
    library->setAutoUnload( false );

    TQComponentFactoryInterface *cfIface =0;
    library->queryInterface( IID_QComponentFactory, (TQUnknownInterface**)&cfIface );

    if ( cfIface ) {
	res = cfIface->createInstance( uuid, iid, iface, outer );
	cfIface->release();
    } else {
	res = library->queryInterface( iid, iface );
    }
    TQLibraryInterface *libiface = 0;
    if ( library->queryInterface( IID_QLibrary, (TQUnknownInterface**)&libiface ) != TQS_OK || !tqApp ) {
	delete library; // only deletes the object, thanks to TQLibrary::Manual
    } else {
	libiface->release();
	library->setAutoUnload( true );
	liblist()->prepend( library );
    }
    return res;
}

/*!
  Loads the shared library \a filename and queries for a
  TQComponentRegistrationInterface. If the library implements this interface,
  the \link TQComponentRegistrationInterface::registerComponents()
  registerComponents() \endlink function is called.

  Returns true if the interface is found and successfully called,
  otherwise returns false.
*/
TQRESULT TQComponentFactory::registerServer( const TQString &filename )
{
    TQComLibrary lib( filename );
    lib.load();
    TQComponentRegistrationInterface *iface = 0;
    TQRESULT res = lib.queryInterface( IID_QComponentRegistration, (TQUnknownInterface**)&iface );
    if ( res != TQS_OK )
	return res;
    TQDir dir( filename );
    bool ok = iface->registerComponents( dir.absPath() );
    iface->release();
    return ok ? TQS_OK : TQS_FALSE;
}

/*!
  Loads the shared library \a filename and queries for a
  TQComponentRegistrationInterface. If the library implements this interface,
  the \link TQComponentRegistrationInterface::unregisterComponents()
  unregisterComponents() \endlink function is called.

  Returns true if the interface is found and successfully unregistered,
  otherwise returns false.
*/
TQRESULT TQComponentFactory::unregisterServer( const TQString &filename )
{
    TQComLibrary lib( filename );
    lib.load();
    TQComponentRegistrationInterface *iface = 0;
    TQRESULT res = lib.queryInterface( IID_QComponentRegistration, (TQUnknownInterface**)&iface );
    if ( res != TQS_OK )
	return res;
    bool ok = iface->unregisterComponents();
    iface->release();
    return ok ? TQS_OK : TQS_FALSE;
}

/*!
  Registers the component with id \a cid in the system component registry and
  returns true if the component was registerd successfully, otherwise returns
  false. The component is provided by the component server at \a filepath and
  registered with an optional \a name, \a version and \a description.

  This function does nothing and returns false if a component with an identical
  \a cid does already exist on the system.

  A component that has been registered with a \a name can be created using both the
  \a cid and the \a name value using createInstance().

  Call this function for each component in an implementation of
  \link TQComponentRegistrationInterface::registerComponents() registerComponents() \endlink.

  \sa unregisterComponent(), registerServer(), createInstance()
*/
bool TQComponentFactory::registerComponent( const TQUuid &cid, const TQString &filepath, const TQString &name, int version, const TQString &description )
{
    bool ok = false;
    TQSettings settings;
    settings.insertSearchPath( TQSettings::Windows, "/Classes" );

    TQString cidStr = cid.toString().upper();
    settings.readEntry( "/CLSID/" + cidStr + "/InprocServer32/Default", TQString::null, &ok );
    if ( ok ) // don't overwrite existing component
	return false;

    ok = settings.writeEntry( "/CLSID/" + cidStr + "/InprocServer32/Default", filepath );
    if ( ok && !!description )
	settings.writeEntry( "/CLSID/" + cidStr + "/Default", description );

    // register the human readable part
    if ( ok && !!name ) {
	TQString vName = version ? name + "." + TQString::number( version ) : name;
	settings.writeEntry( "/CLSID/" + cidStr + "/ProgID/Default", vName );
	ok = settings.writeEntry( "/" + vName + "/CLSID/Default", cidStr );
	if ( ok && !!description )
	    settings.writeEntry( "/" + vName + "/Default", description );

	if ( ok && version ) {
	    settings.writeEntry( "/CLSID/" + cidStr + "/VersionIndependentProgID/Default", name );
	    TQString curVer = settings.readEntry( "/" + name + "/CurVer/Default" );
	    if ( !curVer || curVer < vName ) { // no previous, or a lesser version installed
		settings.writeEntry( "/" + name + "/CurVer/Default", vName );
		ok = settings.writeEntry( "/" + name + "/CLSID/Default", cidStr );
		if ( ok && !!description )
		    settings.writeEntry( "/" + name + "/Default", description );
	    }
	}
    }

    return ok;
}

/*!
  Unregisters the component with id \a cid from the system component registry and returns
  true if the component was unregistered successfully, otherwise returns false.

  Call this function for each component in an implementation of
  \link TQComponentRegistrationInterface::unregisterComponents() unregisterComponents() \endlink.

  \sa registerComponent(), unregisterServer()
*/
bool TQComponentFactory::unregisterComponent( const TQUuid &cid )
{
    TQSettings settings;
    bool ok = false;
    settings.insertSearchPath( TQSettings::Windows, "/Classes" );

    TQString cidStr = cid.toString().upper();
    if ( cidStr.isEmpty() )
	return false;

    // unregister the human readable part
    TQString vName = settings.readEntry( "/CLSID/" + cidStr + "/ProgID/Default", TQString::null, &ok );
    if ( ok ) {
	TQString name = settings.readEntry( "/CLSID/" + cidStr + "/VersionIndependentProgID/Default", TQString::null );
	if ( !!name && settings.readEntry( "/" + name + "/CurVer/Default" ) == vName ) {
	    // unregistering the current version -> change CurVer to previous version
	    TQString version = vName.right( vName.length() - name.length() - 1 );
	    TQString newVerName;
	    TQString newCidStr;
	    if ( version.find( '.' ) == -1 ) {
		int ver = version.toInt();
		// see if a lesser version is installed, and make that the CurVer
		while ( ver-- ) {
		    newVerName = name + "." + TQString::number( ver );
		    newCidStr = settings.readEntry( "/" + newVerName + "/CLSID/Default" );
		    if ( !!newCidStr )
			break;
		}
	    } else {
		// oh well...
	    }
	    if ( !!newCidStr ) {
		settings.writeEntry( "/" + name + "/CurVer/Default", newVerName );
		settings.writeEntry( "/" + name + "/CLSID/Default", newCidStr );
	    } else {
		settings.removeEntry( "/" + name + "/CurVer/Default" );
		settings.removeEntry( "/" + name + "/CLSID/Default" );
		settings.removeEntry( "/" + name + "/Default" );
	    }
	}

	settings.removeEntry( "/" + vName + "/CLSID/Default" );
	settings.removeEntry( "/" + vName + "/Default" );
    }

    settings.removeEntry( "/CLSID/" + cidStr + "/VersionIndependentProgID/Default" );
    settings.removeEntry( "/CLSID/" + cidStr + "/ProgID/Default" );
    settings.removeEntry( "/CLSID/" + cidStr + "/InprocServer32/Default" );
    ok = settings.removeEntry( "/CLSID/" + cidStr + "/Default" );

    return ok;
}

#endif // TQT_NO_COMPONENT
