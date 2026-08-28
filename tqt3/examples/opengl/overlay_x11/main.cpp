/****************************************************************************
**
** Example application showing how to use TQt and TQt OpenGL Extension on an 
** X11 overlay visual 
**
** Copyright (C) 1999-2008 Trolltech ASA.  All rights reserved.
**
** This file is part of an example program for TQt.  This example
** program may be used, distributed and modified without limitation.
**
*****************************************************************************/

#include <ntqapplication.h>
#include "gearwidget.h"
#include "rubberbandwidget.h"

#if defined(TQ_WS_X11)
#include <X11/Xlib.h>
#endif

TQColor findOverlayTransparentColor()
{
    TQColor invalidColor;

#if defined(TQ_WS_X11)

    Display* appDisplay;
    Visual* appVisual;

    // The static methods are called 'App' in TQt 2.x
#if TQT_VERSION < 200
    appDisplay = TQPaintDevice::x__Display();
    appVisual = (Visual*)TQPaintDevice::x11Visual();
#else
    appDisplay = TQPaintDevice::x11AppDisplay();
    appVisual = (Visual*)TQPaintDevice::x11AppVisual();
#endif

    tqDebug( "Default Visual ID: 0x%x", (int)XVisualIDFromVisual(appVisual) );

    typedef struct OverlayProp {
	long  visual;
	long  type;
	long  value;
	long  layer;
    } OverlayProp;

    TQWidget* rootWin = TQApplication::desktop();
    if ( !rootWin )
	return invalidColor; // Should not happen

    Atom overlayVisualsAtom = XInternAtom( appDisplay, 
					   "SERVER_OVERLAY_VISUALS", True );
    if ( overlayVisualsAtom == None ) {
	warning( "Server has no overlay visuals" );
	return invalidColor;
    }

    Atom actualType;
    int actualFormat;
    ulong nItems;
    ulong bytesAfter;
    OverlayProp* overlayProp;
    int res = XGetWindowProperty( appDisplay, TQApplication::desktop()->winId(),
				  overlayVisualsAtom, 0, 10000, False, 
				  overlayVisualsAtom, &actualType, 
				  &actualFormat, &nItems, &bytesAfter,
				  (uchar**)&overlayProp );

    if ( res != Success || actualType != overlayVisualsAtom 
	 || actualFormat != 32 || nItems < 4 ) {
	warning( "Failed to get overlay visual property from server" );
	return invalidColor;
    }


    for ( uint i = 0; i < nItems/4; i++ ) {
	if ( (VisualID)overlayProp[i].visual == XVisualIDFromVisual(appVisual)
	     && overlayProp[i].type == 1 )
	    return TQColor( tqRgb( 1, 2, 3 ), overlayProp[i].value );
    }

    tqWarning( "Default visual is not in overlay plane" );
    return invalidColor;

#else // defined(TQ_WS_X11)
    tqWarning( "Wrong window system - Only X11 has overlay support." );
    return invalidColor;
#endif
}


int main( int argc, char **argv )
{
    TQApplication::setColorSpec( TQApplication::CustomColor );
    TQApplication a( argc, argv );

    if ( !TQGLFormat::hasOpenGL() ) {
	tqWarning( "This system has no OpenGL support. Exiting." );
	return -1;
    }

    TQColor transparentColor = findOverlayTransparentColor();
    if ( !transparentColor.isValid() ) {
	tqWarning( "Failed to get transparent color for overlay. Exiting." );
	return -1;
    }

    TQWidget top;
    a.setMainWidget( &top );
    top.setGeometry( 50, 50, 600, 400 );

    // Make an OpenGL widget. It will use the deepest visual available
    // (typically a TrueColor visual), which typically is in the normal layer.
    GearWidget g( &top );
    g.setGeometry( 20, 20, 560, 360 );

    // Put the rubberband widget (which uses the default, i.e. overlay visual)
    // on top of the OpenGL widget:
    RubberbandWidget r( transparentColor, &top );
    r.setGeometry( 20, 20, 560, 360 );

    top.show();
    return a.exec();
}
