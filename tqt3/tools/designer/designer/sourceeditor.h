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

#ifndef SOURCEEDITOR_H
#define SOURCEEDITOR_H

#include "../interfaces/editorinterface.h"
#include <ntqvbox.h>
#include <ntqguardedptr.h>

class FormWindow;
class TQCloseEvent;
struct LanguageInterface;
class Project;
class SourceFile;

class SourceEditor : public TQVBox
{
    TQ_OBJECT

public:
    SourceEditor( TQWidget *parent, EditorInterface *iface, LanguageInterface *liface );
    ~SourceEditor();

    void setObject( TQObject *fw, Project *p );
    TQObject *object() const { return obj; }
    Project *project() const { return pro; }
    void setFunction( const TQString &func, const TQString &clss = TQString::null );
    void setClass( const TQString &clss );
    void save();
    bool saveAs();
    void setModified( bool b );
    bool isModified() const;

    static TQString sourceOfObject( TQObject *fw, const TQString &lang, EditorInterface *iface, LanguageInterface *lIface );

    TQString language() const;
    void setLanguage( const TQString &l );

    void editCut();
    void editCopy();
    void editPaste();
    bool editIsUndoAvailable();
    bool editIsRedoAvailable();
    void editUndo();
    void editRedo();
    void editSelectAll();

    void configChanged();
    void refresh( bool allowSave );
    void resetContext();

    EditorInterface *editorInterface() const { return iFace; }

    void setFocus();
    int numLines() const;
    void saveBreakPoints();
    void clearStep();
    void clearStackFrame();
    void resetBreakPoints();

    TQString text() const;

    void checkTimeStamp();

    SourceFile *sourceFile() const;
    FormWindow *formWindow() const;

protected:
    void closeEvent( TQCloseEvent *e );


private:
    EditorInterface *iFace;
    LanguageInterface *lIface;
    TQGuardedPtr<TQObject> obj;
    Project *pro;
    TQString lang;
    TQGuardedPtr<TQWidget> editor;

};

#endif
