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

#ifndef COMMAND_H
#define COMMAND_H

#include "metadatabase.h"
#include "layout.h"

#include <ntqstring.h>
#include <ntqrect.h>
#include <ntqvaluelist.h>
#include <ntqvariant.h>
#include <ntqobject.h>
#include <ntqlistview.h>
#include <ntqptrlist.h>
#include <ntqmap.h>
#include <ntqiconset.h>

class TQWizard;
class TQTabWidget;
class Command;
class TQWidget;
class FormWindow;
class PropertyEditor;
class TQListBox;
class TQIconView;
class TQTable;
class TQAction;
class TQDesignerToolBar;
class TQMainWindow;
class TQTextEdit;
struct LanguageInterface;
class TQToolBox;

class MenuBarEditor;
class MenuBarEditorItem;
class PopupMenuEditor;
class PopupMenuEditorItem;
class ActionEditor;

class Command : public TQt
{
public:
    Command( const TQString &n, FormWindow *fw );
    virtual ~Command();

    enum Type {
	Resize,
	Insert,
	Move,
	Delete,
	SetProperty,
	LayoutHorizontal,
	LayoutVertical,
	LayoutHorizontalSplitter,
	LayoutVerticalSplitter,
	LayoutGrid,
	BreakLayout,
	Macro,
	AddTabPage,
	DeleteTabPage,
	MoveTabPage,
	AddWidgetStackPage,
	DeleteWidgetStackPage,
	AddWizardPage,
	DeleteWizardPage,
	SwapWizardPages,
	MoveWizardPage,
	RenameWizardPage,
	AddConnection,
	RemoveConnection,
	Lower,
	Raise,
	Paste,
	TabOrder,
	PopulateListBox,
	PopulateIconView,
	PopulateListView,
	PopulateMultiLineEdit,
	PopulateTable,
	AddActionToToolBar,
	RemoveActionFromToolBar,
	AddToolBar,
	RemoveToolBar,
	AddFunction,
	ChangeFunctionAttrib,
	RemoveFunction,
	AddVariable,
	SetVariables,
	RemoveVariable,
	EditDefinitions,
	AddContainerPage,
	DeleteContainerPage,
	RenameContainerPage,
	AddActionToPopup,
	RemoveActionFromPopup,
	ExchangeActionInPopup,
	//MoveActionInPopup,
	RenameAction,
	SetActionIcons,
	AddMenu,
	RemoveMenu,
	ExchangeMenu,
	MoveMenu,
	RenameMenu,
	AddToolBoxPage,
	DeleteToolBoxPage
    };

    TQString name() const;

    virtual void execute() = 0;
    virtual void unexecute() = 0;
    virtual Type type() const = 0;
    virtual void merge( Command *c );
    virtual bool canMerge( Command *c );

    FormWindow *formWindow() const;

private:
    TQString cmdName;
    FormWindow *formWin;

};

class CommandHistory : public TQObject
{
    TQ_OBJECT

public:
    CommandHistory( int s );

    void addCommand( Command *cmd, bool tryCompress = false );
    void undo();
    void redo();

    void emitUndoRedo();

    void setModified( bool m );
    bool isModified() const;

public slots:
    void checkCompressedCommand();

signals:
    void undoRedoChanged( bool undoAvailable, bool redoAvailable,
			  const TQString &undoCmd, const TQString &redoCmd );
    void modificationChanged( bool m );

private:
    TQPtrList<Command> history;
    int current, steps;
    bool modified;
    int savedAt;
    Command *compressedCommand;

};

class ResizeCommand : public Command
{
public:
    ResizeCommand( const TQString &n, FormWindow *fw,
		   TQWidget *w, const TQRect &oldr, const TQRect &nr );

    void execute();
    void unexecute();
    Type type() const { return Resize; }

private:
    TQWidget *widget;
    TQRect oldRect, newRect;

};

class InsertCommand : public Command
{
public:
    InsertCommand( const TQString &n, FormWindow *fw, TQWidget *w, const TQRect &g );

    void execute();
    void unexecute();
    Type type() const { return Insert; }

private:
    TQWidget *widget;
    TQRect geometry;

};

class MoveCommand : public Command
{
public:
    MoveCommand( const TQString &n, FormWindow *fw,
		 const TQWidgetList &w,
		 const TQValueList<TQPoint> op,
		 const TQValueList<TQPoint> np,
		 TQWidget *opr, TQWidget *npr );
    void execute();
    void unexecute();
    Type type() const { return Move; }
    void merge( Command *c );
    bool canMerge( Command *c );

private:
    TQWidgetList widgets;
    TQValueList<TQPoint> oldPos, newPos;
    TQWidget *oldParent, *newParent;

};

class DeleteCommand : public Command
{
public:
    DeleteCommand( const TQString &n, FormWindow *fw,
		   const TQWidgetList &w );
    void execute();
    void unexecute();
    Type type() const { return Delete; }

private:
    TQWidgetList widgets;
    TQMap< TQWidget*, TQValueList<MetaDataBase::Connection> > connections;

};

class SetPropertyCommand : public Command
{
public:
    SetPropertyCommand( const TQString &n, FormWindow *fw,
			TQObject *w, PropertyEditor *e,
			const TQString &pn, const TQVariant &ov,
			const TQVariant &nv, const TQString &ncut,
			const TQString &ocut,
			bool reset = false );

    void execute();
    void unexecute();
    Type type() const { return SetProperty; }
    void merge( Command *c );
    bool canMerge( Command *c );
    bool checkProperty();

private:
    void setProperty( const TQVariant &v, const TQString &currentItemText, bool select = true );

    TQGuardedPtr<TQObject> widget;
    PropertyEditor *editor;
    TQString propName;
    TQVariant oldValue, newValue;
    TQString oldCurrentItemText, newCurrentItemText;
    bool wasChanged;
    bool isResetCommand;

};

class LayoutHorizontalCommand : public Command
{
public:
    LayoutHorizontalCommand( const TQString &n, FormWindow *fw,
			     TQWidget *parent, TQWidget *layoutBase,
			     const TQWidgetList &wl );

    void execute();
    void unexecute();
    Type type() const { return LayoutHorizontal; }

private:
    HorizontalLayout layout;

};

class LayoutHorizontalSplitCommand : public Command
{
public:
    LayoutHorizontalSplitCommand( const TQString &n, FormWindow *fw,
				  TQWidget *parent, TQWidget *layoutBase,
				  const TQWidgetList &wl );

    void execute();
    void unexecute();
    Type type() const { return LayoutHorizontalSplitter; }

private:
    HorizontalLayout layout;

};

class LayoutVerticalCommand : public Command
{
public:
    LayoutVerticalCommand( const TQString &n, FormWindow *fw,
			   TQWidget *parent, TQWidget *layoutBase,
			   const TQWidgetList &wl );

    void execute();
    void unexecute();
    Type type() const { return LayoutVertical; }

private:
    VerticalLayout layout;

};

class LayoutVerticalSplitCommand : public Command
{
public:
    LayoutVerticalSplitCommand( const TQString &n, FormWindow *fw,
				TQWidget *parent, TQWidget *layoutBase,
				const TQWidgetList &wl );

    void execute();
    void unexecute();
    Type type() const { return LayoutVerticalSplitter; }

private:
    VerticalLayout layout;

};

class LayoutGridCommand : public Command
{
public:
    LayoutGridCommand( const TQString &n, FormWindow *fw,
		       TQWidget *parent, TQWidget *layoutBase,
		       const TQWidgetList &wl, int xres, int yres );

    void execute();
    void unexecute();
    Type type() const { return LayoutGrid; }

private:
    GridLayout layout;

};

class BreakLayoutCommand : public Command
{
public:
    BreakLayoutCommand( const TQString &n, FormWindow *fw,
			TQWidget *layoutBase, const TQWidgetList &wl );

    void execute();
    void unexecute();
    Type type() const { return BreakLayout; }

private:
    Layout *layout;
    int spacing;
    int margin;
    TQWidget *lb;
    TQWidgetList widgets;

};

class MacroCommand : public Command
{
public:
    MacroCommand( const TQString &n, FormWindow *fw,
		  const TQPtrList<Command> &cmds );

    void execute();
    void unexecute();
    Type type() const { return Macro; }

private:
    TQPtrList<Command> commands;

};

class AddTabPageCommand : public Command
{
public:
    AddTabPageCommand( const TQString &n, FormWindow *fw,
		       TQTabWidget *tw, const TQString &label );

    void execute();
    void unexecute();
    Type type() const { return AddTabPage; }

private:
    TQTabWidget *tabWidget;
    int index;
    TQWidget *tabPage;
    TQString tabLabel;

};

class DeleteTabPageCommand : public Command
{
public:
    DeleteTabPageCommand( const TQString &n, FormWindow *fw,
			  TQTabWidget *tw, TQWidget *page );

    void execute();
    void unexecute();
    Type type() const { return DeleteTabPage; }

private:
    TQTabWidget *tabWidget;
    int index;
    TQWidget *tabPage;
    TQString tabLabel;

};

class MoveTabPageCommand : public Command
{
public:
    MoveTabPageCommand(const TQString &n, FormWindow *fw,
		       TQTabWidget *tw, TQWidget *page,  const TQString& label, int nIndex, int oIndex );

    void execute();
    void unexecute();
    Type type() const { return MoveTabPage; }

private:
    TQTabWidget *tabWidget;
    int newIndex, oldIndex;
    TQWidget *tabPage;
    TQString tabLabel;

};
class TQDesignerWidgetStack;
class AddWidgetStackPageCommand : public Command
{
public:
    AddWidgetStackPageCommand( const TQString &n, FormWindow *fw,
			       TQDesignerWidgetStack *ws );

    void execute();
    void unexecute();
    Type type() const { return AddWidgetStackPage; }

private:
    TQDesignerWidgetStack *widgetStack;
    int index;
    TQWidget *stackPage;
};

class DeleteWidgetStackPageCommand : public Command
{
public:
    DeleteWidgetStackPageCommand( const TQString &n, FormWindow *fw,
				  TQDesignerWidgetStack *ws, TQWidget *page );

    void execute();
    void unexecute();
    Type type() const { return DeleteWidgetStackPage; }

private:
    TQDesignerWidgetStack *widgetStack;
    int index;
    TQWidget *stackPage;
};



class AddWizardPageCommand : public Command
{
public:
    AddWizardPageCommand( const TQString &n, FormWindow *fw,
			  TQWizard *w, const TQString &label, int index = -1, bool show = true );

    void execute();
    void unexecute();
    Type type() const { return AddWizardPage; }

private:
    TQWizard *wizard;
    int index;
    bool show;
    TQWidget *page;
    TQString pageLabel;

};

class DeleteWizardPageCommand : public Command
{
public:
    DeleteWizardPageCommand( const TQString &n, FormWindow *fw,
			     TQWizard *w, int index, bool show = true );

    void execute();
    void unexecute();
    Type type() const { return DeleteWizardPage; }

private:
    TQWizard *wizard;
    int index;
    bool show;
    TQWidget *page;
    TQString pageLabel;

};

class RenameWizardPageCommand : public Command
{
public:
    RenameWizardPageCommand( const TQString &n, FormWindow *fw,
			     TQWizard *w, int index, const TQString& name );

    void execute();
    void unexecute();
    Type type() const { return RenameWizardPage; }

private:
    TQWizard *wizard;
    int index;
    TQWidget *page;
    TQString label;

};

class SwapWizardPagesCommand : public Command
{
public:
    SwapWizardPagesCommand( const TQString &n, FormWindow *fw,
			      TQWizard *w, int index1, int index2 );

    void execute();
    void unexecute();
    Type type() const { return SwapWizardPages; }

private:
    TQWizard *wizard;
    int index1, index2;

};

class MoveWizardPageCommand : public Command
{
public:
    MoveWizardPageCommand( const TQString &n, FormWindow *fw,
			      TQWizard *w, int index1, int index2 );

    void execute();
    void unexecute();
    Type type() const { return MoveWizardPage; }

private:
    TQWizard *wizard;
    int index1, index2;

};

class AddConnectionCommand : public Command
{
public:
    AddConnectionCommand( const TQString &name, FormWindow *fw,
			  MetaDataBase::Connection c );

    void execute();
    void unexecute();
    Type type() const { return AddConnection; }

private:
    MetaDataBase::Connection connection;

};

class RemoveConnectionCommand : public Command
{
public:
    RemoveConnectionCommand( const TQString &name, FormWindow *fw,
			     MetaDataBase::Connection c );

    void execute();
    void unexecute();
    Type type() const { return RemoveConnection; }

private:
    MetaDataBase::Connection connection;

};


class AddFunctionCommand : public Command
{
public:
    AddFunctionCommand( const TQString &name, FormWindow *fw, const TQCString &f, const TQString& spec,
		        const TQString &a, const TQString &t, const TQString &l, const TQString &rt );

    void execute();
    void unexecute();
    Type type() const { return AddFunction; }

private:
    TQCString function;
    TQString specifier;
    TQString access;
    TQString functionType;
    TQString language;
    TQString returnType;
};

class ChangeFunctionAttribCommand : public Command
{
public:
    ChangeFunctionAttribCommand( const TQString &name, FormWindow *fw, MetaDataBase::Function f,
				 const TQString &on, const TQString &os, const TQString &oa,
				 const TQString &ot, const TQString &ol, const TQString &ort );

    void execute();
    void unexecute();
    Type type() const { return ChangeFunctionAttrib; }

private:
    TQString oldName, newName;
    TQString oldSpec, newSpec;
    TQString oldAccess, newAccess;
    TQString oldType, newType;
    TQString oldLang, newLang;
    TQString oldReturnType, newReturnType;
};

class RemoveFunctionCommand : public Command
{
public:
    RemoveFunctionCommand( const TQString &name, FormWindow *fw, const TQCString &f, const TQString& spec,
		           const TQString &a, const TQString &t, const TQString &l, const TQString &rt );

    void execute();
    void unexecute();
    Type type() const { return RemoveFunction; }

private:
    TQCString function;
    TQString specifier;
    TQString access;
    TQString functionType;
    TQString language;
    TQString returnType;
};

class AddVariableCommand : public Command
{
public:
    AddVariableCommand( const TQString &name, FormWindow *fw, const TQString &vn, const TQString &a );
    void execute();
    void unexecute();
    Type type() const { return AddVariable; }

private:
    TQString varName;
    TQString access;
};

class SetVariablesCommand : public Command
{
public:
    SetVariablesCommand( const TQString &name, FormWindow *fw, TQValueList<MetaDataBase::Variable> lst );
    void execute();
    void unexecute();
    Type type() const { return SetVariables; }

private:
    TQValueList<MetaDataBase::Variable> oldList, newList;
};

class RemoveVariableCommand : public Command
{
public:
    RemoveVariableCommand( const TQString &name, FormWindow *fw, const TQString &vn );
    void execute();
    void unexecute();
    Type type() const { return RemoveVariable; }

private:
    TQString varName;
    TQString access;
};

class EditDefinitionsCommand : public Command
{
public:
    EditDefinitionsCommand( const TQString &name, FormWindow *fw, LanguageInterface *lf,
			    const TQString &n, const TQStringList &l );
    void execute();
    void unexecute();
    Type type() const { return EditDefinitions; }

private:
    LanguageInterface *lIface;
    TQString defName;
    TQStringList newList, oldList;
};

class LowerCommand : public Command
{
public:
    LowerCommand( const TQString &name, FormWindow *fw, const TQWidgetList &w );

    void execute();
    void unexecute();
    Type type() const { return Lower; }

private:
    TQWidgetList widgets;

};

class RaiseCommand : public Command
{
public:
    RaiseCommand( const TQString &name, FormWindow *fw, const TQWidgetList &w );

    void execute();
    void unexecute();
    Type type() const { return Raise; }

private:
    TQWidgetList widgets;

};

class PasteCommand : public Command
{
public:
    PasteCommand( const TQString &n, FormWindow *fw, const TQWidgetList &w );

    void execute();
    void unexecute();
    Type type() const { return Paste; }

private:
    TQWidgetList widgets;

};

class TabOrderCommand : public Command
{
public:
    TabOrderCommand( const TQString &n, FormWindow *fw, const TQWidgetList &ol, const TQWidgetList &nl );

    void execute();
    void unexecute();
    Type type() const { return TabOrder; }
    void merge( Command *c );
    bool canMerge( Command *c );

private:
    TQWidgetList oldOrder, newOrder;

};

class PopulateListBoxCommand : public Command
{
public:
    struct Item
    {
	TQString text;
	TQPixmap pix;
    };

    PopulateListBoxCommand( const TQString &n, FormWindow *fw,
			    TQListBox *lb, const TQValueList<Item> &items );
    void execute();
    void unexecute();
    Type type() const { return PopulateListBox; }

    bool operator==( const PopulateListBoxCommand & ) const;

private:
    TQValueList<Item> oldItems, newItems;
    TQListBox *listbox;

};

class PopulateIconViewCommand : public Command
{
public:
    struct Item
    {
	TQString text;
	TQPixmap pix;
    };

    PopulateIconViewCommand( const TQString &n, FormWindow *fw,
			    TQIconView *iv, const TQValueList<Item> &items );
    void execute();
    void unexecute();
    Type type() const { return PopulateIconView; }

    bool operator==( const PopulateIconViewCommand & ) const;

private:
    TQValueList<Item> oldItems, newItems;
    TQIconView *iconview;

};

class PopulateListViewCommand : public Command
{
public:
    PopulateListViewCommand( const TQString &n, FormWindow *fw,
			     TQListView *lv, TQListView *from );
    void execute();
    void unexecute();
    Type type() const { return PopulateListView; }
    static void transferItems( TQListView *from, TQListView *to );

    bool operator==( const PopulateListViewCommand & ) const;

private:
    TQListView *oldItems, *newItems;
    TQListView *listview;

};

class PopulateMultiLineEditCommand : public Command
{
public:
    PopulateMultiLineEditCommand( const TQString &n, FormWindow *fw,
				  TQTextEdit *mle, const TQString &txt );
    void execute();
    void unexecute();
    Type type() const { return PopulateMultiLineEdit; }

private:
    TQString newText, oldText;
    TQTextEdit *mlined;
    bool wasChanged;

};

class PopulateTableCommand : public Command
{
public:
    struct Row
    {
	TQString text;
	TQPixmap pix;
    };

    struct Column
    {
	TQString text;
	TQPixmap pix;
	TQString field;
    };

    PopulateTableCommand( const TQString &n, FormWindow *fw, TQTable *t,
			  const TQValueList<Row> &rows,
			  const TQValueList<Column> &columns );

    void execute();
    void unexecute();
    Type type() const { return PopulateTable; }

private:
    TQValueList<Row> oldRows, newRows;
    TQValueList<Column> oldColumns, newColumns;
    TQTable *table;

};

class AddActionToToolBarCommand : public Command
{
public:
    AddActionToToolBarCommand( const TQString &n, FormWindow *fw,
			       TQAction *a, TQDesignerToolBar *tb, int idx );

    void execute();
    void unexecute();
    Type type() const { return AddActionToToolBar; }

private:
    TQAction *action;
    TQDesignerToolBar *toolBar;
    int index;

};

class RemoveActionFromToolBarCommand : public AddActionToToolBarCommand
{
public:
    RemoveActionFromToolBarCommand( const TQString &n, FormWindow *fw,
				    TQAction *a, TQDesignerToolBar *tb, int idx )
	: AddActionToToolBarCommand( n, fw, a, tb, idx ) {}

    void execute() { AddActionToToolBarCommand::unexecute(); }
    void unexecute() { AddActionToToolBarCommand::execute(); }
    Type type() const { return RemoveActionFromToolBar; }

};

// MenuCommands

class AddToolBarCommand : public Command
{
public:
    AddToolBarCommand( const TQString &n, FormWindow *fw, TQMainWindow *mw );

    void execute();
    void unexecute();
    Type type() const { return AddToolBar; }

protected:
    TQDesignerToolBar *toolBar;
    TQMainWindow *mainWindow;

};

class RemoveToolBarCommand : public AddToolBarCommand
{
public:
    RemoveToolBarCommand( const TQString &n, FormWindow *fw, TQMainWindow *mw, TQDesignerToolBar *tb )
	: AddToolBarCommand( n, fw, mw ) { toolBar = tb; }

    void execute() { AddToolBarCommand::unexecute(); }
    void unexecute() { AddToolBarCommand::execute(); }
    Type type() const { return RemoveToolBar; }

};

struct TQWidgetContainerInterfacePrivate;

class AddContainerPageCommand : public Command
{
public:
    AddContainerPageCommand( const TQString &n, FormWindow *fw,
			     TQWidget *c, const TQString &label );

    ~AddContainerPageCommand();

    void execute();
    void unexecute();
    Type type() const { return AddContainerPage; }

private:
    TQString wClassName;
    TQWidget *container;
    int index;
    TQString pageLabel;
    TQWidget *page;
    TQWidgetContainerInterfacePrivate *wiface;

};

class DeleteContainerPageCommand : public Command
{
public:
    DeleteContainerPageCommand( const TQString &n, FormWindow *fw,
				TQWidget *container, int index );
    ~DeleteContainerPageCommand();

    void execute();
    void unexecute();
    Type type() const { return DeleteContainerPage; }

private:
    TQString wClassName;
    TQWidget *container;
    int index;
    TQString pageLabel;
    TQWidget *page;
    TQWidgetContainerInterfacePrivate *wiface;

};

class RenameContainerPageCommand : public Command
{
public:
    RenameContainerPageCommand( const TQString &n, FormWindow *fw,
				TQWidget *container, int index,
				const TQString &label );
    ~RenameContainerPageCommand();

    void execute();
    void unexecute();
    Type type() const { return RenameContainerPage; }

private:
    TQString wClassName;
    TQWidget *container;
    int index;
    TQString oldLabel, newLabel;
    TQWidgetContainerInterfacePrivate *wiface;

};

class AddActionToPopupCommand : public Command
{
public:
    AddActionToPopupCommand( const TQString &n,
			     FormWindow *fw,
			     PopupMenuEditor *m,
			     PopupMenuEditorItem *i,
			     int idx = -1 );
    void execute();
    void unexecute();
    Type type() const { return AddActionToPopup; }
protected:
    TQString constructName() const;
    PopupMenuEditor *menu;
    PopupMenuEditorItem *item;
    int index;
private:
};

class RemoveActionFromPopupCommand : public AddActionToPopupCommand
{
public:
    RemoveActionFromPopupCommand( const TQString &n,
				  FormWindow *fw,
				  PopupMenuEditor *m,
				  int idx );
    void execute();
    void unexecute();
    Type type() const { return RemoveActionFromPopup; }
protected:
private:
};

class ExchangeActionInPopupCommand : public Command
{
public:
    ExchangeActionInPopupCommand( const TQString &n,
				  FormWindow *fw,
				  PopupMenuEditor *m,
				  int a,
				  int b );
    void execute();
    void unexecute();
    Type type() const { return ExchangeActionInPopup; }
protected:
private:
    PopupMenuEditor *menu;
    int c;
    int d;
};
/*
class MoveActionInPopupCommand : public Command
{
public:
    MoveActionInPopupCommand( const TQString &n,
			      FormWindow *fw,
			      PopupMenuEditor *m,
			      int a,
			      int b );
    void execute();
    void unexecute();
    Type type() const { return MoveActionInPopup; }
protected:
private:
    PopupMenuEditor *menu;
    int from;
    int to;
};
*/
class ActionCommand : public Command
{
public:
    ActionCommand( const TQString &n,
		   FormWindow *fw,
		   TQAction *a )
	: Command( n, fw ), action( a ) { }
    virtual Type type() const = 0;
protected:
    ActionEditor *actionEditor();
    TQAction *action;
};

class RenameActionCommand : public ActionCommand
{
public:
    RenameActionCommand( const TQString &n,
			 FormWindow *fw,
			 TQAction *a,
			 PopupMenuEditor *m,
			 TQString nm );
    void execute();
    void unexecute();
    Type type() const { return RenameAction; }
private:
    PopupMenuEditor *menu;
    TQString newName;
    TQString oldName;
};

class SetActionIconsCommand : public ActionCommand
{
public:
    SetActionIconsCommand( const TQString &n,
			   FormWindow *fw,
			   TQAction *a,
			   PopupMenuEditor *m,
			   TQIconSet &icons );
    void execute();
    void unexecute();
    Type type() const { return SetActionIcons; }
protected:
    void updateActionEditorItem();
private:
    PopupMenuEditor *menu;
    TQIconSet newIcons;
    TQIconSet oldIcons;
};

class AddMenuCommand : public Command
{
public:
    AddMenuCommand( const TQString &n,
		    FormWindow *fw,
		    MenuBarEditor *b,
		    MenuBarEditorItem *i,
		    int idx = -1 );

    AddMenuCommand( const TQString &n,
		    FormWindow *fw,
		    TQMainWindow *mw,
		    const TQString &nm = "Menu" );

    void execute();
    void unexecute();
    Type type() const { return AddMenu; }
protected:
    MenuBarEditor *mb;
    MenuBarEditorItem *item;
    TQString name;
    int index;
private:
};

class RemoveMenuCommand : public AddMenuCommand
{
public:
    RemoveMenuCommand( const TQString &n,
		       FormWindow *fw,
		       MenuBarEditor *b,
		       int idx );
    void execute();
    void unexecute();
    Type type() const { return RemoveMenu; }
protected:
private:
};

class ExchangeMenuCommand : public Command
{
public:
    ExchangeMenuCommand( const TQString &n,
			 FormWindow *fw,
			 MenuBarEditor *b,
			 int i,
			 int j );
    void execute();
    void unexecute();
    Type type() const { return ExchangeMenu; }
protected:
private:
    MenuBarEditor *bar;
    int k;
    int l;
};


class MoveMenuCommand : public Command
{
public:
    MoveMenuCommand( const TQString &n,
		     FormWindow *fw,
		     MenuBarEditor *b,
		     int i,
		     int j );
    void execute();
    void unexecute();
    Type type() const { return MoveMenu; }
protected:
private:
    MenuBarEditor *bar;
    int from;
    int to;
};

class RenameMenuCommand : public Command
{
public:
    RenameMenuCommand( const TQString &n,
		       FormWindow *fw,
		       MenuBarEditor *m,
		       TQString nm,
		       MenuBarEditorItem *i );
    void execute();
    void unexecute();
    Type type() const { return RenameMenu; }
    static TQString makeLegal( const TQString &str );
private:
    MenuBarEditor *bar;
    MenuBarEditorItem *item;
    TQString newName;
    TQString oldName;
};

class AddToolBoxPageCommand : public Command
{
public:
    AddToolBoxPageCommand( const TQString &n, FormWindow *fw,
			   TQToolBox *tw, const TQString &label );

    void execute();
    void unexecute();
    Type type() const { return AddToolBoxPage; }

private:
    TQToolBox *toolBox;
    int index;
    TQWidget *toolBoxPage;
    TQString toolBoxLabel;

};

class DeleteToolBoxPageCommand : public Command
{
public:
    DeleteToolBoxPageCommand( const TQString &n, FormWindow *fw,
			      TQToolBox *tw, TQWidget *page );

    void execute();
    void unexecute();
    Type type() const { return DeleteToolBoxPage; }

private:
    TQToolBox *toolBox;
    int index;
    TQWidget *toolBoxPage;
    TQString toolBoxLabel;

};

#endif
