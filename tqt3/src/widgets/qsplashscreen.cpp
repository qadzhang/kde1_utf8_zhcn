/****************************************************************************
**
** Definition of TQSplashScreen class
**
** Copyright (C) 2005-2008 Trolltech ASA.  All rights reserved.
**
** This file is part of the widgets module of the TQt GUI Toolkit.
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
** This file may be used under the terms of the Q Public License as
** defined by Trolltech ASA and appearing in the file LICENSE.TQPL
** included in the packaging of this file.  Licensees holding valid TQt
** Commercial licenses may use this file in accordance with the TQt
** Commercial License Agreement provided with the Software.
**
** This file is provided "AS IS" with NO WARRANTY OF ANY KIND,
** INCLUDING THE WARRANTIES OF DESIGN, MERCHANTABILITY AND FITNESS FOR
** A PARTICULAR PURPOSE. Trolltech reserves all rights not granted
** herein.
**
**********************************************************************/

#include "ntqsplashscreen.h"

#ifndef TQT_NO_SPLASHSCREEN

#include "ntqapplication.h"
#include "ntqpainter.h"
#include "ntqpixmap.h"

class TQSplashScreenPrivate
{
public:
    TQPixmap pixmap;
    TQString currStatus;
    TQColor currColor;
    int currAlign;
};

/*!
   \class TQSplashScreen ntqsplashscreen.h
   \brief The TQSplashScreen widget provides a splash screen that can
   be shown during application startup.

    \ingroup misc
    \mainclass

   A splash screen is a widget that is usually displayed when an
   application is being started. Splash screens are often used for
   applications that have long start up times (e.g. database or
   networking applications that take time to establish connections) to
   provide the user with feedback that the application is loading.

   The splash screen appears centered on the screen. It may be useful to add
   the \c WStyle_StaysOnTop if you desire to keep above all the windows in the
   GUI.

   Some X11 window managers do not support the "stays on top" flag. A
   solution is to set up a timer that periodically calls raise() on
   the splash screen to simulate the "stays on top" effect.

   The most common usage is to show a splash screen before the main
   widget is displayed on the screen. This is illustrated in the
   following code snippet.

   \code
   int main( int argc, char **argv )
   {
       TQApplication app( argc, argv );
       TQPixmap pixmap( "splash.png" );
       TQSplashScreen *splash = new TQSplashScreen( pixmap );
       splash->show();
       TQMainWindow *mainWin = new TQMainWindow;
       ...
       app.setMainWidget( mainWin );
       mainWin->show();
       splash->finish( mainWin );
       delete splash;
       return app.exec();
   }
   \endcode

   It is sometimes useful to update the splash screen with messages,
   for example, announcing connections established or modules loaded
   as the application starts up. TQSplashScreen supports this with the
   message() function. If you wish to do your own drawing you can
   get a pointer to the pixmap used in the splash screen with pixmap().
   Alternatively, you can subclass TQSplashScreen and reimplement
   drawContents().

   The user can hide the splash screen by clicking on it with the
   mouse. Since the splash screen is typically displayed before the
   event loop has started running, it is necessary to periodically
   call TQApplication::processEvents() to receive the mouse clicks.

   \code
   TQPixmap pixmap( "splash.png" );
   TQSplashScreen *splash = new TQSplashScreen( pixmap );
   splash->show();
   ... // Loading some items
   splash->message( "Loaded modules" );
   tqApp->processEvents();
   ... // Establishing connections
   splash->message( "Established connections" );
   tqApp->processEvents();
   \endcode

*/

/*!
    Construct a splash screen that will display the \a pixmap.

    There should be no need to set the widget flags, \a f, except
    perhaps \c WDestructiveClose or \c WStyle_StaysOnTop.
*/
TQSplashScreen::TQSplashScreen( const TQPixmap &pixmap, WFlags f )
    : TQWidget( 0, 0, WStyle_Customize | WStyle_Splash | f )
{
    d = new TQSplashScreenPrivate();
    d->pixmap = pixmap;
    setPixmap( d->pixmap );  // Does an implicit repaint
}

/*!
  Destructor.
*/
TQSplashScreen::~TQSplashScreen()
{
    delete d;
}

/*!
    \reimp
*/
void TQSplashScreen::mousePressEvent( TQMouseEvent * )
{
    hide();
}

/*!
    This overrides TQWidget::repaint(). It differs from the standard
    repaint function in that it also calls TQApplication::flush() to
    ensure the updates are displayed, even when there is no event loop
    present.
*/
void TQSplashScreen::repaint()
{
    drawContents();
    TQWidget::repaint();
    TQApplication::flush();
}

/*!
    \fn TQSplashScreen::messageChanged( const TQString &message )

    This signal is emitted when the message on the splash screen
    changes. \a message is the new message and is a null-string
    when the message has been removed.

    \sa message(), clear()
*/



/*!
    Draws the \a message text onto the splash screen with color \a
    color and aligns the text according to the flags in \a alignment.

    \sa TQt::AlignmentFlags clear()
*/
void TQSplashScreen::message( const TQString &message, int alignment,
			     const TQColor &color )
{
    d->currStatus = message;
    d->currAlign = alignment;
    d->currColor = color;
    emit messageChanged( d->currStatus );
    repaint();
}

/*!
    Removes the message being displayed on the splash screen

    \sa message()
 */
void TQSplashScreen::clear()
{
    d->currStatus = TQString::null;
    emit messageChanged( d->currStatus );
    repaint();
}

/*!
    Makes the splash screen wait until the widget \a mainWin is displayed
    before calling close() on itself.
*/
void TQSplashScreen::finish( TQWidget *mainWin )
{
    if ( mainWin ) {
#if defined(TQ_WS_X11)
	extern void tqt_wait_for_window_manager( TQWidget *mainWin );
	tqt_wait_for_window_manager( mainWin );
#endif
    }
    close();
}

/*!
    Sets the pixmap that will be used as the splash screen's image to
    \a pixmap.
*/
void TQSplashScreen::setPixmap( const TQPixmap &pixmap )
{
    d->pixmap = pixmap;
    TQRect r(0, 0, d->pixmap.size().width(), d->pixmap.size().height());
    resize( d->pixmap.size() );
    move( TQApplication::desktop()->screenGeometry().center() - r.center() );
    repaint();
}

/*!
    Returns the pixmap that is used in the splash screen. The image
    does not have any of the text drawn by message() calls.
*/
TQPixmap* TQSplashScreen::pixmap() const
{
    return &( d->pixmap );
}

/*!
  \internal
*/
void TQSplashScreen::drawContents()
{
    TQPixmap textPix = d->pixmap;
    TQPainter painter( &textPix, this );
    drawContents( &painter );
    setErasePixmap( textPix );
}

/*!
    Draw the contents of the splash screen using painter \a painter.
    The default implementation draws the message passed by message().
    Reimplement this function if you want to do your own drawing on
    the splash screen.
*/
void TQSplashScreen::drawContents( TQPainter *painter )
{
    painter->setPen( d->currColor );
    TQRect r = rect();
    r.setRect( r.x() + 5, r.y() + 5, r.width() - 10, r.height() - 10 );
    painter->drawText( r, d->currAlign, d->currStatus );
}

#endif //TQT_NO_SPLASHSCREEN
