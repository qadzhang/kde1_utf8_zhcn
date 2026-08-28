/**********************************************************************
**
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

#include "cppeditor.h"
#include "syntaxhighliter_cpp.h"
#include <cindent.h>
#include "cppcompletion.h"
#include "cppbrowser.h"
#include <parenmatcher.h>
#include <ntqsettings.h>
#include <ntqpopupmenu.h>
#include <ntqinputdialog.h>
#include <designerinterface.h>

CppEditor::CppEditor( const TQString &fn, TQWidget *parent, const char *name, DesignerInterface *i )
    : Editor( fn, parent, name ), dIface( i )
{
    if ( dIface )
	dIface->addRef();
    document()->setPreProcessor( new SyntaxHighlighter_CPP );
    document()->setIndent( (indent = new CIndent) );
    completion = new CppEditorCompletion( this );
    browser = new CppEditorBrowser( this );
    int j = 0;
    while ( SyntaxHighlighter_CPP::keywords[ j ] != TQString::null )
	    completion->addCompletionEntry( SyntaxHighlighter_CPP::keywords[ j++ ], 0, false );
    configChanged();
}

CppEditor::~CppEditor()
{
    delete completion;
    if ( dIface )
	dIface->release();
}

void CppEditor::configChanged()
{
    TQString path = "/Trolltech/CppEditor/";
    TQMap<TQString, ConfigStyle> styles = Config::readStyles( path );
    config()->styles = styles;
    ( (SyntaxHighlighter_CPP*)document()->preProcessor() )->updateStyles( config()->styles );

    completion->setEnabled( Config::completion( path ) );
    parenMatcher->setEnabled( Config::parenMatching( path ) );
    if ( Config::wordWrap( path ) ) {
	if ( hScrollBarMode() != AlwaysOff ) {
	    document()->setFormatter( new TQTextFormatterBreakInWords );
	    setHScrollBarMode( AlwaysOff );
	}
    } else {
	if ( hScrollBarMode() != AlwaysOn ) {
	    TQTextFormatterBreakWords *f = new TQTextFormatterBreakWords;
	    f->setWrapEnabled( false );
	    document()->setFormatter( f );
	    setHScrollBarMode( AlwaysOn );
	}
    }

    setFont( ( (SyntaxHighlighter_CPP*)document()->preProcessor() )->format( 0 )->font() );

    indent->setTabSize( Config::indentTabSize( path ) );
    indent->setIndentSize( Config::indentIndentSize( path ) );
    indent->setKeepTabs( Config::indentKeepTabs( path ) );
    indent->setAutoIndent( Config::indentAutoIndent( path ) );
    if ( !Config::indentAutoIndent( path ) )
	document()->setIndent( 0 );
    else
	document()->setIndent( indent );

    document()->setTabStops( ( (SyntaxHighlighter_CPP*)document()->preProcessor() )->format( TQTextPreProcessor::Standard )->width( 'x' ) * Config::indentTabSize( path ) );

    Editor::configChanged();
}

TQPopupMenu *CppEditor::createPopupMenu( const TQPoint &p )
{
    TQPopupMenu *m = Editor::createPopupMenu( p );
    m->insertSeparator();
    int adddeclid = m->insertItem( tr( "Add Include File (in Declaration)..." ), this, TQ_SLOT( addInclDecl() ) );
    int addimplid = m->insertItem( tr( "Add Include File (in Implementation)..." ), this, TQ_SLOT( addInclImpl() ) );
    int addforid = m->insertItem( tr( "Add Forward Declaration..." ), this, TQ_SLOT( addForward() ) );
    if ( !dIface->currentForm() ) {
	m->setItemEnabled( adddeclid, false );
	m->setItemEnabled( addimplid, false );
	m->setItemEnabled( addforid, false );
    }
    return m;
}

void CppEditor::addInclDecl()
{
    if ( !dIface )
	return;
    TQString s = TQInputDialog::getText( tr( "Add Include File (In Declaration)" ),
				       tr( "Input this using the format <b>&lt;include.h&gt;</b> or <b>\"include.h\"</b>" ) );
    if ( s.isEmpty() )
	return;
    DesignerFormWindow *form = dIface->currentForm();
    TQStringList lst = form->declarationIncludes();
    lst << s;
    form->setDeclarationIncludes( lst );
}

void CppEditor::addInclImpl()
{
    if ( !dIface )
	return;
    TQString s = TQInputDialog::getText( tr( "Add Include File (In Implementation)" ),
				       tr( "Input this using the format <b>&lt;include.h&gt;</b> or <b>\"include.h\"</b>" ) );
    if ( s.isEmpty() )
	return;
    DesignerFormWindow *form = dIface->currentForm();
    TQStringList lst = form->implementationIncludes();
    lst << s;
    form->setImplementationIncludes( lst );
}

void CppEditor::addForward()
{
    if ( !dIface )
	return;
    TQString s = TQInputDialog::getText( tr( "Add Forward Declaration" ),
				       tr( "Input this using the format <b>ClassName;</b>" ) );
    if ( s.isEmpty() )
	return;
    DesignerFormWindow *form = dIface->currentForm();
    TQStringList lst = form->forwardDeclarations();
    lst << s;
    form->setForwardDeclarations( lst );
}

void CppEditor::paste()
{
    Editor::paste();
    emit intervalChanged();
}
