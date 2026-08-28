/**********************************************************************
** Copyright (C) 2000-2008 Trolltech ASA.  All rights reserved.
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

#include "uic.h"
#include <ntqfile.h>
#include <ntqimage.h>
#include <ntqstringlist.h>
#include <ntqdatetime.h>
#include <ntqfileinfo.h>
#include <stdio.h>
#include <ctype.h>

// on embedded, we do not compress image data. Rationale: by mapping
// the ready-only data directly into memory we are both faster and
// more memory efficient
#if defined(TQ_WS_QWS) && !defined(TQT_NO_IMAGE_COLLECTION_COMPRESSION)
#define TQT_NO_IMAGE_COLLECTION_COMPRESSION
#endif

struct EmbedImage
{
    ~EmbedImage() { delete[] colorTable; }
    int width, height, depth;
    int numColors;
    TQRgb* colorTable;
    TQString name;
    TQString cname;
    bool alpha;
#ifndef TQT_NO_IMAGE_COLLECTION_COMPRESSION
    ulong compressed;
#endif
};

static TQString convertToCIdentifier( const char *s )
{
    TQString r = s;
    int len = r.length();
    if ( len > 0 && !isalpha( (char)r[0].latin1() ) )
	r[0] = '_';
    for ( int i=1; i<len; i++ ) {
	if ( !isalnum( (char)r[i].latin1() ) )
	    r[i] = '_';
    }
    return r;
}


static ulong embedData( TQTextStream& out, const uchar* input, int nbytes )
{
#ifndef TQT_NO_IMAGE_COLLECTION_COMPRESSION
    TQByteArray bazip( tqCompress( input, nbytes ) );
    ulong len = bazip.size();
#else
    ulong len = nbytes;
#endif
    static const char hexdigits[] = "0123456789abcdef";
    TQString s;
    for ( int i=0; i<(int)len; i++ ) {
	if ( (i%14) == 0 ) {
	    s += "\n    ";
	    out << (const char*)s;
	    s.truncate( 0 );
	}
	uint v = (uchar)
#ifndef TQT_NO_IMAGE_COLLECTION_COMPRESSION
		 bazip
#else
		 input
#endif
		 [i];
	s += "0x";
	s += hexdigits[(v >> 4) & 15];
	s += hexdigits[v & 15];
	if ( i < (int)len-1 )
	    s += ',';
    }
    if ( s.length() )
	out << (const char*)s;
    return len;
}

static void embedData( TQTextStream& out, const TQRgb* input, int n )
{
    out << hex;
    const TQRgb *v = input;
    for ( int i=0; i<n; i++ ) {
	if ( (i%14) == 0  )
	    out << "\n    ";
	out << "0x";
	out << hex << *v++;
	if ( i < n-1 )
	    out << ',';
    }
    out << dec; // back to decimal mode
}

void Uic::embed( TQTextStream& out, const char* project, const TQStringList& images )
{

    TQString cProject = convertToCIdentifier( project );

    TQStringList::ConstIterator it;
    out << "/****************************************************************************\n";
    out << "** Image collection for project '" << project << "'.\n";
    out << "**\n";
    out << "** Generated from reading image files: \n";
    for ( it = images.begin(); it != images.end(); ++it )
	out << "**      " << *it << "\n";
    out << "**\n";
    out << "** Created by: The TQt user interface compiler (TQt " << TQT_VERSION_STR << ")\n";
    out << "**\n";
    out << "** WARNING! All changes made in this file will be lost!\n";
    out << "****************************************************************************/\n";
    out << "\n";

    out << "#include <ntqimage.h>\n";
    out << "#include <ntqdict.h>\n";
    out << "#include <ntqmime.h>\n";
    out << "#include <ntqdragobject.h>\n";
    out << "\n";

    TQPtrList<EmbedImage> list_image;
    list_image.setAutoDelete( true );
    int image_count = 0;
    for ( it = images.begin(); it != images.end(); ++it ) {
	TQImage img;
	if ( !img.load( *it ) ) {
	    fprintf( stderr, "uic: cannot load image file %s\n", (*it).latin1() );
	    continue;
	}
	EmbedImage *e = new EmbedImage;
	e->width = img.width();
	e->height = img.height();
	e->depth = img.depth();
	e->numColors = img.numColors();
	e->colorTable = new TQRgb[e->numColors];
	e->alpha = img.hasAlphaBuffer();
	memcpy(e->colorTable, img.colorTable(), e->numColors*sizeof(TQRgb));
	TQFileInfo fi( *it );
	e->name = fi.fileName();
	e->cname = TQString("image_%1").arg( image_count++);
	list_image.append( e );
	out << "// " << *it << "\n";
	TQString s;
	if ( e->depth == 1 )
	    img = img.convertBitOrder(TQImage::BigEndian);
	out << s.sprintf( "static const unsigned char %s_data[] = {",
			  (const char *)e->cname );
#ifndef TQT_NO_IMAGE_COLLECTION_COMPRESSION
	e->compressed =
#endif
	    embedData( out, img.bits(), img.numBytes() );
	out << "\n};\n\n";
	if ( e->numColors ) {
	    out << s.sprintf( "static const TQRgb %s_ctable[] = {",
			      (const char *)e->cname );
	    embedData( out, e->colorTable, e->numColors );
	    out << "\n};\n\n";
	}
    }

    if ( !list_image.isEmpty() ) {
	out << "static struct EmbedImage {\n"
	    "    int width, height, depth;\n"
	    "    const unsigned char *data;\n"
#ifndef TQT_NO_IMAGE_COLLECTION_COMPRESSION
	    "    ulong compressed;\n"
#endif
	    "    int numColors;\n"
	    "    const TQRgb *colorTable;\n"
	    "    bool alpha;\n"
	    "    const char *name;\n"
	    "} embed_image_vec[] = {\n";
	EmbedImage *e = list_image.first();
	while ( e ) {
	    out << "    { "
		<< e->width << ", "
		<< e->height << ", "
		<< e->depth << ", "
		<< "(const unsigned char*)" << e->cname << "_data, "
#ifndef TQT_NO_IMAGE_COLLECTION_COMPRESSION
		<< e->compressed << ", "
#endif
		<< e->numColors << ", ";
	    if ( e->numColors )
		out << e->cname << "_ctable, ";
	    else
		out << "0, ";
	    if ( e->alpha )
		out << "true, ";
	    else
		out << "false, ";
	    out << "\"" << e->name << "\" },\n";
	    e = list_image.next();
	}
#ifndef TQT_NO_IMAGE_COLLECTION_COMPRESSION
	out << "    { 0, 0, 0, 0, 0, 0, 0, 0, 0 }\n};\n";
#else
	out << "    { 0, 0, 0, 0, 0, 0, 0, 0 }\n};\n";
#endif

	out << "\n"
	    "static TQImage uic_findImage( const TQString& name )\n"
	    "{\n"
	    "    for ( int i=0; embed_image_vec[i].data; i++ ) {\n"
	    "	if ( TQString::fromUtf8(embed_image_vec[i].name) == name ) {\n"
#ifndef TQT_NO_IMAGE_COLLECTION_COMPRESSION
	    "	    TQByteArray baunzip;\n"
	    "	    baunzip = tqUncompress( embed_image_vec[i].data, \n"
	    "		embed_image_vec[i].compressed );\n"
	    "	    TQImage img((uchar*)baunzip.data(),\n"
	    "			embed_image_vec[i].width,\n"
	    "			embed_image_vec[i].height,\n"
	    "			embed_image_vec[i].depth,\n"
	    "			(TQRgb*)embed_image_vec[i].colorTable,\n"
	    "			embed_image_vec[i].numColors,\n"
	    "			TQImage::BigEndian\n"
	    "		);\n"
	    "	    img = img.copy();\n"
#else
	    "	    TQImage img((uchar*)embed_image_vec[i].data,\n"
	    "			embed_image_vec[i].width,\n"
	    "			embed_image_vec[i].height,\n"
	    "			embed_image_vec[i].depth,\n"
	    "			(TQRgb*)embed_image_vec[i].colorTable,\n"
	    "			embed_image_vec[i].numColors,\n"
	    "			TQImage::BigEndian\n"
	    "		);\n"
#endif
	    "	    if ( embed_image_vec[i].alpha )\n"
	    "		img.setAlphaBuffer(true);\n"
	    "	    return img;\n"
	    "        }\n"
	    "    }\n"
	    "    return TQImage();\n"
	    "}\n\n";

	out << "class MimeSourceFactory_" << cProject << " : public TQMimeSourceFactory\n";
	out << "{\n";
	out << "public:\n";
	out << "    MimeSourceFactory_" << cProject << "() {}\n";
	out << "    ~MimeSourceFactory_" << cProject << "() {}\n";
	out << "    const TQMimeSource* data( const TQString& abs_name ) const {\n";
	out << "\tconst TQMimeSource* d = TQMimeSourceFactory::data( abs_name );\n";
	out << "\tif ( d || abs_name.isNull() ) return d;\n";
	out << "\tTQImage img = uic_findImage( abs_name );\n";
	out << "\tif ( !img.isNull() )\n";
	out << "\t    ((TQMimeSourceFactory*)this)->setImage( abs_name, img );\n";
	out << "\treturn TQMimeSourceFactory::data( abs_name );\n";
	out << "    };\n";
	out << "};\n\n";

	out << "static TQMimeSourceFactory* factory = 0;\n";
	out << "\n";

	out << "TQ_EXPORT void qInitImages_" << cProject << "()\n";
	out << "{\n";
	out << "    if ( !factory ) {\n";
	out << "\tfactory = new MimeSourceFactory_" << cProject << ";\n";
	out << "\tTQMimeSourceFactory::defaultFactory()->addFactory( factory );\n";
	out << "    }\n";
	out << "}\n\n";

	out << "void qCleanupImages_" << cProject << "()\n";
	out << "{\n";
	out << "    if ( factory ) {\n";
	out << "\tTQMimeSourceFactory::defaultFactory()->removeFactory( factory );\n";
	out << "\tdelete factory;\n";
	out << "\tfactory = 0;\n";
	out << "    }\n";
	out << "}\n\n";

	out << "class StaticInitImages_" << cProject << "\n";
	out << "{\n";
	out << "public:\n";
	out << "    StaticInitImages_" << cProject << "() { qInitImages_" << cProject << "(); }\n";
	out << "#if defined(Q_OS_UNIXWARE)\n";
	out << "    ~StaticInitImages_" << cProject << "() { }\n";
	out << "#else\n";
	out << "    ~StaticInitImages_" << cProject << "() { qCleanupImages_" << cProject << "(); }\n";
	out << "#endif\n";
	out << "};\n\n";

	out << "static StaticInitImages_" << cProject << " staticImages;\n";
    }
}
