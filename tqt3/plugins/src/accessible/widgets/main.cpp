#include "qaccessiblewidget.h"
#include "qaccessiblemenu.h"

#include <ntqtoolbutton.h>
#include <ntqtoolbar.h>
#include <ntqvariant.h>

class AccessibleFactory : public TQAccessibleFactoryInterface, public TQLibraryInterface
{
public:
    AccessibleFactory();

    TQRESULT queryInterface( const TQUuid &, TQUnknownInterface **iface );
    TQ_REFCOUNT

    TQStringList featureList() const;
    TQRESULT createAccessibleInterface( const TQString &classname, TQObject *object, TQAccessibleInterface **iface );

    bool init();
    void cleanup();
    bool canUnload() const;
};

AccessibleFactory::AccessibleFactory()
{
}

TQRESULT AccessibleFactory::queryInterface( const TQUuid &iid, TQUnknownInterface **iface )
{
    *iface = 0;
    if ( iid == IID_QUnknown )
	*iface = (TQUnknownInterface*)(TQFeatureListInterface*)(TQAccessibleFactoryInterface*)this;
    else if ( iid == IID_QFeatureList )
	*iface = (TQFeatureListInterface*)this;
    else if ( iid == IID_QAccessibleFactory )
	*iface = (TQAccessibleFactoryInterface*)this;
    else if ( iid == IID_QLibrary )
	*iface = (TQLibraryInterface*)this;
    else
	return TQE_NOINTERFACE;

    (*iface)->addRef();
    return TQS_OK;
}

TQStringList AccessibleFactory::featureList() const
{
    TQStringList list;
    list << "TQLineEdit";
    list << "TQComboBox";
    list << "TQSpinBox";
    list << "TQSpinWidget";
    list << "TQDial";
    list << "TQScrollBar";
    list << "TQSlider";
    list << "TQToolButton";
    list << "TQCheckBox";
    list << "TQRadioButton";
    list << "TQButton";
    list << "TQViewportWidget";
    list << "TQClipperWidget";
    list << "TQTextEdit";
#ifndef TQT_NO_ICONVIEW
    list << "TQIconView";
#endif
    list << "TQListView";
    list << "TQListBox";
    list << "TQTable";
    list << "TQDialog";
    list << "TQMessageBox";
    list << "TQMainWindow";
    list << "TQLabel";
    list << "TQGroupBox";
    list << "TQStatusBar";
    list << "TQProgressBar";
    list << "TQToolBar";
    list << "TQMenuBar";
    list << "TQPopupMenu";
    list << "TQHeader";
    list << "TQTabBar";
    list << "TQTitleBar";
    list << "TQWorkspaceChild";
    list << "TQSizeGrip";
    list << "TQSplitterHandle";
    list << "TQToolBarSeparator";
    list << "TQDockWindowHandle";
    list << "TQDockWindowResizeHandle";
    list << "TQTipLabel";
    list << "TQFrame";
    list << "TQWidgetStack";
    list << "TQWidget";
    list << "TQScrollView";

    return list;
}

TQRESULT AccessibleFactory::createAccessibleInterface( const TQString &classname, TQObject *object, TQAccessibleInterface **iface )
{
    *iface = 0;

    if ( classname == "TQLineEdit" ) {
	*iface = new TQAccessibleText( object, EditableText );
    } else if ( classname == "TQComboBox" ) {
	*iface = new TQAccessibleComboBox( object );
    } else if ( classname == "TQSpinBox" ) {
	*iface = new TQAccessibleRangeControl( object, SpinBox );
    } else if ( classname == "TQSpinWidget" ) {
	*iface = new TQAccessibleSpinWidget( object );
    } else if ( classname == "TQDial" ) {
	*iface = new TQAccessibleRangeControl( object, Dial );
    } else if ( classname == "TQScrollBar" ) {
	*iface = new TQAccessibleScrollBar( object );
    } else if ( classname == "TQSlider" ) {
	*iface = new TQAccessibleSlider( object );
    } else if ( classname == "TQToolButton" ) {
	TQToolButton *tb = (TQToolButton*)object;
	if ( !tb->popup() )
	    *iface = new TQAccessibleButton( object, PushButton );
	else if ( !tb->popupDelay() )
	    *iface = new TQAccessibleButton( object, ButtonDropDown );
	else
	    *iface = new TQAccessibleButton( object, ButtonMenu );
    } else if ( classname == "TQCheckBox" ) {
	*iface = new TQAccessibleButton( object, CheckBox );
    } else if ( classname == "TQRadioButton" ) {
	*iface = new TQAccessibleButton( object, RadioButton );
    } else if ( classname == "TQButton" ) {
	*iface = new TQAccessibleButton( object, PushButton );
    } else if ( classname == "TQViewportWidget" ) {
	*iface = new TQAccessibleViewport( object, object->parent() );
    } else if ( classname == "TQClipperWidget" ) {
	*iface = new TQAccessibleViewport( object, object->parent()->parent() );
    } else if ( classname == "TQTextEdit" ) {
	*iface = new TQAccessibleTextEdit( object );
#ifndef TQT_NO_ICONVIEW
    } else if ( classname == "TQIconView" ) {
	*iface = new TQAccessibleIconView( object );
#endif
    } else if ( classname == "TQListView" ) {
	*iface = new TQAccessibleListView( object );
    } else if ( classname == "TQListBox" ) {
	*iface = new TQAccessibleListBox( object );
    } else if ( classname == "TQTable" ) {
	*iface = new TQAccessibleScrollView( object, Table );
    } else if ( classname == "TQDialog" ) {
	*iface = new TQAccessibleWidget( object, Dialog );
    } else if ( classname == "TQMessageBox" ) {
	*iface = new TQAccessibleWidget( object, AlertMessage );
    } else if ( classname == "TQMainWindow" ) {
	*iface = new TQAccessibleWidget( object, Application );
    } else if ( classname == "TQLabel" || classname == "TQLCDNumber" ) {
	*iface = new TQAccessibleDisplay( object, StaticText );
    } else if ( classname == "TQGroupBox" ) {
	*iface = new TQAccessibleDisplay( object, Grouping );
    } else if ( classname == "TQStatusBar" ) {
	*iface = new TQAccessibleWidget( object, StatusBar );
    } else if ( classname == "TQProgressBar" ) {
	*iface = new TQAccessibleRangeControl( object, ProgressBar );
    } else if ( classname == "TQToolBar" ) {
	TQToolBar *tb = (TQToolBar*)object;
	*iface = new TQAccessibleWidget( object, ToolBar, tb->label() );
    } else if ( classname == "TQMenuBar" ) {
	*iface = new TQAccessibleMenuBar( object );
    } else if ( classname == "TQPopupMenu" ) {
	*iface = new TQAccessiblePopup( object );
    } else if ( classname == "TQHeader" ) {
	*iface = new TQAccessibleHeader( object );
    } else if ( classname == "TQTabBar" ) {
	*iface = new TQAccessibleTabBar( object );
    } else if ( classname == "TQTitleBar" ) {
	*iface = new TQAccessibleTitleBar( object );
    } else if ( classname == "TQWorkspaceChild" ) {
	*iface = new TQAccessibleWidget( object, Window );
    } else if ( classname == "TQSizeGrip" ) {
	*iface = new TQAccessibleWidget( object, Grip );
    } else if ( classname == "TQSplitterHandle" ) {
	*iface = new TQAccessibleWidget( object, Separator, TQString::null, 
					TQString::null, TQString::null, TQString::null, 
					TQString::null, TQString::null, TQAccessible::Moveable );
    } else if ( classname == "TQToolBarSeparator" ) {
	*iface = new TQAccessibleWidget( object, TQAccessible::Separator, TQString::null, 
					TQString::null, TQString::null, TQString::null, 
					TQString::null, TQString::null, TQAccessible::Unavailable );
    } else if ( classname == "TQDockWindowHandle" ) {
	*iface = new TQAccessibleWidget( object, TQAccessible::Grip, object->property( "caption" ).toString(),
					TQString::null, TQString::null, TQString::null,
					TQString::null, TQString::null, TQAccessible::Moveable );
    } else if ( classname == "TQDockWindowResizeHandle" ) {
	*iface = new TQAccessibleWidget( object, TQAccessible::Separator, TQString::null,
					TQString::null, TQString::null, TQString::null,
					TQString::null, TQString::null, TQAccessible::Moveable );
    } else if ( classname == "TQTipLabel" ) {
	*iface = new TQAccessibleWidget( object, ToolTip );
    } else if ( classname == "TQFrame" ) {
	*iface = new TQAccessibleWidget( object, Border );
    } else if ( classname == "TQWidgetStack" ) {
	*iface = new TQAccessibleWidgetStack( object );
    } else if ( classname == "TQWidget" ) {
	*iface = new TQAccessibleWidget( object );
    } else if ( classname == "TQScrollView" ) {
        *iface = new TQAccessibleScrollView( object, Client );
    } else
	return TQE_NOINTERFACE;

    (*iface)->addRef();
    return TQS_OK;
}

bool AccessibleFactory::init()
{
    return true;
}

void AccessibleFactory::cleanup()
{
}

bool AccessibleFactory::canUnload() const
{
    return (TQAccessibleWidget::objects == 0);
}

TQ_EXPORT_COMPONENT()
{
    Q_CREATE_INSTANCE( AccessibleFactory )
}
