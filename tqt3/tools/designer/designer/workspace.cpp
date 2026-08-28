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

#include <ntqvariant.h>
#include "workspace.h"
#include "formwindow.h"
#include "mainwindow.h"
#include "globaldefs.h"
#include "command.h"
#include "project.h"
#include "pixmapcollection.h"
#include "sourcefile.h"
#include "sourceeditor.h"
#include "propertyeditor.h"

#include <ntqheader.h>
#include <ntqdragobject.h>
#include <ntqfileinfo.h>
#include <ntqapplication.h>
#include <ntqpainter.h>
#include <ntqpen.h>
#include <ntqobjectlist.h>
#include <ntqworkspace.h>
#include <ntqpopupmenu.h>
#include <ntqtextstream.h>
#include "qcompletionedit.h"

WorkspaceItem::WorkspaceItem( TQListView *parent, Project* p )
    : TQListViewItem( parent )
{
    init();
    project = p;
    t = ProjectType;
    setPixmap( 0, TQPixmap::fromMimeSource( "designer_folder.png" ) );
    setExpandable( false );
}

WorkspaceItem::WorkspaceItem( TQListViewItem *parent, SourceFile* sf )
    : TQListViewItem( parent )
{
    init();
    sourceFile = sf;
    t = SourceFileType;
    setPixmap( 0, TQPixmap::fromMimeSource( "designer_filenew.png" ) );
}

WorkspaceItem::WorkspaceItem( TQListViewItem *parent, TQObject *o, Project *p )
    : TQListViewItem( parent )
{
    init();
    object = o;
    project = p;
    t = ObjectType;
    setPixmap( 0, TQPixmap::fromMimeSource( "designer_object.png" ) );
    TQObject::connect( p->fakeFormFileFor( o ), TQ_SIGNAL( somethingChanged(FormFile*) ),
		      listView(), TQ_SLOT( update() ) );
}

WorkspaceItem::WorkspaceItem( TQListViewItem *parent, FormFile* ff, Type type )
    : TQListViewItem( parent )
{
    init();
    formFile = ff;
    t = type;
    if ( type ==  FormFileType ) {
	setPixmap( 0, TQPixmap::fromMimeSource( "designer_form.png" ) );
	TQObject::connect( ff, TQ_SIGNAL( somethingChanged(FormFile*) ), listView(), TQ_SLOT( update(FormFile*) ) );
	if ( formFile->supportsCodeFile() ) {
	    (void) new WorkspaceItem( this, formFile, FormSourceType );
	}
    } else if ( type == FormSourceType ) {
	setPixmap( 0, TQPixmap::fromMimeSource( "designer_filenew.png" ) );
    }
}


void WorkspaceItem::init()
{
    autoOpen = false;
    useOddColor = false;
    project = 0;
    sourceFile = 0;
    formFile = 0;
}

void WorkspaceItem::paintCell( TQPainter *p, const TQColorGroup &cg, int column, int width, int align )
{
    TQColorGroup g( cg );
    g.setColor( TQColorGroup::Base, backgroundColor() );
    g.setColor( TQColorGroup::Foreground, TQt::black );

    if ( type() == FormSourceType &&
	 ( !formFile->hasFormCode() || ( formFile->codeFileState() == FormFile::Deleted && formFile->formWindow() ) ) &&
	 parent() && parent()->parent() && ( (WorkspaceItem*)parent()->parent() )->project &&
	 ( (WorkspaceItem*)parent()->parent() )->project->isCpp() ) {
	g.setColor( TQColorGroup::Text, listView()->palette().disabled().color( TQColorGroup::Text) );
	g.setColor( TQColorGroup::HighlightedText, listView()->palette().disabled().color( TQColorGroup::Text) );
    } else {
	g.setColor( TQColorGroup::Text, TQt::black );
    }
    p->save();

    if ( isModified() ) {
	TQFont f = p->font();
	f.setBold( true );
	p->setFont( f );
    }

    TQListViewItem::paintCell( p, g, column, width, align );
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

TQString WorkspaceItem::text( int column ) const
{
    if ( column != 0 )
	return TQListViewItem::text( column );
    switch( t ) {
    case ProjectType:
	if ( project->isDummy() ) {
	    return Project::tr("<No Project>" );
	} else if ( MainWindow::self->singleProjectMode() ) {
	    return TQFileInfo( project->fileName() ).baseName();
	}
	return project->makeRelative( project->fileName() );
    case FormFileType:
	if ( !MainWindow::self->singleProjectMode() )
	    return formFile->formName() + ": " + formFile->fileName();
	return formFile->formName();
    case FormSourceType:
	if ( !MainWindow::self->singleProjectMode() )
	    return formFile->codeFile();
	return formFile->formName() + " [Source]";
    case SourceFileType:
	return sourceFile->fileName();
    case ObjectType:
	if ( !project->hasParentObject( object ) )
	    return object->name();
	return project->qualifiedName( object );
    }

    return TQString::null; // shut up compiler
}

void WorkspaceItem::fillCompletionList( TQStringList& completion )
{
    switch( t ) {
    case ProjectType:
	break;
    case FormFileType:
	completion += formFile->formName();
	completion += formFile->fileName();
	break;
    case FormSourceType:
	completion += formFile->codeFile();
	break;
    case SourceFileType:
	completion += sourceFile->fileName();
	break;
    case ObjectType:
	completion += object->name();
    }
}

bool WorkspaceItem::checkCompletion( const TQString& completion )
{
    switch( t ) {
    case ProjectType:
	break;
    case FormFileType:
	return  completion == formFile->formName()
		 || completion == formFile->fileName();
    case FormSourceType:
	return completion == formFile->codeFile();
    case SourceFileType:
	return completion == sourceFile->fileName();
    case ObjectType:
	return completion == object->name();
    }
    return false;
}


bool WorkspaceItem::isModified() const
{
    switch( t ) {
    case ProjectType:
	return project->isModified();
    case FormFileType:
	return formFile->isModified( FormFile::WFormWindow );
    case FormSourceType:
	return formFile->isModified( FormFile::WFormCode );
    case SourceFileType:
	return sourceFile->isModified();
    case ObjectType:
	return project->fakeFormFileFor( object )->isModified();
	break;
    }
    return false; // shut up compiler
}

TQString WorkspaceItem::key( int column, bool ) const
{
    TQString key = text( column );
    if ( t == FormFileType )
	key.prepend( "0" );
    else if ( t == ObjectType )
	key.prepend( "a" );
    else
	key.prepend( "A" );
    return key;
}

TQColor WorkspaceItem::backgroundColor()
{
    bool b = useOddColor;
    if ( t == FormSourceType && parent() )
	b = ( ( WorkspaceItem*)parent() )->useOddColor;
    return b ? *backColor2 : *backColor1;
}


void WorkspaceItem::setOpen( bool b )
{
    TQListViewItem::setOpen( b );
    autoOpen = false;
}

void WorkspaceItem::setAutoOpen( bool b )
{
    TQListViewItem::setOpen( b );
    autoOpen = b;
}

Workspace::Workspace( TQWidget *parent, MainWindow *mw )
    : TQListView( parent, 0, WStyle_Customize | WStyle_NormalBorder | WStyle_Title |
		 WStyle_Tool | WStyle_MinMax | WStyle_SysMenu ), mainWindow( mw ),
	project( 0 ), completionDirty( false )
{
    init_colors();

    setDefaultRenameAction( Accept );
    blockNewForms = false;
    bufferEdit = 0;
    header()->setStretchEnabled( true );
    header()->hide();
    setSorting( 0 );
    setResizePolicy( TQScrollView::Manual );
#ifndef TQ_WS_MAC
    TQPalette p( palette() );
    p.setColor( TQColorGroup::Base, TQColor( *backColor2 ) );
    (void)*selectedBack; // hack
    setPalette( p );
#endif
    addColumn( tr( "Files" ) );
    setAllColumnsShowFocus( true );
    connect( this, TQ_SIGNAL( mouseButtonClicked( int, TQListViewItem *, const TQPoint &, int ) ),
	     this, TQ_SLOT( itemClicked( int, TQListViewItem *, const TQPoint& ) ) ),
    connect( this, TQ_SIGNAL( doubleClicked( TQListViewItem * ) ),
	     this, TQ_SLOT( itemDoubleClicked( TQListViewItem * ) ) ),
    connect( this, TQ_SIGNAL( contextMenuRequested( TQListViewItem *, const TQPoint &, int ) ),
	     this, TQ_SLOT( rmbClicked( TQListViewItem *, const TQPoint& ) ) ),
    setHScrollBarMode( AlwaysOff );
    setVScrollBarMode( AlwaysOn );
    viewport()->setAcceptDrops( true );
    setAcceptDrops( true );
    setColumnWidthMode( 1, Manual );
}


void Workspace::projectDestroyed( TQObject* o )
{
    if ( o == project ) {
	project = 0;
	clear();
    }
}

void Workspace::setCurrentProject( Project *pro )
{
    if ( project == pro )
	return;
    if ( project ) {
	disconnect( project, TQ_SIGNAL( sourceFileAdded(SourceFile*) ), this, TQ_SLOT( sourceFileAdded(SourceFile*) ) );
	disconnect( project, TQ_SIGNAL( sourceFileRemoved(SourceFile*) ), this, TQ_SLOT( sourceFileRemoved(SourceFile*) ) );
	disconnect( project, TQ_SIGNAL( formFileAdded(FormFile*) ), this, TQ_SLOT( formFileAdded(FormFile*) ) );
	disconnect( project, TQ_SIGNAL( formFileRemoved(FormFile*) ), this, TQ_SLOT( formFileRemoved(FormFile*) ) );
	disconnect( project, TQ_SIGNAL( objectAdded(TQObject*) ), this, TQ_SLOT( objectAdded(TQObject*) ) );
	disconnect( project, TQ_SIGNAL( objectRemoved(TQObject*) ), this, TQ_SLOT( objectRemoved(TQObject*) ) );
	disconnect( project, TQ_SIGNAL( projectModified() ), this, TQ_SLOT( update() ) );
    }
    project = pro;
    connect( project, TQ_SIGNAL( sourceFileAdded(SourceFile*) ), this, TQ_SLOT( sourceFileAdded(SourceFile*) ) );
    connect( project, TQ_SIGNAL( sourceFileRemoved(SourceFile*) ), this, TQ_SLOT( sourceFileRemoved(SourceFile*) ) );
    connect( project, TQ_SIGNAL( formFileAdded(FormFile*) ), this, TQ_SLOT( formFileAdded(FormFile*) ) );
    connect( project, TQ_SIGNAL( formFileRemoved(FormFile*) ), this, TQ_SLOT( formFileRemoved(FormFile*) ) );
    connect( project, TQ_SIGNAL( destroyed(TQObject*) ), this, TQ_SLOT( projectDestroyed(TQObject*) ) );
    connect( project, TQ_SIGNAL( objectAdded(TQObject*) ), this, TQ_SLOT( objectAdded(TQObject*) ) );
    connect( project, TQ_SIGNAL( objectRemoved(TQObject*) ), this, TQ_SLOT( objectRemoved(TQObject*) ) );
    connect( project, TQ_SIGNAL( projectModified() ), this, TQ_SLOT( update() ) );
    clear();

    if ( bufferEdit )
	bufferEdit->clear();

    projectItem = new WorkspaceItem( this, project );

    projectItem->setOpen( true );

    for ( TQPtrListIterator<SourceFile> sources = project->sourceFiles();
	  sources.current(); ++sources ) {
	SourceFile* f = sources.current();
	(void) new WorkspaceItem( projectItem, f );
    }

    for ( TQPtrListIterator<FormFile> forms = project->formFiles();
	  forms.current(); ++forms ) {
	FormFile* f = forms.current();
	if ( f->isFake() )
	    continue;

	(void) new WorkspaceItem( projectItem, f );
    }

    TQObjectList l = project->objects();
    TQObjectListIt objs( l );
    for ( ;objs.current(); ++objs ) {
	TQObject* o = objs.current();
	(void) new WorkspaceItem( projectItem, o, project );
    }

    updateColors();
    completionDirty = true;
}

void Workspace::sourceFileAdded( SourceFile* sf )
{
    (void) new WorkspaceItem( projectItem, sf );
    updateColors();
}

void Workspace::sourceFileRemoved( SourceFile* sf )
{
    delete findItem( sf );
    updateColors();
}

void Workspace::formFileAdded( FormFile* ff )
{
    if ( ff->isFake() )
	return;
    (void) new WorkspaceItem( projectItem, ff );
    updateColors();
}

void Workspace::formFileRemoved( FormFile* ff )
{
    delete findItem( ff );
    updateColors();
}

void Workspace::objectAdded( TQObject *o )
{
    (void) new WorkspaceItem( projectItem, o, project );
    updateColors();
}

void Workspace::objectRemoved( TQObject *o )
{
    delete findItem( o );
    updateColors();
}

void Workspace::update()
{
    completionDirty = true;
    triggerUpdate();
}

void Workspace::update( FormFile* ff )
{
    TQListViewItem* i = findItem( ff );
    if ( i ) {
	i->repaint();
	if ( (i = i->firstChild()) )
	    i->repaint();
    }
}


void Workspace::activeFormChanged( FormWindow *fw )
{
    WorkspaceItem *i = findItem( fw->formFile() );
    if ( i ) {
	setCurrentItem( i );
	setSelected( i, true );
	if ( !i->isOpen() )
	    i->setAutoOpen( true );
    }

    closeAutoOpenItems();

}

void Workspace::activeEditorChanged( SourceEditor *se )
{
    if ( !se->object() )
	return;

    if ( se->formWindow() ) {
	WorkspaceItem *i = findItem( se->formWindow()->formFile() );
	if ( i && i->firstChild() ) {
	    if ( !i->isOpen() )
		i->setAutoOpen( true );
	    setCurrentItem( i->firstChild() );
	    setSelected( i->firstChild(), true );
	}
    } else {
	WorkspaceItem *i = findItem( se->sourceFile() );
	if ( i ) {
	    setCurrentItem( i );
	    setSelected( i, true );
	}
    }

    closeAutoOpenItems();
}

WorkspaceItem *Workspace::findItem( FormFile* ff)
{
    TQListViewItemIterator it( this );
    for ( ; it.current(); ++it ) {
	if ( ( (WorkspaceItem*)it.current() )->formFile == ff )
	    return (WorkspaceItem*)it.current();
    }
    return 0;
}

WorkspaceItem *Workspace::findItem( SourceFile *sf )
{
    TQListViewItemIterator it( this );
    for ( ; it.current(); ++it ) {
	if ( ( (WorkspaceItem*)it.current() )->sourceFile == sf )
	    return (WorkspaceItem*)it.current();
    }
    return 0;
}

WorkspaceItem *Workspace::findItem( TQObject *o )
{
    TQListViewItemIterator it( this );
    for ( ; it.current(); ++it ) {
	if ( ( (WorkspaceItem*)it.current() )->object == o )
	    return (WorkspaceItem*)it.current();
    }
    return 0;
}

void Workspace::closeAutoOpenItems()
{
    TQListViewItemIterator it( this );
    for ( ; it.current(); ++it ) {
	WorkspaceItem* i = (WorkspaceItem*) it.current();
	WorkspaceItem* ip = (WorkspaceItem*) i->parent();
	if ( i->type() == WorkspaceItem::FormSourceType ) {
	    if ( !i->isSelected() && !ip->isSelected()
		 && ip->isAutoOpen() ) {
		ip->setAutoOpen( false );
	    }
	}
    }
}


void Workspace::closeEvent( TQCloseEvent *e )
{
    e->accept();
}

void Workspace::itemDoubleClicked( TQListViewItem *i )
{
    if ( ( (WorkspaceItem*)i)->type()== WorkspaceItem::ProjectType )
	i->setOpen( true );
}

void Workspace::itemClicked( int button, TQListViewItem *i, const TQPoint& )
{
    if ( !i || button != LeftButton )
	return;

    closeAutoOpenItems();

    WorkspaceItem* wi = (WorkspaceItem*)i;
    switch( wi->type() ) {
    case WorkspaceItem::ProjectType:
	break; // ### TODO
    case WorkspaceItem::FormFileType:
	wi->formFile->showFormWindow();
	break;
    case WorkspaceItem::FormSourceType:
	wi->formFile->showEditor( false );
	break;
    case WorkspaceItem::SourceFileType:
	mainWindow->editSource( wi->sourceFile );
	break;
    case WorkspaceItem::ObjectType:
	project->fakeFormFileFor( wi->object )->formWindow()->setFocus();
	mainWindow->propertyeditor()->setWidget( wi->object,
				 project->fakeFormFileFor( wi->object )->formWindow() );
	mainWindow->objectHierarchy()->
	    setFormWindow( project->fakeFormFileFor( wi->object )->formWindow(), wi->object );
	project->fakeFormFileFor( wi->object )->showEditor();
	break;
    }
}

void Workspace::contentsDropEvent( TQDropEvent *e )
{
    if ( !TQUriDrag::canDecode( e ) ) {
	e->ignore();
    } else {
	TQStringList files;
	TQUriDrag::decodeLocalFiles( e, files );
	if ( !files.isEmpty() ) {
	    for ( TQStringList::Iterator it = files.begin(); it != files.end(); ++it ) {
		TQString fn = *it;
		mainWindow->fileOpen( "", "", fn );
	    }
	}
    }
}

void Workspace::contentsDragEnterEvent( TQDragEnterEvent *e )
{
    if ( !TQUriDrag::canDecode( e ) )
	e->ignore();
    else
	e->accept();
}

void Workspace::contentsDragMoveEvent( TQDragMoveEvent *e )
{
    if ( !TQUriDrag::canDecode( e ) )
	e->ignore();
    else
	e->accept();
}

void Workspace::rmbClicked( TQListViewItem *i, const TQPoint& pos )
{
    if ( !i )
	return;
    WorkspaceItem* wi = (WorkspaceItem*)i;
    enum { OPEN_SOURCE, REMOVE_SOURCE, OPEN_FORM, REMOVE_FORM,
	   OPEN_FORM_SOURCE, REMOVE_FORM_SOURCE, OPEN_OBJECT_SOURCE };
    TQPopupMenu menu( this );
    menu.setCheckable( true );
    switch ( wi->type() ) {
    case WorkspaceItem::SourceFileType:
	menu.insertItem( tr( "&Open source file" ), OPEN_SOURCE );
	menu.insertSeparator();
	menu.insertItem( TQPixmap::fromMimeSource( "designer_editcut.png" ),
			 tr( "&Remove source file from project" ), REMOVE_SOURCE );
	break;
    case WorkspaceItem::FormFileType:
	menu.insertItem( tr( "&Open form" ), OPEN_FORM );
	menu.insertSeparator();
	menu.insertItem( TQPixmap::fromMimeSource( "designer_editcut.png" ),
			 tr( "&Remove form from project" ), REMOVE_FORM );
	break;
    case WorkspaceItem::FormSourceType:
	menu.insertItem( tr( "&Open form source" ), OPEN_FORM_SOURCE );
	menu.insertSeparator();
	if ( project->isCpp() )
	    menu.insertItem( TQPixmap::fromMimeSource( "designer_editcut.png" ),
			     tr( "&Remove source file from form" ), REMOVE_FORM_SOURCE );
	else
	    menu.insertItem( TQPixmap::fromMimeSource( "designer_editcut.png" ),
	                     tr( "&Remove form from project" ), REMOVE_FORM );
	break;
    case WorkspaceItem::ProjectType:
	MainWindow::self->popupProjectMenu( pos );
	return;
    case WorkspaceItem::ObjectType:
	menu.insertItem( tr( "&Open source" ), OPEN_OBJECT_SOURCE );
	break;
    }

    switch ( menu.exec( pos ) ) {
    case REMOVE_SOURCE:
	project->removeSourceFile( wi->sourceFile );
	break;
    case REMOVE_FORM:
	project->removeFormFile( wi->formFile );
	break;
    case REMOVE_FORM_SOURCE:
	( (WorkspaceItem*)i )->formFile->setModified( true );
	( (WorkspaceItem*)i )->formFile->setCodeFileState( FormFile::Deleted );
	delete ( (WorkspaceItem*)i )->formFile->editor();
	break;
    case OPEN_OBJECT_SOURCE:
    case OPEN_SOURCE:
    case OPEN_FORM:
    case OPEN_FORM_SOURCE:
	itemClicked( LeftButton, i, pos );
	break;
    }
}

bool Workspace::eventFilter( TQObject *o, TQEvent * e )
{
    // Reggie, on what type of events do we have to execute updateBufferEdit()
    if ( o ==bufferEdit && e->type() != TQEvent::ChildRemoved )
	updateBufferEdit();
    return TQListView::eventFilter( o, e );
}

void Workspace::setBufferEdit( TQCompletionEdit *edit )
{
    bufferEdit = edit;
    connect( bufferEdit, TQ_SIGNAL( chosen( const TQString & ) ),
	     this, TQ_SLOT( bufferChosen( const TQString & ) ) );
    bufferEdit->installEventFilter( this );
}

void Workspace::updateBufferEdit()
{
    if ( !bufferEdit || !completionDirty || !MainWindow::self)
	return;
    completionDirty = false;
    TQStringList completion = MainWindow::self->projectFileNames();
    TQListViewItemIterator it( this );
    while ( it.current() ) {
	( (WorkspaceItem*)it.current())->fillCompletionList( completion );
	++it;
    }
    completion.sort();
    bufferEdit->setCompletionList( completion );
}

void Workspace::bufferChosen( const TQString &buffer )
{
    if ( bufferEdit )
	bufferEdit->setText( "" );

    if ( MainWindow::self->projectFileNames().contains( buffer ) ) {
	MainWindow::self->setCurrentProjectByFilename( buffer );
	return;
    }

    TQListViewItemIterator it( this );
    while ( it.current() ) {
	if ( ( (WorkspaceItem*)it.current())->checkCompletion( buffer ) ) {
	    itemClicked( LeftButton, it.current(), TQPoint() );
	    break;
	}
	++it;
    }
}

void Workspace::updateColors()
{
    TQListViewItem* i = firstChild();
    if ( i )
	i = i->firstChild();
    bool b = true;
    while ( i ) {
	WorkspaceItem* wi = ( WorkspaceItem*) i;
	i = i->nextSibling();
	wi->useOddColor = b;
	b = !b;
    }
}
