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

#ifndef LISTDND_H
#define LISTDND_H

#include <ntqobject.h>
#include <ntqscrollview.h>

class ListDnd : public TQObject
{
    TQ_OBJECT
public:
    enum DragMode { None = 0, External = 1, Internal = 2, Both = 3, Move = 4, NullDrop = 8 };
    ListDnd( TQScrollView * eventSource, const char * name = 0 );
    void setDragMode( int mode );
    int dragMode() const;
    bool eventFilter( TQObject *, TQEvent * event );

protected:
    virtual bool dragEnterEvent( TQDragEnterEvent * event );
    virtual bool dragLeaveEvent( TQDragLeaveEvent * );
    virtual bool dragMoveEvent( TQDragMoveEvent * event );
    virtual bool dropEvent( TQDropEvent * event );
    virtual bool mousePressEvent( TQMouseEvent * event );
    virtual bool mouseMoveEvent( TQMouseEvent * event );
    virtual void updateLine( const TQPoint & dragPos );
    virtual bool canDecode( TQDragEnterEvent * event );

    TQScrollView * src;
    TQWidget * line;
    TQPoint mousePressPos;
    TQPoint dragPos;
    bool dragInside;
    bool dragDelete;
    bool dropConfirmed;
    int dMode;
};

#endif // LISTDND_H
