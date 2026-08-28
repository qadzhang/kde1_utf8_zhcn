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

#ifndef COMPLETION_H
#define COMPLETION_H

#include <ntqstring.h>
#include <ntqstringlist.h>
#include <ntqobject.h>
#include <ntqmap.h>

class TQTextDocument;
class Editor;
class TQVBox;
class TQListBox;
class ArgHintWidget;

struct CompletionEntry
{
    TQString type;
    TQString text;
    TQString postfix;
    TQString prefix;
    TQString postfix2;

    bool operator==( const CompletionEntry &c ) const {
	return ( c.type == type &&
		 c.text == text &&
		 c.postfix == postfix &&
		 c.prefix == prefix &&
		 c.postfix2 == postfix2 );
    }
};

class EditorCompletion : public TQObject
{
    TQ_OBJECT

public:
    EditorCompletion( Editor *e );
    ~EditorCompletion();

    virtual void addCompletionEntry( const TQString &s, TQTextDocument *doc, bool strict );
    virtual TQValueList<CompletionEntry> completionList( const TQString &s, TQTextDocument *doc ) const;
    virtual void updateCompletionMap( TQTextDocument *doc );

    bool eventFilter( TQObject *o, TQEvent *e );
    virtual void setCurrentEdior( Editor *e );
    virtual bool doCompletion();
    virtual bool doObjectCompletion();
    virtual bool doObjectCompletion( const TQString &object );
    virtual bool doArgumentHint( bool useIndex );

    virtual void addEditor( Editor *e );
    virtual TQValueList<TQStringList> functionParameters( const TQString &func, TQChar &, TQString &prefix, TQString &postfix );

    virtual void setContext( TQObject *this_ );

    void setEnabled( bool b ) { enabled = b; }

protected:
    virtual bool continueComplete();
    virtual void showCompletion( const TQValueList<CompletionEntry> &lst );
    virtual void completeCompletion();

protected:
    TQVBox *completionPopup;
    TQListBox *completionListBox;
    ArgHintWidget *functionLabel;
    int completionOffset;
    Editor *curEditor;
    TQString searchString;
    TQValueList<CompletionEntry> cList;
    TQMap<TQChar, TQStringList> completionMap;
    bool enabled;
    TQTextDocument *lastDoc;

};

#endif
