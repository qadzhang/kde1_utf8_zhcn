/****************************************************************************
**
** ...
**
** Copyright (C) 2000-2008 Trolltech ASA.  All rights reserved.
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

#ifndef TQWIDGETINTERFACE_P_H
#define TQWIDGETINTERFACE_P_H

//
//  W A R N I N G
//  -------------
//
// This file is not part of the TQt API.  It exists for the convenience
// of a number of TQt sources files.  This header file may change from
// version to version without notice, or even be removed.
//
// We mean it.
//
//

#ifndef QT_H
#include <private/qcom_p.h>
#include "ntqiconset.h"
#endif // QT_H

#ifndef TQT_NO_WIDGETPLUGIN

class TQWidget;

// {55184143-f18f-42c0-a8eb-71c01516019a}
#ifndef IID_QWidgetFactory
#define IID_QWidgetFactory TQUuid( 0x55184143, 0xf18f, 0x42c0, 0xa8, 0xeb, 0x71, 0xc0, 0x15, 0x16, 0x1, 0x9a )
#endif

/*! To add custom widgets to the TQt Designer, implement that interface
  in your custom widget plugin.

  You also have to implement the function featureList() (\sa
  TQFeatureListInterface) and return there all widgets (names of it)
  which this interface provides.
*/

struct TQWidgetFactoryInterface : public TQFeatureListInterface
{
public:

    /*! In the implementation create and return the widget \a widget
      here, use \a parent and \a name when creating the widget */
    virtual TQWidget* create( const TQString &widget, TQWidget* parent = 0, const char* name = 0 ) = 0;

    /*! In the implementation return the name of the group of the
      widget \a widget */
    virtual TQString group( const TQString &widget ) const = 0;

    /*! In the implementation return the iconset, which should be used
      in the TQt Designer menubar and toolbar to represent the widget
      \a widget */
    virtual TQIconSet iconSet( const TQString &widget ) const = 0;

    /*! In the implementation return the include file which is needed
      for the widget \a widget in the generated code which uic
      generates. */
    virtual TQString includeFile( const TQString &widget ) const = 0;

    /*! In the implementation return the text which should be
      displayed as tooltip for the widget \a widget */
    virtual TQString toolTip( const TQString &widget ) const = 0;

    /*! In the implementation return the text which should be used for
      what's this help for the widget \a widget. */
    virtual TQString whatsThis( const TQString &widget ) const = 0;

    /*! In the implementation return true here, of the \a widget
      should be able to contain other widget in the TQt Designer, else
      false. */
    virtual bool isContainer( const TQString &widget ) const = 0;
};

#ifdef QT_CONTAINER_CUSTOM_WIDGETS
// {15976628-e3c3-47f4-b525-d124a3caf30e}
#ifndef IID_QWidgetContainer
#define IID_QWidgetContainer TQUuid( 0x15976628, 0xe3c3, 0x47f4, 0xb5, 0x25, 0xd1, 0x24, 0xa3, 0xca, 0xf3, 0x0e )
#endif

struct TQWidgetContainerInterfacePrivate : public TQUnknownInterface
{
public:
    virtual TQWidget *containerOfWidget( const TQString &f, TQWidget *container ) const = 0;
    virtual bool isPassiveInteractor( const TQString &f, TQWidget *container ) const = 0;

    virtual bool supportsPages( const TQString &f ) const = 0;

    virtual TQWidget *addPage( const TQString &f, TQWidget *container,
			      const TQString &name, int index ) const = 0;
    virtual void insertPage( const TQString &f, TQWidget *container,
			     const TQString &name, int index, TQWidget *page ) const = 0;
    virtual void removePage( const TQString &f, TQWidget *container, int index ) const = 0;
    virtual void movePage( const TQString &f, TQWidget *container, int fromIndex, int toIndex ) const = 0;
    virtual int count( const TQString &key, TQWidget *container ) const = 0;
    virtual int currentIndex( const TQString &key, TQWidget *container ) const = 0;
    virtual TQString pageLabel( const TQString &key, TQWidget *container, int index ) const = 0;
    virtual TQWidget *page( const TQString &key, TQWidget *container, int index ) const = 0;
    virtual void renamePage( const TQString &key, TQWidget *container,
			     int index, const TQString &newName ) const = 0;
    virtual TQWidgetList pages( const TQString &f, TQWidget *container ) const = 0;
    virtual TQString createCode( const TQString &f, const TQString &container,
				const TQString &page, const TQString &pageName ) const = 0;
};

#endif // QT_CONTAINER_CUSTOM_WIDGETS
#endif // TQT_NO_WIDGETPLUGIN
#endif // TQWIDGETINTERFACE_P_H
