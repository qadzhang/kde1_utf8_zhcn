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

#include <ntqheader.h>
#include <ntqlineedit.h>
#include <ntqtimer.h>
#include "listboxrename.h"

class EditableListBoxItem : public TQListBoxItem
{
public:
    void setText( const TQString & text )
    {
	TQListBoxItem::setText( text );
    }
};

ListBoxRename::ListBoxRename( TQListBox * eventSource, const char * name )
    : TQObject( eventSource, name ),
      clickedItem( 0 ), activity( false )
{
    src = eventSource;
    src->installEventFilter( this );
    ed = new TQLineEdit( src->viewport() );
    ed->hide();
    ed->setFrame( false );

    TQObject::connect( ed, TQ_SIGNAL( returnPressed() ),
		      this, TQ_SLOT( renameClickedItem() ) );
}

bool ListBoxRename::eventFilter( TQObject *, TQEvent * event )
{
    switch ( event->type() ) {
	
    case TQEvent::MouseButtonPress:
        {
	    TQPoint pos = ((TQMouseEvent *) event)->pos();
	    
	    if ( clickedItem &&
		 clickedItem->isSelected() &&
		 (clickedItem == src->itemAt( pos )) ) {
		TQTimer::singleShot( 500, this, TQ_SLOT( showLineEdit() ) );
		activity = false; // no drags or clicks for 500 ms before we start the renaming
	    } else { // new item clicked
		activity = true;
		clickedItem = src->itemAt( pos );
		ed->hide();
	    }
	}
        break;
	
    case TQEvent::MouseMove:

	if ( ((TQMouseEvent *) event)->state() & TQt::LeftButton ) {
	    activity = true;  // drag
	}
	break;
	
    case TQEvent::KeyPress:
	
	switch ( ((TQKeyEvent *) event)->key() ) {

	case TQt::Key_F2:
	    
	    activity = false;
	    clickedItem = src->item( src->currentItem() );
	    showLineEdit();
	    break;

	case TQt::Key_Escape:
	    if ( !ed->isHidden() ) {
		hideLineEdit(); // abort rename
		return true;
	    }
	    break;

	case TQt::Key_Up:
	case TQt::Key_Down:
	case TQt::Key_PageUp:
	case TQt::Key_PageDown:
	    
	    if ( !ed->isHidden() )
		return true; // Filter out the keystrokes
	    break;

	}
	break;

    case TQEvent::Resize:

	if ( clickedItem && ed && !ed->isHidden() ) {
	    TQRect rect = src->itemRect( clickedItem );
	    ed->resize( rect.right() - rect.left() - 1,
		rect.bottom() - rect.top() - 1 );
	}
	break;

    default:
	break;
    }
    
    return false;
}

void ListBoxRename::showLineEdit()
{
    if ( !clickedItem || activity )
	return;
    TQRect rect = src->itemRect( clickedItem );
    ed->resize( rect.right() - rect.left() - 1,
		rect.bottom() - rect.top() - 1 );
    ed->move( rect.left() + 1, rect.top() + 1 );
    ed->setText( clickedItem->text() );
    ed->selectAll();
    ed->show();
    ed->setFocus();
}

void ListBoxRename::hideLineEdit()
{
    ed->hide();
    clickedItem = 0;
    src->setFocus();
}

void ListBoxRename::renameClickedItem()
{
    if ( clickedItem && ed ) {
	( (EditableListBoxItem *) clickedItem )->setText( ed->text() );
	emit itemTextChanged( ed->text() );
    }
    hideLineEdit();
}
