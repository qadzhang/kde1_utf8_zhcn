/****************************************************************************
**
** Definition of TQBrush class
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

#ifndef TQBRUSH_H
#define TQBRUSH_H

#ifndef QT_H
#include "ntqcolor.h"
#include "ntqshared.h"
#endif // QT_H


class TQ_EXPORT TQBrush: public TQt
{
friend class TQPainter;
public:
    TQBrush();
    TQBrush( BrushStyle );
    TQBrush( const TQColor &, BrushStyle=SolidPattern );
    TQBrush( const TQColor &, const TQPixmap & );
    TQBrush( const TQBrush & );
   ~TQBrush();
    TQBrush &operator=( const TQBrush & );

    BrushStyle	style()	 const		{ return data->style; }
    void	setStyle( BrushStyle );
    const TQColor &color()const		{ return data->color; }
    void	setColor( const TQColor & );
    TQPixmap    *pixmap() const		{ return data->pixmap; }
    void	setPixmap( const TQPixmap & );

    bool	operator==( const TQBrush &p ) const;
    bool	operator!=( const TQBrush &b ) const
					{ return !(operator==(b)); }

private:
    TQBrush	copy()	const;
    void	detach();
    void	init( const TQColor &, BrushStyle );
    struct TQBrushData : public TQShared {	// brush data
	BrushStyle style;
	TQColor	  color;
	TQPixmap	 *pixmap;
    } *data;
};


/*****************************************************************************
  TQBrush stream functions
 *****************************************************************************/

#ifndef TQT_NO_DATASTREAM
TQ_EXPORT TQDataStream &operator<<( TQDataStream &, const TQBrush & );
TQ_EXPORT TQDataStream &operator>>( TQDataStream &, TQBrush & );
#endif

#endif // TQBRUSH_H
