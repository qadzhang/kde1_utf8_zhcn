/****************************************************************************
**
** Copyright (C) 1992-2008 Trolltech ASA.  All rights reserved.
**
** This file is part of an example program for TQt.  This example
** program may be used, distributed and modified without limitation.
**
*****************************************************************************/

#include <ntqapplication.h>
#include <ntqcombobox.h>
#include <ntqmap.h>
#include <ntqsqldatabase.h>
#include <ntqsqlcursor.h>
#include <ntqsqleditorfactory.h>
#include <ntqsqlpropertymap.h>
#include "../connection.h"

class StatusPicker : public TQComboBox
{
    TQ_OBJECT
    TQ_PROPERTY( int statusid READ statusId WRITE setStatusId )
    public:
	StatusPicker( TQWidget *parent=0, const char *name=0 );
	int statusId() const;
	void setStatusId( int id );
    private:
	TQMap< int, int > index2id;
};


class CustomSqlEditorFactory : public TQSqlEditorFactory
{
    TQ_OBJECT
    public:
	TQWidget *createEditor( TQWidget *parent, const TQSqlField *field );
};



