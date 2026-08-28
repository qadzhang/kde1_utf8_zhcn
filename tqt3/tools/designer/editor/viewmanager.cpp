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

#include "viewmanager.h"
#include "editor.h"
#include "markerwidget.h"
#include <ntqlayout.h>
#include <private/qrichtext_p.h>
#include "paragdata.h"
#include <ntqobjectlist.h>
#include <ntqlabel.h>
#include <ntqtimer.h>

ViewManager::ViewManager( TQWidget *parent, const char *name )
    : TQWidget( parent, name ), curView( 0 )
{
    TQHBoxLayout *l = new TQHBoxLayout( this );
    markerWidget = new MarkerWidget( this, "editor_markerwidget" );
    connect( markerWidget, TQ_SIGNAL( markersChanged() ),
	     this, TQ_SIGNAL( markersChanged() ) );
    connect( markerWidget, TQ_SIGNAL( collapseFunction( TQTextParagraph * ) ),
	     this, TQ_SIGNAL( collapseFunction( TQTextParagraph * ) ) );
    connect( markerWidget, TQ_SIGNAL( expandFunction( TQTextParagraph * ) ),
	     this, TQ_SIGNAL( expandFunction( TQTextParagraph * ) ) );
    connect( markerWidget, TQ_SIGNAL( collapse( bool ) ),
	     this, TQ_SIGNAL( collapse( bool ) ) );
    connect( markerWidget, TQ_SIGNAL( expand( bool ) ),
	     this, TQ_SIGNAL( expand( bool ) ) );
    connect( markerWidget, TQ_SIGNAL( editBreakPoints() ),
	     this, TQ_SIGNAL( editBreakPoints() ) );
    connect( markerWidget, TQ_SIGNAL( isBreakpointPossible( bool&, const TQString &, int ) ),
	     this, TQ_SIGNAL( isBreakpointPossible( bool&, const TQString &, int ) ) );
    connect( markerWidget, TQ_SIGNAL( showMessage( const TQString & ) ),
	     this, TQ_SLOT( showMessage( const TQString & ) ) );
    messageTimer = new TQTimer( this );
    connect( messageTimer, TQ_SIGNAL( timeout() ), this, TQ_SLOT( clearStatusBar() ) );
    markerWidget->setFixedWidth( fontMetrics().width( "0000" ) + 20 );
    l->addWidget( markerWidget );
    layout = new TQVBoxLayout( l );
}

void ViewManager::addView( TQWidget *view )
{
    layout->addWidget( view );
    curView = view;
    connect( ( (Editor*)curView )->verticalScrollBar(), TQ_SIGNAL( valueChanged( int ) ),
	     markerWidget, TQ_SLOT( doRepaint() ) );
    connect( (Editor*)curView, TQ_SIGNAL( textChanged() ),
	     markerWidget, TQ_SLOT( doRepaint() ) );
    connect( (Editor*)curView, TQ_SIGNAL( clearErrorMarker() ),
	     this, TQ_SLOT( clearErrorMarker() ) );
    posLabel = new TQLabel( this, "editor_poslabel" );
    posLabel->setAlignment( TQt::AlignVCenter | TQt::AlignRight );
    posLabel->setText( " Line: 1 Col: 1" );
    posLabel->setFrameStyle( TQFrame::Sunken | TQFrame::Panel );
    posLabel->setLineWidth( 1 );
    posLabel->setFixedHeight( posLabel->fontMetrics().height() );
    layout->addWidget( posLabel );
    connect( curView, TQ_SIGNAL( cursorPositionChanged( int, int ) ),
	     this, TQ_SLOT( cursorPositionChanged( int, int ) ) );
}

TQWidget *ViewManager::currentView() const
{
    return curView;
}

void ViewManager::childEvent( TQChildEvent *e )
{
    if ( e->type() == TQEvent::ChildInserted && ::tqt_cast<Editor*>(e->child()) )
	addView( (TQWidget*)e->child() );
    TQWidget::childEvent( e );
}

void ViewManager::setError( int line )
{
    TQTextParagraph *p = ( (Editor*)curView )->document()->paragAt( line );
    if ( p ) {
	( (Editor*)curView )->setErrorSelection( line );
	( (Editor*)curView )->setCursorPosition( line, 0 );
	( (Editor*)curView )->viewport()->setFocus();
	( (Editor*)curView )->makeFunctionVisible( p );
	ParagData *paragData = (ParagData*)p->extraData();
	if ( !paragData )
	    paragData = new ParagData;
	paragData->marker = ParagData::Error;
	p->setExtraData( paragData );
	markerWidget->doRepaint();
    }
}

void ViewManager::setStep( int line )
{
    TQTextParagraph *p = ( (Editor*)curView )->document()->firstParagraph();
    while ( p ) {
	if ( p->extraData() )
	    ( (ParagData*)p->extraData() )->step = false;
	p = p->next();
    }
    p = ( (Editor*)curView )->document()->paragAt( line );
    if ( p ) {
	( (Editor*)curView )->setStepSelection( line );
	( (Editor*)curView )->setCursorPosition( line, 0 );
	( (Editor*)curView )->viewport()->setFocus();
	( (Editor*)curView )->makeFunctionVisible( p );
	ParagData *paragData = (ParagData*)p->extraData();
	if ( !paragData )
	    paragData = new ParagData;
 	paragData->step = true;
	p->setExtraData( paragData );
	markerWidget->doRepaint();
    }
}

void ViewManager::clearStep()
{
    ( (Editor*)curView )->clearStepSelection();
    TQTextParagraph *p = ( (Editor*)curView )->document()->firstParagraph();
    while ( p ) {
	if ( p->extraData() )
	    ( (ParagData*)p->extraData() )->step = false;
	p = p->next();
    }
    markerWidget->doRepaint();
}

void ViewManager::setStackFrame( int line )
{
    TQTextParagraph *p = ( (Editor*)curView )->document()->paragAt( line );
    if ( p ) {
	( (Editor*)curView )->sync();
	( (Editor*)curView )->setCursorPosition( line, 0 );
	( (Editor*)curView )->ensureCursorVisible();
	( (Editor*)curView )->viewport()->setFocus();
	( (Editor*)curView )->makeFunctionVisible( p );
	ParagData *paragData = (ParagData*)p->extraData();
	if ( !paragData )
	    paragData = new ParagData;
 	paragData->stackFrame = true;
	p->setExtraData( paragData );
	markerWidget->doRepaint();
    }
}

void ViewManager::clearStackFrame()
{
    TQTextParagraph *p = ( (Editor*)curView )->document()->firstParagraph();
    while ( p ) {
	if ( p->extraData() ) {
	    ( (ParagData*)p->extraData() )->stackFrame = false;
	    if ( ( (ParagData*)p->extraData() )->marker == ParagData::Error )
		( (ParagData*)p->extraData() )->marker = ParagData::NoMarker;
	}
	p = p->next();
    }
    markerWidget->doRepaint();
}

void ViewManager::resizeEvent( TQResizeEvent *e )
{
    TQWidget::resizeEvent( e );
    markerWidget->doRepaint();
}

void ViewManager::clearErrorMarker()
{
    TQTextParagraph *p = ( (Editor*)curView )->document()->firstParagraph();
    while ( p ) {
	if ( p->extraData() )
	    ( (ParagData*)p->extraData() )->marker = ParagData::NoMarker;
	p = p->next();
    }
    markerWidget->doRepaint();
}

void ViewManager::setBreakPoints( const TQValueList<uint> &l )
{
    TQTextParagraph *p = ( (Editor*)curView )->document()->firstParagraph();
    int i = 0;
    while ( p ) {
	if ( l.find( i ) != l.end() ) {
	    if ( !p->extraData() ) {
		ParagData *data = new ParagData;
		p->setExtraData( data );
	    }
	    ParagData *data = (ParagData*)p->extraData();
	    data->marker = ParagData::Breakpoint;
	} else if ( p->extraData() ) {
	    ParagData *data = (ParagData*)p->extraData();
	    data->marker = ParagData::NoMarker;
	}
	p = p->next();
	++i;
    }
    markerWidget->doRepaint();
}

TQValueList<uint> ViewManager::breakPoints() const
{
    TQValueList<uint> l;
    int i = 0;
    TQTextParagraph *p = ( (Editor*)curView )->document()->firstParagraph();
    while ( p ) {
	if ( p->extraData() &&
	     ( (ParagData*)p->extraData() )->marker == ParagData::Breakpoint )
	    l << i;
	p = p->next();
	++i;
    }
    return l;
}

void ViewManager::showMarkerWidget( bool b )
{
    if ( b )
	markerWidget->show();
    else
	markerWidget->hide();
}

void ViewManager::emitMarkersChanged()
{
    emit markersChanged();
}

void ViewManager::cursorPositionChanged( int row, int col )
{
    posLabel->setText( TQString( " Line: %1 Col: %2" ).arg( row + 1 ).arg( col + 1 ) );
}

void ViewManager::showMessage( const TQString &msg )
{
    int row;
    int col;
    ( (TQTextEdit*)currentView() )->getCursorPosition( &row, &col );
    posLabel->setText( msg );
    messageTimer->start( 1000, true );
}

void ViewManager::clearStatusBar()
{
    int row;
    int col;
    ( (TQTextEdit*)currentView() )->getCursorPosition( &row, &col );
    posLabel->setText( TQString( " Line: %1 Col: %2" ).arg( row + 1 ).arg( col + 1 ) );
}
