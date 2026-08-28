#include "graph.h"
#include <ntqcanvas.h>
#include <stdlib.h>
#include <ntqdatetime.h>
#include <ntqhbox.h>
#include <ntqpushbutton.h>
#include <ntqslider.h>
#include <ntqlabel.h>
#include <ntqlayout.h>

const int bounce_rtti = 1234;

// We use a global variable to save memory - all the brushes and pens in
// the mesh are shared.
static TQBrush *tb = 0;
static TQPen *tp = 0;

class EdgeItem;
class NodeItem;
class FigureEditor;
typedef TQValueList<NodeItem*> NodeItemList;
typedef TQValueList<EdgeItem*> EdgeItemList;

#define SPEED2ADVANCE(x) (301-x)

class GraphWidgetPrivate
{
public:
    GraphWidgetPrivate() {
	moving = 0;
	speed = 275;
    }
    ~GraphWidgetPrivate() {
	delete canvas;
    }
    NodeItemList nodeItems;
    FigureEditor* editor;
    TQCanvas* canvas;
    TQCanvasItem* moving;
    int speed;
};

class EdgeItem: public TQCanvasLine
{
public:
    EdgeItem( NodeItem*, NodeItem*, TQCanvas* );
    void setFromPoint( int x, int y ) ;
    void setToPoint( int x, int y );
    void moveBy(double dx, double dy);

    NodeItem* from;
    NodeItem* to;
};



class NodeItem: public TQCanvasEllipse
{
public:
    NodeItem( GraphWidgetPrivate* g );
    ~NodeItem() {}

    void addInEdge( EdgeItem *edge ) { inList.append( edge ); }
    void addOutEdge( EdgeItem *edge ) { outList.append( edge ); }

    void moveBy(double dx, double dy);

    void calcForce();
    void advance( int stage );

private:
    GraphWidgetPrivate* graph;
    EdgeItemList inList;
    EdgeItemList outList;
};



void EdgeItem::moveBy(double, double)
{
    //nothing
}

EdgeItem::EdgeItem( NodeItem *fromItem, NodeItem *toItem, TQCanvas *canvas )
    : TQCanvasLine( canvas )
{
    from = fromItem;
    to = toItem;
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
    double nx = x() + dx;
    double ny = y() + dy;
    if ( graph->moving != this ) {
	nx = TQMAX( width()/2, nx );
	ny = TQMAX( height()/2, ny );
	nx = TQMIN( canvas()->width() - width()/2, nx );
	ny = TQMIN( canvas()->height() - height()/2, ny );
    }
    TQCanvasEllipse::moveBy( nx-x(), ny-y() );
    EdgeItemList::Iterator it;
    for (  it = inList.begin(); it != inList.end(); ++it )
	(*it)->setToPoint( int(x()), int(y()) );
    for (  it = outList.begin(); it != outList.end(); ++it )
	(*it)->setFromPoint( int(x()), int(y()) );
}

NodeItem::NodeItem( GraphWidgetPrivate* g )
    : TQCanvasEllipse( 32, 32, g->canvas )
{
    graph = g;
    graph->nodeItems.append( this );
    setPen( *tp );
    setBrush( *tb );
    setZ( 128 );
}

void NodeItem::advance( int stage ) {
	switch ( stage ) {
	case 0:
	    calcForce();
	    break;
	case 1:
	    TQCanvasItem::advance(stage);
	    break;
	}
}

void NodeItem::calcForce() {
    if ( graph->moving == this ) {
	setVelocity( 0, 0 );
	return;
    }
    double xvel = 0;
    double yvel = 0;
    for ( NodeItemList::Iterator it = graph->nodeItems.begin(); it != graph->nodeItems.end(); ++it ) {
	NodeItem* n = (*it);
	if ( n == this )
	    continue;
	double dx  = x() - n->x();
	double dy  = y() - n->y();
	double l = 2 * ( dx * dx + dy * dy );
	if ( l > 0 ) {
	    xvel = xvel + dx*260 / l;
	    yvel = yvel + dy*260 / l;
	}
    }
    double w = 1 + outList.count() + inList.count();
    w *= 10;
    EdgeItemList::Iterator it2;
    EdgeItem * e;
    NodeItem* n;
    for ( it2 = outList.begin(); it2 != outList.end(); ++it2 ) {
	e = (*it2);
	n = e->to;
	xvel = xvel - ( x() - n->x() ) / w;
	yvel = yvel - ( y() - n->y() ) / w;
    }
    for ( it2 = inList.begin(); it2 != inList.end(); ++it2 ) {
	e = (*it2);
	n = e->from;
	xvel = xvel - ( x() - n->x() ) / w;
	yvel = yvel - ( y() - n->y() ) / w;
    }
    if ( TQABS( xvel ) < .1 && TQABS( yvel ) < .1 )
	xvel = yvel = 0;
   setVelocity( xvel, yvel );
}


class FigureEditor : public TQCanvasView {
public:
    FigureEditor( GraphWidgetPrivate *g, TQWidget* parent=0, const char* name=0, WFlags f=0);

    TQSize sizeHint() const;


protected:
    void contentsMousePressEvent(TQMouseEvent*);
    void contentsMouseReleaseEvent(TQMouseEvent*);
    void contentsMouseMoveEvent(TQMouseEvent*);


    void resizeEvent( TQResizeEvent* );
    void showEvent( TQShowEvent* );
    void hideEvent( TQHideEvent* e);

private:
    void initialize();
    TQPoint moving_start;
    GraphWidgetPrivate* graph;
};


FigureEditor::FigureEditor(
	GraphWidgetPrivate* g, TQWidget* parent,
	const char* name, WFlags f) :
    TQCanvasView(g->canvas, parent,name,f)
{
    graph = g;
}

void FigureEditor::contentsMousePressEvent(TQMouseEvent* e)
{
    TQCanvasItemList l=canvas()->collisions(e->pos());
    for (TQCanvasItemList::Iterator it=l.begin(); it!=l.end(); ++it) {
	if ((*it)->rtti()==bounce_rtti )
	    continue;
	graph->moving = *it;
	moving_start = e->pos();
	return;
    }
    graph->moving = 0;
}

void FigureEditor::contentsMouseReleaseEvent(TQMouseEvent* )
{
    if ( graph->moving )
	graph->moving = 0;
}

void FigureEditor::contentsMouseMoveEvent(TQMouseEvent* e)
{
    if ( graph->moving ) {
	graph->moving->moveBy(e->pos().x() - moving_start.x(),
		       e->pos().y() - moving_start.y());
	moving_start = e->pos();
	canvas()->update();
    }
}

class BouncyText : public TQCanvasText {
    void initPos();
    void initSpeed();
public:
    int rtti() const;
    BouncyText(const TQString&, TQFont, TQCanvas*);
    void advance(int);
};

BouncyText::BouncyText( const TQString& text, TQFont f, TQCanvas* canvas) :
    TQCanvasText(text, f, canvas)
{
    setAnimated(true);
    initPos();
}


int BouncyText::rtti() const
{
    return bounce_rtti;
}

void BouncyText::initPos()
{
    initSpeed();
    int trial=1000;
    do {
	move(rand()%(canvas()->width()-boundingRect().width()),
	     rand()%(canvas()->height()-boundingRect().height()));
	advance(0);
    } while (trial-- && xVelocity()==0.0 && yVelocity()==0.0);
}

void BouncyText::initSpeed()
{
    const double speed = 2.0;
    double d = (double)(rand()%1024) / 1024.0;
    double e = (double)(rand()%1024) / 1024.0;

    if ( d < .5 )
	d = -1 - d;
    else
	d = d + 1;
    if ( e < .5 )
	e = -1 - e;
    else
	e = e + 1;

    setVelocity( d*speed, e * speed );
}

void BouncyText::advance( int stage )
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

	TQRect r = boundingRect();
	r.moveBy( int(vx), int(vy) );

	if ( r.left() < 0 || r.right() > canvas()->width() )
	    vx = -vx;
	if ( r.top() < 0 || r.bottom() > canvas()->height() )
	    vy = -vy;

	r = boundingRect();
	r.moveBy( int(vx), int(vy) );
	if ( r.left() < 0 || r.right() > canvas()->width() )
	    vx = 0;
	if ( r.top() < 0 || r.bottom() > canvas()->height() )
	    vy = 0;

	setVelocity( vx, vy );
      } break;
      case 1:
	TQCanvasItem::advance( stage );
	break;
    }
}

GraphWidget::GraphWidget( TQWidget *parent, const char *name)
    : TQWidget( parent, name )
{
    d = new GraphWidgetPrivate;
    d->canvas = 0;
    TQVBoxLayout* vb = new TQVBoxLayout(  this, 11, 6 );
    d->editor = new FigureEditor( d, this  );
    vb->addWidget( d->editor );
    TQHBoxLayout* hb = new TQHBoxLayout(  vb );
    hb->addWidget( new TQLabel("Slow", this ) );
    TQSlider* slider = new TQSlider( 0, 300, 25, d->speed, Horizontal, this );
    connect( slider, TQ_SIGNAL( valueChanged(int) ), this, TQ_SLOT( setSpeed(int) ) );
    hb->addWidget( slider );
    hb->addWidget( new TQLabel("Fast", this ) );
    hb->addSpacing( 10 );
    TQPushButton* btn = new TQPushButton( "Shuffle Nodes", this );
    connect( btn, TQ_SIGNAL( clicked() ), this, TQ_SLOT( shuffle() ) );
    hb->addWidget( btn );
}


GraphWidget::~GraphWidget()
{
    delete d;
}

void GraphWidget::setSpeed(int s)
{
    d->speed = s;
    if ( isVisible() && d->canvas )
	d->canvas->setAdvancePeriod( SPEED2ADVANCE( s ) );
}

void GraphWidget::shuffle()
{

    for ( NodeItemList::Iterator it = d->nodeItems.begin(); it != d->nodeItems.end(); ++it ) {
	NodeItem* ni = (*it);
	ni->move(rand()%(d->canvas->width()-ni->width()),rand()%(d->canvas->height()-ni->height()));
    }
}


TQSize FigureEditor::sizeHint() const
{
    return TQSize( 600, 400 );
}

void FigureEditor::resizeEvent( TQResizeEvent* e )
{
    if ( canvas() )
	canvas()->resize( contentsRect().width(), contentsRect().height() );
    TQCanvasView::resizeEvent( e );
}

void FigureEditor::showEvent( TQShowEvent* )
{
    initialize();
    canvas()->setAdvancePeriod( SPEED2ADVANCE(graph->speed) );
}

void FigureEditor::hideEvent( TQHideEvent* )
{
    initialize();
    canvas()->setAdvancePeriod( -10 );
}

void FigureEditor::initialize()
{
    if ( canvas() )
	return;
    resize( sizeHint() );
    graph->canvas = new TQCanvas( contentsRect().width(), contentsRect().height() );
    if ( !tb ) tb = new TQBrush( TQt::red );
    if ( !tp ) tp = new TQPen( TQt::black );
    srand( TQTime::currentTime().msec() );
    int nodecount = 0;

    int rows = 3;
    int cols = 3;

    TQMemArray<NodeItem*> lastRow(cols);
    for ( int r = 0; r < rows; r++ ) {
	NodeItem *prev = 0;
	for ( int c = 0; c < cols; c++ ) {
	    NodeItem *ni = new NodeItem( graph );
	    ni->setAnimated( true );
	    nodecount++;
	    ni->move(rand()%(graph->canvas->width()-ni->width()),rand()%(graph->canvas->height()-ni->height()));

	    if ( r > 0 )
		(new EdgeItem( lastRow[c], ni, graph->canvas ))->show();
	    if ( prev )
		(new EdgeItem( prev, ni, graph->canvas ))->show();
	    prev = ni;
	    lastRow[c] = ni;
	    ni->show();
	}
    }

    graph->canvas->advance();

    TQCanvasItem* i = new BouncyText( tr( "Drag the nodes around!" ), TQFont("helvetica", 24), graph->canvas);
    i->show();
    setCanvas( graph->canvas );
    setMinimumSize( 600, 400 );
    setSizePolicy( TQSizePolicy::MinimumExpanding, TQSizePolicy::MinimumExpanding );
}
