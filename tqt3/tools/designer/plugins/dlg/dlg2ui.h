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

#ifndef DLG2UI_H
#define DLG2UI_H

#include <ntqdom.h>
#include <ntqmap.h>
#include <ntqstring.h>
#include <ntqvariant.h>

typedef TQMap<TQString, TQString> AttributeMap;

struct DlgConnection
{
    TQString sender;
    TQString signal;
    TQString slot;
};

class Dlg2Ui
{
public:
    TQStringList convertTQtArchitectDlgFile( const TQString& fileName );

private:
    TQString alias( const TQString& name ) const;
    TQString opening( const TQString& tag,
		     const AttributeMap& attr = AttributeMap() );
    TQString closing( const TQString& tag );
    void error( const TQString& message );
    void syntaxError();
    TQString getTextValue( const TQDomNode& node );
    TQVariant getValue( const TQDomNodeList& children, const TQString& tagName,
		       const TQString& type = "qstring" );
    void emitHeader();
    void emitFooter();
    void emitSimpleValue( const TQString& tag, const TQString& value,
			  const AttributeMap& attr = AttributeMap() );
    void emitOpening( const TQString& tag,
		      const AttributeMap& attr = AttributeMap() );
    void emitClosing( const TQString& tag );
    void emitOpeningWidget( const TQString& className );
    TQString widgetClassName( const TQDomElement& e );
    void emitColor( const TQColor& color );
    void emitColorGroup( const TQString& name, const TQColorGroup& group );
    void emitVariant( const TQVariant& val,
		      const TQString& stringType = "string" );
    void emitProperty( const TQString& prop, const TQVariant& val,
		       const TQString& stringType = "string" );
    void emitAttribute( const TQString& attr, const TQVariant& val,
			   const TQString& stringType = "string" );
    void emitOpeningLayout( bool needsWidget, const TQString& layoutKind,
			    const TQString& name, int border, int autoBorder );
    void flushWidgets();
    void emitClosingLayout( bool needsWidget, const TQString& layoutKind );
    bool isWidgetType( const TQDomElement& e );
    void emitSpacer( int spacing, int stretch );
    TQString filteredFlags( const TQString& flags, const TQRegExp& filter );
    void emitFrameStyleProperty( int style );
    void emitWidgetBody( const TQDomElement& e, bool layouted );
    bool checkTagName( const TQDomElement& e, const TQString& tagName );
    TQString normalizeType( const TQString& type );
    TQVariant getValue( const TQDomElement& e, const TQString& tagName,
		       const TQString& type = "qstring" );
    void matchDialogCommon( const TQDomElement& dialogCommon );
    bool needsTQLayoutWidget( const TQDomElement& e );
    void matchBoxLayout( const TQDomElement& boxLayout );
    void matchBoxSpacing( const TQDomElement& boxSpacing );
    void matchBoxStretch( const TQDomElement& boxStretch );
    void matchGridLayout( const TQDomElement& gridLayout );
    void matchGridRow( const TQDomElement& gridRow );
    void matchGridSpacer( const TQDomElement& gridSpacer );
    void matchLayoutWidget( const TQDomElement& layoutWidget );
    void matchBox( const TQDomElement& box );
    void matchLayout( const TQDomElement& layout );
    void matchWidgetLayoutCommon( const TQDomElement& widgetLayoutCommon );
    void matchWidget( const TQDomElement& widget );
    void matchWidgets( const TQDomElement& widgets );
    void matchTabOrder( const TQDomElement& tabOrder );
    void matchWidgetLayout( const TQDomElement& widgetLayout );
    void matchDialog( const TQDomElement& dialog );

    TQString yyOut;
    TQString yyIndentStr;
    TQString yyFileName;
    TQString yyClassName;
    TQMap<TQString, int> yyWidgetTypeSet;
    TQMap<TQString, TQMap<TQString, int> > yyPropertyMap;
    TQMap<TQString, TQDomElement> yyWidgetMap;
    TQMap<TQString, TQString> yyCustomWidgets;
    TQValueList<DlgConnection> yyConnections;
    TQMap<TQString, TQString> yySlots;
    TQMap<TQString, TQString> yyAliasMap;
    TQStringList yyTabStops;
    TQString yyBoxKind;
    int yyLayoutDepth;
    int yyGridRow;
    int yyGridColumn;

    int numErrors;
    int uniqueLayout;
    int uniqueSpacer;
    int uniqueWidget;
};

#endif
