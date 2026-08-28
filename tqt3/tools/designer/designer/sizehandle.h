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

#ifndef SIZEHANDLE_H
#define SIZEHANDLE_H

#include <ntqwidget.h>
#include <ntqintdict.h>
#include <ntqptrdict.h>

class TQMouseEvent;
class FormWindow;
class WidgetSelection;
class TQPaintEvent;

class SizeHandle : public TQWidget
{
    TQ_OBJECT

public:
    enum Direction { LeftTop, Top, RightTop, Right, RightBottom, Bottom, LeftBottom, Left };

    SizeHandle( FormWindow *parent, Direction d, WidgetSelection *s );
    void setWidget( TQWidget *w );
    void setActive( bool a );
    void updateCursor();

    void setEnabled( bool ) {}

protected:
    void paintEvent( TQPaintEvent *e );
    void mousePressEvent( TQMouseEvent *e );
    void mouseMoveEvent( TQMouseEvent *e );
    void mouseReleaseEvent( TQMouseEvent *e );

private:
    void trySetGeometry( TQWidget *w, int x, int y, int width, int height );
    void tryResize( TQWidget *w, int width, int height );

private:
    TQWidget *widget;
    Direction dir;
    TQPoint oldPressPos;
    FormWindow *formWindow;
    WidgetSelection *sel;
    TQRect geom, origGeom;
    bool active;

};

class WidgetSelection
{
public:
    WidgetSelection( FormWindow *parent, TQPtrDict<WidgetSelection> *selDict );

    void setWidget( TQWidget *w, bool updateDict = true );
    bool isUsed() const;

    void updateGeometry();
    void hide();
    void show();
    void update();
    
    TQWidget *widget() const;

protected:
    TQIntDict<SizeHandle> handles;
    TQWidget *wid;
    FormWindow *formWindow;
    TQPtrDict<WidgetSelection> *selectionDict;

};

#endif
