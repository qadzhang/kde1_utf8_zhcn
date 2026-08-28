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

#include "styledbutton.h"

#include <ntqcolordialog.h>
#include <ntqpalette.h>
#include <ntqlabel.h>
#include <ntqpainter.h>
#include <ntqimage.h>
#include <ntqpixmap.h>
#include <ntqapplication.h>
#include <ntqdragobject.h>
#include <ntqstyle.h>

StyledButton::StyledButton(TQWidget* parent, const char* name)
    : TQButton( parent, name ), pix( 0 ), spix( 0 ), edit( ColorEditor ), s( 0 ), mousePressed( false )
{
    setMinimumSize( minimumSizeHint() );
    setAcceptDrops( true );

    connect( this, TQ_SIGNAL(clicked()), TQ_SLOT(onEditor()));
}

StyledButton::StyledButton( const TQBrush& b, TQWidget* parent, const char* name, WFlags f )
    : TQButton( parent, name, f ), spix( 0 ), s( 0 )
{
    col = b.color();
    pix = b.pixmap();
    setMinimumSize( minimumSizeHint() );
}

StyledButton::~StyledButton()
{
    if ( pix ) {
	delete pix;
	pix = 0;
    }
    if ( spix ) {
	delete spix;
	spix = 0;
    }
}

void StyledButton::setEditor( EditorType e )
{
    if ( edit == e )
	return;

    edit = e;
    update();
}

StyledButton::EditorType StyledButton::editor() const
{
    return edit;
}

void StyledButton::setColor( const TQColor& c )
{
    col = c;
    update();
}

void StyledButton::setPixmap( const TQPixmap & pm )
{
    if ( !pm.isNull() ) {
	delete pix;
	pix = new TQPixmap( pm );
    } else {
	delete pix;
	pix = 0;
    }
    scalePixmap();
}

TQColor StyledButton::color() const
{
    return col;
}

TQPixmap* StyledButton::pixmap() const
{
    return pix;
}

bool StyledButton::scale() const
{
    return s;
}

void StyledButton::setScale( bool on )
{
    if ( s == on )
	return;

    s = on;
    scalePixmap();
}

TQSize StyledButton::sizeHint() const
{
    return TQSize( 50, 25 );
}

TQSize StyledButton::minimumSizeHint() const
{
    return TQSize( 50, 25 );
}

void StyledButton::scalePixmap()
{
    delete spix;

    if ( pix ) {
	spix = new TQPixmap( 6*width()/8, 6*height()/8 );
	TQImage img = pix->convertToImage();

	spix->convertFromImage( s? img.smoothScale( 6*width()/8, 6*height()/8 ) : img );
    } else {
	spix = 0;
    }

    update();
}

void StyledButton::resizeEvent( TQResizeEvent* e )
{
    scalePixmap();
    TQButton::resizeEvent( e );
}

void StyledButton::drawButton( TQPainter *paint )
{
    style().drawPrimitive(TQStyle::PE_ButtonBevel, paint, rect(), colorGroup(),
			  isDown() ? TQStyle::Style_Sunken : TQStyle::Style_Default);
    drawButtonLabel(paint);

    if (hasFocus())
	style().drawPrimitive(TQStyle::PE_FocusRect, paint,
			      style().subRect(TQStyle::SR_PushButtonFocusRect, this),
			      colorGroup(), TQStyle::Style_Default);
}

void StyledButton::drawButtonLabel( TQPainter *paint )
{
    TQColor pen = isEnabled() ?
		 hasFocus() ? palette().active().buttonText() : palette().inactive().buttonText()
		 : palette().disabled().buttonText();
    paint->setPen( pen );

    if(!isEnabled()) {
	paint->setBrush( TQBrush( colorGroup().button() ) );
    }
    else if ( edit == PixmapEditor && spix ) {
	paint->setBrush( TQBrush( col, *spix ) );
	paint->setBrushOrigin( width()/8, height()/8 );
    } else
	paint->setBrush( TQBrush( col ) );

    paint->drawRect( width()/8, height()/8, 6*width()/8, 6*height()/8 );
}

void StyledButton::onEditor()
{
    switch (edit) {
    case ColorEditor: {
	TQColor c = TQColorDialog::getColor( palette().active().background(), this );
	if ( c.isValid() ) {
	    setColor( c );
	    emit changed();
	}
    } break;
    case PixmapEditor: {
	TQPixmap p;
        /*
        if ( pixmap() )
		p = qChoosePixmap( this,*pixmap() );
        else
		p = qChoosePixmap( this, TQPixmap() );
	if ( !p.isNull() ) {
	    setPixmap( p );
	    emit changed();
	}
        */
    } break;
    default:
	break;
    }
}

void StyledButton::mousePressEvent(TQMouseEvent* e)
{
    TQButton::mousePressEvent(e);
    mousePressed = true;
    pressPos = e->pos();
}

void StyledButton::mouseMoveEvent(TQMouseEvent* e)
{
    TQButton::mouseMoveEvent( e );
#ifndef TQT_NO_DRAGANDDROP
    if ( !mousePressed )
	return;
    if ( ( pressPos - e->pos() ).manhattanLength() > TQApplication::startDragDistance() ) {
	if ( edit == ColorEditor ) {
	    TQColorDrag *drg = new TQColorDrag( col, this );
	    TQPixmap pix( 25, 25 );
	    pix.fill( col );
	    TQPainter p( &pix );
	    p.drawRect( 0, 0, pix.width(), pix.height() );
	    p.end();
	    drg->setPixmap( pix );
	    mousePressed = false;
	    drg->dragCopy();
	}
	else if ( edit == PixmapEditor && pix && !pix->isNull() ) {
	    TQImage img = pix->convertToImage();
	    TQImageDrag *drg = new TQImageDrag( img, this );
	    if(spix)
		drg->setPixmap( *spix );
	    mousePressed = false;
	    drg->dragCopy();
	}
    }
#endif
}

#ifndef TQT_NO_DRAGANDDROP
void StyledButton::dragEnterEvent( TQDragEnterEvent *e )
{
    setFocus();
    if ( edit == ColorEditor && TQColorDrag::canDecode( e ) )
	e->accept();
    else if ( edit == PixmapEditor && TQImageDrag::canDecode( e ) )
	e->accept();
    else
	e->ignore();
}

void StyledButton::dragLeaveEvent( TQDragLeaveEvent * )
{
    if ( hasFocus() )
	parentWidget()->setFocus();
}

void StyledButton::dragMoveEvent( TQDragMoveEvent *e )
{
    if ( edit == ColorEditor && TQColorDrag::canDecode( e ) )
	e->accept();
    else if ( edit == PixmapEditor && TQImageDrag::canDecode( e ) )
	e->accept();
    else
	e->ignore();
}

void StyledButton::dropEvent( TQDropEvent *e )
{
    if ( edit == ColorEditor && TQColorDrag::canDecode( e ) ) {
	TQColor color;
	TQColorDrag::decode( e, color );
	setColor(color);
	emit changed();
	e->accept();
    }
    else if ( edit == PixmapEditor && TQImageDrag::canDecode( e ) ) {
	TQImage img;
	TQImageDrag::decode( e, img );
	TQPixmap pm;
	pm.convertFromImage(img);
	setPixmap(pm);
	emit changed();
	e->accept();
    } else {
	e->ignore();
    }
}
#endif // TQT_NO_DRAGANDDROP
