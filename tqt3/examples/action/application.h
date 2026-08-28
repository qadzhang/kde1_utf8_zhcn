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

class TQTextEdit;

class ApplicationWindow: public TQMainWindow
{
    TQ_OBJECT

public:
    ApplicationWindow();
    ~ApplicationWindow();

protected:
    void closeEvent( TQCloseEvent* );

private slots:
    void newDoc();
    void choose();
    void load( const TQString &fileName );
    void save();
    void saveAs();
    void print();

    void about();
    void aboutTQt();

private:
    TQPrinter *printer;
    TQTextEdit *e;
    TQString filename;
};


#endif
