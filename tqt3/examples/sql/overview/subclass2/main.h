/****************************************************************************
**
** Copyright (C) 1992-2008 Trolltech ASA.  All rights reserved.
**
** This file is part of an example program for TQt.  This example
** program may be used, distributed and modified without limitation.
**
*****************************************************************************/

#include <ntqapplication.h>
#include <ntqsqldatabase.h>
#include <ntqsqlcursor.h>
#include "../connection.h"

class TQSqlRecord;

class InvoiceItemCursor : public TQSqlCursor
{
    public:
	InvoiceItemCursor();
};


