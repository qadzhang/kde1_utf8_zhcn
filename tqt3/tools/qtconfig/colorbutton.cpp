/**********************************************************************
** Copyright (C) 2000-2008 Trolltech ASA.  All rights reserved.
**
** This file is part of TQt Configuration.
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

#include "colorbutton.h"

#include <ntqapplication.h>
#include <ntqcolordialog.h>
#include <ntqpainter.h>
#include <ntqdragobject.h>
#include <ntqstyle.h>


ColorButton::ColorButton(TQWidget *parent, const char *name)
    : TQButton(parent, name), mousepressed(false)
{
    setAcceptDrops(true);
    col = black;
    connect(this, TQ_SIGNAL(clicked()), TQ_SLOT(changeColor()));
}


ColorButton::ColorButton(const TQColor &c, TQWidget *parent, const char *name)
    : TQButton(parent, name)
{
    setAcceptDrops(true);
    col = c;
    connect(this, TQ_SIGNAL(clicked()), TQ_SLOT(changeColor()));
}


void ColorButton::setColor(const TQColor &c)
{
    col = c;
    update();
}


void ColorButton::changeColor()
{
    TQColor c = TQColorDialog::getColor(col, tqApp->activeWindow());

    if (c.isValid()) {
	setColor(c);
	emit colorChanged(color());
    }
}


TQSize ColorButton::sizeHint() const
{
    return TQSize(40, 25);
}


TQSize ColorButton::minimumSizeHint() const
{
    return TQSize(40, 25);
}


void ColorButton::drawButton(TQPainter *p)
{
    style().drawPrimitive(TQStyle::PE_ButtonBevel, p, rect(), colorGroup(),
			  isDown() ? TQStyle::Style_Down : TQStyle::Style_Raised);
    drawButtonLabel(p);

    if (hasFocus())
	style().drawPrimitive(TQStyle::PE_FocusRect, p,
			      style().subRect(TQStyle::SR_PushButtonFocusRect, this),
			      colorGroup(), TQStyle::Style_Default);
}


void ColorButton::drawButtonLabel(TQPainter *p)
{
    TQColor pen = (isEnabled() ?
		  (hasFocus() ? palette().active().buttonText() :
		   palette().inactive().buttonText())
		  : palette().disabled().buttonText());
    p->setPen( pen );
    p->setBrush(col);
    p->drawRect(width() / 4, height() / 4, width() / 2, height() / 2);
}


void ColorButton::dragEnterEvent(TQDragEnterEvent *e)
{
    if (! TQColorDrag::canDecode(e)) {
	e->ignore();
	return;
    }
}


void ColorButton::dragMoveEvent(TQDragMoveEvent *e)
{
    if (! TQColorDrag::canDecode(e)) {
	e->ignore();
	return;
    }

    e->accept();
}


void ColorButton::dropEvent(TQDropEvent *e)
{
    if (! TQColorDrag::canDecode(e)) {
	e->ignore();
	return;
    }

    TQColor c;
    TQColorDrag::decode(e, c);
    setColor(c);
    emit colorChanged(color());
}


void ColorButton::mousePressEvent(TQMouseEvent *e)
{
    presspos = e->pos();
    mousepressed = true;
    TQButton::mousePressEvent(e);
}


void ColorButton::mouseReleaseEvent(TQMouseEvent *e)
{
    mousepressed = false;
    TQButton::mouseReleaseEvent(e);
}


void ColorButton::mouseMoveEvent(TQMouseEvent *e)
{
    if (! mousepressed)
	return;

    if ((presspos - e->pos()).manhattanLength() > TQApplication::startDragDistance()) {
	mousepressed = false;
	setDown(false);

	TQColorDrag *cd = new TQColorDrag(color(), this);
	cd->dragCopy();
    }
}
