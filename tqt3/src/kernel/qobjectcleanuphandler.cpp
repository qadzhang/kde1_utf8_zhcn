/****************************************************************************
**
** Implementation of TQObjectCleanupHandler class
**
** Copyright (C) 2000-2008 Trolltech ASA.  All rights reserved.
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

#include "ntqobjectcleanuphandler.h"
#include "ntqobjectlist.h"

/*!
    \class TQObjectCleanupHandler ntqobjectcleanuphandler.h
    \brief The TQObjectCleanupHandler class watches the lifetime of multiple TQObjects.

    \ingroup objectmodel

    A TQObjectCleanupHandler is useful whenever you need to know when a
    number of \l{TQObject}s that are owned by someone else have been
    deleted. This is important, for example, when referencing memory
    in an application that has been allocated in a shared library.

    Example:

    \code
    class FactoryComponent : public FactoryInterface, public TQLibraryInterface
    {
    public:
	...

	TQObject *createObject();

	bool init();
	void cleanup();
	bool canUnload() const;

    private:
	TQObjectCleanupHandler objects;
    };

    // allocate a new object, and add it to the cleanup handler
    TQObject *FactoryComponent::createObject()
    {
	return objects.add( new TQObject() );
    }

    // TQLibraryInterface implementation
    bool FactoryComponent::init()
    {
	return true;
    }

    void FactoryComponent::cleanup()
    {
    }

    // it is only safe to unload the library when all TQObject's have been destroyed
    bool FactoryComponent::canUnload() const
    {
	return objects.isEmpty();
    }
    \endcode
*/

/*!
    Constructs an empty TQObjectCleanupHandler.
*/
TQObjectCleanupHandler::TQObjectCleanupHandler()
: TQObject(), cleanupObjects( 0 )
{
}

/*!
    Destroys the cleanup handler. All objects in this cleanup handler
    will be deleted.
*/
TQObjectCleanupHandler::~TQObjectCleanupHandler()
{
    clear();
}

/*!
    Adds \a object to this cleanup handler and returns the pointer to
    the object.
*/
TQObject* TQObjectCleanupHandler::add( TQObject* object )
{
    if ( !object )
	return 0;

    if ( !cleanupObjects ) {
	cleanupObjects = new TQObjectList;
 	cleanupObjects->setAutoDelete( true );
    }
    connect( object, TQ_SIGNAL(destroyed(TQObject*)), this, TQ_SLOT(objectDestroyed(TQObject*)) );
    cleanupObjects->insert( 0, object );
    return object;
}

/*!
    Removes the \a object from this cleanup handler. The object will
    not be destroyed.
*/
void TQObjectCleanupHandler::remove( TQObject *object )
{
    if ( !cleanupObjects )
	return;
    if ( cleanupObjects->findRef( object ) >= 0 ) {
	(void) cleanupObjects->take();
	disconnect( object, TQ_SIGNAL(destroyed(TQObject*)), this, TQ_SLOT(objectDestroyed(TQObject*)) );
    }
}

/*!
    Returns true if this cleanup handler is empty or if all objects in
    this cleanup handler have been destroyed; otherwise return false.
*/
bool TQObjectCleanupHandler::isEmpty() const
{
    return cleanupObjects ? cleanupObjects->isEmpty() : true;
}

/*!
    Deletes all objects in this cleanup handler. The cleanup handler
    becomes empty.
*/
void TQObjectCleanupHandler::clear()
{
    delete cleanupObjects;
    cleanupObjects = 0;
}

void TQObjectCleanupHandler::objectDestroyed( TQObject*object )
{
    if ( cleanupObjects )
	cleanupObjects->setAutoDelete( false );

    remove( object );

    if ( cleanupObjects )
	cleanupObjects->setAutoDelete( true );
}
