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

#ifndef POPUPMENUEDITOR_H
#define POPUPMENUEDITOR_H

#include <ntqwidget.h>
#include <ntqptrlist.h>
#include <ntqaction.h>

class PopupMenuEditor;
class TQMenuItem;

class PopupMenuEditorItem : public TQObject
{
    TQ_OBJECT

    friend class PopupMenuEditor;

    PopupMenuEditorItem( PopupMenuEditor * menu = 0, TQObject * parent = 0, const char * name = 0 );

public:
    enum ItemType {
	Unknown = -1,
	Separator = 0,
	Action = 1
    };

    PopupMenuEditorItem( TQAction * action, PopupMenuEditor * menu,
			 TQObject * parent = 0, const char * name = 0 );
    PopupMenuEditorItem( PopupMenuEditorItem * item, PopupMenuEditor * menu,
			 TQObject * parent = 0, const char * name = 0 );
    ~PopupMenuEditorItem();

    void init();

    ItemType type() const;
    TQAction * action() const { return a; }

    void setVisible( bool enable );
    bool isVisible() const;

    void setSeparator( bool enable ) { separator = enable; }
    bool isSeparator() const { return separator; }

    void setRemovable( bool enable ) { removable = enable; }
    bool isRemovable() const { return removable; }
    
    void showMenu( int x, int y );
    void hideMenu();
    void focusOnMenu();
    PopupMenuEditor * subMenu() const { return s; }

    int count() const;

    bool eventFilter( TQObject *, TQEvent * event );
    
public slots:
    void selfDestruct();

protected:

private:
    TQAction * a;
    PopupMenuEditor * s;
    PopupMenuEditor * m;
    uint separator : 1;
    uint removable : 1;
};

class FormWindow;
class MainWindow;
class TQLineEdit;

#include <ntqpopupmenu.h>

class PopupMenuEditor : public TQWidget
{
    TQ_OBJECT

    friend class PopupMenuEditorItem;
    friend class MenuBarEditor;
    friend class Resource;

public:
    PopupMenuEditor( FormWindow * fw, TQWidget * parent = 0, const char * name = 0 );
    PopupMenuEditor( FormWindow * fw, PopupMenuEditor * menu, TQWidget * parent, const char * name = 0 );
    ~PopupMenuEditor();

    void init();

    void insert( PopupMenuEditorItem * item, int index = -1 );
    void insert( TQAction * action, int index = -1 );
    void insert( TQActionGroup * actionGroup, int index = -1 );
    int find( const TQAction * action );
    int find( PopupMenuEditor * menu );
    int count();
    PopupMenuEditorItem * at( int index );
    PopupMenuEditorItem * at( TQPoint pos ) { return itemAt( pos.y() ); }
    void exchange( int a, int b );

    void cut( int index );
    void copy( int index );
    void paste( int index );

    void insertedActions( TQPtrList<TQAction> & list );

    void show();
    void choosePixmap( int index = -1 );
    void showLineEdit( int index = -1);
    void setAccelerator( int key, TQt::ButtonState state, int index = -1 );

    FormWindow * formWindow() { return formWnd; }
    bool isCreatingAccelerator() { return ( currentField == 2 ); }

    TQPtrList<PopupMenuEditorItem> * items() { return &itemList; }

    TQWidget * parentEditor() { return parentMenu; }

signals:
    void inserted( TQAction * );
    void removed(  TQAction * );
    
public slots:

    void cut() { cut( currentIndex ); }
    void copy() { copy( currentIndex ); }
    void paste() { paste( currentIndex ); }

    void remove( int index );
    void remove( TQAction * a ) { remove( find( a ) ); }

    void resizeToContents();
    void showSubMenu();
    void hideSubMenu();
    void focusOnSubMenu();
    
protected:
    PopupMenuEditorItem * createItem( TQAction * a = 0 );
    void removeItem( int index = -1 );
    PopupMenuEditorItem * currentItem();
    PopupMenuEditorItem * itemAt( int y );
    void setFocusAt( const TQPoint & pos );

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
    void focusInEvent( TQFocusEvent * e );
    void focusOutEvent( TQFocusEvent * e );

    void drawItems( TQPainter * p );
    void drawItem( TQPainter * p, PopupMenuEditorItem * i, const TQRect & r, int f ) const;
    void drawWinFocusRect( TQPainter * p, const TQRect & r ) const;

    TQSize contentsSize();
    int itemHeight( const PopupMenuEditorItem * item ) const;
    int itemPos( const PopupMenuEditorItem * item ) const;
    
    int snapToItem( int y );
    void dropInPlace( PopupMenuEditorItem * i, int y );
    void dropInPlace( TQActionGroup * g, int y );

    void safeDec();
    void safeInc();

    void clearCurrentField();
    void navigateUp( bool ctrl );
    void navigateDown( bool ctrl );
    void navigateLeft();
    void navigateRight();
    void enterEditMode( TQKeyEvent * e );
    void leaveEditMode( TQKeyEvent * e );

    TQString constructName( PopupMenuEditorItem *item );

private:
    FormWindow * formWnd;
    TQLineEdit * lineEdit;
    TQWidget * dropLine;
    TQPtrList<PopupMenuEditorItem> itemList;
    PopupMenuEditorItem addItem;
    PopupMenuEditorItem addSeparator;
    TQWidget * parentMenu;

    int iconWidth;
    int textWidth;
    int accelWidth;
    int arrowWidth;
    int borderSize;
    
    int currentField;
    int currentIndex;
    TQPoint mousePressPos;
    static PopupMenuEditorItem * draggedItem;

    enum ClipboardOperation {
	None = 0,
	Cut = 1,
	Copy = 2
    };
    static int clipboardOperation;
    static PopupMenuEditorItem * clipboardItem;
};

#endif //POPUPMENUEDITOR_H
