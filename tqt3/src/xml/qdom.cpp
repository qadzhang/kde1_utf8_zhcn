/****************************************************************************
**
** Implementation of TQDomDocument and related classes.
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

#include "ntqdom.h"

#ifndef TQT_NO_DOM

#include "ntqxml.h"
#include "ntqptrlist.h"
#include "ntqdict.h"
#include "ntqtextstream.h"
#include "ntqtextcodec.h"
#include "ntqiodevice.h"
#include "ntqregexp.h"
#include "ntqbuffer.h"

/*
  ### old todo comments -- I don't know if they still apply...

  If the document dies, remove all pointers to it from children
  which can not be deleted at this time.

  If a node dies and has direct children which can not be deleted,
  then remove the pointer to the parent.

  createElement and friends create double reference counts.
*/

/*
  Reference counting:

  Some simple rules:
  1) If an intern object returns a pointer to another intern object
     then the reference count of the returned object is not increased.
  2) If an extern object is created and gets a pointer to some intern
     object, then the extern object increases the intern objects reference count.
  3) If an extern object is deleted, then it decreases the reference count
     on its associated intern object and deletes it if nobody else hold references
     on the intern object.
*/


/*
  Helper to split a qualified name in the prefix and local name.
*/
static void qt_split_namespace( TQString& prefix, TQString& name, const TQString& qName, bool hasURI )
{
    int i = qName.find( ':' );
    if ( i == -1 ) {
	if ( hasURI )
	    prefix = "";
	else
	    prefix = TQString::null;
	name = qName;
    } else {
	prefix = qName.left( i );
	name = qName.mid( i + 1 );
    }
}

/*
  Counter for the TQDomNodeListPrivate timestamps.
*/
static volatile long qt_nodeListTime = 0;

/**************************************************************
 *
 * Private class declerations
 *
 **************************************************************/

class TQDomImplementationPrivate : public TQShared
{
public:
    TQDomImplementationPrivate();
    ~TQDomImplementationPrivate();

    TQDomImplementationPrivate* clone();

};

class TQDomNodePrivate : public TQShared
{
public:
    TQDomNodePrivate( TQDomDocumentPrivate*, TQDomNodePrivate* parent = 0 );
    TQDomNodePrivate( TQDomNodePrivate* n, bool deep );
    virtual ~TQDomNodePrivate();

    TQString nodeName() const { return name; }
    TQString nodeValue() const { return value; }
    virtual void setNodeValue( const TQString& v ) { value = v; }

    TQDomDocumentPrivate* ownerDocument();
    void setOwnerDocument( TQDomDocumentPrivate* doc );

    virtual TQDomNamedNodeMapPrivate* attributes();
    virtual bool hasAttributes() { return false; }
    virtual TQDomNodePrivate* insertBefore( TQDomNodePrivate* newChild, TQDomNodePrivate* refChild );
    virtual TQDomNodePrivate* insertAfter( TQDomNodePrivate* newChild, TQDomNodePrivate* refChild );
    virtual TQDomNodePrivate* replaceChild( TQDomNodePrivate* newChild, TQDomNodePrivate* oldChild );
    virtual TQDomNodePrivate* removeChild( TQDomNodePrivate* oldChild );
    virtual TQDomNodePrivate* appendChild( TQDomNodePrivate* newChild );

    TQDomNodePrivate* namedItem( const TQString& name );

    virtual TQDomNodePrivate* cloneNode( bool deep = true );
    virtual void normalize();
    virtual void clear();

    TQDomNodePrivate* parent() { return hasParent ? ownerNode : 0; }
    void setParent( TQDomNodePrivate *p ) { ownerNode = p; hasParent = true; }
    void setNoParent() {
	ownerNode = hasParent ? (TQDomNodePrivate*)ownerDocument() : 0;
	hasParent = false;
    }

    // Dynamic cast
    virtual bool isAttr() { return false; }
    virtual bool isCDATASection() { return false; }
    virtual bool isDocumentFragment() { return false; }
    virtual bool isDocument() { return false; }
    virtual bool isDocumentType() { return false; }
    virtual bool isElement() { return false; }
    virtual bool isEntityReference() { return false; }
    virtual bool isText() { return false; }
    virtual bool isEntity() { return false; }
    virtual bool isNotation() { return false; }
    virtual bool isProcessingInstruction() { return false; }
    virtual bool isCharacterData() { return false; }
    virtual bool isComment() { return false; }
    virtual TQDomNode::NodeType nodeType() const { return TQDomNode::BaseNode; }

    virtual void save( TQTextStream&, int, int ) const;

    // Variables
    TQDomNodePrivate* prev;
    TQDomNodePrivate* next;
    TQDomNodePrivate* ownerNode; // either the node's parent or the node's owner document
    TQDomNodePrivate* first;
    TQDomNodePrivate* last;

    TQString name; // this is the local name if prefix != null
    TQString value;
    TQString prefix; // set this only for ElementNode and AttributeNode
    TQString namespaceURI; // set this only for ElementNode and AttributeNode
    bool createdWithDom1Interface;
    bool hasParent;
};

class TQDomNodeListPrivate : public TQShared
{
public:
    TQDomNodeListPrivate( TQDomNodePrivate* );
    TQDomNodeListPrivate( TQDomNodePrivate*, const TQString&  );
    TQDomNodeListPrivate( TQDomNodePrivate*, const TQString&, const TQString&  );
    virtual ~TQDomNodeListPrivate();

    virtual bool operator== ( const TQDomNodeListPrivate& ) const;
    virtual bool operator!= ( const TQDomNodeListPrivate& ) const;

    void createList();
    virtual TQDomNodePrivate* item( int index );
    virtual uint length() const;

    TQDomNodePrivate* node_impl;
    TQString tagname;
    TQString nsURI;
    TQPtrList<TQDomNodePrivate> list;
    long timestamp;
};

class TQDomNamedNodeMapPrivate : public TQShared
{
public:
    TQDomNamedNodeMapPrivate( TQDomNodePrivate* );
    ~TQDomNamedNodeMapPrivate();

    TQDomNodePrivate* namedItem( const TQString& name ) const;
    TQDomNodePrivate* namedItemNS( const TQString& nsURI, const TQString& localName ) const;
    TQDomNodePrivate* setNamedItem( TQDomNodePrivate* arg );
    TQDomNodePrivate* setNamedItemNS( TQDomNodePrivate* arg );
    TQDomNodePrivate* removeNamedItem( const TQString& name );
    TQDomNodePrivate* item( int index ) const;
    uint length() const;
    bool contains( const TQString& name ) const;
    bool containsNS( const TQString& nsURI, const TQString & localName ) const;

    /**
     * Remove all children from the map.
     */
    void clearMap();
    bool isReadOnly() { return readonly; }
    void setReadOnly( bool r ) { readonly = r; }
    bool isAppendToParent() { return appendToParent; }
    /**
     * If true, then the node will redirect insert/remove calls
     * to its parent by calling TQDomNodePrivate::appendChild or removeChild.
     * In addition the map wont increase or decrease the reference count
     * of the nodes it contains.
     *
     * By default this value is false and the map will handle reference counting
     * by itself.
     */
    void setAppendToParent( bool b ) { appendToParent = b; }

    /**
     * Creates a copy of the map. It is a deep copy
     * that means that all children are cloned.
     */
    TQDomNamedNodeMapPrivate* clone( TQDomNodePrivate* parent );

    // Variables
    TQDict<TQDomNodePrivate> map;
    TQDomNodePrivate* parent;
    bool readonly;
    bool appendToParent;
};

class TQDomDocumentTypePrivate : public TQDomNodePrivate
{
public:
    TQDomDocumentTypePrivate( TQDomDocumentPrivate*, TQDomNodePrivate* parent = 0 );
    TQDomDocumentTypePrivate( TQDomDocumentTypePrivate* n, bool deep );
    ~TQDomDocumentTypePrivate();
    void init();

    // Reimplemented from TQDomNodePrivate
    TQDomNodePrivate* cloneNode( bool deep = true );
    TQDomNodePrivate* insertBefore( TQDomNodePrivate* newChild, TQDomNodePrivate* refChild );
    TQDomNodePrivate* insertAfter( TQDomNodePrivate* newChild, TQDomNodePrivate* refChild );
    TQDomNodePrivate* replaceChild( TQDomNodePrivate* newChild, TQDomNodePrivate* oldChild );
    TQDomNodePrivate* removeChild( TQDomNodePrivate* oldChild );
    TQDomNodePrivate* appendChild( TQDomNodePrivate* newChild );

    bool isDocumentType() { return true; }
    TQDomNode::NodeType nodeType() const { return TQDomNode::DocumentTypeNode; }

    void save( TQTextStream& s, int, int ) const;

    // Variables
    TQDomNamedNodeMapPrivate* entities;
    TQDomNamedNodeMapPrivate* notations;
    TQString publicId;
    TQString systemId;
    TQString internalSubset;
};

class TQDomDocumentFragmentPrivate : public TQDomNodePrivate
{
public:
    TQDomDocumentFragmentPrivate( TQDomDocumentPrivate*, TQDomNodePrivate* parent = 0 );
    TQDomDocumentFragmentPrivate( TQDomNodePrivate* n, bool deep );
    ~TQDomDocumentFragmentPrivate();

    // Reimplemented from TQDomNodePrivate
    TQDomNodePrivate* cloneNode( bool deep = true );
    bool isDocumentFragment() { return true; }
    TQDomNode::NodeType nodeType() const { return TQDomNode::DocumentFragmentNode; }
};

class TQDomCharacterDataPrivate : public TQDomNodePrivate
{
public:
    TQDomCharacterDataPrivate( TQDomDocumentPrivate*, TQDomNodePrivate* parent, const TQString& data );
    TQDomCharacterDataPrivate( TQDomCharacterDataPrivate* n, bool deep );
    ~TQDomCharacterDataPrivate();

    uint dataLength() const;
    TQString substringData( unsigned long offset, unsigned long count ) const;
    void appendData( const TQString& arg );
    void insertData( unsigned long offset, const TQString& arg );
    void deleteData( unsigned long offset, unsigned long count );
    void replaceData( unsigned long offset, unsigned long count, const TQString& arg );

    // Reimplemented from TQDomNodePrivate
    bool isCharacterData() { return true; }
    TQDomNode::NodeType nodeType() const { return TQDomNode::CharacterDataNode; }
    TQDomNodePrivate* cloneNode( bool deep = true );

};

class TQDomTextPrivate : public TQDomCharacterDataPrivate
{
public:
    TQDomTextPrivate( TQDomDocumentPrivate*, TQDomNodePrivate* parent, const TQString& value );
    TQDomTextPrivate( TQDomTextPrivate* n, bool deep );
    ~TQDomTextPrivate();

    TQDomTextPrivate* splitText( int offset );

    // Reimplemented from TQDomNodePrivate
    TQDomNodePrivate* cloneNode( bool deep = true );
    bool isText() { return true; }
    TQDomNode::NodeType nodeType() const { return TQDomNode::TextNode; }
    void save( TQTextStream& s, int, int ) const;

};

class TQDomAttrPrivate : public TQDomNodePrivate
{
public:
    TQDomAttrPrivate( TQDomDocumentPrivate*, TQDomNodePrivate*, const TQString& name );
    TQDomAttrPrivate( TQDomDocumentPrivate*, TQDomNodePrivate*, const TQString& nsURI, const TQString& qName );
    TQDomAttrPrivate( TQDomAttrPrivate* n, bool deep );
    ~TQDomAttrPrivate();

    bool specified() const;

    // Reimplemented from TQDomNodePrivate
    void setNodeValue( const TQString& v );
    TQDomNodePrivate* cloneNode( bool deep = true );
    bool isAttr() { return true; }
    TQDomNode::NodeType nodeType() const { return TQDomNode::AttributeNode; }
    void save( TQTextStream& s, int, int ) const;

    // Variables
    bool m_specified;
};

class TQDomElementPrivate : public TQDomNodePrivate
{
public:
    TQDomElementPrivate( TQDomDocumentPrivate*, TQDomNodePrivate* parent, const TQString& name );
    TQDomElementPrivate( TQDomDocumentPrivate*, TQDomNodePrivate* parent, const TQString& nsURI, const TQString& qName );
    TQDomElementPrivate( TQDomElementPrivate* n, bool deep );
    ~TQDomElementPrivate();

    TQString attribute( const TQString& name,  const TQString& defValue ) const;
    TQString attributeNS( const TQString& nsURI, const TQString& localName, const TQString& defValue ) const;
    void setAttribute( const TQString& name, const TQString& value );
    void setAttributeNS( const TQString& nsURI, const TQString& qName, const TQString& newValue );
    void removeAttribute( const TQString& name );
    TQDomAttrPrivate* attributeNode( const TQString& name);
    TQDomAttrPrivate* attributeNodeNS( const TQString& nsURI, const TQString& localName );
    TQDomAttrPrivate* setAttributeNode( TQDomAttrPrivate* newAttr );
    TQDomAttrPrivate* setAttributeNodeNS( TQDomAttrPrivate* newAttr );
    TQDomAttrPrivate* removeAttributeNode( TQDomAttrPrivate* oldAttr );
    bool hasAttribute( const TQString& name );
    bool hasAttributeNS( const TQString& nsURI, const TQString& localName );

    TQString text();

    // Reimplemented from TQDomNodePrivate
    TQDomNamedNodeMapPrivate* attributes() { return m_attr; }
    bool hasAttributes() { return ( m_attr->length() > 0 ); }
    bool isElement() { return true; }
    TQDomNode::NodeType nodeType() const { return TQDomNode::ElementNode; }
    TQDomNodePrivate* cloneNode( bool deep = true );
    void save( TQTextStream& s, int, int ) const;

    // Variables
    TQDomNamedNodeMapPrivate* m_attr;
};


class TQDomCommentPrivate : public TQDomCharacterDataPrivate
{
public:
    TQDomCommentPrivate( TQDomDocumentPrivate*, TQDomNodePrivate* parent, const TQString& value );
    TQDomCommentPrivate( TQDomCommentPrivate* n, bool deep );
    ~TQDomCommentPrivate();

    // Reimplemented from TQDomNodePrivate
    TQDomNodePrivate* cloneNode( bool deep = true );
    bool isComment() { return true; }
    TQDomNode::NodeType nodeType() const { return TQDomNode::CommentNode; }
    void save( TQTextStream& s, int, int ) const;

};

class TQDomCDATASectionPrivate : public TQDomTextPrivate
{
public:
    TQDomCDATASectionPrivate( TQDomDocumentPrivate*, TQDomNodePrivate* parent, const TQString& value );
    TQDomCDATASectionPrivate( TQDomCDATASectionPrivate* n, bool deep );
    ~TQDomCDATASectionPrivate();

    // Reimplemented from TQDomNodePrivate
    TQDomNodePrivate* cloneNode( bool deep = true );
    bool isCDATASection() { return true; }
    TQDomNode::NodeType nodeType() const { return TQDomNode::CDATASectionNode; }
    void save( TQTextStream& s, int, int ) const;

};

class TQDomNotationPrivate : public TQDomNodePrivate
{
public:
    TQDomNotationPrivate( TQDomDocumentPrivate*, TQDomNodePrivate* parent, const TQString& name,
			  const TQString& pub, const TQString& sys );
    TQDomNotationPrivate( TQDomNotationPrivate* n, bool deep );
    ~TQDomNotationPrivate();

    // Reimplemented from TQDomNodePrivate
    TQDomNodePrivate* cloneNode( bool deep = true );
    bool isNotation() { return true; }
    TQDomNode::NodeType nodeType() const { return TQDomNode::NotationNode; }
    void save( TQTextStream& s, int, int ) const;

    // Variables
    TQString m_sys;
    TQString m_pub;
};

class TQDomEntityPrivate : public TQDomNodePrivate
{
public:
    TQDomEntityPrivate( TQDomDocumentPrivate*, TQDomNodePrivate* parent, const TQString& name,
			const TQString& pub, const TQString& sys, const TQString& notation );
    TQDomEntityPrivate( TQDomEntityPrivate* n, bool deep );
    ~TQDomEntityPrivate();

    // Reimplemented from TQDomNodePrivate
    TQDomNodePrivate* cloneNode( bool deep = true );
    bool isEntity() { return true; }
    TQDomNode::NodeType nodeType() const { return TQDomNode::EntityNode; }
    void save( TQTextStream& s, int, int ) const;

    // Variables
    TQString m_sys;
    TQString m_pub;
    TQString m_notationName;
};

class TQDomEntityReferencePrivate : public TQDomNodePrivate
{
public:
    TQDomEntityReferencePrivate( TQDomDocumentPrivate*, TQDomNodePrivate* parent, const TQString& name );
    TQDomEntityReferencePrivate( TQDomNodePrivate* n, bool deep );
    ~TQDomEntityReferencePrivate();

    // Reimplemented from TQDomNodePrivate
    TQDomNodePrivate* cloneNode( bool deep = true );
    bool isEntityReference() { return true; }
    TQDomNode::NodeType nodeType() const { return TQDomNode::EntityReferenceNode; }
    void save( TQTextStream& s, int, int ) const;
};

class TQDomProcessingInstructionPrivate : public TQDomNodePrivate
{
public:
    TQDomProcessingInstructionPrivate( TQDomDocumentPrivate*, TQDomNodePrivate* parent, const TQString& target,
				       const TQString& data);
    TQDomProcessingInstructionPrivate( TQDomProcessingInstructionPrivate* n, bool deep );
    ~TQDomProcessingInstructionPrivate();

    // Reimplemented from TQDomNodePrivate
    TQDomNodePrivate* cloneNode( bool deep = true );
    bool isProcessingInstruction() { return true; }
    TQDomNode::NodeType nodeType() const { return TQDomNode::ProcessingInstructionNode; }
    void save( TQTextStream& s, int, int ) const;
};

class TQDomDocumentPrivate : public TQDomNodePrivate
{
public:
    TQDomDocumentPrivate();
    TQDomDocumentPrivate( const TQString& name );
    TQDomDocumentPrivate( TQDomDocumentTypePrivate* dt );
    TQDomDocumentPrivate( TQDomDocumentPrivate* n, bool deep );
    ~TQDomDocumentPrivate();

    bool setContent( TQXmlInputSource *source, bool namespaceProcessing, TQString *errorMsg, int *errorLine, int *errorColumn );
    bool setContent( TQXmlInputSource *source, TQXmlReader *reader, TQString *errorMsg, int *errorLine, int *errorColumn );

    // Attributes
    TQDomDocumentTypePrivate* doctype() { return type; };
    TQDomImplementationPrivate* implementation() { return impl; };
    TQDomElementPrivate* documentElement();

    // Factories
    TQDomElementPrivate* createElement( const TQString& tagName );
    TQDomElementPrivate*	createElementNS( const TQString& nsURI, const TQString& qName );
    TQDomDocumentFragmentPrivate* createDocumentFragment();
    TQDomTextPrivate* createTextNode( const TQString& data );
    TQDomCommentPrivate* createComment( const TQString& data );
    TQDomCDATASectionPrivate* createCDATASection( const TQString& data );
    TQDomProcessingInstructionPrivate* createProcessingInstruction( const TQString& target, const TQString& data );
    TQDomAttrPrivate* createAttribute( const TQString& name );
    TQDomAttrPrivate* createAttributeNS( const TQString& nsURI, const TQString& qName );
    TQDomEntityReferencePrivate* createEntityReference( const TQString& name );

    TQDomNodePrivate* importNode( const TQDomNodePrivate* importedNode, bool deep );

    // Reimplemented from TQDomNodePrivate
    TQDomNodePrivate* cloneNode( bool deep = true );
    bool isDocument() { return true; }
    TQDomNode::NodeType nodeType() const { return TQDomNode::DocumentNode; }
    void clear();
    void save( TQTextStream&, int, int ) const;

    // Variables
    TQDomImplementationPrivate* impl;
    TQDomDocumentTypePrivate* type;
};

/**************************************************************
 *
 * TQDomHandler
 *
 **************************************************************/

class TQDomHandler : public TQXmlDefaultHandler
{
public:
    TQDomHandler( TQDomDocumentPrivate* d, bool namespaceProcessing );
    ~TQDomHandler();

    // content handler
    bool endDocument();
    bool startElement( const TQString& nsURI, const TQString& localName, const TQString& qName, const TQXmlAttributes& atts );
    bool endElement( const TQString& nsURI, const TQString& localName, const TQString& qName );
    bool characters( const TQString& ch );
    bool processingInstruction( const TQString& target, const TQString& data );
    bool skippedEntity( const TQString& name );

    // error handler
    bool fatalError( const TQXmlParseException& exception );

    // lexical handler
    bool startCDATA();
    bool endCDATA();
    bool startEntity( const TQString & );
    bool endEntity( const TQString & );
    bool startDTD( const TQString& name, const TQString& publicId, const TQString& systemId );
    bool comment( const TQString& ch );

    // decl handler
    bool externalEntityDecl( const TQString &name, const TQString &publicId, const TQString &systemId ) ;

    // DTD handler
    bool notationDecl( const TQString & name, const TQString & publicId, const TQString & systemId );
    bool unparsedEntityDecl( const TQString &name, const TQString &publicId, const TQString &systemId, const TQString &notationName ) ;

    TQString errorMsg;
    int errorLine;
    int errorColumn;

private:
    TQDomDocumentPrivate *doc;
    TQDomNodePrivate *node;
    TQString entityName;
    bool cdata;
    bool nsProcessing;
};

/**************************************************************
 *
 * TQDomImplementationPrivate
 *
 **************************************************************/

TQDomImplementationPrivate::TQDomImplementationPrivate()
{
}

TQDomImplementationPrivate::~TQDomImplementationPrivate()
{
}

TQDomImplementationPrivate* TQDomImplementationPrivate::clone()
{
    TQDomImplementationPrivate* p = new TQDomImplementationPrivate;
    // We are not interested in this node
    p->deref();
    return p;
}

/**************************************************************
 *
 * TQDomImplementation
 *
 **************************************************************/

/*!
    \class TQDomImplementation ntqdom.h
    \reentrant
    \brief The TQDomImplementation class provides information about the
    features of the DOM implementation.
\if defined(commercial)
    It is part of the <a href="commercialeditions.html">TQt Enterprise Edition</a>.
\endif

    \module XML
    \ingroup xml-tools

    This class describes the features that are supported by the DOM
    implementation. Currently the XML subset of DOM Level 1 and DOM
    Level 2 Core are supported.

    Normally you will use the function TQDomDocument::implementation()
    to get the implementation object.

    You can create a new document type with createDocumentType() and a
    new document with createDocument().

    For further information about the Document Object Model see \link
    http://www.w3.org/TR/REC-DOM-Level-1/\endlink and \link
    http://www.w3.org/TR/DOM-Level-2-Core/\endlink. For a more general
    introduction of the DOM implementation see the TQDomDocument
    documentation.

    \sa hasFeature()
*/

/*!
    Constructs a TQDomImplementation object.
*/
TQDomImplementation::TQDomImplementation()
{
    impl = 0;
}

/*!
    Constructs a copy of \a x.
*/
TQDomImplementation::TQDomImplementation( const TQDomImplementation& x )
{
    impl = x.impl;
    if ( impl )
	impl->ref();
}

TQDomImplementation::TQDomImplementation( TQDomImplementationPrivate* p )
{
    // We want to be co-owners, so increase the reference count
    impl = p;
    if (impl)
    	impl->ref();
}

/*!
    Assigns \a x to this DOM implementation.
*/
TQDomImplementation& TQDomImplementation::operator= ( const TQDomImplementation& x )
{
    if ( x.impl )
	x.impl->ref(); // avoid x=x
    if ( impl && impl->deref() )
	delete impl;
    impl = x.impl;

    return *this;
}

/*!
    Returns true if \a x and this DOM implementation object were
    created from the same TQDomDocument; otherwise returns false.
*/
bool TQDomImplementation::operator==( const TQDomImplementation& x ) const
{
    return ( impl == x.impl );
}

/*!
    Returns true if \a x and this DOM implementation object were
    created from different TQDomDocuments; otherwise returns false.
*/
bool TQDomImplementation::operator!=( const TQDomImplementation& x ) const
{
    return ( impl != x.impl );
}

/*!
    Destroys the object and frees its resources.
*/
TQDomImplementation::~TQDomImplementation()
{
    if ( impl && impl->deref() )
	delete impl;
}

/*!
    The function returns true if TQDom implements the requested \a
    version of a \a feature; otherwise returns false.

    The currently supported features and their versions:
    \table
    \header \i Feature \i Version
    \row \i XML \i 1.0
    \endtable
*/
bool TQDomImplementation::hasFeature( const TQString& feature, const TQString& version )
{
    if ( feature == "XML" ) {
	if ( version.isEmpty() || version == "1.0" ) {
	    return true;
	}
    }
    // ### add DOM level 2 features
    return false;
}

/*!
    Creates a document type node for the name \a qName.

    \a publicId specifies the public identifier of the external
    subset. If you specify TQString::null as the \a publicId, this
    means that the document type has no public identifier.

    \a systemId specifies the system identifier of the external
    subset. If you specify TQString::null as the \a systemId, this
    means that the document type has no system identifier.

    Since you cannot have a public identifier without a system
    identifier, the public identifier is set to TQString::null if there
    is no system identifier.

    DOM level 2 does not support any other document type declaration
    features.

    The only way you can use a document type that was created this
    way, is in combination with the createDocument() function to
    create a TQDomDocument with this document type.

    \sa createDocument();
*/
TQDomDocumentType TQDomImplementation::createDocumentType( const TQString& qName, const TQString& publicId, const TQString& systemId )
{
    TQDomDocumentTypePrivate *dt = new TQDomDocumentTypePrivate( 0 );
    dt->name = qName;
    if ( systemId.isNull() ) {
	dt->publicId = TQString::null;
	dt->systemId = TQString::null;
    } else {
	dt->publicId = publicId;
	dt->systemId = systemId;
    }
    return TQDomDocumentType( dt );
}

/*!
    Creates a DOM document with the document type \a doctype. This
    function also adds a root element node with the qualified name \a
    qName and the namespace URI \a nsURI.
*/
TQDomDocument TQDomImplementation::createDocument( const TQString& nsURI, const TQString& qName, const TQDomDocumentType& doctype )
{
    TQDomDocument doc( doctype );
    TQDomElement root = doc.createElementNS( nsURI, qName );
    doc.appendChild( root );
    return doc;
}

/*!
    Returns false if the object was created by
    TQDomDocument::implementation(); otherwise returns true.
*/
bool TQDomImplementation::isNull()
{
    return ( impl == 0 );
}

/**************************************************************
 *
 * TQDomNodeListPrivate
 *
 **************************************************************/

TQDomNodeListPrivate::TQDomNodeListPrivate( TQDomNodePrivate* n_impl )
{
    node_impl = n_impl;
    if ( node_impl )
	node_impl->ref();
    timestamp = -1;
}

TQDomNodeListPrivate::TQDomNodeListPrivate( TQDomNodePrivate* n_impl, const TQString& name )
{
    node_impl = n_impl;
    if ( node_impl )
	node_impl->ref();
    tagname = name;
    timestamp = -1;
}

TQDomNodeListPrivate::TQDomNodeListPrivate( TQDomNodePrivate* n_impl, const TQString& _nsURI, const TQString& localName )
{
    node_impl = n_impl;
    if ( node_impl )
	node_impl->ref();
    tagname = localName;
    nsURI = _nsURI;
    timestamp = -1;
}

TQDomNodeListPrivate::~TQDomNodeListPrivate()
{
    if ( node_impl && node_impl->deref() )
	delete node_impl;
}

bool TQDomNodeListPrivate::operator== ( const TQDomNodeListPrivate& other ) const
{
    return ( node_impl == other.node_impl ) && ( tagname == other.tagname ) ;
}

bool TQDomNodeListPrivate::operator!= ( const TQDomNodeListPrivate& other ) const
{
    return ( node_impl != other.node_impl ) || ( tagname != other.tagname ) ;
}

void TQDomNodeListPrivate::createList()
{
    if ( !node_impl )
	return;
    timestamp = qt_nodeListTime;
    TQDomNodePrivate* p = node_impl->first;

    list.clear();
    if ( tagname.isNull() ) {
	while ( p ) {
	    list.append( p );
	    p = p->next;
	}
    } else if ( nsURI.isNull() ) {
	while ( p && p != node_impl ) {
	    if ( p->isElement() && p->nodeName() == tagname ) {
		list.append( p );
	    }
	    if ( p->first )
		p = p->first;
	    else if ( p->next )
		p = p->next;
	    else {
		p = p->parent();
		while ( p && p != node_impl && !p->next )
		    p = p->parent();
		if ( p && p != node_impl )
		    p = p->next;
	    }
	}
    } else {
	while ( p && p != node_impl ) {
	    if ( p->isElement() && p->name==tagname && p->namespaceURI==nsURI ) {
		list.append( p );
	    }
	    if ( p->first )
		p = p->first;
	    else if ( p->next )
		p = p->next;
	    else {
		p = p->parent();
		while ( p && p != node_impl && !p->next )
		    p = p->parent();
		if ( p && p != node_impl )
		    p = p->next;
	    }
	}
    }
}

TQDomNodePrivate* TQDomNodeListPrivate::item( int index )
{
    if ( !node_impl )
	return 0;
    if ( timestamp < qt_nodeListTime )
	createList();
    return list.at( index );
}

uint TQDomNodeListPrivate::length() const
{
    if ( !node_impl )
	return 0;
    if ( timestamp < qt_nodeListTime ) {
	TQDomNodeListPrivate *that = (TQDomNodeListPrivate*)this;
	that->createList();
    }
    return list.count();
}

/**************************************************************
 *
 * TQDomNodeList
 *
 **************************************************************/

/*!
    \class TQDomNodeList ntqdom.h
    \reentrant
    \brief The TQDomNodeList class is a list of TQDomNode objects.
\if defined(commercial)
    It is part of the <a href="commercialeditions.html">TQt Enterprise Edition</a>.
\endif

    \module XML
    \ingroup xml-tools

    Lists can be obtained by TQDomDocument::elementsByTagName() and
    TQDomNode::childNodes(). The Document Object Model (DOM) requires
    these lists to be "live": whenever you change the underlying
    document, the contents of the list will get updated.

    You can get a particular node from the list with item(). The
    number of items in the list is returned by count() (and by
    length()).

    For further information about the Document Object Model see \link
    http://www.w3.org/TR/REC-DOM-Level-1/\endlink and \link
    http://www.w3.org/TR/DOM-Level-2-Core/\endlink. For a more general
    introduction of the DOM implementation see the TQDomDocument
    documentation.

    \sa TQDomNode::childNodes() TQDomDocument::elementsByTagName()
*/

/*!
    Creates an empty node list.
*/
TQDomNodeList::TQDomNodeList()
{
    impl = 0;
}

TQDomNodeList::TQDomNodeList( TQDomNodeListPrivate* p )
{
    impl = p;
}

/*!
    Constructs a copy of \a n.
*/
TQDomNodeList::TQDomNodeList( const TQDomNodeList& n )
{
    impl = n.impl;
    if ( impl )
	impl->ref();
}

/*!
    Assigns \a n to this node list.
*/
TQDomNodeList& TQDomNodeList::operator= ( const TQDomNodeList& n )
{
    if ( n.impl )
	n.impl->ref();
    if ( impl && impl->deref() )
	delete impl;
    impl = n.impl;

    return *this;
}

/*!
    Returns true if the node list \a n and this node list are equal;
    otherwise returns false.
*/
bool TQDomNodeList::operator== ( const TQDomNodeList& n ) const
{
    if ( impl == n.impl )
	return true;
    if ( !impl || !n.impl )
	return false;
    return (*impl == *n.impl);
}

/*!
    Returns true the node list \a n and this node list are not equal;
    otherwise returns false.
*/
bool TQDomNodeList::operator!= ( const TQDomNodeList& n ) const
{
    return !operator==(n);
}

/*!
    Destroys the object and frees its resources.
*/
TQDomNodeList::~TQDomNodeList()
{
    if ( impl && impl->deref() )
	delete impl;
}

/*!
    Returns the node at position \a index.

    If \a index is negative or if \a index >= length() then a null
    node is returned (i.e. a node for which TQDomNode::isNull() returns
    true).

    \sa count()
*/
TQDomNode TQDomNodeList::item( int index ) const
{
    if ( !impl )
	return TQDomNode();

    return TQDomNode( impl->item( index ) );
}

/*!
    Returns the number of nodes in the list.

    This function is the same as count().
*/
uint TQDomNodeList::length() const
{
    if ( !impl )
	return 0;
    return impl->length();
}

/*!
    \fn uint TQDomNodeList::count() const

    Returns the number of nodes in the list.

    This function is the same as length().
*/


/**************************************************************
 *
 * TQDomNodePrivate
 *
 **************************************************************/

inline void TQDomNodePrivate::setOwnerDocument( TQDomDocumentPrivate* doc )
{
    ownerNode = doc;
    hasParent = false;
}

TQDomNodePrivate::TQDomNodePrivate( TQDomDocumentPrivate* doc, TQDomNodePrivate *par )
{
    if ( par )
	setParent( par );
    else
	setOwnerDocument( doc );
    prev = 0;
    next = 0;
    first = 0;
    last = 0;
    createdWithDom1Interface = true;
}

TQDomNodePrivate::TQDomNodePrivate( TQDomNodePrivate* n, bool deep )
{
    setOwnerDocument( n->ownerDocument() );
    prev = 0;
    next = 0;
    first = 0;
    last = 0;

    name = n->name;
    value = n->value;
    prefix = n->prefix;
    namespaceURI = n->namespaceURI;
    createdWithDom1Interface = n->createdWithDom1Interface;

    if ( !deep )
	return;

    for ( TQDomNodePrivate* x = n->first; x; x = x->next )
	appendChild( x->cloneNode( true ) );
}

TQDomNodePrivate::~TQDomNodePrivate()
{
    TQDomNodePrivate* p = first;
    TQDomNodePrivate* n;

    while ( p ) {
	n = p->next;
	if ( p->deref() )
	    delete p;
	else
	    p->setNoParent();
	p = n;
    }

    first = 0;
    last = 0;
}

void TQDomNodePrivate::clear()
{
    TQDomNodePrivate* p = first;
    TQDomNodePrivate* n;

    while ( p ) {
	n = p->next;
	if ( p->deref() )
	    delete p;
	p = n;
    }

    first = 0;
    last = 0;
}

TQDomNodePrivate* TQDomNodePrivate::namedItem( const TQString& n )
{
    TQDomNodePrivate* p = first;
    while ( p ) {
	if ( p->nodeName() == n )
	    return p;
	p = p->next;
    }

    return 0;
}

TQDomNamedNodeMapPrivate* TQDomNodePrivate::attributes()
{
    return 0;
}

TQDomNodePrivate* TQDomNodePrivate::insertBefore( TQDomNodePrivate* newChild, TQDomNodePrivate* refChild )
{
    // Error check
    if ( !newChild )
	return 0;

    // Error check
    if ( newChild == refChild )
	return 0;

    // Error check
    if ( refChild && refChild->parent() != this )
	return 0;

    // "mark lists as dirty"
    qt_nodeListTime++;

    // Special handling for inserting a fragment. We just insert
    // all elements of the fragment instead of the fragment itself.
    if ( newChild->isDocumentFragment() ) {
	// Fragment is empty ?
	if ( newChild->first == 0 )
	    return newChild;

	// New parent
	TQDomNodePrivate* n = newChild->first;
	while ( n )  {
	    n->setParent( this );
	    n = n->next;
	}

	// Insert at the beginning ?
	if ( !refChild || refChild->prev == 0 ) {
	    if ( first )
		first->prev = newChild->last;
	    newChild->last->next = first;
	    if ( !last )
		last = newChild->last;
	    first = newChild->first;
	} else {
	    // Insert in the middle
	    newChild->last->next = refChild;
	    newChild->first->prev = refChild->prev;
	    refChild->prev->next = newChild->first;
	    refChild->prev = newChild->last;
	}

	// No need to increase the reference since TQDomDocumentFragment
	// does not decrease the reference.

	// Remove the nodes from the fragment
	newChild->first = 0;
	newChild->last = 0;
	return newChild;
    }

    // No more errors can occur now, so we take
    // ownership of the node.
    newChild->ref();

    if ( newChild->parent() )
	newChild->parent()->removeChild( newChild );

    newChild->setParent( this );

    if ( !refChild ) {
	if ( first )
	    first->prev = newChild;
	newChild->next = first;
	if ( !last )
	    last = newChild;
	first = newChild;
	return newChild;
    }

    if ( refChild->prev == 0 ) {
	if ( first )
	    first->prev = newChild;
	newChild->next = first;
	if ( !last )
	    last = newChild;
	first = newChild;
	return newChild;
    }

    newChild->next = refChild;
    newChild->prev = refChild->prev;
    refChild->prev->next = newChild;
    refChild->prev = newChild;

    return newChild;
}

TQDomNodePrivate* TQDomNodePrivate::insertAfter( TQDomNodePrivate* newChild, TQDomNodePrivate* refChild )
{
    // Error check
    if ( !newChild )
	return 0;

    // Error check
    if ( newChild == refChild )
	return 0;

    // Error check
    if ( refChild && refChild->parent() != this )
	return 0;

    // "mark lists as dirty"
    qt_nodeListTime++;

    // Special handling for inserting a fragment. We just insert
    // all elements of the fragment instead of the fragment itself.
    if ( newChild->isDocumentFragment() ) {
	// Fragment is empty ?
	if ( newChild->first == 0 )
	    return newChild;

	// New parent
	TQDomNodePrivate* n = newChild->first;
	while ( n ) {
	    n->setParent( this );
	    n = n->next;
	}

	// Insert at the end
	if ( !refChild || refChild->next == 0 ) {
	    if ( last )
		last->next = newChild->first;
	    newChild->first->prev = last;
	    if ( !first )
		first = newChild->first;
	    last = newChild->last;
	} else { // Insert in the middle
	    newChild->first->prev = refChild;
	    newChild->last->next = refChild->next;
	    refChild->next->prev = newChild->last;
	    refChild->next = newChild->first;
	}

	// No need to increase the reference since TQDomDocumentFragment
	// does not decrease the reference.

	// Remove the nodes from the fragment
	newChild->first = 0;
	newChild->last = 0;
	return newChild;
    }

    // Release new node from its current parent
    if ( newChild->parent() )
	newChild->parent()->removeChild( newChild );

    // No more errors can occur now, so we take
    // ownership of the node
    newChild->ref();

    newChild->setParent( this );

    // Insert at the end
    if ( !refChild ) {
	if ( last )
	    last->next = newChild;
	newChild->prev = last;
	if ( !first )
	    first = newChild;
	last = newChild;
	return newChild;
    }

    if ( refChild->next == 0 ) {
	if ( last )
	    last->next = newChild;
	newChild->prev = last;
	if ( !first )
	    first = newChild;
	last = newChild;
	return newChild;
    }

    newChild->prev = refChild;
    newChild->next = refChild->next;
    refChild->next->prev = newChild;
    refChild->next = newChild;

    return newChild;
}

TQDomNodePrivate* TQDomNodePrivate::replaceChild( TQDomNodePrivate* newChild, TQDomNodePrivate* oldChild )
{
    if ( oldChild->parent() != this )
	return 0;
    if ( !newChild || !oldChild )
	return 0;
    if ( newChild == oldChild )
	return 0;

    // mark lists as dirty
    qt_nodeListTime++;

    // Special handling for inserting a fragment. We just insert
    // all elements of the fragment instead of the fragment itself.
    if ( newChild->isDocumentFragment() ) {
	// Fragment is empty ?
	if ( newChild->first == 0 )
	    return newChild;

	// New parent
	TQDomNodePrivate* n = newChild->first;
	while ( n ) {
	    n->setParent( this );
	    n = n->next;
	}


	if ( oldChild->next )
	    oldChild->next->prev = newChild->last;
	if ( oldChild->prev )
	    oldChild->prev->next = newChild->first;

	newChild->last->next = oldChild->next;
	newChild->first->prev = oldChild->prev;

	if ( first == oldChild )
	    first = newChild->first;
	if ( last == oldChild )
	    last = newChild->last;

	oldChild->setNoParent();
	oldChild->next = 0;
	oldChild->prev = 0;

	// No need to increase the reference since TQDomDocumentFragment
	// does not decrease the reference.

	// Remove the nodes from the fragment
	newChild->first = 0;
	newChild->last = 0;

	// We are no longer interested in the old node
	if ( oldChild ) oldChild->deref();

	return oldChild;
    }

    // No more errors can occur now, so we take
    // ownership of the node
    newChild->ref();

    // Release new node from its current parent
    if ( newChild->parent() )
	newChild->parent()->removeChild( newChild );

    newChild->setParent( this );

    if ( oldChild->next )
	oldChild->next->prev = newChild;
    if ( oldChild->prev )
	oldChild->prev->next = newChild;

    newChild->next = oldChild->next;
    newChild->prev = oldChild->prev;

    if ( first == oldChild )
	first = newChild;
    if ( last == oldChild )
	last = newChild;

    oldChild->setNoParent();
    oldChild->next = 0;
    oldChild->prev = 0;

    // We are no longer interested in the old node
    if ( oldChild ) oldChild->deref();

    return oldChild;
}

TQDomNodePrivate* TQDomNodePrivate::removeChild( TQDomNodePrivate* oldChild )
{
    // Error check
    if ( oldChild->parent() != this )
	return 0;

    // "mark lists as dirty"
    qt_nodeListTime++;

    // Perhaps oldChild was just created with "createElement" or that. In this case
    // its parent is TQDomDocument but it is not part of the documents child list.
    if ( oldChild->next == 0 && oldChild->prev == 0 && first != oldChild )
	return 0;

    if ( oldChild->next )
	oldChild->next->prev = oldChild->prev;
    if ( oldChild->prev )
	oldChild->prev->next = oldChild->next;

    if ( last == oldChild )
	last = oldChild->prev;
    if ( first == oldChild )
	first = oldChild->next;

    oldChild->setNoParent();
    oldChild->next = 0;
    oldChild->prev = 0;

    // We are no longer interested in the old node
    if ( oldChild ) oldChild->deref();

    return oldChild;
}

TQDomNodePrivate* TQDomNodePrivate::appendChild( TQDomNodePrivate* newChild )
{
    // No reference manipulation needed. Done in insertAfter.
    return insertAfter( newChild, 0 );
}

TQDomDocumentPrivate* TQDomNodePrivate::ownerDocument()
{
    TQDomNodePrivate* p = this;
    while ( p && !p->isDocument() ) {
	if ( !p->hasParent )
	    return (TQDomDocumentPrivate*)p->ownerNode;
	p = p->parent();
    }

    return (TQDomDocumentPrivate*)p;
}

TQDomNodePrivate* TQDomNodePrivate::cloneNode( bool deep )
{
    TQDomNodePrivate* p = new TQDomNodePrivate( this, deep );
    // We are not interested in this node
    p->deref();
    return p;
}

static void qNormalizeNode( TQDomNodePrivate* n )
{
    TQDomNodePrivate* p = n->first;
    TQDomTextPrivate* t = 0;

    while ( p ) {
	if ( p->isText() ) {
	    if ( t ) {
		TQDomNodePrivate* tmp = p->next;
		t->appendData( p->nodeValue() );
		n->removeChild( p );
		p = tmp;
	    } else {
		t = (TQDomTextPrivate*)p;
		p = p->next;
	    }
	} else {
	    p = p->next;
	    t = 0;
	}
    }
}
void TQDomNodePrivate::normalize()
{
    // ### This one has moved from TQDomElementPrivate to this position. It is
    // not tested.
    qNormalizeNode( this );
}

void TQDomNodePrivate::save( TQTextStream& s, int depth, int indent ) const
{
    const TQDomNodePrivate* n = first;
    while ( n ) {
	n->save( s, depth, indent );
	n = n->next;
    }
}

/**************************************************************
 *
 * TQDomNode
 *
 **************************************************************/

#define IMPL ((TQDomNodePrivate*)impl)

/*!
    \class TQDomNode ntqdom.h
    \reentrant
    \brief The TQDomNode class is the base class for all the nodes in a DOM tree.
\if defined(commercial)
    It is part of the <a href="commercialeditions.html">TQt Enterprise Edition</a>.
\endif

    \module XML
    \ingroup xml-tools

    Many functions in the DOM return a TQDomNode.

    You can find out the type of a node using isAttr(),
    isCDATASection(), isDocumentFragment(), isDocument(),
    isDocumentType(), isElement(), isEntityReference(), isText(),
    isEntity(), isNotation(), isProcessingInstruction(),
    isCharacterData() and isComment().

    A TQDomNode can be converted into one of its subclasses using
    toAttr(), toCDATASection(), toDocumentFragment(), toDocument(),
    toDocumentType(), toElement(), toEntityReference(), toText(),
    toEntity(), toNotation(), toProcessingInstruction(),
    toCharacterData() or toComment(). You can convert a node to a null
    node with clear().

    Copies of the TQDomNode class share their data using explicit
    sharing. This means that modifying one node will change all
    copies. This is especially useful in combination with functions
    which return a TQDomNode, e.g. firstChild(). You can make an
    independent (deep) copy of the node with cloneNode().

    Nodes are inserted with insertBefore(), insertAfter() or
    appendChild(). You can replace one node with another using
    replaceChild() and remove a node with removeChild().

    To traverse nodes use firstChild() to get a node's first child (if
    any), and nextSibling() to traverse. TQDomNode also provides
    lastChild(), previousSibling() and parentNode(). To find the first
    child node with a particular node name use namedItem().

    To find out if a node has children use hasChildNodes() and to get
    a list of all of a node's children use childNodes().

    The node's name and value (the meaning of which varies depending
    on its type) is returned by nodeName() and nodeValue()
    respectively. The node's type is returned by nodeType(). The
    node's value can be set with setNodeValue().

    The document to which the node belongs is returned by
    ownerDocument().

    Adjacent TQDomText nodes can be merged into a single node with
    normalize().

    \l TQDomElement nodes have attributes which can be retrieved with
    attributes().

    TQDomElement and TQDomAttr nodes can have namespaces which can be
    retrieved with namespaceURI(). Their local name is retrieved with
    localName(), and their prefix with prefix(). The prefix can be set
    with setPrefix().

    You can write the XML representation of the node to a text stream
    with save().

    The following example looks for the first element in an XML document and
    prints the names of all the elements that are its direct children.
    \code
    TQDomDocument d;
    d.setContent( someXML );
    TQDomNode n = d.firstChild();
    while ( !n.isNull() ) {
	if ( n.isElement() ) {
	    TQDomElement e = n.toElement();
	    cout << "Element name: " << e.tagName() << endl;
	    break;
	}
	n = n.nextSibling();
    }
    \endcode

    For further information about the Document Object Model see \link
    http://www.w3.org/TR/REC-DOM-Level-1/\endlink and \link
    http://www.w3.org/TR/DOM-Level-2-Core/\endlink. For a more general
    introduction of the DOM implementation see the TQDomDocument
    documentation.
*/

/*!
    Constructs a \link isNull() null\endlink node.
*/
TQDomNode::TQDomNode()
{
    impl = 0;
}

/*!
    Constructs a copy of \a n.

    The data of the copy is shared (shallow copy): modifying one node
    will also change the other. If you want to make a deep copy, use
    cloneNode().
*/
TQDomNode::TQDomNode( const TQDomNode& n )
{
    impl = n.impl;
    if ( impl ) impl->ref();
}

/*!  \internal
  Constructs a new node for the data \a n.
*/
TQDomNode::TQDomNode( TQDomNodePrivate* n )
{
    impl = n;
    if ( impl ) impl->ref();
}

/*!
    Assigns a copy of \a n to this DOM node.

    The data of the copy is shared (shallow copy): modifying one node
    will also change the other. If you want to make a deep copy, use
    cloneNode().
*/
TQDomNode& TQDomNode::operator= ( const TQDomNode& n )
{
    if ( n.impl ) n.impl->ref();
    if ( impl && impl->deref() ) delete impl;
    impl = n.impl;

    return *this;
}

/*!
    Returns true if \a n and this DOM node are equal; otherwise
    returns false.
*/
bool TQDomNode::operator== ( const TQDomNode& n ) const
{
    return ( impl == n.impl );
}

/*!
    Returns true if \a n and this DOM node are not equal; otherwise
    returns false.
*/
bool TQDomNode::operator!= ( const TQDomNode& n ) const
{
    return ( impl != n.impl );
}

/*!
    Destroys the object and frees its resources.
*/
TQDomNode::~TQDomNode()
{
    if ( impl && impl->deref() ) delete impl;
}

/*!
    Returns the name of the node.

    The meaning of the name depends on the subclass:
    \table
    \header \i Name \i Meaning
    \row \i TQDomAttr \i The name of the attribute
    \row \i TQDomCDATASection \i The string "#cdata-section"
    \row \i TQDomComment \i The string "#comment"
    \row \i TQDomDocument \i The string "#document"
    \row \i TQDomDocumentFragment \i The string "#document-fragment"
    \row \i TQDomDocumentType \i The name of the document type
    \row \i TQDomElement \i The tag name
    \row \i TQDomEntity \i The name of the entity
    \row \i TQDomEntityReference \i The name of the referenced entity
    \row \i TQDomNotation \i The name of the notation
    \row \i TQDomProcessingInstruction \i The target of the processing instruction
    \row \i TQDomText \i The string "#text"
    \endtable

    \sa nodeValue()
*/
TQString TQDomNode::nodeName() const
{
    if ( !impl )
	return TQString::null;

    if ( !IMPL->prefix.isEmpty() )
	return IMPL->prefix + ":" + IMPL->name;
    return IMPL->name;
}

/*!
    Returns the value of the node.

    The meaning of the value depends on the subclass:
    \table
    \header \i Name \i Meaning
    \row \i TQDomAttr \i The attribute value
    \row \i TQDomCDATASection \i The content of the CDATA section
    \row \i TQDomComment \i The comment
    \row \i TQDomProcessingInstruction \i The data of the processing intruction
    \row \i TQDomText \i The text
    \endtable

    All the other subclasses do not have a node value and will return
    TQString::null.

    \sa setNodeValue() nodeName()
*/
TQString TQDomNode::nodeValue() const
{
    if ( !impl )
	return TQString::null;
    return IMPL->value;
}

/*!
    Sets the node's value to \a v.

    \sa nodeValue()
*/
void TQDomNode::setNodeValue( const TQString& v )
{
    if ( !impl )
	return;
    IMPL->setNodeValue( v );
}

/*!
    \enum TQDomNode::NodeType

    This enum defines the type of the node:
    \value ElementNode
    \value AttributeNode
    \value TextNode
    \value CDATASectionNode
    \value EntityReferenceNode
    \value EntityNode
    \value ProcessingInstructionNode
    \value CommentNode
    \value DocumentNode
    \value DocumentTypeNode
    \value DocumentFragmentNode
    \value NotationNode
    \value BaseNode  A TQDomNode object, i.e. not a TQDomNode subclass.
    \value CharacterDataNode
*/

/*!
    Returns the type of the node.

    \sa toAttr(), toCDATASection(), toDocumentFragment(),
    toDocument() toDocumentType(), toElement(), toEntityReference(),
    toText(), toEntity() toNotation(), toProcessingInstruction(),
    toCharacterData(), toComment()
*/
TQDomNode::NodeType TQDomNode::nodeType() const
{
    if ( !impl )
	return TQDomNode::BaseNode;
    return IMPL->nodeType();
}

/*!
    Returns the parent node. If this node has no parent, a null node
    is returned (i.e. a node for which isNull() returns true).
*/
TQDomNode TQDomNode::parentNode() const
{
    if ( !impl )
	return TQDomNode();
    return TQDomNode( IMPL->parent() );
}

/*!
    Returns a list of all direct child nodes.

    Most often you will call this function on a TQDomElement object.

    For example, if the XML document looks like this:
    \code
    <body>
    <h1>Heading</h1>
    <p>Hello <b>you</b></p>
    </body>
    \endcode
    Then the list of child nodes for the "body"-element will contain
    the node created by the &lt;h1&gt; tag and the node created by the
    &lt;p&gt; tag.

    The nodes in the list are not copied; so changing the nodes in the
    list will also change the children of this node.

    \sa firstChild() lastChild()
*/
TQDomNodeList TQDomNode::childNodes() const
{
    if ( !impl )
	return TQDomNodeList();
    return TQDomNodeList( new TQDomNodeListPrivate( impl ) );
}

/*!
    Returns the first child of the node. If there is no child node, a
    \link isNull() null node\endlink is returned. Changing the
    returned node will also change the node in the document tree.

    \sa lastChild() childNodes()
*/
TQDomNode TQDomNode::firstChild() const
{
    if ( !impl )
	return TQDomNode();
    return TQDomNode( IMPL->first );
}

/*!
    Returns the last child of the node. If there is no child node, a
    \link isNull() null node\endlink is returned. Changing the
    returned node will also change the node in the document tree.

    \sa firstChild() childNodes()
*/
TQDomNode TQDomNode::lastChild() const
{
    if ( !impl )
	return TQDomNode();
    return TQDomNode( IMPL->last );
}

/*!
    Returns the previous sibling in the document tree. Changing the
    returned node will also change the node in the document tree.

    For example, if you have XML like this:
    \code
    <h1>Heading</h1>
    <p>The text...</p>
    <h2>Next heading</h2>
    \endcode
    and this TQDomNode represents the &lt;p&gt; tag, previousSibling()
    will return the node representing the &lt;h1&gt; tag.

    \sa nextSibling()
*/
TQDomNode TQDomNode::previousSibling() const
{
    if ( !impl )
	return TQDomNode();
    return TQDomNode( IMPL->prev );
}

/*!
    Returns the next sibling in the document tree. Changing the
    returned node will also change the node in the document tree.

    If you have XML like this:
    \code
    <h1>Heading</h1>
    <p>The text...</p>
    <h2>Next heading</h2>
    \endcode
    and this TQDomNode represents the &lt;p&gt; tag, nextSibling() will
    return the node representing the &lt;h2&gt; tag.

    \sa previousSibling()
*/
TQDomNode TQDomNode::nextSibling() const
{
    if ( !impl )
	return TQDomNode();
    return TQDomNode( IMPL->next );
}

/*!
    Returns a named node map of all attributes. Attributes are only
    provided for \l{TQDomElement}s.

    Changing the attributes in the map will also change the attributes
    of this TQDomNode.
*/
TQDomNamedNodeMap TQDomNode::attributes() const
{
    if ( !impl )
	return TQDomNamedNodeMap();

    return TQDomNamedNodeMap( impl->attributes() );
}

/*!
    Returns the document to which this node belongs.
*/
TQDomDocument TQDomNode::ownerDocument() const
{
    if ( !impl )
	return TQDomDocument();
    return TQDomDocument( IMPL->ownerDocument() );
}

/*!
    Creates a deep (not shallow) copy of the TQDomNode.

    If \a deep is true, then the cloning is done recursively which
    means that all the node's children are deep copied too. If \a deep
    is false only the node itself is copied and the copy will have no
    child nodes.
*/
TQDomNode TQDomNode::cloneNode( bool deep ) const
{
    if ( !impl )
	return TQDomNode();
    return TQDomNode( IMPL->cloneNode( deep ) );
}

/*!
    Calling normalize() on an element converts all its children into a
    standard form. This means that adjacent TQDomText objects will be
    merged into a single text object (TQDomCDATASection nodes are not
    merged).
*/
void TQDomNode::normalize()
{
    if ( !impl )
	return;
    IMPL->normalize();
}

/*!
    Returns true if the DOM implementation implements the feature \a
    feature and this feature is supported by this node in the version
    \a version; otherwise returns false.

    \sa TQDomImplementation::hasFeature()
*/
bool TQDomNode::isSupported( const TQString& feature, const TQString& version ) const
{
    TQDomImplementation i;
    return i.hasFeature( feature, version );
}

/*!
    Returns the namespace URI of this node or TQString::null if the
    node has no namespace URI.

    Only nodes of type \link TQDomNode::NodeType ElementNode\endlink or
    \link TQDomNode::NodeType AttributeNode\endlink can have
    namespaces. A namespace URI must be specified at creation time and
    cannot be changed later.

    \sa prefix() localName() TQDomDocument::createElementNS()
    TQDomDocument::createAttributeNS()
*/
TQString TQDomNode::namespaceURI() const
{
    if ( !impl )
	return TQString::null;
    return IMPL->namespaceURI;
}

/*!
    Returns the namespace prefix of the node or TQString::null if the
    node has no namespace prefix.

    Only nodes of type \link TQDomNode::NodeType ElementNode\endlink or
    \link TQDomNode::NodeType AttributeNode\endlink can have
    namespaces. A namespace prefix must be specified at creation time.
    If a node was created with a namespace prefix, you can change it
    later with setPrefix().

    If you create an element or attribute with
    TQDomDocument::createElement() or TQDomDocument::createAttribute(),
    the prefix will be TQString::null. If you use
    TQDomDocument::createElementNS() or
    TQDomDocument::createAttributeNS() instead, the prefix will not be
    TQString::null; but it might be an empty string if the name does
    not have a prefix.

    \sa setPrefix() localName() namespaceURI()
    TQDomDocument::createElementNS() TQDomDocument::createAttributeNS()
*/
TQString TQDomNode::prefix() const
{
    if ( !impl )
	return TQString::null;
    return IMPL->prefix;
}

/*!
    If the node has a namespace prefix, this function changes the
    namespace prefix of the node to \a pre. Otherwise this function
    does nothing.

    Only nodes of type \link TQDomNode::NodeType ElementNode\endlink or
    \link TQDomNode::NodeType AttributeNode\endlink can have
    namespaces. A namespace prefix must have be specified at creation
    time; it is not possible to add a namespace prefix afterwards.

    \sa prefix() localName() namespaceURI()
    TQDomDocument::createElementNS() TQDomDocument::createAttributeNS()
*/
void TQDomNode::setPrefix( const TQString& pre )
{
    if ( !impl || IMPL->prefix.isNull() )
	return;
    if ( isAttr() || isElement() )
	IMPL->prefix = pre;
}

/*!
    If the node uses namespaces, this function returns the local name
    of the node; otherwise it returns TQString::null.

    Only nodes of type \link TQDomNode::NodeType ElementNode\endlink or
    \link TQDomNode::NodeType AttributeNode\endlink can have
    namespaces. A namespace must have been specified at creation time;
    it is not possible to add a namespace afterwards.

    \sa prefix() namespaceURI() TQDomDocument::createElementNS()
    TQDomDocument::createAttributeNS()
*/
TQString TQDomNode::localName() const
{
    if ( !impl || IMPL->createdWithDom1Interface )
	return TQString::null;
    return IMPL->name;
}

/*!
    Returns true if the node has attributes; otherwise returns false.

    \sa attributes()
*/
bool TQDomNode::hasAttributes() const
{
    if ( !impl )
	return false;
    return IMPL->hasAttributes();
}

/*!
    Inserts the node \a newChild before the child node \a refChild.
    \a refChild must be a direct child of this node. If \a refChild is
    \link isNull() null\endlink then \a newChild is inserted as the
    node's first child.

    If \a newChild is the child of another node, it is reparented to
    this node. If \a newChild is a child of this node, then its
    position in the list of children is changed.

    If \a newChild is a TQDomDocumentFragment, then the children of the
    fragment are removed from the fragment and inserted before \a
    refChild.

    Returns a new reference to \a newChild on success or a \link
    isNull() null node\endlink on failure.

    \sa insertAfter() replaceChild() removeChild() appendChild()
*/
TQDomNode TQDomNode::insertBefore( const TQDomNode& newChild, const TQDomNode& refChild )
{
    if ( !impl ) {
        if (nodeType() == DocumentNode)
            impl = new TQDomDocumentPrivate;
        else
            return TQDomNode();
    }
    return TQDomNode( IMPL->insertBefore( newChild.impl, refChild.impl ) );
}

/*!
    Inserts the node \a newChild after the child node \a refChild. \a
    refChild must be a direct child of this node. If \a refChild is
    \link isNull() null\endlink then \a newChild is appended as this
    node's last child.

    If \a newChild is the child of another node, it is reparented to
    this node. If \a newChild is a child of this node, then its
    position in the list of children is changed.

    If \a newChild is a TQDomDocumentFragment, then the children of the
    fragment are removed from the fragment and inserted after \a
    refChild.

    Returns a new reference to \a newChild on success or a \link
    isNull() null node\endlink on failure.

    \sa insertBefore() replaceChild() removeChild() appendChild()
*/
TQDomNode TQDomNode::insertAfter( const TQDomNode& newChild, const TQDomNode& refChild )
{
    if ( !impl ) {
        if (nodeType() == DocumentNode)
            impl = new TQDomDocumentPrivate;
        else
            return TQDomNode();
    }
    return TQDomNode( IMPL->insertAfter( newChild.impl, refChild.impl ) );
}

/*!
    Replaces \a oldChild with \a newChild. \a oldChild must be a
    direct child of this node.

    If \a newChild is the child of another node, it is reparented to
    this node. If \a newChild is a child of this node, then its
    position in the list of children is changed.

    If \a newChild is a TQDomDocumentFragment, then \a oldChild is
    replaced by all of the children of the fragment.

    Returns a new reference to \a oldChild on success or a \link
    isNull() null node\endlink an failure.

    \sa insertBefore() insertAfter() removeChild() appendChild()
*/
TQDomNode TQDomNode::replaceChild( const TQDomNode& newChild, const TQDomNode& oldChild )
{
    if ( !impl ) {
        if (nodeType() == DocumentNode)
            impl = new TQDomDocumentPrivate;
        else
            return TQDomNode();
    }
    return TQDomNode( IMPL->replaceChild( newChild.impl, oldChild.impl ) );
}

/*!
    Removes \a oldChild from the list of children. \a oldChild must be
    a direct child of this node.

    Returns a new reference to \a oldChild on success or a \link
    isNull() null node\endlink on failure.

    \sa insertBefore() insertAfter() replaceChild() appendChild()
*/
TQDomNode TQDomNode::removeChild( const TQDomNode& oldChild )
{
    if ( !impl )
	return TQDomNode();

    if ( oldChild.isNull() )
	return TQDomNode();

    return TQDomNode( IMPL->removeChild( oldChild.impl ) );
}

/*!
    Appends \a newChild as the node's last child.

    If \a newChild is the child of another node, it is reparented to
    this node. If \a newChild is a child of this node, then its
    position in the list of children is changed.

    If \a newChild is a TQDomDocumentFragment, then the children of the
    fragment are removed from the fragment and appended.

    Returns a new reference to \a newChild.

    \sa insertBefore() insertAfter() replaceChild() removeChild()
*/
TQDomNode TQDomNode::appendChild( const TQDomNode& newChild )
{
    if ( !impl ) {
        if (nodeType() == DocumentNode)
            impl = new TQDomDocumentPrivate;
        else
            return TQDomNode();
    }
    return TQDomNode( IMPL->appendChild( newChild.impl ) );
}

/*!
    Returns true if the node has one or more children; otherwise
    returns false.
*/
bool TQDomNode::hasChildNodes() const
{
    if ( !impl )
	return false;
    return IMPL->first != 0;
}

/*!
    Returns true if this node is null (i.e. if it has no type or
    contents); otherwise returns false.
*/
bool TQDomNode::isNull() const
{
    return ( impl == 0 );
}

/*!
    Converts the node into a null node; if it was not a null node
    before, its type and contents are deleted.

    \sa isNull()
*/
void TQDomNode::clear()
{
    if ( impl && impl->deref() ) delete impl;
    impl = 0;
}

/*!
    Returns the first direct child node for which nodeName() equals \a
    name.

    If no such direct child exists, a \link isNull() null node\endlink
    is returned.

    \sa nodeName()
*/
TQDomNode TQDomNode::namedItem( const TQString& name ) const
{
    if ( !impl )
	return TQDomNode();
    return TQDomNode( impl->namedItem( name ) );
}

/*!
    Writes the XML representation of the node and all its children to
    the stream \a str. This function uses \a indent as the amount of
    space to indent the node.
*/
void TQDomNode::save( TQTextStream& str, int indent ) const
{
    if ( impl )
	IMPL->save( str, 1, indent );
}

/*!
    \relates TQDomNode

    Writes the XML representation of the node \a node and all its
    children to the stream \a str.
*/
TQTextStream& operator<<( TQTextStream& str, const TQDomNode& node )
{
    node.save( str, 1 );

    return str;
}

/*!
    Returns true if the node is an attribute; otherwise returns false.

    If this function returns true, it does not imply that this object
    is a TQDomAttribute; you can get the TQDomAttribute with
    toAttribute().

    \sa toAttr()
*/
bool TQDomNode::isAttr() const
{
    if(impl)
	return impl->isAttr();
    return false;
}

/*!
    Returns true if the node is a CDATA section; otherwise returns
    false.

    If this function returns true, it does not imply that this object
    is a TQDomCDATASection; you can get the TQDomCDATASection with
    toCDATASection().

    \sa toCDATASection()
*/
bool TQDomNode::isCDATASection() const
{
    if(impl)
	return impl->isCDATASection();
    return false;
}

/*!
    Returns true if the node is a document fragment; otherwise returns
    false.

    If this function returns true, it does not imply that this object
    is a TQDomDocumentFragment; you can get the TQDomDocumentFragment
    with toDocumentFragment().

    \sa toDocumentFragment()
*/
bool TQDomNode::isDocumentFragment() const
{
    if(impl)
	return impl->isDocumentFragment();
    return false;
}

/*!
    Returns true if the node is a document; otherwise returns false.

    If this function returns true, it does not imply that this object
    is a TQDomDocument; you can get the TQDomDocument with toDocument().

    \sa toDocument()
*/
bool TQDomNode::isDocument() const
{
    if(impl)
	return impl->isDocument();
    return false;
}

/*!
    Returns true if the node is a document type; otherwise returns
    false.

    If this function returns true, it does not imply that this object
    is a TQDomDocumentType; you can get the TQDomDocumentType with
    toDocumentType().

    \sa toDocumentType()
*/
bool TQDomNode::isDocumentType() const
{
    if(impl)
	return impl->isDocumentType();
    return false;
}

/*!
    Returns true if the node is an element; otherwise returns false.

    If this function returns true, it does not imply that this object
    is a TQDomElement; you can get the TQDomElement with toElement().

    \sa toElement()
*/
bool TQDomNode::isElement() const
{
    if(impl)
	return impl->isElement();
    return false;
}

/*!
    Returns true if the node is an entity reference; otherwise returns
    false.

    If this function returns true, it does not imply that this object
    is a TQDomEntityReference; you can get the TQDomEntityReference with
    toEntityReference().

    \sa toEntityReference()
*/
bool TQDomNode::isEntityReference() const
{
    if(impl)
	return impl->isEntityReference();
    return false;
}

/*!
    Returns true if the node is a text node; otherwise returns false.

    If this function returns true, it does not imply that this object
    is a TQDomText; you can get the TQDomText with toText().

    \sa toText()
*/
bool TQDomNode::isText() const
{
    if(impl)
	return impl->isText();
    return false;
}

/*!
    Returns true if the node is an entity; otherwise returns false.

    If this function returns true, it does not imply that this object
    is a TQDomEntity; you can get the TQDomEntity with toEntity().

    \sa toEntity()
*/
bool TQDomNode::isEntity() const
{
    if(impl)
	return impl->isEntity();
    return false;
}

/*!
    Returns true if the node is a notation; otherwise returns false.

    If this function returns true, it does not imply that this object
    is a TQDomNotation; you can get the TQDomNotation with toNotation().

    \sa toNotation()
*/
bool TQDomNode::isNotation() const
{
    if(impl)
	return impl->isNotation();
    return false;
}

/*!
    Returns true if the node is a processing instruction; otherwise
    returns false.

    If this function returns true, it does not imply that this object
    is a TQDomProcessingInstruction; you can get the
    TQProcessingInstruction with toProcessingInstruction().

    \sa toProcessingInstruction()
*/
bool TQDomNode::isProcessingInstruction() const
{
    if(impl)
	return impl->isProcessingInstruction();
    return false;
}

/*!
    Returns true if the node is a character data node; otherwise
    returns false.

    If this function returns true, it does not imply that this object
    is a TQDomCharacterData; you can get the TQDomCharacterData with
    toCharacterData().

    \sa toCharacterData()
*/
bool TQDomNode::isCharacterData() const
{
    if(impl)
	return impl->isCharacterData();
    return false;
}

/*!
    Returns true if the node is a comment; otherwise returns false.

    If this function returns true, it does not imply that this object
    is a TQDomComment; you can get the TQDomComment with toComment().

    \sa toComment()
*/
bool TQDomNode::isComment() const
{
     if(impl)
	return impl->isComment();
    return false;
}

#undef IMPL

/**************************************************************
 *
 * TQDomNamedNodeMapPrivate
 *
 **************************************************************/

TQDomNamedNodeMapPrivate::TQDomNamedNodeMapPrivate( TQDomNodePrivate* n )
{
    readonly = false;
    parent = n;
    appendToParent = false;
}

TQDomNamedNodeMapPrivate::~TQDomNamedNodeMapPrivate()
{
    clearMap();
}

TQDomNamedNodeMapPrivate* TQDomNamedNodeMapPrivate::clone( TQDomNodePrivate* p )
{
    TQDomNamedNodeMapPrivate* m = new TQDomNamedNodeMapPrivate( p );
    m->readonly = readonly;
    m->appendToParent = appendToParent;

    TQDictIterator<TQDomNodePrivate> it ( map );
    for ( ; it.current(); ++it )
	m->setNamedItem( it.current()->cloneNode() );

    // we are no longer interested in ownership
    m->deref();
    return m;
}

void TQDomNamedNodeMapPrivate::clearMap()
{
    // Dereference all of our children if we took references
    if ( !appendToParent ) {
	TQDictIterator<TQDomNodePrivate> it( map );
	for ( ; it.current(); ++it )
	    if ( it.current()->deref() )
		delete it.current();
    }

    map.clear();
}

TQDomNodePrivate* TQDomNamedNodeMapPrivate::namedItem( const TQString& name ) const
{
    TQDomNodePrivate* p = map[ name ];
    return p;
}

TQDomNodePrivate* TQDomNamedNodeMapPrivate::namedItemNS( const TQString& nsURI, const TQString& localName ) const
{
    TQDictIterator<TQDomNodePrivate> it( map );
    TQDomNodePrivate *n = it.current();
    while ( n ) {
	if ( !n->prefix.isNull() ) {
	    // node has a namespace
	    if ( n->namespaceURI==nsURI && n->name==localName ) {
		return n;
	    }
	}
	++it;
	n = it.current();
    }
    return 0;
}

TQDomNodePrivate* TQDomNamedNodeMapPrivate::setNamedItem( TQDomNodePrivate* arg )
{
    if ( readonly || !arg )
	return 0;

    if ( appendToParent )
	return parent->appendChild( arg );

    TQDomNodePrivate *n = map[ arg->nodeName() ];
    // We take a reference
    arg->ref();
    map.insert( arg->nodeName(), arg );
    return n;
}

TQDomNodePrivate* TQDomNamedNodeMapPrivate::setNamedItemNS( TQDomNodePrivate* arg )
{
    if ( readonly || !arg )
	return 0;

    if ( appendToParent )
	return parent->appendChild( arg );

    if ( !arg->prefix.isNull() ) {
	// node has a namespace
	TQDomNodePrivate *n = namedItemNS( arg->namespaceURI, arg->name );
	// We take a reference
	arg->ref();
	map.insert( arg->nodeName(), arg );
	return n;
    } else {
	// ### check the following code if it is ok
	return setNamedItem( arg );
    }
}

TQDomNodePrivate* TQDomNamedNodeMapPrivate::removeNamedItem( const TQString& name )
{
    if ( readonly )
	return 0;

    TQDomNodePrivate* p = namedItem( name );
    if ( p == 0 )
	return 0;
    if ( appendToParent )
	return parent->removeChild( p );

    map.remove( p->nodeName() );
    // We took a reference, so we have to free one here
    p->deref();
    return p;
}

TQDomNodePrivate* TQDomNamedNodeMapPrivate::item( int index ) const
{
    if ( (uint)index >= length() )
	return 0;

    TQDictIterator<TQDomNodePrivate> it( map );
    for ( int i = 0; i < index; ++i, ++it )
	;
    return it.current();
}

uint TQDomNamedNodeMapPrivate::length() const
{
    return map.count();
}

bool TQDomNamedNodeMapPrivate::contains( const TQString& name ) const
{
    return ( map[ name ] != 0 );
}

bool TQDomNamedNodeMapPrivate::containsNS( const TQString& nsURI, const TQString & localName ) const
{
    return ( namedItemNS( nsURI, localName ) != 0 );
}

/**************************************************************
 *
 * TQDomNamedNodeMap
 *
 **************************************************************/

#define IMPL ((TQDomNamedNodeMapPrivate*)impl)

/*!
    \class TQDomNamedNodeMap ntqdom.h
    \reentrant
    \brief The TQDomNamedNodeMap class contains a collection of nodes
    that can be accessed by name.
\if defined(commercial)
    It is part of the <a href="commercialeditions.html">TQt Enterprise Edition</a>.
\endif

    \module XML
    \ingroup xml-tools

    Note that TQDomNamedNodeMap does not inherit from TQDomNodeList.
    TQDomNamedNodeMaps do not provide any specific node ordering.
    Although nodes in a TQDomNamedNodeMap may be accessed by an ordinal
    index, this is simply to allow a convenient enumeration of the
    contents of a TQDomNamedNodeMap, and does not imply that the DOM
    specifies an ordering of the nodes.

    The TQDomNamedNodeMap is used in three places:
    \list 1
    \i TQDomDocumentType::entities() returns a map of all entities
	described in the DTD.
    \i TQDomDocumentType::notations() returns a map of all notations
	described in the DTD.
    \i TQDomNode::attributes() returns a map of all attributes of an
	element.
    \endlist

    Items in the map are identified by the name which TQDomNode::name()
    returns. Nodes are retrieved using namedItem(), namedItemNS() or
    item(). New nodes are inserted with setNamedItem() or
    setNamedItemNS() and removed with removeNamedItem() or
    removeNamedItemNS(). Use contains() to see if an item with the
    given name is in the named node map. The number of items is
    returned by length().

    Terminology: in this class we use "item" and "node"
    interchangeably.
*/

/*!
    Constructs an empty named node map.
*/
TQDomNamedNodeMap::TQDomNamedNodeMap()
{
    impl = 0;
}

/*!
    Constructs a copy of \a n.
*/
TQDomNamedNodeMap::TQDomNamedNodeMap( const TQDomNamedNodeMap& n )
{
    impl = n.impl;
    if ( impl )
	impl->ref();
}

TQDomNamedNodeMap::TQDomNamedNodeMap( TQDomNamedNodeMapPrivate* n )
{
    impl = n;
    if ( impl )
	impl->ref();
}

/*!
    Assigns \a n to this named node map.
*/
TQDomNamedNodeMap& TQDomNamedNodeMap::operator= ( const TQDomNamedNodeMap& n )
{
    if ( impl && impl->deref() )
	delete impl;
    impl = n.impl;
    if ( impl )
	impl->ref();

    return *this;
}

/*!
    Returns true if \a n and this named node map are equal; otherwise
    returns false.
*/
bool TQDomNamedNodeMap::operator== ( const TQDomNamedNodeMap& n ) const
{
    return ( impl == n.impl );
}

/*!
    Returns true if \a n and this named node map are not equal;
    otherwise returns false.
*/
bool TQDomNamedNodeMap::operator!= ( const TQDomNamedNodeMap& n ) const
{
    return ( impl != n.impl );
}

/*!
    Destroys the object and frees its resources.
*/
TQDomNamedNodeMap::~TQDomNamedNodeMap()
{
    if ( impl && impl->deref() )
	delete impl;
}

/*!
    Returns the node called \a name.

    If the named node map does not contain such a node, a \link
    TQDomNode::isNull() null node\endlink is returned. A node's name is
    the name returned by TQDomNode::nodeName().

    \sa setNamedItem() namedItemNS()
*/
TQDomNode TQDomNamedNodeMap::namedItem( const TQString& name ) const
{
    if ( !impl )
	return TQDomNode();
    return TQDomNode( IMPL->namedItem( name ) );
}

/*!
    Inserts the node \a newNode into the named node map. The name used
    by the map is the node name of \a newNode as returned by
    TQDomNode::nodeName().

    If the new node replaces an existing node, i.e. the map contains a
    node with the same name, the replaced node is returned.

    \sa namedItem() removeNamedItem() setNamedItemNS()
*/
TQDomNode TQDomNamedNodeMap::setNamedItem( const TQDomNode& newNode )
{
    if ( !impl )
	return TQDomNode();
    return TQDomNode( IMPL->setNamedItem( (TQDomNodePrivate*)newNode.impl ) );
}

/*!
    Removes the node called \a name from the map.

    The function returns the removed node or a \link
    TQDomNode::isNull() null node\endlink if the map did not contain a
    node called \a name.

    \sa setNamedItem() namedItem() removeNamedItemNS()
*/
TQDomNode TQDomNamedNodeMap::removeNamedItem( const TQString& name )
{
    if ( !impl )
	return TQDomNode();
    return TQDomNode( IMPL->removeNamedItem( name ) );
}

/*!
    Retrieves the node at position \a index.

    This can be used to iterate over the map. Note that the nodes in
    the map are ordered arbitrarily.

    \sa length()
*/
TQDomNode TQDomNamedNodeMap::item( int index ) const
{
    if ( !impl )
	return TQDomNode();
    return TQDomNode( IMPL->item( index ) );
}

/*!
    Returns the node associated with the local name \a localName and
    the namespace URI \a nsURI.

    If the map does not contain such a node, a \link
    TQDomNode::isNull() null node\endlink is returned.

    \sa setNamedItemNS() namedItem()
*/
TQDomNode TQDomNamedNodeMap::namedItemNS( const TQString& nsURI, const TQString& localName ) const
{
    if ( !impl )
	return TQDomNode();
    return TQDomNode( IMPL->namedItemNS( nsURI, localName ) );
}

/*!
    Inserts the node \a newNode in the map. If a node with the same
    namespace URI and the same local name already exists in the map,
    it is replaced by \a newNode. If the new node replaces an existing
    node, the replaced node is returned.

    \sa namedItemNS() removeNamedItemNS() setNamedItem()
*/
TQDomNode TQDomNamedNodeMap::setNamedItemNS( const TQDomNode& newNode )
{
    if ( !impl )
	return TQDomNode();
    return TQDomNode( IMPL->setNamedItemNS( (TQDomNodePrivate*)newNode.impl ) );
}

/*!
    Removes the node with the local name \a localName and the
    namespace URI \a nsURI from the map.

    The function returns the removed node or a \link
    TQDomNode::isNull() null node\endlink if the map did not contain a
    node with the local name \a localName and the namespace URI \a
    nsURI.

    \sa setNamedItemNS() namedItemNS() removeNamedItem()
*/
TQDomNode TQDomNamedNodeMap::removeNamedItemNS( const TQString& nsURI, const TQString& localName )
{
    if ( !impl )
	return TQDomNode();
    TQDomNodePrivate *n = IMPL->namedItemNS( nsURI, localName );
    if ( !n )
	return TQDomNode();
    return TQDomNode( IMPL->removeNamedItem( n->name ) );
}

/*!
    Returns the number of nodes in the map.

    \sa item()
*/
uint TQDomNamedNodeMap::length() const
{
    if ( !impl )
	return 0;
    return IMPL->length();
}

/*!
    \fn uint TQDomNamedNodeMap::count() const

    Returns the number of nodes in the map.

    This function is the same as length().
*/

/*!
    Returns true if the map contains a node called \a name; otherwise
    returns false.
*/
bool TQDomNamedNodeMap::contains( const TQString& name ) const
{
    if ( !impl )
	return false;
    return IMPL->contains( name );
}

#undef IMPL

/**************************************************************
 *
 * TQDomDocumentTypePrivate
 *
 **************************************************************/

TQDomDocumentTypePrivate::TQDomDocumentTypePrivate( TQDomDocumentPrivate* doc, TQDomNodePrivate* parent )
    : TQDomNodePrivate( doc, parent )
{
    init();
}

TQDomDocumentTypePrivate::TQDomDocumentTypePrivate( TQDomDocumentTypePrivate* n, bool deep )
    : TQDomNodePrivate( n, deep )
{
    init();
    // Refill the maps with our new children
    TQDomNodePrivate* p = first;
    while ( p ) {
	if ( p->isEntity() )
	    // Dont use normal insert function since we would create infinite recursion
	    entities->map.insert( p->nodeName(), p );
	if ( p->isNotation() )
	    // Dont use normal insert function since we would create infinite recursion
	    notations->map.insert( p->nodeName(), p );
    }
}

TQDomDocumentTypePrivate::~TQDomDocumentTypePrivate()
{
    if ( entities->deref() )
	delete entities;
    if ( notations->deref() )
	delete notations;
}

void TQDomDocumentTypePrivate::init()
{
    entities = new TQDomNamedNodeMapPrivate( this );
    notations = new TQDomNamedNodeMapPrivate( this );
    publicId = TQString::null;
    systemId = TQString::null;
    internalSubset = TQString::null;

    entities->setAppendToParent( true );
    notations->setAppendToParent( true );
}

TQDomNodePrivate* TQDomDocumentTypePrivate::cloneNode( bool deep)
{
    TQDomNodePrivate* p = new TQDomDocumentTypePrivate( this, deep );
    // We are not interested in this node
    p->deref();
    return p;
}

TQDomNodePrivate* TQDomDocumentTypePrivate::insertBefore( TQDomNodePrivate* newChild, TQDomNodePrivate* refChild )
{
    // Call the origianl implementation
    TQDomNodePrivate* p = TQDomNodePrivate::insertBefore( newChild, refChild );
    // Update the maps
    if ( p && p->isEntity() )
	entities->map.insert( p->nodeName(), p );
    else if ( p && p->isNotation() )
	notations->map.insert( p->nodeName(), p );

    return p;
}

TQDomNodePrivate* TQDomDocumentTypePrivate::insertAfter( TQDomNodePrivate* newChild, TQDomNodePrivate* refChild )
{
    // Call the origianl implementation
    TQDomNodePrivate* p = TQDomNodePrivate::insertAfter( newChild, refChild );
    // Update the maps
    if ( p && p->isEntity() )
	entities->map.insert( p->nodeName(), p );
    else if ( p && p->isNotation() )
	notations->map.insert( p->nodeName(), p );

    return p;
}

TQDomNodePrivate* TQDomDocumentTypePrivate::replaceChild( TQDomNodePrivate* newChild, TQDomNodePrivate* oldChild )
{
    // Call the origianl implementation
    TQDomNodePrivate* p = TQDomNodePrivate::replaceChild( newChild, oldChild );
    // Update the maps
    if ( p ) {
	if ( oldChild && oldChild->isEntity() )
	    entities->map.remove( oldChild->nodeName() );
	else if ( oldChild && oldChild->isNotation() )
	    notations->map.remove( oldChild->nodeName() );

	if ( p->isEntity() )
	    entities->map.insert( p->nodeName(), p );
	else if ( p->isNotation() )
	    notations->map.insert( p->nodeName(), p );
    }

    return p;
}

TQDomNodePrivate* TQDomDocumentTypePrivate::removeChild( TQDomNodePrivate* oldChild )
{
    // Call the origianl implementation
    TQDomNodePrivate* p = TQDomNodePrivate::removeChild(  oldChild );
    // Update the maps
    if ( p && p->isEntity() )
	entities->map.remove( p->nodeName() );
    else if ( p && p->isNotation() )
	notations->map.remove( p ->nodeName() );

    return p;
}

TQDomNodePrivate* TQDomDocumentTypePrivate::appendChild( TQDomNodePrivate* newChild )
{
    return insertAfter( newChild, 0 );
}

void TQDomDocumentTypePrivate::save( TQTextStream& s, int, int indent ) const
{
    if ( name.isEmpty() )
	return;

    s << "<!DOCTYPE " << name;

    if ( !publicId.isNull() ) {
	s << " PUBLIC \"" << publicId << "\"";
	if ( !systemId.isNull() )
	    s << " \"" << systemId << "\"";
    } else if ( !systemId.isNull() ) {
	s << " SYSTEM \"" << systemId << "\"";
    }

    if ( entities->length()>0 || notations->length()>0 ) {
	s << " [ " << endl;

	TQDictIterator<TQDomNodePrivate> it2( notations->map );
	for ( ; it2.current(); ++it2 )
	    it2.current()->save( s, 0, indent );

	TQDictIterator<TQDomNodePrivate> it( entities->map );
	for ( ; it.current(); ++it )
	    it.current()->save( s, 0, indent );

	s << " ]";
    }

    s << ">" << endl;
}

/**************************************************************
 *
 * TQDomDocumentType
 *
 **************************************************************/

#define IMPL ((TQDomDocumentTypePrivate*)impl)

/*!
    \class TQDomDocumentType ntqdom.h
    \reentrant
    \brief The TQDomDocumentType class is the representation of the DTD
    in the document tree.
\if defined(commercial)
    It is part of the <a href="commercialeditions.html">TQt Enterprise Edition</a>.
\endif

    \module XML
    \ingroup xml-tools

    The TQDomDocumentType class allows read-only access to some of the
    data structures in the DTD: it can return a map of all entities()
    and notations(). In addition the function name() returns the name
    of the document type as specified in the &lt;!DOCTYPE name&gt;
    tag. This class also provides the publicId(), systemId() and
    internalSubset() functions.

    \sa TQDomDocument
*/

/*!
    Creates an empty TQDomDocumentType object.
*/
TQDomDocumentType::TQDomDocumentType() : TQDomNode()
{
}

/*!
    Constructs a copy of \a n.

    The data of the copy is shared (shallow copy): modifying one node
    will also change the other. If you want to make a deep copy, use
    cloneNode().
*/
TQDomDocumentType::TQDomDocumentType( const TQDomDocumentType& n )
    : TQDomNode( n )
{
}

TQDomDocumentType::TQDomDocumentType( TQDomDocumentTypePrivate* n )
    : TQDomNode( n )
{
}

/*!
    Assigns \a n to this document type.

    The data of the copy is shared (shallow copy): modifying one node
    will also change the other. If you want to make a deep copy, use
    cloneNode().
*/
TQDomDocumentType& TQDomDocumentType::operator= ( const TQDomDocumentType& n )
{
    return (TQDomDocumentType&) TQDomNode::operator=( n );
}

/*!
    Destroys the object and frees its resources.
*/
TQDomDocumentType::~TQDomDocumentType()
{
}

/*!
    Returns the name of the document type as specified in the
    &lt;!DOCTYPE name&gt; tag.

    \sa nodeName()
*/
TQString TQDomDocumentType::name() const
{
    if ( !impl )
	return TQString::null;

    return IMPL->nodeName();
}

/*!
    Returns a map of all entities described in the DTD.
*/
TQDomNamedNodeMap TQDomDocumentType::entities() const
{
    if ( !impl )
	return TQDomNamedNodeMap();
    return TQDomNamedNodeMap( IMPL->entities );
}

/*!
    Returns a map of all notations described in the DTD.
*/
TQDomNamedNodeMap TQDomDocumentType::notations() const
{
    if ( !impl )
	return TQDomNamedNodeMap();
    return TQDomNamedNodeMap( IMPL->notations );
}

/*!
    Returns the public identifier of the external DTD subset or
    TQString::null if there is no public identifier.

    \sa systemId() internalSubset() TQDomImplementation::createDocumentType()
*/
TQString TQDomDocumentType::publicId() const
{
    if ( !impl )
	return TQString::null;
    return IMPL->publicId;
}

/*!
    Returns the system identifier of the external DTD subset or
    TQString::null if there is no system identifier.

    \sa publicId() internalSubset() TQDomImplementation::createDocumentType()
*/
TQString TQDomDocumentType::systemId() const
{
    if ( !impl )
	return TQString::null;
    return IMPL->systemId;
}

/*!
    Returns the internal subset of the document type or TQString::null
    if there is no internal subset.

    \sa publicId() systemId()
*/
TQString TQDomDocumentType::internalSubset() const
{
    if ( !impl )
	return TQString::null;
    return IMPL->internalSubset;
}

/*!
    Returns \c DocumentTypeNode.

    \sa isDocumentType() TQDomNode::toDocumentType()
*/
TQDomNode::NodeType TQDomDocumentType::nodeType() const
{
    return DocumentTypeNode;
}

/*!
    This function overloads TQDomNode::isDocumentType().

    \sa nodeType() TQDomNode::toDocumentType()
*/
bool TQDomDocumentType::isDocumentType() const
{
    return true;
}

#undef IMPL

/**************************************************************
 *
 * TQDomDocumentFragmentPrivate
 *
 **************************************************************/

TQDomDocumentFragmentPrivate::TQDomDocumentFragmentPrivate( TQDomDocumentPrivate* doc, TQDomNodePrivate* parent )
    : TQDomNodePrivate( doc, parent )
{
    name = "#document-fragment";
}

TQDomDocumentFragmentPrivate::TQDomDocumentFragmentPrivate( TQDomNodePrivate* n, bool deep )
    : TQDomNodePrivate( n, deep )
{
}

TQDomDocumentFragmentPrivate::~TQDomDocumentFragmentPrivate()
{
}

TQDomNodePrivate* TQDomDocumentFragmentPrivate::cloneNode( bool deep)
{
    TQDomNodePrivate* p = new TQDomDocumentFragmentPrivate( this, deep );
    // We are not interested in this node
    p->deref();
    return p;
}

/**************************************************************
 *
 * TQDomDocumentFragment
 *
 **************************************************************/

#define IMPL ((TQDomDocumentFragmentPrivate*)impl)

/*!
    \class TQDomDocumentFragment ntqdom.h
    \reentrant
    \brief The TQDomDocumentFragment class is a tree of TQDomNodes which is not usually a complete TQDomDocument.
\if defined(commercial)
    It is part of the <a href="commercialeditions.html">TQt Enterprise Edition</a>.
\endif

    \module XML
    \ingroup xml-tools

    If you want to do complex tree operations it is useful to have a
    lightweight class to store nodes and their relations.
    TQDomDocumentFragment stores a subtree of a document which does not
    necessarily represent a well-formed XML document.

    TQDomDocumentFragment is also useful if you want to group several
    nodes in a list and insert them all together as children of some
    node. In these cases TQDomDocumentFragment can be used as a
    temporary container for this list of children.

    The most important feature of TQDomDocumentFragment is that it is
    treated in a special way by TQDomNode::insertAfter(),
    TQDomNode::insertBefore(), TQDomNode::replaceChild() and
    TQDomNode::appendChild(): instead of inserting the fragment itself, all
    the fragment's children are inserted.
*/

/*!
    Constructs an empty document fragment.
*/
TQDomDocumentFragment::TQDomDocumentFragment()
{
}

TQDomDocumentFragment::TQDomDocumentFragment( TQDomDocumentFragmentPrivate* n )
    : TQDomNode( n )
{
}

/*!
    Constructs a copy of \a x.

    The data of the copy is shared (shallow copy): modifying one node
    will also change the other. If you want to make a deep copy, use
    cloneNode().
*/
TQDomDocumentFragment::TQDomDocumentFragment( const TQDomDocumentFragment& x )
    : TQDomNode( x )
{
}

/*!
    Assigns \a x to this DOM document fragment.

    The data of the copy is shared (shallow copy): modifying one node
    will also change the other. If you want to make a deep copy, use
    cloneNode().
*/
TQDomDocumentFragment& TQDomDocumentFragment::operator= ( const TQDomDocumentFragment& x )
{
    return (TQDomDocumentFragment&) TQDomNode::operator=( x );
}

/*!
    Destroys the object and frees its resources.
*/
TQDomDocumentFragment::~TQDomDocumentFragment()
{
}

/*!
    Returns \c DocumentFragment.

    \sa isDocumentFragment() TQDomNode::toDocumentFragment()
*/
TQDomNode::NodeType TQDomDocumentFragment::nodeType() const
{
    return TQDomNode::DocumentFragmentNode;
}

/*!
    This function reimplements TQDomNode::isDocumentFragment().

    \sa nodeType() TQDomNode::toDocumentFragment()
*/
bool TQDomDocumentFragment::isDocumentFragment() const
{
    return true;
}

#undef IMPL

/**************************************************************
 *
 * TQDomCharacterDataPrivate
 *
 **************************************************************/

TQDomCharacterDataPrivate::TQDomCharacterDataPrivate( TQDomDocumentPrivate* d, TQDomNodePrivate* p,
						      const TQString& data )
    : TQDomNodePrivate( d, p )
{
    value = data;

    name = "#character-data";
}

TQDomCharacterDataPrivate::TQDomCharacterDataPrivate( TQDomCharacterDataPrivate* n, bool deep )
    : TQDomNodePrivate( n, deep )
{
}

TQDomCharacterDataPrivate::~TQDomCharacterDataPrivate()
{
}

TQDomNodePrivate* TQDomCharacterDataPrivate::cloneNode( bool deep )
{
    TQDomNodePrivate* p = new TQDomCharacterDataPrivate( this, deep );
    // We are not interested in this node
    p->deref();
    return p;
}

uint TQDomCharacterDataPrivate::dataLength() const
{
    return value.length();
}

TQString TQDomCharacterDataPrivate::substringData( unsigned long offset, unsigned long n ) const
{
    return value.mid( offset, n );
}

void TQDomCharacterDataPrivate::insertData( unsigned long offset, const TQString& arg )
{
    value.insert( offset, arg );
}

void TQDomCharacterDataPrivate::deleteData( unsigned long offset, unsigned long n )
{
    value.remove( offset, n );
}

void TQDomCharacterDataPrivate::replaceData( unsigned long offset, unsigned long n, const TQString& arg )
{
    value.replace( offset, n, arg );
}

void TQDomCharacterDataPrivate::appendData( const TQString& arg )
{
    value += arg;
}

/**************************************************************
 *
 * TQDomCharacterData
 *
 **************************************************************/

#define IMPL ((TQDomCharacterDataPrivate*)impl)

/*!
    \class TQDomCharacterData ntqdom.h
    \reentrant
    \brief The TQDomCharacterData class represents a generic string in the DOM.
\if defined(commercial)
    It is part of the <a href="commercialeditions.html">TQt Enterprise Edition</a>.
\endif

    \module XML
    \ingroup xml-tools

    Character data as used in XML specifies a generic data string.
    More specialized versions of this class are TQDomText, TQDomComment
    and TQDomCDATASection.

    The data string is set with setData() and retrieved with data().
    You can retrieve a portion of the data string using
    substringData(). Extra data can be appended with appendData(), or
    inserted with insertData(). Portions of the data string can be
    deleted with deleteData() or replaced with replaceData(). The
    length of the data string is returned by length().

    The node type of the node containing this character data is
    returned by nodeType().

    \sa TQDomText TQDomComment TQDomCDATASection
*/

/*!
    Constructs an empty character data object.
*/
TQDomCharacterData::TQDomCharacterData()
{
}

/*!
    Constructs a copy of \a x.

    The data of the copy is shared (shallow copy): modifying one node
    will also change the other. If you want to make a deep copy, use
    cloneNode().
*/
TQDomCharacterData::TQDomCharacterData( const TQDomCharacterData& x )
    : TQDomNode( x )
{
}

TQDomCharacterData::TQDomCharacterData( TQDomCharacterDataPrivate* n )
    : TQDomNode( n )
{
}

/*!
    Assigns \a x to this character data.

    The data of the copy is shared (shallow copy): modifying one node
    will also change the other. If you want to make a deep copy, use
    cloneNode().
*/
TQDomCharacterData& TQDomCharacterData::operator= ( const TQDomCharacterData& x )
{
    return (TQDomCharacterData&) TQDomNode::operator=( x );
}

/*!
    Destroys the object and frees its resources.
*/
TQDomCharacterData::~TQDomCharacterData()
{
}

/*!
    Returns the string stored in this object.

    If the node is a \link isNull() null node\endlink, it will return
    TQString::null.
*/
TQString TQDomCharacterData::data() const
{
    if ( !impl )
	return TQString::null;
    return impl->nodeValue();
}

/*!
    Sets this object's string to \a v.
*/
void TQDomCharacterData::setData( const TQString& v )
{
    if ( impl )
	impl->setNodeValue( v );
}

/*!
    Returns the length of the stored string.
*/
uint TQDomCharacterData::length() const
{
    if ( impl )
	return IMPL->dataLength();
    return 0;
}

/*!
    Returns the substring of length \a count from position \a offset.
*/
TQString TQDomCharacterData::substringData( unsigned long offset, unsigned long count )
{
    if ( !impl )
	return TQString::null;
    return IMPL->substringData( offset, count );
}

/*!
    Appends the string \a arg to the stored string.
*/
void TQDomCharacterData::appendData( const TQString& arg )
{
    if ( impl )
	IMPL->appendData( arg );
}

/*!
    Inserts the string \a arg into the stored string at position \a offset.
*/
void TQDomCharacterData::insertData( unsigned long offset, const TQString& arg )
{
    if ( impl )
	IMPL->insertData( offset, arg );
}

/*!
    Deletes a substring of length \a count from position \a offset.
*/
void TQDomCharacterData::deleteData( unsigned long offset, unsigned long count )
{
    if ( impl )
	IMPL->deleteData( offset, count );
}

/*!
    Replaces the substring of length \a count starting at position \a
    offset with the string \a arg.
*/
void TQDomCharacterData::replaceData( unsigned long offset, unsigned long count, const TQString& arg )
{
    if ( impl )
	IMPL->replaceData( offset, count, arg );
}

/*!
    Returns the type of node this object refers to (i.e. \c TextNode,
    \c CDATASectionNode, \c CommentNode or \c CharacterDataNode). For
    a \link isNull() null node\endlink \c CharacterDataNode is
    returned.
*/
TQDomNode::NodeType TQDomCharacterData::nodeType() const
{
    if( !impl )
	return CharacterDataNode;
    return TQDomNode::nodeType();
}

/*!
    Returns true.
*/
bool TQDomCharacterData::isCharacterData() const
{
    return true;
}

#undef IMPL

/**************************************************************
 *
 * TQDomAttrPrivate
 *
 **************************************************************/

TQDomAttrPrivate::TQDomAttrPrivate( TQDomDocumentPrivate* d, TQDomNodePrivate* parent, const TQString& name_ )
    : TQDomNodePrivate( d, parent )
{
    name = name_;
    m_specified = false;
}

TQDomAttrPrivate::TQDomAttrPrivate( TQDomDocumentPrivate* d, TQDomNodePrivate* p, const TQString& nsURI, const TQString& qName )
    : TQDomNodePrivate( d, p )
{
    qt_split_namespace( prefix, name, qName, !nsURI.isNull() );
    namespaceURI = nsURI;
    createdWithDom1Interface = false;
    m_specified = false;
}

TQDomAttrPrivate::TQDomAttrPrivate( TQDomAttrPrivate* n, bool deep )
    : TQDomNodePrivate( n, deep )
{
    m_specified = n->specified();
}

TQDomAttrPrivate::~TQDomAttrPrivate()
{
}

void TQDomAttrPrivate::setNodeValue( const TQString& v )
{
    value = v;
    TQDomTextPrivate *t = new TQDomTextPrivate( 0, this, v );
    // keep the refcount balanced: appendChild() does a ref() anyway.
    t->deref();
    if ( first ) {
	delete removeChild( first );
    }
    appendChild( t );
}

TQDomNodePrivate* TQDomAttrPrivate::cloneNode( bool deep )
{
    TQDomNodePrivate* p = new TQDomAttrPrivate( this, deep );
    // We are not interested in this node
    p->deref();
    return p;
}

bool TQDomAttrPrivate::specified() const
{
    return m_specified;
}

static bool isXmlChar(const TQChar &c)
{
    // Characters in this range must be accepted by XML parsers.
    // Consequently characters outside of this range need to be escaped.

    ushort uc = c.unicode();

    return uc == 0x9
            || uc == 0xA
            || uc == 0xD
            || ( 0x20 <= uc && uc <= 0xD7FF )
            || ( 0xE000 <= uc && uc <= 0xFFFD );
}

/*
  Encode an attribute value upon saving.
*/
static TQString encodeAttr( const TQString& str )
{
    TQString tmp( str );
    uint len = tmp.length();
    uint i = 0;
    while ( i < len ) {
	if ( tmp[(int)i] == '<' ) {
	    tmp.replace( i, 1, "&lt;" );
	    len += 3;
	    i += 4;
	} else if ( tmp[(int)i] == '"' ) {
	    tmp.replace( i, 1, "&quot;" );
	    len += 5;
	    i += 6;
	} else if ( tmp[(int)i] == '&' ) {
	    tmp.replace( i, 1, "&amp;" );
	    len += 4;
	    i += 5;
	} else if ( tmp[(int)i] == '>' && i>=2 && tmp[(int)i-1]==']' && tmp[(int)i-2]==']' ) {
	    tmp.replace( i, 1, "&gt;" );
	    len += 3;
	    i += 4;
        } else if (!isXmlChar(tmp[(int)i])) {
            TQString repl = "&#x" + TQString::number(tmp[(int)i].unicode(), 16) + ';';
            tqWarning("TQDom: saving invalid character %s, the document will not be well-formed", repl.latin1());
            tmp.replace(i, 1, repl);
            len += repl.length() - 1;
            i += repl.length();
        } else {
	    ++i;
	}
    }

    return tmp;
}

void TQDomAttrPrivate::save( TQTextStream& s, int, int ) const
{
    if ( namespaceURI.isNull() ) {
	s << name << "=\"" << encodeAttr( value ) << "\"";
    } else {
	// ### optimize this (see comment of TQDomElementPrivate::save()
	s << prefix << ":" << name << "=\"" << encodeAttr( value ) << "\""
	    << " xmlns:" << prefix << "=\"" << encodeAttr( namespaceURI ) << "\"";
    }
}

/**************************************************************
 *
 * TQDomAttr
 *
 **************************************************************/

#define IMPL ((TQDomAttrPrivate*)impl)

/*!
    \class TQDomAttr ntqdom.h
    \reentrant
    \brief The TQDomAttr class represents one attribute of a TQDomElement.
\if defined(commercial)
    It is part of the <a href="commercialeditions.html">TQt Enterprise Edition</a>.
\endif

    \module XML
    \ingroup xml-tools

    For example, the following piece of XML produces an element with
    no children, but two attributes:

    \code
    <link href="http://www.trolltech.com" color="red" />
    \endcode

    You can access the attributes of an element with code like this:

    \code
    TQDomElement e = //...
    //...
    TQDomAttr a = e.attributeNode( "href" );
    cout << a.value() << endl;                // prints "http://www.trolltech.com"
    a.setValue( "http://doc.trolltech.com" ); // change the node's attribute
    TQDomAttr a2 = e.attributeNode( "href" );
    cout << a2.value() << endl;               // prints "http://doc.trolltech.com"
    \endcode

    This example also shows that changing an attribute received from
    an element changes the attribute of the element. If you do not
    want to change the value of the element's attribute you must
    use cloneNode() to get an independent copy of the attribute.

    TQDomAttr can return the name() and value() of an attribute. An
    attribute's value is set with setValue(). If specified() returns
    true the value was either set in the document or set with
    setValue(); otherwise the value hasn't been set. The node this
    attribute is attached to (if any) is returned by ownerElement().

    For further information about the Document Object Model see
    \link http://www.w3.org/TR/REC-DOM-Level-1/\endlink and
    \link http://www.w3.org/TR/DOM-Level-2-Core/\endlink.
    For a more general introduction of the DOM implementation see the
    TQDomDocument documentation.
*/


/*!
    Constructs an empty attribute.
*/
TQDomAttr::TQDomAttr()
{
}

/*!
    Constructs a copy of \a x.

    The data of the copy is shared (shallow copy): modifying one node
    will also change the other. If you want to make a deep copy, use
    cloneNode().
*/
TQDomAttr::TQDomAttr( const TQDomAttr& x )
    : TQDomNode( x )
{
}

TQDomAttr::TQDomAttr( TQDomAttrPrivate* n )
    : TQDomNode( n )
{
}

/*!
    Assigns \a x to this DOM attribute.

    The data of the copy is shared (shallow copy): modifying one node
    will also change the other. If you want to make a deep copy, use
    cloneNode().
*/
TQDomAttr& TQDomAttr::operator= ( const TQDomAttr& x )
{
    return (TQDomAttr&) TQDomNode::operator=( x );
}

/*!
    Destroys the object and frees its resources.
*/
TQDomAttr::~TQDomAttr()
{
}

/*!
    Returns the attribute's name.
*/
TQString TQDomAttr::name() const
{
    if ( !impl )
	return TQString::null;
    return impl->nodeName();
}

/*!
    Returns true if the attribute has either been expicitly specified
    in the XML document or was set by the user with setValue().
    Returns false if the value hasn't been specified or set.

    \sa setValue()
*/
bool TQDomAttr::specified() const
{
    if ( !impl )
	return false;
    return IMPL->specified();
}

/*!
    Returns the element node this attribute is attached to or a \link
    TQDomNode::isNull() null node\endlink if this attribute is not
    attached to any element.
*/
TQDomElement TQDomAttr::ownerElement() const
{
    if ( !impl && !impl->parent()->isElement() )
	return TQDomElement();
    return TQDomElement( (TQDomElementPrivate*)(impl->parent()) );
}

/*!
    Returns the value of the attribute or TQString::null if the
    attribute has not been specified.

    \sa specified() setValue()
*/
TQString TQDomAttr::value() const
{
    if ( !impl )
	return TQString::null;
    return impl->nodeValue();
}

/*!
    Sets the attribute's value to \a v.

    \sa value()
*/
void TQDomAttr::setValue( const TQString& v )
{
    if ( !impl )
	return;
    impl->setNodeValue( v );
    IMPL->m_specified = true;
}

/*!
    Returns \link TQDomNode::NodeType AttributeNode\endlink.
*/
TQDomNode::NodeType TQDomAttr::nodeType() const
{
    return AttributeNode;
}

/*!
    Returns true.
*/
bool TQDomAttr::isAttr() const
{
    return true;
}

#undef IMPL

/**************************************************************
 *
 * TQDomElementPrivate
 *
 **************************************************************/

TQDomElementPrivate::TQDomElementPrivate( TQDomDocumentPrivate* d, TQDomNodePrivate* p,
					  const TQString& tagname )
    : TQDomNodePrivate( d, p )
{
    name = tagname;
    m_attr = new TQDomNamedNodeMapPrivate( this );
}

TQDomElementPrivate::TQDomElementPrivate( TQDomDocumentPrivate* d, TQDomNodePrivate* p,
	const TQString& nsURI, const TQString& qName )
    : TQDomNodePrivate( d, p )
{
    qt_split_namespace( prefix, name, qName, !nsURI.isNull() );
    namespaceURI = nsURI;
    createdWithDom1Interface = false;
    m_attr = new TQDomNamedNodeMapPrivate( this );
}

TQDomElementPrivate::TQDomElementPrivate( TQDomElementPrivate* n, bool deep ) :
    TQDomNodePrivate( n, deep )
{
    m_attr = n->m_attr->clone( this );
    // Reference is down to 0, so we set it to 1 here.
    m_attr->ref();
}

TQDomElementPrivate::~TQDomElementPrivate()
{
    if ( m_attr->deref() )
	delete m_attr;
}

TQDomNodePrivate* TQDomElementPrivate::cloneNode( bool deep)
{
    TQDomNodePrivate* p = new TQDomElementPrivate( this, deep );
    // We are not interested in this node
    p->deref();
    return p;
}

TQString TQDomElementPrivate::attribute( const TQString& name_, const TQString& defValue ) const
{
    TQDomNodePrivate* n = m_attr->namedItem( name_ );
    if ( !n )
	return defValue;

    return n->nodeValue();
}

TQString TQDomElementPrivate::attributeNS( const TQString& nsURI, const TQString& localName, const TQString& defValue ) const
{
    TQDomNodePrivate* n = m_attr->namedItemNS( nsURI, localName );
    if ( !n )
	return defValue;

    return n->nodeValue();
}

void TQDomElementPrivate::setAttribute( const TQString& aname, const TQString& newValue )
{
    TQDomNodePrivate* n = m_attr->namedItem( aname );
    if ( !n ) {
	n = new TQDomAttrPrivate( ownerDocument(), this, aname );
	n->setNodeValue( newValue );

	// Referencing is done by the map, so we set the reference counter back
	// to 0 here. This is ok since we created the TQDomAttrPrivate.
	n->deref();
	m_attr->setNamedItem( n );
    } else {
	n->setNodeValue( newValue );
    }
}

void TQDomElementPrivate::setAttributeNS( const TQString& nsURI, const TQString& qName, const TQString& newValue )
{
    TQString prefix, localName;
    qt_split_namespace( prefix, localName, qName, true );
    TQDomNodePrivate* n = m_attr->namedItemNS( nsURI, localName );
    if ( !n ) {
	n = new TQDomAttrPrivate( ownerDocument(), this, nsURI, qName );
	n->setNodeValue( newValue );

	// Referencing is done by the map, so we set the reference counter back
	// to 0 here. This is ok since we created the TQDomAttrPrivate.
	n->deref();
	m_attr->setNamedItem( n );
    } else {
	n->setNodeValue( newValue );
	n->prefix = prefix;
    }
}

void TQDomElementPrivate::removeAttribute( const TQString& aname )
{
    TQDomNodePrivate* p = m_attr->removeNamedItem( aname );
    if ( p && p->count == 0 )
	delete p;
}

TQDomAttrPrivate* TQDomElementPrivate::attributeNode( const TQString& aname )
{
    return (TQDomAttrPrivate*)m_attr->namedItem( aname );
}

TQDomAttrPrivate* TQDomElementPrivate::attributeNodeNS( const TQString& nsURI, const TQString& localName )
{
    return (TQDomAttrPrivate*)m_attr->namedItemNS( nsURI, localName );
}

TQDomAttrPrivate* TQDomElementPrivate::setAttributeNode( TQDomAttrPrivate* newAttr )
{
    TQDomNodePrivate* n = m_attr->namedItem( newAttr->nodeName() );

    // Referencing is done by the maps
    m_attr->setNamedItem( newAttr );

    return (TQDomAttrPrivate*)n;
}

TQDomAttrPrivate* TQDomElementPrivate::setAttributeNodeNS( TQDomAttrPrivate* newAttr )
{
    TQDomNodePrivate* n = 0;
    if ( !newAttr->prefix.isNull() )
	n = m_attr->namedItemNS( newAttr->namespaceURI, newAttr->name );

    // Referencing is done by the maps
    m_attr->setNamedItem( newAttr );

    return (TQDomAttrPrivate*)n;
}

TQDomAttrPrivate* TQDomElementPrivate::removeAttributeNode( TQDomAttrPrivate* oldAttr )
{
    return (TQDomAttrPrivate*)m_attr->removeNamedItem( oldAttr->nodeName() );
}

bool TQDomElementPrivate::hasAttribute( const TQString& aname )
{
    return m_attr->contains( aname );
}

bool TQDomElementPrivate::hasAttributeNS( const TQString& nsURI, const TQString& localName )
{
    return m_attr->containsNS( nsURI, localName );
}

TQString TQDomElementPrivate::text()
{
    TQString t( "" );

    TQDomNodePrivate* p = first;
    while ( p ) {
	if ( p->isText() || p->isCDATASection() )
	    t += p->nodeValue();
	else if ( p->isElement() )
	    t += ((TQDomElementPrivate*)p)->text();
	p = p->next;
    }

    return t;
}

void TQDomElementPrivate::save( TQTextStream& s, int depth, int indent ) const
{
    if ( !( prev && prev->isText() ) )
	for ( int i = 0; i < depth*indent; ++i )
	    s << " ";

    TQString qName( name );
    TQString nsDecl( "" );
    if ( !namespaceURI.isNull() ) {
	// ### optimize this, so that you only declare namespaces that are not
	// yet declared -- we loose default namespace mappings, so maybe we
	// should rather store the information that we get from
	// startPrefixMapping()/endPrefixMapping() and use them (you have to
	// take care if the DOM tree is modified, though)
	if ( prefix.isEmpty() ) {
	    nsDecl = " xmlns";
	} else {
	    qName = prefix + ":" + name;
	    nsDecl = " xmlns:" + prefix;
	}
	nsDecl += "=\"" + encodeAttr( namespaceURI ) + "\"";
    }
    s << "<" << qName << nsDecl;

    if ( !m_attr->map.isEmpty() ) {
	s << " ";
	TQDictIterator<TQDomNodePrivate> it( m_attr->map );
	for ( ; it.current(); ++it ) {
	    it.current()->save( s, 0, indent );
	    s << " ";
	}
    }

    if ( last ) {
	// has child nodes
	if ( first->isText() )
	    s << ">";
	else
	    s << ">" << endl;
	TQDomNodePrivate::save( s, depth + 1, indent);
	if ( !last->isText() )
	    for( int i = 0; i < depth*indent; ++i )
		s << " ";

	s << "</" << qName << ">";
    } else {
	s << "/>";
    }
    if ( !( next && next->isText() ) )
	s << endl;
}

/**************************************************************
 *
 * TQDomElement
 *
 **************************************************************/

#define IMPL ((TQDomElementPrivate*)impl)

/*!
    \class TQDomElement ntqdom.h
    \reentrant
    \brief The TQDomElement class represents one element in the DOM tree.
\if defined(commercial)
    It is part of the <a href="commercialeditions.html">TQt Enterprise Edition</a>.
\endif

    \module XML
    \ingroup xml-tools

    Elements have a tagName() and zero or more attributes associated
    with them. The tag name can be changed with setTagName().

    Element attributes are represented by TQDomAttr objects that can
    be queried using the attribute() and attributeNode() functions.
    You can set attributes with the setAttribute() and
    setAttributeNode() functions. Attributes can be removed with
    removeAttribute(). There are namespace-aware equivalents to these
    functions, i.e. setAttributeNS(), setAttributeNodeNS() and
    removeAttributeNS().

    If you want to access the text of a node use text(), e.g.
    \code
    TQDomElement e = //...
    //...
    TQString s = e.text()
    \endcode
    The text() function operates recursively to find the text (since
    not all elements contain text). If you want to find all the text
    in all of a node's children, iterate over the children looking for
    TQDomText nodes, e.g.
    \code
    TQString text;
    TQDomElement element = doc.documentElement();
    for( TQDomNode n = element.firstChild(); !n.isNull(); n = n.nextSibling() )
    {
	TQDomText t = n.toText();
	if ( !t.isNull() )
	    text += t.data();
    }
    \endcode
    Note that we attempt to convert each node to a text node and use
    text() rather than using firstChild().toText().data() or
    n.toText().data() directly on the node, because the node may not
    be a text element.

    You can get a list of all the decendents of an element which have
    a specified tag name with elementsByTagName() or
    elementsByTagNameNS().

    For further information about the Document Object Model see
    \link http://www.w3.org/TR/REC-DOM-Level-1/\endlink and
    \link http://www.w3.org/TR/DOM-Level-2-Core/\endlink.
    For a more general introduction of the DOM implementation see the
    TQDomDocument documentation.
*/

/*!
    Constructs an empty element. Use the TQDomDocument::createElement()
    function to construct elements with content.
*/
TQDomElement::TQDomElement()
    : TQDomNode()
{
}

/*!
    Constructs a copy of \a x.

    The data of the copy is shared (shallow copy): modifying one node
    will also change the other. If you want to make a deep copy, use
    cloneNode().
*/
TQDomElement::TQDomElement( const TQDomElement& x )
    : TQDomNode( x )
{
}

TQDomElement::TQDomElement( TQDomElementPrivate* n )
    : TQDomNode( n )
{
}

/*!
    Assigns \a x to this DOM element.

    The data of the copy is shared (shallow copy): modifying one node
    will also change the other. If you want to make a deep copy, use
    cloneNode().
*/
TQDomElement& TQDomElement::operator= ( const TQDomElement& x )
{
    return (TQDomElement&) TQDomNode::operator=( x );
}

/*!
    Destroys the object and frees its resources.
*/
TQDomElement::~TQDomElement()
{
}

/*!
    Returns \c ElementNode.
*/
TQDomNode::NodeType TQDomElement::nodeType() const
{
    return ElementNode;
}

/*!
    Sets this element's tag name to \a name.

    \sa tagName()
*/
void TQDomElement::setTagName( const TQString& name )
{
    if ( impl )
	impl->name = name;
}

/*!
    Returns the tag name of this element. For an XML element like this:
    \code
    <img src="myimg.png">
    \endcode
    the tagname would return "img".

    \sa setTagName()
*/
TQString TQDomElement::tagName() const
{
    if ( !impl )
	return TQString::null;
    return impl->nodeName();
}

/*!
    Returns the attribute called \a name. If the attribute does not
    exist \a defValue is returned.

    \sa setAttribute() attributeNode() setAttributeNode() attributeNS()
*/
TQString TQDomElement::attribute( const TQString& name,  const TQString& defValue ) const
{
    if ( !impl )
	return defValue;
    return IMPL->attribute( name, defValue );
}

/*!
    Adds an attribute called \a name with value \a value. If an
    attribute with the same name exists, its value is replaced by \a
    value.

    \sa attribute() setAttributeNode() setAttributeNS()
*/
void TQDomElement::setAttribute( const TQString& name, const TQString& value )
{
    if ( !impl )
	return;
    IMPL->setAttribute( name, value );
}

/*!
    \overload
*/
void TQDomElement::setAttribute( const TQString& name, int value )
{
    // ### 4.0: inline
    setAttribute( name, long(value) );
}

/*!
    \overload
*/
void TQDomElement::setAttribute( const TQString& name, uint value )
{
    // ### 4.0: inline
    setAttribute( name, ulong(value) );
}

/*!
    \overload
*/
void TQDomElement::setAttribute( const TQString& name, long value )
{
    if ( !impl )
	return;
    TQString x;
    x.setNum( value );
    IMPL->setAttribute( name, x );
}

/*!
    \overload
*/
void TQDomElement::setAttribute( const TQString& name, ulong value )
{
    if ( !impl )
	return;
    TQString x;
    x.setNum( value );
    IMPL->setAttribute( name, x );
}

/*!
    \overload
*/
void TQDomElement::setAttribute( const TQString& name, double value )
{
    if ( !impl )
	return;
    TQString x;
    x.setNum( value );
    IMPL->setAttribute( name, x );
}

/*!
    Removes the attribute called name \a name from this element.

    \sa setAttribute() attribute() removeAttributeNS()
*/
void TQDomElement::removeAttribute( const TQString& name )
{
    if ( !impl )
	return;
    IMPL->removeAttribute( name );
}

/*!
    Returns the TQDomAttr object that corresponds to the attribute
    called \a name. If no such attribute exists a \link
    TQDomNode::isNull() null attribute\endlink is returned.

    \sa setAttributeNode() attribute() setAttribute() attributeNodeNS()
*/
TQDomAttr TQDomElement::attributeNode( const TQString& name)
{
    if ( !impl )
	return TQDomAttr();
    return TQDomAttr( IMPL->attributeNode( name ) );
}

/*!
    Adds the attribute \a newAttr to this element.

    If the element has another attribute that has the same name as \a
    newAttr, this function replaces that attribute and returns it;
    otherwise the function returns a \link TQDomNode::isNull() null
    attribute\endlink.

    \sa attributeNode() setAttribute() setAttributeNodeNS()
*/
TQDomAttr TQDomElement::setAttributeNode( const TQDomAttr& newAttr )
{
    if ( !impl )
	return TQDomAttr();
    return TQDomAttr( IMPL->setAttributeNode( ((TQDomAttrPrivate*)newAttr.impl) ) );
}

/*!
    Removes the attribute \a oldAttr from the element and returns it.

    \sa attributeNode() setAttributeNode()
*/
TQDomAttr TQDomElement::removeAttributeNode( const TQDomAttr& oldAttr )
{
    if ( !impl )
	return TQDomAttr(); // ### should this return oldAttr?
    return TQDomAttr( IMPL->removeAttributeNode( ((TQDomAttrPrivate*)oldAttr.impl) ) );
}

/*!
    Returns a TQDomNodeList containing all descendent elements of this
    element that are called \a tagname. The order they are in the node
    list is the order they are encountered in a preorder traversal of
    the element tree.

    \sa elementsByTagNameNS() TQDomDocument::elementsByTagName()
*/
TQDomNodeList TQDomElement::elementsByTagName( const TQString& tagname ) const
{
    return TQDomNodeList( new TQDomNodeListPrivate( impl, tagname ) );
}

/*!
    Returns true.
*/
bool TQDomElement::isElement() const
{
    return true;
}

/*!
    Returns a TQDomNamedNodeMap containing all this element's attributes.

    \sa attribute() setAttribute() attributeNode() setAttributeNode()
*/
TQDomNamedNodeMap TQDomElement::attributes() const
{
    if ( !impl )
	return TQDomNamedNodeMap();
    return TQDomNamedNodeMap( IMPL->attributes() );
}

/*!
    Returns true if this element has an attribute called \a name;
    otherwise returns false.
*/
bool TQDomElement::hasAttribute( const TQString& name ) const
{
    if ( !impl )
	return false;
    return IMPL->hasAttribute( name );
}

/*!
    Returns the attribute with the local name \a localName and the
    namespace URI \a nsURI. If the attribute does not exist \a
    defValue is returned.

    \sa setAttributeNS() attributeNodeNS() setAttributeNodeNS() attribute()
*/
TQString TQDomElement::attributeNS( const TQString nsURI, const TQString& localName, const TQString& defValue ) const
{
    if ( !impl )
	return defValue;
    return IMPL->attributeNS( nsURI, localName, defValue );
}

/*!
    Adds an attribute with the qualified name \a qName and the
    namespace URI \a nsURI with the value \a value. If an attribute
    with the same local name and namespace URI exists, its prefix is
    replaced by the prefix of \a qName and its value is repaced by \a
    value.

    Although \a qName is the qualified name, the local name is used to
    decide if an existing attribute's value should be replaced.

    \sa attributeNS() setAttributeNodeNS() setAttribute()
*/
void TQDomElement::setAttributeNS( const TQString nsURI, const TQString& qName, const TQString& value )
{
    if ( !impl )
	return;
    IMPL->setAttributeNS( nsURI, qName, value );
}

/*!
    \overload
*/
void TQDomElement::setAttributeNS( const TQString nsURI, const TQString& qName, int value )
{
    // ### 4.0: inline
    setAttributeNS( nsURI, qName, long(value) );
}

/*!
    \overload
*/
void TQDomElement::setAttributeNS( const TQString nsURI, const TQString& qName, uint value )
{
    // ### 4.0: inline
    setAttributeNS( nsURI, qName, ulong(value) );
}

/*!
    \overload
*/
void TQDomElement::setAttributeNS( const TQString nsURI, const TQString& qName, long value )
{
    if ( !impl )
	return;
    TQString x;
    x.setNum( value );
    IMPL->setAttributeNS( nsURI, qName, x );
}

/*!
    \overload
*/
void TQDomElement::setAttributeNS( const TQString nsURI, const TQString& qName, ulong value )
{
    if ( !impl )
	return;
    TQString x;
    x.setNum( value );
    IMPL->setAttributeNS( nsURI, qName, x );
}

/*!
    \overload
*/
void TQDomElement::setAttributeNS( const TQString nsURI, const TQString& qName, double value )
{
    if ( !impl )
	return;
    TQString x;
    x.setNum( value );
    IMPL->setAttributeNS( nsURI, qName, x );
}

/*!
    Removes the attribute with the local name \a localName and the
    namespace URI \a nsURI from this element.

    \sa setAttributeNS() attributeNS() removeAttribute()
*/
void TQDomElement::removeAttributeNS( const TQString& nsURI, const TQString& localName )
{
    if ( !impl )
	return;
    TQDomNodePrivate *n = IMPL->attributeNodeNS( nsURI, localName );
    if ( !n )
	return;
    IMPL->removeAttribute( n->nodeName() );
}

/*!
    Returns the TQDomAttr object that corresponds to the attribute with
    the local name \a localName and the namespace URI \a nsURI. If no
    such attribute exists a \link TQDomNode::isNull() null
    attribute\endlink is returned.

    \sa setAttributeNode() attribute() setAttribute() attributeNodeNS()
*/
TQDomAttr TQDomElement::attributeNodeNS( const TQString& nsURI, const TQString& localName )
{
    if ( !impl )
	return TQDomAttr();
    return TQDomAttr( IMPL->attributeNodeNS( nsURI, localName ) );
}

/*!
    Adds the attribute \a newAttr to this element.

    If the element has another attribute that has the same local name
    and namespace URI as \a newAttr, this function replaces that
    attribute and returns it; otherwise the function returns a \link
    TQDomNode::isNull() null attribute\endlink.

    \sa attributeNodeNS() setAttributeNS() setAttributeNode()
*/
TQDomAttr TQDomElement::setAttributeNodeNS( const TQDomAttr& newAttr )
{
    if ( !impl )
	return TQDomAttr();
    return TQDomAttr( IMPL->setAttributeNodeNS( ((TQDomAttrPrivate*)newAttr.impl) ) );
}

/*!
    Returns a TQDomNodeList containing all the descendent elements of
    this element with the local name \a localName and the namespace
    URI \a nsURI. The order they are in the node list is the order
    they are encountered in a preorder traversal of the element tree.

    \sa elementsByTagName() TQDomDocument::elementsByTagNameNS()
*/
TQDomNodeList TQDomElement::elementsByTagNameNS( const TQString& nsURI, const TQString& localName ) const
{
    return TQDomNodeList( new TQDomNodeListPrivate( impl, nsURI, localName ) );
}

/*!
    Returns true if this element has an attribute with the local name
    \a localName and the namespace URI \a nsURI; otherwise returns
    false.
*/
bool TQDomElement::hasAttributeNS( const TQString& nsURI, const TQString& localName ) const
{
    if ( !impl )
	return false;
    return IMPL->hasAttributeNS( nsURI, localName );
}

/*!
    Returns the element's text or TQString::null.

    Example:
    \code
    <h1>Hello <b>TQt</b> <![CDATA[<xml is cool>]]></h1>
    \endcode

    The function text() of the TQDomElement for the &lt;h1&gt; tag,
    will return "Hello TQt &lt;xml is cool&gt;".

    Comments are ignored by this function. It only evaluates TQDomText
    and TQDomCDATASection objects.
*/
TQString TQDomElement::text() const
{
    if ( !impl )
	return TQString::null;
    return IMPL->text();
}

#undef IMPL

/**************************************************************
 *
 * TQDomTextPrivate
 *
 **************************************************************/

TQDomTextPrivate::TQDomTextPrivate( TQDomDocumentPrivate* d, TQDomNodePrivate* parent, const TQString& value )
    : TQDomCharacterDataPrivate( d, parent, value )
{
    name = "#text";
}

TQDomTextPrivate::TQDomTextPrivate( TQDomTextPrivate* n, bool deep )
    : TQDomCharacterDataPrivate( n, deep )
{
}

TQDomTextPrivate::~TQDomTextPrivate()
{
}

TQDomNodePrivate* TQDomTextPrivate::cloneNode( bool deep)
{
    TQDomNodePrivate* p = new TQDomTextPrivate( this, deep );
    // We are not interested in this node
    p->deref();
    return p;
}

TQDomTextPrivate* TQDomTextPrivate::splitText( int offset )
{
    if ( !parent() ) {
	tqWarning( "TQDomText::splitText  The node has no parent. So I can not split" );
	return 0;
    }

    TQDomTextPrivate* t = new TQDomTextPrivate( ownerDocument(), 0, value.mid( offset ) );
    value.truncate( offset );

    parent()->insertAfter( t, this );

    return t;
}

void TQDomTextPrivate::save( TQTextStream& s, int, int ) const
{
    s << encodeAttr( value );
}

/**************************************************************
 *
 * TQDomText
 *
 **************************************************************/

#define IMPL ((TQDomTextPrivate*)impl)

/*!
    \class TQDomText ntqdom.h
    \reentrant
    \brief The TQDomText class represents text data in the parsed XML document.
\if defined(commercial)
    It is part of the <a href="commercialeditions.html">TQt Enterprise Edition</a>.
\endif

    \module XML
    \ingroup xml-tools

    You can split the text in a TQDomText object over two TQDomText
    objecs with splitText().

    For further information about the Document Object Model see
    \link http://www.w3.org/TR/REC-DOM-Level-1/\endlink and
    \link http://www.w3.org/TR/DOM-Level-2-Core/\endlink.
    For a more general introduction of the DOM implementation see the
    TQDomDocument documentation.
*/

/*!
    Constructs an empty TQDomText object.

    To construct a TQDomText with content, use TQDomDocument::createTextNode().
*/
TQDomText::TQDomText()
    : TQDomCharacterData()
{
}

/*!
    Constructs a copy of \a x.

    The data of the copy is shared (shallow copy): modifying one node
    will also change the other. If you want to make a deep copy, use
    cloneNode().
*/
TQDomText::TQDomText( const TQDomText& x )
    : TQDomCharacterData( x )
{
}

TQDomText::TQDomText( TQDomTextPrivate* n )
    : TQDomCharacterData( n )
{
}

/*!
    Assigns \a x to this DOM text.

    The data of the copy is shared (shallow copy): modifying one node
    will also change the other. If you want to make a deep copy, use
    cloneNode().
*/
TQDomText& TQDomText::operator= ( const TQDomText& x )
{
    return (TQDomText&) TQDomNode::operator=( x );
}

/*!
    Destroys the object and frees its resources.
*/
TQDomText::~TQDomText()
{
}

/*!
    Returns \c TextNode.
*/
TQDomNode::NodeType TQDomText::nodeType() const
{
    return TextNode;
}

/*!
    Splits this DOM text object into two TQDomText objects. This object
    keeps its first \a offset characters and the second (newly
    created) object is inserted into the document tree after this
    object with the remaining characters.

    The function returns the newly created object.

    \sa TQDomNode::normalize()
*/
TQDomText TQDomText::splitText( int offset )
{
    if ( !impl )
	return TQDomText();
    return TQDomText( IMPL->splitText( offset ) );
}

/*!
    Returns true.
*/
bool TQDomText::isText() const
{
    return true;
}

#undef IMPL

/**************************************************************
 *
 * TQDomCommentPrivate
 *
 **************************************************************/

TQDomCommentPrivate::TQDomCommentPrivate( TQDomDocumentPrivate* d, TQDomNodePrivate* parent, const TQString& value )
    : TQDomCharacterDataPrivate( d, parent, value )
{
    name = "#comment";
}

TQDomCommentPrivate::TQDomCommentPrivate( TQDomCommentPrivate* n, bool deep )
    : TQDomCharacterDataPrivate( n, deep )
{
}

TQDomCommentPrivate::~TQDomCommentPrivate()
{
}

TQDomNodePrivate* TQDomCommentPrivate::cloneNode( bool deep)
{
    TQDomNodePrivate* p = new TQDomCommentPrivate( this, deep );
    // We are not interested in this node
    p->deref();
    return p;
}

void TQDomCommentPrivate::save( TQTextStream& s, int, int ) const
{
    s << "<!--" << value << "-->";
}

/**************************************************************
 *
 * TQDomComment
 *
 **************************************************************/

#define IMPL ((TQDomCommentPrivate*)impl)

/*!
    \class TQDomComment ntqdom.h
    \reentrant
    \brief The TQDomComment class represents an XML comment.
\if defined(commercial)
    It is part of the <a href="commercialeditions.html">TQt Enterprise Edition</a>.
\endif

    \module XML
    \ingroup xml-tools

    A comment in the parsed XML such as this:
    \code
    <!-- this is a comment -->
    \endcode
    is represented by TQDomComment objects in the parsed Dom tree.

    For further information about the Document Object Model see
    \link http://www.w3.org/TR/REC-DOM-Level-1/\endlink and
    \link http://www.w3.org/TR/DOM-Level-2-Core/\endlink.
    For a more general introduction of the DOM implementation see the
    TQDomDocument documentation.
*/

/*!
    Constructs an empty comment. To construct a comment with content,
    use the TQDomDocument::createComment() function.
*/
TQDomComment::TQDomComment()
    : TQDomCharacterData()
{
}

/*!
    Constructs a copy of \a x.

    The data of the copy is shared (shallow copy): modifying one node
    will also change the other. If you want to make a deep copy, use
    cloneNode().
*/
TQDomComment::TQDomComment( const TQDomComment& x )
    : TQDomCharacterData( x )
{
}

TQDomComment::TQDomComment( TQDomCommentPrivate* n )
    : TQDomCharacterData( n )
{
}

/*!
    Assigns \a x to this DOM comment.

    The data of the copy is shared (shallow copy): modifying one node
    will also change the other. If you want to make a deep copy, use
    cloneNode().
*/
TQDomComment& TQDomComment::operator= ( const TQDomComment& x )
{
    return (TQDomComment&) TQDomNode::operator=( x );
}

/*!
    Destroys the object and frees its resources.
*/
TQDomComment::~TQDomComment()
{
}

/*!
    Returns \c CommentNode.
*/
TQDomNode::NodeType TQDomComment::nodeType() const
{
    return CommentNode;
}

/*!
    Returns true.
*/
bool TQDomComment::isComment() const
{
    return true;
}

#undef IMPL

/**************************************************************
 *
 * TQDomCDATASectionPrivate
 *
 **************************************************************/

TQDomCDATASectionPrivate::TQDomCDATASectionPrivate( TQDomDocumentPrivate* d, TQDomNodePrivate* parent,
						    const TQString& value )
    : TQDomTextPrivate( d, parent, value )
{
    name = "#cdata-section";
}

TQDomCDATASectionPrivate::TQDomCDATASectionPrivate( TQDomCDATASectionPrivate* n, bool deep )
    : TQDomTextPrivate( n, deep )
{
}

TQDomCDATASectionPrivate::~TQDomCDATASectionPrivate()
{
}

TQDomNodePrivate* TQDomCDATASectionPrivate::cloneNode( bool deep)
{
    TQDomNodePrivate* p = new TQDomCDATASectionPrivate( this, deep );
    // We are not interested in this node
    p->deref();
    return p;
}

void TQDomCDATASectionPrivate::save( TQTextStream& s, int, int ) const
{
    // ### How do we escape "]]>" ?
    // "]]>" is not allowed; so there should be none in value anyway
    s << "<![CDATA[" << value << "]]>";
}

/**************************************************************
 *
 * TQDomCDATASection
 *
 **************************************************************/

#define IMPL ((TQDomCDATASectionPrivate*)impl)

/*!
    \class TQDomCDATASection ntqdom.h
    \reentrant
    \brief The TQDomCDATASection class represents an XML CDATA section.
\if defined(commercial)
    It is part of the <a href="commercialeditions.html">TQt Enterprise Edition</a>.
\endif

    \module XML
    \ingroup xml-tools

    CDATA sections are used to escape blocks of text containing
    characters that would otherwise be regarded as markup. The only
    delimiter that is recognized in a CDATA section is the "]]&gt;"
    string that terminates the CDATA section. CDATA sections cannot be
    nested. Their primary purpose is for including material such as
    XML fragments, without needing to escape all the delimiters.

    Adjacent TQDomCDATASection nodes are not merged by the
    TQDomNode::normalize() function.

    For further information about the Document Object Model see
    \link http://www.w3.org/TR/REC-DOM-Level-1/\endlink and
    \link http://www.w3.org/TR/DOM-Level-2-Core/\endlink.
    For a more general introduction of the DOM implementation see the
    TQDomDocument documentation.
*/

/*!
    Constructs an empty CDATA section. To create a CDATA section with
    content, use the TQDomDocument::createCDATASection() function.
*/
TQDomCDATASection::TQDomCDATASection()
    : TQDomText()
{
}

/*!
    Constructs a copy of \a x.

    The data of the copy is shared (shallow copy): modifying one node
    will also change the other. If you want to make a deep copy, use
    cloneNode().
*/
TQDomCDATASection::TQDomCDATASection( const TQDomCDATASection& x )
    : TQDomText( x )
{
}

TQDomCDATASection::TQDomCDATASection( TQDomCDATASectionPrivate* n )
    : TQDomText( n )
{
}

/*!
    Assigns \a x to this CDATA section.

    The data of the copy is shared (shallow copy): modifying one node
    will also change the other. If you want to make a deep copy, use
    cloneNode().
*/
TQDomCDATASection& TQDomCDATASection::operator= ( const TQDomCDATASection& x )
{
    return (TQDomCDATASection&) TQDomNode::operator=( x );
}

/*!
    Destroys the object and frees its resources.
*/
TQDomCDATASection::~TQDomCDATASection()
{
}

/*!
    Returns \c CDATASection.
*/
TQDomNode::NodeType TQDomCDATASection::nodeType() const
{
    return CDATASectionNode;
}

/*!
    Returns true.
*/
bool TQDomCDATASection::isCDATASection() const
{
    return true;
}

#undef IMPL

/**************************************************************
 *
 * TQDomNotationPrivate
 *
 **************************************************************/

TQDomNotationPrivate::TQDomNotationPrivate( TQDomDocumentPrivate* d, TQDomNodePrivate* parent,
					    const TQString& aname,
					    const TQString& pub, const TQString& sys )
    : TQDomNodePrivate( d, parent )
{
    name = aname;
    m_pub = pub;
    m_sys = sys;
}

TQDomNotationPrivate::TQDomNotationPrivate( TQDomNotationPrivate* n, bool deep )
    : TQDomNodePrivate( n, deep )
{
    m_sys = n->m_sys;
    m_pub = n->m_pub;
}

TQDomNotationPrivate::~TQDomNotationPrivate()
{
}

TQDomNodePrivate* TQDomNotationPrivate::cloneNode( bool deep)
{
    TQDomNodePrivate* p = new TQDomNotationPrivate( this, deep );
    // We are not interested in this node
    p->deref();
    return p;
}

void TQDomNotationPrivate::save( TQTextStream& s, int, int ) const
{
    s << "<!NOTATION " << name << " ";
    if ( !m_pub.isNull() )  {
	s << "PUBLIC \"" << m_pub << "\"";
	if ( !m_sys.isNull() )
	    s << " \"" << m_sys << "\"";
    }  else {
	s << "SYSTEM \"" << m_sys << "\"";
    }
    s << ">" << endl;
}

/**************************************************************
 *
 * TQDomNotation
 *
 **************************************************************/

#define IMPL ((TQDomNotationPrivate*)impl)

/*!
    \class TQDomNotation ntqdom.h
    \reentrant
    \brief The TQDomNotation class represents an XML notation.
\if defined(commercial)
    It is part of the <a href="commercialeditions.html">TQt Enterprise Edition</a>.
\endif

    \module XML
    \ingroup xml-tools

    A notation either declares, by name, the format of an unparsed
    entity (see section 4.7 of the XML 1.0 specification), or is used
    for formal declaration of processing instruction targets (see
    section 2.6 of the XML 1.0 specification).

    DOM does not support editing notation nodes; they are therefore
    read-only.

    A notation node does not have any parent.

    You can retrieve the publicId() and systemId() from a notation
    node.

    For further information about the Document Object Model see
    \link http://www.w3.org/TR/REC-DOM-Level-1/\endlink and
    \link http://www.w3.org/TR/DOM-Level-2-Core/\endlink.
    For a more general introduction of the DOM implementation see the
    TQDomDocument documentation.
*/


/*!
    Constructor.
*/
TQDomNotation::TQDomNotation()
    : TQDomNode()
{
}

/*!
    Constructs a copy of \a x.

    The data of the copy is shared (shallow copy): modifying one node
    will also change the other. If you want to make a deep copy, use
    cloneNode().
*/
TQDomNotation::TQDomNotation( const TQDomNotation& x )
    : TQDomNode( x )
{
}

TQDomNotation::TQDomNotation( TQDomNotationPrivate* n )
    : TQDomNode( n )
{
}

/*!
    Assigns \a x to this DOM notation.

    The data of the copy is shared (shallow copy): modifying one node
    will also change the other. If you want to make a deep copy, use
    cloneNode().
*/
TQDomNotation& TQDomNotation::operator= ( const TQDomNotation& x )
{
    return (TQDomNotation&) TQDomNode::operator=( x );
}

/*!
    Destroys the object and frees its resources.
*/
TQDomNotation::~TQDomNotation()
{
}

/*!
    Returns \c NotationNode.
*/
TQDomNode::NodeType TQDomNotation::nodeType() const
{
    return NotationNode;
}

/*!
    Returns the public identifier of this notation.
*/
TQString TQDomNotation::publicId() const
{
    if ( !impl )
	return TQString::null;
    return IMPL->m_pub;
}

/*!
    Returns the system identifier of this notation.
*/
TQString TQDomNotation::systemId() const
{
    if ( !impl )
	return TQString::null;
    return IMPL->m_sys;
}

/*!
    Returns true.
*/
bool TQDomNotation::isNotation() const
{
    return true;
}

#undef IMPL

/**************************************************************
 *
 * TQDomEntityPrivate
 *
 **************************************************************/

TQDomEntityPrivate::TQDomEntityPrivate( TQDomDocumentPrivate* d, TQDomNodePrivate* parent,
					const TQString& aname,
					const TQString& pub, const TQString& sys, const TQString& notation )
    : TQDomNodePrivate( d, parent )
{
    name = aname;
    m_pub = pub;
    m_sys = sys;
    m_notationName = notation;
}

TQDomEntityPrivate::TQDomEntityPrivate( TQDomEntityPrivate* n, bool deep )
    : TQDomNodePrivate( n, deep )
{
    m_sys = n->m_sys;
    m_pub = n->m_pub;
    m_notationName = n->m_notationName;
}

TQDomEntityPrivate::~TQDomEntityPrivate()
{
}

TQDomNodePrivate* TQDomEntityPrivate::cloneNode( bool deep)
{
    TQDomNodePrivate* p = new TQDomEntityPrivate( this, deep );
    // We are not interested in this node
    p->deref();
    return p;
}

/*
  Encode an entity value upon saving.
*/
static TQCString encodeEntity( const TQCString& str )
{
    TQCString tmp( str );
    uint len = tmp.length();
    uint i = 0;
    const char* d = tmp.data();
    while ( i < len ) {
	if ( d[i] == '%' ){
	    tmp.replace( i, 1, "&#60;" );
	    d = tmp.data();
	    len += 4;
	    i += 5;
	}
	else if ( d[i] == '"' ) {
	    tmp.replace( i, 1, "&#34;" );
	    d = tmp.data();
	    len += 4;
	    i += 5;
	} else if ( d[i] == '&' && i + 1 < len && d[i+1] == '#' ) {
	    // Dont encode &lt; or &quot; or &custom;.
	    // Only encode character references
	    tmp.replace( i, 1, "&#38;" );
	    d = tmp.data();
	    len += 4;
	    i += 5;
	} else {
	    ++i;
	}
    }

    return tmp;
}

void TQDomEntityPrivate::save( TQTextStream& s, int, int ) const
{
    if ( m_sys.isNull() && m_pub.isNull() ) {
	s << "<!ENTITY " << name << " \"" << encodeEntity( value.utf8() ) << "\">" << endl;
    } else {
	s << "<!ENTITY " << name << " ";
	if ( m_pub.isNull() ) {
	    s << "SYSTEM \"" << m_sys << "\"";
	} else {
	    s << "PUBLIC \"" << m_pub << "\" \"" << m_sys << "\"";
	}
	if (! m_notationName.isNull() ) {
	    s << " NDATA " << m_notationName;
	}
	s << ">" << endl;
    }
}

/**************************************************************
 *
 * TQDomEntity
 *
 **************************************************************/

#define IMPL ((TQDomEntityPrivate*)impl)

/*!
    \class TQDomEntity ntqdom.h
    \reentrant
    \brief The TQDomEntity class represents an XML entity.
\if defined(commercial)
    It is part of the <a href="commercialeditions.html">TQt Enterprise Edition</a>.
\endif

    \module XML
    \ingroup xml-tools

    This class represents an entity in an XML document, either parsed
    or unparsed. Note that this models the entity itself not the
    entity declaration.

    DOM does not support editing entity nodes; if a user wants to make
    changes to the contents of an entity, every related
    TQDomEntityReference node must be replaced in the DOM tree by a
    clone of the entity's contents, and then the desired changes must
    be made to each of the clones instead. All the descendents of an
    entity node are read-only.

    An entity node does not have any parent.

    You can access the entity's publicId(), systemId() and
    notationName() when available.

    For further information about the Document Object Model see
    \link http://www.w3.org/TR/REC-DOM-Level-1/\endlink and
    \link http://www.w3.org/TR/DOM-Level-2-Core/\endlink.
    For a more general introduction of the DOM implementation see the
    TQDomDocument documentation.
*/


/*!
    Constructs an empty entity.
*/
TQDomEntity::TQDomEntity()
    : TQDomNode()
{
}


/*!
    Constructs a copy of \a x.

    The data of the copy is shared (shallow copy): modifying one node
    will also change the other. If you want to make a deep copy, use
    cloneNode().
*/
TQDomEntity::TQDomEntity( const TQDomEntity& x )
    : TQDomNode( x )
{
}

TQDomEntity::TQDomEntity( TQDomEntityPrivate* n )
    : TQDomNode( n )
{
}

/*!
    Assigns \a x to this DOM entity.

    The data of the copy is shared (shallow copy): modifying one node
    will also change the other. If you want to make a deep copy, use
    cloneNode().
*/
TQDomEntity& TQDomEntity::operator= ( const TQDomEntity& x )
{
    return (TQDomEntity&) TQDomNode::operator=( x );
}

/*!
    Destroys the object and frees its resources.
*/
TQDomEntity::~TQDomEntity()
{
}

/*!
    Returns \c EntityNode.
*/
TQDomNode::NodeType TQDomEntity::nodeType() const
{
    return EntityNode;
}

/*!
    Returns the public identifier associated with this entity. If the
    public identifier was not specified TQString::null is returned.
*/
TQString TQDomEntity::publicId() const
{
    if ( !impl )
	return TQString::null;
    return IMPL->m_pub;
}

/*!
    Returns the system identifier associated with this entity. If the
    system identifier was not specified TQString::null is returned.
*/
TQString TQDomEntity::systemId() const
{
    if ( !impl )
	return TQString::null;
    return IMPL->m_sys;
}

/*!
    For unparsed entities this function returns the name of the
    notation for the entity. For parsed entities this function returns
    TQString::null.
*/
TQString TQDomEntity::notationName() const
{
    if ( !impl )
	return TQString::null;
    return IMPL->m_notationName;
}

/*!
    Returns true.
*/
bool TQDomEntity::isEntity() const
{
    return true;
}

#undef IMPL

/**************************************************************
 *
 * TQDomEntityReferencePrivate
 *
 **************************************************************/

TQDomEntityReferencePrivate::TQDomEntityReferencePrivate( TQDomDocumentPrivate* d, TQDomNodePrivate* parent, const TQString& aname )
    : TQDomNodePrivate( d, parent )
{
    name = aname;
}

TQDomEntityReferencePrivate::TQDomEntityReferencePrivate( TQDomNodePrivate* n, bool deep )
    : TQDomNodePrivate( n, deep )
{
}

TQDomEntityReferencePrivate::~TQDomEntityReferencePrivate()
{
}

TQDomNodePrivate* TQDomEntityReferencePrivate::cloneNode( bool deep)
{
    TQDomNodePrivate* p = new TQDomEntityReferencePrivate( this, deep );
    // We are not interested in this node
    p->deref();
    return p;
}

void TQDomEntityReferencePrivate::save( TQTextStream& s, int, int ) const
{
    s << "&" << name << ";";
}

/**************************************************************
 *
 * TQDomEntityReference
 *
 **************************************************************/

#define IMPL ((TQDomEntityReferencePrivate*)impl)

/*!
    \class TQDomEntityReference ntqdom.h
    \reentrant
    \brief The TQDomEntityReference class represents an XML entity reference.
\if defined(commercial)
    It is part of the <a href="commercialeditions.html">TQt Enterprise Edition</a>.
\endif

    \module XML
    \ingroup xml-tools

    A TQDomEntityReference object may be inserted into the DOM tree
    when an entity reference is in the source document, or when the
    user wishes to insert an entity reference.

    Note that character references and references to predefined
    entities are expanded by the XML processor so that characters are
    represented by their Unicode equivalent rather than by an entity
    reference.

    Moreover, the XML processor may completely expand references to
    entities while building the DOM tree, instead of providing
    TQDomEntityReference objects.

    If it does provide such objects, then for a given entity reference
    node, it may be that there is no entity node representing the
    referenced entity; but if such an entity exists, then the child
    list of the entity reference node is the same as that of the
    entity  node. As with the entity node, all descendents of the
    entity reference are read-only.

    For further information about the Document Object Model see
    \link http://www.w3.org/TR/REC-DOM-Level-1/\endlink and
    \link http://www.w3.org/TR/DOM-Level-2-Core/\endlink.
    For a more general introduction of the DOM implementation see the
    TQDomDocument documentation.
*/

/*!
    Constructs an empty entity reference. Use
    TQDomDocument::createEntityReference() to create a entity reference
    with content.
*/
TQDomEntityReference::TQDomEntityReference()
    : TQDomNode()
{
}

/*!
    Constructs a copy of \a x.

    The data of the copy is shared (shallow copy): modifying one node
    will also change the other. If you want to make a deep copy, use
    cloneNode().
*/
TQDomEntityReference::TQDomEntityReference( const TQDomEntityReference& x )
    : TQDomNode( x )
{
}

TQDomEntityReference::TQDomEntityReference( TQDomEntityReferencePrivate* n )
    : TQDomNode( n )
{
}

/*!
    Assigns \a x to this entity reference.

    The data of the copy is shared (shallow copy): modifying one node
    will also change the other. If you want to make a deep copy, use
    cloneNode().
*/
TQDomEntityReference& TQDomEntityReference::operator= ( const TQDomEntityReference& x )
{
    return (TQDomEntityReference&) TQDomNode::operator=( x );
}

/*!
    Destroys the object and frees its resources.
*/
TQDomEntityReference::~TQDomEntityReference()
{
}

/*!
    Returns \c EntityReference.
*/
TQDomNode::NodeType TQDomEntityReference::nodeType() const
{
    return EntityReferenceNode;
}

/*!
    Returns true.
*/
bool TQDomEntityReference::isEntityReference() const
{
    return true;
}

#undef IMPL

/**************************************************************
 *
 * TQDomProcessingInstructionPrivate
 *
 **************************************************************/

TQDomProcessingInstructionPrivate::TQDomProcessingInstructionPrivate( TQDomDocumentPrivate* d,
	TQDomNodePrivate* parent, const TQString& target, const TQString& data )
    : TQDomNodePrivate( d, parent )
{
    name = target;
    value = data;
}

TQDomProcessingInstructionPrivate::TQDomProcessingInstructionPrivate( TQDomProcessingInstructionPrivate* n, bool deep )
    : TQDomNodePrivate( n, deep )
{
}

TQDomProcessingInstructionPrivate::~TQDomProcessingInstructionPrivate()
{
}

TQDomNodePrivate* TQDomProcessingInstructionPrivate::cloneNode( bool deep)
{
    TQDomNodePrivate* p = new TQDomProcessingInstructionPrivate( this, deep );
    // We are not interested in this node
    p->deref();
    return p;
}

void TQDomProcessingInstructionPrivate::save( TQTextStream& s, int, int ) const
{
    s << "<?" << name << " " << value << "?>" << endl;
}

/**************************************************************
 *
 * TQDomProcessingInstruction
 *
 **************************************************************/

#define IMPL ((TQDomProcessingInstructionPrivate*)impl)

/*!
    \class TQDomProcessingInstruction ntqdom.h
    \reentrant
    \brief The TQDomProcessingInstruction class represents an XML processing
    instruction.
\if defined(commercial)
    It is part of the <a href="commercialeditions.html">TQt Enterprise Edition</a>.
\endif

    \module XML
    \ingroup xml-tools

    Processing instructions are used in XML to keep processor-specific
    information in the text of the document.

    The content of the processing instruction is retrieved with data()
    and set with setData(). The processing instruction's target is
    retrieved with target().

    For further information about the Document Object Model see
    \link http://www.w3.org/TR/REC-DOM-Level-1/\endlink and
    \link http://www.w3.org/TR/DOM-Level-2-Core/\endlink.
    For a more general introduction of the DOM implementation see the
    TQDomDocument documentation.
*/

/*!
    Constructs an empty processing instruction. Use
    TQDomDocument::createProcessingInstruction() to create a processing
    instruction with content.
*/
TQDomProcessingInstruction::TQDomProcessingInstruction()
    : TQDomNode()
{
}

/*!
    Constructs a copy of \a x.

    The data of the copy is shared (shallow copy): modifying one node
    will also change the other. If you want to make a deep copy, use
    cloneNode().
*/
TQDomProcessingInstruction::TQDomProcessingInstruction( const TQDomProcessingInstruction& x )
    : TQDomNode( x )
{
}

TQDomProcessingInstruction::TQDomProcessingInstruction( TQDomProcessingInstructionPrivate* n )
    : TQDomNode( n )
{
}

/*!
    Assigns \a x to this processing instruction.

    The data of the copy is shared (shallow copy): modifying one node
    will also change the other. If you want to make a deep copy, use
    cloneNode().
*/
TQDomProcessingInstruction& TQDomProcessingInstruction::operator= ( const TQDomProcessingInstruction& x )
{
    return (TQDomProcessingInstruction&) TQDomNode::operator=( x );
}

/*!
    Destroys the object and frees its resources.
*/
TQDomProcessingInstruction::~TQDomProcessingInstruction()
{
}

/*!
    Returns \c ProcessingInstructionNode.
*/
TQDomNode::NodeType TQDomProcessingInstruction::nodeType() const
{
    return ProcessingInstructionNode;
}

/*!
    Returns the target of this processing instruction.

    \sa data()
*/
TQString TQDomProcessingInstruction::target() const
{
    if ( !impl )
	return TQString::null;
    return impl->nodeName();
}

/*!
    Returns the content of this processing instruction.

    \sa setData() target()
*/
TQString TQDomProcessingInstruction::data() const
{
    if ( !impl )
	return TQString::null;
    return impl->nodeValue();
}

/*!
    Sets the data contained in the processing instruction to \a d.

    \sa data()
*/
void TQDomProcessingInstruction::setData( const TQString& d )
{
    if ( !impl )
	return;
    impl->setNodeValue( d );
}

/*!
    Returns true.
*/
bool TQDomProcessingInstruction::isProcessingInstruction() const
{
    return true;
}

#undef IMPL

/**************************************************************
 *
 * TQDomDocumentPrivate
 *
 **************************************************************/

TQDomDocumentPrivate::TQDomDocumentPrivate()
    : TQDomNodePrivate( 0 )
{
    impl = new TQDomImplementationPrivate();
    type = new TQDomDocumentTypePrivate( this, this );

    name = "#document";
}

TQDomDocumentPrivate::TQDomDocumentPrivate( const TQString& aname )
    : TQDomNodePrivate( 0 )
{
    impl = new TQDomImplementationPrivate();
    type = new TQDomDocumentTypePrivate( this, this );
    type->name = aname;

    name = "#document";
}

TQDomDocumentPrivate::TQDomDocumentPrivate( TQDomDocumentTypePrivate* dt )
    : TQDomNodePrivate( 0 )
{
    impl = new TQDomImplementationPrivate();
    if ( dt != 0 ) {
	type = dt;
	type->ref();
    } else {
	type = new TQDomDocumentTypePrivate( this, this );
    }

    name = "#document";
}

TQDomDocumentPrivate::TQDomDocumentPrivate( TQDomDocumentPrivate* n, bool deep )
    : TQDomNodePrivate( n, deep )
{
    impl = n->impl->clone();
    // Reference count is down to 0, so we set it to 1 here.
    impl->ref();
    type = (TQDomDocumentTypePrivate*)n->type->cloneNode();
    type->setParent( this );
    // Reference count is down to 0, so we set it to 1 here.
    type->ref();
}

TQDomDocumentPrivate::~TQDomDocumentPrivate()
{
    if ( impl->deref() ) delete impl;
    if ( type->deref() ) delete type;
}

void TQDomDocumentPrivate::clear()
{
    if ( impl->deref() ) delete impl;
    if ( type->deref() ) delete type;
    impl = 0;
    type = 0;
    TQDomNodePrivate::clear();
}

bool TQDomDocumentPrivate::setContent( TQXmlInputSource *source, bool namespaceProcessing, TQString *errorMsg, int *errorLine, int *errorColumn )
{
    TQXmlSimpleReader reader;
    if ( namespaceProcessing ) {
	reader.setFeature( "http://xml.org/sax/features/namespaces", true );
	reader.setFeature( "http://xml.org/sax/features/namespace-prefixes", false );
    } else {
	reader.setFeature( "http://xml.org/sax/features/namespaces", false );
	reader.setFeature( "http://xml.org/sax/features/namespace-prefixes", true );
    }
    reader.setFeature( "http://trolltech.com/xml/features/report-whitespace-only-CharData", false );
    reader.setUndefEntityInAttrHack( true );

    return setContent( source, &reader, errorMsg, errorLine, errorColumn );
}

bool TQDomDocumentPrivate::setContent( TQXmlInputSource *source, TQXmlReader *reader, TQString *errorMsg, int *errorLine, int *errorColumn )
{
    clear();
    impl = new TQDomImplementationPrivate;
    type = new TQDomDocumentTypePrivate( this, this );

    bool namespaceProcessing = reader->feature( "http://xml.org/sax/features/namespaces" )
	&& !reader->feature( "http://xml.org/sax/features/namespace-prefixes" );

    TQDomHandler hnd( this, namespaceProcessing );
    reader->setContentHandler( &hnd );
    reader->setErrorHandler( &hnd );
    reader->setLexicalHandler( &hnd );
    reader->setDeclHandler( &hnd );
    reader->setDTDHandler( &hnd );

    if ( !reader->parse( source ) ) {
	if ( errorMsg )
	    *errorMsg = hnd.errorMsg;
	if ( errorLine )
	    *errorLine = hnd.errorLine;
	if ( errorColumn )
	    *errorColumn = hnd.errorColumn;
	return false;
    }

    return true;
}

TQDomNodePrivate* TQDomDocumentPrivate::cloneNode( bool deep)
{
    TQDomNodePrivate* p = new TQDomDocumentPrivate( this, deep );
    // We are not interested in this node
    p->deref();
    return p;
}

TQDomElementPrivate* TQDomDocumentPrivate::documentElement()
{
    TQDomNodePrivate* p = first;
    while ( p && !p->isElement() )
	p = p->next;

    return (TQDomElementPrivate*)p;
}

TQDomElementPrivate* TQDomDocumentPrivate::createElement( const TQString& tagName )
{
    TQDomElementPrivate* e = new TQDomElementPrivate( this, 0, tagName );
    e->deref();
    return e;
}

TQDomElementPrivate* TQDomDocumentPrivate::createElementNS( const TQString& nsURI, const TQString& qName )
{
    TQDomElementPrivate* e = new TQDomElementPrivate( this, 0, nsURI, qName );
    e->deref();
    return e;
}

TQDomDocumentFragmentPrivate* TQDomDocumentPrivate::createDocumentFragment()
{
    TQDomDocumentFragmentPrivate* f = new TQDomDocumentFragmentPrivate( this, (TQDomNodePrivate*)0 );
    f->deref();
    return f;
}

TQDomTextPrivate* TQDomDocumentPrivate::createTextNode( const TQString& data )
{
    TQDomTextPrivate* t = new TQDomTextPrivate( this, 0, data );
    t->deref();
    return t;
}

TQDomCommentPrivate* TQDomDocumentPrivate::createComment( const TQString& data )
{
    TQDomCommentPrivate* c = new TQDomCommentPrivate( this, 0, data );
    c->deref();
    return c;
}

TQDomCDATASectionPrivate* TQDomDocumentPrivate::createCDATASection( const TQString& data )
{
    TQDomCDATASectionPrivate* c = new TQDomCDATASectionPrivate( this, 0, data );
    c->deref();
    return c;
}

TQDomProcessingInstructionPrivate* TQDomDocumentPrivate::createProcessingInstruction( const TQString& target, const TQString& data )
{
    TQDomProcessingInstructionPrivate* p = new TQDomProcessingInstructionPrivate( this, 0, target, data );
    p->deref();
    return p;
}

TQDomAttrPrivate* TQDomDocumentPrivate::createAttribute( const TQString& aname )
{
    TQDomAttrPrivate* a = new TQDomAttrPrivate( this, 0, aname );
    a->deref();
    return a;
}

TQDomAttrPrivate* TQDomDocumentPrivate::createAttributeNS( const TQString& nsURI, const TQString& qName )
{
    TQDomAttrPrivate* a = new TQDomAttrPrivate( this, 0, nsURI, qName );
    a->deref();
    return a;
}

TQDomEntityReferencePrivate* TQDomDocumentPrivate::createEntityReference( const TQString& aname )
{
    TQDomEntityReferencePrivate* e = new TQDomEntityReferencePrivate( this, 0, aname );
    e->deref();
    return e;
}

TQDomNodePrivate* TQDomDocumentPrivate::importNode( const TQDomNodePrivate* importedNode, bool deep )
{
    TQDomNodePrivate *node = 0;
    switch ( importedNode->nodeType() ) {
	case TQDomNode::AttributeNode:
	    node = new TQDomAttrPrivate( (TQDomAttrPrivate*)importedNode, true );
	    break;
	case TQDomNode::DocumentFragmentNode:
	    node = new TQDomDocumentFragmentPrivate( (TQDomDocumentFragmentPrivate*)importedNode, deep );
	    break;
	case TQDomNode::ElementNode:
	    node = new TQDomElementPrivate( (TQDomElementPrivate*)importedNode, deep );
	    break;
	case TQDomNode::EntityNode:
	    node = new TQDomEntityPrivate( (TQDomEntityPrivate*)importedNode, deep );
	    break;
	case TQDomNode::EntityReferenceNode:
	    node = new TQDomEntityReferencePrivate( (TQDomEntityReferencePrivate*)importedNode, false );
	    break;
	case TQDomNode::NotationNode:
	    node = new TQDomNotationPrivate( (TQDomNotationPrivate*)importedNode, deep );
	    break;
	case TQDomNode::ProcessingInstructionNode:
	    node = new TQDomProcessingInstructionPrivate( (TQDomProcessingInstructionPrivate*)importedNode, deep );
	    break;
	case TQDomNode::TextNode:
	    node = new TQDomTextPrivate( (TQDomTextPrivate*)importedNode, deep );
	    break;
	case TQDomNode::CDATASectionNode:
	    node = new TQDomCDATASectionPrivate( (TQDomCDATASectionPrivate*)importedNode, deep );
	    break;
	case TQDomNode::CommentNode:
	    node = new TQDomCommentPrivate( (TQDomCommentPrivate*)importedNode, deep );
	    break;
	default:
	    break;
    }
    if ( node ) {
	node->setOwnerDocument( this );
	// The TQDomNode constructor increases the refcount, so deref() first to
	// keep refcount balanced.
	node->deref();
    }
    return node;
}

void TQDomDocumentPrivate::save( TQTextStream& s, int, int indent ) const
{
    bool doc = false;

    TQDomNodePrivate* n = first;
    if ( n && n->isProcessingInstruction() && n->nodeName()=="xml" ) {
	// we have an XML declaration
	TQString data = n->nodeValue();
	TQRegExp encoding( TQString::fromLatin1("encoding\\s*=\\s*((\"([^\"]*)\")|('([^']*)'))") );
	encoding.search( data );
	TQString enc = encoding.cap(3);
	if ( enc.isEmpty() ) {
	    enc = encoding.cap(5);
	}
	if ( enc.isEmpty() ) {
	    s.setEncoding( TQTextStream::UnicodeUTF8 );
	} else {
	    s.setCodec( TQTextCodec::codecForName( enc ) );
	}
    } else {
	s.setEncoding( TQTextStream::UnicodeUTF8 );
    }
    while ( n ) {
	if ( !doc && !(n->isProcessingInstruction()&&n->nodeName()=="xml") ) {
	    // save doctype after XML declaration
	    type->save( s, 0, indent );
	    doc = true;
	}
	n->save( s, 0, indent );
	n = n->next;
    }
}

/**************************************************************
 *
 * TQDomDocument
 *
 **************************************************************/

#define IMPL ((TQDomDocumentPrivate*)impl)

/*!
    \class TQDomDocument ntqdom.h
    \reentrant
    \brief The TQDomDocument class represents an XML document.
\if defined(commercial)
    It is part of the <a href="commercialeditions.html">TQt Enterprise Edition</a>.
\endif

    \module XML
    \ingroup xml-tools

    The TQDomDocument class represents the entire XML document.
    Conceptually, it is the root of the document tree, and provides
    the primary access to the document's data.

    Since elements, text nodes, comments, processing instructions,
    etc., cannot exist outside the context of a document, the document
    class also contains the factory functions needed to create these
    objects. The node objects created have an ownerDocument() function
    which associates them with the document within whose context they
    were created. The DOM classes that will be used most often are
    TQDomNode, TQDomDocument, TQDomElement and TQDomText.

    The parsed XML is represented internally by a tree of objects that
    can be accessed using the various TQDom classes. All TQDom classes
    only \e reference objects in the internal tree. The internal
    objects in the DOM tree will get deleted once the last TQDom
    object referencing them and the TQDomDocument itself are deleted.

    Creation of elements, text nodes, etc. is done using the various
    factory functions provided in this class. Using the default
    constructors of the TQDom classes will only result in empty
    objects that cannot be manipulated or inserted into the Document.

    The TQDomDocument class has several functions for creating document
    data, for example, createElement(), createTextNode(),
    createComment(), createCDATASection(),
    createProcessingInstruction(), createAttribute() and
    createEntityReference(). Some of these functions have versions
    that support namespaces, i.e. createElementNS() and
    createAttributeNS(). The createDocumentFragment() function is used
    to hold parts of the document; this is useful for manipulating for
    complex documents.

    The entire content of the document is set with setContent(). This
    function parses the string it is passed as an XML document and
    creates the DOM tree that represents the document. The root
    element is available using documentElement(). The textual
    representation of the document can be obtained using toString().

    It is possible to insert a node from another document into the
    document using importNode().

    You can obtain a list of all the elements that have a particular
    tag with elementsByTagName() or with elementsByTagNameNS().

    The TQDom classes are typically used as follows:
    \code
    TQDomDocument doc( "mydocument" );
    TQFile file( "mydocument.xml" );
    if ( !file.open( IO_ReadOnly ) )
	return;
    if ( !doc.setContent( &file ) ) {
	file.close();
	return;
    }
    file.close();

    // print out the element names of all elements that are direct children
    // of the outermost element.
    TQDomElement docElem = doc.documentElement();

    TQDomNode n = docElem.firstChild();
    while( !n.isNull() ) {
	TQDomElement e = n.toElement(); // try to convert the node to an element.
	if( !e.isNull() ) {
	    cout << e.tagName() << endl; // the node really is an element.
	}
	n = n.nextSibling();
    }

    // Here we append a new element to the end of the document
    TQDomElement elem = doc.createElement( "img" );
    elem.setAttribute( "src", "myimage.png" );
    docElem.appendChild( elem );
    \endcode

    Once \c doc and \c elem go out of scope, the whole internal tree
    representing the XML document is deleted.

    To create a document using DOM use code like this:
    \code
    TQDomDocument doc( "MyML" );
    TQDomElement root = doc.createElement( "MyML" );
    doc.appendChild( root );

    TQDomElement tag = doc.createElement( "Greeting" );
    root.appendChild( tag );

    TQDomText t = doc.createTextNode( "Hello World" );
    tag.appendChild( t );

    TQString xml = doc.toString();
    \endcode

    For further information about the Document Object Model see
    \link http://www.w3.org/TR/REC-DOM-Level-1/\endlink and
    \link http://www.w3.org/TR/DOM-Level-2-Core/\endlink.
    For a more general introduction of the DOM implementation see the
    TQDomDocument documentation.
*/


/*!
    Constructs an empty document.
*/
TQDomDocument::TQDomDocument()
{
    impl = 0;
}

/*!
    Creates a document and sets the name of the document type to \a
    name.
*/
TQDomDocument::TQDomDocument( const TQString& name )
{
    // We take over ownership
    impl = new TQDomDocumentPrivate( name );
}

/*!
    Creates a document with the document type \a doctype.

    \sa TQDomImplementation::createDocumentType()
*/
TQDomDocument::TQDomDocument( const TQDomDocumentType& doctype )
{
    impl = new TQDomDocumentPrivate( (TQDomDocumentTypePrivate*)(doctype.impl) );
}

/*!
    Constructs a copy of \a x.

    The data of the copy is shared (shallow copy): modifying one node
    will also change the other. If you want to make a deep copy, use
    cloneNode().
*/
TQDomDocument::TQDomDocument( const TQDomDocument& x )
    : TQDomNode( x )
{
}

TQDomDocument::TQDomDocument( TQDomDocumentPrivate* x )
    : TQDomNode( x )
{
}

/*!
    Assigns \a x to this DOM document.

    The data of the copy is shared (shallow copy): modifying one node
    will also change the other. If you want to make a deep copy, use
    cloneNode().
*/
TQDomDocument& TQDomDocument::operator= ( const TQDomDocument& x )
{
    return (TQDomDocument&) TQDomNode::operator=( x );
}

/*!
    Destroys the object and frees its resources.
*/
TQDomDocument::~TQDomDocument()
{
}

/*!
    \overload

    This function reads the XML document from the string \a text.
    Since \a text is already a Unicode string, no encoding detection
    is done.
*/
bool TQDomDocument::setContent( const TQString& text, bool namespaceProcessing, TQString *errorMsg, int *errorLine, int *errorColumn )
{
    if ( !impl )
	impl = new TQDomDocumentPrivate;
    TQXmlInputSource source;
    source.setData( text );
    return IMPL->setContent( &source, namespaceProcessing, errorMsg, errorLine, errorColumn );
}

/*!
    This function parses the XML document from the byte array \a
    buffer and sets it as the content of the document. It tries to
    detect the encoding of the document as required by the XML
    specification.

    If \a namespaceProcessing is true, the parser recognizes
    namespaces in the XML file and sets the prefix name, local name
    and namespace URI to appropriate values. If \a namespaceProcessing
    is false, the parser does no namespace processing when it reads
    the XML file.

    If a parse error occurs, the function returns false; otherwise it
    returns true. If a parse error occurs and \a errorMsg, \a
    errorLine and \a errorColumn are not 0, the error message is
    placed in \a *errorMsg, the line number \a *errorLine and the
    column number in \a *errorColumn.

    If \a namespaceProcessing is true, the function TQDomNode::prefix()
    returns a string for all elements and attributes. It returns an
    empty string if the element or attribute has no prefix.

    If \a namespaceProcessing is false, the functions
    TQDomNode::prefix(), TQDomNode::localName() and
    TQDomNode::namespaceURI() return TQString::null.

    \sa TQDomNode::namespaceURI() TQDomNode::localName()
    TQDomNode::prefix() TQString::isNull() TQString::isEmpty()
*/
bool TQDomDocument::setContent( const TQByteArray& buffer, bool namespaceProcessing, TQString *errorMsg, int *errorLine, int *errorColumn )
{
    if ( !impl )
	impl = new TQDomDocumentPrivate;
    TQBuffer buf( buffer );
    TQXmlInputSource source( &buf );
    return IMPL->setContent( &source, namespaceProcessing, errorMsg, errorLine, errorColumn );
}

/*!
    \overload

    This function reads the XML document from the C string \a buffer.

    \warning This function does not try to detect the encoding:
    instead it assumes that the C string is UTF-8 encoded.
*/
bool TQDomDocument::setContent( const TQCString& buffer, bool namespaceProcessing, TQString *errorMsg, int *errorLine, int *errorColumn )
{
    return setContent( TQString::fromUtf8( buffer, buffer.length() ), namespaceProcessing, errorMsg, errorLine, errorColumn );
}

/*!
    \overload

    This function reads the XML document from the IO device \a dev.
*/
bool TQDomDocument::setContent( TQIODevice* dev, bool namespaceProcessing, TQString *errorMsg, int *errorLine, int *errorColumn )
{
    if ( !impl )
	impl = new TQDomDocumentPrivate;
    TQXmlInputSource source( dev );
    return IMPL->setContent( &source, namespaceProcessing, errorMsg, errorLine, errorColumn );
}

/*!
    \overload

    This function reads the XML document from the string \a text.
    Since \a text is already a Unicode string, no encoding detection
    is performed.

    No namespace processing is performed either.
*/
bool TQDomDocument::setContent( const TQString& text, TQString *errorMsg, int *errorLine, int *errorColumn )
{
    return setContent( text, false, errorMsg, errorLine, errorColumn );
}

/*!
    \overload

    This function reads the XML document from the byte array \a
    buffer.

    No namespace processing is performed.
*/
bool TQDomDocument::setContent( const TQByteArray& buffer, TQString *errorMsg, int *errorLine, int *errorColumn  )
{
    return setContent( buffer, false, errorMsg, errorLine, errorColumn );
}

/*!
    \overload

    This function reads the XML document from the C string \a buffer.

    No namespace processing is performed.

    \warning This function does not try to detect the encoding:
    instead it assumes that the C string is UTF-8 encoded.
*/
bool TQDomDocument::setContent( const TQCString& buffer, TQString *errorMsg, int *errorLine, int *errorColumn  )
{
    return setContent( buffer, false, errorMsg, errorLine, errorColumn );
}

/*!
    \overload

    This function reads the XML document from the IO device \a dev.

    No namespace processing is performed.
*/
bool TQDomDocument::setContent( TQIODevice* dev, TQString *errorMsg, int *errorLine, int *errorColumn  )
{
    return setContent( dev, false, errorMsg, errorLine, errorColumn );
}

/*!
    \overload

    This function reads the XML document from the TQXmlInputSource \a source and
    parses it with the TQXmlReader \a reader.

    This function doesn't change the features of the \a reader. If you want to
    use certain features for parsing you can use this function to set up the
    reader appropriate.

    \sa TQXmlSimpleReader
*/
bool TQDomDocument::setContent( TQXmlInputSource *source, TQXmlReader *reader, TQString *errorMsg, int *errorLine, int *errorColumn  )
{
    if ( !impl )
	impl = new TQDomDocumentPrivate;
    return IMPL->setContent( source, reader, errorMsg, errorLine, errorColumn );
}

/*!
    Converts the parsed document back to its textual representation.

    \sa toCString()
*/
TQString TQDomDocument::toString() const
{
    TQString str;
    TQTextStream s( str, IO_WriteOnly );
    save( s, 1 );

    return str;
}

/*!
    \overload

    This function uses \a indent as the amount of space to indent
    subelements.
*/
TQString TQDomDocument::toString( int indent ) const
{
    TQString str;
    TQTextStream s( str, IO_WriteOnly );
    save( s, indent );

    return str;
}

/*!
    Converts the parsed document back to its textual representation
    and returns a TQCString for that is encoded in UTF-8.

    \sa toString()
*/
TQCString TQDomDocument::toCString() const
{
    // ### if there is an encoding specified in the xml declaration, this
    // encoding declaration should be changed to utf8
    return toString().utf8();
}

/*!
    \overload

    This function uses \a indent as the amount of space to indent
    subelements.
*/
TQCString TQDomDocument::toCString( int indent ) const
{
    // ### if there is an encoding specified in the xml declaration, this
    // encoding declaration should be changed to utf8
    return toString( indent ).utf8();
}


/*!
    Returns the document type of this document.
*/
TQDomDocumentType TQDomDocument::doctype() const
{
    if ( !impl )
	return TQDomDocumentType();
    return TQDomDocumentType( IMPL->doctype() );
}

/*!
    Returns a TQDomImplementation object.
*/
TQDomImplementation TQDomDocument::implementation() const
{
    if ( !impl )
	return TQDomImplementation();
    return TQDomImplementation( IMPL->implementation() );
}

/*!
    Returns the root element of the document.
*/
TQDomElement TQDomDocument::documentElement() const
{
    if ( !impl )
	return TQDomElement();
    return TQDomElement( IMPL->documentElement() );
}

/*!
    Creates a new element called \a tagName that can be inserted into
    the DOM tree, e.g. using TQDomNode::appendChild().

    \sa createElementNS() TQDomNode::appendChild() TQDomNode::insertBefore()
    TQDomNode::insertAfter()
*/
TQDomElement TQDomDocument::createElement( const TQString& tagName )
{
    if ( !impl )
        impl = new TQDomDocumentPrivate;
    return TQDomElement( IMPL->createElement( tagName ) );
}

/*!
    Creates a new document fragment, that can be used to hold parts of
    the document, e.g. when doing complex manipulations of the
    document tree.
*/
TQDomDocumentFragment TQDomDocument::createDocumentFragment()
{
    if ( !impl )
        impl = new TQDomDocumentPrivate;
    return TQDomDocumentFragment( IMPL->createDocumentFragment() );
}

/*!
    Creates a text node for the string \a value that can be inserted
    into the document tree, e.g. using TQDomNode::appendChild().

    \warning All characters within an XML document must be in the range:

    #x9 | #xA | #xD | [#x20-#xD7FF] | [#xE000-#xFFFD] | [#x10000-#x10FFFF]

    This rule also applies to characters encoded as character entities and
    characters in CDATA sections. If you use this function to insert
    characters outside of this range, the document will not be well-formed.

    If you want to store binary data in an XML document you must either use
    your own scheme to escape illegal characters, or you must store it in
    an external unparsed entity.

    \sa TQDomNode::appendChild() TQDomNode::insertBefore() TQDomNode::insertAfter()
*/
TQDomText TQDomDocument::createTextNode( const TQString& value )
{
    if ( !impl )
        impl = new TQDomDocumentPrivate;
    return TQDomText( IMPL->createTextNode( value ) );
}

/*!
    Creates a new comment for the string \a value that can be inserted
    into the document, e.g. using TQDomNode::appendChild().

    \sa TQDomNode::appendChild() TQDomNode::insertBefore() TQDomNode::insertAfter()
*/
TQDomComment TQDomDocument::createComment( const TQString& value )
{
    if ( !impl )
        impl = new TQDomDocumentPrivate;
    return TQDomComment( IMPL->createComment( value ) );
}

/*!
    Creates a new CDATA section for the string \a value that can be
    inserted into the document, e.g. using TQDomNode::appendChild().

    \sa TQDomNode::appendChild() TQDomNode::insertBefore() TQDomNode::insertAfter()
*/
TQDomCDATASection TQDomDocument::createCDATASection( const TQString& value )
{
    if ( !impl )
        impl = new TQDomDocumentPrivate;
    return TQDomCDATASection( IMPL->createCDATASection( value ) );
}

/*!
    Creates a new processing instruction that can be inserted into the
    document, e.g. using TQDomNode::appendChild(). This function sets
    the target for the processing instruction to \a target and the
    data to \a data.

    \sa TQDomNode::appendChild() TQDomNode::insertBefore() TQDomNode::insertAfter()
*/
TQDomProcessingInstruction TQDomDocument::createProcessingInstruction( const TQString& target,
								     const TQString& data )
{
    if ( !impl )
        impl = new TQDomDocumentPrivate;
    return TQDomProcessingInstruction( IMPL->createProcessingInstruction( target, data ) );
}


/*!
    Creates a new attribute called \a name that can be inserted into
    an element, e.g. using TQDomElement::setAttributeNode().

    \sa createAttributeNS()
*/
TQDomAttr TQDomDocument::createAttribute( const TQString& name )
{
    if ( !impl )
        impl = new TQDomDocumentPrivate;
    return TQDomAttr( IMPL->createAttribute( name ) );
}

/*!
    Creates a new entity reference called \a name that can be inserted
    into the document, e.g. using TQDomNode::appendChild().

    \sa TQDomNode::appendChild() TQDomNode::insertBefore() TQDomNode::insertAfter()
*/
TQDomEntityReference TQDomDocument::createEntityReference( const TQString& name )
{
    if ( !impl )
        impl = new TQDomDocumentPrivate;
    return TQDomEntityReference( IMPL->createEntityReference( name ) );
}

/*!
    Returns a TQDomNodeList, that contains all the elements in the
    document with the name \a tagname. The order of the node list is
    the order they are encountered in a preorder traversal of the
    element tree.

    \sa elementsByTagNameNS() TQDomElement::elementsByTagName()
*/
TQDomNodeList TQDomDocument::elementsByTagName( const TQString& tagname ) const
{
    return TQDomNodeList( new TQDomNodeListPrivate( impl, tagname ) );
}

/*!
    Imports the node \a importedNode from another document to this
    document. \a importedNode remains in the original document; this
    function creates a copy that can be used within this document.

    This function returns the imported node that belongs to this
    document. The returned node has no parent. It is not possible to
    import TQDomDocument and TQDomDocumentType nodes. In those cases
    this function returns a \link TQDomNode::isNull() null node\endlink.

    If \a deep is true, this function imports not only the node \a
    importedNode but its whole subtree; if it is false, only the \a
    importedNode is imported. The argument \a deep has no effect on
    TQDomAttr and TQDomEntityReference nodes, since the descendents of
    TQDomAttr nodes are always imported and those of
    TQDomEntityReference nodes are never imported.

    The behavior of this function is slightly different depending on
    the node types:
    \table
    \header \i Node Type \i Behaviour
    \row \i TQDomAttr
	 \i The owner element is set to 0 and the specified flag is
	    set to true in the generated attribute. The whole subtree
	    of \a importedNode is always imported for attribute nodes:
	    \a deep has no effect.
    \row \i TQDomDocument
	 \i Document nodes cannot be imported.
    \row \i TQDomDocumentFragment
	 \i If \a deep is true, this function imports the whole
	    document fragment; otherwise it only generates an empty
	    document fragment.
    \row \i TQDomDocumentType
	 \i Document type nodes cannot be imported.
    \row \i TQDomElement
	 \i Attributes for which TQDomAttr::specified() is true are
	    also imported, other attributes are not imported. If \a
	    deep is true, this function also imports the subtree of \a
	    importedNode; otherwise it imports only the element node
	    (and some attributes, see above).
    \row \i TQDomEntity
	 \i Entity nodes can be imported, but at the moment there is
	    no way to use them since the document type is read-only in
	    DOM level 2.
    \row \i TQDomEntityReference
	 \i Descendents of entity reference nodes are never imported:
	    \a deep has no effect.
    \row \i TQDomNotation
	 \i Notation nodes can be imported, but at the moment there is
	    no way to use them since the document type is read-only in
	    DOM level 2.
    \row \i TQDomProcessingInstruction
	 \i The target and value of the processing instruction is
	    copied to the new node.
    \row \i TQDomText
	 \i The text is copied to the new node.
    \row \i TQDomCDATASection
	 \i The text is copied to the new node.
    \row \i TQDomComment
	 \i The text is copied to the new node.
    \endtable

    \sa TQDomElement::setAttribute() TQDomNode::insertBefore()
	TQDomNode::insertAfter() TQDomNode::replaceChild() TQDomNode::removeChild()
	TQDomNode::appendChild()
*/
TQDomNode TQDomDocument::importNode( const TQDomNode& importedNode, bool deep )
{
    if ( !impl )
        impl = new TQDomDocumentPrivate;
    return TQDomNode( IMPL->importNode( importedNode.impl, deep ) );
}

/*!
    Creates a new element with namespace support that can be inserted
    into the DOM tree. The name of the element is \a qName and the
    namespace URI is \a nsURI. This function also sets
    TQDomNode::prefix() and TQDomNode::localName() to appropriate values
    (depending on \a qName).

    \sa createElement()
*/
TQDomElement TQDomDocument::createElementNS( const TQString& nsURI, const TQString& qName )
{
    if ( !impl )
        impl = new TQDomDocumentPrivate;
    return TQDomElement( IMPL->createElementNS( nsURI, qName ) );
}

/*!
    Creates a new attribute with namespace support that can be
    inserted into an element. The name of the attribute is \a qName
    and the namespace URI is \a nsURI. This function also sets
    TQDomNode::prefix() and TQDomNode::localName() to appropriate values
    (depending on \a qName).

    \sa createAttribute()
*/
TQDomAttr TQDomDocument::createAttributeNS( const TQString& nsURI, const TQString& qName )
{
    if ( !impl )
        impl = new TQDomDocumentPrivate;
    return TQDomAttr( IMPL->createAttributeNS( nsURI, qName ) );
}

/*!
    Returns a TQDomNodeList that contains all the elements in the
    document with the local name \a localName and a namespace URI of
    \a nsURI. The order of the node list is the order they are
    encountered in a preorder traversal of the element tree.

    \sa elementsByTagName() TQDomElement::elementsByTagNameNS()
*/
TQDomNodeList TQDomDocument::elementsByTagNameNS( const TQString& nsURI, const TQString& localName )
{
    return TQDomNodeList( new TQDomNodeListPrivate( impl, nsURI, localName ) );
}

/*!
    Returns the element whose ID is equal to \a elementId. If no
    element with the ID was found, this function returns a \link
    TQDomNode::isNull() null element\endlink.

    Since the TQDomClasses do not know which attributes are element
    IDs, this function returns always a \link TQDomNode::isNull() null
    element\endlink. This may change in a future version.
*/
TQDomElement TQDomDocument::elementById( const TQString& /*elementId*/ )
{
    return TQDomElement();
}

/*!
    Returns \c DocumentNode.
*/
TQDomNode::NodeType TQDomDocument::nodeType() const
{
    return DocumentNode;
}

/*!
    Returns true.
*/
bool TQDomDocument::isDocument() const
{
    return true;
}


#undef IMPL

/**************************************************************
 *
 * Node casting functions
 *
 **************************************************************/

/*!
    Converts a TQDomNode into a TQDomAttr. If the node is not an
    attribute, the returned object will be \link TQDomNode::isNull()
    null\endlink.

    \sa isAttr()
*/
TQDomAttr TQDomNode::toAttr()
{
    if ( impl && impl->isAttr() )
	return TQDomAttr( ((TQDomAttrPrivate*)impl) );
    return TQDomAttr();
}

/*!
    Converts a TQDomNode into a TQDomCDATASection. If the node is not a
    CDATA section, the returned object will be \link
    TQDomNode::isNull() null\endlink.

    \sa isCDATASection()
*/
TQDomCDATASection TQDomNode::toCDATASection()
{
    if ( impl && impl->isCDATASection() )
	return TQDomCDATASection( ((TQDomCDATASectionPrivate*)impl) );
    return TQDomCDATASection();
}

/*!
    Converts a TQDomNode into a TQDomDocumentFragment. If the node is
    not a document fragment the returned object will be \link
    TQDomNode::isNull() null\endlink.

    \sa isDocumentFragment()
*/
TQDomDocumentFragment TQDomNode::toDocumentFragment()
{
    if ( impl && impl->isDocumentFragment() )
	return TQDomDocumentFragment( ((TQDomDocumentFragmentPrivate*)impl) );
    return TQDomDocumentFragment();
}

/*!
    Converts a TQDomNode into a TQDomDocument. If the node is not a
    document the returned object will be \link TQDomNode::isNull()
    null\endlink.

    \sa isDocument()
*/
TQDomDocument TQDomNode::toDocument()
{
    if ( impl && impl->isDocument() )
	return TQDomDocument( ((TQDomDocumentPrivate*)impl) );
    return TQDomDocument();
}

/*!
    Converts a TQDomNode into a TQDomDocumentType. If the node is not a
    document type the returned object will be \link TQDomNode::isNull()
    null\endlink.

    \sa isDocumentType()
*/
TQDomDocumentType TQDomNode::toDocumentType()
{
    if ( impl && impl->isDocumentType() )
	return TQDomDocumentType( ((TQDomDocumentTypePrivate*)impl) );
    return TQDomDocumentType();
}

/*!
    Converts a TQDomNode into a TQDomElement. If the node is not an
    element the returned object will be \link TQDomNode::isNull()
    null\endlink.

    \sa isElement()
*/
TQDomElement TQDomNode::toElement()
{
    if ( impl && impl->isElement() )
	return TQDomElement( ((TQDomElementPrivate*)impl) );
    return TQDomElement();
}

/*!
    Converts a TQDomNode into a TQDomEntityReference. If the node is not
    an entity reference, the returned object will be \link
    TQDomNode::isNull() null\endlink.

    \sa isEntityReference()
*/
TQDomEntityReference TQDomNode::toEntityReference()
{
    if ( impl && impl->isEntityReference() )
	return TQDomEntityReference( ((TQDomEntityReferencePrivate*)impl) );
    return TQDomEntityReference();
}

/*!
    Converts a TQDomNode into a TQDomText. If the node is not a text,
    the returned object will be \link TQDomNode::isNull() null\endlink.

    \sa isText()
*/
TQDomText TQDomNode::toText()
{
    if ( impl && impl->isText() )
	return TQDomText( ((TQDomTextPrivate*)impl) );
    return TQDomText();
}

/*!
    Converts a TQDomNode into a TQDomEntity. If the node is not an
    entity the returned object will be \link TQDomNode::isNull()
    null\endlink.

    \sa isEntity()
*/
TQDomEntity TQDomNode::toEntity()
{
    if ( impl && impl->isEntity() )
	return TQDomEntity( ((TQDomEntityPrivate*)impl) );
    return TQDomEntity();
}

/*!
    Converts a TQDomNode into a TQDomNotation. If the node is not a
    notation the returned object will be \link TQDomNode::isNull()
    null\endlink.

    \sa isNotation()
*/
TQDomNotation TQDomNode::toNotation()
{
    if ( impl && impl->isNotation() )
	return TQDomNotation( ((TQDomNotationPrivate*)impl) );
    return TQDomNotation();
}

/*!
    Converts a TQDomNode into a TQDomProcessingInstruction. If the node
    is not a processing instruction the returned object will be \link
    TQDomNode::isNull() null\endlink.

    \sa isProcessingInstruction()
*/
TQDomProcessingInstruction TQDomNode::toProcessingInstruction()
{
    if ( impl && impl->isProcessingInstruction() )
	return TQDomProcessingInstruction( ((TQDomProcessingInstructionPrivate*)impl) );
    return TQDomProcessingInstruction();
}

/*!
    Converts a TQDomNode into a TQDomCharacterData. If the node is not a
    character data node the returned object will be \link
    TQDomNode::isNull() null\endlink.

    \sa isCharacterData()
*/
TQDomCharacterData TQDomNode::toCharacterData()
{
    if ( impl && impl->isCharacterData() )
	return TQDomCharacterData( ((TQDomCharacterDataPrivate*)impl) );
    return TQDomCharacterData();
}

/*!
    Converts a TQDomNode into a TQDomComment. If the node is not a
    comment the returned object will be \link TQDomNode::isNull()
    null\endlink.

    \sa isComment()
*/
TQDomComment TQDomNode::toComment()
{
    if ( impl && impl->isComment() )
	return TQDomComment( ((TQDomCommentPrivate*)impl) );
    return TQDomComment();
}

/**************************************************************
 *
 * TQDomHandler
 *
 **************************************************************/

TQDomHandler::TQDomHandler( TQDomDocumentPrivate* adoc, bool namespaceProcessing )
{
    doc = adoc;
    node = doc;
    cdata = false;
    nsProcessing = namespaceProcessing;
}

TQDomHandler::~TQDomHandler()
{
}

bool TQDomHandler::endDocument()
{
    // ### is this really necessary? (rms)
    if ( node != doc )
	return false;
    return true;
}

bool TQDomHandler::startDTD( const TQString& name, const TQString& publicId, const TQString& systemId )
{
    doc->doctype()->name = name;
    doc->doctype()->publicId = publicId;
    doc->doctype()->systemId = systemId;
    return true;
}

bool TQDomHandler::startElement( const TQString& nsURI, const TQString&, const TQString& qName, const TQXmlAttributes& atts )
{
    // tag name
    TQDomNodePrivate* n;
    if ( nsProcessing ) {
	n = doc->createElementNS( nsURI, qName );
    } else {
	n = doc->createElement( qName );
    }
    node->appendChild( n );
    node = n;

    // attributes
    for ( int i=0; i<atts.length(); i++ )
    {
	if ( nsProcessing ) {
	    ((TQDomElementPrivate*)node)->setAttributeNS( atts.uri(i), atts.qName(i), atts.value(i) );
	} else {
	    ((TQDomElementPrivate*)node)->setAttribute( atts.qName(i), atts.value(i) );
	}
    }

    return true;
}

bool TQDomHandler::endElement( const TQString&, const TQString&, const TQString& )
{
    if ( node == doc )
	return false;
    node = node->parent();

    return true;
}

bool TQDomHandler::characters( const TQString&  ch )
{
    // No text as child of some document
    if ( node == doc )
	return false;

    if ( cdata ) {
	node->appendChild( doc->createCDATASection( ch ) );
    } else if ( !entityName.isEmpty() ) {
	TQDomEntityPrivate* e = new TQDomEntityPrivate( doc, 0, entityName,
		TQString::null, TQString::null, TQString::null );
	e->value = ch;
	doc->doctype()->appendChild( e );
	node->appendChild( doc->createEntityReference( entityName ) );
    } else {
	node->appendChild( doc->createTextNode( ch ) );
    }

    return true;
}

bool TQDomHandler::processingInstruction( const TQString& target, const TQString& data )
{
    node->appendChild( doc->createProcessingInstruction( target, data ) );
    return true;
}

bool TQDomHandler::skippedEntity( const TQString& name )
{
    node->appendChild( doc->createEntityReference( name ) );
    return true;
}

bool TQDomHandler::fatalError( const TQXmlParseException& exception )
{
    errorMsg = exception.message();
    errorLine =  exception.lineNumber();
    errorColumn =  exception.columnNumber();
    return TQXmlDefaultHandler::fatalError( exception );
}

bool TQDomHandler::startCDATA()
{
    cdata = true;
    return true;
}

bool TQDomHandler::endCDATA()
{
    cdata = false;
    return true;
}

bool TQDomHandler::startEntity( const TQString &name )
{
    entityName = name;
    return true;
}

bool TQDomHandler::endEntity( const TQString & )
{
    entityName = TQString::null;
    return true;
}

bool TQDomHandler::comment( const TQString& ch )
{
    node->appendChild( doc->createComment( ch ) );
    return true;
}

bool TQDomHandler::unparsedEntityDecl( const TQString &name, const TQString &publicId, const TQString &systemId, const TQString &notationName )
{
    TQDomEntityPrivate* e = new TQDomEntityPrivate( doc, 0, name,
	    publicId, systemId, notationName );
    doc->doctype()->appendChild( e );
    return true;
}

bool TQDomHandler::externalEntityDecl( const TQString &name, const TQString &publicId, const TQString &systemId )
{
    return unparsedEntityDecl( name, publicId, systemId, TQString::null );
}

bool TQDomHandler::notationDecl( const TQString & name, const TQString & publicId, const TQString & systemId )
{
    TQDomNotationPrivate* n = new TQDomNotationPrivate( doc, 0, name, publicId, systemId );
    doc->doctype()->appendChild( n );
    return true;
}

#endif //TQT_NO_DOM
