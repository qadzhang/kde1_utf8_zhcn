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

#include "widgetdatabase.h"
#include "../interfaces/widgetinterface.h"

#include <ntqapplication.h>
#define NO_STATIC_COLORS
#include <globaldefs.h>
#include <ntqstrlist.h>
#include <ntqdict.h>
#include <ntqfile.h>
#include <ntqtextstream.h>
#include <ntqcleanuphandler.h>
#include <ntqfeatures.h>

#include <stdlib.h>

const int dbsize = 300;
const int dbcustom = 200;
const int dbdictsize = 211;
static WidgetDatabaseRecord* db[ dbsize ];
static TQDict<int> *className2Id = 0;
static int dbcount  = 0;
static int dbcustomcount = 200;
static TQStrList *wGroups;
static TQStrList *invisibleGroups;
static bool whatsThisLoaded = false;
static TQPluginManager<WidgetInterface> *widgetPluginManager = 0;
static bool plugins_set_up = false;
static bool was_in_setup = false;

TQCleanupHandler<TQPluginManager<WidgetInterface> > cleanup_manager;

WidgetDatabaseRecord::WidgetDatabaseRecord()
{
    isForm = false;
    isContainer = false;
    icon = 0;
    nameCounter = 0;
    isCommon = false;
    isPlugin = false;
}

WidgetDatabaseRecord::~WidgetDatabaseRecord()
{
    delete icon;
}


/*!
  \class WidgetDatabase widgetdatabase.h
  \brief The WidgetDatabase class holds information about widgets

  The WidgetDatabase holds information about widgets like toolTip(),
  iconSet(), ... It works Id-based, so all access functions take the
  widget id as parameter. To get the id for a widget (classname), use
  idFromClassName().

  All access functions are static.  Having multiple widgetdatabases in
  one application doesn't make sense anyway and so you don't need more
  than an instance of the widgetdatabase.

  For creating widgets, layouts, etc. see WidgetFactory.
*/

/*!
  Creates widget database. Does nothing.
*/

WidgetDatabase::WidgetDatabase()
{
}

/*!  Sets up the widget database. If the static widgetdatabase already
  exists, the functions returns immediately.
*/

void WidgetDatabase::setupDataBase( int id )
{
    was_in_setup = true;
#ifndef UIC
    Q_UNUSED( id )
    if ( dbcount )
	return;
#else
    if ( dbcount && id != -2 )
	return;
    if ( dbcount && !plugins_set_up ) {
	setupPlugins();
	return;
    }
    if ( dbcount && plugins_set_up)
	return;
#endif

    wGroups = new TQStrList;
    invisibleGroups = new TQStrList;
    invisibleGroups->append( "Forms" );
    invisibleGroups->append( "Temp" );
    className2Id = new TQDict<int>( dbdictsize );
    className2Id->setAutoDelete( true );

    WidgetDatabaseRecord *r = 0;

    r = new WidgetDatabaseRecord;
    r->iconSet = "designer_pushbutton.png";
    r->name = "TQPushButton";
    r->group = widgetGroup( "Buttons" );
    r->toolTip = "Push Button";
    r->isCommon = true;

    append( r );

    r = new WidgetDatabaseRecord;
    r->iconSet = "designer_toolbutton.png";
    r->name = "TQToolButton";
    r->group = widgetGroup( "Buttons" );
    r->toolTip = "Tool Button";

    append( r );

    r = new WidgetDatabaseRecord;
    r->iconSet = "designer_radiobutton.png";
    r->name = "TQRadioButton";
    r->group = widgetGroup( "Buttons" );
    r->toolTip = "Radio Button";
    r->isCommon = true;

    append( r );

    r = new WidgetDatabaseRecord;
    r->iconSet = "designer_checkbox.png";
    r->name = "TQCheckBox";
    r->group = widgetGroup( "Buttons" );
    r->toolTip = "Check Box";
    r->isCommon = true;

    append( r );

    r = new WidgetDatabaseRecord;
    r->iconSet = "designer_groupbox.png";
    r->name = "TQGroupBox";
    r->group = widgetGroup( "Containers" );
    r->toolTip = "Group Box";
    r->isContainer = true;

    append( r );

    r = new WidgetDatabaseRecord;
    r->iconSet = "designer_buttongroup.png";
    r->name = "TQButtonGroup";
    r->group = widgetGroup( "Containers" );
    r->toolTip = "Button Group";
    r->isContainer = true;
    r->isCommon = true;

    append( r );

    r = new WidgetDatabaseRecord;
    r->iconSet = "designer_frame.png";
    r->name = "TQFrame";
    r->group = widgetGroup( "Containers" );
    r->toolTip = "Frame";
    r->isContainer = true;

    append( r );

    r = new WidgetDatabaseRecord;
    r->iconSet = "designer_tabwidget.png";
    r->name = "TQTabWidget";
    r->group = widgetGroup( "Containers" );
    r->toolTip = "Tabwidget";
    r->isContainer = true;

    append( r );

    r = new WidgetDatabaseRecord;
    r->iconSet = "designer_widgetstack.png";
    r->name = "TQWidgetStack";
    r->group = widgetGroup( "Containers" );
    r->toolTip = "Widget Stack";
    r->isContainer = true;

    append( r );

    r = new WidgetDatabaseRecord;
    r->iconSet = "designer_toolbox.png";
    r->name = "TQToolBox";
    r->group = widgetGroup( "Containers" );
    r->toolTip = "Tool Box";
    r->isContainer = true;

    append( r );

    r = new WidgetDatabaseRecord;
    r->iconSet = "designer_listbox.png";
    r->name = "TQListBox";
    r->group = widgetGroup( "Views" );
    r->toolTip = "List Box";
    r->isCommon = true;

    append( r );

    r = new WidgetDatabaseRecord;
    r->iconSet = "designer_listview.png";
    r->name = "TQListView";
    r->group = widgetGroup( "Views" );
    r->toolTip = "List View";

    append( r );

#if !defined(TQT_NO_ICONVIEW) || defined(UIC)
    r = new WidgetDatabaseRecord;
    r->iconSet = "designer_iconview.png";
    r->name = "TQIconView";
    r->group = widgetGroup( "Views" );
    r->toolTip = "Icon View";

    append( r );
#endif

#if !defined(TQT_NO_TABLE)
    r = new WidgetDatabaseRecord;
    r->iconSet = "designer_table.png";
    r->name = "TQTable";
    r->group = widgetGroup( "Views" );
    r->toolTip = "Table";

    append( r );
#endif

#if !defined(TQT_NO_SQL)
    r = new WidgetDatabaseRecord;
    r->iconSet = "designer_datatable.png";
    r->includeFile = "ntqdatatable.h";
    r->name = "TQDataTable";
    r->group = widgetGroup( "Database" );
    r->toolTip = "Data Table";

    append( r );
#endif

    r = new WidgetDatabaseRecord;
    r->iconSet = "designer_lineedit.png";
    r->name = "TQLineEdit";
    r->group = widgetGroup( "Input" );
    r->toolTip = "Line Edit";
    r->isCommon = true;

    append( r );

    r = new WidgetDatabaseRecord;
    r->iconSet = "designer_spinbox.png";
    r->name = "TQSpinBox";
    r->group = widgetGroup( "Input" );
    r->toolTip = "Spin Box";
    r->isCommon = true;

    append( r );

    r = new WidgetDatabaseRecord;
    r->iconSet = "designer_dateedit.png";
    r->name = "TQDateEdit";
    r->group = widgetGroup( "Input" );
    r->toolTip = "Date Edit";
    r->includeFile = "ntqdatetimeedit.h";

    append( r );

    r = new WidgetDatabaseRecord;
    r->iconSet = "designer_timeedit.png";
    r->name = "TQTimeEdit";
    r->group = widgetGroup( "Input" );
    r->toolTip = "Time Edit";
    r->includeFile = "ntqdatetimeedit.h";

    append( r );

    r = new WidgetDatabaseRecord;
    r->iconSet = "designer_datetimeedit.png";
    r->name = "TQDateTimeEdit";
    r->group = widgetGroup( "Input" );
    r->toolTip = "Date-Time Edit";
    r->includeFile = "ntqdatetimeedit.h";

    append( r );

    r = new WidgetDatabaseRecord;
    r->iconSet = "designer_multilineedit.png";
    r->name = "TQMultiLineEdit";
    r->group = widgetGroup( "Temp" );
    r->toolTip = "Multi Line Edit";

    append( r );

    r = new WidgetDatabaseRecord;
    r->iconSet = "designer_richtextedit.png";
    r->name = "TQTextEdit";
    r->group = widgetGroup( "Input" );
    r->toolTip = "Rich Text Edit";
    r->isCommon = true;

    append( r );

    r = new WidgetDatabaseRecord;
    r->iconSet = "designer_combobox.png";
    r->name = "TQComboBox";
    r->group = widgetGroup( "Input" );
    r->toolTip = "Combo Box";
    r->isCommon = true;

    append( r );

    r = new WidgetDatabaseRecord;
    r->iconSet = "designer_slider.png";
    r->name = "TQSlider";
    r->group = widgetGroup( "Input" );
    r->toolTip = "Slider";

    append( r );

    r = new WidgetDatabaseRecord;
    r->iconSet = "designer_scrollbar.png";
    r->name = "TQScrollBar";
    r->group = widgetGroup( "Input" );
    r->toolTip = "Scrollbar";

    append( r );

    r = new WidgetDatabaseRecord;
    r->iconSet = "designer_dial.png";
    r->name = "TQDial";
    r->group = widgetGroup( "Input" );
    r->toolTip = "Dial";

    append( r );

    r = new WidgetDatabaseRecord;
    r->iconSet = "designer_label.png";
    r->name = "TQLabel";
    r->group = widgetGroup( "Temp" );
    r->toolTip = "Label";

    append( r );

    r = new WidgetDatabaseRecord;
    r->iconSet = "designer_label.png";
    r->name = "TextLabel";
    r->group = widgetGroup( "Display" );
    r->toolTip = "Text Label";
    r->whatsThis = "The Text Label provides a widget to display static text.";
    r->isCommon = true;

    append( r );

    r = new WidgetDatabaseRecord;
    r->iconSet = "designer_pixlabel.png";
    r->name = "PixmapLabel";
    r->group = widgetGroup( "Display" );
    r->toolTip = "Pixmap Label";
    r->whatsThis = "The Pixmap Label provides a widget to display pixmaps.";

    append( r );

    r = new WidgetDatabaseRecord;
    r->iconSet = "designer_lcdnumber.png";
    r->name = "TQLCDNumber";
    r->group = widgetGroup( "Display" );
    r->toolTip = "LCD Number";

    append( r );

    r = new WidgetDatabaseRecord;
    r->iconSet = "designer_line.png";
    r->name = "Line";
    r->group = widgetGroup( "Display" );
    r->toolTip = "Line";
    r->includeFile = "ntqframe.h";
    r->whatsThis = "The Line widget provides horizontal and vertical lines.";

    append( r );

    r = new WidgetDatabaseRecord;
    r->iconSet = "designer_progress.png";
    r->name = "TQProgressBar";
    r->group = widgetGroup( "Display" );
    r->toolTip = "Progress Bar";

    append( r );

    r = new WidgetDatabaseRecord;
    r->iconSet = "designer_textview.png";
    r->name = "TQTextView";
    r->group = widgetGroup( "Temp" );
    r->toolTip = "Text View";

    append( r );

    r = new WidgetDatabaseRecord;
    r->iconSet = "designer_textbrowser.png";
    r->name = "TQTextBrowser";
    r->group = widgetGroup( "Display" );
    r->toolTip = "Text Browser";

    append( r );

    r = new WidgetDatabaseRecord;
    r->iconSet = "designer_spacer.png";
    r->name = "Spacer";
    r->group = widgetGroup( "Temp" );
    r->toolTip = "Spacer";
    r->whatsThis = "The Spacer provides horizontal and vertical spacing to be able to manipulate the behaviour of layouts.";

    append( r );

    r = new WidgetDatabaseRecord;
    r->name = "TQWidget";
    r->isForm = true;
    r->group = widgetGroup( "Forms" );

    append( r );

    r = new WidgetDatabaseRecord;
    r->name = "TQDialog";
    r->group = widgetGroup( "Forms" );
    r->isForm = true;

    append( r );

    r = new WidgetDatabaseRecord;
    r->name = "TQWizard";
    r->group = widgetGroup( "Forms" );
    r->isContainer = true;

    append( r );

    r = new WidgetDatabaseRecord;
    r->name = "TQDesignerWizard";
    r->group = widgetGroup( "Forms" );
    r->isContainer = true;

    append( r );

    r = new WidgetDatabaseRecord;
    r->name = "TQLayoutWidget";
    r->group = widgetGroup( "Temp" );
    r->includeFile = "";
    r->isContainer = true;

    append( r );

    r = new WidgetDatabaseRecord;
    r->name = "TQSplitter";
    r->group = widgetGroup( "Temp" );
    r->includeFile = "ntqsplitter.h";
    r->isContainer = true;

    append( r );

    r = new WidgetDatabaseRecord;
    r->iconSet = "designer_tabwidget.png";
    r->name = "TQDesignerTabWidget";
    r->group = widgetGroup( "Temp" );
    r->isContainer = true;

    append( r );

    r = new WidgetDatabaseRecord;
    r->iconSet = "designer_tabwidget.png";
    r->name = "TQDesignerWidget";
    r->group = widgetGroup( "Temp" );
    r->isContainer = true;

    append( r );

    r = new WidgetDatabaseRecord;
    r->iconSet = "designer_tabwidget.png";
    r->name = "TQDesignerDialog";
    r->group = widgetGroup( "Temp" );
    r->isContainer = true;

    append( r );

    r = new WidgetDatabaseRecord;
    r->iconSet = "";
    r->name = "TQMainWindow";
    r->includeFile = "ntqmainwindow.h";
    r->group = widgetGroup( "Temp" );
    r->isContainer = true;

    append( r );

    r = new WidgetDatabaseRecord;
    r->iconSet = "";
    r->name = "TQDesignerAction";
    r->includeFile = "ntqaction.h";
    r->group = widgetGroup( "Temp" );
    r->isContainer = false;

    append( r );

    r = new WidgetDatabaseRecord;
    r->iconSet = "";
    r->name = "TQDesignerActionGroup";
    r->includeFile = "ntqaction.h";
    r->group = widgetGroup( "Temp" );
    r->isContainer = false;

    append( r );

    r = new WidgetDatabaseRecord;
    r->iconSet = "";
    r->name = "TQScrollView";
    r->includeFile = "ntqscrollview.h";
    r->group = widgetGroup( "Temp" );
    r->isContainer = true;

    append( r );

#ifndef TQT_NO_SQL
    r = new WidgetDatabaseRecord;
    r->iconSet = "";
    r->name = "TQDataBrowser";
    r->includeFile = "ntqdatabrowser.h";
    r->group = widgetGroup( "Database" );
    r->toolTip = "Data Browser";
    r->iconSet = "designer_databrowser.png";
    r->isContainer = true;

    append( r );

    r = new WidgetDatabaseRecord;
    r->iconSet = "";
    r->name = "TQDataView";
    r->includeFile = "ntqdataview.h";
    r->group = widgetGroup( "Database" );
    r->toolTip = "Data View";
    r->iconSet = "designer_dataview.png";
    r->isContainer = true;

    append( r );
#endif

#ifndef UIC
    setupPlugins();
#endif
}

void WidgetDatabase::setupPlugins()
{
    if ( plugins_set_up )
	return;
    plugins_set_up = true;
    TQStringList widgets = widgetManager()->featureList();
    for ( TQStringList::Iterator it = widgets.begin(); it != widgets.end(); ++it ) {
	if ( hasWidget( *it ) )
	    continue;
	WidgetDatabaseRecord *r = new WidgetDatabaseRecord;
	WidgetInterface *iface = 0;
	widgetManager()->queryInterface( *it, &iface );
	if ( !iface )
	    continue;

#ifndef UIC
	TQIconSet icon = iface->iconSet( *it );
	if ( !icon.pixmap().isNull() )
	    r->icon = new TQIconSet( icon );
#endif
	TQString grp = iface->group( *it );
	if ( grp.isEmpty() )
	    grp = "3rd party widgets";
	r->group = widgetGroup( grp );
	r->toolTip = iface->toolTip( *it );
	r->whatsThis = iface->whatsThis( *it );
	r->includeFile = iface->includeFile( *it );
	r->isContainer = iface->isContainer( *it );
	r->name = *it;
	r->isPlugin = true;
	append( r );
	iface->release();
    }
}

/*!
  Returns the number of elements in the widget database.
*/

int WidgetDatabase::count()
{
    setupDataBase( -1 );
    return dbcount;
}

/*!
  Returns the id at which the ids of custom widgets start.
*/

int WidgetDatabase::startCustom()
{
    setupDataBase( -1 );
    return dbcustom;
}

/*!
  Returns the iconset which represents the class registered as \a id.
*/

TQIconSet WidgetDatabase::iconSet( int id )
{
    setupDataBase( id );
    WidgetDatabaseRecord *r = at( id );
    if ( !r )
	return TQIconSet();
#if !defined(UIC) && !defined(RESOURCE)
    if ( !r->icon ) {
	if ( r->iconSet.isEmpty() )
	    return TQIconSet();
	TQPixmap pix = TQPixmap::fromMimeSource( r->iconSet );
	if ( pix.isNull() )
	    pix = TQPixmap( r->iconSet );
	r->icon = new TQIconSet( pix );
    }
    return *r->icon;
#else
    return TQIconSet();
#endif
}

/*!
  Returns the classname of the widget which is registered as \a id.
*/

TQString WidgetDatabase::className( int id )
{
    setupDataBase( id );
    WidgetDatabaseRecord *r = at( id );
    if ( !r )
	return TQString::null;
    return r->name;
}

/*!
  Returns the group the widget registered as \a id belongs to.
*/

TQString WidgetDatabase::group( int id )
{
    setupDataBase( id );
    WidgetDatabaseRecord *r = at( id );
    if ( !r )
	return TQString::null;
    return r->group;
}

/*!
  Returns the tooltip text of the widget which is registered as \a id.
*/

TQString WidgetDatabase::toolTip( int id )
{
    setupDataBase( id );
    WidgetDatabaseRecord *r = at( id );
    if ( !r )
	return TQString::null;
    return r->toolTip;
}

/*!
  Returns the what's this? text of the widget which is registered as \a id.
*/

TQString WidgetDatabase::whatsThis( int id )
{
    setupDataBase( id );
    WidgetDatabaseRecord *r = at( id );
    if ( !r )
	return TQString::null;
    return r->whatsThis;
}

/*!
  Returns the include file if the widget which is registered as \a id.
*/

TQString WidgetDatabase::includeFile( int id )
{
    setupDataBase( id );
    WidgetDatabaseRecord *r = at( id );
    if ( !r )
	return TQString::null;
    if ( r->includeFile.isNull() ) {
	TQString rq = r->name;
	if ( rq[ 0 ] == 'T' && rq[ 1 ] == 'Q')
	    rq = rq.mid(1);
	return "nt" + rq.lower() + ".h";
    }
    return r->includeFile;
}

/*!  Returns whether the widget registered as \a id is a form.
*/
bool WidgetDatabase::isForm( int id )
{
    setupDataBase( id );
    WidgetDatabaseRecord *r = at( id );
    if ( !r )
	return false;
    return r->isForm;
}

/*!  Returns whether the widget registered as \a id can have children.
*/

bool WidgetDatabase::isContainer( int id )
{
    setupDataBase( id );
    WidgetDatabaseRecord *r = at( id );
    if ( !r )
	return false;
    return r->isContainer || r->isForm;
}

bool WidgetDatabase::isCommon( int id )
{
    setupDataBase( id );
    WidgetDatabaseRecord *r = at( id );
    if ( !r )
	return false;
    return r->isCommon;
}

TQString WidgetDatabase::createWidgetName( int id )
{
    setupDataBase( id );
    TQString n = className( id );
    if ( n == "TQLayoutWidget" )
	n = "Layout";
    if ( n[ 0 ] == 'T' && n[ 1 ] == 'Q' && n[ 2 ].lower() != n[ 2 ] )
	n = n.mid( 2 );
    int colonColon = n.findRev( "::" );
    if ( colonColon != -1 )
	n = n.mid( colonColon + 2 );

    WidgetDatabaseRecord *r = at( id );
    if ( !r )
	return n;
    n += TQString::number( ++r->nameCounter );
    n[0] = n[0].lower();
    return n;
}

/*!  Returns the id for \a name or -1 if \a name is unknown.
 */
int WidgetDatabase::idFromClassName( const TQString &name )
{
    setupDataBase( -1 );
    if ( name.isEmpty() )
	return 0;
    int *i = className2Id->find( name );
    if ( i )
	return *i;
    if ( name == "FormWindow" )
	return idFromClassName( "TQLayoutWidget" );
#ifdef UIC
    setupDataBase( -2 );
    i = className2Id->find( name );
    if ( i )
	return *i;
#endif
    return -1;
}

bool WidgetDatabase::hasWidget( const TQString &name )
{
    return className2Id->find( name ) != 0;
}

WidgetDatabaseRecord *WidgetDatabase::at( int index )
{
    if ( index < 0 )
	return 0;
    if ( index >= dbcustom && index < dbcustomcount )
	return db[ index ];
    if ( index < dbcount )
	return db[ index ];
    return 0;
}

void WidgetDatabase::insert( int index, WidgetDatabaseRecord *r )
{
    if ( index < 0 || index >= dbsize )
	return;
    db[ index ] = r;
    className2Id->insert( r->name, new int( index ) );
    if ( index < dbcustom )
	dbcount = TQMAX( dbcount, index );
}

void WidgetDatabase::append( WidgetDatabaseRecord *r )
{
    if ( !was_in_setup )
	setupDataBase( -1 );
    insert( dbcount++, r );
}

TQString WidgetDatabase::widgetGroup( const TQString &g )
{
    if ( wGroups->find( g ) == -1 )
	wGroups->append( g );
    return g;
}

bool WidgetDatabase::isGroupEmpty( const TQString &grp )
{
    WidgetDatabaseRecord *r = 0;
    for ( int i = 0; i < dbcount; ++i ) {
	if ( !( r = db[ i ] ) )
	    continue;
	if ( r->group == grp )
	    return false;
    }
    return true;
}

TQString WidgetDatabase::widgetGroup( int i )
{
    setupDataBase( -1 );
    if ( i >= 0 && i < (int)wGroups->count() )
	return wGroups->at( i );
    return TQString::null;
}

int WidgetDatabase::numWidgetGroups()
{
    setupDataBase( -1 );
    return wGroups->count();
}

bool WidgetDatabase::isGroupVisible( const TQString &g )
{
    setupDataBase( -1 );
    return invisibleGroups->find( g ) == -1;
}

int WidgetDatabase::addCustomWidget( WidgetDatabaseRecord *r )
{
    insert( dbcustomcount++, r );
    return dbcustomcount - 1;
}

void WidgetDatabase::customWidgetClassNameChanged( const TQString &oldName,
						   const TQString &newName )
{
    int id = idFromClassName( oldName );
    if ( id == -1 )
	return;
    WidgetDatabaseRecord *r = db[ id ];
    r->name = newName;
    className2Id->remove( oldName );
    className2Id->insert( newName, new int( id ) );
}

bool WidgetDatabase::isCustomWidget( int id )
{
    if ( id >= dbcustom && id < dbcustomcount )
	return true;
    return false;
}

bool WidgetDatabase::isCustomPluginWidget( int id )
{
    setupDataBase( id );
    WidgetDatabaseRecord *r = at( id );
    if ( !r )
	return false;
    return r->isPlugin;
}

bool WidgetDatabase::isWhatsThisLoaded()
{
    return whatsThisLoaded;
}

void WidgetDatabase::loadWhatsThis( const TQString &docPath )
{
    TQString whatsthisFile = docPath + "/whatsthis";
    TQFile f( whatsthisFile );
    if ( !f.open( IO_ReadOnly ) )
	return;
    TQTextStream ts( &f );
    while ( !ts.atEnd() ) {
	TQString s = ts.readLine();
	TQStringList l = TQStringList::split( " | ", s );
	int id = idFromClassName( l[ 1 ] );
	WidgetDatabaseRecord *r = at( id );
	if ( r )
	    r->whatsThis = l[ 0 ];
    }
    whatsThisLoaded = true;
}


// ### TQt 3.1: make these publically accessible via TQWidgetDatabase API
#if defined(UIC)
bool dbnounload = false;
TQStringList *dbpaths = 0;
#else
extern TQString *qwf_plugin_dir;
#endif


TQPluginManager<WidgetInterface> *widgetManager()
{
    if ( !widgetPluginManager ) {
	TQString pluginDir = "/designer";
#if !defined(UIC)
	if ( qwf_plugin_dir )
	    pluginDir = *qwf_plugin_dir;
#endif
	widgetPluginManager = new TQPluginManager<WidgetInterface>( IID_Widget, TQApplication::libraryPaths(), pluginDir );
	cleanup_manager.add( &widgetPluginManager );
#if defined(UIC)
	if ( dbnounload )
	    widgetPluginManager->setAutoUnload( false );
	if ( dbpaths ) {
	    TQStringList::ConstIterator it = dbpaths->begin();
	    for ( ; it != dbpaths->end(); ++it )
		widgetPluginManager->addLibraryPath( *it );
	}
#endif
    }
    return widgetPluginManager;
}
