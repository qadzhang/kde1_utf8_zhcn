/****************************************************************************
**
** Copyright (C) 1992-2008 Trolltech ASA.  All rights reserved.
**
** This file is part of an example program for TQt.  This example
** program may be used, distributed and modified without limitation.
**
*****************************************************************************/

#include <ntqscrollview.h>
#include <ntqapplication.h>
#include <ntqmenubar.h>
#include <ntqpopupmenu.h>
#include <ntqpushbutton.h>
#include <ntqpainter.h>
#include <ntqpixmap.h>
#include <ntqmessagebox.h>
#include <ntqlayout.h>
#include <ntqlabel.h>
#include <ntqmultilineedit.h>
#include <ntqsizegrip.h>
#include <stdlib.h>


static const int style_id	= 0x1000;
static const int lw_id		= 0x2000;
static const int mlw_id		= 0x4000;
static const int mw_id		= 0x8000;
static const int max_lw		= 16;
static const int max_mlw	= 5;
static const int max_mw		= 10;


class BigShrinker : public TQFrame {
    TQ_OBJECT
public:
    BigShrinker(TQWidget* parent) :
	TQFrame(parent)
    {
	setFrameStyle(TQFrame::Box|TQFrame::Sunken);
	int h=35;
	int b=0;
	for (int y=0; y<2000-h; y+=h+10) {
	    if (y == 0) {
		TQButton* q=new TQPushButton("Quit", this);
		connect(q, TQ_SIGNAL(clicked()), tqApp, TQ_SLOT(quit()));
	    } else {
		TQString str;
		if ( b > 0 ) {
		    str.sprintf("Button %d", b++);
		} else {
		    str = "I'm shrinking!";
		    ++b;
		}
		(new TQPushButton(str, this))->move(y/2,y);
	    }
	}
	resize(1000,2000);

	startTimer(250);
    }

    void timerEvent(TQTimerEvent*)
    {
	int w=width();
	int h=height();
	if ( w > 50 ) w -= 1;
	if ( h > 50 ) h -= 2;
	resize(w,h);
    }

    void mouseReleaseEvent(TQMouseEvent* e)
    {
	emit clicked(e->x(), e->y());
    }

signals:
    void clicked(int,int);
};

class BigMatrix : public TQScrollView {
    TQMultiLineEdit *dragging;
public:
    BigMatrix(TQWidget* parent) :
	TQScrollView(parent,"matrix", WStaticContents),
	bg("bg.ppm")
    {
	bg.load("bg.ppm");
	resizeContents(400000,300000);

	dragging = 0;
    }

    void viewportMousePressEvent(TQMouseEvent* e)
    {
	int x, y;
	viewportToContents( e->x(),  e->y(), x, y );
	dragging = new TQMultiLineEdit(viewport(),"Another");
	dragging->setText("Thanks!");
	dragging->resize(100,100);
	addChild(dragging, x, y);
	showChild(dragging);
    }

    void viewportMouseReleaseEvent(TQMouseEvent*)
    {
	dragging = 0;
    }

    void viewportMouseMoveEvent(TQMouseEvent* e)
    {
	if ( dragging ) {
	    int mx, my;
	    viewportToContents( e->x(),  e->y(), mx, my );
	    int cx = childX(dragging);
	    int cy = childY(dragging);
	    int w = mx - cx + 1;
	    int h = my - cy + 1;
	    TQString msg;
	    msg.sprintf("at (%d,%d) %d by %d",cx,cy,w,h);
	    dragging->setText(msg);
	    dragging->resize(w,h);
	}
    }

protected:
    void drawContents(TQPainter* p, int cx, int cy, int cw, int ch)
    {
	// The Background
	if ( !bg.isNull() ) {
	    int rowheight=bg.height();
	    int toprow=cy/rowheight;
	    int bottomrow=(cy+ch+rowheight-1)/rowheight;
	    int colwidth=bg.width();
	    int leftcol=cx/colwidth;
	    int rightcol=(cx+cw+colwidth-1)/colwidth;
	    for (int r=toprow; r<=bottomrow; r++) {
		int py=r*rowheight;
		for (int c=leftcol; c<=rightcol; c++) {
		    int px=c*colwidth;
		    p->drawPixmap(px, py, bg);
		}
	    }
	} else {
	    p->fillRect(cx, cy, cw, ch, TQColor(240,222,208));
	}

	// The Numbers
	{
	    TQFontMetrics fm=p->fontMetrics();
	    int rowheight=fm.lineSpacing();
	    int toprow=cy/rowheight;
	    int bottomrow=(cy+ch+rowheight-1)/rowheight;
	    int colwidth=fm.width("00000,000000 ")+3;
	    int leftcol=cx/colwidth;
	    int rightcol=(cx+cw+colwidth-1)/colwidth;
	    TQString str;
	    for (int r=toprow; r<=bottomrow; r++) {
		int py=r*rowheight;
		for (int c=leftcol; c<=rightcol; c++) {
		    int px=c*colwidth;
		    str.sprintf("%d,%d",c,r);
		    p->drawText(px+3, py+fm.ascent(), str);
		}
	    }

	    // The Big Hint
	    if (leftcol<10 && toprow<5) {
		p->setFont(TQFont("Charter",30));
		p->setPen(red);
		TQString text;
		text.sprintf("HINT:  Look at %d,%d",215000/colwidth,115000/rowheight);
		p->drawText(100,50,text);
	    }
	}

	// The Big X
	{
	    if (cx+cw>200000 && cy+ch>100000 && cx<230000 && cy<130000) {
		// Note that some X server cannot even handle co-ordinates
		// beyond about 4000, so you might not see this.
		p->drawLine(200000,100000,229999,129999);
		p->drawLine(229999,100000,200000,129999);

		// X marks the spot!
		p->setFont(TQFont("Charter",100));
		p->setPen(blue);
		p->drawText(215000-500,115000-100,1000,200,AlignCenter,"YOU WIN!!!!!");
	    }
	}
    }

private:
    TQPixmap bg;
};

class ScrollViewExample : public TQWidget {
    TQ_OBJECT

public:
    ScrollViewExample(int technique, TQWidget* parent=0, const char* name=0) :
	TQWidget(parent,name)
    {
	TQMenuBar* menubar = new TQMenuBar(this);
	TQ_CHECK_PTR( menubar );

	TQPopupMenu* file = new TQPopupMenu( menubar );
	TQ_CHECK_PTR( file );
	menubar->insertItem( "&File", file );
	file->insertItem( "Quit", tqApp,  TQ_SLOT(quit()) );

	vp_options = new TQPopupMenu( menubar );
	TQ_CHECK_PTR( vp_options );
	vp_options->setCheckable( true );
	menubar->insertItem( "&ScrollView", vp_options );
	connect( vp_options, TQ_SIGNAL(activated(int)),
	    this, TQ_SLOT(doVPMenuItem(int)) );

	vauto_id = vp_options->insertItem( "Vertical Auto" );
	vaoff_id = vp_options->insertItem( "Vertical AlwaysOff" );
	vaon_id = vp_options->insertItem( "Vertical AlwaysOn" );
	vp_options->insertSeparator();
	hauto_id = vp_options->insertItem( "Horizontal Auto" );
	haoff_id = vp_options->insertItem( "Horizontal AlwaysOff" );
	haon_id = vp_options->insertItem( "Horizontal AlwaysOn" );
	vp_options->insertSeparator();
	corn_id = vp_options->insertItem( "cornerWidget" );

	if (technique == 1) {
	    vp = new TQScrollView(this);
	    BigShrinker *bs = new BigShrinker(0);//(vp->viewport());
	    vp->addChild(bs);
	    bs->setAcceptDrops(true);
	    TQObject::connect(bs, TQ_SIGNAL(clicked(int,int)),
			    vp, TQ_SLOT(center(int,int)));
	} else {
	    vp = new BigMatrix(this);
	    if ( technique == 3 )
		vp->enableClipper(true);
	    srand(1);
	    for (int i=0; i<30; i++) {
		TQMultiLineEdit *l = new TQMultiLineEdit(vp->viewport(),"First");
		l->setText("Drag out more of these.");
		l->resize(100,100);
		vp->addChild(l, rand()%800, rand()%10000);
	    }
	    vp->viewport()->setBackgroundMode(NoBackground);
	}

	f_options = new TQPopupMenu( menubar );
	TQ_CHECK_PTR( f_options );
	f_options->setCheckable( true );
	menubar->insertItem( "F&rame", f_options );
	connect( f_options, TQ_SIGNAL(activated(int)),
	    this, TQ_SLOT(doFMenuItem(int)) );

	f_options->insertItem( "No Frame", style_id );
	f_options->insertItem( "Box", style_id|TQFrame::Box );
	f_options->insertItem( "Panel", style_id|TQFrame::Panel );
	f_options->insertItem( "WinPanel", style_id|TQFrame::WinPanel );
	f_options->insertSeparator();
	f_options->insertItem( "Plain", style_id|TQFrame::Plain );
	f_options->insertItem( "Raised", style_id|TQFrame::Raised );
	f_laststyle = f_options->indexOf(
	    f_options->insertItem( "Sunken", style_id|TQFrame::Sunken ));
	f_options->insertSeparator();
	lw_options = new TQPopupMenu( menubar );
	TQ_CHECK_PTR( lw_options );
	lw_options->setCheckable( true );
	for (int lw = 1; lw <= max_lw; lw++) {
	    TQString str;
	    str.sprintf("%d Pixels", lw);
	    lw_options->insertItem( str, lw_id | lw );
	}
	f_options->insertItem( "Line Width", lw_options );
	connect( lw_options, TQ_SIGNAL(activated(int)),
	    this, TQ_SLOT(doFMenuItem(int)) );
	mlw_options = new TQPopupMenu( menubar );
	TQ_CHECK_PTR( mlw_options );
	mlw_options->setCheckable( true );
	for (int mlw = 0; mlw <= max_mlw; mlw++) {
	    TQString str;
	    str.sprintf("%d Pixels", mlw);
	    mlw_options->insertItem( str, mlw_id | mlw );
	}
	f_options->insertItem( "Midline Width", mlw_options );
	connect( mlw_options, TQ_SIGNAL(activated(int)),
	    this, TQ_SLOT(doFMenuItem(int)) );
	mw_options = new TQPopupMenu( menubar );
	TQ_CHECK_PTR( mw_options );
	mw_options->setCheckable( true );
	for (int mw = 0; mw <= max_mw; mw++) {
	    TQString str;
	    str.sprintf("%d Pixels", mw);
	    mw_options->insertItem( str, mw_id | mw );
	}
	f_options->insertItem( "Margin Width", mw_options );
	connect( mw_options, TQ_SIGNAL(activated(int)),
	    this, TQ_SLOT(doFMenuItem(int)) );

	setVPMenuItems();
	setFMenuItems();

	TQVBoxLayout* vbox = new TQVBoxLayout(this);
	vbox->setMenuBar(menubar);
	menubar->setSeparator(TQMenuBar::InWindowsStyle);
	vbox->addWidget(vp);
	vbox->activate();

	corner = new TQSizeGrip(this);
	corner->hide();
    }

private slots:
    void doVPMenuItem(int id)
    {
	if (id == vauto_id ) {
	    vp->setVScrollBarMode(TQScrollView::Auto);
	} else if (id == vaoff_id) {
	    vp->setVScrollBarMode(TQScrollView::AlwaysOff);
	} else if (id == vaon_id) {
	    vp->setVScrollBarMode(TQScrollView::AlwaysOn);
	} else if (id == hauto_id) {
	    vp->setHScrollBarMode(TQScrollView::Auto);
	} else if (id == haoff_id) {
	    vp->setHScrollBarMode(TQScrollView::AlwaysOff);
	} else if (id == haon_id) {
	    vp->setHScrollBarMode(TQScrollView::AlwaysOn);
	} else if (id == corn_id) {
	    bool corn = !vp->cornerWidget();
	    vp->setCornerWidget(corn ? corner : 0);
	} else {
	    return; // Not for us to process.
	}
	setVPMenuItems();
    }

    void setVPMenuItems()
    {
	TQScrollView::ScrollBarMode vm = vp->vScrollBarMode();
	vp_options->setItemChecked( vauto_id, vm == TQScrollView::Auto );
	vp_options->setItemChecked( vaoff_id, vm == TQScrollView::AlwaysOff );
	vp_options->setItemChecked( vaon_id, vm == TQScrollView::AlwaysOn );

	TQScrollView::ScrollBarMode hm = vp->hScrollBarMode();
	vp_options->setItemChecked( hauto_id, hm == TQScrollView::Auto );
	vp_options->setItemChecked( haoff_id, hm == TQScrollView::AlwaysOff );
	vp_options->setItemChecked( haon_id, hm == TQScrollView::AlwaysOn );

	vp_options->setItemChecked( corn_id, !!vp->cornerWidget() );
    }

    void doFMenuItem(int id)
    {
	if (id & style_id) {
	    int sty;

	    if (id == style_id) {
		sty = 0;
	    } else if (id & TQFrame::MShape) {
		sty = vp->frameStyle()&TQFrame::MShadow;
		sty = (sty ? sty : TQFrame::Plain) | (id&TQFrame::MShape);
	    } else {
		sty = vp->frameStyle()&TQFrame::MShape;
		sty = (sty ? sty : TQFrame::Box) | (id&TQFrame::MShadow);
	    }
	    vp->setFrameStyle(sty);
	} else if (id & lw_id) {
	    vp->setLineWidth(id&~lw_id);
	} else if (id & mlw_id) {
	    vp->setMidLineWidth(id&~mlw_id);
	} else {
	    vp->setMargin(id&~mw_id);
	}

	vp->update();
	setFMenuItems();
    }

    void setFMenuItems()
    {
	int sty = vp->frameStyle();

	f_options->setItemChecked( style_id, !sty );

	for (int i=1; i <= f_laststyle; i++) {
	    int id = f_options->idAt(i);
	    if (id & TQFrame::MShape)
		f_options->setItemChecked( id,
		    ((id&TQFrame::MShape) == (sty&TQFrame::MShape)) );
	    else
		f_options->setItemChecked( id,
		    ((id&TQFrame::MShadow) == (sty&TQFrame::MShadow)) );
	}

	for (int lw=1; lw<=max_lw; lw++)
	    lw_options->setItemChecked( lw_id|lw, vp->lineWidth() == lw );

	for (int mlw=0; mlw<=max_mlw; mlw++)
	    mlw_options->setItemChecked( mlw_id|mlw, vp->midLineWidth() == mlw );

	for (int mw=0; mw<=max_mw; mw++)
	    mw_options->setItemChecked( mw_id|mw, vp->margin() == mw );
    }

private:
    TQScrollView* vp;
    TQPopupMenu* vp_options;
    TQPopupMenu* f_options;
    TQPopupMenu* lw_options;
    TQPopupMenu* mlw_options;
    TQPopupMenu* mw_options;
    TQSizeGrip* corner;

    int vauto_id, vaoff_id, vaon_id,
	hauto_id, haoff_id, haon_id,
	corn_id;

    int f_laststyle;
};

int main( int argc, char **argv )
{
    TQApplication a( argc, argv );

    ScrollViewExample ve1(1,0,"ve1");
    ScrollViewExample ve2(2,0,"ve2");
    ScrollViewExample ve3(3,0,"ve3");
    ve1.setCaption("TQt Example - Scrollviews");
    ve1.show();
    ve2.setCaption("TQt Example - Scrollviews");
    ve2.show();
    ve3.setCaption("TQt Example - Scrollviews");
    ve3.show();

    TQObject::connect(tqApp, TQ_SIGNAL(lastWindowClosed()), tqApp, TQ_SLOT(quit()));

    return a.exec();
}

#include "scrollview.moc"
