/****************************************************************************
**
** Definition of TQObject class
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

#ifndef TQOBJECT_H
#define TQOBJECT_H

#ifndef QT_H
#include "ntqobjectdefs.h"
#include "ntqwindowdefs.h"
#include "ntqstring.h"
#include "ntqevent.h"
#include "ntqmutex.h"
#include "ntqnamespace.h"
#endif // QT_H

#define TQT_TR_NOOP(x) (x)
#define TQT_TRANSLATE_NOOP(scope,x) (x)

class TQMetaObject;
class TQVariant;
class TQMetaProperty;
class TQPostEventList;
class TQSenderObjectList;
class TQObjectPrivate;
#ifndef TQT_NO_USERDATA
class TQObjectUserData;
#endif
struct TQUObject;

#ifdef TQT_THREAD_SUPPORT
class TQThread;
#endif

class TQStyleControlElementData;
class TQStyleControlElementDataPrivate;

class TQ_EXPORT TQObject: public TQt
{
    TQ_OBJECT
    TQ_PROPERTY( TQCString name READ name WRITE setName )

public:
    TQObject( TQObject *parent=0, const char *name=0 );
    virtual ~TQObject();

#ifdef Q_QDOC
    virtual const char *className() const;
    static TQString tr( const char *, const char * );
    static TQString trUtf8( const char *, const char * );
    virtual TQMetaObject *metaObject() const;
#endif

    virtual bool event( TQEvent * );
    virtual bool eventFilter( TQObject *, TQEvent * );

    bool	 isA( const char * )	 const;
    bool	 inherits( const char * ) const;

    const char  *name() const;
    const char  *name( const char * defaultName ) const;

    virtual void setName( const char *name );
    bool	 isWidgetType()	  const { return isWidget; }
    bool	 highPriority()	  const { return false; }

    bool	 signalsBlocked()  const { return blockSig; }
    void	 blockSignals( bool b );

    int		 startTimer( int interval );
    void	 killTimer( int id );
    void	 killTimers();

    TQObject           *child( const char *objName, const char *inheritsClass = 0, bool recursiveSearch = true ); //### const in 4.0
    const TQObjectList *children() const { return childObjects; }
    TQObjectList childrenListObject();
    const TQObjectList childrenListObject() const;

    static const TQObjectList *objectTrees();
    static const TQObjectList objectTreesListObject();

    TQObjectList	      *queryList( const char *inheritsClass = 0,
				  const char *objName = 0,
				  bool regexpMatch = true,
				  bool recursiveSearch = true ) const;

    virtual void insertChild( TQObject * );
    virtual void removeChild( TQObject * );

    void	 installEventFilter( const TQObject * );
    void	 removeEventFilter( const TQObject * );

    static bool  connect( const TQObject *sender, const char *signal,
			  const TQObject *receiver, const char *member );
    bool	 connect( const TQObject *sender, const char *signal,
			  const char *member ) const;
    static bool  disconnect( const TQObject *sender, const char *signal,
			     const TQObject *receiver, const char *member );
    bool	 disconnect( const char *signal=0,
			     const TQObject *receiver=0, const char *member=0 );
    bool	 disconnect( const TQObject *receiver, const char *member=0 );
    static void 	 connectInternal( const TQObject *sender, int signal_index,
			  const TQObject *receiver, int membcode, int member_index );
    static bool 	 disconnectInternal( const TQObject *sender, int signal_index,
			     const TQObject *receiver, int membcode, int member_index );

    void	 dumpObjectTree();
    void	 dumpObjectInfo();

    TQStyleControlElementData* controlElementDataObject();
    TQStyleControlElementDataPrivate* controlElementDataPrivateObject();

#ifndef TQT_NO_PROPERTIES
    virtual bool setProperty( const char *name, const TQVariant& value );
    virtual TQVariant property( const char *name ) const;
#endif // TQT_NO_PROPERTIES
#ifdef TQT_NO_TRANSLATION
    static TQString tr( const char *sourceText, const char * = 0);
#ifndef TQT_NO_TEXTCODEC
    static TQString trUtf8( const char *sourceText, const char * = 0);
#endif
#endif //TQT_NO_TRANSLATION

#ifndef TQT_NO_USERDATA
    static uint registerUserData();
    void setUserData( uint id, TQObjectUserData* data);
    TQObjectUserData* userData( uint id ) const;
#endif // TQT_NO_USERDATA

signals:
    void	 destroyed();
    void	 destroyed( TQObject* obj );

public:
    TQObject	*parent() const { return parentObj; }

public slots:
    void	deleteLater();

private slots:
    void	 cleanupEventFilter( TQObject* );

protected:
    bool	activate_filters( TQEvent * );
    TQConnectionList *receivers( const char* signal ) const;
    TQConnectionList *receivers( int signal ) const;
    void	activate_signal( int signal );
    void	activate_signal( int signal, int );
    void	activate_signal( int signal, double );
    void	activate_signal( int signal, TQString );
    void	activate_signal_bool( int signal, bool );
    void 	activate_signal( TQConnectionList *clist, TQUObject *o );

    const TQObject *sender();

    virtual void timerEvent( TQTimerEvent * );
    virtual void childEvent( TQChildEvent * );
    virtual void customEvent( TQCustomEvent * );

    virtual void connectNotify( const char *signal );
    virtual void disconnectNotify( const char *signal );
    virtual bool checkConnectArgs( const char *signal, const TQObject *receiver,
				   const char *member );
    static TQCString normalizeSignalSlot( const char *signalSlot );

private:
    uint	isSignal   : 1;
    uint	isWidget   : 1;
    uint	pendTimer  : 1;
    uint	blockSig   : 1;
    uint	wasDeleted : 1;
    uint	isTree : 1;

    const char	*objname;
    TQObject	*parentObj;
    TQObjectList *childObjects;
    TQSignalVec *connections;
    TQSenderObjectList *senderObjects;
    TQObjectList *eventFilters;
    TQPostEventList *postedEvents;
    TQObjectPrivate* d;

    static TQMetaObject* staticTQtMetaObject();

    friend class TQApplication;
    friend class TQBaseApplication;
    friend class TQWidget;
    friend class TQSignal;

private:	// Disabled copy constructor and operator=
#if defined(TQ_DISABLE_COPY)
    TQObject( const TQObject & );
    TQObject &operator=( const TQObject & );
#endif

public:
#ifdef TQT_THREAD_SUPPORT
    TQThread* contextThreadObject() const;
    void moveToThread(TQThread *targetThread);
    void disableThreadPostedEvents(bool disable);
#endif

private:
    void cleanupControlElementData();
#ifdef TQT_THREAD_SUPPORT
    void moveToThread_helper(TQThread *targetThread);
    void setThreadObject_helper(TQThread *targetThread);
#endif
};


#ifndef TQT_NO_USERDATA
class TQ_EXPORT TQObjectUserData {
public:
    virtual ~TQObjectUserData();
};
#endif


inline bool TQObject::connect( const TQObject *sender, const char *signal,
			      const char *member ) const
{
    return connect( sender, signal, this, member );
}


inline bool TQObject::disconnect( const char *signal,
				 const TQObject *receiver, const char *member )
{
    return disconnect( this, signal, receiver, member );
}


inline bool TQObject::disconnect( const TQObject *receiver, const char *member )
{
    return disconnect( this, 0, receiver, member );
}


#ifdef TQT_NO_TRANSLATION
inline TQString TQObject::tr( const char *sourceText, const char * ) {
    return TQString::fromLatin1( sourceText );
}
#ifndef TQT_NO_TEXTCODEC
inline TQString TQObject::trUtf8( const char *sourceText, const char * ) {
    return TQString::fromUtf8( sourceText );
}
#endif
#endif //TQT_NO_TRANSLATION


#define Q_DEFINED_QOBJECT
#include "ntqwinexport.h"
#endif // TQOBJECT_H
