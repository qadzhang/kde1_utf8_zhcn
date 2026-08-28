#include <ntqtextcodecplugin.h>
#include <ntqtextcodec.h>
#include <ntqptrlist.h>

#include <ntqeuckrcodec.h>
#include <private/qfontcodecs_p.h>


class KRTextCodecs : public TQTextCodecPlugin
{
public:
    KRTextCodecs() {}

    TQStringList names() const { return TQStringList() << "eucKR" << "ksc5601.1987-0"; }
    TQValueList<int> mibEnums() const { return TQValueList<int>() << 38 << 36; }
    TQTextCodec *createForMib( int );
    TQTextCodec *createForName( const TQString & );
};


TQTextCodec *KRTextCodecs::createForMib( int mib )
{
    switch (mib) {
    case 36:
	return new TQFontKsc5601Codec;
    case 38:
	return new TQEucKrCodec;
    default:
	;
    }

    return 0;
}


TQTextCodec *KRTextCodecs::createForName( const TQString &name )
{
    if (name == "eucKR")
	return new TQEucKrCodec;
    if (name == "ksc5601.1987-0")
	return new TQFontKsc5601Codec;

    return 0;
}


TQ_EXPORT_PLUGIN( KRTextCodecs );
