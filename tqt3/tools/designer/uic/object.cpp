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
#include "domtool.h"
#include <ntqregexp.h>
#include <ntqsizepolicy.h>
#include <ntqstringlist.h>
#define NO_STATIC_COLORS
#include <globaldefs.h>
#include "../interfaces/widgetinterface.h"
#include "../shared/widgetdatabase.h"

/*!
  Creates a declaration for the object given in \a e.

  Children are not traversed recursively.

  \sa createObjectImpl()
 */
void Uic::createObjectDecl( const TQDomElement& e )
{
    if ( e.tagName() == "vbox" ) {
	out << "    TQVBoxLayout* " << registerObject(getLayoutName(e) ) << ";" << endl;
    } else if ( e.tagName() == "hbox" ) {
	out << "    TQHBoxLayout* " << registerObject(getLayoutName(e) ) << ";" << endl;
    } else if ( e.tagName() == "grid" ) {
	out << "    TQGridLayout* " << registerObject(getLayoutName(e) ) << ";" << endl;
    } else {
	TQString objClass = getClassName( e );
	if ( objClass.isEmpty() )
	    return;
	TQString objName = getObjectName( e );
	if ( objName.isEmpty() )
	    return;
	// ignore TQLayoutWidgets
	if ( objClass == "TQLayoutWidget" )
	    return;
	// register the object and unify its name
	objName = registerObject( objName );
	if ( objClass == "Line" )
	    objClass = "TQFrame";
	else if (objClass == "Spacer")
	    objClass = "TQSpacerItem";
	out << "    " << objClass << "* " << objName << ";" << endl;
    }
}


/*!
  Creates an implementation for the object given in \a e.

  Traverses recursively over all children.

  Returns the name of the generated child object.

  \sa createObjectDecl()
 */

static bool createdCentralWidget = false;

TQString Uic::createObjectImpl( const TQDomElement &e, const TQString& parentClass, const TQString& par, const TQString& layout )
{
    TQString parent( par );
    if ( parent == "this" && isMainWindow ) {
	if ( !createdCentralWidget )
	    out << indent << "setCentralWidget( new TQWidget( this, \"qt_central_widget\" ) );" << endl;
	createdCentralWidget = true;
	parent = "centralWidget()";
    }
    TQDomElement n;
    TQString objClass, objName;
    int numItems = 0;
    int numColumns = 0;
    int numRows = 0;

    if ( layouts.contains( e.tagName() ) )
	return createLayoutImpl( e, parentClass, parent, layout );

    objClass = getClassName( e );
    if ( objClass.isEmpty() )
	return objName;
    objName = getObjectName( e );

    TQString definedName = objName;
    bool isTmpObject = objName.isEmpty() || objClass == "TQLayoutWidget";
    if ( isTmpObject ) {
	if (( objClass[0] == 'T' ) && ( objClass[1] == 'Q' ))
	    objName = objClass.mid(2);
	else
	    objName = objClass.lower();
	objName.prepend( "private" );
    }

    bool isLine = objClass == "Line";
    if ( isLine )
	objClass = "TQFrame";

    out << endl;
    if ( objClass == "TQLayoutWidget" ) {
	if ( layout.isEmpty() ) {
	    // register the object and unify its name
	    objName = registerObject( objName );
	    out << "    TQWidget* " << objName << " = new TQWidget( " << parent << ", \"" << definedName << "\" );" << endl;
	} else {
	    // the layout widget is not necessary, hide it by creating its child in the parent
	    TQString result;
	    for ( n = e.firstChild().toElement(); !n.isNull(); n = n.nextSibling().toElement() ) {
		if (tags.contains( n.tagName() ) )
		    result = createObjectImpl( n, parentClass, parent, layout );
	    }
	    return result;
	}
    } else if ( objClass != "TQToolBar" && objClass != "TQMenuBar" ) {
	// register the object and unify its name
	objName = registerObject( objName );
	out << "    ";
	if ( isTmpObject )
	    out << objClass << "* ";
	out << objName << " = new " << createObjectInstance( objClass, parent, objName ) << ";" << endl;
    }

    if ( objClass == "TQAxWidget" ) {
	TQString controlId;
	for ( n = e.firstChild().toElement(); !n.isNull(); n = n.nextSibling().toElement() ) {
	    if ( n.tagName() == "property" && n.attribute( "name" ) == "control" ) {
		controlId = n.firstChild().toElement().text();
	    }
	}
	out << "    ";
	out << objName << "->setControl(\"" << controlId << "\");" << endl;
    }

    lastItem = "0";
    // set the properties and insert items
    bool hadFrameShadow = false;
    for ( n = e.firstChild().toElement(); !n.isNull(); n = n.nextSibling().toElement() ) {
	if ( n.tagName() == "property" ) {
	    bool stdset = stdsetdef;
	    if ( n.hasAttribute( "stdset" ) )
		stdset = toBool( n.attribute( "stdset" ) );
	    TQString prop = n.attribute( "name" );
	    if ( prop == "database" )
		continue;
	    TQString value = setObjectProperty( objClass, objName, prop, n.firstChild().toElement(), stdset );
	    if ( value.isEmpty() )
		continue;
	    if ( prop == "name" )
		continue;
	    if ( isLine && prop == "frameShadow" )
		hadFrameShadow = true;
	    if ( prop == "buddy" && value.startsWith("\"") && value.endsWith("\"") ) {
		buddies << Buddy( objName, value.mid(1, value.length() - 2 ) );
		continue;
	    }
	    if ( isLine && prop == "orientation" ) {
		prop = "frameShape";
		if ( value.right(10) == "Horizontal" )
		    value = "TQFrame::HLine";
		else
		    value = "TQFrame::VLine";
		if ( !hadFrameShadow ) {
		    prop = "frameStyle";
		    value += " | TQFrame::Sunken";
		}
	    }
	    if ( prop == "buttonGroupId" ) {
		if ( parentClass == "TQButtonGroup" )
		    out << indent << parent << "->insert( " << objName << ", " << value << " );" << endl;
		continue;
	    }
	    if ( prop == "frameworkCode" )
		continue;
	    if ( objClass == "TQMultiLineEdit" &&
		 TQRegExp("echoMode|hMargin|maxLength|maxLines|undoEnabled").exactMatch(prop) )
		continue;

	    TQString call = objName + "->";
	    if ( stdset ) {
		call += mkStdSet( prop ) + "( ";
	    } else {
		call += "setProperty( \"" + prop + "\", ";
	    }
	    if ( prop == "accel" )
		call += "TQKeySequence( " + value + " ) );";
	    else
		call += value + " );";

	    if ( n.firstChild().toElement().tagName() == "string" ||
		 prop == "currentItem" ) {
		trout << indent << call << endl;
	    } else {
		out << indent << call << endl;
	    }
	} else if ( n.tagName() == "item" ) {
	    TQString call;
	    TQString value;

	    if ( objClass.contains( "ListBox" ) ) {
		call = createListBoxItemImpl( n, objName );
		if ( !call.isEmpty() ) {
		    if ( numItems == 0 )
			trout << indent << objName << "->clear();" << endl;
		    trout << indent << call << endl;
		}
	    } else if ( objClass.contains( "ComboBox" ) ) {
		call = createListBoxItemImpl( n, objName, &value );
		if ( !call.isEmpty() ) {
		    if ( numItems == 0 )
			trout << indent << objName << "->clear();" << endl;
		    trout << indent << call << endl;
		}
	    } else if ( objClass.contains( "IconView" ) ) {
		call = createIconViewItemImpl( n, objName );
		if ( !call.isEmpty() ) {
		    if ( numItems == 0 )
			trout << indent << objName << "->clear();" << endl;
		    trout << indent << call << endl;
		}
	    } else if ( objClass.contains( "ListView" ) ) {
		call = createListViewItemImpl( n, objName, TQString::null );
		if ( !call.isEmpty() ) {
		    if ( numItems == 0 )
			trout << indent << objName << "->clear();" << endl;
		    trout << call << endl;
		}
	    }
	    if ( !call.isEmpty() )
		numItems++;
	} else if ( n.tagName() == "column" || n.tagName() == "row" ) {
	    TQString call;
	    TQString value;

	    if ( objClass.contains( "ListView" ) ) {
		call = createListViewColumnImpl( n, objName, &value );
		if ( !call.isEmpty() ) {
		    out << call;
		    trout << indent << objName << "->header()->setLabel( "
			  << numColumns++ << ", " << value << " );\n";
		}
	    } else if ( objClass ==  "TQTable" || objClass == "TQDataTable" ) {
		bool isCols = ( n.tagName() == "column" );
		call = createTableRowColumnImpl( n, objName, &value );
		if ( !call.isEmpty() ) {
		    out << call;
		    trout << indent << objName << "->"
			  << ( isCols ? "horizontalHeader" : "verticalHeader" )
			  << "()->setLabel( "
			  << ( isCols ? numColumns++ : numRows++ )
			  << ", " << value << " );\n";
		}
	    }
	}
    }

    // create all children, some widgets have special requirements

    if ( objClass == "TQTabWidget" ) {
	for ( n = e.firstChild().toElement(); !n.isNull(); n = n.nextSibling().toElement() ) {
	    if ( tags.contains( n.tagName()  ) ) {
		TQString page = createObjectImpl( n, objClass, objName );
		TQString comment;
		TQString label = DomTool::readAttribute( n, "title", "", comment ).toString();
		out << indent << objName << "->insertTab( " << page << ", TQString::fromLatin1(\"\") );" << endl;
		trout << indent << objName << "->changeTab( " << page << ", "
		      << trcall( label, comment ) << " );" << endl;
	    }
	}
    } else if ( objClass == "TQWidgetStack" ) {
	for ( n = e.firstChild().toElement(); !n.isNull(); n = n.nextSibling().toElement() ) {
	    if ( tags.contains( n.tagName()  ) ) {
		TQString page = createObjectImpl( n, objClass, objName );
		int id = DomTool::readAttribute( n, "id", "" ).toInt();
		out << indent << objName << "->addWidget( " << page << ", " << id << " );" << endl;
	    }
	}
    } else if ( objClass == "TQToolBox" ) {
	for ( n = e.firstChild().toElement(); !n.isNull(); n = n.nextSibling().toElement() ) {
	    if ( tags.contains( n.tagName()  ) ) {
		TQString page = createObjectImpl( n, objClass, objName );
		TQString comment;
		TQString label = DomTool::readAttribute( n, "label", "", comment ).toString();
		out << indent << objName << "->addItem( " << page << ", TQString::fromLatin1(\"\") );" << endl;
		trout << indent << objName << "->setItemLabel( " << objName 
		      << "->indexOf(" << page << "), " << trcall( label, comment ) 
		      << " );" << endl;
	    }
	}
     } else if ( objClass != "TQToolBar" && objClass != "TQMenuBar" ) { // standard widgets
	 WidgetInterface *iface = 0;
	 widgetManager()->queryInterface( objClass, &iface );
#ifdef QT_CONTAINER_CUSTOM_WIDGETS
	 int id = WidgetDatabase::idFromClassName( objClass );
	 if ( WidgetDatabase::isContainer( id ) && WidgetDatabase::isCustomPluginWidget( id ) && iface ) {
	     TQWidgetContainerInterfacePrivate *iface2 = 0;
	     iface->queryInterface( IID_QWidgetContainer, (TQUnknownInterface**)&iface2 );
	     if ( iface2 ) {
		 bool supportsPages = iface2->supportsPages( objClass );
		 for ( n = e.firstChild().toElement(); !n.isNull(); n = n.nextSibling().toElement() ) {
		     if ( tags.contains( n.tagName()  ) ) {
			 if ( supportsPages ) {
			     TQString page = createObjectImpl( n, objClass, objName );
			     TQString comment;
			     TQString label = DomTool::readAttribute( n, "label", "", comment ).toString();
			     out << indent << iface2->createCode( objClass, objName, page, label ) << endl;
			 } else {
			     createObjectImpl( n, objClass, objName );
			 }
		     }
		 }
		 iface2->release();
	     }
	     iface->release();
	 } else {
#endif
	     for ( n = e.firstChild().toElement(); !n.isNull(); n = n.nextSibling().toElement() ) {
		 if ( tags.contains( n.tagName() ) )
		     createObjectImpl( n, objClass, objName );
	     }
#ifdef QT_CONTAINER_CUSTOM_WIDGETS
	 }
#endif
     }

    return objName;
}

/*!
  Creates declarations for spacer items that are children of \a e.

  \sa createObjectDecl()
*/

void Uic::createSpacerDecl( const TQDomElement &e )
{
    for ( TQDomElement n = e.firstChild().toElement();
	  !n.isNull(); n = n.nextSibling().toElement() )
	if ( n.tagName() == "spacer" )
	    out << "    TQSpacerItem* " << registerObject(getObjectName(n)) << ";" << endl;
}

/*!
  Creates a set-call for property \a exclusiveProp of the object
  given in \a e.

  If the object does not have this property, the function does nothing.

  Exclusive properties are used to generate the implementation of
  application font or palette change handlers in createFormImpl().

 */
void Uic::createExclusiveProperty( const TQDomElement & e, const TQString& exclusiveProp )
{
    TQDomElement n;
    TQString objClass = getClassName( e );
    if ( objClass.isEmpty() )
	return;
    TQString objName = getObjectName( e );
#if 0 // it's not clear whether this check should be here or not
    if ( objName.isEmpty() )
	return;
#endif
    for ( n = e.firstChild().toElement(); !n.isNull(); n = n.nextSibling().toElement() ) {
	if ( n.tagName() == "property" ) {
	    bool stdset = stdsetdef;
	    if ( n.hasAttribute( "stdset" ) )
		stdset = toBool( n.attribute( "stdset" ) );
	    TQString prop = n.attribute( "name" );
	    if ( prop != exclusiveProp )
		continue;
	    TQString value = setObjectProperty( objClass, objName, prop, n.firstChild().toElement(), stdset );
	    if ( value.isEmpty() )
		continue;
	    // we assume the property isn't of type 'string'
	    out << '\t' << objName << "->setProperty( \"" << prop << "\", " << value << " );" << endl;
	}
    }
}


/*!  Attention: this function has to be in sync with
  Resource::saveProperty() and DomTool::elementToVariant. If you
  change one, change all.
 */
TQString Uic::setObjectProperty( const TQString& objClass, const TQString& obj, const TQString &prop, const TQDomElement &e, bool stdset )
{
    TQString v;
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
	v = "TQRect( %1, %2, %3, %4 )";
	v = v.arg(x).arg(y).arg(w).arg(h);

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
	v = "TQPoint( %1, %2 )";
	v = v.arg(x).arg(y);
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
	v = "TQSize( %1, %2 )";
	v = v.arg(w).arg(h);
    } else if ( e.tagName() == "color" ) {
	TQDomElement n3 = e.firstChild().toElement();
	int r = 0, g = 0, b = 0;
	while ( !n3.isNull() ) {
	    if ( n3.tagName() == "red" )
		r = n3.firstChild().toText().data().toInt();
	    else if ( n3.tagName() == "green" )
		g = n3.firstChild().toText().data().toInt();
	    else if ( n3.tagName() == "blue" )
		b = n3.firstChild().toText().data().toInt();
	    n3 = n3.nextSibling().toElement();
	}
	v = "TQColor( %1, %2, %3 )";
	v = v.arg(r).arg(g).arg(b);
    } else if ( e.tagName() == "font" ) {
	TQDomElement n3 = e.firstChild().toElement();
	TQString attrname = e.parentNode().toElement().attribute( "name", "font" );
	TQString fontname;
	if ( !obj.isEmpty() ) {
	    fontname = registerObject( obj + "_" + attrname );
	    out << indent << "TQFont "  << fontname << "(  " << obj << "->font() );" << endl;
	} else {
	    fontname = registerObject( "f" );
	    out << indent << "TQFont "  << fontname << "( font() );" << endl;
	}
	while ( !n3.isNull() ) {
	    if ( n3.tagName() == "family" )
		out << indent << fontname << ".setFamily( \"" << n3.firstChild().toText().data() << "\" );" << endl;
	    else if ( n3.tagName() == "pointsize" )
		out << indent << fontname << ".setPointSize( " << n3.firstChild().toText().data() << " );" << endl;
	    else if ( n3.tagName() == "bold" )
		out << indent << fontname << ".setBold( " << mkBool( n3.firstChild().toText().data() ) << " );" << endl;
	    else if ( n3.tagName() == "italic" )
		out << indent << fontname << ".setItalic( " << mkBool( n3.firstChild().toText().data() ) << " );" << endl;
	    else if ( n3.tagName() == "underline" )
		out << indent << fontname << ".setUnderline( " << mkBool( n3.firstChild().toText().data() ) << " );" << endl;
	    else if ( n3.tagName() == "strikeout" )
		out << indent << fontname << ".setStrikeOut( " << mkBool( n3.firstChild().toText().data() ) << " );" << endl;
	    n3 = n3.nextSibling().toElement();
	}

	if ( prop == "font" ) {
	    if ( !obj.isEmpty() )
		out << indent << obj << "->setFont( " << fontname << " ); " << endl;
	    else
		out << indent << "setFont( " << fontname << " ); " << endl;
	} else {
	    v = fontname;
	}
    } else if ( e.tagName() == "string" ) {
	TQString txt = e.firstChild().toText().data();
	TQString com = getComment( e.parentNode() );

	if ( prop == "toolTip" && objClass != "TQAction" && objClass != "TQActionGroup" ) {
	    if ( !obj.isEmpty() )
		trout << indent << "TQToolTip::add( " << obj << ", "
		      << trcall( txt, com ) << " );" << endl;
	    else
		trout << indent << "TQToolTip::add( this, "
		      << trcall( txt, com ) << " );" << endl;
	} else if ( prop == "whatsThis" && objClass != "TQAction" && objClass != "TQActionGroup" ) {
	    if ( !obj.isEmpty() )
		trout << indent << "TQWhatsThis::add( " << obj << ", "
		      << trcall( txt, com ) << " );" << endl;
	    else
		trout << indent << "TQWhatsThis::add( this, "
		      << trcall( txt, com ) << " );" << endl;
	} else {
	    v = trcall( txt, com );
	}
    } else if ( e.tagName() == "cstring" ) {
	    v = "\"%1\"";
	    v = v.arg( e.firstChild().toText().data() );
    } else if ( e.tagName() == "number" ) {
	v = "%1";
	v = v.arg( e.firstChild().toText().data() );
    } else if ( e.tagName() == "bool" ) {
	if ( stdset )
	    v = "%1";
	else
	    v = "TQVariant( %1 )";
	v = v.arg( mkBool( e.firstChild().toText().data() ) );
    } else if ( e.tagName() == "pixmap" ) {
	v = e.firstChild().toText().data();
	if ( !pixmapLoaderFunction.isEmpty() ) {
	    v.prepend( pixmapLoaderFunction + "( " + TQString( externPixmaps ? "\"" : "" ) );
	    v.append( TQString( externPixmaps ? "\"" : "" ) + " )" );
	}
    } else if ( e.tagName() == "iconset" ) {
	v = "TQIconSet( %1 )";
	TQString s = e.firstChild().toText().data();
	if ( !pixmapLoaderFunction.isEmpty() ) {
	    s.prepend( pixmapLoaderFunction + "( " + TQString( externPixmaps ? "\"" : "" ) );
	    s.append( TQString( externPixmaps ? "\"" : "" ) + " )" );
	}
	v = v.arg( s );
    } else if ( e.tagName() == "image" ) {
	v = e.firstChild().toText().data() + ".convertToImage()";
    } else if ( e.tagName() == "enum" ) {
	if ( stdset )
	    v = "%1::%2";
	else
	    v = "\"%1\"";
	TQString oc = objClass;
	TQString ev = e.firstChild().toText().data();
	if ( oc == "TQListView" && ev == "Manual" ) // #### workaround, rename TQListView::Manual in 4.0
	    oc = "TQScrollView";
	if ( stdset )
	    v = v.arg( oc ).arg( ev );
	else
	    v = v.arg( ev );
    } else if ( e.tagName() == "set" ) {
	TQString keys( e.firstChild().toText().data() );
	TQStringList lst = TQStringList::split( '|', keys );
	v = "int( ";
	TQStringList::Iterator it = lst.begin();
	while ( it != lst.end() ) {
	    v += objClass + "::" + *it;
	    if ( it != lst.fromLast() )
		v += " | ";
	    ++it;
	}
	v += " )";
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
	TQString tmp;
	if ( !obj.isEmpty() )
	    tmp = obj + "->";
	v = "TQSizePolicy( (TQSizePolicy::SizeType)%1, (TQSizePolicy::SizeType)%2, %3, %4, " + tmp + "sizePolicy().hasHeightForWidth() )";
	v = v.arg( (int)sp.horData() ).arg( (int)sp.verData() ).arg( sp.horStretch() ).arg( sp.verStretch() );
    } else if ( e.tagName() == "palette" ) {
	TQPalette pal;
	bool no_pixmaps = e.elementsByTagName( "pixmap" ).count() == 0;
	TQDomElement n;
	if ( no_pixmaps ) {
	    n = e.firstChild().toElement();
	    while ( !n.isNull() ) {
		TQColorGroup cg;
		if ( n.tagName() == "active" ) {
		    cg = loadColorGroup( n );
		    pal.setActive( cg );
		} else if ( n.tagName() == "inactive" ) {
		    cg = loadColorGroup( n );
		    pal.setInactive( cg );
		} else if ( n.tagName() == "disabled" ) {
		    cg = loadColorGroup( n );
		    pal.setDisabled( cg );
		}
		n = n.nextSibling().toElement();
	    }
	}
	if ( no_pixmaps && pal == TQPalette( pal.active().button(), pal.active().background() ) ) {
	    v = "TQPalette( TQColor( %1, %2, %3 ), TQColor( %1, %2, %3 ) )";
	    v = v.arg( pal.active().button().red() ).arg( pal.active().button().green() ).arg( pal.active().button().blue() );
	    v = v.arg( pal.active().background().red() ).arg( pal.active().background().green() ).arg( pal.active().background().blue() );
	} else {
	    TQString palette = "pal";
	    if ( !pal_used ) {
		out << indent << "TQPalette " << palette << ";" << endl;
		pal_used = true;
	    }
	    TQString cg = "cg";
	    if ( !cg_used ) {
		out << indent << "TQColorGroup " << cg << ";" << endl;
		cg_used = true;
	    }
	    n = e.firstChild().toElement();
	    while ( !n.isNull() && n.tagName() != "active" )
		n = n.nextSibling().toElement();
	    createColorGroupImpl( cg, n );
	    out << indent << palette << ".setActive( " << cg << " );" << endl;

	    n = e.firstChild().toElement();
	    while ( !n.isNull() && n.tagName() != "inactive" )
		n = n.nextSibling().toElement();
	    createColorGroupImpl( cg, n );
	    out << indent << palette << ".setInactive( " << cg << " );" << endl;

	    n = e.firstChild().toElement();
	    while ( !n.isNull() && n.tagName() != "disabled" )
		n = n.nextSibling().toElement();
	    createColorGroupImpl( cg, n );
	    out << indent << palette << ".setDisabled( " << cg << " );" << endl;
	    v = palette;
	}
    } else if ( e.tagName() == "cursor" ) {
	v = "TQCursor( %1 )";
	v = v.arg( e.firstChild().toText().data() );
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
	v = "TQDate( %1, %2, %3 )";
	v = v.arg(y).arg(m).arg(d);
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
	v = "TQTime( %1, %2, %3 )";
	v = v.arg(h).arg(m).arg(s);
    } else if ( e.tagName() == "datetime" ) {
	TQDomElement n3 = e.firstChild().toElement();
	int h, mi, s, y, mo, d;
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
	v = "TQDateTime( TQDate( %1, %2, %3 ), TQTime( %4, %5, %6 ) )";
	v = v.arg(y).arg(mo).arg(d).arg(h).arg(mi).arg(s);
    } else if ( e.tagName() == "stringlist" ) {
	TQStringList l;
	TQDomElement n3 = e.firstChild().toElement();
	TQString listname = "l";
	if ( !obj.isEmpty() ) {
	    listname = obj + "_stringlist";
	    listname = registerObject( listname );
	    out << indent << "TQStringList "  << listname << ";" << endl;
	} else {
	    listname = registerObject( listname );
	    out << indent << "TQStringList "  << listname << ";" << endl;
	}
	while ( !n3.isNull() ) {
	    if ( n3.tagName() == "string" )
		out << indent << listname << " << \"" << n3.firstChild().toText().data().simplifyWhiteSpace() << "\";" << endl;
	    n3 = n3.nextSibling().toElement();
	}
	v = listname;
    }
    return v;
}

/*! Extracts a named object property from \a e.
 */
TQDomElement Uic::getObjectProperty( const TQDomElement& e, const TQString& name )
{
    TQDomElement n;
    for ( n = e.firstChild().toElement();
	  !n.isNull();
	  n = n.nextSibling().toElement() ) {
	if ( n.tagName() == "property"  && n.toElement().attribute("name") == name )
	    return n;
    }
    return n;
}
