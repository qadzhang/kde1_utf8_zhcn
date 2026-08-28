/****************************************************************************
**
** Definition of TQPen class
**
** Created : 940112
**
** Copyright (C) 1992-2008 Trolltech ASA.  All rights reserved.
**
** This file is part of the kernel module of the TQt GUI Toolkit.
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

#ifndef TQPEN_H
#define TQPEN_H

#ifndef QT_H
#include "ntqcolor.h"
#include "ntqshared.h"
#endif // QT_H


class TQ_EXPORT TQPen: public TQt
{
public:
    TQPen();
    TQPen( PenStyle );
    TQPen( const TQColor &color, uint width=0, PenStyle style=SolidLine );
    TQPen( const TQColor &cl, uint w, PenStyle s, PenCapStyle c, PenJoinStyle j);
    TQPen( const TQPen & );
   ~TQPen();
    TQPen &operator=( const TQPen & );

    PenStyle	style() const		{ return data->style; }
    void	setStyle( PenStyle );
    uint	width() const		{ return data->width; }
    void	setWidth( uint );
    const TQColor &color() const		{ return data->color; }
    void	setColor( const TQColor & );
    PenCapStyle	capStyle() const;
    void	setCapStyle( PenCapStyle );
    PenJoinStyle joinStyle() const;
    void	setJoinStyle( PenJoinStyle );

    bool	operator==( const TQPen &p ) const;
    bool	operator!=( const TQPen &p ) const
					{ return !(operator==(p)); }

private:
    friend class TQPainter;
#ifdef TQ_WS_WIN
    friend class TQFontEngineWin;
#endif

    TQPen	copy()	const;
    void	detach();
    void	init( const TQColor &, uint, uint );
    struct TQPenData : public TQShared {		// pen data
	PenStyle  style;
	uint	  width;
	TQColor	  color;
	TQ_UINT16  linest;
    } *data;
};


/*****************************************************************************
  TQPen stream functions
 *****************************************************************************/
#ifndef TQT_NO_DATASTREAM
TQ_EXPORT TQDataStream &operator<<( TQDataStream &, const TQPen & );
TQ_EXPORT TQDataStream &operator>>( TQDataStream &, TQPen & );
#endif

#endif // TQPEN_H
