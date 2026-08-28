#include <ntqimageformatplugin.h>

#ifndef TQT_NO_IMAGEFORMATPLUGIN

#ifdef TQT_NO_IMAGEIO_PNG
#undef TQT_NO_IMAGEIO_PNG
#endif
#include "../../../../src/kernel/qpngio.cpp"

class PNGFormat : public TQImageFormatPlugin
{
public:
    PNGFormat();

    TQStringList keys() const;
    bool loadImage( const TQString &format, const TQString &filename, TQImage * );
    bool saveImage( const TQString &format, const TQString &filename, const TQImage& );
    bool installIOHandler( const TQString & );
};

PNGFormat::PNGFormat()
{
}


TQStringList PNGFormat::keys() const
{
    TQStringList list;
    list << "PNG";

    return list;
}

bool PNGFormat::loadImage( const TQString &format, const TQString &filename, TQImage *image )
{
    if ( format != "PNG" )
	return false;

    TQImageIO io;
    io.setFileName( filename );
    io.setImage( *image );

    read_png_image( &io );

    return true;
}

bool PNGFormat::saveImage( const TQString &format, const TQString &filename, const TQImage &image )
{
    if ( format != "PNG" )
	return false;

    TQImageIO io;
    io.setFileName( filename );
    io.setImage( image );

    write_png_image( &io );

    return true;
}

bool PNGFormat::installIOHandler( const TQString &name )
{
    if ( name != "PNG" ) 
	return false;

    qInitPngIO();
    return true;
}

TQ_EXPORT_PLUGIN( PNGFormat )

#endif // TQT_NO_IMAGEFORMATPLUGIN
