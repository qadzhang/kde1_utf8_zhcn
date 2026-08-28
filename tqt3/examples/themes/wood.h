/****************************************************************************
**
** Definition of something or other
**
** Created : 979899
**
** Copyright (C) 1997-2008 Trolltech ASA.  All rights reserved.
**
** This file is part of an example program for TQt.  This example
** program may be used, distributed and modified without limitation.
**
*****************************************************************************/

#ifndef WOOD_H
#define WOOD_H


#include <ntqpalette.h>

#ifndef TQT_NO_STYLE_WINDOWS

#include <ntqwindowsstyle.h>


class NorwegianWoodStyle : public TQWindowsStyle
{
public:
    NorwegianWoodStyle();
    void applicationPolish( const TQStyleControlElementData &ceData, ControlElementFlags elementFlags, void * );
    void polish( const TQStyleControlElementData &ceData, ControlElementFlags elementFlags, void * );
    void unPolish( const TQStyleControlElementData &ceData, ControlElementFlags elementFlags, void * );
    void applicationUnPolish( const TQStyleControlElementData &ceData, ControlElementFlags elementFlags, void * );

    void drawPrimitive( PrimitiveElement pe,
			TQPainter *p,
			const TQStyleControlElementData &ceData,
			ControlElementFlags elementFlags,
			const TQRect &r,
			const TQColorGroup &cg,
			SFlags flags = Style_Default,
			const TQStyleOption& = TQStyleOption::Default ) const;

    void drawControl( ControlElement element,
		      TQPainter *p,
		      const TQStyleControlElementData &ceData,
		      ControlElementFlags elementFlags,
		      const TQRect &r,
		      const TQColorGroup &cg,
		      SFlags how = Style_Default,
		      const TQStyleOption& = TQStyleOption::Default,
		      const TQWidget *widget = 0 ) const;

    void drawControlMask( ControlElement element,
			  TQPainter *p,
			  const TQStyleControlElementData &ceData,
			  ControlElementFlags elementFlags,
			  const TQRect &r,
			  const TQStyleOption& = TQStyleOption::Default,
			  const TQWidget *widget = 0 ) const;

    void drawComplexControl( ComplexControl cc,
			     TQPainter *p,
			     const TQStyleControlElementData &ceData,
			     ControlElementFlags elementFlags,
			     const TQRect &r,
			     const TQColorGroup &cg,
			     SFlags how = Style_Default,
			     SCFlags sub = SC_All,
			     SCFlags subActive = SC_None,
			     const TQStyleOption& = TQStyleOption::Default,
			     const TQWidget *widget = 0 ) const;

    void drawComplexControlMask( ComplexControl control,
				 TQPainter *p,
				 const TQStyleControlElementData ceData,
				 const ControlElementFlags elementFlags,
				 const TQRect &r,
				 const TQStyleOption& = TQStyleOption::Default,
				 const TQWidget *widget = 0 ) const;

    TQRect querySubControlMetrics( ComplexControl control,
				  const TQStyleControlElementData &ceData,
				  ControlElementFlags elementFlags,
				  SubControl sc,
				  const TQStyleOption& = TQStyleOption::Default,
				  const TQWidget *widget = 0 ) const;

    TQRect subRect( SubRect r, const TQStyleControlElementData &ceData, const ControlElementFlags elementFlags, const TQWidget *widget ) const;


private:
    void drawSemicircleButton(TQPainter *p, const TQStyleControlElementData &ceData, ControlElementFlags elementFlags, const TQRect &r, int dir,
			      bool sunken, const TQColorGroup &g ) const;
    TQPalette oldPalette;
    TQPixmap *sunkenDark;
    TQPixmap *sunkenLight;

};

#endif

#endif
