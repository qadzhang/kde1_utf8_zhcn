/****************************************************************************
**
** Implementation of TQObject class
**
** Created : 930418
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

#include "ntqvariant.h"
#include "ntqapplication.h"
#include "ntqobject.h"
#include "ntqobjectlist.h"
#include "ntqsignalslotimp.h"
#include "ntqregexp.h"
#include "ntqmetaobject.h"
#include <private/qucom_p.h>
#include "qucomextra_p.h"
#include "ntqptrvector.h"

// TQStyleControlElementData
#include "ntqstyle.h"

#ifdef TQT_THREAD_SUPPORT
#include "ntqmutex.h"
#include <private/qmutexpool_p.h>
#include "ntqthread.h"
#endif

#include <ctype.h>
#include <stdlib.h>

// Keep in sync with duplicate definition in qcommonstyle.cpp
class TQStyleControlElementDataPrivate {
	public:
		bool isTQPushButton;
		bool isTQToolButton;
		bool isTQButton;
		bool isTQTabBar;
		bool isTQTitleBar;
		bool isTQToolBox;
		bool isTQToolBar;
		bool isTQProgressBar;
		bool isTQPopupMenu;
		bool isTQComboBox;
		bool isTQCheckBox;
		bool isTQRadioButton;
		bool isTQHeader;
		bool isTQScrollBar;
		bool isTQSlider;
		bool isTQDialogButtons;
		bool isTQSpinWidget;
		bool isTQListView;
		bool isTQDockWindow;
		bool isTQTabWidget;
		bool isTQFrame;
		bool isTQWidget;
};

#ifndef TQT_NO_USERDATA
class TQObjectPrivate : public TQPtrVector<TQObjectUserData>
#else
class TQObjectPrivate {
#endif
{
public:
#ifndef TQT_NO_USERDATA
    TQObjectPrivate( uint s ) : TQPtrVector<TQObjectUserData>(s) {
#ifdef TQT_THREAD_SUPPORT
        ownThread = NULL;
        disableThreadPostedEvents = false;
#endif
        setAutoDelete( true );
        controlElementData = NULL;
        controlElementDataPrivate = NULL;
    }
    ~TQObjectPrivate() {
        delete controlElementData;
        delete controlElementDataPrivate;
    }
#endif
#ifdef TQT_THREAD_SUPPORT
    TQThread* ownThread;
    TQMutex* senderObjectListMutex;
    TQMutex* childObjectListMutex;
    bool disableThreadPostedEvents;
#endif
    TQStyleControlElementData* controlElementData;
    TQStyleControlElementDataPrivate* controlElementDataPrivate;
};

TQStyleControlElementData* TQObject::controlElementDataObject() {
	if (!d->controlElementData) {
		d->controlElementData = new TQStyleControlElementData();
	}
	return d->controlElementData;
}

TQStyleControlElementDataPrivate* TQObject::controlElementDataPrivateObject() {
	if (!d->controlElementDataPrivate) {
		d->controlElementDataPrivate = new TQStyleControlElementDataPrivate();
	}
	return d->controlElementDataPrivate;
}

void TQObject::cleanupControlElementData() {
    delete d->controlElementData;
    d->controlElementData = 0;
    delete d->controlElementDataPrivate;
    d->controlElementDataPrivate = 0;
}

#if defined(TQT_THREAD_SUPPORT)

void TQObject::moveToThread_helper(TQThread *targetThread)
{
    TQEvent e(TQEvent::ThreadChange);
    TQApplication::sendEvent(this, &e);

#ifdef TQT_THREAD_SUPPORT
    TQMutexLocker locker( d->childObjectListMutex );
#endif // TQT_THREAD_SUPPORT

    if (childObjects) {
        TQObject *child;
        TQObjectListIt it(*childObjects);
        while ( (child=it.current()) ) {
            ++it;
            child->moveToThread_helper(targetThread);
        }
    }
}

void TQObject::setThreadObject_helper(TQThread *targetThread)
{
    d->ownThread = targetThread;

#ifdef TQT_THREAD_SUPPORT
    TQMutexLocker locker( d->childObjectListMutex );
#endif // TQT_THREAD_SUPPORT

    if (childObjects) {
        TQObject *child;
        TQObjectListIt it(*childObjects);
        while ( (child=it.current()) ) {
            ++it;
            child->setThreadObject_helper(targetThread);
        }
    }
}

/*!
    Changes the thread affinity for this object and its children. The
    object cannot be moved if it has a parent. Event processing will
    continue in the \a targetThread. To move an object to the main
    thread, pass TQApplication::guiThread() as the \a targetThread.

    Note that all active timers for the object will be reset. The
    timers are first stopped in the current thread and restarted (with
    the same interval) in the \a targetThread. As a result, constantly
    moving an object between threads can postpone timer events
    indefinitely.

    \sa contextThreadObject()
 */
void TQObject::moveToThread(TQThread *targetThread)
{
#ifdef TQT_THREAD_SUPPORT
    TQMutexLocker locker( TQApplication::tqt_mutex );
#endif // TQT_THREAD_SUPPORT

    if (parentObj) {
#if defined(QT_DEBUG)
        tqWarning( "TQObject::moveToThread: Cannot move objects with a parent" );
#endif
        return;
    }
    if (isWidget) {
#if defined(QT_DEBUG)
        tqWarning( "TQObject::moveToThread: Widgets cannot be moved to a new thread" );
#endif
        return;
    }

    TQThread *objectThread = contextThreadObject();
    // NOTE currentThread could be NULL if the current thread was not started using the TQThread API
    TQThread *currentThread = TQThread::currentThreadObject();

    if (currentThread && objectThread != currentThread) {
#if defined(QT_DEBUG)
        tqWarning( "TQObject::moveToThread: Current thread is not the object's thread" );
#endif
        return;
    }

    if (objectThread == targetThread) {
        return;
    }

    moveToThread_helper(targetThread);
    setThreadObject_helper(targetThread);
}

/*!
    Changes the way cross thread signals are handled
    If disable is false, signals emitted from one thread will be
    posted to any other connected threads' event loops (default).

    If disable is true, calls to emit from one thread
    will immediately execute slots in another thread.
    This mode of operation is inherently unsafe and is provided
    solely to support thread management by a third party application.
 */
void TQObject::disableThreadPostedEvents(bool disable) {
    if ( !d ) {
	d = new TQObjectPrivate(0);
    }

    d->ownThread = TQThread::currentThreadObject();
    d->disableThreadPostedEvents = disable;
}

#endif // defined(TQT_THREAD_SUPPORT)

class TQSenderObjectList : public TQObjectList, public TQShared
{
	public:
		TQSenderObjectList();
		~TQSenderObjectList();
	
	public:
		TQObject *currentSender;
#ifdef TQT_THREAD_SUPPORT
		TQMutex *listMutex;
#endif
};

TQSenderObjectList::TQSenderObjectList() : currentSender( 0 ) {
#ifdef TQT_THREAD_SUPPORT
	listMutex = new TQMutex( true );
#endif
}

TQSenderObjectList::~TQSenderObjectList() {
#ifdef TQT_THREAD_SUPPORT
	delete listMutex;
#endif
}

class TQ_EXPORT TQMetaCallEvent : public TQEvent
{
public:
    enum MetaCallType {
	MetaCallEmit	= 0,
	MetaCallInvoke	= 1
    };

public:
    TQMetaCallEvent(int id, TQObject *sender, TQUObject *data, MetaCallType type);
    ~TQMetaCallEvent();

    inline int id() const { return id_; }
    inline TQObject *sender() const { return sender_; }
    inline TQUObject *data() const { return data_; }
    inline MetaCallType type() const { return type_; }

private:
    const int id_;
    TQObject *sender_;
    TQUObject *data_;
    const MetaCallType type_;
};

/*! \internal
 */
TQMetaCallEvent::TQMetaCallEvent(int id, TQObject *sender, TQUObject *data, MetaCallType type)
    :TQEvent(MetaCall), id_(id), sender_(sender), data_(data), type_(type)
{ }

/*! \internal
 */
TQMetaCallEvent::~TQMetaCallEvent()
{ }

/*!
    \class TQt ntqnamespace.h

    \brief The TQt class is a namespace for miscellaneous identifiers
    that need to be global-like.

    \ingroup misc

    Normally, you can ignore this class. TQObject and a few other
    classes inherit it, so all the identifiers in the TQt namespace are
    normally usable without qualification.

    However, you may occasionally need to say \c TQt::black instead of
    just \c black, particularly in static utility functions (such as
    many class factories).

*/

/*!
    \enum TQt::Orientation

    This type is used to signify an object's orientation.

    \value Horizontal
    \value Vertical

    Orientation is used with TQScrollBar for example.
*/


/*!
    \class TQObject ntqobject.h
    \brief The TQObject class is the base class of all TQt objects.

    \ingroup objectmodel
    \mainclass
    \reentrant

    TQObject is the heart of the \link object.html TQt object model.
    \endlink The central feature in this model is a very powerful
    mechanism for seamless object communication called \link
    signalsandslots.html signals and slots \endlink. You can
    connect a signal to a slot with connect() and destroy the
    connection with disconnect(). To avoid never ending notification
    loops you can temporarily block signals with blockSignals(). The
    protected functions connectNotify() and disconnectNotify() make it
    possible to track connections.

    TQObjects organize themselves in object trees. When you create a
    TQObject with another object as parent, the object will
    automatically do an insertChild() on the parent and thus show up
    in the parent's children() list. The parent takes ownership of the
    object i.e. it will automatically delete its children in its
    destructor. You can look for an object by name and optionally type
    using child() or queryList(), and get the list of tree roots using
    objectTrees().

    Every object has an object name() and can report its className()
    and whether it inherits() another class in the TQObject inheritance
    hierarchy.

    When an object is deleted, it emits a destroyed() signal. You can
    catch this signal to avoid dangling references to TQObjects. The
    TQGuardedPtr class provides an elegant way to use this feature.

    TQObjects can receive events through event() and filter the events
    of other objects. See installEventFilter() and eventFilter() for
    details. A convenience handler, childEvent(), can be reimplemented
    to catch child events.

    Last but not least, TQObject provides the basic timer support in
    TQt; see TQTimer for high-level support for timers.

    Notice that the TQ_OBJECT macro is mandatory for any object that
    implements signals, slots or properties. You also need to run the
    \link moc.html moc program (Meta Object Compiler) \endlink on the
    source file. We strongly recommend the use of this macro in \e all
    subclasses of TQObject regardless of whether or not they actually
    use signals, slots and properties, since failure to do so may lead
    certain functions to exhibit undefined behaviour.

    All TQt widgets inherit TQObject. The convenience function
    isWidgetType() returns whether an object is actually a widget. It
    is much faster than inherits( "TQWidget" ).

    Some TQObject functions, e.g. children(), objectTrees() and
    queryList() return a TQObjectList. A TQObjectList is a TQPtrList of
    TQObjects. TQObjectLists support the same operations as TQPtrLists
    and have an iterator class, TQObjectListIt.
*/


//
// Remove white space from TQ_SIGNAL and TQ_SLOT names.
// Internal for TQObject::connect() and TQObject::disconnect()
//

static inline bool isIdentChar( char x )
{						// Avoid bug in isalnum
    return x == '_' || (x >= '0' && x <= '9') ||
	 (x >= 'a' && x <= 'z') || (x >= 'A' && x <= 'Z');
}

static inline bool isSpace( char x )
{
    return isspace( (uchar) x );
}

static TQCString tqt_rmWS( const char *s )
{
    TQCString result( tqstrlen(s)+1 );
    char *d = result.data();
    char last = 0;
    while( *s && isSpace(*s) )			// skip leading space
	s++;
    while ( *s ) {
	while ( *s && !isSpace(*s) )
	    last = *d++ = *s++;
	while ( *s && isSpace(*s) )
	    s++;
	if ( *s && isIdentChar(*s) && isIdentChar(last) )
	    last = *d++ = ' ';
    }
    *d = '\0';
    result.truncate( (int)(d - result.data()) );
    int void_pos = result.find("(void)");
    if ( void_pos >= 0 )
	result.remove( void_pos+1, (uint)strlen("void") );
    return result;
}


// Event functions, implemented in qapplication_xxx.cpp

int   qStartTimer( int interval, TQObject *obj );
bool  qKillTimer( int id );
bool  qKillTimer( TQObject *obj );

static void removeObjFromList( TQObjectList *objList, const TQObject *obj,
			       bool single=false )
{
    if ( !objList ) {
	return;
    }
    int index = objList->findRef( obj );
    while ( index >= 0 ) {
	objList->remove();
	if ( single )
	    return;
	index = objList->findNextRef( obj );
    }
}


/*!
    \relates TQObject

    Returns a pointer to the object named \a name that inherits \a
    type and with a given \a parent.

    Returns 0 if there is no such child.

    \code
	TQListBox *c = (TQListBox *) tqt_find_obj_child( myWidget, "TQListBox",
						      "my list box" );
	if ( c )
	    c->insertItem( "another string" );
    \endcode
*/

void *tqt_find_obj_child( TQObject *parent, const char *type, const char *name )
{
    const TQObjectList *list = parent->children();
    if ( list ) {
	TQObjectListIt it( *list );
	TQObject *obj;
	while ( (obj = it.current()) ) {
	    ++it;
	    if ( qstrcmp(name,obj->name()) == 0 &&
		 obj->inherits(type) )
		return obj;
	}
    }
    return 0;
}

#ifdef TQT_THREAD_SUPPORT

/*!
    Returns a pointer to the TQThread* associated with
    the current thread affinity of this object.

    \sa moveToThread()
 */

TQThread* TQObject::contextThreadObject() const
{
    return d->ownThread;
}

#endif

#ifndef TQT_NO_PRELIMINARY_SIGNAL_SPY
/*
  Preliminary signal spy
 */
TQ_EXPORT TQObject* tqt_preliminary_signal_spy = 0;
static TQObject* tqt_spy_signal_sender = 0;

static void tqt_spy_signal( TQObject* sender, int signal, TQUObject* o )
{
    TQMetaObject* mo = sender->metaObject();
    while ( mo && signal - mo->signalOffset() < 0 )
	mo = mo->superClass();
    if ( !mo )
	return;
    const TQMetaData* sigData = mo->signal( signal - mo->signalOffset() );
    if ( !sigData )
	return;
    TQCString s;
    mo = sender->metaObject();
    while ( mo ) {
	s.sprintf( "%s_%s", mo->className(), sigData->name );
	int slot = tqt_preliminary_signal_spy->metaObject()->findSlot( s, true );
	if ( slot >= 0 ) {
#ifdef TQT_THREAD_SUPPORT
	    // protect access to tqt_spy_signal_sender
	    void * const address = &tqt_spy_signal_sender;
	    TQMutexLocker locker( tqt_global_mutexpool ?
				 tqt_global_mutexpool->get( address ) : 0 );
#endif // TQT_THREAD_SUPPORT

	    TQObject* old_sender = tqt_spy_signal_sender;
	    tqt_spy_signal_sender = sender;
	    tqt_preliminary_signal_spy->tqt_invoke( slot, o );
	    tqt_spy_signal_sender = old_sender;
	    break;
	}
	mo = mo->superClass();
    }
}

/*
  End Preliminary signal spy
 */
#endif // TQT_NO_PRELIMINARY_SIGNAL_SPY

static TQObjectList* object_trees = 0;

#ifdef TQT_THREAD_SUPPORT
static TQMutex *obj_trees_mutex = 0;
#endif

static void cleanup_object_trees()
{
    delete object_trees;
    object_trees = 0;
#ifdef TQT_THREAD_SUPPORT
    delete obj_trees_mutex;
    obj_trees_mutex = 0;
#endif
}

static void ensure_object_trees()
{
    object_trees = new TQObjectList;
    tqAddPostRoutine( cleanup_object_trees );
}

static void insert_tree( TQObject* obj )
{
#ifdef TQT_THREAD_SUPPORT
    if ( !obj_trees_mutex )
	obj_trees_mutex = new TQMutex();
    TQMutexLocker locker( obj_trees_mutex );
#endif
    if ( !object_trees )
	ensure_object_trees();
    object_trees->insert(0, obj );
}

static void remove_tree( TQObject* obj )
{
    if ( object_trees ) {
#ifdef TQT_THREAD_SUPPORT
	TQMutexLocker locker( obj_trees_mutex );
#endif
	object_trees->removeRef( obj );
    }
}

/*! \internal
    TQt compatibility function
*/
TQObjectList TQObject::childrenListObject() {
	if (children()) return *(children());
	else return TQObjectList();
}

/*! \internal
    TQt compatibility function
*/
const TQObjectList TQObject::childrenListObject() const {
	if (children()) return *(children());
	else return TQObjectList();
}

/*! \internal
    TQt compatibility function
*/
const TQObjectList TQObject::objectTreesListObject() {
	if (objectTrees()) return *(objectTrees());
	else return TQObjectList();
}


/*****************************************************************************
  TQObject member functions
 *****************************************************************************/

/*!
    Constructs an object called \a name with parent object, \a parent.

    The parent of an object may be viewed as the object's owner. For
    instance, a \link TQDialog dialog box\endlink is the parent of the
    "OK" and "Cancel" buttons it contains.

    The destructor of a parent object destroys all child objects.

    Setting \a parent to 0 constructs an object with no parent. If the
    object is a widget, it will become a top-level window.

    The object name is some text that can be used to identify a
    TQObject. It's particularly useful in conjunction with \link
    designer-manual.book <i>TQt Designer</i>\endlink. You can find an
    object by name (and type) using child(). To find several objects
    use queryList().

    \sa parent(), name(), child(), queryList()
*/

TQObject::TQObject( TQObject *parent, const char *name )
    :
    isSignal( false ),				// assume not a signal object
    isWidget( false ), 				// assume not a widget object
    pendTimer( false ),				// no timers yet
    blockSig( false ),      			// not blocking signals
    wasDeleted( false ),       			// double-delete catcher
    isTree( false ), 				// no tree yet
    objname( name ? tqstrdup(name) : 0 ),        // set object name
    parentObj( 0 ),				// no parent yet. It is set by insertChild()
    childObjects( 0 ), 				// no children yet
    connections( 0 ),				// no connections yet
    senderObjects( 0 ),        			// no signals connected yet
    eventFilters( 0 ), 				// no filters installed
    postedEvents( 0 ), 				// no events posted
    d( 0 )
{
    if ( !d ) {
	d = new TQObjectPrivate(0);
    }

#ifdef TQT_THREAD_SUPPORT
    d->ownThread = TQThread::currentThreadObject();
    d->senderObjectListMutex = new TQMutex( true );
    d->childObjectListMutex = new TQMutex( true );
#endif
    if ( !metaObj ) {				// will create object dict
	(void) staticMetaObject();
    }

    if ( parent ) {				// add object to parent
	parent->insertChild( this );
    }
    else {
	insert_tree( this );
	isTree = true;
    }
}


/*!
    Destroys the object, deleting all its child objects.

    All signals to and from the object are automatically disconnected.

    \warning All child objects are deleted. If any of these objects
    are on the stack or global, sooner or later your program will
    crash. We do not recommend holding pointers to child objects from
    outside the parent. If you still do, the TQObject::destroyed()
    signal gives you an opportunity to detect when an object is
    destroyed.

    \warning Deleting a TQObject while pending events are waiting to be
    delivered can cause a crash.  You must not delete the TQObject
    directly from a thread that is not the GUI thread.  Use the
    TQObject::deleteLater() method instead, which will cause the event
    loop to delete the object after all pending events have been
    delivered to the object.
*/

TQObject::~TQObject()
{
    if ( wasDeleted ) {
#if defined(QT_DEBUG)
	tqWarning( "Double TQObject deletion detected." );
#endif
	return;
    }
    if (tqApp) {
	TQEvent destroyEvent(TQEvent::Destroy);
	tqApp->notify(this, &destroyEvent);
    }
    wasDeleted = 1;
    blockSig = 0; // unblock signals to keep TQGuardedPtr happy
    emit destroyed( this );
    emit destroyed();

    if ( objname ) {
	delete [] (char*)objname;
    }
    objname = 0;
    if ( pendTimer ) {				// might be pending timers
	qKillTimer( this );
    }
    if ( tqApp )
    {
	TQApplication::removePostedEvents( this );
    }
    if ( isTree ) {
	remove_tree( this );		// remove from global root list
	isTree = false;
    }
    if ( parentObj )				// remove it from parent object
	parentObj->removeChild( this );
    TQObject *obj;
    if ( senderObjects ) {			// disconnect from senders
	TQSenderObjectList *tmp = senderObjects;
	senderObjects = 0;
	obj = tmp->first();
	while ( obj ) {				// for all senders...
	    obj->disconnect( this );
	    obj = tmp->next();
	}
	if ( tmp->deref() )
	    delete tmp;
    }
    if ( connections ) {			// disconnect receivers
	for ( int i = 0; i < (int) connections->size(); i++ ) {
	    TQConnectionList* clist = (*connections)[i]; // for each signal...
	    if ( !clist )
		continue;
	    TQConnection *c;
	    TQConnectionListIt cit(*clist);
	    while( (c=cit.current()) ) {	// for each connected slot...
		++cit;
		if ( (obj=c->object()) ) {
		    removeObjFromList( obj->senderObjects, this );
		}
	    }
	}
	delete connections;
	connections = 0;
    }
    if ( eventFilters ) {
	delete eventFilters;
	eventFilters = 0;
    }
    if ( childObjects ) {			// delete children objects
	TQObjectListIt it(*childObjects);
	while ( (obj=it.current()) ) {
	    ++it;
	    obj->parentObj = 0;
	    childObjects->removeRef( obj );
	    delete obj;
	}
	delete childObjects;
    }

#ifdef TQT_THREAD_SUPPORT
    delete d->childObjectListMutex;
    delete d->senderObjectListMutex;
#endif // TQT_THREAD_SUPPORT

    delete d;
}


/*!
    \fn TQMetaObject *TQObject::metaObject() const

    Returns a pointer to the meta object of this object.

    A meta object contains information about a class that inherits
    TQObject, e.g. class name, superclass name, properties, signals and
    slots. Every class that contains the TQ_OBJECT macro will also have
    a meta object.

    The meta object information is required by the signal/slot
    connection mechanism and the property system. The functions isA()
    and inherits() also make use of the meta object.
*/

/*!
    \fn const char *TQObject::className() const

    Returns the class name of this object.

    This function is generated by the \link metaobjects.html Meta
    Object Compiler. \endlink

    \warning This function will return the wrong name if the class
    definition lacks the TQ_OBJECT macro.

    \sa name(), inherits(), isA(), isWidgetType()
*/

/*!
    Returns true if this object is an instance of the class \a clname;
    otherwise returns false.

  Example:
  \code
    TQTimer *t = new TQTimer; // TQTimer inherits TQObject
    t->isA( "TQTimer" );     // returns true
    t->isA( "TQObject" );    // returns false
  \endcode

  \sa inherits() metaObject()
*/

bool TQObject::isA( const char *clname ) const
{
    return qstrcmp( clname, className() ) == 0;
}

/*!
    Returns true if this object is an instance of a class that
    inherits \a clname, and \a clname inherits TQObject; otherwise
    returns false.

    A class is considered to inherit itself.

    Example:
    \code
	TQTimer *t = new TQTimer;         // TQTimer inherits TQObject
	t->inherits( "TQTimer" );        // returns true
	t->inherits( "TQObject" );       // returns true
	t->inherits( "TQButton" );       // returns false

	// TQScrollBar inherits TQWidget and TQRangeControl
	TQScrollBar *s = new TQScrollBar( 0 );
	s->inherits( "TQWidget" );       // returns true
	s->inherits( "TQRangeControl" ); // returns false
    \endcode

    (\l TQRangeControl is not a TQObject.)

    \sa isA(), metaObject()
*/

bool TQObject::inherits( const char *clname ) const
{
    return metaObject()->inherits( clname );
}

/*!
    \internal

    Returns true if \a object inherits \a superClass within
    the meta object inheritance chain; otherwise returns false.

    \sa inherits()
*/
void *tqt_inheritedBy( TQMetaObject *superClass, const TQObject *object )
{
    if (!object)
	return 0;
    TQMetaObject *mo = object->metaObject();
    while (mo) {
	if (mo == superClass)
	    return (void*)object;
	mo = mo->superClass();
    }
    return 0;
}

/*!
    \property TQObject::name

    \brief the name of this object

    You can find an object by name (and type) using child(). You can
    find a set of objects with queryList().

    The object name is set by the constructor or by the setName()
    function. The object name is not very useful in the current
    version of TQt, but will become increasingly important in the
    future.

    If the object does not have a name, the name() function returns
    "unnamed", so printf() (used in tqDebug()) will not be asked to
    output a null pointer. If you want a null pointer to be returned
    for unnamed objects, you can call name( 0 ).

    \code
	tqDebug( "MyClass::setPrecision(): (%s) invalid precision %f",
		name(), newPrecision );
    \endcode

    \sa className(), child(), queryList()
*/

const char * TQObject::name() const
{
    // If you change the name here, the builder will be broken
    return objname ? objname : "unnamed";
}

/*!
    Sets the object's name to \a name.
*/
void TQObject::setName( const char *name )
{
    if ( objname )
	delete [] (char*) objname;
    objname = name ? tqstrdup(name) : 0;
}

/*!
    \overload

    Returns the name of this object, or \a defaultName if the object
    does not have a name.
*/

const char * TQObject::name( const char * defaultName ) const
{
    return objname ? objname : defaultName;
}


/*!
    Searches the children and optionally grandchildren of this object,
    and returns a child that is called \a objName that inherits \a
    inheritsClass. If \a inheritsClass is 0 (the default), any class
    matches.

    If \a recursiveSearch is true (the default), child() performs a
    depth-first search of the object's children.

    If there is no such object, this function returns 0. If there are
    more than one, the first one found is retured; if you need all of
    them, use queryList().
*/
TQObject* TQObject::child( const char *objName, const char *inheritsClass,
			 bool recursiveSearch )
{
    const TQObjectList *list = children();
    if ( !list )
	return 0;

    bool onlyWidgets = ( inheritsClass && qstrcmp( inheritsClass, "TQWidget" ) == 0 );
    TQObjectListIt it( *list );
    TQObject *obj;
    while ( ( obj = it.current() ) ) {
	++it;
	if ( onlyWidgets ) {
	    if ( obj->isWidgetType() && ( !objName || qstrcmp( objName, obj->name() ) == 0 ) )
		break;
	} else if ( ( !inheritsClass || obj->inherits(inheritsClass) ) && ( !objName || qstrcmp( objName, obj->name() ) == 0 ) )
	    break;
	if ( recursiveSearch && (obj = obj->child( objName, inheritsClass, recursiveSearch ) ) )
	    break;
    }
    return obj;
}

/*! \internal */
TQUObject* deepCopyTQUObjectArray(TQUObject* origArray)
{
    TQUObject* newArray;
    int count = 0;
    while (!((origArray+count)->isLastObject)) {
	count++;
    }
    count++;
    newArray = (TQUObject*)malloc(sizeof(TQUObject)*count);
    for (int i=0; i<count; i++) {
	(origArray+i)->deepCopy(newArray+i);
    }
    return newArray;
}

/*! \internal */
void destroyDeepCopiedTQUObjectArray(TQUObject* uArray)
{
    int count = 0;
    while (!((uArray+count)->isLastObject)) {
	count++;
    }
    count++;
    for (int i=0; i<count; i++) {
	(uArray+i)->~TQUObject();
    }
    free(uArray);
}

/*!
    \fn bool TQObject::isWidgetType() const

    Returns true if the object is a widget; otherwise returns false.

    Calling this function is equivalent to calling
    inherits("TQWidget"), except that it is much faster.
*/

/*!
    \fn bool TQObject::highPriority() const

    Returns true if the object is a high-priority object, or false if
    it is a standard-priority object.

    High-priority objects are placed first in TQObject's list of
    children on the assumption that they will be referenced very
    often.
*/


/*!
    This virtual function receives events to an object and should
    return true if the event \a e was recognized and processed.

    The event() function can be reimplemented to customize the
    behavior of an object.

    \sa installEventFilter(), timerEvent(), TQApplication::sendEvent(),
    TQApplication::postEvent(), TQWidget::event()
*/

bool TQObject::event( TQEvent *e )
{
#if defined(QT_CHECK_NULL)
    if ( e == 0 ) {
	tqWarning( "TQObject::event: Null events are not permitted" );
    }
#endif
    if ( wasDeleted ) {
	return true;
    }
    if ( eventFilters ) {			// try filters
	if ( activate_filters(e) ) {		// stopped by a filter
	    return true;
	}
    }

    switch ( e->type() ) {
    case TQEvent::Timer:
	timerEvent( (TQTimerEvent*)e );
	return true;

    case TQEvent::ChildInserted:
    case TQEvent::ChildRemoved:
	childEvent( (TQChildEvent*)e );
	return true;

    case TQEvent::DeferredDelete:
	delete this;
	return true;

    case TQEvent::MetaCall:
	{
	    bool handled = false;
	    TQMetaCallEvent* metaEvent = dynamic_cast<TQMetaCallEvent*>(e);
	    if (metaEvent) {
#ifdef TQT_THREAD_SUPPORT
		if ((d->disableThreadPostedEvents)
		    || (d->ownThread == TQThread::currentThreadObject()))
#endif // TQT_THREAD_SUPPORT
		{
		    TQObject *oldSender = nullptr;
		    TQSenderObjectList *sol = senderObjects;
		    if ( sol )
		    {
#ifdef TQT_THREAD_SUPPORT
			sol->listMutex->lock();
#endif // TQT_THREAD_SUPPORT
			oldSender = sol->currentSender;
			sol->ref();
			sol->currentSender = metaEvent->sender();
#ifdef TQT_THREAD_SUPPORT
			sol->listMutex->unlock();
#endif // TQT_THREAD_SUPPORT
		    }
		    TQUObject *o = metaEvent->data();
		    if (metaEvent->type() == TQMetaCallEvent::MetaCallEmit)
		    {
			handled = true;
			tqt_emit( metaEvent->id(), o );
		    }
		    else if (metaEvent->type() == TQMetaCallEvent::MetaCallInvoke)
		    {
			handled = true;
			tqt_invoke( metaEvent->id(), o );
		    }
		    if ( sol )
		    {
#ifdef TQT_THREAD_SUPPORT
			sol->listMutex->lock();
#endif // TQT_THREAD_SUPPORT
			sol->currentSender = oldSender;
			if ( sol->deref() )
			{
#ifdef TQT_THREAD_SUPPORT
			    sol->listMutex->unlock();
#endif // TQT_THREAD_SUPPORT
			    delete sol;
			    sol = nullptr;
			}
		    }
#ifdef TQT_THREAD_SUPPORT
		    if (sol)
		    {
			sol->listMutex->unlock();
		    }
#endif // TQT_THREAD_SUPPORT
		}
#ifdef TQT_THREAD_SUPPORT
		else
		{
		    tqWarning("TQObject: Ignoring metacall event from non-owning thread");
		}
#endif // TQT_THREAD_SUPPORT
		destroyDeepCopiedTQUObjectArray(metaEvent->data());
	    }
	    return handled;
	}

    default:
	if ( e->type() >= TQEvent::User ) {
	    customEvent( (TQCustomEvent*) e );
	    return true;
	}
	break;
    }
    return false;
}

/*!
    This event handler can be reimplemented in a subclass to receive
    timer events for the object.

    TQTimer provides a higher-level interface to the timer
    functionality, and also more general information about timers.

    \sa startTimer(), killTimer(), killTimers(), event()
*/

void TQObject::timerEvent( TQTimerEvent * )
{
}


/*!
    This event handler can be reimplemented in a subclass to receive
    child events.

    Child events are sent to objects when children are inserted or
    removed.

    Note that events with TQEvent::type() \c TQEvent::ChildInserted are
    posted (with \l{TQApplication::postEvent()}) to make sure that the
    child's construction is completed before this function is called.

    If a child is removed immediately after it is inserted, the \c
    ChildInserted event may be suppressed, but the \c ChildRemoved
    event will always be sent. In such cases it is possible that there
    will be a \c ChildRemoved event without a corresponding \c
    ChildInserted event.

    If you change state based on \c ChildInserted events, call
    TQWidget::constPolish(), or do
    \code
	TQApplication::sendPostedEvents( this, TQEvent::ChildInserted );
    \endcode
    in functions that depend on the state. One notable example is
    TQWidget::sizeHint().

    \sa event(), TQChildEvent
*/

void TQObject::childEvent( TQChildEvent * )
{
}

/*!
    This event handler can be reimplemented in a subclass to receive
    custom events. Custom events are user-defined events with a type
    value at least as large as the "User" item of the \l TQEvent::Type
    enum, and is typically a TQCustomEvent or TQCustomEvent subclass.

    \sa event(), TQCustomEvent
*/
void TQObject::customEvent( TQCustomEvent * )
{
}



/*!
    Filters events if this object has been installed as an event
    filter for the \a watched object.

    In your reimplementation of this function, if you want to filter
    the event \a e, out, i.e. stop it being handled further, return
    true; otherwise return false.

    Example:
    \code
    class MyMainWindow : public TQMainWindow
    {
    public:
        MyMainWindow( TQWidget *parent = 0, const char *name = 0 );

    protected:
        bool eventFilter( TQObject *obj, TQEvent *ev );

    private:
        TQTextEdit *textEdit;
    };

    MyMainWindow::MyMainWindow( TQWidget *parent, const char *name )
        : TQMainWindow( parent, name )
    {
        textEdit = new TQTextEdit( this );
        setCentralWidget( textEdit );
        textEdit->installEventFilter( this );
    }

    bool MyMainWindow::eventFilter( TQObject *obj, TQEvent *ev )
    {
        if ( obj == textEdit ) {
            if ( e->type() == TQEvent::KeyPress ) {
                TQKeyEvent *k = (TQKeyEvent*)ev;
                tqDebug( "Ate key press %d", k->key() );
                return true;
            } else {
                return false;
            }
        } else {
            // pass the event on to the parent class
            return TQMainWindow::eventFilter( obj, ev );
        }
    }
    \endcode

    Notice in the example above that unhandled events are passed to
    the base class's eventFilter() function, since the base class
    might have reimplemented eventFilter() for its own internal
    purposes.

    \warning If you delete the receiver object in this function, be
    sure to return true. Otherwise, TQt will forward the event to the
    deleted object and the program might crash.

    \sa installEventFilter()
*/

bool TQObject::eventFilter( TQObject * /* watched */, TQEvent * /* e */ )
{
    return false;
}


/*!
  \internal
  Activates all event filters for this object.
  This function is normally called from TQObject::event() or TQWidget::event().
*/

bool TQObject::activate_filters( TQEvent *e )
{
    if ( !eventFilters )			// no event filter
	return false;
    TQObjectListIt it( *eventFilters );
    TQObject *obj = it.current();
    while ( obj ) {				// send to all filters
	++it;					//   until one returns true
	if ( obj->eventFilter(this,e) ) {
	    return true;
	}
	obj = it.current();
    }
    return false;				// don't do anything with it
}


/*!
    \fn bool TQObject::signalsBlocked() const

    Returns true if signals are blocked; otherwise returns false.

    Signals are not blocked by default.

    \sa blockSignals()
*/

/*!
    Blocks signals if \a block is true, or unblocks signals if \a
    block is false.

    Emitted signals disappear into hyperspace if signals are blocked.
    Note that the destroyed() signals will be emitted even if the signals
    for this object have been blocked.
*/

void TQObject::blockSignals( bool block )
{
    blockSig = block;
}


//
// The timer flag hasTimer is set when startTimer is called.
// It is not reset when killing the timer because more than
// one timer might be active.
//

/*!
    Starts a timer and returns a timer identifier, or returns zero if
    it could not start a timer.

    A timer event will occur every \a interval milliseconds until
    killTimer() or killTimers() is called. If \a interval is 0, then
    the timer event occurs once every time there are no more window
    system events to process.

    The virtual timerEvent() function is called with the TQTimerEvent
    event parameter class when a timer event occurs. Reimplement this
    function to get timer events.

    If multiple timers are running, the TQTimerEvent::timerId() can be
    used to find out which timer was activated.

    Example:
    \code
    class MyObject : public TQObject
    {
	TQ_OBJECT
    public:
	MyObject( TQObject *parent = 0, const char *name = 0 );

    protected:
	void timerEvent( TQTimerEvent * );
    };

    MyObject::MyObject( TQObject *parent, const char *name )
	: TQObject( parent, name )
    {
	startTimer( 50 );    // 50-millisecond timer
	startTimer( 1000 );  // 1-second timer
	startTimer( 60000 ); // 1-minute timer
    }

    void MyObject::timerEvent( TQTimerEvent *e )
    {
	tqDebug( "timer event, id %d", e->timerId() );
    }
    \endcode

    Note that TQTimer's accuracy depends on the underlying operating
    system and hardware. Most platforms support an accuracy of 20 ms;
    some provide more. If TQt is unable to deliver the requested
    number of timer clicks, it will silently discard some.

    The TQTimer class provides a high-level programming interface with
    one-shot timers and timer signals instead of events.

    \sa timerEvent(), killTimer(), killTimers(), TQEventLoop::awake(),
        TQEventLoop::aboutToBlock()
*/

int TQObject::startTimer( int interval )
{
    pendTimer = true;				// set timer flag
    return qStartTimer( interval, (TQObject *)this );
}

/*!
    Kills the timer with timer identifier, \a id.

    The timer identifier is returned by startTimer() when a timer
    event is started.

    \sa timerEvent(), startTimer(), killTimers()
*/

void TQObject::killTimer( int id )
{
    qKillTimer( id );
}

/*!
    Kills all timers that this object has started.

    \warning Using this function can cause hard-to-find bugs: it kills
    timers started by sub- and superclasses as well as those started
    by you, which is often not what you want. We recommend using a
    TQTimer or perhaps killTimer().

    \sa timerEvent(), startTimer(), killTimer()
*/

void TQObject::killTimers()
{
    qKillTimer( this );
}

static void objSearch( TQObjectList *result,
		       TQObjectList *list,
		       const char  *inheritsClass,
		       bool onlyWidgets,
		       const char  *objName,
		       TQRegExp	   *rx,
		       bool	    recurse )
{
    if ( !list || list->isEmpty() )		// nothing to search
	return;
    TQObject *obj = list->first();
    while ( obj ) {
	bool ok = true;
	if ( onlyWidgets )
	    ok = obj->isWidgetType();
	else if ( inheritsClass && !obj->inherits(inheritsClass) )
	    ok = false;
	if ( ok ) {
	    if ( objName )
		ok = ( qstrcmp(objName,obj->name()) == 0 );
#ifndef TQT_NO_REGEXP
	    else if ( rx )
		ok = ( rx->search(TQString::fromLatin1(obj->name())) != -1 );
#endif
	}
	if ( ok )				// match!
	    result->append( obj );
	if ( recurse && obj->children() )
	    objSearch( result, (TQObjectList *)obj->children(), inheritsClass,
		       onlyWidgets, objName, rx, recurse );
	obj = list->next();
    }
}

/*!
    \fn TQObject *TQObject::parent() const

    Returns a pointer to the parent object.

    \sa children()
*/

/*!
    \fn const TQObjectList *TQObject::children() const

    Returns a list of child objects, or 0 if this object has no
    children.

    The TQObjectList class is defined in the \c ntqobjectlist.h header
    file.

    The first child added is the \link TQPtrList::first() first\endlink
    object in the list and the last child added is the \link
    TQPtrList::last() last\endlink object in the list, i.e. new
    children are appended at the end.

    Note that the list order changes when TQWidget children are \link
    TQWidget::raise() raised\endlink or \link TQWidget::lower()
    lowered.\endlink A widget that is raised becomes the last object
    in the list, and a widget that is lowered becomes the first object
    in the list.

    \sa child(), queryList(), parent(), insertChild(), removeChild()
*/


/*!
    Returns a pointer to the list of all object trees (their root
    objects), or 0 if there are no objects.

    The TQObjectList class is defined in the \c ntqobjectlist.h header
    file.

    The most recent root object created is the \link TQPtrList::first()
    first\endlink object in the list and the first root object added
    is the \link TQPtrList::last() last\endlink object in the list.

    \sa children(), parent(), insertChild(), removeChild()
*/
const TQObjectList *TQObject::objectTrees()
{
    return object_trees;
}


/*!
    Searches the children and optionally grandchildren of this object,
    and returns a list of those objects that are named or that match
    \a objName and inherit \a inheritsClass. If \a inheritsClass is 0
    (the default), all classes match. If \a objName is 0 (the
    default), all object names match.

    If \a regexpMatch is true (the default), \a objName is a regular
    expression that the objects's names must match. The syntax is that
    of a TQRegExp. If \a regexpMatch is false, \a objName is a string
    and object names must match it exactly.

    Note that \a inheritsClass uses single inheritance from TQObject,
    the way inherits() does. According to inherits(), TQMenuBar
    inherits TQWidget but not TQMenuData. This does not quite match
    reality, but is the best that can be done on the wide variety of
    compilers TQt supports.

    Finally, if \a recursiveSearch is true (the default), queryList()
    searches \e{n}th-generation as well as first-generation children.

    If all this seems a bit complex for your needs, the simpler
    child() function may be what you want.

    This somewhat contrived example disables all the buttons in this
    window:
    \code
    TQObjectList *l = topLevelWidget()->queryList( "TQButton" );
    TQObjectListIt it( *l ); // iterate over the buttons
    TQObject *obj;

    while ( (obj = it.current()) != 0 ) {
	// for each found object...
	++it;
	((TQButton*)obj)->setEnabled( false );
    }
    delete l; // delete the list, not the objects
    \endcode

    The TQObjectList class is defined in the \c ntqobjectlist.h header
    file.

    \warning Delete the list as soon you have finished using it. The
    list contains pointers that may become invalid at almost any time
    without notice (as soon as the user closes a window you may have
    dangling pointers, for example).

    \sa child() children(), parent(), inherits(), name(), TQRegExp
*/

TQObjectList *TQObject::queryList( const char *inheritsClass,
				 const char *objName,
				 bool regexpMatch,
				 bool recursiveSearch ) const
{
    TQObjectList *list = new TQObjectList;
    TQ_CHECK_PTR( list );
    bool onlyWidgets = ( inheritsClass && qstrcmp(inheritsClass, "TQWidget") == 0 );
#ifndef TQT_NO_REGEXP
    if ( regexpMatch && objName ) {		// regexp matching
	TQRegExp rx(TQString::fromLatin1(objName));
	objSearch( list, (TQObjectList *)children(), inheritsClass, onlyWidgets,
		   0, &rx, recursiveSearch );
    } else
#endif
	{
	objSearch( list, (TQObjectList *)children(), inheritsClass, onlyWidgets,
		   objName, 0, recursiveSearch );
    }
    return list;
}

/*! \internal

  Returns a list of objects/slot pairs that are connected to the
  \a signal, or 0 if nothing is connected to it.
*/

TQConnectionList *TQObject::receivers( const char* signal ) const
{
    if ( connections && signal ) {
	if ( *signal == '2' ) {			// tag == 2, i.e. signal
	    TQCString s = tqt_rmWS( signal+1 );
	    return receivers( metaObject()->findSignal( (const char*)s, true ) );
	} else {
	    return receivers( metaObject()->findSignal(signal, true ) );
	}
    }
    return 0;
}

/*! \internal

  Returns a list of objects/slot pairs that are connected to the
  signal, or 0 if nothing is connected to it.
*/

TQConnectionList *TQObject::receivers( int signal ) const
{
#ifndef TQT_NO_PRELIMINARY_SIGNAL_SPY
    if ( tqt_preliminary_signal_spy && signal >= 0 ) {
	if ( !connections ) {
	    TQObject* that = (TQObject*) this;
	    that->connections = new TQSignalVec( signal+1 );
	    that->connections->setAutoDelete( true );
	}
	if ( !connections->at( signal ) ) {
	    TQConnectionList* clist = new TQConnectionList;
	    clist->setAutoDelete( true );
	    connections->insert( signal, clist );
	    return clist;
	}
    }
#endif
    if ( connections && signal >= 0 )
	return connections->at( signal );
    return 0;
}


/*!
    Inserts an object \a obj into the list of child objects.

    \warning This function cannot be used to make one widget the child
    widget of another widget. Child widgets can only be created by
    setting the parent widget in the constructor or by calling
    TQWidget::reparent().

    \sa removeChild(), TQWidget::reparent()
*/

void TQObject::insertChild( TQObject *obj )
{
#ifdef TQT_THREAD_SUPPORT
	TQMutexLocker locker( d->childObjectListMutex );
#endif // TQT_THREAD_SUPPORT

    if ( obj->isTree ) {
	remove_tree( obj );
	obj->isTree = false;
    }
    if ( obj->parentObj && obj->parentObj != this ) {
#if defined(QT_CHECK_STATE)
	if ( obj->parentObj != this && obj->isWidgetType() )
	    tqWarning( "TQObject::insertChild: Cannot reparent a widget, "
		     "use TQWidget::reparent() instead" );
#endif
	obj->parentObj->removeChild( obj );
    }

    if ( !childObjects ) {
	childObjects = new TQObjectList;
	TQ_CHECK_PTR( childObjects );
    } else if ( obj->parentObj == this ) {
#if defined(QT_CHECK_STATE)
	tqWarning( "TQObject::insertChild: Object %s::%s already in list",
		 obj->className(), obj->name( "unnamed" ) );
#endif
	return;
    }
    obj->parentObj = this;
    childObjects->append( obj );

    TQChildEvent *e = new TQChildEvent( TQEvent::ChildInserted, obj );
    TQApplication::postEvent( this, e );
}

/*!
    Removes the child object \a obj from the list of children.

    \warning This function will not remove a child widget from the
    screen. It will only remove it from the parent widget's list of
    children.

    \sa insertChild(), TQWidget::reparent()
*/

void TQObject::removeChild( TQObject *obj )
{
#ifdef TQT_THREAD_SUPPORT
	TQMutexLocker locker( d->childObjectListMutex );
#endif // TQT_THREAD_SUPPORT

    if ( childObjects && childObjects->removeRef(obj) ) {
	obj->parentObj = 0;
	if ( !obj->wasDeleted ) {
	    insert_tree( obj );			// it's a root object now
	    obj->isTree = true;
	}
	if ( childObjects->isEmpty() ) {
	    delete childObjects;		// last child removed
	    childObjects = 0;			// reset children list
	}

	// remove events must be sent, not posted!!!
	TQChildEvent ce( TQEvent::ChildRemoved, obj );
	TQApplication::sendEvent( this, &ce );
    }
}


/*!
    \fn void TQObject::installEventFilter( const TQObject *filterObj )

    Installs an event filter \a filterObj on this object. For example:
    \code
    monitoredObj->installEventFilter( filterObj );
    \endcode

    An event filter is an object that receives all events that are
    sent to this object. The filter can either stop the event or
    forward it to this object. The event filter \a filterObj receives
    events via its eventFilter() function. The eventFilter() function
    must return true if the event should be filtered, (i.e. stopped);
    otherwise it must return false.

    If multiple event filters are installed on a single object, the
    filter that was installed last is activated first.

    Here's a \c KeyPressEater class that eats the key presses of its
    monitored objects:
    \code
    class KeyPressEater : public TQObject
    {
	...
    protected:
	bool eventFilter( TQObject *o, TQEvent *e );
    };

    bool KeyPressEater::eventFilter( TQObject *o, TQEvent *e )
    {
	if ( e->type() == TQEvent::KeyPress ) {
	    // special processing for key press
	    TQKeyEvent *k = (TQKeyEvent *)e;
	    tqDebug( "Ate key press %d", k->key() );
	    return true; // eat event
	} else {
	    // standard event processing
	    return false;
	}
    }
    \endcode

    And here's how to install it on two widgets:
    \code
	KeyPressEater *keyPressEater = new KeyPressEater( this );
	TQPushButton *pushButton = new TQPushButton( this );
	TQListView *listView = new TQListView( this );

	pushButton->installEventFilter( keyPressEater );
	listView->installEventFilter( keyPressEater );
    \endcode

    The TQAccel class, for example, uses this technique to intercept
    accelerator key presses.

    \warning If you delete the receiver object in your eventFilter()
    function, be sure to return true. If you return false, TQt sends
    the event to the deleted object and the program will crash.

    \sa removeEventFilter(), eventFilter(), event()
*/

void TQObject::installEventFilter( const TQObject *obj )
{
    if ( !obj )
	return;
    if ( eventFilters ) {
	int c = eventFilters->findRef( obj );
	if ( c >= 0 )
	    eventFilters->take( c );
	disconnect( obj, TQ_SIGNAL(destroyed(TQObject*)),
		    this, TQ_SLOT(cleanupEventFilter(TQObject*)) );
    } else {
	eventFilters = new TQObjectList;
	TQ_CHECK_PTR( eventFilters );
    }
    eventFilters->insert( 0, obj );
    connect( obj, TQ_SIGNAL(destroyed(TQObject*)), this, TQ_SLOT(cleanupEventFilter(TQObject*)) );
}

/*!
    Removes an event filter object \a obj from this object. The
    request is ignored if such an event filter has not been installed.

    All event filters for this object are automatically removed when
    this object is destroyed.

    It is always safe to remove an event filter, even during event
    filter activation (i.e. from the eventFilter() function).

    \sa installEventFilter(), eventFilter(), event()
*/

void TQObject::removeEventFilter( const TQObject *obj )
{
    if ( eventFilters && eventFilters->removeRef(obj) ) {
	if ( eventFilters->isEmpty() ) {	// last event filter removed
	    delete eventFilters;
	    eventFilters = 0;			// reset event filter list
	}
	disconnect( obj,  TQ_SIGNAL(destroyed(TQObject*)),
		    this, TQ_SLOT(cleanupEventFilter(TQObject*)) );
    }
}


/*****************************************************************************
  Signal connection management
 *****************************************************************************/

#if defined(QT_CHECK_RANGE)

static bool check_signal_macro( const TQObject *sender, const char *signal,
				const char *func, const char *op )
{
    int sigcode = (int)(*signal) - '0';
    if ( sigcode != TQ_SIGNAL_CODE ) {
	if ( sigcode == TQ_SLOT_CODE )
	    tqWarning( "TQObject::%s: Attempt to %s non-signal %s::%s",
		     func, op, sender->className(), signal+1 );
	else
	    tqWarning( "TQObject::%s: Use the TQ_SIGNAL macro to %s %s::%s",
		     func, op, sender->className(), signal );
	return false;
    }
    return true;
}

static bool check_member_code( int code, const TQObject *object,
			       const char *member, const char *func )
{
    if ( code != TQ_SLOT_CODE && code != TQ_SIGNAL_CODE ) {
	tqWarning( "TQObject::%s: Use the TQ_SLOT or TQ_SIGNAL macro to "
		 "%s %s::%s", func, func, object->className(), member );
	return false;
    }
    return true;
}

static void err_member_notfound( int code, const TQObject *object,
				 const char *member, const char *func )
{
    const char *type = 0;
    switch ( code ) {
	case TQ_SLOT_CODE:   type = "slot";   break;
	case TQ_SIGNAL_CODE: type = "signal"; break;
    }
    if ( strchr(member,')') == 0 )		// common typing mistake
	tqWarning( "TQObject::%s: Parentheses expected, %s %s::%s",
		 func, type, object->className(), member );
    else
	tqWarning( "TQObject::%s: No such %s %s::%s",
		 func, type, object->className(), member );
}


static void err_info_about_objects( const char * func,
				    const TQObject * sender,
				    const TQObject * receiver )
{
    const char * a = sender->name(), * b = receiver->name();
    if ( a )
	tqWarning( "TQObject::%s:  (sender name:   '%s')", func, a );
    if ( b )
	tqWarning( "TQObject::%s:  (receiver name: '%s')", func, b );
}

static void err_info_about_candidates( int code,
				       const TQMetaObject* mo,
				       const char* member,
				       const char *func	)
{
    if ( strstr(member,"const char*") ) {
	// porting help
	TQCString newname = member;
	int p;
	while ( (p=newname.find("const char*")) >= 0 ) {
	    newname.replace(p, 11, "const TQString&");
	}
	const TQMetaData *rm = 0;
	switch ( code ) {
	case TQ_SLOT_CODE:
	    rm = mo->slot( mo->findSlot( newname, true ), true );
	    break;
	case TQ_SIGNAL_CODE:
	    rm = mo->signal( mo->findSignal( newname, true ), true );
	    break;
	}
	if ( rm ) {
	    tqWarning("TQObject::%s:  Candidate: %s", func, newname.data());
	}
    }
}


#endif // QT_CHECK_RANGE


/*!
    Returns a pointer to the object that sent the signal, if called in
    a slot activated by a signal; otherwise it returns 0. The pointer
    is valid only during the execution of the slot that calls this
    function.

    The pointer returned by this function becomes invalid if the
    sender is destroyed, or if the slot is disconnected from the
    sender's signal.

    \warning This function violates the object-oriented principle of
     modularity. However, getting access to the sender might be useful
     when many signals are connected to a single slot. The sender is
     undefined if the slot is called as a normal C++ function.
*/

const TQObject *TQObject::sender()
{
#ifndef TQT_NO_PRELIMINARY_SIGNAL_SPY
    if ( this == tqt_preliminary_signal_spy ) {
#  ifdef TQT_THREAD_SUPPORT
	// protect access to tqt_spy_signal_sender
	void * const address = &tqt_spy_signal_sender;
	TQMutexLocker locker( tqt_global_mutexpool ?
			     tqt_global_mutexpool->get( address ) : 0 );
#  endif // TQT_THREAD_SUPPORT
	return tqt_spy_signal_sender;
    }
#endif
    if ( senderObjects &&
	 senderObjects->currentSender &&
	 /*
	  * currentSender may be a dangling pointer in case the object
	  * it was pointing to was destructed from inside a slot. Thus
	  * verify it still is contained inside the senderObjects list
	  * which gets cleaned on both destruction and disconnect.
	  */

	 senderObjects->findRef( senderObjects->currentSender ) != -1 )
	return senderObjects->currentSender;
    return 0;
}


/*!
    \fn void TQObject::connectNotify( const char *signal )

    This virtual function is called when something has been connected
    to \a signal in this object.

    \warning This function violates the object-oriented principle of
    modularity. However, it might be useful when you need to perform
    expensive initialization only if something is connected to a
    signal.

    \sa connect(), disconnectNotify()
*/

void TQObject::connectNotify( const char * )
{
}

/*!
    \fn void TQObject::disconnectNotify( const char *signal )

    This virtual function is called when something has been
    disconnected from \a signal in this object.

    \warning This function violates the object-oriented principle of
    modularity. However, it might be useful for optimizing access to
    expensive resources.

    \sa disconnect(), connectNotify()
*/

void TQObject::disconnectNotify( const char * )
{
}


/*!
    \fn bool TQObject::checkConnectArgs( const char *signal, const TQObject *receiver, const char *member )

    Returns true if the \a signal and the \a member arguments are
    compatible; otherwise returns false. (The \a receiver argument is
    currently ignored.)

    \warning We recommend that you use the default implementation and
    do not reimplement this function.

    \omit
    true:  "signal(<anything>)", "member()"
    true:  "signal(a,b,c)",	 "member(a,b,c)"
    true:  "signal(a,b,c)",	 "member(a,b)", "member(a)" etc.
    false: "signal(const a)",	 "member(a)"
    false: "signal(a)",		 "member(const a)"
    false: "signal(a)",		 "member(b)"
    false: "signal(a)",		 "member(a,b)"
    \endomit
*/

bool TQObject::checkConnectArgs( const char    *signal,
				const TQObject *,
				const char    *member )
{
    const char *s1 = signal;
    const char *s2 = member;
    while ( *s1++ != '(' ) { }			// scan to first '('
    while ( *s2++ != '(' ) { }
    if ( *s2 == ')' || qstrcmp(s1,s2) == 0 )	// member has no args or
	return true;				//   exact match
    int s1len = tqstrlen(s1);
    int s2len = tqstrlen(s2);
    if ( s2len < s1len && tqstrncmp(s1,s2,s2len-1)==0 && s1[s2len-1]==',' )
	return true;				// member has less args
    return false;
}

/*!
    Normlizes the signal or slot definition \a signalSlot by removing
    unnecessary whitespace.
*/

TQCString TQObject::normalizeSignalSlot( const char *signalSlot )
{
    if ( !signalSlot )
	return TQCString();
    return  tqt_rmWS( signalSlot );
}



/*!
    \overload bool TQObject::connect( const TQObject *sender, const char *signal, const char *member ) const

    Connects \a signal from the \a sender object to this object's \a
    member.

    Equivalent to: \c{TQObject::connect(sender, signal, this, member)}.

    \sa disconnect()
*/

/*!
    Connects \a signal from the \a sender object to \a member in object
    \a receiver, and returns true if the connection succeeds; otherwise
    returns false.

    You must use the TQ_SIGNAL() and TQ_SLOT() macros when specifying the \a signal
    and the \a member, for example:
    \code
    TQLabel     *label  = new TQLabel;
    TQScrollBar *scroll = new TQScrollBar;
    TQObject::connect( scroll, TQ_SIGNAL(valueChanged(int)),
                      label,  TQ_SLOT(setNum(int)) );
    \endcode

    This example ensures that the label always displays the current
    scroll bar value. Note that the signal and slots parameters must not
    contain any variable names, only the type. E.g. the following would
    not work and return false:
    TQObject::connect( scroll, TQ_SIGNAL(valueChanged(int v)),
                      label,  TQ_SLOT(setNum(int v)) );

    A signal can also be connected to another signal:

    \code
    class MyWidget : public TQWidget
    {
	TQ_OBJECT
    public:
	MyWidget();

    signals:
	void myUsefulSignal();

    private:
	TQPushButton *aButton;
    };

    MyWidget::MyWidget()
    {
	aButton = new TQPushButton( this );
	connect( aButton, TQ_SIGNAL(clicked()), TQ_SIGNAL(myUsefulSignal()) );
    }
    \endcode

    In this example, the MyWidget constructor relays a signal from a
    private member variable, and makes it available under a name that
    relates to MyWidget.

    A signal can be connected to many slots and signals. Many signals
    can be connected to one slot.

    If a signal is connected to several slots, the slots are activated
    in an arbitrary order when the signal is emitted.

    The function returns true if it successfully connects the signal
    to the slot. It will return false if it cannot create the
    connection, for example, if TQObject is unable to verify the
    existence of either \a signal or \a member, or if their signatures
    aren't compatible.

    A signal is emitted for \e{every} connection you make, so if you
    duplicate a connection, two signals will be emitted. You can
    always break a connection using \c{disconnect()}.

    \sa disconnect()
*/

bool TQObject::connect( const TQObject *sender,	const char *signal,
		       const TQObject *receiver, const char *member )
{
#if defined(QT_CHECK_NULL)
    if ( sender == 0 || receiver == 0 || signal == 0 || member == 0 ) {
	tqWarning( "TQObject::connect: Cannot connect %s::%s to %s::%s",
		 sender ? sender->className() : "(null)",
		 signal ? signal+1 : "(null)",
		 receiver ? receiver->className() : "(null)",
		 member ? member+1 : "(null)" );
	return false;
    }
#endif
    TQMetaObject *smeta = sender->metaObject();

#if defined(QT_CHECK_RANGE)
    if ( !check_signal_macro( sender, signal, "connect", "bind" ) )
	return false;
#endif
    TQCString nw_signal(signal);			// Assume already normalized
    ++signal;					// skip member type code

    int signal_index = smeta->findSignal( signal, true );
    if ( signal_index < 0 ) {			// normalize and retry
	nw_signal = tqt_rmWS( signal-1 );	// remove whitespace
	signal = nw_signal.data()+1;		// skip member type code
	signal_index = smeta->findSignal( signal, true );
    }

    if ( signal_index < 0  ) {			// no such signal
#if defined(QT_CHECK_RANGE)
	err_member_notfound( TQ_SIGNAL_CODE, sender, signal, "connect" );
	err_info_about_candidates( TQ_SIGNAL_CODE, smeta, signal, "connect" );
	err_info_about_objects( "connect", sender, receiver );
#endif
	return false;
    }
    const TQMetaData *sm = smeta->signal( signal_index, true );
    signal = sm->name;				// use name from meta object

    int membcode = member[0] - '0';		// get member code

    TQObject *s = (TQObject *)sender;		// we need to change them
    TQObject *r = (TQObject *)receiver;		//   internally

#if defined(QT_CHECK_RANGE)
    if ( !check_member_code( membcode, r, member, "connect" ) )
	return false;
#endif
    member++;					// skip code

    TQCString nw_member ;
    TQMetaObject *rmeta = r->metaObject();
    int member_index = -1;
    switch ( membcode ) {			// get receiver member
	case TQ_SLOT_CODE:
	    member_index = rmeta->findSlot( member, true );
	    if ( member_index < 0 ) {		// normalize and retry
		nw_member = tqt_rmWS(member);	// remove whitespace
		member = nw_member;
		member_index = rmeta->findSlot( member, true );
	    }
	    break;
	case TQ_SIGNAL_CODE:
	    member_index = rmeta->findSignal( member, true );
	    if ( member_index < 0 ) {		// normalize and retry
		nw_member = tqt_rmWS(member);	// remove whitespace
		member = nw_member;
		member_index = rmeta->findSignal( member, true );
	    }
	    break;
    }
    if ( member_index < 0  ) {
#if defined(QT_CHECK_RANGE)
	err_member_notfound( membcode, r, member, "connect" );
	err_info_about_candidates( membcode, rmeta, member, "connect" );
	err_info_about_objects( "connect", sender, receiver );
#endif
	return false;
    }
#if defined(QT_CHECK_RANGE)
    if ( !s->checkConnectArgs(signal,receiver,member) ) {
	tqWarning( "TQObject::connect: Incompatible sender/receiver arguments"
		 "\n\t%s::%s --> %s::%s",
		 s->className(), signal,
		 r->className(), member );
	return false;
    } else {
	const TQMetaData *rm = membcode == TQ_SLOT_CODE ?
			      rmeta->slot( member_index, true ) :
			      rmeta->signal( member_index, true );
	if ( rm ) {
	    int si = 0;
	    int ri = 0;
	    while ( si < sm->method->count && ri < rm->method->count ) {
		if ( sm->method->parameters[si].inOut == TQUParameter::Out )
		    si++;
		else if ( rm->method->parameters[ri].inOut == TQUParameter::Out )
		    ri++;
		else if ( !TQUType::isEqual( sm->method->parameters[si++].type,
					    rm->method->parameters[ri++].type ) ) {
		    if ( ( TQUType::isEqual( sm->method->parameters[si-1].type, &static_QUType_ptr )
			 && TQUType::isEqual( rm->method->parameters[ri-1].type, &static_QUType_varptr ) )
			|| ( TQUType::isEqual( sm->method->parameters[si-1].type, &static_QUType_varptr )
			     && TQUType::isEqual( rm->method->parameters[ri-1].type, &static_QUType_ptr ) ) )
			continue; // varptr got introduced in 3.1 and is binary compatible with ptr
		    tqWarning( "TQObject::connect: Incompatible sender/receiver marshalling"
			      "\n\t%s::%s --> %s::%s",
			      s->className(), signal,
			      r->className(), member );
		    return false;
		}
	    }
	}
    }
#endif
    connectInternal( sender, signal_index, receiver, membcode, member_index );
    s->connectNotify( nw_signal );
    return true;
}

/*! \internal */

void TQObject::connectInternal( const TQObject *sender, int signal_index, const TQObject *receiver,
			       int membcode, int member_index )
{
    TQObject *s = (TQObject*)sender;
    TQObject *r = (TQObject*)receiver;

    if ( !s->connections ) {			// create connections lookup table
	s->connections = new TQSignalVec( signal_index+1 );
	TQ_CHECK_PTR( s->connections );
	s->connections->setAutoDelete( true );
    }

    TQConnectionList *clist = s->connections->at( signal_index );
    if ( !clist ) {				// create receiver list
	clist = new TQConnectionList;
	TQ_CHECK_PTR( clist );
	clist->setAutoDelete( true );
	s->connections->insert( signal_index, clist );
    }

    TQMetaObject *rmeta = r->metaObject();
    const TQMetaData *rm = 0;

    switch ( membcode ) {			// get receiver member
	case TQ_SLOT_CODE:
	    rm = rmeta->slot( member_index, true );
	    break;
	case TQ_SIGNAL_CODE:
	    rm = rmeta->signal( member_index, true );
	    break;
    }

    TQConnection *c = new TQConnection( r, member_index, rm ? rm->name : "tqt_invoke", membcode );
    TQ_CHECK_PTR( c );
    clist->append( c );
    if ( !r->senderObjects ) {			// create list of senders
#ifdef TQT_THREAD_SUPPORT
	r->d->senderObjectListMutex->lock();
#endif // TQT_THREAD_SUPPORT
	r->senderObjects = new TQSenderObjectList;
#ifdef TQT_THREAD_SUPPORT
	r->senderObjects->listMutex->lock();
	r->d->senderObjectListMutex->unlock();
#endif // TQT_THREAD_SUPPORT
    }
    else {
#ifdef TQT_THREAD_SUPPORT
	r->senderObjects->listMutex->lock();
#endif // TQT_THREAD_SUPPORT
    }
    r->senderObjects->append( s );		// add sender to list
#ifdef TQT_THREAD_SUPPORT
    r->senderObjects->listMutex->unlock();
#endif // TQT_THREAD_SUPPORT
}


/*!
    \overload bool TQObject::disconnect( const char *signal, const TQObject *receiver, const char *member )

    Disconnects \a signal from \a member of \a receiver.

    A signal-slot connection is removed when either of the objects
    involved are destroyed.
*/

/*!
    \overload bool TQObject::disconnect( const TQObject *receiver, const char *member )

    Disconnects all signals in this object from \a receiver's \a
    member.

    A signal-slot connection is removed when either of the objects
    involved are destroyed.
*/

/*!
    Disconnects \a signal in object \a sender from \a member in object
    \a receiver.

    A signal-slot connection is removed when either of the objects
    involved are destroyed.

    disconnect() is typically used in three ways, as the following
    examples demonstrate.
    \list 1
    \i Disconnect everything connected to an object's signals:
       \code
       disconnect( myObject, 0, 0, 0 );
       \endcode
       equivalent to the non-static overloaded function
       \code
       myObject->disconnect();
       \endcode
    \i Disconnect everything connected to a specific signal:
       \code
       disconnect( myObject, TQ_SIGNAL(mySignal()), 0, 0 );
       \endcode
       equivalent to the non-static overloaded function
       \code
       myObject->disconnect( TQ_SIGNAL(mySignal()) );
       \endcode
    \i Disconnect a specific receiver:
       \code
       disconnect( myObject, 0, myReceiver, 0 );
       \endcode
       equivalent to the non-static overloaded function
       \code
       myObject->disconnect(  myReceiver );
       \endcode
    \endlist

    0 may be used as a wildcard, meaning "any signal", "any receiving
    object", or "any slot in the receiving object", respectively.

    The \a sender may never be 0. (You cannot disconnect signals from
    more than one object in a single call.)

    If \a signal is 0, it disconnects \a receiver and \a member from
    any signal. If not, only the specified signal is disconnected.

    If \a receiver is 0, it disconnects anything connected to \a
    signal. If not, slots in objects other than \a receiver are not
    disconnected.

    If \a member is 0, it disconnects anything that is connected to \a
    receiver. If not, only slots named \a member will be disconnected,
    and all other slots are left alone. The \a member must be 0 if \a
    receiver is left out, so you cannot disconnect a
    specifically-named slot on all objects.

    \sa connect()
*/

bool TQObject::disconnect( const TQObject *sender,   const char *signal,
			  const TQObject *receiver, const char *member )
{
#if defined(QT_CHECK_NULL)
    if ( sender == 0 || (receiver == 0 && member != 0) ) {
	tqWarning( "TQObject::disconnect: Unexpected null parameter" );
	return false;
    }
#endif
    if ( !sender->connections )			// no connected signals
	return false;
    TQObject *s = (TQObject *)sender;
    TQObject *r = (TQObject *)receiver;
    int member_index = -1;
    int membcode = -1;
    TQCString nw_member;
    if ( member ) {
	membcode = member[0] - '0';
#if defined(QT_CHECK_RANGE)
	if ( !check_member_code( membcode, r, member, "disconnect" ) )
	    return false;
#endif
	++member;
	TQMetaObject *rmeta = r->metaObject();

	switch ( membcode ) {			// get receiver member
	case TQ_SLOT_CODE:
	    member_index = rmeta->findSlot( member, true );
	    if ( member_index < 0 ) {		// normalize and retry
		nw_member = tqt_rmWS(member);	// remove whitespace
		member = nw_member;
		member_index = rmeta->findSlot( member, true );
	    }
	    break;
	case TQ_SIGNAL_CODE:
	    member_index = rmeta->findSignal( member, true );
	    if ( member_index < 0 ) {		// normalize and retry
		nw_member = tqt_rmWS(member);	// remove whitespace
		member = nw_member;
		member_index = rmeta->findSignal( member, true );
	    }
	    break;
	}
	if ( member_index < 0 ) {		// no such member
#if defined(QT_CHECK_RANGE)
	    err_member_notfound( membcode, r, member, "disconnect" );
	    err_info_about_candidates( membcode, rmeta, member, "connect" );
	    err_info_about_objects( "disconnect", sender, receiver );
#endif
	    return false;
	}
    }

    if ( signal == 0 ) {			// any/all signals
	if ( disconnectInternal( s, -1, r, membcode, member_index ) )
	    s->disconnectNotify( 0 );
	else
	    return false;
    } else {					// specific signal
#if defined(QT_CHECK_RANGE)
	if ( !check_signal_macro( s, signal, "disconnect", "unbind" ) )
	    return false;
#endif
	TQCString nw_signal(signal);		// Assume already normalized
	++signal;				// skip member type code

	TQMetaObject *smeta = s->metaObject();
	if ( !smeta )				// no meta object
	    return false;
	int signal_index = smeta->findSignal( signal, true );
	if ( signal_index < 0 ) {		// normalize and retry
	    nw_signal = tqt_rmWS( signal-1 );	// remove whitespace
	    signal = nw_signal.data()+1;	// skip member type code
	    signal_index = smeta->findSignal( signal, true );
	}
	if ( signal_index < 0 ) {
#if defined(QT_CHECK_RANGE)
		tqWarning( "TQObject::disconnect: No such signal %s::%s",
			 s->className(), signal );
#endif
		return false;
	}

	/* compatibility and safety: If a receiver has several slots
	 * with the same name, disconnect them all*/
	bool res = false;
	if ( membcode == TQ_SLOT_CODE && r ) {
	    TQMetaObject * rmeta = r->metaObject();
	    do {
		int mi = rmeta->findSlot( member );
		if ( mi != -1 )
		    res |= disconnectInternal( s, signal_index, r, membcode,  mi );
	    } while ( (rmeta = rmeta->superClass()) );
	} else {
	    res = disconnectInternal( s, signal_index, r, membcode,  member_index );
	}
	if ( res )
	    s->disconnectNotify( nw_signal );
	return res;
    }
    return true;
}

/*! \internal */

bool TQObject::disconnectInternal( const TQObject *sender, int signal_index,
		  const TQObject *receiver, int membcode, int member_index )
{
    TQObject *s = (TQObject*)sender;
    TQObject *r = (TQObject*)receiver;

    if ( !s->connections )
	return false;

    bool success = false;
    TQConnectionList *clist;
    TQConnection *c;
    if ( signal_index == -1 ) {
	for ( int i = 0; i < (int) s->connections->size(); i++ ) {
	    clist = (*s->connections)[i]; // for all signals...
	    if ( !clist )
		continue;
	    c = clist->first();
	    while ( c ) {			// for all receivers...
		if ( r == 0 ) {			// remove all receivers
#ifdef TQT_THREAD_SUPPORT
		    if (c->object()->senderObjects) c->object()->senderObjects->listMutex->lock();
#endif // TQT_THREAD_SUPPORT
		    removeObjFromList( c->object()->senderObjects, s );
#ifdef TQT_THREAD_SUPPORT
		    if (c->object()->senderObjects) c->object()->senderObjects->listMutex->unlock();
#endif // TQT_THREAD_SUPPORT
		    success = true;
		    c = clist->next();
		} else if ( r == c->object() &&
			    ( (member_index == -1) ||
			      ((member_index == c->member()) && (c->memberType() == membcode)) ) ) {
#ifdef TQT_THREAD_SUPPORT
		    if (c->object()->senderObjects) c->object()->senderObjects->listMutex->lock();
#endif // TQT_THREAD_SUPPORT
		    removeObjFromList( c->object()->senderObjects, s, true );
#ifdef TQT_THREAD_SUPPORT
		    if (c->object()->senderObjects) c->object()->senderObjects->listMutex->unlock();
#endif // TQT_THREAD_SUPPORT
		    success = true;
		    clist->remove();
		    c = clist->current();
		} else {
		    c = clist->next();
		}
	    }
	    if ( r == 0 )			// disconnect all receivers
		s->connections->insert( i, 0 );
	}
    } else {
	clist = s->connections->at( signal_index );
	if ( !clist )
	    return false;

	c = clist->first();
	while ( c ) {				// for all receivers...
	    if ( r == 0 ) {			// remove all receivers
#ifdef TQT_THREAD_SUPPORT
		    if (c->object()->senderObjects) c->object()->senderObjects->listMutex->lock();
#endif // TQT_THREAD_SUPPORT
		removeObjFromList( c->object()->senderObjects, s, true );
#ifdef TQT_THREAD_SUPPORT
		    if (c->object()->senderObjects) c->object()->senderObjects->listMutex->unlock();
#endif // TQT_THREAD_SUPPORT
		success = true;
		c = clist->next();
	    } else if ( r == c->object() &&
			( (member_index == -1) ||
			  ((member_index == c->member()) && (c->memberType() == membcode)) ) ) {
#ifdef TQT_THREAD_SUPPORT
		    if (c->object()->senderObjects) c->object()->senderObjects->listMutex->lock();
#endif // TQT_THREAD_SUPPORT
		removeObjFromList( c->object()->senderObjects, s, true );
#ifdef TQT_THREAD_SUPPORT
		    if (c->object()->senderObjects) c->object()->senderObjects->listMutex->unlock();
#endif // TQT_THREAD_SUPPORT
		success = true;
		clist->remove();
		c = clist->current();
	    } else {
		c = clist->next();
	    }
	}
	if ( r == 0 ) {				// disconnect all receivers
	    s->connections->insert( signal_index, 0 );
	}
    }
    return success;
}

/*!
    \fn TQObject::destroyed()

    This signal is emitted when the object is being destroyed.

    Note that the signal is emitted by the TQObject destructor, so
    the object's virtual table is already degenerated at this point,
    and it is not safe to call any functions on the object emitting
    the signal. This signal can not be blocked.

    All the objects's children are destroyed immediately after this
    signal is emitted.
*/

/*!
    \overload TQObject::destroyed( TQObject* obj)

    This signal is emitted immediately before the object \a obj is
    destroyed, and can not be blocked.

    All the objects's children are destroyed immediately after this
    signal is emitted.
*/

/*!
    Performs a deferred deletion of this object.

    Instead of an immediate deletion this function schedules a
    deferred delete event for processing when TQt returns to the main
    event loop.
*/
void TQObject::deleteLater()
{
    TQApplication::postEvent( this, new TQEvent( TQEvent::DeferredDelete) );
}

/*!
    This slot is connected to the destroyed() signal of other objects
    that have installed event filters on this object. When the other
    object, \a obj, is destroyed, we want to remove its event filter.
*/

void TQObject::cleanupEventFilter(TQObject* obj)
{
    removeEventFilter( obj );
}


/*!
    \fn TQString TQObject::tr( const char *sourceText, const char * comment )
    \reentrant

    Returns a translated version of \a sourceText, or \a sourceText
    itself if there is no appropriate translated version. The
    translation context is TQObject with \a comment (0 by default).
    All TQObject subclasses using the TQ_OBJECT macro automatically have
    a reimplementation of this function with the subclass name as
    context.

    \warning This method is reentrant only if all translators are
    installed \e before calling this method. Installing or removing
    translators while performing translations is not supported. Doing
    so will probably result in crashes or other undesirable behavior.

    \sa trUtf8() TQApplication::translate()
	\link i18n.html Internationalization with TQt\endlink
*/

/*!
    \fn TQString TQObject::trUtf8( const char *sourceText,
                                 const char *comment )
    \reentrant

    Returns a translated version of \a sourceText, or
    TQString::fromUtf8(\a sourceText) if there is no appropriate
    version. It is otherwise identical to tr(\a sourceText, \a
    comment).

    \warning This method is reentrant only if all translators are
    installed \e before calling this method. Installing or removing
    translators while performing translations is not supported. Doing
    so will probably result in crashes or other undesirable behavior.

    \sa tr() TQApplication::translate()
*/

static TQMetaObjectCleanUp cleanUp_TQt = TQMetaObjectCleanUp( "TQObject", &TQObject::staticMetaObject );

TQMetaObject* TQObject::staticTQtMetaObject()
{
    static TQMetaObject* qtMetaObject = 0;
    if ( qtMetaObject )
	return qtMetaObject;

#ifndef TQT_NO_PROPERTIES
    static const TQMetaEnum::Item enum_0[] = {
	{ "AlignLeft",  (int) TQt::AlignLeft },
	{ "AlignRight",  (int) TQt::AlignRight },
	{ "AlignHCenter",  (int) TQt::AlignHCenter },
	{ "AlignTop",  (int) TQt::AlignTop },
	{ "AlignBottom",  (int) TQt::AlignBottom },
	{ "AlignVCenter",  (int) TQt::AlignVCenter },
	{ "AlignCenter", (int) TQt::AlignCenter },
	{ "AlignAuto", (int) TQt::AlignAuto },
	{ "AlignJustify", (int) TQt::AlignJustify },
	{ "WordBreak", (int) TQt::WordBreak }
    };

    static const TQMetaEnum::Item enum_1[] = {
	{ "Horizontal", (int) TQt::Horizontal },
	{ "Vertical", (int) TQt::Vertical }
    };

    static const TQMetaEnum::Item enum_2[] = {
	{ "PlainText", (int) TQt::PlainText },
	{ "RichText", (int) TQt::RichText },
	{ "AutoText", (int) TQt::AutoText },
	{ "LogText", (int) TQt::LogText }
    };

    static const TQMetaEnum::Item enum_3[] = {
	{ "NoBackground",  (int) TQt::NoBackground },
	{ "PaletteForeground",  (int) TQt::PaletteForeground },
	{ "PaletteButton",  (int) TQt::PaletteButton },
	{ "PaletteLight",  (int) TQt::PaletteLight },
	{ "PaletteMidlight",  (int) TQt::PaletteMidlight },
	{ "PaletteDark",  (int) TQt::PaletteDark },
	{ "PaletteMid",  (int) TQt::PaletteMid },
	{ "PaletteText",  (int) TQt::PaletteText },
	{ "PaletteBrightText",  (int) TQt::PaletteBrightText },
	{ "PaletteBase",  (int) TQt::PaletteBase },
	{ "PaletteBackground",  (int) TQt::PaletteBackground },
	{ "PaletteShadow",  (int) TQt::PaletteShadow },
	{ "PaletteHighlight",  (int) TQt::PaletteHighlight },
	{ "PaletteHighlightedText",  (int) TQt::PaletteHighlightedText },
	{ "PaletteButtonText",  (int) TQt::PaletteButtonText },
	{ "PaletteLink", (int) TQt::PaletteLink },
	{ "PaletteLinkVisited", (int) TQt::PaletteLinkVisited }
    };

    static const TQMetaEnum::Item enum_4[] = {
	{ "TextDate", (int) TQt::TextDate },
	{ "ISODate", (int) TQt::ISODate },
	{ "LocalDate", (int) TQt::LocalDate }
    };


    static const TQMetaEnum enum_tbl[] = {
	{ "Alignment", 10, enum_0, true },
	{ "Orientation", 2, enum_1, false },
	{ "TextFormat", 4, enum_2, false },
	{ "BackgroundMode", 17, enum_3, false },
	{ "DateFormat", 3, enum_4, false }
    };
#endif

    qtMetaObject = new TQMetaObject( "TQt", 0,
			  0, 0,
			  0, 0,
#ifndef TQT_NO_PROPERTIES
			  0, 0,
			  enum_tbl, 5,
#endif
			  0, 0 );
    cleanUp_TQt.setMetaObject( qtMetaObject );

    return qtMetaObject;
}

/*!
  \internal

  Signal activation with the most frequently used parameter/argument
    types. All other combinations are generated by the meta object
    compiler.
  */
void TQObject::activate_signal( int signal )
{
#ifndef TQT_NO_PRELIMINARY_SIGNAL_SPY
    if ( tqt_preliminary_signal_spy ) {
	if ( !signalsBlocked() && signal >= 0 &&
	     ( !connections || !connections->at( signal ) ) ) {
	    TQUObject o[1];
	    o[0].isLastObject = true;
	    tqt_spy_signal( this, signal, o );
	    return;
	}
    }
#endif

    if ( !connections || signalsBlocked() || signal < 0 ) {
	return;
    }
    TQConnectionList *clist = connections->at( signal );
    if ( !clist ) {
	return;
    }
    TQUObject o[1];
    o[0].isLastObject = true;
    activate_signal( clist, o );
}

/*! \internal */

void TQObject::activate_signal( TQConnectionList *clist, TQUObject *o )
{
    if ( !clist ) {
	return;
    }

#ifndef TQT_NO_PRELIMINARY_SIGNAL_SPY
    if ( tqt_preliminary_signal_spy ) {
	tqt_spy_signal( this, connections->findRef( clist), o );
    }
#endif

#ifdef TQT_THREAD_SUPPORT
    // NOTE currentThread could be NULL if the current thread was not started using the TQThread API
    const TQThread *currentThread = TQThread::currentThreadObject();
#endif // TQT_THREAD_SUPPORT

    TQObject *object;
    TQSenderObjectList* sol;
    TQObject* oldSender = 0;
    TQConnection *c;
    if ( clist->count() == 1 ) { // save iterator
	c = clist->first();
	object = c->object();
	sol = object->senderObjects;
#ifdef TQT_THREAD_SUPPORT
	sol->listMutex->lock();
#endif // TQT_THREAD_SUPPORT
	if ( sol ) {
	    oldSender = sol->currentSender;
	    sol->ref();
	    sol->currentSender = this;
	}
	if ( c->memberType() == TQ_SIGNAL_CODE ) {
#if !defined(TQT_THREAD_SUPPORT)
	    object->tqt_emit( c->member(), o );
#else // defined(TQT_THREAD_SUPPORT)
	    if ((d->disableThreadPostedEvents) || 
	        (object->d->disableThreadPostedEvents) || 
	        (currentThread && currentThread->threadPostedEventsDisabled()) || 
	        (currentThread && object->d->ownThread == currentThread)) {
		sol->listMutex->unlock();
		object->tqt_emit( c->member(), o );
		sol->listMutex->lock();
	    }
	    else {
		if (object->d->ownThread && !object->d->ownThread->finished()) {
#ifdef QT_DEBUG
		    tqDebug("TQObject::activate_signal: Emitting cross-thread signal from object %p (member %d receiver %p) (1)\n", this, c->member(), object);
#endif // QT_DEBUG
		    TQApplication::postEvent(object, new TQMetaCallEvent(c->member(), this, deepCopyTQUObjectArray(o), TQMetaCallEvent::MetaCallEmit));
		}
	    }
#endif // !defined(TQT_THREAD_SUPPORT)
	}
	else {
#if !defined(TQT_THREAD_SUPPORT)
	    object->tqt_invoke( c->member(), o );
#else // defined(TQT_THREAD_SUPPORT)
	    if ((d->disableThreadPostedEvents) || 
	        (object->d->disableThreadPostedEvents) || 
	        (currentThread && currentThread->threadPostedEventsDisabled()) || 
	        (currentThread && object->d->ownThread == currentThread)) {
		sol->listMutex->unlock();
		object->tqt_invoke( c->member(), o );
		sol->listMutex->lock();
	    }
	    else {
		if (object->d->ownThread && !object->d->ownThread->finished()) {
#ifdef QT_DEBUG
		    tqDebug("TQObject::activate_signal: Invoking cross-thread method from object %p (member %d receiver %p) (1)\n", this, c->member(), object);
#endif // QT_DEBUG
		    TQApplication::postEvent(object, new TQMetaCallEvent(c->member(), this, deepCopyTQUObjectArray(o), TQMetaCallEvent::MetaCallInvoke));
		}
	    }
#endif // !defined(TQT_THREAD_SUPPORT)
	}
	if ( sol ) {
	    sol->currentSender = oldSender;
	    if ( sol->deref() ) {
#ifdef TQT_THREAD_SUPPORT
	        sol->listMutex->unlock();
#endif // TQT_THREAD_SUPPORT
		delete sol;
		sol = NULL;
	    }
	}
#ifdef TQT_THREAD_SUPPORT
	if (sol) sol->listMutex->unlock();
#endif // TQT_THREAD_SUPPORT
    } else {
	TQConnection *cd = 0;
	TQConnectionListIt it(*clist);
	while ( (c=it.current()) ) {
	    ++it;
	    if ( c == cd )
		continue;
	    cd = c;
	    object = c->object();
	    sol = object->senderObjects;
#ifdef TQT_THREAD_SUPPORT
	sol->listMutex->lock();
#endif // TQT_THREAD_SUPPORT
	    if ( sol ) {
		oldSender = sol->currentSender;
		sol->ref();
		sol->currentSender = this;
	    }
	    if ( c->memberType() == TQ_SIGNAL_CODE ) {
#if !defined(TQT_THREAD_SUPPORT)
		object->tqt_emit( c->member(), o );
#else // defined(TQT_THREAD_SUPPORT)
		if ((d->disableThreadPostedEvents) || 
		    (object->d->disableThreadPostedEvents) || 
		    (currentThread && currentThread->threadPostedEventsDisabled()) || 
		    (currentThread && object->d->ownThread == currentThread)) {
		    sol->listMutex->unlock();
		    object->tqt_emit( c->member(), o );
		    sol->listMutex->lock();
		}
		else {
		    if (object->d->ownThread && !object->d->ownThread->finished()) {
#ifdef QT_DEBUG
			tqDebug("TQObject::activate_signal: Emitting cross-thread signal from object %p (member %d receiver %p) (2)\n", this, c->member(), object);
#endif // QT_DEBUG
			TQApplication::postEvent(object, new TQMetaCallEvent(c->member(), this, deepCopyTQUObjectArray(o), TQMetaCallEvent::MetaCallEmit));
		    }
		}
#endif // !defined(TQT_THREAD_SUPPORT)
	    }
	    else {
#if !defined(TQT_THREAD_SUPPORT)
		object->tqt_invoke( c->member(), o );
#else // defined(TQT_THREAD_SUPPORT)
		if ((d->disableThreadPostedEvents) || 
		    (object->d->disableThreadPostedEvents) || 
		    (currentThread && currentThread->threadPostedEventsDisabled()) || 
		    (currentThread && object->d->ownThread == currentThread)) {
		    sol->listMutex->unlock();
		    object->tqt_invoke( c->member(), o );
		    sol->listMutex->lock();
		}
		else {
		    if (object->d->ownThread && !object->d->ownThread->finished()) {
#ifdef QT_DEBUG
			tqDebug("TQObject::activate_signal: Invoking cross-thread method from object %p (member %d receiver %p) (2)\n", this, c->member(), object);
#endif // QT_DEBUG
			TQApplication::postEvent(object, new TQMetaCallEvent(c->member(), this, deepCopyTQUObjectArray(o), TQMetaCallEvent::MetaCallInvoke));
		    }
		}
#endif // !defined(TQT_THREAD_SUPPORT)
	    }
	    if (sol ) {
		sol->currentSender = oldSender;
		if ( sol->deref() ) {
#ifdef TQT_THREAD_SUPPORT
		    sol->listMutex->unlock();
#endif // TQT_THREAD_SUPPORT
		    delete sol;
		    sol = NULL;
		}
	    }
#ifdef TQT_THREAD_SUPPORT
	    if (sol) sol->listMutex->unlock();
#endif // TQT_THREAD_SUPPORT
	}
    }
}

/*!
    \overload void TQObject::activate_signal( int signal, int )
*/

/*!
    \overload void TQObject::activate_signal( int signal, double )
*/

/*!
    \overload void TQObject::activate_signal( int signal, TQString )
*/

/*!
  \fn void TQObject::activate_signal_bool( int signal, bool )
  \internal

  Like the above functions, but since bool is sometimes
  only a typedef it cannot be a simple overload.
*/

#ifndef TQT_NO_PRELIMINARY_SIGNAL_SPY
#define ACTIVATE_SIGNAL_WITH_PARAM(FNAME,TYPE)					\
void TQObject::FNAME( int signal, TYPE param )					\
{										\
    if ( tqt_preliminary_signal_spy ) {						\
	if ( !signalsBlocked() && signal >= 0 &&				\
	     ( !connections || !connections->at( signal ) ) ) {			\
	    TQUObject o[2];							\
	    o[1].isLastObject = true;						\
	    static_QUType_##TYPE.set( o+1, param );				\
	    tqt_spy_signal( this, signal, o );					\
	    return;								\
	}									\
    }										\
    if ( !connections || signalsBlocked() || signal < 0 )			\
	return;									\
    TQConnectionList *clist = connections->at( signal );			\
    if ( !clist )								\
	return;									\
    TQUObject o[2];								\
    o[1].isLastObject = true;							\
    static_QUType_##TYPE.set( o+1, param );					\
    activate_signal( clist, o );						\
}
#else
#define ACTIVATE_SIGNAL_WITH_PARAM(FNAME,TYPE)					\
void TQObject::FNAME( int signal, TYPE param )					\
{										\
    if ( !connections || signalsBlocked() || signal < 0 )			\
	return;									\
    TQConnectionList *clist = connections->at( signal );			\
    if ( !clist )								\
	return;									\
    TQUObject o[2];								\
    o[1].isLastObject = true;							\
    static_QUType_##TYPE.set( o+1, param );					\
    activate_signal( clist, o );						\
}

#endif
// We don't want to duplicate too much text so...

ACTIVATE_SIGNAL_WITH_PARAM( activate_signal, int )
ACTIVATE_SIGNAL_WITH_PARAM( activate_signal, double )
ACTIVATE_SIGNAL_WITH_PARAM( activate_signal, TQString )
ACTIVATE_SIGNAL_WITH_PARAM( activate_signal_bool, bool )


/*****************************************************************************
  TQObject debugging output routines.
 *****************************************************************************/

static void dumpRecursive( int level, TQObject *object )
{
#if defined(QT_DEBUG)
    if ( object ) {
	TQString buf;
	buf.fill( '\t', level/2 );
	if ( level % 2 )
	    buf += "    ";
	const char *name = object->name();
	TQString flags="";
	if ( tqApp->focusWidget() == object )
	    flags += 'F';
	if ( object->isWidgetType() ) {
	    TQWidget * w = (TQWidget *)object;
	    if ( w->isVisible() ) {
		TQString t( "<%1,%2,%3,%4>" );
		flags += t.arg(w->x()).arg(w->y()).arg(w->width()).arg(w->height());
	    } else {
		flags += 'I';
	    }
	}
	tqDebug( "%s%s::%s %s", (const char*)buf, object->className(), name,
	    flags.latin1() );
	if ( object->children() ) {
	    TQObjectListIt it(*object->children());
	    TQObject * c;
	    while ( (c=it.current()) != 0 ) {
		++it;
		dumpRecursive( level+1, c );
	    }
	}
    }
#else
    Q_UNUSED( level )
    Q_UNUSED( object )
#endif
}

/*!
    Dumps a tree of children to the debug output.

    This function is useful for debugging, but does nothing if the
    library has been compiled in release mode (i.e. without debugging
    information).
*/

void TQObject::dumpObjectTree()
{
    dumpRecursive( 0, this );
}

/*!
    Dumps information about signal connections, etc. for this object
    to the debug output.

    This function is useful for debugging, but does nothing if the
    library has been compiled in release mode (i.e. without debugging
    information).
*/

void TQObject::dumpObjectInfo()
{
#ifdef TQT_THREAD_SUPPORT
    TQMutexLocker locker( d->senderObjectListMutex );
#endif // TQT_THREAD_SUPPORT

#if defined(QT_DEBUG)
    tqDebug( "OBJECT %s::%s", className(), name( "unnamed" ) );
    int n = 0;
    tqDebug( "  SIGNALS OUT" );
    if ( connections ) {
	TQConnectionList *clist;
	for ( uint i = 0; i < connections->size(); i++ ) {
	    if ( ( clist = connections->at( i ) ) ) {
		tqDebug( "\t%s", metaObject()->signal( i, true )->name );
		n++;
		TQConnection *c;
		TQConnectionListIt cit(*clist);
		while ( (c=cit.current()) ) {
		    ++cit;
		    tqDebug( "\t  --> %s::%s %s", c->object()->className(),
			    c->object()->name( "unnamed" ), c->memberName() );
		}
	    }
	}
    }
    if ( n == 0 )
	tqDebug( "\t<None>" );

    tqDebug( "  SIGNALS IN" );
    n = 0;
    if ( senderObjects ) {
	TQObject *sender = senderObjects->first();
	while ( sender ) {
	    tqDebug( "\t%s::%s",
		   sender->className(), sender->name( "unnamed" ) );
	    n++;
	    sender = senderObjects->next();
	}
    }
    if ( n == 0 ) {
	tqDebug( "\t<None>" );
    }
#endif
}

#ifndef TQT_NO_PROPERTIES

/*!
    Sets the value of the object's \a name property to \a value.

    Returns true if the operation was successful; otherwise returns
    false.

    Information about all available properties is provided through the
    metaObject().

    \sa property(), metaObject(), TQMetaObject::propertyNames(), TQMetaObject::property()
*/
bool TQObject::setProperty( const char *name, const TQVariant& value )
{
    if ( !value.isValid() )
	return false;

    TQVariant v = value;

    TQMetaObject* meta = metaObject();
    if ( !meta )
	return false;
    int id = meta->findProperty( name, true );
    const TQMetaProperty* p = meta->property( id, true );
    if ( !p || !p->isValid() || !p->writable() ) {
	tqWarning( "%s::setProperty( \"%s\", value ) failed: property invalid, read-only or does not exist",
		  className(), name );
	return false;
    }

    if ( p->isEnumType() ) {
	if ( v.type() == TQVariant::String || v.type() == TQVariant::CString ) {
	    if ( p->isSetType() ) {
		TQString s = value.toString();
		// TQStrList does not support split, use TQStringList for that.
		TQStringList l = TQStringList::split( '|', s );
		TQStrList keys;
		for ( TQStringList::Iterator it = l.begin(); it != l.end(); ++it )
		    keys.append( (*it).stripWhiteSpace().latin1() );
		v = TQVariant( p->keysToValue( keys ) );
	    } else {
		v = TQVariant( p->keyToValue( value.toCString().data() ) );
	    }
	} else if ( v.type() != TQVariant::Int && v.type() != TQVariant::UInt ) {
	    return false;
	}
	return tqt_property( id, 0, &v );
    }

    TQVariant::Type type = (TQVariant::Type)(p->flags >> 24);
    if ( type == TQVariant::Invalid )
	type = TQVariant::nameToType( p->type() );
    if ( type != TQVariant::Invalid && !v.canCast( type ) )
	return false;
    return tqt_property( id, 0, &v );
}

/*!
    Returns the value of the object's \a name property.

    If no such property exists, the returned variant is invalid.

    Information about all available properties are provided through
    the metaObject().

    \sa setProperty(), TQVariant::isValid(), metaObject(),
    TQMetaObject::propertyNames(), TQMetaObject::property()
*/
TQVariant TQObject::property( const char *name ) const
{
    TQVariant v;
    TQMetaObject* meta = metaObject();
    if ( !meta )
	return v;
    int id = meta->findProperty( name, true );
    const TQMetaProperty* p = meta->property( id, true );
    if ( !p || !p->isValid() ) {
	tqWarning( "%s::property( \"%s\" ) failed: property invalid or does not exist",
		  className(), name );
	return v;
    }
    TQObject* that = (TQObject*) this; // moc ensures constness for the tqt_property call
    that->tqt_property( id, 1, &v );
    return v;
}

#endif // TQT_NO_PROPERTIES

#ifndef TQT_NO_USERDATA
/*!\internal
 */
uint TQObject::registerUserData()
{
    static int user_data_registration = 0;
    return user_data_registration++;
}

/*!\internal
 */
TQObjectUserData::~TQObjectUserData()
{
}

/*!\internal
 */
void TQObject::setUserData( uint id, TQObjectUserData* data)
{
    if ( !d )
	d = new TQObjectPrivate( id+1 );
    if ( id >= d->size() )
	d->resize( id+1 );
    d->insert( id, data );
}

/*!\internal
 */
TQObjectUserData* TQObject::userData( uint id ) const
{
    if ( d && id < d->size() )
	return d->at( id );
    return 0;
}

#endif // TQT_NO_USERDATA
