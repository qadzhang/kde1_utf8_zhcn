/****************************************************************************
**
** Copyright (C) 1992-2008 Trolltech ASA.  All rights reserved.
**
** This file is part of an example program for TQt.  This example
** program may be used, distributed and modified without limitation.
**
*****************************************************************************/

#include <ntqmessagebox.h>
#include <ntqpixmap.h>
#include <ntqlayout.h>
#include <ntqapplication.h>

// Standard TQt widgets

#include <ntqtoolbar.h>
#include <ntqmenubar.h>
#include <ntqpopupmenu.h>
#include <ntqbuttongroup.h>
#include <ntqcheckbox.h>
#include <ntqcombobox.h>
#include <ntqframe.h>
#include <ntqgroupbox.h>
#include <ntqlabel.h>
#include <ntqlcdnumber.h>
#include <ntqmultilineedit.h>
#include <ntqlineedit.h>
#include <ntqlistbox.h>
#include <ntqpushbutton.h>
#include <ntqradiobutton.h>
#include <ntqslider.h>
#include <ntqtooltip.h>
#include <ntqspinbox.h>
#include <ntqstatusbar.h>
#include <ntqwhatsthis.h>
#include <ntqtoolbutton.h>
#include <ntqvbox.h>
#include <ntqtabbar.h>
#include <ntqtabwidget.h>
#include <ntqwidgetstack.h>
#include <ntqprogressbar.h>
#include <ntqsplitter.h>
#include <ntqlistview.h>
#include <ntqheader.h>
#include <ntqtextbrowser.h>
#include <ntqfiledialog.h>
#include <ntqaccel.h>
#include <ntqmetaobject.h>
#include <ntqpainter.h>

#include "widgets.h"


// Some sample widgets

#include "../aclock/aclock.h"
#include "../dclock/dclock.h"


#define MOVIEFILENAME "trolltech.gif"

#include "../application/fileopen.xpm"
#include "../application/filesave.xpm"
#include "../application/fileprint.xpm"


class MyWhatsThis : public TQWhatsThis
{
public:
    MyWhatsThis( TQListBox* lb)
	: TQWhatsThis( lb ) { listbox = lb; };
    ~MyWhatsThis(){};


    TQString text( const TQPoint & p) {
	TQListBoxItem* i = listbox->itemAt( p );
	if ( i && i->pixmap() ) {
	    return "Isn't that a <em>wonderful</em> pixmap? <br>" \
		"Imagine, you could even decorate a" \
		" <b>red</b> pushbutton with it! :-)";
	}
	return "This is a TQListBox.";
    }

private:
    TQListBox* listbox;
};


class MyMenuItem : public TQCustomMenuItem
{
public:
    MyMenuItem( const TQString& s, const TQFont& f )
	: string( s ), font( f ){};
    ~MyMenuItem(){}

    void paint( TQPainter* p, const TQColorGroup& /*cg*/, bool /*act*/,
		bool /*enabled*/, int x, int y, int w, int h )
    {
	p->setFont ( font );
	p->drawText( x, y, w, h,
		     AlignAuto | AlignVCenter | ShowPrefix | DontClip,
		     string );
    }

    TQSize sizeHint()
    {
	return TQFontMetrics( font ).size( AlignAuto | AlignVCenter |
					  ShowPrefix | DontClip,  string );
    }
private:
    TQString string;
    TQFont font;
};

//
// Construct the WidgetView with children
//

WidgetView::WidgetView( TQWidget *parent, const char *name )
    : TQMainWindow( parent, name )
{
    TQColor col;

    // Set the window caption/title
    setCaption( "TQt Example - Widgets Demo Application" );

    // create a toolbar
    TQToolBar *tools = new TQToolBar( this, "toolbar" );

    // put something in it
    TQPixmap openIcon( fileopen );
    TQToolButton * toolb = new TQToolButton( openIcon, "toolbutton 1",
					   TQString::null, this, TQ_SLOT(open()),
					   tools, "open file" );
    TQWhatsThis::add( toolb, "This is a <b>TQToolButton</b>. It lives in a "
		     "TQToolBar. This particular button doesn't do anything "
		     "useful." );

    TQPixmap saveIcon( filesave );
    toolb = new TQToolButton( saveIcon, "toolbutton 2", TQString::null,
			     this, TQ_SLOT(dummy()),
			     tools, "save file" );
    TQWhatsThis::add( toolb, "This is also a <b>TQToolButton</b>." );

    TQPixmap  printIcon( fileprint );
    toolb = new TQToolButton( printIcon, "toolbutton 3", TQString::null,
			     this, TQ_SLOT(dummy()),
			     tools, "print file" );
    TQWhatsThis::add( toolb, "This is the third <b>TQToolButton</b>.");

    toolb = TQWhatsThis::whatsThisButton( tools );
    TQWhatsThis::add( toolb, "This is a <b>What's This</b> button "
		     "It enables the user to ask for help "
		     "about widgets on the screen.");

    // Install an application-global event filter to catch control+leftbutton
    tqApp->installEventFilter( this );

    //make a central widget to contain the other widgets
    central = new TQWidget( this );
    setCentralWidget( central );

    // Create a layout to position the widgets
    TQHBoxLayout *topLayout = new TQHBoxLayout( central, 10 );

    // Create a grid layout to hold most of the widgets
    TQGridLayout *grid = new TQGridLayout( 0, 3 ); //3 wide and autodetect number of rows
    topLayout->addLayout( grid, 1 );

    // Create an easter egg
    TQToolTip::add( menuBar(), TQRect( 0, 0, 2, 2 ), "easter egg" );

    TQPopupMenu* popup;
    popup = new TQPopupMenu( this );
    menuBar()->insertItem( "&File", popup );
    int id;
    id = popup->insertItem( "&New" );
    popup->setItemEnabled( id, false );
    id = popup->insertItem( openIcon, "&Open...", this, TQ_SLOT( open() ) );

    popup->insertSeparator();
    popup->insertItem( "Quit", tqApp, TQ_SLOT(quit()), CTRL+Key_Q );

    textStylePopup = popup = new TQPopupMenu( this );
    menuBar()->insertItem( "&Edit", popup );

    plainStyleID = id = popup->insertItem( "&Plain" );
    popup->setAccel( CTRL+Key_T, id );

    popup->insertSeparator();
    TQFont f = font();
    f.setBold( true );
    id = popup->insertItem( new MyMenuItem( "&Bold", f ) );
    popup->setAccel( CTRL+Key_B, id );
    f = font();
    f.setItalic( true );
    id = popup->insertItem( new MyMenuItem( "&Italic", f ) );
    popup->setItemChecked( id, true );
    popup->setAccel( CTRL+Key_I, id );
    f = font();
    f.setUnderline( true );
    id = popup->insertItem( new MyMenuItem( "&Underline", f ) );
    popup->setAccel( CTRL+Key_U, id );
    f = font();
    f.setStrikeOut( true );
    id = popup->insertItem( new MyMenuItem( "&Strike", f ) );
    connect( textStylePopup, TQ_SIGNAL(activated(int)),
	     this, TQ_SLOT(popupSelected(int)) );

    // Create an analog and a digital clock
    AnalogClock  *aclock = new AnalogClock( central );
    aclock->setAutoMask( true );
    DigitalClock *dclock = new DigitalClock( central );
    dclock->setMaximumWidth(200);
    grid->addWidget( aclock, 0, 2 );
    grid->addWidget( dclock, 1, 2 );

    // Give the dclock widget a blue palette
    col.setRgb( 0xaa, 0xbe, 0xff );
    dclock->setPalette( TQPalette( col ) );

    // make tool tips for both of them
    TQToolTip::add( aclock, "custom widget: analog clock" );
    TQToolTip::add( dclock, "custom widget: digital clock" );

    // Create a push button.
    TQPushButton *pb;
    pb = new TQPushButton( "&Push button 1", central, "button1" );
    grid->addWidget( pb, 0, 0, AlignVCenter );
    connect( pb, TQ_SIGNAL(clicked()), TQ_SLOT(button1Clicked()) );
    TQToolTip::add( pb, "push button 1" );
    TQWhatsThis::add( pb, "This is a <b>TQPushButton</b>.<br>"
		     "Click it and watch...<br>"
		     "The wonders of modern technology.");

    TQPixmap pm;
    bool pix = pm.load("qt.png");
    if ( !pix ) {
	TQMessageBox::information( 0, "TQt Widgets Example",
				  "Could not load the file \"qt.png\", which\n"
				  "contains an icon used...\n\n"
				  "The text \"line 42\" will be substituted.",
				  TQMessageBox::Ok + TQMessageBox::Default );
    }

    // Create a label containing a TQMovie
    movie = TQMovie( MOVIEFILENAME );
    movielabel = new TQLabel( central, "label0" );
    movie.connectStatus(this, TQ_SLOT(movieStatus(int)));
    movie.connectUpdate(this, TQ_SLOT(movieUpdate(const TQRect&)));
    movielabel->setFrameStyle( TQFrame::Box | TQFrame::Plain );
    movielabel->setMovie( movie );
    movielabel->setFixedSize( 128+movielabel->frameWidth()*2,
			      64+movielabel->frameWidth()*2 );
    grid->addWidget( movielabel, 0, 1, AlignCenter );
    TQToolTip::add( movielabel, "movie" );
    TQWhatsThis::add( movielabel, "This is a <b>TQLabel</b> "
		     "that contains a TQMovie." );

    // Create a group of check boxes
    bg = new TQButtonGroup( central, "checkGroup" );
    bg->setTitle( "Check Boxes" );
    grid->addWidget( bg, 1, 0 );

    // Create a layout for the check boxes
    TQVBoxLayout *vbox = new TQVBoxLayout(bg, 10);

    vbox->addSpacing( bg->fontMetrics().height() );

    cb[0] = new TQCheckBox( bg );
    cb[0]->setText( "&Read" );
    vbox->addWidget( cb[0] );
    cb[1] = new TQCheckBox( bg );
    cb[1]->setText( "&Write" );
    vbox->addWidget( cb[1] );
    cb[2] = new TQCheckBox( bg );
    cb[2]->setText( "&Execute" );
    vbox->addWidget( cb[2] );

    connect( bg, TQ_SIGNAL(clicked(int)), TQ_SLOT(checkBoxClicked(int)) );

    TQToolTip::add( cb[0], "check box 1" );
    TQToolTip::add( cb[1], "check box 2" );
    TQToolTip::add( cb[2], "check box 3" );

    // Create a group of radio buttons
    TQRadioButton *rb;
    bg = new TQButtonGroup( central, "radioGroup" );
    bg->setTitle( "Radio buttons" );

    grid->addWidget( bg, 1, 1 );

    // Create a layout for the radio buttons
    vbox = new TQVBoxLayout(bg, 10);

    vbox->addSpacing( bg->fontMetrics().height() );
    rb = new TQRadioButton( bg );
    rb->setText( "&AM" );
    rb->setChecked( true );
    vbox->addWidget(rb);
    TQToolTip::add( rb, "radio button 1" );
    rb = new TQRadioButton( bg );
    rb->setText( "F&M" );
    vbox->addWidget(rb);
    TQToolTip::add( rb, "radio button 2" );
    rb = new TQRadioButton( bg );
    rb->setText( "&Short Wave" );
    vbox->addWidget(rb);

    connect( bg, TQ_SIGNAL(clicked(int)), TQ_SLOT(radioButtonClicked(int)) );
    TQToolTip::add( rb, "radio button 3" );

    // Create a list box
    TQListBox *lb = new TQListBox( central, "listBox" );
    for ( int i=0; i<100; i++ ) {		// fill list box
	TQString str;
	str.sprintf( "line %d", i );
	if ( i == 42 && pix )
	    lb->insertItem( pm );
	else
	    lb->insertItem( str );
    }
    grid->addMultiCellWidget( lb, 2, 4, 0, 0 );
    connect( lb, TQ_SIGNAL(selected(int)), TQ_SLOT(listBoxItemSelected(int)) );
    TQToolTip::add( lb, "list box" );
    (void)new MyWhatsThis( lb );

    vbox = new TQVBoxLayout(8);
    grid->addLayout( vbox, 2, 1 );

    // Create a slider
    TQSlider *sb = new TQSlider( 0, 300, 30, 100, TQSlider::Horizontal,
			       central, "Slider" );
    sb->setTickmarks( TQSlider::Below );
    sb->setTickInterval( 10 );
    sb->setFocusPolicy( TQWidget::TabFocus );
    vbox->addWidget( sb );

    connect( sb, TQ_SIGNAL(valueChanged(int)), TQ_SLOT(sliderValueChanged(int)) );
    TQToolTip::add( sb, "slider" );
    TQWhatsThis::add( sb, "This is a <b>TQSlider</b>. "
		     "The tick marks are optional."
		     " This slider controls the speed of the movie." );
    // Create a combo box
    TQComboBox *combo = new TQComboBox( false, central, "comboBox" );
    combo->insertItem( "darkBlue" );
    combo->insertItem( "darkRed" );
    combo->insertItem( "darkGreen" );
    combo->insertItem( "blue" );
    combo->insertItem( "red" );
    vbox->addWidget( combo );
    connect( combo, TQ_SIGNAL(activated(int)),
	     this, TQ_SLOT(comboBoxItemActivated(int)) );
    TQToolTip::add( combo, "read-only combo box" );

    // Create an editable combo box
    TQComboBox *edCombo = new TQComboBox( true, central, "edComboBox" );
    TQListBox *edComboLst = new TQListBox(this);
    edCombo->setListBox(edComboLst);
    edComboLst->insertItem( "Permutable" );
    edComboLst->insertItem( "Malleable" );
    edComboLst->insertItem( "Adaptable" );
    edComboLst->insertItem( "Alterable" );
    edComboLst->insertItem( "Inconstant" );
    vbox->addWidget( edCombo );
    connect( edCombo, TQ_SIGNAL(activated(const TQString&)),
	     this, TQ_SLOT(edComboBoxItemActivated(const TQString&)) );
    TQToolTip::add( edCombo, "editable combo box" );

    edCombo->setAutoCompletion( true );

    vbox = new TQVBoxLayout(8);
    grid->addLayout( vbox, 2, 2 );

    // Create a spin box
    TQSpinBox *spin = new TQSpinBox( 0, 10, 1, central, "spin" );
    spin->setSuffix(" mm");
    spin->setSpecialValueText( "Auto" );
    connect( spin, TQ_SIGNAL( valueChanged(const TQString&) ),
	     TQ_SLOT( spinBoxValueChanged(const TQString&) ) );
    TQToolTip::add( spin, "spin box" );
    TQWhatsThis::add( spin, "This is a <b>TQSpinBox</b>. "
		     "You can chose values in a given range "
		     "either by using the arrow buttons "
		     "or by typing them in." );
    vbox->addWidget( spin );

    vbox->addStretch( 1 );

    // Create a tabwidget that switches between multi line edits
    tabs = new TQTabWidget( central );
    //tabs->setTabPosition( TQTabWidget::Bottom );
    tabs->setMargin( 4 );
    grid->addMultiCellWidget( tabs, 3, 3, 1, 2 );
    TQMultiLineEdit *mle = new TQMultiLineEdit( tabs, "multiLineEdit" );
    edit = mle;
    mle->setWordWrap( TQMultiLineEdit::WidgetWidth );
    mle->setText("This is a TQMultiLineEdit widget, "
	         "useful for small multi-line "
		 "input fields.");
    TQToolTip::add( mle, "multi line editor" );

    tabs->addTab( mle, "F&irst");

    mle = new TQMultiLineEdit( tabs, "multiLineEdit" );
    TQString mleText = "This is another TQMultiLineEdit widget.";
#if 1
    mleText += "\n";
    mleText += "Japanese: ";
    mleText += TQChar((ushort)0x6a38); // Kanji
    mleText += "\n";
    mleText += "Russian: ";
    mleText += TQChar((ushort)0x042e); // Cyrillic
    mleText += "\n";
    mleText += "Norwegian: ";
    mleText += TQChar((ushort)0x00d8); // Norwegian
    mleText += "\n";
    mleText += "Unicode (black square): ";
    mleText += TQChar((ushort)0x25A0); // BLACK SQUARE
    mleText += "\n";
#endif
    mle->setText( mleText );
    TQToolTip::add( mle, "second multi line editor" );
    tabs->addTab( mle, "Se&cond");


    // Create a single line edit
    TQLineEdit *le = new TQLineEdit( central, "lineEdit" );


    grid->addMultiCellWidget( le, 4, 4, 1, 2 );
    connect( le, TQ_SIGNAL(textChanged(const TQString&)),
	     TQ_SLOT(lineEditTextChanged(const TQString&)) );
    TQToolTip::add( le, "single line editor" );
    TQWhatsThis::add( le, "This is a <b>TQLineEdit</b>, you can enter a "
		     "single line of text in it. "
		      "It also it accepts text drops." );

    grid->setRowStretch(0,0);
    grid->setRowStretch(1,0);
    grid->setRowStretch(2,0);
    grid->setRowStretch(3,1);
    grid->setRowStretch(4,0);

    grid->setColStretch(0,1);
    grid->setColStretch(1,1);
    grid->setColStretch(2,1);


    TQSplitter *split = new TQSplitter( Vertical, central, "splitter" );
    split->setOpaqueResize( true );
    topLayout->addWidget( split, 1 );
    TQListView *lv = new MyListView( split );
    connect(lv, TQ_SIGNAL(selectionChanged() ),
	    this, TQ_SLOT( selectionChanged() ) );
    connect(lv, TQ_SIGNAL(selectionChanged(TQListViewItem*) ),
	    this, TQ_SLOT( selectionChanged(TQListViewItem*) ) );
    connect(lv, TQ_SIGNAL(clicked(TQListViewItem*) ),
	    this, TQ_SLOT( clicked(TQListViewItem*) ) );
    connect(lv, TQ_SIGNAL(mySelectionChanged(TQListViewItem*) ),
	    this, TQ_SLOT( mySelectionChanged(TQListViewItem*) ) );
    lv->addColumn( "One" );
    lv->addColumn( "Two" );
    lv->setAllColumnsShowFocus( true );

    TQListViewItem *lvi=  new TQListViewItem( lv, "Text", "Text" );
    lvi=  new TQListViewItem( lv, "Text", "Other Text" );
    lvi=  new TQListViewItem( lv, "Text", "More Text" );
    lvi=  new TQListViewItem( lv, "Text", "Extra Text" );
    lvi->setOpen(true);
    (void)new TQListViewItem( lvi, "SubText", "Additional Text" );
    lvi=  new TQListViewItem( lvi, "SubText", "Side Text" );
    lvi=  new TQListViewItem( lvi, "SubSubText", "Complimentary Text" );

    TQToolTip::add( lv, "list view" );
    TQWhatsThis::add( lv, "This is a <b>TQListView</b>, you can display lists "
		     "(or outline lists) of multiple-column data in it." );

    lv = new TQListView( split );
    lv->addColumn( "Choices" );
    (void) new TQCheckListItem( lv, "Onion", TQCheckListItem::CheckBox );
    (void) new TQCheckListItem( lv, "Artichoke", TQCheckListItem::CheckBox );
    (void) new TQCheckListItem( lv, "Pepper", TQCheckListItem::CheckBox );
    (void) new TQCheckListItem( lv, "Habaneros", TQCheckListItem::CheckBox );
    (void) new TQCheckListItem( lv, "Pineapple", TQCheckListItem::CheckBox );
    (void) new TQCheckListItem( lv, "Ham", TQCheckListItem::CheckBox );
    (void) new TQCheckListItem( lv, "Pepperoni", TQCheckListItem::CheckBox );
    (void) new TQCheckListItem( lv, "Garlic", TQCheckListItem::CheckBox );


    TQCheckListItem *lit = new TQCheckListItem( lv, "Cheese" );
    lit->setOpen( true );
    (void) new TQCheckListItem( lit, "Cheddar", TQCheckListItem::RadioButton );
    (void) new TQCheckListItem( lit, "Mozarella", TQCheckListItem::RadioButton );
    (void) new TQCheckListItem( lit, "Jarlsberg", TQCheckListItem::RadioButton );

    TQToolTip::add( lv, "list view" );
    TQWhatsThis::add( lv, "This is also a <b>TQListView</b>, with "
		     "interactive items." );

    TQTextBrowser *browser =  new TQTextBrowser( split );
    browser->setText( "<h1>TQTextBrowser</h1>"
		   "<p>TQt supports formatted rich text, such "
		   "as the heading above, <em>emphasized</em> and "
		   "<b>bold</b> text, via an XML subset.</p> "
		   "<p><a href=\"nogo://some.where.com\">Hypertext navigation</a> and style sheets are supported.</p>", "" );
    browser->setFont(TQFont("Charter",11));
    browser->setFrameStyle( TQFrame::WinPanel | TQFrame::Sunken );
    connect( browser, TQ_SIGNAL(linkClicked(const TQString&)), browser, TQ_SLOT(setText(const TQString&)) );

    // Create an label and a message in the status bar
    // The message is updated when buttons are clicked etc.
    msg = new TQLabel( statusBar(), "message" );
    msg->setAlignment( AlignCenter );
    TQFont boldfont; boldfont.setWeight(TQFont::Bold);
    msg->setFont( boldfont );
    statusBar()->addWidget( msg, 4 );
    TQToolTip::add( msg, "Message area" );

    TQAccel* a = new TQAccel( this );
    a->connectItem(  a->insertItem( Key_F9 ),
		     this, TQ_SLOT( showProperties() ) );

    prog = new TQProgressBar( statusBar(), "progress" );
    prog->setTotalSteps( 100 );
    progress = 64;
    prog->setProgress( progress );
    statusBar()->addWidget( prog , 1 );
    TQWhatsThis::add( prog, "This is a <b>TQProgressBar</b> "
		     "You can use it to show that a lengthy "
		     " process is progressing. "
		     "In this program, nothing much seems to happen." );
    statusBar()->message( "Welcome to TQt", 2000 );
}

void WidgetView::setStatus(const TQString& text)
{
    msg->setText(text);
}

void WidgetView::button1Clicked()
{
    msg->setText( "The push button was clicked" );
    prog->setProgress( ++progress );
}


void WidgetView::movieUpdate( const TQRect& )
{
    // Uncomment this to test animated icons on your window manager
    //setIcon( movie.framePixmap() );
}

void WidgetView::movieStatus( int s )
{
    switch ( s ) {
      case TQMovie::SourceEmpty:
      case TQMovie::UnrecognizedFormat:
	{
	    TQPixmap pm("tt-logo.png");
	    movielabel->setPixmap(pm);
	    movielabel->setFixedSize(pm.size());
	}
      break;
      default:
	if ( movielabel->movie() )	 	// for flicker-free animation:
	    movielabel->setBackgroundMode( NoBackground );
    }
}


void WidgetView::popupSelected( int selectedId )
{
    if ( selectedId == plainStyleID ) {
	for ( int i = 0; i < int(textStylePopup->count()); i++ ) {
	    int id = textStylePopup->idAt( i );
	    textStylePopup->setItemChecked( id, false);
	}
    } else {
	textStylePopup->setItemChecked( selectedId, true );
    }
}

void WidgetView::checkBoxClicked( int id )
{
    TQString str;
    str = tr("Check box %1 clicked : ").arg(id);
    TQString chk = "---";
    if ( cb[0]->isChecked() )
	chk[0] = 'r';
    if ( cb[1]->isChecked() )
	chk[1] = 'w';
    if ( cb[2]->isChecked() )
	chk[2] = 'x';
    str += chk;
    msg->setText( str );
}


void WidgetView::edComboBoxItemActivated( const TQString& text)
{
    TQString str = tr("Editable Combo Box set to ");
    str += text;
    msg->setText( str );
}


void WidgetView::radioButtonClicked( int id )
{
    msg->setText( tr("Radio button #%1 clicked").arg(id) );
}


void WidgetView::listBoxItemSelected( int index )
{
    msg->setText( tr("List box item %1 selected").arg(index) );
}


void WidgetView::sliderValueChanged( int value )
{
    msg->setText( tr("Movie set to %1% of normal speed").arg(value) );
    movie.setSpeed( value );
}


void WidgetView::comboBoxItemActivated( int index )
{
    msg->setText( tr("Combo box item %1 activated").arg(index) );
    switch ( index ) {
    default:
    case 0:
	TQApplication::setWinStyleHighlightColor( darkBlue );
	break;
    case 1:
	TQApplication::setWinStyleHighlightColor( darkRed );
	break;
    case 2:
	TQApplication::setWinStyleHighlightColor( darkGreen );
	break;
    case 3:
	TQApplication::setWinStyleHighlightColor( blue );
	break;
    case 4:
	TQApplication::setWinStyleHighlightColor( red );
	break;
    }
}



void WidgetView::lineEditTextChanged( const TQString& newText )
{
    TQString str( "Line edit text: ");
    str += newText;
    if ( newText.length() == 1 ) {
	TQString u;
	u.sprintf(" (U%02x%02x)", newText[0].row(), newText[0].cell() );
	str += u;
    }
    msg->setText( str );
}


void WidgetView::spinBoxValueChanged( const TQString& valueText )
{
    TQString str( "Spin box value: " );
    str += valueText;
    msg->setText( str );
}

//
// All application events are passed through this event filter.
// We're using it to display some information about a clicked
// widget (right mouse button + CTRL).
//

bool WidgetView::eventFilter( TQObject *obj, TQEvent *event )
{
    static bool identify_now = true;
    if ( event->type() == TQEvent::MouseButtonPress && identify_now ) {
	TQMouseEvent *e = (TQMouseEvent*)event;
	if ( e->button() == TQMouseEvent::RightButton &&
	     (e->state() & TQMouseEvent::ControlButton) != 0 ){
	    TQString str = "The clicked widget is a\n";
	    str += obj->className();
	    str += "\nThe widget's name is\n";
	    if ( obj->name() )
		str += obj->name();
	    else
		str += "<no name>";
	    identify_now = false;		// don't do it in message box
	    TQMessageBox::information( (TQWidget*)obj, "Identify Widget", str );
	    identify_now = true;		// allow it again
	}
    }
    return TQMainWindow::eventFilter( obj, event ); // don't eat event
}


void WidgetView::open()
{
    TQFileDialog::getOpenFileName( TQString::null, "Textfiles (*.txt)", this );
}


void WidgetView::dummy()
{
    TQMessageBox::information( this, "Sorry",
			      "This function is not implemented" );
}

void WidgetView::selectionChanged()
{
    //tqDebug("selectionChanged");
}
void WidgetView::selectionChanged( TQListViewItem* /*item*/)
{
    //tqDebug("selectionChanged %p", item );
}

void WidgetView::clicked( TQListViewItem* /*item*/ )
{
    //tqDebug("clicked %p", item );
}

void WidgetView::mySelectionChanged( TQListViewItem* /*item*/ )
{
    //tqDebug("mySelectionChanged %p", item );
}

void WidgetView::showProperties()
{
    if ( !tqApp->focusWidget() )
	return;
    TQCString output;
    output.sprintf( "Properties for class '%s'",
		    tqApp->focusWidget()->className() );
    int i = 0;
    while( i < (int) tqApp->focusWidget()->metaObject()->numProperties( true ) ) {
	const TQMetaProperty* p
	    = tqApp->focusWidget()->metaObject()->property( i, true );
	TQCString tmp;
	tmp.sprintf( "\n %2d: %s (read-%s, %s)", ++i, p->name(),
		     p->writable() ? "write" : "only", p->type() );
	output += tmp;
    }
    tqDebug( output );
}
