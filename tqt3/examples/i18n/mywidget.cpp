/****************************************************************************
**
** Copyright (C) 1992-2008 Trolltech ASA.  All rights reserved.
**
** This file is part of an example program for TQt.  This example
** program may be used, distributed and modified without limitation.
**
*****************************************************************************/

#include <ntqbuttongroup.h>
#include <ntqradiobutton.h>
#include <ntqlabel.h>
#include <ntqlistbox.h>
#include <ntqcombobox.h>
#include <ntqlabel.h>
#include <ntqhbox.h>
#include <ntqvbox.h>
#include <ntqaccel.h>
#include <ntqpopupmenu.h>
#include <ntqmenubar.h>
#include <ntqstatusbar.h>
#include <ntqapplication.h>

#include "mywidget.h"

MyWidget::MyWidget( TQWidget* parent, const char* name )
	: TQMainWindow( parent, name )
{
    TQVBox* central = new TQVBox(this);
    central->setMargin( 5 ); 
    central->setSpacing( 5 ); 
    setCentralWidget(central);

    TQPopupMenu* file = new TQPopupMenu(this);
    file->insertItem( tr("E&xit"), tqApp, TQ_SLOT(quit()),
            TQAccel::stringToKey(tr("Ctrl+Q")) );
    menuBar()->insertItem( tr("&File"), file );

    setCaption( tr( "Internationalization Example" ) ); 

    TQString l;
    statusBar()->message( tr("Language: English") );

    ( void )new TQLabel( tr( "The Main Window" ), central ); 

    TQButtonGroup* gbox = new TQButtonGroup( 1, TQGroupBox::Horizontal, 
				      tr( "View" ), central ); 
    (void)new TQRadioButton( tr( "Perspective" ), gbox ); 
    (void)new TQRadioButton( tr( "Isometric" ), gbox ); 
    (void)new TQRadioButton( tr( "Oblique" ), gbox ); 

    initChoices(central); 
}

static const char* choices[] = {
    TQT_TRANSLATE_NOOP( "MyWidget", "First" ),
    TQT_TRANSLATE_NOOP( "MyWidget", "Second" ),
    TQT_TRANSLATE_NOOP( "MyWidget", "Third" ),
    0
}; 

void MyWidget::initChoices(TQWidget* parent)
{
    TQListBox* lb = new TQListBox( parent ); 
    for ( int i = 0; choices[i]; i++ )
	lb->insertItem( tr( choices[i] ) ); 
}

void MyWidget::closeEvent(TQCloseEvent* e)
{
    TQWidget::closeEvent(e);
    emit closed();
}
