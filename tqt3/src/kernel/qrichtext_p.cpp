/****************************************************************************
**
** Implementation of the internal TQt classes dealing with rich text
**
** Created : 990101
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

#include "qrichtext_p.h"

#ifndef TQT_NO_RICHTEXT

TQTextCommand::~TQTextCommand() {}
TQTextCommand::Commands TQTextCommand::type() const { return Invalid; }


#ifndef TQT_NO_TEXTCUSTOMITEM
TQTextCustomItem::~TQTextCustomItem() {}
void TQTextCustomItem::adjustToPainter( TQPainter* p){ if ( p ) width = 0; }
TQTextCustomItem::Placement TQTextCustomItem::placement() const { return PlaceInline; }

bool TQTextCustomItem::ownLine() const { return false; }
void TQTextCustomItem::resize( int nwidth ){ width = nwidth; }
void TQTextCustomItem::invalidate() {}

bool TQTextCustomItem::isNested() const { return false; }
int TQTextCustomItem::minimumWidth() const { return 0; }

TQString TQTextCustomItem::richText() const { return TQString::null; }

bool TQTextCustomItem::enter( TQTextCursor *, TQTextDocument*&, TQTextParagraph *&, int &, int &, int &, bool )
{
    return true;
}
bool TQTextCustomItem::enterAt( TQTextCursor *, TQTextDocument *&, TQTextParagraph *&, int &, int &, int &, const TQPoint & )
{
    return true;
}
bool TQTextCustomItem::next( TQTextCursor *, TQTextDocument *&, TQTextParagraph *&, int &, int &, int & )
{
    return true;
}
bool TQTextCustomItem::prev( TQTextCursor *, TQTextDocument *&, TQTextParagraph *&, int &, int &, int & )
{
    return true;
}
bool TQTextCustomItem::down( TQTextCursor *, TQTextDocument *&, TQTextParagraph *&, int &, int &, int & )
{
    return true;
}
bool TQTextCustomItem::up( TQTextCursor *, TQTextDocument *&, TQTextParagraph *&, int &, int &, int & )
{
    return true;
}
#endif // TQT_NO_TEXTCUSTOMITEM

void TQTextFlow::setPageSize( int ps ) { pagesize = ps; }
#ifndef TQT_NO_TEXTCUSTOMITEM
bool TQTextFlow::isEmpty() { return leftItems.isEmpty() && rightItems.isEmpty(); }
#else
bool TQTextFlow::isEmpty() { return true; }
#endif

#ifndef TQT_NO_TEXTCUSTOMITEM
void TQTextTableCell::invalidate() { cached_width = -1; cached_sizehint = -1; }

void TQTextTable::invalidate() { cachewidth = -1; }
#endif

TQTextParagraphData::~TQTextParagraphData() {}
void TQTextParagraphData::join( TQTextParagraphData * ) {}

TQTextFormatter::~TQTextFormatter() {}
void TQTextFormatter::setWrapEnabled( bool b ) { wrapEnabled = b; }
void TQTextFormatter::setWrapAtColumn( int c ) { wrapColumn = c; }



int TQTextCursor::x() const
{
    if ( idx >= para->length() )
	return 0;
    TQTextStringChar *c = para->at( idx );
    int curx = c->x;
    if ( !c->rightToLeft && c->c.isSpace() && idx > 0 && para->at( idx - 1 )->c != '\t' &&
	 !c->lineStart && ( para->alignment() & TQt::AlignJustify ) == TQt::AlignJustify )
    {
	curx = para->at( idx - 1 )->x + para->string()->width( idx - 1 );
    }
    if ( c->rightToLeft )
    {
	curx += para->string()->width( idx );
    }
    return curx;
}

int TQTextCursor::y() const
{
    int dummy, line;
    para->lineStartOfChar( idx, &dummy, &line );
    return para->lineY( line );
}

int TQTextCursor::globalX() const { return totalOffsetX() + para->rect().x() + x(); }
int TQTextCursor::globalY() const { return totalOffsetY() + para->rect().y() + y(); }

TQTextDocument *TQTextCursor::document() const
{
    return para ? para->document() : 0;
}

void TQTextCursor::gotoPosition( TQTextParagraph* p, int index )
{
    if ( para && p != para ) {
	while ( !indices.isEmpty() && para->document() != p->document() )
	    pop();
	Q_ASSERT( indices.isEmpty() || para->document() == p->document() );
    }
    para = p;
    if ( index < 0 || index >= para->length() ) {
#if defined(QT_CHECK_RANGE)
	tqWarning( "TQTextCursor::gotoParagraph Index: %d out of range", index );
#endif
	if ( index < 0 || para->length() == 0 )
	    index = 0;
	else
	    index = para->length() - 1;
    }

    tmpX = -1;
    idx = index;
    fixCursorPosition();
}

bool TQTextDocument::hasSelection( int id, bool visible ) const
{
    return ( selections.find( id ) != selections.end() &&
	     ( !visible ||
	       ( (TQTextDocument*)this )->selectionStartCursor( id ) !=
	       ( (TQTextDocument*)this )->selectionEndCursor( id ) ) );
}

void TQTextDocument::setSelectionStart( int id, const TQTextCursor &cursor )
{
    TQTextDocumentSelection sel;
    sel.startCursor = cursor;
    sel.endCursor = cursor;
    sel.swapped = false;
    selections[ id ] = sel;
}

TQTextParagraph *TQTextDocument::paragAt( int i ) const
{
    TQTextParagraph* p = curParag;
    if ( !p || p->paragId() > i )
	p = fParag;
    while ( p && p->paragId() != i )
	p = p->next();
    ((TQTextDocument*)this)->curParag = p;
    return p;
}


TQTextFormat::~TQTextFormat()
{
}

TQTextFormat::TQTextFormat()
    : fm( TQFontMetrics( fn ) ), linkColor( true ), logicalFontSize( 3 ), stdSize( tqApp->font().pointSize() )
{
    ref = 0;

    usePixelSizes = false;
    if ( stdSize == -1 ) {
	stdSize = tqApp->font().pixelSize();
	usePixelSizes = true;
    }

    missp = false;
    ha = AlignNormal;
    collection = 0;
}

TQTextFormat::TQTextFormat( const TQStyleSheetItem *style )
    : fm( TQFontMetrics( fn ) ), linkColor( true ), logicalFontSize( 3 ), stdSize( tqApp->font().pointSize() )
{
    ref = 0;

    usePixelSizes = false;
    if ( stdSize == -1 ) {
	stdSize = tqApp->font().pixelSize();
	usePixelSizes = true;
    }

    missp = false;
    ha = AlignNormal;
    collection = 0;
    fn = TQFont( style->fontFamily(),
		style->fontSize(),
		style->fontWeight(),
		style->fontItalic() );
    fn.setUnderline( style->fontUnderline() );
    fn.setStrikeOut( style->fontStrikeOut() );
    col = style->color();
    fm = TQFontMetrics( fn );
    leftBearing = fm.minLeftBearing();
    rightBearing = fm.minRightBearing();
    hei = fm.lineSpacing();
    asc = fm.ascent() + (fm.leading()+1)/2;
    dsc = fm.descent();
    missp = false;
    ha = AlignNormal;
    memset( widths, 0, 256 );
    generateKey();
    addRef();
}

TQTextFormat::TQTextFormat( const TQFont &f, const TQColor &c, TQTextFormatCollection *parent )
    : fn( f ), col( c ), fm( TQFontMetrics( f ) ), linkColor( true ),
      logicalFontSize( 3 ), stdSize( f.pointSize() )
{
    ref = 0;
    usePixelSizes = false;
    if ( stdSize == -1 ) {
	stdSize = f.pixelSize();
	usePixelSizes = true;
    }
    collection = parent;
    leftBearing = fm.minLeftBearing();
    rightBearing = fm.minRightBearing();
    hei = fm.lineSpacing();
    asc = fm.ascent() + (fm.leading()+1)/2;
    dsc = fm.descent();
    missp = false;
    ha = AlignNormal;
    memset( widths, 0, 256 );
    generateKey();
    addRef();
}

TQTextFormat::TQTextFormat( const TQTextFormat &f )
    : fm( f.fm )
{
    ref = 0;
    collection = 0;
    fn = f.fn;
    col = f.col;
    leftBearing = f.leftBearing;
    rightBearing = f.rightBearing;
    memset( widths, 0, 256 );
    hei = f.hei;
    asc = f.asc;
    dsc = f.dsc;
    stdSize = f.stdSize;
    usePixelSizes = f.usePixelSizes;
    logicalFontSize = f.logicalFontSize;
    missp = f.missp;
    ha = f.ha;
    k = f.k;
    linkColor = f.linkColor;
    addRef();
}

TQTextFormat& TQTextFormat::operator=( const TQTextFormat &f )
{
    ref = 0;
    collection = f.collection;
    fn = f.fn;
    col = f.col;
    fm = f.fm;
    leftBearing = f.leftBearing;
    rightBearing = f.rightBearing;
    memset( widths, 0, 256 );
    hei = f.hei;
    asc = f.asc;
    dsc = f.dsc;
    stdSize = f.stdSize;
    usePixelSizes = f.usePixelSizes;
    logicalFontSize = f.logicalFontSize;
    missp = f.missp;
    ha = f.ha;
    k = f.k;
    linkColor = f.linkColor;
    addRef();
    return *this;
}

void TQTextFormat::update()
{
    fm = TQFontMetrics( fn );
    leftBearing = fm.minLeftBearing();
    rightBearing = fm.minRightBearing();
    hei = fm.lineSpacing();
    asc = fm.ascent() + (fm.leading()+1)/2;
    dsc = fm.descent();
    memset( widths, 0, 256 );
    generateKey();
}


TQPainter* TQTextFormat::pntr = 0;
TQFontMetrics* TQTextFormat::pntr_fm = 0;
int TQTextFormat::pntr_ldg=-1;
int TQTextFormat::pntr_asc=-1;
int TQTextFormat::pntr_hei=-1;
int TQTextFormat::pntr_dsc=-1;

void TQTextFormat::cleanupPrivateData() {
    delete TQTextFormat::pntr_fm;
    TQTextFormat::pntr_fm = 0;
    TQTextFormat::pntr = 0;

    // Not really necessary, but better to tidy-up everything
    TQTextFormat::pntr_fm = 0;
    TQTextFormat::pntr_ldg=-1;
    TQTextFormat::pntr_asc=-1;
    TQTextFormat::pntr_hei=-1;
    TQTextFormat::pntr_dsc=-1;
}

void TQTextFormat::setPainter( TQPainter *p )
{
    pntr = p;
}

TQPainter*  TQTextFormat::painter()
{
    return pntr;
}

void TQTextFormat::applyFont( const TQFont &f )
{
    TQFontMetrics fm( pntr->fontMetrics() );

    if ( !pntr_fm ) {
	tqAddPostRoutine( &TQTextFormat::cleanupPrivateData );
    }

    if ( !pntr_fm
	|| pntr_fm->painter != pntr
	|| pntr_fm->d != fm.d
	|| !pntr->font().isCopyOf( f )
       ) {
	pntr->setFont( f );
	delete pntr_fm;
	pntr_fm = new TQFontMetrics( pntr->fontMetrics() );
	pntr_ldg = pntr_fm->leading();
	pntr_asc = pntr_fm->ascent()+(pntr_ldg+1)/2;
	pntr_hei = pntr_fm->lineSpacing();
	pntr_dsc = -1;
    }
}

int TQTextFormat::minLeftBearing() const
{
    if ( !pntr || !pntr->isActive() )
	return leftBearing;
    applyFont( fn );
    return pntr_fm->minLeftBearing();
}

int TQTextFormat::minRightBearing() const
{
    if ( !pntr || !pntr->isActive() )
	return rightBearing;
    applyFont( fn );
    return pntr_fm->minRightBearing();
}

int TQTextFormat::height() const
{
    if ( !pntr || !pntr->isActive() )
	return hei;
    applyFont( fn );
    return pntr_hei;
}

int TQTextFormat::ascent() const
{
    if ( !pntr || !pntr->isActive() )
	return asc;
    applyFont( fn );
    return pntr_asc;
}

int TQTextFormat::descent() const
{
    if ( !pntr || !pntr->isActive() )
	return dsc;
    applyFont( fn );
    if ( pntr_dsc < 0 )
	pntr_dsc = pntr_fm->descent();
    return pntr_dsc;
}

int TQTextFormat::leading() const
{
    if ( !pntr || !pntr->isActive() )
	return fm.leading();
    applyFont( fn );
    return pntr_ldg;
}

void TQTextFormat::generateKey()
{
    k = getKey( fn, col, isMisspelled(), vAlign() );
}

TQString TQTextFormat::getKey( const TQFont &fn, const TQColor &col, bool misspelled, VerticalAlignment a )
{
    TQString k = fn.key();
    k += '/';
    k += TQString::number( (uint)col.rgb() );
    k += '/';
    k += TQString::number( (int)misspelled );
    k += '/';
    k += TQString::number( (int)a );
    return k;
}

TQString TQTextString::toString( const TQMemArray<TQTextStringChar> &data )
{
    TQString s;
    int l = data.size();
    s.setUnicode( 0, l );
    TQTextStringChar *c = data.data();
    TQChar *uc = (TQChar *)s.unicode();
    while ( l-- )
	*(uc++) = (c++)->c;

    return s;
}

void TQTextParagraph::setSelection( int id, int start, int end )
{
    TQMap<int, TQTextParagraphSelection>::ConstIterator it = selections().find( id );
    if ( it != mSelections->end() ) {
	if ( start == ( *it ).start && end == ( *it ).end )
	    return;
    }

    TQTextParagraphSelection sel;
    sel.start = start;
    sel.end = end;
    (*mSelections)[ id ] = sel;
    setChanged( true, true );
}

void TQTextParagraph::removeSelection( int id )
{
    if ( !hasSelection( id ) )
	return;
    if ( mSelections )
	mSelections->remove( id );
    setChanged( true, true );
}

int TQTextParagraph::selectionStart( int id ) const
{
    if ( !mSelections )
	return -1;
    TQMap<int, TQTextParagraphSelection>::ConstIterator it = mSelections->find( id );
    if ( it == mSelections->end() )
	return -1;
    return ( *it ).start;
}

int TQTextParagraph::selectionEnd( int id ) const
{
    if ( !mSelections )
	return -1;
    TQMap<int, TQTextParagraphSelection>::ConstIterator it = mSelections->find( id );
    if ( it == mSelections->end() )
	return -1;
    return ( *it ).end;
}

bool TQTextParagraph::hasSelection( int id ) const
{
    return mSelections ? mSelections->contains( id ) : false;
}

bool TQTextParagraph::fullSelected( int id ) const
{
    if ( !mSelections )
	return false;
    TQMap<int, TQTextParagraphSelection>::ConstIterator it = mSelections->find( id );
    if ( it == mSelections->end() )
	return false;
    return ( *it ).start == 0 && ( *it ).end == str->length() - 1;
}

int TQTextParagraph::lineY( int l ) const
{
    if ( l > (int)lineStarts.count() - 1 ) {
	tqWarning( "TQTextParagraph::lineY: line %d out of range!", l );
	return 0;
    }

    if ( !isValid() )
	( (TQTextParagraph*)this )->format();

    TQMap<int, TQTextLineStart*>::ConstIterator it = lineStarts.begin();
    while ( l-- > 0 )
	++it;
    return ( *it )->y;
}

int TQTextParagraph::lineBaseLine( int l ) const
{
    if ( l > (int)lineStarts.count() - 1 ) {
	tqWarning( "TQTextParagraph::lineBaseLine: line %d out of range!", l );
	return 10;
    }

    if ( !isValid() )
	( (TQTextParagraph*)this )->format();

    TQMap<int, TQTextLineStart*>::ConstIterator it = lineStarts.begin();
    while ( l-- > 0 )
	++it;
    return ( *it )->baseLine;
}

int TQTextParagraph::lineHeight( int l ) const
{
    if ( l > (int)lineStarts.count() - 1 ) {
	tqWarning( "TQTextParagraph::lineHeight: line %d out of range!", l );
	return 15;
    }

    if ( !isValid() )
	( (TQTextParagraph*)this )->format();

    TQMap<int, TQTextLineStart*>::ConstIterator it = lineStarts.begin();
    while ( l-- > 0 )
	++it;
    return ( *it )->h;
}

void TQTextParagraph::lineInfo( int l, int &y, int &h, int &bl ) const
{
    if ( l > (int)lineStarts.count() - 1 ) {
	tqWarning( "TQTextParagraph::lineInfo: line %d out of range!", l );
	tqDebug( "%d %d", (int)lineStarts.count() - 1, l );
	y = 0;
	h = 15;
	bl = 10;
	return;
    }

    if ( !isValid() )
	( (TQTextParagraph*)this )->format();

    TQMap<int, TQTextLineStart*>::ConstIterator it = lineStarts.begin();
    while ( l-- > 0 )
	++it;
    y = ( *it )->y;
    h = ( *it )->h;
    bl = ( *it )->baseLine;
}


void TQTextParagraph::setAlignment( int a )
{
    if ( a == (int)align )
	return;
    align = a;
    invalidate( 0 );
}

TQTextFormatter *TQTextParagraph::formatter() const
{
    if ( hasdoc )
	return document()->formatter();
    if ( pseudoDocument()->pFormatter )
	return pseudoDocument()->pFormatter;
    return ( ( (TQTextParagraph*)this )->pseudoDocument()->pFormatter = new TQTextFormatterBreakWords );
}

void TQTextParagraph::setTabArray( int *a )
{
    delete [] tArray;
    tArray = a;
}

void TQTextParagraph::setTabStops( int tw )
{
    if ( hasdoc )
	document()->setTabStops( tw );
    else
	tabStopWidth = tw;
}

TQMap<int, TQTextParagraphSelection> &TQTextParagraph::selections() const
{
    if ( !mSelections )
	((TQTextParagraph *)this)->mSelections = new TQMap<int, TQTextParagraphSelection>;
    return *mSelections;
}

#ifndef TQT_NO_TEXTCUSTOMITEM
TQPtrList<TQTextCustomItem> &TQTextParagraph::floatingItems() const
{
    if ( !mFloatingItems )
	((TQTextParagraph *)this)->mFloatingItems = new TQPtrList<TQTextCustomItem>;
    return *mFloatingItems;
}
#endif

TQTextStringChar::~TQTextStringChar()
{
    if ( format() )
	format()->removeRef();
    if ( type ) // not Regular
	delete d.custom;
}

TQTextParagraphPseudoDocument::TQTextParagraphPseudoDocument():pFormatter(0),commandHistory(0), minw(0),wused(0),collection(){}
TQTextParagraphPseudoDocument::~TQTextParagraphPseudoDocument(){ delete pFormatter; delete commandHistory; }


#endif //TQT_NO_RICHTEXT
