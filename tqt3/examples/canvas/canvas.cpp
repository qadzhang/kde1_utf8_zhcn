#include <ntqdatetime.h>
#include <ntqmainwindow.h>
#include <ntqstatusbar.h>
#include <ntqmessagebox.h>
#include <ntqmenubar.h>
#include <ntqapplication.h>
#include <ntqpainter.h>
#include <ntqprinter.h>
#include <ntqlabel.h>
#include <ntqimage.h>
#include <ntqprogressdialog.h>
#include "canvas.h"

#include <stdlib.h>

// We use a global variable to save memory - all the brushes and pens in
// the mesh are shared.
static TQBrush *tb = 0;
static TQPen *tp = 0;

class EdgeItem;
class NodeItem;

class EdgeItem: public TQCanvasLine
{
public:
    EdgeItem( NodeItem*, NodeItem*, TQCanvas *canvas );
    void setFromPoint( int x, int y ) ;
    void setToPoint( int x, int y );
    static int count() { return c; }
    void moveBy(double dx, double dy);
private:
    static int c;
};

static const int imageRTTI = 984376;


class ImageItem: public TQCanvasRectangle
{
public:
    ImageItem( TQImage img, TQCanvas *canvas );
    int rtti () const { return imageRTTI; }
    bool hit( const TQPoint&) const;
protected:
    void drawShape( TQPainter & );
private:
    TQImage image;
    TQPixmap pixmap;
};


ImageItem::ImageItem( TQImage img, TQCanvas *canvas )
    : TQCanvasRectangle( canvas ), image(img)
{
    setSize( image.width(), image.height() );

#if !defined(TQ_WS_QWS)
    pixmap.convertFromImage(image, OrderedAlphaDither);
#endif
}


void ImageItem::drawShape( TQPainter &p )
{
// On TQt/Embedded, we can paint a TQImage as fast as a TQPixmap,
// but on other platforms, we need to use a TQPixmap.
#if defined(TQ_WS_QWS)
    p.drawImage( int(x()), int(y()), image, 0, 0, -1, -1, OrderedAlphaDither );
#else
    p.drawPixmap( int(x()), int(y()), pixmap );
#endif
}

bool ImageItem::hit( const TQPoint &p ) const
{
    int ix = p.x()-int(x());
    int iy = p.y()-int(y());
    if ( !image.valid( ix , iy ) )
	return false;
    TQRgb pixel = image.pixel( ix, iy );
    return tqAlpha( pixel ) != 0;
}

class NodeItem: public TQCanvasEllipse
{
public:
    NodeItem( TQCanvas *canvas );
    ~NodeItem() {}

    void addInEdge( EdgeItem *edge ) { inList.append( edge ); }
    void addOutEdge( EdgeItem *edge ) { outList.append( edge ); }

    void moveBy(double dx, double dy);

    //    TQPoint center() { return boundingRect().center(); }
private:
    TQPtrList<EdgeItem> inList;
    TQPtrList<EdgeItem> outList;
};


int EdgeItem::c = 0;


void EdgeItem::moveBy(double, double)
{
    //nothing
}

EdgeItem::EdgeItem( NodeItem *from, NodeItem *to, TQCanvas *canvas )
    : TQCanvasLine( canvas )
{
    c++;
    setPen( *tp );
    setBrush( *tb );
    from->addOutEdge( this );
    to->addInEdge( this );
    setPoints( int(from->x()), int(from->y()), int(to->x()), int(to->y()) );
    setZ( 127 );
}

void EdgeItem::setFromPoint( int x, int y )
{
    setPoints( x,y, endPoint().x(), endPoint().y() );
}

void EdgeItem::setToPoint( int x, int y )
{
    setPoints( startPoint().x(), startPoint().y(), x, y );
}



void NodeItem::moveBy(double dx, double dy)
{
    TQCanvasEllipse::moveBy( dx, dy );

    TQPtrListIterator<EdgeItem> it1( inList );
    EdgeItem *edge;
    while (( edge = it1.current() )) {
	++it1;
	edge->setToPoint( int(x()), int(y()) );
    }
    TQPtrListIterator<EdgeItem> it2( outList );
    while (( edge = it2.current() )) {
	++it2;
	edge->setFromPoint( int(x()), int(y()) );
    }
}

NodeItem::NodeItem( TQCanvas *canvas )
    : TQCanvasEllipse( 6, 6, canvas )
{
    setPen( *tp );
    setBrush( *tb );
    setZ( 128 );
}

FigureEditor::FigureEditor(
	TQCanvas& c, TQWidget* parent,
	const char* name, WFlags f) :
    TQCanvasView(&c,parent,name,f)
{
}

void FigureEditor::contentsMousePressEvent(TQMouseEvent* e)
{
    TQPoint p = inverseWorldMatrix().map(e->pos());
    TQCanvasItemList l=canvas()->collisions(p);
    for (TQCanvasItemList::Iterator it=l.begin(); it!=l.end(); ++it) {
	if ( (*it)->rtti() == imageRTTI ) {
	    ImageItem *item= (ImageItem*)(*it);
	    if ( !item->hit( p ) )
		 continue;
	}
	moving = *it;
	moving_start = p;
	return;
    }
    moving = 0;
}

void FigureEditor::clear()
{
    TQCanvasItemList list = canvas()->allItems();
    TQCanvasItemList::Iterator it = list.begin();
    for (; it != list.end(); ++it) {
	if ( *it )
	    delete *it;
    }
}

void FigureEditor::contentsMouseMoveEvent(TQMouseEvent* e)
{
    if ( moving ) {
	TQPoint p = inverseWorldMatrix().map(e->pos());
	moving->moveBy(p.x() - moving_start.x(),
		       p.y() - moving_start.y());
	moving_start = p;
	canvas()->update();
    }
}

static uint bouncyLogoCount = 0;
static TQCanvasPixmapArray *logoarr = 0;

BouncyLogo::BouncyLogo(TQCanvas* canvas) :
    TQCanvasSprite(0,canvas)
{
    if ( !logoarr ) {
	logoarr = new TQCanvasPixmapArray("qt-trans.xpm");
    }
    bouncyLogoCount++;
    setSequence(logoarr);
    setAnimated(true);
    initPos();
}

BouncyLogo::~BouncyLogo()
{
    --bouncyLogoCount;
}


const int logo_rtti = 1234;

int BouncyLogo::rtti() const
{
    return logo_rtti;
}

void BouncyLogo::initPos()
{
    initSpeed();
    int trial=1000;
    do {
	move(rand()%canvas()->width(),rand()%canvas()->height());
	advance(0);
    } while (trial-- && xVelocity()==0.0 && yVelocity()==0.0);
}

void BouncyLogo::initSpeed()
{
    const double speed = 4.0;
    double d = (double)(rand()%1024) / 1024.0;
    setVelocity( d*speed*2-speed, (1-d)*speed*2-speed );
}

void BouncyLogo::advance(int stage)
{
    switch ( stage ) {
      case 0: {
	double vx = xVelocity();
	double vy = yVelocity();

	if ( vx == 0.0 && vy == 0.0 ) {
	    // stopped last turn
	    initSpeed();
	    vx = xVelocity();
	    vy = yVelocity();
	}

	double nx = x() + vx;
	double ny = y() + vy;

	if ( nx < 0 || nx >= canvas()->width() )
	    vx = -vx;
	if ( ny < 0 || ny >= canvas()->height() )
	    vy = -vy;

	for (int bounce=0; bounce<4; bounce++) {
	    TQCanvasItemList l=collisions(false);
	    for (TQCanvasItemList::Iterator it=l.begin(); it!=l.end(); ++it) {
		TQCanvasItem *hit = *it;
		if ( hit->rtti()==logo_rtti && hit->collidesWith(this) ) {
		    switch ( bounce ) {
		      case 0:
			vx = -vx;
			break;
		      case 1:
			vy = -vy;
			vx = -vx;
			break;
		      case 2:
			vx = -vx;
			break;
		      case 3:
			// Stop for this turn
			vx = 0;
			vy = 0;
			break;
		    }
		    setVelocity(vx,vy);
		    break;
		}
	    }
	}

	if ( x()+vx < 0 || x()+vx >= canvas()->width() )
	    vx = 0;
	if ( y()+vy < 0 || y()+vy >= canvas()->height() )
	    vy = 0;

	setVelocity(vx,vy);
      } break;
      case 1:
	TQCanvasItem::advance(stage);
	break;
    }
}

// Some objects to cache among different views
static uint mainCount = 0;
static TQImage *butterflyimg = 0;
static TQImage *logoimg = 0;

Main::Main(TQCanvas& c, TQWidget* parent, const char* name, WFlags f) :
    TQMainWindow(parent,name,f),
    canvas(c)
{
    editor = new FigureEditor(canvas,this);
    TQMenuBar* menu = menuBar();

    TQPopupMenu* file = new TQPopupMenu( menu );
    file->insertItem("&Fill canvas", this, TQ_SLOT(init()), CTRL+Key_F);
    file->insertItem("&Erase canvas", this, TQ_SLOT(clear()), CTRL+Key_E);
    file->insertItem("&New view", this, TQ_SLOT(newView()), CTRL+Key_N);
    file->insertSeparator();
    file->insertItem("&Print...", this, TQ_SLOT(print()), CTRL+Key_P);
    file->insertSeparator();
    file->insertItem("E&xit", tqApp, TQ_SLOT(quit()), CTRL+Key_Q);
    menu->insertItem("&File", file);

    TQPopupMenu* edit = new TQPopupMenu( menu );
    edit->insertItem("Add &Circle", this, TQ_SLOT(addCircle()), ALT+Key_C);
    edit->insertItem("Add &Hexagon", this, TQ_SLOT(addHexagon()), ALT+Key_H);
    edit->insertItem("Add &Polygon", this, TQ_SLOT(addPolygon()), ALT+Key_P);
    edit->insertItem("Add Spl&ine", this, TQ_SLOT(addSpline()), ALT+Key_I);
    edit->insertItem("Add &Text", this, TQ_SLOT(addText()), ALT+Key_T);
    edit->insertItem("Add &Line", this, TQ_SLOT(addLine()), ALT+Key_L);
    edit->insertItem("Add &Rectangle", this, TQ_SLOT(addRectangle()), ALT+Key_R);
    edit->insertItem("Add &Sprite", this, TQ_SLOT(addSprite()), ALT+Key_S);
    edit->insertItem("Create &Mesh", this, TQ_SLOT(addMesh()), ALT+Key_M );
    edit->insertItem("Add &Alpha-blended image", this, TQ_SLOT(addButterfly()), ALT+Key_A);
    menu->insertItem("&Edit", edit);

    TQPopupMenu* view = new TQPopupMenu( menu );
    view->insertItem("&Enlarge", this, TQ_SLOT(enlarge()), SHIFT+CTRL+Key_Plus);
    view->insertItem("Shr&ink", this, TQ_SLOT(shrink()), SHIFT+CTRL+Key_Minus);
    view->insertSeparator();
    view->insertItem("&Rotate clockwise", this, TQ_SLOT(rotateClockwise()), CTRL+Key_PageDown);
    view->insertItem("Rotate &counterclockwise", this, TQ_SLOT(rotateCounterClockwise()), CTRL+Key_PageUp);
    view->insertItem("&Zoom in", this, TQ_SLOT(zoomIn()), CTRL+Key_Plus);
    view->insertItem("Zoom &out", this, TQ_SLOT(zoomOut()), CTRL+Key_Minus);
    view->insertItem("Translate left", this, TQ_SLOT(moveL()), CTRL+Key_Left);
    view->insertItem("Translate right", this, TQ_SLOT(moveR()), CTRL+Key_Right);
    view->insertItem("Translate up", this, TQ_SLOT(moveU()), CTRL+Key_Up);
    view->insertItem("Translate down", this, TQ_SLOT(moveD()), CTRL+Key_Down);
    view->insertItem("&Mirror", this, TQ_SLOT(mirror()), CTRL+Key_Home);
    menu->insertItem("&View", view);

    options = new TQPopupMenu( menu );
    dbf_id = options->insertItem("Double buffer", this, TQ_SLOT(toggleDoubleBuffer()));
    options->setItemChecked(dbf_id, true);
    menu->insertItem("&Options",options);

    menu->insertSeparator();

    TQPopupMenu* help = new TQPopupMenu( menu );
    help->insertItem("&About", this, TQ_SLOT(help()), Key_F1);
    help->setItemChecked(dbf_id, true);
    menu->insertItem("&Help",help);

    statusBar();

    setCentralWidget(editor);

    printer = 0;

    mainCount++;

    init();
}

void Main::init()
{
    clear();

    static int r=24;
    srand(++r);

    int i;
    for ( i=0; i<canvas.width() / 56; i++) {
	addButterfly();
    }
    for ( i=0; i<canvas.width() / 85; i++) {
	addHexagon();
    }
    for ( i=0; i<canvas.width() / 128; i++) {
	addLogo();
    }
}

Main::~Main()
{
    delete printer;
    if ( !--mainCount ) {
	delete[] butterflyimg;
	butterflyimg = 0;
	delete[] logoimg;
	logoimg = 0;
	if ( bouncyLogoCount == 0 ) {
	    delete logoarr;
	    logoarr = 0;
	}
    }
}

void Main::newView()
{
    // Open a new view... have it delete when closed.
    Main *m = new Main(canvas, 0, 0, WDestructiveClose);
    tqApp->setMainWidget(m);
    m->show();
    tqApp->setMainWidget(0);
}

void Main::clear()
{
    editor->clear();
}

void Main::help()
{
    static TQMessageBox* about = new TQMessageBox( "TQt Canvas Example",
	    "<h3>The TQCanvas classes example</h3>"
	    "<ul>"
		"<li> Press ALT-S for some sprites."
		"<li> Press ALT-C for some circles."
		"<li> Press ALT-L for some lines."
		"<li> Drag the objects around."
		"<li> Read the code!"
	    "</ul>", TQMessageBox::Information, 1, 0, 0, this, 0, false );
    about->setButtonText( 1, "Dismiss" );
    about->show();
}

void Main::aboutTQt()
{
    TQMessageBox::aboutTQt( this, "TQt Canvas Example" );
}

void Main::toggleDoubleBuffer()
{
    bool s = !options->isItemChecked(dbf_id);
    options->setItemChecked(dbf_id,s);
    canvas.setDoubleBuffering(s);
}

void Main::enlarge()
{
    canvas.resize(canvas.width()*4/3, canvas.height()*4/3);
}

void Main::shrink()
{
    canvas.resize(canvas.width()*3/4, canvas.height()*3/4);
}

void Main::rotateClockwise()
{
    TQWMatrix m = editor->worldMatrix();
    m.rotate( 22.5 );
    editor->setWorldMatrix( m );
}

void Main::rotateCounterClockwise()
{
    TQWMatrix m = editor->worldMatrix();
    m.rotate( -22.5 );
    editor->setWorldMatrix( m );
}

void Main::zoomIn()
{
    TQWMatrix m = editor->worldMatrix();
    m.scale( 2.0, 2.0 );
    editor->setWorldMatrix( m );
}

void Main::zoomOut()
{
    TQWMatrix m = editor->worldMatrix();
    m.scale( 0.5, 0.5 );
    editor->setWorldMatrix( m );
}

void Main::mirror()
{
    TQWMatrix m = editor->worldMatrix();
    m.scale( -1, 1 );
    editor->setWorldMatrix( m );
}

void Main::moveL()
{
    TQWMatrix m = editor->worldMatrix();
    m.translate( -16, 0 );
    editor->setWorldMatrix( m );
}

void Main::moveR()
{
    TQWMatrix m = editor->worldMatrix();
    m.translate( +16, 0 );
    editor->setWorldMatrix( m );
}

void Main::moveU()
{
    TQWMatrix m = editor->worldMatrix();
    m.translate( 0, -16 );
    editor->setWorldMatrix( m );
}

void Main::moveD()
{
    TQWMatrix m = editor->worldMatrix();
    m.translate( 0, +16 );
    editor->setWorldMatrix( m );
}

void Main::print()
{
    if ( !printer ) printer = new TQPrinter;
    if ( printer->setup(this) ) {
	TQPainter pp(printer);
	canvas.drawArea(TQRect(0,0,canvas.width(),canvas.height()),&pp,false);
    }
}


void Main::addSprite()
{
    TQCanvasItem* i = new BouncyLogo(&canvas);
    i->setZ(rand()%256);
    i->show();
}

TQString butterfly_fn;
TQString logo_fn;


void Main::addButterfly()
{
    if ( butterfly_fn.isEmpty() )
	return;
    if ( !butterflyimg ) {
	butterflyimg = new TQImage[4];
	butterflyimg[0].load( butterfly_fn );
	butterflyimg[1] = butterflyimg[0].smoothScale( int(butterflyimg[0].width()*0.75),
		int(butterflyimg[0].height()*0.75) );
	butterflyimg[2] = butterflyimg[0].smoothScale( int(butterflyimg[0].width()*0.5),
		int(butterflyimg[0].height()*0.5) );
	butterflyimg[3] = butterflyimg[0].smoothScale( int(butterflyimg[0].width()*0.25),
		int(butterflyimg[0].height()*0.25) );
    }
    TQCanvasPolygonalItem* i = new ImageItem(butterflyimg[rand()%4],&canvas);
    i->move(rand()%(canvas.width()-butterflyimg->width()),
	    rand()%(canvas.height()-butterflyimg->height()));
    i->setZ(rand()%256+250);
    i->show();
}

void Main::addLogo()
{
    if ( logo_fn.isEmpty() )
	return;
    if ( !logoimg ) {
	logoimg = new TQImage[4];
	logoimg[0].load( logo_fn );
	logoimg[1] = logoimg[0].smoothScale( int(logoimg[0].width()*0.75),
		int(logoimg[0].height()*0.75) );
	logoimg[2] = logoimg[0].smoothScale( int(logoimg[0].width()*0.5),
		int(logoimg[0].height()*0.5) );
	logoimg[3] = logoimg[0].smoothScale( int(logoimg[0].width()*0.25),
		int(logoimg[0].height()*0.25) );
    }
    TQCanvasPolygonalItem* i = new ImageItem(logoimg[rand()%4],&canvas);
    i->move(rand()%(canvas.width()-logoimg->width()),
	    rand()%(canvas.height()-logoimg->width()));
    i->setZ(rand()%256+256);
    i->show();
}



void Main::addCircle()
{
    TQCanvasPolygonalItem* i = new TQCanvasEllipse(50,50,&canvas);
    i->setBrush( TQColor(rand()%32*8,rand()%32*8,rand()%32*8) );
    i->move(rand()%canvas.width(),rand()%canvas.height());
    i->setZ(rand()%256);
    i->show();
}

void Main::addHexagon()
{
    TQCanvasPolygon* i = new TQCanvasPolygon(&canvas);
    const int size = canvas.width() / 25;
    TQPointArray pa(6);
    pa[0] = TQPoint(2*size,0);
    pa[1] = TQPoint(size,-size*173/100);
    pa[2] = TQPoint(-size,-size*173/100);
    pa[3] = TQPoint(-2*size,0);
    pa[4] = TQPoint(-size,size*173/100);
    pa[5] = TQPoint(size,size*173/100);
    i->setPoints(pa);
    i->setBrush( TQColor(rand()%32*8,rand()%32*8,rand()%32*8) );
    i->move(rand()%canvas.width(),rand()%canvas.height());
    i->setZ(rand()%256);
    i->show();
}

void Main::addPolygon()
{
    TQCanvasPolygon* i = new TQCanvasPolygon(&canvas);
    const int size = canvas.width()/2;
    TQPointArray pa(6);
    pa[0] = TQPoint(0,0);
    pa[1] = TQPoint(size,size/5);
    pa[2] = TQPoint(size*4/5,size);
    pa[3] = TQPoint(size/6,size*5/4);
    pa[4] = TQPoint(size*3/4,size*3/4);
    pa[5] = TQPoint(size*3/4,size/4);
    i->setPoints(pa);
    i->setBrush( TQColor(rand()%32*8,rand()%32*8,rand()%32*8) );
    i->move(rand()%canvas.width(),rand()%canvas.height());
    i->setZ(rand()%256);
    i->show();
}

void Main::addSpline()
{
    TQCanvasSpline* i = new TQCanvasSpline(&canvas);
    const int size = canvas.width()/6;
    TQPointArray pa(12);
    pa[0] = TQPoint(0,0);
    pa[1] = TQPoint(size/2,0);
    pa[2] = TQPoint(size,size/2);
    pa[3] = TQPoint(size,size);
    pa[4] = TQPoint(size,size*3/2);
    pa[5] = TQPoint(size/2,size*2);
    pa[6] = TQPoint(0,size*2);
    pa[7] = TQPoint(-size/2,size*2);
    pa[8] = TQPoint(size/4,size*3/2);
    pa[9] = TQPoint(0,size);
    pa[10]= TQPoint(-size/4,size/2);
    pa[11]= TQPoint(-size/2,0);
    i->setControlPoints(pa);
    i->setBrush( TQColor(rand()%32*8,rand()%32*8,rand()%32*8) );
    i->move(rand()%canvas.width(),rand()%canvas.height());
    i->setZ(rand()%256);
    i->show();
}

void Main::addText()
{
    TQCanvasText* i = new TQCanvasText(&canvas);
    i->setText("TQCanvasText");
    i->move(rand()%canvas.width(),rand()%canvas.height());
    i->setZ(rand()%256);
    i->show();
}

void Main::addLine()
{
    TQCanvasLine* i = new TQCanvasLine(&canvas);
    i->setPoints( rand()%canvas.width(), rand()%canvas.height(),
		  rand()%canvas.width(), rand()%canvas.height() );
    i->setPen( TQPen(TQColor(rand()%32*8,rand()%32*8,rand()%32*8), 6) );
    i->setZ(rand()%256);
    i->show();
}

void Main::addMesh()
{
    int x0 = 0;
    int y0 = 0;

    if ( !tb ) tb = new TQBrush( TQt::red );
    if ( !tp ) tp = new TQPen( TQt::black );

    int nodecount = 0;

    int w = canvas.width();
    int h = canvas.height();

    const int dist = 30;
    int rows = h / dist;
    int cols = w / dist;

#ifndef TQT_NO_PROGRESSDIALOG
    TQProgressDialog progress( "Creating mesh...", "Abort", rows,
			      this, "progress", true );
#endif

    TQMemArray<NodeItem*> lastRow(cols);
    for ( int j = 0; j < rows; j++ ) {
	int n = j%2 ? cols-1 : cols;
	NodeItem *prev = 0;
	for ( int i = 0; i < n; i++ ) {
	    NodeItem *el = new NodeItem( &canvas );
	    nodecount++;
	    int r = rand();
	    int xrand = r %20;
	    int yrand = (r/20) %20;
	    el->move( xrand + x0 + i*dist + (j%2 ? dist/2 : 0 ),
		      yrand + y0 + j*dist );

	    if ( j > 0 ) {
		if ( i < cols-1 )
		    (new EdgeItem( lastRow[i], el, &canvas ))->show();
		if ( j%2 )
		    (new EdgeItem( lastRow[i+1], el, &canvas ))->show();
		else if ( i > 0 )
		    (new EdgeItem( lastRow[i-1], el, &canvas ))->show();
	    }
	    if ( prev ) {
		(new EdgeItem( prev, el, &canvas ))->show();
	    }
	    if ( i > 0 ) lastRow[i-1] = prev;
	    prev = el;
	    el->show();
	}
	lastRow[n-1]=prev;
#ifndef TQT_NO_PROGRESSDIALOG
	progress.setProgress( j );
	if ( progress.wasCancelled() )
	    break;
#endif
    }
#ifndef TQT_NO_PROGRESSDIALOG
    progress.setProgress( rows );
#endif
    // tqDebug( "%d nodes, %d edges", nodecount, EdgeItem::count() );
}

void Main::addRectangle()
{
    TQCanvasPolygonalItem *i = new TQCanvasRectangle( rand()%canvas.width(),rand()%canvas.height(),
			    canvas.width()/5,canvas.width()/5,&canvas);
    int z = rand()%256;
    i->setBrush( TQColor(z,z,z) );
    i->setPen( TQPen(TQColor(rand()%32*8,rand()%32*8,rand()%32*8), 6) );
    i->setZ(z);
    i->show();
}
