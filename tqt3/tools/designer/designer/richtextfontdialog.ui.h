/****************************************************************************
** ui.h extension file, included from the uic-generated form implementation.
**
** If you wish to add, delete or rename slots use TQt Designer which will
** update this file, preserving your code. Create an init() slot in place of
** a constructor, and a destroy() slot in place of a destructor.
*****************************************************************************/
#include <ntqcolordialog.h>
#include <ntqfontdatabase.h>

void RichTextFontDialog::init()
{
    TQFontDatabase *db = new TQFontDatabase();
    fontCombo->insertStringList( db->families() );
}

void RichTextFontDialog::selectColor()
{
    color = TQColorDialog::getColor( "", this );   
    if( color.isValid() )
	colorButton->setPaletteBackgroundColor( color );
}

void RichTextFontDialog::accept()
{
    size = fontSizeCombo->currentText();
    font = fontCombo->currentText();
    done( Accepted );
}

void RichTextFontDialog::reject()
{
    done( Rejected );
}

TQString RichTextFontDialog::getSize()
{
    return size;
}

TQString RichTextFontDialog::getColor()
{
    return color.name();
}

TQString RichTextFontDialog::getFont()
{
    return font;
}
