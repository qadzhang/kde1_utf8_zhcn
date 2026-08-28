/**********************************************************************
** Copyright (C) 2000-2008 Trolltech ASA.  All rights reserved.
**
** This file is part of the TQt Assistant.
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

#ifndef DOCUPARSER_H
#define DOCUPARSER_H

#include <ntqxml.h>
#include <ntqptrlist.h>
#include <ntqmap.h>

class Profile;

struct ContentItem {
    ContentItem()
	: title( TQString::null ), reference( TQString::null ), depth( 0 ) {}
    ContentItem( const TQString &t, const TQString &r, int d )
	: title( t ), reference( r ), depth( d ) {}
    TQString title;
    TQString reference;
    int depth;
};

TQDataStream &operator>>( TQDataStream &s, ContentItem &ci );
TQDataStream &operator<<( TQDataStream &s, const ContentItem &ci );

struct IndexItem {
    IndexItem( const TQString &k, const TQString &r )
	: keyword( k ), reference( r ) {}
    TQString keyword;
    TQString reference;
};



class DocuParser : public TQXmlDefaultHandler
{
public:
    enum ParserVersion { TQt310, TQt320 };
    // Since We don't want problems with documentation
    // from version to version, this string stores the correct
    // version string to save documents.
    static const TQString DocumentKey;

    static DocuParser *createParser( const TQString &fileName );

    virtual bool parse( TQFile *file );
    
    TQValueList<ContentItem> getContentItems();
    TQPtrList<IndexItem> getIndexItems();

    TQString errorProtocol() const;
    TQString contentsURL() const { return conURL; }

    virtual ParserVersion parserVersion() const = 0;
    virtual void addTo( Profile *p ) = 0;

    TQString fileName() const { return fname; }
    void setFileName( const TQString &file ) { fname = file; }

protected:
    TQString absolutify( const TQString &input ) const;
    
    TQString contentRef, indexRef, errorProt, conURL;
    TQString docTitle, title, iconName;
    TQValueList<ContentItem> contentList;
    TQPtrList<IndexItem> indexList;
    TQString fname;
};


class DocuParser310 : public DocuParser
{
public:
    enum States{ StateInit, StateContent, StateSect, StateKeyword };
    
    bool startDocument();
    bool startElement( const TQString&, const TQString&, const TQString& ,
                       const TQXmlAttributes& );
    bool endElement( const TQString&, const TQString&, const TQString& );
    bool characters( const TQString & );
    bool fatalError( const TQXmlParseException& exception );

    virtual ParserVersion parserVersion() const { return TQt310; }
    virtual void addTo( Profile *p );
    
private:
    States state;
    int depth;
};


class DocuParser320 : public DocuParser
{
public:
    enum States { StateInit, StateDocRoot, StateProfile, StateProperty,
		  StateContent, StateSect, StateKeyword };

    DocuParser320();    
    
    bool startDocument();
    bool startElement( const TQString&, const TQString&, const TQString& ,
                       const TQXmlAttributes& );
    bool endElement( const TQString&, const TQString&, const TQString& );
    bool characters( const TQString & );
    bool fatalError( const TQXmlParseException& exception );

    virtual ParserVersion parserVersion() const { return TQt320; }
    virtual void addTo( Profile *p );
    Profile *profile() const { return prof; }    

private:
    
    States state;
    int depth;
    int docfileCounter;
    TQString propertyValue;
    TQString propertyName;
    Profile *prof;
};
#endif //DOCUPARSER_H
