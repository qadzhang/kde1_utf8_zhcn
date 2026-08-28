/****************************************************************************
**
** Definition of TQDialogButtons class.
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

#ifndef TQDIALOGBUTTONS_P_H
#define TQDIALOGBUTTONS_P_H

#ifndef QT_H
#ifndef QT_H
#include "ntqwidget.h"
#endif // QT_H
#endif

#ifndef TQT_NO_DIALOGBUTTONS
struct TQDialogButtonsPrivate;

class
TQDialogButtons : public TQWidget
{
    TQ_OBJECT
public:
    enum Button { None=0, Accept=0x01, Reject=0x02, Help=0x04, Apply=0x08, All=0x10, Abort=0x20, Retry=0x40, Ignore=0x80 };
#ifndef TQT_NO_DIALOG
    TQDialogButtons(TQDialog *parent, bool autoConnect = true, TQ_UINT32 buttons = Accept | Reject,
		   Orientation orient = Horizontal, const char *name = NULL);
#endif // TQT_NO_DIALOG
    TQDialogButtons(TQWidget *parent, TQ_UINT32 buttons = Accept | Reject, 
		   Orientation orient = Horizontal, const char *name = NULL);
    ~TQDialogButtons();

    void setQuestionMode(bool);
    bool questionMode() const;

    void setButtonEnabled(Button button, bool enabled);
    bool isButtonEnabled(Button) const;

    inline void showButton(Button b) { setButtonVisible(b, true) ; }
    inline void hideButton(Button b) { setButtonVisible(b, false); }
    virtual void setButtonVisible(Button, bool visible);
    bool isButtonVisible(Button) const;
    TQ_UINT32 visibleButtons() const;

    void addWidget(TQWidget *);

    virtual void setDefaultButton(Button);
    Button defaultButton() const;

    virtual void setButtonText(Button, const TQString &);
    TQString buttonText(Button) const;

    void setOrientation(Orientation);
    Orientation orientation() const;

    virtual TQSize sizeHint(Button) const;
    TQSize minimumSizeHint() const;
    TQSize sizeHint() const;

protected:
    void layoutButtons();
    virtual TQWidget *createButton(Button);

    void showEvent(TQShowEvent *);
    void resizeEvent(TQResizeEvent *);
    void styleChanged(TQStyle &);

private slots:
    void handleClicked();

signals:
    void clicked(Button);
    void acceptClicked();
    void rejectClicked();
    void helpClicked();
    void applyClicked();
    void allClicked();
    void retryClicked();
    void ignoreClicked();
    void abortClicked();

private:
    TQDialogButtonsPrivate *d;
    void init(TQ_UINT32, Orientation);
};
#endif //TQT_NO_DIALOGBUTTONS
#endif //TQDIALOGBUTTONS_P_H
