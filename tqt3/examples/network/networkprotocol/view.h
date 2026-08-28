/****************************************************************************
**
** Copyright (C) 1992-2008 Trolltech ASA.  All rights reserved.
**
** This file is part of an example program for TQt.  This example
** program may be used, distributed and modified without limitation.
**
*****************************************************************************/

#ifndef VIEW_H
#define VIEW_H

#include <ntqvbox.h>
#include <ntqcstring.h>
#include <ntqurloperator.h>

class TQMultiLineEdit;

class View : public TQVBox
{
    TQ_OBJECT
    
public:
    View();
    
private slots:
    void downloadFile();
    void newData( const TQByteArray &ba );

private:
    TQMultiLineEdit *fileView;
    TQUrlOperator op;
    
    TQString getOpenFileName();
};

#endif
