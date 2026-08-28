/****************************************************************************
**
** Copyright (C) 1992-2008 Trolltech ASA.  All rights reserved.
**
** This file is part of an example program for TQt.  This example
** program may be used, distributed and modified without limitation.
**
*****************************************************************************/

#ifndef THEMES_H
#define THEMES_H

#include <ntqmainwindow.h>
#include <ntqfont.h>

class TQTabWidget;

class Themes: public TQMainWindow
{
    TQ_OBJECT

public:
    Themes( TQWidget *parent = 0, const char *name = 0, WFlags f = WType_TopLevel );

protected:
    TQTabWidget *tabwidget;

protected slots:
    void makeStyle(const TQString &);
    void about();
    void aboutTQt();

private:
    TQFont appFont;
};


#endif
