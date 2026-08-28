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

#include "qtextengine_p.h"

#include "qscriptengine_p.h"
#include <ntqfont.h>
#include "qfontdata_p.h"
#include "qfontengine_p.h"
#include <ntqstring.h>
#include <private/qunicodetables_p.h>
#include <stdlib.h>

// -----------------------------------------------------------------------------------------------------
//
// The BiDi algorithm
//
// -----------------------------------------------------------------------------------------------------


#define BIDI_DEBUG 0//2
#if (BIDI_DEBUG >= 1)
#include <iostream>
using namespace std;

static const char *directions[] = {
    "DirL", "DirR", "DirEN", "DirES", "DirET", "DirAN", "DirCS", "DirB", "DirS", "DirWS", "DirON",
    "DirLRE", "DirLRO", "DirAL", "DirRLE", "DirRLO", "DirPDF", "DirNSM", "DirBN"
};

#endif

struct BidiStatus {
    BidiStatus() {
	eor = TQChar::DirON;
	lastStrong = TQChar::DirON;
	last = TQChar:: DirON;
	dir = TQChar::DirON;
    }
    TQChar::Direction eor;
    TQChar::Direction lastStrong;
    TQChar::Direction last;
    TQChar::Direction dir;
};

struct BidiControl {
    struct Context {
	unsigned char level : 6;
	unsigned char override : 1;
	unsigned char unused : 1;
    };

    inline BidiControl( bool rtl )
	: cCtx( 0 ), singleLine( false ) {
	ctx[0].level = (rtl ? 1 : 0);
	ctx[0].override = false;
    }

    inline void embed( int level, bool override = false ) {
	if ( ctx[cCtx].level < 61 && cCtx < 61 ) {
	    (void) ++cCtx;
	    ctx[cCtx].level = level;
	    ctx[cCtx].override = override;
	}
    }
    inline void pdf() {
	if ( cCtx ) (void) --cCtx;
    }

    inline uchar level() const {
	return ctx[cCtx].level;
    }
    inline bool override() const {
	return ctx[cCtx].override;
    }
    inline TQChar::Direction basicDirection() {
	return (ctx[0].level ? TQChar::DirR : TQChar:: DirL );
    }
    inline uchar baseLevel() {
	return ctx[0].level;
    }
    inline TQChar::Direction direction() {
	return ((ctx[cCtx].level%2) ? TQChar::DirR : TQChar:: DirL );
    }

    Context ctx[63];
    unsigned int cCtx : 8;
    bool singleLine : 8;
};

static TQChar::Direction basicDirection( const TQString &str )
{
    int len = str.length();
    int pos = 0;
    const TQChar *uc = str.unicode() + pos;
    while( pos < len ) {
	switch( direction( *uc ) )
	{
	case TQChar::DirL:
	case TQChar::DirLRO:
	case TQChar::DirLRE:
	    return TQChar::DirL;
	case TQChar::DirR:
	case TQChar::DirAL:
	case TQChar::DirRLO:
	case TQChar::DirRLE:
	    return TQChar::DirR;
	default:
	    break;
	}
	++pos;
	++uc;
    }
    return TQChar::DirL;
}


static void appendItems(TQTextEngine *engine, int &start, int &stop, BidiControl &control, TQChar::Direction dir )
{
    TQScriptItemArray &items = engine->items;
    const TQChar *text = engine->string.unicode();

    if ( start > stop ) {
	// #### the algorithm is currently not really safe against this. Still needs fixing.
// 	tqWarning( "Bidi: appendItems() internal error" );
	return;
    }

    int level = control.level();

    if(dir != TQChar::DirON && !control.override()) {
	// add level of run (cases I1 & I2)
	if( level % 2 ) {
	    if(dir == TQChar::DirL || dir == TQChar::DirAN || dir == TQChar::DirEN )
		level++;
	} else {
	    if( dir == TQChar::DirR )
		level++;
	    else if( dir == TQChar::DirAN || dir == TQChar::DirEN )
		level += 2;
	}
    }

#if (BIDI_DEBUG >= 1)
    tqDebug("new run: dir=%s from %d, to %d level = %d\n", directions[dir], start, stop, level);
#endif
    TQFont::Script script = TQFont::NoScript;
    TQScriptItem item;
    item.position = start;
    item.analysis.script = script;
    item.analysis.bidiLevel = level;
    item.analysis.override = control.override();
    item.analysis.reserved = 0;

    if ( control.singleLine ) {
	for ( int i = start; i <= stop; i++ ) {

	    unsigned short uc = text[i].unicode();
	    TQFont::Script s = (TQFont::Script)scriptForChar( uc );
	    if (s == TQFont::UnknownScript || s == TQFont::CombiningMarks)
		s = script;

	    if (s != script) {
		item.analysis.script = s;
		item.analysis.bidiLevel = level;
		item.position = i;
		items.append( item );
		script = s;
	    }
	}
    } else {
	for ( int i = start; i <= stop; i++ ) {

	    unsigned short uc = text[i].unicode();
	    TQFont::Script s = (TQFont::Script)scriptForChar( uc );
	    if (s == TQFont::UnknownScript || s == TQFont::CombiningMarks)
		s = script;

	    TQChar::Category category = ::category( uc );
	    if ( uc == 0xfffcU || uc == 0x2028U ) {
		item.analysis.bidiLevel = level % 2 ? level-1 : level;
		item.analysis.script = TQFont::Latin;
		item.isObject = true;
		s = TQFont::NoScript;
	    } else if ((uc >= 9 && uc <=13) ||
		       (category >= TQChar::Separator_Space && category <= TQChar::Separator_Paragraph)) {
		item.analysis.script = TQFont::Latin;
		item.isSpace = true;
		item.isTab = ( uc == '\t' );
		item.analysis.bidiLevel = item.isTab ? control.baseLevel() : level;
		s = TQFont::NoScript;
	    } else if ( s != script && (category != TQChar::Mark_NonSpacing || script == TQFont::NoScript)) {
		item.analysis.script = s;
		item.analysis.bidiLevel = level;
	    } else {
                if (i - start < 32000)
                    continue;
                start = i;
	    }

	    item.position = i;
	    items.append( item );
	    script = s;
	    item.isSpace = item.isTab = item.isObject = false;
	}
    }
    ++stop;
    start = stop;
}

// creates the next TQScript items.
static void bidiItemize( TQTextEngine *engine, bool rightToLeft, int mode )
{
    BidiControl control( rightToLeft );
    if ( mode & TQTextEngine::SingleLine )
	control.singleLine = true;

    int sor = 0;
    int eor = -1;

    // ### should get rid of this!
    bool first = true;

    int length = engine->string.length();

    if ( !length )
	return;

    const TQChar *unicode = engine->string.unicode();
    int current = 0;

    TQChar::Direction dir = rightToLeft ? TQChar::DirR : TQChar::DirL;
    BidiStatus status;
    TQChar::Direction sdir = direction( *unicode );
    if ( sdir != TQChar::DirL && sdir != TQChar::DirR && sdir != TQChar::DirAL && sdir != TQChar::DirEN && sdir != TQChar::DirAN )
	sdir = TQChar::DirON;
    else
        dir = TQChar::DirON;
    status.eor = sdir;
    status.lastStrong = rightToLeft ? TQChar::DirR : TQChar::DirL;
    status.last = status.lastStrong;
    status.dir = sdir;
#if (BIDI_DEBUG >= 2)
    tqDebug("---- bidiReorder --- '%s'", engine->string.utf8().data());
    tqDebug("rightToLeft = %d", rightToLeft);
#endif


    while ( current <= length ) {

	TQChar::Direction dirCurrent;
	if ( current == (int)length )
	    dirCurrent = control.basicDirection();
	else
	    dirCurrent = direction( unicode[current] );

#if (BIDI_DEBUG >= 2)
	cout << "pos=" << current << " dir=" << directions[dir]
	     << " current=" << directions[dirCurrent] << " last=" << directions[status.last]
	     << " eor=" << eor << "/" << directions[status.eor]
	     << " sor=" << sor << " lastStrong="
	     << directions[status.lastStrong]
	     << " level=" << (int)control.level() << endl;
#endif

	switch(dirCurrent) {

	    // embedding and overrides (X1-X9 in the BiDi specs)
	case TQChar::DirRLE:
	case TQChar::DirRLO:
	case TQChar::DirLRE:
	case TQChar::DirLRO:
	    {
		bool rtl = (dirCurrent == TQChar::DirRLE || dirCurrent == TQChar::DirRLO );
		bool override = (dirCurrent == TQChar::DirLRO || dirCurrent == TQChar::DirRLO );

		uchar level = control.level();
		if( (level%2 != 0) == rtl  )
		    level += 2;
		else
		    level++;
		if(level < 61) {
		    eor = current-1;
		    appendItems(engine, sor, eor, control, dir);
		    eor = current;
		    control.embed( level, override );
		    TQChar::Direction edir = (rtl ? TQChar::DirR : TQChar::DirL );
		    dir = status.eor = edir;
		    status.lastStrong = edir;
		}
		break;
	    }
	case TQChar::DirPDF:
	    {
		if (dir != control.direction()) {
		    eor = current-1;
		    appendItems(engine, sor, eor, control, dir);
		    dir = control.direction();
		}
		eor = current;
		appendItems(engine, sor, eor, control, dir);
		dir = TQChar::DirON; status.eor = TQChar::DirON;
		status.last = control.direction();
		control.pdf();
		if ( control.override() )
		    dir = control.direction();
		else
		    dir = TQChar::DirON;
		status.lastStrong = control.direction();
		break;
	    }

	    // strong types
	case TQChar::DirL:
	    if(dir == TQChar::DirON)
		dir = TQChar::DirL;
	    switch(status.last)
		{
		case TQChar::DirL:
		    eor = current; status.eor = TQChar::DirL; break;
		case TQChar::DirR:
		case TQChar::DirAL:
		case TQChar::DirEN:
		case TQChar::DirAN:
		    if ( !first ) {
			appendItems(engine, sor, eor, control, dir);
			dir = eor < length ? direction( unicode[eor] ) : control.basicDirection();
			status.eor = dir;
		    } else {
                        eor = current; status.eor = dir;
                    }
		    break;
		case TQChar::DirES:
		case TQChar::DirET:
		case TQChar::DirCS:
		case TQChar::DirBN:
		case TQChar::DirB:
		case TQChar::DirS:
		case TQChar::DirWS:
		case TQChar::DirON:
		    if(dir != TQChar::DirL) {
			//last stuff takes embedding dir
			if( control.direction() == TQChar::DirR ) {
			    if(status.eor != TQChar::DirR) {
				// AN or EN
				appendItems(engine, sor, eor, control, dir);
				status.eor = TQChar::DirON;
				dir = TQChar::DirR;
			    }
			    eor = current - 1;
			    appendItems(engine, sor, eor, control, dir);
			    dir = eor < length ? direction( unicode[eor] ) : control.basicDirection();
			    status.eor = dir;
			} else {
			    if(status.eor != TQChar::DirL) {
				appendItems(engine, sor, eor, control, dir);
				status.eor = TQChar::DirON;
				dir = TQChar::DirL;
			    } else {
				eor = current; status.eor = TQChar::DirL; break;
			    }
			}
		    } else {
			eor = current; status.eor = TQChar::DirL;
		    }
		default:
		    break;
		}
	    status.lastStrong = TQChar::DirL;
	    break;
	case TQChar::DirAL:
	case TQChar::DirR:
	    if(dir == TQChar::DirON) dir = TQChar::DirR;
	    switch(status.last)
		{
		case TQChar::DirL:
		case TQChar::DirEN:
		case TQChar::DirAN:
		    if ( !first ) {
			appendItems(engine, sor, eor, control, dir);
			dir = TQChar::DirON; status.eor = TQChar::DirON;
			break;
		    }
		case TQChar::DirR:
		case TQChar::DirAL:
		    eor = current; status.eor = TQChar::DirR; break;
		case TQChar::DirES:
		case TQChar::DirET:
		case TQChar::DirCS:
		case TQChar::DirBN:
		case TQChar::DirB:
		case TQChar::DirS:
		case TQChar::DirWS:
		case TQChar::DirON:
		    if( status.eor != TQChar::DirR && status.eor != TQChar::DirAL ) {
			//last stuff takes embedding dir
			if(control.direction() == TQChar::DirR
                           || status.lastStrong == TQChar::DirR || status.lastStrong == TQChar::DirAL) {
			    appendItems(engine, sor, eor, control, dir);
			    dir = TQChar::DirON; status.eor = TQChar::DirON;
			    dir = TQChar::DirR;
			    eor = current;
			} else {
			    eor = current - 1;
			    appendItems(engine, sor, eor, control, dir);
			    dir = TQChar::DirON; status.eor = TQChar::DirON;
			    dir = TQChar::DirR;
			}
		    } else {
			eor = current; status.eor = TQChar::DirR;
		    }
		default:
		    break;
		}
	    status.lastStrong = dirCurrent;
	    break;

	    // weak types:

	case TQChar::DirNSM:
	    if (eor == current-1)
		eor = current;
	    break;
	case TQChar::DirEN:
	    // if last strong was AL change EN to AN
	    if(status.lastStrong != TQChar::DirAL) {
		if(dir == TQChar::DirON) {
		    if(status.lastStrong == TQChar::DirL)
			dir = TQChar::DirL;
		    else
			dir = TQChar::DirEN;
		}
		switch(status.last)
		    {
		    case TQChar::DirET:
			if ( status.lastStrong == TQChar::DirR || status.lastStrong == TQChar::DirAL ) {
			    appendItems(engine, sor, eor, control, dir);
			    status.eor = TQChar::DirON;
			    dir = TQChar::DirAN;
			}
			// fall through
		    case TQChar::DirEN:
		    case TQChar::DirL:
			eor = current;
			status.eor = dirCurrent;
			break;
		    case TQChar::DirR:
		    case TQChar::DirAL:
		    case TQChar::DirAN:
			if ( !first )
			    appendItems(engine, sor, eor, control, dir);
			status.eor = TQChar::DirEN;
			dir = TQChar::DirAN; break;
		    case TQChar::DirES:
		    case TQChar::DirCS:
			if(status.eor == TQChar::DirEN || dir == TQChar::DirAN) {
			    eor = current; break;
			}
		    case TQChar::DirBN:
		    case TQChar::DirB:
		    case TQChar::DirS:
		    case TQChar::DirWS:
		    case TQChar::DirON:
			if(status.eor == TQChar::DirR) {
			    // neutrals go to R
			    eor = current - 1;
			    appendItems(engine, sor, eor, control, dir);
			    dir = TQChar::DirON; status.eor = TQChar::DirEN;
			    dir = TQChar::DirAN;
			}
			else if( status.eor == TQChar::DirL ||
				 (status.eor == TQChar::DirEN && status.lastStrong == TQChar::DirL)) {
			    eor = current; status.eor = dirCurrent;
			} else {
			    // numbers on both sides, neutrals get right to left direction
			    if(dir != TQChar::DirL) {
				appendItems(engine, sor, eor, control, dir);
				dir = TQChar::DirON; status.eor = TQChar::DirON;
				eor = current - 1;
				dir = TQChar::DirR;
				appendItems(engine, sor, eor, control, dir);
				dir = TQChar::DirON; status.eor = TQChar::DirON;
				dir = TQChar::DirAN;
			    } else {
				eor = current; status.eor = dirCurrent;
			    }
			}
		    default:
			break;
		    }
		break;
	    }
	case TQChar::DirAN:
	    dirCurrent = TQChar::DirAN;
	    if(dir == TQChar::DirON) dir = TQChar::DirAN;
	    switch(status.last)
		{
		case TQChar::DirL:
		case TQChar::DirAN:
		    eor = current; status.eor = TQChar::DirAN; break;
		case TQChar::DirR:
		case TQChar::DirAL:
		case TQChar::DirEN:
		    if ( !first )
			appendItems(engine, sor, eor, control, dir);
		    dir = TQChar::DirON; status.eor = TQChar::DirAN;
		    break;
		case TQChar::DirCS:
		    if(status.eor == TQChar::DirAN) {
			eor = current; break;
		    }
		case TQChar::DirES:
		case TQChar::DirET:
		case TQChar::DirBN:
		case TQChar::DirB:
		case TQChar::DirS:
		case TQChar::DirWS:
		case TQChar::DirON:
		    if(status.eor == TQChar::DirR) {
			// neutrals go to R
			eor = current - 1;
			appendItems(engine, sor, eor, control, dir);
			status.eor = TQChar::DirAN;
			dir = TQChar::DirAN;
		    } else if( status.eor == TQChar::DirL ||
			       (status.eor == TQChar::DirEN && status.lastStrong == TQChar::DirL)) {
			eor = current; status.eor = dirCurrent;
		    } else {
			// numbers on both sides, neutrals get right to left direction
			if(dir != TQChar::DirL) {
			    appendItems(engine, sor, eor, control, dir);
			    status.eor = TQChar::DirON;
			    eor = current - 1;
			    dir = TQChar::DirR;
			    appendItems(engine, sor, eor, control, dir);
			    status.eor = TQChar::DirAN;
			    dir = TQChar::DirAN;
			} else {
			    eor = current; status.eor = dirCurrent;
			}
		    }
		default:
		    break;
		}
	    break;
	case TQChar::DirES:
	case TQChar::DirCS:
	    break;
	case TQChar::DirET:
	    if(status.last == TQChar::DirEN) {
		dirCurrent = TQChar::DirEN;
		eor = current; status.eor = dirCurrent;
	    }
	    break;

	    // boundary neutrals should be ignored
	case TQChar::DirBN:
	    break;
	    // neutrals
	case TQChar::DirB:
	    // ### what do we do with newline and paragraph separators that come to here?
	    break;
	case TQChar::DirS:
	    // ### implement rule L1
	    break;
	case TQChar::DirWS:
	case TQChar::DirON:
	    break;
	default:
	    break;
	}

	//cout << "     after: dir=" << //        dir << " current=" << dirCurrent << " last=" << status.last << " eor=" << status.eor << " lastStrong=" << status.lastStrong << " embedding=" << control.direction() << endl;

	if(current >= (int)length) break;

	// set status.last as needed.
	switch(dirCurrent) {
	case TQChar::DirET:
	case TQChar::DirES:
	case TQChar::DirCS:
	case TQChar::DirS:
	case TQChar::DirWS:
	case TQChar::DirON:
	    switch(status.last)
	    {
	    case TQChar::DirL:
	    case TQChar::DirR:
	    case TQChar::DirAL:
	    case TQChar::DirEN:
	    case TQChar::DirAN:
		status.last = dirCurrent;
		break;
	    default:
		status.last = TQChar::DirON;
	    }
	    break;
	case TQChar::DirNSM:
	case TQChar::DirBN:
	    // ignore these
	    break;
	case TQChar::DirLRO:
	case TQChar::DirLRE:
	    status.last = TQChar::DirL;
	    break;
	case TQChar::DirRLO:
	case TQChar::DirRLE:
	    status.last = TQChar::DirR;
	    break;
	case TQChar::DirEN:
	    if ( status.last == TQChar::DirL ) {
		status.last = TQChar::DirL;
		break;
	    }
	    // fall through
	default:
	    status.last = dirCurrent;
	}

	first = false;
	++current;
    }

#if (BIDI_DEBUG >= 1)
    cout << "reached end of line current=" << current << ", eor=" << eor << endl;
#endif
    eor = current - 1; // remove dummy char

    if ( sor <= eor )
	appendItems(engine, sor, eor, control, dir);
}

void TQTextEngine::bidiReorder( int numItems, const TQ_UINT8 *levels, int *visualOrder )
{

    // first find highest and lowest levels
    uchar levelLow = 128;
    uchar levelHigh = 0;
    int i = 0;
    while ( i < numItems ) {
	//printf("level = %d\n", r->level);
	if ( levels[i] > levelHigh )
	    levelHigh = levels[i];
	if ( levels[i] < levelLow )
	    levelLow = levels[i];
	i++;
    }

    // implements reordering of the line (L2 according to BiDi spec):
    // L2. From the highest level found in the text to the lowest odd level on each line,
    // reverse any contiguous sequence of characters that are at that level or higher.

    // reversing is only done up to the lowest odd level
    if(!(levelLow%2)) levelLow++;

#if (BIDI_DEBUG >= 1)
    cout << "reorderLine: lineLow = " << (uint)levelLow << ", lineHigh = " << (uint)levelHigh << endl;
#endif

    int count = numItems - 1;
    for ( i = 0; i < numItems; i++ )
	visualOrder[i] = i;

    while(levelHigh >= levelLow) {
	int i = 0;
	while ( i < count ) {
	    while(i < count && levels[i] < levelHigh) i++;
	    int start = i;
	    while(i <= count && levels[i] >= levelHigh) i++;
	    int end = i-1;

	    if(start != end) {
		//cout << "reversing from " << start << " to " << end << endl;
		for(int j = 0; j < (end-start+1)/2; j++) {
		    int tmp = visualOrder[start+j];
		    visualOrder[start+j] = visualOrder[end-j];
		    visualOrder[end-j] = tmp;
		}
	    }
	    i++;
	}
	levelHigh--;
    }

#if (BIDI_DEBUG >= 1)
    cout << "visual order is:" << endl;
    for ( i = 0; i < numItems; i++ )
	cout << visualOrder[i] << endl;
#endif
}


// -----------------------------------------------------------------------------------------------------
//
// The line break algorithm. See http://www.unicode.org/reports/tr14/tr14-13.html
//
// -----------------------------------------------------------------------------------------------------

/* The Unicode algorithm does in our opinion allow line breaks at some
   places they shouldn't be allowed. The following changes were thus
   made in comparison to the Unicode reference:

   CL->AL from Dbk to Ibk
   CL->PR from Dbk to Ibk
   EX->AL from Dbk to Ibk
   IS->AL from Dbk to Ibk
   PO->AL from Dbk to Ibk
   SY->AL from Dbk to Ibk
   SY->PO from Dbk to Ibk
   SY->PR from Dbk to Ibk
   SY->OP from Dbk to Ibk
   Al->OP from Dbk to Ibk
   AL->HY from Dbk to Ibk
   AL->PR from Dbk to Ibk
   AL->PO from Dbk to Ibk
   PR->PR from Dbk to Ibk
   PO->PO from Dbk to Ibk
   PR->PO from Dbk to Ibk
   PO->PR from Dbk to Ibk
   HY->PO from Dbk to Ibk
   HY->PR from Dbk to Ibk
   HY->OP from Dbk to Ibk
   PO->OP from Dbk to Ibk
   NU->EX from Dbk to Ibk
   NU->PR from Dbk to Ibk
   PO->NU from Dbk to Ibk
   EX->PO from Dbk to Ibk
*/

enum break_action {
    Dbk, // Direct break
    Ibk, // Indirect break; only allowed if space between the two chars
    Pbk // Prohibited break; no break allowed even if space between chars
};

// The following line break classes are not treated by the table:
// SA, BK, CR, LF, SG, CB, SP
static const TQ_UINT8 breakTable[TQUnicodeTables::LineBreak_CM+1][TQUnicodeTables::LineBreak_CM+1] =
{
    // OP,  CL,  QU,  GL, NS,  EX,  SY,  IS,  PR,  PO,  NU,  AL,  ID,  IN,  HY,  BA,  BB,  B2,  ZW,  CM
    { Pbk, Pbk, Pbk, Pbk, Pbk, Pbk, Pbk, Pbk, Pbk, Pbk, Pbk, Pbk, Pbk, Pbk, Pbk, Pbk, Pbk, Pbk, Pbk, Pbk }, // OP
    { Dbk, Pbk, Ibk, Pbk, Pbk, Pbk, Pbk, Pbk, Ibk, Ibk, Dbk, Ibk, Dbk, Dbk, Ibk, Ibk, Pbk, Pbk, Pbk, Pbk }, // CL
    { Pbk, Pbk, Ibk, Pbk, Ibk, Pbk, Pbk, Pbk, Ibk, Ibk, Ibk, Ibk, Ibk, Ibk, Ibk, Ibk, Ibk, Ibk, Pbk, Pbk }, // QU
    { Ibk, Pbk, Ibk, Pbk, Ibk, Pbk, Pbk, Pbk, Ibk, Ibk, Ibk, Ibk, Ibk, Ibk, Ibk, Ibk, Ibk, Ibk, Pbk, Pbk }, // GL
    { Dbk, Pbk, Ibk, Pbk, Ibk, Pbk, Pbk, Pbk, Dbk, Dbk, Dbk, Dbk, Dbk, Dbk, Ibk, Ibk, Dbk, Dbk, Pbk, Ibk }, // NS
    { Dbk, Pbk, Ibk, Pbk, Ibk, Pbk, Pbk, Pbk, Dbk, Ibk, Ibk, Ibk, Dbk, Dbk, Ibk, Ibk, Dbk, Dbk, Pbk, Ibk }, // EX
    { Ibk, Pbk, Ibk, Pbk, Ibk, Pbk, Pbk, Pbk, Ibk, Ibk, Ibk, Ibk, Dbk, Dbk, Ibk, Ibk, Dbk, Dbk, Pbk, Ibk }, // SY
    { Dbk, Pbk, Ibk, Pbk, Ibk, Pbk, Pbk, Pbk, Dbk, Dbk, Ibk, Ibk, Dbk, Dbk, Ibk, Ibk, Dbk, Dbk, Pbk, Ibk }, // IS
    { Ibk, Pbk, Ibk, Pbk, Ibk, Pbk, Pbk, Pbk, Ibk, Ibk, Ibk, Ibk, Ibk, Dbk, Ibk, Ibk, Dbk, Dbk, Pbk, Pbk }, // PR
    { Ibk, Pbk, Ibk, Pbk, Ibk, Pbk, Pbk, Pbk, Ibk, Ibk, Ibk, Ibk, Dbk, Dbk, Ibk, Ibk, Dbk, Dbk, Pbk, Ibk }, // PO
    { Dbk, Pbk, Ibk, Pbk, Ibk, Pbk, Pbk, Pbk, Ibk, Ibk, Ibk, Ibk, Dbk, Ibk, Ibk, Ibk, Dbk, Dbk, Pbk, Pbk }, // NU
    { Ibk, Pbk, Ibk, Pbk, Ibk, Pbk, Pbk, Pbk, Ibk, Ibk, Ibk, Ibk, Dbk, Ibk, Ibk, Ibk, Dbk, Dbk, Pbk, Pbk }, // AL
    { Dbk, Pbk, Ibk, Pbk, Ibk, Pbk, Pbk, Pbk, Dbk, Ibk, Dbk, Dbk, Dbk, Ibk, Ibk, Ibk, Dbk, Dbk, Pbk, Ibk }, // ID
    { Dbk, Pbk, Ibk, Pbk, Ibk, Pbk, Pbk, Pbk, Dbk, Dbk, Dbk, Dbk, Dbk, Ibk, Ibk, Ibk, Dbk, Dbk, Pbk, Ibk }, // IN
    { Ibk, Pbk, Ibk, Pbk, Ibk, Pbk, Pbk, Pbk, Ibk, Ibk, Ibk, Ibk, Dbk, Dbk, Ibk, Ibk, Dbk, Dbk, Pbk, Ibk }, // HY
    { Dbk, Pbk, Ibk, Pbk, Ibk, Pbk, Pbk, Pbk, Dbk, Dbk, Dbk, Dbk, Dbk, Dbk, Ibk, Ibk, Dbk, Dbk, Pbk, Ibk }, // BA
    { Ibk, Pbk, Ibk, Pbk, Ibk, Pbk, Pbk, Pbk, Ibk, Ibk, Ibk, Ibk, Ibk, Ibk, Ibk, Ibk, Ibk, Ibk, Pbk, Ibk }, // BB
    { Dbk, Pbk, Ibk, Pbk, Ibk, Pbk, Pbk, Pbk, Dbk, Dbk, Dbk, Dbk, Dbk, Dbk, Ibk, Ibk, Dbk, Pbk, Pbk, Ibk }, // B2
    { Dbk, Dbk, Dbk, Dbk, Dbk, Dbk, Dbk, Dbk, Dbk, Dbk, Dbk, Dbk, Dbk, Dbk, Dbk, Dbk, Dbk, Dbk, Pbk, Ibk }, // ZW
    { Dbk, Pbk, Ibk, Pbk, Ibk, Pbk, Pbk, Pbk, Dbk, Ibk, Dbk, Dbk, Dbk, Ibk, Ibk, Ibk, Dbk, Dbk, Pbk, Pbk }  // CM
};

// set the soft break flag at every possible line breaking point. This needs correct clustering information.
static void calcLineBreaks(const TQString &str, TQCharAttributes *charAttributes)
{
    int len = str.length();
    if (!len)
	return;

    const TQChar *uc = str.unicode();
    int cls = lineBreakClass(*uc);
    if (cls >= TQUnicodeTables::LineBreak_CM)
	cls = TQUnicodeTables::LineBreak_ID;

    charAttributes[0].softBreak = false;
    charAttributes[0].whiteSpace = (cls == TQUnicodeTables::LineBreak_SP);
    charAttributes[0].charStop = true;

    bool prevIsHighSurrogate = uc[0].isHighSurrogate();
    for (int i = 1; i < len; ++i)
    {
	// Don't stop on low surrogate characters of complete valid pairs
	if (prevIsHighSurrogate && uc[i].isLowSurrogate())
	{
	    prevIsHighSurrogate = false;
	    charAttributes[i].softBreak = false;
	    charAttributes[i].whiteSpace = false;
	    charAttributes[i].charStop = false;
	    continue;
	}
	prevIsHighSurrogate = uc[i].isHighSurrogate();

	int ncls = ::lineBreakClass(uc[i]);
	int category = ::category(uc[i]);
	if (category == TQChar::Mark_NonSpacing)
	{
	    charAttributes[i].softBreak = false;
	    charAttributes[i].whiteSpace = false;
	    charAttributes[i].charStop = false;
	    continue;
	}

	if (ncls == TQUnicodeTables::LineBreak_SP) {
	    charAttributes[i].softBreak = false;
	    charAttributes[i].whiteSpace = true;
	    charAttributes[i].charStop = true;
	    cls = ncls;
	    continue;
	}

	if (cls == TQUnicodeTables::LineBreak_SA && ncls == TQUnicodeTables::LineBreak_SA)
	{
	    // two complex chars (thai or lao), thai_attributes might override, but here
	    // we do a best guess
	    charAttributes[i].softBreak = true;
	    charAttributes[i].whiteSpace = false;
	    charAttributes[i].charStop = true;
	    cls = ncls;
	    continue;
	}

	int tcls = ncls;
	if (tcls >= TQUnicodeTables::LineBreak_SA)
	    tcls = TQUnicodeTables::LineBreak_ID;
	if (cls >= TQUnicodeTables::LineBreak_SA)
	    cls = TQUnicodeTables::LineBreak_ID;

	bool softBreak;
	int brk = breakTable[cls][tcls];
	if (brk == Ibk)
	    softBreak = (cls == TQUnicodeTables::LineBreak_SP);
	else
	    softBreak = (brk == Dbk);
	// tqDebug("char = %c %04x, cls=%d, ncls=%d, brk=%d soft=%d", uc[i].cell(), uc[i].unicode(), cls, ncls, brk, charAttributes[i].softBreak);
	charAttributes[i].softBreak = softBreak;
	charAttributes[i].whiteSpace = false;
	charAttributes[i].charStop = true;
	cls = ncls;
    }
}

#if defined( TQ_WS_X11 ) || defined ( TQ_WS_QWS )
# include "qtextengine_unix.cpp"
#elif defined( TQ_WS_WIN )
# include "qtextengine_win.cpp"
#elif defined( TQ_WS_MAC )
# include "qtextengine_mac.cpp"
#endif



TQTextEngine::TQTextEngine( const TQString &str, TQFontPrivate *f )
    : string( str ), fnt( f ), direction( TQChar::DirON ), haveCharAttributes( false ), widthOnly( false )
{
#ifdef TQ_WS_WIN
    if ( !resolvedUsp10 )
	resolveUsp10();
#endif
    if ( fnt ) fnt->ref();

    num_glyphs = TQMAX( 16, str.length()*3/2 );
    int space_charAttributes = (sizeof(TQCharAttributes)*str.length()+sizeof(void*)-1)/sizeof(void*);
    int space_logClusters = (sizeof(unsigned short)*str.length()+sizeof(void*)-1)/sizeof(void*);
    int space_glyphs = (sizeof(glyph_t)*num_glyphs+sizeof(void*)-1)/sizeof(void*);
    int space_advances = (sizeof(advance_t)*num_glyphs+sizeof(void*)-1)/sizeof(void*);
    int space_offsets = (sizeof(qoffset_t)*num_glyphs+sizeof(void*)-1)/sizeof(void*);
    int space_glyphAttributes = (sizeof(GlyphAttributes)*num_glyphs+sizeof(void*)-1)/sizeof(void*);

    allocated = space_charAttributes + space_glyphs + space_advances +
		space_offsets + space_logClusters + space_glyphAttributes;
    memory = (void **)::malloc( allocated*sizeof( void * ) );
    memset( memory, 0, allocated*sizeof( void * ) );

    void **m = memory;
    m += space_charAttributes;
    logClustersPtr = (unsigned short *) m;
    m += space_logClusters;
    glyphPtr = (glyph_t *) m;
    m += space_glyphs;
    advancePtr = (advance_t *) m;
    m += space_advances;
    offsetsPtr = (qoffset_t *) m;
    m += space_offsets;
    glyphAttributesPtr = (GlyphAttributes *) m;

    used = 0;
}

TQTextEngine::~TQTextEngine()
{
    if ( fnt && fnt->deref())
	delete fnt;
    free( memory );
    allocated = 0;
}

void TQTextEngine::reallocate( int totalGlyphs )
{
    int new_num_glyphs = totalGlyphs;
    int space_charAttributes = (sizeof(TQCharAttributes)*string.length()+sizeof(void*)-1)/sizeof(void*);
    int space_logClusters = (sizeof(unsigned short)*string.length()+sizeof(void*)-1)/sizeof(void*);
    int space_glyphs = (sizeof(glyph_t)*new_num_glyphs+sizeof(void*)-1)/sizeof(void*);
    int space_advances = (sizeof(advance_t)*new_num_glyphs+sizeof(void*)-1)/sizeof(void*);
    int space_offsets = (sizeof(qoffset_t)*new_num_glyphs+sizeof(void*)-1)/sizeof(void*);
    int space_glyphAttributes = (sizeof(GlyphAttributes)*new_num_glyphs+sizeof(void*)-1)/sizeof(void*);

    int newAllocated = space_charAttributes + space_glyphs + space_advances +
		space_offsets + space_logClusters + space_glyphAttributes;
    void ** newMemory = (void **)::malloc( newAllocated*sizeof( void * ) );

    void **nm = newMemory;
    memcpy( nm, memory, string.length()*sizeof(TQCharAttributes) );
    nm += space_charAttributes;
    memcpy( nm, logClustersPtr, num_glyphs*sizeof(unsigned short) );
    logClustersPtr = (unsigned short *) nm;
    nm += space_logClusters;
    memcpy( nm, glyphPtr, num_glyphs*sizeof(glyph_t) );
    glyphPtr = (glyph_t *) nm;
    nm += space_glyphs;
    memcpy( nm, advancePtr, num_glyphs*sizeof(advance_t) );
    advancePtr = (advance_t *) nm;
    nm += space_advances;
    memcpy( nm, offsetsPtr, num_glyphs*sizeof(qoffset_t) );
    offsetsPtr = (qoffset_t *) nm;
    nm += space_offsets;
    memcpy( nm, glyphAttributesPtr, num_glyphs*sizeof(GlyphAttributes) );
    glyphAttributesPtr = (GlyphAttributes *) nm;

    free( memory );
    memory = newMemory;
    allocated = newAllocated;
    num_glyphs = new_num_glyphs;
}

const TQCharAttributes *TQTextEngine::attributes()
{
    TQCharAttributes *charAttributes = (TQCharAttributes *) memory;
    if ( haveCharAttributes )
	return charAttributes;

    if ( !items.d )
	itemize();

    ensureSpace(string.length());
    charAttributes = (TQCharAttributes *) memory;
    calcLineBreaks(string, charAttributes);
    
    for ( int i = 0; i < items.size(); i++ ) {
	TQScriptItem &si = items[i];
#ifdef TQ_WS_WIN
        int script = uspScriptForItem(this, i);
#else
        int script = si.analysis.script;
#endif
	Q_ASSERT( script < TQFont::NScripts );
        AttributeFunction attributes = scriptEngines[script].charAttributes;
        if (!attributes)
            continue;
	int from = si.position;
	int len = length( i );
	attributes( script, string, from, len, charAttributes );
    }

    haveCharAttributes = true;
    return charAttributes;
}

void TQTextEngine::splitItem( int item, int pos )
{
    if ( pos <= 0 )
	return;

    // we have to ensure we get correct shaping for arabic and other
    // complex languages so we have to call shape _before_ we split the item.
    shape(item);

    if ( items.d->size == items.d->alloc )
	items.resize( items.d->size + 1 );

    int numMove = items.d->size - item-1;
    if ( numMove > 0 )
	memmove( items.d->items + item+2, items.d->items +item+1, numMove*sizeof( TQScriptItem ) );
    items.d->size++;
    TQScriptItem &newItem = items.d->items[item+1];
    TQScriptItem &oldItem = items.d->items[item];
    newItem = oldItem;
    items.d->items[item+1].position += pos;
    if ( newItem.fontEngine )
	newItem.fontEngine->ref();

    if (oldItem.num_glyphs) {
	// already shaped, break glyphs aswell
	int breakGlyph = logClusters(&oldItem)[pos];

	newItem.num_glyphs = oldItem.num_glyphs - breakGlyph;
	oldItem.num_glyphs = breakGlyph;
	newItem.glyph_data_offset = oldItem.glyph_data_offset + breakGlyph;

	for (int i = 0; i < newItem.num_glyphs; i++)
	    logClusters(&newItem)[i] -= breakGlyph;

	int w = 0;
	const advance_t *a = advances(&oldItem);
	for(int j = 0; j < breakGlyph; ++j)
	    w += *(a++);

	newItem.width = oldItem.width - w;
	oldItem.width = w;
    }

//     tqDebug("split at position %d itempos=%d", pos, item );
}


int TQTextEngine::width( int from, int len ) const
{
    int w = 0;

//     tqDebug("TQTextEngine::width( from = %d, len = %d ), numItems=%d, strleng=%d", from,  len, items.size(), string.length() );
    for ( int i = 0; i < items.size(); i++ ) {
	TQScriptItem *si = &items[i];
	int pos = si->position;
	int ilen = length( i );
//  	tqDebug("item %d: from %d len %d", i, pos, ilen );
	if ( pos >= from + len )
	    break;
	if ( pos + ilen > from ) {
	    if ( !si->num_glyphs )
		shape( i );

	    advance_t *advances = this->advances( si );
	    unsigned short *logClusters = this->logClusters( si );

// 	    fprintf( stderr, "  logclusters:" );
// 	    for ( int k = 0; k < ilen; k++ )
// 		fprintf( stderr, " %d", logClusters[k] );
// 	    fprintf( stderr, "\n" );
	    // do the simple thing for now and give the first glyph in a cluster the full width, all other ones 0.
	    int charFrom = from - pos;
	    if ( charFrom < 0 )
		charFrom = 0;
	    int glyphStart = logClusters[charFrom];
	    if ( charFrom > 0 && logClusters[charFrom-1] == glyphStart )
		while ( charFrom < ilen && logClusters[charFrom] == glyphStart )
		    charFrom++;
	    if ( charFrom < ilen ) {
		glyphStart = logClusters[charFrom];
		int charEnd = from + len - 1 - pos;
		if ( charEnd >= ilen )
		    charEnd = ilen-1;
		int glyphEnd = logClusters[charEnd];
		while ( charEnd < ilen && logClusters[charEnd] == glyphEnd )
		    charEnd++;
		glyphEnd = (charEnd == ilen) ? si->num_glyphs : logClusters[charEnd];

// 		tqDebug("char: start=%d end=%d / glyph: start = %d, end = %d", charFrom, charEnd, glyphStart, glyphEnd );
		for ( int i = glyphStart; i < glyphEnd; i++ )
		    w += advances[i];
	    }
	}
    }
//     tqDebug("   --> w= %d ", w );
    return w;
}

void TQTextEngine::itemize( int mode )
{
    if ( !items.d ) {
	int size = 8;
	items.d = (TQScriptItemArrayPrivate *)malloc( sizeof( TQScriptItemArrayPrivate ) +
						    sizeof( TQScriptItem ) * size );
	items.d->alloc = size;
    }
    items.d->size = 0;
    if ( string.length() == 0 )
	return;

    if ( !(mode & NoBidi) ) {
	if ( direction == TQChar::DirON )
	    direction = basicDirection( string );
	bidiItemize( this, direction == TQChar::DirR, mode );
    } else {
	BidiControl control( false );
	if ( mode & TQTextEngine::SingleLine )
	    control.singleLine = true;
	int start = 0;
	int stop = string.length() - 1;
	appendItems(this, start, stop, control, TQChar::DirL);
    }
    if ( (mode & WidthOnly) == WidthOnly )
	widthOnly = true;
}

glyph_metrics_t TQTextEngine::boundingBox( int from,  int len ) const
{
    glyph_metrics_t gm;

    for ( int i = 0; i < items.size(); i++ ) {
	TQScriptItem *si = &items[i];
	int pos = si->position;
	int ilen = length( i );
	if ( pos > from + len )
	    break;
	if ( pos + len > from ) {
	    if ( !si->num_glyphs )
		shape( i );
	    advance_t *advances = this->advances( si );
	    unsigned short *logClusters = this->logClusters( si );
	    glyph_t *glyphs = this->glyphs( si );
	    qoffset_t *offsets = this->offsets( si );

	    // do the simple thing for now and give the first glyph in a cluster the full width, all other ones 0.
	    int charFrom = from - pos;
	    if ( charFrom < 0 )
		charFrom = 0;
	    int glyphStart = logClusters[charFrom];
	    if ( charFrom > 0 && logClusters[charFrom-1] == glyphStart )
		while ( charFrom < ilen && logClusters[charFrom] == glyphStart )
		    charFrom++;
	    if ( charFrom < ilen ) {
		glyphStart = logClusters[charFrom];
		int charEnd = from + len - 1 - pos;
		if ( charEnd >= ilen )
		    charEnd = ilen-1;
		int glyphEnd = logClusters[charEnd];
		while ( charEnd < ilen && logClusters[charEnd] == glyphEnd )
		    charEnd++;
		glyphEnd = (charEnd == ilen) ? si->num_glyphs : logClusters[charEnd];
		if ( glyphStart <= glyphEnd  ) {
		    TQFontEngine *fe = si->fontEngine;
		    glyph_metrics_t m = fe->boundingBox( glyphs+glyphStart, advances+glyphStart,
						       offsets+glyphStart, glyphEnd-glyphStart );
		    gm.x = TQMIN( gm.x, m.x + gm.xoff );
		    gm.y = TQMIN( gm.y, m.y + gm.yoff );
		    gm.width = TQMAX( gm.width, m.width+gm.xoff );
		    gm.height = TQMAX( gm.height, m.height+gm.yoff );
		    gm.xoff += m.xoff;
		    gm.yoff += m.yoff;
		}
	    }
	}
    }
    return gm;
}
