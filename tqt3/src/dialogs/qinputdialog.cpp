/****************************************************************************
**
** Implementation of TQInputDialog class
**
** Created : 991212
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

#include "ntqinputdialog.h"

#ifndef TQT_NO_INPUTDIALOG

#include "ntqlayout.h"
#include "ntqlabel.h"
#include "ntqlineedit.h"
#include "ntqpushbutton.h"
#include "ntqspinbox.h"
#include "ntqcombobox.h"
#include "ntqwidgetstack.h"
#include "ntqvalidator.h"
#include "ntqapplication.h"

class TQInputDialogPrivate
{
public:
    friend class TQInputDialog;
    TQLabel *label;
    TQLineEdit *lineEdit;
    TQSpinBox *spinBox;
    TQComboBox *comboBox, *editComboBox;
    TQPushButton *ok;
    TQWidgetStack *stack;
    TQInputDialog::Type type;
};

/*!
    \class TQInputDialog
    \brief The TQInputDialog class provides a simple convenience dialog to get a single value from the user.
    \ingroup dialogs
    \mainclass

    The input value can be a string, a number or an item from a list. A
    label must be set to tell the user what they should enter.

    Four static convenience functions are provided:
    getText(), getInteger(), getDouble() and getItem(). All the
    functions can be used in a similar way, for example:
    \code
    bool ok;
    TQString text = TQInputDialog::getText(
	    "MyApp 3000", "Enter your name:", TQLineEdit::Normal,
	    TQString::null, &ok, this );
    if ( ok && !text.isEmpty() ) {
	// user entered something and pressed OK
    } else {
	// user entered nothing or pressed Cancel
    }
    \endcode

    \img inputdialogs.png Input Dialogs
*/

/*!
  \enum TQInputDialog::Type

  This enum specifies the type of the dialog, i.e. what kind of data you
  want the user to input:

  \value LineEdit  A TQLineEdit is used for obtaining string or numeric
  input. The TQLineEdit can be accessed using lineEdit().

  \value SpinBox  A TQSpinBox is used for obtaining integer input.
  Use spinBox() to access the TQSpinBox.

  \value ComboBox  A read-only TQComboBox is used to provide a fixed
  list of choices from which the user can choose.
  Use comboBox() to access the TQComboBox.

  \value EditableComboBox  An editable TQComboBox is used to provide a fixed
  list of choices from which the user can choose, but which also
  allows the user to enter their own value instead.
  Use editableComboBox() to access the TQComboBox.
*/

/*!
  Constructs the dialog. The \a label is the text which is shown to the user
  (it should tell the user what they are expected to enter). The \a parent
  is the dialog's parent widget. The widget is called \a name. If \a
  modal is true (the default) the dialog will be modal. The \a type
  parameter is used to specify which type of dialog to construct.

  \sa getText(), getInteger(), getDouble(), getItem()
*/

TQInputDialog::TQInputDialog( const TQString &label, TQWidget* parent,
			    const char* name, bool modal, Type type )
    : TQDialog( parent, name, modal )
{
    d = new TQInputDialogPrivate;
    d->lineEdit = 0;
    d->spinBox = 0;
    d->comboBox = 0;

    TQVBoxLayout *vbox = new TQVBoxLayout( this, 6, 6 );

    d->label = new TQLabel( label, this, "qt_inputdlg_lbl" );
    vbox->addWidget( d->label );

    d->stack = new TQWidgetStack( this, "qt_inputdlg_ws" );
    vbox->addWidget( d->stack );
    d->lineEdit = new TQLineEdit( d->stack, "qt_inputdlg_le" );
    d->spinBox = new TQSpinBox( d->stack, "qt_inputdlg_sb" );
    d->comboBox = new TQComboBox( false, d->stack, "qt_inputdlg_cb" );
    d->editComboBox = new TQComboBox( true, d->stack, "qt_inputdlg_editcb" );

    TQHBoxLayout *hbox = new TQHBoxLayout( 6 );
    vbox->addLayout( hbox, AlignRight );

    d->ok = new TQPushButton( tr( "OK" ), this, "qt_ok_btn" );
    d->ok->setDefault( true );
    TQPushButton *cancel = new TQPushButton( tr( "Cancel" ), this, "qt_cancel_btn" );

    TQSize bs = d->ok->sizeHint().expandedTo( cancel->sizeHint() );
    d->ok->setFixedSize( bs );
    cancel->setFixedSize( bs );

    hbox->addStretch();
    hbox->addWidget( d->ok );
    hbox->addWidget( cancel );

    connect( d->lineEdit, TQ_SIGNAL( returnPressed() ),
	     this, TQ_SLOT( tryAccept() ) );
    connect( d->lineEdit, TQ_SIGNAL( textChanged(const TQString&) ),
	     this, TQ_SLOT( textChanged(const TQString&) ) );

    connect( d->ok, TQ_SIGNAL( clicked() ), this, TQ_SLOT( accept() ) );
    connect( cancel, TQ_SIGNAL( clicked() ), this, TQ_SLOT( reject() ) );

    TQSize sh = sizeHint().expandedTo( TQSize(400, 10) );
    setType( type );
    resize( sh.width(), vbox->heightForWidth(sh.width()) );
}

/*!
  Returns the line edit which is used in LineEdit mode.
*/

TQLineEdit *TQInputDialog::lineEdit() const
{
    return d->lineEdit;
}

/*!
  Returns the spinbox which is used in SpinBox mode.
*/

TQSpinBox *TQInputDialog::spinBox() const
{
    return d->spinBox;
}

/*!
  Returns the combobox which is used in ComboBox mode.
*/

TQComboBox *TQInputDialog::comboBox() const
{
    return d->comboBox;
}

/*!
  Returns the combobox which is used in EditableComboBox mode.
*/

TQComboBox *TQInputDialog::editableComboBox() const
{
    return d->editComboBox;
}

/*!
  Sets the input type of the dialog to \a t.
*/

void TQInputDialog::setType( Type t )
{
    TQWidget *input = 0;
    switch ( t ) {
    case LineEdit:
	input = d->lineEdit;
	break;
    case SpinBox:
	input = d->spinBox;
	break;
    case ComboBox:
	input = d->comboBox;
	break;
    case EditableComboBox:
	input = d->editComboBox;
	break;
    default:
#if defined(QT_CHECK_STATE)
	tqWarning( "TQInputDialog::setType: Invalid type" );
#endif
	break;
    }
    if ( input ) {
	d->stack->raiseWidget( input );
	d->stack->setFixedHeight( input->sizeHint().height() );
	input->setFocus();
#ifndef TQT_NO_ACCEL
	d->label->setBuddy( input );
#endif
    }

    d->type = t;
}

/*!
  Returns the input type of the dialog.

  \sa setType()
*/

TQInputDialog::Type TQInputDialog::type() const
{
    return d->type;
}

/*!
  Destructor.
*/

TQInputDialog::~TQInputDialog()
{
    delete d;
}

/*!
    Static convenience function to get a string from the user. \a
    caption is the text which is displayed in the title bar of the
    dialog. \a label is the text which is shown to the user (it should
    say what should be entered). \a text is the default text which is
    placed in the line edit. The \a mode is the echo mode the line edit
    will use. If \a ok is not-null \e *\a ok will be set to true if the
    user pressed OK and to false if the user pressed Cancel. The
    dialog's parent is \a parent; the dialog is called \a name. The
    dialog will be modal.

    This function returns the text which has been entered in the line
    edit. It will not return an empty string.

    Use this static function like this:

    \code
    bool ok;
    TQString text = TQInputDialog::getText(
	    "MyApp 3000", "Enter your name:", TQLineEdit::Normal,
	    TQString::null, &ok, this );
    if ( ok && !text.isEmpty() ) {
	// user entered something and pressed OK
    } else {
	// user entered nothing or pressed Cancel
    }
    \endcode
*/

TQString TQInputDialog::getText( const TQString &caption, const TQString &label,
			       TQLineEdit::EchoMode mode, const TQString &text,
			       bool *ok, TQWidget *parent, const char *name )
{
    TQInputDialog *dlg = new TQInputDialog( label, parent,
					  name ? name : "qt_inputdlg_gettext",
					  true, LineEdit );

#ifndef TQT_NO_WIDGET_TOPEXTRA
    dlg->setCaption( caption );
#endif
    dlg->lineEdit()->setText( text );
    dlg->lineEdit()->setEchoMode( mode );

    bool ok_ = false;
    TQString result;
    ok_ = dlg->exec() == TQDialog::Accepted;
    if ( ok )
	*ok = ok_;
    if ( ok_ )
	result = dlg->lineEdit()->text();

    delete dlg;
    return result;
}

/*!
    Static convenience function to get an integer input from the
    user. \a caption is the text which is displayed in the title bar
    of the dialog.  \a label is the text which is shown to the user
    (it should say what should be entered). \a value is the default
    integer which the spinbox will be set to.  \a minValue and \a
    maxValue are the minimum and maximum values the user may choose,
    and \a step is the amount by which the values change as the user
    presses the arrow buttons to increment or decrement the value.

    If \a ok is not-null *\a ok will be set to true if the user
    pressed OK and to false if the user pressed Cancel. The dialog's
    parent is \a parent; the dialog is called \a name. The dialog will
    be modal.

    This function returns the integer which has been entered by the user.

    Use this static function like this:

    \code
    bool ok;
    int res = TQInputDialog::getInteger(
	    "MyApp 3000", "Enter a number:", 22, 0, 1000, 2,
	    &ok, this );
    if ( ok ) {
	// user entered something and pressed OK
    } else {
	// user pressed Cancel
    }
    \endcode
*/

int TQInputDialog::getInteger( const TQString &caption, const TQString &label,
			      int value, int minValue, int maxValue, int step, bool *ok,
			      TQWidget *parent, const char *name )
{
    TQInputDialog *dlg = new TQInputDialog( label, parent,
					  name ? name : "qt_inputdlg_getint",
					  true, SpinBox );
#ifndef TQT_NO_WIDGET_TOPEXTRA
    dlg->setCaption( caption );
#endif
    dlg->spinBox()->setRange( minValue, maxValue );
    dlg->spinBox()->setSteps( step, 0 );
    dlg->spinBox()->setValue( value );

    bool ok_ = false;
    int result;
    ok_ = dlg->exec() == TQDialog::Accepted;
    if ( ok )
	*ok = ok_;
    result = dlg->spinBox()->value();

    delete dlg;
    return result;
}

/*!
    Static convenience function to get a floating point number from
    the user. \a caption is the text which is displayed in the title
    bar of the dialog. \a label is the text which is shown to the user
    (it should say what should be entered). \a value is the default
    floating point number that the line edit will be set to. \a
    minValue and \a maxValue are the minimum and maximum values the
    user may choose, and \a decimals is the maximum number of decimal
    places the number may have.

    If \a ok is not-null \e *\a ok will be set to true if the user
    pressed OK and to false if the user pressed Cancel. The dialog's
    parent is \a parent; the dialog is called \a name. The dialog will
    be modal.

    This function returns the floating point number which has been
    entered by the user.

    Use this static function like this:

    \code
    bool ok;
    double res = TQInputDialog::getDouble(
	    "MyApp 3000", "Enter a decimal number:", 33.7, 0,
	    1000, 2, &ok, this );
    if ( ok ) {
	// user entered something and pressed OK
    } else {
	// user pressed Cancel
    }
    \endcode
*/

double TQInputDialog::getDouble( const TQString &caption, const TQString &label,
				double value, double minValue, double maxValue,
				int decimals, bool *ok, TQWidget *parent,
				const char *name )
{
    TQInputDialog dlg( label, parent,
		      name ? name : "qt_inputdlg_getdbl", true, LineEdit );
#ifndef TQT_NO_WIDGET_TOPEXTRA
    dlg.setCaption( caption );
#endif
    dlg.lineEdit()->setValidator( new TQDoubleValidator( minValue, maxValue, decimals, dlg.lineEdit() ) );
    dlg.lineEdit()->setText( TQString::number( value, 'f', decimals ) );
    dlg.lineEdit()->selectAll();

    bool accepted = ( dlg.exec() == TQDialog::Accepted );
    if ( ok )
	*ok = accepted;
    return dlg.lineEdit()->text().toDouble();
}

/*!
    Static convenience function to let the user select an item from a
    string list. \a caption is the text which is displayed in the title
    bar of the dialog. \a label is the text which is shown to the user (it
    should say what should be entered). \a list is the
    string list which is inserted into the combobox, and \a current is the number
    of the item which should be the current item. If \a editable is true
    the user can enter their own text; if \a editable is false the user
    may only select one of the existing items.

    If \a ok is not-null \e *\a ok will be set to true if the user
    pressed OK and to false if the user pressed Cancel. The dialog's
    parent is \a parent; the dialog is called \a name. The dialog will
    be modal.

    This function returns the text of the current item, or if \a
    editable is true, the current text of the combobox.

    Use this static function like this:

    \code
    TQStringList lst;
    lst << "First" << "Second" << "Third" << "Fourth" << "Fifth";
    bool ok;
    TQString res = TQInputDialog::getItem(
	    "MyApp 3000", "Select an item:", lst, 1, true, &ok,
	    this );
    if ( ok ) {
	// user selected an item and pressed OK
    } else {
	// user pressed Cancel
    }
    \endcode
*/

TQString TQInputDialog::getItem( const TQString &caption, const TQString &label, const TQStringList &list,
			       int current, bool editable,
			       bool *ok, TQWidget *parent, const char *name )
{
    TQInputDialog *dlg = new TQInputDialog( label, parent, name ? name : "qt_inputdlg_getitem", true, editable ? EditableComboBox : ComboBox );
#ifndef TQT_NO_WIDGET_TOPEXTRA
    dlg->setCaption( caption );
#endif
    if ( editable ) {
	dlg->editableComboBox()->insertStringList( list );
	dlg->editableComboBox()->setCurrentItem( current );
    } else {
	dlg->comboBox()->insertStringList( list );
	dlg->comboBox()->setCurrentItem( current );
    }

    bool ok_ = false;
    TQString result;
    ok_ = dlg->exec() == TQDialog::Accepted;
    if ( ok )
	*ok = ok_;
    if ( editable )
	result = dlg->editableComboBox()->currentText();
    else
	result = dlg->comboBox()->currentText();

    delete dlg;
    return result;
}

/*!
  \internal

  This slot is invoked when the text is changed; the new text is passed
  in \a s.
*/

void TQInputDialog::textChanged( const TQString &s )
{
    bool on = true;

    if ( d->lineEdit->validator() ) {
	TQString str = d->lineEdit->text();
	int index = d->lineEdit->cursorPosition();
	on = ( d->lineEdit->validator()->validate(str, index) ==
	       TQValidator::Acceptable );
    } else if ( type() != LineEdit ) {
	on = !s.isEmpty();
    }
    d->ok->setEnabled( on );
}

/*!
  \internal
*/

void TQInputDialog::tryAccept()
{
    if ( !d->lineEdit->text().isEmpty() )
	accept();
}

#endif
