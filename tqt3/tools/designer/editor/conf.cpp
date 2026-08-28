/**********************************************************************
** Copyright (C) 2000-2008 Trolltech ASA.  All rights reserved.
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

#include "conf.h"
#include <ntqapplication.h>
#include <ntqfont.h>
#include <ntqcolor.h>
#include <ntqsettings.h>

TQMap<TQString, ConfigStyle> Config::defaultStyles()
{
    ConfigStyle s;
    TQMap<TQString, ConfigStyle> styles;
    int normalSize =  tqApp->font().pointSize();
    TQString normalFamily = tqApp->font().family();
    TQString commentFamily = "times";
    int normalWeight = tqApp->font().weight();

    s.font = TQFont( normalFamily, normalSize, normalWeight );
    s.color = TQt::black;
    styles.insert( "Standard", s );

    s.font = TQFont( commentFamily, normalSize, normalWeight, true );
    s.color = TQt::red;
    styles.insert( "Comment", s );

    s.font = TQFont( normalFamily, normalSize, normalWeight );
    s.color = TQt::blue;
    styles.insert( "Number", s );

    s.font = TQFont( normalFamily, normalSize, normalWeight );
    s.color = TQt::darkGreen;
    styles.insert( "String", s );

    s.font = TQFont( normalFamily, normalSize, normalWeight );
    s.color = TQt::darkMagenta;
    styles.insert( "Type", s );

    s.font = TQFont( normalFamily, normalSize, normalWeight );
    s.color = TQt::darkYellow;
    styles.insert( "Keyword", s );

    s.font = TQFont( normalFamily, normalSize, normalWeight );
    s.color = TQt::darkBlue;
    styles.insert( "Preprocessor", s );

    s.font = TQFont( normalFamily, normalSize, normalWeight );
    s.color = TQt::darkRed;
    styles.insert( "Label", s );

    return styles;
}

TQMap<TQString, ConfigStyle> Config::readStyles( const TQString &path )
{
    TQMap<TQString, ConfigStyle> styles;
    styles = defaultStyles();

    TQString family;
    int size = 10;
    bool bold = false, italic = false, underline = false;
    int red = 0, green = 0, blue = 0;

    TQString elements[] = {
	"Comment",
	"Number",
	"String",
	"Type",
	"Keyword",
	"Preprocessor",
	"Label",
	"Standard",
	TQString::null
    };

    for ( int i = 0; elements[ i ] != TQString::null; ++i ) {
	TQSettings settings;
	bool ok = true;
	for (;;) {
	    family = settings.readEntry( path + elements[ i ] + "/family", TQString::null, &ok );
	    if ( !ok )
		break;
	    size = settings.readNumEntry( path + elements[ i ] + "/size", 10, &ok );
	    if ( !ok )
		break;
	    bold = settings.readBoolEntry( path + elements[ i ] + "/bold", false, &ok );
	    if ( !ok )
		break;
	    italic = settings.readBoolEntry( path + elements[ i ] + "/italic", false, &ok );
	    if ( !ok )
		break;
	    underline = settings.readBoolEntry( path + elements[ i ] + "/underline", false, &ok );
	    if ( !ok )
		break;
	    red = settings.readNumEntry( path + elements[ i ] + "/red", 0, &ok );
	    if ( !ok )
		break;
	    green = settings.readNumEntry( path + elements[ i ] + "/green", 0, &ok );
	    if ( !ok )
		break;
	    blue = settings.readNumEntry( path + elements[ i ] + "/blue", 0, &ok );
	    if ( !ok )
		break;
	    break;
	}
	if ( !ok )
	    continue;
	TQFont f( family );
	f.setPointSize( size );
	f.setBold( bold );
	f.setItalic( italic );
	f.setUnderline( underline );
	TQColor c( red, green, blue );
	ConfigStyle s;
	s.font = f;
	s.color = c;
	styles.remove( elements[ i ] );
	styles.insert( elements[ i ], s );
    }
    return styles;
}

void Config::saveStyles( const TQMap<TQString, ConfigStyle> &styles, const TQString &path )
{
    TQString elements[] = {
	"Comment",
	"Number",
	"String",
	"Type",
	"Keyword",
	"Preprocessor",
	"Label",
	"Standard",
	TQString::null
    };

    TQSettings settings;
    for ( int i = 0; elements[ i ] != TQString::null; ++i ) {
	settings.writeEntry( path + "/" + elements[ i ] + "/family", styles[ elements[ i ] ].font.family() );
	settings.writeEntry( path + "/"  + elements[ i ] + "/size", styles[ elements[ i ] ].font.pointSize() );
	settings.writeEntry( path + "/"  + elements[ i ] + "/bold", styles[ elements[ i ] ].font.bold() );
	settings.writeEntry( path + "/"  + elements[ i ] + "/italic", styles[ elements[ i ] ].font.italic() );
	settings.writeEntry( path + "/"  + elements[ i ] + "/underline", styles[ elements[ i ] ].font.underline() );
	settings.writeEntry( path + "/"  + elements[ i ] + "/red", styles[ elements[ i ] ].color.red() );
	settings.writeEntry( path + "/"  + elements[ i ] + "/green", styles[ elements[ i ] ].color.green() );
	settings.writeEntry( path + "/"  + elements[ i ] + "/blue", styles[ elements[ i ] ].color.blue() );
    }
}

bool Config::completion( const TQString &path )
{
    TQSettings settings;
    bool ret = settings.readBoolEntry( path + "/completion", true );
    return ret;
}

bool Config::wordWrap( const TQString &path )
{
    TQSettings settings;
    bool ret = settings.readBoolEntry( path + "/wordWrap", true );
    return ret;
}

bool Config::parenMatching( const TQString &path )
{
    TQSettings settings;
    bool ret = settings.readBoolEntry( path + "/parenMatching", true );
    return ret;
}

int Config::indentTabSize( const TQString &path )
{
    TQSettings settings;
    int ret = settings.readNumEntry( path + "/indentTabSize", 8 );
    return ret;
}

int Config::indentIndentSize( const TQString &path )
{
    TQSettings settings;
    int ret = settings.readNumEntry( path + "/indentIndentSize", 4 );
    return ret;
}

bool Config::indentKeepTabs( const TQString &path )
{
    TQSettings settings;
    bool ret = settings.readBoolEntry( path + "/indentKeepTabs", true );
    return ret;
}

bool Config::indentAutoIndent( const TQString &path )
{
    TQSettings settings;
    bool ret = settings.readBoolEntry( path + "/indentAutoIndent", true );
    return ret;
}

void Config::setCompletion( bool b, const TQString &path )
{
    TQSettings settings;
    settings.writeEntry( path + "/completion", b );
}

void Config::setWordWrap( bool b, const TQString &path )
{
    TQSettings settings;
    settings.writeEntry( path + "/wordWrap", b );
}

void Config::setParenMatching( bool b,const TQString &path )
{
    TQSettings settings;
    settings.writeEntry( path + "/parenMatching", b );
}

void Config::setIndentTabSize( int s, const TQString &path )
{
    TQSettings settings;
    settings.writeEntry( path + "/indentTabSize", s );
}

void Config::setIndentIndentSize( int s, const TQString &path )
{
    TQSettings settings;
    settings.writeEntry( path + "/indentIndentSize", s );
}

void Config::setIndentKeepTabs( bool b, const TQString &path )
{
    TQSettings settings;
    settings.writeEntry( path + "/indentKeepTabs", b );
}

void Config::setIndentAutoIndent( bool b, const TQString &path )
{
    TQSettings settings;
    settings.writeEntry( path + "/indentAutoIndent", b );
}
