/**********************************************************************
** Copyright (C) 2000-2008 Trolltech ASA.  All rights reserved.
**
** This file is part of TQt Assistant.
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

#include "helpdialogimpl.h"
#include "helpwindow.h"
#include "topicchooserimpl.h"
#include "docuparser.h"
#include "mainwindow.h"
#include "config.h"
#include "tabbedbrowser.h"

#include <ntqaccel.h>
#include <ntqapplication.h>
#include <ntqcursor.h>
#include <ntqdir.h>
#include <ntqeventloop.h>
#include <ntqfile.h>
#include <ntqfileinfo.h>
#include <ntqheader.h>
#include <ntqlabel.h>
#include <ntqlineedit.h>
#include <ntqmessagebox.h>
#include <ntqpixmap.h>
#include <ntqprogressbar.h>
#include <ntqptrlist.h>
#include <ntqptrstack.h>
#include <ntqpushbutton.h>
#include <ntqregexp.h>
#include <ntqsettings.h>
#include <ntqstatusbar.h>
#include <ntqtabwidget.h>
#include <ntqtextbrowser.h>
#include <ntqtextstream.h>
#include <ntqtimer.h>
#include <ntqurl.h>
#include <ntqvalidator.h>

#include <stdlib.h>
#include <limits.h>

static TQString stripAmpersand( const TQString &str )
{
    TQString s( str );
    s = s.replace( '&', "" );
    return s;
}

static bool verifyDirectory(const TQString &str)
{
    TQFileInfo dirInfo(str);
    if (!dirInfo.exists())
	return TQDir().mkdir(str);
    if (!dirInfo.isDir()) {
	tqWarning("'%s' exists but is not a directory", str.latin1());
	return false;
    }
    return true;
}

struct IndexKeyword {
    IndexKeyword( const TQString &kw, const TQString &l )
	: keyword( kw ), link( l ) {}
    IndexKeyword() : keyword( TQString::null ), link( TQString::null ) {}
    bool operator<( const IndexKeyword &ik ) const {
	return keyword.lower() < ik.keyword.lower();
    }
    bool operator<=( const IndexKeyword &ik ) const {
	return keyword.lower() <= ik.keyword.lower();
    }
    bool operator>( const IndexKeyword &ik ) const {
	return keyword.lower() > ik.keyword.lower();
    }
    TQString keyword;
    TQString link;
};

TQDataStream &operator>>( TQDataStream &s, IndexKeyword &ik )
{
    s >> ik.keyword;
    s >> ik.link;
    return s;
}

TQDataStream &operator<<( TQDataStream &s, const IndexKeyword &ik )
{
    s << ik.keyword;
    s << ik.link;
    return s;
}

TQValidator::State SearchValidator::validate( TQString &str, int & ) const
{
    for ( int i = 0; i < (int) str.length(); ++i ) {
	TQChar c = str[i];
	if ( !c.isLetterOrNumber() && c != '\'' && c != '`'
	    && c != '\"' && c != ' ' && c != '-' && c != '_'
	    && c!= '*' )
	    return TQValidator::Invalid;
    }
    return TQValidator::Acceptable;
}

HelpNavigationListItem::HelpNavigationListItem( TQListBox *ls, const TQString &txt )
    : TQListBoxText( ls, txt )
{
}

void HelpNavigationListItem::addLink( const TQString &link )
{
    int hash = link.find( '#' );
    if ( hash == -1 ) {
	linkList << link;
	return;
    }

    TQString preHash = link.left( hash );
    if ( linkList.grep( preHash, false ).count() > 0 )
	return;
    linkList << link;
}

HelpNavigationContentsItem::HelpNavigationContentsItem( TQListView *v, TQListViewItem *after )
    : TQListViewItem( v, after )
{
}

HelpNavigationContentsItem::HelpNavigationContentsItem( TQListViewItem *v, TQListViewItem *after )
    : TQListViewItem( v, after )
{
}

void HelpNavigationContentsItem::setLink( const TQString &lnk )
{
    theLink = lnk;
}

TQString HelpNavigationContentsItem::link() const
{
    return theLink;
}



HelpDialog::HelpDialog( TQWidget *parent, MainWindow *h )
    : HelpDialogBase( parent, 0, false ),  lwClosed( false ), help( h )
{
}

void HelpDialog::initialize()
{
    connect( tabWidget, TQ_SIGNAL( selected(const TQString&) ),
	     this, TQ_SLOT( currentTabChanged(const TQString&) ) );
    connect( listContents, TQ_SIGNAL( mouseButtonClicked(int, TQListViewItem*, const TQPoint &, int ) ),
	     this, TQ_SLOT( showTopic(int,TQListViewItem*, const TQPoint &) ) );
    connect( listContents, TQ_SIGNAL( currentChanged(TQListViewItem*) ),
	     this, TQ_SLOT( currentContentsChanged(TQListViewItem*) ) );
    connect( listContents, TQ_SIGNAL( selectionChanged(TQListViewItem*) ),
	     this, TQ_SLOT( currentContentsChanged(TQListViewItem*) ) );
    connect( listContents, TQ_SIGNAL( doubleClicked(TQListViewItem*) ),
	     this, TQ_SLOT( showTopic(TQListViewItem*) ) );
    connect( listContents, TQ_SIGNAL( returnPressed(TQListViewItem*) ),
	     this, TQ_SLOT( showTopic(TQListViewItem*) ) );
    connect( listContents, TQ_SIGNAL( contextMenuRequested( TQListViewItem*, const TQPoint&, int ) ),
	     this, TQ_SLOT( showItemMenu( TQListViewItem*, const TQPoint& ) ) );
    connect( editIndex, TQ_SIGNAL( returnPressed() ),
	     this, TQ_SLOT( showTopic() ) );
    connect( editIndex, TQ_SIGNAL( textChanged(const TQString&) ),
	     this, TQ_SLOT( searchInIndex(const TQString&) ) );

    connect( listIndex, TQ_SIGNAL( selectionChanged(TQListBoxItem*) ),
	     this, TQ_SLOT( currentIndexChanged(TQListBoxItem*) ) );
    connect( listIndex, TQ_SIGNAL( returnPressed(TQListBoxItem*) ),
	     this, TQ_SLOT( showTopic() ) );
    connect( listIndex, TQ_SIGNAL( mouseButtonClicked(int, TQListBoxItem*, const TQPoint &) ),
	     this, TQ_SLOT( showTopic( int, TQListBoxItem *, const TQPoint & ) ) );
    connect( listIndex, TQ_SIGNAL( currentChanged(TQListBoxItem*) ),
	     this, TQ_SLOT( currentIndexChanged(TQListBoxItem*) ) );
    connect( listIndex, TQ_SIGNAL( contextMenuRequested( TQListBoxItem*, const TQPoint& ) ),
	     this, TQ_SLOT( showItemMenu( TQListBoxItem*, const TQPoint& ) ) );

    connect( listBookmarks, TQ_SIGNAL( mouseButtonClicked(int, TQListViewItem*, const TQPoint&, int ) ),
	     this, TQ_SLOT( showTopic(int, TQListViewItem*, const TQPoint &) ) );
    connect( listBookmarks, TQ_SIGNAL( returnPressed(TQListViewItem*) ),
	     this, TQ_SLOT( showTopic(TQListViewItem*) ) );
    connect( listBookmarks, TQ_SIGNAL( selectionChanged(TQListViewItem*) ),
	     this, TQ_SLOT( currentBookmarkChanged(TQListViewItem*) ) );
    connect( listBookmarks, TQ_SIGNAL( currentChanged(TQListViewItem*) ),
	     this, TQ_SLOT( currentBookmarkChanged(TQListViewItem*) ) );
    connect( listBookmarks, TQ_SIGNAL( contextMenuRequested( TQListViewItem*, const TQPoint&, int ) ),
	     this, TQ_SLOT( showItemMenu( TQListViewItem*, const TQPoint& ) ) );
    connect( resultBox, TQ_SIGNAL( contextMenuRequested( TQListBoxItem*, const TQPoint& ) ),
	     this, TQ_SLOT( showItemMenu( TQListBoxItem*, const TQPoint& ) ) );

    cacheFilesPath = TQDir::homeDirPath() + "/.assistant/"; //### Find a better location for the dbs

    editIndex->installEventFilter( this );
    listBookmarks->header()->hide();
    listBookmarks->header()->setStretchEnabled( true );
    listContents->header()->hide();
    listContents->header()->setStretchEnabled( true );
    framePrepare->hide();
    connect( tqApp, TQ_SIGNAL(lastWindowClosed()), TQ_SLOT(lastWinClosed()) );

    termsEdit->setValidator( new SearchValidator( termsEdit ) );

    itemPopup = new TQPopupMenu( this );
    itemPopup->insertItem( tr( "Open Link in Current Tab" ), 0 );
    itemPopup->insertItem( tr( "Open Link in New Window" ), 1 );
    itemPopup->insertItem( tr( "Open Link in New Tab" ), 2 );

    contentList.setAutoDelete( true );
    contentList.clear();

    initDoneMsgShown = false;
    fullTextIndex = 0;
    indexDone = false;
    titleMapDone = false;
    contentsInserted = false;
    bookmarksInserted = false;
    setupTitleMap();
}


void HelpDialog::processEvents()
{
    tqApp->eventLoop()->processEvents( TQEventLoop::ExcludeUserInput );
}


void HelpDialog::lastWinClosed()
{
    lwClosed = true;
}


void HelpDialog::removeOldCacheFiles()
{
    TQString dir = cacheFilesPath; // ### remove the last '/' ?
    if (!verifyDirectory(cacheFilesPath)) {
	tqWarning( "Failed to created assistant directory" );
	return;
    }
    TQString pname = "." + Config::configuration()->profileName();

    TQStringList fileList;
    fileList <<  "indexdb" << "indexdb.dict" << "indexdb.doc" << "contentdb";
    TQStringList::iterator it = fileList.begin();
    for ( ; it != fileList.end(); ++it ) {
	if ( TQFile::exists( cacheFilesPath + *it + pname ) ) {
	    TQFile f( cacheFilesPath + *it + pname );
	    f.remove();
	}
    }
}

void HelpDialog::timerEvent(TQTimerEvent *e)
{
    static int opacity = 255;
    // To allow patching of 3.2.3 assistant for qsa.
#if TQT_VERSION >= 0x030300
    help->setWindowOpacity((opacity-=4)/255.0);
    if (opacity<=0)
#endif
	tqApp->quit();
}


void HelpDialog::loadIndexFile()
{
    if ( indexDone )
	return;

    setCursor( waitCursor );
    indexDone = true;
    labelPrepare->setText( tr( "Prepare..." ) );
    framePrepare->show();
    processEvents();

    TQProgressBar *bar = progressPrepare;
    bar->setTotalSteps( 100 );
    bar->setProgress( 0 );

    keywordDocuments.clear();
    TQValueList<IndexKeyword> lst;
    TQFile indexFile( cacheFilesPath + "indexdb." +
		     Config::configuration()->profileName() );
    if ( !indexFile.open( IO_ReadOnly ) ) {
	buildKeywordDB();
	processEvents();
	if( lwClosed )
	    return;
	if (!indexFile.open(IO_ReadOnly)) {
	    TQMessageBox::warning(help, tr("TQt Assistant"), tr("Failed to load keyword index file\n"
							      "Assistant will not work!"));
#if defined TQ_WS_WIN || defined TQ_WS_MACX
	    startTimer(50);
#endif
	    return;
	}
    }

    editIndex->setEnabled(false);

    TQDataStream ds( &indexFile );
    TQ_UINT32 fileAges;
    ds >> fileAges;
    if ( fileAges != getFileAges() ) {
	indexFile.close();
	buildKeywordDB();
	if ( !indexFile.open( IO_ReadOnly ) ) {
	    TQMessageBox::warning( help, tr( "TQt Assistant" ),
		tr( "Cannot open the index file %1" ).arg( TQFileInfo( indexFile ).absFilePath() ) );
	    editIndex->setEnabled(true);
	    return;
	}
	ds.setDevice( &indexFile );
	ds >> fileAges;
    }
    ds >> lst;
    indexFile.close();

    bar->setProgress( bar->totalSteps() );
    processEvents();

    listIndex->clear();
    HelpNavigationListItem *lastItem = 0;
    TQString lastKeyword = TQString::null;
    TQValueList<IndexKeyword>::ConstIterator it = lst.begin();
    for ( ; it != lst.end(); ++it ) {
        if ( lastKeyword.lower() != (*it).keyword.lower() )
            lastItem = new HelpNavigationListItem( listIndex, (*it).keyword );
        lastItem->addLink( (*it).link );
        lastKeyword = (*it).keyword;

        TQString lnk = (*it).link;
        int i = lnk.findRev('#');
        if ( i > -1 )
            lnk = lnk.left( i );
        if (!keywordDocuments.contains(lnk))
            keywordDocuments.append(lnk);
    }
    framePrepare->hide();
    showInitDoneMessage();
    setCursor( arrowCursor );
    editIndex->setEnabled(true);
}

TQ_UINT32 HelpDialog::getFileAges()
{
    TQStringList addDocuFiles = Config::configuration()->docFiles();
    TQStringList::const_iterator i = addDocuFiles.begin();

    TQ_UINT32 fileAges = 0;
    for( ; i != addDocuFiles.end(); ++i ) {
	TQFileInfo fi( *i );
	if ( fi.exists() )
	    fileAges += fi.lastModified().toTime_t();
    }

    return fileAges;
}

void HelpDialog::buildKeywordDB()
{
    TQStringList addDocuFiles = Config::configuration()->docFiles();
    TQStringList::iterator i = addDocuFiles.begin();

    int steps = 0;
    for( ; i != addDocuFiles.end(); i++ )
	steps += TQFileInfo( *i ).size();

    labelPrepare->setText( tr( "Prepare..." ) );
    progressPrepare->setTotalSteps( steps );
    progressPrepare->setProgress( 0 );
    processEvents();

    TQValueList<IndexKeyword> lst;
    TQ_UINT32 fileAges = 0;
    for( i = addDocuFiles.begin(); i != addDocuFiles.end(); i++ ){
	TQFile file( *i );
	if ( !file.exists() ) {
	    TQMessageBox::warning( this, tr( "Warning" ),
		tr( "Documentation file %1 does not exist!\n"
		    "Skipping file." ).arg( TQFileInfo( file ).absFilePath() ) );
	    continue;
        }
	fileAges += TQFileInfo( file ).lastModified().toTime_t();
	DocuParser *handler = DocuParser::createParser( *i );
	bool ok = handler->parse( &file );
	file.close();
	if( !ok ){
	    TQString msg = TQString( "In file %1:\n%2" )
			  .arg( TQFileInfo( file ).absFilePath() )
			  .arg( handler->errorProtocol() );
	    TQMessageBox::critical( this, tr( "Parse Error" ), tr( msg ) );
	    delete handler;
	    continue;
	}

	TQPtrList<IndexItem> indLst = handler->getIndexItems();
	TQPtrListIterator<IndexItem> it( indLst );
	IndexItem *indItem;
	int counter = 0;
	while ( ( indItem = it.current() ) != 0 ) {
	    TQFileInfo fi( indItem->reference );
	    lst.append( IndexKeyword( indItem->keyword, fi.absFilePath() ) );
	    if ( progressPrepare )
		progressPrepare->setProgress( progressPrepare->progress() +
					      int(fi.absFilePath().length() * 1.6) );

	    if( ++counter%100 == 0 ) {
		processEvents();
		if( lwClosed ) {
		    return;
		}
	    }
	    ++it;
	}
	delete handler;
    }
    if ( !lst.isEmpty() )
	qHeapSort( lst );

    TQFile indexout( cacheFilesPath + "indexdb." + Config::configuration()->profileName() );
    if ( verifyDirectory(cacheFilesPath) && indexout.open( IO_WriteOnly ) ) {
	TQDataStream s( &indexout );
	s << fileAges;
	s << lst;
	indexout.close();
    }
}

void HelpDialog::setupTitleMap()
{
    if ( titleMapDone )
	return;
    if ( Config::configuration()->docRebuild() ) {
	removeOldCacheFiles();
	Config::configuration()->setDocRebuild( false );
	Config::configuration()->saveProfile( Config::configuration()->profile() );
    }
    if ( contentList.isEmpty() )
	getAllContents();

    titleMapDone = true;
    titleMap.clear();
    TQDictIterator<ContentList> lstIt( contentList );
    for ( ; lstIt.current(); ++lstIt ) {
	TQValueList<ContentItem> &lst = *(lstIt.current());
	TQValueListConstIterator<ContentItem> it;
	for ( it = lst.begin(); it != lst.end(); ++it ) {
	    TQFileInfo link( (*it).reference.simplifyWhiteSpace() );
	    titleMap[ link.absFilePath() ] = (*it).title.stripWhiteSpace();
	}
    }
    processEvents();
}

void HelpDialog::getAllContents()
{
    TQFile contentFile( cacheFilesPath + "contentdb." + Config::configuration()->profileName() );
    contentList.clear();
    if ( !contentFile.open( IO_ReadOnly ) ) {
	buildContentDict();
	return;
    }

    TQDataStream ds( &contentFile );
    TQ_UINT32 fileAges;
    ds >> fileAges;
    if ( fileAges != getFileAges() ) {
	contentFile.close();
	buildContentDict();
	return;
    }
    TQString key;
    TQValueList<ContentItem> lst;
    while ( !ds.atEnd() ) {
	ds >> key;
	ds >> lst;
	contentList.insert( key, new TQValueList<ContentItem>( lst ) );
    }
    contentFile.close();
    processEvents();

}

void HelpDialog::buildContentDict()
{
    TQStringList docuFiles = Config::configuration()->docFiles();

    TQ_UINT32 fileAges = 0;
    for( TQStringList::iterator it = docuFiles.begin(); it != docuFiles.end(); it++ ) {
	TQFile file( *it );
	if ( !file.exists() ) {
	    TQMessageBox::warning( this, tr( "Warning" ),
	    tr( "Documentation file %1 does not exist!\n"
	        "Skipping file." ).arg( TQFileInfo( file ).absFilePath() ) );
	    continue;
        }
	fileAges += TQFileInfo( file ).lastModified().toTime_t();
	DocuParser *handler = DocuParser::createParser( *it );
	if( !handler ) {
	    TQMessageBox::warning( this, tr( "Warning" ),
	    tr( "Documentation file %1 is not compatible!\n"
	        "Skipping file." ).arg( TQFileInfo( file ).absFilePath() ) );
	    continue;
	}
	bool ok = handler->parse( &file );
	file.close();
	if( ok ) {
	    contentList.insert( *it, new TQValueList<ContentItem>( handler->getContentItems() ) );
	    delete handler;
	} else {
	    TQString msg = TQString( "In file %1:\n%2" )
			  .arg( TQFileInfo( file ).absFilePath() )
			  .arg( handler->errorProtocol() );
	    TQMessageBox::critical( this, tr( "Parse Error" ), tr( msg ) );
	    continue;
	}
    }

    TQString pname = Config::configuration()->profileName();
    TQFile contentOut( cacheFilesPath + "contentdb." + pname );
    TQFile::remove( cacheFilesPath + "indexdb.dict." + pname );
    TQFile::remove( cacheFilesPath + "indexdb.doc." + pname );
    if ( contentOut.open( IO_WriteOnly ) ) {
	TQDataStream s( &contentOut );
	s << fileAges;
	TQDictIterator<ContentList> it( contentList );
	for ( ; it.current(); ++it ) {
	    s << it.currentKey();
	    s << *(it.current());
	}
	contentOut.close();
    }
}

void HelpDialog::currentTabChanged( const TQString &s )
{
    if ( stripAmpersand( s ).contains( tr( "Index" ) ) )
	TQTimer::singleShot( 0, this, TQ_SLOT( loadIndexFile() ) );
    else if ( stripAmpersand( s ).contains( tr( "Bookmarks" ) ) )
	insertBookmarks();
    else if ( stripAmpersand( s ).contains( tr( "Contents" ) ) )
	TQTimer::singleShot( 0, this, TQ_SLOT( insertContents() ) );
    else if ( stripAmpersand( s ).contains( tr( "Search" ) ) )
	TQTimer::singleShot( 0, this, TQ_SLOT( setupFullTextIndex() ) );
}

void HelpDialog::showInitDoneMessage()
{
    if ( initDoneMsgShown )
	return;
    initDoneMsgShown = true;
    help->statusBar()->message( tr( "Done" ), 3000 );
}

void HelpDialog::currentIndexChanged( TQListBoxItem * )
{
}


void HelpDialog::showTopic( int button, TQListBoxItem *item,
			    const TQPoint & )
{
    if( button == LeftButton && item )
	showTopic();
}

void HelpDialog::showTopic( int button, TQListViewItem *item,
			    const TQPoint & )
{
    if( button == LeftButton && item )
	showTopic();
}

void HelpDialog::showTopic( TQListViewItem *item )
{
    if( item )
	showTopic();
}

void HelpDialog::showTopic()
{
    if ( stripAmpersand( tabWidget->tabLabel( tabWidget->currentPage() ) ).contains( tr( "Index" ) ) )
	showIndexTopic();
    else if ( stripAmpersand( tabWidget->tabLabel( tabWidget->currentPage() ) ).contains( tr( "Bookmarks" ) ) )
	showBookmarkTopic();
    else if ( stripAmpersand( tabWidget->tabLabel( tabWidget->currentPage() ) ).contains( tr( "Contents" ) ) )
	showContentsTopic();
}

void HelpDialog::showIndexTopic()
{
    TQListBoxItem *i = listIndex->item( listIndex->currentItem() );
    if ( !i )
	return;

    editIndex->blockSignals( true );
    editIndex->setText( i->text() );
    editIndex->blockSignals( false );

    HelpNavigationListItem *item = (HelpNavigationListItem*)i;

    TQStringList links = item->links();
    if ( links.count() == 1 ) {
	emit showLink( links.first() );
    } else {
	qHeapSort( links );
	TQStringList::Iterator it = links.begin();
	TQStringList linkList;
	TQStringList linkNames;
	for ( ; it != links.end(); ++it ) {
	    linkList << *it;
	    linkNames << titleOfLink( *it );
	}
	TQString link = TopicChooser::getLink( this, linkNames, linkList, i->text() );
	if ( !link.isEmpty() )
	    emit showLink( link );
    }
}

void HelpDialog::searchInIndex( const TQString &s )
{
    TQListBoxItem *i = listIndex->firstItem();
    TQString sl = s.lower();
    while ( i ) {
	TQString t = i->text();
	if ( t.length() >= sl.length() &&
	     i->text().left(s.length()).lower() == sl ) {
	    listIndex->setCurrentItem( i );
	    listIndex->setTopItem(listIndex->index(i));
	    break;
	}
	i = i->next();
    }
}

TQString HelpDialog::titleOfLink( const TQString &link )
{
    TQString s( link );
    s.remove( s.find( '#' ), s.length() );
    s = titleMap[ s ];
    if ( s.isEmpty() )
	return link;
    return s;
}

bool HelpDialog::eventFilter( TQObject * o, TQEvent * e )
{
    if ( !o || !e )
	return true;

    if ( o == editIndex && e->type() == TQEvent::KeyPress ) {
	TQKeyEvent *ke = (TQKeyEvent*)e;
	if ( ke->key() == Key_Up ) {
	    int i = listIndex->currentItem();
	    if ( --i >= 0 ) {
		listIndex->setCurrentItem( i );
		editIndex->blockSignals( true );
		editIndex->setText( listIndex->currentText() );
		editIndex->blockSignals( false );
	    }
	    return true;
	} else if ( ke->key() == Key_Down ) {
	    int i = listIndex->currentItem();
	    if ( ++i < int(listIndex->count()) ) {
		listIndex->setCurrentItem( i );
		editIndex->blockSignals( true );
		editIndex->setText( listIndex->currentText() );
		editIndex->blockSignals( false );
	    }
	    return true;
	} else if ( ke->key() == Key_Next || ke->key() == Key_Prior ) {
	    TQApplication::sendEvent( listIndex, e);
	    editIndex->blockSignals( true );
	    editIndex->setText( listIndex->currentText() );
	    editIndex->blockSignals( false );
	}
    }

    return TQWidget::eventFilter( o, e );
}

void HelpDialog::addBookmark()
{
    if ( !bookmarksInserted )
	insertBookmarks();
    TQString link = TQUrl(  help->browsers()->currentBrowser()->context(),
			  help->browsers()->currentBrowser()->source() ).path();
    TQString title = help->browsers()->currentBrowser()->documentTitle();
    if ( title.isEmpty() )
	title = titleOfLink( link );
    HelpNavigationContentsItem *i = new HelpNavigationContentsItem( listBookmarks, 0 );
    i->setText( 0, title );
    i->setLink( link );
    saveBookmarks();
    help->updateBookmarkMenu();
}

void HelpDialog::removeBookmark()
{
    if ( !listBookmarks->currentItem() )
	return;

    delete listBookmarks->currentItem();
    saveBookmarks();
    if ( listBookmarks->firstChild() ) {
	listBookmarks->setSelected( listBookmarks->firstChild(), true );
    }
    help->updateBookmarkMenu();
}

void HelpDialog::insertBookmarks()
{
    if ( bookmarksInserted )
	return;
    bookmarksInserted = true;
    listBookmarks->clear();
    TQFile f( cacheFilesPath + "bookmarks." + Config::configuration()->profileName() );
    if ( !f.open( IO_ReadOnly ) )
	return;
    TQTextStream ts( &f );
    while ( !ts.atEnd() ) {
	HelpNavigationContentsItem *i = new HelpNavigationContentsItem( listBookmarks, 0 );
	i->setText( 0, ts.readLine() );
	i->setLink( ts.readLine() );
    }
    help->updateBookmarkMenu();
    showInitDoneMessage();
}

void HelpDialog::currentBookmarkChanged( TQListViewItem * )
{
}

void HelpDialog::showBookmarkTopic()
{
    if ( !listBookmarks->currentItem() )
	return;

    HelpNavigationContentsItem *i = (HelpNavigationContentsItem*)listBookmarks->currentItem();
    TQString absPath = "";
    if ( TQFileInfo( i->link() ).isRelative() )
	absPath = documentationPath + "/";
    emit showLink( absPath + i->link() );
}

void HelpDialog::saveBookmarks()
{
    TQFile f( cacheFilesPath + "bookmarks." + Config::configuration()->profileName() );
    if ( !f.open( IO_WriteOnly ) )
	return;
    TQTextStream ts( &f );
    TQListViewItemIterator it( listBookmarks );
    for ( ; it.current(); ++it ) {
	HelpNavigationContentsItem *i = (HelpNavigationContentsItem*)it.current();
	ts << i->text( 0 ) << endl;
	ts << i->link() << endl;
    }
    f.close();
}

void HelpDialog::insertContents()
{
    if ( contentsInserted )
	return;

    if ( contentList.isEmpty() )
	getAllContents();

    contentsInserted = true;
    listContents->clear();
    setCursor( waitCursor );
    if ( !titleMapDone )
	setupTitleMap();

    listContents->setSorting( -1 );

    TQDictIterator<ContentList> lstIt( contentList );
    for ( ; lstIt.current(); ++lstIt ) {
	HelpNavigationContentsItem *newEntry;

	HelpNavigationContentsItem *contentEntry = 0;
	TQPtrStack<HelpNavigationContentsItem> stack;
	stack.clear();
	int depth = 0;
	bool root = false;

	HelpNavigationContentsItem *lastItem[64];
	for( int j = 0; j < 64; ++j )
	    lastItem[j] = 0;


	TQValueList<ContentItem> &lst = *(lstIt.current());
	TQValueListConstIterator<ContentItem> it;
	for( it = lst.begin(); it != lst.end(); ++it ){
	    ContentItem item = *it;
	    if( item.depth == 0 ){
		newEntry = new HelpNavigationContentsItem( listContents, 0 );
		newEntry->setPixmap( 0, TQPixmap::fromMimeSource( "book.png" ) );
		newEntry->setText( 0, item.title );
		newEntry->setLink( item.reference );
		stack.push( newEntry );
		depth = 1;
		root = true;
	    }
	    else{
		if( (item.depth > depth) && root ) {
		    depth = item.depth;
		    stack.push( contentEntry );
		}
		if( item.depth == depth ) {
		    contentEntry = new HelpNavigationContentsItem( stack.top(), lastItem[ depth ] );
		    lastItem[ depth ] = contentEntry;
		    contentEntry->setText( 0, item.title );
		    contentEntry->setLink( item.reference );
		}
		else if( item.depth < depth ) {
		    stack.pop();
		    depth--;
		    item = *(--it);
		}
	    }
	}
	processEvents();
    }
    setCursor( arrowCursor );
    showInitDoneMessage();
}

void HelpDialog::currentContentsChanged( TQListViewItem * )
{
}

void HelpDialog::showContentsTopic()
{
    HelpNavigationContentsItem *i = (HelpNavigationContentsItem*)listContents->currentItem();
    if ( !i )
	return;
    emit showLink( i->link() );
}

void HelpDialog::toggleContents()
{
    if ( !isVisible() || tabWidget->currentPageIndex() != 0 ) {
	tabWidget->setCurrentPage( 0 );
	parentWidget()->show();
    }
    else
	parentWidget()->hide();
}

void HelpDialog::toggleIndex()
{
    if ( !isVisible() || tabWidget->currentPageIndex() != 1 || !editIndex->hasFocus() ) {
	tabWidget->setCurrentPage( 1 );
	parentWidget()->show();
	editIndex->setFocus();
    }
    else
	parentWidget()->hide();
}

void HelpDialog::toggleBookmarks()
{
    if ( !isVisible() || tabWidget->currentPageIndex() != 2 ) {
	tabWidget->setCurrentPage( 2 );
	parentWidget()->show();
    }
    else
	parentWidget()->hide();
}

void HelpDialog::toggleSearch()
{
    if ( !isVisible() || tabWidget->currentPageIndex() != 3 ) {
	tabWidget->setCurrentPage( 3 );
	parentWidget()->show();
    }
    else
	parentWidget()->hide();
}

void HelpDialog::setupFullTextIndex()
{
    if ( fullTextIndex )
	return;

    TQStringList documentList;    
    TQString pname = Config::configuration()->profileName();
    fullTextIndex = new Index( documentList, TQDir::homeDirPath() ); // ### Is this correct ?
    if (!verifyDirectory(cacheFilesPath)) {
        TQMessageBox::warning(help, tr("TQt Assistant"),
            tr("Failed to save fulltext search index\n"
            "Assistant will not work!"));
        return;
    }
    searchButton->setEnabled(false);
    helpButton->setEnabled(false);
    termsEdit->setEnabled(false);

    fullTextIndex->setDictionaryFile( cacheFilesPath + "indexdb.dict." + pname );
    fullTextIndex->setDocListFile( cacheFilesPath + "indexdb.doc." + pname );
    processEvents();

    connect( fullTextIndex, TQ_SIGNAL( indexingProgress( int ) ),
        this, TQ_SLOT( setIndexingProgress( int ) ) );
    TQFile f( cacheFilesPath + "indexdb.dict." + pname );
    if ( !f.exists() ) {
        TQMap<TQString, TQString>::ConstIterator it = titleMap.begin();
        TQString documentName;
        for ( ; it != titleMap.end(); ++it ) {
            documentName = it.key();
            int i = documentName.findRev('#');
            if ( i > -1 )
                documentName = documentName.left( i );

            if (!documentList.contains(documentName))
                documentList << documentName;
        }
        loadIndexFile();
        for (TQStringList::Iterator it2 = keywordDocuments.begin(); it2 != keywordDocuments.end(); ++it2) {
            if (!documentList.contains(*it2))
                documentList << *it2;
        }
        fullTextIndex->setDocList( documentList );

        help->statusBar()->clear();
        setCursor( waitCursor );
        labelPrepare->setText( tr( "Indexing files..." ) );
        progressPrepare->setTotalSteps( 100 );
        progressPrepare->reset();
        progressPrepare->show();
        framePrepare->show();
        processEvents();
        if ( fullTextIndex->makeIndex() != -1 ) {
            fullTextIndex->writeDict();
            progressPrepare->setProgress( 100 );
            framePrepare->hide();
            setCursor( arrowCursor );
            showInitDoneMessage();
            keywordDocuments.clear();
        }
    } else {
        setCursor( waitCursor );
        help->statusBar()->message( tr( "Reading dictionary..." ) );
        processEvents();
        fullTextIndex->readDict();
        help->statusBar()->message( tr( "Done" ), 3000 );
        setCursor( arrowCursor );
    }
    searchButton->setEnabled(true);
    termsEdit->setEnabled(true);
    helpButton->setEnabled(true);
}

void HelpDialog::setIndexingProgress( int prog )
{
    progressPrepare->setProgress( prog );
    processEvents();
}

void HelpDialog::startSearch()
{
    TQString str = termsEdit->text();
    str = str.replace( "\'", "\"" );
    str = str.replace( "`", "\"" );
    TQString buf = str;
    str = str.replace( "-", " " );
    str = str.replace( TQRegExp( "\\s[\\S]?\\s" ), " " );
    terms = TQStringList::split( " ", str );
    TQStringList termSeq;
    TQStringList seqWords;
    TQStringList::iterator it = terms.begin();
    for ( ; it != terms.end(); ++it ) {
	(*it) = (*it).simplifyWhiteSpace();
	(*it) = (*it).lower();
	(*it) = (*it).replace( "\"", "" );
    }
    if ( str.contains( '\"' ) ) {
	if ( (str.contains( '\"' ))%2 == 0 ) {
	    int beg = 0;
	    int end = 0;
	    TQString s;
	    beg = str.find( '\"', beg );
	    while ( beg != -1 ) {
		beg++;
		end = str.find( '\"', beg );
		s = str.mid( beg, end - beg );
		s = s.lower();
		s = s.simplifyWhiteSpace();
		if ( s.contains( '*' ) ) {
		    TQMessageBox::warning( this, tr( "Full Text Search" ),
			tr( "Using a wildcard within phrases is not allowed." ) );
		    return;
		}
		seqWords += TQStringList::split( ' ', s );
		termSeq << s;
		beg = str.find( '\"', end + 1);
	    }
	} else {
	    TQMessageBox::warning( this, tr( "Full Text Search" ),
		tr( "The closing quotation mark is missing." ) );
	    return;
	}
    }
    setCursor( waitCursor );
    foundDocs.clear();
    foundDocs = fullTextIndex->query( terms, termSeq, seqWords );
    TQString msg( TQString( "%1 documents found." ).arg( foundDocs.count() ) );
    help->statusBar()->message( tr( msg ), 3000 );
    resultBox->clear();
    for ( it = foundDocs.begin(); it != foundDocs.end(); ++it )
	resultBox->insertItem( fullTextIndex->getDocumentTitle( *it ) );

    terms.clear();
    bool isPhrase = false;
    TQString s = "";
    for ( int i = 0; i < (int)buf.length(); ++i ) {
	if ( buf[i] == '\"' ) {
	    isPhrase = !isPhrase;
	    s = s.simplifyWhiteSpace();
	    if ( !s.isEmpty() )
		terms << s;
	    s = "";
	} else if ( buf[i] == ' ' && !isPhrase ) {
	    s = s.simplifyWhiteSpace();
	    if ( !s.isEmpty() )
		terms << s;
	    s = "";
	} else
	    s += buf[i];
    }
    if ( !s.isEmpty() )
	terms << s;

    setCursor( arrowCursor );
}

void HelpDialog::showSearchHelp()
{
    emit showLink( Config::configuration()->assistantDocPath() + "/assistant-5.html" );
}

void HelpDialog::showResultPage( int button, TQListBoxItem *i, const TQPoint & )
{
    if( button == LeftButton ) {
	showResultPage( i );
    }
}

void HelpDialog::showResultPage( TQListBoxItem *i )
{
    if( !i )
	return;
    emit showSearchLink( foundDocs[resultBox->index( i )], terms );
}

void HelpDialog::showItemMenu( TQListBoxItem *item, const TQPoint &pos )
{
    if ( !item )
	return;
    int id = itemPopup->exec( pos );
    if ( id == 0 ) {
	if ( stripAmpersand( tabWidget->tabLabel( tabWidget->currentPage() ) ).contains( tr( "Index" ) ) )
	    showTopic();
	else {
	    showResultPage( item );
	}
    } else if ( id > 0 ) {
	HelpWindow *hw = help->browsers()->currentBrowser();
	if ( stripAmpersand( tabWidget->tabLabel( tabWidget->currentPage() ) ).contains( tr( "Index" ) ) ) {
	    editIndex->blockSignals( true );
	    editIndex->setText( item->text() );
	    editIndex->blockSignals( false );

	    HelpNavigationListItem *hi = (HelpNavigationListItem*)item;

	    TQStringList links = hi->links();
	    if ( links.count() == 1 ) {
		if ( id == 1 )
		    hw->openLinkInNewWindow( links.first() );
		else
		    hw->openLinkInNewPage( links.first() );
	    } else {
		TQStringList::Iterator it = links.begin();
		TQStringList linkList;
		TQStringList linkNames;
		for ( ; it != links.end(); ++it ) {
		    linkList << *it;
		    linkNames << titleOfLink( *it );
		}
		TQString link = TopicChooser::getLink( this, linkNames, linkList, item->text() );
		if ( !link.isEmpty() ) {
		    if ( id == 1 )
			hw->openLinkInNewWindow( link );
		    else
			hw->openLinkInNewPage( link );
		}
	    }
	} else {
	    TQString link = foundDocs[ resultBox->index( item ) ];
	    if ( id == 1 )
		hw->openLinkInNewWindow( link );
	    else
		hw->openLinkInNewPage( link );
	}
    }
}

void HelpDialog::showItemMenu( TQListViewItem *item, const TQPoint &pos )
{
    if ( !item )
	return;
    int id = itemPopup->exec( pos );
    if ( id == 0 ) {
	if ( stripAmpersand( tabWidget->tabLabel( tabWidget->currentPage() ) ).contains( tr( "Contents" ) ) )
	    showContentsTopic();
	else
	    showBookmarkTopic();
    } else if ( id > 0 ) {
	HelpNavigationContentsItem *i = (HelpNavigationContentsItem*)item;
	if ( id == 1 )
	    help->browsers()->currentBrowser()->openLinkInNewWindow( i->link() );
	else
	    help->browsers()->currentBrowser()->openLinkInNewPage( i->link() );
    }
}
