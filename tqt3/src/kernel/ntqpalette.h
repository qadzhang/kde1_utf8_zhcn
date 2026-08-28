/****************************************************************************
**
** Definition of TQColorGroup and TQPalette classes
**
** Created : 950323
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

#ifndef TQPALETTE_H
#define TQPALETTE_H

#ifndef QT_H
#include "ntqwindowdefs.h"
#include "ntqcolor.h"
#include "ntqshared.h"
#include "ntqbrush.h" // TQColor->TQBrush conversion
#endif // QT_H

#ifndef TQT_NO_PALETTE

class TQColorGroupPrivate;

class TQ_EXPORT TQColorGroup
{
public:
    TQColorGroup();
    TQColorGroup( const TQColor &foreground, const TQColor &button,
		 const TQColor &light, const TQColor &dark, const TQColor &mid,
		 const TQColor &text, const TQColor &base );
    TQColorGroup( const TQBrush &foreground, const TQBrush &button,
		 const TQBrush &light, const TQBrush &dark, const TQBrush &mid,
		 const TQBrush &text, const TQBrush &bright_text,
		 const TQBrush &base, const TQBrush &background);
    TQColorGroup( const TQColorGroup & );

   ~TQColorGroup();

    TQColorGroup& operator =(const TQColorGroup&);

    // Do not change the order, the serialization format depends on it
    enum ColorRole { Foreground, Button, Light, Midlight, Dark, Mid,
		     Text, BrightText, ButtonText, Base, Background, Shadow,
		     Highlight, HighlightedText, Link, LinkVisited,
		     NColorRoles };

    const TQColor &color( ColorRole ) const;
    const TQBrush &brush( ColorRole ) const;
    void setColor( ColorRole, const TQColor & );
    void setBrush( ColorRole, const TQBrush & );

    const TQColor &foreground()	const	{ return br[Foreground].color(); }
    const TQColor &button()	const	{ return br[Button].color(); }
    const TQColor &light()	const	{ return br[Light].color(); }
    const TQColor &dark()	const	{ return br[Dark].color(); }
    const TQColor &mid()		const	{ return br[Mid].color(); }
    const TQColor &text()	const	{ return br[Text].color(); }
    const TQColor &base()	const	{ return br[Base].color(); }
    const TQColor &background()	const	{ return br[Background].color(); }

    const TQColor &midlight()	const	{ return br[Midlight].color(); }
    const TQColor &brightText()	const	{ return br[BrightText].color(); }
    const TQColor &buttonText()	const	{ return br[ButtonText].color(); }
    const TQColor &shadow()	const	{ return br[Shadow].color(); }
    const TQColor &highlight()	const	{ return br[Highlight].color(); }
    const TQColor &highlightedText() const{return br[HighlightedText].color(); }
    const TQColor &link()        const   { return br[Link].color(); }
    const TQColor &linkVisited() const   { return br[LinkVisited].color(); }

    bool	operator==( const TQColorGroup &g ) const;
    bool	operator!=( const TQColorGroup &g ) const
	{ return !(operator==(g)); }

private:
    TQBrush *br;
    TQColorGroupPrivate * d;

    friend class TQPalette;
};


class TQ_EXPORT TQPalette
{
public:
    TQPalette();
    TQPalette( const TQColor &button );
    TQPalette( const TQColor &button, const TQColor &background );
    TQPalette( const TQColorGroup &active, const TQColorGroup &disabled,
	      const TQColorGroup &inactive );
    TQPalette( const TQPalette & );
    ~TQPalette();
    TQPalette &operator=( const TQPalette & );

    enum ColorGroup { Disabled, Active, Inactive, NColorGroups, Normal=Active };

    const TQColor &color( ColorGroup, TQColorGroup::ColorRole ) const;
    const TQBrush &brush( ColorGroup, TQColorGroup::ColorRole ) const;
    void setColor( ColorGroup, TQColorGroup::ColorRole, const TQColor & );
    void setBrush( ColorGroup, TQColorGroup::ColorRole, const TQBrush & );

    void setColor( TQColorGroup::ColorRole, const TQColor & );
    void setBrush( TQColorGroup::ColorRole, const TQBrush & );

    TQPalette	copy() const;

    const TQColorGroup &active() const { return data->active; }
    const TQColorGroup &disabled() const { return data->disabled; }
    const TQColorGroup &inactive() const { return data->inactive; }
#ifndef TQT_NO_COMPAT
    const TQColorGroup &normal() const { return active(); }
#endif

    void	setActive( const TQColorGroup & );
    void	setDisabled( const TQColorGroup & );
    void	setInactive( const TQColorGroup & );
#ifndef TQT_NO_COMPAT
    void	setNormal( const TQColorGroup & cg ) { setActive(cg); }
#endif

    bool	operator==( const TQPalette &p ) const;
    bool	operator!=( const TQPalette &p ) const
              { return !(operator==(p)); }
    bool	isCopyOf( const TQPalette & );

    int		serialNumber() const	{ return data->ser_no; }
    
    
    static TQColorGroup::ColorRole foregroundRoleFromMode( TQt::BackgroundMode mode );
    static TQColorGroup::ColorRole backgroundRoleFromMode( TQt::BackgroundMode mode);

private:
    void	detach();
    const TQBrush &directBrush( ColorGroup, TQColorGroup::ColorRole ) const;
    void directSetBrush( ColorGroup, TQColorGroup::ColorRole, const TQBrush& );

    struct TQPalData : public TQShared {
	TQColorGroup disabled;
	TQColorGroup active;
	int	    ser_no;
	TQColorGroup inactive;
    } *data;
};


/*****************************************************************************
  TQColorGroup/TQPalette stream functions
 *****************************************************************************/

#ifndef TQT_NO_DATASTREAM
TQ_EXPORT TQDataStream &operator<<( TQDataStream &, const TQColorGroup & );
TQ_EXPORT TQDataStream &operator>>( TQDataStream &, TQColorGroup & );

TQ_EXPORT TQDataStream &operator<<( TQDataStream &, const TQPalette & );
TQ_EXPORT TQDataStream &operator>>( TQDataStream &, TQPalette & );
#endif // TQT_NO_DATASTREAM

#endif // TQT_NO_PALETTE
#endif // TQPALETTE_H
