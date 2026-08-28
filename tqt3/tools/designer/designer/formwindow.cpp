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

#include "formwindow.h"
#include "defs.h"
#include "mainwindow.h"
#include "widgetfactory.h"
#include "sizehandle.h"
#include "metadatabase.h"
#include "resource.h"
#include "layout.h"
#include "connectiondialog.h"
#include <widgetdatabase.h>
#include "pixmapchooser.h"
#include "orderindicator.h"
#include "hierarchyview.h"
#include "designerappiface.h"
#include "menubareditor.h"
#define NO_STATIC_COLORS
#include "globaldefs.h"

#include <stdlib.h>

#include <ntqevent.h>
#include <ntqpainter.h>
#include <ntqpen.h>
#include <ntqlabel.h>
#include <ntqobjectlist.h>
#include <ntqtimer.h>
#include <ntqapplication.h>
#include <ntqlayout.h>
#include <ntqspinbox.h>
#include <ntqstatusbar.h>
#include <ntqapplication.h>
#include <ntqpalette.h>
#include <ntqmessagebox.h>
#include <ntqpopupmenu.h>
#include <ntqsizegrip.h>
#include <ntqpushbutton.h>
#include <ntqwhatsthis.h>
#include <ntqmetaobject.h>
#include <ntqtooltip.h>
#include <ntqfeatures.h>
#include <ntqaccel.h>
#include <ntqpixmapcache.h>
#include <ntqbitmap.h>
#include <ntqsplitter.h>

// FormWindow should be able to work to some limited degree
// (existance, loading) without a MainWindow. Functions which require
// a MainWindow in theory should never be called if no MainWindow
// exists. These macros are used to let us know if that happens anyway
// and to ensure that we don't crash
#define CHECK_MAINWINDOW Q_ASSERT( mainWindow() ); if ( !mainWindow() ) return
#define CHECK_MAINWINDOW_VALUE( v ) Q_ASSERT( mainWindow() ); if ( !mainWindow() ) return v

static void setCursorToAll( const TQCursor &c, TQWidget *start )
{
    start->setCursor( c );
    TQObjectList *l = (TQObjectList*)start->children();
    if ( l ) {
	for ( TQObject *o = l->first(); o; o = l->next() ) {
	    if ( o->isWidgetType() && !::tqt_cast<SizeHandle*>(o) )
		setCursorToAll( c, ( (TQWidget*)o ) );
	}
    }
}

static void restoreCursors( TQWidget *start, FormWindow *fw )
{
    if ( fw->widgets()->find( start ) )
	start->setCursor( MetaDataBase::cursor( start ) );
    else
	start->setCursor( TQt::ArrowCursor );
    TQObjectList *l = (TQObjectList*)start->children();
    if ( l ) {
	for ( TQObject *o = l->first(); o; o = l->next() ) {
	    if ( o->isWidgetType() && !::tqt_cast<SizeHandle*>(o) )
		restoreCursors( ( (TQWidget*)o ), fw );
	}
    }
}

#if defined(TQ_WS_WIN32) // #### needed for the workaround for repaint problem on windows
#include <qt_windows.h>
static void flickerfree_update( TQWidget *w )
{
    InvalidateRect( w->winId(), 0, false );
}
#endif

/*!
  \class FormWindow formwindow.h
  \brief Editor window for a form

  The FormWindow is the widget which is used as editor for forms. It
  handles inserting, deleting, moving, resizing, etc. of widgets.

  Normally multiple formwindows are used at the same time in the
  Designer. So each formwindow has its own undo/redo buffer, etc.

  Also the formwindow has some signals to inform e.g. about selection
  changes which is interesting for the PropertyEditor.

  For handling the events of the child widgets (moving, etc.) the
  handleMousePress(), etc. functions are called from the application
  event filter which is implemented in MainWindow::eventFilter().
*/

FormWindow::FormWindow( FormFile *f, MainWindow *mw, TQWidget *parent, const char *name )
    : TQWidget( parent, name, WDestructiveClose ), mainwindow( mw ),
      commands( 100 ), pixInline( true ), pixProject( false )
{
    ff = f;
    init();
    initSlots();
}

FormWindow::FormWindow( FormFile *f, TQWidget *parent, const char *name )
    : TQWidget( parent, name, WDestructiveClose ), mainwindow( 0 ),
      commands( 100 ), pixInline( true )
{
    ff = f;
    init();
}

void FormWindow::init()
{
    fake = qstrcmp( name(), "qt_fakewindow" ) == 0;
    MetaDataBase::addEntry( this );
    ff->setFormWindow( this );
    iface = 0;
    proj = 0;
    propertyWidget = 0;
    toolFixed = false;
    checkedSelectionsForMove = false;
    mContainer = 0;
    startWidget = endWidget = 0;
    currTool = POINTER_TOOL;
    unclippedPainter = 0;
    widgetPressed = false;
    drawRubber = false;
    setFocusPolicy( ClickFocus );
    sizePreviewLabel = 0;
    checkSelectionsTimer = new TQTimer( this, "checkSelectionsTimer" );
    connect( checkSelectionsTimer, TQ_SIGNAL( timeout() ),
	     this, TQ_SLOT( invalidCheckedSelections() ) );
    updatePropertiesTimer = new TQTimer( this );
    connect( updatePropertiesTimer, TQ_SIGNAL( timeout() ),
	     this, TQ_SLOT( updatePropertiesTimerDone() ) );
    showPropertiesTimer = new TQTimer( this );
    connect( showPropertiesTimer, TQ_SIGNAL( timeout() ),
	     this, TQ_SLOT( showPropertiesTimerDone() ) );
    selectionChangedTimer = new TQTimer( this );
    connect( selectionChangedTimer, TQ_SIGNAL( timeout() ),
	     this, TQ_SLOT( selectionChangedTimerDone() ) );
    windowsRepaintWorkaroundTimer = new TQTimer( this );
    connect( windowsRepaintWorkaroundTimer, TQ_SIGNAL( timeout() ),
	     this, TQ_SLOT( windowsRepaintWorkaroundTimerTimeout() ) );
    insertParent = 0;
    connect( &commands, TQ_SIGNAL( undoRedoChanged( bool, bool, const TQString &, const TQString & ) ),
	     this, TQ_SIGNAL( undoRedoChanged( bool, bool, const TQString &, const TQString & ) ) );
    propShowBlocked = false;

    setIcon( TQPixmap::fromMimeSource( "designer_form.png" ) );

    connect( &commands, TQ_SIGNAL( modificationChanged( bool ) ),
	     this, TQ_SLOT( modificationChanged( bool ) ) );
    buffer = 0;

    TQWidget *w = WidgetFactory::create( WidgetDatabase::idFromClassName( "TQFrame" ), this );
    setMainContainer( w );
    propertyWidget = w;
    targetContainer = 0;
    hadOwnPalette = false;

    defSpacing = BOXLAYOUT_DEFAULT_SPACING;
    defMargin = BOXLAYOUT_DEFAULT_MARGIN;
    hasLayoutFunc = false;
}

void FormWindow::setMainWindow( MainWindow *w )
{
    mainwindow = w;
    MetaDataBase::addEntry( this );
    initSlots();
}

void FormWindow::initSlots()
{
    if ( isFake() )
	return;
    Q_ASSERT( project() || MainWindow::self );
    if ( !project() && !MainWindow::self )
	return;
    Project *p = project() ? project() : MainWindow::self->currProject();
    if ( p && p->isCpp() ) {
	TQString code = formFile()->code();
	if ( code.isEmpty() )
	    formFile()->setCode( formFile()->codeComment() );
    }
}

FormWindow::~FormWindow()
{
    if ( MainWindow::self && MainWindow::self->objectHierarchy()->formWindow() == this )
	MainWindow::self->objectHierarchy()->setFormWindow( 0, 0 );

    MetaDataBase::clear( this );
    if ( ff )
	ff->setFormWindow( 0 );
    delete iface;
}

void FormWindow::closeEvent( TQCloseEvent *e )
{
    TQGuardedPtr<FormWindow> that = this;
    if ( ff->closeEvent() && ( !that || ( mainwindow && mainwindow->unregisterClient( this ) ) ) )
	e->accept();
    else
	e->ignore();
}

void FormWindow::paintGrid( TQWidget *w, TQPaintEvent *e )
{
    if ( !mainWindow() || !mainWindow()->showGrid() )
	return;
    TQPixmap grid;
    TQString grid_name;
    grid_name.sprintf("FormWindowGrid_%d_%d", mainWindow()->grid().x(), mainWindow()->grid().y());
    if( !TQPixmapCache::find( grid_name, grid ) ) {
	grid = TQPixmap( 350 + ( 350 % mainWindow()->grid().x() ), 350 + ( 350 % mainWindow()->grid().y() ) );
	grid.fill( colorGroup().color( TQColorGroup::Foreground ) );
	TQBitmap mask( grid.width(), grid.height() );
	mask.fill( color0 );
	TQPainter p( &mask );
	p.setPen( color1 );
	for ( int y = 0; y < grid.width(); y += mainWindow()->grid().y()) {
	    for ( int x = 0; x < grid.height(); x += mainWindow()->grid().x() ) {
		p.drawPoint( x, y );
	    }
	}
	grid.setMask( mask );
	TQPixmapCache::insert( grid_name, grid );
    }
    TQPainter p( w );
    p.setClipRegion( e->rect() );
    p.drawTiledPixmap( TQRect( 0, 0, width(), height() ), grid );
}

/*!  For operations like drawing a rubber band or drawing the rect
  when inserting a new widget, a unclipped painter (which draws also
  on child widgets) is needed. This method does all the initialization.
*/

void FormWindow::beginUnclippedPainter( bool doNot )
{
    endUnclippedPainter();
    bool unclipped = testWFlags( WPaintUnclipped );
    setWFlags( WPaintUnclipped );
    unclippedPainter = new TQPainter;
    unclippedPainter->begin( this );
    if ( !unclipped )
	clearWFlags( WPaintUnclipped );
    if ( doNot ) {
	unclippedPainter->setPen( TQPen( color0, 2 ) );
	unclippedPainter->setRasterOp( NotROP );
    }
}

/*!
  Gets rid of an open unclipped painter.

  \sa beginUnclippedPainter()
*/

void FormWindow::endUnclippedPainter()
{
    if ( unclippedPainter )
	unclippedPainter->end();
    delete unclippedPainter;
    unclippedPainter = 0;
}

TQPoint FormWindow::gridPoint( const TQPoint &p )
{
    return TQPoint( ( p.x() / grid().x() ) * grid().x(),
		   ( p.y() / grid().y() ) * grid().y() );
}

void FormWindow::drawSizePreview( const TQPoint &pos, const TQString& text )
{
    unclippedPainter->save();
    unclippedPainter->setPen( TQPen( colorGroup().foreground(), 1  ));
    unclippedPainter->setRasterOp( CopyROP );
    if ( !sizePreviewPixmap.isNull() )
	unclippedPainter->drawPixmap( sizePreviewPos, sizePreviewPixmap );
    if ( text.isNull() ) {
	sizePreviewPixmap = TQPixmap(); // set null again
	unclippedPainter->restore();
	return;
    }
    TQRect r  =  fontMetrics().boundingRect( 0, 0, 0, 0, AlignCenter, text );
    r = TQRect( pos + TQPoint( 10, 10 ), r.size() + TQSize( 5, 5 ) );

    checkPreviewGeometry( r );

    sizePreviewPos = r.topLeft();
    sizePreviewPixmap = TQPixmap::grabWindow( winId(), r.x(), r.y(), r.width(), r.height() );
    unclippedPainter->setBrush( TQColor( 255, 255, 128 ) );
    unclippedPainter->drawRect( r );
    unclippedPainter->drawText( r, AlignCenter, text );
    unclippedPainter->restore();
}

void FormWindow::insertWidget()
{
    CHECK_MAINWINDOW;
    if ( !insertParent )
	return;

    if ( currTool == POINTER_TOOL )
	return;

    bool useSizeHint = !oldRectValid || ( currRect.width() < 2 && currRect.height() < 2 );
    Orientation orient = Horizontal;
    TQString n = WidgetDatabase::className( currTool );
    if (  useSizeHint && ( n == "Spacer" || n == "TQSlider" || n == "Line" || n == "TQScrollBar" ) ) {
	TQPopupMenu m( mainWindow() );
	m.insertItem( tr( "&Horizontal" ) );
	int ver = m.insertItem( tr( "&Vertical" ) );
	int r = m.exec( TQCursor::pos() );
	if ( r == ver )
	    orient = Vertical;
    }


    TQWidget *w = WidgetFactory::create( currTool, insertParent, 0, true, &currRect, orient );
    if ( !w )
	return;

    if ( !savePixmapInline() && currTool == WidgetDatabase::idFromClassName( "PixmapLabel" ) ) { // ### what to do for pixmaps in project
	TQPixmap pix;
	// we have to force the pixmap to get a new and unique serial number. Unfortunately detatch() doesn't do that
	pix.convertFromImage( TQPixmap::fromMimeSource( "designer_image.png" ).convertToImage() );
	( (TQLabel*)w )->setPixmap( pix );
    }
    int id = WidgetDatabase::idFromClassName( WidgetFactory::classNameOf(w) );
    if ( WidgetDatabase::isCustomWidget( id ) ) {
	TQWhatsThis::add( w, tr("<b>A %1 (custom widget)</b> "
			    "<p>Click <b>Edit Custom Widgets...</b> in the <b>Tools|Custom</b> "
			    "menu to add and change custom widgets. You can add "
			    "properties as well as signals and slots to integrate custom widgets into "
			    "<i>TQt Designer</i>, and provide a pixmap which will be used to represent "
			    "the widget on the form.</p>")
			    .arg(WidgetDatabase::toolTip( id )) );
	TQToolTip::add( w, tr("A %1 (custom widget)").arg(WidgetDatabase::toolTip( id )) );
    } else {
	TQString tt = WidgetDatabase::toolTip( id );
	TQString wt = WidgetDatabase::whatsThis( id );
	if ( !wt.isEmpty() && !tt.isEmpty() )
	    TQWhatsThis::add( w, TQString("<b>A %1</b><p>%2</p>").arg( tt ).arg( wt ) );
    }

    TQString s = w->name();
    unify( w, s, true );
    w->setName( s );
    insertWidget( w );
    TQRect r( currRect );
    if ( !oldRectValid ||
	 ( currRect.width() < 2 && currRect.height() < 2 ) )
	r = TQRect( rectAnchor, TQSize( 0, 0 ) );

    TQPoint p = r.topLeft();
    p = mapToGlobal( p );
    p = insertParent->mapFromGlobal( p );
    r = TQRect( p, r.size() );

    if ( useSizeHint ) {
	if ( n == "Spacer" ) {
	    if ( orient == Vertical ) {
		r.setWidth( 20 );
		r.setHeight( 40 );
	    } else {
		r.setWidth( 40 );
		r.setHeight( 20 );
	    }
	} else {
	    r.setWidth( w->sizeHint().width() );
	    r.setHeight( w->sizeHint().height() );
	}
    }

    if ( r.width() < 2 * grid().x() )
	r.setWidth( 2 * grid().x() );
    if ( r.height() < 2 * grid().y() )
	r.setHeight( 2 * grid().y() );

    const TQObjectList *l = insertParent->children();
    TQObjectListIt it( *l );
    TQWidgetList lst;
    if ( WidgetDatabase::isContainer( WidgetDatabase::idFromClassName( WidgetFactory::classNameOf( w ) ) ) ) {
	for ( ; it.current(); ) {
	    TQObject *o = it.current();
	    ++it;
	    if ( o->isWidgetType() &&
		 ( (TQWidget*)o )->isVisibleTo( this ) &&
		 insertedWidgets.find( (TQWidget*)o ) && o != w ) {
		TQRect r2( ( (TQWidget*)o )->pos(),
			  ( (TQWidget*)o )->size() );
		if ( r.contains( r2 ) )
		    lst.append( (TQWidget*)o );
	    }
	}
    }

    if ( !lst.isEmpty() ) {
	TQWidget *pw = WidgetFactory::containerOfWidget( w );
        if (pw) {
            TQValueList<TQPoint> op, np;
            for ( TQWidget *i = lst.first(); i; i = lst.next() ) {
                op.append( i->pos() );
                TQPoint pos = pw->mapFromGlobal( i->mapToGlobal( TQPoint( 0, 0 ) ) );
                pos -= r.topLeft();
                np.append( pos );
            }
            
            MoveCommand *mv = new MoveCommand( tr( "Reparent Widgets" ), this,
                                               lst, op, np, insertParent, pw );

            if ( !toolFixed )
                mainwindow->resetTool();
            else
                setCursorToAll( CrossCursor, w );
            
            InsertCommand *cmd = new InsertCommand( tr( "Insert %1" ).arg( w->name() ), this, w, r );
            
            TQPtrList<Command> commands;
            commands.append( mv );
            commands.append( cmd );
            
            MacroCommand *mc = new MacroCommand( tr( "Insert %1" ).arg( w->name() ), this, commands );
            commandHistory()->addCommand( mc );
            mc->execute();
        }
    } else {
	if ( !toolFixed )
	    mainwindow->resetTool();
	else
	    setCursorToAll( CrossCursor, w );

	InsertCommand *cmd = new InsertCommand( tr( "Insert %1" ).arg( w->name() ), this, w, r );
	commandHistory()->addCommand( cmd );
	cmd->execute();
    }

    TemplateWizardInterface *iface = mainWindow()->templateWizardInterface( w->className() );
    if ( iface ) {
	iface->setup( w->className(), w, iFace(), mainWindow()->designerInterface() );
	iface->release();
    }
}

void FormWindow::insertWidget( TQWidget *w, bool checkName )
{
    if ( !w )
	return;
    if ( checkName ) {
	TQString s = w->name();
	unify( w, s, true );
	w->setName( s );
    }

    MetaDataBase::addEntry( w );
    int id = WidgetDatabase::idFromClassName( WidgetFactory::classNameOf(w) );
    if ( WidgetDatabase::isCustomWidget( id ) ) {
	TQWhatsThis::add( w, tr("<b>A %1 (custom widget)</b> "
			    "<p>Click <b>Edit Custom Widgets...</b> in the <b>Tools|Custom</b> "
			    "menu to add and change custom widgets. You can add "
			    "properties as well as signals and slots to integrate custom widgets into "
			    "<i>TQt Designer</i>, and provide a pixmap which will be used to represent "
			    "the widget on the form.</p>")
			    .arg(WidgetDatabase::toolTip( id )) );
	TQToolTip::add( w, tr("A %1 (custom widget)").arg(WidgetDatabase::toolTip( id )) );
    } else {
	TQString tt = WidgetDatabase::toolTip( id );
	TQString wt = WidgetDatabase::whatsThis( id );
	if ( !wt.isEmpty() && !tt.isEmpty() )
	    TQWhatsThis::add( w, TQString("<b>A %1</b><p>%2</p>").arg( tt ).arg( wt ) );
    }

    restoreCursors( w, this );
    widgets()->insert( w, w );
    w->show();
}

void FormWindow::removeWidget( TQWidget *w )
{
    MetaDataBase::removeEntry( w );
    widgets()->take( w );
}

void FormWindow::handleContextMenu( TQContextMenuEvent *e, TQWidget *w )
{
    CHECK_MAINWINDOW;
    switch ( currTool ) {
    case POINTER_TOOL: {
	if ( !isMainContainer( w ) && qstrcmp( w->name(), "central widget" ) != 0 ) { // press on a child widget
	    raiseChildSelections( w ); // raise selections and select widget
	    selectWidget( w );
	    // if widget is laid out, find the first non-laid out super-widget
	    TQWidget *realWidget = w; // but store the original one
	    while ( w->parentWidget() &&
		    ( WidgetFactory::layoutType( w->parentWidget()) != WidgetFactory::NoLayout ||
		      !insertedWidgets.find(w) ) )
		w = w->parentWidget();
	    if ( ::tqt_cast<TQMainWindow*>(mainContainer()) && ((TQMainWindow*)mainContainer())->centralWidget() == realWidget ) {
		e->accept();
		mainwindow->popupFormWindowMenu( e->globalPos(), this );
	    } else {
		e->accept();
		mainwindow->popupWidgetMenu( e->globalPos(), this, realWidget);
	    }
	} else {
	    e->accept();
	    clearSelection();
	    mainwindow->popupFormWindowMenu( e->globalPos(), this );
	}
	break; }
    default:
	break;
    }
}

void FormWindow::handleMousePress( TQMouseEvent *e, TQWidget *w )
{
    CHECK_MAINWINDOW;
    checkedSelectionsForMove = false;
    checkSelectionsTimer->stop();
    if ( !sizePreviewLabel ) {
	sizePreviewLabel = new TQLabel( this );
	sizePreviewLabel->hide();
	sizePreviewLabel->setBackgroundColor( TQColor( 255, 255, 128 ) );
	sizePreviewLabel->setFrameStyle( TQFrame::Plain | TQFrame::Box );
    }

    switch ( currTool ) {
    case POINTER_TOOL:
	if ( !isMainContainer( w ) && qstrcmp( w->name(), "central widget" ) != 0 ) { // press on a child widget
	    // if the clicked widget is not in a layout, raise it
	    if ( !w->parentWidget() || WidgetFactory::layoutType( w->parentWidget() ) == WidgetFactory::NoLayout )
		w->raise();
	    if ( ( e->state() & ControlButton ) ) { // with control pressed, always start rubber band selection
		drawRubber = true;
		currRect = TQRect( 0, 0, -1, -1 );
		startRectDraw( mapFromGlobal( e->globalPos() ), e->globalPos(), this, Rubber );
		break;
	    }

	    bool sel = isWidgetSelected( w );
	    if ( !( ( e->state() & ControlButton ) || ( e->state() & ShiftButton ) ) ) { // control not pressed...
		if ( !sel ) { // ...and widget no selectted: unselect all
		    clearSelection( false );
		} else { // ...widget selected
		    // only if widget has a layout (it is a layout meta widget or a laid out container!), unselect its childs
		    if ( WidgetFactory::layoutType( w ) != WidgetFactory::NoLayout ) {
			TQObjectList *l = w->queryList( "TQWidget" );
			setPropertyShowingBlocked( true );
			for ( TQObject *o = l->first(); o; o = l->next() ) {
			    if ( !o->isWidgetType() )
				continue;
			    if ( insertedWidgets.find( (TQWidget*)o ) )
				selectWidget( (TQWidget*)o, false );
			}
			setPropertyShowingBlocked( false );
			delete l;
		    }
		}
		tqApp->processEvents();
	    }
	    if ( ( ( e->state() & ControlButton ) || ( e->state() & ShiftButton ) ) &&
		 sel && e->button() == LeftButton ) { // control pressed and selected, unselect widget
		selectWidget( w, false );
		break;
	    }

	    raiseChildSelections( w ); // raise selections and select widget
	    selectWidget( w );

	    // if widget is laid out, find the first non-laid out super-widget
	    while ( w->parentWidget() &&
		    ( WidgetFactory::layoutType( w->parentWidget()) != WidgetFactory::NoLayout || !insertedWidgets.find(w) ) )
		w = w->parentWidget();

	    if ( e->button() == LeftButton ) { // left button: store original geometry and more as the widget might start moving
		widgetPressed = true;
		widgetGeom = TQRect( w->pos(), w->size() );
		oldPressPos = w->mapFromGlobal( e->globalPos() );
		origPressPos = oldPressPos;
		checkedSelectionsForMove = false;
		moving.clear();
		if ( w->parentWidget() && !isMainContainer( w->parentWidget() ) && !isCentralWidget( w->parentWidget() ) ) {
		    targetContainer = w->parentWidget();
		    hadOwnPalette = w->parentWidget()->ownPalette();
		    restorePalette = w->parentWidget()->palette();
		}
	    }
	} else { // press was on the formwindow
	    if ( e->button() == LeftButton ) { // left button: start rubber selection and show formwindow properties
		drawRubber = true;
		if ( !( ( e->state() & ControlButton ) || ( e->state() & ShiftButton ) ) ) {
		    clearSelection( false );
		    TQObject *opw = propertyWidget;
		    propertyWidget = mainContainer();
		    if ( opw->isWidgetType() )
			repaintSelection( (TQWidget*)opw );
		}
		currRect = TQRect( 0, 0, -1, -1 );
		startRectDraw( mapFromGlobal( e->globalPos() ), e->globalPos(), this, Rubber );
	    }
	}
	break;
    case CONNECT_TOOL:
    case BUDDY_TOOL:
	if ( e->button() != LeftButton )
	    break;
	validForBuddy = false;
	if ( currTool == BUDDY_TOOL ) {
	    if ( !::tqt_cast<TQLabel*>(w) )
		break;
	    clearSelection( false );
	    validForBuddy = true;
	    mainWindow()->statusBar()->message( tr( "Set buddy for '%1' to..." ).arg( w->name() ) );
	} else {
	    mainWindow()->statusBar()->message( tr( "Connect '%1' with..." ).arg( w->name() ) );
	}
	saveBackground();
	startPos = mapFromGlobal( e->globalPos() );
	currentPos = startPos;
	startWidget = designerWidget( w );
	endWidget = startWidget;
	beginUnclippedPainter( false );
	drawConnectionLine();
	break;
    case ORDER_TOOL:
	if ( !isMainContainer( w ) ) { // press on a child widget
	    orderedWidgets.removeRef( w );
	    orderedWidgets.append( w );
	    for ( TQWidget *wid = orderedWidgets.last(); wid; wid = orderedWidgets.prev() ) {
		int i = stackedWidgets.findRef( wid );
		if ( i != -1 ) {
		    stackedWidgets.removeRef( wid );
		    stackedWidgets.insert( 0, wid );
		}
	    }
	    TQWidgetList oldl = MetaDataBase::tabOrder( this );
	    TabOrderCommand *cmd = new TabOrderCommand( tr( "Change Tab Order" ), this, oldl, stackedWidgets );
	    cmd->execute();
	    commandHistory()->addCommand( cmd, true );
	    updateOrderIndicators();
	}
	break;
    default: // any insert widget tool
	if ( e->button() == LeftButton ) {
	    insertParent = WidgetFactory::containerOfWidget( mainContainer() ); // default parent for new widget is the formwindow
	    if ( !isMainContainer( w ) ) { // press was not on formwindow, check if we can find another parent
		TQWidget *wid = w;
		for (;;) {
		    int id = WidgetDatabase::idFromClassName( WidgetFactory::classNameOf( wid ) );
		    if ( ( WidgetDatabase::isContainer( id ) || wid == mainContainer() ) &&
			 !::tqt_cast<TQLayoutWidget*>(wid) && !::tqt_cast<TQSplitter*>(wid) ) {
			insertParent = WidgetFactory::containerOfWidget( wid ); // found another parent, store it
			break;
		    } else {
			wid = wid->parentWidget();
			if ( !wid )
			    break;
		    }
		}
	    }
	    startRectDraw( w->mapFromGlobal( e->globalPos() ), e->globalPos(), w, Insert );
	}
	break;
    }
}

void FormWindow::handleMouseDblClick( TQMouseEvent *, TQWidget *w )
{
    CHECK_MAINWINDOW;
    switch ( currTool ) {
    case ORDER_TOOL:
	if ( !isMainContainer( w ) ) { // press on a child widget
	    orderedWidgets.clear();
	    orderedWidgets.append( w );
	    for ( TQWidget *wid = orderedWidgets.last(); wid; wid = orderedWidgets.prev() ) {
		int i = stackedWidgets.findRef( wid );
		if ( i != -1 ) {
		    stackedWidgets.removeRef( wid );
		    stackedWidgets.insert( 0, wid );
		}
	    }
	    TQWidgetList oldl = MetaDataBase::tabOrder( this );
	    TabOrderCommand *cmd = new TabOrderCommand( tr( "Change Tab Order" ), this, oldl, stackedWidgets );
	    cmd->execute();
	    commandHistory()->addCommand( cmd, true );
	    updateOrderIndicators();
	}
    default:
	if ( !WidgetFactory::isPassiveInteractor( w ) &&
	     ( isMainContainer( w ) || w == this ) )
	    mainWindow()->editSource();
	break;
    }
}

void FormWindow::handleMouseMove( TQMouseEvent *e, TQWidget *w )
{
    CHECK_MAINWINDOW;
    if ( ( e->state() & LeftButton ) != LeftButton )
	return;

    TQWidget *newendWidget = endWidget, *oldendWidget = endWidget, *wid;
    bool drawRecRect;
    switch ( currTool ) {
    case POINTER_TOOL:
	if ( widgetPressed && allowMove( w ) ) { // we are prepated for a move

	    // if widget is laid out, find the first non-laid out super-widget
	    while ( w->parentWidget() &&
		    ( WidgetFactory::layoutType( w->parentWidget() ) != WidgetFactory::NoLayout || !insertedWidgets.find(w ) ) )
		w = w->parentWidget();

	    // calc correct position
	    TQPoint pos = w->mapFromGlobal( e->globalPos() );

#if 0
	    TQPoint mpos = w->mapToParent( pos );
	    // check if we are not outside the visible area, else correct coords
	    if ( mpos.x() < 0 )
		pos.setX( w->mapFromParent( TQPoint( 0, mpos.y() ) ).x() );
	    if ( mpos.y() < 0 )
		pos.setY( w->mapFromParent( TQPoint( mpos.x(), 0 ) ).y() );
	    if ( mpos.x() > w->parentWidget()->width() )
		pos.setX( w->mapFromParent( TQPoint( w->parentWidget()->width(), mpos.y() ) ).x() );
	    if ( mpos.y() > w->parentWidget()->height() )
		pos.setY( w->mapFromParent( TQPoint( mpos.x(), w->parentWidget()->height() ) ).y() );
#endif

	    // calc move distance and store it
	    TQPoint d = oldPressPos - pos;
	    if ( TQABS( d.x() ) < grid().x() )
		d.setX( 0 );
	    if ( TQABS( d.y() ) < grid().y() )
		d.setY( 0 );
	    if ( d.x() == 0 )
		pos.setX( oldPressPos.x() );
	    if ( d.y() == 0 )
		pos.setY( oldPressPos.y() );
	    oldPressPos = pos;

	    // snap to grid
	    int x = widgetGeom.x() - d.x();
	    widgetGeom.setX( x );
	    x = ( x / grid().x() ) * grid().x();
	    int y = widgetGeom.y() - d.y();
	    widgetGeom.setY( y );
	    y = ( y / grid().y() ) * grid().y();
	    TQPoint p = w->pos();

	    if ( x - p.x() != 0 || y - p.y() != 0 ) { // if we actually have to move
		if ( !checkedSelectionsForMove ) { // if not checked yet, check if the correct widget are selected...
		    if ( !isWidgetSelected( w ) ) {	// and unselect others. Only siblings can be moved at the same time
			setPropertyShowingBlocked( true );
			selectWidget( w );
			setPropertyShowingBlocked( false );
		    }
		    checkSelectionsForMove( w );
		}

		// check whether we would have to reparent the selection and highlight the possible new parent container
		TQMapConstIterator<TQWidget*, TQPoint> it = moving.begin();
		TQWidget* wa = containerAt( e->globalPos(), it.key() );
		if ( wa  && !isMainContainer( wa ) && !isCentralWidget( wa ) ) {
		    wa = WidgetFactory::containerOfWidget( wa );
		    // ok, looks like we moved onto a container

		    if ( wa != targetContainer ) {
			if ( targetContainer ) {
			    if ( hadOwnPalette )
				targetContainer->setPalette( restorePalette );
			    else
				targetContainer->unsetPalette();
			}
			targetContainer = wa;
			hadOwnPalette = wa->ownPalette();
			restorePalette = wa->palette();
			wa->setPaletteBackgroundColor( wa->colorGroup().midlight() );
		    }
		}
		else if ( targetContainer ) {
		    if( hadOwnPalette )
			targetContainer->setPalette( restorePalette );
		    else
			targetContainer->unsetPalette();
		    targetContainer = 0;
		}

		// finally move the selected widgets and show/update preview label
		moveSelectedWidgets( x - p.x(), y - p.y() );
		sizePreviewLabel->setText( tr( "%1/%2" ).arg( w->pos().x() ).arg( w->pos().y() ) );
		sizePreviewLabel->adjustSize();
		TQRect lg( mapFromGlobal( e->globalPos() ) + TQPoint( 16, 16 ), sizePreviewLabel->size() );
		checkPreviewGeometry( lg );
		sizePreviewLabel->setGeometry( lg );
		sizePreviewLabel->raise();
		sizePreviewLabel->show();
#if defined(TQ_WS_WIN32)
		windowsRepaintWorkaroundTimer->start( 100, true );
#endif
	    } else { // if we don't need to move, do some indication
		TQRect lg( mapFromGlobal( e->globalPos() ) + TQPoint( 16, 16 ), sizePreviewLabel->size() );
		checkPreviewGeometry( lg );
		sizePreviewLabel->move( lg.x(), lg.y() );
	    }

	    oldPressPos += ( p - w->pos() );
	} else if ( drawRubber ) { // draw rubber if we are in rubber-selection mode
	    continueRectDraw( mapFromGlobal( e->globalPos() ), e->globalPos(), this, Rubber );
	}
	break;
    case CONNECT_TOOL:
	restoreConnectionLine();
	wid = tqApp->widgetAt( e->globalPos(), true );
	if ( wid )
	    wid = designerWidget( wid );
	if ( wid && ( isMainContainer( wid ) || insertedWidgets.find( wid ) ) && wid->isVisibleTo( this ) )
	    newendWidget = wid;
	if ( ::tqt_cast<TQLayoutWidget*>(newendWidget) || ::tqt_cast<Spacer*>(newendWidget) )
	    newendWidget = (TQWidget*)endWidget;
	drawRecRect = newendWidget != endWidget;
	if ( newendWidget &&
	     ( isMainContainer( newendWidget ) || insertedWidgets.find( newendWidget ) ) && !isCentralWidget( newendWidget ) )
	    endWidget = newendWidget;
	mainWindow()->statusBar()->message( tr( "Connect '%1' to '%2'" ).arg( startWidget->name() ).
					    arg( endWidget->name() ) );
	currentPos = mapFromGlobal( e->globalPos() );
	tqApp->processEvents();
	if ( drawRecRect )
	    restoreRect( TQRect( mapToForm( ( (TQWidget*)oldendWidget )->parentWidget(), ( (TQWidget*)oldendWidget )->pos() ),
			 ( (TQWidget*)oldendWidget )->size() ) );
	drawConnectionLine();
	break;
    case BUDDY_TOOL:
	if ( !validForBuddy )
	    break;
	restoreConnectionLine();
	wid = tqApp->widgetAt( e->globalPos(), true );
	if ( wid )
	    wid = designerWidget( wid );
	if ( wid && canBeBuddy( wid ) && wid->isVisibleTo( this ) )
	    newendWidget = wid;
	else
	    newendWidget = 0;
	if ( ::tqt_cast<TQLayoutWidget*>(newendWidget) || ::tqt_cast<Spacer*>(newendWidget) )
	    newendWidget = (TQWidget*)endWidget;
	drawRecRect = newendWidget != endWidget;
	if ( !newendWidget )
	    endWidget = newendWidget;
	else if ( insertedWidgets.find( newendWidget ) && !isCentralWidget( newendWidget ) )
	    endWidget = newendWidget;
	if ( endWidget )
	    mainWindow()->statusBar()->message( tr( "Set buddy '%1' to '%2'" ).arg( startWidget->name() ).
						arg( endWidget->name() ) );
	else
	    mainWindow()->statusBar()->message( tr( "Set buddy '%1' to ..." ).arg( startWidget->name() ) );
	currentPos = mapFromGlobal( e->globalPos() );
	tqApp->processEvents();
	if ( drawRecRect && oldendWidget )
	    restoreRect( TQRect( mapToForm( ( (TQWidget*)oldendWidget )->parentWidget(), ( (TQWidget*)oldendWidget )->pos() ),
			 ( (TQWidget*)oldendWidget )->size() ) );
	drawConnectionLine();
	break;
    case ORDER_TOOL:
	break;
    default: // we are in an insert-widget tool
	if ( insertParent ) // draw insert rect
	    continueRectDraw( w->mapFromGlobal( e->globalPos() ), e->globalPos(), w, Insert );
	break;
    }
}

void FormWindow::handleMouseRelease( TQMouseEvent *e, TQWidget *w )
{
    CHECK_MAINWINDOW;
    if ( e->button() != LeftButton )
	return;

    switch ( currTool ) {
    case POINTER_TOOL:
	if ( widgetPressed && allowMove( w ) ) { // we moved the widget
	    sizePreviewLabel->hide();

	    if ( moving.isEmpty() || w->pos() == *moving.find(w) )
		break;

	    // restore targetContainer
	    if ( targetContainer ) {
		if( hadOwnPalette )
		    targetContainer->setPalette( restorePalette );
		else
		    targetContainer->unsetPalette();
	    }

	    // tell property editor to update
	    if ( propertyWidget && propertyWidget->isWidgetType() && !isMainContainer( propertyWidget ) )
		emitUpdateProperties( propertyWidget );

	    TQMapConstIterator<TQWidget*,TQPoint> it = moving.begin();
	    TQWidget *oldParent = it.key()->parentWidget();
	    TQWidget *newParent = oldParent;
	    // check whether we have to reparent the selection
	    TQWidget* wa = containerAt( e->globalPos(), it.key() );
	    if ( wa ) {
		wa = WidgetFactory::containerOfWidget( wa );
		// ok, looks like we moved onto a container

		// check whether we really have different parents.
		if ( wa == it.key()->parentWidget() )
		    goto make_move_command;

		// break layout if necessary
		if ( WidgetFactory::layoutType( wa ) != WidgetFactory::NoLayout ) {
		    if ( TQMessageBox::information( mainWindow(), tr( "Inserting a Widget" ),
						   tr( "You tried to insert a widget into the "
						       "layout Container Widget '%1'.\n"
						       "This is not possible. "
						       "In order to insert the widget, the layout of '%1'\n"
						       "must first be broken.\n"
						       "Break the layout or cancel the operation?" ).
						   arg( wa->name() ).
						   arg( wa->name() ), tr( "&Break Layout" ), tr( "&Cancel" ) ) )
			goto make_move_command; // cancel
		    breakLayout( wa );
		}

		// doesn't need to be a command, the MoveCommand does reparenting too
		bool emitSelChanged = false;
		for ( TQMap<TQWidget*, TQPoint>::Iterator it = moving.begin(); it != moving.end(); ++it ) {
		    TQWidget *i = it.key();
		    if ( !emitSelChanged && ::tqt_cast<TQButton*>(i) ) {
			if ( ::tqt_cast<TQButtonGroup*>(i->parentWidget()) || ::tqt_cast<TQButtonGroup*>(wa) )
			    emitSelChanged = true;
			if ( !::tqt_cast<TQButtonGroup*>(wa) ) {
			    MetaDataBase::setPropertyChanged( i, "buttonGroupId", false );
			    if ( ::tqt_cast<TQButtonGroup*>(i->parentWidget()) )
				( (TQButtonGroup*)i->parentWidget() )->remove( (TQButton*)i );
			}
		    }
		    TQPoint pos = wa->mapFromGlobal( i->mapToGlobal( TQPoint(0,0) ) );
		    i->reparent( wa, pos, true );
		    raiseSelection( i );
		    raiseChildSelections( i );
		    widgetChanged( i );
		    mainWindow()->objectHierarchy()->widgetRemoved( i );
		    mainWindow()->objectHierarchy()->widgetInserted( i );
		}
		if ( emitSelChanged ) {
		    emit showProperties( wa );
		    emit showProperties( propertyWidget );
		}
		newParent = wa;
	    }

	make_move_command:
	    TQWidgetList widgets; // collect the widgets and its old and new positions which have been moved
	    TQValueList<TQPoint> oldPos, newPos;
	    for ( it = moving.begin(); it != moving.end(); ++it ) {
		widgets.append( it.key() );
		oldPos.append( *it );
		newPos.append( it.key()->pos() );
	    }
	    // add move command, don't execute it, this is just a summary of the operations we did during the move-event handling
	    commandHistory()->addCommand( new MoveCommand( tr( "Move" ),
							   this, widgets,
							   oldPos, newPos, oldParent, newParent ) );
	} else if ( drawRubber ) { // we were drawing a rubber selection
	    endRectDraw(); // get rid of the rectangle
	    blockSignals( true );
	    selectWidgets(); // select widgets which intersect the rect
	    blockSignals( false );
	    emitSelectionChanged(); // inform about selection changes
	    if ( propertyWidget )
		emitShowProperties( propertyWidget );
	}
	break;
    case CONNECT_TOOL:
    case BUDDY_TOOL:
	restoreConnectionLine();
	if ( startWidget )
	    restoreRect( TQRect( mapToForm( ( (TQWidget*)startWidget )->parentWidget(),
					   ( (TQWidget*)startWidget )->pos() ),
				((TQWidget*)startWidget )->size() ) );
	if ( endWidget )
	    restoreRect( TQRect( mapToForm( ( (TQWidget*)endWidget )->parentWidget(),
					   ( (TQWidget*)endWidget )->pos() ),
				( (TQWidget*)endWidget )->size() ) );
	endUnclippedPainter();
	tqApp->processEvents();

	if ( startWidget && endWidget ) {
	    if ( currTool == CONNECT_TOOL )
		editConnections();
	    else if ( currTool == BUDDY_TOOL && validForBuddy && startWidget != endWidget ) {
		TQString oldBuddy = startWidget->property( "buddy" ).toString();
		if ( oldBuddy.isNull() )
		    oldBuddy = "";
		SetPropertyCommand *cmd = new SetPropertyCommand( tr( "Set buddy for " + TQString( startWidget->name() ) ),
								  this, startWidget, mainWindow()->propertyeditor(),
								  "buddy", startWidget->property( "buddy" ),
								  endWidget->name(), endWidget->name(),
								  oldBuddy );
		commandHistory()->addCommand( cmd, true );
		cmd->execute();
		emitUpdateProperties( startWidget );
	    }
	}
	if ( !toolFixed )
	    mainwindow->resetTool();
	startWidget = endWidget = 0;
	mainWindow()->statusBar()->clear();
	break;
    case ORDER_TOOL:
	break;
    default: // any insert widget tool is active
	if ( insertParent ) { // we should insert the new widget now
	    endRectDraw();
	    if ( WidgetFactory::layoutType( insertParent ) != WidgetFactory::NoLayout ) {
		if ( TQMessageBox::information( mainWindow(), tr( "Inserting a Widget" ),
					       tr( "You tried to insert a widget into the "
						   "layout Container Widget '%1'.\n"
						   "This is not possible. "
						   "In order to insert the widget, the layout of '%1'\n"
						   "must first be broken.\n"
						   "Break the layout or cancel the operation?" ).
					       arg( insertParent->name() ).
					       arg( insertParent->name() ), tr( "&Break Layout" ), tr( "&Cancel" ) ) == 0 ) {
		    breakLayout( insertParent );
		} else {
		    if ( !toolFixed )
			mainWindow()->resetTool();
		    break;
		}
	    }
	    insertWidget(); // so do it
	}
	break;
    }
    widgetPressed = false;
    drawRubber = false;
    insertParent = 0;
    delete buffer;
    buffer = 0;
}

void FormWindow::handleKeyPress( TQKeyEvent *e, TQWidget *w )
{
    CHECK_MAINWINDOW;
    e->ignore();
    checkSelectionsTimer->stop();
    if ( !checkedSelectionsForMove &&
	 ( e->key() == Key_Left ||
	   e->key() == Key_Right ||
	   e->key() == Key_Up ||
	   e->key() == Key_Down ) &&
	 propertyWidget->isWidgetType() )
	checkSelectionsForMove( (TQWidget*)propertyWidget );
    checkSelectionsTimer->start( 1000, true );
    if ( e->key() == Key_Left || e->key() == Key_Right ||
	 e->key() == Key_Up || e->key() == Key_Down ) {
	TQWidgetList widgets;
	TQValueList<TQPoint> oldPos, newPos;
	for ( WidgetSelection *s = selections.first(); s; s = selections.next() ) {
	    if ( s->isUsed() ) {
		int dx = 0, dy = 0;
		bool control = e->state() & ControlButton;

		switch ( e->key() ) {
		case Key_Left: {
		    e->accept();
		    if ( control )
			dx = -1;
		    else
			dx = -grid().x();
		} break;
		case Key_Right: {
		    e->accept();
		    if ( control )
			dx = 1;
		    else
			dx = grid().x();
		} break;
		case Key_Up: {
		    e->accept();
		    if ( control )
			dy = -1;
		    else
			dy = -grid().y();
		} break;
		case Key_Down: {
		    e->accept();
		    if ( control )
			dy = 1;
		    else
			dy = grid().y();
		} break;
		default:
		    break;
		}

		widgets.append( s->widget() );
		oldPos.append( s->widget()->pos() );
		newPos.append( s->widget()->pos() + TQPoint( dx, dy ) );
	    }
	}
	if ( !widgets.isEmpty() ) {
	    MoveCommand *cmd = new MoveCommand( tr( "Move" ), this,
						widgets, oldPos, newPos, 0, 0 );
	    commandHistory()->addCommand( cmd, true );
	    cmd->execute();
	}
    }
    if ( !e->isAccepted() ) {
	TQObjectList *l = queryList( "TQWidget" );
	if ( !l )
	    return;
	if ( l->find( w ) != -1 )
	    e->accept();
	delete l;
    }

}

void FormWindow::handleKeyRelease( TQKeyEvent *e, TQWidget * )
{
    e->ignore();
}

void FormWindow::selectWidget( TQObject *o, bool select )
{
    CHECK_MAINWINDOW;
    if ( !o->isWidgetType() ) {
	// ########### do TQObject stuff
	return;
    }

    TQWidget *w = (TQWidget*)o;

    if ( isMainContainer( w ) ) {
	TQObject *opw = propertyWidget;
	propertyWidget = mainContainer();
	if ( opw->isWidgetType() )
	    repaintSelection( (TQWidget*)opw );
	emitShowProperties( propertyWidget );
	return;
    }

    if ( ::tqt_cast<TQMainWindow*>(mainContainer()) && w == ( (TQMainWindow*)mainContainer() )->centralWidget() ) {
	TQObject *opw = propertyWidget;
	propertyWidget = mainContainer();
	if ( opw->isWidgetType() )
	    repaintSelection( (TQWidget*)opw );
	emitShowProperties( propertyWidget );
	return;
    }

    if ( ::tqt_cast<TQDesignerToolBar*>(o) )
	return;

    if ( select ) {
	TQObject *opw = propertyWidget;
	propertyWidget = w;
	if ( opw->isWidgetType() )
	    repaintSelection( (TQWidget*)opw );
	if ( !isPropertyShowingBlocked() )
	    emitShowProperties( propertyWidget );
	WidgetSelection *s = usedSelections.find( w );
	if ( s ) {
	    s->show();
	    return;
	}

	for ( WidgetSelection *s2 = selections.first(); s2; s2 = selections.next() ) {
	    if ( !s2->isUsed() ) {
		s = s2;
	    }
	}

	if ( !s ) {
	    s = new WidgetSelection( this, &usedSelections );
	    selections.append( s );
	}

	s->setWidget( w );
	emitSelectionChanged();
    } else {
	WidgetSelection *s = usedSelections.find( w );
	if ( s )
	    s->setWidget( 0 );
	TQObject *opw = propertyWidget;
	if ( !usedSelections.isEmpty() )
	    propertyWidget = TQPtrDictIterator<WidgetSelection>( usedSelections ).current()->widget();
	else
	    propertyWidget = mainContainer();
	if ( opw->isWidgetType() )
	    repaintSelection( (TQWidget*)opw );
	if ( !isPropertyShowingBlocked() )
	    emitShowProperties( propertyWidget );
	emitSelectionChanged();
    }
}

TQPoint FormWindow::grid() const
{
    if ( !mainWindow() || !mainWindow()->snapGrid() )
	return TQPoint( 1, 1 );
    return mainWindow()->grid();
}

void FormWindow::updateSelection( TQWidget *w )
{
    WidgetSelection *s = usedSelections.find( w );
    if ( !w->isVisibleTo( this ) )
	selectWidget( w, false );
    else if ( s )
	s->updateGeometry();
}

void FormWindow::raiseSelection( TQWidget *w )
{
    WidgetSelection *s = usedSelections.find( w );
    if ( s )
	s->show();
}

void FormWindow::repaintSelection( TQWidget *w )
{
    WidgetSelection *s = usedSelections.find( w );
    if ( s )
	s->update();
}

void FormWindow::clearSelection( bool changePropertyDisplay )
{
    TQPtrDictIterator<WidgetSelection> it( usedSelections );
    for ( ; it.current(); ++it )
	it.current()->setWidget( 0, false );

    usedSelections.clear();
    if ( changePropertyDisplay ) {
	TQObject *opw = propertyWidget;
	propertyWidget = mainContainer();
	if ( opw->isWidgetType() )
	    repaintSelection( (TQWidget*)opw );
	emitShowProperties( propertyWidget );
    }
    emitSelectionChanged();
}

void FormWindow::startRectDraw( const TQPoint &p, const TQPoint &global, TQWidget *, RectType t )
{
    TQPoint pos( p );
    pos = mapFromGlobal( global );
    oldRectValid = false;
    beginUnclippedPainter( true );
    if ( t == Rubber )
	unclippedPainter->setPen( TQPen( color0, 1 ) );
    if ( t == Insert )
	rectAnchor = gridPoint( pos );
    else if ( t == Rubber )
	rectAnchor = pos;
    currRect = TQRect( rectAnchor, TQPoint( 0, 0 ) );
    if ( t == Insert )
	drawSizePreview( pos, tr("Use Size Hint") );
}

void FormWindow::continueRectDraw( const TQPoint &p, const TQPoint &global, TQWidget *, RectType t )
{
    TQPoint pos =p;
    pos = mapFromGlobal( global );
    TQPoint p2;
    if ( t == Insert )
	p2 = gridPoint( pos );
    else if ( t == Rubber )
	p2 = pos;
    TQRect r( rectAnchor, p2 );
    r = r.normalize();

    if ( currRect == r ) {
	TQString t = tr( "%1/%2" );
	t = t.arg( r.width() - 1 ).arg( r.height() - 1 );
	drawSizePreview( pos, t );
	return;
    }

    if ( oldRectValid )
	unclippedPainter->drawRect( currRect );
    if ( r.width() > 1 || r.height() > 1 ) {
	oldRectValid = true;
	currRect = r;
	if ( t == Insert ) {
	    TQString t = tr( "%1/%2" );
	    t = t.arg( r.width() - 1 ).arg( r.height() - 1 );
	    drawSizePreview( pos, t );
	}
	unclippedPainter->setClipRegion( TQRegion( rect() ).subtract( TQRect( sizePreviewPos, sizePreviewPixmap.size() ) ) );
	unclippedPainter->drawRect( currRect );
	unclippedPainter->setClipping( false );
    } else {
	oldRectValid = false;
	if ( t == Insert )
	    drawSizePreview( pos, tr("Use Size Hint") );
    }
}

void FormWindow::endRectDraw()
{
    if ( !unclippedPainter )
	return;

    if ( oldRectValid )
	unclippedPainter->drawRect( currRect );
    drawSizePreview( TQPoint(-1,-1), TQString::null );
    endUnclippedPainter();
}

void FormWindow::selectWidgets()
{
    TQObjectList *l = mainContainer()->queryList( "TQWidget" );
    if ( l ) {
	for ( TQObject *o = l->first(); o; o = l->next() ) {
	    if ( ( (TQWidget*)o )->isVisibleTo( this ) &&
		 insertedWidgets[ (void*)o ] ) {
		TQPoint p = ( (TQWidget*)o )->mapToGlobal( TQPoint(0,0) );
		p = mapFromGlobal( p );
		TQRect r( p, ( (TQWidget*)o )->size() );
		if ( r.intersects( currRect ) && !r.contains( currRect ) )
		    selectWidget( (TQWidget*)o );
	    }
	}
	delete l;
    }
    emitSelectionChanged();
}

bool FormWindow::isWidgetSelected( TQObject *w )
{
    if ( w->isWidgetType() )
	return usedSelections.find( (TQWidget*)w ) != 0;
    return false; // #### do stuff for TQObjects
}

void FormWindow::moveSelectedWidgets( int dx, int dy )
{
    TQPtrDictIterator<WidgetSelection> it( usedSelections );
    for ( ; it.current(); ++it ) {
	WidgetSelection *s = it.current();
	TQWidget *w = s->widget();
	if ( w->parentWidget() && WidgetFactory::layoutType( w->parentWidget() ) != WidgetFactory::NoLayout )
	    continue;
	w->move( w->x() + dx, w->y() + dy );
	s->updateGeometry();
	updateChildSelections( w );
    }
}

CommandHistory *FormWindow::commandHistory()
{
    return &commands;
}

void FormWindow::undo()
{
    commandHistory()->undo();
}

void FormWindow::redo()
{
    commandHistory()->redo();
}

void FormWindow::raiseChildSelections( TQWidget *w )
{
    TQObjectList *l = w->queryList( "TQWidget" );
    if ( !l || !l->first() ) {
	delete l;
	return;
    }

    TQPtrDictIterator<WidgetSelection> it( usedSelections );
    for ( ; it.current(); ++it ) {
	if ( l->findRef( it.current()->widget() ) != -1 )
	    it.current()->show();
    }
    delete l;
}

void FormWindow::updateChildSelections( TQWidget *w )
{
    TQObjectList *l = w->queryList( "TQWidget" );
    if ( l ) {
	for ( TQObject *o = l->first(); o; o = l->next() ) {
	    if ( o->isWidgetType() &&
		 insertedWidgets.find( (TQWidget*)o ) )
		updateSelection( (TQWidget*)o );
	}
	delete l;
    }
}

void FormWindow::checkSelectionsForMove( TQWidget *w )
{
    checkedSelectionsForMove = true;

    TQObjectList *l = w->parentWidget()->queryList( "TQWidget", 0, false, false );
    moving.clear();
    if ( l ) {
	TQPtrDictIterator<WidgetSelection> it( usedSelections );
	WidgetSelection *sel;
	while ( ( sel = it.current() ) != 0 ) {
	    if ( it.current()->widget() == mainContainer() )
		continue;
	    ++it;
	    if ( l->find( sel->widget() ) == -1 ) {
		if ( WidgetFactory::layoutType( w ) == WidgetFactory::NoLayout )
		    sel->setWidget( 0 );
	    } else {
		if ( WidgetFactory::layoutType( sel->widget()->parentWidget() ) == WidgetFactory::NoLayout ) {
		    moving.insert( sel->widget(), sel->widget()->pos() );
		    sel->widget()->raise();
		    raiseChildSelections( sel->widget() );
		    raiseSelection( sel->widget() );
		}
	    }
	}
	delete l;
    }
}

void FormWindow::deleteWidgets()
{
    CHECK_MAINWINDOW;
    TQWidgetList widgets;
    TQPtrDictIterator<WidgetSelection> it( usedSelections );
    for ( ; it.current(); ++it ) {
	TQWidget *tb = 0;
	if ( !( tb = mainWindow()->isAToolBarChild( it.current()->widget() ) ) )
	     widgets.append( it.current()->widget() );
	else
	    ( (TQDesignerToolBar*)tb )->removeWidget( it.current()->widget() );
    }

    if ( widgets.isEmpty() )
	return;

    DeleteCommand *cmd = new DeleteCommand( tr( "Delete" ), this, widgets );
    commandHistory()->addCommand( cmd );
    cmd->execute();
}

void FormWindow::editAdjustSize()
{
    TQPtrList<Command> commands;
    TQWidgetList widgets = selectedWidgets();
    if ( widgets.isEmpty() ) {
	TQRect oldr = geometry();
	mainContainer()->adjustSize();
	resize( mainContainer()->size() );
	// check whether our own size constraint hit us
	if ( size() != mainContainer()->size() )
	    mainContainer()->resize( size() );
	TQRect nr = geometry();
	if ( oldr != nr ) {
	    ResizeCommand *cmd = new ResizeCommand( tr( "Adjust Size" ), this, this, oldr, nr );
	    commandHistory()->addCommand( cmd );
	}
	return;
    }
    for ( TQWidget* w = widgets.first(); w; w = widgets.next() ) {
	if ( w->parentWidget() && WidgetFactory::layoutType( w->parentWidget() ) != WidgetFactory::NoLayout )
	    continue;
	TQRect oldr = w->geometry();
	w->adjustSize();
	TQRect nr = w->geometry();
	if ( oldr != nr )
	    commands.append( new ResizeCommand( tr("Adjust Size"), this, w, oldr, nr ) );
    }

    if ( commands.isEmpty() )
	return;
    for ( WidgetSelection *s = selections.first(); s; s = selections.next() )
	s->updateGeometry();

    MacroCommand *cmd = new MacroCommand( tr( "Adjust Size" ), this, commands );
    commandHistory()->addCommand( cmd );
}


TQWidgetList FormWindow::selectedWidgets() const
{
    TQWidgetList widgets;
    TQPtrDictIterator<WidgetSelection> it( usedSelections );
    for ( ; it.current(); ++it )
	widgets.append( it.current()->widget() );
    return widgets;
}

void FormWindow::widgetChanged( TQObject *w )
{
    if ( w->isWidgetType() )
	updateSelection( (TQWidget*)w );
    // ########## do TQObject stuff
}

TQLabel *FormWindow::sizePreview() const
{
    if ( !sizePreviewLabel ) {
	( (FormWindow*)this )->sizePreviewLabel = new TQLabel( (FormWindow*)this );
	( (FormWindow*)this )->sizePreviewLabel->hide();
	( (FormWindow*)this )->sizePreviewLabel->setBackgroundColor( TQColor( 255, 255, 128 ) );
	( (FormWindow*)this )->sizePreviewLabel->setFrameStyle( TQFrame::Plain | TQFrame::Box );
    }
    return sizePreviewLabel;
}

void FormWindow::invalidCheckedSelections()
{
    checkedSelectionsForMove = false;
}

void FormWindow::checkPreviewGeometry( TQRect &r )
{
    if ( !rect().contains( r ) ) {
	if ( r.left() < rect().left() )
	    r.moveTopLeft( TQPoint( 0, r.top() ) );
	if ( r.right() > rect().right()  )
	    r.moveBottomRight( TQPoint( rect().right(), r.bottom() ) );
	if ( r.top() < rect().top() )
	    r.moveTopLeft( TQPoint( r.left(), rect().top() ) );
	if ( r.bottom() > rect().bottom()  )
	    r.moveBottomRight( TQPoint( r.right(), rect().bottom() ) );
    }
}

void FormWindow::focusInEvent( TQFocusEvent * ){

    if (ff)
	ff->checkTimeStamp();
}

void FormWindow::focusOutEvent( TQFocusEvent * )
{
    if ( propertyWidget && !isMainContainer( propertyWidget ) && !isWidgetSelected( propertyWidget ) ) {
	TQObject *opw = propertyWidget;
	propertyWidget = mainContainer();
	if ( opw->isWidgetType() )
	    repaintSelection( (TQWidget*)opw );
    }
}

void FormWindow::resizeEvent( TQResizeEvent *e )
{
    TQWidget::resizeEvent( e );
    if ( currTool == ORDER_TOOL )
	repositionOrderIndicators();
    if ( isVisible() )
	formFile()->setModified( true, FormFile::WFormWindow );

#if defined(TQ_WS_WIN32)
    windowsRepaintWorkaroundTimer->start( 100, true );
#endif
}

void FormWindow::windowsRepaintWorkaroundTimerTimeout()
{
#if defined(TQ_WS_WIN32)
    TQObjectList *l = queryList( "TQWidget" );
    for ( TQObject *o = l->first(); o; o = l->next() ) {
	flickerfree_update( (TQWidget*)o );
    }
    flickerfree_update( this );
    delete l;
#endif
}

TQPtrDict<TQWidget> *FormWindow::widgets()
{
    return &insertedWidgets;
}

TQWidget *FormWindow::designerWidget( TQObject *o ) const
{
    if ( !o || !o->isWidgetType() )
	return 0;
    TQWidget *w = (TQWidget*)o;
    while ( ( w && !isMainContainer( w ) && !insertedWidgets[ (void*)w ] ) || isCentralWidget( w ) )
	w = (TQWidget*)w->parent();
    return w;
}

void FormWindow::emitShowProperties( TQObject *w )
{
    if ( w ) {
	TQObject *opw = propertyWidget;
	propertyWidget = w;
	if ( opw->isWidgetType() )
	    repaintSelection( (TQWidget*)opw );
    }
    showPropertiesTimer->stop();
    showPropertiesTimer->start( 0, true );
}

void FormWindow::emitUpdateProperties( TQObject *w )
{
    if ( w == propertyWidget ) {
	updatePropertiesTimer->stop();
	updatePropertiesTimer->start( 0, true );
    }
}

void FormWindow::emitSelectionChanged()
{
    selectionChangedTimer->stop();
    selectionChangedTimer->start( 0, true );
}

void FormWindow::updatePropertiesTimerDone()
{
    CHECK_MAINWINDOW;
    if ( propertyWidget && mainWindow()->formWindow() == this )
	emit updateProperties( propertyWidget );
}

void FormWindow::showPropertiesTimerDone()
{
    CHECK_MAINWINDOW;
    if ( propertyWidget && mainWindow()->formWindow() == this )
	emit showProperties( propertyWidget );
}

void FormWindow::selectionChangedTimerDone()
{
    emit selectionChanged();
}

void FormWindow::currentToolChanged()
{
    CHECK_MAINWINDOW;
    toolFixed = false;
    int t = mainwindow->currentTool();
    if ( currTool == t && t != ORDER_TOOL )
	return;

    // tool cleanup
    switch ( currTool ) {
    case ORDER_TOOL:
	hideOrderIndicators();
	break;
    case CONNECT_TOOL:
    case BUDDY_TOOL:
	restoreConnectionLine();
	if ( startWidget )
	    restoreRect( TQRect( mapToForm( ( (TQWidget*)startWidget )->parentWidget(),
					   ( (TQWidget*)startWidget )->pos() ),
				( (TQWidget*)startWidget )->size() ) );
	if ( endWidget )
	    restoreRect( TQRect( mapToForm( ( (TQWidget*)endWidget )->parentWidget(),
					   ( (TQWidget*)endWidget )->pos() ),
				( (TQWidget*)endWidget )->size() ) );
	endUnclippedPainter();
	break;
    case POINTER_TOOL:
	break;
    default:
	if ( insertParent )
	    endRectDraw();
	break;
    }

    startWidget = endWidget = 0;
    widgetPressed = false;
    drawRubber = false;
    insertParent = 0;
    delete buffer;
    buffer = 0;

    currTool = t;

    if ( hasFocus() )
	clearSelection( false );

    mainWindow()->statusBar()->clear();

    // tool setup
    switch ( currTool ) {
    case POINTER_TOOL:
	if ( propertyWidget && !isMainContainer( propertyWidget ) && !isWidgetSelected( propertyWidget ) )
	    emitShowProperties( mainContainer() );
	restoreCursors( this, this );
	break;
    case ORDER_TOOL:
	if ( mainWindow()->formWindow() == this ) {
	    mainWindow()->statusBar()->message( tr( "Click widgets to change the tab order...") );
	    orderedWidgets.clear();
	    showOrderIndicators();
	    if ( mainWindow()->formWindow() == this )
		emitShowProperties( mainContainer() );
	    setCursorToAll( ArrowCursor, this );
	}
	break;
    case CONNECT_TOOL:
    case BUDDY_TOOL:
	if ( currTool == CONNECT_TOOL )
	    mainWindow()->statusBar()->message( tr( "Drag a line to create a connection...") );
	else
	    mainWindow()->statusBar()->message( tr( "Drag a line to set a buddy...") );
	setCursorToAll( CrossCursor, this );
	if ( mainWindow()->formWindow() == this )
	    emitShowProperties( mainContainer() );
	break;
    default:
	mainWindow()->statusBar()->message( tr( "Click on the form to insert a %1..." ).arg( WidgetDatabase::toolTip( currTool ).lower() ) );
	setCursorToAll( CrossCursor, this );
	if ( mainWindow()->formWindow() == this )
	    emitShowProperties( mainContainer() );
	break;
    }
}

void FormWindow::showOrderIndicators()
{
    hideOrderIndicators();
    orderIndicators.setAutoDelete( true );
    TQObjectList *l = mainContainer()->queryList( "TQWidget" );
    stackedWidgets = MetaDataBase::tabOrder( this );
    if ( l ) {
	int order = 1;
	for ( TQObject *o = l->first(); o; o = l->next() ) {
	    TQWidget* w = (TQWidget*) o;
	    if ( w->isShown() &&
		 insertedWidgets[ (void*)w ]  &&
		 w->focusPolicy() != NoFocus ) {
		OrderIndicator* ind = new OrderIndicator( order++, w, this );
		orderIndicators.append( ind );
		if ( stackedWidgets.findRef( w ) == -1 )
		    stackedWidgets.append( w );
	    }
	}
	delete l;
    }
    updateOrderIndicators();
}

void FormWindow::hideOrderIndicators()
{
    orderIndicators.clear();
}

void FormWindow::updateOrderIndicators()
{
    int order = 1;
    for ( TQWidget *w = stackedWidgets.first(); w; w = stackedWidgets.next() ) {
	for ( OrderIndicator* i = orderIndicators.first(); i; i = orderIndicators.next() )
	    i->setOrder( order, w );
	order++;
    }
}

void FormWindow::repositionOrderIndicators()
{
    for ( OrderIndicator* i = orderIndicators.first(); i; i = orderIndicators.next() )
	i->reposition();
}


void FormWindow::updateUndoInfo()
{
    commandHistory()->emitUndoRedo();
}

bool FormWindow::checkCustomWidgets()
{
    TQStringList missingCustomWidgets;
    TQPtrDictIterator<TQWidget> it( insertedWidgets );
    for ( ; it.current(); ++it ) {
	if ( it.current()->isA( "CustomWidget" ) ) {
	    TQString className = WidgetFactory::classNameOf( it.current() );
	    if ( !MetaDataBase::hasCustomWidget( className ) )
		missingCustomWidgets << className;
	}
    }

    if ( !missingCustomWidgets.isEmpty() ) {
	TQString txt = tr( "The following custom widgets are used in '%1',\n"
			  "but are not known to TQt Designer:\n" ).arg( name() );
	for ( TQStringList::Iterator sit = missingCustomWidgets.begin(); sit != missingCustomWidgets.end(); ++sit )
	    txt += "   " + *sit + "\n";
	txt += "If you save this form and generate code for it using uic, \n"
	       "the generated code will not compile.\n"
	       "Do you want to save this form now?";
	if ( TQMessageBox::information( mainWindow(), tr( "Save Form" ), txt ) == 1 )
	    return false;
    }
    return true;
}

void FormWindow::setPropertyShowingBlocked( bool b )
{
    propShowBlocked = b;
}

bool FormWindow::isPropertyShowingBlocked() const
{
    return propShowBlocked;
}

int FormWindow::numSelectedWidgets() const
{
    return usedSelections.count();
}

TQString FormWindow::copy()
{
    CHECK_MAINWINDOW_VALUE( TQString::null );
    Resource resource( mainWindow() );
    resource.setWidget( this );
    return resource.copy();
}

void FormWindow::lowerWidgets()
{
    TQWidgetList widgets;
    TQPtrDictIterator<WidgetSelection> it( usedSelections );
    for ( ; it.current(); ++it )
	widgets.append( it.current()->widget() );

    LowerCommand *cmd = new LowerCommand( tr( "Lower" ), this, widgets );
    cmd->execute();
    commandHistory()->addCommand( cmd );
}

void find_accel( const TQString &txt, TQMap<TQChar, TQWidgetList > &accels, TQWidget *w )
{
    int i = txt.find( "&" );
    if ( i == -1 )
	return;
    TQChar c = txt[ i + 1 ];
    if ( c.isNull() || c == '&' )
	return;
    c = c.lower();
    TQMap<TQChar, TQWidgetList >::Iterator it = accels.find( c );
    if ( it == accels.end() ) {
	TQWidgetList wl;
	wl.append( w );
	accels.insert( c, wl );
    } else {
	TQWidgetList *wl = &*it;
	wl->append( w );
    }
}

void FormWindow::checkAccels()
{
    CHECK_MAINWINDOW;
    TQMap<TQChar, TQWidgetList > accels;
    TQObjectList *l = mainContainer()->queryList( "TQWidget" );
    if ( l ) {
	for ( TQObject *o = l->first(); o; o = l->next() ) {
	    if ( ( (TQWidget*)o )->isVisibleTo( this ) &&
		 insertedWidgets[ (void*)o ] ) {
		TQWidget *w = (TQWidget*)o;
		const TQMetaProperty* text =
		    w->metaObject()->property( w->metaObject()->findProperty( "text", true ), true );
		const TQMetaProperty* title =
		    w->metaObject()->property( w->metaObject()->findProperty( "title", true ), true );
		const TQMetaProperty* pageTitle =
		    w->metaObject()->property( w->metaObject()->findProperty( "pageTitle", true ), true );
		if ( text )
		    find_accel( w->property( "text" ).toString(), accels, w );
		if ( title )
		    find_accel( w->property( "title" ).toString(), accels, w );
		if ( pageTitle )
		    find_accel( w->property( "pageTitle" ).toString(), accels, w );
	    } else if ( ::tqt_cast<MenuBarEditor*>(o) ) {
		((MenuBarEditor *)o)->checkAccels( accels );
	    }
	}
	delete l;
    }

    bool ok = true;
    TQWidget *wid;
    for ( TQMap<TQChar, TQWidgetList >::Iterator it = accels.begin(); it != accels.end(); ++it ) {
	if ( (*it).count() > 1 ) {
	    ok = false;
	    switch ( TQMessageBox::information( mainWindow(), tr( "Check Accelerators" ),
					       tr( "Accelerator '%1' is used %2 times."
						   ).arg( it.key().upper() ).arg( (*it).count() ),
					       tr( "&Select" ),
					       tr( "&Cancel" ), TQString::null, 2 ) ) {
	    case 0: // select
		clearSelection( false );
		for ( wid = (*it).first(); wid; wid = (*it).next() )
		    selectWidget( wid, true );
		return;
	    case 1: // cancel
		return;
	    }
	}
    }

    if ( ok )
	TQMessageBox::information( mainWindow(), tr( "Check Accelerators" ),
				  tr( "No accelerator is used more than once." ) );
}

void FormWindow::raiseWidgets()
{
    TQWidgetList widgets;
    TQPtrDictIterator<WidgetSelection> it( usedSelections );
    for ( ; it.current(); ++it )
	widgets.append( it.current()->widget() );

    RaiseCommand *cmd = new RaiseCommand( tr( "Raise" ), this, widgets );
    cmd->execute();
    commandHistory()->addCommand( cmd );
}

void FormWindow::paste( const TQString &cb, TQWidget *parent )
{
    CHECK_MAINWINDOW;
    Resource resource( mainWindow() );
    resource.setWidget( this );
    resource.paste( cb, parent );
}

void FormWindow::selectAll()
{
    checkedSelectionsForMove = false;
    blockSignals( true );
    TQObjectList *l = mainContainer()->queryList( "TQWidget" );
    if ( l ) {
	for ( TQObject *o = l->first(); o; o = l->next() ) {
	    if ( ( (TQWidget*)o )->isVisibleTo( this ) &&
		 insertedWidgets[ (void*)o ] ) {
		selectWidget( (TQWidget*)o );
	    }
	}
	delete l;
    }

    blockSignals( false );
    emitSelectionChanged();
    if ( propertyWidget )
	emitShowProperties( propertyWidget );
    emitSelectionChanged();
}

void FormWindow::layoutHorizontal()
{
    TQWidgetList widgets( selectedWidgets() );
    LayoutHorizontalCommand *cmd = new LayoutHorizontalCommand( tr( "Lay out horizontally" ),
								this, mainContainer(), 0, widgets );
    clearSelection( false );
    commandHistory()->addCommand( cmd );
    cmd->execute();
}

void FormWindow::layoutVertical()
{
    TQWidgetList widgets( selectedWidgets() );
    LayoutVerticalCommand *cmd = new LayoutVerticalCommand( tr( "Lay out vertically" ),
							    this, mainContainer(), 0, widgets );
    clearSelection( false );
    commandHistory()->addCommand( cmd );
    cmd->execute();
}

void FormWindow::layoutHorizontalSplit()
{
    TQWidgetList widgets( selectedWidgets() );
    LayoutHorizontalSplitCommand *cmd = new LayoutHorizontalSplitCommand( tr( "Lay out horizontally (in splitter)" ),
									  this, mainContainer(), 0, widgets );
    clearSelection( false );
    commandHistory()->addCommand( cmd );
    cmd->execute();
}

void FormWindow::layoutVerticalSplit()
{
    TQWidgetList widgets( selectedWidgets() );
    LayoutVerticalSplitCommand *cmd = new LayoutVerticalSplitCommand( tr( "Lay out vertically (in splitter)" ),
								      this, mainContainer(), 0, widgets );
    clearSelection( false );
    commandHistory()->addCommand( cmd );
    cmd->execute();
}

void FormWindow::layoutGrid()
{
    int xres = grid().x();
    int yres = grid().y();

    TQWidgetList widgets( selectedWidgets() );
    LayoutGridCommand *cmd = new LayoutGridCommand( tr( "Lay out in a grid" ),
						    this, mainContainer(), 0, widgets, xres, yres );
    clearSelection( false );
    commandHistory()->addCommand( cmd );
    cmd->execute();
}

void FormWindow::layoutHorizontalContainer( TQWidget *w )
{
    if ( w == this )
	w = mainContainer();
    TQObjectList *l = (TQObjectList*)WidgetFactory::containerOfWidget(w)->children();
    if ( !l )
	return;
    TQWidgetList widgets;
    for ( TQObject *o = l->first(); o; o = l->next() ) {
	if ( o->isWidgetType() &&
	     ( (TQWidget*)o )->isVisibleTo( this ) &&
	     insertedWidgets.find( (TQWidget*)o ) )
	    widgets.append( (TQWidget*)o );
    }
    LayoutHorizontalCommand *cmd = new LayoutHorizontalCommand( tr( "Lay out children horizontally" ),
								this, mainContainer(), w, widgets );
    clearSelection( false );
    commandHistory()->addCommand( cmd );
    cmd->execute();
}

void FormWindow::layoutVerticalContainer( TQWidget *w )
{
    if ( w == this )
	w = mainContainer();
    TQObjectList *l = (TQObjectList*)WidgetFactory::containerOfWidget(w)->children();
    if ( !l )
	return;
    TQWidgetList widgets;
    for ( TQObject *o = l->first(); o; o = l->next() ) {
	if ( o->isWidgetType() &&
	     ( (TQWidget*)o )->isVisibleTo( this ) &&
	     insertedWidgets.find( (TQWidget*)o ) )
	    widgets.append( (TQWidget*)o );
    }
    LayoutVerticalCommand *cmd = new LayoutVerticalCommand( tr( "Lay out children vertically" ),
							    this, mainContainer(), w, widgets );
    clearSelection( false );
    commandHistory()->addCommand( cmd );
    cmd->execute();
}

void FormWindow::layoutGridContainer( TQWidget *w )
{
    if ( w == this )
	w = mainContainer();
    int xres = grid().x();
    int yres = grid().y();

    TQObjectList *l = (TQObjectList*)WidgetFactory::containerOfWidget(w)->children();
    if ( !l )
	return;
    TQWidgetList widgets;
    for ( TQObject *o = l->first(); o; o = l->next() ) {
	if ( o->isWidgetType() &&
	     ( (TQWidget*)o )->isVisibleTo( this ) &&
	     insertedWidgets.find( (TQWidget*)o ) )
	    widgets.append( (TQWidget*)o );
    }
    LayoutGridCommand *cmd = new LayoutGridCommand( tr( "Lay out children in a grid" ),
						    this, mainContainer(), w, widgets, xres, yres );
    clearSelection( false );
    commandHistory()->addCommand( cmd );
    cmd->execute();
}

void FormWindow::breakLayout( TQWidget *w )
{
    if ( w == this )
	w = mainContainer();
    w = WidgetFactory::containerOfWidget( w );
    TQPtrList<Command> commands;

    for (;;) {
	if ( !w || w == this )
	    break;
	if ( WidgetFactory::layoutType( w ) != WidgetFactory::NoLayout &&
	     WidgetDatabase::isContainer( WidgetDatabase::idFromClassName( WidgetFactory::classNameOf( w ) ) ) ) {
	    Command *cmd = breakLayoutCommand( w );
	    if ( cmd )
		commands.insert( 0, cmd );
	    if ( !::tqt_cast<TQLayoutWidget*>(w) && !::tqt_cast<TQSplitter*>(w) )
		break;
	}
	w = w->parentWidget();
    }

    if ( commands.isEmpty() )
	return;

    clearSelection( false );
    MacroCommand *cmd = new MacroCommand( tr( "Break Layout" ), this, commands );
    commandHistory()->addCommand( cmd );
    cmd->execute();
}

BreakLayoutCommand *FormWindow::breakLayoutCommand( TQWidget *w )
{
    CHECK_MAINWINDOW_VALUE( 0 );
    TQObjectList *l = (TQObjectList*)w->children();
    if ( !l )
	return 0;

    TQWidgetList widgets;
    for ( TQObject *o = l->first(); o; o = l->next() ) {
	if ( o->isWidgetType() &&
	     !mainWindow()->isAToolBarChild( (TQWidget*)o ) &&
	     ( (TQWidget*)o )->isVisibleTo( this ) &&
	     insertedWidgets.find( (TQWidget*)o ) )
	    widgets.append( (TQWidget*)o );
    }
    return new BreakLayoutCommand( tr( "Break Layout" ), this, WidgetFactory::widgetOfContainer( w ), widgets );
}

int FormWindow::numVisibleWidgets() const
{
    TQPtrDictIterator<TQWidget> it( insertedWidgets );
    int visible = 0;
    for ( ; it.current(); ++it ) {
	if ( it.current()->isVisibleTo( (FormWindow*)this ) )
	    visible++;
    }
    return visible;
}

bool FormWindow::hasInsertedChildren( TQWidget *w ) const
{
    if ( !w )
	return false;
    w = WidgetFactory::containerOfWidget( w );
    if ( !w )
	return false;
    TQObjectList *l = w->queryList( "TQWidget" );
    if ( !l || !l->first() ) {
	delete l;
	return false;
    }

    for ( TQObject *o = l->first(); o; o = l->next() ) {
	if ( o->isWidgetType() &&
	     ( (TQWidget*)o )->isVisibleTo( (FormWindow*)this ) &&
	     insertedWidgets.find( (TQWidget*)o ) ) {
	    delete l;
	    return true;
	}
    }

    delete l;
    return false;
}

bool FormWindow::allowMove( TQWidget *w )
{
    w = w->parentWidget();
    while ( w ) {
	if ( ( isMainContainer( w ) || insertedWidgets.find( w ) ) && WidgetFactory::layoutType( w ) == WidgetFactory::NoLayout )
	    return true;
	w = w->parentWidget();
    }
    return false;
}


void FormWindow::editConnections()
{
    CHECK_MAINWINDOW;
    buffer = 0;
    if ( !startWidget || !endWidget )
	return;

    ConnectionDialog dlg( mainwindow );
    mainWindow()->statusBar()->message( tr( "Edit connections...") );
    dlg.addConnection( startWidget, endWidget, TQString::null, TQString::null );
    TQTimer::singleShot( 0, &dlg, TQ_SLOT(ensureConnectionVisible()) );
    dlg.exec();
}

void FormWindow::saveBackground()
{
    if ( buffer )
	delete buffer;
    buffer = new TQPixmap( width(), height() );
    *buffer = TQPixmap::grabWindow( winId() );
}

void FormWindow::restoreConnectionLine()
{
    if (!unclippedPainter || !buffer) // designer will occasionally crash if buffer is not tested to be non-zero
	return;

    int a =TQABS( startPos.x() - currentPos.x() );
    int b = TQABS( startPos.y() - currentPos.y() );
    TQRect r( startPos, currentPos );

    if ( a < 32 || b < 32 ) { // special case: vertical or horizontal line
	r = r.normalize();
	unclippedPainter->drawPixmap( r.x() - 2, r.y() - 2, *buffer,
				      r.x() - 2, r.y() - 2, r.width() + 4, r.height() + 4 );
	return;
    }

    if ( a <= 0 )
	a = 1;
    if ( b <= 0 )
	b = 1;
    int w, h;
    if ( b > a ) {
	h = 64;
	w = ( a * h ) / b;
    } else {
	w = 64;
	h = ( b * w ) / a;
    }

    int dx = 2 * w / 3;
    int dy = 2 * h / 3;
    TQPoint p( startPos );

    if ( r.x() > r.right() ) {
	dx = dx * -1;
	p.setX( p.x() - 64 );
	r.moveBy( -64, 0 );
    }
    if ( r.y() > r.bottom() ) {
	dy = dy * -1;
	p.setY( p.y() - 64 );
	r.moveBy( 0, -64 );
    }

    w = h = 64;
    r = r.normalize();
    while ( r.contains( p ) ) {
	unclippedPainter->drawPixmap( p, *buffer, TQRect( p, TQSize( w, h ) ) );
	unclippedPainter->setPen( red );
	p.setX( p.x() + dx );
	p.setY( p.y() + dy );
    }

    unclippedPainter->drawPixmap( startPos.x() - 10, startPos.y() - 10, *buffer,
				  startPos.x() - 10, startPos.y() - 10, 20, 20 );
}

void FormWindow::restoreRect( const TQRect &rect )
{
    if (!unclippedPainter || !buffer)
	return;

    TQRect r( rect );
    r = r.normalize();

    r = TQRect( r.x() + 2, r.y() + 2, r.width() - 4, r.height() - 4 );

    unclippedPainter->drawPixmap( r.x() - 2, r.y() - 2, *buffer, r.x() - 2, r.y() - 2, r.width() + 4, 4 );
    unclippedPainter->drawPixmap( r.x() - 2, r.y() - 2, *buffer, r.x() - 2, r.y() - 2, 4, r.height() + 4 );
    unclippedPainter->drawPixmap( r.x() - 2, r.y() + r.height() - 3, *buffer, r.x() - 2, r.y() + r.height() - 3, r.width() + 4, 5 );
    unclippedPainter->drawPixmap( r.x() + r.width() - 2, r.y(), *buffer, r.x() + r.width() - 2, r.y(), 4, r.height() + 4 );
}

void FormWindow::drawConnectionLine()
{
    if ( !unclippedPainter )
	return;

    unclippedPainter->setPen( TQPen( white, 2 ) );
    unclippedPainter->drawLine( startPos, currentPos );
    if ( validForBuddy )
	unclippedPainter->setPen( TQPen( darkRed, 1 ) );
    else
	unclippedPainter->setPen( TQPen( darkCyan, 1 ) );
    unclippedPainter->drawLine( startPos, currentPos );

    if ( validForBuddy )
	unclippedPainter->setPen( TQPen( darkGreen, 1 ) );
    else
	unclippedPainter->setPen( TQPen( magenta, 1 ) );
    if ( startWidget ) {
	TQWidget *s = (TQWidget*)startWidget;
	TQPoint p = mapToForm( s, TQPoint(0,0) );
	unclippedPainter->drawRect( TQRect( p + TQPoint( 2, 2 ), s->size() - TQSize( 4, 4 ) ) );
     }
    if ( endWidget ) {
	TQWidget *e = (TQWidget*)endWidget;
	TQPoint p = mapToForm( e, TQPoint(0,0) );
	unclippedPainter->drawRect( TQRect( p + TQPoint( 2, 2 ), e->size() - TQSize( 4, 4 ) ) );
     }
}

TQString FormWindow::fileName() const
{
    return ff->absFileName();
}

void FormWindow::setFileName( const TQString &fn )
{
    ff->setFileName( fn );
    emit fileNameChanged( ff->fileName(), this );
}

void FormWindow::modificationChanged( bool m )
{
    emit modificationChanged( m, this );
    emit modificationChanged( m, ff->fileName() );
}

bool FormWindow::unify( TQObject *w, TQString &s, bool changeIt )
{
    bool found = !isMainContainer( w ) && qstrcmp( name(), s.latin1() ) == 0;
    if ( !found ) {
	TQString orig = s;
	int num  = 1;
	TQPtrDictIterator<TQWidget> it( insertedWidgets );
	for ( ; it.current();) {
	    if ( it.current() != w &&
		 qstrcmp( it.current()->name(), s.latin1() ) == 0 ) {
		found = true;
		if ( !changeIt )
		    break;
		s = orig + "_" + TQString::number( ++num );
		it.toFirst();
	    } else {
		++it;
	    }
	}
	if ( !found ) {
	    TQPtrList<TQAction> al;
	    TQAction *a = 0;
	    for ( a = actions.first(); a; a = actions.next() ) {
		TQObjectList *l = a->queryList( "TQAction" );
		al.append( a );
		for ( TQObject *ao = l->first(); ao; ao = l->next() )
		    al.append( (TQAction*)ao );
		delete l;
	    }
	    for ( a = al.first(); a; a = al.next() ) {
		if ( a != w &&
		     qstrcmp( a->name(), s.latin1() ) == 0 ) {
		    found = true;
		    if ( !changeIt )
			break;
		    s = orig + "_" + TQString::number( ++num );
		    a = actions.first();
		}
	    }
	}
	if ( ::tqt_cast<TQMainWindow*>(mainContainer()) && !found ) {
	    TQObjectList *l = mainContainer()->queryList( "PopupMenuEditor" );
	    for ( TQObject *o = l->first(); o; o = l->next() ) {
		if ( o != w &&
		     qstrcmp ( o->name(), s.latin1() ) == 0 ) {
		    found = true;
		    if ( !changeIt )
			break;
		    s = orig + "_" + TQString::number( ++num );
		    o = l->first();
		}
	    }
	    delete l;
	}
	if ( ::tqt_cast<TQMainWindow*>(mainContainer()) ) {
	    if ( !found ) {
		TQObjectList *l = mainContainer()->queryList( "TQDockWindow", 0, true );
		for ( TQObject *o = l->first(); o; o = l->next() ) {
		    if ( o != w &&
			 qstrcmp( o->name(), s.latin1() ) == 0 ) {
			found = true;
			if ( !changeIt )
			    break;
			s = orig + "_" + TQString::number( ++num );
			o = l->first();
		    }
		}
		delete l;
	    }
	}
    }

    if ( !found )
	return true;
    return false;
}

bool FormWindow::isCustomWidgetUsed( MetaDataBase::CustomWidget *w )
{
    TQPtrDictIterator<TQWidget> it( insertedWidgets );
    for ( ; it.current(); ++it ) {
	if ( it.current()->isA( "CustomWidget" ) ) {
	    if ( qstrcmp( WidgetFactory::classNameOf( it.current() ), w->className.utf8() ) == 0 )
		return true;
	}
    }

    return false;
}

bool FormWindow::isDatabaseWidgetUsed() const
{
#ifndef TQT_NO_SQL
    TQStringList dbClasses;
    dbClasses << "TQDataTable"; // add more here
    TQPtrDictIterator<TQWidget> it( insertedWidgets );
    for ( ; it.current(); ++it ) {
	TQString c( it.current()->className() );
	if ( dbClasses.contains( c ) > 0 ) {
	    return true;
	}
    }
#endif
    return false;
}

bool FormWindow::isDatabaseAware() const
{
#ifndef TQT_NO_SQL
    if ( TQString(mContainer->className()) == "TQDesignerDataBrowser" || TQString(mContainer->className()) == "TQDesignerDataView" )
	return true;
    return isDatabaseWidgetUsed();
#else
    return false;
#endif
}

void FormWindow::visibilityChanged()
{
    if ( currTool != ORDER_TOOL ) {
	emitUpdateProperties( currentWidget() );
    } else {
	updateOrderIndicators();
	repositionOrderIndicators();
    }
}


/*!
  Maps \a pos in \a w's coordinates to the form's coordinate system.

  This is the equivalent to mapFromGlobal(w->mapToGlobal(pos) ) but
  avoids the two roundtrips to the X-Server on Unix/X11.
 */
TQPoint FormWindow::mapToForm( const TQWidget* w, const TQPoint&  pos ) const
{
    TQPoint p = pos;
    const TQWidget* i = w;
    while ( i && !i->isTopLevel() && !isMainContainer( (TQWidget*)i ) ) {
	p = i->mapToParent( p );
	i = i->parentWidget();
    }
    return mapFromGlobal( w->mapToGlobal( pos ) );
}

static int widgetDepth( TQWidget *w )
{
    int d = -1;
    while ( w && !w->isTopLevel() ) {
	d++;
	w = w->parentWidget();
    }

    return d;
}

static bool isChildOf( TQWidget *c, TQWidget *p )
{
    while ( c && !c->isTopLevel() ) {
	if ( c == p )
	    return true;
	c = c->parentWidget();
    }
    return false;
}

TQWidget *FormWindow::containerAt( const TQPoint &pos, TQWidget *notParentOf )
{
    TQPtrDictIterator<TQWidget> it( insertedWidgets );
    TQWidget *container = 0;
    int depth = -1;
    TQWidgetList selected = selectedWidgets();
    if ( rect().contains( mapFromGlobal( pos ) ) ) {
	container = mainContainer();
	depth = widgetDepth( container );
    }

    for ( ; it.current(); ++it ) {
	if ( ::tqt_cast<TQLayoutWidget*>(it.current())
	  || ::tqt_cast<TQSplitter*>(it.current()) )
	    continue;
	if ( !it.current()->isVisibleTo( this ) )
	    continue;
	if ( selected.find( it.current() ) != -1 )
	    continue;
	if ( !WidgetDatabase::isContainer( WidgetDatabase::idFromClassName( WidgetFactory::classNameOf( it.current() ) ) ) &&
	     it.current() != mainContainer() )
	    continue;

	// the rectangles of all ancestors of the container must contain the insert position
	TQWidget *w = it.current();
	while ( w && !w->isTopLevel() ) {
	    if ( !w->rect().contains( ( w->mapFromGlobal( pos ) ) ) )
		break;
	    w = w->parentWidget();
	}
	if ( !( w == 0 || w->isTopLevel() ) ) continue; // we did not get through the full while loop

	int wd = widgetDepth( it.current() );
	if ( wd == depth && container ) {
	    if ( ( (TQObjectList*)it.current()->parentWidget()->children() )->find( it.current() ) >
		 ( (TQObjectList*)container->parentWidget()->children() )->find( container ) )
		wd++;
	}
	if ( wd > depth && !isChildOf( it.current(), notParentOf ) ) {
	    depth = wd;
	    container = it.current();
	}
    }

    return container;
}

bool FormWindow::isMainContainer( TQObject *w ) const
{
    return w && w->isWidgetType() && ( w == (TQWidget*)this || w == mainContainer() );
}

void FormWindow::setMainContainer( TQWidget *w )
{
    bool resetPropertyWidget = isMainContainer( propertyWidget );
    if ( mContainer )
	insertedWidgets.remove( mContainer );
    if ( propertyWidget == mContainer )
	propertyWidget = 0;
    delete mContainer;
    mContainer = w;
    insertedWidgets.insert( mContainer, mContainer );
    delete layout();
    TQHBoxLayout *l = new TQHBoxLayout( this );
    l->addWidget( w );
    if ( resetPropertyWidget ) {
	TQObject *opw = propertyWidget;
	propertyWidget = mContainer;
	if ( opw && opw->isWidgetType() )
	    repaintSelection( (TQWidget*)opw );
    }
    if ( project() ) {
	LanguageInterface *iface = MetaDataBase::languageInterface( project()->language() );
	if ( iface && !project()->isCpp() && !isFake() ) {
	    if ( !MetaDataBase::hasFunction( this, "init()" ) )
		MetaDataBase::addFunction( this, "init()", "", "private", "function",
					   project()->language(), "void" );
	    if ( !MetaDataBase::hasFunction( this, "destroy()" ) )
		MetaDataBase::addFunction( this, "destroy()", "", "private", "function",
					   project()->language(), "void" );
	    if ( !MetaDataBase::hasConnection( this, mainContainer(), "shown()", mainContainer(), "init" ) )
		MetaDataBase::addConnection( this, mainContainer(), "shown()", mainContainer(), "init" );
	    if ( !MetaDataBase::hasConnection( this, mainContainer(), "destroyed()", mainContainer(), "destroy"  ) )
		MetaDataBase::addConnection( this, mainContainer(), "destroyed()",
					     mainContainer(), "destroy" );
	}
    }
}

bool FormWindow::savePixmapInline() const
{
    return pixInline;
}

bool FormWindow::savePixmapInProject() const
{
    return pixProject;
}

TQString FormWindow::pixmapLoaderFunction() const
{
    return pixLoader;
}

void FormWindow::setSavePixmapInline( bool b )
{
    pixInline = b;
    if ( b )
	pixProject = false;
}

void FormWindow::setSavePixmapInProject( bool b )
{
    pixProject = b;
    if ( b )
	pixInline = false;
}

void FormWindow::setPixmapLoaderFunction( const TQString &func )
{
    pixLoader = func;
}

void FormWindow::setActiveObject( TQObject *o )
{
    emitShowProperties( o );
    propertyWidget = o;
}

void FormWindow::setProject( Project *pro )
{
    proj = pro;
}

Project *FormWindow::project() const
{
    return proj;
}

TQAction *FormWindow::findAction( const TQString &name )
{
    for ( TQAction *a = actionList().first(); a; a = actionList().next() ) {
	if ( TQString( a->name() ) == name )
	    return a;
	TQAction *ac = (TQAction*)a->child( name.latin1(), "TQAction" );
	if ( ac )
	    return ac;
    }
    return 0;
}

void FormWindow::killAccels( TQObject *top )
{
    TQObjectList *l = top->queryList( "TQAccel" );
    if ( !l )
	return;
    for ( TQObject *o = l->first(); o; o = l->next() )
	( (TQAccel*)o )->setEnabled( false );
    delete l;
}

DesignerFormWindow *FormWindow::iFace()
{
    if ( !iface )
	iface = new DesignerFormWindowImpl( this );
    return iface;
}

bool FormWindow::isCentralWidget( TQObject *w ) const
{
    if ( !::tqt_cast<TQMainWindow*>(mainContainer()) )
	return false;
    return w == ( (TQMainWindow*)mainContainer() )->centralWidget();
}

int FormWindow::layoutDefaultSpacing() const
{
    return defSpacing;
}

int FormWindow::layoutDefaultMargin() const
{
    return defMargin;
}

void FormWindow::setLayoutDefaultSpacing( int s )
{
    defSpacing = s;
}

void FormWindow::setLayoutDefaultMargin( int s )
{
    defMargin = s;
}

void FormWindow::setSpacingFunction( const TQString &funct )
{
    spacFunction = funct;
}

TQString FormWindow::spacingFunction() const
{
    return spacFunction;
}

void FormWindow::hasLayoutFunctions( bool b )
{
    hasLayoutFunc = b;
}

bool FormWindow::hasLayoutFunctions() const
{
    return hasLayoutFunc;
}

void FormWindow::setMarginFunction( const TQString &funct )
{
    margFunction = funct;
}

TQString FormWindow::marginFunction() const
{
    return margFunction;
}

FormFile *FormWindow::formFile() const
{
    return ff;
}

void FormWindow::setFormFile( FormFile *f )
{
    ff = f;
    if ( ff )
	connect( this, TQ_SIGNAL( modificationChanged(bool, const TQString&) ), ff, TQ_SLOT( formWindowChangedSomehow() ) );
}

bool FormWindow::canBeBuddy( const TQWidget *w ) const
{
    return w->focusPolicy() != TQWidget::NoFocus;
}
