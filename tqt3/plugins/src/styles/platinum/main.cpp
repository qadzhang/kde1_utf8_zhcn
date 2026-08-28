#include <ntqplatinumstyle.h>
#include <ntqstyleplugin.h>

class PlatinumStyle : public TQStylePlugin
{
public:
    PlatinumStyle();

    TQStringList keys() const;
    TQStyle *create( const TQString& );
};

PlatinumStyle::PlatinumStyle()
: TQStylePlugin()
{
}

TQStringList PlatinumStyle::keys() const
{
    TQStringList list;
    list << "Platinum";
    return list;
}

TQStyle* PlatinumStyle::create( const TQString& s )
{
    if ( s.lower() == "platinum" )
        return new TQPlatinumStyle();

    return 0;
}


TQ_EXPORT_PLUGIN( PlatinumStyle )
