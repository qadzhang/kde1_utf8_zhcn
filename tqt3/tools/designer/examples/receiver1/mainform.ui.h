void MainForm::init()
{

}

void MainForm::destroy()
{

}

void MainForm::creditDialog()
{
    TQDialog *creditForm = (TQDialog *)
    	TQWidgetFactory::create( "../credit/creditformbase.ui" );
    // Set up the dynamic dialog here
    
    if ( creditForm->exec() ) {
	// The user accepted, act accordingly
	TQSpinBox *amount = (TQSpinBox *) creditForm->child( "amountSpinBox", "TQSpinBox" );
	if ( amount )
	    ratingTextLabel->setText( amount->text() ); 
    }
    delete creditForm;
}

