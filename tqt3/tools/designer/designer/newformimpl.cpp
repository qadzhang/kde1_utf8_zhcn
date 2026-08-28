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

#include "newformimpl.h"
#include "mainwindow.h"
#include "metadatabase.h"
#include "project.h"
#include "formwindow.h"
#include "widgetfactory.h"
#include "widgetdatabase.h"
#include "actioneditorimpl.h"
#include "hierarchyview.h"
#include "resource.h"
#include "projectsettingsimpl.h"
#include "sourcefile.h"

#include <ntqiconview.h>
#include <ntqlabel.h>
#include <ntqfileinfo.h>
#include <ntqdir.h>
#include <ntqpushbutton.h>
#include <stdlib.h>
#include <ntqcombobox.h>
#include <ntqworkspace.h>
#include <ntqmessagebox.h>

static int forms = 0;

ProjectItem::ProjectItem( TQIconView *view, const TQString &text )
    : NewItem( view, text )
{
}

void ProjectItem::insert( Project * )
{
    MainWindow::self->createNewProject( lang );
}



FormItem::FormItem( TQIconView *view, const TQString &text )
    : NewItem( view, text )
{
}

void FormItem::insert( Project *pro )
{
    TQString n = "Form" + TQString::number( ++forms );
    FormWindow *fw = 0;
    FormFile *ff = new FormFile( FormFile::createUnnamedFileName(), true, pro );
    fw = new FormWindow( ff, MainWindow::self, MainWindow::self->qWorkspace(), n );
    fw->setProject( pro );
    MetaDataBase::addEntry( fw );
    if ( fType == Widget ) {
	TQWidget *w = WidgetFactory::create( WidgetDatabase::idFromClassName( "TQWidget" ),
					    fw, n.latin1() );
	fw->setMainContainer( w );
    } else if ( fType == Dialog ) {
	TQWidget *w = WidgetFactory::create( WidgetDatabase::idFromClassName( "TQDialog" ), fw, n.latin1() );
	fw->setMainContainer( w );
    } else if ( fType == Wizard ) {
	TQWidget *w = WidgetFactory::create( WidgetDatabase::idFromClassName( "TQWizard" ),
					    fw, n.latin1() );
	fw->setMainContainer( w );
    } else if ( fType == MainWindow ) {
	TQWidget *w = WidgetFactory::create( WidgetDatabase::idFromClassName( "TQMainWindow" ),
					    fw, n.latin1() );
	fw->setMainContainer( w );
    }

    fw->setCaption( n );
    fw->resize( 600, 480 );
    MainWindow::self->insertFormWindow( fw );

    TemplateWizardInterface *iface =
	MainWindow::self->templateWizardInterface( fw->mainContainer()->className() );
    if ( iface ) {
	iface->setup( fw->mainContainer()->className(), fw->mainContainer(),
		      fw->iFace(), MainWindow::self->designerInterface() );
	iface->release();
    }

    // the wizard might have changed a lot, lets update everything
    MainWindow::self->actioneditor()->setFormWindow( fw );
    MainWindow::self->objectHierarchy()->setFormWindow( fw, fw );
    MainWindow::self->objectHierarchy()->formDefinitionView()->refresh();
    MainWindow::self->objectHierarchy()->rebuild();
    fw->killAccels( fw );
    fw->project()->setModified( true );
    fw->setFocus();
    if ( !pro->isDummy() ) {
	fw->setSavePixmapInProject( true );
	fw->setSavePixmapInline( false );
    }
}



CustomFormItem::CustomFormItem( TQIconView *view, const TQString &text )
    : NewItem( view, text )
{
}

static void unifyFormName( FormWindow *fw, TQWorkspace *qworkspace )
{
    TQStringList lst;
    TQWidgetList windows = qworkspace->windowList();
    for ( TQWidget *w =windows.first(); w; w = windows.next() ) {
	if ( w == fw )
	    continue;
	lst << w->name();
    }

    if ( lst.findIndex( fw->name() ) == -1 )
	return;
    TQString origName = fw->name();
    TQString n = origName;
    int i = 1;
    while ( lst.findIndex( n ) != -1 ) {
	n = origName + TQString::number( i++ );
    }
    fw->setName( n );
    fw->setCaption( n );
}

void CustomFormItem::insert( Project *pro )
{
    TQString filename = templateFileName();
    if ( !filename.isEmpty() && TQFile::exists( filename ) ) {
	Resource resource( MainWindow::self );
	FormFile *ff = new FormFile( filename, true, pro );
	if ( !resource.load( ff ) ) {
	    TQMessageBox::information( MainWindow::self, MainWindow::tr("Load Template"),
				      MainWindow::tr("Couldn't load form description from template '" +
						     filename + "'" ) );
	    delete ff;
	    return;
	}
	ff->setFileName( TQString::null );
	if ( MainWindow::self->formWindow() ) {
	    MainWindow::self->formWindow()->setFileName( TQString::null );
	    unifyFormName( MainWindow::self->formWindow(), MainWindow::self->qWorkspace() );
	    if ( !pro->isDummy() ) {
		MainWindow::self->formWindow()->setSavePixmapInProject( true );
		MainWindow::self->formWindow()->setSavePixmapInline( false );
	    }
	}
    }
}



SourceFileItem::SourceFileItem( TQIconView *view, const TQString &text )
    : NewItem( view, text ), visible( true )
{
}

void SourceFileItem::insert( Project *pro )
{
    SourceFile *f = new SourceFile( SourceFile::createUnnamedFileName( ext ), true, pro );
    MainWindow::self->editSource( f );
}

void SourceFileItem::setProject( Project *pro )
{
    TQIconView *iv = iconView();
    bool v = lang == pro->language();
    if ( !iv || v == visible )
	return;
    visible = v;
    if ( !visible )
	iv->takeItem( this );
    else
	iv->insertItem( this );
}



SourceTemplateItem::SourceTemplateItem( TQIconView *view, const TQString &text )
    : NewItem( view, text ), visible( true )
{
}

void SourceTemplateItem::insert( Project *pro )
{
    SourceTemplateInterface *siface = MainWindow::self->sourceTemplateInterface( text() );
    if ( !siface )
	return;
    SourceTemplateInterface::Source src = siface->create( text(), MainWindow::self->designerInterface() );
    SourceFile *f = 0;
    if ( src.type == SourceTemplateInterface::Source::Invalid )
	return;
    if ( src.type == SourceTemplateInterface::Source::FileName )
	f = new SourceFile( src.filename, false, pro );
    else
	f = new SourceFile( SourceFile::createUnnamedFileName( src.extension ), true, pro );
    if ( f->isAccepted()) {
	f->setText( src.code );
	MainWindow::self->editSource( f );
	f->setModified( true );
    } else {
	delete f;
    }
}

void SourceTemplateItem::setProject( Project *pro )
{
    TQIconView *iv = iconView();
    bool v = !pro->isDummy() && lang == pro->language();
    if ( !iv || v == visible )
	return;
    visible = v;
    if ( !visible )
	iv->takeItem( this );
    else
	iv->insertItem( this );
}

void NewForm::insertTemplates( TQIconView *tView,
			       const TQString &templatePath )
{
    TQStringList::Iterator it;
    TQStringList languages = MetaDataBase::languages();
    if ( !MainWindow::self->singleProjectMode() ) {
	for ( it = languages.begin(); it != languages.end(); ++it ) {
	    ProjectItem *pi = new ProjectItem( tView, *it + " " + tr( "Project" ) );
	    allItems.append( pi );
	    pi->setLanguage( *it );
	    pi->setPixmap( TQPixmap::fromMimeSource( "designer_project.png" ) );
	    pi->setDragEnabled( false );
	}
    }
    TQIconViewItem *cur = 0;
    FormItem *fi = new FormItem( tView,tr( "Dialog" ) );
    allItems.append( fi );
    fi->setFormType( FormItem::Dialog );
    fi->setPixmap( TQPixmap::fromMimeSource( "designer_newform.png" ) );
    fi->setDragEnabled( false );
    cur = fi;
    if ( !MainWindow::self->singleProjectMode() ) {
	fi = new FormItem( tView,tr( "Wizard" ) );
	allItems.append( fi );
	fi->setFormType( FormItem::Wizard );
	fi->setPixmap( TQPixmap::fromMimeSource( "designer_newform.png" ) );
	fi->setDragEnabled( false );
	fi = new FormItem( tView, tr( "Widget" ) );
	allItems.append( fi );
	fi->setFormType( FormItem::Widget );
	fi->setPixmap( TQPixmap::fromMimeSource( "designer_newform.png" ) );
	fi->setDragEnabled( false );
	fi = new FormItem( tView, tr( "Main Window" ) );
	allItems.append( fi );
	fi->setFormType( FormItem::MainWindow );
	fi->setPixmap( TQPixmap::fromMimeSource( "designer_newform.png" ) );
	fi->setDragEnabled( false );

	TQString templPath = templatePath;
	TQStringList templRoots;
	const char *qtdir = getenv( "TQTDIR" );
	if(qtdir)
	    templRoots << qtdir;
	templRoots << tqInstallPathData();
	if(qtdir) //try the tools/designer directory last!
	    templRoots << (TQString(qtdir) + "/tools/designer");
	for ( TQStringList::Iterator it = templRoots.begin(); it != templRoots.end(); ++it ) {
	    TQString path = (*it) + "/templates";
	    if ( TQFile::exists( path )) {
		templPath = path;
		break;
	    }
	}
	if ( !templPath.isEmpty() ) {
	    TQDir dir( templPath  );
	    const TQFileInfoList *filist = dir.entryInfoList( TQDir::DefaultFilter, TQDir::DirsFirst | TQDir::Name );
	    if ( filist ) {
		TQFileInfoListIterator it( *filist );
		TQFileInfo *fi;
		while ( ( fi = it.current() ) != 0 ) {
		    ++it;
		    if ( !fi->isFile() || fi->extension() != "ui" )
			continue;
		    TQString name = fi->baseName();
		    name = name.replace( '_', ' ' );
		    CustomFormItem *ci = new CustomFormItem( tView, name );
		    allItems.append( ci );
		    ci->setDragEnabled( false );
		    ci->setPixmap( TQPixmap::fromMimeSource( "designer_newform.png" ) );
		    ci->setTemplateFile( fi->absFilePath() );
		}
	    }
	}
    }

    for ( it = languages.begin(); it != languages.end(); ++it ) {
	LanguageInterface *iface = MetaDataBase::languageInterface( *it );
	if ( iface ) {
	    TQMap<TQString, TQString> extensionMap;
	    iface->preferedExtensions( extensionMap );
	    for ( TQMap<TQString, TQString>::Iterator eit = extensionMap.begin();
		  eit != extensionMap.end(); ++eit ) {
		SourceFileItem * si = new SourceFileItem( tView, *eit );
		allItems.append( si );
		si->setExtension( eit.key() );
		si->setLanguage( *it );
		si->setPixmap( TQPixmap::fromMimeSource( "designer_filenew.png" ) );
		si->setDragEnabled( false );
	    }
	    iface->release();
	}
    }

    if ( !MainWindow::self->singleProjectMode() ) {
	TQStringList sourceTemplates = MainWindow::self->sourceTemplates();
	for ( TQStringList::Iterator sit = sourceTemplates.begin(); sit != sourceTemplates.end(); ++sit ) {
	    SourceTemplateInterface *siface = MainWindow::self->sourceTemplateInterface( *sit );
	    if ( !siface )
		continue;
	    SourceTemplateItem * si = new SourceTemplateItem( tView, *sit );
	    allItems.append( si );
	    si->setTemplate( *sit );
	    si->setLanguage( siface->language( *sit ) );
	    si->setPixmap( TQPixmap::fromMimeSource( "designer_filenew.png" ) );
	    si->setDragEnabled( false );
	    siface->release();
	}
    }

    tView->viewport()->setFocus();
    tView->setCurrentItem( cur );

    if ( MainWindow::self->singleProjectMode() )
	adjustSize();
}

NewForm::NewForm( TQIconView *templateView, const TQString &templatePath )
{
    insertTemplates( templateView, templatePath );
    projectChanged( tr( "<No Project>" ) );
}

NewForm::NewForm( TQWidget *parent, const TQStringList& projects,
		  const TQString& currentProject, const TQString &templatePath )
    : NewFormBase( parent, 0, true )
{
    connect( helpButton, TQ_SIGNAL( clicked() ), MainWindow::self, TQ_SLOT( showDialogHelp() ) );

    projectCombo->insertStringList( projects );
    projectCombo->setCurrentText( currentProject );

    insertTemplates( templateView, templatePath );

    projectChanged( projectCombo->currentText() );
}

void NewForm::accept()
{
    if ( !templateView->currentItem() )
	return;
    Project *pro = MainWindow::self->findProject( projectCombo->currentText() );
    if ( !pro )
	return;
    MainWindow::self->setCurrentProject( pro );
    NewFormBase::accept();
    ( (NewItem*)templateView->currentItem() )->insert( pro );
}

void NewForm::projectChanged( const TQString &project )
{
    Project *pro = MainWindow::self->findProject( project );
    if ( !pro )
	return;
    TQIconViewItem *i;
    for ( i = allItems.first(); i; i = allItems.next() )
	( (NewItem*)i )->setProject( pro );
    templateView->setCurrentItem( templateView->firstItem() );
    templateView->arrangeItemsInGrid( true );
}

void NewForm::itemChanged( TQIconViewItem *item )
{
    labelProject->setEnabled( item->rtti() != NewItem::ProjectType );
    projectCombo->setEnabled( item->rtti() != NewItem::ProjectType );
}

TQPtrList<TQIconViewItem> NewForm::allViewItems()
{
    return allItems;
}
