/**********************************************************************
**
** Converts a TQt Architect 2.1+ .dlg file into a .ui file.
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

#include "dlg2ui.h"
#include <ntqfile.h>
#include <ntqframe.h>
#include <ntqmessagebox.h>
#include <ntqregexp.h>
#include <ntqtextstream.h>

/*
  Possible improvements:

  1.  Convert layout stretch factors to size policy stretches, now
      that TQt Designer supports the latter.
*/

/*
  These big tables could be more or less eliminated by using TQt's
  TQMetaObject and TQMetaProperty classes. However, the interface of
  these classes is unwieldy for an otherwise non-GUI program like this
  one, as we would have to create one dummy object for most TQObject
  subclasses in TQt. Let's take the safe road.
*/

static const char *widgetTypes[] = {
    "Button", "ButtonGroup", "CheckBox", "ComboBox", "Dial", "DlgWidget",
    "Frame", "Grid", "GroupBox", "HBox", "HButtonGroup", "HGroupBox",
    "IconView", "LCDNumber", "Label", "LineEdit", "ListBox", "ListView",
    "MenuBar", "MultiLineEdit", "ProgressBar", "PushButton", "RadioButton",
    "ScrollBar", "ScrollView", "Slider", "SpinBox", "Splitter", "TabBar",
    "TextBrowser", "TextView", "User", "VBox", "VButtonGroup", "VGroupBox", 0
};

/*
  This table maps TQt Architect properties to TQt Designer properties.
  If there is no corresponding TQt Designer property, qtName is 0 and
  the property can be handled explicitly.
*/
static const struct {
    const char *widgetName;
    const char *architectName;
    const char *qtName;
    const char *type;
} propertyDefs[] = {
    { "Button", "AutoRepeat", "autoRepeat", "boolean" },
    { "Button", "AutoResize", 0, 0 },
    { "Button", "Text", "text", "qstring" },
    { "ButtonGroup", "Exclusive", "exclusive", "boolean" },
    { "ButtonGroup", "RadioButtonExclusive", "radioButtonExclusive",
      "boolean" },
    { "CheckBox", "Checked", "checked", "boolean" },
    { "ComboBox", "AutoCompletion", "autoCompletion", "boolean" },
    { "ComboBox", "AutoResize", 0, 0 },
    { "ComboBox", "DuplicatesEnabled", "duplicatesEnabled", "boolean" },
    { "ComboBox", "MaxCount", "maxCount", "integer" },
    { "ComboBox", "Policy", "insertionPolicy", "enum" },
    { "ComboBox", "SizeLimit", "sizeLimit", "integer" },
    { "ComboBox", "Style", 0, 0 },
    { "Dial", "Initial", "value", "integer" },
    { "Dial", "LineStep", "lineStep", "integer" },
    { "Dial", "MaxValue", "maxValue", "integer" },
    { "Dial", "MinValue", "minValue", "integer" },
    { "Dial", "NotchTarget", "notchTarget", "double" },
    { "Dial", "PageStep", "pageStep", "integer" },
    { "Dial", "ShowNotches", "notchesVisible", "boolean" },
    { "Dial", "Tracking", "tracking", "boolean" },
    { "Dial", "Wrapping", "wrapping", "boolean" },
    { "DlgWidget", "AdjustSize", 0, 0 },
    { "DlgWidget", "BackgroundMode", "backgroundMode", "enum" },
    { "DlgWidget", "BackgroundOrigin", "backgroundOrigin", "enum" },
    { "DlgWidget", "BackgroundPixmap", "backgroundPixmap", "qpixmap" },
    { "DlgWidget", "DataLenName", 0, 0 },
    { "DlgWidget", "DataVarName", 0, 0 },
    { "DlgWidget", "Enabled", "enabled", "boolean" },
    { "DlgWidget", "FocusPolicy", "focusPolicy", "enum" },
    { "DlgWidget", "Font", "font", "qfont" },
    { "DlgWidget", "FontPropagation", 0, 0 },
    { "DlgWidget", "MaximumSize", "maximumSize", "qsize" },
    { "DlgWidget", "MinimumSize", "minimumSize", "qsize" },
    { "DlgWidget", "Name", 0, 0 },
    { "DlgWidget", "Palette", "palette", "qpalette" },
    { "DlgWidget", "PalettePropagation", 0, 0 },
    { "DlgWidget", "ReadPixmapFromData", 0, 0 },
    { "DlgWidget", "Rect", 0, 0 },
    { "DlgWidget", "SignalConnection", 0, 0 },
    { "DlgWidget", "UseBackgroudPixmap", 0, 0 },
    { "DlgWidget", "Variable", 0, 0 },
    { "Frame", "FrameMargin", "margin", "integer" },
    { "Frame", "LineWidth", "lineWidth", "integer" },
    { "Frame", "MidLineWidth", "midLineWidth", "integer" },
    { "Frame", "Style", 0, 0 },
    { "GroupBox", "Title", "title", "qstring" },
    { "IconView", "AlignMode", 0, 0 },
    { "IconView", "Aligning", 0, 0 },
    { "IconView", "Arrangement", "arrangement", "enum" },
    { "IconView", "AutoArrange", "autoArrange", "boolean" },
    { "IconView", "EnableMoveItems", "itemsMovable", "boolean" },
    { "IconView", "GridX", "gridX", "integer" },
    { "IconView", "GridY", "gridY", "integer" },
    { "IconView", "ItemTextPos", "itemTextPos", "enum" },
    { "IconView", "ItemsMovable", "itemsMovable", "boolean" },
    { "IconView", "MaxItemTextLength", "maxItemTextLength", "integer" },
    { "IconView", "MaxItemWidth", "maxItemWidth", "integer" },
    { "IconView", "ResizeMode", "resizeMode", "enum" },
    { "IconView", "SelectionMode", "selectionMode", "enum" },
    { "IconView", "ShowToolTips", "showToolTips", "boolean" },
    { "IconView", "SortAscending", "sortDirection", "bool" },
    { "IconView", "Spacing", "spacing", "integer" },
    { "IconView", "WordWrapIconText", "wordWrapIconText", "boolean" },
    { "LCDNumber", "Digits", "numDigits", "integer" },
    { "LCDNumber", "Mode", "mode", "enum" },
    { "LCDNumber", "SegmentStyle", "segmentStyle", "enum" },
    { "LCDNumber", "SmallDecimalPoint", "smallDecimalPoint", "boolean" },
    { "LCDNumber", "Value", 0, 0 },
    { "Label", "AutoResize", 0, 0 },
    { "Label", "Indent", "indent", "integer" },
    { "Label", "Text", "text", "qstring" },
    { "Label", "TextFormat", "textFormat", "enum" },
    { "LineEdit", "EchoMode", "echoMode", "enum" },
    { "LineEdit", "FrameShown", "frame", "boolean"  },
    { "LineEdit", "MaxLength", "maxLength", "integer" },
    { "LineEdit", "Text", "text", "qstring" },
    { "ListBox", "AutoScroll", 0, 0 },
    { "ListBox", "AutoUpdate", 0, 0 },
    { "ListBox", "ColumnMode", "columnMode", "enum" },
    { "ListBox", "DragSelect", 0, 0 },
    { "ListBox", "RowMode", "rowMode", "enum" },
    { "ListBox", "SelectionMode", "selectionMode", "enum" },
    { "ListBox", "SmoothScrolling", 0, 0 },
    { "ListView", "AllColumnsShowFocus", "allColumnsShowFocus", "boolean" },
    { "ListView", "HeaderInformation", 0, 0 },
    { "ListView", "ItemMargin", "itemMargin", "integer" },
    { "ListView", "MultiSelection", "multiSelection", "boolean" },
    { "ListView", "RootIsDecorated", "rootIsDecorated", "boolean" },
    { "ListView", "TreeStepSize", "treeStepSize", "boolean" },
    { "MultiLineEdit", "AutoUpdate", 0, 0 },
    { "MultiLineEdit", "EchoMode", 0, 0 },
    { "MultiLineEdit", "HorizontalMargin", 0, 0 },
    { "MultiLineEdit", "MaxLength", 0, 0 },
    { "MultiLineEdit", "MaxLineLength", 0, 0 },
    { "MultiLineEdit", "MaxLines", 0, 0 },
    { "MultiLineEdit", "OverwriteMode", 0, 0 },
    { "MultiLineEdit", "ReadOnly", 0, 0 },
    { "MultiLineEdit", "Text", 0, 0 },
    { "MultiLineEdit", "UndoDepth", "undoDepth", "integer" },
    { "MultiLineEdit", "UndoEnabled", 0, 0 },
    { "MultiLineEdit", "WordWrap", 0, 0 },
    { "MultiLineEdit", "WrapColumnOrWidth", 0, 0 },
    { "MultiLineEdit", "WrapPolicy", 0, 0 },
    { "ProgressBar", "CenterIndicator", "centerIndicator", "boolean" },
    { "ProgressBar", "IndicatorFollowsStyle", "indicatorFollowsStyle",
      "boolean" },
    { "ProgressBar", "Progress", "progress", "integer" },
    { "ProgressBar", "TotalSteps", "totalSteps", "integer" },
    { "PushButton", "AutoDefault", "autoDefault", "boolean" },
    { "PushButton", "Default", "default", "boolean" },
    { "PushButton", "IsMenuButton", 0, 0 },
    { "PushButton", "ToggleButton", "toggleButton", "boolean" },
    { "RadioButton", "Checked", "checked", "boolean" },
    { "ScrollBar", "Initial", "value", "integer" },
    { "ScrollBar", "LineStep", "lineStep", "integer" },
    { "ScrollBar", "MaxValue", "maxValue", "integer" },
    { "ScrollBar", "MinValue", "minValue", "integer" },
    { "ScrollBar", "Orientation", "orientation", "enum" },
    { "ScrollBar", "PageStep", "pageStep", "integer" },
    { "ScrollBar", "Tracking", "tracking", "boolean" },
    { "ScrollView", "DragAutoScroll", "dragAutoScroll", "boolean" },
    { "ScrollView", "HScrollBarMode", "hScrollBarMode", "enum" },
    { "ScrollView", "ResizePolicy", "resizePolicy", "enum" },
    { "ScrollView", "VScrollBarMode", "vScrollBarMode", "enum" },
    { "Slider", "Initial", "value", "integer" },
    { "Slider", "LineStep", "lineStep", "integer" },
    { "Slider", "MaxValue", "maxValue", "integer" },
    { "Slider", "MinValue", "minValue", "integer" },
    { "Slider", "Orientation", "orientation", "enum" },
    { "Slider", "PageStep", "pageStep", "integer" },
    { "Slider", "TickInterval", "tickInterval", "integer" },
    { "Slider", "Tickmarks", "tickmarks", "enum" },
    { "Slider", "Tracking", "tracking", "boolean" },
    { "SpinBox", "ButtonSymbols", "buttonSymbols", "enum" },
    { "SpinBox", "MaxValue", "maxValue", "integer" },
    { "SpinBox", "MinValue", "minValue", "integer" },
    { "SpinBox", "Prefix", "prefix", "qstring" },
    { "SpinBox", "SpecialValue", "specialValueText", "qstring" },
    { "SpinBox", "Step", "lineStep", "integer" },
    { "SpinBox", "Suffix", "suffix", "qstring" },
    { "SpinBox", "Wrapping", "wrapping", "boolean" },
    { "Splitter", "OpaqueResize", 0, 0 },
    { "Splitter", "Orientation", "orientation", "enum" },
    { "TabBar", "Shape", "shape", "enum" },
    { "TabBar", "TabNames", 0, 0 },
    { "TextView", "Context", 0, 0 },
    { "TextView", "LinkUnderline", "linkUnderline", "boolean" },
    { "TextView", "Text", "text", "qstring" },
    { "TextView", "TextFormat", "textFormat", "enum" },
    { "User", "UserClassHeader", 0, 0 },
    { "User", "UserClassName", 0, 0 },
    { 0, 0, 0, 0 }
};

/*
  This is the number of supported color groups in a palette, and
  supported color roles in a color group. Changing these constants is
  dangerous.
*/
static const int NumColorRoles = 14;

static bool isTrue( const TQString& val )
{
    return val.lower() == TQString( "true" );
}

static AttributeMap attribute( const TQString& name, const TQString& val )
{
    AttributeMap attr;
    attr.insert( name, val );
    return attr;
}

static TQString entitize( const TQString& str )
{
    TQString t = str;
    t.replace( '&', TQString("&amp;") );
    t.replace( '>', TQString("&gt;") );
    t.replace( '<', TQString("&lt;") );
    t.replace( '"', TQString("&quot;") );
    t.replace( '\'', TQString("&apos;") );
    return t;
}

TQString Dlg2Ui::alias( const TQString& name ) const
{
    if ( yyAliasMap.contains(name) )
	return yyAliasMap[name];
    else
	return name;
}

TQString Dlg2Ui::opening( const TQString& tag, const AttributeMap& attr )
{
    TQString t = TQChar( '<' ) + tag;
    AttributeMap::ConstIterator a = attr.begin();
    while ( a != attr.end() ) {
	t += TQChar( ' ' ) + a.key() + TQString( "=\"" ) + entitize( *a ) +
	     TQChar( '"' );
	++a;
    }
    t += TQChar( '>' );
    return t;
}

TQString Dlg2Ui::closing( const TQString& tag )
{
    return opening( TQChar('/') + tag );
}

void Dlg2Ui::error( const TQString& message )
{
    if ( numErrors++ == 0 )
	TQMessageBox::warning( 0, yyFileName, message );
}

void Dlg2Ui::syntaxError()
{
    error( TQString("Sorry, I met a random syntax error. I did what I could, but"
		   " that was not enough."
		   "<p>You might want to write to"
		   " <tt>qt-bugs@trolltech.com</tt> about this incident.") );
}

TQString Dlg2Ui::getTextValue( const TQDomNode& node )
{
    if ( node.childNodes().count() > 1 ) {
	syntaxError();
	return TQString::null;
    }

    if ( node.childNodes().count() == 0 )
	return TQString::null;

    TQDomText child = node.firstChild().toText();
    if ( child.isNull() ) {
	syntaxError();
	return TQString::null;
    }
    TQString t = child.data().stripWhiteSpace();
    t.replace( "\\\\", "\\" );
    t.replace( "\\n", "\n" );
    return t;
}

TQVariant Dlg2Ui::getValue( const TQDomNodeList& children, const TQString& tagName,
			   const TQString& type )
{
    for ( int i = 0; i < (int) children.count(); i++ ) {
	TQDomNode n = children.item( i );
	if ( n.toElement().tagName() == tagName )
	    return getValue( n.toElement(), tagName, type );
    }
    return TQVariant();
}

void Dlg2Ui::emitHeader()
{
    yyOut += TQString( "<!DOCTYPE UI><UI version=\"3.0\" stdsetdef=\"1\">\n" );
}

void Dlg2Ui::emitFooter()
{
    yyOut += TQString( "</UI>\n" );
}

void Dlg2Ui::emitSimpleValue( const TQString& tag, const TQString& value,
			      const AttributeMap& attr )
{
    yyOut += yyIndentStr + opening( tag, attr ) + entitize( value ) +
	     closing( tag ) + TQChar( '\n' );
}

void Dlg2Ui::emitOpening( const TQString& tag, const AttributeMap& attr )
{
    yyOut += yyIndentStr + opening( tag, attr ) + TQChar( '\n' );
    yyIndentStr += TQString( "    " );
}

void Dlg2Ui::emitClosing( const TQString& tag )
{
    yyIndentStr.truncate( yyIndentStr.length() - 4 );
    yyOut += yyIndentStr + closing( tag ) + TQChar( '\n' );
}

void Dlg2Ui::emitOpeningWidget( const TQString& className )
{
    AttributeMap attr = attribute( TQString("class"), className );
    if ( yyGridColumn >= 0 ) {
	attr.insert( TQString("row"), TQString::number(yyGridRow) );
	attr.insert( TQString("column"), TQString::number(yyGridColumn) );
	yyGridColumn = -1;
    }
    emitOpening( TQString("widget"), attr );
}

TQString Dlg2Ui::widgetClassName( const TQDomElement& e )
{
    if ( e.tagName() == TQString("User") ) {
	return getValue( e.childNodes(), TQString("UserClassName") )
			 .toString();
    } else if ( e.tagName() == TQString("DlgWidget") ) {
	return TQString( "TQWidget" );
    } else {
	return TQString( "TQ" ).append( e.tagName() );
    }
}

void Dlg2Ui::emitColor( const TQColor& color )
{
    emitOpening( TQString("color") );
    emitSimpleValue( TQString("red"), TQString::number(color.red()) );
    emitSimpleValue( TQString("green"), TQString::number(color.green()) );
    emitSimpleValue( TQString("blue"), TQString::number(color.blue()) );
    emitClosing( TQString("color") );
}

void Dlg2Ui::emitColorGroup( const TQString& name, const TQColorGroup& group )
{
    emitOpening( name );
    for ( int i = 0; i < NumColorRoles; i++ )
	emitColor( group.color((TQColorGroup::ColorRole) i) );
    emitClosing( name );
}

void Dlg2Ui::emitVariant( const TQVariant& val, const TQString& stringType )
{
    if ( val.isValid() ) {
	switch ( val.type() ) {
	case TQVariant::String:
	    emitSimpleValue( stringType, val.toString() );
	    break;
	case TQVariant::CString:
	    emitSimpleValue( TQString("cstring"), val.toString() );
	    break;
	case TQVariant::Bool:
	    emitSimpleValue( TQString("bool"),
			     TQString(val.toBool() ? "true" : "false") );
	    break;
	case TQVariant::Int:
	case TQVariant::UInt:
	    emitSimpleValue( TQString("number"), val.toString() );
	    break;
	case TQVariant::Rect:
	    emitOpening( TQString("rect") );
	    emitSimpleValue( TQString("x"), TQString::number(val.toRect().x()) );
	    emitSimpleValue( TQString("y"), TQString::number(val.toRect().y()) );
	    emitSimpleValue( TQString("width"),
			     TQString::number(val.toRect().width()) );
	    emitSimpleValue( TQString("height"),
			     TQString::number(val.toRect().height()) );
	    emitClosing( TQString("rect") );
	    break;
	case TQVariant::Point:
	    emitOpening( TQString("point") );
	    emitSimpleValue( TQString("x"), TQString::number(val.toPoint().x()) );
	    emitSimpleValue( TQString("y"), TQString::number(val.toPoint().y()) );
	    emitClosing( TQString("point") );
	    break;
	case TQVariant::Size:
	    emitOpening( TQString("size") );
	    emitSimpleValue( TQString("width"),
			     TQString::number(val.toSize().width()) );
	    emitSimpleValue( TQString("height"),
			     TQString::number(val.toSize().height()) );
	    emitClosing( TQString("size") );
	    break;
	case TQVariant::Color:
	    emitColor( val.toColor() );
	    break;
	case TQVariant::Font:
	    emitOpening( TQString("font") );
	    emitSimpleValue( TQString("family"), val.toFont().family() );
	    emitSimpleValue( TQString("pointsize"),
			     TQString::number(val.toFont().pointSize()) );
	    if ( val.toFont().bold() )
		emitSimpleValue( TQString("bold"), TQChar('1') );
	    if ( val.toFont().italic() )
		emitSimpleValue( TQString("italic"), TQChar('1') );
	    if ( val.toFont().underline() )
		emitSimpleValue( TQString("underline"), TQChar('1') );
	    if ( val.toFont().strikeOut() )
		emitSimpleValue( TQString("strikeout"), TQChar('1') );
	    emitClosing( TQString("font") );
	    break;
	case TQVariant::Palette:
	    emitOpening( TQString("palette") );
	    emitColorGroup( TQString("active"), val.toPalette().active() );
	    emitColorGroup( TQString("disabled"), val.toPalette().disabled() );
	    emitColorGroup( TQString("inactive"), val.toPalette().inactive() );
	    emitClosing( TQString("palette") );
	    break;
	default:
	    emitSimpleValue( TQString("fnord"), TQString::null );
	}
    }
}

void Dlg2Ui::emitProperty( const TQString& prop, const TQVariant& val,
			   const TQString& stringType )
{
    emitOpening( TQString("property"), attribute(TQString("name"), prop) );
    emitVariant( val, stringType );
    emitClosing( TQString("property") );
}

void Dlg2Ui::emitAttribute( const TQString& attr, const TQVariant& val,
			    const TQString& stringType )
{
    emitOpening( TQString("attribute"), attribute(TQString("name"), attr) );
    emitVariant( val, stringType );
    emitClosing( TQString("attribute") );
}

void Dlg2Ui::emitOpeningLayout( bool needsWidget, const TQString& layoutKind,
				const TQString& name, int border,
				int autoBorder )
{
    TQString namex = name;

    if ( namex.isEmpty() )
	namex = TQString( "Layout%1" ).arg( uniqueLayout++ );

    if ( needsWidget ) {
	emitOpeningWidget( TQString("TQLayoutWidget") );
	emitProperty( TQString("name"), namex.latin1() );
    }
    emitOpening( layoutKind );
    if ( !needsWidget )
	emitProperty( TQString("name"), namex.latin1() );
    if ( border != 5 )
	emitProperty( TQString("margin"), border );
    if ( autoBorder != 5 )
	emitProperty( TQString("spacing"), autoBorder );
    yyLayoutDepth++;
}

void Dlg2Ui::flushWidgets()
{
    TQRegExp widgetForLayout( TQString("Q(?:[HV]Box|Grid)") );

    while ( !yyWidgetMap.isEmpty() ) {
	TQString className = widgetClassName( *yyWidgetMap.begin() );
	if ( !widgetForLayout.exactMatch(className) ) {
	    emitOpeningWidget( className );
	    emitWidgetBody( *yyWidgetMap.begin(), false );
	    emitClosing( TQString("widget") );
	}
	yyWidgetMap.remove( yyWidgetMap.begin() );
    }
}

void Dlg2Ui::emitClosingLayout( bool needsWidget, const TQString& layoutKind )
{
    yyLayoutDepth--;
    /*
      TQt Designer can deal with layouted widgets and with
      fixed-position widgets, but not both at the same time. If such a
      thing happens, we arbitrarily put the fixed-position widgets in
      the layout so that they at least show up in TQt Designer.
    */
    if ( yyLayoutDepth == 0 )
	flushWidgets();

    emitClosing( layoutKind );
    if ( needsWidget )
	emitClosing( TQString("widget") );
}

bool Dlg2Ui::isWidgetType( const TQDomElement& e )
{
    return yyWidgetTypeSet.contains( e.tagName() );
}

void Dlg2Ui::emitSpacer( int spacing, int stretch )
{
    TQString orientationStr;
    TQSize sizeHint;
    TQString sizeType = TQString( "Fixed" );

    if ( yyBoxKind == TQString("hbox") ) {
	orientationStr = TQString( "Horizontal" );
	sizeHint = TQSize( spacing, 20 );
    } else {
	orientationStr = TQString( "Vertical" );
	sizeHint = TQSize( 20, spacing );
    }
    if ( stretch > 0 )
	sizeType = TQString( "Expanding" );

    emitOpening( TQString("spacer") );
    emitProperty( TQString("name"),
		  TQString("Spacer%1").arg(uniqueSpacer++).latin1() );
    emitProperty( TQString("orientation"), orientationStr, TQString("enum") );
    if ( spacing > 0 )
	emitProperty( TQString("sizeHint"), sizeHint, TQString("qsize") );
    emitProperty( TQString("sizeType"), sizeType, TQString("enum") );
    emitClosing( TQString("spacer") );
}

TQString Dlg2Ui::filteredFlags( const TQString& flags, const TQRegExp& filter )
{
    TQRegExp evil( TQString("[^0-9A-Z_a-z|]") );

    TQString f = flags;
    f.replace( evil, TQString::null );
    TQStringList splitted = TQStringList::split( TQChar('|'), f );
    return splitted.grep( filter ).join( TQChar('|') );
}

void Dlg2Ui::emitFrameStyleProperty( int style )
{
    TQString shape;
    TQString shadow;

    switch ( style & TQFrame::MShape ) {
    case TQFrame::Box:
	shape = TQString( "Box" );
	break;
    case TQFrame::Panel:
	shape = TQString( "Panel" );
	break;
    case TQFrame::WinPanel:
	shape = TQString( "WinPanel" );
	break;
    case TQFrame::HLine:
	shape = TQString( "HLine" );
	break;
    case TQFrame::VLine:
	shape = TQString( "VLine" );
	break;
    case TQFrame::StyledPanel:
	shape = TQString( "StyledPanel" );
	break;
    case TQFrame::PopupPanel:
	shape = TQString( "PopupPanel" );
	break;
    case TQFrame::MenuBarPanel:
	shape = TQString( "MenuBarPanel" );
	break;
    case TQFrame::ToolBarPanel:
	shape = TQString( "ToolBarPanel" );
	break;
    case TQFrame::LineEditPanel:
	shape = TQString( "LineEditPanel" );
	break;
    case TQFrame::TabWidgetPanel:
	shape = TQString( "TabWidgetPanel" );
	break;
    case TQFrame::GroupBoxPanel:
	shape = TQString( "GroupBoxPanel" );
	break;
    default:
	shape = TQString( "NoFrame" );
    }

    switch ( style & TQFrame::MShadow ) {
    case TQFrame::Raised:
	shadow = TQString( "Raised" );
	break;
    case TQFrame::Sunken:
	shadow = TQString( "Sunken" );
	break;
    default:
	shadow = TQString( "Plain" );
    }

    emitProperty( TQString("frameShape"), shape, TQString("enum") );
    emitProperty( TQString("frameShadow"), shadow, TQString("enum") );
}

void Dlg2Ui::emitWidgetBody( const TQDomElement& e, bool layouted )
{
    TQRegExp align( TQString("^(?:Align|WordBreak$)") );
    TQRegExp frameShape( TQString(
	    "^(?:NoFrame|Box|(?:Win|Styled|Popup|(?:Menu|Tool)Bar)?Panel|"
	    "[HV]Line)$") );
    TQRegExp frameShadow( TQString( "^(?:Plain|Raised|Sunken)$") );
    TQRegExp numeric( TQString("[0-9]+(?:\\.[0-9]*)?|\\.[0-9]+") );
    TQRegExp connex( TQString(
	    "\\s*\\[(BaseClass|P(?:ublic|rotected))\\]\\s*([0-9A-Z_a-z]+)\\s*"
	    "-->\\s*([0-9A-Z_a-z]+)\\s*(\\([^()]*\\))\\s*") );
    TQRegExp qdialogSlots( TQString(
	    "done\\(\\s*int\\s*\\)|(?:accept|reject)\\(\\s*\\)") );

    TQString userClassHeader;
    TQString userClassName;
    TQString parentTagName;
    TQString name;
    TQString variableName;
    TQMap<TQString, int> pp;

    TQDomNode n = e;
    while ( !n.isNull() ) {
	if ( isWidgetType(n.toElement()) ) {
	    parentTagName = n.toElement().tagName();
	    pp = yyPropertyMap[parentTagName];
	    n = n.firstChild();
	} else {
	    TQString tagName = n.toElement().tagName();

	    TQMap<TQString, int>::ConstIterator p = pp.find( tagName );
	    if ( p == pp.end() ) {
		/*
		  These properties are not in the propertyDefs table,
		  since they are found in many classes anyway and need
		  to be treated the same in each case.
		*/
		if ( tagName == TQString("Alignement") ||
		     tagName == TQString("Alignment") ) {
		    TQString flags = getValue( n.toElement(), tagName )
				    .toString();
		    flags = filteredFlags( flags, align );
		    if ( !flags.isEmpty() )
			emitProperty( TQString("alignment"), flags,
				      TQString("set") );
		} else if ( tagName == TQString("ItemList") ) {
		    TQDomNode child = n.firstChild();
		    while ( !child.isNull() ) {
			if ( child.toElement().tagName() == TQString("Item") ) {
			    TQString text = getTextValue( child );
			    emitOpening( TQString("item") );
			    emitProperty( TQString("text"), text );
			    emitClosing( TQString("item") );
			}
			child = child.nextSibling();
		    }
		}
	    } else {
		TQString propertyName( propertyDefs[*p].qtName );

		if ( propertyName.isEmpty() ) {
		    /*
		      These properties are in the propertyDefs table,
		      but they have no direct TQt equivalent.
		    */
		    if ( parentTagName == TQString("ComboBox") ) {
			if ( tagName == TQString("Style") ) {
			    if ( getTextValue(n) == TQString("ReadWrite") )
				emitProperty( TQString("editable"),
					      TQVariant(true) );
			}
		    } else if ( parentTagName == TQString("DlgWidget") ) {
			if ( tagName == TQString("Name") ) {
			    name = getTextValue( n );
			} else if ( tagName == TQString("Rect") ) {
			    TQRect rect = getValue( n.toElement(), tagName,
						   TQString("qrect") )
					 .toRect();
			    if ( !layouted )
				emitProperty( TQString("geometry"), rect,
					      TQString("qrect") );
			} else if ( tagName == TQString("SignalConnection") ) {
			    TQDomNode child = n.firstChild();
			    while ( !child.isNull() ) {
				if ( child.toElement().tagName() ==
				     TQString("Signal") ) {
				    TQString text = getTextValue( child );
				    if ( connex.exactMatch(text) ) {
					DlgConnection c;
					c.sender = getValue(
						n.parentNode().childNodes(),
						TQString("Name") ).toString();
					c.signal = connex.cap( 2 ) +
						   connex.cap( 4 );
					c.slot = connex.cap( 3 ) +
						 connex.cap( 4 );
					yyConnections.append( c );

					if ( connex.cap(1) !=
					     TQString("BaseClass") &&
					     !qdialogSlots.exactMatch(c.slot) )
					    yySlots.insert( c.slot,
							    connex.cap(1) );
				    }
				}
				child = child.nextSibling();
			    }
			} else if ( tagName == TQString("Variable") ) {
			    variableName = getTextValue( n );
			}
		    } else if ( parentTagName == TQString("Frame") ) {
			if ( tagName == TQString("Style") ) {
			    int style = getValue( n.toElement(), tagName,
						  TQString("integer") ).toInt();
			    emitFrameStyleProperty( style );
			}
		    } else if ( parentTagName == TQString("LCDNumber") ) {
			if ( tagName == TQString("Value") ) {
			    TQString text = getValue( n.toElement(), tagName )
					   .toString();
			    if ( numeric.exactMatch(text) )
				emitProperty( TQString("value"),
					      text.toDouble() );
			}
		    } else if ( parentTagName == TQString("ListView") ) {
			if ( tagName == TQString("HeaderInformation") ) {
			    int columnNo = 1;
			    TQDomNode child = n.firstChild();
			    while ( !child.isNull() ) {
				if ( child.toElement().tagName() ==
				     TQString("Header") ) {
				    TQString text = getValue( child.childNodes(),
							     TQString("Text") )
						   .toString();
				    if ( text.isEmpty() )
					text = TQString( "Column %1" )
					       .arg( columnNo );
				    emitOpening( TQString("column") );
				    emitProperty( TQString("text"), text );
				    emitClosing( TQString("column") );
				}
				child = child.nextSibling();
				columnNo++;
			    }
			}
		    } else if ( parentTagName == TQString("TabBar") ) {
			if ( tagName == TQString("TabNames") ) {
			    TQDomNode child = n.firstChild();
			    while ( !child.isNull() ) {
				if ( child.toElement().tagName() ==
				     TQString("Tab") ) {
				    TQString text = getTextValue( child );
				    emitOpeningWidget( TQString("TQWidget") );
				    emitProperty( TQString("name"), "tab" );
				    emitAttribute( TQString("title"), text );
				    emitClosing( TQString("widget") );
				}
				child = child.nextSibling();
			    }
			}
		    } else if ( parentTagName == TQString("User") ) {
			if ( tagName == TQString("UserClassHeader") ) {
			    userClassHeader = getTextValue( n );
			} else if ( tagName == TQString("UserClassName") ) {
			    userClassName = getTextValue( n );
			}
		    }
		} else {
		    /*
		      These properties are in the propertyDefs table;
		      they have a direct TQt equivalent.
		    */
		    TQString type( propertyDefs[*p].type );
		    TQVariant val = getValue( n.toElement(), tagName, type );

		    if ( type == TQString("qstring") )
			type = TQString( "string" );

		    bool omit = false;
		    if ( propertyName == TQString("backgroundOrigin") &&
			 val.toString() == TQString("WidgetOrigin") )
			omit = true;
		    if ( propertyName == TQString("enabled") && val.toBool() )
			omit = true;
		    if ( propertyName == TQString("minimumSize") &&
			 val.toSize() == TQSize(-1, -1) )
			omit = true;
		    if ( propertyName == TQString("maximumSize") &&
			 val.toSize() == TQSize(32767, 32767) )
			omit = true;

		    if ( !omit )
			emitProperty( propertyName, val, type );
		}
	    }
	    n = n.nextSibling();
	}
    }

    if ( !variableName.isEmpty() ) {
	yyAliasMap.insert( name, variableName );
	name = variableName;
    }
    if ( !name.isEmpty() )
	emitProperty( TQString("name"), name.latin1() );

    if ( !userClassName.isEmpty() )
	yyCustomWidgets.insert( userClassName, userClassHeader );
}

bool Dlg2Ui::checkTagName( const TQDomElement& e, const TQString& tagName )
{
    bool ok = ( e.tagName() == tagName );
    if ( !ok )
	syntaxError();
    return ok;
}

TQString Dlg2Ui::normalizeType( const TQString& type )
{
    TQString t = type;
    if ( t.isEmpty() || t == TQString("enum") || t == TQString( "qcstring" ) ||
	 t == TQString("set") )
	t = TQString( "qstring" );
    return t;
}

TQVariant Dlg2Ui::getValue( const TQDomElement& e, const TQString& tagName,
			   const TQString& type )
{
    TQVariant val;

    if ( e.tagName() != tagName )
	return val;

    TQString t = e.attributes().namedItem( "type" ).toAttr().value();
    if ( normalizeType(t) != normalizeType(type) )
	return val;

    if ( type == TQString("integer") ) {
	return getTextValue( e ).toInt();
    } else if ( type == TQString("boolean") ) {
	return TQVariant( isTrue(getTextValue(e)) );
    } else if ( type == TQString("double") ) {
	return getTextValue( e ).toDouble();
    } else if ( type == TQString("qcstring") ) {
	return getTextValue( e ).latin1();
    } else if ( type == TQString("enum") || type == TQString("qstring") ||
		type == TQString("set") ) {
	return getTextValue( e );
    } else {
	TQDomNodeList children = e.childNodes();

	if ( type == TQString("qsize") ) {
	    int width = getValue( children, TQString("Width"),
				  TQString("integer") ).toInt();
	    int height = getValue( children, TQString("Height"),
				   TQString("integer") ).toInt();
	    return TQSize( width, height );
	} else if ( type == TQString("qrect") ) {
	    int x = getValue( children, TQString("X"), TQString("integer") )
		    .toInt();
	    int y = getValue( children, TQString("Y"), TQString("integer") )
		    .toInt();
	    int width = getValue( children, TQString("Width"),
				  TQString("integer") ).toInt();
	    int height = getValue( children, TQString("Height"),
				   TQString("integer") ).toInt();
	    return TQRect( x, y, width, height );
	} else if ( type == TQString("qpoint") ) {
	    int x = getValue( children, TQString("X"), TQString("integer") )
		    .toInt();
	    int y = getValue( children, TQString("Y"), TQString("integer") )
		    .toInt();
	    return TQPoint( x, y );
	} else if ( type == TQString("qpalette") ) {
	    TQColorGroup active = getValue( children, TQString("Active"),
					   TQString("qcolorgroup") )
					   .toColorGroup();
	    TQColorGroup disabled = getValue( children, TQString("Disabled"),
					   TQString("qcolorgroup") )
					   .toColorGroup();
	    TQColorGroup inactive = getValue( children, TQString("Inactive"),
					   TQString("qcolorgroup") )
					   .toColorGroup();
	    return TQPalette( active, disabled, inactive );
	} else if ( type == TQString("qfont") ) {
	    TQString family = getValue( children, TQString("Family"),
				       TQString("qstring") ).toString();
	    int pointSize = getValue( children, TQString("PointSize"),
				      TQString("integer") ).toInt();
	    int weight = getValue( children, TQString("weight"),
				   TQString("integer") ).toInt();
	    bool italic = getValue( children, TQString("Italic"),
				    TQString("boolean") ).toBool();
	    bool underline = getValue( children, TQString("Underline"),
				       TQString("boolean") ).toBool();
	    bool strikeOut = getValue( children, TQString("StrikeOut"),
				       TQString("boolean") ).toBool();
	    int styleHint = getValue( children, TQString("StyleHint"),
				      TQString("integer") ).toInt();

	    TQFont f;
	    if ( !family.isEmpty() )
		f.setFamily( family );
	    if ( pointSize != 0 )
		f.setPointSize( pointSize );
	    if ( weight != 0 )
		f.setWeight( weight );
	    f.setItalic( italic );
	    f.setUnderline( underline );
	    f.setStrikeOut( strikeOut );
	    if ( styleHint != 0 )
		f.setStyleHint( (TQFont::StyleHint) styleHint );
	    return f;
	} else if ( type == TQString("qcolor") ) {
	    // if any component missing, zero is to be assumed
	    int red = getValue( children, TQString("Red"), TQString("integer") )
		      .toInt();
	    int green = getValue( children, TQString("Green"),
				  TQString("integer") ).toInt();
	    int blue = getValue( children, TQString("Blue"), TQString("integer") )
		       .toInt();
	    return TQColor( red, green, blue );
	} else if ( type == TQString("qcolorgroup") ) {
	    static const TQColorGroup::ColorRole roles[NumColorRoles] = {
		TQColorGroup::Foreground, TQColorGroup::Button,
		TQColorGroup::Light, TQColorGroup::Midlight, TQColorGroup::Dark,
		TQColorGroup::Mid, TQColorGroup::Text, TQColorGroup::BrightText,
		TQColorGroup::ButtonText, TQColorGroup::Base,
		TQColorGroup::Background, TQColorGroup::Shadow,
		TQColorGroup::Highlight, TQColorGroup::HighlightedText
	    };
	    static const char * const roleNames[NumColorRoles] = {
		"Foreground", "Button", "Light", "MidLight", "Dark", "Mid",
		"Text", "BrightText", "ButtonText", "Base", "Background",
		"Shadow", "HighLighted", "HighLightedText"
	    };
	    TQColorGroup group;

	    for ( int i = 0; i < NumColorRoles; i++ )
		group.setColor( roles[i],
				getValue(children, TQString(roleNames[i]),
					 TQString("qcolor")).toColor() );
	    return group;
	} else {
	    syntaxError();
	}
    }
    return val;
}

void Dlg2Ui::matchDialogCommon( const TQDomElement& dialogCommon )
{
    if ( !checkTagName(dialogCommon, TQString("DialogCommon")) )
	return;

    TQString sourceDir;
    TQString classHeader;
    TQString classSource;
    TQString dataHeader;
    TQString dataSource;
    TQString dataName;
    TQString windowBaseClass( "TQDialog" );
    bool isCustom = false;
    TQString customBaseHeader;
    TQString windowCaption;

    yyClassName = "Form1";

    TQDomNode n = dialogCommon.firstChild();
    while ( !n.isNull() ) {
	TQString tagName = n.toElement().tagName();
	TQString val = getTextValue( n );

	if ( tagName == TQString("SourceDir") ) {
	    sourceDir = val;
	} else if ( tagName == TQString("ClassHeader") ) {
	    classHeader = val;
	} else if ( tagName == TQString("ClassSource") ) {
	    classSource = val;
	} else if ( tagName == TQString("ClassName") ) {
	    yyClassName = val;
	} else if ( tagName == TQString("DataHeader") ) {
	    dataHeader = val;
	} else if ( tagName == TQString("DataSource") ) {
	    dataSource = val;
	} else if ( tagName == TQString("DataName") ) {
	    dataName = val;
	} else if ( tagName == TQString("WindowBaseClass") ) {
	    if ( val == TQString("Custom") )
		isCustom = true;
	    else
		windowBaseClass = val;
	} else if ( tagName == TQString("IsModal") ) {
	} else if ( tagName == TQString("CustomBase") ) {
	    windowBaseClass = val;
	} else if ( tagName == TQString("CustomBaseHeader") ) {
	    customBaseHeader = val;
	} else if ( tagName == TQString("WindowCaption") ) {
	    windowCaption = val;
	}
	n = n.nextSibling();
    }

    emitSimpleValue( TQString("class"), yyClassName );
    emitOpeningWidget( windowBaseClass );

    if ( windowCaption.isEmpty() )
	windowCaption = yyClassName;
    emitProperty( TQString("name"), yyClassName.latin1() );
    emitProperty( TQString("caption"), windowCaption );

    if ( isCustom )
	yyCustomWidgets.insert( windowBaseClass, customBaseHeader );
}

bool Dlg2Ui::needsTQLayoutWidget( const TQDomElement& e )
{
    TQRegExp widgetExists( TQString("WidgetLayout|Layout_Widget") );

    // we should also check that the widget is not a TQHBox, TQVBox, or TQGrid
    TQString grandpa = e.parentNode().parentNode().toElement().tagName();
    return !widgetExists.exactMatch( grandpa );
}

void Dlg2Ui::matchBoxLayout( const TQDomElement& boxLayout )
{
    TQString directionStr;
    TQString prevBoxKind = yyBoxKind;
    int border = 5;
    int autoBorder = 5;
    TQString name;
    bool needsWidget = needsTQLayoutWidget( boxLayout );
    bool opened = false;

    TQDomNode n = boxLayout.firstChild();
    while ( !n.isNull() ) {
	TQString tagName = n.toElement().tagName();

	if ( tagName == TQString("Children") ) {
	    if ( !opened ) {
		emitOpeningLayout( needsWidget, yyBoxKind, name, border,
				   autoBorder );
		if ( !directionStr.isEmpty() )
		    emitProperty( TQString("direction"), directionStr,
				  TQString("enum") );
		opened = true;
	    }
	    matchLayout( n.toElement() );
	} else {
	    TQString val = getTextValue( n );

	    if ( tagName == TQString("Direction") ) {
		if ( val == TQString("LeftToRight") ) {
		    yyBoxKind = TQString( "hbox" );
		} else if ( val == TQString("RightToLeft") ) {
		    directionStr = val;
		    yyBoxKind = TQString( "hbox" );
		} else if ( val == TQString("TopToBottom") ) {
		    yyBoxKind = TQString( "vbox" );
		} else if ( val == TQString("BottomToTop") ) {
		    directionStr = val;
		    yyBoxKind = TQString( "vbox" );
		} else {
		    syntaxError();
		}
	    } else if ( tagName == TQString("Border") ) {
		border = val.toInt();
	    } else if ( tagName == TQString("AutoBorder") ) {
		autoBorder = val.toInt();
	    } else if ( tagName == TQString("Name") ) {
		name = val;
	    }
	}

	n = n.nextSibling();
    }
    if ( opened ) {
	emitClosingLayout( needsWidget, yyBoxKind );
	yyBoxKind = prevBoxKind;
    }
}

void Dlg2Ui::matchBoxSpacing( const TQDomElement& boxSpacing )
{
    int spacing = 7;

    TQDomNode n = boxSpacing.firstChild();
    while ( !n.isNull() ) {
	TQString val = getTextValue( n );

	if ( n.toElement().tagName() == TQString("Spacing") )
	    spacing = val.toInt();
	n = n.nextSibling();
    }
    emitSpacer( spacing, 0 );
}

void Dlg2Ui::matchBoxStretch( const TQDomElement& boxStretch )
{
    int stretch = 1;

    TQDomNode n = boxStretch.firstChild();
    while ( !n.isNull() ) {
	TQString val = getTextValue( n );

	if ( n.toElement().tagName() == TQString("Stretch") )
	    stretch = val.toInt();
	n = n.nextSibling();
    }
    emitSpacer( 0, stretch );
}

void Dlg2Ui::matchGridLayout( const TQDomElement& gridLayout )
{
    int oldGridRow = yyGridRow;
    int oldGridColumn = yyGridColumn;
    int border = 5;
    int autoBorder = 5;
    TQString name;
    TQString menu;
    bool needsWidget = needsTQLayoutWidget( gridLayout );
    bool opened = false;

    TQDomNode n = gridLayout.firstChild();
    while ( !n.isNull() ) {
	TQString tagName = n.toElement().tagName();

	if ( tagName == TQString("Children") ) {
	    if ( !opened ) {
		emitOpeningLayout( needsWidget, TQString("grid"), name, border,
				   autoBorder );
		yyGridRow = -1;
		yyGridColumn = -1;
		opened = true;
	    }
	    matchLayout( n.toElement() );
	} else {
	    if ( tagName == TQString("Border") ) {
		border = getTextValue( n ).toInt();
	    } else if ( tagName == TQString("AutoBorder") ) {
		autoBorder = getTextValue( n ).toInt();
	    } else if ( tagName == TQString("Name") ) {
		name = getTextValue( n );
	    } else if ( tagName == TQString("Menu") ) {
		menu = getTextValue( n );
	    }
	}
	n = n.nextSibling();
    }
    if ( opened )
	emitClosingLayout( needsWidget, TQString("grid") );
    yyGridRow = oldGridRow;
    yyGridColumn = oldGridColumn;
}

void Dlg2Ui::matchGridRow( const TQDomElement& gridRow )
{
    yyGridRow++;

    TQDomNode n = gridRow.firstChild();
    while ( !n.isNull() ) {
	TQString tagName = n.toElement().tagName();

	if ( tagName == TQString("Children") ) {
	    yyGridColumn = 0;
	    matchLayout( n.toElement() );
	}
	n = n.nextSibling();
    }
}

void Dlg2Ui::matchGridSpacer( const TQDomElement& gridSpacer )
{
    if ( !gridSpacer.firstChild().isNull() )
	syntaxError();
}

void Dlg2Ui::matchLayoutWidget( const TQDomElement& layoutWidget )
{
    TQDomElement children;
    TQString widget;

    TQDomNode n = layoutWidget.firstChild();
    while ( !n.isNull() ) {
	TQString tagName = n.toElement().tagName();

	if ( tagName == TQString("Children") )
	    children = n.toElement();
	else if ( tagName == TQString("Widget") )
	    widget = getTextValue( n );
	n = n.nextSibling();
    }

    if ( !widget.isEmpty() ) {
	TQMap<TQString, TQDomElement>::Iterator w = yyWidgetMap.find( widget );
	if ( w == yyWidgetMap.end() ) {
	    syntaxError();
	} else {
	    TQString className = widgetClassName( *w );
	    if ( className == TQString("TQHBox") ||
		 className == TQString("TQVBox") ) {
		bool needsWidget = needsTQLayoutWidget( layoutWidget );

		TQString prevBoxKind = yyBoxKind;
		yyBoxKind = className.mid( 1 ).lower();

		int spacing = getValue( (*w).childNodes(), TQString("Spacing"),
					TQString("integer") ).toInt();
		if ( spacing < 1 )
		    spacing = 5;

		emitOpeningLayout( needsWidget, yyBoxKind, widget, 0, spacing );
		if ( !children.isNull() )
		    matchLayout( children );
		emitClosingLayout( needsWidget, yyBoxKind );
		yyBoxKind = prevBoxKind;
	    } else if ( className == TQString("TQGrid") ) {
		bool needsWidget = needsTQLayoutWidget( layoutWidget );
		int n = 0;

		TQString direction = getValue( (*w).childNodes(),
					      TQString("Direction") ).toString();
		int rowsCols = getValue( (*w).childNodes(), TQString("RowCols"),
					 TQString("integer") ).toInt();
		if ( rowsCols == 0 )
		    rowsCols = getValue( (*w).childNodes(),
					 TQString("RowsCols"),
					 TQString("integer") ).toInt();
		if ( rowsCols < 1 )
		    rowsCols = 5;
		int spacing = getValue( (*w).childNodes(), TQString("Spacing"),
					TQString("integer") ).toInt();
		if ( spacing < 1 )
		    spacing = 5;

		emitOpeningLayout( needsWidget, TQString("grid"), widget, 0,
				   spacing );

		TQDomNode child = children.firstChild();
		while ( !child.isNull() ) {
		    if ( direction == TQString("Vertical") ) {
			yyGridColumn = n / rowsCols;
			yyGridRow = n % rowsCols;
		    } else {
			yyGridColumn = n % rowsCols;
			yyGridRow = n / rowsCols;
		    }
		    matchBox( child.toElement() );
		    n++;
		    child = child.nextSibling();
		}
		yyGridColumn = -1;
		yyGridRow = -1;
		emitClosingLayout( needsWidget, TQString("grid") );
	    } else {
		emitOpeningWidget( widgetClassName(*w) );
		emitWidgetBody( *w, true );
		if ( !children.isNull() )
		    matchLayout( children );
		emitClosing( TQString("widget") );
	    }
	    yyWidgetMap.remove( w );
	}
    }
}

void Dlg2Ui::matchBox( const TQDomElement& box )
{
    /*
      What is this jump table doing in here?
    */
    static const struct {
	const char *tagName;
	void (Dlg2Ui::*matchFunc)( const TQDomElement& );
    } jumpTable[] = {
	{ "Box_Layout", &Dlg2Ui::matchBoxLayout },
	{ "Box_Spacing", &Dlg2Ui::matchBoxSpacing },
	{ "Box_Stretch", &Dlg2Ui::matchBoxStretch },
	{ "Grid_Layout", &Dlg2Ui::matchGridLayout },
	{ "Grid_Row", &Dlg2Ui::matchGridRow },
	{ "Grid_Spacer", &Dlg2Ui::matchGridSpacer },
	{ "Layout_Widget", &Dlg2Ui::matchLayoutWidget },
	{ 0, 0 }
    };

    int i = 0;
    while ( jumpTable[i].tagName != 0 ) {
	if ( TQString(jumpTable[i].tagName) == box.tagName() ) {
	    (this->*jumpTable[i].matchFunc)( box );
	    break;
	}
	i++;
    }
    if ( jumpTable[i].tagName == 0 )
	syntaxError();
}

void Dlg2Ui::matchLayout( const TQDomElement& layout )
{
    int column = yyGridColumn;

    TQDomNode n = layout.firstChild();
    while ( !n.isNull() ) {
	if ( column != -1 )
	    yyGridColumn = column++;
	matchBox( n.toElement() );
	n = n.nextSibling();
    }
}

void Dlg2Ui::matchWidgetLayoutCommon( const TQDomElement& widgetLayoutCommon )
{
    TQDomNodeList children = widgetLayoutCommon.childNodes();

    /*
      Since we do not respect the spacing and margins specified in
      the .dlg file, the specified geometry is slightly wrong (too
      small). It still seems to be better to take it in.
    */
#if 1
    TQPoint initialPos = getValue( children, TQString("InitialPos"),
				  TQString("qpoint") ).toPoint();
    TQSize size = getValue( children, TQString("Size"), TQString("qsize") )
		 .toSize();
#endif
    TQSize minSize = getValue( children, TQString("MinSize"), TQString("qsize") )
		    .toSize();
    TQSize maxSize = getValue( children, TQString("MaxSize"), TQString("qsize") )
		    .toSize();

#if 1
    if ( initialPos == TQPoint(-1, -1) )
	initialPos = TQPoint( 0, 0 );

    emitProperty( TQString("geometry"), TQRect(initialPos, size) );
#endif
    if ( minSize != TQSize(-1, -1) )
	emitProperty( TQString("minimumSize"), minSize );
    if ( maxSize != TQSize(32767, 32767) )
	emitProperty( TQString("maximumSize"), maxSize );
}

void Dlg2Ui::matchWidget( const TQDomElement& widget )
{
    TQString name;

    TQDomNode n = widget;
    while ( !n.isNull() ) {
	if ( isWidgetType(n.toElement()) ) {
	    n = n.firstChild();
	} else {
	    if ( n.toElement().tagName() == TQString("Name") ) {
		name = getTextValue( n );
		break;
	    }
	    n = n.nextSibling();
	}
    }
    if ( name.isEmpty() )
	name = TQString( "Widget%1" ).arg( uniqueWidget++ );

    if ( yyWidgetMap.contains(name) )
	syntaxError();
    yyWidgetMap.insert( name, widget );
}

void Dlg2Ui::matchWidgets( const TQDomElement& widgets )
{
    TQDomNode n = widgets.firstChild();
    while ( !n.isNull() ) {
	matchWidget( n.toElement() );
	n = n.nextSibling();
    }
}

void Dlg2Ui::matchTabOrder( const TQDomElement& tabOrder )
{
    TQDomNode n = tabOrder.firstChild();
    while ( !n.isNull() ) {
	if ( n.toElement().tagName() == TQString("Widget") )
	    yyTabStops.append( getTextValue(n.toElement()) );
	n = n.nextSibling();
    }
}

void Dlg2Ui::matchWidgetLayout( const TQDomElement& widgetLayout )
{
    if ( !checkTagName(widgetLayout, TQString("WidgetLayout")) )
	return;

    TQDomNode n = widgetLayout.firstChild();
    while ( !n.isNull() ) {
	TQString tagName = n.toElement().tagName();

	if ( tagName == TQString("WidgetLayoutCommon") ) {
	    matchWidgetLayoutCommon( n.toElement() );
	} else if ( tagName == TQString("Widgets") ) {
	    matchWidgets( n.toElement() );
	} else if ( tagName == TQString("TabOrder") ) {
	    matchTabOrder( n.toElement() );
	} else if ( tagName == TQString("Layout") ) {
	    matchLayout( n.toElement() );
	}
	n = n.nextSibling();
    }
}

void Dlg2Ui::matchDialog( const TQDomElement& dialog )
{
    if ( !checkTagName(dialog, TQString("Dialog")) )
	return;

    TQDomNodeList nodes = dialog.childNodes();
    if ( nodes.count() == 2 ) {
	matchDialogCommon( nodes.item(0).toElement() );
	matchWidgetLayout( nodes.item(1).toElement() );
	flushWidgets();
	emitClosing( TQString("widget") );

	if ( !yyCustomWidgets.isEmpty() ) {
	    emitOpening( TQString("customwidgets") );

	    TQMap<TQString, TQString>::Iterator w = yyCustomWidgets.begin();
	    while ( w != yyCustomWidgets.end() ) {
		emitOpening( TQString("customwidget") );
		emitSimpleValue( TQString("class"), w.key() );
		if ( !(*w).isEmpty() )
		    emitSimpleValue( TQString("header"), *w,
				     attribute(TQString("location"),
					       TQString("local")) );
		emitClosing( TQString("customwidget") );
		++w;
	    }
	    emitClosing( TQString("customwidgets") );
	}

	if ( yyConnections.count() + yySlots.count() > 0 ) {
	    emitOpening( TQString("connections") );

	    TQValueList<DlgConnection>::Iterator c = yyConnections.begin();
	    while ( c != yyConnections.end() ) {
		emitOpening( TQString("connection") );
		emitSimpleValue( TQString("sender"), alias((*c).sender) );
		emitSimpleValue( TQString("signal"), (*c).signal );
		emitSimpleValue( TQString("receiver"), yyClassName );
		emitSimpleValue( TQString("slot"), (*c).slot );
		emitClosing( TQString("connection") );
		++c;
	    }

	    TQMap<TQString, TQString>::Iterator s = yySlots.begin();
	    while ( s != yySlots.end() ) {
		AttributeMap attr;
		attr.insert( TQString("access"), *s );
		attr.insert( TQString("language"), TQString("C++") );
		attr.insert( TQString("returntype"), TQString("void") );
		emitSimpleValue( TQString("slot"), s.key(), attr );
		++s;
	    }
	    emitClosing( TQString("connections") );
	}

	if ( !yyTabStops.isEmpty() ) {
	    emitOpening( TQString("tabstops") );
	    TQStringList::ConstIterator t = yyTabStops.begin();
	    while ( t != yyTabStops.end() ) {
		emitSimpleValue( TQString("tabstop"), alias(*t) );
		++t;
	    }
	    emitClosing( TQString("tabstops") );
	}
    }
}

TQStringList Dlg2Ui::convertTQtArchitectDlgFile( const TQString& fileName )
{
    int i;

    yyFileName = fileName;
    yyLayoutDepth = 0;
    yyGridRow = -1;
    yyGridColumn = -1;

    numErrors = 0;
    uniqueLayout = 1;
    uniqueSpacer = 1;
    uniqueWidget = 1;

    i = 0;
    while ( widgetTypes[i] != 0 ) {
	yyWidgetTypeSet.insert( TQString(widgetTypes[i]), 0 );
	i++;
    }

    i = 0;
    while ( propertyDefs[i].widgetName != 0 ) {
	yyPropertyMap[TQString(propertyDefs[i].widgetName)]
		.insert( TQString(propertyDefs[i].architectName), i );
	i++;
    }

    TQDomDocument doc( TQString("TQtArch") );
    TQFile f( fileName );
    if ( !f.open(IO_ReadOnly) ) {
	return TQStringList();
    }
    if ( !doc.setContent(&f) ) {
	TQString firstLine;
	f.at( 0 );
	f.readLine( firstLine, 128 );
	firstLine = firstLine.stripWhiteSpace();
	if ( firstLine.startsWith(TQString("DlgEdit:v1")) ) {
	    error( TQString("This file is a TQt Architect 1.x file. TQt Designer"
			   " can only read XML dialog files, as generated by TQt"
			   " Architect 2.1 or above."
			   "<p>To convert this file to the right format,"
			   " first install TQt Architect 2.1 (available at"
			   " <tt>http://qtarch.sourceforge.net/</tt>). Use the"
			   " <i>update20.pl</i> Perl script to update the file"
			   " to the 2.0 format. Load that file in TQt"
			   " Architect and save it. The file should now be in"
			   " XML format and loadable in TQt Designer.") );
	} else if ( firstLine.startsWith(TQString("DlgEdit::v2")) ) {
	    error( TQString("This file is a TQt Architect 2.0 file. TQt Designer"
			   " can only read XML dialog files, as generated by TQt"
			   " Architect 2.1 or above."
			   "<p>To convert this file to the right format,"
			   " first install TQt Architect 2.1 (available at"
			   " <tt>http://qtarch.sourceforge.net/</tt>). Load the"
			   " 2.0 file in TQt Architect and save it. The file"
			   " should now be in XML format and loadable in TQt"
			   " Designer.") );
	} else {
	    error( TQString("The file you gave me is not an XML file, as far as"
			   " I can tell.") );
	}

	f.close();
	return TQStringList();
    }
    f.close();

    TQDomElement root = doc.documentElement();
    if ( root.tagName() != TQString("TQtArch") ||
	 root.attributeNode("type").value() != TQString("Dialog") ) {
	error( TQString("The file you gave me is not a TQt Architect dialog"
		       " file.") );
	return TQStringList();
    }

    emitHeader();

    TQDomNode n = root.firstChild();
    while ( !n.isNull() ) {
	// there should be only one
	matchDialog( n.toElement() );
	n = n.nextSibling();
    }

    emitFooter();

    TQFile outf;
    TQString outFileName = yyClassName + TQString( ".ui" );

    outf.setName( outFileName );
    if ( !outf.open(IO_WriteOnly) ) {
	tqWarning( "dlg2ui: Could not open output file '%s'",
		  outFileName.latin1() );
	return TQStringList();
    }

    TQTextStream out;
    out.setEncoding( TQTextStream::Latin1 );
    out.setDevice( &outf );
    out << yyOut;
    outf.close();

    return TQStringList( outFileName );
}
