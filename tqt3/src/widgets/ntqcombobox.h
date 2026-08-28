/**********************************************************************
**
** Definition of TQComboBox class
**
** Created : 950426
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

#ifndef TQCOMBOBOX_H
#define TQCOMBOBOX_H

#ifndef QT_H
#include "ntqwidget.h"
#endif // QT_H

#ifndef TQT_NO_COMBOBOX


class TQStrList;
class TQStringList;
class TQLineEdit;
class TQValidator;
class TQListBox;
class TQComboBoxData;
class TQWheelEvent;

class TQ_EXPORT TQComboBox : public TQWidget
{
    TQ_OBJECT
    TQ_ENUMS( Policy )
    TQ_PROPERTY( bool editable READ editable WRITE setEditable )
    TQ_PROPERTY( int count READ count )
    TQ_PROPERTY( TQString currentText READ currentText WRITE setCurrentText DESIGNABLE false )
    TQ_PROPERTY( int currentItem READ currentItem WRITE setCurrentItem )
    TQ_PROPERTY( bool autoResize READ autoResize WRITE setAutoResize DESIGNABLE false )
    TQ_PROPERTY( int sizeLimit READ sizeLimit WRITE setSizeLimit )
    TQ_PROPERTY( int maxCount READ maxCount WRITE setMaxCount )
    TQ_PROPERTY( Policy insertionPolicy READ insertionPolicy WRITE setInsertionPolicy )
    TQ_PROPERTY( bool autoCompletion READ autoCompletion WRITE setAutoCompletion )
    TQ_PROPERTY( bool duplicatesEnabled READ duplicatesEnabled WRITE setDuplicatesEnabled )
    TQ_OVERRIDE( bool autoMask DESIGNABLE true SCRIPTABLE true )

public:
    TQComboBox( TQWidget* parent=0, const char* name=0 );
    TQComboBox( bool rw, TQWidget* parent=0, const char* name=0 );
    ~TQComboBox();

    int		count() const;

    void	insertStringList( const TQStringList &, int index=-1 );
    void	insertStrList( const TQStrList &, int index=-1 );
    void	insertStrList( const TQStrList *, int index=-1 );
    void	insertStrList( const char **, int numStrings=-1, int index=-1);

    void	insertItem( const TQString &text, int index=-1 );
    void	insertItem( const TQPixmap &pixmap, int index=-1 );
    void	insertItem( const TQPixmap &pixmap, const TQString &text, int index=-1 );

    void	removeItem( int index );

    int		currentItem() const;
    virtual void setCurrentItem( int index );

    TQString 	currentText() const;
    virtual void setCurrentText( const TQString& );

    TQString 	text( int index ) const;
    const TQPixmap *pixmap( int index ) const;

    void	changeItem( const TQString &text, int index );
    void	changeItem( const TQPixmap &pixmap, int index );
    void	changeItem( const TQPixmap &pixmap, const TQString &text, int index );

    bool	autoResize()	const;
    virtual void setAutoResize( bool );
    TQSize	sizeHint() const;

    void	setPalette( const TQPalette & );
    void	setFont( const TQFont & );
    void	setEnabled( bool );

    virtual void setSizeLimit( int );
    int		sizeLimit() const;

    virtual void setMaxCount( int );
    int		maxCount() const;

    enum Policy { NoInsertion, AtTop, AtCurrent, AtBottom,
		  AfterCurrent, BeforeCurrent };

    virtual void setInsertionPolicy( Policy policy );
    Policy	insertionPolicy() const;

    virtual void setValidator( const TQValidator * );
    const TQValidator * validator() const;

    virtual void setListBox( TQListBox * );
    TQListBox *	listBox() const;

    virtual void setLineEdit( TQLineEdit *edit );
    TQLineEdit*	lineEdit() const;

    virtual void setAutoCompletion( bool );
    bool	autoCompletion() const;

    bool	eventFilter( TQObject *object, TQEvent *event );

    void	setDuplicatesEnabled( bool enable );
    bool	duplicatesEnabled() const;

    bool	editable() const;
    void	setEditable( bool );

    virtual void popup();

    void	hide();

public slots:
    void	clear();
    void	clearValidator();
    void	clearEdit();
    virtual void setEditText( const TQString &);

signals:
    void	activated( int index );
    void	highlighted( int index );
    void	activated( const TQString &);
    void	highlighted( const TQString &);
    void	textChanged( const TQString &);

private slots:
    void	internalActivate( int );
    void	internalHighlight( int );
    void	internalClickTimeout();
    void	returnPressed();

protected:
    void	paintEvent( TQPaintEvent * );
    void	resizeEvent( TQResizeEvent * );
    void	mousePressEvent( TQMouseEvent * );
    void	mouseMoveEvent( TQMouseEvent * );
    void	mouseReleaseEvent( TQMouseEvent * );
    void	mouseDoubleClickEvent( TQMouseEvent * );
    void	keyPressEvent( TQKeyEvent *e );
    void	focusInEvent( TQFocusEvent *e );
    void	focusOutEvent( TQFocusEvent *e );
#ifndef TQT_NO_WHEELEVENT
    void	wheelEvent( TQWheelEvent *e );
#endif
    void	styleChange( TQStyle& );

    void	updateMask();

private:
    void	setUpListBox();
    void	setUpLineEdit();
    void	popDownListBox();
    void	reIndex();
    void	currentChanged();
    int		completionIndex( const TQString &, int ) const;

    TQComboBoxData	*d;

private:	// Disabled copy constructor and operator=
#if defined(TQ_DISABLE_COPY)
    TQComboBox( const TQComboBox & );
    TQComboBox &operator=( const TQComboBox & );
#endif
};


#endif // TQT_NO_COMBOBOX

#endif // TQCOMBOBOX_H
