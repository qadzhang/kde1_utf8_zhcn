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
#include "command.h"
#include "formwindow.h"
#include "widgetfactory.h"
#include "propertyeditor.h"
#include "metadatabase.h"
#include <widgetdatabase.h>
#include "mainwindow.h"
#include "hierarchyview.h"
#include "workspace.h"
#include "actioneditorimpl.h"
#include "actiondnd.h"
#include "formfile.h"
#include "../interfaces/languageinterface.h"
#include "menubareditor.h"
#include "popupmenueditor.h"

#include <ntqfeatures.h>
#include <ntqwidget.h>
#include <ntqmetaobject.h>
#include <ntqapplication.h>
#include <ntqlayout.h>
#include <ntqmessagebox.h>
#include <ntqlistbox.h>
#include <ntqiconview.h>
#include <ntqtextedit.h>
#include <ntqptrstack.h>
#include <ntqheader.h>
#include <ntqsplitter.h>
#ifndef TQT_NO_TABLE
#include <ntqtable.h>
#endif
#include <ntqaction.h>

CommandHistory::CommandHistory( int s )
    : current( -1 ), steps( s ), savedAt( -1 )
{
    history.setAutoDelete( true );
    modified = false;
    compressedCommand = 0;
}

void CommandHistory::addCommand( Command *cmd, bool tryCompress )
{
    if ( tryCompress ) {
	if ( !compressedCommand ||
	     compressedCommand->type() != cmd->type() ||
	     !compressedCommand->canMerge( cmd ) ) {
	    checkCompressedCommand();
	    compressedCommand = 0;
	}

	if ( compressedCommand ) {
	    compressedCommand->merge( cmd );
	    modified = true;
	    modificationChanged( modified );
	    return;
	}
	compressedCommand = cmd;
    } else {
	checkCompressedCommand();
    }

    if ( current < (int)history.count() - 1 ) {
	if ( current < savedAt )
	    savedAt = -2;

	TQPtrList<Command> commands;
	commands.setAutoDelete( false );

	for( int i = 0; i <= current; ++i ) {
	    commands.insert( i, history.at( 0 ) );
	    history.take( 0 );
	}

	commands.append( cmd );
	history.clear();
	history = commands;
	history.setAutoDelete( true );
    } else {
	history.append( cmd );
    }

    if ( (int)history.count() > steps ) {
	savedAt--;
	history.removeFirst();
    } else {
	++current;
    }

    emitUndoRedo();
    modified = true;
    modificationChanged( modified );
}

void CommandHistory::undo()
{
    checkCompressedCommand();
    compressedCommand = 0;
    if ( current > -1 ) {
	history.at( current )->unexecute();
	--current;
    }
    emitUndoRedo();
    modified = savedAt != current;
    modificationChanged( modified );
}

void CommandHistory::redo()
{
    checkCompressedCommand();
    compressedCommand = 0;
    if ( current > -1 ) {
	if ( current < (int)history.count() - 1 ) {
	    ++current;
	    history.at( current )->execute();
	}
    } else {
	if ( history.count() > 0 ) {
	    ++current;
	    history.at( current )->execute();
	}
    }
    emitUndoRedo();
    modified = savedAt != current;
    modificationChanged( modified );
}

void CommandHistory::emitUndoRedo()
{
    Command *undoCmd = 0;
    Command *redoCmd = 0;

    if ( current >= 0 && current < (int)history.count() )
	undoCmd = history.at( current );
    if ( current + 1 >= 0 && current + 1 < (int)history.count() )
	redoCmd = history.at( current + 1 );

    bool ua = (undoCmd != 0);
    TQString uc;
    if ( ua )
	uc = undoCmd->name();
    bool ra = (redoCmd != 0);
    TQString rc;
    if ( ra )
	rc = redoCmd->name();
    emit undoRedoChanged( ua, ra, uc, rc );
}

void CommandHistory::setModified( bool m )
{
    modified = m;
    if ( !modified )
	savedAt = current;
    modificationChanged( modified );
}

bool CommandHistory::isModified() const
{
    return modified;
}

void CommandHistory::checkCompressedCommand()
{
    if ( compressedCommand && compressedCommand->type() == Command::SetProperty ) {
	Command *c = compressedCommand;
	compressedCommand = 0;
	if ( !( (SetPropertyCommand*)c )->checkProperty() ) {
	    history.remove( current );
	    --current;
	    emitUndoRedo();
	}
    }
}

// ------------------------------------------------------------

Command::Command( const TQString &n, FormWindow *fw )
    : cmdName( n ), formWin( fw )
{
}

Command::~Command()
{
}

TQString Command::name() const
{
    return cmdName;
}

FormWindow *Command::formWindow() const
{
    return formWin;
}

void Command::merge( Command * )
{
}

bool Command::canMerge( Command * )
{
    return false;
}

// ------------------------------------------------------------

ResizeCommand::ResizeCommand( const TQString &n, FormWindow *fw,
			      TQWidget *w, const TQRect &oldr, const TQRect &nr )
    : Command( n, fw ), widget( w ), oldRect( oldr ), newRect( nr )
{
}

void ResizeCommand::execute()
{
    widget->setGeometry( newRect );
    formWindow()->updateSelection( widget );
    formWindow()->emitUpdateProperties( widget );
    if ( WidgetFactory::layoutType( widget ) != WidgetFactory::NoLayout )
	formWindow()->updateChildSelections( widget );
}

void ResizeCommand::unexecute()
{
    widget->setGeometry( oldRect );
    formWindow()->updateSelection( widget );
    formWindow()->emitUpdateProperties( widget );
    if ( WidgetFactory::layoutType( widget ) != WidgetFactory::NoLayout )
	formWindow()->updateChildSelections( widget );
}

// ------------------------------------------------------------

InsertCommand::InsertCommand( const TQString &n, FormWindow *fw,
			      TQWidget *w, const TQRect &g )
    : Command( n, fw ), widget( w ), geometry( g )
{
}

void InsertCommand::execute()
{
    if ( geometry.size() == TQSize( 0, 0 ) ) {
	widget->move( geometry.topLeft() );
	widget->adjustSize();
    } else {
	TQSize s = geometry.size().expandedTo( widget->minimumSize() );
	s = s.expandedTo( widget->minimumSizeHint() );
	TQRect r( geometry.topLeft(), s );
	widget->setGeometry( r );
    }
    widget->show();
    formWindow()->widgets()->insert( widget, widget );
    formWindow()->clearSelection( false );
    formWindow()->selectWidget( widget );
    formWindow()->mainWindow()->objectHierarchy()->widgetInserted( widget );
}

void InsertCommand::unexecute()
{
    widget->hide();
    formWindow()->selectWidget( widget, false );
    formWindow()->widgets()->remove( widget );
    formWindow()->mainWindow()->objectHierarchy()->widgetRemoved( widget );
 }

// ------------------------------------------------------------

MoveCommand::MoveCommand( const TQString &n, FormWindow *fw,
			  const TQWidgetList &w,
			  const TQValueList<TQPoint> op,
			  const TQValueList<TQPoint> np,
			  TQWidget *opr, TQWidget *npr )
    : Command( n, fw ), widgets( w ), oldPos( op ), newPos( np ),
      oldParent( opr ), newParent( npr )
{
    widgets.setAutoDelete( false );
}

void MoveCommand::merge( Command *c )
{
    MoveCommand *cmd = (MoveCommand*)c;
    newPos = cmd->newPos;
}

bool MoveCommand::canMerge( Command *c )
{
    MoveCommand *cmd = (MoveCommand*)c;
    return widgets == cmd->widgets;
}


void MoveCommand::execute()
{
    for ( TQWidget *w = widgets.first(); w; w = widgets.next() ) {
	if ( !w->parentWidget() || WidgetFactory::layoutType( w->parentWidget() ) == WidgetFactory::NoLayout ) {
	    if ( newParent && oldParent && newParent != oldParent ) {
		TQPoint pos = newParent->mapFromGlobal( w->mapToGlobal( TQPoint( 0,0 ) ) );
		w->reparent( newParent, pos, true );
		formWindow()->raiseSelection( w );
		formWindow()->raiseChildSelections( w );
		formWindow()->widgetChanged( w );
		formWindow()->mainWindow()->objectHierarchy()->widgetRemoved( w );
		formWindow()->mainWindow()->objectHierarchy()->widgetInserted( w );
	    }
	    w->move( newPos[ widgets.at() ] );
	}
	formWindow()->updateSelection( w );
	formWindow()->updateChildSelections( w );
	formWindow()->emitUpdateProperties( w );
    }
}

void MoveCommand::unexecute()
{
    for ( TQWidget *w = widgets.first(); w; w = widgets.next() ) {
	if ( !w->parentWidget() || WidgetFactory::layoutType( w->parentWidget() ) == WidgetFactory::NoLayout ) {
	    if ( newParent && oldParent && newParent != oldParent ) {
		TQPoint pos = oldParent->mapFromGlobal( w->mapToGlobal( TQPoint( 0,0 ) ) );
		w->reparent( oldParent, pos, true );
		formWindow()->raiseSelection( w );
		formWindow()->raiseChildSelections( w );
		formWindow()->widgetChanged( w );
		formWindow()->mainWindow()->objectHierarchy()->widgetRemoved( w );
		formWindow()->mainWindow()->objectHierarchy()->widgetInserted( w );
	    }
	    w->move( oldPos[ widgets.at() ] );
	}
	formWindow()->updateSelection( w );
	formWindow()->updateChildSelections( w );
	formWindow()->emitUpdateProperties( w );
    }
}

// ------------------------------------------------------------

DeleteCommand::DeleteCommand( const TQString &n, FormWindow *fw,
			      const TQWidgetList &wl )
    : Command( n, fw ), widgets( wl )
{
    widgets.setAutoDelete( false );
    TQWidgetList copyOfWidgets = widgets;
    copyOfWidgets.setAutoDelete(false);
    
    // Include the children of the selected items when deleting
    for ( TQWidget *w = widgets.first(); w; w = widgets.next() ) {
	TQObjectList *children = w->queryList( "TQWidget" );
	for ( TQWidget *c = (TQWidget *)children->first(); c; c = (TQWidget *)children->next() ) {
	    if ( copyOfWidgets.find( c ) == -1 && formWindow()->widgets()->find( c ) ) {
		widgets.insert(widgets.at() + 1, c);
		widgets.prev();
		copyOfWidgets.append(c);
	    }
	}
	delete children;
    }
}

void DeleteCommand::execute()
{
    formWindow()->setPropertyShowingBlocked( true );
    connections.clear();
    for ( TQWidget *w = widgets.first(); w; w = widgets.next() ) {
	w->hide();
	TQString s = w->name();
	s.prepend( "qt_dead_widget_" );
	w->setName( s );
	formWindow()->selectWidget( w, false );
	formWindow()->widgets()->remove( w );
	TQValueList<MetaDataBase::Connection> conns = MetaDataBase::connections( formWindow(), w );
	connections.insert( w, conns );
	TQValueList<MetaDataBase::Connection>::Iterator it = conns.begin();
	for ( ; it != conns.end(); ++it ) {
	    MetaDataBase::removeConnection( formWindow(), (*it).sender,
					    (*it).signal, (*it).receiver, (*it).slot );
	}
    }
    formWindow()->setPropertyShowingBlocked( false );
    formWindow()->emitShowProperties();
    formWindow()->mainWindow()->objectHierarchy()->widgetsRemoved( widgets );

}

void DeleteCommand::unexecute()
{
    formWindow()->setPropertyShowingBlocked( true );
    formWindow()->clearSelection( false );
    for ( TQWidget *w = widgets.first(); w; w = widgets.next() ) {
	w->show();
	TQString s = w->name();
	s.remove( 0, TQString( "qt_dead_widget_" ).length() );
	w->setName( s );
	formWindow()->widgets()->insert( w, w );
	formWindow()->selectWidget( w );
	TQValueList<MetaDataBase::Connection> conns = *connections.find( w );
	TQValueList<MetaDataBase::Connection>::Iterator it = conns.begin();
	for ( ; it != conns.end(); ++it ) {
	    MetaDataBase::addConnection( formWindow(), (*it).sender,
					 (*it).signal, (*it).receiver, (*it).slot );
	}
    }
    formWindow()->setPropertyShowingBlocked( false );
    formWindow()->emitShowProperties();
    formWindow()->mainWindow()->objectHierarchy()->widgetsInserted( widgets );
}

// ------------------------------------------------------------

SetPropertyCommand::SetPropertyCommand( const TQString &n, FormWindow *fw,
					TQObject *w, PropertyEditor *e,
					const TQString &pn, const TQVariant &ov,
					const TQVariant &nv, const TQString &ncut,
					const TQString &ocut, bool reset )
    : Command( n, fw ), widget( w ), editor( e ), propName( pn ),
      oldValue( ov ), newValue( nv ), oldCurrentItemText( ocut ), newCurrentItemText( ncut ),
      wasChanged( true ), isResetCommand( reset )
{
    wasChanged = MetaDataBase::isPropertyChanged( w, propName );
    if ( oldCurrentItemText.isNull() )
	oldCurrentItemText = "";
    if ( newCurrentItemText.isNull() )
	newCurrentItemText = "";
}


void SetPropertyCommand::execute()
{
    if ( !wasChanged )
	MetaDataBase::setPropertyChanged( widget, propName, true );
    if ( isResetCommand ) {
	MetaDataBase::setPropertyChanged( widget, propName, false );
	if ( WidgetFactory::resetProperty( widget, propName ) ) {
	    if ( !formWindow()->isWidgetSelected( widget ) && formWindow() != (TQObject *)widget )
		formWindow()->selectWidget( (TQObject *)widget );
	    if ( editor->widget() != widget )
		editor->setWidget( widget, formWindow() );
	    editor->propertyList()->setCurrentProperty( propName );
	    PropertyItem *i = (PropertyItem*)editor->propertyList()->currentItem();
	    if ( !i )
		return;
	    i->setValue( widget->property( propName ) );
	    i->setChanged( false );
	    editor->refetchData();
	    editor->emitWidgetChanged();
	    return;
	}
    }
    setProperty( newValue, newCurrentItemText );
}

void SetPropertyCommand::unexecute()
{
    if ( !wasChanged )
	MetaDataBase::setPropertyChanged( widget, propName, false );
    if ( isResetCommand )
	MetaDataBase::setPropertyChanged( widget, propName, true );
    setProperty( oldValue, oldCurrentItemText );
}

bool SetPropertyCommand::canMerge( Command *c )
{
    SetPropertyCommand *cmd = (SetPropertyCommand*)c;
    if ( !widget )
	return false;
    const TQMetaProperty *p =
	widget->metaObject()->property( widget->metaObject()->findProperty( propName, true ), true );
    if ( !p ) {
	if ( propName == "toolTip" || propName == "whatsThis" )
	    return true;
	if ( ::tqt_cast<CustomWidget*>((TQObject *)widget) ) {
	    MetaDataBase::CustomWidget *cw = ((CustomWidget*)(TQObject*)widget)->customWidget();
	    if ( !cw )
		return false;
	    for ( TQValueList<MetaDataBase::Property>::Iterator it = cw->lstProperties.begin(); it != cw->lstProperties.end(); ++it ) {
		if ( TQString( (*it ).property ) == propName ) {
		    if ( (*it).type == "String" || (*it).type == "CString" || (*it).type == "Int" || (*it).type == "UInt" )
			return true;
		}
	    }
	}
	return false;
    }
    TQVariant::Type t = TQVariant::nameToType( p->type() );
    return ( ( cmd->propName == propName &&
	     t == TQVariant::String ) || t == TQVariant::CString || t == TQVariant::Int || t == TQVariant::UInt );
}

void SetPropertyCommand::merge( Command *c )
{
    SetPropertyCommand *cmd = (SetPropertyCommand*)c;
    newValue = cmd->newValue;
    newCurrentItemText = cmd->newCurrentItemText;
}

bool SetPropertyCommand::checkProperty()
{
    if ( propName == "name" /*|| propName == "itemName"*/ ) { // ### fix that
	TQString s = newValue.toString();
	if ( !formWindow()->unify( widget, s, false ) ) {
	    TQMessageBox::information( formWindow()->mainWindow(),
				      FormWindow::tr( "Set 'name' property" ),
				      FormWindow::tr( "The name of a widget must be unique.\n"
						      "'%1' is already used in form '%2',\n"
						      "so the name has been reverted to '%3'." ).
				      arg( newValue.toString() ).
				      arg( formWindow()->name() ).
				      arg( oldValue.toString() ));
	    setProperty( oldValue, oldCurrentItemText, false );
	    return false;
	}
	if ( s.isEmpty() ) {
	    TQMessageBox::information( formWindow()->mainWindow(),
				      FormWindow::tr( "Set 'name' property" ),
				      FormWindow::tr( "The name of a widget must not be null.\n"
						      "The name has been reverted to '%1'." ).
				      arg( oldValue.toString() ));
	    setProperty( oldValue, oldCurrentItemText, false );
	    return false;
	}

	if ( ::tqt_cast<FormWindow*>(widget->parent()) )
	    formWindow()->mainWindow()->formNameChanged( (FormWindow*)((TQWidget*)(TQObject*)widget)->parentWidget() );
    }
    return true;
}

void SetPropertyCommand::setProperty( const TQVariant &v, const TQString &currentItemText, bool select )
{
    if ( !widget )
        return;

    if ( !formWindow()->isWidgetSelected( widget ) && !formWindow()->isMainContainer( widget ) && select )
	formWindow()->selectWidget( widget );
    if ( editor->widget() != widget && select )
	editor->setWidget( widget, formWindow() );
    if ( select )
	editor->propertyList()->setCurrentProperty( propName );
    const TQMetaProperty *p =
	widget->metaObject()->property( widget->metaObject()->findProperty( propName, true ), true );
    if ( !p ) {
	if ( propName == "hAlign" ) {
	    p = widget->metaObject()->property( widget->metaObject()->findProperty( "alignment", true ), true );
	    int align = widget->property( "alignment" ).toInt();
	    align &= ~( AlignHorizontal_Mask );
	    align |= p->keyToValue( currentItemText );
	    widget->setProperty( "alignment", TQVariant( align ) );
	} else if ( propName == "vAlign" ) {
	    p = widget->metaObject()->property( widget->metaObject()->findProperty( "alignment", true ), true );
	    int align = widget->property( "alignment" ).toInt();
	    align &= ~( AlignVertical_Mask );
	    align |= p->keyToValue( currentItemText );
	    widget->setProperty( "alignment", TQVariant( align ) );
	} else if ( propName == "wordwrap" ) {
	    int align = widget->property( "alignment" ).toInt();
	    align &= ~WordBreak;
	    if ( v.toBool() )
		align |= WordBreak;
	    widget->setProperty( "alignment", TQVariant( align ) );
	} else if ( propName == "layoutSpacing" ) {
	    TQVariant val = v;
	    if ( val.toString() == "default" )
		val = -1;
	    MetaDataBase::setSpacing( WidgetFactory::containerOfWidget( (TQWidget*)editor->widget() ), val.toInt() );
	} else if ( propName == "layoutMargin" ) {
	    TQVariant val = v;
	    if ( val.toString() == "default" )
		val = -1;
	    MetaDataBase::setMargin( WidgetFactory::containerOfWidget( (TQWidget*)editor->widget() ), val.toInt() );
	} else if ( propName == "resizeMode" ) {
	    MetaDataBase::setResizeMode( WidgetFactory::containerOfWidget( (TQWidget*)editor->widget() ), currentItemText );
	} else if ( propName == "toolTip" || propName == "whatsThis" || propName == "database" || propName == "frameworkCode" ) {
	    MetaDataBase::setFakeProperty( editor->widget(), propName, v );
	} else if ( ::tqt_cast<CustomWidget*>(editor->widget()) ) {
	    MetaDataBase::CustomWidget *cw = ((CustomWidget *)(TQObject *)widget)->customWidget();
	    if ( cw ) {
		MetaDataBase::setFakeProperty( editor->widget(), propName, v );
	    }
	}
	editor->refetchData();
	editor->emitWidgetChanged();
	( ( PropertyItem* )editor->propertyList()->currentItem() )->setChanged( MetaDataBase::isPropertyChanged( widget, propName ) );
#ifndef TQT_NO_SQL
	if ( propName == "database" ) {
	    formWindow()->mainWindow()->objectHierarchy()->databasePropertyChanged( (TQWidget*)((TQObject *)widget), MetaDataBase::fakeProperty( widget, "database" ).toStringList() );
	}
#endif
	return;
    }

    if ( p->isSetType() ) {
	TQStrList strlst;
	TQStringList lst = TQStringList::split( "|", currentItemText );
	TQValueListConstIterator<TQString> it = lst.begin();
	for ( ; it != lst.end(); ++it )
	    strlst.append( (*it).latin1() );
	widget->setProperty( propName, p->keysToValue( strlst ) );
    } else if ( p->isEnumType() ) {
	widget->setProperty( propName, p->keyToValue( currentItemText ) );
    } else if ( qstrcmp( p->name(), "buddy" ) == 0 ) {
	widget->setProperty( propName, currentItemText );
    } else {
	TQVariant ov;
	if ( propName == "name" || propName == "itemName" )
	    ov = widget->property( propName );
	int oldSerNum = -1;
	if ( v.type() == TQVariant::Pixmap )
	    oldSerNum = v.toPixmap().serialNumber();
	widget->setProperty( propName, v );
	if ( oldSerNum != -1 && oldSerNum != widget->property( propName ).toPixmap().serialNumber() )
	    MetaDataBase::setPixmapKey( formWindow(),
					widget->property( propName ).toPixmap().serialNumber(),
					MetaDataBase::pixmapKey( formWindow(), oldSerNum ) );
	if ( propName == "cursor" ) {
	    MetaDataBase::setCursor( (TQWidget*)((TQObject *)widget), v.toCursor() );
	}
	if ( propName == "name" && widget->isWidgetType() ) {
	    formWindow()->mainWindow()->objectHierarchy()->namePropertyChanged( ((TQWidget*)(TQObject *)widget), ov );
	    if ( formWindow()->isMainContainer( widget ) )
		formWindow()->setName( v.toCString() );
	}
	if ( propName == "name" && ::tqt_cast<TQAction*>((TQObject *)widget) && ::tqt_cast<TQMainWindow*>(formWindow()->mainContainer()) ) {
	    formWindow()->mainWindow()->actioneditor()->updateActionName( (TQAction*)(TQObject *)widget );
	}
	if ( propName == "iconSet" && ::tqt_cast<TQAction*>((TQObject *)widget) && ::tqt_cast<TQMainWindow*>(formWindow()->mainContainer()) ) {
	    formWindow()->mainWindow()->actioneditor()->updateActionIcon( (TQAction*)(TQObject *)widget );
	}
	if ( propName == "caption" ) {
	    if ( formWindow()->isMainContainer( widget ) )
		formWindow()->setCaption( v.toString() );
	}
	if ( propName == "icon" ) {
	    if ( formWindow()->isMainContainer( widget ) )
		formWindow()->setIcon( v.toPixmap() );
	}
    }
    editor->refetchData();
    if ( editor->propertyList()->currentItem() && select ) {
	( ( PropertyItem* )editor->propertyList()->currentItem() )->showEditor();
	( ( PropertyItem* )editor->propertyList()->currentItem() )->setChanged( MetaDataBase::isPropertyChanged( widget, propName ) );
    }
    editor->emitWidgetChanged();
    formWindow()->killAccels( widget );
}

// ------------------------------------------------------------

LayoutHorizontalCommand::LayoutHorizontalCommand( const TQString &n, FormWindow *fw,
						  TQWidget *parent, TQWidget *layoutBase,
						  const TQWidgetList &wl )
    : Command( n, fw ), layout( wl, parent, fw, layoutBase )
{
}

void LayoutHorizontalCommand::execute()
{
    formWindow()->clearSelection( false );
    layout.doLayout();
    formWindow()->mainWindow()->objectHierarchy()->rebuild();
}

void LayoutHorizontalCommand::unexecute()
{
    formWindow()->clearSelection( false );
    layout.undoLayout();
    formWindow()->mainWindow()->objectHierarchy()->rebuild();
}

// ------------------------------------------------------------

LayoutHorizontalSplitCommand::LayoutHorizontalSplitCommand( const TQString &n, FormWindow *fw,
							    TQWidget *parent, TQWidget *layoutBase,
							    const TQWidgetList &wl )
    : Command( n, fw ), layout( wl, parent, fw, layoutBase, true, true )
{
}

void LayoutHorizontalSplitCommand::execute()
{
    formWindow()->clearSelection( false );
    layout.doLayout();
    formWindow()->mainWindow()->objectHierarchy()->rebuild();
}

void LayoutHorizontalSplitCommand::unexecute()
{
    formWindow()->clearSelection( false );
    layout.undoLayout();
    formWindow()->mainWindow()->objectHierarchy()->rebuild();
}

// ------------------------------------------------------------

LayoutVerticalCommand::LayoutVerticalCommand( const TQString &n, FormWindow *fw,
					      TQWidget *parent, TQWidget *layoutBase,
					      const TQWidgetList &wl )
    : Command( n, fw ), layout( wl, parent, fw, layoutBase )
{
}

void LayoutVerticalCommand::execute()
{
    formWindow()->clearSelection( false );
    layout.doLayout();
    formWindow()->mainWindow()->objectHierarchy()->rebuild();
}

void LayoutVerticalCommand::unexecute()
{
    formWindow()->clearSelection( false );
    layout.undoLayout();
    formWindow()->mainWindow()->objectHierarchy()->rebuild();
}

// ------------------------------------------------------------

LayoutVerticalSplitCommand::LayoutVerticalSplitCommand( const TQString &n, FormWindow *fw,
							TQWidget *parent, TQWidget *layoutBase,
							const TQWidgetList &wl )
    : Command( n, fw ), layout( wl, parent, fw, layoutBase, true, true )
{
}

void LayoutVerticalSplitCommand::execute()
{
    formWindow()->clearSelection( false );
    layout.doLayout();
    formWindow()->mainWindow()->objectHierarchy()->rebuild();
}

void LayoutVerticalSplitCommand::unexecute()
{
    formWindow()->clearSelection( false );
    layout.undoLayout();
    formWindow()->mainWindow()->objectHierarchy()->rebuild();
}

// ------------------------------------------------------------

LayoutGridCommand::LayoutGridCommand( const TQString &n, FormWindow *fw,
				      TQWidget *parent, TQWidget *layoutBase,
				      const TQWidgetList &wl, int xres, int yres )
    : Command( n, fw ), layout( wl, parent, fw, layoutBase, TQSize( TQMAX(5,xres), TQMAX(5,yres) ) )
{
}

void LayoutGridCommand::execute()
{
    formWindow()->clearSelection( false );
    layout.doLayout();
    formWindow()->mainWindow()->objectHierarchy()->rebuild();
}

void LayoutGridCommand::unexecute()
{
    formWindow()->clearSelection( false );
    layout.undoLayout();
    formWindow()->mainWindow()->objectHierarchy()->rebuild();
}

// ------------------------------------------------------------

BreakLayoutCommand::BreakLayoutCommand( const TQString &n, FormWindow *fw,
					TQWidget *layoutBase, const TQWidgetList &wl )
    : Command( n, fw ), lb( layoutBase ), widgets( wl )
{
    WidgetFactory::LayoutType lay = WidgetFactory::layoutType( layoutBase );
    spacing = MetaDataBase::spacing( layoutBase );
    margin = MetaDataBase::margin( layoutBase );
    layout = 0;
    if ( lay == WidgetFactory::HBox )
	layout = new HorizontalLayout( wl, layoutBase, fw, layoutBase, false, ::tqt_cast<TQSplitter*>(layoutBase) != 0 );
    else if ( lay == WidgetFactory::VBox )
	layout = new VerticalLayout( wl, layoutBase, fw, layoutBase, false, ::tqt_cast<TQSplitter*>(layoutBase) != 0 );
    else if ( lay == WidgetFactory::Grid )
	layout = new GridLayout( wl, layoutBase, fw, layoutBase, TQSize( TQMAX( 5, fw->grid().x()), TQMAX( 5, fw->grid().y()) ), false );
}

void BreakLayoutCommand::execute()
{
    if ( !layout )
	return;
    formWindow()->clearSelection( false );
    layout->breakLayout();
    formWindow()->mainWindow()->objectHierarchy()->rebuild();
    for ( TQWidget *w = widgets.first(); w; w = widgets.next() )
	w->resize( TQMAX( 16, w->width() ), TQMAX( 16, w->height() ) );
}

void BreakLayoutCommand::unexecute()
{
    if ( !layout )
	return;
    formWindow()->clearSelection( false );
    layout->doLayout();
    formWindow()->mainWindow()->objectHierarchy()->rebuild();
    MetaDataBase::setSpacing( WidgetFactory::containerOfWidget( lb ), spacing );
    MetaDataBase::setMargin( WidgetFactory::containerOfWidget( lb ), margin );
}

// ------------------------------------------------------------

MacroCommand::MacroCommand( const TQString &n, FormWindow *fw,
			    const TQPtrList<Command> &cmds )
    : Command( n, fw ), commands( cmds )
{
}

void MacroCommand::execute()
{
    for ( Command *c = commands.first(); c; c = commands.next() )
	c->execute();
}

void MacroCommand::unexecute()
{
    for ( Command *c = commands.last(); c; c = commands.prev() )
	c->unexecute();
}

// ------------------------------------------------------------

AddTabPageCommand::AddTabPageCommand( const TQString &n, FormWindow *fw,
				      TQTabWidget *tw, const TQString &label )
    : Command( n, fw ), tabWidget( tw ), tabLabel( label )
{
    tabPage = new TQDesignerWidget( formWindow(), tabWidget, "TabPage" );
    tabPage->hide();
    index = -1;
    MetaDataBase::addEntry( tabPage );
}

void AddTabPageCommand::execute()
{
    if ( index == -1 )
	index = ( (TQDesignerTabWidget*)tabWidget )->count();
    tabWidget->insertTab( tabPage, tabLabel, index );
    tabWidget->showPage( tabPage );
    formWindow()->emitUpdateProperties( formWindow()->currentWidget() );
    formWindow()->mainWindow()->objectHierarchy()->tabsChanged( tabWidget );
}

void AddTabPageCommand::unexecute()
{
    tabWidget->removePage( tabPage );
    tabPage->hide();
    formWindow()->emitUpdateProperties( formWindow()->currentWidget() );
    formWindow()->mainWindow()->objectHierarchy()->tabsChanged( tabWidget );
}

// ------------------------------------------------------------

MoveTabPageCommand::MoveTabPageCommand( const TQString &n, FormWindow *fw,
				      TQTabWidget *tw, TQWidget* page, const TQString& label, int nIndex, int oIndex )
    : Command( n, fw ), tabWidget( tw ), tabPage( page ), tabLabel( label )
{
    newIndex = nIndex;
    oldIndex = oIndex;
}

void MoveTabPageCommand::execute()
{
    ((TQDesignerTabWidget*)tabWidget )->removePage( tabPage );
    ((TQDesignerTabWidget*)tabWidget )->insertTab( tabPage, tabLabel, newIndex );
    ((TQDesignerTabWidget*)tabWidget )->showPage( tabPage );
    formWindow()->emitUpdateProperties( formWindow()->currentWidget() );
    formWindow()->mainWindow()->objectHierarchy()->tabsChanged( tabWidget );
}

void MoveTabPageCommand::unexecute()
{
    ((TQDesignerTabWidget*)tabWidget )->removePage( tabPage );
    ((TQDesignerTabWidget*)tabWidget )->insertTab( tabPage, tabLabel, oldIndex );
    ((TQDesignerTabWidget*)tabWidget )->showPage( tabPage );
    formWindow()->emitUpdateProperties( formWindow()->currentWidget() );
    formWindow()->mainWindow()->objectHierarchy()->tabsChanged( tabWidget );
}

// ------------------------------------------------------------

DeleteTabPageCommand::DeleteTabPageCommand( const TQString &n, FormWindow *fw,
					    TQTabWidget *tw, TQWidget *page )
    : Command( n, fw ), tabWidget( tw ), tabPage( page )
{
    tabLabel = ( (TQDesignerTabWidget*)tabWidget )->pageTitle();
    index = ( (TQDesignerTabWidget*)tabWidget )->currentPage();
}

void DeleteTabPageCommand::execute()
{
    tabWidget->removePage( tabPage );
    tabPage->hide();
    formWindow()->emitUpdateProperties( formWindow()->currentWidget() );
    formWindow()->mainWindow()->objectHierarchy()->tabsChanged( tabWidget );
}

void DeleteTabPageCommand::unexecute()
{
    tabWidget->insertTab( tabPage, tabLabel, index );
    tabWidget->showPage( tabPage );
    formWindow()->emitUpdateProperties( formWindow()->currentWidget() );
    formWindow()->mainWindow()->objectHierarchy()->tabsChanged( tabWidget );
}

// ------------------------------------------------------------

AddWidgetStackPageCommand::AddWidgetStackPageCommand( const TQString &n, FormWindow *fw,
						      TQDesignerWidgetStack *ws )
    : Command( n, fw ), widgetStack( ws )
{
    stackPage = new TQDesignerWidget( formWindow(), widgetStack, "WStackPage" );
    stackPage->hide();
    index = -1;
    MetaDataBase::addEntry( stackPage );
}

void AddWidgetStackPageCommand::execute()
{
    index = widgetStack->insertPage( stackPage, index );
    formWindow()->emitUpdateProperties( formWindow()->currentWidget() );
    formWindow()->mainWindow()->objectHierarchy()->tabsChanged( 0 );
}

void AddWidgetStackPageCommand::unexecute()
{
    index = widgetStack->removePage( stackPage );
    stackPage->hide();
    formWindow()->emitUpdateProperties( formWindow()->currentWidget() );
    formWindow()->mainWindow()->objectHierarchy()->tabsChanged( 0 );
}

DeleteWidgetStackPageCommand::DeleteWidgetStackPageCommand( const TQString &n, FormWindow *fw,
							    TQDesignerWidgetStack *ws, TQWidget *page )
    : Command( n, fw), widgetStack( ws ), stackPage( page )
{
    index = -1;
}

void DeleteWidgetStackPageCommand::execute()
{
    index = widgetStack->removePage( stackPage );
    stackPage->hide();
    formWindow()->emitUpdateProperties( formWindow()->currentWidget() );
    formWindow()->mainWindow()->objectHierarchy()->widgetRemoved( stackPage );
}

void DeleteWidgetStackPageCommand::unexecute()
{
    index = widgetStack->insertPage( stackPage, index );
    widgetStack->raiseWidget( stackPage );
    formWindow()->emitUpdateProperties( formWindow()->currentWidget() );
    formWindow()->mainWindow()->objectHierarchy()->widgetInserted( stackPage );
}

// ------------------------------------------------------------

AddWizardPageCommand::AddWizardPageCommand( const TQString &n, FormWindow *fw,
					    TQWizard *w, const TQString &label, int i, bool s )
    : Command( n, fw ), wizard( w ), pageLabel( label )
{
    page = new TQDesignerWidget( formWindow(), wizard, "WizardPage" );
    page->hide();
    index = i;
    show = s;
    MetaDataBase::addEntry( page );
}

void AddWizardPageCommand::execute()
{
    if ( index == -1 )
	index = wizard->pageCount();
    wizard->insertPage( page, pageLabel, index );
    if ( show )
	( (TQDesignerWizard*)wizard )->setCurrentPage( ( (TQDesignerWizard*)wizard )->pageNum( page ) );
    formWindow()->emitUpdateProperties( formWindow()->currentWidget() );
    formWindow()->mainWindow()->objectHierarchy()->pagesChanged( wizard );
}

void AddWizardPageCommand::unexecute()
{
    wizard->removePage( page );
    page->hide();
    formWindow()->emitUpdateProperties( formWindow()->currentWidget() );
    formWindow()->mainWindow()->objectHierarchy()->pagesChanged( wizard );
}

// ------------------------------------------------------------

DeleteWizardPageCommand::DeleteWizardPageCommand( const TQString &n, FormWindow *fw,
						  TQWizard *w, int i, bool s )
    : Command( n, fw ), wizard( w ), index( i )
{
    show = s;
}

void DeleteWizardPageCommand::execute()
{
    page = wizard->page( index );
    pageLabel = wizard->title( page );
    wizard->removePage( page );
    page->hide();
    formWindow()->emitUpdateProperties( formWindow()->currentWidget() );
    formWindow()->mainWindow()->objectHierarchy()->pagesChanged( wizard );
}

void DeleteWizardPageCommand::unexecute()
{
    wizard->insertPage( page, pageLabel, index );
    if ( show )
	( (TQDesignerWizard*)wizard )->setCurrentPage( ( (TQDesignerWizard*)wizard )->pageNum( page ) );
    formWindow()->emitUpdateProperties( formWindow()->currentWidget() );
    formWindow()->mainWindow()->objectHierarchy()->pagesChanged( wizard );
}

// ------------------------------------------------------------

RenameWizardPageCommand::RenameWizardPageCommand( const TQString &n, FormWindow *fw,
						  TQWizard *w, int i, const TQString& name )
    : Command( n, fw ), wizard( w ), index( i ), label( name )
{

}

void RenameWizardPageCommand::execute()
{
    page = wizard->page( index );
    TQString oldLabel = wizard->title( page );

    wizard->setTitle( page, label );
    label = oldLabel;
    formWindow()->emitUpdateProperties( formWindow()->currentWidget() );
}

void RenameWizardPageCommand::unexecute()
{
    execute();
}

// ------------------------------------------------------------

SwapWizardPagesCommand::SwapWizardPagesCommand( const TQString &n, FormWindow *fw, TQWizard *w, int i1, int i2 )
    : Command( n, fw ), wizard( w ), index1( i1 ), index2( i2 )
{
}

void SwapWizardPagesCommand::execute()
{
    TQWidget *page1 = wizard->page( index1 );
    TQWidget *page2 = wizard->page( index2 );
    TQString page1Label = wizard->title( page1 );
    TQString page2Label = wizard->title( page2 );
    wizard->removePage( page1 );
    wizard->removePage( page2 );
    wizard->insertPage( page1, page1Label, index2 );
    wizard->insertPage( page2, page2Label, index1 );
    formWindow()->emitUpdateProperties( formWindow()->currentWidget() );
    formWindow()->mainWindow()->objectHierarchy()->pagesChanged( wizard );
}

void SwapWizardPagesCommand::unexecute()
{
    execute();
}

// ------------------------------------------------------------

MoveWizardPageCommand::MoveWizardPageCommand( const TQString &n, FormWindow *fw, TQWizard *w, int i1, int i2 )
    : Command( n, fw ), wizard( w ), index1( i1 ), index2( i2 )
{
}

void MoveWizardPageCommand::execute()
{
    TQWidget *page = wizard->page( index1 );
    TQString pageLabel = wizard->title( page );
    wizard->removePage( page );
    wizard->insertPage( page, pageLabel, index2 );
    formWindow()->emitUpdateProperties( formWindow()->currentWidget() );
    formWindow()->mainWindow()->objectHierarchy()->pagesChanged( wizard );
}

void MoveWizardPageCommand::unexecute()
{
    // ###FIX: index1 may be the wrong place
    TQWidget *page = wizard->page( index2 );
    TQString pageLabel = wizard->title( page );
    wizard->removePage( page );
    wizard->insertPage( page, pageLabel, index1 );
    formWindow()->emitUpdateProperties( formWindow()->currentWidget() );
    formWindow()->mainWindow()->objectHierarchy()->pagesChanged( wizard );
}

// ------------------------------------------------------------

AddConnectionCommand::AddConnectionCommand( const TQString &name, FormWindow *fw,
					    MetaDataBase::Connection c )
    : Command( name, fw ), connection( c )
{
}

void AddConnectionCommand::execute()
{
    MetaDataBase::addConnection( formWindow(), connection.sender,
				 connection.signal, connection.receiver, connection.slot );
    if ( connection.receiver == formWindow()->mainContainer() )
	formWindow()->mainWindow()->propertyeditor()->eventList()->setup();
}

void AddConnectionCommand::unexecute()
{
    MetaDataBase::removeConnection( formWindow(), connection.sender,
				    connection.signal, connection.receiver, connection.slot );
    if ( connection.receiver == formWindow()->mainContainer() )
	formWindow()->mainWindow()->propertyeditor()->eventList()->setup();
}

// ------------------------------------------------------------

RemoveConnectionCommand::RemoveConnectionCommand( const TQString &name, FormWindow *fw,
						  MetaDataBase::Connection c )
    : Command( name, fw ), connection( c )
{
}

void RemoveConnectionCommand::execute()
{
    MetaDataBase::removeConnection( formWindow(), connection.sender,
				    connection.signal, connection.receiver, connection.slot );
    if ( connection.receiver == formWindow()->mainContainer() )
	formWindow()->mainWindow()->propertyeditor()->eventList()->setup();
}

void RemoveConnectionCommand::unexecute()
{
    MetaDataBase::addConnection( formWindow(), connection.sender,
				 connection.signal, connection.receiver, connection.slot );
    if ( connection.receiver == formWindow()->mainContainer() )
	formWindow()->mainWindow()->propertyeditor()->eventList()->setup();
}

// ------------------------------------------------------------

AddFunctionCommand::AddFunctionCommand( const TQString &name, FormWindow *fw, const TQCString &f,
					const TQString& spec, const TQString &a, const TQString &t,
					const TQString &l, const TQString &rt )
    : Command( name, fw ), function( f ), specifier( spec ), access( a ), functionType( t ), language( l ),
      returnType( rt )
{
}

void AddFunctionCommand::execute()
{
    MetaDataBase::addFunction( formWindow(), function, specifier, access, functionType, language, returnType );
    formWindow()->mainWindow()->functionsChanged();
    if ( formWindow()->formFile() )
	formWindow()->formFile()->setModified( true );
}

void AddFunctionCommand::unexecute()
{
    MetaDataBase::removeFunction( formWindow(), function, specifier, access, functionType,  language, returnType );
    formWindow()->mainWindow()->functionsChanged();
    if ( formWindow()->formFile() )
	formWindow()->formFile()->setModified( true );
}

// ------------------------------------------------------------

ChangeFunctionAttribCommand::ChangeFunctionAttribCommand( const TQString &name, FormWindow *fw, MetaDataBase::Function f,
							  const TQString &on, const TQString &os,
							  const TQString &oa, const TQString &ot, const TQString &ol,
							  const TQString &ort )
    : Command( name, fw ), oldName( on ), oldSpec( os ), oldAccess( oa ),
      oldType( ot ), oldLang( ol ), oldReturnType( ort )
{
	    newName = f.function;
	    newSpec = f.specifier;
	    newAccess = f.access;
	    newType = f.type;
	    newLang = f.language;
	    newReturnType = f.returnType;
}

void ChangeFunctionAttribCommand::execute()
{
    MetaDataBase::changeFunctionAttributes( formWindow(), oldName, newName, newSpec, newAccess,
					    newType, newLang, newReturnType );
    formWindow()->formFile()->functionNameChanged( oldName, newName );
    formWindow()->formFile()->functionRetTypeChanged( newName, oldReturnType, newReturnType );
    formWindow()->mainWindow()->functionsChanged();
    if ( formWindow()->formFile() )
	formWindow()->formFile()->setModified( true );
}

void ChangeFunctionAttribCommand::unexecute()
{
    MetaDataBase::changeFunctionAttributes( formWindow(), newName, oldName, oldSpec, oldAccess,
					    oldType, oldLang, oldReturnType );
    formWindow()->formFile()->functionNameChanged( newName, oldName );
    formWindow()->formFile()->functionRetTypeChanged( oldName, newReturnType, oldReturnType );
    formWindow()->mainWindow()->functionsChanged();
    if ( formWindow()->formFile() )
	formWindow()->formFile()->setModified( true );
}

// ------------------------------------------------------------

RemoveFunctionCommand::RemoveFunctionCommand( const TQString &name, FormWindow *fw, const TQCString &f,
						const TQString& spec, const TQString &a, const TQString &t,
						const TQString &l, const TQString &rt )
    : Command( name, fw ), function( f ), specifier( spec ), access( a ), functionType( t ), language( l ),
      returnType( rt )
{
    if ( spec.isNull() ) {
	TQValueList<MetaDataBase::Function> lst = MetaDataBase::functionList( fw );
	for ( TQValueList<MetaDataBase::Function>::Iterator it = lst.begin(); it != lst.end(); ++it ) {
	    if ( MetaDataBase::normalizeFunction( (*it).function ) ==
		 MetaDataBase::normalizeFunction( function ) ) {
		specifier = (*it).specifier;
		access = (*it).access;
		functionType = (*it).type;
		returnType = (*it).returnType;
		language = (*it).language;
		break;
	    }
	}
    }
}

void RemoveFunctionCommand::execute()
{
    MetaDataBase::removeFunction( formWindow(), function, specifier, access, functionType, language, returnType );
    formWindow()->mainWindow()->functionsChanged();
    if ( formWindow()->formFile() )
	formWindow()->formFile()->setModified( true );
}

void RemoveFunctionCommand::unexecute()
{
    if ( MetaDataBase::hasFunction( formWindow(), function ) )
	return;
    MetaDataBase::addFunction( formWindow(), function, specifier, access, functionType, language, returnType );
    formWindow()->mainWindow()->functionsChanged();
    if ( formWindow()->formFile() )
	formWindow()->formFile()->setModified( true );
}

// ------------------------------------------------------------

AddVariableCommand::AddVariableCommand( const TQString &name, FormWindow *fw, const TQString &vn, const TQString &a )
    : Command( name, fw ), varName( vn ), access( a )
{
}

void AddVariableCommand::execute()
{
    MetaDataBase::addVariable( formWindow(), varName, access );
    formWindow()->mainWindow()->objectHierarchy()->updateFormDefinitionView();
    if ( formWindow()->formFile() )
	formWindow()->formFile()->setModified( true );
}

void AddVariableCommand::unexecute()
{
    MetaDataBase::removeVariable( formWindow(), varName );
    formWindow()->mainWindow()->objectHierarchy()->updateFormDefinitionView();
    if ( formWindow()->formFile() )
	formWindow()->formFile()->setModified( true );
}

// ------------------------------------------------------------

SetVariablesCommand::SetVariablesCommand( const TQString &name, FormWindow *fw,
    TQValueList<MetaDataBase::Variable> lst )
    : Command( name, fw ), newList( lst )
{
    oldList = MetaDataBase::variables( formWindow() );
}

void SetVariablesCommand::execute()
{
    MetaDataBase::setVariables( formWindow(), newList );
    formWindow()->mainWindow()->objectHierarchy()->updateFormDefinitionView();
    if ( formWindow()->formFile() )
	formWindow()->formFile()->setModified( true );
}

void SetVariablesCommand::unexecute()
{
    MetaDataBase::setVariables( formWindow(), oldList );
    formWindow()->mainWindow()->objectHierarchy()->updateFormDefinitionView();
    if ( formWindow()->formFile() )
	formWindow()->formFile()->setModified( true );
}

// ------------------------------------------------------------

RemoveVariableCommand::RemoveVariableCommand( const TQString &name, FormWindow *fw, const TQString &vn )
    : Command( name, fw ), varName( vn )
{
    TQValueList<MetaDataBase::Variable> lst = MetaDataBase::variables( fw );
    for ( TQValueList<MetaDataBase::Variable>::Iterator it = lst.begin(); it != lst.end(); ++it ) {
	if ( (*it).varName == varName ) {
	    access = (*it).varAccess;
	    break;
	}
    }
}

void RemoveVariableCommand::execute()
{
    MetaDataBase::removeVariable( formWindow(), varName );
    formWindow()->mainWindow()->objectHierarchy()->updateFormDefinitionView();
    if ( formWindow()->formFile() )
	formWindow()->formFile()->setModified( true );
}

void RemoveVariableCommand::unexecute()
{
    MetaDataBase::addVariable( formWindow(), varName, access );
    formWindow()->mainWindow()->objectHierarchy()->updateFormDefinitionView();
    if ( formWindow()->formFile() )
	formWindow()->formFile()->setModified( true );
}

// ------------------------------------------------------------

EditDefinitionsCommand::EditDefinitionsCommand( const TQString &name, FormWindow *fw, LanguageInterface *lf,
						const TQString &n, const TQStringList &nl )
    : Command( name, fw ), lIface( lf ), defName( n ), newList( nl )
{
    oldList = lIface->definitionEntries( defName, formWindow()->mainWindow()->designerInterface() );
}

void EditDefinitionsCommand::execute()
{
    lIface->setDefinitionEntries( defName, newList, formWindow()->mainWindow()->designerInterface() );
    lIface->release();
    formWindow()->mainWindow()->objectHierarchy()->updateFormDefinitionView();
    if ( formWindow()->formFile() )
	formWindow()->formFile()->setModified( true );
}

void EditDefinitionsCommand::unexecute()
{
    lIface->setDefinitionEntries( defName, oldList, formWindow()->mainWindow()->designerInterface() );
    lIface->release();
    formWindow()->mainWindow()->objectHierarchy()->updateFormDefinitionView();
    if ( formWindow()->formFile() )
	formWindow()->formFile()->setModified( true );
}

// ------------------------------------------------------------

LowerCommand::LowerCommand( const TQString &name, FormWindow *fw, const TQWidgetList &w )
    : Command( name, fw ), widgets( w )
{
}

void LowerCommand::execute()
{
    for ( TQWidget *w = widgets.first(); w; w = widgets.next() ) {
	w->lower();
	formWindow()->raiseSelection( w );
    }

}

void LowerCommand::unexecute()
{
    for ( TQWidget *w = widgets.first(); w; w = widgets.next() ) {
	w->raise();
	formWindow()->raiseSelection( w );
    }
}

// ------------------------------------------------------------

RaiseCommand::RaiseCommand( const TQString &name, FormWindow *fw, const TQWidgetList &w )
    : Command( name, fw ), widgets( w )
{
}

void RaiseCommand::execute()
{
    for ( TQWidget *w = widgets.first(); w; w = widgets.next() ) {
	w->raise();
	formWindow()->raiseSelection( w );
    }

}

void RaiseCommand::unexecute()
{
    for ( TQWidget *w = widgets.first(); w; w = widgets.next() ) {
	w->lower();
	formWindow()->raiseSelection( w );
    }
}

// ------------------------------------------------------------

PasteCommand::PasteCommand( const TQString &n, FormWindow *fw,
			      const TQWidgetList &w )
    : Command( n, fw ), widgets( w )
{
}

void PasteCommand::execute()
{
    for ( TQWidget *w = widgets.first(); w; w = widgets.next() ) {
	w->show();
	formWindow()->selectWidget( w );
	formWindow()->widgets()->insert( w, w );
	formWindow()->mainWindow()->objectHierarchy()->widgetInserted( w );
    }
}

void PasteCommand::unexecute()
{
    for ( TQWidget *w = widgets.first(); w; w = widgets.next() ) {
	w->hide();
	formWindow()->selectWidget( w, false );
	formWindow()->widgets()->remove( w );
	formWindow()->mainWindow()->objectHierarchy()->widgetRemoved( w );
    }
 }

// ------------------------------------------------------------

TabOrderCommand::TabOrderCommand( const TQString &n, FormWindow *fw,
				  const TQWidgetList &ol, const TQWidgetList &nl )
    : Command( n, fw ), oldOrder( ol ), newOrder( nl )
{
}

void TabOrderCommand::merge( Command *c )
{
    TabOrderCommand *cmd = (TabOrderCommand*)c;
    newOrder = cmd->newOrder;
}

bool TabOrderCommand::canMerge( Command * )
{
    return true;
}

void TabOrderCommand::execute()
{
    MetaDataBase::setTabOrder( formWindow(), newOrder );
}

void TabOrderCommand::unexecute()
{
    MetaDataBase::setTabOrder( formWindow(), oldOrder );
}

// ------------------------------------------------------------

PopulateListBoxCommand::PopulateListBoxCommand( const TQString &n, FormWindow *fw,
						TQListBox *lb, const TQValueList<Item> &items )
    : Command( n, fw ), newItems( items ), listbox( lb )
{
    TQListBoxItem *i = 0;
    for ( i = listbox->firstItem(); i; i = i->next() ) {
	Item item;
	if ( i->pixmap() )
	    item.pix = *i->pixmap();
	item.text = i->text();
	oldItems.append( item );
    }
}

void PopulateListBoxCommand::execute()
{
    listbox->clear();
    for ( TQValueList<Item>::Iterator it = newItems.begin(); it != newItems.end(); ++it ) {
	Item i = *it;
	if ( !i.pix.isNull() )
	    (void)new TQListBoxPixmap( listbox, i.pix, i.text );
	else
	    (void)new TQListBoxText( listbox, i.text );
    }
    formWindow()->mainWindow()->propertyeditor()->refetchData();
}

void PopulateListBoxCommand::unexecute()
{
    listbox->clear();
    for ( TQValueList<Item>::Iterator it = oldItems.begin(); it != oldItems.end(); ++it ) {
	Item i = *it;
	if ( !i.pix.isNull() )
	    (void)new TQListBoxPixmap( listbox, i.pix, i.text );
	else
	    (void)new TQListBoxText( listbox, i.text );
    }
    formWindow()->mainWindow()->propertyeditor()->refetchData();
}

// ------------------------------------------------------------

PopulateIconViewCommand::PopulateIconViewCommand( const TQString &n, FormWindow *fw,
						  TQIconView *iv, const TQValueList<Item> &items )
    : Command( n, fw ), newItems( items ), iconview( iv )
{
#ifndef TQT_NO_ICONVIEW
    TQIconViewItem *i = 0;
    for ( i = iconview->firstItem(); i; i = i->nextItem() ) {
	Item item;
	if ( i->pixmap() )
	    item.pix = *i->pixmap();
	item.text = i->text();
	oldItems.append( item );
    }
#endif
}

void PopulateIconViewCommand::execute()
{
#ifndef TQT_NO_ICONVIEW
    iconview->clear();
    for ( TQValueList<Item>::Iterator it = newItems.begin(); it != newItems.end(); ++it ) {
	Item i = *it;
	(void)new TQIconViewItem( iconview, i.text, i.pix );
    }
#endif
}

void PopulateIconViewCommand::unexecute()
{
#ifndef TQT_NO_ICONVIEW
    iconview->clear();
    for ( TQValueList<Item>::Iterator it = oldItems.begin(); it != oldItems.end(); ++it ) {
	Item i = *it;
	(void)new TQIconViewItem( iconview, i.text, i.pix );
    }
#endif
}

// ------------------------------------------------------------

PopulateListViewCommand::PopulateListViewCommand( const TQString &n, FormWindow *fw,
						  TQListView *lv, TQListView *from )
    : Command( n, fw ), listview( lv )
{
    newItems = new TQListView();
    newItems->hide();
    transferItems( from, newItems );
    oldItems = new TQListView();
    oldItems->hide();
    transferItems( listview, oldItems );
}

void PopulateListViewCommand::execute()
{
    listview->clear();
    transferItems( newItems, listview );
}

void PopulateListViewCommand::unexecute()
{
    listview->clear();
    transferItems( oldItems, listview );
}

void PopulateListViewCommand::transferItems( TQListView *from, TQListView *to )
{
    TQHeader *header = to->header();
    while ( header->count() )
	to->removeColumn( 0 );
    TQHeader *h2 = from->header();
    for ( int i = 0; i < h2->count(); ++i ) {
	to->addColumn( h2->label( i ) );
	if ( h2->iconSet( i ) && !h2->iconSet( i )->pixmap().isNull() )
	    header->setLabel( i, *h2->iconSet( i ), h2->label( i ) );
	header->setResizeEnabled( h2->isResizeEnabled( i ), i );
	header->setClickEnabled( h2->isClickEnabled( i ), i );
    }

    TQListViewItemIterator it( from );
    TQPtrStack<TQListViewItem> fromParents, toParents;
    fromParents.push( 0 );
    toParents.push( 0 );
    TQPtrStack<TQListViewItem> toLasts;
    TQListViewItem *fromLast = 0;
    toLasts.push( 0 );
    int cols = from->columns();
    to->setSorting( -1 );
    from->setSorting( -1 );
    for ( ; it.current(); ++it ) {
	TQListViewItem *i = it.current();
	if ( i->parent() == fromParents.top() ) {
	    TQListViewItem *pi = toParents.top();
	    TQListViewItem *ni = 0;
	    if ( pi )
		ni = new TQListViewItem( pi, toLasts.top() );
	    else
		ni = new TQListViewItem( to, toLasts.top() );
	    for ( int c = 0; c < cols; ++c ) {
		ni->setText( c, i->text( c ) );
		if ( i->pixmap( c ) )
		    ni->setPixmap( c, *i->pixmap( c ) );
	    }
	    toLasts.pop();
	    toLasts.push( ni );
	    if ( pi )
		pi->setOpen( true );
	} else {
	    if ( i->parent() == fromLast ) {
		fromParents.push( fromLast );
		toParents.push( toLasts.top() );
		toLasts.push( 0 );
		TQListViewItem *pi = toParents.top();
		TQListViewItem *ni = 0;
		if ( pi )
		    ni = new TQListViewItem( pi );
		else
		    ni = new TQListViewItem( to );
		for ( int c = 0; c < cols; ++c ) {
		    ni->setText( c, i->text( c ) );
		    if ( i->pixmap( c ) )
			ni->setPixmap( c, *i->pixmap( c ) );
		}
		toLasts.pop();
		toLasts.push( ni );
		if ( pi )
		    pi->setOpen( true );
	    } else {
		while ( fromParents.top() != i->parent() ) {
		    fromParents.pop();
		    toParents.pop();
		    toLasts.pop();
		}

		TQListViewItem *pi = toParents.top();
		TQListViewItem *ni = 0;
		if ( pi )
		    ni = new TQListViewItem( pi, toLasts.top() );
		else
		    ni = new TQListViewItem( to, toLasts.top() );
		for ( int c = 0; c < cols; ++c ) {
		    ni->setText( c, i->text( c ) );
		    if ( i->pixmap( c ) )
			ni->setPixmap( c, *i->pixmap( c ) );
		}
		if ( pi )
		    pi->setOpen( true );
		toLasts.pop();
		toLasts.push( ni );
	    }
	}
	fromLast = i;
    }
}



// ------------------------------------------------------------

PopulateMultiLineEditCommand::PopulateMultiLineEditCommand( const TQString &n, FormWindow *fw,
						    TQTextEdit *mle, const TQString &txt )
    : Command( n, fw ), newText( txt ), mlined( mle )
{
    oldText = mlined->text();
    wasChanged = MetaDataBase::isPropertyChanged( mlined, "text" );
}

void PopulateMultiLineEditCommand::execute()
{
    mlined->setText( newText );
    MetaDataBase::setPropertyChanged( mlined, "text", true );
    formWindow()->emitUpdateProperties( mlined );
}

void PopulateMultiLineEditCommand::unexecute()
{
    mlined->setText( oldText );
    MetaDataBase::setPropertyChanged( mlined, "text", wasChanged );
    formWindow()->emitUpdateProperties( mlined );
}

// ------------------------------------------------------------

PopulateTableCommand::PopulateTableCommand( const TQString &n, FormWindow *fw, TQTable *t,
					    const TQValueList<Row> &rows,
					    const TQValueList<Column> &columns )
    : Command( n, fw ), newRows( rows ), newColumns( columns ), table( t )
{
#ifndef TQT_NO_TABLE
    int i = 0;
    TQMap<TQString, TQString> columnFields = MetaDataBase::columnFields( table );
    for ( i = 0; i < table->horizontalHeader()->count(); ++i ) {
	PopulateTableCommand::Column col;
	col.text = table->horizontalHeader()->label( i );
	if ( table->horizontalHeader()->iconSet( i ) )
	    col.pix = table->horizontalHeader()->iconSet( i )->pixmap();
	col.field = *columnFields.find( col.text );
	oldColumns.append( col );
    }
    for ( i = 0; i < table->verticalHeader()->count(); ++i ) {
	PopulateTableCommand::Row row;
	row.text = table->verticalHeader()->label( i );
	if ( table->verticalHeader()->iconSet( i ) )
	    row.pix = table->verticalHeader()->iconSet( i )->pixmap();
	oldRows.append( row );
    }
#endif
}

void PopulateTableCommand::execute()
{
#ifndef TQT_NO_TABLE
    TQMap<TQString, TQString> columnFields;
    table->setNumCols( (int)newColumns.count() );
    int i = 0;
    for ( TQValueList<Column>::Iterator cit = newColumns.begin(); cit != newColumns.end(); ++cit, ++i ) {
	table->horizontalHeader()->setLabel( i, (*cit).pix, (*cit).text );
	if ( !(*cit).field.isEmpty() )
	    columnFields.insert( (*cit).text, (*cit).field );
    }
    MetaDataBase::setColumnFields( table, columnFields );
    table->setNumRows( (int)newRows.count() );
    i = 0;
    for ( TQValueList<Row>::Iterator rit = newRows.begin(); rit != newRows.end(); ++rit, ++i )
	table->verticalHeader()->setLabel( i, (*rit).pix, (*rit).text );
#endif
}

void PopulateTableCommand::unexecute()
{
#ifndef TQT_NO_TABLE
    TQMap<TQString, TQString> columnFields;
    table->setNumCols( (int)oldColumns.count() );
    int i = 0;
    for ( TQValueList<Column>::Iterator cit = oldColumns.begin(); cit != oldColumns.end(); ++cit, ++i ) {
	table->horizontalHeader()->setLabel( i, (*cit).pix, (*cit).text );
	if ( !(*cit).field.isEmpty() )
	    columnFields.insert( (*cit).text, (*cit).field );
    }
    MetaDataBase::setColumnFields( table, columnFields );
    table->setNumRows( (int)oldRows.count() );
    i = 0;
    for ( TQValueList<Row>::Iterator rit = oldRows.begin(); rit != oldRows.end(); ++rit, ++i )
	table->verticalHeader()->setLabel( i, (*rit).pix, (*rit).text );
#endif
}

// ------------------------------------------------------------

AddActionToToolBarCommand::AddActionToToolBarCommand( const TQString &n, FormWindow *fw,
						      TQAction *a, TQDesignerToolBar *tb, int idx )
    : Command( n, fw ), action( a ), toolBar( tb ), index( idx )
{
}

void AddActionToToolBarCommand::execute()
{
    action->addTo( toolBar );

    if ( ::tqt_cast<TQDesignerAction*>(action) ) {
	TQString s = ( (TQDesignerAction*)action )->widget()->name();
	if ( s.startsWith( "qt_dead_widget_" ) ) {
	    s.remove( 0, TQString( "qt_dead_widget_" ).length() );
	    ( (TQDesignerAction*)action )->widget()->setName( s );
	}
	toolBar->insertAction( ( (TQDesignerAction*)action )->widget(), action );
	( (TQDesignerAction*)action )->widget()->installEventFilter( toolBar );
    } else if ( ::tqt_cast<TQDesignerActionGroup*>(action) ) {
	if ( ( (TQDesignerActionGroup*)action )->usesDropDown() ) {
	    toolBar->insertAction( ( (TQDesignerActionGroup*)action )->widget(), action );
	    ( (TQDesignerActionGroup*)action )->widget()->installEventFilter( toolBar );
	}
    } else if ( ::tqt_cast<TQSeparatorAction*>(action) ) {
	toolBar->insertAction( ( (TQSeparatorAction*)action )->widget(), action );
	( (TQSeparatorAction*)action )->widget()->installEventFilter( toolBar );
    }
    if ( !::tqt_cast<TQActionGroup*>(action) || ( (TQActionGroup*)action )->usesDropDown()) {
	if ( index == -1 )
	    toolBar->appendAction( action );
	else
	    toolBar->insertAction( index, action );
	toolBar->reInsert();
	TQObject::connect( action, TQ_SIGNAL( destroyed() ), toolBar, TQ_SLOT( actionRemoved() ) );
    } else {
	if ( action->children() ) {
	    TQObjectListIt it( *action->children() );
	    int i = 0;
	    while ( it.current() ) {
		TQObject *o = it.current();
		++it;
		if ( !::tqt_cast<TQAction*>(o) )
		    continue;
		// ### fix it for nested actiongroups
		if ( ::tqt_cast<TQDesignerAction*>(o) ) {
		    TQDesignerAction *ac = (TQDesignerAction*)o;
		    toolBar->insertAction( ac->widget(), ac );
		    ac->widget()->installEventFilter( toolBar );
		    if ( index == -1 )
			toolBar->appendAction( ac );
		    else
			toolBar->insertAction( index + (i++), ac );
		}
		TQObject::connect( o, TQ_SIGNAL( destroyed() ), toolBar, TQ_SLOT( actionRemoved() ) );
	    }
	}
	toolBar->reInsert();
	TQObject::connect( action, TQ_SIGNAL( destroyed() ), toolBar, TQ_SLOT( actionRemoved() ) );
    }
    formWindow()->mainWindow()->objectHierarchy()->rebuild();
}

void AddActionToToolBarCommand::unexecute()
{
    if ( ::tqt_cast<TQDesignerAction*>(action) ) {
	TQString s = ( (TQDesignerAction*)action )->widget()->name();
	s.prepend( "qt_dead_widget_" );
	( (TQDesignerAction*)action )->widget()->setName( s );
    }

    toolBar->removeAction( action );
    action->removeFrom( toolBar );
    TQObject::disconnect( action, TQ_SIGNAL( destroyed() ), toolBar, TQ_SLOT( actionRemoved() ) );
    if ( !::tqt_cast<TQActionGroup*>(action) || ( (TQActionGroup*)action )->usesDropDown()) {
	action->removeEventFilter( toolBar );
    } else {
	if ( action->children() ) {
	    TQObjectListIt it( *action->children() );
	    while ( it.current() ) {
		TQObject *o = it.current();
		++it;
		if ( !::tqt_cast<TQAction*>(o) )
		    continue;
		if ( ::tqt_cast<TQDesignerAction*>(o) ) {
		    o->removeEventFilter( toolBar );
		    toolBar->removeAction( (TQAction*)o );
		}
		TQObject::disconnect( o, TQ_SIGNAL( destroyed() ), toolBar, TQ_SLOT( actionRemoved() ) );
	    }
	}
    }
    formWindow()->mainWindow()->objectHierarchy()->rebuild();
}

// ------------------------------------------------------------

AddToolBarCommand::AddToolBarCommand( const TQString &n, FormWindow *fw, TQMainWindow *mw )
    : Command( n, fw ), toolBar( 0 ), mainWindow( mw )
{
}

void AddToolBarCommand::execute()
{
    if ( !toolBar ) {
	toolBar = new TQDesignerToolBar( mainWindow );
	TQString n = "Toolbar";
	formWindow()->unify( toolBar, n, true );
	toolBar->setName( n );
	mainWindow->addToolBar( toolBar, n );
    } else {
	toolBar->show();
	TQString s = toolBar->name();
	s.remove( 0, TQString( "qt_dead_widget_" ).length() );
	toolBar->setName( s );
    }
    formWindow()->mainWindow()->objectHierarchy()->rebuild();
}

void AddToolBarCommand::unexecute()
{
    toolBar->hide();
    TQString s = toolBar->name();
    s.prepend( "qt_dead_widget_" );
    toolBar->setName( s );
    formWindow()->mainWindow()->objectHierarchy()->rebuild();
}

// ------------------------------------------------------------

#ifdef QT_CONTAINER_CUSTOM_WIDGETS
AddContainerPageCommand::AddContainerPageCommand( const TQString &n, FormWindow *fw,
						  TQWidget *c, const TQString &label )
    : Command( n, fw ), container( c ), index( -1 ), pageLabel( label ), page( 0 )
{
    WidgetInterface *iface = 0;
    widgetManager()->queryInterface( WidgetFactory::classNameOf( container ), &iface );
    if ( !iface )
	return;
    iface->queryInterface( IID_QWidgetContainer, (TQUnknownInterface**)&wiface );
    if ( !wiface )
	return;
    wClassName = WidgetFactory::classNameOf( container );
}

AddContainerPageCommand::~AddContainerPageCommand()
{
    if ( wiface )
	wiface->release();
}

void AddContainerPageCommand::execute()
{
    if ( !wiface || !wiface->supportsPages( wClassName ) )
	return;
    if ( index == -1 )
	index = wiface->count( wClassName, container );
    if ( !page )
	page = wiface->addPage( wClassName, container, pageLabel, index );
    else
	wiface->insertPage( wClassName, container, pageLabel, index, page );

    MetaDataBase::addEntry( page );
    formWindow()->emitUpdateProperties( formWindow()->currentWidget() );
    // #### show and update pages in object hierarchy view
}

void AddContainerPageCommand::unexecute()
{
    if ( !wiface || !wiface->supportsPages( wClassName ) )
	return;
    wiface->removePage( wClassName, container, index );
    formWindow()->emitUpdateProperties( formWindow()->currentWidget() );
    // #### show and update pages in object hierarchy view
}

// ------------------------------------------------------------

DeleteContainerPageCommand::DeleteContainerPageCommand( const TQString &n, FormWindow *fw,
							TQWidget *c, int idx )
    : Command( n, fw ), container( c ), index( idx )
{
    WidgetInterface *iface = 0;
    widgetManager()->queryInterface( WidgetFactory::classNameOf( container ), &iface );
    if ( !iface )
	return;
    iface->queryInterface( IID_QWidgetContainer, (TQUnknownInterface**)&wiface );
    if ( !wiface )
	return;
    wClassName = WidgetFactory::classNameOf( container );
    page = wiface->page( wClassName, container, index );
    pageLabel = wiface->pageLabel( wClassName, container, index );
}

DeleteContainerPageCommand::~DeleteContainerPageCommand()
{
    if ( wiface )
	wiface->release();
}

void DeleteContainerPageCommand::execute()
{
    if ( !wiface || !wiface->supportsPages( wClassName ) )
	return;

    wiface->removePage( wClassName, container, index );
    formWindow()->emitUpdateProperties( formWindow()->currentWidget() );
    // #### show and update pages in object hierarchy view
}

void DeleteContainerPageCommand::unexecute()
{
    if ( !wiface || !wiface->supportsPages( wClassName ) )
	return;
    if ( index == -1 )
	index = wiface->count( wClassName, container );

    wiface->insertPage( wClassName, container, pageLabel, index, page );
    formWindow()->emitUpdateProperties( formWindow()->currentWidget() );
    // #### show and update pages in object hierarchy view
}

// ------------------------------------------------------------

RenameContainerPageCommand::RenameContainerPageCommand( const TQString &n, FormWindow *fw,
							TQWidget *c, int idx,
							const TQString &label )
    : Command( n, fw ), container( c ), index( idx ), newLabel( label )
{
    WidgetInterface *iface = 0;
    widgetManager()->queryInterface( WidgetFactory::classNameOf( container ), &iface );
    if ( !iface )
	return;
    iface->queryInterface( IID_QWidgetContainer, (TQUnknownInterface**)&wiface );
    if ( !wiface )
	return;
    wClassName = WidgetFactory::classNameOf( container );
    oldLabel = wiface->pageLabel( wClassName, container, index );
}

RenameContainerPageCommand::~RenameContainerPageCommand()
{
    if ( wiface )
	wiface->release();
}

void RenameContainerPageCommand::execute()
{
    if ( !wiface || !wiface->supportsPages( wClassName ) )
	return;
    wiface->renamePage( wClassName, container, index, newLabel );
    formWindow()->emitUpdateProperties( formWindow()->currentWidget() );
    // #### show and update pages in object hierarchy view
}

void RenameContainerPageCommand::unexecute()
{
    if ( !wiface || !wiface->supportsPages( wClassName ) )
	return;
    wiface->renamePage( wClassName, container, index, oldLabel );
    formWindow()->emitUpdateProperties( formWindow()->currentWidget() );
    // #### show and update pages in object hierarchy view
}
#endif // QT_CONTAINER_CUSTOM_WIDGETS
// ------------------------------------------------------------

AddActionToPopupCommand::AddActionToPopupCommand( const TQString &n,
						  FormWindow *fw,
						  PopupMenuEditor *m,
						  PopupMenuEditorItem *i,
						  int idx )
    : Command( n, fw ), menu( m ), item( i ), index( idx )
{ }

void AddActionToPopupCommand::execute()
{
    menu->insert( item, index );
    formWindow()->mainWindow()->objectHierarchy()->rebuild();
}

void AddActionToPopupCommand::unexecute()
{
    item->hideMenu();
    int i = menu->find( item->action() );
    menu->remove( i );
    formWindow()->mainWindow()->objectHierarchy()->rebuild();
}

// ------------------------------------------------------------

RemoveActionFromPopupCommand::RemoveActionFromPopupCommand( const TQString &n,
							    FormWindow *fw,
							    PopupMenuEditor *m,
							    int idx )
    : AddActionToPopupCommand( n, fw, m, 0, idx )
{
    item = menu->at( index );
}

void RemoveActionFromPopupCommand::execute()
{
    AddActionToPopupCommand::unexecute();
}

void RemoveActionFromPopupCommand::unexecute()
{
    AddActionToPopupCommand::execute();
}

// ------------------------------------------------------------

ExchangeActionInPopupCommand::ExchangeActionInPopupCommand( const TQString &n,
							    FormWindow *fw,
							    PopupMenuEditor *m,
							    int a,
							    int b )
    : Command( n, fw ), menu( m ), c( a ), d( b )
{ }

void ExchangeActionInPopupCommand::execute()
{
    menu->exchange( c, d );
}

void ExchangeActionInPopupCommand::unexecute()
{
    execute();
}

// ------------------------------------------------------------

ActionEditor *ActionCommand::actionEditor()
{
    // FIXME: handle multiple action editors
    return (ActionEditor *) formWindow()->mainWindow()->child( 0, "ActionEditor" );
}

// ------------------------------------------------------------

RenameActionCommand::RenameActionCommand( const TQString &n,
					  FormWindow *fw,
					  TQAction *a,
					  PopupMenuEditor *m,
					  TQString nm )
    : ActionCommand( n, fw, a ), menu( m ), newName( nm )
{
    oldName = action->menuText();
}

void RenameActionCommand::execute()
{
    TQString actionText = newName;
    actionText.replace("&&", "&");
    TQString menuText = newName;
    action->setText( actionText );
    action->setMenuText( menuText );

    formWindow()->mainWindow()->objectHierarchy()->rebuild();
}

void RenameActionCommand::unexecute()
{
    action->setMenuText( oldName );
    action->setText( oldName );
    formWindow()->mainWindow()->objectHierarchy()->rebuild();
}

// ------------------------------------------------------------

SetActionIconsCommand::SetActionIconsCommand( const TQString &n,
					      FormWindow *fw,
					      TQAction *a,
					      PopupMenuEditor *m,
					      TQIconSet &icons )
    : ActionCommand( n, fw, a ), menu( m ), newIcons( icons )
{
    oldIcons = a->iconSet();
}

void SetActionIconsCommand::execute()
{
    action->setIconSet( newIcons );
    ActionEditor *ae = actionEditor();
    if ( ae )
	ae->updateActionIcon( action );
    MetaDataBase::setPropertyChanged( action, "iconSet", true );
}

void SetActionIconsCommand::unexecute()
{
    action->setIconSet( oldIcons );
    ActionEditor *ae = actionEditor();
    if ( ae )
	ae->updateActionIcon( action );
    MetaDataBase::setPropertyChanged( action, "iconSet", true );
}

// ------------------------------------------------------------

AddMenuCommand::AddMenuCommand( const TQString &n,
				FormWindow *fw,
				MenuBarEditor *b,
				MenuBarEditorItem *i,
				int idx )
    : Command( n, fw ), mb( b ), item( i ), name( 0 ), index( idx )
{ }

AddMenuCommand::AddMenuCommand( const TQString &n,
				FormWindow *fw,
				TQMainWindow *mw,
				const TQString &nm )
    : Command( n, fw ), mb( 0 ), item( 0 ), name( nm ), index( -1 )
{
    if ( mw )
	mb = (MenuBarEditor *)mw->child( 0, "MenuBarEditor" );
}

void AddMenuCommand::execute()
{
    TQString n;
    TQMainWindow *mw = (TQMainWindow*)formWindow()->mainContainer();
    if ( !mb ) {
	mb = new MenuBarEditor( formWindow(), mw );
	mb->setName( "MenuBarEditor" );
	formWindow()->insertWidget( mb, true );
    }
    if ( !item ) {
	PopupMenuEditor *popup = new PopupMenuEditor( formWindow(), mw );
	popup->setName( "PopupMenuEditor" );
	formWindow()->insertWidget( popup, true );
	mb->insertItem( name, popup, index );
	index = mb->findItem( popup );
	item = mb->item( index );
    } else {
	PopupMenuEditor *popup = item->menu();
	popup->setName( item->menuText() );
	formWindow()->insertWidget( popup, true );
	mb->insertItem( item, index );
    }
    formWindow()->mainWindow()->objectHierarchy()->rebuild();
}

void AddMenuCommand::unexecute()
{
    if ( !item ) {
	tqWarning( "The AddMenuCommand was created without a menu item." );
	return;
    }
    item->menu()->hide();
    int i = mb->findItem( item );
    formWindow()->removeWidget( item->menu() );
    mb->removeItemAt( i );
    formWindow()->mainWindow()->objectHierarchy()->rebuild();
}

// ------------------------------------------------------------

RemoveMenuCommand::RemoveMenuCommand( const TQString &n,
				      FormWindow *fw,
				      MenuBarEditor *b,
				      int idx )
    : AddMenuCommand( n, fw, b, 0, idx )
{
    item = mb->item( index );
}

void RemoveMenuCommand::execute()
{
    mb->hideItem( index );
    AddMenuCommand::unexecute();
}

void RemoveMenuCommand::unexecute()
{
    AddMenuCommand::execute();
}

// ------------------------------------------------------------

ExchangeMenuCommand::ExchangeMenuCommand( const TQString &n,
					  FormWindow *fw,
					  MenuBarEditor *b,
					  int i,
					  int j )
    : Command( n, fw ), bar( b ), k( i ), l( j )
{ }

void ExchangeMenuCommand::execute()
{
    bar->exchange( k, l );
}

void ExchangeMenuCommand::unexecute()
{
    execute();
}

// ------------------------------------------------------------

MoveMenuCommand::MoveMenuCommand( const TQString &n,
				  FormWindow *fw,
				  MenuBarEditor *b,
				  int i,
				  int j )
    : Command( n, fw ), bar( b ), from( i ), to( j )
{ }

void MoveMenuCommand::execute()
{
    bar->hideItem( from );
    MenuBarEditorItem * i = bar->item( from );
    bar->removeItemAt( from );
    int t = ( from > to ? to : to - 1 );
    bar->insertItem( i, t );
}

void MoveMenuCommand::unexecute()
{
    bar->hideItem( to );
    int t = ( from > to ? to : to - 1 );
    MenuBarEditorItem * i = bar->item( t );
    bar->removeItemAt( t );
    bar->insertItem( i, from );
}

// ------------------------------------------------------------

RenameMenuCommand::RenameMenuCommand( const TQString &n,
				      FormWindow *fw,
				      MenuBarEditor *b,
				      TQString nm,
				      MenuBarEditorItem *i )
    : Command( n, fw ), bar( b ), item( i ), newName( nm )
{
    oldName = item->menu()->name();
}

TQString RenameMenuCommand::makeLegal( const TQString &str )
{
    // remove illegal characters
    TQString d;
    char c = 0, i = 0;
    while ( !str.at(i).isNull() ) {
	c = str.at(i).latin1();
	if ( c == '-' || c == ' ' )
	    d += '_';
	else if ( ( c >= '0' && c <= '9') || ( c >= 'A' && c <= 'Z' )
                  || c == '_' || ( c >= 'a' && c <= 'z' ) )
	    d += c;
	++i;
    }
    return d;
}

void RenameMenuCommand::execute()
{
    PopupMenuEditor *popup = item->menu();
    item->setMenuText( newName );
    TQString legal = makeLegal( newName );
    formWindow()->unify( popup, legal, true );
    popup->setName( legal );
    formWindow()->mainWindow()->objectHierarchy()->rebuild();
}

void RenameMenuCommand::unexecute()
{
    PopupMenuEditor *popup = item->menu();
    item->setMenuText( oldName );
    popup->setName( oldName );
    formWindow()->mainWindow()->objectHierarchy()->rebuild();
}

// ------------------------------------------------------------

AddToolBoxPageCommand::AddToolBoxPageCommand( const TQString &n, FormWindow *fw,
				      TQToolBox *tw, const TQString &label )
    : Command( n, fw ), toolBox( tw ), toolBoxLabel( label )
{
    toolBoxPage = new TQDesignerWidget( formWindow(), toolBox, "page" );
    toolBoxPage->hide();
    index = -1;
    MetaDataBase::addEntry( toolBoxPage );
}

void AddToolBoxPageCommand::execute()
{
    index = toolBox->insertItem( index, toolBoxPage, toolBoxLabel);
    toolBox->setCurrentIndex( index );
    formWindow()->emitUpdateProperties( formWindow()->currentWidget() );
    formWindow()->mainWindow()->objectHierarchy()->rebuild();
}

void AddToolBoxPageCommand::unexecute()
{
    toolBox->removeItem( toolBoxPage );
    toolBoxPage->hide();
    formWindow()->emitUpdateProperties( formWindow()->currentWidget() );
    formWindow()->mainWindow()->objectHierarchy()->rebuild();
}

// ------------------------------------------------------------

DeleteToolBoxPageCommand::DeleteToolBoxPageCommand( const TQString &n, FormWindow *fw,
					    TQToolBox *tw, TQWidget *page )
    : Command( n, fw ), toolBox( tw ), toolBoxPage( page )
{
    toolBoxLabel = toolBox->itemLabel( toolBox->currentIndex() );
    index = toolBox->currentIndex();
}

void DeleteToolBoxPageCommand::execute()
{
    toolBox->removeItem( toolBoxPage );
    toolBoxPage->hide();
    formWindow()->emitUpdateProperties( formWindow()->currentWidget() );
    formWindow()->mainWindow()->objectHierarchy()->rebuild();
}

void DeleteToolBoxPageCommand::unexecute()
{
    index = toolBox->insertItem( index, toolBoxPage, toolBoxLabel );
    toolBox->setCurrentIndex( index );
    formWindow()->emitUpdateProperties( formWindow()->currentWidget() );
    formWindow()->mainWindow()->objectHierarchy()->rebuild();
}
