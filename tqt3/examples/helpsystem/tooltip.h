/****************************************************************************
**
** Copyright (C) 1992-2008 Trolltech ASA.  All rights reserved.
**
** This file is part of an example program for TQt.  This example
** program may be used, distributed and modified without limitation.
**
*****************************************************************************/

#ifndef TOOLTIP_H
#define TOOLTIP_H

#include <ntqtooltip.h>


class TQTable;
class TQHeader;

class HeaderToolTip : public TQToolTip
{
public:
    HeaderToolTip( TQHeader *header, TQToolTipGroup *group = 0 );
    
protected:
    void maybeTip ( const TQPoint &p );
};

class TableToolTip : public TQToolTip
{
public:
    TableToolTip( TQTable* table, TQToolTipGroup *group = 0  );
    
protected:
    void maybeTip( const TQPoint &p );
    
private:
    TQTable *table;
};


#endif
