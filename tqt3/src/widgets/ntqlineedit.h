/**********************************************************************
**
** Definition of TQLineEdit widget class
**
** Created : 941011
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

#ifndef TQLINEEDIT_H
#define TQLINEEDIT_H

struct TQLineEditPrivate;

class TQValidator;
class TQPopupMenu;

#ifndef QT_H
#include "ntqframe.h"
#include "ntqstring.h"
#endif // QT_H

#ifndef TQT_NO_LINEEDIT

class TQTextParagraph;
class TQTextCursor;

class TQ_EXPORT TQLineEdit : public TQFrame
{
    TQ_OBJECT
    TQ_ENUMS( EchoMode )
    TQ_PROPERTY( TQString text READ text WRITE setText )
    TQ_PROPERTY( int maxLength READ maxLength WRITE setMaxLength )
    TQ_PROPERTY( bool frame READ frame WRITE setFrame )
    TQ_PROPERTY( EchoMode echoMode READ echoMode WRITE setEchoMode )
    TQ_PROPERTY( TQString displayText READ displayText )
    TQ_PROPERTY( int cursorPosition READ cursorPosition WRITE setCursorPosition )
    TQ_PROPERTY( Alignment alignment READ alignment WRITE setAlignment )
    TQ_PROPERTY( bool edited READ edited WRITE setEdited DESIGNABLE false )
    TQ_PROPERTY( bool modified READ isModified )
    TQ_PROPERTY( bool hasMarkedText READ hasMarkedText DESIGNABLE false )
    TQ_PROPERTY( bool hasSelectedText READ hasSelectedText )
    TQ_PROPERTY( TQString markedText READ markedText DESIGNABLE false )
    TQ_PROPERTY( TQString selectedText READ selectedText )
    TQ_PROPERTY( bool dragEnabled READ dragEnabled WRITE setDragEnabled )
    TQ_PROPERTY( bool readOnly READ isReadOnly WRITE setReadOnly )
    TQ_PROPERTY( bool undoAvailable READ isUndoAvailable )
    TQ_PROPERTY( bool redoAvailable READ isRedoAvailable )
    TQ_PROPERTY( TQString inputMask READ inputMask WRITE setInputMask )
    TQ_PROPERTY( bool acceptableInput READ hasAcceptableInput )

public:
    TQLineEdit( TQWidget* parent, const char* name=0 );
    TQLineEdit( const TQString &, TQWidget* parent, const char* name=0 );
    TQLineEdit( const TQString &, const TQString &, TQWidget* parent, const char* name=0 );
    ~TQLineEdit();

    TQString text() const;

    TQString displayText() const;

    int maxLength() const;

    bool frame() const;

    enum EchoMode { Normal, NoEcho, Password, PasswordThreeStars };
    EchoMode echoMode() const;

    bool isReadOnly() const;

    const TQValidator * validator() const;

    TQSize sizeHint() const;
    TQSize minimumSizeHint() const;

    int cursorPosition() const;
    bool validateAndSet( const TQString &, int, int, int ); // obsolete

    int alignment() const;

#ifndef TQT_NO_COMPAT
    void cursorLeft( bool mark, int steps = 1 ) { cursorForward( mark, -steps ); }
    void cursorRight( bool mark, int steps = 1 ) { cursorForward( mark, steps ); }
#endif
    void cursorForward( bool mark, int steps = 1 );
    void cursorBackward( bool mark, int steps = 1 );
    void cursorWordForward( bool mark );
    void cursorWordBackward( bool mark );
    void backspace();
    void del();
    void home( bool mark );
    void end( bool mark );

    bool isModified() const;
    void clearModified();

    bool edited() const; // obsolete, use isModified()
    void setEdited( bool ); // obsolete, use clearModified()

    bool hasSelectedText() const;
    TQString selectedText() const;
    int selectionStart() const;

    bool isUndoAvailable() const;
    bool isRedoAvailable() const;

#ifndef TQT_NO_COMPAT
    bool hasMarkedText() const { return hasSelectedText(); }
    TQString markedText() const { return selectedText(); }
#endif

    bool dragEnabled() const;

    TQString inputMask() const;
    void setInputMask( const TQString &inputMask );
    bool hasAcceptableInput() const;

public slots:
    virtual void setText( const TQString &);
    virtual void selectAll();
    virtual void deselect();
    virtual void clearValidator();
    virtual void insert( const TQString &);
    virtual void clear();
    virtual void undo();
    virtual void redo();
    virtual void setMaxLength( int );
    virtual void setFrame( bool );
    virtual void setEchoMode( EchoMode );
    virtual void setReadOnly( bool );
    virtual void setValidator( const TQValidator * );
    virtual void setFont( const TQFont & );
    virtual void setPalette( const TQPalette & );
    virtual void setSelection( int, int );
    virtual void setCursorPosition( int );
    virtual void setAlignment( int flag );
#ifndef TQT_NO_CLIPBOARD
    virtual void cut();
    virtual void copy() const;
    virtual void paste();
#endif
    virtual void setDragEnabled( bool b );

signals:
    void textChanged( const TQString &);
    void returnPressed();
    void lostFocus();
    void selectionChanged();

protected:
    bool event( TQEvent * );
    void mousePressEvent( TQMouseEvent * );
    void mouseMoveEvent( TQMouseEvent * );
    void mouseReleaseEvent( TQMouseEvent * );
    void mouseDoubleClickEvent( TQMouseEvent * );
    void keyPressEvent( TQKeyEvent * );
    void imStartEvent( TQIMEvent * );
    void imComposeEvent( TQIMEvent * );
    void imEndEvent( TQIMEvent * );
    void focusInEvent( TQFocusEvent * );
    void focusOutEvent( TQFocusEvent * );
    void resizeEvent( TQResizeEvent * );
    void drawContents( TQPainter * );
#ifndef TQT_NO_DRAGANDDROP
    void dragEnterEvent( TQDragEnterEvent * );
    void dragMoveEvent( TQDragMoveEvent *e );
    void dragLeaveEvent( TQDragLeaveEvent *e );
    void dropEvent( TQDropEvent * );
#endif
    void contextMenuEvent( TQContextMenuEvent * );
    bool sendMouseEventToInputContext( TQMouseEvent *e );
    virtual TQPopupMenu *createPopupMenu();
    void windowActivationChange( bool );
#ifndef TQT_NO_COMPAT
    void repaintArea( int, int ) { update(); }
#endif

private slots:
    void clipboardChanged();

public:
    void setPasswordChar( TQChar c ); // internal obsolete
    TQChar passwordChar() const; // obsolete internal
    int characterAt( int, TQChar* ) const; // obsolete
    bool getSelection( int *, int * ); // obsolete

private:
    friend struct TQLineEditPrivate;
    TQLineEditPrivate * d;

private:	// Disabled copy constructor and operator=
#if defined(TQ_DISABLE_COPY)
    TQLineEdit( const TQLineEdit & );
    TQLineEdit &operator=( const TQLineEdit & );
#endif
};


#endif // TQT_NO_LINEEDIT

#endif // TQLINEEDIT_H
