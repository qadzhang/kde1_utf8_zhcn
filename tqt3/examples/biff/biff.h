/****************************************************************************
**
** Copyright (C) 1992-2008 Trolltech ASA.  All rights reserved.
**
** This file is part of an example program for TQt.  This example
** program may be used, distributed and modified without limitation.
**
*****************************************************************************/

#ifndef BIFF_H
#define BIFF_H

#include <ntqwidget.h>
#include <ntqdatetime.h>
#include <ntqpixmap.h>


class Biff : public TQWidget
{
    TQ_OBJECT
public:
    Biff( TQWidget *parent=0, const char *name=0 );

protected:
    void	timerEvent( TQTimerEvent * );
    void	paintEvent( TQPaintEvent * );
    void	mousePressEvent( TQMouseEvent * );

private:
    TQDateTime	lastModified;
    TQPixmap	hasNewMail;
    TQPixmap	noNewMail;
    TQString	mailbox;
    bool	gotMail;
};


#endif // BIFF_H
