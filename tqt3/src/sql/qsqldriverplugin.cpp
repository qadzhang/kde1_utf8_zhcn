/****************************************************************************
**
** Implementation of TQSqlDriverPlugin class
**
** Created : 2001-09-20
**
** Copyright (C) 2001-2008 Trolltech ASA.  All rights reserved.
**
** This file is part of the sql module of the TQt GUI Toolkit.
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

#include "ntqsqldriverplugin.h"

#ifndef TQT_NO_SQL
#ifndef TQT_NO_COMPONENT

#include "qsqldriverinterface_p.h"

/*!
    \class TQSqlDriverPlugin ntqsqldriverplugin.h
    \brief The TQSqlDriverPlugin class provides an abstract base for custom TQSqlDriver plugins.

    \ingroup plugins
    \mainclass

    The SQL driver plugin is a simple plugin interface that makes it
    easy to create your own SQL driver plugins that can be loaded
    dynamically by TQt.

    Writing a SQL plugin is achieved by subclassing this base class,
    reimplementing the pure virtual functions keys() and create(), and
    exporting the class with the \c TQ_EXPORT_PLUGIN macro. See the SQL
    plugins that come with TQt for example implementations (in the
    \c{plugins/src/sqldrivers} subdirectory of the source
    distribution). Read the \link plugins-howto.html plugins
    documentation\endlink for more information on plugins.
*/

/*!
    \fn TQStringList TQSqlDriverPlugin::keys() const

    Returns the list of drivers (keys) this plugin supports.

    These keys are usually the class names of the custom drivers that
    are implemented in the plugin.

    \sa create()
*/

/*!
    \fn TQSqlDriver* TQSqlDriverPlugin::create( const TQString& key )

    Creates and returns a TQSqlDriver object for the driver key \a key.
    The driver key is usually the class name of the required driver.

    \sa keys()
*/

class TQSqlDriverPluginPrivate : public TQSqlDriverFactoryInterface
{
public:
    TQSqlDriverPluginPrivate( TQSqlDriverPlugin *p )
	: plugin( p )
    {
    }
    virtual ~TQSqlDriverPluginPrivate();

    TQRESULT queryInterface( const TQUuid &iid, TQUnknownInterface **iface );
    TQ_REFCOUNT;

    TQStringList featureList() const;
    TQSqlDriver *create( const TQString &key );

private:
    TQSqlDriverPlugin *plugin;
};

TQSqlDriverPluginPrivate::~TQSqlDriverPluginPrivate()
{
    delete plugin;
}

TQRESULT TQSqlDriverPluginPrivate::queryInterface( const TQUuid &iid, TQUnknownInterface **iface )
{
    *iface = 0;

    if ( iid == IID_QUnknown )
	*iface = this;
    else if ( iid == IID_QFeatureList )
	*iface = this;
    else if ( iid == IID_QSqlDriverFactory )
	*iface = this;
    else
	return TQE_NOINTERFACE;

    (*iface)->addRef();
    return TQS_OK;
}

TQStringList TQSqlDriverPluginPrivate::featureList() const
{
    return plugin->keys();
}

TQSqlDriver *TQSqlDriverPluginPrivate::create( const TQString &key )
{
    return plugin->create( key );
}

/*!
    Constructs a SQL driver plugin. This is invoked automatically by
    the \c TQ_EXPORT_PLUGIN macro.
*/

TQSqlDriverPlugin::TQSqlDriverPlugin()
    : TQGPlugin( d = new TQSqlDriverPluginPrivate( this ) )
{
}

/*!
    Destroys the SQL driver plugin.

    You never have to call this explicitly. TQt destroys a plugin
    automatically when it is no longer used.
*/
TQSqlDriverPlugin::~TQSqlDriverPlugin()
{
    // don't delete d, as this is deleted by d
}

#endif // TQT_NO_COMPONENT
#endif // TQT_NO_SQL
