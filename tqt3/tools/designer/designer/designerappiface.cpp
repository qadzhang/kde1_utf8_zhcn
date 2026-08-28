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

#include <ntqmenubar.h>
#include <ntqpopupmenu.h>
#include "designerappiface.h"
#include "mainwindow.h"
#include "project.h"
#include "formwindow.h"
#include "workspace.h"
#include "widgetfactory.h"
#include "command.h"
#include "outputwindow.h"
#include "../shared/widgetdatabase.h"
#include <ntqvariant.h>
#include <ntqlistview.h>
#include <ntqtextedit.h>
#include <ntqstatusbar.h>
#include "pixmapcollection.h"
#include "hierarchyview.h"
#include <stdlib.h>
#include <ntqmetaobject.h>
#include "popupmenueditor.h"
#include "menubareditor.h"

DesignerInterfaceImpl::DesignerInterfaceImpl( MainWindow *mw )
    : mainWindow( mw )
{
}

DesignerInterfaceImpl::~DesignerInterfaceImpl()
{
}

TQRESULT DesignerInterfaceImpl::queryInterface( const TQUuid &uuid, TQUnknownInterface** iface )
{
    *iface = 0;

    if ( uuid == IID_QUnknown )
	*iface = (TQUnknownInterface*)this;
    else if ( uuid == IID_QComponentInformation )
	*iface = (TQComponentInformationInterface*)this;
    else if ( uuid == IID_Designer )
	*iface = (DesignerInterface*)this;
    else
	return TQE_NOINTERFACE;

    (*iface)->addRef();
    return TQS_OK;
}


DesignerProject *DesignerInterfaceImpl::currentProject() const
{
    return mainWindow->currProject()->iFace();
}

DesignerFormWindow *DesignerInterfaceImpl::currentForm() const
{
    if ( mainWindow->formWindow() )
	return mainWindow->formWindow()->iFace();
    return 0;
}

DesignerSourceFile *DesignerInterfaceImpl::currentSourceFile() const
{
    if ( mainWindow->sourceFile() )
	return mainWindow->sourceFile()->iFace();
    return 0;
}

TQPtrList<DesignerProject> DesignerInterfaceImpl::projectList() const
{
    return mainWindow->projectList();
}

void DesignerInterfaceImpl::showStatusMessage( const TQString &text, int ms ) const
{
    if ( text.isEmpty() ) {
	mainWindow->statusBar()->clear();
	return;
    }
    if ( ms )
	mainWindow->statusBar()->message( text, ms );
    else
	mainWindow->statusBar()->message( text );
}

DesignerDock *DesignerInterfaceImpl::createDock() const
{
    return 0;
}

DesignerOutputDock *DesignerInterfaceImpl::outputDock() const
{
    return mainWindow->outputWindow() ? mainWindow->outputWindow()->iFace() : 0;
}

void DesignerInterfaceImpl::setModified( bool b, TQWidget *window )
{
    mainWindow->setModified( b, window );
}

void DesignerInterfaceImpl::updateFunctionList()
{
    mainWindow->updateFunctionList();
    if ( mainWindow->objectHierarchy()->sourceEditor() )
	mainWindow->objectHierarchy()->updateClassBrowsers();
}

void DesignerInterfaceImpl::onProjectChange( TQObject *receiver, const char *slot )
{
    TQObject::connect( mainWindow, TQ_SIGNAL( projectChanged() ), receiver, slot );
}

void DesignerInterfaceImpl::onFormChange( TQObject *receiver, const char *slot )
{
    TQObject::connect( mainWindow, TQ_SIGNAL( formWindowChanged() ), receiver, slot );
    TQObject::connect( mainWindow, TQ_SIGNAL( editorChanged() ), receiver, slot );
}

bool DesignerInterfaceImpl::singleProjectMode() const
{
    return mainWindow->singleProjectMode();
}

void DesignerInterfaceImpl::showError( TQWidget *widget,
				       int line, const TQString &message )
{
    mainWindow->showErrorMessage( widget, line, message );
}

void DesignerInterfaceImpl::runFinished()
{
    mainWindow->finishedRun();
}

void DesignerInterfaceImpl::showStackFrame( TQWidget *w, int line )
{
    mainWindow->showStackFrame( w, line );
}

void DesignerInterfaceImpl::showDebugStep( TQWidget *w, int line )
{
    mainWindow->showDebugStep( w, line );
}

void DesignerInterfaceImpl::runProjectPrecondition()
{
    mainWindow->runProjectPrecondition();
}

void DesignerInterfaceImpl::runProjectPostcondition( TQObjectList *l )
{
    mainWindow->runProjectPostcondition( l );
}

DesignerProjectImpl::DesignerProjectImpl( Project *pr )
    : project( pr )
{
}

TQPtrList<DesignerFormWindow> DesignerProjectImpl::formList() const
{
    TQPtrList<DesignerFormWindow> list;
    TQObjectList *forms = project->formList();
    if ( !forms )
	return list;

    TQPtrListIterator<TQObject> it( *forms );
    while ( it.current() ) {
	TQObject *obj = it.current();
	++it;
	TQWidget *par = ::tqt_cast<FormWindow*>(obj->parent());
	if ( !obj->isWidgetType() || !par )
	    continue;

	list.append( ((FormWindow*)par)->iFace() );
    }

    delete forms;
    return list;
}

TQString DesignerProjectImpl::formFileName( const TQString &form ) const
{
    for ( TQPtrListIterator<FormFile> forms = project->formFiles();
	  forms.current(); ++forms ) {
	if ( TQString( forms.current()->formName() ) == form )
	    return forms.current()->fileName();
    }
    return TQString::null;
}

TQStringList DesignerProjectImpl::formNames() const
{
    TQStringList l;
    for ( TQPtrListIterator<FormFile> forms = project->formFiles();
	  forms.current(); ++forms ) {
	FormFile* f = forms.current();
	if ( f->isFake() )
	    continue;
	l << f->formName();
    }
    return l;
}

void DesignerProjectImpl::addForm( DesignerFormWindow * )
{
}

void DesignerProjectImpl::removeForm( DesignerFormWindow * )
{
}

TQString DesignerProjectImpl::fileName() const
{
    return project->fileName();
}

void DesignerProjectImpl::setFileName( const TQString & )
{
}

TQString DesignerProjectImpl::projectName() const
{
    return project->projectName();
}

void DesignerProjectImpl::setProjectName( const TQString & )
{
}

TQString DesignerProjectImpl::databaseFile() const
{
    return TQString::null;
}

void DesignerProjectImpl::setDatabaseFile( const TQString & )
{
}

void DesignerProjectImpl::setupDatabases() const
{
    MainWindow::self->editDatabaseConnections();
}

TQPtrList<DesignerDatabase> DesignerProjectImpl::databaseConnections() const
{
    TQPtrList<DesignerDatabase> lst;
#ifndef TQT_NO_SQL
    TQPtrList<DatabaseConnection> conns = project->databaseConnections();
    for ( DatabaseConnection *d = conns.first(); d; d = conns.next() )
	lst.append( d->iFace() );
#endif
    return lst;
}

void DesignerProjectImpl::addDatabase( DesignerDatabase * )
{
}

void DesignerProjectImpl::removeDatabase( DesignerDatabase * )
{
}

void DesignerProjectImpl::save() const
{
}

void DesignerProjectImpl::setLanguage( const TQString &l )
{
    project->setLanguage( l );
}

TQString DesignerProjectImpl::language() const
{
    return project->language();
}

void DesignerProjectImpl::setCustomSetting( const TQString &key, const TQString &value )
{
    project->setCustomSetting( key, value );
}

TQString DesignerProjectImpl::customSetting( const TQString &key ) const
{
    if ( key == "TQTSCRIPT_PACKAGES" ) {
	TQString s = getenv( "TQTSCRIPT_PACKAGES" );
	TQString s2 = project->customSetting( "QUICK_PACKAGES" );
	if ( !s.isEmpty() && !s2.isEmpty() )
#if defined(Q_OS_WIN32)
	    s += ";";
#else
	s += ":";
#endif
	s += s2;
	return s;
    }
    return project->customSetting( key );
}

DesignerPixmapCollection *DesignerProjectImpl::pixmapCollection() const
{
    return project->pixmapCollection()->iFace();
}

void DesignerProjectImpl::breakPoints( TQMap<TQString, TQValueList<uint> > &bps ) const
{
    MainWindow::self->saveAllBreakPoints();
    for ( TQPtrListIterator<SourceFile> sources = project->sourceFiles();
	  sources.current(); ++sources ) {
	SourceFile* f = sources.current();
	bps.insert( project->makeRelative( f->fileName() ) + " <Source-File>", MetaDataBase::breakPoints( f ) );
    }
    for ( TQPtrListIterator<FormFile> forms = project->formFiles();
	  forms.current(); ++forms ) {
	if ( forms.current()->formWindow() )
	    bps.insert( TQString( forms.current()->formWindow()->name() ) + " <Form>", MetaDataBase::breakPoints( forms.current()->formWindow() ) );
    }
}

TQString DesignerProjectImpl::breakPointCondition( TQObject *o, int line ) const
{
    return MetaDataBase::breakPointCondition( MainWindow::self->findRealObject( o ), line );
}

void DesignerProjectImpl::setBreakPointCondition( TQObject *o, int line, const TQString &condition )
{
    MetaDataBase::setBreakPointCondition( MainWindow::self->findRealObject( o ), line, condition );
}

void DesignerProjectImpl::clearAllBreakpoints() const
{
    TQValueList<uint> empty;
    for ( TQPtrListIterator<SourceFile> sources = project->sourceFiles();
	  sources.current(); ++sources ) {
	SourceFile* f = sources.current();
	MetaDataBase::setBreakPoints( f, empty );
    }
    for ( TQPtrListIterator<FormFile> forms = project->formFiles();
	  forms.current(); ++forms ) {
	if ( forms.current()->formWindow() )
	    MetaDataBase::setBreakPoints( forms.current()->formWindow(), empty );
	MainWindow::self->resetBreakPoints();
    }
}

void DesignerProjectImpl::setIncludePath( const TQString &platform, const TQString &path )
{
    project->setIncludePath( platform, path );
}

void DesignerProjectImpl::setLibs( const TQString &platform, const TQString &path )
{
    project->setLibs( platform, path );
}

void DesignerProjectImpl::setDefines( const TQString &platform, const TQString &path )
{
    project->setDefines( platform, path );
}

void DesignerProjectImpl::setConfig( const TQString &platform, const TQString &config )
{
    project->setConfig( platform, config );
}

void DesignerProjectImpl::setTemplate( const TQString &t )
{
    project->setTemplate( t );
}

TQString DesignerProjectImpl::config( const TQString &platform ) const
{
    return project->config( platform );
}

TQString DesignerProjectImpl::libs( const TQString &platform ) const
{
    return project->libs( platform );
}

TQString DesignerProjectImpl::defines( const TQString &platform ) const
{
    return project->defines( platform );
}

TQString DesignerProjectImpl::includePath( const TQString &platform ) const
{
    return project->includePath( platform );
}

TQString DesignerProjectImpl::templte() const
{
    return project->templte();
}

bool DesignerProjectImpl::isGenericObject( TQObject *o ) const
{
    return !!project->fakeFormFileFor( o );
}







#ifndef TQT_NO_SQL
DesignerDatabaseImpl::DesignerDatabaseImpl( DatabaseConnection *d )
    : db( d )
{
}

TQString DesignerDatabaseImpl::name() const
{
    return db->name();
}

void DesignerDatabaseImpl::setName( const TQString & )
{
}

TQString DesignerDatabaseImpl::driver() const
{
    return db->driver();
}

void DesignerDatabaseImpl::setDriver( const TQString & )
{
}

TQString DesignerDatabaseImpl::database() const
{
    return db->database();
}

void DesignerDatabaseImpl::setDatabase( const TQString & )
{
}

TQString DesignerDatabaseImpl::userName() const
{
    return db->username();
}

void DesignerDatabaseImpl::setUserName( const TQString & )
{
}

TQString DesignerDatabaseImpl::password() const
{
    return db->password();
}

void DesignerDatabaseImpl::setPassword( const TQString & )
{
}

TQString DesignerDatabaseImpl::hostName() const
{
    return db->hostname();
}

void DesignerDatabaseImpl::setHostName( const TQString & )
{
}

TQStringList DesignerDatabaseImpl::tables() const
{
    return db->tables();
}

TQMap<TQString, TQStringList> DesignerDatabaseImpl::fields() const
{
    return db->fields();
}

void DesignerDatabaseImpl::open( bool suppressDialog ) const
{
    db->open( suppressDialog );
}

void DesignerDatabaseImpl::close() const
{
    db->close();
}

void DesignerDatabaseImpl::setFields( const TQMap<TQString, TQStringList> & )
{
}

void DesignerDatabaseImpl::setTables( const TQStringList & )
{
}

TQSqlDatabase* DesignerDatabaseImpl::connection()
{
    return db->connection();
}
#endif



DesignerPixmapCollectionImpl::DesignerPixmapCollectionImpl( PixmapCollection *coll )
    : pixCollection( coll )
{
}

void DesignerPixmapCollectionImpl::addPixmap( const TQPixmap &p, const TQString &name, bool force )
{
    PixmapCollection::Pixmap pix;
    pix.pix = p;
    pix.name = name;
    pixCollection->addPixmap( pix, force );
    FormWindow *fw = MainWindow::self->formWindow();
    if ( fw )
	MetaDataBase::setPixmapKey( fw, p.serialNumber(), name );
}

TQPixmap DesignerPixmapCollectionImpl::pixmap( const TQString &name ) const
{
    return pixCollection->pixmap( name );
}



DesignerFormWindowImpl::DesignerFormWindowImpl( FormWindow *fw )
    : formWindow( fw )
{
}

TQString DesignerFormWindowImpl::name() const
{
    return formWindow->name();
 }

void DesignerFormWindowImpl::setName( const TQString &n )
{
    formWindow->setName( n );
}

TQString DesignerFormWindowImpl::fileName() const
{
    return formWindow->fileName();
}

void DesignerFormWindowImpl::setFileName( const TQString & )
{
}

void DesignerFormWindowImpl::save() const
{
}

bool DesignerFormWindowImpl::isModified() const
{
    return formWindow->commandHistory()->isModified();
}

void DesignerFormWindowImpl::insertWidget( TQWidget * )
{
}

TQWidget *DesignerFormWindowImpl::create( const char *className, TQWidget *parent, const char *name )
{
    TQWidget *w = WidgetFactory::create( WidgetDatabase::idFromClassName( className ), parent, name );
    formWindow->insertWidget( w, true );
    formWindow->killAccels( formWindow->mainContainer() );
    return w;
}

void DesignerFormWindowImpl::removeWidget( TQWidget * )
{
}

TQWidgetList DesignerFormWindowImpl::widgets() const
{
    return TQWidgetList();
}

void DesignerFormWindowImpl::undo()
{
}

void DesignerFormWindowImpl::redo()
{
}

void DesignerFormWindowImpl::cut()
{
}

void DesignerFormWindowImpl::copy()
{
}

void DesignerFormWindowImpl::paste()
{
}

void DesignerFormWindowImpl::adjustSize()
{
}

void DesignerFormWindowImpl::editConnections()
{
}

void DesignerFormWindowImpl::checkAccels()
{
}

void DesignerFormWindowImpl::layoutH()
{
    formWindow->layoutHorizontal();
}

void DesignerFormWindowImpl::layoutV()
{
}

void DesignerFormWindowImpl::layoutHSplit()
{
}

void DesignerFormWindowImpl::layoutVSplit()
{
}

void DesignerFormWindowImpl::layoutG()
{
    formWindow->layoutGrid();
}

void DesignerFormWindowImpl::layoutHContainer( TQWidget* w )
{
    formWindow->layoutHorizontalContainer( w );
}

void DesignerFormWindowImpl::layoutVContainer( TQWidget* w )
{
    formWindow->layoutVerticalContainer( w );
}

void DesignerFormWindowImpl::layoutGContainer( TQWidget* w )
{
    formWindow->layoutGridContainer( w );
}

void DesignerFormWindowImpl::breakLayout()
{
}

void DesignerFormWindowImpl::selectWidget( TQWidget * w )
{
    formWindow->selectWidget( w, true );
}

void DesignerFormWindowImpl::selectAll()
{
}

void DesignerFormWindowImpl::clearSelection()
{
    formWindow->clearSelection();
}

bool DesignerFormWindowImpl::isWidgetSelected( TQWidget * ) const
{
    return false;
}

TQWidgetList DesignerFormWindowImpl::selectedWidgets() const
{
    return formWindow->selectedWidgets();
}

TQWidget *DesignerFormWindowImpl::currentWidget() const
{
    return formWindow->currentWidget();
}

TQWidget *DesignerFormWindowImpl::form() const
{
    return formWindow;
}


void DesignerFormWindowImpl::setCurrentWidget( TQWidget * )
{
}

TQPtrList<TQAction> DesignerFormWindowImpl::actionList() const
{
    return TQPtrList<TQAction>();
}

TQAction *DesignerFormWindowImpl::createAction( const TQString& text, const TQIconSet& icon, const TQString& menuText, int accel,
					       TQObject* parent, const char* name, bool toggle )
{
    TQDesignerAction *a = new TQDesignerAction( parent );
    a->setName( name );
    a->setText( text );
    if ( !icon.isNull() && !icon.pixmap().isNull() )
    a->setIconSet( icon );
    a->setMenuText( menuText );
    a->setAccel( accel );
    a->setToggleAction( toggle );
    return a;
}

void DesignerFormWindowImpl::addAction( TQAction *a )
{
    if ( formWindow->actionList().findRef( a ) != -1 )
	return;
    formWindow->actionList().append( a );
    MetaDataBase::addEntry( a );
    setPropertyChanged( a, "name", true );
    setPropertyChanged( a, "text", true );
    setPropertyChanged( a, "menuText", true );
    setPropertyChanged( a, "accel", true );
    if ( !a->iconSet().isNull() && !a->iconSet().pixmap().isNull() )
	setPropertyChanged( a, "iconSet", true );
}

void DesignerFormWindowImpl::removeAction( TQAction *a )
{
    formWindow->actionList().removeRef( a );
}

void DesignerFormWindowImpl::preview() const
{
}

void DesignerFormWindowImpl::addConnection( TQObject *sender, const char *signal, TQObject *receiver, const char *slot )
{
    MetaDataBase::addConnection( formWindow, sender, signal, receiver, slot );
}

void DesignerFormWindowImpl::addFunction( const TQCString &function, const TQString &specifier,
					  const TQString &access, const TQString &type,
					  const TQString &language, const TQString &returnType )
{
    MetaDataBase::addFunction( formWindow, function, specifier, access, type, language, returnType );
    formWindow->mainWindow()->functionsChanged();
}



void DesignerFormWindowImpl::setProperty( TQObject *o, const char *property, const TQVariant &value )
{
    int id = o->metaObject()->findProperty( property, true );
    const TQMetaProperty* p = o->metaObject()->property( id, true );
    if ( p && p->isValid() )
	o->setProperty( property, value );
    else
	MetaDataBase::setFakeProperty( o, property, value );
}

TQVariant DesignerFormWindowImpl::property( TQObject *o, const char *prop ) const
{
    int id = o->metaObject()->findProperty( prop, true );
    const TQMetaProperty* p = o->metaObject()->property( id, true );
    if ( p && p->isValid() )
	return o->property( prop );
    return MetaDataBase::fakeProperty( o, prop );
}

void DesignerFormWindowImpl::setPropertyChanged( TQObject *o, const char *property, bool changed )
{
    MetaDataBase::setPropertyChanged( o, property, changed );
}

bool DesignerFormWindowImpl::isPropertyChanged( TQObject *o, const char *property ) const
{
    return MetaDataBase::isPropertyChanged( o, property );
}

void DesignerFormWindowImpl::setColumnFields( TQObject *o, const TQMap<TQString, TQString> &f )
{
    MetaDataBase::setColumnFields( o, f );
}

TQStringList DesignerFormWindowImpl::implementationIncludes() const
{
    TQValueList<MetaDataBase::Include> includes = MetaDataBase::includes( formWindow );
    TQStringList lst;
    for ( TQValueList<MetaDataBase::Include>::Iterator it = includes.begin(); it != includes.end(); ++it ) {
	MetaDataBase::Include inc = *it;
	if ( inc.implDecl != "in implementation" )
	    continue;
	TQString s = inc.header;
	if ( inc.location == "global" ) {
	    s.prepend( "<" );
	    s.append( ">" );
	} else {
	    s.prepend( "\"" );
	    s.append( "\"" );
	}
	lst << s;
    }
    return lst;
}

TQStringList DesignerFormWindowImpl::declarationIncludes() const
{
    TQValueList<MetaDataBase::Include> includes = MetaDataBase::includes( formWindow );
    TQStringList lst;
    for ( TQValueList<MetaDataBase::Include>::Iterator it = includes.begin(); it != includes.end(); ++it ) {
	MetaDataBase::Include inc = *it;
	if ( inc.implDecl == "in implementation" )
	    continue;
	TQString s = inc.header;
	if ( inc.location == "global" ) {
	    s.prepend( "<" );
	    s.append( ">" );
	} else {
	    s.prepend( "\"" );
	    s.append( "\"" );
	}
	lst << s;
    }
    return lst;
}

void DesignerFormWindowImpl::setImplementationIncludes( const TQStringList &lst )
{
    TQValueList<MetaDataBase::Include> oldIncludes = MetaDataBase::includes( formWindow );
    TQValueList<MetaDataBase::Include> includes;
    for ( TQValueList<MetaDataBase::Include>::Iterator it = oldIncludes.begin(); it != oldIncludes.end(); ++it ) {
	MetaDataBase::Include inc = *it;
	if ( inc.implDecl == "in implementation" )
	    continue;
	includes << inc;
    }

    for ( TQStringList::ConstIterator sit = lst.begin(); sit != lst.end(); ++sit ) {
	TQString s = *sit;
	if ( s.startsWith( "#include" ) )
	    s.remove( (uint)0, 8 );
	s = s.simplifyWhiteSpace();
	if ( s[ 0 ] != '<' && s[ 0 ] != '"' ) {
	    s.prepend( "\"" );
	    s.append( "\"" );
	}
	if ( s[ 0 ] == '<' ) {
	    s.remove( (uint)0, 1 );
	    s.remove( s.length() - 1, 1 );
	    MetaDataBase::Include inc;
	    inc.header = s;
	    inc.implDecl = "in implementation";
	    inc.location = "global";
	    includes << inc;
	} else {
	    s.remove( (uint)0, 1 );
	    s.remove( s.length() - 1, 1 );
	    MetaDataBase::Include inc;
	    inc.header = s;
	    inc.implDecl = "in implementation";
	    inc.location = "local";
	    includes << inc;
	}
    }
    MetaDataBase::setIncludes( formWindow, includes );
    formWindow->mainWindow()->objectHierarchy()->formDefinitionView()->setup();
}

void DesignerFormWindowImpl::setDeclarationIncludes( const TQStringList &lst )
{
    TQValueList<MetaDataBase::Include> oldIncludes = MetaDataBase::includes( formWindow );
    TQValueList<MetaDataBase::Include> includes;
    for ( TQValueList<MetaDataBase::Include>::Iterator it = oldIncludes.begin(); it != oldIncludes.end(); ++it ) {
	MetaDataBase::Include inc = *it;
	if ( inc.implDecl == "in declaration" )
	    continue;
	includes << inc;
    }

    for ( TQStringList::ConstIterator sit = lst.begin(); sit != lst.end(); ++sit ) {
	TQString s = *sit;
	if ( s.startsWith( "#include" ) )
	    s.remove( (uint)0, 8 );
	s = s.simplifyWhiteSpace();
	if ( s[ 0 ] != '<' && s[ 0 ] != '"' ) {
	    s.prepend( "\"" );
	    s.append( "\"" );
	}
	if ( s[ 0 ] == '<' ) {
	    s.remove( (uint)0, 1 );
	    s.remove( s.length() - 1, 1 );
	    MetaDataBase::Include inc;
	    inc.header = s;
	    inc.implDecl = "in declaration";
	    inc.location = "global";
	    includes << inc;
	} else {
	    s.remove( (uint)0, 1 );
	    s.remove( s.length() - 1, 1 );
	    MetaDataBase::Include inc;
	    inc.header = s;
	    inc.implDecl = "in declaration";
	    inc.location = "local";
	    includes << inc;
	}
    }
    MetaDataBase::setIncludes( formWindow, includes );
    formWindow->mainWindow()->objectHierarchy()->formDefinitionView()->setup();
}

TQStringList DesignerFormWindowImpl::forwardDeclarations() const
{
    return MetaDataBase::forwards( formWindow );
}

void DesignerFormWindowImpl::setForwardDeclarations( const TQStringList &lst )
{
    MetaDataBase::setForwards( formWindow, lst );
    formWindow->mainWindow()->objectHierarchy()->formDefinitionView()->setup();
}

TQStringList DesignerFormWindowImpl::signalList() const
{
    return MetaDataBase::signalList( formWindow );
}

void DesignerFormWindowImpl::setSignalList( const TQStringList &lst )
{
    MetaDataBase::setSignalList( formWindow, lst );
    formWindow->mainWindow()->objectHierarchy()->formDefinitionView()->setup();
}

void DesignerFormWindowImpl::onModificationChange( TQObject *receiver, const char *slot )
{
    TQObject::connect( formWindow, TQ_SIGNAL( modificationChanged( bool, FormWindow * ) ), receiver, slot );
}

void DesignerFormWindowImpl::addMenu( const TQString &text, const TQString &name )
{
    if ( !::tqt_cast<TQMainWindow*>(formWindow->mainContainer()) )
	return;

    TQMainWindow *mw = (TQMainWindow*)formWindow->mainContainer();
    PopupMenuEditor *popup = new PopupMenuEditor( formWindow, mw );
    TQString n = name;
    formWindow->unify( popup, n, true );
    popup->setName( n );
    MenuBarEditor *mb = (MenuBarEditor *)mw->child( 0, "MenuBarEditor" );
    if ( !mb ) {
	mb = new MenuBarEditor( formWindow, mw );
	mb->setName( "MenuBar" );
	MetaDataBase::addEntry( mb );
    }
    mb->insertItem( text, popup );
    MetaDataBase::addEntry( popup );
}

void DesignerFormWindowImpl::addMenuAction( const TQString &menu, TQAction *a )
{
    if ( !::tqt_cast<TQMainWindow*>(formWindow->mainContainer()) )
	return;
    TQMainWindow *mw = (TQMainWindow*)formWindow->mainContainer();
    if ( !mw->child( 0, "MenuBarEditor" ) )
	return;
    PopupMenuEditor *popup = (PopupMenuEditor*)mw->child( menu, "PopupMenuEditor" );
    if ( !popup )
	return;
    popup->insert( a );
}

void DesignerFormWindowImpl::addMenuSeparator( const TQString &menu )
{
    if ( !::tqt_cast<TQMainWindow*>(formWindow->mainContainer()) )
	return;
    TQMainWindow *mw = (TQMainWindow*)formWindow->mainContainer();
    if ( !mw->child( 0, "MenuBarEditor" ) )
	return;
    PopupMenuEditor *popup = (PopupMenuEditor*)mw->child( menu, "PopupMenuEditor" );
    if ( !popup )
	return;
    TQAction *a = new TQSeparatorAction( 0 );
    popup->insert( a );
}

void DesignerFormWindowImpl::addToolBar( const TQString &text, const TQString &name )
{
    if ( !::tqt_cast<TQMainWindow*>(formWindow->mainContainer()) )
	return;
    TQMainWindow *mw = (TQMainWindow*)formWindow->mainContainer();
    TQToolBar *tb = new TQDesignerToolBar( mw );
    TQString n = name;
    formWindow->unify( tb, n, true );
    tb->setName( n );
    mw->addToolBar( tb, text );
}

void DesignerFormWindowImpl::addToolBarAction( const TQString &tbn, TQAction *a )
{
    if ( !::tqt_cast<TQMainWindow*>(formWindow->mainContainer()) )
	return;
    TQMainWindow *mw = (TQMainWindow*)formWindow->mainContainer();
    TQDesignerToolBar *tb = (TQDesignerToolBar*)mw->child( tbn, "TQDesignerToolBar" );
    if ( !tb )
	return;
    a->addTo( tb );
    tb->addAction( a );
}

void DesignerFormWindowImpl::addToolBarSeparator( const TQString &tbn )
{
    if ( !::tqt_cast<TQMainWindow*>(formWindow->mainContainer()) )
	return;
    TQMainWindow *mw = (TQMainWindow*)formWindow->mainContainer();
    TQDesignerToolBar *tb = (TQDesignerToolBar*)mw->child( tbn, "TQDesignerToolBar" );
    if ( !tb )
	return;
    TQAction *a = new TQSeparatorAction( 0 );
    a->addTo( tb );
    tb->addAction( a );
}

DesignerDockImpl::DesignerDockImpl()
{
}

TQDockWindow *DesignerDockImpl::dockWindow() const
{
    return 0;
}

DesignerOutputDockImpl::DesignerOutputDockImpl( OutputWindow *ow )
    : outWin( ow )
{
}

TQWidget *DesignerOutputDockImpl::addView( const TQString &title )
{
    TQWidget *page = new TQWidget( outWin );
    outWin->addTab( page, title );
    return page;
}

void DesignerOutputDockImpl::appendDebug( const TQString &s )
{
    outWin->appendDebug( s );
}

void DesignerOutputDockImpl::clearDebug()
{
}

void DesignerOutputDockImpl::appendError( const TQString &s, int l )
{
    TQStringList ls;
    ls << s;
    TQValueList<uint> ll;
    ll << l;
    outWin->setErrorMessages( ls, ll, false, TQStringList(), TQObjectList() );
}

void DesignerOutputDockImpl::clearError()
{
}

DesignerSourceFileImpl::DesignerSourceFileImpl( SourceFile *e )
    : ed( e )
{
}

TQString DesignerSourceFileImpl::fileName() const
{
    return ed->fileName();
}
