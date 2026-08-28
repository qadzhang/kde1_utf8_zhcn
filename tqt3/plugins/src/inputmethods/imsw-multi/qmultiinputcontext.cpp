/****************************************************************************
** $Id$
**
** Implementation of TQMultiInputContext class
**
** Copyright (C) 2004 immodule for TQt Project.  All rights reserved.
**
** This file is written to contribute to Trolltech AS under their own
** licence. You may use this file under your TQt license. Following
** description is copied from their original file headers. Contact
** immodule-qt@freedesktop.org if any conditions of this licensing are
** not clear to you.
**
**
** This file is part of the input method module of the TQt GUI Toolkit.
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
** Licensees holding valid TQt Enterprise Edition or TQt Professional Edition
** licenses may use this file in accordance with the TQt Commercial License
** Agreement provided with the Software.
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

#ifndef TQT_NO_IM
#include "qmultiinputcontext.h"
#include <ntqinputcontextfactory.h>
#include <ntqstringlist.h>
#include <ntqpopupmenu.h>
#ifndef TQT_NO_IM_EXTENSIONS
#include <ntqsettings.h>
#endif

#include <cstdlib>

#define TQT_NO_IM_QMULTIINPUTCONTEXT_IMINDEX

TQMultiInputContext::TQMultiInputContext()
    : TQInputContext(), _slave( 0 ), imIndex( 0 ), cachedFocus( false ),
      cachedFocusWidget( 0 ), cachedHolderWidget( 0 ),
      beIndirectlyConnected( false ), popup( NULL ), currentIMKey( TQString::null )
{
    keyDict.setAutoDelete( true );
    keyDict.clear();

    if ( getenv( "TQT_IM_MODULE" ) ) {
        currentIMKey = getenv( "TQT_IM_MODULE" );
    } else {
#ifndef TQT_NO_IM_EXTENSIONS
	TQSettings settings;
        currentIMKey = settings.readEntry( "/qt/DefaultInputMethod", "xim" );
#else
	currentIMKey = "xim";
#endif
    }
}

TQMultiInputContext::~TQMultiInputContext()
{
    keyDict.clear();
}


TQString TQMultiInputContext::identifierName()
{
    return ( slave() ) ? slave()->identifierName() : "";
}

TQString TQMultiInputContext::language()
{
    return ( slave() ) ? slave()->language() : "";
}


#if defined(TQ_WS_X11)
bool TQMultiInputContext::x11FilterEvent( TQWidget *keywidget, XEvent *event )
{
    return ( slave() ) ? slave()->x11FilterEvent( keywidget, event ) : false;
}
#endif // TQ_WS_X11


bool TQMultiInputContext::filterEvent( const TQEvent *event )
{
#if !defined(TQT_NO_IM_QMULTIINPUTCONTEXT_IMINDEX)
    if ( event->type() == TQEvent::KeyPress ) {
	TQKeyEvent *keyevent = (TQKeyEvent *)event;

	// filter selection key
	// Control+Alt+Key_Down: change to next input method
	// Control+Alt+Key_Up:   change to previous input method
	if ( ( keyevent->state() & TQt::ControlButton ) &&
	     ( keyevent->state() & TQt::AltButton ) ) {
	    if ( keyevent->key() == TQt::Key_Up ) {
		changeInputMethod( --imIndex );
		return true;
	    } else if ( keyevent->key() == TQt::Key_Down ) {
		changeInputMethod( ++imIndex );
		return true;
	    }
	}
    }
#endif

    return ( slave() ) ? slave()->filterEvent( event ) : false;
}

void TQMultiInputContext::reset()
{
    if ( slave() )
	slave()->reset();
}


void TQMultiInputContext::setFocus()
{
    cachedFocus = true;
    if ( slave() )
	slave()->setFocus();
}

void TQMultiInputContext::unsetFocus()
{
    cachedFocus = false;
    if ( slave() )
	slave()->unsetFocus();
}

void TQMultiInputContext::setMicroFocus( int x, int y, int w, int h, TQFont *f )
{
    if ( slave() )
	slave()->setMicroFocus( x, y, w, h, f );
}

void TQMultiInputContext::mouseHandler( int x, TQEvent::Type type,
				       TQt::ButtonState button,
				       TQt::ButtonState state )
{
    if ( slave() )
	slave()->mouseHandler( x, type, button, state );
}

TQFont TQMultiInputContext::font() const
{
    return ( slave() ) ? slave()->font() : TQInputContext::font();
}

void TQMultiInputContext::destroyInputContext()
{
    if ( _slave ) {
	// _slave->reset() may not properly work in the case, so we
	// manually resets the composing state of text widget
	if ( _slave->focusWidget() ) {
	    TQIMEvent *terminator = new TQIMEvent( TQEvent::IMEnd, TQString::null, -1 );
	    emit imEventGenerated( _slave->focusWidget(), terminator );
	}
	_slave->deleteLater();
	_slave = 0;
    }
}


/*!
    This function is a placeholder for future experiment or extension
    such as commit string snooping. set beIndirectlyConnected = true
    to activate this virtual function.
*/
void TQMultiInputContext::postIMEvent( TQObject *receiver, TQIMEvent *event )
{
    emit imEventGenerated( receiver, event );
}


#if defined(TQ_WS_X11)
TQWidget *TQMultiInputContext::focusWidget() const
{
    return ( slave() ) ? slave()->focusWidget() : 0;
}

TQWidget *TQMultiInputContext::holderWidget() const
{
    return ( slave() ) ? slave()->holderWidget() : 0;
}


void TQMultiInputContext::setFocusWidget( TQWidget *w )
{
    cachedFocusWidget = w;
    if ( slave() )
	slave()->setFocusWidget( w );
}

void TQMultiInputContext::setHolderWidget( TQWidget *w )
{
    cachedHolderWidget = w;
    if ( slave() )
	slave()->setHolderWidget( w );
}

void TQMultiInputContext::releaseComposingWidget( TQWidget *w )
{
    if ( slave() )
	slave()->releaseComposingWidget( w );
}

#endif

bool TQMultiInputContext::isComposing() const
{
    return ( slave() ) ? slave()->isComposing() : false;
}

bool TQMultiInputContext::isPreeditRelocationEnabled()
{
    return ( slave() ) ? slave()->isPreeditRelocationEnabled() : false;
}

TQInputContext *TQMultiInputContext::slave()
{
    if ( ! _slave ) {
#if !defined(TQT_NO_IM_QMULTIINPUTCONTEXT_IMINDEX)
	changeInputMethod( imIndex );
#else
	changeInputMethod( currentIMKey );
#endif
    }

    return _slave;
}

const TQInputContext *TQMultiInputContext::slave() const
{
    return _slave;
}

void TQMultiInputContext::changeInputMethod( int newIndex )
{
#if !defined(TQT_NO_IM_QMULTIINPUTCONTEXT_IMINDEX)
    TQStringList keys = TQInputContextFactory::keys();
    if ( keys.size() == 0 )
	return;

    if ( newIndex >= (int)keys.size() ) {
	imIndex = 0;
    } else if ( newIndex < 0 ) {
	imIndex = keys.size() - 1;
    } else {
	imIndex = newIndex;
    }

    changeInputMethod( keys[imIndex] );
#endif
}

void TQMultiInputContext::changeInputMethod( TQString key )
{
    TQStringList keys = TQInputContextFactory::keys();
    if ( keys.size() == 0 )
	return;

    if ( key.isEmpty() )
	key = keys[0];

    if ( _slave ) {
	_slave->reset();
	delete _slave;
    }

    _slave = TQInputContextFactory::create( key, cachedHolderWidget );
    if ( _slave ) {
	insertChild( _slave );

	const char *method;
	if ( beIndirectlyConnected ) {
	    method = TQ_SLOT(imEventReceived(TQObject *,TQIMEvent *));
	} else {
	    method = TQ_SIGNAL(imEventGenerated(TQObject *,TQIMEvent *));
	}
	connect( _slave, TQ_SIGNAL(imEventGenerated(TQObject *,TQIMEvent *)),
		 this, method );
	connect( _slave, TQ_SIGNAL(deletionRequested()),
		 this, TQ_SLOT(destroyInputContext()) );

	if ( cachedFocus ) {
	    _slave->setFocus();
	    _slave->setFocusWidget( cachedFocusWidget );
	}

        currentIMKey = key;

	//tqDebug( "TQMultiInputContext::changeInputMethod(): index=%d, slave=%s",
	//	imIndex, (const char *)_slave->identifierName() );
    }
}

TQPtrList<TQInputContextMenu> *TQMultiInputContext::menus()
{
    TQInputContextMenu *imSelMenu = new TQInputContextMenu;
    imSelMenu->title = tr( "Select Input &Method" );
    imSelMenu->popup = createImSelPopup();

    TQPtrList<TQInputContextMenu> *result = new TQPtrList<TQInputContextMenu>;
    result->append( imSelMenu );

    TQPtrList<TQInputContextMenu> *slaveMenus = ( slave() ) ? slave()->menus() : 0;
    if ( slaveMenus ) {
	for ( TQPtrList<TQInputContextMenu>::Iterator it = slaveMenus->begin();
	      it != slaveMenus->end();
	      ++it ) {
	    TQInputContextMenu *slaveMenu = *it;
	    result->append( slaveMenu );
	}
	delete slaveMenus;
    }

    return result;
}

TQPopupMenu *TQMultiInputContext::createImSelPopup()
{
    if ( popup )
        delete popup;

    popup = new TQPopupMenu();
    keyDict.clear();

    TQStringList keys = TQInputContextFactory::keys();
    for ( uint i=0; i < keys.size(); i++ ) {
	TQString idName = keys[i];
	bool isIMSwitcher = idName.startsWith( "imsw-" );

	if ( ! isIMSwitcher ) {
	    TQString dispName = TQInputContextFactory::displayName( idName );
	    if ( dispName.isEmpty() )
		dispName = idName;

	    int id = popup->insertItem( dispName );
	    keyDict.insert( (long)id, new TQString( idName ) );

	    if ( idName == currentIMKey )
		popup->setItemChecked( id, true );

            TQString descriptionStr = TQInputContextFactory::description( idName );
            if ( ! descriptionStr.isEmpty() )
                popup->setWhatsThis( id, descriptionStr );
	}
    }

    TQObject::connect( popup, TQ_SIGNAL(activated(int)),
                      this, TQ_SLOT(changeInputMethodWithMenuId(int)) );

    return popup;
}

void TQMultiInputContext::changeInputMethodWithMenuId( int menuid )
{
    TQString *key = keyDict.find( (long)menuid );
    changeInputMethod( (*key) );
}

#endif
