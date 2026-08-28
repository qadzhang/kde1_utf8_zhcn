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

#ifndef HIRARCHYVIEW_H
#define HIRARCHYVIEW_H

#include <ntqvariant.h>
#include <ntqlistview.h>
#include <ntqtabwidget.h>
#include <ntqguardedptr.h>
#include <private/qcom_p.h>
#include "../interfaces/classbrowserinterface.h"

class FormWindow;
class TQCloseEvent;
class TQPopupMenu;
class TQKeyEvent;
class TQMouseEvent;
class TQWizard;
class SourceEditor;

class HierarchyItem : public TQListViewItem
{
public:
    enum Type {
	Widget,
	SlotParent,
	SlotPublic,
	SlotProtected,
	SlotPrivate,
	Slot,
	DefinitionParent,
	Definition,
	Event,
	EventFunction,
	FunctParent,
	FunctPublic,
	FunctProtected,
	FunctPrivate,
	Function,
	VarParent,
	VarPublic,
	VarProtected,
	VarPrivate,
	Variable
    };

    HierarchyItem( Type type, TQListViewItem *parent, TQListViewItem *after,
		   const TQString &txt1, const TQString &txt2, const TQString &txt3 );
    HierarchyItem( Type type, TQListView *parent, TQListViewItem *after,
		   const TQString &txt1, const TQString &txt2, const TQString &txt3 );

    void paintCell( TQPainter *p, const TQColorGroup &cg, int column, int width, int align );
    void updateBackColor();

    void setObject( TQObject *o );
    TQObject *object() const;

    void setText( int col, const TQString &txt ) { if ( !txt.isEmpty() ) TQListViewItem::setText( col, txt ); }

    int rtti() const { return (int)typ; }

private:
    void okRename( int col );
    void cancelRename( int col );

private:
    TQColor backgroundColor();
    TQColor backColor;
    TQObject *obj;
    Type typ;

};

class HierarchyList : public TQListView
{
    TQ_OBJECT

public:
    HierarchyList( TQWidget *parent, FormWindow *fw, bool doConnects = true );

    virtual void setup();
    virtual void setCurrent( TQObject *o );
    void setOpen( TQListViewItem *i, bool b );
    void changeNameOf( TQObject *o, const TQString &name );
    void changeDatabaseOf( TQObject *o, const TQString &info );
    void setFormWindow( FormWindow *fw ) { formWindow = fw; }
    void drawContentsOffset( TQPainter *p, int ox, int oy,
			     int cx, int cy, int cw, int ch ) {
	setUpdatesEnabled( false );
	triggerUpdate();
	setUpdatesEnabled( true );
	TQListView::drawContentsOffset( p, ox, oy, cx, cy, cw, ch );
    }

    void insertEntry( TQListViewItem *i, const TQPixmap &pix = TQPixmap(), const TQString &s = TQString::null );

protected:
    void keyPressEvent( TQKeyEvent *e );
    void keyReleaseEvent( TQKeyEvent *e );
    void viewportMousePressEvent( TQMouseEvent *e );
    void viewportMouseReleaseEvent( TQMouseEvent *e );

public slots:
    void addTabPage();
    void removeTabPage();

private:
    void insertObject( TQObject *o, TQListViewItem *parent );
    TQObject *findObject( TQListViewItem *i );
    TQListViewItem *findItem( TQObject *o );
    TQObject *current() const;
    TQObject *handleObjectClick( TQListViewItem *i );

private slots:
    virtual void objectClicked( TQListViewItem *i );
    virtual void objectDoubleClicked( TQListViewItem *i );
    virtual void showRMBMenu( TQListViewItem *, const TQPoint & );

protected:
    FormWindow *formWindow;
    TQPopupMenu *normalMenu, *tabWidgetMenu;
    bool deselect;

};

class FormDefinitionView : public HierarchyList
{
    TQ_OBJECT

public:
    FormDefinitionView( TQWidget *parent, FormWindow *fw );

    void setup();
    void setupVariables();
    void refresh();
    void setCurrent( TQWidget *w );

protected:
    void contentsMouseDoubleClickEvent( TQMouseEvent *e );

private:
    void save( TQListViewItem *p, TQListViewItem *i );
    void execFunctionDialog( const TQString &access, const TQString &type, bool addFunc );
    void addVariable( const TQString &varName, const TQString &access );

private slots:
    void objectClicked( TQListViewItem *i );
    void showRMBMenu( TQListViewItem *, const TQPoint & );
    void renamed( TQListViewItem *i );

private:
    bool popupOpen;
    HierarchyItem *itemSlots, *itemPrivate, *itemProtected, *itemPublic;
    HierarchyItem *itemFunct, *itemFunctPriv, *itemFunctProt, *itemFunctPubl;
    HierarchyItem *itemVar, *itemVarPriv, *itemVarProt, *itemVarPubl;
};


class HierarchyView : public TQTabWidget
{
    TQ_OBJECT

public:
    HierarchyView( TQWidget *parent );
    ~HierarchyView();

    void setFormWindow( FormWindow *fw, TQObject *o );
    FormWindow *formWindow() const;
    SourceEditor *sourceEditor() const { return editor; }
    void clear();

    void showClasses( SourceEditor *se );
    void updateClassBrowsers();

    void widgetInserted( TQWidget *w );
    void widgetRemoved( TQWidget *w );
    void widgetsInserted( const TQWidgetList &l );
    void widgetsRemoved( const TQWidgetList &l );
    void namePropertyChanged( TQWidget *w, const TQVariant &old );
    void databasePropertyChanged( TQWidget *w, const TQStringList& info );
    void tabsChanged( TQTabWidget *w );
    void pagesChanged( TQWizard *w );
    void rebuild();
    void closed( FormWindow *fw );
    void updateFormDefinitionView();

    FormDefinitionView *formDefinitionView() const { return fView; }
    HierarchyList *hierarchyList() const { return listview; }

protected slots:
    void jumpTo( const TQString &func, const TQString &clss,int type );
    void showClassesTimeout();

protected:
    void closeEvent( TQCloseEvent *e );

signals:
    void hidden();

private:
    struct ClassBrowser
    {
	ClassBrowser( TQListView * = 0, ClassBrowserInterface * = 0 );
	~ClassBrowser();
	TQListView *lv;
	TQInterfacePtr<ClassBrowserInterface> iface;
    };
    FormWindow *formwindow;
    HierarchyList *listview;
    FormDefinitionView *fView;
    SourceEditor *editor;
    TQMap<TQString, ClassBrowser> *classBrowsers;
    TQGuardedPtr<SourceEditor> lastSourceEditor;

};


#endif
