/****************************************************************************
** ui.h extension file, included from the uic-generated form implementation.
**
** If you wish to add, delete or rename functions use TQt Designer which will
** update this file, preserving your code. Create an init() function in place
** of a constructor, and a destroy() function in place of a destructor.
*****************************************************************************/


const int CLIP_AS_HEX = 0;
const int CLIP_AS_NAME = 1;
const int CLIP_AS_RGB = 2;
const int COL_NAME = 0;
const int COL_HEX = 1;
const int COL_WEB = 2;
const TQString WINDOWS_REGISTRY = "/TQtExamples";
const TQString APP_KEY = "/ColorTool/";

void MainForm::init()
{
    clipboard = TQApplication::clipboard();
    if ( clipboard->supportsSelection() )
	clipboard->setSelectionMode( true );

    findForm = 0;
    loadSettings();
    m_filename = "";
    m_changed = false;
    m_table_dirty = true;
    m_icons_dirty = true;
    clearData( true );
}

void MainForm::clearData( bool fillWithDefaults )
{
    setCaption( "Color Tool" );

    m_colors.clear();
    m_comments.clear();

    if ( fillWithDefaults ) {
	m_colors["black"] = TQt::black;
	m_colors["blue"] = TQt::blue;
	m_colors["cyan"] = TQt::cyan;
	m_colors["darkblue"] = TQt::darkBlue;
	m_colors["darkcyan"] = TQt::darkCyan;
	m_colors["darkgray"] = TQt::darkGray;
	m_colors["darkgreen"] = TQt::darkGreen;
	m_colors["darkmagenta"] = TQt::darkMagenta;
	m_colors["darkred"] = TQt::darkRed;
	m_colors["darkyellow"] = TQt::darkYellow;
	m_colors["gray"] = TQt::gray;
	m_colors["green"] = TQt::green;
	m_colors["lightgray"] = TQt::lightGray;
	m_colors["magenta"] = TQt::magenta;
	m_colors["red"] = TQt::red;
	m_colors["white"] = TQt::white;
	m_colors["yellow"] = TQt::yellow;
    }

    populate();
}

void MainForm::populate()
{
    if ( m_table_dirty ) {
	for ( int r = 0; r < colorTable->numRows(); ++r ) {
	    for ( int c = 0; c < colorTable->numCols(); ++c ) {
		colorTable->clearCell( r, c );
	    }
	}

	colorTable->setNumRows( m_colors.count() );
	if ( ! m_colors.isEmpty() ) {
	    TQPixmap pixmap( 22, 22 );
	    int row = 0;
	    TQMap<TQString,TQColor>::ConstIterator it;
	    for ( it = m_colors.constBegin(); it != m_colors.constEnd(); ++it ) {
		TQColor color = it.data();
		pixmap.fill( color );
		colorTable->setText( row, COL_NAME, it.key() );
		colorTable->setPixmap( row, COL_NAME, pixmap );
		colorTable->setText( row, COL_HEX, color.name().upper() );
		if ( m_show_web ) {
		    TQCheckTableItem *item = new TQCheckTableItem( colorTable, "" );
		    item->setChecked( isWebColor( color ) );
		    colorTable->setItem( row, COL_WEB, item );
		}
		row++;
	    }
	    colorTable->setCurrentCell( 0, 0 );
	}
	colorTable->adjustColumn( COL_NAME );
	colorTable->adjustColumn( COL_HEX );
	if ( m_show_web ) {
	    colorTable->showColumn( COL_WEB );
	    colorTable->adjustColumn( COL_WEB );
	}
	else
	    colorTable->hideColumn( COL_WEB );
	m_table_dirty = false;
    }

    if ( m_icons_dirty ) {
	colorIconView->clear();

	TQMap<TQString,TQColor>::ConstIterator it;
	for ( it = m_colors.constBegin(); it != m_colors.constEnd(); ++it )
	    (void) new TQIconViewItem( colorIconView, it.key(),
				      colorSwatch( it.data() ) );
	m_icons_dirty = false;
    }
}

TQPixmap MainForm::colorSwatch( const TQColor color )
{
    TQPixmap pixmap( 80, 80 );
    pixmap.fill( white );
    TQPainter painter;
    painter.begin( &pixmap );
    painter.setPen( NoPen );
    painter.setBrush( color );
    painter.drawEllipse( 0, 0, 80, 80 );
    painter.end();
    return pixmap;
}

void MainForm::fileNew()
{
    if ( okToClear() ) {
	m_filename = "";
	m_changed = false;
	m_table_dirty = true;
	m_icons_dirty = true;
	clearData( false );
    }
}

void MainForm::fileOpen()
{
    if ( ! okToClear() )
	return;

    TQString filename = TQFileDialog::getOpenFileName(
			    TQString::null, "Colors (*.txt)", this,
			    "file open", "Color Tool -- File Open" );
    if ( ! filename.isEmpty() )
	load( filename );
    else
	statusBar()->message( "File Open abandoned", 2000 );
}

void MainForm::fileSave()
{
    if ( m_filename.isEmpty() ) {
	fileSaveAs();
	return;
    }

    TQFile file( m_filename );
    if ( file.open( IO_WriteOnly ) ) {
	TQTextStream stream( &file );
	if ( ! m_comments.isEmpty() )
	    stream << m_comments.join( "\n" ) << "\n";
	TQMap<TQString,TQColor>::ConstIterator it;
	for ( it = m_colors.constBegin(); it != m_colors.constEnd(); ++it ) {
	    TQColor color = it.data();
	    stream << TQString( "%1 %2 %3\t\t%4" ).
			arg( color.red(), 3 ).
			arg( color.green(), 3 ).
			arg( color.blue(), 3 ).
			arg( it.key() ) << "\n";
	}
	file.close();
	setCaption( TQString( "Color Tool -- %1" ).arg( m_filename ) );
	statusBar()->message( TQString( "Saved %1 colors to '%2'" ).
				arg( m_colors.count() ).
				arg( m_filename ), 3000 );
	m_changed = false;
    }
    else
	statusBar()->message( TQString( "Failed to save '%1'" ).
				arg( m_filename ), 3000 );

}

void MainForm::fileSaveAs()
{
    TQString filename = TQFileDialog::getSaveFileName(
			    TQString::null, "Colors (*.txt)", this,
			    "file save as", "Color Tool -- File Save As" );
    if ( ! filename.isEmpty() ) {
	int ans = 0;
	if ( TQFile::exists( filename ) )
	    ans = TQMessageBox::warning(
			    this, "Color Tool -- Overwrite File",
			    TQString( "Overwrite\n'%1'?" ).
				arg( filename ),
			    "&Yes", "&No", TQString::null, 1, 1 );
	if ( ans == 0 ) {
	    m_filename = filename;
	    fileSave();
	    return;
	}
    }
    statusBar()->message( "Saving abandoned", 2000 );
}

void MainForm::load( const TQString& filename )
{
    clearData( false );
    m_filename = filename;
    TQRegExp regex( "^\\s*(\\d+)\\s+(\\d+)\\s+(\\d+)\\s+(\\S+.*)$" );
    TQFile file( filename );
    if ( file.open( IO_ReadOnly ) ) {
	statusBar()->message( TQString( "Loading '%1'..." ).
			      arg( filename ) );
	TQTextStream stream( &file );
	TQString line;
	while ( ! stream.eof() ) {
	    line = stream.readLine();
	    if ( regex.search( line ) == -1 )
		m_comments += line;
	    else
		m_colors[regex.cap( 4 )] = TQColor(
					    regex.cap( 1 ).toInt(),
					    regex.cap( 2 ).toInt(),
					    regex.cap( 3 ).toInt() );
	}
	file.close();
	m_filename = filename;
	setCaption( TQString( "Color Tool -- %1" ).arg( m_filename ) );
	statusBar()->message( TQString( "Loaded '%1'" ).
				arg( m_filename ), 3000 );
	TQWidget *visible = colorWidgetStack->visibleWidget();
	m_icons_dirty = ! ( m_table_dirty = ( visible == tablePage ) );
	populate();
	m_icons_dirty = ! ( m_table_dirty = ( visible != tablePage ) );
	m_changed = false;
    }
    else
	statusBar()->message( TQString( "Failed to load '%1'" ).
				arg( m_filename ), 3000 );
}


bool MainForm::okToClear()
{
    if ( m_changed ) {
	TQString msg;
	if ( m_filename.isEmpty() )
	    msg = "Unnamed colors ";
	else
	    msg = TQString( "Colors '%1'\n" ).arg( m_filename );
	msg += TQString( "has been changed." );
	int ans = TQMessageBox::information(
			this,
			"Color Tool -- Unsaved Changes",
			msg, "&Save", "Cancel", "&Abandon",
			0, 1 );
	if ( ans == 0 )
	    fileSave();
	else if ( ans == 1 )
	    return false;
    }

    return true;
}

void MainForm::closeEvent( TQCloseEvent * )
{
    fileExit();
}

void MainForm::fileExit()
{
    if ( okToClear() ) {
	saveSettings();
	TQApplication::exit( 0 );
    }
}

void MainForm::editCut()
{
    TQString name;
    TQWidget *visible = colorWidgetStack->visibleWidget();
    statusBar()->message( TQString( "Deleting '%1'" ).arg( name ) );

    if ( visible == tablePage && colorTable->numRows() ) {
	int row = colorTable->currentRow();
	name = colorTable->text( row, 0 );
	colorTable->removeRow( colorTable->currentRow() );
	if ( row < colorTable->numRows() )
	    colorTable->setCurrentCell( row, 0 );
	else if ( colorTable->numRows() )
	    colorTable->setCurrentCell( colorTable->numRows() - 1, 0 );
	m_icons_dirty = true;
    }
    else if ( visible == iconsPage && colorIconView->currentItem() ) {
	TQIconViewItem *item = colorIconView->currentItem();
	name = item->text();
	if ( colorIconView->count() == 1 )
	    colorIconView->clear();
	else {
	    TQIconViewItem *current = item->nextItem();
	    if ( ! current )
		current = item->prevItem();
	    delete item;
	    if ( current )
		colorIconView->setCurrentItem( current );
	    colorIconView->arrangeItemsInGrid();
	}
	m_table_dirty = true;
    }

    if ( ! name.isNull() ) {
	m_colors.remove( name );
	m_changed = true;
	statusBar()->message( TQString( "Deleted '%1'" ).arg( name ), 5000 );
    }
    else
	statusBar()->message( TQString( "Failed to delete '%1'" ).arg( name ), 5000 );
}

void MainForm::editCopy()
{
    TQString text;
    TQWidget *visible = colorWidgetStack->visibleWidget();

    if ( visible == tablePage && colorTable->numRows() ) {
	int row = colorTable->currentRow();
	text = colorTable->text( row, 0 );
    }
    else if ( visible == iconsPage && colorIconView->currentItem() ) {
	TQIconViewItem *item = colorIconView->currentItem();
	text = item->text();
    }
    if ( ! text.isNull() ) {
	TQColor color = m_colors[text];
	switch ( m_clip_as ) {
	    case CLIP_AS_HEX: text = color.name(); break;
	    case CLIP_AS_NAME: break;
	    case CLIP_AS_RGB:
		    text = TQString( "%1,%2,%3" ).
			arg( color.red() ).
			arg( color.green() ).
			arg( color.blue() );
		    break;
	}
	clipboard->setText( text );
	statusBar()->message( "Copied '" + text + "' to the clipboard" );
    }
}

void MainForm::editFind()
{
    if ( ! findForm ) {
	findForm = new FindForm( this );
	connect( findForm, TQ_SIGNAL( lookfor(const TQString&) ),
		 this, TQ_SLOT( lookfor(const TQString&) ) );
    }
    findForm->show();
}

void MainForm::lookfor( const TQString& text )
{
    if ( text.isEmpty() )
	return;
    TQString ltext = text.lower();
    TQWidget *visible = colorWidgetStack->visibleWidget();
    bool found = false;

    if ( visible == tablePage && colorTable->numRows() ) {
	int row = colorTable->currentRow();
	for ( int i = row + 1; i < colorTable->numRows(); ++i )
	    if ( colorTable->text( i, 0 ).lower().contains( ltext ) ) {
		colorTable->setCurrentCell( i, 0 );
		colorTable->clearSelection();
		colorTable->selectRow( i );
		found = true;
		break;
	}
	if ( ! found )
	    colorTable->setCurrentCell( row, 0 );

    }
    else if ( visible == iconsPage ) {
	TQIconViewItem *start = colorIconView->currentItem();
	for ( TQIconViewItem *item = start->nextItem(); item; item = item->nextItem() )
	    if ( item->text().lower().contains( ltext ) ) {
		colorIconView->setCurrentItem( item );
		colorIconView->ensureItemVisible( item );
		found = true;
		break;
	    }
	if ( ! found && start )
	    colorIconView->setCurrentItem( start );
    }
    if ( ! found ) {
	statusBar()->message( TQString( "Could not find '%1' after here" ).
			      arg( text ) );
	findForm->notfound();
    }
}



void MainForm::helpIndex()
{

}

void MainForm::helpContents()
{

}

void MainForm::helpAbout()
{

}


void MainForm::changedTableColor( int row, int )
{
    changedColor( colorTable->text( row, COL_NAME ) );
}

void MainForm::changedIconColor( TQIconViewItem *item )
{
    changedColor( item->text() );
}

void MainForm::changedColor( const TQString& name )
{
    TQColor color = m_colors[name];
    int r = color.red();
    int g = color.green();
    int b = color.blue();
    statusBar()->message( TQString( "%1 \"%2\" (%3,%4,%5)%6 {%7 %8 %9}" ).
			  arg( name ).
			  arg( color.name().upper() ).
			  arg( r ).arg( g ).arg( b ).
			  arg( isWebColor( color ) ? " web" : "" ).
			  arg( r / 255.0, 1, 'f', 3 ).
			  arg( g / 255.0, 1, 'f', 3 ).
			  arg( b / 255.0, 1, 'f', 3 )
			  );
}


void MainForm::changeView(TQAction* action)
{
    if ( action == viewTableAction )
	colorWidgetStack->raiseWidget( tablePage );
    else
	colorWidgetStack->raiseWidget( iconsPage );
}

bool MainForm::isWebColor( TQColor color )
{
    int r = color.red();
    int g = color.green();
    int b = color.blue();

    return ( ( r ==   0 || r ==  51 || r == 102 ||
	       r == 153 || r == 204 || r == 255 ) &&
	     ( g ==   0 || g ==  51 || g == 102 ||
	       g == 153 || g == 204 || g == 255 ) &&
	     ( b ==   0 || b ==  51 || b == 102 ||
	       b == 153 || b == 204 || b == 255 ) );
}


void MainForm::editAdd()
{
    TQColor color = white;
    if ( ! m_colors.isEmpty() ) {
	TQWidget *visible = colorWidgetStack->visibleWidget();
	if ( visible == tablePage )
	    color = colorTable->text( colorTable->currentRow(),
				      colorTable->currentColumn() );
	else
	    color = colorIconView->currentItem()->text();
    }
    color = TQColorDialog::getColor( color, this );
    if ( color.isValid() ) {
	TQPixmap pixmap( 80, 10 );
	pixmap.fill( color );
	ColorNameForm *colorForm = new ColorNameForm( this, "color", true );
	colorForm->setColors( m_colors );
	colorForm->colorLabel->setPixmap( pixmap );
	if ( colorForm->exec() ) {
	    TQString name = colorForm->colorLineEdit->text();
	    m_colors[name] = color;
	    TQPixmap pixmap( 22, 22 );
	    pixmap.fill( color );
	    int row = colorTable->currentRow();
	    colorTable->insertRows( row, 1 );
	    colorTable->setText( row, COL_NAME, name );
	    colorTable->setPixmap( row, COL_NAME, pixmap );
	    colorTable->setText( row, COL_HEX, color.name().upper() );
	    if ( m_show_web ) {
		TQCheckTableItem *item = new TQCheckTableItem( colorTable, "" );
		item->setChecked( isWebColor( color ) );
		colorTable->setItem( row, COL_WEB, item );
	    }
	    colorTable->setCurrentCell( row, 0 );

	    (void) new TQIconViewItem( colorIconView, name,
				      colorSwatch( color ) );
	    m_changed = true;
	}
    }
}

void MainForm::editOptions()
{
    OptionsForm *options = new OptionsForm( this, "options", true );
    switch ( m_clip_as ) {
    case CLIP_AS_HEX:
	options->hexRadioButton->setChecked( true );
	break;
    case CLIP_AS_NAME:
	options->nameRadioButton->setChecked( true );
	break;
    case CLIP_AS_RGB:
	options->rgbRadioButton->setChecked( true );
	break;
    }
    options->webCheckBox->setChecked( m_show_web );

    if ( options->exec() ) {
	if ( options->hexRadioButton->isChecked() )
	    m_clip_as = CLIP_AS_HEX;
	else if ( options->nameRadioButton->isChecked() )
	    m_clip_as = CLIP_AS_NAME;
	else if ( options->rgbRadioButton->isChecked() )
	    m_clip_as = CLIP_AS_RGB;
	m_table_dirty = m_show_web !=
			options->webCheckBox->isChecked();
	m_show_web = options->webCheckBox->isChecked();

	populate();
    }
}

void MainForm::loadSettings()
{
    TQSettings settings;
    settings.insertSearchPath( TQSettings::Windows, WINDOWS_REGISTRY );
    int windowWidth = settings.readNumEntry( APP_KEY + "WindowWidth", 550 );
    int windowHeight = settings.readNumEntry( APP_KEY + "WindowHeight", 500 );
    int windowX = settings.readNumEntry( APP_KEY + "WindowX", 0 );
    int windowY = settings.readNumEntry( APP_KEY + "WindowY", 0 );
    m_clip_as = settings.readNumEntry( APP_KEY + "ClipAs", CLIP_AS_HEX );
    m_show_web = settings.readBoolEntry( APP_KEY + "ShowWeb", true );
    if ( ! settings.readBoolEntry( APP_KEY + "View", true ) ) {
	colorWidgetStack->raiseWidget( iconsPage );
	viewIconsAction->setOn( true );
    }

    resize( windowWidth, windowHeight );
    move( windowX, windowY );
}

void MainForm::saveSettings()
{
    TQSettings settings;
    settings.insertSearchPath( TQSettings::Windows, WINDOWS_REGISTRY );
    settings.writeEntry( APP_KEY + "WindowWidth", width() );
    settings.writeEntry( APP_KEY + "WindowHeight", height() );
    settings.writeEntry( APP_KEY + "WindowX", x() );
    settings.writeEntry( APP_KEY + "WindowY", y() );
    settings.writeEntry( APP_KEY + "ClipAs", m_clip_as );
    settings.writeEntry( APP_KEY + "ShowWeb", m_show_web );
    settings.writeEntry( APP_KEY + "View",
	    colorWidgetStack->visibleWidget() == tablePage );
}


void MainForm::aboutToShow()
{
    populate();
}
