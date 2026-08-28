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

#include <ntqscrollview.h>

class TQImage;
class TQTimer;
class TQAnimationWriter;
class TQLock;
struct TQVFbHeader;

class TQVFbView : public TQScrollView
{
    TQ_OBJECT
public:
    TQVFbView( int display_id, int w, int h, int d, TQWidget *parent = 0,
		const char *name = 0, uint wflags = 0 );
    ~TQVFbView();

    int displayId() const;
    int displayWidth() const;
    int displayHeight() const;
    int displayDepth() const;

    bool touchScreenEmulation() const { return emulateTouchscreen; }
    int rate() { return refreshRate; }
    bool animating() const { return !!animation; }
    TQImage image() const;

    void setGamma(double gr, double gg, double gb);
    double gammaRed() const { return gred; }
    double gammaGreen() const { return ggreen; }
    double gammaBlue() const { return gblue; }
    void getGamma(int i, TQRgb& rgb);
    void skinKeyPressEvent( TQKeyEvent *e ) { keyPressEvent(e); }
    void skinKeyReleaseEvent( TQKeyEvent *e ) { keyReleaseEvent(e); }

    double zoom() const { return zm; }

    TQSize sizeHint() const;
public slots:
    void setTouchscreenEmulation( bool );

    void setRate( int );
    void setZoom( double );
    void startAnimation( const TQString& );
    void stopAnimation();

protected slots:
    void timeout();

protected:
    void initLock();
    void lock();
    void unlock();
    TQImage getBuffer( const TQRect &r, int &leading ) const;
    void drawScreen();
    void sendMouseData( const TQPoint &pos, int buttons );
    void sendKeyboardData( int unicode, int keycode, int modifiers,
			   bool press, bool repeat );
    virtual bool eventFilter( TQObject *obj, TQEvent *e );
    virtual void viewportPaintEvent( TQPaintEvent *pe );
    virtual void contentsMousePressEvent( TQMouseEvent *e );
    virtual void contentsMouseDoubleClickEvent( TQMouseEvent *e );
    virtual void contentsMouseReleaseEvent( TQMouseEvent *e );
    virtual void contentsMouseMoveEvent( TQMouseEvent *e );
    virtual void keyPressEvent( TQKeyEvent *e );
    virtual void keyReleaseEvent( TQKeyEvent *e );

private:
    bool emulateTouchscreen;
    void setDirty( const TQRect& );
    int shmId;
    unsigned char *data;
    TQVFbHeader *hdr;
    int viewdepth; // "faked" depth
    int rsh;
    int gsh;
    int bsh;
    int rmax;
    int gmax;
    int bmax;
    double gred, ggreen, gblue;
    TQRgb* gammatable;
    TQLock *qwslock;
    TQTimer *timer;
    int mouseFd;
    int keyboardFd;
    int refreshRate;
    TQString mousePipe;
    TQString keyboardPipe;
    TQAnimationWriter *animation;
    int displayid;
    double zm;
};

