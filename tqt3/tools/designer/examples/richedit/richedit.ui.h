void EditorForm::init()
{
    textEdit->setFocus();  
 
    TQFontDatabase fonts;
    fontComboBox->insertStringList( fonts.families() );
    TQString font = textEdit->family();
    font = font.lower();
    for ( int i = 0 ; i < fontComboBox->count(); i++ ) {
	if ( font == fontComboBox->text( i ) ) {
	    fontComboBox->setCurrentItem( i );
	    break;
	}
    }
}

void EditorForm::fileExit()
{
    if ( saveAndContinue( "Exit" ) )
	tqApp->exit();
}

void EditorForm::fileNew()
{
    if ( saveAndContinue( "New" ) )
    	textEdit->clear();
}

void EditorForm::fileOpen()
{
    if ( saveAndContinue( "Open" ) ) {
    	TQString fn( TQFileDialog::getOpenFileName( 
			TQString::null, 
			"Rich Text Files (*.htm*)", this ) );   
	if ( !fn.isEmpty() ) {
	    fileName = fn;
	    TQFile file( fileName );
	    if ( file.open( IO_ReadOnly ) ) {
		TQTextStream ts( &file );
		textEdit->setText( ts.read() );
	    }
	}
    }
}

void EditorForm::fileSave()
{
    if ( fileName.isEmpty() ) {
	fileSaveAs();
    } else {
	TQFile f( fileName );
	if ( f.open( IO_WriteOnly ) ) {
	    TQTextStream ts( &f );
	    ts << textEdit->text();
	    textEdit->setModified( false );
	}
    }
}

void EditorForm::fileSaveAs()
{
    TQString fn = TQFileDialog::getSaveFileName( 
    				"", "Rich Text Files (*.htm*)", this );
    if ( !fn.isEmpty() ) {
	fileName = fn;
	fileSave();
    }
}

void EditorForm::helpAbout()
{
    
}

void EditorForm::helpContents()
{
    
}

void EditorForm::helpIndex()
{
    
}

void EditorForm::changeAlignment(TQAction * align)
{
    if ( align == leftAlignAction ) 
	textEdit->setAlignment( TQt::AlignLeft );
    else if ( align == rightAlignAction )
	textEdit->setAlignment( TQt::AlignRight );
    else if ( align == centerAlignAction )
	textEdit->setAlignment( TQt::AlignCenter );
}

int EditorForm::saveAndContinue(const TQString & action)
{
    int continueAction = 1;
    
    if ( textEdit->isModified() ) {  
	switch( TQMessageBox::information(     
		this, "Rich Edit",   
		"The document contains unsaved changes.\n"   
		"Do you want to save the changes?",   
		"&Save", "&Don't Save", "&Cancel " + action,   
		0, // Enter == button 0   
		2 ) ) { // Escape == button 2   
	case 0: // Save; continue  
     		fileSave();  
        	break;   
    	case 1: // Do not save; continue
        	break;   
    	case 2: // Cancel
		continueAction = 0;
        	break;  
    	}	  
    }  
    
    return continueAction;
}

