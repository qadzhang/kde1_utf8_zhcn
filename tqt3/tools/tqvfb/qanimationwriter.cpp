/**********************************************************************
** Copyright (C) 1992-2008 Trolltech ASA.  All rights reserved.
**
** This file is part of TQt/Embedded virtual framebuffer.
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

#include "qanimationwriter.h"

#include <ntqfile.h>

#include <png.h>
#include <netinet/in.h> // for htonl

class TQAnimationWriterData {
public:
    TQAnimationWriterData(TQIODevice* d) : dev(d)
    {
	framerate = 1000;
    }
    void setFrameRate(int d) { framerate = d; }
    virtual ~TQAnimationWriterData() { }
    virtual void setImage(const TQImage& src)=0;
    virtual bool canCompose() const { return false; }
    virtual void composeImage(const TQImage&, const TQPoint& ) { }

protected:
    int framerate;
    TQIODevice* dev;
};


class TQAnimationWriterMNG : public TQAnimationWriterData {
    bool first;
    png_structp png_ptr;
    png_infop info_ptr;
public:
    TQAnimationWriterMNG(TQIODevice* d) : TQAnimationWriterData(d)
    {
	first = true;
	begin_png();
    }

    ~TQAnimationWriterMNG()
    {
	if ( first ) {
	    // Eh? Not images.
	    TQImage dummy(1,1,32);
	    setImage(dummy);
	}
	writeMEND();
	end_png();
    }

    void begin_png()
    {
	png_ptr = png_create_write_struct(PNG_LIBPNG_VER_STRING,0,0,0);
	info_ptr = png_create_info_struct(png_ptr);
	png_set_compression_level(png_ptr,9);
	png_set_write_fn(png_ptr, (void*)this, write, 0);
    }

    void end_png()
    {
	png_destroy_write_struct(&png_ptr, &info_ptr);
    }


    static void write( png_structp png_ptr, png_bytep data, png_size_t length)
    {
	TQAnimationWriterMNG* that = (TQAnimationWriterMNG*)png_get_io_ptr(png_ptr);
	/*uint nw =*/ that->dev->writeBlock((const char*)data,length);
    }

    void writePNG(const TQImage& image)
    {
#if PNG_LIBPNG_VER_MAJOR>1 || ( PNG_LIBPNG_VER_MAJOR==1 && PNG_LIBPNG_VER_MINOR>=5 )
#warning XXXtnn not too sure about this
/* 
according to png.h, channels is only used on read, not writes, so we
should be able to comment this out.
*/
#else /* LIBPNG 1.5 */
	info_ptr->channels = 4;
#endif
	png_set_sig_bytes(png_ptr, 8); // Pretend we already wrote the sig
	png_set_IHDR(png_ptr, info_ptr, image.width(), image.height(),
	    8, image.hasAlphaBuffer()
		    ? PNG_COLOR_TYPE_RGB_ALPHA : PNG_COLOR_TYPE_RGB,
	    0, 0, 0);
	png_write_info(png_ptr, info_ptr);
	if ( !image.hasAlphaBuffer() )
	    png_set_filler(png_ptr, 0,
		TQImage::systemByteOrder() == TQImage::BigEndian ?
		    PNG_FILLER_BEFORE : PNG_FILLER_AFTER);
	//if ( TQImage::systemByteOrder() == TQImage::BigEndian ) {
            //png_set_swap_alpha(png_ptr);
        //}
	if ( TQImage::systemByteOrder() == TQImage::LittleEndian ) {
	    png_set_bgr(png_ptr);
	}

	png_bytep* row_pointers;
	uint height = image.height();
	uchar** jt = image.jumpTable();
	row_pointers=new png_bytep[height];
	uint y;
	for (y=0; y<height; y++) {
		row_pointers[y]=jt[y];
	}
	png_write_image(png_ptr, row_pointers);
	delete [] row_pointers;
	png_write_end(png_ptr, info_ptr);
	end_png();
	begin_png();
    }

    void writeMHDR( const TQSize& size, int framerate )
    {
	dev->writeBlock("\212MNG\r\n\032\n", 8);

	struct {
	    int width;
	    int height;
	    int framerate;
	    int a,b,c;
	    int profile;
	} chunk;
	chunk.width = htonl(size.width());
	chunk.height = htonl(size.height());
	chunk.framerate = htonl(framerate);
	chunk.a=0;
	chunk.b=0;
	chunk.c=0;
	chunk.profile= htonl(0x00000003);

	png_write_chunk(png_ptr, (png_byte*)"MHDR", (png_byte*)&chunk, sizeof(chunk));
    }

    void writeMEND()
    {
	png_write_chunk(png_ptr, (png_byte*)"MEND", 0, 0);
    }

    void writeDEFI( const TQPoint& offset, const TQSize& size )
    {
	struct {
	    ushort o;
	    uchar s;
	    uchar concrete;
	    int x,y;
	    int lc,rc,tc,bc;
	} chunk;
	chunk.o=0;
	chunk.s=0;
	chunk.concrete=1;
	chunk.x=htonl(offset.x());
	chunk.y=htonl(offset.y());
	chunk.lc=0;
	chunk.rc=0;
	chunk.tc=htonl(INT_MAX);
	chunk.bc=htonl(INT_MAX);

	png_write_chunk(png_ptr, (png_byte*)"DEFI", (png_byte*)&chunk, sizeof(chunk));
    }

    void writeFRAM( const TQSize& size )
    {
	struct {
	    uchar mode;
	    uchar n;
	    uchar nu;
	    uchar d;
	    uchar t;
	    uchar clip;
	    uchar s;
	    uchar deltatype;
	    uint left;
	    uint right;
	    uint top;
	    uint bottom;
	} chunk;
	chunk.mode=1;
	chunk.n='a';
	chunk.nu=0;
	chunk.d=0;
	chunk.clip=1;
	chunk.t=0;
	chunk.s=0;
	chunk.deltatype=0;
	chunk.left=0;
	chunk.right=htonl(size.width());
	chunk.top=0;
	chunk.bottom=htonl(size.height());

	png_write_chunk(png_ptr, (png_byte*)"FRAM", (png_byte*)&chunk, sizeof(chunk));
    }

    void writeMOVE( const TQPoint& offset )
    {
	struct {
	    uchar filler[3];
	    uchar z[5];
	    int x,y;
	} chunk;
	memset(chunk.z,0,5);
	chunk.x=htonl(offset.x());
	chunk.y=htonl(offset.y());

	png_write_chunk(png_ptr, (png_byte*)"MOVE", ((png_byte*)&chunk)+3, sizeof(chunk)-3);
    }

    void setImage(const TQImage& src)
    {
	if ( first ) {
	    first = false;
	    writeMHDR(src.size(),framerate);
	}
	composeImage(src,TQPoint(0,0));
    }

    bool canCompose() const { return true; }

    void composeImage(const TQImage& src, const TQPoint& offset)
    {
	writeMOVE(offset);
	//writeFRAM(src.size());
	writePNG(src);
    }
};

TQAnimationWriter::TQAnimationWriter( const TQString& filename, const char* format )
{
    if ( TQCString(format) != "MNG" ) {
	tqWarning("Format \"%s\" not supported, only MNG", format);
	dev = 0;
	d = 0;
    } else {
	TQFile *f = new TQFile(filename);
	f->open(IO_WriteOnly);
	dev = f;
	d = new TQAnimationWriterMNG(dev);
    }
}

bool TQAnimationWriter::okay() const
{
    return dev && dev->status() == IO_Ok;
}

TQAnimationWriter::~TQAnimationWriter()
{
    delete d;
    delete dev;
}

void TQAnimationWriter::setFrameRate(int r)
{
    if (d) d->setFrameRate(r);
}

void TQAnimationWriter::appendFrame(const TQImage& frm, const TQPoint& offset)
{
    TQImage frame = frm.convertDepth(32);
    const int alignx = 1;
    if ( dev ) {
	if ( prev.isNull() || !d->canCompose() ) {
	    d->setImage(frame);
	} else {
	    bool done;
	    int minx, maxx, miny, maxy;
	    int w = frame.width();
	    int h = frame.height();

	    TQRgb** jt = (TQRgb**)frame.jumpTable();
	    TQRgb** pjt = (TQRgb**)prev.jumpTable() + offset.y();

	    // Find left edge of change
	    done = false;
	    for (minx = 0; minx < w && !done; minx++) {
		for (int ty = 0; ty < h; ty++) {
		    if ( jt[ty][minx] != pjt[ty][minx+offset.x()] ) {
			done = true;
			break;
		    }
		}
	    }
	    minx--;

	    // Find right edge of change
	    done = false;
	    for (maxx = w-1; maxx >= 0 && !done; maxx--) {
		for (int ty = 0; ty < h; ty++) {
		    if ( jt[ty][maxx] != pjt[ty][maxx+offset.x()] ) {
			done = true;
			break;
		    }
		}
	    }
	    maxx++;

	    // Find top edge of change
	    done = false;
	    for (miny = 0; miny < h && !done; miny++) {
		for (int tx = 0; tx < w; tx++) {
		    if ( jt[miny][tx] != pjt[miny][tx+offset.x()] ) {
			done = true;
			break;
		    }
		}
	    }
	    miny--;

	    // Find right edge of change
	    done = false;
	    for (maxy = h-1; maxy >= 0 && !done; maxy--) {
		for (int tx = 0; tx < w; tx++) {
		    if ( jt[maxy][tx] != pjt[maxy][tx+offset.x()] ) {
			done = true;
			break;
		    }
		}
	    }
	    maxy++;

	    if ( minx > maxx ) minx=maxx=0;
	    if ( miny > maxy ) miny=maxy=0;

	    if ( alignx > 1 ) {
		minx -= minx % alignx;
		maxx = maxx - maxx % alignx + alignx - 1;
	    }

	    int dw = maxx-minx+1;
	    int dh = maxy-miny+1;

	    TQImage diff(dw, dh, 32);

	    diff.setAlphaBuffer(true);
	    int x, y;
	    for (y = 0; y < dh; y++) {
		TQRgb* li = (TQRgb*)frame.scanLine(y+miny)+minx;
		TQRgb* lp = (TQRgb*)prev.scanLine(y+miny+offset.y())+minx+offset.x();
		TQRgb* ld = (TQRgb*)diff.scanLine(y);
		if ( alignx ) {
		    for (x = 0; x < dw; x+=alignx) {
			int i;
			for (i=0; i<alignx; i++) {
			    if ( li[x+i] != lp[x+i] )
				break;
			}
			if ( i == alignx ) {
			    // All the same
			    for (i=0; i<alignx; i++) {
				ld[x+i] = tqRgba(0,0,0,0);
			    }
			} else {
			    // Some different
			    for (i=0; i<alignx; i++) {
				ld[x+i] = 0xff000000 | li[x+i];
			    }
			}
		    }
		} else {
		    for (x = 0; x < dw; x++) {
			if ( li[x] != lp[x] )
			    ld[x] = 0xff000000 | li[x];
			else
			    ld[x] = tqRgba(0,0,0,0);
		    }
		}
	    }
tqDebug("%d,%d  %d,%d",minx,miny,offset.x(),offset.y());
	    d->composeImage(diff,TQPoint(minx,miny)+offset);
	}
	if ( prev.isNull() || ( prev.size() == frame.size() && offset == TQPoint(0,0) ) ) {
	    prev = frame;
	} else {
	    bitBlt(&prev,offset.x(),offset.y(),&frame,0,0,frame.width(),frame.height());
	}
    }
}

void TQAnimationWriter::appendFrame(const TQImage& frm)
{
    appendFrame(frm,TQPoint(0,0));
}

void TQAnimationWriter::appendBlankFrame()
{
    TQImage i(1,1,32);
    i.setAlphaBuffer(true);
    i.fill(0);
    d->composeImage(i,TQPoint(0,0));
}
