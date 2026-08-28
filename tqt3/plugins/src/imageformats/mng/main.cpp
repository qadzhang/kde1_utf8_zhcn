#include <ntqimageformatplugin.h>

#ifndef TQT_NO_IMAGEFORMATPLUGIN

#ifdef TQT_NO_IMAGEIO_MNG
#undef TQT_NO_IMAGEIO_MNG
#endif
#include "../../../../src/kernel/qmngio.cpp"

class MNGFormat : public TQImageFormatPlugin
{
public:
    MNGFormat();

    TQStringList keys() const;
    bool loadImage( const TQString &format, const TQString &filename, TQImage *image );
    bool saveImage( const TQString &format, const TQString &filename, const TQImage &image );
    bool installIOHandler( const TQString & );
};

MNGFormat::MNGFormat()
{
}


TQStringList MNGFormat::keys() const
{
    TQStringList list;
    list << "MNG";

    return list;
}

bool MNGFormat::loadImage( const TQString &, const TQString &, TQImage * )
{
    return false;
}

bool MNGFormat::saveImage( const TQString &, const TQString &, const TQImage& )
{
    return false;
}

bool MNGFormat::installIOHandler( const TQString &name )
{
    if ( name != "MNG" )
	return false;

    qInitMngIO();
    return true;
}

TQ_EXPORT_PLUGIN( MNGFormat )

#endif // TQT_NO_IMAGEFORMATPLUGIN
