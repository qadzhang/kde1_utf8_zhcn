/****************************************************************************
** Form implementation generated from reading ui file 'creditformbase.ui'
**
** Created: Fri Aug 10 09:48:16 2001
**      by:  The User Interface Compiler (uic)
**
** WARNING! All changes made in this file will be lost!
****************************************************************************/
#include "creditformbase.h"

#include <ntqvariant.h>   // first for gcc 2.7.2
#include <ntqbuttongroup.h>
#include <ntqpushbutton.h>
#include <ntqradiobutton.h>
#include <ntqspinbox.h>
#include <ntqmime.h>
#include <ntqdragobject.h>
#include <ntqlayout.h>
#include <ntqtooltip.h>
#include <ntqwhatsthis.h>
#include "creditformbase.ui.h"
#include <ntqimage.h>
#include <ntqpixmap.h>

static TQPixmap uic_load_pixmap_CreditFormBase( const TQString &name )
{
    const TQMimeSource *m = TQMimeSourceFactory::defaultFactory()->data( name );
    if ( !m )
	return TQPixmap();
    TQPixmap pix;
    TQImageDrag::decode( m, pix );
    return pix;
}
/* 
 *  Constructs a CreditFormBase which is a child of 'parent', with the 
 *  name 'name' and widget flags set to 'f'.
 *
 *  The dialog will by default be modeless, unless you set 'modal' to
 *  true to construct a modal dialog.
 */
CreditFormBase::CreditFormBase( TQWidget* parent,  const char* name, bool modal, WFlags fl )
    : TQDialog( parent, name, modal, fl )
{
    if ( !name )
	setName( "CreditFormBase" );
    resize( 276, 224 ); 
    setCaption( trUtf8( "Credit Rating", "" ) );
    CreditFormBaseLayout = new TQVBoxLayout( this ); 
    CreditFormBaseLayout->setSpacing( 6 );
    CreditFormBaseLayout->setMargin( 11 );

    creditButtonGroup = new TQButtonGroup( this, "creditButtonGroup" );
    creditButtonGroup->setTitle( trUtf8( "Credit Rating", "" ) );
    creditButtonGroup->setColumnLayout(0, TQt::Vertical );
    creditButtonGroup->layout()->setSpacing( 0 );
    creditButtonGroup->layout()->setMargin( 0 );
    creditButtonGroupLayout = new TQVBoxLayout( creditButtonGroup->layout() );
    creditButtonGroupLayout->setAlignment( TQt::AlignTop );
    creditButtonGroupLayout->setSpacing( 6 );
    creditButtonGroupLayout->setMargin( 11 );

    stdRadioButton = new TQRadioButton( creditButtonGroup, "stdRadioButton" );
    stdRadioButton->setText( trUtf8( "&Standard", "" ) );
    stdRadioButton->setChecked( true );
    creditButtonGroupLayout->addWidget( stdRadioButton );

    noneRadioButton = new TQRadioButton( creditButtonGroup, "noneRadioButton" );
    noneRadioButton->setText( trUtf8( "&None", "" ) );
    creditButtonGroupLayout->addWidget( noneRadioButton );

    specialRadioButton = new TQRadioButton( creditButtonGroup, "specialRadioButton" );
    specialRadioButton->setText( trUtf8( "Sp&ecial", "" ) );
    creditButtonGroupLayout->addWidget( specialRadioButton );
    CreditFormBaseLayout->addWidget( creditButtonGroup );

    amountSpinBox = new TQSpinBox( this, "amountSpinBox" );
    amountSpinBox->setEnabled( false );
    amountSpinBox->setPrefix( trUtf8( "$ ", "" ) );
    amountSpinBox->setButtonSymbols( TQSpinBox::UpDownArrows );
    amountSpinBox->setMaxValue( 100000 );
    amountSpinBox->setLineStep( 10000 );
    CreditFormBaseLayout->addWidget( amountSpinBox );

    Layout4 = new TQHBoxLayout; 
    Layout4->setSpacing( 6 );
    Layout4->setMargin( 0 );
    TQSpacerItem* spacer = new TQSpacerItem( 20, 20, TQSizePolicy::Expanding, TQSizePolicy::Minimum );
    Layout4->addItem( spacer );

    okPushButton = new TQPushButton( this, "okPushButton" );
    okPushButton->setText( trUtf8( "OK", "" ) );
    okPushButton->setDefault( true );
    Layout4->addWidget( okPushButton );
    TQSpacerItem* spacer_2 = new TQSpacerItem( 20, 20, TQSizePolicy::Expanding, TQSizePolicy::Minimum );
    Layout4->addItem( spacer_2 );

    cancelPushButton = new TQPushButton( this, "cancelPushButton" );
    cancelPushButton->setText( trUtf8( "Cancel", "" ) );
    Layout4->addWidget( cancelPushButton );
    TQSpacerItem* spacer_3 = new TQSpacerItem( 20, 20, TQSizePolicy::Expanding, TQSizePolicy::Minimum );
    Layout4->addItem( spacer_3 );
    CreditFormBaseLayout->addLayout( Layout4 );





    // signals and slots connections
    connect( okPushButton, TQ_SIGNAL( clicked() ), this, TQ_SLOT( accept() ) );
    connect( cancelPushButton, TQ_SIGNAL( clicked() ), this, TQ_SLOT( reject() ) );
    connect( creditButtonGroup, TQ_SIGNAL( clicked(int) ), this, TQ_SLOT( setAmount() ) );
    connect( specialRadioButton, TQ_SIGNAL( toggled(bool) ), amountSpinBox, TQ_SLOT( setEnabled(bool) ) );
}

/*  
 *  Destroys the object and frees any allocated resources
 */
CreditFormBase::~CreditFormBase()
{
    // no need to delete child widgets, TQt does it all for us
}

