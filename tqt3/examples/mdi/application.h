/****************************************************************************
**
** Copyright (C) 1992-2008 Trolltech ASA.  All rights reserved.
**
** This file is part of an example program for TQt.  This example
** program may be used, distributed and modified without limitation.
**
*****************************************************************************/

#ifndef APPLICATION_H
#define APPLICATION_H

#include <ntqmainwindow.h>
#include <ntqptrlist.h>

class TQTextEdit;
class TQToolBar;
class TQPopupMenu;
class TQWorkspace;
class TQPopupMenu;
class TQMovie;

class MDIWindow: public TQMainWindow
{
    TQ_OBJECT
public:
    MDIWindow( TQWidget* parent, const char* name, int wflags );
    ~MDIWindow();

    void load( const TQString& fn );
    void save();
    void saveAs();
    void print( TQPrinter* );

protected:
    void closeEvent( TQCloseEvent * );

signals:
    void message(const TQString&, int );

private:
    TQTextEdit* medit;
    TQMovie * mmovie;
    TQString filename;
};


class ApplicationWindow: public TQMainWindow
{
    TQ_OBJECT
public:
    ApplicationWindow();
    ~ApplicationWindow();

protected:
    void closeEvent( TQCloseEvent * );

private slots:
    MDIWindow* newDoc();
    void load();
    void save();
    void saveAs();
    void print();
    void closeWindow();
    void tileHorizontal();

    void about();
    void aboutTQt();

    void windowsMenuAboutToShow();
    void windowsMenuActivated( int id );

private:
    TQPrinter *printer;
    TQWorkspace* ws;
    TQToolBar *fileTools;
    TQPopupMenu* windowsMenu;
};


#endif
