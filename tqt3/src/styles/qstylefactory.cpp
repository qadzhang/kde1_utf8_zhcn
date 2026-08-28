/****************************************************************************
**
** Implementation of TQStyleFactory class
**
** Created : 001103
**
** Copyright (C) 1992-2008 Trolltech ASA.  All rights reserved.
**
** This file is part of the widgets module of the TQt GUI Toolkit.
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

#include "qstyleinterface_p.h" // up here for GCC 2.7.* compatibility
#include "ntqstylefactory.h"

#ifndef TQT_NO_STYLE

#include "ntqapplication.h"
#include "ntqwindowsstyle.h"
#include "ntqmotifstyle.h"
#include "ntqcdestyle.h"
#include "ntqmotifplusstyle.h"
#include "ntqplatinumstyle.h"
#include "ntqsgistyle.h"
#include "ntqcompactstyle.h"
#ifndef TQT_NO_STYLE_WINDOWSXP
#include "qwindowsxpstyle.h"
#endif
#ifndef TQT_NO_STYLE_AQUA
#include "qaquastyle.h"
#endif
#ifndef TQT_NO_STYLE_POCKETPC
#include "qpocketpcstyle_wce.h"
#endif

#if !defined( TQT_NO_STYLE_MAC ) && defined( TQ_WS_MAC )
TQString p2qstring(const unsigned char *c); //qglobal.cpp
#include "qt_mac.h"
#include "qmacstyle_mac.h"
#endif
#include <stdlib.h>

#include <private/qpluginmanager_p.h>
#ifndef TQT_NO_COMPONENT
class TQStyleFactoryPrivate : public TQObject
{
public:
    TQStyleFactoryPrivate();
    ~TQStyleFactoryPrivate();

    static TQPluginManager<TQStyleFactoryInterface> *manager;
};

static TQStyleFactoryPrivate *instance = 0;
TQPluginManager<TQStyleFactoryInterface> *TQStyleFactoryPrivate::manager = 0;

TQStyleFactoryPrivate::TQStyleFactoryPrivate()
: TQObject( tqApp )
{
    manager = new TQPluginManager<TQStyleFactoryInterface>( IID_QStyleFactory, TQApplication::libraryPaths(), "/styles", false );
}

TQStyleFactoryPrivate::~TQStyleFactoryPrivate()
{
    delete manager;
    manager = 0;

    instance = 0;
}

#endif //TQT_NO_COMPONENT

/*!
    \class TQStyleFactory ntqstylefactory.h
    \brief The TQStyleFactory class creates TQStyle objects.

    The style factory creates a TQStyle object for a given key with
    TQStyleFactory::create(key).

    The styles are either built-in or dynamically loaded from a style
    plugin (see \l TQStylePlugin).

    TQStyleFactory::keys() returns a list of valid keys, typically
    including "Windows", "Motif", "CDE", "MotifPlus", "Platinum",
    "SGI" and "Compact". Depending on the platform, "WindowsXP",
    "Aqua" or "Macintosh" may be available.
*/

/*!
    Creates a TQStyle object that matches \a key case-insensitively.
    This is either a built-in style, or a style from a style plugin.

    \sa keys()
*/
TQStyle *TQStyleFactory::create( const TQString& key )
{
    TQStyle *ret = 0;
    TQString style = key.lower();
#ifndef TQT_NO_STYLE_WINDOWS
    if ( style == "windows" )
        ret = new TQWindowsStyle;
    else
#endif
#ifndef TQT_NO_STYLE_WINDOWSXP
    if ( style == "windowsxp" )
	ret = new TQWindowsXPStyle;
    else
#endif
#ifndef TQT_NO_STYLE_MOTIF
    if ( style == "motif" )
        ret = new TQMotifStyle;
    else
#endif
#ifndef TQT_NO_STYLE_CDE
    if ( style == "cde" )
        ret = new TQCDEStyle;
    else
#endif
#ifndef TQT_NO_STYLE_MOTIFPLUS
    if ( style == "motifplus" )
        ret = new TQMotifPlusStyle;
    else
#endif
#ifndef TQT_NO_STYLE_PLATINUM
    if ( style == "platinum" )
        ret = new TQPlatinumStyle;
    else
#endif
#ifndef TQT_NO_STYLE_SGI
    if ( style == "sgi")
        ret = new TQSGIStyle;
    else
#endif
#ifndef TQT_NO_STYLE_COMPACT
    if ( style == "compact" )
        ret = new TQCompactStyle;
    else
#endif
#ifndef TQT_NO_STYLE_AQUA
    if ( style == "aqua" )
        ret = new TQAquaStyle;
#endif
#ifndef TQT_NO_STYLE_POCKETPC
    if ( style == "pocketpc" )
	ret = new TQPocketPCStyle;
#endif
#if !defined( TQT_NO_STYLE_MAC ) && defined( TQ_WS_MAC )
    if( style.left(9) == "macintosh" )
	ret = new TQMacStyle;
#endif
    { } // Keep these here - they make the #ifdefery above work

#ifndef TQT_NO_COMPONENT
    if(!ret) {
	if ( !instance )
	    instance = new TQStyleFactoryPrivate;

	TQInterfacePtr<TQStyleFactoryInterface> iface;
	TQStyleFactoryPrivate::manager->queryInterface( style, &iface );

	if ( iface )
	    ret = iface->create( style );
    }
    if(ret)
	ret->setName(key);
#endif
    return ret;
}

#ifndef TQT_NO_STRINGLIST
/*!
    Returns the list of keys this factory can create styles for.

    \sa create()
*/
TQStringList TQStyleFactory::keys()
{
    TQStringList list;
#ifndef TQT_NO_COMPONENT
    if ( !instance )
	instance = new TQStyleFactoryPrivate;

    list = TQStyleFactoryPrivate::manager->featureList();
#endif //TQT_NO_COMPONENT

#ifndef TQT_NO_STYLE_WINDOWS
    if ( !list.contains( "Windows" ) )
	list << "Windows";
#endif
#ifndef TQT_NO_STYLE_WINDOWSXP
    if ( !list.contains( "WindowsXP" ) && TQWindowsXPStyle::resolveSymbols() )
	list << "WindowsXP";
#endif
#ifndef TQT_NO_STYLE_MOTIF
    if ( !list.contains( "Motif" ) )
	list << "Motif";
#endif
#ifndef TQT_NO_STYLE_CDE
    if ( !list.contains( "CDE" ) )
	list << "CDE";
#endif
#ifndef TQT_NO_STYLE_MOTIFPLUS
    if ( !list.contains( "MotifPlus" ) )
	list << "MotifPlus";
#endif
#ifndef TQT_NO_STYLE_PLATINUM
    if ( !list.contains( "Platinum" ) )
	list << "Platinum";
#endif
#ifndef TQT_NO_STYLE_SGI
    if ( !list.contains( "SGI" ) )
	list << "SGI";
#endif
#ifndef TQT_NO_STYLE_COMPACT
    if ( !list.contains( "Compact" ) )
	list << "Compact";
#endif
#ifndef TQT_NO_STYLE_AQUA
    if ( !list.contains( "Aqua" ) )
	list << "Aqua";
#endif
#if !defined( TQT_NO_STYLE_MAC ) && defined( TQ_WS_MAC )
    TQString mstyle = "Macintosh";
    Collection c = NewCollection();
    if (c) {
	GetTheme(c);
	Str255 str;
	long int s = 256;
	if(!GetCollectionItem(c, kThemeNameTag, 0, &s, &str))
	    mstyle += " (" + p2qstring(str) + ")";
    }
    if (!list.contains(mstyle))
	list << mstyle;
    DisposeCollection(c);
#endif

    return list;
}
#endif
#endif // TQT_NO_STYLE
