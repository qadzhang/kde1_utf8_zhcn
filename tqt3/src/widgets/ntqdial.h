/****************************************************************************
**
** Definition of the dial widget
**
** Created : 990104
**
** Copyright (C) 1999-2008 Trolltech ASA.  All rights reserved.
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


#ifndef TQDIAL_H
#define TQDIAL_H

#ifndef QT_H
#include "ntqwidget.h"
#include "ntqrangecontrol.h"
#endif // QT_H

#ifndef TQT_NO_DIAL

class TQDialPrivate;

class TQ_EXPORT TQDial: public TQWidget, public TQRangeControl
{
    TQ_OBJECT
    TQ_PROPERTY( bool tracking READ tracking WRITE setTracking )
    TQ_PROPERTY( bool wrapping READ wrapping WRITE setWrapping )
    TQ_PROPERTY( int notchSize READ notchSize )
    TQ_PROPERTY( double notchTarget READ notchTarget WRITE setNotchTarget )
    TQ_PROPERTY( bool notchesVisible READ notchesVisible WRITE setNotchesVisible )
    TQ_PROPERTY( int minValue READ minValue WRITE setMinValue )
    TQ_PROPERTY( int maxValue READ maxValue WRITE setMaxValue )
    TQ_PROPERTY( int lineStep READ lineStep WRITE setLineStep )
    TQ_PROPERTY( int pageStep READ pageStep WRITE setPageStep )
    TQ_PROPERTY( int value READ value WRITE setValue )
	
public:
    TQDial( TQWidget* parent=0, const char* name=0, WFlags f = 0 );
    TQDial( int minValue, int maxValue, int pageStep, int value,
	   TQWidget* parent=0, const char* name=0 );
    ~TQDial();

    bool tracking() const;

    bool wrapping() const;

    int notchSize() const;

    virtual void setNotchTarget( double );
    double notchTarget() const;

    bool notchesVisible() const;

    TQSize sizeHint() const;
    TQSize minimumSizeHint() const;

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
    virtual void setValue( int );
    void addLine();
    void subtractLine();
    void addPage();
    void subtractPage();
    virtual void setNotchesVisible( bool b );
    virtual void setWrapping( bool on );
    virtual void setTracking( bool enable );

signals:
    void valueChanged( int value );
    void dialPressed();
    void dialMoved( int value );
    void dialReleased();

protected:
    void resizeEvent( TQResizeEvent * );
    void paintEvent( TQPaintEvent * );

    void keyPressEvent( TQKeyEvent * );
    void mousePressEvent( TQMouseEvent * );
    void mouseReleaseEvent( TQMouseEvent * );
    void mouseMoveEvent( TQMouseEvent * );
#ifndef TQT_NO_WHEELEVENT
    void wheelEvent( TQWheelEvent * );
#endif
    void focusInEvent( TQFocusEvent * );
    void focusOutEvent( TQFocusEvent * );

    void valueChange();
    void rangeChange();

    virtual void repaintScreen( const TQRect *cr = 0 );

private:
    TQDialPrivate * d;

    int valueFromPoint( const TQPoint & ) const;
    double angle( const TQPoint &, const TQPoint & ) const;
    TQPointArray calcArrow( double &a ) const;
    TQRect calcDial() const;
    int calcBigLineSize() const;
    void calcLines();

private: // Disabled copy constructor and operator=
#if defined(TQ_DISABLE_COPY)
    TQDial( const TQDial & );
    TQDial &operator=( const TQDial & );
#endif

};

#endif  // TQT_NO_DIAL

#endif
