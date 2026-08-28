/****************************************************************************
**
** Definition of TQIconView widget class
**
** Created : 990707
**
** Copyright (C) 1992-2008 Trolltech ASA.  All rights reserved.
**
** This file is part of the iconview module of the TQt GUI Toolkit.
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

#ifndef TQICONVIEW_H
#define TQICONVIEW_H

#ifndef QT_H
#include "ntqscrollview.h"
#include "ntqstring.h"
#include "ntqrect.h"
#include "ntqpoint.h"
#include "ntqsize.h"
#include "ntqfont.h" // TQString->TQFont conversion
#include "ntqdragobject.h"
#include "ntqbitmap.h"
#include "ntqpicture.h"
#endif // QT_H

#ifndef TQT_NO_ICONVIEW

class TQIconView;
class TQPainter;
class TQMimeSource;
class TQMouseEvent;
class TQDragEnterEvent;
class TQDragMoveEvent;
class TQDragLeaveEvent;
class TQKeyEvent;
class TQFocusEvent;
class TQShowEvent;
class TQIconViewItem;
class TQIconViewItemLineEdit;
class TQStringList;
class TQIconDragPrivate;

#ifndef TQT_NO_DRAGANDDROP

class TQ_EXPORT TQIconDragItem
{
public:
    TQIconDragItem();
    virtual ~TQIconDragItem();
    virtual TQByteArray data() const;
    virtual void setData( const TQByteArray &d );
    bool operator== ( const TQIconDragItem& ) const;

private:
    TQByteArray ba;

};

class TQ_EXPORT TQIconDrag : public TQDragObject
{
    TQ_OBJECT
public:
    TQIconDrag( TQWidget * dragSource, const char* name = 0 );
    virtual ~TQIconDrag();

    void append( const TQIconDragItem &item, const TQRect &pr, const TQRect &tr );

    virtual const char* format( int i ) const;
    static bool canDecode( TQMimeSource* e );
    virtual TQByteArray encodedData( const char* mime ) const;

private:
    TQIconDragPrivate *d;
    TQChar endMark;

    friend class TQIconView;
    friend class TQIconViewPrivate;
#if defined(TQ_DISABLE_COPY) // Disabled copy constructor and operator=
    TQIconDrag( const TQIconDrag & );
    TQIconDrag &operator=( const TQIconDrag & );
#endif
};

#endif

class TQIconViewToolTip;
class TQIconViewItemPrivate;

class TQ_EXPORT TQIconViewItem : public TQt
{
    friend class TQIconView;
    friend class TQIconViewToolTip;
    friend class TQIconViewItemLineEdit;

public:
    TQIconViewItem( TQIconView *parent );
    TQIconViewItem( TQIconView *parent, TQIconViewItem *after );
    TQIconViewItem( TQIconView *parent, const TQString &text );
    TQIconViewItem( TQIconView *parent, TQIconViewItem *after, const TQString &text );
    TQIconViewItem( TQIconView *parent, const TQString &text, const TQPixmap &icon );
    TQIconViewItem( TQIconView *parent, TQIconViewItem *after, const TQString &text, const TQPixmap &icon );
#ifndef TQT_NO_PICTURE
    TQIconViewItem( TQIconView *parent, const TQString &text, const TQPicture &picture );
    TQIconViewItem( TQIconView *parent, TQIconViewItem *after, const TQString &text, const TQPicture &picture );
#endif
    virtual ~TQIconViewItem();

    virtual void setRenameEnabled( bool allow );
    virtual void setDragEnabled( bool allow );
    virtual void setDropEnabled( bool allow );

    virtual TQString text() const;
    virtual TQPixmap *pixmap() const;
#ifndef TQT_NO_PICTURE
    virtual TQPicture *picture() const;
#endif
    virtual TQString key() const;

    bool renameEnabled() const;
    bool dragEnabled() const;
    bool dropEnabled() const;

    TQIconView *iconView() const;
    TQIconViewItem *prevItem() const;
    TQIconViewItem *nextItem() const;

    int index() const;

    virtual void setSelected( bool s, bool cb );
    virtual void setSelected( bool s );
    virtual void setSelectable( bool s );

    bool isSelected() const;
    bool isSelectable() const;

    void setVisible( bool b );
    bool isVisible() const;

    virtual void repaint();

    virtual bool move( int x, int y );
    virtual void moveBy( int dx, int dy );
    virtual bool move( const TQPoint &pnt );
    virtual void moveBy( const TQPoint &pnt );

    TQRect rect() const;
    int x() const;
    int y() const;
    int width() const;
    int height() const;
    TQSize size() const;
    TQPoint pos() const;
    TQRect textRect( bool relative = true ) const;
    TQRect pixmapRect( bool relative = true ) const;
    bool contains( const TQPoint& pnt ) const;
    bool intersects( const TQRect& r ) const;

    virtual bool acceptDrop( const TQMimeSource *mime ) const;

#ifndef TQT_NO_TEXTEDIT
    void rename();
#endif

    virtual int compare( TQIconViewItem *i ) const;

    virtual void setText( const TQString &text );
    virtual void setPixmap( const TQPixmap &icon );
#ifndef TQT_NO_PICTURE
    virtual void setPicture( const TQPicture &icon );
#endif
    virtual void setText( const TQString &text, bool recalc, bool redraw = true );
    virtual void setPixmap( const TQPixmap &icon, bool recalc, bool redraw = true );
    virtual void setKey( const TQString &k );

    virtual int rtti() const;
    static int RTTI;

protected:
#ifndef TQT_NO_TEXTEDIT
    virtual void removeRenameBox();
#endif
    virtual void calcRect( const TQString &text_ = TQString::null );
    virtual void paintItem( TQPainter *p, const TQColorGroup &cg );
    virtual void paintFocus( TQPainter *p, const TQColorGroup &cg );
#ifndef TQT_NO_DRAGANDDROP
    virtual void dropped( TQDropEvent *e, const TQValueList<TQIconDragItem> &lst );
#endif
    virtual void dragEntered();
    virtual void dragLeft();
    void setItemRect( const TQRect &r );
    void setTextRect( const TQRect &r );
    void setPixmapRect( const TQRect &r );
    void calcTmpText();
    TQString tempText() const;

private:
    void init( TQIconViewItem *after = 0
#ifndef TQT_NO_PICTURE
	       , TQPicture *pic = 0
#endif
	       );
#ifndef TQT_NO_TEXTEDIT
    void renameItem();
    void cancelRenameItem();
#endif
    void checkRect();

    TQIconView *view;
    TQString itemText, itemKey;
    TQString tmpText;
    TQPixmap *itemIcon;
#ifndef TQT_NO_PICTURE
    TQPicture *itemPic;
#endif
    TQIconViewItem *prev, *next;
    uint allow_rename : 1;
    uint allow_drag : 1;
    uint allow_drop : 1;
    uint visible : 1;
    uint selected : 1;
    uint selectable : 1;
    uint dirty : 1;
    uint wordWrapDirty : 1;
    TQRect itemRect, itemTextRect, itemIconRect;
#ifndef TQT_NO_TEXTEDIT
    TQIconViewItemLineEdit *renameBox;
#endif
    TQRect oldRect;

    TQIconViewItemPrivate *d;

};

class TQIconViewPrivate;          /* don't touch */

class TQ_EXPORT TQIconView : public TQScrollView
{
    friend class TQIconViewItem;
    friend class TQIconViewPrivate;
    friend class TQIconViewToolTip;

    TQ_OBJECT
    // #### sorting and sort direction do not work
    TQ_ENUMS( SelectionMode ItemTextPos Arrangement ResizeMode )
    TQ_PROPERTY( bool sorting READ sorting )
    TQ_PROPERTY( bool sortDirection READ sortDirection )
    TQ_PROPERTY( SelectionMode selectionMode READ selectionMode WRITE setSelectionMode )
    TQ_PROPERTY( int gridX READ gridX WRITE setGridX )
    TQ_PROPERTY( int gridY READ gridY WRITE setGridY )
    TQ_PROPERTY( int spacing READ spacing WRITE setSpacing )
    TQ_PROPERTY( ItemTextPos itemTextPos READ itemTextPos WRITE setItemTextPos )
    TQ_PROPERTY( TQBrush itemTextBackground READ itemTextBackground WRITE setItemTextBackground )
    TQ_PROPERTY( Arrangement arrangement READ arrangement WRITE setArrangement )
    TQ_PROPERTY( ResizeMode resizeMode READ resizeMode WRITE setResizeMode )
    TQ_PROPERTY( int maxItemWidth READ maxItemWidth WRITE setMaxItemWidth )
    TQ_PROPERTY( int maxItemTextLength READ maxItemTextLength WRITE setMaxItemTextLength )
    TQ_PROPERTY( bool autoArrange READ autoArrange WRITE setAutoArrange )
    TQ_PROPERTY( bool itemsMovable READ itemsMovable WRITE setItemsMovable )
    TQ_PROPERTY( bool wordWrapIconText READ wordWrapIconText WRITE setWordWrapIconText )
    TQ_PROPERTY( bool showToolTips READ showToolTips WRITE setShowToolTips )
    TQ_PROPERTY( uint count READ count )

public:
    enum SelectionMode {
	Single = 0,
	Multi,
	Extended,
	NoSelection
    };
    enum Arrangement {
	LeftToRight = 0,
	TopToBottom
    };
    enum ResizeMode {
	Fixed = 0,
	Adjust
    };
    enum ItemTextPos {
	Bottom = 0,
	Right
    };

    TQIconView( TQWidget* parent=0, const char* name=0, WFlags f = 0 );
    virtual ~TQIconView();

    virtual void insertItem( TQIconViewItem *item, TQIconViewItem *after = 0L );
    virtual void takeItem( TQIconViewItem *item );

    int index( const TQIconViewItem *item ) const;

    TQIconViewItem *firstItem() const;
    TQIconViewItem *lastItem() const;
    TQIconViewItem *currentItem() const;
    virtual void setCurrentItem( TQIconViewItem *item );
    virtual void setSelected( TQIconViewItem *item, bool s, bool cb = false );

    uint count() const;

public:
    virtual void showEvent( TQShowEvent * );

    virtual void setSelectionMode( SelectionMode m );
    SelectionMode selectionMode() const;

    TQIconViewItem *findItem( const TQPoint &pos ) const;
    TQIconViewItem *findItem( const TQString &text, ComparisonFlags compare = BeginsWith ) const;
    virtual void selectAll( bool select );
    virtual void clearSelection();
    virtual void invertSelection();

    virtual void repaintItem( TQIconViewItem *item );
    void repaintSelectedItems();

    void ensureItemVisible( TQIconViewItem *item );
    TQIconViewItem* findFirstVisibleItem( const TQRect &r ) const;
    TQIconViewItem* findLastVisibleItem( const TQRect &r ) const;

    virtual void clear();

    virtual void setGridX( int rx );
    virtual void setGridY( int ry );
    int gridX() const;
    int gridY() const;
    virtual void setSpacing( int sp );
    int spacing() const;
    virtual void setItemTextPos( ItemTextPos pos );
    ItemTextPos itemTextPos() const;
    virtual void setItemTextBackground( const TQBrush &b );
    TQBrush itemTextBackground() const;
    virtual void setArrangement( Arrangement am );
    Arrangement arrangement() const;
    virtual void setResizeMode( ResizeMode am );
    ResizeMode resizeMode() const;
    virtual void setMaxItemWidth( int w );
    int maxItemWidth() const;
    virtual void setMaxItemTextLength( int w );
    int maxItemTextLength() const;
    virtual void setAutoArrange( bool b );
    bool autoArrange() const;
    virtual void setShowToolTips( bool b );
    bool showToolTips() const;

    void setSorting( bool sort, bool ascending = true );
    bool sorting() const;
    bool sortDirection() const;

    virtual void setItemsMovable( bool b );
    bool itemsMovable() const;
    virtual void setWordWrapIconText( bool b );
    bool wordWrapIconText() const;

    bool eventFilter( TQObject * o, TQEvent * );

    TQSize minimumSizeHint() const;
    TQSize sizeHint() const;

    virtual void sort( bool ascending = true );

    virtual void setFont( const TQFont & );
    virtual void setPalette( const TQPalette & );

    bool isRenaming() const;

public slots:
    virtual void arrangeItemsInGrid( const TQSize &grid, bool update = true );
    virtual void arrangeItemsInGrid( bool update = true );
    virtual void setContentsPos( int x, int y );
    virtual void updateContents();

signals:
    void selectionChanged();
    void selectionChanged( TQIconViewItem *item );
    void currentChanged( TQIconViewItem *item );
    void clicked( TQIconViewItem * );
    void clicked( TQIconViewItem *, const TQPoint & );
    void pressed( TQIconViewItem * );
    void pressed( TQIconViewItem *, const TQPoint & );

    void doubleClicked( TQIconViewItem *item );
    void returnPressed( TQIconViewItem *item );
    void rightButtonClicked( TQIconViewItem* item, const TQPoint& pos );
    void rightButtonPressed( TQIconViewItem* item, const TQPoint& pos );
    void mouseButtonPressed( int button, TQIconViewItem* item, const TQPoint& pos );
    void mouseButtonClicked( int button, TQIconViewItem* item, const TQPoint& pos );
    void contextMenuRequested( TQIconViewItem* item, const TQPoint &pos );

#ifndef TQT_NO_DRAGANDDROP
    void dropped( TQDropEvent *e, const TQValueList<TQIconDragItem> &lst );
#endif
    void moved();
    void onItem( TQIconViewItem *item );
    void onViewport();
    void itemRenamed( TQIconViewItem *item, const TQString & );
    void itemRenamed( TQIconViewItem *item );

protected slots:
    virtual void doAutoScroll();
    virtual void adjustItems();
    virtual void slotUpdate();

private slots:
    void movedContents( int dx, int dy );

protected:
    void drawContents( TQPainter *p, int cx, int cy, int cw, int ch );
    void contentsMousePressEvent( TQMouseEvent *e );
    void contentsMouseReleaseEvent( TQMouseEvent *e );
    void contentsMouseMoveEvent( TQMouseEvent *e );
    void contentsMouseDoubleClickEvent( TQMouseEvent *e );
    void contentsContextMenuEvent( TQContextMenuEvent *e );

#ifndef TQT_NO_DRAGANDDROP
    void contentsDragEnterEvent( TQDragEnterEvent *e );
    void contentsDragMoveEvent( TQDragMoveEvent *e );
    void contentsDragLeaveEvent( TQDragLeaveEvent *e );
    void contentsDropEvent( TQDropEvent *e );
#endif

    void bufferedPaintEvent( TQPaintEvent* );
    void resizeEvent( TQResizeEvent* e );
    void keyPressEvent( TQKeyEvent *e );
    void focusInEvent( TQFocusEvent *e );
    void focusOutEvent( TQFocusEvent *e );
    void enterEvent( TQEvent *e );

    virtual void drawRubber( TQPainter *p );
#ifndef TQT_NO_DRAGANDDROP
    virtual TQDragObject *dragObject();
    virtual void startDrag();
#endif
    virtual void insertInGrid( TQIconViewItem *item );
    virtual void drawBackground( TQPainter *p, const TQRect &r );

    void emitSelectionChanged( TQIconViewItem * i = 0 );
    void emitRenamed( TQIconViewItem *item );

    TQIconViewItem *makeRowLayout( TQIconViewItem *begin, int &y, bool &changed );

    void styleChange( TQStyle& );
    void windowActivationChange( bool );

private:
    void contentsMousePressEventEx( TQMouseEvent *e );
    virtual void drawDragShapes( const TQPoint &pnt );
#ifndef TQT_NO_DRAGANDDROP
    virtual void initDragEnter( TQDropEvent *e );
#endif
    void drawContents( TQPainter* );
    TQIconViewItem* findItemByName( TQIconViewItem *start );
    void handleItemChange( TQIconViewItem *old, bool shift,
			   bool control, bool homeend = false);

    int calcGridNum( int w, int x ) const;
    TQIconViewItem *rowBegin( TQIconViewItem *item ) const;
    void updateItemContainer( TQIconViewItem *item );
    void appendItemContainer();
    void rebuildContainers();
    enum Direction {
	DirUp = 0,
	DirDown,
	DirLeft,
	DirRight
    };
    TQIconViewItem* findItem(Direction dir, const TQIconViewItem *fromItem) const;
    TQBitmap mask( TQPixmap *pix ) const;
    int visibleWidthSB() const;
    int visibleHeightSB() const;

    TQIconViewPrivate *d;

private:	// Disabled copy constructor and operator=
#if defined(TQ_DISABLE_COPY)
    TQIconView( const TQIconView & );
    TQIconView& operator=( const TQIconView & );
#endif
};

#endif // TQT_NO_ICONVIEW

#endif // TQICONVIEW_H
