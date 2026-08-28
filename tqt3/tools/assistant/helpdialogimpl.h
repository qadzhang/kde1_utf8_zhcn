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

#ifndef HELPDIALOGIMPL_H
#define HELPDIALOGIMPL_H

#include "helpdialog.h"
#include "index.h"
#include "helpwindow.h"
#include "docuparser.h"

#include <ntqlistbox.h>
#include <ntqlistview.h>
#include <ntqmap.h>
#include <ntqstringlist.h>
#include <ntqvalidator.h>
#include <ntqpopupmenu.h>
#include <ntqdict.h>
#include <ntqpopupmenu.h>

class TQProgressBar;
class MainWindow;
class TQTextBrowser;

class HelpNavigationListItem : public TQListBoxText
{
public:
    HelpNavigationListItem( TQListBox *ls, const TQString &txt );

    void addLink( const TQString &link );
    TQStringList links() const { return linkList; }
private:
    TQStringList linkList;

};

class SearchValidator : public TQValidator
{
    TQ_OBJECT
public:
    SearchValidator( TQObject *parent, const char *name = 0 )
	: TQValidator( parent, name ) {}
    ~SearchValidator() {}
    TQValidator::State validate( TQString &str, int & ) const;
};

class HelpNavigationContentsItem : public TQListViewItem
{
public:
    HelpNavigationContentsItem( TQListView *v, TQListViewItem *after );
    HelpNavigationContentsItem( TQListViewItem *v, TQListViewItem *after );

    void setLink( const TQString &lnk );
    TQString link() const;

private:
    TQString theLink;
};

class HelpDialog : public HelpDialogBase
{
    TQ_OBJECT

public:
    HelpDialog( TQWidget *parent, MainWindow *h );

    TQString titleOfLink( const TQString &link );
    bool eventFilter( TQObject *, TQEvent * );
    bool lastWindowClosed() { return lwClosed; }

    void timerEvent(TQTimerEvent *e);

protected slots:
    void showTopic( int, TQListBoxItem *, const TQPoint & );
    void showTopic( int, TQListViewItem *, const TQPoint & );
    void showTopic( TQListViewItem * );
    void loadIndexFile();
    void insertContents();
    void setupFullTextIndex();
    void currentTabChanged( const TQString &s );
    void currentIndexChanged( TQListBoxItem *i );
    void showTopic();
    void searchInIndex( const TQString &s );
    void addBookmark();
    void removeBookmark();
    void currentBookmarkChanged( TQListViewItem *i );
    void currentContentsChanged( TQListViewItem *i );
    void startSearch();
    void showSearchHelp();

public slots:
    void initialize();
    void toggleContents();
    void toggleIndex();
    void toggleBookmarks();
    void toggleSearch();


signals:
    void showLink( const TQString &s );
    void showSearchLink( const TQString &s, const TQStringList &terms );

private slots:
    void lastWinClosed();
    void showResultPage( int button, TQListBoxItem *i, const TQPoint &p );
    void showResultPage( TQListBoxItem *i );
    void setIndexingProgress( int prog );
    void showItemMenu( TQListBoxItem *item, const TQPoint &pos );
    void showItemMenu( TQListViewItem *item, const TQPoint &pos );
    void insertBookmarks();
    void processEvents();

private:
    typedef TQValueList<ContentItem> ContentList;
    void removeOldCacheFiles();
    void buildKeywordDB();
    TQ_UINT32 getFileAges();
    void showIndexTopic();
    void showBookmarkTopic();
    void setupTitleMap();
    void saveBookmarks();
    void showContentsTopic();
    void showInitDoneMessage();
    void buildContentDict();

    TQMap<TQString, TQString> titleMap;
    bool indexDone, bookmarksInserted, titleMapDone, contentsInserted;
    bool lwClosed;
    MainWindow *help;
    TQString documentationPath;
    Index *fullTextIndex;
    TQStringList terms, foundDocs;
    bool initDoneMsgShown;
    void getAllContents();
    TQDict<ContentList> contentList;
    TQPopupMenu *itemPopup;
    TQString cacheFilesPath;
    TQStringList keywordDocuments;
};

#endif
