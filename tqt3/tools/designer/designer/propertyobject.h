#ifndef PROPERTYOBJECT_H
#define PROPERTYOBJECT_H

#include <ntqobject.h>
#include <ntqwidgetlist.h>
#include <ntqcursor.h>

class PropertyObject : public TQObject
{
public:
    TQMetaObject *metaObject() const { return (TQMetaObject*)mobj; }
    const char *className() const { return "PropertyObject"; }
    TQObject* qObject() { return (TQObject*)this; }
private:
    static TQMetaObject *metaObj;

public:
    PropertyObject( const TQWidgetList &objs );

    bool setProperty( const char *name, const TQVariant& value );
    TQVariant property( const char *name ) const;

    void mdPropertyChanged( const TQString &property, bool changed );
    bool mdIsPropertyChanged( const TQString &property );
    void mdSetPropertyComment( const TQString &property, const TQString &comment );
    TQString mdPropertyComment( const TQString &property );
    void mdSetFakeProperty( const TQString &property, const TQVariant &value );
    TQVariant mdFakeProperty( const TQString &property );
    void mdSetCursor( const TQCursor &c );
    TQCursor mdCursor();
    void mdSetPixmapKey( int pixmap, const TQString &arg );
    TQString mdPixmapKey( int pixmap );
    void mdSetExportMacro( const TQString &macro );
    TQString mdExportMacro();
    TQWidgetList widgetList() const { return objects; }

private:
    TQWidgetList objects;
    const TQMetaObject *mobj;

};

#endif
