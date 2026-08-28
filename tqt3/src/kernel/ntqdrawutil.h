/****************************************************************************
**
** Definition of draw utilities
**
** Created : 950920
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

#ifndef TQDRAWUTIL_H
#define TQDRAWUTIL_H

#ifndef QT_H
#include "ntqnamespace.h"
#include "ntqstring.h" // char*->TQString conversion
#endif // QT_H

class TQPainter;
class TQColorGroup;
class TQPoint;
class TQBrush;
class TQRect;
class TQPixmap;

#ifndef TQT_NO_DRAWUTIL
//
// Standard shade drawing
//

TQ_EXPORT void qDrawShadeLine( TQPainter *p, int x1, int y1, int x2, int y2,
			      const TQColorGroup &g, bool sunken = true,
			      int lineWidth = 1, int midLineWidth = 0 );

TQ_EXPORT void qDrawShadeLine( TQPainter *p, const TQPoint &p1, const TQPoint &p2,
			      const TQColorGroup &g, bool sunken = true,
			      int lineWidth = 1, int midLineWidth = 0 );

TQ_EXPORT void qDrawShadeRect( TQPainter *p, int x, int y, int w, int h,
			      const TQColorGroup &, bool sunken=false,
			      int lineWidth = 1, int midLineWidth = 0,
			      const TQBrush *fill = 0 );

TQ_EXPORT void qDrawShadeRect( TQPainter *p, const TQRect &r,
			      const TQColorGroup &, bool sunken=false,
			      int lineWidth = 1, int midLineWidth = 0,
			      const TQBrush *fill = 0 );

TQ_EXPORT void qDrawShadePanel( TQPainter *p, int x, int y, int w, int h,
			       const TQColorGroup &, bool sunken=false,
			       int lineWidth = 1, const TQBrush *fill = 0 );

TQ_EXPORT void qDrawShadePanel( TQPainter *p, const TQRect &r,
			       const TQColorGroup &, bool sunken=false,
			       int lineWidth = 1, const TQBrush *fill = 0 );

TQ_EXPORT void qDrawWinButton( TQPainter *p, int x, int y, int w, int h,
			      const TQColorGroup &g, bool sunken = false,
			      const TQBrush *fill = 0 );

TQ_EXPORT void qDrawWinButton( TQPainter *p, const TQRect &r,
			      const TQColorGroup &g, bool sunken = false,
			      const TQBrush *fill = 0 );

TQ_EXPORT void qDrawWinPanel( TQPainter *p, int x, int y, int w, int h,
			     const TQColorGroup &, bool sunken=false,
			     const TQBrush *fill = 0 );

TQ_EXPORT void qDrawWinPanel( TQPainter *p, const TQRect &r,
			     const TQColorGroup &, bool sunken=false,
			     const TQBrush *fill = 0 );

TQ_EXPORT void qDrawPlainRect( TQPainter *p, int x, int y, int w, int h, const TQColor &,
			      int lineWidth = 1, const TQBrush *fill = 0 );

TQ_EXPORT void qDrawPlainRect( TQPainter *p, const TQRect &r, const TQColor &,
			      int lineWidth = 1, const TQBrush *fill = 0 );


//
// Other obsolete drawing functions.
// Use TQStyle::itemRect(), TQStyle::drawItem() and TQStyle::drawArrow() instead.
//
TQ_EXPORT TQRect qItemRect( TQPainter *p, TQt::GUIStyle gs, int x, int y, int w, int h,
			  int flags, bool enabled,
			  const TQPixmap *pixmap, const TQString& text, int len=-1 );

TQ_EXPORT void qDrawItem( TQPainter *p, TQt::GUIStyle gs, int x, int y, int w, int h,
			 int flags, const TQColorGroup &g, bool enabled,
			 const TQPixmap *pixmap, const TQString& text,
			 int len=-1, const TQColor* penColor = 0 );

TQ_EXPORT void qDrawArrow( TQPainter *p, TQt::ArrowType type, TQt::GUIStyle style, bool down,
			  int x, int y, int w, int h,
			  const TQColorGroup &g, bool enabled );

#endif // TQT_NO_DRAWUTIL
#endif // TQDRAWUTIL_H
