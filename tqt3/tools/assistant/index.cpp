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

#include "index.h"

#include <ntqfile.h>
#include <ntqdir.h>
#include <ntqstringlist.h>
#include <ntqdict.h>
#include <ntqapplication.h>

#include <ctype.h>

int TermList::compareItems( TQPtrCollection::Item i1, TQPtrCollection::Item i2 )
{
    if( ( (Term*)i1 )->frequency == ( (Term*)i2 )->frequency )
	return 0;
    if( ( (Term*)i1 )->frequency < ( (Term*)i2 )->frequency )
	return -1;
    return 1;
}

TQDataStream &operator>>( TQDataStream &s, Document &l )
{
    s >> l.docNumber;
    s >> l.frequency;
    return s;
}

TQDataStream &operator<<( TQDataStream &s, const Document &l )
{
    s << (TQ_INT16)l.docNumber;
    s << (TQ_INT16)l.frequency;
    return s;
}

Index::Index( const TQString &dp, const TQString &hp )
    : TQObject( 0, 0 ), dict( 8999 ), docPath( dp )
{
    alreadyHaveDocList = false;
    lastWindowClosed = false;
    connect( tqApp, TQ_SIGNAL( lastWindowClosed() ),
	     this, TQ_SLOT( setLastWinClosed() ) );
}

Index::Index( const TQStringList &dl, const TQString &hp )
    : TQObject( 0, 0 ), dict( 8999 )
{
    docList = dl;
    alreadyHaveDocList = true;
    lastWindowClosed = false;
    connect( tqApp, TQ_SIGNAL( lastWindowClosed() ),
	     this, TQ_SLOT( setLastWinClosed() ) );
}

void Index::setLastWinClosed()
{
    lastWindowClosed = true;
}

void Index::setDictionaryFile( const TQString &f )
{
    dictFile = f;
}

void Index::setDocListFile( const TQString &f )
{
    docListFile = f;
}

void Index::setDocList( const TQStringList &lst )
{
    docList = lst;
}

int Index::makeIndex()
{
    if ( !alreadyHaveDocList )
	setupDocumentList();
    if ( docList.isEmpty() )
	return 1;
    TQStringList::Iterator it = docList.begin();
    int steps = docList.count() / 100;
    if ( !steps )
	steps++;
    int prog = 0;
    for ( int i = 0; it != docList.end(); ++it, ++i ) {
	if ( lastWindowClosed ) {
	    return -1;
	}
	parseDocument( *it, i );
	if ( i%steps == 0 ) {
	    prog++;
	    emit indexingProgress( prog );
	}
    }
    return 0;
}

void Index::setupDocumentList()
{
    TQDir d( docPath );
    TQStringList lst = d.entryList( "*.html" );
    TQStringList::ConstIterator it = lst.begin();
    for ( ; it != lst.end(); ++it )
	docList.append( docPath + "/" + *it );
}

void Index::insertInDict( const TQString &str, int docNum )
{
    if ( strcmp( str, "amp" ) == 0 || strcmp( str, "nbsp" ) == 0 )
	return;
    Entry *e = 0;
    if ( dict.count() )
	e = dict[ str ];

    if ( e ) {
	if ( e->documents.first().docNumber != docNum )
	    e->documents.prepend( Document( docNum, 1 ) );
	else
	    e->documents.first().frequency++;
    } else {
	dict.insert( str, new Entry( docNum ) );
    }
}

void Index::parseDocument( const TQString &filename, int docNum )
{
    TQFile file( filename );
    if ( !file.open( IO_ReadOnly ) ) {
	tqWarning( "can not open file " + filename );
	return;
    }

    TQTextStream s( &file );
    TQString text = s.read();
    if (text.isNull())
        return;

    bool valid = true;
    const TQChar *buf = text.unicode();
    TQChar str[64];
    TQChar c = buf[0];
    int j = 0;
    int i = 0;
    while ( (uint)j < text.length() ) {
	if ( c == '<' || c == '&' ) {
	    valid = false;
	    if ( i > 1 )
		insertInDict( TQString(str,i), docNum );
	    i = 0;
	    c = buf[++j];
	    continue;
	}
	if ( ( c == '>' || c == ';' ) && !valid ) {
	    valid = true;
	    c = buf[++j];
	    continue;
	}
	if ( !valid ) {
	    c = buf[++j];
	    continue;
	}
	if ( ( c.isLetterOrNumber() || c == '_' ) && i < 63 ) {
	    str[i] = c.lower();
	    ++i;
	} else {
	    if ( i > 1 )
		insertInDict( TQString(str,i), docNum );
	    i = 0;
	}
	c = buf[++j];
    }
    if ( i > 1 )
	insertInDict( TQString(str,i), docNum );
    file.close();
}

void Index::writeDict()
{
    TQDictIterator<Entry> it( dict );
    TQFile f( dictFile );
    if ( !f.open( IO_WriteOnly ) )
	return;
    TQDataStream s( &f );
    for( ; it.current(); ++it ) {
        Entry *e = it.current();
	s << it.currentKey();
	s << e->documents;
    }
    f.close();
    writeDocumentList();
}

void Index::writeDocumentList()
{
    TQFile f( docListFile );
    if ( !f.open( IO_WriteOnly ) )
	return;
    TQDataStream s( &f );
    s << docList;
}

void Index::readDict()
{
    TQFile f( dictFile );
    if ( !f.open( IO_ReadOnly ) )
	return;

    dict.clear();
    TQDataStream s( &f );
    TQString key;
    TQValueList<Document> docs;
    while ( !s.atEnd() ) {
	s >> key;
	s >> docs;
	dict.insert( key, new Entry( docs ) );
    }
    f.close();
    readDocumentList();
}

void Index::readDocumentList()
{
    TQFile f( docListFile );
    if ( !f.open( IO_ReadOnly ) )
	return;
    TQDataStream s( &f );
    s >> docList;
}

TQStringList Index::query( const TQStringList &terms, const TQStringList &termSeq, const TQStringList &seqWords )
{
    TermList termList;

    TQStringList::ConstIterator it = terms.begin();
    for ( it = terms.begin(); it != terms.end(); ++it ) {
	Entry *e = 0;
	if ( (*it).contains( '*' ) ) {
	    TQValueList<Document> wcts = setupDummyTerm( getWildcardTerms( *it ) );
	    termList.append( new Term( "dummy", wcts.count(), wcts ) );
	} else if ( dict[ *it ] ) {
	    e = dict[ *it ];
	    termList.append( new Term( *it, e->documents.count(), e->documents ) );
	} else {
	    return TQStringList();
	}
    }
    termList.sort();

    Term *minTerm = termList.first();
    if ( !termList.count() )
	return TQStringList();
    termList.removeFirst();

    TQValueList<Document> minDocs = minTerm->documents;
    TQValueList<Document>::iterator C;
    TQValueList<Document>::ConstIterator It;
    Term *t = termList.first();
    for ( ; t; t = termList.next() ) {
	TQValueList<Document> docs = t->documents;
	C = minDocs.begin();
	while ( C != minDocs.end() ) {
	    bool found = false;
	    for ( It = docs.begin(); It != docs.end(); ++It ) {
		if ( (*C).docNumber == (*It).docNumber ) {
		    (*C).frequency += (*It).frequency;
		    found = true;
		    break;
		}
	    }
	    if ( !found )
		C = minDocs.remove( C );
	    else
		++C;
	}
    }

    TQStringList results;
    qHeapSort( minDocs );
    if ( termSeq.isEmpty() ) {
	for ( C = minDocs.begin(); C != minDocs.end(); ++C )
	    results << docList[ (int)(*C).docNumber ];
	return results;
    }

    TQString fileName;
    for ( C = minDocs.begin(); C != minDocs.end(); ++C ) {
	fileName =  docList[ (int)(*C).docNumber ];
	if ( searchForPattern( termSeq, seqWords, fileName ) )
	    results << fileName;
    }
    return results;
}

TQString Index::getDocumentTitle( const TQString &fileName )
{
    TQFile file( fileName );
    if ( !file.open( IO_ReadOnly ) ) {
	tqWarning( "cannot open file " + fileName );
	return fileName;
    }
    TQTextStream s( &file );
    TQString text = s.read();

    int start = text.find( "<title>", 0, false ) + 7;
    int end = text.find( "</title>", 0, false );

    TQString title = ( end - start <= 0 ? tr("Untitled") : text.mid( start, end - start ) );
    return title;
}

TQStringList Index::getWildcardTerms( const TQString &term )
{
    TQStringList lst;
    TQStringList terms = split( term );
    TQValueList<TQString>::iterator iter;

    TQDictIterator<Entry> it( dict );
    for( ; it.current(); ++it ) {
	int index = 0;
	bool found = false;
	TQString text( it.currentKey() );
	for ( iter = terms.begin(); iter != terms.end(); ++iter ) {
	    if ( *iter == "*" ) {
		found = true;
		continue;
	    }
	    if ( iter == terms.begin() && (*iter)[0] != text[0] ) {
		found = false;
		break;
	    }
	    index = text.find( *iter, index );
	    if ( *iter == terms.last() && index != (int)text.length()-1 ) {
		index = text.findRev( *iter );
		if ( index != (int)text.length() - (int)(*iter).length() ) {
		    found = false;
		    break;
		}
	    }
	    if ( index != -1 ) {
		found = true;
		index += (*iter).length();
		continue;
	    } else {
		found = false;
		break;
	    }
	}
	if ( found )
	    lst << text;
    }

    return lst;
}

TQStringList Index::split( const TQString &str )
{
    TQStringList lst;
    int j = 0;
    int i = str.find( '*', j );

    while ( i != -1 ) {
	if ( i > j && i <= (int)str.length() ) {
	    lst << str.mid( j, i - j );
	    lst << "*";
	}
	j = i + 1;
	i = str.find( '*', j );
    }

    int l = str.length() - 1;
    if ( str.mid( j, l - j + 1 ).length() > 0 )
	lst << str.mid( j, l - j + 1 );

    return lst;
}

TQValueList<Document> Index::setupDummyTerm( const TQStringList &terms )
{
    TermList termList;
    TQStringList::ConstIterator it = terms.begin();
    for ( ; it != terms.end(); ++it ) {
	Entry *e = 0;
	if ( dict[ *it ] ) {
	    e = dict[ *it ];
	    termList.append( new Term( *it, e->documents.count(), e->documents ) );
	}
    }
    termList.sort();

    TQValueList<Document> maxList;

    if ( !termList.count() )
	return maxList;
    maxList = termList.last()->documents;
    termList.removeLast();

    TQValueList<Document>::iterator docIt;
    Term *t = termList.first();
    while ( t ) {
	TQValueList<Document> docs = t->documents;
	for ( docIt = docs.begin(); docIt != docs.end(); ++docIt ) {
	    if ( maxList.findIndex( *docIt ) == -1 )
		maxList.append( *docIt );
	}
	t = termList.next();
    }
    return maxList;
}

void Index::buildMiniDict( const TQString &str )
{
    if ( miniDict[ str ] )
	miniDict[ str ]->positions.append( wordNum );
    ++wordNum;
}

bool Index::searchForPattern( const TQStringList &patterns, const TQStringList &words, const TQString &fileName )
{
    TQFile file( fileName );
    if ( !file.open( IO_ReadOnly ) ) {
	tqWarning( "cannot open file " + fileName );
	return false;
    }

    wordNum = 3;
    miniDict.clear();
    TQStringList::ConstIterator cIt = words.begin();
    for ( ; cIt != words.end(); ++cIt )
	miniDict.insert( *cIt, new PosEntry( 0 ) );

    TQTextStream s( &file );
    TQString text = s.read();
    bool valid = true;
    const TQChar *buf = text.unicode();
    TQChar str[64];
    TQChar c = buf[0];
    int j = 0;
    int i = 0;
    while ( (uint)j < text.length() ) {
	if ( c == '<' || c == '&' ) {
	    valid = false;
	    if ( i > 1 )
		buildMiniDict( TQString(str,i) );
	    i = 0;
	    c = buf[++j];
	    continue;
	}
	if ( ( c == '>' || c == ';' ) && !valid ) {
	    valid = true;
	    c = buf[++j];
	    continue;
	}
	if ( !valid ) {
	    c = buf[++j];
	    continue;
	}
	if ( ( c.isLetterOrNumber() || c == '_' ) && i < 63 ) {
	    str[i] = c.lower();
	    ++i;
	} else {
	    if ( i > 1 )
		buildMiniDict( TQString(str,i) );
	    i = 0;
	}
	c = buf[++j];
    }
    if ( i > 1 )
	buildMiniDict( TQString(str,i) );
    file.close();

    TQStringList::ConstIterator patIt = patterns.begin();
    TQStringList wordLst;
    TQValueList<uint> a, b;
    TQValueList<uint>::iterator aIt;
    for ( ; patIt != patterns.end(); ++patIt ) {
	wordLst = TQStringList::split( ' ', *patIt );
	a = miniDict[ wordLst[0] ]->positions;
	for ( int j = 1; j < (int)wordLst.count(); ++j ) {
	    b = miniDict[ wordLst[j] ]->positions;
	    aIt = a.begin();
	    while ( aIt != a.end() ) {
		if ( b.find( *aIt + 1 ) != b.end() ) {
		    (*aIt)++;
		    ++aIt;
		} else {
		    aIt = a.remove( aIt );
		}
	    }
	}
    }
    if ( a.count() )
	return true;
    return false;
}
