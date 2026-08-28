/****************************************************************************
**
** Implementation of TQWidgetPlugin class
**
** Created : 010920
**
** Copyright (C) 2001-2008 Trolltech ASA.  All rights reserved.
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

#include "ntqwidgetplugin.h"

#ifndef TQT_NO_WIDGETPLUGIN
#include "qwidgetinterface_p.h"
#include "ntqobjectcleanuphandler.h"
#include "ntqwidget.h"
#ifdef QT_CONTAINER_CUSTOM_WIDGETS
#include "ntqwidgetlist.h"
#endif

/*!
    \class TQWidgetPlugin ntqwidgetplugin.h
    \brief The TQWidgetPlugin class provides an abstract base for custom TQWidget plugins.

    \ingroup plugins

    The widget plugin is a simple plugin interface that makes it easy
    to create custom widgets that can be included in forms using \link
    designer-manual.book TQt Designer\endlink and used by applications.

    Writing a widget plugin is achieved by subclassing this base
    class, reimplementing the pure virtual functions keys(), create(),
    group(), iconSet(), includeFile(), toolTip(), whatsThis() and
    isContainer(), and exporting the class with the \c TQ_EXPORT_PLUGIN
    macro.

    See the \link designer-manual.book TQt Designer manual's\endlink,
    'Creating Custom Widgets' section in the 'Creating Custom Widgets'
    chapter, for a complete example of a TQWidgetPlugin.

    See also the \link plugins-howto.html Plugins
    documentation\endlink and the \l{TQWidgetFactory} class that is
    supplied with \link designer-manual.book TQt Designer\endlink.
*/

class TQWidgetPluginPrivate : public TQWidgetFactoryInterface,
#ifdef QT_CONTAINER_CUSTOM_WIDGETS
			     public TQWidgetContainerInterfacePrivate,
#endif
			     private TQLibraryInterface
{
public:
    TQWidgetPluginPrivate( TQWidgetPlugin *p )
	: plugin( p )
    {
    }

    virtual ~TQWidgetPluginPrivate();

    TQRESULT queryInterface( const TQUuid &iid, TQUnknownInterface **iface );
    TQ_REFCOUNT;

    TQStringList featureList() const;
    TQWidget *create( const TQString &key, TQWidget *parent, const char *name );
    TQString group( const TQString &widget ) const;
    TQIconSet iconSet( const TQString &widget ) const;
    TQString includeFile( const TQString &widget ) const;
    TQString toolTip( const TQString &widget ) const;
    TQString whatsThis( const TQString &widget ) const;
    bool isContainer( const TQString &widget ) const;
#ifdef QT_CONTAINER_CUSTOM_WIDGETS
    TQWidget* containerOfWidget( const TQString &key, TQWidget *widget ) const;
    bool isPassiveInteractor( const TQString &key, TQWidget *widget ) const;
    bool supportsPages( const TQString &key ) const;
    TQWidget *addPage( const TQString &key, TQWidget *container, const TQString &name, int index ) const;
    void insertPage( const TQString &key, TQWidget *container,
		     const TQString &name, int index, TQWidget *page ) const;
    void Page( const TQString &key, TQWidget *container,
	       const TQString &name, int index, TQWidget *page ) const;
    void removePage( const TQString &key, TQWidget *container, int index ) const;
    void movePage( const TQString &key, TQWidget *container, int fromIndex, int toIndex ) const;
    int count( const TQString &key, TQWidget *container ) const;
    int currentIndex( const TQString &key, TQWidget *container ) const;
    TQString pageLabel( const TQString &key, TQWidget *container, int index ) const;
    TQWidget *page( const TQString &key, TQWidget *container, int index ) const;
    void renamePage( const TQString &key, TQWidget *container, int index, const TQString &newName ) const;
    TQWidgetList pages( const TQString &key, TQWidget *container ) const;
    TQString createCode( const TQString &key, const TQString &container,
			const TQString &page, const TQString &pageName ) const;
#endif // QT_CONTAINER_CUSTOM_WIDGETS
    bool init();
    void cleanup();
    bool canUnload() const;

private:
    TQWidgetPlugin *plugin;
    TQObjectCleanupHandler widgets;
};

TQRESULT TQWidgetPluginPrivate::queryInterface( const TQUuid &iid, TQUnknownInterface **iface )
{
    *iface = 0;

    if ( iid == IID_QUnknown )
	*iface = (TQWidgetFactoryInterface*)this;
    else if ( iid == IID_QFeatureList )
	*iface = (TQFeatureListInterface*)this;
    else if ( iid == IID_QWidgetFactory )
	*iface = (TQWidgetFactoryInterface*)this;
    else if ( iid == IID_QLibrary )
	*iface = (TQLibraryInterface*)this;
#ifdef QT_CONTAINER_CUSTOM_WIDGETS
    else if ( iid == IID_QWidgetContainer )
	*iface = (TQWidgetContainerInterfacePrivate*)this;
#endif
    else
	return TQE_NOINTERFACE;

    (*iface)->addRef();
    return TQS_OK;
}

/*!
    \fn TQStringList TQWidgetPlugin::keys() const

    Returns the list of widget keys this plugin supports.

    These keys must be the class names of the custom widgets that are
    implemented in the plugin.

    \sa create()
*/

/*!
    \fn TQWidget *TQWidgetPlugin::create( const TQString &, TQWidget *, const char * )

    Creates and returns a TQWidget object for the widget key \a key.
    The widget key is the class name of the required widget. The \a
    name and \a parent arguments are passed to the custom widget's
    constructor.

    \sa keys()
*/

TQWidgetPluginPrivate::~TQWidgetPluginPrivate()
{
    delete plugin;
}

TQStringList TQWidgetPluginPrivate::featureList() const
{
    return plugin->keys();
}

TQWidget *TQWidgetPluginPrivate::create( const TQString &key, TQWidget *parent, const char *name )
{
    TQWidget *w = plugin->create( key, parent, name );
    widgets.add( w );
    return w;
}

TQString TQWidgetPluginPrivate::group( const TQString &widget ) const
{
    return plugin->group( widget );
}

TQIconSet TQWidgetPluginPrivate::iconSet( const TQString &widget ) const
{
    return plugin->iconSet( widget );
}

TQString TQWidgetPluginPrivate::includeFile( const TQString &widget ) const
{
    return plugin->includeFile( widget );
}

TQString TQWidgetPluginPrivate::toolTip( const TQString &widget ) const
{
    return plugin->toolTip( widget );
}

TQString TQWidgetPluginPrivate::whatsThis( const TQString &widget ) const
{
    return plugin->whatsThis( widget );
}

bool TQWidgetPluginPrivate::isContainer( const TQString &widget ) const
{
    return plugin->isContainer( widget );
}

bool TQWidgetPluginPrivate::init()
{
    return true;
}

void TQWidgetPluginPrivate::cleanup()
{
    widgets.clear();
}

bool TQWidgetPluginPrivate::canUnload() const
{
    return widgets.isEmpty();
}

#ifdef QT_CONTAINER_CUSTOM_WIDGETS
TQWidget* TQWidgetPluginPrivate::containerOfWidget( const TQString &key, TQWidget *widget ) const
{
    TQWidgetContainerPlugin *p = (TQWidgetContainerPlugin*)plugin->tqt_cast( "TQWidgetContainerPlugin" );
    if ( p )
	return p->containerOfWidget( key, widget );
    return widget;
}

int TQWidgetPluginPrivate::count( const TQString &key, TQWidget *container ) const
{
    TQWidgetContainerPlugin *p = (TQWidgetContainerPlugin*)plugin->tqt_cast( "TQWidgetContainerPlugin" );
    if ( p )
	return p->count( key, container );
    return 0;
}

int TQWidgetPluginPrivate::currentIndex( const TQString &key, TQWidget *container ) const
{
    TQWidgetContainerPlugin *p = (TQWidgetContainerPlugin*)plugin->tqt_cast( "TQWidgetContainerPlugin" );
    if ( p )
	return p->currentIndex( key, container );
    return -1;
}

TQString TQWidgetPluginPrivate::pageLabel( const TQString &key, TQWidget *container, int index ) const
{
    TQWidgetContainerPlugin *p = (TQWidgetContainerPlugin*)plugin->tqt_cast( "TQWidgetContainerPlugin" );
    if ( p )
	return p->pageLabel( key, container, index );
    return TQString::null;
}

TQWidget *TQWidgetPluginPrivate::page( const TQString &key, TQWidget *container, int index ) const
{
    TQWidgetContainerPlugin *p = (TQWidgetContainerPlugin*)plugin->tqt_cast( "TQWidgetContainerPlugin" );
    if ( p )
	return p->page( key, container, index );
    return 0;
}

bool TQWidgetPluginPrivate::isPassiveInteractor( const TQString &key, TQWidget *widget ) const
{
    TQWidgetContainerPlugin *p = (TQWidgetContainerPlugin*)plugin->tqt_cast( "TQWidgetContainerPlugin" );
    if ( p )
	return p->isPassiveInteractor( key, widget );
    return false;
}

bool TQWidgetPluginPrivate::supportsPages( const TQString &key ) const
{
    TQWidgetContainerPlugin *p = (TQWidgetContainerPlugin*)plugin->tqt_cast( "TQWidgetContainerPlugin" );
    if ( p )
	return p->supportsPages( key );
    return 0;
}

TQWidget *TQWidgetPluginPrivate::addPage( const TQString &key, TQWidget *container,
				    const TQString &name, int index ) const
{
    TQWidgetContainerPlugin *p = (TQWidgetContainerPlugin*)plugin->tqt_cast( "TQWidgetContainerPlugin" );
    if ( p )
	return p->addPage( key, container, name, index );
    return 0;
}

void TQWidgetPluginPrivate::insertPage( const TQString &key, TQWidget *container,
				       const TQString &name, int index, TQWidget *page ) const
{
    TQWidgetContainerPlugin *p = (TQWidgetContainerPlugin*)plugin->tqt_cast( "TQWidgetContainerPlugin" );
    if ( p )
	p->insertPage( key, container, name, index, page );
}

void TQWidgetPluginPrivate::removePage( const TQString &key, TQWidget *container, int index ) const
{
    TQWidgetContainerPlugin *p = (TQWidgetContainerPlugin*)plugin->tqt_cast( "TQWidgetContainerPlugin" );
    if ( p )
	p->removePage( key, container, index );
}

void TQWidgetPluginPrivate::movePage( const TQString &key, TQWidget *container,
				     int fromIndex, int toIndex ) const
{
    TQWidgetContainerPlugin *p = (TQWidgetContainerPlugin*)plugin->tqt_cast( "TQWidgetContainerPlugin" );
    if ( p )
	p->movePage( key, container, fromIndex, toIndex );
}

void TQWidgetPluginPrivate::renamePage( const TQString &key, TQWidget *container,
				      int index, const TQString &newName ) const
{
    TQWidgetContainerPlugin *p = (TQWidgetContainerPlugin*)plugin->tqt_cast( "TQWidgetContainerPlugin" );
    if ( p )
	p->renamePage( key, container, index, newName );
}

TQWidgetList TQWidgetPluginPrivate::pages( const TQString &key, TQWidget *container ) const
{
    TQWidgetContainerPlugin *p = (TQWidgetContainerPlugin*)plugin->tqt_cast( "TQWidgetContainerPlugin" );
    if ( p )
	return p->pages( key, container );
    return TQWidgetList();
}

TQString TQWidgetPluginPrivate::createCode( const TQString &key, const TQString &container,
					  const TQString &page, const TQString &pageName ) const
{
    TQWidgetContainerPlugin *p = (TQWidgetContainerPlugin*)plugin->tqt_cast( "TQWidgetContainerPlugin" );
    if ( p )
	return p->createCode( key, container, page, pageName );
    return TQString::null;
}
#endif // QT_CONTAINER_CUSTOM_WIDGETS

/*!
    Constructs a widget plugin. This is invoked automatically by the
    \c TQ_EXPORT_PLUGIN macro.
*/
TQWidgetPlugin::TQWidgetPlugin()
    : TQGPlugin( (TQWidgetFactoryInterface*)(d = new TQWidgetPluginPrivate( this )) )
{
}

/*!
    Destroys the widget plugin.

    You never have to call this explicitly. TQt destroys a plugin
    automatically when it is no longer used.
*/
TQWidgetPlugin::~TQWidgetPlugin()
{
    // don't delete d, as this is deleted by d
}

/*!
    Returns the group (toolbar name) that the custom widget of class
    \a key should be part of when \e{TQt Designer} loads it.

    The default implementation returns TQString::null.
*/
TQString TQWidgetPlugin::group( const TQString & ) const
{
    return TQString::null;
}

/*!
    Returns the iconset that \e{TQt Designer} should use to represent
    the custom widget of class \a key in the toolbar.

    The default implementation returns an null iconset.
*/
TQIconSet TQWidgetPlugin::iconSet( const TQString & ) const
{
    return TQIconSet();
}

/*!
    Returns the name of the include file that \e{TQt Designer} and \c
    uic should use to include the custom widget of class \a key in
    generated code.

    The default implementation returns TQString::null.
*/
TQString TQWidgetPlugin::includeFile( const TQString & ) const
{
    return TQString::null;
}

/*!
    Returns the text of the tooltip that \e{TQt Designer} should use
    for the custom widget of class \a key's toolbar button.

    The default implementation returns TQString::null.
*/
TQString TQWidgetPlugin::toolTip( const TQString & ) const
{
    return TQString::null;
}

/*!
    Returns the text of the whatsThis text that \e{TQt Designer} should
    use when the user requests whatsThis help for the custom widget of
    class \a key.

    The default implementation returns TQString::null.
*/
TQString TQWidgetPlugin::whatsThis( const TQString & ) const
{
    return TQString::null;
}

/*!
    Returns true if the custom widget of class \a key can contain
    other widgets, e.g. like TQFrame; otherwise returns false.

    The default implementation returns false.
*/
bool TQWidgetPlugin::isContainer( const TQString & ) const
{
    return false;
}

#ifdef QT_CONTAINER_CUSTOM_WIDGETS

/*!
    \class TQWidgetContainerPlugin ntqwidgetplugin.h
    \brief The TQWidgetContainerPlugin class provides an abstract base
    for complex custom container TQWidget plugins.

    \internal

    \ingroup plugins

    The widget container plugin is a subclass of TQWidgetPlugin and
    extends the interface with functions necessary for supporting
    complex container widgets via plugins. These container widgets are
    widgets that have one or multiple sub widgets which act as the
    widget's containers. If the widget has multiple container
    subwidgets, they are referred to as "pages", and only one can be
    active at a time. Examples of complex container widgets include:
    TQTabWidget, TQWidgetStack and TQToolBox.

    Writing a complex container widget plugin is achieved by
    subclassing this base class. First by reimplementing
    TQWidgetPlugin's pure virtual functions keys(), create(), group(),
    iconSet(), includeFile(), toolTip(), whatsThis() and
    isContainer(), and exporting the class with the \c TQ_EXPORT_PLUGIN
    macro. In addition containerOfWidget(), isPassiveInteractor() and
    supportsPages() must be reimplemented. If the widget
    supportsPages(), count(), currentIndex(), pageLabel(), page(),
    pages() and createCode() must be implemented. If the widget
    supportsPages() and you want to allow the containers pages to be
    modified, you must also reimplement addPage(), insertPage(),
    removePage(), movePage() and renamePage().

    \sa TQWidgetPlugin
*/

/*!
    Constructs a complex container widget plugin. This is invoked
    automatically by the \c TQ_EXPORT_PLUGIN macro.
*/

TQWidgetContainerPlugin::TQWidgetContainerPlugin()
    : TQWidgetPlugin()
{
}

/*!
    Destroys the complex container widget plugin.

    You never have to call this explicitly. TQt destroys a plugin
    automatically when it is no longer used.
*/

TQWidgetContainerPlugin::~TQWidgetContainerPlugin()
{
}

/*!
    Operates on the plugin's \a key class.

    Returns the current \a container's custom widget. If the custom
    widget is a tab widget, this function takes the \a container as
    input and returns the widget's current page.

    The default implementation returns \a container.
*/

TQWidget* TQWidgetContainerPlugin::containerOfWidget( const TQString &,
						    TQWidget *container ) const
{
    return container;
}

/*!
    Operates on the plugin's \a key class.

    Returns the \a container custom widget's number of pages. If the
    custom widget is a tab widget, this function returns the number of
    tabs.

    The default implementation returns 0.
*/

int TQWidgetContainerPlugin::count( const TQString &, TQWidget * ) const
{
    return 0;
}

/*!
    Operates on the plugin's \a key class.

    Returns the \a container custom widget's current page index. If
    the custom widget is a tab widget, this function returns the
    current tab's index.

    The default implementation returns -1.
*/

int TQWidgetContainerPlugin::currentIndex( const TQString &, TQWidget * ) const
{
    return -1;
}

/*!
    Operates on the plugin's \a key class.

    Returns the \a container custom widget's label at position \a
    index. If the custom widget is a tab widget, this function returns
    the current tab's label.

    The default implementation returns a null string.
*/

TQString TQWidgetContainerPlugin::pageLabel( const TQString &, TQWidget *, int ) const
{
    return TQString::null;
}

/*!
    Operates on the plugin's \a key class.

    Returns the \a container custom widget's page at position \a
    index. If the custom widget is a tab widget, this function returns
    the tab at index position \e index.


    The default implementation returns 0.
*/

TQWidget *TQWidgetContainerPlugin::page( const TQString &, TQWidget *, int ) const
{
    return 0;
}

/*!
    Operates on the plugin's \a key class.

    Returns true if the \a container custom widget is a passive
    interactor for class \e key; otherwise returns false. The \a
    container is a child widget of the actual custom widget.

    Usually, when a custom widget is used in \e{TQt Designer}'s design
    mode, no widget receives any mouse or key events, since \e{TQt
    Designer} filters and processes them itself. If one or more
    widgets of a custom widget still need to receive such events, for
    example, because the widget needs to change pages, this function
    must return true for the widget. In such cases \e{TQt Designer}
    will not filter out key and mouse events destined for the widget.

    If the custom widget is a tab widget, the tab bar is the passive
    interactor, since that's what the user will use to change pages.

    The default implementation returns false.
*/

bool TQWidgetContainerPlugin::isPassiveInteractor( const TQString &,
						 TQWidget *container ) const
{
    Q_UNUSED( container )
    return false;
}

/*!
    Operates on the plugin's \a key class.

    Returns true if the widget supports pages; otherwise returns
    false. If the custom widget is a tab widget this function should
    return true.

    The default implementation returns false.
*/

bool TQWidgetContainerPlugin::supportsPages( const TQString & ) const
{
    return false;
}

/*!
    Operates on the plugin's \a key class.

    This function is called when a new page with the given \a name
    should be added to the \a container custom widget at position \a
    index.

    The default implementation does nothing.
*/

TQWidget* TQWidgetContainerPlugin::addPage( const TQString &, TQWidget *,
					  const TQString &, int ) const
{
    return 0;
}

/*!
    Operates on the plugin's \a key class.

    This function is called when a new page, \a page, with the given
    \a name should be added to the \a container custom widget at
    position \a index.

    The default implementation does nothing.
*/

void TQWidgetContainerPlugin::insertPage( const TQString &, TQWidget *,
					 const TQString &, int, TQWidget * ) const
{
}

/*!
    Operates on the plugin's \a key class.

    This function is called when the page at position \a index should
    be removed from the \a container custom widget.

    The default implementation does nothing.
*/

void TQWidgetContainerPlugin::removePage( const TQString &, TQWidget *, int ) const
{
}

/*!
    Operates on the plugin's \a key class.

    This function is called when the page at position \a fromIndex should
    be moved to position \a toIndex in the \a container custom widget.

    The default implementation does nothing.
*/

void TQWidgetContainerPlugin::movePage( const TQString &, TQWidget *, int, int ) const
{
}

/*!
    Operates on the plugin's \a key class.

    This function is called when the page at position \a index should
    be renamed (have its label changed) to \a newName in the \a
    container custom widget.

    The default implementation does nothing.
*/

void TQWidgetContainerPlugin::renamePage( const TQString &, TQWidget *,
					 int, const TQString & ) const
{
}

/*!
    Operates on the plugin's \a key class.

    This function should return a list of the \a container custom
    widget's pages.
*/

TQWidgetList TQWidgetContainerPlugin::pages( const TQString &, TQWidget * ) const
{
    return TQWidgetList();
}

/*!
    Operates on the plugin's \a key class.

    This function is called from \e{TQt Designer}'s User Interface
    Compiler \c uic, when generating C++ code for inserting a page in
    the \a container custom widget. The name of the page widget which
    should be inserted at the end of the container is \a page, and the
    label of the page should be \a pageName.

    If the custom widget was a TQTabWidget, the implementation of this
    function should return:

    \code
    return widget + "->addTab( " + page + ", \"" + pageName + "\" )";
    \endcode

    Warning: If the code returned by this function contains invalid
    C++ syntax, the generated \c uic code will not compile.
*/

TQString TQWidgetContainerPlugin::createCode( const TQString &, const TQString &,
					    const TQString &, const TQString & ) const
{
    return TQString::null;
}

#endif // QT_CONTAINER_CUSTOM_WIDGETS

#endif //TQT_NO_WIDGETPLUGIN
