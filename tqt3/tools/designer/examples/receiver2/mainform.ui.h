void MainForm::creditDialog()
{
    Receiver *receiver = new Receiver; 
    TQDialog *creditForm = (TQDialog *) 
    TQWidgetFactory::create( "../credit/creditformbase.ui", receiver );   
    receiver->setParent( creditForm );   

    // Set up the dynamic dialog here 
     
    if ( creditForm->exec() ) { 
	// The user accepted, act accordingly 
	TQSpinBox *amount = (TQSpinBox *) creditForm->child( "amountSpinBox", "TQSpinBox" ); 
	if ( amount ) 
	    ratingTextLabel->setText( amount->text() );  
    } 
    
    delete receiver; 
    delete creditForm; 
}

