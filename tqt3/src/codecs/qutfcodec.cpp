/****************************************************************************
**
** Implementation of TQUtf{8,16}Codec class
**
** Created : 981015
**
** Copyright (C) 1998-2008 Trolltech ASA.  All rights reserved.
**
** This file is part of the tools module of the TQt GUI Toolkit.
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

#include "ntqutfcodec.h"

#ifndef TQT_NO_TEXTCODEC

int TQUtf8Codec::mibEnum() const
{
    return 106;
}

TQCString TQUtf8Codec::fromUnicode(const TQString& uc, int& lenInOut) const
{
    int l = uc.length();
    if (lenInOut > 0)
	l = TQMIN(l, lenInOut);
    int rlen = l*3+1;
    TQCString rstr(rlen);
    uchar* cursor = (uchar*)rstr.data();
    const TQChar *ch = uc.unicode();
    for (int i=0; i < l; i++) {
	uint u = ch->unicode();
 	if ( u < 0x80 ) {
 	    *cursor++ = (uchar)u;
 	} else {
 	    if ( u < 0x0800 ) {
		*cursor++ = 0xc0 | ((uchar) (u >> 6));
 	    } else {
		if (ch[0].isHighSurrogate() && i < (l - 1) && ch[1].isLowSurrogate()) {
		    u = TQChar::surrogateToUcs4(ch[0], ch[1]);
		    ++ch;
		    ++i;
		}
		if (u > 0xffff) {
		    // see TQString::fromUtf8() and TQString::utf8() for explanations
		    if (u > 0x10fe00 && u < 0x10ff00) {
			*cursor++ = (u - 0x10fe00);
			++ch;
			continue;
		    } else {
			*cursor++ = 0xf0 | ((uchar) (u >> 18));
			*cursor++ = 0x80 | ( ((uchar) (u >> 12)) & 0x3f);
		    }
		} else {
		    *cursor++ = 0xe0 | ((uchar) (u >> 12));
		}
 		*cursor++ = 0x80 | ( ((uchar) (u >> 6)) & 0x3f);
 	    }
 	    *cursor++ = 0x80 | ((uchar) (u&0x3f));
 	}
 	++ch;
    }
    *cursor = 0;
    lenInOut = cursor - (uchar*)rstr.data();
    ((TQByteArray&)rstr).resize(lenInOut+1);
    return rstr;
}

TQString TQUtf8Codec::toUnicode(const char* chars, int len) const
{
    if (len > 3 && (uchar)chars[0] == 0xef && (uchar)chars[1] == 0xbb && (uchar)chars[2] == 0xbf) {
	// starts with a byte order mark
	chars += 3;
	len -= 3;
    }
    return TQString::fromUtf8( chars, len );
}


const char* TQUtf8Codec::name() const
{
    return "UTF-8";
}

int TQUtf8Codec::heuristicContentMatch(const char* chars, int len) const
{
    int score = 0;
    for (int i=0; i<len; i++) {
	uchar ch = chars[i];
	// No nulls allowed.
	if ( !ch )
	    return -1;
	if ( ch < 128 ) {
	    // Inconclusive
	    score++;
	} else if ( (ch&0xe0) == 0xc0 ) {
	    if ( i < len-1 ) {
		uchar c2 = chars[++i];
		if ( (c2&0xc0) != 0x80 )
		    return -1;
		score+=3;
	    }
	} else if ( (ch&0xf0) == 0xe0 ) {
	    if ( i < len-1 ) {
		uchar c2 = chars[++i];
		if ( (c2&0xc0) != 0x80 ) {
		    return -1;
#if 0
		    if ( i < len-1 ) {
			uchar c3 = chars[++i];
			if ( (c3&0xc0) != 0x80 )
			    return -1;
			score+=3;
		    }
#endif
		}
		score+=2;
	    }
	}
    }
    return score;
}




class TQUtf8Decoder : public TQTextDecoder {
    uint uc;
    uint min_uc;
    int need;
    bool headerDone;
public:
    TQUtf8Decoder() : need(0), headerDone(false)
    {
    }

    TQString toUnicode(const char* chars, int len)
    {
	TQString result;
	result.setLength( len + 1 ); // worst case
	TQChar *qch = (TQChar *)result.unicode();
	uchar ch;
        int error = -1;
	for (int i=0; i<len; i++) {
	    ch = chars[i];
	    if (need) {
		if ( (ch&0xc0) == 0x80 ) {
		    uc = (uc << 6) | (ch & 0x3f);
		    need--;
		    if ( !need ) {
			if (TQChar::requiresSurrogates(uc)) {
			    // surrogate pair
			    *qch++ = TQChar(TQChar::highSurrogate(uc));
			    *qch++ = TQChar(TQChar::lowSurrogate(uc));
			    headerDone = true;
			} else if ((uc < min_uc) || (uc >= 0xd800 && uc <= 0xdfff) || (uc >= 0xfffe)) {
			    // overlong sequence, UTF16 surrogate or BOM
			    *qch++ = TQChar::replacement;
			} else {
			    if (headerDone || TQChar(uc) != TQChar::byteOrderMark)
				*qch++ = uc;
			    headerDone = true;
			}
		    }
		} else {
		    // error
                    i = error;
		    *qch++ = TQChar::replacement;
		    need = 0;
		}
	    } else {
		if ( ch < 128 ) {
		    *qch++ = ch;
		    headerDone = true;
		} else if ((ch & 0xe0) == 0xc0) {
		    uc = ch & 0x1f;
		    need = 1;
                    error = i;
		    min_uc = 0x80;
		} else if ((ch & 0xf0) == 0xe0) {
		    uc = ch & 0x0f;
		    need = 2;
                    error = i;
		    min_uc = 0x800;
		} else if ((ch&0xf8) == 0xf0) {
		    uc = ch & 0x07;
		    need = 3;
                    error = i;
                    min_uc = 0x10000;
                } else {
                    // error
                    *qch++ = TQChar::replacement;
                }
	    }
	}
	result.truncate( qch - result.unicode() );
	return result;
    }
};

TQTextDecoder* TQUtf8Codec::makeDecoder() const
{
    return new TQUtf8Decoder;
}






int TQUtf16Codec::mibEnum() const
{
    return 1000;
}

const char* TQUtf16Codec::name() const
{
    return "ISO-10646-UCS-2";
}

int TQUtf16Codec::heuristicContentMatch(const char* chars, int len) const
{
    uchar* uchars = (uchar*)chars;
    if ( len >= 2 && ((uchars[0] == 0xff && uchars[1] == 0xfe) ||
		      (uchars[1] == 0xff && uchars[0] == 0xfe)) )
	return len;
    else
	return 0;
}




class TQUtf16Encoder : public TQTextEncoder {
    bool headerdone;
public:
    TQUtf16Encoder() : headerdone(false)
    {
    }

    TQCString fromUnicode(const TQString& uc, int& lenInOut)
    {
	if ( headerdone ) {
	    lenInOut = uc.length()*sizeof(TQChar);
	    TQCString d(lenInOut);
	    memcpy(d.data(),uc.unicode(),lenInOut);
	    return d;
	} else {
	    headerdone = true;
	    lenInOut = (1+uc.length())*sizeof(TQChar);
	    TQCString d(lenInOut);
	    memcpy(d.data(),&TQChar::byteOrderMark,sizeof(TQChar));
	    memcpy(d.data()+sizeof(TQChar),uc.unicode(),uc.length()*sizeof(TQChar));
	    return d;
	}
    }
};

class TQUtf16Decoder : public TQTextDecoder {
    uchar buf;
    bool half;
    bool swap;
    bool headerdone;

public:
    TQUtf16Decoder() : half(false), swap(false), headerdone(false)
    {
    }

    TQString toUnicode(const char* chars, int len)
    {
	TQString result;
	result.setLength( len + 1 ); // worst case
	TQChar *qch = (TQChar *)result.unicode();
	TQChar ch;
	while ( len-- ) {
	    if ( half ) {
		if ( swap ) {
		    ch.setRow( *chars++ );
		    ch.setCell( buf );
		} else {
		    ch.setRow( buf );
		    ch.setCell( *chars++ );
		}
		if ( !headerdone ) {
		    if ( ch == TQChar::byteOrderSwapped ) {
			swap = !swap;
		    } else if ( ch == TQChar::byteOrderMark ) {
			// Ignore ZWNBSP
		    } else {
			*qch++ = ch;
		    }
		    headerdone = true;
		} else
		    *qch++ = ch;
		half = false;
	    } else {
		buf = *chars++;
		half = true;
	    }
	}
	result.truncate( qch - result.unicode() );
	return result;
    }
};

TQTextDecoder* TQUtf16Codec::makeDecoder() const
{
    return new TQUtf16Decoder;
}

TQTextEncoder* TQUtf16Codec::makeEncoder() const
{
    return new TQUtf16Encoder;
}

#endif //TQT_NO_TEXTCODEC
