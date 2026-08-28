#include <ntqstyleplugin.h>
#include <ntqsgistyle.h>

class SGIStyle : public TQStylePlugin
{
public:
    SGIStyle();

    TQStringList keys() const;
    TQStyle *create( const TQString& );
};

SGIStyle::SGIStyle()
: TQStylePlugin()
{
}

TQStringList SGIStyle::keys() const
{
    TQStringList list;
    list << "SGI";
    return list;
}

TQStyle* SGIStyle::create( const TQString& s )
{
    if ( s.lower() == "sgi" )
        return new TQSGIStyle();

    return 0;
}

TQ_EXPORT_PLUGIN( SGIStyle )
