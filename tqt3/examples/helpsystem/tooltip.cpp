/****************************************************************************
**
** Copyright (C) 1992-2008 Trolltech ASA.  All rights reserved.
**
** This file is part of an example program for TQt.  This example
** program may be used, distributed and modified without limitation.
**
*****************************************************************************/

#include <ntqtooltip.h>
#include <ntqtable.h>

#include "tooltip.h"

HeaderToolTip::HeaderToolTip( TQHeader *header, TQToolTipGroup *group ) 
: TQToolTip( header, group )
{
}

void HeaderToolTip::maybeTip ( const TQPoint& p )
{
    TQHeader *header = (TQHeader*)parentWidget();
    
    int section = 0;
    
    if ( header->orientation() == Horizontal )
	section = header->sectionAt( header->offset() + p.x() );
    else
	section = header->sectionAt( header->offset() + p.y() );
    
    TQString tipString = header->label( section );
    tip( header->sectionRect( section ), tipString, "This is a section in a header" );
}

TableToolTip::TableToolTip( TQTable *tipTable, TQToolTipGroup *group ) 
: TQToolTip( tipTable->viewport(), group ), table( tipTable )
{
}


void TableToolTip::maybeTip ( const TQPoint &p )
{
    TQPoint cp = table->viewportToContents( p );
    int row = table->rowAt( cp.y() );
    int col = table->columnAt( cp.x() );
   
    TQString tipString = table->text( row, col );

    TQRect cr = table->cellGeometry( row, col );
    cr.moveTopLeft( table->contentsToViewport( cr.topLeft() ) );
    tip( cr, tipString, "This is a cell in a table" );
}
