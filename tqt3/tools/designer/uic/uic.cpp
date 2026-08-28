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

#include "uic.h"
#include "parser.h"
#include "widgetdatabase.h"
#include "domtool.h"
#include <ntqfile.h>
#include <ntqstringlist.h>
#include <ntqdatetime.h>
#define NO_STATIC_COLORS
#include <globaldefs.h>
#include <ntqregexp.h>
#include <stdio.h>
#include <stdlib.h>

bool Uic::isMainWindow = false;

TQString Uic::getComment( const TQDomNode& n )
{
    TQDomNode child = n.firstChild();
    while ( !child.isNull() ) {
	if ( child.toElement().tagName() == "comment" )
	    return child.toElement().firstChild().toText().data();
	child = child.nextSibling();
    }
    return TQString::null;
}

TQString Uic::mkBool( bool b )
{
    return b? "true" : "false";
}

TQString Uic::mkBool( const TQString& s )
{
    return mkBool( s == "true" || s == "1" );
}

bool Uic::toBool( const TQString& s )
{
    return s == "true" || s.toInt() != 0;
}

TQString Uic::fixString( const TQString &str, bool encode )
{
    TQString s;
    if ( !encode ) {
	s = str;
	s.replace( "\\", "\\\\" );
	s.replace( "\"", "\\\"" );
	s.replace( "\r", "" );
	s.replace( "\n", "\\n\"\n\"" );
    } else {
	TQCString utf8 = str.utf8();
	const int l = utf8.length();

        for ( int i = 0; i < l; ++i ) {
            uchar c = (uchar)utf8[i];
            if (c != 0x0d) // skip \r
               s += "\\x" + TQString::number(c, 16);

            if (i && (i % 20) == 0)
                s += "\"\n    \"";
        }
    }

    return "\"" + s + "\"";
}

TQString Uic::trcall( const TQString& sourceText, const TQString& comment )
{
    if ( sourceText.isEmpty() && comment.isEmpty() )
	return "TQString::null";

    TQString t = trmacro;
    bool encode = false;
    if ( t.isNull() ) {
	t = "tr";
	for ( int i = 0; i < (int) sourceText.length(); i++ ) {
	    if ( sourceText[i].unicode() >= 0x80 ) {
		t = "trUtf8";
		encode = true;
		break;
	    }
	}
    }

    if ( comment.isEmpty() ) {
	return t + "( " + fixString( sourceText, encode ) + " )";
    } else {
	return t + "( " + fixString( sourceText, encode ) + ", " +
	       fixString( comment, encode ) + " )";
    }
}

TQString Uic::mkStdSet( const TQString& prop )
{
    return TQString( "set" ) + prop[0].upper() + prop.mid(1);
}



/*!
  \class Uic uic.h
  \brief User Interface Compiler

  The class Uic encapsulates the user interface compiler (uic).
 */
Uic::Uic( const TQString &fn, const char *outputFn, TQTextStream &outStream,
	  TQDomDocument doc, bool decl, bool subcl, const TQString &trm,
	  const TQString& subClass, bool omitForwardDecls )
    : out( outStream ), trout( &languageChangeBody ),
      outputFileName( outputFn ), trmacro( trm ), nofwd( omitForwardDecls )
{
    fileName = fn;
    writeFunctImpl = true;
    defMargin = BOXLAYOUT_DEFAULT_MARGIN;
    defSpacing = BOXLAYOUT_DEFAULT_SPACING;
    externPixmaps = false;
    indent = "    "; // default indent

    item_used = cg_used = pal_used = 0;

    layouts << "hbox" << "vbox" << "grid";
    tags = layouts;
    tags << "widget";

    pixmapLoaderFunction = getPixmapLoaderFunction( doc.firstChild().toElement() );
    nameOfClass = getFormClassName( doc.firstChild().toElement() );

    uiFileVersion = doc.firstChild().toElement().attribute("version");
    stdsetdef = toBool( doc.firstChild().toElement().attribute("stdsetdef") );

    if ( doc.firstChild().isNull() || doc.firstChild().firstChild().isNull() )
	return;
    TQDomElement e = doc.firstChild().firstChild().toElement();
    TQDomElement widget;
    while ( !e.isNull() ) {
	if ( e.tagName() == "widget" ) {
	    widget = e;
	} else if ( e.tagName() == "pixmapinproject" ) {
	    externPixmaps = true;
	} else if ( e.tagName() == "layoutdefaults" ) {
	    defSpacing = e.attribute( "spacing", defSpacing.toString() );
	    defMargin = e.attribute( "margin", defMargin.toString() );
	} else if ( e.tagName() == "layoutfunctions" ) {
	    defSpacing = e.attribute( "spacing", defSpacing.toString() );
	    bool ok;
	    defSpacing.toInt( &ok );
	    if ( !ok ) {
		TQString buf = defSpacing.toString();
		defSpacing = buf.append( "()" );
	    }
	    defMargin = e.attribute( "margin", defMargin.toString() );
	    defMargin.toInt( &ok );
	    if ( !ok ) {
		TQString buf = defMargin.toString();
		defMargin = buf.append( "()" );
	    }
	}
	e = e.nextSibling().toElement();
    }
    e = widget;

    if ( nameOfClass.isEmpty() )
	nameOfClass = getObjectName( e );
    namespaces = TQStringList::split( "::", nameOfClass );
    bareNameOfClass = namespaces.last();
    namespaces.remove( namespaces.fromLast() );

    if ( subcl ) {
	if ( decl )
	    createSubDecl( e, subClass );
	else
	    createSubImpl( e, subClass );
    } else {
	if ( decl )
	    createFormDecl( e );
	else
	    createFormImpl( e );
    }

}

/*! Extracts a pixmap loader function from \a e
 */
TQString Uic::getPixmapLoaderFunction( const TQDomElement& e )
{
    TQDomElement n;
    for ( n = e.firstChild().toElement(); !n.isNull(); n = n.nextSibling().toElement() ) {
	if ( n.tagName() == "pixmapfunction" )
	    return n.firstChild().toText().data();
    }
    return TQString::null;
}


/*! Extracts the forms class name from \a e
 */
TQString Uic::getFormClassName( const TQDomElement& e )
{
    TQDomElement n;
    TQString cn;
    for ( n = e.firstChild().toElement(); !n.isNull(); n = n.nextSibling().toElement() ) {
	if ( n.tagName() == "class" ) {
	    TQString s = n.firstChild().toText().data();
	    int i;
	    while ( ( i = s.find(' ' )) != -1 )
		s[i] = '_';
	    cn = s;
	}
    }
    return cn;
}

/*! Extracts a class name from \a e.
 */
TQString Uic::getClassName( const TQDomElement& e )
{
    TQString s = e.attribute( "class" );
    if ( s.isEmpty() && e.tagName() == "toolbar" )
	s = "TQToolBar";
    else if ( s.isEmpty() && e.tagName() == "menubar" )
	s = "TQMenuBar";
    return s;
}

/*! Returns true if database framework code is generated, else false.
*/

bool Uic::isFrameworkCodeGenerated( const TQDomElement& e )
{
    TQDomElement n = getObjectProperty( e, "frameworkCode" );
    if ( n.attribute("name") == "frameworkCode" &&
	 !DomTool::elementToVariant( n.firstChild().toElement(), TQVariant( true ) ).toBool() )
	return false;
    return true;
}

/*! Extracts an object name from \a e. It's stored in the 'name'
 property.
 */
TQString Uic::getObjectName( const TQDomElement& e )
{
    TQDomElement n = getObjectProperty( e, "name" );
    if ( n.firstChild().toElement().tagName() == "cstring" )
	return n.firstChild().toElement().firstChild().toText().data();
    return TQString::null;
}

/*! Extracts an layout name from \a e. It's stored in the 'name'
 property of the preceeding sibling (the first child of a TQLayoutWidget).
 */
TQString Uic::getLayoutName( const TQDomElement& e )
{
    TQDomElement p = e.parentNode().toElement();
    TQString name;

    if ( getClassName(p) != "TQLayoutWidget" )
	name = "Layout";

    TQDomElement n = getObjectProperty( p, "name" );
    if ( n.firstChild().toElement().tagName() == "cstring" ) {
	name.prepend( n.firstChild().toElement().firstChild().toText().data() );
	return TQStringList::split( "::", name ).last();
    }
    return e.tagName();
}


TQString Uic::getDatabaseInfo( const TQDomElement& e, const TQString& tag )
{
    TQDomElement n;
    TQDomElement n1;
    int child = 0;
    // database info is a stringlist stored in this order
    if ( tag == "connection" )
	child = 0;
    else if ( tag == "table" )
	child = 1;
    else if ( tag == "field" )
	child = 2;
    else
	return TQString::null;
    n = getObjectProperty( e, "database" );
    if ( n.firstChild().toElement().tagName() == "stringlist" ) {
	    // find correct stringlist entry
	    TQDomElement n1 = n.firstChild().firstChild().toElement();
	    for ( int i = 0; i < child && !n1.isNull(); ++i )
		n1 = n1.nextSibling().toElement();
	    if ( n1.isNull() )
		return TQString::null;
	    return n1.firstChild().toText().data();
    }
    return TQString::null;
}


void Uic::registerLayouts( const TQDomElement &e )
{
    if ( layouts.contains(e.tagName())) {
	createObjectDecl(e);
	TQString t = e.tagName();
	if ( t == "vbox" || t == "hbox" || t == "grid" )
	    createSpacerDecl( e );
    }

    TQDomNodeList nl = e.childNodes();
    for ( int i = 0; i < (int) nl.length(); ++i )
	registerLayouts( nl.item(i).toElement() );
}


/*!
  Returns include file for class \a className or a null string.
 */
TQString Uic::getInclude( const TQString& className )
{
    int wid = WidgetDatabase::idFromClassName( className );
    if ( wid != -1 )
	return WidgetDatabase::includeFile( wid );
    return TQString::null;
}


void Uic::createActionDecl( const TQDomElement& e )
{
    TQString objClass = e.tagName() == "action" ? "TQAction" : "TQActionGroup";
    TQString objName = getObjectName( e );
    if ( objName.isEmpty() )
	return;
    out << "    " << objClass << "* " << objName << ";" << endl;
    if ( e.tagName() == "actiongroup" ) {
	for ( TQDomElement n = e.firstChild().toElement(); !n.isNull(); n = n.nextSibling().toElement() ) {
	    if ( n.tagName() == "action" || n.tagName() == "actiongroup" )
		createActionDecl( n );
	}
    }
}

void Uic::createToolbarDecl( const TQDomElement &e )
{
    if ( e.tagName() == "toolbar" )
	out << "    " << "TQToolBar *" << getObjectName( e ) << ";" << endl;
}

void Uic::createMenuBarDecl( const TQDomElement &e )
{
    if ( e.tagName() == "item" ) {
	out << "    " << "TQPopupMenu *" << e.attribute( "name" ) << ";" << endl;
	createPopupMenuDecl( e );
    }
}

void Uic::createPopupMenuDecl( const TQDomElement &e )
{
    for ( TQDomElement n = e.firstChild().toElement(); !n.isNull(); n = n.nextSibling().toElement() ) {
	if ( n.tagName() == "action" || n.tagName() == "actiongroup" ) {
	    TQDomElement n2 = n.nextSibling().toElement();
	    if ( n2.tagName() == "item" ) { // the action has a sub menu
		out << "    " << "TQPopupMenu *" << n2.attribute( "name" ) << ";" << endl;
		createPopupMenuDecl( n2 );
		n = n2;
	    }
	}
    }
}

void Uic::createActionImpl( const TQDomElement &n, const TQString &parent )
{
    for ( TQDomElement ae = n; !ae.isNull(); ae = ae.nextSibling().toElement() ) {
	TQString objName = registerObject( getObjectName( ae ) );
	if ( ae.tagName() == "action" )
	    out << indent << objName << " = new TQAction( " << parent << ", \"" << objName << "\" );" << endl;
	else if ( ae.tagName() == "actiongroup" )
	    out << indent << objName << " = new TQActionGroup( " << parent << ", \"" << objName << "\" );" << endl;
	else
	    continue;
	bool subActionsDone = false;
	bool hasMenuText = false;
	TQString actionText;
	for ( TQDomElement n2 = ae.firstChild().toElement(); !n2.isNull(); n2 = n2.nextSibling().toElement() ) {
	    if ( n2.tagName() == "property" ) {
		bool stdset = stdsetdef;
		if ( n2.hasAttribute( "stdset" ) )
		    stdset = toBool( n2.attribute( "stdset" ) );
		TQString prop = n2.attribute("name");
		if ( prop == "name" )
		    continue;
		TQString value = setObjectProperty( "TQAction", objName, prop, n2.firstChild().toElement(), stdset );
		if ( value.isEmpty() )
		    continue;

		TQString call = objName + "->";
		if ( stdset ) {
		    call += mkStdSet( prop ) + "( ";
		} else {
		    call += "setProperty( \"" + prop + "\", ";
		}
		call += value + " );";

		if (prop == "menuText")
		    hasMenuText = true;
		else if (prop == "text")
		    actionText = value;

		if ( n2.firstChild().toElement().tagName() == "string" ) {
		    trout << indent << call << endl;
		} else {
		    out << indent << call << endl;
		}
	    } else if ( !subActionsDone && ( n2.tagName() == "actiongroup" || n2.tagName() == "action" ) ) {
		createActionImpl( n2, objName );
		subActionsDone = true;
	    }
	}
	// workaround for loading pre-3.3 files expecting bogus TQAction behavior
	if (!hasMenuText && !actionText.isEmpty() && uiFileVersion < "3.3")
	    trout << indent << objName << "->setMenuText(" << actionText << ");" << endl;
    }
}

TQString get_dock( const TQString &d )
{
    if ( d == "0" )
	return "DockUnmanaged";
    if ( d == "1" )
	return "DockTornOff";
    if ( d == "2" )
	return "DockTop";
    if ( d == "3" )
	return "DockBottom";
    if ( d == "4" )
	return "DockRight";
    if ( d == "5" )
	return "DockLeft";
    if ( d == "6" )
	return "DockMinimized";
    return "";
}

void Uic::createToolbarImpl( const TQDomElement &n, const TQString &parentClass, const TQString &parent )
{
    TQDomNodeList nl = n.elementsByTagName( "toolbar" );
    for ( int i = 0; i < (int) nl.length(); i++ ) {
	TQDomElement ae = nl.item( i ).toElement();
	TQString dock = get_dock( ae.attribute( "dock" ) );
	TQString objName = getObjectName( ae );
 	out << indent << objName << " = new TQToolBar( TQString(\"\"), this, " << dock << " ); " << endl;
	createObjectImpl( ae, parentClass, parent );
	for ( TQDomElement n2 = ae.firstChild().toElement(); !n2.isNull(); n2 = n2.nextSibling().toElement() ) {
	    if ( n2.tagName() == "action" ) {
		out << indent << n2.attribute( "name" ) << "->addTo( " << objName << " );" << endl;
	    } else if ( n2.tagName() == "separator" ) {
		out << indent << objName << "->addSeparator();" << endl;
	    } else if ( n2.tagName() == "widget" ) {
		if ( n2.attribute( "class" ) != "Spacer" ) {
		    createObjectImpl( n2, "TQToolBar", objName );
		} else {
		    TQString child = createSpacerImpl( n2, parentClass, parent, objName );
		    out << indent << "TQApplication::sendPostedEvents( " << objName
			<< ", TQEvent::ChildInserted );" << endl;
		    out << indent << objName << "->boxLayout()->addItem( " << child << " );" << endl;
		}
	    }
	}
    }
}

void Uic::createMenuBarImpl( const TQDomElement &n, const TQString &parentClass, const TQString &parent )
{
    TQString objName = getObjectName( n );
    out << indent << objName << " = new TQMenuBar( this, \"" << objName << "\" );" << endl;
    createObjectImpl( n, parentClass, parent );
    int i = 0;
    TQDomElement c = n.firstChild().toElement();
    while ( !c.isNull() ) {
	if ( c.tagName() == "item" ) {
	    TQString itemName = c.attribute( "name" );
	    out << endl;
	    out << indent << itemName << " = new TQPopupMenu( this );" << endl;
	    createPopupMenuImpl( c, parentClass, itemName );
	    out << indent << objName << "->insertItem( TQString(\"\"), " << itemName << ", " << i << " );" << endl;
	    TQString findItem(objName + "->findItem(%1)");
	    findItem = findItem.arg(i);
	    trout << indent << "if (" << findItem << ")" << endl;
	    trout << indent << indent << findItem << "->setText( " << trcall( c.attribute( "text" ) ) << " );" << endl;
	} else if ( c.tagName() == "separator" ) {
	    out << endl;
	    out << indent << objName << "->insertSeparator( " << i << " );" << endl;
	}
	c = c.nextSibling().toElement();
	i++;
    }
}

void Uic::createPopupMenuImpl( const TQDomElement &e, const TQString &parentClass, const TQString &parent )
{
    int i = 0;
    for ( TQDomElement n = e.firstChild().toElement(); !n.isNull(); n = n.nextSibling().toElement() ) {
	if ( n.tagName() == "action" || n.tagName() == "actiongroup" ) {
	    TQDomElement n2 = n.nextSibling().toElement();
	    if ( n2.tagName() == "item" ) { // the action has a sub menu
		TQString itemName = n2.attribute( "name" );
		TQString itemText = n2.attribute( "text" );
		out << indent << itemName << " = new TQPopupMenu( this );" << endl;
		out << indent << parent << "->insertItem( "  << n.attribute( "name" ) << "->iconSet(), ";
		out << trcall( itemText ) << ", " << itemName << " );" << endl;
		trout << indent << parent << "->changeItem( " << parent << "->idAt( " << i << " ), ";
		trout << trcall( itemText ) << " );" << endl;
		createPopupMenuImpl( n2, parentClass, itemName );
		n = n2;
	    } else {
		out << indent << n.attribute( "name" ) << "->addTo( " << parent << " );" << endl;
	    }
	} else if ( n.tagName() == "separator" ) {
	    out << indent << parent << "->insertSeparator();" << endl;
	}
	++i;
    }
}

/*!
  Creates implementation of an listbox item tag.
*/

TQString Uic::createListBoxItemImpl( const TQDomElement &e, const TQString &parent,
				    TQString *value )
{
    TQDomElement n = e.firstChild().toElement();
    TQString txt;
    TQString com;
    TQString pix;
    while ( !n.isNull() ) {
	if ( n.tagName() == "property" ) {
	    TQString attrib = n.attribute( "name" );
	    TQVariant v = DomTool::elementToVariant( n.firstChild().toElement(), TQVariant() );
	    if ( attrib == "text" ) {
		txt = v.toString();
		com = getComment( n );
	    } else if ( attrib == "pixmap" ) {
		pix = v.toString();
		if ( !pix.isEmpty() && !pixmapLoaderFunction.isEmpty() ) {
		    pix.prepend( pixmapLoaderFunction + "( " + TQString( externPixmaps ? "\"" : "" ) );
		    pix.append(  TQString( externPixmaps ? "\"" : "" ) + " )" );
		}
	    }
	}
	n = n.nextSibling().toElement();
    }

    if ( value )
	*value = trcall( txt, com );

    if ( pix.isEmpty() ) {
	return parent + "->insertItem( " + trcall( txt, com ) + " );";
    } else {
	return parent + "->insertItem( " + pix + ", " + trcall( txt, com ) + " );";
    }
}

/*!
  Creates implementation of an iconview item tag.
*/

TQString Uic::createIconViewItemImpl( const TQDomElement &e, const TQString &parent )
{
    TQDomElement n = e.firstChild().toElement();
    TQString txt;
    TQString com;
    TQString pix;
    while ( !n.isNull() ) {
	if ( n.tagName() == "property" ) {
	    TQString attrib = n.attribute( "name" );
	    TQVariant v = DomTool::elementToVariant( n.firstChild().toElement(), TQVariant() );
	    if ( attrib == "text" ) {
		txt = v.toString();
		com = getComment( n );
	    } else if ( attrib == "pixmap" ) {
		pix = v.toString();
		if ( !pix.isEmpty() && !pixmapLoaderFunction.isEmpty() ) {
		    pix.prepend( pixmapLoaderFunction + "( " + TQString( externPixmaps ? "\"" : "" ) );
		    pix.append( TQString( externPixmaps ? "\"" : "" ) + " )" );
		}
	    }
	}
	n = n.nextSibling().toElement();
    }

    if ( pix.isEmpty() )
	return "(void) new TQIconViewItem( " + parent + ", " + trcall( txt, com ) + " );";
    else
	return "(void) new TQIconViewItem( " + parent + ", " + trcall( txt, com ) + ", " + pix + " );";
}

/*!
  Creates implementation of an listview item tag.
*/

TQString Uic::createListViewItemImpl( const TQDomElement &e, const TQString &parent,
				     const TQString &parentItem )
{
    TQString s;

    TQDomElement n = e.firstChild().toElement();

    bool hasChildren = e.elementsByTagName( "item" ).count() > 0;
    TQString item;

    if ( hasChildren ) {
	item = registerObject( "item" );
	s = indent + "TQListViewItem * " + item + " = ";
    } else {
	item = "item";
	if ( item_used )
	    s = indent + item + " = ";
	else
	    s = indent + "TQListViewItem * " + item + " = ";
	item_used = true;
    }

    if ( !parentItem.isEmpty() )
	s += "new TQListViewItem( " + parentItem + ", " + lastItem + " );\n";
    else
	s += "new TQListViewItem( " + parent + ", " + lastItem + " );\n";

    TQStringList texts;
    TQStringList pixmaps;
    while ( !n.isNull() ) {
	if ( n.tagName() == "property" ) {
	    TQString attrib = n.attribute("name");
	    TQVariant v = DomTool::elementToVariant( n.firstChild().toElement(), TQVariant() );
	    if ( attrib == "text" )
		texts << v.toString();
	    else if ( attrib == "pixmap" ) {
		TQString pix = v.toString();
		if ( !pix.isEmpty() && !pixmapLoaderFunction.isEmpty() ) {
		    pix.prepend( pixmapLoaderFunction + "( " + TQString( externPixmaps ? "\"" : "" ) );
		    pix.append( TQString( externPixmaps ? "\"" : "" ) + " )" );
		}
		pixmaps << pix;
	    }
	} else if ( n.tagName() == "item" ) {
	    s += indent + item + "->setOpen( true );\n";
	    s += createListViewItemImpl( n, parent, item );
	}
	n = n.nextSibling().toElement();
    }

    for ( int i = 0; i < (int)texts.count(); ++i ) {
	if ( !texts[ i ].isEmpty() )
	    s += indent + item + "->setText( " + TQString::number( i ) + ", " + trcall( texts[ i ] ) + " );\n";
	if ( !pixmaps[ i ].isEmpty() )
	    s += indent + item + "->setPixmap( " + TQString::number( i ) + ", " + pixmaps[ i ] + " );\n";
    }

    lastItem = item;
    return s;
}

/*!
  Creates implementation of an listview column tag.
*/

TQString Uic::createListViewColumnImpl( const TQDomElement &e, const TQString &parent,
				       TQString *value )
{
    TQDomElement n = e.firstChild().toElement();
    TQString txt;
    TQString com;
    TQString pix;
    bool clickable = false, resizable = false;
    while ( !n.isNull() ) {
	if ( n.tagName() == "property" ) {
	    TQString attrib = n.attribute("name");
	    TQVariant v = DomTool::elementToVariant( n.firstChild().toElement(), TQVariant() );
	    if ( attrib == "text" ) {
		txt = v.toString();
		com = getComment( n );
	    } else if ( attrib == "pixmap" ) {
		pix = v.toString();
		if ( !pix.isEmpty() && !pixmapLoaderFunction.isEmpty() ) {
		    pix.prepend( pixmapLoaderFunction + "( " + TQString( externPixmaps ? "\"" : "" ) );
		    pix.append( TQString( externPixmaps ? "\"" : "" ) + " )" );
		}
	    } else if ( attrib == "clickable" )
		clickable = v.toBool();
	    else if ( attrib == "resizable" || attrib == "resizeable" )
		resizable = v.toBool();
	}
	n = n.nextSibling().toElement();
    }

    if ( value )
	*value = trcall( txt, com );

    TQString s;
    s = indent + parent + "->addColumn( " + trcall( txt, com ) + " );\n";
    if ( !pix.isEmpty() )
	s += indent + parent + "->header()->setLabel( " + parent + "->header()->count() - 1, " + pix + ", " + trcall( txt, com ) + " );\n";
    if ( !clickable )
	s += indent + parent + "->header()->setClickEnabled( false, " + parent + "->header()->count() - 1 );\n";
    if ( !resizable )
	s += indent + parent + "->header()->setResizeEnabled( false, " + parent + "->header()->count() - 1 );\n";
    return s;
}

TQString Uic::createTableRowColumnImpl( const TQDomElement &e, const TQString &parent,
				       TQString *value )
{
    TQString objClass = getClassName( e.parentNode().toElement() );
    TQDomElement n = e.firstChild().toElement();
    TQString txt;
    TQString com;
    TQString pix;
    TQString field;
    bool isRow = e.tagName() == "row";
    while ( !n.isNull() ) {
	if ( n.tagName() == "property" ) {
	    TQString attrib = n.attribute("name");
	    TQVariant v = DomTool::elementToVariant( n.firstChild().toElement(), TQVariant() );
	    if ( attrib == "text" ) {
		txt = v.toString();
		com = getComment( n );
	    } else if ( attrib == "pixmap" ) {
		pix = v.toString();
		if ( !pix.isEmpty() && !pixmapLoaderFunction.isEmpty() ) {
		    pix.prepend( pixmapLoaderFunction + "( " + TQString( externPixmaps ? "\"" : "" ) );
		    pix.append( TQString( externPixmaps ? "\"" : "" ) + " )" );
		}
	    } else if ( attrib == "field" )
		field = v.toString();
	}
	n = n.nextSibling().toElement();
    }

    if ( value )
	*value = trcall( txt, com );

    // ### This generated code sucks! We have to set the number of
    // rows/cols before and then only do setLabel/()
    // ### careful, though, since TQDataTable has an API which makes this code pretty good

    TQString s;
    if ( isRow ) {
	s = indent + parent + "->setNumRows( " + parent + "->numRows() + 1 );\n";
	if ( pix.isEmpty() )
	    s += indent + parent + "->verticalHeader()->setLabel( " + parent + "->numRows() - 1, "
		 + trcall( txt, com ) + " );\n";
	else
	    s += indent + parent + "->verticalHeader()->setLabel( " + parent + "->numRows() - 1, "
		 + pix + ", " + trcall( txt, com ) + " );\n";
    } else {
	if ( objClass == "TQTable" ) {
	    s = indent + parent + "->setNumCols( " + parent + "->numCols() + 1 );\n";
	    if ( pix.isEmpty() )
		s += indent + parent + "->horizontalHeader()->setLabel( " + parent + "->numCols() - 1, "
		     + trcall( txt, com ) + " );\n";
	    else
		s += indent + parent + "->horizontalHeader()->setLabel( " + parent + "->numCols() - 1, "
		     + pix + ", " + trcall( txt, com ) + " );\n";
	} else if ( objClass == "TQDataTable" ) {
	    if ( !txt.isEmpty() && !field.isEmpty() ) {
		if ( pix.isEmpty() )
		    out << indent << parent << "->addColumn( " << fixString( field ) << ", " << trcall( txt, com ) << " );" << endl;
		else
		    out << indent << parent << "->addColumn( " << fixString( field ) << ", " << trcall( txt, com ) << ", " << pix << " );" << endl;
	    }
	}
    }
    return s;
}

/*!
  Creates the implementation of a layout tag. Called from createObjectImpl().
 */
TQString Uic::createLayoutImpl( const TQDomElement &e, const TQString& parentClass, const TQString& parent, const TQString& layout )
{
    TQDomElement n;
    TQString objClass, objName;
    objClass = e.tagName();

    TQString qlayout = "TQVBoxLayout";
    if ( objClass == "hbox" )
	qlayout = "TQHBoxLayout";
    else if ( objClass == "grid" )
	qlayout = "TQGridLayout";

    bool isGrid = e.tagName() == "grid" ;
    objName = registerObject( getLayoutName( e ) );
    layoutObjects += objName;

    TQString margin = DomTool::readProperty( e, "margin", defMargin ).toString();
    TQString spacing = DomTool::readProperty( e, "spacing", defSpacing ).toString();
    TQString resizeMode = DomTool::readProperty( e, "resizeMode", TQString::null ).toString();

    TQString optcells;
    if ( isGrid )
	optcells = "1, 1, ";
    if ( (parentClass == "TQGroupBox" || parentClass == "TQButtonGroup") && layout.isEmpty() ) {
	// special case for group box
	out << indent << parent << "->setColumnLayout(0, TQt::Vertical );" << endl;
	out << indent << parent << "->layout()->setSpacing( " << spacing << " );" << endl;
	out << indent << parent << "->layout()->setMargin( " << margin << " );" << endl;
	out << indent << objName << " = new " << qlayout << "( " << parent << "->layout() );" << endl;
	out << indent << objName << "->setAlignment( TQt::AlignTop );" << endl;
    } else {
	out << indent << objName << " = new " << qlayout << "( ";
	if ( layout.isEmpty() )
	    out << parent;
	else {
	    out << "0";
	    if ( !DomTool::hasProperty( e, "margin" ) )
		margin = "0";
	}
	out << ", " << optcells << margin << ", " << spacing << ", \"" << objName << "\"); " << endl;
    }
    if ( !resizeMode.isEmpty() )
	out << indent << objName << "->setResizeMode( TQLayout::" << resizeMode << " );" << endl;

    if ( !isGrid ) {
	for ( n = e.firstChild().toElement(); !n.isNull(); n = n.nextSibling().toElement() ) {
	    if ( n.tagName() == "spacer" ) {
		TQString child = createSpacerImpl( n, parentClass, parent, objName );
		out << indent << objName << "->addItem( " << child << " );" << endl;
	    } else if ( tags.contains( n.tagName() ) ) {
		TQString child = createObjectImpl( n, parentClass, parent, objName );
		if ( isLayout( child ) )
		    out << indent << objName << "->addLayout( " << child << " );" << endl;
		else
		    out << indent << objName << "->addWidget( " << child << " );" << endl;
	    }
	}
    } else {
	for ( n = e.firstChild().toElement(); !n.isNull(); n = n.nextSibling().toElement() ) {
	    TQDomElement ae = n;
	    int row = ae.attribute( "row" ).toInt();
	    int col = ae.attribute( "column" ).toInt();
	    int rowspan = ae.attribute( "rowspan" ).toInt();
	    int colspan = ae.attribute( "colspan" ).toInt();
	    if ( rowspan < 1 )
		rowspan = 1;
	    if ( colspan < 1 )
		colspan = 1;
	    if ( n.tagName() == "spacer" ) {
		TQString child = createSpacerImpl( n, parentClass, parent, objName );
		if ( rowspan * colspan != 1 )
		    out << indent << objName << "->addMultiCell( " << child << ", "
			<< row << ", " << ( row + rowspan - 1 ) << ", " << col << ", " << ( col  + colspan - 1 ) << " );" << endl;
		else
		    out << indent << objName << "->addItem( " << child << ", "
			<< row << ", " << col << " );" << endl;
	    } else if ( tags.contains( n.tagName() ) ) {
		TQString child = createObjectImpl( n, parentClass, parent, objName );
		out << endl;
		TQString o = "Widget";
		if ( isLayout( child ) )
		    o = "Layout";
		if ( rowspan * colspan != 1 )
		    out << indent << objName << "->addMultiCell" << o << "( " << child << ", "
			<< row << ", " << ( row + rowspan - 1 ) << ", " << col << ", " << ( col  + colspan - 1 ) << " );" << endl;
		else
		    out << indent << objName << "->add" << o << "( " << child << ", "
			<< row << ", " << col << " );" << endl;
	    }
	}
    }

    return objName;
}



TQString Uic::createSpacerImpl( const TQDomElement &e, const TQString& /*parentClass*/, const TQString& /*parent*/, const TQString& /*layout*/)
{
    TQDomElement n;
    TQString objClass, objName;
    objClass = e.tagName();
    objName = registerObject( getObjectName( e ) );

    TQSize size = DomTool::readProperty( e, "sizeHint", TQSize( 0, 0 ) ).toSize();
    TQString sizeType = DomTool::readProperty( e, "sizeType", "Expanding" ).toString();
    bool isVspacer = DomTool::readProperty( e, "orientation", "Horizontal" ) == "Vertical";

    if ( sizeType != "Expanding" && sizeType != "MinimumExpanding" &&
	 DomTool::hasProperty( e, "geometry" ) ) { // compatibility TQt 2.2
	TQRect geom = DomTool::readProperty( e, "geometry", TQRect(0,0,0,0) ).toRect();
	size = geom.size();
    }

    if ( isVspacer )
	out << "    " << objName << " = new TQSpacerItem( "
	    << size.width() << ", " << size.height()
	    << ", TQSizePolicy::Minimum, TQSizePolicy::" << sizeType << " );" << endl;
    else
	out << "    " << objName << " = new TQSpacerItem( "
	    << size.width() << ", " << size.height()
	    << ", TQSizePolicy::" << sizeType << ", TQSizePolicy::Minimum );" << endl;

    return objName;
}

static const char* const ColorRole[] = {
    "Foreground", "Button", "Light", "Midlight", "Dark", "Mid",
    "Text", "BrightText", "ButtonText", "Base", "Background", "Shadow",
    "Highlight", "HighlightedText", "Link", "LinkVisited", 0
};


/*!
  Creates a colorgroup with name \a name from the color group \a cg
 */
void Uic::createColorGroupImpl( const TQString& name, const TQDomElement& e )
{
    TQColorGroup cg;
    int r = -1;
    TQDomElement n = e.firstChild().toElement();
    TQString color;
    while ( !n.isNull() ) {
	if ( n.tagName() == "color" ) {
	    r++;
	    TQColor col = DomTool::readColor( n );
	    color = "TQColor( %1, %2, %3)";
	    color = color.arg( col.red() ).arg( col.green() ).arg( col.blue() );
	    if ( col == white )
		color = "white";
	    else if ( col == black )
	    color = "black";
	    if ( n.nextSibling().toElement().tagName() != "pixmap" ) {
		out << indent << name << ".setColor( TQColorGroup::" << ColorRole[r] << ", " << color << " );" << endl;
	    }
	} else if ( n.tagName() == "pixmap" ) {
	    TQString pixmap = n.firstChild().toText().data();
	    if ( !pixmapLoaderFunction.isEmpty() ) {
		pixmap.prepend( pixmapLoaderFunction + "( " + TQString( externPixmaps ? "\"" : "" ) );
		pixmap.append( TQString( externPixmaps ? "\"" : "" ) + " )" );
	    }
	    out << indent << name << ".setBrush( TQColorGroup::"
		<< ColorRole[r] << ", TQBrush( " << color << ", " << pixmap << " ) );" << endl;
	}
	n = n.nextSibling().toElement();
    }
}

/*!
  Auxiliary function to load a color group. The colorgroup must not
  contain pixmaps.
 */
TQColorGroup Uic::loadColorGroup( const TQDomElement &e )
{
    TQColorGroup cg;
    int r = -1;
    TQDomElement n = e.firstChild().toElement();
    TQColor col;
    while ( !n.isNull() ) {
	if ( n.tagName() == "color" ) {
	    r++;
	    cg.setColor( (TQColorGroup::ColorRole)r, (col = DomTool::readColor( n ) ) );
	}
	n = n.nextSibling().toElement();
    }
    return cg;
}

/*!  Returns true if the widget properties specify that it belongs to
  the database \a connection and \a table.
*/

bool Uic::isWidgetInTable( const TQDomElement& e, const TQString& connection, const TQString& table )
{
    TQString conn = getDatabaseInfo( e, "connection" );
    TQString tab = getDatabaseInfo( e, "table" );
    if ( conn == connection && tab == table )
	return true;
    return false;
}

/*!
  Registers all database connections, cursors and forms.
*/

void Uic::registerDatabases( const TQDomElement& e )
{
    TQDomElement n;
    TQDomNodeList nl;
    int i;
    nl = e.parentNode().toElement().elementsByTagName( "widget" );
    for ( i = 0; i < (int) nl.length(); ++i ) {
	n = nl.item(i).toElement();
	TQString conn = getDatabaseInfo( n, "connection"  );
	TQString tab = getDatabaseInfo( n, "table"  );
	TQString fld = getDatabaseInfo( n, "field"  );
	if ( !conn.isNull() ) {
	    dbConnections += conn;
	    if ( !tab.isNull() ) {
		dbCursors[conn] += tab;
		if ( !fld.isNull() )
		    dbForms[conn] += tab;
	    }
	}
    }
}

/*!
  Registers an object with name \a name.

  The returned name is a valid variable identifier, as similar to \a
  name as possible and guaranteed to be unique within the form.

  \sa registeredName(), isObjectRegistered()
 */
TQString Uic::registerObject( const TQString& name )
{
    if ( objectNames.isEmpty() ) {
	// some temporary variables we need
	objectNames += "img";
	objectNames += "item";
	objectNames += "cg";
	objectNames += "pal";
    }

    TQString result = name;
    int i;
    while ( ( i = result.find(' ' )) != -1  ) {
	result[i] = '_';
    }

    if ( objectNames.contains( result ) ) {
	int i = 2;
	while ( objectNames.contains( result + "_" + TQString::number(i) ) )
	    i++;
	result += "_";
	result += TQString::number(i);
    }
    objectNames += result;
    objectMapper.insert( name, result );
    return result;
}

/*!
  Returns the registered name for the original name \a name
  or \a name if \a name  wasn't registered.

  \sa registerObject(), isObjectRegistered()
 */
TQString Uic::registeredName( const TQString& name )
{
    if ( !objectMapper.contains( name ) )
	return name;
    return objectMapper[name];
}

/*!
  Returns whether the object \a name was registered yet or not.
 */
bool Uic::isObjectRegistered( const TQString& name )
{
    return objectMapper.contains( name );
}


/*!
  Unifies the entries in stringlist \a list. Should really be a TQStringList feature.
 */
TQStringList Uic::unique( const TQStringList& list )
{
    if ( list.isEmpty() )
	return list;

    TQStringList result;
    for ( TQStringList::ConstIterator it = list.begin(); it != list.end(); ++it ) {
	if ( !result.contains(*it) )
	    result += *it;
    }
    return result;
}



/*!
  Creates an instance of class \a objClass, with parent \a parent and name \a objName
 */
TQString Uic::createObjectInstance( const TQString& objClass, const TQString& parent, const TQString& objName )
{

    if (( objClass.mid( 1 ) == "ComboBox" ) || ( objClass.mid( 2 ) == "ComboBox" )) {
	return objClass + "( false, " + parent + ", \"" + objName + "\" )";
    }
    return objClass + "( " + parent + ", \"" + objName + "\" )";
}

bool Uic::isLayout( const TQString& name ) const
{
    return layoutObjects.contains( name );
}



