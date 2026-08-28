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

#include "customwidgeteditorimpl.h"
#include "defs.h"
#include "pixmapchooser.h"
#include "mainwindow.h"
#include "asciivalidator.h"
#include "resource.h"
#include "widgetfactory.h"
#include "widgetdatabase.h"

#include <ntqlistbox.h>
#include <ntqpushbutton.h>
#include <ntqlineedit.h>
#include <ntqcombobox.h>
#include <ntqspinbox.h>
#include <ntqlabel.h>
#include <ntqmessagebox.h>
#include <ntqfiledialog.h>
#include <ntqmessagebox.h>
#include <ntqtimer.h>
#include <ntqapplication.h>
#include <ntqlistview.h>
#include <ntqfile.h>
#include <ntqtextstream.h>
#include <ntqdom.h>
#include <ntqtextcodec.h>
#include <ntqcheckbox.h>

CustomWidgetEditor::CustomWidgetEditor( TQWidget *parent, MainWindow *mw )
    : CustomWidgetEditorBase( parent, 0, true ), mainWindow( mw )
{
    connect( helpButton, TQ_SIGNAL( clicked() ), MainWindow::self, TQ_SLOT( showDialogHelp() ) );
    checkTimer = new TQTimer( this );
    connect( checkTimer, TQ_SIGNAL( timeout() ),
	     this, TQ_SLOT( checkWidgetName() ) );

    editClass->setEnabled( false );
    editHeader->setEnabled( false );
    buttonChooseHeader->setEnabled( false );
    buttonChoosePixmap->setEnabled( false );
    spinWidth->setEnabled( false );
    spinHeight->setEnabled( false );
    sizeHor->setEnabled( false );
    sizeVer->setEnabled( false );
    checkContainer->setEnabled( false );
    localGlobalCombo->setEnabled( false );
    editClass->setValidator( new AsciiValidator( TQString(":"), editClass ) );
    editSignal->setValidator( new AsciiValidator( true, editSignal ) );
    editSlot->setValidator( new AsciiValidator( true, editSignal ) );
    editProperty->setValidator( new AsciiValidator( editSignal ) );
    editSignal->setEnabled( false );
    buttonRemoveSignal->setEnabled( false );

    setupDefinition();
    setupSignals();
    setupSlots();
    setupProperties();
}

void CustomWidgetEditor::setupDefinition()
{
    TQPtrList<MetaDataBase::CustomWidget> *lst = MetaDataBase::customWidgets();
    for ( MetaDataBase::CustomWidget *w = lst->first(); w; w = lst->next() ) {
	TQListBoxItem *i;
	if ( w->pixmap )
	    i = new TQListBoxPixmap( boxWidgets, *w->pixmap, w->className );
	else
	    i = new TQListBoxText( boxWidgets, w->className );
	customWidgets.insert( i, w );
    }

    if ( boxWidgets->firstItem() ) {
	boxWidgets->setCurrentItem( boxWidgets->firstItem() );
	boxWidgets->setSelected( boxWidgets->firstItem(), true );
    }
    oldItem = 0;
}

void CustomWidgetEditor::setupSignals()
{
    editSignal->setEnabled( false );
    buttonRemoveSignal->setEnabled( false );
    MetaDataBase::CustomWidget *w = findWidget( boxWidgets->item( boxWidgets->currentItem() ) );
    if ( !w )
	return;
    listSignals->clear();
    for ( TQValueList<TQCString>::Iterator it = w->lstSignals.begin(); it != w->lstSignals.end(); ++it )
	listSignals->insertItem( TQString( *it ) );
    if ( listSignals->firstItem() ) {
	listSignals->setCurrentItem( listSignals->firstItem() );
	listSignals->setSelected( listSignals->firstItem(), true );
    }
}

void CustomWidgetEditor::setupSlots()
{
    editSlot->setEnabled( false );
    comboAccess->setEnabled( false );
    buttonRemoveSlot->setEnabled( false );

    MetaDataBase::CustomWidget *w = findWidget( boxWidgets->item( boxWidgets->currentItem() ) );
    if ( !w )
	return;
    listSlots->clear();
    for ( TQValueList<MetaDataBase::Function>::Iterator it = w->lstSlots.begin(); it != w->lstSlots.end(); ++it )
	(void)new TQListViewItem( listSlots, (*it).function, (*it).access );

    if ( listSlots->firstChild() ) {
	listSlots->setCurrentItem( listSlots->firstChild() );
	listSlots->setSelected( listSlots->firstChild(), true );
    }
}

void CustomWidgetEditor::setupProperties()
{
    editProperty->setEnabled( false );
    comboType->setEnabled( false );
    buttonRemoveProperty->setEnabled( false );

    MetaDataBase::CustomWidget *w = findWidget( boxWidgets->item( boxWidgets->currentItem() ) );
    if ( !w )
	return;
    listProperties->clear();
    for ( TQValueList<MetaDataBase::Property>::Iterator it = w->lstProperties.begin(); it != w->lstProperties.end(); ++it )
	(void)new TQListViewItem( listProperties, (*it).property, (*it).type );

    if ( listProperties->firstChild() ) {
	listProperties->setCurrentItem( listProperties->firstChild() );
	listProperties->setSelected( listProperties->firstChild(), true );
    }
}

void CustomWidgetEditor::currentWidgetChanged( TQListBoxItem *i )
{
    checkTimer->stop();
    checkWidgetName();

    MetaDataBase::CustomWidget *w = findWidget( i );
    if ( !i || !w ) {
	buttonChooseHeader->setEnabled( false );
	editClass->setEnabled( false );
	editHeader->setEnabled( false );
	buttonChoosePixmap->setEnabled( false );
	spinWidth->setEnabled( false );
	spinHeight->setEnabled( false );
	localGlobalCombo->setEnabled( false );
	sizeHor->setEnabled( false );
	sizeVer->setEnabled( false );
	checkContainer->setEnabled( false );
	return;
    }

    buttonChooseHeader->setEnabled( true );
    editClass->setEnabled( true );
    editHeader->setEnabled( true );
    buttonChoosePixmap->setEnabled( true );
    spinWidth->setEnabled( true );
    spinHeight->setEnabled( true );
    localGlobalCombo->setEnabled( true );
    sizeHor->setEnabled( true );
    sizeVer->setEnabled( true );
    checkContainer->setEnabled( true );

    editClass->blockSignals( true );
    editClass->setText( w->className );
    editClass->blockSignals( false );
    editHeader->setText( w->includeFile );
    localGlobalCombo->setCurrentItem( (int)w->includePolicy );
    if ( w->pixmap )
	previewPixmap->setPixmap( *w->pixmap );
    else
	previewPixmap->setText( "" );
    spinWidth->setValue( w->sizeHint.width() );
    spinHeight->setValue( w->sizeHint.height() );
    sizeHor->setCurrentItem( size_type_to_int( w->sizePolicy.horData() ) );
    sizeVer->setCurrentItem( size_type_to_int( w->sizePolicy.verData() ) );
    checkContainer->setChecked( w->isContainer );

    setupSignals();
    setupSlots();
    setupProperties();
}

void CustomWidgetEditor::addWidgetClicked()
{
    oldItem = 0;
    checkTimer->stop();
    checkWidgetName();

    MetaDataBase::CustomWidget *w = new MetaDataBase::CustomWidget;
    TQString s = w->className;
    if ( !MetaDataBase::addCustomWidget( w ) ) {
	TQMessageBox::information( this, tr( "Adding a Custom Widget" ),
				  tr( "Custom widget names must be unique.\n"
				      "A custom widget called '%1' already exists, so it is not possible "
				      "to add another widget with this name." ).arg( s ) );
	return;
    }

    TQListBoxPixmap *i = new TQListBoxPixmap( boxWidgets, *w->pixmap, w->className );
    customWidgets.insert( i, w );

    boxWidgets->setCurrentItem( i );
    boxWidgets->setSelected( i, true );
}

void CustomWidgetEditor::classNameChanged( const TQString &s )
{
    TQListBoxItem *i = boxWidgets->item( boxWidgets->currentItem() );
    MetaDataBase::CustomWidget *w = findWidget( i );
    if ( !i || !w )
	return;

    WidgetDatabase::customWidgetClassNameChanged( w->className, s );

    checkTimer->stop();
    boxWidgets->blockSignals( true );
    oldName = w->className;
    w->className = s;
    TQListBoxItem *old = i;
    if ( w->pixmap )
	boxWidgets->changeItem( *w->pixmap, s, boxWidgets->currentItem() );
    else
	boxWidgets->changeItem( s, boxWidgets->currentItem() );
    i = boxWidgets->item( boxWidgets->currentItem() );
    oldItem = i;
    customWidgets.insert( i, w );
    customWidgets.remove( old );
    boxWidgets->blockSignals( false );
    checkTimer->start( 1000, true );
}

void CustomWidgetEditor::deleteWidgetClicked()
{
    oldItem = 0;
    checkTimer->stop();
    checkWidgetName();

    TQListBoxItem *i = boxWidgets->item( boxWidgets->currentItem() );
    MetaDataBase::CustomWidget *w = findWidget( i );

    if ( mainWindow->isCustomWidgetUsed( w ) ) {
	TQMessageBox::information( mainWindow, tr( "Removing Custom Widget" ),
				  tr( "The custom widget '%1' is in use, so it cannot be removed." ).
				  arg( w->className ) );
	return;
    }

    if ( !i || !w )
	return;

    MetaDataBase::CustomWidget *cw = MetaDataBase::customWidget( mainWindow->currentTool() );
    if ( cw == w )
	mainWindow->resetTool();

    MetaDataBase::removeCustomWidget( w );
    customWidgets.remove( i );
    delete i;

    i = boxWidgets->item( boxWidgets->currentItem() );
    if ( i ) {
	boxWidgets->setCurrentItem( i );
	boxWidgets->setSelected( i, true );
    }
}

void CustomWidgetEditor::headerFileChanged( const TQString &s )
{
    TQListBoxItem *i = boxWidgets->item( boxWidgets->currentItem() );
    MetaDataBase::CustomWidget *w = findWidget( i );
    if ( !i || !w )
	return;

    w->includeFile = s;
}

void CustomWidgetEditor::heightChanged( int h )
{
    TQListBoxItem *i = boxWidgets->item( boxWidgets->currentItem() );
    MetaDataBase::CustomWidget *w = findWidget( i );
    if ( !i || !w )
	return;

    w->sizeHint.setHeight( h );
    updateCustomWidgetSizes();
}

void CustomWidgetEditor::includePolicyChanged( int p )
{
    TQListBoxItem *i = boxWidgets->item( boxWidgets->currentItem() );
    MetaDataBase::CustomWidget *w = findWidget( i );
    if ( !i || !w )
	return;

    w->includePolicy = (MetaDataBase::CustomWidget::IncludePolicy)p;
}

void CustomWidgetEditor::pixmapChoosen()
{
    TQListBoxItem *i = boxWidgets->item( boxWidgets->currentItem() );
    MetaDataBase::CustomWidget *w = findWidget( i );
    if ( !i || !w )
	return;

    TQPixmap pix = qChoosePixmap( this );
    if ( pix.isNull() )
	return;
    delete w->pixmap;
    w->pixmap = new TQPixmap( pix );

    boxWidgets->blockSignals( true );
    TQListBoxItem *old = i;
    boxWidgets->changeItem( *w->pixmap, w->className, boxWidgets->currentItem() );
    i = boxWidgets->item( boxWidgets->currentItem() );
    customWidgets.insert( i, w );
    customWidgets.remove( old );
    boxWidgets->blockSignals( false );
    previewPixmap->setPixmap( *w->pixmap );
}

void CustomWidgetEditor::widthChanged( int wid )
{
    TQListBoxItem *i = boxWidgets->item( boxWidgets->currentItem() );
    MetaDataBase::CustomWidget *w = findWidget( i );
    if ( !i || !w )
	return;

    w->sizeHint.setWidth( wid );
    updateCustomWidgetSizes();
}

MetaDataBase::CustomWidget *CustomWidgetEditor::findWidget( TQListBoxItem *i )
{
    if ( !i )
	return 0;

    TQMap<TQListBoxItem*, MetaDataBase::CustomWidget*>::Iterator it = customWidgets.find( i );
    if ( it == customWidgets.end() )
	return 0;
    return *it;
}

void CustomWidgetEditor::chooseHeader()
{
    TQListBoxItem *i = boxWidgets->item( boxWidgets->currentItem() );
    MetaDataBase::CustomWidget *w = findWidget( i );
    if ( !i || !w )
	return;

    TQString h = TQFileDialog::getOpenFileName( TQString::null, tr( "Header Files (*.h *.h++ *.hxx *.hh)" ), this );
    if ( h.isEmpty() )
	return;
    editHeader->setText( h );
    localGlobalCombo->setCurrentItem( (int)MetaDataBase::CustomWidget::Global );
}

void CustomWidgetEditor::checkWidgetName()
{
    TQListBoxItem *i = oldItem ? oldItem : boxWidgets->item( boxWidgets->currentItem() );
    MetaDataBase::CustomWidget *w = findWidget( i );
    oldItem = 0;
    if ( !i || !w )
	return;

    if ( MetaDataBase::isWidgetNameUsed( w ) ) {
	TQString s = w->className;
	w->className = oldName;
	TQMessageBox::information( this, tr( "Renaming a Custom Widget" ),
				  tr( "Custom widget names must be unique.\n"
				      "A custom widget called '%1' already exists, so it is not possible "
				      "to rename this widget with this name." ).arg( s ) );
	if ( i != boxWidgets->item( boxWidgets->currentItem() ) ) {
	    boxWidgets->setCurrentItem( i );
	    tqApp->processEvents();
	}
	editClass->setText( w->className );
	classNameChanged( w->className );
    }
}

void CustomWidgetEditor::closeClicked()
{
    oldItem = 0;
    checkTimer->stop();
    checkWidgetName();
    accept();
}

void CustomWidgetEditor::currentSignalChanged( TQListBoxItem *i )
{
    editSignal->blockSignals( true );
    editSignal->setText( "" );
    editSignal->blockSignals( false );

    if ( !i ) {
	editSignal->setEnabled( false );
	buttonRemoveSignal->setEnabled( false );
	return;
    }

    editSignal->blockSignals( true );
    editSignal->setEnabled( true );
    buttonRemoveSignal->setEnabled( true );
    editSignal->setText( i->text() );
    editSignal->blockSignals( false );
}

void CustomWidgetEditor::addSignal()
{
    TQListBoxItem *i = new TQListBoxText( listSignals, "signal()" );
    listSignals->setCurrentItem( i );
    listSignals->setSelected( i, true );
    MetaDataBase::CustomWidget *w = findWidget( boxWidgets->item( boxWidgets->currentItem() ) );
    if ( w )
	w->lstSignals.append( i->text().latin1() );
}

void CustomWidgetEditor::removeSignal()
{
    TQString s = listSignals->currentText();
    delete listSignals->item( listSignals->currentItem() );
    if ( listSignals->currentItem() != -1 )
	listSignals->setSelected( listSignals->currentItem(), true );
    MetaDataBase::CustomWidget *w = findWidget( boxWidgets->item( boxWidgets->currentItem() ) );
    if ( w )
	w->lstSignals.remove( s.latin1() );
}

void CustomWidgetEditor::signalNameChanged( const TQString &s )
{
    MetaDataBase::CustomWidget *w = findWidget( boxWidgets->item( boxWidgets->currentItem() ) );
    if ( !w || listSignals->currentItem() == -1 )
	return;

    TQValueList<TQCString>::Iterator it = w->lstSignals.find( listSignals->currentText().latin1() );
    if ( it != w->lstSignals.end() )
	w->lstSignals.remove( it );
    listSignals->blockSignals( true );
    listSignals->changeItem( s, listSignals->currentItem() );
    listSignals->blockSignals( false );
    w->lstSignals.append( s.latin1() );
}

void CustomWidgetEditor::slotAccessChanged( const TQString &s )
{
    MetaDataBase::CustomWidget *w = findWidget( boxWidgets->item( boxWidgets->currentItem() ) );
    if ( !w || !listSlots->currentItem() )
	return;

    MetaDataBase::Function slot;
    slot.function = listSlots->currentItem()->text( 0 );
    slot.access = listSlots->currentItem()->text( 1 );
    TQValueList<MetaDataBase::Function>::Iterator it = w->lstSlots.find( slot );
    if ( it != w->lstSlots.end() )
	w->lstSlots.remove( it );
    listSlots->currentItem()->setText( 1, s );
    slot.function = listSlots->currentItem()->text( 0 );
    slot.access = listSlots->currentItem()->text( 1 );
    w->lstSlots.append( slot );
}

void CustomWidgetEditor::slotNameChanged( const TQString &s )
{
    MetaDataBase::CustomWidget *w = findWidget( boxWidgets->item( boxWidgets->currentItem() ) );
    if ( !w || !listSlots->currentItem() )
	return;

    MetaDataBase::Function slot;
    slot.function = listSlots->currentItem()->text( 0 );
    slot.access = listSlots->currentItem()->text( 1 );
    slot.type = "slot";
    TQValueList<MetaDataBase::Function>::Iterator it = w->lstSlots.find( slot );
    if ( it != w->lstSlots.end() )
	w->lstSlots.remove( it );
    listSlots->currentItem()->setText( 0, s );
    slot.function = listSlots->currentItem()->text( 0 );
    slot.access = listSlots->currentItem()->text( 1 );
    w->lstSlots.append( slot );
}

void CustomWidgetEditor::addSlot()
{
    TQListViewItem *i = new TQListViewItem( listSlots, "slot()", "public" );
    listSlots->setCurrentItem( i );
    listSlots->setSelected( i, true );
    MetaDataBase::CustomWidget *w = findWidget( boxWidgets->item( boxWidgets->currentItem() ) );
    if ( w ) {
	MetaDataBase::Function slot;
	slot.function = "slot()";
	slot.access = "public";
	slot.type = "slot";
	w->lstSlots.append( slot );
    }
}

void CustomWidgetEditor::removeSlot()
{
    MetaDataBase::Function slot;
    slot.function = "1 2 3";
    if ( listSlots->currentItem() ) {
	slot.function = listSlots->currentItem()->text( 0 );
	slot.access = listSlots->currentItem()->text( 1 );
    }
    delete listSlots->currentItem();
    if ( listSlots->currentItem() )
	listSlots->setSelected( listSlots->currentItem(), true );
    MetaDataBase::CustomWidget *w = findWidget( boxWidgets->item( boxWidgets->currentItem() ) );
    if ( w && slot.function != "1 2 3" )
	w->lstSlots.remove( slot );
}

void CustomWidgetEditor::currentSlotChanged( TQListViewItem *i )
{
    editSlot->blockSignals( true );
    editSlot->setText( "" );
    editSignal->blockSignals( false );

    if ( !i ) {
	editSlot->setEnabled( false );
	comboAccess->setEnabled( false );
	buttonRemoveSlot->setEnabled( false );
	return;
    }

    editSlot->setEnabled( true );
    comboAccess->setEnabled( true );
    buttonRemoveSlot->setEnabled( true );
    editSlot->blockSignals( true );
    comboAccess->blockSignals( true );
    editSlot->setText( i->text( 0 ) );
    if ( i->text( 1 ) == tr( "protected" ) )
	comboAccess->setCurrentItem( 1 );
    else
	comboAccess->setCurrentItem( 0 );
    editSlot->blockSignals( false );
    comboAccess->blockSignals( false );
}

void CustomWidgetEditor::propertyTypeChanged( const TQString &s )
{
    MetaDataBase::CustomWidget *w = findWidget( boxWidgets->item( boxWidgets->currentItem() ) );
    if ( !w || !listProperties->currentItem() )
	return;

    MetaDataBase::Property property;
    property.property = listProperties->currentItem()->text( 0 );
    property.type = listProperties->currentItem()->text( 1 );
    TQValueList<MetaDataBase::Property>::Iterator it = w->lstProperties.find( property );
    if ( it != w->lstProperties.end() )
	w->lstProperties.remove( it );
    listProperties->currentItem()->setText( 1, s );
    property.property = listProperties->currentItem()->text( 0 );
    property.type = listProperties->currentItem()->text( 1 );
    w->lstProperties.append( property );
}

void CustomWidgetEditor::propertyNameChanged( const TQString &s )
{
    MetaDataBase::CustomWidget *w = findWidget( boxWidgets->item( boxWidgets->currentItem() ) );
    if ( !w || !listProperties->currentItem() )
	return;

    MetaDataBase::Property property;
    property.property = listProperties->currentItem()->text( 0 );
    property.type = listProperties->currentItem()->text( 1 );
    TQValueList<MetaDataBase::Property>::Iterator it = w->lstProperties.find( property );
    if ( it != w->lstProperties.end() )
	w->lstProperties.remove( it );
    listProperties->currentItem()->setText( 0, s );
    property.property = listProperties->currentItem()->text( 0 );
    property.type = listProperties->currentItem()->text( 1 );
    w->lstProperties.append( property );
}

void CustomWidgetEditor::addProperty()
{
    TQListViewItem *i = new TQListViewItem( listProperties, "property", "String" );
    listProperties->setCurrentItem( i );
    listProperties->setSelected( i, true );
    MetaDataBase::CustomWidget *w = findWidget( boxWidgets->item( boxWidgets->currentItem() ) );
    if ( w ) {
	MetaDataBase::Property prop;
	prop.property = "property";
	prop.type = "String";
	w->lstProperties.append( prop );
    }
}

void CustomWidgetEditor::removeProperty()
{
    MetaDataBase::Property property;
    property.property = "1 2 3";
    if ( listProperties->currentItem() ) {
	property.property = listProperties->currentItem()->text( 0 );
	property.type = listProperties->currentItem()->text( 1 );
    }
    delete listProperties->currentItem();
    if ( listProperties->currentItem() )
	listProperties->setSelected( listProperties->currentItem(), true );
    MetaDataBase::CustomWidget *w = findWidget( boxWidgets->item( boxWidgets->currentItem() ) );
    if ( w && property.property != "1 2 3" )
	w->lstProperties.remove( property );
}

void CustomWidgetEditor::currentPropertyChanged( TQListViewItem *i )
{
    editProperty->blockSignals( true );
    editProperty->setText( "" );
    editSignal->blockSignals( false );

    if ( !i ) {
	editProperty->setEnabled( false );
	comboType->setEnabled( false );
	buttonRemoveProperty->setEnabled( false );
	return;
    }

    editProperty->setEnabled( true );
    comboType->setEnabled( true );
    buttonRemoveProperty->setEnabled( true );
    editProperty->blockSignals( true );
    comboType->blockSignals( true );
    editProperty->setText( i->text( 0 ) );

    for ( int j = 0; j < comboType->count(); ++j ) {
	if ( i->text( 1 ) == comboType->text( j ) ) {
	    comboType->setCurrentItem( j );
	    break;
	}
    }
    editProperty->blockSignals( false );
    comboType->blockSignals( false );
}

static TQString makeIndent2( int indent )
{
    TQString s;
    s.fill( ' ', indent * 4 );
    return s;
}

static TQString entitize2( const TQString &s )
{
    TQString s2 = s;
    s2 = s2.replace( "\"", "&quot;" );
    s2 = s2.replace( "&", "&amp;" );
    s2 = s2.replace( ">", "&gt;" );
    s2 = s2.replace( "<", "&lt;" );
    s2 = s2.replace( "'", "&apos;" );
    return s2;
}

void CustomWidgetEditor::saveDescription()
{
    TQString fn = TQFileDialog::getSaveFileName( TQString::null, tr( "Custom-Widget Description (*.cw);;All Files (*)" ), this );
    if ( fn.isEmpty() )
	return;

    if ( TQFileInfo( fn ).extension() != "cw" )
	fn += ".cw";
    TQFile f( fn );
    if ( !f.open( IO_WriteOnly ) )
	return;

    TQTextStream ts( &f );
    ts.setCodec( TQTextCodec::codecForName( "UTF-8" ) );
    int indent = 0;

    ts << "<!DOCTYPE CW><CW>" << endl;

    ts << makeIndent2( indent ) << "<customwidgets>" << endl;
    indent++;

    TQPtrList<MetaDataBase::CustomWidget> *lst = MetaDataBase::customWidgets();
    for ( MetaDataBase::CustomWidget *w = lst->first(); w; w = lst->next() ) {
	ts << makeIndent2( indent ) << "<customwidget>" << endl;
	indent++;
	ts << makeIndent2( indent ) << "<class>" << w->className << "</class>" << endl;
	ts << makeIndent2( indent ) << "<header location=\""
	   << ( w->includePolicy == MetaDataBase::CustomWidget::Local ? "local" : "global" )
	   << "\">" << w->includeFile << "</header>" << endl;
	ts << makeIndent2( indent ) << "<sizehint>" << endl;
	indent++;
	ts << makeIndent2( indent ) << "<width>" << w->sizeHint.width() << "</width>" << endl;
	ts << makeIndent2( indent ) << "<height>" << w->sizeHint.height() << "</height>" << endl;
	indent--;
	ts << makeIndent2( indent ) << "</sizehint>" << endl;
	ts << makeIndent2( indent ) << "<container>" << (int)w->isContainer << "</container>" << endl;
	ts << makeIndent2( indent ) << "<sizepolicy>" << endl;
	indent++;
	ts << makeIndent2( indent ) << "<hordata>" << (int)w->sizePolicy.horData() << "</hordata>" << endl;
	ts << makeIndent2( indent ) << "<verdata>" << (int)w->sizePolicy.verData() << "</verdata>" << endl;
	indent--;
	ts << makeIndent2( indent ) << "</sizepolicy>" << endl;
	ts << makeIndent2( indent ) << "<pixmap>" << endl;
	indent++;
	Resource::saveImageData( w->pixmap->convertToImage(), ts, indent );
	indent--;
	ts << makeIndent2( indent ) << "</pixmap>" << endl;
	if ( !w->lstSignals.isEmpty() ) {
	    for ( TQValueList<TQCString>::Iterator it = w->lstSignals.begin(); it != w->lstSignals.end(); ++it )
		ts << makeIndent2( indent ) << "<signal>" << entitize2( *it ) << "</signal>" << endl;
	}
	if ( !w->lstSlots.isEmpty() ) {
	    for ( TQValueList<MetaDataBase::Function>::Iterator it = w->lstSlots.begin(); it != w->lstSlots.end(); ++it )
		ts << makeIndent2( indent ) << "<slot access=\"" << (*it).access << "\">" << entitize2( (*it).function ) << "</slot>" << endl;
	}
	if ( !w->lstProperties.isEmpty() ) {
	    for ( TQValueList<MetaDataBase::Property>::Iterator it = w->lstProperties.begin(); it != w->lstProperties.end(); ++it )
		ts << makeIndent2( indent ) << "<property type=\"" << (*it).type << "\">" << entitize2( (*it).property ) << "</property>" << endl;
	}
	indent--;
	ts << makeIndent2( indent ) << "</customwidget>" << endl;
    }

    indent--;
    ts << makeIndent2( indent ) << "</customwidgets>" << endl;
    ts << "</CW>" << endl;
}

void CustomWidgetEditor::loadDescription()
{
    TQString fn = TQFileDialog::getOpenFileName( TQString::null, tr( "Custom-Widget Description (*.cw);;All Files (*)" ), this );
    if ( fn.isEmpty() )
	return;

    TQFile f( fn );
    if ( !f.open( IO_ReadOnly ) )
	return;

    TQDomDocument doc;
    TQString errMsg;
    int errLine;
    if ( !doc.setContent( &f, &errMsg, &errLine ) ) {
	tqDebug( TQString("Parse error: ") + errMsg + TQString(" in line %d"), errLine );
	return;
    }

    TQDomElement firstWidget = doc.firstChild().toElement().firstChild().toElement();

    while ( firstWidget.tagName() != "customwidgets" )
	firstWidget = firstWidget.nextSibling().toElement();

    Resource::loadCustomWidgets( firstWidget, 0 );
    boxWidgets->clear();
    setupDefinition();
    setupSignals();
    setupSlots();
    setupProperties();
}

void CustomWidgetEditor::updateCustomWidgetSizes()
{
    if ( cwLst.isEmpty() )
	cwLst = *mainWindow->queryList( "CustomWidget" );
    for ( TQObject *o = cwLst.first(); o; o = cwLst.next() )
	( (TQWidget*)o )->updateGeometry();
}

void CustomWidgetEditor::horDataChanged( int a )
{
    TQSizePolicy::SizeType st = int_to_size_type( a );
    TQListBoxItem *i = boxWidgets->item( boxWidgets->currentItem() );
    MetaDataBase::CustomWidget *w = findWidget( i );
    if ( !i || !w )
	return;

    TQSizePolicy osp = w->sizePolicy;
    w->sizePolicy.setHorData( st );
    if ( cwLst.isEmpty() )
	cwLst = *mainWindow->queryList( "CustomWidget" );
    for ( TQObject *o = cwLst.first(); o; o = cwLst.next() ) {
	CustomWidget *cw = (CustomWidget*)o;
	if ( cw->realClassName() == boxWidgets->currentText() ) {
	    if ( cw->sizePolicy() == osp )
		cw->setSizePolicy( w->sizePolicy );
	}
    }
}

void CustomWidgetEditor::verDataChanged( int a )
{
    TQSizePolicy::SizeType st = int_to_size_type( a );
    TQListBoxItem *i = boxWidgets->item( boxWidgets->currentItem() );
    MetaDataBase::CustomWidget *w = findWidget( i );
    if ( !i || !w )
	return;

    TQSizePolicy osp = w->sizePolicy;
    w->sizePolicy.setVerData( st );
    if ( cwLst.isEmpty() )
	cwLst = *mainWindow->queryList( "CustomWidget" );
    for ( TQObject *o = cwLst.first(); o; o = cwLst.next() ) {
	CustomWidget *cw = (CustomWidget*)o;
	if ( cw->realClassName() == boxWidgets->currentText() ) {
	    if ( cw->sizePolicy() == osp )
		cw->setSizePolicy( w->sizePolicy );
	}
    }
}

void CustomWidgetEditor::widgetIsContainer( bool b )
{
    TQListBoxItem *i = boxWidgets->item( boxWidgets->currentItem() );
    MetaDataBase::CustomWidget *w = findWidget( i );
    if ( !i || !w )
	return;

    w->isContainer = b;
    WidgetDatabaseRecord *r = WidgetDatabase::at( w->id );
    if ( r )
	r->isContainer = b;
}
