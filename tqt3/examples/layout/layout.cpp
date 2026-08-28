/****************************************************************************
**
** Copyright (C) 1992-2008 Trolltech ASA.  All rights reserved.
**
** This file is part of an example program for TQt.  This example
** program may be used, distributed and modified without limitation.
**
*****************************************************************************/

#include <ntqapplication.h>
#include <ntqlabel.h>
#include <ntqcolor.h>
#include <ntqpushbutton.h>
#include <ntqlayout.h>
#include <ntqlineedit.h>
#include <ntqmultilineedit.h>
#include <ntqmenubar.h>
#include <ntqpopupmenu.h>

class ExampleWidget : public TQWidget
{
public:
    ExampleWidget( TQWidget *parent = 0, const char *name = 0 );
    ~ExampleWidget();
};

ExampleWidget::ExampleWidget( TQWidget *parent, const char *name )
    : TQWidget( parent, name )
{
    // Make the top-level layout; a vertical box to contain all widgets
    // and sub-layouts.
    TQBoxLayout *topLayout = new TQVBoxLayout( this, 5 );

    // Create a menubar...
    TQMenuBar *menubar = new TQMenuBar( this );
    menubar->setSeparator( TQMenuBar::InWindowsStyle );
    TQPopupMenu* popup;
    popup = new TQPopupMenu( this );
    popup->insertItem( "&Quit", tqApp, TQ_SLOT(quit()) );
    menubar->insertItem( "&File", popup );

    // ...and tell the layout about it.
    topLayout->setMenuBar( menubar );

    // Make an hbox that will hold a row of buttons.
    TQBoxLayout *buttons = new TQHBoxLayout( topLayout );
    int i;
    for ( i = 1; i <= 4; i++ ) {
	TQPushButton* but = new TQPushButton( this );
	TQString s;
	s.sprintf( "Button %d", i );
	but->setText( s );

	// Set horizontal stretch factor to 10 to let the buttons
	// stretch horizontally. The buttons will not stretch
	// vertically, since bigWidget below will take up vertical
	// stretch.
	buttons->addWidget( but, 10 );
	// (Actually, the result would have been the same with a
	// stretch factor of 0; if no items in a layout have non-zero
	// stretch, the space is divided equally between members.)
    }

    // Make another hbox that will hold a left-justified row of buttons.
    TQBoxLayout *buttons2 = new TQHBoxLayout( topLayout );

    TQPushButton* but = new TQPushButton( "Button five", this );
    buttons2->addWidget( but );

    but = new TQPushButton( "Button 6", this );
    buttons2->addWidget( but );

    // Fill up the rest of the hbox with stretchable space, so that
    // the buttons get their minimum width and are pushed to the left.
    buttons2->addStretch( 10 );

    // Make  a big widget that will grab all space in the middle.
    TQMultiLineEdit *bigWidget = new TQMultiLineEdit( this );
    bigWidget->setText( "This widget will get all the remaining space" );
    bigWidget->setFrameStyle( TQFrame::Panel | TQFrame::Plain );

    // Set vertical stretch factor to 10 to let the bigWidget stretch
    // vertically. It will stretch horizontally because there are no
    // widgets beside it to take up horizontal stretch.
    //    topLayout->addWidget( bigWidget, 10 );
    topLayout->addWidget( bigWidget ); 

    // Make a grid that will hold a vertical table of TQLabel/TQLineEdit
    // pairs next to a large TQMultiLineEdit.

    // Don't use hard-coded row/column numbers in TQGridLayout, you'll
    // regret it when you have to change the layout.
    const int numRows = 3;
    const int labelCol = 0;
    const int linedCol = 1;
    const int multiCol = 2;

    // Let the grid-layout have a spacing of 10 pixels between
    // widgets, overriding the default from topLayout.
    TQGridLayout *grid = new TQGridLayout( topLayout, 0, 0, 10 );
    int row;

    for ( row = 0; row < numRows; row++ ) {
	TQLineEdit *ed = new TQLineEdit( this );
	// The line edit goes in the second column
	grid->addWidget( ed, row, linedCol );	

	// Make a label that is a buddy of the line edit
	TQString s;
	s.sprintf( "Line &%d", row+1 );
	TQLabel *label = new TQLabel( ed, s, this );
	// The label goes in the first column.
	grid->addWidget( label, row, labelCol );
    }

    // The multiline edit will cover the entire vertical range of the
    // grid (rows 0 to numRows) and stay in column 2.

    TQMultiLineEdit *med = new TQMultiLineEdit( this );
    grid->addMultiCellWidget( med, 0, -1, multiCol, multiCol );

    // The labels will take the space they need. Let the remaining
    // horizontal space be shared so that the multiline edit gets
    // twice as much as the line edit.
    grid->setColStretch( linedCol, 10 );
    grid->setColStretch( multiCol, 20 );

    // Add a widget at the bottom.
    TQLabel* sb = new TQLabel( this );
    sb->setText( "Let's pretend this is a status bar" );
    sb->setFrameStyle( TQFrame::Panel | TQFrame::Sunken );
    // This widget will use all horizontal space, and have a fixed height.

    // we should have made a subclass and implemented sizePolicy there...
    sb->setFixedHeight( sb->sizeHint().height() );

    sb->setAlignment( AlignVCenter | AlignLeft );
    topLayout->addWidget( sb );

    topLayout->activate();
}

ExampleWidget::~ExampleWidget()
{
    // All child widgets are deleted by TQt.
    // The top-level layout and all its sub-layouts are deleted by TQt.
}

int main( int argc, char **argv )
{
    TQApplication a( argc, argv );

    ExampleWidget f;
    a.setMainWidget(&f);
    f.setCaption("TQt Example - Layouts");
    f.show();

    return a.exec();
}
