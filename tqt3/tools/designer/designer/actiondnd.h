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

#ifndef ACTIONDND_H
#define ACTIONDND_H

#include <ntqaction.h>
#include <ntqdragobject.h>
#include <ntqmap.h>
#include <ntqmenubar.h>
#include <ntqpixmap.h>
#include <ntqpopupmenu.h>
#include <ntqptrlist.h>
#include <ntqtoolbar.h>
#include <ntqguardedptr.h>
#include "../shared/widgetdatabase.h"

//class TQDesignerPopupMenu;
class TQDesignerIndicatorWidget;
class FormWindow;
class TQPopupMenu;

class ActionDrag : public TQStoredDrag
{
    TQ_OBJECT

public:
    ActionDrag(const TQString &type, TQAction *action, TQWidget *source);
    ActionDrag(TQAction *action, TQWidget *source);
    ActionDrag(TQActionGroup *group, TQWidget *source);

    ~ActionDrag();

    static bool canDecode(TQDropEvent *e);

    static TQAction *action()
    {
	return the_action;
    }

private:
    static TQAction *the_action;
};

class TQDesignerActionGroup : public TQActionGroup
{
    TQ_OBJECT

public:
    TQDesignerActionGroup( TQObject *parent )
	: TQActionGroup( ::tqt_cast<TQActionGroup*>(parent) ? parent : 0 ), wid( 0 ), idx( -1 ) { init(); }

    void init();

    TQWidget *widget() const { return wid; }
    TQWidget *widget( TQAction *a ) const { return *widgets.find( a ); }
    int index() const { return idx; }

protected:
#if !defined(Q_NO_USING_KEYWORD)
    using TQActionGroup::addedTo;
#endif
    void addedTo( TQWidget *w, TQWidget * ) {
	wid = w;
    }
    void addedTo( TQWidget *w, TQWidget *, TQAction *a ) {
	widgets.insert( a, w );
    }
    void addedTo( int index, TQPopupMenu * ) {
	idx = index;
    }

private:
    TQWidget *wid;
    TQMap<TQAction *, TQWidget *> widgets;
    int idx;

};

class TQDesignerAction : public TQAction
{
    TQ_OBJECT

public:
    TQDesignerAction( TQObject *parent )
	: TQAction( ::tqt_cast<TQActionGroup*>(parent) ? parent : 0 ), wid( 0 ), idx( -1 ), widgetToInsert( 0 ) { init(); }
    TQDesignerAction( TQWidget *w, TQObject *parent )
	: TQAction( ::tqt_cast<TQActionGroup*>(parent) ? parent : 0 ), wid( 0 ), idx( -1 ), widgetToInsert( w ) { init(); }

    void init();

    TQWidget *widget() const { return wid; }
    int index() const { return idx; }

    bool addTo( TQWidget *w );
    bool removeFrom( TQWidget *w );

    void remove();
    bool supportsMenu() const { return !widgetToInsert; }

protected:
    void addedTo( TQWidget *w, TQWidget * ) {
	wid = w;
    }
    void addedTo( int index, TQPopupMenu * ) {
	idx = index;
    }

private:
    TQWidget *wid;
    int idx;
    TQWidget *widgetToInsert;

};

class TQDesignerToolBarSeparator : public TQWidget
{
    TQ_OBJECT

public:
    TQDesignerToolBarSeparator( Orientation, TQToolBar *parent, const char* name=0 );

    TQSize sizeHint() const;
    Orientation orientation() const { return orient; }
public slots:
   void setOrientation( Orientation );
protected:
    void styleChange( TQStyle& );
    void paintEvent( TQPaintEvent * );
private:
    Orientation orient;
};


class TQSeparatorAction : public TQAction
{
    TQ_OBJECT

public:
    TQSeparatorAction( TQObject *parent );

    bool addTo( TQWidget *w );
    bool removeFrom( TQWidget *w );
    TQWidget *widget() const;
    int index() const;

private:
    TQWidget *wid;
    int idx;

};


class TQDesignerToolBar : public TQToolBar
{
    TQ_OBJECT

public:
    TQDesignerToolBar( TQMainWindow *mw );
    TQDesignerToolBar( TQMainWindow *mw, Dock dock );
    TQPtrList<TQAction> insertedActions() const { return actionList; }
    void addAction( TQAction *a );

    void clear();
    void installEventFilters( TQWidget *w );
    void insertAction( TQWidget *w, TQAction *a ) { actionMap.insert( w, a ); }
    void insertAction( int index, TQAction *a ) { actionList.insert( index, a ); }
    void appendAction( TQAction *a ) { actionList.append( a ); }
    void removeAction( TQAction *a ) { actionList.remove( a ); }
    void reInsert();
    void removeWidget( TQWidget *w );

protected:
    bool eventFilter( TQObject *, TQEvent * );
    void paintEvent( TQPaintEvent * );
#ifndef TQT_NO_DRAGANDDROP
    void dragEnterEvent( TQDragEnterEvent * );
    void dragMoveEvent( TQDragMoveEvent * );
    void dragLeaveEvent( TQDragLeaveEvent * );
    void dropEvent( TQDropEvent * );
#endif
    void contextMenuEvent( TQContextMenuEvent *e );
    void mousePressEvent( TQMouseEvent *e );
    void mouseReleaseEvent( TQMouseEvent *e );

private slots:
    void actionRemoved();

private:
    void drawIndicator( const TQPoint &pos );
    TQPoint calcIndicatorPos( const TQPoint &pos );
    void buttonContextMenuEvent( TQContextMenuEvent *e, TQObject *o );
    void buttonMousePressEvent( TQMouseEvent *e, TQObject *o );
    void buttonMouseMoveEvent( TQMouseEvent *e, TQObject *o );
    void buttonMouseReleaseEvent( TQMouseEvent *e, TQObject *o );
    void doInsertWidget( const TQPoint &p );
    void findFormWindow();

private:
    TQPoint lastIndicatorPos;
    TQWidget *insertAnchor;
    bool afterAnchor;
    TQPtrList<TQAction> actionList;
    TQMap<TQWidget*, TQAction*> actionMap;
    TQPoint dragStartPos;
    TQDesignerIndicatorWidget *indicator;
    bool widgetInserting;
    FormWindow *formWindow;

};

#endif
