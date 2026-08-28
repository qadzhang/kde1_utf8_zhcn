/**********************************************************************
** Copyright (C) 2000-2008 Trolltech ASA.  All rights reserved.
**
** This file is part of TQt Linguist.
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

#ifndef MSGEDIT_H
#define MSGEDIT_H

#include "trwindow.h"
#include "phrase.h"

#include <metatranslator.h>

#include <ntqsplitter.h>
#include <ntqstring.h>
#include <ntqstringlist.h>
#include <ntqvaluelist.h>
#include <ntqscrollview.h>
#include <ntqpixmap.h>
#include <ntqbitmap.h>
#include <ntqtooltip.h>

class TQAccel;
class TQDockArea;
class TQDockWindow;
class TQLabel;
class TQListView;
class TQListViewItem;
class TQTextView;
class TQVBoxLayout;

class EditorPage;
class MED;
class PhraseLV;

class PageCurl : public TQWidget
{
    TQ_OBJECT
public:
    PageCurl( TQWidget * parent = 0, const char * name = 0,
	      WFlags f = 0 )
	: TQWidget( parent, name, f )
    {
	TQPixmap px = TrWindow::pageCurl();
	if ( px.mask() ) {
	    setMask( *px.mask() );
	}
	setBackgroundPixmap( px );
	setFixedSize( px.size() );
	
	TQRect r( 34, 0, width()-34, 19 );
	TQToolTip::add( this, r, tr("Next unfinished phrase") );
	r.setSize( TQSize(width()-34, height()-20) );
	r.setX( 0 );
	r.setY( 20 );
	TQToolTip::add( this, r, tr("Previous unfinished phrase") );
    }
    
protected:
    void mouseReleaseEvent( TQMouseEvent * e )
    {
	int x = e->pos().x() - 14;
	int y = e->pos().y() - 8;
		
	if ( y <= x )
	    emit nextPage();
	else
	    emit prevPage();
    }
    
signals:
    void prevPage();
    void nextPage();
};

class ShadowWidget : public TQWidget
{    
public:
    ShadowWidget( TQWidget * parent = 0, const char * name = 0 );
    ShadowWidget( TQWidget * child, TQWidget * parent = 0, const char * name = 0 );
    
    void setShadowWidth( int width ) { sWidth = width; }
    int  shadowWidth() const { return sWidth; }
    void setMargin( int margin ){ wMargin = margin; }
    int  margin() const { return wMargin; }
    void setWidget( TQWidget * child);
    
protected:
    void resizeEvent( TQResizeEvent * e );
    void paintEvent( TQPaintEvent * e );
    
private:
    int sWidth;
    int wMargin;
    TQWidget * childWgt;
};

class EditorPage : public TQFrame
{
    TQ_OBJECT
public:
    EditorPage( TQWidget * parent = 0, const char * name = 0 );
    
protected:
    void resizeEvent( TQResizeEvent * );
    void layoutWidgets();
    void updateCommentField();
    void calculateFieldHeight( TQTextView * field );
    void fontChange( const TQFont & );
    
private:
    PageCurl * pageCurl;
    TQLabel * srcTextLbl;
    TQLabel * transLbl;
    TQTextView * srcText;
    TQTextView * cmtText;
    MED   * translationMed;

    friend class MessageEditor;
    
private slots:
    void handleTranslationChanges();
    void handleSourceChanges();
    void handleCommentChanges();
    
signals:
    void pageHeightUpdated( int height );
};

class MessageEditor : public TQWidget
{
    TQ_OBJECT
public:
    MessageEditor( MetaTranslator * t, TQWidget * parent = 0,
		   const char * name = 0 );
    TQListView * sourceTextList() const;
    TQListView * phraseList() const;
    
    void showNothing();
    void showContext( const TQString& context, bool finished );
    void showMessage( const TQString& text, const TQString& comment,
		      const TQString& fullContext, const TQString& translation,
		      MetaTranslatorMessage::Type type,
		      const TQValueList<Phrase>& phrases );
    void setFinished( bool finished );
    bool eventFilter( TQObject *, TQEvent * );

signals:
    void translationChanged( const TQString& translation );
    void finished( bool finished );
    void prevUnfinished();
    void nextUnfinished();
    void updateActions( bool enable );
    
    void undoAvailable( bool avail );
    void redoAvailable( bool avail );
    void cutAvailable( bool avail );
    void copyAvailable( bool avail );
    void pasteAvailable( bool avail );

    void focusSourceList();
    void focusPhraseList();
    
public slots:
    void undo();
    void redo();
    void cut();
    void copy();
    void paste();
    void del();
    void selectAll();
    void beginFromSource();
    void toggleGuessing();
    void finishAndNext();

private slots:
    void emitTranslationChanged();
    void guessActivated( int accelKey );
    void insertPhraseInTranslation( TQListViewItem *item );
    void insertPhraseInTranslationAndLeave( TQListViewItem *item );
    void updateButtons();
    void updateCanPaste();
    void toggleFinished();

    void updatePageHeight( int height );
    
protected:
    void resizeEvent( TQResizeEvent * );
    
private:
    void setTranslation( const TQString& translation, bool emitt );
    void setEditionEnabled( bool enabled );

    TQListView * srcTextList;
    TQDockArea * topDock, * bottomDock;
    TQDockWindow * topDockWnd, *bottomDockWnd;
    EditorPage * editorPage;
    TQVBoxLayout * v;

    TQLabel * phraseLbl;
    PhraseLV * phraseLv;
    TQAccel * accel;
    bool itemFinished;
    
    ShadowWidget * sw;
    TQScrollView * sv;
    
    MetaTranslator *tor;
    TQString sourceText;
    TQStringList guesses;
    bool mayOverwriteTranslation;
    bool canPaste;
    bool doGuesses;
};

#endif
