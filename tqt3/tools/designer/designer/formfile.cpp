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

#include "formfile.h"
#include "timestamp.h"
#include "project.h"
#include "formwindow.h"
#include "command.h"
#include "sourceeditor.h"
#include "mainwindow.h"
#include "../interfaces/languageinterface.h"
#include "resource.h"
#include "workspace.h"
#include <ntqmessagebox.h>
#include <ntqfile.h>
#include <ntqstatusbar.h>
#include "propertyeditor.h"
#include <ntqworkspace.h>
#include <stdlib.h>
#include "designerappiface.h"
#include <ntqapplication.h>

static TQString make_func_pretty( const TQString &s )
{
    TQString res = s;
    if ( res.find( ")" ) - res.find( "(" ) == 1 )
	return res;
    res.replace( "(", "( " );
    res.replace( ")", " )" );
    res.replace( "&", " &" );
    res.replace( "*", " *" );
    res.replace( ",", ", " );
    res.replace( ":", " : " );
    res = res.simplifyWhiteSpace();
    res.replace( " : : ", "::" );
    res.replace( ">>", "> >" );
    return res;
}

FormFile::FormFile( const TQString &fn, bool temp, Project *p, const char *name )
    : filename( fn ), fileNameTemp( temp ), pro( p ), fw( 0 ), ed( 0 ),
      timeStamp( 0, fn + codeExtension() ), codeEdited( false ), pkg( false ),
      cm( false ), codeFileStat( None ), timeStampUi(0, fn)
{
    MetaDataBase::addEntry( this );
    fake = qstrcmp( name, "qt_fakewindow" ) == 0;
    //codeFileStat = FormFile::None;
    pro->addFormFile( this );
    loadCode();
    if ( !temp )
	checkFileName( false );
    timeStampUi.update();
}

FormFile::~FormFile()
{
    pro->removeFormFile( this );
    if ( formWindow() )
	formWindow()->setFormFile( 0 );
}

void FormFile::setFormWindow( FormWindow *f )
{
    if ( f == fw )
	return;
    if ( fw )
	fw->setFormFile( 0 );
    fw = f;
    if ( fw )
	fw->setFormFile( this );
    parseCode( cod, false );
    TQTimer::singleShot( 0, this, TQ_SLOT( notifyFormWindowChange() ) );
}

void FormFile::setEditor( SourceEditor *e )
{
    ed = e;
}

void FormFile::setFileName( const TQString &fn )
{
    if ( fn == filename )
	return;
    if ( fn.isEmpty() ) {
	fileNameTemp = true;
	if ( filename.find( "unnamed" ) != 0 )
	    filename = createUnnamedFileName();
	return;
    } else {
	fileNameTemp = false;
    }
    filename = fn;
    timeStamp.setFileName( filename + codeExtension() );
    timeStampUi.setFileName( filename );
    cod = "";
    loadCode();
}

void FormFile::setCode( const TQString &c )
{
    cod = c;
}

FormWindow *FormFile::formWindow() const
{
    return fw;
}

SourceEditor *FormFile::editor() const
{
    return ed;
}

TQString FormFile::fileName() const
{
    return filename;
}

TQString FormFile::absFileName() const
{
    return pro->makeAbsolute( filename );
}

TQString FormFile::codeFile() const
{
    TQString codeExt = codeExtension();
    if ( codeExt.isEmpty() )
	return "";
    return filename + codeExt;
}

TQString FormFile::code()
{
    return cod;
}

bool FormFile::save( bool withMsgBox, bool ignoreModified )
{
    if ( fileNameTemp )
	return saveAs();
    if ( !ignoreModified && !isModified() )
	return true;
    if ( ed )
	ed->save();
    if (!shouldOverwriteUi())
        return true;

    if ( formWindow() && isModified( WFormWindow ) ) {
	if ( withMsgBox ) {
	    if ( !formWindow()->checkCustomWidgets() )
		return false;
	}

	if ( TQFile::exists( pro->makeAbsolute( filename ) ) ) {
	    TQString fn( pro->makeAbsolute( filename ) );
#if defined(Q_OS_WIN32)
	    fn += ".bak";
#else
	    fn += "~";
#endif
	    TQFile f( pro->makeAbsolute( filename ) );
	    if ( f.open( IO_ReadOnly ) ) {
		TQFile f2( fn );
		if ( f2.open( IO_WriteOnly | IO_Translate ) ) {
		    TQCString data( f.size() );
		    f.readBlock( data.data(), f.size() );
		    f2.writeBlock( data );
		} else {
		    TQMessageBox::warning( MainWindow::self, "Save", "The file " +
					  codeFile() + " could not be saved" );
		}
	    }
	}
    }

    if ( isModified( WFormCode ) ) {
	if ( TQFile::exists( pro->makeAbsolute( codeFile() ) ) ) {
	    TQString fn( pro->makeAbsolute( codeFile() ) );
#if defined(Q_OS_WIN32)
	    fn += ".bak";
#else
	    fn += "~";
#endif
	    TQFile f( pro->makeAbsolute( codeFile() ) );
	    if ( f.open( IO_ReadOnly ) ) {
		TQFile f2( fn );
		if ( f2.open( IO_WriteOnly | IO_Translate) ) {
		    TQCString data( f.size() );
		    f.readBlock( data.data(), f.size() );
		    f2.writeBlock( data );
		} else if ( tqApp->type() != TQApplication::Tty ) {
		    TQMessageBox::warning( MainWindow::self, "Save", "The file " +
					  codeFile() + " could not be saved" );
		}
	    }
	}
    }

    if ( formWindow() ) {
	Resource resource( MainWindow::self );
	resource.setWidget( formWindow() );
	bool formCodeOnly = isModified( WFormCode ) && !isModified( WFormWindow );
	if ( !resource.save( pro->makeAbsolute( filename ), formCodeOnly ) ) {
	    if ( MainWindow::self )
		MainWindow::self->statusBar()->message( tr( "Failed to save file '%1'.").arg( formCodeOnly ? codeFile(): filename ),
							5000 );
	    if ( formCodeOnly )
		return false;
	    return saveAs();
	}
	if ( MainWindow::self )
	    MainWindow::self->statusBar()->message( tr( "'%1' saved.").
						    arg( formCodeOnly ? codeFile() : filename ),
						    3000 );
    } else {
	if ( !Resource::saveFormCode(this, MetaDataBase::languageInterface(pro->language())) )
	    return false;
    }
    timeStamp.update();
    timeStampUi.update();
    setModified( false );
    return true;
}

bool FormFile::shouldOverwriteUi() const
{
    if (timeStampUi.isUpToDate())
	return true;
    return (TQMessageBox::information(MainWindow::self, tr( "TQt Designer" ),
                                     tr("File '%1' has been changed outside TQt Designer.\n"
                                        "Do you want to overwrite it?" ).arg(timeStampUi.fileName()),
                                     tr("&Yes"), tr("&No")) == 0);
}

bool FormFile::saveAs( bool ignoreModified )
{
    TQString f = pro->makeAbsolute( fileName() );
    if ( fileNameTemp && formWindow() ) {
	f = TQString( formWindow()->name() ).lower();
	f.replace( "::", "_" );
	f = pro->makeAbsolute( f + ".ui" );
    }
    bool saved = false;
    if ( ignoreModified ) {
	TQString dir = TQStringList::split( ':', pro->iFace()->customSetting( "TQTSCRIPT_PACKAGES" ) ).first();
	f = TQFileInfo( f ).fileName();
	f.prepend( dir + "/" );
    }
    TQString fn;
    while ( !saved ) {
	fn = TQFileDialog::getSaveFileName( f,
					   tr( "TQt User-Interface Files (*.ui)" ) + ";;" +
					   tr( "All Files (*)" ), MainWindow::self, 0,
					   tr( "Save Form '%1' As ...").arg( formName() ),
					   MainWindow::self ? &MainWindow::self->lastSaveFilter : 0 );
	if ( fn.isEmpty() )
	    return false;
	TQFileInfo fi( fn );
	if ( fi.extension() != "ui" )
	    fn += ".ui";
	fileNameTemp = false;
	filename = pro->makeRelative( fn );
	TQFileInfo relfi( filename );
	if ( relfi.exists() ) {
	    if ( TQMessageBox::warning( MainWindow::self, tr( "File Already Exists" ),
		tr( "The file already exists. Do you wish to overwrite it?" ),
		TQMessageBox::Yes,
		TQMessageBox::No ) == TQMessageBox::Yes ) {
		saved = true;
	    } else {
		filename = f;
	    }

	} else {
	    saved = true;
	}
    }
    if ( !checkFileName( true ) ) {
	filename = f;
	return false;
    }
    pro->setModified( true );
    timeStamp.setFileName( pro->makeAbsolute( codeFile() ) );
    if ( ed && formWindow() )
	ed->setCaption( tr( "Edit %1" ).arg( formWindow()->name() ) );
    setModified( true );
    if ( pro->isDummy() )
	fw->mainWindow()->addRecentlyOpenedFile( fn );
    return save( true, ignoreModified );
}

bool FormFile::close()
{
    if ( editor() ) {
	editor()->save();
	editor()->close();
    }
    if ( formWindow() )
	return formWindow()->close();
    return true;
}

bool FormFile::closeEvent()
{
    if ( !isModified() && fileNameTemp ) {
	pro->removeFormFile( this );
	return true;
    }

    if ( !isModified() )
	return true;

    if ( editor() )
	editor()->save();

    switch ( TQMessageBox::warning( MainWindow::self, tr( "Save Form" ),
				   tr( "Save changes to form '%1'?" ).arg( filename ),
				   tr( "&Yes" ), tr( "&No" ), tr( "&Cancel" ), 0, 2 ) ) {
    case 0: // save
	if ( !save() )
	    return false;
    case 1: // don't save
	loadCode();
	if ( ed )
	    ed->editorInterface()->setText( cod );
	if ( fileNameTemp )
	    pro->removeFormFile( this );
	if ( MainWindow::self )
	    MainWindow::self->workspace()->update();
	break;
    case 2: // cancel
	return false;
    default:
	break;
    }

    setModified( false );
    if ( MainWindow::self )
	MainWindow::self->updateFunctionList();
    setCodeEdited( false );
    return true;
}

void FormFile::setModified( bool m, int who )
{
    if ( ( who & WFormWindow ) == WFormWindow )
	setFormWindowModified( m );
    if ( ( who & WFormCode ) == WFormCode )
	setCodeModified( m );
}

bool FormFile::isModified( int who )
{
    if ( who == WFormWindow )
	return isFormWindowModified();
    if ( who == WFormCode )
	return isCodeModified();
    return isCodeModified() || isFormWindowModified();
}

bool FormFile::isFormWindowModified() const
{
    if ( !formWindow()  || !formWindow()->commandHistory() )
	return false;
    return formWindow()->commandHistory()->isModified();
}

bool FormFile::isCodeModified() const
{
    if ( !editor() )
	return cm;
    return editor()->isModified();
}

void FormFile::setFormWindowModified( bool m )
{
    bool b = isFormWindowModified();
    if ( m == b )
	return;
    if ( !formWindow() || !formWindow()->commandHistory() )
	return;
    formWindow()->commandHistory()->setModified( m );
    emit somethingChanged( this );
}

void FormFile::setCodeModified( bool m )
{
    bool b = isCodeModified();
    if ( m == b )
	return;
    emit somethingChanged( this );
    cm = m;
    if ( !editor() )
	return;
    editor()->setModified( m );
}

void FormFile::showFormWindow()
{
    if ( !MainWindow::self )
	return;
    if ( formWindow() ) {
	if ( ( formWindow()->hasFocus() ||
	      MainWindow::self->qWorkspace()->activeWindow() == formWindow() ) &&
	     MainWindow::self->propertyeditor()->formWindow() != formWindow() ) {
	    MainWindow::self->propertyeditor()->setWidget( formWindow()->currentWidget(), formWindow() );
	    MainWindow::self->objectHierarchy()->setFormWindow( formWindow(),
								formWindow()->currentWidget() );
	}
	formWindow()->setFocus();
	return;
    }
    MainWindow::self->openFormWindow( pro->makeAbsolute( filename ), true, this );
}

bool FormFile::setupUihFile( bool askForUih )
{
    if ( !pro->isCpp() || !askForUih ) {
	if ( !hasFormCode() ) {
	    createFormCode();
	    setModified( true );
	}
	codeFileStat = FormFile::Ok;
	return true;
    }
    if ( codeFileStat != FormFile::Ok && !ed ) {
	if ( hasFormCode() ) {
	    int i = TQMessageBox::information( MainWindow::self, tr( "Using ui.h file" ),
	                                      tr( "An \"ui.h\" file for this form already exists.\n"
					      "Do you want to use it or create a new one?" ),
	                                      tr( "Use existing" ), tr( "Create new" ),
					      tr( "Cancel" ), 2, 2 );
	    if ( i == 2 )
		return false;
	    if ( i == 1 )
		createFormCode();
	} else {
	    if ( TQMessageBox::Yes != TQMessageBox::information( MainWindow::self, tr( "Creating ui.h file" ),
						 tr( "Do you want to create an new \"ui.h\" file?" ),
						 TQMessageBox::Yes, TQMessageBox::No ) )
		return false;
	    createFormCode();
	}
	setModified( true );
    }
    codeFileStat = FormFile::Ok;
    return true;
}

SourceEditor *FormFile::showEditor( bool askForUih )
{
    if ( !MainWindow::self )
	return 0;
    showFormWindow();
    if ( !setupUihFile( askForUih ) )
	return 0;
    SourceEditor *se = MainWindow::self->openSourceEditor();
    if ( !se )
        return 0;
    se->setActiveWindow();
    se->setFocus();
    return se;
}

static int ui_counter = 0;
TQString FormFile::createUnnamedFileName()
{
    return TQString( "unnamed" ) + TQString::number( ++ui_counter ) + TQString( ".ui" );
}

TQString FormFile::codeExtension() const
{
    LanguageInterface *iface = MetaDataBase::languageInterface( pro->language() );
    if ( iface )
	return iface->formCodeExtension();
    return "";
}

TQString FormFile::codeComment() const
{
    return TQString( "/****************************************************************************\n"
    "** ui.h extension file, included from the uic-generated form implementation.\n"
    "**\n"
    "** If you want to add, delete, or rename functions or slots, use\n"
    "** TQt Designer to update this file, preserving your code.\n"
    "**\n"
    "** You should not define a constructor or destructor in this file.\n"
    "** Instead, write your code in functions called init() and destroy().\n"
    "** These will automatically be called by the form's constructor and\n"
    "** destructor.\n"
    "*****************************************************************************/\n" );
}

bool FormFile::hasFormCode() const
{
    return !cod.isEmpty() && cod != TQString( codeComment() );
}

int FormFile::codeFileState() const
{
    return hasFormCode() ? codeFileStat : None;
}

void FormFile::setCodeFileState( UihState s )
{
    codeFileStat = s;
}

void FormFile::createFormCode()
{
    if ( !formWindow() )
	return;
    LanguageInterface *iface = MetaDataBase::languageInterface( pro->language() );
    if ( !iface )
	return;
    if ( pro->isCpp() )
	cod = codeComment();
    TQValueList<MetaDataBase::Function> functionList = MetaDataBase::functionList( formWindow() );
    for ( TQValueList<MetaDataBase::Function>::Iterator it = functionList.begin(); it != functionList.end(); ++it ) {
	cod += (!cod.isEmpty() ? "\n\n" : "") +
	       iface->createFunctionStart( formWindow()->name(), make_func_pretty((*it).function),
					   (*it).returnType.isEmpty() ?
					   TQString( "void" ) :
					   (*it).returnType, (*it).access ) +
	       "\n" + iface->createEmptyFunction();
    }
    parseCode( cod, false );
}

void FormFile::load()
{
    showFormWindow();
    code();
}

bool FormFile::loadCode()
{
    TQString fn = pro->makeAbsolute( codeFile() );
    if ( fn.isEmpty() )
        return false;

    TQFile f( fn );
    if ( !f.open( IO_ReadOnly ) ) {
	cod = "";
	setCodeFileState( FormFile::None );
	return false;
    }
    TQTextStream ts( &f );
    cod = ts.read();
    parseCode( cod, false );
    if ( hasFormCode() && codeFileStat != FormFile::Ok )
	setCodeFileState( FormFile::Deleted );
    timeStamp.update();
    return true;
}

bool FormFile::isCodeEdited() const
{
    return codeEdited;
}

void FormFile::setCodeEdited( bool b )
{
    codeEdited = b;
}

void FormFile::parseCode( const TQString &txt, bool allowModify )
{
    if ( !formWindow() )
	return;
    LanguageInterface *iface = MetaDataBase::languageInterface( pro->language() );
    if ( !iface )
	return;
    TQValueList<LanguageInterface::Function> functions;
    TQValueList<MetaDataBase::Function> newFunctions, oldFunctions;
    oldFunctions = MetaDataBase::functionList( formWindow() );
    iface->functions( txt, &functions );
    TQMap<TQString, TQString> funcs;
    for ( TQValueList<LanguageInterface::Function>::Iterator it = functions.begin();
	  it != functions.end(); ++it ) {
	bool found = false;
	for ( TQValueList<MetaDataBase::Function>::Iterator fit = oldFunctions.begin();
	      fit != oldFunctions.end(); ++fit ) {
	    TQString f( (*fit).function );
	    if ( MetaDataBase::normalizeFunction( f ) ==
		 MetaDataBase::normalizeFunction( (*it).name ) ) {
		found = true;
		MetaDataBase::Function function;
		function.function = make_func_pretty( (*it).name );
		function.specifier = (*fit).specifier;
		function.type = (*fit).type;
		if ( !pro->isCpp() )
		    function.access = (*it).access;
		else
		    function.access = (*fit).access;
		function.language = (*fit).language;
		function.returnType = (*it).returnType;
		newFunctions << function;
		funcs.insert( (*it).name, (*it).body );
		oldFunctions.remove( fit );
		break;
	    }
	}
	if ( !found ) {
	    MetaDataBase::Function function;
	    function.function = make_func_pretty( (*it).name );
	    function.specifier = "virtual";
	    function.access = "public";
	    function.language = pro->language();
	    function.returnType = (*it).returnType;
	    if ( function.returnType == "void" )
		function.type = "slot";
	    else
		function.type = "function";
	    if ( function.function == "init()" || function.function == "destroy()" ) {
		function.type = "function";
		function.access = "private";
		function.specifier = "non virtual";
	    }
	    newFunctions << function;
	    funcs.insert( (*it).name, (*it).body );
	    if ( allowModify )
		setFormWindowModified( true );
	}
    }

    if ( allowModify && oldFunctions.count() > 0 )
	setFormWindowModified( true );

    MetaDataBase::setFunctionList( formWindow(), newFunctions );
}

void FormFile::syncCode()
{
    if ( !editor() )
	return;
    parseCode( editor()->editorInterface()->text(), true );
    cod = editor()->editorInterface()->text();
}

void FormFile::checkTimeStamp()
{
    if ( timeStamp.isUpToDate() )
	return;
    timeStamp.update();
    if ( codeEdited ) {
	if ( TQMessageBox::information( MainWindow::self, tr( "TQt Designer" ),
				       tr( "File '%1' has been changed outside TQt Designer.\n"
					   "Do you want to reload it?" ).arg( timeStamp.fileName() ),
				       tr( "&Yes" ), tr( "&No" ) ) == 0 ) {
	    TQFile f( timeStamp.fileName() );
	    if ( f.open( IO_ReadOnly ) ) {
		TQTextStream ts( &f );
		editor()->editorInterface()->setText( ts.read() );
		editor()->save();
		if ( MainWindow::self )
		    MainWindow::self->functionsChanged();
	    }
	}
    } else {
	loadCode();
    }
}

bool FormFile::isUihFileUpToDate()
{
    if ( timeStamp.isUpToDate() )
	return true;
    if ( !editor() ) {
	MainWindow::self->editSource();
	parseCode( editor()->editorInterface()->text(), true );
    }
    checkTimeStamp();
    return false;
}

void FormFile::addFunctionCode( MetaDataBase::Function function )
{
    if ( pro->isCpp() && !hasFormCode() && !codeEdited )
	return;
    LanguageInterface *iface = MetaDataBase::languageInterface( pro->language() );
    if ( !iface )
	return;

    TQValueList<LanguageInterface::Function> funcs;
    iface->functions( cod, &funcs );
    bool hasFunc = false;
    for ( TQValueList<LanguageInterface::Function>::Iterator it = funcs.begin();
	  it != funcs.end(); ++it ) {
	if ( MetaDataBase::normalizeFunction( (*it).name ) == MetaDataBase::normalizeFunction( function.function ) ) {
	    hasFunc = true;
	    break;
	}
    }

    if ( !hasFunc ) {
	if ( !codeEdited && !timeStamp.isUpToDate() )
	    loadCode();
	MetaDataBase::MetaInfo mi = MetaDataBase::metaInfo( formWindow() );
	TQString cn;
	if ( mi.classNameChanged )
	    cn = mi.className;
	if ( cn.isEmpty() )
	    cn = formWindow()->name();
	TQString body = "\n\n" + iface->createFunctionStart( cn,
							    make_func_pretty( function.function ),
							    function.returnType.isEmpty() ?
							    TQString( "void" ) :
							    function.returnType, function.access ) +
		       "\n" + iface->createEmptyFunction();
	cod += body;
	if ( codeEdited ) {
	    setModified( true );
	    emit somethingChanged( this );
	}
    }
}

void FormFile::removeFunctionCode( MetaDataBase::Function function )
{
    if ( pro->isCpp() && !hasFormCode() && !codeEdited )
	return;

    LanguageInterface *iface = MetaDataBase::languageInterface( pro->language() );
    if ( !iface )
	return;

    checkTimeStamp();
    TQString sourceCode = code();
    if ( sourceCode.isEmpty() )
	return;
    TQValueList<LanguageInterface::Function> functions;
    iface->functions( sourceCode, &functions );
    TQString fu = MetaDataBase::normalizeFunction( function.function );
    for ( TQValueList<LanguageInterface::Function>::Iterator fit = functions.begin(); fit != functions.end(); ++fit ) {
	if ( MetaDataBase::normalizeFunction( (*fit).name ) == fu ) {
	    int line = 0;
	    int start = 0;
	    while ( line < (*fit).start - 1 ) {
		start = sourceCode.find( '\n', start );
		if ( start == -1 )
		    return;
		start++;
		line++;
	    }
	    if ( start == -1 )
	        return;
	    int end = start;
	    while ( line < (*fit).end + 1 ) {
		end = sourceCode.find( '\n', end );
		if ( end == -1 ) {
		    if ( line <= (*fit).end )
			end = sourceCode.length() - 1;
		    else
		        return;
		}
		end++;
		line++;
	    }
	    if ( end < start )
		return;
	    sourceCode.remove( start, end - start );
	    setCode( sourceCode );
	}
    }
}

void FormFile::functionNameChanged( const TQString &oldName, const TQString &newName )
{
    if ( !cod.isEmpty() ) {
	TQString funcStart = TQString( formWindow()->name() ) + TQString( "::" );
	int i = cod.find( funcStart + oldName );
	if ( i != -1 ) {
	    cod.remove( i + funcStart.length(), oldName.length() );
	    cod.insert( i + funcStart.length(), newName );
	}
    }
}

void FormFile::functionRetTypeChanged( const TQString &fuName, const TQString &oldType, const TQString &newType )
{
    if ( !cod.isEmpty() ) {
	TQString oldFunct = oldType + " " + TQString( formWindow()->name() ) + "::" + fuName;
	TQString newFunct = newType + " " + TQString( formWindow()->name() ) + "::" + fuName;

	int i = cod.find( oldFunct );
	if ( i != -1 ) {
	    cod.remove( i, oldFunct.length() );
	    cod.insert( i, newFunct );
	}
    }
}

TQString FormFile::formName() const
{
    FormFile* that = (FormFile*) this;
    if ( formWindow() ) {
	that->cachedFormName = formWindow()->name();
	return cachedFormName;
    }
    if ( !cachedFormName.isNull() )
	return cachedFormName;
    TQFile f( pro->makeAbsolute( filename ) );
    if ( f.open( IO_ReadOnly ) ) {
	TQTextStream ts( &f );
	TQString line;
	TQString className;
	while ( !ts.eof() ) {
	    line = ts.readLine();
	    if ( !className.isEmpty() ) {
		int end = line.find( "</class>" );
		if ( end == -1 ) {
		    className += line;
		} else {
		    className += line.left( end );
		    break;
		}
		continue;
	    }
	    int start;
	    if ( ( start = line.find( "<class>" ) ) != -1 ) {
		int end = line.find( "</class>" );
		if ( end == -1 ) {
		    className = line.mid( start + 7 );
		} else {
		    className = line.mid( start + 7, end - ( start + 7 ) );
		    break;
		}
	    }
	}
	that->cachedFormName =  className;
    }
    if ( cachedFormName.isEmpty() )
	that->cachedFormName = filename;
    return cachedFormName;
}

void FormFile::formWindowChangedSomehow()
{
    emit somethingChanged( this );
}

bool FormFile::checkFileName( bool allowBreak )
{
    FormFile *ff = pro->findFormFile( filename, this );
    if ( ff )
	TQMessageBox::warning( MainWindow::self, tr( "Invalid Filename" ),
			      tr( "The project already contains a form with a\n"
				  "filename of '%1'. Please choose a new filename." ).arg( filename ) );
    if (!formWindow()) // We are starting up
        return false;
    while ( ff ) {
	TQString fn;
	while ( fn.isEmpty() ) {
	    fn = TQFileDialog::getSaveFileName( pro->makeAbsolute( fileName() ),
					       tr( "TQt User-Interface Files (*.ui)" ) + ";;" +
					       tr( "All Files (*)" ), MainWindow::self, 0,
					       tr( "Save Form '%1' As ...").
					       arg( formWindow()->name() ),
					       MainWindow::self ? &MainWindow::self->lastSaveFilter : 0 );
	    if ( allowBreak && fn.isEmpty() )
		return false;
	}
	filename = pro->makeRelative( fn );
	ff = pro->findFormFile( filename, this );
     }
    return true;
}

void FormFile::addConnection( const TQString &sender, const TQString &signal,
			      const TQString &receiver, const TQString &slot )
{
    LanguageInterface *iface = MetaDataBase::languageInterface( pro->language() );
    if ( iface )
	iface->addConnection( sender, signal, receiver, slot, &cod );
    if ( ed )
	ed->editorInterface()->setText( cod );
}

void FormFile::removeConnection( const TQString &sender, const TQString &signal,
				 const TQString &receiver, const TQString &slot )
{
    LanguageInterface *iface = MetaDataBase::languageInterface( pro->language() );
    if ( iface )
	iface->removeConnection( sender, signal, receiver, slot, &cod );
    if ( ed )
	ed->editorInterface()->setText( cod );
}

void FormFile::notifyFormWindowChange()
{
    if ( fw )
	pro->formOpened( fw );
}
