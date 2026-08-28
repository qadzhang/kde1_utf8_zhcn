/****************************************************************************
**
** Copyright (C) 1992-2008 Trolltech ASA.  All rights reserved.
**
** This file is part of an example program for TQt.  This example
** program may be used, distributed and modified without limitation.
**
*****************************************************************************/

#ifndef STATISTICS_H
#define STATISTICS_H

#include <ntqtable.h>
#include <ntqcombobox.h>

class TableItem : public TQTableItem
{
public:
    TableItem( TQTable *t, EditType et, const TQString &txt ) : TQTableItem( t, et, txt ) {}
    void paint( TQPainter *p, const TQColorGroup &cg, const TQRect &cr, bool selected );
};

class ComboItem : public TQTableItem
{
public:
    ComboItem( TQTable *t, EditType et );
    TQWidget *createEditor() const;
    void setContentFromEditor( TQWidget *w );
    void setText( const TQString &s );
    
private:
    TQComboBox *cb;

};

class Table : public TQTable
{
    TQ_OBJECT

public:
    Table();
    void sortColumn( int col, bool ascending, bool wholeRows );

private slots:
    void recalcSum( int row, int col );

private:
    void initTable();

};

#endif
