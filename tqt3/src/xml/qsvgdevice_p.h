/****************************************************************************
**
** Definition of the TQSvgDevice class
**
** Created : 001024
**
** Copyright (C) 2000-2008 Trolltech ASA.  All rights reserved.
**
** This file is part of the xml module of the TQt GUI Toolkit.
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
*****************************************************************************/

#ifndef TQSVGDEVICE_P_H
#define TQSVGDEVICE_P_H

//
//  W A R N I N G
//  -------------
//
// This file is not part of the TQt API.  It exists for the convenience
// of the TQPicture class. This header file may change from
// version to version without notice, or even be removed.
//
// We mean it.
//
//

#ifndef QT_H
#include "ntqpaintdevice.h"
#include "ntqrect.h"
#include "ntqdom.h"
#endif // QT_H

#if !defined(TQT_MODULE_XML) || defined( QT_LICENSE_PROFESSIONAL ) || defined( QT_INTERNAL_XML )
#define TQM_EXPORT_SVG
#else
#define TQM_EXPORT_SVG TQ_EXPORT
#endif

#ifndef TQT_NO_SVG

class TQPainter;
class TQDomNode;
class TQDomNamedNodeMap;
struct TQSvgDeviceState;
class TQSvgDevicePrivate;

class TQM_EXPORT_SVG TQSvgDevice : public TQPaintDevice
{
public:
    TQSvgDevice();
    ~TQSvgDevice();

    bool play( TQPainter *p );

    TQString toString() const;

    bool load( TQIODevice *dev );
    bool save( TQIODevice *dev );
    bool save( const TQString &fileName );

    TQRect boundingRect() const;
    void setBoundingRect( const TQRect &r );

protected:
    virtual bool cmd ( int, TQPainter*, TQPDevCmdParam* );
    virtual int	 metric( int ) const;

private:
    // reading
    bool play( const TQDomNode &node );
    void saveAttributes();
    void restoreAttributes();
    TQColor parseColor( const TQString &col );
    double parseLen( const TQString &str, bool *ok=0, bool horiz=true ) const;
    int lenToInt( const TQDomNamedNodeMap &map, const TQString &attr,
		  int def=0 ) const;
    double lenToDouble( const TQDomNamedNodeMap &map, const TQString &attr,
		  int def=0 ) const;
    void setStyleProperty( const TQString &prop, const TQString &val,
			   TQPen *pen, TQFont *font, int *talign );
    void setStyle( const TQString &s );
    void setTransform( const TQString &tr );
    void drawPath( const TQString &data );

    // writing
    void appendChild( TQDomElement &e, int c );
    void applyStyle( TQDomElement *e, int c ) const;
    void applyTransform( TQDomElement *e ) const;

    // reading
    TQRect brect;			// bounding rectangle
    TQDomDocument doc;			// document tree
    TQDomNode current;
    TQPoint curPt;
    TQSvgDeviceState *curr;
    TQPainter *pt;			// used by play() et al

    // writing
    bool dirtyTransform, dirtyStyle;

    TQSvgDevicePrivate *d;
};

inline TQRect TQSvgDevice::boundingRect() const
{
    return brect;
}

#endif // TQT_NO_SVG

#endif // TQSVGDEVICE_P_H
