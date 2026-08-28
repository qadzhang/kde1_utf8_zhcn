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

#include "sourceeditor.h"

#include "formwindow.h"
#include "metadatabase.h"
#include "project.h"
#include "mainwindow.h"
#include "../interfaces/languageinterface.h"
#include <ntqregexp.h>
#include "project.h"
#include "sourcefile.h"
#include "hierarchyview.h"
#include <ntqmessagebox.h>
#include <ntqtextstream.h>

SourceEditor::SourceEditor( TQWidget *parent, EditorInterface *iface, LanguageInterface *liface )
    : TQVBox( parent, 0, WDestructiveClose ), iFace( iface ), lIface( liface ), obj( 0 ), pro( 0 )
{
    iFace->addRef();
    lIface->addRef();
    editor = iFace->editor( MainWindow::self->areEditorsReadOnly(),
			    this, MainWindow::self->designerInterface() );
    iFace->onBreakPointChange( MainWindow::self, TQ_SLOT( breakPointsChanged() ) );
    resize( 600, 400 );
    setIcon( TQPixmap::fromMimeSource( "designer_filenew.png" ) );
}

SourceEditor::~SourceEditor()
{
    saveBreakPoints();
    editor = 0;
    if ( formWindow() ) {
	formWindow()->formFile()->setCodeEdited( false );
	formWindow()->formFile()->setEditor( 0 );
    } else if ( sourceFile() ) {
	sourceFile()->setEditor( 0 );
	if ( MainWindow::self->objectHierarchy()->sourceEditor() == this )
	    MainWindow::self->objectHierarchy()->setFormWindow( 0, 0 );
    }
    iFace->release();
    lIface->release();
    MainWindow::self->editorClosed( this );
}

void SourceEditor::setObject( TQObject *o, Project *p )
{
    if ( sourceFile() )
	sourceFile()->setEditor( 0 );
    if ( formWindow() ) {
	formWindow()->formFile()->setCodeEdited( false );
	formWindow()->formFile()->setEditor( 0 );
    }
    if ( ::tqt_cast<FormWindow*>(o) )
	( (FormWindow*)o )->formFile()->setCodeEdited( true );
    save();
    bool changed = false;
    if ( &(*obj) != o ) {
	saveBreakPoints();
	changed = true;
    }
    obj = o;
    pro = p;
    if ( formWindow() ) {
	if ( formWindow()->isFake() )
	    setCaption( formWindow()->project()->objectForFakeForm( formWindow() )->name() );
	else
	    setCaption( obj->name() );
    } else {
	setCaption( sourceFile()->fileName() );
    }
    if ( sourceFile() )
	sourceFile()->setEditor( this );
    else if ( formWindow() )
	formWindow()->formFile()->setEditor( this );
    iFace->setText( sourceOfObject( obj, lang, iFace, lIface ) );
    if ( pro && formWindow() ) {
	if ( formWindow()->isFake() )
	    iFace->setContext( pro->objectForFakeFormFile( formWindow()->formFile() ) );
	else
	    iFace->setContext( formWindow()->mainContainer() );
    } else {
	iFace->setContext( 0 );
    }
    if ( changed || sourceFile() )
	iFace->setBreakPoints( MetaDataBase::breakPoints( o ) );
    MainWindow::self->objectHierarchy()->showClasses( this );
}

TQString SourceEditor::sourceOfObject( TQObject *o, const TQString &,
				      EditorInterface *, LanguageInterface * )
{
    TQString txt;
    if ( !o )
	return txt;
    if ( ::tqt_cast<FormWindow*>(o) )
	txt = ( (FormWindow*)o )->formFile()->code();
    else if ( ::tqt_cast<SourceFile*>(o) )
	txt = ( (SourceFile*)o )->text();
    return txt;
}

void SourceEditor::setFunction( const TQString &func, const TQString &clss )
{
    iFace->scrollTo( lIface->createFunctionStart( obj->name(), func, "", "" ), clss );
}

void SourceEditor::setClass( const TQString &clss )
{
    iFace->scrollTo( clss, TQString::null );
}

void SourceEditor::closeEvent( TQCloseEvent *e )
{
    e->accept();
    if ( !obj )
	return;
    if ( formWindow() ) {
	save();
	formWindow()->formFile()->cm = formWindow()->formFile()->isModified();
    } else {
	if ( !sourceFile()->closeEvent() )
	    e->ignore();
    }
}

void SourceEditor::save()
{
    if ( !obj )
	return;
    if ( formWindow() )
	formWindow()->formFile()->syncCode();
    else if ( sourceFile() && sourceFile()->editor() )
	sourceFile()->setText( iFace->text() );
}

TQString SourceEditor::language() const
{
    return lang;
}

void SourceEditor::setLanguage( const TQString &l )
{
    lang = l;
}

void SourceEditor::editCut()
{
    iFace->cut();
}

void SourceEditor::editCopy()
{
    iFace->copy();
}

void SourceEditor::editPaste()
{
    iFace->paste();
}

bool SourceEditor::editIsUndoAvailable()
{
    return iFace->isUndoAvailable();
}

bool SourceEditor::editIsRedoAvailable()
{
    return iFace->isRedoAvailable();
}

void SourceEditor::editUndo()
{
    iFace->undo();
}

void SourceEditor::editRedo()
{
    iFace->redo();
}

void SourceEditor::editSelectAll()
{
    iFace->selectAll();
}

void SourceEditor::configChanged()
{
    iFace->readSettings();
}

void SourceEditor::setModified( bool b )
{
    iFace->setModified( b );
}

void SourceEditor::refresh( bool allowSave )
{
    if ( allowSave )
	save();
    bool oldMod = iFace->isModified();
    iFace->setText( sourceOfObject( obj, lang, iFace, lIface ) );
    iFace->setModified( oldMod );
}

void SourceEditor::resetContext()
{
    if ( pro && formWindow() ) {
	if ( formWindow()->isFake() )
	    iFace->setContext( pro->objectForFakeFormFile( formWindow()->formFile() ) );
	else
	    iFace->setContext( formWindow()->mainContainer() );
    } else {
	iFace->setContext( 0 );
    }
}

void SourceEditor::setFocus()
{
    if ( formWindow() )
	formWindow()->formFile()->setCodeEdited( true );
    if ( editor )
	editor->setFocus();
}

int SourceEditor::numLines() const
{
    return iFace->numLines();
}

void SourceEditor::saveBreakPoints()
{
    if ( !obj )
	return;
    TQValueList<uint> l;
    iFace->breakPoints( l );
    MetaDataBase::setBreakPoints( obj, l );
}

void SourceEditor::clearStep()
{
    iFace->clearStep();
}

void SourceEditor::clearStackFrame()
{
    iFace->clearStackFrame();
}

void SourceEditor::resetBreakPoints()
{
    iFace->setBreakPoints( MetaDataBase::breakPoints( obj ) );
}

TQString SourceEditor::text() const
{
    return iFace->text();
}

bool SourceEditor::isModified() const
{
    return iFace->isModified();
}

void SourceEditor::checkTimeStamp()
{
    if ( formWindow() )
	formWindow()->formFile()->checkTimeStamp();
    else if ( sourceFile() )
	sourceFile()->checkTimeStamp();
}

bool SourceEditor::saveAs()
{
    if ( formWindow() )
	return formWindow()->formFile()->saveAs();
    else if ( sourceFile() )
	return sourceFile()->saveAs();
    return false;
}

SourceFile *SourceEditor::sourceFile() const
{
    if ( !::tqt_cast<SourceFile*>((TQObject *)obj) )
	return 0;
    return (SourceFile*)(TQObject*)obj;
}

FormWindow *SourceEditor::formWindow() const
{
    if ( !::tqt_cast<FormWindow*>((TQObject *)obj) )
	return 0;
    return (FormWindow*)(TQObject*)obj;
}
