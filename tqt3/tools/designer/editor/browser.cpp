/**********************************************************************
** Copyright (C) 2005-2008 Trolltech ASA.  All rights reserved.
**
** This file is part of TQt Designer.
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
** Licensees holding valid TQt Commercial licenses may use this file in
** accordance with the TQt Commercial License Agreement provided with
** the Software.
**
** This file is provided "AS IS" with NO WARRANTY OF ANY KIND,
** INCLUDING THE WARRANTIES OF DESIGN, MERCHANTABILITY AND FITNESS FOR
** A PARTICULAR PURPOSE. Trolltech reserves all rights not granted
** herein.
**
**********************************************************************/

#include "browser.h"
#include "editor.h"
#include <private/qrichtext_p.h>


EditorBrowser::EditorBrowser( Editor *e )
    : curEditor( e ), oldHighlightedParag( 0 )
{
    curEditor = e;

    // ### disabled for now
//     curEditor->viewport()->installEventFilter( this );
//     curEditor->installEventFilter( this );

    TQFont fn( curEditor->font() );
    fn.setUnderline( true );
    highlightedFormat = new TQTextFormat( fn, blue );
}

EditorBrowser::~EditorBrowser()
{
    delete highlightedFormat;
}

bool EditorBrowser::eventFilter( TQObject *o, TQEvent *e )
{
    if ( ::tqt_cast<Editor*>(o->parent()) || ::tqt_cast<Editor*>(o) ) {
	TQMouseEvent *me;
	TQKeyEvent *ke;
	switch ( e->type() ) {
	case TQEvent::MouseMove:
	    me = (TQMouseEvent*)e;
	    if ( ( me->state() & ControlButton ) == ControlButton ) {
		curEditor->viewport()->setCursor( pointingHandCursor );
		TQTextCursor c( curEditor->document() );
		curEditor->placeCursor( curEditor->viewportToContents( me->pos() ), &c );
		TQTextCursor from, to;
		if ( oldHighlightedParag ) {
		    oldHighlightedParag->setEndState( -1 );
		    oldHighlightedParag->format();
		    oldHighlightedParag = 0;
		}
		if ( findCursor( c, from, to ) && from.paragraph() == to.paragraph() ) {
		    // avoid collision with other selections
		    for ( int i = 0; i < curEditor->document()->numSelections(); ++i )
			curEditor->document()->removeSelection( i );
		    from.paragraph()->setFormat( from.index(), to.index() - from.index() + 1, highlightedFormat, false );
		    lastWord = from.paragraph()->string()->toString().mid( from.index(), to.index() - from.index() + 1 );
		    oldHighlightedParag = from.paragraph();
		} else {
		    lastWord = "";
		}
		curEditor->repaintChanged();
		return true;
	    }
	    break;
	case TQEvent::MouseButtonPress: {
	    bool killEvent = !lastWord.isEmpty();
	    if ( !lastWord.isEmpty() )
		showHelp( lastWord );
	    lastWord = "";
	    curEditor->viewport()->setCursor( ibeamCursor );
	    if ( oldHighlightedParag ) {
		oldHighlightedParag->setEndState( -1 );
		oldHighlightedParag->format();
		curEditor->repaintChanged();
		oldHighlightedParag = 0;
	    }
	    if ( killEvent )
		return true;
	} break;
	case TQEvent::KeyRelease:
	    lastWord = "";
	    ke = (TQKeyEvent*)e;
	    if ( ke->key() == Key_Control ) {
		curEditor->viewport()->setCursor( ibeamCursor );
		if ( oldHighlightedParag ) {
		    oldHighlightedParag->setEndState( -1 );
		    oldHighlightedParag->format();
		    curEditor->repaintChanged();
		    oldHighlightedParag = 0;
		}
	    }
	default:
	    break;
	}
    }
    return false;
}

void EditorBrowser::setCurrentEdior( Editor *e )
{
    curEditor = e;
    curEditor->installEventFilter( this );
}

void EditorBrowser::addEditor( Editor *e )
{
    e->installEventFilter( this );
}

bool EditorBrowser::findCursor( const TQTextCursor &c, TQTextCursor &from, TQTextCursor &to )
{
    from = c;
    while ( from.paragraph()->at( from.index() )->c != ' ' && from.paragraph()->at( from.index() )->c != '\t'  && from.index() > 0 )
	from.gotoLeft();
    if ( from.paragraph()->at( from.index() )->c == ' ' || from.paragraph()->at( from.index() )->c == '\t' )
	from.gotoRight();
    to = c;
    while ( to.paragraph()->at( to.index() )->c != ' ' && to.paragraph()->at( to.index() )->c != '\t' &&
	    to.index() < to.paragraph()->length() - 1 )
	to.gotoRight();
    if ( to.paragraph()->at( to.index() )->c == ' ' || to.paragraph()->at( to.index() )->c == '\t' )
	to.gotoLeft();
    return true;
}
