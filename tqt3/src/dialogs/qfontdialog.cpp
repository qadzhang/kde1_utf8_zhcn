/****************************************************************************
**
** Implementation of TQFontDialog
**
** Created : 970605
**
** Copyright (C) 1992-2008 Trolltech ASA.  All rights reserved.
**
** This file is part of the dialogs module of the TQt GUI Toolkit.
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

#include "ntqwindowdefs.h"

#ifndef TQT_NO_FONTDIALOG

#include "ntqfontdialog.h"

#include "ntqlineedit.h"
#include "ntqlistbox.h"
#include "ntqpushbutton.h"
#include "ntqcheckbox.h"
#include "ntqcombobox.h"
#include "ntqlayout.h"
#include "ntqvgroupbox.h"
#include "ntqhgroupbox.h"
#include "ntqlabel.h"
#include "ntqapplication.h"
#include "ntqfontdatabase.h"
#include "ntqstyle.h"
#include <private/qfontdata_p.h>
#include <ntqvalidator.h>

#ifdef TQ_WS_X11
#include "private/tqttdeintegration_x11_p.h"
#endif

/*!
  \class TQFontDialog ntqfontdialog.h
  \ingroup dialogs
  \mainclass
  \brief The TQFontDialog class provides a dialog widget for selecting a font.

  The usual way to use this class is to call one of the static convenience
  functions, e.g. getFont().

  Examples:

  \code
    bool ok;
    TQFont font = TQFontDialog::getFont(
		    &ok, TQFont( "Helvetica [Cronyx]", 10 ), this );
    if ( ok ) {
	// font is set to the font the user selected
    } else {
	// the user canceled the dialog; font is set to the initial
	// value, in this case Helvetica [Cronyx], 10
    }
  \endcode

    The dialog can also be used to set a widget's font directly:
  \code
    myWidget.setFont( TQFontDialog::getFont( 0, myWidget.font() ) );
  \endcode
  If the user clicks OK the font they chose will be used for myWidget,
  and if they click Cancel the original font is used.

  \sa TQFont TQFontInfo TQFontMetrics

  <img src=qfontdlg-w.png>
*/

class TQFontDialogPrivate
{
public:
    TQFontDialogPrivate() : script( TQFontPrivate::defaultScript ) {};
    TQLabel * familyAccel;
    TQLineEdit * familyEdit;
    TQListBox * familyList;

    TQLabel * styleAccel;
    TQLineEdit * styleEdit;
    TQListBox * styleList;

    TQLabel * sizeAccel;
    TQLineEdit * sizeEdit;
    TQListBox * sizeList;

    TQVGroupBox * effects;
    TQCheckBox * strikeout;
    TQCheckBox * underline;
    TQComboBox * color;

    TQHGroupBox * sample;
    TQLineEdit * sampleEdit;

    TQLabel * scriptAccel;
    TQComboBox * scriptCombo;

    TQPushButton * ok;
    TQPushButton * cancel;

    TQBoxLayout * buttonLayout;
    TQBoxLayout * effectsLayout;
    TQBoxLayout * sampleLayout;
    TQBoxLayout * sampleEditLayout;

    TQFontDatabase fdb;

    TQString       family;
    TQFont::Script script;
    TQString       style;
    int           size;

    bool smoothScalable;
};


/*!
  \internal
  Constructs a standard font dialog.

  Use setFont() to set the initial font attributes.

  The \a parent, \a name, \a modal and \a f parameters are passed to
  the TQDialog constructor.

  \sa getFont()
*/

TQFontDialog::TQFontDialog( TQWidget *parent, const char *name,
			  bool modal, WFlags f )
    : TQDialog( parent, name, modal, f )
{
    setSizeGripEnabled( true );
    d = new TQFontDialogPrivate;
    // grid
    d->familyEdit = new TQLineEdit( this, "font family I" );
    d->familyEdit->setReadOnly( true );
    d->familyList = new TQListBox( this, "font family II" );
    d->familyEdit->setFocusProxy( d->familyList );

    d->familyAccel
	= new TQLabel( d->familyList, tr("&Font"), this, "family accelerator" );
    d->familyAccel->setIndent( 2 );

    d->styleEdit = new TQLineEdit( this, "font style I" );
    d->styleEdit->setReadOnly( true );
    d->styleList = new TQListBox( this, "font style II" );
    d->styleEdit->setFocusProxy( d->styleList );

    d->styleAccel
	= new TQLabel( d->styleList, tr("Font st&yle"), this, "style accelerator" );
    d->styleAccel->setIndent( 2 );

    d->sizeEdit = new TQLineEdit( this, "font size I" );
    d->sizeEdit->setFocusPolicy( ClickFocus );
    TQIntValidator *validator = new TQIntValidator( 1, 512, this );
    d->sizeEdit->setValidator( validator );
    d->sizeList = new TQListBox( this, "font size II" );

    d->sizeAccel
	= new TQLabel ( d->sizeEdit, tr("&Size"), this, "size accelerator" );
    d->sizeAccel->setIndent( 2 );

    // effects box
    d->effects = new TQVGroupBox( tr("Effects"), this, "font effects" );
    d->strikeout = new TQCheckBox( d->effects, "strikeout on/off" );
    d->strikeout->setText( tr("Stri&keout") );
    d->underline = new TQCheckBox( d->effects, "underline on/off" );
    d->underline->setText( tr("&Underline") );

    d->sample = new TQHGroupBox( tr("Sample"), this, "sample text" );
    d->sampleEdit = new TQLineEdit( d->sample, "r/w sample text" );
    d->sampleEdit->setSizePolicy( TQSizePolicy( TQSizePolicy::Ignored, TQSizePolicy::Ignored) );
    d->sampleEdit->setAlignment( AlignCenter );
    // Note that the sample text is *not* translated with tr(), as the
    // characters used depend on the charset encoding.
    d->sampleEdit->setText( "AaBbYyZz" );

    d->scriptCombo = new TQComboBox( false, this, "font encoding" );

    d->scriptAccel
	= new TQLabel( d->scriptCombo, tr("Scr&ipt"), this,"encoding label");
    d->scriptAccel->setIndent( 2 );

    d->size = 0;
    d->smoothScalable = false;

    connect( d->scriptCombo, TQ_SIGNAL(activated(int)),
	     TQ_SLOT(scriptHighlighted(int)) );
    connect( d->familyList, TQ_SIGNAL(highlighted(int)),
	     TQ_SLOT(familyHighlighted(int)) );
    connect( d->styleList, TQ_SIGNAL(highlighted(int)),
	     TQ_SLOT(styleHighlighted(int)) );
    connect( d->sizeList, TQ_SIGNAL(highlighted(const TQString&)),
	     TQ_SLOT(sizeHighlighted(const TQString&)) );
    connect( d->sizeEdit, TQ_SIGNAL(textChanged(const TQString&)),
	     TQ_SLOT(sizeChanged(const TQString&)) );

    connect( d->strikeout, TQ_SIGNAL(clicked()),
	     TQ_SLOT(updateSample()) );
    connect( d->underline, TQ_SIGNAL(clicked()),
	     TQ_SLOT(updateSample()) );

    (void)d->familyList->sizeHint();
    (void)d->styleList->sizeHint();
    (void)d->sizeList->sizeHint();

    for (int i = 0; i < TQFont::NScripts; i++) {
	TQString scriptname = TQFontDatabase::scriptName((TQFont::Script) i);
	if ( !scriptname.isEmpty() )
	    d->scriptCombo->insertItem( scriptname );
    }

    updateFamilies();
    if ( d->familyList->count() != 0 )
	d->familyList->setCurrentItem( 0 );

    // grid layout
    TQGridLayout * mainGrid = new TQGridLayout( this, 9, 6, 12, 0 );

    mainGrid->addWidget( d->familyAccel, 0, 0 );
    mainGrid->addWidget( d->familyEdit, 1, 0 );
    mainGrid->addWidget( d->familyList, 2, 0 );

    mainGrid->addWidget( d->styleAccel, 0, 2 );
    mainGrid->addWidget( d->styleEdit, 1, 2 );
    mainGrid->addWidget( d->styleList, 2, 2 );

    mainGrid->addWidget( d->sizeAccel, 0, 4 );
    mainGrid->addWidget( d->sizeEdit, 1, 4 );
    mainGrid->addWidget( d->sizeList, 2, 4 );

    mainGrid->setColStretch( 0, 38 );
    mainGrid->setColStretch( 2, 24 );
    mainGrid->setColStretch( 4, 10 );

    mainGrid->addColSpacing( 1, 6 );
    mainGrid->addColSpacing( 3, 6 );
    mainGrid->addColSpacing( 5, 6 );

    mainGrid->addRowSpacing( 3, 12 );

    mainGrid->addWidget( d->effects, 4, 0 );

    mainGrid->addMultiCellWidget( d->sample, 4, 7, 2, 4 );

    mainGrid->addWidget( d->scriptAccel, 5, 0 );
    mainGrid->addRowSpacing( 6, 2 );
    mainGrid->addWidget( d->scriptCombo, 7, 0 );

    mainGrid->addRowSpacing( 8, 12 );

    TQHBoxLayout *buttonBox = new TQHBoxLayout;
    mainGrid->addMultiCell( buttonBox, 9, 9, 0, 4 );

    buttonBox->addStretch( 1 );
    TQString okt = modal ? tr("OK") : tr("Apply");
    d->ok = new TQPushButton( okt, this, "accept font selection" );
    buttonBox->addWidget( d->ok );
    if ( modal )
	connect( d->ok, TQ_SIGNAL(clicked()), TQ_SLOT(accept()) );
    d->ok->setDefault( true );

    buttonBox->addSpacing( 12 );

    TQString cancelt = modal ? tr("Cancel") : tr("Close");
    d->cancel = new TQPushButton( cancelt, this, "cancel/close" );
    buttonBox->addWidget( d->cancel );
    connect( d->cancel, TQ_SIGNAL(clicked()), TQ_SLOT(reject()) );

    resize( 500, 360 );

    d->sizeEdit->installEventFilter( this );
    d->familyList->installEventFilter( this );
    d->styleList->installEventFilter( this );
    d->sizeList->installEventFilter( this );

    d->familyList->setFocus();
}

/*!
  \internal
 Destroys the font dialog and frees up its storage.
*/

TQFontDialog::~TQFontDialog()
{
    delete d;
    d = 0;
}

/*!
  Executes a modal font dialog and returns a font.

  If the user clicks OK, the selected font is returned. If the user
  clicks Cancel, the \a initial font is returned.

  The dialog is called \a name, with the parent \a parent.
  \a initial is the initially selected font.
  If the \a ok parameter is not-null, \e *\a ok is set to true if the
  user clicked OK, and set to false if the user clicked Cancel.

  This static function is less flexible than the full TQFontDialog
  object, but is convenient and easy to use.

  Examples:
  \code
    bool ok;
    TQFont font = TQFontDialog::getFont( &ok, TQFont( "Times", 12 ), this );
    if ( ok ) {
	// font is set to the font the user selected
    } else {
	// the user canceled the dialog; font is set to the initial
	// value, in this case Times, 12.
    }
  \endcode

    The dialog can also be used to set a widget's font directly:
  \code
    myWidget.setFont( TQFontDialog::getFont( 0, myWidget.font() ) );
  \endcode
  In this example, if the user clicks OK the font they chose will be
  used, and if they click Cancel the original font is used.
*/
TQFont TQFontDialog::getFont( bool *ok, const TQFont &initial,
			    TQWidget *parent, const char* name)
{
    return getFont( ok, &initial, parent, name );
}

/*!
    \overload

  Executes a modal font dialog and returns a font.

  If the user clicks OK, the selected font is returned. If the user
  clicks Cancel, the TQt default font is returned.

  The dialog is called \a name, with parent \a parent.
  If the \a ok parameter is not-null, \e *\a ok is set to true if the
  user clicked OK, and false if the user clicked Cancel.

  This static function is less functional than the full TQFontDialog
  object, but is convenient and easy to use.

  Example:
  \code
    bool ok;
    TQFont font = TQFontDialog::getFont( &ok, this );
    if ( ok ) {
	// font is set to the font the user selected
    } else {
	// the user canceled the dialog; font is set to the default
	// application font, TQApplication::font()
    }
  \endcode

*/
TQFont TQFontDialog::getFont( bool *ok, TQWidget *parent,const char* name)
{
    return getFont( ok, 0, parent, name );
}

extern bool tqt_use_native_dialogs;

TQFont TQFontDialog::getFont( bool *ok, const TQFont *def,
			    TQWidget *parent, const char* name)
{
#if defined(TQ_WS_X11)
    if ( tqt_use_native_dialogs && TQTDEIntegration::enabled())
        return TQTDEIntegration::getFont( ok, def, parent, name );
#endif
    TQFont result;
    if ( def )
	result = *def;

    TQFontDialog *dlg = new TQFontDialog( parent, name, true );

    dlg->setFont( ( def ? *def : TQFont() ) );
#ifndef TQT_NO_WIDGET_TOPEXTRA
    dlg->setCaption( tr("Select Font") );
#endif

    bool res = (dlg->exec() == TQDialog::Accepted);
    if ( res )
	result = dlg->font();
    if ( ok )
	*ok = res;
    delete dlg;
    return result;
}


/*!
    \internal
    An event filter to make the Up, Down, PageUp and PageDown keys work
    correctly in the line edits. The source of the event is the object
    \a o and the event is \a e.
*/

bool TQFontDialog::eventFilter( TQObject * o , TQEvent * e )
{
    if ( e->type() == TQEvent::KeyPress) {
	TQKeyEvent * k = (TQKeyEvent *)e;
	if ( o == d->sizeEdit &&
        (k->key() == Key_Up ||
	     k->key() == Key_Down ||
         k->key() == Key_Prior ||
         k->key() == Key_Next) ) {

	    int ci = d->sizeList->currentItem();
	    (void)TQApplication::sendEvent( d->sizeList, k );

	    if ( ci != d->sizeList->currentItem() &&
		style().styleHint(TQStyle::SH_FontDialog_SelectAssociatedText, this))
		d->sizeEdit->selectAll();
	    return true;
	} else if ( ( o == d->familyList || o == d->styleList ) &&
	            ( k->key() == Key_Return || k->key() == Key_Enter) ) {
	    k->accept();
        accept();
	    return true;
	}
    } else if ( e->type() == TQEvent::FocusIn &&
		style().styleHint(TQStyle::SH_FontDialog_SelectAssociatedText, this) ) {
	if ( o == d->familyList )
	    d->familyEdit->selectAll();
	else if ( o == d->styleList )
	    d->styleEdit->selectAll();
	else if ( o == d->sizeList )
	    d->sizeEdit->selectAll();
    } else if ( e->type() == TQEvent::MouseButtonPress && o == d->sizeList ) {
	    d->sizeEdit->setFocus();
    }
    return TQDialog::eventFilter( o, e );
}

#ifdef TQ_WS_MAC
// #define SHOW_FONTS_IN_FAMILIES
#endif

#ifdef SHOW_FONTS_IN_FAMILIES
#include "ntqpainter.h"
#include <sizeedit.h>

class TQListBoxFontText : public TQListBoxText
{
    TQFont cfont;
public:
    TQListBoxFontText( const TQString & text );
    ~TQListBoxFontText() { }

    int	 height( const TQListBox * ) const;
    int	 width( const TQListBox * )  const;

protected:
    void  paint( TQPainter * );
};

TQListBoxFontText::TQListBoxFontText( const TQString & text )
    : TQListBoxText(text), cfont(text)
{
}

int TQListBoxFontText::height( const TQListBox * ) const
{
    TQFontMetrics fm(cfont);
    return TQMAX( fm.lineSpacing() + 2, TQApplication::globalStrut().height() );
}

int TQListBoxFontText::width( const TQListBox * )  const
{
    TQFontMetrics fm(cfont);
    return TQMAX( fm.width( text() ) + 6, TQApplication::globalStrut().width() );
}

void TQListBoxFontText::paint( TQPainter *painter )
{
    painter->save();
    painter->setFont(cfont);
    TQListBoxText::paint(painter);
    painter->restore();
}

#endif

/*!
  \internal
    Updates the contents of the "font family" list box. This
  function can be reimplemented if you have special requirements.
*/

void TQFontDialog::updateFamilies()
{
    d->familyList->blockSignals( true );

    enum match_t { MATCH_NONE=0, MATCH_LAST_RESORT=1, MATCH_APP=2, MATCH_FALLBACK, MATCH_FAMILY=3 };

    TQStringList familyNames = d->fdb.families(d->script);
    {
	// merge the unicode/unknown family list with the above list.
	TQStringList l = d->fdb.families(TQFont::Unicode) +
			d->fdb.families(TQFont::UnknownScript);
	TQStringList::ConstIterator it = l.begin(), end = l.end();
	for (; it != end; ++it) {
	    if (! familyNames.contains(*it))
		familyNames << *it;
	}
    }

    familyNames.sort();

    d->familyList->clear();
#ifdef SHOW_FONTS_IN_FAMILIES
    TQStringList::Iterator it = familyNames.begin();
    int idx = 0;
    for( ; it != familyNames.end() ; ++it )
	d->familyList->insertItem(new TQListBoxFontText(*it), idx++);
#else
    d->familyList->insertStringList( familyNames );
#endif

    TQString foundryName1, familyName1, foundryName2, familyName2;
    int bestFamilyMatch = -1;
    match_t bestFamilyType = MATCH_NONE;

    TQFont f;

    // ##### do the right thing for a list of family names in the font.
    TQFontDatabase::parseFontName(d->family, foundryName1, familyName1);

    TQStringList::Iterator it = familyNames.begin();
    int i = 0;
    for( ; it != familyNames.end(); ++it, ++i ) {

	TQFontDatabase::parseFontName(*it, foundryName2, familyName2);

	//try to match..
	if ( familyName1 == familyName2 ) {
	    bestFamilyType = MATCH_FAMILY;
	    if ( foundryName1 == foundryName2 ) {
		bestFamilyMatch = i;
		break;
	    }
	    if ( bestFamilyMatch < MATCH_FAMILY )
		bestFamilyMatch = i;
	}

	//and try some fall backs
	match_t type = MATCH_NONE;
	if ( bestFamilyType <= MATCH_NONE && familyName2 == f.lastResortFamily() )
		type = MATCH_LAST_RESORT;
	if ( bestFamilyType <= MATCH_LAST_RESORT && familyName2 == f.family() )
		type = MATCH_APP;
	// ### add fallback for script
	if ( type != MATCH_NONE ) {
	    bestFamilyType = type;
	    bestFamilyMatch = i;
	}
    }

    if (i != -1 && bestFamilyType != MATCH_NONE)
	d->familyList->setCurrentItem(bestFamilyMatch);
    else
	d->familyList->setCurrentItem( 0 );
    d->familyEdit->setText( d->familyList->currentText() );
    if ( style().styleHint(TQStyle::SH_FontDialog_SelectAssociatedText, this) &&
	 d->familyList->hasFocus() )
	d->familyEdit->selectAll();

    d->familyList->blockSignals( false );
    updateStyles();
}

/*!
    \internal
    Updates the contents of the "font style" list box. This
  function can be reimplemented if you have special requirements.
*/

void TQFontDialog::updateStyles()
{
    d->styleList->blockSignals( true );

    d->styleList->clear();

    TQStringList styles = d->fdb.styles( d->familyList->currentText() );

    if ( styles.isEmpty() ) {
	d->styleEdit->clear();
	d->smoothScalable = false;
    } else {
	d->styleList->insertStringList( styles );

	if ( !d->style.isEmpty() ) {
	    bool found = false;
	    bool first = true;
	    TQString cstyle = d->style;
	redo:
	    for ( int i = 0 ; i < (int)d->styleList->count() ; i++ ) {
		if ( cstyle == d->styleList->text(i) ) {
		    d->styleList->setCurrentItem( i );
		    found = true;
		    break;
		}
	    }
	    if (!found && first) {
		if (cstyle.contains("Italic")) {
		    cstyle.replace("Italic", "Oblique");
		    first = false;
		    goto redo;
		} else if (cstyle.contains("Oblique")) {
		    cstyle.replace("Oblique", "Italic");
		    first = false;
		    goto redo;
		}
	    }
	    if ( !found )
		d->styleList->setCurrentItem( 0 );
	}

	d->styleEdit->setText( d->styleList->currentText() );
	if ( style().styleHint(TQStyle::SH_FontDialog_SelectAssociatedText, this) &&
	     d->styleList->hasFocus() )
	    d->styleEdit->selectAll();

	d->smoothScalable = d->fdb.isSmoothlyScalable( d->familyList->currentText(), d->styleList->currentText() );
    }

    d->styleList->blockSignals( false );

    updateSizes();
}

/*!
    \internal
    Updates the contents of the "font size" list box. This
  function can be reimplemented if you have special requirements.
*/

void TQFontDialog::updateSizes()
{
    d->sizeList->blockSignals( true );

    d->sizeList->clear();

    if ( !d->familyList->currentText().isEmpty() ) {
	TQValueList<int> sizes = d->fdb.pointSizes( d->familyList->currentText(), d->styleList->currentText() );

	int i = 0;
	bool found = false;
	for( TQValueList<int>::iterator it = sizes.begin() ; it != sizes.end(); ++it ) {
	    d->sizeList->insertItem( TQString::number( *it ) );
	    if ( !found && *it >= d->size ) {
		d->sizeList->setCurrentItem( i );
		found = true;
	    }
	    ++i;
	}
	if ( !found ) {
	    // we request a size bigger than the ones in the list, select the biggest one
	    d->sizeList->setCurrentItem( d->sizeList->count() - 1 );
	}

	d->sizeEdit->blockSignals( true );
	d->sizeEdit->setText( ( d->smoothScalable ? TQString::number( d->size ) : d->sizeList->currentText() ) );
	if ( style().styleHint(TQStyle::SH_FontDialog_SelectAssociatedText, this) &&
	     d->sizeList->hasFocus() )
	    d->sizeEdit->selectAll();
	d->sizeEdit->blockSignals( false );
    } else {
	d->sizeEdit->clear();
    }

    d->sizeList->blockSignals( false );
    updateSample();
}

void TQFontDialog::updateSample()
{
    if ( d->familyList->currentText().isEmpty() )
	d->sampleEdit->clear();
    else
	d->sampleEdit->setFont( font() );
}

/*!
    \internal
*/
void TQFontDialog::scriptHighlighted( int index )
{
    d->script = (TQFont::Script)index;
    d->sampleEdit->setText( d->fdb.scriptSample( d->script ) );
    updateFamilies();
}

/*!
    \internal
*/
void TQFontDialog::familyHighlighted( int i )
{
    d->family = d->familyList->text( i );
    d->familyEdit->setText( d->family );
    if ( style().styleHint(TQStyle::SH_FontDialog_SelectAssociatedText, this) &&
	 d->familyList->hasFocus() )
	d->familyEdit->selectAll();

    updateStyles();
}


/*!
    \internal
*/

void TQFontDialog::styleHighlighted( int index )
{
    TQString s = d->styleList->text( index );
    d->styleEdit->setText( s );
    if ( style().styleHint(TQStyle::SH_FontDialog_SelectAssociatedText, this) &&
	 d->styleList->hasFocus() )
	d->styleEdit->selectAll();

    d->style = s;

    updateSizes();
}


/*!
    \internal
*/

void TQFontDialog::sizeHighlighted( const TQString &s )
{
    d->sizeEdit->setText( s );
    if ( style().styleHint(TQStyle::SH_FontDialog_SelectAssociatedText, this) &&
	 d->sizeEdit->hasFocus() )
	d->sizeEdit->selectAll();

    d->size = s.toInt();
    updateSample();
}

/*!
    \internal
    This slot is called if the user changes the font size.
    The size is passed in the \a s argument as a \e string.
*/

void TQFontDialog::sizeChanged( const TQString &s )
{
    // no need to check if the conversion is valid, since we have an TQIntValidator in the size edit
    int size = s.toInt();
    if ( d->size == size )
	return;

    d->size = size;
    if ( d->sizeList->count() != 0 ) {
	int i;
	for ( i = 0 ; i < (int)d->sizeList->count() - 1 ; i++ ) {
	    if ( d->sizeList->text(i).toInt() >= d->size )
		break;
	}
	d->sizeList->blockSignals( true );
	d->sizeList->setCurrentItem( i );
	d->sizeList->blockSignals( false );
    }
    updateSample();
}

/*!
  \internal
  Sets the font highlighted in the TQFontDialog to font \a f.

  \sa font()
*/

void TQFontDialog::setFont( const TQFont &f )
{
    d->family = f.family();
    d->style = d->fdb.styleString( f );
    d->size = f.pointSize();
    if ( d->size == -1 ) {
	    TQFontInfo fi( f );
	    d->size = fi.pointSize();
    }
    d->strikeout->setChecked( f.strikeOut() );
    d->underline->setChecked( f.underline() );

    updateFamilies();
}

/*!
  \internal
  Returns the font which the user has chosen.

  \sa setFont()
*/

TQFont TQFontDialog::font() const
{
    int pSize = d->sizeEdit->text().toInt();

    TQFont f = d->fdb.font( d->familyList->currentText(), d->style, pSize );
    f.setStrikeOut( d->strikeout->isChecked() );
    f.setUnderline( d->underline->isChecked() );
    return f;
}

#endif
