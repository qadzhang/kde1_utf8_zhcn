/****************************************************************************
**
** Copyright (C) 1992-2008 Trolltech ASA.  All rights reserved.
**
** This file is part of an example program for TQt.  This example
** program may be used, distributed and modified without limitation.
**
*****************************************************************************/

#ifndef HELPWINDOW_H
#define HELPWINDOW_H

#include <ntqmainwindow.h>
#include <ntqtextbrowser.h>
#include <ntqstringlist.h>
#include <ntqmap.h>
#include <ntqdir.h>

class TQComboBox;
class TQPopupMenu;

class HelpWindow : public TQMainWindow
{
    TQ_OBJECT
public:
    HelpWindow( const TQString& home_,  const TQString& path, TQWidget* parent = 0, const char *name=0 );
    ~HelpWindow();

private slots:
    void setBackwardAvailable( bool );
    void setForwardAvailable( bool );

    void sourceChanged( const TQString& );
    void about();
    void aboutTQt();
    void openFile();
    void newWindow();
    void print();

    void pathSelected( const TQString & );
    void histChosen( int );
    void bookmChosen( int );
    void addBookmark();

private:
    void readHistory();
    void readBookmarks();

    TQTextBrowser* browser;
    TQComboBox *pathCombo;
    int backwardId, forwardId;
    TQStringList history, bookmarks;
    TQMap<int, TQString> mHistory, mBookmarks;
    TQPopupMenu *hist, *bookm;

};





#endif

