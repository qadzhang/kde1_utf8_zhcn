/**********************************************************************
** Copyright (C) 2005-2008 Trolltech ASA.  All rights reserved.
**
** This file is part of TQt Designer.
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

#include "completion.h"
#include "paragdata.h"
#include "editor.h"
#include <ntqlistbox.h>
#include <ntqvbox.h>
#include <ntqmap.h>
#include <private/qrichtext_p.h>
#include <ntqapplication.h>
#include <ntqregexp.h>
#include "arghintwidget.h"
#include <ntqsizegrip.h>
#include <ntqtimer.h>

static TQColor getColor( const TQString &type )
{
    if ( type == "function" || type == "slot" || type == "package" )
	return TQt::blue;
    else if ( type == "variable" || type == "widget" || type == "dir" )
	return TQt::darkRed;
    else if ( type == "object" || type == "class" )
	return TQt::darkBlue;
    else if ( type == "property" )
	return TQt::darkGreen;
    else if ( type == "enum" )
	return TQt::darkYellow;
    return TQt::black;
}

class CompletionItem : public TQListBoxItem
{
public:
    CompletionItem( TQListBox *lb, const TQString &txt, const TQString &t, const TQString &p,
		    const TQString &pre, const TQString &p2 )
	: TQListBoxItem( lb ), type( t ), postfix( p ), prefix( pre ), postfix2( p2 ),
	  parag( 0 ), lastState( false ) { setText( txt ); }
    ~CompletionItem() { delete parag; }
    void paint( TQPainter *painter ) {
	if ( lastState != isSelected() ) {
	    delete parag;
	    parag = 0;
	}
	lastState = isSelected();
	if ( !parag )
	    setupParagraph();
	parag->paint( *painter, listBox()->colorGroup() );
    }

    int height( const TQListBox * ) const {
	if ( !parag )
	    ( (CompletionItem*)this )->setupParagraph();
	return parag->rect().height();
    }
    int width( const TQListBox * ) const {
	if ( !parag )
	    ( (CompletionItem*)this )->setupParagraph();
	return parag->rect().width() - 2;
    }
    TQString text() const { return TQListBoxItem::text() + postfix; }

private:
    void setupParagraph();
    TQString type, postfix, prefix, postfix2;
    TQTextParagraph *parag;
    bool lastState;

};

void CompletionItem::setupParagraph() {
    if ( !parag ) {
	TQTextFormatter *formatter;
	formatter = new TQTextFormatterBreakWords;
	formatter->setWrapEnabled( false );
	parag = new TQTextParagraph( 0 );
	parag->setTabStops( listBox()->fontMetrics().width( "propertyXXXX" ) );
	parag->pseudoDocument()->pFormatter = formatter;
	parag->insert( 0, " " + type + ( type.isEmpty() ? " " : "\t" ) + prefix +
		       TQListBoxItem::text() + postfix + postfix2 );
	bool selCol = isSelected() && listBox()->colorGroup().highlightedText() != listBox()->colorGroup().text();
	TQColor sc = selCol ? listBox()->colorGroup().highlightedText() : getColor( type );
	TQTextFormat *f1 = parag->formatCollection()->format( listBox()->font(), sc );
	TQTextFormat *f3 = parag->formatCollection()->format( listBox()->font(), isSelected() ?
							     listBox()->colorGroup().highlightedText() :
							     listBox()->colorGroup().text() );
	TQFont f( listBox()->font() );
	f.setBold( true );
	TQTextFormat *f2 =
	    parag->formatCollection()->format( f, isSelected() ? listBox()->colorGroup().highlightedText() :
					       listBox()->colorGroup().text() );
	parag->setFormat( 1, type.length() + 1, f1 );
	parag->setFormat( type.length() + 2, prefix.length() + TQListBoxItem::text().length(), f2 );
	if ( !postfix.isEmpty() )
	    parag->setFormat( type.length() + 2 + prefix.length() + TQListBoxItem::text().length(),
			      postfix.length(), f3 );
	parag->setFormat( type.length() + 2 + prefix.length() + TQListBoxItem::text().length() + postfix.length(),
			  postfix2.length(), f3 );
	f1->removeRef();
	f2->removeRef();
	f3->removeRef();
	parag->format();
    }
}


EditorCompletion::EditorCompletion( Editor *e )
{
    enabled = true;
    lastDoc = 0;
    completionPopup = new TQVBox( e->topLevelWidget(), 0, WType_Popup );
    completionPopup->setFrameStyle( TQFrame::Box | TQFrame::Plain );
    completionPopup->setLineWidth( 1 );
    functionLabel = new ArgHintWidget( e->topLevelWidget(), "editor_function_lbl" );
    functionLabel->hide();
    completionListBox = new TQListBox( completionPopup, "editor_completion_lb" );
    completionListBox->setFrameStyle( TQFrame::NoFrame );
    completionListBox->installEventFilter( this );
    completionListBox->setHScrollBarMode( TQScrollView::AlwaysOn );
    completionListBox->setVScrollBarMode( TQScrollView::AlwaysOn );
    completionListBox->setCornerWidget( new TQSizeGrip( completionListBox, "editor_cornerwidget" ) );
    completionPopup->installEventFilter( this );
    functionLabel->installEventFilter( this );
    completionPopup->setFocusProxy( completionListBox );
    completionOffset = 0;
    curEditor = e;
    curEditor->installEventFilter( this );
}

EditorCompletion::~EditorCompletion()
{
    delete completionPopup;
    delete functionLabel;
}

void EditorCompletion::addCompletionEntry( const TQString &s, TQTextDocument *, bool strict )
{
    TQChar key( s[ 0 ] );
    TQMap<TQChar, TQStringList>::Iterator it = completionMap.find( key );
    if ( it == completionMap.end() ) {
	completionMap.insert( key, TQStringList( s ) );
    } else {
	if ( strict ) {
	    TQStringList::Iterator sit;
	    for ( sit = (*it).begin(); sit != (*it).end(); ) {
		TQStringList::Iterator it2 = sit;
		++sit;
		if ( (*it2).length() > s.length() && (*it2).left( s.length() ) == s ) {
		    if ( (*it2)[ (int)s.length() ].isLetter() && (*it2)[ (int)s.length() ].upper() != (*it2)[ (int)s.length() ] )
			return;
		} else if ( s.length() > (*it2).length() && s.left( (*it2).length() ) == *it2 ) {
		    if ( s[ (int)(*it2).length() ].isLetter() && s[ (int)(*it2).length() ].upper() != s[ (int)(*it2).length() ] )
			(*it).remove( it2 );
		}
	    }
	}
	(*it).append( s );
    }
}

TQValueList<CompletionEntry> EditorCompletion::completionList( const TQString &s, TQTextDocument *doc ) const
{
    if ( doc )
	( (EditorCompletion*)this )->updateCompletionMap( doc );

    TQChar key( s[ 0 ] );
    TQMap<TQChar, TQStringList>::ConstIterator it = completionMap.find( key );
    if ( it == completionMap.end() )
	return TQValueList<CompletionEntry>();
    TQStringList::ConstIterator it2 = (*it).begin();
    TQValueList<CompletionEntry> lst;
    int len = s.length();
    for ( ; it2 != (*it).end(); ++it2 ) {
	CompletionEntry c;
	c.type = "";
	c.text = *it2;
	c.postfix = "";
	c.prefix = "";
	c.postfix2 = "";
	if ( (int)(*it2).length() > len && (*it2).left( len ) == s && lst.find( c ) == lst.end() )
	    lst << c;
    }

    return lst;
}

void EditorCompletion::updateCompletionMap( TQTextDocument *doc )
{
    bool strict = true;
    if ( doc != lastDoc )
	strict = false;
    lastDoc = doc;
    TQTextParagraph *s = doc->firstParagraph();
    if ( !s->extraData() )
	s->setExtraData( new ParagData );
    while ( s ) {
	if ( s->length() == ( (ParagData*)s->extraData() )->lastLengthForCompletion ) {
	    s = s->next();
	    continue;
	}

	TQChar c;
	TQString buffer;
	for ( int i = 0; i < s->length(); ++i ) {
	    c = s->at( i )->c;
	    if ( c.isLetter() || c.isNumber() || c == '_' || c == '#' ) {
		buffer += c;
	    } else {
		addCompletionEntry( buffer, doc, strict );
		buffer = TQString::null;
	    }
	}
	if ( !buffer.isEmpty() )
	    addCompletionEntry( buffer, doc, strict );

	( (ParagData*)s->extraData() )->lastLengthForCompletion = s->length();
	s = s->next();
    }
}

bool EditorCompletion::doCompletion()
{
    searchString = "";
    if ( !curEditor )
	return false;

    TQTextCursor *cursor = curEditor->textCursor();
    TQTextDocument *doc = curEditor->document();

    if ( cursor->index() > 0 && cursor->paragraph()->at( cursor->index() - 1 )->c == '.' &&
	 ( cursor->index() == 1 || cursor->paragraph()->at( cursor->index() - 2 )->c != '.' ) )
	return doObjectCompletion();

    int idx = cursor->index();
    if ( idx == 0 )
	return false;
    TQChar c = cursor->paragraph()->at( idx - 1 )->c;
    if ( !c.isLetter() && !c.isNumber() && c != '_' && c != '#' )
	return false;

    TQString s;
    idx--;
    completionOffset = 1;
    for (;;) {
	s.prepend( TQString( cursor->paragraph()->at( idx )->c ) );
	idx--;
	if ( idx < 0 )
	    break;
	if ( !cursor->paragraph()->at( idx )->c.isLetter() &&
	     !cursor->paragraph()->at( idx )->c.isNumber() &&
	     cursor->paragraph()->at( idx )->c != '_' &&
	     cursor->paragraph()->at( idx )->c != '#' )
	    break;
	completionOffset++;
    }

    searchString = s;

    TQValueList<CompletionEntry> lst( completionList( s, doc ) );
    if ( lst.count() > 1 ) {
	TQTextStringChar *chr = cursor->paragraph()->at( cursor->index() );
	int h = cursor->paragraph()->lineHeightOfChar( cursor->index() );
	int x = cursor->paragraph()->rect().x() + chr->x;
	int y, dummy;
	cursor->paragraph()->lineHeightOfChar( cursor->index(), &dummy, &y );
	y += cursor->paragraph()->rect().y();
	completionListBox->clear();
	for ( TQValueList<CompletionEntry>::ConstIterator it = lst.begin(); it != lst.end(); ++it )
	    (void)new CompletionItem( completionListBox, (*it).text, (*it).type, (*it).postfix,
				      (*it).prefix, (*it).postfix2 );
	cList = lst;
	completionPopup->resize( completionListBox->sizeHint() +
				 TQSize( completionListBox->verticalScrollBar()->width() + 4,
					completionListBox->horizontalScrollBar()->height() + 4 ) );
	completionListBox->setCurrentItem( 0 );
	completionListBox->setFocus();
	if ( curEditor->mapToGlobal( TQPoint( 0, y ) ).y() + h + completionPopup->height() < TQApplication::desktop()->height() )
	    completionPopup->move( curEditor->mapToGlobal( curEditor->
							   contentsToViewport( TQPoint( x, y + h ) ) ) );
	else
	    completionPopup->move( curEditor->mapToGlobal( curEditor->
							   contentsToViewport( TQPoint( x, y - completionPopup->height() ) ) ) );
	completionPopup->show();
    } else if ( lst.count() == 1 ) {
	curEditor->insert( lst.first().text.mid( completionOffset, 0xFFFFFF ),
 			   (uint) ( TQTextEdit::RedoIndentation |
				    TQTextEdit::CheckNewLines |
				    TQTextEdit::RemoveSelected ) );
    } else {
	return false;
    }

    return true;
}

bool EditorCompletion::eventFilter( TQObject *o, TQEvent *e )
{
    if ( !enabled )
	return false;
    if ( e->type() == TQEvent::KeyPress && ::tqt_cast<Editor*>(o)) {
	curEditor = (Editor*)o;
	TQKeyEvent *ke = (TQKeyEvent*)e;
	if ( ke->key() == Key_Tab ) {
	    TQString s = curEditor->textCursor()->paragraph()->string()->toString().
			left( curEditor->textCursor()->index() );
	    if ( curEditor->document()->hasSelection( TQTextDocument::Standard ) ||
		 s.simplifyWhiteSpace().isEmpty() ) {
		if ( curEditor->document()->indent() ) {
		    curEditor->indent();
		    int i = 0;
		    for ( ; i < curEditor->textCursor()->paragraph()->length() - 1; ++i ) {
			if ( curEditor->textCursor()->paragraph()->at( i )->c != ' ' &&
			     curEditor->textCursor()->paragraph()->at( i )->c != '\t' )
			    break;
		    }
		    curEditor->drawCursor( false );
		    curEditor->textCursor()->setIndex( i );
		    curEditor->drawCursor( true );
		} else {
		    curEditor->insert( "\t" );
		}
		return true;
	    }
	}

	if ( functionLabel->isVisible() ) {
	    if ( ke->key() == Key_Up && ( ke->state() & ControlButton ) == ControlButton ) {
		functionLabel->gotoPrev();
		return true;
	    } else if ( ke->key() == Key_Down && ( ke->state() & ControlButton ) == ControlButton ) {
		functionLabel->gotoNext();
		return true;
	    }
	}

	if ( ( ke->text().length() && !( ke->state() & AltButton ) &&
	     ( !ke->ascii() || ke->ascii() >= 32 ) ) ||
	     ( ke->text() == "\t" && !( ke->state() & ControlButton ) ) ) {
	    if ( ke->key() == Key_Tab ) {
		if ( curEditor->textCursor()->index() == 0 &&
		     curEditor->textCursor()->paragraph()->isListItem() )
		    return false;
		if ( doCompletion() )
			return true;
	    } else if ( ( ke->key() == Key_Period &&
			( curEditor->textCursor()->index() == 0 ||
			  curEditor->textCursor()->paragraph()->at( curEditor->textCursor()->index() - 1 )->c != '.' ) )
			||
			( ke->key() == Key_Greater &&
			curEditor->textCursor()->index() > 0 &&
			curEditor->textCursor()->paragraph()->at( curEditor->textCursor()->index() - 1 )->c == '-' ) ) {
		doObjectCompletion();
	    } else {
		if ( !doArgumentHint( ke->text() == "(" ) )
		    functionLabel->hide();
	    }
	}
    } else if ( o == completionPopup || o == completionListBox ||
	 o == completionListBox->viewport() ) {
	if ( e->type() == TQEvent::KeyPress ) {
	    TQKeyEvent *ke = (TQKeyEvent*)e;
	    if ( ke->key() == Key_Enter || ke->key() == Key_Return || ke->key() == Key_Tab ) {
		if ( ke->key() == Key_Tab && completionListBox->count() > 1 &&
		     completionListBox->currentItem() < (int)completionListBox->count() - 1 ) {
		    completionListBox->setCurrentItem( completionListBox->currentItem() + 1 );
		    return true;
		}
		completeCompletion();
		return true;
	    } else if ( ke->key() == Key_Left || ke->key() == Key_Right ||
			ke->key() == Key_Up || ke->key() == Key_Down ||
			ke->key() == Key_Home || ke->key() == Key_End ||
			ke->key() == Key_Prior || ke->key() == Key_Next ) {
		return false;
	    } else if ( ke->key() != Key_Shift && ke->key() != Key_Control &&
			ke->key() != Key_Alt ) {
		int l = searchString.length();
		if ( ke->key() == Key_Backspace ) {
		    searchString.remove( searchString.length() - 1, 1 );
		} else {
		    searchString += ke->text();
		    l = 1;
		}
		if ( !l || !continueComplete() ) {
		    completionPopup->close();
		    curEditor->setFocus();
		}
		TQApplication::sendEvent( curEditor, e );
		return true;
	    }
	} else if ( e->type() == TQEvent::MouseButtonDblClick ) {
	    completeCompletion();
	    return true;
	}
    }
    if ( o == functionLabel || ( ::tqt_cast<Editor*>(o) && functionLabel->isVisible() ) ) {
	if ( e->type() == TQEvent::KeyPress ) {
	    TQKeyEvent *ke = (TQKeyEvent*)e;
	    if ( ke->key() == Key_Escape ) {
		functionLabel->hide();
	    } else {
		if ( !doArgumentHint( ke->text() == "(" ) )
		    functionLabel->hide();
		if ( o == functionLabel ) {
		    TQApplication::sendEvent( curEditor, e );
		    return true;
		}
	    }
	}
    }
    return false;
}

void EditorCompletion::completeCompletion()
{
    int idx = curEditor->textCursor()->index();
    TQString s = completionListBox->currentText().mid( searchString.length() );
    curEditor->insert( s, (uint) ( TQTextEdit::RedoIndentation |
				   TQTextEdit::CheckNewLines |
				   TQTextEdit::RemoveSelected ) );
    int i = s.find( '(' );
    completionPopup->close();
    curEditor->setFocus();
    if ( i != -1 && i < (int)s.length() ) {
	curEditor->setCursorPosition( curEditor->textCursor()->paragraph()->paragId(), idx + i + 1 );
	doArgumentHint( false );
    }
}

void EditorCompletion::setCurrentEdior( Editor *e )
{
    curEditor = e;
    curEditor->installEventFilter( this );
}

void EditorCompletion::addEditor( Editor *e )
{
    e->installEventFilter( this );
}

bool EditorCompletion::doObjectCompletion()
{
    searchString = "";
    TQString object;
    int i = curEditor->textCursor()->index();
    i--;
    TQTextParagraph *p = curEditor->textCursor()->paragraph();
    for (;;) {
	if ( i < 0 )
	    break;
	if ( p->at( i )->c == ' ' || p->at( i )->c == '\t' )
	    break;
	object.prepend( p->at( i )->c );
	i--;
    }

    if ( object[ (int)object.length() - 1 ] == '-' )
	object.remove( object.length() - 1, 1 );

    if ( object.isEmpty() )
	return false;
    return doObjectCompletion( object );
}

bool EditorCompletion::doObjectCompletion( const TQString & )
{
    return false;
}

static void strip( TQString &txt )
{
    int i = txt.find( "(" );
    if ( i == -1 )
	return;
    txt = txt.left( i );
}

bool EditorCompletion::continueComplete()
{
    if ( searchString.isEmpty() ) {
	completionListBox->clear();
	for ( TQValueList<CompletionEntry>::ConstIterator it = cList.begin(); it != cList.end(); ++it )
	    (void)new CompletionItem( completionListBox, (*it).text, (*it).type,
				      (*it).postfix, (*it).prefix, (*it).postfix2 );
	completionListBox->setCurrentItem( 0 );
	completionListBox->setSelected( completionListBox->currentItem(), true );
	return true;
    }

    TQListBoxItem *i = completionListBox->findItem( searchString );
    if ( !i )
	return false;

    TQString txt1 = i->text();
    TQString txt2 = searchString;
    strip( txt1 );
    strip( txt2 );
    if ( txt1 == txt2 && !i->next() )
	return false;

    TQValueList<CompletionEntry> res;
    for ( TQValueList<CompletionEntry>::ConstIterator it = cList.begin(); it != cList.end(); ++it ) {
	if ( (*it).text.left( searchString.length() ) == searchString )
	    res << *it;
    }
    if ( res.isEmpty() )
	return false;
    completionListBox->clear();
    for ( TQValueList<CompletionEntry>::ConstIterator it2 = res.begin(); it2 != res.end(); ++it2 )
	(void)new CompletionItem( completionListBox, (*it2).text, (*it2).type,
				  (*it2).postfix, (*it2).prefix, (*it2).postfix2 );
    completionListBox->setCurrentItem( 0 );
    completionListBox->setSelected( completionListBox->currentItem(), true );
    return true;
}

bool EditorCompletion::doArgumentHint( bool useIndex )
{
    TQTextCursor *cursor = curEditor->textCursor();
    int i = cursor->index() ;
    if ( !useIndex ) {
	bool foundParen = false;
	int closeParens = 0;
	while ( i >= 0 ) {
	    if ( cursor->paragraph()->at( i )->c == ')' && i != cursor->index() )
		closeParens++;
	    if ( cursor->paragraph()->at( i )->c == '(' ) {
		closeParens--;
		if ( closeParens == -1 ) {
		    foundParen = true;
		    break;
		}
	    }
	    --i;
	}

	if ( !foundParen )
	    return false;
    }
    int j = i - 1;
    bool foundSpace = false;
    bool foundNonSpace = false;
    while ( j >= 0 ) {
	if ( foundNonSpace && ( cursor->paragraph()->at( j )->c == ' ' || cursor->paragraph()->at( j )->c == ',' ) ) {
	    foundSpace = true;
	    break;
	}
	if ( !foundNonSpace && ( cursor->paragraph()->at( j )->c != ' ' || cursor->paragraph()->at( j )->c != ',' ) )
	    foundNonSpace = true;
	--j;
    }
    if ( foundSpace )
	++j;
    j = TQMAX( j, 0 );
    TQString function( cursor->paragraph()->string()->toString().mid( j, i - j + 1 ) );
    TQString part = cursor->paragraph()->string()->toString().mid( j, cursor->index() - j + 1 );
    function = function.simplifyWhiteSpace();
    for (;;) {
	if ( function[ (int)function.length() - 1 ] == '(' ) {
	    function.remove( function.length() - 1, 1 );
	    function = function.simplifyWhiteSpace();
	} else if ( function[ (int)function.length() - 1 ] == ')' ) {
	    function.remove( function.length() - 1, 1 );
	    function = function.simplifyWhiteSpace();
	} else {
	    break;
	}
    }

    TQChar sep;
    TQString pre, post;
    TQValueList<TQStringList> argl = functionParameters( function, sep, pre, post );
    if ( argl.isEmpty() )
	return false;

    TQString label;
    int w = 0;
    int num = 0;
    if ( !functionLabel->isVisible() )
	functionLabel->setNumFunctions( (int)argl.count() );
    for ( TQValueList<TQStringList>::Iterator vit = argl.begin(); vit != argl.end(); ++vit, ++num ) {
	TQStringList args = *vit;
	int argNum = 0;
	int inParen = 0;
	for ( int k = 0; k < (int)part.length(); ++k ) {
	    if ( part[ k ] == sep && inParen < 2 )
		argNum++;
	    if ( part[ k ] == '(' )
		inParen++;
	    if ( part[ k ] == ')' )
		inParen--;
	}

	TQString func = function;
	int pnt = -1;
	pnt = func.findRev( '.' );
	if ( pnt == -1 )
	    func.findRev( '>' );
	if ( pnt != -1 )
	    func = func.mid( pnt + 1 );

	TQString s = func + "( ";
	if ( s[ 0 ] == '\"' )
	    s.remove( (uint)0, 1 );
	i = 0;
	for ( TQStringList::Iterator it = args.begin(); it != args.end(); ++it, ++i ) {
	    if ( i == argNum )
		s += "<b>" + *it + "</b>";
	    else
		s += *it;
	    if ( i < (int)args.count() - 1 )
		s += ", ";
	    else
		s += " ";
	}
	s += ")";
	s.prepend( pre );
	s.append( post );
	label += "<p>" + s + "</p>";
	functionLabel->setFunctionText( num, s );
	w = TQMAX( w, functionLabel->fontMetrics().width( s ) + 10 );
    }
    w += 16;
    if ( label.isEmpty() )
	return false;
    if ( functionLabel->isVisible() ) {
	functionLabel->resize( w + 50, TQMAX( functionLabel->fontMetrics().height(), 16 ) );
    } else {
	TQTextStringChar *chr = cursor->paragraph()->at( cursor->index() );
	int h = cursor->paragraph()->lineHeightOfChar( cursor->index() );
	int x = cursor->paragraph()->rect().x() + chr->x;
	int y, dummy;
	cursor->paragraph()->lineHeightOfChar( cursor->index(), &dummy, &y );
	y += cursor->paragraph()->rect().y();
	functionLabel->resize( w + 50, TQMAX( functionLabel->fontMetrics().height(), 16 ) );
	functionLabel->move( curEditor->mapToGlobal( curEditor->contentsToViewport( TQPoint( x, y + h ) ) ) );
	if ( functionLabel->x() + functionLabel->width() > TQApplication::desktop()->width() )
	    functionLabel->move( TQMAX( 0, TQApplication::desktop()->width() - functionLabel->width() ),
				 functionLabel->y() );
	functionLabel->show();
	curEditor->setFocus();
    }
    TQTimer::singleShot( 0, functionLabel, TQ_SLOT( relayout() ) );

    return true;
}

TQValueList<TQStringList> EditorCompletion::functionParameters( const TQString &, TQChar &, TQString &, TQString & )
{
    return TQValueList<TQStringList>();
}

void EditorCompletion::setContext( TQObject * )
{
}

void EditorCompletion::showCompletion( const TQValueList<CompletionEntry> &lst )
{
    TQTextCursor *cursor = curEditor->textCursor();
    TQTextStringChar *chr = cursor->paragraph()->at( cursor->index() );
    int h = cursor->paragraph()->lineHeightOfChar( cursor->index() );
    int x = cursor->paragraph()->rect().x() + chr->x;
    int y, dummy;
    cursor->paragraph()->lineHeightOfChar( cursor->index(), &dummy, &y );
    y += cursor->paragraph()->rect().y();
    completionListBox->clear();
    for ( TQValueList<CompletionEntry>::ConstIterator it = lst.begin(); it != lst.end(); ++it )
	(void)new CompletionItem( completionListBox, (*it).text, (*it).type,
				  (*it).postfix, (*it).prefix, (*it).postfix2 );
    cList = lst;
    completionPopup->resize( completionListBox->sizeHint() +
			     TQSize( completionListBox->verticalScrollBar()->width() + 4,
				    completionListBox->horizontalScrollBar()->height() + 4 ) );
    completionListBox->setCurrentItem( 0 );
    completionListBox->setFocus();
    if ( curEditor->mapToGlobal( TQPoint( 0, y ) ).y() + h + completionPopup->height() < TQApplication::desktop()->height() )
	completionPopup->move( curEditor->mapToGlobal( curEditor->
						       contentsToViewport( TQPoint( x, y + h ) ) ) );
    else
	completionPopup->move( curEditor->mapToGlobal( curEditor->
						       contentsToViewport( TQPoint( x, y - completionPopup->height() ) ) ) );

    completionPopup->show();
}
