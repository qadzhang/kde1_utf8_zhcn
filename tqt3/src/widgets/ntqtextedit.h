/****************************************************************************
**
** Definition of the TQTextEdit class
**
** Created : 990101
**
** Copyright (C) 1992-2008 Trolltech ASA.  All rights reserved.
**
** This file is part of the widgets module of the TQt GUI Toolkit.
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

#ifndef TQTEXTEDIT_H
#define TQTEXTEDIT_H

#ifndef QT_H
#include "ntqscrollview.h"
#include "ntqstylesheet.h"
#include "ntqptrvector.h"
#include "ntqvaluelist.h"
#include "ntqptrlist.h"
#endif // QT_H

#ifndef TQT_NO_TEXTEDIT
// uncomment below to enable optimization mode - also uncomment the
// optimDoAutoScroll() private slot since moc ignores #ifdefs..
#define QT_TEXTEDIT_OPTIMIZATION

class TQPainter;
class TQTextDocument;
class TQTextCursor;
class TQKeyEvent;
class TQResizeEvent;
class TQMouseEvent;
class TQTimer;
class TQTextString;
class TQTextCommand;
class TQTextParagraph;
class TQTextFormat;
class TQFont;
class TQColor;
class TQTextEdit;
class TQTextBrowser;
class TQTextString;
struct TQUndoRedoInfoPrivate;
class TQPopupMenu;
class TQTextEditPrivate;
class TQSyntaxHighlighter;

#ifdef QT_TEXTEDIT_OPTIMIZATION
class TQTextEditOptimPrivate
{
public:
    // Note: no left-tag has any value for leftTag or parent, and
    // no right-tag has any formatting flags set.
    enum TagType { Color = 0, Format = 1 };
    struct Tag {
	TagType type:2;
	bool bold:1;
	bool italic:1;
	bool underline:1;
	int line;
	int index;
	Tag * leftTag; // ptr to left-tag in a left-right tag pair
	Tag * parent;  // ptr to parent left-tag in a nested tag
	Tag * prev;
	Tag * next;
	TQString tag;
    };
    TQTextEditOptimPrivate()
    {
	len = numLines = maxLineWidth = 0;
	selStart.line = selStart.index = -1;
	selEnd.line = selEnd.index = -1;
	search.line = search.index = 0;
	tags = lastTag = 0;
    }
    void clearTags()
    {
	Tag * itr = tags;
	while ( tags ) {
	    itr  = tags;
	    tags = tags->next;
	    delete itr;
	}
	tags = lastTag = 0;
	tagIndex.clear();
    }
    ~TQTextEditOptimPrivate()
    {
	clearTags();
    }
    int len;
    int numLines;
    int maxLineWidth;
    struct Selection {
	int line;
	int index;
    };
    Selection selStart, selEnd, search;
    Tag * tags, * lastTag;
    TQMap<int, TQString> lines;
    TQMap<int, Tag *> tagIndex;
};
#endif

class TQ_EXPORT TQTextEdit : public TQScrollView
{
    friend class TQTextBrowser;
    friend class TQSyntaxHighlighter;

    TQ_OBJECT
    TQ_ENUMS( WordWrap WrapPolicy )
    TQ_SETS( AutoFormatting )
    TQ_PROPERTY( TextFormat textFormat READ textFormat WRITE setTextFormat )
    TQ_PROPERTY( TQString text READ text WRITE setText )
    TQ_PROPERTY( TQBrush paper READ paper WRITE setPaper )
    TQ_PROPERTY( bool linkUnderline READ linkUnderline WRITE setLinkUnderline )
    TQ_PROPERTY( TQString documentTitle READ documentTitle )
    TQ_PROPERTY( int length READ length )
    TQ_PROPERTY( WordWrap wordWrap READ wordWrap WRITE setWordWrap )
    TQ_PROPERTY( int wrapColumnOrWidth READ wrapColumnOrWidth WRITE setWrapColumnOrWidth )
    TQ_PROPERTY( WrapPolicy wrapPolicy READ wrapPolicy WRITE setWrapPolicy )
    TQ_PROPERTY( bool hasSelectedText READ hasSelectedText )
    TQ_PROPERTY( TQString selectedText READ selectedText )
    TQ_PROPERTY( int undoDepth READ undoDepth WRITE setUndoDepth )
    TQ_PROPERTY( bool overwriteMode READ isOverwriteMode WRITE setOverwriteMode )
    TQ_PROPERTY( bool modified READ isModified WRITE setModified DESIGNABLE false )
    TQ_PROPERTY( bool readOnly READ isReadOnly WRITE setReadOnly )
    TQ_PROPERTY( bool undoRedoEnabled READ isUndoRedoEnabled WRITE setUndoRedoEnabled )
    TQ_PROPERTY( int tabStopWidth READ tabStopWidth WRITE setTabStopWidth )
    TQ_PROPERTY( bool tabChangesFocus READ tabChangesFocus WRITE setTabChangesFocus )
    TQ_PROPERTY( AutoFormatting autoFormatting READ autoFormatting WRITE setAutoFormatting )

public:
    enum WordWrap {
	NoWrap,
	WidgetWidth,
	FixedPixelWidth,
	FixedColumnWidth
    };

    enum WrapPolicy {
	AtWordBoundary,
	AtWhiteSpace = AtWordBoundary, // AtWhiteSpace is deprecated
	Anywhere,
	AtWordOrDocumentBoundary
    };

    enum AutoFormatting {
	AutoNone = 0,
	AutoBulletList = 0x00000001,
	AutoAll = 0xffffffff
    };

    enum KeyboardAction {
	ActionBackspace,
	ActionDelete,
	ActionReturn,
	ActionKill,
	ActionWordBackspace,
	ActionWordDelete
    };

    enum CursorAction {
	MoveBackward,
	MoveForward,
	MoveWordBackward,
	MoveWordForward,
	MoveUp,
	MoveDown,
	MoveLineStart,
	MoveLineEnd,
	MoveHome,
	MoveEnd,
	MovePgUp,
	MovePgDown
    };

    enum VerticalAlignment {
	AlignNormal,
	AlignSuperScript,
	AlignSubScript
    };

    enum TextInsertionFlags {
	RedoIndentation = 0x0001,
	CheckNewLines = 0x0002,
	RemoveSelected = 0x0004,
	AsIMCompositionText = 0x0008,  // internal use
	WithIMSelection = 0x0010       // internal use
    };

    TQTextEdit( const TQString& text, const TQString& context = TQString::null,
	       TQWidget* parent=0, const char* name=0);
    TQTextEdit( TQWidget* parent=0, const char* name=0 );
    virtual ~TQTextEdit();
    void setPalette( const TQPalette & );

    TQString text() const;
    TQString text( int para ) const;
    TextFormat textFormat() const;
    TQString context() const;
    TQString documentTitle() const;

    void getSelection( int *paraFrom, int *indexFrom,
		    int *paraTo, int *indexTo, int selNum = 0 ) const;
    virtual bool find( const TQString &expr, bool cs, bool wo, bool forward = true,
		       int *para = 0, int *index = 0 );

    int paragraphs() const;
    int lines() const;
    int linesOfParagraph( int para ) const;
    int lineOfChar( int para, int chr );
    int length() const;
    TQRect paragraphRect( int para ) const;
    int paragraphAt( const TQPoint &pos ) const;
    int charAt( const TQPoint &pos, int *para ) const;
    int paragraphLength( int para ) const;

    TQStyleSheet* styleSheet() const;
#ifndef TQT_NO_MIME
    TQMimeSourceFactory* mimeSourceFactory() const;
#endif
    TQBrush paper() const;
    bool linkUnderline() const;

    int heightForWidth( int w ) const;

    bool hasSelectedText() const;
    TQString selectedText() const;
    bool isUndoAvailable() const;
    bool isRedoAvailable() const;

    WordWrap wordWrap() const;
    int wrapColumnOrWidth() const;
    WrapPolicy wrapPolicy() const;

    int tabStopWidth() const;

    TQString anchorAt( const TQPoint& pos );
    TQString anchorAt( const TQPoint& pos, AnchorAttribute a );

    TQSize sizeHint() const;

    bool isReadOnly() const { return readOnly; }

    void getCursorPosition( int *parag, int *index ) const;

    bool isModified() const;
    bool italic() const;
    bool bold() const;
    bool underline() const;
    TQString family() const;
    int pointSize() const;
    TQColor color() const;
    TQFont font() const;
    TQFont currentFont() const;
    int alignment() const;
    int undoDepth() const;

    // do not use, will go away
    virtual bool getFormat( int para, int index, TQFont *font, TQColor *color, VerticalAlignment *verticalAlignment );
    // do not use, will go away
    virtual bool getParagraphFormat( int para, TQFont *font, TQColor *color,
				     VerticalAlignment *verticalAlignment, int *alignment,
				     TQStyleSheetItem::DisplayMode *displayMode,
				     TQStyleSheetItem::ListStyle *listStyle,
				     int *listDepth );


    bool isOverwriteMode() const { return overWrite; }
    TQColor paragraphBackgroundColor( int para ) const;

    bool isUndoRedoEnabled() const;
    bool eventFilter( TQObject *o, TQEvent *e );
    bool tabChangesFocus() const;

    void setAutoFormatting( uint features );
    uint autoFormatting() const;
    TQSyntaxHighlighter *syntaxHighlighter() const;

public slots:
    void setEnabled( bool );
#ifndef TQT_NO_MIME
    virtual void setMimeSourceFactory( TQMimeSourceFactory* factory );
#endif
    virtual void setStyleSheet( TQStyleSheet* styleSheet );
    virtual void scrollToAnchor( const TQString& name );
    virtual void setPaper( const TQBrush& pap );
    virtual void setLinkUnderline( bool );

    virtual void setWordWrap( WordWrap mode );
    virtual void setWrapColumnOrWidth( int );
    virtual void setWrapPolicy( WrapPolicy policy );

    virtual void copy();
    virtual void append( const TQString& text );

    void setText( const TQString &txt ) { setText( txt, TQString::null ); }
    virtual void setText( const TQString &txt, const TQString &context );
    virtual void setTextFormat( TextFormat f );

    virtual void selectAll( bool select = true );
    virtual void setTabStopWidth( int ts );
    virtual void zoomIn( int range );
    virtual void zoomIn() { zoomIn( 1 ); }
    virtual void zoomOut( int range );
    virtual void zoomOut() { zoomOut( 1 ); }
    virtual void zoomTo( int size );

    virtual void sync();
    virtual void setReadOnly( bool b );

    virtual void undo();
    virtual void redo();
    virtual void cut();
    virtual void paste();
#ifndef TQT_NO_CLIPBOARD
    virtual void pasteSubType( const TQCString &subtype );
#endif
    virtual void clear();
    virtual void del();
    virtual void indent();
    virtual void setItalic( bool b );
    virtual void setBold( bool b );
    virtual void setUnderline( bool b );
    virtual void setFamily( const TQString &f );
    virtual void setPointSize( int s );
    virtual void setColor( const TQColor &c );
    virtual void setFont( const TQFont &f );
    virtual void setVerticalAlignment( VerticalAlignment a );
    virtual void setAlignment( int a );

    // do not use, will go away
    virtual void setParagType( TQStyleSheetItem::DisplayMode dm, TQStyleSheetItem::ListStyle listStyle );

    virtual void setCursorPosition( int parag, int index );
    virtual void setSelection( int parag_from, int index_from, int parag_to, int index_to, int selNum = 0 );
    virtual void setSelectionAttributes( int selNum, const TQColor &back, bool invertText );
    virtual void setModified( bool m );
    virtual void resetFormat();
    virtual void setUndoDepth( int d );
    virtual void setFormat( TQTextFormat *f, int flags );
    virtual void ensureCursorVisible();
    virtual void placeCursor( const TQPoint &pos, TQTextCursor *c = 0 );
    virtual void moveCursor( CursorAction action, bool select );
    virtual void doKeyboardAction( KeyboardAction action );
    virtual void removeSelectedText( int selNum = 0 );
    virtual void removeSelection( int selNum = 0 );
    virtual void setCurrentFont( const TQFont &f );
    virtual void setOverwriteMode( bool b ) { overWrite = b; }

    virtual void scrollToBottom();

    void insert( const TQString &text, uint insertionFlags = CheckNewLines | RemoveSelected ); // ## virtual in 4.0

    // obsolete
    virtual void insert( const TQString &text, bool, bool = true, bool = true );

    virtual void insertAt( const TQString &text, int para, int index );
    virtual void removeParagraph( int para );
    virtual void insertParagraph( const TQString &text, int para );

    virtual void setParagraphBackgroundColor( int para, const TQColor &bg );
    virtual void clearParagraphBackground( int para );

    virtual void setUndoRedoEnabled( bool b );
    void setTabChangesFocus( bool b ); // ### make virtual in 4.0

#ifdef QT_TEXTEDIT_OPTIMIZATION
    void polish();
    void setMaxLogLines( int numLines );
    int maxLogLines();
#endif

signals:
    void textChanged();
    void selectionChanged();
    void copyAvailable( bool );
    void undoAvailable( bool yes );
    void redoAvailable( bool yes );
    void currentFontChanged( const TQFont &f );
    void currentColorChanged( const TQColor &c );
    void currentAlignmentChanged( int a );
    void currentVerticalAlignmentChanged( VerticalAlignment a );
    void cursorPositionChanged( TQTextCursor *c );
    void cursorPositionChanged( int para, int pos );
    void returnPressed();
    void modificationChanged( bool m );
    void clicked( int parag, int index );
    void doubleClicked( int parag, int index );

protected:
    void repaintChanged();
    void updateStyles();
    void drawContents( TQPainter *p, int cx, int cy, int cw, int ch );
    bool event( TQEvent *e );
    void keyPressEvent( TQKeyEvent *e );
    void resizeEvent( TQResizeEvent *e );
    void viewportResizeEvent( TQResizeEvent* );
    void contentsMousePressEvent( TQMouseEvent *e );
    void contentsMouseMoveEvent( TQMouseEvent *e );
    void contentsMouseReleaseEvent( TQMouseEvent *e );
    void contentsMouseDoubleClickEvent( TQMouseEvent *e );
#ifndef TQT_NO_WHEELEVENT
    void contentsWheelEvent( TQWheelEvent *e );
#endif
    void imStartEvent( TQIMEvent * );
    void imComposeEvent( TQIMEvent * );
    void imEndEvent( TQIMEvent * );
#ifndef TQT_NO_DRAGANDDROP
    void contentsDragEnterEvent( TQDragEnterEvent *e );
    void contentsDragMoveEvent( TQDragMoveEvent *e );
    void contentsDragLeaveEvent( TQDragLeaveEvent *e );
    void contentsDropEvent( TQDropEvent *e );
#endif
    void contentsContextMenuEvent( TQContextMenuEvent *e );
    bool sendMouseEventToInputContext( TQMouseEvent *e );
    bool focusNextPrevChild( bool next );
    TQTextDocument *document() const;
    TQTextCursor *textCursor() const;
    void setDocument( TQTextDocument *doc );
    virtual TQPopupMenu *createPopupMenu( const TQPoint& pos );
    virtual TQPopupMenu *createPopupMenu();
    void drawCursor( bool visible );

    void windowActivationChange( bool );

protected slots:
    virtual void doChangeInterval();
    void sliderReleased(); // ### make virtual in 4.0
#if (TQT_VERSION >= 0x040000)
#error "Some functions need to be changed to virtual for TQt 4.0"
#endif

private slots:
    void formatMore();
    void doResize();
    void autoScrollTimerDone();
    void blinkCursor();
    void setModified();
    void startDrag();
    void documentWidthChanged( int w );
    void clipboardChanged();

private:
    struct TQ_EXPORT UndoRedoInfo {
	enum Type { Invalid, Insert, Delete, Backspace, Return, RemoveSelected, Format, Style, IME };

	UndoRedoInfo( TQTextDocument *dc );
	~UndoRedoInfo();
	void clear();
	bool valid() const;

	TQUndoRedoInfoPrivate *d;
	int id;
	int index;
	int eid;
	int eindex;
	TQTextFormat *format;
	int flags;
	Type type;
	TQTextDocument *doc;
	TQByteArray styleInformation;
    };

private:
    void updateCursor( const TQPoint & pos );
    void handleMouseMove( const TQPoint& pos );
    void drawContents( TQPainter * );
    virtual bool linksEnabled() const { return false; }
    void init();
    void checkUndoRedoInfo( UndoRedoInfo::Type t );
    void updateCurrentFormat();
    bool handleReadOnlyKeyEvent( TQKeyEvent *e );
    void makeParagVisible( TQTextParagraph *p );
    void normalCopy();
    void copyToClipboard();
#ifndef TQT_NO_MIME
    TQCString pickSpecial(TQMimeSource* ms, bool always_ask, const TQPoint&);
    TQTextDrag *dragObject( TQWidget *parent = 0 ) const;
#endif
#ifndef TQT_NO_MIMECLIPBOARD
    void pasteSpecial(const TQPoint&);
#endif
    void setFontInternal( const TQFont &f );

    virtual void emitHighlighted( const TQString & ) {}
    virtual void emitLinkClicked( const TQString & ) {}

    void readFormats( TQTextCursor &c1, TQTextCursor &c2, TQTextString &text, bool fillStyles = false );
    void clearUndoRedo();
    void paintDocument( bool drawAll, TQPainter *p, int cx = -1, int cy = -1, int cw = -1, int ch = -1 );
    void moveCursor( CursorAction action );
    void ensureFormatted( TQTextParagraph *p );
    void placeCursor( const TQPoint &pos, TQTextCursor *c, bool link );
    void updateMicroFocusHint();

#ifdef QT_TEXTEDIT_OPTIMIZATION
    bool checkOptimMode();
    TQString optimText() const;
    void optimSetText( const TQString &str );
    void optimAppend( const TQString &str );
    void optimInsert( const TQString &str, int line, int index );
    void optimDrawContents( TQPainter * p, int cx, int cy, int cw, int ch );
    void optimMousePressEvent( TQMouseEvent * e );
    void optimMouseReleaseEvent( TQMouseEvent * e );
    void optimMouseMoveEvent( TQMouseEvent * e );
    int  optimCharIndex( const TQString &str, int mx ) const;
    void optimSelectAll();
    void optimRemoveSelection();
    void optimSetSelection( int startLine, int startIdx, int endLine,
			    int endIdx );
    bool optimHasSelection() const;
    TQString optimSelectedText() const;
    bool optimFind( const TQString & str, bool, bool, bool, int *, int * );
    void optimParseTags( TQString * str, int lineNo = -1, int indexOffset = 0 );
    TQTextEditOptimPrivate::Tag * optimPreviousLeftTag( int line );
    void optimSetTextFormat( TQTextDocument *, TQTextCursor *, TQTextFormat * f,
			     int, int, TQTextEditOptimPrivate::Tag * t );
    TQTextEditOptimPrivate::Tag * optimAppendTag( int index, const TQString & tag );
    TQTextEditOptimPrivate::Tag * optimInsertTag( int line, int index, const TQString & tag );
    void optimCheckLimit( const TQString& str );
    bool optimHasBoldMetrics( int line );

private slots:
    void optimDoAutoScroll();
#endif // QT_TEXTEDIT_OPTIMIZATION

private:
#ifndef TQT_NO_CLIPBOARD
    void pasteSubType( const TQCString &subtype, TQMimeSource *m );
#endif

private:
    TQTextDocument *doc;
    TQTextCursor *cursor;
    TQTimer *formatTimer, *scrollTimer, *changeIntervalTimer, *blinkTimer, *dragStartTimer;
    TQTextParagraph *lastFormatted;
    int interval;
    UndoRedoInfo undoRedoInfo;
    TQTextFormat *currentFormat;
    int currentAlignment;
    TQPoint oldMousePos, mousePos;
    TQPoint dragStartPos;
    TQString onLink;
    WordWrap wrapMode;
    WrapPolicy wPolicy;
    int wrapWidth;
    TQString pressedLink;
    TQTextEditPrivate *d;
    bool inDoubleClick : 1;
    bool mousePressed : 1;
    bool cursorVisible : 1;
    bool blinkCursorVisible : 1;
    bool noop: 1; //< Removed; keep as padding for binary compat or reuse if needed
    bool modified : 1;
    bool mightStartDrag : 1;
    bool inDnD : 1;
    bool readOnly : 1;
    bool undoEnabled : 1;
    bool overWrite : 1;

private:	// Disabled copy constructor and operator=
#if defined(TQ_DISABLE_COPY)
    TQTextEdit( const TQTextEdit & );
    TQTextEdit &operator=( const TQTextEdit & );
#endif
};

inline TQTextDocument *TQTextEdit::document() const
{
    return doc;
}

inline TQTextCursor *TQTextEdit::textCursor() const
{
    return cursor;
}

inline void TQTextEdit::setCurrentFont( const TQFont &f )
{
    TQTextEdit::setFontInternal( f );
}

#endif //TQT_NO_TEXTEDIT
#endif //TQTEXTVIEW_H
