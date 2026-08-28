/****************************************************************************
**
** Implementation of TQClipboard class
**
** Created : 960430
**
** Copyright (C) 1992-2008 Trolltech ASA.  All rights reserved.
**
** This file is part of the kernel module of the TQt GUI Toolkit.
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

#include "ntqclipboard.h"

#ifndef TQT_NO_CLIPBOARD

#include "ntqapplication.h"
#include "qapplication_p.h"
#include "ntqdragobject.h"
#include "ntqpixmap.h"

/*!
    \class TQClipboard ntqclipboard.h
    \brief The TQClipboard class provides access to the window system clipboard.

    \ingroup io
    \ingroup environment
    \mainclass

    The clipboard offers a simple mechanism to copy and paste data
    between applications.

    TQClipboard supports the same data types that TQDragObject does, and
    uses similar mechanisms. For advanced clipboard usage
    read \link dnd.html the drag-and-drop documentation\endlink.

    There is a single TQClipboard object in an application, and you can
    access it using TQApplication::clipboard().

    Example:
    \code
    TQClipboard *cb = TQApplication::clipboard();

    // Copy text from the clipboard (paste)
    TQString text = cb->text(TQClipboard::Clipboard);
    if ( !text.isNull() )
	tqDebug( "The clipboard contains: " + text );

    // Copy text into the clipboard
    cb->setText( "This text can be pasted by other programs",
		 TQClipboard::Clipboard );
    \endcode

    TQClipboard features some convenience functions to access common data
    types: setText() allows the exchange of Unicode text and
    setPixmap() and setImage() allows the exchange of TQPixmaps
    and TQImages between applications. The setData() function is the
    ultimate in flexibility: it allows you to add any TQMimeSource into the
    clipboard. There are corresponding getters for each of these, e.g.
    text(), image() and pixmap().

    You can clear the clipboard by calling clear().


    \section1 Platform Specific Information

    \section2 X11

    \list

    \i The X11 Window System has the concept of a separate selection
    and clipboard.  When text is selected, it is immediately available
    as the global mouse selection.  The global mouse selection may
    later be copied to the clipboard.  By convention, the middle mouse
    button is used to paste the global mouse selection.

    \i X11 also has the concept of ownership; if you change the
    selection within a window, X11 will only notify the owner and the
    previous owner of the change, i.e. it will not notify all
    applications that the selection or clipboard data changed.

    \i Lastly, the X11 clipboard is event driven, i.e. the clipboard
    will not function properly if the event loop is not running.
    Similarly, it is recommended that the contents of the clipboard
    are stored or retrieved in direct response to user-input events,
    e.g. mouse button or key presses and releases.  You should not
    store or retrieve the clipboard contents in response to timer or
    non-user-input events.

    \endlist

    \section2 Windows

    \list

    \i Microsoft Windows does not support the global mouse selection;
    it only supports the global clipboard, e.g. Windows only adds text
    to the clipboard when an explicit copy or cut is made.

    \i Windows does not have the concept of ownership; the clipboard
    is a fully global resource so all applications are notified of
    changes.

    \endlist

    See the multiclip example in the \e{TQt Designer} examples
    directory for an example of a multiplatform clipboard application
    that also demonstrates selection handling.
*/


/*!
    \internal

    Constructs a clipboard object.

    Do not call this function.

    Call TQApplication::clipboard() instead to get a pointer to the
    application's global clipboard object.

    There is only one clipboard in the window system, and creating
    more than one object to represent it is almost certainly an error.
*/

TQClipboard::TQClipboard( TQObject *parent, const char *name )
    : TQObject( parent, name )
{
    // nothing
}

#ifndef TQ_WS_WIN32
/*!
    \internal

    Destroys the clipboard.

    You should never delete the clipboard. TQApplication will do this
    when the application terminates.
*/
TQClipboard::~TQClipboard()
{
}
#endif

/*!
    \fn void TQClipboard::dataChanged()

    This signal is emitted when the clipboard data is changed.
*/

/*!
    \fn void TQClipboard::selectionChanged()

    This signal is emitted when the selection is changed. This only
    applies to windowing systems that support selections, e.g. X11.
    Windows doesn't support selections.
*/

/*! \enum TQClipboard::Mode
    \keyword clipboard mode

    This enum type is used to control which part of the system clipboard is
    used by TQClipboard::data(), TQClipboard::setData() and related functions.

    \value Clipboard  indicates that data should be stored and retrieved from
    the global clipboard.

    \value Selection  indicates that data should be stored and retrieved from
    the global mouse selection.

    \e Note: Support for \c Selection is provided only on systems with a
    global mouse selection (e.g. X11).

    \sa TQClipboard::supportsSelection()
*/


/*****************************************************************************
  TQApplication member functions related to TQClipboard.
 *****************************************************************************/

#ifndef TQT_NO_MIMECLIPBOARD
// text handling is done directly in qclipboard_qws, for now

/*!
    \overload

    Returns the clipboard text in subtype \a subtype, or a null string
    if the clipboard does not contain any text. If \a subtype is null,
    any subtype is acceptable, and \a subtype is set to the chosen
    subtype.

    The \a mode argument is used to control which part of the system
    clipboard is used.  If \a mode is TQClipboard::Clipboard, the
    text is retrieved from the global clipboard.  If \a mode is
    TQClipboard::Selection, the text is retrieved from the global
    mouse selection.

    Common values for \a subtype are "plain" and "html".

    \sa setText(), data(), TQString::operator!()
*/
TQString TQClipboard::text( TQCString &subtype, Mode mode ) const
{
    TQString r;
    TQTextDrag::decode( data( mode ) ,r, subtype );
    return r;
}

/*!
    \overload

    Returns the clipboard text in subtype \a subtype, or a null string
    if the clipboard does not contain any text. This function uses the
    TQClipboard::text() function which takes a TQClipboard::Mode
    argument.  The value of the mode argument is determined by the
    return value of selectionModeEnabled(). If selectionModeEnabled()
    returns true, the mode argument is TQClipboard::Selection,
    otherwise the mode argument is TQClipboard::Clipboard.
*/
// ### remove 4.0
TQString TQClipboard::text( TQCString& subtype ) const
{
    return text( subtype, selectionModeEnabled() ? Selection : Clipboard );
}

/*!
    Returns the clipboard text as plain text, or a null string if the
    clipboard does not contain any text.

    The \a mode argument is used to control which part of the system
    clipboard is used.  If \a mode is TQClipboard::Clipboard, the
    text is retrieved from the global clipboard.  If \a mode is
    TQClipboard::Selection, the text is retrieved from the global
    mouse selection.

    \sa setText(), data(), TQString::operator!()
*/
TQString TQClipboard::text( Mode mode ) const
{
    TQCString subtype = "plain";
    return text( subtype, mode );
}

/*!
    \overload

    This function uses the TQClipboard::text() function which takes
    a TQClipboard::Mode argument.  The value of the mode argument is
    determined by the return value of selectionModeEnabled().
    If selectionModeEnabled() returns true, the mode argument is
    TQClipboard::Selection, otherwise the mode argument is
    TQClipboard::Clipboard.
*/

TQString TQClipboard::text() const
{
    return text( selectionModeEnabled() ? Selection : Clipboard );
}

 /*!
    Copies \a text into the clipboard as plain text.

    The \a mode argument is used to control which part of the system
    clipboard is used.  If \a mode is TQClipboard::Clipboard, the
    text is stored in the global clipboard.  If \a mode is
    TQClipboard::Selection, the text is stored in the global
    mouse selection.

    \sa text(), setData()
*/

void TQClipboard::setText( const TQString &text, Mode mode )
{
    setData( new TQTextDrag(text), mode );
}

/*!
    \overload

    This function uses the TQClipboard::setText() function which takes
    a TQClipboard::Mode argument.  The value of the mode argument is
    determined by the return value of selectionModeEnabled().
    If selectionModeEnabled() returns true, the mode argument is
    TQClipboard::Selection, otherwise the mode argument is
    TQClipboard::Clipboard.
*/
// ### remove 4.0
void TQClipboard::setText( const TQString &text )
{
    setText( text, selectionModeEnabled() ? Selection : Clipboard );
}

/*!
    Returns the clipboard image, or returns a null image if the
    clipboard does not contain an image or if it contains an image in
    an unsupported image format.

    The \a mode argument is used to control which part of the system
    clipboard is used.  If \a mode is TQClipboard::Clipboard, the
    image is retrieved from the global clipboard.  If \a mode is
    TQClipboard::Selection, the image is retrieved from the global
    mouse selection.

    \sa setImage() pixmap() data(), TQImage::isNull()
*/
TQImage TQClipboard::image( Mode mode ) const
{
    TQImage r;
    TQImageDrag::decode( data( mode ), r );
    return r;
}

/*!
    \overload

    This function uses the TQClipboard::image() function which takes
    a TQClipboard::Mode argument.  The value of the mode argument is
    determined by the return value of selectionModeEnabled().
    If selectionModeEnabled() returns true, the mode argument is
    TQClipboard::Selection, otherwise the mode argument is
    TQClipboard::Clipboard.
*/
// ### remove 4.0
TQImage TQClipboard::image() const
{
    return image( selectionModeEnabled() ? Selection : Clipboard );
}

/*!
    Copies \a image into the clipboard.

    The \a mode argument is used to control which part of the system
    clipboard is used.  If \a mode is TQClipboard::Clipboard, the
    image is stored in the global clipboard.  If \a mode is
    TQClipboard::Selection, the data is stored in the global
    mouse selection.

    This is shorthand for:
    \code
        setData( new TQImageDrag(image), mode )
    \endcode

    \sa image(), setPixmap() setData()
*/
void TQClipboard::setImage( const TQImage &image, Mode mode )
{
    setData( new TQImageDrag( image ), mode );
}

/*!
    \overload

    This function uses the TQClipboard::setImage() function which takes
    a TQClipboard::Mode argument.  The value of the mode argument is
    determined by the return value of selectionModeEnabled().
    If selectionModeEnabled() returns true, the mode argument is
    TQClipboard::Selection, otherwise the mode argument is
    TQClipboard::Clipboard.
*/
// ### remove 4.0
void TQClipboard::setImage( const TQImage &image )
{
    setImage( image, selectionModeEnabled() ? Selection : Clipboard );
}

/*!
    Returns the clipboard pixmap, or null if the clipboard does not
    contain a pixmap. Note that this can lose information. For
    example, if the image is 24-bit and the display is 8-bit, the
    result is converted to 8 bits, and if the image has an alpha
    channel, the result just has a mask.

    The \a mode argument is used to control which part of the system
    clipboard is used.  If \a mode is TQClipboard::Clipboard, the
    pixmap is retrieved from the global clipboard.  If \a mode is
    TQClipboard::Selection, the pixmap is retrieved from the global
    mouse selection.

    \sa setPixmap() image() data() TQPixmap::convertFromImage().
*/
TQPixmap TQClipboard::pixmap( Mode mode ) const
{
    TQPixmap r;
    TQImageDrag::decode( data( mode ), r );
    return r;
}

/*!
    \overload

    This function uses the TQClipboard::pixmap() function which takes
    a TQClipboard::Mode argument.  The value of the mode argument is
    determined by the return value of selectionModeEnabled().
    If selectionModeEnabled() returns true, the mode argument is
    TQClipboard::Selection, otherwise the mode argument is
    TQClipboard::Clipboard.
*/
// ### remove 4.0
TQPixmap TQClipboard::pixmap() const
{
    return pixmap( selectionModeEnabled() ? Selection : Clipboard );
}

/*!
    Copies \a pixmap into the clipboard. Note that this is slower
    than setImage() because it needs to convert the TQPixmap to a
    TQImage first.

    The \a mode argument is used to control which part of the system
    clipboard is used.  If \a mode is TQClipboard::Clipboard, the
    pixmap is stored in the global clipboard.  If \a mode is
    TQClipboard::Selection, the pixmap is stored in the global
    mouse selection.

    \sa pixmap() setImage() setData()
*/
void TQClipboard::setPixmap( const TQPixmap &pixmap, Mode mode )
{
    // *could* just use the handle, but that is X hackery, MIME is better.
    setData( new TQImageDrag( pixmap.convertToImage() ), mode );
}

/*!
    \overload

    This function uses the TQClipboard::setPixmap() function which takes
    a TQClipboard::Mode argument.  The value of the mode argument is
    determined by the return value of selectionModeEnabled().
    If selectionModeEnabled() returns true, the mode argument is
    TQClipboard::Selection, otherwise the mode argument is
    TQClipboard::Clipboard.
*/
// ### remove 4.0
void TQClipboard::setPixmap( const TQPixmap &pixmap )
{
    setPixmap( pixmap, selectionModeEnabled() ? Selection : Clipboard );
}


/*! \fn TQMimeSource *TQClipboard::data( Mode mode ) const
    Returns a reference to a TQMimeSource representation of the current
    clipboard data.

    The \a mode argument is used to control which part of the system
    clipboard is used.  If \a mode is TQClipboard::Clipboard, the
    data is retrieved from the global clipboard.  If \a mode is
    TQClipboard::Selection, the data is retrieved from the global
    mouse selection.

    \sa setData()
*/

/*!
    \overload

    This function uses the TQClipboard::data() function which takes
    a TQClipboard::Mode argument.  The value of the mode argument is
    determined by the return value of selectionModeEnabled().
    If selectionModeEnabled() returns true, the mode argument is
    TQClipboard::Selection, otherwise the mode argument is
    TQClipboard::Clipboard.
*/
// ### remove 4.0
TQMimeSource *TQClipboard::data() const
{
    return data( selectionModeEnabled() ? Selection : Clipboard );
}

/*! \fn void TQClipboard::setData( TQMimeSource *src, Mode mode )
    Sets the clipboard data to \a src. Ownership of the data is
    transferred to the clipboard. If you want to remove the data
    either call clear() or call setData() again with new data.

    The \a mode argument is used to control which part of the system
    clipboard is used.  If \a mode is TQClipboard::Clipboard, the
    data is retrieved from the global clipboard.  If \a mode is
    TQClipboard::Selection, the data is retrieved from the global
    mouse selection.

    The TQDragObject subclasses are reasonable objects to put into the
    clipboard (but do not try to call TQDragObject::drag() on the same
    object). Any TQDragObject placed in the clipboard should have a
    parent of 0. Do not put TQDragMoveEvent or TQDropEvent subclasses in
    the clipboard, as they do not belong to the event handler which
    receives them.

    The setText(), setImage() and setPixmap() functions are simpler
    wrappers for setting text, image and pixmap data respectively.

    \sa data()
*/

/*!
    \overload

    This function uses the TQClipboard::setData() function which takes
    a TQClipboard::Mode argument.  The value of the mode argument is
    determined by the return value of selectionModeEnabled().
    If selectionModeEnabled() returns true, the mode argument is
    TQClipboard::Selection, otherwise the mode argument is
    TQClipboard::Clipboard.
*/
// ### remove 4.0
void TQClipboard::setData( TQMimeSource *src )
{
    setData( src, selectionModeEnabled() ? Selection : Clipboard );
}

/*! \fn void TQClipboard::clear( Mode mode )
    Clear the clipboard contents.

    The \a mode argument is used to control which part of the system
    clipboard is used.  If \a mode is TQClipboard::Clipboard, this
    function clears the the global clipboard contents.  If \a mode is
    TQClipboard::Selection, this function clears the global mouse
    selection contents.

    \sa TQClipboard::Mode, supportsSelection()
*/

/*!
    \overload

    This function uses the TQClipboard::clear() function which takes
    a TQClipboard::Mode argument.  The value of the mode argument is
    determined by the return value of selectionModeEnabled().
    If selectionModeEnabled() returns true, the mode argument is
    TQClipboard::Selection, otherwise the mode argument is TQClipboard::Clipboard.
*/
// ### remove 4.0
void TQClipboard::clear()
{
    clear( selectionModeEnabled() ? Selection : Clipboard );
}

#endif // TQT_NO_MIMECLIPBOARD
#endif // TQT_NO_CLIPBOARD
