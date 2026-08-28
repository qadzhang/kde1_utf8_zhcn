/**********************************************************************
** Copyright (C) 2005-2008 Trolltech ASA.  All rights reserved.
**
** This file is part of TQt Designer.
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

#include <ntqvariant.h>

#include "orderindicator.h"
#include "formwindow.h"

#include <ntqpainter.h>
#include <ntqbitmap.h>
#include <ntqapplication.h>

OrderIndicator::OrderIndicator( int i, TQWidget* w, FormWindow *fw )
    : TQWidget( fw, "order_indicator", WMouseNoMask ), formWindow( fw )
{
    order = -1;
    widget = w;
    setBackgroundMode( NoBackground );
    setAutoMask( true );
    setOrder( i, w );
}


OrderIndicator::~OrderIndicator()
{
}


void OrderIndicator::setOrder( int i, TQWidget* wid )
{
    if ( widget != wid )
	return;
    if ( !wid->isVisibleTo( formWindow ) ) {
	hide();
	return;
    }

    if ( order == i ) {
	show();
	raise();
	return;
    }
    order = i;
    int w = fontMetrics().width( TQString::number( i ) ) + 10;
    int h = fontMetrics().lineSpacing() * 3 / 2;
    TQFont f( font() );
    f.setBold( true );
    setFont( f );
    resize( TQMAX( w, h ), h );
    update(); // in case the size didn't change
    reposition();
    show();
    raise();
}

void OrderIndicator::reposition()
{
    TQPoint p =parentWidget()->mapFromGlobal( widget->mapToGlobal( widget->rect().topLeft() ) );
    move( p - TQPoint( width()/3, height()/3 ) );
}


void OrderIndicator::paintEvent( TQPaintEvent * )
{
    TQPainter p( this );
    p.setPen( white );
    p.setBrush( blue );
    p.drawEllipse( rect() );
    p.drawText( rect(), AlignCenter, TQString::number( order ) );
}


void OrderIndicator::updateMask()
{
    TQBitmap bm( size() );
    bm.fill( color0 );
    {
	TQPainter p( &bm, this );
	p.setPen( color1 );
	p.setBrush( color1 );
	p.drawEllipse( rect() );
    }
    setMask( bm );
}

void OrderIndicator::mousePressEvent( TQMouseEvent *e )
{
    TQApplication::sendEvent( widget, e );
}
