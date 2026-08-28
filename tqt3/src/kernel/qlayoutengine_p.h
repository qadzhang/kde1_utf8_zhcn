/****************************************************************************
**
** Internal header file.
**
** Created : 981027
**
** Copyright (C) 1998-2008 Trolltech ASA.  All rights reserved.
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

#ifndef TQLAYOUTENGINE_P_H
#define TQLAYOUTENGINE_P_H

#ifndef TQLAYOUT_H
    #error "Need to include ntqlayout.h before including qlayoutengine_p.h"
#endif

//
//  W A R N I N G
//  -------------
//
// This file is not part of the TQt API.  It exists for the convenience
// of qlayout.cpp, qlayoutengine.cpp, qmainwindow.cpp and qsplitter.cpp.
// This header file may change from version to version without notice,
// or even be removed.
//
// We mean it.
//
//


#ifndef QT_H
#include "ntqabstractlayout.h"
#endif // QT_H

#ifndef TQT_NO_LAYOUT

struct TQLayoutStruct
{
    inline void init( int stretchFactor = 0, int spacing = 0 ) {
	stretch = stretchFactor;
	minimumSize = sizeHint = spacing;
	maximumSize = TQLAYOUTSIZE_MAX;
	expansive = false;
	empty = true;
    }

    TQCOORD smartSizeHint() {
	return ( stretch > 0 ) ? minimumSize : sizeHint;
    }

    // parameters
    int stretch;
    TQCOORD sizeHint;
    TQCOORD maximumSize;
    TQCOORD minimumSize;
    bool expansive;
    bool empty;

    // temporary storage
    bool done;

    // result
    int pos;
    int size;
};


TQ_EXPORT void qGeomCalc( TQMemArray<TQLayoutStruct> &chain, int start, int count,
			 int pos, int space, int spacer );
TQ_EXPORT TQSize qSmartMinSize( const TQWidgetItem *i );
TQ_EXPORT TQSize qSmartMinSize( TQWidget *w );
TQ_EXPORT TQSize qSmartMaxSize( const TQWidgetItem *i, int align = 0 );
TQ_EXPORT TQSize qSmartMaxSize( TQWidget *w, int align = 0 );


/*
  Modify total maximum (max) and total expansion (exp)
  when adding boxmax/boxexp.

  Expansive boxes win over non-expansive boxes.
*/
static inline void qMaxExpCalc( TQCOORD & max, bool &exp,
				TQCOORD boxmax, bool boxexp )
{
    if ( exp ) {
	if ( boxexp )
	    max = TQMAX( max, boxmax );
    } else {
	if ( boxexp )
	    max = boxmax;
	else
	    max = TQMIN( max, boxmax );
    }
    exp = exp || boxexp;
}

#endif //TQT_NO_LAYOUT
#endif
