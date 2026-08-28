 /**********************************************************************
** Copyright (C) 2000-2008 Trolltech ASA.  All rights reserved.
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

#ifndef EDITORINTERFACE_H
#define EDITORINTERFACE_H

//
//  W A R N I N G  --  PRIVATE INTERFACES
//  --------------------------------------
//
// This file and the interfaces declared in the file are not
// public. It exists for internal purpose. This header file and
// interfaces may change from version to version (even binary
// incompatible) without notice, or even be removed.
//
// We mean it.
//
//

#include <private/qcom_p.h>
#include <ntqmap.h>
#include <ntqvaluelist.h>

class TQWidget;
class TQObjectList;
class TQObject;

// {8668161a-6037-4220-86b6-ccaa20127df8}
#ifndef IID_Editor
#define IID_Editor TQUuid( 0x8668161a, 0x6037, 0x4220, 0x86, 0xb6, 0xcc, 0xaa, 0x20, 0x12, 0x7d, 0xf8 )
#endif

struct EditorInterface : public TQUnknownInterface
{
    enum Mode
    {
	Editing,
	Debugging
    };

    virtual TQWidget *editor( bool readOnly,
			     TQWidget *parent,
			     TQUnknownInterface *designerIface ) = 0;

    virtual void setText( const TQString &txt ) = 0;
    virtual TQString text() const = 0;
    virtual bool isUndoAvailable() const = 0;
    virtual bool isRedoAvailable() const = 0;
    virtual void undo() = 0;
    virtual void redo() = 0;
    virtual void cut() = 0;
    virtual void copy() = 0;
    virtual void paste() = 0;
    virtual void selectAll() = 0;
    virtual bool find( const TQString &expr, bool cs, bool wo, bool forward, bool startAtCursor ) = 0;
    virtual bool replace( const TQString &find, const TQString &replace, bool cs, bool wo, bool forward, bool startAtCursor, bool replaceAll ) = 0;
    virtual void gotoLine( int line ) = 0;
    virtual void indent() = 0;
    virtual void scrollTo( const TQString &txt, const TQString &first ) = 0;
    virtual void splitView() = 0;
    virtual void setContext( TQObject *this_ ) = 0;
    virtual void setError( int line ) = 0;
    virtual void setStep( int line ) = 0;
    virtual void setStackFrame( int line ) = 0;
    virtual void clearStep() = 0;
    virtual void clearStackFrame() = 0;
    virtual void readSettings() = 0;
    virtual void setModified( bool m ) = 0;
    virtual bool isModified() const = 0;
    virtual int numLines() const = 0;
    virtual void breakPoints( TQValueList<uint> &l ) const = 0;
    virtual void setBreakPoints( const TQValueList<uint> &l ) = 0;
    virtual void setMode( Mode m ) = 0;

    virtual void onBreakPointChange( TQObject *receiver, const char *slot ) = 0;

};

#endif
