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
#include <ntqsqlpropertymap.h>
#include "../connection.h"

class CustomEdit : public TQLineEdit
{
    TQ_OBJECT
    TQ_PROPERTY( TQString upperLine READ upperLine WRITE setUpperLine )
    public:
	CustomEdit( TQWidget *parent=0, const char *name=0 );
	TQString upperLine() const;
	void setUpperLine( const TQString &line );
    public slots:
	void changed( const TQString &line );
    private:
	TQString upperLineText;
};


class FormDialog : public TQDialog
{
    TQ_OBJECT
    public:
	FormDialog();
	~FormDialog();
    public slots:
	void save();
    private:
	TQSqlCursor *staffCursor;
	TQSqlForm *sqlForm;
	TQSqlPropertyMap *propMap;
	TQSqlIndex idIndex;
};


