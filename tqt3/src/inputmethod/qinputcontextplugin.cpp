/****************************************************************************
** $Id: qinputcontextplugin.cpp,v 1.2 2004/06/20 18:43:11 daisuke Exp $
**
** Implementation of TQInputContextPlugin class
**
** Created : 010920
**
** Copyright (C) 2001 Trolltech AS.  All rights reserved.
**
** This file is part of the widgets module of the TQt GUI Toolkit.
**
** This file may be distributed under the terms of the Q Public License
** as defined by Trolltech AS of Norway and appearing in the file
** LICENSE.TQPL included in the packaging of this file.
**
** This file may be distributed and/or modified under the terms of the
** GNU General Public License version 2 as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL included in the
** packaging of this file.
**
** Licensees holding valid TQt Enterprise Edition or TQt Professional Edition
** licenses may use this file in accordance with the TQt Commercial License
** Agreement provided with the Software.
**
** This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE
** WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
**
** See http://www.trolltech.com/pricing.html or email sales@trolltech.com for
**   information about TQt Commercial License Agreements.
** See http://www.trolltech.com/qpl/ for TQPL licensing information.
** See http://www.trolltech.com/gpl/ for GPL licensing information.
**
** Contact info@trolltech.com if any conditions of this licensing are
** not clear to you.
**
**********************************************************************/

#include "ntqinputcontextplugin.h"

#ifndef TQT_NO_IM
#ifndef TQT_NO_COMPONENT

#include "qinputcontextinterface_p.h"

/*!
    \class TQInputContextPlugin ntqinputcontextplugin.h
    \brief The TQInputContextPlugin class provides an abstract base for custom TQInputContext plugins.
    \reentrant
    \ingroup plugins

    The input context plugin is a simple plugin interface that makes it
    easy to create custom input contexts that can be loaded dynamically
    into applications.

    Writing a input context plugin is achieved by subclassing this
    base class, reimplementing the pure virtual functions keys(),
    create(), languages(), displayName() description() and exporting
    the class with the \c TQ_EXPORT_PLUGIN macro.  See the \link
    plugins-howto.html TQt Plugins documentation \endlink for details.

    \sa TQInputContext
*/

/*!
    \fn TQStringList TQInputContextPlugin::keys() const

    Returns the list of TQInputContext keys this plugin provides.

    These keys are usually the class names of the custom input context
    that are implemented in the plugin.

    Return value is the names to identify and specify input methods
    for the input method switching mechanism and so on. The names have
    to be consistent with TQInputContext::identifierName(). The names
    have to consist of ASCII characters only. See also
    TQInputContext::identifierName() for further information.

    \sa create(), displayName(), TQInputContext::identifierName()
*/

/*!
    \fn TQInputContext* TQInputContextPlugin::create( const TQString& key )

    Creates and returns a TQInputContext instance for the input context key \a key.
    The input context key is usually the class name of the required input method.

    \sa keys()
*/

/*!
    \fn TQStringList languages( const TQString &key )

    Returns what languages are supported by the TQInputContext instance
    specified by \a key.

    The languages are expressed as language code (e.g. "zh_CN",
    "zh_TW", "zh_HK", "ja", "ko", ...). An input context that suports
    multiple languages can return all supported languages as
    TQStringList. The name has to be consistent with
    TQInputContextPlugin::language().

    This information may be used to optimize user interface.

    \sa TQInputContext::language()
*/

/*!
    \fn TQString displayName( const TQString &key )

    Returns a user friendly i18n-ized name of the TQInputContext
    instance specified by \a key. This string may be appeared in a
    menu and so on for users.

    There are two different names with different responsibility in the
    input method domain. This function returns one of them. Another
    name is called 'identifier name' to identify and specify input
    methods for the input method switching mechanism and so on.

    Although tr( identifierName ) can provide user friendly i18n-ized
    name without this function, the message catalog have to be managed
    by TQt in the case. However, some sophisticated input method
    framework manages their own message catalogs to provide this
    i18n-ized name string. So we need this function rather than just
    call tr() for identifier name.

    \sa keys(), TQInputContext::identifierName()
*/

/*!
    \fn TQString description( const TQString &key )

    Returns a i18n-ized brief description of the TQInputContext
    instance specified by \a key. This string may be appeared in some
    user interfaces.
*/



class TQInputContextPluginPrivate : public TQInputContextFactoryInterface
{
public:
    TQInputContextPluginPrivate( TQInputContextPlugin *p )
	: plugin( p )
    {
    }

    virtual ~TQInputContextPluginPrivate();

    TQRESULT queryInterface( const TQUuid &iid, TQUnknownInterface **iface );
    TQ_REFCOUNT;

    TQStringList featureList() const;
    TQInputContext *create( const TQString &key );
    TQStringList languages( const TQString &key );
    TQString displayName( const TQString &key );
    TQString description( const TQString &key );

private:
    TQInputContextPlugin *plugin;
};

TQRESULT TQInputContextPluginPrivate::queryInterface( const TQUuid &iid, TQUnknownInterface **iface )
{
    *iface = 0;

    if ( iid == IID_QUnknown )
	*iface = this;
    else if ( iid == IID_QFeatureList )
	*iface = this;
    else if ( iid == IID_QInputContextFactory )
	*iface = this;
    else
	return TQE_NOINTERFACE;

    (*iface)->addRef();
    return TQS_OK;
}

TQInputContextPluginPrivate::~TQInputContextPluginPrivate()
{
    delete plugin;
}

TQStringList TQInputContextPluginPrivate::featureList() const
{
    return plugin->keys();
}

TQInputContext *TQInputContextPluginPrivate::create( const TQString &key )
{
    return plugin->create( key );
}

TQStringList TQInputContextPluginPrivate::languages( const TQString &key )
{
    return plugin->languages( key );
}

TQString TQInputContextPluginPrivate::displayName( const TQString &key )
{
    return plugin->displayName( key );
}

TQString TQInputContextPluginPrivate::description( const TQString &key )
{
    return plugin->description( key );
}


/*!
    Constructs a input context plugin. This is invoked automatically by the
    \c TQ_EXPORT_PLUGIN macro.
*/
TQInputContextPlugin::TQInputContextPlugin()
    : TQGPlugin( d = new TQInputContextPluginPrivate( this ) )
{
}

/*!
    Destroys the input context plugin.

    You never have to call this explicitly. TQt destroys a plugin
    automatically when it is no longer used.
*/
TQInputContextPlugin::~TQInputContextPlugin()
{
    // don't delete d, as this is deleted by d
}

#endif // TQT_NO_COMPONENT
#endif // TQT_NO_IM
