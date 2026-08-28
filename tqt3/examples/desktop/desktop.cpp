/****************************************************************************
**
** Copyright (C) 1992-2008 Trolltech ASA.  All rights reserved.
**
** This file is part of an example program for TQt.  This example
** program may be used, distributed and modified without limitation.
**
*****************************************************************************/

#include <ntqimage.h>
#include <ntqbitmap.h>
#include <ntqpainter.h>
#include <ntqapplication.h>
#include <ntqdropsite.h>
#include <ntqdragobject.h>
#include <stdio.h>


static double seed = 0.353535353535;
static const int KINDA_RAND_MAX = 32767;

static int kindaRand()
{
    seed = seed*147;
    seed = seed - (double) ((int) seed);
    return (int) ( seed*(KINDA_RAND_MAX + 1) );
}

static int velocity( int i )			// change velocity
{
    const int velmax = 15;
    const int velmin = 4;
    if ( i == 1 || i == 2 )
	i = (kindaRand()&0x7fff % velmax)/3 + velmin;
    else
	i = (kindaRand()&0x7fff % velmax) + velmin;
    return i;
}

//
// Draw polygon on desktop.
//

void poly()
{
    TQWidget *d = TQApplication::desktop();
    d->setBackgroundColor( TQt::white );		// white desktop

    const int maxpoints = 5;
    const int maxcurves = 8;
    static int xvel[maxpoints];
    static int yvel[maxpoints];
    int head = 0;
    int tail = -maxcurves + 2;
    TQPointArray *a = new TQPointArray[ maxcurves ];
    TQPointArray *p;
    TQRect r = d->rect();			// desktop rectangle

    int i;
    for ( i=0; i<maxcurves; i++ )
	a[i].resize( maxpoints );
    p = &a[0];
    for ( i=0; i<maxpoints; i++ ) {		// setup first polygon points
	p->setPoint( i, (kindaRand()&0x7fff) % r.width(),
			(kindaRand()&0x7fff) % r.height() );
	xvel[i] = velocity(i);
	yvel[i] = velocity(i);
    }

    TQPainter paint;
    paint.begin( d );				// start painting desktop

    for ( int ntimes=0; ntimes<2000; ntimes++ ) {
	paint.setBrush( TQColor(kindaRand()%360, 180, 255, TQColor::Hsv) );
	paint.drawPolygon( a[head] );
	if ( ++tail >= maxcurves )
	    tail = 0;

	int minx=r.left(), maxx=r.right();
	int miny=r.top(),  maxy=r.bottom();
	int x, y;
	p = &a[head];
	if ( ++head >= maxcurves )
	    head = 0;
	for ( i=0; i<maxpoints; i++ ) {		// calc new curve
	    p->point( i, &x, &y );
	    x += xvel[i];
	    y += yvel[i];
	    if ( x >= maxx ) {
		x = maxx - (x - maxx + 1);
		xvel[i] = -velocity(i);
	    }
	    if ( x <= minx ) {
		x = minx + (minx - x + 1);
		xvel[i] = velocity(i);
	    }
	    if ( y >= maxy ) {
		y = maxy - (y - maxy + 1);
		yvel[i] = -velocity(i);
	    }
	    if ( y <= miny ) {
		y = miny + (miny - y + 1);
		yvel[i] = velocity(i);
	    }
	    a[head].setPoint( i, x, y );
	}
    }
    paint.end();				// painting done
    delete[] a;
}


//
// Rotate pattern on desktop.
//

void rotate()
{
    int i;
    const int w = 64;
    const int h = 64;
    TQImage image( w, h, 8, 128 );		// create image
    for ( i=0; i<128; i++ )			// build color table
	image.setColor( i, tqRgb(i,0,0) );
    for ( int y=0; y<h; y++ ) {			// set image pixels
	uchar *p = image.scanLine(y);
	for ( int x=0; x<w; x++ )
	    *p++ = (x+y)%128;
    }

    TQPixmap pm;
    pm = image;					// convert image to pixmap
    pm.setOptimization( TQPixmap::BestOptim );	// rotation will be faster

    TQWidget *d = TQApplication::desktop();	// w = desktop widget

    for ( i=0; i<=360; i += 2 ) {
	TQWMatrix m;
	m.rotate( i );				// rotate coordinate system
	TQPixmap rpm = pm.xForm( m );		// rpm = rotated pixmap
	d->setBackgroundPixmap( rpm );		// set desktop pixmap
	d->update();				// repaint desktop
    }
}

//
// Generates a marble-like pattern in pm.
//

void generateStone( TQPixmap *pm,
		    const TQColor &c1, const TQColor &c2, const TQColor &c3 )
{
    TQPainter p;
    TQPen p1 ( c1, 0 );
    TQPen p2 ( c2, 0 );
    TQPen p3 ( c3, 0 );

    p.begin( pm );
    for( int i = 0 ; i < pm->width() ; i++ )
	for( int j = 0 ; j < pm->height() ; j++ ) {
	    int r = kindaRand();
	    if ( r < KINDA_RAND_MAX / 3 )
		p.setPen( p1 );
	    else if ( r < KINDA_RAND_MAX / 3 * 2 )
		p.setPen( p2 );
	    else
		p.setPen( p3 );
	    p.drawPoint( i,j );
	}
    p.end();
}

void drawShadeText( TQPainter *p, int x, int y, const char *text,
		    const TQColor &topColor, const TQColor &bottomColor,
		    int sw = 2 )
{
    if ( !p->isActive() )
	return;

    p->setPen( bottomColor );
    p->drawText( x+sw, y+sw, text );
    p->setPen( topColor );
    p->drawText( x, y, text );
}

// NOTE: desktop drag/drop is experimental

class DesktopWidget : public TQWidget, private TQDropSite
{
public:
    DesktopWidget( const char *s, TQWidget *parent=0, const char *name=0 );
   ~DesktopWidget();
    void paintEvent( TQPaintEvent * );

    void dragEnterEvent( TQDragEnterEvent *e )
    {
	if ( TQImageDrag::canDecode(e) )
	    e->accept();
    }

    void dragLeaveEvent( TQDragLeaveEvent * )
    {
    }

    void dragMoveEvent( TQDragMoveEvent *e )
    {
	e->accept();
    }

    void dropEvent( TQDropEvent * e )
    {
	TQPixmap pmp;
	if ( TQImageDrag::decode( e, pmp ) ) {
	    setBackgroundPixmap( pmp );
	    update();
	}
    }

private:
    TQPixmap *pm;
    TQString text;
};

DesktopWidget::DesktopWidget( const char *s, TQWidget *parent, const char *name )
    : TQWidget( parent, name, WType_Desktop | WPaintDesktop),
	TQDropSite(this)
{
    text = s;
    pm	 = 0;
}

DesktopWidget::~DesktopWidget()
{
    delete pm;
}

void DesktopWidget::paintEvent( TQPaintEvent * )
{
    TQColor c1 = backgroundColor();
    TQColor c2 = c1.light(104);
    TQColor c3 = c1.dark(106);
    if ( !pm ) {
	pm = new TQPixmap( 64, 64 );
	generateStone( pm, c1, c2, c3 );
	setBackgroundPixmap( *pm );
	update();
    }
    TQRect br = fontMetrics().boundingRect( text );
    TQPixmap offscreen( br.width(), br.height() );
    int x = width()/2  - br.width()/2;
    int y = height()/2 - br.height()/2;
    offscreen.fill( this, x, y );
    TQPainter p;
    p.begin( &offscreen );
    drawShadeText( &p, -br.x(), -br.y(), text, c2, c3, 3 );
    p.end();
    bitBlt( this, x, y, &offscreen );
}

void desktopWidget( const char *s = "Trolltech" )
{
    DesktopWidget *t = new DesktopWidget(s);
    t->update();
    tqApp->exec();
    delete t;
}

void desktopText( const char *s = "Trolltech" )
{
    const int border = 20;

    TQColor c1 =	 tqApp->palette().inactive().background();
    TQColor c2 = c1.light(104);
    TQColor c3 = c1.dark(106);

    TQPixmap pm(10,10);

    TQPainter p;
    p.begin( &pm );
    TQRect r = p.fontMetrics().boundingRect( s );
    p.end();

    int appWidth  =  tqApp->desktop()->width();
    int appHeight =  tqApp->desktop()->height();
    if ( r.width() > appWidth - border*2 )
	r.setWidth( appWidth - border*2 );
    if ( r.height() > appHeight - border*2 )
	r.setHeight( appHeight - border*2 );

    pm.resize( r.size() + TQSize( border*2, border*2 ) );
    generateStone( &pm, c1, c2, c3 );
    p.begin( &pm );
    drawShadeText( &p, -r.x() + border, -r.y() + border, s, c2, c3 );
    p.end();

    tqApp->desktop()->setBackgroundPixmap( pm );
}

//
// The program starts here.
//

int main( int argc, char **argv )
{
    TQApplication app( argc, argv );

    if ( argc > 1 ) {
	TQFont f( "charter", 96, TQFont::Black );
	f.setStyleHint( TQFont::Times );
	app.setFont( f );
    }

    bool validOptions = false;

    if ( argc == 2 ) {
	validOptions = true;
	if ( strcmp(argv[1],"-poly") == 0 )
	    poly();
	else if ( strcmp(argv[1],"-rotate") == 0 )
	    rotate();
	else if ( strcmp(argv[1],"-troll") == 0 )
	    desktopText();
	else if ( strcmp(argv[1],"-trollwidget") == 0 )
	    desktopWidget();
	else
	    validOptions = false;
    }
    if ( argc == 3 ) {
	validOptions = true;
	if ( strcmp(argv[1],"-shadetext") == 0 )
	    desktopText( argv[2] );
	else if ( strcmp(argv[1],"-shadewidget") == 0 )
	    desktopWidget( argv[2] );
	else
	    validOptions = false;
    }
    if ( !validOptions ) {
	fprintf( stderr, "Usage:\n\tdesktop -poly"
			       "\n\tdesktop -rotate"
			       "\n\tdesktop -troll"
			       "\n\tdesktop -trollwidget"
			       "\n\tdesktop -shadetext <text>"
			       "\n\tdesktop -shadewidget <text>\n" );
	rotate();
    }
    return 0;
}
