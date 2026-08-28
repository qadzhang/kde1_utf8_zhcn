/****************************************************************************
**
** Definition of Windows-like style class
**
** Created : 981231
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

#ifndef TQWINDOWSSTYLE_H
#define TQWINDOWSSTYLE_H

#ifndef QT_H
#include "ntqcommonstyle.h"
#endif // QT_H

#if !defined(TQT_NO_STYLE_WINDOWS) || defined(QT_PLUGIN)

#if defined(QT_PLUGIN)
#define TQ_EXPORT_STYLE_WINDOWS
#else
#define TQ_EXPORT_STYLE_WINDOWS TQ_EXPORT
#endif


class TQ_EXPORT_STYLE_WINDOWS TQWindowsStyle : public TQCommonStyle
{
    TQ_OBJECT
public:
    TQWindowsStyle();
    ~TQWindowsStyle();

    void applicationPolish(const TQStyleControlElementData &ceData, ControlElementFlags elementFlags, void *);
    void applicationUnPolish(const TQStyleControlElementData &ceData, ControlElementFlags elementFlags, void *);

    void polish(const TQStyleControlElementData &ceData, ControlElementFlags elementFlags, void *);
    void unPolish(const TQStyleControlElementData &ceData, ControlElementFlags elementFlags, void *);

    void polish( TQPalette & );

    virtual void polishPopupMenu( const TQStyleControlElementData &ceData, ControlElementFlags elementFlags, void * );

    // new stuff
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
		      SFlags flags = Style_Default,
		      const TQStyleOption& = TQStyleOption::Default,
		      const TQWidget *widget = 0 ) const;

    void drawComplexControl( ComplexControl control,
			     TQPainter* p,
			     const TQStyleControlElementData &ceData,
			     ControlElementFlags elementFlags,
			     const TQRect& r,
			     const TQColorGroup& cg,
			     SFlags flags = Style_Default,
#ifdef Q_QDOC
			     SCFlags sub = SC_All,
#else
			     SCFlags sub = (uint)SC_All,
#endif
			     SCFlags subActive = SC_None,
			     const TQStyleOption& = TQStyleOption::Default,
			     const TQWidget* widget = 0 ) const;

    int pixelMetric( PixelMetric metric,
		     const TQStyleControlElementData &ceData,
		     ControlElementFlags elementFlags,
		     const TQWidget *widget = 0 ) const;

    TQSize sizeFromContents( ContentsType contents,
			    const TQStyleControlElementData &ceData,
			    ControlElementFlags elementFlags,
			    const TQSize &contentsSize,
			    const TQStyleOption& = TQStyleOption::Default,
			    const TQWidget *widget = 0 ) const;

    int styleHint(StyleHint sh, const TQStyleControlElementData &ceData, ControlElementFlags elementFlags, const TQStyleOption & = TQStyleOption::Default,
		  TQStyleHintReturn* = 0, const TQWidget* = 0) const;

    TQPixmap stylePixmap( StylePixmap stylepixmap,
			 const TQStyleControlElementData &ceData,
			 ControlElementFlags elementFlags,
			 const TQStyleOption& = TQStyleOption::Default,
			 const TQWidget *widget = 0 ) const;

    TQRect subRect( SubRect r, const TQStyleControlElementData &ceData, const ControlElementFlags elementFlags, const TQWidget *widget ) const;

    bool objectEventHandler( const TQStyleControlElementData &ceData, ControlElementFlags elementFlags, void* source, TQEvent *e );

private:
    class Private;
    Private *d;

    // Disabled copy constructor and operator=
#if defined(TQ_DISABLE_COPY)
    TQWindowsStyle( const TQWindowsStyle & );
    TQWindowsStyle& operator=( const TQWindowsStyle & );
#endif
};

#endif // TQT_NO_STYLE_WINDOWS

#endif // TQWINDOWSSTYLE_H
