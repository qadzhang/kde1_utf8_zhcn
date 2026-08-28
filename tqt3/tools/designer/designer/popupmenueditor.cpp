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

#include <ntqapplication.h>
#include <ntqcstring.h>
#include <ntqdatastream.h>
#include <ntqdragobject.h>
#include <ntqlineedit.h>
#include <ntqobjectlist.h>
#include <ntqpainter.h>
#include <ntqpopupmenu.h>
#include <ntqrect.h>
#include <ntqsize.h>
#include <ntqstyle.h>
#include <ntqtimer.h>
#include "actiondnd.h"
#include "actioneditorimpl.h"
#include "command.h"
#include "formfile.h"
#include "formwindow.h"
#include "mainwindow.h"
#include "metadatabase.h"
#include "pixmapchooser.h"
#include "popupmenueditor.h"
#include "menubareditor.h"

// Drag Object Declaration -------------------------------------------

class PopupMenuEditorItemPtrDrag : public TQStoredDrag
{
public:
    PopupMenuEditorItemPtrDrag( PopupMenuEditorItem * item, TQWidget * parent = 0,
				const char * name = 0 );
    ~PopupMenuEditorItemPtrDrag() {};
    static bool canDecode( TQDragMoveEvent * e );
    static bool decode( TQDropEvent * e, PopupMenuEditorItem ** i );
};

// Drag Object Implementation ---------------------------------------

PopupMenuEditorItemPtrDrag::PopupMenuEditorItemPtrDrag( PopupMenuEditorItem * item,
							TQWidget * parent, const char * name )
    : TQStoredDrag( "qt/popupmenueditoritemptr", parent, name )
{
    TQByteArray data( sizeof( TQ_LONG ) );
    TQDataStream stream( data, IO_WriteOnly );
    stream << ( TQ_LONG ) item;
    setEncodedData( data );
}

bool PopupMenuEditorItemPtrDrag::canDecode( TQDragMoveEvent * e )
{
    return e->provides( "qt/popupmenueditoritemptr" );
}

bool PopupMenuEditorItemPtrDrag::decode( TQDropEvent * e, PopupMenuEditorItem ** i )
{
    TQByteArray data = e->encodedData( "qt/popupmenueditoritemptr" );
    TQDataStream stream( data, IO_ReadOnly );

    if ( !data.size() )
	return false;

    TQ_LONG p = 0;
    stream >> p;
    *i = ( PopupMenuEditorItem *) p;

    return true;
}

// PopupMenuEditorItem Implementation -----------------------------------

PopupMenuEditorItem::PopupMenuEditorItem( PopupMenuEditor * menu, TQObject * parent, const char * name )
    : TQObject( parent, name ),
      a( 0 ),
      s( 0 ),
      m( menu ),
      separator( false ),
      removable( false )
{
    init();
    a = new TQAction( this );
    TQObject::connect( a, TQ_SIGNAL( destroyed() ), this, TQ_SLOT( selfDestruct() ) );
}


PopupMenuEditorItem::PopupMenuEditorItem( TQAction * action, PopupMenuEditor * menu,
					  TQObject * parent, const char * name )
    : TQObject( parent, name ),
      a( action ),
      s( 0 ),
      m( menu ),
      separator( false ),
      removable( true )
{
    init();
    if ( /*a->name() == "qt_separator_action" ||*/ ::tqt_cast<TQSeparatorAction*>(a) )
	separator = true;
    if ( a && a->children() )
 	a->installEventFilter( this );
}

PopupMenuEditorItem::PopupMenuEditorItem( PopupMenuEditorItem * item, PopupMenuEditor * menu,
					  TQObject * parent, const char * name )
    : TQObject( parent, name ),
      a( item->a ),
      s( 0 ),
      m( menu ),
      separator( item->separator ),
      removable( item->removable )
{
    init();
    if ( ::tqt_cast<TQActionGroup*>(a) )
 	a->installEventFilter( this );
}

PopupMenuEditorItem::~PopupMenuEditorItem()
{

}

void PopupMenuEditorItem::init()
{
    if ( a ) {
	TQObject::connect( a, TQ_SIGNAL( destroyed() ), this, TQ_SLOT( selfDestruct() ) );
	if ( m && !isSeparator() ) {
	    s = new PopupMenuEditor( m->formWindow(), m );
	    TQString n = "popupMenu";
	    m->formWindow()->unify( s, n, true );
	    s->setName( n );
	    MetaDataBase::addEntry( s );
	}
    }
}

PopupMenuEditorItem::ItemType PopupMenuEditorItem::type() const
{
    if ( separator )
	return Separator;
    else if ( a )
	return Action;
    return Unknown;
}

void PopupMenuEditorItem::setVisible( bool enable )
{
    if ( a )
	a->setVisible( enable );
}

bool PopupMenuEditorItem::isVisible() const
{
    TQActionGroup *g = ::tqt_cast<TQActionGroup*>(a);
    if ( g )
	return ( g->isVisible() && g->usesDropDown() );
    else if ( a )
	return a->isVisible();
    return false;
}

void PopupMenuEditorItem::showMenu( int x, int y )
{
    if ( ( !separator ) && s ) {
	s->move( x, y );
	s->show();
	s->raise();
    }
}

void PopupMenuEditorItem::hideMenu()
{
    if ( s ) {
	s->hideSubMenu();
	s->hide();
    }
}

void PopupMenuEditorItem::focusOnMenu()
{
    if ( s ) {
	s->showSubMenu();
	s->setFocus();
    }
}

int PopupMenuEditorItem::count() const
{
    if ( s ) {
	return s->count();
    } else if ( ::tqt_cast<TQActionGroup*>(a) ) {
	const TQObjectList * l = a->children();
	if ( l )
	    return l->count();
    }
    return 0;
}

bool PopupMenuEditorItem::eventFilter( TQObject * o, TQEvent * event )
{
    if ( ! ::tqt_cast<TQActionGroup*>( o ) )
	return false;
    if ( event->type() == TQEvent::ChildInserted ) {
	TQChildEvent * ce = ( TQChildEvent * ) event;
	TQObject * c = ce->child();
	TQAction * action = ::tqt_cast<TQAction*>( c );
	if ( s->find( action ) != -1 ) // avoid duplicates
	    return false;
	TQActionGroup * actionGroup = ::tqt_cast<TQActionGroup*>( c );
	if ( actionGroup )
	    s->insert( actionGroup );
 	else if ( action )
 	    s->insert( action );
    }
    return false;
}

void PopupMenuEditorItem::selfDestruct()
{
    hideMenu();
    int i = m->find( s );
    if ( i != -1 && i < m->count() )
	m->remove( i ); // remove this item
    a = 0; // the selfDestruct call was caused by the deletion of the action
    delete this;
}

// PopupMenuEditor Implementation -----------------------------------

PopupMenuEditorItem * PopupMenuEditor::draggedItem = 0;
int PopupMenuEditor::clipboardOperation = 0;
PopupMenuEditorItem * PopupMenuEditor::clipboardItem = 0;

PopupMenuEditor::PopupMenuEditor( FormWindow * fw, TQWidget * parent, const char * name )
    : TQWidget( 0, name, WStyle_Customize | WStyle_NoBorder | WRepaintNoErase | WResizeNoErase ),
      formWnd( fw ),
      parentMenu( parent ),
      iconWidth( 0 ),
      textWidth( 0 ),
      accelWidth( 0 ),
      arrowWidth( 30 ),
      borderSize( 2 ),
      currentField( 1 ),
      currentIndex( 0 )
{
    init();
}

PopupMenuEditor::PopupMenuEditor( FormWindow * fw, PopupMenuEditor * menu,
				  TQWidget * parent, const char * name )
    : TQWidget( 0, name, WStyle_Customize | WStyle_NoBorder | WRepaintNoErase ),
      formWnd( fw ),
      parentMenu( parent ),
      iconWidth( menu->iconWidth ),
      textWidth( menu->textWidth ),
      accelWidth( menu->accelWidth ),
      arrowWidth( menu->arrowWidth ),
      borderSize( menu->borderSize ),
      currentField( menu->currentField ),
      currentIndex( menu->currentIndex )
{
    init();
    PopupMenuEditorItem * i;
    for ( i = menu->itemList.first(); i; i = menu->itemList.next() ) {
	PopupMenuEditorItem * n = new PopupMenuEditorItem( i, this );
	itemList.append( n );
    }
}

PopupMenuEditor::~PopupMenuEditor()
{
    itemList.setAutoDelete( true );
}

void PopupMenuEditor::init()
{
    reparent( ( TQMainWindow * ) formWnd->mainContainer(), pos() );

    addItem.action()->setMenuText( tr("new item") );
    addSeparator.action()->setMenuText( tr("new separator") );

    setAcceptDrops( true );
    setFocusPolicy( StrongFocus );

    lineEdit = new TQLineEdit( this );
    lineEdit->hide();
    lineEdit->setFrameStyle(TQFrame::Plain | TQFrame::NoFrame);
    lineEdit->polish();
    lineEdit->setBackgroundOrigin(ParentOrigin);
    lineEdit->setBackgroundMode(PaletteButton);
    lineEdit->installEventFilter( this );

    dropLine = new TQWidget( this, 0, TQt::WStyle_NoBorder | WStyle_StaysOnTop );
    dropLine->setBackgroundColor( TQt::red );
    dropLine->hide();

    hide();
}

void PopupMenuEditor::insert( PopupMenuEditorItem * item, int index )
{
    if ( !item )
	return;
    if ( index == -1 ) {
	itemList.append( item );
	if ( isVisible() )
	    currentIndex = itemList.count() - 1;
    } else {
	itemList.insert( index, item );
	if ( isVisible() )
	    currentIndex = index;
    }
    item->m = this;
    if (item->s)
        item->s->parentMenu = this;
    resizeToContents();
    if ( isVisible() && parentMenu )
	parentMenu->update(); // draw arrow in parent menu
    emit inserted( item->action() );
}

void PopupMenuEditor::insert( TQAction * action, int index )
{
    if ( !action )
	return;
    insert( new PopupMenuEditorItem( action, this, 0, action->name() ), index );
}

void PopupMenuEditor::insert( TQActionGroup * actionGroup, int index )
{
    if ( !actionGroup )
	return;
    bool dropdown = actionGroup->usesDropDown();
    PopupMenuEditorItem *i = new PopupMenuEditorItem( (TQAction *)actionGroup, this, 0,
						      TQString( actionGroup->name() ) + "Menu" );
    TQActionGroup *g = 0;
    TQObjectList *l = actionGroup->queryList( "TQAction", 0, false, false );
    TQObjectListIterator it( *l );
    insert( i, index );
    for ( ; it.current(); ++it ) {
	g = ::tqt_cast<TQActionGroup*>(it.current());
	if ( g ) {
	    if ( dropdown )
		i->s->insert( g );
	    else
		insert( g );
	} else {
	    i->s->insert( (TQAction*)it.current() );
	}
    }
    delete l;
}

int PopupMenuEditor::find( const TQAction * action )
{
    PopupMenuEditorItem * i = itemList.first();
    while ( i ) {
	if ( i->action() == action )
	    return itemList.at();
	i = itemList.next();
    }
    return -1;
}

int PopupMenuEditor::find( PopupMenuEditor * menu )
{
    PopupMenuEditorItem * i = itemList.first();
    while ( i ) {
	if ( i->subMenu() == menu )
	    return itemList.at();
	i = itemList.next();
    }
    return -1;
}

int PopupMenuEditor::count()
{
    return itemList.count();
}

PopupMenuEditorItem * PopupMenuEditor::at( int index )
{
    return itemList.at( index );
}

void PopupMenuEditor::exchange( int a, int b )
{
    PopupMenuEditorItem * ia = itemList.at( a );
    PopupMenuEditorItem * ib = itemList.at( b );
    if ( !ia || !ib ||
	 ia == &addItem || ia == &addSeparator ||
	 ib == &addItem || ib == &addSeparator )
	return; // do nothing
    itemList.replace( b, ia );
    itemList.replace( a, ib );
}

void PopupMenuEditor::cut( int index )
{
    int idx = ( index == -1 ? currentIndex : index );

    clipboardOperation = Cut;
    clipboardItem = itemList.at( idx );

    if ( clipboardItem == &addItem || clipboardItem == &addSeparator ) {
	clipboardOperation = None;
	clipboardItem = 0;
	return; // do nothing
    }

    RemoveActionFromPopupCommand * cmd =
	new RemoveActionFromPopupCommand( "Cut Item", formWnd, this, idx );
    formWnd->commandHistory()->addCommand( cmd );
    cmd->execute();
}

void PopupMenuEditor::copy( int index )
{
    int idx = ( index == -1 ? currentIndex : index );

    clipboardOperation = Copy;
    clipboardItem = itemList.at( idx );

    if ( clipboardItem == &addItem || clipboardItem == &addSeparator ) {
	clipboardOperation = None;
	clipboardItem = 0;
    }
}

void PopupMenuEditor::paste( int index )
{
    int idx = ( index == -1 ? currentIndex : index );

    if ( clipboardItem && clipboardOperation ) {
	PopupMenuEditorItem * n = new PopupMenuEditorItem( clipboardItem, this );
	AddActionToPopupCommand * cmd =
	    new AddActionToPopupCommand( "Paste Item", formWnd, this, n, idx );
	formWnd->commandHistory()->addCommand( cmd );
	cmd->execute();
    }
}

void PopupMenuEditor::insertedActions( TQPtrList<TQAction> & list )
{
    TQAction * a = 0;
    PopupMenuEditorItem * i = itemList.first();

    while ( i ) {
	a = i->action();
	if ( a )
	    list.append( a );
	i = itemList.next();
    }
}

void PopupMenuEditor::show()
{
    resizeToContents();
    TQWidget::show();
}

void PopupMenuEditor::choosePixmap( int index )
{
    int idx = ( index == -1 ? currentIndex : index );

    PopupMenuEditorItem * i = 0;
    TQAction * a = 0;

    if ( idx < (int)itemList.count() ) {
	i = itemList.at( idx );
	a = i->action();
    } else {
	createItem();
    }

    hide(); // qChoosePixmap hides the menu
    TQIconSet icons( qChoosePixmap( 0, formWnd, 0, 0 ) );
    SetActionIconsCommand * cmd =
	new SetActionIconsCommand( "Set icon", formWnd, a, this, icons );
    formWnd->commandHistory()->addCommand( cmd );
    cmd->execute();
    show();
    setFocus();
}

void PopupMenuEditor::showLineEdit( int index )
{
    int idx = ( index == -1 ? currentIndex : index );

    PopupMenuEditorItem * i = 0;

    if ( idx >= (int)itemList.count() )
	i = &addItem;
    else
	i = itemList.at( idx );

    // open edit currentField for item name
    lineEdit->setText( i->action()->menuText() );
    lineEdit->selectAll();
    lineEdit->setGeometry( borderSize + iconWidth, borderSize + itemPos( i ),
			   textWidth, itemHeight( i ) );
    lineEdit->show();
    lineEdit->setFocus();
}

void PopupMenuEditor::setAccelerator( int key, TQt::ButtonState state, int index )
{
    // FIXME: make this a command

    int idx = ( index == -1 ? currentIndex : index );

    if ( key == TQt::Key_Shift ||
	 key == TQt::Key_Control ||
	 key == TQt::Key_Alt ||
	 key == TQt::Key_Meta ||
	 key == TQt::Key_unknown )
	return; // ignore these keys when they are pressed

    PopupMenuEditorItem * i = 0;

    if ( idx >= (int)itemList.count() )
	i = createItem();
    else
	i = itemList.at( idx );

    int shift = ( state & TQt::ShiftButton ? TQt::SHIFT : 0 );
    int ctrl = ( state & TQt::ControlButton ? TQt::CTRL : 0 );
    int alt = ( state & TQt::AltButton ? TQt::ALT : 0 );
    int meta = ( state & TQt::MetaButton ? TQt::META : 0 );

    TQAction * a = i->action();
    TQKeySequence ks = a->accel();
    int keys[4] = { ks[0], ks[1], ks[2], ks[3] };
    int n = 0;
    while ( n < 4 && ks[n++] );
    n--;
    if ( n < 4 )
	keys[n] = key | shift | ctrl | alt | meta;
    a->setAccel( TQKeySequence( keys[0], keys[1], keys[2], keys[3] ) );
    MetaDataBase::setPropertyChanged( a, "accel", true );
    resizeToContents();
}

void PopupMenuEditor::resizeToContents()
{
    TQSize s = contentsSize();
    dropLine->resize( s.width(), 2 );
    s.rwidth() += borderSize * 2;
    s.rheight() += borderSize * 2;
    resize( s );
}

void PopupMenuEditor::showSubMenu()
{
    if ( currentIndex < (int)itemList.count() ) {
	itemList.at( currentIndex )->showMenu( pos().x() + width() - borderSize * 3,
					       pos().y() + itemPos( at( currentIndex ) ) +
					       borderSize * 2 );
	setFocus(); // Keep focus in this widget
    }
}

void PopupMenuEditor::hideSubMenu()
{
    if ( currentIndex < (int)itemList.count() )
	itemList.at( currentIndex )->hideMenu();
}

void PopupMenuEditor::focusOnSubMenu()
{
    if ( currentIndex < (int)itemList.count() )
	itemList.at( currentIndex )->focusOnMenu();
}

// This function has no undo. It is only here to remove an item when its action was
// removed from the action editor.
// Use removeItem to put the command on the command stack.
void PopupMenuEditor::remove( int index )
{
    int idx = ( index == -1 ? currentIndex : index );
    PopupMenuEditorItem * i = itemList.at( idx );
    if ( i && i->isRemovable() ) {
	itemList.remove( idx );
	int n = itemList.count() + 1;
	if ( currentIndex >= n )
	    currentIndex = itemList.count() + 1;
	emit removed( i->action() );
	resizeToContents();
    }
}

PopupMenuEditorItem * PopupMenuEditor::createItem( TQAction * a )
{
    ActionEditor * ae = (ActionEditor *) formWindow()->mainWindow()->child( 0, "ActionEditor" );
    if ( !a )
	a = ae->newActionEx();
    PopupMenuEditorItem * i = new PopupMenuEditorItem( a, this );
    TQString n = TQString( a->name() ) + "Item";
    formWindow()->unify( i, n, false );
    i->setName( n );
    AddActionToPopupCommand * cmd =
	new AddActionToPopupCommand( "Add Item", formWnd, this, i );
    formWnd->commandHistory()->addCommand( cmd );
    cmd->execute();
    return i;
}

void PopupMenuEditor::removeItem( int index )
{
    int idx = ( index == -1 ? currentIndex : index );
    if ( idx < (int)itemList.count() ) {
	RemoveActionFromPopupCommand * cmd = new RemoveActionFromPopupCommand( "Remove Item",
									       formWnd,
									       this,
									       idx );
	formWnd->commandHistory()->addCommand( cmd );
	cmd->execute();
	if ( itemList.count() == 0 && parentMenu )
	    parentMenu->update();
	resizeToContents();
    }
}

PopupMenuEditorItem * PopupMenuEditor::currentItem()
{
    int count = itemList.count();
    if ( currentIndex < count )
	return itemList.at( currentIndex );
    else if ( currentIndex == count )
	return &addItem;
    return &addSeparator;
}

PopupMenuEditorItem * PopupMenuEditor::itemAt( int y )
{
    PopupMenuEditorItem * i = itemList.first();
    int iy = 0;

    while ( i ) {
	iy += itemHeight( i );
	if ( iy > y )
	    return i;
	i = itemList.next();
    }
    iy += itemHeight( &addItem );
    if ( iy > y )
	return &addItem;
    return &addSeparator;
}

void PopupMenuEditor::setFocusAt( const TQPoint & pos )
{
    hideSubMenu();
    lineEdit->hide();

    currentIndex = 0;
    int iy = 0;
    PopupMenuEditorItem * i = itemList.first();

    while ( i ) {
	iy += itemHeight( i );
	if ( iy > pos.y() )
	    break;
	i = itemList.next();
	currentIndex++;
    }

    iy += itemHeight( &addItem );
    if ( iy <= pos.y() )
	currentIndex++;

    if ( currentIndex < (int)itemList.count() ) {
	if ( pos.x() < iconWidth )
	    currentField = 0;
	else if ( pos.x() < iconWidth + textWidth )
	    currentField = 1;
	else
	    currentField = 2;
    } else {
	currentField = 1;
    }

    showSubMenu();
}

bool PopupMenuEditor::eventFilter( TQObject * o, TQEvent * e )
{
    if ( o == lineEdit  && e->type() == TQEvent::FocusOut ) {
	leaveEditMode( 0 );
	update();
    }
    return TQWidget::eventFilter( o, e );
}

void PopupMenuEditor::paintEvent( TQPaintEvent * )
{
    TQPainter p( this );
    p.save();
    TQRegion reg( rect() );
    TQRegion mid( borderSize, borderSize,
		 rect().width() - borderSize * 2, rect().height() - borderSize * 2 );
    reg -= mid;
    p.setClipRegion( reg );
    style().drawPrimitive( TQStyle::PE_PanelPopup, &p, rect(), colorGroup() );
    p.restore();
    drawItems( &p );
}

void PopupMenuEditor::mousePressEvent( TQMouseEvent * e )
{
    mousePressPos = e->pos();
    setFocusAt( mousePressPos );
    e->accept();
    update();
}

void PopupMenuEditor::mouseDoubleClickEvent( TQMouseEvent * )
{
    setFocusAt( mousePressPos );
    if ( currentItem() == &addSeparator ) {
	PopupMenuEditorItem * i = createItem( new TQSeparatorAction( 0 ) );
	i->setSeparator( true );
	return;
    }
    if ( currentField == 0 ) {
	choosePixmap();
	resizeToContents();
    } else if ( currentField == 1 ) {
	showLineEdit();
    }
}

void PopupMenuEditor::mouseMoveEvent( TQMouseEvent * e )
{
    if ( e->state() & TQt::LeftButton ) {
	if ( ( e->pos() - mousePressPos ).manhattanLength() > 3 ) {
	    draggedItem = itemAt( mousePressPos.y() );
	    if ( draggedItem == &addItem ) {
		draggedItem = createItem();
		RenameActionCommand cmd( "Rename Item", formWnd, draggedItem->action(),
					 this, "Unnamed" );
		cmd.execute();
                // FIXME: start rename after drop
	    } else if ( draggedItem == &addSeparator ) {
		draggedItem = createItem( new TQSeparatorAction( 0 ) );
		draggedItem->setSeparator( true );
	    }

	    PopupMenuEditorItemPtrDrag * d =
		new PopupMenuEditorItemPtrDrag( draggedItem, this );

	    hideSubMenu();

	    draggedItem->setVisible( false );
	    resizeToContents();

	    // If the item is dropped in the same list,
	    //  we will have two instances of the same pointer
	    // in the list. We use node instead.
	    int idx = itemList.find( draggedItem );
	    TQLNode * node = itemList.currentNode();

	    d->dragCopy(); // dragevents and stuff happens

	    if ( draggedItem ) { // item was not dropped
		draggedItem->setVisible( true );
		draggedItem = 0;
		if ( hasFocus() ) {
		    hideSubMenu();
		    resizeToContents();
		    showSubMenu();
		}
	    } else { // item was dropped
		itemList.takeNode( node )->setVisible( true );
		if ( currentIndex > 0 && currentIndex > idx )
		    --currentIndex;
		// the drop might happen in another menu, so we'll resize
		// and show the submenu there
	    }
	}
    }
}

void PopupMenuEditor::dragEnterEvent( TQDragEnterEvent * e )
{
    if ( e->provides( "qt/popupmenueditoritemptr" ) ||
	 e->provides( "application/x-designer-actions" ) ||
	 e->provides( "application/x-designer-actiongroup" ) ) {
	e->accept();
	dropLine->show();
    }
}

void PopupMenuEditor::dragLeaveEvent( TQDragLeaveEvent * )
{
    dropLine->hide();
}

void PopupMenuEditor::dragMoveEvent( TQDragMoveEvent * e )
{
    TQPoint pos = e->pos();
    dropLine->move( borderSize, snapToItem( pos.y() ) );

    if ( currentItem() != itemAt( pos.y() ) ) {
	hideSubMenu();
	setFocusAt( pos );
	showSubMenu();
    }
}

void PopupMenuEditor::dropEvent( TQDropEvent * e )
{
    if ( !( e->provides( "qt/popupmenueditoritemptr" ) ||
	    e->provides( "application/x-designer-actions" ) ||
	    e->provides( "application/x-designer-actiongroup" ) ) )
	return;

    // Hide the sub menu of the current item, but do it later
    if ( currentIndex < (int)itemList.count() ) {
	PopupMenuEditor *s = itemList.at( currentIndex )->s;
	TQTimer::singleShot( 0, s, TQ_SLOT( hide() ) );
    }

    draggedItem = 0;
    PopupMenuEditorItem * i = 0;

    if ( e->provides( "qt/popupmenueditoritemptr" ) ) {
	PopupMenuEditorItemPtrDrag::decode( e, &i );
    } else {
	if ( e->provides( "application/x-designer-actiongroup" ) ) {
	    TQActionGroup * g = ::tqt_cast<TQDesignerActionGroup*>(ActionDrag::action());
	    if ( g->usesDropDown() ) {
		i = new PopupMenuEditorItem( g, this );
		TQString n = TQString( g->name() ) + "Item";
		formWindow()->unify( i, n, false );
		i->setName( n );
		TQObjectList *l = g->queryList( "TQAction", 0, false, false );
		TQObjectListIterator it( *l );
		for ( ; it.current(); ++it ) {
		    g = ::tqt_cast<TQActionGroup*>(it.current());
		    if ( g )
			i->s->insert( g );
		    else
			i->s->insert( (TQAction*)it.current() );
		}
		delete l;
	    } else {
		dropInPlace( g, e->pos().y() );
	    }
	} else if ( e->provides( "application/x-designer-actions" ) ) {
	    TQAction *a = ::tqt_cast<TQDesignerAction*>(ActionDrag::action());
	    i = new PopupMenuEditorItem( a, this );
	}
    }

    if ( i ) {
	dropInPlace( i, e->pos().y() );
	TQTimer::singleShot( 0, this, TQ_SLOT( resizeToContents() ) );
    }

    TQTimer::singleShot( 0, this, TQ_SLOT( showSubMenu() ) );
    TQTimer::singleShot( 0, this, TQ_SLOT( setFocus() ) );
    dropLine->hide();
    e->accept();
}

void PopupMenuEditor::keyPressEvent( TQKeyEvent * e )
{
    if ( lineEdit->isHidden() ) { // In navigation mode

	switch ( e->key() ) {

	case TQt::Key_Delete:
	    hideSubMenu();
	    removeItem();
	    showSubMenu();
	    break;

	case TQt::Key_Backspace:
	    clearCurrentField();
	    break;

	case TQt::Key_Up:
	    navigateUp( e->state() & TQt::ControlButton );
	    break;

	case TQt::Key_Down:
	    navigateDown( e->state() & TQt::ControlButton );
	    break;

	case TQt::Key_Left:
	    navigateLeft();
	    break;

	case TQt::Key_Right:
	    navigateRight();
	    break;

	case TQt::Key_PageUp:
	    currentIndex = 0;
	    break;

	case TQt::Key_PageDown:
	    currentIndex = itemList.count();
	    break;

	case TQt::Key_Enter:
	case TQt::Key_Return:
	case TQt::Key_F2:
	    enterEditMode( e );
	    // move on
	case TQt::Key_Alt:
	case TQt::Key_Shift:
	case TQt::Key_Control:
	    // do nothing
	    return;

	case TQt::Key_Escape:
	    currentField = 0;
	    navigateLeft();
	    break;

	case TQt::Key_C:
	    if ( e->state() & TQt::ControlButton &&
		 currentIndex < (int)itemList.count() ) {
		copy( currentIndex );
		break;
	    }

	case TQt::Key_X:
	    if ( e->state() & TQt::ControlButton &&
		 currentIndex < (int)itemList.count() ) {
		hideSubMenu();
		cut( currentIndex );
		showSubMenu();
		break;
	    }

	case TQt::Key_V:
	    if ( e->state() & TQt::ControlButton ) {
		hideSubMenu();
		paste( currentIndex < (int)itemList.count() ? currentIndex + 1: itemList.count() );
		showSubMenu();
		break;
	    }

	default:
	    if (  currentItem()->isSeparator() )
		return;
	    if ( currentField == 1 ) {
		showLineEdit();
		TQApplication::sendEvent( lineEdit, e );
		e->accept();
		return;
	    } else if ( currentField == 2 ) {
		setAccelerator( e->key(), e->state() );
		showSubMenu();
	    }
	    break;

	}

    } else { // In edit mode
	switch ( e->key() ) {
	case TQt::Key_Enter:
	case TQt::Key_Return:
	case TQt::Key_Escape:
	    leaveEditMode( e );
	    e->accept();
	    return;
	}
    }
    update();
}

void PopupMenuEditor::focusInEvent( TQFocusEvent * )
{
    showSubMenu();
    update();
    parentMenu->update();
}

void PopupMenuEditor::focusOutEvent( TQFocusEvent * )
{
    TQWidget * fw = tqApp->focusWidget();
    if ( !fw || ( !::tqt_cast<PopupMenuEditor*>(fw) && fw != lineEdit ) ) {
	hideSubMenu();
	if ( fw && ::tqt_cast<MenuBarEditor*>(fw) )
	    return;
	TQWidget * w = this;
	while ( w && w != fw && ::tqt_cast<PopupMenuEditor*>(w) ) { // hide all popups
	    w->hide();
	    w = ((PopupMenuEditor *)w)->parentEditor();
	}
    }
}

void PopupMenuEditor::drawItem( TQPainter * p, PopupMenuEditorItem * i,
				const TQRect & r, int f ) const
{
    int x = r.x();
    int y = r.y();
    int h = r.height();

    p->fillRect( r, colorGroup().brush( TQColorGroup::Background ) );

    if ( i->isSeparator() ) {
	style().drawPrimitive( TQStyle::PE_Separator, p,
			       TQRect( r.x(), r.y() + 2, r.width(), 1 ),
			       colorGroup(), TQStyle::Style_Sunken | f );
	return;
    }

    const TQAction * a = i->action();
    if ( a->isToggleAction() && a->isOn() ) {
	style().drawPrimitive( TQStyle::PE_CheckMark, p,
			       TQRect( x , y, iconWidth, h ),
			       colorGroup(), f );
    } else {
	TQPixmap icon = a->iconSet().pixmap( TQIconSet::Automatic, TQIconSet::Normal );
	p->drawPixmap( x + ( iconWidth - icon.width() ) / 2,
		       y + ( h - icon.height() ) / 2,
		       icon );
    }
    x += iconWidth;
    p->drawText( x, y, textWidth, h,
		 TQPainter::AlignLeft |
		 TQPainter::AlignVCenter |
		 TQt::ShowPrefix |
		 TQt::SingleLine,
		 a->menuText() );

    x += textWidth + borderSize * 3;
    p->drawText( x, y, accelWidth, h,
		 TQPainter::AlignLeft | TQPainter::AlignVCenter,
		 a->accel() );
    if ( i->count() ) // Item has submenu
	style().drawPrimitive( TQStyle::PE_ArrowRight, p,
			       TQRect( r.width() - arrowWidth, r.y(), arrowWidth, r.height() ),
			       colorGroup(), f );
}

void PopupMenuEditor::drawWinFocusRect( TQPainter * p, const TQRect & r ) const
{
    if ( currentIndex < (int)itemList.count() &&
	 ((PopupMenuEditor*)this)->itemList.at( currentIndex )->isSeparator() ) {
	p->drawWinFocusRect( borderSize, r.y(), width() - borderSize * 2, r.height() );
	return;
    }
    int y = r.y();
    int h = r.height();
    if ( currentField == 0 )
	p->drawWinFocusRect( borderSize + 1, y, iconWidth - 2, h );
    else if ( currentField == 1 )
	p->drawWinFocusRect( borderSize + iconWidth, y, textWidth, h );
    else if ( currentField == 2 )
	p->drawWinFocusRect( borderSize + iconWidth + textWidth +
			     borderSize * 3, y, accelWidth, h );
}

void PopupMenuEditor::drawItems( TQPainter * p )
{
    int flags = 0;
    int idx = 0;

    TQColorGroup enabled = colorGroup();
    TQColorGroup disabled = palette().disabled();
    TQRect focus;
    TQRect rect( borderSize, borderSize, width() - borderSize * 2, 0 );

    PopupMenuEditorItem * i = itemList.first();
    while ( i ) {
	if ( i->isVisible() ) {
	    rect.setHeight( itemHeight( i ) );
	    if ( idx == currentIndex )
		focus = rect;
	    if ( i->action()->isEnabled() ) {
		flags = TQStyle::Style_Enabled;
		p->setPen( enabled.buttonText() );
	    } else {
		flags = TQStyle::Style_Default;
		p->setPen( disabled.buttonText() );
	    }
	    drawItem( p, i, rect, flags );
	    rect.moveBy( 0, rect.height() );
	}
	i = itemList.next();
	idx++;
    }

    // Draw the "add item" and "add separator" items
    p->setPen( darkBlue );
    rect.setHeight( itemHeight( &addItem ) );
    if ( idx == currentIndex )
	focus = rect;
    drawItem( p, &addItem, rect, TQStyle::Style_Default );
    rect.moveBy( 0, rect.height() );
    idx++;
    rect.setHeight( itemHeight( &addSeparator ) );
    if ( idx == currentIndex )
	focus = rect;
    drawItem( p, &addSeparator, rect, TQStyle::Style_Default );
    idx++;

    if ( hasFocus() && !draggedItem )
	drawWinFocusRect( p, focus );
}

TQSize PopupMenuEditor::contentsSize()
{
    TQRect textRect = fontMetrics().boundingRect( addSeparator.action()->menuText() );
    textWidth = textRect.width();
    accelWidth = textRect.height(); // default size
    iconWidth = textRect.height();

    int w = 0;
    int h = itemHeight( &addItem ) + itemHeight( &addSeparator );
    PopupMenuEditorItem * i = itemList.first();
    TQAction * a = 0;
    while ( i ) {
	if ( i->isVisible() ) {
	    if ( !i->isSeparator() ) {
		a = i->action();
		w = a->iconSet().pixmap( TQIconSet::Automatic, TQIconSet::Normal ).rect().width() +
		    borderSize; // padding
		iconWidth = TQMAX( iconWidth, w );
		w = fontMetrics().boundingRect( a->menuText() ).width();
		textWidth = TQMAX( textWidth, w );
		w = fontMetrics().boundingRect( a->accel() ).width() + 2; // added padding?
		accelWidth = TQMAX( accelWidth, w );
	    }
	    h += itemHeight( i );
	}
	i = itemList.next();
    }

    int width = iconWidth + textWidth + borderSize * 3 + accelWidth + arrowWidth;
    return TQSize( width, h );
}

int PopupMenuEditor::itemHeight( const PopupMenuEditorItem * item ) const
{
    if ( !item || ( item && !item->isVisible() ) )
	return 0;
    if ( item->isSeparator() )
	return 4; // FIXME: hardcoded ( get from styles )r
    int padding =  + borderSize * 6;
    TQAction * a = item->action();
    int h = a->iconSet().pixmap( TQIconSet::Automatic, TQIconSet::Normal ).rect().height();
    h = TQMAX( h, fontMetrics().boundingRect( a->menuText() ).height() + padding );
    h = TQMAX( h, fontMetrics().boundingRect( a->accel() ).height() + padding );
    return h;
}

int PopupMenuEditor::itemPos( const PopupMenuEditorItem * item ) const
{
    PopupMenuEditor * that = ( PopupMenuEditor * ) this;
    int y = 0;
    PopupMenuEditorItem * i = that->itemList.first();
    while ( i ) {
	if ( i == item )
	    return y;
	y += itemHeight( i );
	i = that->itemList.next();
    }
    return y;
}

int PopupMenuEditor::snapToItem( int y )
{
    int iy = 0;
    int dy = 0;

    PopupMenuEditorItem * i = itemList.first();

    while ( i ) {
	    dy = itemHeight( i );
	    if ( iy + dy / 2 > y )
		return iy;
	    iy += dy;
	i = itemList.next();
    }

    return iy;
}

void PopupMenuEditor::dropInPlace( PopupMenuEditorItem * i, int y )
{
    int iy = 0;
    int dy = 0;
    int idx = 0;

    PopupMenuEditorItem * n = itemList.first();

    while ( n ) {
	dy = itemHeight( n );
	if ( iy + dy / 2 > y )
	    break;
	iy += dy;
	idx++;
	n = itemList.next();
    }
    int same = itemList.findRef( i );
    AddActionToPopupCommand * cmd = new AddActionToPopupCommand( "Drop Item", formWnd, this, i, idx );
    formWnd->commandHistory()->addCommand( cmd );
    cmd->execute();
    currentIndex = ( same >= 0 && same < idx ) ? idx - 1 : idx;
    currentField = 1;
}

void PopupMenuEditor::dropInPlace( TQActionGroup * g, int y )
{
    if (!g->children())
	return;
    TQObjectList l = *g->children();
    for ( int i = 0; i < (int)l.count(); ++i ) {
	TQAction *a = ::tqt_cast<TQAction*>(l.at(i));
	TQActionGroup *g = ::tqt_cast<TQActionGroup*>(l.at(i));
	if ( g )
	    dropInPlace( g, y );
	else if ( a )
	    dropInPlace( new PopupMenuEditorItem( a, this ), y );
    }
}

void PopupMenuEditor::safeDec()
{
    do  {
	currentIndex--;
    } while ( currentIndex > 0 && !currentItem()->isVisible() );
    if ( currentIndex == 0 &&
	 !currentItem()->isVisible() &&
	 parentMenu ) {
	parentMenu->setFocus();
    }
}

void PopupMenuEditor::safeInc()
{
    int max = itemList.count() + 1;
    if ( currentIndex < max ) {
	do  {
	    currentIndex++;
	} while ( currentIndex < max && !currentItem()->isVisible() ); // skip invisible items
    }
}

void PopupMenuEditor::clearCurrentField()
{
    if ( currentIndex >= (int)itemList.count() )
	return; // currentIndex is addItem or addSeparator
    PopupMenuEditorItem * i = currentItem();
    hideSubMenu();
    if ( i->isSeparator() )
	return;
    if ( currentField == 0 ) {
	TQIconSet icons( 0 );
	SetActionIconsCommand * cmd = new SetActionIconsCommand( "Remove icon",
								 formWnd,
								 i->action(),
								 this,
								 icons );
	formWnd->commandHistory()->addCommand( cmd );
	cmd->execute();
    } else if ( currentField == 2 ) {
	i->action()->setAccel( 0 );
    }
    resizeToContents();
    showSubMenu();
    return;
}

void PopupMenuEditor::navigateUp( bool ctrl )
{
    if ( currentIndex > 0 ) {
	hideSubMenu();
	if ( ctrl ) {
	    ExchangeActionInPopupCommand * cmd =
		new ExchangeActionInPopupCommand( "Move Item Up",
						  formWnd,
						  this,
						  currentIndex,
						  currentIndex - 1 );
	    formWnd->commandHistory()->addCommand( cmd );
	    cmd->execute();
	    safeDec();
	} else {
	    safeDec();
	}
	showSubMenu();
    } else if ( parentMenu ) {
	parentMenu->setFocus();
	parentMenu->update();
    }
}

void PopupMenuEditor::navigateDown( bool ctrl )
{
    hideSubMenu();
    if ( ctrl ) {
	if ( currentIndex < ( (int)itemList.count() - 1 ) ) { // safe index
	    ExchangeActionInPopupCommand * cmd =
		new ExchangeActionInPopupCommand( "Move Item Down",
						  formWnd,
						  this,
						  currentIndex,
						  currentIndex + 1 );
	    formWnd->commandHistory()->addCommand( cmd );
	    cmd->execute();
	    safeInc();
	}
    } else { // ! Ctrl
	safeInc();
    }
    if ( currentIndex >= (int)itemList.count() )
	currentField = 1;
    showSubMenu();
}

void PopupMenuEditor::navigateLeft()
{
    if ( currentItem()->isSeparator() ||
	 currentIndex >= (int)itemList.count() ||
	 currentField == 0 ) {
	if ( parentMenu ) {
	    hideSubMenu();
	    parentMenu->setFocus();
	} else if ( !currentItem()->isSeparator() ) {
	    currentField = 2;
	}
    } else {
	currentField--;
    }
}

void PopupMenuEditor::navigateRight()
{
    if ( !currentItem()->isSeparator() &&
	 currentIndex < (int)itemList.count() ) {
	if ( currentField == 2 ) {
	    focusOnSubMenu();
	} else {
	    currentField++;
	    currentField %= 3;
	}
    }
}

void PopupMenuEditor::enterEditMode( TQKeyEvent * e )
{
    PopupMenuEditorItem * i = currentItem();

    if ( i == &addSeparator ) {
	i = createItem( new TQSeparatorAction( 0 ) );
    } else if ( i->isSeparator() ) {
	return;
    } else if ( currentField == 0 ) {
	choosePixmap();
    } else if ( currentField == 1 ) {
	showLineEdit();
	return;
    } else {// currentField == 2
	setAccelerator( e->key(), e->state() );
    }
    showSubMenu();
    return;
}

void PopupMenuEditor::leaveEditMode( TQKeyEvent * e )
{
    setFocus();
    lineEdit->hide();

    PopupMenuEditorItem * i = 0;
    if ( e && e->key() == TQt::Key_Escape ) {
 	update();
	return;
    }

    if ( currentIndex >= (int)itemList.count() ) {
	// new item was created
	TQAction * a = formWnd->mainWindow()->actioneditor()->newActionEx();
	TQString actionText = lineEdit->text();
	actionText.replace("&&", "&");
	TQString menuText = lineEdit->text();
	a->setText( actionText );
	a->setMenuText( menuText );
	i = createItem( a );
	TQString n = constructName( i );
	formWindow()->unify( a, n, true );
	a->setName( n );
	MetaDataBase::addEntry( a );
	MetaDataBase::setPropertyChanged( a, "menuText", true );
	ActionEditor *ae = (ActionEditor*)formWindow()->mainWindow()->child( 0, "ActionEditor" );
	if ( ae )
	    ae->updateActionName( a );
    } else {
	i = itemList.at( currentIndex );
	RenameActionCommand * cmd = new RenameActionCommand( "Rename Item",
							     formWnd,
							     i->action(),
							     this,
							     lineEdit->text() );
	formWnd->commandHistory()->addCommand( cmd );
	cmd->execute();
    }
    resizeToContents();

    if ( !i )
	return;

    if ( i->isSeparator() )
	hideSubMenu();
    else
	showSubMenu();
}

TQString PopupMenuEditor::constructName( PopupMenuEditorItem *item )
{
    TQString s;
    TQString name = item->action()->menuText();
    TQWidget *e = parentEditor();
    PopupMenuEditor *p = ::tqt_cast<PopupMenuEditor*>(e);
    if ( p ) {
	int idx = p->find( item->m );
	PopupMenuEditorItem * i = ( idx > -1 ? p->at( idx ) : 0 );
	s = ( i ? TQString( i->action()->name() ).remove( "Action" ) : TQString( "" ) );
    } else {
	MenuBarEditor *b = ::tqt_cast<MenuBarEditor*>(e);
	if ( b ) {
	    int idx = b->findItem( item->m );
	    MenuBarEditorItem * i = ( idx > -1 ? b->item( idx ) : 0 );
	    s = ( i ? i->menuText().lower() : TQString( "" ) );
	}
    }
    // replace illegal characters

    return ( RenameMenuCommand::makeLegal( s ) +
	     RenameMenuCommand::makeLegal( name ) + "Action" );
}
