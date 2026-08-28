#include <ntqtextcodecplugin.h>
#include <ntqtextcodec.h>
#include <ntqptrlist.h>

#include <ntqbig5codec.h>
#include <private/qfontcodecs_p.h>


class TWTextCodecs : public TQTextCodecPlugin
{
public:
    TWTextCodecs() {}
    
    TQStringList names() const { return TQStringList() << "Big5" << "big5*-0"; }
    TQValueList<int> mibEnums() const { return TQValueList<int>() << 2026 << -2026; }
    TQTextCodec *createForMib( int );
    TQTextCodec *createForName( const TQString & );
};

TQTextCodec *TWTextCodecs::createForMib( int mib )
{
    switch (mib) {
    case -2026:
	return new TQFontBig5Codec;
    case 2026:
	return new TQBig5Codec;
    default:
	;
    }

    return 0;
}


TQTextCodec *TWTextCodecs::createForName( const TQString &name )
{
    if (name == "Big5")
	return new TQBig5Codec;
    if (name == "big5*-0")
	return new TQFontBig5Codec;

    return 0;
}


TQ_EXPORT_PLUGIN( TWTextCodecs );

