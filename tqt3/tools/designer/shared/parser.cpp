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

#include "parser.h"
#include <ntqobject.h>
#include <ntqstringlist.h>

class NormalizeObject : public TQObject
{
public:
    NormalizeObject() : TQObject() {}
    static TQCString normalizeSignalSlot( const char *signalSlot ) { return TQObject::normalizeSignalSlot( signalSlot ); }
};

TQString Parser::cleanArgs( const TQString &func )
{
    TQString slot( func );
    int begin = slot.find( "(" ) + 1;
    TQString args = slot.mid( begin );
    args = args.left( args.find( ")" ) );
    TQStringList lst = TQStringList::split( ',', args );
    TQString res = slot.left( begin );
    for ( TQStringList::Iterator it = lst.begin(); it != lst.end(); ++it ) {
	if ( it != lst.begin() )
	    res += ",";
	TQString arg = *it;
	int pos = 0;
	if ( ( pos = arg.find( "&" ) ) != -1 ) {
	    arg = arg.left( pos + 1 );
	} else if ( ( pos = arg.find( "*" ) ) != -1 ) {
	    arg = arg.left( pos + 1 );
	} else {
	    arg = arg.simplifyWhiteSpace();
	    if ( ( pos = arg.find( ':' ) ) != -1 )
		arg = arg.left( pos ).simplifyWhiteSpace() + ":" + arg.mid( pos + 1 ).simplifyWhiteSpace();
	    TQStringList l = TQStringList::split( ' ', arg );
	    if ( l.count() == 2 ) {
		if ( l[ 0 ] != "const" && l[ 0 ] != "unsigned" && l[ 0 ] != "var" )
		    arg = l[ 0 ];
	    } else if ( l.count() == 3 ) {
		arg = l[ 0 ] + " " + l[ 1 ];
	    }
	}
	res += arg;
    }	
    res += ")";

    return TQString::fromLatin1( NormalizeObject::normalizeSignalSlot( res.latin1() ) );
}
