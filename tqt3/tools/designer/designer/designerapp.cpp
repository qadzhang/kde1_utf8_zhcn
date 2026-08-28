 /**********************************************************************
** Copyright (C) 2005-2008 Trolltech ASA.  All rights reserved.
**
** This file is part of TQt Designer.
**
** This file may be used under the terms of the GNU General
** Public License versions 2.0 or 3.0 as published by the Free
** Software Foundation and appearing in the files LICENSE.GPL2
** and LICENSE.GPL3 included in the packaging of this file.
** Alternatively you may (at your option) use any later version
** of the GNU General Public License if such license has been
** publicly approved by Trolltech ASA (or its successors, if any)
** and the KDE Free TQt Foundation.
**
** Please review the following information to ensure GNU General
** Public Licensing requirements will be met:
** http://trolltech.com/products/qt/licenses/licensing/opensource/.
** If you are unsure which license is appropriate for your use, please
** review the following information:
** http://trolltech.com/products/qt/licenses/licensing/licensingoverview
** or contact the sales department at sales@trolltech.com.
**
** Licensees holding valid TQt Commercial licenses may use this file in
** accordance with the TQt Commercial License Agreement provided with
** the Software.
**
** This file is provided "AS IS" with NO WARRANTY OF ANY KIND,
** INCLUDING THE WARRANTIES OF DESIGN, MERCHANTABILITY AND FITNESS FOR
** A PARTICULAR PURPOSE. Trolltech reserves all rights not granted
** herein.
**
**********************************************************************/

#include "designerappiface.h"
#include "designerapp.h"
#include "mainwindow.h"
#include "formwindow.h"

#include <ntqfile.h>
#include <ntqdir.h>
#include <ntqsettings.h>
#include <ntqsplashscreen.h>

#ifdef TQ_WS_WIN
#include <qt_windows.h>
#include <process.h>
#endif

static TQSplashScreen *splash = 0;

void set_splash_status( const TQString &txt )
{
    if ( !splash )
	return;
    TQString splashText = "Licensed to "
			 + TQString::fromLatin1( TQT_PRODUCT_LICENSEE ) + "\n"
			 + txt;
    splash->message( splashText, TQt::AlignRight|TQt::AlignTop );
}

TQ_EXPORT DesignerApplication::DesignerApplication( int &argc, char **argv )
    : TQApplication( argc, argv )
{
#if defined(TQ_WS_WIN)
    if ( winVersion() & TQt::WV_NT_based )
	DESIGNER_OPENFILE = RegisterWindowMessage((TCHAR*)"QT_DESIGNER_OPEN_FILE");
    else
	DESIGNER_OPENFILE = RegisterWindowMessageA("QT_DESIGNER_OPEN_FILE");
#endif
}

TQ_EXPORT TQSplashScreen *DesignerApplication::showSplash()
{
    TQRect screen = TQApplication::desktop()->screenGeometry();
    TQSettings config;
    config.insertSearchPath( TQSettings::Windows, "/Trolltech" );

    TQRect mainRect;
    TQString keybase = settingsKey();
    bool show = config.readBoolEntry( keybase + "SplashScreen", true );
    mainRect.setX( config.readNumEntry( keybase + "Geometries/MainwindowX", 0 ) );
    mainRect.setY( config.readNumEntry( keybase + "Geometries/MainwindowY", 0 ) );
    mainRect.setWidth( config.readNumEntry( keybase + "Geometries/MainwindowWidth", 500 ) );
    mainRect.setHeight( config.readNumEntry( keybase + "Geometries/MainwindowHeight", 500 ) );
    screen = TQApplication::desktop()->screenGeometry( TQApplication::desktop()->screenNumber( mainRect.center() ) );

    if ( show ) {
	splash = new TQSplashScreen( TQPixmap::fromMimeSource("designer_splash.png") );
	splash->show();
	set_splash_status( "Initializing..." );
    }

    return splash;
}

void DesignerApplication::closeSplash()
{
    splash->hide();
}

static TQString *settings_key = 0;
static TQString *old_settings_key = 0;

TQ_EXPORT TQString DesignerApplication::settingsKey()
{
    if ( !settings_key )
	settings_key = new TQString( "/TQt Designer/" +
				    TQString::number( (TQT_VERSION >> 16) & 0xff ) +
	                            "." + TQString::number( (TQT_VERSION >> 8) & 0xff ) + "/" );
    return *settings_key;
}

TQString DesignerApplication::oldSettingsKey()
{
    if ( !old_settings_key ) {
	int majorVer = (TQT_VERSION >> 16) & 0xff;
	int minorVer = (TQT_VERSION >> 8) & 0xff;
	
	// If minorVer is 0 (e.g. 4.0) then we don't want to read the
	// old settings, too much might have changed.
	if ( !minorVer == 0 )
	    minorVer--;

	old_settings_key = new TQString( "/TQt Designer/" +
				    TQString::number( majorVer ) +
	                            "." + TQString::number( minorVer ) + "/" );
    }
    return *old_settings_key;
}

void DesignerApplication::setSettingsKey( const TQString &key )
{
    if ( !settings_key )
	settings_key = new TQString( key );
    else
	*settings_key = key;
}

#if defined(TQ_WS_WIN)
bool DesignerApplication::winEventFilter( MSG *msg )
{
    if ( msg->message == DESIGNER_OPENFILE ) {
	TQFile f( TQDir::homeDirPath() + "/.designerargs" );
	TQFileInfo fi(f);
	if ( fi.lastModified() == lastMod )
	    return TQApplication::winEventFilter( msg );
	lastMod = fi.lastModified();
	f.open( IO_ReadOnly );
	TQString args;
	f.readLine( args, f.size() );
	TQStringList lst = TQStringList::split( " ", args );

	for ( TQStringList::Iterator it = lst.begin(); it != lst.end(); ++it ) {
	    TQString arg = (*it).stripWhiteSpace();
	    if ( arg[0] != '-' ) {
		TQObjectList* l = MainWindow::self->queryList( "FormWindow" );
		FormWindow* fw = (FormWindow*) l->first();
		FormWindow* totop = 0;
		bool haveit = false;
		while ( fw ) {
		    haveit = haveit || fw->fileName() == arg;
		    if ( haveit )
			totop = fw;

		    fw = (FormWindow*) l->next();
		}

		if ( !haveit ) {
		    FlashWindow( MainWindow::self->winId(), true );
		    MainWindow::self->openFormWindow( arg );
		} else if ( totop ) {
		    totop->setFocus();
		}
		delete l;
	    }
	}
	return true;
    }
    return TQApplication::winEventFilter( msg );
}
#endif
