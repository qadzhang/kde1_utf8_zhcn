/****************************************************************************
**
** ???
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
** Licensees holding valid TQt Commercial licenses may use this file in
** accordance with the TQt Commercial License Agreement provided with
** the Software.
**
** This file is provided "AS IS" with NO WARRANTY OF ANY KIND,
** INCLUDING THE WARRANTIES OF DESIGN, MERCHANTABILITY AND FITNESS FOR
** A PARTICULAR PURPOSE. Trolltech reserves all rights not granted
** herein.
**
**********************************************************************/

#ifndef TQTEXTLAYOUT_P_H
#define TQTEXTLAYOUT_P_H

#ifndef QT_H
#include "ntqstring.h"
#include "ntqnamespace.h"
#include "ntqrect.h"
#endif // QT_H

class TQTextEngine;
class TQFont;

class TQ_EXPORT TQTextItem
{
public:
    inline TQTextItem() : item(0), engine(0) {}
    inline bool isValid() const { return (bool)engine; }

    TQRect rect() const;
    int x() const;
    int y() const;
    int width() const;
    int ascent() const;
    int descent() const;

    enum Edge {
	Leading,
	Trailing
    };
    enum CursorPosition {
	BetweenCharacters,
	OnCharacters
    };

    /* cPos gets set to the valid position */
    int cursorToX( int *cPos, Edge edge = Leading ) const;
    inline int cursorToX( int cPos, Edge edge = Leading ) const { return cursorToX( &cPos, edge ); }
    int xToCursor( int x, CursorPosition = BetweenCharacters ) const;

    bool isRightToLeft() const;
    bool isObject() const;
    bool isSpace() const;
    bool isTab() const;

    void setWidth( int w );
    void setAscent( int a );
    void setDescent( int d );

    int from() const;
    int length() const;

private:
    friend class TQTextLayout;
    friend class TQPainter;
    friend class TQPSPrinter;
    TQTextItem( int i, TQTextEngine *e ) : item( i ), engine( e ) {}
    int item;
    TQTextEngine *engine;
};


class TQPainter;

class TQ_EXPORT TQTextLayout
{
public:
    // does itemization
    TQTextLayout();
    TQTextLayout( const TQString& string, TQPainter * = 0 );
    TQTextLayout( const TQString& string, const TQFont& fnt );
    ~TQTextLayout();

    void setText( const TQString& string, const TQFont& fnt );

    enum LineBreakStrategy {
	AtWordBoundaries,
	AtCharBoundaries
    };

    /* add an additional item boundary eg. for style change */
    void setBoundary( int strPos );

    int numItems() const;
    TQTextItem itemAt( int i ) const;
    TQTextItem findItem( int strPos ) const;

    enum LayoutMode {
	NoBidi,
	SingleLine,
	MultiLine
    };
    void beginLayout( LayoutMode m = MultiLine );
    void beginLine( int width );

    bool atEnd() const;
    TQTextItem nextItem();
    TQTextItem currentItem();
    /* ## maybe also currentItem() */
    void setLineWidth( int newWidth );
    int lineWidth() const;
    int widthUsed() const;
    int availableWidth() const;

    enum Result {
	Ok,
	LineFull,
	LineEmpty,
	Error
    };
    /* returns true if completely added */
    Result addCurrentItem();

    /* Note: if ascent and descent are used they must be initialized to the minimum ascent/descent
       acceptable for the line. TQFontMetrics::ascent/descent() is usually the right choice */
    Result endLine( int x = 0, int y = 0, int alignment = TQt::AlignLeft,
		    int *ascent = 0, int *descent = 0, int *left = 0, int *right = 0 );
    void endLayout();

    enum CursorMode {
	SkipCharacters,
	SkipWords
    };
    bool validCursorPosition( int pos ) const;
    int nextCursorPosition( int oldPos, CursorMode mode = SkipCharacters ) const;
    int previousCursorPosition( int oldPos, CursorMode mode = SkipCharacters ) const;

    void setDirection(TQChar::Direction);
private:
    TQTextLayout( TQTextEngine *e ) : d( e ) {}
    /* disable copy and assignment */
    TQTextLayout( const TQTextLayout & ) {}
    void operator = ( const TQTextLayout & ) {}

    friend class TQTextItem;
    friend class TQPainter;
    friend class TQPSPrinter;
    TQTextEngine *d;
};


/*
  class TQPainter {
      .....
      void drawTextItem( int x, int y, TQTextItem *item );
  };
*/

#endif
