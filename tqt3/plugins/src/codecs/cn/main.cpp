#include <ntqtextcodecplugin.h>
#include <ntqtextcodec.h>
#include <ntqptrlist.h>
#include <ntqapplication.h>

#include <ntqgb18030codec.h>
#include <private/qfontcodecs_p.h>


class CNTextCodecs : public TQTextCodecPlugin
{
public:
    CNTextCodecs() {}

    TQStringList names() const { return TQStringList() << "GB18030" << "GBK" << "gb2312.1980-0" << "gbk-0"; }
    TQValueList<int> mibEnums() const { return TQValueList<int>() << -2025 << 57 << 2025; }

    TQTextCodec *createForMib( int );
    TQTextCodec *createForName( const TQString & );
};

TQTextCodec *CNTextCodecs::createForMib( int mib )
{
    switch (mib) {
    case 57:
	return new TQGb2312Codec;
    case 2025:
	return new TQGbkCodec;
    case -2025:
	return new TQGb18030Codec;
    default:
	;
    }

    return 0;
}


TQTextCodec *CNTextCodecs::createForName( const TQString &name )
{
    if (name == "GB18030")
	return new TQGb18030Codec;
    if (name == "GBK" || name == "gbk-0")
	return new TQGbkCodec;
    if (name == "gb2312.1980-0")
	return new TQGb2312Codec;

    return 0;
}


TQ_EXPORT_PLUGIN( CNTextCodecs );
