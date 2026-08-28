/**********************************************************************
** Copyright (C) 2000-2008 Trolltech ASA.  All rights reserved.
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

#include "ntqwidgetfactory.h"

#include "../interfaces/languageinterface.h"
#include "../interfaces/widgetinterface.h"

#include <ntqfeatures.h>
#include "../designer/database2.h"
#include <ntqdom.h>
#include <ntqdir.h>
#include <ntqlayout.h>
#include <ntqmetaobject.h>
#include <domtool.h>
#include <uib.h>
#include <ntqapplication.h>
#include <ntqtooltip.h>
#include <ntqwhatsthis.h>
#include <ntqobjectlist.h>
#include <private/qpluginmanager_p.h>
#include <ntqmime.h>
#include <ntqdragobject.h>

#ifndef TQT_NO_SQL
#include <ntqsqlrecord.h>
#include <ntqsqldatabase.h>
#include <ntqdatatable.h>
#endif

// include all TQt widgets we support
#include <ntqpushbutton.h>
#include <ntqtoolbutton.h>
#include <ntqcheckbox.h>
#include <ntqradiobutton.h>
#include <ntqgroupbox.h>
#include <ntqbuttongroup.h>
#include <ntqiconview.h>
#include <ntqheader.h>
#ifndef TQT_NO_TABLE
#include <ntqtable.h>
#endif
#include <ntqlistbox.h>
#include <ntqlistview.h>
#include <ntqlineedit.h>
#include <ntqspinbox.h>
#include <ntqmultilineedit.h>
#include <ntqlabel.h>
#include <ntqwidget.h>
#include <ntqtabwidget.h>
#include <ntqcombobox.h>
#include <ntqdialog.h>
#include <ntqwizard.h>
#include <ntqlcdnumber.h>
#include <ntqprogressbar.h>
#include <ntqtextview.h>
#include <ntqtextbrowser.h>
#include <ntqdial.h>
#include <ntqslider.h>
#include <ntqframe.h>
#include <ntqwidgetstack.h>
#include <ntqtoolbox.h>
#include <ntqtextedit.h>
#include <ntqscrollbar.h>
#include <ntqmainwindow.h>
#include <ntqsplitter.h>
#include <ntqaction.h>
#include <ntqpopupmenu.h>
#include <ntqmenubar.h>
#include <ntqpopupmenu.h>
#include <ntqdatetimeedit.h>
#include <ntqvbox.h>
#include <ntqhbox.h>
#include <ntqgrid.h>

#include <stdlib.h>

class TQWidgetFactoryPrivate
{
public:
    TQCString translationContext;
    TQListViewItem *lastItem;
    TQDict<bool> customWidgets;
};

static TQPtrList<TQWidgetFactory> widgetFactories;
static TQPluginManager<LanguageInterface> *languageInterfaceManager = 0;
static TQPluginManager<WidgetInterface> *widgetInterfaceManager = 0;

static TQMap<TQString, bool> *availableWidgetMap = 0;
static TQStringList *availableWidgetList = 0;

TQ_EXPORT TQMap<TQWidget*, TQString> *qwf_forms = 0;
TQString *qwf_language = 0;
TQ_EXPORT bool qwf_execute_code = true;
bool qwf_stays_on_top = false;
TQString qwf_currFileName = "";
TQObject *qwf_form_object = 0;
TQ_EXPORT TQString *qwf_plugin_dir = 0;

static void setupPluginDir()
{
    if ( !qwf_plugin_dir )
	qwf_plugin_dir = new TQString( "/designer" );
}

static void setupWidgetListAndMap()
{
    if ( availableWidgetMap )
	return;
    setupPluginDir();
    availableWidgetList = new TQStringList;
    (*availableWidgetList) << "TQPushButton" << "TQToolButton" << "TQCheckBox" << "TQRadioButton"
			   << "TQGroupBox" << "TQButtonGroup" << "TQIconView" << "TQTable"
			   << "TQListBox" << "TQListView" << "TQLineEdit" << "TQSpinBox"
			   << "TQMultiLineEdit" << "TQLabel" << "TextLabel" << "PixmapLabel"
			   << "TQLayoutWidget" << "TQTabWidget" << "TQComboBox"
			   << "TQWidget" << "TQDialog" << "TQWizard" << "TQLCDNumber";
    // put less stress on the compiler by limiting the template nesting depth
    (*availableWidgetList) << "TQProgressBar" << "TQTextView" << "TQTextBrowser"
			   << "TQDial" << "TQSlider" << "TQFrame" << "Line" << "TQTextEdit"
			   << "TQDateEdit" << "TQTimeEdit" << "TQDateTimeEdit" << "TQScrollBar"
			   << "TQPopupMenu" << "TQWidgetStack" << "TQMainWindow"
			   << "TQDataTable" << "TQDataBrowser" << "TQDataView"
			   << "TQVBox" << "TQHBox" << "TQGrid" << "TQToolBox" << "TQSplitter";

    if ( !widgetInterfaceManager )
	widgetInterfaceManager =
	    new TQPluginManager<WidgetInterface>( IID_Widget, TQApplication::libraryPaths(), *qwf_plugin_dir );

    TQStringList l = widgetInterfaceManager->featureList();
    TQStringList::Iterator it;
    for ( it = l.begin(); it != l.end(); ++it )
	(*availableWidgetList) << *it;

    availableWidgetMap = new TQMap<TQString, bool>;
    for ( it = availableWidgetList->begin(); it != availableWidgetList->end(); ++it )
	availableWidgetMap->insert( *it, true );
}

static TQImage loadImageData( const TQString& format, ulong len, TQByteArray data )
{
    TQImage img;
    if ( format == "XPM.GZ" || format == "XBM.GZ" ) {
	if ( len < data.size() * 10 )
	    len = data.size() * 10;
	// tqUncompress() expects the first 4 bytes to be the expected length of
	// the uncompressed data
	TQByteArray dataTmp( data.size() + 4 );
	memcpy( dataTmp.data()+4, data.data(), data.size() );
	dataTmp[0] = ( len & 0xff000000 ) >> 24;
	dataTmp[1] = ( len & 0x00ff0000 ) >> 16;
	dataTmp[2] = ( len & 0x0000ff00 ) >> 8;
	dataTmp[3] = ( len & 0x000000ff );
	TQByteArray baunzip = tqUncompress( dataTmp );
	len = baunzip.size();
	img.loadFromData( (const uchar*)baunzip.data(), len, format.left(format.find('.')) );
    } else {
	img.loadFromData( (const uchar*)data.data(), data.size(), format );
    }
    return img;
}

static TQSizePolicy::SizeType stringToSizeType( const TQString& str )
{
    if ( str == "Fixed" ) {
	return TQSizePolicy::Fixed;
    } else if ( str == "Minimum" ) {
	return TQSizePolicy::Minimum;
    } else if ( str == "Maximum" ) {
	return TQSizePolicy::Maximum;
    } else if ( str == "Preferred" ) {
	return TQSizePolicy::Preferred;
    } else if ( str == "MinimumExpanding" ) {
	return TQSizePolicy::MinimumExpanding;
    } else if ( str == "Expanding" ) {
	return TQSizePolicy::Expanding;
    } else {
	return TQSizePolicy::Ignored;
    }
}


/*!
  \class TQWidgetFactory

  \brief The TQWidgetFactory class provides for the dynamic creation of widgets
  from TQt Designer .ui files.

  This class basically offers two things:

  \list

  \i Dynamically creating widgets from \link designer-manual.book TQt
  Designer\endlink user interface description files.
  You can do this using the static function TQWidgetFactory::create().
  This function also performs signal and slot connections, tab
  ordering, etc., as defined in the .ui file, and returns the
  top-level widget in the .ui file. After creating the widget you can
  use TQObject::child() and TQObject::queryList() to access child
  widgets of this returned widget.

  \i Adding additional widget factories to be able to create custom
  widgets. See createWidget() for details.

  \endlist

  This class is not included in the TQt library itself. To use it you
  must link against \c libtqui.so (Unix) or \c tqui.lib (Windows), which is
  built into \c INSTALL/lib if you built \e{TQt Designer} (\c INSTALL is
  the directory where TQt is installed ).

  If you create a TQMainWindow using a TQWidgetFactory, be aware that
  it already has a central widget. Therefore, you need to delete this
  one before setting another one.

  See the "Creating Dynamic Dialogs from .ui Files" section of the \link
  designer-manual.book TQt Designer manual\endlink for an example. See
  also the \l{TQWidgetPlugin} class and the \link plugins-howto.html
  Plugins documentation\endlink.
*/

/*! Constructs a TQWidgetFactory. */

TQWidgetFactory::TQWidgetFactory()
    : d( new TQWidgetFactoryPrivate() ), dbControls( 0 ),
      usePixmapCollection( false ), defMargin( 11 ), defSpacing( 6 )
{
    widgetFactories.setAutoDelete( true );
    d->customWidgets.setAutoDelete( true );
}

/*! \fn TQWidgetFactory::~TQWidgetFactory()
    Destructor.
*/
TQWidgetFactory::~TQWidgetFactory()
{
    delete d;
}

/*!

    Loads the \e{TQt Designer} user interface description file \a uiFile
  and returns the top-level widget in that description. \a parent and
  \a name are passed to the constructor of the top-level widget.

  This function also performs signal and slot connections, tab
  ordering, etc., as described in the .ui file. In \e{TQt Designer} it
  is possible to add custom slots to a form and connect to them. If
  you want these connections to be made, you must create a class
  derived from TQObject, which implements all these slots. Then pass an
  instance of the object as \a connector to this function. If you do
  this, the connections to the custom slots will be done using the \a
  connector as slot.

  If something fails, 0 is returned.

  The ownership of the returned widget is passed to the caller.
*/

TQWidget *TQWidgetFactory::create( const TQString &uiFile, TQObject *connector,
				 TQWidget *parent, const char *name )
{
    setupPluginDir();
    TQFile f( uiFile );
    bool failed = false;
    if ( !f.open( IO_ReadOnly ) )
	failed = true;
    if ( failed && tqApp->type() == TQApplication::Tty ) {
	// for TQSA: If we have no GUI, we have no form definition
	// files, but just the code. So try if only the code exists.
	f.setName( uiFile + ".qs" );
	failed = !f.open( IO_ReadOnly );
    }
    if ( failed )
	return 0;

    qwf_currFileName = uiFile;
    TQWidget *w = TQWidgetFactory::create( &f, connector, parent, name );
    if ( !qwf_forms )
	qwf_forms = new TQMap<TQWidget*, TQString>;
    qwf_forms->insert( w, uiFile );
    return w;
}

#undef slots

/*!  \overload
    Loads the user interface description from device \a dev.
 */

TQWidget *TQWidgetFactory::create( TQIODevice *dev, TQObject *connector, TQWidget *parent, const char *name )
{
    setupPluginDir();
    TQWidget *w = 0;
    TQDomDocument doc;
    TQString errMsg;
    int errLine;

    TQWidgetFactory *widgetFactory = new TQWidgetFactory;
    widgetFactory->toplevel = 0;

    // If we have no GUI, we only want to load the code
    if ( tqApp->type() != TQApplication::Tty ) {
	TQIODevice::Offset start = dev->at();
	TQ_UINT32 magic;
	TQDataStream in( dev );
	in >> magic;
	if ( magic == UibMagic ) {
	    w = widgetFactory->createFromUibFile( in, connector, parent, name );
	} else {
	    in.unsetDevice();
	    dev->at( start );
	    if ( doc.setContent( dev, &errMsg, &errLine ) ) {
		w = widgetFactory->createFromUiFile( doc, connector, parent, name );
	    } else {
		// tqDebug( TQString("Parse error: ") + errMsg + TQString(" in line %d"), errLine );
	    }
	}
	if ( !w ) {
	    delete widgetFactory;
	    return 0;
	}
    }

    if ( !languageInterfaceManager )
	languageInterfaceManager =
	    new TQPluginManager<LanguageInterface>( IID_Language, TQApplication::libraryPaths(), *qwf_plugin_dir );
    widgetFactory->loadExtraSource();

    if ( widgetFactory->toplevel ) {
#ifndef TQT_NO_SQL
	TQMap<TQWidget*, SqlWidgetConnection>::Iterator cit = widgetFactory->sqlWidgetConnections.begin();
	for( ; cit != widgetFactory->sqlWidgetConnections.end(); ++cit ) {
	    if ( widgetFactory->noDatabaseWidgets.find( cit.key()->name() ) !=
		 widgetFactory->noDatabaseWidgets.end() )
		continue;
	    if ( cit.key()->inherits( "TQDesignerDataBrowser2" ) )
		( (TQDesignerDataBrowser2*)cit.key() )->initPreview( (*cit).conn, (*cit).table,
								    cit.key(), *(*cit).dbControls );
	    else if ( cit.key()->inherits( "TQDesignerDataView2" ) )
		( (TQDesignerDataView2*)cit.key() )->initPreview( (*cit).conn, (*cit).table,
								 cit.key(), *(*cit).dbControls );
	}

	for ( TQMap<TQString, TQStringList>::Iterator it = widgetFactory->dbTables.begin();
	      it != widgetFactory->dbTables.end(); ++it ) {
	    TQDataTable *table = (TQDataTable*)widgetFactory->toplevel->child( it.key(), "TQDataTable" );
	    if ( !table )
		continue;
	    if ( widgetFactory->noDatabaseWidgets.find( table->name() ) !=
		 widgetFactory->noDatabaseWidgets.end() )
		continue;
	    TQValueList<Field> fieldMap = *widgetFactory->fieldMaps.find( table );
	    TQString conn = (*it)[ 0 ];
	    TQSqlCursor* c = 0;
	    TQSqlDatabase *db = 0;
	    if ( conn.isEmpty() || conn == "(default)" ) {
		db = TQSqlDatabase::database();
		c = new TQSqlCursor( (*it)[ 1 ] );
	    } else {
		db = TQSqlDatabase::database( conn );
		c = new TQSqlCursor( (*it)[ 1 ], true, db );
	    }
	    if ( db ) {
		table->setSqlCursor( c, fieldMap.isEmpty(), true );
		table->refresh( TQDataTable::RefreshAll );
	    }
	}
#endif
    }

    for ( TQMap<TQString, TQString>::Iterator it = widgetFactory->buddies.begin();
	  it != widgetFactory->buddies.end(); ++it ) {
	TQLabel *label = (TQLabel*)widgetFactory->toplevel->child( it.key(), "TQLabel" );
	TQWidget *buddy = (TQWidget*)widgetFactory->toplevel->child( *it, "TQWidget" );
	if ( label && buddy )
	    label->setBuddy( buddy );
    }

    delete widgetFactory;

    TQApplication::sendPostedEvents();

    return w;
}

TQWidget *TQWidgetFactory::createFromUiFile( TQDomDocument doc, TQObject *connector,
	TQWidget *parent, const char *name )
{
    DomTool::fixDocument( doc );

    uiFileVersion = doc.firstChild().toElement().attribute("version");
    TQDomElement e = doc.firstChild().toElement().firstChild().toElement();

    TQDomElement variables = e;
    while ( variables.tagName() != "variables" && !variables.isNull() )
	variables = variables.nextSibling().toElement();

    TQDomElement slots = e;
    while ( slots.tagName() != "slots" && !slots.isNull() )
	slots = slots.nextSibling().toElement();

    TQDomElement connections = e;
    while ( connections.tagName() != "connections" && !connections.isNull() )
	connections = connections.nextSibling().toElement();

    TQDomElement imageCollection = e;
    while ( imageCollection.tagName() != "images" && !imageCollection.isNull() )
	imageCollection = imageCollection.nextSibling().toElement();

    TQDomElement tabOrder = e;
    while ( tabOrder.tagName() != "tabstops" && !tabOrder.isNull() )
	tabOrder = tabOrder.nextSibling().toElement();

    TQDomElement actions = e;
    while ( actions.tagName() != "actions" && !actions.isNull() )
	actions = actions.nextSibling().toElement();

    TQDomElement toolbars = e;
    while ( toolbars.tagName() != "toolbars" && !toolbars.isNull() )
	toolbars = toolbars.nextSibling().toElement();

    TQDomElement menubar = e;
    while ( menubar.tagName() != "menubar" && !menubar.isNull() )
	menubar = menubar.nextSibling().toElement();

    TQDomElement functions = e;
    while ( functions.tagName() != "functions" && !functions.isNull() )
	functions = functions.nextSibling().toElement();

    TQDomElement widget;
    while ( !e.isNull() ) {
	if ( e.tagName() == "class" ) {
	    d->translationContext = e.firstChild().toText().data();
	} else if ( e.tagName() == "widget" ) {
	    widget = e;
	} else if ( e.tagName() == "pixmapinproject" ) {
	    usePixmapCollection = true;
	} else if ( e.tagName() == "layoutdefaults" ) {
	    defSpacing = e.attribute( "spacing", TQString::number( defSpacing ) ).toInt();
	    defMargin = e.attribute( "margin", TQString::number( defMargin ) ).toInt();
	}
	e = e.nextSibling().toElement();
    }

    if ( !imageCollection.isNull() )
	loadImageCollection( imageCollection );

    createWidgetInternal( widget, parent, 0, widget.attribute("class", "TQWidget") );
    TQWidget *w = toplevel;
    if ( !w )
	return 0;

    if ( !actions.isNull() )
	loadActions( actions );
    if ( !toolbars.isNull() )
	loadToolBars( toolbars );
    if ( !menubar.isNull() )
	loadMenuBar( menubar );

    if ( !connections.isNull() )
	loadConnections( connections, connector );
    if ( w && name && tqstrlen( name ) > 0 )
	w->setName( name );

    if ( !tabOrder.isNull() )
	loadTabOrder( tabOrder );

#if 0
    if ( !functions.isNull() ) // compatibiliy with early 3.0 betas
	loadFunctions( functions );
#endif

    return w;
}

void TQWidgetFactory::unpackUInt16( TQDataStream& in, TQ_UINT16& n )
{
    TQ_UINT8 half;
    in >> half;
    if ( half == 255 ) {
	in >> n;
    } else {
	n = half;
    }
}

void TQWidgetFactory::unpackUInt32( TQDataStream& in, TQ_UINT32& n )
{
    TQ_UINT16 half;
    in >> half;
    if ( half == 65535 ) {
	in >> n;
    } else {
	n = half;
    }
}

void TQWidgetFactory::unpackByteArray( TQDataStream& in, TQByteArray& array )
{
    TQ_UINT32 size;
    unpackUInt32( in, size );
    array.resize( size );
    in.readRawBytes( array.data(), size );
}

void TQWidgetFactory::unpackCString( const UibStrTable& strings, TQDataStream& in,
				    TQCString& cstr )
{
    TQ_UINT32 n;
    unpackUInt32( in, n );
    cstr = strings.asCString( n );
}

void TQWidgetFactory::unpackString( const UibStrTable& strings, TQDataStream& in,
				   TQString& str )
{
    TQ_UINT32 n;
    unpackUInt32( in, n );
    str = strings.asString( n );
}

void TQWidgetFactory::unpackStringSplit( const UibStrTable& strings,
					TQDataStream& in, TQString& str )
{
    TQString remainder;
    unpackString( strings, in, str );
    unpackString( strings, in, remainder );
    str += remainder;
}

void TQWidgetFactory::unpackVariant( const UibStrTable& strings, TQDataStream& in,
				    TQVariant& value )
{
    TQString imageName;
    TQ_UINT32 number;
    TQ_UINT16 count;
    TQ_UINT16 x;
    TQ_UINT16 y;
    TQ_UINT16 width;
    TQ_UINT16 height;
    TQ_UINT8 bit;
    TQ_UINT8 type;

    in >> type;

    switch ( type ) {
    case TQVariant::String:
	unpackString( strings, in, value.asString() );
	break;
    case TQVariant::Pixmap:
	unpackString( strings, in, imageName );
	if ( imageName.isEmpty() ) {
	    value.asPixmap() = TQPixmap();
	} else {
	    value.asPixmap() = loadPixmap( imageName );
	}
	break;
    case TQVariant::Image:
	unpackString( strings, in, imageName );
	if ( imageName.isEmpty() ) {
	    value.asImage() = TQImage();
	} else {
	    value.asImage() = loadFromCollection( imageName );
	}
	break;
    case TQVariant::IconSet:
	unpackString( strings, in, imageName );
	if ( imageName.isEmpty() ) {
	    value.asIconSet() = TQIconSet();
	} else {
	    value.asIconSet() = TQIconSet( loadPixmap(imageName) );
	}
	break;
    case TQVariant::StringList:
	unpackUInt16( in, count );
	while ( count-- ) {
	    TQString str;
	    unpackString( strings, in, str );
	    value.asStringList().append( str );
	}
	break;
    case TQVariant::Rect:
	unpackUInt16( in, x );
	unpackUInt16( in, y );
	unpackUInt16( in, width );
	unpackUInt16( in, height );
	value = TQRect( x, y, width, height );
	break;
    case TQVariant::Size:
	unpackUInt16( in, width );
	unpackUInt16( in, height );
	value = TQSize( width, height );
	break;
    case TQVariant::Color:
	in >> value.asColor();
	break;
    case TQVariant::Point:
	unpackUInt16( in, x );
	unpackUInt16( in, y );
	value = TQPoint( x, y );
	break;
    case TQVariant::Int:
	unpackUInt32( in, number );
	value = (int) number;
	break;
    case TQVariant::Bool:
	in >> bit;
	value = TQVariant( bit != 0 );
	break;
    case TQVariant::Double:
	in >> value.asDouble();
	break;
    case TQVariant::CString:
	unpackCString( strings, in, value.asCString() );
	break;
    case TQVariant::Cursor:
	in >> value.asCursor();
	break;
    case TQVariant::Date:
	in >> value.asDate();
	break;
    case TQVariant::Time:
	in >> value.asTime();
	break;
    case TQVariant::DateTime:
	in >> value.asDateTime();
	break;
    default:
	in >> value;
    }
}

void TQWidgetFactory::inputSpacer( const UibStrTable& strings, TQDataStream& in,
				  TQLayout *parent )
{
    TQCString name;
    TQVariant value;
    TQCString comment;
    TQSizePolicy::SizeType sizeType = TQSizePolicy::Preferred;
    bool vertical = false;
    int w = 0;
    int h = 0;
    TQ_UINT16 column = 0;
    TQ_UINT16 row = 0;
    TQ_UINT16 colspan = 1;
    TQ_UINT16 rowspan = 1;
    TQ_UINT8 objectTag;

    in >> objectTag;
    while ( !in.atEnd() && objectTag != Object_End ) {
	switch ( objectTag ) {
	case Object_GridCell:
	    unpackUInt16( in, column );
	    unpackUInt16( in, row );
	    unpackUInt16( in, colspan );
	    unpackUInt16( in, rowspan );
	    break;
	case Object_VariantProperty:
	    unpackCString( strings, in, name );
	    unpackVariant( strings, in, value );

	    if ( name == "orientation" ) {
		vertical = ( value == "Vertical" );
	    } else if ( name == "sizeHint" ) {
		w = value.toSize().width();
		h = value.toSize().height();
	    } else if ( name == "sizeType" ) {
		sizeType = stringToSizeType( value.toString() );
	    }
	    break;
	default:
	    tqFatal( "Corrupt" );
	}
	in >> objectTag;
    }

    if ( parent != 0 ) {
	TQSpacerItem *spacer;
	if ( vertical ) {
	    spacer = new TQSpacerItem( w, h, TQSizePolicy::Minimum, sizeType );
	} else {
	    spacer = new TQSpacerItem( w, h, sizeType, TQSizePolicy::Minimum );
	}

	if ( parent->inherits("TQGridLayout") ) {
	    ((TQGridLayout *) parent)->addMultiCell( spacer, row,
		    row + rowspan - 1, column, column + colspan - 1,
		    vertical ? TQt::AlignHCenter : TQt::AlignVCenter );
	} else {
	    parent->addItem( spacer );
	}
    }
}

void TQWidgetFactory::inputColumnOrRow( const UibStrTable& strings,
				       TQDataStream& in, TQObject *parent,
				       bool isRow )
{
    TQString text;
    TQPixmap pixmap;
    TQString field;
    bool clickable = true;
    bool resizable = true;

    TQCString name;
    TQVariant value;
    TQCString comment;
    TQString str;
    TQ_UINT8 objectTag;

    in >> objectTag;
    while ( !in.atEnd() && objectTag != Object_End ) {
	switch ( objectTag ) {
	case Object_TextProperty:
	    unpackCString( strings, in, name );
	    unpackCString( strings, in, value.asCString() );
	    unpackCString( strings, in, comment );
	    str = translate( value.asCString().data(), comment.data() );

	    if ( name == "field" ) {
		field = str;
	    } else if ( name == "text" ) {
		text = str;
	    }
	    break;
	case Object_VariantProperty:
	    unpackCString( strings, in, name );
	    unpackVariant( strings, in, value );

	    if ( name == "clickable" ) {
		clickable = value.toBool();
	    } else if ( name == "pixmap" ) {
		pixmap = value.asPixmap();
	    } else if ( name == "resizable" ) {
		resizable = value.toBool();
	    }
	    break;
	default:
	    tqFatal( "Corrupt" );
	}
	in >> objectTag;
    }

    if ( parent != 0 ) {
	if ( parent->inherits("TQListView") ) {
	    createListViewColumn( (TQListView *) parent, text, pixmap, clickable,
				  resizable );
#ifndef TQT_NO_TABLE
	} else if ( parent->inherits("TQTable") ) {
	    createTableColumnOrRow( (TQTable *) parent, text, pixmap, field,
				    isRow );
#endif
	}
    }
}

void TQWidgetFactory::inputItem( const UibStrTable& strings, TQDataStream& in,
				TQObject *parent, TQListViewItem *parentItem )
{
    TQStringList texts;
    TQValueList<TQPixmap> pixmaps;
    TQCString name;
    TQVariant value;
    TQCString comment;
    TQ_UINT8 objectTag;

    TQListView *listView = 0;
    if ( parent != 0 && parent->inherits("TQListView") )
	parent = (TQListView *) parent;
    TQListViewItem *item = 0;
    if ( listView != 0 ) {
	if ( parentItem == 0 ) {
	    item = new TQListViewItem( listView, d->lastItem );
	} else {
	    item = new TQListViewItem( parentItem, d->lastItem );
	}
	d->lastItem = item;
    }

    in >> objectTag;
    while ( !in.atEnd() && objectTag != Object_End ) {
	switch ( objectTag ) {
	case Object_Item:
	    if ( listView != 0 )
		d->lastItem->setOpen( true );
	    inputItem( strings, in, parent, item );
	    break;
	case Object_TextProperty:
	    unpackCString( strings, in, name );
	    unpackCString( strings, in, value.asCString() );
	    unpackCString( strings, in, comment );

	    if ( name == "text" )
		texts << translate( value.asCString().data(), comment.data() );
	    break;
	case Object_VariantProperty:
	    unpackCString( strings, in, name );
	    unpackVariant( strings, in, value );

	    if ( name == "pixmap" )
		pixmaps << value.asPixmap();
	    break;
	default:
	    tqFatal( "Corrupt" );
	}
	in >> objectTag;
    }

    if ( listView != 0 ) {
	int i = 0;
	TQStringList::ConstIterator t = texts.begin();
	while ( t != texts.end() ) {
	    item->setText( i, *t );
	    ++i;
	    ++t;
	}

	int j = 0;
	TQValueList<TQPixmap>::ConstIterator p = pixmaps.begin();
	while ( p != pixmaps.end() ) {
	    item->setPixmap( j, *p );
	    ++j;
	    ++p;
	}
    } else {
	TQString text = texts.last();
	TQPixmap pixmap = pixmaps.last();

	if ( parent != 0 ) {
	    if ( parent->inherits("TQComboBox") ||
		 parent->inherits("TQListBox") ) {
		TQListBox *listBox = (TQListBox *) parent->tqt_cast( "TQListBox" );
		if ( listBox == 0 )
		    listBox = ((TQComboBox *) parent)->listBox();

		if ( pixmap.isNull() ) {
		    (void) new TQListBoxText( listBox, text );
		} else {
		    (void) new TQListBoxPixmap( listBox, pixmap, text );
		}
    #ifndef TQT_NO_ICONVIEW
	    } else if ( parent->inherits("TQIconView") ) {
		(void) new TQIconViewItem( (TQIconView *) parent, text, pixmap );
    #endif
	    }
	}
    }
}

void TQWidgetFactory::inputMenuItem( TQObject **objects,
				    const UibStrTable& strings, TQDataStream& in,
				    TQMenuBar *menuBar )
{
    TQCString name;
    TQCString text;
    TQ_UINT16 actionNo;
    TQ_UINT8 objectTag;

    unpackCString( strings, in, name );
    unpackCString( strings, in, text );

    TQPopupMenu *popupMenu = new TQPopupMenu( menuBar->parentWidget(), name );

    in >> objectTag;
    while ( !in.atEnd() && objectTag != Object_End ) {
	switch ( objectTag ) {
	case Object_ActionRef:
	    unpackUInt16( in, actionNo );
	    ((TQAction *) objects[actionNo])->addTo( popupMenu );
	    break;
	case Object_Separator:
	    popupMenu->insertSeparator();
	    break;
	default:
	    tqFatal( "Corrupt" );
	}
	in >> objectTag;
    }
    menuBar->insertItem( translate(text.data()), popupMenu );
}

TQObject *TQWidgetFactory::inputObject( TQObject **objects, int& numObjects,
				      const UibStrTable& strings,
				      TQDataStream& in, TQWidget *ancestorWidget,
				      TQObject *parent, TQCString className )
{
    TQObject *obj = 0;
    TQWidget *widget = 0;
    TQLayout *layout = 0;
    TQWidget *parentWidget = 0;
    TQLayout *parentLayout = 0;

    bool isTQObject = !className.isEmpty();
    if ( isTQObject ) {
	if ( parent != 0 ) {
	    if ( parent->isWidgetType() ) {
		if ( parent->inherits("TQMainWindow") ) {
		    parentWidget = ((TQMainWindow *) parent)->centralWidget();
		} else {
		    parentWidget = (TQWidget *) parent;
		}
	    } else if ( parent->inherits("TQLayout") ) {
		parentLayout = (TQLayout *) parent;
		parentWidget = ancestorWidget;
	    }
	}

	if ( className == "TQAction" ) {
	    unpackCString( strings, in, className );
	    if ( className == "TQActionGroup" ) {
		obj = new TQActionGroup( parent );
	    } else {
		obj = new TQAction( parent );
	    }
	} else if ( className == "TQLayout" ) {
	    unpackCString( strings, in, className );
	    LayoutType type = Grid;
	    if ( className == "TQHBoxLayout" ) {
		type = HBox;
	    } else if ( className == "TQVBoxLayout" ) {
		type = VBox;
	    }
	    if ( parentLayout != 0 && parentLayout->inherits("TQGridLayout") ) {
		layout = createLayout( 0, 0, type );
	    } else {
		layout = createLayout( parentWidget, parentLayout, type );
	    }
	    obj = layout;
	} else if ( className == "TQMenuBar" ) {
	    unpackCString( strings, in, className );
	    widget = ((TQMainWindow *) parent)->menuBar();
	    obj = widget;
	} else if ( className == "TQToolBar" ) {
	    TQ_UINT8 dock;
	    in >> dock;
	    unpackCString( strings, in, className );
	    widget = new TQToolBar( TQString::null, (TQMainWindow *) parent,
				   (TQt::Dock) dock );
	    obj = widget;
	} else if ( className == "TQWidget" ) {
	    unpackCString( strings, in, className );
	    widget = createWidget( className, parentWidget, 0 );
	    obj = widget;
	}

	if ( widget != 0 )
	    ancestorWidget = widget;
	d->lastItem = 0;
	objects[numObjects++] = obj;
    }

    TQCString name;
    TQVariant value;
    TQCString comment;
    TQString str;
    TQ_UINT16 actionNo;
    int metAttribute = 0;
    TQ_UINT16 column = 0;
    TQ_UINT16 row = 0;
    TQ_UINT16 colspan = 1;
    TQ_UINT16 rowspan = 1;
    TQ_UINT8 paletteTag;
    TQ_UINT8 objectTag;

    in >> objectTag;
    while ( !in.atEnd() && objectTag != Object_End ) {
	switch ( objectTag ) {
	case Object_ActionRef:
	    unpackUInt16( in, actionNo );
	    ((TQAction *) objects[actionNo])->addTo( (TQToolBar *) widget );
	    break;
	case Object_Attribute:
	    metAttribute = 2;
	    break;
	case Object_Column:
	    inputColumnOrRow( strings, in, obj, false );
	    break;
	case Object_Event:
	    unpackCString( strings, in, name );
	    unpackVariant( strings, in, value );
	    // ### do something with value.asStringList()
	    break;
	case Object_FontProperty:
	    {
		TQFont font;
		TQString family;
		TQ_UINT16 pointSize;
		TQ_UINT8 fontFlags;

		unpackCString( strings, in, name );
		in >> fontFlags;

		if ( fontFlags & Font_Family ) {
		    unpackString( strings, in, family );
		    font.setFamily( family );
		}
		if ( fontFlags & Font_PointSize ) {
		    unpackUInt16( in, pointSize );
		    font.setPointSize( pointSize );
		}
		if ( fontFlags & Font_Bold )
		    font.setBold( true );
		if ( fontFlags & Font_Italic )
		    font.setItalic( true );
		if ( fontFlags & Font_Underline )
		    font.setUnderline( true );
		if ( fontFlags & Font_StrikeOut )
		    font.setStrikeOut( true );

		if ( obj != 0 )
		    setProperty( obj, name, font );
	    }
	    break;
	case Object_GridCell:
	    unpackUInt16( in, column );
	    unpackUInt16( in, row );
	    unpackUInt16( in, colspan );
	    unpackUInt16( in, rowspan );
	    break;
	case Object_Item:
	    inputItem( strings, in, obj );
	    break;
	case Object_MenuItem:
	    inputMenuItem( objects, strings, in, (TQMenuBar *) widget );
	    break;
	case Object_PaletteProperty:
	    {
		TQPalette palette;
		TQColorGroup colorGroup;
		TQColor color;
		int role = -1;

		unpackCString( strings, in, name );

		in >> paletteTag;
		while ( !in.atEnd() && paletteTag != Palette_End ) {
		    switch ( paletteTag ) {
		    case Palette_Active:
			palette.setActive( colorGroup );
			role = -1;
			break;
		    case Palette_Inactive:
			palette.setInactive( colorGroup );
			role = -1;
			break;
		    case Palette_Disabled:
			palette.setDisabled( colorGroup );
			role = -1;
			break;
		    case Palette_Color:
			role++;
			in >> color;
			colorGroup.setColor( (TQColorGroup::ColorRole) role,
					     color );
			break;
		    case Palette_Pixmap:
			unpackVariant( strings, in, value );
			colorGroup.setBrush( (TQColorGroup::ColorRole) role,
					     TQBrush(color, value.asPixmap()) );
			break;
		    default:
			tqFatal( "Corrupt" );
		    }
		    in >> paletteTag;
		}
		if ( obj != 0 )
		    setProperty( obj, name, palette );
	    }
	    break;
	case Object_Row:
	    inputColumnOrRow( strings, in, obj, true );
	    break;
	case Object_Spacer:
	    inputSpacer( strings, in, layout );
	    break;
	case Object_Separator:
	    ((TQToolBar *) widget)->addSeparator();
	    break;
	case Object_SubAction:
	    inputObject( objects, numObjects, strings, in, parentWidget,
			 obj != 0 ? obj : parent, "TQAction" );
	    break;
	case Object_SubLayout:
	    inputObject( objects, numObjects, strings, in, parentWidget, obj,
			 "TQLayout" );
	    break;
	case Object_SubWidget:
	    inputObject( objects, numObjects, strings, in, parentWidget, obj,
			 "TQWidget" );
	    break;
	case Object_TextProperty:
	    unpackCString( strings, in, name );
	    unpackCString( strings, in, value.asCString() );
	    unpackCString( strings, in, comment );
	    str = translate( value.asCString().data(), comment.data() );

	    if ( metAttribute > 0 ) {
		if ( name == "title" ) {
		    if ( parent != 0 ) {
			if ( parent->inherits("TQTabWidget") ) {
			    ((TQTabWidget *) parent)->insertTab( widget, str );
			} else if ( parent->inherits("TQWizard") ) {
			    ((TQWizard *) parent)->addPage( widget, str );
			}
		    }
		}
	    } else {
		if ( obj != 0 )
		    setProperty( obj, name, str );
	    }
	    break;
	case Object_VariantProperty:
	    unpackCString( strings, in, name );
	    unpackVariant( strings, in, value );

	    if ( metAttribute > 0 ) {
		if ( name == "id" ) {
		    if ( parent != 0 && parent->inherits("TQWidgetStack") )
			((TQWidgetStack *) parent)->addWidget( widget, value.toInt() );
		}
	    } else {
		if ( obj != 0 )
		    setProperty( obj, name, value );
	    }
	    break;
	default:
	    tqFatal( "Corrupt" );
	}
	in >> objectTag;
	metAttribute--;
    }

    if ( parentLayout != 0 ) {
	if ( widget != 0 ) {
	    if ( parentLayout->inherits("TQGridLayout") ) {
		((TQGridLayout *) parentLayout)->addMultiCellWidget(
			widget, row, row + rowspan - 1, column,
			column + colspan - 1 );
	    } else {
		((TQBoxLayout *) parentLayout)->addWidget( widget );
	    }
	} else if ( layout != 0 ) {
	    if ( parentLayout->inherits("TQGridLayout") ) {
		((TQGridLayout *) parentLayout)->addMultiCellLayout(
			layout, row, row + rowspan - 1, column,
			column + colspan - 1 );
	    }
	}
    }
    return obj;
}

TQWidget *TQWidgetFactory::createFromUibFile( TQDataStream& in,
	TQObject * /* connector */ , TQWidget *parent, const char *name )
{
#define END_OF_BLOCK() \
	( in.atEnd() || in.device()->at() >= nextBlock )

    TQ_UINT8 lf;
    TQ_UINT8 cr;
    in >> lf;
    in >> cr;
    if ( lf != '\n' || cr != '\r' ) {
	tqWarning( "File corrupted" );
	return 0;
    }

    TQ_UINT8 qdatastreamVersion;
    in >> qdatastreamVersion;
    if ( (int) qdatastreamVersion > in.version() ) {
	tqWarning( "Incompatible version of TQt" );
	return 0;
    }
    in.setVersion( qdatastreamVersion );

    UibStrTable strings;
    TQObject **objects = 0;
    int numObjects = 0;

    TQ_UINT8 blockType;
    TQ_UINT32 blockSize;

    in >> blockType;
    while ( !in.atEnd() && blockType != Block_End ) {
	unpackUInt32( in, blockSize );
	TQIODevice::Offset nextBlock = in.device()->at() + blockSize;

	switch ( blockType ) {
	case Block_Actions:
	    inputObject( objects, numObjects, strings, in, toplevel, toplevel );
	    break;
	case Block_Buddies:
	    {
		TQ_UINT16 labelNo;
		TQ_UINT16 buddyNo;

		do {
		    unpackUInt16( in, labelNo );
		    unpackUInt16( in, buddyNo );
		    TQLabel *label =
			(TQLabel *) objects[labelNo]->tqt_cast( "TQLabel" );
		    if ( label != 0 )
			label->setBuddy( (TQWidget *) objects[buddyNo] );
		} while ( !END_OF_BLOCK() );
	    }
	    break;
	case Block_Connections:
	    {
		TQString language = "C++";
		TQ_UINT16 senderNo = 0;
		TQString signal = "clicked()";
		TQ_UINT16 receiverNo = 0;
		TQString slot = "accept()";
		TQ_UINT8 connectionFlags;

		do {
		    in >> connectionFlags;
		    if ( connectionFlags & Connection_Language )
			unpackString( strings, in, language );
		    if ( connectionFlags & Connection_Sender )
			unpackUInt16( in, senderNo );
		    if ( connectionFlags & Connection_Signal )
			unpackStringSplit( strings, in, signal );
		    if ( connectionFlags & Connection_Receiver )
			unpackUInt16( in, receiverNo );
		    if ( connectionFlags & Connection_Slot )
			unpackStringSplit( strings, in, slot );
		    // ###
#if 0
		    tqWarning( "connect( %p, %s, %p, %s )", objects[senderNo],
			      signal.latin1(), objects[receiverNo],
			      slot.latin1() );
#endif
		} while ( !END_OF_BLOCK() );
	    }
	    break;
	case Block_Functions:
	    // ###
	    tqWarning( "Block_Functions not supported" );
	    in.device()->at( nextBlock );
	    break;
	case Block_Images:
	    {
		TQString format;
		TQ_UINT32 length;
		TQByteArray data;
		Image image;

		do {
		    unpackString( strings, in, image.name );
		    unpackString( strings, in, format );
		    unpackUInt32( in, length );
		    unpackByteArray( in, data );
		    image.img = loadImageData( format, length, data );
		    images += image;
		} while ( !END_OF_BLOCK() );
	    }
	    break;
	case Block_Intro:
	    {
		TQ_INT16 defaultMargin;
		TQ_INT16 defaultSpacing;
		TQ_UINT16 maxObjects;
		TQ_UINT8 introFlags;

		in >> introFlags;
		in >> defaultMargin;
		in >> defaultSpacing;
		unpackUInt16( in, maxObjects );
		unpackCString( strings, in, d->translationContext );

		if ( introFlags & Intro_Pixmapinproject )
		    usePixmapCollection = true;
		if ( defaultMargin != -32768 )
		    defMargin = defaultMargin;
		if ( defaultSpacing != -32768 )
		    defSpacing = defaultSpacing;
		objects = new TQObject *[maxObjects];
	    }
	    break;
	case Block_Menubar:
	    inputObject( objects, numObjects, strings, in, toplevel, toplevel,
			 "TQMenuBar" );
	    break;
	case Block_Slots:
	    {
		TQString language;
		TQString slot;

		do {
		    unpackString( strings, in, language );
		    unpackStringSplit( strings, in, slot );
		} while ( !END_OF_BLOCK() );
	    }
	    break;
	case Block_Strings:
	    strings.readBlock( in, blockSize );
	    break;
	case Block_Tabstops:
	    {
		TQ_UINT16 beforeNo;
		TQ_UINT16 afterNo;

		unpackUInt16( in, beforeNo );
		while ( !END_OF_BLOCK() ) {
		    unpackUInt16( in, afterNo );
		    toplevel->setTabOrder( (TQWidget *) objects[beforeNo],
					   (TQWidget *) objects[afterNo] );
		    beforeNo = afterNo;
		}
	    }
	    break;
	case Block_Toolbars:
	    do {
		inputObject( objects, numObjects, strings, in, toplevel,
			     toplevel, "TQToolBar" );
	    } while ( !END_OF_BLOCK() );
	    break;
	case Block_Variables:
	    // ###
	    tqWarning( "Block_Variables not supported" );
	    in.device()->at( nextBlock );
	    break;
	case Block_Widget:
	    toplevel = (TQWidget *)
		inputObject( objects, numObjects, strings, in, toplevel, parent,
			     "TQWidget" );
	    if ( toplevel != 0 )
		toplevel->setName( name );
	    break;
	default:
	    tqWarning( "Version error" );
	    return 0;
	}
	in >> blockType;
    }
    delete[] objects;
    return toplevel;
}

/*! Installs a widget factory \a factory, which normally contains
  additional widgets that can then be created using a TQWidgetFactory.
  See createWidget() for further details.
*/

void TQWidgetFactory::addWidgetFactory( TQWidgetFactory *factory )
{
    widgetFactories.append( factory );
}

/*!
    Creates a widget of the type \a className passing \a parent and \a
    name to its constructor.

    If \a className is a widget in the TQt library, it is directly
    created by this function. If the widget isn't in the TQt library,
    each of the installed widget plugins is asked, in turn, to create
    the widget. As soon as a plugin says it can create the widget it
    is asked to do so. It may occur that none of the plugins can
    create the widget, in which case each installed widget factory is
    asked to create the widget (see addWidgetFactory()). If the widget
    cannot be created by any of these means, 0 is returned.

    If you have a custom widget, and want it to be created using the
    widget factory, there are two approaches you can use:

    \list 1

    \i Write a widget plugin. This allows you to use the widget in
    \e{TQt Designer} and in this TQWidgetFactory. See the widget plugin
    documentation for further details. (See the "Creating Custom
    Widgets with Plugins" section of the \link designer-manual.book TQt
    Designer manual\endlink for an example.

    \i Subclass TQWidgetFactory. Then reimplement this function to
    create and return an instance of your custom widget if \a
    className equals the name of your widget, otherwise return 0. Then
    at the beginning of your program where you want to use the widget
    factory to create widgets do a:
    \code
    TQWidgetFactory::addWidgetFactory( new MyWidgetFactory );
    \endcode
    where MyWidgetFactory is your TQWidgetFactory subclass.

    \endlist
*/

TQWidget *TQWidgetFactory::createWidget( const TQString &className, TQWidget *parent,
				       const char *name ) const
{
    // create widgets we know
    if ( className == "TQPushButton" ) {
	return new TQPushButton( parent, name );
    } else if ( className == "TQToolButton" ) {
	return new TQToolButton( parent, name );
    } else if ( className == "TQCheckBox" ) {
	return new TQCheckBox( parent, name );
    } else if ( className == "TQRadioButton" ) {
	return new TQRadioButton( parent, name );
    } else if ( className == "TQGroupBox" ) {
	return new TQGroupBox( parent, name );
    } else if ( className == "TQButtonGroup" ) {
	return new TQButtonGroup( parent, name );
    } else if ( className == "TQIconView" ) {
#if !defined(TQT_NO_ICONVIEW)
	return new TQIconView( parent, name );
#endif
    } else if ( className == "TQTable" ) {
#if !defined(TQT_NO_TABLE)
	return new TQTable( parent, name );
#endif
    } else if ( className == "TQListBox" ) {
	return new TQListBox( parent, name );
    } else if ( className == "TQListView" ) {
	return new TQListView( parent, name );
    } else if ( className == "TQLineEdit" ) {
	return new TQLineEdit( parent, name );
    } else if ( className == "TQSpinBox" ) {
	return new TQSpinBox( parent, name );
    } else if ( className == "TQMultiLineEdit" ) {
	return new TQMultiLineEdit( parent, name );
    } else if ( className == "TQLabel" || className == "TextLabel" || className == "PixmapLabel" ) {
	return new TQLabel( parent, name );
    } else if ( className == "TQLayoutWidget" ) {
	return new TQWidget( parent, name );
    } else if ( className == "TQTabWidget" ) {
	return new TQTabWidget( parent, name );
    } else if ( className == "TQComboBox" ) {
	return new TQComboBox( false, parent, name );
    } else if ( className == "TQWidget" ) {
	if ( !qwf_stays_on_top )
	    return new TQWidget( parent, name );
	return new TQWidget( parent, name, TQt::WStyle_StaysOnTop );
    } else if ( className == "TQDialog" ) {
	if ( !qwf_stays_on_top )
	    return new TQDialog( parent, name );
	return new TQDialog( parent, name, false, TQt::WStyle_StaysOnTop );
    } else if ( className == "TQWizard" ) {
	return  new TQWizard( parent, name );
    } else if ( className == "TQLCDNumber" ) {
	return new TQLCDNumber( parent, name );
    } else if ( className == "TQProgressBar" ) {
	return new TQProgressBar( parent, name );
    } else if ( className == "TQTextView" ) {
	return new TQTextView( parent, name );
    } else if ( className == "TQTextBrowser" ) {
	return new TQTextBrowser( parent, name );
    } else if ( className == "TQDial" ) {
	return new TQDial( parent, name );
    } else if ( className == "TQSlider" ) {
	return new TQSlider( parent, name );
    } else if ( className == "TQFrame" ) {
	return new TQFrame( parent, name );
    } else if ( className == "TQSplitter" ) {
	return new TQSplitter( parent, name );
    } else if ( className == "Line" ) {
	TQFrame *f = new TQFrame( parent, name );
	f->setFrameStyle( TQFrame::HLine | TQFrame::Sunken );
	return f;
    } else if ( className == "TQTextEdit" ) {
	return new TQTextEdit( parent, name );
    } else if ( className == "TQDateEdit" ) {
	return new TQDateEdit( parent, name );
    } else if ( className == "TQTimeEdit" ) {
	return new TQTimeEdit( parent, name );
    } else if ( className == "TQDateTimeEdit" ) {
	return new TQDateTimeEdit( parent, name );
    } else if ( className == "TQScrollBar" ) {
	return new TQScrollBar( parent, name );
    } else if ( className == "TQPopupMenu" ) {
	return new TQPopupMenu( parent, name );
    } else if ( className == "TQWidgetStack" ) {
	return new TQWidgetStack( parent, name );
    } else if ( className == "TQToolBox" ) {
	return new TQToolBox( parent, name );
    } else if ( className == "TQVBox" ) {
	return new TQVBox( parent, name );
    } else if ( className == "TQHBox" ) {
	return new TQHBox( parent, name );
    } else if ( className == "TQGrid" ) {
	return new TQGrid( 4, parent, name );
    } else if ( className == "TQMainWindow" ) {
	TQMainWindow *mw = 0;
	if ( !qwf_stays_on_top )
	    mw = new TQMainWindow( parent, name );
	else
	    mw = new TQMainWindow( parent, name, TQt::WType_TopLevel | TQt::WStyle_StaysOnTop );
	mw->setCentralWidget( new TQWidget( mw, "qt_central_widget" ) );
	mw->centralWidget()->show();
	(void)mw->statusBar();
	return mw;

    }
#if !defined(TQT_NO_SQL)
    else if ( className == "TQDataTable" ) {
	return new TQDataTable( parent, name );
    } else if ( className == "TQDataBrowser" ) {
	return new TQDesignerDataBrowser2( parent, name );
    } else if ( className == "TQDataView" ) {
	return new TQDesignerDataView2( parent, name );
    }
#endif

    setupPluginDir();
    // try to create it using the loaded widget plugins
    if ( !widgetInterfaceManager )
	widgetInterfaceManager =
	    new TQPluginManager<WidgetInterface>( IID_Widget, TQApplication::libraryPaths(),
						 *qwf_plugin_dir );

    TQInterfacePtr<WidgetInterface> iface = 0;
    widgetInterfaceManager->queryInterface( className, &iface );
    if ( iface ) {
	TQWidget *w = iface->create( className, parent, name );
	if ( w ) {
	    d->customWidgets.replace( className.latin1(), new bool(true) );
	    return w;
	}
    }

    // hope we have a factory which can do it
    for ( TQWidgetFactory* f = widgetFactories.first(); f; f = widgetFactories.next() ) {
	TQWidget *w = f->createWidget( className, parent, name );
	if ( w )
	    return w;
    }

    // no success
    return 0;
}

/*! Returns the names of the widgets this factory can create. */

TQStringList TQWidgetFactory::widgets()
{
    setupWidgetListAndMap();
    return *availableWidgetList;
}

/*! Returns true if the widget factory can create the specified \a widget;
otherwise returns false. */

bool TQWidgetFactory::supportsWidget( const TQString &widget )
{
    setupWidgetListAndMap();
    return ( availableWidgetMap->find( widget ) != availableWidgetMap->end() );
}

TQWidget *TQWidgetFactory::createWidgetInternal( const TQDomElement &e, TQWidget *parent,
					       TQLayout* layout, const TQString &classNameArg )
{
    d->lastItem = 0;
    TQDomElement n = e.firstChild().toElement();
    TQWidget *w = 0; // the widget that got created
    TQObject *obj = 0; // gets the properties

    TQString className = classNameArg;

    int row = e.attribute( "row" ).toInt();
    int col = e.attribute( "column" ).toInt();
    int rowspan = e.attribute( "rowspan" ).toInt();
    int colspan = e.attribute( "colspan" ).toInt();
    if ( rowspan < 1 )
	rowspan = 1;
    if ( colspan < 1 )
	colspan = 1;

    bool isTQLayoutWidget = false;

    if ( !className.isEmpty() ) {
	if ( !layout && className  == "TQLayoutWidget" ) {
	    className = "TQWidget";
	    isTQLayoutWidget = true;
	}
	if ( layout && className == "TQLayoutWidget" ) {
	    // hide layout widgets
	    w = parent;
	} else {
	    obj = TQWidgetFactory::createWidget( className, parent, 0 );
	    if ( !obj )
		return 0;
	    w = (TQWidget*)obj;
	    if ( !toplevel )
		toplevel = w;
	    if ( w->inherits( "TQMainWindow" ) )
		w = ( (TQMainWindow*)w )->centralWidget();
	    if ( layout ) {
		switch( layoutType( layout ) ) {
		case HBox:
		    ( (TQHBoxLayout*)layout )->addWidget( w );
		    break;
		case VBox:
		    ( (TQVBoxLayout*)layout )->addWidget( w );
		    break;
		case Grid:
		    ( (TQGridLayout*)layout )->addMultiCellWidget( w, row, row + rowspan - 1,
								  col, col + colspan - 1 );
		    break;
		default:
		    break;
		}
	    }

	    layout = 0;
	}
    }
#ifdef QT_CONTAINER_CUSTOM_WIDGETS
    TQString parentClassName = parent ? parent->className() : 0;
    bool isPlugin = parent ? !!d->customWidgets.find( parent->className() ) : false;
    if ( isPlugin )
	tqWarning( "####### loading custom container widgets without page support not implemented!" );
    // ### TODO loading for custom container widgets without pages
#endif
    
    int idx = 0;
    while ( !n.isNull() ) {
	if ( n.tagName() == "spacer" ) {
	    createSpacer( n, layout );
	} else if ( n.tagName() == "widget" ) {
	    TQMap< TQString, TQString> *oldDbControls = dbControls;
	    createWidgetInternal( n, w, layout, n.attribute( "class", "TQWidget" ) );
	    dbControls = oldDbControls;
	} else if ( n.tagName() == "hbox" ) {
	    TQLayout *parentLayout = layout;
	    if ( layout && layout->inherits( "TQGridLayout" ) )
		layout = createLayout( 0, 0, TQWidgetFactory::HBox, isTQLayoutWidget );
	    else
		layout = createLayout( w, layout, TQWidgetFactory::HBox, isTQLayoutWidget );
	    obj = layout;
	    n = n.firstChild().toElement();
	    if ( parentLayout && parentLayout->inherits( "TQGridLayout" ) )
		( (TQGridLayout*)parentLayout )->addMultiCellLayout( layout, row,
				    row + rowspan - 1, col, col + colspan - 1 );
	    continue;
	} else if ( n.tagName() == "grid" ) {
	    TQLayout *parentLayout = layout;
	    if ( layout && layout->inherits( "TQGridLayout" ) )
		layout = createLayout( 0, 0, TQWidgetFactory::Grid, isTQLayoutWidget );
	    else
		layout = createLayout( w, layout, TQWidgetFactory::Grid, isTQLayoutWidget );
	    obj = layout;
	    n = n.firstChild().toElement();
	    if ( parentLayout && parentLayout->inherits( "TQGridLayout" ) )
		( (TQGridLayout*)parentLayout )->addMultiCellLayout( layout, row,
				    row + rowspan - 1, col, col + colspan - 1 );
	    continue;
	} else if ( n.tagName() == "vbox" ) {
	    TQLayout *parentLayout = layout;
	    if ( layout && layout->inherits( "TQGridLayout" ) )
		layout = createLayout( 0, 0, TQWidgetFactory::VBox, isTQLayoutWidget );
	    else
		layout = createLayout( w, layout, TQWidgetFactory::VBox, isTQLayoutWidget );
	    obj = layout;
	    n = n.firstChild().toElement();
	    if ( parentLayout && parentLayout->inherits( "TQGridLayout" ) )
		( (TQGridLayout*)parentLayout )->addMultiCellLayout( layout, row,
				    row + rowspan - 1, col, col + colspan - 1 );
	    continue;
	} else if ( n.tagName() == "property" && obj ) {
	    setProperty( obj, n.attribute( "name" ), n.firstChild().toElement() );
	} else if ( n.tagName() == "attribute" && w ) {
	    TQString attrib = n.attribute( "name" );
	    TQVariant v = DomTool::elementToVariant( n.firstChild().toElement(), TQVariant() );
	    if ( parent ) {
		if ( parent->inherits( "TQTabWidget" ) ) {
		    if ( attrib == "title" )
			( (TQTabWidget*)parent )->insertTab( w, translate( v.toString() ) );
		} else if ( parent->inherits( "TQWidgetStack" ) ) {
		    if ( attrib == "id" )
			( (TQWidgetStack*)parent )->addWidget( w, v.toInt() );
		} else if ( parent->inherits( "TQToolBox" ) ) {
		    if ( attrib == "label" )
			( (TQToolBox*)parent )->addItem( w, v.toString() );
		} else if ( parent->inherits( "TQWizard" ) ) {
		    if ( attrib == "title" )
			( (TQWizard*)parent )->addPage( w, translate( v.toString() ) );
#ifdef QT_CONTAINER_CUSTOM_WIDGETS
		} else if ( isPlugin ) {
		    if ( attrib == "label" ) {
			WidgetInterface *iface = 0;
			widgetInterfaceManager->queryInterface( parentClassName, &iface );
			if ( iface ) {
			    TQWidgetContainerInterfacePrivate *iface2 = 0;
			    iface->queryInterface( IID_QWidgetContainer,
						   (TQUnknownInterface**)&iface2 );
			    if ( iface2 ) {
				iface2->insertPage( parentClassName,
						    (TQWidget*)parent, translate( v.toString() ), -1, w );
				iface2->release();
			    }
			    iface->release();
			}
		    }
#endif
		}
	    }
	} else if ( n.tagName() == "item" ) {
	    createItem( n, w );
	} else if ( n.tagName() == "column" || n.tagName() == "row" ) {
	    createColumn( n, w );
	}

	n = n.nextSibling().toElement();
	idx++;
    }

    return w;
}

TQLayout *TQWidgetFactory::createLayout( TQWidget *widget, TQLayout* layout,
				       LayoutType type, bool isTQLayoutWidget )
{
    int spacing = defSpacing;
    int margin = defMargin;

    if ( layout || !widget || isTQLayoutWidget )
	margin = 0;

    if ( !layout && widget && widget->inherits( "TQTabWidget" ) )
	widget = ((TQTabWidget*)widget)->currentPage();

    if ( !layout && widget && widget->inherits( "TQWizard" ) )
	widget = ((TQWizard*)widget)->currentPage();

    if ( !layout && widget && widget->inherits( "TQWidgetStack" ) )
	widget = ((TQWidgetStack*)widget)->visibleWidget();

    if ( !layout && widget && widget->inherits( "TQToolBox" ) )
	widget = ((TQToolBox*)widget)->currentItem();

    TQLayout *l = 0;
    int align = 0;
    if ( !layout && widget && widget->inherits( "TQGroupBox" ) ) {
	TQGroupBox *gb = (TQGroupBox*)widget;
	gb->setColumnLayout( 0, TQt::Vertical );
	layout = gb->layout();
	layout->setMargin( 0 );
	layout->setSpacing( 0 );
	align = TQt::AlignTop;
    }
    if ( layout ) {
	switch ( type ) {
        case HBox:
	    l = new TQHBoxLayout( layout );
	    break;
	case VBox:
	    l = new TQVBoxLayout( layout );
	    break;
	case Grid:
	    l = new TQGridLayout( layout );
	    break;
	default:
	    return 0;
	}
    } else {
	switch ( type ) {
	case HBox:
	    l = new TQHBoxLayout( widget );
	    break;
	case VBox:
	    l = new TQVBoxLayout( widget );
	    break;
	case Grid:
	    l = new TQGridLayout( widget );
	    break;
	default:
	    return 0;
	}
    }
    l->setAlignment( align );
    l->setMargin( margin );
    l->setSpacing( spacing );
    return l;
}

TQWidgetFactory::LayoutType TQWidgetFactory::layoutType( TQLayout *layout ) const
{
    if ( layout->inherits( "TQHBoxLayout" ) )
	return HBox;
    else if ( layout->inherits( "TQVBoxLayout" ) )
	return VBox;
    else if ( layout->inherits( "TQGridLayout" ) )
	return Grid;
    return NoLayout;
}

void TQWidgetFactory::setProperty( TQObject* obj, const TQString &prop,
				  TQVariant value )
{
    int offset = obj->metaObject()->findProperty( prop, true );

    if ( offset != -1 ) {
	if ( prop == "geometry" && obj == toplevel ) {
	    toplevel->resize( value.toRect().size() );
	} else if ( prop == "accel" ) {
	    obj->setProperty( prop, value.toKeySequence() );
	} else {
	    if ( value.type() == TQVariant::String ||
		 value.type() == TQVariant::CString ) {
		const TQMetaProperty *metaProp =
			obj->metaObject()->property( offset, true );
		if ( metaProp != 0 && metaProp->isEnumType() ) {
		    if ( metaProp->isSetType() ) {
			TQStrList flagsCStr;
			TQStringList flagsStr =
			    TQStringList::split( '|', value.asString() );
			TQStringList::ConstIterator f = flagsStr.begin();
			while ( f != flagsStr.end() ) {
			    flagsCStr.append( *f );
			    ++f;
			}
			value = TQVariant( metaProp->keysToValue(flagsCStr) );
		    } else {
			TQCString key = value.toCString();
			value = TQVariant( metaProp->keyToValue(key) );
		    }
		}
	    }
	    obj->setProperty( prop, value );
	}
    } else {
	if ( obj->isWidgetType() ) {
	    if ( prop == "toolTip" ) {
		if ( !value.toString().isEmpty() )
		    TQToolTip::add( (TQWidget*)obj, translate( value.toString() ) );
	    } else if ( prop == "whatsThis" ) {
		if ( !value.toString().isEmpty() )
		    TQWhatsThis::add( (TQWidget*)obj, translate( value.toString() ) );
	    } else if ( prop == "buddy" ) {
		buddies.insert( obj->name(), value.toCString() );
	    } else if ( prop == "buttonGroupId" ) {
		if ( obj->inherits( "TQButton" ) && obj->parent()->inherits( "TQButtonGroup" ) )
		    ( (TQButtonGroup*)obj->parent() )->insert( (TQButton*)obj, value.toInt() );
#ifndef TQT_NO_SQL
	    } else if ( prop == "database" && !obj->inherits( "TQDataView" )
		 && !obj->inherits( "TQDataBrowser" ) ) {
		const TQStringList& lst = value.asStringList();
		if ( lst.count() > 2 ) {
		    if ( dbControls )
			dbControls->insert( obj->name(), lst[ 2 ] );
		} else if ( lst.count() == 2 ) {
		    dbTables.insert( obj->name(), lst );
		}
	    } else if ( prop == "database" ) {
		const TQStringList& lst = value.asStringList();
		if ( lst.count() == 2 && obj->inherits( "TQWidget" ) ) {
		    SqlWidgetConnection conn( lst[ 0 ], lst[ 1 ] );
		    sqlWidgetConnections.insert( (TQWidget*)obj, conn );
		    dbControls = conn.dbControls;
		}
#endif
	    } else if ( prop == "frameworkCode" ) {
		if ( value.isValid() && !value.toBool() )
		    noDatabaseWidgets << obj->name();
	    }
	}
    }
}

void TQWidgetFactory::setProperty( TQObject* widget, const TQString &prop, const TQDomElement &e )
{
    TQString comment;
    TQVariant value( DomTool::elementToVariant( e, TQVariant(), comment ) );

    if ( e.tagName() == "string" ) {
	value = translate( value.asString(), comment );
    } else if ( e.tagName() == "pixmap" ) {
	TQPixmap pix = loadPixmap( value.toString() );
	if ( !pix.isNull() )
	    value = pix;
    } else if ( e.tagName() == "iconset" ) {
	TQPixmap pix = loadPixmap( value.toString() );
	if ( !pix.isNull() )
	    value = TQIconSet( pix );
    } else if ( e.tagName() == "image" ) {
	value = loadFromCollection( value.toString() );
    } else if ( e.tagName() == "palette" ) {
	TQDomElement n = e.firstChild().toElement();
	TQPalette p;
	while ( !n.isNull() ) {
	    TQColorGroup cg;
	    if ( n.tagName() == "active" ) {
		cg = loadColorGroup( n );
		p.setActive( cg );
	    } else if ( n.tagName() == "inactive" ) {
		cg = loadColorGroup( n );
		p.setInactive( cg );
	    } else if ( n.tagName() == "disabled" ) {
		cg = loadColorGroup( n );
		p.setDisabled( cg );
	    }
	    n = n.nextSibling().toElement();
	}
	value = p;
    }
    setProperty( widget, prop, value );
}

void TQWidgetFactory::createSpacer( const TQDomElement &e, TQLayout *layout )
{
    TQDomElement n = e.firstChild().toElement();
    int row = e.attribute( "row" ).toInt();
    int col = e.attribute( "column" ).toInt();
    int rowspan = e.attribute( "rowspan" ).toInt();
    int colspan = e.attribute( "colspan" ).toInt();

    TQt::Orientation orient = TQt::Horizontal;
    int w = 0, h = 0;
    TQSizePolicy::SizeType sizeType = TQSizePolicy::Preferred;
    while ( !n.isNull() ) {
	if ( n.tagName() == "property" ) {
	    TQString prop = n.attribute( "name" );
	    if ( prop == "orientation" ) {
		if ( n.firstChild().firstChild().toText().data() == "Horizontal" )
		    orient = TQt::Horizontal;
		else
		    orient = TQt::Vertical;
	    } else if ( prop == "sizeType" ) {
		sizeType = stringToSizeType( n.firstChild().firstChild().toText().data() );
	    } else if ( prop == "sizeHint" ) {
		w = n.firstChild().firstChild().firstChild().toText().data().toInt();
		h = n.firstChild().firstChild().nextSibling().firstChild().toText().data().toInt();
	    }
	}
	n = n.nextSibling().toElement();
    }

    if ( rowspan < 1 )
	rowspan = 1;
    if ( colspan < 1 )
	colspan = 1;
    TQSpacerItem *item = new TQSpacerItem( w, h, orient == TQt::Horizontal ? sizeType : TQSizePolicy::Minimum,
					 orient == TQt::Vertical ? sizeType : TQSizePolicy::Minimum );
    if ( layout ) {
	if ( layout->inherits( "TQBoxLayout" ) )
	    ( (TQBoxLayout*)layout )->addItem( item );
	else
	    ( (TQGridLayout*)layout )->addMultiCell( item, row, row + rowspan - 1, col, col + colspan - 1,
						    orient == TQt::Horizontal ? TQt::AlignVCenter : TQt::AlignHCenter );
    }
}

static TQImage loadImageData( TQDomElement &n2 )
{
    TQString format = n2.attribute( "format", "PNG" );
    TQString hex = n2.firstChild().toText().data();
    int n = hex.length() / 2;
    TQByteArray data( n );
    for ( int i = 0; i < n; i++ )
	data[i] = (char) hex.mid( 2 * i, 2 ).toUInt( 0, 16 );
    return loadImageData( format, n2.attribute("length").toULong(), data );
}

void TQWidgetFactory::loadImageCollection( const TQDomElement &e )
{
    TQDomElement n = e.firstChild().toElement();
    while ( !n.isNull() ) {
	if ( n.tagName() == "image" ) {
	    Image img;
	    img.name =  n.attribute( "name" );
	    TQDomElement n2 = n.firstChild().toElement();
	    while ( !n2.isNull() ) {
		if ( n2.tagName() == "data" )
		    img.img = loadImageData( n2 );
		n2 = n2.nextSibling().toElement();
	    }
	    images.append( img );
	    n = n.nextSibling().toElement();
	}
    }
}

TQImage TQWidgetFactory::loadFromCollection( const TQString &name )
{
    TQValueList<Image>::Iterator it = images.begin();
    for ( ; it != images.end(); ++it ) {
	if ( ( *it ).name == name )
	    return ( *it ).img;
    }
    return TQImage();
}

TQPixmap TQWidgetFactory::loadPixmap( const TQString& name )
{
    TQPixmap pix;
    if ( usePixmapCollection ) {
	const TQMimeSource *m = TQMimeSourceFactory::defaultFactory()->data( name );
	if ( m )
	    TQImageDrag::decode( m, pix );
    } else {
	pix.convertFromImage( loadFromCollection(name) );
    }
    return pix;
}

TQPixmap TQWidgetFactory::loadPixmap( const TQDomElement &e )
{
    return loadPixmap( e.firstChild().toText().data() );
}

TQColorGroup TQWidgetFactory::loadColorGroup( const TQDomElement &e )
{
    TQColorGroup cg;
    int r = -1;
    TQDomElement n = e.firstChild().toElement();
    TQColor col;
    while ( !n.isNull() ) {
	if ( n.tagName() == "color" ) {
	    r++;
	    cg.setColor( (TQColorGroup::ColorRole)r, (col = DomTool::readColor( n ) ) );
	} else if ( n.tagName() == "pixmap" ) {
	    TQPixmap pix = loadPixmap( n );
	    cg.setBrush( (TQColorGroup::ColorRole)r, TQBrush( col, pix ) );
	}
	n = n.nextSibling().toElement();
    }
    return cg;
}

struct Connection
{
    TQObject *sender, *receiver;
    TQCString signal, slot;
    bool operator==( const Connection &c ) const {
	return sender == c.sender && receiver == c.receiver &&
	       signal == c.signal && slot == c.slot ;
    }

    Connection() : sender( 0 ), receiver( 0 ) { }
};

class NormalizeObject : public TQObject
{
public:
    NormalizeObject() : TQObject() {}
    static TQCString normalizeSignalSlot( const char *signalSlot ) { return TQObject::normalizeSignalSlot( signalSlot ); }
};

void TQWidgetFactory::loadConnections( const TQDomElement &e, TQObject *connector )
{
    TQDomElement n = e.firstChild().toElement();
    while ( !n.isNull() ) {
	if ( n.tagName() == "connection" ) {
	    TQString lang = n.attribute( "language", "C++" );
	    TQDomElement n2 = n.firstChild().toElement();
	    Connection conn;
	    while ( !n2.isNull() ) {
		if ( n2.tagName() == "sender" ) {
		    TQString name = n2.firstChild().toText().data();
		    if ( name == "this" || qstrcmp( toplevel->name(), name ) == 0 ) {
			conn.sender = toplevel;
		    } else {
			if ( name == "this" )
			    name = toplevel->name();
			TQObjectList *l = toplevel->queryList( 0, name, false );
			if ( l ) {
			    if ( l->first() )
				conn.sender = l->first();
			    delete l;
			}
		    }
		    if ( !conn.sender )
			conn.sender = findAction( name );
		} else if ( n2.tagName() == "signal" ) {
		    conn.signal = n2.firstChild().toText().data();
		} else if ( n2.tagName() == "receiver" ) {
		    TQString name = n2.firstChild().toText().data();
		    if ( name == "this" || qstrcmp( toplevel->name(), name ) == 0 ) {
			conn.receiver = toplevel;
		    } else {
			TQObjectList *l = toplevel->queryList( 0, name, false );
			if ( l ) {
			    if ( l->first() )
				conn.receiver = l->first();
			    delete l;
			}
		    }
		} else if ( n2.tagName() == "slot" ) {
		    conn.slot = n2.firstChild().toText().data();
		}
		n2 = n2.nextSibling().toElement();
	    }

	    conn.signal = NormalizeObject::normalizeSignalSlot( conn.signal );
	    conn.slot = NormalizeObject::normalizeSignalSlot( conn.slot );

	    if ( !conn.sender || !conn.receiver ) {
		n = n.nextSibling().toElement();
		continue;
	    }

	    TQObject *sender = 0, *receiver = 0;
	    TQObjectList *l = toplevel->queryList( 0, conn.sender->name(), false );
	    if ( qstrcmp( conn.sender->name(), toplevel->name() ) == 0 ) {
		sender = toplevel;
	    } else {
		if ( !l || !l->first() ) {
		    delete l;
		    n = n.nextSibling().toElement();
		    continue;
		}
		sender = l->first();
		delete l;
	    }
	    if ( !sender )
		sender = findAction( conn.sender->name() );

	    if ( qstrcmp( conn.receiver->name(), toplevel->name() ) == 0 ) {
		receiver = toplevel;
	    } else {
		l = toplevel->queryList( 0, conn.receiver->name(), false );
		if ( !l || !l->first() ) {
		    delete l;
		    n = n.nextSibling().toElement();
		    continue;
		}
		receiver = l->first();
		delete l;
	    }

	    TQString s = "2""%1";
	    s = s.arg( conn.signal );
	    TQString s2 = "1""%1";
	    s2 = s2.arg( conn.slot );

	    TQStrList signalList = sender->metaObject()->signalNames( true );
	    TQStrList slotList = receiver->metaObject()->slotNames( true );

	    // if this is a connection to a custom slot and we have a connector, try this as receiver
	    if ( slotList.find( conn.slot ) == -1 && receiver == toplevel && connector ) {
		slotList = connector->metaObject()->slotNames( true );
		receiver = connector;
	    }

	    // avoid warnings
	    if ( signalList.find( conn.signal ) == -1 ||
		 slotList.find( conn.slot ) == -1 ) {
		n = n.nextSibling().toElement();
		continue;
	    }
	    TQObject::connect( sender, s, receiver, s2 );
	}
	n = n.nextSibling().toElement();
    }
}

void TQWidgetFactory::loadTabOrder( const TQDomElement &e )
{
    TQWidget *last = 0;
    TQDomElement n = e.firstChild().toElement();
    while ( !n.isNull() ) {
	if ( n.tagName() == "tabstop" ) {
	    TQString name = n.firstChild().toText().data();
	    TQObjectList *l = toplevel->queryList( 0, name, false );
	    if ( l ) {
		if ( l->first() ) {
		    TQWidget *w = (TQWidget*)l->first();
		    if ( last )
			toplevel->setTabOrder( last, w );
		    last = w;
		}
		delete l;
	    }
	}
	n = n.nextSibling().toElement();
    }
}

void TQWidgetFactory::createListViewColumn( TQListView *lv, const TQString& txt,
					   const TQPixmap& pix, bool clickable,
					   bool resizable )
{
    if ( pix.isNull() ) {
	lv->addColumn( txt );
    } else {
	lv->addColumn( pix, txt );
    }

    int i = lv->header()->count() - 1;
    if ( !pix.isNull() )
	lv->header()->setLabel( i, pix, txt );
    if ( !clickable )
	lv->header()->setClickEnabled( clickable, i );
    if ( !resizable )
	lv->header()->setResizeEnabled( resizable, i );
}

#ifndef TQT_NO_TABLE
void TQWidgetFactory::createTableColumnOrRow( TQTable *table, const TQString& txt,
					     const TQPixmap& pix,
					     const TQString& field, bool isRow )
{
#ifndef TQT_NO_SQL
    bool isSql = table->inherits( "TQDataTable" );
#endif
    if ( isRow )
	table->setNumRows( table->numRows() + 1 );
    else {
#ifndef TQT_NO_SQL
	if ( !isSql )
#endif
	    table->setNumCols( table->numCols() + 1 );
    }

    TQValueList<Field> fieldMap;
    if ( fieldMaps.find( table ) != fieldMaps.end() ) {
	fieldMap = *fieldMaps.find( table );
	fieldMaps.remove( table );
    }

    int i = isRow ? table->numRows() - 1 : table->numCols() - 1;
    TQHeader *h = !isRow ? table->horizontalHeader() : table->verticalHeader();
    if ( !pix.isNull() ) {
#ifndef TQT_NO_SQL
	if ( isSql )
	    ((TQDataTable*)table)->addColumn( field, txt, -1, pix );
	else
#endif
	    h->setLabel( i, pix, txt );
    } else {
#ifndef TQT_NO_SQL
	if ( isSql )
	    ((TQDataTable*)table)->addColumn( field, txt );
	else
#endif
	    h->setLabel( i, txt );
    }
    if ( !isRow && !field.isEmpty() ) {
	fieldMap.append( Field( txt, pix, field ) );
	fieldMaps.insert( table, fieldMap );
    }

}
#endif

void TQWidgetFactory::createColumn( const TQDomElement &e, TQWidget *widget )
{
    if ( widget->inherits( "TQListView" ) && e.tagName() == "column" ) {
	TQListView *lv = (TQListView*)widget;
	TQDomElement n = e.firstChild().toElement();
	TQPixmap pix;
	TQString txt;
	bool clickable = true, resizable = true;
	while ( !n.isNull() ) {
	    if ( n.tagName() == "property" ) {
		TQString attrib = n.attribute( "name" );
		TQVariant v = DomTool::elementToVariant( n.firstChild().toElement(), TQVariant() );
		if ( attrib == "text" )
		    txt = translate( v.toString() );
		else if ( attrib == "pixmap" )
		    pix = loadPixmap( n.firstChild().toElement().toElement() );
		else if ( attrib == "clickable" )
		    clickable = v.toBool();
		else if ( attrib == "resizable" || attrib == "resizeable" )
		    resizable = v.toBool();
	    }
	    n = n.nextSibling().toElement();
	}
	createListViewColumn( lv, txt, pix, clickable, resizable );
    }
#ifndef TQT_NO_TABLE
    else if ( widget->inherits( "TQTable" ) ) {
	TQTable *table = (TQTable*)widget;

	TQDomElement n = e.firstChild().toElement();
	TQPixmap pix;
	TQString txt;
	TQString field;

	while ( !n.isNull() ) {
	    if ( n.tagName() == "property" ) {
		TQString attrib = n.attribute( "name" );
		TQVariant v = DomTool::elementToVariant( n.firstChild().toElement(), TQVariant() );
		if ( attrib == "text" )
		    txt = translate( v.toString() );
		else if ( attrib == "pixmap" ) {
		    if ( !n.firstChild().firstChild().toText().data().isEmpty() )
			pix = loadPixmap( n.firstChild().toElement().toElement() );
		} else if ( attrib == "field" )
		    field = translate( v.toString() );
	    }
	    n = n.nextSibling().toElement();
	}
	createTableColumnOrRow( table, txt, pix, field, e.tagName() == "row" );
    }
#endif
}

void TQWidgetFactory::loadItem( const TQDomElement &e, TQPixmap &pix, TQString &txt, bool &hasPixmap )
{
    TQDomElement n = e;
    hasPixmap = false;
    while ( !n.isNull() ) {
	if ( n.tagName() == "property" ) {
	    TQString attrib = n.attribute( "name" );
	    TQVariant v = DomTool::elementToVariant( n.firstChild().toElement(), TQVariant() );
	    if ( attrib == "text" )
		txt = translate( v.toString() );
	    else if ( attrib == "pixmap" ) {
		pix = loadPixmap( n.firstChild().toElement() );
		hasPixmap = !pix.isNull();
	    }
	}
	n = n.nextSibling().toElement();
    }
}

void TQWidgetFactory::createItem( const TQDomElement &e, TQWidget *widget, TQListViewItem *i )
{
    if ( widget->inherits( "TQListBox" ) || widget->inherits( "TQComboBox" ) ) {
	TQDomElement n = e.firstChild().toElement();
	TQPixmap pix;
	bool hasPixmap = false;
	TQString txt;
	loadItem( n, pix, txt, hasPixmap );
	TQListBox *lb = 0;
	if ( widget->inherits( "TQListBox" ) ) {
	    lb = (TQListBox*)widget;
	} else {
            TQComboBox *cb = (TQComboBox*)widget;
	    lb = cb->listBox();
            if (!lb) {
                lb = new TQListBox(cb);
                cb->setListBox(lb);
            }
        }
	if ( hasPixmap ) {
	    new TQListBoxPixmap( lb, pix, txt );
	} else {
	    new TQListBoxText( lb, txt );
	}
#ifndef TQT_NO_ICONVIEW
    } else if ( widget->inherits( "TQIconView" ) ) {
	TQDomElement n = e.firstChild().toElement();
	TQPixmap pix;
	bool hasPixmap = false;
	TQString txt;
	loadItem( n, pix, txt, hasPixmap );

	TQIconView *iv = (TQIconView*)widget;
	new TQIconViewItem( iv, txt, pix );
#endif
    } else if ( widget->inherits( "TQListView" ) ) {
	TQDomElement n = e.firstChild().toElement();
	TQPixmap pix;
	TQValueList<TQPixmap> pixmaps;
	TQStringList textes;
	TQListViewItem *item = 0;
	TQListView *lv = (TQListView*)widget;
	if ( i )
	    item = new TQListViewItem( i, d->lastItem );
	else
	    item = new TQListViewItem( lv, d->lastItem );
	while ( !n.isNull() ) {
	    if ( n.tagName() == "property" ) {
		TQString attrib = n.attribute( "name" );
		TQVariant v = DomTool::elementToVariant( n.firstChild().toElement(), TQVariant() );
		if ( attrib == "text" )
		    textes << translate( v.toString() );
		else if ( attrib == "pixmap" ) {
		    TQString s = v.toString();
		    if ( s.isEmpty() ) {
			pixmaps << TQPixmap();
		    } else {
			pix = loadPixmap( n.firstChild().toElement() );
			pixmaps << pix;
		    }
		}
	    } else if ( n.tagName() == "item" ) {
		item->setOpen( true );
		createItem( n, widget, item );
	    }

	    n = n.nextSibling().toElement();
	}

	for ( int i = 0; i < lv->columns(); ++i ) {
	    item->setText( i, textes[ i ] );
	    item->setPixmap( i, pixmaps[ i ] );
	}
	d->lastItem = item;
    }
}



void TQWidgetFactory::loadChildAction( TQObject *parent, const TQDomElement &e )
{
    TQDomElement n = e;
    TQAction *a = 0;
    bool hasMenuText = false;
    if ( n.tagName() == "action" ) {
	a = new TQAction( parent );
	TQDomElement n2 = n.firstChild().toElement();
	
	while ( !n2.isNull() ) {
	    if ( n2.tagName() == "property" ) {
		TQString prop(n2.attribute("name"));
		if (prop == "menuText")
		    hasMenuText = true;
		setProperty( a, prop, n2.firstChild().toElement() );
	    }
	    n2 = n2.nextSibling().toElement();
	}
	if ( !parent->inherits( "TQAction" ) )
	    actionList.append( a );
    } else if ( n.tagName() == "actiongroup" ) {
	a = new TQActionGroup( parent );
	TQDomElement n2 = n.firstChild().toElement();
	while ( !n2.isNull() ) {
	    if ( n2.tagName() == "property" ) {
		TQString prop(n2.attribute("name"));
		if (prop == "menuText")
		    hasMenuText = true;
		setProperty( a, prop, n2.firstChild().toElement() );
	    } else if ( n2.tagName() == "action" ||
			n2.tagName() == "actiongroup" ) {
		loadChildAction( a, n2 );

	    }
	    n2 = n2.nextSibling().toElement();
	}
	if ( !parent->inherits( "TQAction" ) )
	    actionList.append( a );
    }

    if (a && !hasMenuText && !a->text().isEmpty() && uiFileVersion < "3.3")
	a->setMenuText(a->text());
}

void TQWidgetFactory::loadActions( const TQDomElement &e )
{
    TQDomElement n = e.firstChild().toElement();
    while ( !n.isNull() ) {
	if ( n.tagName() == "action" ) {
	    loadChildAction( toplevel, n );
	} else if ( n.tagName() == "actiongroup" ) {
	    loadChildAction( toplevel, n );
	}
	n = n.nextSibling().toElement();
    }
}

void TQWidgetFactory::loadToolBars( const TQDomElement &e )
{
    TQDomElement n = e.firstChild().toElement();
    TQMainWindow *mw = ( (TQMainWindow*)toplevel );
    TQToolBar *tb = 0;
    while ( !n.isNull() ) {
	if ( n.tagName() == "toolbar" ) {
	    TQt::Dock dock = (TQt::Dock)n.attribute( "dock" ).toInt();
	    tb = new TQToolBar( TQString::null, mw, dock );
	    tb->setLabel( n.attribute( "label" ) );
	    tb->setName( n.attribute( "name" ) );
	    TQDomElement n2 = n.firstChild().toElement();
	    while ( !n2.isNull() ) {
		if ( n2.tagName() == "action" ) {
		    TQAction *a = findAction( n2.attribute( "name" ) );
		    if ( a )
			a->addTo( tb );
		} else if ( n2.tagName() == "separator" ) {
		    tb->addSeparator();
		} else if ( n2.tagName() == "widget" ) {
		    (void)createWidgetInternal( n2, tb, 0, n2.attribute( "class", "TQWidget" ) );
		} else if ( n2.tagName() == "property" ) {
		    setProperty( tb, n2.attribute( "name" ), n2.firstChild().toElement() );
		}
		n2 = n2.nextSibling().toElement();
	    }
	}
	n = n.nextSibling().toElement();
    }
}

void TQWidgetFactory::loadMenuBar( const TQDomElement &e )
{
    TQDomElement n = e.firstChild().toElement();
    TQMainWindow *mw = ( (TQMainWindow*)toplevel );
    TQMenuBar *mb = mw->menuBar();
    while ( !n.isNull() ) {
	if ( n.tagName() == "item" ) {
	    TQPopupMenu *popup = new TQPopupMenu( mw );
	    loadPopupMenu( popup, n );
	    popup->setName( n.attribute( "name" ) );
	    mb->insertItem( translate( n.attribute( "text" ) ), popup );
	} else if ( n.tagName() == "property" ) {
	    setProperty( mb, n.attribute( "name" ), n.firstChild().toElement() );
	} else if ( n.tagName() == "separator" ) {
	    mb->insertSeparator();
	}
	n = n.nextSibling().toElement();
    }
}

void TQWidgetFactory::loadPopupMenu( TQPopupMenu *p, const TQDomElement &e )
{
    TQMainWindow *mw = ( (TQMainWindow*)toplevel );
    TQDomElement n = e.firstChild().toElement();
    while ( !n.isNull() ) {
	if ( n.tagName() == "action" || n.tagName() == "actiongroup" ) {
	    TQAction *a = findAction( n.attribute( "name" ) );
	    TQDomElement n2 = n.nextSibling().toElement();
	    if ( n2.tagName() == "item") { // load submenu
		TQPopupMenu *popup = new TQPopupMenu( mw );
		popup->setName( n2.attribute( "name" ) );
		if ( a ) {
		    p->setAccel( a->accel(), p->insertItem( a->iconSet(),
					     translate( n2.attribute( "text" ).utf8().data() ),
					     popup ) );
		} else {
		    p->insertItem( translate( n2.attribute( "text" ).utf8().data() ), popup );
		}
		loadPopupMenu( popup, n2 );
		n = n2;
	    } else {
		if ( a ) {
		    a->addTo( p );
		}
	    }
	    a = 0;
	} else if ( n.tagName() == "separator" ) {
	    p->insertSeparator();
	}
	n = n.nextSibling().toElement();
    }
}

// compatibility with early 3.0 betas
// ### remove for 4.0
void TQWidgetFactory::loadFunctions( const TQDomElement & )
{
}

TQAction *TQWidgetFactory::findAction( const TQString &name )
{
    for ( TQAction *a = actionList.first(); a; a = actionList.next() ) {
	if ( TQString( a->name() ) == name )
	    return a;
	TQAction *ac = (TQAction*)a->child( name.latin1(), "TQAction" );
	if ( ac )
	    return ac;
    }
    return 0;
}

/*!
    If you use a pixmap collection (which is the default for new
    projects) rather than saving the pixmaps within the .ui XML file,
    you must load the pixmap collection. TQWidgetFactory looks in the
    default TQMimeSourceFactory for the pixmaps. Either add it there
    manually, or call this function and specify the directory where
    the images can be found, as \a dir. This is normally the
    directory called \c images in the project's directory.
*/

void TQWidgetFactory::loadImages( const TQString &dir )
{
    TQDir d( dir );
    TQStringList l = d.entryList( TQDir::Files );
    for ( TQStringList::Iterator it = l.begin(); it != l.end(); ++it )
	TQMimeSourceFactory::defaultFactory()->setPixmap( *it, TQPixmap( d.path() + "/" + *it, "PNG" ) );

}

void TQWidgetFactory::loadExtraSource()
{
    if ( !qwf_language || !languageInterfaceManager )
	return;
    TQString lang = *qwf_language;
    LanguageInterface *iface = 0;
    languageInterfaceManager->queryInterface( lang, &iface );
    if ( !iface )
	return;
    TQFile f( qwf_currFileName + iface->formCodeExtension() );
    if ( f.open( IO_ReadOnly ) ) {
	TQTextStream ts( &f );
	code = ts.read();
    }
}

TQString TQWidgetFactory::translate( const TQString& sourceText, const TQString& comment )
{
    return tqApp->translate( d->translationContext, sourceText.utf8(), comment.utf8(),
			    TQApplication::UnicodeUTF8 );
}

TQString TQWidgetFactory::translate( const char *sourceText, const char *comment )
{
    return tqApp->translate( d->translationContext, sourceText, comment,
			    TQApplication::UnicodeUTF8 );
}
