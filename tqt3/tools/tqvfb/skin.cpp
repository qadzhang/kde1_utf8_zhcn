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

#include "skin.h"
#include "tqvfb.h"
#include "tqvfbview.h"

#include <ntqbitmap.h>
#include <ntqpixmap.h>
#include <ntqtextstream.h>
#include <ntqfile.h>
#include <ntqpainter.h>
#ifdef TQ_WS_MAC
# include <qt_mac.h>
#endif

Skin::Skin( TQVFb *p, const TQString &skinFile, int &viewW, int &viewH ) : TQWidget(p)
{
    TQFile f( skinFile );
    f.open( IO_ReadOnly );
    TQTextStream ts( &f );
    ts >> skinImageUpFileName;
    ts >> skinImageDownFileName;
    ts >> viewX1;
    ts >> viewY1;
    ts >> viewW;
    ts >> viewH;
    ts >> transparancy;
    ts >> numberOfAreas;
//  Debug the skin file parsing
//  printf("read: -%s- -%i- -%i- -%i-\n", skinImage.latin1(), viewX1, viewY1, numberOfAreas );
    areas = new ButtonAreas[numberOfAreas];

    for (int i = 0; i < numberOfAreas; i++) {
	ts >> areas[i].name;
	ts >> areas[i].keyCode;
	ts >> areas[i].x1;
	ts >> areas[i].y1;
	ts >> areas[i].x2;
	ts >> areas[i].y2;
//	Debug the skin file parsing
//	printf("read: -%s- -%i- -%i- -%i- -%i- -%i-\n", areas[i].name.latin1(),
//	    areas[i].keyCode, areas[i].x1, areas[i].y1, areas[i].x2, areas[i].y2 );
    }

    parent = p;
    skinImageUp = new TQPixmap( skinImageUpFileName );
    skinImageDown = new TQPixmap( skinImageDownFileName );
//  setPixmap( ipaq );
    setFixedSize( skinImageUp->size() );
    TQBitmap mask = skinImageUp->createHeuristicMask();
    int wf = WStyle_Customize | WType_TopLevel | WStyle_NoBorder;
    parent->reparent( 0, wf, pos(), true );
    parent->setMask( mask );
    parent->setFixedSize( skinImageUp->size() );
    buttonPressed = false;
    buttonIndex = 0;
}


Skin::~Skin( )
{
}


void Skin::setView( TQVFbView *v )
{
    view = v;
    view->move( viewX1, viewY1 );
#ifdef TQ_WS_MAC
    TQMacSavedPortInfo::setAlphaTransparancy(view, transparancy);
#endif
}


void Skin::paintEvent( TQPaintEvent * )
{
    TQPainter p( this );
//  printf("read: -%s-\n", skinImageUp.latin1());
    if (skinImageUp)
	p.drawPixmap( 0, 0, *skinImageUp );
    if (buttonPressed == true) {
	ButtonAreas *ba = &areas[buttonIndex];
	if (skinImageDown)
	    p.drawPixmap( ba->x1, ba->y1, *skinImageDown, ba->x1, ba->y1, ba->x2 - ba->x1, ba->y2 - ba->y1 );
    }
}


void Skin::mousePressEvent( TQMouseEvent *e )
{
    if (e->button() == RightButton) {
	parent->popupMenu();
    } else {
	buttonPressed = false;

	for (int i = 0; i < numberOfAreas; i++) {
	    TQPoint p1( areas[i].x1, areas[i].y1 );
	    TQPoint p2( areas[i].x2, areas[i].y2 );
	    TQRect r( p1, p2 );
	    if ( r.contains( e->pos() ) ) {
		buttonPressed = true;
		buttonIndex = i;
		buttonCode = areas[buttonIndex].keyCode;
		TQKeyEvent keyEvent( TQEvent::KeyPress, buttonCode, 0, 0 );
		if (view)
		    view->skinKeyPressEvent( &keyEvent );
//		Debug message to be sure we are clicking the right areas
//		printf("%s clicked\n", areas[i].name);
		ButtonAreas *ba = &areas[buttonIndex];
		repaint( ba->x1, ba->y1, ba->x2 - ba->x1, ba->y2 - ba->y1, false );
		continue;
	    }
	}
	
//	This is handy for finding the areas to define rectangles for new skins
//	printf("Clicked in %i,%i\n",  e->pos().x(),  e->pos().y());
	clickPos = e->pos();
    }
}


void Skin::mouseMoveEvent( TQMouseEvent *e )
{
    if ( buttonPressed == false ) {
	TQPoint newpos =  e->globalPos() - clickPos;
	parent->move( newpos );
    }
}


void Skin::mouseReleaseEvent( TQMouseEvent * )
{
    if ( buttonPressed ) {
	TQKeyEvent keyEvent( TQEvent::KeyRelease, buttonCode, 0, 0 );
	if (view)
	    view->skinKeyReleaseEvent( &keyEvent );
	buttonPressed = false;
	ButtonAreas *ba = &areas[buttonIndex];
	repaint( ba->x1, ba->y1, ba->x2 - ba->x1, ba->y2 - ba->y1, false );
    }
}


