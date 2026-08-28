/****************************************************************************
**
** Copyright (C) 1992-2008 Trolltech ASA.  All rights reserved.
**
** This file is part of an example program for TQt.  This example
** program may be used, distributed and modified without limitation.
**
*****************************************************************************/

#ifndef AB_CENTRALWIDGET_H
#define AB_CENTRALWIDGET_H

#include <ntqwidget.h>
#include <ntqstring.h>

class TQTabWidget;
class TQListView;
class TQGridLayout;
class TQLineEdit;
class TQPushButton;
class TQListViewItem;
class TQCheckBox;

class ABCentralWidget : public TQWidget
{
    TQ_OBJECT

public:
    ABCentralWidget( TQWidget *parent, const char *name = 0 );

    void save( const TQString &filename );
    void load( const TQString &filename );

protected slots:
    void addEntry();
    void changeEntry();
    void itemSelected( TQListViewItem* );
    void selectionChanged();
    void toggleFirstName();
    void toggleLastName();
    void toggleAddress();
    void toggleEMail();
    void findEntries();

protected:
    void setupTabWidget();
    void setupListView();

    TQGridLayout *mainGrid;
    TQTabWidget *tabWidget;
    TQListView *listView;
    TQPushButton *add, *change, *find;
    TQLineEdit *iFirstName, *iLastName, *iAddress, *iEMail,
        *sFirstName, *sLastName, *sAddress, *sEMail;
    TQCheckBox *cFirstName, *cLastName, *cAddress, *cEMail;

};

#endif
