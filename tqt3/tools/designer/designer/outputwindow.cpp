/**********************************************************************
** Copyright (C) 2001-2008 Trolltech ASA.  All rights reserved.
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

#include "outputwindow.h"
#include "designerappiface.h"
#include "metadatabase.h"
#include "mainwindow.h"

#include <ntqlistview.h>
#include <ntqtextedit.h>
#include <ntqapplication.h>
#include <ntqheader.h>
#include <stdlib.h>
#include <stdio.h>
#include <ntqpainter.h>

static TQTextEdit *debugoutput = 0;
TQ_EXPORT bool debugToStderr = false;

TQtMsgHandler OutputWindow::oldMsgHandler = 0;

OutputWindow::OutputWindow( TQWidget *parent )
    : TQTabWidget( parent, "output_window" ), debugView( 0 ), errorView( 0 )
{
    setupDebug();
    setupError();
    iface = new DesignerOutputDockImpl( this );
}

OutputWindow::~OutputWindow()
{
    debugoutput = debugView = 0;
    errorView = 0;
    if ( !debugToStderr )
	qInstallMsgHandler( oldMsgHandler );
    delete iface;
}

void OutputWindow::shuttingDown()
{
    if ( !debugToStderr )
	qInstallMsgHandler( oldMsgHandler );
}

void OutputWindow::setupError()
{
    errorView = new TQListView( this, "OutputWindow::errorView" );
    errorView->setSorting( -1 );
    connect( errorView, TQ_SIGNAL( currentChanged( TQListViewItem* ) ),
	     this, TQ_SLOT( currentErrorChanged( TQListViewItem* ) ) );
    connect( errorView, TQ_SIGNAL( clicked( TQListViewItem* ) ),
	     this, TQ_SLOT( currentErrorChanged( TQListViewItem* ) ) );

    if ( MetaDataBase::languages().count() > 1 )
	addTab( errorView, tr( "Warnings/Errors" ) );
    else
	errorView->hide();
    errorView->addColumn( tr( "Type" ) );
    errorView->addColumn( tr( "Message" ) );
    errorView->addColumn( tr( "Line" ) );
    errorView->addColumn( tr( "Location" ) );
    errorView->setResizeMode( TQListView::LastColumn );
    errorView->setColumnWidth( 0, errorView->fontMetrics().width( "WARNING1234" ) );
    errorView->setColumnWidth( 1, errorView->fontMetrics().width( "ABCDEFGHIJKLMNOPQRSTUVWXYZABCDEFGHIJKLMNOPQRSTUVWXYZABCDEFGHIJKLMNOP" ) );
    errorView->setColumnWidth( 2, errorView->fontMetrics().width( "9999999" ) );
    errorView->setColumnAlignment( 2, TQt::AlignRight );
    errorView->setAllColumnsShowFocus( true );
}

static void debugMessageOutput( TQtMsgType type, const char *msg )
{
    TQString s( msg );
    s += "\n";

    if ( type != TQtFatalMsg ) {
	if ( debugoutput && debugoutput->isVisible() )
	    debugoutput->append( s );
	else if ( OutputWindow::oldMsgHandler )
	    (*OutputWindow::oldMsgHandler)( type, msg );
	else
	    fputs( s.latin1(), stderr );
    } else {
	fputs( s.latin1(), stderr );
	abort();
    }

    tqApp->flush();
}

void OutputWindow::setupDebug()
{
    debugoutput = debugView = new TQTextEdit( this, "OutputWindow::debugView" );
    //debugView->setReadOnly( true );
    addTab( debugView, "Debug Output" );

    if ( !debugToStderr )
	oldMsgHandler = qInstallMsgHandler( debugMessageOutput );
}

void OutputWindow::setErrorMessages( const TQStringList &errors, const TQValueList<uint> &lines,
				     bool clear, const TQStringList &locations,
				     const TQObjectList &locationObjects )
{
    if ( clear )
	errorView->clear();
    TQStringList::ConstIterator mit = errors.begin();
    TQValueList<uint>::ConstIterator lit = lines.begin();
    TQStringList::ConstIterator it = locations.begin();
    TQObjectList objects = (TQObjectList)locationObjects;
    TQObject *o = objects.first();
    TQListViewItem *after = 0;
    for ( ; lit != lines.end() && mit != errors.end(); ++lit, ++mit, ++it, o = objects.next() )
	after = new ErrorItem( errorView, after, *mit, *lit, *it, o );
    setCurrentPage( 1 );
}

DesignerOutputDock *OutputWindow::iFace()
{
    return iface;
}

void OutputWindow::appendDebug( const TQString &text )
{
    debugView->append( text + "\n" );
}

void OutputWindow::clearErrorMessages()
{
    errorView->clear();
}

void OutputWindow::clearDebug()
{
    debugView->clear();
}

void OutputWindow::showDebugTab()
{
    showPage( debugView );
}

void OutputWindow::currentErrorChanged( TQListViewItem *i )
{
    if ( !i )
	return;
    ErrorItem *ei = (ErrorItem*)i;
    ei->setRead( true );
    MainWindow::self->showSourceLine( ei->location(), ei->line() - 1, MainWindow::Error );
}



ErrorItem::ErrorItem( TQListView *parent, TQListViewItem *after, const TQString &message, int line,
		      const TQString &locationString, TQObject *locationObject )
    : TQListViewItem( parent, after )
{
    setMultiLinesEnabled( true );
    TQString m( message );
    type = m.startsWith( "Warning: " ) ? Warning : Error;
    m = m.mid( m.find( ':' ) + 1 );
    setText( 0, type == Error ? "Error" : "Warning" );
    setText( 1, m );
    setText( 2, TQString::number( line ) );
    setText( 3, locationString );
    object = locationObject;
    read = !after;
    if ( !after ) {
	parent->setSelected( this, true );
	parent->setCurrentItem( this );
    }
}

void ErrorItem::paintCell( TQPainter *p, const TQColorGroup & cg,
			   int column, int width, int alignment )
{
    TQColorGroup g( cg );
    g.setColor( TQColorGroup::Text, type == Error ? TQt::red : TQt::darkYellow );
    if ( !read ) {
	TQFont f( p->font() );
	f.setBold( true );
	p->setFont( f );
    }
    TQListViewItem::paintCell( p, g, column, width, alignment );
}
