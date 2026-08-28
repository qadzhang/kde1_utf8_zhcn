/****************************************************************************
**
** ...
**
** Copyright (C) 2001-2008 Trolltech ASA.  All rights reserved.
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

#include "ntqimageformatplugin.h"
#ifndef TQT_NO_IMAGEFORMATPLUGIN
#include "qimageformatinterface_p.h"
#include "ntqimage.h"

/*!
    \class TQImageFormatPlugin ntqimageformatplugin.h
    \brief The TQImageFormatPlugin class provides an abstract base for custom image format plugins.

    \ingroup plugins

    The image format plugin is a simple plugin interface that makes
    it easy to create custom image formats that can be used
    transparently by applications.

    Writing an image format plugin is achieved by subclassing this
    base class, reimplementing the pure virtual functions keys() and
    installIOHandler(), and exporting the class with the
    TQ_EXPORT_PLUGIN macro.  See the \link plugins-howto.html Plugins
    documentation\endlink for details.
*/

/*!
    \fn TQStringList TQImageFormatPlugin::keys() const

    Returns the list of image formats this plugin supports.

    \sa installIOHandler()
*/


/*!
    \fn  bool TQImageFormatPlugin::installIOHandler( const TQString &format )

    Installs a TQImageIO image I/O handler for the image format \a
    format.

    \sa keys()
*/

class TQImageFormatPluginPrivate : public TQImageFormatInterface
{
public:
    TQImageFormatPluginPrivate( TQImageFormatPlugin *p )
	: plugin( p )
    {
    }
    virtual ~TQImageFormatPluginPrivate();

    TQRESULT queryInterface( const TQUuid &iid, TQUnknownInterface **iface );
    TQ_REFCOUNT;

    TQStringList featureList() const;

    TQRESULT loadImage( const TQString &format, const TQString &filename, TQImage * );
    TQRESULT saveImage( const TQString &format, const TQString &filename, const TQImage & );

    TQRESULT installIOHandler( const TQString & );

private:
    TQImageFormatPlugin *plugin;
};

TQImageFormatPluginPrivate::~TQImageFormatPluginPrivate()
{
    delete plugin;
}

TQRESULT TQImageFormatPluginPrivate::queryInterface( const TQUuid &iid, TQUnknownInterface **iface )
{
    *iface = 0;

    if ( iid == IID_QUnknown )
	*iface = this;
    else if ( iid == IID_QFeatureList )
	*iface = this;
    else if ( iid == IID_QImageFormat )
	*iface = this;
    else
	return TQE_NOINTERFACE;

    (*iface)->addRef();
    return TQS_OK;
}

TQStringList TQImageFormatPluginPrivate::featureList() const
{
    return plugin->keys();
}

TQRESULT TQImageFormatPluginPrivate::loadImage( const TQString &format, const TQString &filename, TQImage *image )
{
    return plugin->loadImage( format, filename, image ) ? TQS_FALSE : TQS_OK;
}

TQRESULT TQImageFormatPluginPrivate::saveImage( const TQString &format, const TQString &filename, const TQImage &image )
{
    return plugin->saveImage( format, filename, image ) ? TQS_FALSE : TQS_OK;
}

TQRESULT TQImageFormatPluginPrivate::installIOHandler( const TQString &format )
{
    return plugin->installIOHandler( format ) ? TQS_FALSE : TQS_OK;
}

/*!
    Constructs an image format plugin. This is invoked automatically
    by the TQ_EXPORT_PLUGIN macro.
*/
TQImageFormatPlugin::TQImageFormatPlugin()
    : TQGPlugin( d = new TQImageFormatPluginPrivate( this ) )
{
}

/*!
    Destroys the image format plugin.

    You never have to call this explicitly. TQt destroys a plugin
    automatically when it is no longer used.
*/
TQImageFormatPlugin::~TQImageFormatPlugin()
{
}


/*!\internal
 */
bool TQImageFormatPlugin::loadImage( const TQString &format, const TQString &filename, TQImage *image )
{
    Q_UNUSED( format )
    Q_UNUSED( filename )
    Q_UNUSED( image )
    return false;
}

/*! \internal
 */
bool TQImageFormatPlugin::saveImage( const TQString &format, const TQString &filename, const TQImage &image )
{
    Q_UNUSED( format )
    Q_UNUSED( filename )
    Q_UNUSED( image )
    return false;
}

#endif // TQT_NO_IMAGEFORMATPLUGIN
