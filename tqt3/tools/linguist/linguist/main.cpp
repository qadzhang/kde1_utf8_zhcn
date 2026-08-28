/**********************************************************************
** Copyright (C) 2000-2008 Trolltech ASA.  All rights reserved.
**
** This file is part of TQt Linguist.
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

#include "trwindow.h"

#include <ntqapplication.h>
#include <ntqlabel.h>
#include <ntqpixmap.h>
#include <ntqtimer.h>
#include <ntqtextcodec.h>
#include <ntqtranslator.h>
#include <ntqsettings.h>
#include <ntqsplashscreen.h>

int main( int argc, char **argv )
{
    TQApplication app( argc, argv );
    TQApplication::setOverrideCursor( TQt::waitCursor );


    TQTranslator translator( 0 );
    translator.load( TQString( "linguist_" ) + TQTextCodec::locale(), "." );
    app.installTranslator( &translator );

    bool showSplash = true;

    TQString keybase("/TQt Linguist/3.1/");
    TQSettings config;
    config.insertSearchPath( TQSettings::Windows, "/Trolltech" );

    TQRect r( TQApplication::desktop()->screenGeometry() );
    r.setX( config.readNumEntry( keybase + "Geometry/MainwindowX", r.x() ) );
    r.setY( config.readNumEntry( keybase + "Geometry/MainwindowY", r.y() ) );
    r.setWidth( config.readNumEntry( keybase + "Geometry/MainwindowWidth", r.width() ) );
    r.setHeight( config.readNumEntry( keybase + "Geometry/MainwindowHeight", r.height() ) );

    TQSplashScreen *splash = 0;
    if ( showSplash ) {
	splash = new TQSplashScreen( TQPixmap::fromMimeSource("splash.png"),
				    TQt::WDestructiveClose );
	splash->show();
    }

    TrWindow *tw = new TrWindow;
    app.setMainWidget( tw );

    if ( app.argc() > 1 )
	tw->openFile( TQString(app.argv()[app.argc() - 1]) );

    if ( config.readBoolEntry( keybase + "Geometry/MainwindowMaximized", false ) )
	tw->showMaximized();
    else
	tw->show();
    if ( splash )
	splash->finish( tw );
    TQApplication::restoreOverrideCursor();

    return app.exec();
}
