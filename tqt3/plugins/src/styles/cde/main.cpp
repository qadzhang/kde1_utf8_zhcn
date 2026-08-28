#include <ntqstyleplugin.h>
#include <ntqcdestyle.h>

class CDEStyle : public TQStylePlugin
{
public:
    CDEStyle();

    TQStringList keys() const;
    TQStyle *create( const TQString& );
};

CDEStyle::CDEStyle()
: TQStylePlugin()
{
}

TQStringList CDEStyle::keys() const
{
    TQStringList list;
    list << "CDE";
    return list;
}

TQStyle* CDEStyle::create( const TQString& s )
{
    if ( s.lower() == "cde" )
	return new TQCDEStyle();

    return 0;
}

TQ_EXPORT_PLUGIN( CDEStyle )
