/****************************************************************************
**
** Definition of TQRangeControl class
**
** Created : 940427
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

#ifndef TQRANGECONTROL_H
#define TQRANGECONTROL_H

#ifndef QT_H
#include "ntqglobal.h"
#include "ntqframe.h"
#endif // QT_H

#ifndef TQT_NO_RANGECONTROL


class TQRangeControlPrivate;


class TQ_EXPORT TQRangeControl
{
public:
    TQRangeControl();
    TQRangeControl( int minValue, int maxValue,
		   int lineStep, int pageStep, int value );
    virtual ~TQRangeControl();
    int		value()		const;
    void	setValue( int );
    void	addPage();
    void	subtractPage();
    void	addLine();
    void	subtractLine();

    int		minValue()	const;
    int		maxValue()	const;
    void	setRange( int minValue, int maxValue );
    void	setMinValue( int minVal );
    void	setMaxValue( int minVal );

    int		lineStep()	const;
    int		pageStep()	const;
    void	setSteps( int line, int page );

    int		bound( int ) const;

protected:
    int		positionFromValue( int val, int space ) const;
    int		valueFromPosition( int pos, int space ) const;
    void	directSetValue( int val );
    int		prevValue()	const;

    virtual void valueChange();
    virtual void rangeChange();
    virtual void stepChange();

private:
    int		minVal, maxVal;
    int		line, page;
    int		val, prevVal;

    TQRangeControlPrivate * d;

private:	// Disabled copy constructor and operator=
#if defined(TQ_DISABLE_COPY)
    TQRangeControl( const TQRangeControl & );
    TQRangeControl &operator=( const TQRangeControl & );
#endif
};


inline int TQRangeControl::value() const
{ return val; }

inline int TQRangeControl::prevValue() const
{ return prevVal; }

inline int TQRangeControl::minValue() const
{ return minVal; }

inline int TQRangeControl::maxValue() const
{ return maxVal; }

inline int TQRangeControl::lineStep() const
{ return line; }

inline int TQRangeControl::pageStep() const
{ return page; }


#endif // TQT_NO_RANGECONTROL

#ifndef TQT_NO_SPINWIDGET

class TQSpinWidgetPrivate;
class TQ_EXPORT TQSpinWidget : public TQWidget
{
    TQ_OBJECT
public:
    TQSpinWidget( TQWidget* parent=0, const char* name=0 );
    ~TQSpinWidget();

    void 	setEditWidget( TQWidget * widget );
    TQWidget * 	editWidget();

    TQRect upRect() const;
    TQRect downRect() const;

    void setUpEnabled( bool on );
    void setDownEnabled( bool on );

    bool isUpEnabled() const;
    bool isDownEnabled() const;

    enum ButtonSymbols { UpDownArrows, PlusMinus };
    virtual void	setButtonSymbols( ButtonSymbols bs );
    ButtonSymbols	buttonSymbols() const;

    void arrange();

signals:
    void stepUpPressed();
    void stepDownPressed();

public slots:
    void stepUp();
    void stepDown();
    
protected:
    void mousePressEvent( TQMouseEvent *e );
    void resizeEvent( TQResizeEvent* ev );
    void mouseReleaseEvent( TQMouseEvent *e );
    void mouseMoveEvent( TQMouseEvent *e );
#ifndef TQT_NO_WHEELEVENT
    void wheelEvent( TQWheelEvent * );
#endif
    void styleChange( TQStyle& );
    void paintEvent( TQPaintEvent * );
    void enableChanged( bool old );
    void windowActivationChange( bool );

private slots:
    void timerDone();
    void timerDoneEx();

private:
    TQSpinWidgetPrivate * d;

    void updateDisplay();

private:	// Disabled copy constructor and operator=
#if defined(TQ_DISABLE_COPY)
    TQSpinWidget( const TQSpinWidget& );
    TQSpinWidget& operator=( const TQSpinWidget& );
#endif
};

#endif // TQT_NO_SPINWIDGET

#endif // TQRANGECONTROL_H
