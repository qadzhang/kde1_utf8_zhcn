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

#ifndef CONF_H
#define CONF_H

#include <ntqfont.h>
#include <ntqcolor.h>
#include <ntqmap.h>

struct ConfigStyle
{
    TQFont font;
    TQColor color;
};

struct Config
{
    TQMap<TQString, ConfigStyle> styles;
    bool hasCompletion, hasParenMatching, hasWordWrap;

    static TQMap<TQString, ConfigStyle> defaultStyles();
    static TQMap<TQString, ConfigStyle> readStyles( const TQString &path );
    static void saveStyles( const TQMap<TQString, ConfigStyle> &styles, const TQString &path );
    static bool completion( const TQString &path );
    static bool wordWrap( const TQString &path );
    static bool parenMatching( const TQString &path );
    static int indentTabSize( const TQString &path );
    static int indentIndentSize( const TQString &path );
    static bool indentKeepTabs( const TQString &path );
    static bool indentAutoIndent( const TQString &path );

    static void setCompletion( bool b, const TQString &path );
    static void setWordWrap( bool b, const TQString &path );
    static void setParenMatching( bool b,const TQString &path );
    static void setIndentTabSize( int s, const TQString &path );
    static void setIndentIndentSize( int s, const TQString &path );
    static void setIndentKeepTabs( bool b, const TQString &path );
    static void setIndentAutoIndent( bool b, const TQString &path );

};

#endif
