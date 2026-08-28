/****************************************************************************
**
** Definition of TQSlider class
**
** Created : 961019
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

#ifndef TQSLIDER_H
#define TQSLIDER_H

#ifndef QT_H
#include "ntqwidget.h"
#include "ntqrangecontrol.h"
#endif // QT_H

#ifndef TQT_NO_SLIDER

struct TQSliderPrivate;

class TQTimer;

class TQ_EXPORT TQSlider : public TQWidget, public TQRangeControl
{
    TQ_OBJECT
    TQ_ENUMS( TickSetting )
    TQ_PROPERTY( int minValue READ minValue WRITE setMinValue )
    TQ_PROPERTY( int maxValue READ maxValue WRITE setMaxValue )
    TQ_PROPERTY( int lineStep READ lineStep WRITE setLineStep )
    TQ_PROPERTY( int pageStep READ pageStep WRITE setPageStep )
    TQ_PROPERTY( int value READ value WRITE setValue )
    TQ_PROPERTY( bool tracking READ tracking WRITE setTracking )
    TQ_PROPERTY( Orientation orientation READ orientation WRITE setOrientation )
    TQ_PROPERTY( TickSetting tickmarks READ tickmarks WRITE setTickmarks )
    TQ_PROPERTY( int tickInterval READ tickInterval WRITE setTickInterval )
	
public:
    enum TickSetting { NoMarks = 0, Above = 1, Left = Above,
		       Below = 2, Right = Below, Both = 3 };

    TQSlider( TQWidget *parent, const char* name = 0 );
    TQSlider( Orientation, TQWidget *parent, const char* name = 0 );
    TQSlider( int minValue, int maxValue, int pageStep, int value, Orientation,
	     TQWidget *parent, const char* name = 0 );
    ~TQSlider();

    virtual void	setOrientation( Orientation );
    Orientation orientation() const;
    virtual void	setTracking( bool enable );
    bool	tracking() const;
    virtual void 	setPalette( const TQPalette & );

    int sliderStart() const;
    TQRect sliderRect() const;
    TQSize sizeHint() const;
    void setSizePolicy( TQSizePolicy sp );
    void setSizePolicy( TQSizePolicy::SizeType hor, TQSizePolicy::SizeType ver, bool hfw = false );

    TQSizePolicy sizePolicy() const;
    TQSize minimumSizeHint() const;

    virtual void setTickmarks( TickSetting );
    TickSetting tickmarks() const { return ticks; }

    virtual void setTickInterval( int );
    int 	tickInterval() const { return tickInt; }

    int	 minValue() const;
    int	 maxValue() const;
    void setMinValue( int );
    void setMaxValue( int );
    int	 lineStep() const;
    int	 pageStep() const;
    void setLineStep( int );
    void setPageStep( int );
    int  value() const;

public slots:
    virtual void	setValue( int );
    void	addStep();
    void	subtractStep();
    void	addLine();
    void	subtractLine();

signals:
    void	valueChanged( int value );
    void	sliderPressed();
    void	sliderMoved( int value );
    void	sliderReleased();

protected:
    void	resizeEvent( TQResizeEvent * );
    void	paintEvent( TQPaintEvent * );

    void	keyPressEvent( TQKeyEvent * );
    void	mousePressEvent( TQMouseEvent * );
    void	mouseReleaseEvent( TQMouseEvent * );
    void	mouseMoveEvent( TQMouseEvent * );
#ifndef TQT_NO_WHEELEVENT
    void	wheelEvent( TQWheelEvent * );
#endif
    void	focusInEvent( TQFocusEvent *e );
    void	focusOutEvent( TQFocusEvent *e );

    void	styleChange( TQStyle& );

    void	valueChange();
    void	rangeChange();

private slots:
    void	repeatTimeout();

private:
    enum State { Idle, Dragging, TimingUp, TimingDown };

    void	init();
    int		positionFromValue( int ) const;
    int		valueFromPosition( int ) const;
    void	moveSlider( int );
    void	reallyMoveSlider( int );
    void	resetState();
    int		available() const;
    int		goodPart( const TQPoint& ) const;
    void	initTicks();

    TQSliderPrivate *d;
    TQTimer	*timer;
    TQCOORD	sliderPos;
    int		sliderVal;
    TQCOORD	clickOffset;
    State	state;
    bool	track;
    TQCOORD	tickOffset;
    TickSetting	ticks;
    int		tickInt;
    Orientation orient;

private:	// Disabled copy constructor and operator=
#if defined(TQ_DISABLE_COPY)
    TQSlider( const TQSlider & );
    TQSlider &operator=( const TQSlider & );
#endif
};

inline bool TQSlider::tracking() const
{
    return track;
}

inline TQSlider::Orientation TQSlider::orientation() const
{
    return orient;
}

inline int TQSlider::sliderStart() const
{
    return sliderPos;
}

inline void TQSlider::setSizePolicy( TQSizePolicy::SizeType hor, TQSizePolicy::SizeType ver, bool hfw )
{ 
    TQWidget::setSizePolicy( hor, ver, hfw ); 
}

#endif // TQT_NO_SLIDER

#endif // TQSLIDER_H
