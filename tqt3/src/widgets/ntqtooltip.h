/****************************************************************************
**
** Definition of Tool Tips (or Balloon Help) for any widget or rectangle
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

#ifndef TQTOOLTIP_H
#define TQTOOLTIP_H

#ifndef QT_H
#include "ntqwidget.h"
#endif // QT_H

#ifndef TQT_NO_TOOLTIP

#if __GNUC__  - 0 > 3
#pragma GCC system_header
#endif

class TQTipManager;
class TQIconViewToolTip;
class TQListViewToolTip;

class TQ_EXPORT TQToolTipGroup: public TQObject
{
    TQ_OBJECT
    TQ_PROPERTY( bool delay READ delay WRITE setDelay )
    TQ_PROPERTY( bool enabled READ enabled WRITE setEnabled )

public:
    TQToolTipGroup( TQObject *parent, const char *name = 0 );
   ~TQToolTipGroup();

    bool delay() const;
    bool enabled() const;

public slots:
    void setDelay( bool );
    void setEnabled( bool );

signals:
    void showTip( const TQString &);
    void removeTip();

private:
    uint del:1;
    uint ena:1;

    friend class TQTipManager;

private:	// Disabled copy constructor and operator=
#if defined(TQ_DISABLE_COPY)
    TQToolTipGroup( const TQToolTipGroup & );
    TQToolTipGroup& operator=( const TQToolTipGroup & );
#endif
};


class TQ_EXPORT TQToolTip: public TQt
{
public:
    TQToolTip( TQWidget *, TQToolTipGroup * = 0 );
    virtual ~TQToolTip();
    //### add virtual d'tor for 4.0

    static void add( TQWidget *, const TQString &);
    static void add( TQWidget *, const TQString &,
		     TQToolTipGroup *, const TQString& );
    static void remove( TQWidget * );

    static void add( TQWidget *, const TQRect &, const TQString &);
    static void add( TQWidget *, const TQRect &, const TQString &,
		     TQToolTipGroup *, const TQString& );
    static void remove( TQWidget *, const TQRect & );

    static TQString textFor( TQWidget *, const TQPoint & pos = TQPoint() );

    static void hide();

    static TQFont    font();
    static void	    setFont( const TQFont & );
    static TQPalette palette();
    static void	    setPalette( const TQPalette & );

#ifndef TQT_NO_COMPAT
    static void	    setEnabled( bool enable ) { setGloballyEnabled( enable ); }
    static bool	    enabled() { return isGloballyEnabled(); }
#endif
    static void	    setGloballyEnabled( bool );
    static bool	    isGloballyEnabled();
    static void	    setWakeUpDelay(int);

protected:
    virtual void maybeTip( const TQPoint & ) = 0;
    void    tip( const TQRect &, const TQString &);
    void    tip( const TQRect &, const TQString& , const TQString &);
    void    tip( const TQRect &, const TQString &, const TQRect & );
    void    tip( const TQRect &, const TQString&, const TQString &, const TQRect &);

    void    clear();

public:
    TQWidget	  *parentWidget() const { return p; }
    TQToolTipGroup *group()	  const { return g; }

private:
    TQWidget	    *p;
    TQToolTipGroup   *g;
    static TQFont    *ttFont;
    static TQPalette *ttPalette;

    friend class TQTipManager;
};


#endif // TQT_NO_TOOLTIP

#endif // TQTOOLTIP_H
