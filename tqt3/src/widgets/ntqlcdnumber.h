/****************************************************************************
**
** Definition of TQLCDNumber class
**
** Created : 940518
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

#ifndef TQLCDNUMBER_H
#define TQLCDNUMBER_H

#ifndef QT_H
#include "ntqframe.h"
#include "ntqbitarray.h"
#endif // QT_H

#ifndef TQT_NO_LCDNUMBER


class TQLCDNumberPrivate;

class TQ_EXPORT TQLCDNumber : public TQFrame		// LCD number widget
{
    TQ_OBJECT
    TQ_ENUMS( Mode SegmentStyle )
    TQ_PROPERTY( bool smallDecimalPoint READ smallDecimalPoint WRITE setSmallDecimalPoint )
    TQ_PROPERTY( int numDigits READ numDigits WRITE setNumDigits )
    TQ_PROPERTY( Mode mode READ mode WRITE setMode )
    TQ_PROPERTY( SegmentStyle segmentStyle READ segmentStyle WRITE setSegmentStyle )
    TQ_PROPERTY( double value READ value WRITE display )
    TQ_PROPERTY( int intValue READ intValue WRITE display )

public:
    TQLCDNumber( TQWidget* parent=0, const char* name=0 );
    TQLCDNumber( uint numDigits, TQWidget* parent=0, const char* name=0 );
    ~TQLCDNumber();

    enum Mode { Hex, Dec, Oct, Bin, HEX = Hex, DEC = Dec, OCT = Oct,
		BIN = Bin };
    enum SegmentStyle { Outline, Filled, Flat };

    bool    smallDecimalPoint() const;

    int	    numDigits() const;
    virtual void setNumDigits( int nDigits );

    bool    checkOverflow( double num ) const;
    bool    checkOverflow( int	  num ) const;

    Mode mode() const;
    virtual void setMode( Mode );

    SegmentStyle segmentStyle() const;
    virtual void setSegmentStyle( SegmentStyle );

    double  value() const;
    int	    intValue() const;

    TQSize sizeHint() const;

public slots:
    void    display( const TQString &str );
    void    display( int num );
    void    display( double num );
    virtual void setHexMode();
    virtual void setDecMode();
    virtual void setOctMode();
    virtual void setBinMode();
    virtual void setSmallDecimalPoint( bool );

signals:
    void    overflow();

protected:
    void    drawContents( TQPainter * );

private:
    void    init();
    void    internalDisplay( const TQString &);
    void    internalSetString( const TQString& s );
    void    drawString( const TQString& s, TQPainter &, TQBitArray * = 0,
			bool = true );
    //void    drawString( const TQString &, TQPainter &, TQBitArray * = 0 ) const;
    void    drawDigit( const TQPoint &, TQPainter &, int, char,
		       char = ' ' );
    void    drawSegment( const TQPoint &, char, TQPainter &, int, bool = false );

    int	    ndigits;
    double  val;
    uint    base	: 2;
    uint    smallPoint	: 1;
    uint    fill	: 1;
    uint    shadow	: 1;
    TQString digitStr;
    TQBitArray points;
    TQLCDNumberPrivate * d;

private:	// Disabled copy constructor and operator=
#if defined(TQ_DISABLE_COPY)
    TQLCDNumber( const TQLCDNumber & );
    TQLCDNumber &operator=( const TQLCDNumber & );
#endif
};

inline bool TQLCDNumber::smallDecimalPoint() const
{ return (bool)smallPoint; }

inline int TQLCDNumber::numDigits() const
{ return ndigits; }


#endif // TQT_NO_LCDNUMBER

#endif // TQLCDNUMBER_H
