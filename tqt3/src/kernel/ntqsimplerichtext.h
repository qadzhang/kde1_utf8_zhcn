/****************************************************************************
**
** Definition of the TQSimpleRichText class
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

#ifndef TQSIMPLERICHTEXT_H
#define TQSIMPLERICHTEXT_H

#ifndef QT_H
#include "ntqnamespace.h"
#include "ntqstring.h"
#include "ntqregion.h"
#endif // QT_H

#ifndef TQT_NO_RICHTEXT

class TQPainter;
class TQWidget;
class TQStyleSheet;
class TQBrush;
class TQMimeSourceFactory;
class TQSimpleRichTextData;

class TQ_EXPORT TQSimpleRichText
{
public:
    TQSimpleRichText( const TQString& text, const TQFont& fnt,
		     const TQString& context = TQString::null, const TQStyleSheet* sheet = 0);
    TQSimpleRichText( const TQString& text, const TQFont& fnt,
		     const TQString& context,  const TQStyleSheet* sheet,
		     const TQMimeSourceFactory* factory, int pageBreak = -1,
		     const TQColor& linkColor = TQt::blue, bool linkUnderline = true );
    ~TQSimpleRichText();

    void setWidth( int );
    void setWidth( TQPainter*, int );
    void setDefaultFont( const TQFont &f );
    int width() const;
    int widthUsed() const;
    int height() const;
    void adjustSize();

    void draw( TQPainter* p,  int x, int y, const TQRect& clipRect,
	       const TQColorGroup& cg, const TQBrush* paper = 0) const;

    // obsolete
    void draw( TQPainter* p,  int x, int y, const TQRegion& clipRegion,
	       const TQColorGroup& cg, const TQBrush* paper = 0) const {
	draw( p, x, y, clipRegion.boundingRect(), cg, paper );
    }

    TQString context() const;
    TQString anchorAt( const TQPoint& pos ) const;

    bool inText( const TQPoint& pos ) const;

private:
    TQSimpleRichTextData* d;

private:	// Disabled copy constructor and operator=
#if defined(TQ_DISABLE_COPY)
    TQSimpleRichText( const TQSimpleRichText & );
    TQSimpleRichText &operator=( const TQSimpleRichText & );
#endif
};

#endif // TQT_NO_RICHTEXT

#endif // TQSIMPLERICHTEXT_H
