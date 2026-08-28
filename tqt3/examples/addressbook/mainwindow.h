/****************************************************************************
**
** Copyright (C) 1992-2008 Trolltech ASA.  All rights reserved.
**
** This file is part of an example program for TQt.  This example
** program may be used, distributed and modified without limitation.
**
*****************************************************************************/

#ifndef AB_MAINWINDOW_H
#define AB_MAINWINDOW_H

#include <ntqmainwindow.h>
#include <ntqstring.h>

class TQToolBar;
class TQPopupMenu;
class ABCentralWidget;

class ABMainWindow: public TQMainWindow
{
    TQ_OBJECT

public:
    ABMainWindow();
    ~ABMainWindow();

protected slots:
    void fileNew();
    void fileOpen();
    void fileSave();
    void fileSaveAs();
    void filePrint();
    void closeWindow();

protected:
    void setupMenuBar();
    void setupFileTools();
    void setupStatusBar();
    void setupCentralWidget();

    TQToolBar *fileTools;
    TQString filename;
    ABCentralWidget *view;

};


#endif
