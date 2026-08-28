/**********************************************************************
** Copyright (C) 2000-2008 Trolltech ASA.  All rights reserved.
**
** This file is part of TQt Linguist.
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

#include "phrase.h"

#include <ntqapplication.h>
#include <ntqcstring.h>
#include <ntqfile.h>
#include <ntqmessagebox.h>
#include <ntqregexp.h>
#include <ntqtextstream.h>
#include <ntqxml.h>

static TQString protect( const TQString& str )
{
    TQString p = str;
    p.replace( "&", "&amp;" );
    p.replace( "\"", "&quot;" );
    p.replace( ">", "&gt;" );
    p.replace( "<", "&lt;" );
    p.replace( "'", "&apos;" );
    return p;
}

Phrase::Phrase( const TQString& source, const TQString& target,
		const TQString& definition )
    : s( source ), t( target ), d( definition )
{
}

bool operator==( const Phrase& p, const Phrase& q )
{
    return p.source() == q.source() && p.target() == q.target() &&
	   p.definition() == q.definition();
}

class QphHandler : public TQXmlDefaultHandler
{
public:
    QphHandler( PhraseBook *phraseBook )
	: pb( phraseBook ), ferrorCount( 0 ) { }

    virtual bool startElement( const TQString& namespaceURI,
			       const TQString& localName, const TQString& qName,
			       const TQXmlAttributes& atts );
    virtual bool endElement( const TQString& namespaceURI,
			     const TQString& localName, const TQString& qName );
    virtual bool characters( const TQString& ch );
    virtual bool fatalError( const TQXmlParseException& exception );

private:
    PhraseBook *pb;
    TQString source;
    TQString target;
    TQString definition;

    TQString accum;
    int ferrorCount;
};

bool QphHandler::startElement( const TQString& /* namespaceURI */,
			       const TQString& /* localName */,
			       const TQString& qName,
			       const TQXmlAttributes& /* atts */ )
{
    if ( qName == TQString("phrase") ) {
	source.truncate( 0 );
	target.truncate( 0 );
	definition.truncate( 0 );
    }
    accum.truncate( 0 );
    return true;
}

bool QphHandler::endElement( const TQString& /* namespaceURI */,
			     const TQString& /* localName */,
			     const TQString& qName )
{
    if ( qName == TQString("source") )
	source = accum;
    else if ( qName == TQString("target") )
	target = accum;
    else if ( qName == TQString("definition") )
	definition = accum;
    else if ( qName == TQString("phrase") )
	pb->append( Phrase(source, target, definition) );
    return true;
}

bool QphHandler::characters( const TQString& ch )
{
    accum += ch;
    return true;
}

bool QphHandler::fatalError( const TQXmlParseException& exception )
{
    if ( ferrorCount++ == 0 ) {
	TQString msg;
	msg.sprintf( "Parse error at line %d, column %d (%s).",
		     exception.lineNumber(), exception.columnNumber(),
		     exception.message().latin1() );
	TQMessageBox::information( tqApp->mainWidget(),
				  TQObject::tr("TQt Linguist"), msg );
    }
    return false;
}

bool PhraseBook::load( const TQString& filename )
{
    TQFile f( filename );
    if ( !f.open(IO_ReadOnly) )
	return false;

    TQTextStream t( &f );
    TQXmlInputSource in( t );
    TQXmlSimpleReader reader;
    // don't click on these!
    reader.setFeature( "http://xml.org/sax/features/namespaces", false );
    reader.setFeature( "http://xml.org/sax/features/namespace-prefixes", true );
    reader.setFeature( "http://trolltech.com/xml/features/report-whitespace"
		       "-only-CharData", false );
    TQXmlDefaultHandler *hand = new QphHandler( this );
    reader.setContentHandler( hand );
    reader.setErrorHandler( hand );

    bool ok = reader.parse( in );
    reader.setContentHandler( 0 );
    reader.setErrorHandler( 0 );
    delete hand;
    f.close();
    if ( !ok )
	clear();
    return ok;
}

bool PhraseBook::save( const TQString& filename ) const
{
    TQFile f( filename );
    if ( !f.open(IO_WriteOnly) )
	return false;

    TQTextStream t( &f );
    t.setEncoding(TQTextStream::UnicodeUTF8);
    t << "<!DOCTYPE TQPH><TQPH>\n";
    ConstIterator p;
    for ( p = begin(); p != end(); ++p ) {
	t << "<phrase>\n";
	t << "    <source>" << protect( (*p).source() ) << "</source>\n";
	t << "    <target>" << protect( (*p).target() ) << "</target>\n";
	if ( !(*p).definition().isEmpty() )
	    t << "    <definition>" << protect( (*p).definition() )
	      << "</definition>\n";
	t << "</phrase>\n";
    }
    t << "</TQPH>\n";
    f.close();
    return true;
}
