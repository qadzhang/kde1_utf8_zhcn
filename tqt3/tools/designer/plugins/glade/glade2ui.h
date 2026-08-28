/**********************************************************************
**
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

#ifndef GLADE2UI_H
#define GLADE2UI_H

#include <ntqdom.h>
#include <ntqmap.h>
#include <ntqstring.h>
#include <ntqvariant.h>

typedef TQMap<TQString, TQString> AttributeMap;

struct GladeAction
{
    TQString text;
    TQString menuText;
    TQString toolTip;
    int accel;
    TQString iconSet;
};

struct GladeConnection
{
    TQString sender;
    TQString signal;
    TQString slot;
};

class Glade2Ui
{
public:
    Glade2Ui();

    TQStringList convertGladeFile( const TQString& fileName );

private:
    TQString imageName( const TQString& fileName );
    TQString opening( const TQString& tag,
		     const AttributeMap& attr = AttributeMap() );
    TQString closing( const TQString& tag );
    TQString atom( const TQString& tag,
		  const AttributeMap& attr = AttributeMap() );
    void error( const TQString& message );
    void syntaxError();
    TQString getTextValue( const TQDomNode& node );
    void emitHeader();
    void emitFooter();
    void emitSimpleValue( const TQString& tag, const TQString& value,
			  const AttributeMap& attr = AttributeMap() );
    void emitOpening( const TQString& tag,
		      const AttributeMap& attr = AttributeMap() );
    void emitClosing( const TQString& tag );
    void emitAtom( const TQString& tag,
		   const AttributeMap& attr = AttributeMap() );
    void emitVariant( const TQVariant& val,
		      const TQString& stringType = "string" );
    void emitProperty( const TQString& prop, const TQVariant& val,
		       const TQString& stringType = "string" );
    void emitFontProperty( const TQString& prop, int pointSize, bool bold );
    void emitAttribute( const TQString& prop, const TQVariant& val,
			const TQString& stringType = "string" );
    TQString gtk2qtClass( const TQString& gtkClass,
			 const TQValueList<TQDomElement>& childWidgets );

    int matchAccelOnActivate( const TQDomElement& accel );
    void emitGtkMenu( const TQDomElement& menu );
    void emitGtkMenuBarChildWidgets(
	    const TQValueList<TQDomElement>& childWidgets );
    void emitGtkToolbarChildWidgets(
	    const TQValueList<TQDomElement>& childWidgets );
    void emitPushButton( const TQString& text, const TQString& name );
    void attach( AttributeMap *attr, int leftAttach, int rightAttach,
		 int topAttach, int bottomAttach );
    void emitSpacer( const TQString& orientation, int leftAttach = -1,
		     int rightAttach = -1, int topAttach = -1,
		     int bottomAttach = -1 );
    void emitPixmap( const TQString& imageName, int leftAttach = -1,
		     int rightAttach = -1, int topAttach = -1,
		     int bottomAttach = -1 );
    void emitGnomeAbout( TQString copyright, TQString authors, TQString comments );
    void emitGnomeAppChildWidgetsPass1(
	    const TQValueList<TQDomElement>& childWidgets );
    void doPass2( const TQValueList<TQDomElement>& childWidgets,
		  TQValueList<TQDomElement> *menuBar,
		  TQValueList<TQValueList<TQDomElement> > *toolBars );
    void emitGnomeAppChildWidgetsPass2(
	    const TQValueList<TQDomElement>& childWidgets );
    void emitGtkButtonChildWidgets( TQValueList<TQDomElement> childWidgets );
    void emitGtkComboChildWidgets( const TQValueList<TQDomElement>& childWidgets,
				   const TQStringList& items );
    void emitGtkNotebookChildWidgets(
	    const TQValueList<TQDomElement>& childWidgets );
    void emitTQListViewColumns( const TQDomElement& qlistview );
    void emitGtkScrolledWindowChildWidgets(
	    const TQValueList<TQDomElement>& childWidgets,
	    const TQString& qtClass );
    void emitGnomeDruidPage( const TQDomElement& druidPage );
    void emitGtkWindowChildWidgets( const TQValueList<TQDomElement>& childWidgets,
				    const TQString& qtClass );
    bool packEnd( const TQDomElement& widget );
    void emitChildWidgets( const TQValueList<TQDomElement>& childWidgets,
			   bool layouted, int leftAttach = -1,
			   int rightAttach = -1, int topAttach = -1,
			   int bottomAttach = -1 );
    void emitOpeningWidget( const TQString& qtClass, int leftAttach = -1,
			    int rightAttach = -1, int topAttach = -1,
			    int bottomAttach = -1 );
    bool shouldPullup( const TQValueList<TQDomElement>& childWidgets );
    TQString emitWidget( const TQDomElement& widget, bool layouted,
			int leftAttach = -1, int rightAttach = -1,
			int topAttach = -1, int bottomAttach = -1 );

    TQString yyOut;
    TQString yyIndentStr;
    TQString yyFileName;
    TQString yyProgramName;
    TQString yyPixmapDirectory;
    TQMap<TQString, TQString> yyClassNameMap;
    TQMap<TQString, TQString> yyStockMap;
    TQMap<TQString, int> yyKeyMap;
    TQMap<TQString, TQString> yyCustomWidgets;
    TQMap<TQString, TQString> yyStockItemActions;
    TQMap<TQString, GladeAction> yyActions;
    TQValueList<GladeConnection> yyConnections;
    TQMap<TQString, TQString> yySlots;
    TQString yyFormName;
    TQMap<TQString, TQString> yyImages;

    int numErrors;
    int uniqueAction;
    int uniqueForm;
    int uniqueMenuBar;
    int uniqueSpacer;
    int uniqueToolBar;
};

#endif
