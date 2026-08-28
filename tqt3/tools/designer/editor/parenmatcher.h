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

#ifndef PARENMATCHER_H
#define PARENMATCHER_H

#include <ntqstring.h>
#include <ntqvaluelist.h>

class TQTextCursor;

struct Paren
{
    Paren() : type( Open ), chr( ' ' ), pos( -1 ) {}
    Paren( int t, const TQChar &c, int p ) : type( (Type)t ), chr( c ), pos( p ) {}
    enum Type { Open, Closed };
    Type type;
    TQChar chr;
    int pos;
};

typedef TQValueList<Paren> ParenList;

class ParenMatcher
{
public:
    enum Selection {
	Match = 1,
	Mismatch
    };

    ParenMatcher();

    virtual bool match( TQTextCursor *c );

    void setEnabled( bool b ) { enabled = b; }

private:
    bool checkOpenParen( TQTextCursor *c );
    bool checkClosedParen( TQTextCursor *c );

    bool enabled;

};

#endif
