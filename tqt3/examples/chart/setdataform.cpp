#include "setdataform.h"
#include "chartform.h"

#include <ntqcolordialog.h>
#include <ntqcombobox.h>
#include <ntqlayout.h>
#include <ntqpixmap.h>
#include <ntqpushbutton.h>
#include <ntqtable.h>

#include "images/pattern01.xpm"
#include "images/pattern02.xpm"
#include "images/pattern03.xpm"
#include "images/pattern04.xpm"
#include "images/pattern05.xpm"
#include "images/pattern06.xpm"
#include "images/pattern07.xpm"
#include "images/pattern08.xpm"
#include "images/pattern09.xpm"
#include "images/pattern10.xpm"
#include "images/pattern11.xpm"
#include "images/pattern12.xpm"
#include "images/pattern13.xpm"
#include "images/pattern14.xpm"

const int MAX_PATTERNS = 14;


SetDataForm::SetDataForm( ElementVector *elements, int decimalPlaces,
			  TQWidget* parent,  const char* name,
			  bool modal, WFlags f )
    : TQDialog( parent, name, modal, f )

{
    m_elements = elements;
    m_decimalPlaces = decimalPlaces;

    setCaption( "Chart -- Set Data" );
    resize( 540, 440 );

    tableButtonBox = new TQVBoxLayout( this, 11, 6, "table button box layout" );

    table = new TQTable( this, "data table" );
    table->setNumCols( 5 );
    table->setNumRows( ChartForm::MAX_ELEMENTS );
    table->setColumnReadOnly( 1, true );
    table->setColumnReadOnly( 2, true );
    table->setColumnReadOnly( 4, true );
    table->setColumnWidth( 0, 80 );
    table->setColumnWidth( 1, 60 ); // Columns 1 and 4 must be equal
    table->setColumnWidth( 2, 60 );
    table->setColumnWidth( 3, 200 );
    table->setColumnWidth( 4, 60 );
    TQHeader *th = table->horizontalHeader();
    th->setLabel( 0, "Value" );
    th->setLabel( 1, "Color" );
    th->setLabel( 2, "Pattern" );
    th->setLabel( 3, "Label" );
    th->setLabel( 4, "Color" );
    tableButtonBox->addWidget( table );

    buttonBox = new TQHBoxLayout( 0, 0, 6, "button box layout" );

    colorPushButton = new TQPushButton( this, "color button" );
    colorPushButton->setText( "&Color..." );
    colorPushButton->setEnabled( false );
    buttonBox->addWidget( colorPushButton );

    TQSpacerItem *spacer = new TQSpacerItem( 0, 0, TQSizePolicy::Expanding,
						 TQSizePolicy::Minimum );
    buttonBox->addItem( spacer );

    okPushButton = new TQPushButton( this, "ok button" );
    okPushButton->setText( "OK" );
    okPushButton->setDefault( true );
    buttonBox->addWidget( okPushButton );

    cancelPushButton = new TQPushButton( this, "cancel button" );
    cancelPushButton->setText( "Cancel" );
    cancelPushButton->setAccel( Key_Escape );
    buttonBox->addWidget( cancelPushButton );

    tableButtonBox->addLayout( buttonBox );

    connect( table, TQ_SIGNAL( clicked(int,int,int,const TQPoint&) ),
	     this, TQ_SLOT( setColor(int,int) ) );
    connect( table, TQ_SIGNAL( currentChanged(int,int) ),
	     this, TQ_SLOT( currentChanged(int,int) ) );
    connect( table, TQ_SIGNAL( valueChanged(int,int) ),
	     this, TQ_SLOT( valueChanged(int,int) ) );
    connect( colorPushButton, TQ_SIGNAL( clicked() ), this, TQ_SLOT( setColor() ) );
    connect( okPushButton, TQ_SIGNAL( clicked() ), this, TQ_SLOT( accept() ) );
    connect( cancelPushButton, TQ_SIGNAL( clicked() ), this, TQ_SLOT( reject() ) );

    TQPixmap patterns[MAX_PATTERNS];
    patterns[0]  = TQPixmap( pattern01 );
    patterns[1]  = TQPixmap( pattern02 );
    patterns[2]  = TQPixmap( pattern03 );
    patterns[3]  = TQPixmap( pattern04 );
    patterns[4]  = TQPixmap( pattern05 );
    patterns[5]  = TQPixmap( pattern06 );
    patterns[6]  = TQPixmap( pattern07 );
    patterns[7]  = TQPixmap( pattern08 );
    patterns[8]  = TQPixmap( pattern09 );
    patterns[9]  = TQPixmap( pattern10 );
    patterns[10] = TQPixmap( pattern11 );
    patterns[11] = TQPixmap( pattern12 );
    patterns[12] = TQPixmap( pattern13 );
    patterns[13] = TQPixmap( pattern14 );

    TQRect rect = table->cellRect( 0, 1 );
    TQPixmap pix( rect.width(), rect.height() );

    for ( int i = 0; i < ChartForm::MAX_ELEMENTS; ++i ) {
	Element element = (*m_elements)[i];

	if ( element.isValid() )
	    table->setText(
		i, 0,
		TQString( "%1" ).arg( element.value(), 0, 'f',
				     m_decimalPlaces ) );

	TQColor color = element.valueColor();
	pix.fill( color );
	table->setPixmap( i, 1, pix );
	table->setText( i, 1, color.name() );

	TQComboBox *combobox = new TQComboBox;
	for ( int j = 0; j < MAX_PATTERNS; ++j )
	    combobox->insertItem( patterns[j] );
	combobox->setCurrentItem( element.valuePattern() - 1 );
	table->setCellWidget( i, 2, combobox );

	table->setText( i, 3, element.label() );

	color = element.labelColor();
	pix.fill( color );
	table->setPixmap( i, 4, pix );
	table->setText( i, 4, color.name() );
    }

}


void SetDataForm::currentChanged( int, int col )
{
    colorPushButton->setEnabled( col == 1 || col == 4 );
}


void SetDataForm::valueChanged( int row, int col )
{
    if ( col == 0 ) {
	bool ok;
	double d = table->text( row, col ).toDouble( &ok );
	if ( ok && d > EPSILON )
	    table->setText(
		row, col, TQString( "%1" ).arg(
			    d, 0, 'f', m_decimalPlaces ) );
	else if ( !table->text( row, col ).isEmpty() )
	    table->setText( row, col, table->text( row, col ) + "?" );
    }
}


void SetDataForm::setColor()
{
    setColor( table->currentRow(), table->currentColumn() );
    table->setFocus();
}


void SetDataForm::setColor( int row, int col )
{
    if ( !( col == 1 || col == 4 ) )
	return;

    TQColor color = TQColorDialog::getColor(
			TQColor( table->text( row, col ) ),
			this, "color dialog" );
    if ( color.isValid() ) {
	TQPixmap pix = table->pixmap( row, col );
	pix.fill( color );
	table->setPixmap( row, col, pix );
	table->setText( row, col, color.name() );
    }
}


void SetDataForm::accept()
{
    bool ok;
    for ( int i = 0; i < ChartForm::MAX_ELEMENTS; ++i ) {
	Element &element = (*m_elements)[i];
	double d = table->text( i, 0 ).toDouble( &ok );
	if ( ok )
	    element.setValue( d );
	else
	    element.setValue( Element::INVALID );
	element.setValueColor( TQColor( table->text( i, 1 ) ) );
	element.setValuePattern(
		((TQComboBox*)table->cellWidget( i, 2 ))->currentItem() + 1 );
	element.setLabel( table->text( i, 3 ) );
	element.setLabelColor( TQColor( table->text( i, 4 ) ) );
    }

    TQDialog::accept();
}
