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

#include "tqvfb.h"
#include "tqvfbview.h"
#include "tqvfbratedlg.h"
#include "config.h"
#include "skin.h"

#include <ntqmenubar.h>
#include <ntqpopupmenu.h>
#include <ntqapplication.h>
#include <ntqmessagebox.h>
#include <ntqcombobox.h>
#include <ntqlabel.h>
#include <ntqfiledialog.h>
#include <ntqslider.h>
#include <ntqspinbox.h>
#include <ntqradiobutton.h>
#include <ntqimage.h>
#include <ntqpixmap.h>
#include <ntqcursor.h>
#include <ntqdragobject.h>
#include <ntqcheckbox.h>

TQVFb::TQVFb( int display_id, int w, int h, int d, const TQString &skin, TQWidget *parent,
	    const char *name, uint flags )
    : TQMainWindow( parent, name, flags )
{
    const TQMimeSource *m = TQMimeSourceFactory::defaultFactory()->data( "logo.png" );
    if ( m ) {
	TQPixmap pix;
	TQImageDrag::decode( m, pix );
	setIcon( pix );
    }

    imagesave = new TQFileDialog( this, 0, true );

    rateDlg = 0;
    view = 0;
    init( display_id, w, h, d, skin );
    createMenu( menuBar() );
    adjustSize();
}

TQVFb::~TQVFb()
{
}

void TQVFb::popupMenu()
{
    TQPopupMenu *pm = new TQPopupMenu( this );
    createMenu( pm );
    pm->exec(TQCursor::pos());
}

void TQVFb::init( int display_id, int w, int h, int d, const TQString &skin_name )
{
    setCaption( TQString("Virtual framebuffer %1x%2 %3bpp Display :%4")
		    .arg(w).arg(h).arg(d).arg(display_id) );
    delete view;

    if ( !skin_name.isEmpty() && TQFile::exists(skin_name) ) {
	bool vis = isVisible();
	if ( vis ) hide();
	menuBar()->hide();
	Skin *skin = new Skin( this, skin_name, w, h );
	view = new TQVFbView( display_id, w, h, d, skin );
	skin->setView( view );
	view->setMargin( 0 );
	view->setFrameStyle( TQFrame::NoFrame );
	view->setFixedSize( w, h );
	setCentralWidget( skin );
	adjustSize();
	view->show();
	if ( vis ) show();
    } else {
	if ( !currentSkin.isEmpty() ) {
	    clearMask();
	    reparent( 0, 0, pos(), true );
	}
	menuBar()->show();
	view = new TQVFbView( display_id, w, h, d, this );
	view->setMargin( 0 );
	view->setFrameStyle( TQFrame::NoFrame );
	setCentralWidget( view );
	resize( sizeHint() );
	view->show();
    }

    currentSkin = skin_name;
}

void TQVFb::enableCursor( bool e )
{
    view->viewport()->setCursor( e ? ArrowCursor : BlankCursor );
    viewMenu->setItemChecked( cursorId, e );
}

void TQVFb::createMenu(TQMenuData *menu)
{
    TQPopupMenu *file = new TQPopupMenu( this );
    file->insertItem( "&Configure...", this, TQ_SLOT(configure()), ALT+CTRL+Key_C );
    file->insertSeparator();
    file->insertItem( "&Save image...", this, TQ_SLOT(saveImage()), ALT+CTRL+Key_S );
    file->insertItem( "&Animation...", this, TQ_SLOT(toggleAnimation()), ALT+CTRL+Key_A );
    file->insertSeparator();
    file->insertItem( "&Quit", tqApp, TQ_SLOT(quit()) );

    menu->insertItem( "&File", file );

    viewMenu = new TQPopupMenu( this );
    viewMenu->setCheckable( true );
    cursorId = viewMenu->insertItem( "Show &Cursor", this, TQ_SLOT(toggleCursor()) );
    enableCursor(true);
    viewMenu->insertItem( "&Refresh Rate...", this, TQ_SLOT(changeRate()) );
    viewMenu->insertSeparator();
    viewMenu->insertItem( "Zoom scale &1", this, TQ_SLOT(setZoom1()) );
    viewMenu->insertItem( "Zoom scale &2", this, TQ_SLOT(setZoom2()) );
    viewMenu->insertItem( "Zoom scale &3", this, TQ_SLOT(setZoom3()) );
    viewMenu->insertItem( "Zoom scale &4", this, TQ_SLOT(setZoom4()) );
    viewMenu->insertItem( "Zoom scale &0.5", this, TQ_SLOT(setZoomHalf()) );

    menu->insertItem( "&View", viewMenu );

    TQPopupMenu *help = new TQPopupMenu( this );
    help->insertItem("&About", this, TQ_SLOT(about()));
    help->insertItem("About &TQt", this, TQ_SLOT(aboutTQt()));
    menu->insertSeparator();
    menu->insertItem( "&Help", help );
}

void TQVFb::setZoom(double z)
{
    view->setZoom(z);
}

void TQVFb::setZoomHalf()
{
    setZoom(0.5);
}

void TQVFb::setZoom1()
{
    setZoom(1);
}

void TQVFb::setZoom2()
{
    setZoom(2);
}

void TQVFb::setZoom3()
{
    setZoom(3);
}

void TQVFb::setZoom4()
{
    setZoom(4);
}

void TQVFb::saveImage()
{
    TQImage img = view->image();
    TQString filename = imagesave->getSaveFileName("snapshot.png", "*.png", this, "", "Save Image");
    if ( !!filename )
	img.save(filename,"PNG");
}

void TQVFb::toggleAnimation()
{
    if ( view->animating() ) {
	view->stopAnimation();
    } else {
	TQString filename = imagesave->getSaveFileName("animation.mng", "*.mng", this, "", "Save animation");
	if ( !filename ) {
	    view->stopAnimation();
	} else {
	    view->startAnimation(filename);
	}
    }
}

void TQVFb::toggleCursor()
{
    enableCursor( !viewMenu->isItemChecked( cursorId ) );
}

void TQVFb::changeRate()
{
    if ( !rateDlg ) {
	rateDlg = new TQVFbRateDialog( view->rate(), this );
	connect( rateDlg, TQ_SIGNAL(updateRate(int)), view, TQ_SLOT(setRate(int)) );
    }

    rateDlg->show();
}

void TQVFb::about()
{
#if defined( TQ_WS_MAC )
    TQString platform("Mac OS X");
    TQString qt("Mac");
#elif defined( TQ_WS_WIN )
    TQString platform("Windows");
    TQString qt("Windows");
#else
    TQString platform("X11");
    TQString qt("X11");
#endif
    TQMessageBox::about(this, "About TQVFB",
	"<p><b><font size=+2>TQt/Embedded Virtual " + platform + " Framebuffer</font></b></p>"
	"<p></p>"
	"<p>Version 1.0</p>"
	"<p>Copyright (C) 2001-2008 Trolltech ASA. All rights reserved.</p>"
	"<p></p>"
	"<p>This program is licensed to you under the terms of the GNU General "
	"Public License Version 2 as published by the Free Software Foundation. This "
	"gives you legal permission to copy, distribute and/or modify this software "
	"under certain conditions. For details, see the file 'LICENSE.GPL' that came with "
	"this software distribution. If you did not get the file, send email to "
	"info@trolltech.com.</p>\n\n<p>The program is provided AS IS with NO WARRANTY "
	"OF ANY KIND, INCLUDING THE WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS "
	"FOR A PARTICULAR PURPOSE.</p>"
    );
}

void TQVFb::aboutTQt()
{
    TQMessageBox::aboutTQt( this, tr("TQVFB") );
}

void TQVFb::configure()
{
    config = new Config(this,0,true);

    int w = view->displayWidth();
    int h = view->displayHeight();
    TQString skin;
    config->size_width->setValue(w);
    config->size_height->setValue(h);
    config->size_custom->setChecked(true); // unless changed by settings below
    config->size_240_320->setChecked(w==240&&h==320);
    config->size_320_240->setChecked(w==320&&h==240);
    config->size_640_480->setChecked(w==640&&h==480);
    config->skin->setEditable(true);
    if (!currentSkin.isNull()) {
	config->size_skin->setChecked(true);
	config->skin->setEditText(currentSkin);
    }
    config->touchScreen->setChecked(view->touchScreenEmulation());
    config->depth_1->setChecked(view->displayDepth()==1);
    config->depth_4gray->setChecked(view->displayDepth()==4);
    config->depth_8->setChecked(view->displayDepth()==8);
    config->depth_12->setChecked(view->displayDepth()==12);
    config->depth_16->setChecked(view->displayDepth()==16);
    config->depth_32->setChecked(view->displayDepth()==32);
    if ( view->gammaRed() == view->gammaGreen() && view->gammaGreen() == view->gammaBlue() ) {
	config->gammaslider->setValue(int(view->gammaRed()*400));
	config->rslider->setValue(100);
	config->gslider->setValue(100);
	config->bslider->setValue(100);
    } else {
	config->gammaslider->setValue(100);
	config->rslider->setValue(int(view->gammaRed()*400));
	config->gslider->setValue(int(view->gammaGreen()*400));
	config->bslider->setValue(int(view->gammaBlue()*400));
    }
    connect(config->gammaslider, TQ_SIGNAL(valueChanged(int)), this, TQ_SLOT(setGamma400(int)));
    connect(config->rslider, TQ_SIGNAL(valueChanged(int)), this, TQ_SLOT(setR400(int)));
    connect(config->gslider, TQ_SIGNAL(valueChanged(int)), this, TQ_SLOT(setG400(int)));
    connect(config->bslider, TQ_SIGNAL(valueChanged(int)), this, TQ_SLOT(setB400(int)));
    updateGammaLabels();

    double ogr=view->gammaRed(), ogg=view->gammaGreen(), ogb=view->gammaBlue();

    if ( config->exec() ) {
	int id = view->displayId(); // not settable yet
	if ( config->size_240_320->isChecked() ) {
	    w=240; h=320;
	} else if ( config->size_320_240->isChecked() ) {
	    w=320; h=240;
	} else if ( config->size_640_480->isChecked() ) {
	    w=640; h=480;
	} else if ( config->size_skin->isChecked() ) {
	    skin = config->skin->currentText();
	} else {
	    w=config->size_width->value();
	    h=config->size_height->value();
	}
	int d;
	if ( config->depth_1->isChecked() )
	    d=1;
	else if ( config->depth_4gray->isChecked() )
	    d=4;
	else if ( config->depth_8->isChecked() )
	    d=8;
	else if ( config->depth_12->isChecked() )
	    d=12;
	else if ( config->depth_16->isChecked() )
	    d=16;
	else
	    d=32;

	if ( w != view->displayWidth() || h != view->displayHeight()
		|| d != view->displayDepth() || skin != currentSkin )
	    init( id, w, h, d, skin );
	view->setTouchscreenEmulation( config->touchScreen->isChecked() );
    } else {
	view->setGamma(ogr, ogg, ogb);
    }

    delete config;
    config=0;
}

void TQVFb::setGamma400(int n)
{
    double g = n/100.0;
    view->setGamma(config->rslider->value()/100.0*g,
                   config->gslider->value()/100.0*g,
                   config->bslider->value()/100.0*g);
    updateGammaLabels();
}

void TQVFb::setR400(int n)
{
    double g = n/100.0;
    view->setGamma(config->rslider->value()/100.0*g,
                   view->gammaGreen(),
                   view->gammaBlue());
    updateGammaLabels();
}

void TQVFb::setG400(int n)
{
    double g = n/100.0;
    view->setGamma(view->gammaRed(),
                   config->gslider->value()/100.0*g,
                   view->gammaBlue());
    updateGammaLabels();
}

void TQVFb::setB400(int n)
{
    double g = n/100.0;
    view->setGamma(view->gammaRed(),
                   view->gammaGreen(),
                   config->bslider->value()/100.0*g);
    updateGammaLabels();
}

void TQVFb::updateGammaLabels()
{
    config->rlabel->setText(TQString::number(view->gammaRed(),'g',2));
    config->glabel->setText(TQString::number(view->gammaGreen(),'g',2));
    config->blabel->setText(TQString::number(view->gammaBlue(),'g',2));
}
