/****************************************************************************
**
** Definition of TQInputDialog class
**
** Created : 991212
**
** Copyright (C) 1992-2008 Trolltech ASA.  All rights reserved.
**
** This file is part of the dialogs module of the TQt GUI Toolkit.
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

#ifndef TQINPUTDIALOG_H
#define TQINPUTDIALOG_H

#ifndef QT_H
#include "ntqdialog.h"
#include "ntqstring.h"
#include "ntqlineedit.h"
#endif // QT_H

#ifndef TQT_NO_INPUTDIALOG

class TQSpinBox;
class TQComboBox;
class TQInputDialogPrivate;

class TQ_EXPORT TQInputDialog : public TQDialog
{
    TQ_OBJECT

private:
    enum Type { LineEdit, SpinBox, ComboBox, EditableComboBox };

    TQInputDialog( const TQString &label, TQWidget* parent=0, const char* name=0,
		 bool modal = true, Type type = LineEdit ); //### 4.0: widget flag!
    ~TQInputDialog();

    TQLineEdit *lineEdit() const;
    TQSpinBox *spinBox() const;
    TQComboBox *comboBox() const;
    TQComboBox *editableComboBox() const;

    void setType( Type t );
    Type type() const;

public:
    //### 4.0: widget flag!
    static TQString getText( const TQString &caption, const TQString &label, TQLineEdit::EchoMode echo = TQLineEdit::Normal,
			    const TQString &text = TQString::null, bool *ok = 0, TQWidget *parent = 0, const char *name = 0 );
    static int getInteger( const TQString &caption, const TQString &label, int value = 0, int minValue = -2147483647,
			   int maxValue = 2147483647,
			   int step = 1, bool *ok = 0, TQWidget *parent = 0, const char *name = 0 );
    static double getDouble( const TQString &caption, const TQString &label, double value = 0,
			     double minValue = -2147483647, double maxValue = 2147483647,
			     int decimals = 1, bool *ok = 0, TQWidget *parent = 0, const char *name = 0 );
    static TQString getItem( const TQString &caption, const TQString &label, const TQStringList &list,
			    int current = 0, bool editable = true,
			    bool *ok = 0, TQWidget *parent = 0, const char *name = 0 );

private slots:
    void textChanged( const TQString &s );
    void tryAccept();

private:
    TQInputDialogPrivate *d;
    friend class TQInputDialogPrivate; /* to avoid 'has no friends' warnings... */

private:	// Disabled copy constructor and operator=
#if defined(TQ_DISABLE_COPY)
    TQInputDialog( const TQInputDialog & );
    TQInputDialog &operator=( const TQInputDialog & );
#endif
};

#endif // TQT_NO_INPUTDIALOG

#endif // TQINPUTDIALOG_H

