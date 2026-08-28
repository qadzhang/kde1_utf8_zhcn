#include "filechooser.h"
#include <ntqlineedit.h>
#include <ntqpushbutton.h>
#include <ntqfiledialog.h>
#include <ntqlayout.h>

FileChooser::FileChooser( TQWidget *parent, const char *name )
    : TQWidget( parent, name ), md( File )
{
    TQHBoxLayout *layout = new TQHBoxLayout( this );
    layout->setMargin( 0 );

    lineEdit = new TQLineEdit( this, "filechooser_lineedit" );
    layout->addWidget( lineEdit );

    connect( lineEdit, TQ_SIGNAL( textChanged( const TQString & ) ),
	     this, TQ_SIGNAL( fileNameChanged( const TQString & ) ) );

    button = new TQPushButton( "...", this, "filechooser_button" );
    button->setFixedWidth( button->fontMetrics().width( " ... " ) );
    layout->addWidget( button );

    connect( button, TQ_SIGNAL( clicked() ),
	     this, TQ_SLOT( chooseFile() ) );

    setFocusProxy( lineEdit );
}

void FileChooser::setMode( Mode m )
{
    md = m;
}

FileChooser::Mode FileChooser::mode() const
{
    return md;
}

void FileChooser::setFileName( const TQString &fn )
{
    lineEdit->setText( fn );
}

TQString FileChooser::fileName() const
{
    return lineEdit->text();
}

void FileChooser::chooseFile()
{
    TQString fn;
    if ( mode() == File )
	fn = TQFileDialog::getOpenFileName( lineEdit->text(), TQString::null, this );
    else
	fn = TQFileDialog::getExistingDirectory( lineEdit->text(),this );

    if ( !fn.isEmpty() ) {
	lineEdit->setText( fn );
	emit fileNameChanged( fn );
    }
}

