/****************************************************************************
**
** Implementation of MIME support
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

#include "ntqmime.h"

#ifndef TQT_NO_MIME

#include "ntqmap.h"
#include "ntqstringlist.h"
#include "ntqfileinfo.h"
#include "ntqdir.h"
#include "ntqdragobject.h"
#include "ntqcleanuphandler.h"
#include "ntqapplication.h" // ### for now
#include "ntqclipboard.h" // ### for now

/*!
    \class TQMimeSource ntqmime.h
    \brief The TQMimeSource class is an abstraction of objects which provide formatted data of a certain MIME type.

    \ingroup io
    \ingroup draganddrop
    \ingroup misc

    \link dnd.html Drag-and-drop\endlink and
    \link TQClipboard clipboard\endlink use this abstraction.

    \sa \link http://www.isi.edu/in-notes/iana/assignments/media-types/
	    IANA list of MIME media types\endlink
*/

static int tqt_mime_serial_number = 0;
static TQMimeSourceFactory* defaultfactory = 0;
static TQSingleCleanupHandler<TQMimeSourceFactory> qmime_cleanup_factory;

/*!
    Constructs a mime source and assigns a globally unique serial
    number to it.

    \sa serialNumber()
*/

TQMimeSource::TQMimeSource()
{
    ser_no = tqt_mime_serial_number++;
    cacheType = NoCache;
}

/*!
    \fn int TQMimeSource::serialNumber() const

    Returns the mime source's globally unique serial number.
*/


void TQMimeSource::clearCache()
{
    if ( cacheType == Text ) {
	delete cache.txt.str;
	delete cache.txt.subtype;
	cache.txt.str = 0;
	cache.txt.subtype = 0;
    } else if ( cacheType == Graphics ) {
	delete cache.gfx.img;
	delete cache.gfx.pix;
	cache.gfx.img = 0;
	cache.gfx.pix = 0;
    }
    cacheType = NoCache;
}

/*!
    Provided to ensure that subclasses destroy themselves correctly.
*/
TQMimeSource::~TQMimeSource()
{
#ifndef TQT_NO_CLIPBOARD
    extern void tqt_clipboard_cleanup_mime_source(TQMimeSource *);
    tqt_clipboard_cleanup_mime_source(this);
#endif
    clearCache();
}

/*!
    \fn TQByteArray TQMimeSource::encodedData(const char*) const

    Returns the encoded data of this object in the specified MIME
    format.

    Subclasses must reimplement this function.
*/



/*!
    Returns true if the object can provide the data in format \a
    mimeType; otherwise returns false.

    If you inherit from TQMimeSource, for consistency reasons it is
    better to implement the more abstract canDecode() functions such
    as TQTextDrag::canDecode() and TQImageDrag::canDecode().
*/
bool TQMimeSource::provides(const char* mimeType) const
{
    const char* fmt;
    for (int i=0; (fmt = format(i)); i++) {
	if ( !tqstricmp(mimeType,fmt) ) {
	    return true;
	}
    }
    return false;
}


/*!
    \fn const char * TQMimeSource::format(int i) const

    Returns the \a{i}-th supported MIME format, or 0.
*/



class TQMimeSourceFactoryData {
public:
    TQMimeSourceFactoryData() :
	last(0)
    {
    }

    ~TQMimeSourceFactoryData()
    {
	TQMap<TQString, TQMimeSource*>::Iterator it = stored.begin();
	while ( it != stored.end() ) {
	    delete *it;
	    ++it;
	}
	delete last;
    }

    TQMap<TQString, TQMimeSource*> stored;
    TQMap<TQString, TQString> extensions;
    TQStringList path;
    TQMimeSource* last;
    TQPtrList<TQMimeSourceFactory> factories;
};


/*!
    \class TQMimeSourceFactory ntqmime.h
    \brief The TQMimeSourceFactory class is an extensible provider of mime-typed data.

    \ingroup io
    \ingroup environment

    A TQMimeSourceFactory provides an abstract interface to a
    collection of information. Each piece of information is
    represented by a TQMimeSource object which can be examined and
    converted to concrete data types by functions such as
    TQImageDrag::canDecode() and TQImageDrag::decode().

    The base TQMimeSourceFactory can be used in two ways: as an
    abstraction of a collection of files or as specifically stored
    data. For it to access files, call setFilePath() before accessing
    data. For stored data, call setData() for each item (there are
    also convenience functions, e.g. setText(), setImage() and
    setPixmap(), that simply call setData() with appropriate
    parameters).

    The rich text widgets, TQTextEdit and TQTextBrowser, use
    TQMimeSourceFactory to resolve references such as images or links
    within rich text documents. They either access the default factory
    (see \l{defaultFactory()}) or their own (see
    \l{TQTextEdit::setMimeSourceFactory()}). Other classes that are
    capable of displaying rich text (such as TQLabel, TQWhatsThis or
    TQMessageBox) always use the default factory.

    A factory can also be used as a container to store data associated
    with a name. This technique is useful whenever rich text contains
    images that are stored in the program itself, not loaded from the
    hard disk. Your program may, for example, define some image data
    as:
    \code
    static const char* myimage_data[]={
    "...",
    ...
    "..."};
    \endcode

    To be able to use this image within some rich text, for example
    inside a TQLabel, you must create a TQImage from the raw data and
    insert it into the factory with a unique name:
    \code
    TQMimeSourceFactory::defaultFactory()->setImage( "myimage", TQImage(myimage_data) );
    \endcode

    Now you can create a rich text TQLabel with

    \code
    TQLabel* label = new TQLabel(
	"Rich text with embedded image:<img source=\"myimage\">"
	"Isn't that <em>cute</em>?" );
    \endcode

    When no longer needed, you can clear the data from the factory:

    \code
    delete label;
    TQMimeSourceFactory::defaultFactory()->setData( "myimage", 0 );
    \endcode
*/


/*!
    Constructs a TQMimeSourceFactory that has no file path and no
    stored content.
*/
TQMimeSourceFactory::TQMimeSourceFactory() :
    d(new TQMimeSourceFactoryData)
{
    // add some reasonable defaults
    setExtensionType("htm", "text/html;charset=iso8859-1");
    setExtensionType("html", "text/html;charset=iso8859-1");
    setExtensionType("txt", "text/plain");
    setExtensionType("xml", "text/xml;charset=UTF-8");
    setExtensionType("jpg", "image/jpeg"); // support misspelled jpeg files
}

/*!
    Destroys the TQMimeSourceFactory, deleting all stored content.
*/
TQMimeSourceFactory::~TQMimeSourceFactory()
{
    if ( defaultFactory() == this )
	defaultfactory = 0;
    delete d;
}

TQMimeSource* TQMimeSourceFactory::dataInternal(const TQString& abs_name, const TQMap<TQString, TQString> &extensions ) const
{
    TQMimeSource* r = 0;
    TQFileInfo fi(abs_name);
    if ( fi.isReadable() ) {

	// get the right mimetype
	TQString e = fi.extension(false);
	TQCString mimetype = "application/octet-stream";
	const char* imgfmt;
	if ( extensions.contains(e) )
	    mimetype = extensions[e].latin1();
	else if ( ( imgfmt = TQImage::imageFormat( abs_name ) ) )
	    mimetype = TQCString("image/")+TQCString(imgfmt).lower();

	TQFile f(abs_name);
	if ( f.open(IO_ReadOnly) && f.size() ) {
	    TQByteArray ba(f.size());
	    f.readBlock(ba.data(), ba.size());
	    TQStoredDrag* sr = new TQStoredDrag( mimetype );
	    sr->setEncodedData( ba );
	    delete d->last;
	    d->last = r = sr;
	}
    }

    // we didn't find the mime-source, so ask the default factory for
    // the mime-source (this one will iterate over all installed ones)
    //
    // this looks dangerous, as this dataInternal() function will be
    // called again when the default factory loops over all installed
    // factories (including this), but the static bool looping in
    // data() avoids endless recursions
    if ( !r && this != defaultFactory() )
	r = (TQMimeSource*)defaultFactory()->data( abs_name );

    return r;
}


/*!
    Returns a reference to the data associated with \a abs_name. The
    return value remains valid only until the next data() or setData()
    call, so you should immediately decode the result.

    If there is no data associated with \a abs_name in the factory's
    store, the factory tries to access the local filesystem. If \a
    abs_name isn't an absolute file name, the factory will search for
    it in all defined paths (see \l{setFilePath()}).

    The factory understands all the image formats supported by
    TQImageIO. Any other mime types are determined by the file name
    extension. The default settings are
    \code
    setExtensionType("html", "text/html;charset=iso8859-1");
    setExtensionType("htm", "text/html;charset=iso8859-1");
    setExtensionType("txt", "text/plain");
    setExtensionType("xml", "text/xml;charset=UTF-8");
    \endcode
    The effect of these is that file names ending in "txt" will be
    treated as text encoded in the local encoding; those ending in
    "xml" will be treated as text encoded in Unicode UTF-8 encoding.
    The text/html type is treated specially, since the encoding can be
    specified in the html file itself. "html" or "htm" will be treated
    as text encoded in the encoding specified by the html meta tag, if
    none could be found, the charset of the mime type will be used.
    The text subtype ("html", "plain", or "xml") does not affect the
    factory, but users of the factory may behave differently. We
    recommend creating "xml" files where practical. These files can be
    viewed regardless of the runtime encoding and can encode any
    Unicode characters without resorting to encoding definitions
    inside the file.

    Any file data that is not recognized will be retrieved as a
    TQMimeSource providing the "application/octet-stream" mime type,
    meaning uninterpreted binary data.

    You can add further extensions or change existing ones with
    subsequent calls to setExtensionType(). If the extension mechanism
    is not sufficient for your problem domain, you can inherit
    TQMimeSourceFactory and reimplement this function to perform some
    more specialized mime-type detection. The same applies if you want
    to use the mime source factory to access URL referenced data over
    a network.
*/
const TQMimeSource* TQMimeSourceFactory::data(const TQString& abs_name) const
{
    if ( d->stored.contains(abs_name) )
	return d->stored[abs_name];

    TQMimeSource* r = 0;
    TQStringList::Iterator it;
    if ( abs_name[0] == '/'
#ifdef TQ_WS_WIN
	    || ( abs_name[0] && abs_name[1] == ':' ) || abs_name.startsWith("\\\\")
#endif
    )
    {
	// handle absolute file names directly
	r = dataInternal( abs_name, d->extensions);
    }
    else { // check list of paths
	for ( it = d->path.begin(); !r && it != d->path.end(); ++it ) {
	    TQString filename = *it;
	    if ( filename[(int)filename.length()-1] != '/' )
		filename += '/';
	    filename += abs_name;
	    r = dataInternal( filename, d->extensions );
	}
    }

    static bool looping = false;
    if ( !r && this == defaultFactory() ) {
	// we found no mime-source and we are the default factory, so
	// we know all the other installed mime-source factories, so
	// ask them
	if ( !looping ) {
	    // to avoid endless recustions, don't enter the loop below
	    // if data() got called from within the loop below
	    looping = true;
	    TQPtrListIterator<TQMimeSourceFactory> it( d->factories );
	    TQMimeSourceFactory *f;
	    while ( ( f = it.current() ) ) {
		++it;
		if ( f == this )
		    continue;
		r = (TQMimeSource*)f->data( abs_name );
		if ( r ) {
		    looping = false;
		    return r;
		}
	    }
	    looping = false;
	}
    } else if ( !r ) {
	// we are not the default mime-source factory, so ask the
	// default one for the mime-source, as this one will loop over
	// all installed mime-source factories and ask these
	r = (TQMimeSource*)defaultFactory()->data( abs_name );
    }

    return r;
}

/*!
    Sets the list of directories that will be searched when named data
    is requested to the those given in the string list \a path.

    \sa filePath()
*/
void TQMimeSourceFactory::setFilePath( const TQStringList& path )
{
    d->path = path;
}

/*!
    Returns the currently set search paths.
*/
TQStringList TQMimeSourceFactory::filePath() const
{
    return d->path;
}

/*!
    Adds another search path, \a p to the existing search paths.

    \sa setFilePath()
*/
void TQMimeSourceFactory::addFilePath( const TQString& p )
{
    d->path += p;
}

/*!
    Sets the mime-type to be associated with the file name extension,
    \a ext to \a mimetype. This determines the mime-type for files
    found via the paths set by setFilePath().
*/
void TQMimeSourceFactory::setExtensionType( const TQString& ext, const char* mimetype )
{
    d->extensions.replace(ext, mimetype);
}

/*!
    Converts the absolute or relative data item name \a
    abs_or_rel_name to an absolute name, interpreted within the
    context (path) of the data item named \a context (this must be an
    absolute name).
*/
TQString TQMimeSourceFactory::makeAbsolute(const TQString& abs_or_rel_name, const TQString& context) const
{
    if ( context.isNull() ||
	 !(context[0] == '/'
#ifdef TQ_WS_WIN
	 || ( context[0] && context[1] == ':')
#endif
	   ))
	return abs_or_rel_name;
    if ( abs_or_rel_name.isEmpty() )
	return context;
    TQFileInfo c( context );
    if (!c.isDir()) {
	TQFileInfo r( c.dir(true), abs_or_rel_name );
	return r.absFilePath();
    } else {
	TQDir d(context);
	TQFileInfo r(d, abs_or_rel_name);
	return r.absFilePath();
    }
}

/*!
    \overload
    A convenience function. See data(const TQString& abs_name). The
    file name is given in \a abs_or_rel_name and the path is in \a
    context.
*/
const TQMimeSource* TQMimeSourceFactory::data(const TQString& abs_or_rel_name, const TQString& context) const
{
    const TQMimeSource* r = data(makeAbsolute(abs_or_rel_name,context));
    if ( !r && !d->path.isEmpty() )
	r = data(abs_or_rel_name);
    return r;
}


/*!
    Sets \a text to be the data item associated with the absolute name
    \a abs_name.

    Equivalent to setData(abs_name, new TQTextDrag(text)).
*/
void TQMimeSourceFactory::setText( const TQString& abs_name, const TQString& text )
{
    setData(abs_name, new TQTextDrag(text));
}

/*!
    Sets \a image to be the data item associated with the absolute
    name \a abs_name.

    Equivalent to setData(abs_name, new TQImageDrag(image)).
*/
void TQMimeSourceFactory::setImage( const TQString& abs_name, const TQImage& image )
{
    setData(abs_name, new TQImageDrag(image));
}

/*!
    Sets \a pixmap to be the data item associated with the absolute
    name \a abs_name.
*/
void TQMimeSourceFactory::setPixmap( const TQString& abs_name, const TQPixmap& pixmap )
{
    setData(abs_name, new TQImageDrag(pixmap.convertToImage()));
}

/*!
  Sets \a data to be the data item associated with
  the absolute name \a abs_name. Note that the ownership of \a data is
  transferred to the factory: do not delete or access the pointer after
  passing it to this function.

  Passing 0 for data removes previously stored data.
*/
void TQMimeSourceFactory::setData( const TQString& abs_name, TQMimeSource* data )
{
    if ( d->stored.contains(abs_name) )
	delete d->stored[abs_name];
    d->stored.replace(abs_name,data);
}


/*!
    Returns the application-wide default mime source factory. This
    factory is used by rich text rendering classes such as
    TQSimpleRichText, TQWhatsThis and TQMessageBox to resolve named
    references within rich text documents. It serves also as the
    initial factory for the more complex render widgets, TQTextEdit and
    TQTextBrowser.

    \sa setDefaultFactory()
*/
TQMimeSourceFactory* TQMimeSourceFactory::defaultFactory()
{
    if (!defaultfactory)
    {
	defaultfactory = new TQMimeSourceFactory();
	qmime_cleanup_factory.set( &defaultfactory );
    }
    return defaultfactory;
}

/*!
    Sets the default \a factory, destroying any previously set mime
    source provider. The ownership of the factory is transferred to
    TQt.

    \sa defaultFactory()
*/
void TQMimeSourceFactory::setDefaultFactory( TQMimeSourceFactory* factory)
{
    if ( !defaultfactory )
	qmime_cleanup_factory.set( &defaultfactory );
    else if ( defaultfactory != factory )
	delete defaultfactory;
    defaultfactory = factory;
}

/*!
    Sets the defaultFactory() to 0 and returns the previous one.
*/

TQMimeSourceFactory* TQMimeSourceFactory::takeDefaultFactory()
{
    TQMimeSourceFactory *f = defaultfactory;
    defaultfactory = 0;
    return f;
}

/*!
    Adds the TQMimeSourceFactory \a f to the list of available
    mimesource factories. If the defaultFactory() can't resolve a
    data() it iterates over the list of installed mimesource factories
    until the data can be resolved.

    \sa removeFactory();
*/

void TQMimeSourceFactory::addFactory( TQMimeSourceFactory *f )
{
    TQMimeSourceFactory::defaultFactory()->d->factories.append( f );
}

/*!
    Removes the mimesource factory \a f from the list of available
    mimesource factories.

    \sa addFactory();
*/

void TQMimeSourceFactory::removeFactory( TQMimeSourceFactory *f )
{
    TQMimeSourceFactory::defaultFactory()->d->factories.removeRef( f );
}

#endif // TQT_NO_MIME
