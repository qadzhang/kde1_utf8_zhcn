#include <ntqtextcodecplugin.h>
#include <ntqtextcodec.h>
#include <ntqptrlist.h>

#include <ntqeucjpcodec.h>
#include <ntqjiscodec.h>
#include <ntqsjiscodec.h>
#include <private/qfontcodecs_p.h>


class JPTextCodecs : public TQTextCodecPlugin
{
public:
    JPTextCodecs() {}

    TQStringList names() const { return TQStringList() << "eucJP" << "JIS7" << "SJIS" << "jisx0208.1983-0"; }
    TQValueList<int> mibEnums() const { return TQValueList<int>() << 16 << 17 << 18 << 63; }
    TQTextCodec *createForMib( int );
    TQTextCodec *createForName( const TQString & );
};

TQTextCodec *JPTextCodecs::createForMib( int mib )
{
    switch (mib) {
    case 16:
	return new TQJisCodec;
    case 17:
	return new TQSjisCodec;
    case 18:
	return new TQEucJpCodec;
    case 63:
	return new TQFontJis0208Codec;
    default:
	;
    }

    return 0;
}


TQTextCodec *JPTextCodecs::createForName( const TQString &name )
{
    if (name == "JIS7")
	return new TQJisCodec;
    if (name == "SJIS")
	return new TQSjisCodec;
    if (name == "eucJP")
	return new TQEucJpCodec;
    if (name == "jisx0208.1983-0")
	return new TQFontJis0208Codec;

    return 0;
}


TQ_EXPORT_PLUGIN( JPTextCodecs );
