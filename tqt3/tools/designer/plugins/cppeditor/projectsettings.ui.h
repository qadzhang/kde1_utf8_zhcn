/****************************************************************************
** ui.h extension file, included from the uic-generated form implementation.
** 
** Add custom slot implementations here. Use a slot init() for
** initialization code called during construction, and a slot destroy()
** for cleanup code called during destruction.
**
** This file gets modified by TQt Designer whenever you add, rename or
** remove custom slots. Implementation code does not get lost.
*****************************************************************************/

#include <designerinterface.h>

void CppProjectSettings::reInit( TQUnknownInterface *iface )
{
    comboConfig->setCurrentItem( 0 );
    comboLibs->setCurrentItem( 0 );
    comboDefines->setCurrentItem( 0 );
    comboInclude->setCurrentItem( 0 );
    
    DesignerInterface *dIface = 0;
    iface->queryInterface( IID_Designer, (TQUnknownInterface**)&dIface );
    if ( !dIface )
	return;
    DesignerProject *project = dIface->currentProject();
    if ( project->templte() == "app" )
	comboTemplate->setCurrentItem( 0 );
    else
	comboTemplate->setCurrentItem( 1 );
     
    config.clear();
    defines.clear();
    libs.clear();
    defines.clear();
    includes.clear();
    
    const TQString platforms[] = { "(all)", "win32", "unix", "mac", TQString::null };
    for ( int i = 0; platforms[ i ] != TQString::null; ++i ) {
	config.replace( platforms[ i ], project->config( platforms[ i ] ) );
	libs.replace( platforms[ i ], project->libs( platforms[ i ] ) );
	defines.replace( platforms[ i ], project->defines( platforms[ i ] ) );
	includes.replace( platforms[ i ], project->includePath( platforms[ i ] ) );
    }
    editConfig->setText( config[ "(all)" ] );
    editLibs->setText( libs[ "(all)" ] );
    editDefines->setText( defines[ "(all)" ] );
    editInclude->setText( includes[ "(all)" ] );
}

void CppProjectSettings::save( TQUnknownInterface *iface )
{
    DesignerInterface *dIface = 0;
    iface->queryInterface( IID_Designer, (TQUnknownInterface**)&dIface );
    if ( !dIface )
	return;
    DesignerProject *project = dIface->currentProject();
    
    project->setTemplate( comboTemplate->currentText() );
    const TQString platforms[] = { "(all)", "win32", "unix", "mac", TQString::null };
    for ( int i = 0; platforms[ i ] != TQString::null; ++i ) {
	project->setConfig( platforms[ i ], config[ platforms[ i ] ] );
	project->setLibs( platforms[ i ], libs[ platforms[ i ] ] );
	project->setDefines( platforms[ i ], defines[ platforms[ i ] ] );
	project->setIncludePath( platforms[ i ], includes[ platforms[ i ] ] );
    }
}

void CppProjectSettings::configChanged( const TQString &str )
{
    config.replace( comboConfig->currentText(), str );
}

void CppProjectSettings::libsChanged( const TQString &str )
{
    libs.replace( comboLibs->currentText(), str );
}

void CppProjectSettings::definesChanged( const TQString &str )
{
    defines.replace( comboDefines->currentText(), str );
}

void CppProjectSettings::includesChanged( const TQString &str )
{
    includes.replace( comboInclude->currentText(), str );
}

void CppProjectSettings::configPlatformChanged( const TQString &plat )
{
    editConfig->setText( config[ plat ] );
}

void CppProjectSettings::libsPlatformChanged( const TQString &plat )
{
    editLibs->setText( libs[ plat ] );
}

void CppProjectSettings::definesPlatformChanged( const TQString &plat )
{
    editDefines->setText( defines[ plat ] );
}

void CppProjectSettings::includesPlatformChanged( const TQString &plat )
{
    editInclude->setText( includes[ plat ] );
}
