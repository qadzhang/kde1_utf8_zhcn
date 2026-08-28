#include <ntqlistview.h>

class ListViewItem : public TQListViewItem
{
public:
    ListViewItem ( TQListView * parent, const TQString& name, const TQString& tag )
        : TQListViewItem( parent, name ), _tag( tag ) {}
    ListViewItem ( TQListView * parent, TQListViewItem * after, const TQString& name, const TQString& tag )
        : TQListViewItem( parent, after, name ), _tag( tag ) {}
    virtual ~ListViewItem() {}

    TQString tag() { return _tag; }

private:
    TQString _tag;
};

class ListView : public TQListView
{
    TQ_OBJECT

public:
    ListView( TQWidget* parent = 0, const char* name = 0 );
    ~ListView();

    void dragEnterEvent( TQDragEnterEvent * );
    void dropEvent( TQDropEvent * );
    void contentsMousePressEvent( TQMouseEvent * );
    void contentsMouseMoveEvent( TQMouseEvent * );
    void contentsMouseReleaseEvent( TQMouseEvent * );

private:
    TQPoint pressPos;
    bool dragging;
};
