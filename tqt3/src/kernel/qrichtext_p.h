/****************************************************************************
**
** Definition of internal rich text classes
**
** Created : 990124
**
** Copyright (C) 1999-2008 Trolltech ASA.  All rights reserved.
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

#ifndef TQRICHTEXT_P_H
#define TQRICHTEXT_P_H

//
//  W A R N I N G
//  -------------
//
// This file is not part of the TQt API.  It exists for the convenience
// of a number of TQt sources files.  This header file may change from
// version to version without notice, or even be removed.
//
// We mean it.
//
//

#ifndef QT_H
#include "ntqstring.h"
#include "ntqptrlist.h"
#include "ntqrect.h"
#include "ntqfontmetrics.h"
#include "ntqintdict.h"
#include "ntqmap.h"
#include "ntqstringlist.h"
#include "ntqfont.h"
#include "ntqcolor.h"
#include "ntqsize.h"
#include "ntqvaluelist.h"
#include "ntqvaluestack.h"
#include "ntqobject.h"
#include "ntqdict.h"
#include "ntqpixmap.h"
#include "ntqstylesheet.h"
#include "ntqptrvector.h"
#include "ntqpainter.h"
#include "ntqlayout.h"
#include "ntqobject.h"
#include "ntqapplication.h"
#endif // QT_H

#ifndef TQT_NO_RICHTEXT

class TQTextDocument;
class TQTextString;
class TQTextPreProcessor;
class TQTextFormat;
class TQTextCursor;
class TQTextParagraph;
class TQTextFormatter;
class TQTextIndent;
class TQTextFormatCollection;
class TQStyleSheetItem;
#ifndef TQT_NO_TEXTCUSTOMITEM
class TQTextCustomItem;
#endif
class TQTextFlow;
struct TQBidiContext;

// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

class TQ_EXPORT TQTextStringChar
{
    friend class TQTextString;

public:
    // this is never called, initialize variables in TQTextString::insert()!!!
    TQTextStringChar() : nobreak(false), lineStart( 0 ), type( Regular ) {d.format=0;}
    ~TQTextStringChar();

    struct CustomData
    {
	TQTextFormat *format;
#ifndef TQT_NO_TEXTCUSTOMITEM
	TQTextCustomItem *custom;
#endif
	TQString anchorName;
	TQString anchorHref;
    };
    enum Type { Regular=0, Custom=1, Anchor=2, CustomAnchor=3 };

    TQChar c;
    // this is the same struct as in qtextengine_p.h. Don't change!
    uchar softBreak      :1;     // Potential linebreak point
    uchar whiteSpace     :1;     // A unicode whitespace character, except NBSP, ZWNBSP
    uchar charStop       :1;     // Valid cursor position (for left/right arrow)
    uchar wordStop       :1;     // Valid cursor position (for ctrl + left/right arrow)
    uchar nobreak        :1;

    uchar lineStart : 1;
    uchar /*Type*/ type : 2;
    uchar bidiLevel       :7;
    uchar rightToLeft : 1;

    int x;
    union {
	TQTextFormat* format;
	CustomData* custom;
    } d;


    int height() const;
    int ascent() const;
    int descent() const;
    bool isCustom() const { return (type & Custom) != 0; }
    TQTextFormat *format() const;
#ifndef TQT_NO_TEXTCUSTOMITEM
    TQTextCustomItem *customItem() const;
#endif
    void setFormat( TQTextFormat *f );
#ifndef TQT_NO_TEXTCUSTOMITEM
    void setCustomItem( TQTextCustomItem *i );
#endif

#ifndef TQT_NO_TEXTCUSTOMITEM
    void loseCustomItem();
#endif


    bool isAnchor() const { return ( type & Anchor) != 0; }
    bool isLink() const { return isAnchor() && !!d.custom->anchorHref; }
    TQString anchorName() const;
    TQString anchorHref() const;
    void setAnchor( const TQString& name, const TQString& href );

private:
    TQTextStringChar &operator=( const TQTextStringChar & ) {
	//abort();
	return *this;
    }
    TQTextStringChar( const TQTextStringChar & ) {
    }
    friend class TQTextParagraph;
};

#if defined(Q_TEMPLATEDLL)
// MOC_SKIP_BEGIN
Q_TEMPLATE_EXTERN template class TQ_EXPORT TQMemArray<TQTextStringChar>;
// MOC_SKIP_END
#endif

class TQ_EXPORT TQTextString
{
public:

    TQTextString();
    TQTextString( const TQTextString &s );
    virtual ~TQTextString();

    static TQString toString( const TQMemArray<TQTextStringChar> &data );
    TQString toString() const;

    inline TQTextStringChar &at( int i ) const { return data[ i ]; }
    inline int length() const { return data.size(); }

    int width( int idx ) const;

    void insert( int index, const TQString &s, TQTextFormat *f );
    void insert( int index, const TQChar *unicode, int len, TQTextFormat *f );
    void insert( int index, TQTextStringChar *c, bool doAddRefFormat = false );
    void truncate( int index );
    void remove( int index, int len );
    void clear();

    void setFormat( int index, TQTextFormat *f, bool useCollection );

    void setBidi( bool b ) { bidi = b; }
    bool isBidi() const;
    bool isRightToLeft() const;
    TQChar::Direction direction() const;
    void setDirection( TQChar::Direction d ) { dir = d; bidiDirty = true; }

    TQMemArray<TQTextStringChar> rawData() const { return data.copy(); }

    void operator=( const TQString &s ) { clear(); insert( 0, s, 0 ); }
    void operator+=( const TQString &s ) { insert( length(), s, 0 ); }
    void prepend( const TQString &s ) { insert( 0, s, 0 ); }
    int appendParagraphs( TQTextParagraph *start, TQTextParagraph *end );

    // return next and previous valid cursor positions.
    bool validCursorPosition( int idx );
    int nextCursorPosition( int idx );
    int previousCursorPosition( int idx );

private:
    void checkBidi() const;

    TQMemArray<TQTextStringChar> data;
    TQString stringCache;
    uint bidiDirty : 1;
    uint bidi : 1; // true when the paragraph has right to left characters
    uint rightToLeft : 1;
    uint dir : 5;
};

inline bool TQTextString::isBidi() const
{
    if ( bidiDirty )
	checkBidi();
    return bidi;
}

inline bool TQTextString::isRightToLeft() const
{
    if ( bidiDirty )
	checkBidi();
    return rightToLeft;
}

inline TQString TQTextString::toString() const
{
    if(bidiDirty)
        checkBidi();
    return stringCache;
}

inline TQChar::Direction TQTextString::direction() const
{
    return (TQChar::Direction) dir;
}

inline int TQTextString::nextCursorPosition( int next )
{
    if ( bidiDirty )
	checkBidi();

    const TQTextStringChar *c = data.data();
    int len = length();

    if ( next < len - 1 ) {
	next++;
	while ( next < len - 1 && !c[next].charStop )
	    next++;
    }
    return next;
}

inline int TQTextString::previousCursorPosition( int prev )
{
    if ( bidiDirty )
	checkBidi();

    const TQTextStringChar *c = data.data();

    if ( prev ) {
	prev--;
	while ( prev && !c[prev].charStop )
	    prev--;
    }
    return prev;
}

inline bool TQTextString::validCursorPosition( int idx )
{
    if ( bidiDirty )
	checkBidi();

    return (at( idx ).charStop);
}

// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

#if defined(Q_TEMPLATEDLL)
// MOC_SKIP_BEGIN
Q_TEMPLATE_EXTERN template class TQ_EXPORT TQValueStack<int>;
Q_TEMPLATE_EXTERN template class TQ_EXPORT TQValueStack<TQTextParagraph*>;
Q_TEMPLATE_EXTERN template class TQ_EXPORT TQValueStack<bool>;
// MOC_SKIP_END
#endif

class TQ_EXPORT TQTextCursor
{
public:
    TQTextCursor( TQTextDocument *d = 0 );
    TQTextCursor( const TQTextCursor &c );
    TQTextCursor &operator=( const TQTextCursor &c );
    virtual ~TQTextCursor() {}

    bool operator==( const TQTextCursor &c ) const;
    bool operator!=( const TQTextCursor &c ) const { return !(*this == c); }

    inline TQTextParagraph *paragraph() const { return para; }

    TQTextDocument *document() const;
    int index() const;

    void gotoPosition( TQTextParagraph* p, int index = 0);
    void setIndex( int index ) { gotoPosition(paragraph(), index ); }
    void setParagraph( TQTextParagraph*p ) { gotoPosition(p, 0 ); }

    void gotoLeft();
    void gotoRight();
    void gotoNextLetter();
    void gotoPreviousLetter();
    void gotoUp();
    void gotoDown();
    void gotoLineEnd();
    void gotoLineStart();
    void gotoHome();
    void gotoEnd();
    void gotoPageUp( int visibleHeight );
    void gotoPageDown( int visibleHeight );
    void gotoNextWord( bool onlySpace = false );
    void gotoPreviousWord( bool onlySpace = false );
    void gotoWordLeft();
    void gotoWordRight();

    void insert( const TQString &s, bool checkNewLine, TQMemArray<TQTextStringChar> *formatting = 0 );
    void splitAndInsertEmptyParagraph( bool ind = true, bool updateIds = true );
    bool remove();
    bool removePreviousChar();
    void indent();

    bool atParagStart();
    bool atParagEnd();

    int x() const; // x in current paragraph
    int y() const; // y in current paragraph

    int globalX() const;
    int globalY() const;

    TQTextParagraph *topParagraph() const { return paras.isEmpty() ? para : paras.first(); }
    int offsetX() const { return ox; } // inner document  offset
    int offsetY() const { return oy; } // inner document offset
    int totalOffsetX() const; // total document offset
    int totalOffsetY() const; // total document offset

    bool place( const TQPoint &pos, TQTextParagraph *s ) { return place( pos, s, false ); }
    bool place( const TQPoint &pos, TQTextParagraph *s, bool link ) { return place( pos, s, link, true, true ); }
    bool place( const TQPoint &pos, TQTextParagraph *s, bool link, bool loosePlacing, bool matchBetweenCharacters );
    void restoreState();


    int nestedDepth() const { return (int)indices.count(); } //### size_t/int cast
    void oneUp() { if ( !indices.isEmpty() ) pop(); }
    void setValid( bool b ) { valid = b; }
    bool isValid() const { return valid; }

    void fixCursorPosition();
private:
    enum Operation { EnterBegin, EnterEnd, Next, Prev, Up, Down };

    void push();
    void pop();
    bool processNesting( Operation op );
    void invalidateNested();
    void gotoIntoNested( const TQPoint &globalPos );

    TQTextParagraph *para;
    int idx, tmpX;
    int ox, oy;
    TQValueStack<int> indices;
    TQValueStack<TQTextParagraph*> paras;
    TQValueStack<int> xOffsets;
    TQValueStack<int> yOffsets;
    uint valid : 1;

};

// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

class TQ_EXPORT TQTextCommand
{
public:
    enum Commands { Invalid, Insert, Delete, Format, Style };

    TQTextCommand( TQTextDocument *d ) : doc( d ), cursor( d ) {}
    virtual ~TQTextCommand();

    virtual Commands type() const;

    virtual TQTextCursor *execute( TQTextCursor *c ) = 0;
    virtual TQTextCursor *unexecute( TQTextCursor *c ) = 0;

protected:
    TQTextDocument *doc;
    TQTextCursor cursor;

};

#if defined(Q_TEMPLATEDLL)
// MOC_SKIP_BEGIN
Q_TEMPLATE_EXTERN template class TQ_EXPORT TQPtrList<TQTextCommand>;
// MOC_SKIP_END
#endif

class TQ_EXPORT TQTextCommandHistory
{
public:
    TQTextCommandHistory( int s ) : current( -1 ), steps( s ) { history.setAutoDelete( true ); }
    virtual ~TQTextCommandHistory();

    void clear() { history.clear(); current = -1; }

    void addCommand( TQTextCommand *cmd );
    TQTextCursor *undo( TQTextCursor *c );
    TQTextCursor *redo( TQTextCursor *c );

    bool isUndoAvailable();
    bool isRedoAvailable();

    void setUndoDepth( int d ) { steps = d; }
    int undoDepth() const { return steps; }

    int historySize() const { return history.count(); }
    int currentPosition() const { return current; }

private:
    TQPtrList<TQTextCommand> history;
    int current, steps;

};

inline TQTextCommandHistory::~TQTextCommandHistory()
{
    clear();
}

// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

#ifndef TQT_NO_TEXTCUSTOMITEM
class TQ_EXPORT TQTextCustomItem
{
public:
    TQTextCustomItem( TQTextDocument *p )
	:  xpos(0), ypos(-1), width(-1), height(0), parent( p )
    {}
    virtual ~TQTextCustomItem();
    virtual void draw(TQPainter* p, int x, int y, int cx, int cy, int cw, int ch, const TQColorGroup& cg, bool selected ) = 0;

    virtual void adjustToPainter( TQPainter* );

    enum Placement { PlaceInline = 0, PlaceLeft, PlaceRight };
    virtual Placement placement() const;
    bool placeInline() { return placement() == PlaceInline; }

    virtual bool ownLine() const;
    virtual void resize( int nwidth );
    virtual void invalidate();
    virtual int ascent() const { return height; }

    virtual bool isNested() const;
    virtual int minimumWidth() const;

    virtual TQString richText() const;

    int xpos; // used for floating items
    int ypos; // used for floating items
    int width;
    int height;

    TQRect geometry() const { return TQRect( xpos, ypos, width, height ); }

    virtual bool enter( TQTextCursor *, TQTextDocument *&doc, TQTextParagraph *&parag, int &idx, int &ox, int &oy, bool atEnd = false );
    virtual bool enterAt( TQTextCursor *, TQTextDocument *&doc, TQTextParagraph *&parag, int &idx, int &ox, int &oy, const TQPoint & );
    virtual bool next( TQTextCursor *, TQTextDocument *&doc, TQTextParagraph *&parag, int &idx, int &ox, int &oy );
    virtual bool prev( TQTextCursor *, TQTextDocument *&doc, TQTextParagraph *&parag, int &idx, int &ox, int &oy );
    virtual bool down( TQTextCursor *, TQTextDocument *&doc, TQTextParagraph *&parag, int &idx, int &ox, int &oy );
    virtual bool up( TQTextCursor *, TQTextDocument *&doc, TQTextParagraph *&parag, int &idx, int &ox, int &oy );

    virtual void setParagraph( TQTextParagraph *p ) { parag = p; }
    TQTextParagraph *paragraph() const { return parag; }

    TQTextDocument *parent;
    TQTextParagraph *parag;

    virtual void pageBreak( int  y, TQTextFlow* flow );
};
#endif

#if defined(Q_TEMPLATEDLL)
// MOC_SKIP_BEGIN
//Q_TEMPLATE_EXTERN template class TQ_EXPORT TQMap<TQString, TQString>;
// MOC_SKIP_END
#endif

#ifndef TQT_NO_TEXTCUSTOMITEM
class TQ_EXPORT TQTextImage : public TQTextCustomItem
{
public:
    TQTextImage( TQTextDocument *p, const TQMap<TQString, TQString> &attr, const TQString& context,
		TQMimeSourceFactory &factory );
    virtual ~TQTextImage();

    Placement placement() const { return place; }
    void adjustToPainter( TQPainter* );
    int minimumWidth() const { return width; }

    TQString richText() const;

    void draw( TQPainter* p, int x, int y, int cx, int cy, int cw, int ch, const TQColorGroup& cg, bool selected );

private:
    TQRegion* reg;
    TQPixmap pm;
    Placement place;
    int tmpwidth, tmpheight;
    TQMap<TQString, TQString> attributes;
    TQString imgId;

};
#endif

#ifndef TQT_NO_TEXTCUSTOMITEM
class TQ_EXPORT TQTextHorizontalLine : public TQTextCustomItem
{
public:
    TQTextHorizontalLine( TQTextDocument *p, const TQMap<TQString, TQString> &attr, const TQString& context,
			 TQMimeSourceFactory &factory );
    virtual ~TQTextHorizontalLine();

    void adjustToPainter( TQPainter* );
    void draw(TQPainter* p, int x, int y, int cx, int cy, int cw, int ch, const TQColorGroup& cg, bool selected );
    TQString richText() const;

    bool ownLine() const { return true; }

private:
    int tmpheight;
    TQColor color;
    bool shade;

};
#endif

#ifndef TQT_NO_TEXTCUSTOMITEM
#if defined(Q_TEMPLATEDLL)
// MOC_SKIP_BEGIN
Q_TEMPLATE_EXTERN template class TQ_EXPORT TQPtrList<TQTextCustomItem>;
// MOC_SKIP_END
#endif
#endif

class TQ_EXPORT TQTextFlow
{
    friend class TQTextDocument;
#ifndef TQT_NO_TEXTCUSTOMITEM
    friend class TQTextTableCell;
#endif

public:
    TQTextFlow();
    virtual ~TQTextFlow();

    virtual void setWidth( int width );
    int width() const;

    virtual void setPageSize( int ps );
    int pageSize() const { return pagesize; }

    virtual int adjustLMargin( int yp, int h, int margin, int space );
    virtual int adjustRMargin( int yp, int h, int margin, int space );

#ifndef TQT_NO_TEXTCUSTOMITEM
    virtual void registerFloatingItem( TQTextCustomItem* item );
    virtual void unregisterFloatingItem( TQTextCustomItem* item );
#endif
    virtual TQRect boundingRect() const;
    virtual void drawFloatingItems(TQPainter* p, int cx, int cy, int cw, int ch, const TQColorGroup& cg, bool selected );

    virtual int adjustFlow( int  y, int w, int h ); // adjusts y according to the defined pagesize. Returns the shift.

    virtual bool isEmpty();

    void clear();

private:
    int w;
    int pagesize;

#ifndef TQT_NO_TEXTCUSTOMITEM
    TQPtrList<TQTextCustomItem> leftItems;
    TQPtrList<TQTextCustomItem> rightItems;
#endif
};

inline int TQTextFlow::width() const { return w; }

#ifndef TQT_NO_TEXTCUSTOMITEM
class TQTextTable;

class TQ_EXPORT TQTextTableCell : public TQLayoutItem
{
    friend class TQTextTable;

public:
    TQTextTableCell( TQTextTable* table,
		    int row, int column,
		    const TQMap<TQString, TQString> &attr,
		    const TQStyleSheetItem* style,
		    const TQTextFormat& fmt, const TQString& context,
		    TQMimeSourceFactory &factory, TQStyleSheet *sheet, const TQString& doc );
    virtual ~TQTextTableCell();

    TQSize sizeHint() const ;
    TQSize minimumSize() const ;
    TQSize maximumSize() const ;
    TQSizePolicy::ExpandData expanding() const;
    bool isEmpty() const;
    void setGeometry( const TQRect& ) ;
    TQRect geometry() const;

    bool hasHeightForWidth() const;
    int heightForWidth( int ) const;

    void adjustToPainter( TQPainter* );

    int row() const { return row_; }
    int column() const { return col_; }
    int rowspan() const { return rowspan_; }
    int colspan() const { return colspan_; }
    int stretch() const { return stretch_; }

    TQTextDocument* richText()  const { return richtext; }
    TQTextTable* table() const { return parent; }

    void draw( TQPainter* p, int x, int y, int cx, int cy, int cw, int ch, const TQColorGroup& cg, bool selected );

    TQBrush *backGround() const { return background; }
    virtual void invalidate();

    int verticalAlignmentOffset() const;
    int horizontalAlignmentOffset() const;

private:
    TQRect geom;
    TQTextTable* parent;
    TQTextDocument* richtext;
    int row_;
    int col_;
    int rowspan_;
    int colspan_;
    int stretch_;
    int maxw;
    int minw;
    bool hasFixedWidth;
    TQBrush *background;
    int cached_width;
    int cached_sizehint;
    TQMap<TQString, TQString> attributes;
    int align;
};
#endif

#if defined(Q_TEMPLATEDLL)
// MOC_SKIP_BEGIN
Q_TEMPLATE_EXTERN template class TQ_EXPORT TQPtrList<TQTextTableCell>;
Q_TEMPLATE_EXTERN template class TQ_EXPORT TQMap<TQTextCursor*, int>;
// MOC_SKIP_END
#endif

#ifndef TQT_NO_TEXTCUSTOMITEM
class TQ_EXPORT TQTextTable: public TQTextCustomItem
{
    friend class TQTextTableCell;

public:
    TQTextTable( TQTextDocument *p, const TQMap<TQString, TQString> &attr );
    virtual ~TQTextTable();

    void adjustToPainter( TQPainter *p );
    void pageBreak( int  y, TQTextFlow* flow );
    void draw( TQPainter* p, int x, int y, int cx, int cy, int cw, int ch,
	       const TQColorGroup& cg, bool selected );

    bool noErase() const { return true; }
    bool ownLine() const { return true; }
    Placement placement() const { return place; }
    bool isNested() const { return true; }
    void resize( int nwidth );
    virtual void invalidate();

    virtual bool enter( TQTextCursor *c, TQTextDocument *&doc, TQTextParagraph *&parag, int &idx, int &ox, int &oy, bool atEnd = false );
    virtual bool enterAt( TQTextCursor *c, TQTextDocument *&doc, TQTextParagraph *&parag, int &idx, int &ox, int &oy, const TQPoint &pos );
    virtual bool next( TQTextCursor *c, TQTextDocument *&doc, TQTextParagraph *&parag, int &idx, int &ox, int &oy );
    virtual bool prev( TQTextCursor *c, TQTextDocument *&doc, TQTextParagraph *&parag, int &idx, int &ox, int &oy );
    virtual bool down( TQTextCursor *c, TQTextDocument *&doc, TQTextParagraph *&parag, int &idx, int &ox, int &oy );
    virtual bool up( TQTextCursor *c, TQTextDocument *&doc, TQTextParagraph *&parag, int &idx, int &ox, int &oy );

    TQString richText() const;

    int minimumWidth() const;

    TQPtrList<TQTextTableCell> tableCells() const { return cells; }

    bool isStretching() const { return stretch; }
    void setParagraph(TQTextParagraph *p);

private:
    void format( int w );
    void addCell( TQTextTableCell* cell );

private:
    TQGridLayout* layout;
    TQPtrList<TQTextTableCell> cells;
    int cachewidth;
    int fixwidth;
    int cellpadding;
    int cellspacing;
    int border;
    int outerborder;
    int stretch;
    int innerborder;
    int us_cp, us_ib, us_b, us_ob, us_cs;
    int us_fixwidth;
    TQMap<TQString, TQString> attributes;
    TQMap<TQTextCursor*, int> currCell;
    Placement place;
    void adjustCells( int y , int shift );
    int pageBreakFor;
};
#endif
// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

#ifndef TQT_NO_TEXTCUSTOMITEM
class TQTextTableCell;
class TQTextParagraph;
#endif

struct TQ_EXPORT TQTextDocumentSelection
{
    TQTextCursor startCursor, endCursor;
    bool swapped;
};

#if defined(Q_TEMPLATEDLL)
// MOC_SKIP_BEGIN
Q_TEMPLATE_EXTERN template class TQ_EXPORT TQMap<int, TQColor>;
//Q_TEMPLATE_EXTERN template class TQ_EXPORT TQMap<int, bool>;
Q_TEMPLATE_EXTERN template class TQ_EXPORT TQMap<int, TQTextDocumentSelection>;
Q_TEMPLATE_EXTERN template class TQ_EXPORT TQPtrList<TQTextDocument>;
// MOC_SKIP_END
#endif

class TQ_EXPORT TQTextDocument : public TQObject
{
    TQ_OBJECT

#ifndef TQT_NO_TEXTCUSTOMITEM
    friend class TQTextTableCell;
#endif
    friend class TQTextCursor;
    friend class TQTextEdit;
    friend class TQTextParagraph;
    friend class TQTextTable;

public:
    enum SelectionIds {
	Standard = 0,
	IMSelectionText		= 31998,
	IMCompositionText	= 31999, // this must be higher!
	Temp = 32000 // This selection must not be drawn, it's used e.g. by undo/redo to
	// remove multiple lines with removeSelectedText()
    };

    TQTextDocument( TQTextDocument *p );
    virtual ~TQTextDocument();

    TQTextDocument *parent() const { return par; }
    TQTextParagraph *parentParagraph() const { return parentPar; }

    void setText( const TQString &text, const TQString &context );
    TQMap<TQString, TQString> attributes() const { return attribs; }
    void setAttributes( const TQMap<TQString, TQString> &attr ) { attribs = attr; }

    TQString text() const;
    TQString text( int parag ) const;
    TQString originalText() const;

    int x() const;
    int y() const;
    int width() const;
    int widthUsed() const;
    int visibleWidth() const;
    int height() const;
    void setWidth( int w );
    int minimumWidth() const;
    bool setMinimumWidth( int needed, int used = -1, TQTextParagraph *parag = 0 );

    void setY( int y );
    int leftMargin() const;
    void setLeftMargin( int lm );
    int rightMargin() const;
    void setRightMargin( int rm );

    TQTextParagraph *firstParagraph() const;
    TQTextParagraph *lastParagraph() const;
    void setFirstParagraph( TQTextParagraph *p );
    void setLastParagraph( TQTextParagraph *p );

    void invalidate();

    void setPreProcessor( TQTextPreProcessor *sh );
    TQTextPreProcessor *preProcessor() const;

    void setFormatter( TQTextFormatter *f );
    TQTextFormatter *formatter() const;

    void setIndent( TQTextIndent *i );
    TQTextIndent *indent() const;

    TQColor selectionColor( int id ) const;
    bool invertSelectionText( int id ) const;
    void setSelectionColor( int id, const TQColor &c );
    void setInvertSelectionText( int id, bool b );
    bool hasSelection( int id, bool visible = false ) const;
    void setSelectionStart( int id, const TQTextCursor &cursor );
    bool setSelectionEnd( int id, const TQTextCursor &cursor );
    void selectAll( int id );
    bool removeSelection( int id );
    void selectionStart( int id, int &paragId, int &index );
    TQTextCursor selectionStartCursor( int id );
    TQTextCursor selectionEndCursor( int id );
    void selectionEnd( int id, int &paragId, int &index );
    void setFormat( int id, TQTextFormat *f, int flags );
    int numSelections() const { return nSelections; }
    void addSelection( int id );

    TQString selectedText( int id, bool asRichText = false ) const;
    void removeSelectedText( int id, TQTextCursor *cursor );
    void indentSelection( int id );

    TQTextParagraph *paragAt( int i ) const;

    void addCommand( TQTextCommand *cmd );
    TQTextCursor *undo( TQTextCursor *c = 0 );
    TQTextCursor *redo( TQTextCursor *c  = 0 );
    TQTextCommandHistory *commands() const { return commandHistory; }

    TQTextFormatCollection *formatCollection() const;

    bool find( TQTextCursor &cursor, const TQString &expr, bool cs, bool wo, bool forward);

    void setTextFormat( TQt::TextFormat f );
    TQt::TextFormat textFormat() const;

    bool inSelection( int selId, const TQPoint &pos ) const;

    TQStyleSheet *styleSheet() const { return sheet_; }
#ifndef TQT_NO_MIME
    TQMimeSourceFactory *mimeSourceFactory() const { return factory_; }
#endif
    TQString context() const { return contxt; }

    void setStyleSheet( TQStyleSheet *s );
    void setDefaultFormat( const TQFont &font, const TQColor &color );
#ifndef TQT_NO_MIME
    void setMimeSourceFactory( TQMimeSourceFactory *f ) { if ( f ) factory_ = f; }
#endif
    void setContext( const TQString &c ) { if ( !c.isEmpty() ) contxt = c; }

    void setUnderlineLinks( bool b );
    bool underlineLinks() const { return underlLinks; }

    void setPaper( TQBrush *brush ) { if ( backBrush ) delete backBrush; backBrush = brush; }
    TQBrush *paper() const { return backBrush; }

    void doLayout( TQPainter *p, int w );
    void draw( TQPainter *p, const TQRect& rect, const TQColorGroup &cg, const TQBrush *paper = 0 );
    bool useDoubleBuffer( TQTextParagraph *parag, TQPainter *p );

    void drawParagraph( TQPainter *p, TQTextParagraph *parag, int cx, int cy, int cw, int ch,
		    TQPixmap *&doubleBuffer, const TQColorGroup &cg,
		    bool drawCursor, TQTextCursor *cursor, bool resetChanged = true );
    TQTextParagraph *draw( TQPainter *p, int cx, int cy, int cw, int ch, const TQColorGroup &cg,
		      bool onlyChanged = false, bool drawCursor = false, TQTextCursor *cursor = 0,
		      bool resetChanged = true );

#ifndef TQT_NO_TEXTCUSTOMITEM
    void registerCustomItem( TQTextCustomItem *i, TQTextParagraph *p );
    void unregisterCustomItem( TQTextCustomItem *i, TQTextParagraph *p );
#endif

    void setFlow( TQTextFlow *f );
    void takeFlow();
    TQTextFlow *flow() const { return flow_; }
    bool isPageBreakEnabled() const { return pages; }
    void setPageBreakEnabled( bool b ) { pages = b; }

    void setUseFormatCollection( bool b ) { useFC = b; }
    bool useFormatCollection() const { return useFC; }

#ifndef TQT_NO_TEXTCUSTOMITEM
    TQTextTableCell *tableCell() const { return tc; }
    void setTableCell( TQTextTableCell *c ) { tc = c; }
#endif

    void setPlainText( const TQString &text );
    void setRichText( const TQString &text, const TQString &context, const TQTextFormat *initialFormat = 0 );
    TQString richText() const;
    TQString plainText() const;

    bool focusNextPrevChild( bool next );

    int alignment() const;
    void setAlignment( int a );

    int *tabArray() const;
    int tabStopWidth() const;
    void setTabArray( int *a );
    void setTabStops( int tw );

    void setUndoDepth( int d ) { commandHistory->setUndoDepth( d ); }
    int undoDepth() const { return commandHistory->undoDepth(); }

    int length() const;
    void clear( bool createEmptyParag = false );

    virtual TQTextParagraph *createParagraph( TQTextDocument *d, TQTextParagraph *pr = 0, TQTextParagraph *nx = 0, bool updateIds = true );
    void insertChild( TQObject *o ) { TQObject::insertChild( o ); }
    void removeChild( TQObject *o ) { TQObject::removeChild( o ); }
    void insertChild( TQTextDocument *d ) { childList.append( d ); }
    void removeChild( TQTextDocument *d ) { childList.removeRef( d ); }
    TQPtrList<TQTextDocument> children() const { return childList; }

    bool hasFocusParagraph() const;
    TQString focusHref() const;
    TQString focusName() const;

    void invalidateOriginalText() { oTextValid = false; oText = ""; }

signals:
    void minimumWidthChanged( int );

private:
    void init();
    TQPixmap *bufferPixmap( const TQSize &s );
    // HTML parser
    bool hasPrefix(const TQChar* doc, int length, int pos, TQChar c);
    bool hasPrefix(const TQChar* doc, int length, int pos, const TQString& s);
#ifndef TQT_NO_TEXTCUSTOMITEM
    TQTextCustomItem* parseTable( const TQMap<TQString, TQString> &attr, const TQTextFormat &fmt,
				 const TQChar* doc, int length, int& pos, TQTextParagraph *curpar );
#endif
    bool eatSpace(const TQChar* doc, int length, int& pos, bool includeNbsp = false );
    bool eat(const TQChar* doc, int length, int& pos, TQChar c);
    TQString parseOpenTag(const TQChar* doc, int length, int& pos, TQMap<TQString, TQString> &attr, bool& emptyTag);
    TQString parseCloseTag( const TQChar* doc, int length, int& pos );
    TQChar parseHTMLSpecialChar(const TQChar* doc, int length, int& pos);
    TQString parseWord(const TQChar* doc, int length, int& pos, bool lower = true);
    TQChar parseChar(const TQChar* doc, int length, int& pos, TQStyleSheetItem::WhiteSpaceMode wsm );
    void setRichTextInternal( const TQString &text, TQTextCursor* cursor = 0, const TQTextFormat *initialFormat = 0 );
    void setRichTextMarginsInternal( TQPtrList< TQPtrVector<TQStyleSheetItem> >& styles, TQTextParagraph* stylesPar );

private:
    struct TQ_EXPORT Focus {
	TQTextParagraph *parag;
	int start, len;
	TQString href;
	TQString name;
    };

    int cx, cy, cw, vw;
    TQTextParagraph *fParag, *lParag;
    TQTextPreProcessor *pProcessor;
    TQMap<int, TQColor> selectionColors;
    TQMap<int, TQTextDocumentSelection> selections;
    TQMap<int, bool> selectionText;
    TQTextCommandHistory *commandHistory;
    TQTextFormatter *pFormatter;
    TQTextIndent *indenter;
    TQTextFormatCollection *fCollection;
    TQt::TextFormat txtFormat;
    uint preferRichText : 1;
    uint pages : 1;
    uint useFC : 1;
    uint withoutDoubleBuffer : 1;
    uint underlLinks : 1;
    uint nextDoubleBuffered : 1;
    uint oTextValid : 1;
    uint mightHaveCustomItems : 1;
    int align;
    int nSelections;
    TQTextFlow *flow_;
    TQTextDocument *par;
    TQTextParagraph *parentPar;
#ifndef TQT_NO_TEXTCUSTOMITEM
    TQTextTableCell *tc;
#endif
    TQBrush *backBrush;
    TQPixmap *buf_pixmap;
    Focus focusIndicator;
    int minw;
    int wused;
    int leftmargin;
    int rightmargin;
    TQTextParagraph *minwParag, *curParag;
    TQStyleSheet* sheet_;
#ifndef TQT_NO_MIME
    TQMimeSourceFactory* factory_;
#endif
    TQString contxt;
    TQMap<TQString, TQString> attribs;
    int *tArray;
    int tStopWidth;
    int uDepth;
    TQString oText;
    TQPtrList<TQTextDocument> childList;
    TQColor linkColor, bodyText;
    double scaleFontsFactor;

    short list_tm,list_bm, list_lm, li_tm, li_bm, par_tm, par_bm;
#if defined(TQ_DISABLE_COPY) // Disabled copy constructor and operator=
    TQTextDocument( const TQTextDocument & );
    TQTextDocument &operator=( const TQTextDocument & );
#endif
};

// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++


class TQ_EXPORT TQTextDeleteCommand : public TQTextCommand
{
public:
    TQTextDeleteCommand( TQTextDocument *d, int i, int idx, const TQMemArray<TQTextStringChar> &str,
			const TQByteArray& oldStyle );
    TQTextDeleteCommand( TQTextParagraph *p, int idx, const TQMemArray<TQTextStringChar> &str );
    virtual ~TQTextDeleteCommand();

    Commands type() const { return Delete; }
    TQTextCursor *execute( TQTextCursor *c );
    TQTextCursor *unexecute( TQTextCursor *c );

protected:
    int id, index;
    TQTextParagraph *parag;
    TQMemArray<TQTextStringChar> text;
    TQByteArray styleInformation;

};

class TQ_EXPORT TQTextInsertCommand : public TQTextDeleteCommand
{
public:
    TQTextInsertCommand( TQTextDocument *d, int i, int idx, const TQMemArray<TQTextStringChar> &str,
			const TQByteArray& oldStyleInfo )
	: TQTextDeleteCommand( d, i, idx, str, oldStyleInfo ) {}
    TQTextInsertCommand( TQTextParagraph *p, int idx, const TQMemArray<TQTextStringChar> &str )
	: TQTextDeleteCommand( p, idx, str ) {}
    virtual ~TQTextInsertCommand() {}

    Commands type() const { return Insert; }
    TQTextCursor *execute( TQTextCursor *c ) { return TQTextDeleteCommand::unexecute( c ); }
    TQTextCursor *unexecute( TQTextCursor *c ) { return TQTextDeleteCommand::execute( c ); }

};

class TQ_EXPORT TQTextFormatCommand : public TQTextCommand
{
public:
    TQTextFormatCommand( TQTextDocument *d, int sid, int sidx, int eid, int eidx, const TQMemArray<TQTextStringChar> &old, TQTextFormat *f, int fl );
    virtual ~TQTextFormatCommand();

    Commands type() const { return Format; }
    TQTextCursor *execute( TQTextCursor *c );
    TQTextCursor *unexecute( TQTextCursor *c );

protected:
    int startId, startIndex, endId, endIndex;
    TQTextFormat *format;
    TQMemArray<TQTextStringChar> oldFormats;
    int flags;

};

class TQ_EXPORT TQTextStyleCommand : public TQTextCommand
{
public:
    TQTextStyleCommand( TQTextDocument *d, int fParag, int lParag, const TQByteArray& beforeChange  );
    virtual ~TQTextStyleCommand() {}

    Commands type() const { return Style; }
    TQTextCursor *execute( TQTextCursor *c );
    TQTextCursor *unexecute( TQTextCursor *c );

    static TQByteArray readStyleInformation(  TQTextDocument* d, int fParag, int lParag );
    static void writeStyleInformation(  TQTextDocument* d, int fParag, const TQByteArray& style );

private:
    int firstParag, lastParag;
    TQByteArray before;
    TQByteArray after;
};

// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

struct TQ_EXPORT TQTextParagraphSelection
{
    TQTextParagraphSelection() : start(0), end(0) { }
    int start, end;
};

struct TQ_EXPORT TQTextLineStart
{
    TQTextLineStart() : y( 0 ), baseLine( 0 ), h( 0 )
    {  }
    TQTextLineStart( int y_, int bl, int h_ ) : y( y_ ), baseLine( bl ), h( h_ ),
	w( 0 )
    {  }

public:
    int y, baseLine, h;
    int w;
};

#if defined(Q_TEMPLATEDLL)
// MOC_SKIP_BEGIN
Q_TEMPLATE_EXTERN template class TQ_EXPORT TQMap<int, TQTextParagraphSelection>;
Q_TEMPLATE_EXTERN template class TQ_EXPORT TQMap<int, TQTextLineStart*>;
// MOC_SKIP_END
#endif

class TQ_EXPORT TQTextParagraphData
{
public:
    TQTextParagraphData() {}
    virtual ~TQTextParagraphData();
    virtual void join( TQTextParagraphData * );
};

class TQTextParagraphPseudoDocument;

class TQSyntaxHighlighter;

class TQ_EXPORT TQTextParagraph
{
    friend class TQTextDocument;
    friend class TQTextCursor;
    friend class TQSyntaxHighlighter;

public:
    TQTextParagraph( TQTextDocument *d, TQTextParagraph *pr = 0, TQTextParagraph *nx = 0, bool updateIds = true );
    ~TQTextParagraph();

    TQTextString *string() const;
    TQTextStringChar *at( int i ) const; // maybe remove later
    int leftGap() const;
    int length() const; // maybe remove later

    void setListStyle( TQStyleSheetItem::ListStyle ls ) { lstyle = ls; changed = true; }
    TQStyleSheetItem::ListStyle listStyle() const { return (TQStyleSheetItem::ListStyle)lstyle; }
    void setListItem( bool li );
    bool isListItem() const { return litem; }
    void setListValue( int v ) { list_val = v; }
    int listValue() const { return list_val > 0 ? list_val : -1; }

    void setListDepth( int depth );
    int listDepth() const { return ldepth; }

//     void setFormat( TQTextFormat *fm );
//     TQTextFormat *paragFormat() const;

    inline TQTextDocument *document() const {
	if (hasdoc) return (TQTextDocument*) docOrPseudo;
	return 0;
    }
    TQTextParagraphPseudoDocument *pseudoDocument() const;

    TQRect rect() const;
    void setHeight( int h ) { r.setHeight( h ); }
    void show();
    void hide();
    bool isVisible() const { return visible; }

    TQTextParagraph *prev() const;
    TQTextParagraph *next() const;
    void setPrev( TQTextParagraph *s );
    void setNext( TQTextParagraph *s );

    void insert( int index, const TQString &s );
    void insert( int index, const TQChar *unicode, int len );
    void append( const TQString &s, bool reallyAtEnd = false );
    void truncate( int index );
    void remove( int index, int len );
    void join( TQTextParagraph *s );

    void invalidate( int chr );

    void move( int &dy );
    void format( int start = -1, bool doMove = true );

    bool isValid() const;
    bool hasChanged() const;
    void setChanged( bool b, bool recursive = false );

    int lineHeightOfChar( int i, int *bl = 0, int *y = 0 ) const;
    TQTextStringChar *lineStartOfChar( int i, int *index = 0, int *line = 0 ) const;
    int lines() const;
    TQTextStringChar *lineStartOfLine( int line, int *index = 0 ) const;
    int lineY( int l ) const;
    int lineBaseLine( int l ) const;
    int lineHeight( int l ) const;
    void lineInfo( int l, int &y, int &h, int &bl ) const;

    void setSelection( int id, int start, int end );
    void removeSelection( int id );
    int selectionStart( int id ) const;
    int selectionEnd( int id ) const;
    bool hasSelection( int id ) const;
    bool hasAnySelection() const;
    bool fullSelected( int id ) const;

    void setEndState( int s );
    int endState() const;

    void setParagId( int i );
    int paragId() const;

    bool firstPreProcess() const;
    void setFirstPreProcess( bool b );

    void indent( int *oldIndent = 0, int *newIndent = 0 );

    void setExtraData( TQTextParagraphData *data );
    TQTextParagraphData *extraData() const;

    TQMap<int, TQTextLineStart*> &lineStartList();

    void setFormat( int index, int len, TQTextFormat *f, bool useCollection = true, int flags = -1 );

    void setAlignment( int a );
    int alignment() const;

    void paint( TQPainter &painter, const TQColorGroup &cg, TQTextCursor *cursor = 0, bool drawSelections = false,
			int clipx = -1, int clipy = -1, int clipw = -1, int cliph = -1 );

    int topMargin() const;
    int bottomMargin() const;
    int leftMargin() const;
    int firstLineMargin() const;
    int rightMargin() const;
    int lineSpacing() const;

#ifndef TQT_NO_TEXTCUSTOMITEM
    void registerFloatingItem( TQTextCustomItem *i );
    void unregisterFloatingItem( TQTextCustomItem *i );
#endif

    void setFullWidth( bool b ) { fullWidth = b; }
    bool isFullWidth() const { return fullWidth; }

#ifndef TQT_NO_TEXTCUSTOMITEM
    TQTextTableCell *tableCell() const;
#endif

    TQBrush *background() const;

    int documentWidth() const;
    int documentVisibleWidth() const;
    int documentX() const;
    int documentY() const;
    TQTextFormatCollection *formatCollection() const;
    TQTextFormatter *formatter() const;

    int nextTab( int i, int x );
    int *tabArray() const;
    void setTabArray( int *a );
    void setTabStops( int tw );

    void adjustToPainter( TQPainter *p );

    void setNewLinesAllowed( bool b );
    bool isNewLinesAllowed() const;

    TQString richText() const;

    void addCommand( TQTextCommand *cmd );
    TQTextCursor *undo( TQTextCursor *c = 0 );
    TQTextCursor *redo( TQTextCursor *c  = 0 );
    TQTextCommandHistory *commands() const;
    void copyParagData( TQTextParagraph *parag );

    void setBreakable( bool b ) { breakable = b; }
    bool isBreakable() const { return breakable; }

    void setBackgroundColor( const TQColor &c );
    TQColor *backgroundColor() const { return bgcol; }
    void clearBackgroundColor();

    void setMovedDown( bool b ) { movedDown = b; }
    bool wasMovedDown() const { return movedDown; }

    void setDirection( TQChar::Direction d );
    TQChar::Direction direction() const;
    void setPaintDevice( TQPaintDevice *pd ) { paintdevice = pd; }

    void readStyleInformation( TQDataStream& stream );
    void writeStyleInformation( TQDataStream& stream ) const;

protected:
    void setColorForSelection( TQColor &c, TQPainter &p, const TQColorGroup& cg, int selection );
    void drawLabel( TQPainter* p, int x, int y, int w, int h, int base, const TQColorGroup& cg );
    void drawString( TQPainter &painter, const TQString &str, int start, int len, int xstart,
			     int y, int baseLine, int w, int h, bool drawSelections, int fullSelectionWidth,
			     TQTextStringChar *formatChar, const TQColorGroup& cg,
			     bool rightToLeft );

private:
    TQMap<int, TQTextParagraphSelection> &selections() const;
#ifndef TQT_NO_TEXTCUSTOMITEM
    TQPtrList<TQTextCustomItem> &floatingItems() const;
#endif
    TQBrush backgroundBrush( const TQColorGroup&cg ) { if ( bgcol ) return *bgcol; return cg.brush( TQColorGroup::Base ); }
    void invalidateStyleCache();

    TQMap<int, TQTextLineStart*> lineStarts;
    TQRect r;
    TQTextParagraph *p, *n;
    void *docOrPseudo;
    uint changed : 1;
    uint firstFormat : 1;
    uint firstPProcess : 1;
    uint needPreProcess : 1;
    uint fullWidth : 1;
    uint lastInFrame : 1;
    uint visible : 1;
    uint breakable : 1;
    uint movedDown : 1;
    uint mightHaveCustomItems : 1;
    uint hasdoc : 1;
    uint litem : 1; // whether the paragraph is a list item
    uint rtext : 1; // whether the paragraph needs rich text margin
    int align : 4;
    uint /*TQStyleSheetItem::ListStyle*/ lstyle : 4;
    int invalid;
    int state, id;
    TQTextString *str;
    TQMap<int, TQTextParagraphSelection> *mSelections;
#ifndef TQT_NO_TEXTCUSTOMITEM
    TQPtrList<TQTextCustomItem> *mFloatingItems;
#endif
    short utm, ubm, ulm, urm, uflm, ulinespacing;
    short tabStopWidth;
    int minwidth;
    int *tArray;
    TQTextParagraphData *eData;
    short list_val;
    ushort ldepth;
    TQColor *bgcol;
    TQPaintDevice *paintdevice;
};

// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

class TQ_EXPORT TQTextFormatter
{
public:
    TQTextFormatter();
    virtual ~TQTextFormatter();

    virtual int format( TQTextDocument *doc, TQTextParagraph *parag, int start, const TQMap<int, TQTextLineStart*> &oldLineStarts ) = 0;
    virtual int formatVertically( TQTextDocument* doc, TQTextParagraph* parag );

    bool isWrapEnabled( TQTextParagraph *p ) const { if ( !wrapEnabled ) return false; if ( p && !p->isBreakable() ) return false; return true;}
    int wrapAtColumn() const { return wrapColumn;}
    virtual void setWrapEnabled( bool b );
    virtual void setWrapAtColumn( int c );
    virtual void setAllowBreakInWords( bool b ) { biw = b; }
    bool allowBreakInWords() const { return biw; }

    int minimumWidth() const { return thisminw; }
    int widthUsed() const { return thiswused; }

protected:
    virtual TQTextLineStart *formatLine( TQTextParagraph *parag, TQTextString *string, TQTextLineStart *line, TQTextStringChar *start,
					       TQTextStringChar *last, int align = TQt::AlignAuto, int space = 0 );
#ifndef TQT_NO_COMPLEXTEXT
    virtual TQTextLineStart *bidiReorderLine( TQTextParagraph *parag, TQTextString *string, TQTextLineStart *line, TQTextStringChar *start,
						    TQTextStringChar *last, int align, int space );
#endif
    void insertLineStart( TQTextParagraph *parag, int index, TQTextLineStart *ls );

    int thisminw;
    int thiswused;

private:
    bool wrapEnabled;
    int wrapColumn;
    bool biw;

#ifdef HAVE_THAI_BREAKS
    static TQCString *thaiCache;
    static TQTextString *cachedString;
    static ThBreakIterator *thaiIt;
#endif
};

// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

class TQ_EXPORT TQTextFormatterBreakInWords : public TQTextFormatter
{
public:
    TQTextFormatterBreakInWords();
    virtual ~TQTextFormatterBreakInWords() {}

    int format( TQTextDocument *doc, TQTextParagraph *parag, int start, const TQMap<int, TQTextLineStart*> &oldLineStarts );

};

// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

class TQ_EXPORT TQTextFormatterBreakWords : public TQTextFormatter
{
public:
    TQTextFormatterBreakWords();
    virtual ~TQTextFormatterBreakWords() {}

    int format( TQTextDocument *doc, TQTextParagraph *parag, int start, const TQMap<int, TQTextLineStart*> &oldLineStarts );

};

// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

class TQ_EXPORT TQTextIndent
{
public:
    TQTextIndent();
    virtual ~TQTextIndent() {}

    virtual void indent( TQTextDocument *doc, TQTextParagraph *parag, int *oldIndent = 0, int *newIndent = 0 ) = 0;

};

// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

class TQ_EXPORT TQTextPreProcessor
{
public:
    enum Ids {
	Standard = 0
    };

    TQTextPreProcessor();
    virtual ~TQTextPreProcessor() {}

    virtual void process( TQTextDocument *doc, TQTextParagraph *, int, bool = true ) = 0;
    virtual TQTextFormat *format( int id ) = 0;

};

// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

class TQ_EXPORT TQTextFormat
{
    friend class TQTextFormatCollection;
    friend class TQTextDocument;

public:
    enum Flags {
	NoFlags,
	Bold = 1,
	Italic = 2,
	Underline = 4,
	Family = 8,
	Size = 16,
	Color = 32,
	Misspelled = 64,
	VAlign = 128,
	StrikeOut= 256,
	Font = Bold | Italic | Underline | Family | Size | StrikeOut,
	Format = Font | Color | Misspelled | VAlign
    };

    enum VerticalAlignment { AlignNormal, AlignSuperScript, AlignSubScript };

    TQTextFormat();
    virtual ~TQTextFormat();

    TQTextFormat( const TQStyleSheetItem *s );
    TQTextFormat( const TQFont &f, const TQColor &c, TQTextFormatCollection *parent = 0 );
    TQTextFormat( const TQTextFormat &fm );
    TQTextFormat makeTextFormat( const TQStyleSheetItem *style, const TQMap<TQString,TQString>& attr, double scaleFontsFactor ) const;
    TQTextFormat& operator=( const TQTextFormat &fm );
    TQColor color() const;
    TQFont font() const;
    TQFontMetrics fontMetrics() const { return fm; }
    bool isMisspelled() const;
    VerticalAlignment vAlign() const;
    int minLeftBearing() const;
    int minRightBearing() const;
    int width( const TQChar &c ) const;
    int width( const TQString &str, int pos ) const;
    int height() const;
    int ascent() const;
    int descent() const;
    int leading() const;
    bool useLinkColor() const;

    void setBold( bool b );
    void setItalic( bool b );
    void setUnderline( bool b );
    void setStrikeOut( bool b );
    void setFamily( const TQString &f );
    void setPointSize( int s );
    void setFont( const TQFont &f );
    void setColor( const TQColor &c );
    void setMisspelled( bool b );
    void setVAlign( VerticalAlignment a );

    bool operator==( const TQTextFormat &f ) const;
    TQTextFormatCollection *parent() const;
    const TQString &key() const;

    static TQString getKey( const TQFont &f, const TQColor &c, bool misspelled, VerticalAlignment vAlign );

    void addRef();
    void removeRef();

    TQString makeFormatChangeTags( TQTextFormat* defaultFormat, TQTextFormat *f, const TQString& oldAnchorHref, const TQString& anchorHref ) const;
    TQString makeFormatEndTags( TQTextFormat* defaultFormat, const TQString& anchorHref ) const;

    static void setPainter( TQPainter *p );
    static TQPainter* painter();

    bool fontSizesInPixels() { return usePixelSizes; }

protected:
    virtual void generateKey();

private:
    void update();
    static void applyFont( const TQFont &f );
    static void cleanupPrivateData();

private:
    TQFont fn;
    TQColor col;
    TQFontMetrics fm;
    uint missp : 1;
    uint linkColor : 1;
    uint usePixelSizes : 1;
    int leftBearing, rightBearing;
    VerticalAlignment ha;
    uchar widths[ 256 ];
    int hei, asc, dsc;
    TQTextFormatCollection *collection;
    int ref;
    TQString k;
    int logicalFontSize;
    int stdSize;
    static TQPainter *pntr;
    static TQFontMetrics *pntr_fm;
    static int pntr_asc;
    static int pntr_hei;
    static int pntr_ldg;
    static int pntr_dsc;

};

// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

#if defined(Q_TEMPLATEDLL)
// MOC_SKIP_BEGIN
Q_TEMPLATE_EXTERN template class TQ_EXPORT TQDict<TQTextFormat>;
// MOC_SKIP_END
#endif

class TQ_EXPORT TQTextFormatCollection
{
    friend class TQTextDocument;
    friend class TQTextFormat;

public:
    TQTextFormatCollection();
    virtual ~TQTextFormatCollection();

    void setDefaultFormat( TQTextFormat *f );
    TQTextFormat *defaultFormat() const;
    virtual TQTextFormat *format( TQTextFormat *f );
    virtual TQTextFormat *format( TQTextFormat *of, TQTextFormat *nf, int flags );
    virtual TQTextFormat *format( const TQFont &f, const TQColor &c );
    virtual void remove( TQTextFormat *f );
    virtual TQTextFormat *createFormat( const TQTextFormat &f ) { return new TQTextFormat( f ); }
    virtual TQTextFormat *createFormat( const TQFont &f, const TQColor &c ) { return new TQTextFormat( f, c, this ); }

    void updateDefaultFormat( const TQFont &font, const TQColor &c, TQStyleSheet *sheet );

    TQPaintDevice *paintDevice() const { return paintdevice; }
    void setPaintDevice( TQPaintDevice * );

private:
    void updateKeys();

private:
    TQTextFormat *defFormat, *lastFormat, *cachedFormat;
    TQDict<TQTextFormat> cKey;
    TQTextFormat *cres;
    TQFont cfont;
    TQColor ccol;
    TQString kof, knf;
    int cflags;

    TQPaintDevice *paintdevice;
};

class TQ_EXPORT TQTextParagraphPseudoDocument
{
public:
    TQTextParagraphPseudoDocument();
    ~TQTextParagraphPseudoDocument();
    TQRect docRect;
    TQTextFormatter *pFormatter;
    TQTextCommandHistory *commandHistory;
    int minw;
    int wused;
    TQTextFormatCollection collection;
};

// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

inline int TQTextParagraph::length() const
{
    return str->length();
}

inline TQRect TQTextParagraph::rect() const
{
    return r;
}

inline int TQTextCursor::index() const
{
    return idx;
}

// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

inline int TQTextDocument::x() const
{
    return cx;
}

inline int TQTextDocument::y() const
{
    return cy;
}

inline int TQTextDocument::width() const
{
    return TQMAX( cw, flow_->width() );
}

inline int TQTextDocument::visibleWidth() const
{
    return vw;
}

inline TQTextParagraph *TQTextDocument::firstParagraph() const
{
    return fParag;
}

inline TQTextParagraph *TQTextDocument::lastParagraph() const
{
    return lParag;
}

inline void TQTextDocument::setFirstParagraph( TQTextParagraph *p )
{
    fParag = p;
}

inline void TQTextDocument::setLastParagraph( TQTextParagraph *p )
{
    lParag = p;
}

inline void TQTextDocument::setWidth( int w )
{
    cw = TQMAX( w, minw );
    flow_->setWidth( cw );
    vw = w;
}

inline int TQTextDocument::minimumWidth() const
{
    return minw;
}

inline void TQTextDocument::setY( int y )
{
    cy = y;
}

inline int TQTextDocument::leftMargin() const
{
    return leftmargin;
}

inline void TQTextDocument::setLeftMargin( int lm )
{
    leftmargin = lm;
}

inline int TQTextDocument::rightMargin() const
{
    return rightmargin;
}

inline void TQTextDocument::setRightMargin( int rm )
{
    rightmargin = rm;
}

inline TQTextPreProcessor *TQTextDocument::preProcessor() const
{
    return pProcessor;
}

inline void TQTextDocument::setPreProcessor( TQTextPreProcessor * sh )
{
    pProcessor = sh;
}

inline void TQTextDocument::setFormatter( TQTextFormatter *f )
{
    delete pFormatter;
    pFormatter = f;
}

inline TQTextFormatter *TQTextDocument::formatter() const
{
    return pFormatter;
}

inline void TQTextDocument::setIndent( TQTextIndent *i )
{
    indenter = i;
}

inline TQTextIndent *TQTextDocument::indent() const
{
    return indenter;
}

inline TQColor TQTextDocument::selectionColor( int id ) const
{
    return selectionColors[ id ];
}

inline bool TQTextDocument::invertSelectionText( int id ) const
{
    return selectionText[ id ];
}

inline void TQTextDocument::setSelectionColor( int id, const TQColor &c )
{
    selectionColors[ id ] = c;
}

inline void TQTextDocument::setInvertSelectionText( int id, bool b )
{
    selectionText[ id ] = b;
}

inline TQTextFormatCollection *TQTextDocument::formatCollection() const
{
    return fCollection;
}

inline int TQTextDocument::alignment() const
{
    return align;
}

inline void TQTextDocument::setAlignment( int a )
{
    align = a;
}

inline int *TQTextDocument::tabArray() const
{
    return tArray;
}

inline int TQTextDocument::tabStopWidth() const
{
    return tStopWidth;
}

inline void TQTextDocument::setTabArray( int *a )
{
    tArray = a;
}

inline void TQTextDocument::setTabStops( int tw )
{
    tStopWidth = tw;
}

inline TQString TQTextDocument::originalText() const
{
    if ( oTextValid )
	return oText;
    return text();
}

inline void TQTextDocument::setFlow( TQTextFlow *f )
{
    if ( flow_ )
	delete flow_;
    flow_ = f;
}

inline void TQTextDocument::takeFlow()
{
    flow_ = 0;
}

inline bool TQTextDocument::useDoubleBuffer( TQTextParagraph *parag, TQPainter *p )
{
    return ( !parag->document()->parent() || parag->document()->nextDoubleBuffered ) &&
	( !p || !p->device() || p->device()->devType() != TQInternal::Printer );
}

// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

inline TQColor TQTextFormat::color() const
{
    return col;
}

inline TQFont TQTextFormat::font() const
{
    return fn;
}

inline bool TQTextFormat::isMisspelled() const
{
    return missp;
}

inline TQTextFormat::VerticalAlignment TQTextFormat::vAlign() const
{
    return ha;
}

inline bool TQTextFormat::operator==( const TQTextFormat &f ) const
{
    return k == f.k;
}

inline TQTextFormatCollection *TQTextFormat::parent() const
{
    return collection;
}

inline void TQTextFormat::addRef()
{
    ref++;
}

inline void TQTextFormat::removeRef()
{
    ref--;
    if ( !collection )
	return;
    if ( this == collection->defFormat )
	return;
    if ( ref == 0 )
	collection->remove( this );
}

inline const TQString &TQTextFormat::key() const
{
    return k;
}

inline bool TQTextFormat::useLinkColor() const
{
    return linkColor;
}

inline TQTextStringChar *TQTextParagraph::at( int i ) const
{
    return &str->at( i );
}

inline bool TQTextParagraph::isValid() const
{
    return invalid == -1;
}

inline bool TQTextParagraph::hasChanged() const
{
    return changed;
}

inline void TQTextParagraph::setBackgroundColor( const TQColor & c )
{
    delete bgcol;
    bgcol = new TQColor( c );
    setChanged( true );
}

inline void TQTextParagraph::clearBackgroundColor()
{
    delete bgcol; bgcol = 0; setChanged( true );
}

inline void TQTextParagraph::append( const TQString &s, bool reallyAtEnd )
{
    if ( reallyAtEnd )
	insert( str->length(), s );
    else
	insert( TQMAX( str->length() - 1, 0 ), s );
}

inline TQTextParagraph *TQTextParagraph::prev() const
{
    return p;
}

inline TQTextParagraph *TQTextParagraph::next() const
{
    return n;
}

inline bool TQTextParagraph::hasAnySelection() const
{
    return mSelections ? !selections().isEmpty() : false;
}

inline void TQTextParagraph::setEndState( int s )
{
    if ( s == state )
	return;
    state = s;
}

inline int TQTextParagraph::endState() const
{
    return state;
}

inline void TQTextParagraph::setParagId( int i )
{
    id = i;
}

inline int TQTextParagraph::paragId() const
{
    if ( id == -1 )
	tqWarning( "invalid parag id!!!!!!!! (%p)", (void*)this );
    return id;
}

inline bool TQTextParagraph::firstPreProcess() const
{
    return firstPProcess;
}

inline void TQTextParagraph::setFirstPreProcess( bool b )
{
    firstPProcess = b;
}

inline TQMap<int, TQTextLineStart*> &TQTextParagraph::lineStartList()
{
    return lineStarts;
}

inline TQTextString *TQTextParagraph::string() const
{
    return str;
}

inline TQTextParagraphPseudoDocument *TQTextParagraph::pseudoDocument() const
{
    if ( hasdoc )
	return 0;
    return (TQTextParagraphPseudoDocument*) docOrPseudo;
}


#ifndef TQT_NO_TEXTCUSTOMITEM
inline TQTextTableCell *TQTextParagraph::tableCell() const
{
    return hasdoc ? document()->tableCell () : 0;
}
#endif

inline TQTextCommandHistory *TQTextParagraph::commands() const
{
    return hasdoc ? document()->commands() : pseudoDocument()->commandHistory;
}


inline int TQTextParagraph::alignment() const
{
    return align;
}

#ifndef TQT_NO_TEXTCUSTOMITEM
inline void TQTextParagraph::registerFloatingItem( TQTextCustomItem *i )
{
    floatingItems().append( i );
}

inline void TQTextParagraph::unregisterFloatingItem( TQTextCustomItem *i )
{
    floatingItems().removeRef( i );
}
#endif

inline TQBrush *TQTextParagraph::background() const
{
#ifndef TQT_NO_TEXTCUSTOMITEM
    return tableCell() ? tableCell()->backGround() : 0;
#else
    return 0;
#endif
}

inline int TQTextParagraph::documentWidth() const
{
    return hasdoc ? document()->width() : pseudoDocument()->docRect.width();
}

inline int TQTextParagraph::documentVisibleWidth() const
{
    return hasdoc ? document()->visibleWidth() : pseudoDocument()->docRect.width();
}

inline int TQTextParagraph::documentX() const
{
    return hasdoc ? document()->x() : pseudoDocument()->docRect.x();
}

inline int TQTextParagraph::documentY() const
{
    return hasdoc ? document()->y() : pseudoDocument()->docRect.y();
}

inline void TQTextParagraph::setExtraData( TQTextParagraphData *data )
{
    eData = data;
}

inline TQTextParagraphData *TQTextParagraph::extraData() const
{
    return eData;
}

// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

inline void TQTextFormatCollection::setDefaultFormat( TQTextFormat *f )
{
    defFormat = f;
}

inline TQTextFormat *TQTextFormatCollection::defaultFormat() const
{
    return defFormat;
}

// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

inline TQTextFormat *TQTextStringChar::format() const
{
    return (type == Regular) ? d.format : d.custom->format;
}


#ifndef TQT_NO_TEXTCUSTOMITEM
inline TQTextCustomItem *TQTextStringChar::customItem() const
{
    return isCustom() ? d.custom->custom : 0;
}
#endif

inline int TQTextStringChar::height() const
{
#ifndef TQT_NO_TEXTCUSTOMITEM
    return !isCustom() ? format()->height() : ( customItem()->placement() == TQTextCustomItem::PlaceInline ? customItem()->height : 0 );
#else
    return format()->height();
#endif
}

inline int TQTextStringChar::ascent() const
{
#ifndef TQT_NO_TEXTCUSTOMITEM
    return !isCustom() ? format()->ascent() : ( customItem()->placement() == TQTextCustomItem::PlaceInline ? customItem()->ascent() : 0 );
#else
    return format()->ascent();
#endif
}

inline int TQTextStringChar::descent() const
{
#ifndef TQT_NO_TEXTCUSTOMITEM
    return !isCustom() ? format()->descent() : 0;
#else
    return format()->descent();
#endif
}

#endif //TQT_NO_RICHTEXT

#endif
