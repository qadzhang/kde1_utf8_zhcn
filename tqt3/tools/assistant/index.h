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

#ifndef INDEX_H
#define INDEX_H

#include <ntqstringlist.h>
#include <ntqdict.h>
#include <ntqdatastream.h>
#include <ntqobject.h>

struct Document {
    Document( int d, int f ) : docNumber( d ), frequency( f ) {}
    Document() : docNumber( -1 ), frequency( 0 ) {}
    bool operator==( const Document &doc ) const {
	return docNumber == doc.docNumber;
    }
    bool operator<( const Document &doc ) const {
	return frequency > doc.frequency;
    }
    bool operator<=( const Document &doc ) const {
	return frequency >= doc.frequency;
    }
    bool operator>( const Document &doc ) const {
	return frequency < doc.frequency;
    }
    TQ_INT16 docNumber;
    TQ_INT16 frequency;
};

TQDataStream &operator>>( TQDataStream &s, Document &l );
TQDataStream &operator<<( TQDataStream &s, const Document &l );

class Index : public TQObject
{
    TQ_OBJECT
public:
    struct Entry {
	Entry( int d ) { documents.append( Document( d, 1 ) ); }
	Entry( TQValueList<Document> l ) : documents( l ) {}
	TQValueList<Document> documents;
    };
    struct PosEntry {
	PosEntry( int p ) { positions.append( p ); }
	TQValueList<uint> positions;
    };

    Index( const TQString &dp, const TQString &hp );
    Index( const TQStringList &dl, const TQString &hp );
    void writeDict();
    void readDict();
    int makeIndex();
    TQStringList query( const TQStringList&, const TQStringList&, const TQStringList& );
    TQString getDocumentTitle( const TQString& );
    void setDictionaryFile( const TQString& );
    void setDocListFile( const TQString& );
    void setDocList( const TQStringList & );

signals:
    void indexingProgress( int );

private slots:
    void setLastWinClosed();

private:
    void setupDocumentList();
    void parseDocument( const TQString&, int );
    void insertInDict( const TQString&, int );
    void writeDocumentList();
    void readDocumentList();
    TQStringList getWildcardTerms( const TQString& );
    TQStringList split( const TQString& );
    TQValueList<Document> setupDummyTerm( const TQStringList& );
    bool searchForPattern( const TQStringList&, const TQStringList&, const TQString& );
    void buildMiniDict( const TQString& );
    TQStringList docList;
    TQDict<Entry> dict;
    TQDict<PosEntry> miniDict;
    uint wordNum;
    TQString docPath;
    TQString dictFile, docListFile;
    bool alreadyHaveDocList;
    bool lastWindowClosed;
};

struct Term {
    Term( const TQString &t, int f, TQValueList<Document> l )
	: term( t ), frequency( f ), documents( l ) {}
    TQString term;
    int frequency;
    TQValueList<Document>documents;
};

class TermList : public TQPtrList<Term>
{
public:
    TermList() : TQPtrList<Term>() {}
    int compareItems( TQPtrCollection::Item i1, TQPtrCollection::Item i2 );
};

#endif
