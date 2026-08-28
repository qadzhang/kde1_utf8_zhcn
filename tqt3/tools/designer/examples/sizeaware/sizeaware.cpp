#include "sizeaware.h"
#include <ntqsettings.h>


SizeAware::SizeAware( TQDialog *parent, const char *name, bool modal )
    : TQDialog( parent, name, modal )
{
    if ( company().isEmpty() )
	setCompany( "UnknownCompany" );
    if ( settingsFile().isEmpty() ) 
	setSettingsFile( "UnknownFile" );

    TQSettings settings;
    settings.insertSearchPath( TQSettings::Windows, "/" + company() );
    settings.insertSearchPath( TQSettings::Unix, "/Opt/" + company() + "/share" );
    int width  = settings.readNumEntry( "/" + settingsFile() + "/width",  640 );
    int height = settings.readNumEntry( "/" + settingsFile() + "/height", 480 );
    resize( width, height );
}


SizeAware::~SizeAware()
{
    // NOOP
}

void SizeAware::destroy()
{
    TQSettings settings;
    settings.insertSearchPath( TQSettings::Windows, "/" + company() );
    settings.insertSearchPath( TQSettings::Unix, "/Opt/" + company() + "/share" );
    settings.writeEntry( "/" + settingsFile() + "/width",   width() );
    settings.writeEntry( "/" + settingsFile() + "/height",  height() );
    close( true );
}



