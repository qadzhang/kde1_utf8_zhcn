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

#include "actiondnd.h"
#include "command.h"
#ifndef TQT_NO_SQL
#include "database.h"
#endif
#include "formfile.h"
#include "formwindow.h"
#include "mainwindow.h"
#include "menubareditor.h"
#include "metadatabase.h"
#include "pixmapcollection.h"
#include "popupmenueditor.h"
#include "project.h"
#include "resource.h"
#include "widgetfactory.h"

#include <domtool.h>
#include <widgetdatabase.h>

#include <ntqaccel.h>
#include <ntqapplication.h>
#include <ntqbuffer.h>
#include <ntqcombobox.h>
#include <ntqdatetime.h>
#include <ntqdom.h>
#include <ntqfeatures.h>
#include <ntqfile.h>
#include <ntqheader.h>
#include <ntqiconview.h>
#include <ntqlabel.h>
#include <ntqlayout.h>
#include <ntqlistbox.h>
#include <ntqlistview.h>
#include <ntqmenudata.h>
#include <ntqmessagebox.h>
#include <ntqmetaobject.h>
#include <ntqobject.h>
#include <ntqobjectlist.h>
#include <ntqtabbar.h>
#ifndef TQT_NO_TABLE
#include <ntqtable.h>
#include <ntqdatatable.h>
#endif
#include <ntqtabwidget.h>
#include <ntqtabwidget.h>
#include <ntqtextcodec.h>
#include <ntqtextstream.h>
#include <ntqtooltip.h>
#include <ntqwhatsthis.h>
#include <ntqwidget.h>
#include <ntqwidgetstack.h>
#include <ntqwizard.h>
#include <ntqworkspace.h>
#include <ntqworkspace.h>
#include <ntqsplitter.h>
#include <private/qucom_p.h>

static TQString makeIndent( int indent )
{
    TQString s;
    s.fill( ' ', indent * 4 );
    return s;
}

static TQString entitize( const TQString &s, bool attribute = false )
{
    TQString s2 = s;
    s2 = s2.replace( "&", "&amp;" );
    s2 = s2.replace( ">", "&gt;" );
    s2 = s2.replace( "<", "&lt;" );
    if ( attribute ) {
	s2 = s2.replace( "\"", "&quot;" );
	s2 = s2.replace( "'", "&apos;" );
    }
    return s2;
}

#ifdef TQ_WS_MACX
static struct {
    int key;
    const char* name;
} keyname[] = {
    { TQt::Key_Space,        TQT_TRANSLATE_NOOP( "TQAccel", "Space" ) },
    { TQt::Key_Escape,       TQT_TRANSLATE_NOOP( "TQAccel", "Esc" ) },
    { TQt::Key_Tab,          TQT_TRANSLATE_NOOP( "TQAccel", "Tab" ) },
    { TQt::Key_Backtab,      TQT_TRANSLATE_NOOP( "TQAccel", "Backtab" ) },
    { TQt::Key_Backspace,    TQT_TRANSLATE_NOOP( "TQAccel", "Backspace" ) },
    { TQt::Key_Return,       TQT_TRANSLATE_NOOP( "TQAccel", "Return" ) },
    { TQt::Key_Enter,        TQT_TRANSLATE_NOOP( "TQAccel", "Enter" ) },
    { TQt::Key_Insert,       TQT_TRANSLATE_NOOP( "TQAccel", "Ins" ) },
    { TQt::Key_Delete,       TQT_TRANSLATE_NOOP( "TQAccel", "Del" ) },
    { TQt::Key_Pause,        TQT_TRANSLATE_NOOP( "TQAccel", "Pause" ) },
    { TQt::Key_Print,        TQT_TRANSLATE_NOOP( "TQAccel", "Print" ) },
    { TQt::Key_SysReq,       TQT_TRANSLATE_NOOP( "TQAccel", "SysReq" ) },
    { TQt::Key_Home,         TQT_TRANSLATE_NOOP( "TQAccel", "Home" ) },
    { TQt::Key_End,          TQT_TRANSLATE_NOOP( "TQAccel", "End" ) },
    { TQt::Key_Left,         TQT_TRANSLATE_NOOP( "TQAccel", "Left" ) },
    { TQt::Key_Up,           TQT_TRANSLATE_NOOP( "TQAccel", "Up" ) },
    { TQt::Key_Right,        TQT_TRANSLATE_NOOP( "TQAccel", "Right" ) },
    { TQt::Key_Down,         TQT_TRANSLATE_NOOP( "TQAccel", "Down" ) },
    { TQt::Key_Prior,        TQT_TRANSLATE_NOOP( "TQAccel", "PgUp" ) },
    { TQt::Key_Next,         TQT_TRANSLATE_NOOP( "TQAccel", "PgDown" ) },
    { TQt::Key_CapsLock,     TQT_TRANSLATE_NOOP( "TQAccel", "CapsLock" ) },
    { TQt::Key_NumLock,      TQT_TRANSLATE_NOOP( "TQAccel", "NumLock" ) },
    { TQt::Key_ScrollLock,   TQT_TRANSLATE_NOOP( "TQAccel", "ScrollLock" ) },
    { TQt::Key_Menu,         TQT_TRANSLATE_NOOP( "TQAccel", "Menu" ) },
    { TQt::Key_Help,         TQT_TRANSLATE_NOOP( "TQAccel", "Help" ) },

    // Multimedia keys
    { TQt::Key_Back,         TQT_TRANSLATE_NOOP( "TQAccel", "Back" ) },
    { TQt::Key_Forward,      TQT_TRANSLATE_NOOP( "TQAccel", "Forward" ) },
    { TQt::Key_Stop,         TQT_TRANSLATE_NOOP( "TQAccel", "Stop" ) },
    { TQt::Key_Refresh,      TQT_TRANSLATE_NOOP( "TQAccel", "Refresh" ) },
    { TQt::Key_VolumeDown,   TQT_TRANSLATE_NOOP( "TQAccel", "Volume Down" ) },
    { TQt::Key_VolumeMute,   TQT_TRANSLATE_NOOP( "TQAccel", "Volume Mute" ) },
    { TQt::Key_VolumeUp,     TQT_TRANSLATE_NOOP( "TQAccel", "Volume Up" ) },
    { TQt::Key_BassBoost,    TQT_TRANSLATE_NOOP( "TQAccel", "Bass Boost" ) },
    { TQt::Key_BassUp,       TQT_TRANSLATE_NOOP( "TQAccel", "Bass Up" ) },
    { TQt::Key_BassDown,     TQT_TRANSLATE_NOOP( "TQAccel", "Bass Down" ) },
    { TQt::Key_TrebleUp,     TQT_TRANSLATE_NOOP( "TQAccel", "Treble Up" ) },
    { TQt::Key_TrebleDown,   TQT_TRANSLATE_NOOP( "TQAccel", "Treble Down" ) },
    { TQt::Key_MediaPlay,    TQT_TRANSLATE_NOOP( "TQAccel", "Media Play" ) },
    { TQt::Key_MediaStop,    TQT_TRANSLATE_NOOP( "TQAccel", "Media Stop" ) },
    { TQt::Key_MediaPrev,    TQT_TRANSLATE_NOOP( "TQAccel", "Media Previous" ) },
    { TQt::Key_MediaNext,    TQT_TRANSLATE_NOOP( "TQAccel", "Media Next" ) },
    { TQt::Key_MediaRecord,  TQT_TRANSLATE_NOOP( "TQAccel", "Media Record" ) },
    { TQt::Key_HomePage,     TQT_TRANSLATE_NOOP( "TQAccel", "Home" ) },
    { TQt::Key_Favorites,    TQT_TRANSLATE_NOOP( "TQAccel", "Favorites" ) },
    { TQt::Key_Search,       TQT_TRANSLATE_NOOP( "TQAccel", "Search" ) },
    { TQt::Key_Standby,      TQT_TRANSLATE_NOOP( "TQAccel", "Standby" ) },
    { TQt::Key_OpenUrl,      TQT_TRANSLATE_NOOP( "TQAccel", "Open URL" ) },
    { TQt::Key_LaunchMail,   TQT_TRANSLATE_NOOP( "TQAccel", "Launch Mail" ) },
    { TQt::Key_LaunchMedia,  TQT_TRANSLATE_NOOP( "TQAccel", "Launch Media" ) },
    { TQt::Key_Launch0,      TQT_TRANSLATE_NOOP( "TQAccel", "Launch (0)" ) },
    { TQt::Key_Launch1,      TQT_TRANSLATE_NOOP( "TQAccel", "Launch (1)" ) },
    { TQt::Key_Launch2,      TQT_TRANSLATE_NOOP( "TQAccel", "Launch (2)" ) },
    { TQt::Key_Launch3,      TQT_TRANSLATE_NOOP( "TQAccel", "Launch (3)" ) },
    { TQt::Key_Launch4,      TQT_TRANSLATE_NOOP( "TQAccel", "Launch (4)" ) },
    { TQt::Key_Launch5,      TQT_TRANSLATE_NOOP( "TQAccel", "Launch (5)" ) },
    { TQt::Key_Launch6,      TQT_TRANSLATE_NOOP( "TQAccel", "Launch (6)" ) },
    { TQt::Key_Launch7,      TQT_TRANSLATE_NOOP( "TQAccel", "Launch (7)" ) },
    { TQt::Key_Launch8,      TQT_TRANSLATE_NOOP( "TQAccel", "Launch (8)" ) },
    { TQt::Key_Launch9,      TQT_TRANSLATE_NOOP( "TQAccel", "Launch (9)" ) },
    { TQt::Key_LaunchA,      TQT_TRANSLATE_NOOP( "TQAccel", "Launch (A)" ) },
    { TQt::Key_LaunchB,      TQT_TRANSLATE_NOOP( "TQAccel", "Launch (B)" ) },
    { TQt::Key_LaunchC,      TQT_TRANSLATE_NOOP( "TQAccel", "Launch (C)" ) },
    { TQt::Key_LaunchD,      TQT_TRANSLATE_NOOP( "TQAccel", "Launch (D)" ) },
    { TQt::Key_LaunchE,      TQT_TRANSLATE_NOOP( "TQAccel", "Launch (E)" ) },
    { TQt::Key_LaunchF,      TQT_TRANSLATE_NOOP( "TQAccel", "Launch (F)" ) },
    { TQt::Key_MonBrightnessUp,        TQT_TRANSLATE_NOOP( "TQAccel", "Monitor Brightness Up" ) },
    { TQt::Key_MonBrightnessDown,      TQT_TRANSLATE_NOOP( "TQAccel", "Monitor Brightness Down" ) },
    { TQt::Key_KeyboardLightOnOff,     TQT_TRANSLATE_NOOP( "TQAccel", "Keyboard Light On Off" ) },
    { TQt::Key_KeyboardBrightnessUp,   TQT_TRANSLATE_NOOP( "TQAccel", "Keyboard Brightness Up" ) },
    { TQt::Key_KeyboardBrightnessDown, TQT_TRANSLATE_NOOP( "TQAccel", "Keyboard Brightness Down" ) },

    // --------------------------------------------------------------
    // More consistent namings
    { TQt::Key_Print,        TQT_TRANSLATE_NOOP( "TQAccel", "Print Screen" ) },
    { TQt::Key_Prior,        TQT_TRANSLATE_NOOP( "TQAccel", "Page Up" ) },
    { TQt::Key_Next,         TQT_TRANSLATE_NOOP( "TQAccel", "Page Down" ) },
    { TQt::Key_CapsLock,     TQT_TRANSLATE_NOOP( "TQAccel", "Caps Lock" ) },
    { TQt::Key_NumLock,      TQT_TRANSLATE_NOOP( "TQAccel", "Num Lock" ) },
    { TQt::Key_NumLock,      TQT_TRANSLATE_NOOP( "TQAccel", "Number Lock" ) },
    { TQt::Key_ScrollLock,   TQT_TRANSLATE_NOOP( "TQAccel", "Scroll Lock" ) },
    { TQt::Key_Insert,       TQT_TRANSLATE_NOOP( "TQAccel", "Insert" ) },
    { TQt::Key_Delete,       TQT_TRANSLATE_NOOP( "TQAccel", "Delete" ) },
    { TQt::Key_Escape,       TQT_TRANSLATE_NOOP( "TQAccel", "Escape" ) },
    { TQt::Key_SysReq,       TQT_TRANSLATE_NOOP( "TQAccel", "System Request" ) },

    { 0, 0 }
};
#endif
static TQString platformNeutralKeySequence(const TQKeySequence &ks)
{
#ifndef TQ_WS_MACX
    return TQString(ks);
#else
    uint k;
    TQString str;
    TQString p;
    for (k = 0; k < ks.count(); ++k) {
	int keycombo = ks[k];
	int basekey = keycombo & ~(TQt::SHIFT | TQt::CTRL | TQt::ALT | TQt::META);
	if (keycombo & TQt::CTRL)
	    str += "Ctrl+";
	if (keycombo & TQt::ALT)
	    str += "Alt+";
	if (keycombo & TQt::META)
	    str += "Meta+";
	if (keycombo & TQt::SHIFT)
	    str += "Shift+";

	// begin copy and paste from TQKeySequence :(
	if (basekey & TQt::UNICODE_ACCEL) {
	    // Note: This character should NOT be upper()'ed, since
	    // the encoded string should indicate EXACTLY what the
	    // key represents! Hence a 'Ctrl+Shift+c' is posible to
	    // represent, but is clearly impossible to trigger...
	    p = TQChar(basekey & 0xffff);
	} else if ( basekey >= TQt::Key_F1 && basekey <= TQt::Key_F35 ) {
	    p = TQAccel::tr( "F%1" ).arg(basekey - TQt::Key_F1 + 1);
	} else if ( basekey > TQt::Key_Space && basekey <= TQt::Key_AsciiTilde ) {
	    p.sprintf( "%c", basekey );
	} else {
	    int i = 0;
	    while (keyname[i].name) {
		if (basekey == keyname[i].key) {
		    p = TQAccel::tr(keyname[i].name);
		    break;
		}
		++i;
	    }
	    // If we can't find the actual translatable keyname,
	    // fall back on the unicode representation of it...
	    // Or else characters like Key_aring may not get displayed
	    // ( Really depends on you locale )
	    if ( !keyname[i].name )
		// Note: This character should NOT be upper()'ed, see above!
		p = TQChar(basekey & 0xffff);
	}
	// end copy...
	str += p + ", ";
    }
    str.truncate(str.length() - 2);
    return str;
#endif
}

static TQString mkBool( bool b )
{
    return b? "true" : "false";
}

/*!
  \class Resource resource.h
  \brief Class for saving/loading, etc. forms

  This class is used for saving and loading forms, code generation,
  transferring data of widgets over the clipboard, etc..

*/


Resource::Resource()
{
    mainwindow = 0;
    formwindow = 0;
    toplevel = 0;
    copying = false;
    pasting = false;
    hadGeometry = false;
    langIface = 0;
    hasFunctions = false;
}

Resource::Resource( MainWindow* mw )
    : mainwindow( mw )
{
    formwindow = 0;
    toplevel = 0;
    copying = false;
    pasting = false;
    hadGeometry = false;
    langIface = 0;
    hasFunctions = false;
}

Resource::~Resource()
{
    if ( langIface )
	langIface->release();
}

void Resource::setWidget( FormWindow *w )
{
    formwindow = w;
    toplevel = w;
}

TQWidget *Resource::widget() const
{
    return toplevel;
}

bool Resource::load( FormFile *ff, Project *defProject )
{
    if ( !ff || ff->absFileName().isEmpty() )
	return false;
    currFileName = ff->absFileName();
    mainContainerSet = false;

    TQFile f( ff->absFileName() );
    f.open( IO_ReadOnly | IO_Translate );

    bool b = load( ff, &f, defProject );
    f.close();

    return b;
}

#undef signals
#undef slots

bool Resource::load( FormFile *ff, TQIODevice* dev, Project *defProject )
{
    TQDomDocument doc;
    TQString errMsg;
    int errLine;
    if ( !doc.setContent( dev, &errMsg, &errLine ) ) {
	return false;
    }

    DomTool::fixDocument( doc );

    TQWidget *p = mainwindow ? mainwindow->qWorkspace() : 0;
    toplevel = formwindow = new FormWindow( ff, p, 0 );
    if ( defProject )
	formwindow->setProject( defProject );
    else if ( MainWindow::self )
	formwindow->setProject( MainWindow::self->currProject() );
    if ( mainwindow )
	formwindow->setMainWindow( mainwindow );
    MetaDataBase::addEntry( formwindow );

    if ( !langIface ) {
	TQString lang = "TQt Script";
	if ( mainwindow )
	    lang = mainwindow->currProject()->language();
	langIface = MetaDataBase::languageInterface( lang );
	if ( langIface )
	    langIface->addRef();
    }

    uiFileVersion = doc.firstChild().toElement().attribute("version");
    TQDomElement e = doc.firstChild().toElement().firstChild().toElement();

    TQDomElement forwards = e;
    while ( forwards.tagName() != "forwards" && !forwards.isNull() )
	forwards = forwards.nextSibling().toElement();

    TQDomElement includes = e;
    while ( includes.tagName() != "includes" && !includes.isNull() )
	includes = includes.nextSibling().toElement();

    TQDomElement variables = e;
    while ( variables.tagName() != "variables" && !variables.isNull() )
	variables = variables.nextSibling().toElement();

    TQDomElement signals = e;
    while ( signals.tagName() != "signals" && !signals.isNull() )
	signals = signals.nextSibling().toElement();

    TQDomElement slots = e;
    while ( slots.tagName() != "slots" && !slots.isNull() )
	slots = slots.nextSibling().toElement();

    TQDomElement functions = e;
    while ( functions.tagName() != "functions" && !functions.isNull() )
	functions = functions.nextSibling().toElement();

    TQDomElement connections = e;
    while ( connections.tagName() != "connections" && !connections.isNull() )
	connections = connections.nextSibling().toElement();

    TQDomElement imageCollection = e;
    images.clear();
    while ( imageCollection.tagName() != "images" && !imageCollection.isNull() )
	imageCollection = imageCollection.nextSibling().toElement();

    TQDomElement customWidgets = e;
    while ( customWidgets.tagName() != "customwidgets" && !customWidgets.isNull() )
	customWidgets = customWidgets.nextSibling().toElement();

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

    TQDomElement widget;
    while ( !e.isNull() ) {
	if ( e.tagName() == "widget" ) {
	    widgets.clear();
	    widget = e;
	} else if ( e.tagName() == "include" ) { // compatibility with 2.x
	    MetaDataBase::Include inc;
	    inc.location = "global";
	    if ( e.attribute( "location" ) == "local" )
		inc.location = "local";
	    inc.implDecl = "in declaration";
	    if ( e.attribute( "impldecl" ) == "in implementation" )
		inc.implDecl = "in implementation";
	    inc.header = e.firstChild().toText().data();
	    if ( inc.header.right( 5 ) != ".ui.h" ) {
		bool found = false;
		TQValueList<MetaDataBase::Include>::Iterator it;
		for ( it = metaIncludes.begin(); it != metaIncludes.end(); ++it ) {
		    MetaDataBase::Include currInc = *it;
		    if ( currInc.location == inc.location && currInc.implDecl == inc.implDecl &&
			 currInc.header == inc.header) {
			found = true;
			break;
		    }
		}
		if ( !found )
		{
		    metaIncludes.append( inc );
		}
	    } else {
		if ( formwindow->formFile() )
		    formwindow->formFile()->setCodeFileState( FormFile::Ok );
	    }
	} else if ( e.tagName() == "comment" ) {
	    metaInfo.comment = e.firstChild().toText().data();
	} else if ( e.tagName() == "forward" ) { // compatibility with old betas
	    metaForwards << e.firstChild().toText().data();
	} else if ( e.tagName() == "variable" ) { // compatibility with old betas
	    MetaDataBase::Variable v;
	    v.varName = e.firstChild().toText().data();
	    v.varAccess = "protected";
	    metaVariables << v;
	} else if ( e.tagName() == "author" ) {
	    metaInfo.author = e.firstChild().toText().data();
	} else if ( e.tagName() == "class" ) {
	    metaInfo.className = e.firstChild().toText().data();
	} else if ( e.tagName() == "pixmapfunction" ) {
	    if ( formwindow ) {
		formwindow->setSavePixmapInline( false );
		formwindow->setSavePixmapInProject( false );
		formwindow->setPixmapLoaderFunction( e.firstChild().toText().data() );
	    }
	} else if ( e.tagName() == "pixmapinproject" ) {
	    if ( formwindow ) {
		formwindow->setSavePixmapInline( false );
		formwindow->setSavePixmapInProject( true );
	    }
	} else if ( e.tagName() == "exportmacro" ) {
	    exportMacro = e.firstChild().toText().data();
	} else if ( e.tagName() == "layoutdefaults" ) {
	    formwindow->setLayoutDefaultSpacing( e.attribute( "spacing", TQString::number( formwindow->layoutDefaultSpacing() ) ).toInt() );
	    formwindow->setLayoutDefaultMargin( e.attribute( "margin", TQString::number( formwindow->layoutDefaultMargin() ) ).toInt() );
	} else if ( e.tagName() == "layoutfunctions" ) {
	    formwindow->setSpacingFunction( e.attribute( "spacing" ) );
	    formwindow->setMarginFunction( e.attribute( "margin" ) );
	    if ( !formwindow->marginFunction().isEmpty() || !formwindow->spacingFunction().isEmpty() )
		formwindow->hasLayoutFunctions( true );
	}

	e = e.nextSibling().toElement();
    }

    if ( !imageCollection.isNull() )
	loadImageCollection( imageCollection );
    if ( !customWidgets.isNull() )
	loadCustomWidgets( customWidgets, this );

    if ( !createObject( widget, formwindow) )
	return false;

    if ( !forwards.isNull() ) {
	for ( TQDomElement n = forwards.firstChild().toElement(); !n.isNull(); n = n.nextSibling().toElement() )
	    if ( n.tagName() == "forward" )
		metaForwards << n.firstChild().toText().data();
    }

    if ( !includes.isNull() ) {
	for ( TQDomElement n = includes.firstChild().toElement(); !n.isNull(); n = n.nextSibling().toElement() )
	    if ( n.tagName() == "include" ) {
		if ( n.tagName() == "include" ) {
		    MetaDataBase::Include inc;
		    inc.location = "global";
		    if ( n.attribute( "location" ) == "local" )
			inc.location = "local";
		    inc.implDecl = "in declaration";
		    if ( n.attribute( "impldecl" ) == "in implementation" )
			inc.implDecl = "in implementation";
		    inc.header = n.firstChild().toText().data();
		    if ( inc.header.right( 5 ) != ".ui.h" ) {
			bool found = false;
			TQValueList<MetaDataBase::Include>::Iterator it;
			for ( it = metaIncludes.begin(); it != metaIncludes.end(); ++it ) {
			    MetaDataBase::Include currInc = *it;
			    if ( currInc.location == inc.location && currInc.implDecl == inc.implDecl &&
				 currInc.header == inc.header) {
				found = true;
				break;
			    }
			}
			if ( !found )
			{
			    metaIncludes.append( inc );
			}
		    } else {
			if ( formwindow->formFile() )
			    formwindow->formFile()->setCodeFileState( FormFile::Ok );
		    }
		}
	    }
    }

    if ( !variables.isNull() ) {
	for ( TQDomElement n = variables.firstChild().toElement(); !n.isNull();
	      n = n.nextSibling().toElement() ) {
	    if ( n.tagName() == "variable" ) {
		MetaDataBase::Variable v;
		v.varName = n.firstChild().toText().data();
		v.varAccess = n.attribute( "access", "protected" );
		if ( v.varAccess.isEmpty() )
		    v.varAccess = "protected";
		metaVariables << v;
	    }
	}
    }
    if ( !signals.isNull() ) {
	for ( TQDomElement n = signals.firstChild().toElement(); !n.isNull(); n = n.nextSibling().toElement() )
	    if ( n.tagName() == "signal" )
		metaSignals << n.firstChild().toText().data();
    }
    if ( !slots.isNull() ) {
	for ( TQDomElement n = slots.firstChild().toElement(); !n.isNull(); n = n.nextSibling().toElement() )
	    if ( n.tagName() == "slot" ) {
		MetaDataBase::Function function;
		function.specifier = n.attribute( "specifier", "virtual" );
		if ( function.specifier.isEmpty() )
		    function.specifier = "virtual";
		function.access = n.attribute( "access", "public" );
		if ( function.access.isEmpty() )
		    function.access = "public";
		function.language = n.attribute( "language", "C++" );
		function.returnType = n.attribute( "returnType", "void" );
		if ( function.returnType.isEmpty() )
		    function.returnType = "void";
		function.type = "slot";
		function.function = n.firstChild().toText().data();
		if ( !MetaDataBase::hasFunction( formwindow, function.function, true ) )
		    MetaDataBase::addFunction( formwindow, function.function, function.specifier,
					       function.access, "slot", function.language, function.returnType );
		else
		    MetaDataBase::changeFunctionAttributes( formwindow, function.function, function.function,
							    function.specifier, function.access,
							    "slot", function.language,
							    function.returnType );
	    }
    }

    if ( !functions.isNull() ) {
	for ( TQDomElement n = functions.firstChild().toElement(); !n.isNull(); n = n.nextSibling().toElement() ) {
	    if ( n.tagName() == "function" ) {
		MetaDataBase::Function function;
		function.specifier = n.attribute( "specifier", "virtual" );
		if ( function.specifier.isEmpty() )
		    function.specifier = "virtual";
		function.access = n.attribute( "access", "public" );
		if ( function.access.isEmpty() )
		    function.access = "public";
		function.type = n.attribute( "type", "function" );
		function.type = "function";
		function.language = n.attribute( "language", "C++" );
		function.returnType = n.attribute( "returnType", "void" );
		if ( function.returnType.isEmpty() )
		    function.returnType = "void";
		function.function = n.firstChild().toText().data();
		if ( !MetaDataBase::hasFunction( formwindow, function.function, true ) )
		    MetaDataBase::addFunction( formwindow, function.function, function.specifier,
					       function.access, function.type, function.language,
					       function.returnType );
		else
		    MetaDataBase::changeFunctionAttributes( formwindow, function.function, function.function,
							    function.specifier, function.access,
							    function.type, function.language, function.returnType );
	    }
	}
    }

    if ( !actions.isNull() )
	loadActions( actions );
    if ( !toolbars.isNull() )
	loadToolBars( toolbars );
    if ( !menubar.isNull() )
	loadMenuBar( menubar );

    if ( !connections.isNull() )
	loadConnections( connections );

    if ( !tabOrder.isNull() )
	loadTabOrder( tabOrder );

    if ( formwindow ) {
	MetaDataBase::setIncludes( formwindow, metaIncludes );
	MetaDataBase::setForwards( formwindow, metaForwards );
	MetaDataBase::setVariables( formwindow, metaVariables );
	MetaDataBase::setSignalList( formwindow, metaSignals );
	metaInfo.classNameChanged = metaInfo.className != TQString( formwindow->name() );
	MetaDataBase::setMetaInfo( formwindow, metaInfo );
	MetaDataBase::setExportMacro( formwindow->mainContainer(), exportMacro );
    }

    loadExtraSource( formwindow->formFile(), currFileName, langIface, hasFunctions );

    if ( mainwindow && formwindow )
	mainwindow->insertFormWindow( formwindow );

    if ( formwindow ) {
	formwindow->killAccels( formwindow );
	if ( formwindow->layout() )
	    formwindow->layout()->activate();
	if ( hadGeometry )
	    formwindow->resize( formwindow->size().expandedTo( formwindow->minimumSize().
							       expandedTo( formwindow->minimumSizeHint() ) ) );
	else
	    formwindow->resize( formwindow->size().expandedTo( formwindow->sizeHint() ) );
    }

    return true;
}

static bool saveCode( const TQString &filename, const TQString &code )
{
    TQFile f( filename );
    if ( f.open(IO_WriteOnly | IO_Translate) ) {
	TQTextStream ts( &f );
	ts << code;
	return true;
    }
    return false;
}

bool Resource::save( const TQString& filename, bool formCodeOnly )
{
    if ( !formwindow || filename.isEmpty() )
	return false;
    if (!langIface) {
	TQString lang = "TQt Script";
	if ( mainwindow )
	    lang = mainwindow->currProject()->language();
	langIface = MetaDataBase::languageInterface( lang );
	if ( langIface )
	    langIface->addRef();
    }
    if ( formCodeOnly && langIface ) {
	if ( saveFormCode(formwindow->formFile(), langIface) )
	    return true;
	bool breakout = false;
	FormFile *ff = formwindow->formFile();
	TQString codeFile = ff->project()->makeAbsolute( ff->codeFile() );
	TQString filter = langIface->fileFilterList().join(";;");
	while ( !breakout ) {
	    TQString fn = TQFileDialog::getSaveFileName( codeFile, filter );
	    breakout = fn.isEmpty();
	    if ( !breakout ) {
		if ( saveCode(fn, ff->code()) )
		    return true;
	    }
	}
    }
    currFileName = filename;

    TQFile f( filename );
    if ( !f.open( IO_WriteOnly | IO_Translate ) )
	return false;
    bool b = save( &f );
    f.close();
    return b;
}

bool Resource::save( TQIODevice* dev )
{
    if ( !formwindow )
	return false;

    if ( !langIface ) {
	TQString lang = "C++";
	if ( mainwindow )
	    lang = mainwindow->currProject()->language();
	langIface = MetaDataBase::languageInterface( lang );
	if ( langIface )
	    langIface->addRef();
    }

    TQTextStream ts( dev );
    ts.setCodec( TQTextCodec::codecForName( "UTF-8" ) );

    ts << "<!DOCTYPE UI><UI version=\"3.3\" stdsetdef=\"1\">" << endl;
    saveMetaInfoBefore( ts, 0 );
    saveObject( formwindow->mainContainer(), 0, ts, 0 );
    if ( ::tqt_cast<TQMainWindow*>(formwindow->mainContainer()) ) {
	saveMenuBar( (TQMainWindow*)formwindow->mainContainer(), ts, 0 );
	saveToolBars( (TQMainWindow*)formwindow->mainContainer(), ts, 0 );
    }
    if ( !MetaDataBase::customWidgets()->isEmpty() && !usedCustomWidgets.isEmpty() )
	saveCustomWidgets( ts, 0 );
    if ( ::tqt_cast<TQMainWindow*>(formwindow->mainContainer()) )
	saveActions( formwindow->actionList(), ts, 0 );
    if ( !images.isEmpty() )
	saveImageCollection( ts, 0 );
    if ( !MetaDataBase::connections( formwindow ).isEmpty() ||
	 !MetaDataBase::slotList( formwindow ).isEmpty() )
	saveConnections( ts, 0 );
    saveTabOrder( ts, 0 );
    saveMetaInfoAfter( ts, 0 );
    ts << "</UI>" << endl;
    bool ok = saveFormCode( formwindow->formFile(), langIface );
    images.clear();

    return ok;
}

TQString Resource::copy()
{
    if ( !formwindow )
	return TQString::null;

    copying = true;
    TQString s;
    TQTextOStream ts( &s );

    ts << "<!DOCTYPE UI-SELECTION><UI-SELECTION>" << endl;
    TQWidgetList widgets = formwindow->selectedWidgets();
    TQWidgetList tmp( widgets );
    for ( TQWidget *w = widgets.first(); w; w = widgets.next() ) {
	TQWidget *p = w->parentWidget();
	bool save = true;
	while ( p ) {
	    if ( tmp.findRef( p ) != -1 ) {
		save = false;
		break;
	    }
	    p = p->parentWidget();
	}
	if ( save )
	    saveObject( w, 0, ts, 0 );
    }
    if ( !MetaDataBase::customWidgets()->isEmpty() && !usedCustomWidgets.isEmpty() )
	saveCustomWidgets( ts, 0 );
    if ( !images.isEmpty() )
	saveImageCollection( ts, 0 );
    ts << "</UI-SELECTION>" << endl;

    return s;
}


void Resource::paste( const TQString &cb, TQWidget *parent )
{
    if ( !formwindow )
	return;
    mainContainerSet = true;

    pasting = true;
    TQDomDocument doc;
    TQString errMsg;
    int errLine;
    doc.setContent( cb, &errMsg, &errLine );

    TQDomElement firstWidget = doc.firstChild().toElement().firstChild().toElement();

    TQDomElement imageCollection = firstWidget;
    images.clear();
    while ( imageCollection.tagName() != "images" && !imageCollection.isNull() )
	imageCollection = imageCollection.nextSibling().toElement();

    TQDomElement customWidgets = firstWidget;
    while ( customWidgets.tagName() != "customwidgets" && !customWidgets.isNull() )
	customWidgets = customWidgets.nextSibling().toElement();

    if ( !imageCollection.isNull() )
	loadImageCollection( imageCollection );
    if ( !customWidgets.isNull() )
	loadCustomWidgets( customWidgets, this );

    TQWidgetList widgets;
    formwindow->clearSelection( false );
    formwindow->setPropertyShowingBlocked( true );
    formwindow->clearSelection( false );
    while ( !firstWidget.isNull() ) {
	if ( firstWidget.tagName() == "widget" ) {
	    TQWidget *w = (TQWidget*)createObject( firstWidget, parent, 0 );
	    if ( !w )
		continue;
	    widgets.append( w );
	    int x = w->x() + formwindow->grid().x();
	    int y = w->y() + formwindow->grid().y();
	    if ( w->x() + w->width() > parent->width() )
		x = TQMAX( 0, parent->width() - w->width() );
	    if ( w->y() + w->height() > parent->height() )
		y = TQMAX( 0, parent->height() - w->height() );
	    if ( x != w->x() || y != w->y() )
		w->move( x, y );
	    formwindow->selectWidget( w );
	} else if ( firstWidget.tagName() == "spacer" ) {
	    TQWidget *w = createSpacer( firstWidget, parent, 0, firstWidget.tagName() == "vspacer" ? TQt::Vertical : TQt::Horizontal );
	    if ( !w )
		continue;
	    widgets.append( w );
	    int x = w->x() + formwindow->grid().x();
	    int y = w->y() + formwindow->grid().y();
	    if ( w->x() + w->width() > parent->width() )
		x = TQMAX( 0, parent->width() - w->width() );
	    if ( w->y() + w->height() > parent->height() )
		y = TQMAX( 0, parent->height() - w->height() );
	    if ( x != w->x() || y != w->y() )
		w->move( x, y );
	    formwindow->selectWidget( w );
	}
	firstWidget = firstWidget.nextSibling().toElement();
    }
    formwindow->setPropertyShowingBlocked( false );
    formwindow->emitShowProperties();

    PasteCommand *cmd = new PasteCommand( FormWindow::tr( "Paste" ), formwindow, widgets );
    formwindow->commandHistory()->addCommand( cmd );
}

void Resource::saveObject( TQObject *obj, TQDesignerGridLayout* grid, TQTextStream &ts, int indent )
{
    if ( obj && obj->isWidgetType() && ( (TQWidget*)obj )->isHidden() )
	return;
    TQString closeTag;
    const char* className = WidgetFactory::classNameOf( obj );
    int classID = WidgetDatabase::idFromClassName( className );
    bool isPlugin = WidgetDatabase::isCustomPluginWidget( classID );
    if ( obj->isWidgetType() ) {
	if ( obj->isA("CustomWidget") || isPlugin ) {
	    usedCustomWidgets << TQString( className );
	    MetaDataBase::Include inc;
	    inc.location = "global";
	    inc.implDecl = "in implementation";
	    inc.header = WidgetDatabase::includeFile( classID );
	    bool found = false;
	    TQValueList<MetaDataBase::Include> includes = MetaDataBase::includes( formwindow );
	    TQValueList<MetaDataBase::Include>::Iterator it;
	    for ( it = includes.begin(); it != includes.end(); ++it ) {
		MetaDataBase::Include currInc = *it;
		if ( currInc.location == inc.location && currInc.implDecl == inc.implDecl &&
		     currInc.header == inc.header) {
		    found = true;
		    break;
		}
	    }
	    if ( !found )
	    {
		includes << inc;
	    }
	    MetaDataBase::setIncludes( formwindow, includes );
	}

	if ( obj != formwindow && !formwindow->widgets()->find( (TQWidget*)obj ) )
	    return; // we don't know anything about this thing

	TQString attributes;
	if ( grid ) {
	    TQDesignerGridLayout::Item item = grid->items[ (TQWidget*)obj ];
	    attributes += TQString(" row=\"") + TQString::number(item.row) + "\"";
	    attributes += TQString(" column=\"") + TQString::number(item.column) + "\"";
	    if ( item.rowspan * item.colspan != 1 ) {
		attributes += TQString(" rowspan=\"") + TQString::number(item.rowspan) + "\"";
		attributes += TQString(" colspan=\"") + TQString::number(item.colspan) + "\"";
	    }
	}

	if ( qstrcmp( className, "Spacer" ) == 0 ) {
	    closeTag = makeIndent( indent ) + "</spacer>\n";
	    ts << makeIndent( indent ) << "<spacer" << attributes << ">" << endl;
	    ++indent;
	} else {
	    closeTag = makeIndent( indent ) + "</widget>\n";
	    ts << makeIndent( indent ) << "<widget class=\"" << className << "\"" << attributes << ">" << endl;
	    ++indent;
	}
	if ( WidgetFactory::hasItems(classID, obj) )
	    saveItems( obj, ts, indent );
	saveObjectProperties( obj, ts, indent );
    } else {
	// test for other objects we created. Nothing so far.
	return;
    }

    TQDesignerWidgetStack* ws = 0;

    if ( ::tqt_cast<TQTabWidget*>(obj) ) {
	TQTabWidget* tw = (TQTabWidget*) obj;
	TQObjectList* tmpl = tw->queryList( "TQWidgetStack" );
	TQWidgetStack *ws = (TQWidgetStack*)tmpl->first();
	TQTabBar *tb = ( (TQDesignerTabWidget*)obj )->tabBar();
	for ( int i = 0; i < tb->count(); ++i ) {
	    TQTab *t = tb->tabAt( i );
	    if ( !t )
		continue;
	    TQWidget *w = ws->widget( t->identifier() );
	    if ( !w )
		continue;
	    if ( WidgetDatabase::idFromClassName( WidgetFactory::classNameOf(w) ) == -1 )
		continue; // we don't know this widget
	    ts << makeIndent( indent ) << "<widget class=\"TQWidget\">" << endl;
	    ++indent;
	    ts << makeIndent( indent ) << "<property name=\"name\">" << endl;
	    indent++;
	    ts << makeIndent( indent ) << "<cstring>" << entitize( w->name() ) << "</cstring>" << endl;
	    indent--;
	    ts << makeIndent( indent ) << "</property>" << endl;

	    ts << makeIndent( indent ) << "<attribute name=\"title\">" << endl;
	    indent++;
	    ts << makeIndent( indent ) << "<string>" << entitize( t->text() ) << "</string>" << endl;
	    indent--;
	    ts << makeIndent( indent ) << "</attribute>" << endl;
	    saveChildrenOf( w, ts, indent );
	    --indent;
	    ts << makeIndent( indent ) << "</widget>" << endl;
	}
	delete tmpl;
    } else if ( (ws = ::tqt_cast<TQDesignerWidgetStack*>(obj)) != 0 ) {
	for ( int i = 0; i < ws->count(); ++i ) {
	    TQWidget *w = ws->page( i );
	    if ( !w )
		continue;
	    if ( WidgetDatabase::idFromClassName(WidgetFactory::classNameOf(w)) == -1 )
		continue; // we don't know this widget
	    ts << makeIndent( indent ) << "<widget class=\"TQWidget\">" << endl;
	    ++indent;
	    ts << makeIndent( indent ) << "<property name=\"name\">" << endl;
	    indent++;
	    ts << makeIndent( indent ) << "<cstring>" << entitize( w->name() ) << "</cstring>" << endl;
	    indent--;
	    ts << makeIndent( indent ) << "</property>" << endl;
	    ts << makeIndent( indent ) << "<attribute name=\"id\">" << endl;
	    indent++;
	    ts << makeIndent( indent ) << "<number>" << TQString::number(i) << "</number>" << endl;
	    indent--;
	    ts << makeIndent( indent ) << "</attribute>" << endl;
	    saveChildrenOf( w, ts, indent );
	    --indent;
	    ts << makeIndent( indent ) << "</widget>" << endl;
	}
    } else if ( ::tqt_cast<TQToolBox*>(obj) ) {
	TQToolBox* tb = (TQToolBox*)obj;
	for ( int i = 0; i < tb->count(); ++i ) {
	    TQWidget *w = tb->item( i );
	    if ( !w )
		continue;
	    if ( WidgetDatabase::idFromClassName(WidgetFactory::classNameOf(w)) == -1 )
		continue; // we don't know this widget
	    ts << makeIndent( indent ) << "<widget class=\"TQWidget\">" << endl;
	    ++indent;
	    ts << makeIndent( indent ) << "<property name=\"name\">" << endl;
	    indent++;
	    ts << makeIndent( indent ) << "<cstring>" << entitize( w->name() ) << "</cstring>" << endl;
	    indent--;
	    ts << makeIndent( indent ) << "</property>" << endl;
	    ts << makeIndent( indent ) << "<property name=\"backgroundMode\">" << endl;
	    indent++;
	    saveEnumProperty( w, "backgroundMode", TQVariant::Invalid, ts, indent );
	    indent--;
	    ts << makeIndent( indent ) << "</property>" << endl;
	    ts << makeIndent( indent ) << "<attribute name=\"label\">" << endl;
	    indent++;
	    ts << makeIndent( indent ) << "<string>" << entitize( tb->itemLabel( tb->indexOf(w) ) ) << "</string>" << endl;
	    indent--;
	    ts << makeIndent( indent ) << "</attribute>" << endl;
	    saveChildrenOf( w, ts, indent );
	    --indent;
	    ts << makeIndent( indent ) << "</widget>" << endl;
	}
    } else if ( ::tqt_cast<TQWizard*>(obj) ) {
	TQWizard* wiz = (TQWizard*)obj;
	for ( int i = 0; i < wiz->pageCount(); ++i ) {
	    TQWidget *w = wiz->page( i );
	    if ( !w )
		continue;
	    if ( WidgetDatabase::idFromClassName(WidgetFactory::classNameOf(w)) == -1 )
		continue; // we don't know this widget
	    ts << makeIndent( indent ) << "<widget class=\"TQWidget\">" << endl;
	    ++indent;
	    ts << makeIndent( indent ) << "<property name=\"name\">" << endl;
	    indent++;
	    ts << makeIndent( indent ) << "<cstring>" << entitize( w->name() ) << "</cstring>" << endl;
	    indent--;
	    ts << makeIndent( indent ) << "</property>" << endl;

	    ts << makeIndent( indent ) << "<attribute name=\"title\">" << endl;
	    indent++;
	    ts << makeIndent( indent ) << "<string>" << entitize( wiz->title( w ) ) << "</string>" << endl;
	    indent--;
	    ts << makeIndent( indent ) << "</attribute>" << endl;
	    saveChildrenOf( w, ts, indent );
	    --indent;
	    ts << makeIndent( indent ) << "</widget>" << endl;
	}
    } else if ( ::tqt_cast<TQMainWindow*>(obj) ) {
	saveChildrenOf( ( (TQMainWindow*)obj )->centralWidget(), ts, indent );
    } else {
	bool saved = false;
#ifdef QT_CONTAINER_CUSTOM_WIDGETS
	if ( isPlugin ) {
	    WidgetInterface *iface = 0;
	    widgetManager()->queryInterface( className, &iface );
	    if ( iface ) {
		TQWidgetContainerInterfacePrivate *iface2 = 0;
		iface->queryInterface( IID_QWidgetContainer, (TQUnknownInterface**)&iface2 );
		if ( iface2 ) {
		    if ( iface2->supportsPages( className ) )  {
			TQWidgetList containers = iface2->pages( className, (TQWidget*)obj );
			if ( !containers.isEmpty() ) {
			    saved = true;
			    int i = 0;
			    for ( TQWidget *w = containers.first(); w; w = containers.next(), ++i ) {
				if ( WidgetDatabase::
				     idFromClassName( WidgetFactory::classNameOf( w ) ) == -1 )
				    continue; // we don't know this widget
				ts << makeIndent( indent ) << "<widget class=\""
				   << WidgetFactory::classNameOf( w )
				   << "\">" << endl;
				++indent;
				ts << makeIndent( indent ) << "<property name=\"name\">" << endl;
				indent++;
				ts << makeIndent( indent ) << "<cstring>" << entitize( w->name() )
				   << "</cstring>" << endl;
				indent--;
				ts << makeIndent( indent ) << "</property>" << endl;
				ts << makeIndent( indent ) << "<attribute name=\"label\">" << endl;
				indent++;
				ts << makeIndent( indent ) << "<cstring>"
				   << entitize( iface2->pageLabel( className, (TQWidget*)obj, i ) )
				   << "</cstring>" << endl;
				indent--;
				ts << makeIndent( indent ) << "</attribute>" << endl;
				saveChildrenOf( w, ts, indent );
				--indent;
				ts << makeIndent( indent ) << "</widget>" << endl;
			    }
			}
		    } else {
			saved = true;
			TQWidget *w = iface2->containerOfWidget( className, (TQWidget*)obj );
			if ( obj != w ) {
			    ts << makeIndent( indent ) << "<widget class=\""
			       << WidgetFactory::classNameOf( w )
			       << "\">" << endl;
			    ++indent;
			    ts << makeIndent( indent ) << "<property name=\"name\">" << endl;
			    indent++;
			    ts << makeIndent( indent ) << "<cstring>" << entitize( w->name() )
			       << "</cstring>" << endl;
			    indent--;
			    ts << makeIndent( indent ) << "</property>" << endl;
			    saveChildrenOf( w, ts, indent );
			    --indent;
			    ts << makeIndent( indent ) << "</widget>" << endl;
			}
			// Create a custom widget and then store it in the database
			// so we can save the custom widgets.
			MetaDataBase::CustomWidget *cw = new MetaDataBase::CustomWidget;
			cw->className = className;
			cw->includeFile =  WidgetDatabase::includeFile( classID );
			TQStrList lst = w->metaObject()->signalNames( true );
			for ( TQPtrListIterator<char> it(lst); it.current(); ++it )
			    cw->lstSignals.append(it.current());

			int i;
			int total = w->metaObject()->numProperties( true );
			for ( i = 0; i < total; i++ ) {
			    const TQMetaProperty *p = w->metaObject()->property( i, true );
			    if ( p->designable(w) ) {
				MetaDataBase::Property prop;
				prop.property = p->name();
				TQString pType = p->type();
				// *sigh* designer types are not normal types
				// Handle most cases, the ones it misses are
				// probably too difficult to deal with anyway...
				if ( pType.startsWith("Q") ) {
				    pType = pType.right( pType.length() - 1 );
				} else {
				    pType[0] = pType[0].upper();
				}
				prop.type = pType;
				cw->lstProperties.append( prop );
			    }
			}

			total = w->metaObject()->numSlots( true );
			for ( i = 0; i < total; i++ ) {
			    const TQMetaData *md = w->metaObject()->slot( i, true );
			    MetaDataBase::Function funky;
			    // Find out if we have a return type.
			    if ( md->method->count > 0 ) {
				const TQUParameter p = md->method->parameters[0];
				if ( p.inOut == TQUParameter::InOut )
				    funky.returnType = p.type->desc();
			    }

			    funky.function = md->name;
			    funky.language = "C++";
			    switch ( md->access ) {
				case TQMetaData::Public:
				    funky.access = "public";
				    break;
				case TQMetaData::Protected:
				    funky.access = "protected";
				    break;
				case TQMetaData::Private:
				    funky.access = "private";
				    break;
			    }
			    cw->lstSlots.append( funky );
			}
			MetaDataBase::addCustomWidget( cw );
		    }
		    iface2->release();
		}
		iface->release();
	    }
	}
#endif // QT_CONTAINER_CUSTOM_WIDGETS
	if ( !saved )
	    saveChildrenOf( obj, ts, indent );
    }

    indent--;
    ts << closeTag;
}

void Resource::saveItems( TQObject *obj, TQTextStream &ts, int indent )
{
    if ( ::tqt_cast<TQListBox*>(obj) || ::tqt_cast<TQComboBox*>(obj) ) {
	TQListBox *lb = 0;
	if ( ::tqt_cast<TQListBox*>(obj) ) {
	    lb = (TQListBox*)obj;
	} else {
            TQComboBox *cb = (TQComboBox*)obj;
	    lb = cb->listBox();
            if (!lb) {
                TQPopupMenu *popup = (TQPopupMenu*)cb->child( 0, "TQPopupMenu" );
                Q_ASSERT(popup);
                for ( int id = 0; id < (int)popup->count(); ++id ) {
                    ts << makeIndent( indent ) << "<item>" << endl;
                    indent++;
                    TQStringList text;
                    text << popup->text(id);
                    TQPtrList<TQPixmap> pixmaps;
                    if ( popup->pixmap(id) )
                        pixmaps.append( popup->pixmap(id) );
                    saveItem( text, pixmaps, ts, indent );
                    indent--;
                    ts << makeIndent( indent ) << "</item>" << endl;
                }
            }
        }

        if (lb) {
            TQListBoxItem *i = lb->firstItem();
            for ( ; i; i = i->next() ) {
                ts << makeIndent( indent ) << "<item>" << endl;
                indent++;
                TQStringList text;
                text << i->text();
                TQPtrList<TQPixmap> pixmaps;
                if ( i->pixmap() )
                    pixmaps.append( i->pixmap() );
                saveItem( text, pixmaps, ts, indent );
                indent--;
                ts << makeIndent( indent ) << "</item>" << endl;
            }
        }

    } else if ( ::tqt_cast<TQIconView*>(obj) ) {
	TQIconView *iv = (TQIconView*)obj;

	TQIconViewItem *i = iv->firstItem();
	for ( ; i; i = i->nextItem() ) {
	    ts << makeIndent( indent ) << "<item>" << endl;
	    indent++;
	    TQStringList text;
	    text << i->text();
	    TQPtrList<TQPixmap> pixmaps;
	    if ( i->pixmap() )
		pixmaps.append( i->pixmap() );
	    saveItem( text, pixmaps, ts, indent );
	    indent--;
	    ts << makeIndent( indent ) << "</item>" << endl;
	}
    } else if ( ::tqt_cast<TQListView*>(obj) ) {
	TQListView *lv = (TQListView*)obj;
	int i;
	for ( i = 0; i < lv->header()->count(); ++i ) {
	    ts << makeIndent( indent ) << "<column>" << endl;
	    indent++;
	    TQStringList l;
	    l << lv->header()->label( i );
	    TQPtrList<TQPixmap> pix;
	    pix.setAutoDelete( true );
	    if ( lv->header()->iconSet( i ) )
		pix.append( new TQPixmap( lv->header()->iconSet( i )->pixmap() ) );
	    saveItem( l, pix, ts, indent );
	    ts << makeIndent( indent ) << "<property name=\"clickable\">" << endl;
	    indent++;
	    ts << makeIndent( indent ) << "<bool>" << mkBool( lv->header()->isClickEnabled( i ) )<< "</bool>" << endl;
	    indent--;
	    ts << makeIndent( indent ) << "</property>" << endl;
	    ts << makeIndent( indent ) << "<property name=\"resizable\">" << endl;
	    indent++;
	    ts << makeIndent( indent ) << "<bool>" << mkBool( lv->header()->isResizeEnabled( i ) ) << "</bool>" << endl;
	    indent--;
	    ts << makeIndent( indent ) << "</property>" << endl;
	    indent--;
	    ts << makeIndent( indent ) << "</column>" << endl;
	}
	if (lv->firstChild())
	{
	    saveItem( lv->firstChild(), ts, indent - 1 );
	}
    }
#if !defined (TQT_NO_TABLE)
    else if ( ::tqt_cast<TQTable*>(obj) ) {
	TQTable *table = (TQTable*)obj;
	int i;
	TQMap<TQString, TQString> columnFields = MetaDataBase::columnFields( table );
#  ifndef TQT_NO_SQL
	bool isDataTable = ::tqt_cast<TQDataTable*>(table);
#  else
        bool isDataTable = false;
#  endif
	for ( i = 0; i < table->horizontalHeader()->count(); ++i ) {
	    if ( ( !table->horizontalHeader()->label( i ).isNull() &&
		 table->horizontalHeader()->label( i ).toInt() != i + 1 ) ||
		 table->horizontalHeader()->iconSet( i ) ||
		  isDataTable ) {
		ts << makeIndent( indent ) << "<column>" << endl;
		indent++;
		TQStringList l;
		l << table->horizontalHeader()->label( i );
		TQPtrList<TQPixmap> pix;
		pix.setAutoDelete( true );
		if ( table->horizontalHeader()->iconSet( i ) )
		    pix.append( new TQPixmap( table->horizontalHeader()->iconSet( i )->pixmap() ) );
		saveItem( l, pix, ts, indent );
		if ( isDataTable && !columnFields.isEmpty() ) {
		    ts << makeIndent( indent ) << "<property name=\"field\">" << endl;
		    indent++;
		    ts << makeIndent( indent ) << "<string>" << entitize( *columnFields.find( l[ 0 ] ) ) << "</string>" << endl;
		    indent--;
		    ts << makeIndent( indent ) << "</property>" << endl;
		}
		indent--;
		ts << makeIndent( indent ) << "</column>" << endl;
	    }
	}
	for ( i = 0; i < table->verticalHeader()->count(); ++i ) {
	    if ( ( !table->verticalHeader()->label( i ).isNull() &&
		 table->verticalHeader()->label( i ).toInt() != i + 1 ) ||
		 table->verticalHeader()->iconSet( i ) ) {
		ts << makeIndent( indent ) << "<row>" << endl;
		indent++;
		TQStringList l;
		l << table->verticalHeader()->label( i );
		TQPtrList<TQPixmap> pix;
		pix.setAutoDelete( true );
		if ( table->verticalHeader()->iconSet( i ) )
		    pix.append( new TQPixmap( table->verticalHeader()->iconSet( i )->pixmap() ) );
		saveItem( l, pix, ts, indent );
		indent--;
		ts << makeIndent( indent ) << "</row>" << endl;
	    }
	}
    }
#endif
}

void Resource::saveItem( TQListViewItem *i, TQTextStream &ts, int indent )
{
    if (!i)
    {
	return;
    }

    TQListView *lv = i->listView();
    while ( i ) {
	ts << makeIndent( indent ) << "<item>" << endl;
	indent++;

	TQPtrList<TQPixmap> pixmaps;
	TQStringList textes;
	for ( int c = 0; c < lv->columns(); ++c ) {
	    pixmaps.append( i->pixmap( c ) );
	    textes << i->text( c );
	}
	saveItem( textes, pixmaps, ts, indent );

	if ( i->firstChild() )
	    saveItem( i->firstChild(), ts, indent );

	indent--;
	ts << makeIndent( indent ) << "</item>" << endl;
	i = i->nextSibling();
    }
}

void Resource::savePixmap( const TQPixmap &p, TQTextStream &ts, int indent, const TQString &tagname )
{
    if ( p.isNull() ) {
	ts << makeIndent( indent ) << "<" << tagname << "></"  << tagname << ">" << endl;
	return;
    }

    if ( formwindow && formwindow->savePixmapInline() )
	ts << makeIndent( indent ) << "<" << tagname << ">" << saveInCollection( p ) << "</" << tagname << ">" << endl;
    else if ( formwindow && formwindow->savePixmapInProject() )
	ts << makeIndent( indent ) << "<" << tagname << ">" << MetaDataBase::pixmapKey( formwindow, p.serialNumber() )
	   << "</" << tagname << ">" << endl;
    else
	ts << makeIndent( indent ) << "<" << tagname << ">" << MetaDataBase::pixmapArgument( formwindow, p.serialNumber() )
	   << "</" << tagname << ">" << endl;
}

TQPixmap Resource::loadPixmap( const TQDomElement &e, const TQString &/*tagname*/ )
{
    TQString arg = e.firstChild().toText().data();

    if ( formwindow && formwindow->savePixmapInline() ) {
	TQImage img = loadFromCollection( arg );
	TQPixmap pix;
	pix.convertFromImage( img );
	MetaDataBase::setPixmapArgument( formwindow, pix.serialNumber(), arg );
	return pix;
    } else if ( formwindow && formwindow->savePixmapInProject() ) {
	TQPixmap pix;
	if ( mainwindow && mainwindow->currProject() ) {
	    pix = mainwindow->currProject()->pixmapCollection()->pixmap( arg );
	} else {
	    pix = TQPixmap::fromMimeSource( "designer_image.png" );
	    // we have to force the pixmap to get a new and unique serial number. Unfortunately detatch() doesn't do that
	    pix.convertFromImage( pix.convertToImage() );
	}

	MetaDataBase::setPixmapKey( formwindow, pix.serialNumber(), arg );
	return pix;
    }
    TQPixmap pix = TQPixmap::fromMimeSource( "designer_image.png" );
    // we have to force the pixmap to get a new and unique serial number. Unfortunately detatch() doesn't do that
    pix.convertFromImage( pix.convertToImage() );
    MetaDataBase::setPixmapArgument( formwindow, pix.serialNumber(), arg );
    return pix;
}

void Resource::saveItem( const TQStringList &text,
			 const TQPtrList<TQPixmap> &pixmaps, TQTextStream &ts,
			 int indent )
{
    TQStringList::ConstIterator it = text.begin();
    for ( ; it != text.end(); ++it ) {
	ts << makeIndent( indent ) << "<property name=\"text\">" << endl;
	indent++;
	ts << makeIndent( indent ) << "<string>" << entitize( *it ) << "</string>" << endl;
	indent--;
	ts << makeIndent( indent ) << "</property>" << endl;
    }

    for ( int i = 0; i < (int)pixmaps.count(); ++i ) {
	TQPixmap *p = ( (TQPtrList<TQPixmap>)pixmaps ).at( i );
	ts << makeIndent( indent ) << "<property name=\"pixmap\">" << endl;
	indent++;
	if ( p )
	    savePixmap( *p, ts, indent );
	else
	    savePixmap( TQPixmap(), ts, indent );
	indent--;
	ts << makeIndent( indent ) << "</property>" << endl;
    }
}

void Resource::saveChildrenOf( TQObject* obj, TQTextStream &ts, int indent )
{
    const TQObjectList *l = obj->children();
    if ( !l )
	return; // no children to save

    TQString closeTag;
    // if the widget has a layout we pretend that all widget's childs are childs of the layout - makes the structure nicer
    TQLayout *layout = 0;
    TQDesignerGridLayout* grid = 0;
    if ( !::tqt_cast<TQSplitter*>(obj) &&
	 WidgetDatabase::isContainer( WidgetDatabase::idFromClassName( WidgetFactory::classNameOf( obj ) ) ) &&
	 obj->isWidgetType() &&
	 WidgetFactory::layoutType( (TQWidget*)obj, layout ) != WidgetFactory::NoLayout ) {
	WidgetFactory::LayoutType lay = WidgetFactory::layoutType( (TQWidget*)obj, layout );
	switch ( lay ) {
	case WidgetFactory::HBox:
	    closeTag = makeIndent( indent ) + "</hbox>";
	    ts << makeIndent( indent ) << "<hbox>" << endl;
	    ++indent;
	    break;
	case WidgetFactory::VBox:
	    closeTag = makeIndent( indent ) + "</vbox>";
	    ts << makeIndent( indent ) << "<vbox>" << endl;
	    ++indent;
	    break;
	case WidgetFactory::Grid:
	    closeTag = makeIndent( indent ) + "</grid>";
	    ts << makeIndent( indent ) << "<grid>" << endl;
	    ++indent;
	    grid = (TQDesignerGridLayout*) layout;
	    break;
	default:
	    break;
	}

	// save properties of layout
	if ( lay != WidgetFactory::NoLayout )
	    saveObjectProperties( layout, ts, indent );

    }

    TQObject *o = 0;
    for ( TQPtrListIterator<TQObject> it ( *l ); ( o = it.current() ); ++it )
	if ( !TQString( o->name() ).startsWith( "qt_dead_widget_" ) )
	    saveObject( o, grid, ts, indent );
    if ( !closeTag.isEmpty() ) {
	indent--;
	ts << closeTag << endl;
    }
}

void Resource::saveObjectProperties( TQObject *w, TQTextStream &ts, int indent )
{
    TQStringList saved;
    TQStringList changed;
    changed = MetaDataBase::changedProperties( w );
    if ( w->isWidgetType() ) {
	if ( ::tqt_cast<Spacer*>(w) ) {
	    if ( !changed.contains( "sizeHint" ) )
		changed << "sizeHint";
	    if ( !changed.contains( "geometry" ) )
		changed << "geometry";
 	} else {
	    TQToolButton *tb = ::tqt_cast<TQToolButton*>(w);
	    if ( tb && !tb->iconSet().isNull() ) {
		changed << "iconSet";
	    }
	}
    } else if ( ::tqt_cast<TQLayout*>(w) ) {
	if ( MetaDataBase::spacing( WidgetFactory::containerOfWidget( WidgetFactory::layoutParent( (TQLayout*)w ) ) ) > -1 )
	    changed << "spacing";
	if ( MetaDataBase::margin( WidgetFactory::containerOfWidget( WidgetFactory::layoutParent( (TQLayout*)w ) ) ) > -1 )
	    changed << "margin";
	if ( MetaDataBase::resizeMode( WidgetFactory::containerOfWidget( WidgetFactory::layoutParent( (TQLayout*)w ) ) ) != "Auto"
	     && !MetaDataBase::resizeMode( WidgetFactory::containerOfWidget( WidgetFactory::layoutParent( (TQLayout*)w ) ) ).isEmpty() )
	    changed << "resizeMode";
    }

    if ( w == formwindow->mainContainer() ) {
	if ( changed.findIndex( "geometry" ) == -1 )
	    changed << "geometry";
	if ( changed.findIndex( "caption" ) == -1 )
	    changed << "caption";
    }

    if ( changed.isEmpty() )
	    return;

    bool inLayout = w != formwindow->mainContainer() && !copying && w->isWidgetType() && ( (TQWidget*)w )->parentWidget() &&
		    WidgetFactory::layoutType( ( (TQWidget*)w )->parentWidget() ) != WidgetFactory::NoLayout;

    TQStrList lst = w->metaObject()->propertyNames( !::tqt_cast<Spacer*>(w) );
    for ( TQPtrListIterator<char> it( lst ); it.current(); ++it ) {
	if ( changed.find( TQString::fromLatin1( it.current() ) ) == changed.end() )
	    continue;
	if ( saved.find( TQString::fromLatin1( it.current() ) ) != saved.end() )
	    continue;
	saved << TQString::fromLatin1( it.current() );
	const TQMetaProperty* p = w->metaObject()->
				 property( w->metaObject()->findProperty( it.current(), true ), true );
	if ( !p || !p->stored( w ) || ( inLayout && qstrcmp( p->name(), "geometry" ) == 0 ) )
	    continue;
	if ( ::tqt_cast<TQLabel*>(w) && qstrcmp( p->name(), "pixmap" ) == 0 &&
	     ( !( (TQLabel*)w )->pixmap() || ( (TQLabel*)w )->pixmap()->isNull() ) )
	    continue;
	if ( ::tqt_cast<MenuBarEditor*>(w) &&
	     ( qstrcmp( p->name(), "itemName" ) == 0 || qstrcmp( p->name(), "itemNumber" ) == 0 ||
	       qstrcmp( p->name(), "itemText" ) == 0 ) )
	    continue;
	if ( qstrcmp( p->name(), "name" ) == 0 )
	    knownNames << w->property( "name" ).toString();
	if ( !p->isSetType() && !p->isEnumType() && !w->property( p->name() ).isValid() )
	    continue;
	ts << makeIndent( indent ) << "<property";
	ts << " name=\"" << it.current() << "\"";
	if ( !p->stdSet() )
	    ts << " stdset=\"0\"";
	ts << ">" << endl;
	indent++;
	if ( strcmp( it.current(), "resizeMode" ) == 0 && ::tqt_cast<TQLayout*>(w) ) {
	    saveProperty( w, it.current(), "", TQVariant::String, ts, indent );
	} else if ( p->isSetType() ) {
	    saveSetProperty( w, it.current(), TQVariant::nameToType( p->type() ), ts, indent );
	} else if ( p->isEnumType() ) {
	    saveEnumProperty( w, it.current(), TQVariant::nameToType( p->type() ), ts, indent );
	} else {
	    saveProperty( w, it.current(), w->property( p->name() ), TQVariant::nameToType( p->type() ), ts, indent );
	}
	indent--;
	ts << makeIndent( indent ) << "</property>" << endl;
    }

    if ( w->isWidgetType() && MetaDataBase::fakeProperties( w ) ) {
	TQStringVariantMap* fakeProperties = MetaDataBase::fakeProperties( w );
	for ( TQStringVariantMap::Iterator fake = fakeProperties->begin();
	      fake != fakeProperties->end(); ++fake ) {
	    if ( MetaDataBase::isPropertyChanged( w, fake.key() ) ) {
		if ( w->inherits("CustomWidget") ) {
		    MetaDataBase::CustomWidget *cw = ( (CustomWidget*)w )->customWidget();
		    if ( cw && !cw->hasProperty( fake.key().latin1() ) && fake.key() != "toolTip" && fake.key() != "whatsThis" )
			continue;
		}

		ts << makeIndent( indent ) << "<property name=\"" << fake.key() << "\" stdset=\"0\">" << endl;
		indent++;
		saveProperty( w, fake.key(), *fake, (*fake).type(), ts, indent );
		indent--;
		ts << makeIndent( indent ) << "</property>" << endl;
	    }
	}
    }
}

void Resource::saveSetProperty( TQObject *w, const TQString &name, TQVariant::Type, TQTextStream &ts, int indent )
{
    const TQMetaProperty *p = w->metaObject()->property( w->metaObject()->findProperty( name, true ), true );
    TQStrList l( p->valueToKeys( w->property( name ).toInt() ) );
    TQString v;
    for ( uint i = 0; i < l.count(); ++i ) {
	v += l.at( i );
	if ( i < l.count() - 1 )
	    v += "|";
    }
    ts << makeIndent( indent ) << "<set>" << v << "</set>" << endl;
}

void Resource::saveEnumProperty( TQObject *w, const TQString &name, TQVariant::Type, TQTextStream &ts, int indent )
{
    const TQMetaProperty *p = w->metaObject()->property( w->metaObject()->findProperty( name, true ), true );
    int value = w->property( name ).toInt();
    const char* key = p->valueToKey( value );
    if (key)
        ts << makeIndent( indent ) << "<enum>" << key << "</enum>" << endl;
}

void Resource::saveProperty( TQObject *w, const TQString &name, const TQVariant &value, TQVariant::Type t, TQTextStream &ts, int indent )
{
    if ( name == "hAlign" || name =="vAlign" || name == "wordwrap" ||
	 name == "layoutMargin" || name =="layoutSpacing" )
	return;
    int num;
    uint unum;
    double dob;
    TQString comment;
    if ( ( w && formwindow->widgets()->find( (TQWidget*)w ) ) || formwindow->actionList().find( (TQAction*)w ) )
	comment = MetaDataBase::propertyComment( w, name );
    switch ( t ) {
    case TQVariant::String:
	if ( name == "resizeMode" ) {
	    TQString resmod = MetaDataBase::resizeMode( WidgetFactory::containerOfWidget( WidgetFactory::layoutParent( (TQLayout*)w ) ) );
	    if ( !resmod.isNull() && resmod != "Auto" ) {
		ts << makeIndent( indent ) << "<enum>";
		ts << resmod;
		ts << "</enum>" << endl;
	    }

	} else {
	    ts << makeIndent( indent ) << "<string>" << entitize( value.toString() ) << "</string>" << endl;
	    if ( !comment.isEmpty() )
		ts << makeIndent( indent ) << "<comment>" << entitize( comment ) << "</comment>" << endl;
	}
	break;
    case TQVariant::CString:
	ts << makeIndent( indent ) << "<cstring>" << entitize( value.toCString() ).latin1() << "</cstring>" << endl;
	break;
    case TQVariant::Bool:
	ts << makeIndent( indent ) << "<bool>" << mkBool( value.toBool() ) << "</bool>" << endl;
	break;
    case TQVariant::Int:
	if ( ::tqt_cast<TQLayout*>(w) ) {
	    num = -1;
	    if ( name == "spacing" )
		num = MetaDataBase::spacing( WidgetFactory::containerOfWidget( WidgetFactory::layoutParent( (TQLayout*)w ) ) );
	    else if ( name == "margin" )
		num = MetaDataBase::margin( WidgetFactory::containerOfWidget( WidgetFactory::layoutParent( (TQLayout*)w ) ) );
	    if ( num != -1 )
		ts << makeIndent( indent ) << "<number>" << TQString::number( num ) << "</number>" << endl;
	} else {
	    num = value.toInt();
	    ts << makeIndent( indent ) << "<number>" << TQString::number( num ) << "</number>" << endl;
	}
	break;
    case TQVariant::Double:
	dob = value.toDouble();
	ts << makeIndent( indent ) << "<number>" << TQString::number( dob ) << "</number>" << endl;
	break;
    case TQVariant::KeySequence:
	ts << makeIndent( indent ) << "<string>"
	   << entitize(platformNeutralKeySequence(value.toKeySequence())) << "</string>" << endl;
	break;
    case TQVariant::UInt:
	unum = value.toUInt();
	ts << makeIndent( indent ) << "<number>" << TQString::number( unum ) << "</number>" << endl;
	break;
    case TQVariant::Rect: {
	TQVariant v( value );
	ts << makeIndent( indent ) << "<rect>" << endl;
	indent++;
	ts << makeIndent( indent ) << "<x>" << TQString::number( v.toRect().x() ) << "</x>" << endl;
	ts << makeIndent( indent ) << "<y>" << TQString::number( v.toRect().y() ) << "</y>" << endl;
	ts << makeIndent( indent ) << "<width>" << TQString::number( v.toRect().width() ) << "</width>" << endl;
	ts << makeIndent( indent ) << "<height>" << TQString::number( v.toRect().height() ) << "</height>" << endl;
	indent--;
	ts << makeIndent( indent ) << "</rect>" << endl;
    } break;
    case TQVariant::Point: {
	TQVariant v( value );
	ts << makeIndent( indent ) << "<point>" << endl;
	indent++;
	ts << makeIndent( indent ) << "<x>" << TQString::number( v.toPoint().x() ) << "</x>" << endl;
	ts << makeIndent( indent ) << "<y>" << TQString::number( v.toPoint().y() ) << "</y>" << endl;
	indent--;
	ts << makeIndent( indent ) << "</point>" << endl;
    } break;
    case TQVariant::Size: {
	TQVariant v( value );
	ts << makeIndent( indent ) << "<size>" << endl;
	indent++;
	ts << makeIndent( indent ) << "<width>" << TQString::number( v.toSize().width() ) << "</width>" << endl;
	ts << makeIndent( indent ) << "<height>" << TQString::number( v.toSize().height() ) << "</height>" << endl;
	indent--;
	ts << makeIndent( indent ) << "</size>" << endl;
    } break;
    case TQVariant::Color: {
	TQVariant v( value );
	ts << makeIndent( indent ) << "<color>" << endl;
	indent++;
	saveColor( ts, indent, v.toColor() );
	indent--;
	ts << makeIndent( indent ) << "</color>" << endl;
    } break;
    case TQVariant::Font: {
	TQVariant v( value );
	ts << makeIndent( indent ) << "<font>" << endl;
	TQFont f( tqApp->font() );
	if ( w && w->isWidgetType() && ((TQWidget*)w)->parentWidget() )
	    f = ((TQWidget*)w)->parentWidget()->font();
	TQFont f2( v.toFont() );
	indent++;
	if ( f.family() != f2.family() )
	    ts << makeIndent( indent ) << "<family>" << f2.family() << "</family>" << endl;
	if ( f.pointSize() != f2.pointSize() )
	    ts << makeIndent( indent ) << "<pointsize>" << TQString::number( f2.pointSize() ) << "</pointsize>" << endl;
	if ( f.bold() != f2.bold() )
	    ts << makeIndent( indent ) << "<bold>" << TQString::number( (int)f2.bold() ) << "</bold>" << endl;
	if ( f.italic() != f2.italic() )
	    ts << makeIndent( indent ) << "<italic>" << TQString::number( (int)f2.italic() ) << "</italic>" << endl;
	if ( f.underline() != f2.underline() )
	    ts << makeIndent( indent ) << "<underline>" << TQString::number( (int)f2.underline() ) << "</underline>" << endl;
	if ( f.strikeOut() != f2.strikeOut() )
	    ts << makeIndent( indent ) << "<strikeout>" << TQString::number( (int)f2.strikeOut() ) << "</strikeout>" << endl;
	indent--;
	ts << makeIndent( indent ) << "</font>" << endl;
    } break;
    case TQVariant::SizePolicy: {
	TQSizePolicy sp( value.toSizePolicy() );
	ts << makeIndent( indent ) << "<sizepolicy>" << endl;
	indent++;
	ts << makeIndent( indent ) << "<hsizetype>" << (int)sp.horData() << "</hsizetype>" << endl;
	ts << makeIndent( indent ) << "<vsizetype>" << (int)sp.verData() << "</vsizetype>" << endl;
	ts << makeIndent( indent ) << "<horstretch>" << (int)sp.horStretch() << "</horstretch>" << endl;
	ts << makeIndent( indent ) << "<verstretch>" << (int)sp.verStretch() << "</verstretch>" << endl;
	indent--;
	ts << makeIndent( indent ) << "</sizepolicy>" << endl;
	break;
    }
    case TQVariant::Pixmap:
	savePixmap( value.toPixmap(), ts, indent );
	break;
    case TQVariant::IconSet:
	savePixmap( value.toIconSet().pixmap(), ts, indent, "iconset" );
	break;
    case TQVariant::Image:
	ts << makeIndent( indent ) << "<image>" << saveInCollection( value.toImage() ) << "</image>" << endl;
    break;
    case TQVariant::Palette: {
	TQPalette p( value.toPalette() );
	ts << makeIndent( indent ) << "<palette>" << endl;
	indent++;

	ts << makeIndent( indent ) << "<active>" << endl;
	indent++;
	saveColorGroup( ts, indent, p.active() );
	indent--;
	ts << makeIndent( indent ) << "</active>" << endl;

	ts << makeIndent( indent ) << "<disabled>" << endl;
	indent++;
	saveColorGroup( ts, indent, p.disabled() );
	indent--;
	ts << makeIndent( indent ) << "</disabled>" << endl;

	ts << makeIndent( indent ) << "<inactive>" << endl;
	indent++;
	saveColorGroup( ts, indent, p.inactive() );
	indent--;
	ts << makeIndent( indent ) << "</inactive>" << endl;

	indent--;
	ts << makeIndent( indent ) << "</palette>" << endl;
    } break;
    case TQVariant::Cursor:
	ts << makeIndent( indent ) << "<cursor>" << value.toCursor().shape() << "</cursor>" << endl;
	break;
    case TQVariant::StringList: {
	TQStringList lst = value.toStringList();
	uint i = 0;
	ts << makeIndent( indent ) << "<stringlist>" << endl;
	indent++;
	if ( !lst.isEmpty() ) {
	    for ( i = 0; i < lst.count(); ++i )
		ts << makeIndent( indent ) << "<string>" << entitize( lst[ i ] ) << "</string>" << endl;
	}
	indent--;
	ts << makeIndent( indent ) << "</stringlist>" << endl;
    } break;
    case TQVariant::Date: {
	TQDate d = value.toDate();
	ts << makeIndent( indent ) << "<date>" << endl;
	indent++;
	ts << makeIndent( indent ) << "<year>" << d.year() << "</year>"  << endl;
	ts << makeIndent( indent ) << "<month>" << d.month() << "</month>"  << endl;
	ts << makeIndent( indent ) << "<day>" << d.day() << "</day>"  << endl;
	indent--;
	ts << makeIndent( indent ) << "</date>" << endl;
	break;
    }
    case TQVariant::Time: {
	TQTime t = value.toTime();
	ts << makeIndent( indent ) << "<time>" << endl;
	indent++;
	ts << makeIndent( indent ) << "<hour>" << t.hour() << "</hour>"  << endl;
	ts << makeIndent( indent ) << "<minute>" << t.minute() << "</minute>"  << endl;
	ts << makeIndent( indent ) << "<second>" << t.second() << "</second>"  << endl;
	indent--;
	ts << makeIndent( indent ) << "</time>" << endl;
	break;
    }
    case TQVariant::DateTime: {
	TQDateTime dt = value.toDateTime();
	ts << makeIndent( indent ) << "<datetime>" << endl;
	indent++;
	ts << makeIndent( indent ) << "<year>" << dt.date().year() << "</year>"  << endl;
	ts << makeIndent( indent ) << "<month>" << dt.date().month() << "</month>"  << endl;
	ts << makeIndent( indent ) << "<day>" << dt.date().day() << "</day>"  << endl;
	ts << makeIndent( indent ) << "<hour>" << dt.time().hour() << "</hour>"  << endl;
	ts << makeIndent( indent ) << "<minute>" << dt.time().minute() << "</minute>"  << endl;
	ts << makeIndent( indent ) << "<second>" << dt.time().second() << "</second>"  << endl;
	indent--;
	ts << makeIndent( indent ) << "</datetime>" << endl;
	break;
    }
    default:
	tqWarning( "saving the property %s of type %d not supported yet", name.latin1(), (int)t );
    }
}

void Resource::saveColorGroup( TQTextStream &ts, int indent, const TQColorGroup &cg )
{
    for( int r = 0 ; r < TQColorGroup::NColorRoles ; r++ ) {
	ts << makeIndent( indent ) << "<color>" << endl;
	indent++;
	saveColor( ts, indent, cg.color( (TQColorGroup::ColorRole)r ) );
	indent--;
	ts << makeIndent( indent ) << "</color>" << endl;
	TQPixmap* pm = cg.brush( (TQColorGroup::ColorRole)r ).pixmap();
	if ( pm && !pm->isNull() )
	    savePixmap( *pm, ts, indent );
    }
}

void Resource::saveColor( TQTextStream &ts, int indent, const TQColor &c )
{
    ts << makeIndent( indent ) << "<red>" << TQString::number( c.red() ) << "</red>" << endl;
    ts << makeIndent( indent ) << "<green>" << TQString::number( c.green() ) << "</green>" << endl;
    ts << makeIndent( indent ) << "<blue>" << TQString::number( c.blue() ) << "</blue>" << endl;
}

TQObject *Resource::createObject( const TQDomElement &e, TQWidget *parent, TQLayout* layout )
{
    lastItem = 0;
    TQDomElement n = e.firstChild().toElement();
    TQWidget *w = 0; // the widget that got created
    TQObject *obj = 0; // gets the properties

    int row = e.attribute( "row" ).toInt();
    int col = e.attribute( "column" ).toInt();
    int rowspan = e.attribute( "rowspan" ).toInt();
    int colspan = e.attribute( "colspan" ).toInt();
    if ( rowspan < 1 )
	rowspan = 1;
    if ( colspan < 1 )
	colspan = 1;

    TQString className = e.attribute( "class", "TQWidget" );
#ifdef QT_CONTAINER_CUSTOM_WIDGETS
    TQString parentClassName = WidgetFactory::classNameOf( parent );
    bool isPlugin =
	WidgetDatabase::isCustomPluginWidget( WidgetDatabase::idFromClassName( parentClassName ) );
    if ( isPlugin )
	tqWarning( "####### loading custom container widgets without page support not implemented!" );
    // ### TODO loading for custom container widgets without pages
#endif
    if ( !className.isNull() ) {
	obj = WidgetFactory::create( WidgetDatabase::idFromClassName( className ), parent, 0, false );
	if ( !obj ) {
	    TQMessageBox::critical( MainWindow::self, MainWindow::tr( "Loading File" ),
				   MainWindow::tr( "Error loading %1.\n"
						   "The widget %2 couldn't be created.\n"
                                                   "Information may be lost if this form is saved." ).
				   arg( currFileName ).arg( className ) );
	    return 0;
	}
	if ( !mainContainerSet ) {
	    if ( formwindow )
		formwindow->setMainContainer( (TQWidget*)obj );
	    mainContainerSet = true;
	}
	w = (TQWidget*)obj;
	if ( ::tqt_cast<TQMainWindow*>(w) )
	    w = ( (TQMainWindow*)w )->centralWidget();
	if ( layout ) {
	    switch ( WidgetFactory::layoutType( layout ) ) {
	    case WidgetFactory::HBox:
		( (TQHBoxLayout*)layout )->addWidget( w );
		break;
	    case WidgetFactory::VBox:
		( (TQVBoxLayout*)layout )->addWidget( w );
		break;
	    case WidgetFactory::Grid:
		( (TQDesignerGridLayout*)layout )->addMultiCellWidget( w, row, row + rowspan - 1,
								      col, col + colspan - 1 );
		break;
	    default:
		break;
	    }
	}

	if ( !toplevel )
	    toplevel = w;
	layout = 0;

	if ( w && formwindow ) {
	    if ( !parent ||
		 ( !::tqt_cast<TQTabWidget*>(parent) &&
		   !::tqt_cast<TQWidgetStack*>(parent) &&
		   !::tqt_cast<TQToolBox*>(parent) &&
		   !::tqt_cast<TQWizard*>(parent)
#ifdef QT_CONTAINER_CUSTOM_WIDGETS
                   && !isPlugin
#endif
                     ) )
		formwindow->insertWidget( w, pasting );
	    else if ( parent &&
		      ( ::tqt_cast<TQTabWidget*>(parent) ||
			::tqt_cast<TQWidgetStack*>(parent) ||
			::tqt_cast<TQToolBox*>(parent) ||
			::tqt_cast<TQWizard*>(parent)
#ifdef QT_CONTAINER_CUSTOM_WIDGETS
                        || isPlugin
#endif
                          ) )
		MetaDataBase::addEntry( w );
	}
    }

    TQDomElement sizePolicyElement;
    TQObject *sizePolicyObject = 0;

    while ( !n.isNull() ) {
	if ( n.tagName() == "spacer" ) {
	    createSpacer( n, w, layout, TQt::Horizontal );
	} else if ( n.tagName() == "widget" ) {
	    createObject( n, w, layout );
	} else if ( n.tagName() == "hbox" ) {
	    layout = WidgetFactory::createLayout( w, layout, WidgetFactory::HBox );
	    obj = layout;
	    n = n.firstChild().toElement();
	    continue;
	} else if ( n.tagName() == "grid" ) {
	    layout = WidgetFactory::createLayout( w, layout, WidgetFactory::Grid );
	    obj = layout;
	    n = n.firstChild().toElement();
	    continue;
	} else if ( n.tagName() == "vbox" ) {
	    layout = WidgetFactory::createLayout( w, layout, WidgetFactory::VBox );
	    obj = layout;
	    n = n.firstChild().toElement();
	    continue;
	} else if ( n.tagName() == "property" && obj ) {
	    if ( n.attribute( "name" ) == "sizePolicy" ) {
		// ### Evil hack ### Delay setting sizePolicy so it won't be overridden by other properties.
		sizePolicyElement = n;
		sizePolicyObject = obj;
	    } else {
		setObjectProperty( obj, n.attribute( "name" ), n.firstChild().toElement() );
	    }
	} else if ( n.tagName() == "attribute" && w ) {
	    TQString attrib = n.attribute( "name" );
	    TQVariant v = DomTool::elementToVariant( n.firstChild().toElement(), TQVariant() );
	    if ( ::tqt_cast<TQTabWidget*>(parent) ) {
		if ( attrib == "title" )
		    ( (TQTabWidget*)parent )->insertTab( w, v.toString() );
	    } else if ( ::tqt_cast<TQWidgetStack*>(parent) ) {
		if ( attrib == "id" )
		    ( (TQDesignerWidgetStack*)parent )->insertPage( w, v.toInt() );
	    } else if ( ::tqt_cast<TQToolBox*>(parent) ) {
		if ( attrib == "label" )
		    ( (TQToolBox*)parent )->addItem( w, v.toString() );
	    } else if ( ::tqt_cast<TQWizard*>(parent) ) {
		if ( attrib == "title" )
		    ( (TQWizard*)parent )->addPage( w, v.toString() );
#ifdef QT_CONTAINER_CUSTOM_WIDGETS
	    } else if ( isPlugin ) {
		if ( attrib == "label" ) {
		    WidgetInterface *iface = 0;
		    widgetManager()->queryInterface( parentClassName, &iface );
		    if ( iface ) {
			TQWidgetContainerInterfacePrivate *iface2 = 0;
			iface->queryInterface( IID_QWidgetContainer, (TQUnknownInterface**)&iface2 );
			if ( iface2 ) {
			    iface2->insertPage( parentClassName,
						(TQWidget*)parent, v.toString(), -1, w );
			    iface2->release();
			}
			iface->release();
		    }
		}
#endif // QT_CONTAINER_CUSTOM_WIDGETS
	    }
	} else if ( n.tagName() == "item" ) {
	    createItem( n, w );
	} else if ( n.tagName() == "column" || n.tagName() =="row" ) {
	    createColumn( n, w );
	}

	n = n.nextSibling().toElement();
    }

    // ### Evil hack ### See description above.
    if ( !sizePolicyElement.isNull() ) {
	setObjectProperty( sizePolicyObject,
			   sizePolicyElement.attribute( "name" ),
			   sizePolicyElement.firstChild().toElement() );
    }

    if ( w->isWidgetType() )
	widgets.insert( w->name(), w );

    return w;
}

void Resource::createColumn( const TQDomElement &e, TQWidget *widget )
{
    if ( !widget )
	return;

    if ( ::tqt_cast<TQListView*>(widget) && e.tagName() == "column" ) {
	TQListView *lv = (TQListView*)widget;
	TQDomElement n = e.firstChild().toElement();
	TQPixmap pix;
	bool hasPixmap = false;
	TQString txt;
	bool clickable = true, resizable = true;
	while ( !n.isNull() ) {
	    if ( n.tagName() == "property" ) {
		TQString attrib = n.attribute( "name" );
		TQVariant v = DomTool::elementToVariant( n.firstChild().toElement(), TQVariant() );
		if ( attrib == "text" )
		    txt = v.toString();
		else if ( attrib == "pixmap" ) {
		    pix = loadPixmap( n.firstChild().toElement().toElement() );
		    hasPixmap = !pix.isNull();
		} else if ( attrib == "clickable" )
		    clickable = v.toBool();
		else if ( attrib == "resizable" )
		    resizable = v.toBool();
	    }
	    n = n.nextSibling().toElement();
	}
	lv->addColumn( txt );
	int i = lv->header()->count() - 1;
	if ( hasPixmap ) {
	    lv->header()->setLabel( i, pix, txt );
	}
	if ( !clickable )
	    lv->header()->setClickEnabled( clickable, i );
	if ( !resizable )
	    lv->header()->setResizeEnabled( resizable, i );
    }
#ifndef TQT_NO_TABLE
    else if ( ::tqt_cast<TQTable*>(widget) ) {
	TQTable *table = (TQTable*)widget;
	bool isRow;
	if ( ( isRow = e.tagName() == "row" ) )
	    table->setNumRows( table->numRows() + 1 );
	else
	    table->setNumCols( table->numCols() + 1 );

	TQDomElement n = e.firstChild().toElement();
	TQPixmap pix;
	bool hasPixmap = false;
	TQString txt;
	TQString field;
	TQMap<TQString, TQString> fieldMap = MetaDataBase::columnFields( table );
	while ( !n.isNull() ) {
	    if ( n.tagName() == "property" ) {
		TQString attrib = n.attribute( "name" );
		TQVariant v = DomTool::elementToVariant( n.firstChild().toElement(), TQVariant() );
		if ( attrib == "text" )
		    txt = v.toString();
		else if ( attrib == "pixmap" ) {
		    hasPixmap = !n.firstChild().firstChild().toText().data().isEmpty();
		    if ( hasPixmap )
			pix = loadPixmap( n.firstChild().toElement() );
		} else if ( attrib == "field" )
		    field = v.toString();
	    }
	    n = n.nextSibling().toElement();
	}

	int i = isRow ? table->numRows() - 1 : table->numCols() - 1;
	TQHeader *h = !isRow ? table->horizontalHeader() : table->verticalHeader();
	if ( hasPixmap )
	    h->setLabel( i, pix, txt );
	else
	    h->setLabel( i, txt );
	if ( !isRow && !field.isEmpty() )
	    fieldMap.insert( txt, field );
	MetaDataBase::setColumnFields( table, fieldMap );
    }
#endif
}

void Resource::loadItem( const TQDomElement &e, TQPixmap &pix, TQString &txt, bool &hasPixmap )
{
    TQDomElement n = e;
    hasPixmap = false;
    while ( !n.isNull() ) {
	if ( n.tagName() == "property" ) {
	    TQString attrib = n.attribute( "name" );
	    TQVariant v = DomTool::elementToVariant( n.firstChild().toElement(), TQVariant() );
	    if ( attrib == "text" )
		txt = v.toString();
	    else if ( attrib == "pixmap" ) {
		pix = loadPixmap( n.firstChild().toElement() );
		hasPixmap = !pix.isNull();
	    }
	}
	n = n.nextSibling().toElement();
    }
}

void Resource::createItem( const TQDomElement &e, TQWidget *widget, TQListViewItem *i )
{
    if ( !widget || !WidgetFactory::hasItems( WidgetDatabase::idFromClassName( WidgetFactory::classNameOf( widget ) ), widget ) )
	return;

    if ( ::tqt_cast<TQListBox*>(widget) || ::tqt_cast<TQComboBox*>(widget) ) {
	TQDomElement n = e.firstChild().toElement();
	TQPixmap pix;
	bool hasPixmap = false;
	TQString txt;
	loadItem( n, pix, txt, hasPixmap );
	TQListBox *lb = 0;
	if ( ::tqt_cast<TQListBox*>(widget) ) {
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
    } else if ( ::tqt_cast<TQIconView*>(widget) ) {
	TQDomElement n = e.firstChild().toElement();
	TQPixmap pix;
	bool hasPixmap = false;
	TQString txt;
	loadItem( n, pix, txt, hasPixmap );
	TQIconView *iv = (TQIconView*)widget;
	if ( hasPixmap )
	    new TQIconViewItem( iv, txt, pix );
	else
	    new TQIconViewItem( iv, txt );
    } else if ( ::tqt_cast<TQListView*>(widget) ) {
	TQDomElement n = e.firstChild().toElement();
	TQPixmap pix;
	TQValueList<TQPixmap> pixmaps;
	TQStringList textes;
	TQListViewItem *item = 0;
	TQListView *lv = (TQListView*)widget;
	if ( i )
	    item = new TQListViewItem( i, lastItem );
	else
	    item = new TQListViewItem( lv, lastItem );
	while ( !n.isNull() ) {
	    if ( n.tagName() == "property" ) {
		TQString attrib = n.attribute( "name" );
		TQVariant v = DomTool::elementToVariant( n.firstChild().toElement(), TQVariant() );
		if ( attrib == "text" )
		    textes << v.toString();
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
	lastItem = item;
    }
}

TQWidget *Resource::createSpacer( const TQDomElement &e, TQWidget *parent, TQLayout *layout, TQt::Orientation o )
{
    TQDomElement n = e.firstChild().toElement();
    int row = e.attribute( "row" ).toInt();
    int col = e.attribute( "column" ).toInt();
    int rowspan = e.attribute( "rowspan" ).toInt();
    int colspan = e.attribute( "colspan" ).toInt();
    if ( rowspan < 1 )
	rowspan = 1;
    if ( colspan < 1 )
	colspan = 1;

    Spacer *spacer = (Spacer*) WidgetFactory::create( WidgetDatabase::idFromClassName("Spacer"),
						      parent, "spacer", false);
    spacer->setOrientation( o );
    spacer->setInteraciveMode( false );
    while ( !n.isNull() ) {
	if ( n.tagName() == "property" )
	    setObjectProperty( spacer, n.attribute( "name" ), n.firstChild().toElement() );
	n = n.nextSibling().toElement();
    }
    spacer->setInteraciveMode( true );
    if ( formwindow )
	formwindow->insertWidget( spacer, pasting );
    if ( layout ) {
	if ( ::tqt_cast<TQBoxLayout*>(layout) )
	    ( (TQBoxLayout*)layout )->addWidget( spacer, 0, spacer->alignment() );
	else
	    ( (TQDesignerGridLayout*)layout )->addMultiCellWidget( spacer, row, row + rowspan - 1, col, col + colspan - 1,
								  spacer->alignment() );
    }
    return spacer;
}

/*!
  Attention: this function has to be in sync with Uic::setObjectProperty(). If you change one, change both.
*/
void Resource::setObjectProperty( TQObject* obj, const TQString &prop, const TQDomElement &e )
{
    const TQMetaProperty *p = obj->metaObject()->property( obj->metaObject()->findProperty( prop, true ), true );

    if ( !::tqt_cast<TQLayout*>(obj)  ) {// no layouts in metadatabase... (RS)
	if ( obj->inherits( "CustomWidget" ) ) {
	    MetaDataBase::CustomWidget *cw = ( (CustomWidget*)obj )->customWidget();
	    if ( cw && !cw->hasProperty( prop.latin1() ) && !p && prop != "toolTip" && prop != "whatsThis" )
		return;
	}
	MetaDataBase::setPropertyChanged( obj, prop, true );
    }
    
    TQVariant defVarient;
    if ( e.tagName() == "font" ) {
	TQFont f( tqApp->font() );
	if ( obj->isWidgetType() && ( (TQWidget*)obj )->parentWidget() )
	    f = ( (TQWidget*)obj )->parentWidget()->font();
	defVarient = TQVariant( f );
    }

    TQString comment;
    TQVariant v( DomTool::elementToVariant( e, defVarient, comment ) );

    if ( !comment.isEmpty() ) {
	MetaDataBase::addEntry( obj );
	MetaDataBase::setPropertyComment( obj, prop, comment );
    }

    if ( e.tagName() == "pixmap" ) {
	TQPixmap pix = loadPixmap( e );
	if ( pix.isNull() )
	    return;
	v = TQVariant( pix );
    } else if ( e.tagName() == "iconset" ) {
	TQPixmap pix = loadPixmap( e, "iconset" );
	if ( pix.isNull() )
	    return;
	v = TQVariant( TQIconSet( pix ) );
    } else if ( e.tagName() == "image" ) {
	v = TQVariant( loadFromCollection( v.toString() ) );
    }

    if ( !p ) {
	MetaDataBase::setFakeProperty( obj, prop, v );
	if ( obj->isWidgetType() ) {
	    if ( prop == "database" && obj != toplevel ) {
		TQStringList lst = MetaDataBase::fakeProperty( obj, "database" ).toStringList();
		if ( lst.count() > 2 )
		    dbControls.insert( obj->name(), lst[ 2 ] );
		else if ( lst.count() == 2 )
		    dbTables.insert( obj->name(), lst );
	    }
	    return;
	}
    }

    if ( e.tagName() == "palette" ) {
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
	v = TQPalette( p );
    } else if ( e.tagName() == "enum" && p && p->isEnumType() && prop != "resizeMode" ) {
	TQString key( v.toString() );
	int vi = p->keyToValue( key );
 	if ( vi == -1 )
 	    return; // ignore invalid properties
	v = TQVariant( vi );
    } else if ( e.tagName() == "set" && p && p->isSetType() ) {
	TQString keys( v.toString() );
	TQStringList lst = TQStringList::split( '|', keys );
	TQStrList l;
	for ( TQStringList::Iterator it = lst.begin(); it != lst.end(); ++it )
	    l.append( *it );
	v = TQVariant( p->keysToValue( l ) );
    }

    if ( prop == "caption" ) {
	TQCString s1 = v.toCString();
	TQString s2 = v.toString();
	if ( !s2.isEmpty() )
	    formwindow->setCaption( s2 );
	else if ( !s1.isEmpty() )
	    formwindow->setCaption( s1 );
    }
    if ( prop == "icon" ) {
	formwindow->setIcon( v.toPixmap() );
	TQString pmk = MetaDataBase::pixmapKey( formwindow, v.toPixmap().serialNumber() );
	MetaDataBase::setPixmapKey( formwindow,
				    formwindow->icon()->serialNumber(), pmk );
    }

    if ( prop == "geometry" ) {
	if ( obj == toplevel ) {
	    hadGeometry = true;
	    toplevel->resize( v.toRect().size() );
	    return;
	} else if ( obj == formwindow->mainContainer() ) {
	    hadGeometry = true;
	    formwindow->resize( v.toRect().size() );
	    return;
	}
    }

    if ( ::tqt_cast<TQLayout*>(obj) ) {
	if ( prop == "spacing" ) {
	    MetaDataBase::setSpacing( WidgetFactory::containerOfWidget( WidgetFactory::layoutParent( (TQLayout*)obj ) ), v.toInt() );
	    return;
	} else if ( prop == "margin" ) {
	    MetaDataBase::setMargin( WidgetFactory::containerOfWidget( WidgetFactory::layoutParent( (TQLayout*)obj ) ), v.toInt() );
	    return;
	} else if ( e.tagName() == "enum" &&  prop == "resizeMode" ) {
	    MetaDataBase::setResizeMode( WidgetFactory::containerOfWidget( WidgetFactory::layoutParent( (TQLayout*)obj ) ), v.toString() );
	    return;
	}
    }

    if ( prop == "name" ) {
	if ( pasting ) {
	    TQString s = v.toString();
	    formwindow->unify( (TQWidget*)obj, s, true );
	    obj->setName( s );
	    return;
	} else if ( formwindow && obj == formwindow->mainContainer() ) {
	    formwindow->setName( v.toCString() );
	}
    }

    if ( prop == "sizePolicy" ) {
	TQSizePolicy sp = v.toSizePolicy();
	sp.setHeightForWidth( ( (TQWidget*)obj )->sizePolicy().hasHeightForWidth() );
    }

    if ( prop == "cursor" )
	MetaDataBase::setCursor( (TQWidget*)obj, v.toCursor() );

    obj->setProperty( prop, v );
}


TQString Resource::saveInCollection( const TQImage &img )
{
    TQString imgName = "none";
    TQValueList<Image>::Iterator it = images.begin();
    for ( ; it != images.end(); ++it ) {
	if ( img == ( *it ).img ) {
	    imgName = ( *it ).name;
	    break;
	}
    }

    if ( imgName == "none" ) {
	Image i;
	imgName = "image" + TQString::number( images.count() );
	i.name = imgName;
	i.img = img;
	images.append( i );
    }
    return imgName;
}

void Resource::saveImageData( const TQImage &img, TQTextStream &ts, int indent )
{
    TQByteArray ba;
    TQBuffer buf( ba );
    buf.open( IO_WriteOnly | IO_Translate );
    TQString format;
    bool compress = false;
    if (img.hasAlphaBuffer()) {
	format = "PNG";
    } else {
	format = img.depth() > 1 ? "XPM" : "XBM";
	compress = true;
    }
    TQImageIO iio( &buf, format );
    iio.setImage( img );
    iio.write();
    buf.close();
    TQByteArray bazip = ba;
    int i = 0;
    if (compress) {
	bazip = tqCompress( ba );
	format += ".GZ";
	// The first 4 bytes in tqCompress() are the length of the unzipped
	// format. The XPM.GZ format does not use these.
	i = 4;
    }
    ulong len = bazip.size();
    ts << makeIndent( indent ) << "<data format=\"" + format + "\" length=\"" << ba.size() << "\">";
    static const char hexchars[] = "0123456789abcdef";
    for (; i < (int)len; ++i ) {
	uchar s = (uchar) bazip[i];
	ts << hexchars[s >> 4];
	ts << hexchars[s & 0x0f];
    }
    ts << "</data>" << endl;
}

void Resource::saveImageCollection( TQTextStream &ts, int indent )
{
    ts << makeIndent( indent ) << "<images>" << endl;
    indent++;

    TQValueList<Image>::Iterator it = images.begin();
    for ( ; it != images.end(); ++it ) {
	ts << makeIndent( indent ) << "<image name=\"" << (*it).name << "\">" << endl;
	indent++;
	saveImageData( (*it).img, ts, indent );
	indent--;
	ts << makeIndent( indent ) << "</image>" << endl;
    }

    indent--;
    ts << makeIndent( indent ) << "</images>" << endl;
}

static TQImage loadImageData( TQDomElement &n2 )
{
    TQImage img;
    TQString data = n2.firstChild().toText().data();
    const int lengthOffset = 4;
    int baSize = data.length() / 2 + lengthOffset;
    uchar *ba = new uchar[ baSize ];
    for ( int i = lengthOffset; i < baSize; ++i ) {
	char h = data[ 2 * (i-lengthOffset) ].latin1();
	char l = data[ 2 * (i-lengthOffset) + 1 ].latin1();
	uchar r = 0;
	if ( h <= '9' )
	    r += h - '0';
	else
	    r += h - 'a' + 10;
	r = r << 4;
	if ( l <= '9' )
	    r += l - '0';
	else
	    r += l - 'a' + 10;
	ba[ i ] = r;
    }
    TQString format = n2.attribute( "format", "PNG" );
    if ( format == "XPM.GZ" || format == "XBM.GZ" ) {
	ulong len = n2.attribute( "length" ).toULong();
	if ( len < data.length() * 5 )
	    len = data.length() * 5;
	// tqUncompress() expects the first 4 bytes to be the expected length of
	// the uncompressed data
	ba[0] = ( len & 0xff000000 ) >> 24;
	ba[1] = ( len & 0x00ff0000 ) >> 16;
	ba[2] = ( len & 0x0000ff00 ) >> 8;
	ba[3] = ( len & 0x000000ff );
	TQByteArray baunzip = tqUncompress( ba, baSize );
	img.loadFromData( (const uchar*)baunzip.data(), baunzip.size(), format.left(format.find('.')) );
    }  else {
	img.loadFromData( (const uchar*)ba+lengthOffset, baSize-lengthOffset, format );
    }
    delete [] ba;
    return img;
}

void Resource::loadImageCollection( const TQDomElement &e )
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

TQImage Resource::loadFromCollection( const TQString &name )
{
    TQValueList<Image>::Iterator it = images.begin();
    for ( ; it != images.end(); ++it ) {
	if ( ( *it ).name == name )
	    return ( *it ).img;
    }
    return TQImage();
}

void Resource::saveConnections( TQTextStream &ts, int indent )
{
    TQValueList<MetaDataBase::Connection> connections = MetaDataBase::connections( formwindow );
    if ( connections.isEmpty() )
	return;
    ts << makeIndent( indent ) << "<connections>" << endl;
    indent++;
    TQValueList<MetaDataBase::Connection>::Iterator it = connections.begin();
    for ( ; it != connections.end(); ++it ) {
	MetaDataBase::Connection conn = *it;
	if ( ( knownNames.findIndex( TQString( conn.sender->name() ) ) == -1 &&
	       qstrcmp( conn.sender->name(), "this" ) != 0 ) ||
	     ( knownNames.findIndex( TQString( conn.receiver->name() ) ) == -1 &&
	       qstrcmp( conn.receiver->name(), "this" ) != 0 ) )
	    continue;
	if ( formwindow->isMainContainer( (TQWidget*)(*it).receiver ) &&
	     !MetaDataBase::hasSlot( formwindow, MetaDataBase::normalizeFunction( (*it).slot ).latin1() ) )
	    continue;

	if ( conn.sender->inherits( "CustomWidget" ) ) {
	    MetaDataBase::CustomWidget *cw = ( (CustomWidget*)conn.sender )->customWidget();
	    if ( cw && !cw->hasSignal( conn.signal ) )
		continue;
	}

	if ( conn.receiver->inherits( "CustomWidget" ) && !formwindow->isMainContainer( conn.receiver ) ) {
	    MetaDataBase::CustomWidget *cw = ( (CustomWidget*)conn.receiver )->customWidget();
	    if ( cw && !cw->hasSlot( MetaDataBase::normalizeFunction( conn.slot ).latin1() ) )
		continue;
	}

	ts << makeIndent( indent ) << "<connection>" << endl;
	indent++;
	ts << makeIndent( indent ) << "<sender>" << entitize( conn.sender->name() ) << "</sender>" << endl;
	ts << makeIndent( indent ) << "<signal>" << entitize( conn.signal ) << "</signal>" << endl;
	ts << makeIndent( indent ) << "<receiver>" << entitize( conn.receiver->name() ) << "</receiver>" << endl;
	ts << makeIndent( indent ) << "<slot>" << entitize( MetaDataBase::normalizeFunction( conn.slot ) ) << "</slot>" << endl;
	indent--;
	ts << makeIndent( indent ) << "</connection>" << endl;
    }

    TQString lang = formwindow->project()->language();
    indent--;
    ts << makeIndent( indent ) << "</connections>" << endl;
}

void Resource::loadConnections( const TQDomElement &e )
{
    TQDomElement n = e.firstChild().toElement();
    while ( !n.isNull() ) {
	if ( n.tagName() == "connection" ) {
	    TQString lang = n.attribute( "language", "C++" );
	    TQDomElement n2 = n.firstChild().toElement();
	    MetaDataBase::Connection conn;
	    while ( !n2.isNull() ) {
		if ( n2.tagName() == "sender" ) {
		    conn.sender = 0;
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
			    l = 0;
			}
			if ( !conn.sender )
			    conn.sender = formwindow->findAction( name );
		    }
		} else if ( n2.tagName() == "signal" ) {
		    conn.signal = n2.firstChild().toText().data();
		} else if ( n2.tagName() == "receiver" ) {
		    conn.receiver = 0;
		    TQString name = n2.firstChild().toText().data();
		    if ( name == "this" || qstrcmp( toplevel->name(), name ) == 0 ) {
			conn.receiver = toplevel;
		    } else {
			TQObjectList *l = toplevel->queryList( 0, name, false );
			if ( l ) {
			    if ( l->first() )
				conn.receiver = l->first();
			    delete l;
			    l = 0;
			}
			if ( !conn.receiver )
			    conn.receiver = formwindow->findAction( name );
		    }
		} else if ( n2.tagName() == "slot" ) {
		    conn.slot = n2.firstChild().toText().data();
		}
		n2 = n2.nextSibling().toElement();
	    }
	    if ( formwindow ) {
		if ( conn.sender == formwindow )
		    conn.sender = formwindow->mainContainer();
		if ( conn.receiver == formwindow )
		    conn.receiver = formwindow->mainContainer();
	    }
	    if ( conn.sender && conn.receiver ) {
		if ( lang == "C++" ) {
		    MetaDataBase::addConnection( formwindow ? formwindow : toplevel,
						 conn.sender, conn.signal, conn.receiver, conn.slot );
		}
	    }
	} else if ( n.tagName() == "slot" ) { // compatibility with 2.x
	    MetaDataBase::Function slot;
	    slot.specifier = n.attribute( "specifier", "virtual" );
	    if ( slot.specifier.isEmpty() )
		slot.specifier = "virtual";
	    slot.access = n.attribute( "access", "public" );
	    if ( slot.access.isEmpty() )
		slot.access = "public";
	    slot.language = n.attribute( "language", "C++" );
	    slot.returnType = n.attribute( "returnType", "void" );
	    if ( slot.returnType.isEmpty() )
		slot.returnType = "void";
	    slot.function = n.firstChild().toText().data();
	    if ( !MetaDataBase::hasFunction( formwindow, slot.function, true ) )
		MetaDataBase::addFunction( formwindow, slot.function, slot.specifier,
				       slot.access, "slot", slot.language, slot.returnType );
	    else
		MetaDataBase::changeFunctionAttributes( formwindow, slot.function, slot.function,
							slot.specifier, slot.access,
							"slot", slot.language, slot.returnType );
	}
	n = n.nextSibling().toElement();
    }
}

void Resource::saveCustomWidgets( TQTextStream &ts, int indent )
{
    ts << makeIndent( indent ) << "<customwidgets>" << endl;
    indent++;

    TQPtrList<MetaDataBase::CustomWidget> *lst = MetaDataBase::customWidgets();
    for ( MetaDataBase::CustomWidget *w = lst->first(); w; w = lst->next() ) {
	if ( usedCustomWidgets.findIndex( w->className ) == -1 )
	    continue;
	ts << makeIndent( indent ) << "<customwidget>" << endl;
	indent++;
	ts << makeIndent( indent ) << "<class>" << w->className << "</class>" << endl;
	ts << makeIndent( indent ) << "<header location=\""
	   << ( w->includePolicy == MetaDataBase::CustomWidget::Local ? "local" : "global" )
	   << "\">" << w->includeFile << "</header>" << endl;
	ts << makeIndent( indent ) << "<sizehint>" << endl;
	indent++;
	ts << makeIndent( indent ) << "<width>" << w->sizeHint.width() << "</width>" << endl;
	ts << makeIndent( indent ) << "<height>" << w->sizeHint.height() << "</height>" << endl;
	indent--;
	ts << makeIndent( indent ) << "</sizehint>" << endl;
	ts << makeIndent( indent ) << "<container>" << (int)w->isContainer << "</container>" << endl;
	ts << makeIndent( indent ) << "<sizepolicy>" << endl;
	indent++;
	ts << makeIndent( indent ) << "<hordata>" << (int)w->sizePolicy.horData() << "</hordata>" << endl;
	ts << makeIndent( indent ) << "<verdata>" << (int)w->sizePolicy.verData() << "</verdata>" << endl;
	ts << makeIndent( indent ) << "<horstretch>" << (int)w->sizePolicy.horStretch() << "</horstretch>" << endl;
	ts << makeIndent( indent ) << "<verstretch>" << (int)w->sizePolicy.verStretch() << "</verstretch>" << endl;
	indent--;
	ts << makeIndent( indent ) << "</sizepolicy>" << endl;
	ts << makeIndent( indent ) << "<pixmap>" << saveInCollection( w->pixmap->convertToImage() ) << "</pixmap>" << endl;
	if ( !w->lstSignals.isEmpty() ) {
	    for ( TQValueList<TQCString>::Iterator it = w->lstSignals.begin(); it != w->lstSignals.end(); ++it )
		ts << makeIndent( indent ) << "<signal>" << entitize( *it ) << "</signal>" << endl;
	}
	if ( !w->lstSlots.isEmpty() ) {
	    for ( TQValueList<MetaDataBase::Function>::Iterator it = w->lstSlots.begin(); it != w->lstSlots.end(); ++it )
		ts << makeIndent( indent ) << "<slot access=\"" << (*it).access << "\" specifier=\""
		   << (*it).specifier << "\">" << entitize( (*it).function ) << "</slot>" << endl;
	}
	if ( !w->lstProperties.isEmpty() ) {
	    for ( TQValueList<MetaDataBase::Property>::Iterator it = w->lstProperties.begin(); it != w->lstProperties.end(); ++it )
		ts << makeIndent( indent ) << "<property type=\"" << (*it).type << "\">" << entitize( (*it).property ) << "</property>" << endl;
	}
	indent--;
	ts << makeIndent( indent ) << "</customwidget>" << endl;
    }


    indent--;
    ts << makeIndent( indent ) << "</customwidgets>" << endl;
}

void Resource::loadCustomWidgets( const TQDomElement &e, Resource *r )
{
    TQDomElement n = e.firstChild().toElement();
    while ( !n.isNull() ) {
	if ( n.tagName() == "customwidget" ) {
	    TQDomElement n2 = n.firstChild().toElement();
	    MetaDataBase::CustomWidget *w = new MetaDataBase::CustomWidget;
	    while ( !n2.isNull() ) {
		if ( n2.tagName() == "class" ) {
		    w->className = n2.firstChild().toText().data();
		} else if ( n2.tagName() == "header" ) {
		    w->includeFile = n2.firstChild().toText().data();
		    TQString s = n2.attribute( "location" );
		    if ( s != "local" )
			w->includePolicy = MetaDataBase::CustomWidget::Global;
		    else
			w->includePolicy = MetaDataBase::CustomWidget::Local;
		} else if ( n2.tagName() == "sizehint" ) {
		    TQDomElement n3 = n2.firstChild().toElement();
		    while ( !n3.isNull() ) {
			if ( n3.tagName() == "width" )
			    w->sizeHint.setWidth( n3.firstChild().toText().data().toInt() );
			else if ( n3.tagName() == "height" )
			    w->sizeHint.setHeight( n3.firstChild().toText().data().toInt() );
			n3 = n3.nextSibling().toElement();
		    }
		} else if ( n2.tagName() == "sizepolicy" ) {
		    TQDomElement n3 = n2.firstChild().toElement();
		    while ( !n3.isNull() ) {
			if ( n3.tagName() == "hordata" )
			    w->sizePolicy.setHorData( (TQSizePolicy::SizeType)n3.firstChild().toText().data().toInt() );
			else if ( n3.tagName() == "verdata" )
			    w->sizePolicy.setVerData( (TQSizePolicy::SizeType)n3.firstChild().toText().data().toInt() );
			else if ( n3.tagName() == "horstretch" )
			    w->sizePolicy.setHorStretch( n3.firstChild().toText().data().toInt() );
			else if ( n3.tagName() == "verstretch" )
			    w->sizePolicy.setVerStretch( n3.firstChild().toText().data().toInt() );
			n3 = n3.nextSibling().toElement();
		    }
		} else if ( n2.tagName() == "pixmap" ) {
		    TQPixmap pix;
		    if ( r ) {
			pix = r->loadPixmap( n2 );
		    } else {
			TQDomElement n3 = n2.firstChild().toElement();
			TQImage img;
			while ( !n3.isNull() ) {
			    if ( n3.tagName() == "data" ) {
				img = loadImageData( n3 );
			    }
			    n3 = n3.nextSibling().toElement();
			}
			pix.convertFromImage( img );
		    }
		    w->pixmap = new TQPixmap( pix );
		} else if ( n2.tagName() == "signal" ) {
		    w->lstSignals.append( n2.firstChild().toText().data().latin1() );
		} else if ( n2.tagName() == "container" ) {
		    w->isContainer = (bool)n2.firstChild().toText().data().toInt();
		} else if ( n2.tagName() == "slot" ) {
		    MetaDataBase::Function function;
		    function.function = n2.firstChild().toText().data().latin1();
		    function.access = n2.attribute( "access" );
		    function.type = "slot";
		    w->lstSlots.append( function );
		} else if ( n2.tagName() == "property" ) {
		    MetaDataBase::Property property;
		    property.property = n2.firstChild().toText().data().latin1();
		    property.type = n2.attribute( "type" );
		    w->lstProperties.append( property );
		}
		n2 = n2.nextSibling().toElement();
	    }
	    MetaDataBase::addCustomWidget( w );
	}
	n = n.nextSibling().toElement();
    }
}

void Resource::saveTabOrder( TQTextStream &ts, int indent )
{
    TQWidgetList l = MetaDataBase::tabOrder( toplevel );
    if ( l.isEmpty() )
	return;

    ts << makeIndent( indent ) << "<tabstops>" << endl;
    indent++;

    for ( TQWidget *w = l.first(); w; w = l.next() ) {
	if ( w->testWState( TQt::WState_ForceHide ) || knownNames.findIndex( w->name() ) == -1 )
	    continue;
	ts << makeIndent( indent ) << "<tabstop>" << w->name() << "</tabstop>" << endl;
    }

    indent--;
    ts << makeIndent( indent ) << "</tabstops>" << endl;
}

void Resource::loadTabOrder( const TQDomElement &e )
{
    TQWidget *last = 0;
    TQDomElement n = e.firstChild().toElement();
    TQWidgetList widgets;
    while ( !n.isNull() ) {
	if ( n.tagName() == "tabstop" ) {
	    TQString name = n.firstChild().toText().data();
	    if ( name.isEmpty() )
		continue;
	    TQObjectList *l = toplevel->queryList( 0, name, false );
	    if ( l ) {
		if ( l->first() ) {
		    TQWidget *w = (TQWidget*)l->first();
		    widgets.append( w );
		    if ( last )
			toplevel->setTabOrder( last, w );
		    last = w;
		}
		delete l;
	    }
	}
	n = n.nextSibling().toElement();
    }

    if ( !widgets.isEmpty() )
	MetaDataBase::setTabOrder( toplevel, widgets );
}

void Resource::saveMetaInfoBefore( TQTextStream &ts, int indent )
{
    MetaDataBase::MetaInfo info = MetaDataBase::metaInfo( formwindow );
    TQString cn;
    if ( info.classNameChanged && !info.className.isEmpty() )
	cn = info.className;
    else
	cn = formwindow->name();
    ts << makeIndent( indent ) << "<class>" << entitize( cn ) << "</class>" << endl;
    if ( !info.comment.isEmpty() )
	ts << makeIndent( indent ) << "<comment>" << entitize( info.comment ) << "</comment>" << endl;
    if ( !info.author.isEmpty() )
	ts << makeIndent( indent ) << "<author>" << entitize( info.author ) << "</author>" << endl;
}

void Resource::saveMetaInfoAfter( TQTextStream &ts, int indent )
{
    MetaDataBase::MetaInfo info = MetaDataBase::metaInfo( formwindow );
    if ( !langIface || formwindow->project()->isCpp() ) {
	TQValueList<MetaDataBase::Include> includes = MetaDataBase::includes( formwindow );
	TQString extensionInclude;
	bool needExtensionInclude = false;
	if ( langIface &&
	     formwindow->formFile()->hasFormCode() &&
	     formwindow->formFile()->codeFileState() != FormFile::Deleted ) {
	    extensionInclude = TQFileInfo( currFileName ).fileName() + langIface->formCodeExtension();
	    needExtensionInclude = true;
	}
	if ( !includes.isEmpty() || needExtensionInclude ) {
	    ts << makeIndent( indent ) << "<includes>" << endl;
	    indent++;

	    for ( TQValueList<MetaDataBase::Include>::Iterator it = includes.begin(); it != includes.end(); ++it ) {
		ts << makeIndent( indent ) << "<include location=\"" << (*it).location
		   << "\" impldecl=\"" << (*it).implDecl << "\">" << (*it).header << "</include>" << endl;
		if ( needExtensionInclude )
		    needExtensionInclude = (*it).header != extensionInclude;
	    }

	    if ( needExtensionInclude )
		ts << makeIndent( indent ) << "<include location=\"local\" impldecl=\"in implementation\">"
		   << extensionInclude << "</include>" << endl;
	    indent--;
	    ts << makeIndent( indent ) << "</includes>" << endl;
	}

	TQStringList forwards = MetaDataBase::forwards( formwindow );
	if ( !forwards.isEmpty() ) {
	    ts << makeIndent( indent ) << "<forwards>" << endl;
	    indent++;
	    for ( TQStringList::Iterator it2 = forwards.begin(); it2 != forwards.end(); ++it2 )
		ts << makeIndent( indent ) << "<forward>" << entitize( *it2 ) << "</forward>" << endl;
	    indent--;
	    ts << makeIndent( indent ) << "</forwards>" << endl;
	}
	TQValueList<MetaDataBase::Variable> varLst = MetaDataBase::variables( formwindow );
	if ( !varLst.isEmpty() ) {
	    ts << makeIndent( indent ) << "<variables>" << endl;
	    indent++;

	    TQValueList<MetaDataBase::Variable>::Iterator it = varLst.begin();
	    for ( ; it != varLst.end(); ++it ) {
		ts << makeIndent( indent ) << "<variable";
		if ( (*it).varAccess != "protected" )
		    ts << " access=\"" << (*it).varAccess << "\"";

		ts << ">" << entitize( (*it).varName ) << "</variable>" << endl;
	    }
	    indent--;
	    ts << makeIndent( indent ) << "</variables>" << endl;
	}
	TQStringList sigs = MetaDataBase::signalList( formwindow );
	if ( !sigs.isEmpty() ) {
	    ts << makeIndent( indent ) << "<signals>" << endl;
	    indent++;
	    for ( TQStringList::Iterator it3 = sigs.begin(); it3 != sigs.end(); ++it3 )
		ts << makeIndent( indent ) << "<signal>" << entitize( *it3 ) << "</signal>" << endl;
	    indent--;
	    ts << makeIndent( indent ) << "</signals>" << endl;
	}

	TQValueList<MetaDataBase::Function> slotList = MetaDataBase::slotList( formwindow );
	if ( !slotList.isEmpty() ) {
	    ts << makeIndent( indent ) << "<slots>" << endl;
	    indent++;
	    TQString lang = formwindow->project()->language();
	    TQValueList<MetaDataBase::Function>::Iterator it = slotList.begin();
	    for ( ; it != slotList.end(); ++it ) {
		MetaDataBase::Function function = *it;
		ts << makeIndent( indent ) << "<slot";
		if ( function.access != "public" )
		    ts << " access=\"" << function.access << "\"";
		if ( function.specifier != "virtual" )
		    ts << " specifier=\"" << function.specifier << "\"";
		if ( function.language != "C++" )
		    ts << " language=\"" << function.language<< "\"";
		if ( function.returnType != "void" )
		    ts << " returnType=\"" << entitize( function.returnType ) << "\"";
		ts << ">" << entitize( function.function ) << "</slot>" << endl;
	    }
	    indent--;
	    ts << makeIndent( indent ) << "</slots>" << endl;
	}

	TQValueList<MetaDataBase::Function> functionList = MetaDataBase::functionList( formwindow, true );
	if ( !functionList.isEmpty() ) {
	    ts << makeIndent( indent ) << "<functions>" << endl;
	    indent++;
	    TQString lang = formwindow->project()->language();
	    TQValueList<MetaDataBase::Function>::Iterator it = functionList.begin();
	    for ( ; it != functionList.end(); ++it ) {
		MetaDataBase::Function function = *it;
		ts << makeIndent( indent ) << "<function";
		if ( function.access != "public" )
		    ts << " access=\"" << function.access << "\"";
		if ( function.specifier != "virtual" )
		    ts << " specifier=\"" << function.specifier << "\"";
		if ( function.language != "C++" )
		    ts << " language=\"" << function.language<< "\"";
		if ( function.returnType != "void" )
		    ts << " returnType=\"" << entitize( function.returnType ) << "\"";
		ts << ">" << entitize( function.function ) << "</function>" << endl;
	    }
	    indent--;
	    ts << makeIndent( indent ) << "</functions>" << endl;
	}
    }

    if ( formwindow && formwindow->savePixmapInline() )
	;
    else if ( formwindow && formwindow->savePixmapInProject() )
	ts << makeIndent( indent ) << "<pixmapinproject/>" << endl;
    else
	ts << makeIndent( indent ) << "<pixmapfunction>" << formwindow->pixmapLoaderFunction() << "</pixmapfunction>" << endl;
    if ( !( exportMacro = MetaDataBase::exportMacro( formwindow->mainContainer() ) ).isEmpty() )
	ts << makeIndent( indent ) << "<exportmacro>" << exportMacro << "</exportmacro>" << endl;
    if ( formwindow ) {
	ts << makeIndent( indent ) << "<layoutdefaults spacing=\"" << formwindow->layoutDefaultSpacing()
	   << "\" margin=\"" << formwindow->layoutDefaultMargin() << "\"/>" << endl;
	if ( formwindow->hasLayoutFunctions() ) {
	    TQString s = "";
	    TQString m = "";
	    if ( !formwindow->spacingFunction().isEmpty() )
		s = TQString( " spacing=\"%1\"" ).arg( formwindow->spacingFunction() );
	    if ( !formwindow->marginFunction().isEmpty() )
		m = TQString( " margin=\"%1\"" ).arg( formwindow->marginFunction() );
	    ts << makeIndent( indent ) << "<layoutfunctions" << s << m << "/>" << endl;
	}
    }
}

TQColorGroup Resource::loadColorGroup( const TQDomElement &e )
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

void Resource::saveChildActions( TQAction *a, TQTextStream &ts, int indent )
{
    if ( !a->children() )
	return;
    TQObjectListIt it( *a->children() );
    while ( it.current() ) {
	TQObject *o = it.current();
	++it;
	if ( !::tqt_cast<TQAction*>(o) )
	    continue;
	TQAction *ac = (TQAction*)o;
	bool isGroup = ::tqt_cast<TQActionGroup*>(ac);
	if ( isGroup )
	    ts << makeIndent( indent ) << "<actiongroup>" << endl;
	else
	    ts << makeIndent( indent ) << "<action>" << endl;
	indent++;
	saveObjectProperties( ac, ts, indent );
	indent--;
	if ( isGroup ) {
	    indent++;
	    saveChildActions( ac, ts, indent );
	    indent--;
	}
	if ( isGroup )
	    ts << makeIndent( indent ) << "</actiongroup>" << endl;
	else
	    ts << makeIndent( indent ) << "</action>" << endl;
    }
}

void Resource::saveActions( const TQPtrList<TQAction> &actions, TQTextStream &ts, int indent )
{
    if ( actions.isEmpty() )
	return;
    ts << makeIndent( indent ) << "<actions>" << endl;
    indent++;
    TQPtrListIterator<TQAction> it( actions );
    while ( it.current() ) {
	TQAction *a = it.current();
	bool isGroup = ::tqt_cast<TQActionGroup*>(a);
	if ( isGroup )
	    ts << makeIndent( indent ) << "<actiongroup>" << endl;
	else
	    ts << makeIndent( indent ) << "<action>" << endl;
	indent++;
	saveObjectProperties( a, ts, indent );
	indent--;
	if ( isGroup ) {
	    indent++;
	    saveChildActions( a, ts, indent );
	    indent--;
	}
	if ( isGroup )
	    ts << makeIndent( indent ) << "</actiongroup>" << endl;
	else
	    ts << makeIndent( indent ) << "</action>" << endl;
	++it;
    }
    indent--;
    ts << makeIndent( indent ) << "</actions>" << endl;
}

void Resource::loadChildAction( TQObject *parent, const TQDomElement &e )
{
    TQDomElement n = e;
    TQAction *a = 0;
    if ( n.tagName() == "action" ) {
	a = new TQDesignerAction( parent );
	MetaDataBase::addEntry( a );
	TQDomElement n2 = n.firstChild().toElement();
	bool hasMenuText = false;
	while ( !n2.isNull() ) {
	    if ( n2.tagName() == "property" ) {
		TQDomElement n3(n2); // don't modify n2
		TQString prop = n3.attribute( "name" );
		if (prop == "menuText")
		    hasMenuText = true;
		TQDomElement value(n3.firstChild().toElement());
		setObjectProperty( a, prop, value );
		if (!hasMenuText && uiFileVersion < "3.3" && prop == "text")
		    setObjectProperty( a, "menuText", value );
	    }
	    n2 = n2.nextSibling().toElement();
	}
	if ( !::tqt_cast<TQAction*>(parent) )
	    formwindow->actionList().append( a );
    } else if ( n.tagName() == "actiongroup" ) {
	a = new TQDesignerActionGroup( parent );
	MetaDataBase::addEntry( a );
	TQDomElement n2 = n.firstChild().toElement();
	bool hasMenuText = false;
	while ( !n2.isNull() ) {
	    if ( n2.tagName() == "property" ) {
		TQDomElement n3(n2); // don't modify n2
		TQString prop = n3.attribute( "name" );
		if (prop == "menuText")
		    hasMenuText = true;
		TQDomElement value = n3.firstChild().toElement();
		setObjectProperty( a, prop, value );
		if (!hasMenuText && uiFileVersion < "3.3" && prop == "text")
		    setObjectProperty( a, "menuText", value );
	    } else if ( n2.tagName() == "action" ||
			n2.tagName() == "actiongroup" ) {
		loadChildAction( a, n2 );
	    }
	    n2 = n2.nextSibling().toElement();
	}
	if ( !::tqt_cast<TQAction*>(parent) )
	    formwindow->actionList().append( a );
    }
}

void Resource::loadActions( const TQDomElement &e )
{
    TQDomElement n = e.firstChild().toElement();
    while ( !n.isNull() ) {
	if ( n.tagName() == "action" ) {
	    loadChildAction( formwindow, n );
	} else if ( n.tagName() == "actiongroup" ) {
	    loadChildAction( formwindow, n );
	}
	n = n.nextSibling().toElement();
    }
}

void Resource::saveToolBars( TQMainWindow *mw, TQTextStream &ts, int indent )
{
    ts << makeIndent( indent ) << "<toolbars>" << endl;
    indent++;

    TQPtrList<TQToolBar> tbList;
    for ( int i = 0; i <= (int)TQt::DockMinimized; ++i ) {
	tbList = mw->toolBars( (TQt::Dock)i );
	if ( tbList.isEmpty() )
	    continue;
	for ( TQToolBar *tb = tbList.first(); tb; tb = tbList.next() ) {
	    if ( tb->isHidden() )
		continue;
	    ts << makeIndent( indent ) << "<toolbar dock=\"" << i << "\">" << endl;
	    indent++;
	    saveObjectProperties( tb, ts, indent );
	    TQPtrList<TQAction> actionList = ( (TQDesignerToolBar*)tb )->insertedActions();
	    for ( TQAction *a = actionList.first(); a; a = actionList.next() ) {
		if ( ::tqt_cast<TQSeparatorAction*>(a) ) {
		    ts << makeIndent( indent ) << "<separator/>" << endl;
		} else {
		    if ( ::tqt_cast<TQDesignerAction*>(a) && !( (TQDesignerAction*)a )->supportsMenu() ) {
			TQWidget *w = ( (TQDesignerAction*)a )->widget();
			ts <<  makeIndent( indent ) << "<widget class=\""
			   << WidgetFactory::classNameOf( w ) << "\">" << endl;
			indent++;
			const char *className = WidgetFactory::classNameOf( w );
			if ( w->isA( "CustomWidget" ) )
			    usedCustomWidgets << TQString( className );
			if ( WidgetFactory::hasItems( WidgetDatabase::idFromClassName( WidgetFactory::classNameOf( w ) ), w ) )
			    saveItems( w, ts, indent );
			saveObjectProperties( w, ts, indent );
			indent--;
			ts << makeIndent( indent ) << "</widget>" << endl;
		    } else {
			ts << makeIndent( indent ) << "<action name=\"" << a->name() << "\"/>" << endl;
		    }
		}
	    }
	    indent--;
	    ts << makeIndent( indent ) << "</toolbar>" << endl;
	}
    }
    indent--;
    ts << makeIndent( indent ) << "</toolbars>" << endl;
}

void Resource::saveMenuBar( TQMainWindow *mw, TQTextStream &ts, int indent )
{
    MenuBarEditor *mb = (MenuBarEditor *)mw->child( 0, "MenuBarEditor" );
    if ( !mb )
	return;
    ts << makeIndent( indent ) << "<menubar>" << endl;
    indent++;
    MetaDataBase::setPropertyChanged( mb, "name", true ); // FIXME: remove
    saveObjectProperties( mb, ts, indent );

    for ( int i = 0; i < (int)mb->count(); ++i ) {
	MenuBarEditorItem *m = mb->item( i );
	if ( !m )
	    continue;
	if ( m->isSeparator() ) {
	    ts << makeIndent( indent ) << "<separator/>" << endl;
	} else {
	    ts << makeIndent( indent ) << "<item text=\"" << entitize( m->menuText() )
	       << "\" name=\"" << entitize( m->menu()->name() ) << "\">" << endl;
	    indent++;
	    savePopupMenu( m->menu(), mw, ts, indent );
	    indent--;
	    ts << makeIndent( indent ) << "</item>" << endl;
	}
    }
    indent--;
    ts << makeIndent( indent ) << "</menubar>" << endl;
}

void Resource::savePopupMenu( PopupMenuEditor *pm, TQMainWindow *mw, TQTextStream &ts, int indent )
{
    for ( PopupMenuEditorItem *i = pm->items()->first(); i; i = pm->items()->next() ) {
	TQAction *a = i->action();
	if ( ::tqt_cast<TQSeparatorAction*>(a) )
	    ts <<  makeIndent( indent ) << "<separator/>" << endl;
	else if ( ::tqt_cast<TQDesignerAction*>(a) )
	    ts <<  makeIndent( indent ) << "<action name=\"" << a->name() << "\"/>" << endl;
	else if  ( ::tqt_cast<TQDesignerActionGroup*>(a) )
	    ts <<  makeIndent( indent ) << "<actiongroup name=\"" << a->name() << "\"/>" << endl;
	PopupMenuEditor *s =  i->subMenu();
	if ( s && s->count() ) {
	    TQString n = s->name();
	    ts << makeIndent( indent ) << "<item text=\"" << entitize( a->menuText() )
	       << "\" name=\"" << entitize( n )
	       << "\" accel=\"" << entitize( a->accel() )
	       << "\">" << endl;
	    indent++;
	    savePopupMenu( s, mw, ts, indent );
	    indent--;
	    ts << makeIndent( indent ) << "</item>" << endl;
	}
    }
}

void Resource::loadToolBars( const TQDomElement &e )
{
    TQDomElement n = e.firstChild().toElement();
    TQMainWindow *mw = ( (TQMainWindow*)formwindow->mainContainer() );
    TQDesignerToolBar *tb = 0;
    while ( !n.isNull() ) {
	if ( n.tagName() == "toolbar" ) {
	    TQt::Dock dock = (TQt::Dock)n.attribute( "dock" ).toInt();
	    tb = new TQDesignerToolBar( mw, dock );
	    TQDomElement n2 = n.firstChild().toElement();
	    while ( !n2.isNull() ) {
		if ( n2.tagName() == "action" ) {
		    TQAction *a = formwindow->findAction( n2.attribute( "name" ) );
		    if ( a ) {
			a->addTo( tb );
			tb->addAction( a );
		    }
		} else if ( n2.tagName() == "separator" ) {
		    TQAction *a = new TQSeparatorAction( 0 );
		    a->addTo( tb );
		    tb->addAction( a );
		} else if ( n2.tagName() == "widget" ) {
		    TQWidget *w = (TQWidget*)createObject( n2, tb );
		    TQDesignerAction *a = new TQDesignerAction( w, tb );
		    a->addTo( tb );
		    tb->addAction( a );
		    tb->installEventFilters( w );
		} else if ( n2.tagName() == "property" ) {
		    setObjectProperty( tb, n2.attribute( "name" ), n2.firstChild().toElement() );
		}
		n2 = n2.nextSibling().toElement();
	    }
	}
	n = n.nextSibling().toElement();
    }
}

void Resource::loadMenuBar( const TQDomElement &e )
{
    TQDomElement n = e.firstChild().toElement();
    TQMainWindow *mw = (TQMainWindow*)formwindow->mainContainer();
    MenuBarEditor *mb = new MenuBarEditor( formwindow, mw );
    MetaDataBase::addEntry( mb );
    while ( !n.isNull() ) {
	if ( n.tagName() == "item" ) {
	    PopupMenuEditor * popup = new PopupMenuEditor( formwindow, mw );
	    loadPopupMenu( popup, n );
	    popup->setName( n.attribute( "name" ) );
	    mb->insertItem( n.attribute( "text" ), popup );
	    MetaDataBase::addEntry( popup );
	} else if ( n.tagName() == "property" ) {
	    setObjectProperty( mb, n.attribute( "name" ), n.firstChild().toElement() );
	} else if ( n.tagName() == "separator" ) {
	    mb->insertSeparator();
	}
	n = n.nextSibling().toElement();
    }
}

void Resource::loadPopupMenu( PopupMenuEditor *p, const TQDomElement &e )
{
    MetaDataBase::addEntry( p );
    TQDomElement n = e.firstChild().toElement();
    TQAction *a = 0;
    while ( !n.isNull() ) {
	if ( n.tagName() == "action" || n.tagName() == "actiongroup") {
	    a = formwindow->findAction( n.attribute( "name" ) );
	    if ( a )
		p->insert( a );
	}
	if ( n.tagName() == "item" ) {
	    PopupMenuEditorItem *i = p->at( p->find( a ) );
	    if ( i ) {
		TQString name = n.attribute( "name" );
		formwindow->unify( i, name, true );
		i->setName( name );
		MetaDataBase::addEntry( i );
		loadPopupMenu( i->subMenu(), n );
	    }
	} else if ( n.tagName() == "separator" ) {
	    a = new TQSeparatorAction( 0 );
	    p->insert( a );
	}
	n = n.nextSibling().toElement();
    }
}

bool Resource::saveFormCode( FormFile *formfile, LanguageInterface * /*langIface*/ )
{
    TQString lang = formfile->project()->language();
    if ( formfile->hasTempFileName() ||
	 formfile->code().isEmpty() ||
	 !formfile->hasFormCode() ||
	 !formfile->isModified(FormFile::WFormCode) )
	return true;  // There is no code to be saved.
    return saveCode( formfile->project()->makeAbsolute(formfile->codeFile()),
		     formfile->code() );
}

void Resource::loadExtraSource( FormFile *formfile, const TQString &currFileName,
				LanguageInterface *langIface, bool hasFunctions )
{
    TQString lang = "TQt Script";
    if ( MainWindow::self )
	lang = MainWindow::self->currProject()->language();
    LanguageInterface *iface = langIface;
    if ( hasFunctions || !iface )
	return;
    TQValueList<LanguageInterface::Function> functions;
    TQStringList forwards;
    TQStringList includesImpl;
    TQStringList includesDecl;
    TQStringList vars;
    TQValueList<LanguageInterface::Connection> connections;

    iface->loadFormCode( formfile->formName(),
			 currFileName + iface->formCodeExtension(),
			 functions,
			 vars,
			 connections );

    TQFile f( formfile->project()->makeAbsolute( formfile->codeFile() ) );
    TQString code;
    if ( f.open( IO_ReadOnly ) ) {
	TQTextStream ts( &f );
	code = ts.read();
    }
    formfile->setCode( code );

    if ( !MainWindow::self || !MainWindow::self->currProject()->isCpp() )
	MetaDataBase::setupConnections( formfile, connections );

    for ( TQValueList<LanguageInterface::Function>::Iterator fit = functions.begin();
	  fit != functions.end(); ++fit ) {

	if ( MetaDataBase::hasFunction( formfile->formWindow() ?
					(TQObject*)formfile->formWindow() :
					(TQObject*)formfile,
					(*fit).name.latin1() ) ) {
	    TQString access = (*fit).access;
	    if ( !MainWindow::self || !MainWindow::self->currProject()->isCpp() )
		MetaDataBase::changeFunction( formfile->formWindow() ?
					      (TQObject*)formfile->formWindow() :
					      (TQObject*)formfile,
					      (*fit).name,
					      (*fit).name,
					      TQString::null );
	} else {
	    TQString access = (*fit).access;
	    if ( access.isEmpty() )
		access = "protected";
	    TQString type = "function";
	    if ( (*fit).returnType == "void" )
		type = "slot";
	    MetaDataBase::addFunction( formfile->formWindow() ?
				       (TQObject*)formfile->formWindow() :
				       (TQObject*)formfile,
				       (*fit).name.latin1(), "virtual", (*fit).access,
				       type, lang, (*fit).returnType );
	}
    }
}
