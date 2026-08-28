/****************************************************************************
**
** Copyright (C) 1992-2008 Trolltech ASA.  All rights reserved.
**
** This file is part of an example program for TQt.  This example
** program may be used, distributed and modified without limitation.
**
*****************************************************************************/

#include "themes.h"
#include "wood.h"
#include "metal.h"

#include "../buttongroups/buttongroups.h"
#include "../lineedits/lineedits.h"
#include "../listboxcombo/listboxcombo.h"
#include "../checklists/checklists.h"
#include "../progressbar/progressbar.h"
#include "../rangecontrols/rangecontrols.h"
#include "../richtext/richtext.h"

#include <ntqtabwidget.h>
#include <ntqapplication.h>
#include <ntqpopupmenu.h>
#include <ntqmenubar.h>
#include <ntqmessagebox.h>
#include <ntqfont.h>
#include <ntqstylefactory.h>
#include <ntqaction.h>
#include <ntqsignalmapper.h>
#include <ntqdict.h>

Themes::Themes( TQWidget *parent, const char *name, WFlags f )
    : TQMainWindow( parent, name, f )
{
    appFont = TQApplication::font();
    tabwidget = new TQTabWidget( this );

    tabwidget->addTab( new ButtonsGroups( tabwidget ), "Buttons/Groups" );
    TQHBox *hbox = new TQHBox( tabwidget );
    hbox->setMargin( 5 );
    (void)new LineEdits( hbox );
    (void)new ProgressBar( hbox );
    tabwidget->addTab( hbox, "Lineedits/Progressbar" );
    tabwidget->addTab( new ListBoxCombo( tabwidget ), "Listboxes/Comboboxes" );
    tabwidget->addTab( new CheckLists( tabwidget ), "Listviews" );
    tabwidget->addTab( new RangeControls( tabwidget ), "Rangecontrols" );
    tabwidget->addTab( new MyRichText( tabwidget ), "Fortune" );

    setCentralWidget( tabwidget );

    TQPopupMenu *style = new TQPopupMenu( this );
    style->setCheckable( true );
    menuBar()->insertItem( "&Style" , style );

    style->setCheckable( true );
    TQActionGroup *ag = new TQActionGroup( this, 0 );
    ag->setExclusive( true );
    TQSignalMapper *styleMapper = new TQSignalMapper( this );
    connect( styleMapper, TQ_SIGNAL( mapped( const TQString& ) ), this, TQ_SLOT( makeStyle( const TQString& ) ) );
    TQStringList list = TQStyleFactory::keys();
    list.sort();
#ifndef TQT_NO_STYLE_WINDOWS
    list.insert(list.begin(), "Norwegian Wood");
    list.insert(list.begin(), "Metal");
#endif
    TQDict<int> stylesDict( 17, false );
    for ( TQStringList::Iterator it = list.begin(); it != list.end(); ++it ) {
	TQString styleStr = *it;
	TQString styleAccel = styleStr;
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
	TQAction *a = new TQAction( styleStr, TQIconSet(), styleAccel, 0, ag, 0, ag->isExclusive() );
	connect( a, TQ_SIGNAL( activated() ), styleMapper, TQ_SLOT(map()) );
	styleMapper->setMapping( a, a->text() );
    }
    ag->addTo(style);
    style->insertSeparator();
    style->insertItem("&Quit", tqApp, TQ_SLOT( quit() ), CTRL | Key_Q );

    TQPopupMenu * help = new TQPopupMenu( this );
    menuBar()->insertSeparator();
    menuBar()->insertItem( "&Help", help );
    help->insertItem( "&About", this, TQ_SLOT(about()), Key_F1);
    help->insertItem( "About &TQt", this, TQ_SLOT(aboutTQt()));

#ifndef TQT_NO_STYLE_WINDOWS
    tqApp->setStyle( new NorwegianWoodStyle );
#endif
}

void Themes::makeStyle(const TQString &style)
{
    if(style == "Norwegian Wood") {
#ifndef TQT_NO_STYLE_WINDOWS
	tqApp->setStyle( new NorwegianWoodStyle );
#endif
    } else if( style == "Metal" ) {
#ifndef TQT_NO_STYLE_WINDOWS
	tqApp->setStyle( new MetalStyle );
#endif
    } else {
	tqApp->setStyle(style);
	if(style == "Platinum") {
	    TQPalette p( TQColor( 239, 239, 239 ) );
	    tqApp->setPalette( p, true );
	    tqApp->setFont( appFont, true );
	} else if(style == "Windows") {
	    tqApp->setFont( appFont, true );
	} else if(style == "CDE") {
	    TQPalette p( TQColor( 75, 123, 130 ) );
	    p.setColor( TQPalette::Active, TQColorGroup::Base, TQColor( 55, 77, 78 ) );
	    p.setColor( TQPalette::Inactive, TQColorGroup::Base, TQColor( 55, 77, 78 ) );
	    p.setColor( TQPalette::Disabled, TQColorGroup::Base, TQColor( 55, 77, 78 ) );
	    p.setColor( TQPalette::Active, TQColorGroup::Highlight, TQt::white );
	    p.setColor( TQPalette::Active, TQColorGroup::HighlightedText, TQColor( 55, 77, 78 ) );
	    p.setColor( TQPalette::Inactive, TQColorGroup::Highlight, TQt::white );
	    p.setColor( TQPalette::Inactive, TQColorGroup::HighlightedText, TQColor( 55, 77, 78 ) );
	    p.setColor( TQPalette::Disabled, TQColorGroup::Highlight, TQt::white );
	    p.setColor( TQPalette::Disabled, TQColorGroup::HighlightedText, TQColor( 55, 77, 78 ) );
	    p.setColor( TQPalette::Active, TQColorGroup::Foreground, TQt::white );
	    p.setColor( TQPalette::Active, TQColorGroup::Text, TQt::white );
	    p.setColor( TQPalette::Active, TQColorGroup::ButtonText, TQt::white );
	    p.setColor( TQPalette::Inactive, TQColorGroup::Foreground, TQt::white );
	    p.setColor( TQPalette::Inactive, TQColorGroup::Text, TQt::white );
	    p.setColor( TQPalette::Inactive, TQColorGroup::ButtonText, TQt::white );
	    p.setColor( TQPalette::Disabled, TQColorGroup::Foreground, TQt::lightGray );
	    p.setColor( TQPalette::Disabled, TQColorGroup::Text, TQt::lightGray );
	    p.setColor( TQPalette::Disabled, TQColorGroup::ButtonText, TQt::lightGray );
	    tqApp->setPalette( p, true );
	    tqApp->setFont( TQFont( "times", appFont.pointSize() ), true );
	} else if(style == "Motif" || style == "MotifPlus") {
	    TQPalette p( TQColor( 192, 192, 192 ) );
	    tqApp->setPalette( p, true );
	    tqApp->setFont( appFont, true );
	}
    }
}

void Themes::about()
{
    TQMessageBox::about( this, "TQt Themes Example",
			"<p>This example demonstrates the concept of "
			"<b>generalized GUI styles </b> first introduced "
			" with the 2.0 release of TQt.</p>" );
}


void Themes::aboutTQt()
{
    TQMessageBox::aboutTQt( this, "TQt Themes Example" );
}


