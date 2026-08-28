/****************************************************************************
**
** Copyright (C) 1992-2008 Trolltech ASA.  All rights reserved.
**
** This file is part of an example program for TQt.  This example
** program may be used, distributed and modified without limitation.
**
*****************************************************************************/

#include "menu.h"
#include <ntqcursor.h>
#include <ntqpopupmenu.h>
#include <ntqapplication.h>
#include <ntqmessagebox.h>
#include <ntqpixmap.h>
#include <ntqpainter.h>

/* XPM */
static const char * p1_xpm[] = {
"16 16 3 1",
" 	c None",
".	c #000000000000",
"X	c #FFFFFFFF0000",
"                ",
"                ",
"         ....   ",
"        .XXXX.  ",
" .............. ",
" .XXXXXXXXXXXX. ",
" .XXXXXXXXXXXX. ",
" .XXXXXXXXXXXX. ",
" .XXXXXXXXXXXX. ",
" .XXXXXXXXXXXX. ",
" .XXXXXXXXXXXX. ",
" .XXXXXXXXXXXX. ",
" .XXXXXXXXXXXX. ",
" .XXXXXXXXXXXX. ",
" .............. ",
"                "};

/* XPM */
static const char * p2_xpm[] = {
"16 16 3 1",
" 	c None",
".	c #000000000000",
"X	c #FFFFFFFFFFFF",
"                ",
"   ......       ",
"   .XXX.X.      ",
"   .XXX.XX.     ",
"   .XXX.XXX.    ",
"   .XXX.....    ",
"   .XXXXXXX.    ",
"   .XXXXXXX.    ",
"   .XXXXXXX.    ",
"   .XXXXXXX.    ",
"   .XXXXXXX.    ",
"   .XXXXXXX.    ",
"   .XXXXXXX.    ",
"   .........    ",
"                ",
"                "};

/* XPM */
static const char * p3_xpm[] = {
"16 16 3 1",
" 	c None",
".	c #000000000000",
"X	c #FFFFFFFFFFFF",
"                ",
"                ",
"   .........    ",
"  ...........   ",
"  ........ ..   ",
"  ...........   ",
"  ...........   ",
"  ...........   ",
"  ...........   ",
"  ...XXXXX...   ",
"  ...XXXXX...   ",
"  ...XXXXX...   ",
"  ...XXXXX...   ",
"   .........    ",
"                ",
"                "};


/*
  Auxiliary class to provide fancy menu items with different
  fonts. Used for the "bold" and "underline" menu items in the options
  menu.
 */
class MyMenuItem : public TQCustomMenuItem
{
public:
    MyMenuItem( const TQString& s, const TQFont& f )
	: string( s ), font( f ){};
    ~MyMenuItem(){}

    void paint( TQPainter* p, const TQColorGroup& /*cg*/, bool /*act*/, bool /*enabled*/, int x, int y, int w, int h )
    {
	p->setFont ( font );
	p->drawText( x, y, w, h, AlignLeft | AlignVCenter | DontClip | ShowPrefix, string );
    }

    TQSize sizeHint()
    {
	return TQFontMetrics( font ).size( AlignLeft | AlignVCenter | ShowPrefix | DontClip,  string );
    }
private:
    TQString string;
    TQFont font;
};


MenuExample::MenuExample( TQWidget *parent, const char *name )
    : TQWidget( parent, name )
{
    TQPixmap p1( p1_xpm );
    TQPixmap p2( p2_xpm );
    TQPixmap p3( p3_xpm );
    TQPopupMenu *print = new TQPopupMenu( this );
    TQ_CHECK_PTR( print );
    print->insertTearOffHandle();
    print->insertItem( "&Print to printer", this, TQ_SLOT(printer()) );
    print->insertItem( "Print to &file", this, TQ_SLOT(file()) );
    print->insertItem( "Print to fa&x", this, TQ_SLOT(fax()) );
    print->insertSeparator();
    print->insertItem( "Printer &Setup", this, TQ_SLOT(printerSetup()) );

    TQPopupMenu *file = new TQPopupMenu( this );
    TQ_CHECK_PTR( file );
    file->insertItem( p1, "&Open",  this, TQ_SLOT(open()), CTRL+Key_O );
    file->insertItem( p2, "&New", this, TQ_SLOT(news()), CTRL+Key_N );
    file->insertItem( p3, "&Save", this, TQ_SLOT(save()), CTRL+Key_S );
    file->insertItem( "&Close", this, TQ_SLOT(closeDoc()), CTRL+Key_W );
    file->insertSeparator();
    file->insertItem( "&Print", print, CTRL+Key_P );
    file->insertSeparator();
    file->insertItem( "E&xit",  tqApp, TQ_SLOT(quit()), CTRL+Key_Q );

    TQPopupMenu *edit = new TQPopupMenu( this );
    TQ_CHECK_PTR( edit );
    int undoID = edit->insertItem( "&Undo", this, TQ_SLOT(undo()) );
    int redoID = edit->insertItem( "&Redo", this, TQ_SLOT(redo()) );
    edit->setItemEnabled( undoID, false );
    edit->setItemEnabled( redoID, false );

    TQPopupMenu* options = new TQPopupMenu( this );
    TQ_CHECK_PTR( options );
    options->insertTearOffHandle();
    options->setCaption("Options");
    options->insertItem( "&Normal Font", this, TQ_SLOT(normal()) );
    options->insertSeparator();

    options->polish(); // adjust system settings
    TQFont f = options->font();
    f.setBold( true );
    boldID = options->insertItem( new MyMenuItem( "Bold", f ) );
    options->setAccel( CTRL+Key_B, boldID );
    options->connectItem( boldID, this, TQ_SLOT(bold()) );
    f = font();
    f.setUnderline( true );
    underlineID = options->insertItem( new MyMenuItem( "Underline", f ) );
    options->setAccel( CTRL+Key_U, underlineID );
    options->connectItem( underlineID, this, TQ_SLOT(underline()) );

    isBold = false;
    isUnderline = false;
    options->setCheckable( true );


    TQPopupMenu *help = new TQPopupMenu( this );
    TQ_CHECK_PTR( help );
    help->insertItem( "&About", this, TQ_SLOT(about()), CTRL+Key_H );
    help->insertItem( "About &TQt", this, TQ_SLOT(aboutTQt()) );

    // If we used a TQMainWindow we could use its built-in menuBar().
    menu = new TQMenuBar( this );
    TQ_CHECK_PTR( menu );
    menu->insertItem( "&File", file );
    menu->insertItem( "&Edit", edit );
    menu->insertItem( "&Options", options );
    menu->insertSeparator();
    menu->insertItem( "&Help", help );
    menu->setSeparator( TQMenuBar::InWindowsStyle );


    TQLabel *msg = new TQLabel( this );
    TQ_CHECK_PTR( msg );
    msg->setText( "A context menu is available.\n"
		  "Invoke it by right-clicking or by"
		  " pressing the 'context' button." );
    msg->setGeometry( 0, height() - 60, width(), 60 );
    msg->setAlignment( AlignCenter );

    label = new TQLabel( this );
    TQ_CHECK_PTR( label );
    label->setGeometry( 20, rect().center().y()-20, width()-40, 40 );
    label->setFrameStyle( TQFrame::Box | TQFrame::Raised );
    label->setLineWidth( 1 );
    label->setAlignment( AlignCenter );

    connect( this,  TQ_SIGNAL(explain(const TQString&)),
	     label, TQ_SLOT(setText(const TQString&)) );

    setMinimumSize( 100, 80 );
    setFocusPolicy( TQWidget::ClickFocus );
}


void MenuExample::contextMenuEvent( TQContextMenuEvent * )
{
    TQPopupMenu*	contextMenu = new TQPopupMenu( this );
    TQ_CHECK_PTR( contextMenu );
    TQLabel *caption = new TQLabel( "<font color=darkblue><u><b>"
	"Context Menu</b></u></font>", this );
    caption->setAlignment( TQt::AlignCenter );
    contextMenu->insertItem( caption );
    contextMenu->insertItem( "&New",  this, TQ_SLOT(news()), CTRL+Key_N );
    contextMenu->insertItem( "&Open...", this, TQ_SLOT(open()), CTRL+Key_O );
    contextMenu->insertItem( "&Save", this, TQ_SLOT(save()), CTRL+Key_S );
    TQPopupMenu *submenu = new TQPopupMenu( this );
    TQ_CHECK_PTR( submenu );
    submenu->insertItem( "&Print to printer", this, TQ_SLOT(printer()) );
    submenu->insertItem( "Print to &file", this, TQ_SLOT(file()) );
    submenu->insertItem( "Print to fa&x", this, TQ_SLOT(fax()) );
    contextMenu->insertItem( "&Print", submenu );
    contextMenu->exec( TQCursor::pos() );
    delete contextMenu;
}


void MenuExample::open()
{
    emit explain( "File/Open selected" );
}


void MenuExample::news()
{
    emit explain( "File/New selected" );
}

void MenuExample::save()
{
    emit explain( "File/Save selected" );
}


void MenuExample::closeDoc()
{
    emit explain( "File/Close selected" );
}


void MenuExample::undo()
{
    emit explain( "Edit/Undo selected" );
}


void MenuExample::redo()
{
    emit explain( "Edit/Redo selected" );
}


void MenuExample::normal()
{
    isBold = false;
    isUnderline = false;
    TQFont font;
    label->setFont( font );
    menu->setItemChecked( boldID, isBold );
    menu->setItemChecked( underlineID, isUnderline );
    emit explain( "Options/Normal selected" );
}


void MenuExample::bold()
{
    isBold = !isBold;
    TQFont font;
    font.setBold( isBold );
    font.setUnderline( isUnderline );
    label->setFont( font );
    menu->setItemChecked( boldID, isBold );
    emit explain( "Options/Bold selected" );
}


void MenuExample::underline()
{
    isUnderline = !isUnderline;
    TQFont font;
    font.setBold( isBold );
    font.setUnderline( isUnderline );
    label->setFont( font );
    menu->setItemChecked( underlineID, isUnderline );
    emit explain( "Options/Underline selected" );
}


void MenuExample::about()
{
    TQMessageBox::about( this, "TQt Menu Example",
			"This example demonstrates simple use of TQt menus.\n"
			"You can cut and paste lines from it to your own\n"
			"programs." );
}


void MenuExample::aboutTQt()
{
    TQMessageBox::aboutTQt( this, "TQt Menu Example" );
}


void MenuExample::printer()
{
    emit explain( "File/Printer/Print selected" );
}

void MenuExample::file()
{
    emit explain( "File/Printer/Print To File selected" );
}

void MenuExample::fax()
{
    emit explain( "File/Printer/Print To Fax selected" );
}

void MenuExample::printerSetup()
{
    emit explain( "File/Printer/Printer Setup selected" );
}


void MenuExample::resizeEvent( TQResizeEvent * )
{
    label->setGeometry( 20, rect().center().y()-20, width()-40, 40 );
}


int main( int argc, char ** argv )
{
    TQApplication a( argc, argv );
    MenuExample m;
    m.setCaption("TQt Examples - Menus");
    a.setMainWidget( &m );
    m.show();
    return a.exec();
}
