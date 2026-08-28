/****************************************************************************
**
** Implementation of TQSqlDriverPlugin class
**
** Created : 010920
**
** Copyright (C) 2005-2008 Trolltech ASA.  All rights reserved.
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

#include "ntqstyleplugin.h"

#ifndef TQT_NO_STYLE
#ifndef TQT_NO_COMPONENT

#include "qstyleinterface_p.h"
#include "ntqobjectcleanuphandler.h"
#include "ntqstyle.h"

/*!
    \class TQStylePlugin ntqstyleplugin.h
    \brief The TQStylePlugin class provides an abstract base for custom TQStyle plugins.

    \ingroup plugins

    The style plugin is a simple plugin interface that makes it easy
    to create custom styles that can be loaded dynamically into
    applications with a TQStyleFactory.

    Writing a style plugin is achieved by subclassing this base class,
    reimplementing the pure virtual functions keys() and create(), and
    exporting the class with the \c TQ_EXPORT_PLUGIN macro. See the
    \link plugins-howto.html plugins documentation\endlink for an
    example.
*/

/*!
    \fn TQStringList TQStylePlugin::keys() const

    Returns the list of style keys this plugin supports.

    These keys are usually the class names of the custom styles that
    are implemented in the plugin.

    \sa create()
*/

/*!
    \fn TQStyle* TQStylePlugin::create( const TQString& key )

    Creates and returns a TQStyle object for the style key \a key. The
    style key is usually the class name of the required style.

    \sa keys()
*/

class TQStylePluginPrivate : public TQStyleFactoryInterface, public TQLibraryInterface
{
public:
    TQStylePluginPrivate( TQStylePlugin *p )
	: plugin( p )
    {
    }

    virtual ~TQStylePluginPrivate();

    TQRESULT queryInterface( const TQUuid &iid, TQUnknownInterface **iface );
    TQ_REFCOUNT;

    TQStringList featureList() const;
    TQStyle *create( const TQString &key );

    bool init();
    void cleanup();
    bool canUnload() const;

private:
    TQStylePlugin *plugin;
    TQObjectCleanupHandler styles;
};

TQRESULT TQStylePluginPrivate::queryInterface( const TQUuid &iid, TQUnknownInterface **iface )
{
    *iface = 0;

    if ( iid == IID_QUnknown )
	*iface = (TQStyleFactoryInterface*)this;
    else if ( iid == IID_QFeatureList )
	*iface = (TQFeatureListInterface*)this;
    else if ( iid == IID_QStyleFactory )
	*iface = (TQStyleFactoryInterface*)this;
    else if ( iid == IID_QLibrary )
	*iface = (TQLibraryInterface*) this;
    else
	return TQE_NOINTERFACE;

    (*iface)->addRef();
    return TQS_OK;
}

TQStylePluginPrivate::~TQStylePluginPrivate()
{
    delete plugin;
}

TQStringList TQStylePluginPrivate::featureList() const
{
    return plugin->keys();
}

TQStyle *TQStylePluginPrivate::create( const TQString &key )
{
    TQStyle *st = plugin->create( key );
    styles.add( st );
    return st;
}

bool TQStylePluginPrivate::init()
{
    return true;
}

void TQStylePluginPrivate::cleanup()
{
    styles.clear();
}

bool TQStylePluginPrivate::canUnload() const
{
    return styles.isEmpty();
}


/*!
    Constructs a style plugin. This is invoked automatically by the
    \c TQ_EXPORT_PLUGIN macro.
*/
TQStylePlugin::TQStylePlugin()
    : TQGPlugin( (TQStyleFactoryInterface*)(d = new TQStylePluginPrivate( this )) )
{
}

/*!
    Destroys the style plugin.

    You never have to call this explicitly. TQt destroys a plugin
    automatically when it is no longer used.
*/
TQStylePlugin::~TQStylePlugin()
{
    // don't delete d, as this is deleted by d
}

#endif // TQT_NO_COMPONENT
#endif // TQT_NO_STYLE
