/*
* PNGR.CPP -- QImageIO read/write handlers for
*       the PNG graphic format using libpng.
*
*       Copyright (c) October 1998, Sirtaj Singh Kang.  Distributed under
*       the LGPL.
*
*       $Id: pngr.cpp,v 1.5.4.1 1999/07/06 09:35:34 garbanzo Exp $
*/

#ifdef HAVE_CONFIG_H
#include"config.h"
#endif

#ifdef HAVE_LIBPNG

#include<stdio.h>
#include<stdlib.h>
#include<qimage.h>
#include<qfile.h>

extern "C" {
#include<png.h>
}

void kimgio_png_read( QImageIO *io )
{
	// open png file

	QImage image;
	FILE *fp = fopen ( io->fileName(), "r" );
	int passes = 0;

	if( fp == 0 ) {
		tqDebug( "Couldn't open %s for reading.", io->fileName() );
		return;
	}

		
	// init png structures

	png_structp png_ptr = png_create_read_struct(  // image ptr
		PNG_LIBPNG_VER_STRING, 0, 0, 0 );

	if( png_ptr == 0 ) {
		fclose( fp );
		return;
	}

	png_infop png_info = png_create_info_struct( png_ptr ); // info ptr

	if( png_info == 0 ) {
		png_destroy_read_struct( &png_ptr, 0, 0 );
		fclose( fp );
		return;
	}

	png_infop png_end = png_create_info_struct( png_ptr );

	if( !png_end ) {
		png_destroy_read_struct( &png_ptr, &png_info, 0 );
		fclose( fp );
		return;
	}

	// error jump point

	if( setjmp( png_jmpbuf(png_ptr) ) ) {
		png_destroy_read_struct( &png_ptr, &png_info, &png_end );
		fclose( fp );
		return;
	}

	// read header
	png_init_io( png_ptr, fp );
	png_read_info( png_ptr, png_info );

	// transformations
	png_set_packing( png_ptr );
	png_set_strip_16( png_ptr );

	if( !(png_get_color_type(png_ptr, png_info) & PNG_COLOR_MASK_COLOR) ) {
		png_set_gray_to_rgb( png_ptr );
	}
	else {
		png_set_expand( png_ptr );
	}

	if( ! (png_get_color_type(png_ptr, png_info) & PNG_COLOR_MASK_ALPHA) ) {
		// [KDE1 Revival 2026] filler 字节与位置按 TQt3 内存约定设置：
		// TQt3 32bpp 在 little-endian 上为 B,G,R,0xFF（QRgb=0xFFRRGGBB）。
		// 原实现 filler(0, BEFORE) 得 X,R,G,B 字节流再 >>8 ——那是 Qt1 的
		// R,G,B,X 内存假设，在 TQt3 下红蓝对调（PNG 颜色错误的根因）。
		png_set_filler( png_ptr, 0xff, PNG_FILLER_AFTER );
	}

	// [KDE1 Revival 2026] 字节流按 TQt3 约定重排（与 tqt3 qpngio.cpp 同款）：
	// little-endian 上 png_set_bgr 把 libpng 的 R,G,B(A) 输出翻转成 B,G,R(A)；
	// big-endian 上 png_set_swap_alpha 把 RGBA 变 ARGB（0xAARRGGBB 的大端字节序）。
	if ( QImage::systemByteOrder() == QImage::BigEndian )
		png_set_swap_alpha( png_ptr );
	else
		png_set_bgr( png_ptr );

	passes = png_set_interlace_handling ( png_ptr );

	png_read_update_info( png_ptr, png_info );

	if ( png_get_color_type(png_ptr, png_info) != PNG_COLOR_TYPE_RGB_ALPHA ) {
		tqDebug( "Colortype %d is not rgb/alpha",
			png_get_color_type(png_ptr, png_info) );
	}

	if( png_get_bit_depth(png_ptr, png_info) != 8 ) {
		tqDebug( "Depth %d is not 8", png_get_bit_depth(png_ptr, png_info) );
	}

	// create image
	if ( !image.create( png_get_image_width(png_ptr, png_info), png_get_image_height(png_ptr, png_info), 32 ) ) {
		// out of memory
		tqWarning( "Out of memory creating QImage." );
		png_destroy_read_struct( &png_ptr, &png_info, &png_end );
		fclose( fp );
		return;
	}

	// read image
	for( ; passes; passes-- ) {
		for( unsigned row = 0; row < png_get_image_height(png_ptr, png_info); row++ ) {
			png_read_row( png_ptr, image.scanLine( row ), NULL );
		}
	}

	if ( png_get_color_type(png_ptr, png_info) == PNG_COLOR_TYPE_RGB_ALPHA ) {
		tqDebug( "Colortype %d is rgb/alpha",
			png_get_color_type(png_ptr, png_info) );
		image.setAlphaBuffer(true);
	}
	// [KDE1 Revival 2026] 删除原 >>8 移位后处理循环——filler+bgr 已把字节流
	// 直接排成 TQt3 约定的 B,G,R,0xFF，无需再按 Qt1 的 R,G,B,X 假设移位。

	png_read_end( png_ptr, png_info );

	io->setImage( image );
	io->setStatus( 0 );
	
	// clean up 
	png_destroy_read_struct( &png_ptr, &png_info, &png_end );
	fclose( fp );

	return;
}

#if (defined PNG_LIBPNG_VER) && (PNG_LIBPNG_VER >= 100)

void kimgio_png_write( QImageIO *iio )
{
	QIODevice *f = ( iio->ioDevice() );
	FILE *fp = 0;
	png_structp png_ptr;
	png_infop info_ptr;
        int colortype = 0;

	const QImage& image = iio->image();
	int w = image.width(), h = image.height();

	int numcolors = image.numColors();
	int depth = image.depth() == 1 ? 1 : 8;

	//tqDebug("Size:\t%d X %d\n\tColors:\t%d\n\tDepth:\t%d",
	//	w, h, numcolors, image.depth());

	if(numcolors > 0) {
		//tqDebug("PALETTE");
		colortype = PNG_COLOR_TYPE_PALETTE;
	}
	else if(image.hasAlphaBuffer()) {
		//tqDebug("RGB_ALPHA");
		colortype = PNG_COLOR_TYPE_RGB_ALPHA;
	}
	else {
		//tqDebug("RGB");
		colortype = PNG_COLOR_TYPE_RGB;
	}

	// open the file
	fp = fdopen(((QFile*)f)->handle(), "wb");
	if (fp == 0) {
		iio->setStatus( -1 );
		return;
	}

	/* Create and initialize the png_struct with the desired error handler
	 * functions.  If you want to use the default stderr and longjump method,
	 * you can supply NULL for the last three parameters.  We also check that
	 * the library version is compatible with the one used at compile time,
	 * in case we are using dynamically linked libraries.  REQUIRED.
	 */
	png_ptr = png_create_write_struct(PNG_LIBPNG_VER_STRING, 0, 0, 0);

	if (png_ptr == 0) {
		fclose(fp);
		iio->setStatus( -2 );
		return;
	}

	/* Allocate/initialize the image information data.  REQUIRED */
	info_ptr = png_create_info_struct(png_ptr);
	if (info_ptr == 0) {
		fclose(fp);
		png_destroy_write_struct(&png_ptr,  (png_infopp)NULL);
		iio->setStatus( -3 );
		return;
	}

	/* set up the output control if you are using standard C streams */
	png_init_io(png_ptr, fp);

	/* Set the image information here.  Width and height are up to 2^31,
	* bit_depth is one of 1, 2, 4, 8, or 16, but valid values also depend on
	* the color_type selected. color_type is one of PNG_COLOR_TYPE_GRAY,
	* PNG_COLOR_TYPE_GRAY_ALPHA, PNG_COLOR_TYPE_PALETTE, PNG_COLOR_TYPE_RGB,
	* or PNG_COLOR_TYPE_RGB_ALPHA.  interlace is either PNG_INTERLACE_NONE or
	* PNG_INTERLACE_ADAM7, and the compression_type and filter_type MUST
	* currently be PNG_COMPRESSION_TYPE_BASE and PNG_FILTER_TYPE_BASE. REQUIRED
	*/
	png_set_IHDR(png_ptr, info_ptr, w, h, depth, colortype,
		PNG_INTERLACE_NONE, PNG_COMPRESSION_TYPE_BASE, PNG_FILTER_TYPE_BASE);

	/* set the palette if there is one.  REQUIRED for indexed-color images */

	if(numcolors > 0) {
		// [KDE1 Revival 2026] 恢复被注释的调色板写出（原代码声明 PALETTE
		// 色彩类型却从不 set PLTE，libpng 必然报错，8bpp 图存 PNG 从未成功）。
		// png_color 通道经 qRed/qGreen/qBlue 值级取色，不涉字节序。
		png_colorp palette = (png_colorp)png_malloc(png_ptr,
			numcolors * sizeof(png_color));
		for(int i = 0; i < numcolors; i++) {
			palette[i].red = qRed(image.color(i));
			palette[i].green = qGreen(image.color(i));
			palette[i].blue = qBlue(image.color(i));
		}
		png_set_PLTE(png_ptr, info_ptr, palette, numcolors);
	}

	//optional significant bit chunk

    /*png_color_8p sig_bit;
    png_get_sBit(png_ptr, info_ptr, sig_bit);

	if(image.isGrayscale()) {
		sig_bit->gray = 8;
	}
	else {
		sig_bit->red = 8;
		sig_bit->green = 8;
		sig_bit->blue = 8;
	}

	if(image.hasAlphaBuffer())
		sig_bit->alpha = 8;

    png_set_sBit(png_ptr, info_ptr, &sig_bit);
    */

	// Optional gamma chunk is strongly suggested if you have any guess
	// as to the correct gamma of the image.
	//png_set_gAMA(png_ptr, info_ptr, gamma);

	// Optionally write comments into the image
	//text_ptr[0].key = "Title";
	//text_ptr[0].text = "Mona Lisa";
	//text_ptr[0].compression = PNG_TEXT_COMPRESSION_NONE;
	//text_ptr[1].key = "Author";
	//text_ptr[1].text = "Leonardo DaVinci";
	//text_ptr[1].compression = PNG_TEXT_COMPRESSION_NONE;
	//text_ptr[2].key = "Description";
	//text_ptr[2].text = "<long text>";
	//text_ptr[2].compression = PNG_TEXT_COMPRESSION_zTXt;
	//png_set_text(png_ptr, info_ptr, text_ptr, 2);

	// Write the file header information.  REQUIRED
	png_write_info(png_ptr, info_ptr);

	// Once we write out the header, the compression type on the text
	// chunks gets changed to PNG_TEXT_COMPRESSION_NONE_WR or
	// PNG_TEXT_COMPRESSION_zTXt_WR, so it doesn't get written out again
	// at the end.

	// pack pixels into bytes
	png_set_packing( png_ptr );
	png_set_strip_16( png_ptr );

	// swap location of alpha bytes from ARGB to RGBA 
	//png_set_swap_alpha(png_ptr);

	// Get rid of filler (OR ALPHA) bytes, pack XRGB/RGBX/ARGB/RGBA into
	// RGB (4 channels -> 3 channels). The second parameter is not used.
	// [KDE1 Revival 2026] 条件修正：32bpp 直色图像（无调色板、无 alpha）都需
	// 去 filler；原 depth==8 条件把 32bpp 也算了进去本无大碍，但调色板图像
	// 决不能进此分支（palette 无 filler 概念，libpng 会报错）。
	if ( colortype != PNG_COLOR_TYPE_PALETTE && !image.hasAlphaBuffer() )
		png_set_filler(png_ptr, 0,
		    QImage::systemByteOrder() == QImage::BigEndian ?
			PNG_FILLER_BEFORE : PNG_FILLER_AFTER);

	// flip BGR pixels to RGB
	// [KDE1 Revival 2026] 启用：TQt3 32bpp 内存序为 B,G,R(A)，little-endian
	// 上必须经 png_set_bgr 翻转回 R,G,B(A) 才是 PNG 文件格式——原注释掉后
	// 写出的 PNG 红蓝对调；big-endian 上 png_set_swap_alpha 把 ARGB 转 RGBA。
	if ( colortype != PNG_COLOR_TYPE_PALETTE ) {
	    if ( QImage::systemByteOrder() == QImage::BigEndian )
		png_set_swap_alpha(png_ptr);
	    else
		png_set_bgr(png_ptr);
	}

	// swap bytes of 16-bit files to most significant byte first
	//png_set_swap(png_ptr);

	// swap bits of 1, 2, 4 bit packed pixel formats
	//png_set_packswap(png_ptr);

	// The easiest way to write the image (you may have a different memory
	// layout, however, so choose what fits your needs best).  You need to
	// use the first method if you aren't handling interlacing yourself.

#define entire
	// One of the following output methods is REQUIRED 
#ifdef entire // write out the entire image data in one call

	png_byte **row_pointers = image.jumpTable();
	png_write_image(png_ptr, row_pointers);

#else // (no_entire) write out the image data by one or more scanlines
	// If you are only writing one row at a time, this works

	for (int y = 0; y < h; y++) {
		png_bytep row_pointer = image.scanLine(y);
		png_write_rows(png_ptr, row_pointer, 1);
	}
#endif // (no_entire) use only one output method

	// You can write optional chunks like tEXt, zTXt, and tIME at the end as well.

	// It is REQUIRED to call this to finish writing the rest of the file
	png_write_end(png_ptr, info_ptr);

	// if you malloced the palette, free it here
	//if(numcolors > 0)
	//	free(info_ptr->palette);

	// if you allocated any text comments, free them here

	// clean up after the write, and free any memory allocated
	png_destroy_write_struct(&png_ptr, (png_infopp)NULL);

	// close the file
	fclose(fp);

	iio->setStatus( 0 );

	return;
}
#else
	// png library is too old

void kimgio_png_write( QImageIO *iio )
{
        // TODO: implement this
        tqWarning("kimgio_png_write: not yet implemented for old PNG libraries");
}

#endif


#endif /* HAVE_LIBPNG */
