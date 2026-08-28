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

/*  TRANSLATOR MsgEdit

  This is the right panel of the main window.
*/

#include "msgedit.h"

#include "trwindow.h"
#include "phraselv.h"
#include "simtexth.h"

#include <ntqapplication.h>
#include <ntqcheckbox.h>
#include <ntqclipboard.h>
#include <ntqlabel.h>
#include <ntqlayout.h>
#include <ntqtextedit.h>
#include <ntqpalette.h>
#include <ntqpushbutton.h>
#include <ntqstring.h>
#include <ntqtextview.h>
#include <ntqwhatsthis.h>
#include <ntqvbox.h>
#include <ntqmainwindow.h>
#include <ntqheader.h>
#include <ntqregexp.h>
#include <ntqdockarea.h>
#include <ntqdockwindow.h>
#include <ntqscrollview.h>
#include <ntqfont.h>
#include <ntqaccel.h>
#include <private/qrichtext_p.h>

static const int MaxCandidates = 5;

class MED : public TQTextEdit
{
public:
    MED( TQWidget *parent, const char *name = 0 )
	: TQTextEdit( parent, name ) {}

    int cursorX() const { return textCursor()->x(); }
    int cursorY() const { return textCursor()->paragraph()->rect().y() +
			         textCursor()->y(); }
};


TQString richMeta( const TQString& text )
{
    return TQString( "<small><font color=blue>(" ) + text +
	   TQString( ")</font></small>" );
}

TQString richText( const TQString& text )
{
    const char backTab[] = "\a\b\f\n\r\t";
    const char * const friendlyBackTab[] = {
	TQT_TRANSLATE_NOOP( "MessageEditor", "bell" ),
	TQT_TRANSLATE_NOOP( "MessageEditor", "backspace" ),
	TQT_TRANSLATE_NOOP( "MessageEditor", "new page" ),
	TQT_TRANSLATE_NOOP( "MessageEditor", "new line" ),
	TQT_TRANSLATE_NOOP( "MessageEditor", "carriage return" ),
	TQT_TRANSLATE_NOOP( "MessageEditor", "tab" )
    };
    TQString rich;

    for ( int i = 0; i < (int) text.length(); i++ ) {
	int ch = text[i].unicode();

	if ( ch < 0x20 ) {
	    const char *p = strchr( backTab, ch );
	    if ( p == 0 )
		rich += richMeta( TQString::number(ch, 16) );
	    else
		rich += richMeta( MessageEditor::tr(friendlyBackTab[p - backTab]) );
	} else if ( ch == '<' ) {
	    rich += TQString( "&lt;" );
	} else if ( ch == '>' ) {
	    rich += TQString( "&gt;" );
	} else if ( ch == '&' ) {
	    rich += TQString( "&amp;" );
	} else if ( ch == ' ' ) {
	    if ( i == 0 || i == ((int) text.length()-1) || text[i - 1] == ' ' || text[i + 1] == ' ' ) {
		rich += richMeta( MessageEditor::tr("sp") );
	    } else {
		rich += ' ';
	    }
	} else {
	    rich += TQChar( ch );
	}
	if ( ch == '\n' )
	    rich += TQString( "<br>" );
    }
    return rich;
}

/*
   ShadowWidget class impl.

   Used to create a shadow like effect for a widget
*/
ShadowWidget::ShadowWidget( TQWidget * parent, const char * name )
    : TQWidget( parent, name ), sWidth( 10 ), wMargin( 3 ), childWgt( 0 )
{
}

ShadowWidget::ShadowWidget( TQWidget * child, TQWidget * parent,
			    const char * name )
    : TQWidget( parent, name ), sWidth( 10 ), wMargin( 3 ), childWgt( 0 )
{
    setWidget( child );
}

void ShadowWidget::setWidget( TQWidget * child )
{
    childWgt = child;
    if ( childWgt && childWgt->parent() != this ) {
	childWgt->reparent( this, TQPoint( 0, 0 ), true );
    }
}

void ShadowWidget::resizeEvent( TQResizeEvent * )
{
    if( childWgt ) {
	childWgt->move( wMargin, wMargin );
	childWgt->resize( width() - sWidth - wMargin, height() - sWidth -
			  wMargin );
    }
}

void ShadowWidget::paintEvent( TQPaintEvent * e )
{
    TQPainter p;
    int w = width() - sWidth;
    int h = height() - sWidth;


    if ( !((w > 0) && (h > 0)) )
	return;

    if ( p.begin( this ) ) {
	p.setPen( colorGroup().shadow() );

	p.drawPoint( w + 5, 6 );
	p.drawLine( w + 3, 6, w + 5, 8 );
	p.drawLine( w + 1, 6, w + 5, 10 );
	int i;
	for( i=7; i < h; i += 2 )
	    p.drawLine( w, i, w + 5, i + 5 );
	for( i = w - i + h; i > 6; i -= 2 )
	    p.drawLine( i, h, i + 5, h + 5 );
	for( ; i > 0 ; i -= 2 )
	    p.drawLine( 6, h + 6 - i, i + 5, h + 5 );

//	p.eraseRect( w, 0, sWidth, 45 ); // Cheap hack for the page curl..
	p.end();
    }
    TQWidget::paintEvent( e );
}

/*
   EditorPage class impl.

   A frame that contains the source text, translated text and any
   source code comments and hints.
*/
EditorPage::EditorPage( TQWidget * parent, const char * name )
    : TQFrame( parent, name )
{
    setLineWidth( 1 );
    setFrameStyle( TQFrame::Box | TQFrame::Plain );

    // Use white explicitly as the background color for the editor page.
    TQPalette p = palette();
    p.setColor( TQPalette::Active, TQColorGroup::Base, TQColor( white ) );
    p.setColor( TQPalette::Inactive, TQColorGroup::Base, TQColor( white ) );
    p.setColor( TQPalette::Disabled, TQColorGroup::Base, TQColor( white ) );
    p.setColor( TQPalette::Active, TQColorGroup::Background,
		p.active().color( TQColorGroup::Base ) );
    p.setColor( TQPalette::Inactive, TQColorGroup::Background,
		p.inactive().color( TQColorGroup::Base ) );
    p.setColor( TQPalette::Disabled, TQColorGroup::Background,
		p.disabled().color( TQColorGroup::Base ) );
    setPalette( p );

    srcTextLbl = new TQLabel( tr("Source text"), this, "source text label" );
    transLbl   = new TQLabel( tr("Translation"), this, "translation label" );

    TQFont fnt = font();
    fnt.setBold( true );
    srcTextLbl->setFont( fnt );
    transLbl->setFont( fnt );

    srcText = new TQTextView( this, "source text view" );
    srcText->setFrameStyle( TQFrame::NoFrame );
    srcText->setSizePolicy( TQSizePolicy( TQSizePolicy::MinimumExpanding,
					 TQSizePolicy::Minimum ) );
    srcText->setResizePolicy( TQScrollView::AutoOne );
    srcText->setHScrollBarMode( TQScrollView::AlwaysOff );
    srcText->setVScrollBarMode( TQScrollView::AlwaysOff );
    p = srcText->palette();
    p.setColor( TQPalette::Disabled, TQColorGroup::Base, p.active().base() );
    srcText->setPalette( p );
    connect( srcText, TQ_SIGNAL(textChanged()), TQ_SLOT(handleSourceChanges()) );

    cmtText = new TQTextView( this, "comment/context view" );
    cmtText->setFrameStyle( TQFrame::NoFrame );
    cmtText->setSizePolicy( TQSizePolicy( TQSizePolicy::MinimumExpanding,
					 TQSizePolicy::Minimum ) );
    cmtText->setResizePolicy( TQScrollView::AutoOne );
    cmtText->setHScrollBarMode( TQScrollView::AlwaysOff );
    cmtText->setVScrollBarMode( TQScrollView::AlwaysOff );
    p = cmtText->palette();
    p.setColor( TQPalette::Active, TQColorGroup::Base, TQColor( 236,245,255 ) );
    p.setColor( TQPalette::Inactive, TQColorGroup::Base, TQColor( 236,245,255 ) );
    cmtText->setPalette( p );
    connect( cmtText, TQ_SIGNAL(textChanged()), TQ_SLOT(handleCommentChanges()) );

    translationMed = new MED( this, "translation editor" );
    translationMed->setFrameStyle( TQFrame::NoFrame );
    translationMed->setSizePolicy( TQSizePolicy( TQSizePolicy::MinimumExpanding,
					     TQSizePolicy::MinimumExpanding ) );
    translationMed->setHScrollBarMode( TQScrollView::AlwaysOff );
    translationMed->setVScrollBarMode( TQScrollView::AlwaysOff );
    translationMed->setResizePolicy( TQScrollView::AutoOne );
    translationMed->setWrapPolicy( TQTextView::AtWhiteSpace );
    translationMed->setWordWrap( TQTextView::WidgetWidth );
    translationMed->setTextFormat( TQTextView::PlainText );
    p = translationMed->palette();
    p.setColor( TQPalette::Disabled, TQColorGroup::Base, p.active().base() );
    translationMed->setPalette( p );
    connect( translationMed, TQ_SIGNAL(textChanged()),
	     TQ_SLOT(handleTranslationChanges()) );

    pageCurl = new PageCurl( this, "page curl" );

    // Focus
    setFocusPolicy( StrongFocus );
    transLbl->setFocusProxy( translationMed );
    srcTextLbl->setFocusProxy( translationMed );
    cmtText->setFocusProxy( translationMed );
    srcText->setFocusProxy( translationMed );
    setFocusProxy( translationMed );

    updateCommentField();
}

/*
   Don't show the comment field if there are no comments.
*/
void EditorPage::updateCommentField()
{
    if( cmtText->text().isEmpty() )
	cmtText->hide();
    else
	cmtText->show();

    layoutWidgets();
}

/*
   Handle the widget layout manually
*/
void EditorPage::layoutWidgets()
 {
    int margin = 6;
    int space  = 2;
    int w = width();

    pageCurl->move( width() - pageCurl->width(), 0 );

    TQFontMetrics fm( srcTextLbl->font() );
    srcTextLbl->move( margin, margin );
    srcTextLbl->resize( fm.width( srcTextLbl->text() ), srcTextLbl->height() );

    srcText->move( margin, srcTextLbl->y() + srcTextLbl->height() + space );
    srcText->resize( w - margin*2, srcText->height() );

    cmtText->move( margin, srcText->y() + srcText->height() + space );
    cmtText->resize( w - margin*2, cmtText->height() );

    if( cmtText->isHidden() )
	transLbl->move( margin, srcText->y() + srcText->height() + space );
    else
	transLbl->move( margin, cmtText->y() + cmtText->height() + space );
    transLbl->resize( w - margin*2, transLbl->height() );

    translationMed->move( margin, transLbl->y() + transLbl->height() + space );
    translationMed->resize( w - margin*2, translationMed->height() );

    // Calculate the total height for the editor page - emit a signal
    // if the actual page size is larger/smaller
    int totHeight = margin + srcTextLbl->height() +
		    srcText->height() + space +
		    transLbl->height() + space +
		    translationMed->height() + space +
		    frameWidth()*lineWidth()*2 + space * 3;

    if( !cmtText->isHidden() )
	totHeight += cmtText->height() + space;

     if( height() != totHeight )
	 emit pageHeightUpdated( totHeight );
}

void EditorPage::resizeEvent( TQResizeEvent * )
{
    handleTranslationChanges();
    handleSourceChanges();
    handleCommentChanges();
    layoutWidgets();
}

void EditorPage::handleTranslationChanges()
{
    calculateFieldHeight( (TQTextView *) translationMed );
}

void EditorPage::handleSourceChanges()
{
    calculateFieldHeight( srcText );
}

void EditorPage::handleCommentChanges()
{
    calculateFieldHeight( cmtText );
}

/*
   Check if the translation text field is big enough to show all text
   that has been entered. If it isn't, resize it.
*/
void EditorPage::calculateFieldHeight( TQTextView * field )
{
    field->sync(); // make sure the text formatting is done!
    int contentsHeight = field->contentsHeight();

    if( contentsHeight != field->height() ) {
	int oldHeight = field->height();
	if( contentsHeight < 30 )
	    contentsHeight = 30;
	field->resize( field->width(), contentsHeight );
	emit pageHeightUpdated( height() + (field->height() - oldHeight) );
    }
}

void EditorPage::fontChange( const TQFont & )
{
    TQFont fnt = font();

    fnt.setBold( true );
    TQFontMetrics fm( fnt );
    srcTextLbl->setFont( fnt );
    srcTextLbl->resize( fm.width( srcTextLbl->text() ), srcTextLbl->height() );
    transLbl->setFont( fnt );
    transLbl->resize( fm.width( transLbl->text() ), transLbl->height() );
    update();
}

/*
   MessageEditor class impl.

   Handle layout of dock windows and the editor page.
*/
MessageEditor::MessageEditor( MetaTranslator * t, TQWidget * parent,
			      const char * name )
    : TQWidget( parent, name ),
      tor( t )
{
    doGuesses = true;
    v = new TQVBoxLayout( this );
    topDock = new TQDockArea( TQt::Horizontal, TQDockArea::Normal, this,
			     "top dock area" );
    topDock->setMinimumHeight( 10 );
    topDock->setSizePolicy( TQSizePolicy( TQSizePolicy::Minimum,
 					 TQSizePolicy::Minimum) );

    topDockWnd = new TQDockWindow( TQDockWindow::InDock, topDock,
				  "top dock window" );
    TQMainWindow *mw = (TQMainWindow*)topLevelWidget();
    if ( mw ) {
	mw->setDockEnabled( topDockWnd, TQt::DockTop, true );
	mw->setDockEnabled( topDockWnd, TQt::DockLeft, true );
	mw->setDockEnabled( topDockWnd, TQt::DockRight, true );
	mw->setDockEnabled( topDockWnd, TQt::DockBottom, true );
    }

    topDockWnd->setCaption( tr("Source text") );
    topDockWnd->setCloseMode( TQDockWindow::Always );
    topDockWnd->setResizeEnabled( true );
    topDockWnd->setFixedExtentHeight( 110 );

    srcTextList = new TQListView( topDockWnd, "source text list view" );
    srcTextList->setShowSortIndicator( true );
    srcTextList->setAllColumnsShowFocus( true );
    srcTextList->setSorting( 0 );
    TQFontMetrics fm( font() );
    srcTextList->addColumn( tr("Done"), fm.width( tr("Done") ) + 10 );
    srcTextList->addColumn( tr("Source text") );
    srcTextList->addColumn( tr("Translation"), 300 );
    srcTextList->setColumnAlignment( 0, TQt::AlignCenter );
    srcTextList->setColumnWidthMode( 1, TQListView::Manual );
    srcTextList->header()->setStretchEnabled( true, 1 );
    srcTextList->setMinimumSize( TQSize( 50, 50 ) );
    srcTextList->setHScrollBarMode( TQScrollView::AlwaysOff );
    srcTextList->installEventFilter( this );
    topDockWnd->setWidget( srcTextList );

    sv = new TQScrollView( this, "scroll view" );
    sv->setHScrollBarMode( TQScrollView::AlwaysOff );
    sv->viewport()->setBackgroundMode( PaletteBackground );

    editorPage = new EditorPage( sv, "editor page" );
    connect( editorPage, TQ_SIGNAL(pageHeightUpdated(int)),
	     TQ_SLOT(updatePageHeight(int)) );

    editorPage->translationMed->installEventFilter( this );

    sw = new ShadowWidget( editorPage, sv, "editor page shadow" );
    sw->setSizePolicy( TQSizePolicy( TQSizePolicy::Expanding,
				    TQSizePolicy::Expanding) );
    sw->setMinimumSize( TQSize( 100, 150 ) );
    sv->addChild( sw );

    bottomDock = new TQDockArea( TQt::Horizontal, TQDockArea::Reverse,
				this, "bottom dock area" );
    bottomDock->setMinimumHeight( 10 );
    bottomDock->setSizePolicy( TQSizePolicy( TQSizePolicy::Minimum,
					    TQSizePolicy::Minimum) );
    bottomDockWnd = new TQDockWindow( TQDockWindow::InDock, bottomDock,
				     "bottom dock window" );
    if ( mw ) {
	mw->setDockEnabled( bottomDockWnd, TQt::DockTop, true );
	mw->setDockEnabled( bottomDockWnd, TQt::DockLeft, true );
	mw->setDockEnabled( bottomDockWnd, TQt::DockRight, true );
	mw->setDockEnabled( bottomDockWnd, TQt::DockBottom, true );
    }
    bottomDockWnd->setCaption( tr("Phrases") );
    bottomDockWnd->setCloseMode( TQDockWindow::Always );
    bottomDockWnd->setResizeEnabled( true );

    TQWidget * w = new TQWidget( bottomDockWnd );
    w->setSizePolicy( TQSizePolicy( TQSizePolicy::Minimum,
				   TQSizePolicy::Minimum ) );
    TQHBoxLayout *hl = new TQHBoxLayout( w, 6 );
    TQVBoxLayout *vl = new TQVBoxLayout( 6 );

    phraseLbl = new TQLabel( tr("Phrases and guesses:"), w );
    phraseLv = new PhraseLV( w, "phrase list view" );
    phraseLv->setSorting( PhraseLVI::DefinitionText );
    phraseLv->installEventFilter( this );
    hl->addLayout( vl );
    vl->addWidget( phraseLbl );
    vl->addWidget( phraseLv );

    accel = new TQAccel( this, "accel" );
    connect( accel, TQ_SIGNAL(activated(int)), this, TQ_SLOT(guessActivated(int)) );
    for ( int i = 0; i < 9; i++ )
	accel->insertItem( CTRL + (Key_1 + i), i + 1 );

    bottomDockWnd->setWidget( w );

    v->addWidget( topDock );
    v->addWidget( sv );
    v->addWidget( bottomDock );

    // Signals
    connect( editorPage->pageCurl, TQ_SIGNAL(nextPage()),
	     TQ_SIGNAL(nextUnfinished()) );
    connect( editorPage->pageCurl, TQ_SIGNAL(prevPage()),
	     TQ_SIGNAL(prevUnfinished()) );

    connect( editorPage->translationMed, TQ_SIGNAL(textChanged()),
	     this, TQ_SLOT(emitTranslationChanged()) );
    connect( editorPage->translationMed, TQ_SIGNAL(textChanged()),
	     this, TQ_SLOT(updateButtons()) );
    connect( editorPage->translationMed, TQ_SIGNAL(undoAvailable(bool)),
	     this, TQ_SIGNAL(undoAvailable(bool)) );
    connect( editorPage->translationMed, TQ_SIGNAL(redoAvailable(bool)),
	     this, TQ_SIGNAL(redoAvailable(bool)) );
    connect( editorPage->translationMed, TQ_SIGNAL(copyAvailable(bool)),
	     this, TQ_SIGNAL(cutAvailable(bool)) );
    connect( editorPage->translationMed, TQ_SIGNAL(copyAvailable(bool)),
	     this, TQ_SIGNAL(copyAvailable(bool)) );
    connect( tqApp->clipboard(), TQ_SIGNAL(dataChanged()),
	     this, TQ_SLOT(updateCanPaste()) );
    connect( phraseLv, TQ_SIGNAL(doubleClicked(TQListViewItem *)),
	     this, TQ_SLOT(insertPhraseInTranslation(TQListViewItem *)) );
    connect( phraseLv, TQ_SIGNAL(returnPressed(TQListViewItem *)),
	     this, TQ_SLOT(insertPhraseInTranslationAndLeave(TQListViewItem *)) );

    // What's this
    TQWhatsThis::add( this, tr("This whole panel allows you to view and edit "
			      "the translation of some source text.") );
    TQWhatsThis::add( editorPage->srcText,
		     tr("This area shows the source text.") );
    TQWhatsThis::add( editorPage->cmtText, tr("This area shows a comment that"
			" may guide you, and the context in which the text"
			" occurs.") );
    TQWhatsThis::add( editorPage->translationMed,
		     tr("This is where you can enter or modify"
			" the translation of some source text.") );
}

void MessageEditor::toggleFinished()
{
    if ( itemFinished )
	itemFinished = false;
    else
	itemFinished = true;
    emit finished( itemFinished );
}

bool MessageEditor::eventFilter( TQObject *o, TQEvent *e )
{
    // Handle keypresses in the message editor - scroll the view if the current
    // line is hidden.
    if ( e->type() == TQEvent::KeyPress || e->type() == TQEvent::KeyRelease ) {
	TQKeyEvent * ke = (TQKeyEvent*)e;
	const int k = ke->key();

	if ( ::tqt_cast<TQTextEdit*>(o) ) {
	    if ( e->type() == TQEvent::KeyPress ) {
		// Hardcode the Tab key to do focus changes when pressed
		// inside the editor
                if ( k == Key_BackTab ) {
                    emit focusSourceList();
                    return true;
                }
	    } else if ( e->type() == TQEvent::KeyRelease ) {
		MED * ed = (MED *) o;
		switch( k ) {
		case Key_Up:
		    if (ed->cursorY() < 10)
			sv->verticalScrollBar()->subtractLine();
		    break;

		case Key_Down:
		    if (ed->cursorY() >= ed->height() - 20)
			sv->verticalScrollBar()->addLine();
		    break;

		case Key_PageUp:
		    if (ed->cursorY() < 10)
			sv->verticalScrollBar()->subtractPage();
		    break;

		case Key_PageDown:
		    if (ed->cursorY() >= ed->height() - 50)
			sv->verticalScrollBar()->addPage();
		    break;
		default:
		    sv->ensureVisible( sw->margin() + ed->x() + ed->cursorX(),
				       sw->margin() + ed->y() + ed->cursorY() );
		    break;
		}
	    }
	} else if ( ::tqt_cast<TQListView*>(o) ) {
	    // handle the ESC key in the list views
	    if ( e->type() == TQEvent::KeyRelease && k == Key_Escape )
		editorPage->translationMed->setFocus();
	}
    }
    return TQWidget::eventFilter( o, e );
}

void MessageEditor::updatePageHeight( int height )
{
    sw->resize( sw->width(), height + sw->margin() + sw->shadowWidth() );
}

void MessageEditor::resizeEvent( TQResizeEvent * )
{
    sw->resize( sv->viewport()->width(), sw->height() );
}

TQListView * MessageEditor::sourceTextList() const
{
    return srcTextList;
}

TQListView * MessageEditor::phraseList() const
{
    return phraseLv;
}

void MessageEditor::showNothing()
{
    editorPage->srcText->setText( "" );
    showContext( TQString(""), false );
}

void MessageEditor::showContext( const TQString& context, bool finished )
{
    setEditionEnabled( false );
    sourceText = TQString::null;
    guesses.clear();

    if( context.isEmpty() )
	editorPage->cmtText->setText("");
    else
	editorPage->cmtText->setText( richText(context.simplifyWhiteSpace()) );
    setTranslation( TQString(""), false );
    setFinished( finished );
    phraseLv->clear();
    editorPage->handleSourceChanges();
    editorPage->handleCommentChanges();
    editorPage->handleTranslationChanges();
    editorPage->updateCommentField();
}

void MessageEditor::showMessage( const TQString& text,
				 const TQString& comment,
				 const TQString& fullContext,
				 const TQString& translation,
				 MetaTranslatorMessage::Type type,
				 const TQValueList<Phrase>& phrases )
{
    bool obsolete = ( type == MetaTranslatorMessage::Obsolete );
    setEditionEnabled( !obsolete );
    sourceText = text;
    guesses.clear();

    editorPage->srcText->setText( TQString("<p>") + richText( text ) +
				  TQString("</p>") );

    if ( !fullContext.isEmpty() && !comment.isEmpty() )
	editorPage->cmtText->setText( richText(fullContext.simplifyWhiteSpace()) +
				      "\n" + richText(comment.simplifyWhiteSpace()) );
    else if ( !fullContext.isEmpty() && comment.isEmpty() )
	editorPage->cmtText->setText(richText(fullContext.simplifyWhiteSpace() ) );
    else if ( fullContext.isEmpty() && !comment.isEmpty() )
	editorPage->cmtText->setText( richText(comment.simplifyWhiteSpace() ) );
    else
	editorPage->cmtText->setText( "" );

    setTranslation( translation, false );
    setFinished( type != MetaTranslatorMessage::Unfinished );
    TQValueList<Phrase>::ConstIterator p;
    phraseLv->clear();
    for ( p = phrases.begin(); p != phrases.end(); ++p )
 	(void) new PhraseLVI( phraseLv, *p );

    if ( doGuesses && !sourceText.isEmpty() ) {
	CandidateList cl = similarTextHeuristicCandidates( tor,
							   sourceText.latin1(),
							   MaxCandidates );
	int n = 0;
	TQValueList<Candidate>::Iterator it = cl.begin();
	while ( it != cl.end() ) {
	    TQString def;
	    if ( n < 9 )
		def = tr( "Guess (%1)" ).arg( TQString(TQKeySequence(CTRL | (Key_0 + (n + 1)))) );
	    else
		def = tr( "Guess" );
	    (void) new PhraseLVI( phraseLv,
				  Phrase((*it).source, (*it).target, def),
				  n + 1 );
	    n++;
	    ++it;
	}
    }
    editorPage->handleSourceChanges();
    editorPage->handleCommentChanges();
    editorPage->handleTranslationChanges();
    editorPage->updateCommentField();
}

void MessageEditor::setTranslation( const TQString& translation, bool emitt )
{
    // Block signals so that 'textChanged()' is not emitted when
    // for example a new source text item is selected and *not*
    // the actual translation.
    editorPage->translationMed->blockSignals( !emitt );
    editorPage->translationMed->setText( translation );
    editorPage->translationMed->blockSignals( false );
    if ( !emitt )
 	updateButtons();
    emit cutAvailable( false );
    emit copyAvailable( false );
}

void MessageEditor::setEditionEnabled( bool enabled )
{
    editorPage->transLbl->setEnabled( enabled );
    editorPage->translationMed->setReadOnly( !enabled );

    phraseLbl->setEnabled( enabled );
    phraseLv->setEnabled( enabled );
    updateCanPaste();
}

void MessageEditor::undo()
{
    editorPage->translationMed->undo();
}

void MessageEditor::redo()
{
    editorPage->translationMed->redo();
}

void MessageEditor::cut()
{
    editorPage->translationMed->cut();
}

void MessageEditor::copy()
{
    editorPage->translationMed->copy();
}

void MessageEditor::paste()
{
    editorPage->translationMed->paste();
}

void MessageEditor::del()
{
    editorPage->translationMed->del();
}

void MessageEditor::selectAll()
{
    editorPage->translationMed->selectAll();
}

void MessageEditor::emitTranslationChanged()
{
    emit translationChanged( editorPage->translationMed->text() );
}

void MessageEditor::guessActivated( int accelKey )
{
    TQListViewItem *item = phraseLv->firstChild();
    while ( item != 0 && ((PhraseLVI *) item)->accelKey() != accelKey )
	item = item->nextSibling();
    if ( item != 0 )
	insertPhraseInTranslation( item );
}

void MessageEditor::insertPhraseInTranslation( TQListViewItem *item )
{
    editorPage->translationMed->insert(((PhraseLVI *) item)->phrase().target());
    emit translationChanged( editorPage->translationMed->text() );
}

void MessageEditor::insertPhraseInTranslationAndLeave( TQListViewItem *item )
{
    editorPage->translationMed->insert(((PhraseLVI *) item)->phrase().target());
    emit translationChanged( editorPage->translationMed->text() );
    editorPage->translationMed->setFocus();
}

void MessageEditor::updateButtons()
{
    bool overwrite = ( !editorPage->translationMed->isReadOnly() &&
	     (editorPage->translationMed->text().stripWhiteSpace().isEmpty() ||
	      mayOverwriteTranslation) );
    mayOverwriteTranslation = false;
    emit updateActions( overwrite );
}

void MessageEditor::beginFromSource()
{
    mayOverwriteTranslation = true;
    setTranslation( sourceText, true );
    if ( !editorPage->hasFocus() )
	editorPage->setFocus();
}

void MessageEditor::finishAndNext()
{
    setFinished( true );
    emit nextUnfinished();
    if ( !editorPage->hasFocus() )
	editorPage->setFocus();
}

void MessageEditor::updateCanPaste()
{
    bool oldCanPaste = canPaste;
    canPaste = ( !editorPage->translationMed->isReadOnly() &&
		 !tqApp->clipboard()->text().isNull() );
    if ( canPaste != oldCanPaste )
	emit pasteAvailable( canPaste );
}

void MessageEditor::setFinished( bool finished )
{
    if ( !finished != !itemFinished )
	toggleFinished();
}

void MessageEditor::toggleGuessing()
{
    doGuesses = !doGuesses;
    if ( !doGuesses ) {
	phraseLv->clear();
    }
}
