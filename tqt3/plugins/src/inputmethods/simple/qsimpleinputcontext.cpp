/****************************************************************************
** $Id$
**
** Implementation of TQSimpleInputContext class
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

#include "qsimpleinputcontext.h"

#include <ntqnamespace.h>
#include <ntqevent.h>
#include <ntqglobal.h>
#include <algorithm>

static const int ignoreKeys[] = {
    TQt::Key_Shift,
    TQt::Key_Control,
    TQt::Key_Meta,
    TQt::Key_Alt,
    TQt::Key_CapsLock,
    TQt::Key_Super_L,
    TQt::Key_Super_R,
    TQt::Key_Hyper_L,
    TQt::Key_Hyper_R,
    TQt::Key_Mode_switch
};

static const int composingKeys[] = {
    TQt::Key_Multi_key,
    TQt::Key_Dead_Grave,
    TQt::Key_Dead_Acute,
    TQt::Key_Dead_Circumflex,
    TQt::Key_Dead_Tilde,
    TQt::Key_Dead_Macron,
    TQt::Key_Dead_Breve,
    TQt::Key_Dead_Abovedot,
    TQt::Key_Dead_Diaeresis,
    TQt::Key_Dead_Abovering,
    TQt::Key_Dead_Doubleacute,
    TQt::Key_Dead_Caron,
    TQt::Key_Dead_Cedilla,
    TQt::Key_Dead_Ogonek,
    TQt::Key_Dead_Iota,
    TQt::Key_Dead_Voiced_Sound,
    TQt::Key_Dead_Semivoiced_Sound,
    TQt::Key_Dead_Belowdot,
    TQt::Key_Dead_Hook,
    TQt::Key_Dead_Horn
};

class Cmp
{
public:
    bool operator () (const TQComposeTableElement &lhs, const TQComposeTableElement &rhs) const {
        for ( size_t i=0; i < QT_KEYSEQUENCE_MAX_LEN; i++ ) {
            if ( lhs.keys[i] < rhs.keys[i] ) return true;
            else
            if ( lhs.keys[i] > rhs.keys[i] ) return false;
        }
        return false;
    }

    bool operator () (const TQComposeTableElement &lhs, const uint rhs[QT_KEYSEQUENCE_MAX_LEN]) const {
        for ( size_t i=0; i < QT_KEYSEQUENCE_MAX_LEN; i++ ) {
            if ( lhs.keys[i] < rhs[i] ) return true;
            else
            if ( lhs.keys[i] > rhs[i] ) return false;
        }
        return false;
    }
};

TQSimpleInputContext::TQSimpleInputContext()
    : TQInputContext()
{
    clearComposeBuffer();
}

TQSimpleInputContext::~TQSimpleInputContext()
{
    clearComposeBuffer();
}

bool TQSimpleInputContext::filterEvent( const TQEvent *event )
{
    if ( event->type() != TQEvent::KeyPress )
        return false;

    TQKeyEvent *keyevent = (TQKeyEvent *)event;
    int keyval = keyevent->key();
    int val = 0;

    if ( isIgnoreKeys( keyval ) )
        return false;

    if ( isComposingKeys( keyval ) ) {
        // If composing keys are pressed, use keyval directly
        val = UNITIZE( keyval );
    } else {
        TQString text = keyevent->text();
        if ( text.isEmpty() )
            return false;
        
        // If not composing keys are pressed, use the character's unicode value
        // NOTE : The contents of TQKeyEvent::text() is restricted to
        // only one character. See description of
        // TQInputContext::filterEvent() about key compression.
        val = text[0].unicode();
       // tqDebug( "str = %s", (const char*)keyevent->text().local8Bit() );
    }

    // Store value
    int nCompose = 0;
    while ( composeBuffer[nCompose] != 0 && nCompose < QT_KEYSEQUENCE_MAX_LEN )
        nCompose++;

    if ( nCompose == QT_KEYSEQUENCE_MAX_LEN ) {
        clearComposeBuffer();
        nCompose = 0;
    }

    composeBuffer[nCompose] = val;

    // check sequence
    if( checkComposeTable( composeBuffer, &defaultComposeTable ) )
        return true;

    return false;
}

void TQSimpleInputContext::setFocus()
{
    //tqDebug( "TQSimpleInputContext: %p->setFocus(), focusWidget()=%p",
            //this, focusWidget() );
}

void TQSimpleInputContext::unsetFocus()
{
    //tqDebug( "TQSimpleInputContext: %p->unsetFocus(), focusWidget()=%p",
            //this, focusWidget() );
    reset();
}

void TQSimpleInputContext::setMicroFocus( int x, int y, int w, int h, TQFont *f )
{
}

void TQSimpleInputContext::mouseHandler( int x, TQEvent::Type type,
				     TQt::ButtonState button,
				     TQt::ButtonState state )
{
    switch ( type ) {
    case TQEvent::MouseButtonPress:
    case TQEvent::MouseButtonRelease:
    case TQEvent::MouseButtonDblClick:
    case TQEvent::MouseMove:
       // tqDebug( "TQSimpleInputContext::mouseHandler: "
               // "x=%d, type=%d, button=%d, state=%d", x, type, button, state );
        break;
    default:
        break;
    }
}


void TQSimpleInputContext::reset()
{
    clearComposeBuffer();
    TQInputContext::reset();
}

TQString TQSimpleInputContext::identifierName()
{
    return "simple";
}

TQString TQSimpleInputContext::language()
{
    return "";  // FIXME
}

bool TQSimpleInputContext::isIgnoreKeys( int keyval )
{
    for ( uint i = 0; i < (sizeof(ignoreKeys)/sizeof(ignoreKeys[0])); i++ )
        if ( keyval == ignoreKeys[i] )
            return true;

    return false;
}

bool TQSimpleInputContext::isComposingKeys( int keyval )
{
    for ( uint i = 0; i < (sizeof(composingKeys)/sizeof(composingKeys[0])); i++ )
        if ( keyval == composingKeys[i] )
            return true;

    return false;
}

bool TQSimpleInputContext::checkComposeTable( uint* composeBuffer, const TQComposeTable *composeTable )
{
    const TQComposeTableElement *p = std::lower_bound( composeTable->data,
						      composeTable->data + composeTable->size,
						      composeBuffer,
						      Cmp() );

    // no entries were found
    if ( p == composeTable->data + composeTable->size ) {
       // tqDebug( "no match" );
        clearComposeBuffer();
        return false;
    }

    // check if compose buffer is matched
    for ( int i=0; i < QT_KEYSEQUENCE_MAX_LEN; i++ ) {

        // check if partial match
        if ( composeBuffer[i] == 0 && p->keys[i] ) {
            // tqDebug("partial match");
            return true;
        }

        if ( composeBuffer[i] != p->keys[i] ) {
          // tqDebug("different entry");
            clearComposeBuffer();
            return i!=0;
        }
    }

   // tqDebug("match exactly");

    // match exactly
    commitChar( p->value );
    clearComposeBuffer();

    return true;
}

void TQSimpleInputContext::commitChar( uint c )
{
    sendIMEvent( TQEvent::IMStart );
    sendIMEvent( TQEvent::IMEnd, TQString(TQChar(c)) );
}

void TQSimpleInputContext::clearComposeBuffer(void)
{
    for ( uint i=0; i < (sizeof(composeBuffer)/sizeof(int)); i++ )
        composeBuffer[i] = 0;
}

