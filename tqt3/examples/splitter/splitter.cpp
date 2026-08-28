/****************************************************************************
**
** Copyright (C) 1992-2008 Trolltech ASA.  All rights reserved.
**
** This file is part of an example program for TQt.  This example
** program may be used, distributed and modified without limitation.
**
*****************************************************************************/

#include <ntqapplication.h>
#include <ntqlabel.h>
#include <ntqsplitter.h>
#include <ntqmultilineedit.h>

#include <ntqpainter.h>


class Test : public TQWidget {
public:
    Test(TQWidget* parent=0, const char* name=0, int f=0);
    void paintEvent(TQPaintEvent* e);
private:
};



Test::Test(TQWidget* parent, const char* name, int f) :
    TQWidget(parent, name, f)
{

}

void Test::paintEvent(TQPaintEvent* e)
{
    TQPainter p(this);
    p.setClipRect(e->rect());
    const int d = 1000; //large number
    int x1 = 0;
    int x2 = width()-1;
    int y1 = 0;
    int y2 = height()-1;

    int x = (x1+x2)/2;
    p.drawLine( x, y1, x+d, y1+d   );
    p.drawLine( x, y1, x-d, y1+d   );
    p.drawLine( x, y2, x+d, y2-d   );
    p.drawLine( x, y2, x-d, y2-d   );

    int y = (y1+y2)/2;
    p.drawLine( x1, y, x1+d, y+d   );
    p.drawLine( x1, y, x1+d, y-d   );
    p.drawLine( x2, y, x2-d, y+d   );
    p.drawLine( x2, y, x2-d, y-d   );
}


int main( int argc, char ** argv )
{
    TQApplication a( argc, argv );

    TQSplitter *s1 = new TQSplitter( TQSplitter::Vertical, 0 , "main" );

    TQSplitter *s2 = new TQSplitter( TQSplitter::Horizontal, s1, "top" );

    Test *t1 = new Test( s2, "topLeft" );
    t1->setBackgroundColor( TQt::blue.light( 180 ) );
    t1->setMinimumSize( 50, 0 );

    Test *t2 = new Test( s2, "topRight" );
    t2->setBackgroundColor( TQt::green.light( 180 ) );
    s2->setResizeMode( t2, TQSplitter::KeepSize );
    s2->moveToFirst( t2 );

    TQSplitter *s3 = new TQSplitter( TQSplitter::Horizontal,  s1, "bottom" );

    Test *t3 = new Test( s3, "bottomLeft" );
    t3->setBackgroundColor( TQt::red );
    Test *t4 = new Test( s3, "bottomMiddle" );
    t4->setBackgroundColor( TQt::white );

    Test *t5 = new Test( s3, "bottomRight" );
    t5->setMaximumHeight( 250 );
    t5->setMinimumSize( 80, 50 );
    t5->setBackgroundColor( TQt::yellow );

#ifdef TQ_WS_QWS
    // TQt/Embedded XOR drawing not yet implemented.
    s1->setOpaqueResize( true );
#endif
    s2->setOpaqueResize( true );
    s3->setOpaqueResize( true );

    a.setMainWidget( s1 );
    s1->setCaption("TQt Example - Splitters");
    s1->show();
    int result = a.exec();
    delete s1;
    return result;
}
