/****************************************************************************
**
** Implementation of platform specific TQFontDatabase
**
** Created : 970521
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

#include <qplatformdefs.h>

#include <ntqdatetime.h>
#include <ntqpaintdevicemetrics.h>

#include "qt_x11_p.h"

#include <ctype.h>
#include <stdlib.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/mman.h>

#ifndef TQT_NO_XFTFREETYPE
#include <ft2build.h>
#include FT_FREETYPE_H
#endif

#ifdef TQFONTDATABASE_DEBUG
#  define FD_DEBUG tqDebug
#else
#  define FD_DEBUG if (false) tqDebug
#endif // TQFONTDATABASE_DEBUG

// from qfont_x11.cpp
extern double tqt_pointSize(double pixelSize, TQPaintDevice *paintdevice, int screen);
extern double tqt_pixelSize(double pointSize, TQPaintDevice *paintdevice, int screen);


static inline void capitalize ( char *s )
{
    bool space = true;
    while( *s ) {
	if ( space )
	    *s = toupper( *s );
	space = ( *s == ' ' );
	++s;
    }
}


// ----- begin of generated code -----

#define make_tag( c1, c2, c3, c4 ) \
( (((unsigned int)c1)<<24) | (((unsigned int)c2)<<16) | \
(((unsigned int)c3)<<8) | ((unsigned int)c4) )

struct XlfdEncoding {
    const char *name;
    int id;
    int mib;
    unsigned int hash1;
    unsigned int hash2;
};

static const XlfdEncoding xlfd_encoding[] = {
    { "iso8859-1", 0, 4, make_tag('i','s','o','8'), make_tag('5','9','-','1') },
    { "iso8859-2", 1, 5, make_tag('i','s','o','8'), make_tag('5','9','-','2') },
    { "iso8859-3", 2, 6, make_tag('i','s','o','8'), make_tag('5','9','-','3') },
    { "iso8859-4", 3, 7, make_tag('i','s','o','8'), make_tag('5','9','-','4') },
    { "iso8859-9", 4, 12, make_tag('i','s','o','8'), make_tag('5','9','-','9') },
    { "iso8859-10", 5, 13, make_tag('i','s','o','8'), make_tag('9','-','1','0') },
    { "iso8859-13", 6, 109, make_tag('i','s','o','8'), make_tag('9','-','1','3') },
    { "iso8859-14", 7, 110, make_tag('i','s','o','8'), make_tag('9','-','1','4') },
    { "iso8859-15", 8, 111, make_tag('i','s','o','8'), make_tag('9','-','1','5') },
    { "hp-roman8", 9, 2004, make_tag('h','p','-','r'), make_tag('m','a','n','8') },
    { "jisx0208*-0", 10, 63, make_tag('j','i','s','x'), 0 },
#define LAST_LATIN_ENCODING 10
    { "iso8859-5", 11, 8, make_tag('i','s','o','8'), make_tag('5','9','-','5') },
    { "*-cp1251", 12, 2251, 0, make_tag('1','2','5','1') },
    { "koi8-ru", 13, 2084, make_tag('k','o','i','8'), make_tag('8','-','r','u') },
    { "koi8-u", 14, 2088, make_tag('k','o','i','8'), make_tag('i','8','-','u') },
    { "koi8-r", 15, 2084, make_tag('k','o','i','8'), make_tag('i','8','-','r') },
    { "iso8859-7", 16, 10, make_tag('i','s','o','8'), make_tag('5','9','-','7') },
    { "iso10646-1", 17, 0, make_tag('i','s','o','1'), make_tag('4','6','-','1') },
    { "iso8859-8", 18, 85, make_tag('i','s','o','8'), make_tag('5','9','-','8') },
    { "gb18030-0", 19, -114, make_tag('g','b','1','8'), make_tag('3','0','-','0') },
    { "gb18030.2000-0", 20, -113, make_tag('g','b','1','8'), make_tag('0','0','-','0') },
    { "gbk-0", 21, -113, make_tag('g','b','k','-'), make_tag('b','k','-','0') },
    { "gb2312.*-0", 22, 57, make_tag('g','b','2','3'), 0 },
    { "jisx0201*-0", 23, 15, make_tag('j','i','s','x'), 0 },
    { "ksc5601*-*", 24, 36, make_tag('k','s','c','5'), 0 },
    { "big5hkscs-0", 25, -2101, make_tag('b','i','g','5'), make_tag('c','s','-','0') },
    { "hkscs-1", 26, -2101, make_tag('h','k','s','c'), make_tag('c','s','-','1') },
    { "big5*-*", 27, -2026, make_tag('b','i','g','5'), 0 },
    { "tscii-*", 28, 2028, make_tag('t','s','c','i'), 0 },
    { "tis620*-*", 29, 2259, make_tag('t','i','s','6'), 0 },
    { "iso8859-11", 30, 2259, make_tag('i','s','o','8'), make_tag('9','-','1','1') },
    { "mulelao-1", 31, -4242, make_tag('m','u','l','e'), make_tag('a','o','-','1') },
    { "ethiopic-unicode", 32, 0, make_tag('e','t','h','i'), make_tag('c','o','d','e') },
    { "unicode-*", 33, 0, make_tag('u','n','i','c'), 0 },
    { "*-symbol", 34, 0, 0, make_tag('m','b','o','l') },
    { "*-fontspecific", 35, 0, 0, make_tag('i','f','i','c') },
    { "fontspecific-*", 36, 0, make_tag('f','o','n','t'), 0 },
    { 0, 0, 0, 0, 0 }
};

static const char scripts_for_xlfd_encoding[37][61] = {
    // iso8859-1
    { 1, 0, 0, 0, 0, 0, 0, 0, 0, 0,
      0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
      0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
      0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
      0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
      0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
      0 },
    // iso8859-2
    { 1, 0, 0, 0, 0, 0, 0, 0, 0, 0,
      0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
      0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
      0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
      0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
      0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
      0 },
    // iso8859-3
    { 1, 0, 0, 0, 0, 0, 0, 0, 0, 0,
      0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
      0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
      0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
      0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
      0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
      0 },
    // iso8859-4
    { 1, 0, 0, 0, 0, 0, 0, 0, 0, 0,
      0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
      0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
      0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
      0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
      0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
      0 },
    // iso8859-9
    { 1, 0, 0, 0, 0, 0, 0, 0, 0, 0,
      0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
      0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
      0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
      0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
      0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
      0 },
    // iso8859-10
    { 1, 0, 0, 0, 0, 0, 0, 0, 0, 0,
      0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
      0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
      0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
      0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
      0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
      0 },
    // iso8859-13
    { 1, 0, 0, 0, 0, 0, 0, 0, 0, 0,
      0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
      0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
      0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
      0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
      0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
      0 },
    // iso8859-14
    { 1, 0, 0, 0, 0, 0, 0, 0, 0, 0,
      0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
      0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
      0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
      0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
      0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
      0 },
    // iso8859-15
    { 1, 0, 0, 0, 0, 0, 0, 0, 0, 0,
      0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
      0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
      0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
      0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
      0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
      0 },
    // hp-roman8
    { 1, 0, 0, 0, 0, 0, 0, 0, 0, 0,
      0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
      0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
      0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
      0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
      0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
      0 },
    // jisx0208*-0
    { 1, 0, 0, 0, 0, 0, 0, 0, 0, 0,
      0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
      0, 0, 0, 0, 0, 0, 0, 0, 1, 1,
      1, 0, 0, 0, 0, 0, 0, 0, 0, 0,
      0, 0, 0, 0, 0, 0, 0, 1, 0, 0,
      0, 0, 0, 0, 0, 0, 0, 1, 0, 0,
      0 },
    // iso8859-5
    { 0, 0, 1, 0, 0, 0, 0, 0, 0, 0,
      0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
      0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
      0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
      0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
      0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
      0 },
    // *-cp1251
    { 0, 0, 1, 0, 0, 0, 0, 0, 0, 0,
      0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
      0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
      0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
      0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
      0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
      0 },
    // koi8-ru
    { 0, 0, 1, 0, 0, 0, 0, 0, 0, 0,
      0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
      0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
      0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
      0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
      0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
      0 },
    // koi8-u
    { 0, 0, 1, 0, 0, 0, 0, 0, 0, 0,
      0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
      0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
      0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
      0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
      0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
      0 },
    // koi8-r
    { 0, 0, 1, 0, 0, 0, 0, 0, 0, 0,
      0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
      0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
      0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
      0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
      0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
      0 },
    // iso8859-7
    { 0, 1, 0, 0, 0, 0, 0, 0, 0, 0,
      0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
      0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
      0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
      0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
      0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
      0 },
    // iso10646-1
    { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
      1, 0, 0, 0, 0, 0, 0, 0, 0, 0,
      0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
      0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
      0, 0, 0, 0, 0, 0, 0, 1, 0, 0,
      0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
      0 },
    // iso8859-8
    { 0, 0, 0, 0, 0, 0, 0, 0, 0, 1,
      0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
      0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
      0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
      0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
      0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
      0 },
    // gb18030-0
    { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
      0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
      0, 0, 0, 0, 0, 0, 0, 0, 1, 0,
      0, 0, 1, 0, 0, 0, 0, 0, 0, 0,
      0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
      0, 0, 0, 0, 0, 0, 0, 0, 1, 0,
      0 },
    // gb18030.2000-0
    { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
      0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
      0, 0, 0, 0, 0, 0, 0, 0, 1, 0,
      0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
      0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
      0, 0, 0, 0, 0, 0, 0, 0, 1, 0,
      0 },
    // gbk-0
    { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
      0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
      0, 0, 0, 0, 0, 0, 0, 0, 1, 0,
      0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
      0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
      0, 0, 0, 0, 0, 0, 0, 0, 1, 0,
      0 },
    // gb2312.*-0
    { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
      0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
      0, 0, 0, 0, 0, 0, 0, 0, 1, 0,
      0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
      0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
      0, 0, 0, 0, 0, 0, 0, 0, 1, 0,
      0 },
    // jisx0201*-0
    { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
      0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
      0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
      0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
      0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
      0, 0, 1, 0, 0, 0, 0, 0, 0, 0,
      0 },
    // ksc5601*-*
    { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
      0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
      0, 0, 0, 0, 0, 0, 0, 0, 1, 0,
      0, 1, 0, 0, 0, 0, 0, 0, 0, 0,
      0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
      0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
      1 },
    // big5hkscs-0
    { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
      0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
      0, 0, 0, 0, 0, 0, 0, 0, 1, 0,
      0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
      0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
      0, 0, 0, 0, 0, 0, 0, 0, 0, 1,
      0 },
    // hkscs-1
    { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
      0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
      0, 0, 0, 0, 0, 0, 0, 0, 1, 0,
      0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
      0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
      0, 0, 0, 0, 0, 0, 0, 0, 0, 1,
      0 },
    // big5*-*
    { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
      0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
      0, 0, 0, 0, 0, 0, 0, 0, 1, 0,
      0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
      0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
      0, 0, 0, 0, 0, 0, 0, 0, 0, 1,
      0 },
    // tscii-*
    { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
      0, 0, 0, 0, 0, 0, 0, 0, 1, 0,
      0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
      0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
      0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
      0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
      0 },
    // tis620*-*
    { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
      0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
      0, 0, 0, 1, 0, 0, 0, 0, 0, 0,
      0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
      0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
      0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
      0 },
    // iso8859-11
    { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
      0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
      0, 0, 0, 1, 0, 0, 0, 0, 0, 0,
      0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
      0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
      0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
      0 },
    // mulelao-1
    { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
      0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
      0, 0, 0, 0, 1, 0, 0, 0, 0, 0,
      0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
      0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
      0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
      0 },
    // ethiopic-unicode
    { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
      0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
      0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
      0, 0, 0, 0, 1, 0, 0, 0, 0, 0,
      0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
      0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
      0 },
    // unicode-*
    { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
      0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
      0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
      0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
      0, 0, 0, 0, 0, 0, 0, 1, 0, 0,
      0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
      0 },
    // *-symbol
    { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
      0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
      0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
      0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
      0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
      0, 0, 0, 0, 0, 1, 0, 0, 0, 0,
      0 },
    // *-fontspecific
    { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
      0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
      0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
      0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
      0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
      0, 0, 0, 0, 0, 1, 0, 0, 0, 0,
      0 },
    // fontspecific-*
    { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
      0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
      0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
      0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
      0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
      0, 0, 0, 0, 0, 1, 0, 0, 0, 0,
      0 }

};

// ----- end of generated code -----


const int numEncodings = sizeof( xlfd_encoding ) / sizeof( XlfdEncoding ) - 1;

int tqt_xlfd_encoding_id( const char *encoding )
{
    // tqDebug("looking for encoding id for '%s'", encoding );
    int len = strlen( encoding );
    if ( len < 4 )
	return -1;
    unsigned int hash1 = make_tag( encoding[0], encoding[1], encoding[2], encoding[3] );
    const char *ch = encoding + len - 4;
    unsigned int hash2 = make_tag( ch[0], ch[1], ch[2], ch[3] );

    const XlfdEncoding *enc = xlfd_encoding;
    for ( ; enc->name; ++enc ) {
	if ( (enc->hash1 && enc->hash1 != hash1) ||
	     (enc->hash2 && enc->hash2 != hash2) )
	    continue;
	// hashes match, do a compare if strings match
	// the enc->name can contain '*'s we have to interpret correctly
	const char *n = enc->name;
	const char *e = encoding;
	while ( 1 ) {
 	    // tqDebug("bol: *e='%c', *n='%c'", *e,  *n );
	    if ( *e == '\0' ) {
		if ( *n )
		    break;
		// tqDebug( "found encoding id %d", enc->id );
		return enc->id;
	    }
	    if ( *e == *n ) {
		++e;
		++n;
		continue;
	    }
	    if ( *n != '*' )
		break;
	    ++n;
 	    // tqDebug("skip: *e='%c', *n='%c'", *e,  *n );
	    while ( *e && *e != *n )
		++e;
	}
    }
    // tqDebug( "couldn't find encoding %s", encoding );
    return -1;
}

int tqt_mib_for_xlfd_encoding( const char *encoding )
{
    int id = tqt_xlfd_encoding_id( encoding );
    if ( id != -1 ) return xlfd_encoding[id].mib;
    return 0;
}

int tqt_encoding_id_for_mib( int mib )
{
    const XlfdEncoding *enc = xlfd_encoding;
    for ( ; enc->name; ++enc ) {
	if ( enc->mib == mib )
	    return enc->id;
    }
    return -1;
}

static const char * xlfd_for_id( int id )
{
    // special case: -1 returns the "*-*" encoding, allowing us to do full
    // database population in a single X server round trip.
    if ( id < 0 || id > numEncodings )
	return "*-*";
    return xlfd_encoding[id].name;
}

enum XLFDFieldNames {
    Foundry,
    Family,
    Weight,
    Slant,
    Width,
    AddStyle,
    PixelSize,
    PointSize,
    ResolutionX,
    ResolutionY,
    Spacing,
    AverageWidth,
    CharsetRegistry,
    CharsetEncoding,
    NFontFields
};

// Splits an X font name into fields separated by '-'
static bool parseXFontName( char *fontName, char **tokens )
{
    if ( ! fontName || fontName[0] == '0' || fontName[0] != '-' ) {
	tokens[0] = 0;
	return false;
    }

    int	  i;
    ++fontName;
    for ( i = 0; i < NFontFields && fontName && fontName[0]; ++i ) {
	tokens[i] = fontName;
	for ( ;; ++fontName ) {
	    if ( *fontName == '-' )
		break;
	    if ( ! *fontName ) {
		fontName = 0;
		break;
	    }
	}

	if ( fontName ) *fontName++ = '\0';
    }

    if ( i < NFontFields ) {
	for ( int j = i ; j < NFontFields; ++j )
	    tokens[j] = 0;
	return false;
    }

    return true;
}

static inline bool isZero(char *x)
{
    return (x[0] == '0' && x[1] == 0);
}

static inline bool isScalable( char **tokens )
{
    return (isZero(tokens[PixelSize]) &&
	    isZero(tokens[PointSize]) &&
	    isZero(tokens[AverageWidth]));
}

static inline bool isSmoothlyScalable( char **tokens )
{
    return (isZero(tokens[ResolutionX]) &&
	    isZero(tokens[ResolutionY]));
}

static inline bool isFixedPitch( char **tokens )
{
    return (tokens[Spacing][0] == 'm' ||
	    tokens[Spacing][0] == 'c' ||
	    tokens[Spacing][0] == 'M' ||
	    tokens[Spacing][0] == 'C');
}

/*
  Fills in a font definition (TQFontDef) from an XLFD (X Logical Font
  Description).

  Returns true if the the given xlfd is valid.  The fields lbearing
  and rbearing are not given any values.
*/
bool tqt_fillFontDef( const TQCString &xlfd, TQFontDef *fd, int screen )
{
    char *tokens[NFontFields];
    TQCString buffer = xlfd.copy();
    if ( ! parseXFontName(buffer.data(), tokens) )
	return false;

    capitalize(tokens[Family]);
    capitalize(tokens[Foundry]);

    fd->family = TQString::fromLatin1(tokens[Family]);
    TQString foundry = TQString::fromLatin1(tokens[Foundry]);
    if ( ! foundry.isEmpty() && foundry != TQString::fromLatin1("*") )
	fd->family +=
	    TQString::fromLatin1(" [") + foundry + TQString::fromLatin1("]");

    if ( tqstrlen( tokens[AddStyle] ) > 0 )
	fd->addStyle = TQString::fromLatin1(tokens[AddStyle]);
    else
	fd->addStyle = TQString::null;

    fd->pointSize = atoi(tokens[PointSize]);
    fd->styleHint = TQFont::AnyStyle;	// ### any until we match families

    char slant = tolower( (uchar) tokens[Slant][0] );
    fd->italic = ( slant == 'o' || slant == 'i' );
    char fixed = tolower( (uchar) tokens[Spacing][0] );
    fd->fixedPitch = ( fixed == 'm' || fixed == 'c' );
    fd->weight = getFontWeight( tokens[Weight] );

    int r = atoi(tokens[ResolutionY]);
    fd->pixelSize = atoi(tokens[PixelSize]);
    // not "0" or "*", or required DPI
    if ( r && fd->pixelSize && TQPaintDevice::x11AppDpiY( screen ) &&
	 r != TQPaintDevice::x11AppDpiY( screen ) ) {
	// calculate actual pointsize for display DPI
	fd->pointSize = tqRound(tqt_pointSize(fd->pixelSize, 0, screen) * 10.);
    } else if ( fd->pixelSize == 0 && fd->pointSize ) {
	// calculate pixel size from pointsize/dpi
	fd->pixelSize = tqRound(tqt_pixelSize(fd->pointSize / 10., 0, screen));
    }

    return true;
}

/*
  Fills in a font definition (TQFontDef) from the font properties in an
  XFontStruct.

  Returns true if the TQFontDef could be filled with properties from
  the XFontStruct.  The fields lbearing and rbearing are not given any
  values.
*/
static bool tqt_fillFontDef( XFontStruct *fs, TQFontDef *fd, int screen )
{
    unsigned long value;
    if ( fs && !XGetFontProperty( fs, XA_FONT, &value ) )
	return false;

    char *n = XGetAtomName( TQPaintDevice::x11AppDisplay(), value );
    TQCString xlfd( n );
    if ( n )
	XFree( n );
    return tqt_fillFontDef( xlfd.lower(), fd, screen );
}


static TQtFontStyle::Key getStyle( char ** tokens )
{
    TQtFontStyle::Key key;

    char slant0 = tolower( (uchar) tokens[Slant][0] );

    if ( slant0 == 'r' ) {
        if ( tokens[Slant][1]) {
            char slant1 = tolower( (uchar) tokens[Slant][1] );

            if ( slant1 == 'o' )
                key.oblique = true;
            else if ( slant1 == 'i' )
		key.italic = true;
        }
    } else if ( slant0 == 'o' )
	key.oblique = true;
    else if ( slant0 == 'i' )
	key.italic = true;

    key.weight = getFontWeight( tokens[Weight] );

    if ( qstrcmp( tokens[Width], "normal" ) == 0 ) {
	key.stretch = 100;
    } else if ( qstrcmp( tokens[Width], "semi condensed" ) == 0 ||
		qstrcmp( tokens[Width], "semicondensed" ) == 0 ) {
	key.stretch = 90;
    } else if ( qstrcmp( tokens[Width], "condensed" ) == 0 ) {
	key.stretch = 80;
    } else if ( qstrcmp( tokens[Width], "narrow" ) == 0 ) {
	key.stretch = 60;
    }

    return key;
}


extern bool tqt_has_xft; // defined in qfont_x11.cpp

static bool xlfdsFullyLoaded = false;
static unsigned char encodingLoaded[numEncodings];

static void loadXlfds( const char *reqFamily, int encoding_id )
{
    TQtFontFamily *fontFamily = reqFamily ? db->family( reqFamily ) : 0;

    // make sure we don't load twice
    if ( (encoding_id == -1 && xlfdsFullyLoaded) || (encoding_id != -1 && encodingLoaded[encoding_id]) )
	return;
    if ( fontFamily && fontFamily->xlfdLoaded )
	return;

    if ( !tqt_has_xft ) {
    int fontCount;
    // force the X server to give us XLFDs
    TQCString xlfd_pattern = "-*-";
    xlfd_pattern += reqFamily ? reqFamily : "*";
    xlfd_pattern += "-*-*-*-*-*-*-*-*-*-*-";
    xlfd_pattern += xlfd_for_id( encoding_id );

    char **fontList = XListFonts( TQPaintDevice::x11AppDisplay(),
				  xlfd_pattern.data(),
				  0xffff, &fontCount );
    // tqDebug("requesting xlfd='%s', got %d fonts", xlfd_pattern.data(), fontCount );


    char *tokens[NFontFields];

    for( int i = 0 ; i < fontCount ; i++ ) {
	if ( ! parseXFontName( fontList[i], tokens ) ) continue;

	// get the encoding_id for this xlfd.  we need to do this
	// here, since we can pass -1 to this function to do full
	// database population
	*(tokens[CharsetEncoding]-1) = '-';
	int encoding_id = tqt_xlfd_encoding_id( tokens[CharsetRegistry] );
	if ( encoding_id == -1 )
	    continue;

	char *familyName = tokens[Family];
	capitalize( familyName );
	char *foundryName = tokens[Foundry];
	capitalize( foundryName );
	TQtFontStyle::Key styleKey = getStyle( tokens );

	bool smooth_scalable = false;
	bool bitmap_scalable = false;
	if ( isScalable(tokens) ) {
	    if ( isSmoothlyScalable( tokens ) )
		smooth_scalable = true;
	    else
		bitmap_scalable = true;
	}
	uint pixelSize = atoi( tokens[PixelSize] );
	uint xpointSize = atoi( tokens[PointSize] );
	uint xres = atoi( tokens[ResolutionX] );
	uint yres = atoi( tokens[ResolutionY] );
	uint avgwidth = atoi( tokens[AverageWidth] );
	bool fixedPitch = isFixedPitch( tokens );

        if (avgwidth == 0 && pixelSize != 0) {
            /*
              Ignore bitmap scalable fonts that are automatically
              generated by some X servers.  We know they are bitmap
              scalable because even though they have a specified pixel
              size, the average width is zero.
            */
            continue;
        }

	TQtFontFamily *family = fontFamily ? fontFamily : db->family( familyName, true );
	family->fontFileIndex = -1;
	TQtFontFoundry *foundry = family->foundry( foundryName, true );
	TQtFontStyle *style = foundry->style( styleKey, true );

	delete [] style->weightName;
	style->weightName = tqstrdup( tokens[Weight] );
	delete [] style->setwidthName;
	style->setwidthName = tqstrdup( tokens[Width] );

	if ( smooth_scalable ) {
	    style->smoothScalable = true;
	    style->bitmapScalable = false;
	    pixelSize = SMOOTH_SCALABLE;
	}
	if ( !style->smoothScalable && bitmap_scalable )
	    style->bitmapScalable = true;
	if ( !fixedPitch )
	    family->fixedPitch = false;

	TQtFontSize *size = style->pixelSize( pixelSize, true );
	TQtFontEncoding *enc =
	    size->encodingID( encoding_id, xpointSize, xres, yres, avgwidth, true );
	enc->pitch = *tokens[Spacing];
	if ( !enc->pitch ) enc->pitch = '*';

	for ( int script = 0; script < TQFont::LastPrivateScript; ++script ) {
	    if ( scripts_for_xlfd_encoding[encoding_id][script] )
		family->scripts[script] = TQtFontFamily::Supported;
	    else
		family->scripts[script] |= TQtFontFamily::UnSupported_Xlfd;
	}
	if ( encoding_id == -1 )
	    family->xlfdLoaded = true;
    }
    if ( !reqFamily ) {
	// mark encoding as loaded
	if ( encoding_id == -1 )
	    xlfdsFullyLoaded = true;
	else
	    encodingLoaded[encoding_id] = true;
    }

    XFreeFontNames( fontList );
    }
}

#ifndef TQT_NO_XFTFREETYPE
static int getXftWeight(int xftweight)
{
    int qtweight = TQFont::Black;
    if (xftweight <= (XFT_WEIGHT_LIGHT + XFT_WEIGHT_MEDIUM) / 2)
	qtweight = TQFont::Light;
    else if (xftweight <= (XFT_WEIGHT_MEDIUM + XFT_WEIGHT_DEMIBOLD) / 2)
	qtweight = TQFont::Normal;
    else if (xftweight <= (XFT_WEIGHT_DEMIBOLD + XFT_WEIGHT_BOLD) / 2)
	qtweight = TQFont::DemiBold;
    else if (xftweight <= (XFT_WEIGHT_BOLD + XFT_WEIGHT_BLACK) / 2)
	qtweight = TQFont::Bold;

    return qtweight;
}

static void loadXft()
{
    if (!tqt_has_xft)
	return;

    struct XftDefaultFont {
	const char *qtname;
	const char *rawname;
	bool fixed;
    };
    const XftDefaultFont defaults[] = {
	{ "Serif", "serif", false },
	{ "Sans Serif", "sans-serif", false },
        { "Monospace", "monospace", true },
	{ 0, 0, false }
    };
    const XftDefaultFont *f = defaults;
    while (f->qtname) {
	TQtFontFamily *family = db->family( f->qtname, true );
        family->fixedPitch = f->fixed;
	family->rawName = f->rawname;
	family->hasXft = true;
        family->synthetic = true;
	TQtFontFoundry *foundry
	    = family->foundry( TQString::null,  true );

	for ( int i = 0; i < TQFont::LastPrivateScript; ++i ) {
            if (i == TQFont::UnknownScript)
                continue;
	    family->scripts[i] = TQtFontFamily::Supported;
        }

	TQtFontStyle::Key styleKey;
	styleKey.oblique = false;
	for (int i = 0; i < 4; ++i) {
	    styleKey.italic = (i%2);
	    styleKey.weight = (i > 1) ? TQFont::Bold : TQFont::Normal;
	    TQtFontStyle *style = foundry->style( styleKey,  true );
	    style->smoothScalable = true;
	    TQtFontSize *size = style->pixelSize( SMOOTH_SCALABLE, true );
	    TQtFontEncoding *enc = size->encodingID( -1, 0, 0, 0, 0, true );
	    enc->pitch = (f->fixed ? 'm' : 'p');
	}
	++f;
    }
}

#ifdef XFT_MATRIX
static void checkXftMatrix( TQtFontFamily* family ) {
	for ( int j = 0; j < family->count; ++j ) {	// each foundry
	    TQtFontFoundry *foundry = family->foundries[j];
	    for ( int k = 0; k < foundry->count; ++k ) {
		TQtFontStyle *style = foundry->styles[k];
		if ( style->key.italic || style->key.oblique ) continue;

		TQtFontSize *size = style->pixelSize( SMOOTH_SCALABLE );
		if ( ! size ) continue;
		TQtFontEncoding *enc = size->encodingID( -1, 0, 0, 0, 0, true );
		if ( ! enc ) continue;

		TQtFontStyle::Key key = style->key;

		// does this style have an italic equivalent?
		key.italic = true;
		TQtFontStyle *equiv = foundry->style( key );
		if ( equiv ) continue;

		// does this style have an oblique equivalent?
		key.italic = false;
		key.oblique = true;
		equiv = foundry->style( key );
		if ( equiv ) continue;

		// let's fake one...
		equiv = foundry->style( key, true );
		equiv->fakeOblique = true;
		equiv->smoothScalable = true;

		TQtFontSize *equiv_size = equiv->pixelSize( SMOOTH_SCALABLE, true );
		TQtFontEncoding *equiv_enc = equiv_size->encodingID( -1, 0, 0, 0, 0, true );

		// keep the same pitch
		equiv_enc->pitch = enc->pitch;
	    }
	}
}
#endif // XFT_MATRIX

static bool loadXftFont( FcPattern* font )
{
    TQString familyName;
    TQString rawName;
    char *value;
    int weight_value;
    int slant_value;
    int spacing_value;
    char *file_value;
    int index_value;
    char *foundry_value = 0;
    FcBool scalable = FcTrue;

	if (XftPatternGetString( font,
				XFT_FAMILY, 0, &value) != XftResultMatch )
	    return false;
	// 	capitalize( value );
	rawName = familyName = TQString::fromUtf8(value);
	familyName.replace('-', ' ');
	familyName.replace("/", "");

	slant_value = XFT_SLANT_ROMAN;
	weight_value = XFT_WEIGHT_MEDIUM;
	spacing_value = XFT_PROPORTIONAL;
	file_value = 0;
	index_value = 0;
	XftPatternGetInteger (font, XFT_SLANT, 0, &slant_value);
	XftPatternGetInteger (font, XFT_WEIGHT, 0, &weight_value);
	XftPatternGetInteger (font, XFT_SPACING, 0, &spacing_value);
	XftPatternGetString (font, XFT_FILE, 0, &file_value);
	XftPatternGetInteger (font, XFT_INDEX, 0, &index_value);
	FcPatternGetBool(font, FC_SCALABLE, 0, &scalable);
	foundry_value = 0;
	XftPatternGetString(font, FC_FOUNDRY, 0, &foundry_value);
	TQtFontFamily *family = db->family( familyName, true );
	family->rawName = rawName;
	family->hasXft = true;

	FcCharSet *charset = 0;
	FcResult res = FcPatternGetCharSet(font, FC_CHARSET, 0, &charset);
	if (res == FcResultMatch && FcCharSetCount(charset) > 1) {
	    for (int i = 0; i < TQFont::LastPrivateScript; ++i) {
		bool supported = sample_chars[i][0];
		for (int j = 0; sample_chars[i][j]; ++j){
		    if (!FcCharSetHasChar(charset, sample_chars[i][j])) {
		        supported = false;
		        break;
		    }
		}
		if ( supported ){
		    family->scripts[i] = TQtFontFamily::Supported;
		} else {
		    family->scripts[i] |= TQtFontFamily::UnSupported_Xft;
		}
	    }
	    family->xftScriptCheck = true;
	} else {
	    // we set UnknownScript to supported for symbol fonts. It makes no sense to merge these
	    // with other ones, as they are special in a way.
	    for ( int i = 0; i < TQFont::LastPrivateScript; ++i )
		family->scripts[i] |= TQtFontFamily::UnSupported_Xft;
	    family->scripts[TQFont::UnknownScript] = TQtFontFamily::Supported;
	}

	TQCString file = (file_value ? file_value : "");
	family->fontFilename = file;
	family->fontFileIndex = index_value;

	TQtFontStyle::Key styleKey;
	styleKey.italic = (slant_value == XFT_SLANT_ITALIC);
	styleKey.oblique = (slant_value == XFT_SLANT_OBLIQUE);
	styleKey.weight = getXftWeight( weight_value );
        if (!scalable) {
            int width = 100;
#if FC_VERSION >= 20193
	    XftPatternGetInteger (font, FC_WIDTH, 0, &width);
#endif
            styleKey.stretch = width;
        }

	TQtFontFoundry *foundry
	    = family->foundry( foundry_value ? TQString::fromUtf8(foundry_value) : TQString::null,  true );
	TQtFontStyle *style = foundry->style( styleKey,  true );

        if (spacing_value < XFT_MONO )
            family->fixedPitch = false;

	TQtFontSize *size;
	if (scalable) {
	    style->smoothScalable = true;
	    size = style->pixelSize( SMOOTH_SCALABLE, true );
	}
	else {
	    double pixel_size = 0;
	    XftPatternGetDouble (font, FC_PIXEL_SIZE, 0, &pixel_size);
	    size = style->pixelSize( (int)pixel_size, true );
        }
	TQtFontEncoding *enc = size->encodingID( -1, 0, 0, 0, 0, true );
	enc->pitch = ( spacing_value >= XFT_CHARCELL ? 'c' :
		       ( spacing_value >= XFT_MONO ? 'm' : 'p' ) );

        checkXftMatrix( family );

    return true;
}

#endif // TQT_NO_XFTFREETYPE

static void load( const TQString &family = TQString::null, int script = -1 )
{
#ifdef TQFONTDATABASE_DEBUG
    TQTime t;
    t.start();
#endif

    if ( family.isNull() ) {
#ifndef TQT_NO_XFTFREETYPE
        static bool xft_readall_done = false;
        if (tqt_has_xft && !xft_readall_done) {
            xft_readall_done = true;
            XftFontSet  *fonts =
            	    XftListFonts(TQPaintDevice::x11AppDisplay(),
		         TQPaintDevice::x11AppScreen(),
		         (const char *)0,
		         XFT_FAMILY, XFT_WEIGHT, XFT_SLANT,
		         XFT_SPACING, XFT_FILE, XFT_INDEX,
		         FC_CHARSET, FC_FOUNDRY, FC_SCALABLE, FC_PIXEL_SIZE,
#if FC_VERSION >= 20193
                         FC_WIDTH,
#endif
		         (const char *)0);
            for (int i = 0; i < fonts->nfont; i++)
                loadXftFont( fonts->fonts[i] );
            XftFontSetDestroy (fonts);
        }
        if (tqt_has_xft)
            return;
#endif // TQT_NO_XFTFREETYPE
	if ( script == -1 )
	    loadXlfds( 0, -1 );
	else {
	    for ( int i = 0; i < numEncodings; i++ ) {
		if ( scripts_for_xlfd_encoding[i][script] )
		    loadXlfds( 0, i );
	    }
	}
    } else {
	TQtFontFamily *f = db->family( family, true );
	if ( !f->fullyLoaded ) {

#ifndef TQT_NO_XFTFREETYPE
            if (tqt_has_xft) {
                TQString mfamily = family;
            redo:
                XftFontSet  *fonts =
            	    XftListFonts(TQPaintDevice::x11AppDisplay(),
		         TQPaintDevice::x11AppScreen(),
                         XFT_FAMILY, XftTypeString, mfamily.utf8().data(),
		         (const char *)0,
		         XFT_FAMILY, XFT_WEIGHT, XFT_SLANT,
		         XFT_SPACING, XFT_FILE, XFT_INDEX,
		         FC_CHARSET, FC_FOUNDRY, FC_SCALABLE, FC_PIXEL_SIZE,
#if FC_VERSION >= 20193
                         FC_WIDTH,
#endif
		         (const char *)0);
                for (int i = 0; i < fonts->nfont; i++)
                    loadXftFont( fonts->fonts[i] );
                XftFontSetDestroy (fonts);
                if (mfamily.contains(' ')) {
                    mfamily.replace(TQChar(' '), TQChar('-'));
                    goto redo;
                }
		f->fullyLoaded = true;
                return;
            }
#endif // TQT_NO_XFTFREETYPE
	    // could reduce this further with some more magic:
	    // would need to remember the encodings loaded for the family.
	    if ( ( script == -1 && !f->xlfdLoaded ) ||
		 ( !f->hasXft && !(f->scripts[script] & TQtFontFamily::Supported) &&
		   !(f->scripts[script] & TQtFontFamily::UnSupported_Xlfd) ) ) {
		loadXlfds( family, -1 );
		f->fullyLoaded = true;
	    }
	}
    }

#ifdef TQFONTDATABASE_DEBUG
    FD_DEBUG("TQFontDatabase: load( %s, %d) took %d ms", family.latin1(), script, t.elapsed() );
#endif
}


static void initializeDb()
{
    if ( db ) return;
    db = new TQFontDatabasePrivate;
    qfontdatabase_cleanup.set(&db);

    TQTime t;
    t.start();

#ifndef TQT_NO_XFTFREETYPE
    loadXft();
    FD_DEBUG("TQFontDatabase: loaded Xft: %d ms",  t.elapsed() );
#endif

    t.start();

#ifndef TQT_NO_XFTFREETYPE
    for ( int i = 0; i < db->count; i++ ) {
#ifdef XFT_MATRIX
        checkXftMatrix( db->families[i] );
#endif // XFT_MATRIX
    }
#endif


#ifdef TQFONTDATABASE_DEBUG
    if (!tqt_has_xft)
	// load everything at startup in debug mode.
	loadXlfds( 0,  -1 );

    // print the database
    for ( int f = 0; f < db->count; f++ ) {
	TQtFontFamily *family = db->families[f];
	FD_DEBUG("'%s' %s  hasXft=%s", family->name.latin1(), (family->fixedPitch ? "fixed" : ""),
		 (family->hasXft ? "yes" : "no") );
	for ( int i = 0; i < TQFont::LastPrivateScript; ++i ) {
	    FD_DEBUG("\t%s: %s", TQFontDatabase::scriptName((TQFont::Script) i).latin1(),
		     ((family->scripts[i] & TQtFontFamily::Supported) ? "Supported" :
		      (family->scripts[i] & TQtFontFamily::UnSupported) == TQtFontFamily::UnSupported ?
		      "UnSupported" : "Unknown"));
	}

	for ( int fd = 0; fd < family->count; fd++ ) {
	    TQtFontFoundry *foundry = family->foundries[fd];
	    FD_DEBUG("\t\t'%s'", foundry->name.latin1() );
	    for ( int s = 0; s < foundry->count; s++ ) {
		TQtFontStyle *style = foundry->styles[s];
		FD_DEBUG("\t\t\tstyle: italic=%d oblique=%d (fake=%d) weight=%d (%s)\n"
			 "\t\t\tstretch=%d (%s)",
			 style->key.italic, style->key.oblique, style->fakeOblique, style->key.weight,
			 style->weightName, style->key.stretch,
			 style->setwidthName ? style->setwidthName : "nil" );
		if ( style->smoothScalable )
		    FD_DEBUG("\t\t\t\tsmooth scalable" );
		else if ( style->bitmapScalable )
		    FD_DEBUG("\t\t\t\tbitmap scalable" );
		if ( style->pixelSizes ) {
		    tqDebug("\t\t\t\t%d pixel sizes",  style->count );
		    for ( int z = 0; z < style->count; ++z ) {
			TQtFontSize *size = style->pixelSizes + z;
			for ( int e = 0; e < size->count; ++e ) {
			    FD_DEBUG( "\t\t\t\t  size %5d pitch %c encoding %s",
				      size->pixelSize,
				      size->encodings[e].pitch,
				      xlfd_for_id( size->encodings[e].encoding ) );
			}
		    }
		}
	    }
	}
    }
#endif // TQFONTDATABASE_DEBUG
}

void TQFontDatabase::createDatabase()
{
    initializeDb();
}


// --------------------------------------------------------------------------------------
// font loader
// --------------------------------------------------------------------------------------
#define MAXFONTSIZE_XFT 256
#define MAXFONTSIZE_XLFD 128
#ifndef TQT_NO_XFTFREETYPE
static double addPatternProps(XftPattern *pattern, const TQtFontStyle::Key &key, bool fakeOblique,
			      bool smoothScalable, const TQFontPrivate *fp, const TQFontDef &request)
{
    int weight_value = XFT_WEIGHT_BLACK;
    if ( key.weight == 0 )
	weight_value = XFT_WEIGHT_MEDIUM;
    else if ( key.weight < (TQFont::Light + TQFont::Normal) / 2 )
	weight_value = XFT_WEIGHT_LIGHT;
    else if ( key.weight < (TQFont::Normal + TQFont::DemiBold) / 2 )
	weight_value = XFT_WEIGHT_MEDIUM;
    else if ( key.weight < (TQFont::DemiBold + TQFont::Bold) / 2 )
	weight_value = XFT_WEIGHT_DEMIBOLD;
    else if ( key.weight < (TQFont::Bold + TQFont::Black) / 2 )
	weight_value = XFT_WEIGHT_BOLD;
    XftPatternAddInteger( pattern, XFT_WEIGHT, weight_value );

    int slant_value = XFT_SLANT_ROMAN;
    if ( key.italic )
	slant_value = XFT_SLANT_ITALIC;
    else if ( key.oblique && !fakeOblique )
	slant_value = XFT_SLANT_OBLIQUE;
    XftPatternAddInteger( pattern, XFT_SLANT, slant_value );

    /*
      Xft1 doesn't obey user settings for turning off anti-aliasing using
      the following:

      match any size > 6 size < 12 edit antialias = false;

      ... if we request pixel sizes.  so, work around this limitiation and
      convert the pixel size to a point size and request that.
    */
    double size_value = request.pixelSize;
    double dpi = TQPaintDevice::x11AppDpiY(fp->screen);
    double scale = 1.;
    if ( size_value > MAXFONTSIZE_XFT ) {
	scale = (double)size_value/(double)MAXFONTSIZE_XFT;
	size_value = MAXFONTSIZE_XFT;
    }

    /*
     2006-12-28 If QT is not compiled against xft1, there is no need
     for the workaround above (confirmed). Thus, in addition, add
     font pixelsize to the pattern to avoid fontconfig making wrong
     guesses. Also provide a DPI value for fontconfig so it never
     attempts to fallback to its default.
    */
    XftPatternAddDouble(pattern, XFT_PIXEL_SIZE, size_value);
    XftPatternAddDouble(pattern, XFT_DPI, dpi);
    size_value = size_value*72./dpi;
    XftPatternAddDouble( pattern, XFT_SIZE, size_value );

#ifdef XFT_MATRIX
    if (!smoothScalable) {
#    if FC_VERSION >= 20193
        int stretch = request.stretch;
        if (!stretch)
            stretch = 100;
	XftPatternAddInteger(pattern, FC_WIDTH, stretch);
#    endif
    } else
    if ( ( request.stretch > 0 && request.stretch != 100 ) ||
	 ( key.oblique && fakeOblique ) ) {
	XftMatrix matrix;
	XftMatrixInit( &matrix );

	if ( request.stretch > 0 && request.stretch != 100 )
	    XftMatrixScale( &matrix, double( request.stretch ) / 100.0, 1.0 );
	if ( key.oblique && fakeOblique )
	    XftMatrixShear( &matrix, 0.20, 0.0 );

	XftPatternAddMatrix( pattern, XFT_MATRIX, &matrix );
    }
#endif // XFT_MATRIX
    if (request.styleStrategy & (TQFont::PreferAntialias|TQFont::NoAntialias)) {
        XftPatternDel(pattern, XFT_ANTIALIAS);
	XftPatternAddBool(pattern, XFT_ANTIALIAS,
			  !(request.styleStrategy & TQFont::NoAntialias));
    }

    return scale;
}
#endif // TQT_NO_XFTFREETYPE

static
TQFontEngine *loadEngine( TQFont::Script script,
			 const TQFontPrivate *fp, const TQFontDef &request,
			 TQtFontFamily *family, TQtFontFoundry *foundry,
			 TQtFontStyle *style, TQtFontSize *size,
			 TQtFontEncoding *encoding, bool forced_encoding )
{
    Q_UNUSED(script);

    if ( fp && fp->rawMode ) {
	TQCString xlfd = request.family.latin1();
	FM_DEBUG( "Loading XLFD (rawmode) '%s'", xlfd.data() );

	XFontStruct *xfs;
	if (! (xfs = XLoadQueryFont(TQPaintDevice::x11AppDisplay(), xlfd.data() ) ) )
	    return 0;

	TQFontEngine *fe = new TQFontEngineXLFD( xfs, xlfd.data(), 0 );
	if ( ! tqt_fillFontDef( xfs, &fe->fontDef, TQPaintDevice::x11AppScreen() ) &&
	     ! tqt_fillFontDef( xlfd, &fe->fontDef, TQPaintDevice::x11AppScreen() ) )
	    fe->fontDef = TQFontDef();

	return fe;
    }

#ifndef TQT_NO_XFTFREETYPE
    if ( encoding->encoding == -1 ) {

	FM_DEBUG( "    using Xft" );

	XftPattern *pattern = XftPatternCreate();
	if ( !pattern ) return 0;

	bool symbol = (family->scripts[TQFont::UnknownScript] == TQtFontFamily::Supported);
	if (!symbol && script != TQFont::Unicode) {
	    FcCharSet *cs = FcCharSetCreate();
	    for ( int j=0; sample_chars[script][j]; j++ )
	        FcCharSetAddChar(cs, sample_chars[script][j]);
	    if (script == TQFont::Latin)
		// add Euro character
		FcCharSetAddChar(cs, 0x20ac);
	    FcPatternAddCharSet(pattern, FC_CHARSET, cs);
	    FcCharSetDestroy(cs);
	}

	if ( !foundry->name.isEmpty() )
	    XftPatternAddString( pattern, XFT_FOUNDRY,
				 foundry->name.utf8().data() );

	if ( !family->rawName.isEmpty() )
	    XftPatternAddString( pattern, XFT_FAMILY,
				 family->rawName.utf8().data() );


	char pitch_value = ( encoding->pitch == 'c' ? XFT_CHARCELL :
			     ( encoding->pitch == 'm' ? XFT_MONO : XFT_PROPORTIONAL ) );
	XftPatternAddInteger( pattern, XFT_SPACING, pitch_value );

	double scale = addPatternProps(pattern, style->key, style->fakeOblique,
                                       style->smoothScalable, fp, request);

	XftResult res;
	XftPattern *result =
	    XftFontMatch( TQPaintDevice::x11AppDisplay(), fp->screen, pattern, &res );
	if (result && script == TQFont::Latin) {
	    // since we added the Euro char on top, check we actually got the family
	    // we requested. If we didn't get it correctly, remove the Euro from the pattern
	    // and try again.
	    FcChar8 *f;
	    res = FcPatternGetString(result, FC_FAMILY, 0, &f);
	    if (res == FcResultMatch && TQString::fromUtf8((char *)f) != family->rawName) {
		FcPatternDel(pattern, FC_CHARSET);
		FcCharSet *cs = FcCharSetCreate();
		for ( int j=0; sample_chars[script][j]; j++ )
		  FcCharSetAddChar(cs, sample_chars[script][j]);
		FcPatternAddCharSet(pattern, FC_CHARSET, cs);
		FcCharSetDestroy(cs);
		result = XftFontMatch( TQPaintDevice::x11AppDisplay(), fp->screen, pattern, &res );
	    }
	}
	XftPatternDestroy(pattern);
	if (!result)
	    return 0;

        // somehow this gets lost in the XftMatch call, reset the anitaliasing property correctly.
        if (request.styleStrategy & (TQFont::PreferAntialias|TQFont::NoAntialias)) {
            XftPatternDel(result, XFT_ANTIALIAS);
            XftPatternAddBool(result, XFT_ANTIALIAS,
                              !(request.styleStrategy & TQFont::NoAntialias));
        }
	// We pass a duplicate to XftFontOpenPattern because either xft font
	// will own the pattern after the call or the pattern will be
	// destroyed.
	XftPattern *dup = XftPatternDuplicate( result );
	XftFont *xftfs = XftFontOpenPattern( TQPaintDevice::x11AppDisplay(), dup );

	if ( ! xftfs ) // Xft couldn't find a font?
	    return 0;

	TQFontEngine *fe = new TQFontEngineXft( xftfs, result, symbol ? 1 : 0 );
	if (fp->paintdevice
	    && TQPaintDeviceMetrics(fp->paintdevice).logicalDpiY() != TQPaintDevice::x11AppDpiY()) {
	    double px;
	    XftPatternGetDouble(result, XFT_PIXEL_SIZE, 0, &px);
	    scale = (double)request.pixelSize/px;
	}
	fe->setScale( scale );
	return fe;
    }
#endif // TQT_NO_XFTFREETYPE

    FM_DEBUG( "    using XLFD" );

    TQCString xlfd = "-";
    xlfd += foundry->name.isEmpty() ? "*" : foundry->name.latin1();
    xlfd += "-";
    xlfd += family->name.isEmpty() ? "*" : family->name.latin1();

    xlfd += "-";
    xlfd += style->weightName ? style->weightName : "*";
    xlfd += "-";
    xlfd += ( style->key.italic ? "i" : ( style->key.oblique ? "o" : "r" ) );

    xlfd += "-";
    xlfd += style->setwidthName ? style->setwidthName : "*";
    // ### handle add-style
    xlfd += "-*-";

    int px = size->pixelSize;
    if ( style->smoothScalable && px == SMOOTH_SCALABLE )
	px = request.pixelSize;
    else if ( style->bitmapScalable && px == 0 )
	px = request.pixelSize;
    double scale = 1.;
    if ( px > MAXFONTSIZE_XLFD ) {
	scale = (double)px/(double)MAXFONTSIZE_XLFD;
	px = MAXFONTSIZE_XLFD;
    }
    if (fp && fp->paintdevice
	&& TQPaintDeviceMetrics(fp->paintdevice).logicalDpiY() != TQPaintDevice::x11AppDpiY())
	scale = (double)request.pixelSize/(double)px;

    xlfd += TQString::number( px ).latin1();
    xlfd += "-";
    xlfd += TQString::number( encoding->xpoint );
    xlfd += "-";
    xlfd += TQString::number( encoding->xres );
    xlfd += "-";
    xlfd += TQString::number( encoding->yres );
    xlfd += "-";

    // ### handle cell spaced fonts
    xlfd += encoding->pitch;
    xlfd += "-";
    xlfd += TQString::number( encoding->avgwidth );
    xlfd += "-";
    xlfd += xlfd_for_id( encoding->encoding );

    FM_DEBUG( "    xlfd: '%s'", xlfd.data() );

    XFontStruct *xfs;
    if (! (xfs = XLoadQueryFont(TQPaintDevice::x11AppDisplay(), xlfd.data() ) ) )
	return 0;

    TQFontEngine *fe = 0;
    const int mib = xlfd_encoding[ encoding->encoding ].mib;
    if (script == TQFont::Latin && encoding->encoding <= LAST_LATIN_ENCODING && !forced_encoding) {
	fe = new TQFontEngineLatinXLFD( xfs, xlfd.data(), mib );
    } else {
	fe = new TQFontEngineXLFD( xfs, xlfd.data(), mib );
    }

    fe->setScale( scale );

    return fe;
}


static void parseFontName(const TQString &name, TQString &foundry, TQString &family)
{
    if ( name.contains('[') && name.contains(']')) {
	int i = name.find('[');
	int li = name.findRev(']');

	if (i < li) {
	    foundry = name.mid(i + 1, li - i - 1);
	    if (name[i - 1] == ' ')
		i--;
	    family = name.left(i);
	}
    } else {
	foundry = TQString::null;
	family = name;
    }
}


static TQFontEngine *loadFontConfigFont(const TQFontPrivate *fp, const TQFontDef &request, TQFont::Script script)
{
    if (!tqt_has_xft)
	return 0;

    TQStringList family_list;
    if (request.family.isEmpty()) {
	family_list = TQStringList::split(TQChar(','), fp->request.family);

	TQString stylehint;
	switch ( request.styleHint ) {
	case TQFont::SansSerif:
	    stylehint = "sans-serif";
	    break;
	case TQFont::Serif:
	    stylehint = "serif";
	    break;
	case TQFont::TypeWriter:
	    stylehint = "monospace";
	    break;
	default:
	    if (request.fixedPitch)
		stylehint = "monospace";
	    break;
	}
	if (!stylehint.isEmpty())
	    family_list << stylehint;
    } else {
	family_list << request.family;
    }

    FcPattern *pattern = FcPatternCreate();

    {
	TQString family, foundry;
	for (TQStringList::ConstIterator it = family_list.begin(); it != family_list.end(); ++it) {
	    parseFontName(*it, foundry, family);
	    XftPatternAddString(pattern, XFT_FAMILY, family.utf8().data());
	}
    }

    TQtFontStyle::Key key;
    key.italic = request.italic;
    key.weight = request.weight;
    key.stretch = request.stretch;

    double scale = addPatternProps(pattern, key, false, true, fp, request);
#ifdef FONT_MATCH_DEBUG
    tqDebug("original pattern contains:");
    FcPatternPrint(pattern);
#endif

    // XftFontMatch calls the right ConfigSubstitute variants, but as we use
    // FcFontMatch/Sort here we have to do it manually.
    FcConfigSubstitute(0, pattern, FcMatchPattern);
    XftDefaultSubstitute(TQPaintDevice::x11AppDisplay(), TQPaintDevice::x11AppScreen(), pattern);

//     tqDebug("1: pattern contains:");
//     FcPatternPrint(pattern);

    {
	FcValue value;
	value.type = FcTypeString;

	// these should only get added to the pattern _after_ substitution
	// append the default fallback font for the specified script
	extern TQString tqt_fallback_font_family( TQFont::Script );
	TQString fallback = tqt_fallback_font_family( script );
	if ( ! fallback.isEmpty() && ! family_list.contains( fallback ) ) {
	    TQCString cs = fallback.utf8();
	    value.u.s = (const FcChar8 *)cs.data();
	    FcPatternAddWeak(pattern, FC_FAMILY, value, FcTrue);
	}

	// add the default family
	TQString defaultFamily = TQApplication::font().family();
	if ( ! family_list.contains( defaultFamily ) ) {
	    TQCString cs = defaultFamily.utf8();
	    value.u.s = (const FcChar8 *)cs.data();
	    FcPatternAddWeak(pattern, FC_FAMILY, value, FcTrue);
	}

	// add TQFont::defaultFamily() to the list, for compatibility with
	// previous versions
	defaultFamily = TQApplication::font().defaultFamily();
	if ( ! family_list.contains( defaultFamily ) ) {
	    TQCString cs = defaultFamily.utf8();
	    value.u.s = (const FcChar8 *)cs.data();
	    FcPatternAddWeak(pattern, FC_FAMILY, value, FcTrue);
	}
    }

    if (script != TQFont::Unicode) {
        FcCharSet *cs = FcCharSetCreate();
	for ( int j=0; sample_chars[script][j]; j++ )
	    FcCharSetAddChar(cs, sample_chars[script][j]);
        if (script == TQFont::Latin)
            // add Euro character
            FcCharSetAddChar(cs, 0x20ac);
        FcPatternAddCharSet(pattern, FC_CHARSET, cs);
        FcCharSetDestroy(cs);
    }

#ifdef FONT_MATCH_DEBUG
	printf("final pattern contains:\n");
	FcPatternPrint(pattern);
#endif

    TQFontEngine *fe = 0;

    for( int jj = (FcGetVersion() >= 20392 ? 0 : 1); jj < 2; ++jj ) {
        bool use_fontsort = ( jj == 1 );

    FcResult result;
        FcFontSet *fs = 0;
        FcPattern *fsp = 0;

    // Properly conform to fontconfig API. We need to call FcDefaultSubstitute()
    // before FcFontSort()/FcFontMatch().
    FcDefaultSubstitute(pattern);

        if( use_fontsort ) {
            fs = FcFontSort(0, pattern, FcFalse, 0, &result);
    if (!fs)
                continue;
        } else {
            fsp = FcFontMatch(0, pattern, &result);
            if (!fsp)
                continue;
        }

#ifdef FONT_MATCH_DEBUG
        if( use_fontsort ) {
    printf("fontset contains:\n");
    for (int i = 0; i < fs->nfont; ++i) {
	FcPattern *test = fs->fonts[i];
	FcChar8 *fam;
	FcPatternGetString(test, FC_FAMILY, 0, &fam);
	printf("    %s\n", fam);
    }
        } else {
            printf("fontmatch:");
    	    FcChar8 *fam;
            FcPatternGetString(fsp, FC_FAMILY, 0, &fam);
            printf("    %s\n", fam);
        }
#endif

    double size_value = request.pixelSize;
    if ( size_value > MAXFONTSIZE_XFT )
	size_value = MAXFONTSIZE_XFT;

        int cnt = use_fontsort ? fs->nfont : 1;

        for (int i = 0; i < cnt; ++i) {
    	    FcPattern *font = use_fontsort ? fs->fonts[i] : fsp;
	FcCharSet *cs;
	FcResult res = FcPatternGetCharSet(font, FC_CHARSET, 0, &cs);
	if (res != FcResultMatch)
	    continue;
	bool do_break=true;
	for ( int j=0; sample_chars[script][j]; j++ ){
	    do_break=false;
	    if (!FcCharSetHasChar(cs, sample_chars[script][j])) {
		do_break=true;
                    break;
              }
        }
	if ( do_break )
	    continue;
	FcBool scalable;
	res = FcPatternGetBool(font, FC_SCALABLE, 0, &scalable);
	if (res != FcResultMatch || !scalable) {
	    int pixelSize;
	    res = FcPatternGetInteger(font, FC_PIXEL_SIZE, 0, &pixelSize);
	    if (res != FcResultMatch || TQABS((size_value-pixelSize)/size_value) > 0.2)
		continue;
	}

	XftPattern *pattern = XftPatternDuplicate(font);
	// add properties back in as the font selected from the list doesn't contain them.
	addPatternProps(pattern, key, false, true, fp, request);

	XftPattern *result =
	    XftFontMatch( TQPaintDevice::x11AppDisplay(), fp->screen, pattern, &res );
	XftPatternDestroy(pattern);

	// We pass a duplicate to XftFontOpenPattern because either xft font
	// will own the pattern after the call or the pattern will be
	// destroyed.
	XftPattern *dup = XftPatternDuplicate( result );
	XftFont *xftfs = XftFontOpenPattern( TQPaintDevice::x11AppDisplay(), dup );

	if ( !xftfs ) {
	    // Xft couldn't find a font?
	    tqDebug("couldn't open fontconfigs chosen font with Xft!!!");
	} else {
	    fe = new TQFontEngineXft( xftfs, result, 0 );
	    if (fp->paintdevice
		&& TQPaintDeviceMetrics(fp->paintdevice).logicalDpiY() != TQPaintDevice::x11AppDpiY()) {
		double px;
		XftPatternGetDouble(result, XFT_PIXEL_SIZE, 0, &px);
		scale = request.pixelSize/px;
	    }
	    fe->setScale( scale );
	    fe->fontDef = request;
	    if ( script != TQFont::Unicode && !canRender(fe, script) ) {
		FM_DEBUG( "  WARN: font loaded cannot render samples" );
		delete fe;
		fe = 0;
	    }else
		FM_DEBUG( "  USE: %s", fe->fontDef.family.latin1() );
	}
	if (fe) {
            TQFontEngineXft *xft = (TQFontEngineXft *)fe;
            char *family;
            if (XftPatternGetString(xft->pattern(), XFT_FAMILY, 0, &family) == XftResultMatch)
                xft->fontDef.family = TQString::fromUtf8(family);

            double px;
            if (XftPatternGetDouble(xft->pattern(), XFT_PIXEL_SIZE, 0, &px) == XftResultMatch)
                xft->fontDef.pixelSize = tqRound(px);

            int weight = XFT_WEIGHT_MEDIUM;
            XftPatternGetInteger(xft->pattern(), XFT_WEIGHT, 0, &weight);
            xft->fontDef.weight = getXftWeight(weight);

            int slant = XFT_SLANT_ROMAN;
            XftPatternGetInteger(xft->pattern(), XFT_SLANT, 0, &slant);
            xft->fontDef.italic = (slant != XFT_SLANT_ROMAN);

            int spacing = XFT_PROPORTIONAL;
            XftPatternGetInteger(xft->pattern(), XFT_SPACING, 0, &spacing);
	    xft->fontDef.fixedPitch = spacing != XFT_PROPORTIONAL;

            xft->fontDef.ignorePitch = false;
	    break;
        }
    }

        if( use_fontsort )
    FcFontSetDestroy(fs);
        else
            FcPatternDestroy(fsp);

        if( fe )
            break;

    } // for( jj )

    FcPatternDestroy(pattern);

    return fe;
}
