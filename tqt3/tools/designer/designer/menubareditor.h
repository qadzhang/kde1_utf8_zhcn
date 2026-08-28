/**********************************************************************
** Copyright (C) 2005-2008 Trolltech ASA.  All rights reserved.
**
** This file is part of TQt Designer.
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
** Licensees holding valid TQt Commercial licenses may use this file in
** accordance with the TQt Commercial License Agreement provided with
** the Software.
**
** This file is provided "AS IS" with NO WARRANTY OF ANY KIND,
** INCLUDING THE WARRANTIES OF DESIGN, MERCHANTABILITY AND FITNESS FOR
** A PARTICULAR PURPOSE. Trolltech reserves all rights not granted
** herein.
**
**********************************************************************/

#ifndef MENUBAREDITOR_H
#define MENUBAREDITOR_H

#include <ntqmenubar.h>
#include <ntqptrlist.h>

class PopupMenuEditor;
class MenuBarEditor;
class TQActionGroup;

class MenuBarEditorItem : public TQObject
{
    TQ_OBJECT

    friend class MenuBarEditor;

    MenuBarEditorItem( MenuBarEditor * bar = 0, TQObject * parent = 0, const char * name = 0 );
    
public:
    MenuBarEditorItem( PopupMenuEditor * menu, MenuBarEditor * bar,
		       TQObject * parent = 0, const char * name = 0);
    MenuBarEditorItem( TQActionGroup * actionGroup, MenuBarEditor * bar,
		       TQObject * parent = 0, const char * name = 0);
    MenuBarEditorItem( MenuBarEditorItem * item,
		       TQObject * parent = 0, const char * name = 0);
    ~MenuBarEditorItem() { };

    PopupMenuEditor * menu() { return popupMenu; }

    void setMenuText( const TQString t ) { text = t; };
    TQString menuText() { return text; }

    void setVisible( bool enable ) { visible = enable; }
    bool isVisible() { return visible; }

    void setRemovable( bool enable ) { removable = enable; }
    bool isRemovable() { return removable; }

    bool isSeparator() { return separator; }
protected:
    void setSeparator( bool enable ) { separator = enable; }
    
private:
    MenuBarEditor * menuBar;
    PopupMenuEditor * popupMenu;
    TQString text;
    uint visible : 1;
    uint separator : 1;
    uint removable : 1;
};

class TQLineEdit;
class FormWindow;

class MenuBarEditor : public TQMenuBar
{
    TQ_OBJECT
    
public:
    MenuBarEditor( FormWindow * fw, TQWidget * parent = 0, const char * name = 0 );
    ~MenuBarEditor();

    FormWindow * formWindow();

    MenuBarEditorItem * createItem( int index = -1, bool addToCmdStack = true );
    void insertItem( MenuBarEditorItem * item, int index = -1 );
    void insertItem( TQString text, PopupMenuEditor * menu, int index = -1 );
    void insertItem( TQString text, TQActionGroup * group, int index = -1 );

    void insertSeparator( int index = -1 );
    
    void removeItemAt( int index );
    void removeItem( MenuBarEditorItem * item );
    
    int findItem( MenuBarEditorItem * item );
    int findItem( PopupMenuEditor * menu );
    int findItem( TQPoint & pos );
    
    MenuBarEditorItem * item( int index = -1 );

    int count();
    int current();

    void cut( int index );
    void copy( int index );
    void paste( int index );
    void exchange( int a, int b );

    void showLineEdit( int index = -1);
    void showItem( int index = -1 );
    void hideItem( int index = -1 );
    void focusItem( int index = -1 );
    void deleteItem( int index = -1 );

    TQSize sizeHint() const;
    TQSize minimumSize() const { return sizeHint(); }
    TQSize minimumSizeHint() const { return sizeHint(); }
    int heightForWidth( int max_width ) const;

    void show();

    void checkAccels( TQMap<TQChar, TQWidgetList > &accels );

public slots:
    void cut();
    void copy();
    void paste();
    
protected:
    bool eventFilter( TQObject * o, TQEvent * e );
    void paintEvent( TQPaintEvent * e );
    void mousePressEvent( TQMouseEvent * e );
    void mouseDoubleClickEvent( TQMouseEvent * e );
    void mouseMoveEvent( TQMouseEvent * e );
    void dragEnterEvent( TQDragEnterEvent * e );
    void dragLeaveEvent( TQDragLeaveEvent * e );
    void dragMoveEvent( TQDragMoveEvent * e );
    void dropEvent( TQDropEvent * e );
    void keyPressEvent( TQKeyEvent * e );
    void focusOutEvent( TQFocusEvent * e );
    void resizeEvent( TQResizeEvent * e ) { TQFrame::resizeEvent( e ); }

    void resizeInternals();
    
    void drawItems( TQPainter & p );
    void drawItem( TQPainter & p, MenuBarEditorItem * i, int idx, TQPoint & pos );
    void drawSeparator( TQPainter & p, TQPoint & pos );
    
    TQSize itemSize( MenuBarEditorItem * i );
    void addItemSizeToCoords( MenuBarEditorItem * i, int & x, int & y, int w );

    TQPoint itemPos( int index );
    TQPoint snapToItem( const TQPoint & pos );
    void dropInPlace( MenuBarEditorItem * i, const TQPoint & pos );

    void safeDec();
    void safeInc();

    void navigateLeft( bool ctrl );
    void navigateRight( bool ctrl );
    void enterEditMode();
    void leaveEditMode();

    TQPixmap createTextPixmap( const TQString &text );
    int borderSize() const { return margin() + 4; } // add 4 pixels to the margin
    
private:
    FormWindow * formWnd;
    TQLineEdit * lineEdit;
    TQWidget * dropLine;
    TQPtrList<MenuBarEditorItem> itemList;
    MenuBarEditorItem addItem;
    MenuBarEditorItem addSeparator;
    MenuBarEditorItem * draggedItem;
    TQPoint mousePressPos;
    int currentIndex;
    int itemHeight;
    int separatorWidth;
    bool hideWhenEmpty;
    bool hasSeparator;
    bool dropConfirmed;
    
    enum ClipboardOperation {
	None = 0,
	Cut = 1,
	Copy = 2
    };
    static int clipboardOperation;
    static MenuBarEditorItem * clipboardItem;
};

#endif //MENUBAREDITOR_H
