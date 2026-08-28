#include <ntqstyleplugin.h>
#include <ntqwindowsstyle.h>

class WindowsStyle : public TQStylePlugin
{
public:
    WindowsStyle();

    TQStringList keys() const;
    TQStyle *create( const TQString& );
};

WindowsStyle::WindowsStyle()
: TQStylePlugin()
{
}

TQStringList WindowsStyle::keys() const
{
    TQStringList list;
    list << "Windows";
    return list;
}

TQStyle* WindowsStyle::create( const TQString& s )
{
    if ( s.lower() == "windows" )
	return new TQWindowsStyle();

    return 0;
}

TQ_EXPORT_PLUGIN( WindowsStyle )

