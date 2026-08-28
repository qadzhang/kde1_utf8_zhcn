/**********************************************************************
** Copyright (C) 2005-2008 Trolltech ASA.  All rights reserved.
**
** This file is part of TQt GUI Designer.
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

#include <ntqfile.h>
#include <ntqtextstream.h>
#include <ntqvaluelist.h>
#include <ntqstrlist.h>
#include <ntqmetaobject.h>
#include <ntqapplication.h>

// STEP1: Include header files of the widgets for which a description
// should be created here. If you have a widget which is defined in
// the file mycustomwidget.h in /home/joedeveloper/src, write here
//
// #include "/home/joedeveloper/src/mycustomwidget.h"
//
// Now go on to STEP2!

// -----------------------------

struct Widget
{
    TQWidget *w; // the widget
    TQString include; // header file
    TQString location; // "global" for include <...> or "local" include "..."
};

static TQString makeIndent( int indent )
{
    TQString s;
    s.fill( ' ', indent * 4 );
    return s;
}

static TQString entitize( const TQString &s )
{
    TQString s2 = s;
    s2 = s2.replace( "\"", "&quot;" );
    s2 = s2.replace( "&", "&amp;" );
    s2 = s2.replace( ">", "&gt;" );
    s2 = s2.replace( "<", "&lt;" );
    s2 = s2.replace( "'", "&apos;" );
    return s2;
}

static TQString convert_type( const TQString &s )
{
    TQString str( s );
    if (( str[ 0 ] == 'T' ) && ( str[ 1 ] == 'Q' ))
	str.remove( 0, 2 );
    str[ 0 ] = str[ 0 ].upper();
    return str;
}

static void createDescription( const TQValueList<Widget> &l, TQTextStream &ts )
{
    int indent = 0;
    ts << "<!DOCTYPE CW><CW>" << endl;
    ts << makeIndent( indent ) << "<customwidgets>" << endl;
    indent++;

    for ( TQValueList<Widget>::ConstIterator it = l.begin(); it != l.end(); ++it ) {
	Widget w = *it;
	ts << makeIndent( indent ) << "<customwidget>" << endl;
	indent++;
	ts << makeIndent( indent ) << "<class>" << w.w->className() << "</class>" << endl;
	ts << makeIndent( indent ) << "<header location=\"" << w.location << "\">" << w.include << "</header>" << endl;
	ts << makeIndent( indent ) << "<sizehint>" << endl;
	indent++;
	ts << makeIndent( indent ) << "<width>" << w.w->sizeHint().width() << "</width>" << endl;
	ts << makeIndent( indent ) << "<height>" << w.w->sizeHint().height() << "</height>" << endl;
	indent--;
	ts << makeIndent( indent ) << "</sizehint>" << endl;
	ts << makeIndent( indent ) << "<container>" << ( w.w->inherits( "TQGroupBox" ) || w.w->inherits( "TQWidgetStack" ) ) << "</container>" << endl;
	ts << makeIndent( indent ) << "<sizepolicy>" << endl;
	indent++;
	ts << makeIndent( indent ) << "<hordata>" << (int)w.w->sizePolicy().horData() << "</hordata>" << endl;
	ts << makeIndent( indent ) << "<verdata>" << (int)w.w->sizePolicy().verData() << "</verdata>" << endl;
	indent--;
	ts << makeIndent( indent ) << "</sizepolicy>" << endl;
	
	TQStrList sigs = w.w->metaObject()->signalNames( true );
	if ( !sigs.isEmpty() ) {
	    for ( int i = 0; i < (int)sigs.count(); ++i )
		ts << makeIndent( indent ) << "<signal>" << entitize( sigs.at( i ) ) << "</signal>" << endl;
	}
	TQStrList slts = w.w->metaObject()->slotNames( true );
	if ( !slts.isEmpty() ) {
	    for ( int i = 0; i < (int)slts.count(); ++i ) {
		TQMetaData::Access data = w.w->metaObject()->slot( i, true )->access;
		if ( data == TQMetaData::Private )
		    continue;
		ts << makeIndent( indent ) << "<slot access=\""
		   << ( data == TQMetaData::Protected ? "protected" : "public" )
		   << "\">" << entitize( slts.at( i ) ) << "</slot>" << endl;
	    }
	}	
	TQStrList props = w.w->metaObject()->propertyNames( true );
	if ( !props.isEmpty() ) {
	    for ( int i = 0; i < (int)props.count(); ++i ) {
		const TQMetaProperty *p = w.w->metaObject()->
					 property( w.w->metaObject()->
						   findProperty( props.at( i ), true ), true );
		if ( !p )
		    continue;
		if ( !p->writable() || !p->designable( w.w ) )
		    continue;
		ts << makeIndent( indent ) << "<property type=\"" << convert_type( p->type() ) << "\">" << entitize( p->name() ) << "</property>" << endl;
	    }
	}
	indent--;
	ts << makeIndent( indent ) << "</customwidget>" << endl;
    }

    indent--;
    ts << makeIndent( indent ) << "</customwidgets>" << endl;
    ts << "</CW>" << endl;
}

int main( int argc, char **argv )
{
    if ( argc < 2 )
	return -1;
    TQString fn = argv[1];
    TQFile f( fn );
    if ( !f.open( IO_WriteOnly ) )
	return -1;
    TQTextStream ts( &f );
    TQApplication a( argc, argv );

    TQValueList<Widget> wl;

    // STEP2: Instantiate all widgets for which a description should
    // be created here and add them to the list wl. If your custom widget
    // is e.g. called MyCustomWidget you would write here
    //
    // Widget w;
    // w.w = new MyCustomWidget( 0, 0 );
    // w.include = "mycustomwidget.h";
    // w.location = "global";
    // wl.append( w );
    //
    // After that compile the program, link it with your custom widget
    // (library or object file) and run it like this:
    // (unix): ./tqtcreatecw mywidgets.cw
    // (win32): tqtcreatecw mywidgets.cw
    //
    // After that you can import this description file into the TQt
    // Designer using the Custom-Widget Dialog (See
    // Tools->Custom->Edit Custom Widgets... in the TQt Designer)
    // and use these custom widget there in your forms.



    // ----------------------------------------------

    createDescription( wl, ts );
    f.close();
    return 0;
}
