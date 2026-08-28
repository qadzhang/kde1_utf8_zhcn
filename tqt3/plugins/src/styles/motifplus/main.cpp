#include <ntqstyleplugin.h>
#include <ntqmotifplusstyle.h>

class MotifPlusStyle : public TQStylePlugin
{
public:
    MotifPlusStyle();

    TQStringList keys() const;
    TQStyle *create( const TQString& );

};

MotifPlusStyle::MotifPlusStyle()
: TQStylePlugin()
{
}

TQStringList MotifPlusStyle::keys() const
{
    TQStringList list;
    list << "MotifPlus";
    return list;
}

TQStyle* MotifPlusStyle::create( const TQString& s )
{
    if ( s.lower() == "motifplus" )
	return new TQMotifPlusStyle();

    return 0;
}

TQ_EXPORT_PLUGIN( MotifPlusStyle )

