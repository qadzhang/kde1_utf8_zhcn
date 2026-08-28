/****************************************************************************
**
** Definition of TQListView widget class
**
** Created : 970809
**
** Copyright (C) 1992-2008 Trolltech ASA.  All rights reserved.
**
** This file is part of the widgets module of the TQt GUI Toolkit.
**
** This file may be used under the terms of the GNU General
** Public License versions 2.0 or 3.0 as published by the Free
** Software Foundation and appearing in the files LICENSE.GPL2
** and LICENSE.GPL3 included in the packaging of this file.
** Alternatively you may (at your option) use any later version
** of the GNU General Public License if such license has been
** publicly approved by Trolltech ASA (or its successors, if any)
** and the KDE Free TQt Foundation.
**
** Please review the following information to ensure GNU General
** Public Licensing requirements will be met:
** http://trolltech.com/products/qt/licenses/licensing/opensource/.
** If you are unsure which license is appropriate for your use, please
** review the following information:
** http://trolltech.com/products/qt/licenses/licensing/licensingoverview
** or contact the sales department at sales@trolltech.com.
**
** This file may be used under the terms of the Q Public License as
** defined by Trolltech ASA and appearing in the file LICENSE.TQPL
** included in the packaging of this file.  Licensees holding valid TQt
** Commercial licenses may use this file in accordance with the TQt
** Commercial License Agreement provided with the Software.
**
** This file is provided "AS IS" with NO WARRANTY OF ANY KIND,
** INCLUDING THE WARRANTIES OF DESIGN, MERCHANTABILITY AND FITNESS FOR
** A PARTICULAR PURPOSE. Trolltech reserves all rights not granted
** herein.
**
**********************************************************************/

#ifndef TQLISTVIEW_H
#define TQLISTVIEW_H

#ifndef QT_H
#include "ntqscrollview.h"
#endif // QT_H

#ifndef TQT_NO_LISTVIEW


class TQPixmap;
class TQFont;
class TQHeader;
class TQIconSet;

class TQListView;
struct TQListViewPrivate;
struct TQCheckListItemPrivate;
class TQListViewItemIterator;
struct TQListViewItemIteratorPrivate;
class TQDragObject;
class TQMimeSource;
class TQLineEdit;
class TQListViewToolTip;

class TQ_EXPORT TQListViewItem : public TQt
{
    friend class TQListViewItemIterator;
    friend class TQListViewToolTip;

public:
    TQListViewItem( );
    TQListViewItem( TQListView * parent );
    TQListViewItem( TQListViewItem * parent );
    TQListViewItem( TQListView * parent, TQListViewItem * after );
    TQListViewItem( TQListViewItem * parent, TQListViewItem * after );

    TQListViewItem( TQListView * parent,
		   TQString,     TQString = TQString::null,
		   TQString = TQString::null, TQString = TQString::null,
		   TQString = TQString::null, TQString = TQString::null,
		   TQString = TQString::null, TQString = TQString::null );
    TQListViewItem( TQListViewItem * parent,
		   TQString,     TQString = TQString::null,
		   TQString = TQString::null, TQString = TQString::null,
		   TQString = TQString::null, TQString = TQString::null,
		   TQString = TQString::null, TQString = TQString::null );

    TQListViewItem( TQListView * parent, TQListViewItem * after,
		   TQString,     TQString = TQString::null,
		   TQString = TQString::null, TQString = TQString::null,
		   TQString = TQString::null, TQString = TQString::null,
		   TQString = TQString::null, TQString = TQString::null );
    TQListViewItem( TQListViewItem * parent, TQListViewItem * after,
		   TQString,     TQString = TQString::null,
		   TQString = TQString::null, TQString = TQString::null,
		   TQString = TQString::null, TQString = TQString::null,
		   TQString = TQString::null, TQString = TQString::null );
    virtual ~TQListViewItem();

    virtual void insertItem( TQListViewItem * );
    virtual void takeItem( TQListViewItem * );
    virtual void removeItem( TQListViewItem *item ) { takeItem( item ); } //obsolete, use takeItem instead

    int height() const;
    virtual void invalidateHeight();
    int totalHeight() const;
    virtual int width( const TQFontMetrics&,
		       const TQListView*, int column) const;
    void widthChanged(int column=-1) const;
    int depth() const;

    virtual void setText( int, const TQString &);
    virtual TQString text( int ) const;

    virtual void setPixmap( int, const TQPixmap & );
    virtual const TQPixmap * pixmap( int ) const;

    virtual TQString key( int, bool ) const;
    virtual int compare( TQListViewItem *i, int col, bool ) const;
    virtual void sortChildItems( int, bool );

    int childCount() const { return nChildren; }

    bool isOpen() const { return open; }
    virtual void setOpen( bool );
    virtual void setup();

    virtual void setSelected( bool );
    bool isSelected() const { return selected; }

    virtual void paintCell( TQPainter *, const TQColorGroup & cg,
			    int column, int width, int alignment );
    virtual void paintBranches( TQPainter * p, const TQColorGroup & cg,
				int w, int y, int h );
    virtual void paintFocus( TQPainter *, const TQColorGroup & cg,
			     const TQRect & r );

    TQListViewItem * firstChild() const;
    TQListViewItem * nextSibling() const { return siblingItem; }
    TQListViewItem * parent() const;

    TQListViewItem * itemAbove();
    TQListViewItem * itemBelow();

    int itemPos() const;

    TQListView *listView() const;

    virtual void setSelectable( bool enable );
    bool isSelectable() const { return selectable && enabled; }

    virtual void setExpandable( bool );
    bool isExpandable() const { return expandable; }

    void repaint() const;

    virtual void sort();
    void moveItem( TQListViewItem *after );

    virtual void setDragEnabled( bool allow );
    virtual void setDropEnabled( bool allow );
    bool dragEnabled() const;
    bool dropEnabled() const;
    virtual bool acceptDrop( const TQMimeSource *mime ) const;

    void setVisible( bool b );
    bool isVisible() const;

    virtual void setRenameEnabled( int col, bool b );
    bool renameEnabled( int col ) const;
    virtual void startRename( int col );

    virtual void setEnabled( bool b );
    bool isEnabled() const;

    virtual int rtti() const;
    // ### TQt 4: make const or better use an enum
    static int RTTI;

    virtual void setMultiLinesEnabled( bool b );
    bool multiLinesEnabled() const;

protected:
    virtual void enforceSortOrder() const;
    virtual void setHeight( int );
    virtual void activate();

    bool activatedPos( TQPoint & );
#ifndef TQT_NO_DRAGANDDROP
    virtual void dropped( TQDropEvent *e );
#endif
    virtual void dragEntered();
    virtual void dragLeft();
    virtual void okRename( int col );
    virtual void cancelRename( int col );

    void ignoreDoubleClick();

private:
    void init();
    void moveToJustAfter( TQListViewItem * );
    void enforceSortOrderBackToRoot();
    void removeRenameBox();

    int ownHeight;
    int maybeTotalHeight;
    int nChildren;

    uint lsc: 14;
    uint lso: 1;
    uint open : 1;
    uint selected : 1;
    uint selectable: 1;
    uint configured: 1;
    uint expandable: 1;
    uint is_root: 1;
    uint allow_drag : 1;
    uint allow_drop : 1;
    uint visible : 1;
    uint enabled : 1;
    uint mlenabled : 1;

    TQListViewItem * parentItem;
    TQListViewItem * siblingItem;
    TQListViewItem * childItem;
    TQLineEdit *renameBox;
    int renameCol;

    void * columns;

    friend class TQListView;
};

class TQCheckListItem;

class TQ_EXPORT TQListView: public TQScrollView
{
    friend class TQListViewItemIterator;
    friend class TQListViewItem;
    friend class TQCheckListItem;
    friend class TQListViewToolTip;

    TQ_OBJECT
    TQ_ENUMS( SelectionMode ResizeMode RenameAction )
    TQ_PROPERTY( int columns READ columns )
    TQ_PROPERTY( bool multiSelection READ isMultiSelection WRITE setMultiSelection DESIGNABLE false )
    TQ_PROPERTY( SelectionMode selectionMode READ selectionMode WRITE setSelectionMode )
    TQ_PROPERTY( int childCount READ childCount )
    TQ_PROPERTY( bool allColumnsShowFocus READ allColumnsShowFocus WRITE setAllColumnsShowFocus )
    TQ_PROPERTY( bool showSortIndicator READ showSortIndicator WRITE setShowSortIndicator )
    TQ_PROPERTY( int itemMargin READ itemMargin WRITE setItemMargin )
    TQ_PROPERTY( bool rootIsDecorated READ rootIsDecorated WRITE setRootIsDecorated )
    TQ_PROPERTY( bool showToolTips READ showToolTips WRITE setShowToolTips )
    TQ_PROPERTY( ResizeMode resizeMode READ resizeMode WRITE setResizeMode )
    TQ_PROPERTY( int treeStepSize READ treeStepSize WRITE setTreeStepSize )
    TQ_PROPERTY( RenameAction defaultRenameAction READ defaultRenameAction WRITE setDefaultRenameAction )

public:
    TQListView( TQWidget* parent=0, const char* name=0, WFlags f = 0 );
    ~TQListView();

    int treeStepSize() const;
    virtual void setTreeStepSize( int );

    virtual void insertItem( TQListViewItem * );
    virtual void takeItem( TQListViewItem * );
    virtual void removeItem( TQListViewItem *item ) { takeItem( item ); } // obsolete, use takeItem instead

    TQHeader * header() const;

    virtual int addColumn( const TQString &label, int size = -1);
    virtual int addColumn( const TQIconSet& iconset, const TQString &label, int size = -1);
    virtual void removeColumn( int index );
    virtual void setColumnText( int column, const TQString &label );
    virtual void setColumnText( int column, const TQIconSet& iconset, const TQString &label );
    TQString columnText( int column ) const;
    virtual void setColumnWidth( int column, int width );
    int columnWidth( int column ) const;
    enum WidthMode { Manual, Maximum };
    virtual void setColumnWidthMode( int column, WidthMode );
    WidthMode columnWidthMode( int column ) const;
    int columns() const;

    virtual void setColumnAlignment( int, int );
    int columnAlignment( int ) const;

    void show();

    TQListViewItem * itemAt( const TQPoint & screenPos ) const;
    TQRect itemRect( const TQListViewItem * ) const;
    int itemPos( const TQListViewItem * );

    void ensureItemVisible( const TQListViewItem * );

    void repaintItem( const TQListViewItem * ) const;

    virtual void setMultiSelection( bool enable );
    bool isMultiSelection() const;

    enum SelectionMode { Single, Multi, Extended, NoSelection  };
    void setSelectionMode( SelectionMode mode );
    SelectionMode selectionMode() const;

    virtual void clearSelection();
    virtual void setSelected( TQListViewItem *, bool );
    void setSelectionAnchor( TQListViewItem * );
    bool isSelected( const TQListViewItem * ) const;
    TQListViewItem * selectedItem() const;
    virtual void setOpen( TQListViewItem *, bool );
    bool isOpen( const TQListViewItem * ) const;

    virtual void setCurrentItem( TQListViewItem * );
    TQListViewItem * currentItem() const;

    TQListViewItem * firstChild() const;
    TQListViewItem * lastItem() const;

    int childCount() const;

    virtual void setAllColumnsShowFocus( bool );
    bool allColumnsShowFocus() const;

    virtual void setItemMargin( int );
    int itemMargin() const;

    virtual void setRootIsDecorated( bool );
    bool rootIsDecorated() const;

    virtual void setSorting( int column, bool ascending = true );
    int sortColumn() const;
    void setSortColumn( int column );
    SortOrder sortOrder() const;
    void setSortOrder( SortOrder order );
    virtual void sort();

    virtual void setFont( const TQFont & );
    virtual void setPalette( const TQPalette & );

    bool eventFilter( TQObject * o, TQEvent * );

    TQSize sizeHint() const;
    TQSize minimumSizeHint() const;

    virtual void setShowSortIndicator( bool show );
    bool showSortIndicator() const;
    virtual void setShowToolTips( bool b );
    bool showToolTips() const;

    enum ResizeMode { NoColumn, AllColumns, LastColumn };
    virtual void setResizeMode( ResizeMode m );
    ResizeMode resizeMode() const;

    TQListViewItem * findItem( const TQString& text, int column, ComparisonFlags compare = ExactMatch | CaseSensitive ) const;

    enum RenameAction { Accept, Reject };
    virtual void setDefaultRenameAction( RenameAction a );
    RenameAction defaultRenameAction() const;
    bool isRenaming() const;

    void hideColumn( int column );

public slots:
    virtual void clear();
    virtual void invertSelection();
    virtual void selectAll( bool select );
    void triggerUpdate();
    void setContentsPos( int x, int y );
    void adjustColumn( int col );

signals:
    void selectionChanged();
    void selectionChanged( TQListViewItem * );
    void currentChanged( TQListViewItem * );
    void clicked( TQListViewItem * );
    void clicked( TQListViewItem *, const TQPoint &, int );
    void pressed( TQListViewItem * );
    void pressed( TQListViewItem *, const TQPoint &, int );

    void doubleClicked( TQListViewItem * );
    void doubleClicked( TQListViewItem *, const TQPoint&, int );
    void returnPressed( TQListViewItem * );
    void spacePressed( TQListViewItem * );
    void rightButtonClicked( TQListViewItem *, const TQPoint&, int );
    void rightButtonPressed( TQListViewItem *, const TQPoint&, int );
    void mouseButtonPressed( int, TQListViewItem *, const TQPoint& , int );
    void mouseButtonClicked( int, TQListViewItem *,  const TQPoint&, int );

    void contextMenuRequested( TQListViewItem *, const TQPoint &, int );

    void onItem( TQListViewItem *item );
    void onViewport();

    void expanded( TQListViewItem *item );
    void collapsed( TQListViewItem *item );
#ifndef TQT_NO_DRAGANDDROP
    void dropped( TQDropEvent *e );
#endif
    void itemRenamed( TQListViewItem *item, int col, const TQString & );
    void itemRenamed( TQListViewItem *item, int col  );

protected:
    void contentsMousePressEvent( TQMouseEvent * e );
    void contentsMouseReleaseEvent( TQMouseEvent * e );
    void contentsMouseMoveEvent( TQMouseEvent * e );
    void contentsMouseDoubleClickEvent( TQMouseEvent * e );
    void contentsContextMenuEvent( TQContextMenuEvent * e );
#ifndef TQT_NO_DRAGANDDROP
    void contentsDragEnterEvent( TQDragEnterEvent *e );
    void contentsDragMoveEvent( TQDragMoveEvent *e );
    void contentsDragLeaveEvent( TQDragLeaveEvent *e );
    void contentsDropEvent( TQDropEvent *e );
    virtual TQDragObject *dragObject();
    virtual void startDrag();
#endif

    void focusInEvent( TQFocusEvent * e );
    void focusOutEvent( TQFocusEvent * e );

    void keyPressEvent( TQKeyEvent *e );

    void resizeEvent( TQResizeEvent *e );
    void viewportResizeEvent( TQResizeEvent *e );

    void showEvent( TQShowEvent * );

    void drawContentsOffset( TQPainter *, int ox, int oy,
			     int cx, int cy, int cw, int ch );

    virtual void paintEmptyArea( TQPainter *, const TQRect & );
    void styleChange( TQStyle& );
    void windowActivationChange( bool );

protected slots:
    void updateContents();
    void doAutoScroll();

private slots:
    void changeSortColumn( int );
    void handleIndexChange();
    void updateDirtyItems();
    void makeVisible();
    void handleSizeChange( int, int, int );
    void startRename();
    void openFocusItem();

private:
    void contentsMousePressEventEx( TQMouseEvent * e );
    void contentsMouseReleaseEventEx( TQMouseEvent * e );
    void init();
    void updateGeometries();
    void buildDrawableList() const;
    void reconfigureItems();
    void widthChanged(const TQListViewItem*, int c);
    void handleItemChange( TQListViewItem *old, bool shift, bool control );
    void selectRange( TQListViewItem *from, TQListViewItem *to, bool invert, bool includeFirst, bool clearSel = false );
    bool selectRange( TQListViewItem *newItem, TQListViewItem *oldItem, TQListViewItem *anchorItem );
    bool clearRange( TQListViewItem *from, TQListViewItem *to, bool includeFirst = true );
    void doAutoScroll( const TQPoint &cursorPos );

    TQListViewPrivate * d;

private:	// Disabled copy constructor and operator=
#if defined(TQ_DISABLE_COPY)
    TQListView( const TQListView & );
    TQListView &operator=( const TQListView & );
#endif
};


class TQ_EXPORT TQCheckListItem : public TQListViewItem
{
public:
    enum Type { RadioButton,
		CheckBox,
		Controller,
		RadioButtonController=Controller,
		CheckBoxController };
    // ### should be integrated with qbutton in ver4 perhaps
    enum ToggleState { Off, NoChange, On };

    TQCheckListItem( TQCheckListItem *parent, const TQString &text,
		    Type = RadioButtonController );
    TQCheckListItem( TQCheckListItem *parent, TQListViewItem *after,
 		    const TQString &text, Type = RadioButtonController );
    TQCheckListItem( TQListViewItem *parent, const TQString &text,
		    Type = RadioButtonController );
    TQCheckListItem( TQListViewItem *parent, TQListViewItem *after,
 		    const TQString &text, Type = RadioButtonController );
    TQCheckListItem( TQListView *parent, const TQString &text,
		    Type = RadioButtonController );
    TQCheckListItem( TQListView *parent, TQListViewItem *after,
 		    const TQString &text, Type = RadioButtonController );
    TQCheckListItem( TQListViewItem *parent, const TQString &text,
		    const TQPixmap & );
    TQCheckListItem( TQListView *parent, const TQString &text,
		    const TQPixmap & );
    ~TQCheckListItem();

    void paintCell( TQPainter *,  const TQColorGroup & cg,
		    int column, int width, int alignment );
    virtual void paintFocus( TQPainter *, const TQColorGroup & cg,
			     const TQRect & r );
    int width( const TQFontMetrics&, const TQListView*, int column) const;
    void setup();

    virtual void setOn( bool ); // ### should be replaced by setChecked in ver4
    bool isOn() const { return on; }
    Type type() const { return myType; }
    TQString text() const { return TQListViewItem::text( 0 ); }
    TQString text( int n ) const { return TQListViewItem::text( n ); }

    void setTristate( bool );
    bool isTristate() const;
    ToggleState state() const;
    void setState( ToggleState s);

    int rtti() const;
    static int RTTI;

protected:
    void activate();
    void turnOffChild();
    virtual void stateChange( bool );

private:
    void init();
    ToggleState internalState() const;
    void setStoredState( ToggleState newState, void *key );
    ToggleState storedState( void *key ) const;
    void stateChange( ToggleState s );
    void restoreState( void *key, int depth = 0 );
    void updateController( bool update = true , bool store = false );
    void updateStoredState( void *key );
    void setState( ToggleState s, bool update, bool store );
    void setCurrentState( ToggleState s );

    Type myType;
    bool on; // ### remove in ver4
    TQCheckListItemPrivate *d;
};

class TQ_EXPORT TQListViewItemIterator
{
    friend struct TQListViewPrivate;
    friend class TQListView;
    friend class TQListViewItem;

public:
    enum IteratorFlag {
	Visible = 		0x00000001,
	Invisible = 		0x00000002,
	Selected =		0x00000004,
	Unselected = 		0x00000008,
	Selectable =		0x00000010,
	NotSelectable =		0x00000020,
	DragEnabled =		0x00000040,
	DragDisabled =		0x00000080,
	DropEnabled =		0x00000100,
	DropDisabled =		0x00000200,
	Expandable =		0x00000400,
	NotExpandable =		0x00000800,
	Checked =		0x00001000,
	NotChecked =		0x00002000
    };

    TQListViewItemIterator();
    TQListViewItemIterator( TQListViewItem *item );
    TQListViewItemIterator( TQListViewItem *item, int iteratorFlags );

    TQListViewItemIterator( const TQListViewItemIterator &it );
    TQListViewItemIterator( TQListView *lv );
    TQListViewItemIterator( TQListView *lv, int iteratorFlags );

    TQListViewItemIterator &operator=( const TQListViewItemIterator &it );

    ~TQListViewItemIterator();

    TQListViewItemIterator &operator++();
    const TQListViewItemIterator operator++( int );
    TQListViewItemIterator &operator+=( int j );

    TQListViewItemIterator &operator--();
    const TQListViewItemIterator operator--( int );
    TQListViewItemIterator &operator-=( int j );

    TQListViewItem* operator*();
    TQListViewItem *current() const;

protected:
    TQListViewItem *curr;
    TQListView *listView;

private:
    TQListViewItemIteratorPrivate* d() const;
    void init( int flags );
    void addToListView();
    void currentRemoved();
    bool matchesFlags( const TQListViewItem* ) const;
    bool testPair( TQListViewItemIterator::IteratorFlag, TQListViewItemIterator::IteratorFlag, bool ) const;
    bool isChecked( const TQListViewItem* ) const;
};

#endif // TQT_NO_LISTVIEW

#endif // TQLISTVIEW_H
