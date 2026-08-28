/****************************************************************************
**
** Definition of TQScrollBar class
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

#ifndef TQSCROLLBAR_H
#define TQSCROLLBAR_H

class TQTimer;

#ifndef QT_H
#include "ntqwidget.h"
#include "ntqrangecontrol.h"
#endif // QT_H

#ifndef TQT_NO_SCROLLBAR

class TQ_EXPORT TQScrollBar : public TQWidget, public TQRangeControl
{
    TQ_OBJECT
    TQ_PROPERTY( int minValue READ minValue WRITE setMinValue )
    TQ_PROPERTY( int maxValue READ maxValue WRITE setMaxValue )
    TQ_PROPERTY( int lineStep READ lineStep WRITE setLineStep )
    TQ_PROPERTY( int pageStep READ pageStep WRITE setPageStep )
    TQ_PROPERTY( int value READ value WRITE setValue )
    TQ_PROPERTY( bool tracking READ tracking WRITE setTracking )
    TQ_PROPERTY( bool draggingSlider READ draggingSlider )
    TQ_PROPERTY( Orientation orientation READ orientation WRITE setOrientation )

public:
    TQScrollBar( TQWidget *parent, const char* name = 0 );
    TQScrollBar( Orientation, TQWidget *parent, const char* name = 0 );
    TQScrollBar( int minValue, int maxValue, int lineStep, int pageStep,
		int value, Orientation, TQWidget *parent, const char* name = 0 );
    ~TQScrollBar();

    virtual void setOrientation( Orientation );
    Orientation orientation() const;
    virtual void setTracking( bool enable );
    bool	tracking() const;
    bool	draggingSlider() const;

    virtual void setPalette( const TQPalette & );
    virtual TQSize sizeHint() const;
    virtual void setSizePolicy( TQSizePolicy sp );
    void setSizePolicy( TQSizePolicy::SizeType hor, TQSizePolicy::SizeType ver, bool hfw = false );

    int	 minValue() const;
    int	 maxValue() const;
    void setMinValue( int );
    void setMaxValue( int );
    int	 lineStep() const;
    int	 pageStep() const;
    void setLineStep( int );
    void setPageStep( int );
    int  value() const;

    int		sliderStart() const;
    TQRect	sliderRect() const;

public slots:
    void setValue( int );

signals:
    void	valueChanged( int value );
    void	sliderPressed();
    void	sliderMoved( int value );
    void	sliderReleased();
    void	nextLine();
    void	prevLine();
    void	nextPage();
    void	prevPage();

protected:
#ifndef TQT_NO_WHEELEVENT
    void 	wheelEvent( TQWheelEvent * );
#endif
    void	keyPressEvent( TQKeyEvent * );
    void	resizeEvent( TQResizeEvent * );
    void	paintEvent( TQPaintEvent * );

    void	mousePressEvent( TQMouseEvent * );
    void	mouseReleaseEvent( TQMouseEvent * );
    void	mouseMoveEvent( TQMouseEvent * );
    void	contextMenuEvent( TQContextMenuEvent * );
    void	hideEvent( TQHideEvent* );

    void	valueChange();
    void	stepChange();
    void	rangeChange();

    void	styleChange( TQStyle& );

private slots:
    void doAutoRepeat();

private:
    void init();
    void positionSliderFromValue();
    int calculateValueFromSlider() const;

    void startAutoRepeat();
    void stopAutoRepeat();

    int rangeValueToSliderPos( int val ) const;
    int sliderPosToRangeValue( int val ) const;

    void action( int control );

    void drawControls( uint controls, uint activeControl ) const;
    void drawControls( uint controls, uint activeControl,
		       TQPainter *p ) const;

    uint pressedControl;
    bool track;
    bool clickedAt;
    Orientation orient;

    int slidePrevVal;
    TQCOORD sliderPos;
    TQCOORD clickOffset;

    TQTimer * repeater;
    void * d;

private:	// Disabled copy constructor and operator=
#if defined(TQ_DISABLE_COPY)
    TQScrollBar( const TQScrollBar & );
    TQScrollBar &operator=( const TQScrollBar & );
#endif
};


inline void TQScrollBar::setTracking( bool t )
{
    track = t;
}

inline bool TQScrollBar::tracking() const
{
    return track;
}

inline TQScrollBar::Orientation TQScrollBar::orientation() const
{
    return orient;
}

inline int TQScrollBar::sliderStart() const
{
    return sliderPos;
}

inline void TQScrollBar::setSizePolicy( TQSizePolicy::SizeType hor, TQSizePolicy::SizeType ver, bool hfw )
{ 
    TQWidget::setSizePolicy( hor, ver, hfw ); 
}


#endif // TQT_NO_SCROLLBAR

#endif // TQSCROLLBAR_H
