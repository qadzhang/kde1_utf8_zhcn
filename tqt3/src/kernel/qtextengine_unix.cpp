/****************************************************************************
**
** Text engine classes
**
** Copyright (C) 2005-2008 Trolltech ASA.  All rights reserved.
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

#include <assert.h>


TQScriptItemArray::~TQScriptItemArray()
{
    clear();
    free( d );
}

void TQScriptItemArray::clear()
{
    if ( d ) {
	for ( unsigned int i = 0; i < d->size; i++ ) {
	    TQScriptItem &si = d->items[i];
	    if ( si.fontEngine )
		si.fontEngine->deref();
	}
	d->size = 0;
    }
}

void TQScriptItemArray::resize( int s )
{
    int alloc = (s + 8) >> 3 << 3;
    d = (TQScriptItemArrayPrivate *)realloc( d, sizeof( TQScriptItemArrayPrivate ) +
		 sizeof( TQScriptItem ) * alloc );
    d->alloc = alloc;
}

void TQTextEngine::shape( int item ) const
{
    assert( item < items.size() );
    TQScriptItem &si = items[item];

    if ( si.num_glyphs )
	return;

    TQFont::Script script = (TQFont::Script)si.analysis.script;
    si.glyph_data_offset = used;

    if ( !si.fontEngine )
	si.fontEngine = fnt->engineForScript( script );
    si.fontEngine->ref();

    si.ascent = si.fontEngine->ascent();
    si.descent = si.fontEngine->descent();
    si.num_glyphs = 0;

    if ( si.fontEngine && si.fontEngine != (TQFontEngine*)-1 ) {
        TQShaperItem shaper_item;
        shaper_item.script = si.analysis.script;
        shaper_item.string = &string;
        shaper_item.from = si.position;
        shaper_item.length = length(item);
        shaper_item.font = si.fontEngine;
        shaper_item.num_glyphs = TQMAX(int(num_glyphs - used), shaper_item.length);
        shaper_item.flags = si.analysis.bidiLevel % 2 ? RightToLeft : 0;
        shaper_item.has_positioning = false;

        while (1) {
            ensureSpace(shaper_item.num_glyphs);
            shaper_item.num_glyphs = num_glyphs - used;
//          tqDebug("    .. num_glyphs=%d, used=%d, item.num_glyphs=%d", num_glyphs, used, shaper_item.num_glyphs);
            shaper_item.glyphs = glyphs(&si);
            shaper_item.advances = advances(&si);
            shaper_item.offsets = offsets(&si);
            shaper_item.attributes = glyphAttributes(&si);
            shaper_item.log_clusters = logClusters(&si);
            if (scriptEngines[shaper_item.script].shape(&shaper_item))
                break;
        }

        si.num_glyphs = shaper_item.num_glyphs;
        si.hasPositioning = shaper_item.has_positioning;
    }
    ((TQTextEngine *)this)->used += si.num_glyphs;

    si.width = 0;
    advance_t *advances = this->advances( &si );
    advance_t *end = advances + si.num_glyphs;
    while ( advances < end ) {
//         tqDebug("advances[%d] = %d", advances - this->advances(&si), *advances);
	si.width += *(advances++);
    }

    return;
}

