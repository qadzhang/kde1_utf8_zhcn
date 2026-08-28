/****************************************************************
**
** Implementation of PrintPanel class, translation tutorial 3
**
****************************************************************/

#include "printpanel.h"

#include <ntqlabel.h>
#include <ntqradiobutton.h>
#include <ntqhbuttongroup.h>

PrintPanel::PrintPanel( TQWidget *parent, const char *name )
    : TQVBox( parent, name )
{
    setMargin( 10 );
    setSpacing( 10 );

/*
    TQLabel *lab = new TQLabel( tr("<b>TROLL PRINT</b>"), this );
    lab->setAlignment( AlignCenter );
*/

    TQRadioButton *but;

    TQHButtonGroup *twoSided = new TQHButtonGroup( this );
    twoSided->setTitle( tr("2-sided") );
    but = new TQRadioButton( tr("Enabled"), twoSided );
    but = new TQRadioButton( tr("Disabled"), twoSided );
    but->toggle();

    TQHButtonGroup *colors = new TQHButtonGroup( this );
    colors->setTitle( tr("Colors") );
    but = new TQRadioButton( tr("Enabled"), colors );
    but = new TQRadioButton( tr("Disabled"), colors );
    but->toggle();
}
