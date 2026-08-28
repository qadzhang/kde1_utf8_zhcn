/****************************************************************************
**
** Definition of TQXmlSimpleReader and related classes.
**
** Created : 000518
**
** Copyright (C) 1992-2008 Trolltech ASA.  All rights reserved.
**
** This file is part of the xml module of the TQt GUI Toolkit.
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
** This file may be used under the terms of the Q Public License as
** defined by Trolltech ASA and appearing in the file LICENSE.TQPL
** included in the packaging of this file.  Licensees holding valid TQt
** Commercial licenses may use this file in accordance with the TQt
** Commercial License Agreement provided with the Software.
**
** This file is provided "AS IS" with NO WARRANTY OF ANY KIND,
** INCLUDING THE WARRANTIES OF DESIGN, MERCHANTABILITY AND FITNESS FOR
** A PARTICULAR PURPOSE. Trolltech reserves all rights not granted
** herein.
**
**********************************************************************/

#ifndef TQXML_H
#define TQXML_H

#ifndef QT_H
#include "ntqtextstream.h"
#include "ntqfile.h"
#include "ntqstring.h"
#include "ntqstringlist.h"
#include "ntqvaluevector.h"
#endif // QT_H

#if !defined(TQT_MODULE_XML) || defined( QT_LICENSE_PROFESSIONAL ) || defined( QT_INTERNAL_XML )
#define TQM_EXPORT_XML
#else
#define TQM_EXPORT_XML TQ_EXPORT
#endif

#ifndef TQT_NO_XML

class TQXmlNamespaceSupport;
class TQXmlAttributes;
class TQXmlContentHandler;
class TQXmlDefaultHandler;
class TQXmlDTDHandler;
class TQXmlEntityResolver;
class TQXmlErrorHandler;
class TQXmlLexicalHandler;
class TQXmlDeclHandler;
class TQXmlInputSource;
class TQXmlLocator;
class TQXmlNamespaceSupport;
class TQXmlParseException;

class TQXmlReader;
class TQXmlSimpleReader;

class TQXmlSimpleReaderPrivate;
class TQXmlNamespaceSupportPrivate;
class TQXmlAttributesPrivate;
class TQXmlInputSourcePrivate;
class TQXmlParseExceptionPrivate;
class TQXmlLocatorPrivate;
class TQXmlDefaultHandlerPrivate;


//
// SAX Namespace Support
//

class TQM_EXPORT_XML TQXmlNamespaceSupport
{
public:
    TQXmlNamespaceSupport();
    ~TQXmlNamespaceSupport();

    void setPrefix( const TQString&, const TQString& );

    TQString prefix( const TQString& ) const;
    TQString uri( const TQString& ) const;
    void splitName( const TQString&, TQString&, TQString& ) const;
    void processName( const TQString&, bool, TQString&, TQString& ) const;
    TQStringList prefixes() const;
    TQStringList prefixes( const TQString& ) const;

    void pushContext();
    void popContext();
    void reset();

private:
    TQXmlNamespaceSupportPrivate *d;

    friend class TQXmlSimpleReader;
};


//
// SAX Attributes
//

class TQM_EXPORT_XML TQXmlAttributes
{
public:
    TQXmlAttributes() {}
    virtual ~TQXmlAttributes() {}

    int index( const TQString& qName ) const;
    int index( const TQString& uri, const TQString& localPart ) const;
    int length() const;
    int count() const;
    TQString localName( int index ) const;
    TQString qName( int index ) const;
    TQString uri( int index ) const;
    TQString type( int index ) const;
    TQString type( const TQString& qName ) const;
    TQString type( const TQString& uri, const TQString& localName ) const;
    TQString value( int index ) const;
    TQString value( const TQString& qName ) const;
    TQString value( const TQString& uri, const TQString& localName ) const;

    void clear();
    void append( const TQString &qName, const TQString &uri, const TQString &localPart, const TQString &value );

private:
    TQStringList qnameList;
    TQStringList uriList;
    TQStringList localnameList;
    TQStringList valueList;

    TQXmlAttributesPrivate *d;
};

//
// SAX Input Source
//

class TQM_EXPORT_XML TQXmlInputSource
{
public:
    TQXmlInputSource();
    TQXmlInputSource( TQIODevice *dev );
    TQXmlInputSource( TQFile& file ); // obsolete
    TQXmlInputSource( TQTextStream& stream ); // obsolete
    virtual ~TQXmlInputSource();

    virtual void setData( const TQString& dat );
    virtual void setData( const TQByteArray& dat );
    virtual void fetchData();
    virtual TQString data();
    virtual TQChar next();
    virtual void reset();

    static const TQChar EndOfData;
    static const TQChar EndOfDocument;

protected:
    virtual TQString fromRawData( const TQByteArray &data, bool beginning = false );

private:
    void init();

    TQIODevice *inputDevice;
    TQTextStream *inputStream;

    TQString str;
    const TQChar *unicode;
    int pos;
    int length;
    bool nextReturnedEndOfData;
    TQTextDecoder *encMapper;

    TQXmlInputSourcePrivate *d;
};

//
// SAX Exception Classes
//

class TQM_EXPORT_XML TQXmlParseException
{
public:
    TQXmlParseException( const TQString& name="", int c=-1, int l=-1, const TQString& p="", const TQString& s="" )
	: msg( name ), column( c ), line( l ), pub( p ), sys( s )
    { }

    int columnNumber() const;
    int lineNumber() const;
    TQString publicId() const;
    TQString systemId() const;
    TQString message() const;

private:
    TQString msg;
    int column;
    int line;
    TQString pub;
    TQString sys;

    TQXmlParseExceptionPrivate *d;
};


//
// XML Reader
//

class TQM_EXPORT_XML TQXmlReader
{
public:
    virtual bool feature( const TQString& name, bool *ok = 0 ) const = 0;
    virtual void setFeature( const TQString& name, bool value ) = 0;
    virtual bool hasFeature( const TQString& name ) const = 0;
    virtual void* property( const TQString& name, bool *ok = 0 ) const = 0;
    virtual void setProperty( const TQString& name, void* value ) = 0;
    virtual bool hasProperty( const TQString& name ) const = 0;
    virtual void setEntityResolver( TQXmlEntityResolver* handler ) = 0;
    virtual TQXmlEntityResolver* entityResolver() const = 0;
    virtual void setDTDHandler( TQXmlDTDHandler* handler ) = 0;
    virtual TQXmlDTDHandler* DTDHandler() const = 0;
    virtual void setContentHandler( TQXmlContentHandler* handler ) = 0;
    virtual TQXmlContentHandler* contentHandler() const = 0;
    virtual void setErrorHandler( TQXmlErrorHandler* handler ) = 0;
    virtual TQXmlErrorHandler* errorHandler() const = 0;
    virtual void setLexicalHandler( TQXmlLexicalHandler* handler ) = 0;
    virtual TQXmlLexicalHandler* lexicalHandler() const = 0;
    virtual void setDeclHandler( TQXmlDeclHandler* handler ) = 0;
    virtual TQXmlDeclHandler* declHandler() const = 0;
    virtual bool parse( const TQXmlInputSource& input ) = 0;
    virtual bool parse( const TQXmlInputSource* input ) = 0;
};

class TQM_EXPORT_XML TQXmlSimpleReader : public TQXmlReader
{
public:
    TQXmlSimpleReader();
    virtual ~TQXmlSimpleReader();

    bool feature( const TQString& name, bool *ok = 0 ) const;
    void setFeature( const TQString& name, bool value );
    bool hasFeature( const TQString& name ) const;

    void* property( const TQString& name, bool *ok = 0 ) const;
    void setProperty( const TQString& name, void* value );
    bool hasProperty( const TQString& name ) const;

    void setEntityResolver( TQXmlEntityResolver* handler );
    TQXmlEntityResolver* entityResolver() const;
    void setDTDHandler( TQXmlDTDHandler* handler );
    TQXmlDTDHandler* DTDHandler() const;
    void setContentHandler( TQXmlContentHandler* handler );
    TQXmlContentHandler* contentHandler() const;
    void setErrorHandler( TQXmlErrorHandler* handler );
    TQXmlErrorHandler* errorHandler() const;
    void setLexicalHandler( TQXmlLexicalHandler* handler );
    TQXmlLexicalHandler* lexicalHandler() const;
    void setDeclHandler( TQXmlDeclHandler* handler );
    TQXmlDeclHandler* declHandler() const;

    bool parse( const TQXmlInputSource& input );
    bool parse( const TQXmlInputSource* input );
    virtual bool parse( const TQXmlInputSource* input, bool incremental );
    virtual bool parseContinue();

private:
    // variables
    TQXmlContentHandler *contentHnd;
    TQXmlErrorHandler   *errorHnd;
    TQXmlDTDHandler     *dtdHnd;
    TQXmlEntityResolver *entityRes;
    TQXmlLexicalHandler *lexicalHnd;
    TQXmlDeclHandler    *declHnd;

    TQXmlInputSource *inputSource;

    TQChar c; // the character at reading position
    int   lineNr; // number of line
    int   columnNr; // position in line

    int     nameArrayPos;
    TQChar   nameArray[256]; // only used for names
    TQString nameValue; // only used for names
    int     refArrayPos;
    TQChar   refArray[256]; // only used for references
    TQString refValue; // only used for references
    int     stringArrayPos;
    TQChar   stringArray[256]; // used for any other strings that are parsed
    TQString stringValue; // used for any other strings that are parsed

    TQXmlSimpleReaderPrivate* d;

    // The limit to the amount of times the DTD parsing functions can be called
    // for the DTD currently being parsed.
    static const uint dtdRecursionLimit = 2U;
    // The maximum amount of characters an entity value may contain, after expansion.
    static const uint entityCharacterLimit = 4096U;

    const TQString &string();
    void stringClear();
    inline void stringAddC() { stringAddC(c); }
    void stringAddC(const TQChar&);
    const TQString& name();
    void nameClear();
    inline void nameAddC() { nameAddC(c); }
    void nameAddC(const TQChar&);
    const TQString& ref();
    void refClear();
    inline void refAddC() { refAddC(c); }
    void refAddC(const TQChar&);

    // used by parseReference() and parsePEReference()
    enum EntityRecognitionContext { InContent, InAttributeValue, InEntityValue, InDTD };

    // private functions
    bool eat_ws();
    bool next_eat_ws();

    void next();
    bool atEnd();

    void init( const TQXmlInputSource* i );
    void initData();

    bool entityExist( const TQString& ) const;

    bool parseBeginOrContinue( int state, bool incremental );

    bool parseProlog();
    bool parseElement();
    bool processElementEmptyTag();
    bool processElementETagBegin2();
    bool processElementAttribute();
    bool parseMisc();
    bool parseContent();

    bool parsePI();
    bool parseDoctype();
    bool parseComment();

    bool parseName();
    bool parseNmtoken();
    bool parseAttribute();
    bool parseReference();
    bool processReference();

    bool parseExternalID();
    bool parsePEReference();
    bool parseMarkupdecl();
    bool parseAttlistDecl();
    bool parseAttType();
    bool parseAttValue();
    bool parseElementDecl();
    bool parseNotationDecl();
    bool parseChoiceSeq();
    bool parseEntityDecl();
    bool parseEntityValue();

    bool parseString();

    bool insertXmlRef( const TQString&, const TQString&, bool );

    bool reportEndEntities();
    void reportParseError( const TQString& error );

    typedef bool (TQXmlSimpleReader::*ParseFunction) ();
    void unexpectedEof( ParseFunction where, int state );
    void parseFailed( ParseFunction where, int state );
    void pushParseState( ParseFunction function, int state );
    bool isExpandedEntityValueTooLarge(TQString *errorMessage);

    void setUndefEntityInAttrHack(bool b);

    friend class TQXmlSimpleReaderPrivate;
    friend class TQXmlSimpleReaderLocator;
    friend class TQDomDocumentPrivate;
};

//
// SAX Locator
//

class TQM_EXPORT_XML TQXmlLocator
{
public:
    TQXmlLocator();
    virtual ~TQXmlLocator();

    virtual int columnNumber() = 0;
    virtual int lineNumber() = 0;
//    TQString getPublicId()
//    TQString getSystemId()
};

//
// SAX handler classes
//

class TQM_EXPORT_XML TQXmlContentHandler
{
public:
    virtual void setDocumentLocator( TQXmlLocator* locator ) = 0;
    virtual bool startDocument() = 0;
    virtual bool endDocument() = 0;
    virtual bool startPrefixMapping( const TQString& prefix, const TQString& uri ) = 0;
    virtual bool endPrefixMapping( const TQString& prefix ) = 0;
    virtual bool startElement( const TQString& namespaceURI, const TQString& localName, const TQString& qName, const TQXmlAttributes& atts ) = 0;
    virtual bool endElement( const TQString& namespaceURI, const TQString& localName, const TQString& qName ) = 0;
    virtual bool characters( const TQString& ch ) = 0;
    virtual bool ignorableWhitespace( const TQString& ch ) = 0;
    virtual bool processingInstruction( const TQString& target, const TQString& data ) = 0;
    virtual bool skippedEntity( const TQString& name ) = 0;
    virtual TQString errorString() = 0;
};

class TQM_EXPORT_XML TQXmlErrorHandler
{
public:
    virtual bool warning( const TQXmlParseException& exception ) = 0;
    virtual bool error( const TQXmlParseException& exception ) = 0;
    virtual bool fatalError( const TQXmlParseException& exception ) = 0;
    virtual TQString errorString() = 0;
};

class TQM_EXPORT_XML TQXmlDTDHandler
{
public:
    virtual bool notationDecl( const TQString& name, const TQString& publicId, const TQString& systemId ) = 0;
    virtual bool unparsedEntityDecl( const TQString& name, const TQString& publicId, const TQString& systemId, const TQString& notationName ) = 0;
    virtual TQString errorString() = 0;
};

class TQM_EXPORT_XML TQXmlEntityResolver
{
public:
    virtual bool resolveEntity( const TQString& publicId, const TQString& systemId, TQXmlInputSource*& ret ) = 0;
    virtual TQString errorString() = 0;
};

class TQM_EXPORT_XML TQXmlLexicalHandler
{
public:
    virtual bool startDTD( const TQString& name, const TQString& publicId, const TQString& systemId ) = 0;
    virtual bool endDTD() = 0;
    virtual bool startEntity( const TQString& name ) = 0;
    virtual bool endEntity( const TQString& name ) = 0;
    virtual bool startCDATA() = 0;
    virtual bool endCDATA() = 0;
    virtual bool comment( const TQString& ch ) = 0;
    virtual TQString errorString() = 0;
};

class TQM_EXPORT_XML TQXmlDeclHandler
{
public:
    virtual bool attributeDecl( const TQString& eName, const TQString& aName, const TQString& type, const TQString& valueDefault, const TQString& value ) = 0;
    virtual bool internalEntityDecl( const TQString& name, const TQString& value ) = 0;
    virtual bool externalEntityDecl( const TQString& name, const TQString& publicId, const TQString& systemId ) = 0;
    virtual TQString errorString() = 0;
};


class TQM_EXPORT_XML TQXmlDefaultHandler : public TQXmlContentHandler, public TQXmlErrorHandler, public TQXmlDTDHandler, public TQXmlEntityResolver, public TQXmlLexicalHandler, public TQXmlDeclHandler
{
public:
    TQXmlDefaultHandler() { }
    virtual ~TQXmlDefaultHandler() { }

    void setDocumentLocator( TQXmlLocator* locator );
    bool startDocument();
    bool endDocument();
    bool startPrefixMapping( const TQString& prefix, const TQString& uri );
    bool endPrefixMapping( const TQString& prefix );
    bool startElement( const TQString& namespaceURI, const TQString& localName, const TQString& qName, const TQXmlAttributes& atts );
    bool endElement( const TQString& namespaceURI, const TQString& localName, const TQString& qName );
    bool characters( const TQString& ch );
    bool ignorableWhitespace( const TQString& ch );
    bool processingInstruction( const TQString& target, const TQString& data );
    bool skippedEntity( const TQString& name );

    bool warning( const TQXmlParseException& exception );
    bool error( const TQXmlParseException& exception );
    bool fatalError( const TQXmlParseException& exception );

    bool notationDecl( const TQString& name, const TQString& publicId, const TQString& systemId );
    bool unparsedEntityDecl( const TQString& name, const TQString& publicId, const TQString& systemId, const TQString& notationName );

    bool resolveEntity( const TQString& publicId, const TQString& systemId, TQXmlInputSource*& ret );

    bool startDTD( const TQString& name, const TQString& publicId, const TQString& systemId );
    bool endDTD();
    bool startEntity( const TQString& name );
    bool endEntity( const TQString& name );
    bool startCDATA();
    bool endCDATA();
    bool comment( const TQString& ch );

    bool attributeDecl( const TQString& eName, const TQString& aName, const TQString& type, const TQString& valueDefault, const TQString& value );
    bool internalEntityDecl( const TQString& name, const TQString& value );
    bool externalEntityDecl( const TQString& name, const TQString& publicId, const TQString& systemId );

    TQString errorString();

private:
    TQXmlDefaultHandlerPrivate *d;
};


//
// inlines
//

inline bool TQXmlSimpleReader::atEnd()
{ return (c.unicode()|0x0001) == 0xffff; }
inline int TQXmlAttributes::count() const
{ return length(); }


#endif //TQT_NO_XML

#endif
