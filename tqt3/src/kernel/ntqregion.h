/****************************************************************************
**
** Definition of TQRegion class
**
** Created : 940514
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

#ifndef TQREGION_H
#define TQREGION_H

#ifndef QT_H
#include "ntqshared.h"
#include "ntqrect.h"
#endif // QT_H

#ifdef TQ_WS_X11
struct TQRegionPrivate;
#endif

class TQ_EXPORT TQRegion
{
public:
    enum RegionType { Rectangle, Ellipse };

    TQRegion();
    TQRegion( int x, int y, int w, int h, RegionType = Rectangle );
    TQRegion( const TQRect &, RegionType = Rectangle );
    TQRegion( const TQPointArray &, bool winding=false );
    TQRegion( const TQRegion & );
    TQRegion( const TQBitmap & );
   ~TQRegion();
    TQRegion &operator=( const TQRegion & );

    bool    isNull()   const;
    bool    isEmpty()  const;

    bool    contains( const TQPoint &p ) const;
    bool    contains( const TQRect &r )	const;

    void    translate( int dx, int dy );

    TQRegion unite( const TQRegion & )	const;
    TQRegion intersect( const TQRegion &) const;
    TQRegion subtract( const TQRegion & ) const;
    TQRegion eor( const TQRegion & )	const;

    TQRect   boundingRect() const;
    TQMemArray<TQRect> rects() const;
    void setRects( const TQRect *, int );

    const TQRegion operator|( const TQRegion & ) const;
    const TQRegion operator+( const TQRegion & ) const;
    const TQRegion operator&( const TQRegion & ) const;
    const TQRegion operator-( const TQRegion & ) const;
    const TQRegion operator^( const TQRegion & ) const;
    TQRegion& operator|=( const TQRegion & );
    TQRegion& operator+=( const TQRegion & );
    TQRegion& operator&=( const TQRegion & );
    TQRegion& operator-=( const TQRegion & );
    TQRegion& operator^=( const TQRegion & );

    bool    operator==( const TQRegion & )  const;
    bool    operator!=( const TQRegion &r ) const
			{ return !(operator==(r)); }

#if defined(TQ_WS_WIN)
    HRGN    handle() const { return data->rgn; }
#elif defined(TQ_WS_X11)
    Region handle() const { if(!data->rgn) updateX11Region(); return data->rgn; }
#elif defined(TQ_WS_MAC)
    RgnHandle handle(bool require_rgn=false) const;
#elif defined(TQ_WS_QWS)
    // TQGfx_QWS needs this for region drawing
    void * handle() const { return data->rgn; }
#endif

#ifndef TQT_NO_DATASTREAM
    friend TQ_EXPORT TQDataStream &operator<<( TQDataStream &, const TQRegion & );
    friend TQ_EXPORT TQDataStream &operator>>( TQDataStream &, TQRegion & );
#endif
private:
    TQRegion( bool );
    TQRegion copy() const;
    void    detach();
#if defined(TQ_WS_WIN)
    TQRegion winCombine( const TQRegion &, int ) const;
#endif
#if defined(TQ_WS_X11)
    void updateX11Region() const;
    void *clipRectangles( int &num ) const;
    friend void *qt_getClipRects( const TQRegion &, int & );
#endif
    void    exec( const TQByteArray &, int ver = 0 );
    struct TQRegionData : public TQShared {
#if defined(TQ_WS_WIN)
	HRGN   rgn;
#elif defined(TQ_WS_X11)
	Region rgn;
	void *xrectangles;
	TQRegionPrivate *region;
#elif defined(TQ_WS_MAC)
	uint is_rect:1;
	TQRect rect;
	RgnHandle rgn;
#elif defined(TQ_WS_QWS)
	void * rgn;
#endif
	bool   is_null;
    } *data;
#if defined(TQ_WS_MAC)
    friend struct tqt_mac_rgn_data_cache;
    friend TQRegionData *tqt_mac_get_rgn_data();
    friend void tqt_mac_free_rgn_data(TQRegionData *);
    void rectifyRegion();
#elif defined(TQ_WS_WIN)
    friend class TQETWidget;
#endif

};


#define TQRGN_SETRECT		1		// region stream commands
#define TQRGN_SETELLIPSE		2		//  (these are internal)
#define TQRGN_SETPTARRAY_ALT	3
#define TQRGN_SETPTARRAY_WIND	4
#define TQRGN_TRANSLATE		5
#define TQRGN_OR			6
#define TQRGN_AND		7
#define TQRGN_SUB		8
#define TQRGN_XOR		9
#define TQRGN_RECTS	       10


/*****************************************************************************
  TQRegion stream functions
 *****************************************************************************/

#ifndef TQT_NO_DATASTREAM
TQ_EXPORT TQDataStream &operator<<( TQDataStream &, const TQRegion & );
TQ_EXPORT TQDataStream &operator>>( TQDataStream &, TQRegion & );
#endif


#endif // TQREGION_H
