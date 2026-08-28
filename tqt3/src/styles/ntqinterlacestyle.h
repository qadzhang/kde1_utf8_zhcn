/****************************************************************************
**
** Implementation of TQInterlaceStyle widget class
**
** Created : 010122
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

#if 0 // ###### not ported to new API yet
#ifndef TQINTERLACESTYLE_H
#define TQINTERLACESTYLE_H

#ifndef QT_H
#include "ntqmotifstyle.h"
#endif // QT_H

#if !defined(TQT_NO_STYLE_INTERLACE) || defined(QT_PLUGIN)

#include "ntqpalette.h"

class TQ_EXPORT TQInterlaceStyle : public TQMotifStyle
{
public:
    TQInterlaceStyle();
    void polish( TQApplication*);
    void unPolish( TQApplication*);
    void polish( TQWidget* );
    void unPolish( TQWidget* );

    int defaultFrameWidth() const;
    TQRect pushButtonContentsRect( TQPushButton *btn );

    void drawFocusRect ( TQPainter *, const TQRect &, const TQColorGroup &, const TQColor * bg = 0, bool = false );
    void drawButton( TQPainter *p, int x, int y, int w, int h,
			     const TQColorGroup &g, bool sunken = false,
			     const TQBrush *fill = 0 );
    void drawButtonMask ( TQPainter * p, int x, int y, int w, int h );
    void drawBevelButton( TQPainter *p, int x, int y, int w, int h,
			  const TQColorGroup &g, bool sunken = false,
			  const TQBrush *fill = 0 );

    void drawPushButton( TQPushButton* btn, TQPainter *p);
    TQSize indicatorSize () const;
    void drawIndicator ( TQPainter * p, int x, int y, int w, int h, const TQColorGroup & g, int state, bool down = false, bool enabled = true );
    void drawIndicatorMask( TQPainter *p, int x, int y, int w, int h, int );
    TQSize exclusiveIndicatorSize () const;
    void drawExclusiveIndicator( TQPainter * p, int x, int y, int w, int h, const TQColorGroup & g, bool on, bool down = false, bool enabled = true );
    void drawExclusiveIndicatorMask( TQPainter * p, int x, int y, int w, int h, bool );
    TQRect comboButtonRect ( int x, int y, int w, int h );
    void drawComboButton( TQPainter *p, int x, int y, int w, int h, const TQColorGroup &g, bool sunken, bool editable, bool enabled, const TQBrush *fb );
    void drawPushButtonLabel( TQPushButton* btn, TQPainter *p);
    void drawPanel( TQPainter *p, int x, int y, int w, int h,
		    const TQColorGroup &, bool sunken,
		    int lineWidth, const TQBrush *fill );

    void scrollBarMetrics( const TQScrollBar* sb, int &sliderMin, int &sliderMax, int &sliderLength, int &buttonDim );
    void drawScrollBarControls( TQPainter* p, const TQScrollBar* sb, int sliderStart, uint controls, uint activeControl );
    void drawSlider( TQPainter * p, int x, int y, int w, int h, const TQColorGroup & g, Orientation, bool tickAbove, bool tickBelow );
    void drawSliderGroove( TQPainter * p, int x, int y, int w, int h, const TQColorGroup & g, TQCOORD c, Orientation );
    int splitterWidth() const;
    void drawSplitter( TQPainter *p, int x, int y, int w, int h,
		      const TQColorGroup &g, Orientation orient);

    int buttonDefaultIndicatorWidth() const;
    int setSliderThickness() const;
    TQSize scrollBarExtent() const;

private:
    TQPalette oldPalette;
};

#endif // TQT_NO_STYLE_INTERLACE

#endif
#endif
