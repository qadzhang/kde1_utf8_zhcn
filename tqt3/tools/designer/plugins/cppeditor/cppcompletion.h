/**********************************************************************
**
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

#ifndef CPPCOMPLETION_H
#define CPPCOMPLETION_H

#include <completion.h>
#include <ntqguardedptr.h>

class CppEditorCompletion : public EditorCompletion
{
    TQ_OBJECT

public:
    CppEditorCompletion( Editor *e );

#if defined(Q_USING)
    using EditorCompletion::doObjectCompletion;
#endif
    bool doObjectCompletion( const TQString &object );
    TQValueList<TQStringList> functionParameters( const TQString &func, TQChar &, TQString &prefix, TQString &postfix );
    void setContext( TQObject *this_ );

private:
    TQGuardedPtr<TQObject> ths;

};

#endif
