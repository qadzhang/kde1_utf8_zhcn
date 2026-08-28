/****************************************************************************
**
** Copyright (C) 1992-2008 Trolltech ASA.  All rights reserved.
**
** This file is part of an example program for TQt.  This example
** program may be used, distributed and modified without limitation.
**
*****************************************************************************/

#include "qdragapp.h"
#include "ntqptrlist.h"
#include "ntqintdict.h"
#include "ntqpopupmenu.h"
#include "ntqguardedptr.h"
#include "ntqcolor.h"
#include "ntqwidget.h"
#include "ntqfontmetrics.h"
#include "ntqcursor.h"
#include "ntqobjectlist.h"

TQWidget *cursorWidget( TQPoint * = 0 );

class TQDragger;


class DropWindow : public TQWidget
{
    TQ_OBJECT
public:
    void paintEvent( TQPaintEvent * );
    void closeEvent( TQCloseEvent * );

    TQDragger *master;
};


struct DropInfo {
    DropInfo()	{ w=0; }
   ~DropInfo()  { delete w; }
    DropWindow *w;
    bool userOpened;
};

struct DraggedInfo {
    TQWidget *w;
    TQWidget *mother;
    TQPoint   pos;
};


class TQDragger : public TQObject
{
    TQ_OBJECT
public:
    TQDragger();
    ~TQDragger();

    bool notify( TQObject *, TQEvent * ); // event filter
    void closeDropWindow( DropWindow * );
public slots:
    void openDropWindow();
    void killDropWindow();
    void killAllDropWindows();
    void sendChildHome();
    void sendAllChildrenHome();
private:
    bool isParentToDragged( TQWidget * );
    bool noWidgets( TQWidget * );
    void killDropWindow( DropInfo * );
    void killAllDropWindows( bool );
    void sendChildHome( DraggedInfo * );
    void sendAllChildrenHome( TQWidget * );
    TQWidget *openDropWindow( const TQRect&, bool );

    bool startGrab();
    void grabFinished();
    bool dragEvent( TQWidget *, TQMouseEvent * );
    bool killDropEvent( TQMouseEvent * );
    bool sendChildEvent( TQMouseEvent * );

    bool		   killingDrop;
    bool		   sendingChild;
    TQWidget		  *clickedWidget;
    TQGuardedPtr<TQWidget>   hostWidget;
    TQCursor		   cursor;

    TQPopupMenu*		   menu;
    TQPoint		   clickOffset;
    TQColor		   dragBackground;
    TQColor		   dragForeground;
    DraggedInfo		   dragInfo;
    TQIntDict<DraggedInfo>  draggedDict;
    TQIntDict<DropInfo>	   dropDict;
};


TQDragApplication::TQDragApplication( int &argc, char **argv )
    : TQApplication( argc, argv ), dragger( 0 )
{
    dragger = new TQDragger;
}

TQDragApplication::~TQDragApplication()
{
    delete dragger;
}

bool TQDragApplication::notify( TQObject *o, TQEvent *e )
{
    if ( dragger && !dragger->notify( o, e ) )
	return TQApplication::notify( o, e );
    else
	return false;
}

void DropWindow::paintEvent( TQPaintEvent * )
{
    const char *msg    = "Drag widgets and drop them here or anywhere!";
    int		startX = ( width() - fontMetrics().width( msg ) )/2;
    startX	       = startX < 0 ? 0 : startX;

    drawText( startX, height()/2, msg );
}

void DropWindow::closeEvent( TQCloseEvent *e )
{
    master->closeDropWindow( this );
    e->ignore();
}

TQDragger::TQDragger()
{
    dragInfo.w	 = 0;
    killingDrop	 = false;
    sendingChild = false;
    draggedDict.setAutoDelete( true );
    dropDict   .setAutoDelete( true );

    menu = new TQPopupMenu;
    menu->insertItem( "Open drop window", 1 );
    menu->insertItem( "Kill drop window", 2 );
    menu->insertItem( "Kill all drop windows", 3 );
    menu->insertSeparator();
//    menu->insertItem( "Send child home", 4 );
    menu->insertItem( "Send all children home", 5 );

    menu->connectItem( 1, this, TQ_SLOT(openDropWindow()) );
    menu->connectItem( 2, this, TQ_SLOT(killDropWindow()) );
    menu->connectItem( 3, this, TQ_SLOT(killAllDropWindows()) );
//    menu->connectItem( 4, this, TQ_SLOT(sendChildHome()) );
    menu->connectItem( 5, this, TQ_SLOT(sendAllChildrenHome()) );
}

TQDragger::~TQDragger()
{
    delete menu;
}


bool TQDragger::notify( TQObject *o, TQEvent *e )
{
    if ( !o->isWidgetType() || o == menu )
	return false;
    switch( e->type() ) {
	case TQEvent::MouseMove:
	     {
		 TQMouseEvent *tmp = (TQMouseEvent*) e;
		 if ( killingDrop )
		     return killDropEvent( tmp );
		 if ( sendingChild )
		     return sendChildEvent( tmp );
		 if ( tmp->state() & TQMouseEvent::RightButton )
		     return dragEvent( (TQWidget*) o, tmp );
		 break;
	     }
	case TQEvent::MouseButtonPress:
	case TQEvent::MouseButtonRelease:
	case TQEvent::MouseButtonDblClick:
	     {
		 TQMouseEvent *tmp = (TQMouseEvent*) e;
		 if ( killingDrop )
		     return killDropEvent( tmp );
		 if ( sendingChild )
		     return sendChildEvent( tmp );
		 if ( tmp->button() == TQMouseEvent::RightButton )
		     return dragEvent( (TQWidget*) o, tmp );
	     }
	     break;
	default:
	     break;
    }
    return false;
}

bool TQDragger::isParentToDragged( TQWidget *w )
{
    TQIntDictIterator<DraggedInfo> iter( draggedDict );

    DraggedInfo *tmp;
    while( (tmp = iter.current()) ) {
	++iter;
	if ( tmp->mother == w )
	    return true;
    }
    return false;
}

bool TQDragger::noWidgets( TQWidget *w )
{
    const TQObjectList *l = w->children();
    if ( !l )
	return true;
    TQObjectListIt iter( *l );
    TQObject *tmp;
    while( (tmp = iter.current()) ) {
	++iter;
	if ( tmp->isWidgetType() )
	    return false;
    }
    return true;
}

void TQDragger::sendAllChildrenHome( TQWidget *w )
{
    const TQObjectList *l = w->children();
    if ( !l )
	return;
    TQObjectListIt iter( *l );
    TQObject *tmp;
    while( (tmp = iter.current()) ) {
	++iter;
	if ( tmp->isWidgetType() ) {
	    sendAllChildrenHome( (TQWidget*) tmp );
	    DraggedInfo *di = draggedDict.find( (long) tmp );
	    if ( di )
		sendChildHome( di );
	}
    }
}

bool TQDragger::dragEvent( TQWidget *w, TQMouseEvent *e )
{
    switch( e->type() ) {
	case TQEvent::MouseButtonDblClick:
	case TQEvent::MouseButtonPress: {
	    if ( !noWidgets( w ) || // has widget children
		 isParentToDragged( w )	|| // has had widget children
		 w->parentWidget() == 0 ) {       // is top level window
		hostWidget = w;
		menu->popup( w->mapToGlobal( e->pos() ) );
		return true;
	    }
	    if ( !draggedDict.find( (long) w ) ) {
		DraggedInfo *tmp = new DraggedInfo;
		tmp->w	      = w;
		tmp->mother      = w->parentWidget();
		tmp->pos	      = w->frameGeometry().topLeft();
		draggedDict.insert( (long) w, tmp );
	    }
	    dragBackground	 = w->backgroundColor();
	    dragForeground	 = w->foregroundColor();
	    dragInfo.w	 = w;
	    dragInfo.mother = w->parentWidget();
	    dragInfo.pos	 = w->frameGeometry().topLeft();
	    clickOffset = e->pos();
	    dragInfo.w  = w;
	    TQPoint p    = w->mapToGlobal(TQPoint(0,0));
	    w->reparent( 0, WType_Popup, p, true );

	    return true;
	}
	case TQEvent::MouseButtonRelease:
	case TQEvent::MouseMove: {
	    if ( dragInfo.w != 0 ) {
		TQPoint p = TQCursor::pos() - clickOffset;
		dragInfo.w->move( p );
		if ( e->type() == TQEvent::MouseMove )
		    return true;
	    } else {
		return false;
	    }
	    if ( !dragInfo.w )
		return false;
	    if ( w != dragInfo.w )
		w = dragInfo.w;
	    dragInfo.w = 0;
	    w->hide();
	    TQPoint pos;
	    TQWidget *target = cursorWidget( &pos );
	    pos = pos - clickOffset;
	    TQPoint p;
	    if ( !target ) {
		target = openDropWindow( TQRect( pos, w->size() ),
					 false);
		p = TQPoint( 0, 0 );
	    }
	    else
		p = target->mapFromGlobal( pos );
	    w->reparent( target, 0, p, true );
	    DropInfo *tmp = dropDict.find( (long) dragInfo.mother );
	    if ( tmp ) {
		if ( !tmp->userOpened && noWidgets( tmp->w ) )
		    dropDict.remove( (long) tmp->w );
	    }
	    if ( !target->isVisible() )
		target->show();
	  }
	  return true;
	default:
	  return false;
    }
}

bool TQDragger::killDropEvent( TQMouseEvent *e )
{
    switch( e->type() ) {
	case TQEvent::MouseButtonDblClick:
	case TQEvent::MouseButtonPress:
	    clickedWidget = cursorWidget();
	    return true;
	case TQEvent::MouseButtonRelease:
	    hostWidget->releaseMouse();
	    if ( clickedWidget ) {
		DropInfo *tmp = dropDict.find( (long) clickedWidget );
		if( tmp ) {
		    killDropWindow( tmp );
		    dropDict.remove( (long) tmp->w );
		}
	    }
	    grabFinished();
	    return true;
	case TQEvent::MouseMove:
	    return true;
	default:
	    break;
    }
    return false;
}

bool TQDragger::sendChildEvent( TQMouseEvent *e )
{
    switch( e->type() ) {
	case TQEvent::MouseButtonDblClick:
	case TQEvent::MouseButtonPress:
	    clickedWidget = cursorWidget();
	    return true;
	case TQEvent::MouseButtonRelease:
	    hostWidget->releaseMouse();
	    if ( clickedWidget ) {
		DraggedInfo *tmp = draggedDict.find((long) clickedWidget);
		if( tmp ) {
		    TQWidget *parent = tmp->w->parentWidget();
		    sendChildHome( tmp );
		    DropInfo *dri = dropDict.find( (long) parent );
		    if ( dri && noWidgets(dri->w) && !dri->userOpened ) {
			killDropWindow( dri );
			dropDict.remove( (long) dri );
		    }
		}
		grabFinished();
	    }
	    return true;
	case TQEvent::MouseMove:
	    return true;
	default:
	    break;
    }
    return false;
}

bool TQDragger::startGrab()
{
    if ( !hostWidget )
	return false;
    clickedWidget = 0;
    cursor	  = hostWidget->cursor();
    hostWidget->grabMouse();
    hostWidget->setCursor( TQCursor( CrossCursor ) );
    return true;
}

void TQDragger::grabFinished()
{
    killingDrop	 = false;
    sendingChild = false;
    if(hostWidget)
	hostWidget->setCursor( cursor );
}

void TQDragger::closeDropWindow( DropWindow *w )
{
    DropInfo *tmp = dropDict.find( (long) w);
    if( tmp )
	killDropWindow( tmp );
}

void TQDragger::openDropWindow()
{
    TQWidget *tmp = openDropWindow( TQRect(100, 100, 300, 200), true );
    tmp->show();
}

TQWidget *TQDragger::openDropWindow( const TQRect &r, bool user )
{
    DropInfo *tmp = new DropInfo;
    DropWindow *w = new DropWindow;
    if ( user ) {
	tmp->userOpened = true;
	w->setCaption( "Drop window" );
    } else {
	tmp->userOpened = false;
	w->setCaption( "Auto drop window" );
    }
    tmp->w = w;
    w->master = this;
    w->setGeometry( r );
    dropDict.insert( (long) w, tmp );
    w->show();
    return w;
}

void TQDragger::killDropWindow()
{
    if ( startGrab() )
	killingDrop   = true;
}

void TQDragger::killDropWindow( DropInfo *di )
{
    const TQObjectList *l = di->w->children();
    if ( !l )
	return;
    TQObjectListIt iter( *l );
    TQObject *tmp;
    while( (tmp = iter.current()) ) {
	++iter;
	if ( tmp->isWidgetType() ) {
	    DraggedInfo *dri = draggedDict.find( (long) tmp );
	    if ( dri ) {
		sendChildHome( dri );
		draggedDict.remove( (long) tmp );
	    }
	}
    }
    di->w->hide();
}

void TQDragger::killAllDropWindows()
{
    killAllDropWindows( false );
}

void TQDragger::killAllDropWindows( bool autoOnly )
{
    TQIntDictIterator<DropInfo> iter( dropDict );

    DropInfo *tmp;
    while( (tmp = iter.current()) ) {
	++iter;
	if( !autoOnly || !tmp->userOpened ) {
	    killDropWindow( tmp );
	    dropDict.remove( (long) tmp->w );
	}
    }
}

void TQDragger::sendChildHome( DraggedInfo *i )
{
    i->w->reparent( i->mother, 0, i->pos, true );
}

void TQDragger::sendChildHome()
{
    if ( startGrab() )
	sendingChild  = true;
}

void TQDragger::sendAllChildrenHome()
{
    TQIntDictIterator<DraggedInfo> iter( draggedDict );

    DraggedInfo *tmp;
    while( (tmp = iter.current()) ) {
	++iter;
	sendChildHome( tmp );
	draggedDict.remove( (long) tmp->w );
    }
    killAllDropWindows( true );
    draggedDict.clear();
}


TQWidget *cursorWidget( TQPoint *p )
{
    TQPoint curpos = TQCursor::pos();
    if ( p )
	*p = curpos;
    return TQApplication::widgetAt( curpos );
}


#include "qdragapp.moc"
