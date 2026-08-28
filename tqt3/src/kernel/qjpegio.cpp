/****************************************************************************
**
** Implementation of JPEG TQImage IOHandler
**
** Created : 990521
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

#include "ntqimage.h"

#ifndef TQT_NO_IMAGEIO_JPEG

#include "ntqiodevice.h"
#include "ntqjpegio.h"

#include <stdio.h>      // jpeglib needs this to be pre-included
#include <setjmp.h>


// including jpeglib.h seems to be a little messy
extern "C" {
#define XMD_H           // shut JPEGlib up
#if defined(Q_OS_UNIXWARE)
#  define HAVE_BOOLEAN  // libjpeg under Unixware seems to need this
#endif
#include <jpeglib.h>
#ifdef const
#  undef const          // remove crazy C hackery in jconfig.h
#endif
}


struct my_error_mgr : public jpeg_error_mgr {
    jmp_buf setjmp_buffer;
};

#if defined(Q_C_CALLBACKS)
extern "C" {
#endif

static
void my_error_exit (j_common_ptr cinfo)
{
    my_error_mgr* myerr = (my_error_mgr*) cinfo->err;
    char buffer[JMSG_LENGTH_MAX];
    (*cinfo->err->format_message)(cinfo, buffer);
    tqWarning("%s", buffer);
    longjmp(myerr->setjmp_buffer, 1);
}

#if defined(Q_C_CALLBACKS)
}
#endif


static const int max_buf = 4096;

struct my_jpeg_source_mgr : public jpeg_source_mgr {
    // Nothing dynamic - cannot rely on destruction over longjump
    TQImageIO* iio;
    JOCTET buffer[max_buf];

public:
    my_jpeg_source_mgr(TQImageIO* iio);
};

#if defined(Q_C_CALLBACKS)
extern "C" {
#endif

static
void tqt_init_source(j_decompress_ptr)
{
}

static
boolean tqt_fill_input_buffer(j_decompress_ptr cinfo)
{
    int num_read;
    my_jpeg_source_mgr* src = (my_jpeg_source_mgr*)cinfo->src;
    TQIODevice* dev = src->iio->ioDevice();
    src->next_input_byte = src->buffer;
    num_read = dev->readBlock((char*)src->buffer, max_buf);
    if ( num_read <= 0 ) {
	// Insert a fake EOI marker - as per jpeglib recommendation
	src->buffer[0] = (JOCTET) 0xFF;
	src->buffer[1] = (JOCTET) JPEG_EOI;
	src->bytes_in_buffer = 2;
    } else {
	src->bytes_in_buffer = num_read;
    }
#if defined(Q_OS_UNIXWARE)
    return B_TRUE;
#else
    return true;
#endif
}

static
void tqt_skip_input_data(j_decompress_ptr cinfo, long num_bytes)
{
    my_jpeg_source_mgr* src = (my_jpeg_source_mgr*)cinfo->src;

    // `dumb' implementation from jpeglib

    /* Just a dumb implementation for now.  Could use fseek() except
     * it doesn't work on pipes.  Not clear that being smart is worth
     * any trouble anyway --- large skips are infrequent.
     */
    if (num_bytes > 0) {
	while (num_bytes > (long) src->bytes_in_buffer) {
	    num_bytes -= (long) src->bytes_in_buffer;
	    (void) tqt_fill_input_buffer(cinfo);
	    /* note we assume that tqt_fill_input_buffer will never return false,
	    * so suspension need not be handled.
	    */
	}
	src->next_input_byte += (size_t) num_bytes;
	src->bytes_in_buffer -= (size_t) num_bytes;
    }
}

static
void tqt_term_source(j_decompress_ptr)
{
}

#if defined(Q_C_CALLBACKS)
}
#endif


inline my_jpeg_source_mgr::my_jpeg_source_mgr(TQImageIO* iioptr)
{
    jpeg_source_mgr::init_source = tqt_init_source;
    jpeg_source_mgr::fill_input_buffer = tqt_fill_input_buffer;
    jpeg_source_mgr::skip_input_data = tqt_skip_input_data;
    jpeg_source_mgr::resync_to_restart = jpeg_resync_to_restart;
    jpeg_source_mgr::term_source = tqt_term_source;
    iio = iioptr;
    bytes_in_buffer = 0;
    next_input_byte = buffer;
}


static
void scaleSize( int &reqW, int &reqH, int imgW, int imgH, TQImage::ScaleMode mode )
{
    if ( mode == TQImage::ScaleFree )
        return;
    int t1 = imgW * reqH;
    int t2 = reqW * imgH;
    if (( mode == TQImage::ScaleMin && (t1 > t2) ) || ( mode == TQImage::ScaleMax && (t1 < t2) ))
	reqH = t2 / imgW;
    else
        reqW = t1 / imgH;
}


static
void read_jpeg_image(TQImageIO* iio)
{
    TQImage image;

    struct jpeg_decompress_struct cinfo;

    struct my_jpeg_source_mgr *iod_src = new my_jpeg_source_mgr(iio);
    struct my_error_mgr jerr;

    cinfo.err = jpeg_std_error(&jerr);
    jerr.error_exit = my_error_exit;

    jpeg_create_decompress(&cinfo);

    cinfo.src = iod_src;

    if (!setjmp(jerr.setjmp_buffer)) {
#if defined(Q_OS_UNIXWARE)
	(void) jpeg_read_header(&cinfo, B_TRUE);
#else
	(void) jpeg_read_header(&cinfo, true);
#endif

	(void) jpeg_start_decompress(&cinfo);

	TQString params = iio->parameters();
	params.simplifyWhiteSpace();
	int sWidth = 0, sHeight = 0;
	char sModeStr[1024] = "";
	TQImage::ScaleMode sMode;

	if ( params.contains( "GetHeaderInformation" ) ) {

	    // Create TQImage's without allocating the data
	    if ( cinfo.output_components == 3 || cinfo.output_components == 4) {
		image = TQImage( NULL, cinfo.output_width, cinfo.output_height, 32, NULL, 0, TQImage::IgnoreEndian );
	    } else if ( cinfo.output_components == 1 ) {
		image = TQImage( NULL, cinfo.output_width, cinfo.output_height, 8, NULL, 0, TQImage::IgnoreEndian );
	    } else {
		// Unsupported format
	    }


	} else if ( params.contains( "Scale" ) ) {
	    sscanf( params.latin1(), "Scale( %i, %i, %1023s )",
		    &sWidth, &sHeight, sModeStr );

	    TQString sModeTQStr( sModeStr );
	    if ( sModeTQStr == "ScaleFree" ) {
		sMode = TQImage::ScaleFree;
	    } else if ( sModeTQStr == "ScaleMin" ) {
		sMode = TQImage::ScaleMin;
	    } else if ( sModeTQStr == "ScaleMax" ) {
		sMode = TQImage::ScaleMax;
	    } else {
		tqDebug("read_jpeg_image: invalid scale mode \"%s\", see TQImage::ScaleMode documentation", sModeStr);
		sMode = TQImage::ScaleFree;
	    }

//	    tqDebug( "Parameters ask to scale the image to %i x %i ScaleMode: %s", sWidth, sHeight, sModeStr );
	    scaleSize( sWidth, sHeight, cinfo.output_width, cinfo.output_height, sMode );
//	    tqDebug( "Scaling the jpeg to %i x %i", sWidth, sHeight, sModeStr );

	    bool created = false;
	    if ( cinfo.output_components == 3 || cinfo.output_components == 4) {
		created = image.create( sWidth, sHeight, 32 );
	    } else if ( cinfo.output_components == 1 ) {
		created = image.create( sWidth, sHeight, 8, 256 );
		for (int i=0; i<256; i++)
		    image.setColor(i, tqRgb(i,i,i));
	    } else {
		// Unsupported format
	    }
	    if (!created)
		image = TQImage();

	    if (!image.isNull()) {
		TQImage tmpImage( cinfo.output_width, 1, 32 );
		uchar** inLines = tmpImage.jumpTable();
		uchar** outLines = image.jumpTable();
		while (cinfo.output_scanline < cinfo.output_height) {
		    int outputLine = sHeight * cinfo.output_scanline / cinfo.output_height;
		    (void) jpeg_read_scanlines(&cinfo, inLines, 1);
		    if ( cinfo.output_components == 3 ) {
			uchar *in = inLines[0];
			TQRgb *out = (TQRgb*)outLines[outputLine];
			for (uint i=0; i<cinfo.output_width; i++ ) {
// ### Only scaling down an image works, I don't think scaling up will work at the moment
// ### An idea I have to make this a smooth scale is to progressively add the pixel values up
// When scaling down, multiple values are being over drawn in to the output buffer.
// Instead, a weighting based on the distance the line or pixel is from the output pixel determines
// the weight of it when added to the output buffer. At present it is a non-smooth scale which is
// inefficently implemented, it still uncompresses all the jpeg, an optimization for progressive
// jpegs could be made if scaling by say 50% or some other special cases
			    out[sWidth * i / cinfo.output_width] = tqRgb( in[0], in[1], in[2] );
			    in += 3;
			}
		    } else {
// ### Need to test the case where the jpeg is grayscale, need some black and white jpegs to test
// this code. (also only scales down and probably won't scale to a larger size)
			uchar *in = inLines[0];
			uchar *out = outLines[outputLine];
			for (uint i=0; i<cinfo.output_width; i++ ) {
			    out[sWidth * i / cinfo.output_width] = in[i];
			}
		    }
		}
		(void) jpeg_finish_decompress(&cinfo);
	    }

	} else {

	    bool created = false;
	    if ( cinfo.output_components == 3 || cinfo.output_components == 4) {
		created = image.create( cinfo.output_width, cinfo.output_height, 32 );
	    } else if ( cinfo.output_components == 1 ) {
		created = image.create( cinfo.output_width, cinfo.output_height, 8, 256 );
		for (int i=0; i<256; i++)
		    image.setColor(i, tqRgb(i,i,i));
	    } else {
		// Unsupported format
	    }
	    if (!created)
		image = TQImage();

	    if (!image.isNull()) {
		uchar** lines = image.jumpTable();
		while (cinfo.output_scanline < cinfo.output_height)
		    (void) jpeg_read_scanlines(&cinfo,
		                               lines + cinfo.output_scanline,
		                               cinfo.output_height);
		(void) jpeg_finish_decompress(&cinfo);

		if ( cinfo.output_components == 3 ) {
		    // Expand 24->32 bpp.
		    for (uint j=0; j<cinfo.output_height; j++) {
			uchar *in = image.scanLine(j) + cinfo.output_width * 3;
			TQRgb *out = (TQRgb*)image.scanLine(j);

			for (uint i=cinfo.output_width; i--; ) {
			    in-=3;
			    out[i] = tqRgb(in[0], in[1], in[2]);
			}
		    }
		}
	    }
        }

	if (!image.isNull()) {
	    if ( cinfo.density_unit == 1 ) {
	        image.setDotsPerMeterX( int(100. * cinfo.X_density / 2.54) );
	        image.setDotsPerMeterY( int(100. * cinfo.Y_density / 2.54) );
	    } else if ( cinfo.density_unit == 2 ) {
		image.setDotsPerMeterX( int(100. * cinfo.X_density) );
		image.setDotsPerMeterY( int(100. * cinfo.Y_density) );
	    }
	}

	iio->setImage(image);
	iio->setStatus(image.isNull());
    }

    jpeg_destroy_decompress(&cinfo);
    delete iod_src;
}


struct my_jpeg_destination_mgr : public jpeg_destination_mgr {
    // Nothing dynamic - cannot rely on destruction over longjump
    TQImageIO* iio;
    JOCTET buffer[max_buf];

public:
    my_jpeg_destination_mgr(TQImageIO*);
};


#if defined(Q_C_CALLBACKS)
extern "C" {
#endif

static
void tqt_init_destination(j_compress_ptr)
{
}

static
void tqt_exit_on_error(j_compress_ptr cinfo, TQIODevice* dev)
{
    if (dev->status() == IO_Ok) {
	return;
    } else {
	// cinfo->err->msg_code = JERR_FILE_WRITE;
	(*cinfo->err->error_exit)((j_common_ptr)cinfo);
    }
}

static
boolean tqt_empty_output_buffer(j_compress_ptr cinfo)
{
    my_jpeg_destination_mgr* dest = (my_jpeg_destination_mgr*)cinfo->dest;
    TQIODevice* dev = dest->iio->ioDevice();

    if ( dev->writeBlock( (char*)dest->buffer, max_buf ) != max_buf )
	tqt_exit_on_error(cinfo, dev);

    dest->next_output_byte = dest->buffer;
    dest->free_in_buffer = max_buf;

#if defined(Q_OS_UNIXWARE)
    return B_TRUE;
#else
    return true;
#endif
}

static
void tqt_term_destination(j_compress_ptr cinfo)
{
    my_jpeg_destination_mgr* dest = (my_jpeg_destination_mgr*)cinfo->dest;
    TQIODevice* dev = dest->iio->ioDevice();
    TQ_LONG n = max_buf - dest->free_in_buffer;

    if ( dev->writeBlock( (char*)dest->buffer, n ) != n )
	tqt_exit_on_error(cinfo, dev);

    dev->flush();

    tqt_exit_on_error(cinfo, dev);
}

#if defined(Q_C_CALLBACKS)
}
#endif


inline
my_jpeg_destination_mgr::my_jpeg_destination_mgr(TQImageIO* iioptr)
{
    jpeg_destination_mgr::init_destination = tqt_init_destination;
    jpeg_destination_mgr::empty_output_buffer = tqt_empty_output_buffer;
    jpeg_destination_mgr::term_destination = tqt_term_destination;
    iio = iioptr;
    next_output_byte = buffer;
    free_in_buffer = max_buf;
}


static
void write_jpeg_image(TQImageIO* iio)
{
    TQImage image = iio->image();

    struct jpeg_compress_struct cinfo;
    JSAMPROW row_pointer[1];
    row_pointer[0] = 0;

    struct my_jpeg_destination_mgr *iod_dest = new my_jpeg_destination_mgr(iio);
    struct my_error_mgr jerr;

    cinfo.err = jpeg_std_error(&jerr);

    jerr.error_exit = my_error_exit;

    if (!setjmp(jerr.setjmp_buffer)) {
	jpeg_create_compress(&cinfo);

	cinfo.dest = iod_dest;

	cinfo.image_width = image.width();
	cinfo.image_height = image.height();

	TQRgb* cmap=0;
	bool gray=false;
	switch ( image.depth() ) {
	  case 1:
	  case 8:
	    cmap = image.colorTable();
	    gray = true;
	    int i;
	    for (i=image.numColors(); gray && i--; ) {
		gray = gray & ( tqRed(cmap[i]) == tqGreen(cmap[i]) &&
				tqRed(cmap[i]) == tqBlue(cmap[i]) );
	    }
	    cinfo.input_components = gray ? 1 : 3;
	    cinfo.in_color_space = gray ? JCS_GRAYSCALE : JCS_RGB;
	    break;
	  case 32:
	    cinfo.input_components = 3;
	    cinfo.in_color_space = JCS_RGB;
	}

	jpeg_set_defaults(&cinfo);

	float diffInch = TQABS(image.dotsPerMeterX()*2.54/100. - tqRound(image.dotsPerMeterX()*2.54/100.))
	                 + TQABS(image.dotsPerMeterY()*2.54/100. - tqRound(image.dotsPerMeterY()*2.54/100.));
	float diffCm = (TQABS(image.dotsPerMeterX()/100. - tqRound(image.dotsPerMeterX()/100.))
	                + TQABS(image.dotsPerMeterY()/100. - tqRound(image.dotsPerMeterY()/100.)))*2.54;
	if (diffInch < diffCm) {
	    cinfo.density_unit = 1; // dots/inch
	    cinfo.X_density = tqRound(image.dotsPerMeterX()*2.54/100.);
	    cinfo.Y_density = tqRound(image.dotsPerMeterY()*2.54/100.);
	} else {
	    cinfo.density_unit = 2; // dots/cm
	    cinfo.X_density = (image.dotsPerMeterX()+50) / 100;
	    cinfo.Y_density = (image.dotsPerMeterY()+50) / 100;
	}

	int quality = iio->quality() >= 0 ? TQMIN(iio->quality(),100) : 75;
#if defined(Q_OS_UNIXWARE)
	jpeg_set_quality(&cinfo, quality, B_TRUE /* limit to baseline-JPEG values */);
	jpeg_start_compress(&cinfo, B_TRUE);
#else
	jpeg_set_quality(&cinfo, quality, true /* limit to baseline-JPEG values */);
	jpeg_start_compress(&cinfo, true);
#endif

	row_pointer[0] = new uchar[cinfo.image_width*cinfo.input_components];
	int w = cinfo.image_width;
	while (cinfo.next_scanline < cinfo.image_height) {
	    uchar *row = row_pointer[0];
	    switch ( image.depth() ) {
	      case 1:
		if (gray) {
		    uchar* data = image.scanLine(cinfo.next_scanline);
		    if ( image.bitOrder() == TQImage::LittleEndian ) {
			for (int i=0; i<w; i++) {
			    bool bit = !!(*(data + (i >> 3)) & (1 << (i & 7)));
			    row[i] = tqRed(cmap[bit]);
			}
		    } else {
			for (int i=0; i<w; i++) {
			    bool bit = !!(*(data + (i >> 3)) & (1 << (7 -(i & 7))));
			    row[i] = tqRed(cmap[bit]);
			}
		    }
		} else {
		    uchar* data = image.scanLine(cinfo.next_scanline);
		    if ( image.bitOrder() == TQImage::LittleEndian ) {
			for (int i=0; i<w; i++) {
			    bool bit = !!(*(data + (i >> 3)) & (1 << (i & 7)));
			    *row++ = tqRed(cmap[bit]);
			    *row++ = tqGreen(cmap[bit]);
			    *row++ = tqBlue(cmap[bit]);
			}
		    } else {
			for (int i=0; i<w; i++) {
			    bool bit = !!(*(data + (i >> 3)) & (1 << (7 -(i & 7))));
			    *row++ = tqRed(cmap[bit]);
			    *row++ = tqGreen(cmap[bit]);
			    *row++ = tqBlue(cmap[bit]);
			}
		    }
		}
		break;
	      case 8:
		if (gray) {
		    uchar* pix = image.scanLine(cinfo.next_scanline);
		    for (int i=0; i<w; i++) {
			*row = tqRed(cmap[*pix]);
			++row; ++pix;
		    }
		} else {
		    uchar* pix = image.scanLine(cinfo.next_scanline);
		    for (int i=0; i<w; i++) {
			*row++ = tqRed(cmap[*pix]);
			*row++ = tqGreen(cmap[*pix]);
			*row++ = tqBlue(cmap[*pix]);
			++pix;
		    }
		}
		break;
	      case 32: {
		TQRgb* rgb = (TQRgb*)image.scanLine(cinfo.next_scanline);
		for (int i=0; i<w; i++) {
		    *row++ = tqRed(*rgb);
		    *row++ = tqGreen(*rgb);
		    *row++ = tqBlue(*rgb);
		    ++rgb;
		}
	      }
	    }
	    jpeg_write_scanlines(&cinfo, row_pointer, 1);
	}

	jpeg_finish_compress(&cinfo);
	jpeg_destroy_compress(&cinfo);

	iio->setStatus(0);
    }

    delete iod_dest;
    delete [] row_pointer[0];
}

void qInitJpegIO()
{
    // Not much to go on - just 3 bytes: 0xFF, M_SOI, 0xFF
    // Even the third is not strictly specified as required.
    TQImageIO::defineIOHandler("JPEG", "^\377\330\377", 0, read_jpeg_image, write_jpeg_image);
}

#endif
