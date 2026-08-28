/**********************************************************************
**
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

#include "syntaxhighlighter_html.h"
#include "ntqstring.h"
#include "ntqmap.h"
#include "ntqapplication.h"


SyntaxHighlighter_HTML::SyntaxHighlighter_HTML()
    : TQTextPreProcessor(), lastFormat( 0 ), lastFormatId( -1 )
{
    TQFont f( tqApp->font() );

    addFormat(Standard, new TQTextFormat(f, tqApp->palette().color(TQPalette::Active, TQColorGroup::Text)));
    addFormat(Keyword, new TQTextFormat(f, tqApp->palette().color(TQPalette::Active, TQColorGroup::Dark)));
    addFormat(Attribute, new TQTextFormat(f, tqApp->palette().color(TQPalette::Active, TQColorGroup::Link)));
    addFormat(AttribValue, new TQTextFormat(f, tqApp->palette().color(TQPalette::Active, TQColorGroup::LinkVisited)));
}

SyntaxHighlighter_HTML::~SyntaxHighlighter_HTML()
{
}

void SyntaxHighlighter_HTML::process( TQTextDocument *doc, TQTextParagraph *string, int, bool invalidate )
{

    TQTextFormat *formatStandard = format( Standard );
    TQTextFormat *formatKeyword = format( Keyword );
    TQTextFormat *formatAttribute = format( Attribute );
    TQTextFormat *formatAttribValue = format( AttribValue );

    const int StateStandard  = 0;
    const int StateTag       = 1;
    const int StateAttribute = 2;
    const int StateAttribVal = 3;

    TQString buffer = "";

    int state = StateStandard;


    if ( string->prev() ) {
	if ( string->prev()->endState() == -1 )
	    process( doc, string->prev(), 0, false );
	state = string->prev()->endState();
    }


    int i = 0;
    for ( ;; ) {
	TQChar c = string->at( i )->c;

	if ( c == '<' ) {
	    if ( state != StateStandard  )
		string->setFormat( i - buffer.length(), buffer.length(), formatStandard, false );
	    buffer = c;
	    state = StateTag;
	    string->setFormat( i, 1, formatKeyword, false );
	}
	else if ( c == '>' && ( state != StateStandard ) ) {
	    string->setFormat( i, 1, formatKeyword, false );
	    buffer = "";
	    state = StateStandard;
	}
	else if ( c == ' ' && state == StateTag ) {
	    buffer += c;
	    string->setFormat( i, 1, formatStandard, false );
	    state = StateAttribute;
	}
	else if ( c == '=' && state == StateAttribute ) {
	    buffer += c;
	    string->setFormat( i, 1, formatStandard, false );
	    state = StateAttribute;
	}
	else if ( c == '\"' && state == StateAttribute ) {
	    buffer += c;
	    string->setFormat( i, 1, formatStandard, false );
	    state = StateAttribVal;
	}
	else if ( c == '\"' && state == StateAttribVal ) {
	    buffer += c;
	    string->setFormat( i, 1, formatStandard, false );
	    state = StateAttribute;
	}
	else if ( state == StateAttribute ) {
	    buffer += c;
	    string->setFormat( i, 1, formatAttribute, false );
	}
	else if ( state == StateAttribVal ) {
	    buffer += c;
	    string->setFormat( i, 1, formatAttribValue, false );
	}
	else if ( state == StateTag ) {
	    string->setFormat( i, 1, formatKeyword, false );
	    buffer += c;
	}
	else if ( state == StateStandard ) {
	    string->setFormat( i, 1, formatStandard, false );
	}

	i++;
	if ( i >= string->length() )
	    break;
    }

    string->setEndState( state );
    string->setFirstPreProcess( false );

    if ( invalidate && string->next() &&
	 !string->next()->firstPreProcess() && string->next()->endState() != -1 ) {
	TQTextParagraph *p = string->next();
	while ( p ) {
	    if ( p->endState() == -1 )
		return;
	    p->setEndState( -1 );
	    p = p->next();
	}
    }
}

TQTextFormat *SyntaxHighlighter_HTML::format( int id )
{
    if ( lastFormatId == id  && lastFormat )
	return lastFormat;

    TQTextFormat *f = formats[ id ];
    lastFormat = f ? f : formats[ 0 ];
    lastFormatId = id;
    return lastFormat;
}

void SyntaxHighlighter_HTML::addFormat( int id, TQTextFormat *f )
{
    formats.insert( id, f );
}
