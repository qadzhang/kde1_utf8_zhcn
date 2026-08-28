/**********************************************************************
**
** Converts a Glade .glade file into a .ui file.
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

#include "glade2ui.h"

#include <ntqapplication.h>
#include <ntqfile.h>
#include <ntqimage.h>
#include <ntqprogressdialog.h>
#include <ntqmessagebox.h>
#include <ntqrect.h>
#include <ntqregexp.h>
#include <ntqsizepolicy.h>

#include <ctype.h>

static const struct {
    const char *gtkName;
    const char *qtName;
} classNames[] = {
    { "Custom", "Custom" },
    { "GnomeAbout", "TQDialog" },
    { "GnomeApp", "TQMainWindow" },
    { "GnomeCanvas", "TQLabel" },
    { "GnomeColorPicker", "TQComboBox" },
    { "GnomeDateEdit", "TQDateTimeEdit" },
    { "GnomeDialog", "TQDialog" },
    { "GnomeFontPicker", "TQComboBox" },
    { "GnomeIconSelection", "TQIconView" },
    { "GnomePixmap", "TQLabel" },
    { "GnomePropertyBox", "TQDialog" },
    { "GtkAccelLabel", "TQLabel" },
    { "GtkAspectFrame", "TQFrame" },
    { "GtkButton", "TQPushButton" },
    { "GtkCList", "TQListView" },
    { "GtkCTree", "TQListView" },
    { "GtkCheckButton", "TQCheckBox" },
    { "GtkCombo", "TQComboBox" },
    { "GtkDial", "TQDial" },
    { "GtkDialog", "TQDialog" },
    { "GtkEntry", "TQLineEdit" },
    { "GtkFixed", "TQLayoutWidget" },
    { "GtkFrame", "TQFrame" },
    { "GtkHPaned", "TQSplitter" },
    { "GtkHScale", "TQSlider" },
    { "GtkHScrollbar", "TQScrollBar" },
    { "GtkHSeparator", "Line" },
    { "GtkHandleBox", "TQFrame" },
    { "GtkImage", "TQLabel" },
    { "GtkLabel", "TQLabel" },
    { "GtkList", "TQListBox" },
    { "GtkNotebook", "TQTabWidget" },
    { "GtkOptionMenu", "TQComboBox" },
    { "GtkPixmap", "TQLabel" },
    { "GtkPreview", "TQLabel" },
    { "GtkProgressBar", "TQProgressBar" },
    { "GtkRadioButton", "TQRadioButton" },
    { "GtkSpinButton", "TQSpinBox" },
    { "GtkStatusbar", "TQStatusBar" },
    { "GtkText", "TQTextEdit" },
    { "GtkToggleButton", "TQPushButton" },
    { "GtkTree", "TQListView" },
    { "GtkVPaned", "TQSplitter" },
    { "GtkVScale", "TQSlider" },
    { "GtkVScrollbar", "TQScrollBar" },
    { "GtkVSeparator", "Line" },
    { "Placeholder", "TQLabel" },
    { 0, 0 }
};

static const struct {
    const char *name;
    const char *menuText;
} stockMenuItems[] = {
    { "ABOUT", "&About" },
    { "CLEAR", "C&lear" },
    { "CLOSE", "&Close" },
    { "CLOSE_WINDOW", "&Close This Window" },
    { "COPY", "&Copy" },
    { "CUT", "C&ut" },
    { "END_GAME", "&End Game" },
    { "EXIT", "E&xit" },
    { "FIND", "&Find..." },
    { "FIND_AGAIN", "Find &Again" },
    { "HINT", "&Hint" },
    { "NEW", "&New" },
    { "NEW_GAME", "&New Game" },
    { "NEW_WINDOW", "Create New &Window" },
    { "OPEN", "&Open..." },
    { "PASTE", "&Paste" },
    { "PAUSE_GAME", "&Pause Game" },
    { "PREFERENCES", "&Preferences..." },
    { "PRINT", "&Print" },
    { "PRINT_SETUP", "Print S&etup..." },
    { "PROPERTIES", "&Properties..." },
    { "REDO", "&Redo" },
    { "REDO_MOVE", "&Redo Move" },
    { "REPLACE", "&Replace..." },
    { "RESTART_GAME", "&Restart Game" },
    { "REVERT", "&Revert" },
    { "SAVE", "&Save" },
    { "SAVE_AS", "Save &As..." },
    { "SCORES", "&Scores..." },
    { "SELECT_ALL", "&Select All" },
    { "UNDO", "&Undo" },
    { "UNDO_MOVE", "&Undo Move" },
    { 0, 0 }
};

static const struct {
    const char *gtkName;
    int qtValue;
} keys[] = {
    { "BackSpace", TQt::Key_BackSpace },
    { "Delete", TQt::Key_Delete },
    { "Down", TQt::Key_Down },
    { "End", TQt::Key_End },
    { "Escape", TQt::Key_Escape },
    { "F1", TQt::Key_F1 },
    { "F10", TQt::Key_F10 },
    { "F11", TQt::Key_F11 },
    { "F12", TQt::Key_F12 },
    { "F2", TQt::Key_F2 },
    { "F3", TQt::Key_F3 },
    { "F4", TQt::Key_F4 },
    { "F5", TQt::Key_F5 },
    { "F6", TQt::Key_F6 },
    { "F7", TQt::Key_F7 },
    { "F8", TQt::Key_F8 },
    { "F9", TQt::Key_F9 },
    { "Home", TQt::Key_Home },
    { "Insert", TQt::Key_Insert },
    { "KP_Enter", TQt::Key_Enter },
    { "Left", TQt::Key_Left },
    { "Page_Down", TQt::Key_PageDown },
    { "Page_Up", TQt::Key_PageUp },
    { "Return", TQt::Key_Return },
    { "Right", TQt::Key_Right },
    { "Tab", TQt::Key_Tab },
    { "Up", TQt::Key_Up },
    { "space", TQt::Key_Space },
    { 0, 0 }
};

static TQString nonMenuText( const TQString& menuText )
{
    TQString t;
    int len = (int) menuText.length();
    if ( menuText.endsWith(TQString("...")) )
	len -= 3;
    for ( int i = 0; i < len; i++ ) {
	if ( menuText[i] != TQChar('&') )
	    t += menuText[i];
    }
    return t;
}

/*
  Some GTK dialog use hyphens in variable names. Let's take no chance.
*/
static TQString fixedName( const TQString& name )
{
    const char *latin1 = name.latin1();
    TQString fixed;

    int i = 0;
    while ( latin1 != 0 && latin1[i] != '\0' ) {
	if ( isalnum(latin1[i]) )
	    fixed += name[i];
	else
	    fixed += TQChar( '_' );
	i++;
    }
    return fixed;
}

/*
  Returns an hexadecimal rendering of a block of memory.
*/
static TQString hexed( const char *data, int length )
{
    TQString t;
    for ( int i = 0; i < length; i++ ) {
	TQString x;
	x.sprintf( "%.2x", (uchar) data[i] );
	t += x;
    }
    return t;
}

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

Glade2Ui::Glade2Ui()
{
    int i = 0;
    while ( classNames[i].gtkName != 0 ) {
	yyClassNameMap.insert( TQString(classNames[i].gtkName),
			       TQString(classNames[i].qtName) );
	i++;
    }

    i = 0;
    while ( stockMenuItems[i].name != 0 ) {
	yyStockMap.insert( TQString(stockMenuItems[i].name),
			   TQString(stockMenuItems[i].menuText) );
	i++;
    }

    i = 0;
    while ( keys[i].gtkName != 0 ) {
	yyKeyMap.insert( TQString(keys[i].gtkName), keys[i].qtValue );
	i++;
    }
}

TQString Glade2Ui::imageName( const TQString& fileName )
{
    return *yyImages.insert( fileName, TQString("image%1").arg(yyImages.count()),
			     false );
}

TQString Glade2Ui::opening( const TQString& tag, const AttributeMap& attr )
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

TQString Glade2Ui::closing( const TQString& tag )
{
    return opening( TQChar('/') + tag );
}

TQString Glade2Ui::atom( const TQString& tag, const AttributeMap& attr )
{
    TQString t = opening( tag, attr );
    t.insert( t.length() - 1, TQChar('/') );
    return t;
}

void Glade2Ui::error( const TQString& message )
{
    if ( numErrors++ == 0 )
	TQMessageBox::warning( 0, yyFileName, message );
}

void Glade2Ui::syntaxError()
{
    error( TQString("Sorry, I met a random syntax error. I did what I could, but"
		   " that was not enough."
		   "<p>You might want to write to"
		   " <tt>qt-bugs@trolltech.com</tt> about this incident.") );
}

TQString Glade2Ui::getTextValue( const TQDomNode& node )
{
    if ( node.childNodes().count() > 1 ) {
	syntaxError();
	return TQString::null;
    }

    if ( node.childNodes().count() == 0 )
	return TQString::null;

    TQDomText t = node.firstChild().toText();
    if ( t.isNull() ) {
	syntaxError();
	return TQString::null;
    }
    return t.data().stripWhiteSpace();
}

void Glade2Ui::emitHeader()
{
    yyOut += TQString( "<!DOCTYPE UI><UI version=\"3.0\" stdsetdef=\"1\">\n" );
}

void Glade2Ui::emitFooter()
{
    yyOut += TQString( "</UI>\n" );
}

void Glade2Ui::emitSimpleValue( const TQString& tag, const TQString& value,
				const AttributeMap& attr )
{
    yyOut += yyIndentStr + opening( tag, attr ) + entitize( value ) +
	     closing( tag ) + TQChar( '\n' );
}

void Glade2Ui::emitOpening( const TQString& tag, const AttributeMap& attr )
{
    yyOut += yyIndentStr + opening( tag, attr ) + TQChar( '\n' );
    yyIndentStr += TQString( "    " );
}

void Glade2Ui::emitClosing( const TQString& tag )
{
    yyIndentStr.truncate( yyIndentStr.length() - 4 );
    yyOut += yyIndentStr + closing( tag ) + TQChar( '\n' );
}

void Glade2Ui::emitAtom( const TQString& tag, const AttributeMap& attr )
{
    yyOut += yyIndentStr + atom( tag, attr ) + TQChar( '\n' );
}

void Glade2Ui::emitVariant( const TQVariant& val, const TQString& stringType )
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
	case TQVariant::Size:
	    emitOpening( TQString("size") );
	    emitSimpleValue( TQString("width"),
			     TQString::number(val.toSize().width()) );
	    emitSimpleValue( TQString("height"),
			     TQString::number(val.toSize().height()) );
	    emitClosing( TQString("size") );
	    break;
	case TQVariant::SizePolicy:
	    emitOpening( TQString("sizepolicy") );
	    emitSimpleValue( TQString("hsizetype"),
			     TQString::number((int) val.toSizePolicy()
						      .horData()) );
	    emitSimpleValue( TQString("vsizetype"),
			     TQString::number((int) val.toSizePolicy()
						      .verData()) );
	    emitClosing( TQString("sizepolicy") );
	    break;
	default:
	    emitSimpleValue( TQString("fnord"), TQString::null );
	}
    }
}

void Glade2Ui::emitProperty( const TQString& prop, const TQVariant& val,
			     const TQString& stringType )
{
    emitOpening( TQString("property"), attribute(TQString("name"), prop) );
    emitVariant( val, stringType );
    emitClosing( TQString("property") );
}

void Glade2Ui::emitFontProperty( const TQString& prop, int pointSize, bool bold )
{
    emitOpening( TQString("property"), attribute(TQString("name"), prop) );
    emitOpening( TQString("font") );
    emitSimpleValue( TQString("pointsize"), TQString::number(pointSize) );
    if ( bold )
	emitSimpleValue( TQString("bold"), TQString("1") );
    emitClosing( TQString("font") );
    emitClosing( TQString("property") );
}

void Glade2Ui::emitAttribute( const TQString& prop, const TQVariant& val,
			      const TQString& stringType )
{
    emitOpening( TQString("attribute"), attribute(TQString("name"), prop) );
    emitVariant( val, stringType );
    emitClosing( TQString("attribute") );
}

static TQString accelerate( const TQString& gtkLabel )
{
    TQString qtLabel = gtkLabel;
    qtLabel.replace( '&', TQString("&&") );
    // close but not quite right
    qtLabel.replace( TQChar('_'), TQChar('&') );
    return qtLabel;
}

static TQString decelerate( const TQString& gtkLabel )
{
    TQString qtLabel = gtkLabel;
    // ditto
    qtLabel.replace( '_', TQString::null );
    return qtLabel;
}

/*
  Converting a GTK widget to a corresponding TQt widget is sometimes
  hard. For example, a GtkScrolledWindow should sometimes be converted
  into a TQTextView, a TQTextEdit, a TQListView or whatever. What we do
  is pretty hackish, but it mostly works.
*/
TQString Glade2Ui::gtk2qtClass( const TQString& gtkClass,
			       const TQValueList<TQDomElement>& childWidgets )
{
    TQRegExp gnomeXEntry( TQString("Gnome(File|Number|Pixmap)?Entry") );

    TQString qtClass;

    if ( gtkClass == TQString("GtkScrolledWindow") ) {
	if ( childWidgets.count() == 1 ) {
	    TQString g;
	    bool editable = false;
	    bool showTitles = true;

	    TQDomNode n = childWidgets.first().firstChild();
	    while ( !n.isNull() ) {
		TQString tagName = n.toElement().tagName();
		if ( tagName == TQString("class") ) {
		    g = getTextValue( n );
		} else if ( tagName == TQString("editable") ) {
		    editable = isTrue( getTextValue(n) );
		} else if ( tagName.startsWith(TQString("show_tit")) ) {
		    showTitles = isTrue( getTextValue(n) );
		}
		n = n.nextSibling();
	    }

	    if ( g == TQString("GnomeCanvas") ||
		 g == TQString("GtkDrawingArea") ) {
		qtClass = TQString( "TQLabel" );
	    } else if ( g == TQString("GnomeIconList") ) {
		qtClass = TQString( "TQIconView" );
	    } else if ( g == TQString("GtkCList") ) {
		if ( showTitles )
		    qtClass = TQString( "TQListView" );
		else
		    qtClass = TQString( "TQListBox" );
	    } else if ( g == TQString("GtkCTree") ) {
		qtClass = TQString( "TQListView" );
	    } else if ( g == TQString("GtkList") ) {
		qtClass = TQString( "TQListBox" );
	    } else if ( g == TQString("GtkText") ) {
		if ( editable )
		    qtClass = TQString( "TQTextEdit" );
		else
		    qtClass = TQString( "TQTextView" );
	    } else if ( g == TQString("GtkTree") ) {
		qtClass = TQString( "TQListView" );
	    }
	    // else too bad (qtClass is empty)
	}
    } else if ( gtkClass == TQString("GtkWindow") ) {
	qtClass = TQString( "TQDialog" );
	if ( childWidgets.count() == 1 ) {
	    TQString g;

	    TQDomNode n = childWidgets.first().firstChild();
	    while ( !n.isNull() ) {
		TQString tagName = n.toElement().tagName();
		if ( tagName == TQString("class") )
		    g = getTextValue( n );
		n = n.nextSibling();
	    }
	    if ( g == TQString("GnomeDruid") )
		qtClass = TQString( "TQWizard" );
	}
    /*
      GnomeEntry and friends are a wrapper around a GtkEntry. We only
      want the GtkEntry child.
    */
    } else if ( !gnomeXEntry.exactMatch(gtkClass) &&
		gtkClass != TQString("GtkAlignment") &&
		gtkClass != TQString("GtkEventBox") ) {
	qtClass = yyClassNameMap[gtkClass];
	if ( qtClass.isEmpty() )
	    qtClass = TQString( "Unknown" );
    }
    return qtClass;
}

static TQString gtk2qtScrollBarMode( const TQString& scrollbarPolicy )
{
    if ( scrollbarPolicy.endsWith(TQString("_NEVER")) ) {
	return TQString( "AlwaysOff" );
    } else if ( scrollbarPolicy.endsWith(TQString("_ALWAYS")) ) {
	return TQString( "AlwaysOn" );
    } else {
	return TQString( "Auto" );
    }
}

static TQString gtk2qtSelectionMode( const TQString& selectionMode )
{
    if ( selectionMode.endsWith(TQString("_MULTIPLE")) )
	return TQString( "Multi" );
    else if ( selectionMode.endsWith(TQString("_EXTENDED")) )
	return TQString( "Extended" );
    else
	return TQString( "Single" );
}

int Glade2Ui::matchAccelOnActivate( const TQDomElement& accel )
{
    TQString key;
    TQString modifiers;

    TQDomNode n = accel.firstChild();
    while ( !n.isNull() ) {
	TQString tagName = n.toElement().tagName();
	if ( tagName == TQString("key") ) {
	    key = getTextValue( n );
	    if ( !key.startsWith(TQString("GDK_")) )
		return 0;
	} else if ( tagName == TQString("modifiers") ) {
	    modifiers = getTextValue( n );
	} else if ( tagName == TQString("signal") ) {
	    if ( getTextValue(n) != TQString("activate") )
		return 0;
	}
	n = n.nextSibling();
    }

    int flags = 0;

    if ( key.length() == 5 ) {
	flags = key[4].upper().latin1();
    } else if ( yyKeyMap.contains(key.mid(4)) ) {
	flags = yyKeyMap[key.mid(4)];
    } else {
	return 0;
    }

    if ( modifiers.contains(TQString("_CONTROL_")) )
	flags |= TQt::CTRL;
    if ( modifiers.contains(TQString("_SHIFT_")) )
	flags |= TQt::SHIFT;
    if ( modifiers.contains(TQString("_MOD1_")) )
	flags |= TQt::ALT;
    return flags;
}

void Glade2Ui::emitGtkMenu( const TQDomElement& menu )
{
    TQRegExp gnomeuiinfoMenuXItem( TQString("GNOMEUIINFO_MENU_(.+)_ITEM") );

    TQDomNode n = menu.firstChild();
    while ( !n.isNull() ) {
	TQString tagName = n.toElement().tagName();
	if ( tagName == TQString("widget") ) {
	    TQString activateHandler;
	    TQString gtkClass;
	    TQString icon;
	    TQString label;
	    TQString name;
	    TQString stockItem;
	    TQString tooltip;
	    int qtAccel = 0;

	    TQDomNode child = n.firstChild();
	    while ( !child.isNull() ) {
		TQString childTagName = child.toElement().tagName();
		if ( childTagName == TQString("accelerator") ) {
		    qtAccel = matchAccelOnActivate( child.toElement() );
		} else if ( childTagName == TQString("class") ) {
		    gtkClass = getTextValue( child );
		} else if ( childTagName == TQString("icon") ) {
		    icon = getTextValue( child );
		} else if ( childTagName == TQString("label") ) {
		    label = getTextValue( child );
		} else if ( childTagName == TQString("name") ) {
		    name = getTextValue( child );
		} else if ( childTagName == TQString("signal") ) {
		    TQString signalName;
		    TQString signalHandler;

		    TQDomNode grandchild = child.firstChild();
		    while ( !grandchild.isNull() ) {
			TQString grandchildTagName =
				grandchild.toElement().tagName();
			if ( grandchildTagName == TQString("handler") ) {
			    signalHandler = getTextValue( grandchild );
			} else if ( grandchildTagName == TQString("name") ) {
			    signalName = getTextValue( grandchild );
			}
			grandchild = grandchild.nextSibling();
		    }
		    if ( signalName == TQString("activate") )
			activateHandler = signalHandler;
		} else if ( childTagName == TQString("stock_item") ) {
		    stockItem = getTextValue( child );
		} else if ( childTagName == TQString("tooltip") ) {
		    tooltip = getTextValue( child );
		}
		child = child.nextSibling();
	    }

	    if ( label.length() + stockItem.length() == 0 ) {
		emitAtom( TQString("separator") );
	    } else {
		if ( name.isEmpty() )
		    name = TQString( "action%1" ).arg( uniqueAction++ );
		emitAtom( TQString("action"), attribute(TQString("name"), name) );

		if ( !activateHandler.isEmpty() ) {
		    TQString slot = activateHandler + TQString( "()" );
		    GladeConnection c;
		    c.sender = name;
		    c.signal = TQString( "activated()" );
		    c.slot = slot;
		    yyConnections.push_back( c );
		    yySlots.insert( slot, TQString("public") );
		}

		TQString x;
		GladeAction a;

		if ( gnomeuiinfoMenuXItem.exactMatch(stockItem) ) {
		    x = gnomeuiinfoMenuXItem.cap( 1 );
		    a.menuText = yyStockMap[x];
		    if ( x == TQString("EXIT") && qtAccel == 0 )
			qtAccel = TQt::CTRL + TQt::Key_Q;
		} else {
		    a.menuText = accelerate( label );
		}
		a.text = nonMenuText( a.menuText );
		a.toolTip = tooltip;
		a.accel = qtAccel;
		a.iconSet = icon;
		yyActions.insert( name, a );
		if ( !x.isEmpty() )
		    yyStockItemActions.insert( x, name );
	    }
	}
	n = n.nextSibling();
    }
}

void Glade2Ui::emitGtkMenuBarChildWidgets(
	const TQValueList<TQDomElement>& childWidgets )
{
    TQRegExp gnomeuiinfoMenuXTree( TQString("GNOMEUIINFO_MENU_(.+)_TREE") );

    emitOpening( TQString("menubar") );
    emitProperty( TQString("name"),
		  TQString("MenuBar%1").arg(uniqueMenuBar++).latin1() );

    TQValueList<TQDomElement>::ConstIterator c = childWidgets.begin();
    while ( c != childWidgets.end() ) {
	TQValueList<TQDomElement> grandchildWidgets;
	TQString gtkClass;
	TQString label;
	TQString name;
	TQString stockItem;

	TQDomNode n = (*c).firstChild();
	while ( !n.isNull() ) {
	    TQString tagName = n.toElement().tagName();
	    if ( tagName == TQString("class") ) {
		gtkClass = getTextValue( n );
	    } else if ( tagName == TQString("label") ) {
		label = getTextValue( n );
	    } else if ( tagName == TQString("name") ) {
		name = getTextValue( n );
	    } else if ( tagName == TQString("stock_item") ) {
		stockItem = getTextValue( n );
	    } else if ( tagName == TQString("widget") ) {
		grandchildWidgets.push_back( n.toElement() );
	    }
	    n = n.nextSibling();
	}

	if ( gtkClass == TQString("GtkMenuItem") &&
	     grandchildWidgets.count() == 1 ) {
	    TQString text;
	    if ( gnomeuiinfoMenuXTree.exactMatch(stockItem) ) {
		text = gnomeuiinfoMenuXTree.cap( 1 );
		if ( text == TQString("Files") )
		    text = TQString( "Fi&les" );
		else
		    text = TQChar( '&' ) + text.left( 1 ) +
			   text.mid( 1 ).lower();
	    } else {
		text = accelerate( label );
	    }

	    AttributeMap attr;
	    attr.insert( TQString("name"), name );
	    attr.insert( TQString("text"), text );

	    emitOpening( TQString("item"), attr );
	    emitGtkMenu( grandchildWidgets.first() );
	    emitClosing( TQString("item") );
	}
	++c;
    }
    emitClosing( TQString("menubar") );
}

void Glade2Ui::emitGtkToolbarChildWidgets(
	const TQValueList<TQDomElement>& childWidgets )
{
    TQRegExp gnomeStockPixmapX( TQString("GNOME_STOCK_PIXMAP_(.+)") );

    emitOpening( TQString("toolbar"), attribute(TQString("dock"), TQString("2")) );
    emitProperty( TQString("name"),
		  TQString("ToolBar%1").arg(uniqueToolBar++).latin1() );

    TQValueList<TQDomElement>::ConstIterator c = childWidgets.begin();
    while ( c != childWidgets.end() ) {
	TQString childName;
	TQString icon;
	TQString label;
	TQString name;
	TQString stockPixmap;
	TQString tooltip;

	TQDomNode n = (*c).firstChild();
	while ( !n.isNull() ) {
	    TQString tagName = n.toElement().tagName();
	    if ( tagName == TQString("child_name") ) {
		childName = getTextValue( n );
	    } else if ( tagName == TQString("icon") ) {
		icon = getTextValue( n );
	    } else if ( tagName == TQString("label") ) {
		label = getTextValue( n );
	    } else if ( tagName == TQString("name") ) {
		name = getTextValue( n );
	    } else if ( tagName == TQString("stock_pixmap") ) {
		stockPixmap = getTextValue( n );
	    } else if ( tagName == TQString("tooltip") ) {
		tooltip = getTextValue( n );
	    }
	    n = n.nextSibling();
	}

	if ( childName == TQString("Toolbar:button") ) {
	    TQString actionName;
	    GladeAction a;
	    a.menuText = label;
	    a.text = label;
	    a.accel = 0;
	    a.iconSet = icon;

	    if ( gnomeStockPixmapX.exactMatch(stockPixmap) ) {
		TQString x = gnomeStockPixmapX.cap( 1 );
		actionName = yyStockItemActions[x];
	    }
	    if ( actionName.isEmpty() ) {
		if ( name.isEmpty() )
		    actionName = TQString( "action%1" ).arg( uniqueAction++ );
		else
		    actionName = TQString( "action_%1" ).arg( name );
		yyActions.insert( actionName, a );
	    }
	    if ( !tooltip.isEmpty() )
		yyActions[actionName].toolTip = tooltip;

	    emitAtom( TQString("action"), attribute(TQString("name"),
						   actionName) );
	} else {
	    emitAtom( TQString("separator") );
	}
	++c;
    }
    emitClosing( TQString("toolbar") );
}

void Glade2Ui::emitPushButton( const TQString& text, const TQString& name )
{
    emitOpening( TQString("widget"),
		 attribute(TQString("class"), TQString("TQPushButton")) );
    emitProperty( TQString("name"), name.latin1() );
    emitProperty( TQString("text"), text );
    if ( name.contains(TQString("ok")) > 0 ) {
	emitProperty( TQString("default"), TQVariant(true) );
    } else if ( name.contains(TQString("help")) > 0 ) {
	emitProperty( TQString("accel"), (int) TQt::Key_F1 );
    }
    emitClosing( TQString("widget") );
}

void Glade2Ui::attach( AttributeMap *attr, int leftAttach, int rightAttach,
		       int topAttach, int bottomAttach )
{
    if ( leftAttach >= 0 ) {
	attr->insert( TQString("row"), TQString::number(topAttach) );
	attr->insert( TQString("column"), TQString::number(leftAttach) );
	if ( bottomAttach - topAttach != 1 )
	    attr->insert( TQString("rowspan"),
			 TQString::number(bottomAttach - topAttach) );
	if ( rightAttach - leftAttach != 1 )
	    attr->insert( TQString("colspan"),
			 TQString::number(rightAttach - leftAttach) );
    }
}

void Glade2Ui::emitSpacer( const TQString& orientation, int leftAttach,
			   int rightAttach, int topAttach, int bottomAttach )
{
    AttributeMap attr;
    attach( &attr, leftAttach, rightAttach, topAttach, bottomAttach );
    emitOpening( TQString("spacer"), attr );
    emitProperty( TQString("name"),
		  TQString("Spacer%1").arg(uniqueSpacer++).latin1() );
    emitProperty( TQString("orientation"), orientation, TQString("enum") );
    emitProperty( TQString("sizeType"), TQString("Expanding"),
		  TQString("enum") );
    emitClosing( TQString("spacer") );
}

void Glade2Ui::emitPixmap( const TQString& imageName, int leftAttach,
			   int rightAttach, int topAttach, int bottomAttach )
{
    emitOpeningWidget( TQString("TQLabel"), leftAttach, rightAttach, topAttach,
		       bottomAttach );
    emitProperty( TQString("sizePolicy"),
		  TQSizePolicy(TQSizePolicy::Fixed, TQSizePolicy::Fixed) );
    emitProperty( TQString("pixmap"), imageName, TQString("pixmap") );
    emitClosing( TQString("widget") );
}

void Glade2Ui::emitGnomeAbout( TQString copyright, TQString authors,
			       TQString comments )
{
    TQString prog = yyProgramName;
    if ( prog.isEmpty() )
	prog = TQString( "Gnomovision 1.69" );
    if ( copyright.isEmpty() )
	copyright = TQString( "(C) 2001 Jasmin Blanchette" );
    if ( authors.isEmpty() )
	authors = TQString( "Jasmin Blanchette <jasmin@troll.no>" );
    if ( comments.isEmpty() )
	comments = TQString( "Gnomovision is the official GNU application." );

    emitOpening( TQString("hbox") );
    emitSpacer( TQString("Horizontal") );
    emitOpeningWidget( TQString("TQLayoutWidget") );
    emitOpening( TQString("vbox") );
    emitProperty( TQString("spacing"), 17 );

    /*
      Emit the application name.
    */
    emitOpeningWidget( TQString("TQLabel") );
    emitFontProperty( TQString("font"), 24, true );
    emitProperty( TQString("text"), prog );
    emitProperty( TQString("alignment"), TQString("AlignAuto|AlignCenter"),
		  TQString("set") );
    emitClosing( TQString("widget") );

    /*
      Emit the copyright notice.
    */
    emitOpeningWidget( TQString("TQLabel") );
    emitFontProperty( TQString("font"), 12, true );
    emitProperty( TQString("text"), copyright );
    emitClosing( TQString("widget") );

    /*
      Emit the authors' names.
    */
    emitOpeningWidget( TQString("TQLayoutWidget") );
    emitOpening( TQString("hbox") );

    emitOpeningWidget( TQString("TQLabel") );
    emitFontProperty( TQString("font"), 12, true );
    emitProperty( TQString("text"), TQString("Authors:") );
    emitProperty( TQString("alignment"), TQString("AlignAuto|AlignTop"),
		  TQString("set") );
    emitClosing( TQString("widget") );

    emitOpeningWidget( TQString("TQLabel") );
    emitFontProperty( TQString("font"), 12, false );
    emitProperty( TQString("text"), authors );
    emitProperty( TQString("alignment"), TQString("AlignAuto|AlignTop"),
		  TQString("set") );
    emitClosing( TQString("widget") );

    emitSpacer( TQString("Horizontal") );

    emitClosing( TQString("hbox") );
    emitClosing( TQString("widget") );

    /*
      Emit the comments.
    */
    emitOpeningWidget( TQString("TQLabel") );
    emitFontProperty( TQString("font"), 10, false );
    emitProperty( TQString("text"), comments );
    emitProperty( TQString("alignment"), TQString("AlignAuto|AlignTop"),
		  TQString("set") );
    emitClosing( TQString("widget") );

    /*
      Emit the spacer and the OK button.
    */
    emitSpacer( TQString("Vertical") );

    emitOpeningWidget( TQString("TQLayoutWidget") );
    emitOpening( TQString("hbox") );
    emitSpacer( TQString("Horizontal") );

    emitPushButton( TQString("&OK"), TQString("okButton") );

    emitSpacer( TQString("Horizontal") );
    emitClosing( TQString("hbox") );
    emitClosing( TQString("widget") );

    emitClosing( TQString("vbox") );
    emitClosing( TQString("widget") );
    emitSpacer( TQString("Horizontal") );
    emitClosing( TQString("hbox") );
}

/*
  GnomeApps are emitted in two passes, because some information goes
  into the <widget> section, whereas other information goes into the
  <toolbars> section.
*/
void Glade2Ui::emitGnomeAppChildWidgetsPass1(
	const TQValueList<TQDomElement>& childWidgets )
{
    TQValueList<TQDomElement>::ConstIterator c = childWidgets.begin();
    while ( c != childWidgets.end() ) {
	TQValueList<TQDomElement> grandchildWidgets;
	TQString childName;

	TQDomNode n = (*c).firstChild();
	while ( !n.isNull() ) {
	    TQString tagName = n.toElement().tagName();
	    if ( tagName == TQString("child_name") ) {
		childName = getTextValue( n );
	    } else if ( tagName == TQString("widget") ) {
		grandchildWidgets.push_back( n.toElement() );
	    }
	    n = n.nextSibling();
	}

	if ( childName == TQString("GnomeDock:contents") ) {
	    emitWidget( *c, false );
	} else {
	    emitGnomeAppChildWidgetsPass1( grandchildWidgets );
	}
	++c;
    }
}

void Glade2Ui::doPass2( const TQValueList<TQDomElement>& childWidgets,
			TQValueList<TQDomElement> *menuBar,
			TQValueList<TQValueList<TQDomElement> > *toolBars )
{
    TQValueList<TQDomElement>::ConstIterator c = childWidgets.begin();
    while ( c != childWidgets.end() ) {
	TQValueList<TQDomElement> grandchildWidgets;
	TQString childName;
	TQString gtkClass;

	TQDomNode n = (*c).firstChild();
	while ( !n.isNull() ) {
	    TQString tagName = n.toElement().tagName();
	    if ( tagName == TQString("child_name") ) {
		childName = getTextValue( n );
	    } else if ( tagName == TQString("class") ) {
		gtkClass = getTextValue( n );
	    } else if ( tagName == TQString("widget") ) {
		grandchildWidgets.push_back( n.toElement() );
	    }
	    n = n.nextSibling();
	}

	if ( gtkClass == TQString("GtkMenuBar") ) {
	    *menuBar = grandchildWidgets;
	} else if ( gtkClass == TQString("GtkToolbar") ) {
	    toolBars->push_back( grandchildWidgets );
	} else if ( childName != TQString("GnomeDock:contents") ) {
	    doPass2( grandchildWidgets, menuBar, toolBars );
	}
	++c;
    }
}

void Glade2Ui::emitGnomeAppChildWidgetsPass2(
	const TQValueList<TQDomElement>& childWidgets )
{
    TQValueList<TQDomElement> menuBar;
    TQValueList<TQValueList<TQDomElement> > toolBars;

    doPass2( childWidgets, &menuBar, &toolBars );

    emitGtkMenuBarChildWidgets( menuBar );
    if ( !toolBars.isEmpty() ) {
	emitOpening( TQString("toolbars") );
	while ( !toolBars.isEmpty() ) {
	    emitGtkToolbarChildWidgets( toolBars.first() );
	    toolBars.remove( toolBars.begin() );
	}
	emitClosing( TQString("toolbars") );
    }
}

void Glade2Ui::emitGtkButtonChildWidgets( TQValueList<TQDomElement> childWidgets )
{
    TQValueList<TQDomElement>::ConstIterator c = childWidgets.begin();
    while ( c != childWidgets.end() ) {
	TQString label;

	TQDomNode n = (*c).firstChild();
	while ( !n.isNull() ) {
	    TQString tagName = n.toElement().tagName();
	    if ( tagName == TQString("label") ) {
		label = getTextValue( n );
	    } else if ( tagName == TQString("widget") ) {
		childWidgets.push_back( n.toElement() );
	    }
	    n = n.nextSibling();
	}

	if ( !label.isEmpty() ) {
	    emitProperty( TQString("text"), accelerate(label) );
	    break;
	}
	++c;
    }
}

void Glade2Ui::emitGtkComboChildWidgets(
	const TQValueList<TQDomElement>& childWidgets, const TQStringList& items )
{
    TQString text;

    // there should be exactly one child, of type GtkEntry
    if ( childWidgets.count() == 1 ) {
	TQDomNode n = childWidgets.first().firstChild();
	while ( !n.isNull() ) {
	    TQString tagName = n.toElement().tagName();
	    if ( tagName == TQString("name") ) {
		// grep 'elsewhere'
		emitProperty( TQString("name"),
			      fixedName(getTextValue(n).latin1()) );
	    } else if ( tagName == TQString("text") ) {
		text = getTextValue( n );
	    }
	    n = n.nextSibling();
	}
    }

    int n = 0;
    TQStringList::ConstIterator s = items.begin();
    while ( s != items.end() ) {
	if ( !text.isEmpty() && *s == text )
	    emitProperty( TQString("currentItem"), n );
	n++;
	++s;
    }

}

void Glade2Ui::emitGtkNotebookChildWidgets(
	const TQValueList<TQDomElement>& childWidgets )
{
    TQStringList tabNames;
    TQStringList tabLabels;

    for ( int i = 0; i < (int) childWidgets.count(); i++ ) {
	tabNames.push_back( TQString("tab%1").arg(i + 1) );
	tabLabels.push_back( TQString("Tab %1").arg(i + 1) );
    }

    TQValueList<TQDomElement>::ConstIterator c;
    c = childWidgets.begin();

    TQStringList::Iterator nam = tabNames.begin();
    TQStringList::Iterator lab = tabLabels.begin();

    while ( c != childWidgets.end() ) {
	TQString childName;
	TQString name;
	TQString label;

	TQDomNode n = (*c).firstChild();
	while ( !n.isNull() ) {
	    TQString tagName = n.toElement().tagName();
	    if ( tagName == TQString("child_name") ) {
		childName = getTextValue( n );
	    } else if ( tagName == TQString("name") ) {
		name = getTextValue( n );
	    } else if ( tagName == TQString("label") ) {
		label = getTextValue( n );
	    }
	    n = n.nextSibling();
	}

	if ( childName == TQString("Notebook:tab") ) {
	    if ( !name.isEmpty() )
		*nam = name;
	    if ( !label.isEmpty() )
		*lab = label;
	    ++nam;
	    ++lab;
	}
	++c;
    }

    c = childWidgets.begin();
    while ( c != childWidgets.end() ) {
	TQString childName;
	TQString name;
	TQString label;

	TQDomNode n = (*c).firstChild();
	while ( !n.isNull() ) {
	    TQString tagName = n.toElement().tagName();
	    if ( tagName == TQString("child_name") )
		childName = getTextValue( n );
	    n = n.nextSibling();
	}

	if ( childName != TQString("Notebook:tab") ) {
	    emitOpeningWidget( TQString("TQWidget") );
	    emitProperty( TQString("name"), tabNames.first().latin1() );
	    tabNames.remove( tabNames.begin() );
	    emitAttribute( TQString("title"), accelerate(tabLabels.first()) );
	    tabLabels.remove( tabLabels.begin() );
	    emitWidget( *c, false );
	    emitClosing( TQString("widget") );
	}
	++c;
    }
}

void Glade2Ui::emitTQListViewColumns( const TQDomElement& qlistview )
{
    TQDomNode n = qlistview.firstChild();
    while ( !n.isNull() ) {
	TQString tagName = n.toElement().tagName();
	if ( tagName == TQString("widget") ) {
	    TQDomNode child = n.firstChild();
	    while ( !child.isNull() ) {
		TQString tagName = child.toElement().tagName();
		if ( tagName == TQString("label") ) {
		    emitOpening( TQString("column") );
		    emitProperty( TQString("text"),
				  decelerate(getTextValue(child)) );
		    emitClosing( TQString("column") );
		}
		child = child.nextSibling();
	    }
	} else if ( tagName == TQString("class") ) {
	    TQString gtkClass = getTextValue( n );
	    if ( gtkClass.endsWith(TQString("Tree")) )
		emitProperty( TQString("rootIsDecorated"), TQVariant(true) );
	} else if ( tagName == TQString("selection_mode") ) {
	    emitProperty( TQString("selectionMode"),
			  gtk2qtSelectionMode(getTextValue(n)) );
	}
	n = n.nextSibling();
    }
}

void Glade2Ui::emitGtkScrolledWindowChildWidgets(
	const TQValueList<TQDomElement>& childWidgets, const TQString& qtClass )
{
    if ( childWidgets.count() == 1 ) {
	if ( qtClass == TQString("TQIconView") ||
	     qtClass == TQString("TQListBox") ||
	     qtClass == TQString("TQListView") ) {
	    TQDomNode n = childWidgets.first().firstChild();
	    while ( !n.isNull() ) {
		TQString tagName = n.toElement().tagName();
		if ( tagName == TQString("selection_mode") ) {
		    emitProperty( TQString("selectionMode"),
				  gtk2qtSelectionMode(getTextValue(n)) );
		}
		n = n.nextSibling();
	    }
	}

	if ( qtClass == TQString("TQListView") ) {
	    emitTQListViewColumns( childWidgets.first() );
	} else if ( qtClass == TQString("TQTextEdit") ||
		    qtClass == TQString("TQTextView") ) {
	    TQDomNode n = childWidgets.first().firstChild();
	    while ( !n.isNull() ) {
		TQString tagName = n.toElement().tagName();
		if ( tagName == TQString("text") )
		    emitProperty( TQString("text"), getTextValue(n) );
		n = n.nextSibling();
	    }
	}
    }
}

void Glade2Ui::emitGnomeDruidPage( const TQDomElement& druidPage )
{
    TQValueList<TQDomElement> childWidgets;
    TQString gtkClass;
    TQString logoImage;
    TQString name;
    TQString text;
    TQString title;
    TQString watermarkImage;

    emitOpeningWidget( TQString("TQWidget") );

    TQDomNode n = druidPage.firstChild();
    while ( !n.isNull() ) {
	TQString tagName = n.toElement().tagName();
	if ( tagName == TQString("class") ) {
	    gtkClass = getTextValue( n );
	} else if ( tagName == TQString("logo_image") ) {
	    logoImage = getTextValue( n );
	} else if ( tagName == TQString("name") ) {
	    name = getTextValue( n );
	} else if ( tagName == TQString("text") ) {
	    text = getTextValue( n );
	} else if ( tagName == TQString("title") ) {
	    title = getTextValue( n );
	} else if ( tagName == TQString("watermark_image") ) {
	    watermarkImage = getTextValue( n );
	} else if ( tagName == TQString("widget") ) {
	    childWidgets.push_back( n.toElement() );
	}
	n = n.nextSibling();
    }

    if ( !name.isEmpty() )
	emitProperty( TQString("name"), fixedName(name).latin1() );
    if ( title.isEmpty() )
	title = TQString( "Page" );
    emitAttribute( TQString("title"), title );

    /*
      We're striving to get the logoImage and/or the watermarkImage at
      the right places with a grid layout.
    */
    int leftAttach = 0;
    int rightAttach = 0;
    int topAttach = 0;
    int bottomAttach = 0;

    int numImages = 0;
    if ( !logoImage.isEmpty() ) {
	topAttach = 1;
	numImages++;
    }
    if ( !watermarkImage.isEmpty() ) {
	leftAttach = 1;
	numImages++;
    }
    rightAttach = leftAttach + numImages;
    bottomAttach = topAttach + numImages;

    bool layouted = ( numImages > 0 );
    if ( layouted ) {
	emitOpening( TQString("grid") );
	if ( !logoImage.isEmpty() )
	    emitPixmap( imageName(logoImage), numImages, numImages + 1, 0, 1 );
	if ( !watermarkImage.isEmpty() )
	    emitPixmap( imageName(watermarkImage), 0, 1, numImages,
			numImages + 1 );
    } else {
	leftAttach = -1;
	rightAttach = -1;
	topAttach = -1;
	bottomAttach = -1;
    }

    if ( gtkClass.endsWith(TQString("Standard")) ) {
	emitChildWidgets( childWidgets, layouted, leftAttach, rightAttach,
			  topAttach, bottomAttach );
    } else if ( !text.isEmpty() ) {
	if ( layouted )
	    emitOpeningWidget( TQString("TQLayoutWidget"), leftAttach,
			       rightAttach, topAttach, bottomAttach );
	emitOpening( TQString("hbox") );
	emitSpacer( TQString("Horizontal") );
	emitOpeningWidget( TQString("TQLabel") );
	emitProperty( TQString("text"), text );
	emitClosing( TQString("widget") );
	emitSpacer( TQString("Horizontal") );
	emitClosing( TQString("hbox") );
	if ( layouted )
	    emitClosing( TQString("widget") );
    }

    if ( layouted )
	emitClosing( TQString("grid") );
    emitClosing( TQString("widget") );
}

void Glade2Ui::emitGtkWindowChildWidgets(
	const TQValueList<TQDomElement>& childWidgets, const TQString& qtClass )
{
    if ( childWidgets.count() == 1 && qtClass == TQString("TQWizard") ) {
	emitFontProperty( TQString("titleFont"), 18, false );

	TQDomNode n = childWidgets.first().firstChild();
	while ( !n.isNull() ) {
	    if ( n.toElement().tagName() == TQString("widget") )
		emitGnomeDruidPage( n.toElement() );
	    n = n.nextSibling();
	}
    } else {
	emitChildWidgets( childWidgets, false );
    }
}

bool Glade2Ui::packEnd( const TQDomElement& widget )
{
    TQDomNode n = widget.firstChild();
    while ( !n.isNull() ) {
	if ( n.toElement().tagName() == TQString("child") ) {
	    TQDomNode child = n.firstChild();
	    while ( !child.isNull() ) {
		if ( child.toElement().tagName() == TQString("pack") ) {
		    TQString pack = getTextValue( child );
		    return pack.endsWith( TQString("_END") );
		}
		child = child.nextSibling();
	    }
	}
	n = n.nextSibling();
    }
    return false;
}

void Glade2Ui::emitChildWidgets( const TQValueList<TQDomElement>& childWidgets,
				 bool layouted, int leftAttach, int rightAttach,
				 int topAttach, int bottomAttach )
{
    TQValueList<TQDomElement> start;
    TQValueList<TQDomElement> end;
    TQValueList<TQDomElement>::ConstIterator e;

    if ( layouted ) {
	e = childWidgets.begin();
	while ( e != childWidgets.end() ) {
	    if ( packEnd(*e) )
		end.push_front( *e );
	    else
		start.push_back( *e );
	    ++e;
	}
    } else {
	start = childWidgets;
    }

    e = start.begin();
    while ( e != start.end() ) {
	emitWidget( *e, layouted, leftAttach, rightAttach, topAttach,
		    bottomAttach );
	++e;
    }
    e = end.begin();
    while ( e != end.end() ) {
	emitWidget( *e, layouted, leftAttach, rightAttach, topAttach,
		    bottomAttach );
	++e;
    }
}

void Glade2Ui::emitOpeningWidget( const TQString& qtClass, int leftAttach,
				  int rightAttach, int topAttach,
				  int bottomAttach )
{
    AttributeMap attr = attribute( TQString("class"), qtClass );
    attach( &attr, leftAttach, rightAttach, topAttach, bottomAttach );
    emitOpening( TQString("widget"), attr );
}

/*
  Returns true if the vbox containing childWidgets should have a
  spacer at the end to prevent it from looking bad, otherwise returns
  false.

  The algorithm is very experimental.
*/
bool Glade2Ui::shouldPullup( const TQValueList<TQDomElement>& childWidgets )
{
    TQRegExp gtkSmallWidget( TQString(
	    "G.*(?:Button|Combo|Dial|Entry|Label|OptionMenu|Picker|ProgressBar"
	    "|Separator|Statusbar|Toolbar|VBox)") );

    TQValueList<TQDomElement>::ConstIterator c = childWidgets.begin();
    while ( c != childWidgets.end() ) {
	TQValueList<TQDomElement> grandchildWidgets;
	TQString gtkClass;

	TQDomNode n = (*c).firstChild();
	while ( !n.isNull() ) {
	    TQString tagName = n.toElement().tagName();
	    if ( tagName == TQString("class") ) {
		gtkClass = getTextValue( n );
	    } else if ( tagName == TQString("widget") ) {
		grandchildWidgets.push_back( n.toElement() );
	    }
	    n = n.nextSibling();
	}

	if ( !gtkSmallWidget.exactMatch(gtkClass) ||
	     !shouldPullup(grandchildWidgets) )
	    return false;
	++c;
    }
    return true;
}

TQString Glade2Ui::emitWidget( const TQDomElement& widget, bool layouted,
			      int leftAttach, int rightAttach, int topAttach,
			      int bottomAttach )
{
    TQRegExp gtkLayoutWidget( TQString(
	    "Gtk(?:Packer|Table|Toolbar|[HV](?:(?:Button)?Box))") );
    TQRegExp gtkOrientedWidget( TQString(
	    "Gtk([HV])(?:Paned|Scale|Scrollbar|Separator)") );

    TQValueList<TQDomElement> childWidgets;
    TQString gtkClass;
    TQString name;
    TQString title;
    int x = 0;
    int y = 0;
    int width = 0;
    int height = 0;
    int numRows = 0;
    int numColumns = 0;

    bool active = false;
    TQString authors;
    TQString childName;
    TQString comments;
    TQString copyright;
    TQString creationFunction;
    bool editable = true;
    TQString filename;
    TQString focusTarget;
    TQString hscrollbarPolicy;
    TQString icon;
    int initialChoice = 0;
    TQStringList items;
    TQString justify;
    TQString label;
    TQString logoImage;
    int lower = -123456789;
    int page = 10;
    int pageSize = 10;
    TQString selectionMode;
    TQString shadowType( "GTK_SHADOW_NONE" );
    bool showText = true;
    bool showTitles = true;
    int step = 1;
    TQString tabPos;
    TQString text;
    int textMaxLength = 0;
    bool textVisible = true;
    TQString tooltip;
    TQString type;
    int upper = 123456789;
    int value = 123456789;
    bool valueInList = true;
    TQString vscrollbarPolicy;
    TQString watermarkImage;
    bool wrap = false;

    bool topLevel = yyFormName.isEmpty();
    if ( topLevel )
	name = TQString( "Form%1" ).arg( uniqueForm++ );

    TQDomNode n = widget.firstChild();
    while ( !n.isNull() ) {
	TQString tagName = n.toElement().tagName();
	if ( !tagName.isEmpty() ) {
	    /*
	      Recognize the properties and stores them in variables.
	      This step is a bit silly, and if this function were to
	      be rewritten, almost everything would just be stored, as
	      strings, in a giant map, and looked up for when emitting
	      the properties.
	    */
	    switch ( (uchar) tagName[0].cell() ) {
	    case 'a':
		if ( tagName == TQString("active") ) {
		    active = isTrue( getTextValue(n) );
		} else if ( tagName == TQString("authors") ) {
		    authors = getTextValue( n );
		}
		break;
	    case 'c':
		if ( tagName == TQString("child") ) {
		    TQDomNode child = n.firstChild();
		    while ( !child.isNull() ) {
			TQString childTagName = child.toElement().tagName();
			if ( childTagName == TQString("left_attach") ) {
			    leftAttach = getTextValue( child ).toInt();
			} else if ( childTagName == TQString("right_attach") ) {
			    rightAttach = getTextValue( child ).toInt();
			} else if ( childTagName == TQString("top_attach") ) {
			    topAttach = getTextValue( child ).toInt();
			} else if ( childTagName == TQString("bottom_attach") ) {
			    bottomAttach = getTextValue( child ).toInt();
			}
			child = child.nextSibling();
		    }
		} else if ( tagName == TQString("child_name") ) {
		    childName = getTextValue( n );
		} else if ( tagName == TQString("class") ) {
		    gtkClass = getTextValue( n );
		} else if ( tagName == TQString("climb_rate") ) {
		    step = getTextValue( n ).toInt();
		} else if ( tagName == TQString("columns") ) {
		    numColumns = getTextValue( n ).toInt();
		} else if ( tagName == TQString("comments") ) {
		    comments = getTextValue( n );
		} else if ( tagName == TQString("copyright") ) {
		    copyright = getTextValue( n );
		} else if ( tagName == TQString("creation_function") ) {
		    creationFunction = getTextValue( n );
		}
		break;
	    case 'd':
		if ( tagName == TQString("default_focus_target") ) {
		    if ( focusTarget.isEmpty() )
			focusTarget = getTextValue( n );
		}
		break;
	    case 'e':
		if ( tagName == TQString("editable") )
		    editable = isTrue( getTextValue(n) );
		break;
	    case 'f':
		if ( tagName == TQString("filename") ) {
		    filename = getTextValue( n );
		} else if ( tagName == TQString("focus_target") ) {
		    focusTarget = getTextValue( n );
		}
		break;
	    case 'h':
		if ( tagName == TQString("height") ) {
		    height = getTextValue( n ).toInt();
		} else if ( tagName == TQString("hscrollbar_policy") ) {
		    hscrollbarPolicy = getTextValue( n );
		}
		break;
	    case 'i':
		if ( tagName == TQString("icon") ) {
		    icon = getTextValue( n );
		} else if ( tagName == TQString("initial_choice") ) {
		    initialChoice = getTextValue( n ).toInt();
		} else if ( tagName == TQString("items") ) {
		    items = TQStringList::split( TQChar('\n'), getTextValue(n) );
		}
		break;
	    case 'j':
		if ( tagName == TQString("justify") )
		    justify = getTextValue( n );
		break;
	    case 'l':
		if ( tagName == TQString("label") ) {
		    label = getTextValue( n );
		} else if ( tagName == TQString("logo_image") ) {
		    logoImage = getTextValue( n );
		} else if ( tagName == TQString("lower") ) {
		    lower = getTextValue( n ).toInt();
		}
		break;
	    case 'n':
		if ( tagName == TQString("name") )
		    name = getTextValue( n );
		break;
	    case 'p':
		if ( tagName == TQString("page") ) {
		    page = getTextValue( n ).toInt();
		} else if ( tagName == TQString("page_size") ) {
		    pageSize = getTextValue( n ).toInt();
		}
		break;
	    case 'r':
		if ( tagName == TQString("rows") )
		    numRows = getTextValue( n ).toInt();
		break;
	    case 's':
		if ( tagName == TQString("selection_mode") ) {
		    selectionMode = getTextValue( n );
		} else if ( tagName == TQString("shadow_type") ) {
		    shadowType = getTextValue( n );
		} else if ( tagName == TQString("show_text") ) {
		    showText = isTrue( getTextValue(n) );
		} else if ( tagName == TQString(TQString("show_titles")) ) {
		    showTitles = isTrue( getTextValue(n) );
		} else if ( tagName == TQString("step") ) {
		    step = getTextValue( n ).toInt();
		} else if ( tagName == TQString("stock_button") ) {
		    /*
		      Let's cheat: We convert the symbolic name into a
		      button label.
		    */
		    label = getTextValue( n );
		    int k = label.findRev( TQChar('_') );
		    if ( k != -1 )
			label = label.mid( k + 1 );
		    if ( !label.isEmpty() && label != TQString("OK") )
			label = label.left( 1 ) + label.mid( 1 ).lower();
		}
		break;
	    case 't':
		if ( tagName == TQString("tab_pos") ) {
		    tabPos = getTextValue( n );
		} else if ( tagName == TQString("text") ) {
		    text = getTextValue( n );
		} else if ( tagName == TQString("textMaxLength") ) {
		    textMaxLength = getTextValue( n ).toInt();
		} else if ( tagName == TQString("textVisible") ) {
		    textVisible = isTrue( getTextValue(n) );
		} else if ( tagName == TQString("title") ) {
		    title = getTextValue( n );
		} else if ( tagName == TQString("tooltip") ) {
		    tooltip = getTextValue( n );
		} else if ( tagName == TQString("type") ) {
		    type = getTextValue( n );
		}
		break;
	    case 'u':
		if ( tagName == TQString("upper") )
		    upper = getTextValue( n ).toInt();
		break;
	    case 'v':
		if ( tagName == TQString("value") ) {
		    value = getTextValue( n ).toInt();
		} else if ( tagName == TQString("value_in_list") ) {
		    valueInList = isTrue( getTextValue(n) );
		} else if ( tagName == TQString("vscrollbar_policy") ) {
		    vscrollbarPolicy = getTextValue( n );
		}
		break;
	    case 'w':
		if ( tagName == TQString("watermark_image") ) {
		    watermarkImage = getTextValue( n );
		} else if ( tagName == TQString("widget") )
		    childWidgets.push_back( n.toElement() );
		else if ( tagName == TQString("width") )
		    width = getTextValue( n ).toInt();
		else if ( tagName == TQString("wrap") )
		    wrap = isTrue( getTextValue(n) );
		break;
	    case 'x':
		if ( tagName == TQString("x") )
		    x = getTextValue( n ).toInt();
		break;
	    case 'y':
		if ( tagName == TQString("y") )
		    y = getTextValue( n ).toInt();
	    }
	}
	n = n.nextSibling();
    }

    if ( topLevel ) {
	yyFormName = name;
	emitSimpleValue( TQString("class"), yyFormName );
    }

    if ( gtkLayoutWidget.exactMatch(gtkClass) ) {
	TQString boxKind;
	TQString orientation;

	if ( gtkClass.startsWith(TQString("GtkH")) ||
	     gtkClass == TQString(TQString("GtkToolbar")) ) {
	    /*
	      GtkToolbar, right. Toolbars may appear anywhere in a
	      widget, but then they really are just a row of buttons.
	    */
	    boxKind = TQString( "hbox" );
	    orientation = TQString( "Horizontal" );
	} else if ( gtkClass.startsWith(TQString("GtkV")) ||
		    gtkClass == TQString("GtkPacker") ) {
	    /*
	      We don't support the GtkPacker, whose trails lead to
	      destruction.
	    */
	    boxKind = TQString( "vbox" );
	    orientation = TQString( "Vertical" );
	} else {
	    boxKind = TQString( "grid" );
	}

	if ( layouted )
	    emitOpeningWidget( TQString("TQLayoutWidget"), leftAttach,
			       rightAttach, topAttach, bottomAttach );
	emitOpening( boxKind );
	emitProperty( TQString("name"), fixedName(name).latin1() );
	if ( gtkClass == TQString("GtkHButtonBox") ||
	     childName == TQString("Dialog:action_area") )
	    emitSpacer( orientation );
	emitChildWidgets( childWidgets, true );

	if ( gtkClass == TQString("GtkVButtonBox") ||
	     childName == TQString("Dialog:action_area") ||
	     (boxKind == TQString("vbox") && shouldPullup(childWidgets)) ) {
	    emitSpacer( orientation );
	} else if ( boxKind == TQString("grid") && shouldPullup(childWidgets) ) {
	    emitSpacer( TQString("Vertical"), 0, numColumns, numRows,
			numRows + 1 );
	}

	emitClosing( boxKind );
	if ( layouted )
	    emitClosing( TQString("widget") );
    } else if ( gtkClass == TQString("GtkFixed") && !layouted ) {
	emitChildWidgets( childWidgets, false );
    /*
      Placeholders in a grid are typically needless.
    */
    } else if ( !(leftAttach >= 0 && gtkClass == TQString("Placeholder")) ) {
	bool needFakeLayout = ( !layouted && !topLevel && x == 0 && y == 0 &&
				width == 0 && height == 0 );
	TQString qtClass = gtk2qtClass( gtkClass, childWidgets );
	bool unknown = false;

	if ( qtClass == TQString("TQFrame") && !label.isEmpty() ) {
	    qtClass = TQString( "TQButtonGroup" );
	} else if ( qtClass == TQString("TQListView") && !showTitles &&
		    gtkClass.endsWith(TQString("List")) ) {
	    qtClass = TQString( "TQListBox" );
	} else if ( qtClass == TQString("Custom") ) {
	    qtClass = creationFunction;
	    yyCustomWidgets.insert( qtClass, TQString::null );
	} else if ( qtClass == TQString("Unknown") ) {
	    qtClass = TQString( "TQLabel" );
	    unknown = true;
	}

	if ( qtClass.isEmpty() ) {
	    emitChildWidgets( childWidgets, layouted, leftAttach, rightAttach,
			      topAttach, bottomAttach );
	} else {
	    if ( needFakeLayout ) {
		emitOpening( TQString("vbox") );
		emitProperty( TQString("margin"), 11 );
	    }
	    emitOpeningWidget( qtClass, leftAttach, rightAttach, topAttach,
			       bottomAttach );

	    // grep 'elsewhere'
	    if ( gtkClass != TQString("GtkCombo") )
		emitProperty( TQString("name"), fixedName(name).latin1() );
	    if ( !title.isEmpty() )
		emitProperty( TQString("caption"), title );
	    if ( !layouted && (x != 0 || y != 0 || width != 0 || height != 0) )
		emitProperty( TQString("geometry"), TQRect(x, y, width, height) );
	    if ( gtkClass == TQString("GtkToggleButton") ) {
		emitProperty( TQString("toggleButton"), TQVariant(true) );
		if ( active )
		    emitProperty( TQString("on"), TQVariant(true) );
	    } else {
		if ( active )
		    emitProperty( TQString("checked"), TQVariant(true) );
	    }

	    if ( !editable )
		emitProperty( TQString("readOnly"), TQVariant(true) );
	    if ( !focusTarget.isEmpty() )
		emitProperty( TQString("buddy"),
			      fixedName(focusTarget).latin1() );
	    if ( !hscrollbarPolicy.isEmpty() )
		emitProperty( TQString("hScrollBarMode"),
			      gtk2qtScrollBarMode(hscrollbarPolicy),
			      TQString("enum") );
	    if ( !vscrollbarPolicy.isEmpty() )
		emitProperty( TQString("vScrollBarMode"),
			      gtk2qtScrollBarMode(vscrollbarPolicy),
			      TQString("enum") );
	    if ( !justify.isEmpty() ||
		 (wrap && gtkClass != TQString("GtkSpinButton")) ) {
		TQStringList flags;
		if ( wrap )
		    flags.push_back( TQString("WordBreak") );

		if ( justify.endsWith(TQString("_CENTER")) ) {
		    flags.push_back( TQString("AlignCenter") );
		} else {
		    if ( justify.endsWith(TQString("_RIGHT")) )
			flags.push_back( TQString("AlignRight") );
		    flags.push_back( TQString("AlignVCenter") );
		}

		if ( !flags.isEmpty() )
		    emitProperty( TQString("alignment"), flags.join(TQChar('|')),
				  TQString("set") );
	    }
	    if ( !label.isEmpty() ) {
		if ( gtkClass.endsWith(TQString("Frame")) ) {
		    emitProperty( TQString("title"), label );
		} else {
		    emitProperty( TQString("text"), accelerate(label) );
		}
	    }
	    if ( lower != -123456789 )
		emitProperty( TQString("minValue"), lower );
	    if ( upper != 123456789 )
		emitProperty( TQString("maxValue"), upper );
	    if ( value != 123456789 ) {
		if ( gtkClass == TQString("GtkProgressBar") ) {
		    emitProperty( TQString("progress"), value );
		} else {
		    emitProperty( TQString("value"), value );
		}
	    }
	    if ( TQMAX(page, pageSize) != 10 &&
		 gtkClass.endsWith(TQString("Scrollbar")) )
		emitProperty( TQString("pageStep"), TQMAX(page, pageSize) );
	    if ( !selectionMode.isEmpty() )
		emitProperty( TQString("selectionMode"),
			      gtk2qtSelectionMode(selectionMode),
			      TQString("enum") );
	    if ( !shadowType.endsWith(TQString("_NONE")) ) {
		TQString shape = shadowType.contains( TQString("_ETCHED_") ) > 0 ?
				TQString( "Box" ) : TQString( "WinPanel" );
		TQString shadow = shadowType.endsWith( TQString("_IN") ) ?
				 TQString( "Sunken" ) : TQString( "Raised" );
		emitProperty( TQString("frameShape"), shape, TQString("enum") );
		emitProperty( TQString("frameShadow"), shadow, TQString("enum") );
	    }
	    if ( !showText )
		emitProperty( TQString("percentageVisible"),
			      TQVariant(false) );
	    if ( step != 1 )
		emitProperty( TQString("lineStep"), step );
	    if ( tabPos.endsWith(TQString("_BOTTOM")) ||
		 tabPos.endsWith(TQString("_RIGHT")) )
		emitProperty( TQString("tabPosition"), TQString("Bottom") );
	    if ( !text.isEmpty() )
		emitProperty( TQString("text"), text );
	    if ( textMaxLength != 0 )
		emitProperty( TQString("maxLength"), textMaxLength );
	    if ( !textVisible )
		emitProperty( TQString("echoMode"), TQString("Password"),
			      TQString("enum") );
	    if ( !tooltip.isEmpty() )
		emitProperty( TQString("toolTip"), tooltip );
	    if ( !valueInList )
		emitProperty( TQString("editable"), TQVariant(true) );
	    if ( wrap && gtkClass == TQString("GtkSpinButton") )
		emitProperty( TQString("wrapping"), TQVariant(true) );

	    if ( gtkClass.endsWith(TQString("Tree")) ) {
		emitProperty( TQString("rootIsDecorated"), TQVariant(true) );
	    } else if ( gtkOrientedWidget.exactMatch(gtkClass) ) {
		TQString s = ( gtkOrientedWidget.cap(1) == TQChar('H') ) ?
			    TQString( "Horizontal" ) : TQString( "Vertical" );
		emitProperty( TQString("orientation"), s, TQString("enum") );
	    }

	    if ( qtClass == TQString("TQListView") )
		emitProperty( TQString("resizeMode"), TQString("AllColumns"),
			      TQString("enum") );

	    while ( !items.isEmpty() ) {
		emitOpening( TQString("item") );
		emitProperty( TQString("text"), items.first() );
		emitClosing( TQString("item") );
		items.remove( items.begin() );
	    }
	    if ( initialChoice != 0 )
		emitProperty( TQString("currentItem"), initialChoice );

	    if ( !icon.isEmpty() )
		emitProperty( TQString("pixmap"), imageName(icon),
			      TQString("pixmap") );

	    if ( gtkClass == TQString("GnomeAbout") ) {
		emitGnomeAbout( copyright, authors, comments );
	    } else if ( gtkClass == TQString("GnomeApp") ) {
		emitGnomeAppChildWidgetsPass1( childWidgets );
	    } else if ( gtkClass == TQString("GnomePropertyBox") ) {
		emitOpening( TQString("vbox") );
		emitChildWidgets( childWidgets, true );
		emitOpeningWidget( TQString("TQLayoutWidget") );
		emitOpening( TQString("hbox") );
		emitPushButton( TQString("&Help"), TQString("helpButton") );
		emitSpacer( TQString("Horizontal") );
		emitPushButton( TQString("&OK"), TQString("okButton") );
		emitPushButton( TQString("&Apply"), TQString("applyButton") );
		emitPushButton( TQString("&Close"), TQString("closeButton") );
		emitClosing( TQString("hbox") );
		emitClosing( TQString("widget") );
		emitClosing( TQString("vbox") );
	    } else if ( gtkClass.endsWith(TQString("Button")) ) {
		if ( label.isEmpty() )
		    emitGtkButtonChildWidgets( childWidgets );
	    } else if ( gtkClass == TQString("GtkCombo") ) {
		emitGtkComboChildWidgets( childWidgets, items );
	    } else if ( gtkClass == TQString("GtkNotebook") ) {
		emitGtkNotebookChildWidgets( childWidgets );
	    } else if ( gtkClass == TQString("GtkWindow") ) {
		emitGtkWindowChildWidgets( childWidgets, qtClass );
	    } else if ( gtkClass == TQString("GtkScrolledWindow") ) {
		emitGtkScrolledWindowChildWidgets( childWidgets, qtClass );
	    } else if ( qtClass == TQString("TQListView") ) {
		emitTQListViewColumns( widget );
	    } else if ( unknown || gtkClass == TQString("Placeholder") ) {
		TQString prefix;
		if ( unknown )
		    prefix = gtkClass;
		emitProperty( TQString("text"),
			      TQString("<font color=\"red\">%1<b>?</b></font>")
			      .arg(prefix) );
		emitProperty( TQString("alignment"),
			      TQString("AlignAuto|AlignCenter"),
			      TQString("set") );
	    } else if ( qtClass != TQString("TQListBox") ) {
		emitChildWidgets( childWidgets, false );
	    }

	    emitClosing( TQString("widget") );
	    if ( needFakeLayout )
		emitClosing( TQString("vbox") );
	    if ( gtkClass == TQString("GnomeApp") )
		emitGnomeAppChildWidgetsPass2( childWidgets );
	}
    }
    return name;
}

TQStringList Glade2Ui::convertGladeFile( const TQString& fileName )
{
    TQStringList outFileNames;

    yyFileName = fileName;

    TQDomDocument doc( TQString("GTK-Interface") );
    TQFile f( fileName );
    if ( !f.open(IO_ReadOnly) ) {
	error( TQString("Cannot open file for reading.") );
	return TQStringList();
    }
    if ( !doc.setContent(&f) ) {
	error( TQString("File is not an XML file.") );
	f.close();
	return TQStringList();
    }
    f.close();

    TQDomElement root = doc.documentElement();
    if ( root.tagName() != TQString("GTK-Interface") ) {
	error( TQString("File is not a Glade XML file.") );
	return TQStringList();
    }

    int widgetNo = 0;
    int numWidgets = 0;
    TQDomNode n = root.firstChild();
    while ( !n.isNull() ) {
	if ( n.toElement().tagName() == TQString("widget") )
	    numWidgets++;	
	n = n.nextSibling();
    }

    TQProgressDialog fremskritt( TQString("Converting Glade files..."),
				TQString("Abort Conversion"), numWidgets, 0,
				"fremskritt", true );

    n = root.firstChild();
    while ( !n.isNull() ) {
	TQString tagName = n.toElement().tagName();

	if ( tagName == TQString("project") ) {
	    TQDomNode child = n.firstChild();
	    while ( !child.isNull() ) {
		TQString childTagName = child.toElement().tagName();
		if ( childTagName == TQString("pixmaps_directory") ) {
		    yyPixmapDirectory = getTextValue( child );
		} else if ( childTagName == TQString("program_name") ) {
		    yyProgramName = getTextValue( child );
		}
		child = child.nextSibling();
	    }
	} else if ( tagName == TQString("widget") ) {
	    yyOut = TQString::null;
	    yyCustomWidgets.clear();
	    yyStockItemActions.clear();
	    yyActions.clear();
	    yyConnections.clear();
	    yySlots.clear();
	    yyFormName = TQString::null;
	    yyImages.clear();

	    uniqueAction = 1;
	    uniqueForm = 1;
	    uniqueMenuBar = 1;
	    uniqueSpacer = 1;
	    uniqueToolBar = 1;

	    emitHeader();
	    TQString name = emitWidget( n.toElement(), false );

	    if ( !yyCustomWidgets.isEmpty() ) {
		emitOpening( TQString("customwidgets") );

		TQMap<TQString, TQString>::Iterator c = yyCustomWidgets.begin();
		while ( c != yyCustomWidgets.end() ) {
		    emitOpening( TQString("customwidget") );
		    emitSimpleValue( TQString("class"), c.key() );
		    if ( !(*c).isEmpty() )
			emitSimpleValue( TQString("header"), *c,
					 attribute(TQString("location"),
						   TQString("local")) );
		    emitClosing( TQString("customwidget") );
		    ++c;
		}
		emitClosing( TQString("customwidgets") );
	    }

	    if ( !yyActions.isEmpty() ) {
		emitOpening( TQString("actions") );

		TQMap<TQString, GladeAction>::Iterator a = yyActions.begin();
		while ( a != yyActions.end() ) {
		    emitOpening( TQString("action") );
		    emitProperty( TQString("name"),
				  fixedName(a.key()).latin1() );
		    emitProperty( TQString("text"), (*a).text );
		    emitProperty( TQString("menuText"), (*a).menuText );
		    if ( (*a).toolTip )
			emitProperty( TQString("toolTip"), (*a).toolTip );
		    if ( (*a).accel != 0 )
			emitProperty( TQString("accel"), (*a).accel );
		    if ( !(*a).iconSet.isEmpty() )
			emitProperty( TQString("iconSet"),
				      imageName((*a).iconSet),
				      TQString("iconset") );
		    emitClosing( TQString("action") );
		    ++a;
		}
		emitClosing( TQString("actions") );
	    }

	    if ( !yyImages.isEmpty() ) {
		uint maxLength = 524288;
		char *data = new char[maxLength];

		TQString dir = yyFileName;
		dir.truncate( dir.findRev(TQChar('/')) + 1 );
		dir += yyPixmapDirectory;

		emitOpening( TQString("images") );

		TQMap<TQString, TQString>::ConstIterator im = yyImages.begin();
		while ( im != yyImages.end() ) {
		    uint length = 0;
		    const char *format = 0;

		    TQString fn = dir + TQChar( '/' ) + im.key();
		    TQFile f( fn );
		    if ( !f.open(IO_ReadOnly) ) {
			error( TQString("Cannot open image '%1' for reading.")
			       .arg(fn) );
		    } else {
			length = f.readBlock( data, maxLength );
			f.at( 0 );
			format = TQImageIO::imageFormat( &f );
			f.close();
		    }
		    if ( format == 0 )
			format = "XPM";

		    AttributeMap attr;
		    attr.insert( TQString("format"), TQString(format) );
		    attr.insert( TQString("length"), TQString::number(length) );

		    emitOpening( TQString("image"),
				 attribute(TQString("name"), *im) );
		    emitSimpleValue( TQString("data"), hexed(data, length),
				     attr );
		    emitClosing( TQString("image") );
		    ++im;
		}
		emitClosing( TQString("images") );
		delete[] data;
	    }

	    if ( yyConnections.count() + yySlots.count() > 0 ) {
		emitOpening( TQString("connections") );

		TQValueList<GladeConnection>::Iterator c = yyConnections.begin();
		while ( c != yyConnections.end() ) {
		    emitOpening( TQString("connection") );
		    emitSimpleValue( TQString("sender"), (*c).sender );
		    emitSimpleValue( TQString("signal"), (*c).signal );
		    emitSimpleValue( TQString("receiver"), yyFormName );
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
	    emitFooter();

	    TQString outFileName = fileName;
	    int k = outFileName.findRev( "." );
	    if ( k != -1 )
		outFileName.truncate( k );
	    if ( widgetNo != 0 )
		outFileName += TQString::number( widgetNo + 1 );
	    outFileName += TQString( ".ui" );
	    FILE *out = fopen( outFileName.latin1(), "w" );
	    if ( out == 0 ) {
		tqWarning( "glade2ui: Could not open file '%s'",
			 outFileName.latin1() );
	    } else {
		if ( !yyOut.isEmpty() )
		    fprintf( out, "%s", yyOut.latin1() );
		fclose( out );
		outFileNames.push_back( outFileName );
	    }
	    yyOut = TQString::null;
	    widgetNo++;

	    tqApp->processEvents();
	    if ( fremskritt.wasCancelled() )
		break;
	    fremskritt.setProgress( widgetNo );
	}
	n = n.nextSibling();
    }
    return outFileNames;
}
