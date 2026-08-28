/****************************************************************************
**
** ???
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

#include "qtextlayout_p.h"
#include "qtextengine_p.h"

#include <ntqfont.h>
#include <ntqapplication.h>
#include <ntqpainter.h>


TQRect TQTextItem::rect() const
{
    TQScriptItem& si = engine->items[item];
    return TQRect( si.x, si.y, si.width, si.ascent+si.descent );
}

int TQTextItem::x() const
{
    return engine->items[item].x;
}

int TQTextItem::y() const
{
    return engine->items[item].y;
}

int TQTextItem::width() const
{
    return engine->items[item].width;
}

int TQTextItem::ascent() const
{
    return engine->items[item].ascent;
}

int TQTextItem::descent() const
{
    return engine->items[item].descent;
}

void TQTextItem::setWidth( int w )
{
    engine->items[item].width = w;
}

void TQTextItem::setAscent( int a )
{
    engine->items[item].ascent = a;
}

void TQTextItem::setDescent( int d )
{
    engine->items[item].descent = d;
}

int TQTextItem::from() const
{
    return engine->items[item].position;
}

int TQTextItem::length() const
{
    return engine->length(item);
}


int TQTextItem::cursorToX( int *cPos, Edge edge ) const
{
    int pos = *cPos;
    TQScriptItem *si = &engine->items[item];

    engine->shape( item );
    advance_t *advances = engine->advances( si );
    GlyphAttributes *glyphAttributes = engine->glyphAttributes( si );
    unsigned short *logClusters = engine->logClusters( si );

    int l = engine->length( item );
    if ( pos > l )
	pos = l;
    if ( pos < 0 )
	pos = 0;

    int glyph_pos = pos == l ? si->num_glyphs : logClusters[pos];
    if ( edge == Trailing ) {
	// trailing edge is leading edge of next cluster
	while ( glyph_pos < si->num_glyphs && !glyphAttributes[glyph_pos].clusterStart )
	    glyph_pos++;
    }

    int x = 0;
    bool reverse = engine->items[item].analysis.bidiLevel % 2;

    if ( reverse ) {
	for ( int i = si->num_glyphs-1; i >= glyph_pos; i-- )
	    x += advances[i];
    } else {
	for ( int i = 0; i < glyph_pos; i++ )
	    x += advances[i];
    }
//     tqDebug("cursorToX: pos=%d, gpos=%d x=%d", pos, glyph_pos, x );
    *cPos = pos;
    return x;
}

int TQTextItem::xToCursor( int x, CursorPosition cpos ) const
{
    TQScriptItem *si = &engine->items[item];
    engine->shape( item );
    advance_t *advances = engine->advances( si );
    unsigned short *logClusters = engine->logClusters( si );

    int l = engine->length( item );
    bool reverse = si->analysis.bidiLevel % 2;
    if ( x < 0 )
	return reverse ? l : 0;


    if ( reverse ) {
	int width = 0;
	for ( int i = 0; i < si->num_glyphs; i++ ) {
	    width += advances[i];
	}
	x = -x + width;
    }
    int cp_before = 0;
    int cp_after = 0;
    int x_before = 0;
    int x_after = 0;

    int lastCluster = 0;
    for ( int i = 1; i <= l; i++ ) {
	int newCluster = i < l ? logClusters[i] : si->num_glyphs;
	if ( newCluster != lastCluster ) {
	    // calculate cluster width
	    cp_before = cp_after;
	    x_before = x_after;
	    cp_after = i;
	    for ( int j = lastCluster; j < newCluster; j++ )
		x_after += advances[j];
	    // 		tqDebug("cluster boundary: lastCluster=%d, newCluster=%d, x_before=%d, x_after=%d",
	    // 		       lastCluster, newCluster, x_before, x_after );
	    if ( x_after > x )
		break;
	    lastCluster = newCluster;
	}
    }

    bool before = ( cpos == OnCharacters || (x - x_before) < (x_after - x) );

//     tqDebug("got cursor position for %d: %d/%d, x_ba=%d/%d using %d",
// 	   x, cp_before,cp_after,  x_before, x_after,  before ? cp_before : cp_after );

    return before ? cp_before : cp_after;

}


bool TQTextItem::isRightToLeft() const
{
    return (engine->items[item].analysis.bidiLevel % 2);
}

bool TQTextItem::isObject() const
{
    return engine->items[item].isObject;
}

bool TQTextItem::isSpace() const
{
    return engine->items[item].isSpace;
}

bool TQTextItem::isTab() const
{
    return engine->items[item].isTab;
}


TQTextLayout::TQTextLayout()
    :d(0) {}

TQTextLayout::TQTextLayout( const TQString& string, TQPainter *p )
{
    TQFontPrivate *f = p ? ( p->pfont ? p->pfont->d : p->cfont.d ) : TQApplication::font().d;
    d = new TQTextEngine( (string.isNull() ? (const TQString&)TQString::fromLatin1("") : string), f );
}

TQTextLayout::TQTextLayout( const TQString& string, const TQFont& fnt )
{
    d = new TQTextEngine( (string.isNull() ? (const TQString&)TQString::fromLatin1("") : string), fnt.d );
}

TQTextLayout::~TQTextLayout()
{
    delete d;
}

void TQTextLayout::setText( const TQString& string, const TQFont& fnt )
{
    delete d;
    d = new TQTextEngine( (string.isNull() ? (const TQString&)TQString::fromLatin1("") : string), fnt.d );
}

/* add an additional item boundary eg. for style change */
void TQTextLayout::setBoundary( int strPos )
{
    if ( strPos <= 0 || strPos >= (int)d->string.length() )
	return;

    int itemToSplit = 0;
    while ( itemToSplit < d->items.size() && d->items[itemToSplit].position <= strPos )
	itemToSplit++;
    itemToSplit--;
    if ( d->items[itemToSplit].position == strPos ) {
	// already a split at the requested position
	return;
    }
    d->splitItem( itemToSplit, strPos - d->items[itemToSplit].position );
}


int TQTextLayout::numItems() const
{
    return d->items.size();
}

TQTextItem TQTextLayout::itemAt( int i ) const
{
    return TQTextItem( i, d );
}


TQTextItem TQTextLayout::findItem( int strPos ) const
{
    if ( strPos == 0 && d->items.size() )
	return TQTextItem( 0, d );
    // ## TODO use bsearch
    for ( int i = d->items.size()-1; i >= 0; --i ) {
	if ( d->items[i].position < strPos )
	    return TQTextItem( i, d );
    }
    return TQTextItem();
}


void TQTextLayout::beginLayout( TQTextLayout::LayoutMode m )
{
    d->items.clear();
    TQTextEngine::Mode mode = TQTextEngine::Full;
    if (m == NoBidi)
	mode = TQTextEngine::NoBidi;
    else if (m == SingleLine)
	mode = TQTextEngine::SingleLine;
    d->itemize( mode );
    d->currentItem = 0;
    d->firstItemInLine = -1;
}

void TQTextLayout::beginLine( int width )
{
    d->lineWidth = width;
    d->widthUsed = 0;
    d->firstItemInLine = -1;
}

bool TQTextLayout::atEnd() const
{
    return d->currentItem >= d->items.size();
}

TQTextItem TQTextLayout::nextItem()
{
    d->currentItem++;

    if ( d->currentItem >= d->items.size() )
	return TQTextItem();

    d->shape( d->currentItem );
    return TQTextItem( d->currentItem, d );
}

TQTextItem TQTextLayout::currentItem()
{
    if ( d->currentItem >= d->items.size() )
	return TQTextItem();

    d->shape( d->currentItem );
    return TQTextItem( d->currentItem, d );
}

/* ## maybe also currentItem() */
void TQTextLayout::setLineWidth( int newWidth )
{
    d->lineWidth = newWidth;
}

int TQTextLayout::lineWidth() const
{
    return d->lineWidth;
}

int TQTextLayout::widthUsed() const
{
    return d->widthUsed;
}

int TQTextLayout::availableWidth() const
{
    return d->lineWidth - d->widthUsed;
}


/* returns true if completely added */
TQTextLayout::Result TQTextLayout::addCurrentItem()
{
    if ( d->firstItemInLine == -1 )
	d->firstItemInLine = d->currentItem;
    TQScriptItem &current = d->items[d->currentItem];
    d->shape( d->currentItem );
    d->widthUsed += current.width;
//     tqDebug("trying to add item %d with width %d, remaining %d", d->currentItem, current.width, d->lineWidth-d->widthUsed );

    d->currentItem++;

    return (d->widthUsed <= d->lineWidth
	    || (d->currentItem < d->items.size() && d->items[d->currentItem].isSpace)) ? Ok : LineFull;
}

TQTextLayout::Result TQTextLayout::endLine( int x, int y, int alignment,
					  int *ascent, int *descent, int *lineLeft, int *lineRight )
{
    int available = d->lineWidth;
    int numRuns = 0;
    int numSpaceItems = 0;
    TQ_UINT8 _levels[128];
    int _visual[128];
    TQ_UINT8 *levels = _levels;
    int *visual = _visual;
    int i;
    TQTextLayout::Result result = LineEmpty;

//    tqDebug("endLine x=%d, y=%d, first=%d, current=%d lw=%d wu=%d", x,  y, d->firstItemInLine, d->currentItem, d->lineWidth, d->widthUsed );
    int width_nobreak_found = d->widthUsed;
    if ( d->firstItemInLine == -1 )
	goto end;

    if ( !(alignment & (TQt::SingleLine|TQt::IncludeTrailingSpaces))
	&& d->currentItem > d->firstItemInLine && d->items[d->currentItem-1].isSpace ) {
	int i = d->currentItem-1;
	while ( i > d->firstItemInLine && d->items[i].isSpace ) {
	    numSpaceItems++;
	    d->widthUsed -= d->items[i--].width;
	}
    }

    if ( (alignment & (TQt::WordBreak|TQt::BreakAnywhere)) &&
	 d->widthUsed > d->lineWidth ) {
	// find linebreak

	// even though we removed trailing spaces the line was too wide. We'll have to break at an earlier
	// position. To not confuse the layouting below, reset the number of space items
	numSpaceItems = 0;


	bool breakany = alignment & TQt::BreakAnywhere;

	const TQCharAttributes *attrs = d->attributes();
	int w = 0;
	int itemWidth = 0;
	int breakItem = d->firstItemInLine;
	int breakPosition = -1;
#if 0
	// we iterate backwards or forward depending on what we guess is closer
	if ( d->widthUsed - d->lineWidth < d->lineWidth ) {
	    // backwards search should be faster

	} else
#endif
	{
	    int tmpWidth = 0;
	    int swidth = 0;
	    // forward search is probably faster
	    for ( int i = d->firstItemInLine; i < d->currentItem; i++ ) {
		const TQScriptItem *si = &d->items[i];
		int length = d->length( i );
		const TQCharAttributes *itemAttrs = attrs + si->position;

		advance_t *advances = d->advances( si );
		unsigned short *logClusters = d->logClusters( si );

		int lastGlyph = 0;
		int tmpItemWidth = 0;

//     		tqDebug("looking for break in item %d, isSpace=%d", i, si->isSpace );
		if(si->isSpace && !(alignment & (TQt::SingleLine|TQt::IncludeTrailingSpaces))) {
		    swidth += si->width;
		} else {
		    tmpWidth += swidth;
		    swidth = 0;
		    for ( int pos = 0; pos < length; pos++ ) {
//  			tqDebug("advance=%d, w=%d, tmpWidth=%d, softbreak=%d, whitespace=%d",
//                                *advances, w, tmpWidth, itemAttrs->softBreak, itemAttrs->whiteSpace );
			int glyph = logClusters[pos];
			if ( lastGlyph != glyph ) {
			    while ( lastGlyph < glyph )
				tmpItemWidth += advances[lastGlyph++];
			    if ( breakPosition != -1 && w + tmpWidth + tmpItemWidth > d->lineWidth ) {
// 				tqDebug("found break at w=%d, tmpWidth=%d, tmpItemWidth=%d", w, tmpWidth, tmpItemWidth);
				d->widthUsed = w;
				goto found;
			    }
			}
			if ( (itemAttrs->softBreak ||
			      ( breakany && itemAttrs->charStop ) ) &&
			     (i != d->firstItemInLine || pos != 0) ) {
			    if ( breakItem != i )
				itemWidth = 0;
			    if (itemAttrs->softBreak)
				breakany = false;
			    breakItem = i;
			    breakPosition = pos;
//     			    tqDebug("found possible break at item %d, position %d (absolute=%d), w=%d, tmpWidth=%d, tmpItemWidth=%d", breakItem, breakPosition, d->items[breakItem].position+breakPosition, w, tmpWidth, tmpItemWidth);
			    w += tmpWidth + tmpItemWidth;
			    itemWidth += tmpItemWidth;
			    tmpWidth = 0;
			    tmpItemWidth = 0;
			}
			itemAttrs++;
		    }
		    while ( lastGlyph < si->num_glyphs )
			tmpItemWidth += advances[lastGlyph++];
		    tmpWidth += tmpItemWidth;
		    if ( w + tmpWidth > d->lineWidth ) {
			d->widthUsed = w;
			goto found;
		    }
		}
	    }
	}

    found:
	// no valid break point found
	if ( breakPosition == -1 ) {
            d->widthUsed = width_nobreak_found;
	    goto nobreak;
        }

//   	tqDebug("linebreak at item %d, position %d, wu=%d", breakItem, breakPosition, d->widthUsed );
	// split the line
	if ( breakPosition > 0 ) {
// 	    int length = d->length( breakItem );

//   	    tqDebug("splitting item, itemWidth=%d", itemWidth);
	    // not a full item, need to break
	    d->splitItem( breakItem, breakPosition );
	    d->currentItem = breakItem+1;
	} else {
	    d->currentItem = breakItem;
	}
    }

    result = Ok;

 nobreak:
    // position the objects in the line
    available -= d->widthUsed;

    numRuns = d->currentItem - d->firstItemInLine - numSpaceItems;
    if ( numRuns > 127 ) {
	levels = new TQ_UINT8[numRuns];
	visual = new int[numRuns];
    }

//     tqDebug("reordering %d runs, numSpaceItems=%d", numRuns, numSpaceItems );
    for ( i = 0; i < numRuns; i++ ) {
	levels[i] = d->items[i+d->firstItemInLine].analysis.bidiLevel;
// 	tqDebug("    level = %d", d->items[i+d->firstItemInLine].analysis.bidiLevel );
    }
    d->bidiReorder( numRuns, levels, visual );

 end:
    // ### FIXME
    if ( alignment & TQt::AlignJustify ) {
	// #### justify items
	alignment = TQt::AlignAuto;
    }
    if ( (alignment & TQt::AlignHorizontal_Mask) == TQt::AlignAuto )
	alignment = TQt::AlignLeft;
    if ( alignment & TQt::AlignRight )
	x += available;
    else if ( alignment & TQt::AlignHCenter )
	x += available/2;


    int asc = ascent ? *ascent : 0;
    int desc = descent ? *descent : 0;

    for ( i = 0; i < numRuns; i++ ) {
	TQScriptItem &si = d->items[d->firstItemInLine+visual[i]];
	asc = TQMAX( asc, si.ascent );
	desc = TQMAX( desc, si.descent );
    }

    int left = x;
    for ( i = 0; i < numRuns; i++ ) {
	TQScriptItem &si = d->items[d->firstItemInLine+visual[i]];
//  	tqDebug("positioning item %d with width %d (from=%d/length=%d) at %d", d->firstItemInLine+visual[i], si.width, si.position,
// 	       d->length(d->firstItemInLine+visual[i]), x );
	si.x = x;
	si.y = y + asc;
	x += si.width;
    }
    int right = x;

    if ( numSpaceItems ) {
	if ( d->items[d->firstItemInLine+numRuns].analysis.bidiLevel % 2 ) {
	    x = left;
	    for ( i = 0; i < numSpaceItems; i++ ) {
		TQScriptItem &si = d->items[d->firstItemInLine + numRuns + i];
		x -= si.width;
		si.x = x;
		si.y = y + asc;
	    }
	} else {
	    for ( i = 0; i < numSpaceItems; i++ ) {
		TQScriptItem &si = d->items[d->firstItemInLine + numRuns + i];
		si.x = x;
		si.y = y + asc;
		x += si.width;
	    }
	}
    }

    if ( lineLeft )
	*lineLeft = left;
    if ( lineRight )
	*lineRight = right;
    if ( ascent )
	*ascent = asc;
    if ( descent )
	*descent = desc;

    if (levels != _levels)
	delete []levels;
    if (visual != _visual)
	delete []visual;

    return result;
}

void TQTextLayout::endLayout()
{
    // nothing to do currently
}


int TQTextLayout::nextCursorPosition( int oldPos, CursorMode mode ) const
{
//     tqDebug("looking for next cursor pos for %d", oldPos );
    const TQCharAttributes *attributes = d->attributes();
    int len = d->string.length();
    if ( oldPos >= len )
	return oldPos;
    oldPos++;
    if ( mode == SkipCharacters ) {
	while ( oldPos < len && !attributes[oldPos].charStop )
	    oldPos++;
    } else {
	while ( oldPos < len && !attributes[oldPos].wordStop && !attributes[oldPos-1].whiteSpace )
	    oldPos++;
    }
//     tqDebug("  -> %d",  oldPos );
    return oldPos;
}

int TQTextLayout::previousCursorPosition( int oldPos, CursorMode mode ) const
{
//     tqDebug("looking for previous cursor pos for %d", oldPos );
    const TQCharAttributes *attributes = d->attributes();
    if ( oldPos <= 0 )
	return 0;
    oldPos--;
    if ( mode == SkipCharacters ) {
	while ( oldPos && !attributes[oldPos].charStop )
	    oldPos--;
    } else {
	while ( oldPos && !attributes[oldPos].wordStop && !attributes[oldPos-1].whiteSpace )
	    oldPos--;
    }
//     tqDebug("  -> %d",  oldPos );
    return oldPos;
}


bool TQTextLayout::validCursorPosition( int pos ) const
{
    const TQCharAttributes *attributes = d->attributes();
    if ( pos < 0 || pos > (int)d->string.length() )
	return false;
    return attributes[pos].charStop;
}

void TQTextLayout::setDirection(TQChar::Direction dir)
{
    d->direction = dir;
}
