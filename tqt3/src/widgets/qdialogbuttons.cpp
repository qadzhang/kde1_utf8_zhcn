/****************************************************************************
**
** Implementation of TQDialogButtons class
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

#include "qdialogbuttons_p.h"
#ifndef TQT_NO_DIALOGBUTTONS

#include <ntqapplication.h>
#include <ntqpushbutton.h>
#include <ntqguardedptr.h>
#include <ntqmap.h>
#include <ntqvariant.h>
#ifndef TQT_NO_DIALOG
#include <ntqdialog.h>
#endif // TQT_NO_DIALOG
#include <ntqlayout.h>
#include <ntqstyle.h>
#include <ntqmap.h>

struct TQDialogButtonsPrivate
{
    TQMap<int, TQString> text;
    TQMap<TQDialogButtons::Button, TQWidget *> buttons;
    TQGuardedPtr<TQWidget> custom;
    TQ_UINT32 enabled, visible;
    TQDialogButtons::Button def;
    TQt::Orientation orient;
    bool questionMode;
};

#ifndef TQT_NO_DIALOG
TQDialogButtons::TQDialogButtons(TQDialog *parent, bool autoConnect, TQ_UINT32 buttons,
			       Orientation orient, const char *name ) : TQWidget(parent, name)
{
    init(buttons, orient);
    if(parent && autoConnect) {
	TQObject::connect(this, TQ_SIGNAL(acceptClicked()), parent, TQ_SLOT(accept()));
	TQObject::connect(this, TQ_SIGNAL(rejectClicked()), parent, TQ_SLOT(reject()));
    }
}
#endif // TQT_NO_DIALOG

TQDialogButtons::TQDialogButtons(TQWidget *parent, TQ_UINT32 buttons, 
			       Orientation orient, const char *name ) : TQWidget(parent, name)
{
    init(buttons, orient);
}

void
TQDialogButtons::init(TQ_UINT32 buttons, Orientation orient)
{
    if(buttons == All) {
	tqWarning("TQDialogButtons: cannot specify All by itself!");
	buttons = None;
    }
    d = new TQDialogButtonsPrivate;
    d->questionMode = false;
    d->orient = orient;
    d->def = (Button)style().styleHint(TQStyle::SH_DialogButtons_DefaultButton, this);
    d->enabled = d->visible = buttons;
}

TQDialogButtons::~TQDialogButtons()
{
    delete (TQWidget *)d->custom;
    delete d;
}

void
TQDialogButtons::setQuestionMode(bool b)
{
    d->questionMode = b;
}

bool
TQDialogButtons::questionMode() const
{
    return d->questionMode;
}

void
TQDialogButtons::setButtonEnabled(Button button, bool enabled)
{
    if(enabled) 
	d->enabled |= button;
    else
	d->enabled ^= button;
    if(d->buttons.contains(button))
	d->buttons[button]->setEnabled(enabled);
}

bool
TQDialogButtons::isButtonEnabled(Button button) const
{
    return ((int)(d->enabled & button)) == button;
}

void
TQDialogButtons::setButtonVisible(Button button, bool visible)
{
    if(visible) {
	if(d->buttons.contains(button))
	    d->buttons[button]->show();
	d->visible |= button;
    } else {
	if(d->buttons.contains(button))
	    d->buttons[button]->hide();
	d->visible ^= button;
    }
    layoutButtons();
}

bool
TQDialogButtons::isButtonVisible(Button button) const
{
    return ((int)(d->visible & button)) == button;
}

TQ_UINT32
TQDialogButtons::visibleButtons() const
{
    return d->visible;
}

void
TQDialogButtons::addWidget(TQWidget *w)
{
    TQBoxLayout *lay = NULL;
    if(!d->custom) {
	d->custom = new TQWidget(this, "dialog_custom_area");
	if(orientation() == Horizontal)
	    lay = new TQHBoxLayout(d->custom);
	else
	    lay = new TQVBoxLayout(d->custom);
	layoutButtons();
    } else {
	lay = (TQBoxLayout*)d->custom->layout();
    }
    if(w->parent() != d->custom)
	w->reparent(d->custom, 0, TQPoint(0, 0), true);
    lay->addWidget(w);
}

void
TQDialogButtons::setDefaultButton(Button button)
{
    if(!((int)(d->visible & button) == button)) {
	tqWarning("TQDialogButtons: Button '%d' is not visible (so cannot be default)", button);
	return;
    }
    if(d->def != button) {
#ifndef TQT_NO_PROPERTIES
	if(d->buttons.contains(d->def))
	    d->buttons[d->def]->setProperty("default", TQVariant(false));
#endif
	d->def = button;
#ifndef TQT_NO_PROPERTIES
	if(d->buttons.contains(d->def))
	    d->buttons[d->def]->setProperty("default", TQVariant(false));
#endif
    }
}

TQDialogButtons::Button
TQDialogButtons::defaultButton() const
{
    return d->def;
}

void
TQDialogButtons::setButtonText(Button button, const TQString &str)
{
    d->text[button] = str;
#ifndef TQT_NO_PROPERTIES
    if(d->buttons.contains(button))
	d->buttons[button]->setProperty("text", TQVariant(str));
#endif
    layoutButtons();
}

TQString
TQDialogButtons::buttonText(Button b) const
{
    if(d->text.contains(b))
	return d->text[b];
    return TQString(); //null if it is default..
}

void
TQDialogButtons::setOrientation(Orientation orient)
{
    if(d->orient != orient) {
	d->orient = orient;
	if(d->custom && d->custom->layout())
	    ((TQBoxLayout*)d->custom->layout())->setDirection(orient == Horizontal ? TQBoxLayout::LeftToRight : 
							     TQBoxLayout::TopToBottom);
	layoutButtons();
    }
}

TQt::Orientation
TQDialogButtons::orientation() const
{
    return d->orient;
}

TQWidget *
TQDialogButtons::createButton(Button b)
{
    TQPushButton *ret = new TQPushButton(this, "qdialog_button");
    TQObject::connect(ret, TQ_SIGNAL(clicked()), this, TQ_SLOT(handleClicked()));
    if(d->text.contains(b)) {
	ret->setText(d->text[b]);
    } else {
	switch(b) {
	case All: {
	    TQString txt = buttonText(defaultButton());
	    if(txt.isNull()) {
		if(defaultButton() == Accept) {
		    if(questionMode())
			txt = tr("Yes to All");
		    else
		        txt = tr("OK to All");
		} else {
		    if(questionMode())
		        txt = tr("No to All");
		    else
			txt = tr("Cancel All");
		}
	    } else {
		txt += tr(" to All"); //ick, I can't really do this!!
	    }
	    ret->setText(txt);
	    break; }
	case Accept:
	    if(questionMode())
		ret->setText(tr("Yes"));
	    else
		ret->setText(tr("OK"));
	    break;
	case Reject:
	    if(questionMode())
		ret->setText(tr("No"));
	    else
		ret->setText(tr("Cancel"));
	    break;
	case Apply:
	    ret->setText(tr("Apply"));
	    break;
	case Ignore:
	    ret->setText(tr("Ignore"));
	    break;
	case Retry:
	    ret->setText(tr("Retry"));
	    break;
	case Abort:
	    ret->setText(tr("Abort"));
	    break;
	case Help:
	    ret->setText(tr("Help"));
	    break;
	default:
	    break;
	}
    }
    return ret;
}

void
TQDialogButtons::handleClicked()
{
    const TQObject *s = sender();
    if(!s)
	return;

    for(TQMapIterator<TQDialogButtons::Button, TQWidget *> it = d->buttons.begin(); it != d->buttons.end(); ++it) {
	if(it.data() == s) {
	    emit clicked((TQDialogButtons::Button)it.key());
	    switch(it.key()) {
	    case Retry:
		emit retryClicked();
		break;
	    case Ignore:
		emit ignoreClicked();
		break;
	    case Abort:
		emit abortClicked();
		break;
	    case All:
		emit allClicked();
		break;
	    case Accept:
		emit acceptClicked();
		break;
	    case Reject:
		emit rejectClicked();
		break;
	    case Apply:
		emit applyClicked();
		break;
	    case Help:
		emit helpClicked();
		break;
	    default:
		break;
	    }
	    return;
	}
    }
}

void
TQDialogButtons::resizeEvent(TQResizeEvent *)
{
    layoutButtons();
}

void
TQDialogButtons::showEvent(TQShowEvent *)
{
    layoutButtons();
}

void
TQDialogButtons::styleChanged(TQStyle &old)
{
    layoutButtons();
    TQWidget::styleChange(old);
}

void
TQDialogButtons::layoutButtons()
{
    if(!isVisible()) //nah..
	return;

    TQStyle::SubRect rects[] = {
    	TQStyle::SR_DialogButtonAccept, TQStyle::SR_DialogButtonReject,
	TQStyle::SR_DialogButtonApply,  TQStyle::SR_DialogButtonHelp, 
	TQStyle::SR_DialogButtonCustom, TQStyle::SR_DialogButtonAll,
        TQStyle::SR_DialogButtonRetry,  TQStyle::SR_DialogButtonIgnore,
        TQStyle::SR_DialogButtonAbort };
    for(unsigned int i = 0; i < (sizeof(rects) / sizeof(rects[0])); i++) {
	TQWidget *w = NULL;
	if(rects[i] == TQStyle::SR_DialogButtonCustom) {
	    w = d->custom;
	} else {
	    Button b = None;
	    if(rects[i] == TQStyle::SR_DialogButtonApply)
		b = Apply;
	    else if(rects[i] == TQStyle::SR_DialogButtonAll)
		b = All;
	    else if(rects[i] == TQStyle::SR_DialogButtonAccept)
		b = Accept;
	    else if(rects[i] == TQStyle::SR_DialogButtonReject)
		b = Reject;
	    else if(rects[i] == TQStyle::SR_DialogButtonHelp)
		b = Help;
	    else if(rects[i] == TQStyle::SR_DialogButtonRetry)
		b = Retry;
	    else if(rects[i] == TQStyle::SR_DialogButtonAbort)
		b = Abort;
	    else if(rects[i] == TQStyle::SR_DialogButtonIgnore)
		b = Ignore;
	    if(b != None) {
		if(d->buttons.contains(b)) {
		    w = d->buttons[b];
		    if(!(d->visible & b)) {
			w->hide();
			continue;
		    }
		} else if(d->visible & b) {
		    w = createButton(b);
		    d->buttons.insert(b, w);
		} else {
		    continue;
		}
		if(w) {
		    if(b == d->def) {
			w->setFocus();
#ifndef TQT_NO_PROPERTIES
			w->setProperty("default", TQVariant(true));
#endif
		    }
		    w->setEnabled(d->enabled & b);
		}
	    }
	}
	if(w) {
	    w->show();
	    w->setGeometry(style().subRect(rects[i], this));
	}
    }
}

TQSize
TQDialogButtons::sizeHint() const
{
    constPolish();
    TQSize s;
    if(d->custom)
	s = d->custom->sizeHint();
    return (style().sizeFromContents(TQStyle::CT_DialogButtons, this, s.
				     expandedTo(TQApplication::globalStrut())));
}

TQSize
TQDialogButtons::sizeHint(TQDialogButtons::Button button) const
{
    TQWidget *w = NULL;
    if(d->visible & button) {
	if(!d->buttons.contains(button)) {
	    TQDialogButtons *that = (TQDialogButtons*)this; //ick, constness..
	    w = that->createButton(button);
	    that->d->buttons.insert(button, w);
	} else {
	    w = d->buttons[button];
	}
    }
    return w->sizeHint();
}

TQSize
TQDialogButtons::minimumSizeHint() const
{
    return sizeHint();
}
#endif
