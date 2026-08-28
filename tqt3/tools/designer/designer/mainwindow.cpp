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

#include "designerappiface.h"
#include "designerapp.h"

#include "mainwindow.h"
#include "defs.h"
#include "globaldefs.h"
#include "formwindow.h"
#include "widgetdatabase.h"
#include "widgetfactory.h"
#include "propertyeditor.h"
#include "metadatabase.h"
#include "resource.h"
#include "hierarchyview.h"
#include "newformimpl.h"
#include "workspace.h"
#include "about.h"
#include "multilineeditorimpl.h"
#include "wizardeditorimpl.h"
#include "outputwindow.h"
#include "actioneditorimpl.h"
#include "actiondnd.h"
#include "project.h"
#include "projectsettingsimpl.h"
#include "ntqwidgetfactory.h"
#include "pixmapcollection.h"
#include "qcompletionedit.h"
#include "sourcefile.h"
#include "orderindicator.h"
#include <ntqtoolbox.h>
#include "widgetaction.h"
#include "propertyobject.h"
#include "popupmenueditor.h"
#include "menubareditor.h"

#include "startdialog.h"
#include "createtemplate.h"
#include "editfunctions.h"
//#include "connectionviewer.h"
#include "formsettings.h"
#include "preferences.h"
#include "pixmapcollectioneditor.h"
#ifndef TQT_NO_SQL
#include "dbconnectionimpl.h"
#endif
//#include "connectioneditor.h"
#include "customwidgeteditorimpl.h"
#include "paletteeditorimpl.h"
#include "listboxeditorimpl.h"
#include "listvieweditorimpl.h"
#include "iconvieweditorimpl.h"
#include "tableeditorimpl.h"
#include "multilineeditor.h"
#include "finddialog.h"
#include "replacedialog.h"
#include "gotolinedialog.h"

#include <ntqinputdialog.h>
#include <ntqtoolbar.h>
#include <ntqfeatures.h>
#include <ntqmetaobject.h>
#include <ntqaction.h>
#include <ntqpixmap.h>
#include <ntqworkspace.h>
#include <ntqfiledialog.h>
#include <ntqclipboard.h>
#include <ntqmessagebox.h>
#include <ntqbuffer.h>
#include <ntqdir.h>
#include <ntqstyle.h>
#include <ntqlabel.h>
#include <ntqstatusbar.h>
#include <ntqfile.h>
#include <ntqcheckbox.h>
#include <ntqwhatsthis.h>
#include <ntqwizard.h>
#include <ntqtimer.h>
#include <ntqlistbox.h>
#include <ntqdockwindow.h>
#include <ntqstylefactory.h>
#include <ntqvbox.h>
#include <ntqprocess.h>
#include <ntqsettings.h>
#include <ntqaccel.h>
#include <ntqtooltip.h>
#include <ntqsizegrip.h>
#include <ntqtextview.h>
#include <ntqassistantclient.h>
#include <stdlib.h>

static bool mblockNewForms = false;
extern TQMap<TQWidget*, TQString> *qwf_forms;
extern TQString *qwf_language;
extern bool qwf_execute_code;
extern bool qwf_stays_on_top;
extern void set_splash_status( const TQString &txt );
extern TQObject* qwf_form_object;
extern TQString *qwf_plugin_dir;

TQ_EXPORT MainWindow *MainWindow::self = 0;

TQString assistantPath()
{
    TQString path = TQDir::cleanDirPath( TQString( tqInstallPathBins() ) +
				       TQDir::separator() );
    return path;
}


static TQString textNoAccel( const TQString& text)
{
    TQString t = text;
    int i;
    while ( (i = t.find('&') )>= 0 ) {
	t.remove(i,1);
    }
    return t;
}


TQ_EXPORT MainWindow::MainWindow( bool asClient, bool single, const TQString &plgDir )
    : TQMainWindow( 0, "designer_mainwindow", WType_TopLevel | (single ? 0 : WDestructiveClose) | WGroupLeader ),
      grd( 10, 10 ), sGrid( true ), snGrid( true ), restoreConfig( true ), splashScreen( true ),
      fileFilter( tr( "TQt User-Interface Files (*.ui)" ) ), client( asClient ),
      previewing( false ), databaseAutoEdit( false ), autoSaveEnabled( false ), autoSaveInterval( 1800 )
{
    extern void qInitImages_tqtdesignercore();
    qInitImages_tqtdesignercore();

    self = this;
    setPluginDirectory( plgDir );
    customWidgetToolBar = customWidgetToolBar2 = 0;
    guiStuffVisible = true;
    editorsReadOnly = false;
    sSignalHandlers = true;
    init_colors();
    shStartDialog = true;

    desInterface = new DesignerInterfaceImpl( this );
    desInterface->addRef();
    inDebugMode = false;
    savePluginPaths = false;

    updateFunctionsTimer = new TQTimer( this );
    connect( updateFunctionsTimer, TQ_SIGNAL( timeout() ),
	     this, TQ_SLOT( doFunctionsChanged() ) );

    autoSaveTimer = new TQTimer( this );
    connect( autoSaveTimer, TQ_SIGNAL( timeout() ),
	     this, TQ_SLOT( fileSaveAll() ) );

    set_splash_status( "Loading Plugins..." );
    setupPluginManagers();

    if ( !single )
	tqApp->setMainWidget( this );
    TQWidgetFactory::addWidgetFactory( new CustomWidgetFactory );
#ifndef TQ_WS_MACX
    setIcon( TQPixmap::fromMimeSource( "designer_appicon.png" ) );
#endif

    actionGroupTools = 0;
    prefDia = 0;
    windowMenu = 0;
    hierarchyView = 0;
    actionEditor = 0;
    currentProject = 0;
    wspace = 0;
    oWindow = 0;
    singleProject = single;

    statusBar()->clear();
    statusBar()->addWidget( new TQLabel("Ready", statusBar()), 1 );

    set_splash_status( "Setting up GUI..." );
    setupMDI();
    setupMenuBar();

    setupToolbox();

    setupFileActions();
    setupEditActions();
    setupProjectActions();
    setupSearchActions();
    layoutToolBar = new TQToolBar( this, "Layout" );
    layoutToolBar->setCloseMode( TQDockWindow::Undocked );
    addToolBar( layoutToolBar, tr( "Layout" ) );
    interpreterPluginManager = 0;
    setupToolActions();
    setupLayoutActions();
    setupPreviewActions();
    setupOutputWindow();
    setupActionManager();
    setupWindowActions();

    setupWorkspace();
    setupHierarchyView();
    setupPropertyEditor();
    setupActionEditor();

    setupHelpActions();

    setupRMBMenus();

    connect( this, TQ_SIGNAL( projectChanged() ), this, TQ_SLOT( emitProjectSignals() ) );
    connect( this, TQ_SIGNAL( hasActiveWindow(bool) ), this, TQ_SLOT( emitProjectSignals() ) );

    emit hasActiveForm( false );
    emit hasActiveWindow( false );

    lastPressWidget = 0;
    tqApp->installEventFilter( this );

    TQSize as( tqApp->desktop()->size() );
    as -= TQSize( 30, 30 );
    resize( TQSize( 1200, 1000 ).boundedTo( as ) );

    connect( tqApp->clipboard(), TQ_SIGNAL( dataChanged() ),
	     this, TQ_SLOT( clipboardChanged() ) );
    clipboardChanged();
    layoutChilds = false;
    layoutSelected = false;
    breakLayout = false;
    backPix = true;

    set_splash_status( "Loading User Settings..." );
    readConfig();
    // hack to make WidgetFactory happy (so it knows TQWidget and TQDialog for resetting properties)
    TQWidget *w = WidgetFactory::create( WidgetDatabase::idFromClassName( "TQWidget" ), this, 0, false );
    delete w;
    w = WidgetFactory::create( WidgetDatabase::idFromClassName( "TQDialog" ), this, 0, false );
    delete w;
    w = WidgetFactory::create( WidgetDatabase::idFromClassName( "TQLabel" ), this, 0, false );
    delete w;
    w = WidgetFactory::create( WidgetDatabase::idFromClassName( "TQTabWidget" ), this, 0, false );
    delete w;
    w = WidgetFactory::create( WidgetDatabase::idFromClassName( "TQFrame" ), this, 0, false );
    delete w;
    setAppropriate( (TQDockWindow*)actionEditor->parentWidget(), false );
    actionEditor->parentWidget()->hide();

    assistant = new TQAssistantClient( assistantPath(), this );

    statusBar()->setSizeGripEnabled( true );
    set_splash_status( "Initialization Done." );
    if ( shStartDialog )
	TQTimer::singleShot( 0, this, TQ_SLOT( showStartDialog() ));

    if ( autoSaveEnabled )
	autoSaveTimer->start( autoSaveInterval * 1000 );
}

MainWindow::~MainWindow()
{
    TQValueList<Tab>::Iterator tit;
    for ( tit = preferenceTabs.begin(); tit != preferenceTabs.end(); ++tit ) {
	Tab t = *tit;
	delete t.w;
    }
    for ( tit = projectTabs.begin(); tit != projectTabs.end(); ++tit ) {
	Tab t = *tit;
	delete t.w;
    }

    TQMap< TQAction*, Project* >::Iterator it = projects.begin();
    while ( it != projects.end() ) {
	Project *p = *it;
	++it;
	delete p;
    }
    projects.clear();

    delete oWindow;
    oWindow = 0;

    desInterface->release();
    desInterface = 0;

    delete actionPluginManager;
    delete preferencePluginManager;
    delete projectSettingsPluginManager;
    delete interpreterPluginManager;
    delete templateWizardPluginManager;
    delete editorPluginManager;
    delete sourceTemplatePluginManager;

    MetaDataBase::clearDataBase();
    if(self == this)
	self = 0;
}

void MainWindow::setupMDI()
{
    TQVBox *vbox = new TQVBox( this );
    setCentralWidget( vbox );
    vbox->setFrameStyle( TQFrame::StyledPanel | TQFrame::Sunken );
    vbox->setMargin( 1 );
    vbox->setLineWidth( 1 );
    qworkspace = new TQWorkspace( vbox );
    qworkspace->setPaletteBackgroundPixmap( TQPixmap::fromMimeSource( "designer_background.png" ) );
    qworkspace->setScrollBarsEnabled( true );
    connect( qworkspace, TQ_SIGNAL( windowActivated( TQWidget * ) ),
	     this, TQ_SLOT( activeWindowChanged( TQWidget * ) ) );
    lastActiveFormWindow = 0;
    qworkspace->setAcceptDrops( true );
}

void MainWindow::setupMenuBar()
{
    menubar = menuBar();
}

void MainWindow::setupPropertyEditor()
{
    TQDockWindow *dw = new TQDockWindow( TQDockWindow::InDock, this );
    dw->setResizeEnabled( true );
    dw->setCloseMode( TQDockWindow::Always );
    propertyEditor = new PropertyEditor( dw );
    addToolBar( dw, TQt::DockRight );
    dw->setWidget( propertyEditor );
    dw->setFixedExtentWidth( 250 );
    dw->setCaption( tr( "Property Editor/Signal Handlers" ) );
    TQWhatsThis::add( propertyEditor,
		     tr("<b>The Property Editor</b>"
			"<p>You can change the appearance and behavior of the selected widget in the "
			"property editor.</p>"
			"<p>You can set properties for components and forms at design time and see the "
			"immediately see the effects of the changes. "
			"Each property has its own editor which (depending on the property) can be used "
			"to enter "
			"new values, open a special dialog, or to select values from a predefined list. "
			"Click <b>F1</b> to get detailed help for the selected property.</p>"
			"<p>You can resize the columns of the editor by dragging the separators in the "
			"list's header.</p>"
			"<p><b>Signal Handlers</b></p>"
			"<p>In the Signal Handlers tab you can define connections between "
			"the signals emitted by widgets and the slots in the form. "
			"(These connections can also be made using the connection tool.)" ) );
    dw->show();
}

void MainWindow::setupOutputWindow()
{
    TQDockWindow *dw = new TQDockWindow( TQDockWindow::InDock, this );
    dw->setResizeEnabled( true );
    dw->setCloseMode( TQDockWindow::Always );
    addToolBar( dw, TQt::DockBottom );
    oWindow = new OutputWindow( dw );
    dw->setWidget( oWindow );
    dw->setFixedExtentHeight( 150 );
    dw->setCaption( tr( "Output Window" ) );
}

void MainWindow::setupHierarchyView()
{
    if ( hierarchyView )
	return;
    TQDockWindow *dw = new TQDockWindow( TQDockWindow::InDock, this );
    dw->setResizeEnabled( true );
    dw->setCloseMode( TQDockWindow::Always );
    hierarchyView = new HierarchyView( dw );
    addToolBar( dw, TQt::DockRight );
    dw->setWidget( hierarchyView );

    dw->setCaption( tr( "Object Explorer" ) );
    dw->setFixedExtentWidth( 250 );
    TQWhatsThis::add( hierarchyView,
		     tr("<b>The Object Explorer</b>"
			"<p>The Object Explorer provides an overview of the relationships "
			"between the widgets in a form. You can use the clipboard functions using "
			"a context menu for each item in the view. It is also useful for selecting widgets "
			"in forms that have complex layouts.</p>"
			"<p>The columns can be resized by dragging the separator in the list's header.</p>"
			"<p>The second tab shows all the form's slots, class variables, includes, etc.</p>") );
    dw->show();
}

void MainWindow::setupWorkspace()
{
    TQDockWindow *dw = new TQDockWindow( TQDockWindow::InDock, this );
    dw->setResizeEnabled( true );
    dw->setCloseMode( TQDockWindow::Always );
    TQVBox *vbox = new TQVBox( dw );
    TQCompletionEdit *edit = new TQCompletionEdit( vbox );
    TQToolTip::add( edit, tr( "Start typing the buffer you want to switch to here (ALT+B)" ) );
    TQAccel *a = new TQAccel( this );
    a->connectItem( a->insertItem( ALT + Key_B ), edit, TQ_SLOT( setFocus() ) );
    wspace = new Workspace( vbox, this );
    wspace->setBufferEdit( edit );
    wspace->setCurrentProject( currentProject );
    addToolBar( dw, TQt::DockRight );
    dw->setWidget( vbox );

    dw->setCaption( tr( "Project Overview" ) );
    TQWhatsThis::add( wspace, tr("<b>The Project Overview Window</b>"
				"<p>The Project Overview Window displays all the current "
				"project, including forms and source files.</p>"
				"<p>Use the search field to rapidly switch between files.</p>"));
    dw->setFixedExtentHeight( 100 );
    dw->show();
}

void MainWindow::setupActionEditor()
{
    TQDockWindow *dw = new TQDockWindow( TQDockWindow::OutsideDock, this, 0 );
    addDockWindow( dw, TQt::DockTornOff );
    dw->setResizeEnabled( true );
    dw->setCloseMode( TQDockWindow::Always );
    actionEditor = new ActionEditor( dw );
    dw->setWidget( actionEditor );
    actionEditor->show();
    dw->setCaption( tr( "Action Editor" ) );
    TQWhatsThis::add( actionEditor, tr("<b>The Action Editor</b>"
				      "<p>The Action Editor is used to add actions and action groups to "
				      "a form, and to connect actions to slots. Actions and action "
				      "groups can be dragged into menus and into toolbars, and may "
				      "feature keyboard shortcuts and tooltips. If actions have pixmaps "
				      "these are displayed on toolbar buttons and besides their names in "
				      "menus.</p>" ) );
    dw->hide();
    setAppropriate( dw, false );
}

void MainWindow::setupToolbox()
{
    TQDockWindow *dw = new TQDockWindow( TQDockWindow::InDock, this );
    dw->setResizeEnabled( true );
    dw->setCloseMode( TQDockWindow::Always );
    addToolBar( dw, TQt::DockLeft );
    toolBox = new TQToolBox( dw );
    dw->setWidget( toolBox );
    dw->setFixedExtentWidth( 160 );
    dw->setCaption( tr( "Toolbox" ) );
    dw->show();
    setDockEnabled( dw, TQt::DockTop, false );
    setDockEnabled( dw, TQt::DockBottom, false );
    commonWidgetsToolBar = new TQToolBar( "Common Widgets", 0, toolBox, false, "Common Widgets" );
    commonWidgetsToolBar->setFrameStyle( TQFrame::NoFrame );
    commonWidgetsToolBar->setOrientation( TQt::Vertical );
    commonWidgetsToolBar->setBackgroundMode(PaletteBase);
    toolBox->addItem( commonWidgetsToolBar, "Common Widgets" );
}

void MainWindow::setupRMBMenus()
{
    rmbWidgets = new TQPopupMenu( this );
    actionEditCut->addTo( rmbWidgets );
    actionEditCopy->addTo( rmbWidgets );
    actionEditPaste->addTo( rmbWidgets );
    actionEditDelete->addTo( rmbWidgets );
#if 0
    rmbWidgets->insertSeparator();
    actionEditLower->addTo( rmbWidgets );
    actionEditRaise->addTo( rmbWidgets );
#endif
    rmbWidgets->insertSeparator();
    actionEditAdjustSize->addTo( rmbWidgets );
    actionEditHLayout->addTo( rmbWidgets );
    actionEditVLayout->addTo( rmbWidgets );
    actionEditGridLayout->addTo( rmbWidgets );
    actionEditSplitHorizontal->addTo( rmbWidgets );
    actionEditSplitVertical->addTo( rmbWidgets );
    actionEditBreakLayout->addTo( rmbWidgets );
    rmbWidgets->insertSeparator();
    if ( !singleProjectMode() )
	actionEditConnections->addTo( rmbWidgets );
    actionEditSource->addTo( rmbWidgets );

    rmbFormWindow = new TQPopupMenu( this );
    actionEditPaste->addTo( rmbFormWindow );
    actionEditSelectAll->addTo( rmbFormWindow );
    actionEditAccels->addTo( rmbFormWindow );
    rmbFormWindow->insertSeparator();
    actionEditAdjustSize->addTo( rmbFormWindow );
    actionEditHLayout->addTo( rmbFormWindow );
    actionEditVLayout->addTo( rmbFormWindow );
    actionEditGridLayout->addTo( rmbFormWindow );
    actionEditBreakLayout->addTo( rmbFormWindow );
    rmbFormWindow->insertSeparator();
    if ( !singleProjectMode() ) {
	actionEditFunctions->addTo( rmbFormWindow );
	actionEditConnections->addTo( rmbFormWindow );
    }
    actionEditSource->addTo( rmbFormWindow );
    rmbFormWindow->insertSeparator();
    actionEditFormSettings->addTo( rmbFormWindow );
}

void MainWindow::toolSelected( TQAction* action )
{
    actionCurrentTool = action;
    emit currentToolChanged();
    if ( formWindow() )
	formWindow()->commandHistory()->emitUndoRedo();
}

int MainWindow::currentTool() const
{
    if ( !actionCurrentTool )
	return POINTER_TOOL;
    return TQString::fromLatin1(actionCurrentTool->name()).toInt();
}

void MainWindow::runProjectPrecondition()
{
    for ( SourceEditor *e = sourceEditors.first(); e; e = sourceEditors.next() ) {
	e->save();
	e->saveBreakPoints();
    }
    fileSaveProject();

    if ( currentTool() == ORDER_TOOL )
	resetTool();
    oWindow->clearErrorMessages();
    oWindow->clearDebug();
    oWindow->showDebugTab();
    previewing = true;
}

void MainWindow::runProjectPostcondition( TQObjectList *l )
{
    inDebugMode = true;
    debuggingForms = *l;
    enableAll( false );
    for ( SourceEditor *e2 = sourceEditors.first(); e2; e2 = sourceEditors.next() ) {
	if ( e2->project() == currentProject )
	    e2->editorInterface()->setMode( EditorInterface::Debugging );
    }
}

TQWidget* MainWindow::previewFormInternal( TQStyle* style, TQPalette* palet )
{
    qwf_execute_code = false;
    for ( SourceEditor *e = sourceEditors.first(); e; e = sourceEditors.next() )
	e->save();
    if ( currentTool() == ORDER_TOOL )
	resetTool();

    FormWindow *fw = formWindow();
    if ( !fw )
	return 0;

    TQStringList databases;
    TQPtrDictIterator<TQWidget> wit( *fw->widgets() );
    while ( wit.current() ) {
	TQStringList lst = MetaDataBase::fakeProperty( wit.current(), "database" ).toStringList();
	if ( !lst.isEmpty() )
	    databases << lst [ 0 ];
	++wit;
    }

    if ( fw->project() ) {
	TQStringList::ConstIterator it;
	for ( it = databases.begin(); it != databases.end(); ++it )
	    fw->project()->openDatabase( *it, false );
    }
    TQApplication::setOverrideCursor( WaitCursor );

    TQCString s;
    TQBuffer buffer( s );
    buffer.open( IO_WriteOnly );
    Resource resource( this );
    resource.setWidget( fw );
    TQValueList<Resource::Image> images;
    resource.save( &buffer );

    buffer.close();
    buffer.open( IO_ReadOnly );

    TQWidget *w = TQWidgetFactory::create( &buffer );
    if ( w ) {
	previewedForm = w;
	if ( palet ) {
	    if ( style )
		style->polish( *palet );
	    w->setPalette( *palet );
	}

	if ( style )
	    w->setStyle( style );

	TQObjectList *l = w->queryList( "TQWidget" );
	for ( TQObject *o = l->first(); o; o = l->next() ) {
	    if ( style )
		( (TQWidget*)o )->setStyle( style );
	}
	delete l;

	w->move( fw->mapToGlobal( TQPoint(0,0) ) );
	((MainWindow*)w )->setWFlags( WDestructiveClose );
	previewing = true;
	w->show();
	previewing = false;
	TQApplication::restoreOverrideCursor();
	return w;
    }
    TQApplication::restoreOverrideCursor();
    return 0;
}

void MainWindow::previewForm()
{
    TQWidget* w = previewFormInternal();
    if ( w )
	w->show();
}

void MainWindow::previewForm( const TQString & style )
{
    TQStyle* st = TQStyleFactory::create( style );
    TQWidget* w = 0;
    if ( style == "Motif" ) {
	TQPalette p( TQColor( 192, 192, 192 ) );
	w = previewFormInternal( st, &p );
    } else if ( style == "Windows" ) {
	TQPalette p( TQColor( 212, 208, 200 ) );
	w = previewFormInternal( st, &p );
    } else if ( style == "Platinum" ) {
	TQPalette p( TQColor( 220, 220, 220 ) );
	w = previewFormInternal( st, &p );
    } else if ( style == "CDE" ) {
	TQPalette p( TQColor( 75, 123, 130 ) );
	p.setColor( TQPalette::Active, TQColorGroup::Base, TQColor( 55, 77, 78 ) );
	p.setColor( TQPalette::Inactive, TQColorGroup::Base, TQColor( 55, 77, 78 ) );
	p.setColor( TQPalette::Disabled, TQColorGroup::Base, TQColor( 55, 77, 78 ) );
	p.setColor( TQPalette::Active, TQColorGroup::Highlight, TQt::white );
	p.setColor( TQPalette::Active, TQColorGroup::HighlightedText, TQColor( 55, 77, 78 ) );
	p.setColor( TQPalette::Inactive, TQColorGroup::Highlight, TQt::white );
	p.setColor( TQPalette::Inactive, TQColorGroup::HighlightedText, TQColor( 55, 77, 78 ) );
	p.setColor( TQPalette::Disabled, TQColorGroup::Highlight, TQt::white );
	p.setColor( TQPalette::Disabled, TQColorGroup::HighlightedText, TQColor( 55, 77, 78 ) );
	p.setColor( TQPalette::Active, TQColorGroup::Foreground, TQt::white );
	p.setColor( TQPalette::Active, TQColorGroup::Text, TQt::white );
	p.setColor( TQPalette::Active, TQColorGroup::ButtonText, TQt::white );
	p.setColor( TQPalette::Inactive, TQColorGroup::Foreground, TQt::white );
	p.setColor( TQPalette::Inactive, TQColorGroup::Text, TQt::white );
	p.setColor( TQPalette::Inactive, TQColorGroup::ButtonText, TQt::white );
	p.setColor( TQPalette::Disabled, TQColorGroup::Foreground, TQt::lightGray );
	p.setColor( TQPalette::Disabled, TQColorGroup::Text, TQt::lightGray );
	p.setColor( TQPalette::Disabled, TQColorGroup::ButtonText, TQt::lightGray );

	w = previewFormInternal( st, &p );
    } else if ( style == "SGI" ) {
	TQPalette p( TQColor( 220, 220, 220 ) );
	w = previewFormInternal( st, &p );
    } else if ( style == "MotifPlus" ) {
	TQColor gtkfg(0x00, 0x00, 0x00);
	TQColor gtkdf(0x75, 0x75, 0x75);
	TQColor gtksf(0xff, 0xff, 0xff);
	TQColor gtkbs(0xff, 0xff, 0xff);
	TQColor gtkbg(0xd6, 0xd6, 0xd6);
	TQColor gtksl(0x00, 0x00, 0x9c);
	TQColorGroup active(gtkfg,            // foreground
			   gtkbg,            // button
			   gtkbg.light(),    // light
			   gtkbg.dark(142),  // dark
			   gtkbg.dark(110),  // mid
			   gtkfg,            // text
			   gtkfg,            // bright text
			   gtkbs,            // base
			   gtkbg),           // background
	    disabled(gtkdf,            // foreground
		     gtkbg,            // button
		     gtkbg.light(), // light
		     gtkbg.dark(156),  // dark
		     gtkbg.dark(110),  // mid
		     gtkdf,            // text
		     gtkdf,            // bright text
		     gtkbs,            // base
		     gtkbg);           // background

	TQPalette pal(active, disabled, active);

	pal.setColor(TQPalette::Active, TQColorGroup::Highlight,
		     gtksl);
	pal.setColor(TQPalette::Active, TQColorGroup::HighlightedText,
		     gtksf);
	pal.setColor(TQPalette::Inactive, TQColorGroup::Highlight,
		     gtksl);
	pal.setColor(TQPalette::Inactive, TQColorGroup::HighlightedText,
		     gtksf);
	pal.setColor(TQPalette::Disabled, TQColorGroup::Highlight,
		     gtksl);
	pal.setColor(TQPalette::Disabled, TQColorGroup::HighlightedText,
		     gtkdf);
	w = previewFormInternal( st, &pal );
    } else {
	w = previewFormInternal( st );
    }

    if ( !w )
	return;
    w->insertChild( st );
    w->show();
}

void MainWindow::helpContents()
{
    TQWidget *focusWidget = tqApp->focusWidget();
    bool showClassDocu = true;
    while ( focusWidget ) {
	if ( focusWidget->isA( "PropertyList" ) ) {
	    showClassDocu = false;
	    break;
	}
	focusWidget = focusWidget->parentWidget();
    }

    TQString source = "designer-manual.html";
    if ( propertyDocumentation.isEmpty() ) {
	TQString indexFile = documentationPath() + "/propertyindex";
	TQFile f( indexFile );
	if ( f.open( IO_ReadOnly ) ) {
	    TQTextStream ts( &f );
	    while ( !ts.eof() ) {
		TQString s = ts.readLine();
		int from = s.find( "\"" );
		if ( from == -1 )
		    continue;
		int to = s.findRev( "\"" );
		if ( to == -1 )
		    continue;
		propertyDocumentation[ s.mid( from + 1, to - from - 1 ) ] = s.mid( to + 2 ) + "-prop";
	    }
	    f.close();
	}
    }

    if ( propertyEditor->widget() && !showClassDocu ) {
	if ( !propertyEditor->currentProperty().isEmpty() ) {
	    TQMetaObject* mo = propertyEditor->metaObjectOfCurrentProperty();
	    TQString s;
	    TQString cp = propertyEditor->currentProperty();
	    if ( cp == "layoutMargin" ) {
		source = propertyDocumentation[ "TQLayout/margin" ];
	    } else if ( cp == "layoutSpacing" ) {
		source = propertyDocumentation[ "TQLayout/spacing" ];
	    } else if ( cp == "toolTip" ) {
		source = "ntqtooltip.html#details";
	    } else if ( mo && qstrcmp( mo->className(), "Spacer" ) == 0 ) {
		if ( cp != "name" )
		    source = "ntqsizepolicy.html#SizeType";
		else
		    source = propertyDocumentation[ "TQObject/name" ];
	    } else {
		while ( mo && !propertyDocumentation.contains( ( s = TQString( mo->className() ) + "/" + cp ) ) )
		    mo = mo->superClass();
		if ( mo )
		    source = "p:" + propertyDocumentation[s];
	    }
	}

	TQString classname =  WidgetFactory::classNameOf( propertyEditor->widget() );
	if ( source.isEmpty() || source == "designer-manual.html" ) {
	    if ( classname.lower() == "spacer" )
		source = "qspaceritem.html#details";
	    else if ( classname == "TQLayoutWidget" )
		source = "layout.html";
	    else
		source = TQString( WidgetFactory::classNameOf( propertyEditor->widget() ) ).lower() + ".html#details";
	}
    } else if ( propertyEditor->widget() ) {
	source = TQString( WidgetFactory::classNameOf( propertyEditor->widget() ) ).lower() + ".html#details";
    }

    if ( !source.isEmpty() )
	assistant->showPage( documentationPath() + source );
}

void MainWindow::helpManual()
{
    assistant->showPage( documentationPath() + "/designer-manual.html" );
}

void MainWindow::helpAbout()
{
    AboutDialog dlg( this, 0, true );
    if ( singleProjectMode() ) {
	dlg.aboutPixmap->setText( "" );
	dlg.aboutVersion->setText( "" );
	dlg.aboutCopyright->setText( "" );
	LanguageInterface *iface = MetaDataBase::languageInterface( eProject->language() );
	dlg.aboutLicense->setText( iface->aboutText() );
    }
    dlg.aboutVersion->setText(TQString("Version ") + TQString(TQT_VERSION_STR));
    dlg.resize( dlg.width(), dlg.layout()->heightForWidth(dlg.width()) );
    dlg.exec();
}

void MainWindow::helpAboutTQt()
{
    TQMessageBox::aboutTQt( this, "TQt Designer" );
}

#if defined(_WS_WIN_)
#include <qt_windows.h>
#include <ntqprocess.h>
#endif

void MainWindow::helpRegister()
{
#if defined(_WS_WIN_)
    HKEY key;
    HKEY subkey;
    long res;
    DWORD type;
    DWORD size = 255;
    TQString command;
    TQString sub( "htmlfile\\shell" );
#if defined(UNICODE)
    if ( TQApplication::winVersion() & TQt::WV_NT_based ) {
	unsigned char data[256];
	res = RegOpenKeyExW( HKEY_CLASSES_ROOT, NULL, 0, KEY_READ, &key );
	res = RegOpenKeyExW( key, (TCHAR*)sub.ucs2(), 0, KEY_READ, &subkey );
	res = RegQueryValueExW( subkey, NULL, NULL, &type, data, &size );
	command = qt_winTQString( data ) + "\\command";
	size = 255;
	res = RegOpenKeyExW( subkey, (TCHAR*)command.ucs2(), 0, KEY_READ, &subkey );
	res = RegQueryValueExW( subkey, NULL, NULL, &type, data, &size );
	command = qt_winTQString( data );
    } else
#endif
    {
	unsigned char data[256];
	res = RegOpenKeyExA( HKEY_CLASSES_ROOT, NULL, 0, KEY_READ, &key );
	res = RegOpenKeyExA( key, sub.local8Bit(), 0, KEY_READ, &subkey );
	res = RegQueryValueExA( subkey, NULL, NULL, &type, data, &size );
	command = TQString::fromLocal8Bit( (const char*) data ) + "\\command";
	size = 255;
	res = RegOpenKeyExA( subkey, command.local8Bit(), 0, KEY_READ, &subkey );
	res = RegQueryValueExA( subkey, NULL, NULL, &type, data, &size );
	command = TQString::fromLocal8Bit( (const char*) data );
    }

    res = RegCloseKey( subkey );
    res = RegCloseKey( key );

    TQProcess process( command + " www.trolltech.com/products/download/eval/evaluation.html" );
    if ( !process.start() )
	TQMessageBox::information( this, "Register TQt", "Launching your web browser failed.\n"
	"To register TQt, point your browser to www.trolltech.com/products/download/eval/evaluation.html" );
#endif
}

void MainWindow::showProperties( TQObject *o )
{
    if ( !o->isWidgetType() ) {
	propertyEditor->setWidget( o, lastActiveFormWindow );
	if ( lastActiveFormWindow )
	    hierarchyView->setFormWindow( lastActiveFormWindow, lastActiveFormWindow->mainContainer() );
	else
	    hierarchyView->setFormWindow( 0, 0 );
	return;
    }
    TQWidget *w = (TQWidget*)o;
    setupHierarchyView();
    FormWindow *fw = (FormWindow*)isAFormWindowChild( w );
    if ( fw ) {
	if ( fw->numSelectedWidgets() > 1 ) {
	    TQWidgetList wl = fw->selectedWidgets();
	    if ( wl.first() != w ) {
		wl.removeRef( w );
		wl.insert( 0, w );
	    }
	    propertyEditor->setWidget( new PropertyObject( wl ), fw );
	} else {
	    propertyEditor->setWidget( w, fw );
	}
	hierarchyView->setFormWindow( fw, w );
    } else {
	propertyEditor->setWidget( 0, 0 );
	hierarchyView->setFormWindow( 0, 0 );
    }

    if ( currentTool() == POINTER_TOOL && fw &&
	 ( !qworkspace->activeWindow() || !::tqt_cast<SourceEditor*>(qworkspace->activeWindow()) ) )
	fw->setFocus();
}

void MainWindow::resetTool()
{
    actionPointerTool->setOn( true );
}

void MainWindow::updateProperties( TQObject * )
{
    if ( propertyEditor )
	propertyEditor->refetchData();
}

bool MainWindow::eventFilter( TQObject *o, TQEvent *e )
{
    if ( ::tqt_cast<MenuBarEditor*>(o) || ::tqt_cast<PopupMenuEditor*>(o) ||
	 ( o &&
	   ( ::tqt_cast<MenuBarEditor*>(o->parent()) ||
	     ::tqt_cast<PopupMenuEditor*>(o->parent()) ) ) ) {

	if ( e->type() == TQEvent::Accel && ::tqt_cast<PopupMenuEditor*>(o) ) {
	    return true; // consume accel events
 	} else if ( e->type() == TQEvent::MouseButtonPress && ::tqt_cast<MenuBarEditor*>(o) ) {
	    TQPoint pos = ((TQMouseEvent*)e)->pos();
	    MenuBarEditor *m = ::tqt_cast<MenuBarEditor*>(o);
	    showProperties( o );
	    if ( m->findItem( pos ) >= m->count() )
		m->setFocus();
	} else if ( e->type() == TQEvent::MouseButtonPress && ::tqt_cast<PopupMenuEditor*>(o) ) {
	    PopupMenuEditor *m = ::tqt_cast<PopupMenuEditor*>(o);
	    PopupMenuEditorItem *i = m->at( ((TQMouseEvent*)e)->pos() );
	    if ( m->find( i->action() ) != -1 && !i->isSeparator() )
		showProperties( i->action() );
	}
	return TQMainWindow::eventFilter( o, e );
    }

    if ( !o || !e || !o->isWidgetType() )
	return TQMainWindow::eventFilter( o, e );

    TQWidget *w = 0;
    bool passiveInteractor;
    switch ( e->type() ) {
    case TQEvent::AccelOverride:
	if ( ( (TQKeyEvent*)e )->key() == Key_F1 &&
	     ( ( (TQKeyEvent*)e )->state() & ShiftButton ) != ShiftButton ) {
	    w = (TQWidget*)o;
	    while ( w ) {
		if ( ::tqt_cast<PropertyList*>(w) )
		    break;
		w = w->parentWidget( true );
	    }
	    if ( w ) {
		propertyEditor->propertyList()->showCurrentWhatsThis();
		( (TQKeyEvent*)e )->accept();
		return true;
	    }
	}
	break;
    case TQEvent::Accel:
	if ( ( ( (TQKeyEvent*)e )->key() == Key_A ||
	       ( (TQKeyEvent*)e )->key() == Key_E ) &&
	     ( (TQKeyEvent*)e )->state() & ControlButton ) {
	    if ( qWorkspace()->activeWindow() &&
		 ::tqt_cast<SourceEditor*>(qWorkspace()->activeWindow()) ) {
		( (TQKeyEvent*)e )->ignore();
		return true;
	    }
	}
	break;
    case TQEvent::ContextMenu:
    case TQEvent::MouseButtonPress:
	if ( o && currentTool() == POINTER_TOOL &&
	     ( ::tqt_cast<MenuBarEditor*>(o) ||
	       ::tqt_cast<PopupMenuEditor*>(o) ||
	       ::tqt_cast<TQDesignerToolBar*>(o) ||
	       ( ( ::tqt_cast<TQComboBox*>(o) ||
		 ::tqt_cast<TQToolButton*>(o) ||
		 ::tqt_cast<TQDesignerToolBarSeparator*>(o) ) &&
	       o->parent()
	       && ( ::tqt_cast<TQDesignerToolBar*>(o->parent())
		   || ::tqt_cast<TQDesignerWidgetStack*>(o->parent())) ) ) ) {
	    TQWidget *w = (TQWidget*)o;
	    if ( ::tqt_cast<TQToolButton*>(w) ||
		 ::tqt_cast<TQComboBox*>(w) ||
		 ::tqt_cast<PopupMenuEditor*>(w) ||
		 ::tqt_cast<TQDesignerToolBarSeparator*>(w) )
		w = w->parentWidget();
	    TQWidget *pw = w->parentWidget();
	    while ( pw ) {
		if ( ::tqt_cast<FormWindow*>(pw) ) {
		    ( (FormWindow*)pw )->emitShowProperties( w );
		    if ( !::tqt_cast<TQDesignerToolBar*>(o) )
			return ( !::tqt_cast<TQToolButton*>(o) &&
				 !::tqt_cast<MenuBarEditor*>(o) &&
				 !::tqt_cast<TQComboBox*>(o) &&
				 !::tqt_cast<TQDesignerToolBarSeparator*>(o) );
		}
		pw = pw->parentWidget();
	    }
	}
	if ( o && ( ::tqt_cast<TQDesignerToolBar*>(o) || o->inherits("TQDockWindowHandle") )
	     && e->type() == TQEvent::ContextMenu )
	    break;
	if ( isAToolBarChild( o ) && currentTool() != CONNECT_TOOL && currentTool() != BUDDY_TOOL )
	    break;
	if ( ::tqt_cast<TQSizeGrip*>(o) )
	    break;
	if ( !( w = isAFormWindowChild( o ) ) ||
	     ::tqt_cast<SizeHandle*>(o) ||
	     ::tqt_cast<OrderIndicator*>(o) )
	    break;
	if ( !w->hasFocus() )
	    w->setFocus();
	passiveInteractor = WidgetFactory::isPassiveInteractor( o );
	if ( !passiveInteractor || currentTool() != ORDER_TOOL ) {
	    if( e->type() == TQEvent::ContextMenu ) {
		( (FormWindow*)w )->handleContextMenu( (TQContextMenuEvent*)e,
						       ( (FormWindow*)w )->designerWidget( o ) );
		return true;
	    } else {
		( (FormWindow*)w )->handleMousePress( (TQMouseEvent*)e,
						      ( (FormWindow*)w )->designerWidget( o ) );
	    }
	}
	lastPressWidget = (TQWidget*)o;
	if ( passiveInteractor )
	    TQTimer::singleShot( 0, formWindow(), TQ_SLOT( visibilityChanged() ) );
	if ( currentTool() == CONNECT_TOOL || currentTool() == BUDDY_TOOL )
	    return true;
	return !passiveInteractor;
    case TQEvent::MouseButtonRelease:
	lastPressWidget = 0;
	if ( isAToolBarChild( o )  && currentTool() != CONNECT_TOOL && currentTool() != BUDDY_TOOL )
	    break;
	if ( ::tqt_cast<TQSizeGrip*>(o) )
	    break;
	if ( !( w = isAFormWindowChild( o ) ) ||
	     ::tqt_cast<SizeHandle*>(o) ||
	     ::tqt_cast<OrderIndicator*>(o) )
	    break;
	passiveInteractor = WidgetFactory::isPassiveInteractor( o );
	if ( !passiveInteractor )
	    ( (FormWindow*)w )->handleMouseRelease( (TQMouseEvent*)e,
						    ( (FormWindow*)w )->designerWidget( o ) );
	if ( passiveInteractor ) {
	    TQTimer::singleShot( 0, this, TQ_SLOT( selectionChanged() ) );
	    TQTimer::singleShot( 0, formWindow(), TQ_SLOT( visibilityChanged() ) );
	}
	return !passiveInteractor;
    case TQEvent::MouseMove:
	if ( isAToolBarChild( o ) && currentTool() != CONNECT_TOOL && currentTool() != BUDDY_TOOL )
	    break;
	w = isAFormWindowChild( o );
	if ( lastPressWidget != (TQWidget*)o && w &&
	     !::tqt_cast<SizeHandle*>(o) && !::tqt_cast<OrderIndicator*>(o) &&
	     !::tqt_cast<PopupMenuEditor*>(o) && !::tqt_cast<TQMenuBar*>(o) &&
	     !::tqt_cast<TQSizeGrip*>(o) )
	    return true;
	if ( o && ::tqt_cast<TQSizeGrip*>(o) )
	    break;
	if ( lastPressWidget != (TQWidget*)o ||
	     ( !w || ::tqt_cast<SizeHandle*>(o) ) ||
	       ::tqt_cast<OrderIndicator*>(o) )
	    break;
	passiveInteractor = WidgetFactory::isPassiveInteractor( o );
	if ( !passiveInteractor )
	    ( (FormWindow*)w )->handleMouseMove( (TQMouseEvent*)e,
						 ( (FormWindow*)w )->designerWidget( o ) );
	return !passiveInteractor;
    case TQEvent::KeyPress:
	if ( ( (TQKeyEvent*)e )->key() == Key_Escape && currentTool() != POINTER_TOOL ) {
	    resetTool();
	    return false;
	}
	if ( ( (TQKeyEvent*)e )->key() == Key_Escape && incrementalSearch->hasFocus() ) {
	    if ( ::tqt_cast<SourceEditor*>(qWorkspace()->activeWindow()) ) {
		qWorkspace()->activeWindow()->setFocus();
		return true;
	    }
	}
	if ( !( w = isAFormWindowChild( o ) ) ||
	     ::tqt_cast<SizeHandle*>(o) ||
	     ::tqt_cast<OrderIndicator*>(o) )
	    break;
	( (FormWindow*)w )->handleKeyPress( (TQKeyEvent*)e, ( (FormWindow*)w )->designerWidget( o ) );
	if ( ((TQKeyEvent*)e)->isAccepted() )
	    return true;
	break;
    case TQEvent::MouseButtonDblClick:
	if ( !( w = isAFormWindowChild( o ) ) ||
	     ::tqt_cast<SizeHandle*>(o) ||
	     ::tqt_cast<OrderIndicator*>(o) ) {
	    if ( ::tqt_cast<TQToolButton*>(o) && ((TQToolButton*)o)->isOn() &&
		 o->parent() && ::tqt_cast<TQToolBar*>(o->parent()) && formWindow() )
		formWindow()->setToolFixed();
	    break;
	}
	if ( currentTool() == ORDER_TOOL ) {
	    ( (FormWindow*)w )->handleMouseDblClick( (TQMouseEvent*)e,
						     ( (FormWindow*)w )->designerWidget( o ) );
	    return true;
	}
	if ( !WidgetFactory::isPassiveInteractor( o ) && ( (FormWindow*)w )->formFile() )
	    return openEditor( ( (FormWindow*)w )->designerWidget( o ), (FormWindow*)w );
	return true;
    case TQEvent::KeyRelease:
	if ( !( w = isAFormWindowChild( o ) ) ||
	     ::tqt_cast<SizeHandle*>(o) ||
	     ::tqt_cast<OrderIndicator*>(o) )
	    break;
	( (FormWindow*)w )->handleKeyRelease( (TQKeyEvent*)e, ( (FormWindow*)w )->designerWidget( o ) );
	if ( ((TQKeyEvent*)e)->isAccepted() )
	    return true;
	break;
    case TQEvent::Hide:
	if ( !( w = isAFormWindowChild( o ) ) ||
	     ::tqt_cast<SizeHandle*>(o) ||
	     ::tqt_cast<OrderIndicator*>(o) )
	    break;
	if ( ( (FormWindow*)w )->isWidgetSelected( (TQWidget*)o ) )
	    ( (FormWindow*)w )->selectWidget( (TQWidget*)o, false );
	break;
    case TQEvent::Enter:
    case TQEvent::Leave:
	if ( !( w = isAFormWindowChild( o ) ) ||
	     ::tqt_cast<SizeHandle*>(o) ||
	     ::tqt_cast<OrderIndicator*>(o) ||
	     ::tqt_cast<MenuBarEditor*>(o) )
	    break;
	return true;
    case TQEvent::Resize:
    case TQEvent::Move:
	if ( !( w = isAFormWindowChild( o ) ) ||
	     ::tqt_cast<SizeHandle*>(o) ||
	     ::tqt_cast<OrderIndicator*>(o) )
	    break;
	if ( WidgetFactory::layoutType( (TQWidget*)o->parent() ) != WidgetFactory::NoLayout ) {
	    ( (FormWindow*)w )->updateSelection( (TQWidget*)o );
	    if ( e->type() != TQEvent::Resize )
		( (FormWindow*)w )->updateChildSelections( (TQWidget*)o );
	}
	break;
    case TQEvent::Close:
	if ( o->isWidgetType() && (TQWidget*)o == (TQWidget*)previewedForm ) {
	    if ( lastActiveFormWindow && lastActiveFormWindow->project() ) {
		TQStringList lst =
		    MetaDataBase::fakeProperty( lastActiveFormWindow, "database" ).toStringList();
		lastActiveFormWindow->project()->closeDatabase( lst[ 0 ] );
	    }
	}
	break;
    case TQEvent::DragEnter:
	if ( o == qWorkspace() || o == workspace() || o == workspace()->viewport() ) {
	    workspace()->contentsDragEnterEvent( (TQDragEnterEvent*)e );
	    return true;
	}
	break;
    case TQEvent::DragMove:
	if ( o == qWorkspace() || o == workspace() || o == workspace()->viewport() ) {
	    workspace()->contentsDragMoveEvent( (TQDragMoveEvent*)e );
	    return true;
	}
	break;
    case TQEvent::Drop:
	if ( o == qWorkspace() || o == workspace() || o == workspace()->viewport() ) {
	    workspace()->contentsDropEvent( (TQDropEvent*)e );
	    return true;
	}
	break;
    case TQEvent::Show:
	if ( o != this )
	    break;
	if ( ((TQShowEvent*)e)->spontaneous() )
	    break;
	TQApplication::sendPostedEvents( qworkspace, TQEvent::ChildInserted );
	showEvent( (TQShowEvent*)e );
	checkTempFiles();
	return true;
    case TQEvent::Wheel:
	if ( !( w = isAFormWindowChild( o ) ) ||
	     ::tqt_cast<SizeHandle*>(o) ||
	     ::tqt_cast<OrderIndicator*>(o) )
	    break;
	return true;
    case TQEvent::FocusIn:
	if ( !::tqt_cast<FormWindow*>(o) && isAFormWindowChild( o ) )
	    return true; //FIXME
	if ( hierarchyView->formDefinitionView()->isRenaming()
	     && ( o->inherits( "Editor" ) || ::tqt_cast<FormWindow*>(o) ) )
		TQApplication::sendPostedEvents();
	if ( o->inherits( "Editor" ) ) {
	    TQWidget *w = (TQWidget*)o;
	    while ( w ) {
		if ( ::tqt_cast<SourceEditor*>(w) )
		    break;
		w = w->parentWidget( true );
	    }
	    if ( ::tqt_cast<SourceEditor*>(w) )
		( (SourceEditor*)w )->checkTimeStamp();
	} else if ( ::tqt_cast<FormWindow*>(o) ) {
	    FormWindow *fw = (FormWindow*)o;
	    if ( fw->formFile() && fw->formFile()->editor() )
		fw->formFile()->editor()->checkTimeStamp();
	}
	break;
    case TQEvent::FocusOut:
	if ( !::tqt_cast<FormWindow*>(o) && isAFormWindowChild( o ) )
	    return true;
	break;
    default:
	return TQMainWindow::eventFilter( o, e );
    }

    return TQMainWindow::eventFilter( o, e );
}

TQWidget *MainWindow::isAFormWindowChild( TQObject *o ) const
{
    if ( ::tqt_cast<TQWizard*>(o->parent()) && !::tqt_cast<TQPushButton*>(o) )
	return 0;
    while ( o ) {
	if ( ::tqt_cast<FormWindow*>(o) )
	    return (TQWidget*)o;
	o = o->parent();
    }
    return 0;
}

TQWidget *MainWindow::isAToolBarChild( TQObject *o ) const
{
    while ( o ) {
	if ( ::tqt_cast<TQDesignerToolBar*>(o) )
	    return (TQWidget*)o;
	if ( ::tqt_cast<FormWindow*>(o) )
	    return 0;
	o = o->parent();
    }
    return 0;
}

FormWindow *MainWindow::formWindow()
{
    if ( qworkspace->activeWindow() ) {
	FormWindow *fw = 0;
	if ( ::tqt_cast<FormWindow*>(qworkspace->activeWindow()) )
	    fw = (FormWindow*)qworkspace->activeWindow();
	else if ( lastActiveFormWindow &&
		    qworkspace->windowList().find( lastActiveFormWindow ) != -1)
	    fw = lastActiveFormWindow;
	return fw;
    }
    return 0;
}

void MainWindow::emitProjectSignals()
{
    emit hasNonDummyProject( !currentProject->isDummy() );
    emit hasActiveWindowOrProject( !!qworkspace->activeWindow() || !currentProject->isDummy() );
}

void MainWindow::insertFormWindow( FormWindow *fw )
{
    if ( fw )
	TQWhatsThis::add( fw, tr( "<b>The Form Window</b>"
			       "<p>Use the various tools to add widgets or to change the layout "
			       "and behavior of the components in the form. Select one or multiple "
			       "widgets to move them or lay them out. If a single widget is chosen it can "
			       "be resized using the resize handles.</p>"
			       "<p>Changes in the <b>Property Editor</b> are visible at design time, "
			       "and you can preview the form in different styles.</p>"
			       "<p>You can change the grid resolution, or turn the grid off in the "
			       "<b>Preferences</b> dialog in the <b>Edit</b> menu."
			       "<p>You can have several forms open, and all open forms are listed "
			       "in the <b>Form List</b>.") );

    connect( fw, TQ_SIGNAL( showProperties( TQObject * ) ),
	     this, TQ_SLOT( showProperties( TQObject * ) ) );
    connect( fw, TQ_SIGNAL( updateProperties( TQObject * ) ),
	     this, TQ_SLOT( updateProperties( TQObject * ) ) );
    connect( this, TQ_SIGNAL( currentToolChanged() ),
	     fw, TQ_SLOT( currentToolChanged() ) );
    connect( fw, TQ_SIGNAL( selectionChanged() ),
	     this, TQ_SLOT( selectionChanged() ) );
    connect( fw, TQ_SIGNAL( undoRedoChanged( bool, bool, const TQString &, const TQString & ) ),
	     this, TQ_SLOT( updateUndoRedo( bool, bool, const TQString &, const TQString & ) ) );

    if ( !mblockNewForms ) {
    } else {
	fw->setProject( currentProject );
    }
    fw->show();
    fw->currentToolChanged();
    if ( fw->caption().isEmpty() && tqstrlen( fw->name() )  )
	fw->setCaption( fw->name() );
    fw->mainContainer()->setCaption( fw->caption() );
    WidgetFactory::saveDefaultProperties( fw->mainContainer(),
					  WidgetDatabase::
					  idFromClassName( WidgetFactory::classNameOf( fw->mainContainer() ) ) );
    activeWindowChanged( fw );
    emit formWindowsChanged();
    for ( SourceEditor *e = sourceEditors.first(); e; e = sourceEditors.next() ) {
	if ( e->project() == fw->project() )
	    e->resetContext();
    }
}

void MainWindow::createNewProject( const TQString &lang )
{
    Project *pro = new Project( "", "", projectSettingsPluginManager );
    pro->setLanguage( lang );


    if ( !openProjectSettings( pro ) ) {
	delete pro;
	return;
    }

    if ( !pro->isValid() ) {
	TQMessageBox::information( this, tr("New Project"), tr( "Cannot create an invalid project." ) );
	delete pro;
	return;
    }

    TQAction *a = new TQAction( pro->makeRelative( pro->fileName() ),
			      pro->makeRelative( pro->fileName() ), 0, actionGroupProjects, 0, true );
    projects.insert( a, pro );
    addRecentlyOpened( pro->makeAbsolute( pro->fileName() ), recentlyProjects );
    projectSelected( a );
}


bool MainWindow::unregisterClient( FormWindow *w )
{
    propertyEditor->closed( w );
    objectHierarchy()->closed( w );
    if ( w == lastActiveFormWindow )
	lastActiveFormWindow = 0;

    TQPtrList<SourceEditor> waitingForDelete;
    waitingForDelete.setAutoDelete( true );
    for ( SourceEditor *e = sourceEditors.first(); e; e = sourceEditors.next() ) {
	if ( e->object() == w )
	    waitingForDelete.append( e );
    }

    if ( actionEditor->form() == w ) {
	actionEditor->setFormWindow( 0 );
	actionEditor->parentWidget()->hide();
    }

    return true;
}

void MainWindow::activeWindowChanged( TQWidget *w )
{
    TQWidget *old = formWindow();
    if ( ::tqt_cast<FormWindow*>(w) ) {
	FormWindow *fw = (FormWindow*)w;
	FormWindow *ofw = lastActiveFormWindow;
	lastActiveFormWindow = fw;
	lastActiveFormWindow->updateUndoInfo();
	emit hasActiveForm( true );
	if ( formWindow() ) {
	    formWindow()->emitShowProperties();
	    emit formModified( formWindow()->commandHistory()->isModified() );
	    if ( currentTool() != POINTER_TOOL )
		formWindow()->clearSelection();
	}
	workspace()->activeFormChanged( fw );
	setAppropriate( (TQDockWindow*)actionEditor->parentWidget(),
			::tqt_cast<TQMainWindow*>(lastActiveFormWindow->mainContainer()) );
	if ( appropriate( (TQDockWindow*)actionEditor->parentWidget() ) ) {
	    if ( actionEditor->wantToBeShown() )
		actionEditor->parentWidget()->show();
	} else {
	    TQWidget *mc = 0;
	    if ( ofw && (mc = ofw->mainContainer()) && ::tqt_cast<TQMainWindow*>(mc) )
		actionEditor->setWantToBeShown( !actionEditor->parentWidget()->isHidden() );
	    actionEditor->parentWidget()->hide();
	}

	actionEditor->setFormWindow( lastActiveFormWindow );
	if ( wspace && fw->project() && fw->project() != currentProject ) {
	    for ( TQMap<TQAction*, Project *>::ConstIterator it = projects.begin(); it != projects.end(); ++it ) {
		if ( *it == fw->project() ) {
		    projectSelected( it.key() );
		    break;
		}
	    }
	}
	emit formWindowChanged();

    } else if ( w == propertyEditor ) {
	propertyEditor->resetFocus();
    } else if ( !lastActiveFormWindow ) {
	emit formWindowChanged();
	emit hasActiveForm( false );
	actionEditUndo->setEnabled( false );
	actionEditRedo->setEnabled( false );
    }

    if ( !w ) {
	emit formWindowChanged();
	emit hasActiveForm( false );
	propertyEditor->clear();
	hierarchyView->clear();
	updateUndoRedo( false, false, TQString::null, TQString::null );
    }

    selectionChanged();

    if ( ::tqt_cast<SourceEditor*>(w) ) {
	SourceEditor *se = (SourceEditor*)w;
	TQGuardedPtr<FormWindow> fw = se->formWindow();
	if ( se->formWindow() &&
	     lastActiveFormWindow != fw ) {
	    activeWindowChanged( se->formWindow() );
	}
	actionSearchFind->setEnabled( true );
	actionSearchIncremetal->setEnabled( true );
	actionSearchReplace->setEnabled( true );
	actionSearchGotoLine->setEnabled( true );
	incrementalSearch->setEnabled( true );

	actionEditUndo->setEnabled( false );
	actionEditRedo->setEnabled( false );
	actionEditCut->setEnabled( true );
	actionEditCopy->setEnabled( true );
        // actionEditPaste will be updated by calling clipboardChanged()
	actionEditSelectAll->setEnabled( true );
	actionEditUndo->setMenuText( tr( "&Undo" ) );
	actionEditUndo->setToolTip( textNoAccel( actionEditUndo->menuText()) );
	actionEditRedo->setMenuText( tr( "&Redo" ) );
	actionEditRedo->setToolTip( textNoAccel( actionEditRedo->menuText()) );
	if ( hierarchyView->sourceEditor() != w )
	    hierarchyView->showClasses( se );
	actionEditor->setFormWindow( 0 );
	if ( wspace && se->project() && se->project() != currentProject ) {
	    for ( TQMap<TQAction*, Project *>::ConstIterator it = projects.begin(); it != projects.end(); ++it ) {
		if ( *it == se->project() ) {
		    projectSelected( it.key() );
		    break;
		}
	    }
	}
	workspace()->activeEditorChanged( se );
    } else {
	actionSearchFind->setEnabled( false );
	actionSearchIncremetal->setEnabled( false );
	actionSearchReplace->setEnabled( false );
	actionSearchGotoLine->setEnabled( false );
	incrementalSearch->setEnabled( false );
    }

    clipboardChanged(); // update actionEditPaste

    if ( currentTool() == ORDER_TOOL && w != old )
	emit currentToolChanged();

    emit hasActiveWindow( !!qworkspace->activeWindow() );
}

void MainWindow::updateUndoRedo( bool undoAvailable, bool redoAvailable,
				 const TQString &undoCmd, const TQString &redoCmd )
{
    if ( qWorkspace()->activeWindow() &&
	 ::tqt_cast<SourceEditor*>(qWorkspace()->activeWindow()) )
	 return; // do not set a formWindow related command
    actionEditUndo->setEnabled( undoAvailable );
    actionEditRedo->setEnabled( redoAvailable );
    if ( !undoCmd.isEmpty() )
	actionEditUndo->setMenuText( tr( "&Undo: %1" ).arg( undoCmd ) );
    else
	actionEditUndo->setMenuText( tr( "&Undo: Not Available" ) );
    if ( !redoCmd.isEmpty() )
	actionEditRedo->setMenuText( tr( "&Redo: %1" ).arg( redoCmd ) );
    else
	actionEditRedo->setMenuText( tr( "&Redo: Not Available" ) );

    actionEditUndo->setToolTip( textNoAccel( actionEditUndo->menuText()) );
    actionEditRedo->setToolTip( textNoAccel( actionEditRedo->menuText()) );

    if ( currentTool() == ORDER_TOOL ) {
	actionEditUndo->setEnabled( false );
	actionEditRedo->setEnabled( false );
    }
}

void MainWindow::updateEditorUndoRedo()
{
    if ( !qWorkspace()->activeWindow() ||
	 !::tqt_cast<SourceEditor*>(qWorkspace()->activeWindow()) )
	return;
    SourceEditor *editor = (SourceEditor*)qWorkspace()->activeWindow();
    actionEditUndo->setEnabled( editor->editIsUndoAvailable() );
    actionEditRedo->setEnabled( editor->editIsRedoAvailable() );
}

TQWorkspace *MainWindow::qWorkspace() const
{
    return qworkspace;
}

void MainWindow::popupFormWindowMenu( const TQPoint & gp, FormWindow *fw )
{
    TQValueList<uint> ids;
    TQMap<TQString, int> commands;

    setupRMBSpecialCommands( ids, commands, fw );
    setupRMBProperties( ids, commands, fw );

    tqApp->processEvents();
    int r = rmbFormWindow->exec( gp );

    handleRMBProperties( r, commands, fw );
    handleRMBSpecialCommands( r, commands, fw );

    for ( TQValueList<uint>::ConstIterator i = ids.begin(); i != ids.end(); ++i )
	rmbFormWindow->removeItem( *i );
}

void MainWindow::popupWidgetMenu( const TQPoint &gp, FormWindow * /*fw*/, TQWidget * w)
{
    TQValueList<uint> ids;
    TQMap<TQString, int> commands;

    setupRMBSpecialCommands( ids, commands, w );
    setupRMBProperties( ids, commands, w );

    tqApp->processEvents();
    int r = rmbWidgets->exec( gp );

    handleRMBProperties( r, commands, w );
    handleRMBSpecialCommands( r, commands, w );

    for ( TQValueList<uint>::ConstIterator i = ids.begin(); i != ids.end(); ++i )
	rmbWidgets->removeItem( *i );
}

void MainWindow::setupRMBProperties( TQValueList<uint> &ids, TQMap<TQString, int> &props, TQWidget *w )
{
    const TQMetaProperty* text = w->metaObject()->property( w->metaObject()->findProperty( "text", true ), true );
    if ( text && qstrcmp( text->type(), "TQString") != 0 )
	text = 0;
    const TQMetaProperty* title = w->metaObject()->property( w->metaObject()->findProperty( "title", true ), true );
    if ( title && qstrcmp( title->type(), "TQString") != 0 )
	title = 0;
    const TQMetaProperty* pagetitle =
	w->metaObject()->property( w->metaObject()->findProperty( "pageTitle", true ), true );
    if ( pagetitle && qstrcmp( pagetitle->type(), "TQString") != 0 )
	pagetitle = 0;
    const TQMetaProperty* pixmap =
	w->metaObject()->property( w->metaObject()->findProperty( "pixmap", true ), true );
    if ( pixmap && qstrcmp( pixmap->type(), "TQPixmap") != 0 )
	pixmap = 0;

    if ( ( text && text->designable(w) ) ||
	 ( title && title->designable(w) ) ||
	 ( pagetitle && pagetitle->designable(w) ) ||
	 ( pixmap && pixmap->designable(w) ) ) {
	int id = 0;
	if ( ids.isEmpty() )
	    ids << rmbWidgets->insertSeparator(0);
	if ( pixmap && pixmap->designable(w) ) {
	    ids << ( id = rmbWidgets->insertItem( tr("Choose Pixmap..."), -1, 0) );
	    props.insert( "pixmap", id );
	}
	if ( text && text->designable(w) && !::tqt_cast<TQTextEdit*>(w) ) {
	    ids << ( id = rmbWidgets->insertItem( tr("Edit Text..."), -1, 0) );
	    props.insert( "text", id );
	}
	if ( title && title->designable(w) ) {
	    ids << ( id = rmbWidgets->insertItem( tr("Edit Title..."), -1, 0) );
	    props.insert( "title", id );
	}
	if ( pagetitle && pagetitle->designable(w) ) {
	    ids << ( id = rmbWidgets->insertItem( tr("Edit Page Title..."), -1, 0) );
	    props.insert( "pagetitle", id );
	}
    }
}

#ifdef QT_CONTAINER_CUSTOM_WIDGETS
static TQWidgetContainerInterfacePrivate *containerWidgetInterface( TQWidget *w )
{
    WidgetInterface *iface = 0;
    widgetManager()->queryInterface( WidgetFactory::classNameOf( w ), &iface );
    if ( !iface )
	return 0;
    TQWidgetContainerInterfacePrivate *iface2 = 0;
    iface->queryInterface( IID_QWidgetContainer, (TQUnknownInterface**)&iface2 );
    if ( !iface2 )
	return 0;
    iface->release();
    return iface2;
}
#endif

void MainWindow::setupRMBSpecialCommands( TQValueList<uint> &ids,
					  TQMap<TQString, int> &commands, TQWidget *w )
{
    int id;

    if ( ::tqt_cast<TQTabWidget*>(w) ) {
	if ( ids.isEmpty() )
	    ids << rmbWidgets->insertSeparator( 0 );
	if ( ( (TQDesignerTabWidget*)w )->count() > 1) {
	    ids << ( id = rmbWidgets->insertItem( tr("Delete Page"), -1, 0 ) );
	    commands.insert( "remove", id );
	}
	ids << ( id = rmbWidgets->insertItem( tr("Add Page"), -1, 0 ) );
	commands.insert( "add", id );
    } else if ( ::tqt_cast<TQDesignerWidgetStack*>(w) ) {
	if ( ids.isEmpty() )
	    ids << rmbWidgets->insertSeparator( 0 );
	if ( ( (TQDesignerWidgetStack*)w )->count() > 1) {
	    ids << ( id = rmbWidgets->insertItem( tr("Previous Page"), -1, 0 ) );
	    commands.insert( "prevpage", id );
	    ids << ( id = rmbWidgets->insertItem( tr("Next Page"), -1, 0 ) );
	    ids << rmbWidgets->insertSeparator( 0 );
	    commands.insert( "nextpage", id );
	    ids << ( id = rmbWidgets->insertItem( tr("Delete Page"), -1, 0 ) );
	    commands.insert( "remove", id );
	}
	ids << ( id = rmbWidgets->insertItem( tr("Add Page"), -1, 0 ) );
	commands.insert( "add", id );
    } else if ( ::tqt_cast<TQToolBox*>(w) ) {
	if ( ids.isEmpty() )
	    ids << rmbWidgets->insertSeparator( 0 );
	if ( ( (TQToolBox*)w )->count() > 1 ) {
	    ids << ( id = rmbWidgets->insertItem( tr("Delete Page"), -1, 0 ) );
	    commands.insert( "remove", id );
	}
	ids << ( id = rmbWidgets->insertItem( tr("Add Page"), -1, 0 ) );
	commands.insert( "add", id );
#ifdef QT_CONTAINER_CUSTOM_WIDGETS
    } else if ( WidgetDatabase::
		isCustomPluginWidget( WidgetDatabase::
				      idFromClassName( WidgetFactory::classNameOf( w ) ) ) ) {
	TQWidgetContainerInterfacePrivate *iface = containerWidgetInterface( w );
	if ( iface && iface->supportsPages( WidgetFactory::classNameOf( w ) ) ) {
	    if ( ids.isEmpty() )
		ids << rmbWidgets->insertSeparator( 0 );

	    if ( iface->count( WidgetFactory::classNameOf( w ), w ) > 1 ) {
		ids << ( id = rmbWidgets->insertItem( tr( "Delete Page" ), -1, 0 ) );
		commands.insert( "remove", id );
	    }
	    ids << ( id = rmbWidgets->insertItem( tr("Add Page"), -1, 0 ) );
	    commands.insert( "add", id );
	    if ( iface->currentIndex( WidgetFactory::classNameOf( w ), w ) != -1 ) {
		ids << ( id = rmbWidgets->insertItem( tr("Rename Current Page..."), -1, 0 ) );
		commands.insert( "rename", id );
	    }
	}
	if ( iface )
	    iface->release();
#endif // QT_CONTAINER_CUSTOM_WIDGETS
    }

    if ( WidgetFactory::hasSpecialEditor( WidgetDatabase::
			  idFromClassName( WidgetFactory::classNameOf( w ) ), w ) ) {
	if ( ids.isEmpty() )
	    ids << rmbWidgets->insertSeparator( 0 );
	ids << ( id = rmbWidgets->insertItem( tr("Edit..."), -1, 0 ) );
	commands.insert( "edit", id );
    }
}

void MainWindow::setupRMBSpecialCommands( TQValueList<uint> &ids,
					  TQMap<TQString, int> &commands, FormWindow *fw )
{
    int id;

    if ( ::tqt_cast<TQWizard*>(fw->mainContainer()) ) {
	if ( ids.isEmpty() )
	    ids << rmbFormWindow->insertSeparator( 0 );

	if ( ( (TQWizard*)fw->mainContainer() )->pageCount() > 1) {
	    ids << ( id = rmbFormWindow->insertItem( tr("Delete Page"), -1, 0 ) );
	    commands.insert( "remove", id );
	}

	ids << ( id = rmbFormWindow->insertItem( tr("Add Page"), -1, 0 ) );
	commands.insert( "add", id );

	ids << ( id = rmbFormWindow->insertItem( tr("Edit Page Title..."), -1, 0 ) );
	commands.insert( "rename", id );

	ids << ( id = rmbFormWindow->insertItem( tr("Edit Pages..."), -1, 0 ) );
	commands.insert( "edit", id );

    } else if ( ::tqt_cast<TQMainWindow*>(fw->mainContainer()) ) {
	if ( ids.isEmpty() )
	    ids << rmbFormWindow->insertSeparator( 0 );
	ids << ( id = rmbFormWindow->insertItem( tr( "Add Menu Item" ), -1, 0 ) );
	commands.insert( "add_menu_item", id );
	ids << ( id = rmbFormWindow->insertItem( tr( "Add Toolbar" ), -1, 0 ) );
	commands.insert( "add_toolbar", id );
    }
}

void MainWindow::handleRMBProperties( int id, TQMap<TQString, int> &props, TQWidget *w )
{
    if ( id == props[ "text" ] ) {
	bool ok = false;
	bool oldDoWrap = false;
	if ( ::tqt_cast<TQLabel*>(w) ) {
	    int align = w->property( "alignment" ).toInt();
	    if ( align & WordBreak )
		oldDoWrap = true;
	}
	bool doWrap = oldDoWrap;

	TQString text;
	if ( ::tqt_cast<TQTextView*>(w) || ::tqt_cast<TQLabel*>(w) || ::tqt_cast<TQButton*>(w) ) {
	    text = MultiLineEditor::getText( this, w->property( "text" ).toString(), !::tqt_cast<TQButton*>(w), &doWrap );
	    ok = !text.isNull();
	} else {
	    text = TQInputDialog::getText( tr("Text"), tr( "New text" ),
				  TQLineEdit::Normal, w->property("text").toString(), &ok, this );
	}
	if ( ok ) {
	    if ( oldDoWrap != doWrap ) {
		TQString pn( tr( "Set 'wordwrap' of '%1'" ).arg( w->name() ) );
		SetPropertyCommand *cmd = new SetPropertyCommand( pn, formWindow(), w, propertyEditor,
								  "wordwrap", TQVariant( oldDoWrap ),
								  TQVariant( doWrap ), TQString::null, TQString::null );
		cmd->execute();
		formWindow()->commandHistory()->addCommand( cmd );
		MetaDataBase::setPropertyChanged( w, "wordwrap", true );
	    }

	    TQString pn( tr( "Set the 'text' of '%1'" ).arg( w->name() ) );
	    SetPropertyCommand *cmd = new SetPropertyCommand( pn, formWindow(), w, propertyEditor,
							      "text", w->property( "text" ),
							      text, TQString::null, TQString::null );
	    cmd->execute();
	    formWindow()->commandHistory()->addCommand( cmd );
	    MetaDataBase::setPropertyChanged( w, "text", true );
	}
    } else if ( id == props[ "title" ] ) {
	bool ok = false;
	TQString title = TQInputDialog::getText( tr("Title"), tr( "New title" ),
			       TQLineEdit::Normal, w->property("title").toString(), &ok, this );
	if ( ok ) {
	    TQString pn( tr( "Set the 'title' of '%2'" ).arg( w->name() ) );
	    SetPropertyCommand *cmd = new SetPropertyCommand( pn, formWindow(), w, propertyEditor,
							      "title", w->property( "title" ),
							      title, TQString::null, TQString::null );
	    cmd->execute();
	    formWindow()->commandHistory()->addCommand( cmd );
	    MetaDataBase::setPropertyChanged( w, "title", true );
	}
    } else if ( id == props[ "pagetitle" ] ) {
	bool ok = false;
	TQString text = TQInputDialog::getText( tr("Page Title"), tr( "New page title" ),
			      TQLineEdit::Normal, w->property("pageTitle").toString(), &ok, this );
	if ( ok ) {
	    TQString pn( tr( "Set the 'pageTitle' of '%2'" ).arg( w->name() ) );
	    SetPropertyCommand *cmd = new SetPropertyCommand( pn, formWindow(), w, propertyEditor,
							      "pageTitle",
							      w->property( "pageTitle" ),
							      text, TQString::null, TQString::null );
	    cmd->execute();
	    formWindow()->commandHistory()->addCommand( cmd );
	    MetaDataBase::setPropertyChanged( w, "pageTitle", true );
	}
    } else if ( id == props[ "pixmap" ] ) {
	TQPixmap oldPix = w->property( "pixmap" ).toPixmap();
	TQPixmap pix = qChoosePixmap( this, formWindow(), oldPix );
	if ( !pix.isNull() ) {
	    TQString pn( tr( "Set the 'pixmap' of '%2'" ).arg( w->name() ) );
	    SetPropertyCommand *cmd = new SetPropertyCommand( pn, formWindow(), w, propertyEditor,
							      "pixmap", w->property( "pixmap" ),
							      pix, TQString::null, TQString::null );
	    cmd->execute();
	    formWindow()->commandHistory()->addCommand( cmd );
	    MetaDataBase::setPropertyChanged( w, "pixmap", true );
	}
    }
}

void MainWindow::handleRMBSpecialCommands( int id, TQMap<TQString, int> &commands, TQWidget *w )
{
    if ( ::tqt_cast<TQTabWidget*>(w) ) {
	TQTabWidget *tw = (TQTabWidget*)w;
	if ( id == commands[ "add" ] ) {
	    AddTabPageCommand *cmd =
		new AddTabPageCommand( tr( "Add Page to %1" ).arg( tw->name() ), formWindow(),
				       tw, "Tab" );
	    formWindow()->commandHistory()->addCommand( cmd );
	    cmd->execute();
	} else if ( id == commands[ "remove" ] ) {
	    if ( tw->currentPage() ) {
		TQDesignerTabWidget *dtw = (TQDesignerTabWidget*)tw;
		DeleteTabPageCommand *cmd =
		    new DeleteTabPageCommand( tr( "Delete Page %1 of %2" ).
					      arg( dtw->pageTitle() ).arg( tw->name() ),
					      formWindow(), tw, tw->currentPage() );
		formWindow()->commandHistory()->addCommand( cmd );
		cmd->execute();
	    }
	}
    } else if ( ::tqt_cast<TQToolBox*>(w) ) {
	TQToolBox *tb = (TQToolBox*)w;
	if ( id == commands[ "add" ] ) {
	    AddToolBoxPageCommand *cmd =
		new AddToolBoxPageCommand( tr( "Add Page to %1" ).arg( tb->name() ),
					   formWindow(),
					   tb, "Page" );
	    formWindow()->commandHistory()->addCommand( cmd );
	    cmd->execute();
	} else if ( id == commands[ "remove" ] ) {
	    if ( tb->currentItem() ) {
		DeleteToolBoxPageCommand *cmd =
		    new DeleteToolBoxPageCommand( tr( "Delete Page %1 of %2" ).
					  arg( tb->itemLabel( tb->currentIndex() ) ).
					  arg( tb->name() ),
					  formWindow(), tb, tb->currentItem() );
		formWindow()->commandHistory()->addCommand( cmd );
		cmd->execute();
	    }
	}
    } else if ( ::tqt_cast<TQWidgetStack*>(w) ) {
	TQDesignerWidgetStack *ws = (TQDesignerWidgetStack*)w;
	if ( id == commands[ "add" ] ) {
	    AddWidgetStackPageCommand *cmd =
		new AddWidgetStackPageCommand( tr( "Add Page to %1" ).arg( ws->name() ),
					       formWindow(), ws );
	    formWindow()->commandHistory()->addCommand( cmd );
	    cmd->execute();
	} else if ( id == commands[ "remove" ] ) {
	    if ( ws->visibleWidget() ) {
		DeleteWidgetStackPageCommand *cmd =
		    new DeleteWidgetStackPageCommand( tr( "Delete Page %1 of %2" ).
						      arg( ws->currentPage() ).arg( ws->name() ),
						      formWindow(), ws, ws->visibleWidget() );
		formWindow()->commandHistory()->addCommand( cmd );
		cmd->execute();
	    }
	} else if ( id == commands[ "nextpage" ] ) {
	    int currentPage = w->property( "currentPage" ).toInt();
	    TQString pn( tr( "Raise next page of '%2'" ).arg( w->name() ) );
	    SetPropertyCommand *cmd =
		new SetPropertyCommand( pn, formWindow(), w, propertyEditor,
					"currentPage", currentPage,
					currentPage + 1, TQString::null, TQString::null );
	    cmd->execute();
	    formWindow()->commandHistory()->addCommand( cmd );
	    MetaDataBase::setPropertyChanged( w, "currentPage", true );
	} else if ( id == commands[ "prevpage" ] ) {
	    int currentPage = w->property( "currentPage" ).toInt();
	    TQString pn( tr( "Raise previous page of '%2'" ).arg( w->name() ) );
	    SetPropertyCommand *cmd =
		new SetPropertyCommand( pn, formWindow(), w, propertyEditor,
					"currentPage", currentPage,
					currentPage -1, TQString::null, TQString::null );
	    cmd->execute();
	    formWindow()->commandHistory()->addCommand( cmd );
	    MetaDataBase::setPropertyChanged( w, "currentPage", true );
	}
#ifdef QT_CONTAINER_CUSTOM_WIDGETS
    } else if ( WidgetDatabase::
		isCustomPluginWidget( WidgetDatabase::
				      idFromClassName( WidgetFactory::classNameOf( w ) ) ) ) {
	if ( id == commands[ "add" ] ) {
	    AddContainerPageCommand *cmd =
		new AddContainerPageCommand( tr( "Add Page to %1" ).arg( w->name() ),
					     formWindow(), w, "Page" );
	    formWindow()->commandHistory()->addCommand( cmd );
	    cmd->execute();
	} else if ( id == commands[ "remove" ] ) {
	    TQWidgetContainerInterfacePrivate *iface = containerWidgetInterface( w );
	    if ( iface ) {
		TQString wClassName = WidgetFactory::classNameOf( w );
		int index = iface->currentIndex( wClassName, w );
		DeleteContainerPageCommand *cmd =
		    new DeleteContainerPageCommand( tr( "Delete Page %1 of %2" ).
						    arg( iface->pageLabel( wClassName,
									   w, index ) ).
						    arg( w->name() ),
						    formWindow(), w, index );
		formWindow()->commandHistory()->addCommand( cmd );
		cmd->execute();
		iface->release();
	    }
	} else if ( id == commands[ "rename" ] ) {
	    TQWidgetContainerInterfacePrivate *iface = containerWidgetInterface( w );
	    if ( iface ) {
		bool ok = false;
		TQString wClassName = WidgetFactory::classNameOf( w );
		int index = iface->currentIndex( wClassName, w );
		TQString text = TQInputDialog::getText( tr("Page Title"), tr( "New page title" ),
						      TQLineEdit::Normal,
						      iface->pageLabel( wClassName, w,
								       index ),
						      &ok, this );
		if ( ok ) {
		    RenameContainerPageCommand *cmd =
			new RenameContainerPageCommand( tr( "Rename Page %1 to %2" ).
							arg( iface->
							     pageLabel( wClassName, w,
									index ) ).
							arg( text ), formWindow(),
							w, index, text );
		    formWindow()->commandHistory()->addCommand( cmd );
		    cmd->execute();
		}
		iface->release();
	    }
	}
#endif // QT_CONTAINER_CUSTOM_WIDGETS
    }

    if ( WidgetFactory::hasSpecialEditor( WidgetDatabase::
				  idFromClassName( WidgetFactory::classNameOf( w ) ), w ) ) {
	if ( id == commands[ "edit" ] )
	    WidgetFactory::editWidget( WidgetDatabase::
		       idFromClassName( WidgetFactory::classNameOf( w ) ), this, w, formWindow() );
    }
}

void MainWindow::handleRMBSpecialCommands( int id, TQMap<TQString, int> &commands, FormWindow *fw )
{
    if ( ::tqt_cast<TQWizard*>(fw->mainContainer()) ) {
	TQWizard *wiz = (TQWizard*)fw->mainContainer();
	if ( id == commands[ "add" ] ) {
	    AddWizardPageCommand *cmd =
		new AddWizardPageCommand( tr( "Add Page to %1" ).arg( wiz->name() ),
					  formWindow(), wiz, "WizardPage" );
	    formWindow()->commandHistory()->addCommand( cmd );
	    cmd->execute();
	} else if ( id == commands[ "remove" ] ) {
	    if ( wiz->currentPage() ) {
		TQDesignerWizard *dw = (TQDesignerWizard*)wiz;
		DeleteWizardPageCommand *cmd =
		    new DeleteWizardPageCommand( tr( "Delete Page %1 of %2" ).
						 arg( dw->pageTitle() ).arg( wiz->name() ),
						 formWindow(), wiz,
						 wiz->indexOf( wiz->currentPage() ) );
		formWindow()->commandHistory()->addCommand( cmd );
		cmd->execute();
	    }
	} else if ( id == commands[ "edit" ] ) {
	    WizardEditor *e = new WizardEditor( this, wiz, fw );
	    e->exec();
	    delete e;
	} else if ( id == commands[ "rename" ] ) {

	    bool ok = false;
	    TQDesignerWizard *dw = (TQDesignerWizard*)wiz;
	    TQString text = TQInputDialog::getText( tr("Page Title"), tr( "New page title" ),
					  TQLineEdit::Normal, dw->pageTitle(), &ok, this );
	    if ( ok ) {
		TQString pn( tr( "Rename page %1 of %2" ).arg( dw->pageTitle() ).arg( wiz->name() ) );
		RenameWizardPageCommand *cmd =
		    new RenameWizardPageCommand( pn, formWindow()
						 , wiz, wiz->indexOf( wiz->currentPage() ), text );
		formWindow()->commandHistory()->addCommand( cmd );
		cmd->execute();
	    }
	}
    } else if ( ::tqt_cast<TQMainWindow*>(fw->mainContainer()) ) {
	TQMainWindow *mw = (TQMainWindow*)fw->mainContainer();
	if ( id == commands[ "add_toolbar" ] ) {
	    AddToolBarCommand *cmd =
		new AddToolBarCommand( tr( "Add Toolbar to '%1'" ).arg( formWindow()->name() ),
				       formWindow(), mw );
	    formWindow()->commandHistory()->addCommand( cmd );
	    cmd->execute();
	} else if ( id == commands[ "add_menu_item" ] ) {
	    AddMenuCommand *cmd =
		new AddMenuCommand( tr( "Add Menu to '%1'" ).arg( formWindow()->name() ),
				    formWindow(), mw );
	    formWindow()->commandHistory()->addCommand( cmd );
	    cmd->execute();
	}
    }
}

void MainWindow::clipboardChanged()
{
    TQString text( tqApp->clipboard()->text() );
    if (qWorkspace()->activeWindow() && ::tqt_cast<SourceEditor*>(qWorkspace()->activeWindow())) {
        actionEditPaste->setEnabled(!text.isEmpty());
    } else {
        TQString start( "<!DOCTYPE UI-SELECTION>" );
        actionEditPaste->setEnabled( text.left( start.length() ) == start );
    }
}

void MainWindow::selectionChanged()
{
    layoutChilds = false;
    layoutSelected = false;
    breakLayout = false;
    if ( !formWindow() ) {
	actionEditCut->setEnabled( false );
	actionEditCopy->setEnabled( false );
	actionEditDelete->setEnabled( false );
	actionEditAdjustSize->setEnabled( false );
	actionEditHLayout->setEnabled( false );
	actionEditVLayout->setEnabled( false );
	actionEditSplitHorizontal->setEnabled( false );
	actionEditSplitVertical->setEnabled( false );
	actionEditGridLayout->setEnabled( false );
	actionEditBreakLayout->setEnabled( false );
	actionEditLower->setEnabled( false );
	actionEditRaise->setEnabled( false );
	actionEditAdjustSize->setEnabled( false );
	return;
    }

    int selectedWidgets = formWindow()->numSelectedWidgets();
    bool enable = selectedWidgets > 0;
    actionEditCut->setEnabled( enable );
    actionEditCopy->setEnabled( enable );
    actionEditDelete->setEnabled( enable );
    actionEditLower->setEnabled( enable );
    actionEditRaise->setEnabled( enable );

    actionEditAdjustSize->setEnabled( false );
    actionEditSplitHorizontal->setEnabled( false );
    actionEditSplitVertical->setEnabled( false );

    enable = false;
    TQWidgetList widgets = formWindow()->selectedWidgets();
    if ( selectedWidgets > 1 ) {
	int unlaidout = 0;
	int laidout = 0;
	for ( TQWidget *w = widgets.first(); w; w = widgets.next() ) {
	    if ( !w->parentWidget() || WidgetFactory::layoutType( w->parentWidget() ) == WidgetFactory::NoLayout )
		unlaidout++;
	    else
		laidout++;
	}
	actionEditHLayout->setEnabled( unlaidout > 1 );
	actionEditVLayout->setEnabled( unlaidout > 1 );
	actionEditSplitHorizontal->setEnabled( unlaidout > 1 );
	actionEditSplitVertical->setEnabled( unlaidout > 1 );
	actionEditGridLayout->setEnabled( unlaidout > 1 );
	actionEditBreakLayout->setEnabled( laidout > 0 );
	actionEditAdjustSize->setEnabled( laidout > 0 );
	layoutSelected = unlaidout > 1;
	breakLayout = laidout > 0;
    } else if ( selectedWidgets == 1 ) {
	TQWidget *w = widgets.first();
	bool isContainer = WidgetDatabase::isContainer( WidgetDatabase::idFromClassName( WidgetFactory::classNameOf( w ) ) ) ||
			   w == formWindow()->mainContainer();
	actionEditAdjustSize->setEnabled( !w->parentWidget() ||
					  WidgetFactory::layoutType( w->parentWidget() ) == WidgetFactory::NoLayout );

	if ( !isContainer ) {
	    actionEditHLayout->setEnabled( false );
	    actionEditVLayout->setEnabled( false );
	    actionEditGridLayout->setEnabled( false );
	    if ( w->parentWidget() && WidgetFactory::layoutType( w->parentWidget() ) != WidgetFactory::NoLayout ) {
		actionEditBreakLayout->setEnabled( !isAToolBarChild( w ) );
		breakLayout = true;
	    } else {
		actionEditBreakLayout->setEnabled( false );
	    }
	} else {
	    if ( WidgetFactory::layoutType( w ) == WidgetFactory::NoLayout ) {
		if ( !formWindow()->hasInsertedChildren( w ) ) {
		    actionEditHLayout->setEnabled( false );
		    actionEditVLayout->setEnabled( false );
		    actionEditGridLayout->setEnabled( false );
		    actionEditBreakLayout->setEnabled( false );
		} else {
		    actionEditHLayout->setEnabled( true );
		    actionEditVLayout->setEnabled( true );
		    actionEditGridLayout->setEnabled( true );
		    actionEditBreakLayout->setEnabled( false );
		    layoutChilds = true;
		}
		if ( w->parentWidget() && WidgetFactory::layoutType( w->parentWidget() ) != WidgetFactory::NoLayout ) {
		    actionEditBreakLayout->setEnabled( !isAToolBarChild( w ) );
		    breakLayout = true;
		}
	    } else {
		actionEditHLayout->setEnabled( false );
		actionEditVLayout->setEnabled( false );
		actionEditGridLayout->setEnabled( false );
		actionEditBreakLayout->setEnabled( !isAToolBarChild( w ) );
		breakLayout = true;
	    }
	}
    } else if ( selectedWidgets == 0 ) {
	actionEditAdjustSize->setEnabled( true );
	TQWidget *w = formWindow()->mainContainer();
	if ( WidgetFactory::layoutType( w ) == WidgetFactory::NoLayout ) {
	    if ( !formWindow()->hasInsertedChildren( w ) ) {
		actionEditHLayout->setEnabled( false );
		actionEditVLayout->setEnabled( false );
		actionEditGridLayout->setEnabled( false );
		actionEditBreakLayout->setEnabled( false );
	    } else {
		actionEditHLayout->setEnabled( true );
		actionEditVLayout->setEnabled( true );
		actionEditGridLayout->setEnabled( true );
		actionEditBreakLayout->setEnabled( false );
		layoutChilds = true;
	    }
	} else {
	    actionEditHLayout->setEnabled( false );
	    actionEditVLayout->setEnabled( false );
	    actionEditGridLayout->setEnabled( false );
	    actionEditBreakLayout->setEnabled( true );
	    breakLayout = true;
	}
    } else {
	actionEditHLayout->setEnabled( false );
	actionEditVLayout->setEnabled( false );
	actionEditGridLayout->setEnabled( false );
	actionEditBreakLayout->setEnabled( false );
    }
}

static TQString fixArgs( const TQString &s2 )
{
    TQString s = s2;
    return s.replace( ',', ';' );
}

void MainWindow::writeConfig()
{
    TQSettings config;

    // No search path for unix, only needs application name
    config.insertSearchPath( TQSettings::Windows, "/Trolltech" );

    TQString keybase = DesignerApplication::settingsKey();

    if (savePluginPaths) {
	TQStringList pluginPaths = TQApplication::libraryPaths();
	config.writeEntry( keybase + "PluginPaths", pluginPaths );
    }
    config.writeEntry( keybase + "RestoreWorkspace", restoreConfig );
    config.writeEntry( keybase + "SplashScreen", splashScreen );
    config.writeEntry( keybase + "ShowStartDialog", shStartDialog );
    config.writeEntry( keybase + "FileFilter", fileFilter );
    config.writeEntry( keybase + "TemplatePath", templPath );
    config.writeEntry( keybase + "RecentlyOpenedFiles", recentlyFiles );
    config.writeEntry( keybase + "RecentlyOpenedProjects", recentlyProjects );
    config.writeEntry( keybase + "DatabaseAutoEdit", databaseAutoEdit );

    config.writeEntry( keybase + "AutoSave/Enabled", autoSaveEnabled );
    config.writeEntry( keybase + "AutoSave/Interval", autoSaveInterval );

    config.writeEntry( keybase + "Grid/Snap", snGrid );
    config.writeEntry( keybase + "Grid/Show", sGrid );
    config.writeEntry( keybase + "Grid/x", grid().x() );
    config.writeEntry( keybase + "Grid/y", grid().y() );
    config.writeEntry( keybase + "LastToolPage",
		       toolBox->itemLabel( toolBox->currentIndex() ) );

    config.writeEntry( keybase + "Background/UsePixmap", backPix );
    config.writeEntry( keybase + "Background/Color", (int)qworkspace->backgroundColor().rgb() );
    if ( qworkspace->backgroundPixmap() )
	qworkspace->backgroundPixmap()->save( TQDir::home().absPath() + "/.designer/" + "background.xpm", "PNG" );

    config.writeEntry( keybase + "Geometries/MainwindowX", x() );
    config.writeEntry( keybase + "Geometries/MainwindowY", y() );
    config.writeEntry( keybase + "Geometries/MainwindowWidth", width() );
    config.writeEntry( keybase + "Geometries/MainwindowHeight", height() );
    config.writeEntry( keybase + "Geometries/MainwindowMaximized", isMaximized() );
    config.writeEntry( keybase + "Geometries/PropertyEditorX", propertyEditor->parentWidget()->x() );
    config.writeEntry( keybase + "Geometries/PropertyEditorY", propertyEditor->parentWidget()->y() );
    config.writeEntry( keybase + "Geometries/PropertyEditorWidth", propertyEditor->parentWidget()->width() );
    config.writeEntry( keybase + "Geometries/PropertyEditorHeight", propertyEditor->parentWidget()->height() );
    config.writeEntry( keybase + "Geometries/HierarchyViewX", hierarchyView->parentWidget()->x() );
    config.writeEntry( keybase + "Geometries/HierarchyViewY", hierarchyView->parentWidget()->y() );
    config.writeEntry( keybase + "Geometries/HierarchyViewWidth", hierarchyView->parentWidget()->width() );
    config.writeEntry( keybase + "Geometries/HierarchyViewHeight", hierarchyView->parentWidget()->height() );
    config.writeEntry( keybase + "Geometries/WorkspaceX", wspace->parentWidget()->x() );
    config.writeEntry( keybase + "Geometries/WorkspaceY", wspace->parentWidget()->y() );
    config.writeEntry( keybase + "Geometries/WorkspaceWidth", wspace->parentWidget()->width() );
    config.writeEntry( keybase + "Geometries/WorkspaceHeight", wspace->parentWidget()->height() );

    config.writeEntry( keybase + "View/TextLabels", usesTextLabel() );
    config.writeEntry( keybase + "View/BigIcons", usesBigPixmaps() );

    TQString mwlKey = "MainwindowLayout";
    if ( singleProjectMode() )
	mwlKey += "S";
    TQString mainWindowLayout;
    TQTextStream ts( &mainWindowLayout, IO_WriteOnly );
    ts << *this;
    config.writeEntry( keybase + mwlKey, mainWindowLayout );


    TQPtrList<MetaDataBase::CustomWidget> *lst = MetaDataBase::customWidgets();
    config.writeEntry( keybase + "CustomWidgets/num", (int)lst->count() );
    int j = 0;
    TQDir::home().mkdir( ".designer" );
    for ( MetaDataBase::CustomWidget *w = lst->first(); w; w = lst->next() ) {
	TQStringList l;
	l << w->className;
	l << w->includeFile;
	l << TQString::number( (int)w->includePolicy );
	l << TQString::number( w->sizeHint.width() );
	l << TQString::number( w->sizeHint.height() );
	l << TQString::number( w->lstSignals.count() );
	for ( TQValueList<TQCString>::ConstIterator it = w->lstSignals.begin(); it != w->lstSignals.end(); ++it )
	    l << TQString( fixArgs( *it ) );
	l << TQString::number( w->lstSlots.count() );
	for ( TQValueList<MetaDataBase::Function>::ConstIterator it2 = w->lstSlots.begin(); it2 != w->lstSlots.end(); ++it2 ) {
	    l << fixArgs( (*it2).function );
	    l << (*it2).access;
	}
	l << TQString::number( w->lstProperties.count() );
	for ( TQValueList<MetaDataBase::Property>::ConstIterator it3 = w->lstProperties.begin(); it3 != w->lstProperties.end(); ++it3 ) {
	    l << (*it3).property;
	    l << (*it3).type;
	}
	l << TQString::number( size_type_to_int( w->sizePolicy.horData() ) );
	l << TQString::number( size_type_to_int( w->sizePolicy.verData() ) );
	l << TQString::number( (int)w->isContainer );
	config.writeEntry( keybase + "CustomWidgets/Widget" + TQString::number( j++ ), l, ',' );
	w->pixmap->save( TQDir::home().absPath() + "/.designer/" + w->className, "XPM" );
    }

    TQStringList l;
    for ( TQAction *a = commonWidgetsPage.first(); a; a = commonWidgetsPage.next() )
	l << a->text();
    config.writeEntry( keybase + "ToolBox/CommonWidgets", l );
}

static TQString fixArgs2( const TQString &s2 )
{
    TQString s = s2;
    return s.replace( ';', ',' );
}

void MainWindow::readConfig()
{
    TQString keybase = DesignerApplication::settingsKey();
    TQSettings config;
    config.insertSearchPath( TQSettings::Windows, "/Trolltech" );

    bool ok;
    bool readPreviousConfig = false;
    TQString backPixName( TQDir::home().absPath() + "/.designer/" + "background.xpm" );
    restoreConfig = config.readBoolEntry( keybase + "RestoreWorkspace", true, &ok );
    if ( !ok ) {
	keybase = DesignerApplication::oldSettingsKey();
	restoreConfig = config.readBoolEntry( keybase + "RestoreWorkspace", true, &ok );
	if ( !ok ) {
	    if ( oWindow ) {
		oWindow->shuttingDown();
		( (TQDockWindow*)oWindow->parent() )->hide();
	    }
	    TQPixmap pix;
	    pix.load( backPixName );
	    if ( !pix.isNull() )
		qworkspace->setBackgroundPixmap( pix );
	    return;
	}
	readPreviousConfig = true;
    }
    if ( !readPreviousConfig ) {
	fileFilter = config.readEntry( keybase + "FileFilter", fileFilter );
	templPath = config.readEntry( keybase + "TemplatePath", TQString::null );
	databaseAutoEdit = config.readBoolEntry( keybase + "DatabaseAutoEdit", databaseAutoEdit );
	shStartDialog = config.readBoolEntry( keybase + "ShowStartDialog", shStartDialog );
	autoSaveEnabled = config.readBoolEntry( keybase + "AutoSave/Enabled", autoSaveEnabled );
	autoSaveInterval = config.readNumEntry( keybase + "AutoSave/Interval", autoSaveInterval );
    }

    if ( restoreConfig || readPreviousConfig ) {
	TQString s = config.readEntry( keybase + "LastToolPage" );
	for ( int i = 0; i < toolBox->count(); ++i ) {
	    if ( toolBox->itemLabel(i) == s ) {
		toolBox->setCurrentIndex( i );
		break;
	    }
	}
	// We know that the oldSettingsKey() will return 3.1
	if ( keybase == DesignerApplication::oldSettingsKey() ) {
	    if (keybase.contains("3.1"))
		recentlyFiles = config.readListEntry( keybase + "RecentlyOpenedFiles", ',' );
	    else
		recentlyFiles = config.readListEntry(keybase + "RecentlyOpenedFiles");

	    if ( recentlyFiles.count() == 1 && recentlyFiles[0].isNull() )
		recentlyFiles.clear();
	    if (keybase.contains("3.1"))
		recentlyProjects = config.readListEntry( keybase + "RecentlyOpenedProjects", ',' );
	    else
		recentlyProjects = config.readListEntry( keybase + "RecentlyOpenedProjects");
	    if ( recentlyProjects.count() == 1 && recentlyProjects[0].isNull() )
		recentlyProjects.clear();
	} else {
	    recentlyFiles = config.readListEntry( keybase + "RecentlyOpenedFiles" );
	    recentlyProjects = config.readListEntry( keybase + "RecentlyOpenedProjects" );
	}

	backPix = config.readBoolEntry( keybase + "Background/UsePixmap", true ) | readPreviousConfig;
	if ( backPix ) {
	    TQPixmap pix;
	    pix.load( backPixName );
	    if ( !pix.isNull() )
		qworkspace->setBackgroundPixmap( pix );
	} else {
	    qworkspace->setBackgroundColor( TQColor( (TQRgb)config.readNumEntry( keybase + "Background/Color" ) ) );
	}

	if ( !readPreviousConfig ) {
	    splashScreen = config.readBoolEntry( keybase + "SplashScreen", true );

	    sGrid = config.readBoolEntry( keybase + "Grid/Show", true );
	    snGrid = config.readBoolEntry( keybase + "Grid/Snap", true );
	    grd.setX( config.readNumEntry( keybase + "Grid/x", 10 ) );
	    grd.setY( config.readNumEntry( keybase + "Grid/y", 10 ) );

	    if ( !config.readBoolEntry( DesignerApplication::settingsKey() + "Geometries/MainwindowMaximized", false ) ) {
		TQRect r( pos(), size() );
		r.setX( config.readNumEntry( keybase + "Geometries/MainwindowX", r.x() ) );
		r.setY( config.readNumEntry( keybase + "Geometries/MainwindowY", r.y() ) );
		r.setWidth( config.readNumEntry( keybase + "Geometries/MainwindowWidth", r.width() ) );
		r.setHeight( config.readNumEntry( keybase + "Geometries/MainwindowHeight", r.height() ) );

		TQRect desk = TQApplication::desktop()->geometry();
		TQRect inter = desk.intersect( r );
		resize( r.size() );
		if ( inter.width() * inter.height() > ( r.width() * r.height() / 20 ) ) {
		    move( r.topLeft() );
		}
	    }
	    setUsesTextLabel( config.readBoolEntry( keybase + "View/TextLabels", false ) );
	    setUsesBigPixmaps( false /*config.readBoolEntry( "BigIcons", false )*/ ); // ### disabled for now
	}
    }
    int num = config.readNumEntry( keybase + "CustomWidgets/num" );
    for ( int j = 0; j < num; ++j ) {
	MetaDataBase::CustomWidget *w = new MetaDataBase::CustomWidget;
	TQStringList l = config.readListEntry( keybase + "CustomWidgets/Widget" + TQString::number( j ), ',' );
	w->className = l[ 0 ];
	w->includeFile = l[ 1 ];
	w->includePolicy = (MetaDataBase::CustomWidget::IncludePolicy)l[ 2 ].toInt();
	w->sizeHint.setWidth( l[ 3 ].toInt() );
	w->sizeHint.setHeight( l[ 4 ].toInt() );
	uint c = 5;
	if ( l.count() > c ) {
	    int numSignals = l[ c ].toInt();
	    c++;
	    for ( int i = 0; i < numSignals; ++i, c++ )
		w->lstSignals.append( fixArgs2( l[ c ] ).latin1() );
	}
	if ( l.count() > c ) {
	    int numSlots = l[ c ].toInt();
	    c++;
	    for ( int i = 0; i < numSlots; ++i ) {
		MetaDataBase::Function slot;
		slot.function = fixArgs2( l[ c ] );
		c++;
		slot.access = l[ c ];
		c++;
		w->lstSlots.append( slot );
	    }
	}
	if ( l.count() > c ) {
	    int numProperties = l[ c ].toInt();
	    c++;
	    for ( int i = 0; i < numProperties; ++i ) {
		MetaDataBase::Property prop;
		prop.property = l[ c ];
		c++;
		prop.type = l[ c ];
		c++;
		w->lstProperties.append( prop );
	    }
	} if ( l.count() > c ) {
	    TQSizePolicy::SizeType h, v;
	     h = int_to_size_type( l[ c++ ].toInt() );
	     v = int_to_size_type( l[ c++ ].toInt() );
	     w->sizePolicy = TQSizePolicy( h, v );
	}
	if ( l.count() > c ) {
	    w->isContainer = (bool)l[ c++ ].toInt();
	}
	w->pixmap = new TQPixmap( TQPixmap::fromMimeSource( TQDir::home().absPath() + "/.designer/" + w->className ) );
	MetaDataBase::addCustomWidget( w );
    }
    if ( num > 0 )
	rebuildCustomWidgetGUI();

    if ( !restoreConfig )
	return;

#ifndef TQ_WS_MAC
    /* I'm sorry to make this not happen on the Mac, but it seems to hang somewhere deep
       in TQLayout, it gets into a very large loop - and seems it has to do with clever
       things the designer does ###Sam */
    TQApplication::sendPostedEvents();
#endif

    if ( !readPreviousConfig ) {
	TQString mwlKey = "MainwindowLayout";
	if ( singleProjectMode() )
	    mwlKey += "S";
	TQString mainWindowLayout = config.readEntry( keybase + mwlKey );
	TQTextStream ts( &mainWindowLayout, IO_ReadOnly );
	ts >> *this;
    }
    if ( readPreviousConfig && oWindow ) {
	oWindow->shuttingDown();
	( (TQDockWindow*)oWindow->parent() )->hide();
    }

    rebuildCustomWidgetGUI();

    TQStringList l = config.readListEntry( keybase + "ToolBox/CommonWidgets" );
    if ( !l.isEmpty() ) {
	TQPtrList<TQAction> lst;
	commonWidgetsPage.clear();
	for ( TQStringList::ConstIterator it = l.begin(); it != l.end(); ++it ) {
	    for ( TQAction *a = toolActions.first(); a; a = toolActions.next() ) {
		if ( *it == a->text() ) {
		    lst.append( a );
		    break;
		}
	    }
	}
	if ( lst != commonWidgetsPage ) {
	    commonWidgetsPage = lst;
	    rebuildCommonWidgetsToolBoxPage();
	}
    }
}

HierarchyView *MainWindow::objectHierarchy() const
{
    if ( !hierarchyView )
	( (MainWindow*)this )->setupHierarchyView();
    return hierarchyView;
}

TQPopupMenu *MainWindow::setupNormalHierarchyMenu( TQWidget *parent )
{
    TQPopupMenu *menu = new TQPopupMenu( parent );

    actionEditCut->addTo( menu );
    actionEditCopy->addTo( menu );
    actionEditPaste->addTo( menu );
    actionEditDelete->addTo( menu );

    return menu;
}

TQPopupMenu *MainWindow::setupTabWidgetHierarchyMenu( TQWidget *parent, const char *addSlot, const char *removeSlot )
{
    TQPopupMenu *menu = new TQPopupMenu( parent );

    menu->insertItem( tr( "Add Page" ), parent, addSlot );
    menu->insertItem( tr( "Delete Page" ), parent, removeSlot );
    menu->insertSeparator();
    actionEditCut->addTo( menu );
    actionEditCopy->addTo( menu );
    actionEditPaste->addTo( menu );
    actionEditDelete->addTo( menu );

    return menu;
}

void MainWindow::closeEvent( TQCloseEvent *e )
{
    if ( singleProject ) {
	hide();
	e->ignore();
	return;
    }

    TQWidgetList windows = qWorkspace()->windowList();
    TQWidgetListIt wit( windows );
    while ( wit.current() ) {
	TQWidget *w = wit.current();
	++wit;
	if ( ::tqt_cast<FormWindow*>(w) ) {
	    if ( ( (FormWindow*)w )->formFile()->editor() )
		windows.removeRef( ( (FormWindow*)w )->formFile()->editor() );
	    if ( ( (FormWindow*)w )->formFile()->formWindow() )
		windows.removeRef( ( (FormWindow*)w )->formFile()->formWindow() );
	    if ( !( (FormWindow*)w )->formFile()->close() ) {
		e->ignore();
		return;
	    }
	} else if ( ::tqt_cast<SourceEditor*>(w) ) {
	    if ( !( (SourceEditor*)w )->close() ) {
		e->ignore();
		return;
	    }
	}
	w->close();
    }

    TQMapConstIterator<TQAction*, Project*> it = projects.begin();
    while( it != projects.end() ) {
	Project *pro = it.data();
	++it;
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
		e->ignore();
		return;
	    default:
		break;
	    }
	}
    }

    writeConfig();
    hide();
    e->accept();

    if ( client ) {
	TQDir home( TQDir::homeDirPath() );
	home.remove( ".designerpid" );
    }
}

Workspace *MainWindow::workspace() const
{
    if ( !wspace )
	( (MainWindow*)this )->setupWorkspace();
    return wspace;
}

PropertyEditor *MainWindow::propertyeditor() const
{
    if ( !propertyEditor )
	( (MainWindow*)this )->setupPropertyEditor();
    return propertyEditor;
}

ActionEditor *MainWindow::actioneditor() const
{
    if ( !actionEditor )
	( (MainWindow*)this )->setupActionEditor();
    return actionEditor;
}

bool MainWindow::openEditor( TQWidget *w, FormWindow *f )
{
    if ( f && !f->project()->isCpp() && !WidgetFactory::isPassiveInteractor( w ) ) {
	TQString defSignal = WidgetFactory::defaultSignal( w );
	if ( defSignal.isEmpty() ) {
	    editSource();
	} else {
	    TQString s = TQString( w->name() ) + "_" + defSignal;
	    LanguageInterface *iface = MetaDataBase::languageInterface( f->project()->language() );
	    if ( iface ) {
		TQStrList sigs = iface->signalNames( w );
		TQString fullSignal;
		for ( int i = 0; i < (int)sigs.count(); ++i ) {
		    TQString sig = sigs.at( i );
		    if ( sig.left( sig.find( '(' ) ) == defSignal ) {
			fullSignal = sig;
			break;
		    }
		}

		if ( !fullSignal.isEmpty() ) {
		    TQString signl = fullSignal;
		    fullSignal = fullSignal.mid( fullSignal.find( '(' ) + 1 );
		    fullSignal.remove( (int)fullSignal.length() - 1, 1 );
		    fullSignal = iface->createArguments( fullSignal.simplifyWhiteSpace() );
		    s += "(" + fullSignal + ")";
		    if ( !MetaDataBase::hasFunction( f, s.latin1() ) )
			MetaDataBase::addFunction( f, s.latin1(), "", "public", "slot",
						   f->project()->language(), "void" );
		    s = s.left( s.find( '(' ) ).latin1();
		    if ( !MetaDataBase::hasConnection( f, w, defSignal.latin1(), f->mainContainer(), s.latin1() ) ) {
			MetaDataBase::Connection conn;
			conn.sender = w;
			conn.receiver = f->mainContainer();
			conn.signal = signl;
			conn.slot = s;
			AddConnectionCommand *cmd =
			    new AddConnectionCommand( tr( "Add connection" ), f, conn );
			f->commandHistory()->addCommand( cmd );
			cmd->execute();
			f->formFile()->setModified( true );
		    }
		}
	    }
	    editFunction( s, true );
	}
	return true;
    }
    if ( WidgetFactory::hasSpecialEditor( WidgetDatabase::
					  idFromClassName( WidgetFactory::classNameOf( w ) ), w ) ) {
	statusBar()->message( tr( "Edit %1..." ).arg( w->className() ) );
	WidgetFactory::editWidget( WidgetDatabase::idFromClassName( WidgetFactory::classNameOf( w ) ),
				   this, w, formWindow() );
	statusBar()->clear();
	return true;
    }

    const TQMetaProperty* text = w->metaObject()->property( w->metaObject()->findProperty( "text", true ), true );
    const TQMetaProperty* title = w->metaObject()->property( w->metaObject()->findProperty( "title", true ), true );
    if ( text && text->designable(w) ) {
	bool ok = false;
	bool oldDoWrap = false;
	if ( ::tqt_cast<TQLabel*>(w) ) {
	    int align = w->property( "alignment" ).toInt();
	    if ( align & WordBreak )
		oldDoWrap = true;
	}
	bool doWrap = oldDoWrap;

	TQString text;
	if ( ::tqt_cast<TQTextEdit*>(w) || ::tqt_cast<TQLabel*>(w) || ::tqt_cast<TQButton*>(w) ) {
	    text = MultiLineEditor::getText( this, w->property( "text" ).toString(),
					     !::tqt_cast<TQButton*>(w), &doWrap );
	    ok = !text.isNull();
	} else {
	    text = TQInputDialog::getText( tr("Text"), tr( "New text" ),
				  TQLineEdit::Normal, w->property("text").toString(), &ok, this );
	}
	if ( ok ) {
	    if ( oldDoWrap != doWrap ) {
		TQString pn( tr( "Set 'wordwrap' of '%1'" ).arg( w->name() ) );
		SetPropertyCommand *cmd = new SetPropertyCommand( pn, formWindow(), w, propertyEditor,
								  "wordwrap", TQVariant( oldDoWrap ),
								  TQVariant( doWrap ), TQString::null, TQString::null );
		cmd->execute();
		formWindow()->commandHistory()->addCommand( cmd );
		MetaDataBase::setPropertyChanged( w, "wordwrap", true );
	    }

	    TQString pn( tr( "Set the 'text' of '%1'" ).arg( w->name() ) );
	    SetPropertyCommand *cmd = new SetPropertyCommand( pn, formWindow(), w, propertyEditor,
							      "text", w->property( "text" ),
							      text, TQString::null, TQString::null );
	    cmd->execute();
	    formWindow()->commandHistory()->addCommand( cmd );
	    MetaDataBase::setPropertyChanged( w, "text", true );
	}
	return true;
    }
    if ( title && title->designable(w) ) {
	bool ok = false;
	TQString text;
	text = TQInputDialog::getText( tr("Title"), tr( "New title" ), TQLineEdit::Normal, w->property("title").toString(), &ok, this );
	if ( ok ) {
	    TQString pn( tr( "Set the 'title' of '%2'" ).arg( w->name() ) );
	    SetPropertyCommand *cmd = new SetPropertyCommand( pn, formWindow(), w, propertyEditor,
							      "title", w->property( "title" ),
							      text, TQString::null, TQString::null );
	    cmd->execute();
	    formWindow()->commandHistory()->addCommand( cmd );
	    MetaDataBase::setPropertyChanged( w, "title", true );
	}
	return true;
    }

    if ( !WidgetFactory::isPassiveInteractor( w ) )
	editSource();

    return true;
}

void MainWindow::rebuildCustomWidgetGUI()
{
    customWidgetToolBar->clear();
    customWidgetMenu->clear();
    customWidgetToolBar2->clear();
    int count = 0;

    TQPtrListIterator<TQAction> it( toolActions );
    TQAction *action;
    while ( ( action = it.current() ) ) {
	++it;
	if ( ( (WidgetAction*)action )->group() == "Custom Widgets" )
	    delete action;
    }

    TQPtrList<MetaDataBase::CustomWidget> *lst = MetaDataBase::customWidgets();

    actionToolsCustomWidget->addTo( customWidgetMenu );
    customWidgetMenu->insertSeparator();

    for ( MetaDataBase::CustomWidget *w = lst->first(); w; w = lst->next() ) {
	WidgetAction* a = new WidgetAction( "Custom Widgets", actionGroupTools, TQString::number( w->id ).latin1() );
	a->setToggleAction( true );
	a->setText( w->className );
	a->setIconSet( *w->pixmap );
	a->setStatusTip( tr( "Insert a " +w->className + " (custom widget)" ) );
	a->setWhatsThis( tr("<b>" + w->className + " (custom widget)</b>"
			    "<p>Click <b>Edit Custom Widgets...</b> in the <b>Tools|Custom</b> menu to "
			    "add and change custom widgets. You can add properties as well as "
			    "signals and slots to integrate them into TQt Designer, "
			    "and provide a pixmap which will be used to represent the widget on the form.</p>") );

	a->addTo( customWidgetToolBar );
	a->addTo( customWidgetToolBar2 );
	a->addTo( customWidgetMenu );
	count++;
    }
    TQWidget *wid;
    customWidgetToolBar2->setStretchableWidget( ( wid = new TQWidget( customWidgetToolBar2 ) ) );
    wid->setBackgroundMode( customWidgetToolBar2->backgroundMode() );

    if ( count == 0 )
	customWidgetToolBar->hide();
    else if ( customWidgetToolBar->isVisible() )
	customWidgetToolBar->show();
}

void MainWindow::rebuildCommonWidgetsToolBoxPage()
{
    toolBox->setUpdatesEnabled( false );
    commonWidgetsToolBar->setUpdatesEnabled( false );
    commonWidgetsToolBar->clear();
    for ( TQAction *a = commonWidgetsPage.first(); a; a = commonWidgetsPage.next() )
	a->addTo( commonWidgetsToolBar );
    TQWidget *w;
    commonWidgetsToolBar->setStretchableWidget( ( w = new TQWidget( commonWidgetsToolBar ) ) );
    w->setBackgroundMode( commonWidgetsToolBar->backgroundMode() );
    toolBox->setUpdatesEnabled( true );
    commonWidgetsToolBar->setUpdatesEnabled( true );
}

bool MainWindow::isCustomWidgetUsed( MetaDataBase::CustomWidget *wid )
{
    TQWidgetList windows = qWorkspace()->windowList();
    for ( TQWidget *w = windows.first(); w; w = windows.next() ) {
	if ( ::tqt_cast<FormWindow*>(w) ) {
	    if ( ( (FormWindow*)w )->isCustomWidgetUsed( wid ) )
		return true;
	}
    }
    return false;
}

void MainWindow::setGrid( const TQPoint &p )
{
    if ( p == grd )
	return;
    grd = p;
    TQWidgetList windows = qWorkspace()->windowList();
    for ( TQWidget *w = windows.first(); w; w = windows.next() ) {
	if ( !::tqt_cast<FormWindow*>(w) )
	    continue;
	( (FormWindow*)w )->mainContainer()->update();
    }
}

void MainWindow::setShowGrid( bool b )
{
    if ( b == sGrid )
	return;
    sGrid = b;
    TQWidgetList windows = qWorkspace()->windowList();
    for ( TQWidget *w = windows.first(); w; w = windows.next() ) {
	if ( !::tqt_cast<FormWindow*>(w) )
	    continue;
	( (FormWindow*)w )->mainContainer()->update();
    }
}

void MainWindow::setSnapGrid( bool b )
{
    if ( b == snGrid )
	return;
    snGrid = b;
}

TQString MainWindow::documentationPath() const
{
    return TQString( tqInstallPathDocs() ) + "/html/";
}

void MainWindow::windowsMenuActivated( int id )
{
    TQWidget* w = qworkspace->windowList().at( id );
    if ( w )
	w->setFocus();
}

void MainWindow::projectSelected( TQAction *a )
{
    a->setOn( true );
    if ( currentProject )
	currentProject->setActive( false );
    Project *p = *projects.find( a );
    p->setActive( true );
    if ( currentProject == p )
	return;
    currentProject = p;
    if ( wspace )
	wspace->setCurrentProject( currentProject );
}

void MainWindow::openProject( const TQString &fn )
{
    for ( TQMap<TQAction*, Project*>::ConstIterator it = projects.begin(); it != projects.end(); ++it ) {
	if ( (*it)->fileName() == fn ) {
	    projectSelected( it.key() );
	    return;
	}
    }
    TQApplication::setOverrideCursor( waitCursor );
    Project *pro = new Project( fn, "", projectSettingsPluginManager );
    pro->setModified( false );
    TQAction *a = new TQAction( pro->projectName(), pro->projectName(), 0, actionGroupProjects, 0, true );
    projects.insert( a, pro );
    projectSelected( a );
    TQApplication::restoreOverrideCursor();
}

void MainWindow::checkTempFiles()
{
    TQString s = TQDir::homeDirPath() + "/.designer";
    TQString baseName = s+ "/saved-form-";
    if ( !TQFile::exists( baseName + "1.ui" ) )
	return;
    DesignerApplication::closeSplash();
    TQDir d( s );
    d.setNameFilter( "*.ui" );
    TQStringList lst = d.entryList();
    TQApplication::restoreOverrideCursor();
    bool load = TQMessageBox::information( this, tr( "Restoring the Last Session" ),
					  tr( "TQt Designer found some temporary saved files, which were\n"
					      "written when TQt Designer crashed last time. Do you want to\n"
					      "load these files?" ), tr( "&Yes" ), tr( "&No" ) ) == 0;
    TQApplication::setOverrideCursor( waitCursor );
    for ( TQStringList::ConstIterator it = lst.begin(); it != lst.end(); ++it ) {
	if ( load )
	    openFormWindow( s + "/" + *it, false );
	d.remove( *it );
    }
}

void MainWindow::showDialogHelp()
{
    TQWidget *w = (TQWidget*)sender();
    w = w->topLevelWidget();

    TQString link = documentationPath() + "/designer-manual-13.html#";

    if ( ::tqt_cast<NewFormBase*>(w) || ::tqt_cast<StartDialogBase*>(w) ) // own doc for startdialog?
	link += "dialog-file-new";
    else if ( ::tqt_cast<CreateTemplate*>(w) )
	link += "dialog-file-create-template";
    else if ( ::tqt_cast<EditFunctionsBase*>(w) )
	link += "dialog-edit-functions";
//     else if ( ::tqt_cast<ConnectionViewerBase*>(w) )
    else if ( w->inherits("ConnectionViewerBase") )
 	link += "dialog-view-connections";
    else if ( ::tqt_cast<FormSettingsBase*>(w) )
	link += "dialog-edit-form-settings";
    else if ( ::tqt_cast<Preferences*>(w) )
	link += "dialog-edit-preferences";
    else if ( ::tqt_cast<PixmapCollectionEditor*>(w) )
	link += "dialog-image-collection";
//    else if ( ::tqt_cast<DatabaseConnectionBase*>(w) )
    else if ( w->inherits( "DatabaseConnectionBase" ) )
	link += "dialog-edit-database-connections";
    else if ( ::tqt_cast<ProjectSettingsBase*>(w) )
	link += "dialog-project-settings";
    else if ( ::tqt_cast<FindDialog*>(w) )
	link += "dialog-find-text";
    else if ( ::tqt_cast<ReplaceDialog*>(w) )
	link += "dialog-replace-text";
    else if ( ::tqt_cast<GotoLineDialog*>(w) )
	link += "dialog-go-to-line";
//    else if ( ::tqt_cast<ConnectionEditorBase*>(w) )
    else if ( w->inherits("ConnectionEditorBase") )
	link += "dialog-edit-connections";
    else if ( ::tqt_cast<CustomWidgetEditorBase*>(w) )
	link += "dialog-edit-custom-widgets";
    else if ( ::tqt_cast<PaletteEditorBase*>(w) )
	link += "dialog-edit-palette";
    else if ( ::tqt_cast<ListBoxEditorBase*>(w) )
	link += "dialog-edit-listbox";
    else if ( ::tqt_cast<ListViewEditorBase*>(w) )
	link += "dialog-edit-listview";
    else if ( ::tqt_cast<IconViewEditorBase*>(w) )
	link += "dialog-edit-iconview";
    else if ( ::tqt_cast<TableEditorBase*>(w) )
	link += "dialog-edit-table";
    else if ( ::tqt_cast<MultiLineEditor*>(w) )
	link += "dialog-text";

    else {
	TQMessageBox::information( this, tr( "Help" ),
				  tr( "There is no help available for this dialog at the moment." ) );
	return;
    }

    assistant->showPage( link );
}

void MainWindow::setupActionManager()
{
    actionPluginManager = new TQPluginManager<ActionInterface>( IID_Action, TQApplication::libraryPaths(), pluginDirectory() );

    TQStringList lst = actionPluginManager->featureList();
    for ( TQStringList::ConstIterator ait = lst.begin(); ait != lst.end(); ++ait ) {
	ActionInterface *iface = 0;
	actionPluginManager->queryInterface( *ait, &iface );
	if ( !iface )
	    continue;

	iface->connectTo( desInterface );
	TQAction *a = iface->create( *ait, this );
	if ( !a )
	    continue;

	TQString grp = iface->group( *ait );
	if ( grp.isEmpty() )
	    grp = "3rd party actions";
	TQPopupMenu *menu = 0;
	TQToolBar *tb = 0;

	if ( !( menu = (TQPopupMenu*)child( grp.latin1(), "TQPopupMenu" ) ) ) {
	    menu = new TQPopupMenu( this, grp.latin1() );
	    menuBar()->insertItem( tr( grp ), menu );
	}
	if ( !( tb = (TQToolBar*)child( grp.latin1(), "TQToolBar" ) ) ) {
	    tb = new TQToolBar( this, grp.latin1() );
	    tb->setCloseMode( TQDockWindow::Undocked );
	    addToolBar( tb, grp );
	}

	if ( iface->location( *ait, ActionInterface::Menu ) )
	    a->addTo( menu );
	if ( iface->location( *ait, ActionInterface::Toolbar ) )
	    a->addTo( tb );

	iface->release();
    }
}

void MainWindow::editFunction( const TQString &func, bool rereadSource )
{
    if ( !formWindow() )
	return;

    if ( formWindow()->formFile()->codeFileState() != FormFile::Ok )
	if ( !formWindow()->formFile()->setupUihFile(false) )
	    return;

    TQString lang = currentProject->language();
    if ( !MetaDataBase::hasEditor( lang ) ) {
	TQMessageBox::information( this, tr( "Edit Source" ),
				  tr( "There is no plugin for editing " + lang + " code installed!\n"
				      "Note: Plugins are not available in static TQt configurations." ) );
	return;
    }

    for ( SourceEditor *e = sourceEditors.first(); e; e = sourceEditors.next() ) {
	if ( e->language() == lang && e->formWindow() == formWindow() ) {
	    e->show();
	    e->setFunction( func );
	    return;
	}
    }

    createSourceEditor( formWindow(), formWindow()->project(), lang, func, rereadSource );
}

void MainWindow::setupRecentlyFilesMenu()
{
    recentlyFilesMenu->clear();
    int id = 0;
    for ( TQStringList::ConstIterator it = recentlyFiles.begin(); it != recentlyFiles.end(); ++it ) {
	recentlyFilesMenu->insertItem( *it, id );
	id++;
    }
}

void MainWindow::setupRecentlyProjectsMenu()
{
    recentlyProjectsMenu->clear();
    int id = 0;
    for ( TQStringList::ConstIterator it = recentlyProjects.begin(); it != recentlyProjects.end(); ++it ) {
	recentlyProjectsMenu->insertItem( *it, id );
	id++;
    }
}

TQPtrList<DesignerProject> MainWindow::projectList() const
{
    TQPtrList<DesignerProject> list;
    TQMapConstIterator<TQAction*, Project*> it = projects.begin();

    while( it != projects.end() ) {
	Project *p = it.data();
	++it;
	list.append( p->iFace() );
    }

    return list;
}

TQStringList MainWindow::projectNames() const
{
    TQStringList res;
    for ( TQMap<TQAction*, Project* >::ConstIterator it = projects.begin(); it != projects.end(); ++it )
	res << (*it)->projectName();
    return res;
}

TQStringList MainWindow::projectFileNames() const
{
    TQStringList res;
    for ( TQMap<TQAction*, Project* >::ConstIterator it = projects.begin(); it != projects.end(); ++it )
	res << (*it)->makeRelative( (*it)->fileName() );
    return res;
}

Project *MainWindow::findProject( const TQString &projectName ) const
{
    for ( TQMap<TQAction*, Project* >::ConstIterator it = projects.begin(); it != projects.end(); ++it ) {
	if ( (*it)->projectName() == projectName )
	    return *it;
    }
    return 0;
}

void MainWindow::setCurrentProject( Project *pro )
{
    for ( TQMap<TQAction*, Project* >::ConstIterator it = projects.begin(); it != projects.end(); ++it ) {
	if ( *it == pro ) {
	    projectSelected( it.key() );
	    return;
	}
    }
}

void MainWindow::setCurrentProjectByFilename( const TQString& proFilename )
{
    for ( TQMap<TQAction*, Project* >::ConstIterator it = projects.begin(); it != projects.end(); ++it ) {
	if ( (*it)->makeRelative( (*it)->fileName() ) == proFilename ) {
	    projectSelected( it.key() );
	    return;
	}
    }
}


void MainWindow::recentlyFilesMenuActivated( int id )
{
    if ( id != -1 ) {
	if ( !TQFile::exists( *recentlyFiles.at( id ) ) ) {
	    TQMessageBox::warning( this, tr( "Open File" ),
				  tr( "Could not open '%1'. File does not exist." ).
				  arg( *recentlyFiles.at( id ) ) );
	    recentlyFiles.remove( recentlyFiles.at( id ) );
	    return;
	}
	fileOpen( "", "", *recentlyFiles.at( id ) );
	TQString fn( *recentlyFiles.at( id ) );
	addRecentlyOpened( fn, recentlyFiles );
    }
}

void MainWindow::recentlyProjectsMenuActivated( int id )
{
    if ( id != -1 ) {
	if ( !TQFile::exists( *recentlyProjects.at( id ) ) ) {
	    TQMessageBox::warning( this, tr( "Open Project" ),
				  tr( "Could not open '%1'. File does not exist." ).
				  arg( *recentlyProjects.at( id ) ) );
	    recentlyProjects.remove( recentlyProjects.at( id ) );
	    return;
	}
	openProject( *recentlyProjects.at( id ) );
	TQString fn( *recentlyProjects.at( id ) );
	addRecentlyOpened( fn, recentlyProjects );
    }
}

void MainWindow::addRecentlyOpened( const TQString &fn, TQStringList &lst )
{
    TQFileInfo fi( fn );
    fi.convertToAbs();
    TQString f = fi.filePath();
    if ( lst.find( f ) != lst.end() )
	lst.remove( f );
    if ( lst.count() >= 10 )
	lst.pop_back();
    lst.prepend( f );
}

TemplateWizardInterface * MainWindow::templateWizardInterface( const TQString& className )
{
    TemplateWizardInterface* iface = 0;
    templateWizardPluginManager->queryInterface( className, & iface );
    return iface;
}

void MainWindow::setupPluginManagers()
{
    editorPluginManager = new TQPluginManager<EditorInterface>( IID_Editor, TQApplication::libraryPaths(), pluginDirectory() );
    MetaDataBase::setEditor( editorPluginManager->featureList() );

    templateWizardPluginManager =
	new TQPluginManager<TemplateWizardInterface>( IID_TemplateWizard, TQApplication::libraryPaths(), pluginDirectory() );

    MetaDataBase::setupInterfaceManagers( pluginDirectory() );
    preferencePluginManager =
	new TQPluginManager<PreferenceInterface>( IID_Preference, TQApplication::libraryPaths(), pluginDirectory() );
    projectSettingsPluginManager =
	new TQPluginManager<ProjectSettingsInterface>( IID_ProjectSettings, TQApplication::libraryPaths(), pluginDirectory() );
    sourceTemplatePluginManager =
	new TQPluginManager<SourceTemplateInterface>( IID_SourceTemplate, TQApplication::libraryPaths(), pluginDirectory() );

    if ( preferencePluginManager ) {
	TQStringList lst = preferencePluginManager->featureList();
	for ( TQStringList::ConstIterator it = lst.begin(); it != lst.end(); ++it ) {
	    PreferenceInterface *i = 0;
	    preferencePluginManager->queryInterface( *it, &i );
	    if ( !i )
		continue;
	    i->connectTo( designerInterface() );
	    PreferenceInterface::Preference *pf = i->preference();
	    if ( pf )
		addPreferencesTab( pf->tab, pf->title, pf->receiver, pf->init_slot, pf->accept_slot );
	    i->deletePreferenceObject( pf );

	    i->release();
	}
    }
    if ( projectSettingsPluginManager ) {
	TQStringList lst = projectSettingsPluginManager->featureList();
	for ( TQStringList::ConstIterator it = lst.begin(); it != lst.end(); ++it ) {
	    ProjectSettingsInterface *i = 0;
	    projectSettingsPluginManager->queryInterface( *it, &i );
	    if ( !i )
		continue;
	    i->connectTo( designerInterface() );

	    ProjectSettingsInterface::ProjectSettings *pf = i->projectSetting();
	    if ( pf )
		addProjectTab( pf->tab, pf->title, pf->receiver, pf->init_slot, pf->accept_slot );
	    i->deleteProjectSettingsObject( pf );
	    i->release();
	}
    }
}

void MainWindow::addPreferencesTab( TQWidget *tab, const TQString &title, TQObject *receiver, const char *init_slot, const char *accept_slot )
{
    Tab t;
    t.w = tab;
    t.title = title;
    t.receiver = receiver;
    t.init_slot = init_slot;
    t.accept_slot = accept_slot;
    preferenceTabs << t;
}

void MainWindow::addProjectTab( TQWidget *tab, const TQString &title, TQObject *receiver, const char *init_slot, const char *accept_slot )
{
    Tab t;
    t.w = tab;
    t.title = title;
    t.receiver = receiver;
    t.init_slot = init_slot;
    t.accept_slot = accept_slot;
    projectTabs << t;
}

void MainWindow::setModified( bool b, TQWidget *window )
{
    TQWidget *w = window;
    while ( w ) {
	if ( ::tqt_cast<FormWindow*>(w) ) {
	    ( (FormWindow*)w )->modificationChanged( b );
	    return;
	} else if ( ::tqt_cast<SourceEditor*>(w) ) {
	    FormWindow *fw = ( (SourceEditor*)w )->formWindow();
	    if ( fw && !fw->isFake() ) {
		//fw->commandHistory()->setModified( b );
		//fw->modificationChanged( b );
		fw->formFile()->setModified( b, FormFile::WFormCode );
		wspace->update( fw->formFile() );
	    } else {
		wspace->update();
	    }
	    return;
	}
	w = w->parentWidget( true );
    }
}

void MainWindow::editorClosed( SourceEditor *e )
{
    sourceEditors.take( sourceEditors.findRef( e ) );
}

void MainWindow::functionsChanged()
{
    updateFunctionsTimer->start( 0, true );
}

void MainWindow::doFunctionsChanged()
{
    for ( SourceEditor *e = sourceEditors.first(); e; e = sourceEditors.next() )
	e->refresh( false );
    hierarchyView->formDefinitionView()->refresh();
}

void MainWindow::updateFunctionList()
{
    if ( !qWorkspace()->activeWindow() || !::tqt_cast<SourceEditor*>(qWorkspace()->activeWindow()) )
	return;
    SourceEditor *se = (SourceEditor*)qWorkspace()->activeWindow();
    se->save();
    hierarchyView->formDefinitionView()->refresh();
    if ( !currentProject->isCpp() && se->formWindow() ) {
	LanguageInterface *iface = MetaDataBase::languageInterface( currentProject->language() );
	if ( !iface )
	    return;
	TQValueList<LanguageInterface::Connection> conns;
	iface->connections( se->text(), &conns );
	MetaDataBase::setupConnections( se->formWindow(), conns );
	propertyEditor->eventList()->setup();
    }
}

void MainWindow::updateWorkspace()
{
    wspace->setCurrentProject( currentProject );
}

void MainWindow::showDebugStep( TQObject *o, int line )
{
    for ( SourceEditor *e = sourceEditors.first(); e; e = sourceEditors.next() )
	e->clearStep();
    if ( !o || line == -1 )
	return;
    showSourceLine( o, line, Step );
}

void MainWindow::showStackFrame( TQObject *o, int line )
{
    if ( !o || line == -1 )
	return;
    showSourceLine( o, line, StackFrame );
}

void MainWindow::showErrorMessage( TQObject *o, int errorLine, const TQString &errorMessage )
{
    if ( o ) {
	errorLine--; // ######
	TQValueList<uint> l;
	l << ( errorLine + 1 );
	TQStringList l2;
	l2 << errorMessage;
	TQObjectList ol;
	ol.append( o );
	TQStringList ll;
	ll << currentProject->locationOfObject( o );
	oWindow->setErrorMessages( l2, l, true, ll, ol );
	showSourceLine( o, errorLine, Error );
    }
}

void MainWindow::finishedRun()
{
    inDebugMode = false;
    previewing = false;
    debuggingForms.clear();
    enableAll( true );
    for ( SourceEditor *e = sourceEditors.first(); e; e = sourceEditors.next() ) {
	if ( e->project() == currentProject )
	    e->editorInterface()->setMode( EditorInterface::Editing );
	e->clearStackFrame();
    }
    outputWindow()->clearErrorMessages();
}

void MainWindow::enableAll( bool enable )
{
    menuBar()->setEnabled( enable );
    TQObjectList *l = queryList( "TQDockWindow" );
    for ( TQObject *o = l->first(); o; o = l->next() ) {
	if ( o == wspace->parentWidget() ||
	     o == oWindow->parentWidget() ||
	     o == hierarchyView->parentWidget() )
	    continue;
	( (TQWidget*)o )->setEnabled( enable );
    }
    delete l;
}

void MainWindow::showSourceLine( TQObject *o, int line, LineMode lm )
{
    TQWidgetList windows = qworkspace->windowList();
    for ( TQWidget *w = windows.first(); w; w = windows.next() ) {
	FormWindow *fw = 0;
	SourceEditor *se = 0;
	SourceFile *sf = 0;
	if ( ::tqt_cast<FormWindow*>(w) ) {
	    fw = (FormWindow*)w;
	} else if ( ::tqt_cast<SourceEditor*>(w) ) {
	    se = (SourceEditor*)w;
	    if ( !se->object() )
		continue;
	    if ( se->formWindow() )
		fw = se->formWindow();
	    else
		sf = se->sourceFile();
	}

	if ( fw ) {
	    if ( fw->project() != currentProject )
		continue;
	    if ( qstrcmp( fw->name(), o->name() ) == 0 ||
		 ( fw->isFake() && currentProject->objectForFakeForm( fw ) == o ) ) {
		if ( se ) {
		    switch ( lm ) {
		    case Error:
			se->editorInterface()->setError( line );
			break;
		    case Step:
			se->editorInterface()->setStep( line );
			break;
		    case StackFrame:
			se->editorInterface()->setStackFrame( line );
			break;
		    }
		    return;
		} else {
		    fw->showNormal();
		    fw->setFocus();
		    lastActiveFormWindow = fw;
		    tqApp->processEvents();
		    se = editSource();
		    if ( se ) {
			switch ( lm ) {
			case Error:
			    se->editorInterface()->setError( line );
			    break;
			case Step:
			    se->editorInterface()->setStep( line );
			    break;
			case StackFrame:
			    se->editorInterface()->setStackFrame( line );
			    break;
			}
			return;
		    }
		}
	    }
	} else if ( se ) {
	    if ( o != sf )
		continue;
	    switch ( lm ) {
	    case Error:
		se->editorInterface()->setError( line );
		break;
	    case Step:
		se->editorInterface()->setStep( line );
		break;
	    case StackFrame:
		se->editorInterface()->setStackFrame( line );
		break;
	    }
	    return;
	}
    }

    if ( ::tqt_cast<SourceFile*>(o) ) {
	for ( TQPtrListIterator<SourceFile> sources = currentProject->sourceFiles();
	      sources.current(); ++sources ) {
	    SourceFile* f = sources.current();
	    if ( f == o ) {
		SourceEditor *se = editSource( f );
		if ( se ) {
		    switch ( lm ) {
		    case Error:
			se->editorInterface()->setError( line );
			break;
		    case Step:
			se->editorInterface()->setStep( line );
			break;
		    case StackFrame:
			se->editorInterface()->setStackFrame( line );
			break;
		    }
		}
		return;
	    }
	}
    }

    FormFile *ff = currentProject->fakeFormFileFor( o );
    FormWindow *fw = 0;
    if ( ff )
	fw = ff->formWindow();

    if ( !fw && !qwf_forms ) {
	tqWarning( "MainWindow::showSourceLine: qwf_forms is NULL!" );
	return;
    }

    mblockNewForms = true;
    if ( !fw )
	openFormWindow( currentProject->makeAbsolute( *qwf_forms->find( (TQWidget*)o ) ) );
    else
	fw->formFile()->showEditor( false );
    tqApp->processEvents(); // give all views the chance to get the formwindow
    SourceEditor *se = editSource();
    if ( se ) {
	switch ( lm ) {
	case Error:
	    se->editorInterface()->setError( line );
	    break;
	case Step:
	    se->editorInterface()->setStep( line );
	    break;
	case StackFrame:
	    se->editorInterface()->setStackFrame( line );
	    break;
	}
    }
    mblockNewForms = false;
}


TQObject *MainWindow::findRealObject( TQObject *o )
{
    TQWidgetList windows = qWorkspace()->windowList();
    for ( TQWidget *w = windows.first(); w; w = windows.next() ) {
	if ( ::tqt_cast<FormWindow*>(w) && TQString( w->name() ) == TQString( o->name() ) )
	    return w;
	else if ( ::tqt_cast<SourceEditor*>(w) && ( (SourceEditor*)w )->formWindow() &&
		  TQString( ( (SourceEditor*)w )->formWindow()->name() ) == TQString( o->name() ) )
	    return w;
	else if ( ::tqt_cast<SourceFile*>(w) && ( (SourceEditor*)w )->sourceFile() &&
		  ( (SourceEditor*)w )->sourceFile() == o )
	    return o;
    }
    return 0;
}

void MainWindow::formNameChanged( FormWindow *fw )
{
    for ( SourceEditor *e = sourceEditors.first(); e; e = sourceEditors.next() ) {
	if ( e->object() == fw )
	    e->refresh( true );
	if ( e->project() == fw->project() )
	    e->resetContext();
    }
}

void MainWindow::breakPointsChanged()
{
    if ( !inDebugMode )
	return;
    if ( !qWorkspace()->activeWindow() || !::tqt_cast<SourceEditor*>(qWorkspace()->activeWindow()) )
	return;
    SourceEditor *e = (SourceEditor*)qWorkspace()->activeWindow();
    if ( !e->object() || !e->project() )
	return;
    if ( e->project() != currentProject )
	return;

    if ( !interpreterPluginManager ) {
	interpreterPluginManager =
	    new TQPluginManager<InterpreterInterface>( IID_Interpreter,
						      TQApplication::libraryPaths(),
						      "/qsa" );
    }

    InterpreterInterface *iiface = 0;
    if ( interpreterPluginManager ) {
	TQString lang = currentProject->language();
	iiface = 0;
	interpreterPluginManager->queryInterface( lang, &iiface );
	if ( !iiface )
	    return;
    }

    e->saveBreakPoints();

    for ( TQObject *o = debuggingForms.first(); o; o = debuggingForms.next() ) {
	if ( qstrcmp( o->name(), e->object()->name() ) == 0 ) {
	    iiface->setBreakPoints( o, MetaDataBase::breakPoints( e->object() ) );
	    break;
	}
    }

    for ( e = sourceEditors.first(); e; e = sourceEditors.next() ) {
	if ( e->project() == currentProject && e->sourceFile() ) {
	    TQValueList<uint> bps = MetaDataBase::breakPoints( e->sourceFile() );
	    iiface->setBreakPoints( e->object(), bps );
	}
    }

    iiface->release();
}

int MainWindow::currentLayoutDefaultSpacing() const
{
    if ( ( (MainWindow*)this )->formWindow() )
	return ( (MainWindow*)this )->formWindow()->layoutDefaultSpacing();
    return BOXLAYOUT_DEFAULT_SPACING;
}

int MainWindow::currentLayoutDefaultMargin() const
{
    if ( ( (MainWindow*)this )->formWindow() )
	return ( (MainWindow*)this )->formWindow()->layoutDefaultMargin();
    return BOXLAYOUT_DEFAULT_MARGIN;
}

void MainWindow::saveAllBreakPoints()
{
    for ( SourceEditor *e = sourceEditors.first(); e; e = sourceEditors.next() ) {
	e->save();
	e->saveBreakPoints();
    }
}

void MainWindow::resetBreakPoints()
{
    for ( SourceEditor *e = sourceEditors.first(); e; e = sourceEditors.next() )
	e->resetBreakPoints();
}

SourceFile *MainWindow::sourceFile()
{
    for ( SourceEditor *e = sourceEditors.first(); e; e = sourceEditors.next() ) {
	if ( qworkspace->activeWindow() == e ) {
	    if ( e->sourceFile() )
		return e->sourceFile();
	}
    }
    return 0;
}

bool MainWindow::openProjectSettings( Project *pro )
{
    ProjectSettings dia( pro, this, 0, true );
    SenderObject *senderObject = new SenderObject( designerInterface() );
    TQValueList<Tab>::ConstIterator it;
    for ( it = projectTabs.begin(); it != projectTabs.end(); ++it ) {
	Tab t = *it;
	if ( t.title != pro->language() )
	    continue;
	dia.tabWidget->addTab( t.w, t.title );
	if ( t.receiver ) {
	    connect( dia.buttonOk, TQ_SIGNAL( clicked() ), senderObject, TQ_SLOT( emitAcceptSignal() ) );
	    connect( senderObject, TQ_SIGNAL( acceptSignal( TQUnknownInterface * ) ), t.receiver, t.accept_slot );
	    connect( senderObject, TQ_SIGNAL( initSignal( TQUnknownInterface * ) ), t.receiver, t.init_slot );
	    senderObject->emitInitSignal();
	    disconnect( senderObject, TQ_SIGNAL( initSignal( TQUnknownInterface * ) ), t.receiver, t.init_slot );
	}
    }

    if ( singleProject )
	dia.tabWidget->setTabEnabled( dia.tabSettings, false );

    int res = dia.exec();

    delete senderObject;

    for ( it = projectTabs.begin(); it != projectTabs.end(); ++it ) {
	Tab t = *it;
	dia.tabWidget->removePage( t.w );
	t.w->reparent( 0, TQPoint(0,0), false );
    }

    return res == TQDialog::Accepted;
}

void MainWindow::popupProjectMenu( const TQPoint &pos )
{
    projectMenu->exec( pos );
}

TQStringList MainWindow::sourceTemplates() const
{
    return sourceTemplatePluginManager->featureList();
}

SourceTemplateInterface* MainWindow::sourceTemplateInterface( const TQString& templ )
{
    SourceTemplateInterface *iface = 0;
    sourceTemplatePluginManager->queryInterface( templ, &iface);
    return iface;
}

TQString MainWindow::whatsThisFrom( const TQString &key )
{
    if ( menuHelpFile.isEmpty() ) {
	TQString fn( documentationPath() );
	fn += "/designer-manual-11.html";
	TQFile f( fn );
	if ( f.open( IO_ReadOnly ) ) {
	    TQTextStream ts( &f );
	    menuHelpFile = ts.read();
	}
    }

    int i = menuHelpFile.find( key );
    if ( i == -1 )
	return TQString::null;
    int start = i;
    int end = i;
    start = menuHelpFile.findRev( "<li>", i ) + 4;
    end = menuHelpFile.find( '\n', i ) - 1;
    return menuHelpFile.mid( start, end - start + 1 );
}

void MainWindow::setSingleProject( Project *pro )
{
    if ( eProject ) {
	Project *pro = eProject;
	pro->save();
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
		    ( (FormWindow*)w )->formFile()->close();
		}
	    } else if ( ::tqt_cast<SourceEditor*>(w) ) {
		( (SourceEditor*)w )->close();
	    }
	}
	hierarchyView->clear();
	windows = qWorkspace()->windowList();
	qWorkspace()->blockSignals( false );
	currentProject = 0;
	updateUndoRedo( false, false, TQString::null, TQString::null );
    }

    singleProject = true;
    projects.clear();
    TQAction *a = new TQAction( tr( pro->name() ), tr( pro->name() ), 0,
			      actionGroupProjects, 0, true );
    eProject = pro;
    projects.insert( a, eProject );
    a->setOn( true );
    actionGroupProjects->removeFrom( projectMenu );
    actionGroupProjects->removeFrom( projectToolBar );
    currentProject = eProject;
    currentProject->designerCreated();
}

void MainWindow::shuttingDown()
{
    outputWindow()->shuttingDown();
}

void MainWindow::showGUIStuff( bool b )
{
    if ( (bool)guiStuffVisible == b )
	return;
    guiStuffVisible = b;
    if ( !b ) {
	setAppropriate( (TQDockWindow*)toolBox->parentWidget(), false );
	toolBox->parentWidget()->hide();
	for ( TQToolBar *tb = widgetToolBars.first(); tb; tb = widgetToolBars.next() ) {
	    tb->hide();
	    setAppropriate( tb, false );
	}
	propertyEditor->setPropertyEditorEnabled( false );
	setAppropriate( layoutToolBar, false );
	layoutToolBar->hide();
	setAppropriate( toolsToolBar, false );
	toolsToolBar->hide();
	menubar->removeItem( toolsMenuId );
	menubar->removeItem( toolsMenuId + 1 );
	menubar->removeItem( toolsMenuId + 2 );
	disconnect( this, TQ_SIGNAL( hasActiveForm(bool) ), actionEditAccels, TQ_SLOT( setEnabled(bool) ) );
	disconnect( this, TQ_SIGNAL( hasActiveForm(bool) ), actionEditFunctions, TQ_SLOT( setEnabled(bool) ) );
	disconnect( this, TQ_SIGNAL( hasActiveForm(bool) ), actionEditConnections, TQ_SLOT( setEnabled(bool) ) );
	disconnect( this, TQ_SIGNAL( hasActiveForm(bool) ), actionEditSource, TQ_SLOT( setEnabled(bool) ) );
	disconnect( this, TQ_SIGNAL( hasActiveForm(bool) ), actionEditFormSettings, TQ_SLOT( setEnabled(bool) ) );
	actionEditFormSettings->setEnabled( false );
	actionEditSource->setEnabled( false );
	actionEditConnections->setEnabled( false );
	actionEditFunctions->setEnabled( false );
	actionEditAccels->setEnabled( false );
	( (TQDockWindow*)propertyEditor->parentWidget() )->
	    setCaption( tr( "Signal Handlers" ) );
	actionGroupNew->removeFrom( fileMenu );
	actionGroupNew->removeFrom( projectToolBar );
	actionFileSave->removeFrom( fileMenu );
	actionFileSave->removeFrom( projectToolBar );
	actionFileExit->removeFrom( fileMenu );
	actionNewFile->addTo( fileMenu );
	actionNewFile->addTo( projectToolBar );
	actionFileSave->addTo( fileMenu );
	actionFileSave->addTo( projectToolBar );
	actionFileExit->addTo( fileMenu );
    } else {
	setAppropriate( (TQDockWindow*)toolBox->parentWidget(), true );
	toolBox->parentWidget()->show();
	for ( TQToolBar *tb = widgetToolBars.first(); tb; tb = widgetToolBars.next() ) {
	    setAppropriate( tb, true );
	    tb->hide();
	}
	propertyEditor->setPropertyEditorEnabled( true );
	setAppropriate( layoutToolBar, true );
	layoutToolBar->show();
	setAppropriate( toolsToolBar, true );
	toolsToolBar->show();
	menubar->insertItem( tr( "&Tools" ), toolsMenu, toolsMenuId, toolsMenuIndex );
	menubar->insertItem( tr( "&Layout" ), layoutMenu, toolsMenuId + 1, toolsMenuIndex + 1 );
	menubar->insertItem( tr( "&Preview" ), previewMenu, toolsMenuId + 2, toolsMenuIndex + 2 );
	connect( this, TQ_SIGNAL( hasActiveForm(bool) ), actionEditAccels, TQ_SLOT( setEnabled(bool) ) );
	connect( this, TQ_SIGNAL( hasActiveForm(bool) ), actionEditFunctions, TQ_SLOT( setEnabled(bool) ) );
	connect( this, TQ_SIGNAL( hasActiveForm(bool) ), actionEditConnections, TQ_SLOT( setEnabled(bool) ) );
	connect( this, TQ_SIGNAL( hasActiveForm(bool) ), actionEditSource, TQ_SLOT( setEnabled(bool) ) );
	connect( this, TQ_SIGNAL( hasActiveForm(bool) ), actionEditFormSettings, TQ_SLOT( setEnabled(bool) ) );
	actionEditFormSettings->setEnabled( true );
	actionEditSource->setEnabled( true );
	actionEditConnections->setEnabled( true );
	actionEditFunctions->setEnabled( true );
	actionEditAccels->setEnabled( true );
	( (TQDockWindow*)propertyEditor->parentWidget() )->
	    setCaption( tr( "Property Editor/Signal Handlers" ) );
	actionFileSave->removeFrom( fileMenu );
	actionFileSave->removeFrom( projectToolBar );
	actionFileExit->removeFrom( fileMenu );
	actionGroupNew->addTo( fileMenu );
	actionGroupNew->addTo( projectToolBar );
	actionFileSave->addTo( fileMenu );
	actionFileSave->addTo( projectToolBar );
	actionFileExit->addTo( fileMenu );
    }
}

void MainWindow::setEditorsReadOnly( bool b )
{
    editorsReadOnly = b;
}

void MainWindow::setPluginDirectory( const TQString &pd )
{
    pluginDir = pd;
    if ( !qwf_plugin_dir )
	qwf_plugin_dir = new TQString( pd );
    else
	*qwf_plugin_dir = pd;
}

void MainWindow::toggleSignalHandlers( bool show )
{
    if ( sSignalHandlers == show )
	return;
    sSignalHandlers = show;
    propertyEditor->setSignalHandlersEnabled( show );
}
