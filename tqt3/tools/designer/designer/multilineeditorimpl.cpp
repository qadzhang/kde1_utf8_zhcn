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

#include "multilineeditorimpl.h"
#include "formwindow.h"
#include "command.h"
#include "mainwindow.h"
#include "richtextfontdialog.h"
#include "syntaxhighlighter_html.h"
#include "widgetfactory.h"

#include <ntqtextedit.h>
#include <./private/qrichtext_p.h>
#include <ntqpushbutton.h>
#include <ntqlayout.h>
#include <ntqtoolbutton.h>
#include <ntqapplication.h>
#include <ntqaction.h>
#include <ntqmetaobject.h>

ToolBarItem::ToolBarItem( TQWidget *parent, TQWidget *toolBar,
			  const TQString &label, const TQString &tagstr,
			  const TQIconSet &icon, const TQKeySequence &key )
    : TQAction( parent )
{
    setIconSet( icon );
    setText( label );
    setAccel( key );
    addTo( toolBar );
    tag = tagstr;
    connect( this, TQ_SIGNAL( activated() ), this, TQ_SLOT( wasActivated() ) );
}

ToolBarItem::~ToolBarItem()
{

}

void ToolBarItem::wasActivated()
{
    emit clicked( tag );
}

TextEdit::TextEdit( TQWidget *parent, const char *name )
    : TQTextEdit( parent, name )
{
    setTextFormat( TQt::PlainText );
}

TQTextParagraph* TextEdit::paragraph()
{
    TQTextCursor *tc = new TQTextCursor( TQTextEdit::document() );
    return tc->paragraph();
}


MultiLineEditor::MultiLineEditor( bool call_static, bool richtextMode, TQWidget *parent, TQWidget *editWidget,
				  FormWindow *fw, const TQString &text )
    : MultiLineEditorBase( parent, 0, WType_Dialog | WShowModal ), formwindow( fw ), doWrap( false )
{
    callStatic = call_static;

    if ( callStatic )
	applyButton->hide();

    textEdit = new TextEdit( centralWidget(), "textedit" );
    Layout4->insertWidget( 0, textEdit );

    if ( richtextMode ) {
	TQPopupMenu *stylesMenu = new TQPopupMenu( this );
	menuBar->insertItem( tr( "&Styles" ), stylesMenu );

	basicToolBar = new TQToolBar( tr( "Basics" ), this, DockTop );

	ToolBarItem *it = new ToolBarItem( this, basicToolBar, tr( "Italic" ),
					   "i", TQPixmap::fromMimeSource( "designer_textitalic.png" ), CTRL+Key_I );
	it->addTo( stylesMenu );
	connect( it, TQ_SIGNAL( clicked( const TQString& ) ),
		 this, TQ_SLOT( insertTags( const TQString& )));

	ToolBarItem *b = new ToolBarItem( this, basicToolBar, tr( "Bold" ),
					  "b", TQPixmap::fromMimeSource( "designer_textbold.png" ), CTRL+Key_B );
	b->addTo( stylesMenu );
	connect( b, TQ_SIGNAL( clicked( const TQString& ) ),
		 this, TQ_SLOT( insertTags( const TQString& )));

	ToolBarItem *ul = new ToolBarItem( this, basicToolBar, tr( "Underline" ),
					   "u", TQPixmap::fromMimeSource( "designer_textunderline.png" ), CTRL+Key_U );
	ul->addTo( stylesMenu );
	connect( ul, TQ_SIGNAL( clicked( const TQString& ) ),
		 this, TQ_SLOT( insertTags( const TQString& )));

	ToolBarItem *tt = new ToolBarItem( this, basicToolBar, tr( "Typewriter" ),
					   "tt", TQPixmap::fromMimeSource( "designer_textteletext.png" ) );
	tt->addTo( stylesMenu );
	connect( tt, TQ_SIGNAL( clicked( const TQString& ) ),
		 this, TQ_SLOT( insertTags( const TQString& )));

	basicToolBar->addSeparator();

	TQPopupMenu *layoutMenu = new TQPopupMenu( this );
	menuBar->insertItem( tr( "&Layout" ), layoutMenu );

	TQAction *brAction = new TQAction( this );
	brAction->setIconSet( TQPixmap::fromMimeSource( "designer_textlinebreak.png" ) );
	brAction->setText( tr("Break" ) );
	brAction->addTo( basicToolBar );
	brAction->addTo( layoutMenu );
	connect( brAction, TQ_SIGNAL( activated() ) , this, TQ_SLOT( insertBR() ) );

	ToolBarItem *p = new ToolBarItem( this, basicToolBar, tr( "Paragraph" ),
					  "p", TQPixmap::fromMimeSource( "designer_textparagraph.png" ) );
	p->addTo( layoutMenu );
	connect( p, TQ_SIGNAL( clicked( const TQString& ) ),
		 this, TQ_SLOT( insertTags( const TQString& )));
	layoutMenu->insertSeparator();
	basicToolBar->addSeparator();

	ToolBarItem *al = new ToolBarItem( this, basicToolBar, tr( "Align left" ),
					   "p align=\"left\"", TQPixmap::fromMimeSource( "designer_textleft.png" ) );
	al->addTo( layoutMenu );
	connect( al, TQ_SIGNAL( clicked( const TQString& ) ),
		 this, TQ_SLOT( insertTags( const TQString& )));

	ToolBarItem *ac = new ToolBarItem( this, basicToolBar, tr( "Align center" ),
					   "p align=\"center\"", TQPixmap::fromMimeSource( "designer_textcenter.png" ) );
	ac->addTo( layoutMenu );
	connect( ac, TQ_SIGNAL( clicked( const TQString& ) ),
		 this, TQ_SLOT( insertTags( const TQString& )));

	ToolBarItem *ar = new ToolBarItem( this, basicToolBar, tr( "Align right" ),
					   "p align=\"right\"", TQPixmap::fromMimeSource( "designer_textright.png" ) );
	ar->addTo( layoutMenu );
	connect( ar, TQ_SIGNAL( clicked( const TQString& ) ),
		 this, TQ_SLOT( insertTags( const TQString& )));

	ToolBarItem *block = new ToolBarItem( this, basicToolBar, tr( "Blockquote" ),
					      "blockquote", TQPixmap::fromMimeSource( "designer_textjustify.png" ) );
	block->addTo( layoutMenu );
	connect( block, TQ_SIGNAL( clicked( const TQString& ) ),
		 this, TQ_SLOT( insertTags( const TQString& )));


	TQPopupMenu *fontMenu = new TQPopupMenu( this );
	menuBar->insertItem( tr( "&Font" ), fontMenu );

	fontToolBar = new TQToolBar( "Fonts", this, DockTop );

	TQAction *fontAction = new TQAction( this );
	fontAction->setIconSet( TQPixmap::fromMimeSource( "designer_textfont.png" ) );
	fontAction->setText( tr("Font" ) );
	fontAction->addTo( fontToolBar );
	fontAction->addTo( fontMenu );
	connect( fontAction, TQ_SIGNAL( activated() ) , this, TQ_SLOT( showFontDialog() ) );


	ToolBarItem *fp1 = new ToolBarItem( this, fontToolBar, tr( "Fontsize +1" ),
					    "font size=\"+1\"", TQPixmap::fromMimeSource( "designer_textlarger.png" ) );
	connect( fp1, TQ_SIGNAL( clicked( const TQString& ) ),
		 this, TQ_SLOT( insertTags( const TQString& )));

	ToolBarItem *fm1 = new ToolBarItem( this, fontToolBar, tr( "Fontsize -1" ),
					    "font size=\"-1\"", TQPixmap::fromMimeSource( "designer_textsmaller.png" ) );
	connect( fm1, TQ_SIGNAL( clicked( const TQString& ) ),
		 this, TQ_SLOT( insertTags( const TQString& )));

	ToolBarItem *h1 = new ToolBarItem( this, fontToolBar, tr( "Headline 1" ),
					   "h1", TQPixmap::fromMimeSource( "designer_texth1.png" ) );
	connect( h1, TQ_SIGNAL( clicked( const TQString& ) ),
		 this, TQ_SLOT( insertTags( const TQString& )));

	ToolBarItem *h2 = new ToolBarItem( this, fontToolBar, tr( "Headline 2" ),
					   "h2", TQPixmap::fromMimeSource( "designer_texth2.png"  ) );
	connect( h2, TQ_SIGNAL( clicked( const TQString& ) ),
		 this, TQ_SLOT( insertTags( const TQString& )));

	ToolBarItem *h3 = new ToolBarItem( this, fontToolBar, tr( "Headline 3" ),
					   "h3", TQPixmap::fromMimeSource( "designer_texth3.png" ) );
	connect( h3, TQ_SIGNAL( clicked( const TQString& ) ),
		 this, TQ_SLOT( insertTags( const TQString& )));

	TQPopupMenu *optionsMenu = new TQPopupMenu( this );
	menuBar->insertItem( tr( "O&ptions" ), optionsMenu );

	optionsToolBar = new TQToolBar( "Options", this, DockTop );
	wrapAction = new TQAction( this );
	wrapAction->setToggleAction( true );
	wrapAction->setIconSet( TQPixmap::fromMimeSource( "designer_wordwrap.png" ) );
	wrapAction->setText( tr( "Word Wrapping" ) );
	wrapAction->addTo( optionsToolBar );
	wrapAction->addTo( optionsMenu );
	connect( wrapAction, TQ_SIGNAL( toggled( bool ) ), this, TQ_SLOT( changeWrapMode( bool ) ) );

	oldDoWrap = doWrap;
	wrapAction->setOn( doWrap );

	connect( helpButton, TQ_SIGNAL( clicked() ), MainWindow::self, TQ_SLOT( showDialogHelp() ) );
	textEdit->document()->setFormatter( new TQTextFormatterBreakInWords );
	textEdit->document()->setUseFormatCollection( false );
	textEdit->document()->setPreProcessor( new SyntaxHighlighter_HTML );

	if ( !callStatic && ::tqt_cast<TQTextEdit*>(editWidget) ) {
	    mlined = (TQTextEdit*)editWidget;
	    mlined->setReadOnly( true );

	    const TQMetaProperty *wordWrap = mlined->metaObject()->property(
					    mlined->metaObject()->findProperty( "wordWrap", true ), true );
	    oldWrapMode = 0;
	    oldWrapString = "NoWrap";
	    if ( wordWrap ) {
		oldWrapMode = mlined->property( "wordWrap" );
		oldWrapString = TQString( wordWrap->valueToKey( oldWrapMode.toInt() ) );
		if ( oldWrapString != "NoWrap" )
		    doWrap = true;
	    }
	    textEdit->setAlignment( mlined->alignment() );
	    textEdit->setWordWrap( mlined->wordWrap() );
	    textEdit->setWrapColumnOrWidth( mlined->wrapColumnOrWidth() );
	    textEdit->setWrapPolicy( mlined->wrapPolicy() );
	    textEdit->setText( mlined->text() );
	    if ( !mlined->text().isEmpty() )
		textEdit->selectAll();
	} else {
	    textEdit->setText( text );
	    textEdit->selectAll();
	}
    } else {
	textEdit->setText( text );
	textEdit->selectAll();
    }
    textEdit->setFocus();
}

int MultiLineEditor::exec()
{
    res = 1;
    show();
    tqApp->enter_loop();
    return res;
}

void MultiLineEditor::okClicked()
{
    applyClicked();
    close();
}

void MultiLineEditor::applyClicked()
{
    if ( !callStatic ) {
	PopulateMultiLineEditCommand *cmd = new PopulateMultiLineEditCommand( tr( "Set the text of '%1'" ).arg( mlined->name() ),
						formwindow, mlined, textEdit->text() );
	cmd->execute();
	formwindow->commandHistory()->addCommand( cmd );

	if ( oldDoWrap != doWrap ) {
	    TQString pn( tr( "Set 'wordWrap' of '%2'" ).arg( mlined->name() ) );
	    SetPropertyCommand *propcmd;
	    if ( doWrap )
		propcmd = new SetPropertyCommand( pn, formwindow,
					mlined, MainWindow::self->propertyeditor(),
					"wordWrap", WidgetFactory::property( mlined, "wordWrap" ),
					TQVariant( 1 ), "WidgetWidth", oldWrapString );
	    else
		propcmd = new SetPropertyCommand( pn, formwindow,
					mlined, MainWindow::self->propertyeditor(),
					"wordWrap", WidgetFactory::property( mlined, "wordWrap" ),
					TQVariant( 0 ), "NoWrap", oldWrapString );

	    propcmd->execute();
	    formwindow->commandHistory()->addCommand( propcmd, true );
	}
	textEdit->setFocus();
    }
    else {
	staticText = textEdit->text();
    }
}

void MultiLineEditor::cancelClicked()
{
    res = 0;
    close();
}

void MultiLineEditor::closeEvent( TQCloseEvent *e )
{
    tqApp->exit_loop();
    MultiLineEditorBase::closeEvent( e );
}

void MultiLineEditor::insertTags( const TQString &tag )
{
    int pfrom, pto, ifrom, ito;
    TQString tagend(  tag.simplifyWhiteSpace() );
    tagend.remove( tagend.find( ' ', 0 ), tagend.length() );
    if ( textEdit->hasSelectedText() ) {
	textEdit->getSelection( &pfrom, &ifrom, &pto, &ito );
	TQString buf = textEdit->selectedText();
	buf = TQString( "<%1>%3</%2>" ).arg( tag ).arg( tagend ).arg( buf );
	textEdit->removeSelectedText();
	textEdit->insertAt( buf, pfrom, ifrom );
	textEdit->setCursorPosition( pto, ito + 2 + tag.length() );
    }
    else {
	int para, index;
	textEdit->getCursorPosition( &para, &index );
	textEdit->insert( TQString( "<%1></%2>" ).arg( tag ).arg( tagend ) );
	index += 2 + tag.length();
	textEdit->setCursorPosition( para, index  );
    }
}

void MultiLineEditor::insertBR()
{
    textEdit->insert( "<br>" );
}

void MultiLineEditor::showFontDialog()
{
    bool selText = false;
    int pfrom, pto, ifrom, ito;
    if ( textEdit->hasSelectedText() ) {
	textEdit->getSelection( &pfrom, &ifrom, &pto, &ito );
	selText = true;
    }
    RichTextFontDialog *fd = new RichTextFontDialog( this );
    if ( fd->exec() == TQDialog::Accepted ) {
	TQString size, font;
	if ( fd->getSize() != "0" )
	    size = "size=\"" + fd->getSize() + "\"";
	TQString color;
	if ( !fd->getColor().isEmpty() && fd->getColor() != "#000000" )
	    color = "color=\"" + fd->getColor() + "\"";
	if ( fd->getFont() != "default" )
	    font = "face=\"" + fd->getFont() + "\"";
	TQString tag( TQString( "font %1 %2 %3" )
	             .arg( color ).arg( size ).arg( font ) );

	if ( selText )
	    textEdit->setSelection( pfrom, ifrom, pto, ito );
	insertTags( tag.simplifyWhiteSpace() );
    }
    else if ( selText )
	textEdit->setSelection( pfrom, ifrom, pto, ito );
}

TQString MultiLineEditor::getStaticText()
{
    return staticText.stripWhiteSpace();
}

TQString MultiLineEditor::getText( TQWidget *parent, const TQString &text, bool richtextMode, bool *useWrap )
{
    MultiLineEditor medit( true, richtextMode, parent, 0, 0, text );
    if ( richtextMode )
	medit.setUseWrapping( *useWrap );
    if ( medit.exec() == TQDialog::Accepted ) {
	*useWrap = medit.useWrapping();
	return medit.getStaticText();
    }
    return TQString::null;
}

void MultiLineEditor::changeWrapMode( bool b )
{
    doWrap = b;
    if ( doWrap && !callStatic ) {
	if ( oldDoWrap )
	    textEdit->setProperty( "wordWrap", oldWrapMode );
	else
	    textEdit->setWordWrap( TQTextEdit::WidgetWidth );
    } else {
	textEdit->setWordWrap( TQTextEdit::NoWrap );
    }
}

bool MultiLineEditor::useWrapping() const
{
    return doWrap;
}

void MultiLineEditor::setUseWrapping( bool b )
{
    doWrap = b;
    wrapAction->setOn( doWrap );
}
