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

#ifndef LISTVIEWEDITORIMPL_H
#define LISTVIEWEDITORIMPL_H

#include "listvieweditor.h"

#include <ntqmap.h>
#include <ntqpixmap.h>
#include <ntqvaluelist.h>

class FormWindow;

class ListViewEditor : public ListViewEditorBase
{
    TQ_OBJECT

public:
    ListViewEditor( TQWidget *parent, TQListView *lv, FormWindow *fw );

signals:
    void itemRenamed(const TQString &);

protected slots:
    void applyClicked();
    void columnClickable(bool);
    void columnDownClicked();
    void columnPixmapChosen();
    void columnPixmapDeleted();
    void columnResizable(bool);
    void columnTextChanged(const TQString &);
    void columnUpClicked();
    void currentColumnChanged(TQListBoxItem*);
    void currentItemChanged(TQListViewItem*);
    void deleteColumnClicked();
    void itemColChanged(int);
    void itemDeleteClicked();
    void itemDownClicked();
    void itemNewClicked();
    void itemNewSubClicked();
    void itemPixmapChoosen();
    void itemPixmapDeleted();
    void itemTextChanged(const TQString &);
    void itemUpClicked();
    void itemLeftClicked();
    void itemRightClicked();
    void newColumnClicked();
    void okClicked();
    void initTabPage(const TQString &page);
    void emitItemRenamed(TQListViewItem*, int, const TQString&); // signal relay

private:
    struct Column
    {
	TQListBoxItem *item;
	TQString text;
	TQPixmap pixmap;
	bool clickable, resizable;
    };

private:
    void setupColumns();
    void setupItems();
    Column *findColumn( TQListBoxItem *i );
    void transferItems( TQListView *from, TQListView *to );
    void displayItem( TQListViewItem *i, int col );

private:
    TQListView *listview;
    TQValueList<Column> columns;
    int numColumns;
    FormWindow *formwindow;

};


#endif
