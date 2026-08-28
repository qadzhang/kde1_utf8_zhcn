/****************************************************************************
**
** Definition of TQIconSet class
**
** Created : 980318
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

#ifndef TQICONSET_H
#define TQICONSET_H

#ifndef QT_H
#include "ntqobject.h"
#include "ntqpixmap.h"
#endif // QT_H

#ifndef TQT_NO_ICONSET

class TQIconFactory;
class TQIconSetPrivate;

// ### Remove all 'virtual' functions in TQIconSet (but not TQIconFactory) in TQt 4.0
class TQ_EXPORT TQIconSet
{
public:
    // the implementation makes assumptions about the value of these
    enum Size { Automatic, Small, Large };
    enum Mode { Normal, Disabled, Active };
    enum State { On, Off };

    TQIconSet();
    TQIconSet( const TQPixmap& pixmap, Size size = Automatic );
    TQIconSet( const TQPixmap& smallPix, const TQPixmap& largePix );
    TQIconSet( const TQIconSet& other );
    virtual ~TQIconSet();

    void reset( const TQPixmap& pixmap, Size size );

    virtual void setPixmap( const TQPixmap& pixmap, Size size,
			    Mode mode = Normal, State state = Off );
    virtual void setPixmap( const TQString& fileName, Size size,
			    Mode mode = Normal, State state = Off );
    TQPixmap pixmap( Size size, Mode mode, State state = Off ) const;
    TQPixmap pixmap( Size size, bool enabled, State state = Off ) const;
    TQPixmap pixmap() const;
    bool isGenerated( Size size, Mode mode, State state = Off ) const;
    void clearGenerated();
    void installIconFactory( TQIconFactory *factory );

    bool isNull() const;

    void detach();

    TQIconSet& operator=( const TQIconSet& other );

    // static functions
    static void setIconSize( Size which, const TQSize& size );
    static const TQSize& iconSize( Size which );

private:
    void normalize( Size& which, const TQSize& pixSize );
    TQPixmap *createScaled( Size size, const TQPixmap *suppliedPix ) const;
    TQPixmap *createDisabled( Size size, State state ) const;

    TQIconSetPrivate *d;
};

class TQ_EXPORT TQIconFactory : private TQShared
{
public:
    TQIconFactory();
    virtual ~TQIconFactory();

    virtual TQPixmap *createPixmap( const TQIconSet& iconSet, TQIconSet::Size size,
				   TQIconSet::Mode mode, TQIconSet::State state );
    void setAutoDelete( bool autoDelete ) { autoDel = autoDelete; }
    bool autoDelete() const { return autoDel; }

    static TQIconFactory *defaultFactory();
    static void installDefaultFactory( TQIconFactory *factory );

private:
#if defined(TQ_DISABLE_COPY)
    TQIconFactory( const TQIconFactory & );
    TQIconFactory &operator=( const TQIconFactory & );
#endif

    friend class TQIconSet;
    friend class TQIconSetPrivate;

    uint autoDel : 1;
    uint unused : 31;
};

#endif // TQT_NO_ICONSET
#endif
