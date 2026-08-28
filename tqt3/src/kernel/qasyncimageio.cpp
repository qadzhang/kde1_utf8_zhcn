/****************************************************************************
**
** Implementation of asynchronous image/movie loading classes
**
** Created : 970617
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

#include "ntqasyncimageio.h"

#ifndef TQT_NO_ASYNC_IMAGE_IO

#include "ntqptrlist.h"
#include "ntqgif.h"
#include <stdlib.h>

extern void tqt_init_image_handlers();
extern void tqt_init_image_plugins();

#define Q_TRANSPARENT 0x00ffffff

/*!
    \class TQImageConsumer ntqasyncimageio.h
    \brief The TQImageConsumer class is an abstraction used by TQImageDecoder.

    \ingroup images
    \ingroup graphics
    \ingroup multimedia

    The TQMovie class, or TQLabel::setMovie(), are easy to use and for
    most situations do what you want with regards animated images.

    A TQImageConsumer consumes information about changes to the TQImage
    maintained by a TQImageDecoder. Think of the TQImage as the model or
    source of the image data, with the TQImageConsumer as a view of
    that data and the TQImageDecoder being the controller that
    orchestrates the relationship between the model and the view.

    You'd use the TQImageConsumer class, for example, if you were
    implementing a web browser with your own image loaders.

    \sa TQImageDecoder
*/

/*!
    \fn void TQImageConsumer::changed(const TQRect&)

    Called when the given area of the image has changed.
*/

/*!
    \fn void TQImageConsumer::end()

    Called when all the data from all the frames has been decoded and
    revealed as changed().
*/

/*!
    \fn void TQImageConsumer::frameDone()

    One of the two frameDone() functions will be called when a frame
    of an animated image has ended and been revealed as changed().

    When this function is called, the current image should be
    displayed.

    The decoder will not make any further changes to the image until
    the next call to TQImageFormat::decode().
*/

/*!
    \overload void TQImageConsumer::frameDone( const TQPoint& offset, const TQRect& rect )

    One of the two frameDone() functions will be called when a frame
    of an animated image has ended and been revealed as changed().

    When this function is called, the area \a rect in the current
    image should be moved by \a offset and displayed.

    The decoder will not make any further changes to the image until
    the next call to TQImageFormat::decode().
*/

/*!
    \fn void TQImageConsumer::setLooping(int n)

    Called to indicate that the sequence of frames in the image
    should be repeated \a n times, including the sequence during
    decoding.

    \list
    \i 0 = Forever
    \i 1 = Only display frames the first time through
    \i 2 = Repeat once after first pass through images
    \i etc.
    \endlist

    To make the TQImageDecoder do this, just delete it and pass the
    information to it again for decoding (setLooping() will be called
    again, of course, but that can be ignored), or keep copies of the
    changed areas at the ends of frames.
*/

/*!
    \fn void TQImageConsumer::setFramePeriod(int milliseconds)

    Notes that the frame about to be decoded should not be displayed
    until the given number of \a milliseconds after the time that this
    function is called. Of course, the image may not have been
    decoded by then, in which case the frame should not be displayed
    until it is complete. A value of -1 (the assumed default)
    indicates that the image should be displayed even while it is only
    partially loaded.
*/

/*!
    \fn void TQImageConsumer::setSize(int, int)

    This function is called as soon as the size of the image has been
    determined.
*/


/*!
    \class TQImageDecoder ntqasyncimageio.h
    \brief The TQImageDecoder class is an incremental image decoder for all supported image formats.

    \ingroup images
    \ingroup graphics
    \ingroup multimedia

    New formats are installed by creating objects of class
    TQImageFormatType; the TQMovie class can be used for all installed
    incremental image formats. TQImageDecoder is only useful for
    creating new ways of feeding data to an TQImageConsumer.

    A TQImageDecoder is a machine that decodes images. It takes encoded
    image data via its decode() method and expresses its decoding by
    supplying information to a TQImageConsumer. It implements its
    decoding by using a TQImageFormat created by one of the
    currently-existing TQImageFormatType factory objects.

    TQImageFormatType and TQImageFormat are the classes that you might
    need to implement support for additional image formats.

    \legalese

    TQt supports GIF reading if it is configured that way during
    installation (see ntqgif.h). If it is, we are required to state that
    "The Graphics Interchange Format(c) is the Copyright property of
    CompuServe Incorporated. GIF(sm) is a Service Mark property of
    CompuServe Incorporated."

    \warning If you are in a country that recognizes software patents
    and in which Unisys holds a patent on LZW compression and/or
    decompression and you want to use GIF, Unisys may require you to
    license that technology. Such countries include Canada, Japan,
    the USA, France, Germany, Italy and the UK.

    GIF support may be removed completely in a future version of TQt.
    We recommend using the MNG or PNG format.
*/

static const int max_header = 32;





// See ntqgif.h for important information regarding this option
#if defined(QT_BUILTIN_GIF_READER) && QT_BUILTIN_GIF_READER == 1
class TQGIFFormat : public TQImageFormat {
public:
    TQGIFFormat();
    virtual ~TQGIFFormat();

    int decode(TQImage& img, TQImageConsumer* consumer,
	    const uchar* buffer, int length);

private:
    void fillRect(TQImage&, int x, int y, int w, int h, TQRgb col);
    TQRgb color( uchar index ) const;

    // GIF specific stuff
    TQRgb* globalcmap;
    TQRgb* localcmap;
    TQImage backingstore;
    unsigned char hold[16];
    bool gif89;
    int count;
    int ccount;
    int expectcount;
    enum State {
	Header,
	LogicalScreenDescriptor,
	GlobalColorMap,
	LocalColorMap,
	Introducer,
	ImageDescriptor,
	TableImageLZWSize,
	ImageDataBlockSize,
	ImageDataBlock,
	ExtensionLabel,
	GraphicControlExtension,
	ApplicationExtension,
	NetscapeExtensionBlockSize,
	NetscapeExtensionBlock,
	SkipBlockSize,
	SkipBlock,
	Done,
	Error
    } state;
    int gncols;
    int lncols;
    int ncols;
    int lzwsize;
    bool lcmap;
    int swidth, sheight;
    int width, height;
    int left, top, right, bottom;
    enum Disposal { NoDisposal, DoNotChange, RestoreBackground, RestoreImage };
    Disposal disposal;
    bool disposed;
    int trans_index;
    bool gcmap;
    int bgcol;
    int interlace;
    int accum;
    int bitcount;

    enum { max_lzw_bits=12 }; // (poor-compiler's static const int)

    int code_size, clear_code, end_code, max_code_size, max_code;
    int firstcode, oldcode, incode;
    short table[2][1<< max_lzw_bits];
    short stack[(1<<(max_lzw_bits))*2];
    short *sp;
    bool needfirst;
    int x, y;
    int frame;
    bool out_of_bounds;
    bool digress;
    void nextY(TQImage& img, TQImageConsumer* consumer);
    void disposePrevious( TQImage& img, TQImageConsumer* consumer );
};

class TQGIFFormatType : public TQImageFormatType
{
    TQImageFormat* decoderFor(const uchar* buffer, int length);
    const char* formatName() const;
};

#endif


class TQImageDecoderPrivate
{
public:
    TQImageDecoderPrivate()
    {
	count = 0;
    }

    static void cleanup();

    static void ensureFactories()
    {
	if ( !factories ) {
	    factories = new TQPtrList<TQImageFormatType>;
// See ntqgif.h for important information regarding this option
#if defined(QT_BUILTIN_GIF_READER) && QT_BUILTIN_GIF_READER == 1
	    gif_decoder_factory = new TQGIFFormatType;
#endif
	    tqt_init_image_handlers();
	    tqAddPostRoutine( cleanup );
	}
    }

    static TQPtrList<TQImageFormatType> * factories;

// See ntqgif.h for important information regarding this option
#if defined(QT_BUILTIN_GIF_READER) && QT_BUILTIN_GIF_READER == 1
    static TQGIFFormatType * gif_decoder_factory;
#endif

    uchar header[max_header];
    int count;
};

TQPtrList<TQImageFormatType> * TQImageDecoderPrivate::factories = 0;
// See ntqgif.h for important information regarding this option
#if defined(QT_BUILTIN_GIF_READER) && QT_BUILTIN_GIF_READER == 1
TQGIFFormatType * TQImageDecoderPrivate::gif_decoder_factory = 0;
#endif


void TQImageDecoderPrivate::cleanup()
{
    delete factories;
    factories = 0;
// See ntqgif.h for important information regarding this option
#if defined(QT_BUILTIN_GIF_READER) && QT_BUILTIN_GIF_READER == 1
    delete gif_decoder_factory;
    gif_decoder_factory = 0;
#endif
}


/*!
    Constructs a TQImageDecoder that will send change information to
    the TQImageConsumer \a c.
*/
TQImageDecoder::TQImageDecoder(TQImageConsumer* c)
{
    tqt_init_image_handlers();
    d = new TQImageDecoderPrivate;
    TQ_CHECK_PTR(d);
    consumer = c;
    actual_decoder = 0;
}

/*!
    Destroys a TQImageDecoder. The image it built is destroyed. The
    decoder built by the factory for the file format is destroyed. The
    consumer for which it decoded the image is \e not destroyed.
*/
TQImageDecoder::~TQImageDecoder()
{
    delete d;
    delete actual_decoder;
}

/*!
    \fn const TQImage& TQImageDecoder::image()

    Returns the image currently being decoded.
*/

static bool plugins_loaded = false;

/*!
    Call this function to decode some data into image changes. The
    data in \a buffer will be decoded, sending change information to
    the TQImageConsumer of this TQImageDecoder until one of the change
    functions of the consumer returns false. The length of the data is
    given in \a length.

    Returns the number of bytes consumed: 0 if consumption is
    complete, and -1 if decoding fails due to invalid data.
*/
int TQImageDecoder::decode(const uchar* buffer, int length)
{
    if (!actual_decoder) {
	int i=0;

	while (i < length && d->count < max_header)
	    d->header[d->count++] = buffer[i++];

	TQImageDecoderPrivate::ensureFactories();

	for (TQImageFormatType* f = TQImageDecoderPrivate::factories->first();
	    f && !actual_decoder;
	    f = TQImageDecoderPrivate::factories->next())
	{
	    actual_decoder = f->decoderFor(d->header, d->count);
	}
	if ( !actual_decoder && !plugins_loaded) {
	    tqt_init_image_plugins();
	    plugins_loaded = true;

	    for (TQImageFormatType* f = TQImageDecoderPrivate::factories->first();
		f && !actual_decoder;
		f = TQImageDecoderPrivate::factories->next())
	    {
		actual_decoder = f->decoderFor(d->header, d->count);
	    }
	}

	if (!actual_decoder) {
	    if ( d->count < max_header ) {
		// not enough info yet
		return i;
	    } else {
		// failure - nothing matches max_header bytes
		return -1;
	    }
	}
    }
    return actual_decoder->decode(img, consumer, buffer, length);
}

/*!
    Returns a TQImageFormatType by name. This might be used when the
    user needs to force data to be interpreted as being in a certain
    format. \a name is one of the formats listed by
    TQImageDecoder::inputFormats(). Note that you will still need to
    supply decodable data to result->decoderFor() before you can begin
    decoding the data.
*/
TQImageFormatType* TQImageDecoder::format( const char* name )
{
    TQImageDecoderPrivate::ensureFactories();
    tqt_init_image_plugins();

    for (TQImageFormatType* f = TQImageDecoderPrivate::factories->first();
	f;
	f = TQImageDecoderPrivate::factories->next())
    {
	if ( tqstricmp(name,f->formatName())==0 )
	    return f;
    }
    return 0;
}

/*!
    Call this function to find the name of the format of the given
    header. The returned string is statically allocated. The function
    will look at the first \a length characters in the \a buffer.

    Returns 0 if the format is not recognized.
*/
const char* TQImageDecoder::formatName(const uchar* buffer, int length)
{
    TQImageDecoderPrivate::ensureFactories();

    const char* name = 0;
    for (TQImageFormatType* f = TQImageDecoderPrivate::factories->first();
	f && !name;
	f = TQImageDecoderPrivate::factories->next())
    {
	TQImageFormat *decoder = f->decoderFor(buffer, length);
	if (decoder) {
	    name = f->formatName();
	    delete decoder;
	}
    }
    if ( !name && !plugins_loaded) {
	tqt_init_image_plugins();
	plugins_loaded = true;
	for (TQImageFormatType* f = TQImageDecoderPrivate::factories->first();
	    f && !name;
	    f = TQImageDecoderPrivate::factories->next())
	{
	    TQImageFormat *decoder = f->decoderFor(buffer, length);
	    if (decoder) {
		name = f->formatName();
		delete decoder;
	    }
	}
    }

    return name;
}

/*!
    Returns a sorted list of formats for which asynchronous loading is
    supported.
*/
TQStrList TQImageDecoder::inputFormats()
{
    TQImageDecoderPrivate::ensureFactories();
    tqt_init_image_plugins();

    TQStrList result;

    for (TQImageFormatType* f = TQImageDecoderPrivate::factories->first();
	 f;
	 f = TQImageDecoderPrivate::factories->next())
    {
	if ( !result.contains(  f->formatName() ) ) {
	    result.inSort(  f->formatName() );
	}
    }

    return result;
}

/*!
    Registers the new TQImageFormatType \a f. This is not needed in
    application code because factories call this themselves.
*/
void TQImageDecoder::registerDecoderFactory(TQImageFormatType* f)
{
    TQImageDecoderPrivate::ensureFactories();

    TQImageDecoderPrivate::factories->insert(0,f);
}

/*!
    Unregisters the TQImageFormatType \a f. This is not needed in
    application code because factories call this themselves.
*/
void TQImageDecoder::unregisterDecoderFactory(TQImageFormatType* f)
{
    if ( !TQImageDecoderPrivate::factories )
	return;

    TQImageDecoderPrivate::factories->remove(f);
}

/*!
    \class TQImageFormat ntqasyncimageio.h
    \brief The TQImageFormat class is an incremental image decoder for a specific image format.

    \ingroup images
    \ingroup graphics
    \ingroup multimedia

    By making a derived class of TQImageFormatType, which in turn
    creates objects that are a subclass of TQImageFormat, you can add
    support for more incremental image formats, allowing such formats
    to be sources for a TQMovie or for the first frame of the image
    stream to be loaded as a TQImage or TQPixmap.

    Your new subclass must reimplement the decode() function in order
    to process your new format.

    New TQImageFormat objects are generated by new TQImageFormatType
    factories.
*/

/*!
  Destroys the object.

  \internal
  More importantly, destroys derived classes.
*/
TQImageFormat::~TQImageFormat()
{
}

/*!
    \fn int TQImageFormat::decode(TQImage& img, TQImageConsumer* consumer, const uchar* buffer, int length)

    New subclasses must reimplement this method.

    It should decode some or all of the bytes from \a buffer into \a
    img, calling the methods of \a consumer as the decoding proceeds
    to inform that consumer of changes to the image. The length of the
    data is given in \a length. The consumer may be 0, in which case
    the function should just process the data into \a img without
    telling any consumer about the changes. Note that the decoder must
    store enough state to be able to continue in subsequent calls to
    this method - this is the essence of the incremental image
    loading.

    The function should return without processing all the data if it
    reaches the end of a frame in the input.

    The function must return the number of bytes it has processed.
*/

/*!
    \class TQImageFormatType ntqasyncimageio.h
    \brief The TQImageFormatType class is a factory that makes TQImageFormat objects.

    \ingroup images
    \ingroup graphics
    \ingroup multimedia

    Whereas the TQImageIO class allows for \e complete loading of
    images, TQImageFormatType allows for \e incremental loading of
    images.

    New image file formats are installed by creating objects of
    derived classes of TQImageFormatType. They must implement
    decoderFor() and formatName().

    TQImageFormatType is a very simple class. Its only task is to
    recognize image data in some format and make a new object,
    subclassed from TQImageFormat, which can decode that format.

    The factories for formats built into TQt are automatically defined
    before any other factory is initialized. If two factories would
    recognize an image format, the factory created last will override
    the earlier one; you can thus override current and future built-in
    formats.
*/

/*!
    \fn virtual TQImageFormat* TQImageFormatType::decoderFor(const uchar* buffer, int length)

    Returns a decoder for decoding an image that starts with the bytes
    in \a buffer. The length of the data is given in \a length. This
    function should only return a decoder if it is certain that the
    decoder applies to data with the given header. Returns 0 if there
    is insufficient data in the header to make a positive
    identification or if the data is not recognized.
*/

/*!
    \fn virtual const char* TQImageFormatType::formatName() const

    Returns the name of the format supported by decoders from this
    factory. The string is statically allocated.
*/

/*!
    Constructs a factory. It automatically registers itself with
    TQImageDecoder.
*/
TQImageFormatType::TQImageFormatType()
{
    TQImageDecoder::registerDecoderFactory(this);
}

/*!
    Destroys a factory. It automatically unregisters itself from
    TQImageDecoder.
*/
TQImageFormatType::~TQImageFormatType()
{
    TQImageDecoder::unregisterDecoderFactory(this);
}


/*!
    Returns true if TQt was compiled with built-in GIF reading support;
    otherwise returns false.
*/
bool qt_builtin_gif_reader()
{
#if defined(QT_BUILTIN_GIF_READER)
    return QT_BUILTIN_GIF_READER == 1;
#else
    return 0;
#endif
}

// See ntqgif.h for important information regarding this option
#if defined(QT_BUILTIN_GIF_READER) && QT_BUILTIN_GIF_READER == 1

/* -- NOTDOC
  \class TQGIFFormat ntqasyncimageio.h
  \brief Incremental image decoder for GIF image format.

  \ingroup images
  \ingroup graphics

  This subclass of TQImageFormat decodes GIF format images,
  including animated GIFs. Internally in
*/

/*!
    Constructs a TQGIFFormat.
*/
TQGIFFormat::TQGIFFormat()
{
    globalcmap = 0;
    localcmap = 0;
    lncols = 0;
    gncols = 0;
    disposal = NoDisposal;
    out_of_bounds = false;
    disposed = true;
    frame = -1;
    state = Header;
    count = 0;
    lcmap = false;
}

/*!
    Destroys a TQGIFFormat.
*/
TQGIFFormat::~TQGIFFormat()
{
    if (globalcmap) delete[] globalcmap;
    if ( localcmap ) delete[] localcmap;
}


/* -- NOTDOC
  \class TQGIFFormatType ntqasyncimageio.h
  \brief Incremental image decoder for GIF image format.

  \ingroup images
  \ingroup graphics

  This subclass of TQImageFormatType recognizes GIF
  format images, creating a TQGIFFormat when required. An instance
  of this class is created automatically before any other factories,
  so you should have no need for such objects.
*/

TQImageFormat* TQGIFFormatType::decoderFor(
    const uchar* buffer, int length)
{
    if (length < 6) return 0;
    if (buffer[0]=='G'
     && buffer[1]=='I'
     && buffer[2]=='F'
     && buffer[3]=='8'
     && (buffer[4]=='9' || buffer[4]=='7')
     && buffer[5]=='a')
	return new TQGIFFormat;
    return 0;
}

const char* TQGIFFormatType::formatName() const
{
    return "GIF";
}


void TQGIFFormat::disposePrevious( TQImage& img, TQImageConsumer* consumer )
{
    if ( out_of_bounds ) // flush anything that survived
	consumer->changed(TQRect(0,0,swidth,sheight));

    // Handle disposal of previous image before processing next one

    if ( disposed ) return;

    int l = TQMIN(swidth-1,left);
    int r = TQMIN(swidth-1,right);
    int t = TQMIN(sheight-1,top);
    int b = TQMIN(sheight-1,bottom);

    switch (disposal) {
      case NoDisposal:
	break;
      case DoNotChange:
	break;
      case RestoreBackground:
	if (trans_index>=0) {
	    // Easy:  we use the transparent color
	    fillRect(img, l, t, r-l+1, b-t+1, Q_TRANSPARENT);
	} else if (bgcol>=0) {
	    // Easy:  we use the bgcol given
	    fillRect(img, l, t, r-l+1, b-t+1, color(bgcol));
	} else {
	    // Impossible:  We don't know of a bgcol - use pixel 0
	    TQRgb** line = (TQRgb **)img.jumpTable();
	    fillRect(img, l, t, r-l+1, b-t+1, line[0][0]);
	}
	if (consumer)
	    consumer->changed(TQRect(l, t, r-l+1, b-t+1));
	break;
      case RestoreImage: {
	if ( frame >= 0 ) {
	    TQRgb** line = (TQRgb **)img.jumpTable();
	    for (int ln=t; ln<=b; ln++) {
		memcpy(line[ln]+l,
		    backingstore.scanLine(ln-t),
		    (r-l+1)*sizeof(TQRgb) );
	    }
	    consumer->changed(TQRect(l, t, r-l+1, b-t+1));
	}
      }
    }
    disposal = NoDisposal; // Until an extension says otherwise.

    disposed = true;
}

/*!
    This function decodes some data into image changes.

    Returns the number of bytes consumed.
*/
int TQGIFFormat::decode(TQImage& img, TQImageConsumer* consumer,
	const uchar* buffer, int length)
{
    // We are required to state that
    //    "The Graphics Interchange Format(c) is the Copyright property of
    //    CompuServe Incorporated. GIF(sm) is a Service Mark property of
    //    CompuServe Incorporated."

#define LM(l, m) (((m)<<8)|l)
    digress = false;
    int initial = length;
    TQRgb** line = (TQRgb **)img.jumpTable();
    while (!digress && length) {
	length--;
	unsigned char ch=*buffer++;
	switch (state) {
	  case Header:
	    hold[count++]=ch;
	    if (count==6) {
		// Header
		gif89=(hold[3]!='8' || hold[4]!='7');
		state=LogicalScreenDescriptor;
		count=0;
	    }
	    break;
	  case LogicalScreenDescriptor:
	    hold[count++]=ch;
	    if (count==7) {
		// Logical Screen Descriptor
		swidth=LM(hold[0], hold[1]);
		sheight=LM(hold[2], hold[3]);
		gcmap=!!(hold[4]&0x80);
		//UNUSED: bpchan=(((hold[4]&0x70)>>3)+1);
		//UNUSED: gcmsortflag=!!(hold[4]&0x08);
		gncols=2<<(hold[4]&0x7);
		bgcol=(gcmap) ? hold[5] : -1;
		//aspect=hold[6] ? double(hold[6]+15)/64.0 : 1.0;

		trans_index = -1;
		count=0;
		ncols=gncols;
		if (gcmap) {
		    ccount=0;
		    state=GlobalColorMap;
		    globalcmap = new TQRgb[gncols+1]; // +1 for trans_index
		    globalcmap[gncols] = Q_TRANSPARENT;
		} else {
		    state=Introducer;
		}
	    }
	    break;
	  case GlobalColorMap: case LocalColorMap:
	    hold[count++]=ch;
	    if (count==3) {
		TQRgb rgb = tqRgb(hold[0], hold[1], hold[2]);
		if ( state == LocalColorMap ) {
		    if ( ccount < lncols )
			localcmap[ccount] =  rgb;
		} else {
		    globalcmap[ccount] = rgb;
		}
		if (++ccount >= ncols) {
		    if ( state == LocalColorMap )
			state=TableImageLZWSize;
		    else
			state=Introducer;
		}
		count=0;
	    }
	    break;
	  case Introducer:
	    hold[count++]=ch;
	    switch (ch) {
	      case ',':
		state=ImageDescriptor;
		break;
	      case '!':
		state=ExtensionLabel;
		break;
	      case ';':
		if (consumer) {
		    if ( out_of_bounds ) // flush anything that survived
			consumer->changed(TQRect(0,0,swidth,sheight));
		    consumer->end();
		}
		state=Done;
		break;
	      default:
		digress=true;
		// Unexpected Introducer - ignore block
		state=Error;
	    }
	    break;
	  case ImageDescriptor:
	    hold[count++]=ch;
	    if (count==10) {
		int newleft=LM(hold[1], hold[2]);
		int newtop=LM(hold[3], hold[4]);
		int newwidth=LM(hold[5], hold[6]);
		int newheight=LM(hold[7], hold[8]);

		// disbelieve ridiculous logical screen sizes,
		// unless the image frames are also large.
		if ( swidth/10 > TQMAX(newwidth,200) )
		    swidth = -1;
		if ( sheight/10 > TQMAX(newheight,200) )
		    sheight = -1;

		if ( swidth <= 0 )
		    swidth = newleft + newwidth;
		if ( sheight <= 0 )
		    sheight = newtop + newheight;

		if (img.isNull()) {
		    if (!img.create(swidth, sheight, 32)) {
			// Check if the attempt to create the image failed. If
			// it did, the image is broken and we should give up.
			state = Error;
			return -1;
		    }
		    memset( img.bits(), 0, img.numBytes() );
		    if (consumer) consumer->setSize(swidth, sheight);
		}
		img.setAlphaBuffer(trans_index >= 0);
		line = (TQRgb **)img.jumpTable();

		disposePrevious( img, consumer );
		disposed = false;

		left = newleft;
		top = newtop;
		width = newwidth;
		height = newheight;

		right=TQMAX( 0, TQMIN(left+width, swidth)-1);
		bottom=TQMAX(0, TQMIN(top+height, sheight)-1);
		lcmap=!!(hold[9]&0x80);
		interlace=!!(hold[9]&0x40);
		//bool lcmsortflag=!!(hold[9]&0x20);
		lncols=lcmap ? (2<<(hold[9]&0x7)) : 0;
		if (lncols) {
		    if ( localcmap )
			delete [] localcmap;
		    localcmap = new TQRgb[lncols+1];
		    localcmap[lncols] = Q_TRANSPARENT;
		    ncols = lncols;
		} else {
		    ncols = gncols;
		}
		frame++;
		if ( frame == 0 ) {
		    if ( left || top || width<swidth || height<sheight ) {
			// Not full-size image - erase with bg or transparent
			if ( trans_index >= 0 ) {
			    fillRect(img, 0, 0, swidth, sheight, color(trans_index));
			    if (consumer) consumer->changed(TQRect(0,0,swidth,sheight));
			} else if ( bgcol>=0 ) {
			    fillRect(img, 0, 0, swidth, sheight, color(bgcol));
			    if (consumer) consumer->changed(TQRect(0,0,swidth,sheight));
			}
		    }
		}

		if ( disposal == RestoreImage ) {
		    int l = TQMIN(swidth-1,left);
		    int r = TQMIN(swidth-1,right);
		    int t = TQMIN(sheight-1,top);
		    int b = TQMIN(sheight-1,bottom);
		    int w = r-l+1;
		    int h = b-t+1;

		    if (backingstore.width() < w
			|| backingstore.height() < h) {
			// We just use the backing store as a byte array
			if(!backingstore.create( TQMAX(backingstore.width(), w),
						 TQMAX(backingstore.height(), h),
						 32)) {
				state = Error;
				return -1;
			}
			memset( img.bits(), 0, img.numBytes() );
		    }
		    for (int ln=0; ln<h; ln++) {
			memcpy(backingstore.scanLine(ln),
			       line[t+ln]+l, w*sizeof(TQRgb));
		    }
		}

		count=0;
		if (lcmap) {
		    ccount=0;
		    state=LocalColorMap;
		} else {
		    state=TableImageLZWSize;
		}
		x = left;
		y = top;
		accum = 0;
		bitcount = 0;
		sp = stack;
                firstcode = oldcode = 0;
		needfirst = false;
		out_of_bounds = left>=swidth || y>=sheight;
	    }
	    break;
	  case TableImageLZWSize: {
	    lzwsize=ch;
	    if ( lzwsize > max_lzw_bits ) {
		state=Error;
	    } else {
		code_size=lzwsize+1;
		clear_code=1<<lzwsize;
		end_code=clear_code+1;
		max_code_size=2*clear_code;
		max_code=clear_code+2;
		int i;
		for (i=0; i<clear_code && i<(1<<max_lzw_bits); i++) {
		    table[0][i]=0;
		    table[1][i]=i;
		}
		for (i=clear_code; i<(1<<max_lzw_bits); i++) {
		    table[0][i]=table[1][i]=0;
		}
		state=ImageDataBlockSize;
	    }
	    count=0;
	    break;
	  } case ImageDataBlockSize:
	    expectcount=ch;
	    if (expectcount) {
		state=ImageDataBlock;
	    } else {
		if (consumer) {
		    consumer->frameDone();
		    digress = true;
		}

		state=Introducer;
	    }
	    break;
	  case ImageDataBlock:
	    count++;
	    accum|=(ch<<bitcount);
	    bitcount+=8;
	    while (bitcount>=code_size && state==ImageDataBlock) {
		int code=accum&((1<<code_size)-1);
		bitcount-=code_size;
		accum>>=code_size;

		if (code==clear_code) {
		    if (!needfirst) {
			int i;
			code_size=lzwsize+1;
			max_code_size=2*clear_code;
			max_code=clear_code+2;
			for (i=0; i<clear_code; i++) {
			    table[0][i]=0;
			    table[1][i]=i;
			}
			for (i=clear_code; i<(1<<max_lzw_bits); i++) {
			    table[0][i]=table[1][i]=0;
			}
		    }
		    needfirst=true;
		} else if (code==end_code) {
		    bitcount = -32768;
		    // Left the block end arrive
		} else {
		    if (needfirst) {
			firstcode=oldcode=code;
			if (!out_of_bounds && line && firstcode!=trans_index)
			    line[y][x] = color(firstcode);
			x++;
			if (x>=swidth) out_of_bounds = true;
			needfirst=false;
			if (x>=left+width) {
			    x=left;
                            out_of_bounds = left>=swidth || y>=sheight;
			    nextY(img,consumer);
			}
		    } else {
			incode=code;
			if (code>=max_code) {
			    *sp++=firstcode;
			    code=oldcode;
			}
			while (code>=clear_code) {
			    *sp++=table[1][code];
			    if (code==table[0][code]) {
				state=Error;
				break;
			    }
			    if (sp-stack>=(1<<(max_lzw_bits))*2) {
				state=Error;
				break;
			    }
			    code=table[0][code];
			}
			*sp++=firstcode=table[1][code];
			code=max_code;
			if (code<(1<<max_lzw_bits)) {
			    table[0][code]=oldcode;
			    table[1][code]=firstcode;
			    max_code++;
			    if ((max_code>=max_code_size)
			     && (max_code_size<(1<<max_lzw_bits)))
			    {
				max_code_size*=2;
				code_size++;
			    }
			}
			oldcode=incode;
			while (sp>stack) {
			    --sp;
			    if (!out_of_bounds && line && *sp!=trans_index)
				line[y][x] = color(*sp);
			    x++;
			    if (x>=swidth) out_of_bounds = true;
			    if (x>=left+width) {
				x=left;
                                out_of_bounds = left>=swidth || y>=sheight;
				nextY(img,consumer);
			    }
			}
		    }
		}
	    }
	    if (count==expectcount) {
		count=0;
		state=ImageDataBlockSize;
	    }
	    break;
	  case ExtensionLabel:
	    switch (ch) {
	    case 0xf9:
		state=GraphicControlExtension;
		break;
	    case 0xff:
		state=ApplicationExtension;
		break;
#if 0
	    case 0xfe:
		state=CommentExtension;
		break;
	    case 0x01:
		break;
#endif
	    default:
		state=SkipBlockSize;
	    }
	    count=0;
	    break;
	  case ApplicationExtension:
	    if (count<11) hold[count]=ch;
	    count++;
	    if (count==hold[0]+1) {
		if (tqstrncmp((char*)(hold+1), "NETSCAPE", 8)==0) {
		    // Looping extension
		    state=NetscapeExtensionBlockSize;
		} else {
		    state=SkipBlockSize;
		}
		count=0;
	    }
	    break;
	  case NetscapeExtensionBlockSize:
	    expectcount=ch;
	    count=0;
	    if (expectcount) state=NetscapeExtensionBlock;
	    else state=Introducer;
	    break;
	  case NetscapeExtensionBlock:
	    if (count<3) hold[count]=ch;
	    count++;
	    if (count==expectcount) {
		int loop = hold[0]+hold[1]*256;
		if (consumer) consumer->setLooping(loop);
		state=SkipBlockSize; // Ignore further blocks
	    }
	    break;
	  case GraphicControlExtension:
	    if (count<5) hold[count]=ch;
	    count++;
	    if (count==hold[0]+1) {
		disposePrevious( img, consumer );
		disposal=Disposal((hold[1]>>2)&0x7);
		//UNUSED: waitforuser=!!((hold[1]>>1)&0x1);
		int delay=count>3 ? LM(hold[2], hold[3]) : 1;
		// IE and mozilla use a minimum delay of 10. With the minumum delay of 10
		// we are compatible to them and avoid huge loads on the app and xserver.
		if ( delay < 10 )
		    delay = 10;

		bool havetrans=hold[1]&0x1;
 		trans_index = havetrans ? hold[4] : -1;

		if (consumer) consumer->setFramePeriod(delay*10);
		count=0;
		state=SkipBlockSize;
	    }
	    break;
	  case SkipBlockSize:
	    expectcount=ch;
	    count=0;
	    if (expectcount) state=SkipBlock;
	    else state=Introducer;
	    break;
	  case SkipBlock:
	    count++;
	    if (count==expectcount) state=SkipBlockSize;
	    break;
	  case Done:
	    digress=true;
	    /* Netscape ignores the junk, so we do too.
	    length++; // Unget
	    state=Error; // More calls to this is an error
	    */
	    break;
	  case Error:
	    return -1; // Called again after done.
	}
    }
    return initial-length;
}

void TQGIFFormat::fillRect(TQImage& img, int col, int row, int w, int h, TQRgb color)
{
    if (w>0) {
	TQRgb** line = (TQRgb **)img.jumpTable() + row;
	for (int j=0; j<h; j++) {
	    for ( int i=0; i<w; i++ ) {
		*(line[j]+col+i) = color;
	    }
	}
    }
}

void TQGIFFormat::nextY(TQImage& img, TQImageConsumer* consumer)
{
    if (out_of_bounds)
        return;
    int my;
    switch (interlace) {
      case 0:
	// Non-interlaced
	if (consumer && !out_of_bounds)
	    consumer->changed(TQRect(left, y, right-left+1, 1));
	y++;
	break;
      case 1:
	{
	    int i;
	    my = TQMIN(7, bottom-y);
	    if ( trans_index < 0 ) // Don't dup with transparency
		for (i=1; i<=my; i++)
		    memcpy(img.scanLine(y+i)+left, img.scanLine(y)+left,
			(right-left+1)*sizeof(TQRgb));
	    if (consumer && !out_of_bounds)
		consumer->changed(TQRect(left, y, right-left+1, my+1));
	    y+=8;
	    if (y>bottom) {
		interlace++; y=top+4;
		if (y > bottom) { // for really broken GIFs with bottom < 5
		    interlace=2;
		    y = top + 2;
		    if (y > bottom) { // for really broken GIF with bottom < 3
			interlace = 0;
			y = top + 1;
		    }
		}
	    }
	} break;
      case 2:
	{
	    int i;
	    my = TQMIN(3, bottom-y);
	    if ( trans_index < 0 ) // Don't dup with transparency
		for (i=1; i<=my; i++)
		    memcpy(img.scanLine(y+i)+left, img.scanLine(y)+left,
			(right-left+1)*sizeof(TQRgb));
	    if (consumer && !out_of_bounds)
		consumer->changed(TQRect(left, y, right-left+1, my+1));
	    y+=8;
	    if (y>bottom) {
		interlace++; y=top+2;
		if (y > bottom) { // for really broken GIF with bottom < 3
		    interlace = 3;
		    y = top + 1;
		}
	    }
	} break;
      case 3:
	{
	    int i;
	    my = TQMIN(1, bottom-y);
	    if ( trans_index < 0 ) // Don't dup with transparency
		for (i=1; i<=my; i++)
		    memcpy(img.scanLine(y+i)+left, img.scanLine(y)+left,
			(right-left+1)*sizeof(TQRgb));
	    if (consumer && !out_of_bounds)
		consumer->changed(TQRect(left, y, right-left+1, my+1));
	    y+=4;
	    if (y>bottom) { interlace++; y=top+1; }
	} break;
      case 4:
	if (consumer && !out_of_bounds)
	    consumer->changed(TQRect(left, y, right-left+1, 1));
	y+=2;
    }

    // Consume bogus extra lines
    if (y >= sheight) out_of_bounds=true; //y=bottom;
}

TQRgb TQGIFFormat::color( uchar index ) const
{
    if ( index == trans_index || index > ncols )
	return Q_TRANSPARENT;
    TQRgb *map = lcmap ? localcmap : globalcmap;
    return map ? map[index] : 0;
}



#endif // QT_BUILTIN_GIF_READER

#endif // TQT_NO_ASYNC_IMAGE_IO
