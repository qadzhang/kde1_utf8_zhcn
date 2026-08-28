#include <ntqiconview.h>
#include <ntqstring.h>

#include "dnd.h"

class IconViewItem : public TQIconViewItem
{
public:
    IconViewItem( TQIconView * parent, const TQString & text, const TQPixmap & icon, const TQString& tag )
        : TQIconViewItem( parent, text, icon ), _tag( tag ) {}
    virtual ~IconViewItem() {}

    TQString tag() { return _tag; }

private:
    TQString _tag;
};

class IconView : public TQIconView
{
    TQ_OBJECT

public:
    IconView( TQWidget* parent = 0, const char* name = 0 );
    ~IconView();

    TQDragObject *dragObject();

public slots:
    void slotNewItem( TQDropEvent *t, const TQValueList<TQIconDragItem>& );
};
