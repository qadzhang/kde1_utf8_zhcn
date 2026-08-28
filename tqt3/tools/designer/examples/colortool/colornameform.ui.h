/****************************************************************************
** ui.h extension file, included from the uic-generated form implementation.
**
** If you wish to add, delete or rename functions use TQt Designer which will
** update this file, preserving your code. Create an init() function in place
** of a constructor, and a destroy() function in place of a destructor.
*****************************************************************************/
#include <ntqcolor.h>
#include <ntqmap.h>
#include <ntqstring.h>

TQMap<TQString,TQColor> m_colors;

void ColorNameForm::setColors( const TQMap<TQString,TQColor>& colors )
{
    m_colors = colors;
}

void ColorNameForm::validate()
{
    TQString name = colorLineEdit->text();
    if ( ! name.isEmpty() &&
	 ( m_colors.isEmpty() || ! m_colors.contains( name ) ) )
	accept();
    else
	colorLineEdit->selectAll();
}
