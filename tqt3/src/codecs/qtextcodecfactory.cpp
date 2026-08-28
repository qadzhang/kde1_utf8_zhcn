/****************************************************************************
**
** Implementation of TQTextCodecFactory class
**
** Created : 010130
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

#include "ntqtextcodecfactory.h"

#ifndef TQT_NO_TEXTCODEC

#ifndef TQT_NO_COMPONENT
#include "ntqapplication.h"
#include "ntqcleanuphandler.h"
#include <private/qpluginmanager_p.h>
#include "qtextcodecinterface_p.h"

#ifdef TQT_THREAD_SUPPORT
#  include <private/qmutexpool_p.h>
#endif // TQT_THREAD_SUPPORT

#include <stdlib.h>


static TQPluginManager<TQTextCodecFactoryInterface> *manager = 0;
static TQSingleCleanupHandler< TQPluginManager<TQTextCodecFactoryInterface> > cleanup_manager;

static void create_manager()
{
    if ( manager ) // already created
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

    manager =
	new TQPluginManager<TQTextCodecFactoryInterface>(IID_QTextCodecFactory,
						       TQApplication::libraryPaths(), "/codecs",
						       false);
    TQ_CHECK_PTR( manager );
    cleanup_manager.set( &manager );
}

#endif // TQT_NO_COMPONENT


TQTextCodec *TQTextCodecFactory::createForName(const TQString &name)
{
    TQTextCodec *codec = 0;

#ifndef TQT_NO_COMPONENT

    // make sure the manager is created
    create_manager();

    TQInterfacePtr<TQTextCodecFactoryInterface> iface;
    manager->queryInterface(name, &iface );

    if (iface)
	codec = iface->createForName(name);

#endif // TQT_NO_COMPONENT

    return codec;
}


TQTextCodec *TQTextCodecFactory::createForMib(int mib)
{
    TQTextCodec *codec = 0;

#ifndef TQT_NO_COMPONENT

    // make sure the manager is created
    create_manager();

    TQInterfacePtr<TQTextCodecFactoryInterface> iface;
    manager->queryInterface("MIB-" + TQString::number(mib), &iface );

    if (iface)
	codec = iface->createForMib(mib);

#endif // TQT_NO_COMPONENT

    return codec;
}


#endif // TQT_NO_TEXTCODEC
