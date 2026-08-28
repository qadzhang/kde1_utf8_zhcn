/****************************************************************************
**
** Definition of TQDomDocument and related classes.
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

#ifndef TQDOM_H
#define TQDOM_H

#ifndef QT_H
#include "ntqstring.h"
#endif // QT_H

#if !defined(TQT_MODULE_XML) || defined( QT_LICENSE_PROFESSIONAL ) || defined( QT_INTERNAL_XML )
#define TQM_EXPORT_DOM
#else
#define TQM_EXPORT_DOM TQ_EXPORT
#endif

#ifndef TQT_NO_DOM

class TQIODevice;
class TQTextStream;

class TQXmlInputSource;
class TQXmlReader;

class TQDomDocumentPrivate;
class TQDomDocumentTypePrivate;
class TQDomDocumentFragmentPrivate;
class TQDomNodePrivate;
class TQDomNodeListPrivate;
class TQDomImplementationPrivate;
class TQDomElementPrivate;
class TQDomNotationPrivate;
class TQDomEntityPrivate;
class TQDomEntityReferencePrivate;
class TQDomProcessingInstructionPrivate;
class TQDomAttrPrivate;
class TQDomCharacterDataPrivate;
class TQDomTextPrivate;
class TQDomCommentPrivate;
class TQDomCDATASectionPrivate;
class TQDomNamedNodeMapPrivate;
class TQDomImplementationPrivate;

class TQDomNodeList;
class TQDomElement;
class TQDomText;
class TQDomComment;
class TQDomCDATASection;
class TQDomProcessingInstruction;
class TQDomAttr;
class TQDomEntityReference;
class TQDomDocument;
class TQDomNamedNodeMap;
class TQDomDocument;
class TQDomDocumentFragment;
class TQDomDocumentType;
class TQDomImplementation;
class TQDomNode;
class TQDomEntity;
class TQDomNotation;
class TQDomCharacterData;

class TQM_EXPORT_DOM TQDomImplementation
{
public:
    TQDomImplementation();
    TQDomImplementation( const TQDomImplementation& );
    virtual ~TQDomImplementation();
    TQDomImplementation& operator= ( const TQDomImplementation& );
    bool operator== ( const TQDomImplementation& ) const;
    bool operator!= ( const TQDomImplementation& ) const;

    // functions
    virtual bool hasFeature( const TQString& feature, const TQString& version );
    virtual TQDomDocumentType createDocumentType( const TQString& qName, const TQString& publicId, const TQString& systemId );
    virtual TQDomDocument createDocument( const TQString& nsURI, const TQString& qName, const TQDomDocumentType& doctype );

    // TQt extension
    bool isNull();

private:
    TQDomImplementationPrivate* impl;
    TQDomImplementation( TQDomImplementationPrivate* );

    friend class TQDomDocument;
};

class TQM_EXPORT_DOM TQDomNode
{
public:
    enum NodeType {
	ElementNode               = 1,
	AttributeNode             = 2,
	TextNode                  = 3,
	CDATASectionNode          = 4,
	EntityReferenceNode       = 5,
	EntityNode                = 6,
	ProcessingInstructionNode = 7,
	CommentNode               = 8,
	DocumentNode              = 9,
	DocumentTypeNode          = 10,
	DocumentFragmentNode      = 11,
	NotationNode              = 12,
	BaseNode                  = 21,// this is not in the standard
	CharacterDataNode         = 22 // this is not in the standard
    };

    TQDomNode();
    TQDomNode( const TQDomNode& );
    TQDomNode& operator= ( const TQDomNode& );
    bool operator== ( const TQDomNode& ) const;
    bool operator!= ( const TQDomNode& ) const;
    virtual ~TQDomNode();

    // DOM functions
    virtual TQDomNode insertBefore( const TQDomNode& newChild, const TQDomNode& refChild );
    virtual TQDomNode insertAfter( const TQDomNode& newChild, const TQDomNode& refChild );
    virtual TQDomNode replaceChild( const TQDomNode& newChild, const TQDomNode& oldChild );
    virtual TQDomNode removeChild( const TQDomNode& oldChild );
    virtual TQDomNode appendChild( const TQDomNode& newChild );
    virtual bool hasChildNodes() const;
    virtual TQDomNode cloneNode( bool deep = true ) const;
    virtual void normalize();
    virtual bool isSupported( const TQString& feature, const TQString& version ) const;

    // DOM read only attributes
    virtual TQString nodeName() const;
    virtual TQDomNode::NodeType nodeType() const;
    virtual TQDomNode         parentNode() const;
    virtual TQDomNodeList     childNodes() const;
    virtual TQDomNode         firstChild() const;
    virtual TQDomNode         lastChild() const;
    virtual TQDomNode         previousSibling() const;
    virtual TQDomNode         nextSibling() const;
    virtual TQDomNamedNodeMap attributes() const;
    virtual TQDomDocument     ownerDocument() const;
    virtual TQString namespaceURI() const;
    virtual TQString localName() const;
    virtual bool hasAttributes() const;

    // DOM attributes
    virtual TQString nodeValue() const;
    virtual void setNodeValue( const TQString& );
    virtual TQString prefix() const;
    virtual void setPrefix( const TQString& pre );

    // TQt extensions
    virtual bool isAttr() const;
    virtual bool isCDATASection() const;
    virtual bool isDocumentFragment() const;
    virtual bool isDocument() const;
    virtual bool isDocumentType() const;
    virtual bool isElement() const;
    virtual bool isEntityReference() const;
    virtual bool isText() const;
    virtual bool isEntity() const;
    virtual bool isNotation() const;
    virtual bool isProcessingInstruction() const;
    virtual bool isCharacterData() const;
    virtual bool isComment() const;

    /**
     * Shortcut to avoid dealing with TQDomNodeList
     * all the time.
     */
    TQDomNode namedItem( const TQString& name ) const;

    bool isNull() const;
    void clear();

    TQDomAttr toAttr();
    TQDomCDATASection toCDATASection();
    TQDomDocumentFragment toDocumentFragment();
    TQDomDocument toDocument();
    TQDomDocumentType toDocumentType();
    TQDomElement toElement();
    TQDomEntityReference toEntityReference();
    TQDomText toText();
    TQDomEntity toEntity();
    TQDomNotation toNotation();
    TQDomProcessingInstruction toProcessingInstruction();
    TQDomCharacterData toCharacterData();
    TQDomComment toComment();

    void save( TQTextStream&, int ) const;

protected:
    TQDomNodePrivate* impl;
    TQDomNode( TQDomNodePrivate* );

private:
    friend class TQDomDocument;
    friend class TQDomDocumentType;
    friend class TQDomNodeList;
    friend class TQDomNamedNodeMap;
};

class TQM_EXPORT_DOM TQDomNodeList
{
public:
    TQDomNodeList();
    TQDomNodeList( const TQDomNodeList& );
    TQDomNodeList& operator= ( const TQDomNodeList& );
    bool operator== ( const TQDomNodeList& ) const;
    bool operator!= ( const TQDomNodeList& ) const;
    virtual ~TQDomNodeList();

    // DOM functions
    virtual TQDomNode item( int index ) const;

    // DOM read only attributes
    virtual uint length() const;
    uint count() const { return length(); } // TQt API consitancy

private:
    TQDomNodeListPrivate* impl;
    TQDomNodeList( TQDomNodeListPrivate* );

    friend class TQDomNode;
    friend class TQDomElement;
    friend class TQDomDocument;
};

class TQM_EXPORT_DOM TQDomDocumentType : public TQDomNode
{
public:
    TQDomDocumentType();
    TQDomDocumentType( const TQDomDocumentType& x );
    TQDomDocumentType& operator= ( const TQDomDocumentType& );
    ~TQDomDocumentType();

    // DOM read only attributes
    virtual TQString name() const;
    virtual TQDomNamedNodeMap entities() const;
    virtual TQDomNamedNodeMap notations() const;
    virtual TQString publicId() const;
    virtual TQString systemId() const;
    virtual TQString internalSubset() const;

    // Reimplemented from TQDomNode
    TQDomNode::NodeType nodeType() const;
    bool isDocumentType() const;

private:
    TQDomDocumentType( TQDomDocumentTypePrivate* );

    friend class TQDomImplementation;
    friend class TQDomDocument;
    friend class TQDomNode;
};

class TQM_EXPORT_DOM TQDomDocument : public TQDomNode
{
public:
    TQDomDocument();
    explicit TQDomDocument( const TQString& name );
    explicit TQDomDocument( const TQDomDocumentType& doctype );
    TQDomDocument( const TQDomDocument& x );
    TQDomDocument& operator= ( const TQDomDocument& );
    ~TQDomDocument();

    // DOM functions
    TQDomElement createElement( const TQString& tagName );
    TQDomDocumentFragment createDocumentFragment();
    TQDomText createTextNode( const TQString& data );
    TQDomComment createComment( const TQString& data );
    TQDomCDATASection createCDATASection( const TQString& data );
    TQDomProcessingInstruction createProcessingInstruction( const TQString& target, const TQString& data );
    TQDomAttr createAttribute( const TQString& name );
    TQDomEntityReference createEntityReference( const TQString& name );
    TQDomNodeList elementsByTagName( const TQString& tagname ) const;
    TQDomNode importNode( const TQDomNode& importedNode, bool deep );
    TQDomElement createElementNS( const TQString& nsURI, const TQString& qName );
    TQDomAttr createAttributeNS( const TQString& nsURI, const TQString& qName );
    TQDomNodeList elementsByTagNameNS( const TQString& nsURI, const TQString& localName );
    TQDomElement elementById( const TQString& elementId );

    // DOM read only attributes
    TQDomDocumentType doctype() const;
    TQDomImplementation implementation() const;
    TQDomElement documentElement() const;

    // TQt extensions
    bool setContent( const TQCString& text, bool namespaceProcessing, TQString *errorMsg=0, int *errorLine=0, int *errorColumn=0  );
    bool setContent( const TQByteArray& text, bool namespaceProcessing, TQString *errorMsg=0, int *errorLine=0, int *errorColumn=0  );
    bool setContent( const TQString& text, bool namespaceProcessing, TQString *errorMsg=0, int *errorLine=0, int *errorColumn=0  );
    bool setContent( TQIODevice* dev, bool namespaceProcessing, TQString *errorMsg=0, int *errorLine=0, int *errorColumn=0  );
    bool setContent( const TQCString& text, TQString *errorMsg=0, int *errorLine=0, int *errorColumn=0 );
    bool setContent( const TQByteArray& text, TQString *errorMsg=0, int *errorLine=0, int *errorColumn=0  );
    bool setContent( const TQString& text, TQString *errorMsg=0, int *errorLine=0, int *errorColumn=0  );
    bool setContent( TQIODevice* dev, TQString *errorMsg=0, int *errorLine=0, int *errorColumn=0  );

    bool setContent( TQXmlInputSource *source, TQXmlReader *reader, TQString *errorMsg=0, int *errorLine=0, int *errorColumn=0  );

    // Reimplemented from TQDomNode
    TQDomNode::NodeType nodeType() const;
    bool isDocument() const;

    // TQt extensions
    TQString toString() const; // ### TQt 4: merge the two overloads
    TQString toString( int ) const;
    TQCString toCString() const; // ### TQt 4: merge the two overloads
    TQCString toCString( int ) const;

private:
    TQDomDocument( TQDomDocumentPrivate* );

    friend class TQDomNode;
};

class TQM_EXPORT_DOM TQDomNamedNodeMap
{
public:
    TQDomNamedNodeMap();
    TQDomNamedNodeMap( const TQDomNamedNodeMap& );
    TQDomNamedNodeMap& operator= ( const TQDomNamedNodeMap& );
    bool operator== ( const TQDomNamedNodeMap& ) const;
    bool operator!= ( const TQDomNamedNodeMap& ) const;
    ~TQDomNamedNodeMap();

    // DOM functions
    TQDomNode namedItem( const TQString& name ) const;
    TQDomNode setNamedItem( const TQDomNode& newNode );
    TQDomNode removeNamedItem( const TQString& name );
    TQDomNode item( int index ) const;
    TQDomNode namedItemNS( const TQString& nsURI, const TQString& localName ) const;
    TQDomNode setNamedItemNS( const TQDomNode& newNode );
    TQDomNode removeNamedItemNS( const TQString& nsURI, const TQString& localName );

    // DOM read only attributes
    uint length() const;
    uint count() const { return length(); } // TQt API consitancy

    // TQt extension
    bool contains( const TQString& name ) const;

private:
    TQDomNamedNodeMapPrivate* impl;
    TQDomNamedNodeMap( TQDomNamedNodeMapPrivate* );

    friend class TQDomNode;
    friend class TQDomDocumentType;
    friend class TQDomElement;
};

class TQM_EXPORT_DOM TQDomDocumentFragment : public TQDomNode
{
public:
    TQDomDocumentFragment();
    TQDomDocumentFragment( const TQDomDocumentFragment& x );
    TQDomDocumentFragment& operator= ( const TQDomDocumentFragment& );
    ~TQDomDocumentFragment();

    // Reimplemented from TQDomNode
    TQDomNode::NodeType nodeType() const;
    bool isDocumentFragment() const;

private:
    TQDomDocumentFragment( TQDomDocumentFragmentPrivate* );

    friend class TQDomDocument;
    friend class TQDomNode;
};

class TQM_EXPORT_DOM TQDomCharacterData : public TQDomNode
{
public:
    TQDomCharacterData();
    TQDomCharacterData( const TQDomCharacterData& x );
    TQDomCharacterData& operator= ( const TQDomCharacterData& );
    ~TQDomCharacterData();

    // DOM functions
    virtual TQString substringData( unsigned long offset, unsigned long count );
    virtual void appendData( const TQString& arg );
    virtual void insertData( unsigned long offset, const TQString& arg );
    virtual void deleteData( unsigned long offset, unsigned long count );
    virtual void replaceData( unsigned long offset, unsigned long count, const TQString& arg );

    // DOM read only attributes
    virtual uint length() const;

    // DOM attributes
    virtual TQString data() const;
    virtual void setData( const TQString& );

    // Reimplemented from TQDomNode
    TQDomNode::NodeType nodeType() const;
    bool isCharacterData() const;

private:
    TQDomCharacterData( TQDomCharacterDataPrivate* );

    friend class TQDomDocument;
    friend class TQDomText;
    friend class TQDomComment;
    friend class TQDomNode;
};

class TQM_EXPORT_DOM TQDomAttr : public TQDomNode
{
public:
    TQDomAttr();
    TQDomAttr( const TQDomAttr& x );
    TQDomAttr& operator= ( const TQDomAttr& );
    ~TQDomAttr();

    // DOM read only attributes
    virtual TQString name() const;
    virtual bool specified() const;
    virtual TQDomElement ownerElement() const;

    // DOM attributes
    virtual TQString value() const;
    virtual void setValue( const TQString& );

    // Reimplemented from TQDomNode
    TQDomNode::NodeType nodeType() const;
    bool isAttr() const;

private:
    TQDomAttr( TQDomAttrPrivate* );

    friend class TQDomDocument;
    friend class TQDomElement;
    friend class TQDomNode;
};

class TQM_EXPORT_DOM TQDomElement : public TQDomNode
{
public:
    TQDomElement();
    TQDomElement( const TQDomElement& x );
    TQDomElement& operator= ( const TQDomElement& );
    ~TQDomElement();

    // DOM functions
    TQString attribute( const TQString& name, const TQString& defValue = TQString::null ) const;
    void setAttribute( const TQString& name, const TQString& value );
    void setAttribute( const TQString& name, int value );
    void setAttribute( const TQString& name, uint value );
    void setAttribute( const TQString& name, long value );
    void setAttribute( const TQString& name, ulong value );
    void setAttribute( const TQString& name, double value );
    void removeAttribute( const TQString& name );
    TQDomAttr attributeNode( const TQString& name);
    TQDomAttr setAttributeNode( const TQDomAttr& newAttr );
    TQDomAttr removeAttributeNode( const TQDomAttr& oldAttr );
    virtual TQDomNodeList elementsByTagName( const TQString& tagname ) const;
    bool hasAttribute( const TQString& name ) const;

    TQString attributeNS( const TQString nsURI, const TQString& localName, const TQString& defValue ) const;
    void setAttributeNS( const TQString nsURI, const TQString& qName, const TQString& value );
    void setAttributeNS( const TQString nsURI, const TQString& qName, int value );
    void setAttributeNS( const TQString nsURI, const TQString& qName, uint value );
    void setAttributeNS( const TQString nsURI, const TQString& qName, long value );
    void setAttributeNS( const TQString nsURI, const TQString& qName, ulong value );
    void setAttributeNS( const TQString nsURI, const TQString& qName, double value );
    void removeAttributeNS( const TQString& nsURI, const TQString& localName );
    TQDomAttr attributeNodeNS( const TQString& nsURI, const TQString& localName );
    TQDomAttr setAttributeNodeNS( const TQDomAttr& newAttr );
    virtual TQDomNodeList elementsByTagNameNS( const TQString& nsURI, const TQString& localName ) const;
    bool hasAttributeNS( const TQString& nsURI, const TQString& localName ) const;

    // DOM read only attributes
    TQString tagName() const;
    void setTagName( const TQString& name ); // TQt extension

    // Reimplemented from TQDomNode
    TQDomNamedNodeMap attributes() const;
    TQDomNode::NodeType nodeType() const;
    bool isElement() const;

    TQString text() const;

private:
    TQDomElement( TQDomElementPrivate* );

    friend class TQDomDocument;
    friend class TQDomNode;
    friend class TQDomAttr;
};

class TQM_EXPORT_DOM TQDomText : public TQDomCharacterData
{
public:
    TQDomText();
    TQDomText( const TQDomText& x );
    TQDomText& operator= ( const TQDomText& );
    ~TQDomText();

    // DOM functions
    TQDomText splitText( int offset );

    // Reimplemented from TQDomNode
    TQDomNode::NodeType nodeType() const;
    bool isText() const;

private:
    TQDomText( TQDomTextPrivate* );

    friend class TQDomCDATASection;
    friend class TQDomDocument;
    friend class TQDomNode;
};

class TQM_EXPORT_DOM TQDomComment : public TQDomCharacterData
{
public:
    TQDomComment();
    TQDomComment( const TQDomComment& x );
    TQDomComment& operator= ( const TQDomComment& );
    ~TQDomComment();

    // Reimplemented from TQDomNode
    TQDomNode::NodeType nodeType() const;
    bool isComment() const;

private:
    TQDomComment( TQDomCommentPrivate* );

    friend class TQDomDocument;
    friend class TQDomNode;
};

class TQM_EXPORT_DOM TQDomCDATASection : public TQDomText
{
public:
    TQDomCDATASection();
    TQDomCDATASection( const TQDomCDATASection& x );
    TQDomCDATASection& operator= ( const TQDomCDATASection& );
    ~TQDomCDATASection();

    // Reimplemented from TQDomNode
    TQDomNode::NodeType nodeType() const;
    bool isCDATASection() const;

private:
    TQDomCDATASection( TQDomCDATASectionPrivate* );

    friend class TQDomDocument;
    friend class TQDomNode;
};

class TQM_EXPORT_DOM TQDomNotation : public TQDomNode
{
public:
    TQDomNotation();
    TQDomNotation( const TQDomNotation& x );
    TQDomNotation& operator= ( const TQDomNotation& );
    ~TQDomNotation();

    // DOM read only attributes
    TQString publicId() const;
    TQString systemId() const;

    // Reimplemented from TQDomNode
    TQDomNode::NodeType nodeType() const;
    bool isNotation() const;

private:
    TQDomNotation( TQDomNotationPrivate* );

    friend class TQDomDocument;
    friend class TQDomNode;
};

class TQM_EXPORT_DOM TQDomEntity : public TQDomNode
{
public:
    TQDomEntity();
    TQDomEntity( const TQDomEntity& x );
    TQDomEntity& operator= ( const TQDomEntity& );
    ~TQDomEntity();

    // DOM read only attributes
    virtual TQString publicId() const;
    virtual TQString systemId() const;
    virtual TQString notationName() const;

    // Reimplemented from TQDomNode
    TQDomNode::NodeType nodeType() const;
    bool isEntity() const;

private:
    TQDomEntity( TQDomEntityPrivate* );

    friend class TQDomNode;
};

class TQM_EXPORT_DOM TQDomEntityReference : public TQDomNode
{
public:
    TQDomEntityReference();
    TQDomEntityReference( const TQDomEntityReference& x );
    TQDomEntityReference& operator= ( const TQDomEntityReference& );
    ~TQDomEntityReference();

    // Reimplemented from TQDomNode
    TQDomNode::NodeType nodeType() const;
    bool isEntityReference() const;

private:
    TQDomEntityReference( TQDomEntityReferencePrivate* );

    friend class TQDomDocument;
    friend class TQDomNode;
};

class TQM_EXPORT_DOM TQDomProcessingInstruction : public TQDomNode
{
public:
    TQDomProcessingInstruction();
    TQDomProcessingInstruction( const TQDomProcessingInstruction& x );
    TQDomProcessingInstruction& operator= ( const TQDomProcessingInstruction& );
    ~TQDomProcessingInstruction();

    // DOM read only attributes
    virtual TQString target() const;

    // DOM attributes
    virtual TQString data() const;
    virtual void setData( const TQString& d );

    // Reimplemented from TQDomNode
    TQDomNode::NodeType nodeType() const;
    bool isProcessingInstruction() const;

private:
    TQDomProcessingInstruction( TQDomProcessingInstructionPrivate* );

    friend class TQDomDocument;
    friend class TQDomNode;
};


TQM_EXPORT_DOM TQTextStream& operator<<( TQTextStream&, const TQDomNode& );

#endif //TQT_NO_DOM
#endif // TQDOM_H
