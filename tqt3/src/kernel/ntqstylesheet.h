/****************************************************************************
**
** Definition of the TQStyleSheet class
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

#ifndef TQSTYLESHEET_H
#define TQSTYLESHEET_H

#ifndef QT_H
#include "ntqstring.h"
#include "ntqvaluelist.h"
#include "ntqptrvector.h"
#include "ntqdict.h"
#include "ntqobject.h"
#endif // QT_H

#ifndef TQT_NO_RICHTEXT

class TQStyleSheet;
class TQTextDocument;
template<class Key, class T> class TQMap;
class TQStyleSheetItemData;

class TQ_EXPORT TQStyleSheetItem : public TQt
{
public:
    TQStyleSheetItem( TQStyleSheet* parent, const TQString& name );
    TQStyleSheetItem( const TQStyleSheetItem & );
    ~TQStyleSheetItem();

    TQStyleSheetItem& operator=( const TQStyleSheetItem& other );

    TQString name() const;

    TQStyleSheet* styleSheet();
    const TQStyleSheet* styleSheet() const;

    enum AdditionalStyleValues { Undefined  = - 1};

    enum DisplayMode {
	DisplayBlock,
	DisplayInline,
	DisplayListItem,
	DisplayNone
#ifndef Q_QDOC
	, DisplayModeUndefined = -1
#endif
    };

    DisplayMode displayMode() const;
    void setDisplayMode(DisplayMode m);

    int alignment() const;
    void setAlignment( int f);

    enum VerticalAlignment {
	VAlignBaseline,
	VAlignSub,
	VAlignSuper
    };

    VerticalAlignment verticalAlignment() const;
    void setVerticalAlignment( VerticalAlignment valign );

    int fontWeight() const;
    void setFontWeight(int w);

    int logicalFontSize() const;
    void setLogicalFontSize(int s);

    int logicalFontSizeStep() const;
    void setLogicalFontSizeStep( int s );

    int fontSize() const;
    void setFontSize(int s);

    TQString fontFamily() const;
    void setFontFamily( const TQString& );

    int numberOfColumns() const;
    void setNumberOfColumns(int ncols);

    TQColor color() const;
    void setColor( const TQColor &);

    bool fontItalic() const;
    void setFontItalic( bool );
    bool definesFontItalic() const;

    bool fontUnderline() const;
    void setFontUnderline( bool );
    bool definesFontUnderline() const;

    bool fontStrikeOut() const;
    void setFontStrikeOut( bool );
    bool definesFontStrikeOut() const;

    bool isAnchor() const;
    void setAnchor(bool anc);

    enum WhiteSpaceMode {
	WhiteSpaceNormal,
	WhiteSpacePre,
	WhiteSpaceNoWrap
#ifndef Q_QDOC
	, WhiteSpaceModeUndefined = -1
#endif
    };
    WhiteSpaceMode whiteSpaceMode() const;
    void setWhiteSpaceMode(WhiteSpaceMode m);

    enum Margin {
	MarginLeft,
	MarginRight,
	MarginTop,
	MarginBottom,
	MarginFirstLine,
	MarginAll,
	MarginVertical,
	MarginHorizontal
#ifndef Q_QDOC
	, MarginUndefined = -1
#endif
    };

    int margin( Margin m) const;
    void setMargin( Margin, int);

    enum ListStyle {
	ListDisc,
	ListCircle,
	ListSquare,
	ListDecimal,
	ListLowerAlpha,
	ListUpperAlpha
#ifndef Q_QDOC
	, ListStyleUndefined = -1
#endif
    };

    ListStyle listStyle() const;
    void setListStyle( ListStyle );

    TQString contexts() const;
    void setContexts( const TQString& );
    bool allowedInContext( const TQStyleSheetItem* ) const;

    bool selfNesting() const;
    void setSelfNesting( bool );

    void setLineSpacing( int ls );
    int lineSpacing() const;

private:
    void init();
    TQStyleSheetItemData* d;
};


#if defined(Q_TEMPLATEDLL)
// MOC_SKIP_BEGIN
Q_TEMPLATE_EXTERN template class TQ_EXPORT TQDict<TQStyleSheetItem>;
Q_TEMPLATE_EXTERN template class TQ_EXPORT TQValueList< TQPtrVector<TQStyleSheetItem> >;
Q_TEMPLATE_EXTERN template class TQ_EXPORT TQPtrVector<TQStyleSheetItem>;
Q_TEMPLATE_EXTERN template class TQ_EXPORT TQValueList<TQStyleSheetItem::ListStyle>;
// MOC_SKIP_END
#endif

#ifndef TQT_NO_TEXTCUSTOMITEM
class TQTextCustomItem;
#endif

class TQ_EXPORT TQStyleSheet : public TQObject
{
    TQ_OBJECT
public:
    TQStyleSheet( TQObject *parent=0, const char *name=0 );
    virtual ~TQStyleSheet();

    static TQStyleSheet* defaultSheet();
    static void setDefaultSheet( TQStyleSheet* );


    TQStyleSheetItem* item( const TQString& name);
    const TQStyleSheetItem* item( const TQString& name) const;

    void insert( TQStyleSheetItem* item);

#ifndef TQT_NO_TEXTCUSTOMITEM
    virtual TQTextCustomItem* tag( const TQString& name,
				  const TQMap<TQString, TQString> &attr,
				  const TQString& context,
				  const TQMimeSourceFactory& factory,
				  bool emptyTag, TQTextDocument *doc ) const;
#endif
    static TQString escape( const TQString& );
    static TQString convertFromPlainText( const TQString&,
					 TQStyleSheetItem::WhiteSpaceMode mode = TQStyleSheetItem::WhiteSpacePre );
    static bool mightBeRichText( const TQString& );

    virtual void scaleFont( TQFont& font, int logicalSize ) const;

    virtual void error( const TQString& ) const;

private:
    void init();
    TQDict<TQStyleSheetItem> styles;
    TQStyleSheetItem* nullstyle;
private:	// Disabled copy constructor and operator=
#if defined(TQ_DISABLE_COPY)
    TQStyleSheet( const TQStyleSheet & );
    TQStyleSheet &operator=( const TQStyleSheet & );
#endif
};

#endif // TQT_NO_RICHTEXT

#endif // TQSTYLESHEET_H
