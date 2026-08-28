#include <ntqstyleplugin.h>
#include <ntqmotifstyle.h>

class MotifStyle : public TQStylePlugin
{
public:
    MotifStyle();

    TQStringList keys() const;
    TQStyle *create( const TQString& );
};

MotifStyle::MotifStyle()
: TQStylePlugin()
{
}

TQStringList MotifStyle::keys() const
{
    TQStringList list;
    list << "Motif";
    return list;
}

TQStyle* MotifStyle::create( const TQString& s )
{
    if ( s.lower() == "motif" )
	return new TQMotifStyle();

    return 0;
}

TQ_EXPORT_PLUGIN( MotifStyle )

