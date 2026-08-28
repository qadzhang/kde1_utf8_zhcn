#include <ntqimageformatplugin.h>

#ifndef TQT_NO_IMAGEFORMATPLUGIN

#ifdef TQT_NO_IMAGEIO_JPEG
#undef TQT_NO_IMAGEIO_JPEG
#endif
#include "../../../../src/kernel/qjpegio.cpp"

class JPEGFormat : public TQImageFormatPlugin
{
public:
    JPEGFormat();

    TQStringList keys() const;
    bool loadImage( const TQString &format, const TQString &filename, TQImage * );
    bool saveImage( const TQString &format, const TQString &filename, const TQImage & );
    bool installIOHandler( const TQString & );
};

JPEGFormat::JPEGFormat()
{
}


TQStringList JPEGFormat::keys() const
{
    TQStringList list;
    list << "JPEG";

    return list;
}

bool JPEGFormat::loadImage( const TQString &format, const TQString &filename, TQImage *image )
{
    if ( format != "JPEG" )
	return false;

    TQImageIO io;
    io.setFileName( filename );
    io.setImage( *image );

    read_jpeg_image( &io );

    return true;
}

bool JPEGFormat::saveImage( const TQString &format, const TQString &filename, const TQImage &image )
{
    if ( format != "JPEG" )
	return false;

    TQImageIO io;
    io.setFileName( filename );
    io.setImage( image );

    write_jpeg_image( &io );

    return true;
}

bool JPEGFormat::installIOHandler( const TQString &name )
{
    if ( name.upper() != "JPEG" )
	return false;

    qInitJpegIO();
    return true;
}

TQ_EXPORT_PLUGIN( JPEGFormat )

#endif // TQT_NO_IMAGEFORMATPLUGIN
