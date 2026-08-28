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

#include "domtool.h"

#include <ntqsizepolicy.h>
#include <ntqcolor.h>
#include <ntqcursor.h>
#include <ntqdatetime.h>
#include <ntqrect.h>
#include <ntqsize.h>
#include <ntqfont.h>
#include <ntqdom.h>

/*!
  \class DomTool domtool.h
  \brief Tools for the dom

  A collection of static functions used by Resource (part of the
  designer) and Uic.

*/

/*!
  Returns the contents of property \a name of object \a e as
  variant or the variant passed as \a defValue if the property does
  not exist.

  \sa hasProperty()
*/
TQVariant DomTool::readProperty( const TQDomElement& e, const TQString& name, const TQVariant& defValue, TQString& comment )
{
    TQDomElement n;
    for ( n = e.firstChild().toElement(); !n.isNull(); n = n.nextSibling().toElement() ) {
	if ( n.tagName() == "property" ) {
	    if ( n.attribute( "name" ) != name )
		continue;
	    return elementToVariant( n.firstChild().toElement(), defValue, comment );
	}
    }
    return defValue;
}


/*!
  \overload
 */
TQVariant DomTool::readProperty( const TQDomElement& e, const TQString& name, const TQVariant& defValue )
{
    TQString comment;
    return readProperty( e, name, defValue, comment );
}

/*!
  Returns wheter object \a e defines property \a name or not.

  \sa readProperty()
 */
bool DomTool::hasProperty( const TQDomElement& e, const TQString& name )
{
    TQDomElement n;
    for ( n = e.firstChild().toElement(); !n.isNull(); n = n.nextSibling().toElement() ) {
	if ( n.tagName() == "property" ) {
	    if ( n.attribute( "name" ) != name )
		continue;
	    return true;
	}
    }
    return false;
}

TQStringList DomTool::propertiesOfType( const TQDomElement& e, const TQString& type )
{
    TQStringList result;
    TQDomElement n;
    for ( n = e.firstChild().toElement(); !n.isNull(); n = n.nextSibling().toElement() ) {
	if ( n.tagName() == "property" ) {
	    TQDomElement n2 = n.firstChild().toElement();
	    if ( n2.tagName() == type )
		result += n.attribute( "name" );
	}
    }
    return result;
}


TQVariant DomTool::elementToVariant( const TQDomElement& e, const TQVariant& defValue )
{
    TQString dummy;
    return elementToVariant( e, defValue, dummy );
}

/*!
  Interprets element \a e as variant and returns the result of the interpretation.
 */
TQVariant DomTool::elementToVariant( const TQDomElement& e, const TQVariant& defValue, TQString &comment )
{
    TQVariant v;
    if ( e.tagName() == "rect" ) {
	TQDomElement n3 = e.firstChild().toElement();
	int x = 0, y = 0, w = 0, h = 0;
	while ( !n3.isNull() ) {
	    if ( n3.tagName() == "x" )
		x = n3.firstChild().toText().data().toInt();
	    else if ( n3.tagName() == "y" )
		y = n3.firstChild().toText().data().toInt();
	    else if ( n3.tagName() == "width" )
		w = n3.firstChild().toText().data().toInt();
	    else if ( n3.tagName() == "height" )
		h = n3.firstChild().toText().data().toInt();
	    n3 = n3.nextSibling().toElement();
	}
	v = TQVariant( TQRect( x, y, w, h ) );
    } else if ( e.tagName() == "point" ) {
	TQDomElement n3 = e.firstChild().toElement();
	int x = 0, y = 0;
	while ( !n3.isNull() ) {
	    if ( n3.tagName() == "x" )
		x = n3.firstChild().toText().data().toInt();
	    else if ( n3.tagName() == "y" )
		y = n3.firstChild().toText().data().toInt();
	    n3 = n3.nextSibling().toElement();
	}
	v = TQVariant( TQPoint( x, y ) );
    } else if ( e.tagName() == "size" ) {
	TQDomElement n3 = e.firstChild().toElement();
	int w = 0, h = 0;
	while ( !n3.isNull() ) {
	    if ( n3.tagName() == "width" )
		w = n3.firstChild().toText().data().toInt();
	    else if ( n3.tagName() == "height" )
		h = n3.firstChild().toText().data().toInt();
	    n3 = n3.nextSibling().toElement();
	}
	v = TQVariant( TQSize( w, h ) );
    } else if ( e.tagName() == "color" ) {
	v = TQVariant( readColor( e ) );
    } else if ( e.tagName() == "font" ) {
	TQDomElement n3 = e.firstChild().toElement();
	TQFont f( defValue.toFont()  );
	while ( !n3.isNull() ) {
	    if ( n3.tagName() == "family" )
		f.setFamily( n3.firstChild().toText().data() );
	    else if ( n3.tagName() == "pointsize" )
		f.setPointSize( n3.firstChild().toText().data().toInt() );
	    else if ( n3.tagName() == "bold" )
		f.setBold( n3.firstChild().toText().data().toInt() );
	    else if ( n3.tagName() == "italic" )
		f.setItalic( n3.firstChild().toText().data().toInt() );
	    else if ( n3.tagName() == "underline" )
		f.setUnderline( n3.firstChild().toText().data().toInt() );
	    else if ( n3.tagName() == "strikeout" )
		f.setStrikeOut( n3.firstChild().toText().data().toInt() );
	    n3 = n3.nextSibling().toElement();
	}
	v = TQVariant( f );
    } else if ( e.tagName() == "string" ) {
	v = TQVariant( e.firstChild().toText().data() );
	TQDomElement n = e;
	n = n.nextSibling().toElement();
	if ( n.tagName() == "comment" )
	    comment = n.firstChild().toText().data();
    } else if ( e.tagName() == "cstring" ) {
	v = TQVariant( TQCString( e.firstChild().toText().data() ) );
    } else if ( e.tagName() == "number" ) {
	bool ok = true;
	v = TQVariant( e.firstChild().toText().data().toInt( &ok ) );
	if ( !ok )
	    v = TQVariant( e.firstChild().toText().data().toDouble() );
    } else if ( e.tagName() == "bool" ) {
	TQString t = e.firstChild().toText().data();
	v = TQVariant( t == "true" || t == "1" );
    } else if ( e.tagName() == "pixmap" ) {
	v = TQVariant( e.firstChild().toText().data() );
    } else if ( e.tagName() == "iconset" ) {
	v = TQVariant( e.firstChild().toText().data() );
    } else if ( e.tagName() == "image" ) {
	v = TQVariant( e.firstChild().toText().data() );
    } else if ( e.tagName() == "enum" ) {
	v = TQVariant( e.firstChild().toText().data() );
    } else if ( e.tagName() == "set" ) {
	v = TQVariant( e.firstChild().toText().data() );
    } else if ( e.tagName() == "sizepolicy" ) {
	TQDomElement n3 = e.firstChild().toElement();
	TQSizePolicy sp;
	while ( !n3.isNull() ) {
	    if ( n3.tagName() == "hsizetype" )
		sp.setHorData( (TQSizePolicy::SizeType)n3.firstChild().toText().data().toInt() );
	    else if ( n3.tagName() == "vsizetype" )
		sp.setVerData( (TQSizePolicy::SizeType)n3.firstChild().toText().data().toInt() );
	    else if ( n3.tagName() == "horstretch" )
		sp.setHorStretch( n3.firstChild().toText().data().toInt() );
	    else if ( n3.tagName() == "verstretch" )
		sp.setVerStretch( n3.firstChild().toText().data().toInt() );
	    n3 = n3.nextSibling().toElement();
	}
	v = TQVariant( sp );
    } else if ( e.tagName() == "cursor" ) {
	v = TQVariant( TQCursor( e.firstChild().toText().data().toInt() ) );
    } else if ( e.tagName() == "stringlist" ) {
	TQStringList lst;
	TQDomElement n;
	for ( n = e.firstChild().toElement(); !n.isNull(); n = n.nextSibling().toElement() )
	    lst << n.firstChild().toText().data();
	v = TQVariant( lst );
    } else if ( e.tagName() == "date" ) {
	TQDomElement n3 = e.firstChild().toElement();
	int y, m, d;
	y = m = d = 0;
	while ( !n3.isNull() ) {
	    if ( n3.tagName() == "year" )
		y = n3.firstChild().toText().data().toInt();
	    else if ( n3.tagName() == "month" )
		m = n3.firstChild().toText().data().toInt();
	    else if ( n3.tagName() == "day" )
		d = n3.firstChild().toText().data().toInt();
	    n3 = n3.nextSibling().toElement();
	}
	v = TQVariant( TQDate( y, m, d ) );
    } else if ( e.tagName() == "time" ) {
	TQDomElement n3 = e.firstChild().toElement();
	int h, m, s;
	h = m = s = 0;
	while ( !n3.isNull() ) {
	    if ( n3.tagName() == "hour" )
		h = n3.firstChild().toText().data().toInt();
	    else if ( n3.tagName() == "minute" )
		m = n3.firstChild().toText().data().toInt();
	    else if ( n3.tagName() == "second" )
		s = n3.firstChild().toText().data().toInt();
	    n3 = n3.nextSibling().toElement();
	}
	v = TQVariant( TQTime( h, m, s ) );
    } else if ( e.tagName() == "datetime" ) {
	TQDomElement n3 = e.firstChild().toElement();
	int h, mi, s, y, mo, d ;
	h = mi = s = y = mo = d = 0;
	while ( !n3.isNull() ) {
	    if ( n3.tagName() == "hour" )
		h = n3.firstChild().toText().data().toInt();
	    else if ( n3.tagName() == "minute" )
		mi = n3.firstChild().toText().data().toInt();
	    else if ( n3.tagName() == "second" )
		s = n3.firstChild().toText().data().toInt();
	    else if ( n3.tagName() == "year" )
		y = n3.firstChild().toText().data().toInt();
	    else if ( n3.tagName() == "month" )
		mo = n3.firstChild().toText().data().toInt();
	    else if ( n3.tagName() == "day" )
		d = n3.firstChild().toText().data().toInt();
	    n3 = n3.nextSibling().toElement();
	}
	v = TQVariant( TQDateTime( TQDate( y, mo, d ), TQTime( h, mi, s ) ) );
    }
    return v;
}


/*!  Returns the color which is returned in the dom element \a e.
 */

TQColor DomTool::readColor( const TQDomElement &e )
{
    TQDomElement n = e.firstChild().toElement();
    int r= 0, g = 0, b = 0;
    while ( !n.isNull() ) {
	if ( n.tagName() == "red" )
	    r = n.firstChild().toText().data().toInt();
	else if ( n.tagName() == "green" )
	    g = n.firstChild().toText().data().toInt();
	else if ( n.tagName() == "blue" )
	    b = n.firstChild().toText().data().toInt();
	n = n.nextSibling().toElement();
    }

    return TQColor( r, g, b );
}

/*!
  Returns the contents of attribute \a name of object \a e as
  variant or the variant passed as \a defValue if the attribute does
  not exist.

  \sa hasAttribute()
 */
TQVariant DomTool::readAttribute( const TQDomElement& e, const TQString& name, const TQVariant& defValue, TQString& comment )
{
    TQDomElement n;
    for ( n = e.firstChild().toElement(); !n.isNull(); n = n.nextSibling().toElement() ) {
	if ( n.tagName() == "attribute" ) {
	    if ( n.attribute( "name" ) != name )
		continue;
	    return elementToVariant( n.firstChild().toElement(), defValue, comment );
	}
    }
    return defValue;
}

/*!
  \overload
*/
TQVariant DomTool::readAttribute( const TQDomElement& e, const TQString& name, const TQVariant& defValue )
{
    TQString comment;
    return readAttribute( e, name, defValue, comment );
}

/*!
  Returns wheter object \a e defines attribute \a name or not.

  \sa readAttribute()
 */
bool DomTool::hasAttribute( const TQDomElement& e, const TQString& name )
{
    TQDomElement n;
    for ( n = e.firstChild().toElement(); !n.isNull(); n = n.nextSibling().toElement() ) {
	if ( n.tagName() == "attribute" ) {
	    if ( n.attribute( "name" ) != name )
		continue;
	    return true;
	}
    }
    return false;
}

static bool toBool( const TQString& s )
{
    return s == "true" || s.toInt() != 0;
}

/*!
  Convert TQt 2.x format to TQt 3.0 format if necessary
*/
void DomTool::fixDocument( TQDomDocument& doc )
{
    TQDomElement e;
    TQDomNode n;
    TQDomNodeList nl;
    int i = 0;

    e = doc.firstChild().toElement();
    if ( e.tagName() != "UI" )
	return;

    // latest version, don't do anything
    if ( e.hasAttribute("version") && e.attribute("version").toDouble() > 3.0 )
	return;

    nl = doc.elementsByTagName( "property" );

    // in 3.0, we need to fix a spelling error
    if ( e.hasAttribute("version") && e.attribute("version").toDouble() == 3.0 ) {
	for ( i = 0; i <  (int) nl.length(); i++ ) {
	    TQDomElement el = nl.item(i).toElement();
	    TQString s = el.attribute( "name" );
	    if ( s == "resizeable" ) {
		el.removeAttribute( "name" );
		el.setAttribute( "name", "resizable" );
	    }
	}
	return;
    }


    // in versions smaller than 3.0 we need to change more
    e.setAttribute( "version", 3.0 );

    e.setAttribute("stdsetdef", 1 );
    for ( i = 0; i <  (int) nl.length(); i++ ) {
	e = nl.item(i).toElement();
	TQString name;
	TQDomElement n2 = e.firstChild().toElement();
	if ( n2.tagName() == "name" ) {
	    name = n2.firstChild().toText().data();
	    if ( name == "resizeable" )
		e.setAttribute( "name", "resizable" );
	    else
		e.setAttribute( "name", name );
	    e.removeChild( n2 );
	}
	bool stdset = toBool( e.attribute( "stdset" ) );
	if ( stdset || name == "toolTip" || name == "whatsThis" ||
	     name == "buddy" ||
	     e.parentNode().toElement().tagName() == "item" ||
	     e.parentNode().toElement().tagName() == "spacer" ||
	     e.parentNode().toElement().tagName() == "column"
	     )
	    e.removeAttribute( "stdset" );
	else
	    e.setAttribute( "stdset", 0 );
    }

    nl = doc.elementsByTagName( "attribute" );
    for ( i = 0; i <  (int) nl.length(); i++ ) {
	e = nl.item(i).toElement();
	TQString name;
	TQDomElement n2 = e.firstChild().toElement();
	if ( n2.tagName() == "name" ) {
	    name = n2.firstChild().toText().data();
	    e.setAttribute( "name", name );
	    e.removeChild( n2 );
	}
    }

    nl = doc.elementsByTagName( "image" );
    for ( i = 0; i <  (int) nl.length(); i++ ) {
	e = nl.item(i).toElement();
	TQString name;
	TQDomElement n2 = e.firstChild().toElement();
	if ( n2.tagName() == "name" ) {
	    name = n2.firstChild().toText().data();
	    e.setAttribute( "name", name );
	    e.removeChild( n2 );
	}
    }

    nl = doc.elementsByTagName( "widget" );
    for ( i = 0; i <  (int) nl.length(); i++ ) {
	e = nl.item(i).toElement();
	TQString name;
	TQDomElement n2 = e.firstChild().toElement();
	if ( n2.tagName() == "class" ) {
	    name = n2.firstChild().toText().data();
	    e.setAttribute( "class", name );
	    e.removeChild( n2 );
	}
    }

}

