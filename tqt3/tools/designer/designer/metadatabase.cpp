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

#include "metadatabase.h"
#include "widgetfactory.h"
#include "formwindow.h"
#include "parser.h"
#include "widgetdatabase.h"
#include "formfile.h"
#include "propertyobject.h"
#include "project.h"
#include "mainwindow.h"

#include <ntqapplication.h>
#include <ntqobject.h>
#include <ntqlayout.h>
#include <ntqptrdict.h>
#include <ntqobjectlist.h>
#include <ntqstrlist.h>
#include <ntqmetaobject.h>
#include <ntqwidgetlist.h>
#include <ntqmainwindow.h>
#include <ntqregexp.h>
#include <private/qpluginmanager_p.h>
#include <ntqdatetime.h>
#include <ntqfile.h>
#include <ntqfileinfo.h>
#include <ntqtextstream.h>

#include <stdlib.h>

class MetaDataBaseRecord
{
public:
    TQObject *object;
    TQStringList changedProperties;
    TQStringVariantMap fakeProperties;
    TQMap<TQString, TQString> propertyComments;
    int spacing, margin;
    TQString resizeMode;
    TQValueList<MetaDataBase::Connection> connections;
    TQValueList<MetaDataBase::Function> functionList;
    TQValueList<MetaDataBase::Include> includes;
    TQValueList<MetaDataBase::Variable> variables;
    TQStringList forwards, sigs;
    TQWidgetList tabOrder;
    MetaDataBase::MetaInfo metaInfo;
    TQCursor cursor;
    TQMap<int, TQString> pixmapArguments;
    TQMap<int, TQString> pixmapKeys;
    TQMap<TQString, TQString> columnFields;
    TQValueList<uint> breakPoints;
    TQMap<int, TQString> breakPointConditions;
    TQString exportMacro;
};

static TQPtrDict<MetaDataBaseRecord> *db = 0;
static TQPtrList<MetaDataBase::CustomWidget> *cWidgets = 0;
static bool doUpdate = true;
static TQStringList langList;
static TQStringList editorLangList;
static TQPluginManager<LanguageInterface> *languageInterfaceManager = 0;

/*!
  \class MetaDataBase metadatabase.h
  \brief Database which stores meta data of widgets

  The MetaDataBase stores meta information of widgets, which are not
  stored directly in widgets (properties). This is e.g. the
  information which properties have been modified.
*/

MetaDataBase::MetaDataBase()
{
}

inline void setupDataBase()
{
    if ( !db || !cWidgets ) {
	db = new TQPtrDict<MetaDataBaseRecord>( 1481 );
	db->setAutoDelete( true );
	cWidgets = new TQPtrList<MetaDataBase::CustomWidget>;
	cWidgets->setAutoDelete( true );
    }
}

void MetaDataBase::clearDataBase()
{
    delete db;
    db = 0;
    delete cWidgets;
    cWidgets = 0;
}

void MetaDataBase::addEntry( TQObject *o )
{
    if ( !o )
	return;
    setupDataBase();
    if ( db->find( o ) )
	return;
    MetaDataBaseRecord *r = new MetaDataBaseRecord;
    r->object = o;
    r->spacing = r->margin = -1;
    db->insert( (void*)o, r );

    WidgetFactory::initChangedProperties( o );
}

void MetaDataBase::removeEntry( TQObject *o )
{
    setupDataBase();
    db->remove( o );
}

void MetaDataBase::setPropertyChanged( TQObject *o, const TQString &property, bool changed )
{
    setupDataBase();
    if ( o->isA( "PropertyObject" ) ) {
	( (PropertyObject*)o )->mdPropertyChanged( property, changed );
	return;
    }
    MetaDataBaseRecord *r = db->find( (void*)o );
    if ( !r ) {
	tqWarning( "No entry for %p (%s, %s) found in MetaDataBase",
		  o, o->name(), o->className() );
	return;
    }

    if ( changed ) {
	if ( r->changedProperties.findIndex( property ) == -1 )
	    r->changedProperties.append( property );
    } else {
	if ( r->changedProperties.findIndex( property ) != -1 )
	    r->changedProperties.remove( property );
    }

    if ( doUpdate &&
	 ( property == "hAlign" || property == "vAlign" || property == "wordwrap" ) ) {
	doUpdate = false;
	setPropertyChanged( o, "alignment", changed ||
			    isPropertyChanged( o, "hAlign" ) ||
			    isPropertyChanged( o, "vAlign" ) ||
			    isPropertyChanged( o, "wordwrap" ) );
	doUpdate = true;
    }

    if ( doUpdate && property == "alignment" ) {
	doUpdate = false;
	setPropertyChanged( o, "hAlign", changed );
	setPropertyChanged( o, "vAlign", changed );
	setPropertyChanged( o, "wordwrap", changed );
	doUpdate = true;
    }
}

bool MetaDataBase::isPropertyChanged( TQObject *o, const TQString &property )
{
    setupDataBase();
    if ( o->isA( "PropertyObject" ) )
	return ( (PropertyObject*)o )->mdIsPropertyChanged( property );
    MetaDataBaseRecord *r = db->find( (void*)o );
    if ( !r ) {
	tqWarning( "No entry for %p (%s, %s) found in MetaDataBase",
		  o, o->name(), o->className() );
	return false;
    }

    return r->changedProperties.findIndex( property ) != -1;
}

TQStringList MetaDataBase::changedProperties( TQObject *o )
{
    setupDataBase();
    MetaDataBaseRecord *r = db->find( (void*)o );
    if ( !r ) {
	tqWarning( "No entry for %p (%s, %s) found in MetaDataBase",
		  o, o->name(), o->className() );
	return TQStringList();
    }

    TQStringList lst( r->changedProperties );
    return lst;
}

void MetaDataBase::setPropertyComment( TQObject *o, const TQString &property, const TQString &comment )
{
    setupDataBase();
    if ( o->isA( "PropertyObject" ) ) {
	( (PropertyObject*)o )->mdSetPropertyComment( property, comment );
	return;
    }
    MetaDataBaseRecord *r = db->find( (void*)o );
    if ( !r ) {
	tqWarning( "No entry for %p (%s, %s) found in MetaDataBase",
		  o, o->name(), o->className() );
	return;
    }

    r->propertyComments.insert( property, comment );
}

TQString MetaDataBase::propertyComment( TQObject *o, const TQString &property )
{
    setupDataBase();
    if ( o->isA( "PropertyObject" ) )
	return ( (PropertyObject*)o )->mdPropertyComment( property );
    MetaDataBaseRecord *r = db->find( (void*)o );
    if ( !r ) {
	tqWarning( "No entry for %p (%s, %s) found in MetaDataBase",
		  o, o->name(), o->className() );
	return TQString::null;
    }

    return *r->propertyComments.find( property );
}

void MetaDataBase::setFakeProperty( TQObject *o, const TQString &property, const TQVariant& value )
{
    setupDataBase();
    if ( o->isA( "PropertyObject" ) ) {
	( (PropertyObject*)o )->mdSetFakeProperty( property, value );
	return;
    }
    MetaDataBaseRecord *r = db->find( (void*)o );
    if ( !r ) {
	tqWarning( "No entry for %p (%s, %s) found in MetaDataBase",
		  o, o->name(), o->className() );
	return;
    }
    r->fakeProperties[property] = value;
}

TQVariant MetaDataBase::fakeProperty( TQObject * o, const TQString &property)
{
    setupDataBase();
    if ( o->isA( "PropertyObject" ) )
	return ( (PropertyObject*)o )->mdFakeProperty( property );
    MetaDataBaseRecord *r = db->find( (void*)o );
    if ( !r ) {
	tqWarning( "No entry for %p (%s, %s) found in MetaDataBase",
		  o, o->name(), o->className() );
	return TQVariant();
    }
    TQStringVariantMap::Iterator it = r->fakeProperties.find( property );
    if ( it != r->fakeProperties.end() )
	return r->fakeProperties[property];
    return WidgetFactory::defaultValue( o, property );

}

TQStringVariantMap* MetaDataBase::fakeProperties( TQObject* o )
{
    setupDataBase();
    MetaDataBaseRecord *r = db->find( (void*)o );
    if ( !r ) {
	tqWarning( "No entry for %p (%s, %s) found in MetaDataBase",
		  o, o->name(), o->className() );
	return 0;
    }
    return &r->fakeProperties;
}

void MetaDataBase::setSpacing( TQObject *o, int spacing )
{
    if ( !o )
	return;
    setupDataBase();
    MetaDataBaseRecord *r = db->find( (void*)o );
    if ( !r || !o->isWidgetType() ) {
	tqWarning( "No entry for %p (%s, %s) found in MetaDataBase",
		  o, o->name(), o->className() );
	return;
    }

    r->spacing = spacing;
    TQLayout * layout = 0;
    WidgetFactory::layoutType( (TQWidget*)o, layout );
    if ( layout ) {
	int spadef = 6;
	if ( MainWindow::self->formWindow() )
	    spadef = MainWindow::self->formWindow()->layoutDefaultSpacing();
	if ( spacing == -1 )
	    layout->setSpacing( spadef );
	else
	    layout->setSpacing( spacing );
    }
}

int MetaDataBase::spacing( TQObject *o )
{
    if ( !o )
	return -1;
    setupDataBase();
    if ( ::tqt_cast<TQMainWindow*>(o) )
	o = ( (TQMainWindow*)o )->centralWidget();
    MetaDataBaseRecord *r = db->find( (void*)o );
    if ( !r || !o->isWidgetType() ) {
	tqWarning( "No entry for %p (%s, %s) found in MetaDataBase",
		  o, o->name(), o->className() );
	return -1;
    }

    return r->spacing;
}

void MetaDataBase::setMargin( TQObject *o, int margin )
{
    if ( !o )
	return;
    setupDataBase();
    MetaDataBaseRecord *r = db->find( (void*)o );
    if ( !r || !o->isWidgetType() ) {
	tqWarning( "No entry for %p (%s, %s) found in MetaDataBase",
		  o, o->name(), o->className() );
	return;
    }

    r->margin = margin;
    TQLayout * layout = 0;
    WidgetFactory::layoutType( (TQWidget*)o, layout );

    bool isInnerLayout = true;

    TQWidget *widget = (TQWidget*)o;
    if ( widget && !::tqt_cast<TQLayoutWidget*>(widget) &&
	( WidgetDatabase::isContainer( WidgetDatabase::idFromClassName( WidgetFactory::classNameOf( widget ) ) ) ||
	  ( widget && widget->parentWidget() && ::tqt_cast<FormWindow*>(widget->parentWidget()) ) ) )
	isInnerLayout = false;


    if ( layout ) {
	int mardef = 11;
	if ( MainWindow::self->formWindow() )
	    mardef = MainWindow::self->formWindow()->layoutDefaultMargin();
	if ( margin == -1 ) {
	    if ( isInnerLayout )
		layout->setMargin( 1 );
	    else
		layout->setMargin( TQMAX( 1, mardef ) );
	}
	else
	    layout->setMargin( TQMAX( 1, margin ) );
    }
}

int MetaDataBase::margin( TQObject *o )
{
    if ( !o )
	return -1;
    setupDataBase();
    if ( ::tqt_cast<TQMainWindow*>(o) )
	o = ( (TQMainWindow*)o )->centralWidget();
    MetaDataBaseRecord *r = db->find( (void*)o );
    if ( !r || !o->isWidgetType() ) {
	tqWarning( "No entry for %p (%s, %s) found in MetaDataBase",
		  o, o->name(), o->className() );
	return -1;
    }
    return r->margin;
}

void MetaDataBase::setResizeMode( TQObject *o, const TQString &mode )
{
    if ( !o )
	return;
    setupDataBase();
    MetaDataBaseRecord *r = db->find( (void*)o );
    if ( !r || !o->isWidgetType() ) {
	tqWarning( "No entry for %p (%s, %s) found in MetaDataBase",
		  o, o->name(), o->className() );
	return;
    }

    r->resizeMode = mode;
}

TQString MetaDataBase::resizeMode( TQObject *o )
{
    if ( !o )
	return TQString::null;
    setupDataBase();
    if ( ::tqt_cast<TQMainWindow*>(o) )
	o = ( (TQMainWindow*)o )->centralWidget();
    MetaDataBaseRecord *r = db->find( (void*)o );
    if ( !r || !o->isWidgetType() ) {
	tqWarning( "No entry for %p (%s, %s) found in MetaDataBase",
		  o, o->name(), o->className() );
	return TQString::null;
    }
    return r->resizeMode;
}

void MetaDataBase::addConnection( TQObject *o, TQObject *sender, const TQCString &signal,
				  TQObject *receiver, const TQCString &slot, bool addCode )
{
    setupDataBase();
    MetaDataBaseRecord *r = db->find( (void*)o );
    if ( !r ) {
	tqWarning( "No entry for %p (%s, %s) found in MetaDataBase",
		  o, o->name(), o->className() );
	return;
    }
    if ( !(sender && receiver) )
	return;
    Connection conn;
    conn.sender = sender;
    conn.signal = signal;
    conn.receiver = receiver;
    conn.slot = slot;
    r->connections.append( conn );
    if ( addCode ) {
	TQString rec = receiver->name();
	if ( ::tqt_cast<FormWindow*>(o) && receiver == ( (FormWindow*)o )->mainContainer() )
	    rec = "this";
	TQString sen = sender->name();
	if ( ::tqt_cast<FormWindow*>(o) && sender == ( (FormWindow*)o )->mainContainer() )
	    sen = "this";
	FormFile *ff = 0;
	if ( ::tqt_cast<FormFile*>(o) )
	    ff = (FormFile*)o;
	else if ( ::tqt_cast<FormWindow*>(o) )
	    ff = ( (FormWindow*)o )->formFile();
	ff->addConnection( sen, signal, rec, slot );
    }
}

void MetaDataBase::removeConnection( TQObject *o, TQObject *sender, const TQCString &signal,
				     TQObject *receiver, const TQCString &slot )
{
    setupDataBase();
    MetaDataBaseRecord *r = db->find( (void*)o );
    if ( !r ) {
	tqWarning( "No entry for %p (%s, %s) found in MetaDataBase",
		  o, o->name(), o->className() );
	return;
    }
    if ( !(sender && receiver) )
	return;
    for ( TQValueList<Connection>::Iterator it = r->connections.begin(); it != r->connections.end(); ++it ) {
	Connection conn = *it;
	if ( conn.sender == sender &&
	     conn.signal == signal &&
	     conn.receiver == receiver &&
	     conn.slot == slot ) {
	    r->connections.remove( it );
	    break;
	}
    }
    if ( ::tqt_cast<FormWindow*>(o) ) {
	TQString rec = receiver->name();
	if ( receiver == ( (FormWindow*)o )->mainContainer() )
	    rec = "this";
	( (FormWindow*)o )->formFile()->removeConnection( sender->name(), signal, rec, slot );
    }
}

void MetaDataBase::setupConnections( TQObject *o, const TQValueList<LanguageInterface::Connection> &conns )
{
    setupDataBase();
    MetaDataBaseRecord *r = db->find( (void*)o );
    if ( !r ) {
	tqWarning( "No entry for %p (%s, %s) found in MetaDataBase",
		  o, o->name(), o->className() );
	return;
    }

    if ( !::tqt_cast<FormFile*>(o) )
	return;

    FormFile *formfile = (FormFile*)o;

    r->connections.clear();

    for ( TQValueList<LanguageInterface::Connection>::ConstIterator cit = conns.begin();
	  cit != conns.end(); ++cit ) {
	// #### get the correct sender object out of Bla.Blub.sender
	TQString senderName = (*cit).sender;
	if ( senderName.find( '.' ) != -1 )
	    senderName = senderName.mid( senderName.findRev( '.' ) + 1 );
	TQObject *sender = 0;
	if ( formfile->formWindow() )
	    sender = formfile->formWindow()->child( senderName );
	if ( !sender && formfile->isFake() )
	    sender = formfile->project()->objectForFakeFormFile( formfile );
	if ( !sender && senderName == "this" )
	    sender = formfile->formWindow() ?
		     formfile->formWindow()->mainContainer() :
		     formfile->project()->objectForFakeFormFile( formfile );
	if ( !sender )
	    continue;
	MetaDataBase::addConnection( formfile->formWindow() ?
				     (TQObject*)formfile->formWindow() :
				     (TQObject*)formfile,
				     sender,
				     (*cit).signal.latin1(),
				     formfile->formWindow() ?
				     formfile->formWindow()->mainContainer() :
				     formfile->project()->objectForFakeFormFile( formfile ),
				     (*cit).slot.latin1(),
				     false );
    }
}

bool MetaDataBase::hasConnection( TQObject *o, TQObject *sender, const TQCString &signal,
				  TQObject *receiver, const TQCString &slot )
{
    setupDataBase();
    MetaDataBaseRecord *r = db->find( (void*)o );
    if ( !r ) {
	tqWarning( "No entry for %p (%s, %s) found in MetaDataBase",
		  o, o->name(), o->className() );
	return false;
    }

    for ( TQValueList<Connection>::Iterator it = r->connections.begin(); it != r->connections.end(); ++it ) {
	Connection conn = *it;
	if ( conn.sender == sender &&
	     conn.signal == signal &&
	     conn.receiver == receiver &&
	     conn.slot == slot )
	    return true;
    }
    return false;
}


TQValueList<MetaDataBase::Connection> MetaDataBase::connections( TQObject *o )
{
    setupDataBase();
    MetaDataBaseRecord *r = db->find( (void*)o );
    if ( !r ) {
	tqWarning( "No entry for %p (%s, %s) found in MetaDataBase",
		  o, o->name(), o->className() );
	return TQValueList<Connection>();
    }
    return r->connections;
}

TQValueList<MetaDataBase::Connection> MetaDataBase::connections( TQObject *o, TQObject *sender,
								TQObject *receiver )
{
    setupDataBase();
    MetaDataBaseRecord *r = db->find( (void*)o );
    if ( !r ) {
	tqWarning( "No entry for %p (%s, %s) found in MetaDataBase",
		  o, o->name(), o->className() );
	return TQValueList<Connection>();
    }
    TQValueList<Connection>::Iterator it = r->connections.begin();
    TQValueList<Connection> ret;
    TQValueList<Connection>::Iterator conn;
    while ( ( conn = it ) != r->connections.end() ) {
	++it;
	if ( (*conn).sender == sender &&
	     (*conn).receiver == receiver )
	    ret << *conn;
    }

    return ret;
}

TQValueList<MetaDataBase::Connection> MetaDataBase::connections( TQObject *o, TQObject *object )
{
    setupDataBase();
    MetaDataBaseRecord *r = db->find( (void*)o );
    if ( !r ) {
	tqWarning( "No entry for %p (%s, %s) found in MetaDataBase",
		  o, o->name(), o->className() );
	return TQValueList<Connection>();
    }
    TQValueList<Connection>::Iterator it = r->connections.begin();
    TQValueList<Connection> ret;
    TQValueList<Connection>::Iterator conn;
    while ( ( conn = it ) != r->connections.end() ) {
	++it;
	if ( (*conn).sender == object ||
	     (*conn).receiver == object )
	    ret << *conn;
    }
    return ret;
}

void MetaDataBase::doConnections( TQObject *o )
{
    setupDataBase();
    MetaDataBaseRecord *r = db->find( (void*)o );
    if ( !r ) {
	tqWarning( "No entry for %p (%s, %s) found in MetaDataBase",
		  o, o->name(), o->className() );
	return;
    }

    TQObject *sender = 0, *receiver = 0;
    TQObjectList *l = 0;
    TQValueList<Connection>::Iterator it = r->connections.begin();
    for ( ; it != r->connections.end(); ++it ) {
	Connection conn = *it;
	if ( qstrcmp( conn.sender->name(), o->name() ) == 0 ) {
	    sender = o;
	} else {
	    l = o->queryList( 0, conn.sender->name(), false );
	    if ( !l || !l->first() ) {
		delete l;
		continue;
	    }
	    sender = l->first();
	    delete l;
	}
	if ( qstrcmp( conn.receiver->name(), o->name() ) == 0 ) {
	    receiver = o;
	} else {
	    l = o->queryList( 0, conn.receiver->name(), false );
	    if ( !l || !l->first() ) {
		delete l;
		continue;
	    }
	    receiver = l->first();
	    delete l;
	}
	TQString s = "2""%1";
	s = s.arg( conn.signal );
	TQString s2 = "1""%1";
	s2 = s2.arg( conn.slot );

	TQStrList signalList = sender->metaObject()->signalNames( true );
	TQStrList slotList = receiver->metaObject()->slotNames( true );

	// avoid warnings
	if ( signalList.find( conn.signal ) == -1 ||
	     slotList.find( conn.slot ) == -1 )
	    continue;

	TQObject::connect( sender, s, receiver, s2 );
    }
}

bool MetaDataBase::hasSlot( TQObject *o, const TQCString &slot, bool onlyCustom )
{
    setupDataBase();
    MetaDataBaseRecord *r = db->find( (void*)o );
    if ( !r ) {
	tqWarning( "No entry for %p (%s, %s) found in MetaDataBase",
		  o, o->name(), o->className() );
	return false;
    }

    if ( !onlyCustom ) {
	TQStrList slotList = o->metaObject()->slotNames( true );
	if ( slotList.find( slot ) != -1 )
	    return true;

	if ( ::tqt_cast<FormWindow*>(o) ) {
	    o = ( (FormWindow*)o )->mainContainer();
	    slotList = o->metaObject()->slotNames( true );
	    if ( slotList.find( slot ) != -1 )
		return true;
	}

	//if ( ::tqt_cast<CustomWidget*>(o) ) {
	if ( o->inherits( "CustomWidget" ) ) {
	    MetaDataBase::CustomWidget *w = ( (::CustomWidget*)o )->customWidget();
	    for ( TQValueList<Function>::Iterator it = w->lstSlots.begin(); it != w->lstSlots.end(); ++it ) {
		TQCString s = (*it).function;
		if ( !s.data() )
		    continue;
		if ( s == slot )
		    return true;
	    }
	}
    }

    for ( TQValueList<Function>::Iterator it = r->functionList.begin(); it != r->functionList.end(); ++it ) {
	Function f = *it;
	if ( normalizeFunction( f.function ) == normalizeFunction( slot ) && f.type == "slot" )
	    return true;
    }

    return false;
}

bool MetaDataBase::isSlotUsed( TQObject *o, const TQCString &slot )
{
    setupDataBase();
    MetaDataBaseRecord *r = db->find( (void*)o );
    if ( !r ) {
	tqWarning( "No entry for %p (%s, %s) found in MetaDataBase",
	          o, o->name(), o->className() );
	return false;
    }

    TQValueList<Connection> conns = connections( o );
    for ( TQValueList<Connection>::Iterator it = conns.begin(); it != conns.end(); ++it ) {
	if ( (*it).slot == slot )
	return true;
    }
    return false;
}


void MetaDataBase::addFunction( TQObject *o, const TQCString &function, const TQString &specifier,
				const TQString &access, const TQString &type, const TQString &language,
				const TQString &returnType )
{
    setupDataBase();
    MetaDataBaseRecord *r = db->find( (void*)o );
    if ( !r ) {
	tqWarning( "No entry for %p (%s, %s) found in MetaDataBase",
		  o, o->name(), o->className() );
	return;
    }

    Function f;
    f.function = function;
    f.specifier = specifier;
    f.access = access;
    f.type = type;
    f.language = language;
    f.returnType = returnType;
    TQValueList<MetaDataBase::Function>::Iterator it = r->functionList.find( f );
    if ( it != r->functionList.end() )
	r->functionList.remove( it );
    r->functionList.append( f );
    ( (FormWindow*)o )->formFile()->addFunctionCode( f );
}

void MetaDataBase::setFunctionList( TQObject *o, const TQValueList<Function> &functionList )
{
    setupDataBase();
    MetaDataBaseRecord *r = db->find( (void*)o );
    if ( !r ) {
	tqWarning( "No entry for %p (%s, %s) found in MetaDataBase",
		  o, o->name(), o->className() );
	return;
    }
    r->functionList = functionList;
}

void MetaDataBase::removeFunction( TQObject *o, const TQCString &function, const TQString &specifier,
				   const TQString &access, const TQString &type, const TQString &language,
				   const TQString &returnType )
{
    setupDataBase();
    MetaDataBaseRecord *r = db->find( (void*)o );
    if ( !r ) {
	tqWarning( "No entry for %p (%s, %s) found in MetaDataBase",
		  o, o->name(), o->className() );
	return;
    }
    for ( TQValueList<Function>::Iterator it = r->functionList.begin(); it != r->functionList.end(); ++it ) {
	if ( MetaDataBase::normalizeFunction( (*it).function ) ==
	     MetaDataBase::normalizeFunction( function ) &&
	     (*it).specifier == specifier &&
	     (*it).access == access &&
	     (*it).type == type &&
	     ( language.isEmpty() || (*it).language == language ) &&
	       ( returnType.isEmpty() || (*it).returnType == returnType ) ) {
	    ( (FormWindow*)o )->formFile()->removeFunctionCode( *it );
	    r->functionList.remove( it );
	    break;
	}
    }
}

void MetaDataBase::removeFunction( TQObject *o, const TQString &function )
{
    setupDataBase();
    MetaDataBaseRecord *r = db->find( (void*)o );
    if ( !r ) {
	tqWarning( "No entry for %p (%s, %s) found in MetaDataBase",
		  o, o->name(), o->className() );
	return;
    }
    for ( TQValueList<Function>::Iterator it = r->functionList.begin(); it != r->functionList.end(); ++it ) {
	if ( normalizeFunction( (*it).function ) == normalizeFunction( function ) ) {
	    ( (FormWindow*)o )->formFile()->removeFunctionCode( *it );
	    r->functionList.remove( it );
	    break;
	}
    }
}

TQValueList<MetaDataBase::Function> MetaDataBase::functionList( TQObject *o, bool onlyFunctions )
{
    setupDataBase();
    MetaDataBaseRecord *r = db->find( (void*)o );
    if ( !r ) {
	tqWarning( "No entry for %p (%s, %s) found in MetaDataBase",
		  o, o->name(), o->className() );
	return TQValueList<Function>();
    }
    if ( !onlyFunctions )
	return r->functionList;
    TQValueList<Function> fList;
    for ( TQValueList<Function>::Iterator it = r->functionList.begin(); it != r->functionList.end(); ++it ) {
	if ( (*it).type == "function" )
	    fList.append( *it );
    }
    return fList;
}

TQValueList<MetaDataBase::Function> MetaDataBase::slotList( TQObject *o )
{
    setupDataBase();
    MetaDataBaseRecord *r = db->find( (void*)o );
    if ( !r ) {
	tqWarning( "No entry for %p (%s, %s) found in MetaDataBase",
		  o, o->name(), o->className() );
	return TQValueList<Function>();
    }
    TQValueList<Function> slotList;
    for ( TQValueList<Function>::Iterator it = r->functionList.begin(); it != r->functionList.end(); ++it ) {
	if ( (*it).type == "slot" )
	    slotList.append( *it );
    }
    return slotList;
}

void MetaDataBase::changeFunction( TQObject *o, const TQString &function, const TQString &newName,
				   const TQString &returnType )
{
    setupDataBase();
    MetaDataBaseRecord *r = db->find( (void*)o );
    if ( !r ) {
	tqWarning( "No entry for %p (%s, %s) found in MetaDataBase",
		  o, o->name(), o->className() );
	return;
    }

    for ( TQValueList<Function>::Iterator it = r->functionList.begin(); it != r->functionList.end(); ++it ) {
	Function f = *it;
	if ( normalizeFunction( f.function ) == normalizeFunction( function ) ) {
	    (*it).function = newName;
	    if ( !returnType.isNull() )
		(*it).returnType = returnType;
	    return;
	}
    }
}

void MetaDataBase::changeFunctionAttributes( TQObject *o, const TQString &oldName, const TQString &newName,
					     const TQString &specifier, const TQString &access,
					     const TQString &type, const TQString &language,
					     const TQString &returnType )
{
    setupDataBase();
    MetaDataBaseRecord *r = db->find( (void*)o );
    if ( !r ) {
	tqWarning( "No entry for %p (%s, %s) found in MetaDataBase",
		  o, o->name(), o->className() );
	return;
    }

    for ( TQValueList<Function>::Iterator it = r->functionList.begin(); it != r->functionList.end(); ++it ) {
	Function f = *it;
	if ( normalizeFunction( f.function ) == normalizeFunction( oldName ) ) {
	    (*it).function = newName;
	    (*it).specifier = specifier;
	    (*it).access = access;
	    (*it).type = type;
	    (*it).language = language;
	    (*it).returnType = returnType;
	    return;
	}
    }
}

bool MetaDataBase::hasFunction( TQObject *o, const TQCString &function, bool onlyCustom )
{
    setupDataBase();
    MetaDataBaseRecord *r = db->find( (void*)o );
    if ( !r ) {
	tqWarning( "No entry for %p (%s, %s) found in MetaDataBase",
		  o, o->name(), o->className() );
	return false;
    }

    if ( !onlyCustom ) {
	TQStrList functionList = o->metaObject()->slotNames( true );
	if ( functionList.find( function ) != -1 )
	    return true;

	if ( ::tqt_cast<FormWindow*>(o) ) {
	    o = ( (FormWindow*)o )->mainContainer();
	    functionList = o->metaObject()->slotNames( true );
	    if ( functionList.find( function ) != -1 )
		return true;
	}

	//if ( ::tqt_cast<CustomWidget*>(o) ) {
	if ( o->inherits( "CustomWidget" ) ) {
	    MetaDataBase::CustomWidget *w = ( (::CustomWidget*)o )->customWidget();
	    for ( TQValueList<Function>::Iterator it = w->lstSlots.begin(); it != w->lstSlots.end(); ++it ) {
		TQCString s = (*it).function;
		if ( !s.data() )
		    continue;
		if ( s == function )
		    return true;
	    }
	}
    }

    for ( TQValueList<Function>::Iterator it = r->functionList.begin(); it != r->functionList.end(); ++it ) {
	Function f = *it;
	if ( normalizeFunction( f.function ) == normalizeFunction( function ) )
	    return true;
    }

    return false;
}

TQString MetaDataBase::languageOfFunction( TQObject *o, const TQCString &function )
{
    setupDataBase();
    MetaDataBaseRecord *r = db->find( (void*)o );
    if ( !r ) {
	tqWarning( "No entry for %p (%s, %s) found in MetaDataBase",
		  o, o->name(), o->className() );
	return TQString::null;
    }

    TQString fu = normalizeFunction( function );
    for ( TQValueList<Function>::Iterator it = r->functionList.begin(); it != r->functionList.end(); ++it ) {
	if ( fu == normalizeFunction( (*it).function ) )
	    return (*it).language;
    }
    return TQString::null;
}

bool MetaDataBase::addCustomWidget( CustomWidget *wid )
{
    setupDataBase();

    for ( CustomWidget *w = cWidgets->first(); w; w = cWidgets->next() ) {
	if ( *wid == *w ) {
	    for ( TQValueList<TQCString>::ConstIterator it = wid->lstSignals.begin(); it != wid->lstSignals.end(); ++it ) {
		if ( !w->hasSignal( *it ) )
		    w->lstSignals.append( *it );
	    }
	    for ( TQValueList<Function>::ConstIterator it2 = wid->lstSlots.begin(); it2 != wid->lstSlots.end(); ++it2 ) {
		if ( !w->hasSlot( MetaDataBase::normalizeFunction( (*it2).function ).latin1() ) )
		    w->lstSlots.append( *it2 );
	    }
	    for ( TQValueList<Property>::ConstIterator it3 = wid->lstProperties.begin(); it3 != wid->lstProperties.end(); ++it3 ) {
		if ( !w->hasProperty( (*it3).property ) )
		    w->lstProperties.append( *it3 );
	    }
	    delete wid;
	    return false;
	}
    }


    WidgetDatabaseRecord *r = new WidgetDatabaseRecord;
    r->name = wid->className;
    r->includeFile = wid->includeFile;
    r->group = WidgetDatabase::widgetGroup( "Custom" );
    r->toolTip = wid->className;
    r->icon = new TQIconSet( *wid->pixmap, *wid->pixmap );
    r->isContainer = wid->isContainer;
    wid->id = WidgetDatabase::addCustomWidget( r );
    cWidgets->append( wid );
    return true;
}

void MetaDataBase::removeCustomWidget( CustomWidget *w )
{
    cWidgets->removeRef( w );
}

TQPtrList<MetaDataBase::CustomWidget> *MetaDataBase::customWidgets()
{
    setupDataBase();
    return cWidgets;
}

MetaDataBase::CustomWidget *MetaDataBase::customWidget( int id )
{
    for ( CustomWidget *w = cWidgets->first(); w; w = cWidgets->next() ) {
	if ( id == w->id )
	    return w;
    }
    return 0;
}

bool MetaDataBase::isWidgetNameUsed( CustomWidget *wid )
{
    for ( CustomWidget *w = cWidgets->first(); w; w = cWidgets->next() ) {
	if ( w == wid )
	    continue;
	if ( wid->className == w->className )
	    return true;
    }
    return false;
}

bool MetaDataBase::hasCustomWidget( const TQString &className )
{
    for ( CustomWidget *w = cWidgets->first(); w; w = cWidgets->next() ) {
	if ( w->className == className )
	    return true;
    }
    return false;
}

void MetaDataBase::setTabOrder( TQWidget *w, const TQWidgetList &order )
{
    setupDataBase();
    MetaDataBaseRecord *r = db->find( (void*) w );
    if ( !r ) {
	tqWarning( "No entry for %p (%s, %s) found in MetaDataBase",
		  w, w->name(), w->className() );
	return;
    }

    r->tabOrder = order;
}

TQWidgetList MetaDataBase::tabOrder( TQWidget *w )
{
    setupDataBase();
    MetaDataBaseRecord *r = db->find( (void*) w );
    if ( !r ) {
	tqWarning( "No entry for %p (%s, %s) found in MetaDataBase",
		  w, w->name(), w->className() );
	return TQWidgetList();
    }

    return r->tabOrder;
}

void MetaDataBase::setIncludes( TQObject *o, const TQValueList<Include> &incs )
{
    setupDataBase();
    MetaDataBaseRecord *r = db->find( (void*)o );
    if ( !r ) {
	tqWarning( "No entry for %p (%s, %s) found in MetaDataBase",
		  o, o->name(), o->className() );
	return;
    }

    r->includes = incs;
}

TQValueList<MetaDataBase::Include> MetaDataBase::includes( TQObject *o )
{
    setupDataBase();
    MetaDataBaseRecord *r = db->find( (void*)o );
    if ( !r ) {
	tqWarning( "No entry for %p (%s, %s) found in MetaDataBase",
		  o, o->name(), o->className() );
	return TQValueList<Include>();
    }

    return r->includes;
}

void MetaDataBase::setForwards( TQObject *o, const TQStringList &fwds )
{
    setupDataBase();
    MetaDataBaseRecord *r = db->find( (void*)o );
    if ( !r ) {
	tqWarning( "No entry for %p (%s, %s) found in MetaDataBase",
		  o, o->name(), o->className() );
	return;
    }

    r->forwards = fwds;
}

TQStringList MetaDataBase::forwards( TQObject *o )
{
    setupDataBase();
    MetaDataBaseRecord *r = db->find( (void*)o );
    if ( !r ) {
	tqWarning( "No entry for %p (%s, %s) found in MetaDataBase",
		  o, o->name(), o->className() );
	return TQStringList();
    }

    return r->forwards;
}

void MetaDataBase::setVariables( TQObject *o, const TQValueList<Variable> &vars )
{
    setupDataBase();
    MetaDataBaseRecord *r = db->find( (void*)o );
    if ( !r ) {
	tqWarning( "No entry for %p (%s, %s) found in MetaDataBase",
		  o, o->name(), o->className() );
	return;
    }

    r->variables = vars;
}

void MetaDataBase::addVariable( TQObject *o, const TQString &name, const TQString &access )
{
    setupDataBase();
    MetaDataBaseRecord *r = db->find( (void*)o );
    if ( !r ) {
	tqWarning( "No entry for %p (%s, %s) found in MetaDataBase",
		  o, o->name(), o->className() );
	return;
    }
    Variable v;
    v.varName = name;
    v.varAccess = access;
    r->variables << v;
}

void MetaDataBase::removeVariable( TQObject *o, const TQString &name )
{
    setupDataBase();
    MetaDataBaseRecord *r = db->find( (void*)o );
    if ( !r ) {
	tqWarning( "No entry for %p (%s, %s) found in MetaDataBase",
		  o, o->name(), o->className() );
	return;
    }
    TQValueList<Variable>::Iterator it = r->variables.begin();
    for ( ; it != r->variables.end(); ++it ) {
	if ( (*it).varName == name ) {
	    r->variables.remove( it );
	    break;
	}
    }
}

TQValueList<MetaDataBase::Variable> MetaDataBase::variables( TQObject *o )
{
    setupDataBase();
    MetaDataBaseRecord *r = db->find( (void*)o );
    if ( !r ) {
	tqWarning( "No entry for %p (%s, %s) found in MetaDataBase",
		  o, o->name(), o->className() );
	return TQValueList<MetaDataBase::Variable>();
    }

    return r->variables;
}

bool MetaDataBase::hasVariable( TQObject *o, const TQString &name )
{
    setupDataBase();
    MetaDataBaseRecord *r = db->find( (void*)o );
    if ( !r ) {
	tqWarning( "No entry for %p (%s, %s) found in MetaDataBase",
		  o, o->name(), o->className() );
	return false;
    }

    TQValueList<Variable>::Iterator it = r->variables.begin();
    for ( ; it != r->variables.end(); ++it ) {
	if ( extractVariableName( name ) == extractVariableName( (*it).varName ) )
	    return true;
    }
    return false;
}

TQString MetaDataBase::extractVariableName( const TQString &name )
{
    TQString n = name.right( name.length() - name.findRev( ' ' ) - 1 );
    if ( n[ 0 ] == '*' || n[ 0 ] == '&' )
	n[ 0 ] = ' ';
    if ( n[ (int)n.length() - 1 ] == ';' )
	n[ (int)n.length() - 1 ] = ' ';
    return n.simplifyWhiteSpace();
}

void MetaDataBase::setSignalList( TQObject *o, const TQStringList &sigs )
{
    setupDataBase();
    MetaDataBaseRecord *r = db->find( (void*)o );
    if ( !r ) {
	tqWarning( "No entry for %p (%s, %s) found in MetaDataBase",
		  o, o->name(), o->className() );
	return;
    }

    r->sigs.clear();

    for ( TQStringList::ConstIterator it = sigs.begin(); it != sigs.end(); ++it ) {
	TQString s = (*it).simplifyWhiteSpace();
	bool hasSemicolon = s.endsWith( ";" );
	if ( hasSemicolon )
	    s = s.left( s.length() - 1 );
	int p = s.find( '(' );
	if ( p < 0 )
	    p = s.length();
	int sp = s.find( ' ' );
	if ( sp >= 0 && sp < p ) {
	    s = s.mid( sp+1 );
	    p -= sp + 1;
	}
	if ( p == (int) s.length() )
	    s += "()";
	if ( hasSemicolon )
	    s += ";";
	r->sigs << s;
    }
}

TQStringList MetaDataBase::signalList( TQObject *o )
{
    setupDataBase();
    MetaDataBaseRecord *r = db->find( (void*)o );
    if ( !r ) {
	tqWarning( "No entry for %p (%s, %s) found in MetaDataBase",
		  o, o->name(), o->className() );
	return TQStringList();
    }

    return r->sigs;
}

void MetaDataBase::setMetaInfo( TQObject *o, MetaInfo mi )
{
    setupDataBase();
    MetaDataBaseRecord *r = db->find( (void*)o );
    if ( !r ) {
	tqWarning( "No entry for %p (%s, %s) found in MetaDataBase",
		  o, o->name(), o->className() );
	return;
    }

    r->metaInfo = mi;
}

MetaDataBase::MetaInfo MetaDataBase::metaInfo( TQObject *o )
{
    setupDataBase();
    MetaDataBaseRecord *r = db->find( (void*)o );
    if ( !r ) {
	tqWarning( "No entry for %p (%s, %s) found in MetaDataBase",
		  o, o->name(), o->className() );
	return MetaInfo();
    }

    return r->metaInfo;
}




MetaDataBase::CustomWidget::CustomWidget()
{
    className = "MyCustomWidget";
    includeFile = "mywidget.h";
    includePolicy = Local;
    sizeHint = TQSize( -1, -1 );
    pixmap = new TQPixmap( TQPixmap::fromMimeSource( "designer_customwidget.png" ) );
    id = -1;
    sizePolicy = TQSizePolicy( TQSizePolicy::Preferred, TQSizePolicy::Preferred );
    isContainer = false;
}

MetaDataBase::CustomWidget::CustomWidget( const CustomWidget &w )
{
    className = w.className;
    includeFile = w.includeFile;
    includePolicy = w.includePolicy;
    sizeHint = w.sizeHint;
    if ( w.pixmap )
	pixmap = new TQPixmap( *w.pixmap );
    else
	pixmap = 0;
    id = w.id;
    isContainer = w.isContainer;
}

void MetaDataBase::setCursor( TQWidget *w, const TQCursor &c )
{
    setupDataBase();
    if ( w->isA( "PropertyObject" ) ) {
	( (PropertyObject*)w )->mdSetCursor( c );
	return;
    }
    MetaDataBaseRecord *r = db->find( (void*)w );
    if ( !r ) {
	tqWarning( "No entry for %p (%s, %s) found in MetaDataBase",
		  w, w->name(), w->className() );
	return;
    }

    r->cursor = c;
}

TQCursor MetaDataBase::cursor( TQWidget *w )
{
    setupDataBase();
    if ( w->isA( "PropertyObject" ) )
	return ( (PropertyObject*)w )->mdCursor();
    MetaDataBaseRecord *r = db->find( (void*)w );
    if ( !r ) {
	w->unsetCursor();
	return w->cursor();
    }

    return r->cursor;
}

bool MetaDataBase::CustomWidget::operator==( const CustomWidget &w ) const
{
    return className == w.className;
}

MetaDataBase::CustomWidget &MetaDataBase::CustomWidget::operator=( const CustomWidget &w )
{
    delete pixmap;
    className = w.className;
    includeFile = w.includeFile;
    includePolicy = w.includePolicy;
    sizeHint = w.sizeHint;
    if ( w.pixmap )
	pixmap = new TQPixmap( *w.pixmap );
    else
	pixmap = 0;
    lstSignals = w.lstSignals;
    lstSlots = w.lstSlots;
    lstProperties = w.lstProperties;
    id = w.id;
    isContainer = w.isContainer;
    return *this;
}

bool MetaDataBase::CustomWidget::hasSignal( const TQCString &signal ) const
{
    TQStrList sigList = TQWidget::staticMetaObject()->signalNames( true );
    if ( sigList.find( signal ) != -1 )
	return true;
    for ( TQValueList<TQCString>::ConstIterator it = lstSignals.begin(); it != lstSignals.end(); ++it ) {
	if ( normalizeFunction( *it ) == normalizeFunction( signal ) )
	    return true;
    }
    return false;
}

bool MetaDataBase::CustomWidget::hasSlot( const TQCString &slot ) const
{
    TQStrList slotList = TQWidget::staticMetaObject()->slotNames( true );
    if ( slotList.find( normalizeFunction( slot ) ) != -1 )
	return true;

    for ( TQValueList<MetaDataBase::Function>::ConstIterator it = lstSlots.begin(); it != lstSlots.end(); ++it ) {
	if ( normalizeFunction( (*it).function ) == normalizeFunction( slot ) )
	    return true;
    }
    return false;
}

bool MetaDataBase::CustomWidget::hasProperty( const TQCString &prop ) const
{
    TQStrList propList = TQWidget::staticMetaObject()->propertyNames( true );
    if ( propList.find( prop ) != -1 )
	return true;

    for ( TQValueList<MetaDataBase::Property>::ConstIterator it = lstProperties.begin(); it != lstProperties.end(); ++it ) {
	if ( (*it).property == prop )
	    return true;
    }
    return false;
}

void MetaDataBase::setPixmapArgument( TQObject *o, int pixmap, const TQString &arg )
{
    if ( !o )
	return;
    setupDataBase();
    MetaDataBaseRecord *r = db->find( (void*)o );
    if ( !r ) {
	tqWarning( "No entry for %p (%s, %s) found in MetaDataBase",
		  o, o->name(), o->className() );
	return;
    }

    r->pixmapArguments.remove( pixmap );
    r->pixmapArguments.insert( pixmap, arg );
}

TQString MetaDataBase::pixmapArgument( TQObject *o, int pixmap )
{
    if ( !o )
	return TQString::null;
    setupDataBase();
    MetaDataBaseRecord *r = db->find( (void*)o );
    if ( !r ) {
	tqWarning( "No entry for %p (%s, %s) found in MetaDataBase",
		  o, o->name(), o->className() );
	return TQString::null;
    }

    return *r->pixmapArguments.find( pixmap );
}

void MetaDataBase::clearPixmapArguments( TQObject *o )
{
    if ( !o )
	return;
    setupDataBase();
    MetaDataBaseRecord *r = db->find( (void*)o );
    if ( !r ) {
	tqWarning( "No entry for %p (%s, %s) found in MetaDataBase",
		  o, o->name(), o->className() );
	return;
    }

    r->pixmapArguments.clear();
}


void MetaDataBase::setPixmapKey( TQObject *o, int pixmap, const TQString &arg )
{
    if ( !o )
	return;
    setupDataBase();
    if ( o->isA( "PropertyObject" ) ) {
	( (PropertyObject*)o )->mdSetPixmapKey( pixmap, arg );
	return;
    }
    MetaDataBaseRecord *r = db->find( (void*)o );
    if ( !r ) {
	tqWarning( "No entry for %p (%s, %s) found in MetaDataBase",
		  o, o->name(), o->className() );
	return;
    }

    r->pixmapKeys.remove( pixmap );
    r->pixmapKeys.insert( pixmap, arg );
}

TQString MetaDataBase::pixmapKey( TQObject *o, int pixmap )
{
    if ( !o )
	return TQString::null;
    setupDataBase();
    if ( o->isA( "PropertyObject" ) )
	return ( (PropertyObject*)o )->mdPixmapKey( pixmap );
    MetaDataBaseRecord *r = db->find( (void*)o );
    if ( !r ) {
	tqWarning( "No entry for %p (%s, %s) found in MetaDataBase",
		  o, o->name(), o->className() );
	return TQString::null;
    }

    TQString s = *r->pixmapKeys.find( pixmap );
    if ( !s.isNull() )
	return s;
    if ( !o->isWidgetType() )
	return s;
    TQWidget *w = (TQWidget*)o;
    if ( w->icon() )
	return *r->pixmapKeys.find( w->icon()->serialNumber() );
    return s;
}

void MetaDataBase::clearPixmapKeys( TQObject *o )
{
    if ( !o )
	return;
    setupDataBase();
    MetaDataBaseRecord *r = db->find( (void*)o );
    if ( !r ) {
	tqWarning( "No entry for %p (%s, %s) found in MetaDataBase",
		  o, o->name(), o->className() );
	return;
    }

    r->pixmapKeys.clear();
}



void MetaDataBase::setColumnFields( TQObject *o, const TQMap<TQString, TQString> &columnFields )
{
    if ( !o )
	return;
    setupDataBase();
    MetaDataBaseRecord *r = db->find( (void*)o );
    if ( !r ) {
	tqWarning( "No entry for %p (%s, %s) found in MetaDataBase",
		  o, o->name(), o->className() );
	return;
    }

    r->columnFields = columnFields;
}

TQMap<TQString, TQString> MetaDataBase::columnFields( TQObject *o )
{
    if ( !o )
	return TQMap<TQString, TQString>();
    setupDataBase();
    MetaDataBaseRecord *r = db->find( (void*)o );
    if ( !r ) {
	tqWarning( "No entry for %p (%s, %s) found in MetaDataBase",
		  o, o->name(), o->className() );
	return TQMap<TQString, TQString>();
    }

    return r->columnFields;
}

void MetaDataBase::setEditor( const TQStringList &langs )
{
    editorLangList = langs;
}

bool MetaDataBase::hasEditor( const TQString &lang )
{
    return editorLangList.find( lang ) != editorLangList.end();
}

void MetaDataBase::setupInterfaceManagers( const TQString &plugDir )
{
    if ( !languageInterfaceManager ) {
	languageInterfaceManager =
	    new TQPluginManager<LanguageInterface>( IID_Language,
						   TQApplication::libraryPaths(),
						   plugDir );

	langList = languageInterfaceManager->featureList();
	langList.remove( "C++" );
	langList << "C++";
    }
}

TQStringList MetaDataBase::languages()
{
    return langList;
}

TQString MetaDataBase::normalizeFunction( const TQString &f )
{
    return Parser::cleanArgs( f );
}

LanguageInterface *MetaDataBase::languageInterface( const TQString &lang )
{
    LanguageInterface* iface = 0;
    languageInterfaceManager->queryInterface( lang, &iface );
    return iface;
}

void MetaDataBase::clear( TQObject *o )
{
    if ( !o )
	return;
    setupDataBase();
    db->remove( (void*)o );
    for ( TQPtrDictIterator<TQWidget> it( *( (FormWindow*)o )->widgets() ); it.current(); ++it )
	db->remove( (void*)it.current() );
}

void MetaDataBase::setBreakPoints( TQObject *o, const TQValueList<uint> &l )
{
    if ( !o )
	return;
    setupDataBase();
    MetaDataBaseRecord *r = db->find( (void*)o );
    if ( !r ) {
	tqWarning( "No entry for %p (%s, %s) found in MetaDataBase",
		  o, o->name(), o->className() );
	return;
    }

    r->breakPoints = l;

    TQMap<int, TQString>::Iterator it = r->breakPointConditions.begin();
    while ( it != r->breakPointConditions.end() ) {
	int line = it.key();
	++it;
	if ( r->breakPoints.find( line ) == r->breakPoints.end() )
	    r->breakPointConditions.remove( r->breakPointConditions.find( line ) );
    }
}

TQValueList<uint> MetaDataBase::breakPoints( TQObject *o )
{
    if ( !o )
	return TQValueList<uint>();
    setupDataBase();
    MetaDataBaseRecord *r = db->find( (void*)o );
    if ( !r ) {
	tqWarning( "No entry for %p (%s, %s) found in MetaDataBase",
		  o, o->name(), o->className() );
	return TQValueList<uint>();
    }

    return r->breakPoints;
}

void MetaDataBase::setBreakPointCondition( TQObject *o, int line, const TQString &condition )
{
    if ( !o )
	return;
    setupDataBase();
    MetaDataBaseRecord *r = db->find( (void*)o );
    if ( !r ) {
	tqWarning( "No entry for %p (%s, %s) found in MetaDataBase",
		  o, o->name(), o->className() );
	return;
    }
    r->breakPointConditions.replace( line, condition );
}

TQString MetaDataBase::breakPointCondition( TQObject *o, int line )
{
    if ( !o )
	return TQString::null;
    setupDataBase();
    MetaDataBaseRecord *r = db->find( (void*)o );
    if ( !r ) {
	tqWarning( "No entry for %p (%s, %s) found in MetaDataBase",
		  o, o->name(), o->className() );
	return TQString::null;
    }
    TQMap<int, TQString>::Iterator it = r->breakPointConditions.find( line );
    if ( it == r->breakPointConditions.end() )
	return TQString::null;
    return *it;
}

void MetaDataBase::setExportMacro( TQObject *o, const TQString &macro )
{
    if ( !o )
	return;
    setupDataBase();
    if ( o->isA( "PropertyObject" ) ) {
	( (PropertyObject*)o )->mdSetExportMacro( macro );
	return;
    }
    MetaDataBaseRecord *r = db->find( (void*)o );
    if ( !r ) {
	tqWarning( "No entry for %p (%s, %s) found in MetaDataBase",
		  o, o->name(), o->className() );
	return;
    }

    r->exportMacro = macro;
}

TQString MetaDataBase::exportMacro( TQObject *o )
{
    if ( !o )
	return "";
    setupDataBase();
    if ( o->isA( "PropertyObject" ) )
	return ( (PropertyObject*)o )->mdExportMacro();
    MetaDataBaseRecord *r = db->find( (void*)o );
    if ( !r ) {
	tqWarning( "No entry for %p (%s, %s) found in MetaDataBase",
		  o, o->name(), o->className() );
	return "";
    }

    return r->exportMacro;
}

bool MetaDataBase::hasObject( TQObject *o )
{
    return !!db->find( o );
}
