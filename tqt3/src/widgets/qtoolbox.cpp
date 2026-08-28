/****************************************************************************
**
** Implementation of TQToolBox widget class
**
** Created : 961105
**
** Copyright (C) 1992-2008 Trolltech ASA.  All rights reserved.
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

#include "ntqtoolbox.h"

#ifndef TQT_NO_TOOLBOX

#include <ntqbutton.h>
#include <ntqlayout.h>
#include <ntqscrollview.h>
#include <ntqpainter.h>
#include <ntqstyle.h>
#include <ntqobjectlist.h>
#include <ntqapplication.h>
#include <ntqwidgetlist.h>
#include <ntqlayout.h>
#include <ntqvaluelist.h>
#include <ntqtooltip.h>
#include <ntqeventloop.h>
#include <ntqdatetime.h>

class TQToolBoxButton : public TQButton
{
public:
    TQToolBoxButton( TQWidget *parent, const char *name )
	: TQButton( parent, name ), selected( false )
    {
	setBackgroundMode(PaletteBackground);
	setSizePolicy(TQSizePolicy::Preferred, TQSizePolicy::Minimum);
	setFocusPolicy(NoFocus);
    }

    inline void setSelected( bool b ) { selected = b; update(); }
    inline void setTextLabel( const TQString &text ) { label = text; update(); }
    inline TQString textLabel() const { return label; }
    inline void setIconSet( const TQIconSet &is ) { icon = is; update(); }
    inline TQIconSet iconSet() const { return icon; }

    TQSize sizeHint() const;
    TQSize minimumSizeHint() const;

protected:
    void drawButton( TQPainter * );

private:
    bool selected;
    TQString label;
    TQIconSet icon;
};

class TQToolBoxPrivate
{
public:
    struct Page
    {
	TQToolBoxButton *button;
	TQScrollView *sv;
	TQWidget *widget;
	TQString toolTip;

	inline void setTextLabel( const TQString &text ) { button->setTextLabel(text); }
	inline void setIconSet( const TQIconSet &is ) { button->setIconSet(is); }
	inline void setToolTip( const TQString &tip )
	{
	    toolTip = tip;
	    TQToolTip::remove( button );
	    if ( !tip.isNull() )
		TQToolTip::add( button, tip );
	}

	inline bool operator==(const Page& other) const
	{
	    return widget == other.widget;
	}
    };
    typedef TQValueList<Page> PageList;

    inline TQToolBoxPrivate()
	: currentPage( 0 )
    {
    }

    Page *page( TQWidget *widget );
    Page *page( int index );

    void updateTabs();

    PageList pageList;
    TQVBoxLayout *layout;
    Page *currentPage;
};

TQToolBoxPrivate::Page *TQToolBoxPrivate::page( TQWidget *widget )
{
    if ( !widget )
	return 0;

    for ( PageList::ConstIterator i = pageList.constBegin(); i != pageList.constEnd(); ++i )
	if ( (*i).widget == widget )
	    return (Page*) &(*i);
    return 0;
}

TQToolBoxPrivate::Page *TQToolBoxPrivate::page( int index )
{
    if (index >= 0 && index < (int)pageList.size() )
	return &*pageList.at(index);
    return 0;
}

void TQToolBoxPrivate::updateTabs()
{
    TQToolBoxButton *lastButton = currentPage ? currentPage->button : 0;
    bool after = false;
    for ( PageList::ConstIterator i = pageList.constBegin(); i != pageList.constEnd(); ++i ) {
	if (after) {
	    (*i).button->setEraseColor((*i).widget->eraseColor());
	    (*i).button->update();
	} else if ( (*i).button->backgroundMode() != TQt::PaletteBackground ) {
	    (*i).button->setBackgroundMode( TQt::PaletteBackground );
	    (*i).button->update();
	}
	after = (*i).button == lastButton;
    }
}

TQSize TQToolBoxButton::sizeHint() const
{
    TQSize iconSize(8, 8);
    if ( !icon.isNull() )
	iconSize += icon.pixmap( TQIconSet::Small, TQIconSet::Normal ).size() + TQSize( 2, 0 );
    TQSize textSize = fontMetrics().size( TQt::ShowPrefix, label ) + TQSize(0, 8);

    TQSize total(iconSize.width() + textSize.width(), TQMAX(iconSize.height(), textSize.height()));
    return total.expandedTo(TQApplication::globalStrut());
}

TQSize TQToolBoxButton::minimumSizeHint() const
{
    if ( icon.isNull() )
	return TQSize();
    return TQSize(8, 8) + icon.pixmap( TQIconSet::Small, TQIconSet::Normal ).size();
}

void TQToolBoxButton::drawButton( TQPainter *p )
{
    TQStyle::SFlags flags = TQStyle::Style_Default;
    const TQColorGroup &cg = colorGroup();

    if ( isEnabled() )
	flags |= TQStyle::Style_Enabled;
    if ( selected )
	flags |= TQStyle::Style_Selected;
    if ( hasFocus() )
	flags |= TQStyle::Style_HasFocus;
    if (isDown())
	flags |= TQStyle::Style_Down;
    style().drawControl( TQStyle::CE_ToolBoxTab, p, parentWidget(), rect(), cg, flags );

    TQPixmap pm = icon.pixmap( TQIconSet::Small, isEnabled() ? TQIconSet::Normal : TQIconSet::Disabled );

    TQRect cr = style().subRect( TQStyle::SR_ToolBoxTabContents, this );
    TQRect tr, ir;
    int ih = 0;
    if ( pm.isNull() ) {
	tr = cr;
	tr.addCoords( 4, 0, -8, 0 );
    } else {
	int iw = pm.width() + 4;
	ih = pm.height();
	ir = TQRect( cr.left() + 4, cr.top(), iw + 2, ih );
	tr = TQRect( ir.right(), cr.top(), cr.width() - ir.right() - 4, cr.height() );
    }

    if ( selected && style().styleHint( TQStyle::SH_ToolBox_SelectedPageTitleBold ) ) {
	TQFont f( p->font() );
	f.setBold( true );
	p->setFont( f );
    }

    TQString txt;
    if ( p->fontMetrics().width(label) < tr.width() ) {
	txt = label;
    } else {
	txt = label.left( 1 );
	int ew = p->fontMetrics().width( "..." );
	int i = 1;
	while ( p->fontMetrics().width( txt ) + ew +
		p->fontMetrics().width( label[i] )  < tr.width() )
	    txt += label[i++];
	txt += "...";
    }

    if ( ih )
	p->drawPixmap( ir.left(), (height() - ih) / 2, pm );

    TQToolBox *tb = (TQToolBox*)parentWidget();

    const TQColor* fill = 0;
    if ( selected &&
	 style().styleHint( TQStyle::SH_ToolBox_SelectedPageTitleBold ) &&
	 tb->backgroundMode() != NoBackground )
	fill = &cg.color( TQPalette::foregroundRoleFromMode( tb->backgroundMode() ) );

    int alignment = AlignLeft | AlignVCenter | ShowPrefix;
    if ((!style().styleHint(TQStyle::SH_UnderlineAccelerator, this)) || ((style().styleHint(TQStyle::SH_HideUnderlineAcceleratorWhenAltUp, this)) && (!style().acceleratorsShown()))) {
	alignment |= NoAccel;
    }
    style().drawItem( p, tr, alignment, cg,
		      isEnabled(), 0, txt, -1, fill );

    if ( !txt.isEmpty() && hasFocus() )
	style().drawPrimitive( TQStyle::PE_FocusRect, p, tr, cg );
}

/*!
    \class TQToolBox

    \brief The TQToolBox class provides a column of tabbed widget
    items.

    \mainclass
    \ingroup advanced

    A toolbox is a widget that displays a column of tabs one above the
    other, with the current item displayed below the current tab.
    Every tab has an index position within the column of tabs. A tab's
    item is a TQWidget.

    Each item has an itemLabel(), an optional icon, itemIconSet(), an
    optional itemToolTip(), and a \link item() widget\endlink. The
    item's attributes can be changed with setItemLabel(),
    setItemIconSet() and setItemToolTip().

    Items are added using addItem(), or inserted at particular
    positions using insertItem(). The total number of items is given
    by count(). Items can be deleted with delete, or removed from the
    toolbox with removeItem(). Combining removeItem() and insertItem()
    allows to move items to different positions.

    The current item widget is returned by currentItem() and set with
    setCurrentItem(). If you prefer you can work in terms of indexes
    using currentIndex(), setCurrentIndex(), indexOf() and item().

    The currentChanged() signal is emitted when the current item is
    changed.

    \sa TQTabWidget
*/

/*!
    \fn void TQToolBox::currentChanged( int index )

    This signal is emitted when the current item changed. The new
    current item's index is passed in \a index, or -1 if there is no
    current item.
*/

/*!
    Constructs a toolbox called \a name with parent \a parent and flags \a f.
*/

TQToolBox::TQToolBox( TQWidget *parent, const char *name, WFlags f )
    :  TQFrame( parent, name, f )
{
    d = new TQToolBoxPrivate;
    d->layout = new TQVBoxLayout( this );
    TQWidget::setBackgroundMode( PaletteButton );
}

/*! \reimp */

TQToolBox::~TQToolBox()
{
    delete d;
}

/*!
    \fn int TQToolBox::addItem( TQWidget *w, const TQString &label )
    \overload

    Adds the widget \a w in a new tab at bottom of the toolbox. The
    new tab's label is set to \a label. Returns the new tab's index.
*/

/*!
    \fn int TQToolBox::addItem( TQWidget *item, const TQIconSet &iconSet,const TQString &label )
    Adds the widget \a item in a new tab at bottom of the toolbox. The
    new tab's label is set to \a label, and the \a iconSet is
    displayed to the left of the \a label.  Returns the new tab's index.
*/

/*!
    \fn int TQToolBox::insertItem( int index, TQWidget *item, const TQString &label )
    \overload

    Inserts the widget \a item at position \a index, or at the bottom
    of the toolbox if \a index is out of range. The new item's label is
    set to \a label. Returns the new item's index.
*/

/*!
    Inserts the widget \a item at position \a index, or at the bottom
    of the toolbox if \a index is out of range. The new item's label
    is set to \a label, and the \a iconSet is displayed to the left of
    the \a label. Returns the new item's index.
*/

int TQToolBox::insertItem( int index, TQWidget *item, const TQIconSet &iconSet,
			   const TQString &label )
{
    if ( !item )
	return -1;

    connect(item, TQ_SIGNAL(destroyed(TQObject*)), this, TQ_SLOT(itemDestroyed(TQObject*)));

    TQToolBoxPrivate::Page c;
    c.widget = item;
    c.button = new TQToolBoxButton( this, label.latin1() );
    connect( c.button, TQ_SIGNAL( clicked() ), this, TQ_SLOT( buttonClicked() ) );

    c.sv = new TQScrollView( this );
    c.sv->hide();
    c.sv->setResizePolicy( TQScrollView::AutoOneFit );
    c.sv->addChild( item );
    c.sv->setFrameStyle( TQFrame::NoFrame );

    c.setTextLabel( label );
    c.setIconSet( iconSet );

    if ( index < 0 || index >= (int)d->pageList.count() ) {
	index = (int)d->pageList.count();
	d->pageList.append( c );
	d->layout->addWidget( c.button );
	d->layout->addWidget( c.sv );
	if ( index == 0 )
	    setCurrentIndex( index );
    } else {
	d->pageList.insert( d->pageList.at(index), c );
	relayout();
	if (d->currentPage) {
	    TQWidget *current = d->currentPage->widget;
	    int oldindex = indexOf(current);
	    if ( index <= oldindex ) {
		d->currentPage = 0; // trigger change
		setCurrentIndex(oldindex);
	    }
	}
    }

    c.button->show();

    d->updateTabs();
    itemInserted(index);
    return index;
}

void TQToolBox::buttonClicked()
{
    TQToolBoxButton *tb = ::tqt_cast<TQToolBoxButton*>(sender());
    TQWidget* item = 0;
    for ( TQToolBoxPrivate::PageList::ConstIterator i = d->pageList.constBegin(); i != d->pageList.constEnd(); ++i )
	if ( (*i).button == tb ) {
	    item = (*i).widget;
	    break;
	}

    setCurrentItem( item );
}

/*!
    \property TQToolBox::count
    \brief The number of items contained in the toolbox.
*/

int TQToolBox::count() const
{
    return (int)d->pageList.count();
}

void TQToolBox::setCurrentIndex( int index )
{
    setCurrentItem( item( index ) );
}

/*!
    Sets the current item to be \a item.
*/

void TQToolBox::setCurrentItem( TQWidget *item )
{
    TQToolBoxPrivate::Page *c = d->page( item );
    if ( !c || d->currentPage == c )
	return;

    c->button->setSelected( true );
    if ( d->currentPage ) {
	d->currentPage->sv->hide();
	d->currentPage->button->setSelected(false);
    }
    d->currentPage = c;
    d->currentPage->sv->show();
    d->updateTabs();
    emit currentChanged( indexOf(item) );
}

void TQToolBox::relayout()
{
    delete d->layout;
    d->layout = new TQVBoxLayout( this );
    for ( TQToolBoxPrivate::PageList::ConstIterator i = d->pageList.constBegin(); i != d->pageList.constEnd(); ++i ) {
	d->layout->addWidget( (*i).button );
	d->layout->addWidget( (*i).sv );
    }
}

void TQToolBox::itemDestroyed(TQObject *object)
{
    // no verification - vtbl corrupted already
    TQWidget *page = (TQWidget*)object;

    TQToolBoxPrivate::Page *c = d->page(page);
    if ( !page || !c )
	return;

    d->layout->remove( c->sv );
    d->layout->remove( c->button );
    c->sv->deleteLater(); // page might still be a child of sv
    delete c->button;

    bool removeCurrent = c == d->currentPage;
    d->pageList.remove( *c );

    if ( !d->pageList.count() ) {
	d->currentPage = 0;
	emit currentChanged(-1);
    } else if ( removeCurrent ) {
	d->currentPage = 0;
	setCurrentIndex(0);
    }
}

/*!
    Removes the widget \a item from the toolbox. Note that the widget
    is \e not deleted. Returns the removed widget's index, or -1 if
    the widget was not in this tool box.
*/

int TQToolBox::removeItem( TQWidget *item )
{
    int index = indexOf(item);
    if (index >= 0) {
	disconnect(item, TQ_SIGNAL(destroyed(TQObject*)), this, TQ_SLOT(itemDestroyed(TQObject*)));
	item->reparent( this, TQPoint(0,0) );
	// destroy internal data
	itemDestroyed(item);
    }
    itemRemoved(index);
    return index;
}


/*!
    Returns the toolbox's current item, or 0 if the toolbox is empty.
*/

TQWidget *TQToolBox::currentItem() const
{
    return d->currentPage ? d->currentPage->widget : 0;
}

/*!
    \property TQToolBox::currentIndex
    \brief the index of the current item, or -1 if the toolbox is empty.
    \sa currentItem(), indexOf(), item()
*/


int TQToolBox::currentIndex() const
{
    return d->currentPage ? indexOf( d->currentPage->widget ) : -1;
}

/*!
    Returns the item at position \a index, or 0 if there is no such
    item.
*/

TQWidget *TQToolBox::item( int index ) const
{
    if ( index < 0 || index >= (int) d->pageList.size() )
	return 0;
    return (*d->pageList.at( index )).widget;
}

/*!
    Returns the index of item \a item, or -1 if the item does not
    exist.
*/

int TQToolBox::indexOf( TQWidget *item ) const
{
    TQToolBoxPrivate::Page *c = d->page(item);
    return c ? d->pageList.findIndex( *c ) : -1;
}

/*!
    If \a enabled is true then the item at position \a index is enabled; otherwise item
    \a index is disabled.
*/

void TQToolBox::setItemEnabled( int index, bool enabled )
{
    TQToolBoxPrivate::Page *c = d->page( index );
    if ( !c )
	return;

    c->button->setEnabled( enabled );
    if ( !enabled && c == d->currentPage ) {
	int curIndexUp = index;
	int curIndexDown = curIndexUp;
	const int count = (int)d->pageList.count();
	while ( curIndexUp > 0 || curIndexDown < count-1 ) {
	    if ( curIndexDown < count-1 ) {
		if (d->page(++curIndexDown)->button->isEnabled()) {
		    index = curIndexDown;
		    break;
		}
	    }
	    if ( curIndexUp > 0 ) {
		if (d->page(--curIndexUp)->button->isEnabled()) {
		    index = curIndexUp;
		    break;
		}
	    }
	}
	setCurrentIndex(index);
    }
}


/*!
    Sets the label of the item at position \a index to \a label.
*/

void TQToolBox::setItemLabel( int index, const TQString &label )
{
    TQToolBoxPrivate::Page *c = d->page( index );
    if ( c )
	c->setTextLabel( label );
}

/*!
    Sets the icon of the item at position \a index to \a iconSet.
*/

void TQToolBox::setItemIconSet( int index, const TQIconSet &iconSet )
{
    TQToolBoxPrivate::Page *c = d->page( index );
    if ( c )
	c->setIconSet( iconSet );
}

/*!
    Sets the tooltip of the item at position \a index to \a toolTip.
*/

void TQToolBox::setItemToolTip( int index, const TQString &toolTip )
{
    TQToolBoxPrivate::Page *c = d->page( index );
    if ( c )
	c->setToolTip( toolTip );
}

/*!
    Returns true if the item at position \a index is enabled; otherwise returns false.
*/

bool TQToolBox::isItemEnabled( int index ) const
{
    TQToolBoxPrivate::Page *c = d->page( index );
    return c && c->button->isEnabled();
}

/*!
    Returns the label of the item at position \a index, or a null string if
    \a index is out of range.
*/

TQString TQToolBox::itemLabel( int index ) const
{
    TQToolBoxPrivate::Page *c = d->page( index );
    return ( c ? c->button->textLabel() : TQString::null );
}

/*!
    Returns the icon of the item at position \a index, or a null
    icon if \a index is out of range.
*/

TQIconSet TQToolBox::itemIconSet( int index ) const
{
    TQToolBoxPrivate::Page *c = d->page( index );
    return ( c ? c->button->iconSet() : TQIconSet() );
}

/*!
    Returns the tooltip of the item at position \a index, or a null
    string if \a index is out of range.
*/

TQString TQToolBox::itemToolTip( int index ) const
{
    TQToolBoxPrivate::Page *c = d->page( index );
    return ( c ? c->toolTip : TQString::null );
}

/*! \reimp */
void TQToolBox::showEvent( TQShowEvent *e )
{
    TQWidget::showEvent( e );
}

/*! \reimp */
void TQToolBox::frameChanged()
{
    d->layout->setMargin( frameWidth() );
    TQFrame::frameChanged();
}

/*! \reimp */
void TQToolBox::styleChange(TQStyle &style)
{
    d->updateTabs();
    TQFrame::styleChange(style);
}

/*!
  This virtual handler is called after a new item was added or
  inserted at position \a index.
 */
void TQToolBox::itemInserted( int index )
{
    Q_UNUSED(index)
}

/*!
  This virtual handler is called after an item was removed from
  position \a index.
 */
void TQToolBox::itemRemoved( int index )
{
    Q_UNUSED(index)
}

#endif //TQT_NO_TOOLBOX
