#include <ntqpixmap.h>
#include <ntqmap.h>
#include "dndbase.h"

#ifndef DNDDEMO_H
#define DNDDEMO_H

class IconItem
{
public:
    IconItem( const TQString& name = TQString::null, const TQString& icon = TQString::null );

    TQString name() { return _name; }
    TQPixmap *pixmap() { return &_pixmap; }

protected:
    TQPixmap loadPixmap( const TQString& name );

private:
    TQString _name;
    TQPixmap _pixmap;
};

class DnDDemo : public DnDDemoBase
{
    TQ_OBJECT

public:
    DnDDemo( TQWidget* parent = 0, const char* name = 0 );
    ~DnDDemo();

    IconItem findItem( const TQString& tag );

private:
    TQMap<TQString,IconItem> items;
};

#endif
