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

#ifndef TABLEEDITORIMPL_H
#define TABLEEDITORIMPL_H

#include "tableeditor.h"
#include <ntqmap.h>

class TQListBoxItem;
class TQTable;
class FormWindow;

class TableEditor : public TableEditorBase
{
    TQ_OBJECT

public:
    TableEditor( TQWidget* parent = 0, TQWidget *editWidget = 0, FormWindow *fw = 0,
		 const char* name = 0, bool modal = false, WFlags fl = 0 );
    ~TableEditor();

protected slots:
    void columnDownClicked();
    void columnTextChanged( const TQString & );
    void columnUpClicked();
    void currentColumnChanged( TQListBoxItem * );
    void currentFieldChanged( const TQString & );
    void currentRowChanged( TQListBoxItem * );
    void deleteColumnClicked();
    void deleteRowClicked();
    void newColumnClicked();
    void newRowClicked();
    void okClicked();
    void rowDownClicked();
    void rowTextChanged( const TQString & );
    void rowUpClicked();
    void applyClicked();
    void chooseRowPixmapClicked();
    void deleteRowPixmapClicked();
    void chooseColPixmapClicked();
    void deleteColPixmapClicked();

private:
    void readFromTable();
    void readColumns();
    void readRows();
    void saveFieldMap();
    void restoreFieldMap();

private:
    TQTable *editTable;
    FormWindow *formWindow;
    TQMap<int, TQString> fieldMap;
    TQMap<TQListBoxItem*, TQString> tmpFieldMap;

};

#endif // TABLEEDITOR_H
