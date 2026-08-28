/**********************************************************************
**
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

#ifndef EDITORINTERFACEIMPL_H
#define EDITORINTERFACEIMPL_H

#include <editorinterface.h>
#include <ntqobject.h>
#include <ntqguardedptr.h>

class TQTimer;
class ViewManager;
struct DesignerInterface;

class EditorInterfaceImpl : public TQObject, public EditorInterface
{
    TQ_OBJECT

public:
    EditorInterfaceImpl();
    virtual ~EditorInterfaceImpl();

    TQRESULT queryInterface( const TQUuid&, TQUnknownInterface** );
    TQ_REFCOUNT;

    TQWidget *editor( bool readonly, TQWidget *parent, TQUnknownInterface *designerIface );

    void setText( const TQString &txt );
    TQString text() const;
    bool isUndoAvailable() const;
    bool isRedoAvailable() const;
    void undo();
    void redo();
    void cut();
    void copy();
    void paste();
    void selectAll();
    bool find( const TQString &expr, bool cs, bool wo, bool forward, bool startAtCursor );
    bool replace( const TQString &find, const TQString &replace, bool cs, bool wo, bool forward, bool startAtCursor, bool replaceAll );
    void gotoLine( int line );
    void indent();
    void scrollTo( const TQString &txt, const TQString &first );
    void splitView();
    void setContext( TQObject *this_ );
    void readSettings();

    void setError( int line );
    void setStep( int ) {}
    void clearStep() {}
    void setModified( bool m );
    bool isModified() const;
    void setMode( Mode ) {}

    int numLines() const;
    void breakPoints( TQValueList<uint> & ) const {}
    void setBreakPoints( const TQValueList<uint> & ) {}
    void onBreakPointChange( TQObject *receiver, const char *slot );
    void clearStackFrame() {}
    void setStackFrame( int ) {}

protected:
    bool eventFilter( TQObject*, TQEvent* );

private slots:
    void modificationChanged( bool m );
    void intervalChanged();
    void update();

private:
    TQGuardedPtr<ViewManager> viewManager;
    DesignerInterface *dIface;
    TQTimer *updateTimer;

};

#endif
