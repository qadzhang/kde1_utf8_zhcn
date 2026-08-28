#include "plugin.h"
#include "../widget/filechooser.h"

static const char *filechooser_pixmap[] = {
    "22 22 8 1",
    "  c Gray100",
    ". c Gray97",
    "X c #4f504f",
    "o c #00007f",
    "O c Gray0",
    "+ c none",
    "@ c Gray0",
    "# c Gray0",
    "++++++++++++++++++++++",
    "++++++++++++++++++++++",
    "++++++++++++++++++++++",
    "++++++++++++++++++++++",
    "+OOOOOOOOOOOOOOOOOOOO+",
    "OOXXXXXXXXXXXXXXXXXXOO",
    "OXX.          OO OO  O",
    "OX.      oo     O    O",
    "OX.      oo     O   .O",
    "OX  ooo  oooo   O    O",
    "OX    oo oo oo  O    O",
    "OX  oooo oo oo  O    O",
    "OX oo oo oo oo  O    O",
    "OX oo oo oo oo  O    O",
    "OX  oooo oooo   O    O",
    "OX            OO OO  O",
    "OO..................OO",
    "+OOOOOOOOOOOOOOOOOOOO+",
    "++++++++++++++++++++++",
    "++++++++++++++++++++++",
    "++++++++++++++++++++++",
    "++++++++++++++++++++++"
};

CustomWidgetPlugin::CustomWidgetPlugin()
{
}

TQStringList CustomWidgetPlugin::keys() const
{
    TQStringList list;
    list << "FileChooser";
    return list;
}

TQWidget* CustomWidgetPlugin::create( const TQString &key, TQWidget* parent, const char* name )
{
    if ( key == "FileChooser" )
	return new FileChooser( parent, name );
    return 0;
}

TQString CustomWidgetPlugin::group( const TQString& feature ) const
{
    if ( feature == "FileChooser" )
	return "Input";
    return TQString::null;
}

TQIconSet CustomWidgetPlugin::iconSet( const TQString& ) const
{
    return TQIconSet( TQPixmap( filechooser_pixmap ) );
}

TQString CustomWidgetPlugin::includeFile( const TQString& feature ) const
{
    if ( feature == "FileChooser" )
	return "filechooser.h";
    return TQString::null;
}

TQString CustomWidgetPlugin::toolTip( const TQString& feature ) const
{
    if ( feature == "FileChooser" )
	return "File Chooser Widget";
    return TQString::null;
}

TQString CustomWidgetPlugin::whatsThis( const TQString& feature ) const
{
    if ( feature == "FileChooser" )
	return "A widget to choose a file or directory";
    return TQString::null;
}

bool CustomWidgetPlugin::isContainer( const TQString& ) const
{
    return false;
}


TQ_EXPORT_PLUGIN( CustomWidgetPlugin )
