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

#include "editorinterfaceimpl.h"
#include <viewmanager.h>
#include "cppeditor.h"
#include <private/qrichtext_p.h>
#include <ntqapplication.h>
#include "completion.h"
#include <designerinterface.h>
#include <ntqtimer.h>

EditorInterfaceImpl::EditorInterfaceImpl()
    : EditorInterface(), viewManager( 0 ), dIface( 0 )
{
    updateTimer = new TQTimer( this );
    connect( updateTimer, TQ_SIGNAL( timeout() ),
	     this, TQ_SLOT( update() ) );
}

EditorInterfaceImpl::~EditorInterfaceImpl()
{
    updateTimer->stop();
    delete (ViewManager*)viewManager;
    if ( dIface )
	dIface->release();
}

TQRESULT EditorInterfaceImpl::queryInterface( const TQUuid &uuid, TQUnknownInterface** iface )
{
    *iface = 0;
    if ( uuid == IID_QUnknown )
	*iface = (TQUnknownInterface*)this;
    else if ( uuid == IID_Editor )
	*iface = (EditorInterface*)this;
    else
	return TQE_NOINTERFACE;

    (*iface)->addRef();
    return TQS_OK;
}

TQWidget *EditorInterfaceImpl::editor( bool readonly,
				      TQWidget *parent, TQUnknownInterface *iface )
{
    if ( !viewManager ) {
	( (EditorInterfaceImpl*)this )->viewManager = new ViewManager( parent, 0 );
	( (EditorInterfaceImpl*)this )->viewManager->showMarkerWidget( false );
	if ( iface )
	    iface->queryInterface( IID_Designer, (TQUnknownInterface**) &dIface );
	CppEditor *e = new CppEditor( TQString::null, viewManager, "editor", dIface );
	e->setEditable( !readonly );
	e->installEventFilter( this );
	connect( e, TQ_SIGNAL( intervalChanged() ), this, TQ_SLOT( intervalChanged() ) );
	TQApplication::sendPostedEvents( viewManager, TQEvent::ChildInserted );
    }
    return viewManager->currentView();
}

void EditorInterfaceImpl::setText( const TQString &txt )
{
    if ( !viewManager || !viewManager->currentView() )
	return;
    CppEditor *e = (CppEditor*)viewManager->currentView();
    disconnect( e, TQ_SIGNAL( modificationChanged( bool ) ), this, TQ_SLOT( modificationChanged( bool ) ) );
    e->setText( txt );
    e->setModified( false );
    connect( e, TQ_SIGNAL( modificationChanged( bool ) ), this, TQ_SLOT( modificationChanged( bool ) ) );
}

TQString EditorInterfaceImpl::text() const
{
    if ( !viewManager || !viewManager->currentView() )
	return TQString::null;
    TQString txt = ( (CppEditor*)viewManager->currentView() )->text();
    if ( !txt.isEmpty() && !txt.endsWith("\n") )
	txt += "\n";
    return txt;
}

bool EditorInterfaceImpl::isUndoAvailable() const
{
    if ( !viewManager || !viewManager->currentView() )
	return false;
    return ( (CppEditor*)viewManager->currentView() )->isUndoAvailable();
}

bool EditorInterfaceImpl::isRedoAvailable() const
{
    if ( !viewManager || !viewManager->currentView() )
	return false;
    return ( (CppEditor*)viewManager->currentView() )->isRedoAvailable();
}

void EditorInterfaceImpl::undo()
{
    if ( !viewManager || !viewManager->currentView() )
	return;
    ( (CppEditor*)viewManager->currentView() )->undo();
}

void EditorInterfaceImpl::redo()
{
    if ( !viewManager || !viewManager->currentView() )
	return;
    ( (CppEditor*)viewManager->currentView() )->redo();
}

void EditorInterfaceImpl::cut()
{
    if ( !viewManager || !viewManager->currentView() )
	return;
    ( (CppEditor*)viewManager->currentView() )->cut();
}

void EditorInterfaceImpl::copy()
{
    if ( !viewManager || !viewManager->currentView() )
	return;
    ( (CppEditor*)viewManager->currentView() )->copy();
}

void EditorInterfaceImpl::paste()
{
    if ( !viewManager || !viewManager->currentView() )
	return;
    ( (CppEditor*)viewManager->currentView() )->paste();
}

void EditorInterfaceImpl::selectAll()
{
    if ( !viewManager || !viewManager->currentView() )
	return;
    ( (CppEditor*)viewManager->currentView() )->selectAll();
}

bool EditorInterfaceImpl::find( const TQString &expr, bool cs, bool wo, bool forward, bool startAtCursor )
{
    if ( !viewManager || !viewManager->currentView() )
	return false;
    CppEditor *e = (CppEditor*)viewManager->currentView();
    if ( startAtCursor )
	return e->find( expr, cs, wo, forward );
    int dummy = 0;
    return e->find( expr, cs, wo, forward, &dummy, &dummy );
}

bool EditorInterfaceImpl::replace( const TQString &find, const TQString &replace, bool cs, bool wo,
				   bool forward, bool startAtCursor, bool replaceAll )
{
    if ( !viewManager || !viewManager->currentView() )
	return false;
    CppEditor *e = (CppEditor*)viewManager->currentView();
    bool ok = false;
    if ( startAtCursor ) {
	ok = e->find( find, cs, wo, forward );
    } else {
	int dummy = 0;
	ok =  e->find( find, cs, wo, forward, &dummy, &dummy );
    }

    if ( ok ) {
	e->removeSelectedText();
	e->insert( replace, false, false );
    }

    if ( !replaceAll || !ok ) {
	if ( ok )
	    e->setSelection( e->textCursor()->paragraph()->paragId(),
			     e->textCursor()->index() - replace.length(),
			     e->textCursor()->paragraph()->paragId(),
			     e->textCursor()->index() );
	return ok;
    }

    bool ok2 = true;
    while ( ok2 ) {
	ok2 = e->find( find, cs, wo, forward );
	if ( ok2 ) {
	    e->removeSelectedText();
	    e->insert( replace, false, false );
	}
    }

    return true;
}

void EditorInterfaceImpl::gotoLine( int line )
{
    if ( !viewManager || !viewManager->currentView() )
	return;
    ( (CppEditor*)viewManager->currentView() )->setCursorPosition( line, 0 );
}

void EditorInterfaceImpl::indent()
{
    if ( !viewManager || !viewManager->currentView() )
	return;
    ( (CppEditor*)viewManager->currentView() )->TQTextEdit::indent();
}

void EditorInterfaceImpl::splitView()
{
#if 0
    if ( !viewManager || !viewManager->currentView() )
	return;
    TQTextDocument *doc = ( (CppEditor*)viewManager->currentView() )->document();
    CppEditor *editor = new CppEditor( TQString::null, viewManager, "editor" );
    editor->setDocument( doc );
#endif
}

void EditorInterfaceImpl::scrollTo( const TQString &txt, const TQString & )
{
    if ( !viewManager || !viewManager->currentView() )
	return;
    ( (CppEditor*)viewManager->currentView() )->sync();
    TQTextDocument *doc = ( (CppEditor*)viewManager->currentView() )->document();
    TQTextParagraph *p = doc->firstParagraph();
    while ( p ) {
	if ( p->string()->toString().find( txt ) != -1 ) {
	    ( (CppEditor*)viewManager->currentView() )->setCursorPosition( p->paragId() + 2, 0 );
	    break;
	}
	p = p->next();
    }
    ( (CppEditor*)viewManager->currentView() )->setFocus();
}

void EditorInterfaceImpl::setContext( TQObject *this_ )
{
    if ( !viewManager || !viewManager->currentView() )
	return;
    ( (CppEditor*)viewManager->currentView() )->completionManager()->setContext( this_ );
}

void EditorInterfaceImpl::setError( int line )
{
    if ( !viewManager )
	return;
    viewManager->setError( line );
}

void EditorInterfaceImpl::readSettings()
{
    if ( !viewManager )
	return;
    ( (CppEditor*)viewManager->currentView() )->configChanged();
}

void EditorInterfaceImpl::modificationChanged( bool m )
{
    if ( viewManager && dIface )
	dIface->setModified( m, viewManager->currentView() );
}

void EditorInterfaceImpl::setModified( bool m )
{
    if ( !viewManager )
	return;
    ( (CppEditor*)viewManager->currentView() )->setModified( m );
}

bool EditorInterfaceImpl::isModified() const
{
    if ( !viewManager )
	return false;
    return ( (CppEditor*)viewManager->currentView() )->isModified();
}

bool EditorInterfaceImpl::eventFilter( TQObject *o, TQEvent *e )
{
    if ( e->type() == TQEvent::KeyPress )
	updateTimer->stop();
    else if ( e->type() == TQEvent::FocusOut )
	update();

    return TQObject::eventFilter( o, e );
}

int EditorInterfaceImpl::numLines() const
{
    if ( !viewManager || !viewManager->currentView() )
	return 0;
    return ( (CppEditor*)viewManager->currentView() )->paragraphs();
}

void EditorInterfaceImpl::intervalChanged()
{
    if ( !dIface )
	return;
    updateTimer->start( 2000, true );
}

void EditorInterfaceImpl::update()
{
    if ( !dIface )
	return;
    dIface->updateFunctionList();
}

void EditorInterfaceImpl::onBreakPointChange( TQObject *receiver, const char *slot )
{
    if ( !viewManager )
	return;
    connect( viewManager, TQ_SIGNAL( markersChanged() ), receiver, slot );
}
