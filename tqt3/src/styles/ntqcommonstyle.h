/****************************************************************************
**
** Definition of TQCommonStyle class
**
** Created : 980616
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

#ifndef TQCOMMONSTYLE_H
#define TQCOMMONSTYLE_H

#ifndef QT_H
#include "ntqstyle.h"
#endif // QT_H

#ifndef TQT_NO_STYLE

class TQ_EXPORT TQCommonStyle: public TQStyle
{
    TQ_OBJECT

public:
    TQCommonStyle();
    ~TQCommonStyle();

    // New API
    void drawPrimitive( PrimitiveElement pe,
			TQPainter *p,
			const TQStyleControlElementData &ceData,
			ControlElementFlags elementFlags,
			const TQRect &r,
			const TQColorGroup &cg,
			SFlags flags = Style_Default,
			const TQStyleOption& = TQStyleOption::Default ) const;

    // New API
    void drawControl( ControlElement element,
			TQPainter *p,
			const TQStyleControlElementData &ceData,
			ControlElementFlags elementFlags,
			const TQRect &r,
			const TQColorGroup &cg,
			SFlags how = Style_Default,
			const TQStyleOption& = TQStyleOption::Default,
			const TQWidget *widget = 0 ) const;

    // New API
    void drawControlMask( ControlElement element,
			TQPainter *p,
			const TQStyleControlElementData &ceData,
			ControlElementFlags elementFlags,
			const TQRect &r,
			const TQStyleOption& = TQStyleOption::Default,
			const TQWidget *widget = 0 ) const;

    // New API
    TQRect subRect( SubRect r, const TQStyleControlElementData &ceData, const ControlElementFlags elementFlags, const TQWidget *widget ) const;

    // New API
    void drawComplexControl( ComplexControl control,
			     TQPainter *p,
			     const TQStyleControlElementData &ceData,
			     ControlElementFlags elementFlags,
			     const TQRect &r,
			     const TQColorGroup &cg,
			     SFlags how = Style_Default,
#ifdef Q_QDOC
			     SCFlags sub = SC_All,
#else
			     SCFlags sub = (uint)SC_All,
#endif
			     SCFlags subActive = SC_None,
			     const TQStyleOption& = TQStyleOption::Default,
			     const TQWidget *widget = 0 ) const;

    // New API
    void drawComplexControlMask( ComplexControl control,
				 TQPainter *p,
				 const TQStyleControlElementData &ceData,
				 const ControlElementFlags elementFlags,
				 const TQRect &r,
				 const TQStyleOption& = TQStyleOption::Default,
				 const TQWidget *widget = 0 ) const;

    // New API
    TQRect querySubControlMetrics( ComplexControl control,
				  const TQStyleControlElementData &ceData,
				  ControlElementFlags elementFlags,
				  SubControl sc,
				  const TQStyleOption& = TQStyleOption::Default,
				  const TQWidget *widget = 0 ) const;

    // New API
    SubControl querySubControl( ComplexControl control,
				const TQStyleControlElementData &ceData,
				ControlElementFlags elementFlags,
				const TQPoint &pos,
				const TQStyleOption& = TQStyleOption::Default,
				const TQWidget *widget = 0 ) const;

    // New API
    int pixelMetric( PixelMetric m, const TQStyleControlElementData &ceData, ControlElementFlags elementFlags, const TQWidget *widget = 0 ) const;

    // New API
    TQSize sizeFromContents( ContentsType s,
			    const TQStyleControlElementData &ceData,
			    ControlElementFlags elementFlags,
			    const TQSize &contentsSize,
			    const TQStyleOption& = TQStyleOption::Default,
			    const TQWidget *widget = 0 ) const;

    // New API
    int styleHint(StyleHint sh, const TQStyleControlElementData &ceData, ControlElementFlags elementFlags, const TQStyleOption &, TQStyleHintReturn *, const TQWidget * w) const;

    // New API
    TQPixmap stylePixmap( StylePixmap sp,
			 const TQStyleControlElementData &ceData,
			 ControlElementFlags elementFlags,
			 const TQStyleOption& = TQStyleOption::Default,
			 const TQWidget *widget = 0 ) const;

private:
    // Disabled copy constructor and operator=
#if defined(TQ_DISABLE_COPY)
    TQCommonStyle( const TQCommonStyle & );
    TQCommonStyle &operator=( const TQCommonStyle & );
#endif
};

#endif // TQT_NO_STYLE

#endif // TQCOMMONSTYLE_H
