/**********************************************************************
**
** Implementation of TQComboBox widget class
**
** Created : 940426
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

#include "ntqcombobox.h"
#ifndef TQT_NO_COMBOBOX
#include "ntqpopupmenu.h"
#include "ntqlistbox.h"
#include "ntqpainter.h"
#include "ntqdrawutil.h"
#include "ntqstrlist.h"
#include "ntqpixmap.h"
#include "ntqtimer.h"
#include "ntqapplication.h"
#include "ntqlineedit.h"
#include "ntqbitmap.h"
#include "qeffects_p.h"
#include "ntqstringlist.h"
#include "ntqcombobox.h"
#include "ntqstyle.h"
#include <limits.h>
#if defined(QT_ACCESSIBILITY_SUPPORT)
#include "ntqaccessible.h"
#endif

/*!
    \class TQComboBox ntqcombobox.h
    \brief The TQComboBox widget is a combined button and popup list.

    \ingroup basic
    \mainclass

    A combobox is a selection widget which displays the current item
    and can pop up a list of items. A combobox may be editable in
    which case the user can enter arbitrary strings.

    Comboboxes provide a means of showing the user's current choice
    out of a list of options in a way that takes up the minimum amount
    of screen space.

    TQComboBox supports three different display styles: Aqua/Motif 1.x,
    Motif 2.0 and Windows. In Motif 1.x, a combobox was called
    XmOptionMenu. In Motif 2.0, OSF introduced an improved combobox
    and named that XmComboBox. TQComboBox provides both.

    TQComboBox provides two different constructors. The simplest
    constructor creates an "old-style" combobox in Motif (or Aqua)
    style:
    \code
	TQComboBox *c = new TQComboBox( this, "read-only combobox" );
    \endcode

    The other constructor creates a new-style combobox in Motif style,
    and can create both read-only and editable comboboxes:
    \code
	TQComboBox *c1 = new TQComboBox( false, this, "read-only combobox" );
	TQComboBox *c2 = new TQComboBox( true, this, "editable combobox" );
    \endcode

    New-style comboboxes use a list box in both Motif and Windows
    styles, and both the content size and the on-screen size of the
    list box can be limited with sizeLimit() and setMaxCount()
    respectively. Old-style comboboxes use a popup in Aqua and Motif
    style, and that popup will happily grow larger than the desktop if
    you put enough data into it.

    The two constructors create identical-looking comboboxes in
    Windows style.

    Comboboxes can contain pixmaps as well as strings; the
    insertItem() and changeItem() functions are suitably overloaded.
    For editable comboboxes, the function clearEdit() is provided,
    to clear the displayed string without changing the combobox's
    contents.

    A combobox emits two signals, activated() and highlighted(), when
    a new item has been activated (selected) or highlighted (made
    current). Both signals exist in two versions, one with a \c
    TQString argument and one with an \c int argument. If the user
    highlights or activates a pixmap, only the \c int signals are
    emitted. Whenever the text of an editable combobox is changed the
    textChanged() signal is emitted.

    When the user enters a new string in an editable combobox, the
    widget may or may not insert it, and it can insert it in several
    locations. The default policy is is \c AtBottom but you can change
    this using setInsertionPolicy().

    It is possible to constrain the input to an editable combobox
    using TQValidator; see setValidator(). By default, any input is
    accepted.

    If the combobox is not editable then it has a default
    focusPolicy() of \c TabFocus, i.e. it will not grab focus if
    clicked. This differs from both Windows and Motif. If the combobox
    is editable then it has a default focusPolicy() of \c StrongFocus,
    i.e. it will grab focus if clicked.

    A combobox can be populated using the insert functions,
    insertStringList() and insertItem() for example. Items can be
    changed with changeItem(). An item can be removed with
    removeItem() and all items can be removed with clear(). The text
    of the current item is returned by currentText(), and the text of
    a numbered item is returned with text(). The current item can be
    set with setCurrentItem() or setCurrentText(). The number of items
    in the combobox is returned by count(); the maximum number of
    items can be set with setMaxCount(). You can allow editing using
    setEditable(). For editable comboboxes you can set auto-completion
    using setAutoCompletion() and whether or not the user can add
    duplicates is set with setDuplicatesEnabled().

    <img src="qcombo1-m.png">(Motif 1, read-only)<br clear=all>
    <img src="qcombo2-m.png">(Motif 2, editable)<br clear=all>
    <img src="qcombo3-m.png">(Motif 2, read-only)<br clear=all>
    <img src="qcombo1-w.png">(Windows style)

    Depending on the style, TQComboBox will use a TQListBox or a
    TQPopupMenu to display the list of items. See setListBox() for
    more information.

    \sa TQLineEdit TQListBox TQSpinBox TQRadioButton TQButtonGroup
    \link guibooks.html#fowler GUI Design Handbook: Combo Box,\endlink
    \link guibooks.html#fowler GUI Design Handbook: Drop-Down List Box.\endlink
*/


/*!
    \enum TQComboBox::Policy

    This enum specifies what the TQComboBox should do when a new string
    is entered by the user.

    \value NoInsertion the string will not be inserted into the
    combobox.

    \value AtTop insert the string as the first item in the combobox.

    \value AtCurrent replace the previously selected item with the
    string the user has entered.

    \value AtBottom insert the string as the last item in the
    combobox.

    \value AfterCurrent insert the string after the previously
    selected item.

    \value BeforeCurrent insert the string before the previously
    selected item.

    activated() is always emitted when the string is entered.

    If inserting the new string would cause the combobox to breach its
    content size limit, the item at the other end of the list is
    deleted. The definition of "other end" is
    implementation-dependent.
*/


/*!
    \fn void TQComboBox::activated( int index )

    This signal is emitted when a new item has been activated
    (selected). The \a index is the position of the item in the
    combobox.

    This signal is not emitted if the item is changed
    programmatically, e.g. using setCurrentItem().
*/

/*!
    \overload void TQComboBox::activated( const TQString &string )

    This signal is emitted when a new item has been activated
    (selected). \a string is the selected string.

    You can also use the activated(int) signal, but be aware that its
    argument is meaningful only for selected strings, not for user
    entered strings.
*/

/*!
    \fn void TQComboBox::highlighted( int index )

    This signal is emitted when a new item has been set to be the
    current item. The \a index is the position of the item in the
    combobox.

    This signal is not emitted if the item is changed
    programmatically, e.g. using setCurrentItem().
*/

/*!
    \overload void TQComboBox::highlighted( const TQString &string )

    This signal is emitted when a new item has been set to be the
    current item. \a string is the item's text.

    You can also use the highlighted(int) signal.
*/

/*!
    \fn void TQComboBox::textChanged( const TQString &string )

    This signal is used for editable comboboxes. It is emitted
    whenever the contents of the text entry field changes. \a string
    contains the new text.
*/

/*!
    \property TQComboBox::autoCompletion
    \brief whether auto-completion is enabled

    This property can only be set for editable comboboxes, for
    non-editable comboboxes it has no effect. It is false by default.
*/

/*!
    \property TQComboBox::autoMask
    \brief whether the combobox is automatically masked

    \sa TQWidget::setAutoMask()
*/

/*! \property TQComboBox::autoResize
    \brief whether auto resize is enabled
    \obsolete

  If this property is set to true then the combobox will resize itself
  whenever its contents change. The default is false.
*/

/*!
    \property TQComboBox::count
    \brief the number of items in the combobox
*/

/*!
    \property TQComboBox::currentItem
    \brief the index of the current item in the combobox

    Note that the activated() and highlighted() signals are only
    emitted when the user changes the current item, not when it is
    changed programmatically.
*/

/*!
    \property TQComboBox::currentText
    \brief the text of the combobox's current item
*/

/*!
    \property TQComboBox::duplicatesEnabled
    \brief whether duplicates are allowed

    If the combobox is editable and the user enters some text in the
    combobox's lineedit and presses Enter (and the insertionPolicy()
    is not \c NoInsertion), then what happens is this:
    \list
    \i If the text is not already in the list, the text is inserted.
    \i If the text is in the list and this property is true (the
    default), the text is inserted.
    \i If the text is in the list and this property is false, the text
    is \e not inserted; instead the item which has matching text becomes
    the current item.
    \endlist

    This property only affects user-interaction. You can use
    insertItem() to insert duplicates if you wish regardless of this
    setting.
*/

/*!
    \property TQComboBox::editable
    \brief whether the combobox is editable

    This property's default is false. Note that the combobox will be
    cleared if this property is set to true for a 1.x Motif style
    combobox. To avoid this, use setEditable() before inserting any
    items. Also note that the 1.x version of Motif didn't have any
    editable comboboxes, so the combobox will change it's appearance
    to a 2.0 style Motif combobox is it is set to be editable.
*/

/*!
    \property TQComboBox::insertionPolicy
    \brief the position of the items inserted by the user

    The default insertion policy is \c AtBottom. See \l Policy.
*/

/*!
    \property TQComboBox::maxCount
    \brief the maximum number of items allowed in the combobox
*/

/*!
    \property TQComboBox::sizeLimit
    \brief the maximum on-screen size of the combobox.

    This property is ignored for both Motif 1.x style and non-editable
    comboboxes in Mac style. The default limit is ten
    lines. If the number of items in the combobox is or grows larger
    than lines, a scrollbar is added.
*/

class TQComboBoxPopup : public TQPopupMenu
{
public:
    TQComboBoxPopup( TQWidget *parent=0, const char *name=0 )
	: TQPopupMenu( parent, name )
    {
    }

    int itemHeight( int index )
    {
	return TQPopupMenu::itemHeight( index );
    }
};

static inline TQString escapedComboString(const TQString &str)
{
    TQString stringToReturn = str;
    return stringToReturn.replace('&', "&&");
}

class TQComboBoxPopupItem : public TQCustomMenuItem
{
    TQListBoxItem *li;
    TQSize sc; // Size cache optimization
public:
    TQComboBoxPopupItem(TQListBoxItem *i) : TQCustomMenuItem(), li(i), sc(0, 0) {  }
    virtual bool fullSpan() const { return true; }
    virtual void paint( TQPainter*, const TQColorGroup&, bool, bool, int, int, int, int);
    virtual TQSize sizeHint() { if (sc.isNull()) sc = TQSize(li->width(li->listBox()), TQMAX(25, li->height(li->listBox()))); return sc; }
};
void TQComboBoxPopupItem::paint( TQPainter* p, const TQColorGroup&, bool,
				bool, int x, int y, int, int)
{
    p->save();
    p->translate(x, y + ((sizeHint().height() / 2) - (li->height(li->listBox()) / 2)));
    li->paint(p);
    p->restore();
}


class TQComboBoxData
{
public:
    TQComboBoxData( TQComboBox *cb ): ed( 0 ), usingLBox( false ), pop( 0 ), lBox( 0 ), combo( cb )
    {
	duplicatesEnabled = true;
	cb->setSizePolicy( TQSizePolicy( TQSizePolicy::Minimum, TQSizePolicy::Fixed ) );
    }

    inline bool usingListBox()  { return usingLBox; }
    inline TQListBox * listBox() { return lBox; }
    inline TQComboBoxPopup * popup() { return pop; }
    void updateLinedGeometry();
    void setListBox( TQListBox *l );
    void setPopupMenu( TQComboBoxPopup * pm, bool isPopup=true );

    int		current;
    int		maxCount;
    int		sizeLimit;
    TQComboBox::Policy p;
    bool	autoresize;
    bool	poppedUp;
    bool	mouseWasInsidePopup;
    bool	arrowPressed;
    bool	arrowDown;
    bool	discardNextMousePress;
    bool	shortClick;
    bool	useCompletion;
    bool	completeNow;
    int		completeAt;
    bool duplicatesEnabled;
    int fullHeight, currHeight;

    TQLineEdit * ed;  // /bin/ed rules!
    TQTimer *completionTimer;

    TQSize sizeHint;

private:
    bool	usingLBox;
    TQComboBoxPopup *pop;
    TQListBox   *lBox;
    TQComboBox *combo;

};

void TQComboBoxData::updateLinedGeometry()
{
    if ( !ed || !combo )
	return;
    TQRect r = TQStyle::visualRect( combo->style().querySubControlMetrics(TQStyle::CC_ComboBox, combo,
								       TQStyle::SC_ComboBoxEditField), combo );

    const TQPixmap *pix = current < combo->count() ? combo->pixmap( current ) : 0;
    if ( pix && pix->width() < r.width() )
	r.setLeft( r.left() + pix->width() + 4 );
    if ( r != ed->geometry() )
	ed->setGeometry( r );
}

void TQComboBoxData::setListBox( TQListBox *l )
{
    lBox = l;
    usingLBox = true;
    l->setMouseTracking( true );
#ifdef TQ_WS_X11
    l->x11SetWindowType( TQWidget::X11WindowTypeCombo );
    l->x11SetWindowTransient( combo->topLevelWidget());
#endif
}

void TQComboBoxData::setPopupMenu( TQComboBoxPopup * pm, bool isPopup )
{
    pop = pm;
    if(isPopup)
        usingLBox = false;
#ifdef TQ_WS_X11
    if( pm ) {
        pm->x11SetWindowType( TQWidget::X11WindowTypeCombo );
        pm->x11SetWindowTransient( combo->topLevelWidget());
    }
#endif
}

static inline bool checkInsertIndex( const char *method, const char * name,
				     int count, int *index)
{
    bool range_err = (*index > count);
#if defined(QT_CHECK_RANGE)
    if ( range_err )
	tqWarning( "TQComboBox::%s: (%s) Index %d out of range",
		 method, name ? name : "<no name>", *index );
#else
    Q_UNUSED( method )
    Q_UNUSED( name )
#endif
    if ( *index < 0 )				// append
	*index = count;
    return !range_err;
}


static inline bool checkIndex( const char *method, const char * name,
			       int count, int index )
{
    bool range_err = (index >= count);
#if defined(QT_CHECK_RANGE)
    if ( range_err )
	tqWarning( "TQComboBox::%s: (%s) Index %i out of range",
		 method, name ? name : "<no name>", index );
#else
    Q_UNUSED( method )
    Q_UNUSED( name )
#endif
    return !range_err;
}



/*!
    Constructs a combobox widget with parent \a parent called \a name.

    This constructor creates a popup list if the program uses Motif
    (or Aqua) look and feel; this is compatible with Motif 1.x and
    Aqua.

    Note: If you use this constructor to create your TQComboBox, then
    the pixmap() function will always return 0. To workaround this,
    use the other constructor.

*/



TQComboBox::TQComboBox( TQWidget *parent, const char *name )
    : TQWidget( parent, name, WNoAutoErase )
{
    d = new TQComboBoxData( this );
    if ( style().styleHint(TQStyle::SH_ComboBox_Popup, this) ||
	 style().styleHint(TQStyle::SH_GUIStyle) == TQt::MotifStyle ) {
	d->setPopupMenu( new TQComboBoxPopup( this, "in-combo" ) );
	d->popup()->setFont( font() );
	connect( d->popup(), TQ_SIGNAL(activated(int)),
			     TQ_SLOT(internalActivate(int)) );
	connect( d->popup(), TQ_SIGNAL(highlighted(int)),
			     TQ_SLOT(internalHighlight(int)) );
    } else {
	setUpListBox();
    }
    d->ed                    = 0;
    d->current               = 0;
    d->maxCount              = INT_MAX;
    d->sizeLimit	     = 10;
    d->p                     = AtBottom;
    d->autoresize            = false;
    d->poppedUp              = false;
    d->arrowDown             = false;
    d->arrowPressed          = false;
    d->discardNextMousePress = false;
    d->shortClick            = false;
    d->useCompletion         = false;
    d->completeAt            = 0;
    d->completeNow           = false;
    d->completionTimer       = new TQTimer( this );

    setFocusPolicy( TabFocus );
    setBackgroundMode( PaletteButton );
}


/*!
    Constructs a combobox with a maximum size and either Motif 2.0 or
    Windows look and feel.

    The input field can be edited if \a rw is true, otherwise the user
    may only choose one of the items in the combobox.

    The \a parent and \a name arguments are passed on to the TQWidget
    constructor.
*/


TQComboBox::TQComboBox( bool rw, TQWidget *parent, const char *name )
    : TQWidget( parent, name, WNoAutoErase )
{
    d = new TQComboBoxData( this );
    setUpListBox();

    if(d->popup() && style().styleHint(TQStyle::SH_ComboBox_Popup, this))
	d->popup()->setItemChecked(d->current, false);
    d->current = 0;
    d->maxCount = INT_MAX;
    setSizeLimit(10);
    d->p = AtBottom;
    d->autoresize = false;
    d->poppedUp = false;
    d->arrowDown = false;
    d->discardNextMousePress = false;
    d->shortClick = false;
    d->useCompletion = false;
    d->completeAt = 0;
    d->completeNow = false;
    d->completionTimer = new TQTimer( this );

    setFocusPolicy( StrongFocus );

    d->ed = 0;
    if ( rw )
	setUpLineEdit();
    setBackgroundMode( PaletteButton, PaletteBase );
}



/*!
    Destroys the combobox.
*/

TQComboBox::~TQComboBox()
{
    delete d;
}

void TQComboBox::setDuplicatesEnabled( bool enable )
{
   d->duplicatesEnabled = enable;
}

bool TQComboBox::duplicatesEnabled() const
{
    return d->duplicatesEnabled;
}

int TQComboBox::count() const
{
    if ( d->usingListBox() )
	return d->listBox()->count();
    else
	return d->popup()->count();
}


/*!
    \overload

    Inserts the \a list of strings at position \a index in the
    combobox.

    This is only for compatibility since it does not support Unicode
    strings. See insertStringList().
*/

void TQComboBox::insertStrList( const TQStrList &list, int index )
{
    insertStrList( &list, index );
}

/*!
    \overload

    Inserts the \a list of strings at position \a index in the
    combobox.

    This is only for compatibility since it does not support Unicode
    strings. See insertStringList().
*/

void TQComboBox::insertStrList( const TQStrList *list, int index )
{
    if ( !list ) {
#if defined(QT_CHECK_NULL)
	Q_ASSERT( list != 0 );
#endif
	return;
    }
    TQStrListIterator it( *list );
    const char* tmp;
    if ( index < 0 )
	index = count();
    while ( (tmp=it.current()) ) {
	++it;
	if ( d->usingListBox() )
	    d->listBox()->insertItem( TQString::fromLatin1(tmp), index );
	else
	    d->popup()->insertItem( escapedComboString(TQString::fromLatin1(tmp)), index, index );
	if ( index++ == d->current && d->current < count() ) {
	    if ( d->ed ) {
		d->ed->setText( text( d->current ) );
		d->updateLinedGeometry();
	    } else
		update();
	    currentChanged();
	}
    }
    if ( index != count() )
	reIndex();
}

/*!
    Inserts the \a list of strings at position \a index in the
    combobox.
*/

void TQComboBox::insertStringList( const TQStringList &list, int index )
{
    TQStringList::ConstIterator it = list.begin();
    if ( index < 0 )
	index = count();
    while ( it != list.end() ) {
	if ( d->usingListBox() )
	    d->listBox()->insertItem( *it, index );
	else
	    d->popup()->insertItem( escapedComboString(*it), index, index );
	if ( index++ == d->current && d->current < count() ) {
	    if ( d->ed ) {
		d->ed->setText( text( d->current ) );
		d->updateLinedGeometry();
	    } else
		update();
	    currentChanged();
	}
	++it;
    }
    if ( index != count() )
	reIndex();
}

/*!
    Inserts the array of char * \a strings at position \a index in the
    combobox.

    The \a numStrings argument is the number of strings. If \a
    numStrings is -1 (default), the \a strings array must be
    terminated with 0.

    Example:
    \code
	static const char* items[] = { "red", "green", "blue", 0 };
	combo->insertStrList( items );
    \endcode

    \sa insertStringList()
*/

void TQComboBox::insertStrList( const char **strings, int numStrings, int index)
{
    if ( !strings ) {
#if defined(QT_CHECK_NULL)
	Q_ASSERT( strings != 0 );
#endif
	return;
    }
    if ( index < 0 )
	index = count();
    int i = 0;
    while ( (numStrings<0 && strings[i]!=0) || i<numStrings ) {
	if ( d->usingListBox() )
	    d->listBox()->insertItem( TQString::fromLatin1(strings[i]), index );
	else
	    d->popup()->insertItem( escapedComboString(TQString::fromLatin1(strings[i])), index, index );
	i++;
	if ( index++ == d->current && d->current < count()  ) {
	    if ( d->ed ) {
		d->ed->setText( text( d->current ) );
		d->updateLinedGeometry();
	    } else
		update();
	    currentChanged();
	}
    }
    if ( index != count() )
	reIndex();
}


/*!
    Inserts a text item with text \a t, at position \a index. The item
    will be appended if \a index is negative.
*/

void TQComboBox::insertItem( const TQString &t, int index )
{
    int cnt = count();
    if ( !checkInsertIndex( "insertItem", name(), cnt, &index ) )
	return;
    if ( d->usingListBox() )
	d->listBox()->insertItem( t, index );
    else
	d->popup()->insertItem( escapedComboString(t), index, index );
    if ( index != cnt )
	reIndex();
    if ( index == d->current && d->current < count()  ) {
	if ( d->ed ) {
	    d->ed->setText( text( d->current ) );
	    d->updateLinedGeometry();
	} else
	    update();
    }
    if ( index == d->current )
	currentChanged();
}

/*!
    \overload

    Inserts a \a pixmap item at position \a index. The item will be
    appended if \a index is negative.
*/

void TQComboBox::insertItem( const TQPixmap &pixmap, int index )
{
    int cnt = count();
    if ( !checkInsertIndex( "insertItem", name(), cnt, &index ) )
	return;
    if ( d->usingListBox() )
	d->listBox()->insertItem( pixmap, index );
    else
	d->popup()->insertItem( pixmap, index, index );
    if ( index != cnt )
	reIndex();
    if ( index == d->current && d->current < count()  ) {
	if ( d->ed ) {
	    d->ed->setText( text( d->current ) );
	    d->updateLinedGeometry();
	} else
	    update();
    }
    if ( index == d->current )
	currentChanged();
}

/*!
    \overload

    Inserts a \a pixmap item with additional text \a text at position
    \a index. The item will be appended if \a index is negative.
*/

void TQComboBox::insertItem( const TQPixmap &pixmap, const TQString& text, int index )
{
    int cnt = count();
    if ( !checkInsertIndex( "insertItem", name(), cnt, &index ) )
	return;
    if ( d->usingListBox() )
	d->listBox()->insertItem( pixmap, text, index );
    else
	d->popup()->insertItem( pixmap, escapedComboString(text), index, index );
    if ( index != cnt )
	reIndex();
    if ( index == d->current && d->current < count()  ) {
	if ( d->ed ) {
	    d->ed->setText( this->text( d->current ) );
	    d->updateLinedGeometry();
	} else
	    update();
    }
    if ( index == d->current )
	currentChanged();
}


/*!
    Removes the item at position \a index.
*/

void TQComboBox::removeItem( int index )
{
    int cnt = count();
    if ( !checkIndex( "removeItem", name(), cnt, index ) )
	return;
    if ( d->usingListBox() ) {
	if ( style().styleHint(TQStyle::SH_ComboBox_Popup, this) && d->popup() )
	    d->popup()->removeItemAt( index );
	d->listBox()->removeItem( index );
    } else {
	d->popup()->removeItemAt( index );
    }
    if ( index != cnt-1 )
	reIndex();
    if ( index == d->current ) {
	if ( d->ed ) {
	    TQString s = TQString::fromLatin1("");
	    if (d->current < cnt - 1)
		s = text( d->current );
	    d->ed->setText( s );
	    d->updateLinedGeometry();
	}
	else {
	    if ( d->usingListBox() ) {
		d->current = d->listBox()->currentItem();
	    } else {
		if (d->current > count()-1 && d->current > 0)
		    d->current--;
	    }
	    update();
	}
	currentChanged();
    }
    else {
	if ( !d->ed ) {
	    if (d->current < cnt - 1)
		setCurrentItem( d->current );
	    else
		setCurrentItem( d->current - 1 );
	}
    }

}


/*!
    Removes all combobox items.
*/

void TQComboBox::clear()
{
    if ( d->usingListBox() ) {
	if ( style().styleHint(TQStyle::SH_ComboBox_Popup, this) && d->popup() )
	    d->popup()->clear();
	d->listBox()->resize( 0, 0 );
	d->listBox()->clear();
    } else {
	d->popup()->clear();
    }

    if(d->popup() && style().styleHint(TQStyle::SH_ComboBox_Popup, this))
	d->popup()->setItemChecked(d->current, false);
    d->current = 0;
    if ( d->ed ) {
	d->ed->setText( TQString::fromLatin1("") );
	d->updateLinedGeometry();
    }
    currentChanged();
}


TQString TQComboBox::currentText() const
{
    if ( d->ed )
	return d->ed->text();
    else if ( d->current < count() )
	return text( currentItem() );
    else
	return TQString::null;
}

void TQComboBox::setCurrentText( const TQString& txt )
{
    int i;
    for ( i = 0; i < count(); i++)
	if ( text( i ) == txt )
	    break;
    if ( i < count() )
	setCurrentItem( i );
    else if ( d->ed )
	d->ed->setText( txt );
    else
	changeItem( txt, currentItem() );
}


/*!
    Returns the text item at position \a index, or TQString::null if
    the item is not a string.

    \sa currentText()
*/

TQString TQComboBox::text( int index ) const
{
    if ( !checkIndex( "text", name(), count(), index ) )
	return TQString::null;
    if ( d->usingListBox() ) {
	return d->listBox()->text( index );
    } else {
        TQString retText = d->popup()->text(index);
        retText.replace("&&", "&");
	return retText;
    }
}

/*!
    Returns the pixmap item at position \a index, or 0 if the item is
    not a pixmap.
*/

const TQPixmap *TQComboBox::pixmap( int index ) const
{
    if ( !checkIndex( "pixmap", name(), count(), index ) )
	return 0;
    if ( d->usingListBox() )
	return d->listBox()->pixmap( index );
    else
	return d->popup()->pixmap( index );
}

/*!
    Replaces the item at position \a index with the text \a t.
*/

void TQComboBox::changeItem( const TQString &t, int index )
{
    if ( !checkIndex( "changeItem", name(), count(), index ) )
	return;
    if ( d->usingListBox() )
	d->listBox()->changeItem( t, index );
    else
	d->popup()->changeItem( t, index );
    if ( index == d->current ) {
	if ( d->ed ) {
	    d->ed->setText( text( d->current ) );
	    d->updateLinedGeometry();
	} else
	    update();
    }
}

/*!
    \overload

    Replaces the item at position \a index with the pixmap \a im,
    unless the combobox is editable.

    \sa insertItem()
*/

void TQComboBox::changeItem( const TQPixmap &im, int index )
{
    if ( !checkIndex( "changeItem", name(), count(), index ) )
	return;
    if ( d->usingListBox() )
	d->listBox()->changeItem( im, index );
    else
	d->popup()->changeItem( im, index );
    if ( index == d->current )
	update();
}

/*!
    \overload

    Replaces the item at position \a index with the pixmap \a im and
    the text \a t.

    \sa insertItem()
*/

void TQComboBox::changeItem( const TQPixmap &im, const TQString &t, int index )
{
    if ( !checkIndex( "changeItem", name(), count(), index ) )
	return;
    if ( d->usingListBox() )
	d->listBox()->changeItem( im, t, index );
    else
	d->popup()->changeItem( im, t, index );
    if ( index == d->current )
	update();
}


int TQComboBox::currentItem() const
{
    return d->current;
}

void TQComboBox::setCurrentItem( int index )
{
    if ( index == d->current && !d->ed ) {
	return;
    }
    if ( !checkIndex( "setCurrentItem", name(), count(), index ) ) {
	return;
    }

    if ( d->usingListBox() && !( listBox()->item(index) && listBox()->item(index)->isSelectable() ) )
	return;

    if(d->popup() && style().styleHint(TQStyle::SH_ComboBox_Popup, this))
	d->popup()->setItemChecked(d->current, false);
    d->current = index;
    d->completeAt = 0;
    if ( d->ed ) {
	d->ed->setText( text( index ) );
	d->updateLinedGeometry();
    }
    // ### We want to keep ListBox's currentItem in sync, even if NOT popuped...
    if ( d->usingListBox() && d->listBox() ) {
	d->listBox()->setCurrentItem( index );
    } else {
	internalHighlight( index );
	// internalActivate( index ); ### this leads to weird behavior, as in 3.0.1
    }

    currentChanged();
}

bool TQComboBox::autoResize() const
{
    return d->autoresize;
}

void TQComboBox::setAutoResize( bool enable )
{
    if ( (bool)d->autoresize != enable ) {
	d->autoresize = enable;
	if ( enable )
	    adjustSize();
    }
}


/*!
    \reimp

    This implementation caches the size hint to avoid resizing when
    the contents change dynamically. To invalidate the cached value
    call setFont().
*/
TQSize TQComboBox::sizeHint() const
{
    if ( isVisible() && d->sizeHint.isValid() )
	return d->sizeHint;

    constPolish();
    int i, w;
    TQFontMetrics fm = fontMetrics();

    int maxW = count() ? 18 : 7 * fm.width(TQChar('x')) + 18;
    int maxH = TQMAX( fm.lineSpacing(), 14 ) + 2;

    if ( !d->usingListBox() ) {
	w = d->popup()->sizeHint().width() - 2* d->popup()->frameWidth();
	if ( w > maxW )
	    maxW = w;
    } else {
	for( i = 0; i < count(); i++ ) {
	    w = d->listBox()->item( i )->width( d->listBox() );
	    if ( w > maxW )
		maxW = w;
	}
    }

    d->sizeHint = (style().sizeFromContents(TQStyle::CT_ComboBox, this,
					    TQSize(maxW, maxH)).
		   expandedTo(TQApplication::globalStrut()));

    return d->sizeHint;
}


/*!
  \internal
  Receives activated signals from an internal popup list and emits
  the activated() signal.
*/

void TQComboBox::internalActivate( int index )
{
    if ( d->current != index ) {
	if ( !d->usingListBox() || listBox()->item( index )->isSelectable() ) {
	    if(d->popup() && style().styleHint(TQStyle::SH_ComboBox_Popup, this))
		d->popup()->setItemChecked(d->current, false);
	    d->current = index;
	    currentChanged();
	}
    }
    if ( d->usingListBox() )
	popDownListBox();
    else
	d->popup()->removeEventFilter( this );
    d->poppedUp = false;

    TQString t( text( index ) );
    if ( d->ed ) {
	d->ed->setText( t );
	d->updateLinedGeometry();
    }
    emit activated( index );
    emit activated( t );
}

/*!
  \internal
  Receives highlighted signals from an internal popup list and emits
  the highlighted() signal.
*/

void TQComboBox::internalHighlight( int index )
{
    emit highlighted( index );
    TQString t = text( index );
    if ( !t.isNull() )
	emit highlighted( t );
}

/*!
  \internal
  Receives timeouts after a click. Used to decide if a Motif style
  popup should stay up or not after a click.
*/
void TQComboBox::internalClickTimeout()
{
    d->shortClick = false;
}

/*!
    Sets the palette for both the combobox button and the combobox
    popup list to \a palette.
*/

void TQComboBox::setPalette( const TQPalette &palette )
{
    TQWidget::setPalette( palette );
    if ( d->listBox() )
	d->listBox()->setPalette( palette );
    if ( d->popup() )
	d->popup()->setPalette( palette );
}

/*!
    Sets the font for both the combobox button and the combobox popup
    list to \a font.
*/

void TQComboBox::setFont( const TQFont &font )
{
    d->sizeHint = TQSize();		// invalidate size hint
    TQWidget::setFont( font );
    if ( d->usingListBox() )
	d->listBox()->setFont( font );
    else
	d->popup()->setFont( font );
    if (d->ed)
	d->ed->setFont( font );
    if ( d->autoresize )
	adjustSize();
}


/*!\reimp
*/

void TQComboBox::resizeEvent( TQResizeEvent * e )
{
    if ( d->ed )
	d->updateLinedGeometry();
    if ( d->listBox() )
	d->listBox()->resize( width(), d->listBox()->height() );
    TQWidget::resizeEvent( e );
}

/*!\reimp
*/

void TQComboBox::paintEvent( TQPaintEvent * )
{
    TQPainter p( this );
    const TQColorGroup & g = colorGroup();
    p.setPen(g.text());

    TQStyle::SFlags flags = TQStyle::Style_Default;
    if (isEnabled())
	flags |= TQStyle::Style_Enabled;
    if (hasFocus())
	flags |= TQStyle::Style_HasFocus;
    if (hasMouse())
	flags |= TQStyle::Style_MouseOver;

    if ( width() < 5 || height() < 5 ) {
	qDrawShadePanel( &p, rect(), g, false, 2,
			 &g.brush( TQColorGroup::Button ) );
	return;
    }

    bool reverse = TQApplication::reverseLayout();
    if ( !d->usingListBox() &&
	 style().styleHint(TQStyle::SH_GUIStyle) == TQt::MotifStyle) {			// motif 1.x style
	int dist, buttonH, buttonW;
	dist     = 8;
	buttonH  = 7;
	buttonW  = 11;
	int xPos;
	int x0;
	int w = width() - dist - buttonW - 1;
	if ( reverse ) {
	    xPos = dist + 1;
	    x0 = xPos + 4;
	} else {
	    xPos = w;
	    x0 = 4;
	}
	qDrawShadePanel( &p, rect(), g, false,
			 style().pixelMetric(TQStyle::PM_DefaultFrameWidth, this),
			 &g.brush( TQColorGroup::Button ) );
	qDrawShadePanel( &p, xPos, (height() - buttonH)/2,
			 buttonW, buttonH, g, false,
			 style().pixelMetric(TQStyle::PM_DefaultFrameWidth, this) );
	TQRect clip( x0, 2, w - 2 - 4 - 5, height() - 4 );
	TQString str = d->popup()->text( this->d->current );
	if ( !str.isNull() ) {
	    p.drawText( clip, AlignCenter | SingleLine, str );
	}

	TQPixmap *pix = d->popup()->pixmap( this->d->current );
	TQIconSet *iconSet = d->popup()->iconSet( this->d->current );
	if (pix || iconSet) {
	    TQPixmap pm = ( pix ? *pix : iconSet->pixmap() );
	    p.setClipRect( clip );
	    p.drawPixmap( 4, (height()-pm.height())/2, pm );
	    p.setClipping( false );
	}

	if ( hasFocus() )
	    p.drawRect( xPos - 5, 4, width() - xPos + 1 , height() - 8 );
    } else if(!d->usingListBox()) {
	style().drawComplexControl( TQStyle::CC_ComboBox, &p, this, rect(), g,
				    flags, (uint)TQStyle::SC_All,
				    (d->arrowDown ?
				     TQStyle::SC_ComboBoxArrow :
				     TQStyle::SC_None ));

	TQRect re = style().querySubControlMetrics( TQStyle::CC_ComboBox, this,
						   TQStyle::SC_ComboBoxEditField );
	re = TQStyle::visualRect(re, this);
	p.setClipRect( re );

	TQString str = d->popup()->text( this->d->current );
	TQPixmap *pix = d->popup()->pixmap( this->d->current );
	if ( !str.isNull() ) {
	    p.save();
	    p.setFont(font());
	    TQFontMetrics fm(font());
	    int x = re.x(), y = re.y() + fm.ascent();
	    if( pix )
		x += pix->width() + 5;
	    p.drawText( x, y, str );
	    p.restore();
	}
	if ( pix ) {
	    p.fillRect( re.x(), re.y(), pix->width() + 4, re.height(),
			colorGroup().brush( TQColorGroup::Base ) );
	    p.drawPixmap( re.x() + 2, re.y() +
			  ( re.height() - pix->height() ) / 2, *pix );
	}
    } else {
	style().drawComplexControl( TQStyle::CC_ComboBox, &p, this, rect(), g,
				    flags, (uint)TQStyle::SC_All,
				    (d->arrowDown ?
				     TQStyle::SC_ComboBoxArrow :
				     TQStyle::SC_None ));

	TQRect re = style().querySubControlMetrics( TQStyle::CC_ComboBox, this,
						   TQStyle::SC_ComboBoxEditField );
	re = TQStyle::visualRect(re, this);
	p.setClipRect( re );

	if ( !d->ed ) {
	    TQListBoxItem * item = d->listBox()->item( d->current );
	    if ( item ) {
		int itemh = item->height( d->listBox() );
		p.translate( re.x(), re.y() + (re.height() - itemh)/2  );
		item->paint( &p );
	    }
	} else if ( d->listBox() && d->listBox()->item( d->current ) ) {
	    TQListBoxItem * item = d->listBox()->item( d->current );
	    const TQPixmap *pix = item->pixmap();
	    if ( pix ) {
		p.fillRect( re.x(), re.y(), pix->width() + 4, re.height(),
			    colorGroup().brush( TQColorGroup::Base ) );
		p.drawPixmap( re.x() + 2, re.y() +
			      ( re.height() - pix->height() ) / 2, *pix );
	    }
	}
	p.setClipping( false );
    }
}


/*!\reimp
*/

void TQComboBox::mousePressEvent( TQMouseEvent *e )
{
    if ( e->button() != LeftButton )
	return;
    if ( d->discardNextMousePress ) {
	d->discardNextMousePress = false;
	return;
    }
    TQRect arrowRect = style().querySubControlMetrics( TQStyle::CC_ComboBox, this,
						      TQStyle::SC_ComboBoxArrow);
    arrowRect = TQStyle::visualRect(arrowRect, this);

    // Correction for motif style, where arrow is smaller
    // and thus has a rect that doesn't fit the button.
    arrowRect.setHeight( TQMAX(  height() - (2 * arrowRect.y()), arrowRect.height() ) );

    if ( count() && ( !editable() || arrowRect.contains( e->pos() ) ) ) {
	d->arrowPressed = false;

	if ( d->usingListBox() ) {
	    listBox()->blockSignals( true );
	    tqApp->sendEvent( listBox(), e ); // trigger the listbox's autoscroll
	    listBox()->setCurrentItem(d->current);
	    listBox()->blockSignals( false );
	    popup();
	    if ( arrowRect.contains( e->pos() ) ) {
		d->arrowPressed = true;
		d->arrowDown    = true;
		repaint( false );
	    }
	} else {
	    popup();
	}
	TQTimer::singleShot( 200, this, TQ_SLOT(internalClickTimeout()));
	d->shortClick = true;
    }
}

/*!\reimp
*/

void TQComboBox::mouseMoveEvent( TQMouseEvent * )
{
}

/*!\reimp
*/

void TQComboBox::mouseReleaseEvent( TQMouseEvent * )
{
}

/*!\reimp
*/

void TQComboBox::mouseDoubleClickEvent( TQMouseEvent *e )
{
    mousePressEvent( e );
}


/*!\reimp
*/

void TQComboBox::keyPressEvent( TQKeyEvent *e )
{
    int c = currentItem();
    if ( ( e->key() == Key_F4 && e->state() == 0 ) ||
	 ( e->key() == Key_Down && (e->state() & AltButton) ) ||
	 ( !d->ed && e->key() == Key_Space ) ) {
	if ( count() ) {
	    if ( !d->usingListBox() )
		d->popup()->setActiveItem( this->d->current );
	    popup();
	}
	return;
    } else if ( d->usingListBox() && e->key() == Key_Up ) {
	if ( c > 0 )
	    setCurrentItem( c-1 );
    } else if ( d->usingListBox() && e->key() == Key_Down ) {
	if ( ++c < count() )
	    setCurrentItem( c );
    } else if ( d->usingListBox() && e->key() == Key_Home && ( !d->ed || !d->ed->hasFocus() ) ) {
	setCurrentItem( 0 );
    } else if ( d->usingListBox() && e->key() == Key_End && ( !d->ed || !d->ed->hasFocus() ) ) {
	setCurrentItem( count()-1 );
    } else if ( !d->ed && e->ascii() >= 32 && !e->text().isEmpty() ) {
	if ( !d->completionTimer->isActive() ) {
	    d->completeAt = 0;
	    c = completionIndex( e->text(), ++c );
	    if ( c >= 0 ) {
		setCurrentItem( c );
		d->completeAt = e->text().length();
	    }
	} else {
	    d->completionTimer->stop();
	    TQString ct = currentText().left( d->completeAt ) + e->text();
	    c = completionIndex( ct, c );
	    if ( c < 0 && d->completeAt > 0 ) {
		c = completionIndex( e->text(), 0 );
		ct = e->text();
	    }
	    d->completeAt = 0;
	    if ( c >= 0 ) {
		setCurrentItem( c );
		d->completeAt = ct.length();
	    }
	}
	d->completionTimer->start( 400, true );
    } else {
	e->ignore();
	return;
    }

    c = currentItem();
    if ( count() && !text( c ).isNull() )
	emit activated( text( c ) );
    emit activated( c );
}


/*!\reimp
*/

void TQComboBox::focusInEvent( TQFocusEvent * e )
{
    TQWidget::focusInEvent( e );
    d->completeNow = false;
    d->completeAt = 0;
}

/*!\reimp
*/

void TQComboBox::focusOutEvent( TQFocusEvent * e )
{
    TQWidget::focusOutEvent( e );
    d->completeNow = false;
    d->completeAt = 0;
}

/*!\reimp
*/
#ifndef TQT_NO_WHEELEVENT
void TQComboBox::wheelEvent( TQWheelEvent *e )
{
    if ( d->poppedUp ) {
	if ( d->usingListBox() ) {
	    TQApplication::sendEvent( d->listBox(), e );
	}
    } else {
	if ( e->delta() > 0 ) {
	    int c = currentItem();
	    if ( c > 0 ) {
		setCurrentItem( c-1 );
		emit activated( currentItem() );
		emit activated( currentText() );
	    }
	} else {
	    int c = currentItem();
	    if ( ++c < count() ) {
		setCurrentItem( c );
		emit activated( currentItem() );
		emit activated( currentText() );
	    }
	}
	e->accept();
    }
}
#endif

/*!
  \internal
   Calculates the listbox height needed to contain all items, or as
   many as the list box is supposed to contain.
*/
static int listHeight( TQListBox *l, int sl )
{
    if ( l->count() > 0 )
	return TQMIN( l->count(), (uint)sl) * l->item( 0 )->height(l);
    else
	return l->sizeHint().height();
}


/*!
    Pops up the combobox popup list.

    If the list is empty, no items appear.
*/

void TQComboBox::popup()
{
    if ( !count() || d->poppedUp )
	return;

    if( !d->usingListBox() || style().styleHint(TQStyle::SH_ComboBox_Popup, this) ) {
	if(d->usingListBox()) {
	    if(!d->popup()) {
		TQComboBoxPopup *p = new TQComboBoxPopup( this, "in-combo" );
		d->setPopupMenu( p, false );
		p->setFont( font() );
		connect( p, TQ_SIGNAL(activated(int)), TQ_SLOT(internalActivate(int)) );
		connect( p, TQ_SIGNAL(highlighted(int)), TQ_SLOT(internalHighlight(int)) );
	    }
	    d->popup()->clear();
	    for(unsigned int i = 0; i < d->listBox()->count(); i++) {
		TQListBoxItem *item = d->listBox()->item(i);
		if(item->rtti() == TQListBoxText::RTTI) {
		    d->popup()->insertItem(escapedComboString(item->text()), i, i);
		} else if(item->rtti() == TQListBoxPixmap::RTTI) {
		    if(item->pixmap())
			d->popup()->insertItem(TQIconSet(*item->pixmap()), escapedComboString(item->text()), i, i);
		    else
			d->popup()->insertItem(escapedComboString(item->text()), i, i);
		} else {
		    d->popup()->insertItem(new TQComboBoxPopupItem(item), i, i);
		}
	    }
	}
	d->popup()->installEventFilter( this );
	if(d->popup() && style().styleHint(TQStyle::SH_ComboBox_Popup, this))
	    d->popup()->setItemChecked(this->d->current, true);
	d->popup()->popup( mapToGlobal( TQPoint(0,0) ), this->d->current );
	update();
    } else {
	// Send all listbox events to eventFilter():
	TQListBox* lb = d->listBox();
	lb->triggerUpdate( true );
	lb->installEventFilter( this );
	d->mouseWasInsidePopup = false;
	int w = lb->variableWidth() ? lb->sizeHint().width() : width();
	int h = listHeight( lb, d->sizeLimit ) + 2;
	TQRect screen = TQApplication::desktop()->availableGeometry( this );

	int sx = screen.x();				// screen pos
	int sy = screen.y();
	int sw = screen.width();			// screen width
	int sh = screen.height();			// screen height
	TQPoint pos = mapToGlobal( TQPoint(0,height()) );
	// ## Similar code is in TQPopupMenu
	int x = pos.x();
	int y = pos.y();

	// the complete widget must be visible
	if ( x + w > sx + sw )
	    x = sx+sw - w;
	if ( x < sx )
	    x = sx;
	if (y + h > sy+sh && y - h - height() >= 0 )
	    y = y - h - height();

       	TQRect rect =
	    style().querySubControlMetrics( TQStyle::CC_ComboBox, this,
					    TQStyle::SC_ComboBoxListBoxPopup,
					    TQStyleOption( x, y, w, h ) );
	// work around older styles that don't implement the combobox
	// listbox popup subcontrol
	if ( rect.isNull() )
	    rect.setRect( x, y, w, h );
	lb->setGeometry( rect );

	lb->raise();
	bool block = lb->signalsBlocked();
	lb->blockSignals( true );
	TQListBoxItem* currentLBItem = 0;
	if ( editable() && currentText() != text( currentItem() ) )
	    currentLBItem = lb->findItem( currentText() );

	currentLBItem = currentLBItem ? currentLBItem : lb->item( d->current );

	lb->setCurrentItem( currentLBItem );
	lb->setContentsPos( lb->contentsX(),
			    lb->viewportToContents( lb->itemRect( currentLBItem ).topLeft() ).y() );

	// set the current item to also be the selected item if it isn't already
	if ( currentLBItem && currentLBItem->isSelectable() && !currentLBItem->isSelected() )
	    lb->setSelected( currentLBItem, true );
	lb->blockSignals( block );
	lb->setVScrollBarMode(TQScrollView::Auto);

#ifndef TQT_NO_EFFECTS
	if ( TQApplication::isEffectEnabled( UI_AnimateCombo ) ) {
	    if ( lb->y() < mapToGlobal(TQPoint(0,0)).y() )
		qScrollEffect( lb, TQEffects::UpScroll );
	    else
		qScrollEffect( lb );
	} else
#endif
	    lb->show();
    }
    d->poppedUp = true;
}


/*!
  \reimp
*/
void TQComboBox::updateMask()
{
    TQBitmap bm( size() );
    bm.fill( color0 );

    {
	TQPainter p( &bm, this );
	style().drawComplexControlMask(TQStyle::CC_ComboBox, &p, this, rect());
    }

    setMask( bm );
}

/*!
  \internal
  Pops down (removes) the combobox popup list box.
*/
void TQComboBox::popDownListBox()
{
    Q_ASSERT( d->usingListBox() );
    d->listBox()->removeEventFilter( this );
    d->listBox()->viewport()->removeEventFilter( this );
    d->listBox()->hide();
    d->listBox()->setCurrentItem( d->current );
    if ( d->arrowDown ) {
	d->arrowDown = false;
	repaint( false );
    }
    d->poppedUp = false;

    update();
}


/*!
  \internal
  Re-indexes the identifiers in the popup list.
*/

void TQComboBox::reIndex()
{
    if ( !d->usingListBox() ) {
	int cnt = count();
	while ( cnt-- )
	    d->popup()->setId( cnt, cnt );
    }
}

/*!
  \internal
  Repaints the combobox.
*/

void TQComboBox::currentChanged()
{
    if ( d->autoresize )
	adjustSize();
    update();

#if defined(QT_ACCESSIBILITY_SUPPORT)
    TQAccessible::updateAccessibility( this, 0, TQAccessible::ValueChanged );
#endif
}

/*! \reimp

  \internal

  The event filter steals events from the popup or listbox when they
  are popped up. It makes the popup stay up after a short click in
  motif style. In windows style it toggles the arrow button of the
  combobox field, and activates an item and takes down the listbox
  when the mouse button is released.
*/

bool TQComboBox::eventFilter( TQObject *object, TQEvent *event )
{
    if ( !event )
	return true;
    else if ( object == d->ed ) {
	if ( event->type() == TQEvent::KeyPress ) {
	    bool isAccepted = ( (TQKeyEvent*)event )->isAccepted();
	    keyPressEvent( (TQKeyEvent *)event );
	    if ( ((TQKeyEvent *)event)->isAccepted() ) {
		d->completeNow = false;
		return true;
	    } else if ( ((TQKeyEvent *)event)->key() != Key_End ) {
		d->completeNow = true;
		d->completeAt = d->ed->cursorPosition();
	    }
	    if ( isAccepted )
		( (TQKeyEvent*)event )->accept();
	    else
		( (TQKeyEvent*)event )->ignore();
	} else if ( event->type() == TQEvent::KeyRelease ) {
	    keyReleaseEvent( (TQKeyEvent *)event );
	    return ((TQKeyEvent *)event)->isAccepted();
	} else if ( event->type() == TQEvent::FocusIn ) {
	    focusInEvent( (TQFocusEvent *)event );
	} else if ( event->type() == TQEvent::FocusOut ) {
	    focusOutEvent( (TQFocusEvent *)event );
	} else if ( d->useCompletion && d->completeNow ) {
	    d->completeNow = false;
	    if ( !d->ed->text().isNull() &&
		 d->ed->cursorPosition() > d->completeAt &&
		 d->ed->cursorPosition() == (int)d->ed->text().length() ) {
		TQString ct( d->ed->text() );
		int i = completionIndex( ct, currentItem() );
		if ( i > -1 ) {
		    TQString it = text( i );
		    d->ed->validateAndSet( it, ct.length(),
					   ct.length(), it.length() );
		    d->current = i;
                    // ### sets current item without emitting signals. This is to
		    // make sure the right item is current if you change current with
		    // wheel/up/down. While typing current is not valid anyway. Fix properly
		    // in 4.0.
		}
	    }
	}
    } else if ( d->usingListBox() && ( object == d->listBox() ||
                                       object == d->listBox()->viewport() )) {
	TQMouseEvent *e = (TQMouseEvent*)event;
	switch( event->type() ) {
	case TQEvent::MouseMove:
	    if ( !d->mouseWasInsidePopup  ) {
		TQPoint pos = e->pos();
		if ( d->listBox()->rect().contains( pos ) )
		    d->mouseWasInsidePopup = true;
		// Check if arrow button should toggle
		if ( d->arrowPressed ) {
		    TQPoint comboPos;
		    comboPos = mapFromGlobal( d->listBox()->mapToGlobal(pos) );
		    TQRect arrowRect =
			style().querySubControlMetrics( TQStyle::CC_ComboBox, this,
							TQStyle::SC_ComboBoxArrow);
		    arrowRect = TQStyle::visualRect(arrowRect, this);
		    if ( arrowRect.contains( comboPos ) ) {
			if ( !d->arrowDown  ) {
			    d->arrowDown = true;
			    repaint( false );
			}
		    } else {
			if ( d->arrowDown  ) {
			    d->arrowDown = false;
			    repaint( false );
			}
		    }
		}
	    } else if ((e->state() & ( RightButton | LeftButton | MidButton ) ) == 0 &&
		       style().styleHint(TQStyle::SH_ComboBox_ListMouseTracking, this)) {
		TQWidget *mouseW = TQApplication::widgetAt( e->globalPos(), true );
		if ( mouseW == d->listBox()->viewport() ) { //###
		    TQMouseEvent m( TQEvent::MouseMove, e->pos(), e->globalPos(),
				   LeftButton, LeftButton );
		    TQApplication::sendEvent( object, &m ); //### Evil
		    return true;
		}
	    }

	    break;
	case TQEvent::MouseButtonRelease:
	    if ( d->listBox()->rect().contains( e->pos() ) ) {
		TQMouseEvent tmp( TQEvent::MouseButtonDblClick,
				 e->pos(), e->button(), e->state() ) ;
		// will hide popup
		TQApplication::sendEvent( object, &tmp );
		return true;
	    } else {
		if ( d->mouseWasInsidePopup ) {
		    popDownListBox();
		} else {
		    d->arrowPressed = false;
		    if ( d->arrowDown  ) {
			d->arrowDown = false;
			repaint( false );
		    }
		}
	    }
	    break;
	case TQEvent::MouseButtonDblClick:
	case TQEvent::MouseButtonPress:
	    if ( !d->listBox()->rect().contains( e->pos() ) ) {
		TQPoint globalPos = d->listBox()->mapToGlobal(e->pos());
		if ( TQApplication::widgetAt( globalPos, true ) == this ) {
		    d->discardNextMousePress = true;
		    // avoid popping up again
		}
		popDownListBox();
		return true;
	    }
	    break;
	case TQEvent::KeyPress:
	    switch( ((TQKeyEvent *)event)->key() ) {
	    case Key_Up:
	    case Key_Down:
		if ( !(((TQKeyEvent *)event)->state() & AltButton) )
		    break;
	    case Key_F4:
	    case Key_Escape:
		if ( d->poppedUp ) {
		    popDownListBox();
		    return true;
		}
		break;
	    case Key_Enter:
	    case Key_Return:
		// work around TQDialog's enter handling
		return false;
	    default:
		break;
	    }
	    break;
	case TQEvent::Hide:
	    popDownListBox();
	    break;
	default:
	    break;
	}
    } else if ( (!d->usingListBox() || style().styleHint(TQStyle::SH_ComboBox_Popup, this)) &&
		object == d->popup() ) {
	TQMouseEvent *e = (TQMouseEvent*)event;
	switch ( event->type() ) {
	case TQEvent::MouseButtonRelease:
	    if ( d->shortClick ) {
		TQMouseEvent tmp( TQEvent::MouseMove,
				 e->pos(), e->button(), e->state() ) ;
		// highlight item, but don't pop down:
		TQApplication::sendEvent( object, &tmp );
		return true;
	    }
	    break;
	case TQEvent::MouseButtonDblClick:
	case TQEvent::MouseButtonPress:
	    if ( !d->popup()->rect().contains( e->pos() ) ) {
                d->poppedUp = false;
                d->arrowDown = false;
		// remove filter, event will take down popup:
		d->popup()->removeEventFilter( this );
		// ### uglehack!
		// call internalHighlight so the highlighed signal
		// will be emitted at least as often as necessary.
		// it may be called more often than necessary
		internalHighlight( d->current );
	    }
	    break;
	case TQEvent::Hide:
	    d->poppedUp = false;
	    break;
	default:
	    break;
	}
    }
    return TQWidget::eventFilter( object, event );
}


/*!
    Returns the index of the first item \e after \a startingAt of
    which \a prefix is a case-insensitive prefix. Returns -1 if no
    items start with \a prefix.
*/

int TQComboBox::completionIndex( const TQString & prefix,
				int startingAt = 0 ) const
{
    int start = startingAt;
    if ( start < 0 || start >= count() )
	start = 0;
    if ( start >= count() )
	return -1;
    TQString match = prefix.lower();
    if ( match.length() < 1 )
	return start;

    TQString current;
    int i = start;
    do {
	current = text( i ).lower();
	if ( current.startsWith( match ) )
	    return i;
	i++;
	if ( i == count() )
	    i = 0;
    } while ( i != start );
    return -1;
}


int TQComboBox::sizeLimit() const
{
    return d ? d->sizeLimit : INT_MAX;
}

void TQComboBox::setSizeLimit( int lines )
{
    d->sizeLimit = lines;
}


int TQComboBox::maxCount() const
{
    return d ? d->maxCount : INT_MAX;
}

void TQComboBox::setMaxCount( int count )
{
    int l = this->count();
    while( --l > count )
	removeItem( l );
    d->maxCount = count;
}

TQComboBox::Policy TQComboBox::insertionPolicy() const
{
    return d->p;
}

void TQComboBox::setInsertionPolicy( Policy policy )
{
    d->p = policy;
}



/*!
  Internal slot to keep the line editor up to date.
*/

void TQComboBox::returnPressed()
{
    TQString s( d->ed->text() );

    if ( s.isEmpty() )
	return;

    int c = 0;
    bool doInsert = true;
    if ( !d->duplicatesEnabled ) {
	for ( int i = 0; i < count(); ++i ) {
	    if ( s == text( i ) ) {
		doInsert = false;
		c = i;
		break;
	    }
	}
    }

    if ( doInsert ) {
	if ( insertionPolicy() != NoInsertion ) {
	    int cnt = count();
	    while ( cnt >= d->maxCount ) {
		removeItem( --cnt );
	    }
	}

	switch ( insertionPolicy() ) {
	case AtCurrent:
	    if (count() == 0)
		insertItem(s);
	    else if ( s != text( currentItem() ) )
		changeItem( s, currentItem() );
	    emit activated( currentItem() );
	    emit activated( s );
	    return;
	case NoInsertion:
	    emit activated( s );
	    return;
	case AtTop:
	    c = 0;
	    break;
	case AtBottom:
	    c = count();
	    break;
	case BeforeCurrent:
	    c = currentItem();
	    break;
	case AfterCurrent:
	    c = count() == 0 ? 0 : currentItem() + 1;
	    break;
	}
	insertItem( s, c );
    }

    setCurrentItem( c );
    emit activated( c );
    emit activated( s );
}


/*! \reimp
*/

void TQComboBox::setEnabled( bool enable )
{
    if ( !enable ) {
	if ( d->usingListBox() ) {
	    popDownListBox();
	} else {
	    d->popup()->removeEventFilter( this );
	    d->popup()->close();
	    d->poppedUp = false;
	}
    }
    TQWidget::setEnabled( enable );
}



/*!
    Applies the validator \a v to the combobox so that only text which
    is valid according to \a v is accepted.

    This function does nothing if the combobox is not editable.

    \sa validator() clearValidator() TQValidator
*/

void TQComboBox::setValidator( const TQValidator * v )
{
    if ( d && d->ed )
	d->ed->setValidator( v );
}


/*!
    Returns the validator which constrains editing for this combobox
    if there is one; otherwise returns 0.

    \sa setValidator() clearValidator() TQValidator
*/

const TQValidator * TQComboBox::validator() const
{
    return d && d->ed ? d->ed->validator() : 0;
}


/*!
    This slot is equivalent to setValidator( 0 ).
*/

void TQComboBox::clearValidator()
{
    if ( d && d->ed )
	d->ed->setValidator( 0 );
}


/*!
    Sets the combobox to use \a newListBox instead of the current list
    box or popup. As a side effect, it clears the combobox of its
    current contents.

    \warning TQComboBox assumes that newListBox->text(n) returns
    non-null for 0 \<= n \< newListbox->count(). This assumption is
    necessary because of the line edit in TQComboBox.
*/

void TQComboBox::setListBox( TQListBox * newListBox )
{
    clear();

    if ( d->usingListBox() ) {
	delete d->listBox();
    } else {
	delete d->popup();
        d->setPopupMenu(0, false);
    }

    newListBox->reparent( this, WType_Popup, TQPoint(0,0), false );
    d->setListBox( newListBox );
    d->listBox()->setFont( font() );
    d->listBox()->setPalette( palette() );
    d->listBox()->setVScrollBarMode(TQScrollView::AlwaysOff);
    d->listBox()->setHScrollBarMode(TQScrollView::AlwaysOff);
    d->listBox()->setFrameStyle( TQFrame::Box | TQFrame::Plain );
    d->listBox()->setLineWidth( 1 );
    d->listBox()->resize( 100, 10 );

    connect( d->listBox(), TQ_SIGNAL(selected(int)),
	     TQ_SLOT(internalActivate(int)) );
    connect( d->listBox(), TQ_SIGNAL(highlighted(int)),
	     TQ_SLOT(internalHighlight(int)));
}


/*!
    Returns the current list box, or 0 if there is no list box.
    (TQComboBox can use TQPopupMenu instead of TQListBox.) Provided to
    match setListBox().

    \sa setListBox()
*/

TQListBox * TQComboBox::listBox() const
{
    return d && d->usingListBox() ? d->listBox() : 0;
}

/*!
    Returns the line edit, or 0 if there is no line edit.

    Only editable listboxes have a line editor.
*/
TQLineEdit* TQComboBox::lineEdit() const
{
    return d->ed;
}



/*!
    Clears the line edit without changing the combobox's contents.
    Does nothing if the combobox isn't editable.

    This is particularly useful when using a combobox as a line edit
    with history. For example you can connect the combobox's
    activated() signal to clearEdit() in order to present the user
    with a new, empty line as soon as Enter is pressed.

    \sa setEditText()
*/

void TQComboBox::clearEdit()
{
    if ( d && d->ed )
	d->ed->clear();
}


/*!
    Sets the text in the line edit to \a newText without changing the
    combobox's contents. Does nothing if the combobox isn't editable.

    This is useful e.g. for providing a good starting point for the
    user's editing and entering the change in the combobox only when
    the user presses Enter.

    \sa clearEdit() insertItem()
*/

void TQComboBox::setEditText( const TQString &newText )
{
    if ( d && d->ed ) {
	d->updateLinedGeometry();
	d->ed->setText( newText );
    }
}

void TQComboBox::setAutoCompletion( bool enable )
{
    d->useCompletion = enable;
    d->completeNow = false;
}


bool TQComboBox::autoCompletion() const
{
    return d->useCompletion;
}

/*!\reimp
 */
void TQComboBox::styleChange( TQStyle& s )
{
    d->sizeHint = TQSize();		// invalidate size hint...
    if ( d->ed )
	d->updateLinedGeometry();
    TQWidget::styleChange( s );
}

bool TQComboBox::editable() const
{
    return d->ed != 0;
}

void TQComboBox::setEditable( bool y )
{
    if ( y == editable() )
	return;
    if ( y ) {
	if ( !d->usingListBox() )
	    setUpListBox();
	setUpLineEdit();
	d->ed->show();
	if ( currentItem() )
	    setEditText( currentText() );
    } else {
	delete d->ed;
	d->ed = 0;
    }

    setFocusPolicy( StrongFocus );
    updateGeometry();
    update();
}


void TQComboBox::setUpListBox()
{
    d->setListBox( new TQListBox( this, "in-combo", WType_Popup ) );
    d->listBox()->setFont( font() );
    d->listBox()->setPalette( palette() );
    d->listBox()->setVScrollBarMode( TQListBox::AlwaysOff );
    d->listBox()->setHScrollBarMode( TQListBox::AlwaysOff );
    d->listBox()->setFrameStyle( TQFrame::Box | TQFrame::Plain );
    d->listBox()->setLineWidth( 1 );
    d->listBox()->resize( 100, 10 );

    connect( d->listBox(), TQ_SIGNAL(selected(int)),
	     TQ_SLOT(internalActivate(int)) );
    connect( d->listBox(), TQ_SIGNAL(highlighted(int)),
	     TQ_SLOT(internalHighlight(int)));
}


void TQComboBox::setUpLineEdit()
{
    if ( !d->ed )
	setLineEdit( new TQLineEdit( this, "combo edit" ) );
}

/*!
    Sets the line edit to use \a edit instead of the current line edit.
*/

void TQComboBox::setLineEdit( TQLineEdit *edit )
{
    if ( !edit ) {
#if defined(QT_CHECK_NULL)
	Q_ASSERT( edit != 0 );
#endif
	return;
    }

    edit->setText( currentText() );
    delete d->ed;
    d->ed = edit;

    if ( edit->parent() != this )
	edit->reparent( this, TQPoint(0,0), false );

    connect (edit, TQ_SIGNAL( textChanged(const TQString&) ),
	     this, TQ_SIGNAL( textChanged(const TQString&) ) );
    connect( edit, TQ_SIGNAL(returnPressed()), TQ_SLOT(returnPressed()) );

    edit->setFrame( false );
    d->updateLinedGeometry();
    edit->installEventFilter( this );
    setFocusProxy( edit );
    setFocusPolicy( StrongFocus );
    setInputMethodEnabled( true );

    if ( !d->usingListBox() )
	setUpListBox();

    if ( isVisible() )
	edit->show();

    updateGeometry();
    update();
}

/*!
    \reimp
*/
void TQComboBox::hide()
{
    TQWidget::hide();

    if (listBox())
	listBox()->hide();
    else if (d->popup())
	d->popup()->hide();
}

#endif // TQT_NO_COMBOBOX
