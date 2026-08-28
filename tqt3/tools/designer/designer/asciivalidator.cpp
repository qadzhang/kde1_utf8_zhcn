/**********************************************************************
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

#include "asciivalidator.h"

#include <ntqstring.h>

AsciiValidator::AsciiValidator( TQObject * parent, const char *name )
    : TQValidator( parent, name ), functionName( false )
{
}

AsciiValidator::AsciiValidator( bool funcName, TQObject * parent, const char *name )
    : TQValidator( parent, name ), functionName( funcName )
{
}

AsciiValidator::AsciiValidator( const TQString &allow, TQObject * parent, const char *name )
    : TQValidator( parent, name ), functionName( false ), allowedChars( allow )
{
}

AsciiValidator::~AsciiValidator()
{
}

TQValidator::State AsciiValidator::validate( TQString &s, int & ) const
{
    bool inParen = false;
    bool outParen = false;
    if ( !s.isEmpty() && s[0].row() == 0 && s[0].cell() >= '0' && s[0].cell() <= '9' )
	s[0] = '_';
    for ( int i = 0, j = 0; i < (int) s.length(); i++ ) {
	uchar r = s[i].row();
	uchar c = s[i].cell();

	if ( outParen ) { // check if we have 'const' or 'volatile'
	    static const TQString con = " const";
	    static const TQString vol = " volatile";
	    TQString mid = s.mid( j );
	    if ( !( con.startsWith( mid ) || vol.startsWith( mid ) ) )
		return TQValidator::Invalid;
	}

	if ( inParen && c != ')' )
	    continue;

	if ( r == 0 && ( ( c >= '0' && c <= '9' ) ||
			 ( c >= 'a' && c <= 'z' ) ||
			 ( c >= 'A' && c <= 'Z' ) ) )
	    continue;
	
	if ( functionName ) {
	    if ( c == '(' ) {
		inParen = true;
		continue;
	    }
	    if ( c == ')' ) {
		outParen = true;
		j = i + 1;
		continue;
	    }
	}
	
	if ( allowedChars.find( s[ i ] ) != -1 )
	    continue;
	
	s[i] = '_';
    }
    return TQValidator::Acceptable;
}
