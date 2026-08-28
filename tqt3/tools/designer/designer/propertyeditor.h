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

#ifndef PROPERTYEDITOR_H
#define PROPERTYEDITOR_H

#include <ntqfeatures.h>
#include <ntqvariant.h>
#include <ntqlistview.h>
#include <ntqptrlist.h>
#include <ntqguardedptr.h>
#include <ntqtabwidget.h>
#include <ntqmodules.h>
#include <ntqptrlist.h>
#include <ntqlayout.h>
#include <ntqcheckbox.h>
#include <ntqcombobox.h>
#include "hierarchyview.h"
#include "metadatabase.h"

class PropertyList;
class PropertyEditor;
class TQPainter;
class TQColorGroup;
class TQLineEdit;
class TQPushButton;
class TQHBox;
class TQSpinBox;
class TQLabel;
class TQFrame;
class FormWindow;
class TQCloseEvent;
class TQResizeEvent;
class PropertyWhatsThis;
class TQDateEdit;
class TQTimeEdit;
class TQDateTimeEdit;

class PropertyItem : public TQListViewItem
{
public:
    PropertyItem( PropertyList *l, PropertyItem *after, PropertyItem *prop, const TQString &propName );
    ~PropertyItem();

    void paintCell( TQPainter *p, const TQColorGroup &cg, int column, int width, int align );
    void paintBranches( TQPainter * p, const TQColorGroup & cg,
			int w, int y, int h );
    void paintFocus( TQPainter *p, const TQColorGroup &cg, const TQRect &r );

    virtual bool hasSubItems() const;
    virtual void createChildren();
    virtual void initChildren();

    bool isOpen() const;
    void setOpen( bool b );

    virtual void showEditor();
    virtual void hideEditor();

    virtual void setValue( const TQVariant &v );
    TQVariant value() const;
    TQString name() const;
    virtual void notifyValueChange();

    virtual void setChanged( bool b, bool updateDb = true );
    bool isChanged() const;

    virtual void placeEditor( TQWidget *w );

    virtual PropertyItem *propertyParent() const;
    virtual void childValueChanged( PropertyItem *child );

    void addChild( PropertyItem *i );
    int childCount() const;
    PropertyItem *child( int i ) const;

    virtual bool hasCustomContents() const;
    virtual void drawCustomContents( TQPainter *p, const TQRect &r );

    void updateBackColor();

    void setup() { TQListViewItem::setup(); setHeight( TQListViewItem::height() + 2 ); }

    virtual TQString currentItem() const;
    virtual int currentIntItem() const;
    virtual void setCurrentItem( const TQString &s );
    virtual void setCurrentItem( int i );
    virtual int currentIntItemFromObject() const;
    virtual TQString currentItemFromObject() const;

    void setFocus( TQWidget *w );

    virtual void toggle();
    void setText( int col, const TQString &txt );

protected:
    PropertyList *listview;
    TQVariant val;

private:
    TQColor backgroundColor();
    void createResetButton();
    void updateResetButtonState();

private:
    bool open, changed;
    PropertyItem *property;
    TQString propertyName;
    TQPtrList<PropertyItem> children;
    TQColor backColor;
    TQPushButton *resetButton;

};

class PropertyTextItem : public TQObject,
			 public PropertyItem
{
    TQ_OBJECT

public:
    PropertyTextItem( PropertyList *l, PropertyItem *after, PropertyItem *prop,
		      const TQString &propName, bool comment, bool multiLine, bool ascii = false, bool a = false );
    ~PropertyTextItem();

    virtual void createChildren();
    virtual void initChildren();

    virtual void showEditor();
    virtual void hideEditor();

    virtual void setValue( const TQVariant &v );
    virtual bool hasSubItems() const;
    virtual void childValueChanged( PropertyItem *child );

    virtual void setChanged( bool b, bool updateDb = true );

private slots:
    void setValue();
    void getText();

private:
    TQLineEdit *lined();
    TQGuardedPtr<TQLineEdit> lin;
    TQGuardedPtr<TQHBox> box;
    TQPushButton *button;
    bool withComment, hasMultiLines, asciiOnly, accel;

};


class PropertyBoolItem : public TQObject,
			 public PropertyItem
{
    TQ_OBJECT

public:
    PropertyBoolItem( PropertyList *l, PropertyItem *after, PropertyItem *prop, const TQString &propName );
    ~PropertyBoolItem();

    virtual void showEditor();
    virtual void hideEditor();

    virtual void setValue( const TQVariant &v );
    virtual void toggle();

private slots:
    void setValue();

private:
    TQComboBox *combo();
    TQGuardedPtr<TQComboBox> comb;

};

class PropertyIntItem : public TQObject,
			public PropertyItem
{
    TQ_OBJECT

public:
    PropertyIntItem( PropertyList *l, PropertyItem *after, PropertyItem *prop,
		     const TQString &propName, bool s );
    ~PropertyIntItem();

    virtual void showEditor();
    virtual void hideEditor();

    virtual void setValue( const TQVariant &v );

private slots:
    void setValue();

private:
    TQSpinBox *spinBox();
    TQGuardedPtr<TQSpinBox> spinBx;
    bool signedValue;

};

class PropertyLayoutItem : public TQObject,
			public PropertyItem
{
    TQ_OBJECT

public:
    PropertyLayoutItem( PropertyList *l, PropertyItem *after, PropertyItem *prop,
		        const TQString &propName );
    ~PropertyLayoutItem();

    virtual void showEditor();
    virtual void hideEditor();

    virtual void setValue( const TQVariant &v );

private slots:
    void setValue();

private:
    TQSpinBox *spinBox();
    TQGuardedPtr<TQSpinBox> spinBx;

};

class PropertyDoubleItem : public TQObject,
			public PropertyItem
{
    TQ_OBJECT

public:
    PropertyDoubleItem( PropertyList *l, PropertyItem *after, PropertyItem *prop,
		     const TQString &propName );
    ~PropertyDoubleItem();

    virtual void showEditor();
    virtual void hideEditor();

    virtual void setValue( const TQVariant &v );

private slots:
    void setValue();

private:
    TQLineEdit *lined();
    TQGuardedPtr<TQLineEdit> lin;
};

class PropertyListItem : public TQObject,
			 public PropertyItem
{
    TQ_OBJECT

public:
    PropertyListItem( PropertyList *l, PropertyItem *after, PropertyItem *prop,
		      const TQString &propName, bool editable );
    ~PropertyListItem();

    virtual void showEditor();
    virtual void hideEditor();

    virtual void setValue( const TQVariant &v );

    TQString currentItem() const;
    int currentIntItem() const;
    void setCurrentItem( const TQString &s );
    void setCurrentItem( int i );
    int currentIntItemFromObject() const;
    TQString currentItemFromObject() const;
    void addItem( const TQString &s );

private slots:
    void setValue();

private:
    TQComboBox *combo();
    TQGuardedPtr<TQComboBox> comb;
    int oldInt;
    bool editable;
    TQString oldString;

};

class PropertyFontItem : public TQObject,
			 public PropertyItem
{
    TQ_OBJECT

public:
    PropertyFontItem( PropertyList *l, PropertyItem *after, PropertyItem *prop, const TQString &propName );
    ~PropertyFontItem();

    virtual void createChildren();
    virtual void initChildren();

    virtual void showEditor();
    virtual void hideEditor();

    virtual void setValue( const TQVariant &v );
    virtual bool hasSubItems() const;
    virtual void childValueChanged( PropertyItem *child );

private slots:
    void getFont();

private:
    TQGuardedPtr<TQLineEdit> lined;
    TQGuardedPtr<TQPushButton> button;
    TQGuardedPtr<TQHBox> box;

};

class PropertyCoordItem : public TQObject,
			  public PropertyItem
{
    TQ_OBJECT

public:
    enum Type { Rect, Size, Point };

    PropertyCoordItem( PropertyList *l, PropertyItem *after, PropertyItem *prop,
		       const TQString &propName, Type t );
    ~PropertyCoordItem();

    virtual void createChildren();
    virtual void initChildren();

    virtual void showEditor();
    virtual void hideEditor();

    virtual void setValue( const TQVariant &v );
    virtual bool hasSubItems() const;
    virtual void childValueChanged( PropertyItem *child );

private:
    TQLineEdit *lined();
    TQGuardedPtr<TQLineEdit> lin;
    Type typ;

};

class PropertyColorItem : public TQObject,
			  public PropertyItem
{
    TQ_OBJECT

public:
    PropertyColorItem( PropertyList *l, PropertyItem *after, PropertyItem *prop,
		       const TQString &propName, bool children );
    ~PropertyColorItem();

    virtual void createChildren();
    virtual void initChildren();

    virtual void showEditor();
    virtual void hideEditor();

    virtual void setValue( const TQVariant &v );
    virtual bool hasSubItems() const;
    virtual void childValueChanged( PropertyItem *child );

    virtual bool hasCustomContents() const;
    virtual void drawCustomContents( TQPainter *p, const TQRect &r );

private slots:
    void getColor();

private:
    TQGuardedPtr<TQHBox> box;
    TQGuardedPtr<TQFrame> colorPrev;
    TQGuardedPtr<TQPushButton> button;
    bool withChildren;

};

class PropertyPixmapItem : public TQObject,
			   public PropertyItem
{
    TQ_OBJECT

public:
    enum Type { Pixmap, IconSet, Image };

    PropertyPixmapItem( PropertyList *l, PropertyItem *after, PropertyItem *prop,
			const TQString &propName, Type t );
    ~PropertyPixmapItem();

    virtual void showEditor();
    virtual void hideEditor();

    virtual void setValue( const TQVariant &v );

    virtual bool hasCustomContents() const;
    virtual void drawCustomContents( TQPainter *p, const TQRect &r );

private slots:
    void getPixmap();

private:
    TQGuardedPtr<TQHBox> box;
    TQGuardedPtr<TQLabel> pixPrev;
    TQPushButton *button;
    Type type;

};


class PropertySizePolicyItem : public TQObject,
			  public PropertyItem
{
    TQ_OBJECT

public:
    PropertySizePolicyItem( PropertyList *l, PropertyItem *after, PropertyItem *prop,
			    const TQString &propName );
    ~PropertySizePolicyItem();

    virtual void createChildren();
    virtual void initChildren();

    virtual void showEditor();
    virtual void hideEditor();

    virtual void setValue( const TQVariant &v );
    virtual bool hasSubItems() const;
    virtual void childValueChanged( PropertyItem *child );

private:
    TQLineEdit *lined();
    TQGuardedPtr<TQLineEdit> lin;

};

class PropertyPaletteItem : public TQObject,
			    public PropertyItem
{
    TQ_OBJECT

public:
    PropertyPaletteItem( PropertyList *l, PropertyItem *after, PropertyItem *prop,
			const TQString &propName );
    ~PropertyPaletteItem();

    virtual void showEditor();
    virtual void hideEditor();

    virtual void setValue( const TQVariant &v );

    virtual bool hasCustomContents() const;
    virtual void drawCustomContents( TQPainter *p, const TQRect &r );

private slots:
    void getPalette();

private:
    TQGuardedPtr<TQHBox> box;
    TQGuardedPtr<TQLabel> palettePrev;
    TQGuardedPtr<TQPushButton> button;

};

class PropertyCursorItem : public TQObject,
			   public PropertyItem
{
    TQ_OBJECT

public:
    PropertyCursorItem( PropertyList *l, PropertyItem *after, PropertyItem *prop,
			const TQString &propName );
    ~PropertyCursorItem();

    virtual void showEditor();
    virtual void hideEditor();

    virtual void setValue( const TQVariant &v );

private slots:
    void setValue();

private:
    TQComboBox *combo();
    TQGuardedPtr<TQComboBox> comb;

};

class PropertyKeysequenceItem : public TQObject,
				public PropertyItem
{
    TQ_OBJECT

public:
    PropertyKeysequenceItem( PropertyList *l, PropertyItem *after, PropertyItem *prop,
			     const TQString &propName );
    ~PropertyKeysequenceItem();

    virtual void showEditor();
    virtual void hideEditor();

    virtual void setValue( const TQVariant &v );

private slots:
    void setValue();

private:
    bool eventFilter( TQObject *o, TQEvent *e );
    void handleKeyEvent( TQKeyEvent *e );
    int translateModifiers( int state );

    TQGuardedPtr<TQLineEdit> sequence;
    TQGuardedPtr<TQHBox> box;
    int k1,k2,k3,k4;
    int num;
    bool mouseEnter;
};

class PropertyDatabaseItem : public TQObject,
			     public PropertyItem
{
    TQ_OBJECT

public:
    PropertyDatabaseItem( PropertyList *l, PropertyItem *after, PropertyItem *prop, const TQString &propName, bool wField );
    ~PropertyDatabaseItem();

    virtual void createChildren();
    virtual void initChildren();

    virtual void showEditor();
    virtual void hideEditor();

    virtual void setValue( const TQVariant &v );
    virtual bool hasSubItems() const;
    virtual void childValueChanged( PropertyItem *child );

private:
    TQGuardedPtr<TQLineEdit> lined;
    TQGuardedPtr<TQPushButton> button;
    TQGuardedPtr<TQHBox> box;
    bool withField;

};

struct EnumItem {
    EnumItem( const TQString &k, bool s )
	: key( k ), selected( s ) {}
    EnumItem() : key( TQString::null ), selected( false ) {}
    bool operator==( const EnumItem &item ) const {
	return key == item.key;
    }
    TQString key;
    bool selected;
};

class EnumPopup : public TQFrame
{
    TQ_OBJECT

public:
    EnumPopup( TQWidget *parent, const char *name, WFlags f=0 );
    ~EnumPopup();
    void insertEnums( TQValueList<EnumItem> lst );
    TQValueList<EnumItem> enumList() const;
    void closeWidget();

signals:
    void closed();
    void hidden();

protected:
    void keyPressEvent( TQKeyEvent *e );

private:
    TQValueList<EnumItem> itemList;
    TQPtrList<TQCheckBox> checkBoxList;
    TQVBoxLayout *popLayout;
};

class EnumBox : public TQComboBox
{
    TQ_OBJECT

public:
    EnumBox( TQWidget *parent, const char *name = 0 );
    ~EnumBox() {}
    void setText( const TQString &text );
    void insertEnums( TQValueList<EnumItem> lst );
    TQValueList<EnumItem> enumList() const;

signals:
    void aboutToShowPopup();
    void valueChanged();

protected:
    void paintEvent( TQPaintEvent * );
    void mousePressEvent( TQMouseEvent *e );
    void keyPressEvent( TQKeyEvent *e );

protected slots:
    void restoreArrow();
    void popupHidden();
    void popupClosed();

private:
    void popup();
    bool arrowDown;
    TQString str;
    bool popupShown;
    EnumPopup *pop;

};
class PropertyEnumItem : public TQObject,
			 public PropertyItem
{
    TQ_OBJECT

public:
    PropertyEnumItem( PropertyList *l, PropertyItem *after, PropertyItem *prop,
		      const TQString &propName );
    ~PropertyEnumItem();

    void showEditor();
    void hideEditor();
    void setValue( const TQVariant &v );
    TQString currentItem() const;
    TQString currentItemFromObject() const;
    void setCurrentValues( TQStrList lst );

private slots:
    void setValue();
    void insertEnums();

private:
    TQGuardedPtr<EnumBox> box;
    TQValueList<EnumItem> enumList;
    TQString enumString;
};

class PropertyList : public TQListView
{
    TQ_OBJECT

public:
    PropertyList( PropertyEditor *e );

    virtual void setupProperties();

    virtual void setCurrentItem( TQListViewItem *i );
    virtual void valueChanged( PropertyItem *i );
    virtual void refetchData();
    virtual void setPropertyValue( PropertyItem *i );
    virtual void setCurrentProperty( const TQString &n );

    void layoutInitValue( PropertyItem *i, bool changed = false );
    PropertyEditor *propertyEditor() const;
    TQString whatsThisAt( const TQPoint &p );
    void showCurrentWhatsThis();

    enum LastEventType { KeyEvent, MouseEvent };
    LastEventType lastEvent();

public slots:
    void updateEditorSize();
    void resetProperty();
    void toggleSort();

private slots:
    void itemPressed( TQListViewItem *i, const TQPoint &p, int c );
    void toggleOpen( TQListViewItem *i );
    bool eventFilter( TQObject *o, TQEvent *e );

protected:
    void resizeEvent( TQResizeEvent *e );
    void paintEmptyArea( TQPainter *p, const TQRect &r );
    bool addPropertyItem( PropertyItem *&item, const TQCString &name, TQVariant::Type t );

    void viewportDragEnterEvent( TQDragEnterEvent * );
    void viewportDragMoveEvent ( TQDragMoveEvent * );
    void viewportDropEvent ( TQDropEvent * );

protected:
    PropertyEditor *editor;

private:
    void readPropertyDocs();
    void setupCusWidgetProperties( MetaDataBase::CustomWidget *cw,
				   TQMap<TQString, bool> &unique,
				   PropertyItem *&item );
    TQString whatsThisText( TQListViewItem *i );

private:
    PropertyListItem* pressItem;
    TQPoint pressPos;
    bool mousePressed;
    bool showSorted;
    TQMap<TQString, TQString> propertyDocs;
    PropertyWhatsThis *whatsThis;
    LastEventType theLastEvent;
};

class EventList : public HierarchyList
{
    TQ_OBJECT

public:
    EventList( TQWidget *parent, FormWindow *fw, PropertyEditor *e );

    void setup();
    void setCurrent( TQWidget *w );

protected:
    void contentsMouseDoubleClickEvent( TQMouseEvent *e );

private:
    void save( TQListViewItem *p );

private slots:
    void objectClicked( TQListViewItem *i );
    void showRMBMenu( TQListViewItem *, const TQPoint & );
    void renamed( TQListViewItem *i );

private:
    PropertyEditor *editor;

};


class PropertyEditor : public TQTabWidget
{
    TQ_OBJECT

public:
    PropertyEditor( TQWidget *parent );

    TQObject *widget() const;

    void clear();
    void setup();

    void emitWidgetChanged();
    void refetchData();

    void closed( FormWindow *w );

    PropertyList *propertyList() const;
    FormWindow *formWindow() const;
    EventList *eventList() const;

    TQString currentProperty() const;
    TQString classOfCurrentProperty() const;
    TQMetaObject* metaObjectOfCurrentProperty() const;

    void resetFocus();

    void setPropertyEditorEnabled( bool b );
    void setSignalHandlersEnabled( bool b );

signals:
    void hidden();

public slots:
    void setWidget( TQObject *w, FormWindow *fw );

protected:
    void closeEvent( TQCloseEvent *e );

private:
    void updateWindow();

private:
    TQObject *wid;
    PropertyList *listview;
    EventList *eList;
    FormWindow *formwindow;

};

class PropertyDateItem : public TQObject,
			 public PropertyItem
{
    TQ_OBJECT

public:
    PropertyDateItem( PropertyList *l, PropertyItem *after, PropertyItem *prop, const TQString &propName );
    ~PropertyDateItem();

    virtual void showEditor();
    virtual void hideEditor();

    virtual void setValue( const TQVariant &v );

private slots:
    void setValue();

private:
    TQDateEdit *lined();
    TQGuardedPtr<TQDateEdit> lin;

};

class PropertyTimeItem : public TQObject,
			 public PropertyItem
{
    TQ_OBJECT

public:
    PropertyTimeItem( PropertyList *l, PropertyItem *after, PropertyItem *prop, const TQString &propName );
    ~PropertyTimeItem();

    virtual void showEditor();
    virtual void hideEditor();

    virtual void setValue( const TQVariant &v );

private slots:
    void setValue();

private:
    TQTimeEdit *lined();
    TQGuardedPtr<TQTimeEdit> lin;

};

class PropertyDateTimeItem : public TQObject,
			 public PropertyItem
{
    TQ_OBJECT

public:
    PropertyDateTimeItem( PropertyList *l, PropertyItem *after, PropertyItem *prop, const TQString &propName );
    ~PropertyDateTimeItem();

    virtual void showEditor();
    virtual void hideEditor();

    virtual void setValue( const TQVariant &v );

private slots:
    void setValue();

private:
    TQDateTimeEdit *lined();
    TQGuardedPtr<TQDateTimeEdit> lin;

};

#endif
