/**********************************************************************
**
** Definition of TQListBox widget class
**
** Created : 941121
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

#ifndef TQLISTBOX_H
#define TQLISTBOX_H

#ifndef QT_H
#include "ntqscrollview.h"
#include "ntqpixmap.h"
#endif // QT_H

#ifndef TQT_NO_LISTBOX


class TQListBoxPrivate;
class TQListBoxItem;
class TQString;
class TQStrList;
class TQStringList;


class TQ_EXPORT TQListBox : public TQScrollView
{
    friend class TQListBoxItem;
    friend class TQListBoxPrivate;

    TQ_OBJECT
    TQ_ENUMS( SelectionMode LayoutMode )
    TQ_PROPERTY( uint count READ count )
    TQ_PROPERTY( int numItemsVisible READ numItemsVisible )
    TQ_PROPERTY( int currentItem READ currentItem WRITE setCurrentItem )
    TQ_PROPERTY( TQString currentText READ currentText )
    TQ_PROPERTY( int topItem READ topItem WRITE setTopItem DESIGNABLE false )
    TQ_PROPERTY( SelectionMode selectionMode READ selectionMode WRITE setSelectionMode )
    TQ_PROPERTY( bool multiSelection READ isMultiSelection WRITE setMultiSelection DESIGNABLE false )
    TQ_PROPERTY( LayoutMode columnMode READ columnMode WRITE setColumnMode )
    TQ_PROPERTY( LayoutMode rowMode READ rowMode WRITE setRowMode )
    TQ_PROPERTY( int numColumns READ numColumns )
    TQ_PROPERTY( int numRows READ numRows )
    TQ_PROPERTY( bool variableWidth READ variableWidth WRITE setVariableWidth )
    TQ_PROPERTY( bool variableHeight READ variableHeight WRITE setVariableHeight )

public:
    TQListBox( TQWidget* parent=0, const char* name=0, WFlags f=0  );
    ~TQListBox();

    virtual void setFont( const TQFont & );

    uint count() const;

    void insertStringList( const TQStringList&, int index=-1 );
    void insertStrList( const TQStrList *, int index=-1 );
    void insertStrList( const TQStrList &, int index=-1 );
    void insertStrList( const char **,
			int numStrings=-1, int index=-1 );

    void insertItem( const TQListBoxItem *, int index=-1 );
    void insertItem( const TQListBoxItem *, const TQListBoxItem *after );
    void insertItem( const TQString &text, int index=-1 );
    void insertItem( const TQPixmap &pixmap, int index=-1 );
    void insertItem( const TQPixmap &pixmap, const TQString &text, int index=-1 );

    void removeItem( int index );

    TQString text( int index )	const;
    const TQPixmap *pixmap( int index )	const;

    void changeItem( const TQListBoxItem *, int index );
    void changeItem( const TQString &text, int index );
    void changeItem( const TQPixmap &pixmap, int index );
    void changeItem( const TQPixmap &pixmap, const TQString &text, int index );

    void takeItem( const TQListBoxItem * );

    int numItemsVisible() const;

    int currentItem() const;
    TQString currentText() const { return text(currentItem()); }
    virtual void setCurrentItem( int index );
    virtual void setCurrentItem( TQListBoxItem * );
    void centerCurrentItem() { ensureCurrentVisible(); }
    int topItem() const;
    virtual void setTopItem( int index );
    virtual void setBottomItem( int index );

    long maxItemWidth() const;

    enum SelectionMode { Single, Multi, Extended, NoSelection };
    virtual void setSelectionMode( SelectionMode );
    SelectionMode selectionMode() const;

    void setMultiSelection( bool multi );
    bool isMultiSelection() const;

    virtual void setSelected( TQListBoxItem *, bool );
    void setSelected( int, bool );
    bool isSelected( int ) const;
    bool isSelected( const TQListBoxItem * ) const;
    TQListBoxItem* selectedItem() const;

    TQSize sizeHint() const;
    TQSize	minimumSizeHint() const;

    TQListBoxItem *item( int index ) const;
    int index( const TQListBoxItem * ) const;
    TQListBoxItem *findItem( const TQString &text, ComparisonFlags compare = BeginsWith ) const;

    void triggerUpdate( bool doLayout );

    bool itemVisible( int index );
    bool itemVisible( const TQListBoxItem * );

    enum LayoutMode { FixedNumber,
		      FitToWidth, FitToHeight = FitToWidth,
		      Variable };
    virtual void setColumnMode( LayoutMode );
    virtual void setColumnMode( int );
    virtual void setRowMode( LayoutMode );
    virtual void setRowMode( int );

    LayoutMode columnMode() const;
    LayoutMode rowMode() const;

    int numColumns() const;
    int numRows() const;

    bool variableWidth() const;
    virtual void setVariableWidth( bool );

    bool variableHeight() const;
    virtual void setVariableHeight( bool );

    void viewportPaintEvent( TQPaintEvent * );

#ifndef TQT_NO_COMPAT
    bool dragSelect() const { return true; }
    void setDragSelect( bool ) {}
    bool autoScroll() const { return true; }
    void setAutoScroll( bool ) {}
    bool autoScrollBar() const { return vScrollBarMode() == Auto; }
    void setAutoScrollBar( bool enable ) { setVScrollBarMode( enable ? Auto : AlwaysOff ); }
    bool scrollBar() const { return vScrollBarMode() != AlwaysOff; }
    void setScrollBar( bool enable ) { setVScrollBarMode( enable ? AlwaysOn : AlwaysOff ); }
    bool autoBottomScrollBar() const { return hScrollBarMode() == Auto; }
    void setAutoBottomScrollBar( bool enable ) { setHScrollBarMode( enable ? Auto : AlwaysOff ); }
    bool bottomScrollBar() const { return hScrollBarMode() != AlwaysOff; }
    void setBottomScrollBar( bool enable ) { setHScrollBarMode( enable ? AlwaysOn : AlwaysOff ); }
    bool smoothScrolling() const { return false; }
    void setSmoothScrolling( bool ) {}
    bool autoUpdate() const { return true; }
    void setAutoUpdate( bool ) {}
    void setFixedVisibleLines( int lines ) { setRowMode( lines ); }
    int inSort( const TQListBoxItem * );
    int inSort( const TQString& text );
    int cellHeight( int i ) const { return itemHeight(i); }
    int cellHeight() const { return itemHeight(); }
    int cellWidth() const { return maxItemWidth(); }
    int cellWidth(int i) const { Q_ASSERT(i==0); Q_UNUSED(i) return maxItemWidth(); }
    int	numCols() const { return numColumns(); }
#endif

    int itemHeight( int index = 0 ) const;
    TQListBoxItem * itemAt( const TQPoint & ) const;

    TQRect itemRect( TQListBoxItem *item ) const;

    TQListBoxItem *firstItem() const;

    void sort( bool ascending = true );

public slots:
    void clear();
    virtual void ensureCurrentVisible();
    virtual void clearSelection();
    virtual void selectAll( bool select );
    virtual void invertSelection();

signals:
    void highlighted( int index );
    void selected( int index );
    void highlighted( const TQString &);
    void selected( const TQString &);
    void highlighted( TQListBoxItem * );
    void selected( TQListBoxItem * );

    void selectionChanged();
    void selectionChanged( TQListBoxItem * );
    void currentChanged( TQListBoxItem * );
    void clicked( TQListBoxItem * );
    void clicked( TQListBoxItem *, const TQPoint & );
    void pressed( TQListBoxItem * );
    void pressed( TQListBoxItem *, const TQPoint & );

    void doubleClicked( TQListBoxItem * );
    void returnPressed( TQListBoxItem * );
    void rightButtonClicked( TQListBoxItem *, const TQPoint & );
    void rightButtonPressed( TQListBoxItem *, const TQPoint & );
    void mouseButtonPressed( int, TQListBoxItem*, const TQPoint& );
    void mouseButtonClicked( int, TQListBoxItem*, const TQPoint& );

    void contextMenuRequested( TQListBoxItem *, const TQPoint & );

    void onItem( TQListBoxItem *item );
    void onViewport();

protected:
    void mousePressEvent( TQMouseEvent * );
    void mouseReleaseEvent( TQMouseEvent * );
    void mouseDoubleClickEvent( TQMouseEvent * );
    void mouseMoveEvent( TQMouseEvent * );
    void contentsContextMenuEvent( TQContextMenuEvent * );

    void keyPressEvent( TQKeyEvent *e );
    void focusInEvent( TQFocusEvent *e );
    void focusOutEvent( TQFocusEvent *e );
    void resizeEvent( TQResizeEvent * );
    void showEvent( TQShowEvent * );

    bool eventFilter( TQObject *o, TQEvent *e );

    void updateItem( int index );
    void updateItem( TQListBoxItem * );

#ifndef TQT_NO_COMPAT
    void updateCellWidth() { }
    int totalWidth() const { return contentsWidth(); }
    int totalHeight() const { return contentsHeight(); }
#endif

    virtual void paintCell( TQPainter *, int row, int col );

    void toggleCurrentItem();
    bool isRubberSelecting() const;

    void doLayout() const;

    void windowActivationChange( bool );

#ifndef TQT_NO_COMPAT
    bool itemYPos( int index, int *yPos ) const;
    int findItem( int yPos ) const { return index(itemAt(TQPoint(0,yPos)) ); }
#endif

protected slots:
    void clearInputString();

private slots:
    void refreshSlot();
    void doAutoScroll();
    void adjustItems();

private:
    void mousePressEventEx( TQMouseEvent * );
    void tryGeometry( int, int ) const;
    int currentRow() const;
    int currentColumn() const;
    void updateSelection();
    void repaintSelection();
    void drawRubber();
    void doRubberSelection( const TQRect &old, const TQRect &rubber );
    void handleItemChange( TQListBoxItem *old, bool shift, bool control );
    void selectRange( TQListBoxItem *from, TQListBoxItem *to, bool invert, bool includeFirst, bool clearSel = false );

    void emitChangedSignal( bool );

    int columnAt( int ) const;
    int rowAt( int ) const;

    TQListBoxPrivate * d;

    static TQListBox * changedListBox;

private:	// Disabled copy constructor and operator=
#if defined(TQ_DISABLE_COPY)
    TQListBox( const TQListBox & );
    TQListBox &operator=( const TQListBox & );
#endif
};


class TQ_EXPORT TQListBoxItem
{
public:
    TQListBoxItem( TQListBox* listbox = 0 );
    TQListBoxItem( TQListBox* listbox, TQListBoxItem *after );
    virtual ~TQListBoxItem();

    virtual TQString text() const;
    virtual const TQPixmap *pixmap() const;

    virtual int	 height( const TQListBox * ) const;
    virtual int	 width( const TQListBox * )  const;

    bool isSelected() const { return s; }
    bool isCurrent() const;

#ifndef TQT_NO_COMPAT
    bool selected() const { return isSelected(); }
    bool current() const { return isCurrent(); }
#endif

    TQListBox *listBox() const;

    void setSelectable( bool b );
    bool isSelectable() const;

    TQListBoxItem *next() const;
    TQListBoxItem *prev() const;

    virtual int rtti() const;
    static int RTTI;

protected:
    virtual void paint( TQPainter * ) = 0;
    virtual void setText( const TQString &text ) { txt = text; }
    void setCustomHighlighting( bool );

private:
    TQString txt;
    uint s:1;
    uint dirty:1;
    uint custom_highlight : 1;
    int x, y;
    TQListBoxItem * p, * n;
    TQListBox* lbox;
    friend class TQListBox;
    friend class TQListBoxPrivate;
    friend class TQComboBox;
    friend class TQComboBoxPopupItem;

private:	// Disabled copy constructor and operator=
#if defined(TQ_DISABLE_COPY)
    TQListBoxItem( const TQListBoxItem & );
    TQListBoxItem &operator=( const TQListBoxItem & );
#endif
};


class TQ_EXPORT TQListBoxText : public TQListBoxItem
{
public:
    TQListBoxText( TQListBox* listbox, const TQString & text=TQString::null );
    TQListBoxText( const TQString & text=TQString::null );
    TQListBoxText( TQListBox* listbox, const TQString & text, TQListBoxItem *after );
   ~TQListBoxText();

    int	 height( const TQListBox * ) const;
    int	 width( const TQListBox * )  const;

    int rtti() const;
    static int RTTI;

protected:
    void  paint( TQPainter * );

private:	// Disabled copy constructor and operator=
#if defined(TQ_DISABLE_COPY)
    TQListBoxText( const TQListBoxText & );
    TQListBoxText &operator=( const TQListBoxText & );
#endif
};


class TQ_EXPORT TQListBoxPixmap : public TQListBoxItem
{
public:
    TQListBoxPixmap( TQListBox* listbox, const TQPixmap & );
    TQListBoxPixmap( const TQPixmap & );
    TQListBoxPixmap( TQListBox* listbox, const TQPixmap & pix, TQListBoxItem *after );
    TQListBoxPixmap( TQListBox* listbox, const TQPixmap &, const TQString& );
    TQListBoxPixmap( const TQPixmap &, const TQString& );
    TQListBoxPixmap( TQListBox* listbox, const TQPixmap & pix, const TQString&, TQListBoxItem *after );
   ~TQListBoxPixmap();

    const TQPixmap *pixmap() const { return &pm; }

    int	 height( const TQListBox * ) const;
    int	 width( const TQListBox * )  const;

    int rtti() const;
    static int RTTI;

protected:
    void paint( TQPainter * );

private:
    TQPixmap pm;
private:	// Disabled copy constructor and operator=
#if defined(TQ_DISABLE_COPY)
    TQListBoxPixmap( const TQListBoxPixmap & );
    TQListBoxPixmap &operator=( const TQListBoxPixmap & );
#endif
};


#endif // TQT_NO_LISTBOX

#endif // TQLISTBOX_H
