#include "canvasview.h"
#include "chartform.h"
#include "optionsform.h"
#include "setdataform.h"

#include <ntqaction.h>
#include <ntqapplication.h>
#include <ntqcombobox.h>
#include <ntqfile.h>
#include <ntqfiledialog.h>
#include <ntqfont.h>
#include <ntqfontdialog.h>
#include <ntqmenubar.h>
#include <ntqmessagebox.h>
#include <ntqpixmap.h>
#include <ntqpopupmenu.h>
#include <ntqprinter.h>
#include <ntqradiobutton.h>
#include <ntqsettings.h>
#include <ntqspinbox.h>
#include <ntqstatusbar.h>
#include <ntqtoolbar.h>
#include <ntqtoolbutton.h>

#include "images/file_new.xpm"
#include "images/file_open.xpm"
#include "images/file_save.xpm"
#include "images/file_print.xpm"
#include "images/options_setdata.xpm"
#include "images/options_setfont.xpm"
#include "images/options_setoptions.xpm"
#include "images/options_horizontalbarchart.xpm"
#include "images/options_piechart.xpm"
#include "images/options_verticalbarchart.xpm"


const TQString WINDOWS_REGISTRY = "/Trolltech/TQtExamples";
const TQString APP_KEY = "/Chart/";


ChartForm::ChartForm( const TQString& filename )
    : TQMainWindow( 0, 0, WDestructiveClose )
{
    setIcon( TQPixmap( options_piechart ) );

    TQAction *fileNewAction;
    TQAction *fileOpenAction;
    TQAction *fileSaveAction;
    TQAction *fileSaveAsAction;
    TQAction *fileSaveAsPixmapAction;
    TQAction *filePrintAction;
    TQAction *fileQuitAction;
    TQAction *optionsSetDataAction;
    TQAction *optionsSetFontAction;
    TQAction *optionsSetOptionsAction;

    fileNewAction = new TQAction(
	    "New Chart", TQPixmap( file_new ),
	    "&New", CTRL+Key_N, this, "new" );
    connect( fileNewAction, TQ_SIGNAL( activated() ), this, TQ_SLOT( fileNew() ) );

    fileOpenAction = new TQAction(
	    "Open Chart", TQPixmap( file_open ),
	    "&Open...", CTRL+Key_O, this, "open" );
    connect( fileOpenAction, TQ_SIGNAL( activated() ), this, TQ_SLOT( fileOpen() ) );

    fileSaveAction = new TQAction(
	    "Save Chart", TQPixmap( file_save ),
	    "&Save", CTRL+Key_S, this, "save" );
    connect( fileSaveAction, TQ_SIGNAL( activated() ), this, TQ_SLOT( fileSave() ) );

    fileSaveAsAction = new TQAction(
	    "Save Chart As", TQPixmap( file_save ),
	    "Save &As...", 0, this, "save as" );
    connect( fileSaveAsAction, TQ_SIGNAL( activated() ),
	     this, TQ_SLOT( fileSaveAs() ) );

    fileSaveAsPixmapAction = new TQAction(
	    "Save Chart As Bitmap", TQPixmap( file_save ),
	    "Save As &Bitmap...", CTRL+Key_B, this, "save as bitmap" );
    connect( fileSaveAsPixmapAction, TQ_SIGNAL( activated() ),
	     this, TQ_SLOT( fileSaveAsPixmap() ) );

    filePrintAction = new TQAction(
	    "Print Chart", TQPixmap( file_print ),
	    "&Print Chart...", CTRL+Key_P, this, "print chart" );
    connect( filePrintAction, TQ_SIGNAL( activated() ),
	     this, TQ_SLOT( filePrint() ) );

    optionsSetDataAction = new TQAction(
	    "Set Data", TQPixmap( options_setdata ),
	    "Set &Data...", CTRL+Key_D, this, "set data" );
    connect( optionsSetDataAction, TQ_SIGNAL( activated() ),
	     this, TQ_SLOT( optionsSetData() ) );


    TQActionGroup *chartGroup = new TQActionGroup( this ); // Connected later
    chartGroup->setExclusive( true );

    optionsPieChartAction = new TQAction(
	    "Pie Chart", TQPixmap( options_piechart ),
	    "&Pie Chart", CTRL+Key_I, chartGroup, "pie chart" );
    optionsPieChartAction->setToggleAction( true );

    optionsHorizontalBarChartAction = new TQAction(
	    "Horizontal Bar Chart", TQPixmap( options_horizontalbarchart ),
	    "&Horizontal Bar Chart", CTRL+Key_H, chartGroup,
	    "horizontal bar chart" );
    optionsHorizontalBarChartAction->setToggleAction( true );

    optionsVerticalBarChartAction = new TQAction(
	    "Vertical Bar Chart", TQPixmap( options_verticalbarchart ),
	    "&Vertical Bar Chart", CTRL+Key_V, chartGroup, "Vertical bar chart" );
    optionsVerticalBarChartAction->setToggleAction( true );


    optionsSetFontAction = new TQAction(
	    "Set Font", TQPixmap( options_setfont ),
	    "Set &Font...", CTRL+Key_F, this, "set font" );
    connect( optionsSetFontAction, TQ_SIGNAL( activated() ),
	     this, TQ_SLOT( optionsSetFont() ) );

    optionsSetOptionsAction = new TQAction(
	    "Set Options", TQPixmap( options_setoptions ),
	    "Set &Options...", 0, this, "set options" );
    connect( optionsSetOptionsAction, TQ_SIGNAL( activated() ),
	     this, TQ_SLOT( optionsSetOptions() ) );

    fileQuitAction = new TQAction( "Quit", "&Quit", CTRL+Key_Q, this, "quit" );
    connect( fileQuitAction, TQ_SIGNAL( activated() ), this, TQ_SLOT( fileQuit() ) );


    TQToolBar* fileTools = new TQToolBar( this, "file operations" );
    fileTools->setLabel( "File Operations" );
    fileNewAction->addTo( fileTools );
    fileOpenAction->addTo( fileTools );
    fileSaveAction->addTo( fileTools );
    fileTools->addSeparator();
    filePrintAction->addTo( fileTools );

    TQToolBar *optionsTools = new TQToolBar( this, "options operations" );
    optionsTools->setLabel( "Options Operations" );
    optionsSetDataAction->addTo( optionsTools );
    optionsTools->addSeparator();
    optionsPieChartAction->addTo( optionsTools );
    optionsHorizontalBarChartAction->addTo( optionsTools );
    optionsVerticalBarChartAction->addTo( optionsTools );
    optionsTools->addSeparator();
    optionsSetFontAction->addTo( optionsTools );
    optionsTools->addSeparator();
    optionsSetOptionsAction->addTo( optionsTools );

    fileMenu = new TQPopupMenu( this );
    menuBar()->insertItem( "&File", fileMenu );
    fileNewAction->addTo( fileMenu );
    fileOpenAction->addTo( fileMenu );
    fileSaveAction->addTo( fileMenu );
    fileSaveAsAction->addTo( fileMenu );
    fileMenu->insertSeparator();
    fileSaveAsPixmapAction->addTo( fileMenu );
    fileMenu->insertSeparator();
    filePrintAction->addTo( fileMenu );
    fileMenu->insertSeparator();
    fileQuitAction->addTo( fileMenu );

    optionsMenu = new TQPopupMenu( this );
    menuBar()->insertItem( "&Options", optionsMenu );
    optionsSetDataAction->addTo( optionsMenu );
    optionsMenu->insertSeparator();
    optionsPieChartAction->addTo( optionsMenu );
    optionsHorizontalBarChartAction->addTo( optionsMenu );
    optionsVerticalBarChartAction->addTo( optionsMenu );
    optionsMenu->insertSeparator();
    optionsSetFontAction->addTo( optionsMenu );
    optionsMenu->insertSeparator();
    optionsSetOptionsAction->addTo( optionsMenu );

    menuBar()->insertSeparator();

    TQPopupMenu *helpMenu = new TQPopupMenu( this );
    menuBar()->insertItem( "&Help", helpMenu );
    helpMenu->insertItem( "&Help", this, TQ_SLOT(helpHelp()), Key_F1 );
    helpMenu->insertItem( "&About", this, TQ_SLOT(helpAbout()) );
    helpMenu->insertItem( "About &TQt", this, TQ_SLOT(helpAboutTQt()) );


    m_printer = 0;
    m_elements.resize( MAX_ELEMENTS );

    TQSettings settings;
    settings.insertSearchPath( TQSettings::Windows, WINDOWS_REGISTRY );
    int windowWidth = settings.readNumEntry( APP_KEY + "WindowWidth", 460 );
    int windowHeight = settings.readNumEntry( APP_KEY + "WindowHeight", 530 );
    int windowX = settings.readNumEntry( APP_KEY + "WindowX", -1 );
    int windowY = settings.readNumEntry( APP_KEY + "WindowY", -1 );
    setChartType( ChartType(
	    settings.readNumEntry( APP_KEY + "ChartType", int(PIE) ) ) );
    m_addValues = AddValuesType(
		    settings.readNumEntry( APP_KEY + "AddValues", int(NO) ));
    m_decimalPlaces = settings.readNumEntry( APP_KEY + "Decimals", 2 );
    m_font = TQFont( "Helvetica", 18, TQFont::Bold );
    m_font.fromString(
	    settings.readEntry( APP_KEY + "Font", m_font.toString() ) );
    for ( int i = 0; i < MAX_RECENTFILES; ++i ) {
	TQString filename = settings.readEntry( APP_KEY + "File" +
					       TQString::number( i + 1 ) );
	if ( !filename.isEmpty() )
	    m_recentFiles.push_back( filename );
    }
    if ( m_recentFiles.count() )
	updateRecentFilesMenu();


    // Connect *after* we've set the chart type on so we don't call
    // drawElements() prematurely.
    connect( chartGroup, TQ_SIGNAL( selected(TQAction*) ),
	     this, TQ_SLOT( updateChartType(TQAction*) ) );

    resize( windowWidth, windowHeight );
    if ( windowX != -1 || windowY != -1 )
	move( windowX, windowY );

    m_canvas = new TQCanvas( this );
    m_canvas->resize( width(), height() );
    m_canvasView = new CanvasView( m_canvas, &m_elements, this );
    setCentralWidget( m_canvasView );
    m_canvasView->show();

    if ( !filename.isEmpty() )
	load( filename );
    else {
	init();
	m_elements[0].set( 20, red,    14, "Red" );
	m_elements[1].set( 70, cyan,    2, "Cyan",   darkGreen );
	m_elements[2].set( 35, blue,   11, "Blue" );
	m_elements[3].set( 55, yellow,  1, "Yellow", darkBlue );
	m_elements[4].set( 80, magenta, 1, "Magenta" );
	drawElements();
    }

    statusBar()->message( "Ready", 2000 );
}


ChartForm::~ChartForm()
{
    delete m_printer;
}


void ChartForm::init()
{
    setCaption( "Chart" );
    m_filename = TQString::null;
    m_changed = false;

    m_elements[0]  = Element( Element::INVALID, red );
    m_elements[1]  = Element( Element::INVALID, cyan );
    m_elements[2]  = Element( Element::INVALID, blue );
    m_elements[3]  = Element( Element::INVALID, yellow );
    m_elements[4]  = Element( Element::INVALID, green );
    m_elements[5]  = Element( Element::INVALID, magenta );
    m_elements[6]  = Element( Element::INVALID, darkYellow );
    m_elements[7]  = Element( Element::INVALID, darkRed );
    m_elements[8]  = Element( Element::INVALID, darkCyan );
    m_elements[9]  = Element( Element::INVALID, darkGreen );
    m_elements[10] = Element( Element::INVALID, darkMagenta );
    m_elements[11] = Element( Element::INVALID, darkBlue );
    for ( int i = 12; i < MAX_ELEMENTS; ++i ) {
	double x = (double(i) / MAX_ELEMENTS) * 360;
	int y = (int(x * 256) % 105) + 151;
	int z = ((i * 17) % 105) + 151;
	m_elements[i] = Element( Element::INVALID, TQColor( int(x), y, z, TQColor::Hsv ) );
    }
}

void ChartForm::closeEvent( TQCloseEvent * )
{
    fileQuit();
}


void ChartForm::fileNew()
{
    if ( okToClear() ) {
	init();
	drawElements();
    }
}


void ChartForm::fileOpen()
{
    if ( !okToClear() )
	return;

    TQString filename = TQFileDialog::getOpenFileName(
			    TQString::null, "Charts (*.cht)", this,
			    "file open", "Chart -- File Open" );
    if ( !filename.isEmpty() )
	load( filename );
    else
	statusBar()->message( "File Open abandoned", 2000 );
}


void ChartForm::fileSaveAs()
{
    TQString filename = TQFileDialog::getSaveFileName(
			    TQString::null, "Charts (*.cht)", this,
			    "file save as", "Chart -- File Save As" );
    if ( !filename.isEmpty() ) {
	int answer = 0;
	if ( TQFile::exists( filename ) )
	    answer = TQMessageBox::warning(
			    this, "Chart -- Overwrite File",
			    TQString( "Overwrite\n\'%1\'?" ).
				arg( filename ),
			    "&Yes", "&No", TQString::null, 1, 1 );
	if ( answer == 0 ) {
	    m_filename = filename;
	    updateRecentFiles( filename );
	    fileSave();
	    return;
	}
    }
    statusBar()->message( "Saving abandoned", 2000 );
}


void ChartForm::fileOpenRecent( int index )
{
    if ( !okToClear() )
	return;

    load( m_recentFiles[index] );
}


void ChartForm::updateRecentFiles( const TQString& filename )
{
    if ( m_recentFiles.find( filename ) != m_recentFiles.end() )
	return;

    m_recentFiles.push_back( filename );
    if ( m_recentFiles.count() > MAX_RECENTFILES )
	m_recentFiles.pop_front();

    updateRecentFilesMenu();
}


void ChartForm::updateRecentFilesMenu()
{
    for ( int i = 0; i < MAX_RECENTFILES; ++i ) {
	if ( fileMenu->findItem( i ) )
	    fileMenu->removeItem( i );
	if ( i < int(m_recentFiles.count()) )
	    fileMenu->insertItem( TQString( "&%1 %2" ).
				    arg( i + 1 ).arg( m_recentFiles[i] ),
				  this, TQ_SLOT( fileOpenRecent(int) ),
				  0, i );
    }
}


void ChartForm::fileQuit()
{
    if ( okToClear() ) {
	saveOptions();
        tqApp->exit( 0 );
    }
}


bool ChartForm::okToClear()
{
    if ( m_changed ) {
	TQString msg;
	if ( m_filename.isEmpty() )
	    msg = "Unnamed chart ";
	else
	    msg = TQString( "Chart '%1'\n" ).arg( m_filename );
	msg += "has been changed.";

	int x = TQMessageBox::information( this, "Chart -- Unsaved Changes",
					  msg, "&Save", "Cancel", "&Abandon",
					  0, 1 );
	switch( x ) {
	    case 0: // Save
		fileSave();
		break;
	    case 1: // Cancel
	    default:
		return false;
	    case 2: // Abandon
		break;
	}
    }

    return true;
}


void ChartForm::saveOptions()
{
    TQSettings settings;
    settings.insertSearchPath( TQSettings::Windows, WINDOWS_REGISTRY );
    settings.writeEntry( APP_KEY + "WindowWidth", width() );
    settings.writeEntry( APP_KEY + "WindowHeight", height() );
    settings.writeEntry( APP_KEY + "WindowX", x() );
    settings.writeEntry( APP_KEY + "WindowY", y() );
    settings.writeEntry( APP_KEY + "ChartType", int(m_chartType) );
    settings.writeEntry( APP_KEY + "AddValues", int(m_addValues) );
    settings.writeEntry( APP_KEY + "Decimals", m_decimalPlaces );
    settings.writeEntry( APP_KEY + "Font", m_font.toString() );
    for ( int i = 0; i < int(m_recentFiles.count()); ++i )
	settings.writeEntry( APP_KEY + "File" + TQString::number( i + 1 ),
			     m_recentFiles[i] );
}


void ChartForm::optionsSetData()
{
    SetDataForm *setDataForm = new SetDataForm( &m_elements, m_decimalPlaces, this );
    if ( setDataForm->exec() ) {
	m_changed = true;
	drawElements();
    }
    delete setDataForm;
}


void ChartForm::setChartType( ChartType chartType )
{
    m_chartType = chartType;
    switch ( m_chartType ) {
	case PIE:
	    optionsPieChartAction->setOn( true );
	    break;
	case VERTICAL_BAR:
	    optionsVerticalBarChartAction->setOn( true );
	    break;
	case HORIZONTAL_BAR:
	    optionsHorizontalBarChartAction->setOn( true );
	    break;
    }
}


void ChartForm::updateChartType( TQAction *action )
{
    if ( action == optionsPieChartAction ) {
	m_chartType = PIE;
    }
    else if ( action == optionsHorizontalBarChartAction ) {
	m_chartType = HORIZONTAL_BAR;
    }
    else if ( action == optionsVerticalBarChartAction ) {
	m_chartType = VERTICAL_BAR;
    }

    drawElements();
}


void ChartForm::optionsSetFont()
{
    bool ok;
    TQFont font = TQFontDialog::getFont( &ok, m_font, this );
    if ( ok ) {
	m_font = font;
	drawElements();
    }
}


void ChartForm::optionsSetOptions()
{
    OptionsForm *optionsForm = new OptionsForm( this );
    optionsForm->chartTypeComboBox->setCurrentItem( m_chartType );
    optionsForm->setFont( m_font );
    switch ( m_addValues ) {
	case NO:
	    optionsForm->noRadioButton->setChecked( true );
	    break;
	case YES:
	    optionsForm->yesRadioButton->setChecked( true );
	    break;
	case AS_PERCENTAGE:
	    optionsForm->asPercentageRadioButton->setChecked( true );
	    break;
    }
    optionsForm->decimalPlacesSpinBox->setValue( m_decimalPlaces );
    if ( optionsForm->exec() ) {
	setChartType( ChartType(
		optionsForm->chartTypeComboBox->currentItem()) );
	m_font = optionsForm->font();
	if ( optionsForm->noRadioButton->isChecked() )
	    m_addValues = NO;
	else if ( optionsForm->yesRadioButton->isChecked() )
	    m_addValues = YES;
	else if ( optionsForm->asPercentageRadioButton->isChecked() )
	    m_addValues = AS_PERCENTAGE;
	m_decimalPlaces = optionsForm->decimalPlacesSpinBox->value();
	drawElements();
    }
    delete optionsForm;
}


void ChartForm::helpHelp()
{
    statusBar()->message( "Help is not implemented yet", 2000 );
}


void ChartForm::helpAbout()
{
    TQMessageBox::about( this, "Chart -- About",
			"<center><h1><font color=blue>Chart<font></h1></center>"
			"<p>Chart your data with <i>chart</i>.</p>"
			);
}


void ChartForm::helpAboutTQt()
{
    TQMessageBox::aboutTQt( this, "Chart -- About TQt" );
}

