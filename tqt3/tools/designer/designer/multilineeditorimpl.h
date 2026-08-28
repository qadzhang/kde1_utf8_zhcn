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

#ifndef MULTILINEEDITORIMPL_H
#define MULTILINEEDITORIMPL_H

#include <ntqaction.h>
#include <ntqtextedit.h>
#include <private/qrichtext_p.h>
#include "multilineeditor.h"

class FormWindow;
class TQToolBar;
class TQTextDocument;

class TextEdit : public TQTextEdit
{
    TQ_OBJECT

public:
    TextEdit( TQWidget *parent = 0, const char *name = 0 );
    TQTextDocument *document() const { return TQTextEdit::document(); }
    TQTextParagraph *paragraph();

};

class ToolBarItem : public TQAction
{
    TQ_OBJECT

public:
    ToolBarItem( TQWidget *parent, TQWidget *toolBar,
                 const TQString &label, const TQString &tagstr,
		 const TQIconSet &icon, const TQKeySequence &key = 0 );
    ~ToolBarItem();
signals:
    void clicked( const TQString &t );

protected slots:
    void wasActivated();
private:
    TQString tag;
};


class MultiLineEditor : public MultiLineEditorBase
{
    TQ_OBJECT

public:
    MultiLineEditor( bool call_static, bool richtextMode, TQWidget *parent, TQWidget *editWidget,
		     FormWindow *fw, const TQString &text = TQString::null );
    bool useWrapping() const;
    void setUseWrapping( bool );
    static TQString getText( TQWidget *parent, const TQString &text, bool richtextMode, bool *useWrap );
    int exec();

protected slots:
    void okClicked();
    void applyClicked();
    void cancelClicked();
    void insertTags( const TQString& );
    void insertBR();
    void showFontDialog();
    void changeWrapMode( bool );

    void closeEvent( TQCloseEvent *e );

private:
    TQString getStaticText();
    TextEdit *textEdit;
    TQTextEdit *mlined;
    FormWindow *formwindow;
    TQToolBar *basicToolBar;
    TQToolBar *fontToolBar;
    TQToolBar *optionsToolBar;
    TQAction *wrapAction;
    int res;
    TQString staticText;
    bool callStatic;
    bool oldDoWrap, doWrap;
    TQVariant oldWrapMode;
    TQString oldWrapString;
};

#endif
