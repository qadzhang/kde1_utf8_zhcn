/****************************************************************************
**
** Copyright (C) 1992-2008 Trolltech ASA.  All rights reserved.
**
** This file is part of an example program for TQt.  This example
** program may be used, distributed and modified without limitation.
**
*****************************************************************************/

#ifndef TABDIALOG_H
#define TABDIALOG_H

#include <ntqtabdialog.h>
#include <ntqstring.h>
#include <ntqfileinfo.h>

class TabDialog : public TQTabDialog
{
    TQ_OBJECT

public:
    TabDialog( TQWidget *parent, const char *name, const TQString &_filename );

protected:
    TQString filename;
    TQFileInfo fileinfo;

    void setupTab1();
    void setupTab2();
    void setupTab3();

};

#endif
