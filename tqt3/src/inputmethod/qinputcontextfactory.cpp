/****************************************************************************
** $Id: qinputcontextfactory.cpp,v 1.2 2004/06/20 18:43:11 daisuke Exp $
**
** Implementation of TQInputContextFactory class
**
** Created : 001103
**
** Copyright (C) 1992-2002 Trolltech AS.  All rights reserved.
**
** This file is part of the widgets module of the TQt GUI Toolkit.
**
** This file may be distributed under the terms of the Q Public License
** as defined by Trolltech AS of Norway and appearing in the file
** LICENSE.TQPL included in the packaging of this file.
**
** This file may be distributed and/or modified under the terms of the
** GNU General Public License version 2 as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL included in the
** packaging of this file.
**
** Licensees holding valid TQt Enterprise Edition licenses may use this
** file in accordance with the TQt Commercial License Agreement provided
** with the Software.
**
** This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE
** WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
**
** See http://www.trolltech.com/pricing.html or email sales@trolltech.com for
**   information about TQt Commercial License Agreements.
** See http://www.trolltech.com/qpl/ for TQPL licensing information.
** See http://www.trolltech.com/gpl/ for GPL licensing information.
**
** Contact info@trolltech.com if any conditions of this licensing are
** not clear to you.
**
**********************************************************************/

#include "qinputcontextinterface_p.h" // up here for GCC 2.7.* compatibility
#include "ntqinputcontextfactory.h"
#include "ntqinputcontext.h"

#ifndef TQT_NO_IM

#include "ntqapplication.h"

#ifdef TQT_THREAD_SUPPORT
#include <private/qmutexpool_p.h>
#endif // TQT_THREAD_SUPPORT

#include <stdlib.h>

#include "ntqcleanuphandler.h"
#include <private/qpluginmanager_p.h>
#ifndef TQT_NO_COMPONENT


static TQPluginManager<TQInputContextFactoryInterface> *manager = 0;
static TQSingleCleanupHandler< TQPluginManager<TQInputContextFactoryInterface> > cleanup_manager;

static void create_manager()
{
    if( manager ) // already created
	return;

#ifdef TQT_THREAD_SUPPORT
    // protect manager creation
    TQMutexLocker locker( tqt_global_mutexpool ?
			 tqt_global_mutexpool->get( &manager ) : 0);

    // we check the manager pointer again to make sure that another thread
    // has not created the manager before us.

    if ( manager ) // already created
        return;
#endif

    manager = new TQPluginManager<TQInputContextFactoryInterface>( IID_QInputContextFactory, TQApplication::libraryPaths(), "/inputmethods", false );

    TQ_CHECK_PTR( manager );
    cleanup_manager.set( &manager );
}

#endif //TQT_NO_COMPONENT


/*!
    This function generates the input context that has the identifier
    name which is in agreement with \a key. \a widget is the client
    widget of TQInputContext. \a widget may be null.
*/
TQInputContext *TQInputContextFactory::create( const TQString& key, TQWidget *widget )
{
    TQInputContext *ret = 0;
    TQString inputcontext = key;
#ifndef TQT_NO_COMPONENT
    // make sure the manager is created
    create_manager();

    TQInterfacePtr<TQInputContextFactoryInterface> iface;
    manager->queryInterface( inputcontext, &iface );

    if ( iface ) {
	ret = iface->create( inputcontext );
#ifdef TQ_WS_X11
	if ( ret )
	    ret->setHolderWidget( widget );
#endif
    }
#endif
    return ret;
}


/*!
    This function returns the list of the names input methods.
    Only input methods included in default and placed under
    $TQTDIR/plugins/inputmethods are listed.
*/
TQStringList TQInputContextFactory::keys()
{
    TQStringList list;
#ifndef TQT_NO_COMPONENT
    // make sure the manager is created
    create_manager();

    list = manager->featureList();
#endif //TQT_NO_COMPONENT

    return list;
}


TQStringList TQInputContextFactory::languages( const TQString &key )
{
    TQStringList result;
#ifndef TQT_NO_COMPONENT
    // make sure the manager is created
    create_manager();

    TQInterfacePtr<TQInputContextFactoryInterface> iface;
    manager->queryInterface( key, &iface );

    if ( iface )
	result = iface->languages( key );
#endif //TQT_NO_COMPONENT

    return result;
}


TQString TQInputContextFactory::displayName( const TQString &key )
{
    TQString result( "" );
#ifndef TQT_NO_COMPONENT
    // make sure the manager is created
    create_manager();

    TQInterfacePtr<TQInputContextFactoryInterface> iface;
    manager->queryInterface( key, &iface );

    if ( iface )
	result = iface->displayName( key );
#endif //TQT_NO_COMPONENT

    return result;
}


TQString TQInputContextFactory::description( const TQString &key )
{
    TQString result( "" );
#ifndef TQT_NO_COMPONENT
    // make sure the manager is created
    create_manager();

    TQInterfacePtr<TQInputContextFactoryInterface> iface;
    manager->queryInterface( key, &iface );

    if ( iface )
	result = iface->description( key );
#endif //TQT_NO_COMPONENT

    return result;
}

#endif // TQT_NO_IM
