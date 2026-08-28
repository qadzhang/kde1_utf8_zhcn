/****************************************************************************
**
** Implementation of TQXmlSimpleReader and related classes.
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

#include "ntqxml.h"
#include "ntqtextcodec.h"
#include "ntqbuffer.h"
#include "ntqregexp.h"
#include "ntqptrstack.h"
#include "ntqmap.h"
#include "ntqvaluestack.h"

// needed for TQT_TRANSLATE_NOOP:
#include "ntqobject.h"

#ifndef TQT_NO_XML

//#define QT_QXML_DEBUG

// Error strings for the XML reader
#define XMLERR_OK                         TQT_TRANSLATE_NOOP( "TQXml", "no error occurred" )
#define XMLERR_ERRORBYCONSUMER            TQT_TRANSLATE_NOOP( "TQXml", "error triggered by consumer" )
#define XMLERR_UNEXPECTEDEOF              TQT_TRANSLATE_NOOP( "TQXml", "unexpected end of file" )
#define XMLERR_MORETHANONEDOCTYPE         TQT_TRANSLATE_NOOP( "TQXml", "more than one document type definition" )
#define XMLERR_ERRORPARSINGELEMENT        TQT_TRANSLATE_NOOP( "TQXml", "error occurred while parsing element" )
#define XMLERR_TAGMISMATCH                TQT_TRANSLATE_NOOP( "TQXml", "tag mismatch" )
#define XMLERR_ERRORPARSINGCONTENT        TQT_TRANSLATE_NOOP( "TQXml", "error occurred while parsing content" )
#define XMLERR_UNEXPECTEDCHARACTER        TQT_TRANSLATE_NOOP( "TQXml", "unexpected character" )
#define XMLERR_INVALIDNAMEFORPI           TQT_TRANSLATE_NOOP( "TQXml", "invalid name for processing instruction" )
#define XMLERR_VERSIONEXPECTED            TQT_TRANSLATE_NOOP( "TQXml", "version expected while reading the XML declaration" )
#define XMLERR_WRONGVALUEFORSDECL         TQT_TRANSLATE_NOOP( "TQXml", "wrong value for standalone declaration" )
#define XMLERR_EDECLORSDDECLEXPECTED      TQT_TRANSLATE_NOOP( "TQXml", "encoding declaration or standalone declaration expected while reading the XML declaration" )
#define XMLERR_SDDECLEXPECTED             TQT_TRANSLATE_NOOP( "TQXml", "standalone declaration expected while reading the XML declaration" )
#define XMLERR_ERRORPARSINGDOCTYPE        TQT_TRANSLATE_NOOP( "TQXml", "error occurred while parsing document type definition" )
#define XMLERR_LETTEREXPECTED             TQT_TRANSLATE_NOOP( "TQXml", "letter is expected" )
#define XMLERR_ERRORPARSINGCOMMENT        TQT_TRANSLATE_NOOP( "TQXml", "error occurred while parsing comment" )
#define XMLERR_ERRORPARSINGREFERENCE      TQT_TRANSLATE_NOOP( "TQXml", "error occurred while parsing reference" )
#define XMLERR_INTERNALGENERALENTITYINDTD TQT_TRANSLATE_NOOP( "TQXml", "internal general entity reference not allowed in DTD" )
#define XMLERR_EXTERNALGENERALENTITYINAV  TQT_TRANSLATE_NOOP( "TQXml", "external parsed general entity reference not allowed in attribute value" )
#define XMLERR_EXTERNALGENERALENTITYINDTD TQT_TRANSLATE_NOOP( "TQXml", "external parsed general entity reference not allowed in DTD" )
#define XMLERR_UNPARSEDENTITYREFERENCE    TQT_TRANSLATE_NOOP( "TQXml", "unparsed entity reference in wrong context" )
#define XMLERR_RECURSIVEENTITIES          TQT_TRANSLATE_NOOP( "TQXml", "recursive entities" )
#define XMLERR_ERRORINTEXTDECL            TQT_TRANSLATE_NOOP( "TQXml", "error in the text declaration of an external entity" )

// the constants for the lookup table
static const signed char cltWS      =  0; // white space
static const signed char cltPer     =  1; // %
static const signed char cltAmp     =  2; // &
static const signed char cltGt      =  3; // >
static const signed char cltLt      =  4; // <
static const signed char cltSlash   =  5; // /
static const signed char cltQm      =  6; // ?
static const signed char cltEm      =  7; // !
static const signed char cltDash    =  8; // -
static const signed char cltCB      =  9; // ]
static const signed char cltOB      = 10; // [
static const signed char cltEq      = 11; // =
static const signed char cltDq      = 12; // "
static const signed char cltSq      = 13; // '
static const signed char cltUnknown = 14;

// character lookup table
static const signed char charLookupTable[256]={
    cltUnknown, cltUnknown, cltUnknown, cltUnknown, cltUnknown, cltUnknown, cltUnknown, cltUnknown, // 0x00 - 0x07
    cltUnknown, // 0x08
    cltWS,      // 0x09 \t
    cltWS,      // 0x0A \n
    cltUnknown, // 0x0B
    cltUnknown, // 0x0C
    cltWS,      // 0x0D \r
    cltUnknown, // 0x0E
    cltUnknown, // 0x0F
    cltUnknown, cltUnknown, cltUnknown, cltUnknown, cltUnknown, cltUnknown, cltUnknown, cltUnknown, // 0x17 - 0x16
    cltUnknown, cltUnknown, cltUnknown, cltUnknown, cltUnknown, cltUnknown, cltUnknown, cltUnknown, // 0x18 - 0x1F
    cltWS,      // 0x20 Space
    cltEm,      // 0x21 !
    cltDq,      // 0x22 "
    cltUnknown, // 0x23
    cltUnknown, // 0x24
    cltPer,     // 0x25 %
    cltAmp,     // 0x26 &
    cltSq,      // 0x27 '
    cltUnknown, // 0x28
    cltUnknown, // 0x29
    cltUnknown, // 0x2A
    cltUnknown, // 0x2B
    cltUnknown, // 0x2C
    cltDash,    // 0x2D -
    cltUnknown, // 0x2E
    cltSlash,   // 0x2F /
    cltUnknown, cltUnknown, cltUnknown, cltUnknown, cltUnknown, cltUnknown, cltUnknown, cltUnknown, // 0x30 - 0x37
    cltUnknown, // 0x38
    cltUnknown, // 0x39
    cltUnknown, // 0x3A
    cltUnknown, // 0x3B
    cltLt,      // 0x3C <
    cltEq,      // 0x3D =
    cltGt,      // 0x3E >
    cltQm,      // 0x3F ?
    cltUnknown, cltUnknown, cltUnknown, cltUnknown, cltUnknown, cltUnknown, cltUnknown, cltUnknown, // 0x40 - 0x47
    cltUnknown, cltUnknown, cltUnknown, cltUnknown, cltUnknown, cltUnknown, cltUnknown, cltUnknown, // 0x48 - 0x4F
    cltUnknown, cltUnknown, cltUnknown, cltUnknown, cltUnknown, cltUnknown, cltUnknown, cltUnknown, // 0x50 - 0x57
    cltUnknown, // 0x58
    cltUnknown, // 0x59
    cltUnknown, // 0x5A
    cltOB,      // 0x5B [
    cltUnknown, // 0x5C
    cltCB,      // 0x5D ]
    cltUnknown, // 0x5E
    cltUnknown, // 0x5F
    cltUnknown, cltUnknown, cltUnknown, cltUnknown, cltUnknown, cltUnknown, cltUnknown, cltUnknown, // 0x60 - 0x67
    cltUnknown, cltUnknown, cltUnknown, cltUnknown, cltUnknown, cltUnknown, cltUnknown, cltUnknown, // 0x68 - 0x6F
    cltUnknown, cltUnknown, cltUnknown, cltUnknown, cltUnknown, cltUnknown, cltUnknown, cltUnknown, // 0x70 - 0x77
    cltUnknown, cltUnknown, cltUnknown, cltUnknown, cltUnknown, cltUnknown, cltUnknown, cltUnknown, // 0x78 - 0x7F
    cltUnknown, cltUnknown, cltUnknown, cltUnknown, cltUnknown, cltUnknown, cltUnknown, cltUnknown, // 0x80 - 0x87
    cltUnknown, cltUnknown, cltUnknown, cltUnknown, cltUnknown, cltUnknown, cltUnknown, cltUnknown, // 0x88 - 0x8F
    cltUnknown, cltUnknown, cltUnknown, cltUnknown, cltUnknown, cltUnknown, cltUnknown, cltUnknown, // 0x90 - 0x97
    cltUnknown, cltUnknown, cltUnknown, cltUnknown, cltUnknown, cltUnknown, cltUnknown, cltUnknown, // 0x98 - 0x9F
    cltUnknown, cltUnknown, cltUnknown, cltUnknown, cltUnknown, cltUnknown, cltUnknown, cltUnknown, // 0xA0 - 0xA7
    cltUnknown, cltUnknown, cltUnknown, cltUnknown, cltUnknown, cltUnknown, cltUnknown, cltUnknown, // 0xA8 - 0xAF
    cltUnknown, cltUnknown, cltUnknown, cltUnknown, cltUnknown, cltUnknown, cltUnknown, cltUnknown, // 0xB0 - 0xB7
    cltUnknown, cltUnknown, cltUnknown, cltUnknown, cltUnknown, cltUnknown, cltUnknown, cltUnknown, // 0xB8 - 0xBF
    cltUnknown, cltUnknown, cltUnknown, cltUnknown, cltUnknown, cltUnknown, cltUnknown, cltUnknown, // 0xC0 - 0xC7
    cltUnknown, cltUnknown, cltUnknown, cltUnknown, cltUnknown, cltUnknown, cltUnknown, cltUnknown, // 0xC8 - 0xCF
    cltUnknown, cltUnknown, cltUnknown, cltUnknown, cltUnknown, cltUnknown, cltUnknown, cltUnknown, // 0xD0 - 0xD7
    cltUnknown, cltUnknown, cltUnknown, cltUnknown, cltUnknown, cltUnknown, cltUnknown, cltUnknown, // 0xD8 - 0xDF
    cltUnknown, cltUnknown, cltUnknown, cltUnknown, cltUnknown, cltUnknown, cltUnknown, cltUnknown, // 0xE0 - 0xE7
    cltUnknown, cltUnknown, cltUnknown, cltUnknown, cltUnknown, cltUnknown, cltUnknown, cltUnknown, // 0xE8 - 0xEF
    cltUnknown, cltUnknown, cltUnknown, cltUnknown, cltUnknown, cltUnknown, cltUnknown, cltUnknown, // 0xF0 - 0xF7
    cltUnknown, cltUnknown, cltUnknown, cltUnknown, cltUnknown, cltUnknown, cltUnknown, cltUnknown  // 0xF8 - 0xFF
};


//
// local helper functions
//

/*
  This function strips the TextDecl [77] ("<?xml ...?>") from the string \a
  str. The stripped version is stored in \a str. If this function finds an
  invalid TextDecl, it returns false, otherwise true.

  This function is used for external entities since those can include an
  TextDecl that must be stripped before inserting the entity.
*/
static bool stripTextDecl( TQString& str )
{
    TQString textDeclStart( "<?xml" );
    if ( str.startsWith( textDeclStart ) ) {
	TQRegExp textDecl(TQString::fromLatin1(
	    "^<\\?xml\\s+"
	    "(version\\s*=\\s*((['\"])[-a-zA-Z0-9_.:]+\\3))?"
	    "\\s*"
	    "(encoding\\s*=\\s*((['\"])[A-Za-z][-a-zA-Z0-9_.]*\\6))?"
	    "\\s*\\?>"
	));
	TQString strTmp = str.replace( textDecl, "" );
	if ( strTmp.length() != str.length() )
	    return false; // external entity has wrong TextDecl
	str = strTmp;
    }
    return true;
}


class TQXmlAttributesPrivate
{
};
class TQXmlInputSourcePrivate
{
};
class TQXmlParseExceptionPrivate
{
};
class TQXmlLocatorPrivate
{
};
class TQXmlDefaultHandlerPrivate
{
};

/*!
    \class TQXmlParseException ntqxml.h
    \reentrant
    \brief The TQXmlParseException class is used to report errors with
    the TQXmlErrorHandler interface.
\if defined(commercial)
    It is part of the <a href="commercialeditions.html">TQt Enterprise Edition</a>.
\endif

    \module XML
    \ingroup xml-tools

    The XML subsystem constructs an instance of this class when it
    detects an error. You can retrieve the place where the error
    occurred using systemId(), publicId(), lineNumber() and
    columnNumber(), along with the error message().

    \sa TQXmlErrorHandler TQXmlReader
*/

/*!
    \fn TQXmlParseException::TQXmlParseException( const TQString& name, int c, int l, const TQString& p, const TQString& s )

    Constructs a parse exception with the error string \a name for
    column \a c and line \a l for the public identifier \a p and the
    system identifier \a s.
*/

/*!
    Returns the error message.
*/
TQString TQXmlParseException::message() const
{
    return msg;
}
/*!
    Returns the column number where the error occurred.
*/
int TQXmlParseException::columnNumber() const
{
    return column;
}
/*!
    Returns the line number where the error occurred.
*/
int TQXmlParseException::lineNumber() const
{
    return line;
}
/*!
    Returns the public identifier where the error occurred.
*/
TQString TQXmlParseException::publicId() const
{
    return pub;
}
/*!
    Returns the system identifier where the error occurred.
*/
TQString TQXmlParseException::systemId() const
{
    return sys;
}


/*!
    \class TQXmlLocator ntqxml.h
    \reentrant
    \brief The TQXmlLocator class provides the XML handler classes with
    information about the parsing position within a file.
\if defined(commercial)
    It is part of the <a href="commercialeditions.html">TQt Enterprise Edition</a>.
\endif

    \module XML
    \ingroup xml-tools

    The reader reports a TQXmlLocator to the content handler before it
    starts to parse the document. This is done with the
    TQXmlContentHandler::setDocumentLocator() function. The handler
    classes can now use this locator to get the position (lineNumber()
    and columnNumber()) that the reader has reached.
*/

/*!
    Constructor.
*/
TQXmlLocator::TQXmlLocator()
{
}

/*!
    Destructor.
*/
TQXmlLocator::~TQXmlLocator()
{
}

/*!
    \fn int TQXmlLocator::columnNumber()

    Returns the column number (starting at 1) or -1 if there is no
    column number available.
*/

/*!
    \fn int TQXmlLocator::lineNumber()

    Returns the line number (starting at 1) or -1 if there is no line
    number available.
*/

class TQXmlSimpleReaderLocator : public TQXmlLocator
{
public:
    TQXmlSimpleReaderLocator( TQXmlSimpleReader* parent )
    {
	reader = parent;
    }
    ~TQXmlSimpleReaderLocator()
    {
    }

    int columnNumber()
    {
	return ( reader->columnNr == -1 ? -1 : reader->columnNr + 1 );
    }
    int lineNumber()
    {
	return ( reader->lineNr == -1 ? -1 : reader->lineNr + 1 );
    }
//    TQString getPublicId()
//    TQString getSystemId()

private:
    TQXmlSimpleReader *reader;
};

/*********************************************
 *
 * TQXmlNamespaceSupport
 *
 *********************************************/

typedef TQMap<TQString, TQString> NamespaceMap;

class TQXmlNamespaceSupportPrivate
{
public:
    TQXmlNamespaceSupportPrivate()
    {
	ns.insert( "xml", "http://www.w3.org/XML/1998/namespace" ); // the XML namespace
    }

    ~TQXmlNamespaceSupportPrivate()
    {
    }

    TQValueStack<NamespaceMap> nsStack;
    NamespaceMap ns;
};

/*!
    \class TQXmlNamespaceSupport ntqxml.h
    \reentrant
    \brief The TQXmlNamespaceSupport class is a helper class for XML
    readers which want to include namespace support.
\if defined(commercial)
    It is part of the <a href="commercialeditions.html">TQt Enterprise Edition</a>.
\endif

    \module XML
    \ingroup xml-tools

    You can set the prefix for the current namespace with setPrefix(),
    and get the list of current prefixes (or those for a given URI)
    with prefixes(). The namespace URI is available from uri(). Use
    pushContext() to start a new namespace context, and popContext()
    to return to the previous namespace context. Use splitName() or
    processName() to split a name into its prefix and local name.

    See also the \link xml.html#sax2Namespaces namespace description\endlink.
*/

/*!
    Constructs a TQXmlNamespaceSupport.
*/
TQXmlNamespaceSupport::TQXmlNamespaceSupport()
{
    d = new TQXmlNamespaceSupportPrivate;
}

/*!
    Destroys a TQXmlNamespaceSupport.
*/
TQXmlNamespaceSupport::~TQXmlNamespaceSupport()
{
    delete d;
}

/*!
    This function declares a prefix \a pre in the current namespace
    context to be the namespace URI \a uri. The prefix remains in
    force until this context is popped, unless it is shadowed in a
    descendant context.

    Note that there is an asymmetry in this library. prefix() does not
    return the default "" prefix, even if you have declared one; to
    check for a default prefix, you must look it up explicitly using
    uri(). This asymmetry exists to make it easier to look up prefixes
    for attribute names, where the default prefix is not allowed.
*/
void TQXmlNamespaceSupport::setPrefix( const TQString& pre, const TQString& uri )
{
    if( pre.isNull() ) {
	d->ns.insert( "", uri );
    } else {
	d->ns.insert( pre, uri );
    }
}

/*!
    Returns one of the prefixes mapped to the namespace URI \a uri.

    If more than one prefix is currently mapped to the same URI, this
    function makes an arbitrary selection; if you want all of the
    prefixes, use prefixes() instead.

    Note: to check for a default prefix, use the uri() function with
    an argument of "".
*/
TQString TQXmlNamespaceSupport::prefix( const TQString& uri ) const
{
    NamespaceMap::const_iterator itc, it = d->ns.constBegin();
    while ( (itc=it) != d->ns.constEnd() ) {
	++it;
	if ( itc.data() == uri && !itc.key().isEmpty() )
	    return itc.key();
    }
    return "";
}

/*!
    Looks up the prefix \a prefix in the current context and returns
    the currently-mapped namespace URI. Use the empty string ("") for
    the default namespace.
*/
TQString TQXmlNamespaceSupport::uri( const TQString& prefix ) const
{
    return d->ns[prefix];
}

/*!
    Splits the name \a qname at the ':' and returns the prefix in \a
    prefix and the local name in \a localname.

    \sa processName()
*/
void TQXmlNamespaceSupport::splitName( const TQString& qname,
	TQString& prefix, TQString& localname ) const
{
    int pos = qname.find(':');
    if (pos == -1)
        pos = qname.length();

    prefix = qname.left( pos );
    localname = qname.mid( pos+1 );
}

/*!
    Processes a raw XML 1.0 name in the current context by removing
    the prefix and looking it up among the prefixes currently
    declared.

    \a qname is the raw XML 1.0 name to be processed. \a isAttribute
    is true if the name is an attribute name.

    This function stores the namespace URI in \a nsuri (which will be
    set to TQString::null if the raw name has an undeclared prefix),
    and stores the local name (without prefix) in \a localname (which
    will be set to TQString::null if no namespace is in use).

    Note that attribute names are processed differently than element
    names: an unprefixed element name gets the default namespace (if
    any), while an unprefixed element name does not.
*/
void TQXmlNamespaceSupport::processName( const TQString& qname,
	bool isAttribute,
	TQString& nsuri, TQString& localname ) const
{
    int len = qname.length();
    const TQChar *data = qname.unicode();
    for (int pos = 0; pos < len; ++pos) {
        if (data[pos].unicode() == ':') {
            nsuri = uri(qname.left(pos));
            localname = qname.mid(pos + 1);
            return;
	}
    }
    // there was no ':'
    nsuri = TQString::null;
    // attributes don't take default namespace
    if (!isAttribute && !d->ns.isEmpty()) {
        /*
            We want to access d->ns.value(""), but as an optimization
            we use the fact that "" compares less than any other
            string, so it's either first in the map or not there.
        */
        NamespaceMap::const_iterator first = d->ns.constBegin();
        if (first.key().isEmpty())
            nsuri = first.data(); // get default namespace
    }
    localname = qname;
}

/*!
    Returns a list of all the prefixes currently declared.

    If there is a default prefix, this function does not return it in
    the list; check for the default prefix using uri() with an
    argument of "".

    Note that if you want to iterate over the list, you should iterate
    over a copy, e.g.
    \code
    TQStringList list = myXmlNamespaceSupport.prefixes();
    TQStringList::iterator it = list.begin();
    while ( it != list.end() ) {
	myProcessing( *it );
	++it;
    }
    \endcode
*/
TQStringList TQXmlNamespaceSupport::prefixes() const
{
    TQStringList list;

    NamespaceMap::const_iterator itc, it = d->ns.constBegin();
    while ( (itc=it) != d->ns.constEnd() ) {
	++it;
	if ( !itc.key().isEmpty() )
	    list.append( itc.key() );
    }
    return list;
}

/*!
    \overload

    Returns a list of all prefixes currently declared for the
    namespace URI \a uri.

    The "xml:" prefix is included. If you only want one prefix that is
    mapped to the namespace URI, and you don't care which one you get,
    use the prefix() function instead.

    Note: the empty (default) prefix is never included in this list;
    to check for the presence of a default namespace, use uri() with
    an argument of "".

    Note that if you want to iterate over the list, you should iterate
    over a copy, e.g.
    \code
    TQStringList list = myXmlNamespaceSupport.prefixes( "" );
    TQStringList::Iterator it = list.begin();
    while( it != list.end() ) {
	myProcessing( *it );
	++it;
    }
    \endcode
*/
TQStringList TQXmlNamespaceSupport::prefixes( const TQString& uri ) const
{
    TQStringList list;

    NamespaceMap::const_iterator itc, it = d->ns.constBegin();
    while ( (itc=it) != d->ns.constEnd() ) {
	++it;
	if ( itc.data() == uri && !itc.key().isEmpty() )
	    list.append( itc.key() );
    }
    return list;
}

/*!
    Starts a new namespace context.

    Normally, you should push a new context at the beginning of each
    XML element: the new context automatically inherits the
    declarations of its parent context, and it also keeps track of
    which declarations were made within this context.

    \sa popContext()
*/
void TQXmlNamespaceSupport::pushContext()
{
    d->nsStack.push(d->ns);
}

/*!
    Reverts to the previous namespace context.

    Normally, you should pop the context at the end of each XML
    element. After popping the context, all namespace prefix mappings
    that were previously in force are restored.

    \sa pushContext()
*/
void TQXmlNamespaceSupport::popContext()
{
    d->ns.clear();
    if( !d->nsStack.isEmpty() )
	d->ns = d->nsStack.pop();
}

/*!
    Resets this namespace support object ready for reuse.
*/
void TQXmlNamespaceSupport::reset()
{
    delete d;
    d = new TQXmlNamespaceSupportPrivate;
}



/*********************************************
 *
 * TQXmlAttributes
 *
 *********************************************/

/*!
    \class TQXmlAttributes ntqxml.h
    \reentrant
    \brief The TQXmlAttributes class provides XML attributes.
\if defined(commercial)
    It is part of the <a href="commercialeditions.html">TQt Enterprise Edition</a>.
\endif

    \module XML
    \ingroup xml-tools

    If attributes are reported by TQXmlContentHandler::startElement()
    this class is used to pass the attribute values.

    Use index() to locate the position of an attribute in the list,
    count() to retrieve the number of attributes, and clear() to
    remove the attributes. New attributes can be added with append().
    Use type() to get an attribute's type and value() to get its
    value. The attribute's name is available from localName() or
    qName(), and its namespace URI from uri().

*/

/*!
    \fn TQXmlAttributes::TQXmlAttributes()

    Constructs an empty attribute list.
*/

/*!
    \fn TQXmlAttributes::~TQXmlAttributes()

    Destroys the attributes object.
*/

/*!
    Looks up the index of an attribute by the qualified name \a qName.

    Returns the index of the attribute or -1 if it wasn't found.

    See also the \link xml.html#sax2Namespaces namespace description\endlink.
*/
int TQXmlAttributes::index( const TQString& qName ) const
{
    return qnameList.findIndex( qName );
}

/*!
    \overload

    Looks up the index of an attribute by a namespace name.

    \a uri specifies the namespace URI, or an empty string if the name
    has no namespace URI. \a localPart specifies the attribute's local
    name.

    Returns the index of the attribute, or -1 if it wasn't found.

    See also the \link xml.html#sax2Namespaces namespace description\endlink.
*/
int TQXmlAttributes::index( const TQString& uri, const TQString& localPart ) const
{
    TQString uriTmp;
    if ( uri.isEmpty() )
       uriTmp = TQString::null;
    else
       uriTmp = uri;
    uint count = (uint)uriList.count(); // ### size_t/int cast
    for ( uint i=0; i<count; i++ ) {
       if ( uriList[i] == uriTmp && localnameList[i] == localPart )
           return i;
    }
    return -1;
}

/*!
    Returns the number of attributes in the list.

    \sa count()
*/
int TQXmlAttributes::length() const
{
    return (int)valueList.count();
}

/*!
    \fn int TQXmlAttributes::count() const

    Returns the number of attributes in the list. This function is
    equivalent to length().
*/

/*!
    Looks up an attribute's local name for the attribute at position
    \a index. If no namespace processing is done, the local name is
    TQString::null.

    See also the \link xml.html#sax2Namespaces namespace description\endlink.
*/
TQString TQXmlAttributes::localName( int index ) const
{
    return localnameList[index];
}

/*!
    Looks up an attribute's XML 1.0 qualified name for the attribute
    at position \a index.

    See also the \link xml.html#sax2Namespaces namespace description\endlink.
*/
TQString TQXmlAttributes::qName( int index ) const
{
    return qnameList[index];
}

/*!
    Looks up an attribute's namespace URI for the attribute at
    position \a index. If no namespace processing is done or if the
    attribute has no namespace, the namespace URI is TQString::null.

    See also the \link xml.html#sax2Namespaces namespace description\endlink.
*/
TQString TQXmlAttributes::uri( int index ) const
{
    return uriList[index];
}

/*!
    Looks up an attribute's type for the attribute at position \a
    index.

    Currently only "CDATA" is returned.
*/
TQString TQXmlAttributes::type( int ) const
{
    return "CDATA";
}

/*!
    \overload

    Looks up an attribute's type for the qualified name \a qName.

    Currently only "CDATA" is returned.
*/
TQString TQXmlAttributes::type( const TQString& ) const
{
    return "CDATA";
}

/*!
    \overload

    Looks up an attribute's type by namespace name.

    \a uri specifies the namespace URI and \a localName specifies the
    local name. If the name has no namespace URI, use an empty string
    for \a uri.

    Currently only "CDATA" is returned.
*/
TQString TQXmlAttributes::type( const TQString&, const TQString& ) const
{
    return "CDATA";
}

/*!
    Looks up an attribute's value for the attribute at position \a
    index.
*/
TQString TQXmlAttributes::value( int index ) const
{
    return valueList[index];
}

/*!
    \overload

    Looks up an attribute's value for the qualified name \a qName.

    See also the \link xml.html#sax2Namespaces namespace description\endlink.
*/
TQString TQXmlAttributes::value( const TQString& qName ) const
{
    int i = index( qName );
    if ( i == -1 )
	return TQString::null;
    return valueList[ i ];
}

/*!
    \overload

    Looks up an attribute's value by namespace name.

    \a uri specifies the namespace URI, or an empty string if the name
    has no namespace URI. \a localName specifies the attribute's local
    name.

    See also the \link xml.html#sax2Namespaces namespace description\endlink.
*/
TQString TQXmlAttributes::value( const TQString& uri, const TQString& localName ) const
{
    int i = index( uri, localName );
    if ( i == -1 )
	return TQString::null;
    return valueList[ i ];
}

/*!
    Clears the list of attributes.

    \sa append()
*/
void TQXmlAttributes::clear()
{
    qnameList.clear();
    uriList.clear();
    localnameList.clear();
    valueList.clear();
}

/*!
    Appends a new attribute entry to the list of attributes. The
    qualified name of the attribute is \a qName, the namespace URI is
    \a uri and the local name is \a localPart. The value of the
    attribute is \a value.

    \sa qName() uri() localName() value()
*/
void TQXmlAttributes::append( const TQString &qName, const TQString &uri, const TQString &localPart, const TQString &value )
{
    qnameList.append( qName );
    uriList.append( uri );
    localnameList.append( localPart);
    valueList.append( value );
}


/*********************************************
 *
 * TQXmlInputSource
 *
 *********************************************/

/*!
    \class TQXmlInputSource ntqxml.h
    \reentrant
    \brief The TQXmlInputSource class provides the input data for the
    TQXmlReader subclasses.
\if defined(commercial)
    It is part of the <a href="commercialeditions.html">TQt Enterprise Edition</a>.
\endif

    \module XML
    \ingroup xml-tools

    All subclasses of TQXmlReader read the input XML document from this
    class.

    This class recognizes the encoding of the data by reading the
    encoding declaration in the XML file if it finds one, and reading
    the data using the corresponding encoding. If it does not find an
    encoding declaration, then it assumes that the data is either in
    UTF-8 or UTF-16, depending on whether it can find a byte-order
    mark.

    There are two ways to populate the input source with data: you can
    construct it with a TQIODevice* so that the input source reads the
    data from that device. Or you can set the data explicitly with one
    of the setData() functions.

    Usually you either construct a TQXmlInputSource that works on a
    TQIODevice* or you construct an empty TQXmlInputSource and set the
    data with setData(). There are only rare occasions where you would
    want to mix both methods.

    The TQXmlReader subclasses use the next() function to read the
    input character by character. If you want to start from the
    beginning again, use reset().

    The functions data() and fetchData() are useful if you want to do
    something with the data other than parsing, e.g. displaying the
    raw XML file. The benefit of using the TQXmlInputClass in such
    cases is that it tries to use the correct encoding.

    \sa TQXmlReader TQXmlSimpleReader
*/

// the following two are guaranteed not to be a character
const TQChar TQXmlInputSource::EndOfData = TQChar((ushort)0xfffe);
const TQChar TQXmlInputSource::EndOfDocument = TQChar((ushort)0xffff);

/*
    Common part of the constructors.
*/
void TQXmlInputSource::init()
{
    inputDevice = 0;
    inputStream = 0;

    setData( TQString::null );
    encMapper = 0;
}

/*!
    Constructs an input source which contains no data.

    \sa setData()
*/
TQXmlInputSource::TQXmlInputSource()
{
    init();
}

/*!
    Constructs an input source and gets the data from device \a dev.
    If \a dev is not open, it is opened in read-only mode. If \a dev
    is 0 or it is not possible to read from the device, the input
    source will contain no data.

    \sa setData() fetchData() TQIODevice
*/
TQXmlInputSource::TQXmlInputSource( TQIODevice *dev )
{
    init();
    inputDevice = dev;
    fetchData();
}

/*! \obsolete
  Constructs an input source and gets the data from the text stream \a stream.
*/
TQXmlInputSource::TQXmlInputSource( TQTextStream& stream )
{
    init();
    inputStream = &stream;
    fetchData();
}

/*! \obsolete
  Constructs an input source and gets the data from the file \a file. If the
  file cannot be read the input source is empty.
*/
TQXmlInputSource::TQXmlInputSource( TQFile& file )
{
    init();
    inputDevice = &file;
    fetchData();
}

/*!
    Destructor.
*/
TQXmlInputSource::~TQXmlInputSource()
{
    delete encMapper;
}

/*!
    Returns the next character of the input source. If this function
    reaches the end of available data, it returns
    TQXmlInputSource::EndOfData. If you call next() after that, it
    tries to fetch more data by calling fetchData(). If the
    fetchData() call results in new data, this function returns the
    first character of that data; otherwise it returns
    TQXmlInputSource::EndOfDocument.

    \sa reset() fetchData() TQXmlSimpleReader::parse() TQXmlSimpleReader::parseContinue()
*/
TQChar TQXmlInputSource::next()
{
    if ( pos >= length ) {
	if ( nextReturnedEndOfData ) {
	    nextReturnedEndOfData = false;
	    fetchData();
	    if ( pos >= length ) {
		return EndOfDocument;
	    }
	    return next();
	}
	nextReturnedEndOfData = true;
	return EndOfData;
    }
    return unicode[pos++];
}

/*!
    This function sets the position used by next() to the beginning of
    the data returned by data(). This is useful if you want to use the
    input source for more than one parse.

    \sa next()
*/
void TQXmlInputSource::reset()
{
    nextReturnedEndOfData = false;
    pos = 0;
}

/*!
    Returns the data the input source contains or TQString::null if the
    input source does not contain any data.

    \sa setData() TQXmlInputSource() fetchData()
*/
TQString TQXmlInputSource::data()
{
    return str;
}

/*!
    Sets the data of the input source to \a dat.

    If the input source already contains data, this function deletes
    that data first.

    \sa data()
*/
void TQXmlInputSource::setData( const TQString& dat )
{
    str = dat;
    unicode = str.unicode();
    pos = 0;
    length = str.length();
    nextReturnedEndOfData = false;
}

/*!
    \overload

    The data \a dat is passed through the correct text-codec, before
    it is set.
*/
void TQXmlInputSource::setData( const TQByteArray& dat )
{
    setData( fromRawData( dat ) );
}

/*!
    This function reads more data from the device that was set during
    construction. If the input source already contained data, this
    function deletes that data first.

    This object contains no data after a call to this function if the
    object was constructed without a device to read data from or if
    this function was not able to get more data from the device.

    There are two occasions where a fetch is done implicitly by
    another function call: during construction (so that the object
    starts out with some initial data where available), and during a
    call to next() (if the data had run out).

    You don't normally need to use this function if you use next().

    \sa data() next() TQXmlInputSource()
*/
void TQXmlInputSource::fetchData()
{
    TQByteArray rawData;

    if ( inputDevice != 0 ) {
	if ( inputDevice->isOpen() || inputDevice->open( IO_ReadOnly )  )
	    rawData = inputDevice->readAll();
    } else if ( inputStream != 0 ) {
	if ( inputStream->device()->isDirectAccess() ) {
	    rawData = inputStream->device()->readAll();
	} else {
	    int nread = 0;
	    const int bufsize = 512;
	    while ( !inputStream->device()->atEnd() ) {
		rawData.resize( nread + bufsize );
		nread += inputStream->device()->readBlock( rawData.data()+nread, bufsize );
	    }
	    rawData.resize( nread );
	}
    }
    setData( fromRawData( rawData ) );
}

/*!
    This function reads the XML file from \a data and tries to
    recognize the encoding. It converts the raw data \a data into a
    TQString and returns it. It tries its best to get the correct
    encoding for the XML file.

    If \a beginning is true, this function assumes that the data
    starts at the beginning of a new XML document and looks for an
    encoding declaration. If \a beginning is false, it converts the
    raw data using the encoding determined from prior calls.
*/
TQString TQXmlInputSource::fromRawData( const TQByteArray &data, bool beginning )
{
    if ( data.size() == 0 )
	return TQString::null;
    if ( beginning ) {
	delete encMapper;
	encMapper = 0;
    }
    if ( encMapper == 0 ) {
	TQTextCodec *codec = 0;
	// look for byte order mark and read the first 5 characters
	if ( data.size() >= 2 &&
		( ((uchar)data.at(0)==(uchar)0xfe &&
		   (uchar)data.at(1)==(uchar)0xff ) ||
		  ((uchar)data.at(0)==(uchar)0xff &&
		   (uchar)data.at(1)==(uchar)0xfe ) )) {
	    codec = TQTextCodec::codecForMib( 1000 ); // UTF-16
	} else {
	    codec = TQTextCodec::codecForMib( 106 ); // UTF-8
	}
	if ( !codec )
	    return TQString::null;

	encMapper = codec->makeDecoder();
	TQString input = encMapper->toUnicode( data.data(), data.size() );
	// ### unexpected EOF? (for incremental parsing)
	// starts the document with an XML declaration?
	if ( input.find("<?xml") == 0 ) {
	    // try to find out if there is an encoding
	    int endPos = input.find( ">" );
	    int pos = input.find( "encoding" );
	    if ( pos < endPos && pos != -1 ) {
		TQString encoding;
		do {
		    pos++;
		    if ( pos > endPos ) {
			return input;
		    }
		} while( input[pos] != '"' && input[pos] != '\'' );
		pos++;
		while( input[pos] != '"' && input[pos] != '\'' ) {
		    encoding += input[pos];
		    pos++;
		    if ( pos > endPos ) {
			return input;
		    }
		}

		codec = TQTextCodec::codecForName( encoding );
		if ( codec == 0 ) {
		    return input;
		}
		delete encMapper;
		encMapper = codec->makeDecoder();
		return encMapper->toUnicode( data.data(), data.size() );
	    }
	}
	return input;
    }
    return encMapper->toUnicode( data.data(), data.size() );
}


/*********************************************
 *
 * TQXmlDefaultHandler
 *
 *********************************************/

/*!
    \class TQXmlContentHandler ntqxml.h
    \reentrant
    \brief The TQXmlContentHandler class provides an interface to
    report the logical content of XML data.
\if defined(commercial)
    It is part of the <a href="commercialeditions.html">TQt Enterprise Edition</a>.
\endif

    \module XML
    \ingroup xml-tools

    If the application needs to be informed of basic parsing events,
    it can implement this interface and activate it using
    TQXmlReader::setContentHandler(). The reader can then report basic
    document-related events like the start and end of elements and
    character data through this interface.

    The order of events in this interface is very important, and
    mirrors the order of information in the document itself. For
    example, all of an element's content (character data, processing
    instructions, and sub-elements) appears, in order, between the
    startElement() event and the corresponding endElement() event.

    The class TQXmlDefaultHandler provides a default implementation for
    this interface; subclassing from the TQXmlDefaultHandler class is
    very convenient if you only want to be informed of some parsing
    events.

    The startDocument() function is called at the start of the
    document, and endDocument() is called at the end. Before parsing
    begins setDocumentLocator() is called. For each element
    startElement() is called, with endElement() being called at the
    end of each element. The characters() function is called with
    chunks of character data; ignorableWhitespace() is called with
    chunks of whitespace and processingInstruction() is called with
    processing instructions. If an entity is skipped skippedEntity()
    is called. At the beginning of prefix-URI scopes
    startPrefixMapping() is called.

    See also the \link xml.html#sax2Intro Introduction to SAX2\endlink.

    \sa TQXmlDTDHandler TQXmlDeclHandler TQXmlEntityResolver TQXmlErrorHandler
    TQXmlLexicalHandler
*/

/*!
    \fn void TQXmlContentHandler::setDocumentLocator( TQXmlLocator* locator )

    The reader calls this function before it starts parsing the
    document. The argument \a locator is a pointer to a TQXmlLocator
    which allows the application to get the parsing position within
    the document.

    Do not destroy the \a locator; it is destroyed when the reader is
    destroyed. (Do not use the \a locator after the reader is
    destroyed).
*/

/*!
    \fn bool TQXmlContentHandler::startDocument()

    The reader calls this function when it starts parsing the
    document. The reader calls this function just once, after the call
    to setDocumentLocator(), and before any other functions in this
    class or in the TQXmlDTDHandler class are called.

    If this function returns false the reader stops parsing and
    reports an error. The reader uses the function errorString() to
    get the error message.

    \sa endDocument()
*/

/*!
    \fn bool TQXmlContentHandler::endDocument()

    The reader calls this function after it has finished parsing. It
    is called just once, and is the last handler function called. It
    is called after the reader has read all input or has abandoned
    parsing because of a fatal error.

    If this function returns false the reader stops parsing and
    reports an error. The reader uses the function errorString() to
    get the error message.

    \sa startDocument()
*/

/*!
    \fn bool TQXmlContentHandler::startPrefixMapping( const TQString& prefix, const TQString& uri )

    The reader calls this function to signal the begin of a prefix-URI
    namespace mapping scope. This information is not necessary for
    normal namespace processing since the reader automatically
    replaces prefixes for element and attribute names.

    Note that startPrefixMapping() and endPrefixMapping() calls are
    not guaranteed to be properly nested relative to each other: all
    startPrefixMapping() events occur before the corresponding
    startElement() event, and all endPrefixMapping() events occur
    after the corresponding endElement() event, but their order is not
    otherwise guaranteed.

    The argument \a prefix is the namespace prefix being declared and
    the argument \a uri is the namespace URI the prefix is mapped to.

    If this function returns false the reader stops parsing and
    reports an error. The reader uses the function errorString() to
    get the error message.

    See also the \link xml.html#sax2Namespaces namespace description\endlink.

    \sa endPrefixMapping()
*/

/*!
    \fn bool TQXmlContentHandler::endPrefixMapping( const TQString& prefix )

    The reader calls this function to signal the end of a prefix
    mapping for the prefix \a prefix.

    If this function returns false the reader stops parsing and
    reports an error. The reader uses the function errorString() to
    get the error message.

    See also the \link xml.html#sax2Namespaces namespace description\endlink.

    \sa startPrefixMapping()
*/

/*!
    \fn bool TQXmlContentHandler::startElement( const TQString& namespaceURI, const TQString& localName, const TQString& qName, const TQXmlAttributes& atts )

    The reader calls this function when it has parsed a start element
    tag.

    There is a corresponding endElement() call when the corresponding
    end element tag is read. The startElement() and endElement() calls
    are always nested correctly. Empty element tags (e.g. \c{<x/>})
    cause a startElement() call to be immediately followed by an
    endElement() call.

    The attribute list provided only contains attributes with explicit
    values. The attribute list contains attributes used for namespace
    declaration (i.e. attributes starting with xmlns) only if the
    namespace-prefix property of the reader is true.

    The argument \a namespaceURI is the namespace URI, or
    TQString::null if the element has no namespace URI or if no
    namespace processing is done. \a localName is the local name
    (without prefix), or TQString::null if no namespace processing is
    done, \a qName is the qualified name (with prefix) and \a atts are
    the attributes attached to the element. If there are no
    attributes, \a atts is an empty attributes object.

    If this function returns false the reader stops parsing and
    reports an error. The reader uses the function errorString() to
    get the error message.

    See also the \link xml.html#sax2Namespaces namespace description\endlink.

    \sa endElement()
*/

/*!
    \fn bool TQXmlContentHandler::endElement( const TQString& namespaceURI, const TQString& localName, const TQString& qName )

    The reader calls this function when it has parsed an end element
    tag with the qualified name \a qName, the local name \a localName
    and the namespace URI \a namespaceURI.

    If this function returns false the reader stops parsing and
    reports an error. The reader uses the function errorString() to
    get the error message.

    See also the \link xml.html#sax2Namespaces namespace description\endlink.

    \sa startElement()
*/

/*!
    \fn bool TQXmlContentHandler::characters( const TQString& ch )

    The reader calls this function when it has parsed a chunk of
    character data (either normal character data or character data
    inside a CDATA section; if you need to distinguish between those
    two types you must use TQXmlLexicalHandler::startCDATA() and
    TQXmlLexicalHandler::endCDATA()). The character data is reported in
    \a ch.

    Some readers report whitespace in element content using the
    ignorableWhitespace() function rather than using this one.

    A reader may report the character data of an element in more than
    one chunk; e.g. a reader might want to report "a\<b" in three
    characters() events ("a ", "\<" and " b").

    If this function returns false the reader stops parsing and
    reports an error. The reader uses the function errorString() to
    get the error message.
*/

/*!
    \fn bool TQXmlContentHandler::ignorableWhitespace( const TQString& ch )

    Some readers may use this function to report each chunk of
    whitespace in element content. The whitespace is reported in \a ch.

    If this function returns false the reader stops parsing and
    reports an error. The reader uses the function errorString() to
    get the error message.
*/

/*!
    \fn bool TQXmlContentHandler::processingInstruction( const TQString& target, const TQString& data )

    The reader calls this function when it has parsed a processing
    instruction.

    \a target is the target name of the processing instruction and \a
    data is the data in the processing instruction.

    If this function returns false the reader stops parsing and
    reports an error. The reader uses the function errorString() to
    get the error message.
*/

/*!
    \fn bool TQXmlContentHandler::skippedEntity( const TQString& name )

    Some readers may skip entities if they have not seen the
    declarations (e.g. because they are in an external DTD). If they
    do so they report that they skipped the entity called \a name by
    calling this function.

    If this function returns false the reader stops parsing and
    reports an error. The reader uses the function errorString() to
    get the error message.
*/

/*!
    \fn TQString TQXmlContentHandler::errorString()

    The reader calls this function to get an error string, e.g. if any
    of the handler functions returns false.
*/


/*!
    \class TQXmlErrorHandler ntqxml.h
    \reentrant
    \brief The TQXmlErrorHandler class provides an interface to report
    errors in XML data.
\if defined(commercial)
    It is part of the <a href="commercialeditions.html">TQt Enterprise Edition</a>.
\endif

    \module XML
    \ingroup xml-tools

    If you want your application to report errors to the user or to
    perform customized error handling, you should subclass this class.

    You can set the error handler with TQXmlReader::setErrorHandler().

    Errors can be reported using warning(), error() and fatalError(),
    with the error text being reported with errorString().

    See also the \link xml.html#sax2Intro Introduction to SAX2\endlink.

    \sa TQXmlDTDHandler TQXmlDeclHandler TQXmlContentHandler TQXmlEntityResolver
    TQXmlLexicalHandler
*/

/*!
    \fn bool TQXmlErrorHandler::warning( const TQXmlParseException& exception )

    A reader might use this function to report a warning. Warnings are
    conditions that are not errors or fatal errors as defined by the
    XML 1.0 specification. Details of the warning are stored in \a
    exception.

    If this function returns false the reader stops parsing and
    reports an error. The reader uses the function errorString() to
    get the error message.
*/

/*!
    \fn bool TQXmlErrorHandler::error( const TQXmlParseException& exception )

    A reader might use this function to report a recoverable error. A
    recoverable error corresponds to the definiton of "error" in
    section 1.2 of the XML 1.0 specification. Details of the error are
    stored in \a exception.

    The reader must continue to provide normal parsing events after
    invoking this function.

    If this function returns false the reader stops parsing and
    reports an error. The reader uses the function errorString() to
    get the error message.
*/

/*!
    \fn bool TQXmlErrorHandler::fatalError( const TQXmlParseException& exception )

    A reader must use this function to report a non-recoverable error.
    Details of the error are stored in \a exception.

    If this function returns true the reader might try to go on
    parsing and reporting further errors; but no regular parsing
    events are reported.
*/

/*!
    \fn TQString TQXmlErrorHandler::errorString()

    The reader calls this function to get an error string if any of
    the handler functions returns false.
*/


/*!
    \class TQXmlDTDHandler ntqxml.h
    \reentrant
    \brief The TQXmlDTDHandler class provides an interface to report
    DTD content of XML data.
\if defined(commercial)
    It is part of the <a href="commercialeditions.html">TQt Enterprise Edition</a>.
\endif

    \module XML
    \ingroup xml-tools

    If an application needs information about notations and unparsed
    entities, it can implement this interface and register an instance
    with TQXmlReader::setDTDHandler().

    Note that this interface includes only those DTD events that the
    XML recommendation requires processors to report, i.e. notation
    and unparsed entity declarations using notationDecl() and
    unparsedEntityDecl() respectively.

    See also the \link xml.html#sax2Intro Introduction to SAX2\endlink.

    \sa TQXmlDeclHandler TQXmlContentHandler TQXmlEntityResolver TQXmlErrorHandler
    TQXmlLexicalHandler
*/

/*!
    \fn bool TQXmlDTDHandler::notationDecl( const TQString& name, const TQString& publicId, const TQString& systemId )

    The reader calls this function when it has parsed a notation
    declaration.

    The argument \a name is the notation name, \a publicId is the
    notation's public identifier and \a systemId is the notation's
    system identifier.

    If this function returns false the reader stops parsing and
    reports an error. The reader uses the function errorString() to
    get the error message.
*/

/*!
    \fn bool TQXmlDTDHandler::unparsedEntityDecl( const TQString& name, const TQString& publicId, const TQString& systemId, const TQString& notationName )

    The reader calls this function when it finds an unparsed entity
    declaration.

    The argument \a name is the unparsed entity's name, \a publicId is
    the entity's public identifier, \a systemId is the entity's system
    identifier and \a notationName is the name of the associated
    notation.

    If this function returns false the reader stops parsing and
    reports an error. The reader uses the function errorString() to
    get the error message.
*/

/*!
    \fn TQString TQXmlDTDHandler::errorString()

    The reader calls this function to get an error string if any of
    the handler functions returns false.
*/


/*!
    \class TQXmlEntityResolver ntqxml.h
    \reentrant
    \brief The TQXmlEntityResolver class provides an interface to
    resolve external entities contained in XML data.
\if defined(commercial)
    It is part of the <a href="commercialeditions.html">TQt Enterprise Edition</a>.
\endif

    \module XML
    \ingroup xml-tools

    If an application needs to implement customized handling for
    external entities, it must implement this interface, i.e.
    resolveEntity(), and register it with
    TQXmlReader::setEntityResolver().

    See also the \link xml.html#sax2Intro Introduction to SAX2\endlink.

    \sa TQXmlDTDHandler TQXmlDeclHandler TQXmlContentHandler TQXmlErrorHandler
    TQXmlLexicalHandler
*/

/*!
    \fn bool TQXmlEntityResolver::resolveEntity( const TQString& publicId, const TQString& systemId, TQXmlInputSource*& ret )

    The reader calls this function before it opens any external
    entity, except the top-level document entity. The application may
    request the reader to resolve the entity itself (\a ret is 0) or
    to use an entirely different input source (\a ret points to the
    input source).

    The reader deletes the input source \a ret when it no longer needs
    it, so you should allocate it on the heap with \c new.

    The argument \a publicId is the public identifier of the external
    entity, \a systemId is the system identifier of the external
    entity and \a ret is the return value of this function. If \a ret
    is 0 the reader should resolve the entity itself, if it is
    non-zero it must point to an input source which the reader uses
    instead.

    If this function returns false the reader stops parsing and
    reports an error. The reader uses the function errorString() to
    get the error message.
*/

/*!
    \fn TQString TQXmlEntityResolver::errorString()

    The reader calls this function to get an error string if any of
    the handler functions returns false.
*/


/*!
    \class TQXmlLexicalHandler ntqxml.h
    \reentrant
    \brief The TQXmlLexicalHandler class provides an interface to
    report the lexical content of XML data.
\if defined(commercial)
    It is part of the <a href="commercialeditions.html">TQt Enterprise Edition</a>.
\endif

    \module XML
    \ingroup xml-tools

    The events in the lexical handler apply to the entire document,
    not just to the document element, and all lexical handler events
    appear between the content handler's startDocument and endDocument
    events.

    You can set the lexical handler with
    TQXmlReader::setLexicalHandler().

    This interface's design is based on the the SAX2 extension
    LexicalHandler.

    The interface provides the startDTD(), endDTD(), startEntity(),
    endEntity(), startCDATA(), endCDATA() and comment() functions.

    See also the \link xml.html#sax2Intro Introduction to SAX2\endlink.

    \sa TQXmlDTDHandler TQXmlDeclHandler TQXmlContentHandler TQXmlEntityResolver
    TQXmlErrorHandler
*/

/*!
    \fn bool TQXmlLexicalHandler::startDTD( const TQString& name, const TQString& publicId, const TQString& systemId )

    The reader calls this function to report the start of a DTD
    declaration, if any. It reports the name of the document type in
    \a name, the public identifier in \a publicId and the system
    identifier in \a systemId.

    If the public identifier is missing, \a publicId is set to
    TQString::null. If the system identifier is missing, \a systemId is
    set to TQString::null. Note that it is not valid XML to have a
    public identifier but no system identifier; in such cases a parse
    error will occur.

    All declarations reported through TQXmlDTDHandler or
    TQXmlDeclHandler appear between the startDTD() and endDTD() calls.

    If this function returns false the reader stops parsing and
    reports an error. The reader uses the function errorString() to
    get the error message.

    \sa endDTD()
*/

/*!
    \fn bool TQXmlLexicalHandler::endDTD()

    The reader calls this function to report the end of a DTD
    declaration, if any.

    If this function returns false the reader stops parsing and
    reports an error. The reader uses the function errorString() to
    get the error message.

    \sa startDTD()
*/

/*!
    \fn bool TQXmlLexicalHandler::startEntity( const TQString& name )

    The reader calls this function to report the start of an entity
    called \a name.

    Note that if the entity is unknown, the reader reports it through
    TQXmlContentHandler::skippedEntity() and not through this
    function.

    If this function returns false the reader stops parsing and
    reports an error. The reader uses the function errorString() to
    get the error message.

    \sa endEntity() TQXmlSimpleReader::setFeature()
*/

/*!
    \fn bool TQXmlLexicalHandler::endEntity( const TQString& name )

    The reader calls this function to report the end of an entity
    called \a name.

    For every startEntity() call, there is a corresponding endEntity()
    call. The calls to startEntity() and endEntity() are properly
    nested.

    If this function returns false the reader stops parsing and
    reports an error. The reader uses the function errorString() to
    get the error message.

    \sa startEntity() TQXmlContentHandler::skippedEntity() TQXmlSimpleReader::setFeature()
*/

/*!
    \fn bool TQXmlLexicalHandler::startCDATA()

    The reader calls this function to report the start of a CDATA
    section. The content of the CDATA section is reported through the
    TQXmlContentHandler::characters() function. This function is
    intended only to report the boundary.

    If this function returns false the reader stops parsing and
    reports an error. The reader uses the function errorString() to
    get the error message.

    \sa endCDATA()
*/

/*!
    \fn bool TQXmlLexicalHandler::endCDATA()

    The reader calls this function to report the end of a CDATA
    section.

    If this function returns false the reader stops parsing and reports
    an error. The reader uses the function errorString() to get the error
    message.

    \sa startCDATA() TQXmlContentHandler::characters()
*/

/*!
    \fn bool TQXmlLexicalHandler::comment( const TQString& ch )

    The reader calls this function to report an XML comment anywhere
    in the document. It reports the text of the comment in \a ch.

    If this function returns false the reader stops parsing and
    reports an error. The reader uses the function errorString() to
    get the error message.
*/

/*!
    \fn TQString TQXmlLexicalHandler::errorString()

    The reader calls this function to get an error string if any of
    the handler functions returns false.
*/


/*!
    \class TQXmlDeclHandler ntqxml.h
    \reentrant
    \brief The TQXmlDeclHandler class provides an interface to report declaration
    content of XML data.
\if defined(commercial)
    It is part of the <a href="commercialeditions.html">TQt Enterprise Edition</a>.
\endif

    \module XML
    \ingroup xml-tools

    You can set the declaration handler with
    TQXmlReader::setDeclHandler().

    This interface is based on the SAX2 extension DeclHandler.

    The interface provides attributeDecl(), internalEntityDecl() and
    externalEntityDecl() functions.

    See also the \link xml.html#sax2Intro Introduction to SAX2\endlink.

    \sa TQXmlDTDHandler TQXmlContentHandler TQXmlEntityResolver TQXmlErrorHandler
    TQXmlLexicalHandler
*/

/*!
    \fn bool TQXmlDeclHandler::attributeDecl( const TQString& eName, const TQString& aName, const TQString& type, const TQString& valueDefault, const TQString& value )

    The reader calls this function to report an attribute type
    declaration. Only the effective (first) declaration for an
    attribute is reported.

    The reader passes the name of the associated element in \a eName
    and the name of the attribute in \a aName. It passes a string that
    represents the attribute type in \a type and a string that
    represents the attribute default in \a valueDefault. This string
    is one of "#IMPLIED", "#REQUIRED", "#FIXED" or TQString::null (if
    none of the others applies). The reader passes the attribute's
    default value in \a value. If no default value is specified in the
    XML file, \a value is TQString::null.

    If this function returns false the reader stops parsing and
    reports an error. The reader uses the function errorString() to
    get the error message.
*/

/*!
    \fn bool TQXmlDeclHandler::internalEntityDecl( const TQString& name, const TQString& value )

    The reader calls this function to report an internal entity
    declaration. Only the effective (first) declaration is reported.

    The reader passes the name of the entity in \a name and the value
    of the entity in \a value.

    If this function returns false the reader stops parsing and
    reports an error. The reader uses the function errorString() to
    get the error message.
*/

/*!
    \fn bool TQXmlDeclHandler::externalEntityDecl( const TQString& name, const TQString& publicId, const TQString& systemId )

    The reader calls this function to report a parsed external entity
    declaration. Only the effective (first) declaration for each
    entity is reported.

    The reader passes the name of the entity in \a name, the public
    identifier in \a publicId and the system identifier in \a
    systemId. If there is no public identifier specified, it passes
    TQString::null in \a publicId.

    If this function returns false the reader stops parsing and
    reports an error. The reader uses the function errorString() to
    get the error message.
*/

/*!
    \fn TQString TQXmlDeclHandler::errorString()

    The reader calls this function to get an error string if any of
    the handler functions returns false.
*/


/*!
    \class TQXmlDefaultHandler ntqxml.h
    \reentrant
    \brief The TQXmlDefaultHandler class provides a default implementation of all
    the XML handler classes.
\if defined(commercial)
    It is part of the <a href="commercialeditions.html">TQt Enterprise Edition</a>.
\endif

    \module XML
    \ingroup xml-tools

    Very often we are only interested in parts of the things that the
    reader reports. This class implements a default behaviour for the
    handler classes (i.e. most of the time do nothing). Usually this
    is the class you subclass for implementing your own customized
    handler.

    See also the \link xml.html#sax2Intro Introduction to SAX2\endlink.

    \sa TQXmlDTDHandler TQXmlDeclHandler TQXmlContentHandler TQXmlEntityResolver
    TQXmlErrorHandler TQXmlLexicalHandler
*/

/*!
    \fn TQXmlDefaultHandler::TQXmlDefaultHandler()

    Constructor.
*/
/*!
    \fn TQXmlDefaultHandler::~TQXmlDefaultHandler()

    Destructor.
*/

/*!
    \reimp

    Does nothing.
*/
void TQXmlDefaultHandler::setDocumentLocator( TQXmlLocator* )
{
}

/*!
    \reimp

    Does nothing.
*/
bool TQXmlDefaultHandler::startDocument()
{
    return true;
}

/*!
    \reimp

    Does nothing.
*/
bool TQXmlDefaultHandler::endDocument()
{
    return true;
}

/*!
    \reimp

    Does nothing.
*/
bool TQXmlDefaultHandler::startPrefixMapping( const TQString&, const TQString& )
{
    return true;
}

/*!
    \reimp

    Does nothing.
*/
bool TQXmlDefaultHandler::endPrefixMapping( const TQString& )
{
    return true;
}

/*!
    \reimp

    Does nothing.
*/
bool TQXmlDefaultHandler::startElement( const TQString&, const TQString&,
	const TQString&, const TQXmlAttributes& )
{
    return true;
}

/*!
    \reimp

    Does nothing.
*/
bool TQXmlDefaultHandler::endElement( const TQString&, const TQString&,
	const TQString& )
{
    return true;
}

/*!
    \reimp

    Does nothing.
*/
bool TQXmlDefaultHandler::characters( const TQString& )
{
    return true;
}

/*!
    \reimp

    Does nothing.
*/
bool TQXmlDefaultHandler::ignorableWhitespace( const TQString& )
{
    return true;
}

/*!
    \reimp

    Does nothing.
*/
bool TQXmlDefaultHandler::processingInstruction( const TQString&,
	const TQString& )
{
    return true;
}

/*!
    \reimp

    Does nothing.
*/
bool TQXmlDefaultHandler::skippedEntity( const TQString& )
{
    return true;
}

/*!
    \reimp

    Does nothing.
*/
bool TQXmlDefaultHandler::warning( const TQXmlParseException& )
{
    return true;
}

/*!
    \reimp

    Does nothing.
*/
bool TQXmlDefaultHandler::error( const TQXmlParseException& )
{
    return true;
}

/*!
    \reimp

    Does nothing.
*/
bool TQXmlDefaultHandler::fatalError( const TQXmlParseException& )
{
    return true;
}

/*!
    \reimp

    Does nothing.
*/
bool TQXmlDefaultHandler::notationDecl( const TQString&, const TQString&,
	const TQString& )
{
    return true;
}

/*!
    \reimp

    Does nothing.
*/
bool TQXmlDefaultHandler::unparsedEntityDecl( const TQString&, const TQString&,
	const TQString&, const TQString& )
{
    return true;
}

/*!
    \reimp

    Sets \a ret to 0, so that the reader uses the system identifier
    provided in the XML document.
*/
bool TQXmlDefaultHandler::resolveEntity( const TQString&, const TQString&,
	TQXmlInputSource*& ret )
{
    ret = 0;
    return true;
}

/*!
    \reimp

    Returns the default error string.
*/
TQString TQXmlDefaultHandler::errorString()
{
    return TQString( XMLERR_ERRORBYCONSUMER );
}

/*!
    \reimp

    Does nothing.
*/
bool TQXmlDefaultHandler::startDTD( const TQString&, const TQString&, const TQString& )
{
    return true;
}

/*!
    \reimp

    Does nothing.
*/
bool TQXmlDefaultHandler::endDTD()
{
    return true;
}

/*!
    \reimp

    Does nothing.
*/
bool TQXmlDefaultHandler::startEntity( const TQString& )
{
    return true;
}

/*!
    \reimp

    Does nothing.
*/
bool TQXmlDefaultHandler::endEntity( const TQString& )
{
    return true;
}

/*!
    \reimp

    Does nothing.
*/
bool TQXmlDefaultHandler::startCDATA()
{
    return true;
}

/*!
    \reimp

    Does nothing.
*/
bool TQXmlDefaultHandler::endCDATA()
{
    return true;
}

/*!
    \reimp

    Does nothing.
*/
bool TQXmlDefaultHandler::comment( const TQString& )
{
    return true;
}

/*!
    \reimp

    Does nothing.
*/
bool TQXmlDefaultHandler::attributeDecl( const TQString&, const TQString&, const TQString&, const TQString&, const TQString& )
{
    return true;
}

/*!
    \reimp

    Does nothing.
*/
bool TQXmlDefaultHandler::internalEntityDecl( const TQString&, const TQString& )
{
    return true;
}

/*!
    \reimp

    Does nothing.
*/
bool TQXmlDefaultHandler::externalEntityDecl( const TQString&, const TQString&, const TQString& )
{
    return true;
}


/*********************************************
 *
 * TQXmlSimpleReaderPrivate
 *
 *********************************************/

class TQXmlSimpleReaderPrivate
{
private:
    // functions
    inline TQXmlSimpleReaderPrivate()
    {
	parseStack = 0;
        undefEntityInAttrHack = false;
    }

    inline ~TQXmlSimpleReaderPrivate()
    {
	delete parseStack;
    }

    inline void initIncrementalParsing()
    {
	delete parseStack;
	parseStack = new TQValueStack<ParseState>;
    }

    // used to determine if elements are correctly nested
    TQValueStack<TQString> tags;

    // used for entity declarations
    struct ExternParameterEntity
    {
	ExternParameterEntity( ) {}
	ExternParameterEntity( const TQString &p, const TQString &s )
	    : publicId(p), systemId(s) {}
	TQString publicId;
	TQString systemId;
    };
    struct ExternEntity
    {
	ExternEntity( ) {}
	ExternEntity( const TQString &p, const TQString &s, const TQString &n )
	    : publicId(p), systemId(s), notation(n) {}
	TQString publicId;
	TQString systemId;
	TQString notation;
    };
    TQMap<TQString,ExternParameterEntity> externParameterEntities;
    TQMap<TQString,TQString> parameterEntities;
    TQMap<TQString,ExternEntity> externEntities;
    TQMap<TQString,TQString> entities;

    // used for parsing of entity references
    TQValueStack<TQString> xmlRef;
    TQValueStack<TQString> xmlRefName;

    // used for standalone declaration
    enum Standalone { Yes, No, Unknown };

    TQString doctype; // only used for the doctype
    TQString xmlVersion; // only used to store the version information
    TQString encoding; // only used to store the encoding
    Standalone standalone; // used to store the value of the standalone declaration

    TQString publicId; // used by parseExternalID() to store the public ID
    TQString systemId; // used by parseExternalID() to store the system ID
    TQString attDeclEName; // use by parseAttlistDecl()
    TQString attDeclAName; // use by parseAttlistDecl()

    // flags for some features support
    bool useNamespaces;
    bool useNamespacePrefixes;
    bool reportWhitespaceCharData;
    bool reportEntities;

    // used to build the attribute list
    TQXmlAttributes attList;

    // used in TQXmlSimpleReader::parseContent() to decide whether character
    // data was read
    bool contentCharDataRead;

    // helper classes
    TQXmlLocator *locator;
    TQXmlNamespaceSupport namespaceSupport;

    // error string
    TQString error;

    // arguments for parse functions (this is needed to allow incremental
    // parsing)
    bool parsePI_xmldecl;
    bool parseName_useRef;
    bool parseReference_charDataRead;
    TQXmlSimpleReader::EntityRecognitionContext parseReference_context;
    bool parseExternalID_allowPublicID;
    TQXmlSimpleReader::EntityRecognitionContext parsePEReference_context;
    TQString parseString_s;

    // for incremental parsing
    struct ParseState {
	typedef bool (TQXmlSimpleReader::*ParseFunction) ();
	ParseFunction function;
	int state;
    };
    TQValueStack<ParseState> *parseStack;

    // used in parseProlog()
    bool xmldecl_possible;
    bool doctype_read;

    // used in parseDoctype()
    bool startDTDwasReported;

    // used in parseString()
    signed char Done;

    bool undefEntityInAttrHack;

    int nameValueLen;
    int refValueLen;
    int stringValueLen;

    // friend declarations
    friend class TQXmlSimpleReader;
};


/*********************************************
 *
 * TQXmlSimpleReader
 *
 *********************************************/

/*!
    \class TQXmlReader ntqxml.h
    \reentrant
    \brief The TQXmlReader class provides an interface for XML readers (i.e.
    parsers).
\if defined(commercial)
    It is part of the <a href="commercialeditions.html">TQt Enterprise Edition</a>.
\endif

    \module XML
    \ingroup xml-tools

    This abstract class provides an interface for all of TQt's XML
    readers. Currently there is only one implementation of a reader
    included in TQt's XML module: TQXmlSimpleReader. In future releases
    there might be more readers with different properties available
    (e.g. a validating parser).

    The design of the XML classes follows the \link
    http://www.saxproject.org/ SAX2 Java interface\endlink, with
    the names adapted to fit TQt naming conventions. It should be very
    easy for anybody who has worked with SAX2 to get started with the
    TQt XML classes.

    All readers use the class TQXmlInputSource to read the input
    document. Since you are normally interested in particular content
    in the XML document, the reader reports the content through
    special handler classes (TQXmlDTDHandler, TQXmlDeclHandler,
    TQXmlContentHandler, TQXmlEntityResolver, TQXmlErrorHandler and
    TQXmlLexicalHandler), which you must subclass, if you want to
    process the contents.

    Since the handler classes only describe interfaces you must
    implement all the functions. We provide the TQXmlDefaultHandler
    class to make this easier: it implements a default behaviour (do
    nothing) for all functions, so you can subclass it and just
    implement the functions you are interested in.

    Features and properties of the reader can be set with setFeature()
    and setProperty() respectively. You can set the reader to use your
    own subclasses with setEntityResolver(), setDTDHandler(),
    setContentHandler(), setErrorHandler(), setLexicalHandler() and
    setDeclHandler(). The parse itself is started with a call to
    parse().

    \sa TQXmlSimpleReader
*/

/*!
    \fn bool TQXmlReader::feature( const TQString& name, bool *ok ) const

    If the reader has the feature called \a name, the feature's value
    is returned. If no such feature exists the return value is
    undefined.

    If \a ok is not 0: \a *ok  is set to true if the reader has the
    feature called \a name; otherwise \a *ok is set to false.

    \sa setFeature() hasFeature()
*/

/*!
    \fn void TQXmlReader::setFeature( const TQString& name, bool value )

    Sets the feature called \a name to the given \a value. If the
    reader doesn't have the feature nothing happens.

    \sa feature() hasFeature()
*/

/*!
    \fn bool TQXmlReader::hasFeature( const TQString& name ) const

    Returns \c true if the reader has the feature called \a name;
    otherwise returns false.

    \sa feature() setFeature()
*/

/*!
    \fn void* TQXmlReader::property( const TQString& name, bool *ok ) const

    If the reader has the property \a name, this function returns the
    value of the property; otherwise the return value is undefined.

    If \a ok is not 0: if the reader has the \a name property \a *ok
    is set to true; otherwise \a *ok is set to false.

    \sa setProperty() hasProperty()
*/

/*!
    \fn void TQXmlReader::setProperty( const TQString& name, void* value )

    Sets the property \a name to \a value. If the reader doesn't have
    the property nothing happens.

    \sa property() hasProperty()
*/

/*!
    \fn bool TQXmlReader::hasProperty( const TQString& name ) const

    Returns true if the reader has the property \a name; otherwise
    returns false.

    \sa property() setProperty()
*/

/*!
    \fn void TQXmlReader::setEntityResolver( TQXmlEntityResolver* handler )

    Sets the entity resolver to \a handler.

    \sa entityResolver()
*/

/*!
    \fn TQXmlEntityResolver* TQXmlReader::entityResolver() const

    Returns the entity resolver or 0 if none was set.

    \sa setEntityResolver()
*/

/*!
    \fn void TQXmlReader::setDTDHandler( TQXmlDTDHandler* handler )

    Sets the DTD handler to \a handler.

    \sa DTDHandler()
*/

/*!
    \fn TQXmlDTDHandler* TQXmlReader::DTDHandler() const

    Returns the DTD handler or 0 if none was set.

    \sa setDTDHandler()
*/

/*!
    \fn void TQXmlReader::setContentHandler( TQXmlContentHandler* handler )

    Sets the content handler to \a handler.

    \sa contentHandler()
*/

/*!
    \fn TQXmlContentHandler* TQXmlReader::contentHandler() const

    Returns the content handler or 0 if none was set.

    \sa setContentHandler()
*/

/*!
    \fn void TQXmlReader::setErrorHandler( TQXmlErrorHandler* handler )

    Sets the error handler to \a handler. Clears the error handler if
    \a handler is 0.

    \sa errorHandler()
*/

/*!
    \fn TQXmlErrorHandler* TQXmlReader::errorHandler() const

    Returns the error handler or 0 if none is set.

    \sa setErrorHandler()
*/

/*!
    \fn void TQXmlReader::setLexicalHandler( TQXmlLexicalHandler* handler )

    Sets the lexical handler to \a handler.

    \sa lexicalHandler()
*/

/*!
    \fn TQXmlLexicalHandler* TQXmlReader::lexicalHandler() const

    Returns the lexical handler or 0 if none was set.

    \sa setLexicalHandler()
*/

/*!
    \fn void TQXmlReader::setDeclHandler( TQXmlDeclHandler* handler )

    Sets the declaration handler to \a handler.

    \sa declHandler()
*/

/*!
    \fn TQXmlDeclHandler* TQXmlReader::declHandler() const

    Returns the declaration handler or 0 if none was set.

    \sa setDeclHandler()
*/

/*!
  \fn bool TQXmlReader::parse( const TQXmlInputSource& input )

  \obsolete
*/

/*!
    \fn bool TQXmlReader::parse( const TQXmlInputSource* input )

    Reads an XML document from \a input and parses it. Returns true if
    the parsing was successful; otherwise returns false.
*/


/*!
    \class TQXmlSimpleReader ntqxml.h
    \reentrant
    \brief The TQXmlSimpleReader class provides an implementation of a
    simple XML reader (parser).
\if defined(commercial)
    It is part of the <a href="commercialeditions.html">TQt Enterprise Edition</a>.
\endif

    \module XML
    \ingroup xml-tools
    \mainclass

    This XML reader is sufficient for simple parsing tasks. The reader:
    \list
    \i provides a well-formed parser;
    \i does not parse any external entities;
    \i can do namespace processing.
    \endlist

    Documents are parsed with a call to parse().

*/

static inline bool is_S(TQChar ch)
{
    ushort uc = ch.unicode();
    return (uc == ' ' || uc == '\t' || uc == '\n' || uc == '\r');
}

enum NameChar { NameBeginning, NameNotBeginning, NotName };

static const char Begi = (char)NameBeginning;
static const char NtBg = (char)NameNotBeginning;
static const char NotN = (char)NotName;

static const char nameCharTable[128] =
{
// 0x00
    NotN, NotN, NotN, NotN, NotN, NotN, NotN, NotN,
    NotN, NotN, NotN, NotN, NotN, NotN, NotN, NotN,
// 0x10
    NotN, NotN, NotN, NotN, NotN, NotN, NotN, NotN,
    NotN, NotN, NotN, NotN, NotN, NotN, NotN, NotN,
// 0x20 (0x2D is '-', 0x2E is '.')
    NotN, NotN, NotN, NotN, NotN, NotN, NotN, NotN,
    NotN, NotN, NotN, NotN, NotN, NtBg, NtBg, NotN,
// 0x30 (0x30..0x39 are '0'..'9', 0x3A is ':')
    NtBg, NtBg, NtBg, NtBg, NtBg, NtBg, NtBg, NtBg,
    NtBg, NtBg, Begi, NotN, NotN, NotN, NotN, NotN,
// 0x40 (0x41..0x5A are 'A'..'Z')
    NotN, Begi, Begi, Begi, Begi, Begi, Begi, Begi,
    Begi, Begi, Begi, Begi, Begi, Begi, Begi, Begi,
// 0x50 (0x5F is '_')
    Begi, Begi, Begi, Begi, Begi, Begi, Begi, Begi,
    Begi, Begi, Begi, NotN, NotN, NotN, NotN, Begi,
// 0x60 (0x61..0x7A are 'a'..'z')
    NotN, Begi, Begi, Begi, Begi, Begi, Begi, Begi,
    Begi, Begi, Begi, Begi, Begi, Begi, Begi, Begi,
// 0x70
    Begi, Begi, Begi, Begi, Begi, Begi, Begi, Begi,
    Begi, Begi, Begi, NotN, NotN, NotN, NotN, NotN
};

static inline NameChar fastDetermineNameChar(TQChar ch)
{
    ushort uc = ch.unicode();
    if (!(uc & ~0x7f)) // uc < 128
        return (NameChar)nameCharTable[uc];

    TQChar::Category cat = ch.category();
    // ### some these categories might be slightly wrong
    if ((cat >= TQChar::Letter_Uppercase && cat <= TQChar::Letter_Other)
        || cat == TQChar::Number_Letter)
        return NameBeginning;
    if ((cat >= TQChar::Number_DecimalDigit && cat <= TQChar::Number_Other)
                || (cat >= TQChar::Mark_NonSpacing && cat <= TQChar::Mark_Enclosing))
        return NameNotBeginning;
    return NotName;
}

static NameChar determineNameChar(TQChar ch)
{
    return fastDetermineNameChar(ch);
}

inline void TQXmlSimpleReader::nameClear()
{
    d->nameValueLen = 0; nameArrayPos = 0;
}

inline void TQXmlSimpleReader::refClear()
{
    d->refValueLen = 0; refArrayPos = 0;
}

/*!
    Constructs a simple XML reader with the following feature settings:
    \table
    \header \i Feature \i Setting
    \row \i \e http://xml.org/sax/features/namespaces \i true
    \row \i \e http://xml.org/sax/features/namespace-prefixes \i false
    \row \i \e http://trolltech.com/xml/features/report-whitespace-only-CharData
	 \i true
    \row \i \e http://trolltech.com/xml/features/report-start-end-entity \i false
    \endtable

    More information about features can be found in the \link
    xml.html#sax2Features TQt SAX2 overview. \endlink

    \sa setFeature()
*/

TQXmlSimpleReader::TQXmlSimpleReader()
{
    d = new TQXmlSimpleReaderPrivate();
    d->locator = new TQXmlSimpleReaderLocator( this );

    entityRes  = 0;
    dtdHnd     = 0;
    contentHnd = 0;
    errorHnd   = 0;
    lexicalHnd = 0;
    declHnd    = 0;

    // default feature settings
    d->useNamespaces = true;
    d->useNamespacePrefixes = false;
    d->reportWhitespaceCharData = true;
    d->reportEntities = false;
}

/*!
    Destroys the simple XML reader.
*/
TQXmlSimpleReader::~TQXmlSimpleReader()
{
    delete d->locator;
    delete d;
}

/*!
    \reimp
*/
bool TQXmlSimpleReader::feature( const TQString& name, bool *ok ) const
{
    if ( ok != 0 )
	*ok = true;
    if        ( name == "http://xml.org/sax/features/namespaces" ) {
	return d->useNamespaces;
    } else if ( name == "http://xml.org/sax/features/namespace-prefixes" ) {
	return d->useNamespacePrefixes;
    } else if ( name == "http://trolltech.com/xml/features/report-whitespace-only-CharData" ) {
	return d->reportWhitespaceCharData;
    } else if ( name == "http://trolltech.com/xml/features/report-start-end-entity" ) {
	return d->reportEntities;
    } else {
	tqWarning( "Unknown feature %s", name.latin1() );
	if ( ok != 0 )
	    *ok = false;
    }
    return false;
}

/*!
    Sets the state of the feature \a name to \a value:

    If the feature is not recognized, it is ignored.

    The following features are supported:
    \table
    \header \i Feature \i Notes
    \row \i \e http://xml.org/sax/features/namespaces
         \i If this feature is true, namespace processing is
     performed.
    \row \i \e http://xml.org/sax/features/namespace-prefixes
         \i If this feature is true, the the original prefixed names
            and attributes used for namespace declarations are
            reported.
    \row \i \e http://trolltech.com/xml/features/report-whitespace-only-CharData
         \i If this feature is true, CharData that only contain
            whitespace are not ignored, but are reported via
            TQXmlContentHandler::characters().
    \row \i \e http://trolltech.com/xml/features/report-start-end-entity
         \i If this feature is true, the parser reports
            TQXmlContentHandler::startEntity() and
            TQXmlContentHandler::endEntity() events. So character data
            might be reported in chunks. If this feature is false, the
            parser does not report those events, but rather silently
            substitutes the entities and reports the character data in
            one chunk.
    \endtable

    \quotefile xml/tagreader-with-features/tagreader.cpp
    \skipto reader
    \printline reader
    \skipto setFeature
    \printline setFeature
    \printline true

    (Code taken from xml/tagreader-with-features/tagreader.cpp)

    \sa feature() hasFeature()
*/
void TQXmlSimpleReader::setFeature( const TQString& name, bool value )
{
    if        ( name == "http://xml.org/sax/features/namespaces" ) {
	d->useNamespaces = value;
    } else if ( name == "http://xml.org/sax/features/namespace-prefixes" ) {
	d->useNamespacePrefixes = value;
    } else if ( name == "http://trolltech.com/xml/features/report-whitespace-only-CharData" ) {
	d->reportWhitespaceCharData = value;
    } else if ( name == "http://trolltech.com/xml/features/report-start-end-entity" ) {
	d->reportEntities = value;
    } else {
	tqWarning( "Unknown feature %s", name.latin1() );
    }
}

/*! \reimp
*/
bool TQXmlSimpleReader::hasFeature( const TQString& name ) const
{
    if ( name == "http://xml.org/sax/features/namespaces"
	    || name == "http://xml.org/sax/features/namespace-prefixes"
	    || name == "http://trolltech.com/xml/features/report-whitespace-only-CharData"
	    || name == "http://trolltech.com/xml/features/report-start-end-entity" ) {
	return true;
    } else {
	return false;
    }
}

/*! \reimp
*/
void* TQXmlSimpleReader::property( const TQString&, bool *ok ) const
{
    if ( ok != 0 )
	*ok = false;
    return 0;
}

/*! \reimp
*/
void TQXmlSimpleReader::setProperty( const TQString&, void* )
{
}

/*!
    \reimp
*/
bool TQXmlSimpleReader::hasProperty( const TQString& ) const
{
    return false;
}

/*!
    \reimp
*/
void TQXmlSimpleReader::setEntityResolver( TQXmlEntityResolver* handler )
{ entityRes = handler; }

/*!
    \reimp
*/
TQXmlEntityResolver* TQXmlSimpleReader::entityResolver() const
{ return entityRes; }

/*!
    \reimp
*/
void TQXmlSimpleReader::setDTDHandler( TQXmlDTDHandler* handler )
{ dtdHnd = handler; }

/*!
    \reimp
*/
TQXmlDTDHandler* TQXmlSimpleReader::DTDHandler() const
{ return dtdHnd; }

/*!
    \reimp
*/
void TQXmlSimpleReader::setContentHandler( TQXmlContentHandler* handler )
{ contentHnd = handler; }

/*!
    \reimp
*/
TQXmlContentHandler* TQXmlSimpleReader::contentHandler() const
{ return contentHnd; }

/*!
    \reimp
*/
void TQXmlSimpleReader::setErrorHandler( TQXmlErrorHandler* handler )
{ errorHnd = handler; }

/*!
    \reimp
*/
TQXmlErrorHandler* TQXmlSimpleReader::errorHandler() const
{ return errorHnd; }

/*!
    \reimp
*/
void TQXmlSimpleReader::setLexicalHandler( TQXmlLexicalHandler* handler )
{ lexicalHnd = handler; }

/*!
    \reimp
*/
TQXmlLexicalHandler* TQXmlSimpleReader::lexicalHandler() const
{ return lexicalHnd; }

/*!
    \reimp
*/
void TQXmlSimpleReader::setDeclHandler( TQXmlDeclHandler* handler )
{ declHnd = handler; }

/*!
    \reimp
*/
TQXmlDeclHandler* TQXmlSimpleReader::declHandler() const
{ return declHnd; }



/*!
    \reimp
*/
bool TQXmlSimpleReader::parse( const TQXmlInputSource& input )
{
    return parse( &input, false );
}

/*!
    \reimp
*/
bool TQXmlSimpleReader::parse( const TQXmlInputSource* input )
{
    return parse( input, false );
}

/*!
    Reads an XML document from \a input and parses it. Returns false
    if the parsing detects an error; otherwise returns true.

    If \a incremental is true, the parser does not return false when
    it reaches the end of the \a input without reaching the end of the
    XML file. Instead it stores the state of the parser so that
    parsing can be continued at a later stage when more data is
    available. You can use the function parseContinue() to continue
    with parsing. This class stores a pointer to the input source \a
    input and the parseContinue() function tries to read from that
    input souce. This means that you should not delete the input
    source \a input until you've finished your calls to
    parseContinue(). If you call this function with \a incremental
    true whilst an incremental parse is in progress a new parsing
    session will be started and the previous session lost.

    If \a incremental is false, this function behaves like the normal
    parse function, i.e. it returns false when the end of input is
    reached without reaching the end of the XML file and the parsing
    cannot be continued.

    \sa parseContinue() TQSocket
*/
bool TQXmlSimpleReader::parse( const TQXmlInputSource *input, bool incremental )
{
    init( input );
    if ( incremental ) {
	d->initIncrementalParsing();
    } else {
	delete d->parseStack;
	d->parseStack = 0;
    }
    // call the handler
    if ( contentHnd ) {
	contentHnd->setDocumentLocator( d->locator );
	if ( !contentHnd->startDocument() ) {
	    reportParseError( contentHnd->errorString() );
	    d->tags.clear();
	    return false;
	}
    }
    return parseBeginOrContinue( 0, incremental );
}

/*!
    Continues incremental parsing; this function reads the input from
    the TQXmlInputSource that was specified with the last parse()
    command. To use this function, you \e must have called parse()
    with the incremental argument set to true.

    Returns false if a parsing error occurs; otherwise returns true.

    If the input source returns an empty string for the function
    TQXmlInputSource::data(), then this means that the end of the XML
    file has been reached; this is quite important, especially if you
    want to use the reader to parse more than one XML file.

    The case of the end of the XML file being reached without having
    finished parsing is not considered to be an error: you can
    continue parsing at a later stage by calling this function again
    when there is more data available to parse.

    This function assumes that the end of the XML document is reached
    if the TQXmlInputSource::next() function returns
    TQXmlInputSource::EndOfDocument. If the parser has not finished
    parsing when it encounters this symbol, it is an error and false
    is returned.

    \sa parse() TQXmlInputSource::next()
*/
bool TQXmlSimpleReader::parseContinue()
{
    if ( d->parseStack == 0 || d->parseStack->isEmpty() )
	return false;
    initData();
    int state = d->parseStack->pop().state;
    return parseBeginOrContinue( state, true );
}

/*
  Common part of parse() and parseContinue()
*/
bool TQXmlSimpleReader::parseBeginOrContinue( int state, bool incremental )
{
    bool atEndOrig = atEnd();

    if ( state==0 ) {
	if ( !parseProlog() ) {
	    if ( incremental && d->error.isNull() ) {
		pushParseState( 0, 0 );
		return true;
	    } else {
		d->tags.clear();
		return false;
	    }
	}
	state = 1;
    }
    if ( state==1 ) {
	if ( !parseElement() ) {
	    if ( incremental && d->error.isNull() ) {
		pushParseState( 0, 1 );
		return true;
	    } else {
		d->tags.clear();
		return false;
	    }
	}
	state = 2;
    }
    // parse Misc*
    while ( !atEnd() ) {
	if ( !parseMisc() ) {
	    if ( incremental && d->error.isNull() ) {
		pushParseState( 0, 2 );
		return true;
	    } else {
		d->tags.clear();
		return false;
	    }
	}
    }
    if ( !atEndOrig && incremental ) {
	// we parsed something at all, so be prepared to come back later
	pushParseState( 0, 2 );
	return true;
    }
    // is stack empty?
    if ( !d->tags.isEmpty() && !d->error.isNull() ) {
	reportParseError( XMLERR_UNEXPECTEDEOF );
	d->tags.clear();
	return false;
    }
    // call the handler
    if ( contentHnd ) {
	delete d->parseStack;
	d->parseStack = 0;
	if ( !contentHnd->endDocument() ) {
	    reportParseError( contentHnd->errorString() );
	    return false;
	}
    }
    return true;
}

//
// The following private parse functions have another semantics for the return
// value: They return true iff parsing has finished successfully (i.e. the end
// of the XML file must be reached!). If one of these functions return false,
// there is only an error when d->error.isNULL() is also false.
//

/*
  For the incremental parsing, it is very important that the parse...()
  functions have a certain structure. Since it might be hard to understand how
  they work, here is a description of the layout of these functions:

    bool TQXmlSimpleReader::parse...()
    {
(1)	const signed char Init             = 0;
	...

(2)	const signed char Inp...           = 0;
	...

(3)	static signed char table[3][2] = {
	...
	};
	signed char state;
	signed char input;

(4)        if (d->parseStack == 0 || d->parseStack->isEmpty()) {
(4a)	...
	} else {
(4b)	...
	}

	for ( ; ; ) {
(5)	    switch ( state ) {
	    ...
	    }

(6)
(6a)	    if ( atEnd() ) {
		unexpectedEof( &TQXmlSimpleReader::parseNmtoken, state );
		return false;
	    }
(6b)        if (determineNameChar(c) != NotName) {
	    ...
	    }
(7)	    state = table[state][input];

(8)	    switch ( state ) {
	    ...
	    }
	}
    }

  Explanation:
  ad 1: constants for the states (used in the transition table)
  ad 2: constants for the input (used in the transition table)
  ad 3: the transition table for the state machine
  ad 4: test if we are in a parseContinue() step
	a) if no, do inititalizations
	b) if yes, restore the state and call parse functions recursively
  ad 5: Do some actions according to the state; from the logical execution
	order, this code belongs after 8 (see there for an explanation)
  ad 6: Check the character that is at the actual "cursor" position:
	a) If we reached the EOF, report either error or push the state (in the
	   case of incremental parsing).
	b) Otherwise, set the input character constant for the transition
	   table.
  ad 7: Get the new state according to the input that was read.
  ad 8: Do some actions according to the state. The last line in every case
	statement reads new data (i.e. it move the cursor). This can also be
	done by calling another parse...() funtion. If you need processing for
	this state after that, you have to put it into the switch statement 5.
	This ensures that you have a well defined re-entry point, when you ran
	out of data.
*/

/*
  Parses the prolog [22].
*/
bool TQXmlSimpleReader::parseProlog()
{
    const signed char Init             = 0;
    const signed char EatWS            = 1; // eat white spaces
    const signed char Lt               = 2; // '<' read
    const signed char Em               = 3; // '!' read
    const signed char DocType          = 4; // read doctype
    const signed char Comment          = 5; // read comment
    const signed char CommentR         = 6; // same as Comment, but already reported
    const signed char PInstr           = 7; // read PI
    const signed char PInstrR          = 8; // same as PInstr, but already reported
    const signed char Done             = 9;

    const signed char InpWs            = 0;
    const signed char InpLt            = 1; // <
    const signed char InpQm            = 2; // ?
    const signed char InpEm            = 3; // !
    const signed char InpD             = 4; // D
    const signed char InpDash          = 5; // -
    const signed char InpUnknown       = 6;

    static const signed char table[9][7] = {
     /*  InpWs   InpLt  InpQm  InpEm  InpD      InpDash  InpUnknown */
	{ EatWS,  Lt,    -1,    -1,    -1,       -1,       -1      }, // Init
	{ -1,     Lt,    -1,    -1,    -1,       -1,       -1      }, // EatWS
	{ -1,     -1,    PInstr,Em,    Done,     -1,       Done    }, // Lt
	{ -1,     -1,    -1,    -1,    DocType,  Comment,  -1      }, // Em
	{ EatWS,  Lt,    -1,    -1,    -1,       -1,       -1      }, // DocType
	{ EatWS,  Lt,    -1,    -1,    -1,       -1,       -1      }, // Comment
	{ EatWS,  Lt,    -1,    -1,    -1,       -1,       -1      }, // CommentR
	{ EatWS,  Lt,    -1,    -1,    -1,       -1,       -1      }, // PInstr
	{ EatWS,  Lt,    -1,    -1,    -1,       -1,       -1      }  // PInstrR
    };
    signed char state;
    signed char input;

    if ( d->parseStack==0 || d->parseStack->isEmpty() ) {
	d->xmldecl_possible = true;
	d->doctype_read = false;
	state = Init;
    } else {
        state = d->parseStack->pop().state;
#if defined(QT_QXML_DEBUG)
	tqDebug( "TQXmlSimpleReader: parseProlog (cont) in state %d", state );
#endif
	if ( !d->parseStack->isEmpty() ) {
            ParseFunction function = d->parseStack->top().function;
	    if ( function == &TQXmlSimpleReader::eat_ws ) {
		d->parseStack->pop();
#if defined(QT_QXML_DEBUG)
		tqDebug( "TQXmlSimpleReader: eat_ws (cont)" );
#endif
	    }
	    if ( !(this->*function)() ) {
		parseFailed( &TQXmlSimpleReader::parseProlog, state );
		return false;
	    }
	}
    }

    for (;;) {
	switch ( state ) {
	    case DocType:
		if ( d->doctype_read ) {
		    reportParseError( XMLERR_MORETHANONEDOCTYPE );
		    return false;
		} else {
		    d->doctype_read = false;
		}
		break;
	    case Comment:
		if ( lexicalHnd ) {
		    if ( !lexicalHnd->comment( string() ) ) {
			reportParseError( lexicalHnd->errorString() );
			return false;
		    }
		}
		state = CommentR;
		break;
	    case PInstr:
		// call the handler
		if ( contentHnd ) {
		    if ( d->xmldecl_possible && !d->xmlVersion.isEmpty() ) {
			TQString value( "version = '" );
			value += d->xmlVersion;
			value += "'";
			if ( !d->encoding.isEmpty() ) {
			    value += " encoding = '";
			    value += d->encoding;
			    value += "'";
			}
			if ( d->standalone == TQXmlSimpleReaderPrivate::Yes ) {
			    value += " standalone = 'yes'";
			} else if ( d->standalone == TQXmlSimpleReaderPrivate::No ) {
			    value += " standalone = 'no'";
			}
			if ( !contentHnd->processingInstruction( "xml", value ) ) {
			    reportParseError( contentHnd->errorString() );
			    return false;
			}
		    } else {
			if ( !contentHnd->processingInstruction( name(), string() ) ) {
			    reportParseError( contentHnd->errorString() );
			    return false;
			}
		    }
		}
		// XML declaration only on first position possible
		d->xmldecl_possible = false;
		state = PInstrR;
		break;
	    case Done:
		return true;
	    case -1:
		reportParseError( XMLERR_ERRORPARSINGELEMENT );
		return false;
	}

	if ( atEnd() ) {
	    unexpectedEof( &TQXmlSimpleReader::parseProlog, state );
	    return false;
	}
	if        ( is_S(c) ) {
	    input = InpWs;
	} else if ( c.unicode() == '<' ) {
	    input = InpLt;
	} else if ( c.unicode() == '?' ) {
	    input = InpQm;
	} else if ( c.unicode() == '!' ) {
	    input = InpEm;
	} else if ( c.unicode() == 'D' ) {
	    input = InpD;
	} else if ( c.unicode() == '-' ) {
	    input = InpDash;
	} else {
	    input = InpUnknown;
	}
	state = table[state][input];

	switch ( state ) {
	    case EatWS:
		// XML declaration only on first position possible
		d->xmldecl_possible = false;
		if ( !eat_ws() ) {
		    parseFailed( &TQXmlSimpleReader::parseProlog, state );
		    return false;
		}
		break;
	    case Lt:
		next();
		break;
	    case Em:
		// XML declaration only on first position possible
		d->xmldecl_possible = false;
		next();
		break;
	    case DocType:
		if ( !parseDoctype() ) {
		    parseFailed( &TQXmlSimpleReader::parseProlog, state );
		    return false;
		}
		break;
	    case Comment:
	    case CommentR:
		if ( !parseComment() ) {
		    parseFailed( &TQXmlSimpleReader::parseProlog, state );
		    return false;
		}
		break;
	    case PInstr:
	    case PInstrR:
		d->parsePI_xmldecl = d->xmldecl_possible;
		if ( !parsePI() ) {
		    parseFailed( &TQXmlSimpleReader::parseProlog, state );
		    return false;
		}
		break;
	}
    }
}

/*
  Parse an element [39].

  Precondition: the opening '<' is already read.
*/
bool TQXmlSimpleReader::parseElement()
{
    const int Init             =  0;
    const int ReadName         =  1;
    const int Ws1              =  2;
    const int STagEnd          =  3;
    const int STagEnd2         =  4;
    const int ETagBegin        =  5;
    const int ETagBegin2       =  6;
    const int Ws2              =  7;
    const int EmptyTag         =  8;
    const int Attrib           =  9;
    const int AttribPro        = 10; // like Attrib, but processAttribute was already called
    const int Ws3              = 11;
    const int Done             = 12;

    const int InpWs            = 0; // whitespace
    const int InpNameBe        = 1; // is_NameBeginning()
    const int InpGt            = 2; // >
    const int InpSlash         = 3; // /
    const int InpUnknown       = 4;

    static const int table[12][5] = {
     /*  InpWs      InpNameBe    InpGt        InpSlash     InpUnknown */
	{ -1,        ReadName,    -1,          -1,          -1        }, // Init
	{ Ws1,       Attrib,      STagEnd,     EmptyTag,    -1        }, // ReadName
	{ -1,        Attrib,      STagEnd,     EmptyTag,    -1        }, // Ws1
	{ STagEnd2,  STagEnd2,    STagEnd2,    STagEnd2,    STagEnd2  }, // STagEnd
	{ -1,        -1,          -1,          ETagBegin,   -1        }, // STagEnd2
	{ -1,        ETagBegin2,  -1,          -1,          -1        }, // ETagBegin
	{ Ws2,       -1,          Done,        -1,          -1        }, // ETagBegin2
	{ -1,        -1,          Done,        -1,          -1        }, // Ws2
	{ -1,        -1,          Done,        -1,          -1        }, // EmptyTag
	{ Ws3,       Attrib,      STagEnd,     EmptyTag,    -1        }, // Attrib
	{ Ws3,       Attrib,      STagEnd,     EmptyTag,    -1        }, // AttribPro
	{ -1,        Attrib,      STagEnd,     EmptyTag,    -1        }  // Ws3
    };
    int state;
    int input;

    if ( d->parseStack==0 || d->parseStack->isEmpty() ) {
	state = Init;
    } else {
	state = d->parseStack->pop().state;
#if defined(QT_QXML_DEBUG)
	tqDebug( "TQXmlSimpleReader: parseElement (cont) in state %d", state );
#endif
	if ( !d->parseStack->isEmpty() ) {
	    ParseFunction function = d->parseStack->top().function;
	    if ( function == &TQXmlSimpleReader::eat_ws ) {
		d->parseStack->pop();
#if defined(QT_QXML_DEBUG)
		tqDebug( "TQXmlSimpleReader: eat_ws (cont)" );
#endif
	    }
	    if ( !(this->*function)() ) {
		parseFailed( &TQXmlSimpleReader::parseElement, state );
		return false;
	    }
	}
    }

    for (;;) {
	switch ( state ) {
	    case ReadName:
		// store it on the stack
		d->tags.push( name() );
		// empty the attributes
		d->attList.clear();
		if ( d->useNamespaces )
		    d->namespaceSupport.pushContext();
		break;
	    case ETagBegin2:
		if ( !processElementETagBegin2() )
		    return false;
		break;
	    case Attrib:
		if ( !processElementAttribute() )
		    return false;
		state = AttribPro;
		break;
	    case Done:
		return true;
	    case -1:
		reportParseError( XMLERR_ERRORPARSINGELEMENT );
		return false;
	}

	if ( atEnd() ) {
	    unexpectedEof( &TQXmlSimpleReader::parseElement, state );
	    return false;
	}

        if (fastDetermineNameChar(c) == NameBeginning) {
	    input = InpNameBe;
	} else if ( c.unicode() == '>' ) {
	    input = InpGt;
	} else if (is_S(c)) {
            input = InpWs;
        } else if (c.unicode() == '/') {
	    input = InpSlash;
	} else {
	    input = InpUnknown;
	}
	state = table[state][input];

	switch ( state ) {
	    case ReadName:
		d->parseName_useRef = false;
		if ( !parseName() ) {
		    parseFailed( &TQXmlSimpleReader::parseElement, state );
		    return false;
		}
		break;
	    case Ws1:
	    case Ws2:
	    case Ws3:
		if ( !eat_ws() ) {
		    parseFailed( &TQXmlSimpleReader::parseElement, state );
		    return false;
		}
		break;
	    case STagEnd:
		// call the handler
		if ( contentHnd ) {
                    const TQString &tagsTop = d->tags.top();
		    if ( d->useNamespaces ) {
			TQString uri, lname;
                        d->namespaceSupport.processName(tagsTop, false, uri, lname);
                        if (!contentHnd->startElement(uri, lname, tagsTop, d->attList)) {
			    reportParseError( contentHnd->errorString() );
			    return false;
			}
		    } else {
                        if (!contentHnd->startElement(TQString::null, TQString::null, tagsTop, d->attList)) {
			    reportParseError( contentHnd->errorString() );
			    return false;
			}
		    }
		}
		next();
		break;
	    case STagEnd2:
		if ( !parseContent() ) {
		    parseFailed( &TQXmlSimpleReader::parseElement, state );
		    return false;
		}
		break;
	    case ETagBegin:
		next();
		break;
	    case ETagBegin2:
		// get the name of the tag
		d->parseName_useRef = false;
		if ( !parseName() ) {
		    parseFailed( &TQXmlSimpleReader::parseElement, state );
		    return false;
		}
		break;
	    case EmptyTag:
		if  ( d->tags.isEmpty() ) {
		    reportParseError( XMLERR_TAGMISMATCH );
		    return false;
		}
		if ( !processElementEmptyTag() )
		    return false;
		next();
		break;
	    case Attrib:
	    case AttribPro:
		// get name and value of attribute
		if ( !parseAttribute() ) {
		    parseFailed( &TQXmlSimpleReader::parseElement, state );
		    return false;
		}
		break;
	    case Done:
		next();
		break;
	}
    }
}
/*
  Helper to break down the size of the code in the case statement.
  Return false on error, otherwise true.
*/
bool TQXmlSimpleReader::processElementEmptyTag()
{
    TQString uri, lname;
    // pop the stack and call the handler
    if ( contentHnd ) {
	if ( d->useNamespaces ) {
	    // report startElement first...
	    d->namespaceSupport.processName( d->tags.top(), false, uri, lname );
	    if ( !contentHnd->startElement( uri, lname, d->tags.top(), d->attList ) ) {
		reportParseError( contentHnd->errorString() );
		return false;
	    }
	    // ... followed by endElement...
	    if ( !contentHnd->endElement( uri, lname, d->tags.pop() ) ) {
		reportParseError( contentHnd->errorString() );
		return false;
	    }
	    // ... followed by endPrefixMapping
	    TQStringList prefixesBefore, prefixesAfter;
	    if ( contentHnd ) {
		prefixesBefore = d->namespaceSupport.prefixes();
	    }
	    d->namespaceSupport.popContext();
	    // call the handler for prefix mapping
	    prefixesAfter = d->namespaceSupport.prefixes();
	    for ( TQStringList::Iterator it = prefixesBefore.begin(); it != prefixesBefore.end(); ++it ) {
		if ( prefixesAfter.contains(*it) == 0 ) {
		    if ( !contentHnd->endPrefixMapping( *it ) ) {
			reportParseError( contentHnd->errorString() );
			return false;
		    }
		}
	    }
	} else {
	    // report startElement first...
	    if ( !contentHnd->startElement( TQString::null, TQString::null, d->tags.top(), d->attList ) ) {
		reportParseError( contentHnd->errorString() );
		return false;
	    }
	    // ... followed by endElement
	    if ( !contentHnd->endElement( TQString::null, TQString::null, d->tags.pop() ) ) {
		reportParseError( contentHnd->errorString() );
		return false;
	    }
	}
    } else {
	d->tags.pop_back();
	d->namespaceSupport.popContext();
    }
    return true;
}
/*
  Helper to break down the size of the code in the case statement.
  Return false on error, otherwise true.
*/
bool TQXmlSimpleReader::processElementETagBegin2()
{
    const TQString &name = TQXmlSimpleReader::name();

    // pop the stack and compare it with the name
    if ( d->tags.pop() != name ) {
	reportParseError( XMLERR_TAGMISMATCH );
	return false;
    }
    // call the handler
    if ( contentHnd ) {
        TQString uri, lname;

        if (d->useNamespaces)
            d->namespaceSupport.processName(name, false, uri, lname);
        if (!contentHnd->endElement(uri, lname, name)) {
            reportParseError(contentHnd->errorString());
            return false;
	}
    }
    if ( d->useNamespaces ) {
        NamespaceMap prefixesBefore, prefixesAfter;
        if (contentHnd)
            prefixesBefore = d->namespaceSupport.d->ns;

	d->namespaceSupport.popContext();
	// call the handler for prefix mapping
	if ( contentHnd ) {
            prefixesAfter = d->namespaceSupport.d->ns;
            if (prefixesBefore.size() != prefixesAfter.size()) {
                for (NamespaceMap::const_iterator it = prefixesBefore.constBegin(); it != prefixesBefore.constEnd(); ++it) {
                    if (!it.key().isEmpty() && !prefixesAfter.contains(it.key())) {
                        if (!contentHnd->endPrefixMapping(it.key())) {
                            reportParseError(contentHnd->errorString());
                            return false;
                        }
		    }
		}
	    }
	}
    }
    return true;
}
/*
  Helper to break down the size of the code in the case statement.
  Return false on error, otherwise true.
*/
bool TQXmlSimpleReader::processElementAttribute()
{
    TQString uri, lname, prefix;
    const TQString &name = TQXmlSimpleReader::name();
    const TQString &string = TQXmlSimpleReader::string();

    // add the attribute to the list
    if ( d->useNamespaces ) {
	// is it a namespace declaration?
        d->namespaceSupport.splitName(name, prefix, lname);
        if (prefix == "xmlns") {
	    // namespace declaration
	    d->namespaceSupport.setPrefix( lname, string );
	    if ( d->useNamespacePrefixes ) {
		// according to http://www.w3.org/2000/xmlns/, the "prefix"
		// xmlns maps to the namespace name
		// http://www.w3.org/2000/xmlns/
		d->attList.append( name, "http://www.w3.org/2000/xmlns/", lname, string );
	    }
	    // call the handler for prefix mapping
	    if ( contentHnd ) {
		if ( !contentHnd->startPrefixMapping( lname, string ) ) {
		    reportParseError( contentHnd->errorString() );
		    return false;
		}
	    }
	} else {
	    // no namespace delcaration
	    d->namespaceSupport.processName( name, true, uri, lname );
	    d->attList.append( name, uri, lname, string );
	}
    } else {
	// no namespace support
	d->attList.append( name, TQString::null, TQString::null, string );
    }
    return true;
}

/*
  Parse a content [43].

  A content is only used between tags. If a end tag is found the < is already
  read and the head stand on the '/' of the end tag '</name>'.
*/
bool TQXmlSimpleReader::parseContent()
{
    const signed char Init             =  0;
    const signed char ChD              =  1; // CharData
    const signed char ChD1             =  2; // CharData help state
    const signed char ChD2             =  3; // CharData help state
    const signed char Ref              =  4; // Reference
    const signed char Lt               =  5; // '<' read
    const signed char PInstr           =  6; // PI
    const signed char PInstrR          =  7; // same as PInstr, but already reported
    const signed char Elem             =  8; // Element
    const signed char Em               =  9; // '!' read
    const signed char Com              = 10; // Comment
    const signed char ComR             = 11; // same as Com, but already reported
    const signed char CDS              = 12; // CDSect
    const signed char CDS1             = 13; // read a CDSect
    const signed char CDS2             = 14; // read a CDSect (help state)
    const signed char CDS3             = 15; // read a CDSect (help state)
    const signed char Done             = 16; // finished reading content

    const signed char InpLt            = 0; // <
    const signed char InpGt            = 1; // >
    const signed char InpSlash         = 2; // /
    const signed char InpTQMark         = 3; // ?
    const signed char InpEMark         = 4; // !
    const signed char InpAmp           = 5; // &
    const signed char InpDash          = 6; // -
    const signed char InpOpenB         = 7; // [
    const signed char InpCloseB        = 8; // ]
    const signed char InpUnknown       = 9;

    static const signed char mapCLT2FSMChar[] = {
	InpUnknown, // white space
	InpUnknown, // %
	InpAmp,     // &
	InpGt,      // >
	InpLt,      // <
	InpSlash,   // /
	InpTQMark,   // ?
	InpEMark,   // !
	InpDash,    // -
	InpCloseB,  // ]
	InpOpenB,   // [
	InpUnknown, // =
	InpUnknown, // "
	InpUnknown, // '
	InpUnknown  // unknown
    };

    static const signed char table[16][10] = {
     /*  InpLt  InpGt  InpSlash  InpTQMark  InpEMark  InpAmp  InpDash  InpOpenB  InpCloseB  InpUnknown */
	{ Lt,    ChD,   ChD,      ChD,      ChD,      Ref,    ChD,     ChD,      ChD1,      ChD  }, // Init
	{ Lt,    ChD,   ChD,      ChD,      ChD,      Ref,    ChD,     ChD,      ChD1,      ChD  }, // ChD
	{ Lt,    ChD,   ChD,      ChD,      ChD,      Ref,    ChD,     ChD,      ChD2,      ChD  }, // ChD1
	{ Lt,    -1,    ChD,      ChD,      ChD,      Ref,    ChD,     ChD,      ChD2,      ChD  }, // ChD2
	{ Lt,    ChD,   ChD,      ChD,      ChD,      Ref,    ChD,     ChD,      ChD,       ChD  }, // Ref (same as Init)
	{ -1,    -1,    Done,     PInstr,   Em,       -1,     -1,      -1,       -1,        Elem }, // Lt
	{ Lt,    ChD,   ChD,      ChD,      ChD,      Ref,    ChD,     ChD,      ChD,       ChD  }, // PInstr (same as Init)
	{ Lt,    ChD,   ChD,      ChD,      ChD,      Ref,    ChD,     ChD,      ChD,       ChD  }, // PInstrR
	{ Lt,    ChD,   ChD,      ChD,      ChD,      Ref,    ChD,     ChD,      ChD,       ChD  }, // Elem (same as Init)
	{ -1,    -1,    -1,       -1,       -1,       -1,     Com,     CDS,      -1,        -1   }, // Em
	{ Lt,    ChD,   ChD,      ChD,      ChD,      Ref,    ChD,     ChD,      ChD,       ChD  }, // Com (same as Init)
	{ Lt,    ChD,   ChD,      ChD,      ChD,      Ref,    ChD,     ChD,      ChD,       ChD  }, // ComR
	{ CDS1,  CDS1,  CDS1,     CDS1,     CDS1,     CDS1,   CDS1,    CDS1,     CDS2,      CDS1 }, // CDS
	{ CDS1,  CDS1,  CDS1,     CDS1,     CDS1,     CDS1,   CDS1,    CDS1,     CDS2,      CDS1 }, // CDS1
	{ CDS1,  CDS1,  CDS1,     CDS1,     CDS1,     CDS1,   CDS1,    CDS1,     CDS3,      CDS1 }, // CDS2
	{ CDS1,  Init,  CDS1,     CDS1,     CDS1,     CDS1,   CDS1,    CDS1,     CDS3,      CDS1 }  // CDS3
    };
    signed char state;
    signed char input;

    if ( d->parseStack==0 || d->parseStack->isEmpty() ) {
	d->contentCharDataRead = false;
	state = Init;
    } else {
	state = d->parseStack->pop().state;
#if defined(QT_QXML_DEBUG)
	tqDebug( "TQXmlSimpleReader: parseContent (cont) in state %d", state );
#endif
	if ( !d->parseStack->isEmpty() ) {
	    ParseFunction function = d->parseStack->top().function;
	    if ( function == &TQXmlSimpleReader::eat_ws ) {
		d->parseStack->pop();
#if defined(QT_QXML_DEBUG)
		tqDebug( "TQXmlSimpleReader: eat_ws (cont)" );
#endif
	    }
	    if ( !(this->*function)() ) {
		parseFailed( &TQXmlSimpleReader::parseContent, state );
		return false;
	    }
	}
    }

    for (;;) {
	switch ( state ) {
	    case Ref:
		if ( !d->contentCharDataRead)
		    d->contentCharDataRead = d->parseReference_charDataRead;
		break;
	    case PInstr:
		if ( contentHnd ) {
		    if ( !contentHnd->processingInstruction(name(),string()) ) {
			reportParseError( contentHnd->errorString() );
			return false;
		    }
		}
		state = PInstrR;
		break;
	    case Com:
		if ( lexicalHnd ) {
		    if ( !lexicalHnd->comment( string() ) ) {
			reportParseError( lexicalHnd->errorString() );
			return false;
		    }
		}
		state = ComR;
		break;
	    case CDS:
		stringClear();
		break;
	    case CDS2:
		if ( !atEnd() && c.unicode() != ']' )
		    stringAddC( ']' );
		break;
	    case CDS3:
		// test if this skipping was legal
		if ( !atEnd() ) {
		    if ( c.unicode() == '>' ) {
			// the end of the CDSect
			if ( lexicalHnd ) {
			    if ( !lexicalHnd->startCDATA() ) {
				reportParseError( lexicalHnd->errorString() );
				return false;
			    }
			}
			if ( contentHnd ) {
			    if ( !contentHnd->characters( string() ) ) {
				reportParseError( contentHnd->errorString() );
				return false;
			    }
			}
			if ( lexicalHnd ) {
			    if ( !lexicalHnd->endCDATA() ) {
				reportParseError( lexicalHnd->errorString() );
				return false;
			    }
			}
		    } else if (c.unicode() == ']') {
			// three or more ']'
			stringAddC( ']' );
		    } else {
			// after ']]' comes another character
			stringAddC( ']' );
			stringAddC( ']' );
		    }
		}
		break;
	    case Done:
		// call the handler for CharData
		if ( contentHnd ) {
		    if ( d->contentCharDataRead ) {
			if ( d->reportWhitespaceCharData || !string().simplifyWhiteSpace().isEmpty() ) {
			    if ( !contentHnd->characters( string() ) ) {
				reportParseError( contentHnd->errorString() );
				return false;
			    }
			}
		    }
		}
		// Done
		return true;
	    case -1:
		// Error
		reportParseError( XMLERR_ERRORPARSINGCONTENT );
		return false;
	}

	// get input (use lookup-table instead of nested ifs for performance
	// reasons)
	if ( atEnd() ) {
	    unexpectedEof( &TQXmlSimpleReader::parseContent, state );
	    return false;
	}
	if ( c.row() ) {
	    input = InpUnknown;
	} else {
	    input = mapCLT2FSMChar[ charLookupTable[ c.cell() ] ];
	}
	state = table[state][input];

	switch ( state ) {
	    case Init:
		// skip the ending '>' of a CDATASection
		next();
		break;
	    case ChD:
		// on first call: clear string
		if ( !d->contentCharDataRead ) {
		    d->contentCharDataRead = true;
		    stringClear();
		}
		stringAddC();
		if ( d->reportEntities ) {
		    if ( !reportEndEntities() )
			return false;
		}
		next();
		break;
	    case ChD1:
		// on first call: clear string
		if ( !d->contentCharDataRead ) {
		    d->contentCharDataRead = true;
		    stringClear();
		}
		stringAddC();
		if ( d->reportEntities ) {
		    if ( !reportEndEntities() )
			return false;
		}
		next();
		break;
	    case ChD2:
		stringAddC();
		if ( d->reportEntities ) {
		    if ( !reportEndEntities() )
			return false;
		}
		next();
		break;
	    case Ref:
		if ( !d->contentCharDataRead) {
		    // reference may be CharData; so clear string to be safe
		    stringClear();
		    d->parseReference_context = InContent;
		    if ( !parseReference() ) {
			parseFailed( &TQXmlSimpleReader::parseContent, state );
			return false;
		    }
		} else {
		    if ( d->reportEntities ) {
			// report character data in chunks
			if ( contentHnd ) {
			    if ( d->reportWhitespaceCharData || !string().simplifyWhiteSpace().isEmpty() ) {
				if ( !contentHnd->characters( string() ) ) {
				    reportParseError( contentHnd->errorString() );
				    return false;
				}
			    }
			}
			stringClear();
		    }
		    d->parseReference_context = InContent;
		    if ( !parseReference() ) {
			parseFailed( &TQXmlSimpleReader::parseContent, state );
			return false;
		    }
		}
		break;
	    case Lt:
		// call the handler for CharData
		if ( contentHnd ) {
		    if ( d->contentCharDataRead ) {
			if ( d->reportWhitespaceCharData || !string().simplifyWhiteSpace().isEmpty() ) {
			    if ( !contentHnd->characters( string() ) ) {
				reportParseError( contentHnd->errorString() );
				return false;
			    }
			}
		    }
		}
		d->contentCharDataRead = false;
		next();
		break;
	    case PInstr:
	    case PInstrR:
		d->parsePI_xmldecl = false;
		if ( !parsePI() ) {
		    parseFailed( &TQXmlSimpleReader::parseContent, state );
		    return false;
		}
		break;
	    case Elem:
		if ( !parseElement() ) {
		    parseFailed( &TQXmlSimpleReader::parseContent, state );
		    return false;
		}
		break;
	    case Em:
		next();
		break;
	    case Com:
	    case ComR:
		if ( !parseComment() ) {
		    parseFailed( &TQXmlSimpleReader::parseContent, state );
		    return false;
		}
		break;
	    case CDS:
		d->parseString_s = "[CDATA[";
		if ( !parseString() ) {
		    parseFailed( &TQXmlSimpleReader::parseContent, state );
		    return false;
		}
		break;
	    case CDS1:
		stringAddC();
		next();
		break;
	    case CDS2:
		// skip ']'
		next();
		break;
	    case CDS3:
		// skip ']'...
		next();
		break;
	}
    }
}
bool TQXmlSimpleReader::reportEndEntities()
{
    int count = (int)d->xmlRef.count();
    while ( count != 0 && d->xmlRef.top().isEmpty() ) {
	if ( contentHnd ) {
	    if ( d->reportWhitespaceCharData || !string().simplifyWhiteSpace().isEmpty() ) {
		if ( !contentHnd->characters( string() ) ) {
		    reportParseError( contentHnd->errorString() );
		    return false;
		}
	    }
	}
	stringClear();
	if ( lexicalHnd ) {
	    if ( !lexicalHnd->endEntity(d->xmlRefName.top()) ) {
		reportParseError( lexicalHnd->errorString() );
		return false;
	    }
	}
	d->xmlRef.pop_back();
	d->xmlRefName.pop_back();
	count--;
    }
    return true;
}

/*
  Parse Misc [27].
*/
bool TQXmlSimpleReader::parseMisc()
{
    const signed char Init             = 0;
    const signed char Lt               = 1; // '<' was read
    const signed char Comment          = 2; // read comment
    const signed char eatWS            = 3; // eat whitespaces
    const signed char PInstr           = 4; // read PI
    const signed char Comment2         = 5; // read comment

    const signed char InpWs            = 0; // S
    const signed char InpLt            = 1; // <
    const signed char InpQm            = 2; // ?
    const signed char InpEm            = 3; // !
    const signed char InpUnknown       = 4;

    static const signed char table[3][5] = {
     /*  InpWs   InpLt  InpQm  InpEm     InpUnknown */
	{ eatWS,  Lt,    -1,    -1,       -1        }, // Init
	{ -1,     -1,    PInstr,Comment,  -1        }, // Lt
	{ -1,     -1,    -1,    -1,       Comment2  }  // Comment
    };
    signed char state;
    signed char input;

    if ( d->parseStack==0 || d->parseStack->isEmpty() ) {
	state = Init;
    } else {
	state = d->parseStack->pop().state;
#if defined(QT_QXML_DEBUG)
	tqDebug( "TQXmlSimpleReader: parseMisc (cont) in state %d", state );
#endif
	if ( !d->parseStack->isEmpty() ) {
	    ParseFunction function = d->parseStack->top().function;
	    if ( function == &TQXmlSimpleReader::eat_ws ) {
		d->parseStack->pop();
#if defined(QT_QXML_DEBUG)
		tqDebug( "TQXmlSimpleReader: eat_ws (cont)" );
#endif
	    }
	    if ( !(this->*function)() ) {
		parseFailed( &TQXmlSimpleReader::parseMisc, state );
		return false;
	    }
	}
    }

    for (;;) {
	switch ( state ) {
	    case eatWS:
		return true;
	    case PInstr:
		if ( contentHnd ) {
		    if ( !contentHnd->processingInstruction(name(),string()) ) {
			reportParseError( contentHnd->errorString() );
			return false;
		    }
		}
		return true;
	    case Comment2:
		if ( lexicalHnd ) {
		    if ( !lexicalHnd->comment( string() ) ) {
			reportParseError( lexicalHnd->errorString() );
			return false;
		    }
		}
		return true;
	    case -1:
		// Error
		reportParseError( XMLERR_UNEXPECTEDCHARACTER );
		return false;
	}

	if ( atEnd() ) {
	    unexpectedEof( &TQXmlSimpleReader::parseMisc, state );
	    return false;
	}
	if        ( is_S(c) ) {
	    input = InpWs;
	} else if ( c.unicode() == '<' ) {
	    input = InpLt;
	} else if ( c.unicode() == '?' ) {
	    input = InpQm;
	} else if ( c.unicode() == '!' ) {
	    input = InpEm;
	} else {
	    input = InpUnknown;
	}
	state = table[state][input];

	switch ( state ) {
	    case eatWS:
		if ( !eat_ws() ) {
		    parseFailed( &TQXmlSimpleReader::parseMisc, state );
		    return false;
		}
		break;
	    case Lt:
		next();
		break;
	    case PInstr:
		d->parsePI_xmldecl = false;
		if ( !parsePI() ) {
		    parseFailed( &TQXmlSimpleReader::parseMisc, state );
		    return false;
		}
		break;
	    case Comment:
		next();
		break;
	    case Comment2:
		if ( !parseComment() ) {
		    parseFailed( &TQXmlSimpleReader::parseMisc, state );
		    return false;
		}
		break;
	}
    }
}

/*
  Parse a processing instruction [16].

  If xmldec is true, it tries to parse a PI or a XML declaration [23].

  Precondition: the beginning '<' of the PI is already read and the head stand
  on the '?' of '<?'.

  If this funktion was successful, the head-position is on the first
  character after the PI.
*/
bool TQXmlSimpleReader::parsePI()
{
    const signed char Init             =  0;
    const signed char QmI              =  1; // ? was read
    const signed char Name             =  2; // read Name
    const signed char XMLDecl          =  3; // read XMLDecl
    const signed char Ws1              =  4; // eat ws after "xml" of XMLDecl
    const signed char PInstr           =  5; // read PI
    const signed char Ws2              =  6; // eat ws after Name of PI
    const signed char Version          =  7; // read versionInfo
    const signed char Ws3              =  8; // eat ws after versionInfo
    const signed char EorSD            =  9; // read EDecl or SDDecl
    const signed char Ws4              = 10; // eat ws after EDecl or SDDecl
    const signed char SD               = 11; // read SDDecl
    const signed char Ws5              = 12; // eat ws after SDDecl
    const signed char ADone            = 13; // almost done
    const signed char Char             = 14; // Char was read
    const signed char Qm               = 15; // Qm was read
    const signed char Done             = 16; // finished reading content

    const signed char InpWs            = 0; // whitespace
    const signed char InpNameBe        = 1; // NameBeginning()
    const signed char InpGt            = 2; // >
    const signed char InpQm            = 3; // ?
    const signed char InpUnknown       = 4;

    static const signed char table[16][5] = {
     /*  InpWs,  InpNameBe  InpGt  InpQm   InpUnknown  */
	{ -1,     -1,        -1,    QmI,    -1     }, // Init
	{ -1,     Name,      -1,    -1,     -1     }, // QmI
	{ -1,     -1,        -1,    -1,     -1     }, // Name (this state is left not through input)
	{ Ws1,    -1,        -1,    -1,     -1     }, // XMLDecl
	{ -1,     Version,   -1,    -1,     -1     }, // Ws1
	{ Ws2,    -1,        -1,    Qm,     -1     }, // PInstr
	{ Char,   Char,      Char,  Qm,     Char   }, // Ws2
	{ Ws3,    -1,        -1,    ADone,  -1     }, // Version
	{ -1,     EorSD,     -1,    ADone,  -1     }, // Ws3
	{ Ws4,    -1,        -1,    ADone,  -1     }, // EorSD
	{ -1,     SD,        -1,    ADone,  -1     }, // Ws4
	{ Ws5,    -1,        -1,    ADone,  -1     }, // SD
	{ -1,     -1,        -1,    ADone,  -1     }, // Ws5
	{ -1,     -1,        Done,  -1,     -1     }, // ADone
	{ Char,   Char,      Char,  Qm,     Char   }, // Char
	{ Char,   Char,      Done,  Qm,     Char   }, // Qm
    };
    signed char state;
    signed char input;

    if ( d->parseStack==0 || d->parseStack->isEmpty() ) {
	state = Init;
    } else {
	state = d->parseStack->pop().state;
#if defined(QT_QXML_DEBUG)
	tqDebug( "TQXmlSimpleReader: parsePI (cont) in state %d", state );
#endif
	if ( !d->parseStack->isEmpty() ) {
	    ParseFunction function = d->parseStack->top().function;
	    if ( function == &TQXmlSimpleReader::eat_ws ) {
		d->parseStack->pop();
#if defined(QT_QXML_DEBUG)
		tqDebug( "TQXmlSimpleReader: eat_ws (cont)" );
#endif
	    }
	    if ( !(this->*function)() ) {
		parseFailed( &TQXmlSimpleReader::parsePI, state );
		return false;
	    }
	}
    }

    for (;;) {
	switch ( state ) {
	    case Name:
		// test what name was read and determine the next state
		// (not very beautiful, I admit)
		if ( name().lower() == "xml" ) {
		    if ( d->parsePI_xmldecl && name()=="xml" ) {
			state = XMLDecl;
		    } else {
			reportParseError( XMLERR_INVALIDNAMEFORPI );
			return false;
		    }
		} else {
		    state = PInstr;
		    stringClear();
		}
		break;
	    case Version:
		// get version (syntax like an attribute)
		if ( name() != "version" ) {
		    reportParseError( XMLERR_VERSIONEXPECTED );
		    return false;
		}
		d->xmlVersion = string();
		break;
	    case EorSD:
		// get the EDecl or SDDecl (syntax like an attribute)
		if        ( name() == "standalone" ) {
		    if ( string()=="yes" ) {
			d->standalone = TQXmlSimpleReaderPrivate::Yes;
		    } else if ( string()=="no" ) {
			d->standalone = TQXmlSimpleReaderPrivate::No;
		    } else {
			reportParseError( XMLERR_WRONGVALUEFORSDECL );
			return false;
		    }
		} else if ( name() == "encoding" ) {
		    d->encoding = string();
		} else {
		    reportParseError( XMLERR_EDECLORSDDECLEXPECTED );
		    return false;
		}
		break;
	    case SD:
		if ( name() != "standalone" ) {
		    reportParseError( XMLERR_SDDECLEXPECTED );
		    return false;
		}
		if ( string()=="yes" ) {
		    d->standalone = TQXmlSimpleReaderPrivate::Yes;
		} else if ( string()=="no" ) {
		    d->standalone = TQXmlSimpleReaderPrivate::No;
		} else {
		    reportParseError( XMLERR_WRONGVALUEFORSDECL );
		    return false;
		}
		break;
	    case Qm:
		// test if the skipping was legal
		if ( !atEnd() && c.unicode() != '>' )
		    stringAddC( '?' );
		break;
	    case Done:
		return true;
	    case -1:
		// Error
		reportParseError( XMLERR_UNEXPECTEDCHARACTER );
		return false;
	}

	if ( atEnd() ) {
	    unexpectedEof( &TQXmlSimpleReader::parsePI, state );
	    return false;
	}
	if        ( is_S(c) ) {
	    input = InpWs;
	} else if (determineNameChar(c) == NameBeginning) {
	    input = InpNameBe;
	} else if ( c.unicode() == '>' ) {
	    input = InpGt;
	} else if ( c.unicode() == '?' ) {
	    input = InpQm;
	} else {
	    input = InpUnknown;
	}
	state = table[state][input];

	switch ( state ) {
	    case QmI:
		next();
		break;
	    case Name:
		d->parseName_useRef = false;
		if ( !parseName() ) {
		    parseFailed( &TQXmlSimpleReader::parsePI, state );
		    return false;
		}
		break;
	    case Ws1:
	    case Ws2:
	    case Ws3:
	    case Ws4:
	    case Ws5:
		if ( !eat_ws() ) {
		    parseFailed( &TQXmlSimpleReader::parsePI, state );
		    return false;
		}
		break;
	    case Version:
		if ( !parseAttribute() ) {
		    parseFailed( &TQXmlSimpleReader::parsePI, state );
		    return false;
		}
		break;
	    case EorSD:
		if ( !parseAttribute() ) {
		    parseFailed( &TQXmlSimpleReader::parsePI, state );
		    return false;
		}
		break;
	    case SD:
		// get the SDDecl (syntax like an attribute)
		if ( d->standalone != TQXmlSimpleReaderPrivate::Unknown ) {
		    // already parsed the standalone declaration
		    reportParseError( XMLERR_UNEXPECTEDCHARACTER );
		    return false;
		}
		if ( !parseAttribute() ) {
		    parseFailed( &TQXmlSimpleReader::parsePI, state );
		    return false;
		}
		break;
	    case ADone:
		next();
		break;
	    case Char:
		stringAddC();
		next();
		break;
	    case Qm:
		// skip the '?'
		next();
		break;
	    case Done:
		next();
		break;
	}
    }
}

/*
  Parse a document type definition (doctypedecl [28]).

  Precondition: the beginning '<!' of the doctype is already read the head
  stands on the 'D' of '<!DOCTYPE'.

  If this funktion was successful, the head-position is on the first
  character after the document type definition.
*/
bool TQXmlSimpleReader::parseDoctype()
{
    const signed char Init             =  0;
    const signed char Doctype          =  1; // read the doctype
    const signed char Ws1              =  2; // eat_ws
    const signed char Doctype2         =  3; // read the doctype, part 2
    const signed char Ws2              =  4; // eat_ws
    const signed char Sys              =  5; // read SYSTEM or PUBLIC
    const signed char Ws3              =  6; // eat_ws
    const signed char MP               =  7; // markupdecl or PEReference
    const signed char MPR              =  8; // same as MP, but already reported
    const signed char PER              =  9; // PERReference
    const signed char Mup              = 10; // markupdecl
    const signed char Ws4              = 11; // eat_ws
    const signed char MPE              = 12; // end of markupdecl or PEReference
    const signed char Done             = 13;

    const signed char InpWs            = 0;
    const signed char InpD             = 1; // 'D'
    const signed char InpS             = 2; // 'S' or 'P'
    const signed char InpOB            = 3; // [
    const signed char InpCB            = 4; // ]
    const signed char InpPer           = 5; // %
    const signed char InpGt            = 6; // >
    const signed char InpUnknown       = 7;

    static const signed char table[13][8] = {
     /*  InpWs,  InpD       InpS       InpOB  InpCB  InpPer InpGt  InpUnknown */
	{ -1,     Doctype,   -1,        -1,    -1,    -1,    -1,    -1        }, // Init
	{ Ws1,    -1,        -1,        -1,    -1,    -1,    -1,    -1        }, // Doctype
	{ -1,     Doctype2,  Doctype2,  -1,    -1,    -1,    -1,    Doctype2  }, // Ws1
	{ Ws2,    -1,        Sys,       MP,    -1,    -1,    Done,  -1        }, // Doctype2
	{ -1,     -1,        Sys,       MP,    -1,    -1,    Done,  -1        }, // Ws2
	{ Ws3,    -1,        -1,        MP,    -1,    -1,    Done,  -1        }, // Sys
	{ -1,     -1,        -1,        MP,    -1,    -1,    Done,  -1        }, // Ws3
	{ -1,     -1,        -1,        -1,    MPE,   PER,   -1,    Mup       }, // MP
	{ -1,     -1,        -1,        -1,    MPE,   PER,   -1,    Mup       }, // MPR
	{ Ws4,    -1,        -1,        -1,    MPE,   PER,   -1,    Mup       }, // PER
	{ Ws4,    -1,        -1,        -1,    MPE,   PER,   -1,    Mup       }, // Mup
	{ -1,     -1,        -1,        -1,    MPE,   PER,   -1,    Mup       }, // Ws4
	{ -1,     -1,        -1,        -1,    -1,    -1,    Done,  -1        }  // MPE
    };
    signed char state;
    signed char input;

    if ( d->parseStack==0 || d->parseStack->isEmpty() ) {
	d->startDTDwasReported = false;
	d->systemId = TQString::null;
	d->publicId = TQString::null;
	state = Init;
    } else {
	state = d->parseStack->pop().state;
#if defined(QT_QXML_DEBUG)
	tqDebug( "TQXmlSimpleReader: parseDoctype (cont) in state %d", state );
#endif
	if ( !d->parseStack->isEmpty() ) {
	    ParseFunction function = d->parseStack->top().function;
	    if ( function == &TQXmlSimpleReader::eat_ws ) {
		d->parseStack->pop();
#if defined(QT_QXML_DEBUG)
		tqDebug( "TQXmlSimpleReader: eat_ws (cont)" );
#endif
	    }
	    if ( !(this->*function)() ) {
		parseFailed( &TQXmlSimpleReader::parseDoctype, state );
		return false;
	    }
	}
    }

    for (;;) {
	switch ( state ) {
	    case Doctype2:
		d->doctype = name();
		break;
	    case MP:
		if ( !d->startDTDwasReported && lexicalHnd  ) {
		    d->startDTDwasReported = true;
		    if ( !lexicalHnd->startDTD( d->doctype, d->publicId, d->systemId ) ) {
			reportParseError( lexicalHnd->errorString() );
			return false;
		    }
		}
		state = MPR;
		break;
	    case Done:
		return true;
	    case -1:
		// Error
		reportParseError( XMLERR_ERRORPARSINGDOCTYPE );
		return false;
	}

	if ( atEnd() ) {
	    unexpectedEof( &TQXmlSimpleReader::parseDoctype, state );
	    return false;
	}
	if        ( is_S(c) ) {
	    input = InpWs;
	} else if ( c.unicode() == 'D' ) {
	    input = InpD;
	} else if ( c.unicode() == 'S' ) {
	    input = InpS;
	} else if ( c.unicode() == 'P' ) {
	    input = InpS;
	} else if ( c.unicode() == '[' ) {
	    input = InpOB;
	} else if ( c.unicode() == ']' ) {
	    input = InpCB;
	} else if ( c.unicode() == '%' ) {
	    input = InpPer;
	} else if ( c.unicode() == '>' ) {
	    input = InpGt;
	} else {
	    input = InpUnknown;
	}
	state = table[state][input];

	switch ( state ) {
	    case Doctype:
		d->parseString_s = "DOCTYPE";
		if ( !parseString() ) {
		    parseFailed( &TQXmlSimpleReader::parseDoctype, state );
		    return false;
		}
		break;
	    case Ws1:
	    case Ws2:
	    case Ws3:
	    case Ws4:
		if ( !eat_ws() ) {
		    parseFailed( &TQXmlSimpleReader::parseDoctype, state );
		    return false;
		}
		break;
	    case Doctype2:
		d->parseName_useRef = false;
		if ( !parseName() ) {
		    parseFailed( &TQXmlSimpleReader::parseDoctype, state );
		    return false;
		}
		break;
	    case Sys:
		d->parseExternalID_allowPublicID = false;
		if ( !parseExternalID() ) {
		    parseFailed( &TQXmlSimpleReader::parseDoctype, state );
		    return false;
		}
		break;
	    case MP:
	    case MPR:
		if ( !next_eat_ws() ) {
		    parseFailed( &TQXmlSimpleReader::parseDoctype, state );
		    return false;
		}
		break;
	    case PER:
		d->parsePEReference_context = InDTD;
		if ( !parsePEReference() ) {
		    parseFailed( &TQXmlSimpleReader::parseDoctype, state );
		    return false;
		}
		break;
	    case Mup:
		if (dtdRecursionLimit > 0U && d->parameterEntities.size() > dtdRecursionLimit) {
		    reportParseError(TQString::fromLatin1(
		        "DTD parsing exceeded recursion limit of %1.").arg(dtdRecursionLimit));
		    return false;
		}
		if ( !parseMarkupdecl() ) {
		    parseFailed( &TQXmlSimpleReader::parseDoctype, state );
		    return false;
		}
		break;
	    case MPE:
		if ( !next_eat_ws() ) {
		    parseFailed( &TQXmlSimpleReader::parseDoctype, state );
		    return false;
		}
		break;
	    case Done:
		if ( lexicalHnd ) {
		    if ( !d->startDTDwasReported ) {
			d->startDTDwasReported = true;
			if ( !lexicalHnd->startDTD( d->doctype, d->publicId, d->systemId ) ) {
			    reportParseError( lexicalHnd->errorString() );
			    return false;
			}
		    }
		    if ( !lexicalHnd->endDTD() ) {
			reportParseError( lexicalHnd->errorString() );
			return false;
		    }
		}
		next();
		break;
	}
    }
}

/*
  Parse a ExternalID [75].

  If allowPublicID is true parse ExternalID [75] or PublicID [83].
*/
bool TQXmlSimpleReader::parseExternalID()
{
    const signed char Init             =  0;
    const signed char Sys              =  1; // parse 'SYSTEM'
    const signed char SysWS            =  2; // parse the whitespace after 'SYSTEM'
    const signed char SysSQ            =  3; // parse SystemLiteral with '
    const signed char SysSQ2           =  4; // parse SystemLiteral with '
    const signed char SysDQ            =  5; // parse SystemLiteral with "
    const signed char SysDQ2           =  6; // parse SystemLiteral with "
    const signed char Pub              =  7; // parse 'PUBLIC'
    const signed char PubWS            =  8; // parse the whitespace after 'PUBLIC'
    const signed char PubSQ            =  9; // parse PubidLiteral with '
    const signed char PubSQ2           = 10; // parse PubidLiteral with '
    const signed char PubDQ            = 11; // parse PubidLiteral with "
    const signed char PubDQ2           = 12; // parse PubidLiteral with "
    const signed char PubE             = 13; // finished parsing the PubidLiteral
    const signed char PubWS2           = 14; // parse the whitespace after the PubidLiteral
    const signed char PDone            = 15; // done if allowPublicID is true
    const signed char Done             = 16;

    const signed char InpSQ            = 0; // '
    const signed char InpDQ            = 1; // "
    const signed char InpS             = 2; // S
    const signed char InpP             = 3; // P
    const signed char InpWs            = 4; // white space
    const signed char InpUnknown       = 5;

    static const signed char table[15][6] = {
     /*  InpSQ    InpDQ    InpS     InpP     InpWs     InpUnknown */
	{ -1,      -1,      Sys,     Pub,     -1,       -1      }, // Init
	{ -1,      -1,      -1,      -1,      SysWS,    -1      }, // Sys
	{ SysSQ,   SysDQ,   -1,      -1,      -1,       -1      }, // SysWS
	{ Done,    SysSQ2,  SysSQ2,  SysSQ2,  SysSQ2,   SysSQ2  }, // SysSQ
	{ Done,    SysSQ2,  SysSQ2,  SysSQ2,  SysSQ2,   SysSQ2  }, // SysSQ2
	{ SysDQ2,  Done,    SysDQ2,  SysDQ2,  SysDQ2,   SysDQ2  }, // SysDQ
	{ SysDQ2,  Done,    SysDQ2,  SysDQ2,  SysDQ2,   SysDQ2  }, // SysDQ2
	{ -1,      -1,      -1,      -1,      PubWS,    -1      }, // Pub
	{ PubSQ,   PubDQ,   -1,      -1,      -1,       -1      }, // PubWS
	{ PubE,    -1,      PubSQ2,  PubSQ2,  PubSQ2,   PubSQ2  }, // PubSQ
	{ PubE,    -1,      PubSQ2,  PubSQ2,  PubSQ2,   PubSQ2  }, // PubSQ2
	{ -1,      PubE,    PubDQ2,  PubDQ2,  PubDQ2,   PubDQ2  }, // PubDQ
	{ -1,      PubE,    PubDQ2,  PubDQ2,  PubDQ2,   PubDQ2  }, // PubDQ2
	{ PDone,   PDone,   PDone,   PDone,   PubWS2,   PDone   }, // PubE
	{ SysSQ,   SysDQ,   PDone,   PDone,   PDone,    PDone   }  // PubWS2
    };
    signed char state;
    signed char input;

    if ( d->parseStack==0 || d->parseStack->isEmpty() ) {
	d->systemId = TQString::null;
	d->publicId = TQString::null;
	state = Init;
    } else {
	state = d->parseStack->pop().state;
#if defined(QT_QXML_DEBUG)
	tqDebug( "TQXmlSimpleReader: parseExternalID (cont) in state %d", state );
#endif
	if ( !d->parseStack->isEmpty() ) {
	    ParseFunction function = d->parseStack->top().function;
	    if ( function == &TQXmlSimpleReader::eat_ws ) {
		d->parseStack->pop();
#if defined(QT_QXML_DEBUG)
		tqDebug( "TQXmlSimpleReader: eat_ws (cont)" );
#endif
	    }
	    if ( !(this->*function)() ) {
		parseFailed( &TQXmlSimpleReader::parseExternalID, state );
		return false;
	    }
	}
    }

    for (;;) {
	switch ( state ) {
	    case PDone:
		if ( d->parseExternalID_allowPublicID ) {
		    d->publicId = string();
		    return true;
		} else {
		    reportParseError( XMLERR_UNEXPECTEDCHARACTER );
		    return false;
		}
	    case Done:
		return true;
	    case -1:
		// Error
		reportParseError( XMLERR_UNEXPECTEDCHARACTER );
		return false;
	}

	if ( atEnd() ) {
	    unexpectedEof( &TQXmlSimpleReader::parseExternalID, state );
	    return false;
	}
	if        ( is_S(c) ) {
	    input = InpWs;
	} else if ( c.unicode() == '\'' ) {
	    input = InpSQ;
	} else if ( c.unicode() == '"' ) {
	    input = InpDQ;
	} else if ( c.unicode() == 'S' ) {
	    input = InpS;
	} else if ( c.unicode() == 'P' ) {
	    input = InpP;
	} else {
	    input = InpUnknown;
	}
	state = table[state][input];

	switch ( state ) {
	    case Sys:
		d->parseString_s = "SYSTEM";
		if ( !parseString() ) {
		    parseFailed( &TQXmlSimpleReader::parseExternalID, state );
		    return false;
		}
		break;
	    case SysWS:
		if ( !eat_ws() ) {
		    parseFailed( &TQXmlSimpleReader::parseExternalID, state );
		    return false;
		}
		break;
	    case SysSQ:
	    case SysDQ:
		stringClear();
		next();
		break;
	    case SysSQ2:
	    case SysDQ2:
		stringAddC();
		next();
		break;
	    case Pub:
		d->parseString_s = "PUBLIC";
		if ( !parseString() ) {
		    parseFailed( &TQXmlSimpleReader::parseExternalID, state );
		    return false;
		}
		break;
	    case PubWS:
		if ( !eat_ws() ) {
		    parseFailed( &TQXmlSimpleReader::parseExternalID, state );
		    return false;
		}
		break;
	    case PubSQ:
	    case PubDQ:
		stringClear();
		next();
		break;
	    case PubSQ2:
	    case PubDQ2:
		stringAddC();
		next();
		break;
	    case PubE:
		next();
		break;
	    case PubWS2:
		d->publicId = string();
		if ( !eat_ws() ) {
		    parseFailed( &TQXmlSimpleReader::parseExternalID, state );
		    return false;
		}
		break;
	    case Done:
		d->systemId = string();
		next();
		break;
	}
    }
}

/*
  Parse a markupdecl [29].
*/
bool TQXmlSimpleReader::parseMarkupdecl()
{
    const signed char Init             = 0;
    const signed char Lt               = 1; // < was read
    const signed char Em               = 2; // ! was read
    const signed char CE               = 3; // E was read
    const signed char Qm               = 4; // ? was read
    const signed char Dash             = 5; // - was read
    const signed char CA               = 6; // A was read
    const signed char CEL              = 7; // EL was read
    const signed char CEN              = 8; // EN was read
    const signed char CN               = 9; // N was read
    const signed char Done             = 10;

    const signed char InpLt            = 0; // <
    const signed char InpQm            = 1; // ?
    const signed char InpEm            = 2; // !
    const signed char InpDash          = 3; // -
    const signed char InpA             = 4; // A
    const signed char InpE             = 5; // E
    const signed char InpL             = 6; // L
    const signed char InpN             = 7; // N
    const signed char InpUnknown       = 8;

    static const signed char table[4][9] = {
     /*  InpLt  InpQm  InpEm  InpDash  InpA   InpE   InpL   InpN   InpUnknown */
	{ Lt,    -1,    -1,    -1,      -1,    -1,    -1,    -1,    -1     }, // Init
	{ -1,    Qm,    Em,    -1,      -1,    -1,    -1,    -1,    -1     }, // Lt
	{ -1,    -1,    -1,    Dash,    CA,    CE,    -1,    CN,    -1     }, // Em
	{ -1,    -1,    -1,    -1,      -1,    -1,    CEL,   CEN,   -1     }  // CE
    };
    signed char state;
    signed char input;

    if ( d->parseStack==0 || d->parseStack->isEmpty() ) {
	state = Init;
    } else {
	state = d->parseStack->pop().state;
#if defined(QT_QXML_DEBUG)
	tqDebug( "TQXmlSimpleReader: parseMarkupdecl (cont) in state %d", state );
#endif
	if ( !d->parseStack->isEmpty() ) {
	    ParseFunction function = d->parseStack->top().function;
	    if ( function == &TQXmlSimpleReader::eat_ws ) {
		d->parseStack->pop();
#if defined(QT_QXML_DEBUG)
		tqDebug( "TQXmlSimpleReader: eat_ws (cont)" );
#endif
	    }
	    if ( !(this->*function)() ) {
		parseFailed( &TQXmlSimpleReader::parseMarkupdecl, state );
		return false;
	    }
	}
    }

    for (;;) {
	switch ( state ) {
	    case Qm:
		if ( contentHnd ) {
		    if ( !contentHnd->processingInstruction(name(),string()) ) {
			reportParseError( contentHnd->errorString() );
			return false;
		    }
		}
		return true;
	    case Dash:
		if ( lexicalHnd ) {
		    if ( !lexicalHnd->comment( string() ) ) {
			reportParseError( lexicalHnd->errorString() );
			return false;
		    }
		}
		return true;
	    case CA:
		return true;
	    case CEL:
		return true;
	    case CEN:
		return true;
	    case CN:
		return true;
	    case Done:
		return true;
	    case -1:
		// Error
		reportParseError( XMLERR_LETTEREXPECTED );
		return false;
	}

	if ( atEnd() ) {
	    unexpectedEof( &TQXmlSimpleReader::parseMarkupdecl, state );
	    return false;
	}
	if        ( c.unicode() == '<' ) {
	    input = InpLt;
	} else if ( c.unicode() == '?' ) {
	    input = InpQm;
	} else if ( c.unicode() == '!' ) {
	    input = InpEm;
	} else if ( c.unicode() == '-' ) {
	    input = InpDash;
	} else if ( c.unicode() == 'A' ) {
	    input = InpA;
	} else if ( c.unicode() == 'E' ) {
	    input = InpE;
	} else if ( c.unicode() == 'L' ) {
	    input = InpL;
	} else if ( c.unicode() == 'N' ) {
	    input = InpN;
	} else {
	    input = InpUnknown;
	}
	state = table[state][input];

	switch ( state ) {
	    case Lt:
		next();
		break;
	    case Em:
		next();
		break;
	    case CE:
		next();
		break;
	    case Qm:
		d->parsePI_xmldecl = false;
		if ( !parsePI() ) {
		    parseFailed( &TQXmlSimpleReader::parseMarkupdecl, state );
		    return false;
		}
		break;
	    case Dash:
		if ( !parseComment() ) {
		    parseFailed( &TQXmlSimpleReader::parseMarkupdecl, state );
		    return false;
		}
		break;
	    case CA:
		if ( !parseAttlistDecl() ) {
		    parseFailed( &TQXmlSimpleReader::parseMarkupdecl, state );
		    return false;
		}
		break;
	    case CEL:
		if ( !parseElementDecl() ) {
		    parseFailed( &TQXmlSimpleReader::parseMarkupdecl, state );
		    return false;
		}
		break;
	    case CEN:
		if ( !parseEntityDecl() ) {
		    parseFailed( &TQXmlSimpleReader::parseMarkupdecl, state );
		    return false;
		}
		break;
	    case CN:
		if ( !parseNotationDecl() ) {
		    parseFailed( &TQXmlSimpleReader::parseMarkupdecl, state );
		    return false;
		}
		break;
	}
    }
}

/*
  Parse a PEReference [69]
*/
bool TQXmlSimpleReader::parsePEReference()
{
    const signed char Init             = 0;
    const signed char Next             = 1;
    const signed char Name             = 2;
    const signed char NameR            = 3; // same as Name, but already reported
    const signed char Done             = 4;

    const signed char InpSemi          = 0; // ;
    const signed char InpPer           = 1; // %
    const signed char InpUnknown       = 2;

    static const signed char table[4][3] = {
     /*  InpSemi  InpPer  InpUnknown */
	{ -1,      Next,   -1    }, // Init
	{ -1,      -1,     Name  }, // Next
	{ Done,    -1,     -1    }, // Name
	{ Done,    -1,     -1    }  // NameR
    };
    signed char state;
    signed char input;

    if ( d->parseStack==0 || d->parseStack->isEmpty() ) {
	state = Init;
    } else {
	state = d->parseStack->pop().state;
#if defined(QT_QXML_DEBUG)
	tqDebug( "TQXmlSimpleReader: parsePEReference (cont) in state %d", state );
#endif
	if ( !d->parseStack->isEmpty() ) {
	    ParseFunction function = d->parseStack->top().function;
	    if ( function == &TQXmlSimpleReader::eat_ws ) {
		d->parseStack->pop();
#if defined(QT_QXML_DEBUG)
		tqDebug( "TQXmlSimpleReader: eat_ws (cont)" );
#endif
	    }
	    if ( !(this->*function)() ) {
		parseFailed( &TQXmlSimpleReader::parsePEReference, state );
		return false;
	    }
	}
    }

    for (;;) {
	switch ( state ) {
	    case Name:
		{
		    bool skipIt = true;
		    TQString xmlRefString;

		    TQMap<TQString,TQString>::Iterator it;
		    it = d->parameterEntities.find( ref() );
		    if ( it != d->parameterEntities.end() ) {
			skipIt = false;
			xmlRefString = it.data();
		    } else if ( entityRes ) {
			TQMap<TQString,TQXmlSimpleReaderPrivate::ExternParameterEntity>::Iterator it2;
			it2 = d->externParameterEntities.find( ref() );
			TQXmlInputSource *ret = 0;
			if ( it2 != d->externParameterEntities.end() ) {
			    if ( !entityRes->resolveEntity( it2.data().publicId, it2.data().systemId, ret ) ) {
				delete ret;
				reportParseError( entityRes->errorString() );
				return false;
			    }
			    if ( ret ) {
				xmlRefString = ret->data();
				delete ret;
				if ( !stripTextDecl( xmlRefString ) ) {
				    reportParseError( XMLERR_ERRORINTEXTDECL );
				    return false;
				}
				skipIt = false;
			    }
			}
		    }

		    if ( skipIt ) {
			if ( contentHnd ) {
			    if ( !contentHnd->skippedEntity( TQString("%") + ref() ) ) {
				reportParseError( contentHnd->errorString() );
				return false;
			    }
			}
		    } else {
			if ( d->parsePEReference_context == InEntityValue ) {
			    // Included in literal
			    if ( !insertXmlRef( xmlRefString, ref(), true ) )
				return false;
			} else if ( d->parsePEReference_context == InDTD ) {
			    // Included as PE
			    if ( !insertXmlRef( TQString(" ")+xmlRefString+TQString(" "), ref(), false ) )
				return false;
			}
		    }
		}
		state = NameR;
		break;
	    case Done:
		return true;
	    case -1:
		// Error
		reportParseError( XMLERR_LETTEREXPECTED );
		return false;
	}

	if ( atEnd() ) {
	    unexpectedEof( &TQXmlSimpleReader::parsePEReference, state );
	    return false;
	}
	if        ( c.unicode() == ';' ) {
	    input = InpSemi;
	} else if ( c.unicode() == '%' ) {
	    input = InpPer;
	} else {
	    input = InpUnknown;
	}
	state = table[state][input];

	switch ( state ) {
	    case Next:
		next();
		break;
	    case Name:
	    case NameR:
		d->parseName_useRef = true;
		if ( !parseName() ) {
		    parseFailed( &TQXmlSimpleReader::parsePEReference, state );
		    return false;
		}
		break;
	    case Done:
		next();
		break;
	}
    }
}

/*
  Parse a AttlistDecl [52].

  Precondition: the beginning '<!' is already read and the head
  stands on the 'A' of '<!ATTLIST'
*/
bool TQXmlSimpleReader::parseAttlistDecl()
{
    const signed char Init             =  0;
    const signed char Attlist          =  1; // parse the string "ATTLIST"
    const signed char Ws               =  2; // whitespace read
    const signed char Name             =  3; // parse name
    const signed char Ws1              =  4; // whitespace read
    const signed char Attdef           =  5; // parse the AttDef
    const signed char Ws2              =  6; // whitespace read
    const signed char Atttype          =  7; // parse the AttType
    const signed char Ws3              =  8; // whitespace read
    const signed char DDecH            =  9; // DefaultDecl with #
    const signed char DefReq           = 10; // parse the string "REQUIRED"
    const signed char DefImp           = 11; // parse the string "IMPLIED"
    const signed char DefFix           = 12; // parse the string "FIXED"
    const signed char Attval           = 13; // parse the AttValue
    const signed char Ws4              = 14; // whitespace read
    const signed char Done             = 15;

    const signed char InpWs            = 0; // white space
    const signed char InpGt            = 1; // >
    const signed char InpHash          = 2; // #
    const signed char InpA             = 3; // A
    const signed char InpI             = 4; // I
    const signed char InpF             = 5; // F
    const signed char InpR             = 6; // R
    const signed char InpUnknown       = 7;

    static const signed char table[15][8] = {
     /*  InpWs    InpGt    InpHash  InpA      InpI     InpF     InpR     InpUnknown */
	{ -1,      -1,      -1,      Attlist,  -1,      -1,      -1,      -1      }, // Init
	{ Ws,      -1,      -1,      -1,       -1,      -1,      -1,      -1      }, // Attlist
	{ -1,      -1,      -1,      Name,     Name,    Name,    Name,    Name    }, // Ws
	{ Ws1,     Done,    Attdef,  Attdef,   Attdef,  Attdef,  Attdef,  Attdef  }, // Name
	{ -1,      Done,    Attdef,  Attdef,   Attdef,  Attdef,  Attdef,  Attdef  }, // Ws1
	{ Ws2,     -1,      -1,      -1,       -1,      -1,      -1,      -1      }, // Attdef
	{ -1,      Atttype, Atttype, Atttype,  Atttype, Atttype, Atttype, Atttype }, // Ws2
	{ Ws3,     -1,      -1,      -1,       -1,      -1,      -1,      -1      }, // Attype
	{ -1,      Attval,  DDecH,   Attval,   Attval,  Attval,  Attval,  Attval  }, // Ws3
	{ -1,      -1,      -1,      -1,       DefImp,  DefFix,  DefReq,  -1      }, // DDecH
	{ Ws4,     Ws4,     -1,      -1,       -1,      -1,      -1,      -1      }, // DefReq
	{ Ws4,     Ws4,     -1,      -1,       -1,      -1,      -1,      -1      }, // DefImp
	{ Ws3,     -1,      -1,      -1,       -1,      -1,      -1,      -1      }, // DefFix
	{ Ws4,     Ws4,     -1,      -1,       -1,      -1,      -1,      -1      }, // Attval
	{ -1,      Done,    Attdef,  Attdef,   Attdef,  Attdef,  Attdef,  Attdef  }  // Ws4
    };
    signed char state;
    signed char input;

    if ( d->parseStack==0 || d->parseStack->isEmpty() ) {
	state = Init;
    } else {
	state = d->parseStack->pop().state;
#if defined(QT_QXML_DEBUG)
	tqDebug( "TQXmlSimpleReader: parseAttlistDecl (cont) in state %d", state );
#endif
	if ( !d->parseStack->isEmpty() ) {
	    ParseFunction function = d->parseStack->top().function;
	    if ( function == &TQXmlSimpleReader::eat_ws ) {
		d->parseStack->pop();
#if defined(QT_QXML_DEBUG)
		tqDebug( "TQXmlSimpleReader: eat_ws (cont)" );
#endif
	    }
	    if ( !(this->*function)() ) {
		parseFailed( &TQXmlSimpleReader::parseAttlistDecl, state );
		return false;
	    }
	}
    }

    for (;;) {
	switch ( state ) {
	    case Name:
		d->attDeclEName = name();
		break;
	    case Attdef:
		d->attDeclAName = name();
		break;
	    case Done:
		return true;
	    case -1:
		// Error
		reportParseError( XMLERR_LETTEREXPECTED );
		return false;
	}

	if ( atEnd() ) {
	    unexpectedEof( &TQXmlSimpleReader::parseAttlistDecl, state );
	    return false;
	}
	if        ( is_S(c) ) {
	    input = InpWs;
	} else if ( c.unicode() == '>' ) {
	    input = InpGt;
	} else if ( c.unicode() == '#' ) {
	    input = InpHash;
	} else if ( c.unicode() == 'A' ) {
	    input = InpA;
	} else if ( c.unicode() == 'I' ) {
	    input = InpI;
	} else if ( c.unicode() == 'F' ) {
	    input = InpF;
	} else if ( c.unicode() == 'R' ) {
	    input = InpR;
	} else {
	    input = InpUnknown;
	}
	state = table[state][input];

	switch ( state ) {
	    case Attlist:
		d->parseString_s = "ATTLIST";
		if ( !parseString() ) {
		    parseFailed( &TQXmlSimpleReader::parseAttlistDecl, state );
		    return false;
		}
		break;
	    case Ws:
	    case Ws1:
	    case Ws2:
	    case Ws3:
		if ( !eat_ws() ) {
		    parseFailed( &TQXmlSimpleReader::parseAttlistDecl, state );
		    return false;
		}
		break;
	    case Name:
		d->parseName_useRef = false;
		if ( !parseName() ) {
		    parseFailed( &TQXmlSimpleReader::parseAttlistDecl, state );
		    return false;
		}
		break;
	    case Attdef:
		d->parseName_useRef = false;
		if ( !parseName() ) {
		    parseFailed( &TQXmlSimpleReader::parseAttlistDecl, state );
		    return false;
		}
		break;
	    case Atttype:
		if ( !parseAttType() ) {
		    parseFailed( &TQXmlSimpleReader::parseAttlistDecl, state );
		    return false;
		}
		break;
	    case DDecH:
		next();
		break;
	    case DefReq:
		d->parseString_s = "REQUIRED";
		if ( !parseString() ) {
		    parseFailed( &TQXmlSimpleReader::parseAttlistDecl, state );
		    return false;
		}
		break;
	    case DefImp:
		d->parseString_s = "IMPLIED";
		if ( !parseString() ) {
		    parseFailed( &TQXmlSimpleReader::parseAttlistDecl, state );
		    return false;
		}
		break;
	    case DefFix:
		d->parseString_s = "FIXED";
		if ( !parseString() ) {
		    parseFailed( &TQXmlSimpleReader::parseAttlistDecl, state );
		    return false;
		}
		break;
	    case Attval:
		if ( !parseAttValue() ) {
		    parseFailed( &TQXmlSimpleReader::parseAttlistDecl, state );
		    return false;
		}
		break;
	    case Ws4:
		if ( declHnd ) {
		    // ### not all values are computed yet...
		    if ( !declHnd->attributeDecl( d->attDeclEName, d->attDeclAName, "", "", "" ) ) {
			reportParseError( declHnd->errorString() );
			return false;
		    }
		}
		if ( !eat_ws() ) {
		    parseFailed( &TQXmlSimpleReader::parseAttlistDecl, state );
		    return false;
		}
		break;
	    case Done:
		next();
		break;
	}
    }
}

/*
  Parse a AttType [54]
*/
bool TQXmlSimpleReader::parseAttType()
{
    const signed char Init             =  0;
    const signed char ST               =  1; // StringType
    const signed char TTI              =  2; // TokenizedType starting with 'I'
    const signed char TTI2             =  3; // TokenizedType helpstate
    const signed char TTI3             =  4; // TokenizedType helpstate
    const signed char TTE              =  5; // TokenizedType starting with 'E'
    const signed char TTEY             =  6; // TokenizedType starting with 'ENTITY'
    const signed char TTEI             =  7; // TokenizedType starting with 'ENTITI'
    const signed char N                =  8; // N read (TokenizedType or Notation)
    const signed char TTNM             =  9; // TokenizedType starting with 'NM'
    const signed char TTNM2            = 10; // TokenizedType helpstate
    const signed char NO               = 11; // Notation
    const signed char NO2              = 12; // Notation helpstate
    const signed char NO3              = 13; // Notation helpstate
    const signed char NOName           = 14; // Notation, read name
    const signed char NO4              = 15; // Notation helpstate
    const signed char EN               = 16; // Enumeration
    const signed char ENNmt            = 17; // Enumeration, read Nmtoken
    const signed char EN2              = 18; // Enumeration helpstate
    const signed char ADone            = 19; // almost done (make next and accept)
    const signed char Done             = 20;

    const signed char InpWs            =  0; // whitespace
    const signed char InpOp            =  1; // (
    const signed char InpCp            =  2; // )
    const signed char InpPipe          =  3; // |
    const signed char InpC             =  4; // C
    const signed char InpE             =  5; // E
    const signed char InpI             =  6; // I
    const signed char InpM             =  7; // M
    const signed char InpN             =  8; // N
    const signed char InpO             =  9; // O
    const signed char InpR             = 10; // R
    const signed char InpS             = 11; // S
    const signed char InpY             = 12; // Y
    const signed char InpUnknown       = 13;

    static const signed char table[19][14] = {
     /*  InpWs    InpOp    InpCp    InpPipe  InpC     InpE     InpI     InpM     InpN     InpO     InpR     InpS     InpY     InpUnknown */
	{ -1,      EN,      -1,      -1,      ST,      TTE,     TTI,     -1,      N,       -1,      -1,      -1,      -1,      -1     }, // Init
	{ Done,    Done,    Done,    Done,    Done,    Done,    Done,    Done,    Done,    Done,    Done,    Done,    Done,    Done   }, // ST
	{ Done,    Done,    Done,    Done,    Done,    Done,    Done,    Done,    Done,    Done,    TTI2,    Done,    Done,    Done   }, // TTI
	{ Done,    Done,    Done,    Done,    Done,    Done,    Done,    Done,    Done,    Done,    Done,    TTI3,    Done,    Done   }, // TTI2
	{ Done,    Done,    Done,    Done,    Done,    Done,    Done,    Done,    Done,    Done,    Done,    Done,    Done,    Done   }, // TTI3
	{ -1,      -1,      -1,      -1,      -1,      -1,      TTEI,    -1,      -1,      -1,      -1,      -1,      TTEY,    -1     }, // TTE
	{ Done,    Done,    Done,    Done,    Done,    Done,    Done,    Done,    Done,    Done,    Done,    Done,    Done,    Done   }, // TTEY
	{ Done,    Done,    Done,    Done,    Done,    Done,    Done,    Done,    Done,    Done,    Done,    Done,    Done,    Done   }, // TTEI
	{ -1,      -1,      -1,      -1,      -1,      -1,      -1,      TTNM,    -1,      NO,      -1,      -1,      -1,      -1     }, // N
	{ Done,    Done,    Done,    Done,    Done,    Done,    Done,    Done,    Done,    Done,    Done,    TTNM2,   Done,    Done   }, // TTNM
	{ Done,    Done,    Done,    Done,    Done,    Done,    Done,    Done,    Done,    Done,    Done,    Done,    Done,    Done   }, // TTNM2
	{ NO2,     -1,      -1,      -1,      -1,      -1,      -1,      -1,      -1,      -1,      -1,      -1,      -1,      -1     }, // NO
	{ -1,      NO3,     -1,      -1,      -1,      -1,      -1,      -1,      -1,      -1,      -1,      -1,      -1,      -1     }, // NO2
	{ NOName,  NOName,  NOName,  NOName,  NOName,  NOName,  NOName,  NOName,  NOName,  NOName,  NOName,  NOName,  NOName,  NOName }, // NO3
	{ NO4,     -1,      ADone,   NO3,     -1,      -1,      -1,      -1,      -1,      -1,      -1,      -1,      -1,      -1     }, // NOName
	{ -1,      -1,      ADone,   NO3,     -1,      -1,      -1,      -1,      -1,      -1,      -1,      -1,      -1,      -1     }, // NO4
	{ -1,      -1,      ENNmt,   -1,      ENNmt,   ENNmt,   ENNmt,   ENNmt,   ENNmt,   ENNmt,   ENNmt,   ENNmt,   ENNmt,   ENNmt  }, // EN
	{ EN2,     -1,      ADone,   EN,      -1,      -1,      -1,      -1,      -1,      -1,      -1,      -1,      -1,      -1     }, // ENNmt
	{ -1,      -1,      ADone,   EN,      -1,      -1,      -1,      -1,      -1,      -1,      -1,      -1,      -1,      -1     }  // EN2
    };
    signed char state;
    signed char input;

    if ( d->parseStack==0 || d->parseStack->isEmpty() ) {
	state = Init;
    } else {
	state = d->parseStack->pop().state;
#if defined(QT_QXML_DEBUG)
	tqDebug( "TQXmlSimpleReader: parseAttType (cont) in state %d", state );
#endif
	if ( !d->parseStack->isEmpty() ) {
	    ParseFunction function = d->parseStack->top().function;
	    if ( function == &TQXmlSimpleReader::eat_ws ) {
		d->parseStack->pop();
#if defined(QT_QXML_DEBUG)
		tqDebug( "TQXmlSimpleReader: eat_ws (cont)" );
#endif
	    }
	    if ( !(this->*function)() ) {
		parseFailed( &TQXmlSimpleReader::parseAttType, state );
		return false;
	    }
	}
    }

    for (;;) {
	switch ( state ) {
	    case ADone:
		return true;
	    case Done:
		return true;
	    case -1:
		// Error
		reportParseError( XMLERR_LETTEREXPECTED );
		return false;
	}

	if ( atEnd() ) {
	    unexpectedEof( &TQXmlSimpleReader::parseAttType, state );
	    return false;
	}
	if        ( is_S(c) ) {
	    input = InpWs;
	} else if ( c.unicode() == '(' ) {
	    input = InpOp;
	} else if ( c.unicode() == ')' ) {
	    input = InpCp;
	} else if ( c.unicode() == '|' ) {
	    input = InpPipe;
	} else if ( c.unicode() == 'C' ) {
	    input = InpC;
	} else if ( c.unicode() == 'E' ) {
	    input = InpE;
	} else if ( c.unicode() == 'I' ) {
	    input = InpI;
	} else if ( c.unicode() == 'M' ) {
	    input = InpM;
	} else if ( c.unicode() == 'N' ) {
	    input = InpN;
	} else if ( c.unicode() == 'O' ) {
	    input = InpO;
	} else if ( c.unicode() == 'R' ) {
	    input = InpR;
	} else if ( c.unicode() == 'S' ) {
	    input = InpS;
	} else if ( c.unicode() == 'Y' ) {
	    input = InpY;
	} else {
	    input = InpUnknown;
	}
	state = table[state][input];

	switch ( state ) {
	    case ST:
		d->parseString_s = "CDATA";
		if ( !parseString() ) {
		    parseFailed( &TQXmlSimpleReader::parseAttType, state );
		    return false;
		}
		break;
	    case TTI:
		d->parseString_s = "ID";
		if ( !parseString() ) {
		    parseFailed( &TQXmlSimpleReader::parseAttType, state );
		    return false;
		}
		break;
	    case TTI2:
		d->parseString_s = "REF";
		if ( !parseString() ) {
		    parseFailed( &TQXmlSimpleReader::parseAttType, state );
		    return false;
		}
		break;
	    case TTI3:
		next(); // S
		break;
	    case TTE:
		d->parseString_s = "ENTIT";
		if ( !parseString() ) {
		    parseFailed( &TQXmlSimpleReader::parseAttType, state );
		    return false;
		}
		break;
	    case TTEY:
		next(); // Y
		break;
	    case TTEI:
		d->parseString_s = "IES";
		if ( !parseString() ) {
		    parseFailed( &TQXmlSimpleReader::parseAttType, state );
		    return false;
		}
		break;
	    case N:
		next(); // N
		break;
	    case TTNM:
		d->parseString_s = "MTOKEN";
		if ( !parseString() ) {
		    parseFailed( &TQXmlSimpleReader::parseAttType, state );
		    return false;
		}
		break;
	    case TTNM2:
		next(); // S
		break;
	    case NO:
		d->parseString_s = "OTATION";
		if ( !parseString() ) {
		    parseFailed( &TQXmlSimpleReader::parseAttType, state );
		    return false;
		}
		break;
	    case NO2:
		if ( !eat_ws() ) {
		    parseFailed( &TQXmlSimpleReader::parseAttType, state );
		    return false;
		}
		break;
	    case NO3:
		if ( !next_eat_ws() ) {
		    parseFailed( &TQXmlSimpleReader::parseAttType, state );
		    return false;
		}
		break;
	    case NOName:
		d->parseName_useRef = false;
		if ( !parseName() ) {
		    parseFailed( &TQXmlSimpleReader::parseAttType, state );
		    return false;
		}
		break;
	    case NO4:
		if ( !eat_ws() ) {
		    parseFailed( &TQXmlSimpleReader::parseAttType, state );
		    return false;
		}
		break;
	    case EN:
		if ( !next_eat_ws() ) {
		    parseFailed( &TQXmlSimpleReader::parseAttType, state );
		    return false;
		}
		break;
	    case ENNmt:
		if ( !parseNmtoken() ) {
		    parseFailed( &TQXmlSimpleReader::parseAttType, state );
		    return false;
		}
		break;
	    case EN2:
		if ( !eat_ws() ) {
		    parseFailed( &TQXmlSimpleReader::parseAttType, state );
		    return false;
		}
		break;
	    case ADone:
		next();
		break;
	}
    }
}

/*
  Parse a AttValue [10]

  Precondition: the head stands on the beginning " or '

  If this function was successful, the head stands on the first
  character after the closing " or ' and the value of the attribute
  is in string().
*/
bool TQXmlSimpleReader::parseAttValue()
{
    const signed char Init             = 0;
    const signed char Dq               = 1; // double quotes were read
    const signed char DqRef            = 2; // read references in double quotes
    const signed char DqC              = 3; // signed character read in double quotes
    const signed char Sq               = 4; // single quotes were read
    const signed char SqRef            = 5; // read references in single quotes
    const signed char SqC              = 6; // signed character read in single quotes
    const signed char Done             = 7;

    const signed char InpDq            = 0; // "
    const signed char InpSq            = 1; // '
    const signed char InpAmp           = 2; // &
    const signed char InpLt            = 3; // <
    const signed char InpUnknown       = 4;

    static const signed char table[7][5] = {
     /*  InpDq  InpSq  InpAmp  InpLt InpUnknown */
	{ Dq,    Sq,    -1,     -1,   -1    }, // Init
	{ Done,  DqC,   DqRef,  -1,   DqC   }, // Dq
	{ Done,  DqC,   DqRef,  -1,   DqC   }, // DqRef
	{ Done,  DqC,   DqRef,  -1,   DqC   }, // DqC
	{ SqC,   Done,  SqRef,  -1,   SqC   }, // Sq
	{ SqC,   Done,  SqRef,  -1,   SqC   }, // SqRef
	{ SqC,   Done,  SqRef,  -1,   SqC   }  // SqRef
    };
    signed char state;
    signed char input;

    if ( d->parseStack==0 || d->parseStack->isEmpty() ) {
	state = Init;
    } else {
	state = d->parseStack->pop().state;
#if defined(QT_QXML_DEBUG)
	tqDebug( "TQXmlSimpleReader: parseAttValue (cont) in state %d", state );
#endif
	if ( !d->parseStack->isEmpty() ) {
	    ParseFunction function = d->parseStack->top().function;
	    if ( function == &TQXmlSimpleReader::eat_ws ) {
		d->parseStack->pop();
#if defined(QT_QXML_DEBUG)
		tqDebug( "TQXmlSimpleReader: eat_ws (cont)" );
#endif
	    }
	    if ( !(this->*function)() ) {
		parseFailed( &TQXmlSimpleReader::parseAttValue, state );
		return false;
	    }
	}
    }

    for (;;) {
	switch ( state ) {
	    case Done:
		return true;
	    case -1:
		// Error
		reportParseError( XMLERR_UNEXPECTEDCHARACTER );
		return false;
	}

	if ( atEnd() ) {
	    unexpectedEof( &TQXmlSimpleReader::parseAttValue, state );
	    return false;
	}
	if        ( c.unicode() == '"' ) {
	    input = InpDq;
	} else if ( c.unicode() == '\'' ) {
	    input = InpSq;
	} else if ( c.unicode() == '&' ) {
	    input = InpAmp;
	} else if ( c.unicode() == '<' ) {
	    input = InpLt;
	} else {
	    input = InpUnknown;
	}
	state = table[state][input];

	switch ( state ) {
	    case Dq:
	    case Sq:
		stringClear();
		next();
		break;
	    case DqRef:
	    case SqRef:
		d->parseReference_context = InAttributeValue;
		if ( !parseReference() ) {
		    parseFailed( &TQXmlSimpleReader::parseAttValue, state );
		    return false;
		}
		break;
	    case DqC:
	    case SqC:
		stringAddC();
		next();
		break;
	    case Done:
		next();
		break;
	}
    }
}

/*
  Parse a elementdecl [45].

  Precondition: the beginning '<!E' is already read and the head
  stands on the 'L' of '<!ELEMENT'
*/
bool TQXmlSimpleReader::parseElementDecl()
{
    const signed char Init             =  0;
    const signed char Elem             =  1; // parse the beginning string
    const signed char Ws1              =  2; // whitespace required
    const signed char Nam              =  3; // parse Name
    const signed char Ws2              =  4; // whitespace required
    const signed char Empty            =  5; // read EMPTY
    const signed char Any              =  6; // read ANY
    const signed char Cont             =  7; // read contentspec (except ANY or EMPTY)
    const signed char Mix              =  8; // read Mixed
    const signed char Mix2             =  9; //
    const signed char Mix3             = 10; //
    const signed char MixN1            = 11; //
    const signed char MixN2            = 12; //
    const signed char MixN3            = 13; //
    const signed char MixN4            = 14; //
    const signed char Cp               = 15; // parse cp
    const signed char Cp2              = 16; //
    const signed char WsD              = 17; // eat whitespace before Done
    const signed char Done             = 18;

    const signed char InpWs            =  0;
    const signed char InpGt            =  1; // >
    const signed char InpPipe          =  2; // |
    const signed char InpOp            =  3; // (
    const signed char InpCp            =  4; // )
    const signed char InpHash          =  5; // #
    const signed char InpQm            =  6; // ?
    const signed char InpAst           =  7; // *
    const signed char InpPlus          =  8; // +
    const signed char InpA             =  9; // A
    const signed char InpE             = 10; // E
    const signed char InpL             = 11; // L
    const signed char InpUnknown       = 12;

    static const signed char table[18][13] = {
     /*  InpWs   InpGt  InpPipe  InpOp  InpCp   InpHash  InpQm  InpAst  InpPlus  InpA    InpE    InpL    InpUnknown */
	{ -1,     -1,    -1,      -1,    -1,     -1,      -1,    -1,     -1,      -1,     -1,     Elem,   -1     }, // Init
	{ Ws1,    -1,    -1,      -1,    -1,     -1,      -1,    -1,     -1,      -1,     -1,     -1,     -1     }, // Elem
	{ -1,     -1,    -1,      -1,    -1,     -1,      -1,    -1,     -1,      Nam,    Nam,    Nam,    Nam    }, // Ws1
	{ Ws2,    -1,    -1,      -1,    -1,     -1,      -1,    -1,     -1,      -1,     -1,     -1,     -1     }, // Nam
	{ -1,     -1,    -1,      Cont,  -1,     -1,      -1,    -1,     -1,      Any,    Empty,  -1,     -1     }, // Ws2
	{ WsD,    Done,  -1,      -1,    -1,     -1,      -1,    -1,     -1,      -1,     -1,     -1,     -1     }, // Empty
	{ WsD,    Done,  -1,      -1,    -1,     -1,      -1,    -1,     -1,      -1,     -1,     -1,     -1     }, // Any
	{ -1,     -1,    -1,      Cp,    Cp,     Mix,     -1,    -1,     -1,      Cp,     Cp,     Cp,     Cp     }, // Cont
	{ Mix2,   -1,    MixN1,   -1,    Mix3,   -1,      -1,    -1,     -1,      -1,     -1,     -1,     -1     }, // Mix
	{ -1,     -1,    MixN1,   -1,    Mix3,   -1,      -1,    -1,     -1,      -1,     -1,     -1,     -1     }, // Mix2
	{ WsD,    Done,  -1,      -1,    -1,     -1,      -1,    WsD,    -1,      -1,     -1,     -1,     -1     }, // Mix3
	{ -1,     -1,    -1,      -1,    -1,     -1,      -1,    -1,     -1,      MixN2,  MixN2,  MixN2,  MixN2  }, // MixN1
	{ MixN3,  -1,    MixN1,   -1,    MixN4,  -1,      -1,    -1,     -1,      -1,     -1,     -1,     -1     }, // MixN2
	{ -1,     -1,    MixN1,   -1,    MixN4,  -1,      -1,    -1,     -1,      -1,     -1,     -1,     -1     }, // MixN3
	{ -1,     -1,    -1,      -1,    -1,     -1,      -1,    WsD,    -1,      -1,     -1,     -1,     -1     }, // MixN4
	{ WsD,    Done,  -1,      -1,    -1,     -1,      Cp2,   Cp2,    Cp2,     -1,     -1,     -1,     -1     }, // Cp
	{ WsD,    Done,  -1,      -1,    -1,     -1,      -1,    -1,     -1,      -1,     -1,     -1,     -1     }, // Cp2
	{ -1,     Done,  -1,      -1,    -1,     -1,      -1,    -1,     -1,      -1,     -1,     -1,     -1     }  // WsD
    };
    signed char state;
    signed char input;

    if ( d->parseStack==0 || d->parseStack->isEmpty() ) {
	state = Init;
    } else {
	state = d->parseStack->pop().state;
#if defined(QT_QXML_DEBUG)
	tqDebug( "TQXmlSimpleReader: parseElementDecl (cont) in state %d", state );
#endif
	if ( !d->parseStack->isEmpty() ) {
	    ParseFunction function = d->parseStack->top().function;
	    if ( function == &TQXmlSimpleReader::eat_ws ) {
		d->parseStack->pop();
#if defined(QT_QXML_DEBUG)
		tqDebug( "TQXmlSimpleReader: eat_ws (cont)" );
#endif
	    }
	    if ( !(this->*function)() ) {
		parseFailed( &TQXmlSimpleReader::parseElementDecl, state );
		return false;
	    }
	}
    }

    for (;;) {
	switch ( state ) {
	    case Done:
		return true;
	    case -1:
		reportParseError( XMLERR_UNEXPECTEDCHARACTER );
		return false;
	}

	if ( atEnd() ) {
	    unexpectedEof( &TQXmlSimpleReader::parseElementDecl, state );
	    return false;
	}
	if        ( is_S(c) ) {
	    input = InpWs;
	} else if ( c.unicode() == '>' ) {
	    input = InpGt;
	} else if ( c.unicode() == '|' ) {
	    input = InpPipe;
	} else if ( c.unicode() == '(' ) {
	    input = InpOp;
	} else if ( c.unicode() == ')' ) {
	    input = InpCp;
	} else if ( c.unicode() == '#' ) {
	    input = InpHash;
	} else if ( c.unicode() == '?' ) {
	    input = InpQm;
	} else if ( c.unicode() == '*' ) {
	    input = InpAst;
	} else if ( c.unicode() == '+' ) {
	    input = InpPlus;
	} else if ( c.unicode() == 'A' ) {
	    input = InpA;
	} else if ( c.unicode() == 'E' ) {
	    input = InpE;
	} else if ( c.unicode() == 'L' ) {
	    input = InpL;
	} else {
	    input = InpUnknown;
	}
	state = table[state][input];

	switch ( state ) {
	    case Elem:
		d->parseString_s = "LEMENT";
		if ( !parseString() ) {
		    parseFailed( &TQXmlSimpleReader::parseElementDecl, state );
		    return false;
		}
		break;
	    case Ws1:
		if ( !eat_ws() ) {
		    parseFailed( &TQXmlSimpleReader::parseElementDecl, state );
		    return false;
		}
		break;
	    case Nam:
		d->parseName_useRef = false;
		if ( !parseName() ) {
		    parseFailed( &TQXmlSimpleReader::parseElementDecl, state );
		    return false;
		}
		break;
	    case Ws2:
		if ( !eat_ws() ) {
		    parseFailed( &TQXmlSimpleReader::parseElementDecl, state );
		    return false;
		}
		break;
	    case Empty:
		d->parseString_s = "EMPTY";
		if ( !parseString() ) {
		    parseFailed( &TQXmlSimpleReader::parseElementDecl, state );
		    return false;
		}
		break;
	    case Any:
		d->parseString_s = "ANY";
		if ( !parseString() ) {
		    parseFailed( &TQXmlSimpleReader::parseElementDecl, state );
		    return false;
		}
		break;
	    case Cont:
		if ( !next_eat_ws() ) {
		    parseFailed( &TQXmlSimpleReader::parseElementDecl, state );
		    return false;
		}
		break;
	    case Mix:
		d->parseString_s = "#PCDATA";
		if ( !parseString() ) {
		    parseFailed( &TQXmlSimpleReader::parseElementDecl, state );
		    return false;
		}
		break;
	    case Mix2:
		if ( !eat_ws() ) {
		    parseFailed( &TQXmlSimpleReader::parseElementDecl, state );
		    return false;
		}
		break;
	    case Mix3:
		next();
		break;
	    case MixN1:
		if ( !next_eat_ws() ) {
		    parseFailed( &TQXmlSimpleReader::parseElementDecl, state );
		    return false;
		}
		break;
	    case MixN2:
		d->parseName_useRef = false;
		if ( !parseName() ) {
		    parseFailed( &TQXmlSimpleReader::parseElementDecl, state );
		    return false;
		}
		break;
	    case MixN3:
		if ( !eat_ws() ) {
		    parseFailed( &TQXmlSimpleReader::parseElementDecl, state );
		    return false;
		}
		break;
	    case MixN4:
		next();
		break;
	    case Cp:
		if ( !parseChoiceSeq() ) {
		    parseFailed( &TQXmlSimpleReader::parseElementDecl, state );
		    return false;
		}
		break;
	    case Cp2:
		next();
		break;
	    case WsD:
		if ( !next_eat_ws() ) {
		    parseFailed( &TQXmlSimpleReader::parseElementDecl, state );
		    return false;
		}
		break;
	    case Done:
		next();
		break;
	}
    }
}

/*
  Parse a NotationDecl [82].

  Precondition: the beginning '<!' is already read and the head
  stands on the 'N' of '<!NOTATION'
*/
bool TQXmlSimpleReader::parseNotationDecl()
{
    const signed char Init             = 0;
    const signed char Not              = 1; // read NOTATION
    const signed char Ws1              = 2; // eat whitespaces
    const signed char Nam              = 3; // read Name
    const signed char Ws2              = 4; // eat whitespaces
    const signed char ExtID            = 5; // parse ExternalID
    const signed char ExtIDR           = 6; // same as ExtID, but already reported
    const signed char Ws3              = 7; // eat whitespaces
    const signed char Done             = 8;

    const signed char InpWs            = 0;
    const signed char InpGt            = 1; // >
    const signed char InpN             = 2; // N
    const signed char InpUnknown       = 3;

    static const signed char table[8][4] = {
     /*  InpWs   InpGt  InpN    InpUnknown */
	{ -1,     -1,    Not,    -1     }, // Init
	{ Ws1,    -1,    -1,     -1     }, // Not
	{ -1,     -1,    Nam,    Nam    }, // Ws1
	{ Ws2,    Done,  -1,     -1     }, // Nam
	{ -1,     Done,  ExtID,  ExtID  }, // Ws2
	{ Ws3,    Done,  -1,     -1     }, // ExtID
	{ Ws3,    Done,  -1,     -1     }, // ExtIDR
	{ -1,     Done,  -1,     -1     }  // Ws3
    };
    signed char state;
    signed char input;

    if ( d->parseStack==0 || d->parseStack->isEmpty() ) {
	state = Init;
    } else {
	state = d->parseStack->pop().state;
#if defined(QT_QXML_DEBUG)
	tqDebug( "TQXmlSimpleReader: parseNotationDecl (cont) in state %d", state );
#endif
	if ( !d->parseStack->isEmpty() ) {
	    ParseFunction function = d->parseStack->top().function;
	    if ( function == &TQXmlSimpleReader::eat_ws ) {
		d->parseStack->pop();
#if defined(QT_QXML_DEBUG)
		tqDebug( "TQXmlSimpleReader: eat_ws (cont)" );
#endif
	    }
	    if ( !(this->*function)() ) {
		parseFailed( &TQXmlSimpleReader::parseNotationDecl, state );
		return false;
	    }
	}
    }

    for (;;) {
	switch ( state ) {
	    case ExtID:
		// call the handler
		if ( dtdHnd ) {
		    if ( !dtdHnd->notationDecl( name(), d->publicId, d->systemId ) ) {
			reportParseError( dtdHnd->errorString() );
			return false;
		    }
		}
		state = ExtIDR;
		break;
	    case Done:
		return true;
	    case -1:
		// Error
		reportParseError( XMLERR_UNEXPECTEDCHARACTER );
		return false;
	}

	if ( atEnd() ) {
	    unexpectedEof( &TQXmlSimpleReader::parseNotationDecl, state );
	    return false;
	}
	if        ( is_S(c) ) {
	    input = InpWs;
	} else if ( c.unicode() == '>' ) {
	    input = InpGt;
	} else if ( c.unicode() == 'N' ) {
	    input = InpN;
	} else {
	    input = InpUnknown;
	}
	state = table[state][input];

	switch ( state ) {
	    case Not:
		d->parseString_s = "NOTATION";
		if ( !parseString() ) {
		    parseFailed( &TQXmlSimpleReader::parseNotationDecl, state );
		    return false;
		}
		break;
	    case Ws1:
		if ( !eat_ws() ) {
		    parseFailed( &TQXmlSimpleReader::parseNotationDecl, state );
		    return false;
		}
		break;
	    case Nam:
		d->parseName_useRef = false;
		if ( !parseName() ) {
		    parseFailed( &TQXmlSimpleReader::parseNotationDecl, state );
		    return false;
		}
		break;
	    case Ws2:
		if ( !eat_ws() ) {
		    parseFailed( &TQXmlSimpleReader::parseNotationDecl, state );
		    return false;
		}
		break;
	    case ExtID:
	    case ExtIDR:
		d->parseExternalID_allowPublicID = true;
		if ( !parseExternalID() ) {
		    parseFailed( &TQXmlSimpleReader::parseNotationDecl, state );
		    return false;
		}
		break;
	    case Ws3:
		if ( !eat_ws() ) {
		    parseFailed( &TQXmlSimpleReader::parseNotationDecl, state );
		    return false;
		}
		break;
	    case Done:
		next();
		break;
	}
    }
}

/*
  Parse choice [49] or seq [50].

  Precondition: the beginning '('S? is already read and the head
  stands on the first non-whitespace character after it.
*/
bool TQXmlSimpleReader::parseChoiceSeq()
{
    const signed char Init             = 0;
    const signed char Ws1              = 1; // eat whitespace
    const signed char CorS             = 2; // choice or set
    const signed char Ws2              = 3; // eat whitespace
    const signed char More             = 4; // more cp to read
    const signed char Name             = 5; // read name
    const signed char Done             = 6; //

    const signed char InpWs            = 0; // S
    const signed char InpOp            = 1; // (
    const signed char InpCp            = 2; // )
    const signed char InpQm            = 3; // ?
    const signed char InpAst           = 4; // *
    const signed char InpPlus          = 5; // +
    const signed char InpPipe          = 6; // |
    const signed char InpComm          = 7; // ,
    const signed char InpUnknown       = 8;

    static const signed char table[6][9] = {
     /*  InpWs   InpOp  InpCp  InpQm  InpAst  InpPlus  InpPipe  InpComm  InpUnknown */
	{ -1,     Ws1,   -1,    -1,    -1,     -1,      -1,      -1,      Name  }, // Init
	{ -1,     CorS,  -1,    -1,    -1,     -1,      -1,      -1,      CorS  }, // Ws1
	{ Ws2,    -1,    Done,  Ws2,   Ws2,    Ws2,     More,    More,    -1    }, // CorS
	{ -1,     -1,    Done,  -1,    -1,     -1,      More,    More,    -1    }, // Ws2
	{ -1,     Ws1,   -1,    -1,    -1,     -1,      -1,      -1,      Name  }, // More (same as Init)
	{ Ws2,    -1,    Done,  Ws2,   Ws2,    Ws2,     More,    More,    -1    }  // Name (same as CorS)
    };
    signed char state;
    signed char input;

    if ( d->parseStack==0 || d->parseStack->isEmpty() ) {
	state = Init;
    } else {
	state = d->parseStack->pop().state;
#if defined(QT_QXML_DEBUG)
	tqDebug( "TQXmlSimpleReader: parseChoiceSeq (cont) in state %d", state );
#endif
	if ( !d->parseStack->isEmpty() ) {
	    ParseFunction function = d->parseStack->top().function;
	    if ( function == &TQXmlSimpleReader::eat_ws ) {
		d->parseStack->pop();
#if defined(QT_QXML_DEBUG)
		tqDebug( "TQXmlSimpleReader: eat_ws (cont)" );
#endif
	    }
	    if ( !(this->*function)() ) {
		parseFailed( &TQXmlSimpleReader::parseChoiceSeq, state );
		return false;
	    }
	}
    }

    for (;;) {
	switch ( state ) {
	    case Done:
		return true;
	    case -1:
		// Error
		reportParseError( XMLERR_UNEXPECTEDCHARACTER );
		return false;
	}

	if ( atEnd() ) {
	    unexpectedEof( &TQXmlSimpleReader::parseChoiceSeq, state );
	    return false;
	}
	if        ( is_S(c) ) {
	    input = InpWs;
	} else if ( c.unicode() == '(' ) {
	    input = InpOp;
	} else if ( c.unicode() == ')' ) {
	    input = InpCp;
	} else if ( c.unicode() == '?' ) {
	    input = InpQm;
	} else if ( c.unicode() == '*' ) {
	    input = InpAst;
	} else if ( c.unicode() == '+' ) {
	    input = InpPlus;
	} else if ( c.unicode() == '|' ) {
	    input = InpPipe;
	} else if ( c.unicode() == ',' ) {
	    input = InpComm;
	} else {
	    input = InpUnknown;
	}
	state = table[state][input];

	switch ( state ) {
	    case Ws1:
		if ( !next_eat_ws() ) {
		    parseFailed( &TQXmlSimpleReader::parseChoiceSeq, state );
		    return false;
		}
		break;
	    case CorS:
		if ( !parseChoiceSeq() ) {
		    parseFailed( &TQXmlSimpleReader::parseChoiceSeq, state );
		    return false;
		}
		break;
	    case Ws2:
		if ( !next_eat_ws() ) {
		    parseFailed( &TQXmlSimpleReader::parseChoiceSeq, state );
		    return false;
		}
		break;
	    case More:
		if ( !next_eat_ws() ) {
		    parseFailed( &TQXmlSimpleReader::parseChoiceSeq, state );
		    return false;
		}
		break;
	    case Name:
		d->parseName_useRef = false;
		if ( !parseName() ) {
		    parseFailed( &TQXmlSimpleReader::parseChoiceSeq, state );
		    return false;
		}
		break;
	    case Done:
		next();
		break;
	}
    }
}

bool TQXmlSimpleReader::isExpandedEntityValueTooLarge(TQString *errorMessage)
{
    TQMap<TQString, uint> literalEntitySizes;
    // The entity at (TQMap<TQString,) referenced the entities at (TQMap<TQString,) (uint>) times.
    TQMap<TQString, TQMap<TQString, uint> > referencesToOtherEntities;
    TQMap<TQString, uint> expandedSizes;

    // For every entity, check how many times all entity names were referenced in its value.
    TQMap<TQString,TQString>::ConstIterator toSearchIterator;
    for (toSearchIterator = d->entities.begin(); toSearchIterator != d->entities.end(); ++toSearchIterator) {
        TQString toSearch = toSearchIterator.key();
        // The amount of characters that weren't entity names, but literals, like 'X'.
        TQString leftOvers = toSearchIterator.data();
        TQMap<TQString,TQString>::ConstIterator entityNameIterator;
        // How many times was entityName referenced by toSearch?
        for (entityNameIterator = d->entities.begin(); entityNameIterator != d->entities.end(); ++entityNameIterator) {
            TQString entityName = entityNameIterator.key();
            for (int i = 0; i >= 0 && (uint) i < leftOvers.length(); ) {
                i = leftOvers.find(TQString::fromLatin1("&%1;").arg(entityName), i);
                if (i != -1) {
                    leftOvers.remove(i, entityName.length() + 2U);
                    // The entityName we're currently trying to find was matched in this string; increase our count.
                    ++referencesToOtherEntities[toSearch][entityName];
                }
            }
        }
        literalEntitySizes[toSearch] = leftOvers.length();
    }

    TQMap<TQString, TQMap<TQString, uint> >::ConstIterator entityIterator;
    for (entityIterator = referencesToOtherEntities.begin(); entityIterator != referencesToOtherEntities.end(); ++entityIterator) {
        TQString entity = entityIterator.key();
        expandedSizes[entity] = literalEntitySizes[entity];
        TQMap<TQString, uint>::ConstIterator referenceToIterator;
        for (referenceToIterator = entityIterator.data().begin(); referenceToIterator != entityIterator.data().end(); ++referenceToIterator) {
            TQString referenceTo = referenceToIterator.key();
            const uint references = referenceToIterator.data();
            // The total size of an entity's value is the expanded size of all of its referenced entities, plus its literal size.
            expandedSizes[entity] += expandedSizes[referenceTo] * references + literalEntitySizes[referenceTo] * references;
        }

        if (expandedSizes[entity] > entityCharacterLimit) {
            if (errorMessage) {
                *errorMessage = TQString::fromLatin1("The XML entity \"%1\" expands to a string that is too large to process (%2 characters > %3).");
                *errorMessage = (*errorMessage).arg(entity).arg(expandedSizes[entity]).arg(entityCharacterLimit);
            }
            return true;
        }
    }
    return false;
}

/*
  Parse a EntityDecl [70].

  Precondition: the beginning '<!E' is already read and the head
  stand on the 'N' of '<!ENTITY'
*/
bool TQXmlSimpleReader::parseEntityDecl()
{
    const signed char Init             =  0;
    const signed char Ent              =  1; // parse "ENTITY"
    const signed char Ws1              =  2; // white space read
    const signed char Name             =  3; // parse name
    const signed char Ws2              =  4; // white space read
    const signed char EValue           =  5; // parse entity value
    const signed char EValueR          =  6; // same as EValue, but already reported
    const signed char ExtID            =  7; // parse ExternalID
    const signed char Ws3              =  8; // white space read
    const signed char Ndata            =  9; // parse "NDATA"
    const signed char Ws4              = 10; // white space read
    const signed char NNam             = 11; // parse name
    const signed char NNamR            = 12; // same as NNam, but already reported
    const signed char PEDec            = 13; // parse PEDecl
    const signed char Ws6              = 14; // white space read
    const signed char PENam            = 15; // parse name
    const signed char Ws7              = 16; // white space read
    const signed char PEVal            = 17; // parse entity value
    const signed char PEValR           = 18; // same as PEVal, but already reported
    const signed char PEEID            = 19; // parse ExternalID
    const signed char PEEIDR           = 20; // same as PEEID, but already reported
    const signed char WsE              = 21; // white space read
    const signed char Done             = 22;
    const signed char EDDone           = 23; // done, but also report an external, unparsed entity decl

    const signed char InpWs            = 0; // white space
    const signed char InpPer           = 1; // %
    const signed char InpQuot          = 2; // " or '
    const signed char InpGt            = 3; // >
    const signed char InpN             = 4; // N
    const signed char InpUnknown       = 5;

    static const signed char table[22][6] = {
     /*  InpWs  InpPer  InpQuot  InpGt  InpN    InpUnknown */
	{ -1,    -1,     -1,      -1,    Ent,    -1      }, // Init
	{ Ws1,   -1,     -1,      -1,    -1,     -1      }, // Ent
	{ -1,    PEDec,  -1,      -1,    Name,   Name    }, // Ws1
	{ Ws2,   -1,     -1,      -1,    -1,     -1      }, // Name
	{ -1,    -1,     EValue,  -1,    -1,     ExtID   }, // Ws2
	{ WsE,   -1,     -1,      Done,  -1,     -1      }, // EValue
	{ WsE,   -1,     -1,      Done,  -1,     -1      }, // EValueR
	{ Ws3,   -1,     -1,      EDDone,-1,     -1      }, // ExtID
	{ -1,    -1,     -1,      EDDone,Ndata,  -1      }, // Ws3
	{ Ws4,   -1,     -1,      -1,    -1,     -1      }, // Ndata
	{ -1,    -1,     -1,      -1,    NNam,   NNam    }, // Ws4
	{ WsE,   -1,     -1,      Done,  -1,     -1      }, // NNam
	{ WsE,   -1,     -1,      Done,  -1,     -1      }, // NNamR
	{ Ws6,   -1,     -1,      -1,    -1,     -1      }, // PEDec
	{ -1,    -1,     -1,      -1,    PENam,  PENam   }, // Ws6
	{ Ws7,   -1,     -1,      -1,    -1,     -1      }, // PENam
	{ -1,    -1,     PEVal,   -1,    -1,     PEEID   }, // Ws7
	{ WsE,   -1,     -1,      Done,  -1,     -1      }, // PEVal
	{ WsE,   -1,     -1,      Done,  -1,     -1      }, // PEValR
	{ WsE,   -1,     -1,      Done,  -1,     -1      }, // PEEID
	{ WsE,   -1,     -1,      Done,  -1,     -1      }, // PEEIDR
	{ -1,    -1,     -1,      Done,  -1,     -1      }  // WsE
    };
    signed char state;
    signed char input;

    if ( d->parseStack==0 || d->parseStack->isEmpty() ) {
	state = Init;
    } else {
	state = d->parseStack->pop().state;
#if defined(QT_QXML_DEBUG)
	tqDebug( "TQXmlSimpleReader: parseEntityDecl (cont) in state %d", state );
#endif
	if ( !d->parseStack->isEmpty() ) {
	    ParseFunction function = d->parseStack->top().function;
	    if ( function == &TQXmlSimpleReader::eat_ws ) {
		d->parseStack->pop();
#if defined(QT_QXML_DEBUG)
		tqDebug( "TQXmlSimpleReader: eat_ws (cont)" );
#endif
	    }
	    if ( !(this->*function)() ) {
		parseFailed( &TQXmlSimpleReader::parseEntityDecl, state );
		return false;
	    }
	}
    }

    for (;;) {
	switch ( state ) {
	    case EValue:
		if (  !entityExist( name() ) ) {
		    TQString errorMessage;
		    if (isExpandedEntityValueTooLarge(&errorMessage)) {
		        reportParseError(errorMessage);
		        return false;
		    }

		    d->entities.insert( name(), string() );
		    if ( declHnd ) {
			if ( !declHnd->internalEntityDecl( name(), string() ) ) {
			    reportParseError( declHnd->errorString() );
			    return false;
			}
		    }
		}
		state = EValueR;
		break;
	    case NNam:
		if (  !entityExist( name() ) ) {
		    d->externEntities.insert( name(), TQXmlSimpleReaderPrivate::ExternEntity( d->publicId, d->systemId, ref() ) );
		    if ( dtdHnd ) {
			if ( !dtdHnd->unparsedEntityDecl( name(), d->publicId, d->systemId, ref() ) ) {
			    reportParseError( declHnd->errorString() );
			    return false;
			}
		    }
		}
		state = NNamR;
		break;
	    case PEVal:
		if (  !entityExist( name() ) ) {
		    d->parameterEntities.insert( name(), string() );
		    if ( declHnd ) {
			if ( !declHnd->internalEntityDecl( TQString("%")+name(), string() ) ) {
			    reportParseError( declHnd->errorString() );
			    return false;
			}
		    }
		}
		state = PEValR;
		break;
	    case PEEID:
		if (  !entityExist( name() ) ) {
		    d->externParameterEntities.insert( name(), TQXmlSimpleReaderPrivate::ExternParameterEntity( d->publicId, d->systemId ) );
		    if ( declHnd ) {
			if ( !declHnd->externalEntityDecl( TQString("%")+name(), d->publicId, d->systemId ) ) {
			    reportParseError( declHnd->errorString() );
			    return false;
			}
		    }
		}
		state = PEEIDR;
		break;
	    case EDDone:
		if (  !entityExist( name() ) ) {
		    d->externEntities.insert( name(), TQXmlSimpleReaderPrivate::ExternEntity( d->publicId, d->systemId, TQString::null ) );
		    if ( declHnd ) {
			if ( !declHnd->externalEntityDecl( name(), d->publicId, d->systemId ) ) {
			    reportParseError( declHnd->errorString() );
			    return false;
			}
		    }
		}
		return true;
	    case Done:
		return true;
	    case -1:
		// Error
		reportParseError( XMLERR_LETTEREXPECTED );
		return false;
	}

	if ( atEnd() ) {
	    unexpectedEof( &TQXmlSimpleReader::parseEntityDecl, state );
	    return false;
	}
	if        ( is_S(c) ) {
	    input = InpWs;
	} else if ( c.unicode() == '%' ) {
	    input = InpPer;
	} else if ( c.unicode() == '"' || c.unicode() == '\'' ) {
	    input = InpQuot;
	} else if ( c.unicode() == '>' ) {
	    input = InpGt;
	} else if ( c.unicode() == 'N' ) {
	    input = InpN;
	} else {
	    input = InpUnknown;
	}
	state = table[state][input];

	switch ( state ) {
	    case Ent:
		d->parseString_s = "NTITY";
		if ( !parseString() ) {
		    parseFailed( &TQXmlSimpleReader::parseEntityDecl, state );
		    return false;
		}
		break;
	    case Ws1:
		if ( !eat_ws() ) {
		    parseFailed( &TQXmlSimpleReader::parseEntityDecl, state );
		    return false;
		}
		break;
	    case Name:
		d->parseName_useRef = false;
		if ( !parseName() ) {
		    parseFailed( &TQXmlSimpleReader::parseEntityDecl, state );
		    return false;
		}
		break;
	    case Ws2:
		if ( !eat_ws() ) {
		    parseFailed( &TQXmlSimpleReader::parseEntityDecl, state );
		    return false;
		}
		break;
	    case EValue:
	    case EValueR:
		if ( !parseEntityValue() ) {
		    parseFailed( &TQXmlSimpleReader::parseEntityDecl, state );
		    return false;
		}
		break;
	    case ExtID:
		d->parseExternalID_allowPublicID = false;
		if ( !parseExternalID() ) {
		    parseFailed( &TQXmlSimpleReader::parseEntityDecl, state );
		    return false;
		}
		break;
	    case Ws3:
		if ( !eat_ws() ) {
		    parseFailed( &TQXmlSimpleReader::parseEntityDecl, state );
		    return false;
		}
		break;
	    case Ndata:
		d->parseString_s = "NDATA";
		if ( !parseString() ) {
		    parseFailed( &TQXmlSimpleReader::parseEntityDecl, state );
		    return false;
		}
		break;
	    case Ws4:
		if ( !eat_ws() ) {
		    parseFailed( &TQXmlSimpleReader::parseEntityDecl, state );
		    return false;
		}
		break;
	    case NNam:
	    case NNamR:
		d->parseName_useRef = true;
		if ( !parseName() ) {
		    parseFailed( &TQXmlSimpleReader::parseEntityDecl, state );
		    return false;
		}
		break;
	    case PEDec:
		next();
		break;
	    case Ws6:
		if ( !eat_ws() ) {
		    parseFailed( &TQXmlSimpleReader::parseEntityDecl, state );
		    return false;
		}
		break;
	    case PENam:
		d->parseName_useRef = false;
		if ( !parseName() ) {
		    parseFailed( &TQXmlSimpleReader::parseEntityDecl, state );
		    return false;
		}
		break;
	    case Ws7:
		if ( !eat_ws() ) {
		    parseFailed( &TQXmlSimpleReader::parseEntityDecl, state );
		    return false;
		}
		break;
	    case PEVal:
	    case PEValR:
		if ( !parseEntityValue() ) {
		    parseFailed( &TQXmlSimpleReader::parseEntityDecl, state );
		    return false;
		}
		break;
	    case PEEID:
	    case PEEIDR:
		d->parseExternalID_allowPublicID = false;
		if ( !parseExternalID() ) {
		    parseFailed( &TQXmlSimpleReader::parseEntityDecl, state );
		    return false;
		}
		break;
	    case WsE:
		if ( !eat_ws() ) {
		    parseFailed( &TQXmlSimpleReader::parseEntityDecl, state );
		    return false;
		}
		break;
	    case EDDone:
		next();
		break;
	    case Done:
		next();
		break;
	}
    }
}

/*
  Parse a EntityValue [9]
*/
bool TQXmlSimpleReader::parseEntityValue()
{
    const signed char Init             = 0;
    const signed char Dq               = 1; // EntityValue is double quoted
    const signed char DqC              = 2; // signed character
    const signed char DqPER            = 3; // PERefence
    const signed char DqRef            = 4; // Reference
    const signed char Sq               = 5; // EntityValue is double quoted
    const signed char SqC              = 6; // signed character
    const signed char SqPER            = 7; // PERefence
    const signed char SqRef            = 8; // Reference
    const signed char Done             = 9;

    const signed char InpDq            = 0; // "
    const signed char InpSq            = 1; // '
    const signed char InpAmp           = 2; // &
    const signed char InpPer           = 3; // %
    const signed char InpUnknown       = 4;

    static const signed char table[9][5] = {
     /*  InpDq  InpSq  InpAmp  InpPer  InpUnknown */
	{ Dq,    Sq,    -1,     -1,     -1    }, // Init
	{ Done,  DqC,   DqRef,  DqPER,  DqC   }, // Dq
	{ Done,  DqC,   DqRef,  DqPER,  DqC   }, // DqC
	{ Done,  DqC,   DqRef,  DqPER,  DqC   }, // DqPER
	{ Done,  DqC,   DqRef,  DqPER,  DqC   }, // DqRef
	{ SqC,   Done,  SqRef,  SqPER,  SqC   }, // Sq
	{ SqC,   Done,  SqRef,  SqPER,  SqC   }, // SqC
	{ SqC,   Done,  SqRef,  SqPER,  SqC   }, // SqPER
	{ SqC,   Done,  SqRef,  SqPER,  SqC   }  // SqRef
    };
    signed char state;
    signed char input;

    if ( d->parseStack==0 || d->parseStack->isEmpty() ) {
	state = Init;
    } else {
	state = d->parseStack->pop().state;
#if defined(QT_QXML_DEBUG)
	tqDebug( "TQXmlSimpleReader: parseEntityValue (cont) in state %d", state );
#endif
	if ( !d->parseStack->isEmpty() ) {
	    ParseFunction function = d->parseStack->top().function;
	    if ( function == &TQXmlSimpleReader::eat_ws ) {
		d->parseStack->pop();
#if defined(QT_QXML_DEBUG)
		tqDebug( "TQXmlSimpleReader: eat_ws (cont)" );
#endif
	    }
	    if ( !(this->*function)() ) {
		parseFailed( &TQXmlSimpleReader::parseEntityValue, state );
		return false;
	    }
	}
    }

    for (;;) {
	switch ( state ) {
	    case Done:
		return true;
	    case -1:
		// Error
		reportParseError( XMLERR_LETTEREXPECTED );
		return false;
	}

	if ( atEnd() ) {
	    unexpectedEof( &TQXmlSimpleReader::parseEntityValue, state );
	    return false;
	}
	if        ( c.unicode() == '"' ) {
	    input = InpDq;
	} else if ( c.unicode() == '\'' ) {
	    input = InpSq;
	} else if ( c.unicode() == '&' ) {
	    input = InpAmp;
	} else if ( c.unicode() == '%' ) {
	    input = InpPer;
	} else {
	    input = InpUnknown;
	}
	state = table[state][input];

	switch ( state ) {
	    case Dq:
	    case Sq:
		stringClear();
		next();
		break;
	    case DqC:
	    case SqC:
		stringAddC();
		next();
		break;
	    case DqPER:
	    case SqPER:
		d->parsePEReference_context = InEntityValue;
		if ( !parsePEReference() ) {
		    parseFailed( &TQXmlSimpleReader::parseEntityValue, state );
		    return false;
		}
		break;
	    case DqRef:
	    case SqRef:
		d->parseReference_context = InEntityValue;
		if ( !parseReference() ) {
		    parseFailed( &TQXmlSimpleReader::parseEntityValue, state );
		    return false;
		}
		break;
	    case Done:
		next();
		break;
	}
    }
}

/*
  Parse a comment [15].

  Precondition: the beginning '<!' of the comment is already read and the head
  stands on the first '-' of '<!--'.

  If this funktion was successful, the head-position is on the first
  character after the comment.
*/
bool TQXmlSimpleReader::parseComment()
{
    const signed char Init             = 0;
    const signed char Dash1            = 1; // the first dash was read
    const signed char Dash2            = 2; // the second dash was read
    const signed char Com              = 3; // read comment
    const signed char Com2             = 4; // read comment (help state)
    const signed char ComE             = 5; // finished reading comment
    const signed char Done             = 6;

    const signed char InpDash          = 0; // -
    const signed char InpGt            = 1; // >
    const signed char InpUnknown       = 2;

    static const signed char table[6][3] = {
     /*  InpDash  InpGt  InpUnknown */
	{ Dash1,   -1,    -1  }, // Init
	{ Dash2,   -1,    -1  }, // Dash1
	{ Com2,    Com,   Com }, // Dash2
	{ Com2,    Com,   Com }, // Com
	{ ComE,    Com,   Com }, // Com2
	{ -1,      Done,  -1  }  // ComE
    };
    signed char state;
    signed char input;

    if ( d->parseStack==0 || d->parseStack->isEmpty() ) {
	state = Init;
    } else {
	state = d->parseStack->pop().state;
#if defined(QT_QXML_DEBUG)
	tqDebug( "TQXmlSimpleReader: parseComment (cont) in state %d", state );
#endif
	if ( !d->parseStack->isEmpty() ) {
	    ParseFunction function = d->parseStack->top().function;
	    if ( function == &TQXmlSimpleReader::eat_ws ) {
		d->parseStack->pop();
#if defined(QT_QXML_DEBUG)
		tqDebug( "TQXmlSimpleReader: eat_ws (cont)" );
#endif
	    }
	    if ( !(this->*function)() ) {
		parseFailed( &TQXmlSimpleReader::parseComment, state );
		return false;
	    }
	}
    }

    for (;;) {
	switch ( state ) {
	    case Dash2:
		stringClear();
		break;
	    case Com2:
		// if next character is not a dash than don't skip it
		if ( !atEnd() && c.unicode() != '-' )
		    stringAddC( '-' );
		break;
	    case Done:
		return true;
	    case -1:
		// Error
		reportParseError( XMLERR_ERRORPARSINGCOMMENT );
		return false;
	}

	if ( atEnd() ) {
	    unexpectedEof( &TQXmlSimpleReader::parseComment, state );
	    return false;
	}
	if        ( c.unicode() == '-' ) {
	    input = InpDash;
	} else if ( c.unicode() == '>' ) {
	    input = InpGt;
	} else {
	    input = InpUnknown;
	}
	state = table[state][input];

	switch ( state ) {
	    case Dash1:
		next();
		break;
	    case Dash2:
		next();
		break;
	    case Com:
		stringAddC();
		next();
		break;
	    case Com2:
		next();
		break;
	    case ComE:
		next();
		break;
	    case Done:
		next();
		break;
	}
    }
}

/*
    Parse an Attribute [41].

    Precondition: the head stands on the first character of the name
    of the attribute (i.e. all whitespaces are already parsed).

    The head stand on the next character after the end quotes. The
    variable name contains the name of the attribute and the variable
    string contains the value of the attribute.
*/
bool TQXmlSimpleReader::parseAttribute()
{
    const int Init             = 0;
    const int PName            = 1; // parse name
    const int Ws               = 2; // eat ws
    const int Eq               = 3; // the '=' was read
    const int Quotes           = 4; // " or ' were read

    const int InpNameBe        = 0;
    const int InpEq            = 1; // =
    const int InpDq            = 2; // "
    const int InpSq            = 3; // '
    const int InpUnknown       = 4;

    static const int table[4][5] = {
     /*  InpNameBe  InpEq  InpDq    InpSq    InpUnknown */
	{ PName,     -1,    -1,      -1,      -1    }, // Init
	{ -1,        Eq,    -1,      -1,      Ws    }, // PName
	{ -1,        Eq,    -1,      -1,      -1    }, // Ws
	{ -1,        -1,    Quotes,  Quotes,  -1    }  // Eq
    };
    int state;
    int input;

    if ( d->parseStack==0 || d->parseStack->isEmpty() ) {
	state = Init;
    } else {
	state = d->parseStack->pop().state;
#if defined(QT_QXML_DEBUG)
	tqDebug( "TQXmlSimpleReader: parseAttribute (cont) in state %d", state );
#endif
	if ( !d->parseStack->isEmpty() ) {
	    ParseFunction function = d->parseStack->top().function;
	    if ( function == &TQXmlSimpleReader::eat_ws ) {
		d->parseStack->pop();
#if defined(QT_QXML_DEBUG)
		tqDebug( "TQXmlSimpleReader: eat_ws (cont)" );
#endif
	    }
	    if ( !(this->*function)() ) {
		parseFailed( &TQXmlSimpleReader::parseAttribute, state );
		return false;
	    }
	}
    }

    for (;;) {
	switch ( state ) {
	    case Quotes:
		// Done
		return true;
	    case -1:
		// Error
		reportParseError( XMLERR_UNEXPECTEDCHARACTER );
		return false;
	}

	if ( atEnd() ) {
	    unexpectedEof( &TQXmlSimpleReader::parseAttribute, state );
	    return false;
	}
	if        ( determineNameChar(c) == NameBeginning ) {
	    input = InpNameBe;
	} else if ( c.unicode() == '=' ) {
	    input = InpEq;
	} else if ( c.unicode() == '"' ) {
	    input = InpDq;
	} else if ( c.unicode() == '\'' ) {
	    input = InpSq;
	} else {
	    input = InpUnknown;
	}
	state = table[state][input];

	switch ( state ) {
	    case PName:
		d->parseName_useRef = false;
		if ( !parseName() ) {
		    parseFailed( &TQXmlSimpleReader::parseAttribute, state );
		    return false;
		}
		break;
	    case Ws:
		if ( !eat_ws() ) {
		    parseFailed( &TQXmlSimpleReader::parseAttribute, state );
		    return false;
		}
		break;
	    case Eq:
		if ( !next_eat_ws() ) {
		    parseFailed( &TQXmlSimpleReader::parseAttribute, state );
		    return false;
		}
		break;
	    case Quotes:
		if ( !parseAttValue() ) {
		    parseFailed( &TQXmlSimpleReader::parseAttribute, state );
		    return false;
		}
		break;
	}
    }
}

/*
  Parse a Name [5] and store the name in name or ref (if useRef is true).
*/
bool TQXmlSimpleReader::parseName()
{
    const int Init             = 0;
    const int Name1            = 1; // parse first signed character of the name
    const int Name             = 2; // parse name
    const int Done             = 3;

    const int InpNameBe        = 0; // name beginning signed characters
    const int InpNameCh        = 1; // NameChar without InpNameBe
    const int InpUnknown       = 2;

    Q_ASSERT(InpNameBe == (int)NameBeginning);
    Q_ASSERT(InpNameCh == (int)NameNotBeginning);
    Q_ASSERT(InpUnknown == (int)NotName);

    static const int table[3][3] = {
     /*  InpNameBe  InpNameCh  InpUnknown */
	{ Name1,     -1,        -1    }, // Init
	{ Name,      Name,      Done  }, // Name1
	{ Name,      Name,      Done  }  // Name
    };
    int state;

    if ( d->parseStack==0 || d->parseStack->isEmpty() ) {
	state = Init;
    } else {
	state = d->parseStack->pop().state;
#if defined(QT_QXML_DEBUG)
	tqDebug( "TQXmlSimpleReader: parseName (cont) in state %d", state );
#endif
	if ( !d->parseStack->isEmpty() ) {
	    ParseFunction function = d->parseStack->top().function;
	    if ( function == &TQXmlSimpleReader::eat_ws ) {
		d->parseStack->pop();
#if defined(QT_QXML_DEBUG)
		tqDebug( "TQXmlSimpleReader: eat_ws (cont)" );
#endif
	    }
	    if ( !(this->*function)() ) {
		parseFailed( &TQXmlSimpleReader::parseName, state );
		return false;
	    }
	}
    }

    for (;;) {
	switch ( state ) {
	    case Done:
		return true;
	    case -1:
		// Error
		reportParseError( XMLERR_LETTEREXPECTED );
		return false;
	}

	if ( atEnd() ) {
	    unexpectedEof( &TQXmlSimpleReader::parseName, state );
	    return false;
	}

        // we can safely do the (int) cast thanks to the Q_ASSERTs earlier in this function
        state = table[state][(int)fastDetermineNameChar(c)];

	switch ( state ) {
	    case Name1:
		if ( d->parseName_useRef ) {
		    refClear();
		    refAddC();
		} else {
		    nameClear();
		    nameAddC();
		}
		next();
		break;
	    case Name:
		if ( d->parseName_useRef ) {
		    refAddC();
		} else {
		    nameAddC();
		}
		next();
		break;
	}
    }
}

/*
  Parse a Nmtoken [7] and store the name in name.
*/
bool TQXmlSimpleReader::parseNmtoken()
{
    const signed char Init             = 0;
    const signed char NameF            = 1;
    const signed char Name             = 2;
    const signed char Done             = 3;

    const signed char InpNameCh        = 0; // NameChar without InpNameBe
    const signed char InpUnknown       = 1;

    static const signed char table[3][2] = {
     /*  InpNameCh  InpUnknown */
	{ NameF,     -1    }, // Init
	{ Name,      Done  }, // NameF
	{ Name,      Done  }  // Name
    };
    signed char state;
    signed char input;

    if ( d->parseStack==0 || d->parseStack->isEmpty() ) {
	state = Init;
    } else {
	state = d->parseStack->pop().state;
#if defined(QT_QXML_DEBUG)
	tqDebug( "TQXmlSimpleReader: parseNmtoken (cont) in state %d", state );
#endif
	if ( !d->parseStack->isEmpty() ) {
	    ParseFunction function = d->parseStack->top().function;
	    if ( function == &TQXmlSimpleReader::eat_ws ) {
		d->parseStack->pop();
#if defined(QT_QXML_DEBUG)
		tqDebug( "TQXmlSimpleReader: eat_ws (cont)" );
#endif
	    }
	    if ( !(this->*function)() ) {
		parseFailed( &TQXmlSimpleReader::parseNmtoken, state );
		return false;
	    }
	}
    }

    for (;;) {
	switch ( state ) {
	    case Done:
		return true;
	    case -1:
		// Error
		reportParseError( XMLERR_LETTEREXPECTED );
		return false;
	}

	if ( atEnd() ) {
	    unexpectedEof( &TQXmlSimpleReader::parseNmtoken, state );
	    return false;
	}
        if (determineNameChar(c) == NotName) {
            input = InpUnknown;
        } else {
	    input = InpNameCh;
	}
	state = table[state][input];

	switch ( state ) {
	    case NameF:
		nameClear();
		nameAddC();
		next();
		break;
	    case Name:
		nameAddC();
		next();
		break;
	}
    }
}

/*
  Parse a Reference [67].

  parseReference_charDataRead is set to true if the reference must not be
  parsed. The character(s) which the reference mapped to are appended to
  string. The head stands on the first character after the reference.

  parseReference_charDataRead is set to false if the reference must be parsed.
  The charachter(s) which the reference mapped to are inserted at the reference
  position. The head stands on the first character of the replacement).
*/
bool TQXmlSimpleReader::parseReference()
{
    // temporary variables (only used in very local context, so they don't
    // interfere with incremental parsing)
    uint tmp;
    bool ok;

    const signed char Init             =  0;
    const signed char SRef             =  1; // start of a reference
    const signed char ChRef            =  2; // parse CharRef
    const signed char ChDec            =  3; // parse CharRef decimal
    const signed char ChHexS           =  4; // start CharRef hexadecimal
    const signed char ChHex            =  5; // parse CharRef hexadecimal
    const signed char Name             =  6; // parse name
    const signed char DoneD            =  7; // done CharRef decimal
    const signed char DoneH            =  8; // done CharRef hexadecimal
    const signed char DoneN            =  9; // done EntityRef

    const signed char InpAmp           = 0; // &
    const signed char InpSemi          = 1; // ;
    const signed char InpHash          = 2; // #
    const signed char InpX             = 3; // x
    const signed char InpNum           = 4; // 0-9
    const signed char InpHex           = 5; // a-f A-F
    const signed char InpUnknown       = 6;

    static const signed char table[8][7] = {
     /*  InpAmp  InpSemi  InpHash  InpX     InpNum  InpHex  InpUnknown */
	{ SRef,   -1,      -1,      -1,      -1,     -1,     -1    }, // Init
	{ -1,     -1,      ChRef,   Name,    Name,   Name,   Name  }, // SRef
	{ -1,     -1,      -1,      ChHexS,  ChDec,  -1,     -1    }, // ChRef
	{ -1,     DoneD,   -1,      -1,      ChDec,  -1,     -1    }, // ChDec
	{ -1,     -1,      -1,      -1,      ChHex,  ChHex,  -1    }, // ChHexS
	{ -1,     DoneH,   -1,      -1,      ChHex,  ChHex,  -1    }, // ChHex
	{ -1,     DoneN,   -1,      -1,      -1,     -1,     -1    }  // Name
    };
    signed char state;
    signed char input;

    if ( d->parseStack==0 || d->parseStack->isEmpty() ) {
	d->parseReference_charDataRead = false;
	state = Init;
    } else {
	state = d->parseStack->pop().state;
#if defined(QT_QXML_DEBUG)
	tqDebug( "TQXmlSimpleReader: parseReference (cont) in state %d", state );
#endif
	if ( !d->parseStack->isEmpty() ) {
	    ParseFunction function = d->parseStack->top().function;
	    if ( function == &TQXmlSimpleReader::eat_ws ) {
		d->parseStack->pop();
#if defined(QT_QXML_DEBUG)
		tqDebug( "TQXmlSimpleReader: eat_ws (cont)" );
#endif
	    }
	    if ( !(this->*function)() ) {
		parseFailed( &TQXmlSimpleReader::parseReference, state );
		return false;
	    }
	}
    }

    for (;;) {
	switch ( state ) {
	    case DoneD:
		return true;
	    case DoneH:
		return true;
	    case DoneN:
		return true;
	    case -1:
		// Error
		reportParseError( XMLERR_ERRORPARSINGREFERENCE );
		return false;
	}

	if ( atEnd() ) {
	    unexpectedEof( &TQXmlSimpleReader::parseReference, state );
	    return false;
	}
	if        ( c.row() ) {
	    input = InpUnknown;
	} else if ( c.cell() == '&' ) {
	    input = InpAmp;
	} else if ( c.cell() == ';' ) {
	    input = InpSemi;
	} else if ( c.cell() == '#' ) {
	    input = InpHash;
	} else if ( c.cell() == 'x' ) {
	    input = InpX;
	} else if ( '0' <= c.cell() && c.cell() <= '9' ) {
	    input = InpNum;
	} else if ( 'a' <= c.cell() && c.cell() <= 'f' ) {
	    input = InpHex;
	} else if ( 'A' <= c.cell() && c.cell() <= 'F' ) {
	    input = InpHex;
	} else {
	    input = InpUnknown;
	}
	state = table[state][input];

	switch ( state ) {
	    case SRef:
		refClear();
		next();
		break;
	    case ChRef:
		next();
		break;
	    case ChDec:
		refAddC();
		next();
		break;
	    case ChHexS:
		next();
		break;
	    case ChHex:
		refAddC();
		next();
		break;
	    case Name:
		// read the name into the ref
		d->parseName_useRef = true;
		if ( !parseName() ) {
		    parseFailed( &TQXmlSimpleReader::parseReference, state );
		    return false;
		}
		break;
	    case DoneD:
		tmp = ref().toUInt( &ok, 10 );
		if ( ok ) {
		    stringAddC( TQChar(tmp) );
		} else {
		    reportParseError( XMLERR_ERRORPARSINGREFERENCE );
		    return false;
		}
		d->parseReference_charDataRead = true;
		next();
		break;
	    case DoneH:
		tmp = ref().toUInt( &ok, 16 );
		if ( ok ) {
		    stringAddC( TQChar(tmp) );
		} else {
		    reportParseError( XMLERR_ERRORPARSINGREFERENCE );
		    return false;
		}
		d->parseReference_charDataRead = true;
		next();
		break;
	    case DoneN:
		if ( !processReference() )
		    return false;
		next();
		break;
	}
    }
}

/* This private function is only called by TQDom. It avoids a data corruption bug
   whereby undefined entities in attribute values would be appended after the
   element that contained them.

   The solution is not perfect - the undefined entity reference is replaced by
   an empty string. The propper fix will come in TQt4, when SAX will be extended
   so that attribute values can be made up of multiple children, rather than just
   a single string value.
*/
void TQXmlSimpleReader::setUndefEntityInAttrHack(bool b)
{
    d->undefEntityInAttrHack = b;
}

/*
  Helper function for parseReference()
*/
bool TQXmlSimpleReader::processReference()
{
    TQString reference = ref();
    if ( reference == "amp" ) {
	if ( d->parseReference_context == InEntityValue ) {
	    // Bypassed
	    stringAddC( '&' ); stringAddC( 'a' ); stringAddC( 'm' ); stringAddC( 'p' ); stringAddC( ';' );
	} else {
	    // Included or Included in literal
	    stringAddC( '&' );
	}
	d->parseReference_charDataRead = true;
    } else if ( reference == "lt" ) {
	if ( d->parseReference_context == InEntityValue ) {
	    // Bypassed
	    stringAddC( '&' ); stringAddC( 'l' ); stringAddC( 't' ); stringAddC( ';' );
	} else {
	    // Included or Included in literal
	    stringAddC( '<' );
	}
	d->parseReference_charDataRead = true;
    } else if ( reference == "gt" ) {
	if ( d->parseReference_context == InEntityValue ) {
	    // Bypassed
	    stringAddC( '&' ); stringAddC( 'g' ); stringAddC( 't' ); stringAddC( ';' );
	} else {
	    // Included or Included in literal
	    stringAddC( '>' );
	}
	d->parseReference_charDataRead = true;
    } else if ( reference == "apos" ) {
	if ( d->parseReference_context == InEntityValue ) {
	    // Bypassed
	    stringAddC( '&' ); stringAddC( 'a' ); stringAddC( 'p' ); stringAddC( 'o' ); stringAddC( 's' ); stringAddC( ';' );
	} else {
	    // Included or Included in literal
	    stringAddC( '\'' );
	}
	d->parseReference_charDataRead = true;
    } else if ( reference == "quot" ) {
	if ( d->parseReference_context == InEntityValue ) {
	    // Bypassed
	    stringAddC( '&' ); stringAddC( 'q' ); stringAddC( 'u' ); stringAddC( 'o' ); stringAddC( 't' ); stringAddC( ';' );
	} else {
	    // Included or Included in literal
	    stringAddC( '"' );
	}
	d->parseReference_charDataRead = true;
    } else {
	TQMap<TQString,TQString>::Iterator it;
	it = d->entities.find( reference );
	if ( it != d->entities.end() ) {
	    // "Internal General"
	    switch ( d->parseReference_context ) {
		case InContent:
		    // Included
		    if ( !insertXmlRef( it.data(), reference, false ) )
			return false;
		    d->parseReference_charDataRead = false;
		    break;
		case InAttributeValue:
		    // Included in literal
		    if ( !insertXmlRef( it.data(), reference, true ) )
			return false;
		    d->parseReference_charDataRead = false;
		    break;
		case InEntityValue:
		    {
			// Bypassed
			stringAddC( '&' );
			for ( int i=0; i<(int)reference.length(); i++ ) {
			    stringAddC( reference[i] );
			}
			stringAddC( ';');
			d->parseReference_charDataRead = true;
		    }
		    break;
		case InDTD:
		    // Forbidden
		    d->parseReference_charDataRead = false;
		    reportParseError( XMLERR_INTERNALGENERALENTITYINDTD );
		    return false;
	    }
	} else {
	    TQMap<TQString,TQXmlSimpleReaderPrivate::ExternEntity>::Iterator itExtern;
	    itExtern = d->externEntities.find( reference );
	    if ( itExtern == d->externEntities.end() ) {
		// entity not declared
		// ### check this case for conformance
		if ( d->parseReference_context == InEntityValue ) {
		    // Bypassed
		    stringAddC( '&' );
		    for ( int i=0; i<(int)reference.length(); i++ ) {
			stringAddC( reference[i] );
		    }
		    stringAddC( ';');
		    d->parseReference_charDataRead = true;
		} else {
		    if ( contentHnd && !(d->parseReference_context == InAttributeValue
                             && d->undefEntityInAttrHack)) {
			if ( !contentHnd->skippedEntity( reference ) ) {
			    reportParseError( contentHnd->errorString() );
			    return false; // error
			}
		    }
		}
	    } else if ( (*itExtern).notation.isNull() ) {
		// "External Parsed General"
		switch ( d->parseReference_context ) {
		    case InContent:
			{
			    // Included if validating
			    bool skipIt = true;
			    if ( entityRes ) {
				TQXmlInputSource *ret = 0;
				if ( !entityRes->resolveEntity( itExtern.data().publicId, itExtern.data().systemId, ret ) ) {
				    delete ret;
				    reportParseError( entityRes->errorString() );
				    return false;
				}
				if ( ret ) {
				    TQString xmlRefString = ret->data();
				    delete ret;
				    if ( !stripTextDecl( xmlRefString ) ) {
					reportParseError( XMLERR_ERRORINTEXTDECL );
					return false;
				    }
				    if ( !insertXmlRef( xmlRefString, reference, false ) )
					return false;
				    skipIt = false;
				}
			    }
			    if ( skipIt && contentHnd ) {
				if ( !contentHnd->skippedEntity( reference ) ) {
				    reportParseError( contentHnd->errorString() );
				    return false; // error
				}
			    }
			    d->parseReference_charDataRead = false;
			} break;
		    case InAttributeValue:
			// Forbidden
			d->parseReference_charDataRead = false;
			reportParseError( XMLERR_EXTERNALGENERALENTITYINAV );
			return false;
		    case InEntityValue:
			{
			    // Bypassed
			    stringAddC( '&' );
			    for ( int i=0; i<(int)reference.length(); i++ ) {
				stringAddC( reference[i] );
			    }
			    stringAddC( ';');
			    d->parseReference_charDataRead = true;
			}
			break;
		    case InDTD:
			// Forbidden
			d->parseReference_charDataRead = false;
			reportParseError( XMLERR_EXTERNALGENERALENTITYINDTD );
			return false;
		}
	    } else {
		// "Unparsed"
		// ### notify for "Occurs as Attribute Value" missing (but this is no refence, anyway)
		// Forbidden
		d->parseReference_charDataRead = false;
		reportParseError( XMLERR_UNPARSEDENTITYREFERENCE );
		return false; // error
	    }
	}
    }
    return true; // no error
}


/*
  Parses over a simple string.

  After the string was successfully parsed, the head is on the first
  character after the string.
*/
bool TQXmlSimpleReader::parseString()
{
    const signed char InpCharExpected  = 0; // the character that was expected
    const signed char InpUnknown       = 1;

    signed char state; // state in this function is the position in the string s
    signed char input;

    if ( d->parseStack==0 || d->parseStack->isEmpty() ) {
	d->Done = d->parseString_s.length();
	state = 0;
    } else {
	state = d->parseStack->pop().state;
#if defined(QT_QXML_DEBUG)
	tqDebug( "TQXmlSimpleReader: parseString (cont) in state %d", state );
#endif
	if ( !d->parseStack->isEmpty() ) {
	    ParseFunction function = d->parseStack->top().function;
	    if ( function == &TQXmlSimpleReader::eat_ws ) {
		d->parseStack->pop();
#if defined(QT_QXML_DEBUG)
		tqDebug( "TQXmlSimpleReader: eat_ws (cont)" );
#endif
	    }
	    if ( !(this->*function)() ) {
		parseFailed( &TQXmlSimpleReader::parseString, state );
		return false;
	    }
	}
    }

    for (;;) {
	if ( state == d->Done ) {
	    return true;
	}

	if ( atEnd() ) {
	    unexpectedEof( &TQXmlSimpleReader::parseString, state );
	    return false;
	}
	if ( c == d->parseString_s[(int)state] ) {
	    input = InpCharExpected;
	} else {
	    input = InpUnknown;
	}
	if ( input == InpCharExpected ) {
	    state++;
	} else {
	    // Error
	    reportParseError( XMLERR_UNEXPECTEDCHARACTER );
	    return false;
	}

	next();
    }
}

/*
  This private function inserts and reports an entity substitution. The
  substituted string is \a data and the name of the entity reference is \a
  name. If \a inLiteral is true, the entity is IncludedInLiteral (i.e., " and '
  must be quoted. Otherwise they are not quoted.

  This function returns false on error.
*/
bool TQXmlSimpleReader::insertXmlRef( const TQString &data, const TQString &name, bool inLiteral )
{
    if ( inLiteral ) {
	TQString tmp = data;
	d->xmlRef.push( tmp.replace( "\"", "&quot;" ).replace( "'", "&apos;" ) );
    } else {
	d->xmlRef.push( data );
    }
    d->xmlRefName.push( name );
    uint n = (uint)TQMAX( d->parameterEntities.count(), d->entities.count() );
    if ( d->xmlRefName.count() > n+1 ) {
	// recursive entities
	reportParseError( XMLERR_RECURSIVEENTITIES );
	return false;
    }
    if ( d->reportEntities && lexicalHnd ) {
	if ( !lexicalHnd->startEntity( name ) ) {
	    reportParseError( lexicalHnd->errorString() );
	    return false;
	}
    }
    return true;
}

/*
  This private function moves the cursor to the next character.
*/
void TQXmlSimpleReader::next()
{
    int count = (int)d->xmlRef.count();
    while ( count != 0 ) {
	if ( d->xmlRef.top().isEmpty() ) {
	    d->xmlRef.pop_back();
	    d->xmlRefName.pop_back();
	    count--;
	} else {
	    c = d->xmlRef.top().constref( 0 );
	    d->xmlRef.top().remove( (uint)0, 1 );
	    return;
	}
    }
    // the following could be written nicer, but since it is a time-critical
    // function, rather optimize for speed
    ushort uc = c.unicode();
    if (uc == '\n') {
	c = inputSource->next();
	lineNr++;
	columnNr = -1;
    } else if ( uc == '\r' ) {
	c = inputSource->next();
	if ( c.unicode() != '\n' ) {
	    lineNr++;
	    columnNr = -1;
	}
    } else {
	c = inputSource->next();
    }
    ++columnNr;
}

/*
  This private function moves the cursor to the next non-whitespace character.
  This function does not move the cursor if the actual cursor position is a
  non-whitespace charcter.

  Returns false when you use incremental parsing and this function reaches EOF
  with reading only whitespace characters. In this case it also poplulates the
  parseStack with useful information. In all other cases, this function returns
  true.
*/
bool TQXmlSimpleReader::eat_ws()
{
    while ( !atEnd() ) {
	if ( !is_S(c) ) {
	    return true;
	}
	next();
    }
    if ( d->parseStack != 0 ) {
	unexpectedEof( &TQXmlSimpleReader::eat_ws, 0 );
	return false;
    }
    return true;
}

bool TQXmlSimpleReader::next_eat_ws()
{
    next();
    return eat_ws();
}


/*
  This private function initializes the reader. \a i is the input source to
  read the data from.
*/
void TQXmlSimpleReader::init( const TQXmlInputSource *i )
{
    lineNr = 0;
    columnNr = -1;
    inputSource = (TQXmlInputSource *)i;
    initData();

    d->externParameterEntities.clear();
    d->parameterEntities.clear();
    d->externEntities.clear();
    d->entities.clear();

    d->tags.clear();

    d->doctype = "";
    d->xmlVersion = "";
    d->encoding = "";
    d->standalone = TQXmlSimpleReaderPrivate::Unknown;
    d->error = TQString::null;
}

/*
  This private function initializes the XML data related variables. Especially,
  it reads the data from the input source.
*/
void TQXmlSimpleReader::initData()
{
    c = TQXmlInputSource::EndOfData;
    d->xmlRef.clear();
    next();
}

/*
  Returns true if a entity with the name \a e exists,
  otherwise returns false.
*/
bool TQXmlSimpleReader::entityExist( const TQString& e ) const
{
    if (  d->parameterEntities.find(e) == d->parameterEntities.end() &&
	  d->externParameterEntities.find(e) == d->externParameterEntities.end() &&
	  d->externEntities.find(e) == d->externEntities.end() &&
	  d->entities.find(e) == d->entities.end() ) {
	return false;
    } else {
	return true;
    }
}

void TQXmlSimpleReader::reportParseError( const TQString& error )
{
    d->error = error;
    if ( errorHnd ) {
	if ( d->error.isNull() ) {
	    errorHnd->fatalError( TQXmlParseException( XMLERR_OK, columnNr+1, lineNr+1 ) );
	} else {
	    errorHnd->fatalError( TQXmlParseException( d->error, columnNr+1, lineNr+1 ) );
	}
    }
}

/*
  This private function is called when a parsing function encounters an
  unexpected EOF. It decides what to do (depending on incremental parsing or
  not). \a where is a pointer to the function where the error occurred and \a
  state is the parsing state in this function.
*/
void TQXmlSimpleReader::unexpectedEof( ParseFunction where, int state )
{
    if ( d->parseStack == 0 ) {
	reportParseError( XMLERR_UNEXPECTEDEOF );
    } else {
	if ( c == TQXmlInputSource::EndOfDocument ) {
	    reportParseError( XMLERR_UNEXPECTEDEOF );
	} else {
	    pushParseState( where, state );
	}
    }
}

/*
  This private function is called when a parse...() function returned false. It
  determines if there was an error or if incremental parsing simply went out of
  data and does the right thing for the case. \a where is a pointer to the
  function where the error occurred and \a state is the parsing state in this
  function.
*/
void TQXmlSimpleReader::parseFailed( ParseFunction where, int state )
{
    if ( d->parseStack!=0 && d->error.isNull() ) {
	pushParseState( where, state );
    }
}

/*
  This private function pushes the function pointer \a function and state \a
  state to the parse stack. This is used when you are doing an incremental
  parsing and reach the end of file too early.

  Only call this function when d->parseStack!=0.
*/
void TQXmlSimpleReader::pushParseState( ParseFunction function, int state )
{
    TQXmlSimpleReaderPrivate::ParseState ps;
    ps.function = function;
    ps.state = state;
    d->parseStack->push( ps );
}

inline static void updateValue(TQString &value, const TQChar *array, int &arrayPos, int &valueLen)
{
    value.setLength(valueLen + arrayPos);
    memcpy(const_cast<TQChar*>(value.unicode()) + valueLen, array, arrayPos * sizeof(TQChar));
    valueLen += arrayPos;
    arrayPos = 0;
}

// use buffers instead of TQString::operator+= when single characters are read
const TQString& TQXmlSimpleReader::string()
{
    updateValue(stringValue, stringArray, stringArrayPos, d->stringValueLen);
    return stringValue;
}

const TQString& TQXmlSimpleReader::name()
{
    updateValue(nameValue, nameArray, nameArrayPos, d->nameValueLen);
    return nameValue;
}

const TQString& TQXmlSimpleReader::ref()
{
    updateValue(refValue, refArray, refArrayPos, d->refValueLen);
    return refValue;
}

void TQXmlSimpleReader::stringAddC(const TQChar &ch)
{
    if (stringArrayPos == 256)
        updateValue(stringValue, stringArray, stringArrayPos, d->stringValueLen);
    stringArray[stringArrayPos++] = ch;
}
void TQXmlSimpleReader::nameAddC(const TQChar &ch)
{
    if (nameArrayPos == 256)
        updateValue(nameValue, nameArray, nameArrayPos, d->nameValueLen);
    nameArray[nameArrayPos++] = ch;
}

void TQXmlSimpleReader::refAddC(const TQChar &ch)
{
    if (refArrayPos == 256)
        updateValue(refValue, refArray, refArrayPos, d->refValueLen);
    refArray[refArrayPos++] = ch;
}

void TQXmlSimpleReader::stringClear()
{
    d->stringValueLen = 0; stringArrayPos = 0;
}


#endif //TQT_NO_XML
