#include <ntqradiobutton.h>
#include <ntqspinbox.h>
#include "receiver.h"

void Receiver::setParent( TQDialog *parent )
{
    p = parent;
    setAmount();
}

void Receiver::setAmount() 
{
    TQSpinBox *amount = 
	(TQSpinBox *) p->child( "amountSpinBox", "TQSpinBox" );

    TQRadioButton *radio = 
	(TQRadioButton *) p->child( "stdRadioButton", "TQRadioButton" );
    if ( radio && radio->isChecked() ) {
	if ( amount )
	    amount->setValue( amount->maxValue() / 2 );
	return;
    }

    radio = 
	(TQRadioButton *) p->child( "noneRadioButton", "TQRadioButton" );
    if ( radio && radio->isChecked() )
	if ( amount )
	    amount->setValue( amount->minValue() );
}
