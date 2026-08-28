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

#include <ntqvariant.h>

#include "propertyeditor.h"
#include "formwindow.h"
#include "command.h"
#include "metadatabase.h"
#include "propertyobject.h"
#include <widgetdatabase.h>
#include "widgetfactory.h"
#include "globaldefs.h"
#include "defs.h"
#include "asciivalidator.h"
#include "paletteeditorimpl.h"
#include "multilineeditorimpl.h"
#include "mainwindow.h"
#include "project.h"
#include "hierarchyview.h"
#include "database.h"
#include "menubareditor.h"

#include <ntqpainter.h>
#include <ntqpalette.h>
#include <ntqapplication.h>
#include <ntqheader.h>
#include <ntqlineedit.h>
#include <ntqstrlist.h>
#include <ntqmetaobject.h>
#include <ntqcombobox.h>
#include <ntqpushbutton.h>
#include <ntqhbox.h>
#include <ntqfontdialog.h>
#include <ntqspinbox.h>
#include <ntqevent.h>
#include <ntqobjectlist.h>
#include <ntqlistbox.h>
#include <ntqfontdatabase.h>
#include <ntqcolor.h>
#include <ntqcolordialog.h>
#include <ntqlabel.h>
#include <ntqlayout.h>
#include <ntqsizepolicy.h>
#include <ntqbitmap.h>
#include <ntqtooltip.h>
#include <ntqwhatsthis.h>
#include <ntqaccel.h>
#include <ntqworkspace.h>
#include <ntqtimer.h>
#include <ntqdragobject.h>
#include <ntqdom.h>
#include <ntqprocess.h>
#include <ntqstyle.h>
#include <ntqdatetimeedit.h>
#include <ntqassistantclient.h>
#include <ntqdrawutil.h>
#include <ntqmultilineedit.h> // FIXME: obsolete
#include <ntqsplitter.h>
#include <ntqdatatable.h>
#include <ntqtextview.h>

#include <limits.h>

static TQFontDatabase *fontDataBase = 0;
TQString assistantPath();

#ifdef TQ_WS_MAC
#include <ntqwindowsstyle.h>
static void setupStyle(TQWidget *w)
{
    static TQStyle *windowsStyle = 0;
    if(!windowsStyle)
	windowsStyle = new TQWindowsStyle;
    w->setStyle(windowsStyle);
}
#else
#define setupStyle(x)
#endif

static void cleanupFontDatabase()
{
    delete fontDataBase;
    fontDataBase = 0;
}

static TQStringList getFontList()
{
    if ( !fontDataBase ) {
	fontDataBase = new TQFontDatabase;
	tqAddPostRoutine( cleanupFontDatabase );
    }
    return fontDataBase->families();
}


class PropertyWhatsThis : public TQWhatsThis
{
public:
    PropertyWhatsThis( PropertyList *l );
    TQString text( const TQPoint &pos );
    bool clicked( const TQString& href );

private:
    PropertyList *propertyList;

};

PropertyWhatsThis::PropertyWhatsThis( PropertyList *l )
    : TQWhatsThis( l->viewport() ), propertyList( l )
{
}

TQString PropertyWhatsThis::text( const TQPoint &pos )
{
    return propertyList->whatsThisAt( pos );
}

bool PropertyWhatsThis::clicked( const TQString& href )
{
    if ( !href.isEmpty() ) {
	TQAssistantClient *ac = MainWindow::self->assistantClient();
	ac->showPage( TQString( tqInstallPathDocs() ) + "/html/" + href );
    }
    return false; // do not hide window
}


/*!
  \class PropertyItem propertyeditor.h
  \brief Base class for all property items

  This is the base class for each property item for the
  PropertyList. A simple property item has just a name and a value to
  provide an editor for a datatype. But more complex datatypes might
  provide an expandable item for editing single parts of the
  datatype. See hasSubItems(), initChildren() for that.
*/

/*!  If this item should be a child of another property item, specify
  \a prop as the parent item.
*/

PropertyItem::PropertyItem( PropertyList *l, PropertyItem *after, PropertyItem *prop, const TQString &propName )
    : TQListViewItem( l, after ), listview( l ), property( prop ), propertyName( propName )
{
    setSelectable( false );
    open = false;
    setText( 0, propertyName );
    changed = false;
    setText( 1, "" );
    resetButton = 0;
}

PropertyItem::~PropertyItem()
{
    if ( resetButton )
	delete resetButton->parentWidget();
    resetButton = 0;
}

void PropertyItem::toggle()
{
}

void PropertyItem::updateBackColor()
{
    if ( itemAbove() && this != listview->firstChild() ) {
	if ( ( ( PropertyItem*)itemAbove() )->backColor == *backColor1 )
	    backColor = *backColor2;
	else
	    backColor = *backColor1;
    } else {
	backColor = *backColor1;
    }
    if ( listview->firstChild() == this )
	backColor = *backColor1;
}

TQColor PropertyItem::backgroundColor()
{
    updateBackColor();
    if ( (TQListViewItem*)this == listview->currentItem() )
	return *selectedBack;
    return backColor;
}

/*!  If a subclass is a expandable item, this is called when the child
items should be created.
*/

void PropertyItem::createChildren()
{
}

/*!  If a subclass is a expandable item, this is called when the child
items should be initialized.
*/

void PropertyItem::initChildren()
{
}

void PropertyItem::paintCell( TQPainter *p, const TQColorGroup &cg, int column, int width, int align )
{
    TQColorGroup g( cg );
    g.setColor( TQColorGroup::Base, backgroundColor() );
    g.setColor( TQColorGroup::Foreground, TQt::black );
    g.setColor( TQColorGroup::Text, TQt::black );
    int indent = 0;
    if ( column == 0 ) {
	indent = 20 + ( property ? 20 : 0 );
	p->fillRect( 0, 0, width, height(), backgroundColor() );
	p->save();
	p->translate( indent, 0 );
    }

    if ( isChanged() && column == 0 ) {
	p->save();
	TQFont f = p->font();
	f.setBold( true );
	p->setFont( f );
    }

    if ( !hasCustomContents() || column != 1 ) {
	TQListViewItem::paintCell( p, g, column, width - indent, align  );
    } else {
	p->fillRect( 0, 0, width, height(), backgroundColor() );
	drawCustomContents( p, TQRect( 0, 0, width, height() ) );
    }

    if ( isChanged() && column == 0 )
	p->restore();
    if ( column == 0 )
	p->restore();
    if ( hasSubItems() && column == 0 ) {
	p->save();
	p->setPen( cg.foreground() );
	p->setBrush( cg.base() );
	p->drawRect( 5, height() / 2 - 4, 9, 9 );
	p->drawLine( 7, height() / 2, 11, height() / 2 );
	if ( !isOpen() )
	    p->drawLine( 9, height() / 2 - 2, 9, height() / 2 + 2 );
	p->restore();
    }
    p->save();
    p->setPen( TQPen( cg.dark(), 1 ) );
    p->drawLine( 0, height() - 1, width, height() - 1 );
    p->drawLine( width - 1, 0, width - 1, height() );
    p->restore();

    if ( listview->currentItem() == this && column == 0 &&
	 !listview->hasFocus() && !listview->viewport()->hasFocus() )
	paintFocus( p, cg, TQRect( 0, 0, width, height() ) );
}

void PropertyItem::paintBranches( TQPainter * p, const TQColorGroup & cg,
				  int w, int y, int h )
{
    TQColorGroup g( cg );
    g.setColor( TQColorGroup::Base, backgroundColor() );
    TQListViewItem::paintBranches( p, g, w, y, h );
}

void PropertyItem::paintFocus( TQPainter *p, const TQColorGroup &cg, const TQRect &r )
{
    p->save();
    TQApplication::style().drawPrimitive(TQStyle::PE_Panel, p, r, cg,
					TQStyle::Style_Sunken, TQStyleOption(1,1) );
    p->restore();
}

/*!  Subclasses which are expandable items have to return true
  here. Default is false.
*/

bool PropertyItem::hasSubItems() const
{
    return false;
}

/*!  Returns the parent property item here if this is a child or 0
 otherwise.
 */

PropertyItem *PropertyItem::propertyParent() const
{
    return property;
}

bool PropertyItem::isOpen() const
{
    return open;
}

void PropertyItem::setOpen( bool b )
{
    if ( b == open )
	return;
    open = b;

    if ( !open ) {
	children.setAutoDelete( true );
	children.clear();
	children.setAutoDelete( false );
	tqApp->processEvents();
	listview->updateEditorSize();
	return;
    }

    createChildren();
    initChildren();
    tqApp->processEvents();
    listview->updateEditorSize();
}

/*!  Subclasses have to show the editor of the item here
*/

void PropertyItem::showEditor()
{
    createResetButton();
    resetButton->parentWidget()->show();
}

/*!  Subclasses have to hide the editor of the item here
*/

void PropertyItem::hideEditor()
{
    createResetButton();
    resetButton->parentWidget()->hide();
}

/*!  This is called to init the value of the item. Reimplement in
  subclasses to init the editor
*/

void PropertyItem::setValue( const TQVariant &v )
{
    val = v;
}

TQVariant PropertyItem::value() const
{
    return val;
}

bool PropertyItem::isChanged() const
{
    return changed;
}

void PropertyItem::setChanged( bool b, bool updateDb )
{
    if ( propertyParent() )
	return;
    if ( changed == b )
	return;
    changed = b;
    repaint();
    if ( updateDb ) {
	MetaDataBase::setPropertyChanged( listview->propertyEditor()->widget(), name(), changed );
    }
    updateResetButtonState();
}

TQString PropertyItem::name() const
{
    return propertyName;
}

void PropertyItem::createResetButton()
{
    if ( resetButton ) {
	resetButton->parentWidget()->lower();
	return;
    }
    TQHBox *hbox = new TQHBox( listview->viewport() );
    hbox->setFrameStyle( TQFrame::StyledPanel | TQFrame::Sunken );
    hbox->setLineWidth( 1 );
    resetButton = new TQPushButton( hbox );
    setupStyle( resetButton );
    resetButton->setPixmap( TQPixmap::fromMimeSource( "designer_resetproperty.png" ) );
    resetButton->setFixedWidth( resetButton->sizeHint().width() );
    hbox->layout()->setAlignment( TQt::AlignRight );
    listview->addChild( hbox );
    hbox->hide();
    TQObject::connect( resetButton, TQ_SIGNAL( clicked() ),
		      listview, TQ_SLOT( resetProperty() ) );
    TQToolTip::add( resetButton, PropertyEditor::tr( "Reset the property to its default value" ) );
    TQWhatsThis::add( resetButton, PropertyEditor::tr( "Click this button to reset the property to its default value" ) );
    updateResetButtonState();
}

void PropertyItem::updateResetButtonState()
{
    if ( !resetButton )
	return;
    if ( propertyParent() || !WidgetFactory::canResetProperty( listview->propertyEditor()->widget(), name() ) )
	resetButton->setEnabled( false );
    else
	resetButton->setEnabled( isChanged() );
}

/*!  Call this to place/resize the item editor correctly (normally
  call it from showEditor())
*/

void PropertyItem::placeEditor( TQWidget *w )
{
    createResetButton();
    TQRect r = listview->itemRect( this );
    if ( !r.size().isValid() ) {
	listview->ensureItemVisible( this );
#if defined(TQ_WS_WIN)
	listview->repaintContents( false );
#endif
	r = listview->itemRect( this );
    }
    r.setX( listview->header()->sectionPos( 1 ) );
    r.setWidth( listview->header()->sectionSize( 1 ) - 1 );
    r.setWidth( r.width() - resetButton->width() - 2 );
    r = TQRect( listview->viewportToContents( r.topLeft() ), r.size() );
    w->resize( r.size() );
    listview->moveChild( w, r.x(), r.y() );
    resetButton->parentWidget()->resize( resetButton->sizeHint().width() + 10, r.height() );
    listview->moveChild( resetButton->parentWidget(), r.x() + r.width() - 8, r.y() );
    resetButton->setFixedHeight( TQMAX( 0, r.height() - 3 ) );
}

/*!  This should be called by subclasses if the user changed the value
  of the property and this value should be applied to the widget property
*/

void PropertyItem::notifyValueChange()
{
    if ( !propertyParent() ) {
	listview->valueChanged( this );
	setChanged( true );
	if ( hasSubItems() )
	    initChildren();
    } else {
	propertyParent()->childValueChanged( this );
	setChanged( true );
    }
}

/*!  If a subclass is a expandable item reimplement this as this is
  always called if a child item changed its value. So update the
  display of the item here then.
*/

void PropertyItem::childValueChanged( PropertyItem * )
{
}

/*!  When adding a child item, call this (normally from addChildren()
*/

void PropertyItem::addChild( PropertyItem *i )
{
    children.append( i );
}

int PropertyItem::childCount() const
{
    return children.count();
}

PropertyItem *PropertyItem::child( int i ) const
{
    // ARRRRRRRRG
    return ( (PropertyItem*)this )->children.at( i );
}

/*!  If the contents of the item is not displayable with a text, but
  you want to draw it yourself (using drawCustomContents()), return
  true here.
*/

bool PropertyItem::hasCustomContents() const
{
    return false;
}

/*!
  \sa hasCustomContents()
*/

void PropertyItem::drawCustomContents( TQPainter *, const TQRect & )
{
}

TQString PropertyItem::currentItem() const
{
    return TQString::null;
}

int PropertyItem::currentIntItem() const
{
    return -1;
}

void PropertyItem::setCurrentItem( const TQString & )
{
}

void PropertyItem::setCurrentItem( int )
{
}

int PropertyItem::currentIntItemFromObject() const
{
    return -1;
}

TQString PropertyItem::currentItemFromObject() const
{
    return TQString::null;
}

void PropertyItem::setFocus( TQWidget *w )
{
    if ( !tqApp->focusWidget() ||
	 ( listview->propertyEditor()->formWindow() &&
	 ( !MainWindow::self->isAFormWindowChild( tqApp->focusWidget() ) &&
	   !tqApp->focusWidget()->inherits( "Editor" ) ) ) )
	w->setFocus();
}

void PropertyItem::setText( int col, const TQString &t )
{
    TQString txt( t );
    if ( col == 1 )
	txt = txt.replace( "\n", " " );
    TQListViewItem::setText( col, txt );
}

// --------------------------------------------------------------

PropertyTextItem::PropertyTextItem( PropertyList *l, PropertyItem *after, PropertyItem *prop,
				    const TQString &propName, bool comment, bool multiLine, bool ascii, bool a )
    : PropertyItem( l, after, prop, propName ), withComment( comment ),
      hasMultiLines( multiLine ), asciiOnly( ascii ), accel( a )
{
    lin = 0;
    box = 0;
}

TQLineEdit *PropertyTextItem::lined()
{
    if ( lin )
	return lin;
    if ( hasMultiLines ) {
	box = new TQHBox( listview->viewport() );
	box->setFrameStyle( TQFrame::StyledPanel | TQFrame::Sunken );
	box->setLineWidth( 2 );
	box->hide();
    }

    lin = 0;
    if ( hasMultiLines )
	lin = new TQLineEdit( box );
    else
	lin = new TQLineEdit( listview->viewport() );

    if ( asciiOnly ) {
	if ( PropertyItem::name() == "name" ) {
	    lin->setValidator( new AsciiValidator( TQString(":"), lin, "ascii_validator" ) );
	    if ( listview->propertyEditor()->formWindow()->isFake() )
		lin->setEnabled( false );
	} else {
	    lin->setValidator( new AsciiValidator( TQString("!\"#$%&'()*+,-./:;<=>?@[\\]^_`{|}~"
							   "\xa1\xa2\xa3\xa4\xa5\xa6\xa7\xa8\xa9"
							   "\xaa\xab\xac\xad\xae\xaf\xb1\xb2\xb3"
							   "\xb4\xb5\xb6\xb7\xb8\xb9\xba\xbb\xbc"
							   "\xbd\xbe\xbf"), lin, "ascii_validator" ) );
	}
    } if ( !hasMultiLines ) {
	lin->hide();
    } else {
	button = new TQPushButton( tr("..."), box );
	setupStyle( button );
	button->setFixedWidth( 20 );
	connect( button, TQ_SIGNAL( clicked() ),
		 this, TQ_SLOT( getText() ) );
	lin->setFrame( false );
    }
    connect( lin, TQ_SIGNAL( returnPressed() ),
	     this, TQ_SLOT( setValue() ) );
    connect( lin, TQ_SIGNAL( textChanged( const TQString & ) ),
	     this, TQ_SLOT( setValue() ) );
    if ( PropertyItem::name() == "name" || PropertyItem::name() == "itemName" )
	connect( lin, TQ_SIGNAL( returnPressed() ),
		 listview->propertyEditor()->formWindow()->commandHistory(),
		 TQ_SLOT( checkCompressedCommand() ) );
    lin->installEventFilter( listview );
    return lin;
}

PropertyTextItem::~PropertyTextItem()
{
    delete (TQLineEdit*)lin;
    lin = 0;
    delete (TQHBox*)box;
    box = 0;
}

void PropertyTextItem::setChanged( bool b, bool updateDb )
{
    PropertyItem::setChanged( b, updateDb );
    if ( withComment && childCount() > 0 )
	( (PropertyTextItem*)PropertyItem::child( 0 ) )->lined()->setEnabled( b );
}

bool PropertyTextItem::hasSubItems() const
{
    return withComment;
}

void PropertyTextItem::childValueChanged( PropertyItem *child )
{
    if ( PropertyItem::name() != "name" )
	MetaDataBase::setPropertyComment( listview->propertyEditor()->widget(),
					  PropertyItem::name(), child->value().toString() );
    else
	MetaDataBase::setExportMacro( listview->propertyEditor()->widget(), child->value().toString() );
    listview->propertyEditor()->formWindow()->commandHistory()->setModified( true );
}

void PropertyTextItem::showEditor()
{
    PropertyItem::showEditor();
    if ( !lin || lin->text().length() == 0 ) {
	lined()->blockSignals( true );
	lined()->setText( value().toString() );
	lined()->blockSignals( false );
    }

    TQWidget* w;
    if ( hasMultiLines )
	w = box;
    else
	w= lined();

    placeEditor( w );
    if ( !w->isVisible() || !lined()->hasFocus() ) {
	w->show();
	setFocus( lined() );
    }
}

void PropertyTextItem::createChildren()
{
    PropertyTextItem *i = new PropertyTextItem( listview, this, this,
						PropertyItem::name() == "name" ?
						"export macro" : "comment", false, false,
						PropertyItem::name() == "name" );
    i->lined()->setEnabled( isChanged() );
    addChild( i );
}

void PropertyTextItem::initChildren()
{
    if ( !childCount() )
	return;
    PropertyItem *item = PropertyItem::child( 0 );
    if ( item ) {
	if ( PropertyItem::name() != "name" )
	    item->setValue( MetaDataBase::propertyComment( listview->propertyEditor()->widget(),
							   PropertyItem::name() ) );
	else
	    item->setValue( MetaDataBase::exportMacro( listview->propertyEditor()->widget() ) );
    }
}

void PropertyTextItem::hideEditor()
{
    PropertyItem::hideEditor();
    TQWidget* w;
    if ( hasMultiLines )
	w = box;
    else
	w = lined();

    w->hide();
}

void PropertyTextItem::setValue( const TQVariant &v )
{
    if ( ( !hasSubItems() || !isOpen() )
	 && value() == v )
	return;
    if ( lin ) {
	lined()->blockSignals( true );
	int oldCursorPos;
	oldCursorPos = lin->cursorPosition();
	lined()->setText( v.toString() );
	if ( oldCursorPos < (int)lin->text().length() )
	    lin->setCursorPosition( oldCursorPos );
	lined()->blockSignals( false );
    }
    setText( 1, v.toString() );
    PropertyItem::setValue( v );
}

void PropertyTextItem::setValue()
{
    setText( 1, lined()->text() );
    TQVariant v;
    if ( accel ) {
	v = TQVariant( TQKeySequence( lined()->text() ) );
	if ( v.toString().isNull() )
	    return; // not yet valid input
    } else {
	v = lined()->text();
    }
    PropertyItem::setValue( v );
    notifyValueChange();
}

void PropertyTextItem::getText()
{
    bool richText = !::tqt_cast<TQButton*>(listview->propertyEditor()->widget()) ||
		    ( text( 0 ) == "whatsThis" );
    bool doWrap = false;
    TQString txt = MultiLineEditor::getText( listview, value().toString(), richText, &doWrap );
    if ( !txt.isEmpty() ) {
	setText( 1, txt );
	PropertyItem::setValue( txt );
	notifyValueChange();
	lined()->blockSignals( true );
	lined()->setText( txt );
	lined()->blockSignals( false );
    }
}

// --------------------------------------------------------------

PropertyDoubleItem::PropertyDoubleItem( PropertyList *l, PropertyItem *after, PropertyItem *prop,
				    const TQString &propName )
    : PropertyItem( l, after, prop, propName )
{
    lin = 0;
}

TQLineEdit *PropertyDoubleItem::lined()
{
    if ( lin )
	return lin;
    lin = new TQLineEdit( listview->viewport() );
    lin->setValidator( new TQDoubleValidator( lin, "double_validator" ) );

    connect( lin, TQ_SIGNAL( returnPressed() ),
	     this, TQ_SLOT( setValue() ) );
    connect( lin, TQ_SIGNAL( textChanged( const TQString & ) ),
	     this, TQ_SLOT( setValue() ) );
    lin->installEventFilter( listview );
    return lin;
}

PropertyDoubleItem::~PropertyDoubleItem()
{
    delete (TQLineEdit*)lin;
    lin = 0;
}

void PropertyDoubleItem::showEditor()
{
    PropertyItem::showEditor();
    if ( !lin ) {
	lined()->blockSignals( true );
	lined()->setText( TQString::number( value().toDouble() ) );
	lined()->blockSignals( false );
    }
    TQWidget* w = lined();

    placeEditor( w );
    if ( !w->isVisible() || !lined()->hasFocus() ) {
	w->show();
	setFocus( lined() );
    }
}


void PropertyDoubleItem::hideEditor()
{
    PropertyItem::hideEditor();
    TQWidget* w = lined();
    w->hide();
}

void PropertyDoubleItem::setValue( const TQVariant &v )
{
    if ( value() == v )
	return;
    if ( lin ) {
	lined()->blockSignals( true );
	int oldCursorPos;
	oldCursorPos = lin->cursorPosition();
	lined()->setText( TQString::number( v.toDouble() ) );
	if ( oldCursorPos < (int)lin->text().length() )
	    lin->setCursorPosition( oldCursorPos );
	lined()->blockSignals( false );
    }
    setText( 1, TQString::number( v.toDouble() ) );
    PropertyItem::setValue( v );
}

void PropertyDoubleItem::setValue()
{
    setText( 1, lined()->text() );
    TQVariant v = lined()->text().toDouble();
    PropertyItem::setValue( v );
    notifyValueChange();
}


// --------------------------------------------------------------

PropertyDateItem::PropertyDateItem( PropertyList *l, PropertyItem *after, PropertyItem *prop, const TQString &propName )
    : PropertyItem( l, after, prop, propName )
{
    lin = 0;
}

TQDateEdit *PropertyDateItem::lined()
{
    if ( lin )
	return lin;
    lin = new TQDateEdit( listview->viewport() );
    TQObjectList *l = lin->queryList( "TQLineEdit" );
    for ( TQObject *o = l->first(); o; o = l->next() )
	o->installEventFilter( listview );
    delete l;
    connect( lin, TQ_SIGNAL( valueChanged( const TQDate & ) ),
	     this, TQ_SLOT( setValue() ) );
    return lin;
}

PropertyDateItem::~PropertyDateItem()
{
    delete (TQDateEdit*)lin;
    lin = 0;
}

void PropertyDateItem::showEditor()
{
    PropertyItem::showEditor();
    if ( !lin ) {
	lined()->blockSignals( true );
	lined()->setDate( value().toDate() );
	lined()->blockSignals( false );
    }
    placeEditor( lin );
    if ( !lin->isVisible() ) {
	lin->show();
	setFocus( lin );
    }
}

void PropertyDateItem::hideEditor()
{
    PropertyItem::hideEditor();
    if ( lin )
	lin->hide();
}

void PropertyDateItem::setValue( const TQVariant &v )
{
    if ( ( !hasSubItems() || !isOpen() )
	 && value() == v )
	return;

    if ( lin ) {
	lined()->blockSignals( true );
	if ( lined()->date() != v.toDate() )
	    lined()->setDate( v.toDate() );
	lined()->blockSignals( false );
    }
    setText( 1, v.toDate().toString( ::TQt::ISODate ) );
    PropertyItem::setValue( v );
}

void PropertyDateItem::setValue()
{
    setText( 1, lined()->date().toString( ::TQt::ISODate ) );
    TQVariant v;
    v = lined()->date();
    PropertyItem::setValue( v );
    notifyValueChange();
}

// --------------------------------------------------------------

PropertyTimeItem::PropertyTimeItem( PropertyList *l, PropertyItem *after, PropertyItem *prop, const TQString &propName )
    : PropertyItem( l, after, prop, propName )
{
    lin = 0;
}

TQTimeEdit *PropertyTimeItem::lined()
{
    if ( lin )
	return lin;
    lin = new TQTimeEdit( listview->viewport() );
    connect( lin, TQ_SIGNAL( valueChanged( const TQTime & ) ),
	     this, TQ_SLOT( setValue() ) );
    TQObjectList *l = lin->queryList( "TQLineEdit" );
    for ( TQObject *o = l->first(); o; o = l->next() )
	o->installEventFilter( listview );
    delete l;
    return lin;
}

PropertyTimeItem::~PropertyTimeItem()
{
    delete (TQTimeEdit*)lin;
    lin = 0;
}

void PropertyTimeItem::showEditor()
{
    PropertyItem::showEditor();
    if ( !lin ) {
	lined()->blockSignals( true );
	lined()->setTime( value().toTime() );
	lined()->blockSignals( false );
    }
    placeEditor( lin );
    if ( !lin->isVisible() ) {
	lin->show();
	setFocus( lin );
    }
}

void PropertyTimeItem::hideEditor()
{
    PropertyItem::hideEditor();
    if ( lin )
	lin->hide();
}

void PropertyTimeItem::setValue( const TQVariant &v )
{
    if ( ( !hasSubItems() || !isOpen() )
	 && value() == v )
	return;

    if ( lin ) {
	lined()->blockSignals( true );
	if ( lined()->time() != v.toTime() )
	    lined()->setTime( v.toTime() );
	lined()->blockSignals( false );
    }
    setText( 1, v.toTime().toString( ::TQt::ISODate ) );
    PropertyItem::setValue( v );
}

void PropertyTimeItem::setValue()
{
    setText( 1, lined()->time().toString( ::TQt::ISODate ) );
    TQVariant v;
    v = lined()->time();
    PropertyItem::setValue( v );
    notifyValueChange();
}

// --------------------------------------------------------------

PropertyDateTimeItem::PropertyDateTimeItem( PropertyList *l, PropertyItem *after, PropertyItem *prop, const TQString &propName )
    : PropertyItem( l, after, prop, propName )
{
    lin = 0;
}

TQDateTimeEdit *PropertyDateTimeItem::lined()
{
    if ( lin )
	return lin;
    lin = new TQDateTimeEdit( listview->viewport() );
    connect( lin, TQ_SIGNAL( valueChanged( const TQDateTime & ) ),
	     this, TQ_SLOT( setValue() ) );
    TQObjectList *l = lin->queryList( "TQLineEdit" );
    for ( TQObject *o = l->first(); o; o = l->next() )
	o->installEventFilter( listview );
    delete l;
    return lin;
}

PropertyDateTimeItem::~PropertyDateTimeItem()
{
    delete (TQDateTimeEdit*)lin;
    lin = 0;
}

void PropertyDateTimeItem::showEditor()
{
    PropertyItem::showEditor();
    if ( !lin ) {
	lined()->blockSignals( true );
	lined()->setDateTime( value().toDateTime() );
	lined()->blockSignals( false );
    }
    placeEditor( lin );
    if ( !lin->isVisible() ) {
	lin->show();
	setFocus( lin );
    }
}

void PropertyDateTimeItem::hideEditor()
{
    PropertyItem::hideEditor();
    if ( lin )
	lin->hide();
}

void PropertyDateTimeItem::setValue( const TQVariant &v )
{
    if ( ( !hasSubItems() || !isOpen() )
	 && value() == v )
	return;

    if ( lin ) {
	lined()->blockSignals( true );
	if ( lined()->dateTime() != v.toDateTime() )
	    lined()->setDateTime( v.toDateTime() );
	lined()->blockSignals( false );
    }
    setText( 1, v.toDateTime().toString( ::TQt::ISODate ) );
    PropertyItem::setValue( v );
}

void PropertyDateTimeItem::setValue()
{
    setText( 1, lined()->dateTime().toString( ::TQt::ISODate ) );
    TQVariant v;
    v = lined()->dateTime();
    PropertyItem::setValue( v );
    notifyValueChange();
}

// --------------------------------------------------------------

PropertyBoolItem::PropertyBoolItem( PropertyList *l, PropertyItem *after, PropertyItem *prop, const TQString &propName )
    : PropertyItem( l, after, prop, propName )
{
    comb = 0;
}

TQComboBox *PropertyBoolItem::combo()
{
    if ( comb )
	return comb;
    comb = new TQComboBox( false, listview->viewport() );
    comb->hide();
    comb->insertItem( tr( "False" ) );
    comb->insertItem( tr( "True" ) );
    connect( comb, TQ_SIGNAL( activated( int ) ),
	     this, TQ_SLOT( setValue() ) );
    comb->installEventFilter( listview );
    return comb;
}

PropertyBoolItem::~PropertyBoolItem()
{
    delete (TQComboBox*)comb;
    comb = 0;
}

void PropertyBoolItem::toggle()
{
    bool b = value().toBool();
    setValue( TQVariant( !b ) );
    setValue();
}

void PropertyBoolItem::showEditor()
{
    PropertyItem::showEditor();
    if ( !comb ) {
	combo()->blockSignals( true );
	if ( value().toBool() )
	    combo()->setCurrentItem( 1 );
	else
	    combo()->setCurrentItem( 0 );
	combo()->blockSignals( false );
    }
    placeEditor( combo() );
    if ( !combo()->isVisible()  || !combo()->hasFocus() ) {
	combo()->show();
	setFocus( combo() );
    }
}

void PropertyBoolItem::hideEditor()
{
    PropertyItem::hideEditor();
    combo()->hide();
}

void PropertyBoolItem::setValue( const TQVariant &v )
{
    if ( ( !hasSubItems() || !isOpen() )
	 && value() == v )
	return;

    if ( comb ) {
	combo()->blockSignals( true );
	if ( v.toBool() )
	    combo()->setCurrentItem( 1 );
	else
	    combo()->setCurrentItem( 0 );
	combo()->blockSignals( false );
    }
    TQString tmp = tr( "True" );
    if ( !v.toBool() )
	tmp = tr( "False" );
    setText( 1, tmp );
    PropertyItem::setValue( v );
}

void PropertyBoolItem::setValue()
{
    if ( !comb )
	return;
    setText( 1, combo()->currentText() );
    bool b = (combo()->currentItem() != 0);
    PropertyItem::setValue( TQVariant( b ) );
    notifyValueChange();
}

// --------------------------------------------------------------

PropertyIntItem::PropertyIntItem( PropertyList *l, PropertyItem *after, PropertyItem *prop,
				  const TQString &propName, bool s )
    : PropertyItem( l, after, prop, propName ), signedValue( s )
{
    spinBx = 0;
}

TQSpinBox *PropertyIntItem::spinBox()
{
    if ( spinBx )
	return spinBx;
    if ( signedValue )
	spinBx = new TQSpinBox( -INT_MAX, INT_MAX, 1, listview->viewport() );
    else
	spinBx = new TQSpinBox( 0, INT_MAX, 1, listview->viewport() );
    spinBx->hide();
    spinBx->installEventFilter( listview );
    TQObjectList *ol = spinBx->queryList( "TQLineEdit" );
    if ( ol && ol->first() )
	ol->first()->installEventFilter( listview );
    delete ol;
    connect( spinBx, TQ_SIGNAL( valueChanged( int ) ),
	     this, TQ_SLOT( setValue() ) );
    return spinBx;
}

PropertyIntItem::~PropertyIntItem()
{
    delete (TQSpinBox*)spinBx;
    spinBx = 0;
}

void PropertyIntItem::showEditor()
{
    PropertyItem::showEditor();
    if ( !spinBx ) {
	spinBox()->blockSignals( true );
	if ( signedValue )
	    spinBox()->setValue( value().toInt() );
	else
	    spinBox()->setValue( value().toUInt() );
	spinBox()->blockSignals( false );
    }
    placeEditor( spinBox() );
    if ( !spinBox()->isVisible()  || !spinBox()->hasFocus()  ) {
	spinBox()->show();
	setFocus( spinBox() );
    }
}

void PropertyIntItem::hideEditor()
{
    PropertyItem::hideEditor();
    spinBox()->hide();
}

void PropertyIntItem::setValue( const TQVariant &v )
{
    if ( ( !hasSubItems() || !isOpen() )
	 && value() == v )
	return;

    if ( spinBx ) {
	spinBox()->blockSignals( true );
	if ( signedValue )
	    spinBox()->setValue( v.toInt() );
	else
	    spinBox()->setValue( v.toUInt() );
	spinBox()->blockSignals( false );
    }

    if ( signedValue )
	    setText( 1, TQString::number( v.toInt() ) );
    else
	    setText( 1, TQString::number( v.toUInt() ) );
    PropertyItem::setValue( v );
}

void PropertyIntItem::setValue()
{
    if ( !spinBx )
	return;
    setText( 1, TQString::number( spinBox()->value() ) );
    if ( signedValue )
	PropertyItem::setValue( spinBox()->value() );
    else
	PropertyItem::setValue( (uint)spinBox()->value() );
    notifyValueChange();
}

// --------------------------------------------------------------

PropertyLayoutItem::PropertyLayoutItem( PropertyList *l, PropertyItem *after, PropertyItem *prop,
		  const TQString &propName )
    : PropertyItem( l, after, prop, propName )
{
    spinBx = 0;
}

PropertyLayoutItem::~PropertyLayoutItem()
{
    delete (TQSpinBox*)spinBx;
    spinBx = 0;
}

TQSpinBox* PropertyLayoutItem::spinBox()
{
    if ( spinBx )
	return spinBx;
    spinBx = new TQSpinBox( -1, INT_MAX, 1, listview->viewport() );
    spinBx->setSpecialValueText( tr( "default" ) );
    spinBx->hide();
    spinBx->installEventFilter( listview );
    TQObjectList *ol = spinBx->queryList( "TQLineEdit" );
    if ( ol && ol->first() )
	ol->first()->installEventFilter( listview );
    delete ol;
    connect( spinBx, TQ_SIGNAL( valueChanged( int ) ),
	     this, TQ_SLOT( setValue() ) );
    return spinBx;
}

void PropertyLayoutItem::showEditor()
{
    PropertyItem::showEditor();
    if ( !spinBx ) {
	spinBox()->blockSignals( true );
	spinBox()->setValue( value().toInt() );
	spinBox()->blockSignals( true );
    }
    placeEditor( spinBox() );
    if ( !spinBox()->isVisible() || !spinBox()->hasFocus() ) {
	spinBox()->show();
	setFocus( spinBox() );
    }
}

void PropertyLayoutItem::hideEditor()
{
    PropertyItem::hideEditor();
    spinBox()->hide();
}

void PropertyLayoutItem::setValue( const TQVariant &v )
{
    if ( spinBx ) {
	spinBox()->blockSignals( true );
	spinBox()->setValue( v.toInt() );
	spinBox()->blockSignals( false );
    }
    TQString s = v.toString();
    if ( v.toInt() == -1 )
	s = spinBox()->specialValueText();
    setText( 1, s );
    PropertyItem::setValue( v );
}

void PropertyLayoutItem::setValue()
{
    if ( !spinBx )
	return;
    PropertyItem::setValue( spinBox()->value() );
    notifyValueChange();
}


// --------------------------------------------------------------

PropertyListItem::PropertyListItem( PropertyList *l, PropertyItem *after, PropertyItem *prop,
				    const TQString &propName, bool e )
    : PropertyItem( l, after, prop, propName ), editable( e )
{
    comb = 0;
    oldInt = -1;
}

TQComboBox *PropertyListItem::combo()
{
    if ( comb )
	return comb;
    comb = new TQComboBox( editable, listview->viewport() );
    comb->hide();
    connect( comb, TQ_SIGNAL( activated( int ) ),
	     this, TQ_SLOT( setValue() ) );
    comb->installEventFilter( listview );
    if ( editable ) {
	TQObjectList *ol = comb->queryList( "TQLineEdit" );
	if ( ol && ol->first() )
	    ol->first()->installEventFilter( listview );
	delete ol;
    }
    return comb;
}

PropertyListItem::~PropertyListItem()
{
    delete (TQComboBox*)comb;
    comb = 0;
}

void PropertyListItem::showEditor()
{
    PropertyItem::showEditor();
    if ( !comb ) {
	combo()->blockSignals( true );
	combo()->clear();
	combo()->insertStringList( value().toStringList() );
	combo()->blockSignals( false );
    }
    placeEditor( combo() );
    if ( !combo()->isVisible()  || !combo()->hasFocus() ) {
	combo()->show();
	setFocus( combo() );
    }
}

void PropertyListItem::hideEditor()
{
    PropertyItem::hideEditor();
    combo()->hide();
}

void PropertyListItem::setValue( const TQVariant &v )
{
    if ( comb ) {
	combo()->blockSignals( true );
	combo()->clear();
	combo()->insertStringList( v.toStringList() );
	combo()->blockSignals( false );
    }
    setText( 1, v.toStringList().first() );
    PropertyItem::setValue( v );
}

void PropertyListItem::setValue()
{
    if ( !comb )
	return;
    setText( 1, combo()->currentText() );
    TQStringList lst;
    for ( uint i = 0; i < combo()->listBox()->count(); ++i )
	lst << combo()->listBox()->item( i )->text();
    PropertyItem::setValue( lst );
    notifyValueChange();
    oldInt = currentIntItem();
    oldString = currentItem();
}

TQString PropertyListItem::currentItem() const
{
    return ( (PropertyListItem*)this )->combo()->currentText();
}

void PropertyListItem::setCurrentItem( const TQString &s )
{
    if ( comb && currentItem().lower() == s.lower() )
	return;

    if ( !comb ) {
	combo()->blockSignals( true );
	combo()->clear();
	combo()->insertStringList( value().toStringList() );
	combo()->blockSignals( false );
    }
    for ( uint i = 0; i < combo()->listBox()->count(); ++i ) {
	if ( combo()->listBox()->item( i )->text().lower() == s.lower() ) {
	    combo()->setCurrentItem( i );
	    setText( 1, combo()->currentText() );
	    break;
	}
    }
    oldInt = currentIntItem();
    oldString = currentItem();
}

void PropertyListItem::addItem( const TQString &s )
{
    combo()->insertItem( s );
}

void PropertyListItem::setCurrentItem( int i )
{
    if ( comb && i == combo()->currentItem() )
	return;

    if ( !comb ) {
	combo()->blockSignals( true );
	combo()->clear();
	combo()->insertStringList( value().toStringList() );
	combo()->blockSignals( false );
    }
    combo()->setCurrentItem( i );
    setText( 1, combo()->currentText() );
    oldInt = currentIntItem();
    oldString = currentItem();
}

int PropertyListItem::currentIntItem() const
{
    return ( (PropertyListItem*)this )->combo()->currentItem();
}

int PropertyListItem::currentIntItemFromObject() const
{
    return oldInt;
}

TQString PropertyListItem::currentItemFromObject() const
{
    return oldString;
}

// --------------------------------------------------------------

PropertyCoordItem::PropertyCoordItem( PropertyList *l, PropertyItem *after, PropertyItem *prop,
				    const TQString &propName, Type t )
    : PropertyItem( l, after, prop, propName ), typ( t )
{
    lin = 0;

}

TQLineEdit *PropertyCoordItem::lined()
{
    if ( lin )
	return lin;
    lin = new TQLineEdit( listview->viewport() );
    lin->setReadOnly( true );
    lin->installEventFilter( listview );
    lin->hide();
    return lin;
}

void PropertyCoordItem::createChildren()
{
    PropertyItem *i = this;
    if ( typ == Rect || typ == Point ) {
	i = new PropertyIntItem( listview, i, this, tr( "x" ), true );
	addChild( i );
	i = new PropertyIntItem( listview, i, this, tr( "y" ), true );
	addChild( i );
    }
    if ( typ == Rect || typ == Size ) {
	i = new PropertyIntItem( listview, i, this, tr( "width" ), true );
	addChild( i );
	i = new PropertyIntItem( listview, i, this, tr( "height" ), true );
	addChild( i );
    }
}

void PropertyCoordItem::initChildren()
{
    PropertyItem *item = 0;
    for ( int i = 0; i < childCount(); ++i ) {
	item = PropertyItem::child( i );
	if ( item->name() == tr( "x" ) ) {
	    if ( typ == Rect )
		item->setValue( val.toRect().x() );
	    else if ( typ == Point )
		item->setValue( val.toPoint().x() );
	} else if ( item->name() == tr( "y" ) ) {
	    if ( typ == Rect )
		item->setValue( val.toRect().y() );
	    else if ( typ == Point )
		item->setValue( val.toPoint().y() );
	} else if ( item->name() == tr( "width" ) ) {
	    if ( typ == Rect )
		item->setValue( val.toRect().width() );
	    else if ( typ == Size )
		item->setValue( val.toSize().width() );
	} else if ( item->name() == tr( "height" ) ) {
	    if ( typ == Rect )
		item->setValue( val.toRect().height() );
	    else if ( typ == Size )
		item->setValue( val.toSize().height() );
	}
    }
}

PropertyCoordItem::~PropertyCoordItem()
{
    delete (TQLineEdit*)lin;
    lin = 0;
}

void PropertyCoordItem::showEditor()
{
    PropertyItem::showEditor();
    if ( !lin )
	lined()->setText( text( 1 ) );
    placeEditor( lined() );
    if ( !lined()->isVisible() || !lined()->hasFocus()  ) {
	lined()->show();
	setFocus( lined() );
    }
}

void PropertyCoordItem::hideEditor()
{
    PropertyItem::hideEditor();
    lined()->hide();
}

void PropertyCoordItem::setValue( const TQVariant &v )
{
    if ( ( !hasSubItems() || !isOpen() )
	 && value() == v )
	return;

    TQString s;
    if ( typ == Rect )
	s = "[ " + TQString::number( v.toRect().x() ) + ", " + TQString::number( v.toRect().y() ) + ", " +
	    TQString::number( v.toRect().width() ) + ", " + TQString::number( v.toRect().height() ) + " ]";
    else if ( typ == Point )
	s = "[ " + TQString::number( v.toPoint().x() ) + ", " +
	    TQString::number( v.toPoint().y() ) + " ]";
    else if ( typ == Size )
	s = "[ " + TQString::number( v.toSize().width() ) + ", " +
	    TQString::number( v.toSize().height() ) + " ]";
    setText( 1, s );
    if ( lin )
	lined()->setText( s );
    PropertyItem::setValue( v );
}

bool PropertyCoordItem::hasSubItems() const
{
    return true;
}

void PropertyCoordItem::childValueChanged( PropertyItem *child )
{
    if ( typ == Rect ) {
	TQRect r = value().toRect();
	if ( child->name() == tr( "x" ) )
	    r.moveBy( -r.x() + child->value().toInt(), 0 );
	else if ( child->name() == tr( "y" ) )
	    r.moveBy( 0, -r.y() + child->value().toInt() );
	else if ( child->name() == tr( "width" ) )
	    r.setWidth( child->value().toInt() );
	else if ( child->name() == tr( "height" ) )
	    r.setHeight( child->value().toInt() );
	setValue( r );
    } else if ( typ == Point ) {
	TQPoint r = value().toPoint();
	if ( child->name() == tr( "x" ) )
	    r.setX( child->value().toInt() );
	else if ( child->name() == tr( "y" ) )
	    r.setY( child->value().toInt() );
	setValue( r );
    } else if ( typ == Size ) {
	TQSize r = value().toSize();
	if ( child->name() == tr( "width" ) )
	    r.setWidth( child->value().toInt() );
	else if ( child->name() == tr( "height" ) )
	    r.setHeight( child->value().toInt() );
	setValue( r );
    }
    notifyValueChange();
}

// --------------------------------------------------------------

PropertyPixmapItem::PropertyPixmapItem( PropertyList *l, PropertyItem *after, PropertyItem *prop,
					const TQString &propName, Type t )
    : PropertyItem( l, after, prop, propName ), type( t )
{
    box = new TQHBox( listview->viewport() );
    box->hide();
    pixPrev = new TQLabel( box );
    pixPrev->setSizePolicy( TQSizePolicy( TQSizePolicy::Ignored, TQSizePolicy::Minimum ) );
    pixPrev->setBackgroundColor( pixPrev->colorGroup().color( TQColorGroup::Base ) );
    button = new TQPushButton( "...", box );
    setupStyle( button );
    button->setFixedWidth( 20 );
    box->setFrameStyle( TQFrame::StyledPanel | TQFrame::Sunken );
    box->setLineWidth( 2 );
    pixPrev->setFrameStyle( TQFrame::NoFrame );
    box->installEventFilter( listview );
    connect( button, TQ_SIGNAL( clicked() ),
	     this, TQ_SLOT( getPixmap() ) );
}

PropertyPixmapItem::~PropertyPixmapItem()
{
    delete (TQHBox*)box;
}

void PropertyPixmapItem::showEditor()
{
    PropertyItem::showEditor();
    placeEditor( box );
    if ( !box->isVisible() ) {
	box->show();
	listView()->viewport()->setFocus();
    }
}

void PropertyPixmapItem::hideEditor()
{
    PropertyItem::hideEditor();
    box->hide();
}

void PropertyPixmapItem::setValue( const TQVariant &v )
{
    TQString s;
    if ( type == Pixmap )
	pixPrev->setPixmap( v.toPixmap() );
    else if ( type == IconSet )
	pixPrev->setPixmap( v.toIconSet().pixmap() );
    else
	pixPrev->setPixmap( v.toImage() );
    PropertyItem::setValue( v );
    repaint();
}

void PropertyPixmapItem::getPixmap()
{
    TQPixmap pix = qChoosePixmap( listview, listview->propertyEditor()->formWindow(), value().toPixmap() );
    if ( !pix.isNull() ) {
	if ( type == Pixmap )
	    setValue( pix );
	else if ( type == IconSet )
	    setValue( TQIconSet( pix ) );
	else
	    setValue( pix.convertToImage() );

	notifyValueChange();
    }
}

bool PropertyPixmapItem::hasCustomContents() const
{
    return true;
}

void PropertyPixmapItem::drawCustomContents( TQPainter *p, const TQRect &r )
{
    TQPixmap pix;
    if ( type == Pixmap )
	pix = value().toPixmap();
    else if ( type == IconSet )
	pix = value().toIconSet().pixmap();
    else
	pix = value().toImage();

    if ( !pix.isNull() ) {
	p->save();
	p->setClipRect( TQRect( TQPoint( (int)(p->worldMatrix().dx() + r.x()),
				       (int)(p->worldMatrix().dy() + r.y()) ),
			       r.size() ) );
	p->drawPixmap( r.x(), r.y() + ( r.height() - pix.height() ) / 2, pix );
	p->restore();
    }
}


// --------------------------------------------------------------

PropertyColorItem::PropertyColorItem( PropertyList *l, PropertyItem *after, PropertyItem *prop,
				      const TQString &propName, bool children )
    : PropertyItem( l, after, prop, propName ), withChildren( children )
{
    box = new TQHBox( listview->viewport() );
    box->hide();
    colorPrev = new TQFrame( box );
    button = new TQPushButton( "...", box );
    setupStyle( button );
    button->setFixedWidth( 20 );
    box->setFrameStyle( TQFrame::StyledPanel | TQFrame::Sunken );
    box->setLineWidth( 2 );
    colorPrev->setFrameStyle( TQFrame::Plain | TQFrame::Box );
    colorPrev->setLineWidth( 2 );
    TQPalette pal = colorPrev->palette();
    TQColorGroup cg = pal.active();
    cg.setColor( TQColorGroup::Foreground, cg.color( TQColorGroup::Base ) );
    pal.setActive( cg );
    pal.setInactive( cg );
    pal.setDisabled( cg );
    colorPrev->setPalette( pal );
    box->installEventFilter( listview );
    connect( button, TQ_SIGNAL( clicked() ),
	     this, TQ_SLOT( getColor() ) );
}

void PropertyColorItem::createChildren()
{
    PropertyItem *i = this;
    i = new PropertyIntItem( listview, i, this, tr( "Red" ), true );
    addChild( i );
    i = new PropertyIntItem( listview, i, this, tr( "Green" ), true );
    addChild( i );
    i = new PropertyIntItem( listview, i, this, tr( "Blue" ), true );
    addChild( i );
}

void PropertyColorItem::initChildren()
{
    PropertyItem *item = 0;
    for ( int i = 0; i < childCount(); ++i ) {
	item = PropertyItem::child( i );
	if ( item->name() == tr( "Red" ) )
	    item->setValue( val.toColor().red() );
	else if ( item->name() == tr( "Green" ) )
	    item->setValue( val.toColor().green() );
	else if ( item->name() == tr( "Blue" ) )
	    item->setValue( val.toColor().blue() );
    }
}

PropertyColorItem::~PropertyColorItem()
{
    delete (TQHBox*)box;
}

void PropertyColorItem::showEditor()
{
    PropertyItem::showEditor();
    placeEditor( box );
    if ( !box->isVisible() ) {
	box->show();
	listView()->viewport()->setFocus();
    }
}

void PropertyColorItem::hideEditor()
{
    PropertyItem::hideEditor();
    box->hide();
}

void PropertyColorItem::setValue( const TQVariant &v )
{
    if ( ( !hasSubItems() || !isOpen() )
	 && value() == v )
	return;

    TQString s;
    setText( 1, v.toColor().name() );
    colorPrev->setBackgroundColor( v.toColor() );
    PropertyItem::setValue( v );
}

bool PropertyColorItem::hasSubItems() const
{
    return withChildren;
}

void PropertyColorItem::childValueChanged( PropertyItem *child )
{
    TQColor c( val.toColor() );
    if ( child->name() == tr( "Red" ) )
	c.setRgb( child->value().toInt(), c.green(), c.blue() );
    else if ( child->name() == tr( "Green" ) )
	c.setRgb( c.red(), child->value().toInt(), c.blue() );
    else if ( child->name() == tr( "Blue" ) )
	c.setRgb( c.red(), c.green(), child->value().toInt() );
    setValue( c );
    notifyValueChange();
}

void PropertyColorItem::getColor()
{
    TQColor c = TQColorDialog::getColor( val.asColor(), listview );
    if ( c.isValid() ) {
	setValue( c );
	notifyValueChange();
    }
}

bool PropertyColorItem::hasCustomContents() const
{
    return true;
}

void PropertyColorItem::drawCustomContents( TQPainter *p, const TQRect &r )
{
    p->save();
    p->setPen( TQPen( black, 1 ) );
    p->setBrush( val.toColor() );
    p->drawRect( r.x() + 2, r.y() + 2, r.width() - 5, r.height() - 5 );
    p->restore();
}

// --------------------------------------------------------------

PropertyFontItem::PropertyFontItem( PropertyList *l, PropertyItem *after, PropertyItem *prop, const TQString &propName )
    : PropertyItem( l, after, prop, propName )
{
    box = new TQHBox( listview->viewport() );
    box->hide();
    lined = new TQLineEdit( box );
    button = new TQPushButton( "...", box );
    setupStyle( button );
    button->setFixedWidth( 20 );
    box->setFrameStyle( TQFrame::StyledPanel | TQFrame::Sunken );
    box->setLineWidth( 2 );
    lined->setFrame( false );
    lined->setReadOnly( true );
    box->setFocusProxy( lined );
    box->installEventFilter( listview );
    lined->installEventFilter( listview );
    button->installEventFilter( listview );
    connect( button, TQ_SIGNAL( clicked() ),
	     this, TQ_SLOT( getFont() ) );
}

void PropertyFontItem::createChildren()
{
    PropertyItem *i = this;
    i = new PropertyListItem( listview, i, this, tr( "Family" ), false );
    addChild( i );
    i = new PropertyIntItem( listview, i, this, tr( "Point Size" ), true );
    addChild( i );
    i = new PropertyBoolItem( listview, i, this, tr( "Bold" ) );
    addChild( i );
    i = new PropertyBoolItem( listview, i, this, tr( "Italic" ) );
    addChild( i );
    i = new PropertyBoolItem( listview, i, this, tr( "Underline" ) );
    addChild( i );
    i = new PropertyBoolItem( listview, i, this, tr( "Strikeout" ) );
    addChild( i );
}

void PropertyFontItem::initChildren()
{
    PropertyItem *item = 0;
    for ( int i = 0; i < childCount(); ++i ) {
	item = PropertyItem::child( i );
	if ( item->name() == tr( "Family" ) ) {
	    ( (PropertyListItem*)item )->setValue( getFontList() );
	    ( (PropertyListItem*)item )->setCurrentItem( val.toFont().family() );
	} else if ( item->name() == tr( "Point Size" ) )
	    item->setValue( val.toFont().pointSize() );
	else if ( item->name() == tr( "Bold" ) )
	    item->setValue( TQVariant( val.toFont().bold() ) );
	else if ( item->name() == tr( "Italic" ) )
	    item->setValue( TQVariant( val.toFont().italic() ) );
	else if ( item->name() == tr( "Underline" ) )
	    item->setValue( TQVariant( val.toFont().underline() ) );
	else if ( item->name() == tr( "Strikeout" ) )
	    item->setValue( TQVariant( val.toFont().strikeOut() ) );
    }
}

PropertyFontItem::~PropertyFontItem()
{
    delete (TQHBox*)box;
}

void PropertyFontItem::showEditor()
{
    PropertyItem::showEditor();
    placeEditor( box );
    if ( !box->isVisible() || !lined->hasFocus() ) {
	box->show();
	setFocus( lined );
    }
}

void PropertyFontItem::hideEditor()
{
    PropertyItem::hideEditor();
    box->hide();
}

void PropertyFontItem::setValue( const TQVariant &v )
{
    if ( value() == v )
	return;

    setText( 1, v.toFont().family() + "-" + TQString::number( v.toFont().pointSize() ) );
    lined->setText( v.toFont().family() + "-" + TQString::number( v.toFont().pointSize() ) );
    PropertyItem::setValue( v );
}

void PropertyFontItem::getFont()
{
    bool ok = false;
    TQFont f = TQFontDialog::getFont( &ok, val.toFont(), listview );
    if ( ok && f != val.toFont() ) {
	setValue( f );
	notifyValueChange();
    }
}

bool PropertyFontItem::hasSubItems() const
{
    return true;
}

void PropertyFontItem::childValueChanged( PropertyItem *child )
{
    TQFont f = val.toFont();
    if ( child->name() == tr( "Family" ) )
	f.setFamily( ( (PropertyListItem*)child )->currentItem() );
    else if ( child->name() == tr( "Point Size" ) )
	f.setPointSize( child->value().toInt() );
    else if ( child->name() == tr( "Bold" ) )
	f.setBold( child->value().toBool() );
    else if ( child->name() == tr( "Italic" ) )
	f.setItalic( child->value().toBool() );
    else if ( child->name() == tr( "Underline" ) )
	f.setUnderline( child->value().toBool() );
    else if ( child->name() == tr( "Strikeout" ) )
	f.setStrikeOut( child->value().toBool() );
    setValue( f );
    notifyValueChange();
}

// --------------------------------------------------------------

PropertyDatabaseItem::PropertyDatabaseItem( PropertyList *l, PropertyItem *after, PropertyItem *prop,
					    const TQString &propName, bool wField )
    : PropertyItem( l, after, prop, propName ), withField( wField )
{
    box = new TQHBox( listview->viewport() );
    box->hide();
    lined = new TQLineEdit( box );
    button = new TQPushButton( "...", box );
    setupStyle( button );
    button->setFixedWidth( 20 );
    box->setFrameStyle( TQFrame::StyledPanel | TQFrame::Sunken );
    box->setLineWidth( 2 );
    lined->setFrame( false );
    lined->setReadOnly( true );
    box->setFocusProxy( lined );
    box->installEventFilter( listview );
    lined->installEventFilter( listview );
    button->installEventFilter( listview );
}

void PropertyDatabaseItem::createChildren()
{
    PropertyItem *i = this;
    i = new PropertyListItem( listview, i, this, tr( "Connection" ), true );
    addChild( i );
    i = new PropertyListItem( listview, i, this, tr( "Table" ), true );
    addChild( i );
    if ( withField ) {
	i = new PropertyListItem( listview, i, this, tr( "Field" ), true );
	addChild( i );
    }
}

void PropertyDatabaseItem::initChildren()
{
#ifndef TQT_NO_SQL
    PropertyItem *item = 0;
    TQStringList lst = value().toStringList();
    TQString conn, table;
    for ( int i = 0; i < childCount(); ++i ) {
	item = PropertyItem::child( i );
	if ( item->name() == tr( "Connection" ) ) {
	    TQStringList cl = listview->propertyEditor()->formWindow()->project()->databaseConnectionList();
	    if ( !cl.isEmpty() )
		item->setValue( cl );
	    else if ( lst.count() > 0 )
		item->setValue( TQStringList( lst[ 0 ] ) );
	    else if ( withField )
		item->setValue( TQStringList( MetaDataBase::fakeProperty( listview->propertyEditor()->formWindow()->mainContainer(),
									 "database" ).toStringList()[ 0 ] ) );

	    if ( lst.count() > 0 && !lst[ 0 ].isEmpty() )
		item->setCurrentItem( lst[ 0 ] );
	    else if ( !isChanged() && withField )
		item->setCurrentItem( MetaDataBase::fakeProperty( listview->propertyEditor()->formWindow()->mainContainer(),
								  "database" ).toStringList()[ 0 ] );
	    else
		item->setCurrentItem( 0 );
	    conn = item->currentItem();
	} else if ( item->name() == tr( "Table" ) ) {
	    TQStringList cl = listview->propertyEditor()->formWindow()->project()->databaseTableList( conn );
	    if ( !cl.isEmpty() )
		item->setValue( cl );
	    else if ( lst.count() > 1 )
		item->setValue( TQStringList( lst[ 1 ] ) );
	    else if ( withField ) {
		TQStringList fakeLst = MetaDataBase::fakeProperty( listview->propertyEditor()->formWindow()->mainContainer(), "database" ).toStringList();
		if ( fakeLst.count() > 1 )
		    item->setValue( fakeLst[ 1 ] );
	    }

	    if ( lst.count() > 1 && !lst[ 1 ].isEmpty() )
		item->setCurrentItem( lst[ 1 ] );
	    else if ( !isChanged() && withField ) {
		TQStringList fakeLst = MetaDataBase::fakeProperty( listview->propertyEditor()->formWindow()->mainContainer(), "database" ).toStringList();
		if ( fakeLst.count() > 1 )
		    item->setCurrentItem( fakeLst[ 1 ] );
	        else
	            item->setCurrentItem( 0 );
	    } else
		item->setCurrentItem( 0 );
	    table = item->currentItem();
	} else if ( item->name() == tr( "Field" ) ) {
	    TQStringList cl = listview->propertyEditor()->formWindow()->project()->databaseFieldList( conn, table );
	    if ( !cl.isEmpty() )
		item->setValue( cl );
	    else if ( lst.count() > 2 )
		item->setValue( TQStringList( lst[ 2 ] ) );
	    if ( lst.count() > 2 && !lst[ 2 ].isEmpty() )
		item->setCurrentItem( lst[ 2 ] );
	    else
		item->setCurrentItem( 0 );
	}
    }
#endif
}

PropertyDatabaseItem::~PropertyDatabaseItem()
{
    delete (TQHBox*)box;
}

void PropertyDatabaseItem::showEditor()
{
    PropertyItem::showEditor();
    placeEditor( box );
    if ( !box->isVisible() || !lined->hasFocus() ) {
	box->show();
	setFocus( lined );
    }
}

void PropertyDatabaseItem::hideEditor()
{
    PropertyItem::hideEditor();
    box->hide();
}

void PropertyDatabaseItem::setValue( const TQVariant &v )
{
    if ( value() == v )
	return;

    TQStringList lst = v.toStringList();
    TQString s = lst.join( "." );
    setText( 1, s );
    lined->setText( s );
    PropertyItem::setValue( v );
}

bool PropertyDatabaseItem::hasSubItems() const
{
    return true;
}

void PropertyDatabaseItem::childValueChanged( PropertyItem *c )
{
#ifndef TQT_NO_SQL
    TQStringList lst;
    lst << ( (PropertyListItem*)PropertyItem::child( 0 ) )->currentItem()
	<< ( (PropertyListItem*)PropertyItem::child( 1 ) )->currentItem();
    if ( withField )
	lst << ( (PropertyListItem*)PropertyItem::child( 2 ) )->currentItem();
    if ( c == PropertyItem::child( 0 ) ) { // if the connection changed
	lst[ 0 ] = ( (PropertyListItem*)c )->currentItem();
	PropertyItem::child( 1 )->setValue( listview->propertyEditor()->formWindow()->project()->databaseTableList( lst[ 0 ] ) );
	if ( withField )
	    PropertyItem::child( 2 )->setValue( listview->propertyEditor()->formWindow()->project()->databaseFieldList( lst[ 0 ], lst[ 1 ] ) );
    } else if ( withField && c == PropertyItem::child( 1 ) ) { // if the table changed
	lst[ 1 ] = ( (PropertyListItem*)c )->currentItem();
	if ( withField )
	    PropertyItem::child( 2 )->setValue( listview->propertyEditor()->formWindow()->project()->databaseFieldList( lst[ 0 ], lst[ 1 ] ) );
    }
    lst.clear();
    lst << ( (PropertyListItem*)PropertyItem::child( 0 ) )->currentItem()
	<< ( (PropertyListItem*)PropertyItem::child( 1 ) )->currentItem();
    if ( withField )
	lst << ( (PropertyListItem*)PropertyItem::child( 2 ) )->currentItem();
    setValue( lst );
    notifyValueChange();
#else
    Q_UNUSED( c );
#endif
}

// --------------------------------------------------------------

PropertySizePolicyItem::PropertySizePolicyItem( PropertyList *l, PropertyItem *after, PropertyItem *prop,
						const TQString &propName )
    : PropertyItem( l, after, prop, propName )
{
    lin = 0;
}

TQLineEdit *PropertySizePolicyItem::lined()
{
    if ( lin )
	return lin;
    lin = new TQLineEdit( listview->viewport() );
    lin->hide();
    lin->setReadOnly( true );
    return lin;
}

void PropertySizePolicyItem::createChildren()
{
    TQStringList lst;
    lst << "Fixed" << "Minimum" << "Maximum" << "Preferred" << "MinimumExpanding" << "Expanding" << "Ignored";

    PropertyItem *i = this;
    i = new PropertyListItem( listview, i, this, tr( "hSizeType" ), false );
    i->setValue( lst );
    addChild( i );
    i = new PropertyListItem( listview, i, this, tr( "vSizeType" ), false );
    i->setValue( lst );
    addChild( i );
    i = new PropertyIntItem( listview, i, this, tr( "horizontalStretch" ), true );
    addChild( i );
    i = new PropertyIntItem( listview, i, this, tr( "verticalStretch" ), true );
    addChild( i );
}

void PropertySizePolicyItem::initChildren()
{
    PropertyItem *item = 0;
    TQSizePolicy sp = val.toSizePolicy();
    for ( int i = 0; i < childCount(); ++i ) {
	item = PropertyItem::child( i );
	if ( item->name() == tr( "hSizeType" ) )
	    ( (PropertyListItem*)item )->setCurrentItem( size_type_to_int( sp.horData() ) );
	else if ( item->name() == tr( "vSizeType" ) )
	    ( (PropertyListItem*)item )->setCurrentItem( size_type_to_int( sp.verData() ) );
	else if ( item->name() == tr( "horizontalStretch" ) )
	    ( (PropertyIntItem*)item )->setValue( sp.horStretch() );
	else if ( item->name() == tr( "verticalStretch" ) )
	    ( (PropertyIntItem*)item )->setValue( sp.verStretch() );
    }
}

PropertySizePolicyItem::~PropertySizePolicyItem()
{
    delete (TQLineEdit*)lin;
}

void PropertySizePolicyItem::showEditor()
{
    PropertyItem::showEditor();
    placeEditor( lined() );
    if ( !lined()->isVisible() || !lined()->hasFocus() ) {
	lined()->show();
	listView()->viewport()->setFocus();
    }
}

void PropertySizePolicyItem::hideEditor()
{
    PropertyItem::hideEditor();
    lined()->hide();
}

void PropertySizePolicyItem::setValue( const TQVariant &v )
{
    if ( value() == v )
	return;

    TQString s = tr( "%1/%2/%2/%2" );
    s = s.arg( size_type_to_string( v.toSizePolicy().horData() ) ).
	arg( size_type_to_string( v.toSizePolicy().verData() ) ).
	arg( v.toSizePolicy().horStretch() ).
	arg( v.toSizePolicy().verStretch() );
    setText( 1, s );
    lined()->setText( s );
    PropertyItem::setValue( v );
}

void PropertySizePolicyItem::childValueChanged( PropertyItem *child )
{
    TQSizePolicy sp = val.toSizePolicy();
    if ( child->name() == tr( "hSizeType" ) )
	sp.setHorData( int_to_size_type( ( ( PropertyListItem*)child )->currentIntItem() ) );
    else if ( child->name() == tr( "vSizeType" ) )
	sp.setVerData( int_to_size_type( ( ( PropertyListItem*)child )->currentIntItem() ) );
    else if ( child->name() == tr( "horizontalStretch" ) )
	sp.setHorStretch( ( ( PropertyIntItem*)child )->value().toInt() );
    else if ( child->name() == tr( "verticalStretch" ) )
	sp.setVerStretch( ( ( PropertyIntItem*)child )->value().toInt() );
    setValue( sp );
    notifyValueChange();
}

bool PropertySizePolicyItem::hasSubItems() const
{
    return true;
}

// --------------------------------------------------------------

PropertyPaletteItem::PropertyPaletteItem( PropertyList *l, PropertyItem *after, PropertyItem *prop,
				      const TQString &propName )
    : PropertyItem( l, after, prop, propName )
{
    box = new TQHBox( listview->viewport() );
    box->hide();
    palettePrev = new TQLabel( box );
    button = new TQPushButton( "...", box );
    setupStyle( button );
    button->setFixedWidth( 20 );
    box->setFrameStyle( TQFrame::StyledPanel | TQFrame::Sunken );
    box->setLineWidth( 2 );
    palettePrev->setFrameStyle( TQFrame::NoFrame );
    box->installEventFilter( listview );
    connect( button, TQ_SIGNAL( clicked() ),
	     this, TQ_SLOT( getPalette() ) );
}
PropertyPaletteItem::~PropertyPaletteItem()
{
    delete (TQHBox*)box;
}

void PropertyPaletteItem::showEditor()
{
    PropertyItem::showEditor();
    placeEditor( box );
    if ( !box->isVisible() ) {
	box->show();
	listView()->viewport()->setFocus();
    }
}

void PropertyPaletteItem::hideEditor()
{
    PropertyItem::hideEditor();
    box->hide();
}

void PropertyPaletteItem::setValue( const TQVariant &v )
{
    TQString s;
    palettePrev->setPalette( v.toPalette() );
    PropertyItem::setValue( v );
    repaint();
}

void PropertyPaletteItem::getPalette()
{
    if ( !listview->propertyEditor()->widget()->isWidgetType() )
	return;
    bool ok = false;
    TQWidget *w = (TQWidget*)listview->propertyEditor()->widget();
    if ( ::tqt_cast<TQScrollView*>(w) )
	w = ( (TQScrollView*)w )->viewport();
    TQPalette pal = PaletteEditor::getPalette( &ok, val.toPalette(),
					      w->backgroundMode(), listview,
 					      "choose_palette", listview->propertyEditor()->formWindow() );
    if ( !ok )
	return;
    setValue( pal );
    notifyValueChange();
}

bool PropertyPaletteItem::hasCustomContents() const
{
    return true;
}

void PropertyPaletteItem::drawCustomContents( TQPainter *p, const TQRect &r )
{
    TQPalette pal( value().toPalette() );
    p->save();
    p->setClipRect( TQRect( TQPoint( (int)(p->worldMatrix().dx() + r.x()),
				   (int)(p->worldMatrix().dy() + r.y()) ),
			   r.size() ) );
    TQRect r2( r );
    r2.setX( r2.x() + 2 );
    r2.setY( r2.y() + 2 );
    r2.setWidth( r2.width() - 3 );
    r2.setHeight( r2.height() - 3 );
    p->setPen( TQPen( black, 1 ) );
    p->setBrush( pal.active().background() );
    p->drawRect( r2 );
    p->restore();
}

// --------------------------------------------------------------

PropertyCursorItem::PropertyCursorItem( PropertyList *l, PropertyItem *after, PropertyItem *prop,
					const TQString &propName )
    : PropertyItem( l, after, prop, propName )
{
    comb = 0;
}

TQComboBox *PropertyCursorItem::combo()
{
    if ( comb )
	return comb;
    comb = new TQComboBox( false, listview->viewport() );
    comb->hide();
    TQBitmap cur;

    comb->insertItem( TQPixmap::fromMimeSource( "designer_arrow.png" ), tr("Arrow"), TQObject::ArrowCursor);
    comb->insertItem( TQPixmap::fromMimeSource( "designer_uparrow.png" ), tr("Up-Arrow"), TQObject::UpArrowCursor );
    comb->insertItem( TQPixmap::fromMimeSource( "designer_cross.png" ), tr("Cross"), TQObject::CrossCursor );
    comb->insertItem( TQPixmap::fromMimeSource( "designer_wait.png" ), tr("Waiting"), TQObject::WaitCursor );
    comb->insertItem( TQPixmap::fromMimeSource( "designer_ibeam.png" ), tr("iBeam"), TQObject::IbeamCursor );
    comb->insertItem( TQPixmap::fromMimeSource( "designer_sizev.png" ), tr("Size Vertical"), TQObject::SizeVerCursor );
    comb->insertItem( TQPixmap::fromMimeSource( "designer_sizeh.png" ), tr("Size Horizontal"), TQObject::SizeHorCursor );
    comb->insertItem( TQPixmap::fromMimeSource( "designer_sizef.png" ), tr("Size Slash"), TQObject::SizeBDiagCursor );
    comb->insertItem( TQPixmap::fromMimeSource( "designer_sizeb.png" ), tr("Size Backslash"), TQObject::SizeFDiagCursor );
    comb->insertItem( TQPixmap::fromMimeSource( "designer_sizeall.png" ), tr("Size All"), TQObject::SizeAllCursor );
    cur = TQBitmap( 25, 25, 1 );
    cur.setMask( cur );
    comb->insertItem( cur, tr("Blank"), TQObject::BlankCursor );
    comb->insertItem( TQPixmap::fromMimeSource( "designer_vsplit.png" ), tr("Split Vertical"), TQObject::SplitVCursor );
    comb->insertItem( TQPixmap::fromMimeSource( "designer_hsplit.png" ), tr("Split Horizontal"), TQObject::SplitHCursor );
    comb->insertItem( TQPixmap::fromMimeSource( "designer_hand.png" ), tr("Pointing Hand"), TQObject::PointingHandCursor );
    comb->insertItem( TQPixmap::fromMimeSource( "designer_no.png" ), tr("Forbidden"), TQObject::ForbiddenCursor );

    connect( comb, TQ_SIGNAL( activated( int ) ),
	     this, TQ_SLOT( setValue() ) );
    comb->installEventFilter( listview );
    return comb;
}

PropertyCursorItem::~PropertyCursorItem()
{
    delete (TQComboBox*)comb;
}

void PropertyCursorItem::showEditor()
{
    PropertyItem::showEditor();
    if ( !comb ) {
	combo()->blockSignals( true );
	combo()->setCurrentItem( (int)value().toCursor().shape() );
	combo()->blockSignals( false );
    }
    placeEditor( combo() );
    if ( !combo()->isVisible() || !combo()->hasFocus() ) {
	combo()->show();
	setFocus( combo() );
    }
}

void PropertyCursorItem::hideEditor()
{
    PropertyItem::hideEditor();
    combo()->hide();
}

void PropertyCursorItem::setValue( const TQVariant &v )
{
    if ( ( !hasSubItems() || !isOpen() )
	 && value() == v )
	return;

    combo()->blockSignals( true );
    combo()->setCurrentItem( (int)v.toCursor().shape() );
    combo()->blockSignals( false );
    setText( 1, combo()->currentText() );
    PropertyItem::setValue( v );
}

void PropertyCursorItem::setValue()
{
    if ( !comb )
	return;
    if ( TQVariant( TQCursor( combo()->currentItem() ) ) == val )
	return;
    setText( 1, combo()->currentText() );
    PropertyItem::setValue( TQCursor( combo()->currentItem() ) );
    notifyValueChange();
}

// --------------------------------------------------------------

PropertyKeysequenceItem::PropertyKeysequenceItem( PropertyList *l,
						  PropertyItem *after,
						  PropertyItem *prop,
						  const TQString &propName )
    : PropertyItem( l, after, prop, propName ),
      k1( 0 ), k2( 0 ), k3( 0 ), k4( 0 ), num( 0 ), mouseEnter( false )
{
    box = new TQHBox( listview->viewport() );
    box->hide();
    sequence = new TQLineEdit( box );
    connect( sequence, TQ_SIGNAL(textChanged( const TQString & )),
	     this, TQ_SLOT(setValue()) );
    sequence->installEventFilter( this );
}

PropertyKeysequenceItem::~PropertyKeysequenceItem()
{
    delete (TQHBox*)box;
}

void PropertyKeysequenceItem::showEditor()
{
    PropertyItem::showEditor();
    placeEditor( box );
    if ( !box->isVisible() ) {
	box->show();
	sequence->setFocus();
    }
}

void PropertyKeysequenceItem::hideEditor()
{
    PropertyItem::hideEditor();
    box->hide();
}

bool PropertyKeysequenceItem::eventFilter( TQObject *o, TQEvent *e )
{
    Q_UNUSED( o );
    if ( e->type() == TQEvent::KeyPress ) {
        TQKeyEvent *k = (TQKeyEvent *)e;
	if ( !mouseEnter &&
	    (k->key() == TQObject::Key_Up ||
	     k->key() == TQObject::Key_Down) )
	    return false;
        handleKeyEvent( k );
        return true;
    } else if ( (e->type() == TQEvent::FocusIn) ||
		(e->type() == TQEvent::MouseButtonPress) ) {
	mouseEnter = ( listview->lastEvent() == PropertyList::MouseEvent ) ||
		     (e->type() == TQEvent::MouseButtonPress);
	return true;
    }

    // Lets eat accelerators now..
    if ( e->type() == TQEvent::Accel ||
	 e->type() == TQEvent::AccelOverride  ||
	 e->type() == TQEvent::KeyRelease )
	return true;
    return false;
}

void PropertyKeysequenceItem::handleKeyEvent( TQKeyEvent *e )
{
    int nextKey = e->key();

    if ( num > 3 ||
 	 nextKey == TQObject::Key_Control ||
 	 nextKey == TQObject::Key_Shift ||
 	 nextKey == TQObject::Key_Meta ||
 	 nextKey == TQObject::Key_Alt)
	 return;

    int modifier = translateModifiers( e->state() );
    if (nextKey == TQObject::Key_BackTab)
        modifier &= ~TQObject::SHIFT;

    nextKey |= modifier;
    switch( num ) {
	case 0:
	    k1 = nextKey;
	    break;
	case 1:
	    k2 = nextKey;
	    break;
	case 2:
	    k3 = nextKey;
	    break;
	case 3:
	    k4 = nextKey;
	    break;
	default:
	    break;
    }
    num++;
    TQKeySequence ks( k1, k2, k3, k4 );
    sequence->setText( ks );
}

int PropertyKeysequenceItem::translateModifiers( int state )
{
    int result = 0;
    if ( state & TQObject::ShiftButton )
	result |= TQObject::SHIFT;
    if ( state & TQObject::ControlButton )
	result |= TQObject::CTRL;
    if ( state & TQObject::MetaButton )
	result |= TQObject::META;
    if ( state & TQObject::AltButton )
	result |= TQObject::ALT;
    return result;
}

void PropertyKeysequenceItem::setValue()
{
    TQVariant v;
    v = TQVariant( TQKeySequence(sequence->text()) );
    if ( v.toString().isNull() )
	return;
    setText( 1, sequence->text() );
    PropertyItem::setValue( v );
    if (sequence->hasFocus()
        // if the user wants to use F1 as accelerator, Designer will pop up a TQWhatsThat widget
        || (tqApp->focusWidget() && tqApp->focusWidget()->inherits("TQWhatsThat")))
        notifyValueChange();
    setChanged( true );
}

void PropertyKeysequenceItem::setValue( const TQVariant &v )
{
    TQKeySequence ks = v.toKeySequence();
    if ( sequence ) {
	sequence->setText( ks );
    }
    num = ks.count();
    k1 = ks[0];
    k2 = ks[1];
    k3 = ks[2];
    k4 = ks[3];
    setText( 1, ks );
    PropertyItem::setValue( v );
}

// --------------------------------------------------------------

EnumPopup::EnumPopup( TQWidget *parent, const char *name, WFlags f )
    : TQFrame( parent, name, f )
{
    setLineWidth( 1 );
    setFrameStyle( Panel | Plain );
    setPaletteBackgroundColor( TQt::white );
    popLayout = new TQVBoxLayout( this, 3 );
    checkBoxList.setAutoDelete( true );
}

EnumPopup::~EnumPopup()
{
}

void EnumPopup::insertEnums( TQValueList<EnumItem> lst )
{
    while ( checkBoxList.count() )
	checkBoxList.removeFirst();

    itemList = lst;
    TQCheckBox *cb;
    TQValueListConstIterator<EnumItem> it = itemList.begin();
    for ( ; it != itemList.end(); ++it ) {
	cb = new TQCheckBox( this );
	cb->setText( (*it).key );
	cb->setChecked( (*it).selected );
	if ( it == itemList.begin() )
	    cb->setFocus();
	checkBoxList.append( cb );
	cb->resize( width(), cb->height() );
	popLayout->addWidget( cb );
    }
}

void EnumPopup::keyPressEvent( TQKeyEvent *e )
{
    if ( e->key() == Key_Escape ) {
	hide();
	emit hidden();
    } else if ( e->key() == Key_Enter || e->key() == Key_Return ) {
	closeWidget();
    }
}

void EnumPopup::closeWidget()
{
    TQPtrListIterator<TQCheckBox> it( checkBoxList );
    int i = 0;
    while ( it.current() != 0 ) {
	itemList[i].selected = (*it)->isChecked();
	++it;
	++i;
    }
    close();
    emit closed();
}

TQValueList<EnumItem> EnumPopup::enumList() const
{
    return itemList;
}

EnumBox::EnumBox( TQWidget *parent, const char *name )
    : TQComboBox( parent, name )
{
    pop = new EnumPopup( this, "popup", TQObject::WType_Popup );
    connect( pop, TQ_SIGNAL( hidden() ), this, TQ_SLOT( popupHidden() ) );
    connect( pop, TQ_SIGNAL( closed() ), this, TQ_SLOT( popupClosed() ) );
    popupShown = false;
    arrowDown = false;
}

void EnumBox::popupHidden()
{
    popupShown = false;
}

void EnumBox::popupClosed()
{
    popupShown = false;
    emit valueChanged();
}

void EnumBox::paintEvent( TQPaintEvent * )
{
    TQPainter p( this );
    const TQColorGroup & g = colorGroup();
    p.setPen(g.text());

    TQStyle::SFlags flags = TQStyle::Style_Default;
    if (isEnabled())
	flags |= TQStyle::Style_Enabled;
    if (hasFocus())
	flags |= TQStyle::Style_HasFocus;

    if ( width() < 5 || height() < 5 ) {
	qDrawShadePanel( &p, rect().x(), rect().y(), rect().width(), rect().height(), g, false, 2,
		&g.brush( TQColorGroup::Button ) );
	return;
    }
    style().drawComplexControl( TQStyle::CC_ComboBox, &p, this, rect(), g,
				    flags, TQStyle::SC_All,
				    (arrowDown ?
				     TQStyle::SC_ComboBoxArrow :
				     TQStyle::SC_None ));

    TQRect re = style().querySubControlMetrics( TQStyle::CC_ComboBox, this,
	    TQStyle::SC_ComboBoxEditField );
    re = TQStyle::visualRect(re, this);
    p.setClipRect( re );

    if ( !str.isNull() ) {
	p.save();
	p.setFont(font());
	TQFontMetrics fm(font());
	int x = re.x(), y = re.y() + fm.ascent();
	p.drawText( x, y, str );
	p.restore();
    }
}

void EnumBox::insertEnums( TQValueList<EnumItem> lst )
{
    pop->insertEnums( lst );
}

TQValueList<EnumItem> EnumBox::enumList() const
{
    return pop->enumList();
}

void EnumBox::popup()
{
    if ( popupShown ) {
	pop->closeWidget();
	popupShown = false;
	return;
    }
    pop->move( ((TQWidget*)parent())->mapToGlobal( geometry().bottomLeft() ) );
    pop->setMinimumWidth( width() );
    emit aboutToShowPopup();
    pop->show();
    popupShown = true;
}

void EnumBox::mousePressEvent( TQMouseEvent *e )
{
    if ( e->button() != LeftButton )
	return;

    TQRect arrowRect = style().querySubControlMetrics( TQStyle::CC_ComboBox, this,
						      TQStyle::SC_ComboBoxArrow);
    arrowRect = TQStyle::visualRect(arrowRect, this);

    arrowRect.setHeight( TQMAX(  height() - (2 * arrowRect.y()), arrowRect.height() ) );

    if ( arrowRect.contains( e->pos() ) ) {
	arrowDown = true;
	repaint( false );
    }

    popup();
    TQTimer::singleShot( 100, this, TQ_SLOT( restoreArrow() ) );
}

void EnumBox::keyPressEvent( TQKeyEvent *e )
{
    if ( e->key() == Key_Space ) {
	popup();
	TQTimer::singleShot( 100, this, TQ_SLOT( restoreArrow() ) );
    } else if ( e->key() == Key_Enter || e->key() == Key_Return ) {
	popup();
    }
}

void EnumBox::restoreArrow()
{
    arrowDown = false;
    repaint( false );
}

void EnumBox::setText( const TQString &text )
{
    str = text;
    repaint( false );
}



PropertyEnumItem::PropertyEnumItem( PropertyList *l,
				    PropertyItem *after,
				    PropertyItem *prop,
				    const TQString &propName )
    : PropertyItem( l, after, prop, propName )
{
    box = new EnumBox( listview->viewport() );
    box->hide();
    box->installEventFilter( listview );
    connect( box, TQ_SIGNAL( aboutToShowPopup() ), this, TQ_SLOT( insertEnums() ) );
    connect( box, TQ_SIGNAL( valueChanged() ), this, TQ_SLOT( setValue() ) );
}

PropertyEnumItem::~PropertyEnumItem()
{
    delete (EnumBox*)box;
}

void PropertyEnumItem::showEditor()
{
    PropertyItem::showEditor();
    placeEditor( box );
    if ( !box->isVisible() ) {
	box->show();
	box->setText( enumString );
	listView()->viewport()->setFocus();
    }
    box->setFocus();
}

void PropertyEnumItem::hideEditor()
{
    PropertyItem::hideEditor();
    box->hide();
}

void PropertyEnumItem::setValue( const TQVariant &v )
{
    enumString = "";
    enumList.clear();
    TQStringList lst = v.toStringList();
    TQValueListConstIterator<TQString> it = lst.begin();
    for ( ; it != lst.end(); ++it )
	enumList.append( EnumItem( *it, false ) );
    enumList.first().selected = true;
    enumString = enumList.first().key;
    box->setText( enumString );
    setText( 1, enumString );
    PropertyItem::setValue( v );
}

void PropertyEnumItem::insertEnums()
{
    box->insertEnums( enumList );
}

void PropertyEnumItem::setValue()
{
    enumList = box->enumList();
    enumString = "";
    TQValueListConstIterator<EnumItem> it = enumList.begin();
    for ( ; it != enumList.end(); ++it ) {
	if ( (*it).selected )
	    enumString += "|" + (*it).key;
    }
    if ( !enumString.isEmpty() )
	enumString.replace( 0, 1, "" );

    box->setText( enumString );
    setText( 1, enumString );
    notifyValueChange();
}

void PropertyEnumItem::setCurrentValues( TQStrList lst )
{
    enumString ="";
    TQStrList::ConstIterator it = lst.begin();
    TQValueList<EnumItem>::Iterator eit = enumList.begin();
    for ( ; eit != enumList.end(); ++eit ) {
	(*eit).selected = false;
	for ( it = lst.begin(); it != lst.end(); ++it ) {
	    if ( TQString( *it ) == (*eit).key ) {
		(*eit).selected = true;
		enumString += "|" + (*eit).key;
		break;
	    }
	}
    }
    if ( !enumString.isEmpty() )
	enumString.replace( 0, 1, "" );
    box->setText( enumString );
    setText( 1, enumString );
}

TQString PropertyEnumItem::currentItem() const
{
    return enumString;
}

TQString PropertyEnumItem::currentItemFromObject() const
{
    return enumString;
}

// --------------------------------------------------------------

/*!
  \class PropertyList propertyeditor.h
  \brief PropertyList is a TQListView derived class which is used for editing widget properties

  This class is used for widget properties. It has to be child of a
  PropertyEditor.

  To initialize it for editing a widget call setupProperties() which
  iterates through the properties of the current widget (see
  PropertyEditor::widget()) and builds the list.

  To update the item values, refetchData() can be called.

  If the value of an item has been changed by the user, and this
  change should be applied to the widget's property, valueChanged()
  has to be called.

  To set the value of an item, setPropertyValue() has to be called.
*/

PropertyList::PropertyList( PropertyEditor *e )
    : TQListView( e ), editor( e )
{
    init_colors();

    whatsThis = new PropertyWhatsThis( this );
    showSorted = false;
    header()->setMovingEnabled( false );
    header()->setStretchEnabled( true );
    setResizePolicy( TQScrollView::Manual );
    viewport()->setAcceptDrops( true );
    viewport()->installEventFilter( this );
    addColumn( tr( "Property" ) );
    addColumn( tr( "Value" ) );
    connect( header(), TQ_SIGNAL( sizeChange( int, int, int ) ),
	     this, TQ_SLOT( updateEditorSize() ) );
    disconnect( header(), TQ_SIGNAL( sectionClicked( int ) ),
		this, TQ_SLOT( changeSortColumn( int ) ) );
    connect( header(), TQ_SIGNAL( sectionClicked( int ) ),
	     this, TQ_SLOT( toggleSort() ) );
    connect( this, TQ_SIGNAL( pressed( TQListViewItem *, const TQPoint &, int ) ),
	     this, TQ_SLOT( itemPressed( TQListViewItem *, const TQPoint &, int ) ) );
    connect( this, TQ_SIGNAL( doubleClicked( TQListViewItem * ) ),
	     this, TQ_SLOT( toggleOpen( TQListViewItem * ) ) );
    setSorting( -1 );
    setHScrollBarMode( AlwaysOff );
    setVScrollBarMode( AlwaysOn );
    setColumnWidthMode( 1, Manual );
    mousePressed = false;
    pressItem = 0;
    theLastEvent = MouseEvent;
    header()->installEventFilter( this );
}

void PropertyList::toggleSort()
{
    showSorted = !showSorted;
    editor->clear();
    editor->setup();
}

void PropertyList::resizeEvent( TQResizeEvent *e )
{
    TQListView::resizeEvent( e );
    if ( currentItem() )
	( ( PropertyItem* )currentItem() )->showEditor();
}

static TQVariant::Type type_to_variant( const TQString &s )
{
    if ( s == "Invalid " )
	return TQVariant::Invalid;
    if ( s == "Map" )
	return TQVariant::Map;
    if ( s == "List" )
	return TQVariant::List;
    if ( s == "String" )
	return TQVariant::String;
    if ( s == "StringList" )
	return TQVariant::StringList;
    if ( s == "Font" )
	return TQVariant::Font;
    if ( s == "Pixmap" )
	return TQVariant::Pixmap;
    if ( s == "Brush" )
	return TQVariant::Brush;
    if ( s == "Rect" )
	return TQVariant::Rect;
    if ( s == "Size" )
	return TQVariant::Size;
    if ( s == "Color" )
	return TQVariant::Color;
    if ( s == "Palette" )
	return TQVariant::Palette;
    if ( s == "ColorGroup" )
	return TQVariant::ColorGroup;
    if ( s == "IconSet" )
	return TQVariant::IconSet;
    if ( s == "Point" )
	return TQVariant::Point;
    if ( s == "Image" )
	return TQVariant::Image;
    if ( s == "Int" )
	return TQVariant::Int;
    if ( s == "UInt" )
	return TQVariant::UInt;
    if ( s == "Bool" )
	return TQVariant::Bool;
    if ( s == "Double" )
	return TQVariant::Double;
    if ( s == "CString" )
	return TQVariant::CString;
    if ( s == "PointArray" )
	return TQVariant::PointArray;
    if ( s == "Region" )
	return TQVariant::Region;
    if ( s == "Bitmap" )
	return TQVariant::Bitmap;
    if ( s == "Cursor" )
	return TQVariant::Cursor;
    if ( s == "SizePolicy" )
	return TQVariant::SizePolicy;
    if ( s == "Date" )
	return TQVariant::Date;
    if ( s == "Time" )
	return TQVariant::Time;
    if ( s == "DateTime" )
	return TQVariant::DateTime;
    return TQVariant::Invalid;
}

#ifndef TQT_NO_SQL
static bool parent_is_data_aware( TQWidget *w )
{
    TQWidget *p = w ? w->parentWidget() : 0;
    while ( p && !p->isTopLevel() ) {
	if ( ::tqt_cast<TQDesignerDataBrowser*>(p) || ::tqt_cast<TQDesignerDataView*>(p) )
	    return true;
	p = p->parentWidget();
    }
    return false;
}
#endif

/*!  Sets up the property list by adding an item for each designable
property of the widget which is just edited.
*/

void PropertyList::setupProperties()
{
    if ( !editor->widget() )
	return;
    bool allProperties = !::tqt_cast<Spacer*>(editor->widget());
    TQStrList lst = editor->widget()->metaObject()->propertyNames( allProperties );
    PropertyItem *item = 0;
    TQMap<TQString, bool> unique;
    TQObject *w = editor->widget();
    TQStringList valueSet;
    bool parentHasLayout =
	w->isWidgetType() &&
	!editor->formWindow()->isMainContainer( (TQWidget*)w ) && ( (TQWidget*)w )->parentWidget() &&
	WidgetFactory::layoutType( ( (TQWidget*)w )->parentWidget() ) != WidgetFactory::NoLayout;
    for ( TQPtrListIterator<char> it( lst ); it.current(); ++it ) {
	const TQMetaProperty* p =
	    editor->widget()->metaObject()->
	    property( editor->widget()->metaObject()->findProperty( it.current(), allProperties), allProperties );
	if ( !p )
	    continue;
	if ( unique.contains( TQString::fromLatin1( it.current() ) ) )
	    continue;
	if ( ::tqt_cast<TQDesignerToolBar*>(editor->widget()) ||
	     ::tqt_cast<MenuBarEditor*>(editor->widget()) ) {
	    if ( qstrcmp( p->name(), "minimumHeight" ) == 0 )
		continue;
	    if ( qstrcmp( p->name(), "minimumWidth" ) == 0 )
		continue;
	    if ( qstrcmp( p->name(), "maximumHeight" ) == 0 )
		continue;
	    if ( qstrcmp( p->name(), "maximumWidth" ) == 0 )
		continue;
	    if ( qstrcmp( p->name(), "geometry" ) == 0 )
		continue;
	    if ( qstrcmp( p->name(), "sizePolicy" ) == 0 )
		continue;
	    if ( qstrcmp( p->name(), "minimumSize" ) == 0 )
		continue;
	    if ( qstrcmp( p->name(), "maximumSize" ) == 0 )
		continue;
	}
	unique.insert( TQString::fromLatin1( it.current() ), true );
	if ( editor->widget()->isWidgetType() &&
	     editor->formWindow()->isMainContainer( (TQWidget*)editor->widget() ) ) {
	    if ( qstrcmp( p->name(), "geometry" ) == 0 )
		continue;
	} else { // hide some toplevel-only stuff
	    if ( qstrcmp( p->name(), "icon" ) == 0 )
		continue;
	    if ( qstrcmp( p->name(), "iconText" ) == 0 )
		continue;
	    if ( qstrcmp( p->name(), "caption" ) == 0 )
		continue;
	    if ( qstrcmp( p->name(), "sizeIncrement" ) == 0 )
		continue;
	    if ( qstrcmp( p->name(), "baseSize" ) == 0 )
		continue;
	    if ( parentHasLayout && qstrcmp( p->name(), "geometry" ) == 0 )
		continue;
	    if ( ::tqt_cast<TQLayoutWidget*>(w) || ::tqt_cast<Spacer*>(w) ) {
		if ( qstrcmp( p->name(), "sizePolicy" ) == 0 )
		    continue;
		if ( qstrcmp( p->name(), "minimumHeight" ) == 0 )
		    continue;
		if ( qstrcmp( p->name(), "minimumWidth" ) == 0 )
		    continue;
		if ( qstrcmp( p->name(), "maximumHeight" ) == 0 )
		    continue;
		if ( qstrcmp( p->name(), "maximumWidth" ) == 0 )
		    continue;
		if ( qstrcmp( p->name(), "geometry" ) == 0 )
		    continue;
		if ( qstrcmp( p->name(), "minimumSize" ) == 0 )
		    continue;
		if ( qstrcmp( p->name(), "maximumSize" ) == 0 )
		    continue;
		if ( qstrcmp( p->name(), "enabled" ) == 0 )
		    continue;
		if ( qstrcmp( p->name(), "paletteForegroundColor" ) == 0 )
		    continue;
		if ( qstrcmp( p->name(), "paletteBackgroundColor" ) == 0 )
		    continue;
		if ( qstrcmp( p->name(), "paletteBackgroundPixmap" ) == 0 )
		    continue;
		if ( qstrcmp( p->name(), "palette" ) == 0 )
		    continue;
		if ( qstrcmp( p->name(), "font" ) == 0 )
		    continue;
		if ( qstrcmp( p->name(), "cursor" ) == 0 )
		    continue;
		if ( qstrcmp( p->name(), "mouseTracking" ) == 0 )
		    continue;
		if ( qstrcmp( p->name(), "focusPolicy" ) == 0 )
		    continue;
		if ( qstrcmp( p->name(), "acceptDrops" ) == 0 )
		    continue;
		if ( qstrcmp( p->name(), "autoMask" ) == 0 )
		    continue;
		if ( qstrcmp( p->name(), "backgroundOrigin" ) == 0 )
		    continue;
	    }
	}
	if ( ::tqt_cast<TQActionGroup*>(w) ) {
	    if ( qstrcmp( p->name(), "usesDropDown" ) == 0 )
		continue;
	    if ( qstrcmp( p->name(), "toggleAction" ) == 0 )
		continue;
	}
	if ( qstrcmp( p->name(), "minimumHeight" ) == 0 )
	    continue;
	if ( qstrcmp( p->name(), "minimumWidth" ) == 0 )
	    continue;
	if ( qstrcmp( p->name(), "maximumHeight" ) == 0 )
	    continue;
	if ( qstrcmp( p->name(), "maximumWidth" ) == 0 )
	    continue;
	if ( qstrcmp( p->name(), "buttonGroupId" ) == 0 ) { // #### remove this when designable in TQ_PROPERTY can take a function (isInButtonGroup() in this case)
	    if ( !editor->widget()->isWidgetType() ||
		 !editor->widget()->parent() ||
		 !::tqt_cast<TQButtonGroup*>(editor->widget()->parent()) )
		continue;
	}

	bool isPropertyObject = w->isA( "PropertyObject" );

	if ( ( p->designable(w) ||
	       ( isPropertyObject && p->designable( ( (PropertyObject*)w )->widgetList().first() ) ) ) &&
	     ( !isPropertyObject || qstrcmp( p->name(), "name" ) != 0 ) ) {
	    if ( p->isSetType() ) {
		if ( TQString( p->name() ) == "alignment" ) {
		    TQStringList lst;
		    lst << p->valueToKey( AlignAuto )
			<< p->valueToKey( AlignLeft )
			<< p->valueToKey( AlignHCenter )
			<< p->valueToKey( AlignRight )
			<< p->valueToKey( AlignJustify );
		    item = new PropertyListItem( this, item, 0, "hAlign", false );
		    item->setValue( lst );
		    setPropertyValue( item );
		    if ( MetaDataBase::isPropertyChanged( editor->widget(), "hAlign" ) )
			item->setChanged( true, false );
		    if ( !::tqt_cast<TQMultiLineEdit*>(editor->widget()) ) {
			lst.clear();
			lst << p->valueToKey( AlignTop )
			    << p->valueToKey( AlignVCenter )
			    << p->valueToKey( AlignBottom );
			item = new PropertyListItem( this, item, 0, "vAlign", false );
			item->setValue( lst );
			setPropertyValue( item );
			if ( MetaDataBase::isPropertyChanged( editor->widget(), "vAlign" ) )
			    item->setChanged( true, false );
			item = new PropertyBoolItem( this, item, 0, "wordwrap" );
			if ( ::tqt_cast<TQGroupBox*>(w) )
			    item->setVisible( false );
			setPropertyValue( item );
			if ( MetaDataBase::isPropertyChanged( editor->widget(), "wordwrap" ) )
			    item->setChanged( true, false );
		    }
		} else {
		    TQStrList lst( p->enumKeys() );
		    TQStringList l;
		    TQPtrListIterator<char> it( lst );
		    while ( it.current() != 0 ) {
			l << TQString(*it);
			++it;
		    }
		    item = new PropertyEnumItem( this, item, 0, p->name() );
		    item->setValue( l );
		    setPropertyValue( item );
		    if ( MetaDataBase::isPropertyChanged( editor->widget(), p->name() ) )
			item->setChanged( true, false );
		}
	    } else if ( p->isEnumType() ) {
		TQStrList l = p->enumKeys();
		TQStringList lst;
		for ( uint i = 0; i < l.count(); ++i ) {
		    TQString k = l.at( i );
		    // filter out enum-masks
		    if ( k[0] == 'M' && k[1].category() == TQChar::Letter_Uppercase )
			continue;
		    lst << k;
		}
		item = new PropertyListItem( this, item, 0, p->name(), false );
		item->setValue( lst );
	    } else {
		TQVariant::Type t = TQVariant::nameToType( p->type() );
		if ( !addPropertyItem( item, p->name(), t ) )
		    continue;
	    }
	}
	if ( item && !p->isSetType() ) {
	    if ( valueSet.findIndex( item->name() ) == -1 ) {
		setPropertyValue( item );
		valueSet << item->name();
	    }
	    if ( MetaDataBase::isPropertyChanged( editor->widget(), p->name() ) )
		item->setChanged( true, false );
	}
    }

    if ( !::tqt_cast<TQSplitter*>(w) && !::tqt_cast<MenuBarEditor*>(w) && !::tqt_cast<TQDesignerToolBar*>(w) &&
	 w->isWidgetType() && WidgetFactory::layoutType( (TQWidget*)w ) != WidgetFactory::NoLayout ) {
	item = new PropertyLayoutItem( this, item, 0, "layoutSpacing" );
	setPropertyValue( item );
	if ( MetaDataBase::isPropertyChanged( editor->widget(), "layoutSpacing" )
	     || MetaDataBase::spacing( editor->widget() ) != -1 )
	    layoutInitValue( item, true );
	else
	    layoutInitValue( item );
	item = new PropertyLayoutItem( this, item, 0, "layoutMargin" );
	setPropertyValue( item );
	if ( MetaDataBase::isPropertyChanged( editor->widget(), "layoutMargin" )
	     || MetaDataBase::margin( editor->widget() ) != -1 )
	    layoutInitValue( item, true );
	else
	    layoutInitValue( item );
	TQWidget *widget = (TQWidget*)w;
	if ( ( !::tqt_cast<TQLayoutWidget*>(widget) &&
	       widget->parentWidget() && ::tqt_cast<FormWindow*>(widget->parentWidget()) ) ) {
	    item = new PropertyListItem( this, item, 0, "resizeMode", false );
	    TQStringList lst;
	    lst << "Auto" << "FreeResize" << "Minimum" << "Fixed";
	    item->setValue( lst );
	    setPropertyValue( item );
	    TQString resizeMod = MetaDataBase::resizeMode( editor->widget() );
	    if ( !resizeMod.isEmpty() &&
		 resizeMod != WidgetFactory::defaultCurrentItem( editor->widget(), "resizeMode" ) ) {
		item->setChanged( true, false );
		MetaDataBase::setPropertyChanged( editor->widget(), "resizeMode", true );
	    }
	}
    }

    if ( !::tqt_cast<Spacer*>(w) && !::tqt_cast<TQLayoutWidget*>(w) &&
	 !::tqt_cast<MenuBarEditor*>(w) && !::tqt_cast<TQDesignerToolBar*>(w) ) {
	item = new PropertyTextItem( this, item, 0, "toolTip", true, false );
	setPropertyValue( item );
	if ( MetaDataBase::isPropertyChanged( editor->widget(), "toolTip" ) )
	    item->setChanged( true, false );
	item = new PropertyTextItem( this, item, 0, "whatsThis", true, true );
	setPropertyValue( item );
	if ( MetaDataBase::isPropertyChanged( editor->widget(), "whatsThis" ) )
	    item->setChanged( true, false );
    }

#ifndef TQT_NO_SQL
    if ( !::tqt_cast<TQDataTable*>(editor->widget()) && !::tqt_cast<TQDataBrowser*>(editor->widget()) &&
	 !::tqt_cast<TQDataView*>(editor->widget()) && parent_is_data_aware( ::tqt_cast<TQWidget*>(editor->widget()) ) ) {
	item = new PropertyDatabaseItem( this, item, 0, "database", editor->formWindow()->mainContainer() != w );
	setPropertyValue( item );
	if ( MetaDataBase::isPropertyChanged( editor->widget(), "database" ) )
	    item->setChanged( true, false );
    }

    if ( ::tqt_cast<TQDataTable*>(editor->widget()) || ::tqt_cast<TQDataBrowser*>(editor->widget()) ||
	 ::tqt_cast<TQDataView*>(editor->widget()) ) {
	item = new PropertyDatabaseItem( this, item, 0, "database", false );
	setPropertyValue( item );
	if ( MetaDataBase::isPropertyChanged( editor->widget(), "database" ) )
	    item->setChanged( true, false );
	item = new PropertyBoolItem( this, item, 0, "frameworkCode" );
	setPropertyValue( item );
	if ( MetaDataBase::isPropertyChanged( editor->widget(), "frameworkCode" ) )
	    item->setChanged( true, false );
    }
#endif

    if ( w->isA("PropertyObject") ) {
	const TQWidgetList wl = ( (PropertyObject*)w )->widgetList();
	TQPtrListIterator<TQWidget> wIt( wl );
	while ( *wIt ) {
	    if ( (*wIt)->inherits("CustomWidget") ) {
		MetaDataBase::CustomWidget *cw = ( (CustomWidget*)*wIt )->customWidget();
		setupCusWidgetProperties( cw, unique, item );
	    }
	    ++wIt;
	}
    } else if ( w->inherits( "CustomWidget" ) ) {
	MetaDataBase::CustomWidget *cw = ( (CustomWidget*)w )->customWidget();
	setupCusWidgetProperties( cw, unique, item );
    }

    setCurrentItem( firstChild() );

    if ( showSorted ) {
	setSorting( 0 );
	sort();
	setSorting( -1 );
	setCurrentItem( firstChild() );
	tqApp->processEvents();
    }

    updateEditorSize();
}

void PropertyList::setupCusWidgetProperties( MetaDataBase::CustomWidget *cw,
					     TQMap<TQString, bool> &unique,
					     PropertyItem *&item )
{
    if ( !cw )
	return;

    for ( TQValueList<MetaDataBase::Property>::Iterator it =
	  cw->lstProperties.begin(); it != cw->lstProperties.end(); ++it ) {
	if ( unique.contains( TQString( (*it).property ) ) )
	    continue;
	unique.insert( TQString( (*it).property ), true );
	addPropertyItem( item, (*it).property, type_to_variant( (*it).type ) );
	setPropertyValue( item );
	if ( MetaDataBase::isPropertyChanged( editor->widget(), (*it).property ) )
	    item->setChanged( true, false );
    }
}

bool PropertyList::addPropertyItem( PropertyItem *&item, const TQCString &name, TQVariant::Type t )
{
    if ( name == "buddy" ) {
	PropertyListItem *itm = new PropertyListItem( this, item, 0, name, true );
	TQPtrDict<TQWidget> *widgets = editor->formWindow()->widgets();
	TQPtrDictIterator<TQWidget> it( *widgets );
	TQStringList l;
	l << "";
	while ( it.current() ) {
	    if ( editor->formWindow()->canBeBuddy( it.current() ) ) {
		if ( l.find( it.current()->name() ) == l.end() )
		    l << it.current()->name();
	    }
	    ++it;
	}
	l.sort();
	itm->setValue( l );
	item = itm;
	return true;
    }

    switch ( t ) {
    case TQVariant::String:
	item = new PropertyTextItem( this, item, 0, name, true,
				     ::tqt_cast<TQLabel*>(editor->widget()) ||
				     ::tqt_cast<TQTextView*>(editor->widget()) ||
				     ::tqt_cast<TQButton*>(editor->widget()) );
	break;
    case TQVariant::CString:
	item = new PropertyTextItem( this, item, 0,
				     name, name == "name" &&
				     editor->widget() == editor->formWindow()->mainContainer(),
				     false, true );
	break;
    case TQVariant::Bool:
	item = new PropertyBoolItem( this, item, 0, name );
	break;
    case TQVariant::Font:
	item = new PropertyFontItem( this, item, 0, name );
	break;
    case TQVariant::Int:
	if ( name == "accel" )
	    item = new PropertyTextItem( this, item, 0, name, false, false, false, true );
	else if ( name == "layoutSpacing" || name == "layoutMargin" )
	    item = new PropertyLayoutItem( this, item, 0, name );
	else if ( name == "resizeMode" )
	    item = new PropertyListItem( this, item, 0, name, true );
	else
	    item = new PropertyIntItem( this, item, 0, name, true );
	break;
    case TQVariant::Double:
	item = new PropertyDoubleItem( this, item, 0, name );
	break;
    case TQVariant::KeySequence:
	item = new PropertyKeysequenceItem( this, item, 0, name );
	break;
    case TQVariant::UInt:
	item = new PropertyIntItem( this, item, 0, name, false );
	break;
    case TQVariant::StringList:
	item = new PropertyListItem( this, item, 0, name, true );
	break;
    case TQVariant::Rect:
	item = new PropertyCoordItem( this, item, 0, name, PropertyCoordItem::Rect );
	break;
    case TQVariant::Point:
	item = new PropertyCoordItem( this, item, 0, name, PropertyCoordItem::Point );
	break;
    case TQVariant::Size:
	item = new PropertyCoordItem( this, item, 0, name, PropertyCoordItem::Size );
	break;
    case TQVariant::Color:
	item = new PropertyColorItem( this, item, 0, name, true );
	break;
    case TQVariant::Pixmap:
	item = new PropertyPixmapItem( this, item, 0, name, PropertyPixmapItem::Pixmap );
	break;
    case TQVariant::IconSet:
	item = new PropertyPixmapItem( this, item, 0, name, PropertyPixmapItem::IconSet );
	break;
    case TQVariant::Image:
	item = new PropertyPixmapItem( this, item, 0, name, PropertyPixmapItem::Image );
	break;
    case TQVariant::SizePolicy:
	item = new PropertySizePolicyItem( this, item, 0, name );
	break;
    case TQVariant::Palette:
	item = new PropertyPaletteItem( this, item, 0, name );
	break;
    case TQVariant::Cursor:
	item = new PropertyCursorItem( this, item, 0, name );
	break;
    case TQVariant::Date:
	item = new PropertyDateItem( this, item, 0, name );
	break;
    case TQVariant::Time:
	item = new PropertyTimeItem( this, item, 0, name );
	break;
    case TQVariant::DateTime:
	item = new PropertyDateTimeItem( this, item, 0, name );
	break;
    default:
	return false;
    }
    return true;
}

void PropertyList::paintEmptyArea( TQPainter *p, const TQRect &r )
{
    p->fillRect( r, *backColor2 );
}

void PropertyList::setCurrentItem( TQListViewItem *i )
{
    if ( !i )
	return;

    if ( currentItem() )
	( (PropertyItem*)currentItem() )->hideEditor();
    TQListView::setCurrentItem( i );
    ( (PropertyItem*)currentItem() )->showEditor();
}

void PropertyList::updateEditorSize()
{
    if ( currentItem() )
	( ( PropertyItem* )currentItem() )->showEditor();
}

/*!  This has to be called if the value if \a i should be set as
  property to the currently edited widget.
*/

void PropertyList::valueChanged( PropertyItem *i )
{
    if ( !editor->widget() )
	return;
    TQString pn( tr( "Set '%1' of '%2'" ).arg( i->name() ).arg( editor->widget()->name() ) );
    SetPropertyCommand *cmd = new SetPropertyCommand( pn, editor->formWindow(),
						      editor->widget(), editor,
						      i->name(), WidgetFactory::property( editor->widget(), i->name() ),
						      i->value(), i->currentItem(), i->currentItemFromObject() );

    cmd->execute();
    editor->formWindow()->commandHistory()->addCommand( cmd, true );
}

void PropertyList::layoutInitValue( PropertyItem *i, bool changed )
{
    if ( !editor->widget() )
	return;
    TQString pn( tr( "Set '%1' of '%2'" ).arg( i->name() ).arg( editor->widget()->name() ) );
    SetPropertyCommand *cmd = new SetPropertyCommand( pn, editor->formWindow(),
						      editor->widget(), editor,
						      i->name(), WidgetFactory::property( editor->widget(), i->name() ),
						      i->value(), i->currentItem(), i->currentItemFromObject() );
    cmd->execute();
    if ( i->value().toString() != "-1" )
	changed = true;
    i->setChanged( changed );
}

void PropertyList::itemPressed( TQListViewItem *i, const TQPoint &p, int c )
{
    if ( !i )
	return;
    PropertyItem *pi = (PropertyItem*)i;
    if ( !pi->hasSubItems() )
	return;

    if ( c == 0 && viewport()->mapFromGlobal( p ).x() < 20 )
	toggleOpen( i );
}

void PropertyList::toggleOpen( TQListViewItem *i )
{
    if ( !i )
	return;
    PropertyItem *pi = (PropertyItem*)i;
    if ( pi->hasSubItems() ) {
	pi->setOpen( !pi->isOpen() );
    } else {
	pi->toggle();
    }
}

bool PropertyList::eventFilter( TQObject *o, TQEvent *e )
{
    if ( !o || !e )
	return true;

    PropertyItem *i = (PropertyItem*)currentItem();
    if ( e->type() == TQEvent::KeyPress )
	theLastEvent = KeyEvent;
    else if ( e->type() == TQEvent::MouseButtonPress )
	theLastEvent = MouseEvent;

    if ( o != this &&e->type() == TQEvent::KeyPress ) {
	TQKeyEvent *ke = (TQKeyEvent*)e;
	if ( ( ke->key() == Key_Up || ke->key() == Key_Down ) &&
	     ( o != this || o != viewport() ) &&
	     !( ke->state() & ControlButton ) ) {
	    TQApplication::sendEvent( this, (TQKeyEvent*)e );
	    return true;
	} else if ( ( !::tqt_cast<TQLineEdit*>(o) ||
		      ( ::tqt_cast<TQLineEdit*>(o) && ( (TQLineEdit*)o )->isReadOnly() ) ) &&
		    i && i->hasSubItems() ) {
	    if ( !i->isOpen() &&
		 ( ke->key() == Key_Plus ||
		   ke->key() == Key_Right ))
		i->setOpen( true );
	    else if ( i->isOpen() &&
		      ( ke->key() == Key_Minus ||
			ke->key() == Key_Left ) )
		i->setOpen( false );
	} else if ( ( ke->key() == Key_Return || ke->key() == Key_Enter ) && ::tqt_cast<TQComboBox*>(o) ) {
	    TQKeyEvent ke2( TQEvent::KeyPress, Key_Space, 0, 0 );
	    TQApplication::sendEvent( o, &ke2 );
	    return true;
	}
    } else if ( e->type() == TQEvent::FocusOut && ::tqt_cast<TQLineEdit*>(o) && editor->formWindow() ) {
	TQTimer::singleShot( 100, editor->formWindow()->commandHistory(), TQ_SLOT( checkCompressedCommand() ) );
    } else if ( o == viewport() ) {
	TQMouseEvent *me;
	PropertyListItem* i;
	switch ( e->type() ) {
	case TQEvent::MouseButtonPress:
	    me = (TQMouseEvent*)e;
	    i = (PropertyListItem*) itemAt( me->pos() );
	    if ( i && ( ::tqt_cast<PropertyColorItem*>(i) || ::tqt_cast<PropertyPixmapItem*>(i) ) ) {
		pressItem = i;
		pressPos = me->pos();
		mousePressed = true;
	    }
	    break;
	case TQEvent::MouseMove:
	    me = (TQMouseEvent*)e;
	    if ( me && me->state() & LeftButton && mousePressed) {

		i = (PropertyListItem*) itemAt( me->pos() );
		if ( i  && i == pressItem ) {

		    if (( pressPos - me->pos() ).manhattanLength() > TQApplication::startDragDistance() ){
			if ( ::tqt_cast<PropertyColorItem*>(i) ) {
			    TQColor col = i->value().asColor();
			    TQColorDrag *drg = new TQColorDrag( col, this );
			    TQPixmap pix( 25, 25 );
			    pix.fill( col );
			    TQPainter p( &pix );
			    p.drawRect( 0, 0, pix.width(), pix.height() );
			    p.end();
			    drg->setPixmap( pix );
			    mousePressed = false;
			    drg->dragCopy();
			}
			else if ( ::tqt_cast<PropertyPixmapItem*>(i) ) {
			    TQPixmap pix = i->value().asPixmap();
			    if( !pix.isNull() ) {
				TQImage img = pix.convertToImage();
				TQImageDrag *drg = new TQImageDrag( img, this );
				drg->setPixmap( pix );
				mousePressed = false;
				drg->dragCopy();
			    }
			}
		    }
		}
	    }
	    break;
	default:
	    break;
	}
    } else if ( o == header() ) {
	if ( e->type() == TQEvent::ContextMenu ) {
	    ((TQContextMenuEvent *)e)->accept();
	    TQPopupMenu menu( 0 );
	    menu.setCheckable( true );
	    const int cat_id = 1;
	    const int alpha_id = 2;
	    menu.insertItem( tr( "Sort &Categorized" ), cat_id );
	    int alpha = menu.insertItem( tr( "Sort &Alphabetically" ), alpha_id );
	    if ( showSorted )
		menu.setItemChecked( alpha_id, true );
	    else
		menu.setItemChecked( cat_id, true );
	    int res = menu.exec( ( (TQContextMenuEvent*)e )->globalPos() );
	    if ( res != -1 ) {
		bool newShowSorted = ( res == alpha );
		if ( showSorted != newShowSorted ) {
		    showSorted = newShowSorted;
		    editor->clear();
		    editor->setup();
		}
	    }
	    return true;
	}
    }

    return TQListView::eventFilter( o, e );
}

/*!  This method re-initializes each item of the property list.
*/

void PropertyList::refetchData()
{
    TQListViewItemIterator it( this );
    for ( ; it.current(); ++it ) {
	PropertyItem *i = (PropertyItem*)it.current();
	if ( !i->propertyParent() )
	    setPropertyValue( i );
	if ( i->hasSubItems() )
	    i->initChildren();
	bool changed = MetaDataBase::isPropertyChanged( editor->widget(), i->name() );
	if ( ( i->name() == "layoutSpacing" || i->name() == "layoutMargin" ) ) {
	    if ( i->value().toString() != "-1" )
		i->setChanged( true, false );
	    else
		i->setChanged( false, false );
	}
	else if ( changed != i->isChanged() )
	    i->setChanged( changed, false );
    }
    updateEditorSize();
}

static void clearAlignList( TQStrList &l )
{
    if ( l.count() == 1 )
	return;
    if ( l.find( "AlignAuto" ) != -1 )
	l.remove( "AlignAuto" );
    if ( l.find( "WordBreak" ) != -1 )
	l.remove( "WordBreak" );
}

/*!  This method initializes the value of the item \a i to the value
  of the corresponding property.
*/

void PropertyList::setPropertyValue( PropertyItem *i )
{
    const TQMetaProperty *p =
	editor->widget()->metaObject()->
	property( editor->widget()->metaObject()->findProperty( i->name(), true), true );
    if ( !p ) {
	if ( i->name() == "hAlign" ) {
	    int align = editor->widget()->property( "alignment" ).toInt();
	    p = editor->widget()->metaObject()->
		property( editor->widget()->metaObject()->findProperty( "alignment", true ), true );
	    align &= ~AlignVertical_Mask;
	    TQStrList l = p->valueToKeys( align );
	    clearAlignList( l );
	    ( (PropertyListItem*)i )->setCurrentItem( l.last() );
	} else if ( i->name() == "vAlign" ) {
	    int align = editor->widget()->property( "alignment" ).toInt();
	    p = editor->widget()->metaObject()->
		property( editor->widget()->metaObject()->findProperty( "alignment", true ), true );
	    align &= ~AlignHorizontal_Mask;
	    ( (PropertyListItem*)i )->setCurrentItem( p->valueToKeys( align ).last() );
	} else if ( i->name() == "wordwrap" ) {
	    int align = editor->widget()->property( "alignment" ).toInt();
	    if ( align & WordBreak )
		i->setValue( TQVariant( true ) );
	    else
		i->setValue( TQVariant( false ) );
	} else if ( i->name() == "layoutSpacing" ) {
	    ( (PropertyLayoutItem*)i )->setValue( MetaDataBase::spacing( WidgetFactory::containerOfWidget( (TQWidget*)editor->widget() ) ) );
	} else if ( i->name() == "layoutMargin" ) {
	    ( (PropertyLayoutItem*)i )->setValue( MetaDataBase::margin( WidgetFactory::containerOfWidget( (TQWidget*)editor->widget() ) ) );
	} else if ( i->name() == "resizeMode" ) {
	    ( (PropertyListItem*)i )->setCurrentItem( MetaDataBase::resizeMode( WidgetFactory::containerOfWidget( (TQWidget*)editor->widget() ) ) );
	} else if ( i->name() == "toolTip" || i->name() == "whatsThis" || i->name() == "database" || i->name() == "frameworkCode" ) {
	    i->setValue( MetaDataBase::fakeProperty( editor->widget(), i->name() ) );
	} else if ( editor->widget()->inherits( "CustomWidget" ) ) {
	    MetaDataBase::CustomWidget *cw = ( (CustomWidget*)editor->widget() )->customWidget();
	    if ( !cw )
		return;
	    i->setValue( MetaDataBase::fakeProperty( editor->widget(), i->name() ) );
	}
	return;
    }
    if ( p->isSetType() )
	( (PropertyEnumItem*)i )->setCurrentValues( p->valueToKeys( editor->widget()->property( i->name() ).toInt() ) );
    else if ( p->isEnumType() )
	( (PropertyListItem*)i )->setCurrentItem( p->valueToKey( editor->widget()->property( i->name() ).toInt() ) );
    else if ( qstrcmp( p->name(), "buddy" ) == 0 )
	( (PropertyListItem*)i )->setCurrentItem( editor->widget()->property( i->name() ).toString() );
    else
	i->setValue( editor->widget()->property( i->name() ) );
}

void PropertyList::setCurrentProperty( const TQString &n )
{
    if ( ( currentItem() && currentItem()->text( 0 ) == n ) ||
	 ( currentItem() && ( (PropertyItem*)currentItem() )->propertyParent() &&
	 ( (PropertyItem*)currentItem() )->propertyParent()->text( 0 ) == n ) )
	return;

    TQListViewItemIterator it( this );
    for ( ; it.current(); ++it ) {
	if ( it.current()->text( 0 ) == n ) {
	    setCurrentItem( it.current() );
	    break;
	}
    }
}

PropertyEditor *PropertyList::propertyEditor() const
{
    return editor;
}

void PropertyList::resetProperty()
{
    if ( !currentItem() )
	return;
    PropertyItem *i = (PropertyItem*)currentItem();
    if ( !MetaDataBase::isPropertyChanged( editor->widget(), i->PropertyItem::name() ) )
	return;
    TQString pn( tr( "Reset '%1' of '%2'" ).arg( i->name() ).arg( editor->widget()->name() ) );
    SetPropertyCommand *cmd = new SetPropertyCommand( pn, editor->formWindow(),
						      editor->widget(), editor,
						      i->name(), i->value(),
						      WidgetFactory::defaultValue( editor->widget(), i->name() ),
						      WidgetFactory::defaultCurrentItem( editor->widget(), i->name() ),
						      i->currentItem(), true );
    cmd->execute();
    editor->formWindow()->commandHistory()->addCommand( cmd, false );
    if ( i->hasSubItems() )
	i->initChildren();
}

void PropertyList::viewportDragEnterEvent( TQDragEnterEvent *e )
{
    PropertyListItem *i = (PropertyListItem*) itemAt( e->pos() );
    if( !i ) {
	e->ignore();
	return;
    }

    if ( ::tqt_cast<PropertyColorItem*>(i) && TQColorDrag::canDecode( e ) )
	e->accept();
    else if ( ::tqt_cast<PropertyPixmapItem*>(i) && TQImageDrag::canDecode( e ) )
	e->accept();
    else
	e->ignore();
}

void PropertyList::viewportDragMoveEvent ( TQDragMoveEvent *e )
{
    PropertyListItem *i = (PropertyListItem*) itemAt( e->pos() );
    if( !i ) {
	e->ignore();
	return;
    }

    if ( ::tqt_cast<PropertyColorItem*>(i) && TQColorDrag::canDecode( e ) )
	e->accept();
    else if ( ::tqt_cast<PropertyPixmapItem*>(i) && TQImageDrag::canDecode( e ) )
	e->accept();
    else
	e->ignore();
}

void PropertyList::viewportDropEvent ( TQDropEvent *e )
{
    PropertyListItem *i = (PropertyListItem*) itemAt( e->pos() );
    if( !i ) {
	e->ignore();
	return;
    }

    if ( ::tqt_cast<PropertyColorItem*>(i) && TQColorDrag::canDecode( e ) ) {
	TQColor color;
	TQColorDrag::decode( e, color );
	i->setValue( TQVariant( color ) );
	valueChanged( i );
	e->accept();
    }
    else if ( ::tqt_cast<PropertyPixmapItem*>(i)  && TQImageDrag::canDecode( e ) ) {
	TQImage img;
	TQImageDrag::decode( e, img );
	TQPixmap pm;
	pm.convertFromImage( img );
	i->setValue( TQVariant( pm ) );
	valueChanged( i );
	e->accept();
    }
    else
	e->ignore();
}

TQString PropertyList::whatsThisAt( const TQPoint &p )
{
    return whatsThisText( itemAt( p ) );
}

void PropertyList::showCurrentWhatsThis()
{
    if ( !currentItem() )
	return;
    TQPoint p( 0, currentItem()->itemPos() );
    p = viewport()->mapToGlobal( contentsToViewport( p ) );
    TQWhatsThis::display( whatsThisText( currentItem() ), p, viewport() );
}

TQString PropertyList::whatsThisText( TQListViewItem *i )
{
    if ( !i || !editor->widget() )
	return TQString::null;
    readPropertyDocs();
    if ( ( (PropertyItem*)i )->propertyParent() )
	i = ( (PropertyItem*)i )->propertyParent();

    const TQMetaObject *mo = editor->widget()->metaObject();
    TQString prop = ( (PropertyItem*)i )->name();
    while ( mo ) {
	TQString s;
	s = TQString( mo->className() ) + "::" + prop;
	TQMap<TQString, TQString>::Iterator it;
	if ( ( it = propertyDocs.find( s ) ) != propertyDocs.end() ) {
	    return *it;
	}
	mo = mo->superClass();
    }

    return tr( "<p><b>TQWidget::%1</b></p><p>There is no documentation available for this property.</p>" ).arg( prop );
}

void PropertyList::readPropertyDocs()
{
    if ( !propertyDocs.isEmpty() )
	return;

    TQString docFile = MainWindow::self->documentationPath() + "/propertydocs";
    TQFile f( docFile );
    if ( !f.open( IO_ReadOnly ) )
	return;
    TQDomDocument doc;
    TQString errMsg;
    int errLine;
    if ( !doc.setContent( &f, &errMsg, &errLine ) )
	return;
    TQDomElement e = doc.firstChild().toElement().firstChild().toElement();

    for ( ; !e.isNull(); e = e.nextSibling().toElement() ) {
	TQDomElement n = e.firstChild().toElement();
	TQString name;
	TQString doc;
	for ( ; !n.isNull(); n = n.nextSibling().toElement() ) {
	    if ( n.tagName() == "name" )
		name = n.firstChild().toText().data();
	    else if ( n.tagName() == "doc" )
		doc = n.firstChild().toText().data();
	}
	doc.insert( 0, "<p><b>" + name + "</b></p>" );
	propertyDocs.insert( name, doc );
    }
}

PropertyList::LastEventType PropertyList::lastEvent()
{
    return theLastEvent;
}
// ------------------------------------------------------------

EventList::EventList( TQWidget *parent, FormWindow *fw, PropertyEditor *e )
    : HierarchyList( parent, fw, true ), editor( e )
{
    header()->hide();
    removeColumn( 1 );
    setRootIsDecorated( true );
    connect( this, TQ_SIGNAL( itemRenamed( TQListViewItem *, int, const TQString & ) ),
	     this, TQ_SLOT( renamed( TQListViewItem * ) ) );
}

TQString clean_arguments( const TQString &s )
{
    TQString slot = s;
    TQString arg = slot.mid( slot.find( '(' ) + 1 );
    arg = arg.left( arg.findRev( ')' ) );
    TQStringList args = TQStringList::split( ',', arg );
    slot = slot.left( slot.find( '(' ) + 1 );
    int num = 0;
    for ( TQStringList::Iterator it = args.begin(); it != args.end(); ++it, ++num ) {
	TQString a = *it;
	int i;
	if ( ( i =a.find( ':' ) ) == -1 )
	    slot += a.simplifyWhiteSpace();
	else
	    slot += a.mid( i + 1 ).simplifyWhiteSpace();
	if ( num < (int)args.count() - 1 )
	    slot += ",";
    }

    slot += ")";

    return slot;
}


void EventList::setup()
{
    clear();

    if ( !formWindow )
	return;
    LanguageInterface *iface = MetaDataBase::languageInterface( formWindow->project()->language() );
    TQStrList sigs;
    if ( iface )
	sigs = iface->signalNames( editor->widget() );
    TQStrListIterator it( sigs );
    while ( it.current() ) {
	HierarchyItem *eventItem = new HierarchyItem( HierarchyItem::Event, this, (HierarchyItem*)0,
						      it.current(), TQString::null, TQString::null );
	eventItem->setOpen( true );
	TQValueList<MetaDataBase::Connection> conns =
	    MetaDataBase::connections( formWindow, editor->widget(), formWindow->mainContainer() );
	HierarchyItem *item = 0;
	for ( TQValueList<MetaDataBase::Connection>::Iterator cit = conns.begin();
	      cit != conns.end(); ++cit ) {
	    TQString s = it.current();
	    if ( MetaDataBase::normalizeFunction( clean_arguments( TQString( (*cit).signal ) ) ) !=
		 MetaDataBase::normalizeFunction( clean_arguments( s ) ) )
		continue;
	    item = new HierarchyItem( HierarchyItem::EventFunction, eventItem, item,
						     (*cit).slot, TQString::null, TQString::null );
	    item->setPixmap( 0, TQPixmap::fromMimeSource( "designer_editslots.png" ) );
	}
	++it;
    }
}

extern TQListViewItem *newItem;

void EventList::contentsMouseDoubleClickEvent( TQMouseEvent *e )
{
    TQListViewItem *i = itemAt( contentsToViewport( e->pos() ) );
    if ( !i || i->parent() )
	return;
    TQString s;
    if ( !formWindow->project()->isCpp() ) {
	TQString s1 = i->text( 0 );
	int pt = s1.find( "(" );
	if ( pt != -1 )
	    s1 = s1.left( pt );
	s = TQString( editor->widget()->name() ) + "_" + s1;
    } else {
	s = TQString( editor->widget()->name() ) + "_" + i->text( 0 );
    }

    insertEntry( i, TQPixmap::fromMimeSource( "designer_editslots.png" ), s );
}

void EventList::setCurrent( TQWidget * )
{
}

void EventList::objectClicked( TQListViewItem *i )
{
    if ( !i || !i->parent() )
	return;
    formWindow->mainWindow()->editFunction( i->text( 0 ) );
}

void EventList::showRMBMenu( TQListViewItem *i, const TQPoint &pos )
{
    if ( !i )
	return;
    TQPopupMenu menu;
    const int NEW_ITEM = 1;
    const int DEL_ITEM = 2;
    menu.insertItem( TQPixmap::fromMimeSource( "designer_filenew.png" ), tr( "New Signal Handler" ), NEW_ITEM );
    menu.insertItem( TQPixmap::fromMimeSource( "designer_editcut.png" ), tr( "Delete Signal Handler" ), DEL_ITEM );
    int res = menu.exec( pos );
    if ( res == NEW_ITEM ) {
	TQString s;
	if ( !formWindow->project()->isCpp() ) {
	    TQString s1 = ( i->parent() ? i->parent() : i )->text( 0 );
	    int pt = s1.find( "(" );
	    if ( pt != -1 )
		s1 = s1.left( pt );
	    s = TQString( editor->widget()->name() ) + "_" + s1;
	} else {
	    s = TQString( editor->widget()->name() ) + "_" + ( i->parent() ? i->parent() : i )->text( 0 );
	}
	insertEntry( i->parent() ? i->parent() : i, TQPixmap::fromMimeSource( "designer_editslots.png" ), s );
    } else if ( res == DEL_ITEM && i->parent() ) {
	MetaDataBase::Connection conn;
	conn.sender = editor->widget();
	conn.receiver = formWindow->mainContainer();
	conn.signal = i->parent()->text( 0 );
	conn.slot = i->text( 0 );
	delete i;
	RemoveConnectionCommand *cmd = new RemoveConnectionCommand( tr( "Remove connection" ),
								    formWindow,
								    conn );
	formWindow->commandHistory()->addCommand( cmd );
	cmd->execute();
	editor->formWindow()->mainWindow()->objectHierarchy()->updateFormDefinitionView();
    }
}

void EventList::renamed( TQListViewItem *i )
{
    if ( newItem == i )
	newItem = 0;
    if ( !i->parent() )
	return;
    TQListViewItem *itm = i->parent()->firstChild();
    bool del = false;
    while ( itm ) {
	if ( itm != i && itm->text( 0 ) == i->text( 0 ) ) {
	    del = true;
	    break;
	}
	itm = itm->nextSibling();
    }
    i->setRenameEnabled( 0, false );
    if ( del ) {
	delete i;
    } else {
	MetaDataBase::Connection conn;
	conn.sender = editor->widget();
	conn.receiver = formWindow->mainContainer();
	conn.signal = i->parent()->text( 0 );
	conn.slot = i->text( 0 );
	AddConnectionCommand *cmd = new AddConnectionCommand( tr( "Add connection" ),
							      formWindow,
							      conn );
	formWindow->commandHistory()->addCommand( cmd );
	// #### we should look if the specified slot already
	// exists and if we can connect to this one
	TQString funcname = i->text( 0 ).latin1();
	if ( funcname.find( '(' ) == -1 ) { // try to create a signature
	    TQString sig = i->parent()->text( 0 );
	    sig = sig.mid( sig.find( '(' ) + 1 );
	    sig.remove( (int)sig.length() - 1, 1 );
	    LanguageInterface *iface = MetaDataBase::languageInterface( formWindow->project()->language() );
	    if ( iface )
		sig = iface->createArguments( sig.simplifyWhiteSpace() );
	    funcname += "(" + sig + ")";
	}
	MetaDataBase::addFunction( formWindow, funcname.latin1(), "virtual", "public",
				   "slot", formWindow->project()->language(), "void" );
	editor->formWindow()->mainWindow()->
	    editFunction( i->text( 0 ).left( i->text( 0 ).find( "(" ) ), true );
	cmd->execute();
	editor->formWindow()->mainWindow()->objectHierarchy()->updateFormDefinitionView();
	editor->formWindow()->formFile()->setModified( true );
    }
}

void EventList::save( TQListViewItem *p )
{
    TQStringList lst;
    TQListViewItem *i = p->firstChild();
    while ( i ) {
	lst << i->text( 0 );
	i = i->nextSibling();
    }
}

// --------------------------------------------------------------

/*!
  \class PropertyEditor propertyeditor.h
  \brief PropertyEdior toplevel window

  This is the toplevel window of the property editor which contains a
  listview for editing properties.
*/

PropertyEditor::PropertyEditor( TQWidget *parent )
    : TQTabWidget( parent, 0, WStyle_Customize | WStyle_NormalBorder | WStyle_Title |
		  WStyle_StaysOnTop | WStyle_Tool |WStyle_MinMax | WStyle_SysMenu )
{
    setCaption( tr( "Property Editor" ) );
    wid = 0;
    formwindow = 0;
    listview = new PropertyList( this );
    addTab( listview, tr( "P&roperties" ) );
    eList = new EventList( this, formWindow(), this );
    addTab( eList, tr( "Signa&l Handlers" ) );
}

TQObject *PropertyEditor::widget() const
{
    return wid;
}

void PropertyEditor::setWidget( TQObject *w, FormWindow *fw )
{
    if ( fw && fw->isFake() )
	w = fw->project()->objectForFakeForm( fw );
    eList->setFormWindow( fw );
    if ( w && w == wid ) {
	// if there is no properties displayed then we really should show them.
	// to do this check the number of properties in the list.
	bool ret = (listview->childCount() > 0);
	if ( wid->isWidgetType() && WidgetFactory::layoutType( (TQWidget*)wid ) != WidgetFactory::NoLayout ) {
	    TQListViewItemIterator it( listview );
	    ret = false;
	    while ( it.current() ) {
		if ( it.current()->text( 0 ) == "layoutSpacing" || it.current()->text( 0 ) == "layoutMargin" ) {
		    ret = true;
		    break;
		}
		++it;
	    }
	}
	if ( ret )
	    return;
    }

    if ( !w || !fw ) {
	setCaption( tr( "Property Editor" ) );
	clear();
	wid = 0;
	formwindow = 0;
	return;
    }

    wid = w;
    formwindow = fw;
    setCaption( tr( "Property Editor (%1)" ).arg( formwindow->name() ) );
    listview->viewport()->setUpdatesEnabled( false );
    listview->setUpdatesEnabled( false );
    clear();
    listview->viewport()->setUpdatesEnabled( true );
    listview->setUpdatesEnabled( true );
    setup();
}

void PropertyEditor::clear()
{
    listview->setContentsPos( 0, 0 );
    listview->clear();
    eList->setContentsPos( 0, 0 );
    eList->clear();
}

void PropertyEditor::setup()
{
    if ( !formwindow || !wid )
	return;
    listview->viewport()->setUpdatesEnabled( false );
    listview->setupProperties();
    listview->viewport()->setUpdatesEnabled( true );
    listview->updateEditorSize();

    // the forced processing of events in setupProperties() can set wid to 0, so we have to check
    if ( wid && !wid->isA( "PropertyObject" ) ) {
	eList->viewport()->setUpdatesEnabled( false );
	eList->setup();
	eList->viewport()->setUpdatesEnabled( true );
    }
}

void PropertyEditor::refetchData()
{
    listview->refetchData();
}

void PropertyEditor::emitWidgetChanged()
{
    if ( formwindow && wid )
	formwindow->widgetChanged( wid );
}

void PropertyEditor::closed( FormWindow *w )
{
    if ( w == formwindow ) {
	formwindow = 0;
	wid = 0;
	clear();
    }
}

void PropertyEditor::closeEvent( TQCloseEvent *e )
{
    emit hidden();
    e->accept();
}

PropertyList *PropertyEditor::propertyList() const
{
    return listview;
}

FormWindow *PropertyEditor::formWindow() const
{
    return formwindow;
}

TQString PropertyEditor::currentProperty() const
{
    if ( !wid )
	return TQString::null;
    if ( ( (PropertyItem*)listview->currentItem() )->propertyParent() )
	return ( (PropertyItem*)listview->currentItem() )->propertyParent()->name();
    return ( (PropertyItem*)listview->currentItem() )->name();
}

TQString PropertyEditor::classOfCurrentProperty() const
{
    if ( !wid )
	return TQString::null;
    TQObject *o = wid;
    TQString curr = currentProperty();
    TQMetaObject *mo = o->metaObject();
    while ( mo ) {
	TQStrList props = mo->propertyNames( false );
	if ( props.find( curr.latin1() ) != -1 )
	    return mo->className();
	mo = mo->superClass();
    }
    return TQString::null;
}

TQMetaObject* PropertyEditor::metaObjectOfCurrentProperty() const
{
    if ( !wid )
	return 0;
    return wid->metaObject();
}

void PropertyEditor::resetFocus()
{
    if ( listview->currentItem() )
	( (PropertyItem*)listview->currentItem() )->showEditor();
}

EventList *PropertyEditor::eventList() const
{
    return eList;
}

void PropertyEditor::setPropertyEditorEnabled( bool b )
{
    if ( !b )
	removePage( listview );
    else
	insertTab( listview, tr( "Property Editor" ), 0 );
    updateWindow();
}

void PropertyEditor::setSignalHandlersEnabled( bool b )
{
    if ( !b )
	removePage( eList );
    else
	insertTab( eList, tr( "Signal Handlers" ), 0 );
    updateWindow();
}

void PropertyEditor::updateWindow()
{
    if ( isHidden() && count() ) {
	parentWidget()->show();
	MainWindow::self->setAppropriate( (TQDockWindow*)parentWidget(), true );
    } else if ( isShown() && !count() ) {
	parentWidget()->hide();
	MainWindow::self->setAppropriate( (TQDockWindow*)parentWidget(), false );
    }
}
