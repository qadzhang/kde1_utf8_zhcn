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

#ifndef CUSTOMWIDGETEDITORIMPL_H
#define CUSTOMWIDGETEDITORIMPL_H

#include "customwidgeteditor.h"
#include "metadatabase.h"

#include <ntqmap.h>
#include <ntqobjectlist.h>

class TQTimer;
class TQListBoxItem;
class MainWindow;

class CustomWidgetEditor : public CustomWidgetEditorBase
{
    TQ_OBJECT

public:
    CustomWidgetEditor( TQWidget *parent , MainWindow *mw);

protected slots:
    void addWidgetClicked();
    void classNameChanged( const TQString & );
    void currentWidgetChanged( TQListBoxItem * );
    void deleteWidgetClicked();
    void headerFileChanged( const TQString & );
    void heightChanged( int );
    void includePolicyChanged( int );
    void pixmapChoosen();
    void widthChanged( int );
    void chooseHeader();
    void checkWidgetName();
    void closeClicked();
    void currentSignalChanged( TQListBoxItem *i );
    void addSignal();
    void removeSignal();
    void signalNameChanged( const TQString &s );
    void slotAccessChanged( const TQString & );
    void slotNameChanged( const TQString & );
    void addSlot();
    void removeSlot();
    void currentSlotChanged( TQListViewItem * );
    void propertyTypeChanged( const TQString & );
    void propertyNameChanged( const TQString & );
    void addProperty();
    void removeProperty();
    void currentPropertyChanged( TQListViewItem * );
    void saveDescription();
    void loadDescription();
    void horDataChanged( int );
    void verDataChanged( int );
    void widgetIsContainer( bool );
    
private:
    MetaDataBase::CustomWidget *findWidget( TQListBoxItem *i );
    void setupDefinition();
    void setupSignals();
    void setupSlots();
    void setupProperties();
    void updateCustomWidgetSizes();

private:
    TQMap<TQListBoxItem*, MetaDataBase::CustomWidget*> customWidgets;
    TQString oldName;
    TQTimer *checkTimer;
    TQListBoxItem *oldItem;
    MainWindow *mainWindow;
    TQObjectList cwLst;

};

#endif
