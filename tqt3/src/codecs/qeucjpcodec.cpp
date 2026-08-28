/****************************************************************************
**
** Implementation of TQEucJpCodec class
**
** Created : 990225
**
** Copyright (C) 2000-2008 Trolltech ASA.  All rights reserved.
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

// Most of the code here was originally written by Serika Kurusugawa
// a.k.a. Junji Takagi, and is included in TQt with the author's permission,
// and the grateful thanks of the Trolltech team.

/*! \class TQEucJpCodec ntqeucjpcodec.h
    \reentrant
    \ingroup i18n

  \brief The TQEucJpCodec class provides conversion to and from EUC-JP character sets.

  More precisely, the TQEucJpCodec class subclasses TQTextCodec to
  provide support for EUC-JP, the main legacy encoding for Unix
  machines in Japan.

  The environment variable \c UNICODEMAP_JP can be used to fine-tune
  TQJisCodec, TQSjisCodec and TQEucJpCodec. The \l TQJisCodec
  documentation describes how to use this variable.

  Most of the code here was written by Serika Kurusugawa,
  a.k.a. Junji Takagi, and is included in TQt with the author's
  permission and the grateful thanks of the Trolltech team. Here is
  the copyright statement for that code:

  \legalese

  Copyright (C) 1999 Serika Kurusugawa. All rights reserved.

  Redistribution and use in source and binary forms, with or without
  modification, are permitted provided that the following conditions
  are met:
  \list 1
  \i Redistributions of source code must retain the above copyright
     notice, this list of conditions and the following disclaimer.
  \i Redistributions in binary form must reproduce the above copyright
     notice, this list of conditions and the following disclaimer in the
     documentation and/or other materials provided with the distribution.
  \endlist

  THIS SOFTWARE IS PROVIDED BY THE AUTHOR AND CONTRIBUTORS "AS IS".
  ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
  IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
  ARE DISCLAIMED.  IN NO EVENT SHALL THE REGENTS OR CONTRIBUTORS BE LIABLE
  FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
  DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
  OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
  HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
  LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
  OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
  SUCH DAMAGE.
*/

/*
 * Copyright (C) 1999 Serika Kurusugawa, All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE REGENTS OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 */

#include "ntqeucjpcodec.h"

#ifndef TQT_NO_BIG_CODECS

static const uchar Esc = 0x1b;
static const uchar Ss2 = 0x8e;	// Single Shift 2
static const uchar Ss3 = 0x8f;	// Single Shift 3

#define	IsKana(c)	(((c) >= 0xa1) && ((c) <= 0xdf))
#define	IsEucChar(c)	(((c) >= 0xa1) && ((c) <= 0xfe))

#define	TQValidChar(u)	((u) ? TQChar((ushort)(u)) : TQChar::replacement)

/*!
  Constructs a TQEucJpCodec.
*/
TQEucJpCodec::TQEucJpCodec() : conv(TQJpUnicodeConv::newConverter( TQJpUnicodeConv::Default ))
{
}

/*!
  Destroys the codec.
*/
TQEucJpCodec::~TQEucJpCodec()
{
    delete (TQJpUnicodeConv*)conv;
    conv = 0;
}

/*!
  Returns 18.
*/
int TQEucJpCodec::mibEnum() const
{
    /*
    Name: Extended_UNIX_Code_Packed_Format_for_Japanese
    MIBenum: 18
    Source: Standardized by OSF, UNIX International, and UNIX Systems
	    Laboratories Pacific.  Uses ISO 2022 rules to select
		   code set 0: US-ASCII (a single 7-bit byte set)
		   code set 1: JIS X0208-1990 (a double 8-bit byte set)
			       restricted to A0-FF in both bytes
		   code set 2: Half Width Katakana (a single 7-bit byte set)
			       requiring SS2 as the character prefix
		   code set 3: JIS X0212-1990 (a double 7-bit byte set)
			       restricted to A0-FF in both bytes
			       requiring SS3 as the character prefix
    Alias: csEUCPkdFmtJapanese
    Alias: EUC-JP  (preferred MIME name)
    */
    return 18;
}

/*!
  \reimp
*/
TQCString TQEucJpCodec::fromUnicode(const TQString& uc, int& lenInOut) const
{
    int l = TQMIN((int)uc.length(),lenInOut);
    int rlen = l*3+1;
    TQCString rstr(rlen);
    uchar* cursor = (uchar*)rstr.data();
    for (int i=0; i<l; i++) {
	TQChar ch = uc[i];
	uint j;
	if ( ch.row() == 0x00 && ch.cell() < 0x80 ) {
	    // ASCII
	    *cursor++ = ch.cell();
	} else if ((j = conv->unicodeToJisx0201(ch.row(), ch.cell())) != 0) {
	    if (j < 0x80) {
		// JIS X 0201 Latin ?
		*cursor++ = j;
	    } else {
		// JIS X 0201 Kana
		*cursor++ = Ss2;
		*cursor++ = j;
	    }
	} else if ((j = conv->unicodeToJisx0208(ch.row(), ch.cell())) != 0) {
	    // JIS X 0208
	    *cursor++ = (j >> 8)   | 0x80;
	    *cursor++ = (j & 0xff) | 0x80;
	} else if ((j = conv->unicodeToJisx0212(ch.row(), ch.cell())) != 0) {
	    // JIS X 0212
	    *cursor++ = Ss3;
	    *cursor++ = (j >> 8)   | 0x80;
	    *cursor++ = (j & 0xff) | 0x80;
	} else {
	    // Error
	    *cursor++ = '?';	// unknown char
	}
    }
    lenInOut = cursor - (uchar*)rstr.data();
    rstr.truncate(lenInOut);
    return rstr;
}

/*!
  \reimp
*/
TQString TQEucJpCodec::toUnicode(const char* chars, int len) const
{
    TQString result;
    for (int i=0; i<len; i++) {
	uchar ch = chars[i];
	if ( ch < 0x80 ) {
	    // ASCII
	    result += TQChar(ch);
	} else if ( ch == Ss2 ) {
	    // JIS X 0201 Kana
	    if ( i < len-1 ) {
		uchar c2 = chars[++i];
		if ( IsKana(c2) ) {
		    uint u = conv->jisx0201ToUnicode(c2);
		    result += TQValidChar(u);
		} else {
		    i--;
		    result += TQChar::replacement;
		}
	    }
	} else if ( ch == Ss3 ) {
	    // JIS X 0212
	    if ( i < len-1 ) {
		uchar c2 = chars[++i];
		if ( IsEucChar(c2) ) {
		    if ( i < len-1 ) {
			uchar c3 = chars[++i];
			if ( IsEucChar(c3) ) {
			    uint u = conv->jisx0212ToUnicode(c2 & 0x7f, c3 & 0x7f);
			    result += TQValidChar(u);
			} else {
			    i--;
			    result += TQChar::replacement;
			}
		    } else {
			result += TQChar::replacement;
		    }
		} else {
		    i--;
		    result += TQChar::replacement;
		}
	    } else {
		result += TQChar::replacement;
	    }
	} else if ( IsEucChar(ch) ) {
	    // JIS X 0208
	    if ( i < len-1 ) {
		uchar c2 = chars[++i];
		if ( IsEucChar(c2) ) {
		    uint u = conv->jisx0208ToUnicode(ch & 0x7f, c2 & 0x7f);
		    result += TQValidChar(u);
		} else {
		    i--;
		    result += TQChar::replacement;
		}
	    } else {
		result += TQChar::replacement;
	    }
	} else {
	    // Invalid
	    result += TQChar::replacement;
	}
    }
    return result;
}

/*!
  \reimp
*/
const char* TQEucJpCodec::name() const
{
    return "eucJP";
}

/*!
    Returns the codec's mime name.
*/
const char* TQEucJpCodec::mimeName() const
{
    return "EUC-JP";
}

/*!
  \reimp
*/
int TQEucJpCodec::heuristicNameMatch(const char* hint) const
{
    int score = 0;
    bool ja = false;
    if (tqstrnicmp(hint, "ja_JP", 5) == 0 || tqstrnicmp(hint, "japan", 5) == 0) {
	score += 3;
	ja = true;
    } else if (tqstrnicmp(hint, "ja", 2) == 0) {
	score += 2;
	ja = true;
    }
    const char *p;
    if (ja) {
	p = strchr(hint, '.');
	if (p == 0) {
	    return score;
	}
	p++;
    } else {
	p = hint;
    }
    if (p) {
	if ((tqstricmp(p, "AJEC") == 0) ||
	    (tqstricmp(p, "eucJP") == 0) ||
	    (tqstricmp(p, "ujis") == 0) ||
	    (simpleHeuristicNameMatch(p, "eucJP") > 0) ||
	    (simpleHeuristicNameMatch(p, "x-euc-jp") > 0)) {
	    return score + 4;
	}
	// there exists ja_JP.EUC, ko_KR.EUC, zh_CN.EUC and zh_TW.EUC
	// so "euc" may or may not be Japanese EUC.
	if (tqstricmp(p, "euc") == 0 && ja) {
	    return score + 4;
	}
    }
    return TQTextCodec::heuristicNameMatch(hint);
}

/*!
  \reimp
*/
int TQEucJpCodec::heuristicContentMatch(const char* chars, int len) const
{
    int score = 0;
    for (int i=0; i<len; i++) {
	uchar ch = chars[i];
	// No nulls allowed.
	if ( !ch || ch == Esc )
	    return -1;
	if ( ch < 32 && ch != '\t' && ch != '\n' && ch != '\r' ) {
	    // Suspicious
	    if ( score )
		score--;
	} else if ( ch < 0x80 ) {
	    // Inconclusive
	    score++;
	} else if ( ch == Ss2 ) {
	    // JIS X 0201 Kana
	    if ( i < len-1 ) {
		uchar c2 = chars[++i];
		if ( !IsKana(c2) )
		    return -1;
		score+=2;
	    }
	    score++;
	} else if ( ch == Ss3 ) {
	    // JIS X 0212
	    if ( i < len-1 ) {
		uchar c2 = chars[++i];
		if ( !IsEucChar(c2) )
		    return -1;
		if ( i < len-1 ) {
		    uchar c3 = chars[++i];
		    if ( !IsEucChar(c3) )
			return -1;
		    score++;
		}
		score+=2;
	    }
	    score++;
	} else if ( IsEucChar(ch) ) {
	    // JIS X 0208-1990
	    if ( i < len-1 ) {
		uchar c2 = chars[++i];
		if ( !IsEucChar(c2) )
		    return -1;
		score+=2;
	    }
	    score++;
	} else {
	    // Invalid
	    return -1;
	}
    }
    return score;
}

class TQEucJpDecoder : public TQTextDecoder {
    uchar buf[2];
    int nbuf;
    const TQJpUnicodeConv * const conv;
public:
    TQEucJpDecoder(const TQJpUnicodeConv *c) : nbuf(0), conv(c)
    {
    }

    TQString toUnicode(const char* chars, int len)
    {
	TQString result;
	for (int i=0; i<len; i++) {
	    uchar ch = chars[i];
	    switch (nbuf) {
	      case 0:
		if ( ch < 0x80 ) {
		    // ASCII
		    result += TQChar(ch);
		} else if ( ch == Ss2 || ch == Ss3 ) {
		    // JIS X 0201 Kana or JIS X 0212
		    buf[0] = ch;
		    nbuf = 1;
		} else if ( IsEucChar(ch) ) {
		    // JIS X 0208
		    buf[0] = ch;
		    nbuf = 1;
		} else {
		    // Invalid
		    result += TQChar::replacement;
		}
		break;
	      case 1:
		if ( buf[0] == Ss2 ) {
		    // JIS X 0201 Kana
		    if ( IsKana(ch) ) {
			uint u = conv->jisx0201ToUnicode(ch);
			result += TQValidChar(u);
		    } else {
			result += TQChar::replacement;
		    }
		    nbuf = 0;
		} else if ( buf[0] == Ss3 ) {
		    // JIS X 0212-1990
		    if ( IsEucChar(ch) ) {
			buf[1] = ch;
			nbuf = 2;
		    } else {
			// Error
			result += TQChar::replacement;
			nbuf = 0;
		    }
		} else {
		    // JIS X 0208-1990
		    if ( IsEucChar(ch) ) {
			uint u = conv->jisx0208ToUnicode(buf[0] & 0x7f, ch & 0x7f);
			result += TQValidChar(u);
		    } else {
			// Error
			result += TQChar::replacement;
		    }
		    nbuf = 0;
		}
		break;
	    case 2:
		// JIS X 0212
		if ( IsEucChar(ch) ) {
		    uint u = conv->jisx0212ToUnicode(buf[1] & 0x7f, ch & 0x7f);
		    result += TQValidChar(u);
		} else {
		    result += TQChar::replacement;
		}
		nbuf = 0;
	    }
	}
	return result;
    }
};

/*!
  \reimp
*/
TQTextDecoder* TQEucJpCodec::makeDecoder() const
{
    return new TQEucJpDecoder(conv);
}

#endif
