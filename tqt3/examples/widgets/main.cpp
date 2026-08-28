/****************************************************************************
**
** Copyright (C) 1992-2008 Trolltech ASA.  All rights reserved.
**
** This file is part of an example program for TQt.  This example
** program may be used, distributed and modified without limitation.
**
*****************************************************************************/

#include <ntqapplication.h>
#include <ntqstylefactory.h>
#include "widgets.h"

class MyWidgetView : public WidgetView
{
    int s;
public:
    MyWidgetView( TQWidget *parent=0, const char *name=0 )
	:WidgetView(parent, name), s(0)
    {
	setToolBarsMovable( true );
    }

    void button1Clicked()
    {
	TQStringList styles = TQStyleFactory::keys();

	s = (++s)%styles.count();
	tqApp->setStyle( styles[ s] );
	WidgetView::button1Clicked();
    }
};


//
// Create and display our WidgetView.
//

int main( int argc, char **argv )
{
    TQApplication::setColorSpec( TQApplication::CustomColor );
    TQApplication a( argc, argv );

    MyWidgetView* w = new MyWidgetView;
    a.setMainWidget( w );

    w->show();
    int res = a.exec();
    delete w;
    return res;
}
