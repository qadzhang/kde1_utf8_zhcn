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

#include "mainwindow.h"

#include <stdlib.h>
#include <ntqaction.h>
#include <ntqwhatsthis.h>
#include <ntqpopupmenu.h>
#include <ntqmenubar.h>
#include <ntqlineedit.h>
#include <ntqtooltip.h>
#include <ntqapplication.h>
#include <ntqsignalmapper.h>
#include <ntqstylefactory.h>
#include <ntqworkspace.h>
#include <ntqmessagebox.h>
#include <ntqstatusbar.h>
#include <ntqlistbox.h>
#include <ntqclipboard.h>
#include <ntqcombobox.h>
#include <ntqspinbox.h>
#include <ntqinputdialog.h>
#include <ntqdatetimeedit.h>
#include <ntqtextedit.h>

#include "defs.h"
#include "project.h"
#include "widgetdatabase.h"
#include "widgetfactory.h"
#include "preferences.h"
#include "formwindow.h"
#include "newformimpl.h"
#include "resource.h"
#include "projectsettingsimpl.h"
#include "workspace.h"
#include "createtemplate.h"
#include "hierarchyview.h"
#include "editfunctionsimpl.h"
#include "finddialog.h"
#include "replacedialog.h"
#include "gotolinedialog.h"
#include "formsettingsimpl.h"
#include "pixmapcollectioneditor.h"
#include "styledbutton.h"
#include "customwidgeteditorimpl.h"
#ifndef TQT_NO_SQL
#include "dbconnectionsimpl.h"
#include "dbconnectionimpl.h"
#endif
#include "widgetaction.h"
#include <ntqtoolbox.h>
#include "startdialogimpl.h"
#include "designerappiface.h"
#include "connectiondialog.h"
#include "configtoolboxdialog.h"

static const char * whatsthis_image[] = {
    "16 16 3 1",
    "	c None",
    "o	c #000000",
    "a	c #000080",
    "o        aaaaa  ",
    "oo      aaa aaa ",
    "ooo    aaa   aaa",
    "oooo   aa     aa",
    "ooooo  aa     aa",
    "oooooo  a    aaa",
    "ooooooo     aaa ",
    "oooooooo   aaa  ",
    "ooooooooo aaa   ",
    "ooooo     aaa   ",
    "oo ooo          ",
    "o  ooo    aaa   ",
    "    ooo   aaa   ",
    "    ooo         ",
    "     ooo        ",
    "     ooo        "};

const TQString toolbarHelp = "<p>Toolbars contain a number of buttons to "
"provide quick access to often used functions.%1"
"<br>Click on the toolbar handle to hide the toolbar, "
"or drag and place the toolbar to a different location.</p>";

static TQIconSet createIconSet( const TQString &name )
{
    TQIconSet ic( TQPixmap::fromMimeSource( "" + name ) );
    TQString prefix = "designer_";
    int right = name.length() - prefix.length();
    ic.setPixmap( TQPixmap::fromMimeSource( prefix + "d_" + name.right( right ) ),
		  TQIconSet::Small, TQIconSet::Disabled );
    return ic;
}

void MainWindow::setupEditActions()
{
    actionEditUndo = new TQAction( tr("Undo"), createIconSet( "designer_undo.png" ),tr("&Undo: Not Available"), CTRL + Key_Z, this, 0 );
    actionEditUndo->setStatusTip( tr( "Undoes the last action" ) );
    actionEditUndo->setWhatsThis( whatsThisFrom( "Edit|Undo" ) );
    connect( actionEditUndo, TQ_SIGNAL( activated() ), this, TQ_SLOT( editUndo() ) );
    actionEditUndo->setEnabled( false );

    actionEditRedo = new TQAction( tr( "Redo" ), createIconSet("designer_redo.png"), tr( "&Redo: Not Available" ), CTRL + Key_Y, this, 0 );
    actionEditRedo->setStatusTip( tr( "Redoes the last undone operation") );
    actionEditRedo->setWhatsThis( whatsThisFrom( "Edit|Redo" ) );
    connect( actionEditRedo, TQ_SIGNAL( activated() ), this, TQ_SLOT( editRedo() ) );
    actionEditRedo->setEnabled( false );

    actionEditCut = new TQAction( tr( "Cut" ), createIconSet("designer_editcut.png"), tr( "Cu&t" ), CTRL + Key_X, this, 0 );
    actionEditCut->setStatusTip( tr( "Cuts the selected widgets and puts them on the clipboard" ) );
    actionEditCut->setWhatsThis(  whatsThisFrom( "Edit|Cut" ) );
    connect( actionEditCut, TQ_SIGNAL( activated() ), this, TQ_SLOT( editCut() ) );
    actionEditCut->setEnabled( false );

    actionEditCopy = new TQAction( tr( "Copy" ), createIconSet("designer_editcopy.png"), tr( "&Copy" ), CTRL + Key_C, this, 0 );
    actionEditCopy->setStatusTip( tr( "Copies the selected widgets to the clipboard" ) );
    actionEditCopy->setWhatsThis(  whatsThisFrom( "Edit|Copy" ) );
    connect( actionEditCopy, TQ_SIGNAL( activated() ), this, TQ_SLOT( editCopy() ) );
    actionEditCopy->setEnabled( false );

    actionEditPaste = new TQAction( tr( "Paste" ), createIconSet("designer_editpaste.png"), tr( "&Paste" ), CTRL + Key_V, this, 0 );
    actionEditPaste->setStatusTip( tr( "Pastes the clipboard's contents" ) );
    actionEditPaste->setWhatsThis( whatsThisFrom( "Edit|Paste" ) );
    connect( actionEditPaste, TQ_SIGNAL( activated() ), this, TQ_SLOT( editPaste() ) );
    actionEditPaste->setEnabled( false );

    actionEditDelete = new TQAction( tr( "Delete" ), TQPixmap(), tr( "&Delete" ), Key_Delete, this, 0 );
    actionEditDelete->setStatusTip( tr( "Deletes the selected widgets" ) );
    actionEditDelete->setWhatsThis( whatsThisFrom( "Edit|Delete" ) );
    connect( actionEditDelete, TQ_SIGNAL( activated() ), this, TQ_SLOT( editDelete() ) );
    actionEditDelete->setEnabled( false );
#ifdef TQ_WS_MAC
    TQAction *macDelete = new TQAction( tr( "Delete" ), TQPixmap(), tr( "&Delete" ), Key_Backspace, this, 0 );
    connect( macDelete, TQ_SIGNAL( activated() ), this, TQ_SLOT( editDelete() ) );
#endif

    actionEditSelectAll = new TQAction( tr( "Select All" ), TQPixmap(), tr( "Select &All" ), CTRL + Key_A, this, 0 );
    actionEditSelectAll->setStatusTip( tr( "Selects all widgets" ) );
    actionEditSelectAll->setWhatsThis( whatsThisFrom( "Edit|Select All" ) );
    connect( actionEditSelectAll, TQ_SIGNAL( activated() ), this, TQ_SLOT( editSelectAll() ) );
    actionEditSelectAll->setEnabled( true );

    actionEditRaise = new TQAction( tr( "Bring to Front" ), createIconSet("designer_editraise.png"), tr( "Bring to &Front" ), 0, this, 0 );
    actionEditRaise->setStatusTip( tr( "Raises the selected widgets" ) );
    actionEditRaise->setWhatsThis( tr( "Raises the selected widgets" ) );
    connect( actionEditRaise, TQ_SIGNAL( activated() ), this, TQ_SLOT( editRaise() ) );
    actionEditRaise->setEnabled( false );

    actionEditLower = new TQAction( tr( "Send to Back" ), createIconSet("designer_editlower.png"), tr( "Send to &Back" ), 0, this, 0 );
    actionEditLower->setStatusTip( tr( "Lowers the selected widgets" ) );
    actionEditLower->setWhatsThis( tr( "Lowers the selected widgets" ) );
    connect( actionEditLower, TQ_SIGNAL( activated() ), this, TQ_SLOT( editLower() ) );
    actionEditLower->setEnabled( false );

    actionEditAccels = new TQAction( tr( "Check Accelerators" ), TQPixmap(),
				    tr( "Chec&k Accelerators" ), ALT + Key_R, this, 0 );
    actionEditAccels->setStatusTip( tr("Checks if the accelerators used in the form are unique") );
    actionEditAccels->setWhatsThis( whatsThisFrom( "Edit|Check Accelerator" ) );
    connect( actionEditAccels, TQ_SIGNAL( activated() ), this, TQ_SLOT( editAccels() ) );
    connect( this, TQ_SIGNAL( hasActiveForm(bool) ), actionEditAccels, TQ_SLOT( setEnabled(bool) ) );

    actionEditFunctions = new TQAction( tr( "Slots" ), createIconSet("designer_editslots.png"),
				   tr( "S&lots..." ), 0, this, 0 );
    actionEditFunctions->setStatusTip( tr("Opens a dialog for editing slots") );
    actionEditFunctions->setWhatsThis( whatsThisFrom( "Edit|Slots" ) );
    connect( actionEditFunctions, TQ_SIGNAL( activated() ), this, TQ_SLOT( editFunctions() ) );
    connect( this, TQ_SIGNAL( hasActiveForm(bool) ), actionEditFunctions, TQ_SLOT( setEnabled(bool) ) );

    actionEditConnections = new TQAction( tr( "Connections" ), createIconSet("designer_connecttool.png"),
					 tr( "Co&nnections..." ), 0, this, 0 );
    actionEditConnections->setStatusTip( tr("Opens a dialog for editing connections") );
    actionEditConnections->setWhatsThis( whatsThisFrom( "Edit|Connections" ) );
    connect( actionEditConnections, TQ_SIGNAL( activated() ), this, TQ_SLOT( editConnections() ) );
    connect( this, TQ_SIGNAL( hasActiveForm(bool) ), actionEditConnections, TQ_SLOT( setEnabled(bool) ) );

    actionEditSource = new TQAction( tr( "Source" ), TQIconSet(),
					 tr( "&Source..." ), CTRL + Key_E, this, 0 );
    actionEditSource->setStatusTip( tr("Opens an editor to edit the form's source code") );
    actionEditSource->setWhatsThis( whatsThisFrom( "Edit|Source" ) );
    connect( actionEditSource, TQ_SIGNAL( activated() ), this, TQ_SLOT( editSource() ) );
    connect( this, TQ_SIGNAL( hasActiveForm(bool) ), actionEditSource, TQ_SLOT( setEnabled(bool) ) );

    actionEditFormSettings = new TQAction( tr( "Form Settings" ), TQPixmap(),
					  tr( "&Form Settings..." ), 0, this, 0 );
    actionEditFormSettings->setStatusTip( tr("Opens a dialog to change the form's settings") );
    actionEditFormSettings->setWhatsThis( whatsThisFrom( "Edit|Form Settings" ) );
    connect( actionEditFormSettings, TQ_SIGNAL( activated() ), this, TQ_SLOT( editFormSettings() ) );
    connect( this, TQ_SIGNAL( hasActiveForm(bool) ), actionEditFormSettings, TQ_SLOT( setEnabled(bool) ) );

    actionEditPreferences = new TQAction( tr( "Preferences" ), TQPixmap(),
					 tr( "Preferences..." ), 0, this, 0 );
    actionEditPreferences->setStatusTip( tr("Opens a dialog to change preferences") );
    actionEditPreferences->setWhatsThis( whatsThisFrom( "Edit|Preferences" ) );
    connect( actionEditPreferences, TQ_SIGNAL( activated() ), this, TQ_SLOT( editPreferences() ) );

    TQToolBar *tb = new TQToolBar( this, "Edit" );
    tb->setCloseMode( TQDockWindow::Undocked );
    TQWhatsThis::add( tb, tr( "<b>The Edit toolbar</b>%1").arg(tr(toolbarHelp).arg("")) );
    addToolBar( tb, tr( "Edit" ) );
    actionEditUndo->addTo( tb );
    actionEditRedo->addTo( tb );
    tb->addSeparator();
    actionEditCut->addTo( tb );
    actionEditCopy->addTo( tb );
    actionEditPaste->addTo( tb );
#if 0
    tb->addSeparator();
    actionEditLower->addTo( tb );
    actionEditRaise->addTo( tb );
#endif

    TQPopupMenu *menu = new TQPopupMenu( this, "Edit" );
    connect( menu, TQ_SIGNAL( aboutToShow() ), this, TQ_SLOT( updateEditorUndoRedo() ) );
    menubar->insertItem( tr( "&Edit" ), menu );
    actionEditUndo->addTo( menu );
    actionEditRedo->addTo( menu );
    menu->insertSeparator();
    actionEditCut->addTo( menu );
    actionEditCopy->addTo( menu );
    actionEditPaste->addTo( menu );
    actionEditDelete->addTo( menu );
    actionEditSelectAll->addTo( menu );
    actionEditAccels->addTo( menu );
#if 0
    menu->insertSeparator();
    actionEditLower->addTo( menu );
    actionEditRaise->addTo( menu );
#endif
    menu->insertSeparator();
    if ( !singleProjectMode() ) {
	actionEditFunctions->addTo( menu );
	actionEditConnections->addTo( menu );
    }
    actionEditFormSettings->addTo( menu );
    menu->insertSeparator();
    actionEditPreferences->addTo( menu );
}

void MainWindow::setupSearchActions()
{
    actionSearchFind = new TQAction( tr( "Find" ), createIconSet( "designer_searchfind.png" ),
				    tr( "&Find..." ), CTRL + Key_F, this, 0 );
    connect( actionSearchFind, TQ_SIGNAL( activated() ), this, TQ_SLOT( searchFind() ) );
    actionSearchFind->setEnabled( false );
    actionSearchFind->setWhatsThis( whatsThisFrom( "Search|Find" ) );

    actionSearchIncremetal = new TQAction( tr( "Find Incremental" ), TQIconSet(),
					  tr( "Find &Incremental" ), ALT + Key_I, this, 0 );
    connect( actionSearchIncremetal, TQ_SIGNAL( activated() ), this, TQ_SLOT( searchIncremetalFindMenu() ) );
    actionSearchIncremetal->setEnabled( false );
    actionSearchIncremetal->setWhatsThis( whatsThisFrom( "Search|Find Incremental" ) );

    actionSearchReplace = new TQAction( tr( "Replace" ), TQIconSet(),
				    tr( "&Replace..." ), CTRL + Key_R, this, 0 );
    connect( actionSearchReplace, TQ_SIGNAL( activated() ), this, TQ_SLOT( searchReplace() ) );
    actionSearchReplace->setEnabled( false );
    actionSearchReplace->setWhatsThis( whatsThisFrom( "Search|Replace" ) );

    actionSearchGotoLine = new TQAction( tr( "Goto Line" ), TQIconSet(),
				    tr( "&Goto Line..." ), ALT + Key_G, this, 0 );
    connect( actionSearchGotoLine, TQ_SIGNAL( activated() ), this, TQ_SLOT( searchGotoLine() ) );
    actionSearchGotoLine->setEnabled( false );
    actionSearchGotoLine->setWhatsThis( whatsThisFrom( "Search|Goto line" ) );

    TQToolBar *tb = new TQToolBar( this, "Search" );
    tb->setCloseMode( TQDockWindow::Undocked );
    addToolBar( tb, tr( "Search" ) );

    actionSearchFind->addTo( tb );
    incrementalSearch = new TQLineEdit( tb );
    TQToolTip::add( incrementalSearch, tr( "Incremetal Search (Alt+I)" ) );
    connect( incrementalSearch, TQ_SIGNAL( textChanged( const TQString & ) ),
	     this, TQ_SLOT( searchIncremetalFind() ) );
    connect( incrementalSearch, TQ_SIGNAL( returnPressed() ),
	     this, TQ_SLOT( searchIncremetalFindNext() ) );
    incrementalSearch->setEnabled( false );

    TQPopupMenu *menu = new TQPopupMenu( this, "Search" );
    menubar->insertItem( tr( "&Search" ), menu );
    actionSearchFind->addTo( menu );
    actionSearchIncremetal->addTo( menu );
    actionSearchReplace->addTo( menu );
    menu->insertSeparator();
    actionSearchGotoLine->addTo( menu );
}

void MainWindow::setupLayoutActions()
{
    if ( !actionGroupTools ) {
	actionGroupTools = new TQActionGroup( this );
	actionGroupTools->setExclusive( true );
	connect( actionGroupTools, TQ_SIGNAL( selected(TQAction*) ), this, TQ_SLOT( toolSelected(TQAction*) ) );
    }

    actionEditAdjustSize = new TQAction( tr( "Adjust Size" ), createIconSet("designer_adjustsize.png"),
					tr( "Adjust &Size" ), CTRL + Key_J, this, 0 );
    actionEditAdjustSize->setStatusTip(tr("Adjusts the size of the selected widget") );
    actionEditAdjustSize->setWhatsThis( whatsThisFrom( "Layout|Adjust Size" ) );
    connect( actionEditAdjustSize, TQ_SIGNAL( activated() ), this, TQ_SLOT( editAdjustSize() ) );
    actionEditAdjustSize->setEnabled( false );

    actionEditHLayout = new TQAction( tr( "Lay Out Horizontally" ), createIconSet("designer_edithlayout.png"),
				     tr( "Lay Out &Horizontally" ), CTRL + Key_H, this, 0 );
    actionEditHLayout->setStatusTip(tr("Lays out the selected widgets horizontally") );
    actionEditHLayout->setWhatsThis( whatsThisFrom( "Layout|Lay Out Horizontally" ) );
    connect( actionEditHLayout, TQ_SIGNAL( activated() ), this, TQ_SLOT( editLayoutHorizontal() ) );
    actionEditHLayout->setEnabled( false );

    actionEditVLayout = new TQAction( tr( "Lay Out Vertically" ), createIconSet("designer_editvlayout.png"),
				     tr( "Lay Out &Vertically" ), CTRL + Key_L, this, 0 );
    actionEditVLayout->setStatusTip(tr("Lays out the selected widgets vertically") );
    actionEditVLayout->setWhatsThis(  whatsThisFrom( "Layout|Lay Out Vertically" ) );
    connect( actionEditVLayout, TQ_SIGNAL( activated() ), this, TQ_SLOT( editLayoutVertical() ) );
    actionEditVLayout->setEnabled( false );

    actionEditGridLayout = new TQAction( tr( "Lay Out in a Grid" ), createIconSet("designer_editgrid.png"),
					tr( "Lay Out in a &Grid" ), CTRL + Key_G, this, 0 );
    actionEditGridLayout->setStatusTip(tr("Lays out the selected widgets in a grid") );
    actionEditGridLayout->setWhatsThis( whatsThisFrom( "Layout|Lay Out in a Grid" ) );
    connect( actionEditGridLayout, TQ_SIGNAL( activated() ), this, TQ_SLOT( editLayoutGrid() ) );
    actionEditGridLayout->setEnabled( false );

    actionEditSplitHorizontal = new TQAction( tr( "Lay Out Horizontally (in Splitter)" ), createIconSet("designer_editvlayoutsplit.png"),
					     tr( "Lay Out Horizontally (in S&plitter)" ), 0, this, 0 );
    actionEditSplitHorizontal->setStatusTip(tr("Lays out the selected widgets horizontally in a splitter") );
    actionEditSplitHorizontal->setWhatsThis( whatsThisFrom( "Layout|Lay Out Horizontally (in Splitter)" ) );
    connect( actionEditSplitHorizontal, TQ_SIGNAL( activated() ), this, TQ_SLOT( editLayoutHorizontalSplit() ) );
    actionEditSplitHorizontal->setEnabled( false );

    actionEditSplitVertical = new TQAction( tr( "Lay Out Vertically (in Splitter)" ), createIconSet("designer_edithlayoutsplit.png"),
					     tr( "Lay Out Vertically (in Sp&litter)" ), 0, this, 0 );
    actionEditSplitVertical->setStatusTip(tr("Lays out the selected widgets vertically in a splitter") );
    actionEditSplitVertical->setWhatsThis( whatsThisFrom( "Layout|Lay Out Vertically (in Splitter)" ) );
    connect( actionEditSplitVertical, TQ_SIGNAL( activated() ), this, TQ_SLOT( editLayoutVerticalSplit() ) );
    actionEditSplitVertical->setEnabled( false );

    actionEditBreakLayout = new TQAction( tr( "Break Layout" ), createIconSet("designer_editbreaklayout.png"),
					 tr( "&Break Layout" ), CTRL + Key_B, this, 0 );
    actionEditBreakLayout->setStatusTip(tr("Breaks the selected layout") );
    actionEditBreakLayout->setWhatsThis( whatsThisFrom( "Layout|Break Layout" ) );
    connect( actionEditBreakLayout, TQ_SIGNAL( activated() ), this, TQ_SLOT( editBreakLayout() ) );
    actionEditBreakLayout->setEnabled( false );

    int id = WidgetDatabase::idFromClassName( "Spacer" );
    actionInsertSpacer = new WidgetAction( "Layout", actionGroupTools, TQString::number( id ).latin1() );
    actionInsertSpacer->setToggleAction( true );
    actionInsertSpacer->setText( WidgetDatabase::className( id ) );
    actionInsertSpacer->setMenuText( tr( "Add ") + WidgetDatabase::className( id ) );
    actionInsertSpacer->setIconSet( WidgetDatabase::iconSet( id ) );
    actionInsertSpacer->setToolTip( WidgetDatabase::toolTip( id ) );
    actionInsertSpacer->setStatusTip( tr( "Insert a %1").arg(WidgetDatabase::toolTip( id )) );
    actionInsertSpacer->setWhatsThis( TQString("<b>A %1</b><p>%2</p>"
			     "<p>Click to insert a single %3,"
			     "or double click to keep the tool selected.")
	.arg(WidgetDatabase::toolTip( id ))
	.arg(WidgetDatabase::whatsThis( id ))
	.arg(WidgetDatabase::toolTip( id ) ));

    actionInsertSpacer->addTo( commonWidgetsToolBar );
    commonWidgetsPage.append( actionInsertSpacer );
    TQWidget *w;
    commonWidgetsToolBar->setStretchableWidget( ( w = new TQWidget( commonWidgetsToolBar ) ) );
    w->setBackgroundMode( commonWidgetsToolBar->backgroundMode() );
    TQWhatsThis::add( layoutToolBar, tr( "<b>The Layout toolbar</b>%1" ).arg(tr(toolbarHelp).arg("")) );
    actionEditAdjustSize->addTo( layoutToolBar );
    layoutToolBar->addSeparator();
    actionEditHLayout->addTo( layoutToolBar );
    actionEditVLayout->addTo( layoutToolBar );
    actionEditGridLayout->addTo( layoutToolBar );
    actionEditSplitHorizontal->addTo( layoutToolBar );
    actionEditSplitVertical->addTo( layoutToolBar );
    actionEditBreakLayout->addTo( layoutToolBar );
    layoutToolBar->addSeparator();
    actionInsertSpacer->addTo( layoutToolBar );

    TQPopupMenu *menu = new TQPopupMenu( this, "Layout" );
    layoutMenu = menu;
    menubar->insertItem( tr( "&Layout" ), menu, toolsMenuId + 1 );
    actionEditAdjustSize->addTo( menu );
    menu->insertSeparator();
    actionEditHLayout->addTo( menu );
    actionEditVLayout->addTo( menu );
    actionEditGridLayout->addTo( menu );
    actionEditSplitHorizontal->addTo( menu );
    actionEditSplitVertical->addTo( menu );
    actionEditBreakLayout->addTo( menu );
    menu->insertSeparator();
    actionInsertSpacer->addTo( menu );
}

void MainWindow::setupToolActions()
{
    if ( !actionGroupTools ) {
	actionGroupTools = new TQActionGroup( this );
	actionGroupTools->setExclusive( true );
	connect( actionGroupTools, TQ_SIGNAL( selected(TQAction*) ),
		 this, TQ_SLOT( toolSelected(TQAction*) ) );
    }

    actionPointerTool = new TQAction( tr("Pointer"), createIconSet("designer_pointer.png"),
				     tr("&Pointer"),  Key_F2,
				     actionGroupTools,
				     TQString::number(POINTER_TOOL).latin1(), true );
    actionPointerTool->setStatusTip( tr("Selects the pointer tool") );
    actionPointerTool->setWhatsThis( whatsThisFrom( "Tools|Pointer" ) );

    actionConnectTool = new TQAction( tr("Connect Signal/Slots"),
				     createIconSet("designer_connecttool.png"),
				     tr("&Connect Signal/Slots"),
				     singleProjectMode() ? 0 : Key_F3,
				     actionGroupTools,
				     TQString::number(CONNECT_TOOL).latin1(), true );
    actionConnectTool->setStatusTip( tr("Selects the connection tool") );
    actionConnectTool->setWhatsThis( whatsThisFrom( "Tools|Connect Signals and Slots" ) );

    actionOrderTool = new TQAction( tr("Tab Order"), createIconSet("designer_ordertool.png"),
				   tr("Tab &Order"),  Key_F4,
				   actionGroupTools,
				   TQString::number(ORDER_TOOL).latin1(), true );
    actionOrderTool->setStatusTip( tr("Selects the tab order tool") );
    actionOrderTool->setWhatsThis( whatsThisFrom( "Tools|Tab Order" ) );

    actionBuddyTool = new TQAction( tr( "Set Buddy" ), createIconSet( "designer_setbuddy.png" ),
				   tr( "Set &Buddy" ), Key_F12,
				   actionGroupTools, TQString::number( BUDDY_TOOL ).latin1(),
				   true );
    actionBuddyTool->setStatusTip( tr( "Sets a buddy to a label" ) );
    actionBuddyTool->setWhatsThis( whatsThisFrom( "Tools|Set Buddy" ) );

    TQToolBar *tb = new TQToolBar( this, "Tools" );
    tb->setCloseMode( TQDockWindow::Undocked );
    toolsToolBar = tb;
    TQWhatsThis::add( tb, tr( "<b>The Tools toolbar</b>%1" ).arg(tr(toolbarHelp).arg("")) );

    addToolBar( tb, tr( "Tools" ), TQMainWindow::DockTop, false );
    actionPointerTool->addTo( tb );
    if ( !singleProjectMode() )
	actionConnectTool->addTo( tb );
    actionOrderTool->addTo( tb );
    actionBuddyTool->addTo( tb );

    TQPopupMenu *mmenu = new TQPopupMenu( this, "Tools" );
    toolsMenu = mmenu;
    toolsMenuId = 100;
    menubar->insertItem( tr( "&Tools" ), mmenu, toolsMenuId );
    toolsMenuIndex = menubar->indexOf( toolsMenuId );
    actionPointerTool->addTo( mmenu );
    if ( !singleProjectMode() )
	actionConnectTool->addTo( mmenu );
    actionOrderTool->addTo( mmenu );
    actionBuddyTool->addTo( mmenu );
    mmenu->insertSeparator();

    customWidgetToolBar = 0;
    customWidgetMenu = 0;

    actionToolsCustomWidget = new TQAction( tr("Custom Widgets"),
					   createIconSet( "designer_customwidget.png" ),
					   tr("Edit &Custom Widgets..."), 0, this, 0 );
    actionToolsCustomWidget->setStatusTip( tr("Opens a dialog to add and change "
					      "custom widgets") );
    actionToolsCustomWidget->setWhatsThis( whatsThisFrom( "Tools|Custom|Edit Custom"
							  "Widgets" ) );
    connect( actionToolsCustomWidget, TQ_SIGNAL( activated() ),
	     this, TQ_SLOT( toolsCustomWidget() ) );

    for ( int j = 0; j < WidgetDatabase::numWidgetGroups(); ++j ) {
	TQString grp = WidgetDatabase::widgetGroup( j );
	if ( !WidgetDatabase::isGroupVisible( grp ) ||
	     WidgetDatabase::isGroupEmpty( grp ) )
	    continue;
	TQToolBar *tb = new TQToolBar( this, grp.latin1() );
	tb->setCloseMode( TQDockWindow::Undocked );
	widgetToolBars.append( tb );
	bool plural = grp[(int)grp.length()-1] == 's';
	if ( plural ) {
	    TQWhatsThis::add( tb, tr( "<b>The %1</b>%2" ).arg(grp).arg(tr(toolbarHelp).
				arg( tr(" Click on a button to insert a single widget, "
				"or double click to insert multiple %1.") ).arg(grp)) );
	} else {
	    TQWhatsThis::add( tb, tr( "<b>The %1 Widgets</b>%2" ).arg(grp).arg(tr(toolbarHelp).
				arg( tr(" Click on a button to insert a single %1 widget, "
				"or double click to insert multiple widgets.") ).arg(grp)) );
	}
	addToolBar( tb, grp );
	tb->hide();
	TQPopupMenu *menu = new TQPopupMenu( this, grp.latin1() );
	mmenu->insertItem( grp, menu );

	TQToolBar *tb2 = new TQToolBar( grp, 0, toolBox, false, grp.latin1() );
	tb2->setFrameStyle( TQFrame::NoFrame );
	tb2->setOrientation( TQt::Vertical );
	tb2->setBackgroundMode( PaletteBase );
	toolBox->addItem( tb2, grp );

	if ( grp == "Custom" ) {
	    if ( !customWidgetMenu )
		actionToolsCustomWidget->addTo( menu );
	    else
		menu->insertSeparator();
	    customWidgetMenu = menu;
	    customWidgetToolBar = tb;
	}

	for ( int i = 0; i < WidgetDatabase::count(); ++i ) {
	    if ( WidgetDatabase::group( i ) != grp )
		continue; // only widgets, i.e. not forms and temp stuff
	    WidgetAction* a =
		new WidgetAction( grp, actionGroupTools, TQString::number( i ).latin1() );
	    a->setToggleAction( true );
	    TQString atext = WidgetDatabase::className( i );
	    if (( atext[0] == 'T' ) && ( atext[1] == 'Q' ))
		atext = atext.mid(2);
	    while ( atext.length() && atext[0] >= 'a' && atext[0] <= 'z' )
		atext = atext.mid(1);
	    if ( atext.isEmpty() )
		atext = WidgetDatabase::className( i );
	    a->setText( atext );
	    TQString ttip = WidgetDatabase::toolTip( i );
	    a->setIconSet( WidgetDatabase::iconSet( i ) );
	    a->setToolTip( ttip );
	    if ( !WidgetDatabase::isWhatsThisLoaded() )
		WidgetDatabase::loadWhatsThis( documentationPath() );
	    a->setStatusTip( tr( "Insert a %1").arg(WidgetDatabase::className( i )) );

	    TQString whats = TQString("<b>A %1</b>").arg( WidgetDatabase::className( i ) );
	    if ( !WidgetDatabase::whatsThis( i ).isEmpty() )
	    whats += TQString("<p>%1</p>").arg(WidgetDatabase::whatsThis( i ));
	    a->setWhatsThis( whats+ tr("<p>Double click on this tool to keep it selected.</p>") );

	    if ( grp != "KDE" )
		a->addTo( tb );
	    a->addTo( menu );
	    a->addTo( tb2 );
	    if ( WidgetDatabase::isCommon( i ) ) {
		a->addTo( commonWidgetsToolBar );
		commonWidgetsPage.append( a );
	    }
	}
	TQWidget *w;
	tb2->setStretchableWidget( ( w = new TQWidget( tb2 ) ) );
	w->setBackgroundMode( tb2->backgroundMode() );
    }

    if ( !customWidgetToolBar ) {
	TQToolBar *tb = new TQToolBar( this, "Custom Widgets" );
	tb->setCloseMode( TQDockWindow::Undocked );
	TQWhatsThis::add( tb, tr( "<b>The Custom Widgets toolbar</b>%1"
				 "<p>Click <b>Edit Custom Widgets...</b>"
				 "in the <b>Tools|Custom</b> menu to "
				 "add and change custom widgets</p>" ).
			 arg(tr(toolbarHelp).
			     arg( tr(" Click on the buttons to insert a single widget, "
				     "or double click to insert multiple widgets.") )) );
	addToolBar( tb, "Custom" );
	tb->hide();
	widgetToolBars.append( tb );
	customWidgetToolBar = tb;
	TQPopupMenu *menu = new TQPopupMenu( this, "Custom Widgets" );
	mmenu->insertItem( "Custom", menu );
	customWidgetMenu = menu;
	customWidgetToolBar->hide();
	actionToolsCustomWidget->addTo( customWidgetMenu );
	customWidgetMenu->insertSeparator();
	TQToolBar *tb2 = new TQToolBar( "Custom Widgets", 0,
				      toolBox, false, "Custom Widgets" );
	tb2->setBackgroundMode(PaletteBase);
	tb2->setOrientation( TQt::Vertical );
	tb2->setFrameStyle( TQFrame::NoFrame );
	toolBox->addItem( tb2, "Custom Widgets" );
	customWidgetToolBar2 = tb2;
    }

    TQAction *a = new TQAction( tr( "Configure Toolbox" ), tr( "Configure Toolbox..." ), 0, this );
    a->setStatusTip( tr( "Opens a dialog to configure the common "
					       "widgets page of the toolbox") );
    connect( a, TQ_SIGNAL( activated() ), this, TQ_SLOT( toolsConfigure() ) );
    mmenu->insertSeparator();
    a->addTo( mmenu );
    resetTool();
}

void MainWindow::setupFileActions()
{
    TQToolBar* tb  = new TQToolBar( this, "File" );
    tb->setCloseMode( TQDockWindow::Undocked );
    projectToolBar = tb;

    TQWhatsThis::add( tb, tr( "<b>The File toolbar</b>%1" ).arg(tr(toolbarHelp).arg("")) );
    addToolBar( tb, tr( "File" ) );
    fileMenu = new TQPopupMenu( this, "File" );
    menubar->insertItem( tr( "&File" ), fileMenu );

    TQAction *a = 0;

    if ( !singleProject ) {
	a = new TQAction( this, 0 );
	a->setText( tr( "New" ) );
	a->setToolTip( tr( "New Dialog or File" ) );
	a->setMenuText( tr( "&New..." ) );
	a->setIconSet( createIconSet("designer_filenew.png") );
	a->setAccel( CTRL + Key_N );
	a->setStatusTip( tr( "Creates a new project, form or source file." ) );
	a->setWhatsThis( whatsThisFrom( "File|New" ) );
	connect( a, TQ_SIGNAL( activated() ), this, TQ_SLOT( fileNew() ) );
	a->addTo( tb );
	a->addTo( fileMenu );
    } else {
	actionGroupNew = new TQActionGroup( this, 0, false );
	 a = actionGroupNew;
	( (TQActionGroup*)a )->setUsesDropDown( true );
	a->setText( tr( "New" ) );
	a->setMenuText( tr( "&New..." ) );
	a->setIconSet( createIconSet("designer_form.png") );
	a->setStatusTip( tr( "Creates a new dialog or file" ) );
	a->setWhatsThis( whatsThisFrom( "File|New" ) );

	TQAction *newForm = new TQAction( a, 0 );
	newForm->setText( tr( "New Dialog" ) );
	newForm->setMenuText( tr( "&Dialog..." ) );
	newForm->setIconSet( createIconSet("designer_form.png") );
	newForm->setAccel( CTRL + Key_N );
	newForm->setStatusTip( tr( "Creates a new dialog." ) );
	connect( newForm, TQ_SIGNAL( activated() ), this, TQ_SLOT( fileNewDialog() ) );

	TQAction *newFile = new TQAction( a, 0 );
	newFile->setText( tr( "New File" ) );
	newFile->setMenuText( tr( "&File..." ) );
	newFile->setIconSet( createIconSet("designer_filenew.png") );
	newFile->setAccel( ALT + Key_N );
	newFile->setStatusTip( tr( "Creates a new file." ) );
	connect( newFile, TQ_SIGNAL( activated() ), this, TQ_SLOT( fileNewFile() ) );
	actionNewFile = newFile;

	a->addTo( tb );
	a->addTo( fileMenu );

	fileMenu->insertSeparator();
    }

    a = new TQAction( this, 0 );
    a->setText( tr( "Open" ) );
    a->setMenuText( tr( "&Open..." ) );
    a->setIconSet( createIconSet("designer_fileopen.png") );
    a->setAccel( CTRL + Key_O );
    a->setStatusTip( tr( "Opens an existing project, form or source file ") );
    a->setWhatsThis( whatsThisFrom( "File|Open" ) );
    connect( a, TQ_SIGNAL( activated() ), this, TQ_SLOT( fileOpen() ) );
    if ( !singleProject ) {
	a->addTo( tb );
	a->addTo( fileMenu );
	fileMenu->insertSeparator();
    }


    a = new TQAction( this, 0 );
    a->setText( tr( "Close" ) );
    a->setMenuText( tr( "&Close" ) );
    a->setStatusTip( tr( "Closes the current project or document" ) );
    a->setWhatsThis(whatsThisFrom( "File|Close" ) );
    connect( a, TQ_SIGNAL( activated() ), this, TQ_SLOT( fileClose() ) );
    connect( this, TQ_SIGNAL( hasActiveWindowOrProject(bool) ), a, TQ_SLOT( setEnabled(bool) ) );
    if ( !singleProject ) {
	a->addTo( fileMenu );
	fileMenu->insertSeparator();
    }

    a = new TQAction( this, 0 );
    actionFileSave = a;
    a->setText( tr( "Save" ) );
    a->setMenuText( tr( "&Save" ) );
    a->setIconSet( createIconSet("designer_filesave.png") );
    a->setAccel( CTRL + Key_S );
    a->setStatusTip( tr( "Saves the current project or document" ) );
    a->setWhatsThis(whatsThisFrom( "File|Save" ) );
    connect( a, TQ_SIGNAL( activated() ), this, TQ_SLOT( fileSave() ) );
    connect( this, TQ_SIGNAL( hasActiveWindowOrProject(bool) ), a, TQ_SLOT( setEnabled(bool) ) );
    a->addTo( tb );
    a->addTo( fileMenu );

    a = new TQAction( this, 0 );
    a->setText( tr( "Save As" ) );
    a->setMenuText( tr( "Save &As..." ) );
    a->setStatusTip( tr( "Saves the current form with a new filename" ) );
    a->setWhatsThis( whatsThisFrom( "File|Save As" ) );
    connect( a, TQ_SIGNAL( activated() ), this, TQ_SLOT( fileSaveAs() ) );
    connect( this, TQ_SIGNAL( hasActiveWindow(bool) ), a, TQ_SLOT( setEnabled(bool) ) );
    if ( !singleProject )
	a->addTo( fileMenu );

    a = new TQAction( this, 0 );
    a->setText( tr( "Save All" ) );
    a->setMenuText( tr( "Sa&ve All" ) );
    a->setStatusTip( tr( "Saves all open documents" ) );
    a->setWhatsThis( whatsThisFrom( "File|Save All" ) );
    connect( a, TQ_SIGNAL( activated() ), this, TQ_SLOT( fileSaveAll() ) );
    connect( this, TQ_SIGNAL( hasActiveWindowOrProject(bool) ), a, TQ_SLOT( setEnabled(bool) ) );
    if ( !singleProject ) {
	a->addTo( fileMenu );
	fileMenu->insertSeparator();
    }

    a = new TQAction( this, 0 );
    a->setText( tr( "Create Template" ) );
    a->setMenuText( tr( "Create &Template..." ) );
    a->setStatusTip( tr( "Creates a new template" ) );
    a->setWhatsThis( whatsThisFrom( "File|Create Template" ) );
    connect( a, TQ_SIGNAL( activated() ), this, TQ_SLOT( fileCreateTemplate() ) );
    if ( !singleProject )
	a->addTo( fileMenu );

    if ( !singleProject )
	fileMenu->insertSeparator();

    recentlyFilesMenu = new TQPopupMenu( this );
    recentlyProjectsMenu = new TQPopupMenu( this );

    if ( !singleProject ) {
	fileMenu->insertItem( tr( "Recently opened files " ), recentlyFilesMenu );
	fileMenu->insertItem( tr( "Recently opened projects" ), recentlyProjectsMenu );
    }

    connect( recentlyFilesMenu, TQ_SIGNAL( aboutToShow() ),
	     this, TQ_SLOT( setupRecentlyFilesMenu() ) );
    connect( recentlyProjectsMenu, TQ_SIGNAL( aboutToShow() ),
	     this, TQ_SLOT( setupRecentlyProjectsMenu() ) );
    connect( recentlyFilesMenu, TQ_SIGNAL( activated( int ) ),
	     this, TQ_SLOT( recentlyFilesMenuActivated( int ) ) );
    connect( recentlyProjectsMenu, TQ_SIGNAL( activated( int ) ),
	     this, TQ_SLOT( recentlyProjectsMenuActivated( int ) ) );

    if ( !singleProject )
	fileMenu->insertSeparator();

    a = new TQAction( this, 0 );
    actionFileExit = a;
    a->setAccel(CTRL + Key_Q);
    if  ( !singleProjectMode() ) {
	a->setText( tr( "Exit" ) );
	a->setMenuText( tr( "E&xit" ) );
	a->setStatusTip( tr( "Quits the application and prompts to save any changed forms, source files or project settings" ) );
	a->setWhatsThis( whatsThisFrom( "File|Exit" ) );
    } else {
	a->setText( tr( "Close" ) );
	a->setMenuText( tr( "&Close" ) );
    }
    connect( a, TQ_SIGNAL( activated() ), this, TQ_SLOT( fileQuit() ) );
    a->addTo( fileMenu );
}

void MainWindow::setupProjectActions()
{
    projectMenu = new TQPopupMenu( this, "Project" );
    menubar->insertItem( tr( "Pr&oject" ), projectMenu );

    TQActionGroup *ag = new TQActionGroup( this, 0 );
    ag->setText( tr( "Active Project" ) );
    ag->setMenuText( tr( "Active Project" ) );
    ag->setExclusive( true );
    ag->setUsesDropDown( true );
    connect( ag, TQ_SIGNAL( selected( TQAction * ) ), this, TQ_SLOT( projectSelected( TQAction * ) ) );
    connect( ag, TQ_SIGNAL( selected( TQAction * ) ), this, TQ_SIGNAL( projectChanged() ) );
    TQAction *a = new TQAction( tr( "<No Project>" ), tr( "<No Project>" ), 0, ag, 0, true );
    eProject = new Project( "", tr( "<No Project>" ), projectSettingsPluginManager, true );
    projects.insert( a, eProject );
    a->setOn( true );
    ag->addTo( projectMenu );
    ag->addTo( projectToolBar );
    actionGroupProjects = ag;

    if ( !singleProject )
	projectMenu->insertSeparator();

    a = new TQAction( tr( "Add File" ), TQPixmap(), tr( "&Add File..." ), 0, this, 0 );
    a->setStatusTip( tr("Adds a file to the current project") );
    a->setWhatsThis( whatsThisFrom( "Project|Add File" ) );
    connect( a, TQ_SIGNAL( activated() ), this, TQ_SLOT( projectInsertFile() ) );
    a->setEnabled( false );
    connect( this, TQ_SIGNAL( hasNonDummyProject(bool) ), a, TQ_SLOT( setEnabled(bool) ) );
    if ( !singleProject )
	a->addTo( projectMenu );

    TQAction* actionEditPixmapCollection = new TQAction( tr( "Image Collection..." ), TQPixmap(),
					  tr( "&Image Collection..." ), 0, this, 0 );
    actionEditPixmapCollection->setStatusTip( tr("Opens a dialog for editing the current project's image collection") );
    actionEditPixmapCollection->setWhatsThis( whatsThisFrom( "Project|Image Collection" ) );
    connect( actionEditPixmapCollection, TQ_SIGNAL( activated() ), this, TQ_SLOT( editPixmapCollection() ) );
    actionEditPixmapCollection->setEnabled( false );
    connect( this, TQ_SIGNAL( hasNonDummyProject(bool) ), actionEditPixmapCollection, TQ_SLOT( setEnabled(bool) ) );
    actionEditPixmapCollection->addTo( projectMenu );

#ifndef TQT_NO_SQL
    TQAction* actionEditDatabaseConnections = new TQAction( tr( "Database Connections..." ), TQPixmap(),
						 tr( "&Database Connections..." ), 0, this, 0 );
    actionEditDatabaseConnections->setStatusTip( tr("Opens a dialog for editing the current project's database connections") );
    actionEditDatabaseConnections->setWhatsThis( whatsThisFrom( "Project|Database Connections" ) );
    connect( actionEditDatabaseConnections, TQ_SIGNAL( activated() ), this, TQ_SLOT( editDatabaseConnections() ) );
    //actionEditDatabaseConnections->setEnabled( false );
    //connect( this, TQ_SIGNAL( hasNonDummyProject(bool) ), actionEditDatabaseConnections, TQ_SLOT( setEnabled(bool) ) );
    if ( !singleProject )
	actionEditDatabaseConnections->addTo( projectMenu );
#endif

    TQAction* actionEditProjectSettings = new TQAction( tr( "Project Settings..." ), TQPixmap(),
					  tr( "&Project Settings..." ), 0, this, 0 );
    actionEditProjectSettings->setStatusTip( tr("Opens a dialog to change the project's settings") );
    actionEditProjectSettings->setWhatsThis( whatsThisFrom( "Project|Project Settings" ) );
    connect( actionEditProjectSettings, TQ_SIGNAL( activated() ), this, TQ_SLOT( editProjectSettings() ) );
    actionEditProjectSettings->setEnabled( false );
    connect( this, TQ_SIGNAL( hasNonDummyProject(bool) ), actionEditProjectSettings, TQ_SLOT( setEnabled(bool) ) );
    actionEditProjectSettings->addTo( projectMenu );

}

void MainWindow::setupPreviewActions()
{
    TQAction* a = 0;
    TQPopupMenu *menu = new TQPopupMenu( this, "Preview" );
    layoutMenu = menu;
    menubar->insertItem( tr( "&Preview" ), menu, toolsMenuId + 2 );

    a = new TQAction( tr( "Preview Form" ), TQPixmap(), tr( "Preview &Form" ), 0, this, 0 );
    a->setAccel( CTRL + Key_T );
    a->setStatusTip( tr("Opens a preview") );
    a->setWhatsThis( whatsThisFrom( "Preview|Preview Form" ) );
    connect( a, TQ_SIGNAL( activated() ), this, TQ_SLOT( previewForm() ) );
    connect( this, TQ_SIGNAL( hasActiveForm(bool) ), a, TQ_SLOT( setEnabled(bool) ) );
    a->addTo( menu );

    menu->insertSeparator();

    TQSignalMapper *mapper = new TQSignalMapper( this );
    connect( mapper, TQ_SIGNAL(mapped(const TQString&)), this, TQ_SLOT(previewForm(const TQString&)) );
    TQStringList styles = TQStyleFactory::keys();
    for ( TQStringList::Iterator it = styles.begin(); it != styles.end(); ++it ) {
	TQString info;
	if ( *it == "Motif" )
	    info = tr( "The preview will use the Motif look and feel which is used as the default style on most UNIX systems." );
	else if ( *it == "Windows" )
	    info = tr( "The preview will use the Windows look and feel." );
	else if ( *it == "Platinum" )
	    info = tr( "The preview will use the Platinum look and feel which is similar to the Macinosh GUI style." );
	else if ( *it == "CDE" )
	    info = tr( "The preview will use the CDE look and feel which is similar to some versions of the Common Desktop Environment." );
	else if ( *it == "SGI" )
	    info = tr( "The preview will use the Motif look and feel which is used as the default style on SGI systems." );
	else if ( *it == "MotifPlus" )
	    info = tr( "The preview will use the advanced Motif look and feel used by the GIMP toolkit (GTK) on Linux." );

	a = new TQAction( tr( "Preview Form in %1 Style" ).arg( *it ), TQPixmap(),
					 tr( "... in %1 Style" ).arg( *it ), 0, this, 0 );
	a->setStatusTip( tr("Opens a preview in %1 style").arg( *it ) );
	a->setWhatsThis( tr("<b>Open a preview in %1 style.</b>"
			"<p>Use the preview to test the design and "
			"signal-slot connections of the current form. %2</p>").arg( *it ).arg( info ) );
	mapper->setMapping( a, *it );
	connect( a, TQ_SIGNAL(activated()), mapper, TQ_SLOT(map()) );
	connect( this, TQ_SIGNAL( hasActiveForm(bool) ), a, TQ_SLOT( setEnabled(bool) ) );
	a->addTo( menu );
    }
}

void MainWindow::setupWindowActions()
{
    static bool windowActionsSetup = false;
    if ( !windowActionsSetup ) {
	windowActionsSetup = true;

	actionWindowTile = new TQAction( tr( "Tile" ), tr( "&Tile" ), 0, this );
	actionWindowTile->setStatusTip( tr("Tiles the windows so that they are all visible") );
	actionWindowTile->setWhatsThis( whatsThisFrom( "Window|Tile" ) );
	connect( actionWindowTile, TQ_SIGNAL( activated() ), qworkspace, TQ_SLOT( tile() ) );
	actionWindowCascade = new TQAction( tr( "Cascade" ), tr( "&Cascade" ), 0, this );
	actionWindowCascade->setStatusTip( tr("Cascades the windows so that all their title bars are visible") );
	actionWindowCascade->setWhatsThis( whatsThisFrom( "Window|Cascade" ) );
	connect( actionWindowCascade, TQ_SIGNAL( activated() ), qworkspace, TQ_SLOT( cascade() ) );

	actionWindowClose = new TQAction( tr( "Close" ), tr( "Cl&ose" ), CTRL + Key_F4, this );
	actionWindowClose->setStatusTip( tr( "Closes the active window") );
	actionWindowClose->setWhatsThis( whatsThisFrom( "Window|Close" ) );
	connect( actionWindowClose, TQ_SIGNAL( activated() ), qworkspace, TQ_SLOT( closeActiveWindow() ) );

	actionWindowCloseAll = new TQAction( tr( "Close All" ), tr( "Close Al&l" ), 0, this );
	actionWindowCloseAll->setStatusTip( tr( "Closes all form windows") );
	actionWindowCloseAll->setWhatsThis( whatsThisFrom( "Window|Close All" ) );
	connect( actionWindowCloseAll, TQ_SIGNAL( activated() ), qworkspace, TQ_SLOT( closeAllWindows() ) );

	actionWindowNext = new TQAction( tr( "Next" ), tr( "Ne&xt" ), CTRL + Key_F6, this );
	actionWindowNext->setStatusTip( tr( "Activates the next window" ) );
	actionWindowNext->setWhatsThis( whatsThisFrom( "Window|Next" ) );
	connect( actionWindowNext, TQ_SIGNAL( activated() ), qworkspace, TQ_SLOT( activateNextWindow() ) );

	actionWindowPrevious = new TQAction( tr( "Previous" ), tr( "Pre&vious" ), CTRL + SHIFT + Key_F6, this );
	actionWindowPrevious->setStatusTip( tr( "Activates the previous window" ) );
	actionWindowPrevious->setWhatsThis( whatsThisFrom( "Window|Previous" ) );
	connect( actionWindowPrevious, TQ_SIGNAL( activated() ), qworkspace, TQ_SLOT( activatePreviousWindow() ) );
    }

    if ( !windowMenu ) {
	windowMenu = new TQPopupMenu( this, "Window" );
	menubar->insertItem( tr( "&Window" ), windowMenu );
	connect( windowMenu, TQ_SIGNAL( aboutToShow() ),
		 this, TQ_SLOT( setupWindowActions() ) );
    } else {
	windowMenu->clear();
    }

    actionWindowClose->addTo( windowMenu );
    actionWindowCloseAll->addTo( windowMenu );
    windowMenu->insertSeparator();
    actionWindowNext->addTo( windowMenu );
    actionWindowPrevious->addTo( windowMenu );
    windowMenu->insertSeparator();
    actionWindowTile->addTo( windowMenu );
    actionWindowCascade->addTo( windowMenu );
    windowMenu->insertSeparator();
    windowMenu->insertItem( tr( "Vie&ws" ), createDockWindowMenu( NoToolBars ) );
    windowMenu->insertItem( tr( "Tool&bars" ), createDockWindowMenu( OnlyToolBars ) );
    TQWidgetList windows = qworkspace->windowList();
    if ( windows.count() && formWindow() )
	windowMenu->insertSeparator();
    int j = 0;
    for ( int i = 0; i < int( windows.count() ); ++i ) {
	TQWidget *w = windows.at( i );
	if ( !::tqt_cast<FormWindow*>(w) && !::tqt_cast<SourceEditor*>(w) )
	    continue;
	if ( ::tqt_cast<FormWindow*>(w) && ( ( (FormWindow*)w )->isFake() ) )
	    continue;
	j++;
	TQString itemText;
	if ( j < 10 )
	    itemText = TQString("&%1 ").arg( j );
	if ( ::tqt_cast<FormWindow*>(w) )
	    itemText += w->name();
	else
	    itemText += w->caption();

	int id = windowMenu->insertItem( itemText, this, TQ_SLOT( windowsMenuActivated( int ) ) );
	windowMenu->setItemParameter( id, i );
	windowMenu->setItemChecked( id, qworkspace->activeWindow() == windows.at( i ) );
    }
}

void MainWindow::setupHelpActions()
{
    actionHelpContents = new TQAction( tr( "Contents" ), tr( "&Contents" ), Key_F1, this, 0 );
    actionHelpContents->setStatusTip( tr("Opens the online help") );
    actionHelpContents->setWhatsThis( whatsThisFrom( "Help|Contents" ) );
    connect( actionHelpContents, TQ_SIGNAL( activated() ), this, TQ_SLOT( helpContents() ) );

    actionHelpManual = new TQAction( tr( "Manual" ), tr( "&Manual" ), CTRL + Key_M, this, 0 );
    actionHelpManual->setStatusTip( tr("Opens the TQt Designer manual") );
    actionHelpManual->setWhatsThis( whatsThisFrom( "Help|Manual" ) );
    connect( actionHelpManual, TQ_SIGNAL( activated() ), this, TQ_SLOT( helpManual() ) );

    actionHelpAbout = new TQAction( tr("About"), TQPixmap(), tr("&About"), 0, this, 0 );
    actionHelpAbout->setStatusTip( tr("Displays information about TQt Designer") );
    actionHelpAbout->setWhatsThis( whatsThisFrom( "Help|About" ) );
    connect( actionHelpAbout, TQ_SIGNAL( activated() ), this, TQ_SLOT( helpAbout() ) );

    actionHelpAboutTQt = new TQAction( tr("About TQt"), TQPixmap(), tr("About &TQt"), 0, this, 0 );
    actionHelpAboutTQt->setStatusTip( tr("Displays information about the TQt Toolkit") );
    actionHelpAboutTQt->setWhatsThis( whatsThisFrom( "Help|About TQt" ) );
    connect( actionHelpAboutTQt, TQ_SIGNAL( activated() ), this, TQ_SLOT( helpAboutTQt() ) );

    actionHelpWhatsThis = new TQAction( tr("What's This?"), TQIconSet( whatsthis_image, whatsthis_image ),
				       tr("What's This?"), SHIFT + Key_F1, this, 0 );
    actionHelpWhatsThis->setStatusTip( tr("\"What's This?\" context sensitive help") );
    actionHelpWhatsThis->setWhatsThis( whatsThisFrom( "Help|What's This?" ) );
    connect( actionHelpWhatsThis, TQ_SIGNAL( activated() ), this, TQ_SLOT( whatsThis() ) );

    TQToolBar *tb = new TQToolBar( this, "Help" );
    tb->setCloseMode( TQDockWindow::Undocked );
    TQWhatsThis::add( tb, tr( "<b>The Help toolbar</b>%1" ).arg(tr(toolbarHelp).arg("") ));
    addToolBar( tb, tr( "Help" ) );
    actionHelpWhatsThis->addTo( tb );

    TQPopupMenu *menu = new TQPopupMenu( this, "Help" );
    menubar->insertSeparator();
    menubar->insertItem( tr( "&Help" ), menu );
    actionHelpContents->addTo( menu );
    actionHelpManual->addTo( menu );
    menu->insertSeparator();
    actionHelpAbout->addTo( menu );
    actionHelpAboutTQt->addTo( menu );
    menu->insertSeparator();
    actionHelpWhatsThis->addTo( menu );
}

void MainWindow::fileNew()
{
    statusBar()->message( tr( "Create a new project, form or source file...") );
    NewForm dlg( this, projectNames(), currentProject->projectName(), templatePath() );
    dlg.exec();
    statusBar()->clear();
}

void MainWindow::fileNewDialog()
{
    static int forms = 0;
    TQString n = "Dialog" + TQString::number( ++forms );
    while ( currentProject->findFormFile( n + ".ui" ) )
	n = "Dialog" + TQString::number( ++forms );
    FormWindow *fw = 0;
    FormFile *ff = new FormFile( n + ".ui", false, currentProject );
    fw = new FormWindow( ff, MainWindow::self, MainWindow::self->qWorkspace(), n );
    ff->setModified( true );
    currentProject->setModified( true );
    workspace()->update();
    fw->setProject( currentProject );
    MetaDataBase::addEntry( fw );
    TQWidget *w = WidgetFactory::create( WidgetDatabase::idFromClassName( "TQDialog" ), fw, n.latin1() );
    fw->setMainContainer( w );
    fw->setCaption( n );
    fw->resize( 600, 480 );
    insertFormWindow( fw );
    fw->killAccels( fw );
    fw->project()->setModified( true );
    fw->setFocus();
    fw->setSavePixmapInProject( true );
    fw->setSavePixmapInline( false );
}

void MainWindow::fileNewFile()
{
    TQString name = TQInputDialog::getText( tr( "Name of File" ), tr( "Enter the name of the new source file:" ) );
    if ( name.isEmpty() )
	return;
    if ( name.right( 3 ) != ".qs" )
	name += ".qs";
    SourceFile *f = new SourceFile( name, false, currentProject );
    MainWindow::self->editSource( f );
    f->setModified( true );
    currentProject->setModified( true );
    workspace()->update();
}

void MainWindow::fileQuit()
{
    close();
    if ( !singleProjectMode() )
	tqApp->closeAllWindows();
}

void MainWindow::fileClose()
{
    if ( !currentProject->isDummy() ) {
	fileCloseProject();
    } else {
	TQWidget *w = qworkspace->activeWindow();
	if ( w ) {
	    if ( ::tqt_cast<FormWindow*>(w) )
		( (FormWindow*)w )->formFile()->close();
	    else if ( ::tqt_cast<SourceEditor*>(w) )
		( (SourceEditor*)w )->close();
	}
    }
}


void MainWindow::fileCloseProject()
{
    if ( currentProject->isDummy() )
	return;
    Project *pro = currentProject;
    TQAction* a = 0;
    TQAction* lastValid = 0;
    for ( TQMap<TQAction*, Project* >::Iterator it = projects.begin(); it != projects.end(); ++it ) {
	if ( it.data() == pro ) {
	    a = it.key();
	    if ( lastValid )
		break;
	}
	lastValid = it.key();
    }
    if ( a ) {
	if ( pro->isModified() ) {
	    switch ( TQMessageBox::warning( this, tr( "Save Project Settings" ),
					   tr( "Save changes to '%1'?" ).arg( pro->fileName() ),
					   tr( "&Yes" ), tr( "&No" ), tr( "&Cancel" ), 0, 2 ) ) {
	    case 0: // save
		pro->save();
		break;
	    case 1: // don't save
		break;
	    case 2: // cancel
		return;
	    default:
		break;
	    }
	}

	TQWidgetList windows = qWorkspace()->windowList();
	qWorkspace()->blockSignals( true );
	TQWidgetListIt wit( windows );
	while ( wit.current() ) {
	    TQWidget *w = wit.current();
	    ++wit;
	    if ( ::tqt_cast<FormWindow*>(w) ) {
		if ( ( (FormWindow*)w )->project() == pro ) {
		    if ( ( (FormWindow*)w )->formFile()->editor() )
			windows.removeRef( ( (FormWindow*)w )->formFile()->editor() );
		    if ( !( (FormWindow*)w )->formFile()->close() )
			return;
		}
	    } else if ( ::tqt_cast<SourceEditor*>(w) ) {
		if ( !( (SourceEditor*)w )->close() )
		    return;
	    }
	}
	hierarchyView->clear();
	windows = qWorkspace()->windowList();
	qWorkspace()->blockSignals( false );
	actionGroupProjects->removeChild( a );
	projects.remove( a );
	delete a;
	currentProject = 0;
	if ( lastValid ) {
	    projectSelected( lastValid );
	    statusBar()->message( "Selected project '" + tr( currentProject->projectName() + "'") );
	}
	if ( !windows.isEmpty() ) {
	    for ( TQWidget *w = windows.first(); w; w = windows.next() ) {
		if (!::tqt_cast<FormWindow*>(w) || w->isHidden())
		    continue;
		w->setFocus();
		activeWindowChanged( w );
		break;
	    }
	} else {
	    emit hasActiveWindow( false );
	    emit hasActiveForm( false );
	    updateUndoRedo( false, false, TQString::null, TQString::null );
	}
    }
}

void MainWindow::fileOpen() // as called by the menu
{
    fileOpen( "", "", "", false );
}

void MainWindow::projectInsertFile()
{
    fileOpen( "", "" );
}

TQ_EXPORT void MainWindow::fileOpen( const TQString &filter, const TQString &extension, const TQString &fn, bool inProject  )
{
    statusBar()->message( tr( "Open a file...") );

    TQPluginManager<ImportFilterInterface> manager( IID_ImportFilter, TQApplication::libraryPaths(), pluginDirectory() );

    Project* project = inProject ? currentProject : eProject;

    TQStringList additionalSources;

    {
	TQStringList filterlist;
	if ( filter.isEmpty() ) {
	    if ( !inProject )
		filterlist << tr( "Designer Files (*.ui *.pro)" );
	    filterlist << tr( "TQt User-Interface Files (*.ui)" );
	    if ( !inProject )
		filterlist << tr( "QMAKE Project Files (*.pro)" );
	    TQStringList list = manager.featureList();
	    for ( TQStringList::Iterator it = list.begin(); it != list.end(); ++it )
		filterlist << *it;
	    LanguageInterface *iface = MetaDataBase::languageInterface( project->language() );
	    if ( iface ) {
		filterlist +=  iface->fileFilterList();
		additionalSources += iface->fileExtensionList();
	    }
	    filterlist << tr( "All Files (*)" );
	} else {
	    filterlist << filter;
	}

	TQString filters = filterlist.join( ";;" );

	TQStringList filenames;
	if ( fn.isEmpty() ) {
	    if ( !inProject ) {
		TQString f = TQFileDialog::getOpenFileName( TQString::null, filters, this, 0,
							  tr("Open" ), &lastOpenFilter );
		filenames << f;
	    } else {
		filenames = TQFileDialog::getOpenFileNames( filters, TQString::null, this, 0,
							   tr("Add"), &lastOpenFilter );
	    }
	} else {
	    filenames << fn;
	}

	for ( TQStringList::Iterator fit = filenames.begin(); fit != filenames.end(); ++fit ) {
	    TQString filename = *fit;
	    if ( !filename.isEmpty() ) {
		TQFileInfo fi( filename );

		if ( fi.extension( false ) == "pro" && ( extension.isEmpty() || extension.find( ";pro" ) != -1 ) ) {
		    addRecentlyOpened( filename, recentlyProjects );
		    openProject( filename );
		} else if ( fi.extension( false ) == "ui" && ( extension.isEmpty() || extension.find( ";ui" ) != -1 ) ) {
		    if ( !inProject )
			setCurrentProject( eProject );
		    openFormWindow( filename );
		    addRecentlyOpened( filename, recentlyFiles );
		} else if ( ( !extension.isEmpty() && extension.find( ";" + fi.extension( false ) ) != -1 ) ||
			    additionalSources.find( fi.extension( false ) ) != additionalSources.end() ) {
		    SourceFile *sf = project->findSourceFile( project->makeRelative( filename ) );
		    if ( !sf )
			sf = new SourceFile( project->makeRelative( filename ), false, project );
		    editSource( sf );
		} else if ( extension.isEmpty() ) {
		    TQString filter;
		    for ( TQStringList::Iterator it2 = filterlist.begin(); it2 != filterlist.end(); ++it2 ) {
			if ( (*it2).contains( "." + fi.extension( false ), false ) ) {
			    filter = *it2;
			    break;
			}
		    }

		    ImportFilterInterface* iface = 0;
		    manager.queryInterface( filter, &iface );
		    if ( !iface ) {
			statusBar()->message( tr( "No import filter is available to import '%1'").
					      arg( filename ), 3000 );
			return;
		    }
		    statusBar()->message( tr( "Importing '%1' using import filter ...").arg( filename ) );
		    TQStringList list = iface->import( filter, filename );
		    iface->release();
		    if ( list.isEmpty() ) {
			statusBar()->message( tr( "Nothing to load in '%1'").arg( filename ), 3000 );
			return;
		    }
		    if ( !inProject )
			setCurrentProject( eProject );
		    addRecentlyOpened( filename, recentlyFiles );
		    for ( TQStringList::Iterator it = list.begin(); it != list.end(); ++it ) {
			openFormWindow( *it, false );
			TQFile::remove( *it );
		    }
		    statusBar()->clear();
		}
	    }
	}
    }
}

TQ_EXPORT FormWindow *MainWindow::openFormWindow( const TQString &filename, bool validFileName, FormFile *ff )
{
    if ( filename.isEmpty() )
	return 0;

    bool makeNew = false;

    if ( !TQFile::exists( filename ) ) {
	makeNew = true;
    } else {
	TQFile f( filename );
	f.open( IO_ReadOnly );
	TQTextStream ts( &f );
	makeNew = ts.read().length() < 2;
    }
    if ( makeNew ) {
	fileNew();
	if ( formWindow() )
	    formWindow()->setFileName( filename );
	return formWindow();
    }

    statusBar()->message( tr( "Reading file '%1'...").arg( filename ) );
    FormFile *ff2 = currentProject->findFormFile( currentProject->makeRelative(filename) );
    if ( ff2 && ff2->formWindow() ) {
	ff2->formWindow()->setFocus();
	return ff2->formWindow();
    }

    if ( ff2 )
	ff = ff2;
    TQApplication::setOverrideCursor( WaitCursor );
    Resource resource( this );
    if ( !ff )
	ff = new FormFile( currentProject->makeRelative( filename ), false, currentProject );
    bool b = resource.load( ff ) && (FormWindow*)resource.widget();
    if ( !validFileName && resource.widget() )
	( (FormWindow*)resource.widget() )->setFileName( TQString::null );
    TQApplication::restoreOverrideCursor();
    if ( b ) {
	rebuildCustomWidgetGUI();
	statusBar()->message( tr( "Loaded file '%1'").arg( filename ), 3000 );
    } else {
	statusBar()->message( tr( "Failed to load file '%1'").arg( filename ), 5000 );
	TQMessageBox::information( this, tr("Load File"), tr("Couldn't load file '%1'").arg( filename ) );
	delete ff;
    }
    return (FormWindow*)resource.widget();
}

bool MainWindow::fileSave()
{

    if ( !currentProject->isDummy() )
	return fileSaveProject();
    return fileSaveForm();
}

bool MainWindow::fileSaveForm()
{
    for ( SourceEditor *e = sourceEditors.first(); e; e = sourceEditors.next() ) {
	if ( e->object() == formWindow() || e == qWorkspace()->activeWindow() ) {
	    e->save();
	}
    }

    FormWindow *fw = 0;

    TQWidget *w = qWorkspace()->activeWindow();
    if ( w ) {
	if ( ::tqt_cast<SourceEditor*>(w) ) {
	    SourceEditor *se = (SourceEditor*)w;
	    if ( se->formWindow() )
		fw = se->formWindow();
	    else if ( se->sourceFile() ) {
		se->sourceFile()->save();
		return true;
	    }
	}
    }

    if ( !fw )
	fw = formWindow();
    if ( !fw || !fw->formFile()->save() )
	return false;
    TQApplication::restoreOverrideCursor();
    return true;
}

bool MainWindow::fileSaveProject()
{
    currentProject->save();
    statusBar()->message( tr( "Project '%1' saved.").arg( currentProject->projectName() ), 3000 );
    return true;
}

bool MainWindow::fileSaveAs()
{
    statusBar()->message( tr( "Enter a filename..." ) );

    TQWidget *w = qworkspace->activeWindow();
    if ( !w )
	return true;
    if ( ::tqt_cast<FormWindow*>(w) )
	return ( (FormWindow*)w )->formFile()->saveAs();
    else if ( ::tqt_cast<SourceEditor*>(w) )
	return ( (SourceEditor*)w )->saveAs();
    return false;
}

void MainWindow::fileSaveAll()
{
    for ( TQMap<TQAction*, Project* >::Iterator it = projects.begin(); it != projects.end(); ++it )
	(*it)->save();
}

void MainWindow::fileCreateTemplate()
{
    CreateTemplate dia( this, 0, true );

    int i = 0;
    for ( i = 0; i < WidgetDatabase::count(); ++i ) {
	if ( WidgetDatabase::isForm( i ) && WidgetDatabase::group( i ) != "Temp") {
	    dia.listClass->insertItem( WidgetDatabase::className( i ) );
	}
    }
    for ( i = 0; i < WidgetDatabase::count(); ++i ) {
	if ( WidgetDatabase::isContainer( i ) && !WidgetDatabase::isForm(i) &&
	     WidgetDatabase::className( i ) != "TQTabWidget" && WidgetDatabase::group( i ) != "Temp" ) {
	    dia.listClass->insertItem( WidgetDatabase::className( i ) );
	}
    }

    TQPtrList<MetaDataBase::CustomWidget> *lst = MetaDataBase::customWidgets();
    for ( MetaDataBase::CustomWidget *w = lst->first(); w; w = lst->next() ) {
	if ( w->isContainer )
	    dia.listClass->insertItem( w->className );
    }

    dia.editName->setText( tr( "NewTemplate" ) );
    connect( dia.buttonCreate, TQ_SIGNAL( clicked() ),
	     this, TQ_SLOT( createNewTemplate() ) );
    dia.exec();
}

void MainWindow::createNewTemplate()
{
    CreateTemplate *dia = (CreateTemplate*)sender()->parent();
    TQString fn = dia->editName->text();
    TQString cn = dia->listClass->currentText();
    if ( fn.isEmpty() || cn.isEmpty() ) {
	TQMessageBox::information( this, tr( "Create Template" ), tr( "Couldn't create the template" ) );
	return;
    }

    TQStringList templRoots;
    const char *qtdir = getenv( "TQTDIR" );
    if(qtdir)
	templRoots << qtdir;
    templRoots << tqInstallPathData();
    if(qtdir) //try the tools/designer directory last!
	templRoots << (TQString(qtdir) + "/tools/designer");
    TQFile f;
    for ( TQStringList::Iterator it = templRoots.begin(); it != templRoots.end(); ++it ) {
	if ( TQFile::exists( (*it) + "/templates/" )) {
	    TQString tmpfn = (*it) + "/templates/" + fn + ".ui";
	    f.setName(tmpfn);
	    if(f.open(IO_WriteOnly))
		break;
	}
    }
    if ( !f.isOpen() ) {
	TQMessageBox::information( this, tr( "Create Template" ), tr( "Couldn't create the template" ) );
	return;
    }
    TQTextStream ts( &f );

    ts << "<!DOCTYPE UI><UI>" << endl;
    ts << "<widget>" << endl;
    ts << "<class>" << cn << "</class>" << endl;
    ts << "<property stdset=\"1\">" << endl;
    ts << "    <name>name</name>" << endl;
    ts << "    <cstring>" << cn << "Form</cstring>" << endl;
    ts << "</property>" << endl;
    ts << "<property stdset=\"1\">" << endl;
    ts << "    <name>geometry</name>" << endl;
    ts << "    <rect>" << endl;
    ts << "        <width>300</width>" << endl;
    ts << "        <height>400</height>" << endl;
    ts << "    </rect>" << endl;
    ts << "</property>" << endl;
    ts << "</widget>" << endl;
    ts << "</UI>" << endl;

    dia->editName->setText( tr( "NewTemplate" ) );

    f.close();
}

void MainWindow::editUndo()
{
    if ( qWorkspace()->activeWindow() &&
	 ::tqt_cast<SourceEditor*>(qWorkspace()->activeWindow()) ) {
	( (SourceEditor*)qWorkspace()->activeWindow() )->editUndo();
	return;
    }
    if ( formWindow() )
	formWindow()->undo();
}

void MainWindow::editRedo()
{
    if ( qWorkspace()->activeWindow() &&
	 ::tqt_cast<SourceEditor*>(qWorkspace()->activeWindow()) ) {
	( (SourceEditor*)qWorkspace()->activeWindow() )->editRedo();
	return;
    }
    if ( formWindow() )
	formWindow()->redo();
}

void MainWindow::editCut()
{
    if ( qWorkspace()->activeWindow() &&
	 ::tqt_cast<SourceEditor*>(qWorkspace()->activeWindow()) ) {
	( (SourceEditor*)qWorkspace()->activeWindow() )->editCut();
	return;
    }
    editCopy();
    editDelete();
}

void MainWindow::editCopy()
{
    if ( qWorkspace()->activeWindow() &&
	 ::tqt_cast<SourceEditor*>(qWorkspace()->activeWindow()) ) {
	( (SourceEditor*)qWorkspace()->activeWindow() )->editCopy();
	return;
    }
    if ( formWindow() )
	tqApp->clipboard()->setText( formWindow()->copy() );
}

void MainWindow::editPaste()
{
    if ( qWorkspace()->activeWindow() &&
	 ::tqt_cast<SourceEditor*>(qWorkspace()->activeWindow()) ) {
	( (SourceEditor*)qWorkspace()->activeWindow() )->editPaste();
	return;
    }
    if ( !formWindow() )
	return;

    TQWidget *w = formWindow()->mainContainer();
    TQWidgetList l( formWindow()->selectedWidgets() );
    if ( l.count() == 1 ) {
	w = l.first();
	if ( WidgetFactory::layoutType( w ) != WidgetFactory::NoLayout ||
	     ( !WidgetDatabase::isContainer( WidgetDatabase::idFromClassName( WidgetFactory::classNameOf( w ) ) ) &&
	       w != formWindow()->mainContainer() ) )
	    w = formWindow()->mainContainer();
    }

    if ( w && WidgetFactory::layoutType( w ) == WidgetFactory::NoLayout ) {
	formWindow()->paste( tqApp->clipboard()->text(), WidgetFactory::containerOfWidget( w ) );
	hierarchyView->widgetInserted( 0 );
	formWindow()->commandHistory()->setModified( true );
    } else {
	TQMessageBox::information( this, tr( "Paste error" ),
				  tr( "Can't paste widgets. Designer couldn't find a container\n"
				      "to paste into which does not contain a layout. Break the layout\n"
				      "of the container you want to paste into and select this container\n"
				      "and then paste again." ) );
    }
}

void MainWindow::editDelete()
{
    if ( formWindow() )
	formWindow()->deleteWidgets();
}

void MainWindow::editSelectAll()
{
    if ( qWorkspace()->activeWindow() &&
	 ::tqt_cast<SourceEditor*>(qWorkspace()->activeWindow()) ) {
	( (SourceEditor*)qWorkspace()->activeWindow() )->editSelectAll();
	return;
    }
    if ( formWindow() )
	formWindow()->selectAll();
}


void MainWindow::editLower()
{
    if ( formWindow() )
	formWindow()->lowerWidgets();
}

void MainWindow::editRaise()
{
    if ( formWindow() )
	formWindow()->raiseWidgets();
}

void MainWindow::editAdjustSize()
{
    if ( formWindow() )
	formWindow()->editAdjustSize();
}

void MainWindow::editLayoutHorizontal()
{
    if ( layoutChilds )
	editLayoutContainerHorizontal();
    else if ( layoutSelected && formWindow() )
	formWindow()->layoutHorizontal();
}

void MainWindow::editLayoutVertical()
{
    if ( layoutChilds )
	editLayoutContainerVertical();
    else if ( layoutSelected && formWindow() )
	formWindow()->layoutVertical();
}

void MainWindow::editLayoutHorizontalSplit()
{
    if ( layoutChilds )
	; // no way to do that
    else if ( layoutSelected && formWindow() )
	formWindow()->layoutHorizontalSplit();
}

void MainWindow::editLayoutVerticalSplit()
{
    if ( layoutChilds )
	; // no way to do that
    else if ( layoutSelected && formWindow() )
	formWindow()->layoutVerticalSplit();
}

void MainWindow::editLayoutGrid()
{
    if ( layoutChilds )
	editLayoutContainerGrid();
    else if ( layoutSelected && formWindow() )
	formWindow()->layoutGrid();
}

void MainWindow::editLayoutContainerVertical()
{
    if ( !formWindow() )
	return;
    TQWidget *w = formWindow()->mainContainer();
    TQWidgetList l( formWindow()->selectedWidgets() );
    if ( l.count() == 1 )
	w = l.first();
    if ( w )
	formWindow()->layoutVerticalContainer( w  );
}

void MainWindow::editLayoutContainerHorizontal()
{
    if ( !formWindow() )
	return;
    TQWidget *w = formWindow()->mainContainer();
    TQWidgetList l( formWindow()->selectedWidgets() );
    if ( l.count() == 1 )
	w = l.first();
    if ( w )
	formWindow()->layoutHorizontalContainer( w );
}

void MainWindow::editLayoutContainerGrid()
{
    if ( !formWindow() )
	return;
    TQWidget *w = formWindow()->mainContainer();
    TQWidgetList l( formWindow()->selectedWidgets() );
    if ( l.count() == 1 )
	w = l.first();
    if ( w )
	formWindow()->layoutGridContainer( w  );
}

void MainWindow::editBreakLayout()
{
    if ( !formWindow() || !breakLayout )
	return;
    TQWidget *w = formWindow()->mainContainer();
    if ( formWindow()->currentWidget() )
	w = formWindow()->currentWidget();
    if ( WidgetFactory::layoutType( w ) != WidgetFactory::NoLayout ||
	 ( w->parentWidget() && WidgetFactory::layoutType( w->parentWidget() ) != WidgetFactory::NoLayout ) ) {
	formWindow()->breakLayout( w );
	return;
    } else {
	TQWidgetList widgets = formWindow()->selectedWidgets();
	for ( w = widgets.first(); w; w = widgets.next() ) {
	    if ( WidgetFactory::layoutType( w ) != WidgetFactory::NoLayout ||
		 ( w->parentWidget() && WidgetFactory::layoutType( w->parentWidget() ) != WidgetFactory::NoLayout ) )
		break;
	}
	if ( w ) {
	    formWindow()->breakLayout( w );
	    return;
	}
    }

    w = formWindow()->mainContainer();
    if ( WidgetFactory::layoutType( w ) != WidgetFactory::NoLayout ||
	 ( w->parentWidget() && WidgetFactory::layoutType( w->parentWidget() ) != WidgetFactory::NoLayout ) )
	formWindow()->breakLayout( w );
}

void MainWindow::editAccels()
{
    if ( !formWindow() )
	return;
    formWindow()->checkAccels();
}

void MainWindow::editFunctions()
{
    if ( !formWindow() )
	return;

    statusBar()->message( tr( "Edit the current form's slots..." ) );
    EditFunctions dlg( this, formWindow(), true );
    dlg.exec();
    statusBar()->clear();
}

void MainWindow::editConnections()
{
    if ( !formWindow() )
	return;

    statusBar()->message( tr( "Edit the current form's connections..." ) );
    ConnectionDialog dlg( this );
    dlg.exec();

    statusBar()->clear();
}

SourceEditor *MainWindow::editSource()
{
    if ( !formWindow() )
	return 0;
    return formWindow()->formFile()->showEditor();
}

SourceEditor *MainWindow::openSourceEditor()
{
    if ( !formWindow() )
	return 0;
    
    TQString lang = currentProject->language();
    if ( !MetaDataBase::hasEditor( lang ) ) {
	TQMessageBox::information( this, tr( "Edit Source" ),
				  tr( "There is no plugin for editing " + lang + " code installed!\n"
				      "Note: Plugins are not available in static TQt configurations." ) );
	return 0;
    }

    SourceEditor *editor = 0;
    for ( SourceEditor *e = sourceEditors.first(); e; e = sourceEditors.next() ) {
	if ( e->language() == lang && e->formWindow() == formWindow() ) {
	    editor = e;
	    break;
	}
    }

    if ( !editor )
	editor = createSourceEditor( formWindow(), formWindow()->project(), lang );
    return editor;
}

SourceEditor *MainWindow::editSource( SourceFile *f )
{
    TQString lang = currentProject->language(); 
    if ( !MetaDataBase::hasEditor( lang ) ) {
	TQMessageBox::information( this, tr( "Edit Source" ),
				  tr( "There is no plugin for editing " + lang + " code installed!\n"
				      "Note: Plugins are not available in static TQt configurations." ) );
	return 0;
    }

    SourceEditor *editor = 0;
    if ( f )
	editor = f->editor();
    if ( !editor )
	editor = createSourceEditor( f, currentProject, lang );
    editor->setActiveWindow();
    editor->setFocus();
    return editor;
}

SourceEditor *MainWindow::createSourceEditor( TQObject *object, Project *project,
					      const TQString &lang, const TQString &func,
					      bool rereadSource )
{
    SourceEditor *editor = 0;
    EditorInterface *eIface = 0;
    editorPluginManager->queryInterface( lang, &eIface );
    if ( !eIface )
	return 0;
    LanguageInterface *lIface = MetaDataBase::languageInterface( lang );
    if ( !lIface )
	return 0;
    TQApplication::setOverrideCursor( WaitCursor );
    editor = new SourceEditor( qWorkspace(), eIface, lIface );
    eIface->release();
    lIface->release();
    
    editor->setLanguage( lang );
    sourceEditors.append( editor );
    TQApplication::restoreOverrideCursor();

    if ( editor->object() != object )
	editor->setObject( object, project );
    else if ( rereadSource )
	editor->refresh( false );
    
    editor->show();
    editor->setFocus();
    if ( !func.isEmpty() )
	editor->setFunction( func );
    emit editorChanged();
    
    return editor;
}

void MainWindow::editFormSettings()
{
    if ( !formWindow() )
	return;

    statusBar()->message( tr( "Edit the current form's settings..." ) );
    FormSettings dlg( this, formWindow() );
    dlg.exec();
    statusBar()->clear();
}

void MainWindow::editProjectSettings()
{
    openProjectSettings( currentProject );
    wspace->setCurrentProject( currentProject );
}

void MainWindow::editPixmapCollection()
{
    PixmapCollectionEditor dia( this, 0, true );
    dia.setProject( currentProject );
    dia.exec();
}

void MainWindow::editDatabaseConnections()
{
#ifndef TQT_NO_SQL
    DatabaseConnectionsEditor dia( currentProject, this, 0, true );
    dia.exec();
#endif
}

void MainWindow::editPreferences()
{
    statusBar()->message( tr( "Edit preferences..." ) );
    Preferences *dia = new Preferences( this, 0, true );
    prefDia = dia;
    connect( dia->helpButton, TQ_SIGNAL( clicked() ), MainWindow::self, TQ_SLOT( showDialogHelp() ) );
    dia->buttonColor->setEditor( StyledButton::ColorEditor );
    dia->buttonPixmap->setEditor( StyledButton::PixmapEditor );
    dia->groupBoxGrid->setChecked( sGrid );
    dia->checkBoxSnapGrid->setChecked( snGrid );
    dia->spinGridX->setValue( grid().x() );
    dia->spinGridY->setValue( grid().y() );
    dia->checkBoxWorkspace->setChecked( restoreConfig );
    dia->checkBoxTextLabels->setChecked( usesTextLabel() );
    dia->buttonColor->setColor( qworkspace->backgroundColor() );
    TQString pluginPaths = TQApplication::libraryPaths().join("\n");
    dia->textEditPluginPaths->setText(pluginPaths);
    if ( qworkspace->backgroundPixmap() )
	dia->buttonPixmap->setPixmap( *qworkspace->backgroundPixmap() );
    if ( backPix )
	dia->radioPixmap->setChecked( true );
    else
	dia->radioColor->setChecked( true );
    dia->checkBoxSplash->setChecked( splashScreen );
    dia->checkAutoEdit->setChecked( !databaseAutoEdit );
    dia->checkBoxStartDialog->setChecked( shStartDialog );
    dia->checkBoxAutoSave->setChecked( autoSaveEnabled );
    int h = autoSaveInterval / 3600;
    int m = ( autoSaveInterval - h * 3600 ) / 60;
    int s = autoSaveInterval - ( h * 3600 + m * 60 );
    TQTime t( h, m, s );
    dia->timeEditAutoSave->setTime( t );

    SenderObject *senderObject = new SenderObject( designerInterface() );
    TQValueList<Tab>::Iterator it;
    for ( it = preferenceTabs.begin(); it != preferenceTabs.end(); ++it ) {
	Tab t = *it;
	dia->tabWidget->addTab( t.w, t.title );
	if ( t.receiver ) {
	    connect( dia->buttonOk, TQ_SIGNAL( clicked() ), senderObject, TQ_SLOT( emitAcceptSignal() ) );
	    connect( senderObject, TQ_SIGNAL( acceptSignal( TQUnknownInterface * ) ), t.receiver, t.accept_slot );
	    connect( senderObject, TQ_SIGNAL( initSignal( TQUnknownInterface * ) ), t.receiver, t.init_slot );
	    senderObject->emitInitSignal();
	    disconnect( senderObject, TQ_SIGNAL( initSignal( TQUnknownInterface * ) ), t.receiver, t.init_slot );
	}
    }

    if ( dia->exec() == TQDialog::Accepted ) {
	setSnapGrid( dia->checkBoxSnapGrid->isChecked() );
	setShowGrid( dia->groupBoxGrid->isChecked() );
	setGrid( TQPoint( dia->spinGridX->value(),
			 dia->spinGridY->value() ) );
	restoreConfig = dia->checkBoxWorkspace->isChecked();
	setUsesTextLabel( dia->checkBoxTextLabels->isChecked() );
	if (dia->textEditPluginPaths->isModified()) {
	    pluginPaths = dia->textEditPluginPaths->text();
	    TQApplication::setLibraryPaths(TQStringList::split("\n", pluginPaths));
	    savePluginPaths = true;
	}

	if ( dia->radioPixmap->isChecked() && dia->buttonPixmap->pixmap() ) {
	    qworkspace->setBackgroundPixmap( *dia->buttonPixmap->pixmap() );
	    backPix = true;
	} else {
	    qworkspace->setBackgroundColor( dia->buttonColor->color() );
	    backPix = false;
	}
	splashScreen = dia->checkBoxSplash->isChecked();
	databaseAutoEdit = !dia->checkAutoEdit->isChecked();
	shStartDialog = dia->checkBoxStartDialog->isChecked();
	autoSaveEnabled = dia->checkBoxAutoSave->isChecked();
	TQTime time = dia->timeEditAutoSave->time();
	autoSaveInterval = time.hour() * 3600 + time.minute() * 60 + time.second();
	if ( autoSaveEnabled )
	    autoSaveTimer->start( autoSaveInterval * 1000 );
	else
	    autoSaveTimer->stop();
    }
    delete senderObject;
    for ( it = preferenceTabs.begin(); it != preferenceTabs.end(); ++it ) {
	Tab t = *it;
	dia->tabWidget->removePage( t.w );
	t.w->reparent( 0, TQPoint(0,0), false );
    }

    for ( SourceEditor *e = sourceEditors.first(); e; e = sourceEditors.next() )
	e->configChanged();

    delete dia;
    prefDia = 0;
    statusBar()->clear();
}

void MainWindow::searchFind()
{
    if ( !qWorkspace()->activeWindow() ||
	 !::tqt_cast<SourceEditor*>(qWorkspace()->activeWindow()) )
	 return;

    if ( !findDialog )
	findDialog = new FindDialog( this, 0, false );
    findDialog->show();
    findDialog->raise();
    findDialog->setEditor( ( (SourceEditor*)qWorkspace()->activeWindow() )->editorInterface(),
			   ( (SourceEditor*)qWorkspace()->activeWindow() )->object() );
    findDialog->comboFind->setFocus();
    findDialog->comboFind->lineEdit()->selectAll();
}

void MainWindow::searchIncremetalFindMenu()
{
    incrementalSearch->selectAll();
    incrementalSearch->setFocus();
}

void MainWindow::searchIncremetalFind()
{
    if ( !qWorkspace()->activeWindow() ||
	 !::tqt_cast<SourceEditor*>(qWorkspace()->activeWindow()) )
	 return;

    ( (SourceEditor*)qWorkspace()->activeWindow() )->editorInterface()->find( incrementalSearch->text(),
									     false, false, true, false );
}

void MainWindow::searchIncremetalFindNext()
{
    if ( !qWorkspace()->activeWindow() ||
	 !::tqt_cast<SourceEditor*>(qWorkspace()->activeWindow()) )
	 return;

    ( (SourceEditor*)qWorkspace()->activeWindow() )->editorInterface()->find( incrementalSearch->text(),
									     false, false, true, true );
}

void MainWindow::searchReplace()
{
    if ( !qWorkspace()->activeWindow() ||
	 !::tqt_cast<SourceEditor*>(qWorkspace()->activeWindow()) )
	 return;

    if ( !replaceDialog )
	replaceDialog = new ReplaceDialog( this, 0, false );
    replaceDialog->show();
    replaceDialog->raise();
    replaceDialog->setEditor( ( (SourceEditor*)qWorkspace()->activeWindow() )->editorInterface(),
			   ( (SourceEditor*)qWorkspace()->activeWindow() )->object() );
    replaceDialog->comboFind->setFocus();
    replaceDialog->comboFind->lineEdit()->selectAll();
}

void MainWindow::searchGotoLine()
{
    if ( !qWorkspace()->activeWindow() ||
	 !::tqt_cast<SourceEditor*>(qWorkspace()->activeWindow()) )
	 return;

    if ( !gotoLineDialog )
	gotoLineDialog = new GotoLineDialog( this, 0, false );
    gotoLineDialog->show();
    gotoLineDialog->raise();
    gotoLineDialog->setEditor( ( (SourceEditor*)qWorkspace()->activeWindow() )->editorInterface() );
    gotoLineDialog->spinLine->setFocus();
    gotoLineDialog->spinLine->setMinValue( 1 );
    gotoLineDialog->spinLine->setMaxValue( ( (SourceEditor*)qWorkspace()->activeWindow() )->numLines() );
    gotoLineDialog->spinLine->selectAll();
}

void MainWindow::toolsCustomWidget()
{
    statusBar()->message( tr( "Edit custom widgets..." ) );
    CustomWidgetEditor edit( this, this );
    edit.exec();
    rebuildCustomWidgetGUI();
    statusBar()->clear();
}

void MainWindow::toolsConfigure()
{
    ConfigToolboxDialog dlg( this );
    if ( dlg.exec() != TQDialog::Accepted )
	return;
    rebuildCommonWidgetsToolBoxPage();
}

void MainWindow::showStartDialog()
{
    if ( singleProjectMode() )
	return;
    for ( int i = 1; i < tqApp->argc(); ++i ) {
	TQString arg = tqApp->argv()[i];
	if ( arg[0] != '-' )
	    return;
    }
    StartDialog *sd = new StartDialog( this, templatePath() );
    sd->setRecentlyFiles( recentlyFiles );
    sd->setRecentlyProjects( recentlyProjects );
    sd->exec();
    shStartDialog = sd->showDialogInFuture();
}
