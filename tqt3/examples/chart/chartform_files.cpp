#include "canvasview.h"
#include "chartform.h"

#include <ntqfile.h>
#include <ntqfiledialog.h>
#include <ntqpainter.h>
#include <ntqprinter.h>
#include <ntqstatusbar.h>


void ChartForm::load( const TQString& filename )
{
    TQFile file( filename );
    if ( !file.open( IO_ReadOnly ) ) {
	statusBar()->message( TQString( "Failed to load \'%1\'" ).
				arg( filename ), 2000 );
	return;
    }

    init(); // Make sure we have colours
    m_filename = filename;
    TQTextStream ts( &file );
    Element element;
    int errors = 0;
    int i = 0;
    while ( !ts.eof() ) {
	ts >> element;
	if ( element.isValid() )
	    m_elements[i++] = element;
	else
	    errors++;
	if ( i == MAX_ELEMENTS ) {
	    statusBar()->message(
		TQString( "Read maximum number of elements (%1)"
		         " discarding others" ).arg( i ), 2000 );
	    break;
	}
    }

    file.close();

    TQString bad = "";
    if ( errors ) {
	bad = TQString( "; skipped " ) + TQString::number( errors ) + " bad record";
	if ( errors > 1 )
	    bad += "s";
    }
    statusBar()->message( TQString( "Read %1 values from \'%2\'%3" ).
			  arg( i ).arg( filename ).arg( bad ), 3000 );

    setCaption( TQString( "Chart -- %1" ).arg( filename ) );
    updateRecentFiles( filename );

    drawElements();
    m_changed = false;
}


void ChartForm::fileSave()
{
    if ( m_filename.isEmpty() ) {
	fileSaveAs();
	return;
    }

    TQFile file( m_filename );
    if ( !file.open( IO_WriteOnly ) ) {
	statusBar()->message( TQString( "Failed to save \'%1\'" ).
				arg( m_filename ), 2000 );
	return;
    }
    TQTextStream ts( &file );
    for ( int i = 0; i < MAX_ELEMENTS; ++i )
	if ( m_elements[i].isValid() )
	    ts << m_elements[i];

    file.close();

    setCaption( TQString( "Chart -- %1" ).arg( m_filename ) );
    statusBar()->message( TQString( "Saved \'%1\'" ).arg( m_filename ), 2000 );
    m_changed = false;
}


void ChartForm::fileSaveAsPixmap()
{
    TQString filename = TQFileDialog::getSaveFileName(
			    TQString::null, "Images (*.png *.xpm *.jpg)",
			    this, "file save as bitmap",
			    "Chart -- File Save As Bitmap" );
    if ( TQPixmap::grabWidget( m_canvasView ).
	    save( filename,
		  filename.mid( filename.findRev( '.' ) + 1 ).upper() ) )
	statusBar()->message( TQString( "Wrote \'%1\'" ).arg( filename ), 2000 );
    else
	statusBar()->message( TQString( "Failed to write \'%1\'" ).
				arg( filename ), 2000 );
}

void ChartForm::filePrint()
{
    if ( !m_printer )
	m_printer = new TQPrinter;
    if ( m_printer->setup() ) {
	TQPainter painter( m_printer );
	m_canvas->drawArea( TQRect( 0, 0, m_canvas->width(), m_canvas->height() ),
			    &painter, false );
	if ( !m_printer->outputFileName().isEmpty() )
	    statusBar()->message( TQString( "Printed \'%1\'" ).
				  arg( m_printer->outputFileName() ), 2000 );
    }
}

