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

#ifndef WIDGETFACTORY_H
#define WIDGETFACTORY_H

#include <ntqvariant.h>
#include <ntqiconset.h>
#include <ntqstring.h>
#include <ntqintdict.h>
#include <ntqtabwidget.h>
#include <ntqpixmap.h>
#include <ntqsize.h>
#include <ntqpainter.h>
#include <ntqevent.h>
#include <ntqobjectlist.h>
#include <ntqlabel.h>
#include <ntqwizard.h>
#include <ntqptrdict.h>
#include <ntqpushbutton.h>
#include <ntqtoolbutton.h>
#include <ntqcheckbox.h>
#include <ntqradiobutton.h>
#include <ntqbuttongroup.h>
#include <ntqwidgetstack.h>
#include <ntqguardedptr.h>
#include <ntqtoolbox.h>

#include "metadatabase.h"
#include "ntqwidgetfactory.h"

class TQWidget;
class TQLayout;
class FormWindow;

class CustomWidgetFactory : public TQWidgetFactory
{
public:
    CustomWidgetFactory();
    TQWidget *createWidget( const TQString &className, TQWidget *parent, const char *name ) const;

};

class WidgetFactory : public TQt
{
    friend class CustomWidgetFactory;

public:
    enum LayoutType {
	HBox,
	VBox,
	Grid,
	NoLayout
    };

    static TQWidget *create( int id, TQWidget *parent, const char *name = 0, bool init = true,
			    const TQRect *rect = 0, TQt::Orientation orient = TQt::Horizontal );
    static TQLayout *createLayout( TQWidget *widget, TQLayout* layout, LayoutType type );
    static void deleteLayout( TQWidget *widget );

    static LayoutType layoutType( TQWidget *w );
    static LayoutType layoutType( TQWidget *w, TQLayout *&layout );
    static LayoutType layoutType( TQLayout *layout );
    static TQWidget *layoutParent( TQLayout *layout );

    static TQWidget* containerOfWidget( TQWidget *w );
    static TQWidget* widgetOfContainer( TQWidget *w );

    static bool isPassiveInteractor( TQObject* o );
    static const char* classNameOf( TQObject* o );

    static void initChangedProperties( TQObject *o );

    static bool hasSpecialEditor( int id, TQObject *editorWidget );
    static bool hasItems( int id, TQObject *editorWidget );
    static void editWidget( int id, TQWidget *parent, TQWidget *editWidget, FormWindow *fw );

    static bool canResetProperty( TQObject *w, const TQString &propName );
    static bool resetProperty( TQObject *w, const TQString &propName );
    static TQVariant defaultValue( TQObject *w, const TQString &propName );
    static TQString defaultCurrentItem( TQObject *w, const TQString &propName );

    static TQVariant property( TQObject *w, const char *name );
    static void saveDefaultProperties( TQObject *w, int id );
    static void saveChangedProperties( TQObject *w, int id );

    static TQString defaultSignal( TQObject *w );

private:
    static TQWidget *createWidget( const TQString &className, TQWidget *parent, const char *name, bool init,
				  const TQRect *r = 0, TQt::Orientation orient = TQt::Horizontal );
    static TQWidget *createCustomWidget( TQWidget *parent, const char *name, MetaDataBase::CustomWidget *w );

    static TQGuardedPtr<TQObject> *lastPassiveInteractor;
    static bool lastWasAPassiveInteractor;
};


class TQDesignerTabWidget : public TQTabWidget
{
    TQ_OBJECT
    TQ_PROPERTY( int currentPage READ currentPage WRITE setCurrentPage STORED false DESIGNABLE true )
    TQ_PROPERTY( TQString pageTitle READ pageTitle WRITE setPageTitle STORED false DESIGNABLE true )
    TQ_PROPERTY( TQCString pageName READ pageName WRITE setPageName STORED false DESIGNABLE true )
public:
    TQDesignerTabWidget( TQWidget *parent, const char *name );

    int currentPage() const;
    void setCurrentPage( int i );
    TQString pageTitle() const;
    void setPageTitle( const TQString& title );
    TQCString pageName() const;
    void setPageName( const TQCString& name );

    int count() const;
    TQTabBar *tabBar() const { return TQTabWidget::tabBar(); }

    bool eventFilter( TQObject*, TQEvent* );

private:
    TQPoint pressPoint;
    TQWidget *dropIndicator;
    TQWidget *dragPage;
    TQString dragLabel;
     bool mousePressed;
};

class TQDesignerWidgetStack : public TQWidgetStack
{
    TQ_OBJECT
    TQ_PROPERTY( int currentPage READ currentPage WRITE setCurrentPage STORED false DESIGNABLE true )
    TQ_PROPERTY( TQCString pageName READ pageName WRITE setPageName STORED false DESIGNABLE true )
public:
    TQDesignerWidgetStack( TQWidget *parent, const char *name );

    int currentPage() const;
    void setCurrentPage( int i );
    TQCString pageName() const;
    void setPageName( const TQCString& name );

    int count() const;
    TQWidget* page( int i ) const;

    int insertPage( TQWidget *p, int i = -1 );
    int removePage( TQWidget *p );

public slots:
    void updateButtons();

protected:
    void resizeEvent( TQResizeEvent *e ) {
	TQWidgetStack::resizeEvent( e );
	updateButtons();
    }

    void showEvent( TQShowEvent *e ) {
	TQWidgetStack::showEvent( e );
	updateButtons();
    }

private slots:
    void prevPage();
    void nextPage();

private:
    TQPtrList<TQWidget> pages;
    TQToolButton *prev, *next;

};

class TQDesignerWizard : public TQWizard
{
    TQ_OBJECT
    TQ_PROPERTY( int currentPage READ currentPageNum WRITE setCurrentPage STORED false DESIGNABLE true )
    TQ_PROPERTY( TQString pageTitle READ pageTitle WRITE setPageTitle STORED false DESIGNABLE true )
    TQ_PROPERTY( TQCString pageName READ pageName WRITE setPageName STORED false DESIGNABLE true )
    TQ_OVERRIDE( bool modal READ isModal WRITE setModal )

public:
    TQDesignerWizard( TQWidget *parent, const char *name ) 
        : TQWizard( parent, name ), modal(false) {}

    int currentPageNum() const;
    void setCurrentPage( int i );
    TQString pageTitle() const;
    void setPageTitle( const TQString& title );
    TQCString pageName() const;
    void setPageName( const TQCString& name );
    int pageNum( TQWidget *page );
    void addPage( TQWidget *p, const TQString & );
    void removePage( TQWidget *p );
    void insertPage( TQWidget *p, const TQString &t, int index );
    bool isPageRemoved( TQWidget *p ) { return (removedPages.find( p ) != 0); }

    bool isModal() const { return modal; }
    void setModal(bool b) { modal = b; }

    void reject() {}

private:
    struct Page
    {
	Page( TQWidget *a, const TQString &b ) : p( a ), t( b ) {}
	Page() : p( 0 ), t( TQString::null ) {}
	TQWidget *p;
	TQString t;
    };
    TQPtrDict<TQWidget> removedPages;
    bool modal;

};

class TQLayoutWidget : public TQWidget
{
    TQ_OBJECT

public:
    TQLayoutWidget( TQWidget *parent, const char *name ) : TQWidget( parent, name ), sp( TQWidget::sizePolicy() ) {}

    TQSizePolicy sizePolicy() const;
    void updateSizePolicy();

protected:
    void paintEvent( TQPaintEvent * );
    bool event( TQEvent * );
    TQSizePolicy sp;

};


class CustomWidget : public TQWidget
{
    TQ_OBJECT

public:
    CustomWidget( TQWidget *parent, const char *name, MetaDataBase::CustomWidget *cw )
	: TQWidget( parent, name ), cusw( cw ) {
	    alwaysExpand = parentWidget() && parentWidget()->inherits( "FormWindow" );
	    setSizePolicy( cw->sizePolicy );
	    if ( !alwaysExpand )
		setBackgroundMode( PaletteDark );
    }

    TQSize sizeHint() const {
	TQSize sh = cusw->sizeHint;
	if ( sh.isValid() )
	    return sh;
	return TQWidget::sizeHint();
    }

    TQString realClassName() { return cusw->className; }
    MetaDataBase::CustomWidget *customWidget() const { return cusw; }

protected:
    void paintEvent( TQPaintEvent *e );

    MetaDataBase::CustomWidget *cusw;
    bool alwaysExpand;

};


class Line : public TQFrame
{
    TQ_OBJECT

    TQ_PROPERTY( Orientation orientation READ orientation WRITE setOrientation )
    TQ_OVERRIDE( int frameWidth DESIGNABLE false )
    TQ_OVERRIDE( Shape frameShape DESIGNABLE false )
    TQ_OVERRIDE( TQRect frameRect DESIGNABLE false )
    TQ_OVERRIDE( TQRect contentsRect DESIGNABLE false )
public:
    Line( TQWidget *parent, const char *name )
	: TQFrame( parent, name, WMouseNoMask ) {
	    setFrameStyle( HLine | Sunken );
    }

    void setOrientation( Orientation orient ) {
	if ( orient == Horizontal )
	    setFrameShape( HLine );
	else
	    setFrameShape( VLine );
    }
    Orientation orientation() const {
	return frameShape() == HLine ? Horizontal : Vertical;
    }
};

class TQDesignerLabel : public TQLabel
{
    TQ_OBJECT

    TQ_PROPERTY( TQCString buddy READ buddyWidget WRITE setBuddyWidget )

public:
    TQDesignerLabel( TQWidget *parent = 0, const char *name = 0 )
	: TQLabel( parent, name ) { myBuddy = 0; }

    void setBuddyWidget( const TQCString &b ) {
	myBuddy = b;
	updateBuddy();
    }
    TQCString buddyWidget() const {
	return myBuddy;
    };

protected:
    void showEvent( TQShowEvent *e ) {
	TQLabel::showEvent( e );
	updateBuddy();
    }


private:
    void updateBuddy();

    TQCString myBuddy;

};

class TQDesignerWidget : public TQWidget
{
    TQ_OBJECT

public:
    TQDesignerWidget( FormWindow *fw, TQWidget *parent, const char *name )
	: TQWidget( parent, name, WResizeNoErase ), formwindow( fw ) {
	    need_frame = parent && parent->inherits("TQDesignerWidgetStack" );
    }

protected:
    void resizeEvent( TQResizeEvent* e);
    void paintEvent( TQPaintEvent *e );

private:
    FormWindow *formwindow;
    uint need_frame : 1;

};

class TQDesignerDialog : public TQDialog
{
    TQ_OBJECT
    TQ_OVERRIDE( bool modal READ isModal WRITE setModal )

public:
    TQDesignerDialog( FormWindow *fw, TQWidget *parent, const char *name )
	: TQDialog( parent, name, false, WResizeNoErase ), formwindow( fw ), modal(false) {}

    bool isModal() const { return modal; }
    void setModal(bool b) { modal = b; }

protected:
    void paintEvent( TQPaintEvent *e );

private:
    FormWindow *formwindow;
    bool modal;

};

class TQDesignerToolButton : public TQToolButton
{
    TQ_OBJECT
    TQ_PROPERTY( int buttonGroupId READ buttonGroupId WRITE setButtonGroupId )

public:
    TQDesignerToolButton( TQWidget *parent, const char *name )
	: TQToolButton( parent, name ) {}

    bool isInButtonGroup() const {
	return parentWidget() && parentWidget()->inherits( "TQButtonGroup" );
    }
    int buttonGroupId() const {
	return parentWidget() && parentWidget()->inherits( "TQButtonGroup" ) ? ( (TQButtonGroup*)parentWidget() )->id( (TQButton*)this ) : -1;
    }
    void setButtonGroupId( int id ) {
	if ( parentWidget() && parentWidget()->inherits( "TQButtonGroup" ) ) {
	    ( (TQButtonGroup*)parentWidget() )->remove( this );
	    ( (TQButtonGroup*)parentWidget() )->insert( this, id );
	}
    }
};

class TQDesignerRadioButton : public TQRadioButton
{
    TQ_OBJECT
    TQ_PROPERTY( int buttonGroupId READ buttonGroupId WRITE setButtonGroupId )

public:
    TQDesignerRadioButton( TQWidget *parent, const char *name )
	: TQRadioButton( parent, name ) {}

    bool isInButtonGroup() const {
	return parentWidget() && parentWidget()->inherits( "TQButtonGroup" );
    }
    int buttonGroupId() const {
	return parentWidget() && parentWidget()->inherits( "TQButtonGroup" ) ? ( (TQButtonGroup*)parentWidget() )->id( (TQButton*)this ) : -1;
    }
    void setButtonGroupId( int id ) {
	if ( parentWidget() && parentWidget()->inherits( "TQButtonGroup" ) ) {
	    ( (TQButtonGroup*)parentWidget() )->remove( this );
	    ( (TQButtonGroup*)parentWidget() )->insert( this, id );
	}
    }

    void setFocusPolicy( FocusPolicy policy );
};

class TQDesignerPushButton : public TQPushButton
{
    TQ_OBJECT
    TQ_PROPERTY( int buttonGroupId READ buttonGroupId WRITE setButtonGroupId )

public:
    TQDesignerPushButton( TQWidget *parent, const char *name )
	: TQPushButton( parent, name ) {}

    bool isInButtonGroup() const {
	return parentWidget() && parentWidget()->inherits( "TQButtonGroup" );
    }
    int buttonGroupId() const {
	return parentWidget() && parentWidget()->inherits( "TQButtonGroup" ) ? ( (TQButtonGroup*)parentWidget() )->id( (TQButton*)this ) : -1;
    }
    void setButtonGroupId( int id ) {
	if ( parentWidget() && parentWidget()->inherits( "TQButtonGroup" ) ) {
	    ( (TQButtonGroup*)parentWidget() )->remove( this );
	    ( (TQButtonGroup*)parentWidget() )->insert( this, id );
	}
    }

};

class TQDesignerCheckBox : public TQCheckBox
{
    TQ_OBJECT
    TQ_PROPERTY( int buttonGroupId READ buttonGroupId WRITE setButtonGroupId )

public:
    TQDesignerCheckBox( TQWidget *parent, const char *name )
	: TQCheckBox( parent, name ) {}

    bool isInButtonGroup() const {
	return parentWidget() && parentWidget()->inherits( "TQButtonGroup" );
    }
    int buttonGroupId() const {
	return parentWidget() && parentWidget()->inherits( "TQButtonGroup" ) ? ( (TQButtonGroup*)parentWidget() )->id( (TQButton*)this ) : -1;
    }
    void setButtonGroupId( int id ) {
	if ( parentWidget() && parentWidget()->inherits( "TQButtonGroup" ) ) {
	    ( (TQButtonGroup*)parentWidget() )->remove( this );
	    ( (TQButtonGroup*)parentWidget() )->insert( this, id );
	}
    }

};

class TQDesignerToolBox : public TQToolBox
{
    TQ_OBJECT
    TQ_PROPERTY( TQString itemLabel READ itemLabel WRITE setItemLabel STORED false DESIGNABLE true )
    TQ_PROPERTY( TQCString itemName READ itemName WRITE setItemName STORED false DESIGNABLE true )
    TQ_PROPERTY( BackgroundMode itemBackgroundMode READ itemBackgroundMode WRITE setItemBackgroundMode STORED false DESIGNABLE true )

public:
    TQDesignerToolBox( TQWidget *parent, const char *name );

    TQString itemLabel() const;
    void setItemLabel( const TQString &l );
    TQCString itemName() const;
    void setItemName( const TQCString &n );
    BackgroundMode itemBackgroundMode() const;
    void setItemBackgroundMode( BackgroundMode );

protected:
    void itemInserted( int index );
};

#endif
