/****************************************************************************
**
** Copyright (C) 1992-2008 Trolltech ASA.  All rights reserved.
**
** This file is part of an example program for TQt.  This example
** program may be used, distributed and modified without limitation.
**
*****************************************************************************/

#include <ntqapplication.h>
#include <ntqdialog.h>
#include <ntqlabel.h>
#include <ntqlayout.h>
#include <ntqlineedit.h>
#include <ntqpushbutton.h>
#include <ntqsqldatabase.h>
#include <ntqsqlcursor.h>
#include <ntqsqlform.h>
#include "../connection.h"

class FormDialog : public TQDialog
{
    TQ_OBJECT
    public:
	FormDialog();
	~FormDialog();
    public slots:
	void save();
    private:
	TQSqlCursor staffCursor;
	TQSqlForm *sqlForm;
	TQSqlIndex idIndex;
};
