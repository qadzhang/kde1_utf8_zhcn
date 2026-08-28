/****************************************************************************
**
** Implementation of TQEffects functions
**
** Created : 000621
**
** Copyright (C) 2005-2008 Trolltech ASA.  All rights reserved.
**
** This file is part of the widgets module of the TQt GUI Toolkit.
**
** This file may be used under the terms of the GNU General
** Public License versions 2.0 or 3.0 as published by the Free
** Software Foundation and appearing in the files LICENSE.GPL2
** and LICENSE.GPL3 included in the packaging of this file.
** Alternatively you may (at your option) use any later version
** of the GNU General Public License if such license has been
** publicly approved by Trolltech ASA (or its successors, if any)
** and the KDE Free TQt Foundation.
**
** Please review the following information to ensure GNU General
** Public Licensing requirements will be met:
** http://trolltech.com/products/qt/licenses/licensing/opensource/.
** If you are unsure which license is appropriate for your use, please
** review the following information:
** http://trolltech.com/products/qt/licenses/licensing/licensingoverview
** or contact the sales department at sales@trolltech.com.
**
** This file may be used under the terms of the Q Public License as
** defined by Trolltech ASA and appearing in the file LICENSE.TQPL
** included in the packaging of this file.  Licensees holding valid TQt
** Commercial licenses may use this file in accordance with the TQt
** Commercial License Agreement provided with the Software.
**
** This file is provided "AS IS" with NO WARRANTY OF ANY KIND,
** INCLUDING THE WARRANTIES OF DESIGN, MERCHANTABILITY AND FITNESS FOR
** A PARTICULAR PURPOSE. Trolltech reserves all rights not granted
** herein.
**
**********************************************************************/

#include "ntqapplication.h"
#ifndef TQT_NO_EFFECTS
#include "ntqwidget.h"
#include "qeffects_p.h"
#include "ntqpixmap.h"
#include "ntqimage.h"
#include "ntqtimer.h"
#include "ntqdatetime.h"
#include "ntqguardedptr.h"
#include "ntqscrollview.h"

/*
  Internal class to get access to protected TQWidget-members
*/

class TQAccessWidget : public TQWidget
{
    friend class TQAlphaWidget;
    friend class TQRollEffect;
public:
    TQAccessWidget( TQWidget* parent=0, const char* name=0, WFlags f = 0 )
	: TQWidget( parent, name, f ) {}
};

/*
  Internal class TQAlphaWidget.

  The TQAlphaWidget is shown while the animation lasts
  and displays the pixmap resulting from the alpha blending.
*/

class TQAlphaWidget: public TQWidget, private TQEffects
{
    TQ_OBJECT
public:
    TQAlphaWidget( TQWidget* w, WFlags f = 0 );

    void run( int time );

protected:
    void paintEvent( TQPaintEvent* e );
    void closeEvent( TQCloseEvent* );
    bool eventFilter( TQObject* o, TQEvent* e );
    void alphaBlend();

protected slots:
    void render();

private:
    TQPixmap pm;
    double alpha;
    TQImage back;
    TQImage front;
    TQImage mixed;
    TQGuardedPtr<TQAccessWidget> widget;
    int duration;
    int elapsed;
    bool showWidget;
    TQTimer anim;
    TQTime checkTime;
};

static TQAlphaWidget* q_blend = 0;

/*
  Constructs a TQAlphaWidget.
*/
TQAlphaWidget::TQAlphaWidget( TQWidget* w, WFlags f )
    : TQWidget( TQApplication::desktop()->screen(TQApplication::desktop()->screenNumber(w)),
               "qt internal alpha effect widget", f )
{
#if 1 //ndef TQ_WS_WIN
    setEnabled( false );
#endif

    pm.setOptimization( TQPixmap::BestOptim );
    setBackgroundMode( NoBackground );
    widget = (TQAccessWidget*)w;
    alpha = 0;
}

/*
  \reimp
*/
void TQAlphaWidget::paintEvent( TQPaintEvent* )
{
    bitBlt( this, TQPoint(0,0), &pm );
}

/*
  Starts the alphablending animation.
  The animation will take about \a time ms
*/
void TQAlphaWidget::run( int time )
{
    duration = time;

    if ( duration < 0 )
	duration = 150;

    if ( !widget )
	return;

    elapsed = 0;
    checkTime.start();

    showWidget = true;
    tqApp->installEventFilter( this );

    widget->setWState( WState_Visible );

    move( widget->geometry().x(),widget->geometry().y() );
    resize( widget->size().width(), widget->size().height() );

    front = TQImage( widget->size(), 32 );
    front = TQPixmap::grabWidget( widget );

    back = TQImage( widget->size(), 32 );
    back = TQPixmap::grabWindow( TQApplication::desktop()->winId(),
				widget->geometry().x(), widget->geometry().y(),
				widget->geometry().width(), widget->geometry().height() );

    if ( !back.isNull() && checkTime.elapsed() < duration / 2 ) {
        mixed = back.copy();
	pm = mixed;
	show();
        setEnabled(false);

	connect( &anim, TQ_SIGNAL(timeout()), this, TQ_SLOT(render()));
	anim.start( 1 );
    } else {
	duration = 0;
	render();
    }
}

/*
  \reimp
*/
bool TQAlphaWidget::eventFilter( TQObject* o, TQEvent* e )
{
    switch ( e->type() ) {
    case TQEvent::Move:
	if ( o != widget )
	    break;
	move( widget->geometry().x(),widget->geometry().y() );
	update();
	break;
    case TQEvent::Hide:
    case TQEvent::Close:
	if ( o != widget )
	    break;
    case TQEvent::MouseButtonPress:
#ifndef TQT_NO_SCROLLVIEW
	if ( ::tqt_cast<TQScrollView*>(o) )
	    break;
#endif
    case TQEvent::MouseButtonDblClick:
	setEnabled(true);
	showWidget = false;
	render();
	break;
    case TQEvent::KeyPress:
	{
	    TQKeyEvent *ke = (TQKeyEvent*)e;
	    if ( ke->key() == Key_Escape )
		showWidget = false;
	    else
		duration = 0;
	    render();
	    break;
	}
    default:
	break;
    }
    return TQWidget::eventFilter( o, e );
}

/*
  \reimp
*/
void TQAlphaWidget::closeEvent( TQCloseEvent *e )
{
    e->accept();
    if ( !q_blend )
	return;

    showWidget = false;
    render();

    TQWidget::closeEvent( e );
}

/*
  Render alphablending for the time elapsed.

  Show the blended widget and free all allocated source
  if the blending is finished.
*/
void TQAlphaWidget::render()
{
    int tempel = checkTime.elapsed();
    if ( elapsed >= tempel )
        elapsed++;
    else
        elapsed = tempel;

    if ( duration != 0 )
	alpha = tempel / double(duration);
    else
	alpha = 1;
    if ( alpha >= 1 || !showWidget) {
	anim.stop();
	tqApp->removeEventFilter( this );

	if ( widget ) {
	    if ( !showWidget ) {
#ifdef TQ_WS_WIN
                setEnabled(true);
                setFocus();
#endif
		widget->hide();
		widget->setWState( WState_ForceHide );
		widget->clearWState( WState_Visible );
	    } else if ( duration ) {
		BackgroundMode bgm = widget->backgroundMode();
		TQColor erc = widget->eraseColor();
		const TQPixmap *erp = widget->erasePixmap();

		widget->clearWState( WState_Visible );
		widget->setBackgroundMode( NoBackground );
		widget->show();
		if ( bgm != FixedColor && bgm != FixedPixmap ) {
		    widget->clearWState( WState_Visible ); // prevent update in setBackgroundMode
		    widget->setBackgroundMode( bgm );
		    widget->setWState( WState_Visible );
		}
		if ( erc.isValid() ) {
		    widget->setEraseColor( erc );
		} else if ( erp ) {
		    widget->setErasePixmap( *erp );
		}
	    } else {
		widget->clearWState( WState_Visible );
		widget->show();
	    }
	}
	q_blend = 0;
	deleteLater();
    } else {
	if (widget)
	    widget->clearWState( WState_ForceHide );
	alphaBlend();
	pm = mixed;
	repaint( false );
    }
}

/*
  Calculate an alphablended image.
*/
void TQAlphaWidget::alphaBlend()
{
    const double ia = 1-alpha;
    const int sw = front.width();
    const int sh = front.height();
    switch( front.depth() ) {
    case 32:
	{
	    TQ_UINT32** md = (TQ_UINT32**)mixed.jumpTable();
	    TQ_UINT32** bd = (TQ_UINT32**)back.jumpTable();
	    TQ_UINT32** fd = (TQ_UINT32**)front.jumpTable();

	    for (int sy = 0; sy < sh; sy++ ) {
		TQ_UINT32* bl = ((TQ_UINT32*)bd[sy]);
		TQ_UINT32* fl = ((TQ_UINT32*)fd[sy]);
		for (int sx = 0; sx < sw; sx++ ) {
		    TQ_UINT32 bp = bl[sx];
		    TQ_UINT32 fp = fl[sx];

		    ((TQ_UINT32*)(md[sy]))[sx] =  tqRgb(int (tqRed(bp)*ia + tqRed(fp)*alpha),
						    int (tqGreen(bp)*ia + tqGreen(fp)*alpha),
						    int (tqBlue(bp)*ia + tqBlue(fp)*alpha) );
		}
	    }
	}
    default:
	break;
    }
}

/*
  Internal class TQRollEffect

  The TQRollEffect widget is shown while the animation lasts
  and displays a scrolling pixmap.
*/

class TQRollEffect : public TQWidget, private TQEffects
{
    TQ_OBJECT
public:
    TQRollEffect( TQWidget* w, WFlags f, DirFlags orient );

    void run( int time );

protected:
    void paintEvent( TQPaintEvent* );
    bool eventFilter( TQObject*, TQEvent* );
    void closeEvent( TQCloseEvent* );

private slots:
    void scroll();

private:
    TQGuardedPtr<TQAccessWidget> widget;

    int currentHeight;
    int currentWidth;
    int totalHeight;
    int totalWidth;

    int duration;
    int elapsed;
    bool done;
    bool showWidget;
    int orientation;

    TQTimer anim;
    TQTime checkTime;

    TQPixmap pm;
};

static TQRollEffect* q_roll = 0;

/*
  Construct a TQRollEffect widget.
*/
TQRollEffect::TQRollEffect( TQWidget* w, WFlags f, DirFlags orient )
    : TQWidget( TQApplication::desktop()->screen(TQApplication::desktop()->screenNumber(w)),
               "qt internal roll effect widget", f ), orientation(orient)
{
#if 1 //ndef TQ_WS_WIN
    setEnabled( false );
#endif
    widget = (TQAccessWidget*) w;
    Q_ASSERT( widget );

    setBackgroundMode( NoBackground );

    if ( widget->testWState( WState_Resized ) ) {
	totalWidth = widget->width();
	totalHeight = widget->height();
    } else {
	totalWidth = widget->sizeHint().width();
	totalHeight = widget->sizeHint().height();
    }

    currentHeight = totalHeight;
    currentWidth = totalWidth;

    if ( orientation & (RightScroll|LeftScroll) )
	currentWidth = 0;
    if ( orientation & (DownScroll|UpScroll) )
	currentHeight = 0;

    pm.setOptimization( TQPixmap::BestOptim );
    pm = TQPixmap::grabWidget( widget );
}

/*
  \reimp
*/
void TQRollEffect::paintEvent( TQPaintEvent* )
{
    int x = orientation & RightScroll ? TQMIN(0, currentWidth - totalWidth) : 0;
    int y = orientation & DownScroll ? TQMIN(0, currentHeight - totalHeight) : 0;

    bitBlt( this, x, y, &pm,
		  0, 0, pm.width(), pm.height(), CopyROP, true );
}

/*
  \reimp
*/
bool TQRollEffect::eventFilter( TQObject* o, TQEvent* e )
{
    switch ( e->type() ) {
    case TQEvent::Move:
	if ( o != widget )
	    break;
	move( widget->geometry().x(),widget->geometry().y() );
	update();
	break;
    case TQEvent::Hide:
    case TQEvent::Close:
	if ( o != widget || done )
	    break;
	setEnabled(true);
	showWidget = false;
	done = true;
	scroll();
	break;
    case TQEvent::MouseButtonPress:
#ifndef TQT_NO_SCROLLVIEW
	if ( ::tqt_cast<TQScrollView*>(o) )
	    break;
#endif
    case TQEvent::MouseButtonDblClick:
	if ( done )
	    break;
	setEnabled(true);
	showWidget = false;
	done = true;
	scroll();
	break;
    case TQEvent::KeyPress:
	{
	    TQKeyEvent *ke = (TQKeyEvent*)e;
	    if ( ke->key() == Key_Escape )
		showWidget = false;
	    done = true;
	    scroll();
	    break;
	}
    default:
	break;
    }
    return TQWidget::eventFilter( o, e );
}

/*
  \reimp
*/
void TQRollEffect::closeEvent( TQCloseEvent *e )
{
    e->accept();
    if ( done )
	return;

    showWidget = false;
    done = true;
    scroll();

    TQWidget::closeEvent( e );
}

/*
  Start the animation.

  The animation will take about \a time ms, or is
  calculated if \a time is negative
*/
void TQRollEffect::run( int time )
{
    if ( !widget )
	return;

    duration  = time;
    elapsed = 0;

    if ( duration < 0 ) {
	int dist = 0;
	if ( orientation & (RightScroll|LeftScroll) )
	    dist += totalWidth - currentWidth;
	if ( orientation & (DownScroll|UpScroll) )
	    dist += totalHeight - currentHeight;
	duration = TQMIN( TQMAX( dist/3, 50 ), 120 );
    }

    connect( &anim, TQ_SIGNAL(timeout()), this, TQ_SLOT(scroll()));

    widget->setWState( WState_Visible );

    move( widget->geometry().x(),widget->geometry().y() );
    resize( TQMIN( currentWidth, totalWidth ), TQMIN( currentHeight, totalHeight ) );

    show();
    setEnabled(false);

    tqApp->installEventFilter( this );

    showWidget = true;
    done = false;
    anim.start( 1 );
    checkTime.start();
}

/*
  Roll according to the time elapsed.
*/
void TQRollEffect::scroll()
{
    if ( !done && widget) {
	widget->clearWState( WState_ForceHide );
	int tempel = checkTime.elapsed();
	if ( elapsed >= tempel )
	    elapsed++;
	else
	    elapsed = tempel;

	if ( currentWidth != totalWidth ) {
	    currentWidth = totalWidth * (elapsed/duration)
		+ ( 2 * totalWidth * (elapsed%duration) + duration )
		/ ( 2 * duration );
	    // equiv. to int( (totalWidth*elapsed) / duration + 0.5 )
	    done = (currentWidth >= totalWidth);
	}
	if ( currentHeight != totalHeight ) {
	    currentHeight = totalHeight * (elapsed/duration)
		+ ( 2 * totalHeight * (elapsed%duration) + duration )
		/ ( 2 * duration );
	    // equiv. to int( (totalHeight*elapsed) / duration + 0.5 )
	    done = (currentHeight >= totalHeight);
	}
	done = ( currentHeight >= totalHeight ) &&
	       ( currentWidth >= totalWidth );

        int w = totalWidth;
	int h = totalHeight;
	int x = widget->geometry().x();
	int y = widget->geometry().y();

	if ( orientation & RightScroll || orientation & LeftScroll )
	    w = TQMIN( currentWidth, totalWidth );
	if ( orientation & DownScroll || orientation & UpScroll )
	    h = TQMIN( currentHeight, totalHeight );

	setUpdatesEnabled( false );
	if ( orientation & UpScroll )
	    y = widget->geometry().y() + TQMAX( 0, totalHeight - currentHeight );
	if ( orientation & LeftScroll )
	    x = widget->geometry().x() + TQMAX( 0, totalWidth - currentWidth );
	if ( orientation & UpScroll || orientation & LeftScroll )
	    move( x, y );

	resize( w, h );
        setUpdatesEnabled( true );
	repaint( false );
    }
    if ( done ) {
	anim.stop();
	tqApp->removeEventFilter( this );
	if ( widget ) {
	    if ( !showWidget ) {
#ifdef TQ_WS_WIN
                setEnabled(true);
                setFocus();
#endif
		widget->hide();
		widget->setWState( WState_ForceHide );
		widget->clearWState( WState_Visible );
	    } else {
		BackgroundMode bgm = widget->backgroundMode();
		TQColor erc = widget->eraseColor();
		const TQPixmap *erp = widget->erasePixmap();

		widget->clearWState( WState_Visible );
		widget->setBackgroundMode( NoBackground );
		widget->show();
		if ( bgm != FixedColor && bgm != FixedPixmap ) {
		    widget->clearWState( WState_Visible ); // prevent update in setBackgroundMode
		    widget->setBackgroundMode( bgm );
		    widget->setWState( WState_Visible );
		}
		if ( erc.isValid() ) {
		    widget->setEraseColor( erc );
		} else if ( erp ) {
		    widget->setErasePixmap( *erp );
		}
	    }
	}
	q_roll = 0;
	deleteLater();
    }
}

/*
  Delete this after timeout
*/

#include "qeffects.moc"

/*!
    Scroll widget \a w in \a time ms. \a orient may be 1 (vertical), 2
    (horizontal) or 3 (diagonal).
*/
void qScrollEffect( TQWidget* w, TQEffects::DirFlags orient, int time )
{
    if ( q_roll ) {
	delete q_roll;
	q_roll = 0;
    }

    tqApp->sendPostedEvents( w, TQEvent::Move );
    tqApp->sendPostedEvents( w, TQEvent::Resize );
#ifdef TQ_WS_X11
    uint flags = TQt::WStyle_Customize | TQt::WNoAutoErase | TQt::WStyle_StaysOnTop
	| (w->isPopup() ? TQt::WType_Popup : (TQt::WX11BypassWM | TQt::WStyle_Tool));
#else
    uint flags = TQt::WStyle_Customize | TQt::WType_Popup | TQt::WX11BypassWM | TQt::WNoAutoErase | TQt::WStyle_StaysOnTop;
#endif

    // those can popups - they would steal the focus, but are disabled
    q_roll = new TQRollEffect( w, flags, orient );
    q_roll->run( time );
}

/*!
    Fade in widget \a w in \a time ms.
*/
void qFadeEffect( TQWidget* w, int time )
{
    if ( q_blend ) {
	delete q_blend;
	q_blend = 0;
    }

    tqApp->sendPostedEvents( w, TQEvent::Move );
    tqApp->sendPostedEvents( w, TQEvent::Resize );

#ifdef TQ_WS_X11
    uint flags = TQt::WStyle_Customize | TQt::WNoAutoErase | TQt::WStyle_StaysOnTop
	| (w->isPopup() ? TQt::WType_Popup : (TQt::WX11BypassWM | TQt::WStyle_Tool));
#else
    uint flags = TQt::WStyle_Customize | TQt::WType_Popup | TQt::WX11BypassWM | TQt::WNoAutoErase | TQt::WStyle_StaysOnTop;
#endif

    // those can popups - they would steal the focus, but are disabled
    q_blend = new TQAlphaWidget( w, flags );

    q_blend->run( time );
}
#endif //TQT_NO_EFFECTS
