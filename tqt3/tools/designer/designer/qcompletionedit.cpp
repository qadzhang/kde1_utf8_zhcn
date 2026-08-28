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

#include "qcompletionedit.h"
#include <ntqlistbox.h>
#include <ntqsizegrip.h>
#include <ntqapplication.h>
#include <ntqvbox.h>

TQCompletionEdit::TQCompletionEdit( TQWidget *parent, const char *name )
    : TQLineEdit( parent, name ), aAdd( false ), caseSensitive( false )
{
    popup = new TQVBox( 0, 0, WType_Popup );
    popup->setFrameStyle( TQFrame::Box | TQFrame::Plain );
    popup->setLineWidth( 1 );
    popup->hide();

    listbox = new TQListBox( popup );
    listbox->setFrameStyle( TQFrame::NoFrame );
    listbox->setLineWidth( 1 );
    listbox->installEventFilter( this );
    listbox->setHScrollBarMode( TQScrollView::AlwaysOn );
    listbox->setVScrollBarMode( TQScrollView::AlwaysOn );
    listbox->setCornerWidget( new TQSizeGrip( listbox, "completion sizegrip" ) );
    connect( this, TQ_SIGNAL( textChanged( const TQString & ) ),
	     this, TQ_SLOT( textDidChange( const TQString & ) ) );
    popup->setFocusProxy( listbox );
    installEventFilter( this );
}

bool TQCompletionEdit::autoAdd() const
{
    return aAdd;
}

TQStringList TQCompletionEdit::completionList() const
{
    return compList;
}

void TQCompletionEdit::setCompletionList( const TQStringList &l )
{
    compList = l;
}

void TQCompletionEdit::setAutoAdd( bool add )
{
    aAdd = add;
}

void TQCompletionEdit::textDidChange( const TQString &text )
{
    if ( text.isEmpty() ) {
	popup->close();
	return;
    }
    updateListBox();
    placeListBox();
}

void TQCompletionEdit::placeListBox()
{
    if ( listbox->count() == 0 ) {
	popup->close();
	return;
    }

    popup->resize( TQMAX( listbox->sizeHint().width() + listbox->verticalScrollBar()->width() + 4, width() ),
		   listbox->sizeHint().height() + listbox->horizontalScrollBar()->height() + 4 );

    TQPoint p( mapToGlobal( TQPoint( 0, 0 ) ) );
    if ( p.y() + height() + popup->height() <= TQApplication::desktop()->height() )
	popup->move( p.x(), p.y() + height() );
    else
	popup->move( p.x(), p.y() - listbox->height() );
    popup->show();
    listbox->setCurrentItem( 0 );
    listbox->setSelected( 0, true );
    setFocus();
}

void TQCompletionEdit::updateListBox()
{
    listbox->clear();
    if ( compList.isEmpty() )
	return;
    for ( TQStringList::Iterator it = compList.begin(); it != compList.end(); ++it ) {
	if ( ( caseSensitive && (*it).left( text().length() ) == text() ) ||
	     ( !caseSensitive && (*it).left( text().length() ).lower() == text().lower() ) )
	    listbox->insertItem( *it );
    }
}

bool TQCompletionEdit::eventFilter( TQObject *o, TQEvent *e )
{
    if ( o == popup || o == listbox || o == listbox->viewport() ) {
	if ( e->type() == TQEvent::KeyPress ) {
	    TQKeyEvent *ke = (TQKeyEvent*)e;
	    if ( ke->key() == Key_Enter || ke->key() == Key_Return || ke->key() == Key_Tab ) {
		if ( ke->key() == Key_Tab && listbox->count() > 1 &&
		     listbox->currentItem() < (int)listbox->count() - 1 ) {
		    listbox->setCurrentItem( listbox->currentItem() + 1 );
		    return true;
		}
		popup->close();
		setFocus();
		blockSignals( true );
		setText( listbox->currentText() );
		blockSignals( false );
		emit chosen( text() );
		return true;
	    } else if ( ke->key() == Key_Left || ke->key() == Key_Right ||
			ke->key() == Key_Up || ke->key() == Key_Down ||
			ke->key() == Key_Home || ke->key() == Key_End ||
			ke->key() == Key_Prior || ke->key() == Key_Next ) {
		return false;
	    } else if ( ke->key() == Key_Escape ) {
		popup->close();
		setFocus();
	    } else if ( ke->key() != Key_Shift && ke->key() != Key_Control &&
			ke->key() != Key_Alt ) {
		updateListBox();
		if ( listbox->count() == 0 || text().length() == 0 ) {
		    popup->close();
		    setFocus();
		}
		TQApplication::sendEvent( this, e );
		return true;
	    }
	} else if ( e->type() == TQEvent::MouseButtonDblClick ) {
	    popup->close();
	    setFocus();
	    blockSignals( true );
	    setText( listbox->currentText() );
	    blockSignals( false );
	    emit chosen( text() );
	    return true;
	}
    } else if ( o == this ) {
	if ( e->type() == TQEvent::KeyPress ) {
	    TQKeyEvent *ke = (TQKeyEvent*)e;
	    if ( ke->key() == Key_Up ||
		 ke->key() == Key_Down ||
		 ke->key() == Key_Prior ||
		 ke->key() == Key_Next ||
		 ke->key() == Key_Return ||
		 ke->key() == Key_Enter ||
		 ke->key() == Key_Tab ||
		 ke->key() ==  Key_Escape ) {
		TQApplication::sendEvent( listbox, e );
		return true;
	    }
	}
    }
    return TQLineEdit::eventFilter( o, e );
}

void TQCompletionEdit::addCompletionEntry( const TQString &entry )
{
    if ( compList.find( entry ) == compList.end() ) {
	compList << entry;
	compList.sort();
    }
}

void TQCompletionEdit::removeCompletionEntry( const TQString &entry )
{
    TQStringList::Iterator it = compList.find( entry );
    if ( it != compList.end() )
	compList.remove( it );
}

void TQCompletionEdit::setCaseSensitive( bool b )
{
    caseSensitive = b;
}

bool TQCompletionEdit::isCaseSensitive() const
{
    return caseSensitive;
}

void TQCompletionEdit::clear()
{
    TQLineEdit::clear();
    compList.clear();
}
