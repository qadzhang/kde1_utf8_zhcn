/**********************************************************************
** Copyright (C) 1998-2008 Trolltech ASA.  All rights reserved.
**
** This is a utility program for converting tqtfindtr msgfiles to
** qtranslator message files
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
#include <ntqtextcodec.h>
#include <ntqtranslator.h>

#include <stdio.h>
#include <stdlib.h>

static TQString* defaultScope = 0;

bool hasHandle( const TQString& line, const TQString& handle)
{
    return line.left(handle.length()) == handle;
}


TQString extractContents( const TQString& line )
{
    TQString contents;
    if ( line.contains('\"') < 2)
	return contents;
    int pos = 0;
    while ( pos < int(line.length()) && line[pos] != '\"' )
	pos++;
    pos++;
    while ( pos < int(line.length()) && line[pos] != '\"' ) {
	// 0xa5: the yen sign is the Japanese backslash
	if ( line[pos] == '\\' || line[pos] == TQChar(0xa5) ) {
	    pos++;
	    switch (char(line[pos]) ) {
	    case 'n':
		contents += '\n';
		break;
	    case 't':
		contents += '\t';
		break;
	    case 'r':
		contents += '\r';
		break;
	    case 'a':
		contents += '\a';
		break;
	    case 'f':
		contents += '\f';
		break;
	    case 'v':
		contents += '\v';
		break;
	    case 'b':
		contents += '\b';
		break;
	    default:
		contents += char(line[pos]);
		break;
	    }
	}
	else
	    contents += line[pos];
	pos++;
    }
    return contents;
}


void addTranslation( TQTranslator* translator, const TQString& msgid, const TQString& msgstr)
{
    if (!msgid.isNull() && !msgstr.isNull() ) {
	TQString scope = "";
	TQString id = msgid;
	int coloncolon = msgid.find("::");
	if (coloncolon != -1) {
	    scope = msgid.left( coloncolon );
	    id = msgid.right( msgid.length() - scope.length() - 2 );
	}
	else if (defaultScope)
	    scope = *defaultScope;

	if (translator->contains( scope.ascii(), id.ascii() ) ) {
	    tqWarning("Error: \"%s\" already in use", msgid.ascii() );
	}
	else {
	    translator->insert( scope.latin1(), id.latin1(), msgstr );
	}
    }
}



void translate( const TQString& filename, const TQString& qmfile )
{
    TQFile f(filename);
    if ( !f.open( IO_ReadOnly) )
	return;
    TQTranslator* translator = new TQTranslator(0);
    TQTextCodec *codec = 0;
    for (int pass =  0; pass < 2; pass++) {
	f.at(0);
	TQTextStream t( &f );
	TQString line;
	TQString msgid;
	TQString msgstr;
	if ( codec != 0 ) {
	    t.setCodec( codec );
	}
	while ( !t.atEnd() || !line.isEmpty() ) {
	    if (line.isEmpty()) {
		t.skipWhiteSpace();
		line = t.readLine();
	    }
	    if ( hasHandle( line, "msgid") ) {
		msgstr = TQString::null;
		msgid = extractContents( line );
		if (!t.atEnd()) {
		    t.skipWhiteSpace();
		    line = t.readLine();
		}
		else
		    line = TQString::null;
		while ( hasHandle( line, "\"") ) {
		    msgid += extractContents( line );
		    if (!t.atEnd()) {
			t.skipWhiteSpace();
			line = t.readLine();
		    }
		    else
			line = TQString::null;
		}
	    }
	    else if ( hasHandle( line, "msgstr") ) {
		msgstr = extractContents( line );
		if (!t.atEnd()) {
		    t.skipWhiteSpace();
		    line = t.readLine();
		}
		else
		    line = TQString::null;
		while ( hasHandle( line, "\"") ) {
		    msgstr += extractContents( line );
		    if (!t.atEnd()) {
			t.skipWhiteSpace();
			line = t.readLine();
		    }
		    else
			line = TQString::null;
		}
		if ( pass == 1 )
		    addTranslation( translator, msgid, msgstr);

		if ( pass == 0 && msgid.isEmpty() ) {
		    // Check for the encoding.
		    int cpos = msgstr.find( "charset=" );
		    if ( cpos >= 0 ) {
			cpos = cpos + 8; //skip "charset="
			int i = cpos;
			int len = msgstr.length();
			while ( i < len && !msgstr[i].isSpace() )
			    i++;
			TQString charset = msgstr.mid( cpos, i-cpos );
			codec = TQTextCodec::codecForName( charset.ascii() );
			if ( codec ) {
			    tqDebug( "PO file character set: %s. Codec: %s",
				   charset.ascii(), codec->name() );
			} else {
			    tqDebug( "No codec for %s", charset.ascii() );
			}
		    }
		    break;
		}
	    }
	    else
		line = TQString::null;
	}
    }
    f.close();
    translator->save( qmfile );
}


// workaround for BCC problem, ntqtranslator.h includes ntqwindowdefs.h via ntqobject.h, see NEEDS_QMAIN
#if defined(main)
#undef main
#endif

int main( int argc, char* argv[] )
{

    int infile = 1;
    if (argc > 1) {
	if ( TQString("-scope") == argv[1] ) {
	    defaultScope = new TQString(argv[2]);
	    infile += 2;
	}
    }

    if ( argc <= infile ) {
	printf("usage: %s [-scope default] infile [outfile]\n", argv[0]);
	exit(1);
    }

    translate(argv[infile], argc > infile+1 ? argv[infile+1] : "tr.qm");
    return 0;
}
