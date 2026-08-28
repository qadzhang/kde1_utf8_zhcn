/**********************************************************************
** Copyright (C) 2000-2008 Trolltech ASA.  All rights reserved.
**
** This file is part of TQt Linguist.
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

#ifndef TRWINDOW_H
#define TRWINDOW_H

#include "phrase.h"

#include <metatranslator.h>

#include <ntqmainwindow.h>
#include <ntqptrlist.h>
#include <ntqdict.h>
#include <ntqprinter.h>
#include <ntqstringlist.h>
#include <ntqpixmap.h>
#include <ntqlistview.h>

class TQAction;
class TQDialog;
class TQLabel;
class TQListView;
class TQListViewItem;
class TQPrinter;

class Action;
class FindDialog;
class MessageEditor;

class PhraseLV;
class ContextLVI;
class Statistics;

class TrWindow : public TQMainWindow
{
    TQ_OBJECT
public:
    static TQPixmap * pxOn;
    static TQPixmap * pxOff;
    static TQPixmap * pxObsolete;
    static TQPixmap * pxDanger;
    static const TQPixmap pageCurl();

    TrWindow();
    ~TrWindow();

    void openFile( const TQString& name );

protected:
    void readConfig();
    void writeConfig();
    void closeEvent( TQCloseEvent * );

signals:
    void statsChanged( int w, int c, int cs, int w2, int c2, int cs2 );
    
private slots:
    void doneAndNext();
    void prev();
    void next();
    void recentFileActivated( int );
    void setupRecentFilesMenu();
    void open();
    void save();
    void saveAs();
    void release();
    void print();
    void find();
    void findAgain();
    void replace();
    void newPhraseBook();
    void openPhraseBook();
    void closePhraseBook( int id );
    void editPhraseBook( int id );
    void printPhraseBook( int id );
    void manual();
    void revertSorting();
    void about();
    void aboutTQt();

    void setupPhrase();
    bool maybeSave();
    void updateCaption();
    void showNewScope( TQListViewItem *item );
    void showNewCurrent( TQListViewItem *item );
    void updateTranslation( const TQString& translation );
    void updateFinished( bool finished );
    void toggleFinished( TQListViewItem *item, const TQPoint& p, int column );
    void prevUnfinished();
    void nextUnfinished();
    void findNext( const TQString& text, int where, bool matchCase );
    void revalidate();
    void toggleGuessing();
    void focusSourceList();
    void focusPhraseList();
    void updateClosePhraseBook();
    void toggleStatistics();
    void updateStatistics();

private:
    typedef TQPtrList<PhraseBook> PBL;
    typedef TQDict<PhraseBook> PBD;

    static TQString friendlyString( const TQString& str );

    void addRecentlyOpenedFile( const TQString & fn, TQStringList & lst );
    void setupMenuBar();
    void setupToolBars();
    void setCurrentContextItem( TQListViewItem *item );
    void setCurrentMessageItem( TQListViewItem *item );
    TQString friendlyPhraseBookName( int k );
    bool openPhraseBook( const TQString& name );
    bool savePhraseBook( TQString& name, const PhraseBook& pb );
    void updateProgress();
    void updatePhraseDict();
    PhraseBook getPhrases( const TQString& source );
    bool danger( const TQString& source, const TQString& translation,
		 bool verbose = false );

    int itemToIndex( TQListView * view, TQListViewItem * item );
    TQListViewItem * indexToItem( TQListView * view, int index );
    bool searchItem( const TQString & searchWhat, TQListViewItem * j,
		     TQListViewItem * k );
    void countStats( TQListViewItem * ci, TQListViewItem * mi, int& trW, int& trC, int& trCS );
    void doCharCounting( const TQString& text, int& trW, int& trC, int& trCS );
    
    TQListView     * plv;
    TQListView     * lv;
    TQListView     * slv;
    MessageEditor * me;
    TQLabel        * progress;
    TQLabel        * modified;
    MetaTranslator tor;
    bool dirty;
    bool messageIsShown;
    int  numFinished;
    int  numNonobsolete;
    int  numMessages;
    int  dirtyItem;
    TQStringList recentFiles;
    TQString     filename;

    PBD phraseDict;
    PBL phraseBooks;
    TQStringList phraseBookNames;

    TQPrinter printer;

    FindDialog *f;
    FindDialog *h;
    TQString findText;
    int findWhere;
    bool findMatchCase;
    int foundItem;
    TQListViewItem *foundScope;
    int foundWhere;
    int foundOffset;

    TQPopupMenu * phrasep;
    TQPopupMenu * closePhraseBookp;
    TQPopupMenu * editPhraseBookp;
    TQPopupMenu * printPhraseBookp;
    TQPopupMenu * recentFilesMenu;
    int closePhraseBookId;
    int editPhraseBookId;
    int printPhraseBookId;
    Action *openAct;
    Action *saveAct;
    Action *saveAsAct;
    Action *releaseAct;
    Action *printAct;
    Action *exitAct;
    Action *undoAct;
    Action *redoAct;
    Action *cutAct;
    Action *copyAct;
    Action *pasteAct;
    Action *selectAllAct;
    Action *findAct;
    Action *findAgainAct;
    Action *replaceAct;
    Action *newPhraseBookAct;
    Action *openPhraseBookAct;
    Action *acceleratorsAct;
    Action *endingPunctuationAct;
    Action *phraseMatchesAct;
    Action *revertSortingAct;
    Action *aboutAct;
    Action *aboutTQtAct;
    Action *manualAct;
    Action *whatsThisAct;
    Action *beginFromSourceAct;
    Action *prevAct;
    Action *nextAct;
    Action *prevUnfinishedAct;
    Action *nextUnfinishedAct;
    Action *doneAndNextAct;
    TQAction *doneAndNextAlt;
    Action *doGuessesAct;
    Action *toggleStats;
    Statistics * stats;
    int  srcWords;
    int  srcChars;
    int  srcCharsSpc;
};

#endif
