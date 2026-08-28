/****************************************************************************
**
** Copyright (C) 1992-2008 Trolltech ASA.  All rights reserved.
**
** This file is part of an example program for TQt.  This example
** program may be used, distributed and modified without limitation.
**
*****************************************************************************/

#ifndef WIZARD_H
#define WIZARD_H

#include <ntqwizard.h>

class TQWidget;
class TQHBox;
class TQLineEdit;
class TQLabel;

class Wizard : public TQWizard
{
    TQ_OBJECT

public:
    Wizard( TQWidget *parent = 0, const char *name = 0 );

    void showPage(TQWidget* page);

protected:
    void setupPage1();
    void setupPage2();
    void setupPage3();

    TQHBox *page1, *page2, *page3;
    TQLineEdit *key, *firstName, *lastName, *address, *phone, *email;
    TQLabel *lKey, *lFirstName, *lLastName, *lAddress, *lPhone, *lEmail;

protected slots:
    void keyChanged( const TQString & );
    void dataChanged( const TQString & );

};

#endif
