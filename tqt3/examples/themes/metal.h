/****************************************************************************
**
** Definition of the Metal Style for the themes example
**
** Created : 979899
**
** Copyright (C) 1997-2008 Trolltech ASA.  All rights reserved.
**
** This file is part of an example program for TQt.  This example
** program may be used, distributed and modified without limitation.
**
*****************************************************************************/

#ifndef METAL_H
#define METAL_H


#include <ntqpalette.h>

#ifndef TQT_NO_STYLE_WINDOWS

#include <ntqwindowsstyle.h>


class MetalStyle : public TQWindowsStyle
{
public:
    MetalStyle();
    void applicationPolish( const TQStyleControlElementData &ceData, ControlElementFlags elementFlags, void * );
    void applicationUnPolish( const TQStyleControlElementData &ceData, ControlElementFlags elementFlags, void * );
    void polish( const TQStyleControlElementData &ceData, ControlElementFlags elementFlags, void * );
    void unPolish( const TQStyleControlElementData &ceData, ControlElementFlags elementFlags, void * );

    void drawPrimitive( PrimitiveElement pe,
			TQPainter *p,
			const TQStyleControlElementData &ceData,
			ControlElementFlags elementFlags,
			const TQRect &r,
			const TQColorGroup &cg,
			SFlags flags = Style_Default,
			const TQStyleOption& = TQStyleOption::Default) const;

    void drawControl( ControlElement element,
		      TQPainter *p,
		      const TQStyleControlElementData &ceData,
		      ControlElementFlags elementFlags,
		      const TQRect &r,
		      const TQColorGroup &cg,
		      SFlags how = Style_Default,
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
    int pixelMetric( PixelMetric, const TQStyleControlElementData &ceData, ControlElementFlags elementFlags, const TQWidget * = 0 ) const;


private:
    void drawMetalFrame(  TQPainter *p, int x, int y, int w, int h ) const;
    void drawMetalGradient( TQPainter *p, int x, int y, int w, int h,
			  bool sunken, bool horz, bool flat=false ) const;
    void drawMetalButton( TQPainter *p, int x, int y, int w, int h,
			  bool sunken, bool horz, bool flat=false ) const;
    TQPalette oldPalette;
};

#endif

#endif
