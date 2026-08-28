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

#ifndef YYREG_H
#define YYREG_H

#include <ntqstringlist.h>
#include <ntqvaluelist.h>

class CppFunction
{
public:
    CppFunction() : cnst( false ), lineno1( 0 ), lineno2( 0 ) { }

    void setReturnType( const TQString& r ) { ret = r; }
    void setScopedName( const TQString& n ) { nam = n; }
    void setParameterList( const TQStringList& p ) { params = p; }
    void setConst( bool c ) { cnst = c; }
    void setBody( const TQString& b ) { bod = b; }
    void setDocumentation( const TQString& d ) { doc = d; }
    void setLineNums( int functionStart, int openingBrace, int closingBrace ) {
	lineno0 = functionStart;
	lineno1 = openingBrace;
	lineno2 = closingBrace;
    }

    const TQString& returnType() const { return ret; }
    const TQString& scopedName() const { return nam; }
    const TQStringList& parameterList() const { return params; }
    bool isConst() const { return cnst; }
    TQString prototype() const;
    const TQString& body() const { return bod; }
    const TQString& documentation() const { return doc; }
    int functionStartLineNum() const { return lineno0; }
    int openingBraceLineNum() const { return lineno1; }
    int closingBraceLineNum() const { return lineno2; }

private:
    TQString ret;
    TQString nam;
    TQStringList params;
    bool cnst;
    TQString bod;
    TQString doc;
    int lineno0;
    int lineno1;
    int lineno2;
};

void extractCppFunctions( const TQString& code, TQValueList<CppFunction> *flist );
TQString canonicalCppProto( const TQString& proto );

#endif
