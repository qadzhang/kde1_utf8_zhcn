/**********************************************************************
** Copyright (C) 2000-2008 Trolltech ASA.  All rights reserved.
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

#ifndef EDITFUNCTIONSIMPL_H
#define EDITFUNCTIONSIMPL_H

#include "editfunctions.h"
#include "hierarchyview.h"
#include "metadatabase.h"
#include <ntqmap.h>

class FormWindow;
class TQListViewItem;

class EditFunctions : public EditFunctionsBase
{
    TQ_OBJECT

public:
    EditFunctions( TQWidget *parent, FormWindow *fw, bool showOnlySlots = false );

    void setCurrentFunction( const TQString &function );
    void functionAdd( const TQString &access = TQString::null,
		      const TQString &type = TQString::null  );
    void functionAdd() { functionAdd( "public" ); }

signals:
    void itemRenamed(const TQString &);

protected slots:
    void okClicked();
    void functionRemove();
    void currentItemChanged( TQListViewItem * );
    void currentTextChanged( const TQString &txt );
    void currentSpecifierChanged( const TQString &s );
    void currentAccessChanged( const TQString &a );
    void currentReturnTypeChanged( const TQString &type );
    void currentTypeChanged( const TQString &type );
    void displaySlots( bool justSlots );
    void emitItemRenamed( TQListViewItem *, int, const TQString & );

private:
    enum Attribute { Name, Specifier, Access, ReturnType, Type };
    struct FunctItem {
	int id;
	TQString oldName;
	TQString newName;
	TQString oldRetTyp;
	TQString retTyp;
	TQString spec;
	TQString oldSpec;
	TQString access;
	TQString oldAccess;
	TQString type;
	TQString oldType;
    };

    void changeItem( TQListViewItem *item, Attribute a, const TQString &nV );

    FormWindow *formWindow;
    TQMap<TQListViewItem*, int> functionIds;
    TQStringList removedFunctions;
    TQValueList<MetaDataBase::Function> itemList;
    TQValueList<FunctItem> functList;
    int id;
    TQString lastType;
};

#endif
