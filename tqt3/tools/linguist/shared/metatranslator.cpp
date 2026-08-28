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

#include "metatranslator.h"

#include <ntqapplication.h>
#include <ntqcstring.h>
#include <ntqfile.h>
#include <ntqmessagebox.h>
#include <ntqtextcodec.h>
#include <ntqtextstream.h>
#include <ntqxml.h>

static bool encodingIsUtf8( const TQXmlAttributes& atts )
{
    for ( int i = 0; i < atts.length(); i++ ) {
	// utf8="true" is a pre-3.0 syntax
	if ( atts.qName(i) == TQString("utf8") ) {
	    return ( atts.value(i) == TQString("true") );
	} else if ( atts.qName(i) == TQString("encoding") ) {
	    return ( atts.value(i) == TQString("UTF-8") );
	}
    }
    return false;
}

class TsHandler : public TQXmlDefaultHandler
{
public:
    TsHandler( MetaTranslator *translator )
	: tor( translator ), type( MetaTranslatorMessage::Finished ),
	  inMessage( false ), ferrorCount( 0 ), contextIsUtf8( false ),
	  messageIsUtf8( false ) { }

    virtual bool startElement( const TQString& namespaceURI,
			       const TQString& localName, const TQString& qName,
			       const TQXmlAttributes& atts );
    virtual bool endElement( const TQString& namespaceURI,
			     const TQString& localName, const TQString& qName );
    virtual bool characters( const TQString& ch );
    virtual bool fatalError( const TQXmlParseException& exception );

private:
    MetaTranslator *tor;
    MetaTranslatorMessage::Type type;
    bool inMessage;
    TQString context;
    TQString source;
    TQString comment;
    TQString translation;

    TQString accum;
    int ferrorCount;
    bool contextIsUtf8;
    bool messageIsUtf8;
};

bool TsHandler::startElement( const TQString& /* namespaceURI */,
			      const TQString& /* localName */,
			      const TQString& qName,
			      const TQXmlAttributes& atts )
{
    if ( qName == TQString("byte") ) {
	for ( int i = 0; i < atts.length(); i++ ) {
	    if ( atts.qName(i) == TQString("value") ) {
		TQString value = atts.value( i );
		int base = 10;
		if ( value.startsWith("x") ) {
		    base = 16;
		    value = value.mid( 1 );
		}
		int n = value.toUInt( 0, base );
		if ( n != 0 )
		    accum += TQChar( n );
	    }
	}	
    } else {
	if ( qName == TQString("context") ) {
	    context.truncate( 0 );
	    source.truncate( 0 );
	    comment.truncate( 0 );
	    translation.truncate( 0 );
	    contextIsUtf8 = encodingIsUtf8( atts );
	} else if ( qName == TQString("message") ) {
	    inMessage = true;
	    type = MetaTranslatorMessage::Finished;
	    source.truncate( 0 );
	    comment.truncate( 0 );
	    translation.truncate( 0 );
	    messageIsUtf8 = encodingIsUtf8( atts );
	} else if ( qName == TQString("translation") ) {
	    for ( int i = 0; i < atts.length(); i++ ) {
		if ( atts.qName(i) == TQString("type") ) {
		    if ( atts.value(i) == TQString("unfinished") )
			type = MetaTranslatorMessage::Unfinished;
		    else if ( atts.value(i) == TQString("obsolete") )
			type = MetaTranslatorMessage::Obsolete;
		    else
			type = MetaTranslatorMessage::Finished;
		}
	    }
	}
	accum.truncate( 0 );
    }
    return true;
}

bool TsHandler::endElement( const TQString& /* namespaceURI */,
			    const TQString& /* localName */,
			    const TQString& qName )
{
    if ( qName == TQString("codec") || qName == TQString("defaultcodec") ) {
	// "codec" is a pre-3.0 syntax
	tor->setCodec( accum );
    } else if ( qName == TQString("name") ) {
	context = accum;
    } else if ( qName == TQString("source") ) {
	source = accum;
    } else if ( qName == TQString("comment") ) {
	if ( inMessage ) {
	    comment = accum;
	} else {
	    if ( contextIsUtf8 )
		tor->insert( MetaTranslatorMessage(context.utf8(),
			     ContextComment,
			     accum.utf8(), TQString::null, true,
			     MetaTranslatorMessage::Unfinished) );
	    else
		tor->insert( MetaTranslatorMessage(context.ascii(),
			     ContextComment,
			     accum.ascii(), TQString::null, false,
			     MetaTranslatorMessage::Unfinished) );
	}
    } else if ( qName == TQString("translation") ) {
	translation = accum;
    } else if ( qName == TQString("message") ) {
	if ( messageIsUtf8 )
	    tor->insert( MetaTranslatorMessage(context.utf8(), source.utf8(),
					       comment.utf8(), translation,
					       true, type) );
	else
	    tor->insert( MetaTranslatorMessage(context.ascii(), source.ascii(),
					       comment.ascii(), translation,
					       false, type) );
	inMessage = false;
    }
    return true;
}

bool TsHandler::characters( const TQString& ch )
{
    TQString t = ch;
    t.replace( "\r", "" );
    accum += t;
    return true;
}

bool TsHandler::fatalError( const TQXmlParseException& exception )
{
    if ( ferrorCount++ == 0 ) {
	TQString msg;
	msg.sprintf( "Parse error at line %d, column %d (%s).",
		     exception.lineNumber(), exception.columnNumber(),
		     exception.message().latin1() );
	if ( tqApp == 0 )
	    fprintf( stderr, "XML error: %s\n", msg.latin1() );
	else
	    TQMessageBox::information( tqApp->mainWidget(),
				      TQObject::tr("TQt Linguist"), msg );
    }
    return false;
}

static TQString numericEntity( int ch )
{
    return TQString( ch <= 0x20 ? "<byte value=\"x%1\"/>" : "&#x%1;" )
	   .arg( ch, 0, 16 );
}

static TQString protect( const TQCString& str )
{
    TQString result;
    int len = (int) str.length();
    for ( int k = 0; k < len; k++ ) {
	switch( str[k] ) {
	case '\"':
	    result += TQString( "&quot;" );
	    break;
	case '&':
	    result += TQString( "&amp;" );
	    break;
	case '>':
	    result += TQString( "&gt;" );
	    break;
	case '<':
	    result += TQString( "&lt;" );
	    break;
	case '\'':
	    result += TQString( "&apos;" );
	    break;
	default:
	    if ( (uchar) str[k] < 0x20 && str[k] != '\n' )
		result += numericEntity( (uchar) str[k] );
	    else
		result += str[k];
	}
    }
    return result;
}

static TQString evilBytes( const TQCString& str, bool utf8 )
{
    if ( utf8 ) {
	return protect( str );
    } else {
	TQString result;
	TQCString t = protect( str ).latin1();
	int len = (int) t.length();
	for ( int k = 0; k < len; k++ ) {
	    if ( (uchar) t[k] >= 0x7f )
		result += numericEntity( (uchar) t[k] );
	    else
		result += TQChar( t[k] );
	}
	return result;
    }
}

MetaTranslatorMessage::MetaTranslatorMessage()
    : utfeight( false ), ty( Unfinished )
{
}

MetaTranslatorMessage::MetaTranslatorMessage( const char *context,
					      const char *sourceText,
					      const char *comment,
					      const TQString& translation,
					      bool utf8, Type type )
    : TQTranslatorMessage( context, sourceText, comment, translation ),
      utfeight( false ), ty( type )
{
    /*
      Don't use UTF-8 if it makes no difference. UTF-8 should be
      reserved for the real problematic case: non-ASCII (possibly
      non-Latin-1) characters in .ui files.
    */
    if ( utf8 ) {
	if ( sourceText != 0 ) {
	    int i = 0;
	    while ( sourceText[i] != '\0' ) {
		if ( (uchar) sourceText[i] >= 0x80 ) {
		    utfeight = true;
		    break;
		}
		i++;
	    }
	}
	if ( !utfeight && comment != 0 ) {
	    int i = 0;
	    while ( comment[i] != '\0' ) {
		if ( (uchar) comment[i] >= 0x80 ) {
		    utfeight = true;
		    break;
		}
		i++;
	    }
	}
    }
}

MetaTranslatorMessage::MetaTranslatorMessage( const MetaTranslatorMessage& m )
    : TQTranslatorMessage( m ), utfeight( m.utfeight ), ty( m.ty )
{
}

MetaTranslatorMessage& MetaTranslatorMessage::operator=(
	const MetaTranslatorMessage& m )
{
    TQTranslatorMessage::operator=( m );
    utfeight = m.utfeight;
    ty = m.ty;
    return *this;
}

bool MetaTranslatorMessage::operator==( const MetaTranslatorMessage& m ) const
{
    return qstrcmp( context(), m.context() ) == 0 &&
	   qstrcmp( sourceText(), m.sourceText() ) == 0 &&
	   qstrcmp( comment(), m.comment() ) == 0;
}

bool MetaTranslatorMessage::operator<( const MetaTranslatorMessage& m ) const
{
    int delta = qstrcmp( context(), m.context() );
    if ( delta == 0 )
	delta = qstrcmp( sourceText(), m.sourceText() );
    if ( delta == 0 )
	delta = qstrcmp( comment(), m.comment() );
    return delta < 0;
}

MetaTranslator::MetaTranslator()
{
    clear();
}

MetaTranslator::MetaTranslator( const MetaTranslator& tor )
    : mm( tor.mm ), codecName( tor.codecName ), codec( tor.codec )
{
}

MetaTranslator& MetaTranslator::operator=( const MetaTranslator& tor )
{
    mm = tor.mm;
    codecName = tor.codecName;
    codec = tor.codec;
    return *this;
}

void MetaTranslator::clear()
{
    mm.clear();
    codecName = "ISO-8859-1";
    codec = 0;
}

bool MetaTranslator::load( const TQString& filename )
{
    TQFile f( filename );
    if ( !f.open(IO_ReadOnly) )
	return false;

    TQTextStream t( &f );
    TQXmlInputSource in( t );
    TQXmlSimpleReader reader;
    reader.setFeature( "http://xml.org/sax/features/namespaces", false );
    reader.setFeature( "http://xml.org/sax/features/namespace-prefixes", true );
    TQXmlDefaultHandler *hand = new TsHandler( this );
    reader.setContentHandler( hand );
    reader.setErrorHandler( hand );

    bool ok = reader.parse( in );
    reader.setContentHandler( 0 );
    reader.setErrorHandler( 0 );
    delete hand;
    f.close();
    return ok;
}

bool MetaTranslator::save( const TQString& filename ) const
{
    TQFile f( filename );
    if ( !f.open(IO_WriteOnly) )
	return false;

    TQTextStream t( &f );
    t.setCodec( TQTextCodec::codecForName("ISO-8859-1") );

    t << "<!DOCTYPE TS><TS>\n";
    if ( codecName != "ISO-8859-1" )
	t << "<defaultcodec>" << codecName << "</defaultcodec>\n";
    TMM::ConstIterator m = mm.begin();
    while ( m != mm.end() ) {
	TMMInv inv;
	TMMInv::Iterator i;
	bool contextIsUtf8 = m.key().utf8();
	TQCString context = m.key().context();
	TQCString comment = "";

	do {
	    if ( TQCString(m.key().sourceText()) == ContextComment ) {
		if ( m.key().type() != MetaTranslatorMessage::Obsolete ) {
		    contextIsUtf8 = m.key().utf8();
		    comment = TQCString( m.key().comment() );
		}
	    } else {
		inv.insert( *m, m.key() );
	    }
	} while ( ++m != mm.end() && TQCString(m.key().context()) == context );

	t << "<context";
	if ( contextIsUtf8 )
	    t << " encoding=\"UTF-8\"";
	t << ">\n";
	t << "    <name>" << evilBytes( context, contextIsUtf8 )
	  << "</name>\n";
	if ( !comment.isEmpty() )
	    t << "    <comment>" << evilBytes( comment, contextIsUtf8 )
	      << "</comment>\n";

	for ( i = inv.begin(); i != inv.end(); ++i ) {
	    // no need for such noise
	    if ( (*i).type() == MetaTranslatorMessage::Obsolete &&
		 (*i).translation().isEmpty() )
		continue;

	    t << "    <message";
	    if ( (*i).utf8() )
		t << " encoding=\"UTF-8\"";
	    t << ">\n"
	      << "        <source>" << evilBytes( (*i).sourceText(),
						  (*i).utf8() )
	      << "</source>\n";
	    if ( !TQCString((*i).comment()).isEmpty() )
		t << "        <comment>" << evilBytes( (*i).comment(),
						       (*i).utf8() )
		  << "</comment>\n";
	    t << "        <translation";
	    if ( (*i).type() == MetaTranslatorMessage::Unfinished )
		t << " type=\"unfinished\"";
	    else if ( (*i).type() == MetaTranslatorMessage::Obsolete )
		t << " type=\"obsolete\"";
	    t << ">" << protect( (*i).translation().utf8() )
	      << "</translation>\n";
	    t << "    </message>\n";
	}
	t << "</context>\n";
    }
    t << "</TS>\n";
    f.close();
    return true;
}

bool MetaTranslator::release( const TQString& filename, bool verbose,
			      TQTranslator::SaveMode mode ) const
{
    TQTranslator tor( 0 );
    int finished = 0;
    int unfinished = 0;
    int untranslated = 0;
    TMM::ConstIterator m;

    for ( m = mm.begin(); m != mm.end(); ++m ) {
	if ( m.key().type() != MetaTranslatorMessage::Obsolete ) {
	    if ( m.key().translation().isEmpty() ) {
		untranslated++;
	    } else {
		if ( m.key().type() == MetaTranslatorMessage::Unfinished )
		    unfinished++;
		else
		    finished++;

		TQCString context = m.key().context();
		TQCString sourceText = m.key().sourceText();
		TQCString comment = m.key().comment();
		TQString translation = m.key().translation();

		/*
		  Drop the comment in (context, sourceText, comment),
		  unless (context, sourceText, "") already exists, or
		  unless we already dropped the comment of (context,
		  sourceText, comment0).
		*/
		if ( comment.isEmpty()
		     || contains(context, sourceText, "")
		     || !tor.findMessage(context, sourceText, "").translation()
			    .isNull() ) {
		    tor.insert( m.key() );
		} else {
		    tor.insert( TQTranslatorMessage(context, sourceText, "",
						   translation) );
		}
	    }
	}
    }

    bool saved = tor.save( filename, mode );
    if ( saved && verbose )
	fprintf( stderr,
		 " %d finished, %d unfinished and %d untranslated messages\n",
		 finished, unfinished, untranslated );
		
    return saved;
}

bool MetaTranslator::contains( const char *context, const char *sourceText,
			       const char *comment ) const
{
    return mm.find( MetaTranslatorMessage(context, sourceText, comment) ) !=
	   mm.end();
}

void MetaTranslator::insert( const MetaTranslatorMessage& m )
{
    int pos = (int)mm.count();
    TMM::Iterator n = mm.find( m );
    if ( n != mm.end() )
	pos = *n;
    mm.replace( m, pos );
}

void MetaTranslator::stripObsoleteMessages()
{
    TMM newmm;

    TMM::Iterator m = mm.begin();
    while ( m != mm.end() ) {
	if ( m.key().type() != MetaTranslatorMessage::Obsolete )
	    newmm.insert( m.key(), *m );
	++m;
    }
    mm = newmm;
}

void MetaTranslator::stripEmptyContexts()
{
    TMM newmm;

    TMM::Iterator m = mm.begin();
    while ( m != mm.end() ) {
	if ( TQCString(m.key().sourceText()) == ContextComment ) {
	    TMM::Iterator n = m;
	    ++n;
	    // the context comment is followed by other messages
	    if ( n != newmm.end() &&
		 qstrcmp(m.key().context(), n.key().context()) == 0 )
		newmm.insert( m.key(), *m );
	} else {
	    newmm.insert( m.key(), *m );
	}
	++m;
    }
    mm = newmm;
}

void MetaTranslator::setCodec( const char *name )
{
    const int latin1 = 4;

    codecName = name;
    codec = TQTextCodec::codecForName( name );
    if ( codec == 0 || codec->mibEnum() == latin1 )
	codec = 0;
}

TQString MetaTranslator::toUnicode( const char *str, bool utf8 ) const
{
    if ( utf8 )
	return TQString::fromUtf8( str );
    else if ( codec == 0 )
	return TQString( str );
    else
	return codec->toUnicode( str );
}

TQValueList<MetaTranslatorMessage> MetaTranslator::messages() const
{
    int n = (int)mm.count();
    TMM::ConstIterator *t = new TMM::ConstIterator[n + 1];
    TMM::ConstIterator m;
    for ( m = mm.begin(); m != mm.end(); ++m )
	t[*m] = m;

    TQValueList<MetaTranslatorMessage> val;
    for ( int i = 0; i < n; i++ )
	val.append( t[i].key() );

    delete[] t;
    return val;
}

TQValueList<MetaTranslatorMessage> MetaTranslator::translatedMessages() const
{
    TQValueList<MetaTranslatorMessage> val;
    TMM::ConstIterator m;
    for ( m = mm.begin(); m != mm.end(); ++m ) {
	if ( m.key().type() == MetaTranslatorMessage::Finished )
	    val.append( m.key() );
    }
    return val;
}
