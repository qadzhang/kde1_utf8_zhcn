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

#ifndef LAYOUT_H
#define LAYOUT_H

#include <ntqwidget.h>
#include <ntqmap.h>
#include <ntqguardedptr.h>
#include <ntqobject.h>
#include <ntqlayout.h>
#include <ntqmap.h>
#include <ntqwidgetlist.h>

class FormWindow;
class TQPaintEvent;

class Layout : public TQObject
{
    TQ_OBJECT

public:
    Layout( const TQWidgetList &wl, TQWidget *p, FormWindow *fw, TQWidget *lb, bool doSetup = true, bool splitter = false );
    virtual ~Layout() {}

    virtual void doLayout() = 0;
    virtual void undoLayout();
    virtual void breakLayout();
    virtual bool prepareLayout( bool &needMove, bool &needReparent );
    virtual void finishLayout( bool needMove, TQLayout *layout );

protected:
    TQWidgetList widgets;
    TQWidget *parent;
    TQPoint startPoint;
    TQMap<TQGuardedPtr<TQWidget>, TQRect> geometries;
    TQWidget *layoutBase;
    FormWindow *formWindow;
    TQRect oldGeometry;
    bool isBreak;
    bool useSplitter;

protected:
    virtual void setup();

protected slots:
    void widgetDestroyed();

};

class HorizontalLayout : public Layout
{
public:
    HorizontalLayout( const TQWidgetList &wl, TQWidget *p, FormWindow *fw, TQWidget *lb, bool doSetup = true, bool splitter = false );

    void doLayout();

protected:
    void setup();

};

class VerticalLayout : public Layout
{
public:
    VerticalLayout( const TQWidgetList &wl, TQWidget *p, FormWindow *fw, TQWidget *lb, bool doSetup = true, bool splitter = false );

    void doLayout();

protected:
    void setup();

};

class Grid;

class GridLayout : public Layout
{
public:
    GridLayout( const TQWidgetList &wl, TQWidget *p, FormWindow *fw, TQWidget *lb, const TQSize &res, bool doSetup = true );
    ~GridLayout();

    void doLayout();

protected:
    void setup();

protected:
    void buildGrid();
    TQSize resolution;
    Grid* grid;

};

class Spacer : public TQWidget
{
    TQ_OBJECT

    TQ_OVERRIDE( TQCString name )
    TQ_PROPERTY( Orientation orientation READ orientation WRITE setOrientation )
    TQ_ENUMS( SizeType )
    TQ_PROPERTY( SizeType sizeType READ sizeType WRITE setSizeType )
    TQ_PROPERTY( TQSize sizeHint READ sizeHint WRITE setSizeHint DESIGNABLE true STORED true )
    TQ_OVERRIDE( TQRect geometry DESIGNABLE false )

private:
    enum { HSize = 6, HMask = 0x3f, VMask = HMask << HSize,
	   MayGrow = 1, ExpMask = 2, MayShrink = 4 };

public:
    enum SizeType { Fixed = 0,
		    Minimum = MayGrow,
		    Maximum = MayShrink,
		    Preferred = MayGrow|MayShrink ,
		    MinimumExpanding = Minimum|ExpMask,
		    Expanding = MinimumExpanding|MayShrink };

    Spacer( TQWidget *parent, const char *name );

    TQSize minimumSize() const;
    TQSize sizeHint() const;
    void setSizeType( SizeType t );
    SizeType sizeType() const;
    int alignment() const;
    Orientation orientation() const;
    void setOrientation( Orientation o );
    void setInteraciveMode( bool b ) { interactive = b; };
    void setSizeHint( const TQSize &s );

protected:
    void paintEvent( TQPaintEvent *e );
    void resizeEvent( TQResizeEvent* e );
    void updateMask();
    TQt::Orientation orient;
    bool interactive;
    TQSize sh;
};

class TQDesignerGridLayout : public TQGridLayout
{
   TQ_OBJECT
public:
    TQDesignerGridLayout( TQWidget *parent ) : TQGridLayout( parent ){};
    TQDesignerGridLayout( TQLayout *parentLayout ) : TQGridLayout( parentLayout ){};

    void addWidget( TQWidget *, int row, int col, int align = 0 );
    void addMultiCellWidget( TQWidget *, int fromRow, int toRow,
			       int fromCol, int toCol, int align = 0 );

    struct Item
    {
	Item(): row(0), column(0),rowspan(1),colspan(1){}
	Item( int r, int c, int rs, int cs): row(r), column(c), rowspan(rs), colspan(cs){}
	int row;
	int column;
	int rowspan;
	int colspan;
    };

    TQMap<TQWidget*, Item> items;
};


#endif
