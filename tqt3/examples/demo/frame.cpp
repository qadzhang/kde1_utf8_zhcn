/****************************************************************************
**
** Copyright (C) 1992-2008 Trolltech ASA.  All rights reserved.
**
** This file is part of an example program for TQt.  This example
** program may be used, distributed and modified without limitation.
**
*****************************************************************************/

#include "frame.h"

#include <ntqapplication.h>
#include <ntqpopupmenu.h>
#include <ntqmenubar.h>
#include <ntqaccel.h>
#include <ntqtoolbox.h>
#include <ntqpainter.h>
#include <ntqwidgetstack.h>
#include <ntqstylefactory.h>
#include <ntqaction.h>
#include <ntqsignalmapper.h>
#include <ntqdict.h>
#include <ntqdir.h>
#include <ntqtextcodec.h>
#include <stdlib.h>
#include <ntqbuttongroup.h>
#include <ntqtoolbutton.h>

static TQTranslator *translator = 0;
static TQTranslator *qt_translator = 0;

Frame::Frame( TQWidget *parent, const char *name )
    : TQMainWindow( parent, name )
{
    TQMenuBar *mainMenu = menuBar();
    TQPopupMenu *fileMenu = new TQPopupMenu( this, "file" );
    fileMenu->insertItem( tr( "&Exit" ), this, TQ_SLOT( close() ),
			  TQAccel::stringToKey( tr( "Ctrl+Q" ) ) );

    TQPopupMenu *styleMenu = new TQPopupMenu( this, "style" );
    styleMenu->setCheckable( true );
    TQActionGroup *ag = new TQActionGroup( this, 0 );
    ag->setExclusive( true );
    TQSignalMapper *styleMapper = new TQSignalMapper( this );
    connect( styleMapper, TQ_SIGNAL( mapped( const TQString& ) ),
	     this, TQ_SLOT( setStyle( const TQString& ) ) );

    TQStringList list = TQStyleFactory::keys();
    list.sort();
    TQDict<int> stylesDict( 17, false );
    for ( TQStringList::Iterator it = list.begin(); it != list.end(); ++it ) {
	TQString style = *it;
	TQString styleAccel = style;
	if ( stylesDict[styleAccel.left(1)] ) {
	    for ( uint i = 0; i < styleAccel.length(); i++ ) {
		if ( !stylesDict[styleAccel.mid( i, 1 )] ) {
		    stylesDict.insert(styleAccel.mid( i, 1 ), (const int *)1);
		    styleAccel = styleAccel.insert( i, '&' );
		    break;
		}
	    }
	} else {
	    stylesDict.insert(styleAccel.left(1), (const int *)1);
	    styleAccel = "&"+styleAccel;
	}
	TQAction *a = new TQAction( style, TQIconSet(),
				  styleAccel, 0, ag, 0, ag->isExclusive() );
	connect( a, TQ_SIGNAL( activated() ), styleMapper, TQ_SLOT(map()) );
	styleMapper->setMapping( a, a->text() );
    }
    ag->addTo( styleMenu );

    mainMenu->insertItem( tr( "&File" ), fileMenu );
    mainMenu->insertItem( tr( "St&yle" ), styleMenu );

    stack = new TQWidgetStack( this );

    setCentralWidget( stack );
}

void Frame::setCategories( const TQPtrList<CategoryInterface> &l )
{
    categories = l;
    TQDockWindow *dw = new TQDockWindow( TQDockWindow::InDock, this );
    dw->setResizeEnabled( true );
    dw->setVerticalStretchable( true );
    addDockWindow( dw, DockLeft );
    setDockEnabled( dw, DockTop, false );
    setDockEnabled( dw, DockBottom, false );
    dw->setCloseMode( TQDockWindow::Always );

    toolBox = new TQToolBox( dw );
    dw->setWidget( toolBox );

    dw->setCaption( tr( "Demo Categories" ) );

    for ( int i = 0; i < categories.count(); ++i )
	toolBox->addItem( createCategoryPage( categories.at(i) ),
			  categories.at(i)->icon(),
			  categories.at(i)->name() );

    categories.first()->setCurrentCategory( 0 );
}

TQWidget *Frame::createCategoryPage( CategoryInterface *c )
{
    TQButtonGroup *g = new TQButtonGroup( 1, Horizontal, toolBox );
    g->setFrameStyle( TQFrame::NoFrame );
    g->setEraseColor(green);
    g->setBackgroundMode(PaletteBase);
    for ( int i = 0; i < c->numCategories(); ++i ) {
	TQToolButton *b = new TQToolButton( g );
	b->setBackgroundMode(PaletteBase);
	b->setTextLabel( c->categoryName( i ) );
	b->setIconSet( c->categoryIcon( i ) );
	b->setAutoRaise( true );
	b->setTextPosition( TQToolButton::Right );
	b->setUsesTextLabel( true );
	g->insert( b, i + c->categoryOffset() );
	connect( g, TQ_SIGNAL( clicked( int ) ), c, TQ_SLOT( setCurrentCategory( int ) ) );
    }
    return g;
}

void Frame::setStyle( const TQString& style )
{
    TQStyle *s = TQStyleFactory::create( style );
    if ( s )
	TQApplication::setStyle( s );
}

void Frame::updateTranslators()
{
    if ( !qt_translator ) {
	qt_translator = new TQTranslator( tqApp );
	translator = new TQTranslator( tqApp );
	tqApp->installTranslator( qt_translator );
	tqApp->installTranslator( translator );
    }

    TQString base = TQDir("../../translations").absPath();
    qt_translator->load( TQString( "qt_%1" ).arg( TQTextCodec::locale() ), base );
    translator->load( TQString( "translations/demo_%1" ).arg( TQTextCodec::locale() ) );
}

bool Frame::event( TQEvent *e )
{
    if ( e->type() == TQEvent::LocaleChange )
	updateTranslators();

    return TQMainWindow::event( e );
}
