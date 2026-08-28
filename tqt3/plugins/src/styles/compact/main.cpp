#include <ntqstyleplugin.h>
#include <ntqcompactstyle.h>

class CompactStyle : public TQStylePlugin
{
public:
    CompactStyle();

    TQStringList keys() const;
    TQStyle *create( const TQString& );
};

CompactStyle::CompactStyle()
: TQStylePlugin()
{
}

TQStringList CompactStyle::keys() const
{
    TQStringList list;
    list << "Compact";
    return list;
}

TQStyle* CompactStyle::create( const TQString& s )
{
    if ( s.lower() == "compact" )
        return new TQCompactStyle();

    return 0;
}


TQ_EXPORT_PLUGIN( CompactStyle )

