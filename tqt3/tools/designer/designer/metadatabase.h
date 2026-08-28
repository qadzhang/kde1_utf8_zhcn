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

#ifndef METADATABASE_H
#define METADATABASE_H

#include <ntqvariant.h>
#include <ntqstring.h>
#include <ntqstringlist.h>
#include <ntqmap.h>
#include <ntqptrlist.h>
#include <ntqsizepolicy.h>
#include <ntqsize.h>
#include <ntqwidgetlist.h>
#include <ntqcursor.h>

#include "pixmapchooser.h"
#include "../interfaces/languageinterface.h"

class TQObject;
class TQPixmap;
struct LanguageInterface;

class MetaDataBase
{
public:
    struct Connection
    {
	TQObject *sender, *receiver;
	TQCString signal, slot;
	bool operator==( const Connection &c ) const {
	    return sender == c.sender && receiver == c.receiver &&
		   signal == c.signal && slot == c.slot ;
	}
    };

    struct Function
    {
	TQString returnType;
	TQCString function;
	TQString specifier;
	TQString access;
	TQString type;
	TQString language;
	bool operator==( const Function &f ) const {
	    return ( returnType == f.returnType &&
		     function == f.function &&
		     specifier == f.specifier &&
		     access == f.access &&
		     type == f.type &&
		     language == f.language
		     );
	}
    };

    struct Property
    {
	TQCString property;
	TQString type;
	bool operator==( const Property &p ) const {
	    return property == p.property &&
		 type == p.type;
	}
    };

    struct CustomWidget
    {
	CustomWidget();
	CustomWidget( const CustomWidget &w );
	~CustomWidget() { delete pixmap; } // inlined to work around 2.7.2.3 bug
	bool operator==( const CustomWidget &w ) const;
	CustomWidget &operator=( const CustomWidget &w );

	bool hasSignal( const TQCString &signal ) const;
	bool hasSlot( const TQCString &slot ) const;
	bool hasProperty( const TQCString &prop ) const;

	enum IncludePolicy { Global, Local };
	TQString className;
	TQString includeFile;
	IncludePolicy includePolicy;
	TQSize sizeHint;
	TQSizePolicy sizePolicy;
	TQPixmap *pixmap;
	TQValueList<TQCString> lstSignals;
	TQValueList<Function> lstSlots;
	TQValueList<Property> lstProperties;
	int id;
	bool isContainer;
    };

    struct Include
    {
	Include() : header(), location(), implDecl( "in implementation" ) {}
	TQString header;
	TQString location;
	bool operator==( const Include &i ) const {
	    return header == i.header && location == i.location;
	}
	TQString implDecl;
    };

    struct Variable
    {
	TQString varName;
	TQString varAccess;
	bool operator==( const Variable &v ) const {
	    return varName == v.varName &&
		   varAccess == v.varAccess;
	}
    };

    struct MetaInfo
    {
	MetaInfo() : classNameChanged( false ) { }
	TQString className;
	bool classNameChanged;
	TQString comment;
	TQString author;
    };

    MetaDataBase();
    static void clearDataBase();

    static void addEntry( TQObject *o );
    static void removeEntry( TQObject *o );
    static void setPropertyChanged( TQObject *o, const TQString &property, bool changed );
    static bool isPropertyChanged( TQObject *o, const TQString &property );
    static void setPropertyComment( TQObject *o, const TQString &property, const TQString &comment );
    static TQString propertyComment( TQObject *o, const TQString &property );
    static TQStringList changedProperties( TQObject *o );

    static void setFakeProperty( TQObject *o, const TQString &property, const TQVariant& value );
    static TQVariant fakeProperty( TQObject * o, const TQString &property );
    static TQStringVariantMap* fakeProperties( TQObject* o );

    static void setSpacing( TQObject *o, int spacing );
    static int spacing( TQObject *o );
    static void setMargin( TQObject *o, int margin );
    static int margin( TQObject *o );

    static void setResizeMode( TQObject *o, const TQString &mode );
    static TQString resizeMode( TQObject *o );

    static void addConnection( TQObject *o, TQObject *sender, const TQCString &signal,
			       TQObject *receiver, const TQCString &slot, bool addCode = true );
    static void removeConnection( TQObject *o, TQObject *sender, const TQCString &signal,
				  TQObject *receiver, const TQCString &slot );
    static bool hasConnection( TQObject *o, TQObject *sender, const TQCString &signal,
			       TQObject *receiver, const TQCString &slot );
    static void setupConnections( TQObject *o, const TQValueList<LanguageInterface::Connection> &conns );
    static TQValueList<Connection> connections( TQObject *o );
    static TQValueList<Connection> connections( TQObject *o, TQObject *sender, TQObject *receiver );
    static TQValueList<Connection> connections( TQObject *o, TQObject *object );
    static void doConnections( TQObject *o );

    static void addFunction( TQObject *o, const TQCString &function, const TQString &specifier,
			     const TQString &access, const TQString &type, const TQString &language,
			     const TQString &returnType );
    static void removeFunction( TQObject *o, const TQCString &function, const TQString &specifier,
				const TQString &access, const TQString &type, const TQString &language,
				const TQString &returnType );
    static void removeFunction( TQObject *o, const TQString &function );
    static TQValueList<Function> functionList( TQObject *o, bool onlyFunctions = false );
    static TQValueList<Function> slotList( TQObject *o );
    static bool isSlotUsed( TQObject *o, const TQCString &slot );
    static bool hasFunction( TQObject *o, const TQCString &function, bool onlyCustom = false );
    static bool hasSlot( TQObject *o, const TQCString &slot, bool onlyCustom = false );
    static void changeFunction( TQObject *o, const TQString &function, const TQString &newName,
				const TQString &returnType );
    static void changeFunctionAttributes( TQObject *o, const TQString &oldName, const TQString &newName,
				      const TQString &specifier, const TQString &access,
				      const TQString &type, const TQString &language,
				      const TQString &returnType );
    static TQString languageOfFunction( TQObject *o, const TQCString &function );
    static void setFunctionList( TQObject *o, const TQValueList<Function> &functionList );


    static bool addCustomWidget( CustomWidget *w );
    static void removeCustomWidget( CustomWidget *w );
    static TQPtrList<CustomWidget> *customWidgets();
    static CustomWidget *customWidget( int id );
    static bool isWidgetNameUsed( CustomWidget *w );
    static bool hasCustomWidget( const TQString &className );

    static void setTabOrder( TQWidget *w, const TQWidgetList &order );
    static TQWidgetList tabOrder( TQWidget *w );

    static void setIncludes( TQObject *o, const TQValueList<Include> &incs );
    static TQValueList<Include> includes( TQObject *o );

    static void setForwards( TQObject *o, const TQStringList &fwds );
    static TQStringList forwards( TQObject *o );

    static void setVariables( TQObject *o, const TQValueList<Variable> &vars );
    static void addVariable( TQObject *o, const TQString &name, const TQString &access );
    static void removeVariable( TQObject *o, const TQString &name );
    static TQValueList<Variable> variables( TQObject *o );
    static bool hasVariable( TQObject *o, const TQString &name );
    static TQString extractVariableName( const TQString &name );

    static void setSignalList( TQObject *o, const TQStringList &sigs );
    static TQStringList signalList( TQObject *o );

    static void setMetaInfo( TQObject *o, MetaInfo mi );
    static MetaInfo metaInfo( TQObject *o );

    static void setCursor( TQWidget *w, const TQCursor &c );
    static TQCursor cursor( TQWidget *w );

    static void setPixmapArgument( TQObject *o, int pixmap, const TQString &arg );
    static TQString pixmapArgument( TQObject *o, int pixmap );
    static void clearPixmapArguments( TQObject *o );

    static void setPixmapKey( TQObject *o, int pixmap, const TQString &arg );
    static TQString pixmapKey( TQObject *o, int pixmap );
    static void clearPixmapKeys( TQObject *o );

    static void setColumnFields( TQObject *o, const TQMap<TQString, TQString> &columnFields );
    static TQMap<TQString, TQString> columnFields( TQObject *o );

    static void setEditor( const TQStringList &langs );
    static bool hasEditor( const TQString &lang );

    static void setupInterfaceManagers( const TQString &plugDir );
    static TQStringList languages();

    static LanguageInterface *languageInterface( const TQString &lang );

    static TQString normalizeFunction( const TQString &f );

    static void clear( TQObject *o );

    static void setBreakPoints( TQObject *o, const TQValueList<uint> &l );
    static void setBreakPointCondition( TQObject *o, int line, const TQString &condition );
    static TQValueList<uint> breakPoints( TQObject *o );
    static TQString breakPointCondition( TQObject *o, int line );

    static void setExportMacro( TQObject *o, const TQString &macro );
    static TQString exportMacro( TQObject *o );

    static bool hasObject( TQObject *o );

};

#endif
