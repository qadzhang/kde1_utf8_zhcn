/****************************************************************************
**
** Hong Kong Font utilities for X11
**
** Created : 20020902
**
** Copyright (C) 1992-2008 Trolltech ASA.  All rights reserved.
**
** This file is part of the kernel module of the TQt GUI Toolkit.
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

#include "private/qfontcodecs_p.h"

#ifndef TQT_NO_CODECS
#ifndef TQT_NO_BIG_CODECS

extern int qt_UnicodeToBig5hkscs(uint wc, uchar *r);


int TQFontBig5hkscsCodec::heuristicContentMatch(const char *, int) const
{
    return 0;
}


int TQFontBig5hkscsCodec::heuristicNameMatch(const char* hint) const
{
    //tqDebug("TQFontBig5hkscsCodec::heuristicNameMatch(const char* hint = \"%s\")", hint);
    return ( tqstricmp(hint, "big5hkscs-0") == 0 ||
	     tqstricmp(hint, "hkscs-1") == 0 )
	? 13 : 0;
}


TQFontBig5hkscsCodec::TQFontBig5hkscsCodec()
{
    //tqDebug("TQFontBig5hkscsCodec::TQFontBig5hkscsCodec()");
}


const char* TQFontBig5hkscsCodec::name() const
{
    //tqDebug("TQFontBig5hkscsCodec::name() = \"big5hkscs-0\"");
    return "big5hkscs-0";
}


int TQFontBig5hkscsCodec::mibEnum() const
{
    //tqDebug("TQFontBig5hkscsCodec::mibEnum() = -2101");
    return -2101;
}


TQString TQFontBig5hkscsCodec::toUnicode(const char* /*chars*/, int /*len*/) const
{
    return TQString::null;
}

unsigned short
TQFontBig5hkscsCodec::characterFromUnicode(const TQString &str, int pos) const
{
    uchar c[2];
    if (qt_UnicodeToBig5hkscs((str.unicode() + pos)->unicode(), c) == 2)
        return (c[0] << 8) + c[1];
    return 0;
}

TQCString TQFontBig5hkscsCodec::fromUnicode(const TQString& uc, int& lenInOut ) const
{
    //tqDebug("TQFontBig5hkscsCodec::fromUnicode(const TQString& uc, int& lenInOut = %d)", lenInOut);
    TQCString result(lenInOut * 2 + 1);
    uchar *rdata = (uchar *) result.data();
    const TQChar *ucp = uc.unicode();

    for ( int i = 0; i < lenInOut; i++ ) {
	TQChar ch(*ucp++);
	uchar c[2];

#if 0
	if ( ch.row() == 0) {
	    if ( ch.cell() == ' ' )
		ch = TQChar( 0x3000 );
	    else if ( ch.cell() > ' ' && ch.cell() < 127 )
		ch = TQChar( ch.cell()-' ', 255 );
	}
#endif
	if ( qt_UnicodeToBig5hkscs( ch.unicode(), c ) == 2) {
	    *rdata++ = c[0];
	    *rdata++ = c[1];
	} else {
	    //white square
	    *rdata++ = 0xa1;
	    *rdata++ = 0xbc;
	}
    }
    lenInOut *=2;
    return result;
}


/*! \internal */
void TQFontBig5hkscsCodec::fromUnicode(const TQChar *in, unsigned short *out, int length) const
{
    uchar c[2];
    while (length--) {
	if ( in->row() == 0x00 && in->cell() < 0x80 ) {
	    // ASCII
	    *out = in->cell();
	} else if ( qt_UnicodeToBig5hkscs( in->unicode(), c ) == 2 ) {
	    // Big5-HKSCS
	    *out = (c[0] << 8) | c[1];
	} else {
	    // Unknown char
	    *out = 0;
	}

	++in;
	++out;
    }
}

bool TQFontBig5hkscsCodec::canEncode( TQChar ch ) const
{
    //tqDebug("TQFontBig5hkscsCodec::canEncode( TQChar ch = %02X%02X )", ch.row(), ch.cell());
    uchar c[2];
    return ( qt_UnicodeToBig5hkscs( ch.unicode(), c ) == 2 );
}

#endif // TQT_NO_BIG_CODECS
#endif // TQT_NO_CODECS
