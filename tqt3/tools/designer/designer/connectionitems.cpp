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

#include "connectionitems.h"
#include "formwindow.h"
#include "mainwindow.h"
#include "metadatabase.h"
#include "widgetfactory.h"
#include "project.h"
#include <ntqpainter.h>
#include <ntqcombobox.h>
#include <ntqmetaobject.h>
#include <ntqcombobox.h>
#include <ntqlistbox.h>
#include <ntqaction.h>
#include <ntqdatabrowser.h>

static const char* const ignore_slots[] = {
    "destroyed()",
    "setCaption(const TQString&)",
    "setIcon(const TQPixmap&)",
    "setIconText(const TQString&)",
    "setMouseTracking(bool)",
    "clearFocus()",
    "setUpdatesEnabled(bool)",
    "update()",
    "update(int,int,int,int)",
    "update(const TQRect&)",
    "repaint()",
    "repaint(bool)",
    "repaint(int,int,int,int,bool)",
    "repaint(const TQRect&,bool)",
    "repaint(const TQRegion&,bool)",
    "show()",
    "hide()",
    "iconify()",
    "showMinimized()",
    "showMaximized()",
    "showFullScreen()",
    "showNormal()",
    "polish()",
    "constPolish()",
    "raise()",
    "lower()",
    "stackUnder(TQWidget*)",
    "move(int,int)",
    "move(const TQPoint&)",
    "resize(int,int)",
    "resize(const TQSize&)",
    "setGeometry(int,int,int,int)",
    "setGeometry(const TQRect&)",
    "focusProxyDestroyed()",
    "showExtension(bool)",
    "setUpLayout()",
    "showDockMenu(const TQPoint&)",
    "init()",
    "destroy()",
    "deleteLater()",
    0
};

ConnectionItem::ConnectionItem( TQTable *table, FormWindow *fw )
    : TQComboTableItem( table, TQStringList(), false ), formWindow( fw ), conn( 0 )
{
    setReplaceable( false );
}

void ConnectionItem::senderChanged( TQObject * )
{
    emit changed();
    TQWidget *w = table()->cellWidget( row(), col() );
    if ( w )
	setContentFromEditor( w );
}

void ConnectionItem::receiverChanged( TQObject * )
{
    emit changed();
    TQWidget *w = table()->cellWidget( row(), col() );
    if ( w )
	setContentFromEditor( w );
}

void ConnectionItem::signalChanged( const TQString & )
{
    emit changed();
    TQWidget *w = table()->cellWidget( row(), col() );
    if ( w )
	setContentFromEditor( w );
}

void ConnectionItem::slotChanged( const TQString & )
{
    emit changed();
    TQWidget *w = table()->cellWidget( row(), col() );
    if ( w )
	setContentFromEditor( w );
}

void ConnectionItem::setSender( SenderItem *i )
{
    connect( i, TQ_SIGNAL( currentSenderChanged( TQObject * ) ),
	     this, TQ_SLOT( senderChanged( TQObject * ) ) );
}

void ConnectionItem::setReceiver( ReceiverItem *i )
{
    connect( i, TQ_SIGNAL( currentReceiverChanged( TQObject * ) ),
	     this, TQ_SLOT( receiverChanged( TQObject * ) ) );
}

void ConnectionItem::setSignal( SignalItem *i )
{
    connect( i, TQ_SIGNAL( currentSignalChanged( const TQString & ) ),
	     this, TQ_SLOT( signalChanged( const TQString & ) ) );
}

void ConnectionItem::setSlot( SlotItem *i )
{
    connect( i, TQ_SIGNAL( currentSlotChanged( const TQString & ) ),
	     this, TQ_SLOT( slotChanged( const TQString & ) ) );
}

void ConnectionItem::paint( TQPainter *p, const TQColorGroup &cg,
			    const TQRect &cr, bool selected )
{
    p->fillRect( 0, 0, cr.width(), cr.height(),
		 selected ? cg.brush( TQColorGroup::Highlight )
			  : cg.brush( TQColorGroup::Base ) );

    int w = cr.width();
    int h = cr.height();

    int x = 0;

    if ( currentText()[0] == '<' )
	p->setPen( TQObject::red );
    else if ( selected )
	p->setPen( cg.highlightedText() );
    else
	p->setPen( cg.text() );

    TQFont f( p->font() );
    TQFont oldf( p->font() );
    if ( conn && conn->isModified() ) {
	f.setBold( true );
	p->setFont( f );
    }

    p->drawText( x + 2, 0, w - x - 4, h, alignment(), currentText() );
    p->setFont( oldf );
}

void ConnectionItem::setConnection( ConnectionContainer *c )
{
    conn = c;
}

// ------------------------------------------------------------------

static void appendChildActions( TQAction *action, TQStringList &lst )
{
    TQObjectListIt it( *action->children() );
    while ( it.current() ) {
	TQObject *o = it.current();
	++it;
	if ( !::tqt_cast<TQAction*>(o) )
	    continue;
	lst << o->name();
	if ( o->children() && ::tqt_cast<TQActionGroup*>(o) )
	    appendChildActions( (TQAction*)o, lst );
    }
}

static TQStringList flatActions( const TQPtrList<TQAction> &l )
{
    TQStringList lst;

    TQPtrListIterator<TQAction> it( l );
    while ( it.current() ) {
	TQAction *action = it.current();
	lst << action->name();
	if ( action->children() && ::tqt_cast<TQActionGroup*>(action) )
	    appendChildActions( action, lst );
	++it;
    }

    return lst;
}

// ------------------------------------------------------------------

SenderItem::SenderItem( TQTable *table, FormWindow *fw )
    : ConnectionItem( table, fw )
{
    TQStringList lst;

    TQPtrDictIterator<TQWidget> it( *formWindow->widgets() );
    while ( it.current() ) {
	if ( lst.find( it.current()->name() ) != lst.end() ) {
	    ++it;
	    continue;
	}
	if ( !TQString( it.current()->name() ).startsWith( "qt_dead_widget_" ) &&
	     !::tqt_cast<TQLayoutWidget*>(it.current()) &&
	     !::tqt_cast<Spacer*>(it.current()) &&
	     !::tqt_cast<SizeHandle*>(it.current()) &&
	     qstrcmp( it.current()->name(), "central widget" ) != 0 ) {
	    lst << it.current()->name();
	}
	++it;
    }

    lst += flatActions( formWindow->actionList() );

    lst.prepend( "<No Sender>" );
    lst.sort();
    setStringList( lst );
}

TQWidget *SenderItem::createEditor() const
{
    TQComboBox *cb = (TQComboBox*)ConnectionItem::createEditor();
    cb->listBox()->setMinimumWidth( cb->fontMetrics().width( "01234567890123456789012345678901234567890123456789" ) );
    connect( cb, TQ_SIGNAL( activated( const TQString & ) ),
	     this, TQ_SLOT( senderChanged( const TQString & ) ) );
    return cb;
}

void SenderItem::setSenderEx( TQObject *sender )
{
    setCurrentItem( sender->name() );
    emit currentSenderChanged( sender );
}

void SenderItem::senderChanged( const TQString &sender )
{
    TQObject *o = formWindow->child( sender, "TQObject" );
    if ( !o )
	o = formWindow->findAction( sender );
    if ( !o )
	return;
    emit currentSenderChanged( o );
}



// ------------------------------------------------------------------

ReceiverItem::ReceiverItem( TQTable *table, FormWindow *fw )
    : ConnectionItem( table, fw )
{
    TQStringList lst;

    TQPtrDictIterator<TQWidget> it( *formWindow->widgets() );
    while ( it.current() ) {
	if ( lst.find( it.current()->name() ) != lst.end() ) {
	    ++it;
	    continue;
	}
	if ( !TQString( it.current()->name() ).startsWith( "qt_dead_widget_" ) &&
	     !::tqt_cast<TQLayoutWidget*>(it.current()) &&
	     !::tqt_cast<Spacer*>(it.current()) &&
	     !::tqt_cast<SizeHandle*>(it.current()) &&
	     qstrcmp( it.current()->name(), "central widget" ) != 0 ) {
	    lst << it.current()->name();
	}
	++it;
    }

    lst += flatActions( formWindow->actionList() );

    lst.prepend( "<No Receiver>" );
    lst.sort();
    setStringList( lst );
}

TQWidget *ReceiverItem::createEditor() const
{
    TQComboBox *cb = (TQComboBox*)ConnectionItem::createEditor();
    cb->listBox()->setMinimumWidth( cb->fontMetrics().width( "01234567890123456789012345678901234567890123456789" ) );
    connect( cb, TQ_SIGNAL( activated( const TQString & ) ),
	     this, TQ_SLOT( receiverChanged( const TQString & ) ) );
    return cb;
}

void ReceiverItem::setReceiverEx( TQObject *receiver )
{
    setCurrentItem( receiver->name() );
    emit currentReceiverChanged( receiver );
}

void ReceiverItem::receiverChanged( const TQString &receiver )
{
    TQObject *o = formWindow->child( receiver, "TQObject" );
    if ( !o )
	o = formWindow->findAction( receiver );
    if ( !o )
	return;
    emit currentReceiverChanged( o );
}



// ------------------------------------------------------------------

SignalItem::SignalItem( TQTable *table, FormWindow *fw )
    : ConnectionItem( table, fw )
{
    TQStringList lst;
    lst << "<No Signal>";
    lst.sort();
    setStringList( lst );
}

void SignalItem::senderChanged( TQObject *sender )
{
    TQStrList sigs = sender->metaObject()->signalNames( true );
    sigs.remove( "destroyed()" );
    sigs.remove( "destroyed(TQObject*)" );
    sigs.remove( "accessibilityChanged(int)" );
    sigs.remove( "accessibilityChanged(int,int)" );

    TQStringList lst = TQStringList::fromStrList( sigs );

    if ( ::tqt_cast<CustomWidget*>(sender) ) {
	MetaDataBase::CustomWidget *w = ( (CustomWidget*)sender )->customWidget();
	for ( TQValueList<TQCString>::Iterator it = w->lstSignals.begin();
	      it != w->lstSignals.end(); ++it )
	    lst << MetaDataBase::normalizeFunction( *it );
    }

    if ( sender == formWindow->mainContainer() ) {
	TQStringList extra = MetaDataBase::signalList( formWindow );
	if ( !extra.isEmpty() )
	    lst += extra;
    }

    lst.prepend( "<No Signal>" );

    lst.sort();
    setStringList( lst );

    ConnectionItem::senderChanged( sender );
}

TQWidget *SignalItem::createEditor() const
{
    TQComboBox *cb = (TQComboBox*)ConnectionItem::createEditor();
    cb->listBox()->setMinimumWidth( cb->fontMetrics().width( "01234567890123456789012345678901234567890123456789" ) );
    connect( cb, TQ_SIGNAL( activated( const TQString & ) ),
	     this, TQ_SIGNAL( currentSignalChanged( const TQString & ) ) );
    return cb;
}

// ------------------------------------------------------------------

SlotItem::SlotItem( TQTable *table, FormWindow *fw )
    : ConnectionItem( table, fw )
{
    TQStringList lst;
    lst << "<No Slot>";
    lst.sort();
    setStringList( lst );

    lastReceiver = 0;
    lastSignal = "<No Signal>";
}

void SlotItem::receiverChanged( TQObject *receiver )
{
    lastReceiver = receiver;
    updateSlotList();
    ConnectionItem::receiverChanged( receiver );
}

void SlotItem::signalChanged( const TQString &signal )
{
    lastSignal = signal;
    updateSlotList();
    ConnectionItem::signalChanged( signal );
}

bool SlotItem::ignoreSlot( const char* slot ) const
{
#ifndef TQT_NO_SQL
    if ( qstrcmp( slot, "update()" ) == 0 &&
	 ::tqt_cast<TQDataBrowser*>(lastReceiver) )
	return false;
#endif

    for ( int i = 0; ignore_slots[i]; i++ ) {
	if ( qstrcmp( slot, ignore_slots[i] ) == 0 )
	    return true;
    }

    if ( !formWindow->isMainContainer( (TQWidget*)lastReceiver ) ) {
	if ( qstrcmp( slot, "close()" ) == 0  )
	    return true;
    }

    if ( qstrcmp( slot, "setFocus()" ) == 0  )
	if ( lastReceiver->isWidgetType() &&
	     ( (TQWidget*)lastReceiver )->focusPolicy() == TQWidget::NoFocus )
	    return true;

    return false;
}

void SlotItem::updateSlotList()
{
    TQStringList lst;
    if ( !lastReceiver || lastSignal == "<No Signal>" ) {
	lst << "<No Slot>";
	lst.sort();
	setStringList( lst );
	return;
    }

    TQString signal = MetaDataBase::normalizeFunction( lastSignal );
    int n = lastReceiver->metaObject()->numSlots( true );
    TQStringList slts;

    for( int i = 0; i < n; ++i ) {
	// accept only public slots. For the form window, also accept protected slots
	const TQMetaData* md = lastReceiver->metaObject()->slot( i, true );
	if ( ( (lastReceiver->metaObject()->slot( i, true )->access == TQMetaData::Public) ||
	       (formWindow->isMainContainer( (TQWidget*)lastReceiver ) &&
		lastReceiver->metaObject()->slot(i, true)->access ==
		TQMetaData::Protected) ) &&
	     !ignoreSlot( md->name ) &&
	     checkConnectArgs( signal.latin1(), lastReceiver, md->name ) )
	    if ( lst.find( md->name ) == lst.end() )
		lst << MetaDataBase::normalizeFunction( md->name );
    }

    LanguageInterface *iface =
	MetaDataBase::languageInterface( formWindow->project()->language() );
    if ( !iface || iface->supports( LanguageInterface::ConnectionsToCustomSlots ) ) {
	if ( formWindow->isMainContainer( (TQWidget*)lastReceiver ) ) {
	    TQValueList<MetaDataBase::Function> moreSlots = MetaDataBase::slotList( formWindow );
	    if ( !moreSlots.isEmpty() ) {
		for ( TQValueList<MetaDataBase::Function>::Iterator it = moreSlots.begin();
		      it != moreSlots.end(); ++it ) {
		    TQCString s = (*it).function;
		    if ( !s.data() )
			continue;
		    s = MetaDataBase::normalizeFunction( s );
		    if ( checkConnectArgs( signal.latin1(), lastReceiver, s ) ) {
			if ( lst.find( (*it).function ) == lst.end() )
			    lst << s;
		    }
		}
	    }
	}
    }

    if ( ::tqt_cast<CustomWidget*>(lastReceiver) ) {
	MetaDataBase::CustomWidget *w = ( (CustomWidget*)lastReceiver )->customWidget();
	for ( TQValueList<MetaDataBase::Function>::Iterator it = w->lstSlots.begin();
	      it != w->lstSlots.end(); ++it ) {
	    TQCString s = (*it).function;
	    if ( !s.data() )
		continue;
	    s = MetaDataBase::normalizeFunction( s );
	    if ( checkConnectArgs( signal.latin1(), lastReceiver, s ) ) {
		if ( lst.find( (*it).function ) == lst.end() )
		    lst << s;
	    }
	}
    }

    lst.prepend( "<No Slot>" );
    lst.sort();
    setStringList( lst );
}

TQWidget *SlotItem::createEditor() const
{
    TQComboBox *cb = (TQComboBox*)ConnectionItem::createEditor();
    cb->listBox()->setMinimumWidth( cb->fontMetrics().width( "01234567890123456789012345678901234567890123456789" ) );
    connect( cb, TQ_SIGNAL( activated( const TQString & ) ),
	     this, TQ_SIGNAL( currentSlotChanged( const TQString & ) ) );
    return cb;
}

void SlotItem::customSlotsChanged()
{
    TQString currSlot = currentText();
    updateSlotList();
    setCurrentItem( "<No Slot>" );
    setCurrentItem( currSlot );
    emit currentSlotChanged( currentText() );
}
