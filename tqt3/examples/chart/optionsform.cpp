#include "optionsform.h"

#include <ntqbuttongroup.h>
#include <ntqcombobox.h>
#include <ntqfontdialog.h>
#include <ntqframe.h>
#include <ntqimage.h>
#include <ntqlabel.h>
#include <ntqlayout.h>
#include <ntqpushbutton.h>
#include <ntqradiobutton.h>
#include <ntqspinbox.h>

#include "images/options_horizontalbarchart.xpm"
#include "images/options_piechart.xpm"
#include "images/options_verticalbarchart.xpm"


OptionsForm::OptionsForm( TQWidget* parent, const char* name,
			  bool modal, WFlags f )
    : TQDialog( parent, name, modal, f )
{
    setCaption( "Chart -- Options" );
    resize( 320, 290 );

    optionsFormLayout = new TQVBoxLayout( this, 11, 6 );

    chartTypeLayout = new TQHBoxLayout( 0, 0, 6 );

    chartTypeTextLabel = new TQLabel( "&Chart Type", this );
    chartTypeLayout->addWidget( chartTypeTextLabel );

    chartTypeComboBox = new TQComboBox( false, this );
    chartTypeComboBox->insertItem( TQPixmap( options_piechart ), "Pie Chart" );
    chartTypeComboBox->insertItem( TQPixmap( options_verticalbarchart ),
				   "Vertical Bar Chart" );
    chartTypeComboBox->insertItem( TQPixmap( options_horizontalbarchart ),
				   "Horizontal Bar Chart" );
    chartTypeLayout->addWidget( chartTypeComboBox );
    optionsFormLayout->addLayout( chartTypeLayout );

    fontLayout = new TQHBoxLayout( 0, 0, 6 );

    fontPushButton = new TQPushButton( "&Font...", this );
    fontLayout->addWidget( fontPushButton );
    TQSpacerItem* spacer = new TQSpacerItem( 0, 0,
					   TQSizePolicy::Expanding,
					   TQSizePolicy::Minimum );
    fontLayout->addItem( spacer );

    fontTextLabel = new TQLabel( this ); // Must be set by caller via setFont()
    fontLayout->addWidget( fontTextLabel );
    optionsFormLayout->addLayout( fontLayout );

    addValuesFrame = new TQFrame( this );
    addValuesFrame->setFrameShape( TQFrame::StyledPanel );
    addValuesFrame->setFrameShadow( TQFrame::Sunken );
    addValuesFrameLayout = new TQVBoxLayout( addValuesFrame, 11, 6 );

    addValuesButtonGroup = new TQButtonGroup( "Show Values", addValuesFrame );
    addValuesButtonGroup->setColumnLayout(0, TQt::Vertical );
    addValuesButtonGroup->layout()->setSpacing( 6 );
    addValuesButtonGroup->layout()->setMargin( 11 );
    addValuesButtonGroupLayout = new TQVBoxLayout(
					addValuesButtonGroup->layout() );
    addValuesButtonGroupLayout->setAlignment( TQt::AlignTop );

    noRadioButton = new TQRadioButton( "&No", addValuesButtonGroup );
    noRadioButton->setChecked( true );
    addValuesButtonGroupLayout->addWidget( noRadioButton );

    yesRadioButton = new TQRadioButton( "&Yes", addValuesButtonGroup );
    addValuesButtonGroupLayout->addWidget( yesRadioButton );

    asPercentageRadioButton = new TQRadioButton( "As &Percentage",
						addValuesButtonGroup );
    addValuesButtonGroupLayout->addWidget( asPercentageRadioButton );
    addValuesFrameLayout->addWidget( addValuesButtonGroup );

    decimalPlacesLayout = new TQHBoxLayout( 0, 0, 6 );

    decimalPlacesTextLabel = new TQLabel( "&Decimal Places", addValuesFrame );
    decimalPlacesLayout->addWidget( decimalPlacesTextLabel );

    decimalPlacesSpinBox = new TQSpinBox( addValuesFrame );
    decimalPlacesSpinBox->setMinValue( 0 );
    decimalPlacesSpinBox->setMaxValue( 9 );
    decimalPlacesLayout->addWidget( decimalPlacesSpinBox );

    addValuesFrameLayout->addLayout( decimalPlacesLayout );

    optionsFormLayout->addWidget( addValuesFrame );

    buttonsLayout = new TQHBoxLayout( 0, 0, 6 );
    spacer = new TQSpacerItem( 0, 0,
			      TQSizePolicy::Expanding, TQSizePolicy::Minimum );
    buttonsLayout->addItem( spacer );

    okPushButton = new TQPushButton( "OK", this );
    okPushButton->setDefault( true );
    buttonsLayout->addWidget( okPushButton );

    cancelPushButton = new TQPushButton( "Cancel", this );
    buttonsLayout->addWidget( cancelPushButton );
    optionsFormLayout->addLayout( buttonsLayout );

    connect( fontPushButton, TQ_SIGNAL( clicked() ), this, TQ_SLOT( chooseFont() ) );
    connect( okPushButton, TQ_SIGNAL( clicked() ), this, TQ_SLOT( accept() ) );
    connect( cancelPushButton, TQ_SIGNAL( clicked() ), this, TQ_SLOT( reject() ) );

    chartTypeTextLabel->setBuddy( chartTypeComboBox );
    decimalPlacesTextLabel->setBuddy( decimalPlacesSpinBox );
}


void OptionsForm::chooseFont()
{
    bool ok;
    TQFont font = TQFontDialog::getFont( &ok, m_font, this );
    if ( ok )
	setFont( font );
}


void OptionsForm::setFont( TQFont font )
{
    TQString label = font.family() + " " +
		    TQString::number( font.pointSize() ) + "pt";
    if ( font.bold() )
	label += " Bold";
    if ( font.italic() )
	label += " Italic";
    fontTextLabel->setText( label );
    m_font = font;
}
