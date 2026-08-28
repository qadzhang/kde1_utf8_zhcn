/****************************************************************************
**
** Copyright (C) 1992-2008 Trolltech ASA.  All rights reserved.
**
** This file is part of an example program for TQt.  This example
** program may be used, distributed and modified without limitation.
**
*****************************************************************************/

#include <ntqapplication.h>
#include <ntqpainter.h>
#include <ntqpicture.h>
#include <ntqpixmap.h>
#include <ntqwidget.h>
#include <ntqmessagebox.h>
#include <ntqfile.h>
#include <ctype.h>


void paintCar( TQPainter *p )			// paint a car
{
    TQPointArray a;
    TQBrush brush( TQt::yellow, TQt::SolidPattern );
    p->setBrush( brush );			// use solid, yellow brush

    a.setPoints( 5, 50,50, 350,50, 450,120, 450,250, 50,250 );
    p->drawPolygon( a );			// draw car body

    TQFont f( "courier", 12, TQFont::Bold );
    p->setFont( f );

    TQColor windowColor( 120, 120, 255 );	// a light blue color
    brush.setColor( windowColor );		// set this brush color
    p->setBrush( brush );			// set brush
    p->drawRect( 80, 80, 250, 70 );		// car window
    p->drawText( 180, 80, 150, 70, TQt::AlignCenter, "--  TQt  --\nTrolltech AS" );

    TQPixmap pixmap;
    if ( pixmap.load("flag.bmp") )		// load and draw image
	p->drawPixmap( 100, 85, pixmap );

    p->setBackgroundMode( TQt::OpaqueMode );	// set opaque mode
    p->setBrush( TQt::DiagCrossPattern );	// black diagonal cross pattern
    p->drawEllipse( 90, 210, 80, 80 );		// back wheel
    p->setBrush( TQt::CrossPattern );		// black cross fill pattern
    p->drawEllipse( 310, 210, 80, 80 );		// front wheel
}


class PictureDisplay : public TQWidget		// picture display widget
{
public:
    PictureDisplay( const char *fileName );
   ~PictureDisplay();
protected:
    void	paintEvent( TQPaintEvent * );
    void	keyPressEvent( TQKeyEvent * );
private:
    TQPicture   *pict;
    TQString	name;
};

PictureDisplay::PictureDisplay( const char *fileName )
{
    pict = new TQPicture;
    name = fileName;
    if ( !pict->load(fileName) ) {		// cannot load picture
	delete pict;
	pict = 0;
	name.sprintf( "Not able to load picture: %s", fileName );
    }
}

PictureDisplay::~PictureDisplay()
{
    delete pict;
}

void PictureDisplay::paintEvent( TQPaintEvent * )
{
    TQPainter paint( this );			// paint widget
    if ( pict )
    	paint.drawPicture( *pict );		// draw picture
    else
	paint.drawText( rect(), AlignCenter, name );
}

void PictureDisplay::keyPressEvent( TQKeyEvent *k )
{
    switch ( tolower(k->ascii()) ) {
	case 'r':				// reload
	    pict->load( name );
	    update();
	    break;
	case 'q':				// quit
	    TQApplication::exit();
	    break;
    }
}


int main( int argc, char **argv )
{
    TQApplication a( argc, argv );		// TQApplication required!

    const char *fileName = "car.pic";		// default picture file name

    if ( argc == 2 )				// use argument as file name
	fileName = argv[1];

    if ( !TQFile::exists(fileName) ) {
	TQPicture pict;				// our picture
	TQPainter paint;				// our painter

	paint.begin( &pict );			// begin painting onto picture
	paintCar( &paint );			// paint!
	paint.end();				// painting done

	pict.save( fileName );			// save picture
	TQMessageBox::information(0, "TQt Example - Picture", "Saved.  Run me again!");
	return 0;
    } else {
	PictureDisplay test( fileName );	// create picture display
	a.setMainWidget( &test);		// set main widget
	test.setCaption("TQt Example - Picture");
	test.show();				// show it

	return a.exec();			// start event loop
    }
}
