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

#ifndef WIDGETDATABASE_H
#define WIDGETDATABASE_H

#include <ntqiconset.h>
#include <ntqstring.h>
#include "../interfaces/widgetinterface.h" // up here for GCC 2.7.* compatibility
#include <private/qpluginmanager_p.h>


extern TQPluginManager<WidgetInterface> *widgetManager();

struct WidgetDatabaseRecord
{
    WidgetDatabaseRecord();
    ~WidgetDatabaseRecord();
    TQString iconSet, name, group, toolTip, whatsThis, includeFile;
    uint isContainer : 1;
    uint isForm : 1;
    uint isCommon : 1;
    uint isPlugin : 1;
    TQIconSet *icon;
    int nameCounter;
};

class WidgetDatabase : public TQt
{
public:
    WidgetDatabase();
    static void setupDataBase( int id );
    static void setupPlugins();

    static int count();
    static int startCustom();

    static TQIconSet iconSet( int id );
    static TQString className( int id );
    static TQString group( int id );
    static TQString toolTip( int id );
    static TQString whatsThis( int id );
    static TQString includeFile( int id );
    static bool isForm( int id );
    static bool isContainer( int id );
    static bool isCommon( int id );

    static int idFromClassName( const TQString &name );
    static TQString createWidgetName( int id );

    static WidgetDatabaseRecord *at( int index );
    static void insert( int index, WidgetDatabaseRecord *r );
    static void append( WidgetDatabaseRecord *r );

    static TQString widgetGroup( const TQString &g );
    static TQString widgetGroup( int i );
    static int numWidgetGroups();
    static bool isGroupVisible( const TQString &g );
    static bool isGroupEmpty( const TQString &grp );

    static int addCustomWidget( WidgetDatabaseRecord *r );
    static bool isCustomWidget( int id );
    static bool isCustomPluginWidget( int id );

    static bool isWhatsThisLoaded();
    static void loadWhatsThis( const TQString &docPath );

    static bool hasWidget( const TQString &name );
    static void customWidgetClassNameChanged( const TQString &oldName, const TQString &newName );

};

#endif
