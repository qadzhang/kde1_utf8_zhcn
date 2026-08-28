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

#ifndef EDITOR_H
#define EDITOR_H

#include <ntqtextedit.h>

struct Config;
class ParenMatcher;
class EditorCompletion;
class EditorBrowser;
class TQAccel;

class Editor : public TQTextEdit
{
    TQ_OBJECT

public:
    enum Selection {
	Error = 3,
	Step = 4
    };

    Editor( const TQString &fn, TQWidget *parent, const char *name );
    ~Editor();
    virtual void load( const TQString &fn );
    virtual void save( const TQString &fn );
    TQTextDocument *document() const { return TQTextEdit::document(); }
    void placeCursor( const TQPoint &p, TQTextCursor *c ) { TQTextEdit::placeCursor( p, c ); }
    void setDocument( TQTextDocument *doc ) { TQTextEdit::setDocument( doc ); }
    TQTextCursor *textCursor() const { return TQTextEdit::textCursor(); }
    void repaintChanged() { TQTextEdit::repaintChanged(); }

    virtual EditorCompletion *completionManager() { return 0; }
    virtual EditorBrowser *browserManager() { return 0; }
    virtual void configChanged();

    Config *config() { return cfg; }

    void setErrorSelection( int line );
    void setStepSelection( int line );
    void clearStepSelection();
    void clearSelections();

    virtual bool supportsErrors() const { return true; }
    virtual bool supportsBreakPoints() const { return true; }
    virtual void makeFunctionVisible( TQTextParagraph * ) {}

    void drawCursor( bool b ) { TQTextEdit::drawCursor( b ); }

    TQPopupMenu *createPopupMenu( const TQPoint &p );
    bool eventFilter( TQObject *o, TQEvent *e );

    void setEditable( bool b ) { editable = b; }

protected:
    void doKeyboardAction( KeyboardAction action );
    void keyPressEvent( TQKeyEvent *e );

signals:
    void clearErrorMarker();
    void intervalChanged();

private slots:
    void cursorPosChanged( TQTextCursor *c );
    void doChangeInterval();
    void commentSelection();
    void uncommentSelection();

protected:
    ParenMatcher *parenMatcher;
    TQString filename;
    Config *cfg;
    bool hasError;
    TQAccel *accelComment, *accelUncomment;
    bool editable;

};

#endif
