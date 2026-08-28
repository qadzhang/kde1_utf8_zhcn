/****************************************************************************
**
** Copyright (C) 1992-2008 Trolltech ASA.  All rights reserved.
**
** This file is part of an example program for TQt.  This example
** program may be used, distributed and modified without limitation.
**
*****************************************************************************/

#include <ntqapplication.h>

#include <ntqdialog.h>
#include <ntqlabel.h>
#include <ntqlineedit.h>
#include <ntqpushbutton.h>
#include <ntqcheckbox.h>
#include <ntqradiobutton.h>
#include <ntqbuttongroup.h>
#include <ntqlcdnumber.h>
#include <ntqslider.h>
#include <ntqmenubar.h>
#include <ntqfontdialog.h>
#include <ntqlayout.h>
#include <ntqvbox.h>
#include <ntqwidgetstack.h>

#include <ntqpainter.h>
#include <ntqpixmap.h>
#include <ntqpicture.h>

#include <stdlib.h>


class ModeNames {
public:
    enum Mode { Text, Image, Picture };
};


class XFormControl : public TQVBox, public ModeNames
{
    TQ_OBJECT
public:
    XFormControl( const TQFont &initialFont, TQWidget *parent=0, const char *name=0 );
   ~XFormControl() {}

    TQWMatrix matrix();

signals:
    void newMatrix( TQWMatrix );
    void newText( const TQString& );
    void newFont( const TQFont & );
    void newMode( int );
private slots:
    void newMtx();
    void newTxt(const TQString&);
    void selectFont();
    void fontSelected( const TQFont & );
    void changeMode(int);
    void timerEvent(TQTimerEvent*);
private:
    Mode mode;
    TQSlider	 *rotS;		       // Rotation angle scroll bar
    TQSlider	 *shearS;	       // Shear value scroll bar
    TQSlider	 *magS;		       // Magnification value scroll bar
    TQLCDNumber	 *rotLCD;	       // Rotation angle LCD display
    TQLCDNumber	 *shearLCD;	       // Shear value LCD display
    TQLCDNumber	 *magLCD;	       // Magnification value LCD display
    TQCheckBox	 *mirror;	       // Checkbox for mirror image on/of
    TQWidgetStack* optionals;
    TQLineEdit	 *textEd;	       // Inp[ut field for xForm text
    TQPushButton  *fpb;		       // Select font push button
    TQRadioButton *rb_txt;	       // Radio button for text
    TQRadioButton *rb_img;	       // Radio button for image
    TQRadioButton *rb_pic;	       // Radio button for picture
    TQFont currentFont;
};

/*
  ShowXForm displays a text or a pixmap (TQPixmap) using a coordinate
  transformation matrix (TQWMatrix)
*/

class ShowXForm : public TQWidget, public ModeNames
{
    TQ_OBJECT
public:
    ShowXForm( const TQFont &f, TQWidget *parent=0, const char *name=0 );
   ~ShowXForm() {}
    void showIt();			// (Re)displays text or pixmap

    Mode mode() const { return m; }
public slots:
    void setText( const TQString& );
    void setMatrix( TQWMatrix );
    void setFont( const TQFont &f );
    void setPixmap( TQPixmap );
    void setPicture( const TQPicture& );
    void setMode( int );
private:
    TQSizePolicy sizePolicy() const;
    TQSize sizeHint() const;
    void paintEvent( TQPaintEvent * );
    void resizeEvent( TQResizeEvent * );
    TQWMatrix  mtx;			// coordinate transform matrix
    TQString   text;			// text to be displayed
    TQPixmap   pix;			// pixmap to be displayed
    TQPicture  picture;			// text to be displayed
    TQRect     eraseRect;		// covers last displayed text/pixmap
    Mode      m;
};

XFormControl::XFormControl( const TQFont &initialFont,
			    TQWidget *parent, const char *name )
	: TQVBox( parent, name )
{
    setSpacing(6);
    setMargin(6);
    currentFont = initialFont;
    mode = Image;

    rotLCD	= new TQLCDNumber( 4, this, "rotateLCD" );
    rotS	= new TQSlider( TQSlider::Horizontal, this,
				  "rotateSlider" );
    shearLCD	= new TQLCDNumber( 5,this, "shearLCD" );
    shearS	= new TQSlider( TQSlider::Horizontal, this,
				  "shearSlider" );
    mirror	= new TQCheckBox( this, "mirrorCheckBox" );
    rb_txt = new TQRadioButton( this, "text" );
    rb_img = new TQRadioButton( this, "image" );
    rb_pic = new TQRadioButton( this, "picture" );
    optionals = new TQWidgetStack(this);
    TQVBox* optionals_text = new TQVBox(optionals);
    optionals_text->setSpacing(6);
    TQVBox* optionals_other = new TQVBox(optionals);
    optionals_other->setSpacing(6);
    optionals->addWidget(optionals_text,0);
    optionals->addWidget(optionals_other,1);
    fpb		= new TQPushButton( optionals_text, "text" );
    textEd	= new TQLineEdit( optionals_text, "text" );
    textEd->setFocus();

    rotLCD->display( "  0'" );

    rotS->setRange( -180, 180 );
    rotS->setValue( 0 );
    connect( rotS, TQ_SIGNAL(valueChanged(int)), TQ_SLOT(newMtx()) );

    shearLCD->display( "0.00" );

    shearS->setRange( -25, 25 );
    shearS->setValue( 0 );
    connect( shearS, TQ_SIGNAL(valueChanged(int)), TQ_SLOT(newMtx()) );

    mirror->setText( tr("Mirror") );
    connect( mirror, TQ_SIGNAL(clicked()), TQ_SLOT(newMtx()) );

    TQButtonGroup *bg = new TQButtonGroup(this);
    bg->hide();
    bg->insert(rb_txt,0);
    bg->insert(rb_img,1);
    bg->insert(rb_pic,2);
    rb_txt->setText( tr("Text") );
    rb_img->setText( tr("Image") );
    rb_img->setChecked(true);
    rb_pic->setText( tr("Picture") );
    connect( bg, TQ_SIGNAL(clicked(int)), TQ_SLOT(changeMode(int)) );

    fpb->setText( tr("Select font...") );
    connect( fpb, TQ_SIGNAL(clicked()), TQ_SLOT(selectFont()) );

    textEd->setText( "Troll" );
    connect( textEd, TQ_SIGNAL(textChanged(const TQString&)),
		     TQ_SLOT(newTxt(const TQString&)) );

    magLCD = new TQLCDNumber( 4,optionals_other, "magLCD" );
    magLCD->display( "100" );
    magS = new TQSlider( TQSlider::Horizontal, optionals_other,
			   "magnifySlider" );
    magS->setRange( 0, 800 );
    connect( magS, TQ_SIGNAL(valueChanged(int)), TQ_SLOT(newMtx()) );
    magS->setValue( 0 );
    connect( magS, TQ_SIGNAL(valueChanged(int)), magLCD, TQ_SLOT(display(int)));

    optionals_text->adjustSize();
    optionals_other->adjustSize();
    changeMode(Image);

    startTimer(20); // start an initial animation
}

void XFormControl::timerEvent(TQTimerEvent*)
{
    int v = magS->value();
    v = (v+2)+v/10;
    if ( v >= 200 ) {
	v = 200;
	killTimers();
    }
    magS->setValue(v);
}



/*
    Called whenever the user has changed one of the matrix parameters
    (i.e. rotate, shear or magnification)
*/
void XFormControl::newMtx()
{
    emit newMatrix( matrix() );
}

void XFormControl::newTxt(const TQString& s)
{
    emit newText(s);
    changeMode(Text);
}

/*
    Calculates the matrix appropriate for the current controls,
    and updates the displays.
*/
TQWMatrix XFormControl::matrix()
{
    TQWMatrix m;
    if (mode != Text) {
	double magVal = 1.0*magS->value()/100;
	m.scale( magVal, magVal );
    }
    double shearVal = 1.0*shearS->value()/25;
    m.shear( shearVal, shearVal );
    m.rotate( rotS->value() );
    if ( mirror->isChecked() ) {
	m.scale( 1, -1 );
	m.rotate( 180 );
    }

    TQString tmp;
    tmp.sprintf( "%1.2f", shearVal  );
    if ( shearVal >= 0 )
	tmp.insert( 0, " " );
    shearLCD->display( tmp );

    int rot = rotS->value();
    if ( rot < 0 )
	rot = rot + 360;
    tmp.sprintf( "%3i'", rot );
    rotLCD->display( tmp );
    return m;
}


void XFormControl::selectFont()
{
    bool ok;
    TQFont f = TQFontDialog::getFont( &ok, currentFont );
    if ( ok ) {
	currentFont = f;
	fontSelected( f );
    }
}

void XFormControl::fontSelected( const TQFont &font )
{
    emit newFont( font );
    changeMode(Text);
}

/*
    Sets the mode - Text, Image, or Picture.
*/

void XFormControl::changeMode(int m)
{
    mode = (Mode)m;

    emit newMode( m );
    newMtx();
    if ( mode == Text ) {
	optionals->raiseWidget(0);
	rb_txt->setChecked(true);
    } else {
	optionals->raiseWidget(1);
	if ( mode == Image )
	    rb_img->setChecked(true);
	else
	    rb_pic->setChecked(true);
    }
    tqApp->flushX();
}

ShowXForm::ShowXForm( const TQFont &initialFont,
		      TQWidget *parent, const char *name )
	: TQWidget( parent, name, WResizeNoErase )
{
    setFont( initialFont );
    setBackgroundColor( white );
    m = Text;
    eraseRect = TQRect( 0, 0, 0, 0 );
}

TQSizePolicy ShowXForm::sizePolicy() const
{
    return TQSizePolicy( TQSizePolicy::Expanding, TQSizePolicy::Expanding );
}

TQSize ShowXForm::sizeHint() const
{
    return TQSize(400,400);
}

void ShowXForm::paintEvent( TQPaintEvent * )
{
    showIt();
}

void ShowXForm::resizeEvent( TQResizeEvent * )
{
    eraseRect = TQRect( width()/2, height()/2, 0, 0 );
    repaint(rect());
}

void ShowXForm::setText( const TQString& s )
{
    text = s;
    showIt();
}

void ShowXForm::setMatrix( TQWMatrix w )
{
    mtx = w;
    showIt();
}

void ShowXForm::setFont( const TQFont &f )
{
    m = Text;
    TQWidget::setFont( f );
}

void ShowXForm::setPixmap( TQPixmap pm )
{
    pix	 = pm;
    m    = Image;
    showIt();
}

void ShowXForm::setPicture( const TQPicture& p )
{
    picture = p;
    m = Picture;
    showIt();
}

void ShowXForm::setMode( int mode )
{
    m = (Mode)mode;
}

void ShowXForm::showIt()
{
    TQPainter p;
    TQRect r;	  // rectangle covering new text/pixmap in virtual coordinates
    TQWMatrix um;  // copy user specified transform
    int textYPos = 0; // distance from boundingRect y pos to baseline
    int textXPos = 0; // distance from boundingRect x pos to text start
    TQRect br;
    TQFontMetrics fm( fontMetrics() );	// get widget font metrics
    switch ( mode() ) {
      case Text:
	br = fm.boundingRect( text );	// rectangle covering text
	r  = br;
	textYPos = -r.y();
	textXPos = -r.x();
	br.moveTopLeft( TQPoint( -br.width()/2, -br.height()/2 ) );
        break;
      case Image:
	r = TQRect(0, 0, pix.width()+1, pix.height()+1);
        break;
      case Picture:
	// ### need TQPicture::boundingRect()
	r = TQRect(0,0,1000,1000);
        break;
    }
    r.moveTopLeft( TQPoint(-r.width()/2, -r.height()/2) );
    r.moveBy( -1, -1 ); // add border for matrix round off
    r.setSize( TQSize( r.width() + 2,r.height() + 2 ) );
	  // compute union of new and old rect
	  // the resulting rectangle will cover what is already displayed
	  // and have room for the new text/pixmap
    eraseRect = eraseRect.unite( mtx.mapRect(r) );
    int pw = TQMIN(eraseRect.width(),width());
    int ph = TQMIN(eraseRect.height(),height());
    TQPixmap pm( pw, ph );		// off-screen drawing pixmap
    pm.fill( backgroundColor() );

    p.begin( &pm );
    um.translate( pw/2, ph/2 );	// 0,0 is center
    um = mtx * um;
    p.setWorldMatrix( um );
    switch ( mode() ) {
      case Text:
	p.setFont( font() );		// use widget font
	p.drawText( r.left() + textXPos, r.top() + textYPos, text );
#if 0
	p.setPen( red );
	p.drawRect( br );
#endif
	break;
    case Image:
	p.drawPixmap( -pix.width()/2, -pix.height()/2, pix );
	break;
      case Picture:
	// ### need TQPicture::boundingRect()
	p.scale(0.25,0.25);
	p.translate(-230,-180);
	p.drawPicture( picture );
    }
    p.end();

    int xpos = width()/2  - pw/2;
    int ypos = height()/2 - ph/2;
    bitBlt( this, xpos, ypos,			// copy pixmap to widget
	    &pm, 0, 0, -1, -1 );
    eraseRect =	 mtx.map( r );
}


/*
    Grand unifying widget, putting ShowXForm and XFormControl
    together.
*/

class XFormCenter : public TQHBox, public ModeNames
{
    TQ_OBJECT
public:
    XFormCenter( TQWidget *parent=0, const char *name=0 );
public slots:
    void setFont( const TQFont &f ) { sx->setFont( f ); }
    void newMode( int );
private:
    ShowXForm	*sx;
    XFormControl *xc;
};

void XFormCenter::newMode( int m )
{
    static bool first_i = true;
    static bool first_p = true;

    if ( sx->mode() == m )
	return;
    if ( m == Image && first_i ) {
	first_i = false;
	TQPixmap pm;
	if ( pm.load( "image.any" ) )
	    sx->setPixmap( pm );
	return;
    }
    if ( m == Picture && first_p ) {
	first_p = false;
	TQPicture p;
	if (p.load( "picture.any" ))
	    sx->setPicture( p );
	return;
    }
    sx->setMode(m);
}

XFormCenter::XFormCenter( TQWidget *parent, const char *name )
    : TQHBox( parent, name )
{
    TQFont f( "Charter", 36, TQFont::Bold );

    xc = new XFormControl( f, this );
    sx = new ShowXForm( f, this );
    setStretchFactor(sx,1);
    xc->setFrameStyle( TQFrame::Panel | TQFrame::Raised );
    xc->setLineWidth( 2 );
    connect( xc, TQ_SIGNAL(newText(const TQString&)), sx,
		 TQ_SLOT(setText(const TQString&)) );
    connect( xc, TQ_SIGNAL(newMatrix(TQWMatrix)),
	     sx, TQ_SLOT(setMatrix(TQWMatrix)) );
    connect( xc, TQ_SIGNAL(newFont(const TQFont&)), sx,
		 TQ_SLOT(setFont(const TQFont&)) );
    connect( xc, TQ_SIGNAL(newMode(int)), TQ_SLOT(newMode(int)) );
    sx->setText( "Troll" );
    newMode( Image );
    sx->setMatrix(xc->matrix());
}


int main( int argc, char **argv )
{
    TQApplication a( argc, argv );

    XFormCenter *xfc = new XFormCenter;

    a.setMainWidget( xfc );
    xfc->setCaption("TQt Example - XForm");
    xfc->show();
    return a.exec();
}

#include "xform.moc"		      // include metadata generated by the moc
