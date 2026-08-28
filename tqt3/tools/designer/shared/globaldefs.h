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

#ifndef GLOBALDEFS_H
#define GLOBALDEFS_H

#include <ntqcolor.h>
#include <ntqapplication.h>

#define BOXLAYOUT_DEFAULT_MARGIN 11
#define BOXLAYOUT_DEFAULT_SPACING 6

#ifndef NO_STATIC_COLORS
static TQColor *backColor1 = 0;
static TQColor *backColor2 = 0;
static TQColor *selectedBack = 0;

static void init_colors()
{
    if ( backColor1 )
	return;

#if 0 // a calculated alternative for backColor1
    TQColorGroup myCg = tqApp->palette().active();
    int h1, s1, v1;
    int h2, s2, v2;
    myCg.color( TQColorGroup::Base ).hsv( &h1, &s1, &v1 );
    myCg.color( TQColorGroup::Background ).hsv( &h2, &s2, &v2 );
    TQColor c( h1, s1, ( v1 + v2 ) / 2, TQColor::Hsv );
#endif

    backColor1 = new TQColor(  250, 248, 235 );
    backColor2 = new TQColor( 255, 255, 255 );
    selectedBack = new TQColor( 230, 230, 230 );
}

#endif

#endif
