/****************************************************************************
**
** Copyright (C) 1992-2008 Trolltech ASA.  All rights reserved.
**
** This file is part of an example program for TQt.  This example
** program may be used, distributed and modified without limitation.
**
*****************************************************************************/

#include <ntqapplication.h>
#include <ntqgridview.h>
#include <ntqpainter.h>

// Grid size
const int numRows = 100;
const int numCols = 100;

class MyGridView : public TQGridView
{
public:
    MyGridView() {
	setNumRows( ::numRows );
	setNumCols( ::numCols );
	setCellWidth( fontMetrics().width( TQString("%1 / %2").arg(numRows()).arg(numCols()))); 
	setCellHeight( 2*fontMetrics().lineSpacing() );
	setCaption( tr( "TQt Example - This is a grid with 100 x 100 cells" ) );
    }

protected:
    void paintCell( TQPainter *p, int row, int col ) {
	p->drawLine( cellWidth()-1, 0, cellWidth()-1, cellHeight()-1 );
	p->drawLine( 0, cellHeight()-1, cellWidth()-1, cellHeight()-1 );
	p->drawText( cellRect(), AlignCenter, TQString("%1 / %1").arg(row).arg(col) );
    }
};

// The program starts here.
int main( int argc, char **argv )
{
    TQApplication app( argc, argv );			

    MyGridView gridview;
    app.setMainWidget( &gridview );
    gridview.show();
    return app.exec();
}
