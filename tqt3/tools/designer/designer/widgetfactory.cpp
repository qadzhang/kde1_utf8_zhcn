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

#include <ntqvariant.h>
#include "widgetfactory.h"
#include <widgetdatabase.h>
#include "metadatabase.h"
#include "mainwindow.h"
#include "formwindow.h"
#include "propertyeditor.h"
#include "layout.h"
#include "listboxeditorimpl.h"
#include "listvieweditorimpl.h"
#include "iconvieweditorimpl.h"
#include "formwindow.h"
#include "multilineeditorimpl.h"
#include "../interfaces/widgetinterface.h"
#ifndef TQT_NO_TABLE
#include "tableeditorimpl.h"
#endif
#include "project.h"
#include "menubareditor.h"
#include "popupmenueditor.h"

#include <ntqfeatures.h>

#include <ntqpixmap.h>
#include <ntqgroupbox.h>
#include <ntqiconview.h>
#ifndef TQT_NO_TABLE
#include <ntqtable.h>
#endif
#ifndef TQT_NO_SQL
#include <ntqdatatable.h>
#endif
#include <ntqdatetimeedit.h>
#include <ntqlineedit.h>
#include <ntqspinbox.h>
#include <ntqmultilineedit.h>
#include <ntqtextedit.h>
#include <ntqlabel.h>
#include <ntqlayout.h>
#include <ntqwidgetstack.h>
#include <ntqcombobox.h>
#include <ntqtabbar.h>
#include <ntqlistbox.h>
#include <ntqlistview.h>
#include <ntqobjectlist.h>
#include <ntqlcdnumber.h>
#include <ntqslider.h>
#include <ntqdial.h>
#include <ntqprogressbar.h>
#include <ntqtextview.h>
#include <ntqtextbrowser.h>
#include <ntqframe.h>
#include <ntqmetaobject.h>
#include <ntqwidgetstack.h>
#include <ntqwizard.h>
#include <ntqvaluelist.h>
#include <ntqtimer.h>
#include <ntqscrollbar.h>
#include <ntqmainwindow.h>
#include <ntqmenubar.h>
#include <ntqapplication.h>
#include <ntqsplitter.h>
#include <ntqtoolbox.h>
#include <ntqsizegrip.h>
#ifndef TQT_NO_SQL
#include "database.h"
#endif

#define NO_STATIC_COLORS
#include <globaldefs.h>
#include <ntqobject.h>

FormWindow *find_formwindow( TQWidget *w )
{
    if ( !w )
	return 0;
    for (;;) {
	if ( ::tqt_cast<FormWindow*>(w) )
	    return (FormWindow*)w;
	if ( !w->parentWidget() )
	    return 0;
	w = w->parentWidget();
    }
}

void TQLayoutWidget::paintEvent( TQPaintEvent* )
{
    TQPainter p ( this );
    p.setPen( red );
    p.drawRect( rect() );
}


TQDesignerTabWidget::TQDesignerTabWidget( TQWidget *parent, const char *name )
    : TQTabWidget( parent, name ), dropIndicator( 0 ), dragPage( 0 ), mousePressed( false )
{
    tabBar()->setAcceptDrops( true );
    tabBar()->installEventFilter( this );
}

int TQDesignerTabWidget::currentPage() const
{
    return tabBar()->currentTab();
}

void TQDesignerTabWidget::setCurrentPage( int i )
{
    tabBar()->setCurrentTab( i );
}

TQString TQDesignerTabWidget::pageTitle() const
{
    return ((TQTabWidget*)this)->tabLabel( TQTabWidget::currentPage() );
}

void TQDesignerTabWidget::setPageTitle( const TQString& title )
{
    changeTab( TQTabWidget::currentPage(), title );
}

void TQDesignerTabWidget::setPageName( const TQCString& name )
{
    if ( TQTabWidget::currentPage() )
	TQTabWidget::currentPage()->setName( name );
}

TQCString TQDesignerTabWidget::pageName() const
{
    if ( !TQTabWidget::currentPage() )
	return 0;
    return TQTabWidget::currentPage()->name();
}

int TQDesignerTabWidget::count() const
{
    return tabBar()->count();
}

bool TQDesignerTabWidget::eventFilter( TQObject *o, TQEvent *e )
{
    if ( o != tabBar() ) return false;

    switch ( e->type() ) {
    case TQEvent::MouseButtonPress: {
	mousePressed = true;
	TQMouseEvent *me = (TQMouseEvent*)e;
	pressPoint = me->pos();
    }
    break;
    case TQEvent::MouseMove: {
	TQMouseEvent *me = (TQMouseEvent*)e;
	if ( mousePressed && ( pressPoint - me->pos()).manhattanLength() > TQApplication::startDragDistance() ) {
	    TQTextDrag *drg = new TQTextDrag( TQString::number( (long) this ) , this );
	    mousePressed = false;
	    dragPage = TQTabWidget::currentPage();
	    dragLabel = TQTabWidget::tabLabel( dragPage );

	    int index = indexOf( dragPage );

	    removePage( dragPage );
	    if ( !drg->dragMove() ) {
		insertTab( dragPage, dragLabel, index );
		showPage( dragPage );
	    }
	    if ( dropIndicator )
		dropIndicator->hide();
	}
    }
    break;
    case TQEvent::DragLeave:	{
	if ( dropIndicator )
	    dropIndicator->hide();
    }
    break;
    case TQEvent::DragMove: {
	TQDragEnterEvent *de = (TQDragEnterEvent*) e;
	if ( TQTextDrag::canDecode( de ) ) {
	    TQString text;
	    TQTextDrag::decode( de, text );
	    if ( text == TQString::number( (long)this ) )
		de->accept();
	    else
		return false;
	}

	int index = 0;
	TQRect rect;
	for ( ; index < tabBar()->count(); index++ ) {
	    if ( tabBar()->tabAt( index )->rect().contains( de->pos() ) ) {
		rect = tabBar()->tabAt( index )->rect();
		break;
	    }
	}

	if ( index == tabBar()->count() -1 ) {
	    TQRect rect2 = rect;
	    rect2.setLeft( rect2.left() + rect2.width() / 2 );
	    if ( rect2.contains( de->pos() ) )
		index++;
	}

	if ( ! dropIndicator ) {
	    dropIndicator = new TQWidget( this );
	    dropIndicator->setBackgroundColor( red );
	}

	TQPoint pos;
	if ( index == tabBar()->count() )
	    pos = tabBar()->mapToParent( TQPoint( rect.x() + rect.width(), rect.y() ) );
	else
	    pos = tabBar()->mapToParent( TQPoint( rect.x(), rect.y() ) );

	dropIndicator->setGeometry( pos.x(), pos.y() , 3, rect.height() );
	dropIndicator->show();
    }
    break;
    case TQEvent::Drop: {
	TQDragEnterEvent *de = (TQDragEnterEvent*) e;
	if ( TQTextDrag::canDecode( de ) ) {
	    TQString text;
	    TQTextDrag::decode( de, text );
	    if ( text == TQString::number( (long)this ) ) {

		int newIndex = 0;
		for ( ; newIndex < tabBar()->count(); newIndex++ ) {
		    if ( tabBar()->tabAt( newIndex )->rect().contains( de->pos() ) )
			break;
		}

		if ( newIndex == tabBar()->count() -1 ) {
		    TQRect rect2 = tabBar()->tabAt( newIndex )->rect();
		    rect2.setLeft( rect2.left() + rect2.width() / 2 );
		    if ( rect2.contains( de->pos() ) )
			newIndex++;
		}

		int oldIndex = 0;
		for ( ; oldIndex < tabBar()->count(); oldIndex++ ) {
		    if ( tabBar()->tabAt( oldIndex )->rect().contains( pressPoint ) )
			break;
		}

		FormWindow *fw = find_formwindow( this );
		MoveTabPageCommand *cmd =
		    new MoveTabPageCommand( tr( "Move Tab Page" ), fw, this,
					    dragPage, dragLabel, newIndex, oldIndex );
		fw->commandHistory()->addCommand( cmd );
		cmd->execute();
		de->accept();
	    }
	}
    }
    break;
    default:
	break;
    }
    return false;
}


TQDesignerWidgetStack::TQDesignerWidgetStack( TQWidget *parent, const char *name )
    : TQWidgetStack( parent, name )
{
    prev = new TQToolButton( TQt::LeftArrow, this, "designer_wizardstack_button" );
    prev->setAutoRaise( true );
    prev->setAutoRepeat( true );
    prev->setSizePolicy( TQSizePolicy( TQSizePolicy::Ignored, TQSizePolicy::Ignored ) );
    next = new TQToolButton( TQt::RightArrow, this, "designer_wizardstack_button" );
    next->setAutoRaise( true );
    next->setAutoRepeat( true );
    next->setSizePolicy( TQSizePolicy( TQSizePolicy::Ignored, TQSizePolicy::Ignored ) );
    connect( prev, TQ_SIGNAL( clicked() ), this, TQ_SLOT( prevPage() ) );
    connect( next, TQ_SIGNAL( clicked() ), this, TQ_SLOT( nextPage() ) );
    updateButtons();
}

void TQDesignerWidgetStack::updateButtons()
{
    prev->setGeometry( width() - 31, 1, 15, 15 );
    next->setGeometry( width() - 16, 1, 15, 15 );
    prev->show();
    next->show();
    prev->raise();
    next->raise();
}

void TQDesignerWidgetStack::prevPage()
{
    setCurrentPage( currentPage() - 1 );
}

void TQDesignerWidgetStack::nextPage()
{
    setCurrentPage( currentPage() + 1 );
}

int TQDesignerWidgetStack::currentPage() const
{
    TQDesignerWidgetStack* that = (TQDesignerWidgetStack*) this;
    return that->pages.find( visibleWidget() );
}

void TQDesignerWidgetStack::setCurrentPage( int i )
{
    // help next/prev page commands
    if ( i < 0 )
	i += count();
    if ( i >= count() )
	i -= count();

    if ( i < 0 || i >= count() )
	return;
    raiseWidget( pages.at( i ) );
    updateButtons();
}

TQCString TQDesignerWidgetStack::pageName() const
{
    if ( !visibleWidget() )
	return 0;
    return visibleWidget()->name();
}

void TQDesignerWidgetStack::setPageName( const TQCString& name )
{
    if ( visibleWidget() )
	visibleWidget()->setName( name );
}

int TQDesignerWidgetStack::count() const
{
    return pages.count();
}

TQWidget* TQDesignerWidgetStack::page( int i ) const
{
    if ( i < 0 || i >= count() )
	return 0;
    TQDesignerWidgetStack* that = (TQDesignerWidgetStack*) this;
    return that->pages.at( i );
}


int TQDesignerWidgetStack::insertPage( TQWidget *p, int i )
{
    if ( i < 0 )
	pages.append( p );
    else
	pages.insert( (uint) i, p );
    addWidget( p );
    p->show();
    raiseWidget( p );
    TQApplication::sendPostedEvents();
    updateButtons();
    return pages.find( p );
}

int TQDesignerWidgetStack::removePage( TQWidget *p )
{
    int i = pages.find( p );
    pages.remove( p );
    removeWidget( p );
    setCurrentPage( 0 );
    updateButtons();
    return i;
}


int TQDesignerWizard::currentPageNum() const
{
    for ( int i = 0; i < pageCount(); ++i ) {
	if ( page( i ) == currentPage() )
	    return i;
    }
    return 0;
}





void TQDesignerWizard::setCurrentPage( int i )
{
    if ( i < currentPageNum() ) {
	while ( i < currentPageNum() ) {
	    if ( currentPageNum() == 0 )
		break;
	    back();
	}

    } else {
	while ( i > currentPageNum() ) {
	    if ( currentPageNum() == pageCount() - 1 )
		break;
	    next();
	}
    }
}

TQString TQDesignerWizard::pageTitle() const
{
    return title( currentPage() );
}

void TQDesignerWizard::setPageTitle( const TQString& title )
{
    setTitle( currentPage(), title );
}

void TQDesignerWizard::setPageName( const TQCString& name )
{
    if ( TQWizard::currentPage() )
	TQWizard::currentPage()->setName( name );
}

TQCString TQDesignerWizard::pageName() const
{
    if ( !TQWizard::currentPage() )
	return 0;
    return TQWizard::currentPage()->name();
}

int TQDesignerWizard::pageNum( TQWidget *p )
{
    for ( int i = 0; i < pageCount(); ++i ) {
	if ( page( i ) == p )
	    return i;
    }
    return -1;
}

void TQDesignerWizard::addPage( TQWidget *p, const TQString &t )
{
    TQWizard::addPage( p, t );
    if ( removedPages.find( p ) )
	removedPages.remove( p );
}

void TQDesignerWizard::removePage( TQWidget *p )
{
    TQWizard::removePage( p );
    removedPages.insert( p, p );
}

void TQDesignerWizard::insertPage( TQWidget *p, const TQString &t, int index )
{
    TQWizard::insertPage( p, t, index );
    if ( removedPages.find( p ) )
	removedPages.remove( p );
}

TQMap< int, TQMap< TQString, TQVariant> > *defaultProperties = 0;
TQMap< int, TQStringList > *changedProperties = 0;

/*!
  \class WidgetFactory widgetfactory.h
  \brief Set of static functions for creating widgets, layouts and do other stuff

  The widget factory offers functions to create widgets, create and
  delete layouts find out other details - all based on the
  WidgetDatabase's data. So the functions that use ids use the same
  ids as in the WidgetDatabase.
*/


void WidgetFactory::saveDefaultProperties( TQObject *w, int id )
{
    TQMap< TQString, TQVariant> propMap;
    TQStrList lst = w->metaObject()->propertyNames( true );
    for ( uint i = 0; i < lst.count(); ++i ) {
	TQVariant var = w->property( lst.at( i ) );
	if ( !var.isValid() && qstrcmp( "pixmap", lst.at( i ) ) == 0 )
	    var = TQVariant( TQPixmap() );
	else if ( !var.isValid() && qstrcmp( "iconSet", lst.at( i ) ) == 0 )
	    var = TQVariant( TQIconSet() );
	propMap.replace( lst.at( i ), var );
    }
    defaultProperties->replace( id, propMap );
}

void WidgetFactory::saveChangedProperties( TQObject *w, int id )
{
    TQStringList l = MetaDataBase::changedProperties( w );
    changedProperties->insert( id, l );
}

/*!  Creates a widget of the type which is registered as \a id as
  child of \a parent. The \a name is optional. If \a init is true, the
  widget is initialized with some defaults, else the plain widget is
  created.
*/

TQWidget *WidgetFactory::create( int id, TQWidget *parent, const char *name, bool init, const TQRect *r, TQt::Orientation orient )
{
    TQString n = WidgetDatabase::className( id );
    if ( n.isEmpty() )
	return 0;

    if ( !defaultProperties ) {
	defaultProperties = new TQMap< int, TQMap< TQString, TQVariant> >();
	changedProperties = new TQMap< int, TQStringList >();
    }

    TQWidget *w = 0;
    TQString str = WidgetDatabase::createWidgetName( id );
    const char *s = str.latin1();
    w = createWidget( n, parent, name ? name : s, init, r, orient );
    if ( ::tqt_cast<TQScrollView*>(w) )
	( (TQScrollView*)w )->disableSizeHintCaching();
    if ( !w && WidgetDatabase::isCustomWidget( id ) )
	w = createCustomWidget( parent, name ? name : s, MetaDataBase::customWidget( id ) );
    if ( !w )
	return 0;
    MetaDataBase::addEntry( w );

    if ( !defaultProperties->contains( id ) )
	saveDefaultProperties( w, id );
    if ( !changedProperties->contains( id ) )
	saveChangedProperties( w, id );

    return w;
}

/*!  Creates a layout on the widget \a widget of the type \a type
  which can be \c HBox, \c VBox or \c Grid.
*/

TQLayout *WidgetFactory::createLayout( TQWidget *widget, TQLayout *layout, LayoutType type )
{
    int spacing = MainWindow::self->currentLayoutDefaultSpacing();
    int margin = 0;

    int metaspacing = MetaDataBase::spacing( widget );
    int metamargin = MetaDataBase::margin( widget );

    if ( ::tqt_cast<TQLayoutWidget*>(widget) &&
	 ( WidgetDatabase::isContainer( WidgetDatabase::idFromClassName( WidgetFactory::classNameOf( widget ) ) ) ||
	   ( widget && ::tqt_cast<FormWindow*>(widget->parentWidget()) ) ) )
	margin = MainWindow::self->currentLayoutDefaultMargin();

    if ( !layout && ::tqt_cast<TQTabWidget*>(widget) )
	widget = ((TQTabWidget*)widget)->currentPage();

    if ( !layout && ::tqt_cast<TQWizard*>(widget) )
	widget = ((TQWizard*)widget)->currentPage();

    if ( !layout && ::tqt_cast<TQMainWindow*>(widget) )
	widget = ((TQMainWindow*)widget)->centralWidget();

    if ( !layout && ::tqt_cast<TQWidgetStack*>(widget) )
	widget = ((TQWidgetStack*)widget)->visibleWidget();

    if ( !layout && ::tqt_cast<TQToolBox*>(widget) )
	widget = ((TQToolBox*)widget)->currentItem();

    MetaDataBase::addEntry( widget );

    TQLayout *l = 0;
    int align = 0;
    if ( !layout && ::tqt_cast<TQGroupBox*>(widget) ) {
	TQGroupBox *gb = (TQGroupBox*)widget;
	gb->setColumnLayout( 0, TQt::Vertical );
	layout = gb->layout();
	layout->setMargin( 0 );
	layout->setSpacing( 0 );
	switch ( type ) {
	case HBox:
	    l = new TQHBoxLayout( layout );
	    break;
	case VBox:
	    l = new TQVBoxLayout( layout );
	    break;
	case Grid:
	    l = new TQDesignerGridLayout( layout );
	    break;
	default:
	    return 0;
	}
	align = TQt::AlignTop;
	MetaDataBase::setMargin( gb, metamargin );
	MetaDataBase::setSpacing( gb, metaspacing );
    } else {
	if ( layout ) {
	    switch ( type ) {
	    case HBox:
		l = new TQHBoxLayout( layout );
		break;
	    case VBox:
		l = new TQVBoxLayout( layout );
		break;
	    case Grid:
		l = new TQDesignerGridLayout( layout );
		break;
	    default:
		return 0;
	    }
	    MetaDataBase::addEntry( l );
	    l->setSpacing( spacing );
	    l->setMargin( margin );
	} else {
	    switch ( type ) {
	    case HBox:
		l = new TQHBoxLayout( widget );
		break;
	    case VBox:
		l = new TQVBoxLayout( widget );
		break;
	    case Grid:
		l = new TQDesignerGridLayout( widget );
		break;
	    default:
		return 0;
	    }
	    MetaDataBase::addEntry( l );
	    if ( widget ) {
		MetaDataBase::setMargin( widget, metamargin );
		MetaDataBase::setSpacing( widget, metaspacing );
	    } else {
		l->setMargin( margin );
		l->setSpacing( spacing );
	    }
	}
    }
    l->setAlignment( align );
    MetaDataBase::addEntry( l );
    return l;
}

void WidgetFactory::deleteLayout( TQWidget *widget )
{
    if ( !widget )
	return;

    if ( ::tqt_cast<TQTabWidget*>(widget) )
	widget = ((TQTabWidget*)widget)->currentPage();
    if ( ::tqt_cast<TQWizard*>(widget) )
	widget = ((TQWizard*)widget)->currentPage();
    if ( ::tqt_cast<TQMainWindow*>(widget) )
	widget = ((TQMainWindow*)widget)->centralWidget();
    if ( ::tqt_cast<TQWidgetStack*>(widget) )
	widget = ((TQWidgetStack*)widget)->visibleWidget();
    if ( ::tqt_cast<TQToolBox*>(widget) )
	widget = ((TQToolBox*)widget)->currentItem();
    delete widget->layout();
}

/*!  Factory functions for creating a widget of the type \a className
  as child of \a parent with the name \a name.

  If \a init is true, some initial default properties are set. This
  has to be in sync with the initChangedProperties() function!
*/

TQWidget *WidgetFactory::createWidget( const TQString &className, TQWidget *parent, const char *name, bool init,
				      const TQRect *r, TQt::Orientation orient )
{
    if ( className == "TQPushButton" ) {
	TQPushButton *b = 0;
	if ( init ) {
	    b = new TQDesignerPushButton( parent, name );
	    b->setText( TQString::fromLatin1( name ) );
	} else {
	    b = new TQDesignerPushButton( parent, name );
	}
	TQWidget *w = find_formwindow( b );
	b->setAutoDefault( w && ::tqt_cast<TQDialog*>(((FormWindow*)w)->mainContainer()) );
	return b;
    } else if ( className == "TQToolButton" ) {
	if ( init ) {
	    TQDesignerToolButton *tb = new TQDesignerToolButton( parent, name );
	    if ( ::tqt_cast<TQToolBox*>(widgetOfContainer(parent))) {
		tb->setUsesTextLabel(true);
		tb->setTextLabel("...");
		tb->setAutoRaise(true);
		tb->setTextPosition(TQToolButton::BesideIcon);
	    } else {
		tb->setText( "..." );
	    }
	    return tb;
	}
	return new TQDesignerToolButton( parent, name );
    } else if ( className == "TQCheckBox" ) {
	if ( init ) {
	    TQDesignerCheckBox *cb = new TQDesignerCheckBox( parent, name );
	    cb->setText( TQString::fromLatin1( name ) );
	    return cb;
	}
	return new TQDesignerCheckBox( parent, name );
    } else if ( className == "TQRadioButton" ) {
	if ( init ) {
	    TQDesignerRadioButton *rb = new TQDesignerRadioButton( parent, name );
	    rb->setText( TQString::fromLatin1( name ) );
	    return rb;
	}
	return new TQDesignerRadioButton( parent, name );
    } else if ( className == "TQGroupBox" ) {
	if ( init )
	    return new TQGroupBox( TQString::fromLatin1( name ), parent, name );
	return new TQGroupBox( parent, name );
    } else if ( className == "TQButtonGroup" ) {
	if ( init )
	    return new TQButtonGroup( TQString::fromLatin1( name ), parent, name );
	return new TQButtonGroup( parent, name );
    } else if ( className == "TQIconView" ) {
#if !defined(TQT_NO_ICONVIEW)
	TQIconView* iv = new TQIconView( parent, name );
	if ( init )
	    (void) new TQIconViewItem( iv, MainWindow::tr( "New Item" ) );
	return iv;
#else
	return 0;
#endif
    } else if ( className == "TQTable" ) {
#if !defined(TQT_NO_TABLE)
	if ( init )
	    return new TQTable( 3, 3, parent, name );
	return new TQTable( parent, name );
#else
	return 0;
#endif
#ifndef TQT_NO_SQL
    } else if ( className == "TQDataTable" ) {
	return new TQDataTable( parent, name );
#endif //TQT_NO_SQL
    } else if ( className == "TQDateEdit" ) {
	return new TQDateEdit( parent, name );
    } else if ( className == "TQTimeEdit" ) {
	return new TQTimeEdit( parent, name );
    } else if ( className == "TQDateTimeEdit" ) {
	return new TQDateTimeEdit( parent, name );
    }
    else if ( className == "TQListBox" ) {
	TQListBox* lb = new TQListBox( parent, name );
	if ( init ) {
	    lb->insertItem( MainWindow::tr( "New Item" ) );
	    lb->setCurrentItem( 0 );
	}
	return lb;
    } else if ( className == "TQListView" ) {
	TQListView *lv = new TQListView( parent, name );
	lv->setSorting( -1 );
	if ( init ) {
	    lv->addColumn( MainWindow::tr( "Column 1" ) );
	    lv->setCurrentItem( new TQListViewItem( lv, MainWindow::tr( "New Item" ) ) );
	}
	return lv;
    } else if ( className == "TQLineEdit" )
	return new TQLineEdit( parent, name );
    else if ( className == "TQSpinBox" )
	return new TQSpinBox( parent, name );
    else if ( className == "TQSplitter" )
	return new TQSplitter( parent, name );
    else if ( className == "TQMultiLineEdit" )
	return new TQMultiLineEdit( parent, name );
    else if ( className == "TQTextEdit" )
	return new TQTextEdit( parent, name );
    else if ( className == "TQLabel"  || className == "TextLabel" ) {
	TQDesignerLabel *l = new TQDesignerLabel( parent, name );
	if ( init ) {
	    l->setText( TQString::fromLatin1( name ) );
	    MetaDataBase::addEntry( l );
	    MetaDataBase::setPropertyChanged( l, "text", true );
	}
	return l;
    } else if ( className == "PixmapLabel" ) {
	TQDesignerLabel *l = new TQDesignerLabel( parent, name );
	if ( init ) {
	    l->setPixmap( TQPixmap::fromMimeSource( "designer_qtlogo.png" ) );
	    l->setScaledContents( true );
	    MetaDataBase::addEntry( l );
	    MetaDataBase::setPropertyChanged( l, "pixmap", true );
	    MetaDataBase::setPropertyChanged( l, "scaledContents", true );
	}
	return l;
    } else if ( className == "TQLayoutWidget" )
	return new TQLayoutWidget( parent, name );
    else if ( className == "TQTabWidget" ) {
	TQTabWidget *tw = new TQDesignerTabWidget( parent, name );
	if ( init ) {
	    FormWindow *fw = find_formwindow( parent );
	    TQWidget *w = fw ? new TQDesignerWidget( fw, tw, "tab" ) : new TQWidget( tw, "tab" );
	    tw->addTab( w, MainWindow::tr("Tab 1") );
	    MetaDataBase::addEntry( w );
	    w = fw ? new TQDesignerWidget( fw, tw, "tab" ) : new TQWidget( tw, "tab" );
	    tw->addTab( w, MainWindow::tr("Tab 2") );
	    MetaDataBase::addEntry( tw );
	    MetaDataBase::addEntry( w );
	}
	return tw;
    } else if ( className == "TQWidgetStack" ) {
	TQDesignerWidgetStack *ws = new TQDesignerWidgetStack( parent, name );
	if ( init ) {
	    FormWindow *fw = find_formwindow( parent );
	    TQWidget *w = fw ? new TQDesignerWidget( fw, ws, "WStackPage" ) : new TQWidget( ws, "WStackPage" );
	    ws->insertPage( w );
	    MetaDataBase::addEntry( w );
	    MetaDataBase::addEntry( ws );
	}
	return ws;
    } else if ( className == "TQComboBox" ) {
	return new TQComboBox( false, parent, name );
    } else if ( className == "TQWidget" ) {
	if ( parent &&
	     ( ::tqt_cast<FormWindow*>(parent) ||
	       ::tqt_cast<TQWizard*>(parent) ||
	       ::tqt_cast<TQTabWidget*>(parent) ||
	       ::tqt_cast<TQWidgetStack*>(parent) ||
	       ::tqt_cast<TQToolBox*>(parent) ||
	       ::tqt_cast<TQMainWindow*>(parent) ) ) {
	    FormWindow *fw = find_formwindow( parent );
	    if ( fw ) {
		TQDesignerWidget *dw = new TQDesignerWidget( fw, parent, name );
		MetaDataBase::addEntry( dw );
		return dw;
	    }
	}
	return new TQWidget( parent, name );
    } else if ( className == "TQDialog" ) {
	TQDialog *dia = 0;
	if ( ::tqt_cast<FormWindow*>(parent) )
	    dia = new TQDesignerDialog( (FormWindow*)parent, parent, name );
	else
	    dia = new TQDialog( parent, name );
	if ( parent )
	    dia->reparent( parent, TQPoint( 0, 0 ), true );
	return dia;
    } else if ( className == "TQWizard" ) {
	TQWizard *wiz = new TQDesignerWizard( parent, name );
	if ( parent )
	    wiz->reparent( parent, TQPoint( 0, 0 ), true );
	if ( init && ::tqt_cast<FormWindow*>(parent) ) {
	    TQDesignerWidget *dw = new TQDesignerWidget( (FormWindow*)parent, wiz, "WizardPage" );
	    MetaDataBase::addEntry( dw );
	    wiz->addPage( dw, FormWindow::tr( "Page" ) );
	    TQTimer::singleShot( 0, wiz, TQ_SLOT( next() ) );
	}
	return wiz;
    } else if ( className == "Spacer" ) {
	Spacer *s = new Spacer( parent, name );
	MetaDataBase::addEntry( s );
	MetaDataBase::setPropertyChanged( s, "orientation", true );
	MetaDataBase::setPropertyChanged( s, "sizeType", true );
	if ( !r )
	    return s;
	if ( !r->isValid() || ( r->width() < 2 && r->height() < 2 ) )
	    s->setOrientation( orient );
	else if ( r->width() < r->height() )
	    s->setOrientation( TQt::Vertical );
	else
	    s->setOrientation( TQt::Horizontal );
	return s;
    } else if ( className == "TQLCDNumber" )
	return new TQLCDNumber( parent, name );
    else if ( className == "TQProgressBar" )
	return new TQProgressBar( parent, name );
    else if ( className == "TQTextView" )
	return new TQTextView( parent, name );
    else if ( className == "TQTextBrowser" )
	return new TQTextBrowser( parent, name );
    else if ( className == "TQDial" )
	return new TQDial( parent, name );
    else if ( className == "TQSlider" ) {
	TQSlider *s = new TQSlider( parent, name );
	if ( !r )
	    return s;
	if ( !r->isValid() || ( r->width() < 2 && r->height() < 2 ) )
	    s->setOrientation( orient );
	else if ( r->width() > r->height() )
	    s->setOrientation( TQt::Horizontal );
	MetaDataBase::addEntry( s );
	MetaDataBase::setPropertyChanged( s, "orientation", true );
	return s;
    } else if ( className == "TQScrollBar" ) {
	TQScrollBar *s = new TQScrollBar( parent, name );
	if ( !r )
	    return s;
	if ( !r->isValid() || ( r->width() < 2 && r->height() < 2 ) )
	    s->setOrientation( orient );
	else if ( r->width() > r->height() )
	    s->setOrientation( TQt::Horizontal );
	MetaDataBase::addEntry( s );
	MetaDataBase::setPropertyChanged( s, "orientation", true );
	return s;
    } else if ( className == "TQFrame" ) {
	if ( !init )
	    return new TQFrame( parent, name );
	TQFrame *f = new TQFrame( parent, name );
	f->setFrameStyle( TQFrame::StyledPanel | TQFrame::Raised );
	return f;
    } else if ( className == "Line" ) {
	Line *l = new Line( parent, name );
	MetaDataBase::addEntry( l );
	MetaDataBase::setPropertyChanged( l, "orientation", true );
	MetaDataBase::setPropertyChanged( l, "frameShadow", true );
	MetaDataBase::setPropertyChanged( l, "frameShape", true );
	if ( !r )
	    return l;
	if ( !r->isValid() || ( r->width() < 2 && r->height() < 2 ) )
	    l->setOrientation( orient );
	else if ( r->width() < r->height() )
	    l->setOrientation( TQt::Vertical );
	return l;
    } else if ( className == "TQMainWindow" ) {
	TQMainWindow *mw = new TQMainWindow( parent, name, 0 );
	mw->setDockEnabled( TQt::DockMinimized, false );
	TQDesignerWidget *dw = new TQDesignerWidget( (FormWindow*)parent, mw, "central widget" );
	mw->setDockMenuEnabled( false );
	MetaDataBase::addEntry( dw );
	mw->setCentralWidget( dw );
	(void)mw->statusBar();
	dw->show();
	return mw;
    } else if ( className == "TQToolBox" ) {
	if ( !init )
	    return new TQDesignerToolBox( parent, name );
	TQToolBox *tb = new TQDesignerToolBox( parent, name );
	FormWindow *fw = find_formwindow( parent );
	TQWidget *w = fw ? new TQDesignerWidget( fw, tb, "page1" ) :
		     new TQWidget( tb, "page1" );
	tb->addItem( w, MainWindow::tr("Page 1") );
	MetaDataBase::addEntry( w );
	w = fw ? new TQDesignerWidget( fw, tb, "page2" ) : new TQWidget( tb, "page2" );
	tb->addItem( w, MainWindow::tr("Page 2") );
	MetaDataBase::addEntry( tb );
	MetaDataBase::addEntry( w );
	return tb;
    }
#ifndef TQT_NO_SQL
    else if ( className == "TQDataBrowser" ) {
	TQWidget *w = new TQDesignerDataBrowser( parent, name );
	if ( parent )
	    w->reparent( parent, TQPoint( 0, 0 ), true );
	return w;
    } else if ( className == "TQDataView" ) {
	TQWidget *w = new TQDesignerDataView( parent, name );
	if ( parent )
	    w->reparent( parent, TQPoint( 0, 0 ), true );
	return w;
    }
#endif

    WidgetInterface *iface = 0;
    widgetManager()->queryInterface( className, &iface );
    if ( !iface )
	return 0;

    TQWidget *w = iface->create( className, parent, name );
#ifdef QT_CONTAINER_CUSTOM_WIDGETS
    if ( init && WidgetDatabase::isCustomPluginWidget( WidgetDatabase::idFromClassName( className ) ) ) {
	TQWidgetContainerInterfacePrivate *iface2 = 0;
	iface->queryInterface( IID_QWidgetContainer, (TQUnknownInterface**)&iface2 );
	if ( iface2 ) {
	    iface2->addPage( className, w, "Page", -1 );
	    iface2->release();
	}
    }
#endif // QT_CONTAINER_CUSTOM_WIDGETS
    iface->release();
    return w;
}



/*!  Find out which type the layout of the widget is. Returns \c HBox,
  \c VBox, \c Grid or \c NoLayout.  \a layout points to this
  TQWidget::layout() of \a w or to 0 after the function call.
*/

WidgetFactory::LayoutType WidgetFactory::layoutType( TQWidget *w, TQLayout *&layout )
{
    layout = 0;

    if ( ::tqt_cast<TQTabWidget*>(w) )
	w = ((TQTabWidget*)w)->currentPage();
    if ( ::tqt_cast<TQWizard*>(w) )
	w = ((TQWizard*)w)->currentPage();
    if ( ::tqt_cast<TQMainWindow*>(w) )
	w = ((TQMainWindow*)w)->centralWidget();
    if ( ::tqt_cast<TQWidgetStack*>(w) )
	w = ((TQWidgetStack*)w)->visibleWidget();
    if ( ::tqt_cast<TQToolBox*>(w) )
	w = ((TQToolBox*)w)->currentItem();

    if ( ::tqt_cast<TQSplitter*>(w) )
	return ( (TQSplitter*)w )->orientation() == Horizontal ? HBox : VBox;

    if ( !w || !w->layout() )
	return NoLayout;
    TQLayout *lay = w->layout();

    if ( ::tqt_cast<TQGroupBox*>(w) ) {
	TQObjectList *l = lay->queryList( "TQLayout" );
	if ( l && l->first() )
	    lay = (TQLayout*)l->first();
	delete l;
    }
    layout = lay;

    if ( ::tqt_cast<TQHBoxLayout*>(lay) )
	return HBox;
    else if ( ::tqt_cast<TQVBoxLayout*>(lay) )
	return VBox;
    else if ( ::tqt_cast<TQGridLayout*>(lay) )
	return Grid;
    return NoLayout;
}

/*!
  \overload
*/
WidgetFactory::LayoutType WidgetFactory::layoutType( TQLayout *layout )
{
    if ( ::tqt_cast<TQHBoxLayout*>(layout) )
	return HBox;
    else if ( ::tqt_cast<TQVBoxLayout*>(layout) )
	return VBox;
    else if ( ::tqt_cast<TQGridLayout*>(layout) )
	return Grid;
    return NoLayout;
}

/*!
  \overload
*/
WidgetFactory::LayoutType WidgetFactory::layoutType( TQWidget *w )
{
    TQLayout *l = 0;
    return layoutType( w, l );
}


TQWidget *WidgetFactory::layoutParent( TQLayout *layout )
{
    TQObject *o = layout;
    while ( o ) {
	if ( o->isWidgetType() )
	    return (TQWidget*)o;
	o = o->parent();
    }
    return 0;
}

/*!  Returns the widget into which children should be inserted when \a
  w is a container known to the designer.

  Usually that is \a w itself, sometimes it is different (e.g. a
  tabwidget is known to the designer as a container but the child
  widgets should be inserted into the current page of the
  tabwidget. So in this case this function returns the current page of
  the tabwidget.)
 */
TQWidget* WidgetFactory::containerOfWidget( TQWidget *w )
{
    if ( !w )
	return w;
    if ( ::tqt_cast<TQTabWidget*>(w) )
	return ((TQTabWidget*)w)->currentPage();
    if ( ::tqt_cast<TQWizard*>(w) )
	return ((TQWizard*)w)->currentPage();
    if ( ::tqt_cast<TQWidgetStack*>(w) )
	return ((TQWidgetStack*)w)->visibleWidget();
    if ( ::tqt_cast<TQToolBox*>(w) )
	return ((TQToolBox*)w)->currentItem();
    if ( ::tqt_cast<TQMainWindow*>(w) )
	return ((TQMainWindow*)w)->centralWidget();
#ifdef QT_CONTAINER_CUSTOM_WIDGETS
    if ( !WidgetDatabase::isCustomPluginWidget( WidgetDatabase::idFromClassName( classNameOf( w ) ) ) )
	return w;
    WidgetInterface *iface = 0;
    widgetManager()->queryInterface( classNameOf( w ), &iface );
    if ( !iface )
	return w;
    TQWidgetContainerInterfacePrivate *iface2 = 0;
    iface->queryInterface( IID_QWidgetContainer, (TQUnknownInterface**)&iface2 );
    if ( !iface2 )
	return w;
    TQWidget *c = iface2->containerOfWidget( w->className(), w );
    iface2->release();
    iface->release();
    if ( c )
	return c;
#endif // QT_CONTAINER_CUSTOM_WIDGETS
    return w;
}

/*!  Returns the actual designer widget of the container \a w. This is
  normally \a w itself, but might be a parent or grand parent of \a w
  (e.g. when working with a tabwidget and \a w is the container which
  contains and layouts childs, but the actual widget known to the
  designer is the tabwidget which is the parent of \a w. So this
  function returns the tabwidget then.)
*/

TQWidget* WidgetFactory::widgetOfContainer( TQWidget *w )
{
    if ( w && ::tqt_cast<TQWidgetStack*>(w->parentWidget()) )
	w = w->parentWidget();
    if ( w->parentWidget() && w->parentWidget()->parentWidget() &&
	 w->parentWidget()->parentWidget()->parentWidget() &&
	 ::tqt_cast<TQToolBox*>(w->parentWidget()->parentWidget()->parentWidget()) )
	return w->parentWidget()->parentWidget()->parentWidget();
    while ( w ) {
	int id = WidgetDatabase::idFromClassName( WidgetFactory::classNameOf( w ) );
	if ( WidgetDatabase::isContainer( id ) ||
	     ( w && ::tqt_cast<FormWindow*>(w->parentWidget()) ) )
	    return w;
	w = w->parentWidget();
    }
    return w;
}

/*!
  Returns whether \a o is a passive interactor or not.
 */

bool WidgetFactory::lastWasAPassiveInteractor = false;
TQGuardedPtr<TQObject> *WidgetFactory::lastPassiveInteractor = new TQGuardedPtr<TQObject>();

bool WidgetFactory::isPassiveInteractor( TQObject* o )
{
    if ( lastPassiveInteractor && *lastPassiveInteractor && (TQObject*)(*lastPassiveInteractor) == o )
	return lastWasAPassiveInteractor;
    lastWasAPassiveInteractor = false;
    (*lastPassiveInteractor) = o;
    if ( TQApplication::activePopupWidget() ) // if a popup is open, we have to make sure that this one is closed, else X might do funny things
	return ( lastWasAPassiveInteractor = true );

    if ( ::tqt_cast<TQTabBar*>(o) )
	return ( lastWasAPassiveInteractor = true );
    else if ( ::tqt_cast<TQSizeGrip*>(o) )
	return ( lastWasAPassiveInteractor = true );
    else if ( ::tqt_cast<TQButton*>(o) &&
	      ( ::tqt_cast<TQTabBar*>(o->parent()) || ::tqt_cast<TQToolBox*>(o->parent()) ) )
	return ( lastWasAPassiveInteractor = true );
    else if ( ::tqt_cast<TQPushButton*>(o) && ::tqt_cast<TQWizard*>(o->parent()) )
	return ( lastWasAPassiveInteractor = true );
    else if ( ::tqt_cast<TQMenuBar*>(o) && ::tqt_cast<TQMainWindow*>(o->parent()) )
	return ( lastWasAPassiveInteractor = true );
//    else if ( ::tqt_cast<TQDockWindowHandle*>(o) )
    else if ( o->inherits( "TQDockWindowHandle" ) )
	return ( lastWasAPassiveInteractor = true );
//    else if ( ::tqt_cast<TQHideDock*>(o) )
    else if ( o->inherits( "TQHideDock" ) )
	return ( lastWasAPassiveInteractor = true );
    else if ( qstrcmp( o->name(), "designer_wizardstack_button" ) == 0 )
	return ( lastWasAPassiveInteractor = true );
#ifdef QT_CONTAINER_CUSTOM_WIDGETS
    if ( !o->isWidgetType() )
	return ( lastWasAPassiveInteractor = false );
    WidgetInterface *iface = 0;
    TQWidget *w = (TQWidget*)o;
    while ( !iface && w && !::tqt_cast<FormWindow*>(w) ) {
	widgetManager()->queryInterface( classNameOf( w ), &iface );
	w = w->parentWidget();
    }
    if ( !iface )
	return ( lastWasAPassiveInteractor = false );
    TQWidgetContainerInterfacePrivate *iface2 = 0;
    iface->queryInterface( IID_QWidgetContainer, (TQUnknownInterface**)&iface2 );
    if ( !iface2 )
	return ( lastWasAPassiveInteractor = false );
    TQWidget *fw = MainWindow::self->isAFormWindowChild( (TQWidget*)o );
    if ( !fw )
	return ( lastWasAPassiveInteractor = false );
    TQWidget *dw = ( (FormWindow*)fw )->designerWidget( (TQWidget*)o );
    if ( !dw )
	return ( lastWasAPassiveInteractor = false );
    lastWasAPassiveInteractor = iface2->isPassiveInteractor( dw->className(), (TQWidget*)o );
    iface2->release();
    iface->release();
#endif // QT_CONTAINER_CUSTOM_WIDGETS
    return lastWasAPassiveInteractor;
}


/*!
  Returns the class name of object \a o that should be used for externally (i.e. for saving)
 */
const char* WidgetFactory::classNameOf( TQObject* o )
{
    if ( o->isA( "PropertyObject" ) )
	return o->className();
    if (WidgetDatabase::isCustomPluginWidget(WidgetDatabase::idFromClassName(o->className())))
        return o->className();
    else if ( ::tqt_cast<TQDesignerTabWidget*>(o) )
	return "TQTabWidget";
    else if ( ::tqt_cast<TQDesignerWidgetStack*>(o) )
	return "TQWidgetStack";
    else if ( ::tqt_cast<TQWidgetStack*>(o) )
	return "TQWeDoNotWantToBreakTabWidget";
    else if ( ::tqt_cast<TQDesignerDialog*>(o) )
	return "TQDialog";
    else if ( ::tqt_cast<TQDesignerWidget*>(o) )
	return "TQWidget";
    else if ( o->inherits( "CustomWidget" ) )
	return ( (CustomWidget*)o )->realClassName().latin1();
    else if ( ::tqt_cast<TQDesignerLabel*>(o) )
	return "TQLabel";
    else if ( ::tqt_cast<TQDesignerWizard*>(o) )
	return "TQWizard";
    else if ( ::tqt_cast<TQDesignerPushButton*>(o) )
	return "TQPushButton";
    else if ( ::tqt_cast<TQDesignerToolButton*>(o) )
	return "TQToolButton";
    else if ( ::tqt_cast<TQDesignerRadioButton*>(o) )
	return "TQRadioButton";
    else if ( ::tqt_cast<TQDesignerCheckBox*>(o) )
	return "TQCheckBox";
    else if ( ::tqt_cast<MenuBarEditor*>(o) )
	return "TQMenuBar";
    else if ( ::tqt_cast<TQDesignerToolBar*>(o) )
	return "TQToolBar";
    else if ( ::tqt_cast<TQDesignerAction*>(o) )
	return "TQAction";
    else if ( ::tqt_cast<TQDesignerActionGroup*>(o) )
	return "TQActionGroup";
    else if ( ::tqt_cast<PopupMenuEditor*>(o) )
	return "TQPopupMenu";
    else if ( ::tqt_cast<TQDesignerToolBox*>(o) )
	return "TQToolBox";
#ifndef TQT_NO_SQL
    else if ( ::tqt_cast<TQDesignerDataBrowser*>(o) )
	return "TQDataBrowser";
    else if ( ::tqt_cast<TQDesignerDataView*>(o) )
	return "TQDataView";
#endif
    return o->className();
}

TQString WidgetFactory::defaultSignal( TQObject *w )
{
    if ( ::tqt_cast<TQRadioButton*>(w) || ::tqt_cast<TQCheckBox*>(w) )
	return "toggled";
    else if ( ::tqt_cast<TQButton*>(w) || ::tqt_cast<TQButtonGroup*>(w) )
	return "clicked";
    else if ( ::tqt_cast<TQTextBrowser*>(w) )
	return "linkClicked";
    else if ( ::tqt_cast<TQLineEdit*>(w) || ::tqt_cast<TQTextEdit*>(w) )
	return "textChanged";
    else if ( ::tqt_cast<TQListView*>(w) || ::tqt_cast<TQIconView*>(w) ||
	      ::tqt_cast<TQListBox*>(w) || ::tqt_cast<TQTable*>(w) )
	return "selectionChanged";
    else if ( ::tqt_cast<TQTabWidget*>(w) )
	return "selected";
    else if ( ::tqt_cast<TQToolBox*>(w) )
	return "currentChanged";
    else if ( ::tqt_cast<TQWidgetStack*>(w) )
	return "aboutToShow";
    else if ( ::tqt_cast<TQSpinBox*>(w) || ::tqt_cast<TQSlider*>(w) ||
	      ::tqt_cast<TQScrollBar*>(w) || ::tqt_cast<TQDateEdit*>(w) ||
	      ::tqt_cast<TQTimeEdit*>(w) || ::tqt_cast<TQDateTimeEdit*>(w) ||
	      ::tqt_cast<TQDial*>(w) )
	return "valueChanged";
    else if ( ::tqt_cast<TQComboBox*>(w) )
	return "activated";
    return TQString::null;
}

/*!  As some properties are set by default when creating a widget this
  functions markes this properties as changed. Has to be in sync with
  createWidget()!
*/

void WidgetFactory::initChangedProperties( TQObject *o )
{
    if ( MainWindow::self && MainWindow::self->currProject() &&
	 MainWindow::self->currProject()->fakeFormFileFor( o ) )
	return;
    MetaDataBase::setPropertyChanged( o, "name", true );
    if ( !::tqt_cast<TQDesignerToolBar*>(o) && !::tqt_cast<MenuBarEditor*>(o) )
	MetaDataBase::setPropertyChanged( o, "geometry", true );

    if ( ::tqt_cast<TQPushButton*>(o) ||
	 ::tqt_cast<TQRadioButton*>(o) ||
	 ::tqt_cast<TQCheckBox*>(o) ||
	 ::tqt_cast<TQToolButton*>(o) ) {
	if (::tqt_cast<TQToolButton*>(o) && ::tqt_cast<TQToolBox*>(widgetOfContainer((TQWidget*)o->parent()))) {
	    MetaDataBase::setPropertyChanged( o, "usesTextLabel", true );
	    MetaDataBase::setPropertyChanged( o, "textLabel", true );
	    MetaDataBase::setPropertyChanged( o, "autoRaise", true );
	    MetaDataBase::setPropertyChanged( o, "textPosition", true );
	} else {
	    MetaDataBase::setPropertyChanged( o, "text", true );
	}
    }
    else if ( ::tqt_cast<TQGroupBox*>(o) )
	MetaDataBase::setPropertyChanged( o, "title", true );
    else if ( o->isA( "TQFrame" ) ) {
	MetaDataBase::setPropertyChanged( o, "frameShadow", true );
	MetaDataBase::setPropertyChanged( o, "frameShape", true );
    } else if ( ::tqt_cast<TQTabWidget*>(o) || ::tqt_cast<TQWizard*>(o) ) {
	MetaDataBase::setPropertyChanged( o, "pageTitle", true );
	MetaDataBase::setPropertyChanged( o, "pageName", true );
	MetaDataBase::setPropertyChanged( o, "currentPage", true );
    } else if ( ::tqt_cast<TQWidgetStack*>(o) ) {
	MetaDataBase::setPropertyChanged( o, "currentPage", true );
	MetaDataBase::setPropertyChanged( o, "pageName", true );
    } else if ( ::tqt_cast<TQToolBox*>(o) ) {
	MetaDataBase::setPropertyChanged( o, "currentIndex", true );
	MetaDataBase::setPropertyChanged( o, "itemName", true );
	MetaDataBase::setPropertyChanged( o, "itemLabel", true );
	MetaDataBase::setPropertyChanged( o, "itemIconSet", true );
	MetaDataBase::setPropertyChanged( o, "itemToolTip", true );
	MetaDataBase::setPropertyChanged( o, "itemBackgroundMode", true );
#ifndef TQT_NO_TABLE
    } else if ( ::tqt_cast<TQTable*>(o) ) {
#  ifndef TQT_NO_SQL
        if (!::tqt_cast<TQDataTable*>(o) )
#  endif
        {
            MetaDataBase::setPropertyChanged( o, "numRows", true );
            MetaDataBase::setPropertyChanged( o, "numCols", true );
            TQTable *t = (TQTable*)o;
            for ( int i = 0; i < 3; ++i ) {
                t->horizontalHeader()->setLabel( i, TQString::number( i + 1 ) );
                t->verticalHeader()->setLabel( i, TQString::number( i + 1 ) );
            }
        }
#endif
    } else if ( ::tqt_cast<TQSplitter*>(o)  ) {
	MetaDataBase::setPropertyChanged( o, "orientation", true );
    } else if ( ::tqt_cast<TQDesignerToolBar*>(o)  ) {
	MetaDataBase::setPropertyChanged( o, "label", true );
    } else if ( ::tqt_cast<MenuBarEditor*>(o)  ) {
	MetaDataBase::setPropertyChanged( o, "itemName", true );
	MetaDataBase::setPropertyChanged( o, "itemNumber", true );
	MetaDataBase::setPropertyChanged( o, "itemText", true );
    }
}

bool WidgetFactory::hasSpecialEditor( int id, TQObject *editorWidget )
{
    TQString className = WidgetDatabase::className( id );

    if ( className.contains( "ListBox" ) )
	return true;
    if ( className.contains( "ComboBox" ) )
	return true;
    if ( className.contains( "ListView" ) )
	return true;
    if ( className.contains( "IconView" ) )
	return true;
    if ( className == "TQTextEdit" || className == "TQMultiLineEdit" )
	return true;
    if ( ::tqt_cast<TQTable*>(editorWidget) != 0 )
	return true;

    return false;
}

bool WidgetFactory::hasItems( int id, TQObject *editorWidget )
{
    TQString className = WidgetDatabase::className( id );

    if ( className.contains( "ListBox" ) || className.contains( "ListView" ) ||
	 className.contains( "IconView" )  || className.contains( "ComboBox" ) ||
	 ::tqt_cast<TQTable*>(editorWidget) != 0 )
	return true;

    return false;
}

void WidgetFactory::editWidget( int id, TQWidget *parent, TQWidget *editWidget, FormWindow *fw )
{
    TQString className = WidgetDatabase::className( id );

    if ( className.contains( "ListBox" ) ) {
	if ( !::tqt_cast<TQListBox*>(editWidget) )
	    return;
	ListBoxEditor *e = new ListBoxEditor( parent, editWidget, fw );
	e->exec();
	delete e;
	return;
    }

    if ( className.contains( "ComboBox" ) ) {
	if ( !::tqt_cast<TQComboBox*>(editWidget) )
	    return;
	TQComboBox *cb = (TQComboBox*)editWidget;
        TQListBox *lb = cb->listBox();
        if (!lb) {
            lb = new TQListBox(cb);
            cb->setListBox(lb);
        }
	ListBoxEditor *e = new ListBoxEditor( parent, lb, fw );
	e->exec();
	delete e;
	cb->update();
	return;
    }

    if ( className.contains( "ListView" ) ) {
	if ( !::tqt_cast<TQListView*>(editWidget) )
	    return;
	TQListView *lv = (TQListView*)editWidget;
	ListViewEditor *e = new ListViewEditor( parent, lv, fw );
	e->exec();
	delete e;
	return;
    }

    if ( className.contains( "IconView" ) ) {
	if ( !::tqt_cast<TQIconView*>(editWidget) )
	    return;
	IconViewEditor *e = new IconViewEditor( parent, editWidget, fw );
	e->exec();
	delete e;
	return;
    }

    if ( className == "TQMultiLineEdit" || className == "TQTextEdit" ) {
	MultiLineEditor *e = new MultiLineEditor( false, true, parent, editWidget, fw );
	e->exec();
	delete e;
	return;
    }
#ifndef TQT_NO_TABLE
    if (::tqt_cast<TQTable*>(editWidget) != 0) {
	TableEditor *e = new TableEditor( parent, editWidget, fw );
	e->exec();
	delete e;
	return;
    }
#endif
}

bool WidgetFactory::canResetProperty( TQObject *w, const TQString &propName )
{
    if ( propName == "name" || propName == "geometry" )
	return false;
    TQStringList l = *changedProperties->find( WidgetDatabase::idFromClassName( WidgetFactory::classNameOf( w ) ) );
    return l.findIndex( propName ) == -1;
}

bool WidgetFactory::resetProperty( TQObject *w, const TQString &propName )
{
    const TQMetaProperty *p = w->metaObject()->property( w->metaObject()->
							findProperty( propName, true ), true );
    if (!p )
	return false;
    return p->reset( w );
}

TQVariant WidgetFactory::defaultValue( TQObject *w, const TQString &propName )
{
    if ( propName == "wordwrap" ) {
	int v = defaultValue( w, "alignment" ).toInt();
	return TQVariant( ( v & WordBreak ) == WordBreak );
    } else if ( propName == "toolTip" || propName == "whatsThis" ) {
	return TQVariant( TQString::fromLatin1( "" ) );
    } else if ( w->inherits( "CustomWidget" ) ) {
	return TQVariant();
    } else if ( propName == "frameworkCode" ) {
	return TQVariant( true );
    } else if ( propName == "layoutMargin" || propName == "layoutSpacing" ) {
	return TQVariant( -1 );
    }

    return *( *defaultProperties->find( WidgetDatabase::idFromClassName( classNameOf( w ) ) ) ).find( propName );
}

TQString WidgetFactory::defaultCurrentItem( TQObject *w, const TQString &propName )
{
    const TQMetaProperty *p = w->metaObject()->
			     property( w->metaObject()->findProperty( propName, true ), true );
    if ( !p ) {
	int v = defaultValue( w, "alignment" ).toInt();
	if ( propName == "hAlign" ) {
	    if ( ( v & AlignAuto ) == AlignAuto )
		return "AlignAuto";
	    if ( ( v & AlignLeft ) == AlignLeft )
		return "AlignLeft";
	    if ( ( v & AlignCenter ) == AlignCenter || ( v & AlignHCenter ) == AlignHCenter )
		return "AlignHCenter";
	    if ( ( v & AlignRight ) == AlignRight )
		return "AlignRight";
	    if ( ( v & AlignJustify ) == AlignJustify )
		return "AlignJustify";
	} else if ( propName == "vAlign" ) {
	    if ( ( v & AlignTop ) == AlignTop )
		return "AlignTop";
	    if ( ( v & AlignCenter ) == AlignCenter || ( v & AlignVCenter ) == AlignVCenter )
		return "AlignVCenter";
	    if ( ( v & AlignBottom ) == AlignBottom )
		return "AlignBottom";
	} else if ( propName == "resizeMode" ) {
	    return "Auto";
	}
	return TQString::null;

    }
    return p->valueToKey( defaultValue( w, propName ).toInt() );
}

TQWidget *WidgetFactory::createCustomWidget( TQWidget *parent, const char *name, MetaDataBase::CustomWidget *w )
{
    if ( !w )
	return 0;
    return new CustomWidget( parent, name, w );
}

TQVariant WidgetFactory::property( TQObject *w, const char *name )
{
    int id = w->metaObject()->findProperty( name, true );
    const TQMetaProperty* p = w->metaObject()->property( id, true );
    if ( !p || !p->isValid() )
	return MetaDataBase::fakeProperty( w, name );
    return w->property( name );
}

void TQDesignerLabel::updateBuddy()
{

    if ( myBuddy.isEmpty() )
	return;

    TQObjectList *l = topLevelWidget()->queryList( "TQWidget", myBuddy, false, true );
    if ( !l || !l->first() ) {
	delete l;
	return;
    }

    TQLabel::setBuddy( (TQWidget*)l->first() );
    delete l;
}


void TQDesignerWidget::resizeEvent( TQResizeEvent* e)
{
    if ( need_frame ) {
	TQPainter p(this);
	p.setPen( backgroundColor() );
	p.drawRect( TQRect( TQPoint(0,0), e->oldSize() ) );
    }
}

void TQDesignerWidget::paintEvent( TQPaintEvent *e )
{
    if ( need_frame ) {
	TQPainter p(this);
	p.setPen( backgroundColor().dark() );
	p.drawRect( rect() );
    }
    formwindow->paintGrid( this, e );
}

void TQDesignerDialog::paintEvent( TQPaintEvent *e )
{
    formwindow->paintGrid( this, e );
}

TQSizePolicy TQLayoutWidget::sizePolicy() const
{
    return sp;
}

bool TQLayoutWidget::event( TQEvent *e )
{
    if ( e && ( e->type() == TQEvent::ChildInserted ||
		e->type() == TQEvent::ChildRemoved ||
		e->type() == TQEvent::LayoutHint ||
		e->type() == TQEvent::Reparent ) )
	updateSizePolicy();
    return TQWidget::event( e );
}

/*
  This function must be called on TQLayoutWidget creation and whenever
  the TQLayoutWidget's parent layout changes (e.g., from a TQHBoxLayout
  to a TQVBoxLayout), because of the (illogical) way layouting works.
*/
void TQLayoutWidget::updateSizePolicy()
{
    if ( !children() || children()->count() == 0 ) {
	sp = TQWidget::sizePolicy();
	return;
    }

    /*
      TQSizePolicy::MayShrink & friends are private. Here we assume the
      following:

	  Fixed = 0
	  Maximum = MayShrink
	  Minimum = MayGrow
	  Preferred = MayShrink | MayGrow
    */

    int ht = (int) TQSizePolicy::Preferred;
    int vt = (int) TQSizePolicy::Preferred;

    if ( layout() ) {
	/*
	  parentLayout is set to the parent layout if there is one and if it is
	  top level, in which case layouting is illogical.
	*/
	TQLayout *parentLayout = 0;
	if ( parent() && parent()->isWidgetType() ) {
	    parentLayout = ((TQWidget *)parent())->layout();
	    if ( parentLayout &&
		 ::tqt_cast<TQLayoutWidget*>(parentLayout->mainWidget()) )
		parentLayout = 0;
	}

	TQObjectListIt it( *children() );
	TQObject *o;

	if ( ::tqt_cast<TQVBoxLayout*>(layout()) ) {
	    if ( ::tqt_cast<TQHBoxLayout*>(parentLayout) )
		vt = TQSizePolicy::Minimum;
	    else
		vt = TQSizePolicy::Fixed;

	    while ( ( o = it.current() ) ) {
		++it;
		if ( !o->isWidgetType() || ( (TQWidget*)o )->testWState( WState_ForceHide ) )
		    continue;
		TQWidget *w = (TQWidget*)o;

		if ( !w->sizePolicy().mayGrowHorizontally() )
		    ht &= ~TQSizePolicy::Minimum;
		if ( !w->sizePolicy().mayShrinkHorizontally() )
		    ht &= ~TQSizePolicy::Maximum;
		if ( w->sizePolicy().mayGrowVertically() )
		    vt |= TQSizePolicy::Minimum;
		if ( w->sizePolicy().mayShrinkVertically() )
		    vt |= TQSizePolicy::Maximum;
	    }
	} else if ( ::tqt_cast<TQHBoxLayout*>(layout()) ) {
	    if ( ::tqt_cast<TQVBoxLayout*>(parentLayout) )
		ht = TQSizePolicy::Minimum;
	    else
		ht = TQSizePolicy::Fixed;

	    while ( ( o = it.current() ) ) {
		++it;
		if ( !o->isWidgetType() || ( (TQWidget*)o )->testWState( WState_ForceHide ) )
		    continue;
		TQWidget *w = (TQWidget*)o;

		if ( w->sizePolicy().mayGrowHorizontally() )
		    ht |= TQSizePolicy::Minimum;
		if ( w->sizePolicy().mayShrinkHorizontally() )
		    ht |= TQSizePolicy::Maximum;
		if ( !w->sizePolicy().mayGrowVertically() )
		    vt &= ~TQSizePolicy::Minimum;
		if ( !w->sizePolicy().mayShrinkVertically() )
		    vt &= ~TQSizePolicy::Maximum;
	    }
	} else if ( ::tqt_cast<TQGridLayout*>(layout()) ) {
	    ht = TQSizePolicy::Fixed;
	    vt = TQSizePolicy::Fixed;
	    if ( parentLayout ) {
		if ( ::tqt_cast<TQVBoxLayout*>(parentLayout) )
		    ht = TQSizePolicy::Minimum;
		else if ( ::tqt_cast<TQHBoxLayout*>(parentLayout) )
		    vt = TQSizePolicy::Minimum;
	    }

	    while ( ( o = it.current() ) ) {
		++it;
		if ( !o->isWidgetType() || ( (TQWidget*)o )->testWState( WState_ForceHide ) )
		    continue;
		TQWidget *w = (TQWidget*)o;

		if ( w->sizePolicy().mayGrowHorizontally() )
		    ht |= TQSizePolicy::Minimum;
		if ( w->sizePolicy().mayShrinkHorizontally() )
		    ht |= TQSizePolicy::Maximum;
		if ( w->sizePolicy().mayGrowVertically() )
		    vt |= TQSizePolicy::Minimum;
		if ( w->sizePolicy().mayShrinkVertically() )
		    vt |= TQSizePolicy::Maximum;
	    }
	}
	if ( layout()->expanding() & TQSizePolicy::Horizontally )
	    ht = TQSizePolicy::Expanding;
	if ( layout()->expanding() & TQSizePolicy::Vertically )
	    vt = TQSizePolicy::Expanding;

	layout()->invalidate();
    }

    sp = TQSizePolicy( (TQSizePolicy::SizeType) ht, (TQSizePolicy::SizeType) vt );
    updateGeometry();
}

void CustomWidget::paintEvent( TQPaintEvent *e )
{
    if ( ::tqt_cast<FormWindow*>(parentWidget()) ) {
	( (FormWindow*)parentWidget() )->paintGrid( this, e );
    } else {
	TQPainter p( this );
	p.fillRect( rect(), colorGroup().dark() );
	p.setPen( colorGroup().light() );
	p.drawText( 2, 2, width() - 4, height() - 4, TQt::AlignAuto | TQt::AlignTop, cusw->className );
	p.drawPixmap( ( width() - cusw->pixmap->width() ) / 2,
		      ( height() - cusw->pixmap->height() ) / 2,
		      *cusw->pixmap );
    }
}


CustomWidgetFactory::CustomWidgetFactory()
{
}

TQWidget *CustomWidgetFactory::createWidget( const TQString &className, TQWidget *parent, const char *name ) const
{
    MetaDataBase::CustomWidget *w = MetaDataBase::customWidget( WidgetDatabase::idFromClassName( className ) );
    if ( !w )
	return 0;
    return WidgetFactory::createCustomWidget( parent, name, w );
}

void TQDesignerRadioButton::setFocusPolicy( FocusPolicy policy )
{
    // Make sure we only set a policy that has a key.
    // If we set a policy that is a combination of enums, it will be imposible to save
    // We need this check because TQButtonGroup::buttonToggled(...) will remove TabFocus,
    // making the policy StrongFocus & ~TabFocus
    
    switch ( policy ) {
    case NoFocus:
    case TabFocus:
    case ClickFocus:
    case StrongFocus:
    case WheelFocus:
        TQRadioButton::setFocusPolicy( policy );
        return;
    }
    TQRadioButton::setFocusPolicy( NoFocus );
}

TQDesignerToolBox::TQDesignerToolBox( TQWidget *parent, const char *name )
    : TQToolBox( parent, name )
{
}

TQString TQDesignerToolBox::itemLabel() const
{
    return TQToolBox::itemLabel( currentIndex() );
}

void TQDesignerToolBox::setItemLabel( const TQString &l )
{
    TQToolBox::setItemLabel( currentIndex(), l );
}

TQCString TQDesignerToolBox::itemName() const
{
    return currentItem() ? currentItem()->name() : 0;
}

void TQDesignerToolBox::setItemName( const TQCString &n )
{
    if (currentItem())
	currentItem()->setName( n );
}


TQt::BackgroundMode TQDesignerToolBox::itemBackgroundMode() const
{
    return (item(0) ? item(0)->backgroundMode() : PaletteBackground);
}

void TQDesignerToolBox::setItemBackgroundMode( BackgroundMode bmode )
{
    for (int i = 0; i < count(); ++i) {
	TQWidget *w = item(i);
	w->setBackgroundMode( bmode );
	w->update();
    }
}

void TQDesignerToolBox::itemInserted( int index )
{
    if (count() > 1)
	item(index)->setBackgroundMode(item(index>0?0:1)->backgroundMode());
}
