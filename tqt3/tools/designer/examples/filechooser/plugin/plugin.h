#include <ntqwidgetplugin.h>

class CustomWidgetPlugin : public TQWidgetPlugin
{
public:
    CustomWidgetPlugin();

    TQStringList keys() const;
    TQWidget* create( const TQString &classname, TQWidget* parent = 0, const char* name = 0 );
    TQString group( const TQString& ) const;
    TQIconSet iconSet( const TQString& ) const;
    TQString includeFile( const TQString& ) const;
    TQString toolTip( const TQString& ) const;
    TQString whatsThis( const TQString& ) const;
    bool isContainer( const TQString& ) const;
};
