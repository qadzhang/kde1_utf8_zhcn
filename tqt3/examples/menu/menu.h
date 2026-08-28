/****************************************************************************
**
** Copyright (C) 1992-2008 Trolltech ASA.  All rights reserved.
**
** This file is part of an example program for TQt.  This example
** program may be used, distributed and modified without limitation.
**
*****************************************************************************/

#ifndef MENU_H
#define MENU_H

#include <ntqwidget.h>
#include <ntqmenubar.h>
#include <ntqlabel.h>


class MenuExample : public TQWidget
{
    TQ_OBJECT
public:
    MenuExample( TQWidget *parent=0, const char *name=0 );

public slots:
    void open();
    void news();
    void save();
    void closeDoc();
    void undo();
    void redo();
    void normal();
    void bold();
    void underline();
    void about();
    void aboutTQt();
    void printer();
    void file();
    void fax();
    void printerSetup();

protected:
    void    resizeEvent( TQResizeEvent * );

signals:
    void    explain( const TQString& );

private:
    void contextMenuEvent ( TQContextMenuEvent * );


    TQMenuBar *menu;
    TQLabel   *label;
    bool isBold;
    bool isUnderline;
    int boldID, underlineID;
};


#endif // MENU_H
