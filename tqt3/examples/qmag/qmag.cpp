/****************************************************************************
**
** Copyright (C) 1992-2008 Trolltech ASA.  All rights reserved.
**
** This file is part of an example program for TQt.  This example
** program may be used, distributed and modified without limitation.
**
*****************************************************************************/

#include <ntqcombobox.h>
#include <ntqpushbutton.h>
#include <ntqpixmap.h>
#include <ntqimage.h>
#include <ntqlabel.h>
#include <ntqfiledialog.h>
#include <ntqregexp.h>

#include <ntqapplication.h>
#include <ntqpainter.h>
#include <ntqwmatrix.h>


class MagWidget : public TQWidget
{
    TQ_OBJECT
public:
    MagWidget( TQWidget *parent=0, const char *name=0 );

public slots:
    void	setZoom( int );
    void	setRefresh( int );
    void	save();
    void	multiSave();

protected:
    void	paintEvent( TQPaintEvent * );
    void	mousePressEvent( TQMouseEvent * );
    void	mouseReleaseEvent( TQMouseEvent * );
    void	mouseMoveEvent( TQMouseEvent * );
    void	focusOutEvent( TQFocusEvent * );
    void	timerEvent( TQTimerEvent * );
    void	resizeEvent( TQResizeEvent * );

private:
    void	grabAround(TQPoint pos);
    void	grab();

    TQComboBox   *zoom;
    TQComboBox   *refresh;
    TQPushButton *saveButton;
    TQPushButton *multiSaveButton;
    TQPushButton *quitButton;
    TQPixmap	pm;		// pixmap, magnified
    TQPixmap	p;		// pixmap
    TQImage	image;		// image of pixmap (for RGB)
    TQLabel      *rgb;
    int		yoffset;	// pixels in addition to the actual picture
    int		z;		// magnification factor
    int		r;		// autorefresh rate (index into refreshrates)
    bool	grabbing;	// true if qmag is currently grabbing
    int		grabx, graby;
    TQString	multifn;	// filename for multisave
};


#ifdef COMPLEX_GUI
static const char *zoomfactors[] = {
    "100%", "200%", "300%", "400%", "500%",
    "600%", "700%", "800%", "1600%", 0 };

static const char *refreshrates[] = {
    "No autorefresh", "50 per second", "4 per second", "3 per second", "2 per second",
    "Every second", "Every two seconds", "Every three seconds",
    "Every five seconds", "Every ten seconds", 0 };
#endif

static const int timer[] = {
    0, 20, 250, 333, 500, 1000, 2000, 3000, 5000, 10000 };


MagWidget::MagWidget( TQWidget *parent, const char *name )
    : TQWidget( parent, name)
{
    z = 1;			// default zoom (100%)
    r = 0;			// default refresh (none)

#ifdef COMPLEX_GUI
    int w=0, x=0, n;

    zoom = new TQComboBox( false, this );
    TQ_CHECK_PTR(zoom);
    zoom->insertStrList( zoomfactors, 9 );
    connect( zoom, TQ_SIGNAL(activated(int)), TQ_SLOT(setZoom(int)) );

    refresh = new TQComboBox( false, this );
    TQ_CHECK_PTR(refresh);
    refresh->insertStrList( refreshrates, 9 );
    connect( refresh, TQ_SIGNAL(activated(int)), TQ_SLOT(setRefresh(int)) );

    for( n=0; n<9; n++) {
	int w2 = zoom->fontMetrics().width( zoomfactors[n] );
	w = TQMAX(w2, w);
    }
    zoom->setGeometry( 2, 2, w+30, 20 );

    x = w+34;
    w = 0;
    for( n=0; n<9; n++) {
	int w2 = refresh->fontMetrics().width( refreshrates[n] );
	w = TQMAX(w2, w);
    }
    refresh->setGeometry( x, 2, w+30, 20 );

    saveButton = new TQPushButton( this );
    TQ_CHECK_PTR(saveButton);
    connect( saveButton, TQ_SIGNAL(clicked()), this, TQ_SLOT(save()) );
    saveButton->setText( "Save" );
    saveButton->setGeometry( x+w+30+2, 2,
			     10+saveButton->fontMetrics().width("Save"), 20 );

    multiSaveButton = new TQPushButton( this );
    multiSaveButton->setToggleButton(true);
    TQ_CHECK_PTR(multiSaveButton);
    connect( multiSaveButton, TQ_SIGNAL(clicked()), this, TQ_SLOT(multiSave()) );
    multiSaveButton->setText( "MultiSave" );
    multiSaveButton->setGeometry( saveButton->geometry().right() + 2, 2,
			     10+multiSaveButton->fontMetrics().width("MultiSave"), 20 );

    quitButton = new TQPushButton( this );
    TQ_CHECK_PTR(quitButton);
    connect( quitButton, TQ_SIGNAL(clicked()), tqApp, TQ_SLOT(quit()) );
    quitButton->setText( "Quit" );
    quitButton->setGeometry( multiSaveButton->geometry().right() + 2, 2,
			     10+quitButton->fontMetrics().width("Quit"), 20 );
#else
    zoom = 0;
    multiSaveButton = 0;
#endif

    setRefresh(1);
    setZoom(5);

    rgb = new TQLabel( this );
    TQ_CHECK_PTR( rgb );
    rgb->setText( "" );
    rgb->setAlignment( AlignVCenter );
    rgb->resize( width(), rgb->fontMetrics().height() + 4 );

#ifdef COMPLEX_GUI
    yoffset = zoom->height()	// top buttons
	+ 4			// space around top buttons
	+ rgb->height();	// color-value text height
    setMinimumSize( quitButton->pos().x(), yoffset+20 );
    resize( quitButton->geometry().topRight().x() + 2, yoffset+60 );
#else
    yoffset = 0;
    resize(350,350);
#endif

    grabx = graby = -1;
    grabbing = false;

    setMouseTracking( true );	// and do let me know what pixel I'm at, eh?

    grabAround( TQPoint(grabx=tqApp->desktop()->width()/2, graby=tqApp->desktop()->height()/2) );
}


void MagWidget::setZoom( int index )
{
    if (index == 8)
	z = 16;
    else
	z = index+1;
    grab();
}


void MagWidget::setRefresh( int index )
{
    r = index;
    killTimers();
    if (index && !grabbing)
	startTimer( timer[r] );
}


void MagWidget::save()
{
    if ( !p.isNull() ) {
	killTimers();
	TQString fn = TQFileDialog::getSaveFileName();
	if ( !fn.isEmpty() )
	    p.save( fn, "BMP" );
	if ( r )
	    startTimer( timer[r] );
    }
}

void MagWidget::multiSave()
{
    if ( !p.isNull() ) {
	multifn = ""; // stops saving
	multifn = TQFileDialog::getSaveFileName();
	if ( multifn.isEmpty() )
	    multiSaveButton->setOn(false);
	if ( !r )
	    p.save( multifn, "BMP" );
    } else {
	multiSaveButton->setOn(false);
    }
}


void MagWidget::grab()
{
    if ( !isVisible() ) 
	return;			// don't eat resources when iconified

    if ( grabx < 0 || graby < 0 )
	return;			// don't grab until the user has said to

    int x,y, w,h;

    w = (width()+z-1)/z;
    h = (height()+z-1-yoffset)/z;
    if ( w<1 || h<1 )
	return;			// don't ask too much from the window system :)

    x = grabx-w/2;		// find a suitable position to grab from
    y = graby-h/2;
    if ( x + w > TQApplication::desktop()->width() )
	x = TQApplication::desktop()->width()-w;
    else if ( x < 0 )
	x = 0;
    if ( y + h > TQApplication::desktop()->height() ) 
	y = TQApplication::desktop()->height()-h;
    else if ( y < 0 )
	y = 0;

    p = TQPixmap::grabWindow( TQApplication::desktop()->winId(),  x, y, w, h );
    image = p.convertToImage();
    TQWMatrix m;			// after getting it, scale it
    m.scale( (double)z, (double)z );
    pm = p.xForm( m );

    if ( !multiSaveButton || !multiSaveButton->isOn() )
	repaint( false );		// and finally repaint, flicker-free
}


void MagWidget::paintEvent( TQPaintEvent * )
{
    if ( !pm.isNull() ) {
	TQPainter paint( this );
	paint.drawPixmap( 0, zoom ? zoom->height()+4 : 0, pm, 
			      0,0, width(), height()-yoffset );
    }
}


void MagWidget::mousePressEvent( TQMouseEvent *e )
{
    if ( !grabbing ) {		// prepare to grab...
	grabbing = true;
	killTimers();
	grabMouse( crossCursor );
	grabx = -1;
	graby = -1;
    } else {			// REALLY prepare to grab
	grabx = mapToGlobal(e->pos()).x();
	graby = mapToGlobal(e->pos()).y();
    }
}



void MagWidget::mouseReleaseEvent( TQMouseEvent * e )
{
    if ( grabbing && grabx >= 0 && graby >= 0 ) {
	grabbing = false;
	grabAround(e->pos());
	releaseMouse();
    }
}

void MagWidget::grabAround(TQPoint pos)
{
    int rx, ry;
    rx = mapToGlobal(pos).x();
    ry = mapToGlobal(pos).y();
    int w = TQABS(rx-grabx);
    int h = TQABS(ry-graby);
    if ( w > 10 && h > 10 ) {
	int pz;
	pz = 1;
	while ( w*pz*h*pz < width()*(height()-yoffset) &&
		w*pz < TQApplication::desktop()->width() &&
		h*pz < TQApplication::desktop()->height() )
	    pz++;
	if ( (w*pz*h*pz - width()*(height()-yoffset)) > 
	     (width()*(height()-yoffset) - w*(pz-1)*h*(pz-1)) )
	    pz--;
	if ( pz < 1 )
	    pz = 1;
	if ( pz > 8 )
	    pz = 8;
	if ( zoom )
	    zoom->setCurrentItem( pz-1 );

	z = pz;
	grabx = TQMIN(rx, grabx) + w/2;
	graby = TQMIN(ry, graby) + h/2;
	resize( w*z, h*z+yoffset );
    }
    grab();
    if ( r )
	startTimer( timer[r] );
}


void MagWidget::mouseMoveEvent( TQMouseEvent *e )
{
    if ( grabbing || pm.isNull() ||
	 e->pos().y() > height() - (zoom ? zoom->fontMetrics().height() - 4 : 0) ||
	 e->pos().y() < (zoom ? zoom->height()+4 : 4) ) {
	rgb->setText( "" );
    } else {
	int x,y;
	x = e->pos().x() / z;
	y = (e->pos().y() - ( zoom ? zoom->height() : 0 ) - 4) / z;
	TQString pixelinfo;
	if ( image.valid(x,y) )
	{
	    TQRgb px = image.pixel(x,y);
	    pixelinfo.sprintf(" %3d,%3d,%3d  #%02x%02x%02x",
		tqRed(px), tqGreen(px), tqBlue(px),
		tqRed(px), tqGreen(px), tqBlue(px));
	}
	TQString label;
	label.sprintf( "x=%d, y=%d %s", 
	    x+grabx, y+graby, (const char*)pixelinfo );
	rgb->setText( label );
    }
}
	

void MagWidget::focusOutEvent( TQFocusEvent * )
{
    rgb->setText( "" );
}


void MagWidget::timerEvent( TQTimerEvent * )
{
    grab();
/*
    if ( multiSaveButton->isOn() && !multifn.isEmpty() ) {
	TQRegExp num("[0-9][0-9]*");
	int start;
	int len;
	if ((start=num.match(multifn,0,&len))>=0)
	    multifn.replace(num,
		TQString().setNum(multifn.mid(start,len).toInt()+1)
	    );
	p.save( multifn, "BMP" );
    }
*/
}


void MagWidget::resizeEvent( TQResizeEvent * )
{
    rgb->setGeometry( 0, height() - rgb->height(), width(), rgb->height() );
    grab();
}


#include "qmag.moc"


int main( int argc, char **argv )
{
    TQApplication a( argc, argv );
    MagWidget m;
    a.setMainWidget( &m );
    m.show();
    return a.exec();
}
