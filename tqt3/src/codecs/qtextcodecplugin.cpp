/****************************************************************************
**
** Implementation of TQTextCodecPlugin class
**
** Created : 010920
**
** Copyright (C) 2001-2008 Trolltech ASA.  All rights reserved.
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

#include "ntqtextcodecplugin.h"
#ifndef TQT_NO_TEXTCODECPLUGIN
#include "qtextcodecinterface_p.h"

/*!
    \class TQTextCodecPlugin ntqtextcodecplugin.h
    \brief The TQTextCodecPlugin class provides an abstract base for custom TQTextCodec plugins.
    \reentrant
    \ingroup plugins

    The text codec plugin is a simple plugin interface that makes it
    easy to create custom text codecs that can be loaded dynamically
    into applications.

    Writing a text codec plugin is achieved by subclassing this base
    class, reimplementing the pure virtual functions names(),
    createForName(), mibEnums() and createForMib(), and exporting the
    class with the \c TQ_EXPORT_PLUGIN macro. See the \link
    plugins-howto.html TQt Plugins documentation \endlink for details.

    See the \link http://www.iana.org/assignments/character-sets IANA
    character-sets encoding file\endlink for more information on mime
    names and mib enums.
*/

/*!
    \fn TQStringList TQTextCodecPlugin::names() const

    Returns the list of mime names supported by this plugin.

    \sa createForName()
*/

/*!
    \fn TQTextCodec *TQTextCodecPlugin::createForName( const TQString &name );

    Creates a TQTextCodec object for the codec called \a name.

    \sa names()
*/


/*!
    \fn TQValueList<int> TQTextCodecPlugin::mibEnums() const

    Returns the list of mib enums supported by this plugin.

    \sa createForMib()
*/

/*!
    \fn TQTextCodec *TQTextCodecPlugin::createForMib( int mib );

    Creates a TQTextCodec object for the mib enum \a mib.

    (See \link
    ftp://ftp.isi.edu/in-notes/iana/assignments/character-sets the
    IANA character-sets encoding file\endlink for more information)

    \sa mibEnums()
*/



class TQTextCodecPluginPrivate : public TQTextCodecFactoryInterface
{
public:
    TQTextCodecPluginPrivate( TQTextCodecPlugin *p )
	: plugin( p )
    {
    }
    virtual ~TQTextCodecPluginPrivate();

    TQRESULT queryInterface( const TQUuid &iid, TQUnknownInterface **iface );
    TQ_REFCOUNT;

    TQStringList featureList() const;
    TQTextCodec *createForMib( int mib );
    TQTextCodec *createForName( const TQString &name );

private:
    TQTextCodecPlugin *plugin;
};

TQTextCodecPluginPrivate::~TQTextCodecPluginPrivate()
{
    delete plugin;
}

TQRESULT TQTextCodecPluginPrivate::queryInterface( const TQUuid &iid, TQUnknownInterface **iface )
{
    *iface = 0;

    if ( iid == IID_QUnknown )
	*iface = this;
    else if ( iid == IID_QFeatureList )
	*iface = this;
    else if ( iid == IID_QTextCodecFactory )
	*iface = this;
    else
	return TQE_NOINTERFACE;

    (*iface)->addRef();
    return TQS_OK;
}

TQStringList TQTextCodecPluginPrivate::featureList() const
{
    TQStringList keys = plugin->names();
    TQValueList<int> mibs = plugin->mibEnums();
    for ( TQValueList<int>::Iterator it = mibs.begin(); it != mibs.end(); ++it )
	keys += TQString("MIB-%1").arg( *it );
    return keys;
}

TQTextCodec *TQTextCodecPluginPrivate::createForMib( int mib )
{
    return plugin->createForMib( mib );
}

TQTextCodec *TQTextCodecPluginPrivate::createForName( const TQString &name )
{
    return plugin->createForName( name );
}


/*!
    Constructs a text codec plugin. This is invoked automatically by
    the \c TQ_EXPORT_PLUGIN macro.
*/
TQTextCodecPlugin::TQTextCodecPlugin()
    : TQGPlugin( d = new TQTextCodecPluginPrivate( this ) )
{
}

/*!
    Destroys the text codec plugin.

    You never have to call this explicitly. TQt destroys a plugin
    automatically when it is no longer used.
*/
TQTextCodecPlugin::~TQTextCodecPlugin()
{
}

#endif // TQT_NO_TEXTCODECPLUGIN
