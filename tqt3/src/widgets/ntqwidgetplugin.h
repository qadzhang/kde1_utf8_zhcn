/****************************************************************************
**
** Definition of TQWidgetPlugin class
**
** Created : 010920
**
** Copyright (C) 2001-2008 Trolltech ASA.  All rights reserved.
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

#ifndef TQWIDGETPLUGIN_H
#define TQWIDGETPLUGIN_H

#ifndef QT_H
#include "ntqgplugin.h"
#include "ntqstringlist.h"
#include "ntqiconset.h"
#endif // QT_H
#ifndef TQT_NO_WIDGETPLUGIN

#ifdef TQ_WS_WIN
#ifdef QT_PLUGIN
#define QT_WIDGET_PLUGIN_EXPORT __declspec(dllexport)
#else
#define QT_WIDGET_PLUGIN_EXPORT __declspec(dllimport)
#endif
#else
#define QT_WIDGET_PLUGIN_EXPORT
#endif

class TQWidgetPluginPrivate;
class TQWidget;

class TQ_EXPORT TQWidgetPlugin : public TQGPlugin
{
    TQ_OBJECT
public:
    TQWidgetPlugin();
    ~TQWidgetPlugin();

    virtual TQStringList keys() const = 0;
    virtual TQWidget *create( const TQString &key, TQWidget *parent = 0, const char *name = 0 ) = 0;

    virtual TQString group( const TQString &key ) const;
    virtual TQIconSet iconSet( const TQString &key ) const;
    virtual TQString includeFile( const TQString &key ) const;
    virtual TQString toolTip( const TQString &key ) const;
    virtual TQString whatsThis( const TQString &key ) const;
    virtual bool isContainer( const TQString &key ) const;

private:
    TQWidgetPluginPrivate *d;
};

#ifdef QT_CONTAINER_CUSTOM_WIDGETS

class TQWidgetContainerPluginPrivate;

class TQ_EXPORT TQWidgetContainerPlugin : public TQWidgetPlugin
{

public:
    TQWidgetContainerPlugin();
    ~TQWidgetContainerPlugin();

    virtual TQWidget* containerOfWidget( const TQString &key, TQWidget *container ) const;
    virtual bool isPassiveInteractor( const TQString &key, TQWidget *container ) const;

    virtual bool supportsPages( const TQString &key ) const;

    virtual TQWidget *addPage( const TQString &key, TQWidget *container,
			      const TQString &name, int index ) const;
    virtual void insertPage( const TQString &key, TQWidget *container,
			     const TQString &name, int index, TQWidget *page ) const;
    virtual void removePage( const TQString &key, TQWidget *container, int index ) const;
    virtual void movePage( const TQString &key, TQWidget *container, int fromIndex, int toIndex ) const;
    virtual int count( const TQString &key, TQWidget *container ) const;
    virtual int currentIndex( const TQString &key, TQWidget *container ) const;
    virtual TQString pageLabel( const TQString &key, TQWidget *container, int index ) const;
    virtual TQWidget *page( const TQString &key, TQWidget *container, int index ) const;
    virtual void renamePage( const TQString &key, TQWidget *container,
			     int index, const TQString &newName ) const;
    virtual TQWidgetList pages( const TQString &key, TQWidget *container ) const;
    virtual TQString createCode( const TQString &key, const TQString &container,
				const TQString &page, const TQString &pageName ) const;
};

#endif // QT_CONTAINER_CUSTOM_WIDGETS
#endif // TQT_NO_WIDGETPLUGIN
#endif // TQWIDGETPLUGIN_H
