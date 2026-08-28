/****************************************************************************
**
** Copyright (C) 1992-2008 Trolltech ASA.  All rights reserved.
**
** This file is part of an example program for TQt.  This example
** program may be used, distributed and modified without limitation.
**
*****************************************************************************/

#ifndef MAINWIN_H
#define MAINWIN_H

#include <ntqmainwindow.h>

class TQtFileIconView;
class DirectoryView;
class TQProgressBar;
class TQLabel;
class TQComboBox;
class TQToolButton;

class FileMainWindow : public TQMainWindow
{
    TQ_OBJECT

public:
    FileMainWindow();

    TQtFileIconView *fileView() { return fileview; }
    DirectoryView *dirList() { return dirlist; }

    void show();

protected:
    void setup();
    void setPathCombo();

    TQtFileIconView *fileview;
    DirectoryView *dirlist;
    TQProgressBar *progress;
    TQLabel *label;
    TQComboBox *pathCombo;
    TQToolButton *upButton, *mkdirButton;

protected slots:
    void directoryChanged( const TQString & );
    void slotStartReadDir( int dirs );
    void slotReadNextDir();
    void slotReadDirDone();
    void cdUp();
    void newFolder();
    void changePath( const TQString &path );
    void enableUp();
    void disableUp();
    void enableMkdir();
    void disableMkdir();

};

#endif
