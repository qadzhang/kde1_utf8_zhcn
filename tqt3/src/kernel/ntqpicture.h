/****************************************************************************
**
** Definition of TQPicture class
**
** Created : 940729
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

#ifndef TQPICTURE_H
#define TQPICTURE_H

#ifndef QT_H
#include "ntqpaintdevice.h"
#include "ntqbuffer.h"
#endif // QT_H

#ifndef TQT_NO_PICTURE

class TQ_EXPORT TQPicture : public TQPaintDevice		// picture class
{
public:
    TQPicture( int formatVersion = -1 );
    TQPicture( const TQPicture & );
   ~TQPicture();

    bool	isNull() const;

    uint	size() const;
    const char* data() const;
    virtual void setData( const char* data, uint size );

    bool	play( TQPainter * );

    bool	load( TQIODevice *dev, const char *format = 0 );
    bool	load( const TQString &fileName, const char *format = 0 );
    bool	save( TQIODevice *dev, const char *format = 0 );
    bool	save( const TQString &fileName, const char *format = 0 );

    TQRect boundingRect() const;
    void setBoundingRect( const TQRect &r );

    TQPicture& operator= (const TQPicture&);

    friend TQ_EXPORT TQDataStream &operator<<( TQDataStream &, const TQPicture & );
    friend TQ_EXPORT TQDataStream &operator>>( TQDataStream &, TQPicture & );

protected:
    bool	cmd( int, TQPainter *, TQPDevCmdParam * );
    int		metric( int ) const;
    void	detach();
    TQPicture	copy() const;

private:
    bool	exec( TQPainter *, TQDataStream &, int );

    struct TQPicturePrivate : public TQShared {
	bool	cmd( int, TQPainter *, TQPDevCmdParam * );
	bool	checkFormat();
	void	resetFormat();

	TQBuffer	pictb;
	int	trecs;
	bool	formatOk;
	int	formatMajor;
	int	formatMinor;
	TQRect	brect;
    } *d;
};


inline bool TQPicture::isNull() const
{
    return d->pictb.buffer().isNull();
}

inline uint TQPicture::size() const
{
    return d->pictb.buffer().size();
}

inline const char* TQPicture::data() const
{
    return d->pictb.buffer().data();
}

/*****************************************************************************
  TQPicture stream functions
 *****************************************************************************/

TQ_EXPORT TQDataStream &operator<<( TQDataStream &, const TQPicture & );
TQ_EXPORT TQDataStream &operator>>( TQDataStream &, TQPicture & );

#endif // TQT_NO_PICTURE

#endif // TQPICTURE_H
