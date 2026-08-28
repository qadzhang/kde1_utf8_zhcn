/****************************************************************************
** ui.h extension file, included from the uic-generated form implementation.
**
** If you wish to add, delete or rename functions respectively slots use
** TQt Designer which will update this file, preserving your code. Create an
** init() function in place of a constructor, and a destroy() function in
** place of a destructor.
*****************************************************************************/

void PrintPreview::init()
{

}

void PrintPreview::destroy()
{

}

void PrintPreview::updatePreview()
{
    TQImage img = pix.convertToImage();
    if ( checkInvert->isChecked() ) {
	img.invertPixels();
    }
    if ( checkMirror->isChecked() ) {
	img = img.mirror( true, false );
    }
    if ( checkFlip->isChecked() ) {
	img = img.mirror( false, true );
    }
    if ( checkLeft->isEnabled() && checkLeft->isChecked() ) {
    }
    if ( checkRight->isEnabled() && checkRight->isChecked() ) {
    }
    TQPixmap pm;
    pm.convertFromImage( img );
    pixmapLabel->setPixmap( pm );
}

void PrintPreview::flip( bool )
{
    updatePreview();
}

void PrintPreview::invertColors( bool )
{
    updatePreview();
}

void PrintPreview::mirror( bool )
{
    updatePreview();   
}

void PrintPreview::rotateLeft( bool )
{
    updatePreview();
}

void PrintPreview::rotateRight( bool )
{
    updatePreview();
}

void PrintPreview::setPixmap( const TQPixmap & pm )
{
    pix = pm;
    updatePreview();
}
