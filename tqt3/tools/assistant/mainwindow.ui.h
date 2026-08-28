/**********************************************************************
** Copyright (C) 2000-2008 Trolltech ASA.  All rights reserved.
**
** This file is part of the TQt Assistant.
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

#include "config.h"

#include <ntqtabwidget.h>
#include <ntqfileinfo.h>
#include <ntqaccel.h>
#include <ntqobjectlist.h>
#include <ntqtimer.h>
#include <ntqdragobject.h>
#include <ntqfontinfo.h>
#include <ntqaccel.h>
#include <ntqmetaobject.h>
#include <ntqeventloop.h>

TQPtrList<MainWindow> *MainWindow::windows = 0;

#if defined(TQ_WS_WIN)
extern TQ_EXPORT int tqt_ntfs_permission_lookup;
#endif

void MainWindow::init()
{
#if defined(TQ_WS_WIN)
    // Workaround for TQMimeSourceFactory failing in TQFileInfo::isReadable() for
    // certain user configs. See task: 34372
    tqt_ntfs_permission_lookup = 0;
#endif
    setupCompleted = false;

    goActions = new TQPtrList<TQAction>;
    goActionDocFiles = new TQMap<TQAction*,TQString>;
    goActions->setAutoDelete( true );

    if ( !windows )
	windows = new TQPtrList<MainWindow>;
    windows->append( this );
    tabs = new TabbedBrowser( this, "qt_assistant_tabbedbrowser" );
    setCentralWidget( tabs );
    settingsDia = 0;

    Config *config = Config::configuration();

    updateProfileSettings();

    dw = new TQDockWindow( TQDockWindow::InDock, this );
    helpDock = new HelpDialog( dw, this );
    dw->setResizeEnabled( true );
    dw->setCloseMode( TQDockWindow::Always );
    addDockWindow( dw, DockLeft );
    dw->setWidget( helpDock );
    dw->setCaption( "Sidebar" );
    dw->setFixedExtentWidth( 320 );

    // read geometry configuration
    setupGoActions();

    if ( !config->isMaximized() ) {
	TQRect geom = config->geometry();
	if( geom.isValid() ) {
	    resize(geom.size());
	    move(geom.topLeft());
	}
    }

    TQString mainWindowLayout = config->mainWindowLayout();

    TQTextStream ts( &mainWindowLayout, IO_ReadOnly );
    ts >> *this;

    if ( config->sideBarHidden() )
	dw->hide();

    tabs->setup();
    TQTimer::singleShot( 0, this, TQ_SLOT( setup() ) );
#if defined(Q_OS_MACX)
    // Use the same forward and backward browser shortcuts as Safari and Internet Explorer do
    // on the Mac. This means that if you have access to one of those cool Intellimice, the thing
    // works just fine, since that's how Microsoft hacked it.
    actionGoPrevious->setAccel(TQKeySequence(TQt::CTRL|TQt::Key_Left));
    actionGoNext->setAccel(TQKeySequence(TQt::CTRL|TQt::Key_Right));
#endif
}

void MainWindow::setup()
{
    if( setupCompleted )
	return;

    tqApp->setOverrideCursor( TQCursor( TQt::WaitCursor ) );
    statusBar()->message( tr( "Initializing TQt Assistant..." ) );
    setupCompleted = true;
    helpDock->initialize();
    connect( actionGoPrevious, TQ_SIGNAL( activated() ), tabs, TQ_SLOT( backward() ) );
    connect( actionGoNext, TQ_SIGNAL( activated() ), tabs, TQ_SLOT( forward() ) );
    connect( actionEditCopy, TQ_SIGNAL( activated() ), tabs, TQ_SLOT( copy() ) );
    connect( actionFileExit, TQ_SIGNAL( activated() ), tqApp, TQ_SLOT( closeAllWindows() ) );
    connect( actionAddBookmark, TQ_SIGNAL( activated() ),
	     helpDock, TQ_SLOT( addBookmark() ) );
    connect( helpDock, TQ_SIGNAL( showLink( const TQString& ) ),
	     this, TQ_SLOT( showLink( const TQString& ) ) );
    connect( helpDock, TQ_SIGNAL( showSearchLink( const TQString&, const TQStringList& ) ),
	     this, TQ_SLOT( showSearchLink( const TQString&, const TQStringList&) ) );

    connect( bookmarkMenu, TQ_SIGNAL( activated( int ) ),
	     this, TQ_SLOT( showBookmark( int ) ) );
    connect( actionZoomIn, TQ_SIGNAL( activated() ), tabs, TQ_SLOT( zoomIn() ) );
    connect( actionZoomOut, TQ_SIGNAL( activated() ), tabs, TQ_SLOT( zoomOut() ) );

    connect( actionOpenPage, TQ_SIGNAL( activated() ), tabs, TQ_SLOT( newTab() ) );
    connect( actionClosePage, TQ_SIGNAL( activated() ), tabs, TQ_SLOT( closeTab() ) );
    connect( actionNextPage, TQ_SIGNAL( activated() ), tabs, TQ_SLOT( nextTab() ) );
    connect( actionPrevPage, TQ_SIGNAL( activated() ), tabs, TQ_SLOT( previousTab() ) );



#if defined(Q_OS_WIN32) || defined(Q_OS_WIN64)
    TQAccel *acc = new TQAccel( this );
//     acc->connectItem( acc->insertItem( Key_F5 ), browser, TQ_SLOT( reload() ) );
    acc->connectItem( acc->insertItem( TQKeySequence("SHIFT+CTRL+=") ), actionZoomIn, TQ_SIGNAL(activated()) );
#endif

    TQAccel *a = new TQAccel( this, dw );
    a->connectItem( a->insertItem( TQAccel::stringToKey( tr("Ctrl+T") ) ),
		    helpDock, TQ_SLOT( toggleContents() ) );
    a->connectItem( a->insertItem( TQAccel::stringToKey( tr("Ctrl+I") ) ),
		    helpDock, TQ_SLOT( toggleIndex() ) );
    a->connectItem( a->insertItem( TQAccel::stringToKey( tr("Ctrl+B") ) ),
		    helpDock, TQ_SLOT( toggleBookmarks() ) );
    a->connectItem( a->insertItem( TQAccel::stringToKey( tr("Ctrl+S") ) ),
		    helpDock, TQ_SLOT( toggleSearch() ) );

    Config *config = Config::configuration();

    setupBookmarkMenu();
    PopupMenu->insertItem( tr( "Vie&ws" ), createDockWindowMenu() );
    helpDock->tabWidget->setCurrentPage( config->sideBarPage() );

    tqApp->restoreOverrideCursor();
    actionGoPrevious->setEnabled( false );
    actionGoNext->setEnabled( false );
}

void MainWindow::setupGoActions()
{
    Config *config = Config::configuration();
    TQStringList titles = config->docTitles();
    TQAction *action = 0;

    static bool separatorInserted = false;

    TQAction *cur = goActions->first();
    while( cur ) {
	cur->removeFrom( goMenu );
	cur->removeFrom( goActionToolbar );
	cur = goActions->next();
    }
    goActions->clear();
    goActionDocFiles->clear();

    int addCount = 0;

    TQStringList::ConstIterator it = titles.begin();
    for ( ; it != titles.end(); ++it ) {
	TQString title = *it;
	TQPixmap pix = config->docIcon( title );
	if( !pix.isNull() ) {
	    if( !separatorInserted ) {
		goMenu->insertSeparator();
		separatorInserted = true;
	    }
	    action = new TQAction( title, TQIconSet( pix ), title, 0, 0 );
	    action->addTo( goMenu );
	    action->addTo( goActionToolbar );
	    goActions->append( action );
	    goActionDocFiles->insert( action, config->indexPage( title ) );
	    connect( action, TQ_SIGNAL( activated() ),
		     this, TQ_SLOT( showGoActionLink() ) );
	    ++addCount;
	}
    }
    if( !addCount )
	goActionToolbar->hide();
    else
	goActionToolbar->show();

}

void MainWindow::browserTabChanged()
{
    if (tabs->currentBrowser()) {
        actionGoPrevious->setEnabled(tabs->currentBrowser()->isBackwardAvailable());
        actionGoNext->setEnabled(tabs->currentBrowser()->isForwardAvailable());
    }
}

bool MainWindow::insertActionSeparator()
{
    goMenu->insertSeparator();
    Toolbar->addSeparator();
    return true;
}

bool MainWindow::close( bool alsoDelete )
{
    saveSettings();
    return TQMainWindow::close( alsoDelete );
}

void MainWindow::destroy()
{
    windows->removeRef( this );
    if ( windows->isEmpty() ) {
	delete windows;
	windows = 0;
    }
    delete goActions;
    delete goActionDocFiles;
}

void MainWindow::about()
{
    TQMessageBox box( this );
    box.setText( "<center><img src=\"splash.png\">"
		 "<p>Version " + TQString(TQT_VERSION_STR) + "</p>"
		 "<p>Copyright (C) 2000-2008 Trolltech ASA. All rights reserved."
		 "</p></center><p></p>"
		 "<p>TQt Commercial Edition license holders: This program is"
		 " licensed to you under the terms of the TQt Commercial License"
		 " Agreement. For details, see the file LICENSE that came with"
		 " this software distribution.</p><p></p>"
		 "<p>TQt Open Source Edition users: This program is licensed to you"
		 " under the terms of the GNU General Public License Version 2."
		 " For details, see the file LICENSE.GPL that came with this"
		 " software distribution.</p><p>The program is provided AS IS"
		 " with NO WARRANTY OF ANY KIND, INCLUDING THE WARRANTY OF"
		 " DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE."
		 "</p>" );
    box.setCaption( tr( "TQt Assistant" ) );
    box.setIcon( TQMessageBox::NoIcon );
    box.exec();
}

void MainWindow::aboutApplication()
{
    TQString url = Config::configuration()->aboutURL();
    if ( url == "about_qt" ) {
	TQMessageBox::aboutTQt( this, "TQt Assistant" );
	return;
    }
    TQString text;
    TQFile file( url );
    if( file.exists() && file.open( IO_ReadOnly ) )
	text = TQString( file.readAll() );
    if( text.isNull() )
	text = tr( "Failed to open about application contents in file: '%1'" ).arg( url );

    TQMessageBox box( this );
    box.setText( text );
    box.setCaption( Config::configuration()->aboutApplicationMenuText() );
    box.setIcon( TQMessageBox::NoIcon );
    box.exec();
}

void MainWindow::find()
{
    if ( !findDialog )
	findDialog = new FindDialog( this );
    findDialog->comboFind->setFocus();
    findDialog->comboFind->lineEdit()->setSelection(
        0, findDialog->comboFind->lineEdit()->text().length() );
    findDialog->show();
}

void MainWindow::findAgain()
{
    if (!findDialog || !findDialog->hasFindExpression()) {
	find();
	return;
    }
    findDialog->doFind(true);
}

void MainWindow::findAgainPrev()
{
    if (!findDialog || !findDialog->hasFindExpression()) {
	find();
	return;
    }
    findDialog->doFind(false);
}

void MainWindow::goHome()
{
    showLink( Config::configuration()->homePage() );
}

void MainWindow::print()
{
    TQPrinter printer( TQPrinter::HighResolution );
    printer.setFullPage( true );
    if ( printer.setup( this ) ) {
	TQPainter p;
	if ( !p.begin( &printer ) )
	    return;

	tqApp->setOverrideCursor( TQCursor( TQt::WaitCursor ) );
	tqApp->eventLoop()->processEvents( TQEventLoop::ExcludeUserInput );

	TQPaintDeviceMetrics metrics(p.device());
	TQTextBrowser *browser = tabs->currentBrowser();
	int dpiy = metrics.logicalDpiY();
	int margin = (int) ( (2/2.54)*dpiy );
	TQRect view( margin,
		    margin,
		    metrics.width() - 2 * margin,
		    metrics.height() - 2 * margin );
	TQSimpleRichText richText( browser->text(), browser->TQWidget::font(), browser->context(),
				  browser->styleSheet(), browser->mimeSourceFactory(),
				  view.height(), TQt::black, false );
	richText.setWidth( &p, view.width() );
	int page = 1;
	do {
	    tqApp->eventLoop()->processEvents( TQEventLoop::ExcludeUserInput );

	    richText.draw( &p, margin, margin, view, palette().active() );
	    view.moveBy( 0, view.height() );
	    p.translate( 0 , -view.height() );
	    p.drawText( view.right() - p.fontMetrics().width( TQString::number(page) ),
			view.bottom() + p.fontMetrics().ascent() + 5, TQString::number(page) );
	    if ( view.top() - margin >= richText.height() )
		break;
	    printer.newPage();
	    page++;
	} while (true);

	tqApp->eventLoop()->processEvents( TQEventLoop::ExcludeUserInput );
	tqApp->restoreOverrideCursor();
    }
}

void MainWindow::updateBookmarkMenu()
{
    for ( MainWindow *mw = windows->first(); mw; mw = windows->next() )
	mw->setupBookmarkMenu();
}

void MainWindow::setupBookmarkMenu()
{
    bookmarkMenu->clear();
    bookmarks.clear();
    actionAddBookmark->addTo( bookmarkMenu );

    TQFile f( TQDir::homeDirPath() + "/.assistant/bookmarks." +
	Config::configuration()->profileName() );
    if ( !f.open( IO_ReadOnly ) )
	return;
    TQTextStream ts( &f );
    bookmarkMenu->insertSeparator();
    while ( !ts.atEnd() ) {
	TQString title = ts.readLine();
	TQString link = ts.readLine();
	bookmarks.insert( bookmarkMenu->insertItem( title ), link );
    }
}

void MainWindow::showBookmark( int id )
{
    if ( bookmarks.find( id ) != bookmarks.end() )
	showLink( *bookmarks.find( id ) );
}

void MainWindow::showLinkFromClient( const TQString &link )
{
    setWindowState(windowState() & ~WindowMinimized);
    raise();
    setActiveWindow();
    showLink( link );
}

void MainWindow::showLink( const TQString &link )
{
    if( link.isEmpty() ) {
	tqWarning( "The link is empty!" );
    }

    int find = link.find( '#' );
    TQString name = find >= 0 ? link.left( find ) : link;

    TQString absLink = link;
    TQFileInfo fi( name );
    if ( fi.isRelative() ) {
	if ( find >= 0 )
	    absLink = fi.absFilePath() + link.right( link.length() - find );
	else
	    absLink = fi.absFilePath();
    }
    if( fi.exists() ) {
	tabs->setSource( absLink );
	tabs->currentBrowser()->setFocus();
    } else {
	// ### Default 404 site!
	statusBar()->message( tr( "Failed to open link: '%1'" ).arg( link ), 5000 );
	tabs->currentBrowser()->setText( tr( "<div align=\"center\"><h1>The page could not be found!</h1><br>"
					     "<h3>'%1'</h3></div>").arg( link ) );
	tabs->updateTitle( tr( "Error..." ) );
    }
}

void MainWindow::showLinks( const TQStringList &links )
{
    if ( links.size() == 0 ) {
	tqWarning( "MainWindow::showLinks() - Empty link" );
	return;
    }

    if ( links.size() == 1 ) {
	showLink( links.first() );
	return;
    }

    pendingLinks = links;

    TQStringList::ConstIterator it = pendingLinks.begin();
    // Initial showing, The tab is empty so update that without creating it first
    if ( tabs->currentBrowser()->source().isEmpty() ) {
	pendingBrowsers.append(tabs->currentBrowser());
	tabs->setTitle(tabs->currentBrowser(), pendingLinks.first());
    }
    ++it;

    while( it != pendingLinks.end() ) {
	pendingBrowsers.append( tabs->newBackgroundTab(*it) );
	++it;
    }

    startTimer(50);
    return;
}

void MainWindow::timerEvent(TQTimerEvent *e)
{
    TQString link = pendingLinks.first();
    HelpWindow *win = pendingBrowsers.first();
    pendingLinks.pop_front();
    pendingBrowsers.removeFirst();
    if (pendingLinks.size() == 0)
	killTimer(e->timerId());
    win->setSource(link);
}

void MainWindow::showTQtHelp()
{
    showLink( TQString( tqInstallPathDocs() ) + "/html/index.html" );
}

void MainWindow::showSettingsDialog()
{
    showSettingsDialog( -1 );
}

void MainWindow::showWebBrowserSettings()
{
    showSettingsDialog( 1 );
}

void MainWindow::showSettingsDialog( int page )
{
    if ( !settingsDia ){
	settingsDia = new SettingsDialog( this );
    }
    TQFontDatabase fonts;
    settingsDia->fontCombo->clear();
    settingsDia->fontCombo->insertStringList( fonts.families() );
    settingsDia->fontCombo->lineEdit()->setText( tabs->browserFont().family() );
    settingsDia->fixedfontCombo->clear();
    settingsDia->fixedfontCombo->insertStringList( fonts.families() );
    settingsDia->fixedfontCombo->lineEdit()->setText( tabs->styleSheet()->item( "pre" )->fontFamily() );
    settingsDia->linkUnderlineCB->setChecked( tabs->linkUnderline() );
    settingsDia->colorButton->setPaletteBackgroundColor( tabs->palette().color( TQPalette::Active, TQColorGroup::Link ) );
    if ( page != -1 )
	settingsDia->settingsTab->setCurrentPage( page );

    int ret = settingsDia->exec();

    if ( ret != TQDialog::Accepted )
	return;

    TQObjectList *lst = (TQObjectList*)Toolbar->children();
    TQObject *obj;
    for ( obj = lst->last(); obj; obj = lst->prev() ) {
	if ( obj->isA( "TQToolBarSeparator" ) ) {
	    delete obj;
	    obj = 0;
	    break;
	}
    }

    setupGoActions();

    TQFont fnt( tabs->browserFont() );
    fnt.setFamily( settingsDia->fontCombo->currentText() );
    tabs->setBrowserFont( fnt );
    tabs->setLinkUnderline( settingsDia->linkUnderlineCB->isChecked() );

    TQPalette pal = tabs->palette();
    TQColor lc = settingsDia->colorButton->paletteBackgroundColor();
    pal.setColor( TQPalette::Active, TQColorGroup::Link, lc );
    pal.setColor( TQPalette::Inactive, TQColorGroup::Link, lc );
    pal.setColor( TQPalette::Disabled, TQColorGroup::Link, lc );
    tabs->setPalette( pal );

    TQString family = settingsDia->fixedfontCombo->currentText();

    TQStyleSheet *sh = tabs->styleSheet();
    sh->item( "pre" )->setFontFamily( family );
    sh->item( "code" )->setFontFamily( family );
    sh->item( "tt" )->setFontFamily( family );
    tabs->currentBrowser()->setText( tabs->currentBrowser()->text() );
    showLink( tabs->currentBrowser()->source() );
}

void MainWindow::hide()
{
    saveToolbarSettings();
    TQMainWindow::hide();
}

MainWindow* MainWindow::newWindow()
{
    saveSettings();
    saveToolbarSettings();
    MainWindow *mw = new MainWindow;
    mw->move( geometry().topLeft() );
    if ( isMaximized() )
	mw->showMaximized();
    else
	mw->show();
    mw->goHome();
    return mw;
}

void MainWindow::saveSettings()
{
    Config *config = Config::configuration();
    config->setFontFamily( tabs->browserFont().family() );
    config->setFontSize( tabs->currentBrowser()->font().pointSize() );
    config->setFontFixedFamily( tabs->styleSheet()->item( "pre" )->fontFamily() );
    config->setLinkUnderline( tabs->linkUnderline() );
    config->setLinkColor( tabs->palette().color( TQPalette::Active, TQColorGroup::Link ).name() );
    config->setSideBarPage( helpDock->tabWidget->currentPageIndex() );
    config->setGeometry( TQRect( x(), y(), width(), height() ) );
    config->setMaximized( isMaximized() );

    // Create list of the tab urls
    TQStringList lst;
    TQPtrList<HelpWindow> browsers = tabs->browsers();
    HelpWindow *browser = browsers.first();
    while (browser) {
	lst << browser->source();
	browser = browsers.next();
    }
    config->setSource(lst);
    config->save();
}

void MainWindow::saveToolbarSettings()
{
    TQString mainWindowLayout;
    TQTextStream ts( &mainWindowLayout, IO_WriteOnly );
    ts << *this;
    Config::configuration()->setMainWindowLayout( mainWindowLayout );
}

TabbedBrowser* MainWindow::browsers()
{
    return tabs;
}

void MainWindow::showSearchLink( const TQString &link, const TQStringList &terms )
{
    HelpWindow * hw = tabs->currentBrowser();
    hw->blockScrolling( true );
    hw->setCursor( waitCursor );
    if ( hw->source() == link )
	hw->reload();
    else
	showLink( link );
    hw->sync();
    hw->setCursor( arrowCursor );

    hw->viewport()->setUpdatesEnabled( false );
    int minPar = INT_MAX;
    int minIndex = INT_MAX;
    TQStringList::ConstIterator it = terms.begin();
    for ( ; it != terms.end(); ++it ) {
	int para = 0;
	int index = 0;
	bool found = hw->find( *it, false, true, true, &para, &index );
	while ( found ) {
	    if ( para < minPar ) {
		minPar = para;
		minIndex = index;
	    }
	    hw->setColor( red );
	    found = hw->find( *it, false, true, true );
	}
    }
    hw->blockScrolling( false );
    hw->viewport()->setUpdatesEnabled( true );
    hw->setCursorPosition( minPar, minIndex );
    hw->updateContents();
}


void MainWindow::showGoActionLink()
{
    const TQObject *origin = sender();
    if( !origin ||
	origin->metaObject()->className() != TQString( "TQAction" ) )
	return;

    TQAction *action = (TQAction*) origin;
    TQString docfile = *( goActionDocFiles->find( action ) );
    showLink( docfile );
}

void MainWindow::showAssistantHelp()
{
    showLink( Config::configuration()->assistantDocPath() + "/assistant.html" );
}

HelpDialog* MainWindow::helpDialog()
{
    return helpDock;
}

void MainWindow::backwardAvailable( bool enable )
{
    actionGoPrevious->setEnabled( enable );
}

void MainWindow::forwardAvailable( bool enable )
{
    actionGoNext->setEnabled( enable );
}

void MainWindow::updateProfileSettings()
{
    Config *config = Config::configuration();
#ifndef TQ_WS_MACX
    setIcon( config->applicationIcon() );
#endif
    helpMenu->clear();
    actionHelpAssistant->addTo( helpMenu );
    helpMenu->insertSeparator();
    helpAbout_TQt_AssistantAction->addTo( helpMenu );
    if ( !config->aboutApplicationMenuText().isEmpty() )
	actionAboutApplication->addTo( helpMenu );
    helpMenu->insertSeparator();
    actionHelpWhatsThis->addTo( helpMenu );

    actionAboutApplication->setMenuText( config->aboutApplicationMenuText() );

    if( !config->title().isNull() )
	setCaption( config->title() );
}
