/****************************************************************************
**
** Copyright (C) 1992-2008 Trolltech ASA.  All rights reserved.
**
** This file is part of an example program for TQt.  This example
** program may be used, distributed and modified without limitation.
**
*****************************************************************************/

#ifndef COMPOSER_H
#define COMPOSER_H

#include <ntqwidget.h>


class TQLineEdit;
class TQMultiLineEdit;
class TQLabel;
class TQPushButton;


class Composer : public TQWidget
{
    TQ_OBJECT

public:
    Composer( TQWidget *parent = 0 );

private slots:
    void sendMessage();
    void enableSend();

private:
    TQLineEdit *from, *to, *subject;
    TQMultiLineEdit *message;
    TQLabel * sendStatus;
    TQPushButton * send;
};


#endif
