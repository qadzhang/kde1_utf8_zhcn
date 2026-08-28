/****************************************************************************
**
** Definition of TQSpinBox widget class
**
** Created : 970101
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

#ifndef TQSPINBOX_H
#define TQSPINBOX_H

#ifndef QT_H
#include "ntqwidget.h"
#include "ntqrangecontrol.h"
#endif // QT_H

#ifndef TQT_NO_SPINBOX

class TQLineEdit;
class TQValidator;
class TQSpinBoxPrivate;

class TQ_EXPORT TQSpinBox: public TQWidget, public TQRangeControl
{
    TQ_OBJECT
    TQ_ENUMS( ButtonSymbols )
    TQ_PROPERTY( TQString text READ text )
    TQ_PROPERTY( TQString prefix READ prefix WRITE setPrefix )
    TQ_PROPERTY( TQString suffix READ suffix WRITE setSuffix )
    TQ_PROPERTY( TQString cleanText READ cleanText )
    TQ_PROPERTY( TQString specialValueText READ specialValueText WRITE setSpecialValueText )
    TQ_PROPERTY( bool wrapping READ wrapping WRITE setWrapping )
    TQ_PROPERTY( ButtonSymbols buttonSymbols READ buttonSymbols WRITE setButtonSymbols )
    TQ_PROPERTY( int maxValue READ maxValue WRITE setMaxValue )
    TQ_PROPERTY( int minValue READ minValue WRITE setMinValue )
    TQ_PROPERTY( int lineStep READ lineStep WRITE setLineStep )
    TQ_PROPERTY( int value READ value WRITE setValue )

public:
    TQSpinBox( TQWidget* parent=0, const char* name=0 );
    TQSpinBox( int minValue, int maxValue, int step = 1,
	      TQWidget* parent=0, const char* name=0 );
    ~TQSpinBox();

    TQString		text() const;

    virtual TQString	prefix() const;
    virtual TQString	suffix() const;
    virtual TQString	cleanText() const;

    virtual void	setSpecialValueText( const TQString &text );
    TQString		specialValueText() const;

    virtual void	setWrapping( bool on );
    bool		wrapping() const;

    enum ButtonSymbols { UpDownArrows, PlusMinus };
    virtual void	setButtonSymbols( ButtonSymbols );
    ButtonSymbols	buttonSymbols() const;

    virtual void	setValidator( const TQValidator* v );
    const TQValidator * validator() const;

    TQSize		sizeHint() const;
    TQSize		minimumSizeHint() const;

    int	 minValue() const;
    int	 maxValue() const;
    void setMinValue( int );
    void setMaxValue( int );
    int	 lineStep() const;
    void setLineStep( int );
    int  value() const;

    TQRect		upRect() const;
    TQRect		downRect() const;

public slots:
    virtual void	setValue( int value );
    virtual void	setPrefix( const TQString &text );
    virtual void	setSuffix( const TQString &text );
    virtual void	stepUp();
    virtual void	stepDown();
    virtual void 	setEnabled( bool enabled );
    virtual void 	selectAll();

signals:
    void		valueChanged( int value );
    void		valueChanged( const TQString &valueText );

protected:
    virtual TQString	mapValueToText( int value );
    virtual int		mapTextToValue( bool* ok );
    TQString		currentValueText();

    virtual void	updateDisplay();
    virtual void	interpretText();

    TQLineEdit*		editor() const;

    virtual void	valueChange();
    virtual void	rangeChange();

    bool		eventFilter( TQObject* obj, TQEvent* ev );
    void		resizeEvent( TQResizeEvent* ev );
#ifndef TQT_NO_WHEELEVENT
    void		wheelEvent( TQWheelEvent * );
#endif
    void		leaveEvent( TQEvent* );

    void		styleChange( TQStyle& );

protected slots:
    void		textChanged();

private:
    void initSpinBox();
    TQSpinBoxPrivate* d;
    TQLineEdit* vi;
    TQValidator* validate;
    TQString pfix;
    TQString sfix;
    TQString specText;

    uint wrap		: 1;
    uint edited		: 1;

    void arrangeWidgets();

private:	// Disabled copy constructor and operator=
#if defined(TQ_DISABLE_COPY)
    TQSpinBox( const TQSpinBox& );
    TQSpinBox& operator=( const TQSpinBox& );
#endif

};

#endif // TQT_NO_SPINBOX

#endif // TQSPINBOX_H
