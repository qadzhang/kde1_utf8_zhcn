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

/*!
  Creates a declaration ( headerfile ) for a subclass \a subClass
  of the form given in \a e

  \sa createSubImpl()
 */
void Uic::createSubDecl( const TQDomElement &e, const TQString& subClass )
{
    TQDomElement n;
    TQDomNodeList nl;
    int i;

    TQString objClass = getClassName( e );
    if ( objClass.isEmpty() )
	return;

    out << "class " << subClass << " : public " << nameOfClass << endl;
    out << "{" << endl;

/* tmake ignore TQ_OBJECT */
    out << "    TQ_OBJECT" << endl;
    out << endl;
    out << "public:" << endl;

    // constructor
    if ( objClass == "TQDialog" || objClass == "TQWizard" ) {
	out << "    " << subClass << "( TQWidget* parent = 0, const char* name = 0, bool modal = false, WFlags fl = 0 );" << endl;
    } else { // standard TQWidget
	out << "    " << subClass << "( TQWidget* parent = 0, const char* name = 0, WFlags fl = 0 );" << endl;
    }

    // destructor
    out << "    ~" << subClass << "();" << endl;
    out << endl;

    // find additional functions
    TQStringList publicSlots, protectedSlots, privateSlots;
    TQStringList publicSlotTypes, protectedSlotTypes, privateSlotTypes;
    TQStringList publicSlotSpecifier, protectedSlotSpecifier, privateSlotSpecifier;
    TQStringList publicFuncts, protectedFuncts, privateFuncts;
    TQStringList publicFunctRetTyp, protectedFunctRetTyp, privateFunctRetTyp;
    TQStringList publicFunctSpec, protectedFunctSpec, privateFunctSpec;

    nl = e.parentNode().toElement().elementsByTagName( "slot" );
    for ( i = 0; i < (int) nl.length(); i++ ) {
	n = nl.item(i).toElement();
	if ( n.parentNode().toElement().tagName() != "slots"
	     && n.parentNode().toElement().tagName() != "connections" )
	    continue;
	if ( n.attribute( "language", "C++" ) != "C++" )
	    continue;
	TQString returnType = n.attribute( "returnType", "void" );
	TQString functionName = n.firstChild().toText().data().stripWhiteSpace();
	if ( functionName.endsWith( ";" ) )
	    functionName = functionName.left( functionName.length() - 1 );
	TQString specifier = n.attribute( "specifier" );
	TQString access = n.attribute( "access" );
	if ( access == "protected" ) {
	    protectedSlots += functionName;
	    protectedSlotTypes += returnType;
	    protectedSlotSpecifier += specifier;
	} else if ( access == "private" ) {
	    privateSlots += functionName;
	    privateSlotTypes += returnType;
	    privateSlotSpecifier += specifier;
	} else {
	    publicSlots += functionName;
	    publicSlotTypes += returnType;
	    publicSlotSpecifier += specifier;
	}
    }

    nl = e.parentNode().toElement().elementsByTagName( "function" );
    for ( i = 0; i < (int) nl.length(); i++ ) {
	n = nl.item(i).toElement();
	if ( n.parentNode().toElement().tagName() != "functions" )
	    continue;
	if ( n.attribute( "language", "C++" ) != "C++" )
	    continue;
	TQString returnType = n.attribute( "returnType", "void" );
	TQString functionName = n.firstChild().toText().data().stripWhiteSpace();
	if ( functionName.endsWith( ";" ) )
	    functionName = functionName.left( functionName.length() - 1 );
	TQString specifier = n.attribute( "specifier" );
	TQString access = n.attribute( "access" );
	if ( access == "protected" ) {
	    protectedFuncts += functionName;
	    protectedFunctRetTyp += returnType;
	    protectedFunctSpec += specifier;
	} else if ( access == "private" ) {
	    privateFuncts += functionName;
	    privateFunctRetTyp += returnType;
	    privateFunctSpec += specifier;
	} else {
	    publicFuncts += functionName;
	    publicFunctRetTyp += returnType;
	    publicFunctSpec += specifier;
	}
    }

    if ( !publicFuncts.isEmpty() )
	writeFunctionsSubDecl( publicFuncts, publicFunctRetTyp, publicFunctSpec );

    // create public additional slots
    if ( !publicSlots.isEmpty() ) {
	out << "public slots:" << endl;
	writeFunctionsSubDecl( publicSlots, publicSlotTypes, publicSlotSpecifier );
    }

    if ( !protectedFuncts.isEmpty() ) {
	out << "protected:" << endl;
	writeFunctionsSubDecl( protectedFuncts, protectedFunctRetTyp, protectedFunctSpec );
    }

    // create protected additional slots
    if ( !protectedSlots.isEmpty() ) {
	out << "protected slots:" << endl;
	writeFunctionsSubDecl( protectedSlots, protectedSlotTypes, protectedSlotSpecifier );
    }

    if ( !privateFuncts.isEmpty() ) {
	out << "private:" << endl;
	writeFunctionsSubDecl( privateFuncts, privateFunctRetTyp, privateFunctSpec );
    }

    // create private additional slots
    if ( !privateSlots.isEmpty() ) {
	out << "private slots:" << endl;
	writeFunctionsSubDecl( privateSlots, privateSlotTypes, privateSlotSpecifier );
    }
    out << "};" << endl;
}

void Uic::writeFunctionsSubDecl( const TQStringList &fuLst, const TQStringList &typLst, const TQStringList &specLst )
{
    TQValueListConstIterator<TQString> it, it2, it3;
    for ( it = fuLst.begin(), it2 = typLst.begin(), it3 = specLst.begin();
	  it != fuLst.end(); ++it, ++it2, ++it3 ) {
	TQString type = *it2;
	if ( type.isEmpty() )
	    type = "void";
	if ( *it3 == "non virtual" )
	    continue;
	out << "    " << type << " " << (*it) << ";" << endl;
    }
    out << endl;
}

/*!
  Creates an implementation for a subclass \a subClass of the form
  given in \a e

  \sa createSubDecl()
 */
void Uic::createSubImpl( const TQDomElement &e, const TQString& subClass )
{
    TQDomElement n;
    TQDomNodeList nl;
    int i;

    TQString objClass = getClassName( e );
    if ( objClass.isEmpty() )
	return;

    // constructor
    if ( objClass == "TQDialog" || objClass == "TQWizard" ) {
	out << "/* " << endl;
	out << " *  Constructs a " << subClass << " which is a child of 'parent', with the " << endl;
	out << " *  name 'name' and widget flags set to 'f' " << endl;
	out << " *" << endl;
	out << " *  The " << objClass.mid(1).lower() << " will by default be modeless, unless you set 'modal' to" << endl;
	out << " *  true to construct a modal " << objClass.mid(1).lower() << "." << endl;
	out << " */" << endl;
	out << subClass << "::" << subClass << "( TQWidget* parent,  const char* name, bool modal, WFlags fl )" << endl;
	out << "    : " << nameOfClass << "( parent, name, modal, fl )" << endl;
    } else { // standard TQWidget
	out << "/* " << endl;
	out << " *  Constructs a " << subClass << " which is a child of 'parent', with the " << endl;
	out << " *  name 'name' and widget flags set to 'f' " << endl;
	out << " */" << endl;
	out << subClass << "::" << subClass << "( TQWidget* parent,  const char* name, WFlags fl )" << endl;
	out << "    : " << nameOfClass << "( parent, name, fl )" << endl;
    }
    out << "{" << endl;
    out << "}" << endl;
    out << endl;

    // destructor
    out << "/*  " << endl;
    out << " *  Destroys the object and frees any allocated resources" << endl;
    out << " */" << endl;
    out << subClass << "::~" << subClass << "()" << endl;
    out << "{" << endl;
    out << "    // no need to delete child widgets, TQt does it all for us" << endl;
    out << "}" << endl;
    out << endl;


    // find additional functions
    TQStringList publicSlots, protectedSlots, privateSlots;
    TQStringList publicSlotTypes, protectedSlotTypes, privateSlotTypes;
    TQStringList publicSlotSpecifier, protectedSlotSpecifier, privateSlotSpecifier;
    TQStringList publicFuncts, protectedFuncts, privateFuncts;
    TQStringList publicFunctRetTyp, protectedFunctRetTyp, privateFunctRetTyp;
    TQStringList publicFunctSpec, protectedFunctSpec, privateFunctSpec;

    nl = e.parentNode().toElement().elementsByTagName( "slot" );
    for ( i = 0; i < (int) nl.length(); i++ ) {
	n = nl.item(i).toElement();
	if ( n.parentNode().toElement().tagName() != "slots"
	     && n.parentNode().toElement().tagName() != "connections" )
	    continue;
	if ( n.attribute( "language", "C++" ) != "C++" )
	    continue;
	TQString returnType = n.attribute( "returnType", "void" );
	TQString functionName = n.firstChild().toText().data().stripWhiteSpace();
	if ( functionName.endsWith( ";" ) )
	    functionName = functionName.left( functionName.length() - 1 );
	TQString specifier = n.attribute( "specifier" );
	TQString access = n.attribute( "access" );
	if ( access == "protected" ) {
	    protectedSlots += functionName;
	    protectedSlotTypes += returnType;
	    protectedSlotSpecifier += specifier;
	} else if ( access == "private" ) {
	    privateSlots += functionName;
	    privateSlotTypes += returnType;
	    privateSlotSpecifier += specifier;
	} else {
	    publicSlots += functionName;
	    publicSlotTypes += returnType;
	    publicSlotSpecifier += specifier;
	}
    }

    nl = e.parentNode().toElement().elementsByTagName( "function" );
    for ( i = 0; i < (int) nl.length(); i++ ) {
	n = nl.item(i).toElement();
	if ( n.parentNode().toElement().tagName() != "functions" )
	    continue;
	if ( n.attribute( "language", "C++" ) != "C++" )
	    continue;
	TQString returnType = n.attribute( "returnType", "void" );
	TQString functionName = n.firstChild().toText().data().stripWhiteSpace();
	if ( functionName.endsWith( ";" ) )
	    functionName = functionName.left( functionName.length() - 1 );
	TQString specifier = n.attribute( "specifier" );
	TQString access = n.attribute( "access" );
	if ( access == "protected" ) {
	    protectedFuncts += functionName;
	    protectedFunctRetTyp += returnType;
	    protectedFunctSpec += specifier;
	} else if ( access == "private" ) {
	    privateFuncts += functionName;
	    privateFunctRetTyp += returnType;
	    privateFunctSpec += specifier;
	} else {
	    publicFuncts += functionName;
	    publicFunctRetTyp += returnType;
	    publicFunctSpec += specifier;
	}
    }

    if ( !publicFuncts.isEmpty() )
	writeFunctionsSubImpl( publicFuncts, publicFunctRetTyp, publicFunctSpec, subClass, "public function" );

    // create stubs for public additional slots
    if ( !publicSlots.isEmpty() )
	writeFunctionsSubImpl( publicSlots, publicSlotTypes, publicSlotSpecifier, subClass, "public slot" );

    if ( !protectedFuncts.isEmpty() )
	writeFunctionsSubImpl( protectedFuncts, protectedFunctRetTyp, protectedFunctSpec, subClass, "protected function" );

    // create stubs for protected additional slots
    if ( !protectedSlots.isEmpty() )
	writeFunctionsSubImpl( protectedSlots, protectedSlotTypes, protectedSlotSpecifier, subClass, "protected slot" );

    if ( !privateFuncts.isEmpty() )
	writeFunctionsSubImpl( privateFuncts, privateFunctRetTyp, privateFunctSpec, subClass, "private function" );

    // create stubs for private additional slots
    if ( !privateSlots.isEmpty() )
	writeFunctionsSubImpl( privateSlots, privateSlotTypes, privateSlotSpecifier, subClass, "private slot" );
}

void Uic::writeFunctionsSubImpl( const TQStringList &fuLst, const TQStringList &typLst, const TQStringList &specLst,
				 const TQString &subClass, const TQString &descr )
{
    TQValueListConstIterator<TQString> it, it2, it3;
    for ( it = fuLst.begin(), it2 = typLst.begin(), it3 = specLst.begin();
	  it != fuLst.end(); ++it, ++it2, ++it3 ) {
	TQString type = *it2;
	if ( type.isEmpty() )
	    type = "void";
	if ( *it3 == "non virtual" )
	    continue;
	out << "/*" << endl;
	out << " * " << descr << endl;
	out << " */" << endl;
	out << type << " " << subClass << "::" << (*it) << endl;
	out << "{" << endl;
	out << "    tqWarning( \"" << subClass << "::" << (*it) << " not yet implemented!\" );" << endl;
	out << "}" << endl << endl;
    }
    out << endl;
}
