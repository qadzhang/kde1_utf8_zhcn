/**********************************************************************
** Copyright (C) 2000-2008 Trolltech ASA.  All rights reserved.
**
** This file is part of TQt Linguist.
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

#ifndef PHRASEBOOKBOX_H
#define PHRASEBOOKBOX_H

#include "phrase.h"

#include <ntqdialog.h>
#include <ntqvaluelist.h>

class TQLineEdit;
class TQPushButton;
class TQListViewItem;

class PhraseLV;

class PhraseBookBox : public TQDialog
{
    TQ_OBJECT
public:
    PhraseBookBox( const TQString& filename, const PhraseBook& phraseBook,
		   TQWidget *parent = 0, const char *name = 0,
		   bool modal = false );

    const PhraseBook& phraseBook() const { return pb; }

protected:
    virtual void keyPressEvent( TQKeyEvent *ev );

private slots:
    void newPhrase();
    void removePhrase();
    void save();
    void sourceChanged( const TQString& source );
    void targetChanged( const TQString& target );
    void definitionChanged( const TQString& definition );
    void selectionChanged( TQListViewItem *item );

private:
    void selectItem( TQListViewItem *item );
    void enableDisable();

    TQLineEdit *sourceLed;
    TQLineEdit *targetLed;
    TQLineEdit *definitionLed;
    TQPushButton *newBut;
    TQPushButton *removeBut;
    PhraseLV *lv;
    TQString fn;
    PhraseBook pb;
};

#endif
