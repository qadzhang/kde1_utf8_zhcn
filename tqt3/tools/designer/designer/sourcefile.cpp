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

#include "sourcefile.h"
#include <ntqfile.h>
#include <ntqtextstream.h>
#include "designerappiface.h"
#include "sourceeditor.h"
#include "metadatabase.h"
#include "../interfaces/languageinterface.h"
#include <ntqfiledialog.h>
#include <ntqmessagebox.h>
#include "mainwindow.h"
#include "workspace.h"
#include <stdlib.h>

SourceFile::SourceFile( const TQString &fn, bool temp, Project *p )
    : filename( fn ), ed( 0 ), fileNameTemp( temp ),
      timeStamp( 0, p->makeAbsolute( fn ) ), pro( p ), pkg( false )
      , accepted( true )
{
    iface = 0;
    
    if ( !temp )
	accepted = checkFileName( true );
    
    if (accepted) {
	load();
	pro->addSourceFile( this );
	MetaDataBase::addEntry( this );
    }
    
}

SourceFile::~SourceFile()
{
    if (iface)
	delete iface;
}

TQString SourceFile::text() const
{
    return txt;
}

void SourceFile::setText( const TQString &s )
{
    txt = s;
}

bool SourceFile::save( bool ignoreModified )
{
    if ( fileNameTemp )
	return saveAs();
    if ( !ignoreModified && !isModified() )
	return true;
    if ( ed )
	ed->save();

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
	    }
	}
    }

    TQFile f( pro->makeAbsolute( filename ) );
    if ( !f.open( IO_WriteOnly | IO_Translate ) )
	return saveAs();

    TQTextStream ts( &f );
    ts << txt;
    timeStamp.update();
    setModified( false );
    return true;
}

bool SourceFile::saveAs( bool ignoreModified )
{
    LanguageInterface *iface = MetaDataBase::languageInterface( pro->language() );
    TQString filter;
    if ( iface )
	filter = iface->fileFilterList().join(";;");

    TQString old = filename;
    TQString initFn = pro->makeAbsolute( filename );
    if ( ignoreModified ) {
	TQString dir = TQStringList::split( ':', project()->iFace()->customSetting( "TQTSCRIPT_PACKAGES" ) ).first();
	initFn = TQFileInfo( initFn ).fileName();
	initFn.prepend( dir + "/" );
    }
    TQString fn = TQFileDialog::getSaveFileName( initFn, filter );
    if ( fn.isEmpty() )
	return false;
    fileNameTemp = false;
    filename = pro->makeRelative( fn );
    if ( !checkFileName( true ) ) {
	filename = old;
	return false;
    }
    pro->setModified( true );
    timeStamp.setFileName( pro->makeAbsolute( filename ) );
    if ( ed )
	ed->setCaption( tr( "Edit %1" ).arg( filename ) );
    setModified( true );
    if ( pro->isDummy() ) {
	TQObject *o = ed->parent();
	while ( o && !o->isA( "MainWindow" ) )
	    o = o->parent();
	if ( o )
	    ((MainWindow *)o)->addRecentlyOpenedFile( fn );
    }
    return save( ignoreModified );
}

bool SourceFile::load()
{
    TQFile f( pro->makeAbsolute( filename ) );
    if ( !f.open( IO_ReadOnly ) )
	return false;
    TQTextStream ts( &f );
    txt = ts.read();
    timeStamp.update();
    return true;
}

DesignerSourceFile *SourceFile::iFace()
{
    if ( !iface )
	iface = new DesignerSourceFileImpl( this );
    return iface;
}

void SourceFile::setEditor( SourceEditor *e )
{
    ed = e;
}

bool SourceFile::isModified() const
{
    if ( !ed )
	return false;
    return ed->isModified();
}

static TQMap<TQString, int> *extensionCounter;
TQString SourceFile::createUnnamedFileName( const TQString &extension )
{
    if ( !extensionCounter )
	extensionCounter = new TQMap<TQString, int>;
    int count = -1;
    TQMap<TQString, int>::Iterator it;
    if ( ( it = extensionCounter->find( extension ) ) != extensionCounter->end() ) {
	count = *it;
	++count;
	extensionCounter->replace( extension, count );
    } else {
	count = 1;
	extensionCounter->insert( extension, count );
    }

    return "unnamed" + TQString::number( count ) + "." + extension;
}

void SourceFile::setModified( bool m )
{
    if ( !ed )
	return;
    ed->setModified( m );
}

bool SourceFile::closeEvent()
{
    if ( !isModified() && fileNameTemp ) {
	pro->removeSourceFile( this );
	return true;
    }

    if ( !isModified() )
	return true;

    if ( ed )
	ed->save();

    switch ( TQMessageBox::warning( MainWindow::self, tr( "Save Code" ),
				   tr( "Save changes to '%1'?" ).arg( filename ),
				   tr( "&Yes" ), tr( "&No" ), tr( "&Cancel" ), 0, 2 ) ) {
    case 0: // save
	if ( !save() )
	    return false;
	break;
    case 1: // don't save
	load();
	if ( ed )
	    ed->editorInterface()->setText( txt );
	if ( fileNameTemp ) {
	    pro->removeSourceFile( this );
	    return true;
	}
	if ( MainWindow::self )
	    MainWindow::self->workspace()->update();
	break;
    case 2: // cancel
	return false;
    default:
	break;
    }
    setModified( false );
    return true;
}

bool SourceFile::close()
{
    if ( !ed )
	return true;
    return ed->close();
}

Project *SourceFile::project() const
{
    return pro;
}

void SourceFile::checkTimeStamp()
{
    if ( timeStamp.isUpToDate() )
	return;
    timeStamp.update();
    if ( TQMessageBox::information( MainWindow::self, tr( "TQt Designer" ),
				   tr( "File '%1' has been changed outside TQt Designer.\n"
				       "Do you want to reload it?" ).arg( filename ),
				   tr( "&Yes" ), tr( "&No" ) ) == 0 ) {
	load();
	if ( ed )
	    ed->editorInterface()->setText( txt );
    }
}

bool SourceFile::checkFileName( bool allowBreak )
{
    SourceFile *sf = pro->findSourceFile( filename, this );
    if ( sf )
	TQMessageBox::warning( MainWindow::self, tr( "Invalid Filename" ),
			      tr( "The project already contains a source file with \n"
				  "filename '%1'. Please choose a new filename." ).arg( filename ) );
    while ( sf ) {
	LanguageInterface *iface = MetaDataBase::languageInterface( pro->language() );
	TQString filter;
	if ( iface )
	    filter = iface->fileFilterList().join(";;");
	TQString fn;
	while ( fn.isEmpty() ) {
	    fn = TQFileDialog::getSaveFileName( pro->makeAbsolute( filename ), filter );
	    if ( allowBreak && fn.isEmpty() )
		return false;
	}
	filename = pro->makeRelative( fn );
	sf = pro->findSourceFile( filename, this );
    }
    return true;
}
