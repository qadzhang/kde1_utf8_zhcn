/**********************************************************************
** Copyright (C) 1992-2008 Trolltech ASA.  All rights reserved.
**
** This file is part of TQt/Embedded virtual framebuffer.
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
** Licensees holding valid TQt Commercial licenses may use this file in
** accordance with the TQt Commercial License Agreement provided with
** the Software.
**
** This file is provided "AS IS" with NO WARRANTY OF ANY KIND,
** INCLUDING THE WARRANTIES OF DESIGN, MERCHANTABILITY AND FITNESS FOR
** A PARTICULAR PURPOSE. Trolltech reserves all rights not granted
** herein.
**
**********************************************************************/

#include "ntqglobal.h"
#if !defined( TQ_WS_QWS ) || defined( TQT_NO_QWS_MULTIPROCESS )
#define TQLock TQWSSemaphore
#undef TQT_NO_QWS_MULTIPROCESS
#include "../../src/kernel/qlock.cpp"
#else
#include "qlock_p.h"
#endif

#include "tqvfbview.h"
#include "ntqvfbhdr.h"

#define TQTE_PIPE "TQtEmbedded-%1"

#include <ntqapplication.h>
#include <ntqimage.h>
#include <ntqbitmap.h>
#include <ntqtimer.h>
#include <ntqwmatrix.h>
#include <ntqpainter.h>
#include "qanimationwriter.h"

#include <unistd.h>
#include <sys/ipc.h>
#include <sys/types.h>
#include <sys/shm.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdlib.h>
#include <errno.h>
#include <math.h>

//#define QT_QWS_EXPERIMENTAL_REVERSE_BIT_ENDIANNESS

TQVFbView::TQVFbView( int display_id, int w, int h, int d, TQWidget *parent,
		    const char *name, uint flags )
    : TQScrollView( parent, name, flags ), emulateTouchscreen(false), qwslock(NULL)
{
    displayid = display_id;
    viewport()->setMouseTracking( true );
    viewport()->setFocusPolicy( StrongFocus );
    zm = 1;
    animation = 0;
    int actualdepth=d;

    switch ( d ) {
	case 12:
	    actualdepth=16;
	    break;
	case 1:
	case 4:
	case 8:
	case 16:
	case 32:
	    break;

	default:
	    tqFatal( "Unsupported bit depth %d\n", d );
    }

    mousePipe = TQString(QT_VFB_MOUSE_PIPE).arg(display_id);
    keyboardPipe = TQString(QT_VFB_KEYBOARD_PIPE).arg(display_id);

    unlink( mousePipe.latin1() );
    mkfifo( mousePipe.latin1(), 0666 );

    mouseFd = open( mousePipe.latin1(), O_RDWR | O_NDELAY );
    if ( mouseFd == -1 ) {
	tqFatal( "Cannot open mouse pipe" );
    }

    unlink( keyboardPipe );
    mkfifo( keyboardPipe, 0666 );
    keyboardFd = open( keyboardPipe, O_RDWR | O_NDELAY );
    if ( keyboardFd == -1 ) {
	tqFatal( "Cannot open keyboard pipe" );
    }

    key_t key = ftok( mousePipe.latin1(), 'b' );

    int bpl;
    if ( d == 1 )
	bpl = (w*d+7)/8;
    else
	bpl = ((w*actualdepth+31)/32)*4;

    int dataSize = bpl * h + sizeof( TQVFbHeader );
    shmId = shmget( key, dataSize, IPC_CREAT|0666);
    if ( shmId != -1 )
	data = (unsigned char *)shmat( shmId, 0, 0 );
    else {
	struct shmid_ds shm;
	shmctl( shmId, IPC_RMID, &shm );
	shmId = shmget( key, dataSize, IPC_CREAT|0666);
	data = (unsigned char *)shmat( shmId, 0, 0 );
    }

    if ( (long)data == -1 )
	tqFatal( "Cannot attach to shared memory" );

    hdr = (TQVFbHeader *)data;
    hdr->width = w;
    hdr->height = h;
    viewdepth = d;
    hdr->depth = actualdepth;
    hdr->linestep = bpl;
    hdr->numcols = 0;
    hdr->dataoffset = sizeof( TQVFbHeader );
    hdr->update = TQRect();

    resizeContents( w, h );

    timer = new TQTimer( this );
    connect( timer, TQ_SIGNAL(timeout()), this, TQ_SLOT(timeout()) );

    gammatable=0;
    setGamma(1.0,1.0,1.0);
    setRate( 30 );
}

TQVFbView::~TQVFbView()
{
    stopAnimation();
    sendKeyboardData( 0, 0, 0, true, false ); // magic die key
    delete qwslock;
    struct shmid_ds shm;
    shmdt( (char*)data );
    shmctl( shmId, IPC_RMID, &shm );
    ::close( mouseFd );
    ::close( keyboardFd );
    unlink( mousePipe );
    unlink( keyboardPipe );
}

TQSize TQVFbView::sizeHint() const
{
    int f = 2 * frameWidth();
    return TQSize( contentsWidth() + f, contentsHeight() + f );
}

void TQVFbView::setGamma(double gr, double gg, double gb)
{
    if ( viewdepth < 12 )
	return; // not implemented

    gred=gr; ggreen=gg; gblue=gb;

    switch ( viewdepth ) {
      case 12:
	rsh = 12;
	gsh = 7;
	bsh = 1;
	rmax = 15;
	gmax = 15;
	bmax = 15;
	break;
      case 16:
	rsh = 11;
	gsh = 5;
	bsh = 0;
	rmax = 31;
	gmax = 63;
	bmax = 31;
	break;
      case 32:
	rsh = 16;
	gsh = 8;
	bsh = 0;
	rmax = 255;
	gmax = 255;
	bmax = 255;
    }
    int mm = TQMAX(rmax,TQMAX(gmax,bmax))+1;
    if ( gammatable )
	delete [] gammatable;
    gammatable = new TQRgb[mm];
    for (int i=0; i<mm; i++) {
	int r = int(pow(i,gr)*255/rmax);
	int g = int(pow(i,gg)*255/gmax);
	int b = int(pow(i,gb)*255/bmax);
	if ( r > 255 ) r = 255;
	if ( g > 255 ) g = 255;
	if ( b > 255 ) b = 255;
	gammatable[i] = tqRgb(r,g,b);
//tqDebug("%d: %d,%d,%d",i,r,g,b);
    }

    setDirty(rect());
}

void TQVFbView::getGamma(int i, TQRgb& rgb)
{
    if ( i > 255 ) i = 255;
    if ( i < 0 ) i = 0;
    rgb = tqRgb(tqRed(gammatable[i*rmax/255]),
               tqGreen(gammatable[i*rmax/255]),
               tqBlue(gammatable[i*rmax/255]));
}

int TQVFbView::displayId() const
{
    return displayid;
}

int TQVFbView::displayWidth() const
{
    return hdr->width;
}

int TQVFbView::displayHeight() const
{
    return hdr->height;
}

int TQVFbView::displayDepth() const
{
    return viewdepth;
}

void TQVFbView::setZoom( double z )
{
    if ( zm != z ) {
	zm = z;
	setDirty(TQRect(0,0,hdr->width,hdr->height));
	resizeContents( int(hdr->width*z), int(hdr->height*z) );
	updateGeometry();
	tqApp->sendPostedEvents();
	topLevelWidget()->adjustSize();
	drawScreen();
    }
}

void TQVFbView::setRate( int r )
{
    refreshRate = r;
    timer->start( 1000/r );
}

#ifndef TQ_WS_QWS
// Get the name of the directory where TQt/Embedded temporary data should
// live.
static TQString qws_dataDir()
{
    TQString username = "unknown";
    const char *logname = getenv("LOGNAME");
    if ( logname )
	username = logname;

    TQString dataDir = "/tmp/qtembedded-" + username;
    if ( mkdir( dataDir.latin1(), 0700 ) ) {
	if ( errno != EEXIST ) {
	    tqFatal( "Cannot create TQt/Embedded data directory: %s",
		    dataDir.latin1() );
	}
    }

    struct stat buf;
    if ( lstat( dataDir.latin1(), &buf ) )
	tqFatal( "stat failed for TQt/Embedded data directory: %s",
		dataDir.latin1() );

    if ( !S_ISDIR( buf.st_mode ) )
	tqFatal( "%s is not a directory", dataDir.latin1() );

    if ( buf.st_uid != getuid() )
	tqFatal( "TQt/Embedded data directory is not owned by user %u",
		getuid() );

    if ( (buf.st_mode & 0677) != 0600 )
	tqFatal( "TQt/Embedded data directory has incorrect permissions: %s",
		dataDir.latin1() );

    dataDir += "/";

    return dataDir;
}
#endif

void TQVFbView::initLock()
{
    TQString username = "unknown";
    const char *logname = getenv("LOGNAME");
    if ( logname )
	username = logname;
    qwslock = new TQLock(qws_dataDir() + TQString( TQTE_PIPE ).arg( displayid ),
			'd', true);
}

void TQVFbView::lock()
{
    if ( !qwslock )
	initLock();
    qwslock->lock(TQLock::Read);
}

void TQVFbView::unlock()
{
    if ( qwslock )
	qwslock->unlock();
}

void TQVFbView::sendMouseData( const TQPoint &pos, int buttons )
{
    write( mouseFd, &pos, sizeof( TQPoint ) );
    write( mouseFd, &buttons, sizeof( int ) );
}

void TQVFbView::sendKeyboardData( int unicode, int keycode, int modifiers,
				 bool press, bool repeat )
{
    TQVFbKeyData kd;
    kd.unicode = unicode | (keycode << 16);
    kd.modifiers = modifiers;
    kd.press = press;
    kd.repeat = repeat;
    write( keyboardFd, &kd, sizeof( TQVFbKeyData ) );
}

void TQVFbView::timeout()
{
    lock();
    if ( animation ) {
	    TQRect r( hdr->update );
	    r = r.intersect( TQRect(0, 0, hdr->width, hdr->height ) );
	    if ( r.isEmpty() ) {
		animation->appendBlankFrame();
	    } else {
		int l;
		TQImage img = getBuffer( r, l );
		animation->appendFrame(img,TQPoint(r.x(),r.y()));
	    }
    }
    if ( hdr->dirty ) {
	drawScreen();
    }
    unlock();
}

TQImage TQVFbView::getBuffer( const TQRect &r, int &leading ) const
{
    switch ( viewdepth ) {
      case 12:
      case 16: {
	static unsigned char *imgData = 0;
	if ( !imgData ) {
	    int bpl = ((hdr->width*32+31)/32)*4;
	    imgData = new unsigned char [ bpl * hdr->height ];
	}
	TQImage img( imgData, r.width(), r.height(), 32, 0, 0, TQImage::IgnoreEndian );
	const int rsh = viewdepth == 12 ? 12 : 11;
	const int gsh = viewdepth == 12 ? 7 : 5;
	const int bsh = viewdepth == 12 ? 1 : 0;
	const int rmax = viewdepth == 12 ? 15 : 31;
	const int gmax = viewdepth == 12 ? 15 : 63;
	const int bmax = viewdepth == 12 ? 15 : 31;
	for ( int row = 0; row < r.height(); row++ ) {
	    TQRgb *dptr = (TQRgb*)img.scanLine( row );
	    ushort *sptr = (ushort*)(data + hdr->dataoffset + (r.y()+row)*hdr->linestep);
	    sptr += r.x();
#ifdef QT_QWS_REVERSE_BYTE_ENDIANNESS
	    for ( int col=0; col < r.width()/2; col++ ) {
#else
	    for ( int col=0; col < r.width(); col++ ) {
#endif
		ushort s = *sptr++;
#ifdef QT_QWS_REVERSE_BYTE_ENDIANNESS
		ushort s2 = *sptr++;
		*dptr++ = tqRgb(tqRed(gammatable[(s2>>rsh)&rmax]),tqGreen(gammatable[(s2>>gsh)&gmax]),tqBlue(gammatable[(s2>>bsh)&bmax]));
#endif
		*dptr++ = tqRgb(tqRed(gammatable[(s>>rsh)&rmax]),tqGreen(gammatable[(s>>gsh)&gmax]),tqBlue(gammatable[(s>>bsh)&bmax]));
		//*dptr++ = tqRgb(((s>>rsh)&rmax)*255/rmax,((s>>gsh)&gmax)*255/gmax,((s>>bsh)&bmax)*255/bmax);
	    }
	}
	leading = 0;
	return img;
      }
      case 4: {
	static unsigned char *imgData = 0;
	if ( !imgData ) {
	    int bpl = ((hdr->width*8+31)/32)*4;
	    imgData = new unsigned char [ bpl * hdr->height ];
	}
	TQImage img( imgData, r.width(), r.height(), 8, hdr->clut, 16,
		    TQImage::IgnoreEndian );
	for ( int row = 0; row < r.height(); row++ ) {
	    unsigned char *dptr = img.scanLine( row );
	    unsigned char *sptr = data + hdr->dataoffset + (r.y()+row)*hdr->linestep;
	    sptr += r.x()/2;
	    int col = 0;
#ifdef QT_QWS_EXPERIMENTAL_REVERSE_BIT_ENDIANNESS
	    if ( r.x() & 1 ) {
		*dptr++ = *sptr++ & 0x0f;
		col++;
	    }
	    for ( ; col < r.width()-1; col+=2 ) {
		unsigned char s = *sptr++;
		*dptr++ = s >> 4;
		*dptr++ = s & 0x0f;
	    }
	    if ( !(r.right() & 1) )
		*dptr = *sptr >> 4;
#else
	    if ( r.x() & 1 ) {
		*dptr++ = *sptr++ >> 4;
		col++;
	    }
	    for ( ; col < r.width()-1; col+=2 ) {
		unsigned char s = *sptr++;
		*dptr++ = s & 0x0f;
		*dptr++ = s >> 4;
	    }
	    if ( !(r.right() & 1) )
		*dptr = *sptr & 0x0f;
#endif
	}
	leading = 0;
	return img;
      }
      case 32: {
	leading = r.x();
	return TQImage( data + hdr->dataoffset + r.y() * hdr->linestep,
		    hdr->width, r.height(), hdr->depth, 0,
		    0, TQImage::LittleEndian );
      }
      case 8: {
	leading = r.x();
	return TQImage( data + hdr->dataoffset + r.y() * hdr->linestep,
		    hdr->width, r.height(), hdr->depth, hdr->clut,
		    256, TQImage::LittleEndian );
      }
      case 1: {
	leading = r.x();
	return TQImage( data + hdr->dataoffset + r.y() * hdr->linestep,
		    hdr->width, r.height(), hdr->depth, hdr->clut,
#ifndef QT_QWS_EXPERIMENTAL_REVERSE_BIT_ENDIANNESS
		    0, TQImage::LittleEndian );
#else
		    0, TQImage::BigEndian );
#endif
      }
    }
    return TQImage();
}

void TQVFbView::drawScreen()
{
    TQPainter p( viewport() );

    p.translate( -contentsX(), -contentsY() );

    lock();
    TQRect r( hdr->update );
    hdr->dirty = false;
    hdr->update = TQRect();
    // tqDebug( "update %d, %d, %dx%d", r.y(), r.x(), r.width(), r.height() );
    r = r.intersect( TQRect(0, 0, hdr->width, hdr->height ) );
    if ( !r.isEmpty() )  {
	if ( int(zm) != zm ) {
	    r.rLeft() = int(int(r.left()*zm)/zm);
	    r.rTop() = int(int(r.top()*zm)/zm);
	    r.rRight() = int(int(r.right()*zm+zm+0.0000001)/zm+1.9999);
	    r.rBottom() = int(int(r.bottom()*zm+zm+0.0000001)/zm+1.9999);
	    r.rRight() = TQMIN(r.right(),hdr->width-1);
	    r.rBottom() = TQMIN(r.bottom(),hdr->height-1);
	}
	int leading;
	TQImage img( getBuffer( r, leading ) );
	TQPixmap pm;
	if ( zm == 1 ) {
	    pm.convertFromImage( img );
	} else if ( int(zm) == zm ) {
	    TQWMatrix m;
	    m.scale(zm,zm);
	    pm.convertFromImage( img );
	    pm = pm.xForm(m);
	} else {
	    pm.convertFromImage( img.smoothScale(int(img.width()*zm),int(img.height()*zm)) );
	}
	unlock();
	p.setPen( black );
	p.setBrush( white );
	p.drawPixmap( int(r.x()*zm), int(r.y()*zm), pm,
			int(leading*zm), 0, pm.width(), pm.height() );
    } else {
	unlock();
    }
}

bool TQVFbView::eventFilter( TQObject *obj, TQEvent *e )
{
    if ( obj == viewport() &&
	 (e->type() == TQEvent::FocusIn || e->type() == TQEvent::FocusOut) )
	return true;

    return TQScrollView::eventFilter( obj, e );
}

void TQVFbView::viewportPaintEvent( TQPaintEvent *pe )
{
    TQRect r( pe->rect() );
    r.moveBy( contentsX(), contentsY() );
    r = TQRect(int(r.x()/zm),int(r.y()/zm),
	    int(r.width()/zm)+1,int(r.height()/zm)+1);
    setDirty(r);
    drawScreen();
}

void TQVFbView::setDirty( const TQRect& r )
{
    lock();
    hdr->update |= r;
    hdr->dirty = true;
    unlock();
}

void TQVFbView::contentsMousePressEvent( TQMouseEvent *e )
{
    sendMouseData( e->pos()/zm, e->stateAfter() );
}

void TQVFbView::contentsMouseDoubleClickEvent( TQMouseEvent *e )
{
    sendMouseData( e->pos()/zm, e->stateAfter() );
}

void TQVFbView::contentsMouseReleaseEvent( TQMouseEvent *e )
{
    sendMouseData( e->pos()/zm, e->stateAfter() );
}

void TQVFbView::contentsMouseMoveEvent( TQMouseEvent *e )
{
    if ( !emulateTouchscreen || (e->state() & MouseButtonMask ) )
	sendMouseData( e->pos()/zm, e->state() );
}



void TQVFbView::keyPressEvent( TQKeyEvent *e )
{
    sendKeyboardData(e->text()[0].unicode(), e->key(),
		     e->stateAfter()&(ShiftButton|ControlButton|AltButton),
		     true, e->isAutoRepeat());
}

void TQVFbView::keyReleaseEvent( TQKeyEvent *e )
{
    sendKeyboardData(e->ascii(), e->key(),
		     e->stateAfter()&(ShiftButton|ControlButton|AltButton),
		     false, e->isAutoRepeat());
}


TQImage TQVFbView::image() const
{
    ((TQVFbView*)this)->lock();
    int l;
    TQImage r = getBuffer( TQRect(0, 0, hdr->width, hdr->height), l ).copy();
    ((TQVFbView*)this)->unlock();
    return r;
}

void TQVFbView::startAnimation( const TQString& filename )
{
    delete animation;
    animation = new TQAnimationWriter(filename,"MNG");
    animation->setFrameRate(refreshRate);
    animation->appendFrame(TQImage(data + hdr->dataoffset,
                hdr->width, hdr->height, hdr->depth, hdr->clut,
                256, TQImage::LittleEndian));
}

void TQVFbView::stopAnimation()
{
    delete animation;
    animation = 0;
}

void TQVFbView::setTouchscreenEmulation( bool b )
{
    emulateTouchscreen = b;
}
