/****************************************************************************
** ui.h extension file, included from the uic-generated form implementation.
**
** If you wish to add, delete or rename functions or slots use
** TQt Designer which will update this file, preserving your code. Create an
** init() function in place of a constructor, and a destroy() function in
** place of a destructor.
*****************************************************************************/

#include <ntqapplication.h>
#include <ntqcursor.h>
#include <ntqeventloop.h>
#include <ntqfile.h>
#include <ntqfiledialog.h>
#include <ntqfileinfo.h>
#include <ntqlineedit.h>
#include <ntqmessagebox.h>
#include <ntqpushbutton.h>
#include <ntqtimer.h>


void Distributor::init()
{
    timer = new TQTimer( this );
    connect( timer, TQ_SIGNAL(timeout()), TQ_SLOT(checkLibData()) );

    cancelButton()->setAutoDefault( false );
    backButton()->setAutoDefault( false );

    setNextEnabled( selectLibrary, false );

    setHelpEnabled( selectLibrary, false );
    setHelpEnabled( modifyPaths, false );
    setHelpEnabled( verifyMods, false );

    setFinishEnabled( verifyMods, true );
}

void Distributor::showPage( TQWidget *page )
{
    if ( page == selectLibrary ) {
	nextButton()->setDefault( true );
	libFilename->setFocus();
    } else if ( page == modifyPaths ) {
	nextButton()->setDefault( true );
	prefixPath->selectAll();
	prefixPath->setFocus();
    } else if ( page == verifyMods ) {
	finishButton()->setDefault( true );
	finishButton()->setFocus();

	TQString labeltext =
	    tr("<p><b>Current Library File:</b> %1</p>"
	       "<table border=0>"
	       "<tr><td><b>New Installation Prefix:</b></td><td>%2</td></tr>"
	       "<tr><td></td><td></td></tr>"
	       "<tr><td><b>Binaries Path:</b></td><td>%3</td></tr>"
	       "<tr><td><b>Documentation Path:</b></td><td>%4</td></tr>"
	       "<tr><td><b>Headers Path:</b></td><td>%5</td></tr>"
	       "<tr><td><b>Libraries Path:</b></td><td>%6</td></tr>"
	       "<tr><td><b>Plugins Path:</b></td><td>%7</td></tr>"
	       "<tr><td><b>Data Path:</b></td><td>%8</td></tr>"
	       "</table>"
	       "<p>Please verify that these options are correct.  Press the "
	       "<i>Finish</i> button to apply these modifications to the TQt "
	       "library.  Use the <i>Back</i> button to make corrections.  Use "
	       "the <i>Cancel</i> button to abort.</p>")
	    .arg( libFilename->text() )
	    .arg( prefixPath->text() )
	    .arg( binPath->text() )
	    .arg( docPath->text() )
	    .arg( hdrPath->text() )
	    .arg( libPath->text() )
	    .arg( plgPath->text() )
	    .arg( datPath->text() );
	textLabel4->setText( labeltext );
    }

    TQWizard::showPage( page );
}

void Distributor::checkLibFilename( const TQString &filename )
{
    setNextEnabled( selectLibrary, false );

    TQFileInfo fileinfo( filename );
    if ( ! filename.isEmpty() && fileinfo.exists() &&
	 fileinfo.isReadable() && fileinfo.isWritable() &&
	 fileinfo.isFile() && !fileinfo.isSymLink() )
	timer->start( 500, true );
}

void Distributor::browseLibFilename()
{
    TQString filename =
	TQFileDialog::getOpenFileName( TQString::null, TQString::null, this );
    libFilename->setText( filename );
}

static char *find_pattern( char *h, const char *n, ulong hlen )
{
    if ( ! h || ! n || hlen == 0 )
	return 0;

#ifdef Q_OS_UNIX
    size_t nlen;
#else
    ulong nlen;
#endif

    char nc = *n++;
    nlen = strlen( n );
    char hc;

    do {
	do {
	    hc = *h++;
	    if ( hlen-- < 1 )
		return 0;
	} while ( hc != nc );

	if ( nlen > hlen )
	    return 0;
    } while ( tqstrncmp( h, n, nlen ) != 0 );
    return h + nlen;
}

void Distributor::checkLibData()
{
    struct step {
	const char *key;
	TQCString value;
	bool done;
    } steps[7];

    steps[0].key = "qt_nstpath=";
    steps[0].done = false;

    steps[1].key = "qt_binpath=";
    steps[1].done = false;

    steps[2].key = "qt_docpath=";
    steps[2].done = false;

    steps[3].key = "qt_hdrpath=";
    steps[3].done = false;

    steps[4].key = "qt_libpath=";
    steps[4].done = false;

    steps[5].key = "qt_plgpath=";
    steps[5].done = false;

    steps[6].key = "qt_datpath=";
    steps[6].done = false;

    uint completed = 0;
    uint total_steps = sizeof(steps) / sizeof(step);

    TQFile file( libFilename->text() );
    if ( file.open( IO_ReadOnly ) ) {
	TQApplication::setOverrideCursor( WaitCursor );

	// instead of reading in the entire file, do the search in chunks
	char data[60000];
	ulong offset = 0;

	while ( ! file.atEnd() && completed < total_steps ) {
	    TQApplication::eventLoop()->processEvents( TQEventLoop::ExcludeUserInput );

	    ulong len = file.readBlock( data, sizeof(data) );
	    if ( len < 267 ) {
		// not enough room to make any modifications... stop
		break;
	    }

	    for ( uint x = 0; x < total_steps; ++x ) {
		if ( steps[x].done ) continue;

		char *s = find_pattern( data, steps[x].key, len );
		if ( s ) {
		    ulong where = s - data;
		    if ( len - where < 256 ) {
			// not enough space left to write the full
			// path... move the file pointer back to just
			// before the pattern and continue
			offset += where - 11;
			file.at( offset );
			len = file.readBlock( data, sizeof(data) );
			--x; // retry the current step
			continue;
		    }

		    steps[x].value = s;
		    steps[x].done = true;

		    ++completed;
		}
	    }

	    // move to the new read position
	    offset += len - 11;
	    file.at( offset );
	}

	file.close();

	TQApplication::restoreOverrideCursor();
    }

    if ( completed == total_steps ) {
	setNextEnabled( selectLibrary, true );

	TQString prefix = TQFile::decodeName( steps[0].value );
	prefixPath->setText( prefix );

	TQString def_bin = prefix + TQString::fromLatin1( "/bin" );
	TQString def_doc = prefix + TQString::fromLatin1( "/doc" );
	TQString def_hdr = prefix + TQString::fromLatin1( "/include" );
	TQString def_lib = prefix + TQString::fromLatin1( "/lib" );
	TQString def_plg = prefix + TQString::fromLatin1( "/plugins" );
	TQString def_dat = prefix;

	TQString bin = TQFile::decodeName( steps[1].value );
	TQString doc = TQFile::decodeName( steps[2].value );
	TQString hdr = TQFile::decodeName( steps[3].value );
	TQString lib = TQFile::decodeName( steps[4].value );
	TQString plg = TQFile::decodeName( steps[5].value );
	TQString dat = TQFile::decodeName( steps[6].value );

	autoSet->setChecked( def_bin == bin &&
			     def_doc == doc &&
			     def_hdr == hdr &&
			     def_lib == lib &&
			     def_plg == plg &&
			     def_dat == dat );

	if ( ! autoSet->isChecked() ) {
	    binPath->setText( bin );
	    docPath->setText( doc );
	    hdrPath->setText( hdr );
	    libPath->setText( lib );
	    plgPath->setText( plg );
	    datPath->setText( dat );
	}
    }
}

void Distributor::checkInstallationPrefix( const TQString &prefix )
{
    if ( autoSet->isChecked() ) {
	binPath->setText( prefix + TQString::fromLatin1( "/bin" ) );
	docPath->setText( prefix + TQString::fromLatin1( "/doc" ) );
	hdrPath->setText( prefix + TQString::fromLatin1( "/include" ) );
	libPath->setText( prefix + TQString::fromLatin1( "/lib" ) );
	plgPath->setText( prefix + TQString::fromLatin1( "/plugins" ) );
	datPath->setText( prefix );
    }
}

void Distributor::browseInstallationPrefix()
{
    TQString prefix =
	TQFileDialog::getOpenFileName( TQString::null, TQString::null, this );
    prefixPath->setText( prefix );
}


void Distributor::toggleAutoSet( bool autoset )
{
    if ( autoset ) checkInstallationPrefix( prefixPath->text() );
}

void Distributor::accept()
{
    struct step {
	const char *key;
	TQCString value;
	bool done;
    } steps[7];

    steps[0].key = "qt_nstpath=";
    steps[0].value = TQFile::encodeName( prefixPath->text() );
    steps[0].done = false;

    steps[1].key = "qt_binpath=";
    steps[1].value = TQFile::encodeName( binPath->text() );
    steps[1].done = false;

    steps[2].key = "qt_docpath=";
    steps[2].value = TQFile::encodeName( docPath->text() );
    steps[2].done = false;

    steps[3].key = "qt_hdrpath=";
    steps[3].value = TQFile::encodeName( hdrPath->text() );
    steps[3].done = false;

    steps[4].key = "qt_libpath=";
    steps[4].value = TQFile::encodeName( libPath->text() );
    steps[4].done = false;

    steps[5].key = "qt_plgpath=";
    steps[5].value = TQFile::encodeName( plgPath->text() );
    steps[5].done = false;

    steps[6].key = "qt_datpath=";
    steps[6].value = TQFile::encodeName( datPath->text() );
    steps[6].done = false;

    uint completed = 0;
    uint total_steps = sizeof(steps) / sizeof(step);

    TQFile file( libFilename->text() );
    if ( file.open( IO_ReadWrite ) ) {
	TQApplication::setOverrideCursor( WaitCursor );

	// instead of reading in the entire file, do the search in chunks
	char data[60000];
	ulong offset = 0;

	while ( ! file.atEnd() && completed < total_steps ) {
	    TQApplication::eventLoop()->processEvents( TQEventLoop::ExcludeUserInput );

	    ulong len = file.readBlock( data, sizeof(data) );
	    if ( len < 267 ) {
		// not enough room to make any modifications... stop
		break;
	    }

	    uint completed_save = completed;
	    for ( uint x = 0; x < total_steps; ++x ) {
		if ( steps[x].done ) continue;

		char *s = find_pattern( data, steps[x].key, len );
		if ( s ) {
		    ulong where = s - data;
		    if ( len - where < 256 ) {
			// not enough space left to write the full
			// path... move the file pointer back to just
			// before the pattern and continue
			offset += where - 11;
			file.at( offset );
			len = file.readBlock( data, sizeof(data) );
			--x; // retry the current step
			continue;
		    }

		    qstrcpy( s, steps[x].value );
		    steps[x].done = true;

		    ++completed;
		}
	    }

	    if ( completed != completed_save ) {
		// something changed...  move file pointer back to
		// where the data was read and write the new data
		file.at( offset );
		file.writeBlock( data, len );
	    }

	    // move to the new read position
	    offset += len - 11;
	    file.at( offset );
	}

	file.close();

	TQApplication::restoreOverrideCursor();
    }

    if ( completed != total_steps ) {
    	TQMessageBox::information( this,
				  tr("TQt Distribution Wizard"),
				  tr("<p><h3>Modifications failed.</h3></p>"
				     "<p>Please make sure that you have permission "
				     "to write the selected file, and that the library "
				     "is properly built.</p>") );
	return;
    }

    TQWizard::accept();
}
