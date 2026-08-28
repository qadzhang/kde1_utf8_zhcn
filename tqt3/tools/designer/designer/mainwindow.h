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

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "metadatabase.h"
#include "../interfaces/actioninterface.h" // for GCC 2.7.* compatibility
#include "../interfaces/editorinterface.h"
#include "../interfaces/templatewizardiface.h"
#include "../interfaces/languageinterface.h"
#include "../interfaces/filterinterface.h"
#include "../interfaces/interpreterinterface.h"
#include "../interfaces/preferenceinterface.h"
#include "../interfaces/projectsettingsiface.h"
#include "../interfaces/sourcetemplateiface.h"
#include "sourceeditor.h"

#include <ntqaction.h>
#include <ntqmainwindow.h>
#include <ntqmap.h>
#include <ntqguardedptr.h>
#include <private/qpluginmanager_p.h>
#include <ntqobjectlist.h>

class TQToolBox;
class PropertyEditor;
class TQWorkspace;
class TQMenuBar;
class FormWindow;
class TQPopupMenu;
class HierarchyView;
class TQCloseEvent;
class Workspace;
class ActionEditor;
class Project;
class OutputWindow;
class TQTimer;
class FindDialog;
struct DesignerProject;
class ReplaceDialog;
class GotoLineDialog;
class SourceFile;
class FormFile;
class TQAssistantClient;
class TQToolBar;
class Preferences;

class MainWindow : public TQMainWindow
{
    TQ_OBJECT

public:
    enum LineMode { Error, Step, StackFrame };

    MainWindow( bool asClient, bool single = false, const TQString &plgDir = "/designer" );
    ~MainWindow();

    HierarchyView *objectHierarchy() const;
    Workspace *workspace() const;
    PropertyEditor *propertyeditor() const;
    ActionEditor *actioneditor() const;

    void resetTool();
    int currentTool() const;

    FormWindow *formWindow();

    bool unregisterClient( FormWindow *w );
    void editorClosed( SourceEditor *e );
    TQWidget *isAFormWindowChild( TQObject *o ) const;
    TQWidget *isAToolBarChild( TQObject *o ) const;

    void insertFormWindow( FormWindow *fw );
    TQWorkspace *qWorkspace() const;

    void popupFormWindowMenu( const TQPoint &gp, FormWindow *fw );
    void popupWidgetMenu( const TQPoint &gp, FormWindow *fw, TQWidget *w );

    TQPopupMenu *setupNormalHierarchyMenu( TQWidget *parent );
    TQPopupMenu *setupTabWidgetHierarchyMenu( TQWidget *parent, const char *addSlot, const char *removeSlot );

    FormWindow *openFormWindow( const TQString &fn, bool validFileName = true, FormFile *ff = 0 );
    bool isCustomWidgetUsed( MetaDataBase::CustomWidget *w );

    void setGrid( const TQPoint &p );
    void setShowGrid( bool b );
    void setSnapGrid( bool b );
    TQPoint grid() const { return grd; }
    bool showGrid() const { return sGrid; }
    bool snapGrid() const { return snGrid && sGrid; }

    TQString documentationPath() const;

    static MainWindow *self;

    TQString templatePath() const { return templPath; }

    void editFunction( const TQString &func, bool rereadSource = false );

    bool isPreviewing() const { return previewing; }

    Project *currProject() const { return currentProject; }

    FormWindow *activeForm() const { return lastActiveFormWindow; }

    TemplateWizardInterface* templateWizardInterface( const TQString& className );
    TQStringList sourceTemplates() const;
    SourceTemplateInterface* sourceTemplateInterface( const TQString& templ );
    TQUnknownInterface* designerInterface() const { return desInterface; }
    TQPtrList<DesignerProject> projectList() const;
    TQStringList projectNames() const;
    TQStringList projectFileNames() const;
    Project *findProject( const TQString &projectName ) const;
    void setCurrentProject( Project *pro );
    void setCurrentProjectByFilename( const TQString& proFilename );
    OutputWindow *outputWindow() const { return oWindow; }
    void addPreferencesTab( TQWidget *tab, const TQString &title, TQObject *receiver, const char *init_slot, const char *accept_slot );
    void addProjectTab( TQWidget *tab, const TQString &title, TQObject *receiver, const char *init_slot, const char *accept_slot );
    void setModified( bool b, TQWidget *window );
    void functionsChanged();
    void updateFunctionList();
    void updateWorkspace();
    void runProjectPrecondition();
    void runProjectPostcondition( TQObjectList *l );

    void formNameChanged( FormWindow *fw );

    int currentLayoutDefaultSpacing() const;
    int currentLayoutDefaultMargin() const;

    void saveAllBreakPoints();
    void resetBreakPoints();

    SourceFile *sourceFile();
    void createNewProject( const TQString &lang );

    void popupProjectMenu( const TQPoint &pos );
    TQObject *findRealObject( TQObject *o );

    void setSingleProject( Project *pro );
    bool singleProjectMode() const { return singleProject; }

    void showSourceLine( TQObject *o, int line, LineMode lm );

    void shuttingDown();
    void showGUIStuff( bool b );
    void setEditorsReadOnly( bool b );
    bool areEditorsReadOnly() const { return editorsReadOnly; }
    void toggleSignalHandlers( bool show );
    bool showSignalHandlers() const { return sSignalHandlers; }
    void writeConfig();

    void openProject( const TQString &fn );
    void setPluginDirectory( const TQString &pd );
    TQString pluginDirectory() const { return pluginDir; }

    TQAssistantClient* assistantClient() const { return assistant; }

    void addRecentlyOpenedFile( const TQString &fn ) { addRecentlyOpened( fn, recentlyFiles ); }

public slots:
    void showProperties( TQObject *w );
    void updateProperties( TQObject *w );
    void showDialogHelp();
    void showDebugStep( TQObject *o, int line );
    void showStackFrame( TQObject *o, int line );
    void showErrorMessage( TQObject *o, int line, const TQString &errorMessage );
    void finishedRun();
    void breakPointsChanged();

signals:
    void currentToolChanged();
    void hasActiveForm( bool );
    void hasActiveWindow( bool );
    void hasActiveWindowOrProject( bool );
    void hasNonDummyProject( bool );
    void formModified( bool );
    void formWindowsChanged();
    void formWindowChanged();
    void projectChanged();
    void editorChanged();

protected:
    bool eventFilter( TQObject *o, TQEvent *e );
    void closeEvent( TQCloseEvent *e );

public slots:
    void fileNew();
    void fileNewDialog();
    void fileNewFile();
    void fileClose();
    void fileQuit();
    void fileCloseProject(); // not visible in menu, called from fileClose
    void fileOpen();
    void fileOpen( const TQString &filter, const TQString &extension, const TQString &filename = "" , bool inProject = true );
    bool fileSave();
    bool fileSaveForm(); // not visible in menu, called from fileSave
    bool fileSaveProject(); // not visible in menu, called from fileSaveProject
    bool fileSaveAs();
    void fileSaveAll();
    void fileCreateTemplate();

public slots:
    void editUndo();
    void editRedo();
    void editCut();
    void editCopy();
    void editPaste();
    void editDelete();
    void editSelectAll();
    void editLower();
    void editRaise();
    void editAdjustSize();
    void editLayoutHorizontal();
    void editLayoutVertical();
    void editLayoutHorizontalSplit();
    void editLayoutVerticalSplit();
    void editLayoutGrid();
    void editLayoutContainerHorizontal();
    void editLayoutContainerVertical();
    void editLayoutContainerGrid();
    void editBreakLayout();
    void editAccels();
    void editFunctions();
    void editConnections();
    SourceEditor *editSource();
    SourceEditor *editSource( SourceFile *f );
    SourceEditor *openSourceEditor();
    SourceEditor *createSourceEditor( TQObject *object, Project *project,
				      const TQString &lang = TQString::null,
				      const TQString &func = TQString::null,
				      bool rereadSource = false );
    void editFormSettings();
    void editProjectSettings();
    void editPixmapCollection();
    void editDatabaseConnections();
    void editPreferences();

    void projectInsertFile();

    void searchFind();
    void searchIncremetalFindMenu();
    void searchIncremetalFind();
    void searchIncremetalFindNext();
    void searchReplace();
    void searchGotoLine();

    void previewForm();
    void previewForm( const TQString& );

    void toolsCustomWidget();
    void toolsConfigure();

    void helpContents();
    void helpManual();
    void helpAbout();
    void helpAboutTQt();
    void helpRegister();

private slots:
    void activeWindowChanged( TQWidget *w );
    void updateUndoRedo( bool, bool, const TQString &, const TQString & );
    void updateEditorUndoRedo();

    void toolSelected( TQAction* );

    void clipboardChanged();
    void selectionChanged();

    void windowsMenuActivated( int id );
    void setupWindowActions();

    void createNewTemplate();
    void projectSelected( TQAction *a );

    void setupRecentlyFilesMenu();
    void setupRecentlyProjectsMenu();
    void recentlyFilesMenuActivated( int id );
    void recentlyProjectsMenuActivated( int id );

    void emitProjectSignals();

    void showStartDialog();

private:
    void setupMDI();
    void setupMenuBar();
    void setupEditActions();
    void setupProjectActions();
    void setupSearchActions();
    void setupToolActions();
    void setupLayoutActions();
    void setupFileActions();
    void setupPreviewActions();
    void setupHelpActions();
    void setupRMBMenus();

    void setupPropertyEditor();
    void setupHierarchyView();
    void setupWorkspace();
    void setupActionEditor();
    void setupOutputWindow();
    void setupToolbox();

    void setupActionManager();
    void setupPluginManagers();

    void enableAll( bool enable );

    TQWidget* previewFormInternal( TQStyle* style = 0, TQPalette* pal = 0 );

    void readConfig();

    void setupRMBProperties( TQValueList<uint> &ids, TQMap<TQString, int> &props, TQWidget *w );
    void handleRMBProperties( int id, TQMap<TQString, int> &props, TQWidget *w );
    void setupRMBSpecialCommands( TQValueList<uint> &ids, TQMap<TQString, int> &commands, TQWidget *w );
    void handleRMBSpecialCommands( int id, TQMap<TQString, int> &commands, TQWidget *w );
    void setupRMBSpecialCommands( TQValueList<uint> &ids, TQMap<TQString, int> &commands, FormWindow *w );
    void handleRMBSpecialCommands( int id, TQMap<TQString, int> &commands, FormWindow *w );
    bool openEditor( TQWidget *w, FormWindow *fw );
    void rebuildCustomWidgetGUI();
    void rebuildCommonWidgetsToolBoxPage();
    void checkTempFiles();

    void addRecentlyOpened( const TQString &fn, TQStringList &lst );

    TQString whatsThisFrom( const TQString &key );

private slots:
    void doFunctionsChanged();
    bool openProjectSettings( Project *pro );

private:
    struct Tab
    {
	TQWidget *w;
	TQString title;
	TQObject *receiver;
	const char *init_slot, *accept_slot;
    };

private:
    PropertyEditor *propertyEditor;
    HierarchyView *hierarchyView;
    Workspace *wspace;
    TQWidget *lastPressWidget;
    TQWorkspace *qworkspace;
    TQMenuBar *menubar;
    TQGuardedPtr<FormWindow> lastActiveFormWindow;
    bool breakLayout, layoutChilds, layoutSelected;
    TQPoint grd;
    bool sGrid, snGrid;
    bool restoreConfig;
    bool backPix;
    bool splashScreen;
    TQString fileFilter;

    TQMap<TQAction*, Project*> projects;
    TQAction *actionEditUndo, *actionEditRedo, *actionEditCut, *actionEditCopy,
    *actionEditPaste, *actionEditDelete,
    *actionEditAdjustSize,
    *actionEditHLayout, *actionEditVLayout, *actionEditGridLayout,
    *actionEditSplitHorizontal, *actionEditSplitVertical,
    *actionEditSelectAll, *actionEditBreakLayout, *actionEditFunctions, *actionEditConnections,
    *actionEditLower, *actionEditRaise, *actionInsertSpacer;
    TQActionGroup *actionGroupTools, *actionGroupProjects;
    TQAction* actionPointerTool, *actionConnectTool, *actionOrderTool, *actionBuddyTool;
    TQAction* actionCurrentTool;
    TQAction *actionHelpContents, *actionHelpAbout, *actionHelpAboutTQt, *actionHelpWhatsThis;
    TQAction *actionHelpManual;
    TQAction *actionToolsCustomWidget, *actionEditPreferences;
    TQAction *actionWindowTile, *actionWindowCascade, *actionWindowClose, *actionWindowCloseAll;
    TQAction *actionWindowNext, *actionWindowPrevious;
    TQAction *actionEditFormSettings, *actionEditAccels;
    TQAction *actionEditSource, *actionNewFile, *actionFileSave, *actionFileExit;
    TQAction *actionSearchFind, *actionSearchIncremetal, *actionSearchReplace, *actionSearchGotoLine;
    TQActionGroup *actionGroupNew;

    TQPopupMenu *rmbWidgets;
    TQPopupMenu *rmbFormWindow;
    TQPopupMenu *customWidgetMenu, *windowMenu, *fileMenu, *recentlyFilesMenu, *recentlyProjectsMenu;
    TQPopupMenu *toolsMenu, *layoutMenu, *previewMenu;
    TQToolBar *customWidgetToolBar, *layoutToolBar, *projectToolBar, *customWidgetToolBar2, *toolsToolBar;
    TQToolBar *commonWidgetsToolBar;
    TQPtrList<TQToolBar> widgetToolBars;

    Preferences *prefDia;
    TQMap<TQString,TQString> propertyDocumentation;
    bool client;
    TQString templPath;
    ActionEditor *actionEditor;
    Project *currentProject;
    TQPluginManager<ActionInterface> *actionPluginManager;
    TQPluginManager<EditorInterface> *editorPluginManager;
    TQPluginManager<TemplateWizardInterface> *templateWizardPluginManager;
    TQPluginManager<InterpreterInterface> *interpreterPluginManager;
    TQPluginManager<PreferenceInterface> *preferencePluginManager;
    TQPluginManager<ProjectSettingsInterface> *projectSettingsPluginManager;
    TQPluginManager<SourceTemplateInterface> *sourceTemplatePluginManager;
    TQPtrList<SourceEditor> sourceEditors;
    bool previewing;
    TQUnknownInterface *desInterface;
    TQStringList recentlyFiles;
    TQStringList recentlyProjects;
    OutputWindow *oWindow;
    TQValueList<Tab> preferenceTabs;
    TQValueList<Tab> projectTabs;
    bool databaseAutoEdit;
    TQTimer *updateFunctionsTimer;
    TQTimer *autoSaveTimer;
    bool autoSaveEnabled;
    int autoSaveInterval;
    TQLineEdit *incrementalSearch;
    TQGuardedPtr<FindDialog> findDialog;
    TQGuardedPtr<ReplaceDialog> replaceDialog;
    TQGuardedPtr<GotoLineDialog> gotoLineDialog;
    Project *eProject;
    bool inDebugMode;
    TQObjectList debuggingForms;
    TQString lastOpenFilter;
    TQGuardedPtr<TQWidget> previewedForm;
    TQPopupMenu *projectMenu;
    TQString menuHelpFile;
    bool singleProject;
    TQToolBox *toolBox;
    int toolsMenuId, toolsMenuIndex;
    uint guiStuffVisible : 1;
    uint editorsReadOnly : 1;
    uint savePluginPaths : 1;
    TQAssistantClient *assistant;
    bool shStartDialog;
    TQString pluginDir;
    bool sSignalHandlers;

public:
    TQString lastSaveFilter;
    TQPtrList<TQAction> toolActions;
    TQPtrList<TQAction> commonWidgetsPage;

};

class SenderObject : public TQObject
{
    TQ_OBJECT

public:
    SenderObject( TQUnknownInterface *i ) : iface( i ) { iface->addRef(); }
    ~SenderObject() { iface->release(); }

public slots:
    void emitInitSignal() { emit initSignal( iface ); }
    void emitAcceptSignal() { emit acceptSignal( iface ); }

signals:
    void initSignal( TQUnknownInterface * );
    void acceptSignal( TQUnknownInterface * );

private:
    TQUnknownInterface *iface;

};

#endif
