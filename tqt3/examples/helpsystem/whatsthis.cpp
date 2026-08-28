/****************************************************************************
**
** Copyright (C) 1992-2008 Trolltech ASA.  All rights reserved.
**
** This file is part of an example program for TQt.  This example
** program may be used, distributed and modified without limitation.
**
*****************************************************************************/

#include <ntqapplication.h>
#include <ntqdir.h>
#include <ntqheader.h>
#include <ntqtable.h>

#include "whatsthis.h"

WhatsThis::WhatsThis( TQWidget *w, TQWidget *watch )
: TQWhatsThis( watch ? watch : w ), widget( w )
{
}

TQWidget *WhatsThis::parentWidget() const
{
    return widget;
}

bool WhatsThis::clicked( const TQString &link )
{
    if ( !link.isEmpty() )
	emit linkClicked( link );

    return true;
}

HeaderWhatsThis::HeaderWhatsThis( TQHeader *h ) 
: WhatsThis( h )
{
}

TQString HeaderWhatsThis::text( const TQPoint &p )
{
    TQHeader *header = (TQHeader*)parentWidget();

    TQString orient;
    int section;
    if ( header->orientation() == TQObject::Horizontal ) {
	orient = "horizontal";
	section = header->sectionAt( p.x() );
    } else {
	orient = "vertical";
	section = header->sectionAt( p.y() );
    }
    if( section == -1 )
	return "This is empty space.";
    TQString docsPath = TQDir("../../doc").absPath();
    return TQString("This is section number %1 in the %2 <a href=%2/html/ntqheader.html>header</a>.").
	arg(section + 1).
	arg(orient).
	arg(docsPath);
}

TableWhatsThis::TableWhatsThis( TQTable *t ) 
: WhatsThis( t, t->viewport() )
{
}


TQString TableWhatsThis::text( const TQPoint &p )
{
    TQTable *table = (TQTable*)parentWidget();

    TQPoint cp = table->viewportToContents( p );
    int row = table->rowAt( cp.y() );
    int col = table->columnAt( cp.x() );

    if ( row == -1 || col == -1 )
	return "This is empty space.";

    TQTableItem* i = table->item( row,col  );
    if ( !i )
	return "This is an empty cell.";

    TQString docsPath = TQDir("../../doc").absPath();

    if ( TQTableItem::RTTI == i->rtti() ) {
	return TQString("This is a <a href=%1/html/qtableitem.html>TQTableItem</a>.").
		       arg(docsPath);
    } else if ( TQComboTableItem::RTTI == i->rtti() ) {
	return TQString("This is a <a href=%1/html/qcombotableitem.html>TQComboTableItem</a>."
		       "<br>It can be used to provide multiple-choice items in a table.").
		       arg(docsPath);
    } else if ( TQCheckTableItem::RTTI == i->rtti() ) {
	return TQString("This is a <a href=%1/html/qchecktableitem.html>TQCheckTableItem</a>."
		       "<br>It provide <a href=%1/html/ntqcheckbox.html>checkboxes</a> in tables.").
		       arg(docsPath).arg(docsPath);
    }

    return "This is a user defined table item.";
}
