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

#include "hierarchyview.h"
#include "formwindow.h"
#include "globaldefs.h"
#include "mainwindow.h"
#include "command.h"
#include "widgetfactory.h"
#include "widgetdatabase.h"
#include "project.h"
#include "sourceeditor.h"
#include "propertyeditor.h"
#include "editfunctionsimpl.h"
#include "listeditor.h"
#include "actiondnd.h"
#include "actioneditorimpl.h"
#include "variabledialogimpl.h"
#include "popupmenueditor.h"
#include "menubareditor.h"

#include <ntqpalette.h>
#include <ntqobjectlist.h>
#include <ntqheader.h>
#include <ntqpopupmenu.h>
#include <ntqtabwidget.h>
#include <ntqwizard.h>
#include <ntqwidgetstack.h>
#include <ntqtabbar.h>
#include <ntqfeatures.h>
#include <ntqapplication.h>
#include <ntqtimer.h>
#include "../interfaces/languageinterface.h"
#include <ntqworkspace.h>
#include <ntqaccel.h>
#include <ntqmessagebox.h>

#include <stdlib.h>

TQListViewItem *newItem = 0;

static TQPluginManager<ClassBrowserInterface> *classBrowserInterfaceManager = 0;

HierarchyItem::HierarchyItem( Type type, TQListViewItem *parent, TQListViewItem *after,
			      const TQString &txt1, const TQString &txt2, const TQString &txt3 )
    : TQListViewItem( parent, after, txt1, txt2, txt3 ), typ( type )
{
}

HierarchyItem::HierarchyItem( Type type, TQListView *parent, TQListViewItem *after,
			      const TQString &txt1, const TQString &txt2, const TQString &txt3 )
    : TQListViewItem( parent, after, txt1, txt2, txt3 ), typ( type )
{
}

void HierarchyItem::paintCell( TQPainter *p, const TQColorGroup &cg, int column, int width, int align )
{
    TQColorGroup g( cg );
    g.setColor( TQColorGroup::Base, backgroundColor() );
    g.setColor( TQColorGroup::Foreground, TQt::black );
    g.setColor( TQColorGroup::Text, TQt::black );
    TQString txt = text( 0 );
    if ( rtti() == Function &&
	 MainWindow::self->currProject()->isCpp() &&
	 ( txt == "init()" || txt == "destroy()" ) ) {
	listView()->setUpdatesEnabled( false );
	if ( txt == "init()" )
	    setText( 0, txt + " " + "(Constructor)" );
	else
	    setText( 0, txt + " " + "(Destructor)" );
	TQListViewItem::paintCell( p, g, column, width, align );
	setText( 0, txt );
	listView()->setUpdatesEnabled( true );
    } else {
	TQListViewItem::paintCell( p, g, column, width, align );
    }
    p->save();
    p->setPen( TQPen( cg.dark(), 1 ) );
    if ( column == 0 )
	p->drawLine( 0, 0, 0, height() - 1 );
    if ( listView()->firstChild() != this ) {
	if ( nextSibling() != itemBelow() && itemBelow()->depth() < depth() ) {
	    int d = depth() - itemBelow()->depth();
	    p->drawLine( -listView()->treeStepSize() * d, height() - 1, 0, height() - 1 );
	}
    }
    p->drawLine( 0, height() - 1, width, height() - 1 );
    p->drawLine( width - 1, 0, width - 1, height() );
    p->restore();
}

TQColor HierarchyItem::backgroundColor()
{
    updateBackColor();
    return backColor;
}

void HierarchyItem::updateBackColor()
{
    if ( listView()->firstChild() == this ) {
	backColor = *backColor1;
	return;
    }

    TQListViewItemIterator it( this );
    --it;
    if ( it.current() ) {
	if ( ( ( HierarchyItem*)it.current() )->backColor == *backColor1 )
	    backColor = *backColor2;
	else
	    backColor = *backColor1;
    } else {
	backColor = *backColor1;
    }
}

void HierarchyItem::setObject( TQObject *o )
{
    obj = o;
}

TQObject *HierarchyItem::object() const
{
    return obj;
}

void HierarchyItem::okRename( int col )
{
    if ( newItem == this )
	newItem = 0;
    TQListViewItem::okRename( col );
}

void HierarchyItem::cancelRename( int col )
{
    if ( newItem == this ) {
	newItem = 0;
	TQListViewItem::cancelRename( col );
	delete this;
	return;
    }
    TQListViewItem::cancelRename( col );
}




HierarchyList::HierarchyList( TQWidget *parent, FormWindow *fw, bool doConnects )
    : TQListView( parent ), formWindow( fw )
{
    init_colors();

    setDefaultRenameAction( Accept );
    header()->setMovingEnabled( false );
    header()->setStretchEnabled( true );
    normalMenu = 0;
    tabWidgetMenu = 0;
    addColumn( tr( "Name" ) );
    addColumn( tr( "Class" ) );
    TQPalette p( palette() );
    p.setColor( TQColorGroup::Base, TQColor( *backColor2 ) );
    (void)*selectedBack; // hack
    setPalette( p );
    disconnect( header(), TQ_SIGNAL( sectionClicked( int ) ),
		this, TQ_SLOT( changeSortColumn( int ) ) );
    setSorting( -1 );
    setHScrollBarMode( AlwaysOff );
    setVScrollBarMode( AlwaysOn );
    if ( doConnects ) {
	connect( this, TQ_SIGNAL( clicked( TQListViewItem * ) ),
		 this, TQ_SLOT( objectClicked( TQListViewItem * ) ) );
	connect( this, TQ_SIGNAL( doubleClicked( TQListViewItem * ) ),
		 this, TQ_SLOT( objectDoubleClicked( TQListViewItem * ) ) );
	connect( this, TQ_SIGNAL( returnPressed( TQListViewItem * ) ),
		 this, TQ_SLOT( objectClicked( TQListViewItem * ) ) );
	connect( this, TQ_SIGNAL( contextMenuRequested( TQListViewItem *, const TQPoint&, int ) ),
		 this, TQ_SLOT( showRMBMenu( TQListViewItem *, const TQPoint & ) ) );
    }
    deselect = true;
    setColumnWidthMode( 1, Manual );
}

void HierarchyList::keyPressEvent( TQKeyEvent *e )
{
    if ( e->key() == Key_Shift || e->key() == Key_Control )
	deselect = false;
    else
	deselect = true;
    TQListView::keyPressEvent( e );
}

void HierarchyList::keyReleaseEvent( TQKeyEvent *e )
{
    deselect = true;
    TQListView::keyReleaseEvent( e );
}

void HierarchyList::viewportMousePressEvent( TQMouseEvent *e )
{
    if ( e->state() & ShiftButton || e->state() & ControlButton )
	deselect = false;
    else
	deselect = true;
    TQListView::viewportMousePressEvent( e );
}

void HierarchyList::viewportMouseReleaseEvent( TQMouseEvent *e )
{
    TQListView::viewportMouseReleaseEvent( e );
}

TQObject *HierarchyList::handleObjectClick( TQListViewItem *i )
{
    if ( !i )
	return 0;

    TQObject *o = findObject( i );
    if ( !o )
	return 0;
    if ( formWindow == o ) {
	if ( deselect )
	    formWindow->clearSelection( false );
	formWindow->emitShowProperties( formWindow );
	return 0;
    }
    if ( o->isWidgetType() ) {
	TQWidget *w = (TQWidget*)o;
	if ( !formWindow->widgets()->find( w ) ) {
	    if ( ::tqt_cast<TQWidgetStack*>(w->parent()) ) {
		if (::tqt_cast<TQTabWidget*>(w->parent()->parent()) ) {
		    ((TQTabWidget*)w->parent()->parent())->showPage( w );
		    o = (TQWidget*)w->parent()->parent();
		    formWindow->emitUpdateProperties( formWindow->currentWidget() );
		} else if ( ::tqt_cast<TQWizard*>(w->parent()->parent()) ) {
		    ((TQDesignerWizard*)w->parent()->parent())->
			setCurrentPage( ( (TQDesignerWizard*)w->parent()->parent() )->pageNum( w ) );
		    o = (TQWidget*)w->parent()->parent();
		    formWindow->emitUpdateProperties( formWindow->currentWidget() );
		} else {
		    ( (TQWidgetStack*)w->parent() )->raiseWidget( w );
		    if ( (TQWidgetStack*)w->parent()->isA( "TQDesignerWidgetStack" ) )
			( (TQDesignerWidgetStack*)w->parent() )->updateButtons();
		}
	    } else if ( ::tqt_cast<TQMenuBar*>(w) || ::tqt_cast<TQDockWindow*>(w) ) {
		formWindow->setActiveObject( w );
	    } else if ( ::tqt_cast<TQPopupMenu*>(w) ) {
		return 0; // ### we could try to find our menu bar and change the currentMenu to our index
	    } else {
		return 0;
	    }
	}
    } else if ( ::tqt_cast<TQAction*>(o) ) {
	MainWindow::self->actioneditor()->setCurrentAction( (TQAction*)o );
	deselect = true;
    }

    if ( deselect )
	formWindow->clearSelection( false );

    return o;
}


void HierarchyList::objectDoubleClicked( TQListViewItem *i )
{
    TQObject *o = handleObjectClick( i );
    if ( !o )
	return;
    if ( o->isWidgetType() && ( (TQWidget*)o )->isVisibleTo( formWindow ) ) {
	TQWidget *w = (TQWidget*)o;
	if ( !w->parentWidget() ||
	     WidgetFactory::layoutType( w->parentWidget() ) == WidgetFactory::NoLayout )
	    w->raise();
	formWindow->selectWidget( w, true );
    }
}

void HierarchyList::objectClicked( TQListViewItem *i )
{
    TQObject *o = handleObjectClick( i );
    if ( !o )
	return;
    if ( o->isWidgetType() && ( (TQWidget*)o )->isVisibleTo( formWindow ) ) {
	TQWidget *w = (TQWidget*)o;
	formWindow->selectWidget( w, true );
    }
}

TQObject *HierarchyList::findObject( TQListViewItem *i )
{
    return ( (HierarchyItem*)i )->object();
}

TQListViewItem *HierarchyList::findItem( TQObject *o )
{
    TQListViewItemIterator it( this );
    while ( it.current() ) {
	if ( ( (HierarchyItem*)it.current() )->object() == o )
	    return it.current();
	++it;
    }
    return 0;
}

TQObject *HierarchyList::current() const
{
    if ( currentItem() )
	return ( (HierarchyItem*)currentItem() )->object();
    return 0;
}

void HierarchyList::changeNameOf( TQObject *o, const TQString &name )
{
    TQListViewItem *item = findItem( o );
    if ( !item )
	return;
    item->setText( 0, name );
}


void HierarchyList::changeDatabaseOf( TQObject *o, const TQString &info )
{
#ifndef TQT_NO_SQL
    if ( !formWindow->isDatabaseAware() )
	return;
    TQListViewItem *item = findItem( o );
    if ( !item )
	return;
    item->setText( 2, info );
#endif
}

static TQPtrList<TQWidgetStack> *widgetStacks = 0;

void HierarchyList::setup()
{
    if ( !formWindow || formWindow->isFake() )
	return;
    clear();
    TQWidget *w = formWindow->mainContainer();
#ifndef TQT_NO_SQL
    if ( formWindow->isDatabaseAware() ) {
	if ( columns() == 2 ) {
	    addColumn( tr( "Database" ) );
	    header()->resizeSection( 0, 1 );
	    header()->resizeSection( 1, 1 );
	    header()->resizeSection( 2, 1 );
	    header()->adjustHeaderSize();
	}
    } else {
	if ( columns() == 3 ) {
	    removeColumn( 2 );
	}
    }
#endif
    if ( !widgetStacks )
	widgetStacks = new TQPtrList<TQWidgetStack>;
    if ( w )
	insertObject( w, 0 );
    widgetStacks->clear();
}

void HierarchyList::setOpen( TQListViewItem *i, bool b )
{
    TQListView::setOpen( i, b );
}

void HierarchyList::insertObject( TQObject *o, TQListViewItem *parent )
{
    if ( TQString( o->name() ).startsWith( "qt_dead_widget_" ) )
	return;
    bool fakeMainWindow = false;
    if ( ::tqt_cast<TQMainWindow*>(o) ) {
	TQObject *cw = ( (TQMainWindow*)o )->centralWidget();
	if ( cw ) {
	    o = cw;
	    fakeMainWindow = true;
	}
    }
    TQListViewItem *item = 0;
    TQString className = WidgetFactory::classNameOf( o );
    if ( ::tqt_cast<TQLayoutWidget*>(o) ) {
	switch ( WidgetFactory::layoutType( (TQWidget*)o ) ) {
	case WidgetFactory::HBox:
	    className = "HBox";
	    break;
	case WidgetFactory::VBox:
	    className = "VBox";
	    break;
	case WidgetFactory::Grid:
	    className = "Grid";
	    break;
	default:
	    break;
	}
    }

    TQString dbInfo;
#ifndef TQT_NO_SQL
    dbInfo = MetaDataBase::fakeProperty( o, "database" ).toStringList().join(".");
#endif

    TQString name = o->name();
    if ( ::tqt_cast<TQWidgetStack*>(o->parent()) ) {
	if ( ::tqt_cast<TQTabWidget*>(o->parent()->parent()) )
	    name = ( (TQTabWidget*)o->parent()->parent() )->tabLabel( (TQWidget*)o );
	else if ( ::tqt_cast<TQWizard*>(o->parent()->parent()) )
	    name = ( (TQWizard*)o->parent()->parent() )->title( (TQWidget*)o );
    }

    TQToolBox *tb;
    if ( o->parent() && o->parent()->parent() &&
	 (tb = ::tqt_cast<TQToolBox*>(o->parent()->parent()->parent())) )
	name = tb->itemLabel( tb->indexOf((TQWidget*)o) );

    if ( fakeMainWindow ) {
	name = o->parent()->name();
	className = "TQMainWindow";
    }

    if ( !parent )
	item = new HierarchyItem( HierarchyItem::Widget, this, 0, name, className, dbInfo );
    else
	item = new HierarchyItem( HierarchyItem::Widget, parent, 0, name, className, dbInfo );
    item->setOpen( true );
    if ( !parent )
	item->setPixmap( 0, TQPixmap::fromMimeSource( "designer_form.png" ) );
    else if ( ::tqt_cast<TQLayoutWidget*>(o) )
	item->setPixmap( 0, TQPixmap::fromMimeSource( "designer_layout.png" ));
    else
	item->setPixmap( 0, WidgetDatabase::iconSet(
		    WidgetDatabase::idFromClassName( WidgetFactory::classNameOf( o ) ) ).
			 pixmap( TQIconSet::Small, TQIconSet::Normal ) );
    if ( ::tqt_cast<TQAction*>(o) )
	item->setPixmap( 0, ( (TQAction*)o )->iconSet().pixmap() );

    ( (HierarchyItem*)item )->setObject( o );
    const TQObjectList *l = o->children();
    if ( ::tqt_cast<TQDesignerToolBar*>(o) )
	l = 0;
    if ( l ) {
	TQObjectListIt it( *l );
	it.toLast();
	for ( ; it.current(); --it ) {
	    if ( !it.current()->isWidgetType() ||
		 ( (TQWidget*)it.current() )->isHidden() )
		continue;
	    if ( !formWindow->widgets()->find( (TQWidget*)it.current() ) ) {
		if ( ::tqt_cast<TQWidgetStack*>(it.current()->parent()) ||
		     ::tqt_cast<TQWidgetStack*>(it.current()) ) {
		    TQObject *obj = it.current();
		    TQDesignerTabWidget *tw = ::tqt_cast<TQDesignerTabWidget*>(it.current()->parent());
		    TQDesignerWizard *dw = ::tqt_cast<TQDesignerWizard*>(it.current()->parent());
		    TQWidgetStack *stack = 0;
		    if ( dw || tw || ::tqt_cast<TQWidgetStack*>(obj) )
			stack = (TQWidgetStack*)obj;
		    else
			stack = (TQWidgetStack*)obj->parent();
		    if ( widgetStacks->findRef( stack ) != -1 )
			continue;
		    widgetStacks->append( stack );
		    TQObjectList *l2 = stack->queryList( "TQWidget", 0, true, false );
		    for ( obj = l2->last(); obj; obj = l2->prev() ) {
			if ( qstrcmp( obj->className(),
				      "TQWidgetStackPrivate::Invisible" ) == 0 ||
			     ( tw && !tw->tabBar()->tab( stack->id( (TQWidget*)obj ) ) ) ||
			     ( dw && dw->isPageRemoved( (TQWidget*)obj ) ) )
			    continue;
			if ( qstrcmp( obj->name(), "designer_wizardstack_button" ) == 0 )
			    continue;
			if ( stack->id( (TQWidget*)obj ) == -1 )
			    continue;
			insertObject( obj, item );
		    }
		    delete l2;
		} else if ( ::tqt_cast<TQToolBox*>(it.current()->parent()) ) {
		    if ( !::tqt_cast<TQScrollView*>(it.current()) )
			continue;
		    TQToolBox *tb = (TQToolBox*)it.current()->parent();
		    for ( int i = tb->count() - 1; i >= 0; --i )
			insertObject( tb->item( i ), item );
		}
		continue;
	    }
	    insertObject( it.current(), item );
	}
    }
    
    if ( fakeMainWindow ) {
	TQObjectList *l = o->parent()->queryList( "TQDesignerToolBar" );
	TQObject *obj;
	for ( obj = l->first(); obj; obj = l->next() )
	    insertObject( obj, item );
	delete l;
	l = o->parent()->queryList( "MenuBarEditor" );
	for ( obj = l->first(); obj; obj = l->next() )
	    insertObject( obj, item );
	delete l;
    } else if ( ::tqt_cast<TQDesignerToolBar*>(o) || ::tqt_cast<PopupMenuEditor*>(o) ) {
	TQPtrList<TQAction> actions;
	if ( ::tqt_cast<TQDesignerToolBar*>(o) )
	    actions = ( (TQDesignerToolBar*)o )->insertedActions();
	else
	    ( (PopupMenuEditor*)o )->insertedActions( actions );

	TQPtrListIterator<TQAction> it( actions );
	it.toLast();
	while ( it.current() ) {
	    TQAction *a = it.current();
	    if ( ::tqt_cast<TQDesignerAction*>(a) ) {
		TQDesignerAction *da = (TQDesignerAction*)a;
		if ( da->supportsMenu() )
		    insertObject( da, item );
		else
		    insertObject( da->widget(), item );
	    } else if ( ::tqt_cast<TQDesignerActionGroup*>(a) ) {
		insertObject( a, item );
	    }
	    --it;
	}
    } else if ( ::tqt_cast<TQDesignerActionGroup*>(o) && o->children() ) {
	TQObjectList *l = (TQObjectList*)o->children();
	for ( TQObject *obj = l->last(); obj; obj = l->prev() ) {
	    if ( ::tqt_cast<TQDesignerAction*>(obj) ) {
		TQDesignerAction *da = (TQDesignerAction*)obj;
		if ( da->supportsMenu() )
		    insertObject( da, item );
		else
		    insertObject( da->widget(), item );
	    } else if ( ::tqt_cast<TQDesignerActionGroup*>(obj) ) {
		insertObject( obj, item );
	    }
	}
    } else if ( ::tqt_cast<MenuBarEditor*>(o) ) {
	MenuBarEditor *mb = (MenuBarEditor*)o;
	for ( int i = mb->count() -1; i >= 0; --i ) {
	    MenuBarEditorItem *md = mb->item( i );
	    if ( !md || !md->menu() )
		continue;
	    insertObject( md->menu(), item );
	}
    }
}

void HierarchyList::setCurrent( TQObject *o )
{
    TQListViewItemIterator it( this );
    while ( it.current() ) {
	if ( ( (HierarchyItem*)it.current() )->object() == o ) {
	    blockSignals( true );
	    setCurrentItem( it.current() );
	    ensureItemVisible( it.current() );
	    blockSignals( false );
	    return;
	}
	++it;
    }
}

void HierarchyList::showRMBMenu( TQListViewItem *i, const TQPoint & p )
{
    if ( !i )
	return;

    TQObject *o = findObject( i );
    if ( !o )
	return;

    if ( !o->isWidgetType() ||
	 ( o != formWindow && !formWindow->widgets()->find( (TQWidget*)o ) ) )
	return;

    TQWidget *w = (TQWidget*)o;
    if ( w->isVisibleTo( formWindow ) ) {
	if ( !::tqt_cast<TQTabWidget*>(w) && !::tqt_cast<TQWizard*>(w) ) {
	    if ( !normalMenu )
		normalMenu = formWindow->mainWindow()->setupNormalHierarchyMenu( this );
	    normalMenu->popup( p );
	} else {
	    if ( !tabWidgetMenu )
		tabWidgetMenu =
		    formWindow->mainWindow()->setupTabWidgetHierarchyMenu(
				  this, TQ_SLOT( addTabPage() ),
				  TQ_SLOT( removeTabPage() ) );
	    tabWidgetMenu->popup( p );
	}
    }
}

void HierarchyList::addTabPage()
{
    TQObject *o = current();
    if ( !o || !o->isWidgetType() )
	return;
    TQWidget *w = (TQWidget*)o;
    if ( ::tqt_cast<TQTabWidget*>(w) ) {
	TQTabWidget *tw = (TQTabWidget*)w;
	AddTabPageCommand *cmd = new AddTabPageCommand( tr( "Add Page to %1" ).
							arg( tw->name() ), formWindow,
							tw, "Tab" );
	formWindow->commandHistory()->addCommand( cmd );
	cmd->execute();
    } else if ( ::tqt_cast<TQWizard*>(w) ) {
	TQWizard *wiz = (TQWizard*)formWindow->mainContainer();
	AddWizardPageCommand *cmd = new AddWizardPageCommand( tr( "Add Page to %1" ).
							      arg( wiz->name() ), formWindow,
							      wiz, "Page" );
	formWindow->commandHistory()->addCommand( cmd );
	cmd->execute();
    }
}

void HierarchyList::removeTabPage()
{
    TQObject *o = current();
    if ( !o || !o->isWidgetType() )
	return;
    TQWidget *w = (TQWidget*)o;
    if ( ::tqt_cast<TQTabWidget*>(w) ) {
	TQTabWidget *tw = (TQTabWidget*)w;
	if ( tw->currentPage() ) {
	    TQDesignerTabWidget *dtw = (TQDesignerTabWidget*)tw;
	    DeleteTabPageCommand *cmd =
		new DeleteTabPageCommand( tr( "Delete Page %1 of %2" ).
					  arg( dtw->pageTitle() ).arg( tw->name() ),
					  formWindow, tw, tw->currentPage() );
	    formWindow->commandHistory()->addCommand( cmd );
	    cmd->execute();
	}
    } else if ( ::tqt_cast<TQWizard*>(w) ) {
	TQWizard *wiz = (TQWizard*)formWindow->mainContainer();
	if ( wiz->currentPage() ) {
	    TQDesignerWizard *dw = (TQDesignerWizard*)wiz;
	    DeleteWizardPageCommand *cmd =
		new DeleteWizardPageCommand( tr( "Delete Page %1 of %2" ).
					     arg( dw->pageTitle() ).arg( wiz->name() ),
					     formWindow, wiz,
					     wiz->indexOf( wiz->currentPage() ), true );
	    formWindow->commandHistory()->addCommand( cmd );
	    cmd->execute();
	}
    }
}

// ------------------------------------------------------------

FormDefinitionView::FormDefinitionView( TQWidget *parent, FormWindow *fw )
    : HierarchyList( parent, fw, true )
{
    header()->hide();
    removeColumn( 1 );
    connect( this, TQ_SIGNAL( itemRenamed( TQListViewItem *, int, const TQString & ) ),
	     this, TQ_SLOT( renamed( TQListViewItem * ) ) );
    popupOpen = false;
}

void FormDefinitionView::setup()
{
    if ( popupOpen || !formWindow )
	return;
    if ( !formWindow->project()->isCpp() )
	return;
    TQListViewItem *i = firstChild();
    while ( i ) {
	if ( i->rtti() == HierarchyItem::DefinitionParent ) {
	    TQListViewItem *a = i;
	    i = i->nextSibling();
	    delete a;
	    continue;
	}
	i = i->nextSibling();
    }

    LanguageInterface *lIface = MetaDataBase::languageInterface( formWindow->project()->language() );
    if ( lIface ) {
	TQStringList defs = lIface->definitions();
	for ( TQStringList::Iterator dit = defs.begin(); dit != defs.end(); ++dit ) {
	    HierarchyItem *itemDef = new HierarchyItem( HierarchyItem::DefinitionParent, this, 0,
							tr( *dit ), TQString::null, TQString::null );
	    itemDef->setPixmap( 0, TQPixmap::fromMimeSource( "designer_folder.png" ) );
	    itemDef->setOpen( true );
	    TQStringList entries =
		lIface->definitionEntries( *dit, formWindow->mainWindow()->designerInterface() );
	    HierarchyItem *item = 0;
	    for ( TQStringList::Iterator eit = entries.begin(); eit != entries.end(); ++eit ) {
		item = new HierarchyItem( HierarchyItem::Definition,
					  itemDef, item, *eit, TQString::null, TQString::null );
		item->setRenameEnabled( 0, true );
	    }
	}
	lIface->release();
    }
    setupVariables();
    refresh();
}

void FormDefinitionView::setupVariables()
{
    bool pubOpen, protOpen, privOpen;
    pubOpen = protOpen = privOpen = true;
    TQListViewItem *i = firstChild();
    while ( i ) {
	if ( i->rtti() == HierarchyItem::VarParent ) {
	    TQListViewItem *a = i;
	    i = i->firstChild();
	    while ( i ) {
		if ( i->rtti() == HierarchyItem::VarPublic )
		    pubOpen = i->isOpen();
		else if ( i->rtti() == HierarchyItem::VarProtected )
		    protOpen = i->isOpen();
		else if ( i->rtti() == HierarchyItem::VarPrivate )
		    privOpen = i->isOpen();
		i = i->nextSibling();
	    }
	    delete a;
	    break;
	}
	i = i->nextSibling();
    }

    HierarchyItem *itemVar = new HierarchyItem( HierarchyItem::VarParent, this, 0, tr( "Class Variables" ),
						TQString::null, TQString::null );
    itemVar->setPixmap( 0, TQPixmap::fromMimeSource( "designer_folder.png" ) );
    itemVar->setOpen( true );

    itemVarPriv = new HierarchyItem( HierarchyItem::VarPrivate, itemVar, 0, tr( "private" ),
				     TQString::null, TQString::null );
    itemVarProt = new HierarchyItem( HierarchyItem::VarProtected, itemVar, 0, tr( "protected" ),
				     TQString::null, TQString::null );
    itemVarPubl = new HierarchyItem( HierarchyItem::VarPublic, itemVar, 0, tr( "public" ),
				     TQString::null, TQString::null );

    TQValueList<MetaDataBase::Variable> varList = MetaDataBase::variables( formWindow );
    TQValueList<MetaDataBase::Variable>::Iterator it = --( varList.end() );
    if ( !varList.isEmpty() && itemVar ) {
	for (;;) {
	    TQListViewItem *item = 0;
	    if ( (*it).varAccess == "public" )
		item = new HierarchyItem( HierarchyItem::Variable, itemVarPubl, 0, (*it).varName,
					  TQString::null, TQString::null );
	    else if ( (*it).varAccess == "private" )
		item = new HierarchyItem( HierarchyItem::Variable, itemVarPriv, 0, (*it).varName,
					  TQString::null, TQString::null );
	    else // default is protected
		item = new HierarchyItem( HierarchyItem::Variable, itemVarProt, 0, (*it).varName,
					  TQString::null, TQString::null );
	    item->setPixmap( 0, TQPixmap::fromMimeSource( "designer_editslots.png" ) );
	    if ( it == varList.begin() )
		break;
	    --it;
	}
    }
    itemVar->setOpen( true );
    itemVarPriv->setOpen( privOpen );
    itemVarProt->setOpen( protOpen );
    itemVarPubl->setOpen( pubOpen );
}

void FormDefinitionView::refresh()
{
    if ( popupOpen || !formWindow || !formWindow->project()->isCpp() )
	return;

    bool fuPub, fuProt, fuPriv, slPub, slProt, slPriv;
    fuPub = fuProt = fuPriv = slPub = slProt = slPriv = true;
    TQListViewItem *i = firstChild();
    while ( i ) {
        if ( i->rtti() == HierarchyItem::SlotParent ||
	     i->rtti() == HierarchyItem::FunctParent ) {
	    TQListViewItem *a = i;
	    i = i->firstChild();
	    while ( i ) {
		switch( i->rtti() ) {
		case HierarchyItem::FunctPublic:
		    fuPub = i->isOpen();
		case HierarchyItem::FunctProtected:
		    fuProt = i->isOpen();
		    break;
		case HierarchyItem::FunctPrivate:
		    fuPriv = i->isOpen();
		    break;
		case HierarchyItem::SlotPublic:
		    slPub = i->isOpen();
		    if ( slPub )
		    break;
		case HierarchyItem::SlotProtected:
		    slProt = i->isOpen();
		    break;
		case HierarchyItem::SlotPrivate:
		    slPriv = i->isOpen();
		}
		i = i->nextSibling();
	    }
	    i = a->nextSibling();
	    delete a;
	    continue;
	}
	i = i->nextSibling();
    }


    itemFunct = new HierarchyItem( HierarchyItem::FunctParent,
				   this, 0, tr( "Functions" ), TQString::null, TQString::null );
    itemFunct->moveItem( i );
    itemFunct->setPixmap( 0, TQPixmap::fromMimeSource( "designer_folder.png" ) );
    itemFunctPriv = new HierarchyItem( HierarchyItem::FunctPrivate, itemFunct, 0,
				       tr( "private" ), TQString::null, TQString::null );
    itemFunctProt = new HierarchyItem( HierarchyItem::FunctProtected, itemFunct, 0,
				       tr( "protected" ), TQString::null, TQString::null );
    itemFunctPubl = new HierarchyItem( HierarchyItem::FunctPublic, itemFunct, 0,
				       tr( "public" ), TQString::null, TQString::null );

    itemSlots = new HierarchyItem( HierarchyItem::SlotParent,
				   this, 0, tr( "Slots" ), TQString::null, TQString::null );
    itemSlots->setPixmap( 0, TQPixmap::fromMimeSource( "designer_folder.png" ) );
    itemPrivate = new HierarchyItem( HierarchyItem::SlotPrivate, itemSlots, 0, tr( "private" ),
				     TQString::null, TQString::null );
    itemProtected = new HierarchyItem( HierarchyItem::SlotProtected, itemSlots, 0, tr( "protected" ),
				       TQString::null, TQString::null );
    itemPublic = new HierarchyItem( HierarchyItem::SlotPublic, itemSlots, 0, tr( "public" ),
				    TQString::null, TQString::null );

    TQValueList<MetaDataBase::Function> functionList = MetaDataBase::functionList( formWindow );
    TQValueList<MetaDataBase::Function>::Iterator it = --( functionList.end() );
    if ( !functionList.isEmpty() && itemFunct ) {
	for (;;) {
	    TQListViewItem *item = 0;
	    if ( (*it).type == "slot" ) {
		if ( (*it).access == "protected" )
		    item = new HierarchyItem( HierarchyItem::Slot, itemProtected, 0, (*it).function,
					      TQString::null, TQString::null );
		else if ( (*it).access == "private" )
		    item = new HierarchyItem( HierarchyItem::Slot, itemPrivate, 0, (*it).function,
					      TQString::null, TQString::null );
		else // default is public
		    item = new HierarchyItem( HierarchyItem::Slot, itemPublic, 0, (*it).function,
					      TQString::null, TQString::null );
	    } else {
		if ( (*it).access == "protected" )
		    item = new HierarchyItem( HierarchyItem::Function, itemFunctProt, 0, (*it).function,
					      TQString::null, TQString::null );
		else if ( (*it).access == "private" )
		    item = new HierarchyItem( HierarchyItem::Function, itemFunctPriv, 0, (*it).function,
					      TQString::null, TQString::null );
		else // default is public
		    item = new HierarchyItem( HierarchyItem::Function, itemFunctPubl, 0, (*it).function,
					      TQString::null, TQString::null );
	    }
	    item->setPixmap( 0, TQPixmap::fromMimeSource( "designer_editslots.png" ) );
	    if ( it == functionList.begin() )
		break;
	    --it;
	}
    }

    itemFunct->setOpen( true );
    itemFunctPubl->setOpen( fuPub );
    itemFunctProt->setOpen( fuProt );
    itemFunctPriv->setOpen( fuPriv );

    itemSlots->setOpen( true );
    itemPublic->setOpen( slPub );
    itemProtected->setOpen( slProt );
    itemPrivate->setOpen( slPriv );
}


void FormDefinitionView::setCurrent( TQWidget * )
{
}

void FormDefinitionView::objectClicked( TQListViewItem *i )
{
    if ( !i )
	return;
    if ( (i->rtti() == HierarchyItem::Slot) || (i->rtti() == HierarchyItem::Function) )
	formWindow->mainWindow()->editFunction( i->text( 0 ) );
}

static HierarchyItem::Type getChildType( int type )
{
    switch ( (HierarchyItem::Type)type ) {
    case HierarchyItem::Widget:
	tqWarning( "getChildType: Inserting childs dynamically to Widget or SlotParent is not allowed!" );
	break;
    case HierarchyItem::SlotParent:
    case HierarchyItem::SlotPublic:
    case HierarchyItem::SlotProtected:
    case HierarchyItem::SlotPrivate:
    case HierarchyItem::Slot:
	return HierarchyItem::Slot;
    case HierarchyItem::DefinitionParent:
    case HierarchyItem::Definition:
	return HierarchyItem::Definition;
    case HierarchyItem::Event:
    case HierarchyItem::EventFunction:
	return HierarchyItem::Event;
    case HierarchyItem::FunctParent:
    case HierarchyItem::FunctPublic:
    case HierarchyItem::FunctProtected:
    case HierarchyItem::FunctPrivate:
    case HierarchyItem::Function:
	return HierarchyItem::Function;
    case HierarchyItem::VarParent:
    case HierarchyItem::VarPublic:
    case HierarchyItem::VarProtected:
    case HierarchyItem::VarPrivate:
    case HierarchyItem::Variable:
	return HierarchyItem::Variable;
    }
    return (HierarchyItem::Type)type;
}

void HierarchyList::insertEntry( TQListViewItem *i, const TQPixmap &pix, const TQString &s )
{
    TQListViewItem *after = i->firstChild();
    while ( after && after->nextSibling() )
	after = after->nextSibling();
    HierarchyItem *item = new HierarchyItem( getChildType( i->rtti() ), i, after, s,
					     TQString::null, TQString::null );
    if ( !pix.isNull() )
	item->setPixmap( 0, pix );
    item->setRenameEnabled( 0, true );
    setCurrentItem( item );
    ensureItemVisible( item );
    tqApp->processEvents();
    newItem = item;
    item->startRename( 0 );
}

void FormDefinitionView::contentsMouseDoubleClickEvent( TQMouseEvent *e )
{
    TQListViewItem *i = itemAt( contentsToViewport( e->pos() ) );
    if ( !i )
	return;

    if ( i->rtti() == HierarchyItem::SlotParent || i->rtti() == HierarchyItem::FunctParent ||
	 i->rtti() == HierarchyItem::VarParent )
	return;

    HierarchyItem::Type t = getChildType( i->rtti() );
    if ( (int)t == i->rtti() )
	i = i->parent();

    if ( formWindow->project()->isCpp() )
	switch( i->rtti() ) {
	case HierarchyItem::FunctPublic:
	    execFunctionDialog( "public", "function", true );
	    break;
	case HierarchyItem::FunctProtected:
	    execFunctionDialog( "protected", "function", true );
	    break;
	case HierarchyItem::FunctPrivate:
	    execFunctionDialog( "private", "function", true );
	    break;
	case HierarchyItem::SlotPublic:
	    execFunctionDialog( "public", "slot", true );
	    break;
	case HierarchyItem::SlotProtected:
	    execFunctionDialog( "protected", "slot", true );
	    break;
	case HierarchyItem::SlotPrivate:
	    execFunctionDialog( "private", "slot", true );
	    break;
	case HierarchyItem::VarPublic:
	case HierarchyItem::VarProtected:
	case HierarchyItem::VarPrivate: {
	    VariableDialog varDia( formWindow, this );
	    TQListViewItem *i = selectedItem();
	    if ( i )
		varDia.setCurrentItem( i->text( 0 ) );
	    varDia.exec();
	    break;
	}
	default:
	    insertEntry( i );
    } else
	insertEntry( i );
}

void FormDefinitionView::execFunctionDialog( const TQString &access, const TQString &type, bool addFunc )
{
    FormFile *formFile = formWindow->formFile();
    if ( !formFile || !formFile->isUihFileUpToDate() )
	    return;
	
    // refresh the functions list in the metadatabase
    SourceEditor *editor = formFile->editor();
    if ( editor )
	editor->refresh( true );

    EditFunctions dlg( this, formWindow );
    if ( addFunc )
	dlg.functionAdd( access, type );
    dlg.exec();
}

void FormDefinitionView::showRMBMenu( TQListViewItem *i, const TQPoint &pos )
{
    if ( !i )
	return;

    const int EDIT = 1;
    const int NEW = 2;
    const int DEL = 3;
    const int PROPS = 4;
    const int GOIMPL = 5;

    TQPopupMenu menu;
    bool insertDelete = false;

    if ( i->rtti() == HierarchyItem::FunctParent || i->rtti() == HierarchyItem::SlotParent ||
	 i->rtti() == HierarchyItem::VarParent ) {
	menu.insertItem( TQPixmap::fromMimeSource( "designer_editslots.png" ), tr( "Edit..." ), EDIT );
    } else
	menu.insertItem( TQPixmap::fromMimeSource( "designer_filenew.png" ), tr( "New" ), NEW );
    if ( i->rtti() == HierarchyItem::DefinitionParent || i->rtti() == HierarchyItem::Variable ||
	 i->rtti() == HierarchyItem::Definition ) {
	menu.insertItem( TQPixmap::fromMimeSource( "designer_editslots.png" ), tr( "Edit..." ), EDIT );
    }
    if ( i->rtti() == HierarchyItem::Function || i->rtti() == HierarchyItem::Slot ) {
	if ( formWindow->project()->isCpp() )
	    menu.insertItem( TQPixmap::fromMimeSource( "designer_editslots.png" ), tr( "Properties..." ), PROPS );
	if ( MetaDataBase::hasEditor( formWindow->project()->language() ) )
	    menu.insertItem( tr( "Goto Implementation" ), GOIMPL );
	insertDelete = true;
    }
    if ( insertDelete || i->rtti() == HierarchyItem::Variable ||
	 i->rtti() == HierarchyItem::Function || i->rtti() == HierarchyItem::Slot ||
	 i->rtti() == HierarchyItem::Definition ) {
	menu.insertSeparator();
	menu.insertItem( TQPixmap::fromMimeSource( "designer_editcut.png" ), tr( "Delete..." ), DEL );
    }
    popupOpen = true;
    int res = menu.exec( pos );
    popupOpen = false;
    if ( res == -1 )
	return;

    if ( res == EDIT ) {
	switch( i->rtti() ) {
	case HierarchyItem::FunctParent:
	    execFunctionDialog( "public", "function", false );
	    break;
	case HierarchyItem::SlotParent:
	    execFunctionDialog( "public", "slot", false );
	    break;
	case HierarchyItem::VarParent:
	case HierarchyItem::VarPublic:
	case HierarchyItem::VarProtected:
	case HierarchyItem::VarPrivate:
	case HierarchyItem::Variable: {
	    VariableDialog varDia( formWindow, this );
	    TQListViewItem *i = selectedItem();
	    if ( i )
		varDia.setCurrentItem( i->text( 0 ) );
	    if ( varDia.exec() == TQDialog::Accepted )
		formWindow->commandHistory()->setModified( true );
	    break;
	}
	case HierarchyItem::Definition:
	case HierarchyItem::DefinitionParent:
	    LanguageInterface *lIface = MetaDataBase::languageInterface( formWindow->project()->language() );
	    if ( !lIface )
		return;
	    if ( i->rtti() == HierarchyItem::Definition )
		i = i->parent();
	    ListEditor dia( this, 0, true );
	    dia.setCaption( tr( "Edit %1" ).arg( i->text( 0 ) ) );
	    TQStringList entries = lIface->definitionEntries( i->text( 0 ), MainWindow::self->designerInterface() );
	    dia.setList( entries );
	    dia.exec();
	    Command *cmd = new EditDefinitionsCommand( tr( "Edit " ) + i->text( 0 ), formWindow,
						       lIface, i->text( 0 ), dia.items() );
	    formWindow->commandHistory()->addCommand( cmd );
	    cmd->execute();
	}
    } else if ( res == NEW ) {
	HierarchyItem::Type t = getChildType( i->rtti() );
	if ( (int)t == i->rtti() )
	    i = i->parent();
	switch( i->rtti() ) {
	case HierarchyItem::SlotPublic:
	    execFunctionDialog( "public", "slot", true );
	    break;
	case HierarchyItem::SlotProtected:
	    execFunctionDialog( "protected", "slot", true );
	    break;
	case HierarchyItem::SlotPrivate:
	    execFunctionDialog( "private" , "slot", true );
	    break;
	case HierarchyItem::FunctPublic:
	    execFunctionDialog( "public", "function", true );
	    break;
	case HierarchyItem::FunctProtected:
	    execFunctionDialog( "protected", "function", true );
	    break;
	case HierarchyItem::FunctPrivate:
	    execFunctionDialog( "private" , "function", true );
	    break;
    	default:
	    insertEntry( i );
	}
    } else if ( res == DEL ) {
	if ( i->rtti() == HierarchyItem::Slot || i->rtti() == HierarchyItem::Function ) {

	    TQCString funct( MetaDataBase::normalizeFunction( i->text( 0 ) ).latin1() );
	    Command *cmd = new RemoveFunctionCommand( tr( "Remove function" ), formWindow, funct,
						     TQString::null, TQString::null, TQString::null,
						     TQString::null, formWindow->project()->language() );
	    formWindow->commandHistory()->addCommand( cmd );
	    cmd->execute();
	    formWindow->mainWindow()->objectHierarchy()->updateFormDefinitionView();
	} else if ( i->rtti() == HierarchyItem::Variable ) {
	    Command *cmd = new RemoveVariableCommand( tr( "Remove variable" ), formWindow,
						      i->text( 0 ) );
	    formWindow->commandHistory()->addCommand( cmd );
	    cmd->execute();
	} else {
	    TQListViewItem *p = i->parent();
	    delete i;
	    save( p, 0 );
	}
    } else if ( res == PROPS ) {
	if ( i->rtti() == HierarchyItem::Slot ||
	     i->rtti() == HierarchyItem::Function ) {
	    EditFunctions dlg( this, formWindow );
	    dlg.setCurrentFunction( MetaDataBase::normalizeFunction( i->text( 0 ) ) );
	    dlg.exec();
	}
    } else if ( res == GOIMPL ) {
	if ( i->rtti() == HierarchyItem::Slot ||
	     i->rtti() == HierarchyItem::Function ) {
	    formWindow->mainWindow()->editFunction( i->text( 0 ) );
	}
    }
}

void FormDefinitionView::renamed( TQListViewItem *i )
{
    if ( newItem == i )
	newItem = 0;
    if ( !i->parent() )
	return;
    save( i->parent(), i );
}


void FormDefinitionView::save( TQListViewItem *p, TQListViewItem *i )
{
    if ( i && i->text( 0 ).isEmpty() ) {
	delete i;
	return;
    }

    if ( i && i->rtti() == HierarchyItem::Variable ) {
	i->setRenameEnabled( 0, false );
	TQString varName = i->text( 0 );
	varName = varName.simplifyWhiteSpace();
	if ( varName[(int)varName.length() - 1] != ';' )
	    varName += ";";
	if ( MetaDataBase::hasVariable( formWindow, varName ) ) {
	    TQMessageBox::information( this, tr( "Edit Variables" ),
				      tr( "This variable has already been declared!" ) );
	} else {
	    if ( p->rtti() == HierarchyItem::VarPublic )
		addVariable( varName, "public" );
	    else if ( p->rtti() == HierarchyItem::VarProtected )
		addVariable( varName, "protected" );
	    else if ( p->rtti() == HierarchyItem::VarPrivate )
		addVariable( varName, "private" );
	}
    } else {
	LanguageInterface *lIface = MetaDataBase::languageInterface( formWindow->project()->language() );
	if ( !lIface )
	    return;
	TQStringList lst;
	i = p->firstChild();
	while ( i ) {
	    lst << i->text( 0 );
	    i = i->nextSibling();
	}
	Command *cmd = new EditDefinitionsCommand( tr( "Edit " ) + p->text( 0 ), formWindow,
						   lIface, p->text( 0 ), lst );
	formWindow->commandHistory()->addCommand( cmd );
	cmd->execute();
    }
}

void FormDefinitionView::addVariable( const TQString &varName, const TQString &access )
{
    Command *cmd = new AddVariableCommand( tr( "Add variable" ), formWindow,
					      varName, access );
    formWindow->commandHistory()->addCommand( cmd );
    cmd->execute();
}

// ------------------------------------------------------------

HierarchyView::HierarchyView( TQWidget *parent )
    : TQTabWidget( parent, 0, WStyle_Customize | WStyle_NormalBorder | WStyle_Title |
		  WStyle_Tool |WStyle_MinMax | WStyle_SysMenu )
{
    formwindow = 0;
    editor = 0;
    listview = new HierarchyList( this, formWindow() );
    fView = new FormDefinitionView( this, formWindow() );
    if ( !MainWindow::self->singleProjectMode() ) {
	addTab( listview, tr( "Objects" ) );
	setTabToolTip( listview, tr( "List of all widgets and objects of the current form in hierarchical order" ) );
	addTab( fView, tr( "Members" ) );
	setTabToolTip( fView, tr( "List of all members of the current form" ) );
    } else {
	listview->hide();
	fView->hide();
    }

    if ( !classBrowserInterfaceManager ) {
	classBrowserInterfaceManager =
	    new TQPluginManager<ClassBrowserInterface>( IID_ClassBrowser, TQApplication::libraryPaths(),
						       MainWindow::self->pluginDirectory() );
    }

    classBrowsers = new TQMap<TQString, ClassBrowser>();
    TQStringList langs = MetaDataBase::languages();
    for ( TQStringList::Iterator it = langs.begin(); it != langs.end(); ++it ) {
	TQInterfacePtr<ClassBrowserInterface> ciface = 0;
	classBrowserInterfaceManager->queryInterface( *it, &ciface );
	if ( ciface ) {
	    ClassBrowser cb( ciface->createClassBrowser( this ), ciface );
	    addTab( cb.lv, tr( "Class Declarations" ) );
	    setTabToolTip( cb.lv, tr( "List of all classes and its declarations of the current source file" ) );
	    ciface->onClick( this, TQ_SLOT( jumpTo( const TQString &, const TQString &, int ) ) );
	    classBrowsers->insert( *it, cb );
	    setTabEnabled( cb.lv, false );
	}
    }
}

HierarchyView::~HierarchyView()
{
}

void HierarchyView::clear()
{
    listview->clear();
    fView->clear();
    for ( TQMap<TQString, ClassBrowser>::Iterator it = classBrowsers->begin();
	  it != classBrowsers->end(); ++it ) {
	(*it).iface->clear();
    }
}

void HierarchyView::setFormWindow( FormWindow *fw, TQObject *o )
{
    bool fake = fw && qstrcmp( fw->name(), "qt_fakewindow" ) == 0;
    if ( fw == 0 || o == 0 ) {
	listview->clear();
	fView->clear();
	listview->setFormWindow( fw );
	fView->setFormWindow( fw );
	formwindow = 0;
	editor = 0;
    }

    setTabEnabled( listview, true );
    setTabEnabled( fView, fw && fw->project()->isCpp() );

    if ( fw == formwindow ) {
	if ( fw ) {
	    if ( !fake )
		listview->setCurrent( (TQWidget*)o );
	    else
		listview->clear();
	    if ( MainWindow::self->qWorkspace()->activeWindow() == fw )
		showPage( listview );
	    else if ( fw->project()->isCpp() )
		showPage( fView );
	    else
		showClasses( fw->formFile()->editor() );
	}
    }

    formwindow = fw;
    if ( !fake ) {
	listview->setFormWindow( fw );
    } else {
	listview->setFormWindow( 0 );
	listview->clear();
    }

    fView->setFormWindow( fw );
    if ( !fake ) {
	listview->setup();
	listview->setCurrent( (TQWidget*)o );
    }
    fView->setup();

    for ( TQMap<TQString, ClassBrowser>::Iterator it = classBrowsers->begin();
	  it != classBrowsers->end(); ++it ) {
	(*it).iface->clear();
	setTabEnabled( (*it).lv, fw && !fw->project()->isCpp() );
    }

    if ( MainWindow::self->qWorkspace()->activeWindow() == fw )
	showPage( listview );
    else if ( fw && fw->project()->isCpp() )
	showPage( fView );
    else if ( fw )
	showClasses( fw->formFile()->editor() );

    editor = 0;
}

void HierarchyView::showClasses( SourceEditor *se )
{
    if ( !se->object() )
	return;

    lastSourceEditor = se;
    TQTimer::singleShot( 100, this, TQ_SLOT( showClassesTimeout() ) );
}

void HierarchyView::showClassesTimeout()
{
    if ( !lastSourceEditor )
	return;
    SourceEditor *se = (SourceEditor*)lastSourceEditor;
    if ( !se->object() )
	return;
    if ( se->formWindow() && se->formWindow()->project()->isCpp() ) {
	setFormWindow( se->formWindow(), se->formWindow()->currentWidget() );
	MainWindow::self->propertyeditor()->setWidget( se->formWindow()->currentWidget(),
						       se->formWindow() );
	return;
    }

    setTabEnabled( listview, !!se->formWindow() && !se->formWindow()->isFake() );
    setTabEnabled( fView, false );

    formwindow = 0;
    listview->setFormWindow( 0 );
    fView->setFormWindow( 0 );
    listview->clear();
    fView->clear();
    if ( !se->formWindow() )
	MainWindow::self->propertyeditor()->setWidget( 0, 0 );
    editor = se;

    for ( TQMap<TQString, ClassBrowser>::Iterator it = classBrowsers->begin();
	  it != classBrowsers->end(); ++it ) {
	if ( it.key() == se->project()->language() ) {
	    (*it).iface->update( se->text() );
	    setTabEnabled( (*it).lv, true );
	    showPage( (*it).lv );
	} else {
	    setTabEnabled( (*it).lv, false );
	    (*it).iface->clear();
	}
    }
}

void HierarchyView::updateClassBrowsers()
{
    if ( !editor )
	return;
    for ( TQMap<TQString, ClassBrowser>::Iterator it = classBrowsers->begin();
	  it != classBrowsers->end(); ++it ) {
	if ( it.key() == editor->project()->language() )
	    (*it).iface->update( editor->text() );
	else
	    (*it).iface->clear();
    }
}

FormWindow *HierarchyView::formWindow() const
{
    return formwindow;
}

void HierarchyView::closeEvent( TQCloseEvent *e )
{
    emit hidden();
    e->accept();
}

void HierarchyView::widgetInserted( TQWidget * )
{
    listview->setup();
}

void HierarchyView::widgetRemoved( TQWidget * )
{
    listview->setup();
}

void HierarchyView::widgetsInserted( const TQWidgetList & )
{
    listview->setup();
}

void HierarchyView::widgetsRemoved( const TQWidgetList & )
{
    listview->setup();
}

void HierarchyView::namePropertyChanged( TQWidget *w, const TQVariant & )
{
    TQWidget *w2 = w;
    if ( ::tqt_cast<TQMainWindow*>(w) )
	w2 = ( (TQMainWindow*)w )->centralWidget();
    listview->changeNameOf( w2, w->name() );
}


void HierarchyView::databasePropertyChanged( TQWidget *w, const TQStringList& info )
{
#ifndef TQT_NO_SQL
    TQString i = info.join( "." );
    listview->changeDatabaseOf( w, i );
#endif
}


void HierarchyView::tabsChanged( TQTabWidget * )
{
    listview->setup();
}

void HierarchyView::pagesChanged( TQWizard * )
{
    listview->setup();
}

void HierarchyView::rebuild()
{
    listview->setup();
}

void HierarchyView::closed( FormWindow *fw )
{
    if ( fw == formwindow ) {
	listview->clear();
	fView->clear();
    }
}

void HierarchyView::updateFormDefinitionView()
{
    fView->setup();
}

void HierarchyView::jumpTo( const TQString &func, const TQString &clss, int type )
{
    if ( !editor )
	return;
    if ( type == ClassBrowserInterface::Class )
	editor->setClass( func );
    else
	editor->setFunction( func, clss );
}

HierarchyView::ClassBrowser::ClassBrowser( TQListView *l, ClassBrowserInterface *i )
    : lv( l ), iface( i )
{
}

HierarchyView::ClassBrowser::~ClassBrowser()
{
}
