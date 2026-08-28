/****************************************************************************
**
** Copyright (C) 1992-2008 Trolltech ASA.  All rights reserved.
**
** This file is part of an example program for TQt.  This example
** program may be used, distributed and modified without limitation.
**
*****************************************************************************/

#include <ntqprogressdialog.h>
#include <ntqapplication.h>
#include <ntqmenubar.h>
#include <ntqpopupmenu.h>
#include <ntqpainter.h>
#include <stdlib.h>

class AnimatedThingy : public TQLabel {
public:
    AnimatedThingy( TQWidget* parent, const TQString& s ) :
	TQLabel(parent),
	label(s),
	step(0)
    {
        setBackgroundColor(white);
	label+="\n... and wasting CPU\nwith this animation!\n";

	for (int i=0; i<nqix; i++)
	    ox[0][i] = oy[0][i] = ox[1][i] = oy[1][i] = 0;
	x0 = y0 = x1 = y1 = 0;
	dx0 = rand()%8+2;
	dy0 = rand()%8+2;
	dx1 = rand()%8+2;
	dy1 = rand()%8+2;
    }

    void show()
    {
	if (!isVisible()) startTimer(100);
	TQWidget::show();
    }

    void hide()
    {
	TQWidget::hide();
	killTimers();
    }

    TQSize sizeHint() const
    {
	return TQSize(120,100);
    }

protected:
    void timerEvent(TQTimerEvent*)
    {
	TQPainter p(this);
	TQPen pn=p.pen();
	pn.setWidth(2);
	pn.setColor(backgroundColor());
	p.setPen(pn);

	step = (step + 1) % nqix;

	p.drawLine(ox[0][step], oy[0][step], ox[1][step], oy[1][step]);

	inc(x0, dx0, width());
	inc(y0, dy0, height());
	inc(x1, dx1, width());
	inc(y1, dy1, height());
	ox[0][step] = x0;
	oy[0][step] = y0;
	ox[1][step] = x1;
	oy[1][step] = y1;

	TQColor c;
	c.setHsv( (step*255)/nqix, 255, 255 ); // rainbow effect
	pn.setColor(c);
	p.setPen(pn);
	p.drawLine(ox[0][step], oy[0][step], ox[1][step], oy[1][step]);
	p.setPen(colorGroup().text());
	p.drawText(rect(), AlignCenter, label);
    }

    void paintEvent(TQPaintEvent* event)
    {
	TQPainter p(this);
	TQPen pn=p.pen();
	pn.setWidth(2);
	p.setPen(pn);
	p.setClipRect(event->rect());
	for (int i=0; i<nqix; i++) {
	    TQColor c;
	    c.setHsv( (i*255)/nqix, 255, 255 ); // rainbow effect
	    pn.setColor(c);
	    p.setPen(pn);
	    p.drawLine(ox[0][i], oy[0][i], ox[1][i], oy[1][i]);
	}
	p.setPen(colorGroup().text());
	p.drawText(rect(), AlignCenter, label);
    }

private:
    void inc(int& x, int& dx, int b)
    {
	x+=dx;
	if (x<0) { x=0; dx=rand()%8+2; }
	else if (x>=b) { x=b-1; dx=-(rand()%8+2); }
    }

    enum {nqix=10};
    int ox[2][nqix];
    int oy[2][nqix];
    int x0,y0,x1,y1;
    int dx0,dy0,dx1,dy1;
    TQString label;
    int step;
};


class CPUWaster : public TQWidget
{
    TQ_OBJECT

    enum { first_draw_item = 1000, last_draw_item = 1006 };

    int drawItemRects(int id)
    {
	int n = id - first_draw_item;
	int r = 100;
	while (n--) r*=(n%3 ? 5 : 4);
	return r;
    }
    TQString drawItemText(int id)
    {
	TQString str;
	str.sprintf("%d Rectangles", drawItemRects(id));
	return str;
    }

public:
    CPUWaster() :
	pb(0)
    {
	menubar = new TQMenuBar( this, "menu" );
	TQ_CHECK_PTR( menubar );

	TQPopupMenu* file = new TQPopupMenu();
	TQ_CHECK_PTR( file );
	menubar->insertItem( "&File", file );
	for (int i=first_draw_item; i<=last_draw_item; i++)
	    file->insertItem( drawItemText(i), i );
	connect( menubar, TQ_SIGNAL(activated(int)), this, TQ_SLOT(doMenuItem(int)) );
	file->insertSeparator();
	file->insertItem( "Quit", tqApp,  TQ_SLOT(quit()) );

	options = new TQPopupMenu();
	TQ_CHECK_PTR( options );
	menubar->insertItem( "&Options", options );
	td_id = options->insertItem( "Timer driven", this, TQ_SLOT(timerDriven()) );
	ld_id = options->insertItem( "Loop driven", this, TQ_SLOT(loopDriven()) );
	options->insertSeparator();
	dl_id = options->insertItem( "Default label", this, TQ_SLOT(defaultLabel()) );
	cl_id = options->insertItem( "Custom label", this, TQ_SLOT(customLabel()) );
	options->insertSeparator();
	md_id = options->insertItem( "No minimum duration", this, TQ_SLOT(toggleMinimumDuration()) );
	options->setCheckable( true );
	loopDriven();
	defaultLabel();

	setFixedSize( 400, 300 );

	setBackgroundColor( black );
    }

public slots:
    void doMenuItem(int id)
    {
	if (id >= first_draw_item && id <= last_draw_item)
	    draw(drawItemRects(id));
    }

    void stopDrawing() { got_stop = true; }

    void timerDriven()
    {
	timer_driven = true;
	options->setItemChecked( td_id, true );
	options->setItemChecked( ld_id, false );
    }

    void loopDriven()
    {
	timer_driven = false;
	options->setItemChecked( ld_id, true );
	options->setItemChecked( td_id, false );
    }

    void defaultLabel()
    {
	default_label = true;
	options->setItemChecked( dl_id, true );
	options->setItemChecked( cl_id, false );
    }

    void customLabel()
    {
	default_label = false;
	options->setItemChecked( dl_id, false );
	options->setItemChecked( cl_id, true );
    }

    void toggleMinimumDuration()
    {
	options->setItemChecked( md_id,
	   !options->isItemChecked( md_id ) );
    }

private:
    void timerEvent( TQTimerEvent* )
    {
	if (!got_stop)
	    pb->setProgress( pb->totalSteps() - rects );
	rects--;

	{
	    TQPainter p(this);

	    int ww = width();
	    int wh = height();

	    if ( ww > 8 && wh > 8 ) {
		TQColor c(rand()%255, rand()%255, rand()%255);
		int x = rand() % (ww-8);
		int y = rand() % (wh-8);
		int w = rand() % (ww-x);
		int h = rand() % (wh-y);
		p.fillRect( x, y, w, h, c );
	    }
	}

	if (!rects || got_stop) {
	    if (!got_stop)
		pb->setProgress( pb->totalSteps() );
	    TQPainter p(this);
	    p.fillRect(0, 0, width(), height(), backgroundColor());
	    enableDrawingItems(true);
	    killTimers();
	    delete pb;
	    pb = 0;
	}
    }

    TQProgressDialog* newProgressDialog( const char* label, int steps, bool modal )
    {
	TQProgressDialog *d = new TQProgressDialog(label, "Cancel", steps, this,
						 "progress", modal);
        if ( options->isItemChecked( md_id ) )
	    d->setMinimumDuration(0);
	if ( !default_label )
	    d->setLabel( new AnimatedThingy(d,label) );
	return d;
    }

    void enableDrawingItems(bool yes)
    {
	for (int i=first_draw_item; i<=last_draw_item; i++) {
	    menubar->setItemEnabled(i, yes);
	}
    }

    void draw(int n)
    {
	if ( timer_driven ) {
	    if ( pb ) {
		tqWarning("This cannot happen!");
		return;
	    }
	    rects = n;
	    pb = newProgressDialog("Drawing rectangles.\n"
				   "Using timer event.", n, false);
	    pb->setCaption("Please Wait");
	    connect(pb, TQ_SIGNAL(cancelled()), this, TQ_SLOT(stopDrawing()));
	    enableDrawingItems(false);
	    startTimer(0);
	    got_stop = false;
	} else {
	    TQProgressDialog* lpb = newProgressDialog(
			"Drawing rectangles.\nUsing loop.", n, true);
	    lpb->setCaption("Please Wait");

	    TQPainter p(this);
	    for (int i=0; i<n; i++) {
		lpb->setProgress(i);
		if ( lpb->wasCancelled() )
		    break;

		TQColor c(rand()%255, rand()%255, rand()%255);
		int x = rand()%(width()-8);
		int y = rand()%(height()-8);
		int w = rand()%(width()-x);
		int h = rand()%(height()-y);
		p.fillRect(x,y,w,h,c);
	    }

	    p.fillRect(0, 0, width(), height(), backgroundColor());

	    delete lpb;
	}
    }

    TQMenuBar* menubar;
    TQProgressDialog* pb;
    TQPopupMenu* options;
    int td_id, ld_id;
    int dl_id, cl_id;
    int md_id;
    int rects;
    bool timer_driven;
    bool default_label;
    bool got_stop;
};

int main( int argc, char **argv )
{
    TQApplication a( argc, argv );

    int wincount = argc > 1 ? atoi(argv[1]) : 1;

    for ( int i=0; i<wincount; i++ ) {
	CPUWaster* cpuw = new CPUWaster;
	if ( i == 0 ) a.setMainWidget(cpuw);
	cpuw->show();
    }
    return a.exec();
}

#include "progress.moc"
