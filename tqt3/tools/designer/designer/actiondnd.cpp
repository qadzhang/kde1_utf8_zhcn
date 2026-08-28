/**********************************************************************
** Copyright (C) 2000-2008 Trolltech ASA.  All rights reserved.
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

#include "actiondnd.h"
#include "command.h"
#include "defs.h"
#include "formwindow.h"
#include "mainwindow.h"
#include "metadatabase.h"
#include "widgetfactory.h"
#include "hierarchyview.h"

#include <ntqaction.h>
#include <ntqapplication.h>
#include <ntqbitmap.h>
#include <ntqdragobject.h>
#include <ntqinputdialog.h>
#include <ntqlayout.h>
#include <ntqmainwindow.h>
#include <ntqmenudata.h>
#include <ntqmessagebox.h>
#include <ntqobjectlist.h>
#include <ntqpainter.h>
#include <ntqstyle.h>
#include <ntqtimer.h>

TQAction *ActionDrag::the_action = 0;

ActionDrag::ActionDrag(TQAction *action, TQWidget *source)
: TQStoredDrag("application/x-designer-actions", source)
{
    Q_ASSERT(the_action == 0);
    the_action = action;
}

ActionDrag::ActionDrag(TQActionGroup *group, TQWidget *source)
: TQStoredDrag("application/x-designer-actiongroup", source)
{
    Q_ASSERT(the_action == 0);
    the_action = group;
}

ActionDrag::ActionDrag(const TQString &type, TQAction *action, TQWidget *source)
: TQStoredDrag(type, source)
{
    Q_ASSERT(the_action == 0);
    the_action = action;
}

bool ActionDrag::canDecode(TQDropEvent *e)
{
    return e->provides( "application/x-designer-actions" ) ||
	   e->provides( "application/x-designer-actiongroup" ) ||
	   e->provides( "application/x-designer-separator" );
}

ActionDrag::~ActionDrag()
{
    the_action = 0;
}

void TQDesignerAction::init()
{
    MetaDataBase::addEntry( this );
    int id = WidgetDatabase::idFromClassName( WidgetFactory::classNameOf( this ) );
    WidgetFactory::saveDefaultProperties( this, id );
    WidgetFactory::saveChangedProperties( this, id );
}

void TQDesignerActionGroup::init()
{
    MetaDataBase::addEntry( this );
    int id = WidgetDatabase::idFromClassName( WidgetFactory::classNameOf( this ) );
    WidgetFactory::saveDefaultProperties( this, id );
    WidgetFactory::saveChangedProperties( this, id );
}

bool TQDesignerAction::addTo( TQWidget *w )
{
    if ( !widgetToInsert )
	return TQAction::addTo( w );

    if ( ::tqt_cast<TQPopupMenu*>(w) )
	return false;

    widgetToInsert->reparent( w, TQPoint( 0, 0 ), false );
    widgetToInsert->show();
    addedTo( widgetToInsert, w );
    return true;
}

bool TQDesignerAction::removeFrom( TQWidget *w )
{
    if ( !widgetToInsert )
	return TQAction::removeFrom( w );

    remove();
    return true;
}

void TQDesignerAction::remove()
{
    if ( !widgetToInsert )
	return;
    MainWindow::self->formWindow()->selectWidget( widgetToInsert, false );
    widgetToInsert->reparent( 0, TQPoint( 0, 0 ), false );
}

class TQDesignerIndicatorWidget : public TQWidget
{
    TQ_OBJECT

public:
    TQDesignerIndicatorWidget( TQWidget *p )
	: TQWidget( p, "qt_dockwidget_internal" ) {
	    setBackgroundColor( red );
    }

};

TQDesignerToolBarSeparator::TQDesignerToolBarSeparator(Orientation o , TQToolBar *parent,
                                     const char* name )
    : TQWidget( parent, name )
{
    connect( parent, TQ_SIGNAL(orientationChanged(Orientation)),
             this, TQ_SLOT(setOrientation(Orientation)) );
    setOrientation( o );
    setBackgroundMode( parent->backgroundMode() );
    setBackgroundOrigin( ParentOrigin );
    setSizePolicy( TQSizePolicy( TQSizePolicy::Minimum, TQSizePolicy::Minimum ) );
}

void TQDesignerToolBarSeparator::setOrientation( Orientation o )
{
    orient = o;
}

void TQDesignerToolBarSeparator::styleChange( TQStyle& )
{
    setOrientation( orient );
}

TQSize TQDesignerToolBarSeparator::sizeHint() const
{
    int extent = style().pixelMetric( TQStyle::PM_DockWindowSeparatorExtent,
				      this );
    if ( orient == Horizontal )
	return TQSize( extent, 0 );
    else
	return TQSize( 0, extent );
}

void TQDesignerToolBarSeparator::paintEvent( TQPaintEvent * )
{
    TQPainter p( this );
    TQStyle::SFlags flags = TQStyle::Style_Default;

    if ( orientation() == Horizontal )
	flags |= TQStyle::Style_Horizontal;

    style().drawPrimitive( TQStyle::PE_DockWindowSeparator, &p, rect(),
			   colorGroup(), flags );
}



TQSeparatorAction::TQSeparatorAction( TQObject *parent )
    : TQAction( parent, "qt_designer_separator" ), wid( 0 )
{
}

bool TQSeparatorAction::addTo( TQWidget *w )
{
    if ( ::tqt_cast<TQToolBar*>(w) ) {
	TQToolBar *tb = (TQToolBar*)w;
	wid = new TQDesignerToolBarSeparator( tb->orientation(), tb );
	return true;
    } else if ( ::tqt_cast<TQPopupMenu*>(w) ) {
	idx = ( (TQPopupMenu*)w )->count();
	( (TQPopupMenu*)w )->insertSeparator( idx );
	return true;
    }
    return false;
}

bool TQSeparatorAction::removeFrom( TQWidget *w )
{
    if ( ::tqt_cast<TQToolBar*>(w) ) {
	delete wid;
	return true;
    } else if ( ::tqt_cast<TQPopupMenu*>(w) ) {
	( (TQPopupMenu*)w )->removeItemAt( idx );
	return true;
    }
    return false;
}

TQWidget *TQSeparatorAction::widget() const
{
    return wid;
}



TQDesignerToolBar::TQDesignerToolBar( TQMainWindow *mw )
    : TQToolBar( mw ), lastIndicatorPos( -1, -1 )
{
    insertAnchor = 0;
    afterAnchor = true;
    setAcceptDrops( true );
    MetaDataBase::addEntry( this );
    lastIndicatorPos = TQPoint( -1, -1 );
    indicator = new TQDesignerIndicatorWidget( this );
    indicator->hide();
    installEventFilter( this );
    widgetInserting = false;
    findFormWindow();
    mw->setDockEnabled( DockTornOff, false );
}

TQDesignerToolBar::TQDesignerToolBar( TQMainWindow *mw, Dock dock )
    : TQToolBar( TQString::null, mw, dock), lastIndicatorPos( -1, -1 )
{
    insertAnchor = 0;
    afterAnchor = true;
    setAcceptDrops( true );
    indicator = new TQDesignerIndicatorWidget( this );
    indicator->hide();
    MetaDataBase::addEntry( this );
    installEventFilter( this );
    widgetInserting = false;
    findFormWindow();
    mw->setDockEnabled( DockTornOff, false );
}

void TQDesignerToolBar::findFormWindow()
{
    TQWidget *w = this;
    while ( w ) {
	formWindow = ::tqt_cast<FormWindow*>(w);
	if ( formWindow )
	    break;
	w = w->parentWidget();
    }
}

void TQDesignerToolBar::addAction( TQAction *a )
{
    actionList.append( a );
    connect( a, TQ_SIGNAL( destroyed() ), this, TQ_SLOT( actionRemoved() ) );
    if ( ::tqt_cast<TQActionGroup*>(a) ) {
	( (TQDesignerActionGroup*)a )->widget()->installEventFilter( this );
	actionMap.insert( ( (TQDesignerActionGroup*)a )->widget(), a );
    } else if ( ::tqt_cast<TQSeparatorAction*>(a) ) {
	( (TQSeparatorAction*)a )->widget()->installEventFilter( this );
	actionMap.insert( ( (TQSeparatorAction*)a )->widget(), a );
    } else {
	( (TQDesignerAction*)a )->widget()->installEventFilter( this );
	actionMap.insert( ( (TQDesignerAction*)a )->widget(), a );
    }
}

static void fixObject( TQObject *&o )
{
    while ( o && o->parent() && !::tqt_cast<TQDesignerToolBar*>(o->parent()) )
	o = o->parent();
}

bool TQDesignerToolBar::eventFilter( TQObject *o, TQEvent *e )
{
    if ( !o || !e || o->inherits( "TQDockWindowHandle" ) || o->inherits( "TQDockWindowTitleBar" ) )
	return TQToolBar::eventFilter( o, e );

    if ( o == this && e->type() == TQEvent::MouseButtonPress &&
	 ( ( TQMouseEvent*)e )->button() == LeftButton ) {
	mousePressEvent( (TQMouseEvent*)e );
	return true;
    }

    if ( o == this )
	return TQToolBar::eventFilter( o, e );

    if ( e->type() == TQEvent::MouseButtonPress ) {
	TQMouseEvent *ke = (TQMouseEvent*)e;
	fixObject( o );
	if ( !o )
	    return false;
	buttonMousePressEvent( ke, o );
	return true;
    } else if(e->type() == TQEvent::ContextMenu ) {
	TQContextMenuEvent *ce = (TQContextMenuEvent*)e;
	fixObject( o );
	if( !o )
	    return false;
	buttonContextMenuEvent( ce, o );
	return true;
    } else if ( e->type() == TQEvent::MouseMove ) {
	TQMouseEvent *ke = (TQMouseEvent*)e;
	fixObject( o );
	if ( !o )
	    return false;
	buttonMouseMoveEvent( ke, o );
	return true;
    } else if ( e->type() == TQEvent::MouseButtonRelease ) {
	TQMouseEvent *ke = (TQMouseEvent*)e;
	fixObject( o );
	if ( !o )
	    return false;
	buttonMouseReleaseEvent( ke, o );
	return true;
    } else if ( e->type() == TQEvent::DragEnter ) {
	TQDragEnterEvent *de = (TQDragEnterEvent*)e;
	if (ActionDrag::canDecode(de))
	    de->accept();
    } else if ( e->type() == TQEvent::DragMove ) {
	TQDragMoveEvent *de = (TQDragMoveEvent*)e;
	if (ActionDrag::canDecode(de))
	    de->accept();
    }

    return TQToolBar::eventFilter( o, e );
}

void TQDesignerToolBar::paintEvent( TQPaintEvent *e )
{
    TQToolBar::paintEvent( e );
    if ( e->rect() != rect() )
	return;
    lastIndicatorPos = TQPoint( -1, -1 );
}

void TQDesignerToolBar::contextMenuEvent( TQContextMenuEvent *e )
{
    e->accept();
    TQPopupMenu menu( 0 );
    menu.insertItem( tr( "Delete Toolbar" ), 1 );
    int res = menu.exec( e->globalPos() );
    if ( res != -1 ) {
	RemoveToolBarCommand *cmd = new RemoveToolBarCommand( tr( "Delete Toolbar '%1'" ).arg( name() ),
							      formWindow, 0, this );
	formWindow->commandHistory()->addCommand( cmd );
	cmd->execute();
    }
}

void TQDesignerToolBar::mousePressEvent( TQMouseEvent *e )
{
    widgetInserting = false;
    if ( e->button() == LeftButton &&
	 MainWindow::self->currentTool() != POINTER_TOOL &&
	 MainWindow::self->currentTool() != ORDER_TOOL &&
	 MainWindow::self->currentTool() != CONNECT_TOOL &&
	 MainWindow::self->currentTool() != BUDDY_TOOL )
	widgetInserting = true;
}

void TQDesignerToolBar::mouseReleaseEvent( TQMouseEvent *e )
{
    if ( widgetInserting )
	doInsertWidget( mapFromGlobal( e->globalPos() ) );
    widgetInserting = false;
}

void TQDesignerToolBar::buttonMouseReleaseEvent( TQMouseEvent *e, TQObject *w )
{
    if ( widgetInserting )
	doInsertWidget( mapFromGlobal( e->globalPos() ) );
    else if ( w->isWidgetType() && formWindow->widgets()->find( w ) ) {
	formWindow->clearSelection( false );
	formWindow->selectWidget( w );
    }
    widgetInserting = false;
}

void TQDesignerToolBar::buttonContextMenuEvent( TQContextMenuEvent *e, TQObject *o )
{
    e->accept();
    TQPopupMenu menu( 0 );
    const int ID_DELETE = 1;
    const int ID_SEP = 2;
    const int ID_DELTOOLBAR = 3;
    TQMap<TQWidget*, TQAction*>::Iterator it = actionMap.find( (TQWidget*)o );
    if ( it != actionMap.end() && ::tqt_cast<TQSeparatorAction*>(*it) )
	menu.insertItem( tr( "Delete Separator" ), ID_DELETE );
    else
	menu.insertItem( tr( "Delete Item" ), ID_DELETE );
    menu.insertItem( tr( "Insert Separator" ), ID_SEP );
    menu.insertSeparator();
    menu.insertItem( tr( "Delete Toolbar" ), ID_DELTOOLBAR );
    int res = menu.exec( e->globalPos() );
    if ( res == ID_DELETE ) {
	TQMap<TQWidget*, TQAction*>::Iterator it = actionMap.find( (TQWidget*)o );
	if ( it == actionMap.end() )
	    return;
	TQAction *a = *it;
	int index = actionList.find( a );
	RemoveActionFromToolBarCommand *cmd = new RemoveActionFromToolBarCommand(
	    tr( "Delete Action '%1' from Toolbar '%2'" ).
	    arg( a->name() ).arg( caption() ),
	    formWindow, a, this, index );
	formWindow->commandHistory()->addCommand( cmd );
	cmd->execute();
    } else if ( res == ID_SEP ) {
	calcIndicatorPos( mapFromGlobal( e->globalPos() ) );
	TQAction *a = new TQSeparatorAction( 0 );
	int index = actionList.findRef( *actionMap.find( insertAnchor ) );
	if ( index != -1 && afterAnchor )
	    ++index;
	if ( !insertAnchor )
	    index = 0;

	AddActionToToolBarCommand *cmd = new AddActionToToolBarCommand(
	    tr( "Add Separator to Toolbar '%1'" ).
	    arg( a->name() ),
	    formWindow, a, this, index );
	formWindow->commandHistory()->addCommand( cmd );
	cmd->execute();
    } else if ( res == ID_DELTOOLBAR ) {
	RemoveToolBarCommand *cmd = new RemoveToolBarCommand( tr( "Delete Toolbar '%1'" ).arg( name() ),
							      formWindow, 0, this );
	formWindow->commandHistory()->addCommand( cmd );
	cmd->execute();
    }
}

void TQDesignerToolBar::buttonMousePressEvent( TQMouseEvent *e, TQObject * )
{
    widgetInserting = false;

    if ( e->button() == MidButton )
	return;

    if ( e->button() == LeftButton &&
	 MainWindow::self->currentTool() != POINTER_TOOL &&
	 MainWindow::self->currentTool() != ORDER_TOOL &&
	 MainWindow::self->currentTool() != CONNECT_TOOL &&
	 MainWindow::self->currentTool() != BUDDY_TOOL ) {
	    widgetInserting = true;
	    return;
    }

    dragStartPos = e->pos();
}

void TQDesignerToolBar::removeWidget( TQWidget *w )
{
    TQMap<TQWidget*, TQAction*>::Iterator it = actionMap.find( w );
    if ( it == actionMap.end() )
	return;
    TQAction *a = *it;
    int index = actionList.find( a );
    RemoveActionFromToolBarCommand *cmd =
	new RemoveActionFromToolBarCommand( tr( "Delete Action '%1' from Toolbar '%2'" ).
					    arg( a->name() ).arg( caption() ),
					    formWindow, a, this, index );
    formWindow->commandHistory()->addCommand( cmd );
    cmd->execute();
    TQApplication::sendPostedEvents();
    adjustSize();
}

void TQDesignerToolBar::buttonMouseMoveEvent( TQMouseEvent *e, TQObject *o )
{
    if ( widgetInserting || ( e->state() & LeftButton ) == 0 )
	return;
    if ( TQABS( TQPoint( dragStartPos - e->pos() ).manhattanLength() ) < TQApplication::startDragDistance() )
	return;
    TQMap<TQWidget*, TQAction*>::Iterator it = actionMap.find( (TQWidget*)o );
    if ( it == actionMap.end() )
	return;
    TQAction *a = *it;
    if ( !a )
	return;
    int index = actionList.find( a );
    RemoveActionFromToolBarCommand *cmd =
	new RemoveActionFromToolBarCommand( tr( "Delete Action '%1' from Toolbar '%2'" ).
					    arg( a->name() ).arg( caption() ),
					    formWindow, a, this, index );
    formWindow->commandHistory()->addCommand( cmd );
    cmd->execute();
    TQApplication::sendPostedEvents();
    adjustSize();

    TQString type = ::tqt_cast<TQActionGroup*>(a) ? TQString( "application/x-designer-actiongroup" ) :
	::tqt_cast<TQSeparatorAction*>(a) ? TQString( "application/x-designer-separator" ) : TQString( "application/x-designer-actions" );
    TQStoredDrag *drag = new ActionDrag( type, a, this );
    drag->setPixmap( a->iconSet().pixmap() );
    if ( ::tqt_cast<TQDesignerAction*>(a) ) {
	if ( formWindow->widgets()->find( ( (TQDesignerAction*)a )->widget() ) )
	    formWindow->selectWidget( ( (TQDesignerAction*)a )->widget(), false );
    }
    if ( !drag->drag() ) {
	AddActionToToolBarCommand *cmd = new AddActionToToolBarCommand( tr( "Add Action '%1' to Toolbar '%2'" ).
									arg( a->name() ).arg( caption() ),
									formWindow, a, this, index );
	formWindow->commandHistory()->addCommand( cmd );
	cmd->execute();
    }
    lastIndicatorPos = TQPoint( -1, -1 );
    indicator->hide();
}

#ifndef TQT_NO_DRAGANDDROP

void TQDesignerToolBar::dragEnterEvent( TQDragEnterEvent *e )
{
    widgetInserting = false;
    lastIndicatorPos = TQPoint( -1, -1 );
    if (ActionDrag::canDecode(e))
	e->accept();
}

void TQDesignerToolBar::dragMoveEvent( TQDragMoveEvent *e )
{
    if (ActionDrag::canDecode(e)) {
	e->accept();
	drawIndicator( calcIndicatorPos( e->pos() ) );
    }
}

void TQDesignerToolBar::dragLeaveEvent( TQDragLeaveEvent * )
{
    indicator->hide();
    insertAnchor = 0;
    afterAnchor = true;
}

void TQDesignerToolBar::dropEvent( TQDropEvent *e )
{
    if (!ActionDrag::canDecode(e))
	return;

    e->accept();

    indicator->hide();
    TQAction *a = 0;
    int index = actionList.findRef( *actionMap.find( insertAnchor ) );
    if ( index != -1 && afterAnchor )
	++index;
    if ( !insertAnchor )
	index = 0;
    if ( e->provides( "application/x-designer-actions" ) ||
	 e->provides( "application/x-designer-separator" ) ) {
	if ( e->provides( "application/x-designer-actions" ) )
	    a = ::tqt_cast<TQDesignerAction*>(ActionDrag::action());
	else
	    a = ::tqt_cast<TQSeparatorAction*>(ActionDrag::action());
    } else {
	a = ::tqt_cast<TQDesignerActionGroup*>(ActionDrag::action());
    }

    if ( actionList.findRef( a ) != -1 ) {
	TQMessageBox::warning( MainWindow::self, tr( "Insert/Move Action" ),
			      tr( "Action '%1' has already been added to this toolbar.\n"
				  "An Action may only occur once in a given toolbar." ).
			      arg( a->name() ) );
	return;
    }

    AddActionToToolBarCommand *cmd = new AddActionToToolBarCommand( tr( "Add Action '%1' to Toolbar '%2'" ).
								    arg( a->name() ).arg( caption() ),
								    formWindow, a, this, index );
    formWindow->commandHistory()->addCommand( cmd );
    cmd->execute();

    lastIndicatorPos = TQPoint( -1, -1 );
}

#endif

void TQDesignerToolBar::reInsert()
{
    TQAction *a = 0;
    actionMap.clear();
    clear();
    for ( a = actionList.first(); a; a = actionList.next() ) {
	a->addTo( this );
	if ( ::tqt_cast<TQActionGroup*>(a) ) {
	    actionMap.insert( ( (TQDesignerActionGroup*)a )->widget(), a );
	    if ( ( (TQDesignerActionGroup*)a )->widget() )
		( (TQDesignerActionGroup*)a )->widget()->installEventFilter( this );
	} else if ( ::tqt_cast<TQDesignerAction*>(a) ) {
	    actionMap.insert( ( (TQDesignerAction*)a )->widget(), a );
	    ( (TQDesignerAction*)a )->widget()->installEventFilter( this );
	} else if ( ::tqt_cast<TQSeparatorAction*>(a) ) {
	    actionMap.insert( ( (TQSeparatorAction*)a )->widget(), a );
	    ( (TQSeparatorAction*)a )->widget()->installEventFilter( this );
	}
    }
    TQApplication::sendPostedEvents();
    adjustSize();
}

void TQDesignerToolBar::actionRemoved()
{
    actionList.removeRef( (TQAction*)sender() );
}

TQPoint TQDesignerToolBar::calcIndicatorPos( const TQPoint &pos )
{
    if ( orientation() == Horizontal ) {
	TQPoint pnt( width() - 2, 0 );
	insertAnchor = 0;
	afterAnchor = true;
	if ( !children() )
	    return pnt;
	pnt = TQPoint( 13, 0 );
	TQObjectListIt it( *children() );
	TQObject * obj;
	while( (obj=it.current()) != 0 ) {
	    ++it;
	    if ( obj->isWidgetType() &&
		 qstrcmp( "qt_dockwidget_internal", obj->name() ) != 0 ) {
		TQWidget *w = (TQWidget*)obj;
		if ( w->x() < pos.x() ) {
		    pnt.setX( w->x() + w->width() + 1 );
		    insertAnchor = w;
		    afterAnchor = true;
		}
	    }
	}
	return pnt;
    } else {
	TQPoint pnt( 0, height() - 2 );
	insertAnchor = 0;
	afterAnchor = true;
	if ( !children() )
	    return pnt;
	pnt = TQPoint( 0, 13 );
	TQObjectListIt it( *children() );
	TQObject * obj;
	while( (obj=it.current()) != 0 ) {
	    ++it;
	    if ( obj->isWidgetType() &&
		 qstrcmp( "qt_dockwidget_internal", obj->name() ) != 0 ) {
		TQWidget *w = (TQWidget*)obj;
		if ( w->y() < pos.y() ) {
		    pnt.setY( w->y() + w->height() + 1 );
		    insertAnchor = w;
		    afterAnchor = true;
		}
	    }
	}
	return pnt;
    }
}

void TQDesignerToolBar::drawIndicator( const TQPoint &pos )
{
    if ( lastIndicatorPos == pos )
	return;
    bool wasVsisible = indicator->isVisible();
    if ( orientation() == Horizontal ) {
	indicator->resize( 3, height() );
	if ( pos != TQPoint( -1, -1 ) )
	     indicator->move( pos.x() - 1, 0 );
	indicator->show();
	indicator->raise();
	lastIndicatorPos = pos;
    } else {
	indicator->resize( width(), 3 );
	if ( pos != TQPoint( -1, -1 ) )
	     indicator->move( 0, pos.y() - 1 );
	indicator->show();
	indicator->raise();
	lastIndicatorPos = pos;
    }
    if ( !wasVsisible )
	TQApplication::sendPostedEvents();
}

void TQDesignerToolBar::doInsertWidget( const TQPoint &p )
{
    if ( formWindow != MainWindow::self->formWindow() )
	return;
    calcIndicatorPos( p );
    TQWidget *w = WidgetFactory::create( MainWindow::self->currentTool(), this, 0, true );
    installEventFilters( w );
    MainWindow::self->formWindow()->insertWidget( w, true );
    TQDesignerAction *a = new TQDesignerAction( w, parent() );
    int index = actionList.findRef( *actionMap.find( insertAnchor ) );
    if ( index != -1 && afterAnchor )
	++index;
    if ( !insertAnchor )
	index = 0;
    AddActionToToolBarCommand *cmd = new AddActionToToolBarCommand( tr( "Add Widget '%1' to Toolbar '%2'" ).
								    arg( w->name() ).arg( caption() ),
								    formWindow, a, this, index );
    formWindow->commandHistory()->addCommand( cmd );
    cmd->execute();
    MainWindow::self->resetTool();
}

void TQDesignerToolBar::clear()
{
    for ( TQAction *a = actionList.first(); a; a = actionList.next() ) {
	if ( ::tqt_cast<TQDesignerAction*>(a) )
	    ( (TQDesignerAction*)a )->remove();
    }
    TQToolBar::clear();
}

void TQDesignerToolBar::installEventFilters( TQWidget *w )
{
    if ( !w )
	return;
    TQObjectList *l = w->queryList( "TQWidget" );
    for ( TQObject *o = l->first(); o; o = l->next() )
	o->installEventFilter( this );
    delete l;
}

#include "actiondnd.moc"
